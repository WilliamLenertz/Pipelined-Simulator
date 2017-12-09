			     !!!!READ ME!!!!!


Creators:

Ryan Trapp
William Lenertz


Project 3: Pipelined Simulator

Answer to where ‘add 12345 0 0’ instruction came from:

*************
the “add” instruction is denoted as 0 in the UST-3400. After the halt instruction, the simulator will try to get the instruction below it which is 12345 (0x00003039). If we decompose this number as an instruction (as the simulator would), we would end up with an add instruction (000 in opcode field), with a source_1 register of 0, a source_2 register of 0 and a destination register of 12345.

Since the format for R-Type instructions is <opcode> <destReg> <Source1Reg> <Source2Reg>, the simulator will print out “add 12345 0 0”
*************





			***Introduction***



Name: Rip_Van_saWinkle’s_Trusty_Pipe

Welcome to our piped simulator.

This program is meant to simulate a piped variant of the UST 3400 machine.(Rip Van saWinkle).

At this point we do believe our simulator works for all cases. We have not found a set of assembly code instructions that manage hazards incorrectly.







			***Contents***



Upon downloading and unzipping the .zip file, a file with the same name as the .zip file will appear.

Inside this file will be the following contents:

1.)README!!!.txt
- The first file you should open inside of the file.
- Will give you background as well as important information about the simulator

2.)pipeSimulator.c
- This is the c code for our piped UST-3400 simulator
- Do not make any modifications to this file please!!
- This is the file that will be compiled into an executable file

3.)Makefile
- Use this to compile the pipeSimulator.c file
- When “make” is input into the command line (while in this directory), this file will compile the pipeSimulator.c file and output an executable file titled “pipeSimulator.”
- Another file will also appear called “pipedSimulator.o.” You do not need to worry about this file

4.)TestSuite
- This is a directory full of test cases for our simulator. 
- For a detailed list of the test cases inside as well as what they test please refer to the list at the bottom of this page.







			***Background***



Our piped simulator is designed to simulate a piped variant of the UST-3400 machine. Please read background on assembly language for the UST-3400 before beginning to use this simulator. This includes what opcodes are allowed and how each instruction is formatted. This way, the machine code you input is correct for the UST-3400 machine.







			***RULES***



1.)Registers:
- As you should know, the UST-3400 machine uses 8 registers (0-7). Please do not attempt to access a register outside of this range

2.)JALR:
- Unlike the original UST-3400 simulator, this piped simulator does not execute JALR instructions. 
- Please refrain from using JALR when writing your assembly code for this simulator

3.)MEMORY:
- Unlike our project 1 simulator, the new pipelined simulator has room for 65536 words of memory
- Although it is much more than the other simulator, keep this in mind when trying to store data and do not exceed the maximum memory.
- If you attempt to store data beyond this threshold, the simulator will stop.







			***Instructions for Use***



(Assumes .zip file has been unzipped in desired directory)

1.)Compiling the C file:
- First, change your current directory to the file that appeared after unzipping the .zip file
- then, in the command line, type “make” like below:

*******
$make
*******

- After executing this, the corresponding executable file should appear. as well as a .o file

2.)Running the Simulator
- In order to run the simulator, you must type your desired machine code file into the command line after typing the executable file. It should look like the following command:

********
$./pipeSimulator <machineCodeFile>
********


3.) Output
- The output from the simulator will appear on screen for your viewing.
- This includes the current state of the machine after each cycle as well as some statistics after the machine has halted (if it halts -> user dependent)

4.)When you are finished running the simulator, you may type “make clean” into the command line to remove the executable file and the .o file from the directory. You may use the following command:

********
$make clean
********






			***Test Cases***



All Test cases listed below can be found inside the “TestSuite” folder.

These test cases test the basic functionality of each instruction as well as how the simulator deals with each different hazard.


1.)initialLoadTest.txt
- This test is the exact assembly code given on the project sheet. 
- The output from this test should match the output on the bottom of the project sheet
- After finished running, 12345 should be stored in register 1
- This test does not test any hazards




2.)addTest.txt
- Tests basic functionality of add instruction
- Tests data forwarding from EXMEM pipeline.
- The second add depends on the result of the first add directly above it
- After completing, 14 should be stored in register 4



3.)add2Test.txt
- Tests data forwarding from MEMWB pipeline.
- The third add instruction depends on the result of the first add instruction
- After completing, 14 should be stored in register 4




4.)add3Test.txt
- Tests data forwarding from WBEND pipeline.
- The fourth add instruction depends on the result of the first add instruction
- After completing, 14 should be stored in register 4




5.)nandTest.txt
- Tests basic functionality of nand instruction
- This test LW stalls for nand. Notice the first nand instruction depends on the result of the previous two LW instructions. The machine must be halted until the LW instructions have completed.
- Tests data forwarding from EXMEM pipeline register for nand
- the second nand instruction depends on the result of the first nand instruction
- After completing, -2 should be stored in register 4 (the math is below):

000101
000100 & (5,4)
————————
000100 not
————————
111011
000101 & (result, 5)
————————
000001 not
————————
111110 => -2 in 2’s compliment binary



6.)nand2Test.txt
- Tests data forwarding from MEMWB pipeline register for nand
- the second nand instruction depends on the result of the first nand instruction which is in the MEMWB pipeline
- After completing, -2 should be stored in register 4 (the math is above):




7.)nand3Test
- Tests data forwarding from WBEND pipeline register for nand
- the second nand instruction depends on the result of the first nand instruction which is in the WBEND pipeline
- After completing, -2 should be stored in register 4 (the math is above):






8.)lwAddTest.txt
- when another instruction relies on result of a load word instruction, we have to implement a certain number of stalls.
- This cases tests stalling when LW is in the IDEX pipeline.
- When looking through the output, the second add instruction should remain in the IFID stage until the lw is all the way through execution
- After finishing, 5 should be stored in register 3




9.)lwlwTest.txt
- This tests the rare case where a lw instruction depends on the result of a previous lw instruction.
- This also tests stalling when LW is in the EXMEM pipeline
- the second lw instruction depends on the completion of the first lw instruction,
- When looking through the output, the second LW instruction should remain in the IFID stage until the first LW instruction has finished executing.
- After completion, 9 should be stored in register 3, 5 should be stored in r2 and 4 should be stored in r1




10.)swTest.txt
- This tests basic functionality of the SW instruction.
- This also tests data forwarding for SW data hazards as well as data hazards for LW and add.
- The SW instruction depends on the result of the second add instruction.
- After completion, 14 should be stored at data memory location 6.





11.)beqMissTest.txt
- This tests basic functionality of the BEQ instruction
- This also tests the branch misprediction hazard
- Under normal conditions, our simulator assumes the result of a branch is “branch not executed.” Therefore in the case that a branch must happen, the simulator must flush the pipeline and fetch the proper instruction from instruction memory
- register 2 and register 1 should be equal when the branch hits the EXMEM stage.
- When looking at the output, when the BEQ hits the MEMWB stage, the instructions after it should be flushed and replaced with NOOP instructions
- The next instruction in the IFID stage should be the HALT instruction



12.)multTest.txt
- More strenuous as well as realistic test for the simulator
- This is the same multiplication function as from project 1.
- Instead of multiplying two large numbers, this instead multiplies 2x2
- Other modifications have been made to keep the total cycles under 100
- This tests the simulator when many different hazards are present as well as branch mispredictions
- After completing, 4 (the result of 2x2) should be stored in register 7





13.)twoForwardTest.txt
- This tests having to forward data from two different destination registers (EXMEM and MEMWB in this case).
- Notice the third ADD instruction depends on the result of both of the previous ADD instructions.
- Because of this, we must be able to forward from two different pipeline registers into the execution stage.
- After completion, 6 should be stored in register 5.





14.)swOutOfRange.txt
- This checks attempting to store data at a memory location that is out of the range of the simulator. 
- Likely an unnecessary addition, but we’ve added it anyway.
- Notice register 3 has a number larger than the maximum allocated data memory. The SW instruction will then try to store data at this location in data memory.
- After running, the machine should halt early and display a “memory access out of range” error.
- The machine should alsp print out the machine state before halting.
