bits 64

%ifidn __OUTPUT_FORMAT__, win64
    %define NEWLINE 0xD, 0xA ; CRLF
    ; Windows Calling Convention
    %define arg1 rcx
    %define arg2 rdx
    %define SHADOW_SPACE 32
%else
    %define NEWLINE 0xA ; LF
    ; Linux/macOS Calling Convention
    %define arg1 rdi
    %define arg2 rsi
    %define SHADOW_SPACE 0
%endif

section .text
global nasmAdd
global nasmSub
global nasmIMul
global nasmIDiv

nasmAdd:
    ; Shadow Space Adjustment (32 + 8 Return on Win64)
    ; Shadow space is used by potential function calls (i.e. printf)
    sub rsp, SHADOW_SPACE+8
    mov rax, arg1
    add rax, arg2
    add rsp, SHADOW_SPACE+8
    ret

nasmSub:
    sub rsp, SHADOW_SPACE+8
    mov rax, arg1
    sub rax, arg2
    add rsp, SHADOW_SPACE+8
    ret

nasmIMul:
    sub rsp, SHADOW_SPACE+8
    mov rax, arg1
    imul rax, arg2
    add rsp, SHADOW_SPACE+8
    ret

nasmIDiv:
    sub rsp, SHADOW_SPACE+8
    mov rax, arg1
%ifidn __OUTPUT_FORMAT__, win64
    mov r8, arg2 ; Save Divisor to spare reg (RDX is used by idiv on win64)
%endif
    cqo ; Sign-extend RAX as RDX:RAX for division
%ifidn __OUTPUT_FORMAT__, win64
    idiv r8 ; Divide RDX:RAX by R8, quotient in RAX
%else
    idiv arg2
%endif
    add rsp, SHADOW_SPACE+8
    ret