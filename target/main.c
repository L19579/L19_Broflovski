#include <stdio.h>
#include "broflovski/broflovski.h"
//#include "amm_keys/amm_keys.h"

char* CONFIG_PATH = "/home/jojo/current_focus/public/L19_Broflovski/unique_data/config.toml";

int main(){
  printf("TRACE main --- START\n");
  int res = launch_broflovski(CONFIG_PATH);
  printf("TRACE main --- END\n");
  return res;
}

/*
int main(){
  // temp: testing call to prompt
  StatementType statement_type = Success;
  char *stmt = "statement fn is linked";
  show_statement(statement_type, stmt);
  
  // temp: testing call to broflovski
  char *fake_path = "random text";
  uint8_t res = test_read_toml(fake_path);
  return 0;
}
*/
