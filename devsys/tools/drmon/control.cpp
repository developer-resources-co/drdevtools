//=============================================================================
// control.c: slave control code(this is where it all happens!)
// This is the lowest level of general slave control. The next layer down
// is platform specific, and is labeled "SLIOxx.C" where xx = processor #
// i.e. slio816.c or slio68.c
//=============================================================================

//== C =========================================================================
#include	<dos.h>

//== Application ===============================================================

#include	"base.hpp"
#include	"global.hpp"
#include	"config.hpp"
#include	"control.hpp"
#include	"slaveio.hpp"
#include	"list.hpp"
#include	"message.hpp"
#include	"input.hpp"
#include	"listrect.hpp"
#include	"break.hpp"
#include	"error.hpp"
#include	"reg.hpp"
#include	"dis.hpp"
#include	"general.hpp"
#include	"drmon.hpp"
#include	"sld.hpp"

//==============================================================================

int slaveUpdate = boolean::TRUE;
int slaveRunning = boolean::FALSE;
uint runMode = RM_WITHUPDATES;
uint stepMode = SM_SOURCE;

//=============================================================================

void
DoSlaveRunWithUpdates(void)
{
	int i;
	_breakList *bPtr;
	modeUpdate = boolean::TRUE;
	if(slaveRunning)
		return;
	if((bPtr = FindBreak(GetReg(REG_INSTRUCTIONPOINTER))) != 0)
	 {
		StepOverBreak(bPtr->addr,bPtr->inst);
		delay(10);								// note: this must be here to give the slave time to trace the instruction
	 }
	SlaveRun();
	slaveRunning = boolean::TRUE;				// kts 6/18/92
}

//=============================================================================

void
SlaveRunNoUpdate(void)
{
	int i;
	_breakList *bPtr;
	modeUpdate = slaveUpdate = boolean::FALSE;
	if(slaveRunning)
		return;
	if((bPtr = FindBreak(GetReg(REG_INSTRUCTIONPOINTER))) != 0)
	 {
		StepOverBreak(bPtr->addr,bPtr->inst);
		delay(10);								// note: this must be here to give the slave time to trace the instruction
	 }
	SlaveRun ();
	slaveRunning = boolean::TRUE;				// kts 6/18/92
}

//============================================================================

void
DoSlaveRun(void)
{
	if(runMode == RM_WITHUPDATES)
		DoSlaveRunWithUpdates();
	if(runMode == RM_NOUPDATES)
		SlaveRunNoUpdate();
}

//============================================================================

void
SlaveStep(void)
{
	_breakList *bPtr;
	if(slaveRunning)
		return;
	if((bPtr = FindBreak(GetReg(REG_INSTRUCTIONPOINTER))) != 0)
	 {
		StepOverBreak(bPtr->addr,bPtr->inst);
		delay(10);								// note: this must be here to give the slave time to trace the instruction
	 }
	else
    	SingleStep ();
}

//============================================================================

void
SlaveStepOver(void)
{
	unsigned long addr,len;
	char xferBuffer[10];
	char textBuffer[160];
	UWORD instruction;
	_breakList *bPtr;

	if(slaveRunning)
		return;

    GetRegsFromSlave(regs);		// ensure registers are current, kts 5/16/92

	addr = GetReg(REG_INSTRUCTIONPOINTER);
	bPtr = FindBreak((ULONG)addr);
	if(bPtr)
	 {
		StepOverBreak(bPtr->addr,bPtr->inst);
		delay(10);								// note: this must be here to give the slave time to trace the instruction
	 }
	ReadSlaveData(addr,xferBuffer,10);
#ifdef GENESIS
	if(bPtr)
		*(unsigned int *)xferBuffer = SwapWord(bPtr->inst);
	instruction = SwapWord(*((unsigned int *)xferBuffer));
	if (( instruction & 0xff00 ) == 0x6100 || (instruction & 0xffc0 )== 0x4e80 || (instruction & 0xfff0 )== 0x4e40)
	 {
#endif
#ifdef SNES
	if(bPtr)
		*(unsigned int *)xferBuffer = bPtr->inst;
	instruction = *((ubyte*)xferBuffer);
	if ( instruction == 0x20 || instruction == 0x02 || instruction == 0x22 || instruction ==  0xfc)
	 {
#endif
	 	printCheckForSymbol = boolean::FALSE;
		len = Disassem(addr,(char far *)xferBuffer,(char far *)textBuffer,2);
		delay(10);
		AddBrkPt(addr+len,1,NULL);
		delay(10);
		if(runMode == RM_NOUPDATES)
			modeUpdate = slaveUpdate = boolean::FALSE;
		SlaveRun();
	 }
	else
	 {
		delay(10);
		SingleStep();
	 }
}

//============================================================================

void
SlaveStepSource(void)
{
	if(stSld)
		do
	 	 {
			SlaveStep();
			delay(10);								// note: this must be here to give the slave time to trace the instruction
    		GetRegsFromSlave(regs);		// ensure registers are current, kts 5/16/92
			stSld->ReSyncSource(GetReg(REG_INSTRUCTIONPOINTER));
	 	 }
		while(!stSld->ExactMatch());
	else
		SlaveStep();
}

//============================================================================

void
SlaveStepOverSource(void)
{
	if(stSld)
		do
	 	 {
			SlaveStepOver();
			delay(10);								// note: this must be here to give the slave time to trace the instruction
    		GetRegsFromSlave(regs);		// ensure registers are current, kts 5/16/92
			stSld->ReSyncSource(GetReg(REG_INSTRUCTIONPOINTER));
	 	 }
		while(!stSld->ExactMatch());
	else
		SlaveStepOver();
}

//============================================================================

