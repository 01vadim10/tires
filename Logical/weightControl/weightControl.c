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
	/*���������� ����� ������� ����*/
	if (sensSQ70){
		rotation = 1;
		time ? (period = time) : (period = 0);
	}
	if (!sensSQ70 && rotation)
		time = clock_ms();
	
	return period;
}

/*�������� ����*/
UDINT furnaceLoading(TIME rotationFurnace)
{
	Qrot = (nomCapacity * (UDINT) rotationFurnace) / hour;
	return Qrot;
}

/*������������������ ��������*/
UDINT capacityBurning(void)
{
	return (nomCapacity * 1000)/ hour;
}

/*�������� ��� ���������� �����*/
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
	tRot = rotationPeriod();	//����� ������ ������� ����
	if (tRot)
		kgRotation = furnaceLoading(tRot); //���������� ���-�� ��������� ��� ����������� ��� �������� ���� �� ���� ������
	kgSec = capacityBurning();	//������������������ ������ ��� �� �������� � �������
	delay(sensSQ70);
	(TON_atten.ET == attenuation) ? (weightMeasure = weight) : (weightMeasure = 0);//���� ����� ���������� ������, ��������� �������� ����
	if (weightMeasure && !sensSQ[7])	//���� �������� ��� � �� ��������� 7 ����� - �������� ���������������� ���������
		elmotorM[6] = 1;
	if (weightMeasure && sensSQ[7])		//���� �������� ��� � �� ��������� 7 ���� ����, �������� �������. ����. ���� ���� � ��������
		loadingRollg7 = weightMeasure;
	
}
