/**************************************************************************
  FILE NAME:   VERDE.C
  MODULE NAME:
  PROGRAMMER:
  DESCRIPTION:
  REVISION:
 **************************************************************************/
	  /*==========================================*
	   *         I N T R O D U C T I O N          *
	   *==========================================*/
	  /*==========================================*
	   *             I N C L U D E S              *
	   *==========================================*/
/*#define DEF_INTERFAC*/

#include "project.def"
#include INTERFAC_H
#include DEFINE_H
#include DEBUG_H
#include MESSAGE_H
#include UTILS_H
#include ERRORS_H
#include FILES_H
#include TRANS_H
#include SLIPS_H
#include HOSTCOMM_H
#include MAINLOOP_H
#include SMART_H
#include PROTOCOL_H
#include VERDE_H
#include SECURITY_H
#include EMVDEF_H
#include EMVDEFIN_H
#include EMVIF_H

extern char ATS_Count_c[3];
extern char seq_no_c[5];                                       
extern boolean DEBUG_MODE;
extern char card_no[20];
extern char ATS_Count_c[3];
extern char glAmount_Req_Type;
extern char POS_Type;			/* Default axess secili. 0:Axess  1:Akbank */

boolean VERDE_TRANSACTION;	/* 1: VERDE transaction 0: Normal Transaction */
extern struct Transaction_Receive		d_Transaction_Receive;
char verde_amount[14];
char verde_discount[14];
//bm 15.12.2004 char verde_amount1[14];

#define VERDE_PORT      UART_A
#define VERDE_BAUD      9600
#define VERDE_DATA_BITS 8
#define VERDE_PARITY    'n' 
const int TIMEOUT_VERDE = 10;
const int VERDE_ACK_TIMEOUT = 500;

boolean EXTERNAL_PINPAD;

extern char master_key_no;
extern char working_key[16];

extern int tran_type;	/* 06_07 */

extern char v_amount_surcharge[14];	/* 06_08 */
extern struct Void_Send	d_Void_Send;/* 06_08 */
extern char card_info[50];
extern char exp_date[5];
extern char cvv_2[4];
//bm 15.12.2004  extern char card_info1_name[25];
//bm 15.12.2004  extern char card_info1_surname[25];
extern char remained_chip_data[17];


/********************** FUCNTIONS ***********************/

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Verde_Init_Port
 *
 * DESCRIPTION	: set the Pinpad ports
 *
 * NOTES		:
 *
 * RETURN		:	R.›.÷. 19/10/2002
 * ------------------------------------------------------------------------ */
void Verde_Init_Port(void)
{
	
	comm_param UartPrm_VerdePort;

    UartPrm_VerdePort.baud_rate = VERDE_BAUD;
    UartPrm_VerdePort.numbits   = VERDE_DATA_BITS;
    UartPrm_VerdePort.parity    = VERDE_PARITY;
    UartPrm_VerdePort.rts_cts   = FALSE;
    UartPrm_VerdePort.dtr_mode  = FALSE;

	Cpuhw_SwitchTo (_PINPAD);							/* select the verde port */

	/* 06_07 lipman'in onerisi uzerine init oncesi flust eklendi */
	Uart_Flush (VERDE_PORT);

	Uart_Init (VERDE_PORT, &UartPrm_VerdePort);

}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Verde_SendControlChar
 *
 * DESCRIPTION	: Sends the control characters using verde port
 *
 * NOTES		:
 *
 * RETURN		:	R.›.÷. 19/10/2002
 * ------------------------------------------------------------------------ */
byte Verde_SendControlChar(byte Cntrl_char)
{
	char lbtemp[50];
	char temp_buffer[5];

	Verde_Init_Port();
	
	/* 06_07 flus init islemi oncesinde yapilmisti */
	/*if(Uart_Flush (VERDE_PORT)!= UART_OK)
	{
		/* 06_04 */
	/*	if (DEBUG_MODE) Slips_PrinterStr("\n\nUart_Flush HATASI\n\n");
		return(STAT_NOK);
	}*/

	memset(temp_buffer, 0, sizeof(temp_buffer));
	switch(Cntrl_char)
	{
	case ACK:
		temp_buffer[0] = 0x06;
		temp_buffer[1] = 0;
		/* 06_04 */
		if (DEBUG_MODE) Slips_PrinterStr("\n ACK gonderildi\n");
		break;
	case NAK:
		temp_buffer[0] = 0x15;
		temp_buffer[1] = 0;
		/* 06_04 */
		if (DEBUG_MODE) Slips_PrinterStr("\n NAK gonderildi\n");
		break;
	case EOT:
		/* 06_04 */
		if (DEBUG_MODE) Slips_PrinterStr("\n EOT gonderildi\n");
		temp_buffer[0] = 0x04;
		temp_buffer[1] = 0;
		break;
	default:
		Mainloop_DisplayWait("Kontrol Karakter Hatasi",1);
		if (DEBUG_MODE) Slips_PrinterStr("\nKontrol Karakter Hatasi\n");
		return(STAT_NOK);
		break;

	}

	if(Uart_SendBuf (VERDE_PORT, (byte *)temp_buffer, 2) <= 0)
	{
		/* 06_04 */
		if (DEBUG_MODE) Slips_PrinterStr("\nUart_SendBuf HATASI\n\n");
		return(STAT_NOK);
	}
/**/
	if(DEBUG_MODE)
	{	
		memset(lbtemp, 0, sizeof(lbtemp));
		sprintf(lbtemp, "\n\n Cntrl_char: %d GONDERILDI\n\n",Cntrl_char);
		Slips_PrinterStr(lbtemp);
	}
/**/
	Mte_Wait(100);
	return(STAT_OK);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Verde_SendReceive
 *
 * DESCRIPTION	: Sends the control characters using verde port
 *
 * NOTES		:
 *
 * RETURN		:	R.›.÷. 19/10/2002
 * ------------------------------------------------------------------------ */
byte Verde_SendReceive(char *Buffer, boolean Wait_Response, ulint Verde_Message_Timeout)
{
	byte  Status;
	boolean Waiting_ACK;
	sint  BufLen;
	char fpBuf[150];
	char fBuf[150];
	char SaveBuf[150];
	usint RxLength;
	ulint Transaction_start_time, Left_timeout, Time_now;
	sint  Retry,i;

	Status = STAT_NOK;      /* default value */
	Retry = 0;

	if (strlen(Buffer)>1)
	{
	    fpBuf[0] = STX;
		fpBuf[1] = 0;
		strcat(fpBuf,Buffer);
		BufLen = strlen(fpBuf);
		fpBuf[BufLen++] = ETX;
		fpBuf[BufLen++] = 0;
		Host_CalcLRC(fpBuf+1);
	}
	else
	{
		strcpy(fpBuf,Buffer);
		BufLen = 1;
    }
	memcpy(SaveBuf,fpBuf,BufLen);

	/* VERDE 'ye mesaj gonderme*/
	Transaction_start_time = Scan_MillSec();
	Verde_Init_Port();

	/* 06_07 flus init islemi oncesinde yapilmisti */
	/*Uart_Flush (VERDE_PORT);*/

	do
	{
		memset( fpBuf, 0, 150 );
		memcpy(fpBuf,SaveBuf,BufLen);

		Mte_Wait(100);

		if (Uart_SendBuf (VERDE_PORT, (byte *)fpBuf, BufLen) > 0)
		{
/**/
			if (DEBUG_MODE)    
			{
				Printer_WriteStr ("Send  VERDE:\n");
				PrntUtil_BufAsciiHex((byte *)fpBuf,BufLen);
				PrntUtil_Cr(1);
			}
/**/
			Kb_Read();

			/* receive ACK */
			Status = Verde_Receive (fBuf, &RxLength, VERDE_ACK_TIMEOUT);

			if (Status == ACK)
			{
/**/
				if (DEBUG_MODE)    
				{
					Printer_WriteStr ("ACK \n");
					PrntUtil_Cr(1);
				}
/**/
				Transaction_start_time = Scan_MillSec();

				if (Wait_Response)	Retry = 0;
				else
				{
					Status = STAT_OK;
					Retry = BFIRMA_COMM_RETRY;
				}
			}
			else if ((Status == NAK) || (Status == STAT_TIMEOUT))
			{
				if (Retry >= (BFIRMA_COMM_RETRY - 1))
				{
					/* 06_04 */
/**/
					if (DEBUG_MODE) 
					{
						Printer_WriteStr("STAT_TIMEOUT\n");
					}
/**/
					if (Wait_Response) 
						Mainloop_DisplayWait(" GôNDERòLEMEDò  ",2);
					return (STAT_TIMEOUT);
				}
			}
			else
			{
				if (Wait_Response) 
					Mainloop_DisplayWait(" VERDE GôNDERòLEMEDò  ",2);
					return (STAT_TIMEOUT);
			}

			/* get receive message */
			if (Wait_Response)
			{
				Time_now = Scan_MillSec();

				if ((Time_now < Transaction_start_time) || ((Time_now - Transaction_start_time) > Verde_Message_Timeout))
					return (STAT_TIMEOUT);

				Left_timeout = Verde_Message_Timeout - (Time_now - Transaction_start_time);

				Status = Verde_Receive (fBuf, &RxLength, Left_timeout);

				if (Status == STAT_OK)
				{
					fBuf[RxLength] = 0;
/**/
					if (DEBUG_MODE)
					{
						Printer_WriteStr ("VERDE Receive :\n");
						PrntUtil_BufAsciiHex((byte *)fBuf,RxLength);
						PrntUtil_Cr(1); 
					}
/**/
					/* Check LRC */
					if (Host_CheckLRC(fBuf,RxLength) != STAT_OK)
					{
						/* 06_04*/
/**/
						if (DEBUG_MODE)
						{
							Printer_WriteStr("LRC Problemi\n");
							Slips_PrinterStr("\nVERDE LRC HATASI\n\n");
						}
/**/
						Status=STAT_NOK;
						memset(SaveBuf, 0, sizeof(SaveBuf));
						SaveBuf[0]=NAK;
						SaveBuf[1]=0;
					}
					else
					{
						memcpy(Buffer,fBuf,RxLength);
						Buffer[RxLength] = 0;

						/* 06_04 */
/**/
						if(DEBUG_MODE) 
						{
							Printer_WriteStr ("Verde Buffer :\n");
							PrntUtil_BufAsciiHex((byte *)Buffer,RxLength);
							PrntUtil_Cr(1); 
						}
/**/
						Status = STAT_OK;
					}						
				}
				else
				{
					/* 06_04 */
/**/
					if (DEBUG_MODE) 
					{
						Printer_WriteStr("POS TIMEOUT FOR VERDE\n");
						Slips_PrinterStr("  POS  TIMEOUT FOR VERDE ");
					}
/**/
					return(Status);
				}
			}
		}

		Retry++;

	} while ((Status != STAT_OK) && (Retry < BFIRMA_COMM_RETRY));

	return(Status);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Verde_Receive
 *
 * DESCRIPTION	:   
 *
 * NOTES		:
 *
 * RETURN		: STAT_NOK, STAT_OK, ACK, NAK
 * ------------------------------------------------------------------------ */
byte Verde_Receive (char *RxMessage, usint *RxLength, usint Left_Time)
{
    ulint time_in_miliseconds, receive_start_time;
	sint i, j;
    byte ch;

	i = 0;
	receive_start_time = Scan_MillSec();


	/* 06_07 receive islemi oncesinde post initialize ediliyor */
	/*Verde_Init_Port(); */
	Mte_Wait(300);

	for (;;)
	{
		Kb_Read();

		if (Uart_GetByte (VERDE_PORT, &ch) == 1)
		{  
			switch (ch)
			{
				case STX :

					RxMessage[i] = ch;
					i++;

					for(;;)              
					{
						Kb_Read();

						if (Uart_GetByte (VERDE_PORT, &ch) == 1)
						{
							/*RxMessage[i++] = ch;*/
							RxMessage[i] = ch;
							i++;

							if (ch == ETX) break;                
						}
		
						time_in_miliseconds = Scan_MillSec();
						if ((time_in_miliseconds < receive_start_time) || ((time_in_miliseconds - receive_start_time) > Left_Time))
						{
							/* 02/09/2002 irfan. debug eklendi */
/**/
							if (DEBUG_MODE)    
							{
								Printer_WriteStr ("Host_Receive_BFirma Mesaj:\n");
								Printer_WriteStr ("Donus Kodu:STAT_TIMEOUT\n");
							}
/**/
							return (STAT_TIMEOUT);
						}
					} 

					/* verde. LRC al˝n˝yor */
					/* 06_07 Verde icin LRC alamama problemini cozmet icin 200 msec wait konuldu */
					Mte_Wait(200);

					if (Uart_GetByte (VERDE_PORT, &ch) == 1)
					{
						RxMessage[i] = ch;
						i++;

/*						RxMessage[i+j++] = ch;*/
					}

					time_in_miliseconds = Scan_MillSec();
					if ((time_in_miliseconds < receive_start_time) || ((time_in_miliseconds - receive_start_time) > Left_Time))
					{
						/* 02/09/2002 irfan. debug eklendi */
/**/
						if (DEBUG_MODE)    
						{
							Printer_WriteStr ("\n Host_Receive_VERDE ExtraChars:\n");
							Printer_WriteStr ("\n Donus Kodu:STAT_TIMEOUT\n");
						}
/**/
						return (STAT_TIMEOUT);
					}

					*RxLength = i;                    

					Host_SetParity ((byte *) RxMessage, *RxLength, CLEAR_PARITY);

					/* 02/09/2002 irfan. debug eklendi */
/**/
					if (DEBUG_MODE)    
					{
						Printer_WriteStr ("Host_Receive_VERDE_STAT_OK :\n");
					}
/**/
					return (STAT_OK);

				case NAK :
				/* 02/09/2002 irfan. debug eklendi */
/**/
				if (DEBUG_MODE)    
				{
					Printer_WriteStr (" \n Host_Receive_VERDE_NAK :\n");
				}
/**/
					return (NAK);
				case ACK:
				/* 02/09/2002 irfan. debug eklendi */
/**/
				if (DEBUG_MODE)    
				{
					Printer_WriteStr ("\n Host_Receive_VERDE_ACK :\n");
				}
/**/
					return (ACK);

				default :
				/* 02/09/2002 irfan. debug eklendi */
/**/
				if (DEBUG_MODE)    
				{
					Printer_WriteStr ("\n Host_Receive_VERDE_Default :\n");
				}
/**/
				break;
			}          
		}

		time_in_miliseconds = Scan_MillSec();
		if ((time_in_miliseconds < receive_start_time) || ((time_in_miliseconds - receive_start_time) > Left_Time))
		{
			/* 02/09/2002 irfan. debug eklendi */
/**/
			if (DEBUG_MODE)    
			{
				Printer_WriteStr ("\n Host_Receive_VERDE 3 :\n");
				Printer_WriteStr ("\n Donus Kodu:STAT_TIMEOUT\n");
			}
/**/		
		        return (STAT_TIMEOUT);
	    }
    }
}


/******************* VERDE REDESIGN **********************/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Verde_SendRes1_GetReq1
 *
 * DESCRIPTION		: Send Res1. and receive Req1
 *
 * RETURN			: STAT_OK or STAT_NOK
 *
 * NOTES			: 19/10/2002
 *
 * --------------------------------------------------------------------------*/
byte Verde_SendRes1_GetReq1(boolean IsEMV)
{	
	char lbSend_Buffer[150];
	char temp_buf[150];

	Verde_Res1 lb_Verde_Res1;
	Verde_Req1 lb_Verde_Req1;

	if (IsEMV == TRUE)
	{
		char Pan[20];
		byte PanLength;
		sint i,j;
		byte *pbIndex;
		char temp_cn[20];
		char tempchnm[50];
		RowData myrdData;

		memset(Pan, 0, sizeof(Pan));
		EmvDb_Find(&myrdData, tagPAN);
		EmvIF_BinToAscii(myrdData.value, Pan, myrdData.length, sizeof(Pan), FALSE);
		while (Pan[strlen(Pan) - 1] == 'F') Pan[strlen(Pan) - 1] = 0;
		PanLength = strlen(Pan);
		memset(card_info,' ',40);
		memset(card_no,0,20);
		memset(exp_date,0,5);
		memset(cvv_2,0,4);
//bm 15.12.2004 		memset(card_info1_name, 0, sizeof(card_info1_name));
//bm 15.12.2004 		memset(card_info1_surname, 0, sizeof(card_info1_surname));

		memset(tempchnm,' ',50);	
		pbIndex = (byte *)tempchnm;
		EmvIF_AppendData(&pbIndex, tagCARDHOLDER_NAME, FALSE, 26);
//bm 15.12.2004 		memcpy(card_info1_name, tempchnm, sizeof(card_info1_name));

		memset(tempchnm,' ',50);	
		pbIndex = (byte *)tempchnm;
		EmvIF_AppendData(&pbIndex, tagCRDHLDR_NAME_EXT, FALSE, 45);
//bm 15.12.2004 		memcpy(card_info1_surname, tempchnm, sizeof(card_info1_surname));

		memcpy(card_info, Pan, PanLength);
		memset(temp_cn,' ',20);	
		pbIndex = (byte *)temp_cn;
		EmvIF_AppendData(&pbIndex, tagAPPL_EXPIRATION_DATE, FALSE, 0);
		card_info[PanLength] = '=';
		EmvIF_BinToAscii(temp_cn,card_info+PanLength+1,2,4,FALSE);
		memcpy(card_info+PanLength+5,"000",3);
		memcpy(card_no, Pan, PanLength);
		memcpy(exp_date,card_info+PanLength+1,4);
		memcpy(cvv_2,"000",3);
		memset(remained_chip_data,0,sizeof(remained_chip_data));
		memcpy(remained_chip_data,card_info+PanLength+1,7);
		if (EmvDb_Find(&myrdData, tagTRACK2_EQUIVALENT_DATA))
		{
			EmvIF_BinToAscii(myrdData.value,card_info,myrdData.length,40,FALSE);
			j = myrdData.length * 2;
			for(i=0;i<40;i++)
				if (i >= j) card_info[i] = ' ';
				else if (card_info[i] == 'D') card_info[i] = '=';
				else if (card_info[i] == 'F')
				{
					card_info[i] = ' ';
					j = i;
				}
		}
	}

	/* initialize the message structures */
	memset(&lb_Verde_Res1, ' ', sizeof(lb_Verde_Res1));
	memset(&lb_Verde_Req1, ' ', sizeof(lb_Verde_Req1));

	/* initialize the send buffer */
	memset(lbSend_Buffer, 0, sizeof(lbSend_Buffer));

	/* fill the res1 message */
/*	memcpy(&lb_Verde_Res1.Ver_Message_Type,"10",2);*/
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) memcpy(&lb_Verde_Res1.Ver_Message_Type,"10",2);
	else memcpy(&lb_Verde_Res1.Ver_Message_Type,"40",2);

	memcpy(&lb_Verde_Res1.Ver_Card_no, "                   ", 19);
	memcpy(&lb_Verde_Res1.Ver_Card_no, card_no, strlen(card_no));
	
	/* 06_07 tum transcation lar icin 1 atan˝yordu. simdi tran_type atan˝yor. */
	lb_Verde_Res1.Ver_Transaction_Type = tran_type + '0';

	/* initialize the buffer */
	memset(lbSend_Buffer, 0, sizeof(lbSend_Buffer));
	memcpy(lbSend_Buffer, &lb_Verde_Res1, sizeof(lb_Verde_Res1));
	
	/* send Res1 */
	/* 06_07 req1 in al˝nmas˝ icin timeout 5 sec den 2 sec'e indirildi. */
	if(Verde_SendReceive(lbSend_Buffer, TRUE,2000) != STAT_OK) 
	{
		if((Verde_SendControlChar(EOT)) != STAT_OK) return(STAT_NOK);
		return(STAT_NOK);
	}

	/* analys the message */
	if(Verde_AnalyseRequest_Message(lbSend_Buffer) !=STAT_OK) return STAT_NOK;

	memcpy(&lb_Verde_Req1, lbSend_Buffer+1, sizeof(Verde_Req1));

	glAmount_Req_Type = lb_Verde_Req1.Ver_Amount_Request_Type;
										
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Verde_SendRes2_GetReq2
 *
 * DESCRIPTION		: Send Res2. and receive Req2
 *
 * RETURN			: STAT_OK or STAT_NOK
 *
 * NOTES			: 19/10/2002
 *
 * --------------------------------------------------------------------------*/
byte Verde_SendRes2_GetReq2(char *Amount, char *Discount)
{	
	char lbSend_Buffer[150];

	Verde_Res2 lb_Verde_Res2;
	Verde_Req2 lb_Verde_Req2;
/*
	Printer_WriteStr("\nAmountVerde:\n");
	PrntUtil_BufAsciiHex((byte *)Amount,strlen(Amount));
	Printer_WriteStr("\n");
*/
	/* initialize the message structures */
	memset(&lb_Verde_Res2, ' ', sizeof(lb_Verde_Res2));
	memset(&lb_Verde_Req2, ' ', sizeof(lb_Verde_Req2));

	/* initialize the send buffer */
	memset(lbSend_Buffer, 0, sizeof(lbSend_Buffer));

	/* fill the res2 message */
/*	memcpy(&lb_Verde_Res2.Ver_Message_Type, "12",2);*/
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) memcpy(&lb_Verde_Res2.Ver_Message_Type,"12",2);
	else memcpy(&lb_Verde_Res2.Ver_Message_Type,"42",2);

	memcpy(&lb_Verde_Res2.Ver_Amount, Amount, 13);
	
	/* initialize the buffer */
	memset(lbSend_Buffer, 0, sizeof(lbSend_Buffer));
	memcpy(lbSend_Buffer, &lb_Verde_Res2, sizeof(lb_Verde_Res2));
	
	/* send Res2 */
	if(Verde_SendReceive(lbSend_Buffer, TRUE,10000) != STAT_OK)  /* 06_08 120 sec. 10 sec yapildi. */
	{
		if((Verde_SendControlChar(EOT)) != STAT_OK) return(STAT_NOK);
		return(STAT_NOK);
	}

	/* analys the message */
	if(Verde_AnalyseRequest_Message(lbSend_Buffer) !=STAT_OK) return STAT_NOK;
	
	memcpy(&lb_Verde_Req2, lbSend_Buffer+1, sizeof(Verde_Req2));

	memcpy(Discount,lb_Verde_Req2.Ver_Discount_Amount,13);

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Verde_SendRes3
 *
 * DESCRIPTION		: Send Res3. and receive ACK
 *
 * RETURN			: STAT_OK or STAT_NOK
 *
 * NOTES			: 19/10/2002
 *
 * --------------------------------------------------------------------------*/
byte Verde_SendRes3(char *Message)
{
	Verde_Res3 d_Verde_Res3;
	char lbSend_Buffer[150];

		/* fill the res3 message */
/*		memcpy(&d_Verde_Res3.Ver_Message_Type,"14",2);*/
		if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) memcpy(&d_Verde_Res3.Ver_Message_Type,"14",2);
		else memcpy(&d_Verde_Res3.Ver_Message_Type,"44",2);

		memcpy(&d_Verde_Res3.Ver_Response_Code,Message,4); /* bm 21.10.2004  "0000",4);*/
		memcpy(&d_Verde_Res3.Ver_Response_Code[1], d_Transaction_Receive.confirmation,3);

		memcpy(&d_Verde_Res3.Ver_Message, d_Transaction_Receive.message, 16);
		memcpy(&d_Verde_Res3.Ver_Installment_Number, ATS_Count_c, 2);
		memcpy(&d_Verde_Res3.Ver_Authorized_Amount,d_Transaction_Receive.amount_surcharge, 13);
		memcpy(&d_Verde_Res3.Ver_Transaction_Seq_No, seq_no_c, 4);
		memcpy(&d_Verde_Res3.Ver_Authorization_Code, d_Transaction_Receive.approval_code, 6);

		memset(lbSend_Buffer, 0, sizeof(lbSend_Buffer));
		memcpy(lbSend_Buffer, &d_Verde_Res3, sizeof(d_Verde_Res3));
		
		return(Verde_SendReceive(lbSend_Buffer,FALSE,1000));
}

/* 06_08 iptal mesajlarinda gonderilmek uzere yeni mesaj yarat˝ld˝ */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Verde_SendRes4
 *
 * DESCRIPTION		: Send Res4. and receive ACK
 *
 * RETURN			: STAT_OK or STAT_NOK
 *
 * NOTES			: 27/12/2002
 *
 * --------------------------------------------------------------------------*/
byte Verde_SendRes4(void)
{
	char lbSend_Buffer[150]; /* 06.09.2004 100 idi stack ten dolay˝ 150 yap˝ld˝. 
								Filler eklendikten sonra olu˛an bi hata*/
	char lbcard_no[20];
	
	memset(lbcard_no,0, sizeof(lbcard_no));
	memcpy(lbcard_no, card_no, sizeof(card_no));
	Utils_LeftPad(lbcard_no,'0',19);
	lbcard_no[19] = 0;

	memset(lbSend_Buffer, 0, sizeof(lbSend_Buffer));
/*	memcpy(lbSend_Buffer, "15", 2);*/
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) memcpy(lbSend_Buffer,"15",2);
	else memcpy(lbSend_Buffer,"45",2);

	memcpy(lbSend_Buffer+2, &d_Void_Send, sizeof(d_Void_Send));
	strcat(lbSend_Buffer, lbcard_no);
	strcat(lbSend_Buffer, v_amount_surcharge);
		
	return(Verde_SendReceive(lbSend_Buffer,FALSE,1000));
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME	: Verde_Poll_PinpadPort
 *
 * DESCRIPTION		:   
 *
 * NOTES			:
 *
 * RETURN			: r.i.ˆ		19/10/2002        
 * ------------------------------------------------------------------------- */
byte Verde_Poll_PinpadPort(sint ExtraChars, char *lbReceived_Buffer_Verde)
{
	ulint Timeout;
    sint i, j;
	int k;
    byte ch;
	char test_temp[30];	/*test amacl˝ 14/12/99 */
	char LRC_Char;
	char lrc;
	char temp_lrc[30];	/* test amacl˝ 14/12/99 */
	char lrc_temp_buffer[150];

	memset(lrc_temp_buffer, 0, sizeof(lrc_temp_buffer));

	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait(" VERDE POD'DAN  TUTAR BEKLENIYOR",NO_WAIT);

	for(;;)
	{
		if (Uart_GetByte (VERDE_PORT, &ch) == 1)
		{
			switch (ch)
			{
			case EOT: /* mesaj beklerken EOT al˝rsa 25/02/2000 AKSIGORTA*/
						return (EOT); 

			case NAK: return (NAK);
			case ACK: return (ACK);

			case STX:

				k=0;	/* k mesaj uzunlugunu say˝yor . STX ve LRC hariÁ,  ETX dahil */
				lrc_temp_buffer[k] = ch;
				k++;

				Timeout = Scan_MillSec();	/* start timeout value to receive all message */
									
				for(;;)
				{
					if (Uart_GetByte (VERDE_PORT, &ch) == 1)
					{
						lrc_temp_buffer[k] = ch;
						k++;
						if (ch == ETX)  break;
					}

					if ((Scan_MillSec() - Timeout) > TIMEOUT_VERDE*1000)
							return (STAT_TIMEOUT_VERDE);
					Kb_Read();				
				}
				
				if (Uart_GetByte(VERDE_PORT, &ch) == 1) 
				{
					LRC_Char = ch;
					lrc_temp_buffer[k] = ch;
				}

				k++;
				lrc_temp_buffer[k] = 0;

				memcpy(lbReceived_Buffer_Verde, lrc_temp_buffer, (k-2));
/**/
				if(DEBUG_MODE)
				{
					Slips_PrinterStr("\nlbReceived_Buffer_Verde\n");
					PrntUtil_BufAsciiHex((byte *)lbReceived_Buffer_Verde,(k-1));
					Slips_PrinterStr("\n\n");
				}
/**/
				/* Check LRC */
				if (Host_CheckLRC(lrc_temp_buffer,k) != STAT_OK)
				{
					if (DEBUG_MODE) Printer_WriteStr("LRC Problemi\n");
					return(NAK);
				}
				else
				{
					return(STAT_OK);
				}						

			default:
					break;
		  }
		}
		
		if (Kb_Read()==MMI_ESCAPE) return(STAT_NOK);
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Verde_AnalyseRequest_Message
 *
 * DESCRIPTION		: Handling the analysing of the Verde Request message.
 *
 * RETURN			: STAT_OK or STAT_NOK
 *
 * NOTES			: 19/10/2002
 *
 * --------------------------------------------------------------------------*/
byte Verde_AnalyseRequest_Message(char *lbReceived_Buffer_Verde)
{
	Verde_Req1 lb_Verde_Req1;
	Verde_Req2 lb_Verde_Req2;
	int myResultFirst, myResultSecond;

	char temp_char[2];
	sint i;
	char lbtemp_string[150];
	char temp_lbReceived_Buffer_Verde[150];

	/* initialize the message structures */
	memset(&lb_Verde_Req1, ' ', sizeof(lb_Verde_Req1));
	memset(&lb_Verde_Req2, ' ', sizeof(lb_Verde_Req2));

	memcpy(temp_lbReceived_Buffer_Verde, 0, sizeof(temp_lbReceived_Buffer_Verde));
	memcpy(temp_lbReceived_Buffer_Verde, lbReceived_Buffer_Verde+1, 90);

	/* check for Request Format*/
/*	if(memcmp(temp_lbReceived_Buffer_Verde, "11",2) == 0) */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) myResultFirst = memcmp(temp_lbReceived_Buffer_Verde, "11",2);
	else myResultFirst = memcmp(temp_lbReceived_Buffer_Verde, "41",2);
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) myResultSecond = memcmp(temp_lbReceived_Buffer_Verde, "13",2);
	else myResultSecond = memcmp(temp_lbReceived_Buffer_Verde, "43",2);
	
	if (myResultFirst == 0)
	{	/**** Req1 ****/
		memcpy(&lb_Verde_Req1, temp_lbReceived_Buffer_Verde, sizeof(lb_Verde_Req1));

		/* check for Ver_Transaction_Type */
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, &lb_Verde_Req1.Ver_Amount_Request_Type, 1);
/**/
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nVer_Amount_Request_Type:");
			Slips_PrinterStr(lbtemp_string);
		}
/**/
		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("Ver_Amount_Request_Type HATASI   ");
			PrntUtil_Cr(7);
			GrphText_Cls (TRUE);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (STAT_NOK);
		}


		/* send ACK as soon as message is received 26/12/2001 */		
		if((Verde_SendControlChar(ACK)) != STAT_OK) return(STAT_NOK);
		
		return STAT_OK;
	}
/*	else if(memcmp(temp_lbReceived_Buffer_Verde, "13",2) == 0) */
	else if (myResultSecond == 0)
	{	/**** Req2 ****/
		memcpy(&lb_Verde_Req2, temp_lbReceived_Buffer_Verde, sizeof(lb_Verde_Req2));
		
		/* check for Ver_Discount_Amount */
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, &lb_Verde_Req2.Ver_Discount_Amount, 13);
/**/
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nVer_Discount_Amount:");
			Slips_PrinterStr(lbtemp_string);
		}
/**/
		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("Ver_Discount_Amount HATASI   ");
			PrntUtil_Cr(7);
			GrphText_Cls (TRUE);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (STAT_NOK);
		}

		/* assign verde amount */
		memset(verde_discount,0, sizeof(verde_discount));
		memcpy(verde_discount, &lb_Verde_Req2.Ver_Discount_Amount, 13);
/**/
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nVerdeDicount:");
			Slips_PrinterStr(verde_discount);
		}
/**/		
		/* check for Ver_Amount */
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, &lb_Verde_Req2.Ver_Amount, 13);
/**/
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nVer_Amount:");
			Slips_PrinterStr(lbtemp_string);
		}
/**/
		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("Ver_Amount HATASI   ");
			PrntUtil_Cr(7);
			GrphText_Cls (TRUE);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (STAT_NOK);
		}

		/* assign verde amount */
//bm 15.12.2004 		memset(verde_amount1,0, sizeof(verde_amount1));
//bm 15.12.2004 		memcpy(verde_amount1, &lb_Verde_Req2.Ver_Amount, 13);
/**/
		if(DEBUG_MODE)
		{
//bm 15.12.2004 			Slips_PrinterStr("\nverde_amount1:");
//bm 15.12.2004 			Slips_PrinterStr(verde_amount1);
		}
/**/
		/* send ACK as soon as message is received 26/12/2001 */		
		if((Verde_SendControlChar(ACK)) != STAT_OK) return(STAT_NOK);
		
		return STAT_OK;
	}
	else
	{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("Mesaj Tipi HATASI   ");
			PrntUtil_Cr(7);
			GrphText_Cls (TRUE);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (STAT_NOK);
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Verde_IdleLoopForReq2
 *
 * DESCRIPTION		: Handle Verde Idle Loop process to receive Req2.
 *
 * RETURN			: None.
 *
 * NOTES			: 19/10/2002
 *
 * --------------------------------------------------------------------------*/
byte Verde_IdleLoopForReq2(void)
{
	char send_buf[10];				
	byte ret_code, Status1, Status2;
	char entered_seq_no_local[5];
	char lbReceived_Buffer_Verde[150];

	Verde_Req1	d_Verde_Req1;
	Verde_Req2	d_Verde_Req2;
	Verde_Res1	d_Verde_Res1;
	Verde_Res2	d_Verde_Res2;
/* Toygar - NextEMV Start - Ok */
/*
	EmvApplication eaApplication;
	EMVTransData etdData;
	RowData rdSDAData;
	PTStruct fpPTStruct;
	boolean IsEMV;
*/
/* Toygar - NextEMV End - Ok */

	/* initialize Received Buffer */
	memset(lbReceived_Buffer_Verde,0,sizeof(lbReceived_Buffer_Verde));	

	Status1 = Verde_Poll_PinpadPort (1,lbReceived_Buffer_Verde);	/* parametre ETX den sonra LRC characterini bekliyor */
										/* bu arada EOT gelirse onu da a˛a˝daki if check de
										  return ediyor 25/02/2000 AKSIGORTA*/

	/* port initialization and clearing */
	Verde_Init_Port();			/* selects the verde port */

	/* 06_07 flus init islemi oncesinde yapilmisti */
	/*Uart_Flush (VERDE_PORT);*/


	switch(Status1)
	{
		case STAT_NOK: 	return(STAT_NOK);
		
		case STAT_OK:	/* RS232 den mesaj al˝nd˝*/

			/* send ACK */
			if((Verde_SendControlChar(ACK)) != STAT_OK) return(STAT_NOK);

			/* initialize the message structures */
			memset(&d_Verde_Req2, ' ', sizeof(d_Verde_Req2));

			/* check for message format and specify the message type and send the ACK*/
			if(Verde_AnalyseRequest_Message(lbReceived_Buffer_Verde) != STAT_OK) return(STAT_NOK);

			memcpy(&d_Verde_Req2, lbReceived_Buffer_Verde+1, sizeof(d_Verde_Req2));

			/* assign the received amounts */
//bm 15.12.2004 			memcpy(verde_amount1, &d_Verde_Req2.Ver_Amount,13);
			memcpy(verde_discount, &d_Verde_Req2.Ver_Discount_Amount,13);
			return(STAT_OK);

		case STAT_NO_MSG_RECEIVED_RS232:	/* RS232 den beklenen mesaj anl˝namad˝ */
			return(STAT_NOK);

		case EOT: Slips_PrinterStr("\n\nEOT\n\n"); return(EOT);

		case NAK:	if((Verde_SendControlChar(NAK)) != STAT_OK)
					{
						Slips_PrinterStr("\n\nNAK GONDERILEMEDI\n\n");
						return(STAT_NOK);
					}
					return(STAT_OK);

		case ACK: Slips_PrinterStr("\n\nACK\n\n"); return(ACK);
		default:
			Mainloop_DisplayWait("beklenmedik RS232 hatasi",1);
			Slips_PrinterStr("\beklenmedik RS232 hatasi\n");
			break;
	}
	return(STAT_NOK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Verde_AskPIN_Embedded
 *
 * DESCRIPTION		: Requests the PIN. from embedded pinpad
 *
 * RETURN			:STAT_OK or STAT_NOK     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Verde_AskPIN_Embedded (char *fpEmbeddedCpin)
{
	byte result;
	PinStruct stPinStruct;
	sint i, j;	
	
	
	Printer_WriteStr("Verde ask pin\n");
	strcpy((char *)stPinStruct.DisplayMessage,"MÅüteri ûifresi?");
	
	for(i=0;i<strlen(card_no);i++)
	if(!isdigit(card_no[i])) break;
	memset(stPinStruct.CardNumber,0,sizeof(stPinStruct.CardNumber));
	memcpy(stPinStruct.CardNumber,card_no,strlen(card_no));
	
	stPinStruct.CardNumberLength=strlen(stPinStruct.CardNumber);

	result=STAT_OK;
	
	if(master_key_no < '0' || master_key_no > '9')
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("PARAMETRE HATALI",1);
		result=STAT_NOK;
	}
	else	stPinStruct.MasterKeyIndex = master_key_no;
	
	memcpy(stPinStruct.WorkingKey, working_key, 16);

	stPinStruct.PinSize=8;
	stPinStruct.PinRetry=3;
	stPinStruct.Timeout=30;
	stPinStruct.PinMode=0;
	stPinStruct.SecurityMode=0;

	if(result==STAT_OK)
	{
		result =Security_GetPINBlock (&stPinStruct);
	}
	if(result == PIN_OK)	
	{
		char str[10];
		memcpy(str,stPinStruct.PinBlock,8);
		
		str[8]=0;
/**/
		if (DEBUG_MODE)
		{
			Printer_WriteStr("stPinStruct.PinBlock \n");
			PrntUtil_BufAsciiHex((byte *)stPinStruct.PinBlock,8);
		}
/**/
		Utils_BinToAscii(str, fpEmbeddedCpin, 8);
		
		result=STAT_OK;
	}
	else	result=STAT_NOK;	
	
	return result;
}
