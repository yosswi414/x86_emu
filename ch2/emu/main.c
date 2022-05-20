// typedef unsigned int uint32_t;
// typedef unsigned char uint8_t;

// typedef uint32_t size_t;

#include <bits/stdint-uintn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum registers_id { EAX,
                    ECX,
                    EDX,
                    EBX,
                    ESP,
                    EBP,
                    ESI,
                    EDI,
                    EIP,
                    TERM };
const char* registers_name[] = {
    "EAX",
    "ECX",
    "EDX",
    "EBX",
    "ESP",
    "EBP",
    "ESI",
    "EDI",
    "EIP"};

char** trace_func;

#define REGISTERS_COUNT TERM
const size_t MEMORY_SIZE = 0x1000000;

typedef struct {
    // generic register
    uint32_t registers[REGISTERS_COUNT];
    // eflags register
    uint32_t eflags;
    // memory
    uint8_t* memory;
    // program counter (instruction pointer)
    uint32_t eip;
} Emulator;

typedef void instruction_func_t(Emulator*);
instruction_func_t* instructions[256];

void mov_r32_imm32(Emulator* emu);
void short_jump(Emulator* emu);
void init_instructions(void);
uint32_t get_code8(Emulator* emu, int index);
int32_t get_sign_code8(Emulator* emu, int index);
uint32_t get_code32(Emulator* emu, int index);
void dump_registers(Emulator* emu);

void* malloc_s(size_t siz) {
    void* ptr = malloc(siz);
    if (!ptr) {
        fprintf(stderr, "error: malloc: failed to allocate %zu bytes\n", siz);
        abort();
    }
    return ptr;
}

// construct emulator
Emulator* create_emu(size_t size, uint32_t eip, uint32_t esp) {
    Emulator* emu = malloc_s(sizeof(Emulator));
    emu->memory = malloc_s(size);

    // initialize the generic registers to zero
    memset(emu->registers, 0, sizeof(emu->registers));

    // initialize some registers as specified by the arguments
    emu->eip = eip;
    emu->registers[ESP] = esp;

    return emu;
}

// destroy emulator
void destroy_emu(Emulator* emu) {
    free(emu->memory);
    free(emu);
}

// initialize func code dictionary trace_func[]
void init_trace_func(){
    trace_func = malloc_s(0xff * sizeof(char *));
    for(int i = 0x00; i <= 0xff; ++i){
        trace_func[i] = malloc_s(64 * sizeof(char));
        strcpy(trace_func[i], "(not registered)");
    }
}

void add_trace_func(uint8_t code, const char* func_name){
    strcpy(trace_func[code], func_name);
}

int main(int argc, char* argv[]) {
    FILE* binary;
    Emulator* emu;

    // just for debug
    init_trace_func();

    if (argc != 2) {
        fprintf(stderr, "usage: px86 filename\n");
        return 1;
    }

    // create emulator with EIP = 0 and ESP = 0x7c00
    emu = create_emu(MEMORY_SIZE, 0x0000, 0x7c00);

    binary = fopen(argv[1], "rb");
    if (!binary) {
        fprintf(stderr, "error: fopen: cannot open file %s\n", argv[1]);
        return 1;
    }

    // fetch machine code file (up to 512 bytes)
    fread(emu->memory, 1, 0x200, binary);
    fclose(binary);

    init_instructions();

    while (emu->eip < MEMORY_SIZE) {
        uint8_t code = get_code8(emu, 0);

        fprintf(stdout, "EIP = 0x%x, Code = 0X%X (-> %s)\n", emu->eip, code, trace_func[code]);

        if (!instructions[code]) {
            fprintf(stderr, "\n\nNot Implemented: %x\n", code);
            break;
        }

        // execute instructions
        instructions[code](emu);
        // emulator terminates if EIP gets to be 0
        if (!emu->eip) {
            fprintf(stdout, "\n\nend of program\n\n");
            break;
        }
    }

    dump_registers(emu);
    destroy_emu(emu);
    return 0;
}

// operations
void mov_r32_imm32(Emulator* emu) {
    uint8_t reg = get_code8(emu, 0) - 0xb8;
    uint32_t value = get_code32(emu, 1);
    emu->registers[reg] = value;
    emu->eip += 5;
}

void short_jump(Emulator* emu) {
    int8_t diff = get_sign_code8(emu, 1);
    emu->eip += (diff + 2);
}

void init_instructions(void) {
    int i;
    memset(instructions, 0, sizeof(instructions));
    // 0xb8 ~ 0xbf: mov_r32_imm32()
    for (i = 0; i < 8; ++i) {
        instructions[0xb8 + i] = mov_r32_imm32;
        // just for debug
        add_trace_func(0xb8 + i, "mov_r32_imm32");
    }
    // 0xeb: short_jump()
    instructions[0xeb] = short_jump;
    // just for debug
    add_trace_func(0xeb, "short_jump");
}

uint32_t get_code8(Emulator* emu, int index) {
    return emu->memory[emu->eip + index];
}

int32_t get_sign_code8(Emulator* emu, int index) {
    return (int8_t)emu->memory[emu->eip + index];
}

uint32_t get_code32(Emulator* emu, int index) {
    int i;
    uint32_t ret = 0;

    // acquire a value in little endian (LE) on the memory
    for (i = 0; i < 4; ++i) ret |= get_code8(emu, index + i) << (i * 8);
    return ret;
}

void dump_registers(Emulator* emu){
    for (int i = 0; i < TERM; ++i){
        fprintf(stdout, "%s = 0x%08x\n", registers_name[i], emu->registers[i]);
    }
}