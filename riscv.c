#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "riscv.h"

/************** BEGIN HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ***************/
const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int U_TYPE = 3;
const int UNKNOWN_TYPE = 4;
int subType;
/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, UNKNOWN_TYPE
 */
static int get_op_type(char *op)
{
    const char *r_type_op[] = {"add", "sub", "and", "or", "xor", "slt", "sll", "sra"};
    const char *i_type_op[] = {"addi", "andi", "ori", "xori", "slti"};
    const char *mem_type_op[] = {"lw", "lb", "sw", "sb"};
    const char *u_type_op[] = {"lui"};
    for (int i = 0; i < (int)(sizeof(r_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(r_type_op[i], op) == 0)
        {
            subType = i;
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(i_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(i_type_op[i], op) == 0)
        {
            subType = i;
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(mem_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(mem_type_op[i], op) == 0)
        {
            subType = i;
            return MEM_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(u_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(u_type_op[i], op) == 0)
        {
            return U_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}
/*************** END HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ****************/

registers_t *registers;
hashtable_t *memory;
const int num_buckets = 4096;
// TODO: create any additional variables to store the state of the interpreter

void init(registers_t *starting_registers)
{
    registers = starting_registers;
    memory = ht_init(num_buckets);
    // TODO: initialize any additional variables needed for state
}

// TODO: create any necessary helper functions

char *ltrim(char *s){
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s){
    char *pointer = s + strlen(s) - 1;
    while(isspace(*pointer) && pointer > s) pointer--;
    *(pointer + 1) = '\0';
    return s;
}

char *trim(char *s)
{
    return ltrim(rtrim(s));
}

void read_instruction(char *instruction, int op_type){
    char *s = instruction;
    s = strtok(NULL, ",");
    char *value[3]; int i = 0;

    while (s != NULL){
        s = trim(s);
        value[i++] = s;
        s = strtok(NULL, ",");
    }
    if (op_type == 0) process0(++value[0], ++value[1], ++value[2]);
    if (op_type == 1) process1(++value[0], ++value[1], value[2]);
    if (op_type == 2) process2(++value[0], value[1]);
    if (op_type == 3) process3(++value[0], value[1]);
}

int *toByte(int num) {
    int* byte = malloc (4 * sizeof(int));
    for (int i = 0; i <= 3; i--) {
        byte[3 - i] = ((int) (num >> (8 * i))) & 0xff;
    }
    return byte;
}

// converts from an array of bytes to an integer value
int toNum(int *byte) {
    int num = 0;
    for (int i = 0; i <= 3; i++) {
        num += (byte[i] << (8 * (3 - i)));
    }
    return num;
}

void step(char *instruction)
{
    // Extracts and returns the substring before the first space character,
    // by replacing the space character with a null-terminator.
    // `instruction` now points to the next character after the space
    // See `man strsep` for how this library function works
    //char temp = *instruction;
    char *op = strtok(instruction, " ");
    // Uses the provided helper function to determine the type of instruction
    int op_type = get_op_type(op);
    if (op_type == UNKNOWN_TYPE) return;
    read_instruction(op, op_type);
        // Skip this instruction if it is not in our supported set of instructions
    // TODO: write logic for evaluating instruction on current interpreter state
}

void process0(char *a, char *b, char *c) {
    int r1 = atoi(a);
    int r2 = atoi(b);
    int r3 = atoi(c);
    switch (subType){
    case 0:
        registers->r[r1] = registers->r[r2] + registers->r[r3];
        break;
    case 1:
        registers->r[r1] = registers->r[r2] - registers->r[r3];
        break;
    case 2:
        registers->r[r1] = registers->r[r2] & registers->r[r3];
        break;
    case 3:
        registers->r[r1] = registers->r[r2] | registers->r[r3];
        break;
    case 4:
        registers->r[r1] = registers->r[r2] ^ registers->r[r3];
        break;
    case 5:
        registers->r[r1] = (registers->r[r2] < registers->r[r3]);
        break;
    case 6:
        registers->r[r1] = (registers->r[r2] << registers->r[r3]);
        break;
    case 7:
        registers->r[r1] = registers->r[r2] >> registers->r[r3];
        break;
    }
}

void process1(char *a, char *b, char *c) {
    int r1 = atoi(a);
    int r2 = atoi(b);
    int r3 = atoi(c);
    switch (subType){
    case 0:
        registers->r[r1] = registers->r[r2] + r3;
        break;
    case 1:
        registers->r[r1] = registers->r[r2] & r3;
        break;
    case 2:
        registers->r[r1] = registers->r[r2] | r3;
        break;
    case 3:
        registers->r[r1] = registers->r[r2] ^ r3;
        break;
    case 4:
        registers->r[r1] = registers->r[r2] < r3;
        break;
    }
}
void process2(char *a, char *b) {
    int r1 = atoi(a);
    char *temp = strtok(b, "(");
    int offset = strtol(temp, NULL, 0);
    temp = strtok(NULL, ")");
    int r2 = atoi(++temp);
    printf("%d %d %d\n", r1, offset, r2);
    if (subType == 0){
        int byte[4];
        byte[3] = ht_get(memory, registers->r[r2] + offset);
        byte[2] = ht_get(memory, registers->r[r2] + offset + 1);
        byte[1] = ht_get(memory, registers->r[r2] + offset + 2);
        byte[0] = ht_get(memory, registers->r[r2] + offset + 3);
        int num = toNum(byte);
        registers->r[r1] = num;
    } else if (subType == 1){
        int byte = ht_get(memory, registers->r[r2] + offset);
        registers->r[r1] = byte;
    } else if (subType == 2){
        int *byte = toByte(registers->r[r1]);
        ht_add(memory, registers->r[r2] + offset,     byte[3]);
        ht_add(memory, registers->r[r2] + offset + 1, byte[2]);
        ht_add(memory, registers->r[r2] + offset + 2, byte[1]);
        ht_add(memory, registers->r[r2] + offset + 3, byte[0]);
    } else {
        int byte = registers->r[r1] & 0xff;
        ht_add(memory, registers->r[r2] + offset, byte);
    }
}
void process3(char *a, char *b) {
    int r1 = atoi(a);
    int r2 = (int) strtol(b, NULL, 0);
    registers->r[r1] = r2 << 12;
}

