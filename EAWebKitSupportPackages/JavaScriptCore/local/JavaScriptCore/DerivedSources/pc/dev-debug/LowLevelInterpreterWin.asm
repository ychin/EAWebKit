; offlineasm input hash: 94f23e4630190e03b831bc3a08fe5931eaaa1c02 f66f20bc4f0505bb4e79b0c2f38a5eecae829676 82fda4e596667d2e1f55992b19f40ba7a9e32fdf
.MODEL FLAT, C
INCLUDE LowLevelInterpreterWin.asm.sym
_TEXT SEGMENT

vmEntryToJavaScript PROC PUBLIC
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:430
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    push edi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:311
    push ebx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:312
    mov ebx, dword ptr [12 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:201
    mov edi, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:202
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    sub esp, 32
    mov dword ptr [0 + esp], ebx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:212
    mov edx, dword ptr [5224 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:213
    mov dword ptr [4 + esp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:214
    mov edx, dword ptr [5220 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:215
    mov dword ptr [8 + esp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:216
    mov dword ptr [5220 + ebx], ebp                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:217
    lea eax, [8 + esp]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:221
    and eax, -16                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:222
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:223
    sub esp, 8
    mov esi, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:236
    mov edx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:239
    add edx, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:240
    sal edx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:241
    mov eax, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:242
    sub eax, edx
    cmp eax, dword ptr [9380 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:247
    jae _offlineasm_doVMEntry__stackHeightOK
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:263
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_throw_stack_overflow_error
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    sub esp, 32
    mov ecx, dword ptr [0 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:273
    mov ebx, dword ptr [4 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:274
    mov dword ptr [5224 + ecx], ebx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:275
    mov ebx, dword ptr [8 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:276
    mov dword ptr [5220 + ecx], ebx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:277
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:283
    sub esp, 12
    pop ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:360
    pop edi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:361
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:362
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:442
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:288

  _offlineasm_doVMEntry__stackHeightOK:
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:291
    mov eax, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:292

  _offlineasm_doVMEntry__copyHeaderLoop:
    sub eax, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:295
    mov ecx, dword ptr [4 + esi + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:296
    mov dword ptr [12 + esp + eax * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:297
    mov ecx, dword ptr [0 + esi + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:298
    mov dword ptr [8 + esp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:299
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:300
    jnz _offlineasm_doVMEntry__copyHeaderLoop
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:302
    sub edx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:303
    mov ecx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:304
    sub ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:305
    cmp edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:307
    je _offlineasm_doVMEntry__copyArgs

  _offlineasm_doVMEntry__fillExtraArgsLoop:
    sub ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:309
    mov dword ptr [44 + esp + ecx * 8], -4                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:310
    mov dword ptr [40 + esp + ecx * 8], 0                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:311
    cmp edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:312
    jne _offlineasm_doVMEntry__fillExtraArgsLoop

  _offlineasm_doVMEntry__copyArgs:
    mov eax, dword ptr [40 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:315

  _offlineasm_doVMEntry__copyArgsLoop:
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:318
    jz _offlineasm_doVMEntry__copyArgsDone
    sub edx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:319
    mov ecx, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:320
    mov dword ptr [44 + esp + edx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:321
    mov ecx, dword ptr [0 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:322
    mov dword ptr [40 + esp + edx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:323
    jmp _offlineasm_doVMEntry__copyArgsLoop                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:324

  _offlineasm_doVMEntry__copyArgsDone:
    mov dword ptr [5224 + ebx], esp                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:327
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:357
    mov ecx, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:251
    and ecx, 15
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:253
    jz _offlineasm_doVMEntry__makeCall__checkStackPointerAlignment__stackPointerOkay
    mov ecx, 3134249986                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:254
    int 3                                                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:255

  _offlineasm_doVMEntry__makeCall__checkStackPointerAlignment__stackPointerOkay:
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:362
    ffree st(1)
    call edi
    mov ecx, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:251
    and ecx, 15
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:253
    jz _offlineasm_doVMEntry__makeCall__0_checkStackPointerAlignment__stackPointerOkay
    mov ecx, 3134249987                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:254
    int 3                                                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:255

  _offlineasm_doVMEntry__makeCall__0_checkStackPointerAlignment__stackPointerOkay:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:365
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    sub esp, 32
    mov ecx, dword ptr [0 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:338
    mov ebx, dword ptr [4 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:339
    mov dword ptr [5224 + ecx], ebx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:340
    mov ebx, dword ptr [8 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:341
    mov dword ptr [5220 + ecx], ebx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:342
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:348
    sub esp, 12
    pop ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:360
    pop edi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:361
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:362
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:442
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:353
vmEntryToJavaScript ENDP

vmEntryToNative PROC PUBLIC
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:430
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    push edi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:311
    push ebx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:312
    mov ebx, dword ptr [12 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:201
    mov edi, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:202
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    sub esp, 32
    mov dword ptr [0 + esp], ebx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:212
    mov edx, dword ptr [5224 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:213
    mov dword ptr [4 + esp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:214
    mov edx, dword ptr [5220 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:215
    mov dword ptr [8 + esp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:216
    mov dword ptr [5220 + ebx], ebp                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:217
    lea eax, [8 + esp]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:221
    and eax, -16                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:222
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:223
    sub esp, 8
    mov esi, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:236
    mov edx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:239
    add edx, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:240
    sal edx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:241
    mov eax, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:242
    sub eax, edx
    cmp eax, dword ptr [9380 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:247
    jae _offlineasm_0_doVMEntry__stackHeightOK
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:263
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_throw_stack_overflow_error
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    sub esp, 32
    mov ecx, dword ptr [0 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:273
    mov ebx, dword ptr [4 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:274
    mov dword ptr [5224 + ecx], ebx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:275
    mov ebx, dword ptr [8 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:276
    mov dword ptr [5220 + ecx], ebx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:277
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:283
    sub esp, 12
    pop ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:360
    pop edi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:361
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:362
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:442
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:288

  _offlineasm_0_doVMEntry__stackHeightOK:
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:291
    mov eax, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:292

  _offlineasm_0_doVMEntry__copyHeaderLoop:
    sub eax, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:295
    mov ecx, dword ptr [4 + esi + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:296
    mov dword ptr [12 + esp + eax * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:297
    mov ecx, dword ptr [0 + esi + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:298
    mov dword ptr [8 + esp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:299
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:300
    jnz _offlineasm_0_doVMEntry__copyHeaderLoop
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:302
    sub edx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:303
    mov ecx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:304
    sub ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:305
    cmp edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:307
    je _offlineasm_0_doVMEntry__copyArgs

  _offlineasm_0_doVMEntry__fillExtraArgsLoop:
    sub ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:309
    mov dword ptr [44 + esp + ecx * 8], -4                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:310
    mov dword ptr [40 + esp + ecx * 8], 0                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:311
    cmp edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:312
    jne _offlineasm_0_doVMEntry__fillExtraArgsLoop

  _offlineasm_0_doVMEntry__copyArgs:
    mov eax, dword ptr [40 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:315

  _offlineasm_0_doVMEntry__copyArgsLoop:
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:318
    jz _offlineasm_0_doVMEntry__copyArgsDone
    sub edx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:319
    mov ecx, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:320
    mov dword ptr [44 + esp + edx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:321
    mov ecx, dword ptr [0 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:322
    mov dword ptr [40 + esp + edx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:323
    jmp _offlineasm_0_doVMEntry__copyArgsLoop                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:324

  _offlineasm_0_doVMEntry__copyArgsDone:
    mov dword ptr [5224 + ebx], esp                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:327
    mov eax, edi                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:369
    mov dword ptr [0 + esp], ebp                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:370
    xor edx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:377
    mov dword ptr [4 + esp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:378
    mov ecx, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:379
    push edx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:380
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:381
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:382
    ffree st(1)
    call eax
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:383
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    sub esp, 32
    mov ecx, dword ptr [0 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:338
    mov ebx, dword ptr [4 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:339
    mov dword ptr [5224 + ecx], ebx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:340
    mov ebx, dword ptr [8 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:341
    mov dword ptr [5220 + ecx], ebx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:342
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:348
    sub esp, 12
    pop ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:360
    pop edi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:361
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:362
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:442
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:353
vmEntryToNative ENDP

sanitizeStackForVMImpl PROC PUBLIC
    mov ecx, dword ptr [4 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:755
    mov edx, dword ptr [9384 + ecx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:758
    cmp esp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:759
    jbe _offlineasm_zeroFillDone
    xor eax, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:761

  _offlineasm_zeroFillLoop:
    mov dword ptr [0 + edx], eax                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:763
    add edx, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:764
    cmp esp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:765
    ja _offlineasm_zeroFillLoop

  _offlineasm_zeroFillDone:
    mov edx, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:768
    mov dword ptr [9384 + ecx], edx                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:769
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:770
sanitizeStackForVMImpl ENDP

vmEntryRecord PROC PUBLIC
    mov ecx, dword ptr [4 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:787
    mov eax, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    sub eax, 32
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:791
vmEntryRecord ENDP

llint_entry PROC PUBLIC
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:430
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    push edi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:311
    push ebx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:312
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:805
    ffree st(1)
    call _relativePCBase

  _relativePCBase:
    pop edx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:807
    mov edi, dword ptr [20 + esp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:808
    lea eax, [_llint_op_enter - _relativePCBase + edx]       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    xor ecx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_scope - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_create_direct_arguments - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_create_scoped_arguments - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_create_out_of_band_arguments - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_create_this - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 5                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_to_this - _relativePCBase + edx]     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 6                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_check_tdz - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 7                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_new_object - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_new_array - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 9                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_new_array_with_size - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 10                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_new_array_buffer - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 11                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_new_regexp - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_mov - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 13                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_not - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 14                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_eq - _relativePCBase + edx]          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 15                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_eq_null - _relativePCBase + edx]     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_neq - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 17                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_neq_null - _relativePCBase + edx]    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 18                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_stricteq - _relativePCBase + edx]    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 19                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_nstricteq - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_less - _relativePCBase + edx]        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 21                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_lesseq - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 22                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_greater - _relativePCBase + edx]     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 23                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_greatereq - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 24                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_inc - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 25                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_dec - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 26                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_to_number - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 27                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_to_string - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_negate - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 29                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_add - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 30                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_mul - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 31                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_div - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_mod - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 33                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_sub - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 34                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_lshift - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 35                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_rshift - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_urshift - _relativePCBase + edx]     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 37                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_unsigned - _relativePCBase + edx]    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 38                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_bitand - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 39                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_bitxor - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_bitor - _relativePCBase + edx]       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 41                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_check_has_instance - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 42                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_instanceof - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 43                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_typeof - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 44                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_is_undefined - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 45                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_is_boolean - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 46                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_is_number - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 47                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_is_string - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 48                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_is_object - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 49                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_is_object_or_null - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 50                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_is_function - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 51                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_in - _relativePCBase + edx]          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 52                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_by_id - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 53                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_by_id_out_of_line - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 54                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_array_length - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 55                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_id - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 56                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_id_out_of_line - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 57                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_id_transition_direct - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 58                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_id_transition_direct_out_of_line - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 59                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_id_transition_normal - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 60                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_id_transition_normal_out_of_line - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 61                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_del_by_id - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 62                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_by_val - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 63                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_val - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 64                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_val_direct - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 65                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_del_by_val - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 66                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_by_index - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 67                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_getter_by_id - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 68                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_setter_by_id - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 69                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_getter_setter - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 70                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jmp - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 71                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jtrue - _relativePCBase + edx]       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 72                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jfalse - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 73                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jeq_null - _relativePCBase + edx]    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 74                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jneq_null - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 75                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jneq_ptr - _relativePCBase + edx]    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 76                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jless - _relativePCBase + edx]       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 77                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jlesseq - _relativePCBase + edx]     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 78                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jgreater - _relativePCBase + edx]    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 79                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jgreatereq - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 80                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jnless - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 81                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jnlesseq - _relativePCBase + edx]    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 82                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jngreater - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 83                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_jngreatereq - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 84                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_loop_hint - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 85                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_switch_imm - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 86                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_switch_char - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 87                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_switch_string - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 88                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_new_func - _relativePCBase + edx]    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 89                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_new_func_exp - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 90                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_call - _relativePCBase + edx]        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 91                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_call_eval - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 92                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_call_varargs - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 93                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_ret - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 94                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_construct - _relativePCBase + edx]   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 95                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_construct_varargs - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 96                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_strcat - _relativePCBase + edx]      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 97                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_to_primitive - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 98                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_resolve_scope - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 99                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_from_scope - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 100                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_to_scope - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 101                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_from_arguments - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 102                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_put_to_arguments - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 103                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_push_with_scope - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 104                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_create_lexical_environment - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 105                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_parent_scope - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 106                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_catch - _relativePCBase + edx]       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 107                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_throw - _relativePCBase + edx]       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 108                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_throw_static_error - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 109                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_debug - _relativePCBase + edx]       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 110                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_profile_will_call - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 111                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_profile_did_call - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 112                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_end - _relativePCBase + edx]         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 113                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_profile_type - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 114                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_profile_control_flow - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 115                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_enumerable_length - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 116                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_has_indexed_property - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 117                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_has_structure_property - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 118                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_has_generic_property - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 119                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_direct_pname - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 120                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_get_property_enumerator - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 121                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_enumerator_structure_pname - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 122                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_enumerator_generic_pname - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 123                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_op_to_index_string - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 124                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_program_prologue - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 125                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_eval_prologue - _relativePCBase + edx]  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 126                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_function_for_call_prologue - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 127                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_function_for_construct_prologue - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 128                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_function_for_call_arity_check - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 129                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_function_for_construct_arity_check - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 130                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_generic_return_point - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 131                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_throw_from_slow_path_trampoline - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 132                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_throw_during_call_trampoline - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 133                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_native_call_trampoline - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 134                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_llint_native_construct_trampoline - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 135                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    lea eax, [_handleUncaughtException - _relativePCBase + edx] ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:839
    mov ecx, 136                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:840
    mov dword ptr [0 + edi + ecx * 4], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:841
    pop ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:360
    pop edi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:361
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:362
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:442
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:879

  _llint_program_prologue:
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:385
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568
    add dword ptr [332 + edx], 5                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:592
    js _offlineasm_prologue__continue
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:598
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_entry_osr
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:600
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:602
    jz _offlineasm_prologue__recover
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:603
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:611
    jmp eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:613

  _offlineasm_prologue__recover:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568

  _offlineasm_prologue__continue:
    mov esi, dword ptr [60 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:626
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    add eax, ebp
    mov ecx, dword ptr [56 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    cmp dword ptr [9380 + ecx], eax                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
    jbe _offlineasm_prologue__stackHeightOK
    sub esp, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_stack_check
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    test  edx, edx                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
    je _offlineasm_prologue__stackHeightOKGetCodeBlock
    mov ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _offlineasm_prologue__stackHeightOKGetCodeBlock:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647
    add eax, ebp

  _offlineasm_prologue__stackHeightOK:
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_eval_prologue:
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:385
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568
    add dword ptr [332 + edx], 5                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:592
    js _offlineasm_0_prologue__continue
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:598
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_entry_osr
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:600
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:602
    jz _offlineasm_0_prologue__recover
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:603
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:611
    jmp eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:613

  _offlineasm_0_prologue__recover:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568

  _offlineasm_0_prologue__continue:
    mov esi, dword ptr [60 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:626
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    add eax, ebp
    mov ecx, dword ptr [56 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    cmp dword ptr [9380 + ecx], eax                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
    jbe _offlineasm_0_prologue__stackHeightOK
    sub esp, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_stack_check
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    test  edx, edx                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
    je _offlineasm_0_prologue__stackHeightOKGetCodeBlock
    mov ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _offlineasm_0_prologue__stackHeightOKGetCodeBlock:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647
    add eax, ebp

  _offlineasm_0_prologue__stackHeightOK:
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_function_for_call_prologue:
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:385
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:551
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    mov edx, dword ptr [100 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554
    add dword ptr [332 + edx], 5                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:592
    js _offlineasm_1_prologue__continue
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:598
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_entry_osr_function_for_call
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:600
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:602
    jz _offlineasm_1_prologue__recover
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:603
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:611
    jmp eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:613

  _offlineasm_1_prologue__recover:
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:551
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    mov edx, dword ptr [100 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554

  _offlineasm_1_prologue__continue:
    mov dword ptr [8 + ebp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:572
    mov esi, dword ptr [60 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:626
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    add eax, ebp
    mov ecx, dword ptr [56 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    cmp dword ptr [9380 + ecx], eax                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
    jbe _offlineasm_1_prologue__stackHeightOK
    sub esp, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_stack_check
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    test  edx, edx                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
    je _offlineasm_1_prologue__stackHeightOKGetCodeBlock
    mov ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _offlineasm_1_prologue__stackHeightOKGetCodeBlock:
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:551
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    mov edx, dword ptr [100 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647
    add eax, ebp

  _offlineasm_1_prologue__stackHeightOK:
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    mov eax, dword ptr [44 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:664
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:665
    cmp eax, 0
    jge _offlineasm_functionInitialization__assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_functionInitialization__assert__ok:
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:666
    jz _offlineasm_functionInitialization__argumentProfileDone
    mov ebx, dword ptr [184 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:667
    imul ecx, eax, 32                                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:668
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669
    add ebx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:670

  _offlineasm_functionInitialization__argumentProfileLoop:
    mov ecx, dword ptr [28 + ebp + eax * 1]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:677
    sub ebx, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:678
    mov dword ptr [20 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:679
    mov ecx, dword ptr [24 + ebp + eax * 1]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:680
    mov dword ptr [16 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:681
    add eax, -8                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:683
    jnz _offlineasm_functionInitialization__argumentProfileLoop

  _offlineasm_functionInitialization__argumentProfileDone:
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_function_for_construct_prologue:
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:385
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:561
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    mov edx, dword ptr [104 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564
    add dword ptr [332 + edx], 5                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:592
    js _offlineasm_2_prologue__continue
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:598
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_entry_osr_function_for_construct
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:600
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:602
    jz _offlineasm_2_prologue__recover
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:603
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:611
    jmp eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:613

  _offlineasm_2_prologue__recover:
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:561
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    mov edx, dword ptr [104 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564

  _offlineasm_2_prologue__continue:
    mov dword ptr [8 + ebp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:572
    mov esi, dword ptr [60 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:626
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    add eax, ebp
    mov ecx, dword ptr [56 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    cmp dword ptr [9380 + ecx], eax                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
    jbe _offlineasm_2_prologue__stackHeightOK
    sub esp, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_stack_check
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    test  edx, edx                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
    je _offlineasm_2_prologue__stackHeightOKGetCodeBlock
    mov ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _offlineasm_2_prologue__stackHeightOKGetCodeBlock:
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:561
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    mov edx, dword ptr [104 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647
    add eax, ebp

  _offlineasm_2_prologue__stackHeightOK:
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    mov eax, dword ptr [44 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
    add eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:664
    cmp eax, 0                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:665
    jge _offlineasm_functionInitialization__2_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_functionInitialization__2_assert__ok:
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:666
    jz _offlineasm_2_functionInitialization__argumentProfileDone
    mov ebx, dword ptr [184 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:667
    imul ecx, eax, 32                                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:668
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669
    add ebx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:670

  _offlineasm_2_functionInitialization__argumentProfileLoop:
    mov ecx, dword ptr [36 + ebp + eax * 1]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:677
    sub ebx, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:678
    mov dword ptr [52 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:679
    mov ecx, dword ptr [32 + ebp + eax * 1]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:680
    mov dword ptr [48 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:681
    add eax, -8                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:683
    jnz _offlineasm_2_functionInitialization__argumentProfileLoop

  _offlineasm_2_functionInitialization__argumentProfileDone:
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_function_for_call_arity_check:
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:385
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:551
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    mov edx, dword ptr [100 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554
    add dword ptr [332 + edx], 5                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:592
    js _offlineasm_3_prologue__continue
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:598
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_entry_osr_function_for_call_arityCheck
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:600
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:602
    jz _offlineasm_3_prologue__recover
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:603
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:611
    jmp eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:613

  _offlineasm_3_prologue__recover:
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:551
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    mov edx, dword ptr [100 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554

  _offlineasm_3_prologue__continue:
    mov dword ptr [8 + ebp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:572
    mov esi, dword ptr [60 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:626
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    add eax, ebp
    mov ecx, dword ptr [56 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    cmp dword ptr [9380 + ecx], eax                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
    jbe _offlineasm_3_prologue__stackHeightOK
    sub esp, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_stack_check
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    test  edx, edx                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
    je _offlineasm_3_prologue__stackHeightOKGetCodeBlock
    mov ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _offlineasm_3_prologue__stackHeightOKGetCodeBlock:
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:551
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    mov edx, dword ptr [100 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647
    add eax, ebp

  _offlineasm_3_prologue__stackHeightOK:
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    mov eax, dword ptr [24 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:633
    cmp eax, dword ptr [44 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:634
    jae _offlineasm_functionForCallBegin
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_call_arityCheck
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:636
    jz _offlineasm_functionArityCheck__noError
    mov ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:637
    jmp _llint_throw_from_slow_path_trampoline               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:638

  _offlineasm_functionArityCheck__noError:
    mov ecx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:642
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:643
    jz _offlineasm_functionArityCheck__proceedInline
    mov esi, dword ptr [8 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:645
    mov eax, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:646
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:647
    ffree st(1)
    call ecx
    mov eax, dword ptr [4 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:649
    mov eax, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:650
    jmp _offlineasm_functionArityCheck__continue             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:652

  _offlineasm_functionArityCheck__proceedInline:
    mov edx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:655
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:656
    jz _offlineasm_functionArityCheck__continue
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:658
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:659
    sal edx, 1
    neg edx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:660
    mov ebx, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:661
    mov ecx, dword ptr [24 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:662
    add ecx, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:663

  _offlineasm_functionArityCheck__copyLoop:
    mov eax, dword ptr [0 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:665
    mov dword ptr [0 + ebx + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:666
    mov eax, dword ptr [4 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:667
    mov dword ptr [4 + ebx + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:668
    add ebx, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:669
    sub ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:670
    jnz _offlineasm_functionArityCheck__copyLoop
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:672
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:673
    mov ecx, edx

  _offlineasm_functionArityCheck__fillLoop:
    xor eax, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:675
    mov dword ptr [0 + ebx + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:676
    mov eax, -4                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:677
    mov dword ptr [4 + ebx + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:678
    add ebx, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:679
    add ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:680
    jnz _offlineasm_functionArityCheck__fillLoop
    sal edx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:682
    add ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:683
    add esp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:684

  _offlineasm_functionArityCheck__continue:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:687
    mov esi, dword ptr [60 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:688
    jmp _offlineasm_functionForCallBegin                     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:689

  _offlineasm_functionForCallBegin:
    mov eax, dword ptr [44 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:664
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:665
    cmp eax, 0
    jge _offlineasm_functionInitialization__3_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_functionInitialization__3_assert__ok:
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:666
    jz _offlineasm_3_functionInitialization__argumentProfileDone
    mov ebx, dword ptr [184 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:667
    imul ecx, eax, 32                                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:668
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669
    add ebx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:670

  _offlineasm_3_functionInitialization__argumentProfileLoop:
    mov ecx, dword ptr [28 + ebp + eax * 1]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:677
    sub ebx, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:678
    mov dword ptr [20 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:679
    mov ecx, dword ptr [24 + ebp + eax * 1]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:680
    mov dword ptr [16 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:681
    add eax, -8                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:683
    jnz _offlineasm_3_functionInitialization__argumentProfileLoop

  _offlineasm_3_functionInitialization__argumentProfileDone:
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_function_for_construct_arity_check:
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:385
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:561
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    mov edx, dword ptr [104 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564
    add dword ptr [332 + edx], 5                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:592
    js _offlineasm_4_prologue__continue
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:598
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_entry_osr_function_for_construct_arityCheck
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:600
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:602
    jz _offlineasm_4_prologue__recover
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:603
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:611
    jmp eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:613

  _offlineasm_4_prologue__recover:
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:561
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    mov edx, dword ptr [104 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564

  _offlineasm_4_prologue__continue:
    mov dword ptr [8 + ebp], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:572
    mov esi, dword ptr [60 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:626
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    add eax, ebp
    mov ecx, dword ptr [56 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    cmp dword ptr [9380 + ecx], eax                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
    jbe _offlineasm_4_prologue__stackHeightOK
    sub esp, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_stack_check
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    test  edx, edx                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
    je _offlineasm_4_prologue__stackHeightOKGetCodeBlock
    mov ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _offlineasm_4_prologue__stackHeightOKGetCodeBlock:
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:561
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    mov edx, dword ptr [104 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564
    mov eax, dword ptr [28 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add eax, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    neg eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647
    add eax, ebp

  _offlineasm_4_prologue__stackHeightOK:
    mov esp, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    mov eax, dword ptr [24 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:633
    cmp eax, dword ptr [44 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:634
    jae _offlineasm_functionForConstructBegin
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_construct_arityCheck
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:636
    jz _offlineasm_4_functionArityCheck__noError
    mov ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:637
    jmp _llint_throw_from_slow_path_trampoline               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:638

  _offlineasm_4_functionArityCheck__noError:
    mov ecx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:642
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:643
    jz _offlineasm_4_functionArityCheck__proceedInline
    mov esi, dword ptr [8 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:645
    mov eax, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:646
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:647
    ffree st(1)
    call ecx
    mov eax, dword ptr [4 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:649
    mov eax, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:650
    jmp _offlineasm_4_functionArityCheck__continue           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:652

  _offlineasm_4_functionArityCheck__proceedInline:
    mov edx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:655
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:656
    jz _offlineasm_4_functionArityCheck__continue
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:658
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:659
    sal edx, 1
    neg edx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:660
    mov ebx, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:661
    mov ecx, dword ptr [24 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:662
    add ecx, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:663

  _offlineasm_4_functionArityCheck__copyLoop:
    mov eax, dword ptr [0 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:665
    mov dword ptr [0 + ebx + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:666
    mov eax, dword ptr [4 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:667
    mov dword ptr [4 + ebx + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:668
    add ebx, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:669
    sub ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:670
    jnz _offlineasm_4_functionArityCheck__copyLoop
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:672
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:673
    mov ecx, edx

  _offlineasm_4_functionArityCheck__fillLoop:
    xor eax, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:675
    mov dword ptr [0 + ebx + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:676
    mov eax, -4                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:677
    mov dword ptr [4 + ebx + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:678
    add ebx, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:679
    add ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:680
    jnz _offlineasm_4_functionArityCheck__fillLoop
    sal edx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:682
    add ebp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:683
    add esp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:684

  _offlineasm_4_functionArityCheck__continue:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:687
    mov esi, dword ptr [60 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:688
    jmp _offlineasm_functionForConstructBegin                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:689

  _offlineasm_functionForConstructBegin:
    mov eax, dword ptr [44 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
    add eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:664
    cmp eax, 0                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:665
    jge _offlineasm_functionInitialization__4_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_functionInitialization__4_assert__ok:
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:666
    jz _offlineasm_4_functionInitialization__argumentProfileDone
    mov ebx, dword ptr [184 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:667
    imul ecx, eax, 32                                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:668
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669
    add ebx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:670

  _offlineasm_4_functionInitialization__argumentProfileLoop:
    mov ecx, dword ptr [36 + ebp + eax * 1]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:677
    sub ebx, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:678
    mov dword ptr [52 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:679
    mov ecx, dword ptr [32 + ebp + eax * 1]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:680
    mov dword ptr [48 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:681
    add eax, -8                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:683
    jnz _offlineasm_4_functionInitialization__argumentProfileLoop

  _offlineasm_4_functionInitialization__argumentProfileDone:
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _handleUncaughtException:
    mov ebx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:391
    and ebx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:392
    mov ebx, dword ptr [180 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:393
    mov ebp, dword ptr [9224 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:394
    mov ebp, dword ptr [0 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:396
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    sub esp, 32
    mov ebx, dword ptr [0 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:405
    mov esi, dword ptr [4 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:406
    mov dword ptr [5224 + ebx], esi                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:407
    mov esi, dword ptr [8 + esp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:408
    mov dword ptr [5220 + ebx], esi                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:409
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:415
    sub esp, 12
    pop ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:360
    pop edi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:361
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:362
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:442
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:420

  _llint_op_enter:
    mov ecx, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:251
    and ecx, 15
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:253
    jz _offlineasm_4_checkStackPointerAlignment__stackPointerOkay
    mov ecx, 3735879905                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:254
    int 3                                                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:255

  _offlineasm_4_checkStackPointerAlignment__stackPointerOkay:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:707
    mov ecx, dword ptr [32 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:708
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:709
    jz _offlineasm_opEnterDone
    mov eax, -4                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:710
    xor edx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:711
    neg ecx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:712

  _offlineasm_opEnterLoop:
    mov dword ptr [4 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:714
    mov dword ptr [0 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:715
    add ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:716
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:717
    jnz _offlineasm_opEnterLoop

  _offlineasm_opEnterDone:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_enter
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_scope:
    mov eax, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:725
    mov eax, dword ptr [16 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:726
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:728
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:729
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_create_this:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:735
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:736
    mov edi, dword ptr [24 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:737
    test edi, edi                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:738
    jz _offlineasm_opCreateThisSlow
    mov edx, dword ptr [8 + edi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:739
    mov ecx, dword ptr [12 + edi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:740
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:741
    jz _offlineasm_opCreateThisSlow
    mov edi, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    cmp edi, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:743
    je _offlineasm_hasSeenMultipleCallee
    cmp edi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:744
    jne _offlineasm_opCreateThisSlow

  _offlineasm_hasSeenMultipleCallee:
    mov eax, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:693
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:694
    jz _offlineasm_opCreateThisSlow
    mov ebx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:697
    mov dword ptr [0 + edx], ebx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:698
    mov dword ptr [8 + eax], 0                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:701
    mov dword ptr [0 + eax], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:563
    mov ebx, dword ptr [12 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:565
    mov dword ptr [4 + eax], ebx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:566
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:747
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:748
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:749
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opCreateThisSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_create_this
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_to_this:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:759
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:760
    jne _offlineasm_opToThisSlow
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:761
    cmp byte ptr [5 + eax], 19                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:762
    jne _offlineasm_opToThisSlow
    mov ecx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    cmp dword ptr [0 + eax], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:764
    jne _offlineasm_opToThisSlow
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opToThisSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_to_this
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_new_object:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov edx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:775
    mov ecx, dword ptr [4 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:776
    mov eax, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:693
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:694
    jz _offlineasm_opNewObjectSlow
    mov ebx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:697
    mov dword ptr [0 + edx], ebx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:698
    mov dword ptr [8 + eax], 0                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:701
    mov dword ptr [0 + eax], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:563
    mov ebx, dword ptr [12 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:565
    mov dword ptr [4 + eax], ebx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:566
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:778
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:779
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:780
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opNewObjectSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_new_object
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_check_tdz:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_loadConstantOrVariableTag__constant
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_loadConstantOrVariableTag__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_loadConstantOrVariableTag__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov edx, dword ptr [4 + edx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_loadConstantOrVariableTag__done:
    cmp edx, -6                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:792
    jne _offlineasm_opNotTDZ
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_throw_tdz_error
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155

  _offlineasm_opNotTDZ:
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_mov:
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:801
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:802
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_loadConstantOrVariable__done             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_loadConstantOrVariable__done:
    mov dword ptr [4 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:804
    mov dword ptr [0 + ebp + eax * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:805
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_not:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:811
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:812
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_4_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_4_loadConstantOrVariable__done           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_4_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_4_loadConstantOrVariable__done:
    cmp ecx, -2                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:814
    jne _offlineasm_opNotSlow
    xor ebx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:815
    mov dword ptr [4 + ebp + edx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:816
    mov dword ptr [0 + ebp + edx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:817
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opNotSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_not
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_eq:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:827
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:828
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_5_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_5_loadConstantOrVariable__done           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_5_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_5_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_loadConstantOrVariable2Reg__done         ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_loadConstantOrVariable2Reg__done:
    cmp ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:831
    jne _offlineasm_opEqSlow
    cmp ecx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:832
    je _offlineasm_opEqSlow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:833
    jb _offlineasm_opEqSlow
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:834
    cmp eax, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:835
    sete al
    movzx eax, al
    mov dword ptr [4 + ebp + ecx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:836
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:837
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opEqSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_eq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_eq_null:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:847
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:848
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:544
    jl _offlineasm_assertNotConstant__5_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_assertNotConstant__5_assert__ok:
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:850
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:851
    cmp edx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:852
    jne _offlineasm_opEqNullImmediate
    test byte ptr [6 + eax], 1                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:853
    jnz _offlineasm_opEqNullMasqueradesAsUndefined
    xor edx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:854
    jmp _offlineasm_opEqNullNotImmediate                     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:855

  _offlineasm_opEqNullMasqueradesAsUndefined:
    mov edx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:857
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:858
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:859
    cmp dword ptr [20 + edx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:860
    sete dl
    movzx edx, dl
    jmp _offlineasm_opEqNullNotImmediate                     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:861

  _offlineasm_opEqNullImmediate:
    cmp edx, -3                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:863
    sete cl
    movzx ecx, cl
    cmp edx, -4                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:864
    sete dl
    movzx edx, dl
    or edx, ecx                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:865

  _offlineasm_opEqNullNotImmediate:
    mov dword ptr [4 + ebp + ebx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:867
    mov dword ptr [0 + ebp + ebx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:868
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_neq:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:874
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:875
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_6_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_6_loadConstantOrVariable__done           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_6_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_6_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_6_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_6_loadConstantOrVariable2Reg__done       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_6_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_6_loadConstantOrVariable2Reg__done:
    cmp ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:878
    jne _offlineasm_opNeqSlow
    cmp ecx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:879
    je _offlineasm_opNeqSlow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:880
    jb _offlineasm_opNeqSlow
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:881
    cmp eax, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:882
    setne al
    movzx eax, al
    mov dword ptr [4 + ebp + ecx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:883
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:884
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opNeqSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_neq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_neq_null:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:894
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:895
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:544
    jl _offlineasm_assertNotConstant__6_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_assertNotConstant__6_assert__ok:
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:897
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:898
    cmp edx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:899
    jne _offlineasm_opNeqNullImmediate
    test byte ptr [6 + eax], 1                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:900
    jnz _offlineasm_opNeqNullMasqueradesAsUndefined
    mov edx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:901
    jmp _offlineasm_opNeqNullNotImmediate                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:902

  _offlineasm_opNeqNullMasqueradesAsUndefined:
    mov edx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:904
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:905
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:906
    cmp dword ptr [20 + edx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:907
    setne dl
    movzx edx, dl
    jmp _offlineasm_opNeqNullNotImmediate                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:908

  _offlineasm_opNeqNullImmediate:
    cmp edx, -3                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:910
    setne cl
    movzx ecx, cl
    cmp edx, -4                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:911
    setne dl
    movzx edx, dl
    and edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:912

  _offlineasm_opNeqNullNotImmediate:
    mov dword ptr [4 + ebp + ebx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:914
    mov dword ptr [0 + ebp + ebx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:915
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_stricteq:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:920
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:921
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_strictEq__7_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_strictEq__7_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_strictEq__7_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_strictEq__7_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_strictEq__7_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_strictEq__7_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_strictEq__7_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_strictEq__7_loadConstantOrVariable2Reg__done:
    cmp ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:924
    jne _offlineasm_strictEq__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:925
    jb _offlineasm_strictEq__slow
    cmp ecx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:926
    jne _offlineasm_strictEq__notStringOrSymbol
    cmp byte ptr [5 + eax], 18                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:927
    jae _offlineasm_strictEq__notStringOrSymbol
    cmp byte ptr [5 + edx], 18                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:928
    jb _offlineasm_strictEq__slow

  _offlineasm_strictEq__notStringOrSymbol:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:930
    cmp eax, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:943
    sete al
    movzx eax, al
    mov dword ptr [4 + ebp + ecx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:932
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:933
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_strictEq__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_stricteq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_nstricteq:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:920
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:921
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_strictEq__8_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_strictEq__8_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_strictEq__8_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_strictEq__8_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_strictEq__8_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_strictEq__8_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_strictEq__8_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_strictEq__8_loadConstantOrVariable2Reg__done:
    cmp ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:924
    jne _offlineasm_8_strictEq__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:925
    jb _offlineasm_8_strictEq__slow
    cmp ecx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:926
    jne _offlineasm_8_strictEq__notStringOrSymbol
    cmp byte ptr [5 + eax], 18                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:927
    jae _offlineasm_8_strictEq__notStringOrSymbol
    cmp byte ptr [5 + edx], 18                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:928
    jb _offlineasm_8_strictEq__slow

  _offlineasm_8_strictEq__notStringOrSymbol:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:930
    cmp eax, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:948
    setne al
    movzx eax, al
    mov dword ptr [4 + ebp + ecx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:932
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:933
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_8_strictEq__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_nstricteq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_inc:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:953
    cmp dword ptr [4 + ebp + eax * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:954
    jne _offlineasm_opIncSlow
    mov edx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:955
    add edx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:956
    jo _offlineasm_opIncSlow
    mov dword ptr [0 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:957
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opIncSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_inc
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_dec:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:967
    cmp dword ptr [4 + ebp + eax * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:968
    jne _offlineasm_opDecSlow
    mov edx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:969
    sub edx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:970
    jo _offlineasm_opDecSlow
    mov dword ptr [0 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:971
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opDecSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_dec
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_to_number:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:981
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:982
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_9_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_9_loadConstantOrVariable__done           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_9_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_9_loadConstantOrVariable__done:
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:984
    je _offlineasm_opToNumberIsInt
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:985
    jae _offlineasm_opToNumberSlow

  _offlineasm_opToNumberIsInt:
    mov dword ptr [4 + ebp + edx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:987
    mov dword ptr [0 + ebp + edx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:988
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opToNumberSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_to_number
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_to_string:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:998
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:999
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_10_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_10_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_10_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_10_loadConstantOrVariable__done:
    cmp ecx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1001
    jne _offlineasm_opToStringSlow
    cmp byte ptr [5 + ebx], 6                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1002
    jne _offlineasm_opToStringSlow

  _offlineasm_opToStringIsString:
    mov dword ptr [4 + ebp + edx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1004
    mov dword ptr [0 + ebp + edx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1005
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opToStringSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_to_string
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_negate:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1015
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1016
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_11_loadConstantOrVariable__constant
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ecx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_11_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_11_loadConstantOrVariable__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov edx, dword ptr [4 + ecx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ecx, dword ptr [0 + ecx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_11_loadConstantOrVariable__done:
    cmp edx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1018
    jne _offlineasm_opNegateSrcNotInt
    test ecx, 2147483647                                     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1019
    jz _offlineasm_opNegateSlow
    neg ecx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1020
    mov dword ptr [4 + ebp + ebx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1021
    mov dword ptr [0 + ebp + ebx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1022
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opNegateSrcNotInt:
    cmp edx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1025
    ja _offlineasm_opNegateSlow
    xor edx, 2147483648                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1026
    mov dword ptr [4 + ebp + ebx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1027
    mov dword ptr [0 + ebp + ebx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1028
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opNegateSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_negate
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_add:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1037
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1038
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable2Reg__done:
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1041
    jne _offlineasm_binaryOp__binaryOpCustomStore__op1NotInt
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1042
    jne _offlineasm_binaryOp__binaryOpCustomStore__op2NotInt
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1043
    add eax, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1091
    jo _offlineasm_binaryOp__binaryOpCustomStore__slow
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1082
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1083
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_binaryOp__binaryOpCustomStore__op1NotInt:
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1049
    ja _offlineasm_binaryOp__binaryOpCustomStore__slow
    cmp ebx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1050
    jb _offlineasm_binaryOp__binaryOpCustomStore__op1NotIntOp2Double
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1051
    jne _offlineasm_binaryOp__binaryOpCustomStore__slow
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1052
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_binaryOp__binaryOpCustomStore__op1NotIntReady ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1053

  _offlineasm_binaryOp__binaryOpCustomStore__op1NotIntOp2Double:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1055
    mov [-4 + esp], ebx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_binaryOp__binaryOpCustomStore__op1NotIntReady:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1057
    mov [-8 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1058
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(1)
    fadd st, st(1)                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1092
    fst qword ptr [0 + ebp + edx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1060
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_binaryOp__binaryOpCustomStore__op2NotInt:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1065
    cmp ebx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1066
    ja _offlineasm_binaryOp__binaryOpCustomStore__slow
    mov [-4 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1067
    fild dword ptr [-4 + esp]
    fstp st(1)
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1068
    mov [-4 + esp], ebx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fadd st, st(1)                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1092
    fst qword ptr [0 + ebp + ecx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1070
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_binaryOp__binaryOpCustomStore__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_add
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_mul:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1037
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1038
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable2Reg__done:
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1041
    jne _offlineasm_13_binaryOpCustomStore__op1NotInt
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1042
    jne _offlineasm_13_binaryOpCustomStore__op2NotInt
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1043
    mov ebx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1101
    imul ebx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1102
    jo _offlineasm_13_binaryOpCustomStore__slow
    test ebx, ebx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1103
    jnz _offlineasm_binaryOpCustomStore__integerOperationAndStore__done
    cmp edx, 0                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1104
    jl _offlineasm_13_binaryOpCustomStore__slow
    cmp eax, 0                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1105
    jl _offlineasm_13_binaryOpCustomStore__slow

  _offlineasm_binaryOpCustomStore__integerOperationAndStore__done:
    mov dword ptr [4 + ebp + ecx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1107
    mov dword ptr [0 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1108
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_13_binaryOpCustomStore__op1NotInt:
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1049
    ja _offlineasm_13_binaryOpCustomStore__slow
    cmp ebx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1050
    jb _offlineasm_13_binaryOpCustomStore__op1NotIntOp2Double
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1051
    jne _offlineasm_13_binaryOpCustomStore__slow
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1052
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_13_binaryOpCustomStore__op1NotIntReady   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1053

  _offlineasm_13_binaryOpCustomStore__op1NotIntOp2Double:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1055
    mov [-4 + esp], ebx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_13_binaryOpCustomStore__op1NotIntReady:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1057
    mov [-8 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1058
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(1)
    fmul st, st(1)                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1110
    fst qword ptr [0 + ebp + edx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1060
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_13_binaryOpCustomStore__op2NotInt:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1065
    cmp ebx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1066
    ja _offlineasm_13_binaryOpCustomStore__slow
    mov [-4 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1067
    fild dword ptr [-4 + esp]
    fstp st(1)
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1068
    mov [-4 + esp], ebx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fmul st, st(1)                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1110
    fst qword ptr [0 + ebp + ecx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1070
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_13_binaryOpCustomStore__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_mul
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_sub:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1037
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1038
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_binaryOp__binaryOpCustomStore__14_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_binaryOp__binaryOpCustomStore__14_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_binaryOp__binaryOpCustomStore__14_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_binaryOp__binaryOpCustomStore__14_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_binaryOp__binaryOpCustomStore__14_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_binaryOp__binaryOpCustomStore__14_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_binaryOp__binaryOpCustomStore__14_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_binaryOp__binaryOpCustomStore__14_loadConstantOrVariable2Reg__done:
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1041
    jne _offlineasm_binaryOp__14_binaryOpCustomStore__op1NotInt
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1042
    jne _offlineasm_binaryOp__14_binaryOpCustomStore__op2NotInt
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1043
    sub eax, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1117
    jo _offlineasm_binaryOp__14_binaryOpCustomStore__slow
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1082
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1083
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_binaryOp__14_binaryOpCustomStore__op1NotInt:
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1049
    ja _offlineasm_binaryOp__14_binaryOpCustomStore__slow
    cmp ebx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1050
    jb _offlineasm_binaryOp__14_binaryOpCustomStore__op1NotIntOp2Double
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1051
    jne _offlineasm_binaryOp__14_binaryOpCustomStore__slow
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1052
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_binaryOp__14_binaryOpCustomStore__op1NotIntReady ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1053

  _offlineasm_binaryOp__14_binaryOpCustomStore__op1NotIntOp2Double:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1055
    mov [-4 + esp], ebx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_binaryOp__14_binaryOpCustomStore__op1NotIntReady:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1057
    mov [-8 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1058
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(1)
    fsub st, st(1)                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1118
    fst qword ptr [0 + ebp + edx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1060
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_binaryOp__14_binaryOpCustomStore__op2NotInt:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1065
    cmp ebx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1066
    ja _offlineasm_binaryOp__14_binaryOpCustomStore__slow
    mov [-4 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1067
    fild dword ptr [-4 + esp]
    fstp st(1)
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1068
    mov [-4 + esp], ebx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fsub st, st(1)                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1118
    fst qword ptr [0 + ebp + ecx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1070
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_binaryOp__14_binaryOpCustomStore__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_sub
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_div:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1037
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1038
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_binaryOpCustomStore__15_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_binaryOpCustomStore__15_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_binaryOpCustomStore__15_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_binaryOpCustomStore__15_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_binaryOpCustomStore__15_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_binaryOpCustomStore__15_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_binaryOpCustomStore__15_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_binaryOpCustomStore__15_loadConstantOrVariable2Reg__done:
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1041
    jne _offlineasm_15_binaryOpCustomStore__op1NotInt
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1042
    jne _offlineasm_15_binaryOpCustomStore__op2NotInt
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1043
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1126
    fild dword ptr [-4 + esp]
    fstp st(1)
    mov [-4 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1127
    fild dword ptr [-4 + esp]
    fstp st(2)
    fdiv st(1), st                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1128
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1129
    fistp dword ptr [-4 + esp]
    mov eax, [-4 + esp]
    test eax, eax
    je _offlineasm_binaryOpCustomStore__integerOperationAndStore__notInt
    fild dword ptr [-4 + esp]
    fucomip st(0), st(2)
    jp _offlineasm_binaryOpCustomStore__integerOperationAndStore__notInt
    jne _offlineasm_binaryOpCustomStore__integerOperationAndStore__notInt
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1130
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1131
    jmp _offlineasm_binaryOpCustomStore__15_integerOperationAndStore__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1132

  _offlineasm_binaryOpCustomStore__integerOperationAndStore__notInt:
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1134
    fstp qword ptr [0 + ebp + ecx * 8]

  _offlineasm_binaryOpCustomStore__15_integerOperationAndStore__done:
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_15_binaryOpCustomStore__op1NotInt:
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1049
    ja _offlineasm_15_binaryOpCustomStore__slow
    cmp ebx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1050
    jb _offlineasm_15_binaryOpCustomStore__op1NotIntOp2Double
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1051
    jne _offlineasm_15_binaryOpCustomStore__slow
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1052
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_15_binaryOpCustomStore__op1NotIntReady   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1053

  _offlineasm_15_binaryOpCustomStore__op1NotIntOp2Double:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1055
    mov [-4 + esp], ebx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_15_binaryOpCustomStore__op1NotIntReady:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1057
    mov [-8 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1058
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(1)
    fdiv st, st(1)                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1137
    fst qword ptr [0 + ebp + edx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1060
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_15_binaryOpCustomStore__op2NotInt:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1065
    cmp ebx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1066
    ja _offlineasm_15_binaryOpCustomStore__slow
    mov [-4 + esp], eax                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1067
    fild dword ptr [-4 + esp]
    fstp st(1)
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1068
    mov [-4 + esp], ebx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fdiv st, st(1)                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1137
    fst qword ptr [0 + ebp + ecx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1070
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_15_binaryOpCustomStore__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_div
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_lshift:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1142
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1143
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_bitOp__16_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_bitOp__16_loadConstantOrVariable__done   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_bitOp__16_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_bitOp__16_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_bitOp__16_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_bitOp__16_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_bitOp__16_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_bitOp__16_loadConstantOrVariable2Reg__done:
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1146
    jne _offlineasm_bitOp__slow
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1147
    jne _offlineasm_bitOp__slow
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1148
    xchg edx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1162
    sal eax, cl
    xchg edx, ecx
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1150
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1151
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_bitOp__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_lshift
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_rshift:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1142
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1143
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_bitOp__17_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_bitOp__17_loadConstantOrVariable__done   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_bitOp__17_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_bitOp__17_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_bitOp__17_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_bitOp__17_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_bitOp__17_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_bitOp__17_loadConstantOrVariable2Reg__done:
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1146
    jne _offlineasm_17_bitOp__slow
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1147
    jne _offlineasm_17_bitOp__slow
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1148
    xchg edx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1170
    sar eax, cl
    xchg edx, ecx
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1150
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1151
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_17_bitOp__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_rshift
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_urshift:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1142
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1143
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_bitOp__18_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_bitOp__18_loadConstantOrVariable__done   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_bitOp__18_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_bitOp__18_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_bitOp__18_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_bitOp__18_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_bitOp__18_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_bitOp__18_loadConstantOrVariable2Reg__done:
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1146
    jne _offlineasm_18_bitOp__slow
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1147
    jne _offlineasm_18_bitOp__slow
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1148
    xchg edx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1178
    shr eax, cl
    xchg edx, ecx
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1150
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1151
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_18_bitOp__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_urshift
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_unsigned:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1185
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1186
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_loadConstantOrVariablePayload__loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opUnsignedSlow
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_loadConstantOrVariablePayload__loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_loadConstantOrVariablePayload__loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opUnsignedSlow
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_loadConstantOrVariablePayload__loadConstantOrVariablePayloadTagCustom__done:
    cmp ecx, 0                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1188
    jl _offlineasm_opUnsignedSlow
    mov dword ptr [0 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1189
    mov dword ptr [4 + ebp + eax * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1190
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opUnsignedSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_unsigned
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_bitand:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1142
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1143
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_bitOp__19_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_bitOp__19_loadConstantOrVariable__done   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_bitOp__19_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_bitOp__19_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_bitOp__19_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_bitOp__19_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_bitOp__19_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_bitOp__19_loadConstantOrVariable2Reg__done:
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1146
    jne _offlineasm_19_bitOp__slow
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1147
    jne _offlineasm_19_bitOp__slow
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1148
    and eax, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1200
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1150
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1151
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_19_bitOp__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_bitand
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_bitxor:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1142
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1143
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_bitOp__20_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_bitOp__20_loadConstantOrVariable__done   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_bitOp__20_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_bitOp__20_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_bitOp__20_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_bitOp__20_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_bitOp__20_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_bitOp__20_loadConstantOrVariable2Reg__done:
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1146
    jne _offlineasm_20_bitOp__slow
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1147
    jne _offlineasm_20_bitOp__slow
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1148
    xor eax, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1208
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1150
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1151
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_20_bitOp__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_bitxor
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_bitor:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1142
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1143
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_bitOp__21_loadConstantOrVariable__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_bitOp__21_loadConstantOrVariable__done   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_bitOp__21_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ebx, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_bitOp__21_loadConstantOrVariable__done:
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_bitOp__21_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_bitOp__21_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_bitOp__21_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_bitOp__21_loadConstantOrVariable2Reg__done:
    cmp ebx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1146
    jne _offlineasm_21_bitOp__slow
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1147
    jne _offlineasm_21_bitOp__slow
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1148
    or eax, edx                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1216
    mov dword ptr [4 + ebp + ecx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1150
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1151
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_21_bitOp__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_bitor
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_check_has_instance:
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1223
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_loadConstantOrVariablePayload__21_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opCheckHasInstanceSlow
    mov eax, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_loadConstantOrVariablePayload__21_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_loadConstantOrVariablePayload__21_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opCheckHasInstanceSlow
    mov eax, dword ptr [0 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_loadConstantOrVariablePayload__21_loadConstantOrVariablePayloadTagCustom__done:
    test byte ptr [6 + eax], 8                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1225
    jz _offlineasm_opCheckHasInstanceSlow
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opCheckHasInstanceSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_check_has_instance
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_instanceof:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1236
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1237
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_loadConstantOrVariablePayload__22_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opInstanceofSlow
    mov edx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_loadConstantOrVariablePayload__22_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_loadConstantOrVariablePayload__22_loadConstantOrVariablePayloadTagCustom__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + edx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opInstanceofSlow
    mov edx, dword ptr [0 + edx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_loadConstantOrVariablePayload__22_loadConstantOrVariablePayloadTagCustom__done:
    cmp byte ptr [5 + edx], 18                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1239
    jb _offlineasm_opInstanceofSlow
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1240
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_loadConstantOrVariablePayload__23_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opInstanceofSlow
    mov ecx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_loadConstantOrVariablePayload__23_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_loadConstantOrVariablePayload__23_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opInstanceofSlow
    mov ecx, dword ptr [0 + ecx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_loadConstantOrVariablePayload__23_loadConstantOrVariablePayloadTagCustom__done:
    mov eax, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1244

  _offlineasm_opInstanceofLoop:
    mov ecx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1246
    mov ecx, dword ptr [24 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1247
    cmp ecx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1248
    je _offlineasm_opInstanceofDone
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1249
    jnz _offlineasm_opInstanceofLoop
    xor eax, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1251

  _offlineasm_opInstanceofDone:
    mov dword ptr [4 + ebp + ebx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1253
    mov dword ptr [0 + ebp + ebx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1254
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opInstanceofSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_instanceof
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_is_undefined:
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1264
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1265
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_23_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_23_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_23_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_23_loadConstantOrVariable__done:
    mov dword ptr [4 + ebp + eax * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1267
    cmp ecx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1268
    je _offlineasm_opIsUndefinedCell
    cmp ecx, -4                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1269
    sete bl
    movzx ebx, bl
    mov dword ptr [0 + ebp + eax * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1270
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opIsUndefinedCell:
    test byte ptr [6 + ebx], 1                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1273
    jnz _offlineasm_opIsUndefinedMasqueradesAsUndefined
    xor edx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1274
    mov dword ptr [0 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1275
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opIsUndefinedMasqueradesAsUndefined:
    mov edx, dword ptr [0 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1278
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1279
    mov ebx, dword ptr [20 + ebx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1280
    cmp dword ptr [20 + edx], ebx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1281
    sete dl
    movzx edx, dl
    mov dword ptr [0 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1282
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_is_boolean:
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1288
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1289
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_23_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_23_loadConstantOrVariableTag__done       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_23_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_23_loadConstantOrVariableTag__done:
    cmp eax, -2                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1291
    sete al
    movzx eax, al
    mov dword ptr [4 + ebp + ecx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1292
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1293
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_is_number:
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1299
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1300
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_24_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_24_loadConstantOrVariableTag__done       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_24_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_24_loadConstantOrVariableTag__done:
    mov dword ptr [4 + ebp + ecx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1302
    add eax, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1303
    cmp eax, -6                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1304
    setb dl
    movzx edx, dl
    mov dword ptr [0 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1305
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_is_string:
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1311
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1312
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_24_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_24_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_24_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_24_loadConstantOrVariable__done:
    mov dword ptr [4 + ebp + ecx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1314
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1315
    jne _offlineasm_opIsStringNotCell
    cmp byte ptr [5 + ebx], 6                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1316
    sete dl
    movzx edx, dl
    mov dword ptr [0 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1317
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opIsStringNotCell:
    mov dword ptr [0 + ebp + ecx * 8], 0                     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1320
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_is_object:
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1326
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1327
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_25_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_25_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_25_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_25_loadConstantOrVariable__done:
    mov dword ptr [4 + ebp + ecx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1329
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1330
    jne _offlineasm_opIsObjectNotCell
    cmp byte ptr [5 + ebx], 18                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1331
    setae dl
    movzx edx, dl
    mov dword ptr [0 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1332
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opIsObjectNotCell:
    mov dword ptr [0 + ebp + ecx * 8], 0                     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1335
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_by_id:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1380
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1381
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_getById__loadConstantOrVariablePayload__25_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_getById__opGetByIdSlow
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_getById__loadConstantOrVariablePayload__25_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_getById__loadConstantOrVariablePayload__25_loadConstantOrVariablePayloadTagCustom__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ebx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_getById__opGetByIdSlow
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_getById__loadConstantOrVariablePayload__25_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1383
    cmp dword ptr [0 + ebx], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1388
    jne _offlineasm_getById__opGetByIdSlow
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1389
    mov eax, dword ptr [4 + ebx + ecx * 1]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1390
    mov ecx, dword ptr [0 + ebx + ecx * 1]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1391
    mov dword ptr [4 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1392
    mov dword ptr [0 + ebp + edx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1393
    mov edx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + edx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + edx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_getById__opGetByIdSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_get_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_by_id_out_of_line:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1380
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1381
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_getById__loadConstantOrVariablePayload__26_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_26_getById__opGetByIdSlow
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_getById__loadConstantOrVariablePayload__26_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_getById__loadConstantOrVariablePayload__26_loadConstantOrVariablePayloadTagCustom__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ebx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_26_getById__opGetByIdSlow
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_getById__loadConstantOrVariablePayload__26_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1383
    mov eax, dword ptr [8 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1036
    cmp dword ptr [0 + ebx], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1388
    jne _offlineasm_26_getById__opGetByIdSlow
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1389
    mov ebx, dword ptr [4 + eax + ecx * 1]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1390
    mov ecx, dword ptr [0 + eax + ecx * 1]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1391
    mov dword ptr [4 + ebp + edx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1392
    mov dword ptr [0 + ebp + edx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1393
    mov edx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + edx], ebx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + edx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_26_getById__opGetByIdSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_get_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_array_length:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1413
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1414
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_loadConstantOrVariablePayload__27_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opGetArrayLengthSlow
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_loadConstantOrVariablePayload__27_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_loadConstantOrVariablePayload__27_loadConstantOrVariablePayloadTagCustom__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ebx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opGetArrayLengthSlow
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_loadConstantOrVariablePayload__27_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1416
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:514
    mov dword ptr [4 + edx], eax                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:515
    movzx ecx, byte ptr [4 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:516
    test ecx, 1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1418
    jz _offlineasm_opGetArrayLengthSlow
    test ecx, 30                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1419
    jz _offlineasm_opGetArrayLengthSlow
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1420
    mov eax, dword ptr [8 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1421
    mov eax, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1422
    cmp eax, 0                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1423
    jl _offlineasm_opGetArrayLengthSlow
    mov ecx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ecx], -1                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ecx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1425
    mov dword ptr [4 + ebp + edx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1426
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opGetArrayLengthSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_get_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_id:
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_putById__writeBarrierOnOperands__27_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_putById__writeBarrierOnOperands__27_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_putById__writeBarrierOnOperands__27_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_putById__writeBarrierOnOperands__27_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_putById__writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__28_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__28_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__28_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__28_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_putById__writeBarrierOnOperands__writeBarrierDone:
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1437
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1438
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putById__loadConstantOrVariablePayload__29_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putById__opPutByIdSlow
    mov eax, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putById__loadConstantOrVariablePayload__29_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putById__loadConstantOrVariablePayload__29_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putById__opPutByIdSlow
    mov eax, dword ptr [0 + eax + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putById__loadConstantOrVariablePayload__29_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1440
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1445
    jne _offlineasm_putById__opPutByIdSlow
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1446
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putById__getPropertyStorage__continuation__29_loadConstantOrVariable2Reg__constant
    mov ebx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putById__getPropertyStorage__continuation__29_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putById__getPropertyStorage__continuation__29_loadConstantOrVariable2Reg__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ebx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ebx, dword ptr [4 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putById__getPropertyStorage__continuation__29_loadConstantOrVariable2Reg__done:
    mov dword ptr [4 + eax + edx * 1], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1448
    mov dword ptr [0 + eax + edx * 1], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1449
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putById__opPutByIdSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_id_out_of_line:
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_putById__writeBarrierOnOperands__29_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_putById__writeBarrierOnOperands__29_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_putById__writeBarrierOnOperands__29_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_putById__writeBarrierOnOperands__29_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_putById__30_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__30_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putById__writeBarrierOnOperands__30_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__30_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__30_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putById__writeBarrierOnOperands__30_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__30_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_putById__writeBarrierOnOperands__30_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_putById__writeBarrierOnOperands__30_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_putById__30_writeBarrierOnOperands__writeBarrierDone:
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1437
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1438
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putById__loadConstantOrVariablePayload__31_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_31_putById__opPutByIdSlow
    mov eax, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putById__loadConstantOrVariablePayload__31_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putById__loadConstantOrVariablePayload__31_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_31_putById__opPutByIdSlow
    mov eax, dword ptr [0 + eax + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putById__loadConstantOrVariablePayload__31_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1440
    mov ebx, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1036
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1445
    jne _offlineasm_31_putById__opPutByIdSlow
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1446
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putById__getPropertyStorage__continuation__31_loadConstantOrVariable2Reg__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putById__getPropertyStorage__continuation__31_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putById__getPropertyStorage__continuation__31_loadConstantOrVariable2Reg__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add eax, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov eax, dword ptr [4 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putById__getPropertyStorage__continuation__31_loadConstantOrVariable2Reg__done:
    mov dword ptr [4 + ebx + edx * 1], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1448
    mov dword ptr [0 + ebx + edx * 1], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1449
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_31_putById__opPutByIdSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_id_transition_direct:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__32_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__32_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__32_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__32_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__32_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__32_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_putByIdTransition__32_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_putByIdTransition__32_writeBarrierOnOperand__writeBarrierDone:
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1469
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1470
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByIdTransition__loadConstantOrVariablePayload__33_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__opPutByIdSlow
    mov eax, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByIdTransition__loadConstantOrVariablePayload__33_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByIdTransition__loadConstantOrVariablePayload__33_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__opPutByIdSlow
    mov eax, dword ptr [0 + eax + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByIdTransition__loadConstantOrVariablePayload__33_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1472
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1473
    jne _offlineasm_putByIdTransition__opPutByIdSlow
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1475
    lea ebx, [edx + eax]                                     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1480
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByIdTransition__getPropertyStorage__continuation__33_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByIdTransition__getPropertyStorage__continuation__33_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByIdTransition__getPropertyStorage__continuation__33_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByIdTransition__getPropertyStorage__continuation__33_loadConstantOrVariable2Reg__done:
    mov dword ptr [4 + ebx], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1482
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1483
    mov dword ptr [0 + ebx], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1484
    mov dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1485
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putByIdTransition__opPutByIdSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_id_transition_direct_out_of_line:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__34_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__34_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__34_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__34_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__34_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__34_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_putByIdTransition__34_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_putByIdTransition__34_writeBarrierOnOperand__writeBarrierDone:
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1469
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1470
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByIdTransition__loadConstantOrVariablePayload__35_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_35_putByIdTransition__opPutByIdSlow
    mov eax, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByIdTransition__loadConstantOrVariablePayload__35_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByIdTransition__loadConstantOrVariablePayload__35_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_35_putByIdTransition__opPutByIdSlow
    mov eax, dword ptr [0 + eax + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByIdTransition__loadConstantOrVariablePayload__35_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1472
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1473
    jne _offlineasm_35_putByIdTransition__opPutByIdSlow
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1475
    mov ebx, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1036
    add ebx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1480
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByIdTransition__getPropertyStorage__continuation__35_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByIdTransition__getPropertyStorage__continuation__35_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByIdTransition__getPropertyStorage__continuation__35_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByIdTransition__getPropertyStorage__continuation__35_loadConstantOrVariable2Reg__done:
    mov dword ptr [4 + ebx], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1482
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1483
    mov dword ptr [0 + ebx], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1484
    mov dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1485
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_35_putByIdTransition__opPutByIdSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_id_transition_normal:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__36_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__36_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__36_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__36_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__36_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__36_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_putByIdTransition__36_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_putByIdTransition__36_writeBarrierOnOperand__writeBarrierDone:
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1469
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1470
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByIdTransition__loadConstantOrVariablePayload__37_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_38_putByIdTransition__opPutByIdSlow
    mov eax, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByIdTransition__loadConstantOrVariablePayload__37_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByIdTransition__loadConstantOrVariablePayload__37_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_38_putByIdTransition__opPutByIdSlow
    mov eax, dword ptr [0 + eax + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByIdTransition__loadConstantOrVariablePayload__37_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1472
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1473
    jne _offlineasm_38_putByIdTransition__opPutByIdSlow
    mov ebx, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1500
    test ebx, ebx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1501
    jnz _offlineasm_putByIdTransition__additionalChecks__37_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_putByIdTransition__additionalChecks__37_assert__ok:
    mov ebx, dword ptr [8 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1502
    test ebx, ebx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1503
    jnz _offlineasm_putByIdTransition__additionalChecks__38_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_putByIdTransition__additionalChecks__38_assert__ok:
    cmp dword ptr [28 + edx], -3                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1504
    je _offlineasm_putByIdTransition__additionalChecks__done

  _offlineasm_putByIdTransition__additionalChecks__loop:
    mov edx, dword ptr [24 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1506
    mov edx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1507
    cmp edx, dword ptr [0 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1508
    jne _offlineasm_38_putByIdTransition__opPutByIdSlow
    add ebx, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1509
    cmp dword ptr [28 + edx], -3                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1510
    jne _offlineasm_putByIdTransition__additionalChecks__loop

  _offlineasm_putByIdTransition__additionalChecks__done:
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1475
    lea ebx, [edx + eax]                                     ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1480
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByIdTransition__getPropertyStorage__continuation__38_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByIdTransition__getPropertyStorage__continuation__38_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByIdTransition__getPropertyStorage__continuation__38_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByIdTransition__getPropertyStorage__continuation__38_loadConstantOrVariable2Reg__done:
    mov dword ptr [4 + ebx], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1482
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1483
    mov dword ptr [0 + ebx], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1484
    mov dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1485
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_38_putByIdTransition__opPutByIdSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_id_transition_normal_out_of_line:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__38_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__38_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__38_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__38_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByIdTransition__38_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariablePayload__38_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_putByIdTransition__38_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_putByIdTransition__38_writeBarrierOnOperand__writeBarrierDone:
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1469
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1470
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByIdTransition__loadConstantOrVariablePayload__39_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_40_putByIdTransition__opPutByIdSlow
    mov eax, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByIdTransition__loadConstantOrVariablePayload__39_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByIdTransition__loadConstantOrVariablePayload__39_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + ebx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_40_putByIdTransition__opPutByIdSlow
    mov eax, dword ptr [0 + eax + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByIdTransition__loadConstantOrVariablePayload__39_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1472
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1473
    jne _offlineasm_40_putByIdTransition__opPutByIdSlow
    mov ebx, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1500
    test ebx, ebx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1501
    jnz _offlineasm_putByIdTransition__additionalChecks__39_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_putByIdTransition__additionalChecks__39_assert__ok:
    mov ebx, dword ptr [8 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1502
    test ebx, ebx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1503
    jnz _offlineasm_putByIdTransition__additionalChecks__40_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_putByIdTransition__additionalChecks__40_assert__ok:
    cmp dword ptr [28 + edx], -3                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1504
    je _offlineasm_putByIdTransition__40_additionalChecks__done

  _offlineasm_putByIdTransition__40_additionalChecks__loop:
    mov edx, dword ptr [24 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1506
    mov edx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1507
    cmp edx, dword ptr [0 + ebx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1508
    jne _offlineasm_40_putByIdTransition__opPutByIdSlow
    add ebx, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1509
    cmp dword ptr [28 + edx], -3                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1510
    jne _offlineasm_putByIdTransition__40_additionalChecks__loop

  _offlineasm_putByIdTransition__40_additionalChecks__done:
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1475
    mov ebx, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1036
    add ebx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1480
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByIdTransition__getPropertyStorage__continuation__40_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByIdTransition__getPropertyStorage__continuation__40_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByIdTransition__getPropertyStorage__continuation__40_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByIdTransition__getPropertyStorage__continuation__40_loadConstantOrVariable2Reg__done:
    mov dword ptr [4 + ebx], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1482
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1483
    mov dword ptr [0 + ebx], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1484
    mov dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1485
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_40_putByIdTransition__opPutByIdSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_by_val:
    mov ecx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1532
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_loadConstantOrVariablePayload__40_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ecx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opGetByValSlow
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_loadConstantOrVariablePayload__40_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_loadConstantOrVariablePayload__40_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + ecx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opGetByValSlow
    mov eax, dword ptr [0 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_loadConstantOrVariablePayload__40_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1534
    mov ebx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1535
    mov edx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:514
    mov dword ptr [4 + ebx], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:515
    movzx ecx, byte ptr [4 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:516
    mov ebx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1537
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_loadConstantOrVariablePayload__41_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ebx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opGetByValSlow
    mov edx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_loadConstantOrVariablePayload__41_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_loadConstantOrVariablePayload__41_loadConstantOrVariablePayloadTagCustom__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + edx + ebx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_opGetByValSlow
    mov edx, dword ptr [0 + edx + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_loadConstantOrVariablePayload__41_loadConstantOrVariablePayloadTagCustom__done:
    mov ebx, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1539
    and ecx, 30                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1540
    cmp ecx, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1541
    je _offlineasm_opGetByValIsContiguous
    cmp ecx, 26                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1542
    jne _offlineasm_opGetByValNotContiguous

  _offlineasm_opGetByValIsContiguous:
    cmp edx, dword ptr [-8 + ebx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1545
    jae _offlineasm_opGetByValOutOfBounds
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1546
    mov edx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1547
    jmp _offlineasm_opGetByValDone                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1548

  _offlineasm_opGetByValNotContiguous:
    cmp ecx, 22                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1551
    jne _offlineasm_opGetByValNotDouble
    cmp edx, dword ptr [-8 + ebx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1552
    jae _offlineasm_opGetByValOutOfBounds
    fld qword ptr [0 + ebx + edx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1553
    fstp st(1)
    fucomi st(0), st(0)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1554
    jp _offlineasm_opGetByValSlow
    fst qword ptr [-8 + esp]                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1556
    mov edx, [-8 + esp]
    mov ecx, [-4 + esp]
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1557
    jmp _offlineasm_opGetByValNotEmpty                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1558

  _offlineasm_opGetByValNotDouble:
    sub ecx, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1561
    cmp ecx, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1562
    ja _offlineasm_opGetByValSlow
    cmp edx, dword ptr [-4 + ebx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1563
    jae _offlineasm_opGetByValOutOfBounds
    mov ecx, dword ptr [20 + ebx + edx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1564
    mov edx, dword ptr [16 + ebx + edx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1565

  _offlineasm_opGetByValDone:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1568
    cmp ecx, -6                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1569
    je _offlineasm_opGetByValOutOfBounds

  _offlineasm_opGetByValNotEmpty:
    mov dword ptr [4 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1571
    mov dword ptr [0 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1572
    mov eax, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 24                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opGetByValOutOfBounds:
    mov eax, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov byte ptr [9 + eax], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1578

  _offlineasm_opGetByValSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_get_by_val
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 24                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_val:
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_putByVal__writeBarrierOnOperands__41_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_putByVal__writeBarrierOnOperands__41_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_putByVal__writeBarrierOnOperands__41_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_putByVal__writeBarrierOnOperands__41_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_putByVal__42_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__42_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__writeBarrierOnOperands__42_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__42_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__42_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__writeBarrierOnOperands__42_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__42_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_putByVal__writeBarrierOnOperands__42_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_putByVal__writeBarrierOnOperands__42_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_putByVal__42_writeBarrierOnOperands__writeBarrierDone:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1603
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByVal__loadConstantOrVariablePayload__43_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__opPutByValSlow
    mov edx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByVal__loadConstantOrVariablePayload__43_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByVal__loadConstantOrVariablePayload__43_loadConstantOrVariablePayloadTagCustom__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + edx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__opPutByValSlow
    mov edx, dword ptr [0 + edx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByVal__loadConstantOrVariablePayload__43_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1605
    mov ebx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1606
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:514
    mov dword ptr [4 + ebx], eax                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:515
    movzx ecx, byte ptr [4 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:516
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1608
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByVal__loadConstantOrVariablePayload__44_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__opPutByValSlow
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByVal__loadConstantOrVariablePayload__44_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByVal__loadConstantOrVariablePayload__44_loadConstantOrVariablePayloadTagCustom__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ebx + eax * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__opPutByValSlow
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByVal__loadConstantOrVariablePayload__44_loadConstantOrVariablePayloadTagCustom__done:
    mov eax, dword ptr [8 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1610
    and ecx, 30                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1611
    cmp ecx, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1612
    jne _offlineasm_putByVal__opPutByValNotInt32
    cmp ebx, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1585
    jae _offlineasm_putByVal__contiguousPutByVal__outOfBounds

  _offlineasm_putByVal__contiguousPutByVal__storeResult:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1587
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByVal__contiguousPutByVal__storeCallback__loadConstantOrVariablePayload__45_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ecx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__opPutByValSlow
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByVal__contiguousPutByVal__storeCallback__loadConstantOrVariablePayload__45_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__loadConstantOrVariablePayload__45_loadConstantOrVariablePayloadTagCustom__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + edx + ecx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__opPutByValSlow
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__loadConstantOrVariablePayload__45_loadConstantOrVariablePayloadTagCustom__done:
    mov dword ptr [4 + eax + ebx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1616
    mov dword ptr [0 + eax + ebx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1617
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putByVal__contiguousPutByVal__outOfBounds:
    cmp ebx, dword ptr [-4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1592
    jae _offlineasm_putByVal__opPutByValOutOfBounds
    mov ecx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1593
    mov byte ptr [8 + ecx], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1594
    lea ecx, [1 + ebx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1595
    mov dword ptr [-8 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1596
    jmp _offlineasm_putByVal__contiguousPutByVal__storeResult ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1597

  _offlineasm_putByVal__opPutByValNotInt32:
    cmp ecx, 22                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1621
    jne _offlineasm_putByVal__opPutByValNotDouble
    cmp ebx, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1585
    jae _offlineasm_putByVal__45_contiguousPutByVal__outOfBounds

  _offlineasm_putByVal__45_contiguousPutByVal__storeResult:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1587
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByVal__contiguousPutByVal__storeCallback__45_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByVal__contiguousPutByVal__storeCallback__45_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__45_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__45_loadConstantOrVariable2Reg__done:
    cmp edx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1627
    jne _offlineasm_putByVal__contiguousPutByVal__storeCallback__notInt
    mov [-4 + esp], ecx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1628
    fild dword ptr [-4 + esp]
    fstp st(1)
    jmp _offlineasm_putByVal__contiguousPutByVal__storeCallback__ready ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1629

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__notInt:
    mov [-8 + esp], ecx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1631
    mov [-4 + esp], edx
    fld qword ptr [-8 + esp]
    fstp st(1)
    fucomi st(0), st(0)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1632
    jp _offlineasm_putByVal__opPutByValSlow

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__ready:
    fst qword ptr [0 + eax + ebx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1634
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putByVal__45_contiguousPutByVal__outOfBounds:
    cmp ebx, dword ptr [-4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1592
    jae _offlineasm_putByVal__opPutByValOutOfBounds
    mov ecx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1593
    mov byte ptr [8 + ecx], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1594
    lea ecx, [1 + ebx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1595
    mov dword ptr [-8 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1596
    jmp _offlineasm_putByVal__45_contiguousPutByVal__storeResult ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1597

  _offlineasm_putByVal__opPutByValNotDouble:
    cmp ecx, 26                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1638
    jne _offlineasm_putByVal__opPutByValNotContiguous
    cmp ebx, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1585
    jae _offlineasm_putByVal__46_contiguousPutByVal__outOfBounds

  _offlineasm_putByVal__46_contiguousPutByVal__storeResult:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1587
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByVal__contiguousPutByVal__storeCallback__46_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByVal__contiguousPutByVal__storeCallback__46_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__46_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__46_loadConstantOrVariable2Reg__done:
    mov dword ptr [4 + eax + ebx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1644
    mov dword ptr [0 + eax + ebx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1645
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putByVal__46_contiguousPutByVal__outOfBounds:
    cmp ebx, dword ptr [-4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1592
    jae _offlineasm_putByVal__opPutByValOutOfBounds
    mov ecx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1593
    mov byte ptr [8 + ecx], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1594
    lea ecx, [1 + ebx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1595
    mov dword ptr [-8 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1596
    jmp _offlineasm_putByVal__46_contiguousPutByVal__storeResult ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1597

  _offlineasm_putByVal__opPutByValNotContiguous:
    cmp ecx, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1649
    jne _offlineasm_putByVal__opPutByValSlow
    cmp ebx, dword ptr [-4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1650
    jae _offlineasm_putByVal__opPutByValOutOfBounds
    cmp dword ptr [20 + eax + ebx * 8], -6                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1651
    je _offlineasm_putByVal__opPutByValArrayStorageEmpty

  _offlineasm_putByVal__opPutByValArrayStorageStoreResult:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1653
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByVal__47_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByVal__47_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByVal__47_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByVal__47_loadConstantOrVariable2Reg__done:
    mov dword ptr [20 + eax + ebx * 8], edx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1655
    mov dword ptr [16 + eax + ebx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1656
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putByVal__opPutByValArrayStorageEmpty:
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1660
    mov byte ptr [8 + edx], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1661
    add dword ptr [8 + eax], 1                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1662
    cmp ebx, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1663
    jb _offlineasm_putByVal__opPutByValArrayStorageStoreResult
    lea edx, [1 + ebx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1664
    mov dword ptr [-8 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1665
    jmp _offlineasm_putByVal__opPutByValArrayStorageStoreResult ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1666

  _offlineasm_putByVal__opPutByValOutOfBounds:
    mov eax, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov byte ptr [9 + eax], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1670

  _offlineasm_putByVal__opPutByValSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_val
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_val_direct:
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_putByVal__writeBarrierOnOperands__47_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_putByVal__writeBarrierOnOperands__47_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_putByVal__writeBarrierOnOperands__47_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_putByVal__writeBarrierOnOperands__47_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_putByVal__47_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__47_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__writeBarrierOnOperands__47_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__47_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__47_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_putByVal__writeBarrierOnOperands__47_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__47_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_putByVal__writeBarrierOnOperands__47_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_putByVal__writeBarrierOnOperands__47_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_putByVal__47_writeBarrierOnOperands__writeBarrierDone:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1603
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByVal__loadConstantOrVariablePayload__48_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_52_putByVal__opPutByValSlow
    mov edx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByVal__loadConstantOrVariablePayload__48_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByVal__loadConstantOrVariablePayload__48_loadConstantOrVariablePayloadTagCustom__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + edx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_52_putByVal__opPutByValSlow
    mov edx, dword ptr [0 + edx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByVal__loadConstantOrVariablePayload__48_loadConstantOrVariablePayloadTagCustom__done:
    mov ecx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1605
    mov ebx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1606
    mov eax, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:514
    mov dword ptr [4 + ebx], eax                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:515
    movzx ecx, byte ptr [4 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:516
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1608
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByVal__loadConstantOrVariablePayload__49_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_52_putByVal__opPutByValSlow
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByVal__loadConstantOrVariablePayload__49_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByVal__loadConstantOrVariablePayload__49_loadConstantOrVariablePayloadTagCustom__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ebx + eax * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_52_putByVal__opPutByValSlow
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByVal__loadConstantOrVariablePayload__49_loadConstantOrVariablePayloadTagCustom__done:
    mov eax, dword ptr [8 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1610
    and ecx, 30                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1611
    cmp ecx, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1612
    jne _offlineasm_52_putByVal__opPutByValNotInt32
    cmp ebx, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1585
    jae _offlineasm_putByVal__50_contiguousPutByVal__outOfBounds

  _offlineasm_putByVal__50_contiguousPutByVal__storeResult:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1587
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_putByVal__contiguousPutByVal__storeCallback__loadConstantOrVariablePayload__50_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + ecx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_52_putByVal__opPutByValSlow
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_putByVal__contiguousPutByVal__storeCallback__loadConstantOrVariablePayload__50_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__loadConstantOrVariablePayload__50_loadConstantOrVariablePayloadTagCustom__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + edx + ecx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_52_putByVal__opPutByValSlow
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__loadConstantOrVariablePayload__50_loadConstantOrVariablePayloadTagCustom__done:
    mov dword ptr [4 + eax + ebx * 8], -1                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1616
    mov dword ptr [0 + eax + ebx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1617
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putByVal__50_contiguousPutByVal__outOfBounds:
    cmp ebx, dword ptr [-4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1592
    jae _offlineasm_52_putByVal__opPutByValOutOfBounds
    mov ecx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1593
    mov byte ptr [8 + ecx], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1594
    lea ecx, [1 + ebx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1595
    mov dword ptr [-8 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1596
    jmp _offlineasm_putByVal__50_contiguousPutByVal__storeResult ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1597

  _offlineasm_52_putByVal__opPutByValNotInt32:
    cmp ecx, 22                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1621
    jne _offlineasm_52_putByVal__opPutByValNotDouble
    cmp ebx, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1585
    jae _offlineasm_putByVal__51_contiguousPutByVal__outOfBounds

  _offlineasm_putByVal__51_contiguousPutByVal__storeResult:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1587
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByVal__contiguousPutByVal__storeCallback__50_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByVal__contiguousPutByVal__storeCallback__50_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__50_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__50_loadConstantOrVariable2Reg__done:
    cmp edx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1627
    jne _offlineasm_putByVal__contiguousPutByVal__50_storeCallback__notInt
    mov [-4 + esp], ecx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1628
    fild dword ptr [-4 + esp]
    fstp st(1)
    jmp _offlineasm_putByVal__contiguousPutByVal__50_storeCallback__ready ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1629

  _offlineasm_putByVal__contiguousPutByVal__50_storeCallback__notInt:
    mov [-8 + esp], ecx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1631
    mov [-4 + esp], edx
    fld qword ptr [-8 + esp]
    fstp st(1)
    fucomi st(0), st(0)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1632
    jp _offlineasm_52_putByVal__opPutByValSlow

  _offlineasm_putByVal__contiguousPutByVal__50_storeCallback__ready:
    fst qword ptr [0 + eax + ebx * 8]                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1634
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putByVal__51_contiguousPutByVal__outOfBounds:
    cmp ebx, dword ptr [-4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1592
    jae _offlineasm_52_putByVal__opPutByValOutOfBounds
    mov ecx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1593
    mov byte ptr [8 + ecx], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1594
    lea ecx, [1 + ebx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1595
    mov dword ptr [-8 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1596
    jmp _offlineasm_putByVal__51_contiguousPutByVal__storeResult ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1597

  _offlineasm_52_putByVal__opPutByValNotDouble:
    cmp ecx, 26                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1638
    jne _offlineasm_52_putByVal__opPutByValNotContiguous
    cmp ebx, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1585
    jae _offlineasm_putByVal__52_contiguousPutByVal__outOfBounds

  _offlineasm_putByVal__52_contiguousPutByVal__storeResult:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1587
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByVal__contiguousPutByVal__storeCallback__51_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByVal__contiguousPutByVal__storeCallback__51_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__51_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByVal__contiguousPutByVal__storeCallback__51_loadConstantOrVariable2Reg__done:
    mov dword ptr [4 + eax + ebx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1644
    mov dword ptr [0 + eax + ebx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1645
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_putByVal__52_contiguousPutByVal__outOfBounds:
    cmp ebx, dword ptr [-4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1592
    jae _offlineasm_52_putByVal__opPutByValOutOfBounds
    mov ecx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1593
    mov byte ptr [8 + ecx], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1594
    lea ecx, [1 + ebx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1595
    mov dword ptr [-8 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1596
    jmp _offlineasm_putByVal__52_contiguousPutByVal__storeResult ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1597

  _offlineasm_52_putByVal__opPutByValNotContiguous:
    cmp ecx, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1649
    jne _offlineasm_52_putByVal__opPutByValSlow
    cmp ebx, dword ptr [-4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1650
    jae _offlineasm_52_putByVal__opPutByValOutOfBounds
    cmp dword ptr [20 + eax + ebx * 8], -6                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1651
    je _offlineasm_52_putByVal__opPutByValArrayStorageEmpty

  _offlineasm_52_putByVal__opPutByValArrayStorageStoreResult:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1653
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_putByVal__52_loadConstantOrVariable2Reg__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ecx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_putByVal__52_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_putByVal__52_loadConstantOrVariable2Reg__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ecx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add edx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ecx, dword ptr [0 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov edx, dword ptr [4 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_putByVal__52_loadConstantOrVariable2Reg__done:
    mov dword ptr [20 + eax + ebx * 8], edx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1655
    mov dword ptr [16 + eax + ebx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1656
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_52_putByVal__opPutByValArrayStorageEmpty:
    mov edx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1660
    mov byte ptr [8 + edx], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1661
    add dword ptr [8 + eax], 1                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1662
    cmp ebx, dword ptr [-8 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1663
    jb _offlineasm_52_putByVal__opPutByValArrayStorageStoreResult
    lea edx, [1 + ebx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1664
    mov dword ptr [-8 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1665
    jmp _offlineasm_52_putByVal__opPutByValArrayStorageStoreResult ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1666

  _offlineasm_52_putByVal__opPutByValOutOfBounds:
    mov eax, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov byte ptr [9 + eax], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1670

  _offlineasm_52_putByVal__opPutByValSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_val_direct
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_jmp:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _llint_op_jeq_null:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1703
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:544
    jl _offlineasm_equalNull__assertNotConstant__52_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_equalNull__assertNotConstant__52_assert__ok:
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1705
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1706
    cmp edx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1707
    jne _offlineasm_equalNull__immediate
    mov ecx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1708
    test byte ptr [6 + eax], 1                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1725
    jz _offlineasm_equalNull__cellHandler__opJeqNullNotMasqueradesAsUndefined
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1726
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1727
    cmp dword ptr [20 + ecx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1728
    je _offlineasm_equalNull__target

  _offlineasm_equalNull__cellHandler__opJeqNullNotMasqueradesAsUndefined:
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_equalNull__target:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_equalNull__immediate:
    or edx, 1                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1716
    cmp edx, -3                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1731
    je _offlineasm_equalNull__target
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_jneq_null:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1703
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:544
    jl _offlineasm_equalNull__assertNotConstant__53_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_equalNull__assertNotConstant__53_assert__ok:
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1705
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1706
    cmp edx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1707
    jne _offlineasm_53_equalNull__immediate
    mov ecx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1708
    test byte ptr [6 + eax], 1                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1738
    jz _offlineasm_53_equalNull__target
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1739
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1740
    cmp dword ptr [20 + ecx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1741
    jne _offlineasm_53_equalNull__target
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_53_equalNull__target:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_53_equalNull__immediate:
    or edx, 1                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1716
    cmp edx, -3                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1743
    jne _offlineasm_53_equalNull__target
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_jneq_ptr:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1748
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1749
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1750
    mov ecx, dword ptr [20 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1751
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1752
    jne _offlineasm_opJneqPtrBranch
    mov edx, dword ptr [552 + ecx + edx * 4]                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1753
    cmp dword ptr [0 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1754
    je _offlineasm_opJneqPtrFallThrough

  _offlineasm_opJneqPtrBranch:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_opJneqPtrFallThrough:
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_switch_imm:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1802
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1803
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_53_loadConstantOrVariable__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_53_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_53_loadConstantOrVariable__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov edx, dword ptr [4 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov eax, dword ptr [0 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_53_loadConstantOrVariable__done:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1805
    mov ecx, dword ptr [376 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1806
    imul ebx, 32                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1807
    mov ecx, dword ptr [24 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1808
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1809
    cmp edx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1810
    jne _offlineasm_opSwitchImmNotInt
    sub eax, dword ptr [12 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1811
    cmp eax, dword ptr [8 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1812
    jae _offlineasm_opSwitchImmFallThrough
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1813
    mov edx, dword ptr [0 + ebx + eax * 4]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1814
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1815
    jz _offlineasm_opSwitchImmFallThrough
    sal edx, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_opSwitchImmNotInt:
    cmp edx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1819
    jb _offlineasm_opSwitchImmSlow

  _offlineasm_opSwitchImmFallThrough:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_opSwitchImmSlow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_switch_imm
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_switch_char:
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1830
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1831
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_54_loadConstantOrVariable__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_54_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_54_loadConstantOrVariable__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov edx, dword ptr [4 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov eax, dword ptr [0 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_54_loadConstantOrVariable__done:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1833
    mov ecx, dword ptr [376 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1834
    imul ebx, 32                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1835
    mov ecx, dword ptr [24 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1836
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1837
    cmp edx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1838
    jne _offlineasm_opSwitchCharFallThrough
    cmp byte ptr [5 + eax], 6                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1839
    jne _offlineasm_opSwitchCharFallThrough
    cmp dword ptr [12 + eax], 1                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1840
    jne _offlineasm_opSwitchCharFallThrough
    mov eax, dword ptr [16 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1841
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1842
    jz _offlineasm_opSwitchOnRope
    mov edx, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1843
    test dword ptr [12 + eax], 8                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1844
    jnz _offlineasm_opSwitchChar8Bit
    movzx eax, word ptr [0 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1845
    jmp _offlineasm_opSwitchCharReady                        ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1846

  _offlineasm_opSwitchChar8Bit:
    movzx eax, byte ptr [0 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1848

  _offlineasm_opSwitchCharReady:
    sub eax, dword ptr [12 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1850
    cmp eax, dword ptr [8 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1851
    jae _offlineasm_opSwitchCharFallThrough
    mov ecx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1852
    mov edx, dword ptr [0 + ecx + eax * 4]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1853
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1854
    jz _offlineasm_opSwitchCharFallThrough
    sal edx, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_opSwitchCharFallThrough:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_opSwitchOnRope:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_switch_char
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_ret:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:529
    add dword ptr [332 + eax], 10                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:530
    js _offlineasm_checkSwitchToJITForEpilogue__checkSwitchToJIT__continue
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_replace
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155

  _offlineasm_checkSwitchToJITForEpilogue__checkSwitchToJIT__continue:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1902
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_55_loadConstantOrVariable__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_55_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_55_loadConstantOrVariable__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov edx, dword ptr [4 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov eax, dword ptr [0 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_55_loadConstantOrVariable__done:
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:395
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:400
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:707

  _llint_op_to_primitive:
    mov ecx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1909
    mov ebx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1910
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_56_loadConstantOrVariable__constant
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_56_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_56_loadConstantOrVariable__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov edx, dword ptr [4 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov eax, dword ptr [0 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_56_loadConstantOrVariable__done:
    cmp edx, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1912
    jne _offlineasm_opToPrimitiveIsImm
    cmp byte ptr [5 + eax], 18                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1913
    jae _offlineasm_opToPrimitiveSlowCase

  _offlineasm_opToPrimitiveIsImm:
    mov dword ptr [4 + ebp + ebx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1915
    mov dword ptr [0 + ebp + ebx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1916
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_opToPrimitiveSlowCase:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_to_primitive
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_catch:
    mov ebx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1930
    and ebx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1931
    mov ebx, dword ptr [180 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1932
    mov ebp, dword ptr [9224 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1933
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    mov esi, dword ptr [9232 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1936
    mov eax, dword ptr [9388 + ebx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1937
    mov dword ptr [9388 + ebx], 0                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1938
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1939
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1940
    mov dword ptr [4 + ebp + ecx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1941
    mov edx, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1943
    mov eax, dword ptr [16 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1944
    mov ecx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1945
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1946
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1947
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_end:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:529
    add dword ptr [332 + eax], 10                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:530
    js _offlineasm_checkSwitchToJITForEpilogue__56_checkSwitchToJIT__continue
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_replace
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155

  _offlineasm_checkSwitchToJITForEpilogue__56_checkSwitchToJIT__continue:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1955
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:544
    jl _offlineasm_assertNotConstant__56_assert__ok
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:227
    ffree st(1)
    call llint_crash

  _offlineasm_assertNotConstant__56_assert__ok:
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1957
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1958
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:395
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:400
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:707

  _llint_throw_from_slow_path_trampoline:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_handle_exception
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1968
    and edx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1969
    mov edx, dword ptr [180 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1970
    jmp dword ptr [9228 + edx]                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1971

  _llint_throw_during_call_trampoline:
    pop ecx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:411
    jmp _llint_throw_from_slow_path_trampoline               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1976

  _llint_op_resolve_scope:
    mov eax, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    test  eax, eax                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2078
    jne _offlineasm_rGlobalVar
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2039
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2040
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2042
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2043
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_rGlobalVar:
    cmp eax, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2083
    jne _offlineasm_rClosureVar
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2039
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2040
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2042
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2043
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_rClosureVar:
    cmp eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2088
    jne _offlineasm_rGlobalPropertyWithVarInjectionChecks
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2054
    mov ecx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2058
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2059
    jz _offlineasm_resolveScope__resolveScopeLoopEnd

  _offlineasm_resolveScope__resolveScopeLoop:
    mov eax, dword ptr [16 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2062
    sub ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2063
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2064
    jnz _offlineasm_resolveScope__resolveScopeLoop

  _offlineasm_resolveScope__resolveScopeLoopEnd:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2068
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2069
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_rGlobalPropertyWithVarInjectionChecks:
    cmp eax, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2093
    jne _offlineasm_rGlobalVarWithVarInjectionChecks
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2047
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2048
    mov eax, dword ptr [592 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2049
    cmp byte ptr [4 + eax], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2050
    je _offlineasm_rDynamic
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2039
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2040
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2042
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2043
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_rGlobalVarWithVarInjectionChecks:
    cmp eax, 5                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2099
    jne _offlineasm_rClosureVarWithVarInjectionChecks
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2047
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2048
    mov eax, dword ptr [592 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2049
    cmp byte ptr [4 + eax], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2050
    je _offlineasm_rDynamic
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2039
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2040
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2042
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2043
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_rClosureVarWithVarInjectionChecks:
    cmp eax, 6                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2105
    jne _offlineasm_rDynamic
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2047
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2048
    mov eax, dword ptr [592 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2049
    cmp byte ptr [4 + eax], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2050
    je _offlineasm_rDynamic
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2054
    mov ecx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2058
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2059
    jz _offlineasm_56_resolveScope__resolveScopeLoopEnd

  _offlineasm_56_resolveScope__resolveScopeLoop:
    mov eax, dword ptr [16 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2062
    sub ecx, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2063
    test ecx, ecx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2064
    jnz _offlineasm_56_resolveScope__resolveScopeLoop

  _offlineasm_56_resolveScope__resolveScopeLoopEnd:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2068
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2069
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_rDynamic:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_resolve_scope
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_from_scope:
    mov eax, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    and eax, 65535                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2154
    test  eax, eax                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2157
    jne _offlineasm_gGlobalVar
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2117
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2119
    jne _offlineasm_gDynamic
    mov ebx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp ebx, 100                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1348
    jl _offlineasm_getProperty__loadPropertyAtVariableOffset__isInline
    mov eax, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1349
    neg ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1350
    jmp _offlineasm_getProperty__loadPropertyAtVariableOffset__ready ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1351

  _offlineasm_getProperty__loadPropertyAtVariableOffset__isInline:
    add eax, -768                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1353

  _offlineasm_getProperty__loadPropertyAtVariableOffset__ready:
    mov edx, dword ptr [788 + eax + ebx * 8]                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1355
    mov ecx, dword ptr [784 + eax + ebx * 8]                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1356
    mov eax, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2127
    mov dword ptr [0 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2128
    add esi, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_gGlobalVar:
    cmp eax, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2163
    jne _offlineasm_gClosureVar
    mov eax, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov edx, dword ptr [4 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2133
    mov ecx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2134
    mov eax, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2137
    mov dword ptr [0 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2138
    add esi, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_gClosureVar:
    cmp eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2168
    jne _offlineasm_gGlobalPropertyWithVarInjectionChecks
    mov ecx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:476
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:477
    mov ebx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov edx, dword ptr [28 + eax + ebx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2143
    mov ecx, dword ptr [24 + eax + ebx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2144
    mov eax, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2147
    mov dword ptr [0 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2148
    add esi, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_gGlobalPropertyWithVarInjectionChecks:
    cmp eax, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2174
    jne _offlineasm_gGlobalVarWithVarInjectionChecks
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2117
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2119
    jne _offlineasm_gDynamic
    mov ebx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp ebx, 100                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1348
    jl _offlineasm_getProperty__56_loadPropertyAtVariableOffset__isInline
    mov eax, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1349
    neg ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1350
    jmp _offlineasm_getProperty__56_loadPropertyAtVariableOffset__ready ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1351

  _offlineasm_getProperty__56_loadPropertyAtVariableOffset__isInline:
    add eax, -768                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1353

  _offlineasm_getProperty__56_loadPropertyAtVariableOffset__ready:
    mov edx, dword ptr [788 + eax + ebx * 8]                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1355
    mov ecx, dword ptr [784 + eax + ebx * 8]                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1356
    mov eax, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2127
    mov dword ptr [0 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2128
    add esi, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_gGlobalVarWithVarInjectionChecks:
    cmp eax, 5                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2180
    jne _offlineasm_gClosureVarWithVarInjectionChecks
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2047
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2048
    mov eax, dword ptr [592 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2049
    cmp byte ptr [4 + eax], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2050
    je _offlineasm_gDynamic
    mov eax, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov edx, dword ptr [4 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2133
    mov ecx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2134
    mov eax, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2137
    mov dword ptr [0 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2138
    add esi, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_gClosureVarWithVarInjectionChecks:
    cmp eax, 6                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2186
    jne _offlineasm_gDynamic
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2047
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2048
    mov eax, dword ptr [592 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2049
    cmp byte ptr [4 + eax], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2050
    je _offlineasm_gDynamic
    mov ecx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:476
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:477
    mov ebx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov edx, dword ptr [28 + eax + ebx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2143
    mov ecx, dword ptr [24 + eax + ebx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2144
    mov eax, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + eax], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + eax * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2147
    mov dword ptr [0 + ebp + eax * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2148
    add esi, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_gDynamic:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_get_from_scope
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 32                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_to_scope:
    mov eax, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    and eax, 65535                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2238
    cmp eax, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2241
    jne _offlineasm_pGlobalProperty
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_writeBarrierOnOperands__56_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_writeBarrierOnOperands__56_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_writeBarrierOnOperands__56_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_writeBarrierOnOperands__56_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_56_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__56_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__56_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__56_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__56_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__56_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__56_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_writeBarrierOnOperands__56_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_writeBarrierOnOperands__56_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_56_writeBarrierOnOperands__writeBarrierDone:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:476
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:477
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_putLocalClosureVar__57_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_putLocalClosureVar__57_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_putLocalClosureVar__57_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_putLocalClosureVar__57_loadConstantOrVariable__done:
    mov edi, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    test edi, edi                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2226
    jz _offlineasm_putLocalClosureVar__noVariableWatchpointSet
    cmp byte ptr [4 + edi], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:525
    jne _offlineasm_pDynamic

  _offlineasm_putLocalClosureVar__noVariableWatchpointSet:
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [28 + eax + edx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2230
    mov dword ptr [24 + eax + edx * 8], ebx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2231
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_pGlobalProperty:
    test  eax, eax                                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2248
    jne _offlineasm_pGlobalVar
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_writeBarrierOnOperands__57_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_writeBarrierOnOperands__57_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_writeBarrierOnOperands__57_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_writeBarrierOnOperands__57_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_57_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__57_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__57_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__57_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__57_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__57_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__57_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_writeBarrierOnOperands__57_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_writeBarrierOnOperands__57_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_57_writeBarrierOnOperands__writeBarrierDone:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2117
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2119
    jne _offlineasm_pDynamic
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_putProperty__58_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_putProperty__58_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_putProperty__58_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_putProperty__58_loadConstantOrVariable__done:
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 100                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1360
    jl _offlineasm_putProperty__storePropertyAtVariableOffset__isInline
    mov eax, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1361
    neg edx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1362
    jmp _offlineasm_putProperty__storePropertyAtVariableOffset__ready ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1363

  _offlineasm_putProperty__storePropertyAtVariableOffset__isInline:
    add eax, -768                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1365

  _offlineasm_putProperty__storePropertyAtVariableOffset__ready:
    mov dword ptr [788 + eax + edx * 8], ecx                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1367
    mov dword ptr [784 + eax + edx * 8], ebx                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1368
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_pGlobalVar:
    cmp eax, 1                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2255
    jne _offlineasm_pClosureVar
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_writeBarrierOnGlobalObject__58_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_writeBarrierOnGlobalObject__58_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_writeBarrierOnGlobalObject__58_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_writeBarrierOnGlobalObject__58_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:603
    jne _offlineasm_writeBarrierOnGlobalObject__writeBarrierDone
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:605
    mov ebx, dword ptr [20 + ebx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:606
    movzx edx, byte ptr [7 + ebx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:609
    jnz _offlineasm_writeBarrierOnGlobalObject__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:610
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:612
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ebx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:614
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:615
    pop ebp

  _offlineasm_writeBarrierOnGlobalObject__writeBarrierDone:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_putGlobalVar__59_loadConstantOrVariable__constant
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ecx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_putGlobalVar__59_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_putGlobalVar__59_loadConstantOrVariable__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov edx, dword ptr [4 + ecx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ecx, dword ptr [0 + ecx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_putGlobalVar__59_loadConstantOrVariable__done:
    mov ebx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    cmp byte ptr [4 + ebx], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:525
    jne _offlineasm_pDynamic
    mov eax, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov dword ptr [4 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2210
    mov dword ptr [0 + eax], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2211
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_pClosureVar:
    cmp eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2261
    jne _offlineasm_pGlobalPropertyWithVarInjectionChecks
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_writeBarrierOnOperands__59_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_writeBarrierOnOperands__59_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_writeBarrierOnOperands__59_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_writeBarrierOnOperands__59_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_59_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__59_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__59_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__59_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__59_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__59_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__59_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_writeBarrierOnOperands__59_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_writeBarrierOnOperands__59_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_59_writeBarrierOnOperands__writeBarrierDone:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:476
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:477
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_putClosureVar__60_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_putClosureVar__60_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_putClosureVar__60_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_putClosureVar__60_loadConstantOrVariable__done:
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [28 + eax + edx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2218
    mov dword ptr [24 + eax + edx * 8], ebx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2219
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_pGlobalPropertyWithVarInjectionChecks:
    cmp eax, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2268
    jne _offlineasm_pGlobalVarWithVarInjectionChecks
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_writeBarrierOnOperands__60_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_writeBarrierOnOperands__60_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_writeBarrierOnOperands__60_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_writeBarrierOnOperands__60_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_60_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__60_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__60_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__60_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__60_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__60_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__60_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_writeBarrierOnOperands__60_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_writeBarrierOnOperands__60_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_60_writeBarrierOnOperands__writeBarrierDone:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2117
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    cmp dword ptr [0 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2119
    jne _offlineasm_pDynamic
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_putProperty__61_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_putProperty__61_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_putProperty__61_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_putProperty__61_loadConstantOrVariable__done:
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 100                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1360
    jl _offlineasm_putProperty__61_storePropertyAtVariableOffset__isInline
    mov eax, dword ptr [8 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1361
    neg edx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1362
    jmp _offlineasm_putProperty__61_storePropertyAtVariableOffset__ready ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1363

  _offlineasm_putProperty__61_storePropertyAtVariableOffset__isInline:
    add eax, -768                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1365

  _offlineasm_putProperty__61_storePropertyAtVariableOffset__ready:
    mov dword ptr [788 + eax + edx * 8], ecx                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1367
    mov dword ptr [784 + eax + edx * 8], ebx                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1368
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_pGlobalVarWithVarInjectionChecks:
    cmp eax, 5                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2275
    jne _offlineasm_pClosureVarWithVarInjectionChecks
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_writeBarrierOnGlobalObject__61_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_writeBarrierOnGlobalObject__61_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_writeBarrierOnGlobalObject__61_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_writeBarrierOnGlobalObject__61_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:603
    jne _offlineasm_61_writeBarrierOnGlobalObject__writeBarrierDone
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:605
    mov ebx, dword ptr [20 + ebx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:606
    movzx edx, byte ptr [7 + ebx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:609
    jnz _offlineasm_61_writeBarrierOnGlobalObject__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:610
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:612
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ebx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:614
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:615
    pop ebp

  _offlineasm_61_writeBarrierOnGlobalObject__writeBarrierDone:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2047
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2048
    mov eax, dword ptr [592 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2049
    cmp byte ptr [4 + eax], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2050
    je _offlineasm_pDynamic
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_putGlobalVar__62_loadConstantOrVariable__constant
    mov edx, dword ptr [4 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ecx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_putGlobalVar__62_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_putGlobalVar__62_loadConstantOrVariable__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov edx, dword ptr [4 + ecx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ecx, dword ptr [0 + ecx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_putGlobalVar__62_loadConstantOrVariable__done:
    mov ebx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    cmp byte ptr [4 + ebx], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:525
    jne _offlineasm_pDynamic
    mov eax, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov dword ptr [4 + eax], edx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2210
    mov dword ptr [0 + eax], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2211
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_pClosureVarWithVarInjectionChecks:
    cmp eax, 6                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2282
    jne _offlineasm_pDynamic
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_writeBarrierOnOperands__62_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_writeBarrierOnOperands__62_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_writeBarrierOnOperands__62_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_writeBarrierOnOperands__62_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_62_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__62_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__62_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__62_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__62_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__62_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__62_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_writeBarrierOnOperands__62_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_writeBarrierOnOperands__62_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_62_writeBarrierOnOperands__writeBarrierDone:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2047
    mov eax, dword ptr [20 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2048
    mov eax, dword ptr [592 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2049
    cmp byte ptr [4 + eax], 2                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2050
    je _offlineasm_pDynamic
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov edx, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:476
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:477
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_putClosureVar__63_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_putClosureVar__63_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_putClosureVar__63_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_putClosureVar__63_loadConstantOrVariable__done:
    mov edx, dword ptr [24 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [28 + eax + edx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2218
    mov dword ptr [24 + eax + edx * 8], ebx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2219
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_pDynamic:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_to_scope
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_from_arguments:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2297
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2298
    mov ecx, dword ptr [36 + eax + edx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2299
    mov ebx, dword ptr [32 + eax + edx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2300
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + eax], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + eax], ebx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    mov dword ptr [4 + ebp + edx * 8], ecx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2303
    mov dword ptr [0 + ebp + edx * 8], ebx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2304
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_to_arguments:
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:498
    jge _offlineasm_writeBarrierOnOperands__63_loadConstantOrVariableTag__constant
    mov eax, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:499
    jmp _offlineasm_writeBarrierOnOperands__63_loadConstantOrVariableTag__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:500

  _offlineasm_writeBarrierOnOperands__63_loadConstantOrVariableTag__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:502
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:503
    mov eax, dword ptr [4 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:506

  _offlineasm_writeBarrierOnOperands__63_loadConstantOrVariableTag__done:
    cmp eax, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:592
    jne _offlineasm_63_writeBarrierOnOperands__writeBarrierDone
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__63_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__63_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__63_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__63_loadConstantOrVariablePayloadTagCustom__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ecx + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_writeBarrierOnOperands__63_writeBarrierOnOperand__writeBarrierDone
    mov ecx, dword ptr [0 + ecx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariablePayload__63_loadConstantOrVariablePayloadTagCustom__done:
    movzx edx, byte ptr [7 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    test dl, dl                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:575
    jnz _offlineasm_writeBarrierOnOperands__63_writeBarrierOnOperand__writeBarrierDone
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:576
    push esi
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:578
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push ecx                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_write_barrier_slow
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:580
    pop esi                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:581
    pop ebp

  _offlineasm_writeBarrierOnOperands__63_writeBarrierOnOperand__writeBarrierDone:

  _offlineasm_63_writeBarrierOnOperands__writeBarrierDone:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2312
    mov edx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_64_loadConstantOrVariable__constant
    mov ecx, dword ptr [4 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov ebx, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_64_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_64_loadConstantOrVariable__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov ecx, dword ptr [4 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov ebx, dword ptr [0 + ebx + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_64_loadConstantOrVariable__done:
    mov edx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2315
    mov dword ptr [36 + eax + edx * 8], ecx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2316
    mov dword ptr [32 + eax + edx * 8], ebx                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2317
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_parent_scope:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov eax, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2324
    mov eax, dword ptr [16 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2325
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    mov dword ptr [4 + ebp + edx * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2327
    mov dword ptr [0 + ebp + edx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2328
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_profile_type:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2334
    mov edx, dword ptr [56 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2335
    mov edx, dword ptr [9436 + edx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2337
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:135
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_65_loadConstantOrVariable__constant
    mov edi, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov eax, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_65_loadConstantOrVariable__done          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_65_loadConstantOrVariable__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov edi, dword ptr [4 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov eax, dword ptr [0 + eax + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_65_loadConstantOrVariable__done:
    cmp edi, -6                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2343
    je _offlineasm_opProfileTypeDone
    mov ecx, dword ptr [8 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2346
    mov dword ptr [4 + ecx], edi                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2349
    mov dword ptr [0 + ecx], eax                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2350
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov dword ptr [8 + ecx], ebx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2354
    cmp edi, -5                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2356
    je _offlineasm_opProfileTypeIsCell
    mov dword ptr [12 + ecx], 0                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2357
    jmp _offlineasm_opProfileTypeSkipIsCell                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2358

  _offlineasm_opProfileTypeIsCell:
    mov ebx, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2360
    mov dword ptr [12 + ecx], ebx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2361

  _offlineasm_opProfileTypeSkipIsCell:
    add ecx, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2365
    mov dword ptr [8 + edx], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2366
    mov edx, dword ptr [12 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2368
    cmp ecx, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2369
    jne _offlineasm_opProfileTypeDone
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_profile_type_clear_log
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155

  _offlineasm_opProfileTypeDone:
    add esi, 24                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_create_direct_arguments:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_create_direct_arguments
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_create_scoped_arguments:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_create_scoped_arguments
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_create_out_of_band_arguments:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_create_out_of_band_arguments
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_new_func:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_new_func
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_new_array:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_new_array
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_new_array_with_size:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_new_array_with_size
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_new_array_buffer:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_new_array_buffer
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_new_regexp:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_new_regexp
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_less:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_less
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_lesseq:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_lesseq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_greater:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_greater
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_greatereq:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_greatereq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_mod:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_mod
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_typeof:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_typeof
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_is_object_or_null:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_is_object_or_null
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_is_function:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_is_function
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_in:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_in
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_del_by_id:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_del_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_del_by_val:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_del_by_val
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_by_index:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_by_index
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_getter_by_id:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_getter_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_setter_by_id:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_setter_by_id
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_put_getter_setter:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_put_getter_setter
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 24                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_jtrue:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1688
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_jumpTrueOrFalse__loadConstantOrVariablePayload__65_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_jumpTrueOrFalse__slow
    mov eax, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_jumpTrueOrFalse__loadConstantOrVariablePayload__65_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_jumpTrueOrFalse__loadConstantOrVariablePayload__65_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + edx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_jumpTrueOrFalse__slow
    mov eax, dword ptr [0 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_jumpTrueOrFalse__loadConstantOrVariablePayload__65_loadConstantOrVariablePayloadTagCustom__done:
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1083
    jnz _offlineasm_jumpTrueOrFalse__target
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_jumpTrueOrFalse__target:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_jumpTrueOrFalse__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jtrue
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jfalse:
    mov edx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1688
    cmp edx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_jumpTrueOrFalse__loadConstantOrVariablePayload__66_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + edx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_66_jumpTrueOrFalse__slow
    mov eax, dword ptr [0 + ebp + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_jumpTrueOrFalse__loadConstantOrVariablePayload__66_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_jumpTrueOrFalse__loadConstantOrVariablePayload__66_loadConstantOrVariablePayloadTagCustom__constant:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov eax, dword ptr [280 + eax]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + eax + edx * 8], -2                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_66_jumpTrueOrFalse__slow
    mov eax, dword ptr [0 + eax + edx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_jumpTrueOrFalse__loadConstantOrVariablePayload__66_loadConstantOrVariablePayloadTagCustom__done:
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1090
    jz _offlineasm_66_jumpTrueOrFalse__target
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_66_jumpTrueOrFalse__target:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_66_jumpTrueOrFalse__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jfalse
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jless:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1762
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1763
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_compare__66_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_compare__66_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_compare__66_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_compare__66_loadConstantOrVariable__done:
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_compare__66_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ebx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_compare__66_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_compare__66_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_compare__66_loadConstantOrVariable2Reg__done:
    cmp eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1766
    jne _offlineasm_compare__op1NotInt
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1767
    jne _offlineasm_compare__op2NotInt
    cmp edx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1097
    jl _offlineasm_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_compare__op1NotInt:
    cmp eax, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1772
    ja _offlineasm_compare__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1773
    jb _offlineasm_compare__op1NotIntOp2Double
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1774
    jne _offlineasm_compare__slow
    mov [-4 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1775
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_compare__op1NotIntReady                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1776

  _offlineasm_compare__op1NotIntOp2Double:
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1778
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_compare__op1NotIntReady:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1780
    mov [-4 + esp], eax
    fld qword ptr [-8 + esp]
    fstp st(1)
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1098
    fucomip st(0), st(1)
    ja _offlineasm_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_compare__op2NotInt:
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1785
    fild dword ptr [-4 + esp]
    fstp st(1)
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1786
    ja _offlineasm_compare__slow
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1787
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1098
    fucomip st(0), st(1)
    ja _offlineasm_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_compare__jumpTarget:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_compare__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jless
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jnless:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1762
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1763
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_compare__67_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_compare__67_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_compare__67_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_compare__67_loadConstantOrVariable__done:
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_compare__67_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ebx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_compare__67_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_compare__67_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_compare__67_loadConstantOrVariable2Reg__done:
    cmp eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1766
    jne _offlineasm_67_compare__op1NotInt
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1767
    jne _offlineasm_67_compare__op2NotInt
    cmp edx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1105
    jge _offlineasm_67_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_67_compare__op1NotInt:
    cmp eax, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1772
    ja _offlineasm_67_compare__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1773
    jb _offlineasm_67_compare__op1NotIntOp2Double
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1774
    jne _offlineasm_67_compare__slow
    mov [-4 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1775
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_67_compare__op1NotIntReady               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1776

  _offlineasm_67_compare__op1NotIntOp2Double:
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1778
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_67_compare__op1NotIntReady:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1780
    mov [-4 + esp], eax
    fld qword ptr [-8 + esp]
    fstp st(1)
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1106
    fucomip st(0), st(1)
    jbe _offlineasm_67_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_67_compare__op2NotInt:
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1785
    fild dword ptr [-4 + esp]
    fstp st(1)
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1786
    ja _offlineasm_67_compare__slow
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1787
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1106
    fucomip st(0), st(1)
    jbe _offlineasm_67_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_67_compare__jumpTarget:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_67_compare__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jnless
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jgreater:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1762
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1763
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_compare__68_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_compare__68_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_compare__68_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_compare__68_loadConstantOrVariable__done:
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_compare__68_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ebx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_compare__68_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_compare__68_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_compare__68_loadConstantOrVariable2Reg__done:
    cmp eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1766
    jne _offlineasm_68_compare__op1NotInt
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1767
    jne _offlineasm_68_compare__op2NotInt
    cmp edx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1113
    jg _offlineasm_68_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_68_compare__op1NotInt:
    cmp eax, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1772
    ja _offlineasm_68_compare__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1773
    jb _offlineasm_68_compare__op1NotIntOp2Double
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1774
    jne _offlineasm_68_compare__slow
    mov [-4 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1775
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_68_compare__op1NotIntReady               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1776

  _offlineasm_68_compare__op1NotIntOp2Double:
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1778
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_68_compare__op1NotIntReady:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1780
    mov [-4 + esp], eax
    fld qword ptr [-8 + esp]
    fstp st(1)
    fucomi st(0), st(1)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1114
    ja _offlineasm_68_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_68_compare__op2NotInt:
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1785
    fild dword ptr [-4 + esp]
    fstp st(1)
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1786
    ja _offlineasm_68_compare__slow
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1787
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fucomi st(0), st(1)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1114
    ja _offlineasm_68_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_68_compare__jumpTarget:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_68_compare__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jgreater
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jngreater:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1762
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1763
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_compare__69_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_compare__69_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_compare__69_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_compare__69_loadConstantOrVariable__done:
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_compare__69_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ebx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_compare__69_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_compare__69_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_compare__69_loadConstantOrVariable2Reg__done:
    cmp eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1766
    jne _offlineasm_69_compare__op1NotInt
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1767
    jne _offlineasm_69_compare__op2NotInt
    cmp edx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1121
    jle _offlineasm_69_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_69_compare__op1NotInt:
    cmp eax, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1772
    ja _offlineasm_69_compare__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1773
    jb _offlineasm_69_compare__op1NotIntOp2Double
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1774
    jne _offlineasm_69_compare__slow
    mov [-4 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1775
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_69_compare__op1NotIntReady               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1776

  _offlineasm_69_compare__op1NotIntOp2Double:
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1778
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_69_compare__op1NotIntReady:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1780
    mov [-4 + esp], eax
    fld qword ptr [-8 + esp]
    fstp st(1)
    fucomi st(0), st(1)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1122
    jbe _offlineasm_69_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_69_compare__op2NotInt:
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1785
    fild dword ptr [-4 + esp]
    fstp st(1)
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1786
    ja _offlineasm_69_compare__slow
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1787
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fucomi st(0), st(1)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1122
    jbe _offlineasm_69_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_69_compare__jumpTarget:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_69_compare__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jngreater
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jlesseq:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1762
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1763
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_compare__70_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_compare__70_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_compare__70_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_compare__70_loadConstantOrVariable__done:
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_compare__70_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ebx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_compare__70_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_compare__70_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_compare__70_loadConstantOrVariable2Reg__done:
    cmp eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1766
    jne _offlineasm_70_compare__op1NotInt
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1767
    jne _offlineasm_70_compare__op2NotInt
    cmp edx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1129
    jle _offlineasm_70_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_70_compare__op1NotInt:
    cmp eax, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1772
    ja _offlineasm_70_compare__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1773
    jb _offlineasm_70_compare__op1NotIntOp2Double
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1774
    jne _offlineasm_70_compare__slow
    mov [-4 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1775
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_70_compare__op1NotIntReady               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1776

  _offlineasm_70_compare__op1NotIntOp2Double:
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1778
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_70_compare__op1NotIntReady:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1780
    mov [-4 + esp], eax
    fld qword ptr [-8 + esp]
    fstp st(1)
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1130
    fucomip st(0), st(1)
    jae _offlineasm_70_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_70_compare__op2NotInt:
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1785
    fild dword ptr [-4 + esp]
    fstp st(1)
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1786
    ja _offlineasm_70_compare__slow
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1787
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1130
    fucomip st(0), st(1)
    jae _offlineasm_70_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_70_compare__jumpTarget:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_70_compare__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jlesseq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jnlesseq:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1762
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1763
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_compare__71_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_compare__71_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_compare__71_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_compare__71_loadConstantOrVariable__done:
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_compare__71_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ebx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_compare__71_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_compare__71_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_compare__71_loadConstantOrVariable2Reg__done:
    cmp eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1766
    jne _offlineasm_71_compare__op1NotInt
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1767
    jne _offlineasm_71_compare__op2NotInt
    cmp edx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1137
    jg _offlineasm_71_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_71_compare__op1NotInt:
    cmp eax, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1772
    ja _offlineasm_71_compare__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1773
    jb _offlineasm_71_compare__op1NotIntOp2Double
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1774
    jne _offlineasm_71_compare__slow
    mov [-4 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1775
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_71_compare__op1NotIntReady               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1776

  _offlineasm_71_compare__op1NotIntOp2Double:
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1778
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_71_compare__op1NotIntReady:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1780
    mov [-4 + esp], eax
    fld qword ptr [-8 + esp]
    fstp st(1)
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1138
    fucomip st(0), st(1)
    jb _offlineasm_71_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_71_compare__op2NotInt:
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1785
    fild dword ptr [-4 + esp]
    fstp st(1)
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1786
    ja _offlineasm_71_compare__slow
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1787
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fld st(1)                                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1138
    fucomip st(0), st(1)
    jb _offlineasm_71_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_71_compare__jumpTarget:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_71_compare__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jnlesseq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jgreatereq:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1762
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1763
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_compare__72_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_compare__72_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_compare__72_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_compare__72_loadConstantOrVariable__done:
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_compare__72_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ebx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_compare__72_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_compare__72_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_compare__72_loadConstantOrVariable2Reg__done:
    cmp eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1766
    jne _offlineasm_72_compare__op1NotInt
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1767
    jne _offlineasm_72_compare__op2NotInt
    cmp edx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1145
    jge _offlineasm_72_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_72_compare__op1NotInt:
    cmp eax, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1772
    ja _offlineasm_72_compare__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1773
    jb _offlineasm_72_compare__op1NotIntOp2Double
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1774
    jne _offlineasm_72_compare__slow
    mov [-4 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1775
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_72_compare__op1NotIntReady               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1776

  _offlineasm_72_compare__op1NotIntOp2Double:
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1778
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_72_compare__op1NotIntReady:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1780
    mov [-4 + esp], eax
    fld qword ptr [-8 + esp]
    fstp st(1)
    fucomi st(0), st(1)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1146
    jae _offlineasm_72_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_72_compare__op2NotInt:
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1785
    fild dword ptr [-4 + esp]
    fstp st(1)
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1786
    ja _offlineasm_72_compare__slow
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1787
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fucomi st(0), st(1)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1146
    jae _offlineasm_72_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_72_compare__jumpTarget:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_72_compare__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jgreatereq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_jngreatereq:
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1762
    mov ebx, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1763
    cmp ecx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:483
    jge _offlineasm_compare__73_loadConstantOrVariable__constant
    mov eax, dword ptr [4 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:484
    mov edx, dword ptr [0 + ebp + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:485
    jmp _offlineasm_compare__73_loadConstantOrVariable__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:486

  _offlineasm_compare__73_loadConstantOrVariable__constant:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:488
    mov edx, dword ptr [280 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:489
    mov eax, dword ptr [4 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:492
    mov edx, dword ptr [0 + edx + ecx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:493

  _offlineasm_compare__73_loadConstantOrVariable__done:
    cmp ebx, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:512
    jge _offlineasm_compare__73_loadConstantOrVariable2Reg__constant
    mov ecx, dword ptr [4 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:513
    mov ebx, dword ptr [0 + ebp + ebx * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:514
    jmp _offlineasm_compare__73_loadConstantOrVariable2Reg__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:515

  _offlineasm_compare__73_loadConstantOrVariable2Reg__constant:
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:517
    mov ecx, dword ptr [280 + ecx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:518
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:521
    add ecx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:522
    mov ebx, dword ptr [0 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:523
    mov ecx, dword ptr [4 + ecx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:524

  _offlineasm_compare__73_loadConstantOrVariable2Reg__done:
    cmp eax, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1766
    jne _offlineasm_73_compare__op1NotInt
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1767
    jne _offlineasm_73_compare__op2NotInt
    cmp edx, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1153
    jl _offlineasm_73_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_73_compare__op1NotInt:
    cmp eax, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1772
    ja _offlineasm_73_compare__slow
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1773
    jb _offlineasm_73_compare__op1NotIntOp2Double
    cmp ecx, -1                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1774
    jne _offlineasm_73_compare__slow
    mov [-4 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1775
    fild dword ptr [-4 + esp]
    fstp st(2)
    jmp _offlineasm_73_compare__op1NotIntReady               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1776

  _offlineasm_73_compare__op1NotIntOp2Double:
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1778
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)

  _offlineasm_73_compare__op1NotIntReady:
    mov [-8 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1780
    mov [-4 + esp], eax
    fld qword ptr [-8 + esp]
    fstp st(1)
    fucomi st(0), st(1)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1154
    jb _offlineasm_73_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_73_compare__op2NotInt:
    mov [-4 + esp], edx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1785
    fild dword ptr [-4 + esp]
    fstp st(1)
    cmp ecx, -7                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1786
    ja _offlineasm_73_compare__slow
    mov [-8 + esp], ebx                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1787
    mov [-4 + esp], ecx
    fld qword ptr [-8 + esp]
    fstp st(2)
    fucomi st(0), st(1)                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1154
    jb _offlineasm_73_compare__jumpTarget
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_73_compare__jumpTarget:
    mov eax, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:86
    sal eax, 2                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:80
    add esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:81
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:82

  _offlineasm_73_compare__slow:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_jngreatereq
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_loop_hint:
    mov edx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1160
    mov edx, dword ptr [56 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1161
    mov eax, dword ptr [5228 + edx]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1162
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1163
    jnz _offlineasm_handleWatchdogTimer

  _offlineasm_afterWatchdogTimerCheck:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:529
    add dword ptr [332 + eax], 1                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:530
    js _offlineasm_checkSwitchToJITForLoop__73_checkSwitchToJIT__continue
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:464
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_loop_osr
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:466
    jz _offlineasm_checkSwitchToJITForLoop__checkSwitchToJIT__action__recover
    mov esp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:467
    jmp eax                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:468

  _offlineasm_checkSwitchToJITForLoop__checkSwitchToJIT__action__recover:
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:470

  _offlineasm_checkSwitchToJITForLoop__73_checkSwitchToJIT__continue:
    add esi, 4                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_handleWatchdogTimer:
    movzx eax, byte ptr [4 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1168
    test al, al                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1169
    jz _offlineasm_afterWatchdogTimerCheck
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:454
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_handle_watchdog_timer
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:456
    jnz _offlineasm_throwHandler
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:457
    jmp _offlineasm_afterWatchdogTimerCheck                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1171

  _offlineasm_throwHandler:
    jmp _llint_throw_from_slow_path_trampoline               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1173

  _llint_op_switch_string:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_switch_string
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
    jmp dword ptr [0 + esi]

  _llint_op_new_func_exp:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_new_func_exp
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_call:
    mov ebx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1866
    neg ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1867
    cmp dword ptr [36 + ebp + ebx * 8], -5                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1868
    jne _offlineasm_arrayProfileForCall__done
    mov eax, dword ptr [32 + ebp + ebx * 8]                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1869
    mov eax, dword ptr [0 + eax]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1870
    mov edx, dword ptr [28 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov dword ptr [4 + edx], eax                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1872

  _offlineasm_arrayProfileForCall__done:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1877
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1878
    mov ecx, dword ptr [8 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1879
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_doCall__loadConstantOrVariablePayload__73_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_doCall__opCallSlow
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_doCall__loadConstantOrVariablePayload__73_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_doCall__loadConstantOrVariablePayload__73_loadConstantOrVariablePayloadTagCustom__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ebx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_doCall__opCallSlow
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_doCall__loadConstantOrVariablePayload__73_loadConstantOrVariablePayloadTagCustom__done:
    cmp ebx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1881
    jne _offlineasm_doCall__opCallSlow
    mov ebx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1882
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1883
    neg ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1884
    add ebx, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1885
    mov dword ptr [16 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1886
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1887
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1888
    mov dword ptr [24 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1889
    mov dword ptr [20 + ebx], -5                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1890
    add ebx, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1891
    mov esp, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:479
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:483
    ffree st(1)
    call dword ptr [16 + edx]
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:91
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:92
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:93
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:94
    mov ebx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ebx], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ebx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_doCall__opCallSlow:
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:448
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_call
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
    jz _offlineasm_doCall__slowPathForCall__callCallSlowPath__action__dontUpdateSP
    lea esp, [8 + edx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  _offlineasm_doCall__slowPathForCall__callCallSlowPath__action__dontUpdateSP:
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:504
    ffree st(1)
    call eax
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:91
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:92
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:93
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:94
    mov ebx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ebx], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ebx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_construct:
    mov eax, dword ptr [8 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1877
    mov edx, dword ptr [20 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1878
    mov ecx, dword ptr [8 + edx]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1879
    cmp eax, 1073741824                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:529
    jge _offlineasm_doCall__loadConstantOrVariablePayload__74_loadConstantOrVariablePayloadTagCustom__constant
    cmp dword ptr [4 + ebp + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_74_doCall__opCallSlow
    mov ebx, dword ptr [0 + ebp + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:531
    jmp _offlineasm_doCall__loadConstantOrVariablePayload__74_loadConstantOrVariablePayloadTagCustom__done ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:532

  _offlineasm_doCall__loadConstantOrVariablePayload__74_loadConstantOrVariablePayloadTagCustom__constant:
    mov ebx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:534
    mov ebx, dword ptr [280 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:535
    cmp dword ptr [4 + ebx + eax * 8], -5                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:551
    jne _offlineasm_74_doCall__opCallSlow
    mov ebx, dword ptr [0 + ebx + eax * 8]                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:539

  _offlineasm_doCall__loadConstantOrVariablePayload__74_loadConstantOrVariablePayloadTagCustom__done:
    cmp ebx, ecx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1881
    jne _offlineasm_74_doCall__opCallSlow
    mov ebx, dword ptr [16 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1882
    sal ebx, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1883
    neg ebx                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1884
    add ebx, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1885
    mov dword ptr [16 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1886
    mov ecx, dword ptr [12 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1887
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1888
    mov dword ptr [24 + ebx], ecx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1889
    mov dword ptr [20 + ebx], -5                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1890
    add ebx, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1891
    mov esp, ebx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:479
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:483
    ffree st(1)
    call dword ptr [16 + edx]
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:91
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:92
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:93
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:94
    mov ebx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ebx], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ebx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _offlineasm_74_doCall__opCallSlow:
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:448
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_construct
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
    jz _offlineasm_doCall__slowPathForCall__callCallSlowPath__74_action__dontUpdateSP
    lea esp, [8 + edx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  _offlineasm_doCall__slowPathForCall__callCallSlowPath__74_action__dontUpdateSP:
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:504
    ffree st(1)
    call eax
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:91
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:92
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:93
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:94
    mov ebx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ebx], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ebx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_call_varargs:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_size_frame_for_varargs
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    mov ebx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:693
    and ebx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:694
    mov ebx, dword ptr [180 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:695
    cmp dword ptr [9388 + ebx], 0                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:696
    jz _offlineasm_branchIfException__noException
    jmp _llint_throw_from_slow_path_trampoline               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:697

  _offlineasm_branchIfException__noException:
    mov esp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1211
    sub esp, 8
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:448
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_call_varargs
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
    jz _offlineasm_slowPathForCall__callCallSlowPath__75_action__dontUpdateSP
    lea esp, [8 + edx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  _offlineasm_slowPathForCall__callCallSlowPath__75_action__dontUpdateSP:
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:504
    ffree st(1)
    call eax
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:91
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:92
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:93
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:94
    mov ebx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ebx], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ebx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_construct_varargs:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_size_frame_for_varargs
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    mov ebx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:693
    and ebx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:694
    mov ebx, dword ptr [180 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:695
    cmp dword ptr [9388 + ebx], 0                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:696
    jz _offlineasm_75_branchIfException__noException
    jmp _llint_throw_from_slow_path_trampoline               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:697

  _offlineasm_75_branchIfException__noException:
    mov esp, edx                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1229
    sub esp, 8
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:448
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_construct_varargs
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
    jz _offlineasm_slowPathForCall__callCallSlowPath__76_action__dontUpdateSP
    lea esp, [8 + edx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  _offlineasm_slowPathForCall__callCallSlowPath__76_action__dontUpdateSP:
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:504
    ffree st(1)
    call eax
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:91
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:92
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:93
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:94
    mov ebx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ebx], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ebx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_call_eval:
    mov dword ptr [28 + ebp], esi                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:448
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_call_eval
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    test edx, edx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
    jz _offlineasm_slowPathForCall__callCallSlowPath__77_action__dontUpdateSP
    lea esp, [8 + edx]                                       ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  _offlineasm_slowPathForCall__callCallSlowPath__77_action__dontUpdateSP:
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:504
    ffree st(1)
    call eax
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:91
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:92
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:93
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:94
    mov ebx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ebx], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ebx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_generic_return_point:
    mov esi, dword ptr [28 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:91
    mov ecx, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:92
    mov dword ptr [4 + ebp + ecx * 8], edx                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:93
    mov dword ptr [0 + ebp + ecx * 8], eax                   ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:94
    mov ebx, dword ptr [32 + esi]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:623
    mov dword ptr [20 + ebx], edx                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:624
    mov dword ptr [16 + ebx], eax                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:625
    add esi, 36                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_strcat:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_strcat
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_push_with_scope:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_push_with_scope
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_create_lexical_environment:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_create_lexical_environment
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_throw:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_throw
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_throw_static_error:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_throw_static_error
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_profile_will_call:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1310
    mov eax, dword ptr [56 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1311
    mov eax, dword ptr [9404 + eax]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1312
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1313
    jz _offlineasm_opProfilerWillCallDone
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_profile_will_call
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155

  _offlineasm_opProfilerWillCallDone:
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_profile_did_call:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1321
    mov eax, dword ptr [56 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1322
    mov eax, dword ptr [9404 + eax]                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1323
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1324
    jz _offlineasm_opProfilerDidCallDone
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_profile_did_call
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155

  _offlineasm_opProfilerDidCallDone:
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_debug:
    mov eax, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1332
    mov eax, dword ptr [48 + eax]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1333
    test eax, eax                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1334
    jz _offlineasm_opDebugDone
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call llint_slow_path_debug
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155

  _offlineasm_opDebugDone:
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_native_call_trampoline:
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:430
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    mov dword ptr [8 + ebp], 0                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1982
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1983
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1984
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1986
    sub esp, 8
    and edx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1987
    mov ebx, dword ptr [180 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1988
    mov dword ptr [5224 + ebx], ebp                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1989
    mov ecx, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1990
    mov dword ptr [0 + esp], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1991
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1992
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1993
    mov ebx, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:251
    and ebx, 15
    test ebx, ebx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:253
    jz _offlineasm_nativeCallTrampoline__77_checkStackPointerAlignment__stackPointerOkay
    mov ebx, 3735879681                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:254
    int 3                                                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:255

  _offlineasm_nativeCallTrampoline__77_checkStackPointerAlignment__stackPointerOkay:
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1995
    ffree st(1)
    call dword ptr [40 + edx]
    mov ebx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1996
    and ebx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1997
    mov ebx, dword ptr [180 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1998
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1999
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:442
    cmp dword ptr [9388 + ebx], 0                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2028
    jnz _offlineasm_nativeCallTrampoline__handleException
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2029

  _offlineasm_nativeCallTrampoline__handleException:
    mov dword ptr [5224 + ebx], ebp                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2032
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    jmp _llint_throw_from_slow_path_trampoline               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2034

  _llint_native_construct_trampoline:
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:430
    mov ebp, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    mov dword ptr [8 + ebp], 0                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1982
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1983
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1984
    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1986
    sub esp, 8
    and edx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1987
    mov ebx, dword ptr [180 + edx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1988
    mov dword ptr [5224 + ebx], ebp                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1989
    mov ecx, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1990
    mov dword ptr [0 + esp], ecx                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1991
    mov edx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1992
    mov edx, dword ptr [20 + edx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1993
    mov ebx, esp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:251
    and ebx, 15
    test ebx, ebx                                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:253
    jz _offlineasm_nativeCallTrampoline__78_checkStackPointerAlignment__stackPointerOkay
    mov ebx, 3735879681                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:254
    int 3                                                    ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:255

  _offlineasm_nativeCallTrampoline__78_checkStackPointerAlignment__stackPointerOkay:
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1995
    ffree st(1)
    call dword ptr [44 + edx]
    mov ebx, dword ptr [16 + ebp]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1996
    and ebx, -16384                                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1997
    mov ebx, dword ptr [180 + ebx]                           ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1998
    add esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:1999
    pop ebp                                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:442
    cmp dword ptr [9388 + ebx], 0                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2028
    jnz _offlineasm_78_nativeCallTrampoline__handleException
    ret                                                      ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2029

  _offlineasm_78_nativeCallTrampoline__handleException:
    mov dword ptr [5224 + ebx], ebp                          ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2032
    mov ecx, dword ptr [8 + ebp]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    mov edi, dword ptr [28 + ecx]                            ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    sal edi, 3                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    add edi, 40                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    mov esp, ebp                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    sub esp, edi
    jmp _llint_throw_from_slow_path_trampoline               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:2034

  _llint_op_get_enumerable_length:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_get_enumerable_length
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_has_indexed_property:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_has_indexed_property
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_has_structure_property:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_has_structure_property
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 20                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_has_generic_property:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_has_generic_property
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_direct_pname:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_get_direct_pname
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 28                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_get_property_enumerator:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_get_property_enumerator
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_enumerator_structure_pname:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_next_structure_enumerator_pname
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_enumerator_generic_pname:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_next_generic_enumerator_pname
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_to_index_string:
    sub esp, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:105
    push esi                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:106
    push ebp                                                 ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:107
    ffree st(0)                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:108
    ffree st(1)
    call slow_path_to_index_string
    add esp, 16                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:109
    mov esi, eax                                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:155
    add esi, 12                                              ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76

  _llint_op_profile_control_flow:
    mov eax, dword ptr [4 + esi]                             ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:139
    mov byte ptr [8 + eax], 1                                ; ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1395
    add esi, 8                                               ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:75
    jmp dword ptr [0 + esi]                                  ; ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter32_64.asm:76
llint_entry ENDP
_TEXT ENDS
END
