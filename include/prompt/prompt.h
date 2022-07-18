#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef enum{
  Test,
  General,
  Success,
  Warning,
  Error,
} StatementType;

void show_statement(StatementType statement_type, char* stmt);
