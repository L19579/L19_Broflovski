#pragma once
#include<stdint.h>
/*
struct amm{
  char amm_name[30];
  char amm_key[45];
  uint8_t amm_schema_type;
  char amm_schema[30];
  char table_name[300];
  uint32_t table_created;
  char account_keys[30][45];
  char account_vals[30][45];
  int n_account_keys;
  uint8_t** ignore;
  uint8_t n_ignore;
};
*/

struct amm;

struct config{
  char amm_names[40][40];
  char amm_keys[40][45];
  uint8_t amm_schema_types[40];
  uint8_t n_amms_loaded;
};

struct curl_buffer{
  char *ptr;
  uint32_t len;
};

int launch_broflovski(char* config_file_path);
char* concatenate_url(char** static_s, char* param_s);
void initialize_amm(struct amm* amm_data, char* amm_name,
    char* amm_key, char* amm_schema_type);
//void curl_buffer_init(struct curl_buffer *b);
//void curl_buffer_write(void *ptr, uint32_t size, uint32_t nmemb, struct curl_buffer *b);
//void pull_json(char* base, char* target);

