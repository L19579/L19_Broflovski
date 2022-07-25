#pragma once;

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
