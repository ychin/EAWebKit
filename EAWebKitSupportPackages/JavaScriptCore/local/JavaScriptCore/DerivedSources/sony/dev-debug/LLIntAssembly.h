// offlineasm input hash: 94f23e4630190e03b831bc3a08fe5931eaaa1c02 514ef57e882689ba869b2c975b8707f047e71a0c 82fda4e596667d2e1f55992b19f40ba7a9e32fdf
#if !OFFLINE_ASM_X86 && !OFFLINE_ASM_X86_WIN && !OFFLINE_ASM_X86_64 && !OFFLINE_ASM_X86_64_WIN && !OFFLINE_ASM_ARM && !OFFLINE_ASM_ARMv7 && !OFFLINE_ASM_ARMv7_TRADITIONAL && !OFFLINE_ASM_ARM64 && !OFFLINE_ASM_MIPS && !OFFLINE_ASM_SH4 && OFFLINE_ASM_C_LOOP && !OFFLINE_ASM_ARMv7k && !OFFLINE_ASM_ARMv7s && OFFLINE_ASM_JSVALUE64 && !OFFLINE_ASM_BIG_ENDIAN && OFFLINE_ASM_ASSERT_ENABLED && !OFFLINE_ASM_EXECUTION_TRACING && OFFLINE_ASM_GGC
OFFLINE_ASM_BEGIN

OFFLINE_ASM_GLUE_LABEL(llint_vm_entry_to_javascript)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:434
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:435
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    sp.i = cfr.i - intptr_t(0x20);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    *CAST<intptr_t*>(sp.i8p) = t1.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:181
    pc.i = *CAST<intptr_t*>(t1.i8p + 10032);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:182
    *CAST<intptr_t*>(sp.i8p + 8) = pc.i;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:183
    pc.i = *CAST<intptr_t*>(t1.i8p + 10024);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:184
    *CAST<intptr_t*>(sp.i8p + 16) = pc.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:185
    *CAST<intptr_t*>(t1.i8p + 10024) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:186
    pc.u = *CAST<uint32_t*>(t2.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:188
    pc.i = intptr_t(0x5) + pc.i;                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:189
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:190
    t3.i = sp.i - pc.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:191
    if (t3.u >= *CAST<uintptr_t*>(t1.i8p + 16872))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:196
        goto _offlineasm_doVMEntry__stackHeightOK;
    pc.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:199
    pcBase.i = t1.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:200
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:201
        SlowPathReturnType result = llint_stack_check_at_vm_entry(t1, t3);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t0.i == intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:202
        goto _offlineasm_doVMEntry__stackCheckFailed;
    t0.i = pc.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:203
    t1.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:204
    goto _offlineasm_doVMEntry__stackHeightOK;               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:205

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doVMEntry__stackCheckFailed)
    t0.i = pc.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:208
    t1.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:209
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_throw_stack_overflow_error(t1, t2);
        decodeResult(result, t0.vp, t1.vp);
    }
    pc.i = cfr.i - intptr_t(0x20);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    t1.i = *CAST<intptr_t*>(pc.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:216
    pcBase.i = *CAST<intptr_t*>(pc.i8p + 8);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:217
    *CAST<intptr_t*>(t1.i8p + 10032) = pcBase.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:218
    pcBase.i = *CAST<intptr_t*>(pc.i8p + 16);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:219
    *CAST<intptr_t*>(t1.i8p + 10024) = pcBase.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:220
    sp.i = cfr.i - intptr_t(0x0);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:222
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:446
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:447
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:226
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doVMEntry__stackHeightOK)
    sp.i = t3.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:229
    t3.i = intptr_t(0x4);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:230

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doVMEntry__copyHeaderLoop)
    t3.i32 = t3.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:233
    t3.clearHighWord();
    pcBase.i64 = *CAST<int64_t*>(t2.i8p + (t3.i << 3));      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:234
    *CAST<int64_t*>(sp.i8p + (t3.i << 3) + intptr_t(0x10)) = pcBase.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:235
    if (t3.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:236
        goto _offlineasm_doVMEntry__copyHeaderLoop;
    pc.u = *CAST<uint32_t*>(t2.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:238
    pc.i32 = pc.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:239
    pc.clearHighWord();
    pcBase.u = *CAST<uint32_t*>(t2.i8p + 32);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:240
    pcBase.i32 = pcBase.i32 - int32_t(0x1);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:241
    pcBase.clearHighWord();
    if (pc.i32 == pcBase.i32)                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:243
        goto _offlineasm_doVMEntry__copyArgs;
    t3.i = intptr_t(0xa);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:244

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doVMEntry__fillExtraArgsLoop)
    pcBase.i32 = pcBase.i32 - int32_t(0x1);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:246
    pcBase.clearHighWord();
    *CAST<int64_t*>(sp.i8p + (pcBase.i << 3) + intptr_t(0x30)) = t3.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:247
    if (pc.i32 != pcBase.i32)                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:248
        goto _offlineasm_doVMEntry__fillExtraArgsLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doVMEntry__copyArgs)
    t3.i = *CAST<intptr_t*>(t2.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:251

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doVMEntry__copyArgsLoop)
    if (pc.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:254
        goto _offlineasm_doVMEntry__copyArgsDone;
    pc.i32 = pc.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:255
    pc.clearHighWord();
    pcBase.i64 = *CAST<int64_t*>(t3.i8p + (pc.i << 3));      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:256
    *CAST<int64_t*>(sp.i8p + (pc.i << 3) + intptr_t(0x30)) = pcBase.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:257
    goto _offlineasm_doVMEntry__copyArgsLoop;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:258

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doVMEntry__copyArgsDone)
    *CAST<intptr_t*>(t1.i8p + 10032) = sp.i;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:265
    tagTypeNumber.i = intptr_t(0xffff000000000000);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:268
    tagMask.i = intptr_t(0x2) + tagTypeNumber.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:269
    sp.i = sp.i + intptr_t(0x10);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:295
    lr.opcode = getOpcode(llint_cloop_did_return_from_js_1); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:297
    opcode = t0.opcode;
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_cloop_did_return_from_js_1)
    sp.i = sp.i - intptr_t(0x10);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:301
    pc.i = cfr.i - intptr_t(0x20);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    t1.i = *CAST<intptr_t*>(pc.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:279
    pcBase.i = *CAST<intptr_t*>(pc.i8p + 8);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:280
    *CAST<intptr_t*>(t1.i8p + 10032) = pcBase.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:281
    pcBase.i = *CAST<intptr_t*>(pc.i8p + 16);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:282
    *CAST<intptr_t*>(t1.i8p + 10024) = pcBase.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:283
    sp.i = cfr.i - intptr_t(0x0);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:285
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:446
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:447
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:290
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_vm_entry_to_native)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:434
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:435
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    sp.i = cfr.i - intptr_t(0x20);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    *CAST<intptr_t*>(sp.i8p) = t1.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:181
    pc.i = *CAST<intptr_t*>(t1.i8p + 10032);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:182
    *CAST<intptr_t*>(sp.i8p + 8) = pc.i;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:183
    pc.i = *CAST<intptr_t*>(t1.i8p + 10024);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:184
    *CAST<intptr_t*>(sp.i8p + 16) = pc.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:185
    *CAST<intptr_t*>(t1.i8p + 10024) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:186
    pc.u = *CAST<uint32_t*>(t2.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:188
    pc.i = intptr_t(0x5) + pc.i;                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:189
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:190
    t3.i = sp.i - pc.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:191
    if (t3.u >= *CAST<uintptr_t*>(t1.i8p + 16872))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:196
        goto _offlineasm_0_doVMEntry__stackHeightOK;
    pc.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:199
    pcBase.i = t1.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:200
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:201
        SlowPathReturnType result = llint_stack_check_at_vm_entry(t1, t3);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t0.i == intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:202
        goto _offlineasm_0_doVMEntry__stackCheckFailed;
    t0.i = pc.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:203
    t1.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:204
    goto _offlineasm_0_doVMEntry__stackHeightOK;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:205

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_doVMEntry__stackCheckFailed)
    t0.i = pc.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:208
    t1.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:209
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_throw_stack_overflow_error(t1, t2);
        decodeResult(result, t0.vp, t1.vp);
    }
    pc.i = cfr.i - intptr_t(0x20);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    t1.i = *CAST<intptr_t*>(pc.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:216
    pcBase.i = *CAST<intptr_t*>(pc.i8p + 8);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:217
    *CAST<intptr_t*>(t1.i8p + 10032) = pcBase.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:218
    pcBase.i = *CAST<intptr_t*>(pc.i8p + 16);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:219
    *CAST<intptr_t*>(t1.i8p + 10024) = pcBase.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:220
    sp.i = cfr.i - intptr_t(0x0);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:222
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:446
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:447
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:226
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_doVMEntry__stackHeightOK)
    sp.i = t3.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:229
    t3.i = intptr_t(0x4);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:230

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_doVMEntry__copyHeaderLoop)
    t3.i32 = t3.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:233
    t3.clearHighWord();
    pcBase.i64 = *CAST<int64_t*>(t2.i8p + (t3.i << 3));      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:234
    *CAST<int64_t*>(sp.i8p + (t3.i << 3) + intptr_t(0x10)) = pcBase.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:235
    if (t3.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:236
        goto _offlineasm_0_doVMEntry__copyHeaderLoop;
    pc.u = *CAST<uint32_t*>(t2.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:238
    pc.i32 = pc.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:239
    pc.clearHighWord();
    pcBase.u = *CAST<uint32_t*>(t2.i8p + 32);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:240
    pcBase.i32 = pcBase.i32 - int32_t(0x1);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:241
    pcBase.clearHighWord();
    if (pc.i32 == pcBase.i32)                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:243
        goto _offlineasm_0_doVMEntry__copyArgs;
    t3.i = intptr_t(0xa);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:244

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_doVMEntry__fillExtraArgsLoop)
    pcBase.i32 = pcBase.i32 - int32_t(0x1);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:246
    pcBase.clearHighWord();
    *CAST<int64_t*>(sp.i8p + (pcBase.i << 3) + intptr_t(0x30)) = t3.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:247
    if (pc.i32 != pcBase.i32)                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:248
        goto _offlineasm_0_doVMEntry__fillExtraArgsLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_doVMEntry__copyArgs)
    t3.i = *CAST<intptr_t*>(t2.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:251

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_doVMEntry__copyArgsLoop)
    if (pc.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:254
        goto _offlineasm_0_doVMEntry__copyArgsDone;
    pc.i32 = pc.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:255
    pc.clearHighWord();
    pcBase.i64 = *CAST<int64_t*>(t3.i8p + (pc.i << 3));      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:256
    *CAST<int64_t*>(sp.i8p + (pc.i << 3) + intptr_t(0x30)) = pcBase.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:257
    goto _offlineasm_0_doVMEntry__copyArgsLoop;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:258

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_doVMEntry__copyArgsDone)
    *CAST<intptr_t*>(t1.i8p + 10032) = sp.i;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:265
    tagTypeNumber.i = intptr_t(0xffff000000000000);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:268
    tagMask.i = intptr_t(0x2) + tagTypeNumber.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:269
    t3.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:306
    *CAST<intptr_t*>(sp.i8p) = cfr.i;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:307
    t0.i = sp.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:313
    *CAST<intptr_t*>(sp.i8p + 8) = lr.i;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:316
    nativeFunc = t3.nativeFunc;                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:317
    functionReturnValue = JSValue::decode(nativeFunc(t0.execState));
    #if USE(JSVALUE32_64)
        t1.i = functionReturnValue.tag();
        t0.i = functionReturnValue.payload();
    #else // USE_JSVALUE64)
        t0.encodedJSValue = JSValue::encode(functionReturnValue);
    #endif // USE_JSVALUE64)
    pc.i = cfr.i - intptr_t(0x20);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    t1.i = *CAST<intptr_t*>(pc.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:279
    pcBase.i = *CAST<intptr_t*>(pc.i8p + 8);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:280
    *CAST<intptr_t*>(t1.i8p + 10032) = pcBase.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:281
    pcBase.i = *CAST<intptr_t*>(pc.i8p + 16);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:282
    *CAST<intptr_t*>(t1.i8p + 10024) = pcBase.i;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:283
    sp.i = cfr.i - intptr_t(0x0);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:285
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:446
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:447
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:290
    DISPATCH_OPCODE();

OFFLINE_ASM_GLOBAL_LABEL(llint_entry)
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

OFFLINE_ASM_GLUE_LABEL(llint_program_prologue)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:382
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:383
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568
    pcBase.i = *CAST<intptr_t*>(t1.i8p + 104);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:623
    t5.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:624
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    t2.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    if (*CAST<uintptr_t*>(t2.i8p + 16872) <= t0.u)           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
        goto _offlineasm_prologue__stackHeightOK;
    sp.i = sp.i - intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_stack_check(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    if (t1.i == intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
        goto _offlineasm_prologue__stackHeightOKGetCodeBlock;
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_prologue__stackHeightOKGetCodeBlock)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_prologue__stackHeightOK)
    sp.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_eval_prologue)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:382
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:383
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568
    pcBase.i = *CAST<intptr_t*>(t1.i8p + 104);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:623
    t5.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:624
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    t2.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    if (*CAST<uintptr_t*>(t2.i8p + 16872) <= t0.u)           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
        goto _offlineasm_0_prologue__stackHeightOK;
    sp.i = sp.i - intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_stack_check(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    if (t1.i == intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
        goto _offlineasm_0_prologue__stackHeightOKGetCodeBlock;
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_prologue__stackHeightOKGetCodeBlock)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:568
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_0_prologue__stackHeightOK)
    sp.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_function_for_call_prologue)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:382
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:383
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:549
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    t1.i = *CAST<intptr_t*>(t1.i8p + 136);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554
    *CAST<intptr_t*>(cfr.i8p + 16) = t1.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:572
    pcBase.i = *CAST<intptr_t*>(t1.i8p + 104);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:623
    t5.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:624
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    t2.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    if (*CAST<uintptr_t*>(t2.i8p + 16872) <= t0.u)           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
        goto _offlineasm_1_prologue__stackHeightOK;
    sp.i = sp.i - intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_stack_check(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    if (t1.i == intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
        goto _offlineasm_1_prologue__stackHeightOKGetCodeBlock;
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_1_prologue__stackHeightOKGetCodeBlock)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:549
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    t1.i = *CAST<intptr_t*>(t1.i8p + 136);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_1_prologue__stackHeightOK)
    sp.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    t0.u = *CAST<uint32_t*>(t1.i8p + 80);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:664
    if (t0.i >= intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:665
        goto _offlineasm_functionInitialization__assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionInitialization__assert__ok)
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:666
        goto _offlineasm_functionInitialization__argumentProfileDone;
    t3.i = *CAST<intptr_t*>(t1.i8p + 224);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:667
    t2.i = intptr_t(0x20) * t0.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:668
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669
    t3.i = t3.i + t2.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:670

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionInitialization__argumentProfileLoop)
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 0) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:673
    t3.i = t3.i - intptr_t(0x20);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:674
    *CAST<int64_t*>(t3.i8p + 16) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:675
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:683
        intptr_t temp = t0.i + intptr_t(-8);
        t0.i = temp;
        if (temp != 0)
            goto  _offlineasm_functionInitialization__argumentProfileLoop;
    }

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionInitialization__argumentProfileDone)
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_function_for_construct_prologue)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:382
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:383
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:559
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    t1.i = *CAST<intptr_t*>(t1.i8p + 144);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564
    *CAST<intptr_t*>(cfr.i8p + 16) = t1.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:572
    pcBase.i = *CAST<intptr_t*>(t1.i8p + 104);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:623
    t5.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:624
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    t2.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    if (*CAST<uintptr_t*>(t2.i8p + 16872) <= t0.u)           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
        goto _offlineasm_2_prologue__stackHeightOK;
    sp.i = sp.i - intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_stack_check(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    if (t1.i == intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
        goto _offlineasm_2_prologue__stackHeightOKGetCodeBlock;
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_2_prologue__stackHeightOKGetCodeBlock)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:559
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    t1.i = *CAST<intptr_t*>(t1.i8p + 144);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_2_prologue__stackHeightOK)
    sp.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    t0.u = *CAST<uint32_t*>(t1.i8p + 80);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
    t0.i = t0.i + intptr_t(-1);                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:664
    if (t0.i >= intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:665
        goto _offlineasm_functionInitialization__2_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionInitialization__2_assert__ok)
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:666
        goto _offlineasm_2_functionInitialization__argumentProfileDone;
    t3.i = *CAST<intptr_t*>(t1.i8p + 224);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:667
    t2.i = intptr_t(0x20) * t0.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:668
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669
    t3.i = t3.i + t2.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:670

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_2_functionInitialization__argumentProfileLoop)
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 0) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:673
    t3.i = t3.i - intptr_t(0x20);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:674
    *CAST<int64_t*>(t3.i8p + 48) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:675
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:683
        intptr_t temp = t0.i + intptr_t(-8);
        t0.i = temp;
        if (temp != 0)
            goto  _offlineasm_2_functionInitialization__argumentProfileLoop;
    }

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_2_functionInitialization__argumentProfileDone)
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_function_for_call_arity_check)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:382
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:383
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:549
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    t1.i = *CAST<intptr_t*>(t1.i8p + 136);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554
    *CAST<intptr_t*>(cfr.i8p + 16) = t1.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:572
    pcBase.i = *CAST<intptr_t*>(t1.i8p + 104);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:623
    t5.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:624
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    t2.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    if (*CAST<uintptr_t*>(t2.i8p + 16872) <= t0.u)           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
        goto _offlineasm_3_prologue__stackHeightOK;
    sp.i = sp.i - intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_stack_check(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    if (t1.i == intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
        goto _offlineasm_3_prologue__stackHeightOKGetCodeBlock;
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_3_prologue__stackHeightOKGetCodeBlock)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:549
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:553
    t1.i = *CAST<intptr_t*>(t1.i8p + 136);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:554
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_3_prologue__stackHeightOK)
    sp.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    t0.u = *CAST<uint32_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:522
    if (t0.u32 >= *CAST<uint32_t*>(t1.i8p + 80))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:523
        goto _offlineasm_functionForCallBegin;
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_call_arityCheck(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t0.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:526
        goto _offlineasm_functionArityCheck__noError;
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:527
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:528

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__noError)
    t2.i = *CAST<intptr_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:532
    if (t2.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:533
        goto _offlineasm_functionArityCheck__proceedInline;
    t7.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:535
    t0.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:536
    CRASH(); // generic call instruction not supported by design! // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:537
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:539
    t0.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:540
    goto _offlineasm_functionArityCheck__continue;           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:542

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__proceedInline)
    t1.u = *CAST<uint32_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:545
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:546
        goto _offlineasm_functionArityCheck__continue;
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:548
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:549
    t1.i = t1.i << (intptr_t(0x1) & 0x1f);
    t1.i64 = -t1.i64;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:550
    t3.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:551
    t2.u = *CAST<uint32_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:552
    t2.i32 = t2.i32 + int32_t(0x5);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:553
    t2.clearHighWord();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__copyLoop)
    t0.i64 = *CAST<int64_t*>(t3.i8p);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:555
    *CAST<int64_t*>(t3.i8p + (t1.i << 3)) = t0.i64;          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:556
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:557
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:558
        int32_t temp = t2.i32 - int32_t(0x1);
        t2.i32 = temp;
        if (temp != 0)
            goto  _offlineasm_functionArityCheck__copyLoop;
    }
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:560
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:561
    t2.i = t1.i;
    t0.i = intptr_t(0xa);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:562

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__fillLoop)
    *CAST<int64_t*>(t3.i8p + (t1.i << 3)) = t0.i64;          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:564
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:565
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:566
        int32_t temp = t2.i32 + int32_t(0x1);
        t2.i32 = temp;
        if (temp != 0)
            goto  _offlineasm_functionArityCheck__fillLoop;
    }
    t1.i = t1.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:568
    cfr.i = cfr.i + t1.i;                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:569
    sp.i = sp.i + t1.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:570

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionArityCheck__continue)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:574
    pcBase.i = *CAST<intptr_t*>(t1.i8p + 104);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:575
    t5.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:576
    goto _offlineasm_functionForCallBegin;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:577

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionForCallBegin)
    t0.u = *CAST<uint32_t*>(t1.i8p + 80);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:664
    if (t0.i >= intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:665
        goto _offlineasm_functionInitialization__3_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionInitialization__3_assert__ok)
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:666
        goto _offlineasm_3_functionInitialization__argumentProfileDone;
    t3.i = *CAST<intptr_t*>(t1.i8p + 224);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:667
    t2.i = intptr_t(0x20) * t0.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:668
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669
    t3.i = t3.i + t2.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:670

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_3_functionInitialization__argumentProfileLoop)
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 0) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:673
    t3.i = t3.i - intptr_t(0x20);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:674
    *CAST<int64_t*>(t3.i8p + 16) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:675
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:683
        intptr_t temp = t0.i + intptr_t(-8);
        t0.i = temp;
        if (temp != 0)
            goto  _offlineasm_3_functionInitialization__argumentProfileLoop;
    }

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_3_functionInitialization__argumentProfileDone)
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_function_for_construct_arity_check)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:382
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:383
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:391
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:559
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    t1.i = *CAST<intptr_t*>(t1.i8p + 144);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564
    *CAST<intptr_t*>(cfr.i8p + 16) = t1.i;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:572
    pcBase.i = *CAST<intptr_t*>(t1.i8p + 104);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:623
    t5.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:624
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:631
    t2.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:632
    if (*CAST<uintptr_t*>(t2.i8p + 16872) <= t0.u)           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:633
        goto _offlineasm_4_prologue__stackHeightOK;
    sp.i = sp.i - intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:636
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_stack_check(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    if (t1.i == intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:638
        goto _offlineasm_4_prologue__stackHeightOKGetCodeBlock;
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:639
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_prologue__stackHeightOKGetCodeBlock)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:559
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:563
    t1.i = *CAST<intptr_t*>(t1.i8p + 144);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:564
    t0.u = *CAST<uint32_t*>(t1.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    t0.i = t0.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    t0.i = cfr.i - t0.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:647

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_prologue__stackHeightOK)
    sp.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:650
    t0.u = *CAST<uint32_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:522
    if (t0.u32 >= *CAST<uint32_t*>(t1.i8p + 80))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:523
        goto _offlineasm_functionForConstructBegin;
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_construct_arityCheck(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t0.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:526
        goto _offlineasm_4_functionArityCheck__noError;
    cfr.i = t1.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:527
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:528

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_functionArityCheck__noError)
    t2.i = *CAST<intptr_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:532
    if (t2.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:533
        goto _offlineasm_4_functionArityCheck__proceedInline;
    t7.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:535
    t0.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:536
    CRASH(); // generic call instruction not supported by design! // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:537
    t0.i = *CAST<intptr_t*>(cfr.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:539
    t0.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:540
    goto _offlineasm_4_functionArityCheck__continue;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:542

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_functionArityCheck__proceedInline)
    t1.u = *CAST<uint32_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:545
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:546
        goto _offlineasm_4_functionArityCheck__continue;
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:548
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:549
    t1.i = t1.i << (intptr_t(0x1) & 0x1f);
    t1.i64 = -t1.i64;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:550
    t3.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:551
    t2.u = *CAST<uint32_t*>(cfr.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:552
    t2.i32 = t2.i32 + int32_t(0x5);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:553
    t2.clearHighWord();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_functionArityCheck__copyLoop)
    t0.i64 = *CAST<int64_t*>(t3.i8p);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:555
    *CAST<int64_t*>(t3.i8p + (t1.i << 3)) = t0.i64;          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:556
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:557
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:558
        int32_t temp = t2.i32 - int32_t(0x1);
        t2.i32 = temp;
        if (temp != 0)
            goto  _offlineasm_4_functionArityCheck__copyLoop;
    }
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:560
    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:561
    t2.i = t1.i;
    t0.i = intptr_t(0xa);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:562

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_functionArityCheck__fillLoop)
    *CAST<int64_t*>(t3.i8p + (t1.i << 3)) = t0.i64;          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:564
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:565
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:566
        int32_t temp = t2.i32 + int32_t(0x1);
        t2.i32 = temp;
        if (temp != 0)
            goto  _offlineasm_4_functionArityCheck__fillLoop;
    }
    t1.i = t1.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:568
    cfr.i = cfr.i + t1.i;                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:569
    sp.i = sp.i + t1.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:570

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_functionArityCheck__continue)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:574
    pcBase.i = *CAST<intptr_t*>(t1.i8p + 104);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:575
    t5.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:576
    goto _offlineasm_functionForConstructBegin;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:577

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionForConstructBegin)
    t0.u = *CAST<uint32_t*>(t1.i8p + 80);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:663
    t0.i = t0.i + intptr_t(-1);                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:664
    if (t0.i >= intptr_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:665
        goto _offlineasm_functionInitialization__4_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_functionInitialization__4_assert__ok)
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:666
        goto _offlineasm_4_functionInitialization__argumentProfileDone;
    t3.i = *CAST<intptr_t*>(t1.i8p + 224);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:667
    t2.i = intptr_t(0x20) * t0.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:668
    t0.i = t0.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:669
    t3.i = t3.i + t2.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:670

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_functionInitialization__argumentProfileLoop)
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 0) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:673
    t3.i = t3.i - intptr_t(0x20);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:674
    *CAST<int64_t*>(t3.i8p + 48) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:675
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:683
        intptr_t temp = t0.i + intptr_t(-8);
        t0.i = temp;
        if (temp != 0)
            goto  _offlineasm_4_functionInitialization__argumentProfileLoop;
    }

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_functionInitialization__argumentProfileDone)
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(handleUncaughtException)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:330
    t3.i = t3.i & intptr_t(-16384);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:331
    t3.i = *CAST<intptr_t*>(t3.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:332
    cfr.i = *CAST<intptr_t*>(t3.i8p + 16632);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:333
    cfr.i = *CAST<intptr_t*>(cfr.i8p);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:335
    t2.i = cfr.i - intptr_t(0x20);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:452
    t3.i = *CAST<intptr_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:338
    t5.i = *CAST<intptr_t*>(t2.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:339
    *CAST<intptr_t*>(t3.i8p + 10032) = t5.i;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:340
    t5.i = *CAST<intptr_t*>(t2.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:341
    *CAST<intptr_t*>(t3.i8p + 10024) = t5.i;                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:342
    sp.i = cfr.i - intptr_t(0x0);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:344
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:446
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:447
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:348
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_enter)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:595
    t2.u = *CAST<uint32_t*>(t2.i8p + 60);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:596
    if (t2.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:597
        goto _offlineasm_opEnterDone;
    t0.i = intptr_t(0xa);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:598
    t2.i32 = -t2.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:599
    t2.clearHighWord();
    t2.i64 = t2.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:600

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opEnterLoop)
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:602
    t2.i64 = t2.i64 + int64_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:603
    if (t2.i64 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:604
        goto _offlineasm_opEnterLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opEnterDone)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_enter(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x1);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_scope)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:612
    t0.i = *CAST<intptr_t*>(t0.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:613
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:615
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_create_this)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i = *CAST<intptr_t*>(cfr.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:622
    pc.i = *CAST<intptr_t*>(t0.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:623
    if (pc.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:624
        goto _offlineasm_opCreateThisSlow;
    t1.i = *CAST<intptr_t*>(pc.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:625
    t2.i = *CAST<intptr_t*>(pc.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:626
    if (t1.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:627
        goto _offlineasm_opCreateThisSlow;
    pc.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (pc.i == intptr_t(0x1))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:629
        goto _offlineasm_hasSeenMultipleCallee;
    if (pc.i != t0.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:630
        goto _offlineasm_opCreateThisSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_hasSeenMultipleCallee)
    t0.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:693
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:694
        goto _offlineasm_opCreateThisSlow;
    t3.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:697
    *CAST<intptr_t*>(t1.i8p) = t3.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:698
    *CAST<intptr_t*>(t0.i8p + 8) = intptr_t(0x0);            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:701
    t3.i64 = *CAST<int64_t*>(t2.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:514
    *CAST<int64_t*>(t0.i8p) = t3.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:515
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:634
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opCreateThisSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_create_this(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_to_this)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:645
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:646
        goto _offlineasm_opToThisSlow;
    if (*CAST<int8_t*>(t0.i8p + 5) != int8_t(0x13))          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:647
        goto _offlineasm_opToThisSlow;
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t2.i = *CAST<intptr_t*>(t2.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t2.i = *CAST<intptr_t*>(t2.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t1.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t1.i = *CAST<intptr_t*>(t2.i8p + (t1.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t1.i != t2.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:650
        goto _offlineasm_opToThisSlow;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToThisSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_to_this(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_object)
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t1.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:661
    t2.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:662
    t0.i = *CAST<intptr_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:693
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:694
        goto _offlineasm_opNewObjectSlow;
    t3.i = *CAST<intptr_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:697
    *CAST<intptr_t*>(t1.i8p) = t3.i;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:698
    *CAST<intptr_t*>(t0.i8p + 8) = intptr_t(0x0);            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:701
    t3.i64 = *CAST<int64_t*>(t2.i8p + 8);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:514
    *CAST<int64_t*>(t0.i8p) = t3.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:515
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:665
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNewObjectSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_new_object(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_check_tdz)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_loadConstantOrVariable__done;           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariable__done)
    if (t1.i64 != int64_t(0x0))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:677
        goto _offlineasm_opNotTDZ;
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_throw_tdz_error(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNotTDZ)
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_mov)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_4_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_4_loadConstantOrVariable__done;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_4_loadConstantOrVariable__done)
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:689
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_not)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_5_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_5_loadConstantOrVariable__done;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_5_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_5_loadConstantOrVariable__done)
    t2.i64 = t2.i64 ^ int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:698
    if ((t2.i64 & int64_t(-2)) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:699
        goto _offlineasm_opNotSlow;
    t2.i64 = t2.i64 ^ int64_t(0x7);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:700
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:701
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNotSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_not(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_eq)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__6_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__6_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__6_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__6_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
        goto _offlineasm_equalityComparison__slow;
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__7_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__7_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__7_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__7_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
        goto _offlineasm_equalityComparison__slow;
    t0.i = (t0.u32 == t1.u32);                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:728
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:717
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:718
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_eq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_neq)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__8_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__8_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__8_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__8_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
        goto _offlineasm_9_equalityComparison__slow;
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__9_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_equalityComparison__loadConstantOrVariableInt32__9_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__9_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalityComparison__loadConstantOrVariableInt32__9_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
        goto _offlineasm_9_equalityComparison__slow;
    t0.i = (t0.u32 != t1.u32);                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:734
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:717
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:718
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_9_equalityComparison__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_neq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_eq_null)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:740
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:741
        goto _offlineasm_equalNullComparison__immediate;
    if ((*CAST<int8_t*>(t0.i8p + 6) & int8_t(0x1)) != 0)     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:742
        goto _offlineasm_equalNullComparison__masqueradesAsUndefined;
    t0.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:743
    goto _offlineasm_equalNullComparison__done;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:744

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNullComparison__masqueradesAsUndefined)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:747
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:748
    t0.i = (*CAST<uintptr_t*>(t2.i8p + 24) == t0.u);         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:749
    goto _offlineasm_equalNullComparison__done;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:750

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNullComparison__immediate)
    t0.i64 = t0.i64 & int64_t(-9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:752
    t0.i = (t0.u64 == uint64_t(0x2));                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:753

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNullComparison__done)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:761
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:762
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_neq_null)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:740
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:741
        goto _offlineasm_9_equalNullComparison__immediate;
    if ((*CAST<int8_t*>(t0.i8p + 6) & int8_t(0x1)) != 0)     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:742
        goto _offlineasm_9_equalNullComparison__masqueradesAsUndefined;
    t0.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:743
    goto _offlineasm_9_equalNullComparison__done;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:744

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_9_equalNullComparison__masqueradesAsUndefined)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:747
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:748
    t0.i = (*CAST<uintptr_t*>(t2.i8p + 24) == t0.u);         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:749
    goto _offlineasm_9_equalNullComparison__done;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:750

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_9_equalNullComparison__immediate)
    t0.i64 = t0.i64 & int64_t(-9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:752
    t0.i = (t0.u64 == uint64_t(0x2));                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:753

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_9_equalNullComparison__done)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = t0.i64 ^ int64_t(0x7);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:770
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:771
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_stricteq)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_strictEq__10_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_strictEq__10_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__10_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__10_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_strictEq__11_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_strictEq__11_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__11_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__11_loadConstantOrVariable__done)
    t2.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:781
    t2.i64 = t2.i64 | t1.i64;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:782
    if ((t2.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:783
        goto _offlineasm_strictEq__slow;
    if (t0.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:784
        goto _offlineasm_strictEq__leftOK;
    if ((t0.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:785
        goto _offlineasm_strictEq__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__leftOK)
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:787
        goto _offlineasm_strictEq__rightOK;
    if ((t1.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:788
        goto _offlineasm_strictEq__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__rightOK)
    t0.i = (t0.u64 == t1.u64);                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:803
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:792
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:793
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_stricteq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_nstricteq)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_strictEq__12_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_strictEq__12_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__12_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__12_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_strictEq__13_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_strictEq__13_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__13_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_strictEq__13_loadConstantOrVariable__done)
    t2.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:781
    t2.i64 = t2.i64 | t1.i64;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:782
    if ((t2.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:783
        goto _offlineasm_13_strictEq__slow;
    if (t0.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:784
        goto _offlineasm_13_strictEq__leftOK;
    if ((t0.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:785
        goto _offlineasm_13_strictEq__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_13_strictEq__leftOK)
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:787
        goto _offlineasm_13_strictEq__rightOK;
    if ((t1.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:788
        goto _offlineasm_13_strictEq__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_13_strictEq__rightOK)
    t0.i = (t0.u64 != t1.u64);                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:809
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:792
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:793
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_13_strictEq__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_nstricteq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_inc)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:816
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:817
        goto _offlineasm_preOp__slow;
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:830
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
    t1.i64 = t1.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:819
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3)) = t1.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:820
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_preOp__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_inc(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_dec)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:816
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:817
        goto _offlineasm_13_preOp__slow;
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:836
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
            goto _offlineasm_13_preOp__slow;
    }
    t1.i64 = t1.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:819
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3)) = t1.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:820
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_13_preOp__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_dec(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_to_number)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_14_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_14_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_14_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_14_loadConstantOrVariable__done)
    if (t2.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:845
        goto _offlineasm_opToNumberIsImmediate;
    if ((t2.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:846
        goto _offlineasm_opToNumberSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToNumberIsImmediate)
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:848
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToNumberSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_to_number(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_to_string)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_15_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_15_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_15_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_15_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:861
        goto _offlineasm_opToStringSlow;
    if (*CAST<int8_t*>(t0.i8p + 5) != int8_t(0x6))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:862
        goto _offlineasm_opToStringSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToStringIsString)
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:864
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToStringSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_to_string(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_negate)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_16_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_16_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_16_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_16_loadConstantOrVariable__done)
    if (t2.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:877
        goto _offlineasm_opNegateNotInt;
    if ((t2.i32 & int32_t(0x7fffffff)) == 0)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:878
        goto _offlineasm_opNegateSlow;
    t2.i32 = -t2.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:879
    t2.clearHighWord();
    t2.i64 = t2.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:880
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:881
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNegateNotInt)
    if ((t2.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:884
        goto _offlineasm_opNegateSlow;
    t2.i64 = t2.i64 ^ int64_t(0x8000000000000000);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:885
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:886
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opNegateSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_negate(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_add)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_binaryOp__binaryOpCustomStore__17_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_binaryOp__binaryOpCustomStore__17_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__17_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__17_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_binaryOp__binaryOpCustomStore__18_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_binaryOp__binaryOpCustomStore__18_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__18_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__18_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:899
        goto _offlineasm_binaryOp__binaryOpCustomStore__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:900
        goto _offlineasm_binaryOp__binaryOpCustomStore__op2NotInt;
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:956
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
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:947
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:948
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:907
        goto _offlineasm_binaryOp__binaryOpCustomStore__slow;
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:908
        goto _offlineasm_binaryOp__binaryOpCustomStore__op1NotIntOp2Int;
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:909
        goto _offlineasm_binaryOp__binaryOpCustomStore__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:910
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:911
    goto _offlineasm_binaryOp__binaryOpCustomStore__op1NotIntReady; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:912

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__op1NotIntOp2Int)
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:914

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__op1NotIntReady)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:917
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:918
    d0.d = d0.d + d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:957
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:920
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:921
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:922
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__op2NotInt)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:928
        goto _offlineasm_binaryOp__binaryOpCustomStore__slow;
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:929
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:930
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:931
    d0.d = d0.d + d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:957
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:933
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:934
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:935
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_add(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_mul)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_binaryOpCustomStore__19_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_binaryOpCustomStore__19_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__19_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__19_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_binaryOpCustomStore__20_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_binaryOpCustomStore__20_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__20_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__20_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:899
        goto _offlineasm_20_binaryOpCustomStore__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:900
        goto _offlineasm_20_binaryOpCustomStore__op2NotInt;
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:966
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:967
        uint32_t a = t1.i32;
        uint32_t b = t3.i32;
        bool didOverflow = ((b | a) >> 15);
        t3.i32 = t3.i32 * t1.i32;
        if (didOverflow)
            goto _offlineasm_20_binaryOpCustomStore__slow;
    }
    if (t3.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:968
        goto _offlineasm_binaryOpCustomStore__integerOperationAndStore__done;
    if (t1.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:969
        goto _offlineasm_20_binaryOpCustomStore__slow;
    if (t0.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:970
        goto _offlineasm_20_binaryOpCustomStore__slow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOpCustomStore__integerOperationAndStore__done)
    t3.i64 = t3.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:972
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t3.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:973
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_20_binaryOpCustomStore__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:907
        goto _offlineasm_20_binaryOpCustomStore__slow;
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:908
        goto _offlineasm_20_binaryOpCustomStore__op1NotIntOp2Int;
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:909
        goto _offlineasm_20_binaryOpCustomStore__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:910
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:911
    goto _offlineasm_20_binaryOpCustomStore__op1NotIntReady; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:912

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_20_binaryOpCustomStore__op1NotIntOp2Int)
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:914

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_20_binaryOpCustomStore__op1NotIntReady)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:917
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:918
    d0.d = d0.d * d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:975
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:920
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:921
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:922
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_20_binaryOpCustomStore__op2NotInt)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:928
        goto _offlineasm_20_binaryOpCustomStore__slow;
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:929
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:930
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:931
    d0.d = d0.d * d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:975
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:933
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:934
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:935
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_20_binaryOpCustomStore__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_mul(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_sub)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_binaryOp__binaryOpCustomStore__21_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_binaryOp__binaryOpCustomStore__21_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__21_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__21_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_binaryOp__binaryOpCustomStore__22_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_binaryOp__binaryOpCustomStore__22_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__22_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__binaryOpCustomStore__22_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:899
        goto _offlineasm_binaryOp__22_binaryOpCustomStore__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:900
        goto _offlineasm_binaryOp__22_binaryOpCustomStore__op2NotInt;
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:982
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
            goto _offlineasm_binaryOp__22_binaryOpCustomStore__slow;
    }
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:947
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:948
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__22_binaryOpCustomStore__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:907
        goto _offlineasm_binaryOp__22_binaryOpCustomStore__slow;
    if (t1.u64 >= tagTypeNumber.u64)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:908
        goto _offlineasm_binaryOp__22_binaryOpCustomStore__op1NotIntOp2Int;
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:909
        goto _offlineasm_binaryOp__22_binaryOpCustomStore__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:910
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:911
    goto _offlineasm_binaryOp__22_binaryOpCustomStore__op1NotIntReady; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:912

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__22_binaryOpCustomStore__op1NotIntOp2Int)
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:914

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__22_binaryOpCustomStore__op1NotIntReady)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:917
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:918
    d0.d = d0.d - d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:983
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:920
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:921
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:922
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__22_binaryOpCustomStore__op2NotInt)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:928
        goto _offlineasm_binaryOp__22_binaryOpCustomStore__slow;
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:929
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:930
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:931
    d0.d = d0.d - d1.d;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:983
    t0.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:933
    t0.i64 = t0.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:934
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:935
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_binaryOp__22_binaryOpCustomStore__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_sub(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_div)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_div(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_lshift)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__23_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__23_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__23_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__23_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__24_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__24_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__24_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__24_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1022
        goto _offlineasm_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1023
        goto _offlineasm_bitOp__slow;
    t0.i32 = t0.i32 << (t1.i & 0x1f);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1037
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1025
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1026
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_lshift(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_rshift)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__25_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__25_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__25_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__25_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__26_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__26_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__26_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__26_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1022
        goto _offlineasm_26_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1023
        goto _offlineasm_26_bitOp__slow;
    t0.i32 = t0.i32 >> (t1.i & 0x1f);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1045
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1025
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1026
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_26_bitOp__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_rshift(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_urshift)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__27_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__27_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__27_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__27_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__28_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__28_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__28_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__28_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1022
        goto _offlineasm_28_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1023
        goto _offlineasm_28_bitOp__slow;
    t0.u32 = t0.u32 >> (t1.i & 0x1f);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1053
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1025
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1026
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_28_bitOp__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_urshift(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_unsigned)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_29_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_29_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_29_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_29_loadConstantOrVariable__done)
    if (t2.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1063
        goto _offlineasm_opUnsignedSlow;
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1064
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opUnsignedSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_unsigned(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_bitand)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__30_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__30_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__30_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__30_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__31_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__31_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__31_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__31_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1022
        goto _offlineasm_31_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1023
        goto _offlineasm_31_bitOp__slow;
    t0.i32 = t0.i32 & t1.i32;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1074
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1025
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1026
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_31_bitOp__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_bitand(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_bitxor)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__32_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__32_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__32_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__32_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__33_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__33_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__33_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__33_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1022
        goto _offlineasm_33_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1023
        goto _offlineasm_33_bitOp__slow;
    t0.i32 = t0.i32 ^ t1.i32;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1082
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1025
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1026
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_33_bitOp__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_bitxor(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_bitor)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__34_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__34_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__34_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__34_loadConstantOrVariable__done)
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_bitOp__35_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_bitOp__35_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__35_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_bitOp__35_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1022
        goto _offlineasm_35_bitOp__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1023
        goto _offlineasm_35_bitOp__slow;
    t0.i32 = t0.i32 | t1.i32;                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1090
    t0.clearHighWord();
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1025
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1026
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_35_bitOp__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_bitor(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_check_has_instance)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_loadConstantOrVariableCell__36_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_loadConstantOrVariableCell__36_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__36_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__36_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opCheckHasInstanceSlow;
    if ((*CAST<int8_t*>(t0.i8p + 6) & int8_t(0x8)) == 0)     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1099
        goto _offlineasm_opCheckHasInstanceSlow;
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opCheckHasInstanceSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_check_has_instance(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_instanceof)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_loadConstantOrVariableCell__37_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_loadConstantOrVariableCell__37_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__37_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__37_loadConstantOrVariable__done)
    if ((t1.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opInstanceofSlow;
    if (*CAST<uint8_t*>(t1.i8p + 5) < uint8_t(0x12))         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1112
        goto _offlineasm_opInstanceofSlow;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_loadConstantOrVariableCell__38_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_loadConstantOrVariableCell__38_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__38_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__38_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opInstanceofSlow;
    t0.i = intptr_t(0x1);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1117

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opInstanceofLoop)
    t3.u = *CAST<uint32_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:506
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:507
    t2.i = *CAST<intptr_t*>(t2.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:508
    t2.i = *CAST<intptr_t*>(t2.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:509
    t3.i = *CAST<intptr_t*>(t2.i8p + (t3.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:510
    t2.i64 = *CAST<int64_t*>(t3.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1120
    if (t2.i64 == t1.i64)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1121
        goto _offlineasm_opInstanceofDone;
    if ((t2.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1122
        goto _offlineasm_opInstanceofLoop;
    t0.i = intptr_t(0x0);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1124

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opInstanceofDone)
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1126
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1128
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opInstanceofSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_instanceof(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_undefined)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_39_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_39_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_39_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_39_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1141
        goto _offlineasm_opIsUndefinedCell;
    t3.i = (t0.u64 == uint64_t(0xa));                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1142
    t3.i64 = t3.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1143
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t3.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1144
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opIsUndefinedCell)
    if ((*CAST<int8_t*>(t0.i8p + 6) & int8_t(0x1)) != 0)     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1147
        goto _offlineasm_masqueradesAsUndefined;
    t1.i = intptr_t(0x6);                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1148
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t1.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1149
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_masqueradesAsUndefined)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t3.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t3.i = *CAST<intptr_t*>(t1.i8p + (t3.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1153
    t1.i = *CAST<intptr_t*>(t1.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1154
    t0.i = (*CAST<uintptr_t*>(t3.i8p + 24) == t1.u);         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1155
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1156
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1157
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_boolean)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_40_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_40_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_40_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_40_loadConstantOrVariable__done)
    t0.i64 = t0.i64 ^ int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1166
    t0.i = ((t0.i64 & int64_t(-2)) == 0);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1167
    t0.i64 = t0.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1168
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1169
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_number)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_41_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_41_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_41_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_41_loadConstantOrVariable__done)
    t1.i = ((t0.i64 & tagTypeNumber.i64) != 0);              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1178
    t1.i64 = t1.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1179
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t1.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1180
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_string)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_42_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_42_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_42_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_42_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1189
        goto _offlineasm_opIsStringNotCell;
    t1.i = (*CAST<uint8_t*>(t0.i8p + 5) == uint8_t(0x6));    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1190
    t1.i64 = t1.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1191
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t1.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1192
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opIsStringNotCell)
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = int64_t(0x6);   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1195
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_object)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_43_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_43_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_43_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_43_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1204
        goto _offlineasm_opIsObjectNotCell;
    t1.i = (*CAST<uint8_t*>(t0.i8p + 5) >= uint8_t(0x12));   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1205
    t1.i64 = t1.i64 | int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1206
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t1.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1207
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opIsObjectNotCell)
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = int64_t(0x6);   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1210
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_getById__loadConstantOrVariableCell__44_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_getById__loadConstantOrVariableCell__44_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__loadConstantOrVariableCell__44_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t3.i = *CAST<intptr_t*>(t3.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__loadConstantOrVariableCell__44_loadConstantOrVariable__done)
    if ((t3.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_getById__opGetByIdSlow;
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t3.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1250
        goto _offlineasm_getById__opGetByIdSlow;
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(t3.i8p + (t2.i << 0));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1257
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1258
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t1.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__opGetByIdSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_get_by_id(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_by_id_out_of_line)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_getById__loadConstantOrVariableCell__45_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_getById__loadConstantOrVariableCell__45_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__loadConstantOrVariableCell__45_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t3.i = *CAST<intptr_t*>(t3.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getById__loadConstantOrVariableCell__45_loadConstantOrVariable__done)
    if ((t3.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_45_getById__opGetByIdSlow;
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t3.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1250
        goto _offlineasm_45_getById__opGetByIdSlow;
    t0.i = *CAST<intptr_t*>(t3.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1036
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 0));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1257
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t3.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1258
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t1.i8p + 16) = t3.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_45_getById__opGetByIdSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_get_by_id(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_array_length)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_loadConstantOrVariableCell__46_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_loadConstantOrVariableCell__46_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__46_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t3.i = *CAST<intptr_t*>(t3.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__46_loadConstantOrVariable__done)
    if ((t3.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opGetArrayLengthSlow;
    t2.i = t3.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1281
    t0.u = *CAST<uint32_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:514
    *CAST<int32_t*>(t1.i8p + 4) = t0.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:515
    t2.i = *CAST<uint8_t*>(t2.i8p + 4);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:516
    if ((t2.i32 & int32_t(0x1)) == 0)                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1283
        goto _offlineasm_opGetArrayLengthSlow;
    if ((t2.i32 & int32_t(0x1e)) == 0)                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1284
        goto _offlineasm_opGetArrayLengthSlow;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i = *CAST<intptr_t*>(t3.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1286
    t0.u = *CAST<uint32_t*>(t0.i8p - 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1287
    if (t0.i32 < int32_t(0x0))                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1288
        goto _offlineasm_opGetArrayLengthSlow;
    t0.i64 = t0.i64 | tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1289
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1291
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetArrayLengthSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_get_by_id(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putById__writeBarrierOnOperands__loadConstantOrVariableCell__47_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putById__writeBarrierOnOperands__loadConstantOrVariableCell__47_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__loadConstantOrVariableCell__47_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__loadConstantOrVariableCell__47_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putById__writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_putById__writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__48_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__48_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__48_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__48_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__writeBarrierDone)
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putById__loadConstantOrVariableCell__49_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putById__loadConstantOrVariableCell__49_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__loadConstantOrVariableCell__49_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__loadConstantOrVariableCell__49_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opPutByIdSlow;
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1306
        goto _offlineasm_opPutByIdSlow;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putById__getPropertyStorage__continuation__50_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putById__getPropertyStorage__continuation__50_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__getPropertyStorage__continuation__50_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t3.i = *CAST<intptr_t*>(t3.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__getPropertyStorage__continuation__50_loadConstantOrVariable__done)
    *CAST<int64_t*>(t0.i8p + (t1.i << 0)) = t3.i64;          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1314
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opPutByIdSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_put_by_id(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_out_of_line)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putById__writeBarrierOnOperands__loadConstantOrVariableCell__51_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putById__writeBarrierOnOperands__loadConstantOrVariableCell__51_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__loadConstantOrVariableCell__51_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__loadConstantOrVariableCell__51_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putById__52_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_putById__52_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__52_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__52_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__52_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__52_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putById__writeBarrierOnOperands__52_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_putById__writeBarrierOnOperands__52_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__writeBarrierOnOperands__52_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__52_writeBarrierOnOperands__writeBarrierDone)
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putById__loadConstantOrVariableCell__53_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putById__loadConstantOrVariableCell__53_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__loadConstantOrVariableCell__53_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__loadConstantOrVariableCell__53_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opPutByIdSlow;
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1306
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1036
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putById__getPropertyStorage__continuation__54_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putById__getPropertyStorage__continuation__54_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__getPropertyStorage__continuation__54_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putById__getPropertyStorage__continuation__54_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p + (t1.i << 0)) = t0.i64;          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1314
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition_direct)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__55_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__55_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__55_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__55_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByIdTransition__55_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_putByIdTransition__55_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__55_writeBarrierOnOperand__writeBarrierDone)
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__56_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__56_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__56_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__56_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t3.i = *CAST<intptr_t*>(t3.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t3.i = *CAST<intptr_t*>(t3.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t3.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1338
        goto _offlineasm_opPutByIdSlow;
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = t1.i + t0.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1346
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__57_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__57_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__57_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__57_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1348
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t1.u = *CAST<uint32_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1350
    *CAST<int32_t*>(t0.i8p) = t1.i32;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1351
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition_direct_out_of_line)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__58_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__58_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__58_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__58_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByIdTransition__58_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_putByIdTransition__58_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__58_writeBarrierOnOperand__writeBarrierDone)
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__59_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__59_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__59_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__59_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t3.i = *CAST<intptr_t*>(t3.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t3.i = *CAST<intptr_t*>(t3.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t3.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1338
        goto _offlineasm_opPutByIdSlow;
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1036
    t3.i = t1.i + t3.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1346
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__60_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__60_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__60_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__60_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1348
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t1.u = *CAST<uint32_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1350
    *CAST<int32_t*>(t0.i8p) = t1.i32;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1351
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition_normal)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__61_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__61_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__61_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__61_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByIdTransition__61_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_putByIdTransition__61_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__61_writeBarrierOnOperand__writeBarrierDone)
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__62_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__62_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__62_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__62_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t3.i = *CAST<intptr_t*>(t3.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t3.i = *CAST<intptr_t*>(t3.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t3.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1338
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t3.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1362
        goto _offlineasm_putByIdTransition__additionalChecks__62_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__62_assert__ok)
    t3.i = *CAST<intptr_t*>(t3.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1363
    if (t3.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1364
        goto _offlineasm_putByIdTransition__additionalChecks__63_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__63_assert__ok)
    if (*CAST<int64_t*>(t1.i8p + 32) == int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1365
        goto _offlineasm_putByIdTransition__additionalChecks__done;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__loop)
    t1.i64 = *CAST<int64_t*>(t1.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1367
    t2.u = *CAST<uint32_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:506
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:507
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:508
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:509
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:510
    t1.i = t2.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1369
    if (t1.i != *CAST<intptr_t*>(t3.i8p))                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1370
        goto _offlineasm_opPutByIdSlow;
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1371
    if (*CAST<int64_t*>(t1.i8p + 32) != int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1372
        goto _offlineasm_putByIdTransition__additionalChecks__loop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__done)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = t1.i + t0.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1346
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__63_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__63_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__63_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__63_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1348
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t1.u = *CAST<uint32_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1350
    *CAST<int32_t*>(t0.i8p) = t1.i32;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1351
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_id_transition_normal_out_of_line)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__64_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__64_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__64_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__writeBarrierOnOperand__loadConstantOrVariableCell__64_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByIdTransition__64_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_putByIdTransition__64_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__64_writeBarrierOnOperand__writeBarrierDone)
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__65_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__loadConstantOrVariableCell__65_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__65_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__loadConstantOrVariableCell__65_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t3.i = *CAST<intptr_t*>(t3.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t3.i = *CAST<intptr_t*>(t3.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t3.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1338
        goto _offlineasm_opPutByIdSlow;
    t3.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t3.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1362
        goto _offlineasm_putByIdTransition__additionalChecks__65_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__65_assert__ok)
    t3.i = *CAST<intptr_t*>(t3.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1363
    if (t3.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1364
        goto _offlineasm_putByIdTransition__additionalChecks__66_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__additionalChecks__66_assert__ok)
    if (*CAST<int64_t*>(t1.i8p + 32) == int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1365
        goto _offlineasm_putByIdTransition__66_additionalChecks__done;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__66_additionalChecks__loop)
    t1.i64 = *CAST<int64_t*>(t1.i8p + 32);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1367
    t2.u = *CAST<uint32_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:506
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:507
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:508
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:509
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:510
    t1.i = t2.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1369
    if (t1.i != *CAST<intptr_t*>(t3.i8p))                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1370
        goto _offlineasm_opPutByIdSlow;
    t3.i = t3.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1371
    if (*CAST<int64_t*>(t1.i8p + 32) != int64_t(0x2))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1372
        goto _offlineasm_putByIdTransition__66_additionalChecks__loop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__66_additionalChecks__done)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1036
    t3.i = t1.i + t3.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1346
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__66_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByIdTransition__getPropertyStorage__continuation__66_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__66_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByIdTransition__getPropertyStorage__continuation__66_loadConstantOrVariable__done)
    *CAST<int64_t*>(t3.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1348
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t1.u = *CAST<uint32_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1350
    *CAST<int32_t*>(t0.i8p) = t1.i32;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1351
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_by_val)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_loadConstantOrVariableCell__67_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_loadConstantOrVariableCell__67_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__67_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableCell__67_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_opGetByValSlow;
    t3.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t2.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1397
    t1.u = *CAST<uint32_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:514
    *CAST<int32_t*>(t3.i8p + 4) = t1.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:515
    t2.i = *CAST<uint8_t*>(t2.i8p + 4);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:516
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_loadConstantOrVariableInt32__68_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_loadConstantOrVariableInt32__68_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableInt32__68_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_loadConstantOrVariableInt32__68_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
        goto _offlineasm_opGetByValSlow;
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1401
    t3.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1402
    t2.i32 = t2.i32 & int32_t(0x1e);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1403
    t2.clearHighWord();
    if (t2.i32 == int32_t(0x14))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1404
        goto _offlineasm_opGetByValIsContiguous;
    if (t2.i32 != int32_t(0x1a))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1405
        goto _offlineasm_opGetByValNotContiguous;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValIsContiguous)
    if (t1.u32 >= *CAST<uint32_t*>(t3.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1408
        goto _offlineasm_opGetByValOutOfBounds;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i64 = *CAST<int64_t*>(t3.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1410
    if (t2.i64 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1411
        goto _offlineasm_opGetByValOutOfBounds;
    goto _offlineasm_opGetByValDone;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1412

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValNotContiguous)
    if (t2.i32 != int32_t(0x16))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1415
        goto _offlineasm_opGetByValNotDouble;
    if (t1.u32 >= *CAST<uint32_t*>(t3.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1416
        goto _offlineasm_opGetByValOutOfBounds;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1417
    d0.d = *CAST<double*>(t3.i8p + (t1.i << 3));             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1418
    if (std::isnan(d0.d) || std::isnan(d0.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1419
        || (d0.d != d0.d))
        goto _offlineasm_opGetByValOutOfBounds;
    t2.i64 = d0.castToInt64;                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1420
    t2.i64 = t2.i64 - tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1421
    goto _offlineasm_opGetByValDone;                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1422

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValNotDouble)
    t2.i32 = t2.i32 - int32_t(0x1c);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1425
    t2.clearHighWord();
    if (t2.u32 > uint32_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1426
        goto _offlineasm_opGetByValSlow;
    if (t1.u32 >= *CAST<uint32_t*>(t3.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1427
        goto _offlineasm_opGetByValOutOfBounds;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i64 = *CAST<int64_t*>(t3.i8p + (t1.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1429
    if (t2.i64 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1430
        goto _offlineasm_opGetByValOutOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValDone)
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1433
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t0.i8p + 16) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x6);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValOutOfBounds)
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<uint8_t*>(t0.i8p + 9) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1439

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opGetByValSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_get_by_val(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x6);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_val)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__writeBarrierOnOperands__loadConstantOrVariableCell__69_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__writeBarrierOnOperands__loadConstantOrVariableCell__69_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__loadConstantOrVariableCell__69_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__loadConstantOrVariableCell__69_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByVal__70_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_putByVal__70_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__70_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__70_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__70_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__70_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByVal__writeBarrierOnOperands__70_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_putByVal__writeBarrierOnOperands__70_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__70_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__70_writeBarrierOnOperands__writeBarrierDone)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__loadConstantOrVariableCell__71_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__loadConstantOrVariableCell__71_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__loadConstantOrVariableCell__71_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__loadConstantOrVariableCell__71_loadConstantOrVariable__done)
    if ((t1.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByVal__opPutByValSlow;
    t3.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t2.i = t1.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1467
    t0.u = *CAST<uint32_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:514
    *CAST<int32_t*>(t3.i8p + 4) = t0.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:515
    t2.i = *CAST<uint8_t*>(t2.i8p + 4);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:516
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__loadConstantOrVariableInt32__72_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__loadConstantOrVariableInt32__72_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__loadConstantOrVariableInt32__72_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t3.i = *CAST<intptr_t*>(t3.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__loadConstantOrVariableInt32__72_loadConstantOrVariable__done)
    if (t3.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
        goto _offlineasm_putByVal__opPutByValSlow;
    t3.i64 = t3.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1471
    t0.i = *CAST<intptr_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1472
    t2.i32 = t2.i32 & int32_t(0x1e);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1473
    t2.clearHighWord();
    if (t2.i32 != int32_t(0x14))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1474
        goto _offlineasm_putByVal__opPutByValNotInt32;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1446
        goto _offlineasm_putByVal__contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__73_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__73_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__73_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__73_loadConstantOrVariable__done)
    if (t1.u < tagTypeNumber.u)                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1478
        goto _offlineasm_putByVal__opPutByValSlow;
    *CAST<intptr_t*>(t0.i8p + (t3.i << 3)) = t1.i;           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1479
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1453
        goto _offlineasm_putByVal__opPutByValOutOfBounds;
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1454
    *CAST<uint8_t*>(t2.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1455
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1456
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1457
    goto _offlineasm_putByVal__contiguousPutByVal__storeResult; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1458

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__opPutByValNotInt32)
    if (t2.i32 != int32_t(0x16))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1483
        goto _offlineasm_putByVal__opPutByValNotDouble;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1446
        goto _offlineasm_putByVal__74_contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__74_contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__74_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__74_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__74_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__74_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1487
        goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__notInt;
    d0.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1488
    goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1489

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__notInt)
    t1.i = t1.i + tagTypeNumber.i;                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1491
    d0.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1492
    if (std::isnan(d0.d) || std::isnan(d0.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1493
        || (d0.d != d0.d))
        goto _offlineasm_putByVal__opPutByValSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__ready)
    *CAST<double*>(t0.i8p + (t3.i << 3)) = d0.d;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1495
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__74_contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1453
        goto _offlineasm_putByVal__opPutByValOutOfBounds;
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1454
    *CAST<uint8_t*>(t2.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1455
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1456
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1457
    goto _offlineasm_putByVal__74_contiguousPutByVal__storeResult; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1458

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__opPutByValNotDouble)
    if (t2.i32 != int32_t(0x1a))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1499
        goto _offlineasm_putByVal__opPutByValNotContiguous;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1446
        goto _offlineasm_putByVal__75_contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__75_contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__75_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__75_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__75_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__75_loadConstantOrVariable__done)
    *CAST<intptr_t*>(t0.i8p + (t3.i << 3)) = t1.i;           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1503
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__75_contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1453
        goto _offlineasm_putByVal__opPutByValOutOfBounds;
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1454
    *CAST<uint8_t*>(t2.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1455
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1456
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1457
    goto _offlineasm_putByVal__75_contiguousPutByVal__storeResult; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1458

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__opPutByValNotContiguous)
    if (t2.i32 != int32_t(0x1c))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1507
        goto _offlineasm_putByVal__opPutByValSlow;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1508
        goto _offlineasm_putByVal__opPutByValOutOfBounds;
    if (*CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x10)) == 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1509
        goto _offlineasm_putByVal__opPutByValArrayStorageEmpty;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__opPutByValArrayStorageStoreResult)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__76_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__76_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__76_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__76_loadConstantOrVariable__done)
    *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x10)) = t1.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1513
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__opPutByValArrayStorageEmpty)
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<uint8_t*>(t1.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1518
    *CAST<int32_t*>(t0.i8p + 12) = *CAST<int32_t*>(t0.i8p + 12) + int32_t(0x1); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1519
    if (t3.u32 < *CAST<uint32_t*>(t0.i8p - 8))               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1520
        goto _offlineasm_putByVal__opPutByValArrayStorageStoreResult;
    t1.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1521
    t1.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t1.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1522
    goto _offlineasm_putByVal__opPutByValArrayStorageStoreResult; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1523

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__opPutByValOutOfBounds)
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<uint8_t*>(t0.i8p + 9) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1527

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__opPutByValSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_put_by_val(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_val_direct)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__writeBarrierOnOperands__loadConstantOrVariableCell__77_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__writeBarrierOnOperands__loadConstantOrVariableCell__77_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__loadConstantOrVariableCell__77_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__loadConstantOrVariableCell__77_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByVal__78_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_putByVal__78_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__78_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__78_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__78_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__78_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_putByVal__writeBarrierOnOperands__78_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_putByVal__writeBarrierOnOperands__78_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__writeBarrierOnOperands__78_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__78_writeBarrierOnOperands__writeBarrierDone)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__loadConstantOrVariableCell__79_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__loadConstantOrVariableCell__79_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__loadConstantOrVariableCell__79_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__loadConstantOrVariableCell__79_loadConstantOrVariable__done)
    if ((t1.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_84_putByVal__opPutByValSlow;
    t3.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t2.i = t1.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1467
    t0.u = *CAST<uint32_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:514
    *CAST<int32_t*>(t3.i8p + 4) = t0.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:515
    t2.i = *CAST<uint8_t*>(t2.i8p + 4);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:516
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__loadConstantOrVariableInt32__80_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__loadConstantOrVariableInt32__80_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__loadConstantOrVariableInt32__80_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t3.i = *CAST<intptr_t*>(t3.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__loadConstantOrVariableInt32__80_loadConstantOrVariable__done)
    if (t3.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:434
        goto _offlineasm_84_putByVal__opPutByValSlow;
    t3.i64 = t3.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1471
    t0.i = *CAST<intptr_t*>(t1.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1472
    t2.i32 = t2.i32 & int32_t(0x1e);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1473
    t2.clearHighWord();
    if (t2.i32 != int32_t(0x14))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1474
        goto _offlineasm_84_putByVal__opPutByValNotInt32;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1446
        goto _offlineasm_putByVal__81_contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__81_contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__81_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__81_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__81_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__81_loadConstantOrVariable__done)
    if (t1.u < tagTypeNumber.u)                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1478
        goto _offlineasm_84_putByVal__opPutByValSlow;
    *CAST<intptr_t*>(t0.i8p + (t3.i << 3)) = t1.i;           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1479
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__81_contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1453
        goto _offlineasm_84_putByVal__opPutByValOutOfBounds;
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1454
    *CAST<uint8_t*>(t2.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1455
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1456
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1457
    goto _offlineasm_putByVal__81_contiguousPutByVal__storeResult; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1458

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_84_putByVal__opPutByValNotInt32)
    if (t2.i32 != int32_t(0x16))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1483
        goto _offlineasm_84_putByVal__opPutByValNotDouble;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1446
        goto _offlineasm_putByVal__82_contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__82_contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__82_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__82_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__82_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__82_loadConstantOrVariable__done)
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1487
        goto _offlineasm_putByVal__contiguousPutByVal__82_storeCallback__notInt;
    d0.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1488
    goto _offlineasm_putByVal__contiguousPutByVal__82_storeCallback__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1489

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__82_storeCallback__notInt)
    t1.i = t1.i + tagTypeNumber.i;                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1491
    d0.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1492
    if (std::isnan(d0.d) || std::isnan(d0.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1493
        || (d0.d != d0.d))
        goto _offlineasm_84_putByVal__opPutByValSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__82_storeCallback__ready)
    *CAST<double*>(t0.i8p + (t3.i << 3)) = d0.d;             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1495
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__82_contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1453
        goto _offlineasm_84_putByVal__opPutByValOutOfBounds;
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1454
    *CAST<uint8_t*>(t2.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1455
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1456
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1457
    goto _offlineasm_putByVal__82_contiguousPutByVal__storeResult; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1458

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_84_putByVal__opPutByValNotDouble)
    if (t2.i32 != int32_t(0x1a))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1499
        goto _offlineasm_84_putByVal__opPutByValNotContiguous;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 8))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1446
        goto _offlineasm_putByVal__83_contiguousPutByVal__outOfBounds;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__83_contiguousPutByVal__storeResult)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__83_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__contiguousPutByVal__storeCallback__83_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__83_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__contiguousPutByVal__storeCallback__83_loadConstantOrVariable__done)
    *CAST<intptr_t*>(t0.i8p + (t3.i << 3)) = t1.i;           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1503
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__83_contiguousPutByVal__outOfBounds)
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1453
        goto _offlineasm_84_putByVal__opPutByValOutOfBounds;
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1454
    *CAST<uint8_t*>(t2.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1455
    t2.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1456
    t2.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t2.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1457
    goto _offlineasm_putByVal__83_contiguousPutByVal__storeResult; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1458

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_84_putByVal__opPutByValNotContiguous)
    if (t2.i32 != int32_t(0x1c))                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1507
        goto _offlineasm_84_putByVal__opPutByValSlow;
    if (t3.u32 >= *CAST<uint32_t*>(t0.i8p - 4))              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1508
        goto _offlineasm_84_putByVal__opPutByValOutOfBounds;
    if (*CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x10)) == 0) // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1509
        goto _offlineasm_84_putByVal__opPutByValArrayStorageEmpty;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_84_putByVal__opPutByValArrayStorageStoreResult)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putByVal__84_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putByVal__84_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__84_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putByVal__84_loadConstantOrVariable__done)
    *CAST<int64_t*>(t0.i8p + (t3.i << 3) + intptr_t(0x10)) = t1.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1513
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_84_putByVal__opPutByValArrayStorageEmpty)
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<uint8_t*>(t1.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1518
    *CAST<int32_t*>(t0.i8p + 12) = *CAST<int32_t*>(t0.i8p + 12) + int32_t(0x1); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1519
    if (t3.u32 < *CAST<uint32_t*>(t0.i8p - 8))               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1520
        goto _offlineasm_84_putByVal__opPutByValArrayStorageStoreResult;
    t1.i32 = int32_t(0x1) + t3.i32;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1521
    t1.clearHighWord();
    *CAST<int32_t*>(t0.i8p - 8) = t1.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1522
    goto _offlineasm_84_putByVal__opPutByValArrayStorageStoreResult; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1523

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_84_putByVal__opPutByValOutOfBounds)
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<uint8_t*>(t0.i8p + 9) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1527

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_84_putByVal__opPutByValSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_put_by_val_direct(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jmp)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jeq_null)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:544
        goto _offlineasm_equalNull__assertNotConstant__84_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__assertNotConstant__84_assert__ok)
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1565
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1566
        goto _offlineasm_equalNull__immediate;
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    if ((*CAST<int8_t*>(t0.i8p + 6) & int8_t(0x1)) == 0)     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1584
        goto _offlineasm_equalNull__cellHandler__notMasqueradesAsUndefined;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1585
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1586
    if (*CAST<intptr_t*>(t2.i8p + 24) == t0.i)               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1587
        goto _offlineasm_equalNull__target;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__cellHandler__notMasqueradesAsUndefined)
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__target)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__immediate)
    t0.i64 = t0.i64 & int64_t(-9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1575
    if (t0.i64 == int64_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1590
        goto _offlineasm_equalNull__target;
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jneq_null)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:544
        goto _offlineasm_equalNull__assertNotConstant__85_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_equalNull__assertNotConstant__85_assert__ok)
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1565
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1566
        goto _offlineasm_85_equalNull__immediate;
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    if ((*CAST<int8_t*>(t0.i8p + 6) & int8_t(0x1)) == 0)     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1597
        goto _offlineasm_85_equalNull__target;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1598
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1599
    if (*CAST<intptr_t*>(t2.i8p + 24) != t0.i)               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1600
        goto _offlineasm_85_equalNull__target;
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_85_equalNull__target)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_85_equalNull__immediate)
    t0.i64 = t0.i64 & int64_t(-9);                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1575
    if (t0.i64 != int64_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1602
        goto _offlineasm_85_equalNull__target;
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jneq_ptr)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1609
    t2.i = *CAST<intptr_t*>(t2.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1610
    t1.i = *CAST<intptr_t*>(t2.i8p + (t1.i << 3) + intptr_t(0x418)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1611
    if (t1.i != *CAST<intptr_t*>(cfr.i8p + (t0.i << 3)))     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1612
        goto _offlineasm_opJneqPtrTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opJneqPtrTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_switch_imm)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_85_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_85_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_85_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_85_loadConstantOrVariable__done)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1666
    t2.i = *CAST<intptr_t*>(t2.i8p + 488);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1667
    t3.i32 = t3.i32 * int32_t(0x18);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1668
    t3.clearHighWord();
    t2.i = *CAST<intptr_t*>(t2.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1669
    t2.i = t2.i + t3.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1670
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1671
        goto _offlineasm_opSwitchImmNotInt;
    t1.i32 = t1.i32 - *CAST<int32_t*>(t2.i8p + 16);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1672
    t1.clearHighWord();
    if (t1.u32 >= *CAST<uint32_t*>(t2.i8p + 12))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1673
        goto _offlineasm_opSwitchImmFallThrough;
    t3.i = *CAST<intptr_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1674
    t1.i = *CAST<int32_t*>(t3.i8p + (t1.i << 2));            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1675
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1676
        goto _offlineasm_opSwitchImmFallThrough;
    t5.i = t5.i + t1.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchImmNotInt)
    if ((t1.i64 & tagTypeNumber.i64) != 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1680
        goto _offlineasm_opSwitchImmSlow;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchImmFallThrough)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchImmSlow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_switch_imm(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_switch_char)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_86_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_86_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_86_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_86_loadConstantOrVariable__done)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1694
    t2.i = *CAST<intptr_t*>(t2.i8p + 488);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1695
    t3.i32 = t3.i32 * int32_t(0x18);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1696
    t3.clearHighWord();
    t2.i = *CAST<intptr_t*>(t2.i8p + 32);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1697
    t2.i = t2.i + t3.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1698
    if ((t1.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1699
        goto _offlineasm_opSwitchCharFallThrough;
    if (*CAST<int8_t*>(t1.i8p + 5) != int8_t(0x6))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1700
        goto _offlineasm_opSwitchCharFallThrough;
    if (*CAST<int32_t*>(t1.i8p + 12) != int32_t(0x1))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1701
        goto _offlineasm_opSwitchCharFallThrough;
    t0.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1702
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1703
        goto _offlineasm_opSwitchOnRope;
    t1.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1704
    if ((*CAST<int32_t*>(t0.i8p + 16) & int32_t(0x8)) != 0)  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1705
        goto _offlineasm_opSwitchChar8Bit;
    t0.i = *CAST<uint16_t*>(t1.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1706
    goto _offlineasm_opSwitchCharReady;                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1707

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchChar8Bit)
    t0.i = *CAST<uint8_t*>(t1.i8p);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1709

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchCharReady)
    t0.i32 = t0.i32 - *CAST<int32_t*>(t2.i8p + 16);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1711
    t0.clearHighWord();
    if (t0.u32 >= *CAST<uint32_t*>(t2.i8p + 12))             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1712
        goto _offlineasm_opSwitchCharFallThrough;
    t2.i = *CAST<intptr_t*>(t2.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1713
    t1.i = *CAST<int32_t*>(t2.i8p + (t0.i << 2));            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1714
    if (t1.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1715
        goto _offlineasm_opSwitchCharFallThrough;
    t5.i = t5.i + t1.i;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchCharFallThrough)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opSwitchOnRope)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_switch_char(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_ret)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:529
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:530
        int32_t temp = *CAST<int32_t*>(t0.i8p + 432) + int32_t(0xa);
        *CAST<int32_t*>(t0.i8p + 432) = temp;
        if (temp < 0)
            goto  _offlineasm_checkSwitchToJITForEpilogue__checkSwitchToJIT__continue;
    }
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_replace(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_checkSwitchToJITForEpilogue__checkSwitchToJIT__continue)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_87_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_87_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_87_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_87_loadConstantOrVariable__done)
    sp.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:395
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:397
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:398
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:707
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_to_primitive)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_88_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_88_loadConstantOrVariable__done;        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1772
        goto _offlineasm_opToPrimitiveIsImm;
    if (*CAST<uint8_t*>(t0.i8p + 5) >= uint8_t(0x12))        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1773
        goto _offlineasm_opToPrimitiveSlowCase;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToPrimitiveIsImm)
    *CAST<int64_t*>(cfr.i8p + (t3.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1775
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opToPrimitiveSlowCase)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_to_primitive(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_catch)
    tagTypeNumber.i = intptr_t(0xffff000000000000);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1786
    tagMask.i = intptr_t(0xffff000000000002);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1787
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1794
    t3.i = t3.i & intptr_t(-16384);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1795
    t3.i = *CAST<intptr_t*>(t3.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1796
    cfr.i = *CAST<intptr_t*>(t3.i8p + 16632);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1797
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1800
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1801
    t5.i = *CAST<intptr_t*>(t3.i8p + 16648);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1802
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1803
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1804
    t0.i64 = *CAST<int64_t*>(t3.i8p + 16888);                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1806
    *CAST<int64_t*>(t3.i8p + 16888) = int64_t(0x0);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1807
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1809
    t3.i64 = *CAST<int64_t*>(t0.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1811
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t2.i << 3)) = t3.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1813
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_end)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:529
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:530
        int32_t temp = *CAST<int32_t*>(t0.i8p + 432) + int32_t(0xa);
        *CAST<int32_t*>(t0.i8p + 432) = temp;
        if (temp < 0)
            goto  _offlineasm_checkSwitchToJITForEpilogue__88_checkSwitchToJIT__continue;
    }
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_replace(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_checkSwitchToJITForEpilogue__88_checkSwitchToJIT__continue)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i32 < int32_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:544
        goto _offlineasm_assertNotConstant__88_assert__ok;
    CRASH();                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:225

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_assertNotConstant__88_assert__ok)
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1824
    sp.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:395
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:397
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:398
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:707
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_throw_from_slow_path_trampoline)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_handle_exception(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1834
    t1.i = t1.i & intptr_t(-16384);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1835
    t1.i = *CAST<intptr_t*>(t1.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1836
    opcode = *CAST<Opcode*>(t1.i8p + 16640);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1837
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_throw_during_call_trampoline)
    t2.i = lr.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:409
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1842

OFFLINE_ASM_OPCODE_LABEL(op_resolve_scope)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i32 != int32_t(0x0))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1948
        goto _offlineasm_rGlobalVar;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1913
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1914
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1916
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rGlobalVar)
    if (t0.i32 != int32_t(0x1))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1953
        goto _offlineasm_rClosureVar;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1913
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1914
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1916
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rClosureVar)
    if (t0.i32 != int32_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1958
        goto _offlineasm_rGlobalPropertyWithVarInjectionChecks;
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i = *CAST<intptr_t*>(cfr.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1929
    if (t2.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1930
        goto _offlineasm_resolveScope__resolveScopeLoopEnd;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_resolveScope__resolveScopeLoop)
    t0.i = *CAST<intptr_t*>(t0.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1933
    t2.i32 = t2.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1934
    t2.clearHighWord();
    if (t2.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1935
        goto _offlineasm_resolveScope__resolveScopeLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_resolveScope__resolveScopeLoopEnd)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1939
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rGlobalPropertyWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x4))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1963
        goto _offlineasm_rGlobalVarWithVarInjectionChecks;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1920
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1921
    t0.i = *CAST<intptr_t*>(t0.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1922
    if (*CAST<int8_t*>(t0.i8p + 4) == int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1923
        goto _offlineasm_rDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1913
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1914
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1916
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rGlobalVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x5))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1969
        goto _offlineasm_rClosureVarWithVarInjectionChecks;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1920
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1921
    t0.i = *CAST<intptr_t*>(t0.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1922
    if (*CAST<int8_t*>(t0.i8p + 4) == int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1923
        goto _offlineasm_rDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1913
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1914
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1916
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rClosureVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x6))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1975
        goto _offlineasm_rDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1920
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1921
    t0.i = *CAST<intptr_t*>(t0.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1922
    if (*CAST<int8_t*>(t0.i8p + 4) == int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1923
        goto _offlineasm_rDynamic;
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i = *CAST<intptr_t*>(cfr.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1929
    if (t2.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1930
        goto _offlineasm_88_resolveScope__resolveScopeLoopEnd;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_resolveScope__resolveScopeLoop)
    t0.i = *CAST<intptr_t*>(t0.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1933
    t2.i32 = t2.i32 - int32_t(0x1);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1934
    t2.clearHighWord();
    if (t2.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1935
        goto _offlineasm_88_resolveScope__resolveScopeLoop;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_88_resolveScope__resolveScopeLoopEnd)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1939
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_rDynamic)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_resolve_scope(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_from_scope)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i32 = t0.i32 & int32_t(0xffff);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2020
    t0.clearHighWord();
    if (t0.i32 != int32_t(0x0))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2023
        goto _offlineasm_gGlobalVar;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1987
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1990
        goto _offlineasm_gDynamic;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1215
        goto _offlineasm_getProperty__loadPropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1216
    t1.i32 = -t1.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1217
    t1.clearHighWord();
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1218
    goto _offlineasm_getProperty__loadPropertyAtVariableOffset__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1219

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getProperty__loadPropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1221

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getProperty__loadPropertyAtVariableOffset__ready)
    t2.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x310)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1223
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t0.i8p + 16) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1998
    t5.i = t5.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gGlobalVar)
    if (t0.i32 != int32_t(0x1))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2029
        goto _offlineasm_gClosureVar;
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t0.i64 = *CAST<int64_t*>(t0.i8p);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2003
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t1.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2006
    t5.i = t5.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gClosureVar)
    if (t0.i32 != int32_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2034
        goto _offlineasm_gGlobalPropertyWithVarInjectionChecks;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:416
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2011
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t1.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2014
    t5.i = t5.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gGlobalPropertyWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x4))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2040
        goto _offlineasm_gGlobalVarWithVarInjectionChecks;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1987
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1990
        goto _offlineasm_gDynamic;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1215
        goto _offlineasm_getProperty__88_loadPropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1216
    t1.i32 = -t1.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1217
    t1.clearHighWord();
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1218
    goto _offlineasm_getProperty__88_loadPropertyAtVariableOffset__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1219

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getProperty__88_loadPropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1221

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_getProperty__88_loadPropertyAtVariableOffset__ready)
    t2.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x310)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1223
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t0.i8p + 16) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t0.i << 3)) = t2.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1998
    t5.i = t5.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gGlobalVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x5))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2046
        goto _offlineasm_gClosureVarWithVarInjectionChecks;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1920
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1921
    t0.i = *CAST<intptr_t*>(t0.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1922
    if (*CAST<int8_t*>(t0.i8p + 4) == int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1923
        goto _offlineasm_gDynamic;
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t0.i64 = *CAST<int64_t*>(t0.i8p);                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2003
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t1.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2006
    t5.i = t5.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gClosureVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x6))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2052
        goto _offlineasm_gDynamic;
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1920
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1921
    t0.i = *CAST<intptr_t*>(t0.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1922
    if (*CAST<int8_t*>(t0.i8p + 4) == int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1923
        goto _offlineasm_gDynamic;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:416
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2011
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t1.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2014
    t5.i = t5.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_gDynamic)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_get_from_scope(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x8);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_to_scope)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i32 = t0.i32 & int32_t(0xffff);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2101
    t0.clearHighWord();
    if (t0.i32 != int32_t(0x3))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2104
        goto _offlineasm_pGlobalProperty;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__89_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__89_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__89_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__89_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_90_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_90_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__90_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__90_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__90_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__90_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_writeBarrierOnOperands__90_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_writeBarrierOnOperands__90_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__90_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_90_writeBarrierOnOperands__writeBarrierDone)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:416
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putLocalClosureVar__91_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putLocalClosureVar__91_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putLocalClosureVar__91_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putLocalClosureVar__91_loadConstantOrVariable__done)
    t3.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t3.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2090
        goto _offlineasm_putLocalClosureVar__noVariableWatchpointSet;
    if (*CAST<int8_t*>(t3.i8p + 4) != int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:525
        goto _offlineasm_pDynamic;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putLocalClosureVar__noVariableWatchpointSet)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x20)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2094
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pGlobalProperty)
    if (t0.i32 != int32_t(0x0))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2111
        goto _offlineasm_pGlobalVar;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__92_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__92_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__92_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__92_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_93_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_93_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__93_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__93_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__93_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__93_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_writeBarrierOnOperands__93_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_writeBarrierOnOperands__93_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__93_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_93_writeBarrierOnOperands__writeBarrierDone)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1987
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1990
        goto _offlineasm_pDynamic;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putProperty__94_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putProperty__94_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__94_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__94_loadConstantOrVariable__done)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1228
        goto _offlineasm_putProperty__storePropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1229
    t1.i32 = -t1.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1230
    t1.clearHighWord();
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1231
    goto _offlineasm_putProperty__storePropertyAtVariableOffset__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1232

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__storePropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1234

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__storePropertyAtVariableOffset__ready)
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x310)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1236
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pGlobalVar)
    if (t0.i32 != int32_t(0x1))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2118
        goto _offlineasm_pClosureVar;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnGlobalObject__loadConstantOrVariableCell__95_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnGlobalObject__loadConstantOrVariableCell__95_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnGlobalObject__loadConstantOrVariableCell__95_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnGlobalObject__loadConstantOrVariableCell__95_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_writeBarrierOnGlobalObject__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:473
        goto _offlineasm_writeBarrierOnGlobalObject__writeBarrierDone;
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:475
    t3.i = *CAST<intptr_t*>(t3.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:476
    t1.i = *CAST<uint8_t*>(t3.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:479
        goto _offlineasm_writeBarrierOnGlobalObject__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:480
    PUSH(t5);
    llint_write_barrier_slow(cfr, t3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:482
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnGlobalObject__writeBarrierDone)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putGlobalVar__96_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putGlobalVar__96_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putGlobalVar__96_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putGlobalVar__96_loadConstantOrVariable__done)
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (*CAST<int8_t*>(t2.i8p + 4) != int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:525
        goto _offlineasm_pDynamic;
    *CAST<int64_t*>(t0.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2076
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pClosureVar)
    if (t0.i32 != int32_t(0x2))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2124
        goto _offlineasm_pGlobalPropertyWithVarInjectionChecks;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__97_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__97_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__97_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__97_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_98_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_98_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__98_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__98_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__98_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__98_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_writeBarrierOnOperands__98_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_writeBarrierOnOperands__98_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__98_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_98_writeBarrierOnOperands__writeBarrierDone)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:416
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putClosureVar__99_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putClosureVar__99_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putClosureVar__99_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putClosureVar__99_loadConstantOrVariable__done)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x20)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2083
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pGlobalPropertyWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x4))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2131
        goto _offlineasm_pGlobalVarWithVarInjectionChecks;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__100_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__100_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__100_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__100_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_101_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_101_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__101_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__101_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__101_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__101_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_writeBarrierOnOperands__101_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_writeBarrierOnOperands__101_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__101_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_101_writeBarrierOnOperands__writeBarrierDone)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1987
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:498
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:499
    t1.i = *CAST<intptr_t*>(t1.i8p + 168);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:500
    t2.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:501
    t2.i = *CAST<intptr_t*>(t1.i8p + (t2.i << 3));           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:502
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1990
        goto _offlineasm_pDynamic;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putProperty__102_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putProperty__102_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__102_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__102_loadConstantOrVariable__done)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i32 < int32_t(0x64))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1228
        goto _offlineasm_putProperty__102_storePropertyAtVariableOffset__isInline;
    t0.i = *CAST<intptr_t*>(t0.i8p + 8);                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1229
    t1.i32 = -t1.i32;                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1230
    t1.clearHighWord();
    t1.i64 = t1.i32;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1231
    goto _offlineasm_putProperty__102_storePropertyAtVariableOffset__ready; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1232

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__102_storePropertyAtVariableOffset__isInline)
    t0.i = t0.i + intptr_t(-768);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1234

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putProperty__102_storePropertyAtVariableOffset__ready)
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x310)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1236
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pGlobalVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x5))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2138
        goto _offlineasm_pClosureVarWithVarInjectionChecks;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnGlobalObject__loadConstantOrVariableCell__103_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnGlobalObject__loadConstantOrVariableCell__103_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnGlobalObject__loadConstantOrVariableCell__103_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnGlobalObject__loadConstantOrVariableCell__103_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_103_writeBarrierOnGlobalObject__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:473
        goto _offlineasm_103_writeBarrierOnGlobalObject__writeBarrierDone;
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:475
    t3.i = *CAST<intptr_t*>(t3.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:476
    t1.i = *CAST<uint8_t*>(t3.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:479
        goto _offlineasm_103_writeBarrierOnGlobalObject__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:480
    PUSH(t5);
    llint_write_barrier_slow(cfr, t3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:482
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_103_writeBarrierOnGlobalObject__writeBarrierDone)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1920
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1921
    t0.i = *CAST<intptr_t*>(t0.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1922
    if (*CAST<int8_t*>(t0.i8p + 4) == int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1923
        goto _offlineasm_pDynamic;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putGlobalVar__104_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putGlobalVar__104_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putGlobalVar__104_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putGlobalVar__104_loadConstantOrVariable__done)
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    if (*CAST<int8_t*>(t2.i8p + 4) != int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:525
        goto _offlineasm_pDynamic;
    *CAST<int64_t*>(t0.i8p) = t1.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2076
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pClosureVarWithVarInjectionChecks)
    if (t0.i32 != int32_t(0x6))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2145
        goto _offlineasm_pDynamic;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__105_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__105_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__105_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__105_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_106_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_106_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__106_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__106_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__106_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__106_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_writeBarrierOnOperands__106_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_writeBarrierOnOperands__106_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__106_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_106_writeBarrierOnOperands__writeBarrierDone)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1920
    t0.i = *CAST<intptr_t*>(t0.i8p + 40);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1921
    t0.i = *CAST<intptr_t*>(t0.i8p + 1128);                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1922
    if (*CAST<int8_t*>(t0.i8p + 4) == int8_t(0x2))           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1923
        goto _offlineasm_pDynamic;
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:416
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_putClosureVar__107_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_putClosureVar__107_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putClosureVar__107_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_putClosureVar__107_loadConstantOrVariable__done)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x30)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x20)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2083
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_pDynamic)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_put_to_scope(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_from_arguments)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:416
    t1.u = *CAST<uint32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2160
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2161
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t1.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2164
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_to_arguments)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__108_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__108_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__108_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__loadConstantOrVariableCell__108_loadConstantOrVariable__done)
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_109_writeBarrierOnOperands__writeBarrierDone;
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:462
        goto _offlineasm_109_writeBarrierOnOperands__writeBarrierDone;
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__109_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__109_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__109_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__writeBarrierOnOperand__loadConstantOrVariableCell__109_loadConstantOrVariable__done)
    if ((t2.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:439
        goto _offlineasm_writeBarrierOnOperands__109_writeBarrierOnOperand__writeBarrierDone;
    t1.i = *CAST<uint8_t*>(t2.i8p + 7);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:520
    if (t1.i8 != 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:448
        goto _offlineasm_writeBarrierOnOperands__109_writeBarrierOnOperand__writeBarrierDone;
    PUSH(pcBase);                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:449
    PUSH(t5);
    llint_write_barrier_slow(cfr, t2);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:91
    POP(t5);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:451
    POP(pcBase);

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_writeBarrierOnOperands__109_writeBarrierOnOperand__writeBarrierDone)

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_109_writeBarrierOnOperands__writeBarrierDone)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:416
    t1.u = *CAST<uint32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2172
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_110_loadConstantOrVariable__constant;
    t2.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_110_loadConstantOrVariable__done;       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_110_loadConstantOrVariable__constant)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t2.i = *CAST<intptr_t*>(t2.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t2.i64 = *CAST<int64_t*>(t2.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_110_loadConstantOrVariable__done)
    *CAST<int64_t*>(t0.i8p + (t1.i << 3) + intptr_t(0x28)) = t2.i64; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2175
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_parent_scope)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:416
    t0.i = *CAST<intptr_t*>(t0.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2182
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2184
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_profile_type)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2190
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2191
    t1.i = *CAST<intptr_t*>(t1.i8p + 16992);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2193
    t2.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2195
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_111_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_111_loadConstantOrVariable__done;       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_111_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_111_loadConstantOrVariable__done)
    if (t0.i64 == int64_t(0x0))                              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2201
        goto _offlineasm_opProfileTypeDone;
    *CAST<int64_t*>(t2.i8p) = t0.i64;                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2203
    t3.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<intptr_t*>(t2.i8p + 8) = t3.i;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2207
    if ((t0.i64 & tagMask.i64) == 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2209
        goto _offlineasm_opProfileTypeIsCell;
    *CAST<int32_t*>(t2.i8p + 16) = int32_t(0x0);             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2210
    goto _offlineasm_opProfileTypeSkipIsCell;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2211

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opProfileTypeIsCell)
    t3.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2213
    *CAST<int32_t*>(t2.i8p + 16) = t3.i32;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2214

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opProfileTypeSkipIsCell)
    t2.i = t2.i + intptr_t(0x18);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2218
    *CAST<intptr_t*>(t1.i8p + 16) = t2.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2219
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2221
    if (t2.i != t1.i)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:2222
        goto _offlineasm_opProfileTypeDone;
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_profile_type_clear_log(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opProfileTypeDone)
    t5.i = t5.i + intptr_t(0x6);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_create_direct_arguments)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_create_direct_arguments(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_create_scoped_arguments)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_create_scoped_arguments(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_create_out_of_band_arguments)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_create_out_of_band_arguments(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_func)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_new_func(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_array)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_new_array(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_array_with_size)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_new_array_with_size(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_array_buffer)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_new_array_buffer(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_regexp)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_new_regexp(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_less)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_less(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_lesseq)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_lesseq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_greater)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_greater(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_greatereq)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_greatereq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_mod)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_mod(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_typeof)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_typeof(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_object_or_null)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_is_object_or_null(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_is_function)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_is_function(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_in)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_in(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_del_by_id)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_del_by_id(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_del_by_val)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_del_by_val(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_by_index)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_put_by_index(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_getter_by_id)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_put_getter_by_id(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_setter_by_id)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_put_setter_by_id(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_put_getter_setter)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_put_getter_setter(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x6);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jtrue)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_jumpTrueOrFalse__112_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_jumpTrueOrFalse__112_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__112_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__112_loadConstantOrVariable__done)
    t0.i64 = t0.i64 ^ int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1548
    if ((t0.i64 & int64_t(-1)) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1549
        goto _offlineasm_jumpTrueOrFalse__slow;
    if (t0.i32 != 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1083
        goto _offlineasm_jumpTrueOrFalse__target;
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__target)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jtrue(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jfalse)
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t1.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_jumpTrueOrFalse__113_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t1.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_jumpTrueOrFalse__113_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__113_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t1.i = t1.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t1.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_jumpTrueOrFalse__113_loadConstantOrVariable__done)
    t0.i64 = t0.i64 ^ int64_t(0x6);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1548
    if ((t0.i64 & int64_t(-1)) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1549
        goto _offlineasm_113_jumpTrueOrFalse__slow;
    if (t0.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1090
        goto _offlineasm_113_jumpTrueOrFalse__target;
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_113_jumpTrueOrFalse__target)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_113_jumpTrueOrFalse__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jfalse(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jless)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__114_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__114_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__114_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__114_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__115_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__115_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__115_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__115_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1624
        goto _offlineasm_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1625
        goto _offlineasm_compare__op2NotInt;
    if (t0.i32 < t1.i32)                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1097
        goto _offlineasm_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1630
        goto _offlineasm_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1631
        goto _offlineasm_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1632
    goto _offlineasm_compare__op1NotIntReady;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1633

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1635
        goto _offlineasm_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1636
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1637

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1639
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1640
    if (d0.d < d1.d)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1098
        goto _offlineasm_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1645
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
        goto _offlineasm_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    if (d0.d < d1.d)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1098
        goto _offlineasm_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__jumpTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jless(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jnless)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__116_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__116_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__116_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__116_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__117_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__117_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__117_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__117_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1624
        goto _offlineasm_117_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1625
        goto _offlineasm_117_compare__op2NotInt;
    if (t0.i32 >= t1.i32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1105
        goto _offlineasm_117_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_117_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1630
        goto _offlineasm_117_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1631
        goto _offlineasm_117_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1632
    goto _offlineasm_117_compare__op1NotIntReady;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1633

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_117_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1635
        goto _offlineasm_117_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1636
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1637

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_117_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1639
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1640
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1106
        || (d0.d >= d1.d))
        goto _offlineasm_117_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_117_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1645
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
        goto _offlineasm_117_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1106
        || (d0.d >= d1.d))
        goto _offlineasm_117_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_117_compare__jumpTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_117_compare__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jnless(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jgreater)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__118_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__118_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__118_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__118_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__119_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__119_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__119_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__119_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1624
        goto _offlineasm_119_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1625
        goto _offlineasm_119_compare__op2NotInt;
    if (t0.i32 > t1.i32)                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1113
        goto _offlineasm_119_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_119_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1630
        goto _offlineasm_119_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1631
        goto _offlineasm_119_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1632
    goto _offlineasm_119_compare__op1NotIntReady;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1633

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_119_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1635
        goto _offlineasm_119_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1636
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1637

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_119_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1639
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1640
    if (d0.d > d1.d)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1114
        goto _offlineasm_119_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_119_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1645
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
        goto _offlineasm_119_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    if (d0.d > d1.d)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1114
        goto _offlineasm_119_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_119_compare__jumpTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_119_compare__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jgreater(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jngreater)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__120_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__120_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__120_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__120_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__121_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__121_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__121_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__121_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1624
        goto _offlineasm_121_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1625
        goto _offlineasm_121_compare__op2NotInt;
    if (t0.i32 <= t1.i32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1121
        goto _offlineasm_121_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_121_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1630
        goto _offlineasm_121_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1631
        goto _offlineasm_121_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1632
    goto _offlineasm_121_compare__op1NotIntReady;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1633

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_121_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1635
        goto _offlineasm_121_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1636
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1637

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_121_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1639
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1640
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1122
        || (d0.d <= d1.d))
        goto _offlineasm_121_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_121_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1645
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
        goto _offlineasm_121_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1122
        || (d0.d <= d1.d))
        goto _offlineasm_121_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_121_compare__jumpTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_121_compare__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jngreater(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jlesseq)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__122_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__122_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__122_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__122_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__123_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__123_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__123_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__123_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1624
        goto _offlineasm_123_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1625
        goto _offlineasm_123_compare__op2NotInt;
    if (t0.i32 <= t1.i32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1129
        goto _offlineasm_123_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_123_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1630
        goto _offlineasm_123_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1631
        goto _offlineasm_123_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1632
    goto _offlineasm_123_compare__op1NotIntReady;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1633

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_123_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1635
        goto _offlineasm_123_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1636
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1637

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_123_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1639
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1640
    if (d0.d <= d1.d)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1130
        goto _offlineasm_123_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_123_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1645
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
        goto _offlineasm_123_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    if (d0.d <= d1.d)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1130
        goto _offlineasm_123_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_123_compare__jumpTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_123_compare__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jlesseq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jnlesseq)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__124_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__124_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__124_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__124_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__125_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__125_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__125_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__125_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1624
        goto _offlineasm_125_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1625
        goto _offlineasm_125_compare__op2NotInt;
    if (t0.i32 > t1.i32)                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1137
        goto _offlineasm_125_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_125_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1630
        goto _offlineasm_125_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1631
        goto _offlineasm_125_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1632
    goto _offlineasm_125_compare__op1NotIntReady;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1633

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_125_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1635
        goto _offlineasm_125_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1636
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1637

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_125_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1639
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1640
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1138
        || (d0.d > d1.d))
        goto _offlineasm_125_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_125_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1645
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
        goto _offlineasm_125_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1138
        || (d0.d > d1.d))
        goto _offlineasm_125_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_125_compare__jumpTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_125_compare__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jnlesseq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jgreatereq)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__126_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__126_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__126_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__126_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__127_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__127_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__127_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__127_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1624
        goto _offlineasm_127_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1625
        goto _offlineasm_127_compare__op2NotInt;
    if (t0.i32 >= t1.i32)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1145
        goto _offlineasm_127_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_127_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1630
        goto _offlineasm_127_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1631
        goto _offlineasm_127_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1632
    goto _offlineasm_127_compare__op1NotIntReady;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1633

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_127_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1635
        goto _offlineasm_127_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1636
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1637

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_127_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1639
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1640
    if (d0.d >= d1.d)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1146
        goto _offlineasm_127_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_127_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1645
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
        goto _offlineasm_127_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    if (d0.d >= d1.d)                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1146
        goto _offlineasm_127_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_127_compare__jumpTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_127_compare__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jgreatereq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_jngreatereq)
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    if (t2.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__128_loadConstantOrVariable__constant;
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t2.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__128_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__128_loadConstantOrVariable__constant)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t0.i = *CAST<intptr_t*>(t0.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t2.i = t2.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t0.i64 = *CAST<int64_t*>(t0.i8p + (t2.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__128_loadConstantOrVariable__done)
    if (t3.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_compare__129_loadConstantOrVariable__constant;
    t1.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_compare__129_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__129_loadConstantOrVariable__constant)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t1.i = *CAST<intptr_t*>(t1.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t3.i = t3.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t1.i64 = *CAST<int64_t*>(t1.i8p + (t3.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_compare__129_loadConstantOrVariable__done)
    if (t0.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1624
        goto _offlineasm_129_compare__op1NotInt;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1625
        goto _offlineasm_129_compare__op2NotInt;
    if (t0.i32 < t1.i32)                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1153
        goto _offlineasm_129_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_129_compare__op1NotInt)
    if ((t0.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1630
        goto _offlineasm_129_compare__slow;
    if (t1.u64 < tagTypeNumber.u64)                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1631
        goto _offlineasm_129_compare__op1NotIntOp2NotInt;
    d1.d = t1.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1632
    goto _offlineasm_129_compare__op1NotIntReady;            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1633

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_129_compare__op1NotIntOp2NotInt)
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1635
        goto _offlineasm_129_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1636
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1637

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_129_compare__op1NotIntReady)
    t0.i64 = t0.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1639
    d0.d = t0.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1640
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1154
        || (d0.d < d1.d))
        goto _offlineasm_129_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_129_compare__op2NotInt)
    d0.d = t0.i32;                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1645
    if ((t1.i64 & tagTypeNumber.i64) == 0)                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1646
        goto _offlineasm_129_compare__slow;
    t1.i64 = t1.i64 + tagTypeNumber.i64;                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1647
    d1.d = t1.castToDouble;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1648
    if (std::isnan(d0.d) || std::isnan(d1.d)                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1154
        || (d0.d < d1.d))
        goto _offlineasm_129_compare__jumpTarget;
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_129_compare__jumpTarget)
    t5.i32 = t5.i32 + *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:36
    t5.clearHighWord();
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_129_compare__slow)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_jngreatereq(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_loop_hint)
    t1.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1160
    t1.i = *CAST<intptr_t*>(t1.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1161
    t0.i = *CAST<intptr_t*>(t1.i8p + 10040);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1162
    if (t0.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1163
        goto _offlineasm_handleWatchdogTimer;

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_afterWatchdogTimerCheck)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:529
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:530
        int32_t temp = *CAST<int32_t*>(t0.i8p + 432) + int32_t(0x1);
        *CAST<int32_t*>(t0.i8p + 432) = temp;
        if (temp < 0)
            goto  _offlineasm_checkSwitchToJITForLoop__129_checkSwitchToJIT__continue;
    }
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:402
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_loop_osr(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:405
        goto _offlineasm_checkSwitchToJITForLoop__checkSwitchToJIT__action__recover;
    sp.i = t1.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:406
    opcode = t0.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:407
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_checkSwitchToJITForLoop__checkSwitchToJIT__action__recover)
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:409
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:410

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_checkSwitchToJITForLoop__129_checkSwitchToJIT__continue)
    t5.i = t5.i + intptr_t(0x1);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_handleWatchdogTimer)
    t0.i = *CAST<uint8_t*>(t0.i8p + 4);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1168
    if (t0.i8 == 0)                                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1169
        goto _offlineasm_afterWatchdogTimerCheck;
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:390
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_handle_watchdog_timer(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t0.i != 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:393
        goto _offlineasm_throwHandler;
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:394
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:395
    goto _offlineasm_afterWatchdogTimerCheck;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1171

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_throwHandler)
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1173

OFFLINE_ASM_OPCODE_LABEL(op_switch_string)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_switch_string(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_new_func_exp)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_new_func_exp(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_call)
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i = -t3.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1728
    t0.i64 = *CAST<int64_t*>(cfr.i8p + (t3.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1729
    if ((t0.i64 & tagMask.i64) != 0)                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1730
        goto _offlineasm_arrayProfileForCall__done;
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x38)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t3.u = *CAST<uint32_t*>(t0.i8p);                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1732
    *CAST<int32_t*>(t1.i8p + 4) = t3.i32;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1733

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_arrayProfileForCall__done)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t2.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1740
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_doCall__130_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_doCall__130_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__130_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t3.i = *CAST<intptr_t*>(t3.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__130_loadConstantOrVariable__done)
    if (t3.i64 != t2.i64)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1742
        goto _offlineasm_doCall__opCallSlow;
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i32 = t3.i32 << (intptr_t(0x3) & 0x1f);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1744
    t3.clearHighWord();
    t3.i = -t3.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1745
    t3.i = t3.i + cfr.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1746
    *CAST<int64_t*>(t3.i8p + 24) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1747
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1749
    *CAST<int32_t*>(t3.i8p + 32) = t2.i32;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1750
    t3.i = t3.i + intptr_t(0x10);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1751
    sp.i = t3.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:479
    lr.opcode = getOpcode(llint_cloop_did_return_from_js_2); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:481
    opcode = *CAST<Opcode*>(t1.i8p + 32);
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_cloop_did_return_from_js_2)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__opCallSlow)
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:383
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_call(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t1.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
        goto _offlineasm_doCall__slowPathForCall__callCallSlowPath__action__dontUpdateSP;
    sp.i = intptr_t(0x10) + t1.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__slowPathForCall__callCallSlowPath__action__dontUpdateSP)
    lr.opcode = getOpcode(llint_cloop_did_return_from_js_3); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:502
    opcode = t0.opcode;
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_cloop_did_return_from_js_3)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_construct)
    t0.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x10)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t1.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x28)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    t2.i = *CAST<intptr_t*>(t1.i8p + 16);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1740
    if (t0.i >= intptr_t(0x40000000))                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:421
        goto _offlineasm_doCall__131_loadConstantOrVariable__constant;
    t3.i64 = *CAST<int64_t*>(cfr.i8p + (t0.i << 3));         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:422
    goto _offlineasm_doCall__131_loadConstantOrVariable__done; // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:423

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__131_loadConstantOrVariable__constant)
    t3.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:425
    t3.i = *CAST<intptr_t*>(t3.i8p + 360);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:426
    t0.i = t0.i - intptr_t(0x40000000);                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:427
    t3.i64 = *CAST<int64_t*>(t3.i8p + (t0.i << 3));          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:428

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__131_loadConstantOrVariable__done)
    if (t3.i64 != t2.i64)                                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1742
        goto _offlineasm_131_doCall__opCallSlow;
    t3.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x20)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    t3.i32 = t3.i32 << (intptr_t(0x3) & 0x1f);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1744
    t3.clearHighWord();
    t3.i = -t3.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1745
    t3.i = t3.i + cfr.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1746
    *CAST<int64_t*>(t3.i8p + 24) = t2.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1747
    t2.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x18)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1749
    *CAST<int32_t*>(t3.i8p + 32) = t2.i32;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1750
    t3.i = t3.i + intptr_t(0x10);                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1751
    sp.i = t3.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:479
    lr.opcode = getOpcode(llint_cloop_did_return_from_js_4); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:481
    opcode = *CAST<Opcode*>(t1.i8p + 32);
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_cloop_did_return_from_js_4)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_131_doCall__opCallSlow)
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:383
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_construct(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t1.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
        goto _offlineasm_doCall__slowPathForCall__callCallSlowPath__131_action__dontUpdateSP;
    sp.i = intptr_t(0x10) + t1.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_doCall__slowPathForCall__callCallSlowPath__131_action__dontUpdateSP)
    lr.opcode = getOpcode(llint_cloop_did_return_from_js_5); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:502
    opcode = t0.opcode;
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_cloop_did_return_from_js_5)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_call_varargs)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_size_frame_for_varargs(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:581
    t3.i = t3.i & intptr_t(-16384);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:582
    t3.i = *CAST<intptr_t*>(t3.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:583
    if (*CAST<int64_t*>(t3.i8p + 16888) == 0)                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:584
        goto _offlineasm_branchIfException__noException;
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:585

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_branchIfException__noException)
    sp.i = t1.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1204
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:383
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_call_varargs(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t1.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
        goto _offlineasm_slowPathForCall__callCallSlowPath__132_action__dontUpdateSP;
    sp.i = intptr_t(0x10) + t1.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_slowPathForCall__callCallSlowPath__132_action__dontUpdateSP)
    lr.opcode = getOpcode(llint_cloop_did_return_from_js_6); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:502
    opcode = t0.opcode;
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_cloop_did_return_from_js_6)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_construct_varargs)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_size_frame_for_varargs(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:581
    t3.i = t3.i & intptr_t(-16384);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:582
    t3.i = *CAST<intptr_t*>(t3.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:583
    if (*CAST<int64_t*>(t3.i8p + 16888) == 0)                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:584
        goto _offlineasm_132_branchIfException__noException;
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:585

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_132_branchIfException__noException)
    sp.i = t1.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1222
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:383
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_construct_varargs(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t1.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
        goto _offlineasm_slowPathForCall__callCallSlowPath__133_action__dontUpdateSP;
    sp.i = intptr_t(0x10) + t1.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_slowPathForCall__callCallSlowPath__133_action__dontUpdateSP)
    lr.opcode = getOpcode(llint_cloop_did_return_from_js_7); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:502
    opcode = t0.opcode;
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_cloop_did_return_from_js_7)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_call_eval)
    *CAST<int32_t*>(cfr.i8p + 36) = t5.i32;                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:383
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_call_eval(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    if (t1.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:493
        goto _offlineasm_slowPathForCall__callCallSlowPath__134_action__dontUpdateSP;
    sp.i = intptr_t(0x10) + t1.i;                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:498

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_slowPathForCall__callCallSlowPath__134_action__dontUpdateSP)
    lr.opcode = getOpcode(llint_cloop_did_return_from_js_8); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:502
    opcode = t0.opcode;
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_cloop_did_return_from_js_8)
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_generic_return_point)
    t5.u = *CAST<uint32_t*>(cfr.i8p + 36);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:45
    pcBase.i = *CAST<intptr_t*>(cfr.i8p + 16);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:46
    pcBase.i = *CAST<intptr_t*>(pcBase.i8p + 104);           // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:47
    t1.i = *CAST<int32_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:121
    *CAST<int64_t*>(cfr.i8p + (t1.i << 3)) = t0.i64;         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:49
    t2.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x40)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<int64_t*>(t2.i8p + 16) = t0.i64;                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:491
    t5.i = t5.i + intptr_t(0x9);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_strcat)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_strcat(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_push_with_scope)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_push_with_scope(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_create_lexical_environment)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_create_lexical_environment(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_throw)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_throw(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_throw_static_error)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_throw_static_error(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_profile_will_call)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1310
    t0.i = *CAST<intptr_t*>(t0.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1311
    t0.u = *CAST<uint32_t*>(t0.i8p + 16928);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1312
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1313
        goto _offlineasm_opProfilerWillCallDone;
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_profile_will_call(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opProfilerWillCallDone)
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_profile_did_call)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1321
    t0.i = *CAST<intptr_t*>(t0.i8p + 96);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1322
    t0.u = *CAST<uint32_t*>(t0.i8p + 16928);                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1323
    if (t0.i == 0)                                           // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1324
        goto _offlineasm_opProfilerDidCallDone;
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_profile_did_call(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opProfilerDidCallDone)
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_debug)
    t0.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1332
    t0.u = *CAST<uint32_t*>(t0.i8p + 84);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1333
    if (t0.i32 == 0)                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1334
        goto _offlineasm_opDebugDone;
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = llint_slow_path_debug(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_opDebugDone)
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_GLUE_LABEL(llint_native_call_trampoline)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:434
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:435
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    *CAST<intptr_t*>(cfr.i8p + 16) = intptr_t(0x0);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1848
    t0.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1878
    t1.i = intptr_t(-16384) & t0.i;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1879
    t1.i = *CAST<intptr_t*>(t1.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1880
    *CAST<intptr_t*>(t1.i8p + 10032) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1881
    t3.i = lr.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:409
    *CAST<intptr_t*>(cfr.i8p + 8) = t3.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1883
    t0.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1884
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1885
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1886
    nativeFunc = *CAST<NativeFunction*>(t1.i8p + 64);        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1888
    functionReturnValue = JSValue::decode(nativeFunc(t0.execState));
    #if USE(JSVALUE32_64)
        t1.i = functionReturnValue.tag();
        t0.i = functionReturnValue.payload();
    #else // USE_JSVALUE64)
        t0.encodedJSValue = JSValue::encode(functionReturnValue);
    #endif // USE_JSVALUE64)
    lr.i = t3.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:420
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1893
    t3.i = t3.i & intptr_t(-16384);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1894
    t3.i = *CAST<intptr_t*>(t3.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1895
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:446
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:447
    if (*CAST<int64_t*>(t3.i8p + 16888) != 0)                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1902
        goto _offlineasm_nativeCallTrampoline__handleException;
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1903
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_nativeCallTrampoline__handleException)
    *CAST<intptr_t*>(t3.i8p + 10032) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1906
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1908

OFFLINE_ASM_GLUE_LABEL(llint_native_construct_trampoline)
    PUSH(lr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:434
    PUSH(cfr);                                               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:435
    cfr.i = sp.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:437
    *CAST<intptr_t*>(cfr.i8p + 16) = intptr_t(0x0);          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1848
    t0.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1878
    t1.i = intptr_t(-16384) & t0.i;                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1879
    t1.i = *CAST<intptr_t*>(t1.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1880
    *CAST<intptr_t*>(t1.i8p + 10032) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1881
    t3.i = lr.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:409
    *CAST<intptr_t*>(cfr.i8p + 8) = t3.i;                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1883
    t0.i = cfr.i;                                            // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1884
    t1.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1885
    t1.i = *CAST<intptr_t*>(t1.i8p + 24);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1886
    nativeFunc = *CAST<NativeFunction*>(t1.i8p + 72);        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1888
    functionReturnValue = JSValue::decode(nativeFunc(t0.execState));
    #if USE(JSVALUE32_64)
        t1.i = functionReturnValue.tag();
        t0.i = functionReturnValue.payload();
    #else // USE_JSVALUE64)
        t0.encodedJSValue = JSValue::encode(functionReturnValue);
    #endif // USE_JSVALUE64)
    lr.i = t3.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:420
    t3.i = *CAST<intptr_t*>(cfr.i8p + 24);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1893
    t3.i = t3.i & intptr_t(-16384);                          // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1894
    t3.i = *CAST<intptr_t*>(t3.i8p + 240);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1895
    POP(cfr);                                                // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:446
    POP(lr);                                                 // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:447
    if (*CAST<int64_t*>(t3.i8p + 16888) != 0)                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1902
        goto _offlineasm_134_nativeCallTrampoline__handleException;
    opcode = lr.opcode;                                      // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1903
    DISPATCH_OPCODE();

  OFFLINE_ASM_LOCAL_LABEL(_offlineasm_134_nativeCallTrampoline__handleException)
    *CAST<intptr_t*>(t3.i8p + 10032) = cfr.i;                // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1906
    t2.i = *CAST<intptr_t*>(cfr.i8p + 16);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:462
    pc.u = *CAST<uint32_t*>(t2.i8p + 56);                    // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:456
    pc.i = pc.i << (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:457
    pc.i = pc.i + intptr_t(0x0);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:458
    sp.i = cfr.i - pc.i;                                     // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:468
    goto llint_throw_from_slow_path_trampoline;              // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:1908

OFFLINE_ASM_OPCODE_LABEL(op_get_enumerable_length)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_get_enumerable_length(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_has_indexed_property)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_has_indexed_property(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_has_structure_property)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_has_structure_property(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x5);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_has_generic_property)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_has_generic_property(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_direct_pname)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_get_direct_pname(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x7);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_get_property_enumerator)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_get_property_enumerator(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_enumerator_structure_pname)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_next_structure_enumerator_pname(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_enumerator_generic_pname)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_next_generic_enumerator_pname(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x4);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_to_index_string)
    t5.i8p = pcBase.i8p + (t5.i << 3);                       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:352
    t3.i = pcBase.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:353
    {                                                        // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:83
        SlowPathReturnType result = slow_path_to_index_string(cfr, t5);
        decodeResult(result, t0.vp, t1.vp);
    }
    t5.i = t0.i;                                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:357
    pcBase.i = t3.i;                                         // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:358
    t5.i = t5.i - pcBase.i;                                  // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:359
    t5.i = t5.i >> (intptr_t(0x3) & 0x1f);                   // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:360
    t5.i = t5.i + intptr_t(0x3);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();

OFFLINE_ASM_OPCODE_LABEL(op_profile_control_flow)
    t0.i = *CAST<intptr_t*>(pcBase.i8p + (t5.i << 3) + intptr_t(0x8)); // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:125
    *CAST<uint8_t*>(t0.i8p + 8) = int8_t(0x1);               // ..\..\JavaScriptCore\local\JavaScriptCore\llint\LowLevelInterpreter.asm:1395
    t5.i = t5.i + intptr_t(0x2);                             // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:31
    opcode = *CAST<Opcode*>(pcBase.i8p + (t5.i << 3));       // ..\..\JavaScriptCore\local\JavaScriptCore\llint/LowLevelInterpreter64.asm:27
    DISPATCH_OPCODE();
OFFLINE_ASM_END
#endif
