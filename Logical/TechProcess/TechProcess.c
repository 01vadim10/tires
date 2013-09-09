/********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Program: TechProcess
 * File: TechProcess.c
 * Author: VZylevich
 * Created: July 02, 2013
 ********************************************************************
 * Implementation of program TechProcess
 ********************************************************************/
#include <bur/plctypes.h>
#include <standard.h>
#include "modesControl.h"


#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

void _CYCLIC TechProcess(void)
{
	if (checkManMode())
	{
		timeStruct.second = 0;
		foo = 0;
	}
	else if (checkAutoMode())
	{
		
	}
}
