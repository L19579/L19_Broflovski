#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt/prompt.h"

void show_statement(StatementType statement_type, char* stmt){
  //char** output = malloc(1 * sizeof(*array));
  char *output = malloc(12);
  if (output){ //req?
    switch (statement_type){
      case Test:
        strcpy(output, "Test"); 
        break;
      case General:
        strcpy(output, "General"); 
        break;
      case Success:
        strcpy(output, "Success"); 
        break;
      case Warning:
        strcpy(output, "Warning"); 
        break;
      case Error:
        strcpy(output, "Error"); 
        break;
    }
  }
  printf("%s : %s\n", output, stmt);
  free(output);
}
