//
//  main.c
//  Project 3 Copy
//
//  Created by William Lenertz and Ryan Trapp on 4/7/17.
//  Copyright © 2017 William Lenertz|Ryan Trapp. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */

#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR – not implemented in this project */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000


typedef struct IFIDstruct{
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXstruct{
    int instr;
    int pcPlus1;
    int readRegA;
    int readRegB;
    int offset;
} IDEXType;

typedef struct EXMEMstruct{
    int instr;
    int branchTarget;
    int aluResult;
    int readReg;
} EXMEMType;

typedef struct MEMWBstruct{
    int instr;
    int writeData;
} MEMWBType;

typedef struct WBENDstruct{
    int instr;
    int writeData;
} WBENDType;

typedef struct statestruct{
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;
    int cycles; /* Number of cycles run so far */
    int fetched; /* Total number of instructions fetched */
    int retired; /* Total number of completed instructions */
    int branches; /* Total number of branches executed */
    int mispreds; /* Number of branch mispredictions*/
} stateType;

void printState(stateType *statePtr);
int opcode(int instruction);
int field0(int instruction);
int field1(int instruction);
int field2(int instruction);
void printInstruction(int instr);
int signExtend(int num);








int main(int argc, const char * argv[]){
    
    
    FILE *f;
    f = fopen(argv[1], "r");
    //f = fopen("/Users/mrnatural028/Desktop/multPipe.txt", "r");
    
    //declare states
    stateType state;
    stateType newState;
    
    
    //initialize variables
    state.numMemory = 0;
    state.cycles = 0, state.fetched = 0, state.retired = 0;
    state.branches = 0, state.mispreds = 0;
    state.pc = 0;
    int j = 0;
    for (j = 0; j<NUMREGS; ++j) {
        state.reg[j] = 0;
    }
    state.IFID.instr = NOOPINSTRUCTION;
    state.IDEX.instr = NOOPINSTRUCTION;
    state.EXMEM.instr = NOOPINSTRUCTION;
    state.MEMWB.instr = NOOPINSTRUCTION;
    state.WBEND.instr = NOOPINSTRUCTION;
    
    
    //Not necessary, but we were getting weird
    //numbers at the start of the program.
    state.IFID.pcPlus1 = 0;
    state.IDEX.pcPlus1 = 0;
    state.IDEX.readRegA = 0;
    state.IDEX.readRegB = 0;
    state.EXMEM.branchTarget = 0;
    state.EXMEM.aluResult = 0;
    state.EXMEM.readReg = 0;
    state.MEMWB.writeData = 0;
    
    
    
    
    //populate instruction mem
    int size = 32;
    char line[size];
    char *lineCopy;
    int instruction;
    int i = 0;
    while (fgets(line, size, f)) {
        lineCopy = strdup(line);
        instruction = (int)strtol(lineCopy, NULL, 10);
        state.instrMem[i] = instruction;
        state.dataMem[i] = instruction;
        ++i;
    }
    
    
    
    int lwFlag = 0;
    int fetchFlag = 0;
    int retireFlag = 0;
    
    state.numMemory = i;
    
    
    //begin simulator
    while(1){
        
        printState(&state);
        
        /* check for halt */
        if(HALT == opcode(state.MEMWB.instr)) {
            printf("machine halted\n");
            printf("total of %d cycles executed\n", state.cycles);
            printf("total of %d instructions fetched\n", state.fetched);
            printf("total of %d instructions retired\n", state.retired);
            printf("total of %d branches executed\n", state.branches);
            printf("total of %d branch mispredictions\n", state.mispreds);
            exit(0);
            
        }
        
        newState = state;
        newState.cycles++;
        
        
        
        //LW Stalls for ADD | NAND | SW
        if (opcode(state.IFID.instr) == ADD ||
            opcode(state.IFID.instr) == NAND ||
            opcode(state.IFID.instr) == SW ||
            opcode(state.IFID.instr) == BEQ) {
            
            if((opcode(state.IDEX.instr) == LW) &&
               (field0(state.IDEX.instr) == field0(state.IFID.instr) ||
                field0(state.IDEX.instr) == field1(state.IFID.instr))){
                   
                lwFlag = 4;
                   
            }else if((opcode(state.EXMEM.instr) == LW) &&
                    (field0(state.EXMEM.instr) == field0(state.IFID.instr) ||
                    field0(state.EXMEM.instr) == field1(state.IFID.instr))){
                            
                lwFlag = 3;
                        
            }else if((opcode(state.MEMWB.instr) == LW) &&
                    (field0(state.MEMWB.instr) == field0(state.IFID.instr) ||
                    field0(state.MEMWB.instr) == field1(state.IFID.instr))){
                                     
                lwFlag = 2;
                                     
            }else if((opcode(state.WBEND.instr) == LW) &&
                    (field0(state.WBEND.instr) == field0(state.IFID.instr) ||
                    field0(state.WBEND.instr) == field1(state.IFID.instr))){
                //Must be 2 for propper
                lwFlag = 2;
        }
        
            
            //LW Stalls for lw
        }else if (opcode(state.IFID.instr) == LW){
            
            if((opcode(state.IDEX.instr) == LW) &&
               (field0(state.IDEX.instr) == field1(state.IFID.instr))){
                lwFlag = 4;
            }else if((opcode(state.EXMEM.instr) == LW) &&
                     (field0(state.EXMEM.instr) == field1(state.IFID.instr))){
                lwFlag = 3;
            }else if((opcode(state.MEMWB.instr) == LW) &&
                     (field0(state.MEMWB.instr) == field1(state.IFID.instr))){
                lwFlag = 2;
            }else if((opcode(state.WBEND.instr) == LW) &&
                     (field0(state.WBEND.instr) == field1(state.IFID.instr))){
                lwFlag = 1;
            }
        }
        
        
        
        
        
        
        
        
        /*------------------ IF stage ----------------- */
        
        //If we are not stalling
        if (lwFlag == 0) {
            newState.IFID.instr = state.instrMem[state.pc];
            newState.IFID.pcPlus1 = state.pc + 1;
            newState.pc = state.pc + 1;
            
            //End fetch count
            if (opcode(state.instrMem[state.pc]) == HALT ) {
                fetchFlag = 1;
                newState.fetched = state.fetched + 1; //count the halt
            }
            //fetch count
            if (!fetchFlag) {
                newState.fetched = state.fetched + 1;
            }
            
        }
        
        
        
        
        
        
        
        
        /*------------------ ID stage ----------------- */
        
        //If we are not stalling
        if (lwFlag == 0) {
            newState.IDEX.instr = state.IFID.instr;
            newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
            newState.IDEX.offset = signExtend(field2(state.IFID.instr));
            newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
            newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
        }
        
        
        
        
        
        
        
        
        /*------------------ EX stage ----------------- */
        
        
        //Noops inserted if we are stalling for lw
        if (lwFlag > 0) {
            lwFlag--;
            newState.EXMEM.instr = state.IDEX.instr;
            newState.IDEX.instr = NOOPINSTRUCTION;
            newState.EXMEM.branchTarget = 0;
        }else{
            newState.EXMEM.instr = state.IDEX.instr;
            newState.EXMEM.branchTarget = state.IDEX.offset + state.IDEX.pcPlus1;
            
        }
        
        //Checks if we've forwarded any data.
        //Used for lw instruction
        int forwardFlag = 0;
        
        //Checks if either of 2 registers came from
        //forwarded inputs (Used for add, nand, sw, beq).
        //Ensures most recent data is forwarded
        int aFlag = 0;
        int bFlag = 0;
        
        //Takes on forwarded data from state (Does NOT take data from newState)
        //Data pushed through to MEMWB is some combination
        //of these two.
        //Used for add, nand, sw, beq
        int aResult;
        int bResult;
        
        
        
        //Current Instruction is add
        if ((opcode(state.IDEX.instr)) == ADD) {
            
            aResult = state.IDEX.readRegA;
            bResult = state.IDEX.readRegB;
            
            
            
            //previous ONE instruction is add or nand
            if (opcode(state.EXMEM.instr) == ADD ||
                opcode(state.EXMEM.instr) == NAND) {
                
                //previous instructions dest field is same as one of two current source fields
                if (field2(state.EXMEM.instr) == field1(state.IDEX.instr) ||
                    field2(state.EXMEM.instr) == field0(state.IDEX.instr)) {
                    
                    forwardFlag = 1;
                    
                    if ((field2(state.EXMEM.instr) == field1(state.IDEX.instr)) &&
                        (field2(state.EXMEM.instr) == field0(state.IDEX.instr))){
                        aResult = state.EXMEM.aluResult;
                        bResult = state.EXMEM.aluResult;
                        aFlag = 1;
                        bFlag = 1;
                        
                        
                    }else {
                        //if we have not forwarded to bResult AND the fields match
                        if ((!bFlag) && field2(state.EXMEM.instr) == field1(state.IDEX.instr)) {
                            bResult = state.EXMEM.aluResult;
                            bFlag = 1;
                        }
                        //if we have not forwarded to bResult AND the fields match
                        else if ((!aFlag) && field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                            aFlag = 1;
                            aResult = state.EXMEM.aluResult;
                        }
                        
                        
                    }
                }
            }
            
            //Previous 2 instruction is ADD or NAND
            if ((!aFlag || !bFlag) &&
                (opcode(state.MEMWB.instr) == ADD ||
                 opcode(state.MEMWB.instr) == NAND)){
                    
                    //previous TWO instruction
                    if (field2(state.MEMWB.instr) == field1(state.IDEX.instr) ||
                        field2(state.MEMWB.instr) == field0(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        //If both fields match previous destination field AND both aResult and bResult have
                        //not taken on forwarded data already
                        if ((!aFlag && !bFlag) && (field2(state.MEMWB.instr) == field1(state.IDEX.instr)) &&
                            (field2(state.MEMWB.instr) == field0(state.IDEX.instr))){
                            
                            aResult = state.MEMWB.writeData;
                            bResult = state.MEMWB.writeData;
                            aFlag = 1;
                            bFlag = 1;
                            
                            
                        }else {
                            if ((!bFlag) && field2(state.MEMWB.instr) == field1(state.IDEX.instr)) {
                                bResult = state.MEMWB.writeData;
                                bFlag = 1;
                            }
                            else if ((!aFlag) && field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                                aFlag = 1;
                                aResult = state.MEMWB.writeData;
                            }
                        }
                    }
                }
            
            
            //Previous THREE instruction
            if ((!aFlag || !bFlag) &&
                (opcode(state.WBEND.instr) == ADD ||
                 opcode(state.WBEND.instr) == NAND)){
                    
                    if (field2(state.WBEND.instr) == field1(state.IDEX.instr) ||
                        field2(state.WBEND.instr) == field0(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        if ((!aFlag && !bFlag) && (field2(state.WBEND.instr) == field1(state.IDEX.instr)) &&
                            (field2(state.WBEND.instr) == field0(state.IDEX.instr))){
                            
                            aResult = state.WBEND.writeData;
                            bResult = state.WBEND.writeData;
                            aFlag = 1;
                            bFlag = 1;
                            
                            
                            
                        }else {
                            if ((!bFlag) && field2(state.WBEND.instr) == field1(state.IDEX.instr)) {
                                bResult = state.WBEND.writeData;
                                bFlag = 1;
                            }
                            else if ((!aFlag) && field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                                aFlag = 1;
                                aResult = state.WBEND.writeData;
                            }
                        }
                    }
                    
                }
            
            //Results are combined and updated in newState according to instruction
            newState.EXMEM.aluResult = aResult + bResult;
            
            newState.EXMEM.readReg = 0;
            
        }// end of add
        
        
        
        
        
        //current instruction is NAND
        else if((opcode(state.IDEX.instr)) == NAND){
            
            
            
            
            aResult = state.IDEX.readRegA;
            bResult = state.IDEX.readRegB;
            
            
            
            //previous ONE instruction is add or nand
            if (opcode(state.EXMEM.instr) == ADD ||
                opcode(state.EXMEM.instr) == NAND) {
                
                //previous instructions dest field is same as one of two current source fields
                if (field2(state.EXMEM.instr) == field1(state.IDEX.instr) ||
                    field2(state.EXMEM.instr) == field0(state.IDEX.instr)) {
                    
                    forwardFlag = 1;
                    
                    if ((field2(state.EXMEM.instr) == field1(state.IDEX.instr)) &&
                        (field2(state.EXMEM.instr) == field0(state.IDEX.instr))){
                        aResult = state.EXMEM.aluResult;
                        bResult = state.EXMEM.aluResult;
                        aFlag = 1;
                        bFlag = 1;
                        
                        
                    }else {
                        if ((!bFlag) && field2(state.EXMEM.instr) == field1(state.IDEX.instr)) {
                            bResult = state.EXMEM.aluResult;
                            bFlag = 1;
                        }
                        else if ((!aFlag) && field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                            aFlag = 1;
                            aResult = state.EXMEM.aluResult;
                        }
                        
                        
                    }
                }
            }
            
            //Previous 2 instruction is ADD or NAND
            if ((!aFlag || !bFlag) &&
                (opcode(state.MEMWB.instr) == ADD ||
                 opcode(state.MEMWB.instr) == NAND)){
                    
                    //previous TWO instruction
                    if (field2(state.MEMWB.instr) == field1(state.IDEX.instr) ||
                        field2(state.MEMWB.instr) == field0(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        if ((!aFlag && !bFlag) && (field2(state.MEMWB.instr) == field1(state.IDEX.instr)) &&
                            (field2(state.MEMWB.instr) == field0(state.IDEX.instr))){
                            aResult = state.MEMWB.writeData;
                            bResult = state.MEMWB.writeData;
                            aFlag = 1;
                            bFlag = 1;
                            
                            
                        }else {
                            if ((!bFlag) && field2(state.MEMWB.instr) == field1(state.IDEX.instr)) {
                                bResult = state.MEMWB.writeData;
                                bFlag = 1;
                            }
                            else if ((!aFlag) && field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                                aFlag = 1;
                                aResult = state.MEMWB.writeData;
                            }
                        }
                    }
                }
            
            
            //Previous THREE instruction
            if ((!aFlag || !bFlag) &&
                (opcode(state.WBEND.instr) == ADD ||
                 opcode(state.WBEND.instr) == NAND)){
                    
                    if (field2(state.WBEND.instr) == field1(state.IDEX.instr) ||
                        field2(state.WBEND.instr) == field0(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        if ((!aFlag && !bFlag) && (field2(state.WBEND.instr) == field1(state.IDEX.instr)) &&
                            (field2(state.WBEND.instr) == field0(state.IDEX.instr))){
                            aResult = state.WBEND.writeData;
                            bResult = state.WBEND.writeData;
                            aFlag = 1;
                            bFlag = 1;
                            
                            
                            
                        }else {
                            if ((!bFlag) && field2(state.WBEND.instr) == field1(state.IDEX.instr)) {
                                bResult = state.WBEND.writeData;
                                bFlag = 1;
                            }
                            else if ((!aFlag) && field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                                aFlag = 1;
                                aResult = state.WBEND.writeData;
                            }
                        }
                    }
                    
                }
            
            newState.EXMEM.aluResult = ~(aResult & bResult);
            
            newState.EXMEM.readReg = 0;
            
            
        }
        
        
        
        //if current opcode is LW
        else if(opcode(state.IDEX.instr) == LW) {
            
            
            
            
            //if previous ONE opcode is add or nand
            if (opcode(state.EXMEM.instr) == ADD ||
                opcode(state.EXMEM.instr) == NAND) {
                
                //previous dest reg is same as current source reg
                if (field2(state.EXMEM.instr) == field1(state.IDEX.instr)) {
                    
                    forwardFlag = 1;
                    
                    newState.EXMEM.aluResult = state.IDEX.offset + state.EXMEM.aluResult;
                }
                
                
            }
            
            //PREVIOUS TWO INSTRUCTION
            
            if ((!forwardFlag) &&
                (opcode(state.MEMWB.instr) == ADD ||
                 opcode(state.MEMWB.instr) == NAND)){
                    
                    
                    if (field2(state.MEMWB.instr) == field1(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        newState.EXMEM.aluResult = state.IDEX.offset + state.MEMWB.writeData;
                    }
                    
                }
            
            
            //PREVIOUS THREE INSTRUCTION
            
            if ((!forwardFlag) && (opcode(state.WBEND.instr) == ADD ||
                                   opcode(state.WBEND.instr) == NAND)){
                
                
                if (field2(state.WBEND.instr) == field1(state.IDEX.instr)) {
                    
                    forwardFlag = 1;
                    
                    newState.EXMEM.aluResult = state.IDEX.offset + state.WBEND.writeData;
                }
                
            }
            
            //If No forwarding
            if (!forwardFlag) {
                newState.EXMEM.aluResult = state.IDEX.readRegB + state.IDEX.offset;
            }
            
            newState.EXMEM.readReg = state.IDEX.readRegA;
        }//END LW
        
        
        //if current opcode is SW
        else if(opcode(state.IDEX.instr) == SW) {
            
            aResult = state.IDEX.readRegA;
            bResult = state.IDEX.readRegB;
            
            //if previous ONE opcode is add or nand
            if (opcode(state.EXMEM.instr) == ADD ||
                opcode(state.EXMEM.instr) == NAND) {
                
                
                if (field2(state.EXMEM.instr) == field1(state.IDEX.instr) ||
                    field2(state.EXMEM.instr) == field0(state.IDEX.instr)) {
                    
                    forwardFlag = 1;
                    
                    //check if both source registers equal destination register
                    if ((field2(state.EXMEM.instr) == field1(state.IDEX.instr)) &&
                        (field2(state.EXMEM.instr) == field0(state.IDEX.instr))){
                        aFlag = 1;
                        bFlag = 1;
                        aResult = state.EXMEM.aluResult;
                        bResult = state.EXMEM.aluResult;
                        
                    }else{
                        
                        if ((!bFlag) && field2(state.EXMEM.instr) == field1(state.IDEX.instr)) {
                            bFlag = 1;
                            bResult = state.EXMEM.aluResult;
                        }
                        else if ((!aFlag) && field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                            aFlag = 1;
                            aResult = state.EXMEM.aluResult;
                        }
                    }
                }
            }
            
            
            //PREVIOUS TWO INSTRUCTION
            if ((!aFlag || !bFlag) &&
                (opcode(state.MEMWB.instr) == ADD ||
                 opcode(state.MEMWB.instr) == NAND)) {
                    
                    //previous dest reg is same as current source reg
                    if (field2(state.MEMWB.instr) == field1(state.IDEX.instr) ||
                        field2(state.MEMWB.instr) == field0(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        if((!aFlag && !bFlag) && (field2(state.MEMWB.instr) == field1(state.IDEX.instr)) &&
                           (field2(state.MEMWB.instr) == field0(state.IDEX.instr))) {
                            aFlag = 1;
                            bFlag = 1;
                            aResult = state.MEMWB.writeData;
                            bResult = state.MEMWB.writeData;
                            
                        }else{
                            if ((!bFlag) && field2(state.MEMWB.instr) == field1(state.IDEX.instr)) {
                                bFlag = 1;
                                bResult = state.MEMWB.writeData;
                            }
                            else if ((!aFlag) && field2(state.MEMWB.instr) == field0(state.IDEX.instr)){
                                aFlag = 1;
                                aResult = state.MEMWB.writeData;
                            }
                            
                        }
                    }
                }
            
            
            //PREVIOUS THREE INSTRUCTION
            if ((!aFlag || !bFlag) &&
                (opcode(state.WBEND.instr) == ADD ||
                 opcode(state.WBEND.instr) == NAND)) {
                    
                    //previous dest reg is same as current source reg
                    if (field2(state.WBEND.instr) == field1(state.IDEX.instr) ||
                        field2(state.WBEND.instr) == field0(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        
                        if((!aFlag && !bFlag) && (field2(state.WBEND.instr) == field1(state.IDEX.instr)) &&
                           (field2(state.WBEND.instr) == field0(state.IDEX.instr))){
                            aFlag = 1;
                            bFlag = 1;
                            aResult = state.WBEND.writeData;
                            bResult = state.WBEND.writeData;
                            
                        }else{
                            if ((!bFlag) && field2(state.WBEND.instr) == field1(state.IDEX.instr)) {
                                bFlag = 1;
                                bResult = state.WBEND.writeData;
                                
                            }
                            else if ((!aFlag) && field2(state.WBEND.instr) == field0(state.IDEX.instr)){
                                aFlag = 1;
                                aResult = state.WBEND.writeData;
                                
                            }
                        }
                    }
                }
            
            newState.EXMEM.aluResult = bResult + state.IDEX.offset;
            newState.EXMEM.readReg = aResult;
            
        }//END SW
        
        
        //if current opcode is beq
        else if((opcode(state.IDEX.instr)) == BEQ){
            
            aResult = state.IDEX.readRegA;
            bResult = state.IDEX.readRegB;
            
            //if previous ONE instruction
            if (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NAND) {
                
                
                //if previous dest reg is same as one of two current source registers
                if (field2(state.EXMEM.instr) == field0(state.IDEX.instr) ||
                    field2(state.EXMEM.instr) == field1(state.IDEX.instr)) {
                    
                    forwardFlag = 1;
                    
                    //If both sources equal destination
                    if((field2(state.EXMEM.instr) == field0(state.IDEX.instr)) &&
                       (field2(state.EXMEM.instr) == field1(state.IDEX.instr))){
                        aFlag = 1;
                        bFlag = 1;
                        aResult = state.EXMEM.aluResult;
                        bResult = state.EXMEM.aluResult;
                        
                    }else{
                        //if previous dest equals first source register
                        if ((!aFlag) &&field2(state.EXMEM.instr) == field0(state.IDEX.instr)) {
                            aFlag = 1;
                            aResult = state.EXMEM.aluResult;
                            
                        }
                        //if previous dest equals second source register
                        else if ((!bFlag) && field2(state.EXMEM.instr) == field0(state.IDEX.instr)){
                            bFlag = 1;
                            bResult = state.EXMEM.aluResult;
                        }
                    }
                    
                    
                }
                
            }
            
            //if previous TWO instruction
            if ((!aFlag || !bFlag) &&
                (opcode(state.MEMWB.instr) == ADD ||
                 opcode(state.MEMWB.instr) == NAND)) {
                    
                    
                    
                    if (field2(state.MEMWB.instr) == field0(state.IDEX.instr) ||
                        field2(state.MEMWB.instr) == field1(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        if((!aFlag && !bFlag) && (field2(state.MEMWB.instr) == field0(state.IDEX.instr)) &&
                           (field2(state.MEMWB.instr) == field1(state.IDEX.instr))){
                            aFlag = 1;
                            bFlag = 1;
                            aResult = state.MEMWB.writeData;
                            bResult = state.MEMWB.writeData;
                            
                        }else{
                            //if previous dest equals first source register
                            if ((!aFlag) && field2(state.MEMWB.instr) == field0(state.IDEX.instr)) {
                                aResult = state.MEMWB.writeData;
                                aFlag = 1;
                            }
                            //if previous dest equals second source register
                            else if ((!bFlag) && field2(state.MEMWB.instr) == field1(state.IDEX.instr)){
                                bResult = state.MEMWB.writeData;
                                bFlag = 1;
                            }
                        }
                        
                    }
                }
            
            //if previous THREE instruction
            
            if ((!aFlag || !bFlag) &&
                (opcode(state.WBEND.instr) == ADD ||
                 opcode(state.WBEND.instr) == NAND)) {
                    
                    
                    if (field2(state.WBEND.instr) == field0(state.IDEX.instr) ||
                        field2(state.WBEND.instr) == field1(state.IDEX.instr)) {
                        
                        forwardFlag = 1;
                        
                        if ((!aFlag && !bFlag) &&field2(state.WBEND.instr) == field0(state.IDEX.instr) &&
                            field2(state.WBEND.instr) == field1(state.IDEX.instr)) {
                            
                            aFlag = 1;
                            bFlag = 1;
                            aResult = state.WBEND.writeData;
                            bResult = state.WBEND.writeData;
                            
                        }else{
                            //if previous dest equals first source register
                            if ((!aFlag) && field2(state.WBEND.instr) == field0(state.IDEX.instr)) {
                                aResult = state.WBEND.writeData;
                                aFlag = 1;
                            }
                            //if previous dest equals second source register
                            else if ((!bFlag) && field2(state.WBEND.instr) == field1(state.IDEX.instr)){
                                bResult = state.WBEND.writeData;
                                bFlag = 1;
                            }
                            
                        }
                    }
                    
                }
            
            newState.EXMEM.aluResult = aResult - bResult;
            
            newState.EXMEM.readReg = 0;
            
        }
        
        
        
        else if((opcode(state.IDEX.instr)) == NOOP){
            newState.EXMEM.aluResult = 0;
            newState.EXMEM.readReg = 0;
            newState.EXMEM.branchTarget = 0;
        }
        
        
        else if(opcode(state.IDEX.instr) == HALT){
            newState.EXMEM.instr = state.IDEX.instr;
            newState.retired = state.retired + 1;
        }
        
        
        
        /*------------------ MEM stage ----------------- */
        
        
        
        
        //begin counting retired after initial noop
        //instructions have passed.
        if (opcode(state.WBEND.instr) != NOOP) {
            retireFlag = 1;
        }
        //will go until machine halts
        if (retireFlag) {
            newState.retired = state.retired + 1;
        }
        
        
        newState.MEMWB.instr = state.EXMEM.instr;
        
        
        if ((opcode(state.EXMEM.instr)) == BEQ) {
            if ((state.EXMEM.aluResult) == 0) {
                newState.pc = state.EXMEM.branchTarget;
                newState.EXMEM.instr = NOOPINSTRUCTION;
                newState.IDEX.instr = NOOPINSTRUCTION;
                newState.IFID.instr = NOOPINSTRUCTION;
                newState.mispreds = state.mispreds + 1;
            }
            newState.branches = state.branches + 1;
            newState.MEMWB.writeData = 0;
        }
        else if((opcode(state.EXMEM.instr)) == ADD || (opcode(state.EXMEM.instr)) == NAND){
            newState.MEMWB.writeData = state.EXMEM.aluResult;
        }
        
        else if((opcode(state.EXMEM.instr)) == LW){
            newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
        }
        
        
        else if((opcode(state.EXMEM.instr)) == SW){
            
            //If we've gone beyond memory
            if (state.EXMEM.aluResult >= 65536) {
                printf("\n\n!!!!!!!!!!!\n\n");
                printf("SW memory access out of range:\n");
                printf("%d is too large for memory allocation\n",state.EXMEM.aluResult);
                printf("\n\n!!!!!!!!!!!\n\n");
                printf("Stats before halting:\n\n");
                printf("total of %d cycles executed\n", state.cycles);
                printf("total of %d instructions fetched\n", state.fetched);
                printf("total of %d instructions retired\n", state.retired);
                printf("total of %d branches executed\n", state.branches);
                printf("total of %d branch mispredictions\n", state.mispreds);
                exit(0);
            }
            
            newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readReg;
            
            if (state.numMemory < state.EXMEM.aluResult) {
                newState.numMemory = state.EXMEM.aluResult + 1;
                newState.MEMWB.writeData = 0;
            }
            
        }
        else{
            newState.MEMWB.writeData = 0;
        }
        
        
        
        /*------------------ WB stage ----------------- */
        
        
        
        
        newState.WBEND.instr = state.MEMWB.instr;
        
        if ((opcode(state.MEMWB.instr)) == ADD || (opcode(state.MEMWB.instr)) == NAND) {
            newState.WBEND.writeData = state.MEMWB.writeData;
            newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
        }
        else if ((opcode(state.MEMWB.instr)) == LW){
            newState.reg[field0(state.MEMWB.instr)] = state.MEMWB.writeData;
            newState.WBEND.writeData = state.MEMWB.writeData;
        }
        else{
            newState.WBEND.writeData = 0;
        }
        
        
        
        
        state = newState;
        /* this is the last statement before the end of the loop. It marks the end of the cycle and updates the current
         state with the values calculated in this cycle – AKA “Clock Tick”. */
        
    }//END MAIN LOOP

    return 0;
    
}//End Function: MAIN


//Sign Extend for IFID->IDEX stage
int signExtend(int num){
    if (num & (1<<15) ){
        num -= (1<<16);
    }
    return(num);
}


int field0(int instruction){
    return( (instruction>>19) & 0x7);
}
int field1(int instruction){
    return( (instruction>>16) & 0x7);
}
int field2(int instruction){
    return(instruction & 0xFFFF);
}
int opcode(int instruction){
    return(instruction>>22);
}


void printInstruction(int instr) {
    char opcodeString[10];
    
    if (opcode(instr) == ADD) {
        strcpy(opcodeString, "add");
        
    } else if (opcode(instr) == NAND) {
        strcpy(opcodeString, "nand");
    } else if (opcode(instr) == LW) {
        strcpy(opcodeString, "lw");
    } else if (opcode(instr) == SW) {
        strcpy(opcodeString, "sw");
    } else if (opcode(instr) == BEQ) {
        strcpy(opcodeString, "beq");
    } else if (opcode(instr) == JALR) {
        strcpy(opcodeString, "jalr");
    } else if (opcode(instr) == HALT) {
        strcpy(opcodeString, "halt");
    } else if (opcode(instr) == NOOP) {
        strcpy(opcodeString, "noop");
    } else {
        strcpy(opcodeString, "data");
    }
    if(opcode(instr) == ADD || opcode(instr) == NAND){
        printf("%s %d %d %d\n", opcodeString, field2(instr), field0(instr), field1(instr));
    }else if(0 == strcmp(opcodeString, "data")){
        printf("%s %d\n", opcodeString, signExtend(field2(instr)));
    }else{
        printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
               signExtend(field2(instr)));
    }
}

void printState(stateType *statePtr){
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);
    printf("\tdata memory:\n");
    for (i=0; i<statePtr->numMemory; i++)
    {
        printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++)
    {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("\tIFID:\n");
    printf("\t\tinstruction "); printInstruction(statePtr->IFID.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
    printf("\t\tinstruction "); printInstruction(statePtr->IDEX.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
    printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
    printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB); printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
    printf("\t\tinstruction "); printInstruction(statePtr->EXMEM.instr);
    printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
    printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
    printf("\t\treadRegB %d\n", statePtr->EXMEM.readReg);
    printf("\tMEMWB:\n");
    printf("\t\tinstruction "); printInstruction(statePtr->MEMWB.instr);
    printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
    printf("\t\tinstruction "); printInstruction(statePtr->WBEND.instr);
    printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

