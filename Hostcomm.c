/**************************************************************************
  FILE NAME:   HOSTCOMM.C
  MODULE NAME: 
  PROGRAMMER:  
  DESCRIPTION: 
  REVISION:    AKB01_15
 **************************************************************************/

	  /*==========================================*
	   *         I N T R O D U C T I O N          *
	   *==========================================*/

	  /*==========================================*
	   *             I N C L U D E S              *
	   *==========================================*/
/* General Header File */
/*#include "config.h"*/

#include "project.def"

/* MCC68K Header Files */

/*UPDATE 01_16 26/10/99 sprintf gibi komutlarýn prototype'ý için konuldu*/
/*#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include NOS_H*/

#include INTERFAC_H
#include STATIST_H
#include ERRORS_H
#include DEFINE_H
#include UTILS_H
#include PROTOCOL_H
#include FILES_H
#include TRANS_H
#include SLIPS_H
#include MAINLOOP_H
#include HOSTCOMM_H
#include MESSAGE_H
#include DEBUG_H
#include STATIST_H
#include EMVIF_H
#include GPRS_H

#include gsmradio_prm
#include gsmradio_h

	  /*==========================================*
	   *           D E F I N I T I O N S          *
	   *==========================================*/

#define PINPAD_PORT      UART_A
#define PINPAD_BAUD      1200
#define PINPAD_DATA_BITS 7
#define PINPAD_PARITY    'e' 
#define DLE  0x10

/* Toygar 13.05.2002 Start */
#define BFIRMA_PORT      UART_A
int BFIRMA_BAUD;
#define BFIRMA_DATA_BITS 7
#define BFIRMA_PARITY    'e' 

extern const int Bfirma_Ack_Timeout;
extern const int Bfirma_Message_Timeout;
boolean BUYUKFIRMA = FALSE;
extern char device_serial_no;   
extern boolean ReversalPending;
/* Toygar 13.05.2002 End */


extern struct Transaction_Receive d_Transaction_Receive;
extern boolean INSTALLATION;   
extern boolean EOD_OK;
extern boolean Debug;

extern struct Statistics                        d_Statistics1;
extern struct Statistics                        d_Statistics2;

extern const int Timeout_Server_Open;                 
extern const int POS_TIMEOUT;
extern const int PinPad_Timeout;

char Received_Buffer[900];
extern char amount_surcharge[14];
/*extern struct Transaction_Receive       d_Transaction_Receive;*/ /*verde yukarida zaten tanýmý var */
extern struct Receipt_Structure         d_Receipt;
/* Toygar EMVed */
/*extern char Send_Buffer[256];       *//* defined in trans.c */
/*extern char Send_Buffer[700];           /* defined in trans.c */
/* Toygar EMVed */
extern char tel_no_1[16];                       /* defined in trans.c */
extern char tel_no_2[16];                       /* defined in trans.c */

extern int card_no_length;
extern char card_no[20];
extern char working_key[16];

extern ulint total_batch_time;
extern char total_batch_time_c[9];

/* STATISTIC RELATED */
CommStatistics1 Modem_Statitics;
SystemStatistics1 SystemStatistics;
int total_statitics_counts1,total_statitics_counts2;    /* tran. base total statistics counter*/

extern char Line_Type;                                                                  /* Tone veya Pulse olabilir. */
boolean IS_HOST_CONNECTED_FLAG; /* to sens whether the connection with host is active or not */
boolean FIRST_PHONE_FLAG;			/* true-> birinci tel arandý, false-> ikinci tel arandý */
boolean SERVER_OPEN_TIMEOUT_FLAG;	/* server timeout oldugunu ifade eder */
boolean NAK_OCCURRED_FLAG;			/* karþýlýklý NAK'leþme sonucu hat kesiliyor */

extern	GPRS_Connect_Param	lsGPRS_Connect_Param;

extern char glb_host_ip[17];
extern usint glb_host_port;
extern boolean GPRS_USEGPRS;
extern boolean autoGSMRetry;
boolean GRPS_GPRSfailed;

/* genius */
extern boolean SORGULAMA;		/*1:TRUE:sorgulama 0:FALSE:gerçek transaction*/ /* 04_03 21/11/2002 */

extern char merch_AMEX_no[11];         
extern int auto_EOD_time_int;
extern char param_ver_no[4];                           
extern char POS_software_ver;  
extern char merch_no[13];
extern char merch_device_no[9];                                
extern int tran_type;
extern boolean DEBUG_MODE;
extern int KAMPANYA_FLAG;

                       
	  /*==========================================*
	   *       P R I V A T E  T Y P E S           *
	   *==========================================*/
	  /*==========================================*
	   *        P R I V A T E  D A T A            *
	   *==========================================*/

extern boolean GSMCALL;
extern GSMTargetParamStruct GSMDialPrm;

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_PrepareCall
 *
 * DESCRIPTION:   prepare parameters in function for calling of protocol
 *
 * RETURN:        Address of the protocol
 *
 * NOTES:
 * ------------------------------------------------------------------------ */
byte Host_PrepareCall(DialTargetType *DialTarget)
{

	memset(DialTarget->PhoneParam[0].PhoneNum, ' ', 16);
	memset(DialTarget->PhoneParam[1].PhoneNum, ' ', 16);

	memcpy(DialTarget->PhoneParam[0].PhoneNum, tel_no_1,strlen(tel_no_1));
	memcpy(DialTarget->PhoneParam[1].PhoneNum, tel_no_2,strlen(tel_no_2));

/* TL
	memcpy(DialTarget->PhoneParam[0].PhoneNum, "9,2816655",strlen("9,2816655"));
	memcpy(DialTarget->PhoneParam[1].PhoneNum, "9,2816655",strlen("9,2816655"));
*/
/* YTL
	memcpy(DialTarget->PhoneParam[0].PhoneNum, "9,2841035",strlen("9,2841035"));
	memcpy(DialTarget->PhoneParam[1].PhoneNum, "9,2841035",strlen("9,2841035"));
*/
	memset(DialTarget->PhoneParam[2].PhoneNum, 0, 16);

	DialTarget->PhoneParam[0].PhoneNum[15] = 0;
	DialTarget->PhoneParam[1].PhoneNum[15] = 0;

	DialTarget->PhoneParam[0].AnswerToneTimeout = MODEMANSWERTIMEOUT;
	DialTarget->PhoneParam[0].even_rounder = FALSE;
	DialTarget->PhoneParam[0].BitRate = c2400BAUDS;
	DialTarget->PhoneParam[0].ByteFormat = FORMAT_8N;
	DialTarget->PhoneParam[0].LogOnProtocolEn = FALSE;
	DialTarget->PhoneParam[1].AnswerToneTimeout = MODEMANSWERTIMEOUT;
	DialTarget->PhoneParam[1].even_rounder = FALSE;
	DialTarget->PhoneParam[1].BitRate = c2400BAUDS;
	DialTarget->PhoneParam[1].ByteFormat = FORMAT_8N;
	DialTarget->PhoneParam[1].LogOnProtocolEn = FALSE;

	/*DialTarget->PhoneParam[0].PhoneNum[15] = 0;*/ /* fazlalýk */


	DialTarget->NumberOfTrials = 3;
	DialTarget->RadioIndex = -1;
	DialTarget->LinePriority = TRUE;
	DialTarget->CommType = 0;

	if(Line_Type == '0')
	{
		/* Tone */
		ComHndlr_SetUserParams(1,'T',0,1);      /* GenParam iptal edilince ben ekledim */
	}
	else
	{
		/* Pulse */
		ComHndlr_SetUserParams(1,'P',0,1);      
	}

	return(STAT_OK);
}




/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_CalcLRC
 *
 * DESCRIPTION:   prepare parameters in function for calling of protocol
 *
 * RETURN:        Address of the protocol
 *
 * NOTES:
 * ------------------------------------------------------------------------ */
void Host_CalcLRC(char *TxMessage)
{
	int  len;
	char lrc;

	for (len = 0, lrc = 0x00; TxMessage[len] != 0; len++)
		lrc ^= TxMessage[len];
	TxMessage[len++] = lrc;
	TxMessage[len] = 0;
	
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_CheckLRC
 *
 * DESCRIPTION:   prepare parameters in function for calling of protocol
 *
 * RETURN:        Address of the protocol
 *
 * NOTES:
 * ------------------------------------------------------------------------ */
sint Host_CheckLRC (char *RxMessage, usint RxLength)
{
    int i;
    char lrc;
	char temp_lrc[50];	/* verde */

    for (i = 1, lrc = 0x00; i < RxLength - 1; i++)
		lrc ^= RxMessage[i];

		/* verde */
	/*
		if(DEBUG_MODE)
		{
			memset(temp_lrc, 0, sizeof(temp_lrc));
			sprintf(temp_lrc, "CALCULATED lrc: %2d", lrc);
			Slips_PrinterStr(temp_lrc);
			Slips_PrinterStr("\n\n");

			memset(temp_lrc, 0, sizeof(temp_lrc));
			sprintf(temp_lrc, "RECEIVE LRC:%2d", RxMessage[i]);
			Slips_PrinterStr(temp_lrc);
			Slips_PrinterStr("\n\n");
		}
	*/
	    if (lrc == RxMessage[i])
	    {
			if (!lrc)
		    {
			    RxMessage[i++] = '\xFF';
			    RxMessage[i] = 0;

		     }
		     return(STAT_OK);
	    }
	    return (STAT_NOK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_SetParity
 *
 * DESCRIPTION:   prepare parameters in function for calling of protocol
 *
 * RETURN:        Address of the protocol
 *
 * NOTES:
 * ------------------------------------------------------------------------ */
void Host_SetParity( byte *buf, /* data buffer to be set */
		     sint len,  /* length of data buffer */
		     byte type  /* set data even/odd parity */ )
{
    /* this table is used to determine if a half byte is odd or even
       even values are designated by 0 odd by 1 */
    byte PTab[16] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};
    sint i;
    byte temp;

    for (i=0; i<len; i++)
    {
		Kb_Read(  ); /*@bm 27.08.2004 */
		temp = buf[i];
	if (type == CLEAR_PARITY) buf[i] &= 0x7f;
	else
			if ((PTab[buf[i] & 0x0f]^(PTab[((temp>>4) & 0x0f)])))  /* buf[i] is odd */
			{
				if (type == EVEN_PARITY) buf [i] |= 0x80;
			}
			else  /* buf[i] is even */
			{
				if (type == ODD_PARITY) buf[i]  |= 0x80;
			}
    }
}




/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_WaitForMessage
 *
 * DESCRIPTION: Waits for a message beginning with Delim1 ending with Delim2
 *              and any ExtraChars following Delim2.
 *
 * RETURN:  OK- Message received
 *          TIMEOUT_ERROR- Conditons not met before timeout.
 *
 * NOTES:  The  form of media (PSTN or Radio) depends on parameters known by
 *         ComHndlr which initiates the communication.
 *
 * ------------------------------------------------------------------------ */
byte Host_WaitForMessage (char *RxMessage, usint *RxLength, usint TimeOut,
			  char delim1, char delim2, sint ExtraChars)
{
	byte *RxM;
	sint dummy;
	RxLineConditionsType MessagesExpected;

	RxM = (byte *)RxMessage;
	MessagesExpected.timeout = TimeOut * 1000;
/* Toygar EMVed */
/*	MessagesExpected.max_length = 256;*/
/*	MessagesExpected.max_length = 600; irfan 02/09/2002 */
	MessagesExpected.max_length = 900;	/*irfan 02/09/2002 600 yapildi */
/* Toygar EMVed */
	MessagesExpected.num_of_conditions = 3;
	MessagesExpected.OneCondition[0].wanted_char = delim1;
	MessagesExpected.OneCondition[0].pre = 0;
	MessagesExpected.OneCondition[0].post = 255;
	MessagesExpected.OneCondition[1].wanted_char = delim2;
	MessagesExpected.OneCondition[1].pre = 255;
	MessagesExpected.OneCondition[1].post = ExtraChars;
	MessagesExpected.OneCondition[2].wanted_char = NAK;
	MessagesExpected.OneCondition[2].pre = 0;
	MessagesExpected.OneCondition[2].post = 0;
	/*MessagesExpected.OneCondition[3].wanted_char = EOT;
	MessagesExpected.OneCondition[3].pre = 0;
	MessagesExpected.OneCondition[3].post = 0;
	MessagesExpected.OneCondition[4].wanted_char = BEL;
	MessagesExpected.OneCondition[4].pre = 0;
	MessagesExpected.OneCondition[4].post = 0;*/
	dummy = ComHndlr_RxRadioOrLine(&MessagesExpected, &RxM, RxLength,0);
	/* If a condition is accepted add one when in NO_CONDITION state */
	switch (dummy)
	{
	case 2:
		/* 02/09/2002 irfan. debug eklendi */
/*
		if (DEBUG_MODE)    
		{
			Printer_WriteStr ("Host_WaitForMessage:\n");
			Printer_WriteStr ("Donus Kodu:2 - STAT_OK:\n");
		}
*/
		memcpy (RxMessage, RxM, MessagesExpected.max_length - ((char *)RxM - RxMessage));
		Host_SetParity ((byte *) RxMessage, *RxLength, CLEAR_PARITY);
		return(STAT_OK);
	case 3:
		/* 02/09/2002 irfan. debug eklendi */
/*
		if (DEBUG_MODE)    
		{
			Printer_WriteStr ("Host_WaitForMessage:\n");
			Printer_WriteStr ("Donus Kodu:3 - NAK:\n");
		}
*/
		return (NAK);
	/*case 4:
		return (EOT);
	case 5:
		return (BEL);*/
	case X_ESC_PRESSED:
		/* 02/09/2002 irfan. debug eklendi */
/*
		if (DEBUG_MODE)    
		{
			Printer_WriteStr ("Host_WaitForMessage:\n");
			Printer_WriteStr ("Donus Kodu:X_ESC_PRESSED-STAT_NOK\n");
		}
*/
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait((char *)MESSAGE_USERBREAK,WAIT_RESPONSE);
		return (STAT_NOK);
	case X_TIMEOUT:
		/* 02/09/2002 irfan. debug eklendi */
/*
		if (DEBUG_MODE)    
		{
			Printer_WriteStr ("Host_WaitForMessage:\n");
			Printer_WriteStr ("Donus Kodu:X_TIMEOUT-AKNET_POS_TIMEOUT_ERROR\n");
		}
*/
		return(AKNET_POS_TIMEOUT_ERROR);		/* aknet grubu ekledi 29/07/99*/

	default:
		/* 02/09/2002 irfan. debug eklendi */
/*
		if (DEBUG_MODE)    
		{
			Printer_WriteStr ("Host_WaitForMessage:\n");
			Printer_WriteStr ("Donus Kodu:default-STAT_NOK\n");
		}
*/
		return (STAT_NOK);
	}
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_SendCntrlChar
 *
 * DESCRIPTION:   prepare parameters in function for calling of protocol
 *
 * RETURN:        Address of the protocol
 *
 * NOTES:
 * ------------------------------------------------------------------------ */
/*
sint Host_SendCntrlChar (char Cntrl)
{
	byte  cntchr;
	sint  ret;

	cntchr = (byte)Cntrl;
	ret = ComHndlr_TxLine (&cntchr,1);
	if (ret == X_ESC_PRESSED) return (X_ESC_PRESSED);
	if (ret != X_OK) return (TIMEOUT_ERROR);
	return(STAT_OK);
}
*/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_SendMessage
 *
 * DESCRIPTION:   prepare parameters in function for calling of protocol
 *
 * RETURN:        Address of the protocol
 *
 * NOTES:
 * ------------------------------------------------------------------------ */
sint Host_SendMessage (char *TxMessage, usint TxLength)
{
	sint ret;

	ret = ComHndlr_TxLine ((byte *)TxMessage,TxLength);
	if (ret == X_ESC_PRESSED) return (X_ESC_PRESSED);
	if (ret != X_OK) return (TIMEOUT_ERROR);
	return (STAT_OK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_WaitForCntrlChar
 *
 * RETURN:  OK- Message received
 *          TIMEOUT_ERROR- Conditons not met before timeout.
 *
 * NOTES:  The  form of media (PSTN or Radio) depends on parameters known by
 *         ComHndlr which initiates the communication.
 * ------------------------------------------------------------------------ */
/*
sint Host_WaitForCntrlChar (usint TimeOut, char Cntrl)
{
	byte  CntrlChar[10];
	byte  *ControlChar_p = CntrlChar;
	usint RxLength;
	sint dummy;
	RxLineConditionsType MessagesExpected;

	MessagesExpected.timeout = TimeOut * 2000;         
	MessagesExpected.max_length = 300;
	MessagesExpected.num_of_conditions = 2;
	MessagesExpected.OneCondition[0].wanted_char = Cntrl;
	MessagesExpected.OneCondition[0].pre = 0;
	MessagesExpected.OneCondition[0].post = 0;
	MessagesExpected.OneCondition[1].wanted_char = EOT;
	MessagesExpected.OneCondition[1].pre = 0;
	MessagesExpected.OneCondition[1].post = 0;

	dummy = ComHndlr_RxRadioOrLine(&MessagesExpected,&ControlChar_p, &RxLength,0);
	if (dummy == 1)
	{
		if (Cntrl == ENQ)
			while ((CntrlChar [0] == ENQ) && (ComHndlr_RxLineDirect (CntrlChar, 1) > 0));
		else if (Cntrl == EOT)
		{
			return(STAT_NOK);
		}
	}
    else if (dummy == 2)
	{
		return(TIMEOUT_ERROR);
	}
	else if (dummy == X_ESC_PRESSED) return (X_ESC_PRESSED);
	else return(TIMEOUT_ERROR);
	return(STAT_OK);
}
*/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_PreDial
 *
 * DESCRIPTION: Processes predial function.
 *
 * RETURN: 
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Host_PreDial(void)
{
	if(!GSMCALL)
	{
		DialTargetType DialTarget;

/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) return(STAT_OK);
/* Toygar 13.05.2002 End */
		
		/* 01_18 08/12/99 Dial öncesi Line olup olmadýðýný kontrol ediyor */
/*
		if(Cpuhw_CheckPhoneLine() != TRUE)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("TELEFONU KAPATIN",1);
			return(STAT_NOK);
		}
*/
		/* reset the power of counter statistics flags */
		Statistics_System ((void *)&SystemStatistics, (usint)sizeof(SystemStatistics), TRUE);

		Host_PrepareCall(&DialTarget);

		/* aranan no nun gorulmemesi icin konuldu. Ancak arama sýrasýnda User Escape iptal */
		/*MK DEF:13 ARANAN NUMARALARIN GORUNMEMESI SAGLANDI*/
		ComHndlr_DialShow (OFF);

		if (ComHndlr_PreDialBkgr2 (&DialTarget))
		{
			/* 01_18 08/12/99 STAT_OK veya STAT_NOK dönmesi saðlanýyor*/
			Mte_Wait(200);
			return(STAT_OK);
		}
		else
		{
			/* 01_18 08/12/99 STAT_OK veya STAT_NOK dönmesi saðlanýyor*/
			Mte_Wait(200);
			return(STAT_NOK);
		}
        }
	else 
	{
		/* reset the power of counter statistics flags */
		return(STAT_OK);
		/*MMM*/
	}

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_Connect
 *
 * DESCRIPTION: Does predial, collects statistics information, updates
 *              statistics database.
 *
 * RETURN: STAT_OK or STAT_NOK
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Host_Connect(boolean PredialCall)
{
	usint Status, i;
	DialTargetType  DialTarget;
/* Toygar EMVed */
/*
	char fpBuf[256];
	usint RxLen=256;
*/
	char fpBuf[900],ConnectStatus=0;
	usint RxLen=900;
/* Toygar EMVed */
	ulint Timeout_Start_ServerOPen;
	boolean wait_for_open;
	boolean OPEN_IS_FOUND;
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	

	Printer_WriteStr("Hostconnect\n");
	if (BUYUKFIRMA) Printer_WriteStr("Buyukfirma\n");
	else Printer_WriteStr("not buyuk firma\n");
/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) return(STAT_OK);
/* Toygar 13.05.2002 End */
	/*MK DEF:13 ARANAN NUMARALARIN GORUNMEMESI SAGLANDI*/
	ComHndlr_DialShow (OFF);
	EmvIF_ClearDsp(DSP_MERCHANT);
	GRPS_GPRSfailed=TRUE;
	if (GPRS_USEGPRS) 
	{
		Printer_WriteStr("GPRS_USEGPRS true\n");		
		if (DEBUG_MODE)    Printer_WriteStr("GPRS_USEGPRS true\n");		
		GRPS_GPRSfailed=FALSE;
		if (lsGPRS_Connect_Param.tcpavailable){
			Printer_WriteStr("success = T1\n");		
			if (DEBUG_MODE)    Printer_WriteStr("Allready Connected\n");
			 return STAT_OK;
		}
		if(GPRS_Init(TRUE)==STAT_OK){
			Printer_WriteStr("success = T2\n");		
			Display_WriteStr(DSPL1,"HOST BA¦LANTISI YAPILIYOR");
//			if(GPRS_OpenConnection(&lsGPRS_Connect_Param.sd, "192.168.3.23",28002) == STAT_OK)
			if(GPRS_OpenConnection(&lsGPRS_Connect_Param.sd, glb_host_ip,glb_host_port) == STAT_OK)
			{
				Printer_WriteStr("Send\n");
				PrntUtil_BufAsciiHex((byte*)&lsGPRS_Connect_Param.sd, 20);
				Printer_WriteStr("success = T3\n");		
				Display_WriteStr(DSPL1,"GPRS DATA       G™NDER˜L˜YOR");
				if (DEBUG_MODE)    Printer_WriteStr("GPRS DATA       G™NDER˜L˜YOR\n");		
				return STAT_OK;
			}else GRPS_GPRSfailed=TRUE;
		}else {
			GRPS_GPRSfailed=TRUE;
			if (DEBUG_MODE)    Printer_WriteStr("GPRS_Init(TRUE) failed\n");		
		}
		if(GRPS_GPRSfailed && !autoGSMRetry){
		
			if(!Mainloop_GetKeyboardInput((char *)"GPRS HATASI.GSM?1:EVET 0:HAYIR",Buffer,1,1,
			       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,0, TRUE))
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("G˜R˜ž HATASI",1);
				return STAT_NOK;
			}
	
			if(Buffer[0] == '1')
			{
				if(!Mainloop_GetKeyboardInput((char *)"BU SORUYU TEKRARSORMA?>1:E 0:H",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,0, TRUE))
				{
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait("G˜R˜ž HATASI",1);
					return STAT_NOK;
				}
				if(Buffer[0] == '1') autoGSMRetry=TRUE;
				else if (Buffer[0] == '0') autoGSMRetry=FALSE;									
			}
			else
			{
				return STAT_NOK;
			}
		
		}
	}

	if(!GSMCALL)
	{	
		Printer_WriteStr("success = T4\n");		
		if (Host_PrepareCall(&DialTarget) == STAT_OK)
		{
			Printer_WriteStr("success = T5\n");		
			if(PredialCall)
			{
				Printer_WriteStr("success = T6\n");		
				Status = ComHndlr_PreDialForegrnd ();
			}
			else
			{
				Printer_WriteStr("success = T7\n");		
				Status = ComHndlr_Call2(&DialTarget);
			}
		}


		/* get the statitics and update them*/
		Statistics_Communication ((void *)&Modem_Statitics,(usint)sizeof(Modem_Statitics),TRUE);
		Statistics_System ((void *)&SystemStatistics, (usint)sizeof(SystemStatistics), TRUE);
		if(SERVER_OPEN_TIMEOUT_FLAG)
		{
			Host_UpdateStatistics_timeout();
		}
		else
		{
			Host_UpdateStatistics();
		}

	}
	else
	{
		Printer_WriteStr("success = T8\n");		
		memset(GSMDialPrm.LineTelephoneNumber, ' ', GSM_MAX_PHONE_NUM);
		strcpy(GSMDialPrm.LineTelephoneNumber,tel_no_1);

		total_statitics_counts1=0;
		if(Mainloop_MakeDataCall() != STAT_OK) 
		{
			Printer_WriteStr("success = T9\n");		
			Status=FAIL_CONNECT;
			IS_HOST_CONNECTED_FLAG = FALSE;
		
			memset(GSMDialPrm.LineTelephoneNumber, ' ', GSM_MAX_PHONE_NUM);
			strcpy(GSMDialPrm.LineTelephoneNumber,tel_no_2);

			total_statitics_counts1++;
			SERVER_OPEN_TIMEOUT_FLAG==TRUE;

			Host_ConnectStatist('1');
			if(Mainloop_MakeDataCall() != STAT_OK) 
			{
				Printer_WriteStr("success = T10\n");		
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("  DATA  ARAMA      YAPILAMADI   ",1);
				EmvIF_ClearDsp(DSP_MERCHANT);
				Status=FAIL_CONNECT;
				IS_HOST_CONNECTED_FLAG = FALSE;
			
				Statistics_System ((void *)&SystemStatistics, (usint)sizeof(SystemStatistics), TRUE);
				if(SystemStatistics.PwrDownCnt > 0)
					d_Statistics2.power_fail++;
				Host_ConnectStatist('2');
				return Status; 
			}
			else
			{
				Printer_WriteStr("success = T11\n");		
				Status = CONNECTED_LINE;	/*MMM*/
				Statistics_System ((void *)&SystemStatistics, (usint)sizeof(SystemStatistics), TRUE);
				if(SystemStatistics.PwrDownCnt > 0)
					d_Statistics2.power_fail++;
				ConnectStatus=2;
			}
                 }
		else
		{
			Printer_WriteStr("success = T12\n");		
			Status = CONNECTED_LINE;	/*MMM*/
			Statistics_System ((void *)&SystemStatistics, (usint)sizeof(SystemStatistics), TRUE);
			if(SystemStatistics.PwrDownCnt > 0)
				d_Statistics1.power_fail++;
			ConnectStatus=1;
		}
		EmvIF_ClearDsp(DSP_MERCHANT);
	}

		/* get the statitics and update them*/

	switch(Status)
	{
		case CONNECTED_LINE :
		Printer_WriteStr("connected line\n");		
		GrphText_Cls (TRUE);
		Debug_GrafPrint2 (" BAGLANIYOR...  ", 3);
		memset (fpBuf,0,900);
		wait_for_open = TRUE;
		Timeout_Start_ServerOPen = Scan_MillSec();
		OPEN_IS_FOUND = FALSE;  /* Buyuk 'O' character i bulunamadý. Default deðer */
		do
		{
			if (Kb_Read()==MMI_ESCAPE) break;

			if ( (Scan_MillSec() - Timeout_Start_ServerOPen) > Timeout_Server_Open*1000)
			{
					wait_for_open = FALSE;
					Status = STAT_NOK;
					if(SERVER_OPEN_TIMEOUT_FLAG) /* ikinci defa server timeout oldu */
					{
						d_Statistics2.no_enq_from_host++;
					}
				        else
					{
					/* eger server open mesajý gelmiyorsa */
					        if((total_statitics_counts1) == 0)
						{
								d_Statistics1.no_enq_from_host++;
								FIRST_PHONE_FLAG = TRUE;
							}
							else
							{
								d_Statistics2.no_enq_from_host++;
							}
						}
						break;
					}
					if (Kb_Read()==MMI_ESCAPE) break;

				if(!GSMCALL)
				{
					if (ComHndlr_RxLineDirect ((byte*)fpBuf, RxLen)>0)
					{
							/* server open mesajý gelinceya kadar timeout suresi icinde bekliyor */
							for (i=0;i<900;i++)
							{
								if(!OPEN_IS_FOUND)
								{
									if(fpBuf[i] == 'O')
									OPEN_IS_FOUND = TRUE;
								}
/*								OPEN_IS_FOUND=TRUE;*/

								if(OPEN_IS_FOUND)
								{
									if(fpBuf[i] == 0x0A)
									{
										Status = STAT_OK;
										wait_for_open = FALSE;
										break;
									}

								}
							if (fpBuf[i] == 0)	break;
						}
					}
				}
				else if(Mainloop_GSM_ReceiveData(fpBuf, &RxLen, 35, ENQ, ENQ, 0)==STAT_OK) 
				{
					if (DEBUG_MODE)    
					{
						Printer_WriteStr("\nReceive\n");
						PrntUtil_BufAsciiHex((byte *)fpBuf,RxLen);
						Printer_WriteStr("\n");
					}
					OPEN_IS_FOUND = TRUE;
					Status = STAT_OK;
					wait_for_open = FALSE;
					break;

				}
				else
				{
					if(ConnectStatus==1)	d_Statistics1.no_enq_from_host++;
					if(ConnectStatus==2)	d_Statistics2.no_enq_from_host++;

					Status = STAT_NOK;
					break;
				}

		} while (wait_for_open);

				if (Status == STAT_OK)
				{
					IS_HOST_CONNECTED_FLAG = TRUE;  /* connection with HOST is achieved */
					/*Debug_GrafPrint2 ((char *)MESSAGE_CONNECTED, 3);*/
					GrphText_Cls (TRUE);
					Debug_GrafPrint2 ("   BAGLANDI    ", 3);
					return(STAT_OK);
				}
				IS_HOST_CONNECTED_FLAG = FALSE;
				return(STAT_NOK);

		case USER_BREAK:

			EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait((char *)MESSAGE_USERBREAK,WAIT_RESPONSE);
				return(STAT_NOK);

		case FAIL_CONNECT:
		case FAIL_WRONG_INPUT:
		case FAIL_NO_PARAM:
		default:break;
		}

	 IS_HOST_CONNECTED_FLAG = FALSE;        /* connection with HOST is achieved */
	 return(STAT_NOK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_Disconnect
 *
 * DESCRIPTION: Disconnects and ends call.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Host_Disconnect(void)
{
	if(!GPRS_USEGPRS || GRPS_GPRSfailed){
if(GSMCALL){
/*nevada*/
	if(Host_CommNevadaProt()!=STAT_OK)	
	{}
}
/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) return;
/* Toygar 13.05.2002 End */
		
		if (DEBUG_MODE) Printer_WriteStr("ComHndlr_EndCall\n");
		ComHndlr_EndCall();

	}else {
		if (DEBUG_MODE) Printer_WriteStr("GPRS_CloseConnection\n");
		GPRS_CloseConnection(lsGPRS_Connect_Param.sd);
	}

    IS_HOST_CONNECTED_FLAG = FALSE;      /* connection with HOST is broken */
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_SendReceive
 *
 * DESCRIPTION: Sends and receives messages.
 *
 * RETURN: Status
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Host_SendReceive(char *Buffer, boolean PinPad, boolean PinType, boolean CntrChr)
{
	char msg_900[17];
	byte  Status,PortTp;
	sint ret;
	boolean NAK_TO_NAK;
	sint  BufLen;
/* Toygar EMVed */
	char  fpBuf[900],strBufLen[5];
	char  SaveBufShort[10];
/* Toygar EMVed */
	usint RxLength,SaveBufLen;
	sint  Counter_NAK_POS;
	sint  Counter_NAK_HOST;
	ulint Start_Timeout;
	ulint Left_Timeout;
	sint  Retry;
	ulint  Starting_Time_Of_Trasaction, lbStart_Time, lbEnd_Time, lbEnd_Time2;
	char lbtemp_string[50];
	sint position;

	

	/*UPDATE 01_16  28/10/99 isdigit kontrolu için konuldu*/
	int digit2, digit3;

	char    temp_batch_time_c[9];   /* test amaclý */

	Status = STAT_NOK;      /* default value */

	NAK_TO_NAK = FALSE;		/* NAK a karþý NAK gelme durumunda TRUE olur */

	memset(fpBuf,0,900);
	memset(SaveBufShort,0,10);
	position=1;
	if (strlen(Buffer)>1)
	{
		if (!PinType)	fpBuf[0] = SI;
		else		    fpBuf[0] = STX;
		fpBuf[1] = 0;

/*-------data length--------*/
/*		if (PinType)*/
		if ((PinType) && (!GPRS_USEGPRS || GRPS_GPRSfailed))
		{
			BufLen = strlen(Buffer);
			sprintf(strBufLen,"%03d",BufLen );
			strcat(fpBuf, strBufLen);
			position=4;
		}
/*-------data length--------*/

		strcat(fpBuf,Buffer);
		BufLen = strlen(fpBuf);
		if (!PinType)
			fpBuf[BufLen++] = SO;
		else
		fpBuf[BufLen++] = ETX;


		fpBuf[BufLen++] = 0;
		Host_CalcLRC(fpBuf+1);
		memcpy(Received_Buffer,fpBuf,BufLen);
		memcpy(SaveBufShort,fpBuf,10);
	}
	else
	{
		strcpy(fpBuf,Buffer);
		BufLen = 1;
		memcpy(Received_Buffer,fpBuf,1);
		memcpy(SaveBufShort,fpBuf,1);
    }

	if (PinPad)
	{
		
		Start_Timeout               = Scan_MillSec();
		Starting_Time_Of_Trasaction = Start_Timeout; /* Transaction için baþlangýç süresi */

		/* set flag */
		NAK_OCCURRED_FLAG = FALSE;
		/* 01_18 25/02/2000 initialize counter before communication 25/02/2000 AKSIGORTA */
		Counter_NAK_HOST = 0;
		Counter_NAK_POS  = 0;

	do
	 {
/* Toygar EMVed */
		memset(fpBuf,0,900);
/* Toygar EMVed */
		memcpy(fpBuf,Received_Buffer,BufLen);
			if(SaveBufShort[0]==NAK) {
				 memset(fpBuf,0,900);
				 memcpy(fpBuf,SaveBufShort,2);
				 BufLen=1;
				}

		if(DEBUG_MODE)
		{
			lbStart_Time = Scan_MillSec();
		}
			if (GPRS_USEGPRS && !GRPS_GPRSfailed)
			{
				ret=GPRS_Send(lsGPRS_Connect_Param.sd,(byte*)fpBuf,BufLen);

			}
			else if(!GSMCALL)
		{
			ret=Host_SendMessage(fpBuf, BufLen);
		}
		else
		{
			ret=Mainloop_GSM_SendData(fpBuf, BufLen);
			/*MMM*/
		}


		if(DEBUG_MODE)
		{
			lbEnd_Time = Scan_MillSec();
			
			memset(lbtemp_string, 0, sizeof(lbtemp_string));
			sprintf(lbtemp_string,"Send Mes.Length: %d \n",BufLen);
			PrntUtil_Cr(2);
			Slips_PrinterStr(lbtemp_string);
			PrntUtil_Cr(2);

			memset(lbtemp_string, 0, sizeof(lbtemp_string));
			sprintf(lbtemp_string,"Time to sent: %d \n",(lbEnd_Time - lbStart_Time));
			PrntUtil_Cr(2);
			Slips_PrinterStr(lbtemp_string);
			PrntUtil_Cr(2);

		}

		if(ret == STAT_OK)
		{

			if (DEBUG_MODE)    
			{
				Printer_WriteStr ("Send :\n");
				PrntUtil_BufAsciiHex((byte *)fpBuf,BufLen);
			}

			Left_Timeout = POS_TIMEOUT - ((Scan_MillSec() - Start_Timeout)/1000);

				if (GPRS_USEGPRS && !GRPS_GPRSfailed)
				{
					memset(fpBuf,0,900);
					//					Mainloop_ClearDsp();
					//					Display_WriteStr(DSPL1,"YANIT BEKLEN˜YOR");
					Kb_Read();
					RxLength=900;
					Status = GPRS_Receive(lsGPRS_Connect_Param.sd,(byte*)fpBuf,&RxLength);



				}
				else if(!GSMCALL)
			{
				Status = Host_WaitForMessage(fpBuf,&RxLength,Left_Timeout,STX,ETX,1);
			}
			else
			{
				memset(fpBuf,0,900);
				RxLength=900;
				Status = Mainloop_GSM_ReceiveData(fpBuf, &RxLength, POS_TIMEOUT, STX, ETX, 1);
			}
	

			if(DEBUG_MODE)
			{
				lbEnd_Time2 = Scan_MillSec();
				
				memset(lbtemp_string, 0, sizeof(lbtemp_string));
				sprintf(lbtemp_string,"Time to receive: %d \n",(lbEnd_Time2 - lbStart_Time));
				PrntUtil_Cr(2);
				Slips_PrinterStr(lbtemp_string);
				PrntUtil_Cr(2);
			}

			if (Status == STAT_OK)
			{

				fpBuf[RxLength] = 0;

				if (DEBUG_MODE)
				{
					Printer_WriteStr ("Receive2 :\n");
					PrntUtil_BufAsciiHex((byte *)fpBuf,RxLength);
				}
				
				/* Check LRC*/
				if (Host_CheckLRC(fpBuf,RxLength) != STAT_OK)
				{

					if(GSMCALL) BufLen=1;
					if (DEBUG_MODE) Printer_WriteStr("LRC Problemi\n");
					memcpy(d_Transaction_Receive.message,"   LRC HATASI   ",16);
					memcpy(d_Transaction_Receive.confirmation,"***",3);
					NAK_TO_NAK = TRUE;
					Status=STAT_NOK;
					Received_Buffer[0]=NAK;
					Received_Buffer[1]=0;
					SaveBufShort[0]=NAK;
					SaveBufShort[1]=0;
					Counter_NAK_POS++;
					if(Counter_NAK_POS >= MAX_COMM_RETRY)
					{
						NAK_OCCURRED_FLAG = TRUE;	/* burada max POS_NAK durumunu gosterir */
					}
				}
				else
				{ /* SaveBuf1 lar degistirildi */
					memset(Received_Buffer, 0, 900);
					Received_Buffer[0] = STX;
					if((GSMCALL) && (!GPRS_USEGPRS || GRPS_GPRSfailed))
					{
						RxLength -= 4;
						memcpy(Received_Buffer+1, fpBuf+4, RxLength);
						memset(fpBuf, 0, 900);
						memcpy(fpBuf, Received_Buffer, RxLength);
					}else
					{
						memcpy(Received_Buffer+1, fpBuf+1, RxLength-1);
					}

					if (DEBUG_MODE) Printer_WriteStr("Comm. Success\n");
						/* successful case */
					if(SERVER_OPEN_TIMEOUT_FLAG)
					{
						d_Statistics2.successful++;
					}
					else
					{
						if((total_statitics_counts1) == 0)
							d_Statistics1.successful++;
						else
							d_Statistics2.successful++;
					}


					/* CHECK FOR 900 MESSAGES INDICATING ERROR CASES */
					if (fpBuf[1] == '9')
					{
						/* UPDATE 01_16 28/10/99  ilk 9 dan sonra  digit in numeric kontrolu yapýlýyor*/
						digit2 = isdigit(fpBuf[2]);
						digit3 = isdigit(fpBuf[3]);
						if( (digit2 == 0) || (digit3 == 0) )
						{
							PrntUtil_Cr(2);
							Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
							Slips_PrinterStr("NUMERIK OLMAYAN 9 LU CODE ALINDI");
							PrntUtil_Cr(7);
							Mainloop_DisplayWait("NUMERIK OLMAYAN 9 LU CODE ALINDI",1);
							return (STAT_NOK);
						}
							
						if (INSTALLATION && EOD_OK)
						{
							ReversalPending = TRUE;
							if (DEBUG_MODE) Printer_WriteStr("MK_ReversalPending TRUE 01\n");
/*							Trans_SendReversal();*/
						}
						
						/*UPDATE_0_13  24/09/99 asagýdaki sartýrda memcpy yerine sprintf
						kullanýlýyordu. Bu da yanlýk kullaným olduðu icin 900 bir mesaj
						geldiginde stack basmasýna neden oluyordu.*/
						memcpy(msg_900,fpBuf+1,16);
						msg_900[16]=0;
						
						/* eger gunsonunda 900 mesaj gelirse error slip basýlmýyor */
						/* 06_01 RECONCILATION Ýrfan Send_Buffer local tanýmlandýðýndan genel kullaným kaldýrýldý */
						/*if(Send_Buffer[1] != '6')*/
						if( (Received_Buffer[1] != 'G') && (Received_Buffer[1] != 'H') )	/* 04_03 21/11/2002 */  /* 04_05 'H' eklendi */
						{	/* non genius transactions */
						   if(Received_Buffer[2] != '6')
						   {
							/* print error slip */
							memcpy(d_Transaction_Receive.amount_surcharge, amount_surcharge,13);
							
							/* 07_15 25/11/2002. burada doldurulmasi gerekmiyor */
							/*Slips_FillCommonReceiptStructure(FALSE,0);*/

							/* 07_15 25/11/2002. 903 mesaji gelmesi durumunda mesaj ve confirmation code kisimlari dolduruldu */
							memcpy(d_Transaction_Receive.message, msg_900,16);
							memcpy(d_Transaction_Receive.confirmation, "   ",3);
							/*memcpy(d_Receipt.message,msg_900,16);
							d_Receipt.message[16]=0;*/
							
							/*Slips_PrintErrorReceipt();
							ERROR_RECEIPT = TRUE;*/

							Mainloop_DisplayWait(msg_900,2);

							Status = STAT_NOK;
							Counter_NAK_POS=MAX_COMM_RETRY;
/* Toygar EMVed */
							/*if (tran_type != BALANCE) ReversalPending = TRUE;*/	/* 06_30 batch upload isleminde reversal gonderilmemeli */
							if(Received_Buffer[1] != 'B')
							{
								if ((tran_type != BALANCE) && (INSTALLATION && EOD_OK)) {
										ReversalPending = TRUE;		/* 06_30 o bakimdan batch upload ise reversal pending set edilmeyecek */
										if (DEBUG_MODE) Printer_WriteStr("MK_ReversalPending TRUE 02\n");
								}
							}
/* Toygar EMVed */
						  }
						  else
						  {
							Status = STAT_NOK;
							PrntUtil_Cr(1);
							Slips_PrinterStr(msg_900);
							PrntUtil_Cr(6);

							/* UPDATE_01_18 18/05/2000 gün sonunda 900 mesaj geldiðinde, POS Timeout
							oluncaya kadar 900 mesaj basmamasý için eklendi. sadece 1 defa
							basýp idle loop a çýkacak*/
							NAK_OCCURRED_FLAG = FALSE;	/* baþarýlý 900 mesaj durumunu ifade eder */
							Counter_NAK_POS=MAX_COMM_RETRY; /* Döngüden çýkmasýný saðlar */
						  }
						}
						else
							{
								/* genius transactions */	/* 04_03 21/11/2002 */
							/* 04_05 27/11/2002 */
							memcpy(d_GTransaction_Receive.message, msg_900,16);
							memcpy(d_GTransaction_Receive.confirmation, "   ",3);

							/*memcpy(d_GReceipt.message,msg_900,16);
							d_GReceipt.message[16]=0;*/
			
							/*Slips_GPrintErrorReceipt();
							ERROR_RECEIPT = TRUE;*/

							Mainloop_DisplayWait(msg_900,2);

							if(!SORGULAMA)
							{
								 ReversalPending = TRUE;			/* 04_03 21/11/2002 */
								if (DEBUG_MODE) Printer_WriteStr("MK_ReversalPending TRUE 03\n");
							}
							Status = STAT_NOK;
							Counter_NAK_POS=MAX_COMM_RETRY;
						}

					}
					else
					{                                       
						/* UPDATE_0_12  20/09/99. Ikýncý sifre istegindeki hata kaldýrýldý. Ikýncý
						/* received message Send buffer'a kopyalanýyordu. Engellendi*/
						/* received buffer is copied to the receive structure */
					    /*	memcpy(Buffer,fpBuf,RxLength);
						Buffer[RxLength] = 0;  */

						/* Buffer is copied to the Received_Buffer which is global variable */
						memset(Received_Buffer,0,sizeof(Received_Buffer));      /* initialize Received_Buffer*/
						memcpy(Received_Buffer,fpBuf,RxLength);
						Received_Buffer[RxLength] = 0;

						Status = STAT_OK;
					}

				}

			}
			else if (Status==STAT_NOK)
			{
				if (DEBUG_MODE) Printer_WriteStr("Status==STAT_NOK\n");
				Counter_NAK_HOST=MAX_COMM_RETRY;
				NAK_OCCURRED_FLAG = TRUE;	/* burada receive de kaynaklanan hatayý gosterir */
				Status = AKNET_POS_TIMEOUT_ERROR;
				memcpy(d_Transaction_Receive.message,"   POS TIMEOUT  ",16);
				memcpy(d_Transaction_Receive.confirmation,"***",3);
				memset(Received_Buffer, 0, 900);
			}
			else if (Status == NAK) 
			{
				if (DEBUG_MODE) Printer_WriteStr("Status==NAK\n");
				if(NAK_TO_NAK)
				{
					if (DEBUG_MODE) Printer_WriteStr("NAK_TO_NAK\n");
					/* Update 01_18 27/03/2000 POS un gönderdiði NAK a karþý HOST'dan NAK alýnmasý durumu*/
					Counter_NAK_POS = MAX_COMM_RETRY;
					NAK_OCCURRED_FLAG = TRUE;	/* burada max POS_NAK durumunu gosterir */
					Status = AKNET_POS_TIMEOUT_ERROR;
					memcpy(d_Transaction_Receive.message,"   POS TIMEOUT  ",16);
					memcpy(d_Transaction_Receive.confirmation,"***",3);
					/* bm YTL 05/07/2004 Stack basma */
					memset(Received_Buffer, 0, 900);
					break;

				}
				else
				{
				    Status=STAT_NOK;  /* Received NAK, retry HOST_NAK durumunu gösterir*/
					Counter_NAK_HOST++;
					if(Counter_NAK_HOST >= MAX_COMM_RETRY)
					{
						NAK_OCCURRED_FLAG = TRUE;	/* burada max HOST_NAK durumunu gosterir */
					}
				}
			}
			else
			{
				if (DEBUG_MODE) Printer_WriteStr("POS Timeout\n");
				/* POS timeout durumunu gösterir */
				Counter_NAK_HOST = MAX_COMM_RETRY;	/* sadece while döngüsünü kýrmak için*/
				NAK_OCCURRED_FLAG = TRUE;	        /* bu atamalar yapýlýr.*/
				Status = AKNET_POS_TIMEOUT_ERROR;
				memcpy(d_Transaction_Receive.message,"   POS TIMEOUT  ",16);
				memcpy(d_Transaction_Receive.confirmation,"***",3);
				memset(Received_Buffer, 0, 900);
			}
		}
		else
		{
			if (DEBUG_MODE) Printer_WriteStr("Send Error\n");
			/* Send modulundeki hatayý ifade eder. */
			Counter_NAK_HOST++;		/* 3 defa HOST dan NAK geldiðini göstermez. Sadece döngüden çýkmasý
										saðlanmaya çalýþýlýyor */
			NAK_OCCURRED_FLAG = TRUE;	/* while döngüsünden çýkýlmasý için atama yapýldý.*/
			Status = AKNET_POS_TIMEOUT_ERROR;
			memcpy(d_Transaction_Receive.message,"   POS TIMEOUT  ",16);
			memcpy(d_Transaction_Receive.confirmation,"***",3);
			memset(Received_Buffer, 0, 900);
		}

	 } while ((Status != STAT_OK) && (Counter_NAK_POS < MAX_COMM_RETRY) && (Counter_NAK_HOST < MAX_COMM_RETRY));

	 
	if(Status == AKNET_POS_TIMEOUT_ERROR)
	{
		/* Hem Satis ve hem iptal de bir onceki mesaji basmamasi icin buraya konuldu */ 
		/* 04_06 Ayhan Add 09/12/2002 */
		memcpy(d_GTransaction_Receive.message, "   POS TIMEOUT  ", 16);
		memcpy(d_GTransaction_Receive.confirmation, "   ",3);
		/* 04_06 Ayhan end */
		
				/* POS Timeout Case */
		if(SERVER_OPEN_TIMEOUT_FLAG)
		{
			d_Statistics2.no_response_from_host++;
		}
		else
		{
			if((total_statitics_counts1) == 0)
				d_Statistics1.no_response_from_host++;
			else
				d_Statistics2.no_response_from_host++;
		}
	}
	else
	{
		if(NAK_OCCURRED_FLAG)
		{	/* host veya pos dan 3 defa NAK geldi veya NAK_TO_NAK case'inin oluþmasý*/
			if(SERVER_OPEN_TIMEOUT_FLAG)
			{
				d_Statistics2.carrier_lost++;
			}
			else
			{
				if((total_statitics_counts1) == 0)
					d_Statistics1.carrier_lost++;
				else
					d_Statistics2.carrier_lost++;
			}
		}
		else
		{
			/*900 'lu hatalarý icerir. basarýlý durum */
			Utils_IToA(total_batch_time,temp_batch_time_c); 
			Utils_LeftPad(temp_batch_time_c, '0', 8);
			total_batch_time = total_batch_time + ((Scan_MillSec() - Starting_Time_Of_Trasaction)/1000);

		}
	}
	 return(Status);
  }
  else
  {
		Host_InitUart();
		Uart_Flush (PINPAD_PORT);
/* Toygar EMVed */
/*		memset(fBuf,0,384);*/
		memset(fpBuf,0,900);
/* Toygar EMVed */
		do
		{
/* Toygar EMVed */
/*			memset(fpBuf,0,384);*/
			memset(fpBuf,0,900);
/* Toygar EMVed */
			memcpy(fpBuf,Received_Buffer,BufLen);
			if (Uart_SendBuf (PINPAD_PORT, (byte *)fpBuf, BufLen) > 0)
			{
				if (Kb_Read() == MMI_ESCAPE)
					return (STAT_NOK);
                if (DEBUG_MODE)    
				{
					Printer_WriteStr ("SENDnet :\n");
					PrntUtil_BufAsciiHex((byte *)fpBuf,BufLen);
				}
				Status = Host_Receive (fpBuf,&RxLength,1,PinType);
				if(!GSMCALL){
				    if (CntrChr)
				    {
				      if (Status == ACK)
					Status = Host_Receive (fpBuf,&RxLength,1,PinType);
				      else return (STAT_NOK);
			            }
				}
				if (Status == STAT_OK)
				{
					fpBuf[RxLength] = 0;
        			if (DEBUG_MODE)    
					{
						Printer_WriteStr ("RECEIVEnet :\n");
						PrntUtil_BufAsciiHex((byte *)fpBuf,RxLength);
					}
					/* Check LRC */
					if (Host_CheckLRC(fpBuf,RxLength) != STAT_OK)
					{
	        			if (DEBUG_MODE)    Printer_WriteStr ("Check LRC  FAIL:\n");
						Printer_WaitForTextReady();
						if(GSMCALL){ 
						memcpy(Buffer,fpBuf,RxLength);
						Buffer[RxLength] = 0;
						return STAT_OK;
						}else{
						Status=STAT_NOK;
						Retry=MAX_COMM_RETRY;
						}
					}
					else
					{
						if (DEBUG_MODE)    Printer_WriteStr ("Check LRC  OK:\n");

						memcpy(Buffer,fpBuf,RxLength);
						Buffer[RxLength] = 0;
						Status = STAT_OK;
					}
				}
				else if (Status == NAK) Status=STAT_NOK;  
				else if (Status == ACK) Retry=MAX_COMM_RETRY;
				else if (Status == BEL)
				{
					Status=STAT_EOF; 
					Retry=MAX_COMM_RETRY;
				}
				else if (Status == EOT) Retry=MAX_COMM_RETRY;
			}
			else
				Retry=MAX_COMM_RETRY;
			Retry++;
     } while ((Status != STAT_OK) && (Retry < MAX_COMM_RETRY));

	 return(Status);
  }
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_AuthorizationProcess
 *
 * DESCRIPTION: Makes the authorization process.
 *
 * RETURN: Status
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
/*byte Host_AuthorizationProcess(TransStruct * fpTransStruct)
{
	byte Len, Status;
	sint Ret, i, liCounter;
	char WrkBuf[256];
	char Temp[41];


	Len = 0;
		for(liCounter=0;liCounter<LEN_AMOUNT-3;liCounter++)
			if(!isdigit(fpTransStruct->Amount[liCounter])) break;
		memset(Temp,'0',LEN_AMOUNT-3);
		for (i=0; i<liCounter;i++)
			Temp[LEN_AMOUNT-3-liCounter+i] = fpTransStruct->Amount[i];

	memcpy(WrkBuf,Temp,LEN_AMOUNT-3);
	Len += LEN_AMOUNT-3;

    WrkBuf[Len++]=';';

		for (liCounter=0;liCounter<LEN_TRACKISO2;liCounter++)
			if (fpTransStruct->TrackISO2[liCounter]== '?')  break;
		memset (Temp, ' ', LEN_TRACKISO2);
		memcpy (Temp, fpTransStruct->TrackISO2, liCounter);

	memcpy(WrkBuf+Len, Temp, LEN_TRACKISO2);
	Len+= LEN_TRACKISO2;

	WrkBuf[Len++]=';';
	memset(WrkBuf+Len++,'0', 80);
	Len += 80;
	WrkBuf[Len] = 0;


	/* Send & Receive */
/*      Status = Host_SendReceive(WrkBuf,TRUE,TRUE,FALSE,FALSE);

	if (Status!=STAT_OK)
	{
		Host_Disconnect();
		return(STAT_NO_CONNECTION);
	}

	Host_SendCntrlChar(DLE);
	Host_SendCntrlChar(EOT);

	memcpy(fpTransStruct->AuthResponse,WrkBuf+1,LEN_AUTHRESPONSE);
	Mte_Wait (50);
	Host_Disconnect();
	return(STAT_OK);
}*/



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_Get12Sayi
 *
 * DESCRIPTION:
 *
 * RETURN: STAT_OK or STAT_INV
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
/*byte Host_Get12Sayi (void)
{
	sint Len, i;
	byte Status;
	HSStruct lsHSBuf;
	sint CounterDeneme;
	sint CounterNumber;
	char fpWrkBuf[256];

	if (Files_InitFile((char *)FILE_AS,LEN_HSSTRUCT) != STAT_OK)
		return(STAT_INV);
	for (CounterDeneme=0; CounterDeneme < DENEMESAYISI; CounterDeneme++)
	{
		fpWrkBuf[0]=ACK;
		fpWrkBuf[1]=0;
		Status = Host_SendReceive(fpWrkBuf,TRUE,TRUE,FALSE,FALSE);

		if (Status == EOT)
		{
			Host_Disconnect();
			return(STAT_INV);
		}
		else if (Status!=STAT_OK) return(STAT_INV);
		if (Status == STAT_OK)
		{
		    Len = 1;
			for (CounterNumber=0; CounterNumber < TESTSAYI; CounterNumber++)
		    {
			    memcpy (lsHSBuf.AkTestNumber[CounterNumber],
					fpWrkBuf+Len+CounterNumber*LEN_TESTSAYI,
					LEN_TESTSAYI);
			}
			Files_HSWrite(&lsHSBuf);
		}
	}
	return(STAT_OK);
}*/


/* --------------------------------------------------------------------------
 * FUNCTION NAME: ExternalPinpad
 *
 * DESCRIPTION:   
 *
 * NOTES:
 *
 * RETURN:
 * ------------------------------------------------------------------------ */
byte Host_ExternalPinpad (byte *EncyPin)
{
	byte Status;
	char WrkBuf[50];
	sint i, display_length;
	char display_amount[14];

	i=card_no_length;
	memset(WrkBuf,0,50);
	memcpy(WrkBuf,  "70.", 3);
	memcpy(WrkBuf+3, card_no, card_no_length);
	i+=3;
	WrkBuf[i++] = 0x1C;
	memcpy(WrkBuf+i, working_key, 16);

	memcpy(display_amount, amount_surcharge, 13);
	display_amount[13] = 0;
	Utils_ClearInsZeros(display_amount);

	display_length = strlen(display_amount);

	if (display_length > 11)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MEBLA¦ BšYšK",2);
		return(STAT_NOK);
	}

	i+=19;
	memcpy(WrkBuf+i, display_amount, display_length);
	WrkBuf[i]=0;
/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,TRUE,FALSE);
	else Status = Host_SendReceive (WrkBuf,FALSE,TRUE,TRUE);
/* Toygar 13.05.2002 End */
	
	memset (EncyPin,0,16);
	
	/* UPDATE 01_18 irfan, eðer pinpad de þifre giriþi yerine clear basýlýrsa, EOT döner. 
	   Bu durumda Pinpad den doldurularak dönmesi gereken pinblock '0' ile pad edilir*/
	if(Status == EOT)
	{
		memset(EncyPin,'0',16);
		return(STAT_OK);
	}


	if (Status!=STAT_OK)
		return(STAT_NO_CONNECTION);


	if(memcmp(WrkBuf+1,"71",2)) return (STAT_NOK);
	else 
	{
		memcpy(EncyPin,WrkBuf+9,16);
		WrkBuf[0] = ACK;
		WrkBuf[1] = 0;
		if (Uart_SendBuf (PINPAD_PORT, (byte *)WrkBuf, strlen(WrkBuf) > 0))
			return (STAT_OK);
	}
	return (STAT_NOK);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Host_PinpadIdlePrompt
 *
 * DESCRIPTION:   
 *
 * NOTES:
 *
 * RETURN:        None.
 * ------------------------------------------------------------------------ */
byte Host_PinpadIdlePrompt (void)
{

if(GSMCALL) return (STAT_OK);

	byte Status;
	char WrkBuf[50];

	memset(WrkBuf,0,50);
	memcpy(WrkBuf,  "Z8", 2);
	memcpy(WrkBuf+2, "  AKBANK - POS  ", 16);

/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,FALSE,FALSE);
	else Status = Host_SendReceive (WrkBuf,FALSE,TRUE,FALSE);
/* Toygar 13.05.2002 End */

	if (Status == ACK)
		   return (STAT_OK);

	return (STAT_NOK);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Host_SelectLanguage
 *
 * DESCRIPTION:   
 *
 * NOTES:
 *
 * RETURN:        None.
 * ------------------------------------------------------------------------ */
byte Host_SelectLanguage (void)
{
	byte Status;
	char WrkBuf[30];

	memset(WrkBuf,0,30);
	memcpy(WrkBuf,  "12T", 3);

/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Status = Host_BFirmaSendReceive(WrkBuf,FALSE,FALSE,FALSE,FALSE);
	else Status = Host_SendReceive(WrkBuf,FALSE,FALSE,FALSE);
/* Toygar 13.05.2002 End */

	if (Status == ACK)
		   return (STAT_OK);
	return (STAT_NOK);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Host_InitUart
 *
 * DESCRIPTION:   set default params and init uart
 *
 * NOTES:
 *
 * RETURN:        None.
 * ------------------------------------------------------------------------ */
void Host_InitUart (void)
{
	comm_param PinPad_param;

    Cpuhw_SwitchTo (_PINPAD);

    PinPad_param.numbits   = PINPAD_DATA_BITS;
    PinPad_param.baud_rate = PINPAD_BAUD;
    PinPad_param.parity    = PINPAD_PARITY;
    PinPad_param.rts_cts   = FALSE;
    PinPad_param.dtr_mode  = FALSE;

    Uart_Init (PINPAD_PORT, &PinPad_param);
	/*Uart_Init (UART_B, &PinPad_param);*/ /* debit islem icin kaldýrýlýdý */
}




/* --------------------------------------------------------------------------
 * FUNCTION NAME: Hostcomm_Receive
 *
 * DESCRIPTION:   
 *
 * NOTES:
 *
 * RETURN:        
 * ------------------------------------------------------------------------ */
byte Host_Receive (char *RxMessage,usint *RxLength,sint ExtraChars,boolean PinType)
{
    ulint Time_out, Time_Start;
    sint i, j;
    byte ch,PortTp;


	Time_Start = Scan_MillSec();
	i = 0;

	if (PinType)
		Time_out = TIMEOUT * 1000;
	else
		Time_out = PinPad_Timeout * 1000;

#ifndef _8000
	PortTp=PINPAD_PORT;
#else
	PortTp=_COM1;
	Time_out = 10000;
#endif

	while( (Scan_MillSec() - Time_Start) <= Time_out)
	  {
		if (Kb_Read() == MMI_ESCAPE)	return (STAT_NOK);

		if (Uart_GetByte (PortTp, &ch) == 1)
		{  
	  switch (ch)
	      {
		      case SI  :
		      case STX :
	      RxMessage[i++] = ch;
	      for(;;)              
	      {
				/* stack basmamasi icin konuldu irfan. 17/10/2002*/
				Kb_Read();

				if (Uart_GetByte (PortTp, &ch) == 1)
		 {
				RxMessage[i++] = ch;
					if (!PinType)
					{
						if (ch == SO) 
							break;
					} 
					else
						if (ch == ETX) break;                
				 }
		 if ((Scan_MillSec() - Time_Start) > Time_out)
					 return (STAT_NOK);
		 if (Kb_Read() == MMI_ESCAPE)
					 return (STAT_NOK);
	      } 

	      for (j =0; j<ExtraChars; )
	      {
				if (Uart_GetByte (PortTp, &ch) == 1)
		      RxMessage[i+j++] = ch;
		 if ((Scan_MillSec() - Time_Start) > Time_out)
					  return (STAT_NOK);
		 if (Kb_Read() == MMI_ESCAPE)
					  return (STAT_NOK);
	      }
	      
	      *RxLength = i+j;                    
	      Host_SetParity ((byte *) RxMessage, *RxLength, CLEAR_PARITY);
	      return (STAT_OK);
	  case NAK :
			return (NAK);
	      case EOT:
		       return (EOT);
	      case ACK:
		       return (ACK);
	      case BEL:
		       return (BEL);
	  default :
			  break;
	      }          
		}
	}
	return (STAT_NOK);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: TransferMasterKey
 *
 * DESCRIPTION:   
 *
 * NOTES:
 *
 * RETURN:       
 * ------------------------------------------------------------------------ */
byte Host_TransferMasterKey (byte *key)
{
	byte Status;
	char WrkBuf[30];

	memset (WrkBuf,0,20);
	memcpy (WrkBuf,  "02", 2);
	memcpy (WrkBuf+2,"7" , 1);
	memcpy (WrkBuf+3, (char *)key,16);


/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf, FALSE, FALSE, TRUE, FALSE);
	else Status = Host_SendReceive (WrkBuf, FALSE, FALSE, TRUE);
/* Toygar 13.05.2002 End */
	
	if (Status!=STAT_OK)
		return(STAT_NO_CONNECTION);
	if(!memcmp(WrkBuf+1,"02",2))    
	{
		WrkBuf[0] = ACK;
		WrkBuf[1] = 0;

/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf, FALSE, FALSE, FALSE, FALSE);
		else Status = Host_SendReceive (WrkBuf, FALSE, FALSE, FALSE);
/* Toygar 13.05.2002 End */

		if (Status!=EOT)
			return(STAT_NO_CONNECTION);
		return(STAT_OK);
	}
	return(STAT_NOK);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: SelectMasterKey
 *
 * DESCRIPTION:   
 *
 * NOTES:
 *
 * RETURN:       
 * ------------------------------------------------------------------------ */
/*byte Host_SelectMasterKey (void)
{
	byte Status;
	char WrkBuf[30];

	memset (WrkBuf,0,20);
	memcpy (WrkBuf,  "08", 2);
	memcpy (WrkBuf+2,"7" , 1);

	Status = Host_SendReceive(WrkBuf,FALSE,FALSE,TRUE);
	if (Status == EOT)
		   return (STAT_OK);
	return (STAT_NOK);
}*/

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Host_CancelSessionRequest
 *
 * DESCRIPTION:
 *
 * NOTES:
 *
 * RETURN:
 * ------------------------------------------------------------------------ */
byte Host_CancelSessionRequest (void)
{
  char WrkBuf[5];
 
	memset(WrkBuf,0,5);
	memcpy(WrkBuf, "72", 2);

/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,FALSE,FALSE);
	else Host_SendReceive (WrkBuf,FALSE,TRUE,FALSE);
/* Toygar 13.05.2002 End */

	return (STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Host_UpdateStatistics
 *
 * DESCRIPTION          : Update the statistics
 *
 * RETURN                       : none.
 *
 * NOTES                        : r.i.o & t.c.a 23/06/99
 *
 * ------------------------------------------------------------------------ */
void Host_UpdateStatistics(void)
{
	if(!GSMCALL)
	{

/*************************** BÝRÝNCÝ server a ait istatistikler *********************************/

	total_statitics_counts1 =   Modem_Statitics.PhoneStat[0].NoDialTone +
								Modem_Statitics.PhoneStat[0].NoLine +
								Modem_Statitics.PhoneStat[0].NoRingBack +
								Modem_Statitics.PhoneStat[0].Busy +
								Modem_Statitics.PhoneStat[0].NoAnswer +
								Modem_Statitics.PhoneStat[0].NoAnswerTone+
								Modem_Statitics.PhoneStat[0].NoHandshake;

	
	
/*      d_Statistics1.successful --> Host_connected olduktan sonra baþarýlý durumda artýyo */

/* verifone da ki NoLine ve NoTone Lipman için NoTone da toplanýyor */
	if((Modem_Statitics.PhoneStat[0].NoDialTone != 0) || (Modem_Statitics.PhoneStat[0].NoLine !=0))
		d_Statistics1.no_tone++;
	
	/* modem cevap vermiyor,  */
	if(Modem_Statitics.PhoneStat[0].NoAnswerTone != 0)
	d_Statistics1.no_answer++;

	/* Handshake Error  */
	if(Modem_Statitics.PhoneStat[0].NoHandshake != 0)
	d_Statistics1.handshake_error++;

	/* carrier lost, Lipmanda NoRingBack ve NoAnswer da toplanýyr */
	if( (Modem_Statitics.PhoneStat[0].NoRingBack != 0) || (Modem_Statitics.PhoneStat[0].NoAnswer != 0))
	d_Statistics1.carrier_lost++;
	
	/* aranan numara meþgul */
	if(Modem_Statitics.PhoneStat[0].Busy  != 0)
		d_Statistics1.busy++;

	
	/*  pos timeout durumu, host conected olduktan sonra serverdan hiç bir mesaj gelmeme duru */
	/*  ilgili yerde artýrýlýyor  (d_Statistics1.no_response_from_host)*/
	

	/* access server tarafýndan server_open mesajý gonderilmeme durumu */
	/* ilgili yerde artýryor (d_Statistics1.no_enq_from_host)*/


/*************************** ýkýncý server a ait istatistikler *********************************/
/*

	total_statitics_counts2 =   Modem_Statitics.PhoneStat[1].NoDialTone +
								Modem_Statitics.PhoneStat[1].NoLine +
								Modem_Statitics.PhoneStat[1].NoRingBack +
								Modem_Statitics.PhoneStat[1].Busy +
								Modem_Statitics.PhoneStat[1].NoAnswer +
								Modem_Statitics.PhoneStat[1].NoAnswerTone+
								Modem_Statitics.PhoneStat[1].NoHandshake;
*/
	
	
/*      d_Statistics2.successful --> Host_connected olduktan sonra baþarýlý durumda artýyo */

/* verifone da ki  NoTone counter'ý Lipman da NoDialTone NoLine da toplanýyor */
	if((Modem_Statitics.PhoneStat[1].NoDialTone != 0) || (Modem_Statitics.PhoneStat[1].NoLine !=0))
		d_Statistics2.no_tone++;
	
	/* modem cevap vermiyor,  */
	if(Modem_Statitics.PhoneStat[1].NoAnswerTone != 0)
	d_Statistics2.no_answer++;

	/* Handshake Error  */
	if(Modem_Statitics.PhoneStat[1].NoHandshake != 0)
	d_Statistics2.handshake_error++;

	/* carrier lost, Lipmanda NoRingBack ve NoAnswer da toplanýyr */
	if( (Modem_Statitics.PhoneStat[1].NoRingBack != 0) || (Modem_Statitics.PhoneStat[1].NoAnswer != 0))
	d_Statistics2.carrier_lost++;
	
	/* aranan numara meþgul */
	if(Modem_Statitics.PhoneStat[1].Busy  != 0)
		d_Statistics2.busy++;

	
	/*  pos timeout durumu, host conected olduktan sonra serverdan hiç bir mesaj gelmeme duru */
	/*  ilgili yerde artýrýlýyor  (d_Statistics2.no_response_from_host)*/
	

	/* access server tarafýndan server_open mesajý gonderilmeme durumu */
	/* ilgili yerde artýryor (d_Statistics2.no_enq_from_host)*/

			/****** HER IKI SERVER A AIT ORTAK COUNTER ********/

	/* Bu counter her iki cihaz icin ortak olup sadece 1. server istatistiklerinde
		gönderilmektedir */
	if(SystemStatistics.PwrDownCnt  != 0)
		d_Statistics1.power_fail++;
	}
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Host_UpdateStatistics_timeout
 *
 * DESCRIPTION          : Update the statistics in case of server time out
 *
 * RETURN               : none.
 *
 * NOTES                : r.i.o & t.c.a 04/08/99
 *
 * ------------------------------------------------------------------------ */
void Host_UpdateStatistics_timeout(void)
{
	if(!GSMCALL)
	{
	
/*      d_Statistics1.successful --> Host_connected olduktan sonra baþarýlý durumda artýyo */

/* verifone da ki NoLine ve NoTone Lipman için NoTone da toplanýyor */
	if((Modem_Statitics.PhoneStat[0].NoDialTone != 0) || (Modem_Statitics.PhoneStat[0].NoLine !=0))
		d_Statistics2.no_tone++;
	
	/* modem cevap vermiyor,  */
	if(Modem_Statitics.PhoneStat[0].NoAnswerTone != 0)
	d_Statistics2.no_answer++;

	/* Handshake Error  */
	if(Modem_Statitics.PhoneStat[0].NoHandshake != 0)
	d_Statistics2.handshake_error++;

	/* carrier lost, Lipmanda NoRingBack ve NoAnswer da toplanýyr */
	if( (Modem_Statitics.PhoneStat[0].NoRingBack != 0) || (Modem_Statitics.PhoneStat[0].NoAnswer != 0))
	d_Statistics2.carrier_lost++;
	
	/* aranan numara meþgul */
	if(Modem_Statitics.PhoneStat[0].Busy  != 0)
		d_Statistics2.busy++;

	
	/*  pos timeout durumu, host conected olduktan sonra serverdan hiç bir mesaj gelmeme duru */
	/*  ilgili yerde artýrýlýyor  (d_Statistics1.no_response_from_host)*/
	

	/* access server tarafýndan server_open mesajý gonderilmeme durumu */
	/* ilgili yerde artýryor (d_Statistics1.no_enq_from_host)*/


	/* Bu counter her iki cihaz icin ortak olup sadece 1. server istatistiklerinde
		gönderilmektedir */
	if(SystemStatistics.PwrDownCnt  != 0)
		d_Statistics1.power_fail++;
	}
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Host_TestPrint
 *
 * DESCRIPTION          : Update the statistics
 *
 * RETURN                       : none.
 *
 * NOTES                        : r.i.o & t.c.a 28/06/99
 *
 * ------------------------------------------------------------------------ */
void Host_TestPrint(void)
{
	char temp[25];

	
	sprintf(temp,"tel_no_1: %s\n",tel_no_1);
	temp[24] = 0;
	Slips_PrinterStr(temp);
	
	sprintf(temp,"tel_no_2: %s\n\n",tel_no_2);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	/* amex no */
	memset(temp, 0, sizeof(temp));
	sprintf(temp,"AMEX_no: %s\n",merch_AMEX_no);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	/* isyeri no */
	memset(temp, 0, sizeof(temp));
	sprintf(temp,"isyeri_no: %s\n",merch_no);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	/* cihaz no */
	memset(temp, 0, sizeof(temp));
	sprintf(temp,"cihaz_no: %s\n",merch_device_no);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	memset(temp, 0, sizeof(temp));
	sprintf(temp,"Auto_EOD: %4d\n",auto_EOD_time_int);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	memset(temp, 0, sizeof(temp));
	sprintf(temp,"Line_Type: %c\n",Line_Type);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	memset(temp, 0, sizeof(temp));
	sprintf(temp,"param_ver_no: %s\n",param_ver_no);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	memset(temp, 0, sizeof(temp));
	sprintf(temp,"yazilim ver: %c\n",POS_software_ver);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	memset(temp, 0, sizeof(temp));
	sprintf(temp,"Top.Isl.Sur: %d\n",total_batch_time);
	temp[24] = 0;
	Slips_PrinterStr(temp);

	/* @bm #3 11.05.2004 */
	memset( temp, 0, sizeof( temp ) );
	sprintf( temp, "Kampanya Kontrol:%03d\n", KAMPANYA_FLAG );
	temp[strlen( temp )] = 0;
	Slips_PrinterStr( temp );
	PrntUtil_Cr(2);	/* 06_30 altýna gelen bolumden dolayý space 7 satýrdan 2 satýra indirildi */

}

/* 02_01 08/09/2000 irfan Pinpad den taksit sorma*/

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Host_GetInstallment
 *
 * DESCRIPTION:   Gets the installment from pinpad
 *
 * NOTES:
 *
 * RETURN:		STAT_OK : successfull receiption, STAT_NO: failure in receiption
 *              R.Ýrfan ÖZTÜRK. 21/08/2000 for Faizli Taksitli Update: 02_01
 * ------------------------------------------------------------------------ */
byte Host_GetInstallment (byte *Pin_Installment)
{
	byte Status;
	char WrkBuf[50],ch;
	sint i, display_length;
	char display_amount[14];
	

	/* önce Z2 gitmeli	*/

	memset(WrkBuf,0,50);
	memcpy(WrkBuf,  "Z2", 2);
	WrkBuf[2] = 0x1A;
	memcpy(WrkBuf+2+1, "TAKSIT SAYISI ?", 15);

/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,FALSE,FALSE);
	else Status = Host_SendReceive (WrkBuf,FALSE,TRUE,FALSE);
/* Toygar 13.05.2002 End */

	if (Status != ACK)
		   return (STAT_NOK);

	/* Taksit sayýsýný alan kýsým */
	memset(WrkBuf,0,50);
	memcpy(WrkBuf,  "Z50", 3);
	WrkBuf[3] = '1';
	memcpy(WrkBuf+4,  "100", 3);
	memcpy(WrkBuf+4+3,  "02", 2);

/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,TRUE,FALSE);
	else Status = Host_SendReceive (WrkBuf,FALSE,TRUE,TRUE);
/* Toygar 13.05.2002 End */

	/* Eðer giriþ yerine CLR e basýldý ise EOT dönecektir. Bu durumda EOT return edilerek
	   iþlemin kesilmesi saðlanýr 22/08/2000*/

	if(Status == EOT) return (Status);
	
	if (Status!=STAT_OK)
		return(STAT_NO_CONNECTION);
	
	
	if(memcmp(WrkBuf+1,"Z51",3)) return (STAT_NOK);
	else 
	{
		/* girilen digit kontrolleri */
		if(WrkBuf[4] == 0x03)	/* eger Enter ile hic digit girilmedi ise taksit sayýsý 1
									kabul edilecek */
		{
			WrkBuf[4] = '0';
			WrkBuf[5] = '1';
		}
		else	/* eger tek digit girildi ise */
		{
			if(WrkBuf[5] == 0x03)
			{
				ch = WrkBuf[4];
				WrkBuf[4] = '0';
				WrkBuf[5] = ch;
			}
		}

		memcpy(Pin_Installment,WrkBuf+4,2);
		WrkBuf[0] = ACK;
		WrkBuf[1] = 0;
	
		if (Uart_SendBuf (PINPAD_PORT, (byte *)WrkBuf, strlen(WrkBuf)) > 0)
			return (STAT_OK);
	}
	
	return (STAT_NOK);

}



/* Toygar 13.05.2002 Start */

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_BFirmaSendReceive
 *
 * DESCRIPTION: Sends and receives messages.
 *
 * RETURN: Status
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Host_BFirmaSendReceive(char *Buffer, boolean PinPad, boolean PinType, boolean CntrChr, boolean Wait_Response)
{
	byte  Status;
	boolean Waiting_ACK;
	sint  BufLen;
/* Toygar EMVed */
/*
	char  fpBuf[384];
	char  fBuf[384];
	char  SaveBuf[384];
*/
	char  fpBuf[900];
	char  SaveBuf[900];
/* Toygar EMVed */
	usint RxLength;
	ulint Transaction_start_time, Left_timeout, Time_now;
	sint  Retry;

	int digit2, digit3;
	char msg_900[17];
	struct EOD_Receive				d_EOD_Receive_my;

	Status = STAT_NOK;      /* default value */
	Waiting_ACK = CntrChr;
	Retry = 0;

	if (strlen(Buffer)>1)
	{
		if (!PinType) fpBuf[0] = SI; else fpBuf[0] = STX;

		fpBuf[1] = 0;
		strcat(fpBuf,Buffer);
		BufLen = strlen(fpBuf);

		if (!PinType) fpBuf[BufLen++] = SO; else fpBuf[BufLen++] = ETX;

		fpBuf[BufLen++] = 0;
		Host_CalcLRC(fpBuf+1);
	}
	else
	{
		strcpy(fpBuf,Buffer);
		BufLen = 1;
    }
	memcpy(SaveBuf,fpBuf,BufLen);

	/* PC 'ye mesaj gonderme*/
	if (PinPad)
	{
		Transaction_start_time = Scan_MillSec();
		Host_Init_BFirma_Port ();
		Uart_Flush (BFIRMA_PORT);

		do
		{
/* Toygar EMVed */
/*			memset(fpBuf,0,384);*/
			memset(fpBuf,0,900);
/* Toygar EMVed */
			memcpy(fpBuf,SaveBuf,BufLen);

			if (Uart_SendBuf (BFIRMA_PORT, (byte *)fpBuf, BufLen) > 0)
			{
				if (DEBUG_MODE)    
				{
					Printer_WriteStr ("Send :\n");
					PrntUtil_BufAsciiHex((byte *)fpBuf,BufLen);
					PrntUtil_Cr(1);
				}

				/** XLS_INTEGRATION2 23/10/2001 irfan. Host ile haberlesme sirasinda ESC ile
				    islemin kesilmemesi saglanir. **/
				/*if (Kb_Read() == MMI_ESCAPE)
					return (STAT_NOK);*/
					Kb_Read();

				if ((!CntrChr) && (!Wait_Response)) return (STAT_OK);		
				if (CntrChr && Waiting_ACK)
				{
					/*Status = Host_Receive_BFirma (fBuf, &RxLength, Bfirma_Ack_Timeout, 0);*/
					Status = Host_Receive_BFirma (fpBuf, &RxLength, Bfirma_Ack_Timeout, 0);	
					if (Status == ACK)
					{
						if (DEBUG_MODE)    
						{
							Printer_WriteStr ("ACK \n");
							PrntUtil_Cr(1);
						}

						Transaction_start_time = Scan_MillSec();
						Waiting_ACK = FALSE;

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
/*							if (DEBUG_MODE) Printer_WriteStr("STAT_TIMEOUT\n");*/
							memcpy(d_Transaction_Receive.message,"   LRC HATASI   ",16);
							memcpy(d_Transaction_Receive.confirmation,"***",3);
							if (Wait_Response) 
								Mainloop_DisplayWait(" G™NDER˜LEMED˜  ",2);
							return (STAT_TIMEOUT);
						}
					}
					else
					{
/*						if (DEBUG_MODE) Printer_WriteStr("STAT_DEFAULT\n");*/
						memcpy(d_Transaction_Receive.message,"  POS  TIMEOUT  ",16);
						memcpy(d_Transaction_Receive.confirmation,"***",3);
						if (Wait_Response) 
							Mainloop_DisplayWait(" G™NDER˜LEMED˜  ",2);
						return (STAT_TIMEOUT);
					}
				}

				if ((!Waiting_ACK) && Wait_Response)
				{
					Time_now = Scan_MillSec();
	
					if ((Time_now < Transaction_start_time) || ((Time_now - Transaction_start_time) > Bfirma_Message_Timeout))
						return (STAT_TIMEOUT);

					Left_timeout = Bfirma_Message_Timeout - (Time_now - Transaction_start_time);

					/*Status = Host_Receive_BFirma (fBuf, &RxLength, Left_timeout, 1);*/
					Status = Host_Receive_BFirma (fpBuf, &RxLength, Left_timeout, 1);

					if (Status == STAT_OK)
					{
						/*fBuf[RxLength] = 0;*/
						fpBuf[RxLength] = 0;

						if (DEBUG_MODE)
						{
							Printer_WriteStr ("Receive :\n");
							PrntUtil_BufAsciiHex((byte *)fpBuf,RxLength);
							/*PrntUtil_BufAsciiHex((byte *)fBuf,RxLength);*/
							PrntUtil_Cr(1); 
						}

						/* Check LRC */
						/*if (Host_CheckLRC(fBuf,RxLength) != STAT_OK)*/
						if (Host_CheckLRC(fpBuf,RxLength) != STAT_OK)
						{
/*							if (DEBUG_MODE) Printer_WriteStr("LRC Problemi\n");*/
							memcpy(d_Transaction_Receive.message,"   LRC HATASI   ",16);
							memcpy(d_Transaction_Receive.confirmation,"***",3);
							Status=STAT_NOK;
							SaveBuf[0]=NAK;
							SaveBuf[1]=0;
						}
						else
						{
							/* CHECK FOR 900 MESSAGES INDICATING ERROR CASES */
							if ( fpBuf[1] == '9' )
							{
								/* UPDATE 01_16 28/10/99  ilk 9 dan sonra  digit in numeric kontrolu yapýlýyor */
								digit2 = isdigit( fpBuf[2] );
								digit3 = isdigit( fpBuf[3] );
								if ( ( digit2 == 0 ) || ( digit3 == 0 ) )
								{
									PrntUtil_Cr( 2 );
									Slips_PrinterStr
										( "***BOZUK MESAJ ALINDI***\n" );
									Slips_PrinterStr
										( "NUMERIK OLMAYAN 9 LU CODE ALINDI" );
									PrntUtil_Cr( 7 );
									Mainloop_DisplayWait
										( "NUMERIK OLMAYAN 9 LU CODE ALINDI", 1 );
									return ( STAT_NOK );
								}
	
								if ( INSTALLATION && EOD_OK )
								{
									ReversalPending = TRUE;
	/*							Trans_SendReversal();*/
								}
	
								/*UPDATE_0_13  24/09/99 asagýdaki sartýrda memcpy yerine sprintf
								 * kullanýlýyordu. Bu da yanlýk kullaným olduðu icin 900 bir mesaj
								 * geldiginde stack basmasýna neden oluyordu. */
								memcpy( msg_900, fpBuf + 1, 16 );
								msg_900[16] = 0;
	
								/* eger gunsonunda 900 mesaj gelirse error slip basýlmýyor */
								/* 06_01 RECONCILATION Ýrfan Send_Buffer local tanýmlandýðýndan genel kullaným kaldýrýldý */
								/*if(Send_Buffer[1] != '6') */
	
								if ( SaveBuf[2] != '6' )
								{
									/* print error slip */
									memcpy( d_Transaction_Receive.
											amount_surcharge, amount_surcharge,
											13 );
	
									/* 07_15 25/11/2002. burada doldurulmasi gerekmiyor */
									/*Slips_FillCommonReceiptStructure(FALSE,0); */
	
									/* 07_15 25/11/2002. 903 mesaji gelmesi durumunda mesaj ve confirmation code kisimlari dolduruldu */
									memcpy( d_Transaction_Receive.message,
											msg_900, 16 );
									memcpy( d_Transaction_Receive.confirmation,
											"   ", 3 );
									/*memcpy(d_Receipt.message,msg_900,16);
									 * d_Receipt.message[16]=0; */
	
									Mainloop_DisplayWait( msg_900, 2 );
	
									Status = STAT_NOK;
									NAK_OCCURRED_FLAG = FALSE;	/* bm YTL 05/07/2004 Stack basma */
									/* baþarýlý 900 mesaj durumunu ifade eder */
									/* Döngüden çýkmasýný saðlar */
									Status = STAT_NOK;	/*bm gec */
									break;	/*bm gec */
								}
								else
								{
									Status = STAT_NOK;
									PrntUtil_Cr( 1 );
									Slips_PrinterStr( msg_900 );
									PrntUtil_Cr( 6 );
	
									memcpy( d_Transaction_Receive.message, msg_900, 16 );
									memcpy( d_Transaction_Receive.confirmation, "***", 3 );

									/* UPDATE_01_18 18/05/2000 gün sonunda 900 mesaj geldiðinde, POS Timeout
									 * oluncaya kadar 900 mesaj basmamasý için eklendi. sadece 1 defa
									 * basýp idle loop a çýkacak */
									NAK_OCCURRED_FLAG = FALSE;	/* baþarýlý 900 mesaj durumunu ifade eder */
									Status = STAT_NOK;	/*bm gec */
									break;	/*bm gec */
								}
	
							}
							else
							{
								if (!(INSTALLATION && EOD_OK))
								{
									if (DEBUG_MODE) Printer_WriteStr("MK903  02\n");
									memset(&d_EOD_Receive_my,0,sizeof(d_EOD_Receive_my));
									memcpy(&d_EOD_Receive_my,fpBuf+1,sizeof(d_EOD_Receive_my));
									if ( ( memcmp(d_EOD_Receive_my.confirmation_code, "902", 3) == 0 )  ||
										 ( memcmp(d_EOD_Receive_my.confirmation_code, "901", 3) == 0 ) ||
										 ( memcmp(d_EOD_Receive_my.confirmation_code, "903", 3) == 0 ))
									{
										Printer_Write(d_EOD_Receive_my.message,16);
										PrntUtil_Cr(4);
										return (STAT_NOK);
									}
								}
								/* Buffer is copied to the Received_Buffer which is global variable */
								memset(Received_Buffer,0,sizeof(Received_Buffer));      /* initialize Received_Buffer*/
								memcpy(Received_Buffer,fpBuf,RxLength);
								Received_Buffer[RxLength] = 0;
								Status = STAT_OK;
							}
					}
					}
					else
					{
/*						if (DEBUG_MODE) Printer_WriteStr("POS TIMEOUT\n");*/
						memcpy(d_Transaction_Receive.message,"  POS  TIMEOUT  ",16);
						memcpy(d_Transaction_Receive.confirmation,"***",3);
						return(Status);
					}
				}
			}

			Retry++;

		} while ((Status != STAT_OK) && (Retry < BFIRMA_COMM_RETRY));

		return(Status);

	}
	else
	{
		Host_InitUart();
		Uart_Flush (PINPAD_PORT);
/* Toygar EMVed */
		memset(fpBuf,0,900);
/* Toygar EMVed */
		do
		{
/* Toygar EMVed */
			memset(fpBuf,0,900);
/* Toygar EMVed */
			memcpy(fpBuf,SaveBuf,BufLen);
			if (Uart_SendBuf (PINPAD_PORT, (byte *)fpBuf, BufLen) > 0)
			{
				/** XLS_INTEGRATION2 23/10/2001 irfan. Host ile haberlesme sirasinda ESC ile
			    islemin kesilmemesi saglanir. **/
				/*if (Kb_Read() == MMI_ESCAPE)
					return (STAT_NOK);*/
					Kb_Read();

                if (DEBUG_MODE)
				{
					Printer_WriteStr ("SENDnet :\n");
					PrntUtil_BufAsciiHex((byte *)fpBuf,BufLen);
				}
				
				/*Status = Host_Receive (fBuf,&RxLength,1,PinType);*/
				Status = Host_Receive (fpBuf,&RxLength,1,PinType);
				if (CntrChr)  
				{
				  if (Status == ACK)
					/*Status = Host_Receive (fBuf,&RxLength,1,PinType);*/
					Status = Host_Receive (fpBuf,&RxLength,1,PinType);
				  else return (STAT_NOK);
				}
				if (Status == STAT_OK)
				{
					/*fBuf[RxLength] = 0;*/
					fpBuf[RxLength] = 0;

                    if (DEBUG_MODE)
					{
						Printer_WriteStr ("RECEIVEnet :\n");
						/*PrntUtil_BufAsciiHex((byte *)fBuf,RxLength);*/
						PrntUtil_BufAsciiHex((byte *)fpBuf,RxLength);
					}

					/* Check LRC */
					/*if (Host_CheckLRC(fBuf,RxLength) != STAT_OK)*/
					if (Host_CheckLRC(fpBuf,RxLength) != STAT_OK)
					{
						Status=STAT_NOK;
						Retry=MAX_COMM_RETRY;
					}
					else
					{
						/*memcpy(Buffer,fBuf,RxLength);*/
						memcpy(Buffer,fpBuf,RxLength);
						Buffer[RxLength] = 0;
						Status = STAT_OK;
					}
				}
				else if (Status == NAK) Status=STAT_NOK;  
				else if (Status == ACK) Retry=MAX_COMM_RETRY;
				else if (Status == BEL)
				{
					Status=STAT_EOF; 
					Retry=MAX_COMM_RETRY;
				}
				else if (Status == EOT) Retry=MAX_COMM_RETRY;
			}
			else
				Retry=MAX_COMM_RETRY;
			Retry++;
     } while ((Status != STAT_OK) && (Retry < MAX_COMM_RETRY));

	 return(Status);
  }
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Host_Send_Status
 *
 * DESCRIPTION	: 60 sn.'de bir cagrilir.  
 *				  Buyuk Firma PC'sine 'Statu' mesaji gonderir.
 * NOTES		:
 *
 * RETURN		: N/A
 * ------------------------------------------------------------------------ */
/*
void Host_Send_Status (void)
{
	byte Status;
	char WrkBuf[30];

	if (!BUYUKFIRMA) return;
	memset(WrkBuf,0,30);
	memcpy(WrkBuf, "11", 2);
	memcpy(WrkBuf+2, merch_device_no, 8);
	WrkBuf[10] = device_serial_no;
	memcpy(WrkBuf+11, "00", 2);
	Status = Host_BFirmaSendReceive(WrkBuf,TRUE, TRUE, FALSE, FALSE);
}
*/

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Host_Init_BFirma_Port
 *
 * DESCRIPTION	: Buyuk Firma Port'unun degerleri
 *
 * NOTES		: 20/10/99
 *
 * RETURN		: None.
 * ------------------------------------------------------------------------ */
void Host_Init_BFirma_Port (void)
{
	comm_param BFirma_Port_param;

    BFirma_Port_param.numbits   = BFIRMA_DATA_BITS;
    BFirma_Port_param.baud_rate = BFIRMA_BAUD;
    BFirma_Port_param.parity    = BFIRMA_PARITY;
    BFirma_Port_param.rts_cts   = FALSE;
    BFirma_Port_param.dtr_mode  = FALSE;

	Cpuhw_SwitchTo(_COM1);	/** XLS_INTEGRATION irfan. RS232'ye switch ediliyor **/
    Uart_Init (BFIRMA_PORT, &BFirma_Port_param);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Hostcomm_Receive_BFirma
 *
 * DESCRIPTION	:   
 *
 * NOTES		:
 *
 * RETURN		: STAT_NOK, STAT_OK, ACK, NAK
 * ------------------------------------------------------------------------ */
byte Host_Receive_BFirma (char *RxMessage, usint *RxLength, usint Left_Time, sint ExtraChars)
{
    ulint time_in_miliseconds, receive_start_time;
	sint i, j;
    byte ch;

	i = 0;
	receive_start_time = Scan_MillSec();


	for (;;)
	{
		Kb_Read();

		if (Uart_GetByte (BFIRMA_PORT, &ch) == 1)
		{  
			switch (ch)
			{
				case STX :

					RxMessage[i++] = ch;
					for(;;)              
					{
						Kb_Read();

						if (Uart_GetByte (BFIRMA_PORT, &ch) == 1)
						{

							RxMessage[i++] = ch;
							if (ch == ETX) break;                
						}
		
						time_in_miliseconds = Scan_MillSec();
						if ((time_in_miliseconds < receive_start_time) || ((time_in_miliseconds - receive_start_time) > Left_Time))
						{
						
							return (STAT_TIMEOUT);
						}

					} 


					for (j =0; j<ExtraChars; )
					{
						Kb_Read();
						if (Uart_GetByte (BFIRMA_PORT, &ch) == 1)
						{
							RxMessage[i + j] = ch;
							j++;	/* bm YTL 05/07/2004 Stack basma */
						}
						time_in_miliseconds = Scan_MillSec();
						if ((time_in_miliseconds < receive_start_time) || ((time_in_miliseconds - receive_start_time) > Left_Time))
						{
						
							return (STAT_TIMEOUT);
						}
					}

					*RxLength = i+j;                    

					Host_SetParity ((byte *) RxMessage, *RxLength, CLEAR_PARITY);


					return (STAT_OK);

				case NAK :
					return (NAK);
				case ACK:
					return (ACK);
				default :
				break;
			}          
		}

		time_in_miliseconds = Scan_MillSec();
		if ((time_in_miliseconds < receive_start_time) || ((time_in_miliseconds - receive_start_time) > Left_Time))
		{
		
		        return (STAT_TIMEOUT);
	    }
    }
}

/* Toygar 13.05.2002 End */

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME:	Host_ConnectStatist
 *
 * DESCRIPTION:		count and increase gsm modem statistics
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Host_ConnectStatist(byte Host)
{
	GSMStatistics   gsmstat;
	GSMStatistics   *GSMSTAT;
	memset (&gsmstat, 0, sizeof(gsmstat));
	GSMSTAT=&gsmstat;
	GSMSTAT->Size = sizeof (gsmstat);
	GSMRadio_GetStatistics(GSMSTAT, TRUE);

	if (DEBUG_MODE)    
	{
		Printer_WriteStr("\nGSMSTAT\n");
		PrntUtil_BufAsciiHex((byte *)GSMSTAT,sizeof(gsmstat));
		Printer_WriteStr("\n\n");
	}
	if(Host=='1')
	{
		if(GSMSTAT->NoDialTone!=0)	d_Statistics1.no_tone++;
		if(GSMSTAT->NoAnswer!=0)	d_Statistics1.no_answer++;
		if(GSMSTAT->Busy!=0)		d_Statistics1.busy++;
		if(GSMSTAT->NoCarrier!=0)	d_Statistics1.carrier_lost++;
	}
	if(Host=='2')
	{
		if(GSMSTAT->NoDialTone!=0)	d_Statistics2.no_tone++;
		if(GSMSTAT->NoAnswer!=0)	d_Statistics2.no_answer++;
		if(GSMSTAT->Busy!=0)		d_Statistics2.busy++;
		if(GSMSTAT->NoCarrier!=0)	d_Statistics2.carrier_lost++;
	}
/*
	if(GSMSTAT->NoNetwork>0)	stGSMSTATIST.siNO_NETWORK++;
	if(GSMSTAT->SIMError>0)		stGSMSTATIST.siSIM_ERROR++;
	if(GSMSTAT->PIN1Required>0)	stGSMSTATIST.siPIN1_REQUIRED++;
	if(GSMSTAT->PUK1Required>0)	stGSMSTATIST.siPUK1_REQUIRED++;
	if(GSMSTAT->PIN2Required>0)	stGSMSTATIST.siPIN2_REQUIRED++;
	if(GSMSTAT->PUK2Required>0)	stGSMSTATIST.siPUK2_REQUIRED++;	
	if(GSMSTAT->ModemFatalErr>0)	stGSMSTATIST.siMODEM_FATAL_ERROR++;
	if(GSMSTAT->SMSTxOK>0)		stGSMSTATIST.siSMS_SEND_COUNTER++;
	if(GSMSTAT->SMSRxOK>0)		stGSMSTATIST.siSMS_RECEIVE_COUNTER++;
	if(GSMSTAT->SMSTxErr>0)		stGSMSTATIST.siSMS_SEND_ERROR++;
	if(GSMSTAT->SMSRxErr>0)		stGSMSTATIST.siSMS_RECEIVE_ERROR++;*/
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME:	Host_ConnectStatist
 *
 * DESCRIPTION:		count and increase gsm modem statistics
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
int Host_CommNevadaProt(void)
{
	char SaveBuf1[900];
	byte Status=STAT_NOK;
	usint SaveBufLen;
	ulint EOT_TimeOut=5;
	memset(SaveBuf1,0,900);

	if(GSMCALL)
	{
		SaveBuf1[0]=STX;
		SaveBuf1[4]=ETX;
		SaveBuf1[5]=0;
		memcpy(SaveBuf1+1,"ACK",3);
		Host_CalcLRC(SaveBuf1+1);
		Status =Mainloop_GSM_SendData(SaveBuf1, 6);

		if (Status == STAT_OK)
		{
			if (DEBUG_MODE)    
			{
				Printer_WriteStr ("SendACKA :\n");
				PrntUtil_BufAsciiHex((byte *)SaveBuf1,6);
			}

			memset(SaveBuf1,0,900);
			SaveBufLen=1;
			Mte_Wait(60);
			Status = Mainloop_GSM_ReceiveData(SaveBuf1, &SaveBufLen, EOT_TimeOut, STX, ETX, 1);

			if (Status == STAT_OK)
			{
				if (DEBUG_MODE)    
				{
					Printer_WriteStr ("ReceiveEOTA :\n");
					PrntUtil_BufAsciiHex((byte *)SaveBuf1,SaveBufLen);
				}

				if(!memcmp(SaveBuf1+1,"EOT",3))
				{
					memset(SaveBuf1,0,900);
					SaveBuf1[0]=ACK;
					
					Status =Mainloop_GSM_SendData(SaveBuf1, 1);
					if(Status ==STAT_OK)
					{
						if (DEBUG_MODE)    
						{
							Printer_WriteStr ("Send(ACK):\n");
							PrntUtil_BufAsciiHex((byte *)SaveBuf1,1);
						}
						memset(SaveBuf1,0,900);
						SaveBufLen=1;
						Status = Mainloop_GSM_ReceiveData(SaveBuf1, &SaveBufLen, POS_TIMEOUT, EOT, EOT, 1);
						if (DEBUG_MODE)
						{
							Printer_WriteStr ("Receive(EOT):\n");
							PrntUtil_BufAsciiHex((byte *)SaveBuf1,1);
						}
					}
				}
				else	return STAT_NOK;

			}
/*			AKNET_POS_TIMEOUT_ERROR*/
		}
	}
	return Status;
}
