#include "amm_keys.h"
#include "broflovski.h"

void assign_schema_keys(struct amm* amm_data){
  switch (amm_data->amm_schema_type){
    case Saber:
      strcpy(amm_data->amm_schema, "saber_type");
      amm_data->n_account_keys = 9;  
      amm_data->ignore = (uint8_t *[3]){2, 3, 6};
      amm_data->n_ignore = 3;
      amm_data->accounts_key = (char*[9]){
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
      strcpy(amm_data->amm_schema, "step_type");
      amm_data->n_account_keys = 12;  
      amm_data->ignore = (uint8_t *[3]){2, 3, 6};
      amm_data->n_ignore = 3;
      amm_data->accounts_key = (char*[12]){
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
      strcpy(amm_data->amm_schema, "lifinity_type");
      //printf("TRACE --- matched with Lifinity\n");
      amm_data->n_account_keys = 13;  
      amm_data->ignore = (uint8_t *[3]){2, 3, 4};
      amm_data->n_ignore = 3;
      amm_data->accounts_key = (char*[13]){
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
      amm_data->accounts_key = (char**){
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
      strcpy(amm_data->amm_schema, "aldrin_type");
      amm_data->n_account_keys = 11;  
      amm_data->ignore = (uint8_t *[3]){6, 7, 8};
      amm_data->n_ignore = 3;
      amm_data->accounts_key = (char*[11]){
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
      amm_data->accounts_key = (char**){
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
      strcpy(amm_data->amm_schema, "serum_type");
      amm_data->n_account_keys = 16;  
      amm_data->ignore = (uint8_t *[3]){1, 6, 7};
      amm_data->n_ignore = 3;
      amm_data->accounts_key = (char*[16]){
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
      strcpy(amm_data->amm_schema, "stepn_type");
      amm_data->n_account_keys = 10;  
      amm_data->ignore = (uint8_t *[3]){2, 3, 6};
      amm_data->n_ignore = 3;
      amm_data->accounts_key = (char*[10]){
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
      strcpy(amm_data->amm_schema, "saros_type");
      amm_data->n_account_keys = 10;  
      amm_data->ignore = (uint8_t *[3]){2, 3, 6};
      amm_data->n_ignore = 3;
      amm_data->accounts_key = (char*[10]){
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
      strcpy(amm_data->amm_schema, "solana_type");
      amm_data->n_account_keys = 10;  
      amm_data->ignore = (uint8_t *[3]){2, 3, 6};
      amm_data->n_ignore = 3;
      amm_data->accounts_key = (char*[10]){
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
  };
}
