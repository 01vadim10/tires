/********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Program: spreading
 * File: spreading.c
 * Author: VZylevich
 * Created: July 16, 2013
 ********************************************************************
 * Implementation of program spreading
 ********************************************************************/

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

void _INIT init()
{
	n = 1;
	spreadingStart = 0;
	spreadingStop = 0;
}
/*Задержка времени для растягивания шины */
TIME spreadingDelay()
{
	TON_spreading.IN = !TON_spreading.Q;
	TON_spreading.PT = tp;
	TON(&TON_spreading);
	return TON_spreading.ET;
}

/*Проверка загрузки рольганга*/
INT checkLoadTires(int i, int k)
{
	if(sensSQ[i] & !sensSQ[k])
		rolgMoving[k-1] = 1;
	return rolgMoving[k-1];
}
/*Участок растаскивания шин*/
void spreadingPlot(int prev, int current)
{
	if(!sensSQ[current] && checkLoadTires(prev, current))
	{
		elmotorM[current-1] = 1;
		TON_spreading_repeat.IN = !TON_spreading_repeat.Q;
		TON_spreading_repeat.PT = tp2;
		TON(&TON_spreading_repeat);
		if(TON_spreading_repeat.ET >= tp2)
		{
			spreadingStart = 1;
			spreadingStop = 0;
			TON_spreading.ET = 0;
		}
		if(!spreadingStop)
		{
			elmotorM[prev-1] = 1;
			spreadingStart = 1;
		} else spreadingStart = 0;
	} else 
	{
		elmotorM[current-1] = 0;
		rolgMoving[current-1] = 0;
		spreadingStart = 0;
	}
	if(spreadingStart && (TON_spreading.ET < tp))
	{	
		TON_spreading.IN = !TON_spreading.Q;
		TON_spreading.PT = tp;
		TON(&TON_spreading);
	}else 
	{
		elmotorM[prev-1] = 0;
		spreadingStop = 1;
	}
}

void _CYCLIC spreading(void)
{
	/*Включить рольганг, если шины загружены и растаскивать, пока не включится SQ22*/
	if(!sensSQ[1] & checkLoadTires(0, 1))
		elmotorM[0] = 1;
	else 
	{
		elmotorM[0] = 0;
		rolgMoving[0] = 0;
	}
	
	rolgangCycle = sizeof(rolgMoving);
	if((n >= rolgangCycle) )
		n = 1;
	if(n < rolgangCycle)
	{
		spreadingPlot(n, n+1);
		if(sensSQ[n+1]) 
		{
			n++;
			spreadingStop = 0;
			TON_spreading.ET = 0;
		}
	}
}
