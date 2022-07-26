#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <json-c/json.h>
#include <curl/curl.h>
#include <libpq-fe.h>
#include "toml.h"
#include "amm_keys/amm_keys.h"
#include "broflovski/broflovski.h"

char* INSPECT_TX_URL[2] = {"https://public-api.solscan.io/transaction/", ""};
char* INSPECT_ACC_URL[2] = {"https://public-api.solscan.io/account/transactions?account=", "&limit=30"};
int INSPECT_ACC_LIMIT = 30;
char* DB_CREDS_AND_TARGET = "user=jojo dbname=l19_db";
char* USER_ACCOUNT_SLOT_FILL = "00000000000000000000000000000000000000000000";
char* OBFUSCATE_SIGNER_ACCOUNTS = 1; 
uint8_t SKIP_FAILED_TXS_DEFAULT = 1;

// --------------------------------------------------------------------------- CONTROL

void initialize_amm(struct amm* amm_data, char* amm_name, 
char* amm_key, char* amm_schema_type){
  strcpy(amm_data->amm_name, amm_name);
  strcpy(amm_data->amm_key, amm_key);
  amm_data->amm_schema_type = amm_schema_type;
  //assigned in assign_schema_keys()
  strcpy(amm_data->amm_schema, ""); 
  amm_data->table_created = 0;
  amm_data-> n_account_keys = 0;
  amm_data-> n_ignore = 0;
  // assigned in create_schema_table()
  strcpy(amm_data->table_name, ""); 
  
  assign_schema_keys(amm_data);
}

int launch_broflovski(char* config_file_path){
  struct amm amms_data[20];
  struct config config_data;
  load_config(*config_file_path, &config_data);
  
  PGconn *db_conn = PQconnectdb(DB_CREDS_AND_TARGET);
  if (PQstatus(db_conn) == CONNECTION_BAD){
    fprintf(stderr, "Couldn't db_connect to database. Error: %s\n",
      PQerrorMessage(db_conn));
    PQfinish(db_conn);
    exit(EXIT_FAILURE);
  }
  printf("db db_connected\n");
  for(uint8_t h = 0; h < config_data.n_amms_loaded; h++){ // TODO: multithreading
    //h = 5;
    initialize_amm(
      &amms_data[h],
      config_data.amm_names[h],
      config_data.amm_keys[h],
      config_data.amm_schema_types[h]
    );  
    // -- temp
      printf("TRACE --- AMM #%i:\n", h);
      printf("\tamm_name: '%s'\n", amms_data[h].amm_name);
      printf("\tamm_key: '%s'\n", amms_data[h].amm_key);
      printf("\tamm_schema_type: '%i'\n", amms_data[h].amm_schema_type);
      printf("\tamm_table_name: '%s'\n", amms_data[h].table_name);
      printf("\tamm_table_created: '%i'\n", amms_data[h].table_created);
      printf("\tamm_n_account_keys: '%i'\n", amms_data[h].n_account_keys);
      printf("\tamm_n_ignore: '%i'\n", amms_data[h].n_ignore);
      
      printf("\tignores: ");
      for(int zz = 0; zz < amms_data[h].n_ignore; zz++){
        printf("  %i ", amms_data[h].ignore[zz]);
      }
      printf("\n");
      for(int zz = 0; zz < amms_data[h].n_account_keys; zz++){
        printf("\t\tkey: '%s' , value: '%s'\n",
          amms_data[h].account_keys[zz],    
          amms_data[h].account_vals[zz]    
        );
      }
    // -- 
 
    char* txs[INSPECT_ACC_LIMIT];
    for(uint32_t i = 0; i < INSPECT_ACC_LIMIT; i++){
      txs[i] = NULL;
    }
    printf("\nFinding last %i Txs for amm_key: %s\n", INSPECT_ACC_LIMIT, amms_data->amm_key);
    sleep(2);
    pull_recent_account_txs(&amms_data[h], txs);
    printf("\nAnalyzing Txs\n\n");
    //uint32_t n_processed_pixs = 0;
    for (uint16_t i_txs = 0; i_txs < INSPECT_ACC_LIMIT; i_txs++){
      pull_and_store_target_accounts(&amms_data[h], txs[i_txs], db_conn);
    };
  } 
  PQfinish(db_conn);
  return 0;
}

void pull_recent_account_txs(struct amm* amm_data, char** grouped_tx_sigs){
  struct curl_buffer b;
  curl_buffer_init(&b);
  pull_json(&b, INSPECT_ACC_URL, amm_data->amm_key);
  
  struct json_object *parsed_json;

  uint32_t n_txs;
  //uint32_t i_txs;
  struct json_object *transactions;
  
  parsed_json = json_tokener_parse(b.ptr);
  n_txs = json_object_array_length(parsed_json);
  for (uint32_t i_txs = 0; i_txs < n_txs; i_txs++){
    struct json_object *transaction;
    struct json_object *tx_sig;

    transaction = json_object_array_get_idx(parsed_json, i_txs);
    json_object_object_get_ex(transaction, "txHash", &tx_sig);
    if(tx_sig == NULL){
      continue;
    }
    char* hash = json_object_get_string(tx_sig);
    grouped_tx_sigs[i_txs] = realloc(hash, strlen(hash) + 1);
    printf("%i. hash: %s\n", i_txs, grouped_tx_sigs[i_txs]); 
    free(tx_sig);
  }
  free(b.ptr);
}

void pull_and_store_target_accounts(struct amm* amm_data, char* tx_sig, 
    PGconn* db_conn){ 
  // fetch tx json
  printf("//// TRACE --- START. Tx Sig: %s\n", tx_sig);
  struct curl_buffer b;
  curl_buffer_init(&b);
  pull_json(&b, INSPECT_TX_URL, tx_sig);
  
  // isolate reqd accounts 
  struct json_object *parsed_json;
  parsed_json = json_tokener_parse(b.ptr);

  // only pulls accounts from successful txs
  struct json_object *json_status;
  json_object_object_get_ex(parsed_json, "status", &json_status);
  char* status = json_object_get_string(json_status);
  if (strcmp(status, "Success") != 0 && SKIP_FAILED_TXS_DEFAULT != 0){
    printf("Tx marked \"Fail\". Checking the next.\n");
    printf("--------------------------------------------------------------------------\n");
    //return; 
  }
  
  struct json_object *inner_instructions; 
  json_object_object_get_ex(parsed_json, "innerInstructions", &inner_instructions); 
  uint32_t n_ixs = json_object_array_length(inner_instructions);
  
  for(uint8_t i_ixs = 0; i_ixs < n_ixs; i_ixs++){
    struct json_object *inner_instruction = json_object_array_get_idx(inner_instructions, i_ixs);
    struct json_object *parsed_instructions;
    json_object_object_get_ex(inner_instruction, "parsedInstructions", &parsed_instructions); 

    uint32_t n_pixs = json_object_array_length(parsed_instructions);
    for (uint32_t i_pixs = 0; i_pixs < n_pixs; i_pixs++){
      struct json_object *parsed_instruction = 
        json_object_array_get_idx(parsed_instructions, i_pixs );
      
      // checking for matching amm key
      struct json_object *program_id;
      json_object_object_get_ex(parsed_instruction, "programId", &program_id);
      char* program_id_s = json_object_get_string(program_id);
      if (strcmp(program_id_s, amm_data->amm_key) != 0){
        free(parsed_instruction);
        free(program_id);
        //amm_reset(amm_data); no longer reqd.
        printf("TRACE ---- Non matching program_id\n");
        printf("--------------------------------------------------------------------------\n");
        continue;
      }
      // extract amm owned accounts
      struct json_object *params;
      json_object_object_get_ex(parsed_instruction, "params", &params);
      
      printf("\nPARSER: finding relevant accounts in tx: \n\t%s\n", tx_sig); 
      for (uint32_t i_account_keys = 0; i_account_keys < amm_data->n_account_keys; i_account_keys++){
        struct json_object *pool_public_key;
        json_object_object_get_ex(params, &amm_data->account_keys[i_account_keys], &pool_public_key);
        bool ignore_account = false;
        // if pubkey is owned by the signer insert USER_ACCOUNT_SLOT_FILL 
        for(uint8_t j = 0; j < amm_data->n_ignore; j++){
          if (i_account_keys == amm_data->ignore[j]) { ignore_account = true;} 
        }
        if (ignore_account && OBFUSCATE_SIGNER_ACCOUNTS){
          strcpy(amm_data->account_vals[i_account_keys], USER_ACCOUNT_SLOT_FILL);
        } else {
          if (json_object_get_string(pool_public_key) == NULL){
            printf("PARSER: NULL key value detected. Skipping tx_sig: \n\t%s\n", tx_sig); 
            continue;
          };
          strcpy(amm_data->account_vals[i_account_keys], json_object_get_string(pool_public_key));
        }
        free(pool_public_key);
        pool_public_key = NULL;
      }
      // show parsed data
      printf("PARSER: Parsed accounts\n"); 
      for (uint8_t i_kk = 0; i_kk < amm_data->n_account_keys; i_kk++){
        printf("\t%s : %s\n", amm_data->account_keys[i_kk], amm_data->account_vals[i_kk]);
      }; 
      // store to db
      store_to_db(db_conn, amm_data);
      amm_data->table_created = 1; // -------------------- cont'd, run for issue
      printf("--------------------------------------------------------------------------\n");
    }
  }
}

// --------------------------------------------------------------------------- CONFIG

load_config(char* config_file_path, struct config* config_data){
  FILE* file;
  char errbuf[200];
  //file = fopen(&config_file_path, "r"); //TODO: FIX! 
  file = fopen("/home/jojo/current_focus/public/L19_Broflovski/unique_data/config.toml", "r");
  if (!file){
    fprintf(stderr, "Could not reach config file. Ending program.\n");
    exit(EXIT_FAILURE);
  }
  toml_table_t* config = toml_parse_file(file, errbuf, sizeof(errbuf));
  fclose(file);

  if (!config){
    fprintf(stderr, "could not process config file; e: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }

  toml_array_t* swap_arr = toml_array_in(config, "swap"); 
  uint16_t n_amms = toml_array_nelem(swap_arr);
  uint16_t n_loaded = 0;
  printf("\nCONFIG: Found %i AMM keys.\n", n_amms);
  for(uint16_t i_amm = 0; i_amm < n_amms; i_amm++){
    toml_table_t* swap = toml_table_at(swap_arr, i_amm);
    toml_datum_t swap_fields[4] = { 
      toml_string_in(swap, "name"),
      toml_string_in(swap, "key"),
      toml_int_in(swap, "schema_type"),
      toml_bool_in(swap, "ready"),
    };
    uint8_t ok_to_proceed = 1;
    for(uint16_t i = 0; i < 4; i++){
      if (!swap_fields[i].ok){
        ok_to_proceed = 0; 
      } 
    }
    if (!ok_to_proceed){
      printf("CONFIG: Failed to load AMM at index %i\n\n", i_amm); 
    } else {
      if(!swap_fields[3].u.b){
        continue;
      }
      strcpy(config_data->amm_names[i_amm], swap_fields[0].u.s);
      strcpy(config_data->amm_keys[i_amm], swap_fields[1].u.s);
      config_data->amm_schema_types[i_amm] = swap_fields[2].u.i;
      n_loaded++;
      printf("CONFIG: AMM at index %i loaded\n", i_amm);
      printf("\tname: %s\n", config_data->amm_names[i_amm]);
      printf("\tkey : %s\n", config_data->amm_keys[i_amm]);
    }
  }
  config_data->n_amms_loaded = n_loaded;
  printf("%i/%i successfully loaded.\n", n_loaded, n_amms);
}

// --------------------------------------------------------------------------- POSTGRESQL

char* DB_COMMANDS[2] = {
  "SELECT * FROM ",
  "INSERT INTO ",
};

char* TABLE_SEP[2] = {
  "c_amm_schema_",
  ".table_",
};

void create_schema_table(PGconn* db_conn, struct amm* amm_data){

    if (PQstatus(db_conn) == CONNECTION_BAD){
      fprintf(stderr, "db connection broken. Error: %s\n",
        PQerrorMessage(db_conn));
      PQfinish(db_conn);
      exit(EXIT_FAILURE);
    }
    // create schema
    char db_command[1000] = "";
    strcat(db_command, "CREATE SCHEMA ");
    strcat(db_command, TABLE_SEP[0]);
    strcat(db_command, amm_data->amm_schema);
    printf("DB COMMAND: \n\t%s\n", db_command);

    PQexec(db_conn, db_command);
    
    // name table
    strcat(amm_data->table_name, TABLE_SEP[0]);
    strcat(amm_data->table_name, amm_data->amm_schema);
    strcat(amm_data->table_name, TABLE_SEP[1]);
    strcat(amm_data->table_name, amm_data->amm_name);

    // create table
    strcpy(db_command, "");
    strcat(db_command, "CREATE TABLE ");
    strcat(db_command, amm_data->table_name);
    strcat(db_command, " (");
    for(uint8_t i = 0; i < amm_data->n_account_keys; i++){
      strcat(db_command, "\"");
      strcat(db_command, amm_data->account_keys[i]);
      strcat(db_command, "\" VARCHAR"); // adjust: limit to 44 chars.
      if (i + 1!= amm_data->n_account_keys){
        strcat(db_command, ", "); // adjust: limit to 44 chars.
      }
    } 
    strcat(db_command, ");");
    printf("DB COMMAND: \n\t%s\n", db_command);
    PQexec(db_conn, db_command);
}

void store_to_db(PGconn* db_conn, struct amm* amm_data){
  if (PQstatus(db_conn) == CONNECTION_BAD){
    fprintf(stderr, "Couldn't connect to database. Error: %s\n",
      PQerrorMessage(db_conn));
    PQfinish(db_conn);
    exit(EXIT_FAILURE);
  }
  
  if (amm_data->table_created < 1){
    create_schema_table(db_conn, &*amm_data);
  }
  for(uint8_t h = 0; h < 2; h++){
    char db_command[2000] = "";
    strcat(db_command, DB_COMMANDS[h]);
    strcat(db_command, amm_data->table_name);
    if (h == 1){ 
      strcat(db_command, " VALUES(");
      for(uint8_t i = 0; i < amm_data->n_account_keys; i++){
        strcat(db_command, "'");
        strcat(db_command, amm_data->account_vals[i]);
        if (i == (amm_data->n_account_keys - 1)){
          strcat(db_command, "')");
          break;
        }
        strcat(db_command, "',");
      } 
    }
    strcat(db_command, ";");
    printf("DB COMMAND: \n\t%s\n", db_command);
    PGresult* res = PQexec(*&db_conn, db_command);
    switch(h){
      case 0:
        if (PQresultStatus(res) != PGRES_TUPLES_OK){
          printf("Table is empty\n");
          break;
        }  
        uint32_t rows = PQntuples(res);
        uint8_t* columns = amm_data->n_account_keys;
        for(uint8_t r = 0; r < rows; r++){
          uint8_t* n_matching_keys = 0;
          for (uint8_t c = 0; c < columns; c++){
            char* current_cell = PQgetvalue(res, r, c); 
            if (strcmp(amm_data->account_vals[c], current_cell) == 0){
              n_matching_keys++;
              //printf("**////// current n_matching_keys value: %i\n", n_matching_keys);
            }
            current_cell = NULL;
          } 
          if (n_matching_keys >= amm_data->n_account_keys){
            printf("\nDB RESULT: Duplicate detected, entry cancelled\n\n");
            h = 3;
          }
        }
        break;
      case 1:
        if (PQresultStatus(res)!= PGRES_COMMAND_OK){
          printf("\nDB RESULT: Failed to add entry to db; e: %s\n\n", 
              PQerrorMessage(db_conn));
        } else {
          printf("\nDB RESULT: entry successful\n\n");
        }
        break;
    };
    PQclear(res);
  }
}

// --------------------------------------------------------------------------- CURL

// initialize buffer
void curl_buffer_init(struct curl_buffer *b){
  (*b).len = 0;
  (*b).ptr = malloc((*b).len+1);
  if ((*b).ptr == NULL){
    fprintf(stderr, "malloc failure at buffer_init");
    exit(EXIT_FAILURE);
  }
  (*b).ptr[0] = '\0';
}

// curl write to buffer callback fn
static uint32_t curl_buffer_write(char *ptr, uint32_t size, uint32_t nmemb, struct curl_buffer *b){
  uint32_t new_len = (*b).len + size*nmemb;
  (*b).ptr = realloc((*b).ptr, new_len+1);
  if ((*b).ptr == NULL){
    fprintf(stderr, "CURL buffer realloc failure at curl_buffer_write");
    exit(EXIT_FAILURE);
  }
  memcpy((*b).ptr + (*b).len, ptr, size*nmemb);
  (*b).ptr[new_len] = '\0';
  (*b).len = new_len;
  return (size*nmemb); 
}

// solscan json fetch
void pull_json(struct curl_buffer *b, char** base, char* target){
  char* url = concatenate_url(base, target);
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if(curl){
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_buffer_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &*b);
    res = curl_easy_perform(curl);
  }
}

// caller needs checker.
char* concatenate_url(char** static_s, char* param_s){
  const uint32_t first_len = strlen(static_s[0]);
  const uint32_t second_len = strlen(param_s);
  const uint32_t third_len = strlen(static_s[1]);
  char *concatenated_s = malloc(first_len + second_len + third_len + 1);
  if (concatenated_s == NULL){
    fprintf(stderr, "URL malloc failure. Ending program.\n");
    exit(EXIT_FAILURE);
  }
  memcpy(concatenated_s, static_s[0], first_len);
  memcpy(concatenated_s + first_len, param_s, second_len);
  memcpy(concatenated_s + first_len + second_len, static_s[1], third_len + 1);
  
  return concatenated_s;
}
