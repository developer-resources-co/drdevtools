//=============================================================================
//  control.h: slave control header
//=============================================================================

#ifndef DRMON_control_H
#define DRMON_control_H

//=============================================================================

#include "menu.hpp"

//=============================================================================

extern int slaveRunning;
extern int slaveUpdate;
extern int slaveStatus;
extern FLAG ((*runMenuRoutines[])(_menuItem *,_object *,int choice));
extern char *runMenuItems[];

extern uint runMode;
extern uint stepMode;

//==============================================================================

enum
{
	RM_WITHUPDATES,
	RM_NOUPDATES
};

enum
{
	SM_ASSEMBLY,
	SM_SOURCE
};

//==============================================================================

void
DoSlaveRun(void);

void
DoSlaveRunWithUpdates(void);

void
DoSlaveStop(void);

void
SlaveRunNoUpdate(void);

void
SlaveStep(void);

void
SlaveStepOver(void);

void
SlaveStepSource(void);

void
SlaveStepOverSource(void);

//=============================================================================

#endif

//=============================================================================
