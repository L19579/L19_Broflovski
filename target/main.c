#include <time.h>
#include <prompt/prompt.h>

int main(){
  // temp
  StatementType statement_type = Success;
  char *stmt = "statement fn is linked";
  show_statement(statement_type, stmt);
  return 0;
}
