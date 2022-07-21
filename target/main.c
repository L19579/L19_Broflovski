#include <time.h>
#include <stdint.h>
#include "prompt/prompt.h"
#include "broflovski/broflovski.h"

int main(){
  char* config_path = "/home/jojo/current_focus/public/L19_Broflovski/unique_data/config.toml";
  uint8_t res = launch_broflovski(config_path);
  // case/switch here.
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
