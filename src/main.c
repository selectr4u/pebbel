#include "stdio.h"
#include "stdlib.h"
#include "vm/vm.h"

U_BYTE *get_bytecode(int *size) {
  U_BYTE bytecode[MAX_BYTECODE_SIZE];
  int c, s;

  s = 0;

  while ((c = getchar()) != EOF) {
    bytecode[s] = (U_BYTE)c;
    s++;
  }

  *size = s; /* set given size variable */
  return bytecode;
}

int main() {
  int size, status;
  U_BYTE *bytecode;
  vm *pvm;

  bytecode = get_bytecode(&size);

  if (size == 0) {
    printf("err: no bytecode provided\n");
    exit(EXIT_FAILURE);
  }

  pvm = vm_new();

  status = vm_run_bytecode(pvm, bytecode, size);
  if (status == OK) {
    printf("ran bytecode OK\n");
  } else {
    printf("encountered err: %s", vm_get_error(pvm));
  }

  vm_free(pvm);

  return EXIT_SUCCESS;
}
