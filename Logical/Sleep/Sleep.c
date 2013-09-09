/********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Program: Sleep
 * File: Sleep.c
 * Author: VZylevich
 * Created: July 10, 2013
 ********************************************************************
 * Implementation of program Sleep
 ********************************************************************/

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

_LOCAL USINT i;
_LOCAL USINT foo;

/*Пульсирующий сигнал технологических светофоров длительностью 30 секунд*/
void timerAutoMode(void)
{
	if (sensSB4)
	{
		if(pulseSignal)
		{
			TON_count.IN = !TON_count.Q;
			TON_count.PT = 30000;
			TON(&TON_count);
			if(TON_count.ET < 30000)
			{
				pulseSignal = 1;
				TON_01.IN = !TON_01.Q;
				TON_01.PT = 1000;
				TON(&TON_01);
				count = TON_count.ET / TON_01.PT;
				(count % 2) ? (sensSB4Light = 0) : (sensSB4Light = 1);
			} else 
			{
				sensSB4Light = 1;
				pulseSignal = 0;
			}
		}
	}
}

/*Моделирование передвижения шин по конвейеру*/
void tireMoving()
{
	if(foo)
	{
		if(i < sizeof(sensSQ))
		{
			if(i > 0) sensSQ[i-1] = 0;
			sensSQ[i] = 1;
			(i % 2) ? (TON0.IN = 1) : (TON0.IN = 0);
			TON0.PT = 5000;
			TON(&TON0);
			i++;
		}else i = 0;
	}else for(i = 0; i < sizeof(sensSQ); i++)
			sensSQ[i] = 0;
}

void _CYCLIC Sleep(void)
{
	timerAutoMode();
	tireMoving();
}
