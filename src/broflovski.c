#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <json-c/json.h>
#include <curl/curl.h>
#include <libpq-fe.h>
#include "broflovski/broflovski.h"
#include "prompt/prompt.h"
#include "toml.h"

char *CONFIG_PATH = "/home/jojo/current_focus/public/L19_Broflovski/unique_data/config.toml";
char *INSPECT_TX_URL[2] = {"https://public-api.solscan.io/transaction/", ""};
char *INSPECT_ACC_URL[2] = {"https://public-api.solscan.io/account/transactions?account=", "&limit=200"};

typedef enum{
 /* This covers orca, orca_v2, stepn, saros, and solana,
  *  0. swap
  *  1. swap authority
  -  2. user signer
  -  3. user source
  *  4. pool source
  *  5. pool destination
  -  6. user destination
  *  7. pool mint
  *  8. pool fee account
  *  9. token program
  */ 
  Orca,
  OrcaV2,
  Stepn,
  Saros,
  Solana,
  /*
  *  0. swap
  *  1. swap authority
  -  2. user signer
  -  3. user source
  *  4. pool source
  *  5. pool destination
  -  6. user destination
  *  7. admin destination
  *  8. token account
  */
  Saber,
  /*
   *  0. swap
   *  1. swap authority
   -  2. user signer
   -  3. user source
   *  4. pool source
   *  5. pool destination
   -  6. user destination
   *  8. pool mint
   *  9. pool fee account
   * 10. refund to (set to token program)
   * 11. token program
   */
  Step,
  /*
   *  0. authority
   *  1. amm
   -  2. user signer
   -  3. user source (source info)
   -  4. user destination (destination info)
   *  5. pool source
   *  6. pool destination
   *  7. pool mint
   *  8. pool fee account
   *  9. token program
   * 10. pyth account
   * 11. pyth pc account
   * 12. config account
   */
  Lifinity,
  /*
   *  0. token program
   *  1. token authority
   *  2. whirlpool
   -  3. user source
   *  4. pool source
   -  5. user destination
   *  6. pool destination
   *  7. tick 0(?)
   *  8. tick 1(?
   *  9. tick 3(?)
   * 10. oracle
   */
  Cykura, // These need reviewing
  Whirlpool, //+1
  /*
   *  0. pool
   *  1. pool signer
   *  2. pool mint
   *  3. base token vault
   *  4. quote token vault
   *  5. fee pool token account
   -  6. user signer
   -  7. user base token account
   -  8. use quote token account
   *  9. curve
   * 10. token program
   *
   */
  Aldrin,
  /*
   *  0. market
   -  1. open orders
   *  2. request queue
   *  3. event queue
   *  4. bids
   *  5. asks
   -  6. user source (payer)
   -  7. user signer (owner)
   *  8. coin vault
   *  9. pc vault
   */
  Serum,
} SwapType;

struct tuple{
  char* a;
  char* b;
};

uint8_t launch_broflovski(char* file_path){
  char* account_key = "BE3G2F5jKygsSNbPFKHHTxvKpuFXSumASeGweLcei6G3";
  char* txs[200];
  for(size_t i = 0; i < 200; i++){
    txs[i] = NULL;
  }
  pull_recent_account_txs(account_key, txs);
  printf("\nrechecking values: \n");
  for(size_t i = 0 ; i < 200 ; i++){
    if (txs[i] == NULL){
      printf("%i. hash: NULL\n", i);
      continue;
    }
    printf("%i. hash: %s\n", i, txs[i]);
  }

  bool test_res = 
    pull_and_store_target_accounts("2SJRnmjJSNkVy75mkZzW2WLQBHvtDD7Aj6jfsF9Y7bYgZQvkEjfaAUUDhEQY6ph66yvqhCSSALro4U5BDHedNn1K", Solana);
  return 0;
}


// buffer
struct CurlBuffer{
  char *ptr;
  size_t len;
};

// initialize buffer
void curl_buffer_init(struct CurlBuffer *b){
  (*b).len = 0;
  (*b).ptr = malloc((*b).len+1);
  if ((*b).ptr == NULL){
    fprintf(stderr, "malloc failure at buffer_init");
    exit(EXIT_FAILURE);
  }
  (*b).ptr[0] = '\0';
}

// curl write to buffer callback fn
static size_t curl_buffer_write(char *ptr, size_t size, size_t nmemb, struct CurlBuffer *b){
  size_t new_len = (*b).len + size*nmemb;
  (*b).ptr = realloc((*b).ptr, new_len+1);
  if ((*b).ptr == NULL){
    fprintf(stderr, "realloc failure at curl_buffer_write");
    exit(EXIT_FAILURE);
  }
  memcpy((*b).ptr + (*b).len, ptr, size*nmemb);
  (*b).ptr[new_len] = '\0';
  (*b).len = new_len;
  //printf("buffer length: %i\n", new_len);
  return (size*nmemb); 
}

// solscan json fetch
void pull_json(struct CurlBuffer *b, char** base, char* target){
  char* url = concatenate_url(base, target);
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if(curl){
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_buffer_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &*b);
    res = curl_easy_perform(curl);
    
    //sleep(5);
    //printf("Data:\n%s\n", (*b).ptr);
  }
}
  

void load_config(char** dex_configs){
 
}

uint8_t read_toml(char* file_path){

  return 0;
}

uint8_t write_toml(char* file_path){
  
  return 0;
}

// caller needs checker.
char* concatenate_url(char** static_s, char* param_s){
  const size_t first_len = strlen(static_s[0]);
  const size_t second_len = strlen(param_s);
  const size_t third_len = strlen(static_s[1]);
  char *concatenated_s = malloc(first_len + second_len + third_len + 1);
  if (concatenated_s == NULL){
    fprintf(stderr, "malloc failure. Ending program.\n");
    exit(EXIT_FAILURE);
  }
  memcpy(concatenated_s, static_s[0], first_len);
  memcpy(concatenated_s + first_len, param_s, second_len);
  memcpy(concatenated_s + first_len + second_len, static_s[1], third_len + 1);
  
  return concatenated_s;
}

void pull_recent_account_txs(char* key, char** grouped_hash){
  struct CurlBuffer b;
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
    struct json_object *tx_hash;

    transaction = json_object_array_get_idx(parsed_json, i_txs);
    json_object_object_get_ex(transaction, "txHash", &tx_hash);
    char* hash = json_object_get_string(tx_hash);
    grouped_hash[i_txs] = realloc(hash, strlen(hash) + 1);
    printf("%i. hash: %s\n", i_txs, grouped_hash[i_txs]); 
    free(tx_hash);
  }
  free(b.ptr);
}

void pull_and_store_target_accounts(char* tx_sig, SwapType swap_type){
  // fetch tx json
  struct CurlBuffer b;
  curl_buffer_init(&b);
  pull_json(&b, INSPECT_TX_URL, tx_sig);
  
  // isolate reqd accounts 
  printf("TRACE --- 0\n");
  struct json_object *parsed_json;
  parsed_json = json_tokener_parse(b.ptr);
  printf("TRACE --- 0.5\n");

  // only pulls accounts from successful txs
  struct json_object *json_status;
  json_object_object_get_ex(parsed_json, "status", &json_status);
  printf("TRACE --- 0.6\n");
  char* status = json_object_get_string(json_status);
  if (status != "Success"){
    printf("TRACE --- Tx status: fail. Checking the next.\n");
    //return false; 
    //exit(EXIT_FAILURE);
  }
  printf("TRACE --- tx status: %s\n", status);
   
  printf("TRACE --- 1\n");
  struct json_object *inner_instructions; 
  printf("TRACE --- 1.1\n");
  json_object_object_get_ex(parsed_json, "innerInstructions", &inner_instructions); 
  printf("TRACE --- 1.2\n");
  size_t n_ixs = json_object_array_length(inner_instructions);
  
  printf("TRACE --- 2\n");
  for(uint8_t i_ixs = 0; i_ixs < n_ixs; i_ixs++){
    /*
    struct json_object *parsed_instructions;
    parsed_instructions = json_object_array_get_idx(inner_instructions, i_ixs);
    */
    struct json_object *inner_instruction = json_object_array_get_idx(inner_instructions, i_ixs);
    struct json_object *parsed_instructions;
    json_object_object_get_ex(inner_instruction, "parsedInstructions", &parsed_instructions); 

  printf("TRACE --- 2.1\n");
    size_t n_pixs = json_object_array_length(parsed_instructions);
  printf("TRACE --- 2.2\n");
    for (size_t i_pixs = 0; i_pixs < n_pixs; i_pixs++){
      struct json_object *parsed_instruction = 
        json_object_array_get_idx(parsed_instructions, i_pixs );
      
    printf("TRACE --- 3\n");
      // checking for matching amm key
      /*
      struct json_object *program_id;
      json_object_object_get_ex(parsed_instruction, "programId", &program_id);
      if (json_object_get_string(program_id) != amm_key){
        printf("TRACE --- program id mismatch. Checking the next.\n");
        printf("\tprogram_id: %s\n\tamm_key: %s\n");
        //return false; 
        exit(EXIT_FAILURE);
      }
     */ 
      printf("TRACE --- 4\n");
      // extract amm owned accounts
      struct json_object *params;
      json_object_object_get_ex(parsed_instruction, "params", &params);
      
      uint8_t n_account_keys = 0;
      uint8_t *ignore;
      char** account_json_keys;
      struct tuple** required_accounts_p;
      
      printf("TRACE --- 5\n");
      switch (swap_type){
        case Saber:
          n_account_keys = 9;  
          ignore = (uint8_t *[3]){2, 3, 6};
          account_json_keys = (char**){
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
          n_account_keys = 12;  
          ignore = (uint8_t *[3]){2, 3, 6};
          account_json_keys = (char**){
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
          n_account_keys = 13;  
          ignore = (uint8_t *[3]){2, 3, 4};
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
            "Account12",
          };
          break;
        case Whirlpool:
          /*
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
          fprintf(stderr, "Swap type set to whirlpool. exiting program");
          exit(EXIT_FAILURE); // --------------------
        case Aldrin:
          n_account_keys = 11;  
          ignore = (uint8_t *[3]){6, 7, 8};
          account_json_keys = (char**){
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
          /*
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
          fprintf(stderr, "Swap type set to cykura. exiting program");
          exit(EXIT_FAILURE); // -------------------
        case Serum:
          n_account_keys = 10;  
          ignore = (uint8_t *[3]){1, 6, 7};
          account_json_keys = (char**){
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
        case Orca:
        case OrcaV2:
        case Stepn:
        case Saros:
        case Solana:
          n_account_keys = 10;  
          ignore = (uint8_t *[3]){2, 3, 6};
          account_json_keys = (char**){
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
          fprintf(stderr, "Invalid SwapType");
          exit(EXIT_FAILURE);
      }
      printf("TRACE --- 6\n");
      struct tuple req_acc[n_account_keys];
      for (size_t i_account_keys = 0; i_account_keys < n_account_keys; i_account_keys++){
        struct json_object *pool_public_key;
      printf("TRACE --- 6.1\n");
      printf("pulling value for key: %s", account_json_keys[i_account_keys]);
        json_object_object_get_ex(params, account_json_keys[i_account_keys], &pool_public_key);
      printf("TRACE --- 6.2\n");
        req_acc[i_account_keys].a = account_json_keys[i_account_keys];
      printf("TRACE --- 6.3\n");
        req_acc[i_account_keys].b = json_object_get_string(pool_public_key);
      printf("TRACE --- 6.4\n");
        free(pool_public_key);
      printf("TRACE --- 6.5\n");
        pool_public_key = NULL;
      printf("TRACE --- 6.6 - LOOP END\n");
      }
      // check db and store here -----------------------------------------------------------
      
      // -- test
      printf("TRACE --- 7\n");
      for (uint8_t i_kk = 0; i_kk < n_account_keys; i_kk++){
        printf("%s : %s", req_acc[i_kk].a, req_acc[i_kk].b);
      }; 
      // -- 
      
    }
    //return true;
  }
      printf("TRACE --- END");
}

// testing pull and json parse.
void test_pull_and_parse(){
  struct CurlBuffer b;
  curl_buffer_init(&b);
  
  pull_json(&b, INSPECT_ACC_URL,
      "BE3G2F5jKygsSNbPFKHHTxvKpuFXSumASeGweLcei6G3");
  
  struct json_object *parsed_json;
  struct json_object *transaction;

  struct json_object *blocktime; 
  struct json_object *slot;
  struct json_object *tx_hash;
  struct json_object *fee;
  struct json_object *status;
  struct json_object *lamport;
  struct json_object *signers;
  struct json_object *signer;
  struct json_object *parsed_instruction;
  struct json_object *program_id;
  struct json_object *program;
  struct json_object *type;
  
  size_t n_transactions;
  size_t i;
  parsed_json = json_tokener_parse(b.ptr);

  n_transactions = json_object_array_length(parsed_json);
  printf("Number of transactions pulled: %i\n", n_transactions);
  
  for (i=0; i<n_transactions; i++){
    transaction = json_object_array_get_idx(parsed_json, i); 
    
    // retrieve blocktime
    json_object_object_get_ex(transaction, "blockTime", &blocktime);
    printf("block time: %d\n", json_object_get_int(blocktime));
    // retrieve slot
    json_object_object_get_ex(transaction, "slot", &slot);
    printf("slot: %d\n", json_object_get_int(slot));
    // retrieve tx hash/sig
    json_object_object_get_ex(transaction, "txHash", &tx_hash);
    printf("hash: %s\n", json_object_get_string(tx_hash));
    // retrieve fee
    json_object_object_get_ex(transaction, "fee", &fee);
    printf("fee: %d\n", json_object_get_int(fee));
    // retrieve lamport
    json_object_object_get_ex(transaction, "lamport", &lamport);
    printf("lamport: %d\n", json_object_get_int(lamport));
    
    size_t n_signers;
    size_t i_signers;
    json_object_object_get_ex(transaction, "signer", &signers);
    n_signers = json_object_array_length(signers);
    printf("signers:\n");
    for (i_signers = 0; i_signers < n_signers; i_signers++){
      signer = json_object_array_get_idx(signers, i_signers); 
      // retrieve signer
      printf("\t%s\n", json_object_get_string(signer));
    }
    // ---------- tx_hash is all we need.
  }
  free(b.ptr);
}

