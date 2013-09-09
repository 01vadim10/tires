#include <bur/plc.h>
#include <bur/plctypes.h>
#include <string.h>
#include "hp.h"
#include <dvframe.h>

/*********************** Объявление GLOBAL и LOCAL переменных *****************************/

/*_GLOBAL HP_data RunHP;	 структура для контроллера 
_LOCAL HP_data UsrHP;	 структура для панели

_GLOBAL FRM_xopen_typ FrameXOpenStruct;                    
_GLOBAL XOPENCONFIG   XOpenConfigStruct;                     
_GLOBAL FRM_gbuf_typ  FrameGetBufferStruct;                 
_GLOBAL FRM_rbuf_typ  FrameReleaseBufferStruct;             
_GLOBAL FRM_robuf_typ FrameReleaseOutputBufferStruct;      
_GLOBAL FRM_write_typ FrameWriteStruct;                    
_GLOBAL FRM_read_typ  FrameReadStruct;                      
_GLOBAL UINT   		 StatusOpen, StatusWrite, StatusRead, StatusGetBuffer, StatusReleaseBuffer;     
_GLOBAL UINT   		 SendBufferLength, ReadBufferLength, StatusReleaseOutputBuffer;
_GLOBAL UDINT  		 Ident;                                       
_GLOBAL UDINT* 		 SendBuffer;                                 
_GLOBAL UDINT* 		 ReadBuffer;                                 
_GLOBAL USINT  		 T[8], T1[8],R[13], Error, leng, step;
_GLOBAL BOOL 		    status_read;
_GLOBAL REAL 		    F[5], Fr[5], U[5], I[5];
//_GLOBAL STRING   		 StringDevice[30], StringMode[30]; */

_GLOBAL   REAL 		    F[6], Fr[6], U[6], I[6],Con_Fr[6]; 
_LOCAL   BOOL 		    status_read;
_LOCAL  UDINT  		 Ident;                                       
_LOCAL  UDINT* 		 SendBuffer;                                 
_LOCAL  UDINT* 		 ReadBuffer;        
_LOCAL  FRM_xopen_typ FrameXOpenStruct;    
_LOCAL XOPENCONFIG   XOpenConfigStruct;                     
_LOCAL  FRM_gbuf_typ  FrameGetBufferStruct;                 
_LOCAL FRM_rbuf_typ  FrameReleaseBufferStruct;             
_LOCAL  FRM_robuf_typ FrameReleaseOutputBufferStruct;      
_LOCAL  FRM_write_typ FrameWriteStruct;                    
_LOCAL  FRM_read_typ  FrameReadStruct;                      
_LOCAL UINT   		 StatusOpen, StatusWrite, StatusRead, StatusGetBuffer, StatusReleaseBuffer;     
_LOCAL UINT   		 SendBufferLength, ReadBufferLength, StatusReleaseOutputBuffer;
_LOCAL USINT  		 T[8], T1[8],R[13], Error, leng, step;
_GLOBAL   HP_data   RunHP;	
_LOCAL    HP_data   UsrHP;	
_LOCAL STRING  		 StringDevice[30], StringMode[30];
//_GLOBAL  UINT transporterSpeed;
void _INIT Init()
{
	memset(&T, 0, sizeof(T));
	memset(&R, 0, sizeof(R));
	memset(&F, 0, sizeof(F));
	memset(&Fr, 0, sizeof(Fr));
	memset(&U, 0, sizeof(U));
	memset(&I, 0, sizeof(I));
	memset(&T1, 0, sizeof(T1));
	status_read = 0;
	step = 1;
	leng = sizeof(T);
	Error = 0;
	strcpy(StringDevice, "SS1.IF2");          
	strcpy(StringMode, "PHY=RS485 /BD=9600 /PA=E /DB=8 /SB=1");

	XOpenConfigStruct.idle     = 4;
	XOpenConfigStruct.delimc   = 0;
	XOpenConfigStruct.delim[0] = 0;
	XOpenConfigStruct.delim[1] = 0;
	XOpenConfigStruct.tx_cnt   = 2;
	XOpenConfigStruct.rx_cnt   = 2;
	XOpenConfigStruct.tx_len   = 256;
	XOpenConfigStruct.rx_len   = 256;
	XOpenConfigStruct.argc     = 0;
	XOpenConfigStruct.argv     = 0;

	FrameXOpenStruct.device = (UDINT) StringDevice;
	FrameXOpenStruct.mode   = (UDINT) StringMode;
	FrameXOpenStruct.config = (UDINT) &XOpenConfigStruct;
	FrameXOpenStruct.enable = 1;

	FRM_xopen(&FrameXOpenStruct);
	StatusOpen = FrameXOpenStruct.status;             
	Ident = FrameXOpenStruct.ident;                   

	if (StatusOpen != 0)                                    
	{
		Error = 1;                                
	}
}

UINT CRC(USINT *data, int length)
{
	int i, j;
	UINT CRC = 0xFFFF, bit;

	for (i = 0; i < length; i++)
	{
		CRC ^= (UINT)data[i];

		for (j = 0; j < 8 ; j++)
		{
			bit = CRC & 0x0001;
			CRC >>= 1;
			if (bit) CRC ^= 0xA001;
		}
	}
	return CRC;
}
/*функция записи команды для ПЧ при записи и чтении по Modbus RTU */
void Write(USINT s,USINT Icode,USINT A1,USINT A2,USINT D1,USINT D2)
{
	FrameGetBufferStruct.enable = 1;
	FrameGetBufferStruct.ident  = Ident;
	FRM_gbuf(&FrameGetBufferStruct);           
	SendBuffer = (UDINT*) FrameGetBufferStruct.buffer; 
	SendBufferLength = FrameGetBufferStruct.buflng; 
	StatusGetBuffer  = FrameGetBufferStruct.status;  
                        
	if (StatusGetBuffer == 0)                             
	{		
		T[0]  = s;	    /*номер инвертора*/	
		T[1]  = Icode;  /* код инструкции 0x03 или 0х06*/		
		T[2]  = A1;	 /*начальный адрес Н/адрес регистра H*/	
		T[3]  = A2;	 /*начальный адрес L/адрес регистра L*/ 	
		T[4]  = D1;	 /*количество адресов Н/данные Н*/
		T[5]  = D2;	 /* количество адресов L/данные L*/
		T[6]  = 0x00;	 /*контрольная сумма L*/
		T[7]  = 0x00;	 /* контрольная сумма H*/
			        
		*(UINT *)(T + sizeof(T) - 2) = CRC(T, sizeof(T) - 2);
		memcpy(SendBuffer, T, sizeof (T)); 
                        
		status_read = 1;
                        
		FrameWriteStruct.ident  = Ident;
		FrameWriteStruct.buffer = (UDINT) SendBuffer;
		FrameWriteStruct.buflng = sizeof (T);
		FrameWriteStruct.enable = 1;
		FRM_write(&FrameWriteStruct);            
		StatusWrite = FrameWriteStruct.status;  
                        
		if (StatusWrite != 0)                          
		{
			FrameReleaseOutputBufferStruct.enable = 1;
			FrameReleaseOutputBufferStruct.ident  = Ident;
			FrameReleaseOutputBufferStruct.buffer = (UDINT) SendBuffer;
			FrameReleaseOutputBufferStruct.buflng = SendBufferLength;
			FRM_robuf(&FrameReleaseOutputBufferStruct); 
			StatusReleaseOutputBuffer = FrameReleaseOutputBufferStruct.status;
			Error = 3;                    

			if (StatusReleaseOutputBuffer != 0) 
			{
				Error = 4;            
			}
		}
	}
	else
	{
		Error = 2;                            
	}
}
/*функция чтения ПЧ при чтении частоты,тока и напряжения (03)*/
void Read()
{		
	USINT rr = 0;
	USINT B[2];
	FrameReadStruct.enable = 1;
	FrameReadStruct.ident = Ident;
	
	FRM_read(&FrameReadStruct);   
	
	ReadBuffer = (UDINT*) FrameReadStruct.buffer;    
	ReadBufferLength = FrameReadStruct.buflng;       
	StatusRead = FrameReadStruct.status;             
		
	if (StatusRead == 0)                              
	{
		memset(&B, 0, sizeof(B));
		memcpy(R, ReadBuffer, sizeof(R)); 
		*(UINT *)B = CRC(R, sizeof(R) - 2);
		if ((B[0] == R[11])&&(B[1] == R[12]))
		{		
			rr = R[0];
			F[rr]  = 256*R[3] + R[4];         
			I[rr]  = 256*R[5] + R[6];         
			U[rr]  = 256*R[7] + R[8]; 
              
		} 
		FrameReleaseBufferStruct.enable = 1;
		FrameReleaseBufferStruct.ident  = Ident;
		FrameReleaseBufferStruct.buffer = (UDINT) ReadBuffer;
		FrameReleaseBufferStruct.buflng = ReadBufferLength;
		FRM_rbuf(&FrameReleaseBufferStruct);      
		StatusReleaseBuffer = FrameReleaseBufferStruct.status;  
                  
		if (StatusReleaseBuffer != 0)          
		{
			Error = 5;                     
		}
	}
	else
	{
		Error = 6; 
	}   

}
/*функция чтения ПЧ при записи заданной частоты (06)*/
void Read_W()
{		
	USINT rr = 0;
	USINT B[2];
	FrameReadStruct.enable = 1;
	FrameReadStruct.ident = Ident;
	
	FRM_read(&FrameReadStruct);   
	
	ReadBuffer = (UDINT*) FrameReadStruct.buffer;    
	ReadBufferLength = FrameReadStruct.buflng;       
	StatusRead = FrameReadStruct.status;             
		
	if (StatusRead == 0)                              
	{
		memset(&B, 0, sizeof(B));
		memcpy(R, ReadBuffer, sizeof(R)); 
		*(UINT *)B = CRC(R, sizeof(R) - 2);
		if ((B[0] == R[11])&&(B[1] == R[12]))
		{		
			
			rr = R[0];
			Fr[rr] = 256*R[4] + R[5]; 
		} 
		FrameReleaseBufferStruct.enable = 1;
		FrameReleaseBufferStruct.ident  = Ident;
		FrameReleaseBufferStruct.buffer = (UDINT) ReadBuffer;
		FrameReleaseBufferStruct.buflng = ReadBufferLength;
		FRM_rbuf(&FrameReleaseBufferStruct);      
		StatusReleaseBuffer = FrameReleaseBufferStruct.status;  
                  
		if (StatusReleaseBuffer != 0)          
		{
			Error = 5;                     
		}
	}
	else
	{
		Error = 6; 
	}   
}
/*функция чтения ПЧ по Modbus RTU */
void Read_R()
{
	if (step <= 6)
	{
		if (!status_read) 
		{	T1[1]=0x03;
			T1[2]=0x00;
			T1[3]=0xc8;
			T1[4]=0x00;
			T1[5]=0x04;
			Write(step, T1[1] , T1[2],T1[3],T1[4],T1[5]);
		}
		else
		{
			Read();
			status_read = 0;
				  
			RunHP.in.Con1_F = F[1]*0.01; 	
			RunHP.in.Con1_U = U[1]*0.1;
			RunHP.in.Con1_I = I[1]*0.1;		
	
			RunHP.in.Con2_F = F[2]*0.01;
			RunHP.in.Con2_U = U[2]*0.1;
			RunHP.in.Con2_I = I[2]*0.1;
	
			RunHP.in.Con3_F = F[3]*0.01;
			RunHP.in.Con3_U = U[3]*0.1;
			RunHP.in.Con3_I = I[3]*0.1;
	
			RunHP.in.Con4_F = F[4]*0.01;		
			RunHP.in.Con4_U = U[4]*0.1;		
			RunHP.in.Con4_I = I[4]*0.1;
			
			RunHP.in.Con5_F = F[5]*0.01;		
			RunHP.in.Con5_U = U[5]*0.1;		
			RunHP.in.Con5_I = I[5]*0.1;
			
			RunHP.in.Con6_F = F[6]*0.01;		
			RunHP.in.Con6_U = U[6]*0.1;		
			RunHP.in.Con6_I = I[6]*0.1;
			step++;
		}
	} 
	else step = 1;
	
}  
	/*функция записи Fr в ПЧ по Modbus RTU */
void Write_Fr()
{   Con_Fr[1]=RunHP.out.Con1_Fr;
	Con_Fr[2]=RunHP.out.Con2_Fr;
	Con_Fr[3]=RunHP.out.Con3_Fr;
	Con_Fr[4]=RunHP.out.Con4_Fr;
	Con_Fr[5]=RunHP.out.Con5_Fr;
	Con_Fr[6]=RunHP.out.Con6_Fr;
	if (step <= 6)
	{
		if (!status_read) 
		{	T1[1]=0x06;
			T1[2]=0x00;
			T1[3]=0x0d;
			T1[4]=Con_Fr[step]*100 / 256;
			T1[5]=Con_Fr[step]*100-T1[4]*256;
				
			Write(step, T1[1] , T1[2],T1[3],T1[4],T1[5]);
		}
		else
		{
			Read_W();
			status_read = 0;
			RunHP.in.Con1_Fr = Fr[1]*0.01;
			RunHP.in.Con2_Fr = Fr[2]*0.01;
			RunHP.in.Con3_Fr = Fr[3]*0.01;
			RunHP.in.Con4_Fr = Fr[4]*0.01;
			RunHP.in.Con5_Fr = Fr[5]*0.01;
			RunHP.in.Con6_Fr = Fr[6]*0.01;
			step++;
		}
	}
	else step = 1;
	
}  

void _CYCLIC run()
{
	UsrHP.in=RunHP.in;
	//RunHP.out.Con1_Fr=transporterSpeed;
	Write_Fr();
	Read_R();
}  
