/* pebbel -> virtual stack-based compiler in C (64 bits, integers only)
 */
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline U_BYTE vm_fetch_byte(U_BYTE *bytecode, int n) {
  return bytecode[n];
}

static inline void vm_set_error(vm *vm, char *msg) {
  strncpy(vm->error, msg, 256);
}

static inline int vm_get_label_pc(U_BYTE label, U_BYTE bytecode[], int size) {
  /* o(n) to find label -> may consider registering labels *in advance* for o(1)
   * to backup o(n) */
  int pc_at = -1;
  int pc = 0;

  while (pc < size) {
    U_BYTE opcode = vm_fetch_byte(bytecode, pc);

    if (opcode == OP_LABEL) {
      /* indicates next byte is label identifier */
      if (pc + 1 >= size) {
        pc_at = -1; /* means after this there isn't any other bytecode */
        break;
      }

      U_BYTE b1 = vm_fetch_byte(bytecode, pc + 1);

      if (b1 == label) {
        pc_at = pc;
        break;
      }
    }
    pc++;
  }

  return pc_at;
}

vm *vm_new() {
  vm *new_vm = malloc(sizeof(vm));
  new_vm->pc = 0;
  new_vm->halt = ONGOING;
  new_vm->stack_top = -1;

  return new_vm;
}

int vm_run_bytecode(vm *vm, U_BYTE bytecode[], int size) {
  vm->pc = 0;

  /* declarations for in the switch statements */
  U_8_BYTES n1, n2, r; /* basically akin to the accumulator */
  U_BYTE b1, b2, b3, b4, b5, b6, b7,
      b8;       /* for reading the 8 bytes when required for opcode */
  U_8_BYTES rb; /* resulting 8 bytes from assembling */
  U_8_BYTES print_item;
  int pc_goto;

  int status = OK;
  while (vm->halt != HALT && status != ERROR) {
    if (vm->pc == size) {
      vm->halt = HALT;
      break;
    }

    U_BYTE opcode = vm_fetch_byte(bytecode, vm->pc);

    switch (opcode) {
    case OP_POP:
      if (vm->stack_top < 0) {
        status = ERROR;
        vm_set_error(vm, "cannot pop from an empty stack");
        break;
      }
      vm->stack_top--;
      break;
    case OP_PUSH:
      if (vm->pc + 8 >= size) {
        status = ERROR;
        vm_set_error(vm, "push not provided with 8 bytes");
        break;
      }

      b1 = vm_fetch_byte(bytecode, vm->pc + 1);
      b2 = vm_fetch_byte(bytecode, vm->pc + 2);
      b3 = vm_fetch_byte(bytecode, vm->pc + 3);
      b4 = vm_fetch_byte(bytecode, vm->pc + 4);
      b5 = vm_fetch_byte(bytecode, vm->pc + 5);
      b6 = vm_fetch_byte(bytecode, vm->pc + 6);
      b7 = vm_fetch_byte(bytecode, vm->pc + 7);
      b8 = vm_fetch_byte(bytecode, vm->pc + 8);

      vm->pc += 8;

      /* put into big endian form (furthest lhs = smallest) */
      rb = ((U_8_BYTES)(b1 << 7 * 8) | (U_8_BYTES)(b2 << 6 * 8) |
            (U_8_BYTES)(b3 << 5 * 8) | (U_8_BYTES)(b4 << 4 * 8) |
            (U_8_BYTES)(b5 << 3 * 8) | (U_8_BYTES)(b6 << 2 * 8) |
            (U_8_BYTES)(b7 << 1 * 8) | (U_8_BYTES)(b8));

      vm->stack_top++;
      vm->stack[vm->stack_top] = rb;
      break;
    case OP_ADD:
      if (vm->stack_top < 1) {
        status = ERROR;
        vm_set_error(vm, "requires 2 values to add");
        break;
      }
      n1 = vm->stack[vm->stack_top];
      vm->stack_top--;
      n2 = vm->stack[vm->stack_top];
      vm->stack_top--;

      r = n1 + n2;

      vm->stack_top++;
      vm->stack[vm->stack_top] = r;
      break;
    case OP_NEGATE:
      if (vm->stack_top < 1) {
        status = ERROR;
        vm_set_error(vm, "requires 2 values to negate");
        break;
      }
      n1 = vm->stack[vm->stack_top];
      vm->stack_top--;
      n2 = vm->stack[vm->stack_top];
      vm->stack_top--;

      /* note that no negative values exist (unsigned long long) */
      r = n1 - n2;

      vm->stack_top++;
      vm->stack[vm->stack_top] = r;
      break;
    case OP_MULTIPLY:
      if (vm->stack_top < 1) {
        status = ERROR;
        vm_set_error(vm, "requires 2 values to multiply");
        break;
      }
      n1 = vm->stack[vm->stack_top];
      vm->stack_top--;
      n2 = vm->stack[vm->stack_top];
      vm->stack_top--;

      r = n1 * n2;

      vm->stack_top++;
      vm->stack[vm->stack_top] = r;
      break;
    case OP_DIVIDE:
      if (vm->stack_top < 1) {
        status = ERROR;
        vm_set_error(vm, "requires 2 values to divide");
        break;
      }
      n1 = vm->stack[vm->stack_top];
      vm->stack_top--;
      n2 = vm->stack[vm->stack_top];
      vm->stack_top--;

      r = n1 / n2;

      vm->stack_top++;
      vm->stack[vm->stack_top] = r;
      break;
    case OP_PRINT:
      if (vm->stack_top < 0) {
        status = ERROR;
        vm_set_error(vm, "nothing on stack to print");
        break;
      }
      print_item = vm->stack[vm->stack_top];
      printf("[OUTPUT] %lld\n", print_item);
      break;
    case OP_GOTO:
      if (vm->pc + 1 >= size) {
        status = ERROR;
        vm_set_error(vm, "goto not provided with 1 byte label");
        break;
      }
      b1 = vm_fetch_byte(bytecode, vm->pc + 1);

      pc_goto = vm_get_label_pc(b1, bytecode, size);
      if (pc_goto == -1) {
        status = ERROR;
        vm_set_error(vm, "provided label in goto does not exist");
        break;
      }

      vm->pc = pc_goto;
      continue;
      break;
    case OP_LABEL:
      vm->pc++;
      break;
    case OP_CLONE:
      if (vm->pc + 1 >= size) {
        status = ERROR;
        vm_set_error(vm, "clone not provided with 1 bytes");
        break;
      }

      b1 = vm_fetch_byte(bytecode, vm->pc + 1);
      vm->pc++;

      if (b1 > MAX_STACK_SIZE) {
        status = ERROR;
        vm_set_error(vm, "exceeds max stack size");
        break;
      }

      if (b1 > vm->stack_top) {
        status = ERROR;
        vm_set_error(vm, "exceeds current stack size");
        break;
      }

      vm->stack_top++;
      vm->stack[vm->stack_top] = vm->stack[b1];
      break;
    case OP_HALT:
      vm->halt = HALT;
      break;
    default:
      status = ERROR;
      vm_set_error(vm, "unknown opcode encountered");
    }

    vm->pc++;
  }

  return status;
}

char *vm_get_error(vm *vm) { return vm->error; }

void vm_free(vm *vm) { free(vm); }
