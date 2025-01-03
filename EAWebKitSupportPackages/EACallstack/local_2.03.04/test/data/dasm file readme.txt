These .dasm files are here to help test our EACallstack disassembler (dasm) functionality.

The files were made by taking a built executable and running the compiler disassembler tool on them. 
The resulting file is usually very big and includes linker sections and stuff that's not important to us.
So we manually take a text editor and chop away all but the body of a function we want to test.

Example of how to create a dasm file with GCC-based tools:

    arm-eabi-objdump.exe --disassemble C:\build\bin\android\EATextTest > Android-debug-arm.dasm

Then we edit the resulting .dasm file to have just one function, like thsi:

    0004e2dc <_ZN19EAThreadDynamicDataD1Ev>:
       4e2dc:	b510      	push	{r4, lr}
       4e2de:	1c04      	adds	r4, r0, #0
       4e2e0:	f7ba ef38 	blx	9154 <_Z18androidGetFileRootv-0xc4>
       4e2e4:	2300      	movs	r3, #0
       4e2e6:	6003      	str	r3, [r0, #0]
       4e2e8:	1c20      	adds	r0, r4, #0
       4e2ea:	3064      	adds	r0, #100
       4e2ec:	f7ba ede8 	blx	8ec0 <_Z18androidGetFileRootv-0x358>
       4e2f0:	1c20      	adds	r0, r4, #0
       4e2f2:	305c      	adds	r0, #92
       4e2f4:	f7ba ee02 	blx	8efc <_Z18androidGetFileRootv-0x31c>
       4e2f8:	1c20      	adds	r0, r4, #0
       4e2fa:	bd10      	pop	{r4, pc}

And our runtime dasm test code will open this file, convert it to binary, and disassemble it, 
with the goal of having a result that's similar to the asm above. Function names won't be possible
in the generated asm unless we have a symbol file such as the .map file handy.


