/********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Program: weightControl
 * File: weightControl.c
 * Author: VZylevich
 * Created: July 15, 2013
 ********************************************************************
 * Implementation of program weightControl
 ********************************************************************/

#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
void _INIT inity(void)
{
	a = 0;
	b = 0;
}

TIME rotationPeriod(void)
{
	/*Определяем время оборота печи*/
	if (sensSQ70){
		rotation = 1;
		time ? (period = time) : (period = 0);
	}
	if (!sensSQ70 && rotation)
		time = clock_ms();
	
	return period;
}

/*Загрузка печи*/
UDINT furnaceLoading(TIME rotationFurnace)
{
	Qrot = (nomCapacity * (UDINT) rotationFurnace) / hour;
	return Qrot;
}

/*Производительность сжигания*/
UDINT capacityBurning(void)
{
	return (nomCapacity * 1000)/ hour;
}

/*Задержка для успокоения весов*/
void delay(USINT sensor)
{
	if (sensor){
		TON_atten.IN = !TON_atten.Q;
		TON_atten.PT = attenuation;
		TON(&TON_atten);
	}
}
void _CYCLIC weightControl(void)
{
	tRot = rotationPeriod();	//Время одного оборота печи
	if (tRot)
		kgRotation = furnaceLoading(tRot); //Определяем кол-во килограмм шин необходимых для загрузки печи за один оборот
	kgSec = capacityBurning();	//Производительносто подачи шин на сжигание в секунду
	delay(sensSQ70);
	(TON_atten.ET == attenuation) ? (weightMeasure = weight) : (weightMeasure = 0);//Если время успокоения прошло, присвоить значение веса
	if (weightMeasure && !sensSQ[7])	//Если известен вес и на рольганге 7 пусто - включить электродвигатель рольганга
		elmotorM[6] = 1;
	if (weightMeasure && sensSQ[7])		//Если известен вес и на рольганге 7 есть шина, передать измерен. знач. веса шины в загрузку
		loadingRollg7 = weightMeasure;
	
}
