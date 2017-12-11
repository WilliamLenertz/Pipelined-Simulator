			     !!!!READ ME!!!!!


Creators:

Ryan Trapp
William Lenertz


Project 3: Pipelined Simulator


			***Introduction***



Name: Rip_Van_saWinkle’s_Trusty_Pipe

Welcome to our piped simulator.

This program is meant to simulate a piped variant of the UST 3400 machine (Rip Van saWinkle).

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

4.) multiplicationTest.txt
- This is an example of assembly code instructions input into the simulator. These are MIPS based instructions.
- This assembly code cannot directly be input into the simulator. It must first be run through an assembler (separate project I have created)

5.) multMachine.txt
- This is the machine code that corresponds to the "multiplicationTest.txt" file. This is the multiplication function in its machine code form.
- This file may be directly input into the program. Please see below for details on compilation and running.


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

