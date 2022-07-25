# pragma once
#include<stdint.h>

struct amm{
  char amm_name[30]; 
  uint8_t amm_schema_type; 
  char amm_schema[30]; 
  char table_name[300]; 
  int32_t table_created; 
  char* accounts_key[20]; 
  char* accounts_val[20]; 
  uint8_t n_account_keys; 
  uint8_t** ignore; 
  uint8_t n_ignore; 
};


int launch_broflovski(char* config_file_path);
char* concatenate_url(char** static_s, char* param_s);
//void curl_buffer_init(struct curl_buffer *b);
//void curl_buffer_write(void *ptr, uint32_t size, uint32_t nmemb, struct curl_buffer *b);
//void pull_json(char* base, char* target);

