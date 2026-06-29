bits 64

%macro macFuncPrologue 1
    push    rbp
    mov     rbp, rsp
    sub     rsp, %1
%endmacro

%macro macFuncEpilogue 1
    add     rsp, %1
    mov     rsp, rbp
    pop     rbp
    ret
%endmacro

%ifidn __OUTPUT_FORMAT__, win64
    %define NEWLINE 0xD, 0xA ; CRLF
    ; Windows Calling Convention
    %define arg1 rcx
    %define arg2 rdx
    %define arg3 r8
    %define arg4 r9
    %define SHADOW_SPACE 32
    ; Stack Frame
    ; ...
    ; [RSP + 40] -> arg6
    ; [RSP + 32] -> arg5
    ; [RSP + 24] -> SS for arg4
    ; [RSP + 16] -> SS for arg3
    ; [RSP + 8] -> SS for arg2
    ; [RSP + 0] -> SS for arg1
    ; [RSP - 8] -> Return Addr
    ; Due to use of RSP, pushing local vars can alter these offsets
%else
    %define NEWLINE 0xA ; LF
    ; Linux/macOS Calling Convention
    %define arg1 rdi
    %define arg2 rsi
    %define arg3 rdx
    %define arg4 rcx
    %define arg5 r8
    %define arg6 r9
    %define SHADOW_SPACE 0
    ; Stack Frame
    ; ...
    ; [RSP + 8] -> SS for arg1
    ; [RSP + 0] -> Return Addr
%endif

section .text
global nasmAdd
global nasmSub
global nasmIMul
global nasmIDiv

nasmAdd:
    ; Shadow Space Adjustment (32 + 8 (for Return Ptr) on Win64)
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