#pragma once
#define U_BYTE unsigned char
#define U_8_BYTES signed long long

#define MAX_BYTECODE_SIZE 2048
#define MAX_STACK_SIZE 255

enum halt_states { HALT, ONGOING };

enum operations {
  OP_POP = 1,
  OP_PUSH = 2,
  OP_ADD = 3,
  OP_NEGATE = 4,
  OP_MULTIPLY = 5,
  OP_DIVIDE = 6,
  OP_PRINT = 7,
  OP_GOTO = 8, /* goes to a LABEL specifically */
  OP_LABEL = 9,
  OP_CLONE = 10,
  OP_HALT = 11,
};

enum run_status {
  OK,
  ERROR,
};

typedef struct {
  int pc;
  int stack_top;
  int halt;
  U_8_BYTES stack[MAX_STACK_SIZE];
  char error[256];
} vm;

vm *vm_new();
int vm_run_bytecode(vm *vm, U_BYTE bytecode[], int size);
char *vm_get_error(vm *vm);
void vm_free(vm *vm);
