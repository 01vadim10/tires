/********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Program: visual
 * File: visual.c
 * Author: vzylevich
 * Created: June 25, 2013
 ********************************************************************
 * Implementation of program visual
 ********************************************************************/

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
_LOCAL DTGetTime_typ DTGetTime_1;

void _INIT visINIT(void)
{
	scalesMSG = 1;
	var = 1;
	localControl = 1;
	control = 1;
	readyStart = 1;
	modeControl = 0;
	nomCapacity = 0;
	realCapacity = 0;
	resValueTires = 0;
	tireWeightRolg6 = 0;
	tireWeightConv8 =	 0;
//	startTime = 0;
//	stopTime = 0;
	indexReset = 0;
	preStartSignal = 30;
	
	/*Technological params*/
	tireWeightMax = 0;
	tireWeightMin = 0;
	transporterSpeed = 0;
	lineCapacityMax = 0;
	lineCapacityMin = 0;
	tireWeightRolg7 = 0;
}

DATE_AND_TIME getTime(DATE_AND_TIME * DT0)
{
	DTGetTime(&DT0);
	return DT0;
}
void _CYCLIC visual(void)
{
	/*Movement of the tires on the panel(Передвижение шин на панели)*/
	sensSQSize = sizeof(sensSQ);
	for (i = 0; i < sensSQSize; i++)
		sensSQ[i] ? (tireMoving[i] = 0) : (tireMoving[i] = 1);
	DTGetTime_1.enable = 1;
	DTGetTime(&DTGetTime_1);
	if (indexReset){
		ascDT(DTGetTime_1.DT1, (UDINT) &startTime, 25);
	}
	ascDT(DTGetTime_1.DT1, (UDINT) &stopTime, 25);
}
