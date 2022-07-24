#include <stdint.h>
#include "broflovski/broflovski.h"

int main(){
  uint8_t res = launch_broflovski();
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
