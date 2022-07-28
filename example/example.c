#include <stdio.h>
#include "broflovski/broflovski.h"

const char* CONFIG_PATH = "/home/jojo/current_focus/public/L19_Broflovski/config.toml";

int main(){
  printf("GLOBAL: Starting program\n\n");
  launch_broflovski(CONFIG_PATH);
  printf("\nGLOBAL: End of program\n");
  return 0;
}
