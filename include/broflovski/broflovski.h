#pragma once
#include<stdint.h>

struct amm;

struct config{
  char amm_names[40][40];
  char amm_keys[40][45];
  uint8_t amm_schema_types[40];
  uint8_t n_amms_loaded;
} config_data;

struct curl_buffer{
  char *ptr;
  uint32_t len;
};

launch_broflovski(const char* config_file_path);
void *operate_on_amm(void*);
char* concatenate_url(char** static_s, char* param_s);
void initialize_amm(struct amm* amm_data, char* amm_name,
    char* amm_key, char* amm_schema_type);

