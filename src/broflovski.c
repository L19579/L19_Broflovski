#include <string.h>
#include <unistd.h>
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
  Whirlpool, // This needs review
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

uint8_t launch_broflovski(char* file_path){
  char* account_key = "temp";
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
  struct curl_buffer b;
  curl_buffer_init(&b);
  pull_json(&b, INSPECT_ACC_URL, key);
  
  struct json_object *parsed_json;

  size_t n_txs;
  size_t i_txs;
  struct json_object *transactions;
  
  parsed_json = json_tokener_parse(b.ptr);
  n_txs = json_object_array_length(parsed_json);
  for (i_txs = 0; i_txs < n_txs; i_txs++){
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

pull_swap_accounts(char* target_key, ){

}


// testing pull and json parse.
void test_pull_and_parse(){
  struct curl_buffer b;
  curl_buffer_init(&b);
  
  pull_json(&b, INSPECT_ACC_URL,
      "temp");
  
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

