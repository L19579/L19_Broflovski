#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <json-c/json.h>
#include <curl/curl.h>
#include <libpq-fe.h>
#include "broflovski/broflovski.h"
#include "prompt/prompt.h"
#include "toml.h"

char* CONFIG_PATH = "/home/jojo/current_focus/public/L19_Broflovski/unique_data/config.toml";
char* INSPECT_TX_URL[2] = {"https://public-api.solscan.io/transaction/", ""};
char* INSPECT_ACC_URL[2] = {"https://public-api.solscan.io/account/transactions?account=", "&limit=210"};
int INSPECT_ACC_LIMIT = 210;
char* DB_CREDS_AND_TARGET = "user=jojo dbname=new_test_db_1";
char* USER_ACCOUNT_SLOT = "00000000000000000000000000000000000000000000";

typedef enum{
  Orca,
  OrcaV2,
  Stepn,
  Saros,
  Solana,
  Saber,
  Step,
  Lifinity,
  Cykura,     // These need reviewing
  Whirlpool,  //+1
  Aldrin,
  Serum,
} SwapType;

struct amm{
  char* amm_name;
  char* db_schema;
  char* db_table;
  char* accounts_key[20];
  char* accounts_val[20];
  int n_account_keys;
  uint8_t** ignore;
  uint8_t n_ignore;
};

uint8_t launch_broflovski(){
  PGconn *db_conn = PQconnectdb(DB_CREDS_AND_TARGET);
  if (PQstatus(db_conn) == CONNECTION_BAD){
    fprintf(stderr, "Couldn't db_connect to database. Error: %s\n",
      PQerrorMessage(db_conn));
    PQfinish(db_conn);
    exit(EXIT_FAILURE);
  }
  printf("db db_connected\n");
  char* amm_key = "SSwapUtytfBdBn1b9NUGG6foMVPtcWgpRU32HToDUZr";
  char* txs[INSPECT_ACC_LIMIT];
  for(size_t i = 0; i < INSPECT_ACC_LIMIT; i++){
    txs[i] = NULL;
  }
  printf("\nFinding last %i Txs for amm_key: %s\n", INSPECT_ACC_LIMIT, amm_key);
  sleep(2);
  pull_recent_account_txs(amm_key, txs);
  printf("\nAnalyzing Txs\n\n");
  for (uint8_t i = 0; i < INSPECT_ACC_LIMIT; i++){
    printf("--------------------------------------------------------------------------\n");
    pull_and_store_target_accounts(amm_key, txs[i], Saros, db_conn);
  };
  PQfinish(db_conn);
  return 0;
}


// buffer
struct curl_buffer{
  char *ptr;
  size_t len;
};

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
static size_t curl_buffer_write(char *ptr, size_t size, size_t nmemb, struct curl_buffer *b){
  size_t new_len = (*b).len + size*nmemb;
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
  const size_t first_len = strlen(static_s[0]);
  const size_t second_len = strlen(param_s);
  const size_t third_len = strlen(static_s[1]);
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

void pull_recent_account_txs(char* key, char** grouped_tx_sigs){
  struct curl_buffer b;
  curl_buffer_init(&b);
  pull_json(&b, INSPECT_ACC_URL, key);
  
  struct json_object *parsed_json;

  size_t n_txs;
  //size_t i_txs;
  struct json_object *transactions;
  
  parsed_json = json_tokener_parse(b.ptr);
  n_txs = json_object_array_length(parsed_json);
  for (size_t i_txs = 0; i_txs < n_txs; i_txs++){
    struct json_object *transaction;
    struct json_object *tx_sig;

    transaction = json_object_array_get_idx(parsed_json, i_txs);
    json_object_object_get_ex(transaction, "txHash", &tx_sig);
    char* hash = json_object_get_string(tx_sig);
    grouped_tx_sigs[i_txs] = realloc(hash, strlen(hash) + 1);
    printf("%i. hash: %s\n", i_txs, grouped_tx_sigs[i_txs]); 
    free(tx_sig);
  }
  free(b.ptr);
}

void pull_and_store_target_accounts(char* amm_key, char* tx_sig, 
    SwapType swap_type, PGconn *db_conn){
  // fetch tx json
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
  if (strcmp(status, "Success") != 0){
    printf("Tx marked \"Fail\". Checking the next.\n");
    return; 
  }
   
  struct json_object *inner_instructions; 
  json_object_object_get_ex(parsed_json, "innerInstructions", &inner_instructions); 
  size_t n_ixs = json_object_array_length(inner_instructions);
  
  for(uint8_t i_ixs = 0; i_ixs < n_ixs; i_ixs++){
    struct json_object *inner_instruction = json_object_array_get_idx(inner_instructions, i_ixs);
    struct json_object *parsed_instructions;
    json_object_object_get_ex(inner_instruction, "parsedInstructions", &parsed_instructions); 

    size_t n_pixs = json_object_array_length(parsed_instructions);
    for (size_t i_pixs = 0; i_pixs < n_pixs; i_pixs++){
      struct json_object *parsed_instruction = 
        json_object_array_get_idx(parsed_instructions, i_pixs );
      
      // checking for matching amm key
      struct json_object *program_id;
      json_object_object_get_ex(parsed_instruction, "programId", &program_id);
      char* program_id_s = json_object_get_string(program_id);
      if (strcmp(program_id_s, amm_key) != 0){
        free(parsed_instruction);
        free(program_id);
        continue;
      }
      // extract amm owned accounts
      struct json_object *params;
      json_object_object_get_ex(parsed_instruction, "params", &params);
      
      struct amm amm_data;
      char** account_json_keys;
      
      switch (swap_type){
        case Saber:
          amm_data.amm_name = "saber";
          //printf("TRACE --- matched with Saber\n");
          amm_data.n_account_keys = 9;  
          amm_data.ignore = (uint8_t *[3]){2, 3, 6};
          amm_data.n_ignore = 3;
          account_json_keys = (char*[9]){
            "Swap Account",
            "Authority",
            "User Authority",
            "User Source",
            "Pool Source",
            "Pool Destination",
            "User Destination",
            "Admin Destination",
            "TOKEN_PROGRAM_ID",
          };
          break;
        case Step:
          amm_data.amm_name = "step";
          //printf("TRACE --- matched with Step\n");
          amm_data.n_account_keys = 12;  
          amm_data.ignore = (uint8_t *[3]){2, 3, 6};
          amm_data.n_ignore = 3;
          account_json_keys = (char*[12]){
            "Token Swap",
            "Authority",
            "User Transfer Authority",
            "User Source",
            "Pool Source",
            "Pool Destination",
            "User Destination",
            "Pool Mint",
            "Fee Account",
            "Refund To",
            "TOKEN_PROGRAM_ID",
          };
          break;
        case Lifinity:
          amm_data.amm_name = "lifinity";
          //printf("TRACE --- matched with Lifinity\n");
          amm_data.n_account_keys = 13;  
          amm_data.ignore = (uint8_t *[3]){2, 3, 4};
          amm_data.n_ignore = 3;
          account_json_keys = (char*[13]){
            "Account0",
            "Account1",
            "Account2",
            "Account3",
            "Account4",
            "Account5",
            "Account6",
            "Account7",
            "Account8",
            "Account9",
            "Account10",
            "Account11",
            "Account12",
          };
          break;
        case Whirlpool:
          //printf("TRACE --- matched with Whirlpool\n");
          /*
          amm_name = "whirlpool";
          account_json_keys = (char**){
            "Account0",
            "Account1",
            "Account2",
            "Account3",
            "Account4",
            "Account5",
            "Account6",
            "Account7",
            "Account8",
            "Account9",
            "Account10",
          };
          */
          fprintf(stderr, "Swap type set to whirlpool; not yet intergrated. Exiting program\n");
          exit(EXIT_FAILURE); // Will be removed.
        case Aldrin:
          amm_data.amm_name = "aldrin";
          printf("TRACE --- matched with Aldrin\n");
          amm_data.n_account_keys = 11;  
          amm_data.ignore = (uint8_t *[3]){6, 7, 8};
          amm_data.n_ignore = 3;
          account_json_keys = (char*[11]){
            "Pool Public Key",
            "Pool Signer",
            "Pool Mint",
            "Base Token Vault",
            "Quote Token Vault",
            "Fee Pool Token Account",
            "Wallet Authority",
            "User Base Token Account",
            "User Quote Token Account",
            "curve",
            "TOKEN_PROGRAM_ID",
          };
          break;
        case Cykura:
          //printf("TRACE --- matched with Cykura\n");
          /*
          amm_name = "cykura";
          account_json_keys = (char**){
            "Account0",
            "Account1",
            "Account2",
            "Account3",
            "Account4",
            "Account5",
            "Account6",
            "Account7",
            "Account8",
            "Account9",
            "Account10",
            "Account11",
          };
          */
          fprintf(stderr, "Swap type set to cykura; not yet intergrated. Exiting program\n");
          exit(EXIT_FAILURE); // -------------------
        case Serum:
          //printf("TRACE --- matched with Serum\n");
          amm_data.amm_name = "serum";
          amm_data.n_account_keys = 16;  
          amm_data.ignore = (uint8_t *[3]){1, 6, 7};
          amm_data.n_ignore = 3;
          account_json_keys = (char*[16]){
            "Market > Market",
            "Market > Open Orders",
            "Market > Request Queue",
            "Market > Event Queue",
            "Market > Bids",
            "Market > Asks",
            "Market > Order Payer Token Account",
            "Market > Coin Vault",
            "Market > Pc Vault",
            "Market > Vault Signer",
            "Market > Coin Wallet",
            "Authority",
            "Pc Wallet",
            "Dex Program",
            "Token Program",
            "Rent", //sysvar rent acc.
          };
          break;
        //orca, orca_v2, stepn, saros, and solana
        case Stepn:
          //printf("TRACE --- matched with Stepn\n");
          amm_data.amm_name = "stepn";
          amm_data.n_account_keys = 10;  
          amm_data.ignore = (uint8_t *[3]){2, 3, 6};
          amm_data.n_ignore = 3;
          account_json_keys = (char*[10]){
            "Account0",
            "Account1",
            "Account2",
            "Account3",
            "Account4",
            "Account5",
            "Account6",
            "Account7",
            "Account8",
            "Account9",
          };
          break;
        case Saros:
          //printf("TRACE --- matched with Saros\n");
          amm_data.amm_name = "saros";
          amm_data.n_account_keys = 10;  
          amm_data.ignore = (uint8_t *[3]){2, 3, 6};
          amm_data.n_ignore = 3;
          account_json_keys = (char*[10]){
            "Swap",
            "Authority",
            "User Transfer Authority",
            "Source",
            "Swap Source",
            "Swap Destination",
            "Destination",
            "Pool Mint",
            "Pool Fee",
            "TOKEN_PROGRAM_ID",
          };
          break;
        case Orca:
        case OrcaV2:
        case Solana:
          switch(swap_type){
            case Orca:
              amm_data.amm_name = "orca";
              break;
            case OrcaV2:
              amm_data.amm_name = "orca_v2";
              break;
            case Solana:
              amm_data.amm_name = "solana";
              break;
          };
          //printf("TRACE --- matched with Orcas, Solana\n");
          amm_data.n_account_keys = 10;  
          amm_data.ignore = (uint8_t *[3]){2, 3, 6};
          amm_data.n_ignore = 3;
          account_json_keys = (char*[10]){
            "Token Swap",
            "Authority",
            "User Transfer Authority",
            "User Source",
            "Pool Source",
            "Pool Destination",
            "User Destination",
            "Pool Mint",
            "Fee Account",
            "TOKEN_PROGRAM_ID",
          };
          break;
        default: 
          fprintf(stderr, "Invalid SwapType\n");
          exit(EXIT_FAILURE);
      }
      printf("\nPARSER: finding relevant accounts in tx: \n\t%s\n", tx_sig); 
      for (size_t i_account_keys = 0; i_account_keys < amm_data.n_account_keys; i_account_keys++){
        struct json_object *pool_public_key;
        json_object_object_get_ex(params, account_json_keys[i_account_keys], &pool_public_key);
        amm_data.accounts_key[i_account_keys] = account_json_keys[i_account_keys];
        bool ignore_account = false;
        // if pubkey is owned by the signer insert USER_ACCOUNT_SLOT 
        for(uint8_t j = 0; j < amm_data.n_ignore; j++){
          if (i_account_keys == amm_data.ignore[j]) { ignore_account = true;} 
        }
        if (ignore_account){
          amm_data.accounts_val[i_account_keys] = USER_ACCOUNT_SLOT;
        } else {
          amm_data.accounts_val[i_account_keys] = json_object_get_string(pool_public_key);
          if (amm_data.accounts_val[i_account_keys] == NULL){
            printf("PARSER: NULL key value detected. Skipping tx_sig: \n\t%s\n", tx_sig); 
            return;
          };
        }
        free(pool_public_key);
        pool_public_key = NULL;
      }
      // show parsed data
      printf("PARSER: Parsed accounts\n"); 
      for (uint8_t i_kk = 0; i_kk < amm_data.n_account_keys; i_kk++){
        printf("\t%s : %s\n", amm_data.accounts_key[i_kk], amm_data.accounts_val[i_kk]);
      }; 
      // store to db
      store_to_db(db_conn, amm_key, amm_data);
    }
  }
}

void store_to_db(PGconn* db_conn,char* amm_key, struct amm amm_data){
  if (PQstatus(db_conn) == CONNECTION_BAD){
    fprintf(stderr, "Couldn't connect to database. Error: %s\n",
      PQerrorMessage(db_conn));
    PQfinish(db_conn);
    exit(EXIT_FAILURE);
  }
  // This fn does NOT fill for missing schemas. Future impl.
  char** db_commands = (char* [3]){
    // create table if it doesn't exist
    "CREATE TABLE c_saros_schema.test_table_",
    // cancel if duplicate
    "SELECT * FROM c_saros_schema.test_table_",
    // table insert
    "INSERT INTO c_saros_schema.test_table_",
  };
  for(uint8_t h = 1; h < 3; h++){
    char db_command[1000] = "";
    strcat(db_command, db_commands[h]);
    strcat(db_command, amm_data.amm_name);
    if (h == 2){
      strcat(db_command, " VALUES(");
      for(uint8_t i = 0; i < amm_data.n_account_keys; i++){
        strcat(db_command, "'");
        strcat(db_command, amm_data.accounts_val[i]);
        if (i == (amm_data.n_account_keys - 1)){
          strcat(db_command, "')");
          break;
        }
        strcat(db_command, "',");
      } 
    }
    strcat(db_command, ";");
    printf("DB COMMAND: \n\t%s\n", db_command);
    PGresult* res = PQexec(db_conn, db_command);
    switch(h){
      case 0:
        if (PQresultStatus(res) == PGRES_COMMAND_OK){
          printf("Table does not exist, creating one now.");
        }
        break;
      case 1:
        if (PQresultStatus(res) != PGRES_TUPLES_OK){
          printf("Table is empty\n");
          break;
        }  
        size_t rows = PQntuples(res);
        uint8_t* columns = amm_data.n_account_keys;
        //printf("TRACE --- printing table's values: \n");
        for(uint8_t r = 0; r < rows; r++){
          uint8_t* n_matching_keys = 0;
          //printf("\t"); 
          for (uint8_t c = 0; c < columns; c++){
            char* current_cell = PQgetvalue(res, r, c); 
            if (strcmp(amm_data.accounts_val[c], current_cell) == 0){
							//printf("\n\nFound matching strings: \n%s\n%s\n\n",
							//	amm_data.accounts_val[c], current_cell);
              n_matching_keys++;
              //printf("**////// current n_matching_keys value: %i\n", n_matching_keys);
            }
            //printf("%s | ", PQgetvalue(res, r, c)); 
            current_cell = NULL;
          } 
          //printf("\n"); 
					//printf("n_matching_keys: %i, n_account_keys: %i\n",
					//	(n_matching_keys - amm_data.n_ignore), (amm_data.n_account_keys - amm_data.n_ignore));
          if (n_matching_keys >= amm_data.n_account_keys){
            printf("DB RESULT: Duplicate detected, entry cancelled\n\n");
            //n_matching_keys = 0;
            //h = 3;
            PQclear(res);
            return;
          }
        }
        break;
      case 2:
        if (PQresultStatus(res)!= PGRES_COMMAND_OK){
          printf("DB RESULT: Failed to add entry to db\n");
        }
printf("TRACE --- store_to_db -- 16\n");
        printf("DB RESULT: entry successful\n\n");
        break;
      default:
        break;
    };
    PQclear(res);
  }
}
