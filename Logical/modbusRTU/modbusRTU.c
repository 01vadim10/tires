#include <bur/plc.h>
#include <bur/plctypes.h>

#include <string.h>
#include <dvframe.h>

#ifdef _DEFAULT_INCLUDES
 #include <AsDefault.h>
#endif

/* Энергосоюз ЦП9010 */
_LOCAL FRM_xopen_typ 	mbXOpenStruct;                    
_LOCAL XOPENCONFIG   	XOpenConfigStruct;                     
_LOCAL FRM_gbuf_typ  	mbGetBufferStruct;                 
_LOCAL FRM_rbuf_typ  	mbReleaseBufferStruct;             
_LOCAL FRM_robuf_typ 	mbReleaseOutputBufferStruct;      
_LOCAL FRM_write_typ 	mbWriteStruct;                    
_LOCAL FRM_read_typ  	mbReadStruct;

_LOCAL STRING StringDevice[30], StringMode[40];

_LOCAL USINT Tb[8], Rb[61];
_LOCAL BOOL enable;


void _INIT modbusRTUINIT( void ){
	memset(&Tb, 0, sizeof(Tb));
	memset(&Rb, 0, sizeof(Rb));
	enable = 1;
	
	strcpy(StringDevice, "SS1.IF2");          
  	strcpy(StringMode, "PHY=RS485 /BD=9600 /PA=N /DB=8 /SB=1");
	
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

	mbXOpenStruct.device = (UDINT) StringDevice;
	mbXOpenStruct.mode   = (UDINT) StringMode;
	mbXOpenStruct.config = (UDINT) &XOpenConfigStruct;
	mbXOpenStruct.enable = 1;

	FRM_xopen(&mbXOpenStruct);
	//Передать вес нетто - c2h; обнулить показания веса - c0h
	Tb[0]  	= 0xff;		
   	Tb[1]  	= 0xC2;		
    Tb[2]  	= 0x01;		
    Tb[3]	= 0x03;
	Tb[4]	= 0x00;
    Tb[5]	= 0x1c;
    Tb[6]	= 0x00;
    Tb[7]	= 0x00;
}

/* Подсчет CRC, ModBus RTU */
UINT CRC(USINT *data, USINT length){
	int i, j;
  	UINT CRC = 0xFFFF, bit;

	for (i = 0; i < length; i++){
    	CRC ^= (UINT)data[i];
    	for (j = 0; j < 8 ; j++){
      		bit = CRC & 0x0001;
      		CRC >>= 1;
      		if (bit) CRC ^= 0xA001;
    	}
	}
	return CRC;
}

/* Функция Write, запись в буфер */
void Write(){
	mbGetBufferStruct.enable = 1;
    mbGetBufferStruct.ident  = mbXOpenStruct.ident;
    FRM_gbuf(&mbGetBufferStruct);
    
	if (!mbGetBufferStruct.status){		
//		Tb[0]  	= 0xff;		
//    	Tb[1]  	= 0x04;		
//      Tb[2]  	= 0x01;		
//      Tb[3]	= 0x03;
//      Tb[4]	= 0x00;
//      Tb[5]	= 0x1c;
		Tb[6]	= 0x00;
      	Tb[7]	= 0x00;
		*(UINT *)(Tb + sizeof(Tb) - 2) = CRC(Tb, sizeof(Tb) - 2);
        *(UINT *)B1 = CRC(Tb, sizeof(Tb) - 2);

		memcpy( (USINT*) mbGetBufferStruct.buffer, Tb, sizeof (Tb)); 
		state	= 1;      
		mbWriteStruct.ident  = mbXOpenStruct.ident;
        mbWriteStruct.buffer = (UDINT) mbGetBufferStruct.buffer;
        mbWriteStruct.buflng = sizeof(Tb);
        mbWriteStruct.enable = 1;
        FRM_write(&mbWriteStruct);
                    
        if (mbWriteStruct.status != 0){
       		alarm = 3; 
        	mbReleaseOutputBufferStruct.enable = 1;
            mbReleaseOutputBufferStruct.ident  = mbXOpenStruct.ident;
            mbReleaseOutputBufferStruct.buffer = (UDINT) mbGetBufferStruct.buffer;
            mbReleaseOutputBufferStruct.buflng = mbGetBufferStruct.buflng;
            FRM_robuf(&mbReleaseOutputBufferStruct);
        }
    } else alarm = 1; 
}

/* Функция Read, чтение из буфера */
void Read(){	
	USINT B[2];
 	mbReadStruct.enable = 1;
    mbReadStruct.ident 	= mbXOpenStruct.ident;
    FRM_read(&mbReadStruct);
	memset(&Rb, 0, sizeof(Rb) );

    if (!mbReadStruct.status){
   		memset(&B, 0, sizeof(B));
		memcpy(Rb, (USINT*) mbReadStruct.buffer, sizeof(Rb) );
        *(UINT *)B = CRC(Rb, sizeof(Rb) - 2);
		state	= 2;
		/* Проверка CRC */
		if( (B[0] == Rb[59]) && (B[1] == Rb[60])){
       	    state	= 3;
       	    e 		= 0;

			/* формирование аналоговых параметров */
			rs485[0].D 	= Rb[3]*256 + Rb[4];
			rs485[1].D 	= Rb[5]*256 + Rb[6];
			rs485[2].D 	= Rb[7]*256 + Rb[8];
			rs485[3].D 	= Rb[9]*256 + Rb[10];
			rs485[4].D 	= Rb[11]*256 + Rb[12];
			rs485[5].D 	= Rb[13]*256 + Rb[14];
			rs485[6].D 	= Rb[15]*256 + Rb[16];
			rs485[7].D 	= Rb[17]*256 + Rb[18];
			rs485f		= (Rb[19]*256 + Rb[20]) * 50 / 50000;
			rs485cos	= (Rb[21]*256 + Rb[22]) / 1000;
			rs485[8].D = Rb[23]*256 + Rb[24];
			rs485[9].D = Rb[25]*256 + Rb[26];
			rs485[10].D = Rb[27]*256 + Rb[28];
			rs485[11].D = Rb[29]*256 + Rb[30];
			rs485[12].D = Rb[31]*256 + Rb[32];
			rs485[13].D = Rb[33]*256 + Rb[34];
			rs485[14].D = Rb[35]*256 + Rb[36];
			rs485[15].D = Rb[37]*256 + Rb[38];
			rs485[16].D = Rb[39]*256 + Rb[40];
			rs485[17].D = Rb[41]*256 + Rb[42];
			rs485[18].D = Rb[43]*256 + Rb[44];
			rs485[19].D = Rb[45]*256 + Rb[46];
			rs485[20].D = Rb[47]*256 + Rb[48];
			rs485[21].D = Rb[49]*256 + Rb[50];
			rs485[22].D = Rb[51]*256 + Rb[52];
			rs485cosa	= (Rb[53]*256 + Rb[54]) / 1000;
			rs485cosb	= (Rb[55]*256 + Rb[56]) / 1000;
			rs485cosc	= (Rb[57]*256 + Rb[58]) / 1000;
		} else{
			e++;
			if(2 < e){
				alarm = 4;
				e = 3;
			}
		}	

     	mbReleaseBufferStruct.enable = 1;
     	mbReleaseBufferStruct.ident  = mbXOpenStruct.ident;
     	mbReleaseBufferStruct.buffer = mbReadStruct.buffer;
     	mbReleaseBufferStruct.buflng = mbReadStruct.buflng;
     	FRM_rbuf(&mbReleaseBufferStruct);
     } else alarm = 2;
}

/* Функция реализации процесса обмена по ModBus */
void mb_main(void){
	if (enable){
		state = 0;
		alarm = 0;
		Write();
		if (state == 1) Read();
		
		/* отработка аварии !alarm */
		if ((alarm > 0) && !sim){
			memset(&Rb, 0, sizeof(Rb) );
			memset(&rs485, 0, sizeof(rs485));
			rs485f = 0;
			rs485cos = rs485cosa = rs485cosb = rs485cosc = 0; 
			rs485f_St	= 6;
			rs485cos_St	= 6;
			rs485_alarm = 1;
		}
		else {
			if (rs485f < 49) rs485f_St = 3;
			else if (rs485f > 51) rs485f_St = 4;
			else rs485f_St = 0;
			rs485cos_St	= 0;
			rs485_alarm	= 0;  	
			
		}
	} else{
  		state = 0;
  		alarm = 0;
	}
}

void _CYCLIC modbusRTUCYCLIC( void ){
	mb_main();
}
