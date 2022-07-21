#pragma once

typedef enum{
  Test,
  General,
  Success,
  Warning,
  Error,
} StatementType;

void show_statement(StatementType statement_type, char* stmt);
