/********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Program: exampleCplusplus
 * File: exampleCplusplus.c
 * Author: VZylevich
 * Created: August 12, 2013
 ********************************************************************
 * Implementation of program exampleCplusplus
 ********************************************************************/

#include <bur/plctypes.h>
//#include <stdafx.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
//#include <conio.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

unsigned char CRCMaker(unsigned char b_input, unsigned char b_CRC) 
{ 
	_asm
	{ 
		mov al,b_input 
		mov ah,b_CRC 
		mov cx,8 
		mod1:  rol al,1 
		rcl ah,1 
		jnc mod2 
		xor ah,69h 
			mod2:  dec cx 
		jnz mod1 
		mov b_CRC,ah 
	} 
	return b_CRC;
}


void _CYCLIC exampleCplusplus(void)
{
	//using namespace std;
	unsigned char crc=0x00;
	unsigned char request[3]={0x01, 0xC2, 0x00}; //Запрос передать вес нетто, команда 0xC2
	
	for (int i=0; i<=2; i++)
		{
		crc=CRCMaker(request[i],crc); //расчёт контрольной суммы для посылаемого запроса
	}
	//Добавив разделители (байт FF в начале и 2 байта FF в конце сообщения), получаем запрос - байтовый массив длинной 5 байт, массив нужно передать ТВ003/05H 
	cout<< "Request to TB3: "<<std::hex<<255<<" "<<int(request[0])<<" "<<int(request[1])<<" "<<int(crc)<<" "<<255<<" "<<255<<endl; 


		//Наример ТВ003/05H ответил так FF 01 C2 40 14 00 B0 D2 FF FF
		//Проверим принятое ли сообщение без ошибок
		unsigned char answer[7]={0x01, 0xC2, 0x40, 0x14, 0x00, 0xB0, 0xD2};
	crc=0x00;
	cout<< "Answer from TB3: "<<std::hex<<255<<" ";
	for (int i=0; i<=6; i++)
		{
		cout<<int(answer[i])<<" ";
			crc=CRCMaker(answer[i],crc); //проверка контрольной суммы сообщения, полученного от ТВ003/05H
	}
	cout<<255<<" "<<255<<endl;

	if (int(crc)==0)
	{
		cout<< "Answer from TB3 received successfully!"<<endl; //принято без ошибок
	}
	else
	{
		cout<< "Answer from TB3 received with errors!"<<endl; //принято с ошибками
		};

	getch();
	return 0;
}
