#pragma once
#include <stdint.h>
//#include "broflovski/broflovski.h"

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
  uint8_t ignore[10];
  uint8_t n_ignore;
};

typedef enum{
  Orca = 0,
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

void assign_schema_keys(struct amm* amm_data);
