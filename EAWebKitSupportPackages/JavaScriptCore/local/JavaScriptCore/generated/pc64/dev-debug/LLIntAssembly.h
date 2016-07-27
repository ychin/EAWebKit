// offlineasm input hash: 104e3dc9e70722b634b050d27f6367890e3714a0 38fcc47197b774072b2654ce28092d3bdfdbac09 d09c46903c3926b12c753ddb8f065ed8b8bf775b
#if !OFFLINE_ASM_ARMv7s && OFFLINE_ASM_JSVALUE64 && !OFFLINE_ASM_BIG_ENDIAN && OFFLINE_ASM_C_LOOP && OFFLINE_ASM_ASSERT_ENABLED && !OFFLINE_ASM_ARM && !OFFLINE_ASM_ARMv7 && !OFFLINE_ASM_ARMv7_TRADITIONAL && !OFFLINE_ASM_MIPS && !OFFLINE_ASM_SH4 && !OFFLINE_ASM_X86 && !OFFLINE_ASM_X86_64 && !OFFLINE_ASM_EXECUTION_TRACING && !OFFLINE_ASM_VALUE_PROFILER && !OFFLINE_ASM_JIT_ENABLED && !OFFLINE_ASM_ALWAYS_ALLOCATE_SLOW
OFFLINE_ASM_BEGIN

OFFLINE_ASM_GLUE_LABEL(llint_begin)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_GLUE_LABEL(llint_program_prologue)
    t2.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    *CAST<intptr_t*>(cfr.i8p + 16) = t2.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:293
    rBasePC.i = *CAST<intptr_t*>(t1.i8p + 104);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:331
    rPC.i = intptr_t(0x0);                                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:332
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_eval_prologue)
    t2.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    *CAST<intptr_t*>(cfr.i8p + 16) = t2.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:293
    rBasePC.i = *CAST<intptr_t*>(t1.i8p + 104);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:331
    rPC.i = intptr_t(0x0);                                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:332
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_function_for_call_prologue)
    t2.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    *CAST<intptr_t*>(cfr.i8p + 16) = t2.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    t1.i = *CAST<intptr_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:281
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:282
    t1.i = *CAST<intptr_t*>(t1.i8p + 120);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:283
    *CAST<intptr_t*>(cfr.i8p + 8) = t1.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:297
    rBasePC.i = *CAST<intptr_t*>(t1.i8p + 104);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:331
    rPC.i = intptr_t(0x0);                                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:332

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionForCallBegin)
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:374
    t2.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:375
    t2.i = *CAST<intptr_t*>(t2.i8p + 46504);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:376
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:377
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:378
    if (*CAST<uintptr_t*>(t2.i8p + 24) <= t0.u)              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:379
        goto _offlineasm_functionInitialization__stackHeightOK;
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_stack_check(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionInitialization__stackHeightOK)
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_function_for_construct_prologue)
    t2.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    *CAST<intptr_t*>(cfr.i8p + 16) = t2.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    t1.i = *CAST<intptr_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:287
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:288
    t1.i = *CAST<intptr_t*>(t1.i8p + 128);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:289
    *CAST<intptr_t*>(cfr.i8p + 8) = t1.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:297
    rBasePC.i = *CAST<intptr_t*>(t1.i8p + 104);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:331
    rPC.i = intptr_t(0x0);                                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:332

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionForConstructBegin)
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:374
    t2.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:375
    t2.i = *CAST<intptr_t*>(t2.i8p + 46504);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:376
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:377
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:378
    if (*CAST<uintptr_t*>(t2.i8p + 24) <= t0.u)              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:379
        goto _offlineasm_0_functionInitialization__stackHeightOK;
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_stack_check(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_functionInitialization__stackHeightOK)
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_function_for_call_arity_check)
    t2.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    *CAST<intptr_t*>(cfr.i8p + 16) = t2.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    t1.i = *CAST<intptr_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:281
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:282
    t1.i = *CAST<intptr_t*>(t1.i8p + 120);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:283
    *CAST<intptr_t*>(cfr.i8p + 8) = t1.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:297
    rBasePC.i = *CAST<intptr_t*>(t1.i8p + 104);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:331
    rPC.i = intptr_t(0x0);                                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:332
    t0.u = *CAST<uint32_t*>(cfr.i8p + 48);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:191
    if (t0.u32 >= *CAST<uint32_t*>(t1.i8p + 80))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:192
        goto _offlineasm_functionForCallBegin;
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_call_arityCheck(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    if (t0.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:195
        goto _offlineasm_functionArityCheck__isArityFixupNeeded;
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:196

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__isArityFixupNeeded)
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:199
        goto _offlineasm_functionArityCheck__continue;
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:201
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:202
    t1.i64 = -t1.i64;
    t3.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:203
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:204
    t2.u = *CAST<uint32_t*>(cfr.i8p + 48);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:205
    t2.i32 = t2.i32 + int32_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:206
    t2.clearHighWord();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__copyLoop)
    t0.i64 = *CAST<int64_t*>(t3.i8p);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:208
    *CAST<int64_t*>(t3.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:209
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:210
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:211
        int32_t temp = t2.i32 - int32_t(0x1);
        t2.i32 = temp;
        if (temp != 0)
            goto  _offlineasm_functionArityCheck__copyLoop;
    }
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:213
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:214
    t2.i = t1.i;
    t0.i = intptr_t(0xa);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:215

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__fillLoop)
    *CAST<int64_t*>(t3.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:217
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:218
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:219
        int32_t temp = t2.i32 + int32_t(0x1);
        t2.i32 = temp;
        if (temp != 0)
            goto  _offlineasm_functionArityCheck__fillLoop;
    }
    t1.i = t1.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:221
    cfr.i = cfr.i + t1.i;                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:222

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__continue)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:226
    rBasePC.i = *CAST<intptr_t*>(t1.i8p + 104);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:227
    rPC.i = intptr_t(0x0);                                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:228
    goto _offlineasm_functionForCallBegin;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:229

OFFLINE_ASM_GLUE_LABEL(llint_function_for_construct_arity_check)
    t2.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    *CAST<intptr_t*>(cfr.i8p + 16) = t2.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:310
    t1.i = *CAST<intptr_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:287
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:288
    t1.i = *CAST<intptr_t*>(t1.i8p + 128);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:289
    *CAST<intptr_t*>(cfr.i8p + 8) = t1.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:297
    rBasePC.i = *CAST<intptr_t*>(t1.i8p + 104);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:331
    rPC.i = intptr_t(0x0);                                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:332
    t0.u = *CAST<uint32_t*>(cfr.i8p + 48);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:191
    if (t0.u32 >= *CAST<uint32_t*>(t1.i8p + 80))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:192
        goto _offlineasm_functionForConstructBegin;
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_construct_arityCheck(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    if (t0.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:195
        goto _offlineasm_0_functionArityCheck__isArityFixupNeeded;
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:196

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_functionArityCheck__isArityFixupNeeded)
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:199
        goto _offlineasm_0_functionArityCheck__continue;
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:201
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:202
    t1.i64 = -t1.i64;
    t3.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:203
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:204
    t2.u = *CAST<uint32_t*>(cfr.i8p + 48);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:205
    t2.i32 = t2.i32 + int32_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:206
    t2.clearHighWord();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_functionArityCheck__copyLoop)
    t0.i64 = *CAST<int64_t*>(t3.i8p);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:208
    *CAST<int64_t*>(t3.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:209
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:210
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:211
        int32_t temp = t2.i32 - int32_t(0x1);
        t2.i32 = temp;
        if (temp != 0)
            goto  _offlineasm_0_functionArityCheck__copyLoop;
    }
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:213
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:214
    t2.i = t1.i;
    t0.i = intptr_t(0xa);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:215

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_functionArityCheck__fillLoop)
    *CAST<int64_t*>(t3.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:217
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:218
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:219
        int32_t temp = t2.i32 + int32_t(0x1);
        t2.i32 = temp;
        if (temp != 0)
            goto  _offlineasm_0_functionArityCheck__fillLoop;
    }
    t1.i = t1.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:221
    cfr.i = cfr.i + t1.i;                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:222

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_functionArityCheck__continue)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:226
    rBasePC.i = *CAST<intptr_t*>(t1.i8p + 104);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:227
    rPC.i = intptr_t(0x0);                                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:228
    goto _offlineasm_functionForConstructBegin;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:229

OFFLINE_ASM_OPCODE_LABEL(op_enter)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:237
    t2.u = *CAST<uint32_t*>(t2.i8p + 60);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:238
    if (t2.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:239
        goto _offlineasm_opEnterDone;
    t0.i = intptr_t(0xa);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:240
    t2.i32 = -t2.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:241
    t2.clearHighWord();
    t2.i64 = t2.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:242

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opEnterLoop)
    t2.i64 = t2.i64 + int64_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:244
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:245
    if (t2.i64 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:246
        goto _offlineasm_opEnterLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opEnterDone)
    rPC.i = rPC.i + intptr_t(0x1);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_create_activation)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) != int64_t(0x0)) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:254
        goto _offlineasm_opCreateActivationDone;
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_create_activation(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opCreateActivationDone)
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_init_lazy_reg)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) = int64_t(0x0); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:263
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_create_arguments)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) != int64_t(0x0)) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:270
        goto _offlineasm_opCreateArgumentsDone;
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_create_arguments(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opCreateArgumentsDone)
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_create_this)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i = *CAST<intptr_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:279
    t1.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:280
    t2.i = *CAST<intptr_t*>(t0.i8p + 48);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:281
    if (t1.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:282
        goto _offlineasm_opCreateThisSlow;
    t0.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:395
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:396
        goto _offlineasm_opCreateThisSlow;
    t3.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:399
    *CAST<intptr_t*>(t1.i8p) = t3.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:400
    *CAST<intptr_t*>(t0.i8p) = t2.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:403
    *CAST<intptr_t*>(t0.i8p + 8) = intptr_t(0x0);            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:404
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:285
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opCreateThisSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_create_this(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_callee)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:296
    t2.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t1.i != t2.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:298
        goto _offlineasm_opGetCalleeSlow;
    *CAST<intptr_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) = t1.i; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:299
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetCalleeSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_get_callee(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_to_this)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:309
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:310
        goto _offlineasm_opToThisSlow;
    t0.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:311
    if (*CAST<int8_t*>(t0.i8p + 92) != int8_t(0x12))         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:312
        goto _offlineasm_opToThisSlow;
    t2.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t0.i != t2.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:314
        goto _offlineasm_opToThisSlow;
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToThisSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_to_this(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_object)
    t0.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    t1.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:325
    t2.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:326
    t0.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:395
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:396
        goto _offlineasm_opNewObjectSlow;
    t3.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:399
    *CAST<intptr_t*>(t1.i8p) = t3.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:400
    *CAST<intptr_t*>(t0.i8p) = t2.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:403
    *CAST<intptr_t*>(t0.i8p + 8) = intptr_t(0x0);            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:404
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:329
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNewObjectSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_new_object(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_mov)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariable__done;           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariable__done)
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:342
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_not)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_0_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_0_loadConstantOrVariable__done;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_loadConstantOrVariable__done)
    t2.i64 = t2.i64 ^ int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:351
    if ((t2.i64 & int64_t(-2)) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
        goto _offlineasm_opNotSlow;
    t2.i64 = t2.i64 ^ int64_t(0x7);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:354
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNotSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_not(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_eq)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__1_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__1_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__1_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__1_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:167
        goto _offlineasm_equalityComparison__slow;
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__2_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__2_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__2_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__2_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:167
        goto _offlineasm_equalityComparison__slow;
    t0.i = (t0.u32 == t1.u32);                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:381
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:370
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:371
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_eq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_neq)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__3_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__3_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__3_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__3_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:167
        goto _offlineasm_4_equalityComparison__slow;
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__4_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__4_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__4_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__4_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:167
        goto _offlineasm_4_equalityComparison__slow;
    t0.i = (t0.u32 != t1.u32);                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:387
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:370
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:371
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_equalityComparison__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_neq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_eq_null)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:393
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:394
        goto _offlineasm_equalNullComparison__immediate;
    t2.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:395
    if ((*CAST<int8_t*>(t2.i8p + 93) & int8_t(0x1)) != 0)    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:396
        goto _offlineasm_equalNullComparison__masqueradesAsUndefined;
    t0.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:397
    goto _offlineasm_equalNullComparison__done;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:398

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNullComparison__masqueradesAsUndefined)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:400
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:401
    t0.i = (*CAST<uintptr_t*>(t2.i8p + 8) == t0.u);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:402
    goto _offlineasm_equalNullComparison__done;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:403

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNullComparison__immediate)
    t0.i64 = t0.i64 & int64_t(-9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:405
    t0.i = (t0.u64 == uint64_t(0x2));                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:406

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNullComparison__done)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:414
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:415
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_neq_null)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:393
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:394
        goto _offlineasm_4_equalNullComparison__immediate;
    t2.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:395
    if ((*CAST<int8_t*>(t2.i8p + 93) & int8_t(0x1)) != 0)    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:396
        goto _offlineasm_4_equalNullComparison__masqueradesAsUndefined;
    t0.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:397
    goto _offlineasm_4_equalNullComparison__done;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:398

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_equalNullComparison__masqueradesAsUndefined)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:400
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:401
    t0.i = (*CAST<uintptr_t*>(t2.i8p + 8) == t0.u);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:402
    goto _offlineasm_4_equalNullComparison__done;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:403

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_equalNullComparison__immediate)
    t0.i64 = t0.i64 & int64_t(-9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:405
    t0.i = (t0.u64 == uint64_t(0x2));                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:406

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_equalNullComparison__done)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 ^ int64_t(0x7);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:424
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_stricteq)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_strictEq__5_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_strictEq__5_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__5_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__5_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_strictEq__6_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_strictEq__6_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__6_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__6_loadConstantOrVariable__done)
    t2.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
    t2.i64 = t2.i64 | t1.i64;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:435
    if ((t2.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:436
        goto _offlineasm_strictEq__slow;
    if (t0.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:437
        goto _offlineasm_strictEq__leftOK;
    if ((t0.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:438
        goto _offlineasm_strictEq__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__leftOK)
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:440
        goto _offlineasm_strictEq__rightOK;
    if ((t1.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:441
        goto _offlineasm_strictEq__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__rightOK)
    t0.i = (t0.u64 == t1.u64);                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:456
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:445
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:446
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_stricteq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_nstricteq)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_strictEq__7_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_strictEq__7_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__7_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__7_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_strictEq__8_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_strictEq__8_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__8_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__8_loadConstantOrVariable__done)
    t2.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
    t2.i64 = t2.i64 | t1.i64;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:435
    if ((t2.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:436
        goto _offlineasm_8_strictEq__slow;
    if (t0.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:437
        goto _offlineasm_8_strictEq__leftOK;
    if ((t0.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:438
        goto _offlineasm_8_strictEq__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_8_strictEq__leftOK)
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:440
        goto _offlineasm_8_strictEq__rightOK;
    if ((t1.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:441
        goto _offlineasm_8_strictEq__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_8_strictEq__rightOK)
    t0.i = (t0.u64 != t1.u64);                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:445
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:446
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_8_strictEq__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_nstricteq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_inc)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:469
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:470
        goto _offlineasm_preOp__slow;
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:483
        int32_t a = int32_t(0x1);
        int32_t b = t1.i32;
        // sign(b) sign(a) | Overflows if:
        // 0       0       | sign(b+a) = 1 (pos + pos != neg)
        // 0       1       | never
        // 1       0       | never
        // 1       1       | sign(b+a) = 0 (neg + neg != pos)
        bool didOverflow = ((SIGN_BIT32(b) == SIGN_BIT32(a)) && (SIGN_BIT32(b+a) != SIGN_BIT32(a)));
        t1.i32 = t1.i32 + int32_t(0x1);
        if (didOverflow)
            goto _offlineasm_preOp__slow;
    }
    t1.i64 = t1.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:472
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) = t1.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:473
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_preOp__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_inc(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_dec)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:469
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:470
        goto _offlineasm_8_preOp__slow;
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:489
        int32_t a = int32_t(0x1);
        int32_t b = t1.i32;
        // sign(b) sign(a) | Overflows if:
        // 0       0       | never
        // 0       1       | sign(b-a) = 1 (pos - neg != pos)
        // 1       0       | sign(b-a) = 0 (neg - pos != pos)
        // 1       1       | never
        bool didOverflow = ((SIGN_BIT32(b) != SIGN_BIT32(a)) && (SIGN_BIT32(b-a) == SIGN_BIT32(a)));
        t1.i32 = t1.i32 - int32_t(0x1);
        if (didOverflow)
            goto _offlineasm_8_preOp__slow;
    }
    t1.i64 = t1.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:472
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) = t1.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:473
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_8_preOp__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_dec(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_to_number)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_9_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_9_loadConstantOrVariable__done;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_9_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_9_loadConstantOrVariable__done)
    if (t2.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
        goto _offlineasm_opToNumberIsImmediate;
    if ((t2.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
        goto _offlineasm_opToNumberSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToNumberIsImmediate)
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToNumberSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_to_number(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_negate)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_10_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_10_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_10_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_10_loadConstantOrVariable__done)
    if (t2.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:514
        goto _offlineasm_opNegateNotInt;
    if ((t2.i32 & int32_t(0x7fffffff)) == 0)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:515
        goto _offlineasm_opNegateSlow;
    t2.i32 = -t2.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:516
    t2.clearHighWord();
    t2.i64 = t2.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:517
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:518
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNegateNotInt)
    if ((t2.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:521
        goto _offlineasm_opNegateSlow;
    t2.i64 = t2.i64 ^ int64_t(0x8000000000000000);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:522
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:523
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNegateSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_negate(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_add)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_binaryOp__binaryOpCustomStore__11_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_binaryOp__binaryOpCustomStore__11_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__11_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__11_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__12_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:536
        goto _offlineasm_binaryOp__binaryOpCustomStore__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:537
        goto _offlineasm_binaryOp__binaryOpCustomStore__op2NotInt;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:593
        int32_t a = t1.i32;
        int32_t b = t0.i32;
        // sign(b) sign(a) | Overflows if:
        // 0       0       | sign(b+a) = 1 (pos + pos != neg)
        // 0       1       | never
        // 1       0       | never
        // 1       1       | sign(b+a) = 0 (neg + neg != pos)
        bool didOverflow = ((SIGN_BIT32(b) == SIGN_BIT32(a)) && (SIGN_BIT32(b+a) != SIGN_BIT32(a)));
        t0.i32 = t0.i32 + t1.i32;
        if (didOverflow)
            goto _offlineasm_binaryOp__binaryOpCustomStore__slow;
    }
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:584
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:585
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:544
        goto _offlineasm_binaryOp__binaryOpCustomStore__slow;
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:545
        goto _offlineasm_binaryOp__binaryOpCustomStore__op1NotIntOp2Int;
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:546
        goto _offlineasm_binaryOp__binaryOpCustomStore__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:547
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:548
    goto _offlineasm_binaryOp__binaryOpCustomStore__op1NotIntReady; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:549

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__op1NotIntOp2Int)
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:551

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__op1NotIntReady)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:554
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:555
    d0.d = d0.d + d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:594
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:557
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:558
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:559
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__op2NotInt)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:565
        goto _offlineasm_binaryOp__binaryOpCustomStore__slow;
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:566
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:567
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:568
    d0.d = d0.d + d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:594
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:570
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:571
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:572
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_add(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_mul)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_binaryOpCustomStore__13_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__13_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__13_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_binaryOpCustomStore__14_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_binaryOpCustomStore__14_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__14_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__14_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:536
        goto _offlineasm_14_binaryOpCustomStore__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:537
        goto _offlineasm_14_binaryOpCustomStore__op2NotInt;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:603
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:604
        uint32_t a = t1.i32;
        uint32_t b = t3.i32;
        bool didOverflow = ((b | a) >> 15);
        t3.i32 = t3.i32 * t1.i32;
        if (didOverflow)
            goto _offlineasm_14_binaryOpCustomStore__slow;
    }
    if (t3.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:605
        goto _offlineasm_binaryOpCustomStore__integerOperationAndStore__done;
    if (t1.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:606
        goto _offlineasm_14_binaryOpCustomStore__slow;
    if (t0.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:607
        goto _offlineasm_14_binaryOpCustomStore__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__integerOperationAndStore__done)
    t3.i64 = t3.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:609
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t3.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:610
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_14_binaryOpCustomStore__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:544
        goto _offlineasm_14_binaryOpCustomStore__slow;
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:545
        goto _offlineasm_14_binaryOpCustomStore__op1NotIntOp2Int;
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:546
        goto _offlineasm_14_binaryOpCustomStore__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:547
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:548
    goto _offlineasm_14_binaryOpCustomStore__op1NotIntReady; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:549

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_14_binaryOpCustomStore__op1NotIntOp2Int)
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:551

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_14_binaryOpCustomStore__op1NotIntReady)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:554
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:555
    d0.d = d0.d * d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:612
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:557
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:558
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:559
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_14_binaryOpCustomStore__op2NotInt)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:565
        goto _offlineasm_14_binaryOpCustomStore__slow;
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:566
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:567
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:568
    d0.d = d0.d * d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:612
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:570
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:571
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:572
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_14_binaryOpCustomStore__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_mul(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_sub)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_binaryOp__binaryOpCustomStore__15_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_binaryOp__binaryOpCustomStore__15_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__15_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__15_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_binaryOp__binaryOpCustomStore__16_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_binaryOp__binaryOpCustomStore__16_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__16_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__16_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:536
        goto _offlineasm_binaryOp__16_binaryOpCustomStore__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:537
        goto _offlineasm_binaryOp__16_binaryOpCustomStore__op2NotInt;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:619
        int32_t a = t1.i32;
        int32_t b = t0.i32;
        // sign(b) sign(a) | Overflows if:
        // 0       0       | never
        // 0       1       | sign(b-a) = 1 (pos - neg != pos)
        // 1       0       | sign(b-a) = 0 (neg - pos != pos)
        // 1       1       | never
        bool didOverflow = ((SIGN_BIT32(b) != SIGN_BIT32(a)) && (SIGN_BIT32(b-a) == SIGN_BIT32(a)));
        t0.i32 = t0.i32 - t1.i32;
        if (didOverflow)
            goto _offlineasm_binaryOp__16_binaryOpCustomStore__slow;
    }
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:584
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:585
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__16_binaryOpCustomStore__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:544
        goto _offlineasm_binaryOp__16_binaryOpCustomStore__slow;
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:545
        goto _offlineasm_binaryOp__16_binaryOpCustomStore__op1NotIntOp2Int;
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:546
        goto _offlineasm_binaryOp__16_binaryOpCustomStore__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:547
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:548
    goto _offlineasm_binaryOp__16_binaryOpCustomStore__op1NotIntReady; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:549

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__16_binaryOpCustomStore__op1NotIntOp2Int)
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:551

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__16_binaryOpCustomStore__op1NotIntReady)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:554
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:555
    d0.d = d0.d - d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:620
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:557
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:558
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:559
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__16_binaryOpCustomStore__op2NotInt)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:565
        goto _offlineasm_binaryOp__16_binaryOpCustomStore__slow;
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:566
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:567
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:568
    d0.d = d0.d - d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:620
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:570
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:571
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:572
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__16_binaryOpCustomStore__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_sub(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_div)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_binaryOpCustomStore__17_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_binaryOpCustomStore__17_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__17_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__17_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_binaryOpCustomStore__18_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_binaryOpCustomStore__18_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__18_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__18_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:536
        goto _offlineasm_18_binaryOpCustomStore__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:537
        goto _offlineasm_18_binaryOpCustomStore__op2NotInt;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:629
        goto _offlineasm_18_binaryOpCustomStore__slow;
    if (t1.i32 != int32_t(-1))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:630
        goto _offlineasm_binaryOpCustomStore__integerOperationAndStore__notNeg2TwoThe31DivByNeg1;
    if (t0.i32 == int32_t(-2147483648))                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:631
        goto _offlineasm_18_binaryOpCustomStore__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__integerOperationAndStore__notNeg2TwoThe31DivByNeg1)
    if (t0.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:633
        goto _offlineasm_binaryOpCustomStore__integerOperationAndStore__intOK;
    if (t1.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:634
        goto _offlineasm_18_binaryOpCustomStore__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__integerOperationAndStore__intOK)
    t3.i = t1.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:636
    t0.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:637
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:638
        int64_t temp = t0.i32; // sign extend the low 32bit
        t0.i32 = temp; // low word
        t0.clearHighWord();
        t1.i32 = uint64_t(temp) >> 32; // high word
        t1.clearHighWord();
    }
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:639
        int64_t dividend = (int64_t(t1.u32) << 32) | t0.u32;
        int64_t divisor = t3.i;
        t1.i32 = dividend % divisor; // remainder
        t1.clearHighWord();
        t0.i32 = dividend / divisor; // quotient
        t0.clearHighWord();
    }
    if (t1.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:640
        goto _offlineasm_18_binaryOpCustomStore__slow;
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:641
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:642
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_18_binaryOpCustomStore__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:544
        goto _offlineasm_18_binaryOpCustomStore__slow;
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:545
        goto _offlineasm_18_binaryOpCustomStore__op1NotIntOp2Int;
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:546
        goto _offlineasm_18_binaryOpCustomStore__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:547
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:548
    goto _offlineasm_18_binaryOpCustomStore__op1NotIntReady; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:549

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_18_binaryOpCustomStore__op1NotIntOp2Int)
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:551

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_18_binaryOpCustomStore__op1NotIntReady)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:554
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:555
    d0.d = d0.d / d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:644
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:557
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:558
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:559
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_18_binaryOpCustomStore__op2NotInt)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:565
        goto _offlineasm_18_binaryOpCustomStore__slow;
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:566
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:567
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:568
    d0.d = d0.d / d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:644
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:570
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:571
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:572
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_18_binaryOpCustomStore__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_div(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_lshift)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__19_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__19_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__19_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__19_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__20_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__20_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__20_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__20_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:654
        goto _offlineasm_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:655
        goto _offlineasm_bitOp__slow;
    t0.i32 = t0.i32 << (t1.i & 0x1f);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:669
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:657
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:658
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_lshift(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_rshift)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__21_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__21_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__21_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__21_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__22_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__22_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__22_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__22_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:654
        goto _offlineasm_22_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:655
        goto _offlineasm_22_bitOp__slow;
    t0.i32 = t0.i32 >> (t1.i & 0x1f);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:677
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:657
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:658
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_22_bitOp__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_rshift(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_urshift)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__23_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__23_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__23_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__23_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__24_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__24_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__24_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__24_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:654
        goto _offlineasm_24_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:655
        goto _offlineasm_24_bitOp__slow;
    t0.u32 = t0.u32 >> (t1.i & 0x1f);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:686
    t0.clearHighWord();
    if (t0.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:687
        goto _offlineasm_24_bitOp__slow;
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:657
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:658
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_24_bitOp__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_urshift(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_bitand)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__25_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__25_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__25_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__25_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__26_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__26_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__26_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__26_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:654
        goto _offlineasm_26_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:655
        goto _offlineasm_26_bitOp__slow;
    t0.i32 = t0.i32 & t1.i32;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:696
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:657
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:658
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_26_bitOp__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_bitand(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_bitxor)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__27_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__27_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__27_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__27_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__28_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__28_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__28_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__28_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:654
        goto _offlineasm_28_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:655
        goto _offlineasm_28_bitOp__slow;
    t0.i32 = t0.i32 ^ t1.i32;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:704
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:657
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:658
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_28_bitOp__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_bitxor(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_bitor)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__29_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__29_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__29_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__29_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_bitOp__30_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_bitOp__30_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__30_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__30_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:654
        goto _offlineasm_30_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:655
        goto _offlineasm_30_bitOp__slow;
    t0.i32 = t0.i32 | t1.i32;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:712
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:657
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:658
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_30_bitOp__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_bitor(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_check_has_instance)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableCell__31_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableCell__31_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__31_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__31_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opCheckHasInstanceSlow;
    t0.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:721
    if ((*CAST<int8_t*>(t0.i8p + 93) & int8_t(0x8)) == 0)    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:722
        goto _offlineasm_opCheckHasInstanceSlow;
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opCheckHasInstanceSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_check_has_instance(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_instanceof)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableCell__32_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableCell__32_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__32_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__32_loadConstantOrVariable__done)
    if ((t1.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opInstanceofSlow;
    t2.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:736
    if (*CAST<uint8_t*>(t2.i8p + 92) < uint8_t(0x11))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:737
        goto _offlineasm_opInstanceofSlow;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableCell__33_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableCell__33_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__33_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__33_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opInstanceofSlow;
    t0.i = intptr_t(0x1);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:742

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opInstanceofLoop)
    t2.i = *CAST<intptr_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:744
    t2.i64 = *CAST<int64_t*>(t2.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:745
    if (t2.i64 == t1.i64)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:746
        goto _offlineasm_opInstanceofDone;
    if ((t2.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:747
        goto _offlineasm_opInstanceofLoop;
    t0.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:749

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opInstanceofDone)
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:751
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:752
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opInstanceofSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_instanceof(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_undefined)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_34_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_34_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_34_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_34_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:765
        goto _offlineasm_opIsUndefinedCell;
    t3.i = (t0.u64 == uint64_t(0xa));                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:766
    t3.i64 = t3.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:767
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t3.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:768
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opIsUndefinedCell)
    t0.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:771
    if ((*CAST<int8_t*>(t0.i8p + 93) & int8_t(0x1)) != 0)    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:772
        goto _offlineasm_masqueradesAsUndefined;
    t1.i = intptr_t(0x6);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:773
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t1.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:774
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_masqueradesAsUndefined)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:777
    t1.i = *CAST<intptr_t*>(t1.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:778
    t3.i = (*CAST<uintptr_t*>(t0.i8p + 8) == t1.u);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:779
    t3.i64 = t3.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:780
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t3.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:781
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_boolean)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_35_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_35_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_35_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_35_loadConstantOrVariable__done)
    t0.i64 = t0.i64 ^ int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:790
    t0.i = ((t0.i64 & int64_t(-2)) == 0);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:791
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:792
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:793
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_number)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_36_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_36_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_36_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_36_loadConstantOrVariable__done)
    t1.i = ((t0.i64 & tagTypeNumber.i64) != 0);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:802
    t1.i64 = t1.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:803
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t1.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:804
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_string)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_37_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_37_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_37_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_37_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:813
        goto _offlineasm_opIsStringNotCell;
    t0.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:814
    t1.i = (*CAST<uint8_t*>(t0.i8p + 92) == uint8_t(0x5));   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:815
    t1.i64 = t1.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:816
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t1.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:817
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opIsStringNotCell)
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = int64_t(0x6); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:820
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_init_global_const)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_38_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_38_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_38_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_38_loadConstantOrVariable__done)
    *CAST<int64_t*>(t0.i8p) = t2.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:855
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_getById__loadConstantOrVariableCell__39_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_getById__loadConstantOrVariableCell__39_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__loadConstantOrVariableCell__39_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t3.i = *CAST<intptr_t*>(t3.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__loadConstantOrVariableCell__39_loadConstantOrVariable__done)
    if ((t3.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_getById__opGetByIdSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<intptr_t*>(t3.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:874
        goto _offlineasm_getById__opGetByIdSlow;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(t3.i8p + (t2.i << 0) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:876
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:877
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__opGetByIdSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_by_id(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_out_of_line)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_getById__loadConstantOrVariableCell__40_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_getById__loadConstantOrVariableCell__40_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__loadConstantOrVariableCell__40_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t3.i = *CAST<intptr_t*>(t3.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__loadConstantOrVariableCell__40_loadConstantOrVariable__done)
    if ((t3.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_40_getById__opGetByIdSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i = *CAST<intptr_t*>(t3.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:548
    if (*CAST<intptr_t*>(t3.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:874
        goto _offlineasm_40_getById__opGetByIdSlow;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 0) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:876
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t3.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:877
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_40_getById__opGetByIdSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_by_id(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_array_length)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableCell__41_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableCell__41_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__41_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t3.i = *CAST<intptr_t*>(t3.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__41_loadConstantOrVariable__done)
    if ((t3.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opGetArrayLengthSlow;
    t2.i = *CAST<intptr_t*>(t3.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:900
    t2.i = *CAST<uint8_t*>(t2.i8p + 95);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:256
    if ((t2.i32 & int32_t(0x1)) == 0)                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:902
        goto _offlineasm_opGetArrayLengthSlow;
    if ((t2.i32 & int32_t(0x1e)) == 0)                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:903
        goto _offlineasm_opGetArrayLengthSlow;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i = *CAST<intptr_t*>(t3.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:905
    t0.u = *CAST<uint32_t*>(t0.i8p - 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:906
    if (t0.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:907
        goto _offlineasm_opGetArrayLengthSlow;
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:908
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:910
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetArrayLengthSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_by_id(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_arguments_length)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) != 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:922
        goto _offlineasm_opGetArgumentsLengthSlow;
    t2.u = *CAST<uint32_t*>(cfr.i8p + 48);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:923
    t2.i32 = t2.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:924
    t2.clearHighWord();
    t2.i64 = t2.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:925
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:926
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetArgumentsLengthSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_arguments_length(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id)
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putById__loadConstantOrVariableCell__42_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putById__loadConstantOrVariableCell__42_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__loadConstantOrVariableCell__42_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__loadConstantOrVariableCell__42_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opPutByIdSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:944
        goto _offlineasm_opPutByIdSlow;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putById__getPropertyStorage__continuation__43_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putById__getPropertyStorage__continuation__43_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__getPropertyStorage__continuation__43_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t3.i = *CAST<intptr_t*>(t3.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__getPropertyStorage__continuation__43_loadConstantOrVariable__done)
    *CAST<int64_t*>(t0.i8p + (t1.i << 0) + intptr_t(0x0)) = t3.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:948
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByIdSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_put_by_id(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_out_of_line)
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putById__loadConstantOrVariableCell__44_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putById__loadConstantOrVariableCell__44_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__loadConstantOrVariableCell__44_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__loadConstantOrVariableCell__44_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opPutByIdSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:548
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:944
        goto _offlineasm_opPutByIdSlow;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putById__getPropertyStorage__continuation__45_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putById__getPropertyStorage__continuation__45_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__getPropertyStorage__continuation__45_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__getPropertyStorage__continuation__45_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p + (t1.i << 0) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:948
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition_direct)
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__46_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__46_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__46_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__46_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opPutByIdSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:971
        goto _offlineasm_opPutByIdSlow;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = t1.i + t0.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:978
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__47_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__47_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__47_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__47_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:981
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    *CAST<intptr_t*>(t0.i8p) = t1.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:983
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition_direct_out_of_line)
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__48_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__48_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__48_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__48_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opPutByIdSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:971
        goto _offlineasm_opPutByIdSlow;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:548
    t3.i = t1.i + t3.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:978
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__49_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__49_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__49_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__49_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:981
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    *CAST<intptr_t*>(t0.i8p) = t1.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:983
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition_normal)
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__50_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__50_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__50_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__50_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opPutByIdSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:971
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t3.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:994
        goto _offlineasm_putByIdTransition__additionalChecks__assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__assert__ok)
    t3.i = *CAST<intptr_t*>(t3.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:995
    if (t3.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:996
        goto _offlineasm_putByIdTransition__additionalChecks__50_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__50_assert__ok)
    if (*CAST<int64_t*>(t1.i8p + 16) == int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:997
        goto _offlineasm_putByIdTransition__additionalChecks__done;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__loop)
    t1.i64 = *CAST<int64_t*>(t1.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:999
    t1.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1000
    if (t1.i != *CAST<intptr_t*>(t3.i8p))                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1001
        goto _offlineasm_opPutByIdSlow;
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1002
    if (*CAST<int64_t*>(t1.i8p + 16) != int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1003
        goto _offlineasm_putByIdTransition__additionalChecks__loop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__done)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = t1.i + t0.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:978
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__51_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__51_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__51_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__51_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:981
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    *CAST<intptr_t*>(t0.i8p) = t1.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:983
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition_normal_out_of_line)
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__52_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__52_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__52_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__52_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opPutByIdSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:971
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (t3.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:994
        goto _offlineasm_putByIdTransition__additionalChecks__52_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__52_assert__ok)
    t3.i = *CAST<intptr_t*>(t3.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:995
    if (t3.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:996
        goto _offlineasm_putByIdTransition__additionalChecks__53_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__53_assert__ok)
    if (*CAST<int64_t*>(t1.i8p + 16) == int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:997
        goto _offlineasm_putByIdTransition__53_additionalChecks__done;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__53_additionalChecks__loop)
    t1.i64 = *CAST<int64_t*>(t1.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:999
    t1.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1000
    if (t1.i != *CAST<intptr_t*>(t3.i8p))                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1001
        goto _offlineasm_opPutByIdSlow;
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1002
    if (*CAST<int64_t*>(t1.i8p + 16) != int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1003
        goto _offlineasm_putByIdTransition__53_additionalChecks__loop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__53_additionalChecks__done)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:548
    t3.i = t1.i + t3.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:978
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__53_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__53_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__53_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__53_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:981
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    *CAST<intptr_t*>(t0.i8p) = t1.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:983
    rPC.i = rPC.i + intptr_t(0x9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_by_val)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableCell__54_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableCell__54_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__54_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__54_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opGetByValSlow;
    t2.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1027
    t3.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    t2.i = *CAST<uint8_t*>(t2.i8p + 95);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:256
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableInt32__55_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableInt32__55_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableInt32__55_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableInt32__55_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:167
        goto _offlineasm_opGetByValSlow;
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1032
    t3.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1033
    t2.i32 = t2.i32 & int32_t(0x1e);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1034
    t2.clearHighWord();
    if (t2.i32 == int32_t(0x14))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1035
        goto _offlineasm_opGetByValIsContiguous;
    if (t2.i32 != int32_t(0x1a))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1036
        goto _offlineasm_opGetByValNotContiguous;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValIsContiguous)
    if (t1.u32 >= *CAST<uint32_t*>(t3.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1039
        goto _offlineasm_opGetByValOutOfBounds;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i64 = *CAST<int64_t*>(t3.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1041
    if (t2.i64 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1042
        goto _offlineasm_opGetByValOutOfBounds;
    goto _offlineasm_opGetByValDone;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1043

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValNotContiguous)
    if (t2.i32 != int32_t(0x16))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1046
        goto _offlineasm_opGetByValNotDouble;
    if (t1.u32 >= *CAST<uint32_t*>(t3.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1047
        goto _offlineasm_opGetByValOutOfBounds;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1048
    d0.d = *CAST<double*>(t3.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1049
    if (std::isnan(d0.d) || std::isnan(d0.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1050
        || (d0.d != d0.d))
        goto _offlineasm_opGetByValOutOfBounds;
    t2.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1051
    t2.i64 = t2.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1052
    goto _offlineasm_opGetByValDone;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1053

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValNotDouble)
    t2.i32 = t2.i32 - int32_t(0x1c);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1056
    t2.clearHighWord();
    if (t2.u32 > uint32_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1057
        goto _offlineasm_opGetByValSlow;
    if (t1.u32 >= *CAST<uint32_t*>(t3.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1058
        goto _offlineasm_opGetByValOutOfBounds;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i64 = *CAST<int64_t*>(t3.i8p + (t1.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1060
    if (t2.i64 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1061
        goto _offlineasm_opGetByValOutOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValDone)
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1064
    rPC.i = rPC.i + intptr_t(0x6);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValOutOfBounds)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_by_val(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x6);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_argument_by_val)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) != 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1084
        goto _offlineasm_opGetArgumentByValSlow;
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableInt32__56_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableInt32__56_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableInt32__56_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableInt32__56_loadConstantOrVariable__done)
    if (t2.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:167
        goto _offlineasm_opGetArgumentByValSlow;
    t2.i32 = t2.i32 + int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1086
    t2.clearHighWord();
    t1.u = *CAST<uint32_t*>(cfr.i8p + 48);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1087
    if (t2.u32 >= t1.u32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1088
        goto _offlineasm_opGetArgumentByValSlow;
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1091
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1092
    rPC.i = rPC.i + intptr_t(0x6);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetArgumentByValSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_argument_by_val(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x6);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_by_pname)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_57_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_57_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_57_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_57_loadConstantOrVariable__done)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_assertNotConstant__57_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_assertNotConstant__57_assert__ok)
    if (t0.i64 != *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0))) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1107
        goto _offlineasm_opGetByPnameSlow;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableCell__58_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableCell__58_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__58_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__58_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opGetByPnameSlow;
    if (t3.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_assertNotConstant__58_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_assertNotConstant__58_assert__ok)
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1112
    t2.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1113
    if (t2.i != *CAST<intptr_t*>(t1.i8p + 8))                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1114
        goto _offlineasm_opGetByPnameSlow;
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.u = *CAST<uint32_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1116
    t3.i32 = t3.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1117
    t3.clearHighWord();
    if (t3.u32 >= *CAST<uint32_t*>(t1.i8p + 24))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1118
        goto _offlineasm_opGetByPnameSlow;
    if (t3.i32 < *CAST<int32_t*>(t1.i8p + 32))               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1119
        goto _offlineasm_opGetByPnameInlineProperty;
    t3.i32 = t3.i32 + int32_t(0x64);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1120
    t3.clearHighWord();
    t3.i32 = t3.i32 - *CAST<int32_t*>(t1.i8p + 32);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1121
    t3.clearHighWord();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByPnameInlineProperty)
    if (t3.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:825
        goto _offlineasm_loadPropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:826
    t3.i32 = -t3.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:827
    t3.clearHighWord();
    t3.i64 = t3.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:828
    goto _offlineasm_loadPropertyAtVariableOffset__ready;    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:829

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadPropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:831

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadPropertyAtVariableOffset__ready)
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x310)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:833
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1125
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByPnameSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_by_pname(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_val)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableCell__59_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableCell__59_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__59_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__59_loadConstantOrVariable__done)
    if ((t1.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:172
        goto _offlineasm_opPutByValSlow;
    t2.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1155
    t3.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    t2.i = *CAST<uint8_t*>(t2.i8p + 95);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:256
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_loadConstantOrVariableInt32__60_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_loadConstantOrVariableInt32__60_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableInt32__60_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t3.i = *CAST<intptr_t*>(t3.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableInt32__60_loadConstantOrVariable__done)
    if (t3.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:167
        goto _offlineasm_opPutByValSlow;
    t3.i64 = t3.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1160
    t0.i = *CAST<intptr_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1161
    t2.i32 = t2.i32 & int32_t(0x1e);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1162
    t2.clearHighWord();
    if (t2.i32 != int32_t(0x14))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1163
        goto _offlineasm_opPutByValNotInt32;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1134
        goto _offlineasm_contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_contiguousPutByVal__storeCallback__61_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_contiguousPutByVal__storeCallback__61_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeCallback__61_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeCallback__61_loadConstantOrVariable__done)
    if (t1.u < tagTypeNumber.u)                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1167
        goto _offlineasm_opPutByValSlow;
    *CAST<intptr_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)) = t1.i; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1168
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1141
        goto _offlineasm_opPutByValOutOfBounds;
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1146
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1147
    goto _offlineasm_contiguousPutByVal__storeResult;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1148

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByValNotInt32)
    if (t2.i32 != int32_t(0x16))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1172
        goto _offlineasm_opPutByValNotDouble;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1134
        goto _offlineasm_62_contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_62_contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_contiguousPutByVal__storeCallback__62_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_contiguousPutByVal__storeCallback__62_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeCallback__62_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeCallback__62_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1176
        goto _offlineasm_contiguousPutByVal__storeCallback__notInt;
    d0.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1177
    goto _offlineasm_contiguousPutByVal__storeCallback__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1178

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeCallback__notInt)
    t1.i = t1.i + tagTypeNumber.i;                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1180
    d0.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1181
    if (std::isnan(d0.d) || std::isnan(d0.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1182
        || (d0.d != d0.d))
        goto _offlineasm_opPutByValSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeCallback__ready)
    *CAST<double*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)) = d0.d; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1184
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_62_contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1141
        goto _offlineasm_opPutByValOutOfBounds;
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1146
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1147
    goto _offlineasm_62_contiguousPutByVal__storeResult;     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1148

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByValNotDouble)
    if (t2.i32 != int32_t(0x1a))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1188
        goto _offlineasm_opPutByValNotContiguous;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1134
        goto _offlineasm_63_contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_63_contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_contiguousPutByVal__storeCallback__63_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_contiguousPutByVal__storeCallback__63_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeCallback__63_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_contiguousPutByVal__storeCallback__63_loadConstantOrVariable__done)
    *CAST<intptr_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x0)) = t1.i; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1193
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_63_contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1141
        goto _offlineasm_opPutByValOutOfBounds;
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1146
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1147
    goto _offlineasm_63_contiguousPutByVal__storeResult;     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1148

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByValNotContiguous)
    if (t2.i32 != int32_t(0x1c))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1197
        goto _offlineasm_opPutByValSlow;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1198
        goto _offlineasm_opPutByValOutOfBounds;
    if (*CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x10)) == 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1199
        goto _offlineasm_opPutByValArrayStorageEmpty;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByValArrayStorageStoreResult)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_64_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_64_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_64_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_64_loadConstantOrVariable__done)
    *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x10)) = t1.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1204
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByValArrayStorageEmpty)
    *CAST<int32_t*>(t0.i8p + 12) = *CAST<int32_t*>(t0.i8p + 12) + int32_t(0x1); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1212
    if (t3.u32 < *CAST<uint32_t*>(t0.i8p - 8))               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1213
        goto _offlineasm_opPutByValArrayStorageStoreResult;
    t1.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1214
    t1.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t1.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1215
    goto _offlineasm_opPutByValArrayStorageStoreResult;      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1216

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByValOutOfBounds)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByValSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_put_by_val(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jmp)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jeq_null)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_equalNull__assertNotConstant__64_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__assertNotConstant__64_assert__ok)
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1253
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1254
        goto _offlineasm_equalNull__immediate;
    t2.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1255
    if ((*CAST<int8_t*>(t2.i8p + 93) & int8_t(0x1)) == 0)    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1272
        goto _offlineasm_equalNull__cellHandler__notMasqueradesAsUndefined;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1273
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1274
    if (*CAST<intptr_t*>(t2.i8p + 8) == t0.i)                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1275
        goto _offlineasm_equalNull__target;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__cellHandler__notMasqueradesAsUndefined)
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__target)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__immediate)
    t0.i64 = t0.i64 & int64_t(-9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1263
    if (t0.i64 == int64_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1278
        goto _offlineasm_equalNull__target;
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jneq_null)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_equalNull__assertNotConstant__65_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__assertNotConstant__65_assert__ok)
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1253
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1254
        goto _offlineasm_65_equalNull__immediate;
    t2.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1255
    if ((*CAST<int8_t*>(t2.i8p + 93) & int8_t(0x1)) == 0)    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1285
        goto _offlineasm_65_equalNull__target;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1286
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1287
    if (*CAST<intptr_t*>(t2.i8p + 8) != t0.i)                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1288
        goto _offlineasm_65_equalNull__target;
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_65_equalNull__target)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_65_equalNull__immediate)
    t0.i64 = t0.i64 & int64_t(-9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1263
    if (t0.i64 != int64_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1290
        goto _offlineasm_65_equalNull__target;
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jneq_ptr)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1297
    t2.i = *CAST<intptr_t*>(t2.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1298
    t1.i = *CAST<intptr_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x400)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1299
    if (t1.i != *CAST<intptr_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0))) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1300
        goto _offlineasm_opJneqPtrTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opJneqPtrTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_switch_imm)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_65_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_65_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_65_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_65_loadConstantOrVariable__done)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1354
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1355
    t3.i32 = t3.i32 * int32_t(0x18);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1356
    t3.clearHighWord();
    t2.i = *CAST<intptr_t*>(t2.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1357
    t2.i = t2.i + t3.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1358
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1359
        goto _offlineasm_opSwitchImmNotInt;
    t1.i32 = t1.i32 - *CAST<int32_t*>(t2.i8p + 16);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1360
    t1.clearHighWord();
    if (t1.u32 >= *CAST<uint32_t*>(t2.i8p + 12))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1361
        goto _offlineasm_opSwitchImmFallThrough;
    t3.i = *CAST<intptr_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1362
    t1.i = *CAST<int32_t*>(t3.i8p + (t1.i << 2) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1363
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1364
        goto _offlineasm_opSwitchImmFallThrough;
    rPC.i = rPC.i + t1.i;                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchImmNotInt)
    if ((t1.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1368
        goto _offlineasm_opSwitchImmSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchImmFallThrough)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchImmSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_switch_imm(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_switch_char)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_66_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_66_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_66_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_66_loadConstantOrVariable__done)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1382
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1383
    t3.i32 = t3.i32 * int32_t(0x18);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1384
    t3.clearHighWord();
    t2.i = *CAST<intptr_t*>(t2.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1385
    t2.i = t2.i + t3.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1386
    if ((t1.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1387
        goto _offlineasm_opSwitchCharFallThrough;
    t0.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1388
    if (*CAST<int8_t*>(t0.i8p + 92) != int8_t(0x5))          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1389
        goto _offlineasm_opSwitchCharFallThrough;
    if (*CAST<int32_t*>(t1.i8p + 12) != int32_t(0x1))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1390
        goto _offlineasm_opSwitchCharFallThrough;
    t0.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1391
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1392
        goto _offlineasm_opSwitchOnRope;
    t1.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1393
    if ((*CAST<int32_t*>(t0.i8p + 24) & int32_t(0x20)) != 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1394
        goto _offlineasm_opSwitchChar8Bit;
    t0.i = *CAST<uint16_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1395
    goto _offlineasm_opSwitchCharReady;                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1396

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchChar8Bit)
    t0.i = *CAST<uint8_t*>(t1.i8p);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1398

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchCharReady)
    t0.i32 = t0.i32 - *CAST<int32_t*>(t2.i8p + 16);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1400
    t0.clearHighWord();
    if (t0.u32 >= *CAST<uint32_t*>(t2.i8p + 12))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1401
        goto _offlineasm_opSwitchCharFallThrough;
    t2.i = *CAST<intptr_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1402
    t1.i = *CAST<int32_t*>(t2.i8p + (t0.i << 2) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1403
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1404
        goto _offlineasm_opSwitchCharFallThrough;
    rPC.i = rPC.i + t1.i;                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchCharFallThrough)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchOnRope)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_switch_char(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_func)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1418
        goto _offlineasm_opNewFuncUnchecked;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) != 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1420
        goto _offlineasm_opNewFuncDone;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNewFuncUnchecked)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_new_func(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNewFuncDone)
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_tear_off_activation)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) == 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1468
        goto _offlineasm_opTearOffActivationNotCreated;
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_tear_off_activation(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opTearOffActivationNotCreated)
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_tear_off_arguments)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = t0.i64 + int64_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1477
    if (*CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) == 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1478
        goto _offlineasm_opTearOffArgumentsNotCreated;
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_tear_off_arguments(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opTearOffArgumentsNotCreated)
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_ret)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_67_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_67_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_67_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_67_loadConstantOrVariable__done)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:409
    cfr.i = *CAST<intptr_t*>(cfr.i8p + 40);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:410
    rRetVPC.i = t2.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:212
    goto doReturnHelper;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:412

OFFLINE_ASM_OPCODE_LABEL(op_ret_object_or_this)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_68_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_68_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_68_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_68_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1497
        goto _offlineasm_opRetObjectOrThisNotObject;
    t2.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1498
    if (*CAST<uint8_t*>(t2.i8p + 92) < uint8_t(0x11))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1499
        goto _offlineasm_opRetObjectOrThisNotObject;
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:409
    cfr.i = *CAST<intptr_t*>(cfr.i8p + 40);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:410
    rRetVPC.i = t2.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:212
    goto doReturnHelper;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:412

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opRetObjectOrThisNotObject)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_69_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_69_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_69_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_69_loadConstantOrVariable__done)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:409
    cfr.i = *CAST<intptr_t*>(cfr.i8p + 40);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:410
    rRetVPC.i = t2.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:212
    goto doReturnHelper;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:412

OFFLINE_ASM_OPCODE_LABEL(op_to_primitive)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_70_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_70_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_70_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_70_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1513
        goto _offlineasm_opToPrimitiveIsImm;
    t2.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1514
    if (*CAST<int8_t*>(t2.i8p + 92) != int8_t(0x5))          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1515
        goto _offlineasm_opToPrimitiveSlowCase;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToPrimitiveIsImm)
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1517
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToPrimitiveSlowCase)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_to_primitive(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_next_pname)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_assertNotConstant__70_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_assertNotConstant__70_assert__ok)
    if (t2.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_assertNotConstant__71_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_assertNotConstant__71_assert__ok)
    t0.u = *CAST<uint32_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1531
    if (t0.i32 == *CAST<int32_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0))) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1532
        goto _offlineasm_opNextPnameEnd;
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_assertNotConstant__72_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_assertNotConstant__72_assert__ok)
    t2.i = *CAST<intptr_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1535
    t3.i = *CAST<intptr_t*>(t2.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1536
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1537
    t0.i32 = t0.i32 + int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1538
    t0.clearHighWord();
    *CAST<int32_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i32; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1539
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t3.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1541
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t3.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_assertNotConstant__73_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_assertNotConstant__73_assert__ok)
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1544
    t1.i = *CAST<intptr_t*>(t3.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1545
    if (t1.i != *CAST<intptr_t*>(t2.i8p + 8))                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1546
        goto _offlineasm_opNextPnameSlow;
    t0.i = *CAST<intptr_t*>(t2.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1547
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1548
    if (*CAST<intptr_t*>(t0.i8p) == 0)                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1549
        goto _offlineasm_opNextPnameTarget;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNextPnameCheckPrototypeLoop)
    if (*CAST<int64_t*>(t1.i8p + 16) == int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1551
        goto _offlineasm_opNextPnameSlow;
    t2.i64 = *CAST<int64_t*>(t1.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1552
    t1.i = *CAST<intptr_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1553
    if (t1.i != *CAST<intptr_t*>(t0.i8p))                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1554
        goto _offlineasm_opNextPnameSlow;
    t0.i = t0.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1555
    if (*CAST<intptr_t*>(t0.i8p) != 0)                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1556
        goto _offlineasm_opNextPnameCheckPrototypeLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNextPnameTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNextPnameEnd)
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNextPnameSlow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_next_pname(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_catch)
    cfr.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1575
    rBasePC.i = *CAST<intptr_t*>(cfr.i8p + 8);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1576
    rBasePC.i = *CAST<intptr_t*>(rBasePC.i8p + 104);         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1577
    t3.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1578
    t3.i = *CAST<intptr_t*>(t3.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1579
    rPC.i = *CAST<intptr_t*>(t3.i8p + 46560);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1580
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1581
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1582
    t0.i64 = *CAST<int64_t*>(t3.i8p + 46760);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1583
    *CAST<int64_t*>(t3.i8p + 46760) = int64_t(0x0);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1584
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1586
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_end)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:277
        goto _offlineasm_assertNotConstant__74_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_assertNotConstant__74_assert__ok)
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1596
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:409
    cfr.i = *CAST<intptr_t*>(cfr.i8p + 40);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:410
    rRetVPC.i = t2.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:212
    goto doReturnHelper;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:412

OFFLINE_ASM_GLUE_LABEL(llint_throw_from_slow_path_trampoline)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_handle_exception(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1606
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1607
    cfr.i = *CAST<intptr_t*>(t1.i8p + 39912);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1608
    t0.i = *CAST<intptr_t*>(t1.i8p + 46544);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1609
    opcode = *CAST<Opcode*>(t1.i8p + 46552);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1610
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_throw_during_call_trampoline)
    t2.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1615

OFFLINE_ASM_OPCODE_LABEL(op_resolve_scope)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i32 != int32_t(0x0))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1724
        goto _offlineasm_rGlobalVar;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1683
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1684
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1686
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rGlobalVar)
    if (t0.i32 != int32_t(0x1))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1729
        goto _offlineasm_rClosureVar;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1683
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1684
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1686
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rClosureVar)
    if (t0.i32 != int32_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1734
        goto _offlineasm_rGlobalPropertyWithVarInjectionChecks;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1697
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<int8_t*>(t0.i8p + 125) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1699
        goto _offlineasm_resolveScope__resolveScopeAfterActivationCheck;
    t1.i = *CAST<int32_t*>(t0.i8p + 120);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1700
    if (*CAST<intptr_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) == 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1701
        goto _offlineasm_resolveScope__resolveScopeAfterActivationCheck;
    t2.i32 = t2.i32 + int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1702
    t2.clearHighWord();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_resolveScope__resolveScopeAfterActivationCheck)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1705
    if (t2.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1706
        goto _offlineasm_resolveScope__resolveScopeLoopEnd;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_resolveScope__resolveScopeLoop)
    t0.i = *CAST<intptr_t*>(t0.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1709
    t2.i32 = t2.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1710
    t2.clearHighWord();
    if (t2.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1711
        goto _offlineasm_resolveScope__resolveScopeLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_resolveScope__resolveScopeLoopEnd)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1715
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rGlobalPropertyWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x3))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1739
        goto _offlineasm_rGlobalVarWithVarInjectionChecks;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1690
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1691
    t0.i = *CAST<intptr_t*>(t0.i8p + 1080);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1692
    if (*CAST<int8_t*>(t0.i8p + 41) != 0)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1693
        goto _offlineasm_rDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1683
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1684
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1686
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rGlobalVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x4))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1745
        goto _offlineasm_rClosureVarWithVarInjectionChecks;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1690
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1691
    t0.i = *CAST<intptr_t*>(t0.i8p + 1080);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1692
    if (*CAST<int8_t*>(t0.i8p + 41) != 0)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1693
        goto _offlineasm_rDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1683
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1684
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1686
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rClosureVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x5))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1751
        goto _offlineasm_rDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1690
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1691
    t0.i = *CAST<intptr_t*>(t0.i8p + 1080);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1692
    if (*CAST<int8_t*>(t0.i8p + 41) != 0)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1693
        goto _offlineasm_rDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1697
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (*CAST<int8_t*>(t0.i8p + 125) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1699
        goto _offlineasm_74_resolveScope__resolveScopeAfterActivationCheck;
    t1.i = *CAST<int32_t*>(t0.i8p + 120);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1700
    if (*CAST<intptr_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) == 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1701
        goto _offlineasm_74_resolveScope__resolveScopeAfterActivationCheck;
    t2.i32 = t2.i32 + int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1702
    t2.clearHighWord();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_74_resolveScope__resolveScopeAfterActivationCheck)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1705
    if (t2.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1706
        goto _offlineasm_74_resolveScope__resolveScopeLoopEnd;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_74_resolveScope__resolveScopeLoop)
    t0.i = *CAST<intptr_t*>(t0.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1709
    t2.i32 = t2.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1710
    t2.clearHighWord();
    if (t2.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1711
        goto _offlineasm_74_resolveScope__resolveScopeLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_74_resolveScope__resolveScopeLoopEnd)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1715
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rDynamic)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_resolve_scope(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_from_scope)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i32 = t0.i32 & int32_t(0xffff);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1796
    t0.clearHighWord();
    if (t0.i32 != int32_t(0x0))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1799
        goto _offlineasm_gGlobalVar;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1763
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1765
        goto _offlineasm_gDynamic;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:825
        goto _offlineasm_getProperty__74_loadPropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:826
    t1.i32 = -t1.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:827
    t1.clearHighWord();
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:828
    goto _offlineasm_getProperty__74_loadPropertyAtVariableOffset__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:829

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getProperty__74_loadPropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:831

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getProperty__74_loadPropertyAtVariableOffset__ready)
    t2.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x310)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:833
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1773
    rPC.i = rPC.i + intptr_t(0x8);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gGlobalVar)
    if (t0.i32 != int32_t(0x1))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1805
        goto _offlineasm_gClosureVar;
    t0.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    t0.i64 = *CAST<int64_t*>(t0.i8p);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1778
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1781
    rPC.i = rPC.i + intptr_t(0x8);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gClosureVar)
    if (t0.i32 != int32_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1810
        goto _offlineasm_gGlobalPropertyWithVarInjectionChecks;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:149
    t0.i = *CAST<intptr_t*>(t0.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1785
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1787
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1790
    rPC.i = rPC.i + intptr_t(0x8);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gGlobalPropertyWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x3))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1816
        goto _offlineasm_gGlobalVarWithVarInjectionChecks;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1763
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1765
        goto _offlineasm_gDynamic;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:825
        goto _offlineasm_getProperty__75_loadPropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:826
    t1.i32 = -t1.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:827
    t1.clearHighWord();
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:828
    goto _offlineasm_getProperty__75_loadPropertyAtVariableOffset__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:829

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getProperty__75_loadPropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:831

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getProperty__75_loadPropertyAtVariableOffset__ready)
    t2.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x310)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:833
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1773
    rPC.i = rPC.i + intptr_t(0x8);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gGlobalVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x4))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1822
        goto _offlineasm_gClosureVarWithVarInjectionChecks;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1690
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1691
    t0.i = *CAST<intptr_t*>(t0.i8p + 1080);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1692
    if (*CAST<int8_t*>(t0.i8p + 41) != 0)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1693
        goto _offlineasm_gDynamic;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:149
    t0.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    t0.i64 = *CAST<int64_t*>(t0.i8p);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1778
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1781
    rPC.i = rPC.i + intptr_t(0x8);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gClosureVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x5))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1829
        goto _offlineasm_gDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1690
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1691
    t0.i = *CAST<intptr_t*>(t0.i8p + 1080);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1692
    if (*CAST<int8_t*>(t0.i8p + 41) != 0)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1693
        goto _offlineasm_gDynamic;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:149
    t0.i = *CAST<intptr_t*>(t0.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1785
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1787
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1790
    rPC.i = rPC.i + intptr_t(0x8);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gDynamic)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_from_scope(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x8);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_to_scope)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i32 = t0.i32 & int32_t(0xffff);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1866
    t0.clearHighWord();
    if (t0.i32 != int32_t(0x0))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1869
        goto _offlineasm_pGlobalVar;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1763
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1765
        goto _offlineasm_pDynamic;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putProperty__75_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putProperty__75_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__75_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__75_loadConstantOrVariable__done)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:838
        goto _offlineasm_putProperty__storePropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:839
    t1.i32 = -t1.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:840
    t1.clearHighWord();
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:841
    goto _offlineasm_putProperty__storePropertyAtVariableOffset__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:842

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__storePropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:844

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__storePropertyAtVariableOffset__ready)
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x310)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:846
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pGlobalVar)
    if (t0.i32 != int32_t(0x1))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1875
        goto _offlineasm_pClosureVar;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putGlobalVar__76_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putGlobalVar__76_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putGlobalVar__76_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putGlobalVar__76_loadConstantOrVariable__done)
    t0.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    *CAST<int64_t*>(t0.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1851
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pClosureVar)
    if (t0.i32 != int32_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1880
        goto _offlineasm_pGlobalPropertyWithVarInjectionChecks;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:149
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putClosureVar__77_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putClosureVar__77_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putClosureVar__77_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putClosureVar__77_loadConstantOrVariable__done)
    t0.i = *CAST<intptr_t*>(t0.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1857
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1859
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pGlobalPropertyWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x3))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1886
        goto _offlineasm_pGlobalVarWithVarInjectionChecks;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1763
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    if (*CAST<intptr_t*>(t0.i8p) != t1.i)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1765
        goto _offlineasm_pDynamic;
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putProperty__78_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putProperty__78_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__78_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__78_loadConstantOrVariable__done)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:838
        goto _offlineasm_putProperty__78_storePropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:839
    t1.i32 = -t1.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:840
    t1.clearHighWord();
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:841
    goto _offlineasm_putProperty__78_storePropertyAtVariableOffset__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:842

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__78_storePropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:844

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__78_storePropertyAtVariableOffset__ready)
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x310)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:846
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pGlobalVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x4))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1892
        goto _offlineasm_pClosureVarWithVarInjectionChecks;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1690
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1691
    t0.i = *CAST<intptr_t*>(t0.i8p + 1080);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1692
    if (*CAST<int8_t*>(t0.i8p + 41) != 0)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1693
        goto _offlineasm_pDynamic;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putGlobalVar__79_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putGlobalVar__79_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putGlobalVar__79_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putGlobalVar__79_loadConstantOrVariable__done)
    t0.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    *CAST<int64_t*>(t0.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1851
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pClosureVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x5))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1898
        goto _offlineasm_pDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1690
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1691
    t0.i = *CAST<intptr_t*>(t0.i8p + 1080);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1692
    if (*CAST<int8_t*>(t0.i8p + 41) != 0)                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1693
        goto _offlineasm_pDynamic;
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:149
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_putClosureVar__80_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_putClosureVar__80_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putClosureVar__80_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t2.i = *CAST<intptr_t*>(t2.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putClosureVar__80_loadConstantOrVariable__done)
    t0.i = *CAST<intptr_t*>(t0.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1857
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1859
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pDynamic)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_put_to_scope(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x7);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_array)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_new_array(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_array_with_size)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_new_array_with_size(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_array_buffer)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_new_array_buffer(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_regexp)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_new_regexp(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_less)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_less(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_lesseq)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_lesseq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_greater)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_greater(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_greatereq)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_greatereq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_mod)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_mod(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_typeof)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_typeof(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_object)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_is_object(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_function)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_is_function(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_in)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_in(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_del_by_id)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_del_by_id(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_del_by_val)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_del_by_val(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_index)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_put_by_index(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_getter_setter)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_put_getter_setter(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jtrue)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_jumpTrueOrFalse__81_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_jumpTrueOrFalse__81_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__81_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__81_loadConstantOrVariable__done)
    t0.i64 = t0.i64 ^ int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1236
    if ((t0.i64 & int64_t(-1)) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1237
        goto _offlineasm_jumpTrueOrFalse__slow;
    if (t0.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:583
        goto _offlineasm_jumpTrueOrFalse__target;
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__target)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jtrue(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jfalse)
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_jumpTrueOrFalse__82_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_jumpTrueOrFalse__82_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__82_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__82_loadConstantOrVariable__done)
    t0.i64 = t0.i64 ^ int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1236
    if ((t0.i64 & int64_t(-1)) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1237
        goto _offlineasm_82_jumpTrueOrFalse__slow;
    if (t0.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:590
        goto _offlineasm_82_jumpTrueOrFalse__target;
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_82_jumpTrueOrFalse__target)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_82_jumpTrueOrFalse__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jfalse(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jless)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__83_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__83_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__83_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__83_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__84_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__84_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__84_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__84_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1312
        goto _offlineasm_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1313
        goto _offlineasm_compare__op2NotInt;
    if (t0.i32 < t1.i32)                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:597
        goto _offlineasm_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1318
        goto _offlineasm_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1319
        goto _offlineasm_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1320
    goto _offlineasm_compare__op1NotIntReady;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1321

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1323
        goto _offlineasm_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1324
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1325

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1327
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1328
    if (d0.d < d1.d)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:598
        goto _offlineasm_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1333
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1334
        goto _offlineasm_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1335
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1336
    if (d0.d < d1.d)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:598
        goto _offlineasm_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__jumpTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jless(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jnless)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__85_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__85_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__85_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__85_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__86_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__86_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__86_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__86_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1312
        goto _offlineasm_86_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1313
        goto _offlineasm_86_compare__op2NotInt;
    if (t0.i32 >= t1.i32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:605
        goto _offlineasm_86_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_86_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1318
        goto _offlineasm_86_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1319
        goto _offlineasm_86_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1320
    goto _offlineasm_86_compare__op1NotIntReady;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1321

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_86_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1323
        goto _offlineasm_86_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1324
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1325

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_86_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1327
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1328
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:606
        || (d0.d >= d1.d))
        goto _offlineasm_86_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_86_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1333
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1334
        goto _offlineasm_86_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1335
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1336
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:606
        || (d0.d >= d1.d))
        goto _offlineasm_86_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_86_compare__jumpTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_86_compare__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jnless(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jgreater)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__87_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__87_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__87_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__87_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__88_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__88_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__88_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__88_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1312
        goto _offlineasm_88_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1313
        goto _offlineasm_88_compare__op2NotInt;
    if (t0.i32 > t1.i32)                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:613
        goto _offlineasm_88_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1318
        goto _offlineasm_88_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1319
        goto _offlineasm_88_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1320
    goto _offlineasm_88_compare__op1NotIntReady;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1321

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1323
        goto _offlineasm_88_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1324
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1325

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1327
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1328
    if (d0.d > d1.d)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:614
        goto _offlineasm_88_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1333
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1334
        goto _offlineasm_88_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1335
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1336
    if (d0.d > d1.d)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:614
        goto _offlineasm_88_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_compare__jumpTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_compare__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jgreater(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jngreater)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__89_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__89_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__89_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__89_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__90_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__90_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__90_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__90_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1312
        goto _offlineasm_90_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1313
        goto _offlineasm_90_compare__op2NotInt;
    if (t0.i32 <= t1.i32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:621
        goto _offlineasm_90_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_90_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1318
        goto _offlineasm_90_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1319
        goto _offlineasm_90_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1320
    goto _offlineasm_90_compare__op1NotIntReady;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1321

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_90_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1323
        goto _offlineasm_90_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1324
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1325

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_90_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1327
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1328
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:622
        || (d0.d <= d1.d))
        goto _offlineasm_90_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_90_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1333
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1334
        goto _offlineasm_90_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1335
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1336
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:622
        || (d0.d <= d1.d))
        goto _offlineasm_90_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_90_compare__jumpTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_90_compare__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jngreater(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jlesseq)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__91_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__91_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__91_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__91_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__92_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__92_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__92_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__92_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1312
        goto _offlineasm_92_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1313
        goto _offlineasm_92_compare__op2NotInt;
    if (t0.i32 <= t1.i32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:629
        goto _offlineasm_92_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_92_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1318
        goto _offlineasm_92_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1319
        goto _offlineasm_92_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1320
    goto _offlineasm_92_compare__op1NotIntReady;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1321

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_92_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1323
        goto _offlineasm_92_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1324
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1325

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_92_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1327
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1328
    if (d0.d <= d1.d)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:630
        goto _offlineasm_92_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_92_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1333
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1334
        goto _offlineasm_92_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1335
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1336
    if (d0.d <= d1.d)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:630
        goto _offlineasm_92_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_92_compare__jumpTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_92_compare__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jlesseq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jnlesseq)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__93_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__93_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__93_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__93_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__94_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__94_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__94_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__94_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1312
        goto _offlineasm_94_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1313
        goto _offlineasm_94_compare__op2NotInt;
    if (t0.i32 > t1.i32)                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:637
        goto _offlineasm_94_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_94_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1318
        goto _offlineasm_94_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1319
        goto _offlineasm_94_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1320
    goto _offlineasm_94_compare__op1NotIntReady;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1321

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_94_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1323
        goto _offlineasm_94_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1324
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1325

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_94_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1327
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1328
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
        || (d0.d > d1.d))
        goto _offlineasm_94_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_94_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1333
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1334
        goto _offlineasm_94_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1335
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1336
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
        || (d0.d > d1.d))
        goto _offlineasm_94_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_94_compare__jumpTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_94_compare__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jnlesseq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jgreatereq)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__95_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__95_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__95_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__95_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__96_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__96_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__96_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__96_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1312
        goto _offlineasm_96_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1313
        goto _offlineasm_96_compare__op2NotInt;
    if (t0.i32 >= t1.i32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:645
        goto _offlineasm_96_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_96_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1318
        goto _offlineasm_96_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1319
        goto _offlineasm_96_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1320
    goto _offlineasm_96_compare__op1NotIntReady;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1321

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_96_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1323
        goto _offlineasm_96_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1324
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1325

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_96_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1327
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1328
    if (d0.d >= d1.d)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:646
        goto _offlineasm_96_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_96_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1333
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1334
        goto _offlineasm_96_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1335
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1336
    if (d0.d >= d1.d)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:646
        goto _offlineasm_96_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_96_compare__jumpTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_96_compare__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jgreatereq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jngreatereq)
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__97_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__97_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__97_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t0.i = *CAST<intptr_t*>(t0.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__97_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_compare__98_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_compare__98_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__98_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t1.i = *CAST<intptr_t*>(t1.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__98_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1312
        goto _offlineasm_98_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1313
        goto _offlineasm_98_compare__op2NotInt;
    if (t0.i32 < t1.i32)                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:653
        goto _offlineasm_98_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_98_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1318
        goto _offlineasm_98_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1319
        goto _offlineasm_98_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1320
    goto _offlineasm_98_compare__op1NotIntReady;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1321

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_98_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1323
        goto _offlineasm_98_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1324
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1325

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_98_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1327
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1328
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:654
        || (d0.d < d1.d))
        goto _offlineasm_98_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_98_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1333
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1334
        goto _offlineasm_98_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1335
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1336
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:654
        || (d0.d < d1.d))
        goto _offlineasm_98_compare__jumpTarget;
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_98_compare__jumpTarget)
    rPC.i32 = rPC.i32 + *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    rPC.clearHighWord();
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_98_compare__slow)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_jngreatereq(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_loop_hint)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:660
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:661
    t0.i = *CAST<uint8_t*>(t1.i8p + 39920);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:662
    if (t0.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
        goto _offlineasm_handleWatchdogTimer;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_afterWatchdogTimerCheck)
    rPC.i = rPC.i + intptr_t(0x1);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_handleWatchdogTimer)
    *CAST<int32_t*>(cfr.i8p + 52) = rPC.i32;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:122
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_handle_watchdog_timer(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:125
    if (t0.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:126
        goto _offlineasm_throwHandler;
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:127
    rPC.u = *CAST<uint32_t*>(cfr.i8p + 52);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:128
    goto _offlineasm_afterWatchdogTimerCheck;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_throwHandler)
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:671

OFFLINE_ASM_OPCODE_LABEL(op_switch_string)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_switch_string(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_func_exp)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_new_func_exp(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_call)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    t2.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1443
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_doCall__99_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_doCall__99_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__99_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t3.i = *CAST<intptr_t*>(t3.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__99_loadConstantOrVariable__done)
    if (t3.i64 != t2.i64)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1445
        goto _offlineasm_doCall__opCallSlow;
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i32 = t3.i32 << (intptr_t(0x3) & 0x1f);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1447
    t3.clearHighWord();
    t3.i = -t3.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1448
    t3.i = t3.i + cfr.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1449
    t0.i = *CAST<intptr_t*>(t2.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1450
    *CAST<int64_t*>(t3.i8p + 32) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1451
    *CAST<int64_t*>(t3.i8p + 24) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1452
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int32_t*>(cfr.i8p + 52) = rPC.i32;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1454
    *CAST<int64_t*>(t3.i8p + 40) = cfr.i64;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1455
    *CAST<int32_t*>(t3.i8p + 48) = t2.i32;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1456
    cfr.i = t3.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1457
    opcode = *CAST<Opcode*>(t1.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:230
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__opCallSlow)
    *CAST<int32_t*>(cfr.i8p + 52) = rPC.i32;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:114
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_call(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:117
    opcode = t0.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:242
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_construct)
    t0.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t1.i = *CAST<intptr_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:85
    t2.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1443
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:154
        goto _offlineasm_doCall__100_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:155
    goto _offlineasm_doCall__100_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:156

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__100_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:158
    t3.i = *CAST<intptr_t*>(t3.i8p + 264);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:159
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:160
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:161

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__100_loadConstantOrVariable__done)
    if (t3.i64 != t2.i64)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1445
        goto _offlineasm_100_doCall__opCallSlow;
    t3.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    t3.i32 = t3.i32 << (intptr_t(0x3) & 0x1f);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1447
    t3.clearHighWord();
    t3.i = -t3.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1448
    t3.i = t3.i + cfr.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1449
    t0.i = *CAST<intptr_t*>(t2.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1450
    *CAST<int64_t*>(t3.i8p + 32) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1451
    *CAST<int64_t*>(t3.i8p + 24) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1452
    t2.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int32_t*>(cfr.i8p + 52) = rPC.i32;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1454
    *CAST<int64_t*>(t3.i8p + 40) = cfr.i64;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1455
    *CAST<int32_t*>(t3.i8p + 48) = t2.i32;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1456
    cfr.i = t3.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1457
    opcode = *CAST<Opcode*>(t1.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:230
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_100_doCall__opCallSlow)
    *CAST<int32_t*>(cfr.i8p + 52) = rPC.i32;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:114
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_construct(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:117
    opcode = t0.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:242
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_call_varargs)
    *CAST<int32_t*>(cfr.i8p + 52) = rPC.i32;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:114
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_call_varargs(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:117
    opcode = t0.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:242
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_call_eval)
    *CAST<int32_t*>(cfr.i8p + 52) = rPC.i32;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:114
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_call_eval(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:117
    opcode = t0.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:242
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_generic_return_point)
    rPC.u = *CAST<uint32_t*>(cfr.i8p + 52);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    rBasePC.i = *CAST<intptr_t*>(cfr.i8p + 8);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    rBasePC.i = *CAST<intptr_t*>(rBasePC.i8p + 104);         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:81
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3) + intptr_t(0x0)) = t0.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    rPC.i = rPC.i + intptr_t(0x8);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_strcat)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = slow_path_strcat(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_pnames)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_get_pnames(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x0);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_push_with_scope)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_push_with_scope(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_pop_scope)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_pop_scope(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x1);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_push_name_scope)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_push_name_scope(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x4);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_throw)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_throw(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_throw_static_error)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_throw_static_error(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x3);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_profile_will_call)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_profile_will_call(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_profile_did_call)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_profile_did_call(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_debug)
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_slow_path_debug(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    rPC.i = rPC.i + intptr_t(0x2);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_native_call_trampoline)
    *CAST<intptr_t*>(cfr.i8p + 8) = intptr_t(0x0);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1619
    t0.i = *CAST<intptr_t*>(cfr.i8p + 40);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1642
    t1.i = *CAST<intptr_t*>(t0.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1643
    *CAST<intptr_t*>(cfr.i8p + 24) = t1.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1644
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
    t3.i = t3.i & intptr_t(-65536);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    t3.i = *CAST<intptr_t*>(t3.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    *CAST<intptr_t*>(t3.i8p + 39912) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1649
    t2.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1651
    t3.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    *CAST<intptr_t*>(cfr.i8p + 16) = t3.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1653
    t0.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1654
    t1.i = *CAST<intptr_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1655
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1656
    cfr.i = t2.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1657
    nativeFunc = *CAST<NativeFunction*>(t1.i8p + 64);        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1658
    functionReturnValue = JSValue::decode(nativeFunc(t0.execState));
    #if USE(JSVALUE32_64)
        t1.i = functionReturnValue.tag();
        t0.i = functionReturnValue.payload();
    #else // USE_JSVALUE64)
        t0.encodedJSValue = JSValue::encode(functionReturnValue);
    #endif // USE_JSVALUE64)
    rRetVPC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:212
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1661
    t3.i = t3.i & intptr_t(-65536);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1662
    t3.i = *CAST<intptr_t*>(t3.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1663
    if (*CAST<int64_t*>(t3.i8p + 46760) != 0)                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1668
        goto _offlineasm_nativeCallTrampoline__exception;
    goto doReturnHelper;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1669

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_nativeCallTrampoline__exception)
    t1.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    rPC.u = *CAST<uint32_t*>(cfr.i8p + 52);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1672
    rBasePC.i = *CAST<intptr_t*>(cfr.i8p + 8);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1673
    t0.i = *CAST<intptr_t*>(rBasePC.i8p + 96);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1674
    rBasePC.i = *CAST<intptr_t*>(rBasePC.i8p + 104);         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1675
    *CAST<intptr_t*>(t0.i8p + 39912) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1676
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_throw_from_native_call(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1678

OFFLINE_ASM_GLUE_LABEL(llint_native_construct_trampoline)
    *CAST<intptr_t*>(cfr.i8p + 8) = intptr_t(0x0);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1619
    t0.i = *CAST<intptr_t*>(cfr.i8p + 40);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1642
    t1.i = *CAST<intptr_t*>(t0.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1643
    *CAST<intptr_t*>(cfr.i8p + 24) = t1.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1644
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
    t3.i = t3.i & intptr_t(-65536);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    t3.i = *CAST<intptr_t*>(t3.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    *CAST<intptr_t*>(t3.i8p + 39912) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1649
    t2.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1651
    t3.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    *CAST<intptr_t*>(cfr.i8p + 16) = t3.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1653
    t0.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1654
    t1.i = *CAST<intptr_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1655
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1656
    cfr.i = t2.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1657
    nativeFunc = *CAST<NativeFunction*>(t1.i8p + 72);        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1658
    functionReturnValue = JSValue::decode(nativeFunc(t0.execState));
    #if USE(JSVALUE32_64)
        t1.i = functionReturnValue.tag();
        t0.i = functionReturnValue.payload();
    #else // USE_JSVALUE64)
        t0.encodedJSValue = JSValue::encode(functionReturnValue);
    #endif // USE_JSVALUE64)
    rRetVPC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:212
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1661
    t3.i = t3.i & intptr_t(-65536);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1662
    t3.i = *CAST<intptr_t*>(t3.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1663
    if (*CAST<int64_t*>(t3.i8p + 46760) != 0)                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1668
        goto _offlineasm_100_nativeCallTrampoline__exception;
    goto doReturnHelper;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1669

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_100_nativeCallTrampoline__exception)
    t1.i = rRetVPC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:199
    rPC.u = *CAST<uint32_t*>(cfr.i8p + 52);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1672
    rBasePC.i = *CAST<intptr_t*>(cfr.i8p + 8);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1673
    t0.i = *CAST<intptr_t*>(rBasePC.i8p + 96);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1674
    rBasePC.i = *CAST<intptr_t*>(rBasePC.i8p + 104);         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1675
    *CAST<intptr_t*>(t0.i8p + 39912) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1676
    rPC.i8p = rBasePC.i8p + (rPC.i << 3);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:82
    t3.i = rBasePC.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:60
        ExecState* exec = CAST<ExecState*>(cfr.vp);
        Instruction* pc = CAST<Instruction*>(rPC.vp);
        SlowPathReturnType result = llint_throw_from_native_call(exec, pc);
        decodeResult(result, t0.instruction, t1.execState);
    }
    rPC.i = t0.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:87
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:88
    rBasePC.i = t3.i;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:89
    rPC.i = rPC.i - rBasePC.i;                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:90
    rPC.i = rPC.i >> (intptr_t(0x3) & 0x1f);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1678

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_chain)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_custom_chain)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_custom_proto)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_custom_self)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_generic)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_getter_chain)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_getter_proto)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_getter_self)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_proto)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_self)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_get_string_length)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_generic)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_replace)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180

OFFLINE_ASM_OPCODE_LABEL(op_init_global_const_nop)
    rPC.i = rPC.i + intptr_t(0x5);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(rBasePC.i8p + (rPC.i << 3) + intptr_t(0x0)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_end)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:180
OFFLINE_ASM_END
#endif
