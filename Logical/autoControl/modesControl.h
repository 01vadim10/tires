/********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Program: TechProcess
 * File: modesControl.h
 * Author: VZylevich
 * Created: July 05, 2013
 *******************************************************************/

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

/*Количество датчиков местного управления*/
USINT amountModeControl()
{
	return sizeof(sensRemoteControl);
}

USINT amountSensAlarm()
{
	return sizeof(sensSQAlarm);
}

/*Проверка условий останова автоматического режима*/
BOOL checkAutoStop(USINT array[], USINT numOfElements, BOOL onOff)
{
	for (i = 0; i < numOfElements; i++)
	{
		if (array[i] == onOff)
		{
			checkStop = 0;
			sensSB4 = 0;
			sensSB4Light = 0;
			count = 0;
			TON_count.IN = 0;
			TON_count.ET = 0;
			TON_count.StartTime = 0;
			break;
		}
		else checkStop = 1;
	}
	return checkStop;
}

/**/
int checkManMode()
{
	ssModeControl = amountModeControl();
	if (sensHL12 == 1)
	{
		for (i = 0; i < ssModeControl; i++)
			sensLocalControl[i] = 1;
		manMode = 1;
		if (checkAutoMode())
		{
			for (i = 0; i < ssModeControl; i++)
				sensRemoteControl[i] = 0;
			sensSB4 = 0;
			rControl = 0;
			autoMode = 0;	
		}
	} else{
		for (i = 0; i < ssModeControl; i++)
			sensLocalControl[i] = 0;
		manMode = 0;
	}
	return manMode;
}

/**/
int checkAutoMode()
{
	rControl = checkAutoStop(sensRemoteControl, amountModeControl(), 0);
	sensAlarm = checkAutoStop(sensSQAlarm, amountSensAlarm(), 1);
	
	if (sensSB4 && rControl && sensAlarm)
	{
		(TON_count.ET < 30000) ? (pulseSignal = 1) : (pulseSignal = 0);
		autoMode = 1;
	}
	else 
	{
		sensSB4Light = 0;
		count = 0;
		TON_count.IN = 0;
		TON_count.ET = 0;
		TON_count.StartTime = 0;
		autoMode = 0;
	}
	return autoMode;
}

/*int autoMode()
{
	sensSQ[0] ? 
	return 0;
}*/