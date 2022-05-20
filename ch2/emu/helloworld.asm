BITS 32
start:
    mov     eax, 0xc0ffee
    mov     ecx, 0xfeeded
    mov     edx, 0x12345678
    mov     ebx, 0xdeadbeef
    jmp     short start
