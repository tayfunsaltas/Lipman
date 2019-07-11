/**************************************************************************
  FILE NAME             : MAINLOOP.C
  MODULE NAME   : 
  PROGRAMMER    : REFIK IRFAN OZTURK
  DESCRIPTION   : Main Loop
  REVISION              : 01.00
**************************************************************************/

/* WRT 06/08/2001 Related Variables olivier */
#define DEF_INTERFAC
#include "project.def"        /* yeni */

/** XLS_PAYMENT include dosyalari 18/07/2001 irfan **/
#include INTERFAC_H

#include DEFINE_H
#include DEBUG_H
#include MESSAGE_H
#include UTILS_H
#include ERRORS_H
#include TRANS_H
#include FILES_H
#include SLIPS_H
#include HOSTCOMM_H
#include MAINLOOP_H
#include SMART_H
#include PROTOCOL_H
#include VERDE_H
#include GPRS_H
#include XLSCHIP_H
#include test_h
#include gsm_msg_def
#include gsmradio_prm
#include gsmradio_h
#include EMVDEFIN_H
#include EMVDEF_H
#include EMVMSG_H
#include EMVIF_H
#include EMVMMI_H
#include EMVDB_H
#include RSA_H
#include SHA1_H

/* 06_30 */
#include OFFLINE_H

/*#include SECURITY_H*/
#define DLE 0x10

extern char APN[12]; /*should be in gprs.h*/
extern char ISP_NAME[32]; /*should be in gprs.h*/

/* permanent flags */
extern boolean INSTALLATION;
extern boolean EOD_FLAG ;
extern boolean ERROR_RECEIPT;
extern boolean SECOND_COPY;             /* defined in trans.c */
extern boolean LAST_4_DIGIT_FLAG;
extern boolean BUYUKFIRMA;
extern char SoftPassword[5];
extern int BFIRMA_BAUD;

/* 06_30 offline  */
//bm 15.12.2004 extern char OfflinePassword[5];

extern boolean OFFLINE_ACIK_FLAG;	/*bm OFF 21.10.2004 */

boolean RestrictedFlag;/*@bm 01.09.2004 */
boolean gotemail;
/*==========================================*
*           D E F I N I T I O N S          *
*==========================================*/
extern boolean POSEntryFlagUsed;
byte oldbatlevel,sarj;

char GsmPin[LEN_GETKEYBOARD];
boolean FirstRun;

boolean BizSlip; //@bm  15.12.2004
extern char genius_seq_no_c[5];


/*boolean Debit;*/ /*verde kullanýlmadýgý icin kaldýrýldý 23/10/2002 */
boolean Debug;
boolean InitMasterKey;

extern GPRS_Connect_Param	lsGPRS_Connect_Param;
/* define.h içine alýndý*/
extern char APN[12];
char glb_host_ip[17];
usint glb_host_port;

extern char ISP_NAME[32]; /*should be in gprs.h*/
extern char GPRS_User[10];
extern char GPRS_Passwd[10];
extern boolean GPRS_CheckMode;
extern int genius_seq_no;
ulint GPRS_TryTime;
boolean autoGSMRetry;
boolean GPRS_USEGPRS;


boolean KEY_PRESSED_GLOBAL;                     /* global flag */
char manual_last_4_digit[5];

extern int Statistic_Server_Open;
extern char input_type;
extern char card_no[20];
extern int card_no_length;
extern char card_info[50];

/* 01_18 irfan 27/06/2000 card track 1 datasý icin tanýmlandý*/
boolean DEBUG_MODE;

/* v04_05 CALISMA_KOSULLARI_UPDATE  0:KAPALI 1:ACIK(default)*/
boolean CALISMA_KOSULLARI_MODE;

/* ara rapor iptali 10/10/2002*/
boolean ARA_RAPOR_IPTAL;
boolean ARA_RAPOR;	/* TRUE:ara rapor transactioninin cagrildigini gosterir*/

extern boolean INSTALLATION_ONGOING;

extern char card_info1[50];
char card_info1_name[25];
char card_info1_surname[25];

extern char last_4_digit[5];

extern char exp_date[5];
extern char cvv_2[4];
extern char approval_code[7];
int tran_type;
extern int record_counter;              /* defined in trans.c */
extern boolean FIRST_RUN;

extern struct Statistics                                        d_Statistics1;
extern struct Statistics                                        d_Statistics2;
extern char ATS_Prefixes[90];
extern char debit_prefix[07][10][7];            /* 30 yerine 10 yapýldý /*verde*/
extern boolean VOID_FLAG;
extern char UserPassword[5];
extern char OperatorPassword[15];
extern sint Cashier_Flag;			/* bm YTL 13/08/2004 Cashier menu */
extern boolean TL_Iade;				/* bm YTL 13.08.2004 */
extern char YTL_Slip[3];			/* bm YTL 13.08.2004 TLY - YTL */
extern char auto_EOD_time[5];
extern int auto_EOD_time_int;
extern boolean EOD_flag;

/* genius 04_00 */
int Gtran_type;
extern boolean SORGULAMA;
char last_genius_eod_date[9];
char genius_flag;

boolean TIME_IS_UP;             /* auto gun sonu saatinin gelip gelmedigini gosterir */
							/* TRUE: auto eod saati geldi, FALSE : auto eod saati gelmedi */
extern boolean  WARNING_FLAG;
extern boolean error_printed;

char last_eod_date[9];
extern boolean  FIRST_PHONE_FLAG;
extern boolean  SERVER_OPEN_TIMEOUT_FLAG;

/* UPDATE_01_14 20/10/99*/
boolean RETURN_PERMISSION;      /* iade fonksiyonuna izin verme flag'ý. 1: IADE AÇIK. 0: IADE KAPALI*/
/* UPDATE_01_18 21/21/2000*/
boolean MANUEL_PERMISSION;      /* MANUEL ÝÞLEM fonksiyonuna izin verme flag'ý. 1: MANUEL ÝÞLEM AÇIK. 0: MANUEL ÝÞLEM KAPALI*/
/* UPDATE_01_18 14/01/00 El ile giriþin kontrol edilmesini saðlamak için */
boolean MANUEL_ENTRY_PERMISSION;

/* UPDATE_01_18 28/06/00 ISIM YAZMA FLAG TANIMI*/
boolean ISIMYAZMA_FLAG;
boolean TRACK1ERROR;

/* UPDATE_01_18 08/12/99 2.koyanýn iþlem yok ise basýlmamasý için kullanýlacak*/
extern int seq_no;

/* 02_01 08/09/2000 Pinpad den taksit giriþ flag irfan*/
/* TRUE : Pinpad den giriþ   FALSE : POS den giriþ*/
boolean PINPAD_TAKSIT_FLAG; 
boolean GSMCALL;
boolean USE_GPRS;
int rState;

boolean PIN_BLOCK;
boolean SEND_EMV_FIELDCHANGES;

extern boolean EOD_OK;
extern boolean TVR_TSI;
boolean EMV_DEBUG_MODE;
boolean PRINTALLTAGVALUES;

const byte gsmlogoDisp [] = 
{ 
 0x3F, 0x8F, 0xE7, 0xFE, 0x60,
 0x40, 0x50, 0x28, 0x21, 0x60,
 0x40, 0x50, 0x08, 0x21, 0x00,
 0x40, 0x10, 0x09, 0xA1, 0x00,
 0x40, 0x0F, 0xC9, 0xA1, 0x00,
 0x47, 0xC0, 0x28, 0x21, 0x00,
 0x40, 0x40, 0x28, 0x21, 0x00,
 0x40, 0x50, 0x29, 0xAD, 0x00,
 0x3F, 0xDF, 0xC9, 0xAD, 0x00
};

const byte _SIGNAL_LEVEL [] = {
0xC0,
0xC0,
0xC0,
0xC0,
0xC0,
0xC0
};

const byte _TILDA2 [] = {

0x30,0x40,
0x48,0x40,
0x84,0x80,
0x83,0x00};


const byte _PIL_L0 [] = {
 0x0F, 0xC0,
 0x30, 0x30,
 0x43, 0x08,
 0x43, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x4F, 0xC8,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x60, 0x18,
 0x30, 0x30,
 0x0F, 0xC0
};

const byte _PIL_L1 [] = {
 0x0F, 0xC0,
 0x30, 0x30,
 0x43, 0x08,
 0x43, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x4F, 0xC8,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x7F, 0xD8,
 0x3C, 0xB0,
 0x0F, 0xC0
};

const byte _PIL_L2 [] = {
 0x0F, 0xC0,
 0x30, 0x30,
 0x43, 0x08,
 0x43, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x4F, 0xC8,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x7F, 0xD8,
 0x7A, 0x28,
 0x7C, 0x58,
 0x3A, 0x30,
 0x0F, 0xC0
};

const byte _PIL_L3 [] = {
 0x0F, 0xC0,
 0x30, 0x30,
 0x43, 0x08,
 0x43, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x4F, 0xC8,
 0x40, 0x08,
 0x40, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x7F, 0xD8,
 0x7A, 0x28,
 0x7C, 0x58,
 0x7A, 0x28,
 0x7C, 0x58,
 0x3A, 0x30,
 0x0F, 0xC0
};

const byte _PIL_L4 [] = {
 0x0F, 0xC0,
 0x30, 0x30,
 0x43, 0x08,
 0x43, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x4F, 0xC8,
 0x40, 0x08,
 0x70, 0x38,
 0x7F, 0xD8,
 0x7A, 0x28,
 0x7C, 0x58,
 0x7A, 0x28,
 0x7C, 0x58,
 0x7A, 0x28,
 0x7C, 0x58,
 0x3A, 0x30,
 0x0F, 0xC0
};
const byte _PIL_L5 [] = {
 0x0F, 0xC0,
 0x30, 0x30,
 0x43, 0x08,
 0x43, 0x08,
 0x40, 0x08,
 0x70, 0x38,
 0x7F, 0xD8,
 0x7C, 0x28,
 0x7A, 0x58,
 0x7C, 0x28,
 0x7A, 0x58,
 0x7C, 0x28,
 0x7A, 0x58,
 0x7C, 0x28,
 0x7A, 0x58,
 0x7C, 0x28,
 0x3A, 0x70,
 0x0F, 0xC0
};


/** XLS_PAYMENT related variables  12/07/2001 irfan **/
extern char Loyalty_Prefixes[90];
boolean RECOPY_FLAG;		/* TRUE : ikinci kopya basilacak FALSE : ikinci kopya basilmayacak */
boolean CORRUPTED_CHIP_TRANSACTION_FLAG;  /* TRUE: courrupted chip  FALSE: Not corrupted chip */
boolean	CHIP_INSERTED_GLOBAL;					/* global flag*/
char last_xls_eod_date[13];	/* 'CCYYMMDDHHMM\0' */
boolean XLS_EOD_TIME_IS_UP; /* xls auto gun sonu saatinin gelip gelmedigini gosterir */
							/* TRUE: xls auto eod saati geldi, FALSE : xls auto eod saati gelmedi */
boolean IS_LOYALTY_INSTALLMENT_INTEREST;	/* For Sale , Manuel Sale and Return Transactions 30/07/2001
											   TRUE  : Loyalty transaction with interes
											   FALSE : Loyalty transaction without interest */
sint image_number;			/** XLS_INTEGRATION2 05/11/2001 irfan logo animation icin kondu**/
char Not_Certified_Prefixes[35];
boolean OFFLINE_CARREFOURSA;
boolean OnlinePinEntry;
boolean PIN_BLOCK;
boolean SEND_EMV_FIELDCHANGES;
int KAMPANYA_FLAG;
boolean KampanyaFlag; /*@bm 16.09.2004 */
boolean OldKampanyaFlag;
char kampanya_str[15];
boolean AlreadyChecked;

extern char Kampanya_Data[15];
extern char Kasiyer_No[20];
extern char POS_Type;	/* Default axess secili. 0:Axess  1:Akbank */
extern boolean AILE_PUAN_SORGULAMA;

/*verde */
extern boolean VERDE_TRANSACTION;
extern boolean VERDE_TRX_CONTINUE;	/* verde 13/12/2002. TRUE: Trx. continue. FALSE: Trx. stop*/
extern char glAmount_Req_Type;

extern boolean EXTERNAL_PINPAD;
 const char AploadMessages[20][17]=       {
												{"GONDERIYOR  "},
												{"ALIYOR          "},
												{"PARAM. KOPYALA  "},
												{"PARAM+PROGRAM   "},
												{"MESAJ BEKLENÝYOR"},
												{"REQ! BEKLENIYOR"},
												{"ID BEKLENIYOR   "},
												{"DATA BEKLENIYOR "},
												{"ERASE! BEKLENIYO"},
												{"'DAN OKUMA      "},
												{"'A YAZMA        "},
												{"TRANS. BASARILI "},
												{"ILETISIM HATASI!"},
												{"FLASH ERASE ERR!"},
												{"FLASH WRITE ERR!"},
												{"YETERSIZ RAM    "},
												{"YETERSIZ   FLASH"},
												{"NOS  UYUSMAZLIGI"},
												{"HATA! ENT->Retry"},
												{"ESC->SystemReset"}
											};



 const FTPMsgType  Msg[FTP_NUM_MSG] =
{
	{"  ALIYOR %d                    "},
	{" G™NDER˜YOR %d                 "},
	{"DOSYA BEKLEN˜YOR               "},
	{"   G™NDERME        BEKLEN˜YOR  "}
};


GSMTargetParamStruct GSMDialPrm;
static GSMSmsFileStruct GsmMsg;
GSMSTATIST stGSMSTATIST;

extern boolean ReversalPending;
/* Toygar EMVed */
extern char merch_device_no[9];
extern int POSEntryFlag;
extern int BadICC;
extern boolean ChangesPending;
extern boolean KeyDownloadStarted;
extern boolean AppListDownloadStarted;
extern boolean ExceptionDownloadStarted;
extern boolean RevocationDownloadStarted;
extern const char Exception_File[];
extern const char Revocation_File[];
extern boolean ChangesPending;
extern boolean CallFlag;	/* 06_20 */

extern boolean EMVAxess;
boolean EMVAxessStatu = FALSE;				/* 19.07.2004 axess visa degisiklikleri */
extern char Received_Buffer[900];
extern boolean AMEX_FLAG;
extern int Reversal_Buffer_Len;
/*#define VERDE_PORT      UART_A*/


/* 05_02 16/09/2002 NOT_CERTIFIED_CARD irfan */
char Not_Certified_Prefixes[35];



/* Toygar EMVed */

/* 06_30 06/03/2003 */
boolean OFFLINE_CARREFOURSA;
boolean OnlinePinEntry;

char TL_Amount[25];
/*
char MyTL_Amount[40];
char MyMyTL_Amount[40];
*/

      /*==========================================*
	   * irfan GRAPHIC  DEFINITIOMNS   20/01/2001 *
	   *==========================================*/



      /*==========================================*
	   *       P R I V A T E  T Y P E S           *
	   *==========================================*/
	  /*==========================================*
	   *        P R I V A T E  D A T A            *
	   *==========================================*/


      /*=========================================*
       *     PRIVATE  FUNCTION   PROTOTYPES      *
       *=========================================*/
      /*=========================================*
       *    P U B L I C     F U N C T I O N S    *
       *=========================================*/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ClearDsp
 *
 * DESCRIPTION: 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*
void Mainloop_ClearDsp (void)
{
#ifndef GRAFDISP
	Display_ClrDsp();
#else

    GrphText_Cls(TRUE);
#endif
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_TerminateTran
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Main_TerminateTran(boolean blFallback)
{
/*MURAT
	ICC_Disactive (EX_CARD);
	EmvIF_ClearDsp(DSP_MERCHANT);
	if (blFallback) EmvIF_DisplayWait((EMVLanguages *)&elsLangs, DSP_MERCHANT, msg_EMV_USE_MAGNETIC_STRIPE, 1);
	else Mainloop_DisplayWait("   KARTINIZI      CIKARTINIZ    ",NO_WAIT);
*/

	if ( ResetCardExt(  ) == STAT_OK )
	{
		ICC_Disactive( EX_CARD );
		EmvIF_ClearDsp(  DSP_MERCHANT);
	}
	else
	{
		ICC_Disactive( EX_CARD );
		EmvIF_ClearDsp(DSP_MERCHANT  );
		if ( blFallback )
			EmvIF_DisplayWait( &elsLangs, DSP_MERCHANT,
							   msg_EMV_USE_MAGNETIC_STRIPE, 1 );
		else
			Mainloop_DisplayWait( "   KARTINIZI      CIKARTINIZ    ",
								  NO_WAIT );
	}

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_ApplicationSelection
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Main_ApplicationSelection (TerminalAppList *pTerminalApps, MutualAppList *pMutualApps,
				byte *pbMutualAppIndex, TerminalParams *pTermData, EMVLanguages *pelsLangs,
				byte *pbErr)
{
	byte rv;
	int i;

	/*Initialize mutually supported app list*/
	memset(pMutualApps, 0, sizeof(MutualAppList));
	switch(Emv_ApplSelect(pTerminalApps, pMutualApps, pbMutualAppIndex, pTermData, pelsLangs, pbErr ))
	{
		case EMV_OK:
/*			Printer_WriteStr("EMV_OK\n");*/
			for(i=0;i<pMutualApps->bAppCount;i++)
	{
/*
				Printer_WriteStr("AppLabel\n");
				PrntUtil_BufAsciiHex((byte *)pMutualApps->MutualApps[i].AppLabel,strlen((const char *)pMutualApps->MutualApps[i].AppLabel));
				Printer_WriteStr("AppPrefName\n");
				PrntUtil_BufAsciiHex((byte *)pMutualApps->MutualApps[i].AppPrefName,strlen((const char *)pMutualApps->MutualApps[i].AppPrefName));
*/
			}
			break;
		case EMV_FALLBACK :
			BadICC = 1;
/*			Printer_WriteStr("EMV_FALLBACK BadICC\n");*/
		default :
			BadICC |= (*pbErr == eNO_MUTUAL_APP);
/*			Printer_WriteStr("KARTTAKI APLIKASYONLAR\n");
			Printer_WriteStr("TERMINALDE BULUNMUYOR.\n");
			PrntUtil_Cr(4);
			Printer_WriteStr("eNO_MUTUAL_APP BadICC\n");
			Printer_WriteStr ("pbErr\n");
			PrntUtil_BufAsciiHex((byte *)pbErr,1);
*/
			Main_TerminateTran(BadICC);
		return(STAT_NOK);
	}
	/* Saving Terminal ID from terminal params*/
	EmvDb_StoreData(tagTERML_ID, BerTlv_AsBufToRd((byte*)merch_device_no, 8), srcTERMINAL);
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait((char *)csEMV_PLEASE_WAIT,NO_WAIT);
	rv = Emv_InitiateAppProcessing(pTerminalApps, pMutualApps, pbMutualAppIndex, pTermData, pelsLangs, pbErr);
	/**************************************************************/
	/* Changed by Alp Sardag 28-06-04		       */	
	/* CAUSE : Clarification Bulletin 18 at 18-05-04         */
	/**************************************************************/
	if (rv != EMV_OK) 
	{ 
		if (rv == EMV_FALLBACK)
		{
			BadICC = 1;
		}
		Main_TerminateTran(BadICC); 
		return STAT_NOK; 
	}
/*
	if (rv != EMV_OK)
	{
		Main_TerminateTran(BadICC);
		return STAT_NOK; 
	}
*/
	return(STAT_OK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_ReadApplicationData
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
/* Toygar - NextEMV Start - Ok */
/*
byte Main_ReadApplicationData(byte *abSDADataBuf, RowData *rdSDAData, EMVTransData *petdData, EmvApplication *pEmvApp)
*/
/* Toygar - NextEMV End - Ok */
byte Main_ReadApplicationData(byte *pbErr)
{
	byte rv = EMV_OK;
	
	/* This buffer will be used for storing SDA related data - if available*/
/* Toygar - NextEMV Start - Ok */
/*
	BerTlv_AssignBufferToRd(rdSDAData, abSDADataBuf, 0);
	rv = Emv_ReadApplicationData(rdSDAData);
*/
/* Toygar - NextEMV End - Ok */
	rv = Emv_ReadApplicationData(pbErr);
	if (rv != EMV_OK) 
	{
		Main_TerminateTran(BadICC);
		return STAT_NOK;
	}
	return(STAT_OK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_DAMethod
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
/* Toygar - NextEMV Start - Ok */
/* 
byte Main_DAMethod(RowData *rdDAData, EmvApplication *eaApplication)
*/
/* Toygar - NextEMV End - Ok */
byte Main_DAMethod(ApplicationParams *pAppData, byte *pbErr)
{

    if (KampanyaFlag == TRUE) return STAT_OK; /*@bm 16.09.2004*/

/* Toygar - NextEMV Start - Ok */
  if ((tran_type != BALANCE) && (tran_type != PREPROV) && (tran_type != MANUEL)&& (tran_type != RETURN)&& 
  	(!EMVAxess || EMVAxessStatu))
  if (Emv_AuthOfflineData(pAppData, pbErr) == EMV_TERMINATE_TRN)
/*
  if (EmvSec_AuthOfflineData(*rdDAData, eaApplication) == EMV_TERMINATE_TRN)	
*/
  {
    Main_TerminateTran(BadICC);
    return STAT_NOK;  
  }
/* Toygar - NextEMV End - Ok */
  return STAT_OK;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_CheckEMV
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
/* Toygar - NextEMV Start - Ok */
/*
byte Main_CheckEMV(RowData rdStatic, EMVTransData *petdData)
*/
/* Toygar - NextEMV End - Ok */
byte Main_CheckEMV(EMVLanguages *pelsLangs, EMVTransData *etdData, byte *pbErr)
{
	Emv_Process_Restrictions();
	/* verde. 1:amount pos tarafýndan saglanacak. 2: amount verde pod tarafýndan saglanacak.*/
	if( !VERDE_TRANSACTION || (glAmount_Req_Type == '1') ) 
	{
		if (!Emv_Amount(pelsLangs))
		{
/*			Printer_WriteStr("!Emv_Amount(pelsLangs)\n");*/
			return(STAT_NOK);
		}
	}
/* Toygar - NextEMV Start - Ok */
/*	
	if (Emv_VerifyCardHolder(rdStatic, petdData) == EMV_TERMINATE_TRN)
*/
/* Toygar - NextEMV End - Ok */
	if (Emv_VerifyCardHolder(pelsLangs, etdData, pbErr) == EMV_TERMINATE_TRN)
	{
/*		Printer_WriteStr("Emv_VerifyCardHolder Fail\n");*/
		Main_TerminateTran(BadICC);
		return(STAT_NOK);
	}
	return(STAT_OK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_InitExceptionList
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*
byte Main_InitExceptionList(void)
{
	byte aExceptionByteList[EXCEPTIONBYTELIST];

	if (Files_InitFile(Exception_File,sizeof(aExceptionByteList)) != STAT_OK) return(STAT_NOK);
	memset(aExceptionByteList, 0, sizeof(aExceptionByteList));
	memcpy(aExceptionByteList,"\x54\x13\x33\x90\x00\x00\x15\x96",8);
	if (Files_ExceptionUpdate(aExceptionByteList) != STAT_OK) return(STAT_NOK);
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_SearchExceptionList
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Main_SearchExceptionList(byte *Card)
{
	byte aExceptionByteList[EXCEPTIONBYTELIST];
	int i, j;
	
	j = 1;
	while (Files_ExceptionRead(aExceptionByteList, j) == STAT_OK)
	{
		for(i=0;i<32;i++) if (memcmp(aExceptionByteList+i*8, Card, 8) == 0) return STAT_OK;
		j++;
	}
	return(STAT_NOK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_InitRevocationList
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*
byte Main_InitRevocationList(void)
{
	byte aRevocationByteList[REVOCATIONBYTELIST];

	if (Files_InitFile(Revocation_File,sizeof(aRevocationByteList)) != STAT_OK) return(STAT_NOK);
	memset(aRevocationByteList, 0, sizeof(aRevocationByteList));
	memcpy(aRevocationByteList,"\xA0\x00\x00\x00\x04\xF8\x00\x10\x00",9);
	if (Files_RevocationUpdate(aRevocationByteList) != STAT_OK) return(STAT_NOK);
	return(STAT_OK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_SearchRevocationList
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Main_SearchRevocationList(EMVRevocEntry *toSearch)
{
	byte aRevocationByteList[REVOCATIONBYTELIST];
	int i, j;
	
	j = 1;
	while (Files_RevocationRead(aRevocationByteList,j) == STAT_OK)
	{
		for(i=0;i<28;i++) if (memcmp(aRevocationByteList+i*9, (byte *)toSearch, 9) == 0) return STAT_OK;
		j++;
	}
	return(STAT_NOK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_LoadCAPK
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
/* Toygar - New EMV Start */
#if 0
byte Main_LoadCAPK(void) 
{
	RowData  rdRID;
	RowData  rdPKI;
	RowData  rdCAPK;
	RowData  rdExp;
	int i;

	for(i=0;i<MAXCAPK;i++)
	{
/* Toygar - NextEMV Start - Ok */
		rdRID = BerTlv_AsBufToRd(Keys[i].rid, sizeof(Keys[i].rid));
		rdPKI = BerTlv_AsBufToRd(&Keys[i].index, sizeof(Keys[i].index));
		rdCAPK = BerTlv_AsBufToRd(Keys[i].key, Keys[i].keylength);
		rdExp = BerTlv_AsBufToRd(Keys[i].exp, sizeof(Keys[i].exp));
/*
		BerTlv_AssignBufferToRd(&rdRID, Keys[i].rid, sizeof(Keys[i].rid));
		BerTlv_AssignBufferToRd(&rdPKI, &Keys[i].index, sizeof(Keys[i].index));
		BerTlv_AssignBufferToRd(&rdCAPK, Keys[i].key, Keys[i].keylength);
		BerTlv_AssignBufferToRd(&rdExp, Keys[i].exp, sizeof(Keys[i].exp));
*/
/* Toygar - NextEMV End - Ok */
		BerTlv_TrimRD(&rdExp, 0x00);
/* Toygar - New EMV Start */
/*
		EmvDb_SaveCAPublicKey(rdRID, rdPKI, rdCAPK, rdExp);				
*/
/* Toygar - New EMV End */
	}
	return(STAT_OK);	
}
#endif
/* Toygar - New EMV End */


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_AssignCAPKs
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
/* Toygar - New EMV Start */
/*
void Main_AssignCAPKs(byte *buffer, usint buffersize)
{
	usint runner = 0, i = 0;

	while(runner + sizeof(Keys[i]) <= buffersize)
	{
		memcpy(&Keys[i], &buffer[runner], sizeof(Keys[i]));
		i++;
		runner += sizeof(Keys[i]);
	}
}
*/
/* Toygar - New EMV End */

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_TerminalConfigurationMenu
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
/*
byte Main_TerminalConfigurationMenu(void)
{
	byte entry_no;
	menu csMENU_menu;
	entry csMENU_entry []=
				{
					{"Config #1    ", 0, 0,  0},
					{"Config #2    ", 0, 0,  0},
					{"Config #3    ", 0, 0,  0},
					{"Config #4    ", 0, 0,  0},
					{"Config #5    ", 0, 0,  0},
					{"Config #6    ", 0, 0,  0},
					{"Config #7    ", 0, 0,  0},
					{"Config #8    ", 0, 0,  0},
					{"Load Config. ", 0, 0,  0}
				};
	csMENU_menu.header		= "CONFIGURATION";
	csMENU_menu.no_of_entries	= 9;
	csMENU_menu.menu_entries	= &csMENU_entry[0];
	csMENU_menu.mode_flags		= DEFAULT_MODE|MENU_TIMEOUT_30_SEC;
	entry_no = Formater_GoMenu ((const)&csMENU_menu,CHOICE_MODE);
	EmvIF_ClearDsp(DSP_MERCHANT);
	return(entry_no);
}
*/
		

/******************************************************************************
******************************* Arithmetic Operators Start ********************
******************************************************************************/
#if 0
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_AddBcd1
 *
 * DESCRIPTION:
 *
 * RETURN: none
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Mainloop_AddBcd1(char *X, int Lx, char *Y, int Ly, char *R)
{
	int i;          /* operand 1 index */
	int j;          /* operand 2 index */
	byte Carry;     /* carry */
	char *A;        /* operand 1 pointer */
	char *B;        /* operand 2 pointer */

	/* if length of operand 1 >= length of operand 2 then */
	if (Lx >= Ly)
	{
		/* the operation will be R = X + Y */
		/* initialise operand 1 pointer with operand 1 address */
		A = X;
		/* initialise operand 1 index with operand 1 length - 1 */
		i = Lx-1;
		/* initialise operand 2 pointer with operand 2 address */
		B = Y;
		/* initialise operand 2 index with operand 2 length - 1 */
		j = Ly-1;
	}
	/* if length of operand 2 > length of operand 1 then */
	else
	{
		/* the operation will be R = Y + X */
		/* initialise operand 1 pointer with operand 2 address */
		A = Y;
		/* initialise operand 1 index with operand 2 length - 1 */
		i = Ly-1;
		/* initialise operand 2 pointer with operand 1 address */
		B = X;
		/* initialise operand 2 index with operand 1 length - 1 */
		j = Lx-1;
	}

	/* for the j last digit of the two operands */
	for (Carry = 0 ; j >= 0 ; i-- , j--)
	{
		/* result digit = operand 1 digit + operand 2 digit + carry */
		R[i+1] = A[i] + B[j] + Carry;
		/* carry = 0 */
		Carry = 0;
		/* if result digit >= 10 then */
		if (R[i+1] >= 10)
		{
			/* result digit = result digit - 10 */
			R[i+1] -= 10;
			/* carry = 1 */
			Carry = 1;
		}
	}

	/* for the i-j remaining digits of the longer operand */
	for (; i >= 0 ; i--)
	{
		/* result digit = longer operand digit + carry */
		R[i+1] = A[i] + Carry;
		/* carry = 0 */
		Carry = 0;
		/* if result digit >= 10 then */
		if (R[i+1] >= 10)
		{
			/* result digit = result digit - 10 */
			R[i+1] -= 10;
			/* carry = 1 */
			Carry = 1;
		}
	}
	/* higher result digit = carry */
	R[0] = Carry;
}





/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_SubstBcd1
 *
 * DESCRIPTION: 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Mainloop_SubstBcd1(char *X, int Lx, char *Y, int Ly, char *R)
{
	byte Carry;
	int i;          /* index for X operand */
	int j;          /* index for Y operand */

	/* for the Ly last digit of each operand */
	for (Carry = 0 , i = Lx-1 , j = Ly-1 ; j >= 0 ; i-- , j--)
	{
		/* if operand 1 digit >= operand 2 digit + carry then */
		if (X[i] >= (Y[j] + Carry))
		{
			/* result digit = operand 1 digit - (operand 2 digit + carry) */
			R[i] = X[i] - (Y[j] + Carry);
			/* carry = 0 */
			Carry = 0;
		}
		/* if operand 1 digit < operand 2 digit + carry then */
		else
		{
			/* result digit = (operand 1 digit+10) - (operand 2 digit+carry) */
			R[i] = (X[i] + 10) - (Y[j] + Carry);
			/* carry = 1 */
			Carry = 1;
		}
	}

	/* for the Lx-Ly digit of the first operand */
	for (; i >= 0 ; i--)
	{
		/* if operand 1 digit >= carry then */
		if (X[i] >= Carry)
		{
			/* result digit = operand 1 digit - carry */
			R[i] = X[i] - Carry;
			/* carry = 0 */
			Carry = 0;
		}
		/* if operand 1 digit < carry then */
		else
		{
			/* result digit = (operand 1 digit + 10) - carry */
			R[i] = (X[i] + 10) - Carry;
			/* carry = 1 */
			Carry = 1;
		}
	}

}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_MulBcd1
 *
 * DESCRIPTION: 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Mainloop_MulBcd1(char *X, int Lx, char *Y, int Ly, char *R)
{
	int i;                  /* first operand index */
	int j;                  /* second operand index */
	byte Carry;             /* carry */

	/* initialisation of the result with 0 */
	for (i = 0 ; i < (Lx+Ly) ; R[i] = 0 , i++);

	/* for each operand 2 digit */
	for (i = Ly-1 ; i >= 0 ; i--)
	{
		/* for each operand 1 digit */
		for (Carry = 0 , j = Lx-1 ; j >= 0 ; j--)
		{
			/* result digit = (op 2 digit * op 1 digit) + carry + result digit */
			R[i+j+1] += (Y[i] * X[j]) + Carry;
			/* carry = 0 */
			Carry = 0;
			/* while result digit >= 10 */
			while (R[i+j+1] >= 10)
			{
				/* result digit = result digit - 10 */
				R[i+j+1] -= 10;
				/* carry = carry + 1 */
				Carry++;
			}
		}
		R[i] = Carry;
	}

}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_SimpleDivBcd1
 *
 * DESCRIPTION: 
 *
 * RETURN: Q     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Mainloop_SimpleDivBcd1(char *X, int Lx, char *Y, int Ly, char *R)
{
	char Res[50];   /* process buffer pointer */
	byte Q;         /* quotient */
	int i;          /* buffer index for transfers */

	Q = 0;

	/* copy the dividand in the remaining and in the process buffer */
	for (i = 0 ; i < Lx ; Res[i] = R[i] = X[i] , i++);

	/* while dividand >= divisor */
	while (Mainloop_StrCmp(Res, Lx, Y, Ly) >= 0)
	{
		/* remainig = dividand - divisor */
		Mainloop_SubstBcd1(Res, Lx, Y, Ly, R);
		/* quotient = quotient + 1 */
		Q++;
		/* dividand = remaining */
		for (i = 0 ; i < Lx ; Res[i] = R[i] , i++);
	}
  return (Q);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_DivBcd1
 *
 * DESCRIPTION: 
 *
 * RETURN: STAT_OK     
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
byte Mainloop_DivBcd1(char *X, int Lx, char *Y, int Ly, char *Q)
{
	int i;          /* buffer index for transfer */
	int Ld;         /* dividand length */
	char R[50];     /* remaining buffer address */

	/* initialise the quotient with 0 */
	for (i = 0 ; i < Lx ; Q[i] = 0 , i++);

	/* suppression of the unsignificant digit of the dividand */
	while (!(*X) && (Lx))
	{
		X++;
		Lx--;
		Q++;
	}

	/* suppression of the unsignificant digit of the divisor */
	while (!(*Y) && (Ly))
	{
		Y++;
		Ly--;
	}

	/* if the divisor = 0 then */
	if (!Ly)
		/* return error division by zero */
		return (STAT_NOK);

	/* the quotient has Ly-1 left zero */
	Q += Ly-1;

	/* process length of the dividand = length of the divisor */
	Ld = Ly;

	/* while dividand >= divisor */
	while (Mainloop_StrCmp(X, Lx, Y, Ly) >= 0)
	{
		/* quotient digit = simple division of X by Y */
		*Q = Mainloop_SimpleDivBcd1(X, Ld, Y, Ly, R);
		/* transfer the remaining of the simple division in the dividand */
		for (i = 0 ; i < Ld ; X[i] = R[i] , i++);
		/* increase length of the divisor and quotient pointer */
		Ld++;
		Q++;
	}
	return (STAT_OK);
}                                                                                            




/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ConvAscBcd1
 *
 * DESCRIPTION: 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Mainloop_ConvAscBcd1(char *fpAsc, char *fpBcd1, int fpLength)
{
	int lbCounter;

	for (lbCounter=0;lbCounter<fpLength;lbCounter++)
		fpBcd1[lbCounter]=fpAsc[lbCounter]-0x30;
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ConvBcd1Asc
 *
 * DESCRIPTION: 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Mainloop_ConvBcd1Asc(char *fpBcd1, char *fpAsc, int fpLength)
{
	int lbCounter;

	for (lbCounter=0;lbCounter<fpLength;lbCounter++)
		fpAsc[lbCounter]=fpBcd1[lbCounter]+0x30;
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_AscLen
 *
 * DESCRIPTION:
 *
 * RETURN: lbCounter
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
int Mainloop_AscLen(char *fpAsc, int fwLength)
{
	int lbCounter;
	for (lbCounter=0;lbCounter<fwLength;lbCounter++)
		if (fpAsc[lbCounter]<'0' || fpAsc[lbCounter]>'9')
			break;
	return(lbCounter);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_AddAscii
 *
 * DESCRIPTION:
 *
 * RETURN: none      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Mainloop_AddAscii(char *fpAsc1,int fwLen1,char *fpAsc2,int fwLen2,char *fpResult, int fwLenRet)
{
	int lwLen1;             /* Length of first number */
	int lwLen2;             /* Length of second number */
	char lpResultBcd1[20];  /* Result in BCD1 */
	char lpResultAsc[20];   /* Result in Ascii */
	char lpBcd11[20];               /* First number in BCD1 */
	char lpBcd12[20];       /* Second number in BCD1 */
	int lwLenRet;                   /* Length of result */
	int lwCounter;          /* Used for loops */

	/* Convert numbers to BCD1 */
	lwLen1=Mainloop_AscLen(fpAsc1,fwLen1);
	lwLen2=Mainloop_AscLen(fpAsc2,fwLen2);
	Mainloop_ConvAscBcd1(fpAsc1,lpBcd11,lwLen1);
	Mainloop_ConvAscBcd1(fpAsc2,lpBcd12,lwLen2);
	/* Add numbers */
	Mainloop_AddBcd1(lpBcd11, lwLen1, lpBcd12, lwLen2, lpResultBcd1);
	/* Calculate the length of the result */
	if (lwLen1>lwLen2) lwLenRet=lwLen1+1;
	else lwLenRet=lwLen2+1;
	/* Convert result to ascii*/
	Mainloop_ConvBcd1Asc(lpResultBcd1, lpResultAsc, lwLenRet);
	lpResultAsc[lwLenRet]=0;
	/* Clear insignificant 0s and right pad result with blanks */
	Mainloop_ClearInsZeros(lpResultAsc);
	lwLenRet=strlen(lpResultAsc);
	for (lwCounter=lwLenRet;lwCounter<fwLenRet;lwCounter++)
		lpResultAsc[lwCounter]=' ';
	memcpy(fpResult,lpResultAsc,fwLenRet);
	return;
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_SubstAscii
 *
 * DESCRIPTION: 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Mainloop_SubstAscii(char *fpAsc1,int fwLen1,char *fpAsc2,int fwLen2,char *fpResult, 
				int fwLenRet, boolean *fpNegSign)
{
	char lpResultBcd1[20];  /* Result in BCD1 */
	char lpResultAsc[20];   /* Result in Ascii */
	char lpBcd11[20];               /* First number in BCD1 */
	char lpBcd12[20];       /* Second number in BCD1 */
	int lwLenRet;                   /* Length of result */
	int lwCounter;          /* Used for loops */
	int lwLen1;             /* Length of first number */
	int lwLen2;             /* Length of second number */
	int Status;

	/* Convert numbers to BCD1 */
	lwLen1=Mainloop_AscLen(fpAsc1,fwLen1);
	lwLen2=Mainloop_AscLen(fpAsc2,fwLen2);
	Mainloop_ConvAscBcd1(fpAsc1,lpBcd11,lwLen1);
	Mainloop_ConvAscBcd1(fpAsc2,lpBcd12,lwLen2);
	/* Compare numbers */
	Status = Mainloop_StrCmp(lpBcd11,lwLen1,lpBcd12,lwLen2);
	/* Get sign and substract numbers */
	switch (Status)
	{
		case 0 :                        /* Acs1=Asc2 */
			memset(lpResultBcd1,0,20);
			*fpNegSign = FALSE;
			lwLenRet=lwLen1;
			break;

		case 1 :                        /* Asc1>Asc2 */
			Mainloop_SubstBcd1(lpBcd11, lwLen1, lpBcd12, lwLen2, lpResultBcd1);
			*fpNegSign = FALSE;
			lwLenRet=lwLen1;
			break;

		case -1:                        /* Asc1<Asc2 */
			Mainloop_SubstBcd1(lpBcd12, lwLen2, lpBcd11, lwLen1, lpResultBcd1);
			*fpNegSign = TRUE;              /* Negative sign */
			lwLenRet=lwLen2;
			break;
	}

	/* Convert result to ascii */
	Mainloop_ConvBcd1Asc(lpResultBcd1, lpResultAsc, lwLenRet);
	lpResultAsc[lwLenRet]=0;
	/* Clear insignificant 0s and right pad result with blanks */
	Mainloop_ClearInsZeros(lpResultAsc);
	lwLenRet=strlen(lpResultAsc);
	for (lwCounter=lwLenRet;lwCounter<fwLenRet;lwCounter++)
		lpResultAsc[lwCounter]=' ';
	memcpy(fpResult,lpResultAsc,fwLenRet);
	return;
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_AsciiCmp
 *
 * DESCRIPTION: 
 *
 * RETURN: Status     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
int Mainloop_AsciiCmp(char *fpAsc1, int fwLen1,char *fpAsc2, int fwLen2)
{
	char lpBcd11[20];   /* First number in BCD1 */
	char lpBcd12[20];   /* Second number in BCD1 */
	int lwLen1;         /* Length of first number */
	int lwLen2;         /* Length of second number */
	int     Status;                 /* Return status */

	/* Convert numbers to BCD1 */
	lwLen1=Mainloop_AscLen(fpAsc1,fwLen1);
	lwLen2=Mainloop_AscLen(fpAsc2,fwLen2);
	Mainloop_ConvAscBcd1(fpAsc1,lpBcd11,lwLen1);
	Mainloop_ConvAscBcd1(fpAsc2,lpBcd12,lwLen2);
	/* Compare numbers */
	Status = Mainloop_StrCmp(lpBcd11,lwLen1,lpBcd12,lwLen2);
	return(Status);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_StrCmp
 *
 * DESCRIPTION: 
 *
 * RETURN:0
 *
 * NOTES:       
 *
 * -------------------------------------------------------------------------- */
int Mainloop_StrCmp(char *X, int Lx, char *Y, int Ly)
{
	int i;  /* first buffer index */
	int j;  /* second buffer index */

	i = j = 0;
	/* if first buffer is longer than second buffer then */
	if (Lx > Ly)
	{
		/* suppress the unsignificant zero in the first buffer */
		for (i = 0 ; (i < (Lx-Ly)) && (X[i] == 0) ; i++);
		/* if first buffer is still longer then */
		if (i != Lx-Ly)
			/* first > second */
			return (1);
	}

	/* if second buffer is longer than first buffer then */
	if (Lx < Ly)
	{
		/* suppress the unsignificant zero in the second buffer */
		for (j = 0 ; (j < (Ly-Lx)) && (Y[j] == 0) ; j++);
		/* if second buffer is still longer then */
		if (j != Ly-Lx)
			/* second > first */
			return (-1);
	}

	/* here the two buffers have the same significant size
	   for each character until the end of the buffers */
	for (; i < Lx ; i++ , j++)
	{
		/* if char of the first buffer > char of the second buffer then */
		if (X[i] > Y[j])
			/* first > second */
			return (1);
		/* else if char of the second buffer > char of the first buffer then */
		else if (X[i] < Y[j])
			/* second > first */
			return (-1);
	}
	/* first = second if we run until here */
	return (0);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ClearInsZeros
 *
 * DESCRIPTION: 
 *
 * RETURN:none
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ClearInsZeros(char *StrToCleared)
{
	char ClearedStr[256];
	byte Len1;
	byte Len2;
	int i;

	memset(ClearedStr,0,256);
	Len1 = strlen(StrToCleared);
	for(i=0;i<Len1;i++)
		if (StrToCleared[i] != '0') break;
	memcpy(ClearedStr,StrToCleared+i,Len1-i);
	Len2 = strlen(ClearedStr);
	memcpy(StrToCleared,ClearedStr,Len2);
	StrToCleared[Len2] = 0;
}
#endif

/******************************************************************************
******************************* Arithmetic Operators End **********************
******************************************************************************/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_WithComma
 *
 * DESCRIPTION: Puts a comma after every 3 digits when displaying amount. 
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_WithComma(char *fpInStr, char *fpOutStr, int fcLength)
{
	sint liOutLen;
	sint liInLen;
	sint liCounter;
	sint liOutMax;

	liOutMax=fcLength+(fcLength-1)/3;     /* length of output string */
	for (liCounter=0;liCounter<(fcLength-1)/3;liCounter++)
	{
		fpOutStr[liOutMax-liCounter*4-1]=fpInStr[fcLength-liCounter*3-1];
		fpOutStr[liOutMax-liCounter*4-2]=fpInStr[fcLength-liCounter*3-2];
		fpOutStr[liOutMax-liCounter*4-3]=fpInStr[fcLength-liCounter*3-3];
		fpOutStr[liOutMax-liCounter*4-4]=',';
	}
	liInLen=fcLength-liCounter*3-1;
	liOutLen=liOutMax-liCounter*4-1;
	for (liCounter=liInLen;liCounter>=0;liCounter--)
		fpOutStr[liOutLen--]=fpInStr[liInLen--];
	if (liOutMax==0)          /* For empty amount put 0 */
		fpOutStr[liOutMax++] = '0';
	fpOutStr[liOutMax]=0;
}






/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_SetDateTimeFormatted
 *
 * DESCRIPTION: Formats date and time.
 *
 * RETURN: none     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_SetDateTimeFormatted(char *DateTime)
{
    real_time_clock Today;

/*	DebugPrintOnPaper(DateTime);*/
	Today.day =     (DateTime[0] -'0')*16 + DateTime[1] -'0';
	Today.month =   (DateTime[2] -'0')*16 + DateTime[3] -'0';
	Today.year =    (DateTime[6] -'0')*16 + DateTime[7] -'0';
	Today.hours =   (DateTime[8] -'0')*16 + DateTime[9] -'0';
	Today.minutes = (DateTime[10]-'0')*16 + DateTime[11]-'0';
	Today.seconds = 0;
	Rtc_SetTime(&Today); 
	Rtc_SetDate(&Today);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GetDateTimeFormatted
 *
 * DESCRIPTION: Inputs date and time.In YYMMDD/HHMM format.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_GetDateTimeFormatted(char *DateTime, boolean IdlePrompt)
{
	char text[19];
    real_time_clock Today;
	int count;

	memset(text,' ',19);
	Rtc_Read(&Today);
	count = 0;
	    text[count++]=(Today.day>>4)+'0';
	    text[count++]=(Today.day&0x0f)+'0';
	    if (IdlePrompt) text[count++]='/';
	    text[count++]=(Today.month>>4)+'0';
	    text[count++]=(Today.month&0x0f)+'0';
	    if (IdlePrompt) text[count++]='/';
	    text[count++]=(Today.year>>4)+'0';
	    text[count++]=(Today.year&0x0f)+'0';
	    if (IdlePrompt) count += 2;
	    text[count++]=(Today.hours>>4)+'0';
	    text[count++]=(Today.hours&0x0f)+'0';
	    if (IdlePrompt) text[count++]=':';
	    text[count++]=(Today.minutes>>4)+'0';
	    text[count++]=(Today.minutes&0x0f)+'0';
	    if (IdlePrompt) text[count++]=':';
	    text[count++]=(Today.seconds>>4)+'0';
	    text[count++]=(Today.seconds&0x0f)+'0';
	text[count]=0;

	    strcpy(DateTime,text);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GetDateTimeIdle
 *
 * DESCRIPTION: 
 *
 * RETURN:none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_GetDateTimeIdle(char *TimeDate, boolean TimeFl)
{
	char text[20];
    real_time_clock Today;
	int count;

	memset(text,' ',20);
	Rtc_Read(&Today);
	
	count = 0;
    text[count++]=(Today.hours>>4)+'0';
	text[count++]=(Today.hours&0x0f)+'0';
    if (TimeFl)
	 text[count++]=':';
    else
     count += 1;
	text[count++]=(Today.minutes>>4)+'0';
	text[count++]=(Today.minutes&0x0f)+'0';
	count += 3;
	text[count++]=(Today.day>>4)+'0';
	text[count++]=(Today.day&0x0f)+'0';
	text[count++]='/';
	text[count++]=(Today.month>>4)+'0';
	text[count++]=(Today.month&0x0f)+'0';
	text[count++]='/';
	text[count++]=(Today.year>>4)+'0';
	text[count++]=(Today.year&0x0f)+'0';
    text[count]=0;

	strcpy(TimeDate,text);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Mainloop_IdleLogo
 *
 * DESCRIPTION          : Displays te logo in the idle prompt.
 *
 * RETURN                       : none
 *
 * NOTES                        : Lipman and r.i.o 19/01/01
 *                                        type_of_medi  - > P for Printer logo
 *                                                                      - > D for Display logo
 *
 * --------------------------------------------------------------------------*/
void Mainloop_IdleLogo(void)
{

	const byte aklogoDisp [] = { 
	 0x00, 0x1F, 0x00, 0x78, 0x0F, 0x8F, 0xFF, 0x80, 0x0F, 0x80, 0x3C, 0x07, 0x87, 0x80, 0xF8,
	 0x00, 0x1F, 0x00, 0x78, 0x0F, 0x0F, 0xFF, 0xC0, 0x0F, 0x80, 0x3E, 0x07, 0x87, 0x80, 0xF0,
	 0x00, 0x3F, 0x80, 0x78, 0x1F, 0x0F, 0xFF, 0xE0, 0x1F, 0xC0, 0x3E, 0x07, 0x87, 0x81, 0xF0,
	 0x00, 0x3F, 0x80, 0x78, 0x3E, 0x0F, 0xFF, 0xF0, 0x1F, 0xC0, 0x3F, 0x07, 0x87, 0x83, 0xE0,
	 0x00, 0x3F, 0x80, 0x78, 0x7C, 0x0F, 0x01, 0xF0, 0x1F, 0xC0, 0x3F, 0x07, 0x87, 0x87, 0xC0,
	 0x00, 0x7B, 0xC0, 0x78, 0xF8, 0x0F, 0x00, 0xF0, 0x3D, 0xE0, 0x3F, 0x87, 0x87, 0x8F, 0x80,
	 0x00, 0x7B, 0xC0, 0x78, 0xF0, 0x0F, 0x00, 0xF0, 0x3D, 0xE0, 0x3F, 0x87, 0x87, 0x8F, 0x00,
	 0x00, 0x7B, 0xC0, 0x79, 0xF0, 0x0F, 0x00, 0xF0, 0x3D, 0xE0, 0x3F, 0x87, 0x87, 0x9F, 0x00,
	 0x00, 0xFB, 0xE0, 0x7B, 0xE0, 0x0F, 0x01, 0xE0, 0x7D, 0xF0, 0x3F, 0xC7, 0x87, 0xBE, 0x00,
	 0x00, 0xF1, 0xE0, 0x7F, 0xE0, 0x0F, 0xFF, 0xE0, 0x78, 0xF0, 0x3D, 0xC7, 0x87, 0xFE, 0x00,
	 0x00, 0xF1, 0xE0, 0x7F, 0xF0, 0x0F, 0xFF, 0xC0, 0x78, 0xF0, 0x3D, 0xE7, 0x87, 0xFF, 0x00,
	 0x01, 0xF1, 0xE0, 0x7F, 0xF0, 0x0F, 0xFF, 0xC0, 0xF8, 0xF0, 0x3C, 0xE7, 0x87, 0xFF, 0x00,
	 0x01, 0xE0, 0xF0, 0x7F, 0xF0, 0x0F, 0xFF, 0xE0, 0xF0, 0x78, 0x3C, 0xF7, 0x87, 0xFF, 0x00,
	 0x01, 0xE0, 0xF0, 0x7E, 0x78, 0x0F, 0x01, 0xF0, 0xF0, 0x78, 0x3C, 0xF7, 0x87, 0xE7, 0x80,
	 0x01, 0xFF, 0xF0, 0x7C, 0x78, 0x0F, 0x00, 0xF0, 0xFF, 0xF8, 0x3C, 0x7F, 0x87, 0xC7, 0x80,
	 0x03, 0xFF, 0xF8, 0x78, 0x3C, 0x0F, 0x00, 0xF1, 0xFF, 0xFC, 0x3C, 0x7F, 0x87, 0x83, 0xC0,
	 0x03, 0xFF, 0xF8, 0x78, 0x3C, 0x0F, 0x00, 0xF1, 0xFF, 0xFC, 0x3C, 0x3F, 0x87, 0x83, 0xC0,
	 0x03, 0xFF, 0xF8, 0x78, 0x1E, 0x0F, 0x00, 0xF1, 0xFF, 0xFC, 0x3C, 0x3F, 0x87, 0x81, 0xE0,
	 0x07, 0xC0, 0x3C, 0x78, 0x1E, 0x0F, 0x01, 0xF3, 0xE0, 0x1E, 0x3C, 0x1F, 0x87, 0x81, 0xE0,
	 0x07, 0x80, 0x3C, 0x78, 0x1F, 0x0F, 0xFF, 0xE3, 0xC0, 0x1E, 0x3C, 0x1F, 0x87, 0x81, 0xF0,
	 0x07, 0x80, 0x3C, 0x78, 0x0F, 0x0F, 0xFF, 0xE3, 0xC0, 0x1E, 0x3C, 0x0F, 0x87, 0x80, 0xF0,
	 0x0F, 0x80, 0x3E, 0x78, 0x0F, 0x8F, 0xFF, 0xC7, 0xC0, 0x1F, 0x3C, 0x0F, 0x87, 0x80, 0xF8,
	 0x0F, 0x00, 0x1E, 0x78, 0x07, 0x8F, 0xFF, 0x87, 0x80, 0x0F, 0x3C, 0x07, 0x87, 0x80, 0x78};
		
/*const byte OemlogoDisp [] = { 
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0x80, 0x00, 0xFF, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xF0,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xE0, 0x03, 0xFF, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xE0, 0x03, 0xFF, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xF0,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xF8, 0x01, 0xFF, 0xC0, 0x0F, 0xFF, 0xFC, 0x0F, 0xFF, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xF8, 0x01, 0xFF, 0xC0, 0x0F, 0xFF, 0xFC, 0x0F, 0xFF, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xF8, 0x01, 0xFF, 0xC0, 0x0F, 0xFF, 0xFC, 0x0F, 0xFF, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xF8, 0x01, 0xFF, 0xC0, 0x0F, 0xFF, 0xFE, 0x1F, 0xFF, 0xFC, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x80, 0x0F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF, 0xFE, 0x00, 0x0F, 0xFF, 0xFF, 0x3F, 0xFF, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF, 0xF0, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF, 0xF0, 0x00, 0x0F, 0xFE, 0xFF, 0xFF, 0xBF, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x0F, 0xFE, 0xFF, 0xFF, 0xBF, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF, 0xFE, 0x00, 0x0F, 0xFE, 0x7F, 0xFF, 0x3F, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x80, 0x0F, 0xFE, 0x7F, 0xFF, 0x3F, 0xFC, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xF8, 0x01, 0xFF, 0xC0, 0x0F, 0xFE, 0x3F, 0xFE, 0x3F, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xF8, 0x01, 0xFF, 0xC0, 0x0F, 0xFE, 0x3F, 0xFE, 0x3F, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xF8, 0x01, 0xFF, 0xC0, 0x0F, 0xFE, 0x1F, 0xFC, 0x3F, 0xFC, 0x00,
 0x01, 0xFF, 0xE0, 0x00, 0x1F, 0xF8, 0x01, 0xFF, 0xC0, 0x0F, 0xFE, 0x1F, 0xFC, 0x3F, 0xFC, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0xFE, 0x0F, 0xFC, 0x3F, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0xFE, 0x07, 0xF8, 0x3F, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFE, 0x07, 0xF8, 0x3F, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFE, 0x03, 0xF0, 0x3F, 0xFF, 0xF1,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xFF, 0xFE, 0x01, 0xE0, 0x3F, 0xFF, 0xF2,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xFF, 0xFE, 0x01, 0xE0, 0x3F, 0xFF, 0xF1,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFE, 0x00, 0xC0, 0x3F, 0xFF, 0xF0,
 0x7F, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xE0, 0x07, 0xFF, 0xFE, 0x00, 0xC0, 0x3F, 0xFF, 0xF0,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};*/

/*		GrphText_ClrArea (1, 0, 3, 127, FALSE);*/
		/*GrphText_ShowImage ((byte *)&OemlogoDisp [0], 128, 50, 0, 0, FALSE);*/

	/*	GrphText_ShowImage ((byte *)&aklogoDisp [0], 120, 23, 3, 4, FALSE);
		GrphText_ShowImage ((byte *)&gsmlogoDisp[0] ,  40, 9, 26,75, FALSE);
		GrphText_DrawDisp ();*/

		GrphText_ShowImage ((byte *)&gsmlogoDisp[0] ,  40, 9, 35,75, FALSE);
		GrphText_ShowImage ((byte *)&aklogoDisp [0], 120, 23, 48, 4, FALSE);
		
		GrphText_DrawDisp ();
}

void Mainloop_ShowOperator(void)
{
	SystemStatistics1 pwrstats;
	OperatorInfo STOperatorInfo;
	sint i,j;

	memset(&STOperatorInfo,0,sizeof(OperatorInfo));
	if(GSMRadio_GetCurrentOperator(&STOperatorInfo)==GSMRADIO_OK)
	{
		if(!memcmp(STOperatorInfo.OperLongName,"CC 286 NC 03",12)){
		strcpy(STOperatorInfo.OperLongName, "ARIA");
			STOperatorInfo.OperLongName[4]=0;
		}
		
		i=strlen(STOperatorInfo.OperLongName);
		if(i>16) i=16;
		j = (16-i)/2;

		GrphText_Write ((byte) 1, (sint) j*8, (char *) STOperatorInfo.OperLongName,
		    (byte) FONT_13X7, (byte) strlen(STOperatorInfo.OperLongName), FALSE, FALSE);
		//Mainloop_GPRSStat(FALSE);
		if(GPRS_USEGPRS) GrphText_Write ((byte) 8, (sint) 48, (char *) "GPRS",
			(byte) FONT_13X7, (byte)4, FALSE, FALSE);
		
			//if power on. then connect to gprs
//		Statistics_System(&pwrstats,sizeof(pwrstats),FALSE); /*read*/        
//		if (pwrstats.PwrUpCnt) {
//			Printer_WriteStr("Power Up: \n");
//			Statistics_System(&pwrstats,sizeof(pwrstats),TRUE); /*reset*/        		
			if(GPRS_USEGPRS && lsGPRS_Connect_Param.init) {
				lsGPRS_Connect_Param.init= FALSE;
				if(GPRS_Init(TRUE)==STAT_OK){
					lsGPRS_Connect_Param.Connected= TRUE;
				}else {
					lsGPRS_Connect_Param.Connected= FALSE;
					lsGPRS_Connect_Param.tcpavailable= FALSE;
					lsGPRS_Connect_Param.sd = -1;
				}
			}
		
//		}

		
		
			
	}
	else	{
		GrphText_ClrLine (1, FALSE);
		GrphText_ClrLine (8, FALSE);
		lsGPRS_Connect_Param.Connected= FALSE;
		lsGPRS_Connect_Param.tcpavailable= FALSE;
		lsGPRS_Connect_Param.sd = -1;
		lsGPRS_Connect_Param.init= TRUE;
		
		if(GPRS_USEGPRS) {
			GPRS_Disconnect(APN);
		}
		
	}
//	else	GrphText_ClrLine (1, TRUE);
	/*
	if(GSMRadio_GetCurrentOperator(&STOperatorInfo)==GSMRADIO_OK)
		Printer_WriteStr("STOperatorInfoOK : \n");
	else
		Printer_WriteStr("STOperatorInfoNOK : \n");
PrntUtil_BufAsciiHex((byte *)&STOperatorInfo ,sizeof(STOperatorInfo));
*/
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_IDLEUpdate
 *
 * DESCRIPTION:Updates the display every minute for updating time every minute.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_IDLEUpdate(char *OldTime)
{
	char  DateTime[20];
	char  TimeDate[20];
	sint  tt;
	char temp_date[17];

	Mainloop_GetDateTimeFormatted (DateTime, TRUE);
	tt = atoi(&DateTime [17]);
	if (tt%2) Mainloop_GetDateTimeIdle (TimeDate, TRUE);
	else Mainloop_GetDateTimeIdle (TimeDate, FALSE);
	if (memcmp(DateTime,OldTime,18) )
	{
		memcpy(temp_date, TimeDate, 6);
		memcpy(temp_date+6, &TimeDate[8], 6);
		if (TimeDate[14] != '9') 
		{
			temp_date[6+6] = '2';
			temp_date[6+6+1] = '0';
		}
		else
		{
			temp_date[6+6] = '1';
			temp_date[6+6+1] = '9';
		}
		temp_date[14] = TimeDate[14];
		temp_date[15] = TimeDate[15];
		temp_date[16] = 0;

		/*GrphText_Format (&LineHeight_13x7[0]);*/
		Debug_SetFont(FALSE);
		GrphText_ClrLine (9 ,FALSE);
		GrphText_Write ((byte) 9, (sint) 0, (char *) temp_date,
		    (byte) FONT_13X7, (byte) strlen(temp_date), FALSE, FALSE);

		GrphText_DrawDisp ();

	}
}

	    
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_Buzzer
 *
 * DESCRIPTION: 
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
#if 0
void Mainloop_Buzzer(void *string)
{
  if (strcmp((char*)string, (char *)MESSAGE_BUZZER_OPEN))
  {
    /*Kb_ClickTimeSet(100);*/
    strcpy((char*)string, (char *)MESSAGE_BUZZER_OPEN);
  }
  else
  {
    /* Kb_ClickTimeSet(0); */
    strcpy((char*)string, (char *)MESSAGE_BUZZER_CLOSE);
  }
}
#endif


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GetAlpha
 *
 * DESCRIPTION: Performs the alpha function of characters for character
 *              input for authorisation code.
 * RETURN: The input character
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
char Mainloop_GetAlpha(char ch, boolean Up)
{
	switch(ch)
	{


	case '1'        : if (Up) return(','); else return(' ');
	case ','        : if (Up) return('.'); else return('1');
	case '.'        : if (Up) return('*'); else return(',');
	case '*'        : if (Up) return('/'); else return('.');
	case '/'        : if (Up) return('-'); else return('*');
	case '-'        : if (Up) return(' '); else return('/');
	case ' '        : if (Up) return('1'); else return('-');
	
	case '2'        : if (Up) return('A'); else return('C');
	case 'A'        : if (Up) return('B'); else return('2');
	case 'B'        : if (Up) return('C'); else return('A');
	case 'C'        : if (Up) return('2'); else return('B');

	case '3'        : if (Up) return('D'); else return('F');
	case 'D'        : if (Up) return('E'); else return('3');
	case 'E'        : if (Up) return('F'); else return('D');
	case 'F'        : if (Up) return('3'); else return('E');

	case '4'        : if (Up) return('G'); else return('I');
	case 'G'        : if (Up) return('H'); else return('4');
	case 'H'        : if (Up) return('I'); else return('G');
	case 'I'        : if (Up) return('4'); else return('H');

	case '5'        : if (Up) return('J'); else return('L');
	case 'J'        : if (Up) return('K'); else return('5');
	case 'K'        : if (Up) return('L'); else return('J');
	case 'L'        : if (Up) return('5'); else return('K');

	case '6'        : if (Up) return('M'); else return('O');
	case 'M'        : if (Up) return('N'); else return('6');
	case 'N'        : if (Up) return('O'); else return('M');
	case 'O'        : if (Up) return('6'); else return('N');

	case '7'        : if (Up) return('P'); else return('R');
	case 'P'        : if (Up) return('Q'); else return('7');
	case 'Q'        : if (Up) return('R'); else return('P');
	case 'R'        : if (Up) return('7'); else return('Q');

	case '8'        : if (Up) return('S'); else return('U');
	case 'S'        : if (Up) return('T'); else return('8');
	case 'T'        : if (Up) return('U'); else return('S');
	case 'U'        : if (Up) return('8'); else return('T');

	case '9'        : if (Up) return('V'); else return('X');
	case 'V'        : if (Up) return('W'); else return('9');
	case 'W'        : if (Up) return('X'); else return('V');
	case 'X'        : if (Up) return('9'); else return('W');

	case '0'        : if (Up) return('Y'); else return('Z');
	case 'Y'        : if (Up) return('Z'); else return('0');
	case 'Z'        : if (Up) return('0'); else return('Y');
	default         : return(0);
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Get Input From Keyboard
 *
 * DESCRIPTION: Gets input from keyboard. 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
boolean Mainloop_GetKeyboardInput ( const char *UpperLine, char *Buffer, 
				    sint DownLimit, sint UpLimit,
				    boolean First, boolean Restricted,
				    boolean Masked, boolean Comma,
				    boolean Alpha, byte *Len, char *ExDefault,
				    boolean EmptyOk)
{
  sint i;
/*@bm 01.09.2004  boolean RestrictedFlag; */
  boolean DefaultFirst;
  char MaskedBuffer[LEN_GETKEYBOARD];
  char Default[LEN_GETKEYBOARD];
  char ret;
  sint MaskedLen;
  char ToDisplay[40];
	boolean CommaType;

  /*1_18 Ek kontroller 05/04/2000 */
  memset(Buffer,0,LEN_PAN);
  if (UpLimit > LEN_GETKEYBOARD) UpLimit = LEN_GETKEYBOARD; /* UpLimit LEN_GETKEYBOARD
															   uzunluðunu geçemez */


  memset (ToDisplay,0,40);

	EmvIF_ClearDsp(DSP_MERCHANT);

	/* 1_18 29/03/2000 initialize Default */
	memset(Default, 0 , sizeof(Default));


  /* test amacli 29/03/2000*/
	/*Printer_WriteStr ("Default :\n");
	PrntUtil_BufAsciiHex((byte *)Default,strlen(Default));

	Printer_WriteStr ("Default2 :\n");
	PrntUtil_BufAsciiHex((byte *)Default,20);

	Printer_WriteStr ("ExDefault :\n");
	PrntUtil_BufAsciiHex((byte *)ExDefault,strlen(ExDefault));

	Printer_WriteStr ("ExDefault2 :\n");
	PrntUtil_BufAsciiHex((byte *)ExDefault,10);*/

  if ((ExDefault == 0) && !Masked) strcpy(Default,"0");
  else if (ExDefault != 0) strcpy(Default,ExDefault);

  /* test amacli 29/03/2000*/
	/*Printer_WriteStr ("Default :\n");
	PrntUtil_BufAsciiHex((byte *)Default,strlen(Default));

	Printer_WriteStr ("Default2 :\n");
	PrntUtil_BufAsciiHex((byte *)Default,20);

	Printer_WriteStr ("ExDefault :\n");
	PrntUtil_BufAsciiHex((byte *)ExDefault,strlen(ExDefault));

	Printer_WriteStr ("ExDefault2 :\n");
	PrntUtil_BufAsciiHex((byte *)ExDefault,10);*/


  DefaultFirst = FALSE;
  if ((Default[0] == 0) || (!isalnum(Default[0])))
	  if (!First)
	  {
		  i = 0;
#ifndef GRAFDISP
		  /* 1_18 05/04/2000 boþluk yerine Null ile dolduruluyor */
		  /*memset(Buffer,' ',LEN_PAN);*/
		  memset(Buffer,0,LEN_PAN);
#else
		  memset(Buffer,0,LEN_PAN);
#endif
	  }
	  else
	  {
		  i = 1;
		  /* 1_18 05/04/2000 boþluk yerine Null ile dolduruluyor */
		  /*memset(Buffer+1,' ',LEN_PAN-1);*/
		    memset(Buffer+1,0,LEN_PAN-1);
	  }
  else
  {
	  DefaultFirst = TRUE;
	  strcpy(Buffer,Default);
	  i = strlen(Buffer);
  }
  RestrictedFlag = TRUE;
  do
  {
		if (strlen(UpperLine)<17)
			Debug_GrafPrint2((char *)UpperLine,2);
	    else
		{
	   memcpy(ToDisplay,UpperLine,16);
		   ToDisplay[16] = 0;
	   Debug_GrafPrint2 ((char *)ToDisplay, 1);
		   memcpy(ToDisplay,UpperLine+16,16);
		   ToDisplay[16] = 0;
	   Debug_GrafPrint2 ((char *)ToDisplay, 2);
		}

	  if (Masked)
	  {
		  memset(MaskedBuffer,0,LEN_GETKEYBOARD);
		  memcpy(MaskedBuffer,csSTARS,i);
#ifndef GRAFDISP
		  Display_DownDisplay((char *)MaskedBuffer);
#else
		  Debug_GrafPrint2 ((char *)MaskedBuffer,3);
#endif
	  }
	  else
	  {
		  if (Comma)
		  {
/* bm YTL 		Mainloop_WithComma( Buffer, MaskedBuffer, i ); */
			  	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
					CommaType = TRUE;	/*bm FALSE-YTL, TRUE-TL */
				else 
				{
				 	if (TL_Iade)
				 	  CommaType = TRUE;	/*bm FALSE-YTL, TRUE-TL */
				 	else	
					  CommaType = FALSE;	/*bm FALSE-YTL, TRUE-TL */
				}
				Utils_WithComma( Buffer, MaskedBuffer, i, CommaType );	/*bm YTL */
			  MaskedLen = strlen(MaskedBuffer);
#ifndef GRAFDISP
			  MaskedBuffer[MaskedLen++] = 'T';
			  MaskedBuffer[MaskedLen++] = 'L';
			  MaskedBuffer[MaskedLen] = 0;
			  Utils_LeftPad(MaskedBuffer,' ',16);
			  Display_DownDisplay((char *)MaskedBuffer);
#else
				/* bm YTL 25/06/2004 */
				if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
				{
			  MaskedBuffer[MaskedLen++] = 0xBE;
			  MaskedBuffer[MaskedLen] = 0;
			  Utils_LeftPad(MaskedBuffer,' ',16);
			  Debug_GrafPrint2 ((char *)MaskedBuffer,3);
				}
				else 
				{
				 	if (TL_Iade)
					{
						MaskedBuffer[MaskedLen++] = 0xBE;
						MaskedBuffer[MaskedLen] = 0;
						Utils_LeftPad( MaskedBuffer, ' ', 16 );
						Debug_GrafPrint2( ( char * ) MaskedBuffer, 3 );
					}
					else
					{
						MaskedBuffer[MaskedLen++] = ' ';
						MaskedBuffer[MaskedLen++] = 'Y';	/* bm YTL 25/06/2004 */
						MaskedBuffer[MaskedLen++] = 'T';	/* bm YTL 25/06/2004 */
						MaskedBuffer[MaskedLen++] = 'L';	/* bm YTL 25/06/2004 */
						MaskedBuffer[MaskedLen] = 0;
						Utils_LeftPad( MaskedBuffer, ' ', 16 );
						Debug_GrafPrint2( ( char * ) MaskedBuffer, 3 );						
					}
				}
#endif
		  }
#ifndef GRAFDISP
		  else if (i>16) Display_DownDisplay((char *)Buffer + (i-16)); /* for scrolling */
		  else Display_DownDisplay((char *)Buffer);
#else

		  else if (i>16) Debug_GrafPrint2((char *)Buffer + (i-16),3); /* for scrolling */
		  else  Debug_GrafPrint2((char *)Buffer,3);
#endif
	  }
	  ret = Kb_WaitForKey();
	  switch (ret)
	  {
	  case MMI_CLEAR :
		  /* 1_18 05/04/2000 boþluk yerine Null ile dolduruluyor */
		  /*memset(Buffer,' ',LEN_PAN);*/
		  memset(Buffer,0,LEN_PAN);

		  strcpy(Buffer,Default);
		  i = strlen(Buffer);
		  DefaultFirst = TRUE;
		  break;
	  case MMI_BACK_SPACE :
		  if (i > 0) i --;
		  Buffer[i] = ' ';
		  DefaultFirst = FALSE;
		  break;
	  case MMI_FORCED :
/*	  case MMI_ALPHA  :
		  if ((Alpha) && (i>0)) Buffer[i-1] = Mainloop_GetAlpha(Buffer[i-1],TRUE);
		  break;*/
	  case MMI_RETURN :
		  if ((Alpha) && (i>0)) Buffer[i-1] = Mainloop_GetAlpha(Buffer[i-1],FALSE);
		  break;
	  case MMI_POINT :
		//MK  if ((i!=0) && (Comma))
		  if (i!=0) 
		  {
			  DefaultFirst = FALSE;
					if ( i < UpLimit )
						Buffer[i++] = '0';
					if ( i < UpLimit )
						Buffer[i++] = '0';
					if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
					{			/*bm YTL 30/06/2004 */
						if ( i < UpLimit )
							Buffer[i++] = '0';
					}
					else if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
					{
						if (TL_Iade)
						{
			  if (i < UpLimit) Buffer[i++] = '0';
						}
					}
		  }
		  break;
	  default:
		  if (isdigit(ret) && !((i==0) && (ret=='0') && Comma))
		  {
			  if ((DefaultFirst) && (!First))
			  {
				  i = 0;
				  /* 1_18 05/04/2000 boþluk yerine Null ile dolduruluyor */
				  /*memset(Buffer,' ',LEN_PAN);*/
					memset(Buffer,0,LEN_PAN);
			  }
			  if (i < UpLimit)
			  {
				  DefaultFirst = FALSE;
				  Buffer[i] = ret;
				  i++;
			  }
		  }
	  }
	  if (Restricted)
	  {
		  if ((i==UpLimit) && (ret == MMI_ENTER) && ((Buffer[0] != ' ') || (EmptyOk)))
			  RestrictedFlag = FALSE;
		  else if ((ret == MMI_ESCAPE)) RestrictedFlag = FALSE;
	  }
	  else if ((ret == MMI_ESCAPE) || ((ret == MMI_ENTER) && ((Buffer[0] != ' ') || (EmptyOk))
				&& (i>=DownLimit) && (i<=UpLimit)))
		  RestrictedFlag = FALSE;
  } while(RestrictedFlag);
  *Len = i;
  return (ret == MMI_ENTER);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_CheckSwipe
 *
 * DESCRIPTION  : Makes the process as the card is swiped and track2 is saved in. 
 *
 * RETURN               : none
 *
 * NOTES                :       
 *
 * MODIFIED     ON      : 05/05/99 By: R.Ýrfan ÖZTÜRK. 
 *
 * --------------------------------------------------------------------------*/
boolean Mainloop_CheckSwipe(boolean DoMagnetic)
{
	sint i,j;
	char ch,ch1,ch2,dummy;
	char temp_card_info1[50];

	/* XLS_PAYMENT 24/07/2001 irfan */
	char temp_first_6_digit[7];

	/* 05_02 not certified card updates 16/09/2002 irfan */
	char first_6_digit_local[7];
	
	input_type = INPUT_FROM_READER;

/*
	Printer_WriteStr("input_type#1\n");
	PrntUtil_BufAsciiHex((byte *)&input_type,1);
*/

	memcpy(card_info,CardRdr_GetTrack2()+1,LEN_TRACKISO2);
	/* XLS_PAYMENT 24/07/2001 irfan */
	/* Check for Loyalty_Prefixes*/
	memcpy(temp_first_6_digit,CardRdr_GetTrack2()+1,6);
	temp_first_6_digit[6] = 0;

	/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
/*bm OPT	if ( POS_Type == '0' )		/* axess pos icin yapiliyor */
	if ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
	{
		/* 06_30 06/03/2003 offline islemler icin chip card okuyucuya zorlanma olmayacak */
		if(!OFFLINE_CARREFOURSA)
		{
		      if(!CORRUPTED_CHIP_TRANSACTION_FLAG)
		      {
			    if( strstr(Loyalty_Prefixes, temp_first_6_digit) != NULL) 
			    {
				Mainloop_DisplayWait("   CHIP KART    OKUYUCU KULLANIN",1);
				return (FALSE);
			    }
		      }
	         }
	}
	

	/* 01_18 27/06/2000 track 1 in datasý saklanýyor*/
	/** XLS 04_02 19/12/2001 irfan. initialize the varibles **/
	memset(temp_card_info1, 0 , sizeof(temp_card_info1));
	memset(card_info1, 0 , sizeof(card_info1));
	memset(card_info1_name, 0, sizeof(card_info1_name));
	memset(card_info1_surname, 0, sizeof(card_info1_surname));

	if ( TRACK1ERROR )
	{
		memcpy(temp_card_info1,CardRdr_GetTrack1()+2,LEN_TRACKISO1);
	
		for (i=0; i<LEN_TRACKISO1; i++)
		{
			ch=temp_card_info1[i];
			if(ch == '^')
			{
				memcpy(card_info1, temp_card_info1+i+1, 24);
				card_info1[24] = 0;
				break;
			}
		}

	}

	/* End Centinel characterlerini space'e set eder. Ex: ?a = '  ' gibi */
	for (i=0; i<40; i++)
	{
		ch = card_info[i];

		if(ch == '?')
		{
			card_info[i] = ' ';
			card_info[i+1] = ' ';
			break;
		}
	}


	/* 01_18 irfan 27/06/2000 track 1 datasýndan müþteri isim ve soyismi alýnýyor */
	if ( TRACK1ERROR )
	{
		for (i=0; i<24; i++)
		{
			ch = card_info1[i];

			card_info1_surname[i] = ch;
			
			if(ch == '/')
			{
				card_info1_surname[i] = 0;

				for ( j=0;j<(23-i);j++)
				{
					ch1 = card_info1[i+j+1];
					ch2 = card_info1[i+j+2];

					/* UPDATE 02_23 15/01/2001 ISO 8583 e gore '^'  charac. de araniyor */
					if ( (ch1 == '^') || ( (ch1 == ' ') &&(ch2 == ' ')) ) 
					{
						card_info1_name[j] = 0;
						break;
					}
					else
					{
						card_info1_name[j] = ch1;
					}
					
				}

				card_info1_name[j] = 0;
				break;
			}
		}
		card_info1_surname[23] = 0; /* 02_01 08/09/00 EGER SOY ISIM 24 KARAKTER ISE STACK BASIYORDU */
	}
	if (!Mainloop_CardProcess())
	{
/*		Printer_WriteStr("Mainloop_CardProcess:FALSE\n");*/
		return(FALSE);
	}

	/* Toygar EMVed */

	if (((cvv_2[0] == '2') || (cvv_2[0] == '6')) && (!CORRUPTED_CHIP_TRANSACTION_FLAG))
	{
		int trycount;

		if (BadICC)
		{
			
			if ((DoMagnetic) || (Mainloop_SwipeOrChip() == 1))
			{
				//Swipe chosen
				EmvIF_ClearDsp(DSP_MERCHANT);
				POSEntryFlag = 2;
				POSEntryFlagUsed = FALSE;
				input_type = INPUT_FROM_READER_92;
	/*
				Printer_WriteStr("input_type#2\n");
				PrntUtil_BufAsciiHex((byte *)&input_type,1);
	*/
				return(TRUE);
			}
			else{
				BadICC=0;
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("   CHIP KART    OKUYUCU KULLANIN",1);
				POSEntryFlag = -1;
				POSEntryFlagUsed = FALSE;
				input_type = INPUT_FROM_READER_91;
				return(FALSE);
			
			}
			
		}
		else
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("   CHIP KART    OKUYUCU KULLANIN",1);
			POSEntryFlag = -1;
			POSEntryFlagUsed = FALSE;
			input_type = INPUT_FROM_READER_91;
			return(FALSE);
		}
	}
	else 
	{ 
		POSEntryFlag = 1;
		POSEntryFlagUsed = FALSE;
		input_type = INPUT_FROM_READER_90;
	}
/* Toygar EMVed */
/*
	Printer_WriteStr("input_type#4\n");
	PrntUtil_BufAsciiHex((byte *)&input_type,1);
*/
	return(TRUE);
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Mainloop_SetPwrOnOffTime
 *
 * DESCRIPTION          :
 *
 * RETURN 
 *
 * NOTES  
 *
 * --------------------------------------------------------------------------*/

void Mainloop_SetPwrOnOffTime(void)
{
	char	auto_dateWakeUp[9], auto_timeWakeUp[5];

	Utils_GetDateTime (auto_dateWakeUp, auto_timeWakeUp);

	sprintf(auto_timeWakeUp,"%d",auto_EOD_time_int);

	Utils_SetPwrOnAlarm (auto_dateWakeUp, auto_timeWakeUp);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_CheckManual
 *
 * DESCRIPTION  : Checks if it is manual entry and processes.
 *
 * RETURN               : none
 *
 * PROGRAMMER   : REFIK IRFAN OZTURK
 * 
 * LAST MODIFIED: 03/05/99
 *
 * NOTES                : Idle loop control for manuel entries.
 * --------------------------------------------------------------------------*/
void Mainloop_CheckManual(void)
{
	byte lbChar;
	char Buffer[LEN_GETKEYBOARD];   /* test amaclý */
	byte Len;                                               /* test amaclý*/
	byte ret_code;
	byte dummy;
	EMVTransData etdData;
	RowData rdSDAData,myrdData;
	PTStruct fpPTStruct;
	boolean IsEMV;
	ApplicationParams *pAppData;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
	EMVLanguages pelsLangs;
	byte pbErr;
	byte Transaction_Ret;
	TerminalAppList fpAppList;
	void *dummy2;
	boolean ReversalPendingTemp;

	/* initialize the flags.  */
	FIRST_PHONE_FLAG			= FALSE;
	SERVER_OPEN_TIMEOUT_FLAG	= FALSE;
	/** XLS_PAYMENT 25/07/2001 irfan **/
	RECOPY_FLAG						= FALSE;
	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;

	/*@bm 24.08.2004 */
	if (!EMVAxessStatu)
	IS_LOYALTY_INSTALLMENT_INTEREST = FALSE;
	/*@bm*/

	Files_ReadEMVAppParam(&fpAppList);
	memcpy(&pelsLangs,&elsLangs,sizeof(pelsLangs));

	lbChar = Kb_WaitForKey();
/*	PrntUtil_Cr(3);
	PrntUtil_BufAsciiHex((byte*)&lbChar,1);
	PrntUtil_Cr(3);
		
	/*if(lbChar == FUNC_KEY_NURIT_3000)*/
/*	if((lbChar==MMI_ENTER)  )*/

/*	if((lbChar==MMI_F)  )
	{
		if(genius_flag=='0')
			Mainloop_ShowFMenu();
		else
			Mainloop_ShowFMenuWOBizz();
	}
	
/*	Printer_WriteStr("Key pressed: ");
	PrntUtil_BufAsciiHex(&lbChar,1);
	PrntUtil_Cr(2);*/
	
	switch (lbChar)
	{
	case	MMI_F		:
//	case	0x8C		:
		if(genius_flag=='0')
				Mainloop_ShowFMenu(  );		
		else
			if ( OFFLINE_ACIK_FLAG )	/* bm OFF 21.10.2004 */
				Mainloop_ShowFMenuOffline(  );
			else		
				Mainloop_ShowFMenuWOBizz( );
		break;	
//	case	MENU		:
	case 	MMI_MENU	:
//	case	0x43		:
		if(!Utils_AskUserPassword()) return;

		/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
/* bm OPT	if ( POS_Type == '1' ) */
			if ( ( POS_Type == '3' ) || ( POS_Type == '5' ) )
		{	/* akbank */
			Mainloop_TransactionMenu();
			break;
		}
		else
		{	/* axess */
			MainloopXLS_LoyaltyMainMenu();
			break;
		}
	case BKSP:	/* Reprint */  
		/** XLS_PAYMENT 25/07/2001 irfan.**/
		RECOPY_FLAG = TRUE;
		Files_ReadAllParams();
		/* 01_18 08/12/99 Kuruluþ yapýlmamýþ bir cihaz için ikinci kopya bastýrýlmýyor*/
		if (INSTALLATION)
		{
			/* 01_18 08/12/99 cihaz üzerinde hiç iþlem yapýlmamýþ ise 2.kopya basýlmýyor*/
			if((seq_no < 1) && (genius_seq_no < 1))
			{
				Mainloop_DisplayWait(" C˜HAZ šZER˜NDE  H˜€ ˜žLEM YOK  ",1);
				return;
			}

			//@bm  15.12.2004
			if( ! BizSlip )
			{
				Slips_PrinterStr("        2. KOPYA        \n\n"); /*01_18 08/12/99 2.kopya olduðu belli olsun diye kondu */
				if(ERROR_RECEIPT == TRUE) Slips_PrintErrorReceipt(TRUE);
				else
				{	
					SECOND_COPY = TRUE;     
					EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
					KampanyaFlag = OldKampanyaFlag;
					Slips_PrintReceipt( FALSE, TRUE );
					OldKampanyaFlag = KampanyaFlag;
				}
			}
			else
			{
				Slips_PrinterStr("        2. KOPYA        \n\n"); 
				if(ERROR_RECEIPT == TRUE) 
					Slips_GPrintErrorReceipt();
				else
				{	
					SECOND_COPY = TRUE;     
					Slips_GPrintReceipt( );
				}		
			}
			//@bm  15.12.2004

		}
		else Mainloop_DisplayWait(" KURULUž YAPIN  ",1);
		break;
	case '1':	/* Sale */
		/* 01_18 14/01/00 El ile kart no'su giriþinde MANUEL_ENTRY_PERMISSION :  1: ÞÝFRE SOR  0: SORMA*/
		if(MANUEL_ENTRY_PERMISSION)
		{
			if(!Utils_AskUserPassword()) return;
		}
		if (!EMVAxessStatu) EMVAxess = FALSE;
		tran_type = SALE;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
/*
		if (Mainloop_Get_CardInfo(&rdSDAData, &etdData, &fpPTStruct, &eaApplication, &IsEMV))
*/
		if (Mainloop_Get_CardInfo(&pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex, &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData, &fpPTStruct, &IsEMV, FALSE))
/* Toygar - NextEMV Start - Ok */
/*		if (Mainloop_Get_CardInfo())*/
/* Toygar EMVed */
		{ 
			/** XLS_PAYMENT 25/07/2001 irfan. Arama islemi Kart girisinden sonraya alindi **/
			/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
			if (!( EMVAxess && !EMVAxessStatu ))
			{
                if(Host_PreDial() != STAT_OK) 
			    {
				Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
				return;
			    }
                        }
				memset( Received_Buffer, 0, sizeof( Received_Buffer) );
				if ( EMVAxess && !EMVAxessStatu )
				{
					memset( fpPTStruct.Pan, 0, sizeof( fpPTStruct.Pan ) );
					EmvDb_Find( &myrdData, tagPAN );
					EmvIF_BinToAscii( myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof( fpPTStruct.Pan ), FALSE );
					while ( fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] == 'F' )
						fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] = 0;
					fpPTStruct.PanLength = strlen( fpPTStruct.Pan );
					memset( card_no, 0, 20 );
					memcpy( card_no, fpPTStruct.Pan, fpPTStruct.PanLength );
					if ( !EMVAxessGetAmount(  ) )
					{
						Mainloop_DisplayWait( "EMVAxessGetAmount Hatasi", 2 );
						EmvIF_ClearDsp(DSP_MERCHANT);
						EMVAxessStatu = FALSE;
					}
					else EMVAxessStatu = TRUE;
					Kb_Insert(MMI_ESCAPE);
					Kb_Insert(MMI_ESCAPE);
					Kb_Insert(MMI_ESCAPE);
					Kb_Insert(MMI_ESCAPE);
					return;
				}

				ret_code =
					Trans_Transaction( pAppData, &fpAppList, &pMutualApps,
									   &pbMutualAppIndex,
									   &lsTerminalDataStruct, &pelsLangs,
									   &pbErr, &rdSDAData, &etdData,
									   &fpPTStruct, IsEMV, FALSE );

			/*save parameters files */
			/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
			eger yazamas ise idle loop'a donuyor */
			if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
			switch (ret_code)
			{
			case STAT_OK:
				break;
			case AKNET_POS_TIMEOUT_ERROR:
				Mainloop_DisplayWait("POS TIMEOUT",2);
				break;
			case STAT_NOK:
				Mainloop_DisplayWait(" TAMAMLANAMADI  TEKRAR DENEY˜N˜Z",2);
				break;
			case TRR_NO_CONNECTION:
				Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
				break;
			}
		}

			/* 20.07.2004 axess visa degisiklikleri */
			EMVAxessStatu = FALSE;

		Host_Disconnect();      
		Mte_Wait(300);

		/** XLS_PAYMENT 19/07/2001 irfan */
		Mainloop_XLSRemoveChipCard();
		EmvIF_ClearDsp(DSP_MERCHANT);
		break;
	/*MK DEF:4*/
	case '2':	/* Önprovizyon */
		/*MK DEF:6a*///if(!Utils_AskUserPassword()) return;
		Menu4_Entry0(dummy2);
		break;
	case '3':	/* Bakiye Sorma */
		/*if(!Utils_AskUserPassword()) return;*/
		Menu4_Entry1(dummy2);
		break;
	case '4':	/* Manuel islem */
		/*if(!Utils_AskUserPassword()) return;*/
		Menu4_Entry2(dummy2);
		break;
	/*MK DEF:4*/
	case '5':	/* Return */
	case DOWN_ARROW:	/* Return */
//	case 0x58:	/* Return */
		if(RETURN_PERMISSION)
		{
			if(!Utils_AskUserPassword()) return;
			if (!EMVAxessStatu) EMVAxess = FALSE;
			tran_type = RETURN;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
				if ( Mainloop_Get_CardInfo
					 ( &pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex,
					   &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData,
					   &fpPTStruct, &IsEMV, FALSE ) )
/* Toygar - NextEMV Start - Ok */
/* Toygar EMVed */
			{ 
					if (!( EMVAxess && !EMVAxessStatu ))
					{
				if(Host_PreDial() != STAT_OK) 
				{
					Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
					return;
				}
					}
					memset( Received_Buffer, 0, sizeof( Received_Buffer) );
					if ( EMVAxess && !EMVAxessStatu )
					{
						memset( fpPTStruct.Pan, 0, sizeof( fpPTStruct.Pan ) );
						EmvDb_Find( &myrdData, tagPAN );
						EmvIF_BinToAscii( myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof( fpPTStruct.Pan ), FALSE );
						while ( fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] == 'F' )
							fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] = 0;
						fpPTStruct.PanLength = strlen( fpPTStruct.Pan );
						memset( card_no, 0, 20 );
						memcpy( card_no, fpPTStruct.Pan, fpPTStruct.PanLength );
						if ( !EMVAxessGetAmount(  ) )
						{
							Mainloop_DisplayWait( "EMVAxessGetAmount Hatasi", 2 );
							EmvIF_ClearDsp(DSP_MERCHANT);
							EMVAxessStatu = FALSE;
						}
						else EMVAxessStatu = TRUE;
						Kb_Insert(MMI_ESCAPE);
						Kb_Insert(MMI_ESCAPE);
						Kb_Insert(MMI_ESCAPE);
						Kb_Insert(MMI_ESCAPE);
						return;
					}
					ret_code =
						Trans_Transaction( pAppData, &fpAppList, &pMutualApps,
										   &pbMutualAppIndex,
										   &lsTerminalDataStruct, &pelsLangs,
										   &pbErr, &rdSDAData, &etdData,
										   &fpPTStruct, IsEMV, FALSE );
/*				ret_code = Trans_Transaction();*/

				/*save parameters files */
				/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
				eger yazamas ise idle loop'a donuyor */
				if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
				switch (ret_code)
				{
				case STAT_OK:
					break;
				case AKNET_POS_TIMEOUT_ERROR:
					Mainloop_DisplayWait("POS TIMEOUT",2);
					break;
				case STAT_NOK:
					Mainloop_DisplayWait("      ˜ADE          HATALI      ",2);
					break;
				case TRR_NO_CONNECTION:
					Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
					break;
				}
			}

			tran_type = SALE; /*@bm 17.09.2004 */
			EMVAxessStatu = FALSE;
			Host_Disconnect();      
			Mte_Wait(300);
		}
		else
		{
			EMVAxessStatu = FALSE;
			Mainloop_DisplayWait( "    ˜ADEYE          KAPALI     ", 1 );
		}

		Mainloop_XLSRemoveChipCard();

		Kb_Insert(MMI_ESCAPE); /*@bm 17.09.2004 */
		Kb_Insert(MMI_ESCAPE);
		Kb_Insert(MMI_ESCAPE);
		Kb_Insert(MMI_ESCAPE);
		Kb_Insert(MMI_ESCAPE);
		Kb_Insert(MMI_ESCAPE);
		Kb_Insert(MMI_ESCAPE);
		break;
		
	/*MK DEF:4*/
	case '6':	/* Cancel */
	case MMI_000:	/* Cancel */
	case RIGHT_ARROW:	/* Cancel */
		if(!Utils_AskUserPassword()) return;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
			if ( Mainloop_Get_CardInfo
				 ( &pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex,
				   &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData,
				   &fpPTStruct, &IsEMV, FALSE ) )
/* Toygar - NextEMV Start - Ok */
/* Toygar EMVed */
		{ 
			/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
			/*Host_PreDial();*/
			if(Host_PreDial() != STAT_OK) 
			{
				Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
				return;
			}

				ret_code = Trans_Cancel( IsEMV, FALSE );

			/*save parameters files */
			/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
			eger yazamas ise idle loop'a donuyor */
			if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
			switch (ret_code)
			{
			case STAT_OK:
			/* 06_08 basarili iptal sonucu verde pod'a bildiriliyor */
			if( VERDE_TRANSACTION )
			{	/* send RES4 get ACK  */
				if(Verde_SendRes4() != STAT_OK)	Verde_SendControlChar(EOT);
			}
				break;
			case AKNET_POS_TIMEOUT_ERROR:        /* UPDATE 01_18 02/12/99 TIMEOUT_ERROR idi.Ancak bunun kullanýlmamasý gerekir*/
				Mainloop_DisplayWait("POS TIMEOUT",2);
				break;
			case STAT_NOK:
				Mainloop_DisplayWait("      ˜PTAL         HATALI      ",2);
				break;
			case TRR_NO_CONNECTION:
				Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
				break;
			}
		}
		Host_Disconnect();      
		Mte_Wait(300);

		/** XLS_PAYMENT 19/07/2001 irfan */
		Mainloop_XLSRemoveChipCard();
		EmvIF_ClearDsp(DSP_MERCHANT);
		break;
	/*MK DEF:4*/
//	case '7':	/* Gösterim */
//		if(!Utils_AskUserPassword()) return;
//		Menu4_Entry4(dummy2);
//		break;
	case '8':	/* Ara rapor */
		if(!Utils_AskUserPassword()) return;
		Menu4_Entry3(dummy2);
		break;
	case '9':                   /* EOD */
	case UP_ARROW:                   /* EOD */
//	case 0x42:                   /* EOD */
		/* eger yuklemenden sonra ilk calýsma ise once kurulus yapýlmalý. once gunsonu yapýlamaz */
		ReversalPendingTemp=ReversalPending;
		if(FIRST_RUN)
		{
			Mainloop_DisplayWait(" KURULUž YAPIN  ",2);
			return;
		}
		if(!Utils_AskUserPassword()) return;
/* Toygar EMVed *//*04_15
		if(record_counter == 0)
		{
			GrphText_Cls (TRUE);
			if(Mainloop_DisplayWait("   YI¦IN BOž    ",10) != MMI_ENTER) return;
		}*/

/* Toygar EMVed */	
		/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
		/*Host_PreDial();*/
		if(Host_PreDial() != STAT_OK) 
		{
			Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
			return;
		}
/* Toygar EMVed */
		ret_code = Trans_EODTransaction();
/* Toygar EMVed */

		/*save parameters files */
		/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
		eger yazamas ise idle loop'a donuyor */
		if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
		switch (ret_code)
		{
		case STAT_OK:
			Mainloop_DisplayWait("GšNSONU OK.",1);

			FirstRun=FALSE; /*MK DEF:22*/
			/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
			INSTALLATION_ONGOING=FALSE;
			
/* bm OPT			if ( POS_Type == '0' )	/* axess pos icin yapiliyor */
					if ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
					{			/* axess pos icin yapiliyor */
				/** XLS_INTEGRATION 15/08/2001 irfan. Sadece basarili payment EOD'den sonra baslatilacak. **/
				/** XLS_PAYMENT 27/07/2001 irfan. initiate XLS EOD **/
				/** XLS_INTEGRATION 23/08/2001 xls eod cagrilmadan once hat kapatiliyor **/
				Host_Disconnect();      
				Mte_Wait(300);
/*				Printer_WriteStr("XLS_EOD(XLS_CALL_FULL)\n");*/
				XLS_EOD(XLS_CALL_FULL);
			}
			break;
		case AKNET_POS_TIMEOUT_ERROR:        /* UPDATE 01_18 02/12/99 TIMEOUT_ERROR idi.Ancak bunun kullanýlmamasý gerekir*/
			Mainloop_DisplayWait("POS TIMEOUT",2);
			break;
		case STAT_NOK:
			Mainloop_DisplayWait("     GšNSONU       YAPILAMADI   ",2);
			break;
		case TRR_NO_CONNECTION:
			Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
			break;
		}
		Host_Disconnect();      
		Mte_Wait(300);
		ReversalPending=ReversalPendingTemp;
		break;
		
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Mainloop_CardProcess
 *
 * DESCRIPTION          : Analyze card number and seperate it to its segments like
 *                                        card_no, exp_date and cvv_2
 *
 * RETURN                       : 
 *
 * NOTES:       
 *
 * MODIFIED     ON              : 05/05/99 By: R.Ýrfan ÖZTÜRK
 * --------------------------------------------------------------------------*/
boolean Mainloop_CardProcess(void)
{
	byte Count;
	char Buffer[LEN_GETKEYBOARD];
	byte Len;

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));




	/* initialize the card_no , exp_date and cvv_2 */
	memset(card_no,' ',20);
	memset(exp_date,' ',5);
	memset(cvv_2,' ',4);
	memset(last_4_digit,' ',5);

	/* seperate card no and calculate its length */
	for (Count = 0; ((card_info[Count]!= '=') && (Count < LEN_TRACKISO2)); Count++);

	if ((Count>19) || (Count<13))
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" KART NO HATALI ",2);             
		return(FALSE);
	}

	card_no_length = Count;
	memcpy(card_no,card_info, Count);
	card_no[Count] = 0;


	/* seperate exp_date */
	
	memcpy(exp_date,card_info+card_no_length+1,4);
	exp_date[4]=0;

	/* seperate cvv_2 */
	
	memcpy(cvv_2,card_info+card_no_length+1+4,3);
	cvv_2[3]=0;
/*
	Printer_WriteStr("cvv_2\n");
	PrntUtil_BufAsciiHex((byte *)cvv_2,4);
*/
	/* LAST 4 DIGIT  control */
	
	if(LAST_4_DIGIT_FLAG)
	{
		 /*get the last 4 digit from card info read by card reader */
		memcpy(last_4_digit,card_no+card_no_length-4, 4);
		last_4_digit[4]=0;

		/* get the manual last 4 digit from keyboard */
		if(!Mainloop_GetKeyboardInput("SON 4 DIGIT?  ",Buffer,4,4,    
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, FALSE))
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("G˜R˜ž YAPILAMADI",2);             /*DEGISECEK_MESAJ*/
			return(FALSE);
		}
		 else
		{
			memcpy(manual_last_4_digit,Buffer,Len);
			manual_last_4_digit[Len]=0;
		}
		 
		if (strcmp(last_4_digit,manual_last_4_digit) != 0 )
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("KARTA EL KOYUNUZ",2);
			return(FALSE);
		}

	}
	

		/* LuhnCheck Control.*/
	if (!Utils_LuhnCheck())
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" GE€ERS˜Z KART  ",2);     /*DEGISECEK_MESAJ*/
		return (FALSE);
	}


	return (TRUE);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_DisplayWait
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
boolean Mainloop_DisplayWait(char *Message,int Delay)
{
	char ToDisplay[17];
	char ch;
	int Delay_Time;

	Delay_Time = 0;

    if (strlen(Message)<17) Debug_GrafPrint2 ((char *)Message, 3);
	else
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		memcpy(ToDisplay,Message,16);
		ToDisplay[16] = 0;
		Debug_GrafPrint2 ((char *)ToDisplay, 2);
		memcpy(ToDisplay,Message+16,16);
		ToDisplay[16] = 0;
		Debug_GrafPrint2 ((char *)ToDisplay, 3);
	}



	while(Delay_Time < Delay*100)
	{
		ch = Kb_Read();
		switch(ch)
		{
			case MMI_ESCAPE :
				return MMI_ESCAPE;                                      
					
		    case MMI_ENTER :
				return MMI_ENTER;                                       
		} 
		Mte_Wait(10);
		Delay_Time++;
	}
	return(ch == MMI_ENTER);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Mainloop_SwipeCard
 *
 * DESCRIPTION          : Chose the entry type (from card reader of keypad)   
 *
 * RETURN                       : none
 *
 * NOTES                        : r.i.o & t.c.a
 *
 * --------------------------------------------------------------------------*/
void Mainloop_SwipeCard (boolean Idle)
{
	ulint	Timeout=0,Timeout2;
	/* 04_10 */
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];
	char temp_string[200];

	sint track1_err;
	sint track2_err;
	int TransOfflineNo, TransOnlineNo;
	char dummyy;
	char OldTime[20];
	byte auto_eod_ret;
	char auto_date_c[9], auto_time_c[5];
	int  auto_time;
	sint i;
	boolean AUTO_EOD_PERMISSION;
	byte tran_ret;
	char test_str[50];	/* irfan test */
	boolean myLogoCount = FALSE;
	byte myCount = 0;
	usint	idle_time_out;

	idle_time_out=300;
	Kb_Idle(300);Mte_Wait(60);
	if(DEBUG_MODE) Printer_WriteStr("KB Idle XLS 1\n");

	strcpy(OldTime,csSTARS);
	CardRdr_Enable();
	EmvIF_ClearDsp(DSP_MERCHANT);
	while (!CardRdr_IsCardCompleted(&track1_err,&track2_err))

	if (dummyy = Kb_Read())
	{
		Mte_Wait(50);
		KEY_PRESSED_GLOBAL=TRUE;
			GPRS_TryTime=0;

	    break;
	}
	else
	{

		if(!Mainloop_PwrDownCnt())
		{
			Mte_Wait(30);
			Kb_Idle(idle_time_out);
			if(DEBUG_MODE)Printer_WriteStr("KB Idle XLS 3\n");
			Mte_Wait(60);
			//KEY_PRESSED_GLOBAL=TRUE;
			Kb_Insert(MMI_ESCAPE);
		}
		Mte_Wait(10);
		Timeout2=Scan_MillSec();
		if( (Timeout2-Timeout)>2500 || !Timeout)
		{
			GSM_NOS_DisplaySignalLevel();
			GSM_NOS_DisplayBatteryLevel();
			GSM_NOS_DisplayBatteryStatus();
			Mainloop_ShowOperator();

			Timeout=Scan_MillSec();
		}

		if (Idle)
		{
				if (!EMVAxessStatu) EMVAxess = FALSE;
				AlreadyChecked = FALSE;
			/** XLS_PAYMENT 27/07/2001 irfan. XLS Idle Loop Operations **/
			/** XLS 04_02 19/12/2001 irfan */
/*bm OPT		if ( POS_Type == '1' ) */
			myLogoCount = !myLogoCount;
			if ( ( POS_Type == '3' ) || ( POS_Type == '5' ) )
			{	/* akbank */
				image_number++;
				if(image_number > 24) {
					image_number = 1;
					Kb_Read();
				}
				if(genius_flag=='0') {
					Mainloop_BizIdleLogo();
					if(rState>88) rState=0;
					rState++;
				}else{
					Mte_Wait(50);
					Mainloop_IdleLogo(); /* sadece graphic display icin gercerli */
				}
				Mainloop_XLSIdleLoop();
			}
			else
			{	/* axess pos */
				if (myCount > 24) 
				{
					EMVAxessStatu = FALSE; 
					myCount = 0;
					myLogoCount = TRUE;
				}
				if (EMVAxessStatu) { myCount++; myLogoCount = FALSE; }
				Mainloop_XLSIdleLoop();
				if (EMVAxessStatu) { myCount++; myLogoCount = FALSE; }
				image_number++;
				if(image_number > 24) {
					image_number = 1;
					Kb_Read();
				}
				if(genius_flag=='0') {
					Mainloop_BizIdleLogo();
					if(rState>88) rState=0;
					rState++;
				}else{
					/*Mainloop_IdleLogoAnimation();*/
					Mainloop_IdleLogoAnimationNew(myLogoCount);
				}
			}
			
/*
			Printer_WriteStr("auto_EOD_time:\n");
			PrntUtil_BufAsciiHex((byte *)auto_EOD_time,5);
			Printer_WriteStr("auto_EOD_time_int:\n");
			PrntUtil_BufAsciiHex((byte *)&auto_EOD_time_int,4);
*/

/*			if(auto_EOD_time_int != 9999)*/
			if(strcmp(auto_EOD_time, "9999") != 0)
			{
				/* auto gun sonu saatinin gelip gelmedigini kontrol eder */
				Utils_GetDateTime (auto_date_c, auto_time_c);
				auto_time_c[2] = auto_time_c[3];
				auto_time_c[3] = auto_time_c[4];
				auto_time_c[4] = 0;

				auto_date_c[8] = 0;             /* null ile sonlandýrýlýyor */

				auto_time = atoi(auto_time_c);

/*
				Printer_WriteStr("auto_EOD_time:\n");
				PrntUtil_BufAsciiHex((byte *)auto_EOD_time,5);
				Printer_WriteStr("auto_EOD_time_int:\n");
				PrntUtil_BufAsciiHex((byte *)&auto_EOD_time_int,4);
				Printer_WriteStr("auto_time:\n");
				PrntUtil_BufAsciiHex((byte *)&auto_time,4);
				Printer_WriteStr("auto_time_c:\n");
				PrntUtil_BufAsciiHex((byte *)&auto_time_c,4);
*/

				if(auto_time >= auto_EOD_time_int)
				{
/*					Printer_WriteStr("TIME_IS_UP = TRUE;\n");*/
					TIME_IS_UP = TRUE;                      /* auto eod saati geldi */
				}
				else
				{
/*					Printer_WriteStr("TIME_IS_UP = FALSE;\n");*/
					TIME_IS_UP = FALSE;                     /* auto eod saati gelmedi */
				}

				/* eger gun ýcýnde auto edo yapýldý ise bir daha yapýlmamasý ýcýn flag false yapýlýr */
				if(Utils_Compare_Strings(auto_date_c, last_eod_date) != 0)
				{
/*					Printer_WriteStr("AUTO_EOD_PERMISSION = TRUE;\n");*/
					AUTO_EOD_PERMISSION = TRUE;
				}
				else
				{
/*					Printer_WriteStr("AUTO_EOD_PERMISSION = FALSE;\n");*/
					AUTO_EOD_PERMISSION = FALSE;
				}

				/* Auto EOD */
				if((TIME_IS_UP) && (AUTO_EOD_PERMISSION))       /* eger auto eod yapýlmadý ve autoeod saati geldi ise*/
				{
						for(i = 0; i<5 ; i++)
						{
							/* 01_18 22/06/2000 eger hat mesgul ise 5 deneme sonunda idle loop a
							donmesi saglanýr*/
							if(Host_PreDial() == STAT_OK)
							{
/* Toygar EMVed */
									auto_eod_ret = Trans_EODTransaction();
/* Toygar EMVed */
								/* 01_18 22/06/2000 irfan, auto gunsonu yaparken parametre yukleme sonrasýnda
								   hat disconnect olmuyor idi. auto gunsonu sonrasýnda sonuc ne olursa olsun
								   host disconnect ediliyor. */
									Host_Disconnect();
									Mte_Wait(1000);
									if (auto_eod_ret == STAT_OK)
									{
										/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
/* bm OPT							if ( POS_Type == '0' )	/* axess POS icin yapiliyor */
									if ( ( POS_Type == '4' ) ||( POS_Type == '6' ) )
										 
										{
											/** XLS_PAYMENT 27/07/2001 irfan. aout. XLS_EOD */
/*											Printer_WriteStr("XLS_EOD(XLS_CALL_AUTOMATIC)\n");*/
											XLS_EOD(XLS_CALL_AUTOMATIC);
										}
										/* en son auto eod yapýlmýþ tarih set ediliyor */
										Utils_GetDateTime (auto_date_c, auto_time_c);
										auto_time_c[2] = auto_time_c[3];
										auto_time_c[3] = auto_time_c[4];
										auto_time_c[4] = 0;
										auto_date_c[8] = 0;             /* null ile sonlandýrýlýyor */
										memcpy(last_eod_date, auto_date_c,8);
										last_eod_date[8] = 0;
										EOD_flag=FALSE;                                 /* gun sonu tamamlandý anlamýna geliyor */
										Trans_Initialize_Variables();
										/*save parameters files */
										/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
										eger yazamas ise idle loop'a donuyor */
										if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
										return;
									}
									else
									{
										/* 01_18 22/06/2000 Yukarýya Trans_EODTransaction() sonuna konuldu.*/
										/*Host_Disconnect();*/
										Display_DownDisplay(" BAžARISIZ! EOD ");
										Printer_WriteStr("*** OTOMATIK GUNSONU ********* YAPILAMADI ******\n\n\n");
										/* 01_18 22/06/2000 Yukarýya Trans_EODTransaction() sonuna konuldu.*/
										/*Mte_Wait(1000);*/
									}
							}
							else
							{
								Display_DownDisplay(" BAžARISIZ! EOD    HAT MESGUL   ");
								Printer_WriteStr("****** HAT MESGUL ******\n");
								Printer_WriteStr("*** OTOMATIK GUNSONU ********* YAPILAMADI ******\n\n\n");
																
							}

						}

						Utils_GetDateTime (auto_date_c, auto_time_c);
						auto_time_c[2] = auto_time_c[3];
						auto_time_c[3] = auto_time_c[4];
						auto_time_c[4] = 0;
						auto_date_c[8] = 0;             /* null ile sonlandýrýlýyor */
						memcpy(last_eod_date, auto_date_c,8);
						last_eod_date[8] = 0;
						/*save parameters files */
						/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
						eger yazamas ise idle loop'a donuyor */
						if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
						Printer_WriteStr("***********************\n");
						Printer_WriteStr("*** 5. DENEME BITTI ***\n");
						Printer_WriteStr("***********************\n\n\n\n\n\n\n\n");
				}
			}

			/* Idle loop'da calýsacak programlar yazýlacak */
			/*Smart_AkBankDemoExt ();*/
			/*Debug_GrafPrint2 ( " ~~~~~~~~~~~~~~ ", 1); 
			Debug_GrafPrint2 ( "  A K B A N K   ", 2); 
			Debug_GrafPrint2 ( " ~~~~~~~~~~~~~~ ", 3); */
			Files_GetTransactionCounts(&TransOnlineNo, &TransOfflineNo);
			Mainloop_IDLEUpdate(OldTime);
			Auto_IDLEUpdate(OldTime, TransOnlineNo, TransOfflineNo, myLogoCount);
			if(genius_flag=='0') {
				/*************** genius idle loop *************/
				/*Printer_WriteStr("x");
				Printer_WaitForTextReady(); */
				
/*				if(Utils_Compare_Strings(auto_date_c, last_genius_eod_date) != 0)*/
				/* auto gun sonu saatinin gelip gelmedigini kontrol eder */
				Utils_GetDateTime (auto_date_c, auto_time_c);
				auto_time_c[2] = auto_time_c[3];
				auto_time_c[3] = auto_time_c[4];
				auto_time_c[4] = 0;

				auto_date_c[8] = 0;             /* null ile sonlandýrýlýyor */
				if(strncmp(auto_date_c, last_genius_eod_date,8) != 0)
				{
					
					/* 04_10 06/01/2003 Key giriþinde null giriþi için kullanýlýyor. */
					memset(value_null, 0 , sizeof(value_null));
					memcpy(temp_string, "BIZ.CARD DOSYASI SILINECEK. ARA  RAPOR ALMAK     ISTER MISINIZ ? EVET>1  HAYIR>0",80);
					if(!Mainloop_GeniusGetKeyboardInput(temp_string,Buffer,1,1,
									   FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
					{
						EmvIF_ClearDsp(DSP_MERCHANT);
						Mainloop_DisplayWait("  G˜R˜ž HATASI  ",2);
						return;
					}

					EmvIF_ClearDsp(DSP_MERCHANT);

					if(Buffer[0] == '1')
					{
						Slips_PrintReportHeader(FALSE);
						Slips_GPrintDetailRecord();
					}

					if(Files_GInitFileGenius() !=STAT_OK)
					{
						Slips_PrinterStr("\n\nBIZ.CARD FILE SILINEMEDI\n\n\n\n");
						Mainloop_DisplayWait("BIZ.CARD FILE SILINEMEDI",1);
					}
					else
					{
						/* genis bilgileri */
						/* 04_10 27/12/2002 Aþaðýdaki mesaj sadece debug mode da çýkar hale getirildi*/
						if(DEBUG_MODE)
						{
							Slips_PrinterStr("\n\n---GENIUSFILE SILINDI---\n");
							Slips_PrinterStr("\nlast_genius_eod_date:\n");
							Slips_PrinterStr(last_genius_eod_date);
							Slips_PrinterStr("\nauto_date_c:");
							Slips_PrinterStr(auto_date_c);
							Slips_PrinterStr("\n\n");
						}
						memcpy(last_genius_eod_date, auto_date_c,8);
						last_genius_eod_date[8] = 0;
						/* update edilen last_genius_eod_date'in dosyaya yazilmasini saglar */
						if(Files_WriteAllParams() != STAT_OK) 
						{
							EmvIF_ClearDsp(DSP_MERCHANT);
							Mainloop_DisplayWait("  PARAMETRELER     YAZILAMADI",1);
						}

					}

			        }
                        }/*if(genius_flag) end*/
		}
		else
			Display_UpDisplay((char *)MESSAGE_SWIPE_CARD);
	}
    if (!KEY_PRESSED_GLOBAL)
	{
		/* 01_18 27/06/2000 track 1 in de kontrolu eklendi */
		/*if(!ISIMYAZMA_FLAG) track1_err = 0; /* eger isim yazma kapalý ise, track 1 okunamasa bile sorun cýkarmýyor */
		if (!track1_err)
		{
			TRACK1ERROR = TRUE;		/* track 1 is read correctly */
		}
		else
		{
			TRACK1ERROR = FALSE;	/* track 1 is read incorrectly */
		}

		if (!track2_err) 
		{       
			/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
			/*Host_PreDial();*/
			if(Host_PreDial() != STAT_OK) 
			{
  			        EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
				return;
			}


			if(!Mainloop_CheckSwipe(FALSE))
			{
			        EmvIF_ClearDsp(DSP_MERCHANT);
//			        Mainloop_DisplayWait("  OKUMA HATASI",2);
				Host_Disconnect();
				return;
			}
			tran_type = SALE;
			 /**/ tran_ret =
				Trans_Transaction( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FALSE,
								   FALSE );
			 /**/
/*			tran_ret = Trans_Transaction();*/

			/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
			eger yazamas ise idle loop'a donuyor */
			if(Files_WriteAllParams() != STAT_OK) 
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("  PARAMETRELER     YAZILAMADI",1);
			}
		        Host_Disconnect();
			switch (tran_ret)
			{
				case    STAT_OK:
						break;
				case    AKNET_POS_TIMEOUT_ERROR:
						EmvIF_ClearDsp(DSP_MERCHANT);
						Mainloop_DisplayWait("POS TIMEOUT",2);
						break;

				case    STAT_NOK:
						EmvIF_ClearDsp(DSP_MERCHANT);
						Mainloop_DisplayWait(" TAMAMLANAMADI  TEKRAR DENEY˜N˜Z",2);
						break;
				case    TRR_NO_CONNECTION:
						EmvIF_ClearDsp(DSP_MERCHANT);
						Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
						break;
			}
			Host_Disconnect();


		}
		else
		{
//			Mainloop_DisplayWait("KART OKUMA HATASI",2);
		}
		EmvIF_ClearDsp(DSP_MERCHANT);
	}
    else
    {
		CardRdr_Disable();
		Kb_Insert(dummyy);
    }
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Main_Loop.
 *
 * DESCRIPTION: This Function Creates The Main Loop for the APPLICATION
 *
 * RETURN:      None.
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
void MainLoop(void)
{
	char  DateTime [20], TimeDate[20];
	char temp[5];
	sint tt;
	
	OfflineParametersStruct lbdummy;
	
//	Kb_NosDisable();

	Printer_WriteStr("\n\n************************");
	Printer_WriteStr("DEBUGMODE KULLANILABILIR\n");	//bm gec
//	Printer_WriteStr("DEBUGMODE KULLANILAMAZ\n");
	Printer_WriteStr("************************\n\n\n\n");

	/* v04_05 CALISMA_KOSULLARI_UPDATE 16/07/2002 */
	CALISMA_KOSULLARI_MODE = TRUE;

	DEBUG_MODE = FALSE;          
	InitMasterKey = FALSE;
	GSMCALL=FALSE;
	USE_GPRS=FALSE;
	EMV_DEBUG_MODE = FALSE;
	TVR_TSI = FALSE;
	PIN_BLOCK = TRUE;
	SEND_EMV_FIELDCHANGES = FALSE;
	OnlinePinEntry = FALSE;
/*	BUYUKFIRMA = TRUE;*/
	KAMPANYA_FLAG = 0;
	Reversal_Buffer_Len = 0;
	BFIRMA_BAUD=4800;
	
	GrphText_Cls (FALSE);
	GrphText_DrawDisp ();
	Mainloop_DisplaySet();
	Kb_ClickTimeSet( 60 );		/* set buzzer on */
	
	/* Set Language For Communication Handler  is inside Mainloop_DisplaySet
	ComHndlr_SetCallerLanguage ((char *)My_Message [0]);*/
	memset( kampanya_str, 0, sizeof( kampanya_str ) );
	memset( Kampanya_Data, '0', sizeof(Kampanya_Data) );
	memset(Kasiyer_No, '0' , sizeof(Kasiyer_No));

	/* 01_18 04/08/2000 Eðer RamDisk de file lar var ise eski deðerler okunur
	   yok ise Tüm parametreler initialize edilir. Boylelikle Idle loop da ilk deðerleri 
	   olmadan kullanýlan deðiþkenlerin initialize iþlemi yapýlmýþ olur.*/
	/** XLS 04_02 21/12/2001 irfan. POS tipi seciliyor **/
	/*verde. 07_10. 18/11/2002 restart yediginde akbank olan bir pos axess'e donusuyor. bu da dogru degil.
	*/POS_Type = '0'; /* Default Axess POS */
	/*POS_Type = '1';  /*Default Axess degil*/
	genius_flag = '1'; /*Default bizz card disabled*/
	
	/** PAYMENT_XLS  16/07/2001 irfan initialize XLS permanent varibales */
	XLS_Start();
	Mte_Wait(50);
	/** XLS_INTEGRATION2 05/11/2001 irfan. initialize the counter**/
	image_number = 1;
	rState=0;

	BadICC = 0;
	POSEntryFlag = 0;
	POSEntryFlagUsed = TRUE;
	ReversalPending = FALSE;
	ChangesPending = FALSE;
	EMVAxessStatu = FALSE;			/* 20.07.2004 */
	
	/* 06_30 
	   Create Offline parameters with initialized parameters if not read (meaning that file is not exist) */
	if (Offline_ReadOfflineParameters(&lbdummy) != STAT_OK) 
	{
		Offline_InitOfflineParameterFile();
		Offline_InitOfflineAkbBatchFile();
	}
 	Kb_Read();
	if (Files_ReadAllParams() != STAT_OK) {
		Files_InitParameters();
		Trans_Init_Permanent_Params();
		strcpy(UserPassword, "4321");
		Files_WriteAllParams();
		POS_Type = '3';			/*24/06/2004 Akbak New POS */



		GPRS_USEGPRS=FALSE;

		FirstRun=TRUE;
		memset(GsmPin,0,LEN_GETKEYBOARD);
		Mainloop_GSMStatistInit();
		EmvIF_ClearDsp(DSP_MERCHANT);
	}
	else	FirstRun=FALSE;
	EmvIF_ClearDsp(DSP_MERCHANT);
	
	Mte_Wait(50);
	Kb_Read();
#ifndef _8320
	GSMCALL=TRUE;
	Mainloop_DisplayWait(" žEBEKE ARIYOR",0);
	if(Mainloop_RegisterNetwork(TRUE ,FALSE) != STAT_OK)
	{
		Mainloop_DisplayWait("     žEBEKE         BULUNAMADI",1);
		FirstRun=TRUE;
		memset(GsmPin,0,LEN_GETKEYBOARD);
	}
	Mte_Wait(3000);
	

	Mainloop_DisplayWait("GPRS T O 30SEC",0);
	GPRS_SetTimeout(30000);
#endif
	for(;;)
	{
//		Kb_NosDisable(); 
		gotemail=FALSE;
		Mte_Wait(50);
		Kb_Read();
		/* v04_05 aile puan sorgulama flag false yapýlýyor 15/07/2002 */
		AILE_PUAN_SORGULAMA = FALSE;
		error_printed = FALSE;

		/* 06_30 06/03/2003 */
		OFFLINE_CARREFOURSA = FALSE;

/* Toygar EMVed */
		KeyDownloadStarted = FALSE;
		AppListDownloadStarted = FALSE;
		ExceptionDownloadStarted = FALSE;
		RevocationDownloadStarted = FALSE;		
		CallFlag = TRUE; /* 06_20 offline islemi baslatan flag. defaul'u true (yani default online isle) */
/* Toygar EMVed */
		KEY_PRESSED_GLOBAL = FALSE;
		CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;	/** XLS_PAYMENT 26/07/2001 irfan **/
		RECOPY_FLAG = FALSE;	/** XLS_INTEGRATION2 02/10/2001 irfan **/
		ARA_RAPOR = FALSE;
		Mainloop_SwipeCard (TRUE);
		Mainloop_GetDateTimeFormatted (DateTime, TRUE);		
		memset(temp,0,5);
		memcpy(temp,DateTime+17,2);
		//tt = atoi(&DateTime [17]);
		tt = atoi(temp);
		if(tt%9 == 0) Mte_Wait(10);
		else if(tt%10 == 0) Kb_Read();
		if(KEY_PRESSED_GLOBAL){
			 Mainloop_CheckManual ();
			 EmvIF_ClearDsp(DSP_MERCHANT);
		}
		if (!EMVAxessStatu)
		{	
			tran_type = SALE; /* @bm 12.08.2004 */
			TL_Iade = FALSE;  /*@bm  24.08.2004 */
			IS_LOYALTY_INSTALLMENT_INTEREST = FALSE;
			POSEntryFlagUsed = TRUE; /*24.08.2004 (B) (E) */
			KampanyaFlag = FALSE; /*@bm 17.08.2004 */
		}

		AMEX_FLAG = FALSE;
		
		/*@bm 01.09.2004 -ISSUE2- */	
		GrphText_Cls (TRUE);
		RestrictedFlag = FALSE;
		
		/*@bm  24.08.2004
		IS_LOYALTY_INSTALLMENT_INTEREST = FALSE; /* @bm 13.08.2004 */

	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_Get_CardInfo.
 *
 * DESCRIPTION: Gets the card info from Card Reader or Key Pad or Chip Card
 *
 * RETURN:      None.
 *
 * NOTES:       24/05/99 revised on 19/07/2001 by irfan.
 *
 * --------------------------------------------------------------------------*/
/* Toygar - NextEMV Start - Ok */
/* Toygar - NextEMV End - Ok */
boolean Mainloop_Get_CardInfo( ApplicationParams ** pAppData,
				   TerminalAppList * pTerminalApps,
				   MutualAppList * pMutualApps,
				   byte * pbMutualAppIndex,
				   TerminalParams * pTermData,
				   EMVLanguages * pelsLangs, byte * pbErr,
				   EMVTransData * etdData, PTStruct * fpPTStruct,
				   boolean * IsEMV, boolean Kampanya )
{
	sint track1_err;
	sint track2_err;
	char dummyy;
	char temp_input_type;
	boolean KeyPressedFlag;
	byte CardInserted = 0x00;
	byte lbStatus;
	byte rv;
	byte abSDADataBuf[MAX_NCA_LEN];

	CardRdr_Enable();
	KeyPressedFlag = FALSE;

	*IsEMV = FALSE;
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("KART NO ?",NO_WAIT);
	while (!CardRdr_IsCardCompleted(&track1_err,&track2_err))
	{	
		if (dummyy = Kb_Read())
		{
			KeyPressedFlag = TRUE;
			break;
		}
		if(!OFFLINE_CARREFOURSA)
		{
			if(!CORRUPTED_CHIP_TRANSACTION_FLAG)
			{
				lbStatus = STAT_NOK;
				switch (ICC_Status(0))
				{
				case ICC_NEEDS_RESET	:
					lbStatus = ResetCardExt();
					if (lbStatus == STAT_NOK)
					{
						EmvIF_ClearDsp(DSP_MERCHANT);
						Mainloop_DisplayWait("MANYETIK KART   OKUYUCU KULLANIN",1);
//						EmvIF_ClearDsp(DSP_MERCHANT);
						BadICC = 1;
						break;
					}
				case ICC_OK		: 
					BadICC=0;
					break;
				case ICC_BAD_CARD	: 
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait("CHIP BOZUK      KARTI GE€˜R˜N˜Z ",1);
//					EmvIF_ClearDsp(DSP_MERCHANT);
					BadICC=1;
					break;
				case ICC_SOFTWARE_ERROR	: 
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait("TEKNIK HATA     KARTI GE€˜R˜N˜Z ",1);
//					EmvIF_ClearDsp(DSP_MERCHANT);
					BadICC=1;
					break;
				case ICC_CARD_BLOCKED	: 
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait("CHIP BLOKE      KARTI GE€˜R˜N˜Z ",1);
//					EmvIF_ClearDsp(DSP_MERCHANT);
					BadICC=1;
					break;
				default			: ;
					lbStatus = STAT_NOK;
				}
				if ( lbStatus != STAT_OK ) lbStatus = ResetCardExtLight(  );
				if ( lbStatus == STAT_OK )
				{
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait( (char*)csEMV_PLEASE_WAIT, NO_WAIT );
					*pbErr = 0;
					if ( Main_ApplicationSelection
						 ( pTerminalApps, pMutualApps, pbMutualAppIndex,
						   &lsTerminalDataStruct, pelsLangs,
						   pbErr ) != STAT_OK )
					{
						lbStatus = ResetCardExt();
						if (lbStatus == STAT_OK)
						{
							CardInserted = 0x01;
							break;
						}
						else return ( FALSE );
					}
					else
					{
						CardInserted = 0x02;
						break;
					}
				}
			}
		}
	}

/* Toygar-EMVed */
/*    if(!Card_Inserted_local)*/		/** XLS_PAYMENT 19/07/2001 irfan if chip is not inserted **/
	switch(CardInserted)
	{
	case 0x00:

		if (!KeyPressedFlag)
		{
			/* 01_18 27/06/2000 track 1 in de kontrolu eklendi */
			/*if(!ISIMYAZMA_FLAG) track1_err = 0; /* eger isim yazma kapalý ise, track 1 okunamasa bile sorun cýkarmýyor */

			if (!track1_err)
			{
				TRACK1ERROR = TRUE;
			}
			else
			{
				TRACK1ERROR = FALSE;
			}

			if (!track2_err) 
			{
				if(!Mainloop_CheckSwipe((BadICC == 1)))
				{
					return(FALSE);
				}
			}
			else
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
//				Mainloop_DisplayWait("KART OKUMA HATASI",2);
				return(FALSE);
			}
		}
		else
		{
			
			temp_input_type = input_type;/*MK DEF:12 */
			input_type = INPUT_FROM_KEYPAD;
/*
			Printer_WriteStr("input_type#5\n");
			PrntUtil_BufAsciiHex((byte *)&input_type,1);
*/
			CardRdr_Disable();
			Kb_Insert(dummyy);
			if(!Trans_InputCardNo())
			{
				input_type=temp_input_type;/*MK DEF:12 */
				return(FALSE);
			}
		}
		break;
	case 0x01:

		/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
/* bm OPT	if ( POS_Type == '0' )	/* axess pos icin yapiliyor */
			if ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
			{					/* axess pos icin yapiliyor */
			/** XlS_PAYMENT 19/07/2001 irfan */
			/* chip card is inserted */
			input_type = INPUT_FROM_CHIPCARD;
/*
			Printer_WriteStr("input_type#6\n");
			PrntUtil_BufAsciiHex((byte *)&input_type,1);
*/
			if(E3744_SelectRead() != STAT_OK)
			{
				Mainloop_XLSRemoveChipCard();			
				return (FALSE);
			}
		}
		else return(FALSE);
		break;
	case 0x02:
/*			Iso78164_Init();*/
			EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait((char *)csEMV_PLEASE_WAIT,NO_WAIT);
		*pbErr = 0; 
		if (Main_ApplicationSelection (pTerminalApps, pMutualApps, pbMutualAppIndex,
						&lsTerminalDataStruct, pelsLangs, pbErr) != STAT_OK)
		{
			Mainloop_XLSRemoveChipCard();
			return ( FALSE );
		}
		*pAppData = &(pTerminalApps->TermApp[pMutualApps->MutualApps[*pbMutualAppIndex].TermAppIndex]);
		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
		if (Main_ReadApplicationData(pbErr) != STAT_OK)
		{
			Mainloop_XLSRemoveChipCard();
			return ( FALSE );
		}
		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
		if (Main_DAMethod(*pAppData, pbErr) != STAT_OK)
		{
			Mainloop_XLSRemoveChipCard();
			return ( FALSE );
		}
		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
/*
		if (POSEntryFlag > 0)
		{
			input_type = INPUT_FROM_READER_22;
			POSEntryFlag = 0;
		}
*/
		if (POSEntryFlagUsed)
		{
			input_type = INPUT_FROM_READER_22;
			POSEntryFlag = 0;
		}

			*IsEMV = TRUE;
		break;
/* Toygar-EMVed */
	}
	return(TRUE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: istatistik counter larýnýn o anki deðerlerini basar
 *
 * DESCRIPTION: 
 *
 * RETURN:      None.
 *
 * NOTES:       23/06/99
 *
 * --------------------------------------------------------------------------*/
void test_statistics(void)
{
	char temp[250];

	memset(temp,0,250);
	
	sprintf(temp, "succes:%03d\nno_tone:%03d\nno_answer:%03d\nhandshake_error:%03d\nbusy:%03d\ncarrier_lost:%03d\nno_resp_from_host:%03d\npower_fail:%03d\nno_enq_from_host:%03d\n\n\n",
			d_Statistics1.successful,
			d_Statistics1.no_tone,
			d_Statistics1.no_answer,
			d_Statistics1.handshake_error,
			d_Statistics1.busy,
			d_Statistics1.carrier_lost,
			d_Statistics1.no_response_from_host,
			d_Statistics1.power_fail,
			d_Statistics1.no_enq_from_host);
	temp[strlen(temp)]=0;

	Slips_PrinterStr(temp);


	sprintf(temp, "succes:%03d\nno_tone:%03d\nno_answer:%03d\nhandshake_error:%03d\nbusy:%03d\ncarrýer_lost:%03d\nno_resp_from_host:%03d\npower_fail:%03d\nno_enq_from_host:%03d\n\n",
															d_Statistics2.successful,
															d_Statistics2.no_tone,
															d_Statistics2.no_answer,
															d_Statistics2.handshake_error,
															d_Statistics2.busy,
															d_Statistics2.carrier_lost,
															d_Statistics2.no_response_from_host,
															d_Statistics2.power_fail,
															d_Statistics2.no_enq_from_host);
	temp[strlen(temp)]=0;

	Slips_PrinterStr(temp);

	/** XLS 04_02 19/12/2001 irfan. XLS parametreleri **/
	Slips_PrinterStr("------------------------");
	Slips_PrinterStr("\n\n---XLS PARAMETRELERI----");
	Slips_PrinterStr("(Aktif:1  Inaktif:Diger)");

	memset(temp, 0, sizeof(temp));
	sprintf(temp, "XLS_STATUS : %d\n\n", XLS_Check_Initialization_State());
	Slips_PrinterStr(temp);

	Slips_PrinterStr("------------------------");
	memset(temp, 0, sizeof(temp));
	Slips_PrinterStr("--(Axess:0   Akbank:1)--");
	sprintf(temp, "POS_Type : %c\n\n", POS_Type);
	Slips_PrinterStr(temp);
    if(genius_flag=='0') Printer_WriteStr("Bizz Card Aktif \n\n");
	Slips_PrinterStr("---VERSIYON BILGILERI---");
	Slips_PrinterStr("Prog.Ver.   : 07_01\n");
	Slips_PrinterStr("Prog.Tarihi : 17/12/2004");	/*bm gec*/
	Slips_PrinterStr("------------------------");

	Slips_PrinterStr("-AKTIF XLS DOSYA DURUMU-");
	if(XLS_TpeVide())
	{
		Slips_PrinterStr("AKTIF XLS DOSYASI YOK.\n");
	}
	else
	{
		Slips_PrinterStr("AKTIF XLS DOSYASI VAR!!\n");
	}
	Slips_PrinterStr("------------------------\n");

	Slips_PrinterStr("- ACK BEKLEYEN MAILLER -");
	if(TCPIP_TpeVide())
	{
		Slips_PrinterStr("ACK BEKLEYEN DOSYA YOK\n");
	}
	else
	{
		Slips_PrinterStr("ACK BEKLEYEN DOSYA VAR!!");
	}
	Slips_PrinterStr("------------------------\n");


}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Kullanýlan ATS Prefix lerini listeler
 *
 * DESCRIPTION: 
 *
 * RETURN:      None.
 *
 * NOTES:       14/07/99
 *
 * --------------------------------------------------------------------------*/
void Mainloop_PrintATSPrefix(void)
{
	Slips_PrinterStr("    ATS PREFIXLERI      \n");
	Slips_PrinterStr("========================\n");
	Slips_PrinterStr(ATS_Prefixes); 
	PrntUtil_Cr(2);

	/*** XLS_PAYMENT 12/07/2001 irfan list the Loyalty Prefixes **/
	Slips_PrinterStr("SADAKAT PRG. PREFIXLERI \n");
	Slips_PrinterStr("========================\n");
	Slips_PrinterStr(Loyalty_Prefixes); 
	PrntUtil_Cr(7);

	/*** 05_02 16/09/2002 irfan List the Not_Certified Prefixes **/
	Slips_PrinterStr("NOT CERTIFIED PREFIXLERI\n");
	Slips_PrinterStr("========================\n");
	Slips_PrinterStr(Not_Certified_Prefixes);
	PrntUtil_Cr(7);

	Slips_PrinterStr("************************\n");
	if (PIN_BLOCK) Slips_PrinterStr("PIN BLOCKLAMA ACIK\n");
	else  Slips_PrinterStr("PIN BLOCKLAMA KAPALI\n");
	Slips_PrinterStr("************************\n");
	PrntUtil_Cr(7);

	Slips_PrinterStr("************************\n");
	if (SEND_EMV_FIELDCHANGES) Slips_PrinterStr("DEGISEN GONDERIMI ACIK\n");
	else  Slips_PrinterStr("DEGISEN GONDERIMI KAPALI\n");
	Slips_PrinterStr("************************\n");
	PrntUtil_Cr(7);

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Kullanýlan Debit Prefix lerini listeler
 *
 * DESCRIPTION: 
 *
 * RETURN:      None.
 *
 * NOTES:       14/07/99
 *
 * --------------------------------------------------------------------------*/
void Mainloop_PrintDebitPrefix(void)
{

	char i,j;
	char temp[250];

    Slips_PrinterStr("    DEBIT PREFIXLERI    \n");
	Slips_PrinterStr("========================\n");

    for (i=0; i<7; i++)	/* 06_05 10 yerine 7 yapýldý */
	{
		for(j=0; j<10; j++)	/* verd 30 yerine 10 yapýldý */
		{
			if (strlen(debit_prefix[i][j]) == 0) break;
			memset(temp,0,30);
			sprintf(temp,"prefix[%d][%2d]= %s", i, j, debit_prefix[i][j]);
			Slips_PrinterStr(temp);
			PrntUtil_Cr(1);
		}
	}
	PrntUtil_Cr(7);
}

/*****************************************************************************/
/* GSMPOS_04_01 16/02/2001 irfan */
/* GSM POS'un register olmasýný saðlar */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_RegisterNetwork(byte )
 *
 * DESCRIPTION: 
 *
 * RETURN:      STAT_OK, STAT_NOK
 *
 * NOTES:       irfan 28/09/00
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_RegisterNetwork(byte flag,byte statFlag)
{
	Mainloop_GSM_NOSInit(flag,statFlag);
	return STAT_OK;
}
	
/*****************************************************************************/
/* Akb_01_00 30/08/2001 */
/* Voice Call Yapmasini saglar.*/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_MakeVoiceCall(void)
 *
 * DESCRIPTION: 
 *
 * RETURN:      STAT_OK, STAT_NOK
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_MakeVoiceCall(void)
{
	GSMDialPrm.CallType=GSM_CALL_VOICE;
	if ( Mainloop_GSM_Dialup() == STAT_OK ) return STAT_OK;
	return STAT_NOK;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_MakeDataCall(void)
 *
 * DESCRIPTION: 
 *
 * RETURN:      STAT_OK, STAT_NOK
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_MakeDataCall(void)
{
/*	Mainloop_RegisterNetwork(TRUE ,FALSE);*/
	ComHndlr_SetUserParams( 1 , 'T' , '\0' , 0 );
	GSMDialPrm.CallType=GSM_CALL_DATA;
	if ( Mainloop_GSM_Dialup() == STAT_OK )
	{
		Mte_Wait(1000);
		return STAT_OK;
	}
	return STAT_NOK;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_Edit.
 *
 * DESCRIPTION: 
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_Edit(char *lpWrkBuf,sint sss)
{
	byte Len=0,enterLen=0;
	char bufferDefault[25];
	switch(sss)
	{
	case 1:
		strcpy(bufferDefault,"Pin : ");
		enterLen=4;
		if (!Mainloop_GetKeyboardInput (bufferDefault, lpWrkBuf, 0, enterLen, FALSE,
						         TRUE, TRUE, FALSE, FALSE, &Len, lpWrkBuf, FALSE))
		return FALSE;
		lpWrkBuf[Len]=0;
		return TRUE;
	case 0:
		strcpy(bufferDefault,"Mesaj Yaz");
		enterLen=LEN_GETKEYBOARD;
		break;
	case 2:
		strcpy(bufferDefault,"Eski Pin1:");
		enterLen=4;
		if (!Mainloop_GetKeyboardInput (bufferDefault, lpWrkBuf, 0, enterLen, FALSE,
						         TRUE, TRUE, FALSE, FALSE, &Len, lpWrkBuf, FALSE))
		return FALSE;
		lpWrkBuf[Len]=0;
		return TRUE;
	case 3:
		strcpy(bufferDefault,"Yeni Pin1:");
		enterLen=4;
		if (!Mainloop_GetKeyboardInput (bufferDefault, lpWrkBuf, 0, enterLen, FALSE,
						         TRUE, TRUE, FALSE, FALSE, &Len, lpWrkBuf, FALSE))
		return FALSE;
		lpWrkBuf[Len]=0;
		return TRUE;
	case 4:
		strcpy(bufferDefault,"Yeni Pin1 Tekrar");
		enterLen=4;
		if (!Mainloop_GetKeyboardInput (bufferDefault, lpWrkBuf, 0, enterLen, FALSE,
						         TRUE, TRUE, FALSE, FALSE, &Len, lpWrkBuf, FALSE))
		return FALSE;
		lpWrkBuf[Len]=0;
		return TRUE;
	case 5:
		strcpy(bufferDefault,"Pin2 tekrar:");
		enterLen=4;
		if (!Mainloop_GetKeyboardInput (bufferDefault, lpWrkBuf, 0, enterLen, FALSE,
						         TRUE, TRUE, FALSE, FALSE, &Len, lpWrkBuf, FALSE))
		return FALSE;
		lpWrkBuf[Len]=0;
		return TRUE;
	case 6:
		strcpy(bufferDefault,"Serv.Tel.No:");
		enterLen=16;
		break;
	case 7:
		strcpy(bufferDefault,"SIM DE¦˜žT˜R E/H");
		lpWrkBuf[0]=MMI_ENTER;
		enterLen=1;
		break;

	}
	if (!Mainloop_GetKeyboardInput(bufferDefault, lpWrkBuf, 0, enterLen, FALSE/*TRUE*/,
				 FALSE, FALSE, FALSE, FALSE, &Len, lpWrkBuf, TRUE))
		return FALSE;
	lpWrkBuf[Len]=0;

	

	return TRUE;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_SMSSend.
 *
 * DESCRIPTION: sen or receive sms
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_GSM_SMSSend(void)
{
	sint                RetVal;           /* Return value             */
	usint               Size;             /* Size of buffer           */
	char WorkBuff[256],Buffer[LEN_GETKEYBOARD];
	byte Len;
	
	memset(Buffer,0,LEN_GETKEYBOARD);
	if(!Mainloop_GetKeyboardInput((char *)"SMS TELEFON NO ?  ",Buffer,1,15,
				FALSE, FALSE, FALSE, FALSE, FALSE, &Len,0, FALSE))
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("     TEL NO       G˜R˜ž HATASI",2);
		return(STAT_NOK);
	}
	Cpuhw_SwitchTo (_RADIO);
	Buffer[Len]=0;
	memset(GSMDialPrm.LineTelephoneNumber, 0, GSM_MAX_PHONE_NUM);

Printer_WriteStr("\nPhone Number\n");

	strcpy(GSMDialPrm.LineTelephoneNumber, Buffer);

Printer_WriteStr(GSMDialPrm.LineTelephoneNumber);

	memset(WorkBuff,0,256);
	strcpy(WorkBuff,"Akbank Deneme");
/*	Mainloop_Edit(WorkBuff,0);*/
	Size=(usint)strlen(WorkBuff);
	Debug_GrafPrint2((char *)"  SENDING ...",3);

	/* Send SMS */
	RetVal = ComHndlr_RadioSendMessage(COMHNDLR_RADIO_MSG_SMS_TXT , COMHNDLR_RADIO_ADDR_TARGET ,
											&GSMDialPrm , (byte *)WorkBuff, Size , 20000 );

Printer_WriteStr("\nSending Msg\n");
Printer_WriteStr(WorkBuff);

	switch ( RetVal )
	{
		case OK:	return STAT_OK;
		case FAIL_WRONG_INPUT:	
Printer_WriteStr("FAIL_WRONG_INPUT\n");
			break;
		case X_TIMEOUT:			
Printer_WriteStr("X_TIMEOUT\n");
			break;
		case FAILURE:			
Printer_WriteStr("FAILURE\n");
			break;
	}
	EmvIF_ClearDsp(DSP_MERCHANT);
	ComHndlr_RadioOff();
	ComHndlr_RadioOn();
	return STAT_NOK;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_NOS_SMSRead.
 *
 * DESCRIPTION: receive sms
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_GSM_NOS_SMSRead(void)
{
	sint FileHandle,Record=0;
	char status=1,result=STAT_NOK,WrkBuf[300];
	ulint Timeout;

	Cpuhw_SwitchTo (_RADIO);
	Timeout=Scan_MillSec();

	EmvIF_ClearDsp(DSP_MERCHANT);
	Debug_GrafPrint2((char *)" RECEIVING ...",3);
	memset(WrkBuf, 0, 300);

	while((Scan_MillSec() - Timeout) < 20*1000)
	{
		ComHndlr_RadioEnableRxMessages(COMHNDLR_RADIO_MSG_SMS_TXT,"SMSRCV",2,COMHNDLR_MSG_FILE_APPEND);
		Mte_Wait(200);

		if ((FileHandle=RamDisk_Reopen((byte *)"SMSRCV")->Handle) < 0) 
		{
			Printer_WriteStr("NOT SMS \n");
		}
		else
			Record=RamDisk_GetNumberOfRecords(FileHandle);
		if(Record>0)
		{
			Printer_WriteStr("NEW SMS \n");
			if(RamDisk_Seek(FileHandle,SEEK_FIRST)>0)
			{
				status=RamDisk_Read(FileHandle,&GsmMsg);
				RamDisk_ClearFile (FileHandle, 0, 0);

				memcpy(WrkBuf,GsmMsg.MsgBody,GsmMsg.SMSLen);
				WrkBuf[GsmMsg.SMSLen]=0;
				Printer_WriteStr("SMS : \n");
				PrntUtil_BufAsciiHex((byte *)WrkBuf,strlen(WrkBuf));
				result=STAT_OK;
			}
			break;
		}
		if(Kb_Read()==MMI_ESCAPE)	return STAT_INV;
	}
	return result;
}



/**************************************************************************************
 ****************************  M E N U  D E S I G N S *********************************
 **************************************************************************************/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_SwipeOrChip
 *
 * DESCRIPTION  : Displays the FALLBACK reader choice menu.
 *
 * RETURN               : 1 for magnetic swipe , 2 for chip
 *
 * NOTES                : Murat KAYIHAN - LIPMAN 
 *
 * --------------------------------------------------------------------------*/

int Mainloop_SwipeOrChip (void)
{
   entry csMENU_READER_entry [] = {
				{"MANYET˜K OKUYUCU", 0, 0, 0},
				{"CH˜P KART OKUYUCU", 0, 0, 0}};
   menu csMENU_READER_menu;
   csMENU_READER_menu.header         = "SE€˜N˜Z";
   csMENU_READER_menu.no_of_entries  = 2;
   csMENU_READER_menu.menu_entries   = &csMENU_READER_entry[0];
   csMENU_READER_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

	return(Formater_GoMenu (&csMENU_READER_menu, CHOICE_MODE));
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowFMenu
 *
 * DESCRIPTION  : Displays the F menu.
 *
 * RETURN               : none
 *
 * NOTES                : GRAPHIC_DISPLAY R.Ý.Ö. 21/01/2001 
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowFMenuOffline(void)
{
   int j;
   entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"Y™NET˜C˜ MENšSš ", 0, Menu0_Entry0   , 0},
				{"KULLANICI MENšSš", 0, Menu0_Entry1   , 0},
				{"YšKLEME MENšSš  ", 0, Menu0_Entry2   , 0},
				{"OFFLINE ˜SLEMLER", 0, Menu0_Entry5   , 0},	/* bm OFF 21.10.2004 */

#ifndef _8320
				{"GPRS    MENšSš  ", 0, Menu0_Entry3   , 0},
#endif

		        {"KAMPANYALI˜SLEMLER", 0, Menu0_Entry6, 0}
				};
   menu csMENU_SISTEM_menu;
   csMENU_SISTEM_menu.header         = "    ANA MENš    ";

#ifdef _8320
   j=0;
#else
   j= 1;
#endif

/* @bm #1 11/05/2001 KVK  str */
/* bm OPT
	if ( ( POS_Type == '0' ) && ( KAMPANYA_FLAG != 0 ) )	/* AXESS ve kampanya açýk */
	if ( ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )	 && ( KAMPANYA_FLAG != 0 ) )
	{	/* AXESS ve kampanya açýk */
		csMENU_SISTEM_menu.no_of_entries = 5 +j;
	}
	else
	{	/* AKBANK */
		csMENU_SISTEM_menu.no_of_entries = 4 +j;
	}

	/* Check for installation */
	if ( !INSTALLATION )
	{
		csMENU_SISTEM_menu.no_of_entries = 4 +j;
	}
/* @bm #1 11/05/2001 KVK  end */

   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = CYCLIC_MENU|DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

	Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);
	EmvIF_ClearDsp(DSP_MERCHANT);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowFMenu
 *
 * DESCRIPTION  : Displays the F menu.
 *
 * RETURN               : none
 *
 * NOTES                : GRAPHIC_DISPLAY R.Ý.Ö. 21/01/2001 
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowFMenu(void)
{
   int j;
   entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"Y™NET˜C˜ MENšSš ", 0, Menu0_Entry0   , 0},
				{"KULLANICI MENšSš", 0, Menu0_Entry1   , 0},
				{"YšKLEME MENšSš  ", 0, Menu0_Entry2   , 0},
				{"biz.card        ", 0, Menu0_Entry4   , 0},							
/*@bm 24.08.2004		{"OFFLINE ˜SLEMLER", 0, Menu0_Entry5   , 0},			/* 06_30 Offline islemler */
#ifndef _8320
				{"GPRS    MENšSš  ", 0, Menu0_Entry3   , 0},
#endif
		        {"KAMPANYALI˜SLEMLER", 0, Menu0_Entry6, 0}
				};
   menu csMENU_SISTEM_menu;


   csMENU_SISTEM_menu.header         = "    ANA MENš    ";
#ifdef _8320
   j=0;
#else
   j= 1;
#endif

/* @bm #1 11/05/2001 KVK  str */
/* bm OPT
	if ( ( POS_Type == '0' ) && ( KAMPANYA_FLAG != 0 ) )	/* AXESS ve kampanya açýk */
	if ( ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )	 && ( KAMPANYA_FLAG != 0 ) )
	{	/* AXESS ve kampanya açýk */
		csMENU_SISTEM_menu.no_of_entries = 5 +j;
	}
	else
	{	/* AKBANK */
		csMENU_SISTEM_menu.no_of_entries = 4 +j;
	}

	/* Check for installation */
	if ( !INSTALLATION )
	{
		csMENU_SISTEM_menu.no_of_entries = 4 +j;
	}
/* @bm #1 11/05/2001 KVK  end */


   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = CYCLIC_MENU|DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

	Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);
	EmvIF_ClearDsp(DSP_MERCHANT);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowFMenuWOBizz
 *
 * DESCRIPTION  : Displays the F menu.
 *
 * RETURN               : none
 *
 * NOTES                : GRAPHIC_DISPLAY R.Ý.Ö. 21/01/2001 
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowFMenuWOBizz(void)
{
	int j;
   entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"Y™NET˜C˜ MENšSš ", 0, Menu0_Entry0   , 0},
				{"KULLANICI MENšSš", 0, Menu0_Entry1   , 0},
				{"YšKLEME MENšSš  ", 0, Menu0_Entry2   , 0},
/*@bm 24.08.2004		{"OFFLINE ˜SLEMLER", 0, Menu0_Entry5   , 0},			/* 06_30 Offline islemler */
#ifndef _8320
				{"GPRS    MENšSš  ", 0, Menu0_Entry3   , 0},
#endif
		                {"KAMPANYALI˜SLEMLER", 0, Menu0_Entry6, 0}
				};
   menu csMENU_SISTEM_menu;


   csMENU_SISTEM_menu.header         = "    ANA MENš    ";
#ifdef _8320
   j=0;
#else
   j= 1;
#endif

/* @bm #1 11/05/2001 KVK  str */
/* bm OPT
	if ( ( POS_Type == '0' ) && ( KAMPANYA_FLAG != 0 ) )	/* AXESS ve kampanya açýk */
	if ( ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
		 && ( KAMPANYA_FLAG != 0 ) )
	{							/* AXESS ve kampanya açýk */
		csMENU_SISTEM_menu.no_of_entries = 4 +j;
	}
	else
	{							/* AKBANK */
		csMENU_SISTEM_menu.no_of_entries = 3 +j;
	}

	/* Check for installation */
	if ( !INSTALLATION )
	{
		csMENU_SISTEM_menu.no_of_entries = 3 +j;
	}
/* @bm #1 11/05/2001 KVK  end */

   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = CYCLIC_MENU|DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

   Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);

}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowFMenu items.
 *
 * DESCRIPTION  : Entry0 - Entry2  
 *
 * RETURN               : none
 *
 * NOTES                : R.Ý.Ö GRAPHIC DISPLAY 21/01/2001 
 *
 * --------------------------------------------------------------------------*/
void Menu0_Entry0 (void *dummy)
{

	EmvIF_ClearDsp(DSP_MERCHANT);

	if(!Utils_AskOperatorPassword())
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
	return;
	}
	Mainloop_ShowF1Menu();
}

void Menu0_Entry1 (void *dummy)
{
	if(!Utils_AskUserPassword())
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
	return;
	}
	Mainloop_ShowF2Menu();
}

void Menu0_Entry2 (void *dummy)
{
	if(!Utils_AskOperatorPassword())
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
	return;
	}
	Mainloop_ShowF3Menu();
}
void Menu0_Entry3 (void *dummy)
{
   entry csMENU_SISTEM_entry [] = {
				{"GSM MENU       ", 0, Menu1_Entry15, 0},
				{"GPRS INIT      ", 0, Menu1_Entry17, 0},
				{"GPRS TRAN      ", 0, Menu1_Entry18, 0},
				{"GPRS TEST      ", 0, Menu1_Entry16, 0},
				{"GPRS DNS TEST  ", 0, MenuG_Entry01, 0},
				{"GPRS/MODEM     ", 0, MenuG_Entry02, 0},
				{"GPRS MAIL      ", 0, MenuG_Entry03, 0},
				{"GPRS   xxxx    ", 0, MenuG_Entry04, 0}
			};
   menu csMENU_SISTEM_menu;


   csMENU_SISTEM_menu.header         = "GSM/GPRS MENšSš ";
   csMENU_SISTEM_menu.no_of_entries  = 8;
   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

   Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);

}


/* kampanyalý satýþ */
void Menu15_Entry1( void *dummy )
{
	byte ret_code;
	EMVTransData etdData;
	RowData rdSDAData, myrdData;
	PTStruct fpPTStruct;
	boolean IsEMV;
	ApplicationParams *pAppData;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
	EMVLanguages pelsLangs;
	byte pbErr;
	TerminalAppList fpAppList;
	char first_6_digit_local[10];	/* Check for Loyalty_Prefixes */
	
	EMVAxessStatu = FALSE; 
	EMVAxess = FALSE;
	KampanyaFlag = TRUE;
	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE; /*@bm 16.09.2004 */

	Files_ReadEMVAppParam( &fpAppList );
	memcpy( &pelsLangs, &elsLangs, sizeof( pelsLangs ) );
	tran_type = SALE;
	if ( Mainloop_Get_CardInfo
		 ( &pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex,
		   &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData, &fpPTStruct,
		   &IsEMV, FALSE ) )
	{
		memset( Received_Buffer, 0, sizeof( Received_Buffer) );
		if (!( EMVAxess && !EMVAxessStatu ))
		{
			if ( Host_PreDial(  ) != STAT_OK )
			{
				Mainloop_DisplayWait( "     ARAMA           HATASI     ", 1 );
				return;
			}
		}
		memset( Received_Buffer, 0, sizeof( Received_Buffer) );
		if ( EMVAxess && !EMVAxessStatu )
		{
			memset( fpPTStruct.Pan, 0, sizeof( fpPTStruct.Pan ) );
			EmvDb_Find( &myrdData, tagPAN );
			EmvIF_BinToAscii( myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof( fpPTStruct.Pan ), FALSE );
			while ( fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] == 'F' )
				fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] = 0;
			fpPTStruct.PanLength = strlen( fpPTStruct.Pan );
			memset( card_no, 0, 20 );
			memcpy( card_no, fpPTStruct.Pan, fpPTStruct.PanLength );
			if ( !EMVAxessGetAmount( ) )
			{
				Mainloop_DisplayWait( "EMVAxessGetAmount Hatasi", 2 );
				EmvIF_ClearDsp(DSP_MERCHANT);
				EMVAxessStatu = FALSE;
			}
			else EMVAxessStatu = TRUE;
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			return;
		}

		memcpy( first_6_digit_local, card_no, 6 );
		first_6_digit_local[6] = 0;
		if ( !CORRUPTED_CHIP_TRANSACTION_FLAG )
		{
			if ( strstr( Loyalty_Prefixes, first_6_digit_local ) == NULL )
			{
				Mainloop_DisplayWait( "SADAKAT PROGRAMIKARTLAR GECERLI ", 1 );
				return;
			}
		}

		ret_code =
			Trans_Transaction( pAppData, &fpAppList, &pMutualApps,
							   &pbMutualAppIndex, &lsTerminalDataStruct,
							   &pelsLangs, &pbErr, &rdSDAData, &etdData,
							   &fpPTStruct, IsEMV, TRUE );
		if ( Files_WriteAllParams(  ) != STAT_OK )
			Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
		switch ( ret_code )
		{
			case STAT_OK:
				break;
			case AKNET_POS_TIMEOUT_ERROR:
				Mainloop_DisplayWait( "POS TIMEOUT", 2 );
				break;
			case STAT_NOK:
				Mainloop_DisplayWait( "˜žLEM           HATALI", 2 );
				break;
			case TRR_NO_CONNECTION:
				Mainloop_DisplayWait( "  G™NDER˜LEMED˜ ", 2 );
				break;
		}
	}

	EMVAxessStatu = FALSE;
	Mte_Wait( 300 );
	Host_Disconnect(  );
	Mainloop_XLSRemoveChipCard(  );
}

void Menu15_Entry2( void *dummy )
{
	byte ret_code;
	EMVTransData etdData;
	RowData rdSDAData, myrdData;
	PTStruct fpPTStruct;
	boolean IsEMV;
	ApplicationParams *pAppData;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
	EMVLanguages pelsLangs;
	byte pbErr;
	TerminalAppList fpAppList;
	char first_6_digit_local[10];	/* Check for Loyalty_Prefixes */
	
	EMVAxessStatu = FALSE; 
	EMVAxess = FALSE;
	KampanyaFlag = TRUE;
	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE; /*@bm 16.09.2004 */

	Files_ReadEMVAppParam( &fpAppList );
	memcpy( &pelsLangs, &elsLangs, sizeof( pelsLangs ) );

	if ( RETURN_PERMISSION )
	{
		tran_type = RETURN;
		if ( Mainloop_Get_CardInfo
			 ( &pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex,
			   &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData,
			   &fpPTStruct, &IsEMV, TRUE ) )
		{
			memset( Received_Buffer, 0, sizeof( Received_Buffer) );
			if (!( EMVAxess && !EMVAxessStatu ))
			{
				if ( Host_PreDial(  ) != STAT_OK )
				{
					Mainloop_DisplayWait( "     ARAMA           HATASI     ", 1 );
					return;
				}
			}
			memset( Received_Buffer, 0, sizeof( Received_Buffer) );
			if ( EMVAxess && !EMVAxessStatu )
			{
				memset( fpPTStruct.Pan, 0, sizeof( fpPTStruct.Pan ) );
				EmvDb_Find( &myrdData, tagPAN );
				EmvIF_BinToAscii( myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof( fpPTStruct.Pan ), FALSE );
				while ( fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] == 'F' )
					fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] = 0;
				fpPTStruct.PanLength = strlen( fpPTStruct.Pan );
				memset( card_no, 0, 20 );
				memcpy( card_no, fpPTStruct.Pan, fpPTStruct.PanLength );
				if ( !EMVAxessGetAmount( ) )
				{
					Mainloop_DisplayWait( "EMVAxessGetAmount Hatasi", 2 );
					EmvIF_ClearDsp(DSP_MERCHANT);
					EMVAxessStatu = FALSE;
				}
				else EMVAxessStatu = TRUE;
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				return;
			}
			/* Check for Loyalty_Prefixes */
			memcpy( first_6_digit_local, card_no, 6 );
			first_6_digit_local[6] = 0;
			if ( !CORRUPTED_CHIP_TRANSACTION_FLAG )
			{
				if ( strstr( Loyalty_Prefixes, first_6_digit_local ) == NULL )
				{
					Mainloop_DisplayWait( "SADAKAT PROGRAMIKARTLAR GECERLI ", 1 );
					return;
				}
			}
			ret_code =
				Trans_Transaction( pAppData, &fpAppList, &pMutualApps,
								   &pbMutualAppIndex, &lsTerminalDataStruct,
								   &pelsLangs, &pbErr, &rdSDAData, &etdData,
								   &fpPTStruct, IsEMV, TRUE );
			if ( Files_WriteAllParams(  ) != STAT_OK )
				Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
			switch ( ret_code )
			{
				case STAT_OK:
					break;
				case AKNET_POS_TIMEOUT_ERROR:
					Mainloop_DisplayWait( "POS TIMEOUT", 2 );
					break;
				case STAT_NOK:
					Mainloop_DisplayWait( "      ˜ADE          HATALI      ", 2 );
					break;
				case TRR_NO_CONNECTION:
					Mainloop_DisplayWait( "  G™NDER˜LEMED˜ ", 2 );
					break;
			}
		}
		EMVAxessStatu = FALSE;
		Mte_Wait( 300 );
		Host_Disconnect(  );
	}
	else Mainloop_DisplayWait( "    ˜ADEYE          KAPALI     ", 1 );
	Mainloop_XLSRemoveChipCard(  );
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowF4Menu
 *
 * DESCRIPTION: Displays the F4 menu.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowF4Menu( void )
{
	entry csMENU_SISTEM_entry[] = {
		{"SATIž          ", 0, Menu15_Entry1, 0},
		{"˜ADE          ", 0, Menu15_Entry2, 0},
	};
	menu csMENU_SISTEM_menu;

	csMENU_SISTEM_menu.header = "KAMPANYALI ˜SLEMLER";
	csMENU_SISTEM_menu.no_of_entries = 2;
	csMENU_SISTEM_menu.menu_entries = &csMENU_SISTEM_entry[0];
	csMENU_SISTEM_menu.mode_flags = DEFAULT_MODE | MENU_TIMEOUT_30_SEC;

	Formater_GoMenu( (struct menu *)&csMENU_SISTEM_menu, 0 );
}


void Menu0_Entry4 (void *dummy)
{	/* GENIUS MENU */
	if(!Utils_AskMerchantPassword())
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		return;
	}
	Mainloop_ShowF6Menu();
}
void Menu0_Entry6( void *dummy )
{
	if ( KAMPANYA_FLAG == 1 )
	{
		strcpy( kampanya_str, "IMEI NO" );
		Mainloop_ShowF4Menu(  );
	}
	else if ( KAMPANYA_FLAG == 2 )
	{
		strcpy( kampanya_str, "šRšN NO" );
		Mainloop_ShowF4Menu(  );
	}
	else
	{
		KAMPANYA_FLAG = 0;
		memset( kampanya_str, 0, sizeof(kampanya_str) );
	}
}

void MenuG_Entry01 (void *dummy)
{
	
 	GPRS_TestDNS();
 }	  
void MenuG_Entry02 (void *dummy)
{
	
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];

	memset(value_null, 0 , sizeof(value_null));

	/*              GPRS/DIALUP MENU                      */
	/*----------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"GPRS/MOD>0:G 1:D",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE))
	{
		Mainloop_DisplayWait("  GPRS / MODEM    G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '0')
	{
		USE_GPRS = TRUE;
	}
	else
	{
		USE_GPRS = FALSE;
	}

 }	  

void MenuG_Entry03 (void *dummy)
{
	SOCKET sd;
	GPRS_Init(TRUE);
	Printer_WriteStr("\n213.161.155.182");	
	Printer_WriteStr("\nTrying POP3(110)\n");	
	if(GPRS_OpenConnection(&sd,"213.161.155.182",(usint)110)==STAT_OK){
		Printer_WriteStr("\nConnection ok\n");
		GPRS_CloseConnection(sd);	
	}else
		Printer_WriteStr("\nConnection NOK\n");	
		
	Printer_WriteStr("\nTrying SMTP(25)\n");	
	if(GPRS_OpenConnection(&sd,"213.161.155.182",(usint)25)==STAT_OK){
		Printer_WriteStr("\nConnection ok\n");	
		GPRS_CloseConnection(sd);	
	}else
		Printer_WriteStr("\nConnection NOK\n");	

	Printer_WriteStr("\n217.169.192.19");	
	Printer_WriteStr("\nTrying POP3(110)\n");	
	if(GPRS_OpenConnection(&sd,"217.169.192.19",(usint)110)==STAT_OK){
		Printer_WriteStr("\nConnection ok\n");	
		GPRS_CloseConnection(sd);	
	}else
		Printer_WriteStr("\nConnection NOK\n");	
		
	Printer_WriteStr("\nTrying SMTP(25)\n");	
	if(GPRS_OpenConnection(&sd,"217.169.192.19",(usint)25)==STAT_OK){
		Printer_WriteStr("\nConnection ok\n");	
		GPRS_CloseConnection(sd);	
	}else
		Printer_WriteStr("\nConnection NOK\n");	
	
}
typedef struct {
	char a[6];
	char b[3][4];
}  testmatrix;
#define LEN_testmatrix  (sizeof(testmatrix))

void MenuG_Entry04 (void *dummy)
{
char fpWrkBuf[20];
memset(fpWrkBuf,0,20);
fpWrkBuf[0] = '˜';
fpWrkBuf[1] = '™'; 
fpWrkBuf[2] = '¦'; 
fpWrkBuf[3] = 'ž'; 
fpWrkBuf[4] = '€'; 
fpWrkBuf[5] = 'š'; 
fpWrkBuf[6] = '§'; 
fpWrkBuf[7] = ''; 
fpWrkBuf[8] = ''; 
fpWrkBuf[9] = '‡'; 
fpWrkBuf[10] = '”'; 
fpWrkBuf[11] = 'Ÿ';
fpWrkBuf[12] = 'Ÿ';

Printer_WriteStr("\n\nTurkish characters\n\n");
Printer_WriteStr(fpWrkBuf);
PrntUtil_Cr(2);
PrntUtil_BufAsciiHex((byte*)fpWrkBuf,13);
	
    GrphText_ClrLine (3, TRUE);
    GrphText_Write ((byte) 3, (sint) 15, (char *) fpWrkBuf,
                    (byte) FONT_13X7, (byte) strlen(fpWrkBuf),
                    TRUE, FALSE);
	Mte_Wait(5000);
	
    GrphText_ClrLine (3, TRUE);
    GrphText_Write ((byte) 3, (sint) 15, (char *) fpWrkBuf,
                    (byte) FONT_10X7, (byte) strlen(fpWrkBuf),
                    TRUE, FALSE);
	Mte_Wait(5000);
	
    GrphText_ClrLine (3, TRUE);
    GrphText_Write ((byte) 3, (sint) 15, (char *) fpWrkBuf,
                    (byte) FONT_7X5, (byte) strlen(fpWrkBuf),
                    TRUE, FALSE);
	Mte_Wait(5000);
	
 
}	  


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowF1Menu
 *
 * DESCRIPTION: Displays the F1 menu.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowF1Menu(void)
{
   entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"KURULUž        ", 0, Menu1_Entry1   , 0},
				{"K.ž˜FRE SIFIRLA", 0, Menu1_Entry2   , 0},
				{"PARAMETRE L˜STE", 0, Menu1_Entry3   , 0},
				{"DEB˜T PREF˜X L.", 0, Menu1_Entry4   , 0},
				{"ATS&SADAKAT PRE", 0, Menu1_Entry5   , 0},
				{"˜ADE KONTROL.  ", 0, Menu1_Entry6   , 0},     /*UPDATE_01_14 IADESIZ 20/10/99 */
				{"EL ˜LE G˜R˜ž K.", 0, Menu1_Entry7   , 0},     /*UPDATE_01_18 EL ÝLE KART GÝRÝÞ KONTROLÜ 14/01/00 */
				{"MANUEL ˜žLEM K.", 0, Menu1_Entry8   , 0},     /*UPDATE_01_18 MANUEL ÝÞLEM KONTROLÜ 21/06/00 */
				{"DEBUG MODE KON.", 0, Menu1_Entry9   , 0},     /*UPDATE_01_18 MANUEL ÝÞLEM KONTROLÜ 21/06/00 */
				{"CALIžMA KOSULLA", 0, Menu1_Entry10  , 0},      /* v04_05 CALISMA_KOSULLARI_UPDATE 16/07/02 */
				{"VERDEPOS SECIMI", 0, Menu1_Entry11  , 0},      /* VERDE 04/11/2002 */
				{"EXTERNAL PINPAD", 0, Menu1_Entry12  , 0},      /* VERDE 07/11/2002 */
				{"PINPAD KEY YUK.", 0, Menu1_Entry13  , 0},       /* VERDE 07/11/2002 */
                {"TVR TSI DEBUG  ", 0, Menu1_Entry14  , 0},
				{"NOS KILIT      ", 0, Menu1_Entry22  , 0},       /* VERDE 07/11/2002 */
/*				{"BAUDRATE SECIMI", 0, Menu1_Entrya16, 0},
				{"TELEFONNO GIRIS", 0, Menu1_Entrya17, 0},*/
				{"KAMPANYA KONTROL", 0, Menu1_Entry24, 0}
/*				{"YTL SWITCH     ", 0, Menu1_Entry23, 0}
				{"PINPAD RMDSKEYS", 0, Menu1_Entry19  , 0},
				{"REVERSAL FALSE ", 0, Menu1_Entry20  , 0},
				{"RAM DISK ERASE ", 0, Menu1_Entry21  , 0}
*/
/*
#ifndef _8320
				,{"GSM MENU       ", 0, Menu1_Entry15, 0},
				{"GPRS INIT      ", 0, Menu1_Entry17, 0},
				{"GPRS TRAN      ", 0, Menu1_Entry18, 0},
				{"GPRS TEST      ", 0, Menu1_Entry16, 0}
#endif		
*/				
			  /*{"MASTER KEY YšK.", 0, Menu1_Entry14   , 0}*/
			};
   menu csMENU_SISTEM_menu;


   csMENU_SISTEM_menu.header         = "Y™NET˜C˜ MENšSš ";
/*#ifdef _8320
   csMENU_SISTEM_menu.no_of_entries  = 17;
#else
   csMENU_SISTEM_menu.no_of_entries  = 22;
#endif
*/
	/* @bm #1 13.05.2004 KVK */
	if ( !INSTALLATION )
	{
		csMENU_SISTEM_menu.no_of_entries = 15;
	}
	else
	{
/*bm OPT if ( POS_Type == '0' )	/* xls */
		if ( ( POS_Type == '2' ) || ( POS_Type == '4' ) || ( POS_Type == '6' ) )	/* xls */
			csMENU_SISTEM_menu.no_of_entries = 16;
		else
			csMENU_SISTEM_menu.no_of_entries = 15;
	}


   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = CYCLIC_MENU|DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

   Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);

}

void Menu1_Entrya16( void *dummy )
{
	Trans_SetBaudRate();
}

void Menu1_Entrya17( void *dummy )
{
	Trans_EnterPhoneNum();
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu1_Entry1
 *
 * DESCRIPTION:
 *
 * RETURN: none
 *
 * NOTES:       13/07/99        r.i.ö. && t.c.a
 *
 * --------------------------------------------------------------------------*/
void Menu1_Entry1 (void *dummy)
{
boolean ReversalPendingTemp;

	ReversalPendingTemp=ReversalPending;
	 
	/* Kuruluþ */
		if(record_counter != 0)
		{
		EmvIF_ClearDsp(DSP_MERCHANT);
		if(Mainloop_DisplayWait("   YI¦IN DOLU   DEV:G˜R  KES:€IK",10) != MMI_ENTER)
			return;
		}
		switch (Trans_Installation())
		{

		case    STAT_OK:
		/*Mainloop_GSMStatistInit();*/

		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("    KURULUž        TAMAMLANDI   ",1);

		FirstRun=FALSE;

			break;
		case    AKNET_POS_TIMEOUT_ERROR:        /* UPDATE 01_18 02/12/99 TIMEOUT_ERROR idi.Ancak bunun kullanýlmamasý gerekir*/
		EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("POS TIMEOUT",2);
			break;

		case    STAT_NOK:
		EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("KURULUž HATALI",2);
			break;
		case TRR_NO_CONNECTION:
		EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
			break;
		}
							
		Host_Disconnect();      
	Mte_Wait(50);

	/*Kullanýcý þifresi sýfýrla*/	
	if(Files_ReadAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER OKUNAMADI",1);
	strcpy(UserPassword, "4321");
	if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
	ReversalPending=ReversalPendingTemp;

}

void Menu1_Entry2 (void *dummy)
{

	/* Kullanýcý Þifresi Resetleme */
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("   KULLANICI    ž˜FRES˜ SIFIRLA ",2);

	strcpy(UserPassword, "4321");

	/*save parameters files */
	/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
	eger yazamas ise idle loop'a donuyor */
	if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);

			

}

void Menu1_Entry3 (void *dummy)
{
   /* Parametre Listeleme */
	
	/*Mainloop_PrintLogo(); /* test amacli 26/01/2001 */
	Display_ClrDsp();
	test_statistics();      
	Host_TestPrint();
        if(genius_flag=='0'){
	/* genius bilgileri */
	Slips_PrinterStr("\n\n--- GENIUS BILGILERI ---\n");
	Slips_PrinterStr("\nlast_genius_eod_date:\n");
	Slips_PrinterStr(last_genius_eod_date);
	Slips_PrinterStr("\n\n");
        }

	if(GSMCALL)
	{
		char temp[250];
		memset(temp,0,250);

		Slips_PrinterStr("GSM comm.parameters\n");
		sprintf(temp, "\nSWITCHED TORADIO:%03d\nFAIL CONNECT    :%03d\nFAIL WRONG INPUT:%03d\nUSER BREAK      :%03d\nxDIAL UP PRESSED:%03d\n\n",
			stGSMSTATIST.siSWITCHED_TO_RADIO,
			stGSMSTATIST.siFAIL_CONNECT,
			stGSMSTATIST.siFAIL_WRONG_INPUT,
			stGSMSTATIST.siUSER_BREAK,
			stGSMSTATIST.siX_DIAL_UP_PRESSED);
		temp[strlen(temp)]=0;
		Slips_PrinterStr(temp);

		memset(temp,0,250);
		sprintf(temp, "NO NETWORK      :%03d\nSIM ERROR       :%03d\nPIN1 REQUIRED   :%03d\nPUK1 REQUIRED   :%03d\nPIN2 REQUIRED   :%03d\nPUK2 REQUIRED   :%03d\n",
			stGSMSTATIST.siNO_NETWORK,
			stGSMSTATIST.siSIM_ERROR,
			stGSMSTATIST.siPIN1_REQUIRED,
			stGSMSTATIST.siPUK1_REQUIRED,
			stGSMSTATIST.siPIN2_REQUIRED,
			stGSMSTATIST.siPUK2_REQUIRED);
		Slips_PrinterStr(temp);
		memset(temp,0,250);
		sprintf(temp, "MODEM FATAL ERR :%03d\nSMS SEND        :%03d\nSMS RECEIVE     :%03d\nSMS SEND ERR    :%03d\nSMS RECEIVE ERR :%03d\n\n\n",
			stGSMSTATIST.siMODEM_FATAL_ERROR,
			stGSMSTATIST.siSMS_SEND_COUNTER,
			stGSMSTATIST.siSMS_RECEIVE_COUNTER,
			stGSMSTATIST.siSMS_SEND_ERROR,
			stGSMSTATIST.siSMS_RECEIVE_ERROR);
		Slips_PrinterStr(temp);
		Printer_WriteStr("\n\n\n");
	}
	Offline_PrintOfflineParameters();
}

void Menu1_Entry4 (void *dummy)
{
   /* Debit Prefix Listeleme */
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_PrintDebitPrefix();

/* bm OFF 21.10.2004 */
	Kb_Read(  );
	Display_ClrDsp(  );
	Files_Print_Offline_BIN(  );

}

void Menu1_Entry5 (void *dummy)
{
   /* ATS Prefix Listeleme */
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_PrintATSPrefix();

}

/*UPDATE_01_14 IADESIZ 20/10/99 */
void Menu1_Entry6 (void *dummy)
{

	char Buffer[LEN_GETKEYBOARD];
	byte Len;

	
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));


	
	/* IADE KONTROL */


	/*              IADE KONTROL              */
	/*----------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"IADE K. >1:A 0:K",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" IADE KONTROL      G˜R˜ž HATASI ",2);
		return;
	}

	if(Buffer[0] == '0')
	{
		RETURN_PERMISSION = FALSE;             /* IADE KAPALI */
	}
	else
	{
		RETURN_PERMISSION = TRUE;             /* IADE ACIK */
	}
}


/*UPDATE_01_18 EL ile kart giriþi kontrolü 14/01/00 */
void Menu1_Entry7 (void *dummy)
{

	char Buffer[LEN_GETKEYBOARD];
	byte Len;

	
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));
	
	
	/* EL ÝLE KART GÝRÝÞ KONTROL */


	/*              /* EL ÝLE KART GÝRÝÞ KONTROL            */
	/*--------------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"SIF.SOR. E>1 H>0",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '0')
	{
		MANUEL_ENTRY_PERMISSION = FALSE;             /* EL ÝLE GÝRÝÞTE ÞÝFRE SORMA */
	}
	else
	{
		MANUEL_ENTRY_PERMISSION = TRUE;             /* EL ÝLE GÝRÝÞTE ÞÝFRE SOR */
	}
}


void Menu1_Entry8 (void *dummy)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;

	
	/* 1_18 21/06/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* 1_18 21/06/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));


	
	/* MANUEL ÝÞLEM KONTROL */


	/*              MANUEL ÝÞLEM KONTROL              */
	/*----------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"MANUEL K>1:A 0:K",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MANUEL ˜žLEM KON.  G˜R˜ž HATASI ",2);
		return;
	}

	if(Buffer[0] == '0')
	{
		MANUEL_PERMISSION = FALSE;             /* MANUEL ÝÞLEM KAPALI */
	}
	else
	{
		MANUEL_PERMISSION = TRUE;             /* MANUEL ÝÞLEM ACIK */
	}
}

void Menu1_Entry9 (void *dummy)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];

// bm gec 
//	Mainloop_DisplayWait("DEBUG MODE      KULLANILAMAZ",WAIT_WARNING);
//	return;

	memset(value_null, 0 , sizeof(value_null));

	if(!Mainloop_GetKeyboardInput((char *)"ALLDEBUG 0:A 1:K",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE))
	{
		Mainloop_DisplayWait(" FULLDEBUGMODE    G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '0')
	{
		EMV_DEBUG_MODE = TRUE;
		DEBUG_MODE = TRUE;
		PRINTALLTAGVALUES = TRUE;
/*		lsTerminalDataStruct.LipmanKernelOpt[7] |= opt_8_PRINT_TRANSPORT;*/
		Files_WriteGenParams();
	}
	else
	{
		EMV_DEBUG_MODE = FALSE;
		DEBUG_MODE = FALSE;          
		PRINTALLTAGVALUES = FALSE;
		lsTerminalDataStruct.LipmanKernelOpt[7] &= 0xFE;
		Files_WriteGenParams();          
	}
}

/* v04_05 CALISMA_KOSULLARI_UPDATE 16/07/2002 */
void Menu1_Entry10 (void *dummy)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];

	memset(value_null, 0 , sizeof(value_null));

	/*            CALISMA KOSULLARI KONTROLU        */
	/*----------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"ACIK>1  KAPALI>0",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE))
	{
		Mainloop_DisplayWait("  G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '0')
	{
		CALISMA_KOSULLARI_MODE = FALSE;
	}
	else
	{
		CALISMA_KOSULLARI_MODE = TRUE;          
	}

}

/* VERDE 04/11/2002 */
void Menu1_Entry11 (void *dummy)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_current[2];
	

	if (VERDE_TRANSACTION)
		value_current[0] = '1';
	else
		value_current[0] = '0';

	value_current[1] = 0;


	/*            VERDE POS SEÇÝMÝ KONTROLU        */
	/*----------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"EVET>1  HAYIR>0",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_current, TRUE))
	{
		Mainloop_DisplayWait("  G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '1')
	{
		VERDE_TRANSACTION = TRUE;
	}
	else
	{
		VERDE_TRANSACTION = FALSE;          
	}

	Files_WriteAllParams();
}

/* VERDE 07/11/2002 */
/* External Pinpad Kullanýmý SEçilmesi */
void Menu1_Entry12 (void *dummy)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_current[2];
	

	if (EXTERNAL_PINPAD)
		value_current[0] = '1';
	else
		value_current[0] = '0';

	value_current[1] = 0;


	/*            EXTERNAL_PINPAD SEÇÝMÝ KONTROLU        */
	/*----------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"EVET>1  HAYIR>0",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_current, TRUE))
	{
		Mainloop_DisplayWait("  G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '1')
	{
		EXTERNAL_PINPAD = TRUE;
	}
	else
	{
		EXTERNAL_PINPAD = FALSE;          
	}

	Files_WriteAllParams();
}

/* VERDE 07/11/2002 */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME:	Menu1_Entry12
 *
 * DESCRIPTION:		LOAD MASTER KEY FROM PC.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Menu1_Entry13(void *dummy)
{
	/*Debug_SetFont(FONT_10X7);*/
	/*Utils_ClearDsp();
	Utils_DisplayWait(" Key Loading.. ",0);*/

	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait(" Key Loading.. ",NO_WAIT);
	
	if(Security_TransferMasterKey() != 0)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("Missed Key.... ",1);
	}
}

void Menu1_Entry19(void *dummy)
{
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait(" Printing Keys",WAIT_WARNING);
	
	MKVP_Eeprom_Print(); 
}
void Menu1_Entry20(void *dummy)
{
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait(" Reversal Pending False",WAIT_WARNING);
	ReversalPending = FALSE;	
}

void Menu1_Entry21(void *dummy)
{
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait(" Erasing RAMDISK",WAIT_WARNING);
	if(RamDisk_Reset()==RD_OK) Printer_WriteStr("RamDisk Reset ok\n"); 
}

void Menu1_Entry22 (void *dummy)
{
	if(Utils_AskNOSPassword())
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Kb_NosEnable(); 
		Mainloop_DisplayWait("  NOS ACIK  ",WAIT_WARNING);
		EmvIF_ClearDsp(DSP_MERCHANT);
	}
	Kb_NosDisable(); 
}

/*void Menu1_Entry14(void *dummy)
{
	/* Master Key yükleme */

/*	Mainloop_DisplayWait("   MASTER KEY       YšKLEME     ",NO_WAIT);
	if(Utils_PinPadInitial() != STAT_OK)
	{
		Mainloop_DisplayWait("MASTER KEY YšK.    BAžARISIZ    ",2);
	}
	else
	{
		Mainloop_DisplayWait("MASTER KEY YšK.     BAžARILI    ",2);
	}

}
*/

void Menu1_Entry14(void *dummy)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];

	memset(value_null, 0 , sizeof(value_null));

	if(!Mainloop_GetKeyboardInput((char *)"TVR TSI  0:A 1:K",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE))
	{
		Mainloop_DisplayWait("     TVR TSI      G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '0') TVR_TSI = TRUE;
	else TVR_TSI = FALSE;
}

void Menu1_Entry15 (void *dummy)
{
	Mainloop_GSMMenu();
	Host_ConnectStatist(3);
}

void Menu1_Entry16 (void *dummy)
{/*GPRS TEST */
	char glb_host_ip[16];
	char GprsBuffer[30];
	strcpy(glb_host_ip, "216.147.156.77");	
	strcpy(APN, "gprstest");	
	strcpy(ISP_NAME, "turkcell");
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("Init GPRS",0);
	Printer_WriteStr("Init GPRS\n");
	PrntUtil_Cr(2);
/*	if(!Utils_GetEditScreen(33,0, APN,24))return;
	if(!Utils_GetEditScreen(34,0, ISP_NAME,24))return;
	if(!Utils_GetEditScreen(35,0, glb_host_ip,24))return;
*/
	/*char host_ip[IPLEN];*/

/*	if(GPRS_Set(glb_host_ip)==STAT_OK)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("GPRS SETOK",1);
		Printer_WriteStr("GPRS Set OK\n");
		PrntUtil_Cr(2);

*/		byte buffer[255];
		SOCKET sd;
		usint BufferSize;
			
		memset(buffer,0,sizeof(buffer));
		memcpy(buffer, "LIPMAN GPRS TEST",16);
		buffer[0]=0x60;
		buffer[1]=0x00;
		buffer[2]=0x01;
		buffer[3]=0x80;
		buffer[4]=0x00;
		buffer[5]=0x04;
		buffer[6]=0x00;
		buffer[7]=0x30;
		buffer[8]=0x20;
		buffer[9]=0x05;
		buffer[10]=0x80;
		buffer[11]=0x20;
		buffer[12]=0xC0;
		buffer[13]=0x00;
		buffer[14]=0x04;
		buffer[15]=0x00;
		buffer[16]=0x30;
		buffer[17]=0x00;
		buffer[18]=0x00;
		buffer[19]=0x00;
		buffer[20]=0x00;
		buffer[21]=0x00;
		buffer[22]=0x00;
		buffer[23]=0x05;
		buffer[24]=0x00;
		buffer[25]=0x01;
		buffer[26]=0x02;
		buffer[27]=0x00;
		buffer[28]=0x22;
		buffer[29]=0x02;
		buffer[30]=0x30;
		buffer[31]=0x00;
		buffer[32]=0x37;
		buffer[33]=0x45;
		buffer[34]=0x56;
		buffer[35]=0x33;
		buffer[36]=0x33;
		buffer[37]=0x44;
		buffer[38]=0x44;
		buffer[39]=0x88;
		buffer[40]=0x11;
		buffer[41]=0xD0;
		buffer[42]=0x30;
		buffer[43]=0x41;
		buffer[44]=0x01;
		buffer[45]=0x17;
		buffer[46]=0x52;
		buffer[47]=0x99;
		buffer[48]=0x55;
		buffer[49]=0x00;
		buffer[50]=0x00;
		buffer[51]=0x0F;
		buffer[52]=0x30;
		buffer[53]=0x30;
		buffer[54]=0x32;
		buffer[55]=0x39;
		buffer[56]=0x32;
		buffer[57]=0x31;
		buffer[58]=0x37;
		buffer[59]=0x32;
		buffer[60]=0x30;
		buffer[61]=0x30;
		buffer[62]=0x30;
		buffer[63]=0x30;
		buffer[64]=0x30;
		buffer[65]=0x30;
		buffer[66]=0x35;
		buffer[67]=0x34;
		buffer[68]=0x38;
		buffer[69]=0x36;
		buffer[70]=0x39;
		buffer[71]=0x34;
		buffer[72]=0x30;
		buffer[73]=0x30;
		buffer[74]=0x30;
		buffer[75]=0x00;
		buffer[76]=0x06;
		buffer[77]=0x30;
		buffer[78]=0x30;
		buffer[79]=0x30;
		buffer[80]=0x30;
		buffer[81]=0x31;
		buffer[82]=0x32;
	
		BufferSize=83;
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("GPRS SENDING",1);
		if(GPRS_TCPAPIhost_Send(&sd, glb_host_ip, buffer, BufferSize)==STAT_OK)
		{
			Printer_WriteStr("GPRS Send OK\n");
			PrntUtil_Cr(2);
			memset(buffer, 0, sizeof(buffer));
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("GPRS Receiving",1);
			if(GPRS_TCPAPIhost_Receive(sd, glb_host_ip, (char *)buffer, &BufferSize))
			{

				Printer_WriteStr("GPRS receive OK\n");
				PrntUtil_Cr(2);
				PrntUtil_BufAsciiHex(buffer,BufferSize);
			}
			
		}
	/*}*/
} 
void Menu1_Entry17 (void *dummy)
{/*GPRS INIT */
	GPRS_Init(TRUE);
}	  
void Menu1_Entry18 (void *dummy)
{/*GPRS tran */
	byte buffer2[512];
	usint BufferSize2;
	sint BufferSize;
	
	SOCKET sd;

	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("OPENING HOST    CONNECTION",1);
   	if(GPRS_OpenConnection(&sd, "216.147.156.77",127) != STAT_OK) {
   		if(GPRS_Init(TRUE) != STAT_OK) 
			Mainloop_DisplayWait("OPEN FAILED     CONNECTION",1);
		Mainloop_DisplayWait("STARTING INIT",1);
   		if(GPRS_OpenConnection(&sd, "216.147.156.77",127) != STAT_OK){
			Mainloop_DisplayWait("INIT FAILED",1);
			exit;
		}
   	}
	EmvIF_ClearDsp(DSP_MERCHANT);
        byte buffer[]={0x00,0x60,0x00,0x01,0x80,0x00,0x04,0x00,0x30,0x20,0x05,
		0x80,0x20,0xC0,0x00,0x04,0x00,0x30,0x00,0x00,0x00,
		0x00,0x00,0x00,0x05,0x00,0x01,0x02,0x00,0x22,0x02,
		0x30,0x00,0x37,0x45,0x56,0x33,0x33,0x44,0x44,0x88,
		0x11,0xD0,0x30,0x41,0x01,0x17,0x52,0x99,0x55,0x00,
		0x00,0x0F,0x30,0x30,0x32,0x39,0x32,0x31,0x37,0x32,
		0x30,0x30,0x30,0x30,0x30,0x30,0x35,0x34,0x38,0x36,
		0x39,0x34,0x30,0x30,0x30,0x00,0x06,0x30,0x30,0x30,
		0x30,0x31,0x32};
	BufferSize=84;


	Mainloop_DisplayWait("SENDING         GPRSDATA",1);
   	if(GPRS_Send(sd,buffer,BufferSize)!= STAT_OK) {
   		Mainloop_DisplayWait("SEND FAILED     GPRSDATA",1);
   		exit;
   	}
   	BufferSize2=512;
	memset(buffer2,0,sizeof(buffer2));
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("RECEIVING       GPRSDATA",1);
	if(GPRS_Receive(sd,buffer2,&BufferSize2)!= STAT_OK)
			Mainloop_DisplayWait("RECEIVE FAILED  GPRSDATA",1);
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("CLOSING HOST    CONNECTION",1);
	
	GPRS_CloseConnection(sd);
	EmvIF_ClearDsp(DSP_MERCHANT);
}
void Menu1_Entry23( void *dummy )
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];

	memset( value_null, 0, sizeof( value_null ) );

	if ( !Mainloop_GetKeyboardInput
		 ( ( char * ) "YTL 0:A 1:K 2:NL", Buffer, 1, 1, FALSE, FALSE, FALSE,
		   FALSE, FALSE, &Len, value_null, TRUE ) )
	{
		Mainloop_DisplayWait( " G˜R˜ž HATASI  ", 2 );
		return;
	}

	Printer_WriteStr( "YTLFlag:\n" );
	PrntUtil_BufAsciiHex( (byte*)YTL_Flag, sizeof(YTL_Flag) );
	Printer_WriteStr( "POS_Type:\n" );
	PrntUtil_BufAsciiHex((byte*) &POS_Type, 1 );
	Printer_WriteStr( "\n\n\n" );

	if ( Buffer[0] == '0' )
	{
		memcpy( YTL_Flag, "YTL", 3 );
		switch(POS_Type)
		{
		case '3':
			POS_Type = '5';	/*Akbank new YTL */
			memcpy( YTL_Flag, "YTL", 3 );	/* bm YTL 04.08.2004 WRT*/
			break;
		case '4':
			POS_Type = '6';	/*Axces new YTL */
			memcpy( YTL_Flag, "YTL", 3 );  	/* bm YTL 04.08.2004 WRT*/
			break;
		default:
			memcpy( YTL_Flag, "YTL", 3 );

		}
	}
	else if ( Buffer[0] == '1' )
	{
		memcpy( YTL_Flag, "TLE", 3 );
		switch(POS_Type)
		{
		case '5':		
			POS_Type = '3';	/*Akbank TL */
			memcpy( YTL_Flag, "TLE", 3 );  	/* bm YTL 04.08.2004 WRT*/
			break;
		case '6':
			POS_Type = '4';	/*Axces TL */
			memcpy( YTL_Flag, "TLE", 3 );  	/* bm YTL 04.08.2004 WRT*/
			break;
		default:
			memcpy( YTL_Flag, "TLE", 3 );
		}
	}
	else memset( YTL_Flag, 0, 3 );
	Files_WriteAllParams(  );
	
	Printer_WriteStr( "YTLFlag Set:\n" );
	PrntUtil_BufAsciiHex( (byte*)YTL_Flag, sizeof(YTL_Flag) );
	Printer_WriteStr( "POS_Type Set:\n" );
	PrntUtil_BufAsciiHex( (byte*)&POS_Type, 1 );
	Printer_WriteStr( "\n\n\n" );
}

void Menu1_Entry24( void *dummy )
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];
	char temp_statu[35];

	memset( value_null, 0, sizeof( value_null ) );

	/* @bm #2 11.05.2004 KVK */
	memset( temp_statu, 0, sizeof( temp_statu ) );
	sprintf( temp_statu, "KAPALI>0 IMEI>1 šRšN>2 [%d]", KAMPANYA_FLAG );
	temp_statu[strlen( temp_statu )] = 0;

	for ( ;; )
	{
		if ( !Mainloop_GetKeyboardInput
			 ( ( char * ) temp_statu, Buffer, 1, 1, FALSE, FALSE, FALSE,
			   FALSE, FALSE, &Len, value_null, TRUE ) )
		{
			Mainloop_DisplayWait( "KAMPANYA KONTROLG˜R˜ž HATASI", 2 );
			return;
		}
		if ( ( Buffer[0] == '0' ) || ( Buffer[0] == '1' ) ||
			 ( Buffer[0] == '2' ) )
		{
			break;
		}
		else
		{
			GrphText_Cls( TRUE );
			Mainloop_DisplayWait( "    0,1,2         G˜R˜LEB˜L˜R ", 1 );
		}
	}

	if ( Buffer[0] == '1' )
		KAMPANYA_FLAG = 1;
	else if ( Buffer[0] == '2' )
		KAMPANYA_FLAG = 2;
	else
		KAMPANYA_FLAG = 0;
	if ( Files_WriteAllParams(  ) != STAT_OK )
		Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
}




/* VARAN ICIN ARA RAPOR BASIMI OPTIONAL HALE GETIRILDI 10/10/2002 */
void Menu2_Entry6 (void *dummy)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];

	memset(value_null, 0 , sizeof(value_null));

	/*            ARA RAPOR BASILMASI KONTROLU       */
	/*------------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"EVET>1   HAYIR>0",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE))
	{
		Mainloop_DisplayWait("  G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '0')
	{
		ARA_RAPOR_IPTAL = FALSE;
	}
	else
	{
		ARA_RAPOR_IPTAL = TRUE;          
	}

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowF2Menu
 *
 * DESCRIPTION: Displays the F2 menu.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowF2Menu(void)
{
   entry csMENU_SISTEM_entry [] = {
		/*1234567890123456*/
		{"K.ž˜FRE DE¦˜žT˜R", 0, Menu2_Entry1   , 0},
/*		{"TUžLAMA SES˜    ", 0, Menu2_Entry2   , 0},
		{"UYARI SES˜      ", 0, Menu2_Entry3   , 0},
		{"˜S˜M YAZMA KONT.", 0, Menu2_Entry4   , 0},
		{"PINPAD TAKS˜T G.", 0, Menu2_Entry5   , 0},     /* 02_01 08/09/2000 Pinpad den taksit giriþi irfan*/
		{"ARA RAPOR IPTAL",  0, Menu2_Entry6   , 0},      /* VARAN ICIN ARA RAPOR BASIMI OPTIONAL YAPILDI 10/10/2002*/
		{"TELEFONNO GIRIS", 0, Menu1_Entrya17, 0},
		{"KSYR.ID TANIT", 0, Menu2_Entry3, 0}	  /* @bm 13.08.2004*/
		};
   menu csMENU_SISTEM_menu;

	if (Cashier_Flag == 1)  /*bm YTL 13.08.2004 */
		csMENU_SISTEM_menu.no_of_entries  = 4; /* kasiyer ID tanýtma açýk*/
   	else 
		csMENU_SISTEM_menu.no_of_entries  = 3; /*kapalý*/

   csMENU_SISTEM_menu.header         = "KULLANICI MENš ";
   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

   Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);

}




/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu2_Entry1
 *
 * DESCRIPTION  :  Kullanýcý Þifresi Deðiþtirme
 *
 * RETURN: none
 *
 * NOTES:       14/07/99        r.i.ö. && t.c.a
 *
 * --------------------------------------------------------------------------*/
void Menu2_Entry1 (void *dummy)
{

	/* Kullanýcý Þifresi Deðiþtirme */              

	Utils_ChangeUserPassword();

	/*read parameters files */
	/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
	eger yazamas ise idle loop'a donuyor */
	if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);


	
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu2_Entry3
 *
 * DESCRIPTION  :  Kasiyer No tanýtma
 *
 * RETURN: none
 *
 * NOTES:       13.08.2004 @bm 
 *
 * --------------------------------------------------------------------------*/
void Menu2_Entry3( void *dummy )
{

	boolean	Status;

	Status =  Mainloop_GetKasierCardData();

	if (Status)
		Trans_Get_KasierID();
	else
		Mainloop_DisplayWait( "KASIER  G˜R˜ž HATASI  ", 2 );
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowF3Menu
 *
 * DESCRIPTION: Displays the F3 menu.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowF3Menu(void)
{
   entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"PROGRAM G™NDER ", 0, Menu3_Entry1   , 0},
		{"PROGRAM AL     ", 0, Menu3_Entry2, 0}
				};
   menu csMENU_SISTEM_menu;


   csMENU_SISTEM_menu.header         = "TRANSFER MENšSš>";
	csMENU_SISTEM_menu.no_of_entries = 2;
   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

   Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu3_Entry1
 *
 * DESCRIPTION  : PROGRAM GÖNDERME
 *
 * RETURN: none
 *
 * NOTES:       25/07/99        r.i.ö. && t.c.a
 *
 * --------------------------------------------------------------------------*/
void Menu3_Entry1 (void *dummy)
{

	/* PROGRAM GÖDNERME */              

	/*Appload_Copy(APPLOAD_OUT,NULL);*/            
	/*Appload_Copy(0,NULL);                        /* MESAJLAR ÝNGÝLÝZCE   */
	Appload_Copy(0,&AploadMessages[0][0]);      /* MESAJLAR TÜRKÇE         */     

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu3_Entry2
 *
 * DESCRIPTION  : PROGRAM ALMA
 *
 * RETURN: none
 *
 * NOTES:       25/07/99        r.i.ö. && t.c.a
 *
 * --------------------------------------------------------------------------*/
void Menu3_Entry2 (void *dummy)
{

	/* PROGRAM ALMA */              

	/*Appload_Copy(APPLOAD_APL,NULL); */
	/*Appload_Copy(1,NULL);*/                                           /* MESAJLAR ÝNGÝLÝZCE   */
	Appload_Copy(1,&AploadMessages[0][0]);      /* MESAJLAR TÜRKÇE              */

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowF6Menu
 *
 * DESCRIPTION: Displays the F6 menu.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowF6Menu(void)
{
   entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"PESIN SATIS    ", 0, Menu6_Entry1   , 0},
				{"VADELI SATIS   ", 0, Menu6_Entry2   , 0},
				{"IPTAL          ", 0, Menu6_Entry3   , 0},
				{"BAKIYE SORMA   ", 0, Menu6_Entry4  , 0}		/* 04_16 06/08/2003 irfan bu satýr yeni eklendi*/
				};
   menu csMENU_SISTEM_menu;


   /* 04_07 A.Y. 18/12/2002 genius menu ismi degisikligi */
   /* csMENU_SISTEM_menu.header      = "GENIUS   MENšSš>"; */
   csMENU_SISTEM_menu.header         = "biz.card       >"; 
   csMENU_SISTEM_menu.no_of_entries  = 4;
   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

   Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu6_Entry1
 *
 * DESCRIPTION  : GENIUS PESIN SATIS
 *
 * RETURN: none
 *
 * NOTES:       25/07/99        r.i.ö. && t.c.a
 *                        Gtran_type : indicates the type of the genius transaction
 *                           GENIUS_SATIS			1
 *							 GENIUS_VADELI			2
 *							 GENUIS_IPTAL			3
 *
 * --------------------------------------------------------------------------*/
void Menu6_Entry1 (void *dummy)
{
	/* GENIUS PESIN SATIS */  
	byte ret_code;
	boolean Lbdummy;
	
	EmvIF_ClearDsp(DSP_MERCHANT);
	
	if(Host_PreDial() != STAT_OK) 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  ARAMA HATASI",1);
		return;
	}

	GVOID_FLAG       = FALSE;

	Gtran_type = GENIUS_SATIS;

	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;

	if (Mainloop_Get_CardInfo(0,0,0,0,0,0,0,0,0,&Lbdummy,FALSE))
	{ 
		ret_code = Trans_GeniusTransaction(Gtran_type);

		/*save parameters files */
		if(Files_WriteAllParams() != STAT_OK) 
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  PARAMETRELER     YAZILAMADI",1);
		}
		switch (ret_code)
		{
			case    STAT_OK:	break;

			case    STAT_DECLINED :	
								Mainloop_DisplayWait("ISLEM REDDEDILDI",2);
								break;

			case    AKNET_POS_TIMEOUT_ERROR:
								EmvIF_ClearDsp(DSP_MERCHANT);
								Mainloop_DisplayWait("  POS TIMEOUT",2);
								break;
			case    STAT_NOK:
								EmvIF_ClearDsp(DSP_MERCHANT);	
								Mainloop_DisplayWait("BASARISIZ ISLEM ",2);
								break;
			case TRR_NO_CONNECTION:
								EmvIF_ClearDsp(DSP_MERCHANT);
								Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
								break;
		}

	}
	else
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  KART BILGISI      ALINAMADI   ",2);
	}
	EmvIF_ClearDsp(DSP_MERCHANT);
	Host_Disconnect();      
	Mte_Wait(50);
	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu6_Entry2
 *
 * DESCRIPTION  : GENIUS PESIN IPTAL
 *
 * RETURN: none
 *
 * NOTES:       25/07/99        r.i.ö. && t.c.a
 *                        Gtran_type : indicates the type of the genius transaction
 *                           GENIUS_SATIS			1
 *							 GENIUS_VADELI			2
 *							 GENUIS_IPTAL			3
 *
 * --------------------------------------------------------------------------*/
void Menu6_Entry2 (void *dummy)
{

	/* GENIUS_VADELI SATIS */              
	byte ret_code;
	boolean Lbdummy;
	
	EmvIF_ClearDsp(DSP_MERCHANT);


	if(Host_PreDial() != STAT_OK) 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  ARAMA HATASI",1);
		return;
	}
	GVOID_FLAG       = FALSE;

	Gtran_type = GENIUS_VADELI;
	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;

	if (Mainloop_Get_CardInfo(0,0,0,0,0,0,0,0,0,&Lbdummy,FALSE))
	{ 
		ret_code = Trans_GeniusTransaction(Gtran_type);

		/*save parameters files */
		if(Files_WriteAllParams() != STAT_OK) 
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  PARAMETRELER     YAZILAMADI",1);
		}
		switch (ret_code)
		{
			case    STAT_OK:	break;

			case    AKNET_POS_TIMEOUT_ERROR:
								EmvIF_ClearDsp(DSP_MERCHANT);
								Mainloop_DisplayWait("  POS TIMEOUT",2);
								break;
			case    STAT_NOK:
								EmvIF_ClearDsp(DSP_MERCHANT);	
								Mainloop_DisplayWait("BASARISIZ ISLEM ",2);
								break;
			case TRR_NO_CONNECTION:
								EmvIF_ClearDsp(DSP_MERCHANT);
								Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
								break;
		}
	}
	else
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  KART BILGISI      ALINAMADI   ",2);
	}
	EmvIF_ClearDsp(DSP_MERCHANT);
	Host_Disconnect();      
	Mte_Wait(50);

	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu6_Entry3
 *
 * DESCRIPTION  : GENIUS VADELI SATIS
 *
 * RETURN: none
 *
 * NOTES:       25/07/99        r.i.ö. && t.c.a
 *                        Gtran_type : indicates the type of the genius transaction
 *                           GENIUS_SATIS			1
 *							 GENIUS_VADELI			2
 *							 GENUIS_IPTAL			3
 *
 * --------------------------------------------------------------------------*/
void Menu6_Entry3 (void *dummy)
{
	/* GENIUS IPTAL */
	byte ret_code;
	boolean Lbdummy;
	
	EmvIF_ClearDsp(DSP_MERCHANT);

	if(Host_PreDial() != STAT_OK) 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  ARAMA HATASI",1);
		return;
	}
	GVOID_FLAG       = TRUE;

	Gtran_type = GENUIS_IPTAL;

	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;
	
	if (Mainloop_Get_CardInfo(0,0,0,0,0,0,0,0,0,&Lbdummy,FALSE))
	{ 
		ret_code = Trans_GCancel(Gtran_type);
		/*save parameters files */
		if(Files_WriteAllParams() != STAT_OK) 
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  PARAMETRELER     YAZILAMADI",1);
		}
		switch (ret_code)
		{
			case    STAT_OK:	break;

			case    STAT_DECLINED :	
								Mainloop_DisplayWait("ISLEM REDDEDILDI",2);
								break;

			case    AKNET_POS_TIMEOUT_ERROR:
								EmvIF_ClearDsp(DSP_MERCHANT);
								Mainloop_DisplayWait("  POS TIMEOUT",2);
								break;
			case    STAT_NOK:
								EmvIF_ClearDsp(DSP_MERCHANT);	
								Mainloop_DisplayWait("BASARISIZ ISLEM ",2);
								break;
			case TRR_NO_CONNECTION:
								EmvIF_ClearDsp(DSP_MERCHANT);
								Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
								break;
		}

	}
	else
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  KART BILGISI      ALINAMADI   ",2);
	}
	EmvIF_ClearDsp(DSP_MERCHANT);
	Host_Disconnect();      
	Mte_Wait(50);
	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;
}

/* 04_16 biz card bakiye sorma fonksiyonu 06/08/2003 irfan */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Menu3_Entry4
 *
 * DESCRIPTION  : BIZ CARD BAKIYE SORMA
 *
 * RETURN		: none
 *
 * NOTES		: 06/08/2003  r.i.ö. 
 *                Gtran_type : indicates the type of the genius transaction
 *                          GENIUS_SATIS			1
 *							GENIUS_VADELI			2
 *							GENUIS_IPTAL			3
 *							GENUIS_BAKIYE_SORMA		5
 *
 * --------------------------------------------------------------------------*/
void Menu6_Entry4 (void *dummy)
{
	/* GENIUS BAKIYE SORMA */  
	byte ret_code;
	boolean Lbdummy;
	
	EmvIF_ClearDsp(DSP_MERCHANT);
	
	GVOID_FLAG       = FALSE;

	Gtran_type = GENUIS_BAKIYE_SORMA;

	if(Host_PreDial() != STAT_OK) 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  ARAMA HATASI",1);
		return;
	}
	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;

	if (Mainloop_Get_CardInfo(0,0,0,0,0,0,0,0,0,&Lbdummy,FALSE))
	{ 
		/*ret_code = Trans_GeniusTransaction(Gtran_type);*/
		ret_code = Trans_GeniusBakiyeSorma(Gtran_type);

		/*save parameters files */
		if(Files_WriteAllParams() != STAT_OK) 
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  PARAMETRELER     YAZILAMADI",1);
		}
		switch (ret_code)
		{
			case    STAT_OK:	break;

			case    AKNET_POS_TIMEOUT_ERROR:
								EmvIF_ClearDsp(DSP_MERCHANT);
								Mainloop_DisplayWait("  POS TIMEOUT",2);
								break;
			case    STAT_NOK:
								EmvIF_ClearDsp(DSP_MERCHANT);	
								Mainloop_DisplayWait("BASARISIZ ISLEM ",2);
								break;
			case TRR_NO_CONNECTION:
								EmvIF_ClearDsp(DSP_MERCHANT);
								Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
								break;
		}
	}
	else
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  KART BILGISI      ALINAMADI   ",2);
	}
	EmvIF_ClearDsp(DSP_MERCHANT);
	Host_Disconnect();      
	Mte_Wait(50);
	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_TransactionMenu
 *
 * DESCRIPTION  : Displays the Transaction Menu.
 *
 * RETURN               : none
 *
 * NOTES                : GRAPHIC_DISPLAY R.Ý.Ö. 21/01/2001 
 *
 * --------------------------------------------------------------------------*/
void Mainloop_TransactionMenu(void)
{
	entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"™N PROV˜ZYON    ", 0, Menu4_Entry0   , 0},
				{"BAK˜YE SORMA    ", 0, Menu4_Entry1   , 0},
				{"MANUEL ˜žLEM    ", 0, Menu4_Entry2   , 0},
				{"ARA RAPOR       ", 0, Menu4_Entry3   , 0}
				};
	menu csMENU_SISTEM_menu;

	csMENU_SISTEM_menu.header         = "    ˜žLEMLER    ";
	csMENU_SISTEM_menu.no_of_entries  = 4;
	csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
	csMENU_SISTEM_menu.mode_flags 	= DEFAULT_MODE | MENU_TIMEOUT_30_SEC;

	Formater_GoMenu( (struct menu *)&csMENU_SISTEM_menu, 0);
	EmvIF_ClearDsp(DSP_MERCHANT);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: 
 *
 * DESCRIPTION  : Entry0 - Entry4  
 *
 * RETURN               : none
 *
 * NOTES                : R.Ý.Ö GRAPHIC DISPLAY 21/01/2001 
 *
 * --------------------------------------------------------------------------*/
void Menu4_Entry0 (void *dummy)
{
	byte ret_code;
/* Toygar EMVed */
/* Toygar - New EMV Start */
/*
	EmvApplication eaApplication;
*/
	EMVTransData etdData;
	RowData rdSDAData, myrdData;
	PTStruct fpPTStruct;
	boolean IsEMV;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
	ApplicationParams *pAppData;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
/*	TerminalParams pTermData;*/
	EMVLanguages pelsLangs;
	byte pbErr;
/* Toygar - NextEMV End - Ok */
	TerminalAppList fpAppList;

	EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
	EMVAxess = FALSE;
	Files_ReadEMVAppParam(&fpAppList);
	memcpy(&pelsLangs,&elsLangs,sizeof(pelsLangs));

	/* preprovision */
	tran_type = PREPROV;
	if ( Mainloop_Get_CardInfo
		 ( &pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex,
		   &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData, &fpPTStruct,
		   &IsEMV, FALSE ) )
	{
		if (!( EMVAxess && !EMVAxessStatu ))
		{
	if(Host_PreDial() != STAT_OK) 
	{
		Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
		return;
	}
		}
		memset( Received_Buffer, 0, sizeof( Received_Buffer) );
		if ( EMVAxess && !EMVAxessStatu )
		{
			memset( fpPTStruct.Pan, 0, sizeof( fpPTStruct.Pan ) );
			EmvDb_Find( &myrdData, tagPAN );
			EmvIF_BinToAscii( myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof( fpPTStruct.Pan ), FALSE );
			while ( fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] == 'F' )
				fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] = 0;
			fpPTStruct.PanLength = strlen( fpPTStruct.Pan );
			memset( card_no, 0, 20 );
			memcpy( card_no, fpPTStruct.Pan, fpPTStruct.PanLength );
			if ( !EMVAxessGetAmount(  ) )
	{ 
				Mainloop_DisplayWait( "EMVAxessGetAmount Hatasi", 2 );
				EmvIF_ClearDsp(DSP_MERCHANT);
				EMVAxessStatu = FALSE;
			}
			else EMVAxessStatu = TRUE;
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			return;
		}
		ret_code = Trans_Transaction( pAppData, &fpAppList, &pMutualApps,
						   &pbMutualAppIndex, &lsTerminalDataStruct,
						   &pelsLangs, &pbErr, &rdSDAData, &etdData,
						   &fpPTStruct, IsEMV, FALSE );
		if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
		switch (ret_code)
		{
		case STAT_OK:
			break;
		case AKNET_POS_TIMEOUT_ERROR:
			Mainloop_DisplayWait("POS TIMEOUT",2);
			break;
		case STAT_NOK:
			Mainloop_DisplayWait("  ™N PROV˜ZYON       HATALI     ",2);
			break;
		case TRR_NO_CONNECTION:
			Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
			break;	
		}
	}
	EMVAxessStatu = FALSE;
	Host_Disconnect();      
	Mte_Wait(300);
	/** XLS_PAYMENT 19/07/2001 irfan */
	Mainloop_XLSRemoveChipCard();
}

void Menu4_Entry1 (void *dummy)
{
	byte ret_code;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
/*
	EmvApplication eaApplication;
*/
/* Toygar - NextEMV End - Ok */
	EMVTransData etdData;
	RowData rdSDAData, myrdData;
	PTStruct fpPTStruct;
	boolean IsEMV;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
	ApplicationParams *pAppData;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
/*	TerminalParams pTermData;*/
	EMVLanguages pelsLangs;
	byte pbErr;
/* Toygar - NextEMV End - Ok */
	TerminalAppList fpAppList;

	EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
	EMVAxess = FALSE;
	Files_ReadEMVAppParam(&fpAppList);
	memcpy(&pelsLangs,&elsLangs,sizeof(pelsLangs));

	/* Balance */
	tran_type = BALANCE;
	if ( Mainloop_Get_CardInfo
		 ( &pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex,
		   &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData, &fpPTStruct,
		   &IsEMV, FALSE ) )
	{
		memset( Received_Buffer, 0, sizeof( Received_Buffer) );
		if (!( EMVAxess && !EMVAxessStatu ))
		{
	if(Host_PreDial() != STAT_OK) 
	{
		Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
		return;
	}
		}
		memset( Received_Buffer, 0, sizeof( Received_Buffer) );
		if ( EMVAxess && !EMVAxessStatu )
	{ 
			memset( fpPTStruct.Pan, 0, sizeof( fpPTStruct.Pan ) );
			EmvDb_Find( &myrdData, tagPAN );
			EmvIF_BinToAscii( myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof( fpPTStruct.Pan ), FALSE );
			while ( fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] == 'F' )
				fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] = 0;
			fpPTStruct.PanLength = strlen( fpPTStruct.Pan );
			memset( card_no, 0, 20 );
			memcpy( card_no, fpPTStruct.Pan, fpPTStruct.PanLength );
			EMVAxessStatu = TRUE;
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			return;
		}
		ret_code =
			Trans_Transaction( pAppData, &fpAppList, &pMutualApps,
							   &pbMutualAppIndex, &lsTerminalDataStruct,
							   &pelsLangs, &pbErr, &rdSDAData, &etdData,
							   &fpPTStruct, IsEMV, FALSE );
		if ( Files_WriteAllParams(  ) != STAT_OK )
		Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
		switch (ret_code)
		{
		case STAT_OK:
			break;
		case AKNET_POS_TIMEOUT_ERROR:
			Mainloop_DisplayWait("POS TIMEOUT",2);
			break;
		case STAT_NOK:
			Mainloop_DisplayWait(" BAK˜YE SORMA       HATALI      ",2);
			break;
		case TRR_NO_CONNECTION:
			Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
			break;
		}
	}
	Host_Disconnect();      
	Mte_Wait(300);
	/** XLS_PAYMENT 19/07/2001 irfan */
	Mainloop_XLSRemoveChipCard();
}

void Menu4_Entry2 (void *dummy)
{
	byte ret_code;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
/*
	EmvApplication eaApplication;
*/
/* Toygar - NextEMV End - Ok */
	EMVTransData etdData;
	RowData rdSDAData, myrdData;
	PTStruct fpPTStruct;
	boolean IsEMV;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
	ApplicationParams *pAppData;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
/*	TerminalParams pTermData;*/
	EMVLanguages pelsLangs;
	byte pbErr;
/* Toygar - NextEMV End - Ok */
	TerminalAppList fpAppList;

	EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
	EMVAxess = FALSE;
	Files_ReadEMVAppParam(&fpAppList);
	memcpy(&pelsLangs,&elsLangs,sizeof(pelsLangs));
	
	/* Manual Transaction */
	/* 01_18 21/21/2000 Manuel iþlemler için kontrol konuldu. */
	if(MANUEL_PERMISSION)
	{
		tran_type = MANUEL;
		if ( Mainloop_Get_CardInfo
			 ( &pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex,
			   &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData,
			   &fpPTStruct, &IsEMV, FALSE ) )
		{
			memset( Received_Buffer, 0, sizeof( Received_Buffer) );
			if (!( EMVAxess && !EMVAxessStatu ))
			{
		if(Host_PreDial() != STAT_OK) 
		{
			Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
			return;
		}
			}
			memset( Received_Buffer, 0, sizeof( Received_Buffer) );
			if ( EMVAxess && !EMVAxessStatu )
			{
				memset( fpPTStruct.Pan, 0, sizeof( fpPTStruct.Pan ) );
				EmvDb_Find( &myrdData, tagPAN );
				EmvIF_BinToAscii( myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof( fpPTStruct.Pan ), FALSE );
				while ( fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] == 'F' )
					fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] = 0;
				fpPTStruct.PanLength = strlen( fpPTStruct.Pan );
				memset( card_no, 0, 20 );
				memcpy( card_no, fpPTStruct.Pan, fpPTStruct.PanLength );
				if ( !EMVAxessGetAmount(  ) )
		{ 
					Mainloop_DisplayWait( "EMVAxessGetAmount Hatasi", 2 );
					EmvIF_ClearDsp(DSP_MERCHANT);
					EMVAxessStatu = FALSE;
				}
				else EMVAxessStatu = TRUE;
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				return;
			}
			ret_code =
				Trans_Transaction( pAppData, &fpAppList, &pMutualApps,
								   &pbMutualAppIndex, &lsTerminalDataStruct,
								   &pelsLangs, &pbErr, &rdSDAData, &etdData,
								   &fpPTStruct, IsEMV, FALSE );
			if ( Files_WriteAllParams(  ) != STAT_OK )
			Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
			switch (ret_code)
			{
			case    STAT_OK:
				break;
			case    AKNET_POS_TIMEOUT_ERROR:
				Mainloop_DisplayWait("POS TIMEOUT",2);
				break;
			case    STAT_NOK:
				Mainloop_DisplayWait(" MANUEL ˜žLEM       HATALI      ",2);
				break;
			case    TRR_NO_CONNECTION:
				Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
				break;
			}
		}
		Mte_Wait(300);
		EMVAxessStatu = FALSE;
		Host_Disconnect();      
	}
	else Mainloop_DisplayWait(" MANUEL ˜žLEME       KAPALI     ",1);
	/** XLS_PAYMENT 19/07/2001 irfan */
	Mainloop_XLSRemoveChipCard();
}

void Menu4_Entry3 (void *dummy)
{
	/* Report */ 
	ARA_RAPOR = TRUE;
	Display_UpDisplay("   ARA RAPOR       BASILIYOR... ");
	if (Slips_Report(FALSE) != STAT_OK)
	Mainloop_DisplayWait("ARA RAPOR'DA HATA OLUžTU",2);
	PrntUtil_Cr(6);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_DisplaySet
 *
 * DESCRIPTION: 
 *
 * RETURN:      None.
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
void Mainloop_DisplaySet(void)
{
	TFont *HedrFnt, *EntrFnt;
	byte   Ymax;
	WinFormat winformat;

    	GrphText_DispInfo(&Ymax,winformat.Format);
	HedrFnt=GrphText_FontPrm(FONT_13X7); /* Header font */
	EntrFnt=GrphText_FontPrm(FONT_10X7); /* Entry Font  */

    	winformat.HeaderFont  = FONT_13X7;
	winformat.EntriesFont = FONT_10X7;
	winformat.Format[0]   = HedrFnt->SiteHeight+1;
	winformat.Format[1]   =
	winformat.Format[2]   = EntrFnt->SiteHeight;
	winformat.Format[3]   = ETC;
	winformat.Format[1]  += ((Ymax-HedrFnt->SiteHeight+1) % EntrFnt->SiteHeight)/2;
    	Formater_Form (&winformat);
	Debug_SetFont (TRUE);	
	SysUtil_SetLanguage(TURKISH,DAY_MONTH_YEAR);
	/* Set Language For Communication Handler*/
	ComHndlr_SetCallerLanguage ((char *)My_Message [0]);
/*	ComHndlr_SetLanguage((char *) msConnect[0]);*/
}



/* ***************************************************************************
   ****************************** LOYALTY FUNCTIONS **************************
   ***************************************************************************/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_LoyaltyMainMenu
 *
 * DESCRIPTION  : Displays the Loyalty Main Menu.
 *
 * RETURN               : none
 *
 * NOTES                : LOYALTY R.Ý.Ö. 10/07/2001 
 *
 * --------------------------------------------------------------------------*/
void MainloopXLS_LoyaltyMainMenu(void)
{

   entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"ODEME ANA MENU  ", 0, MenuXLS00_Entry0   , 0},
				{"SADAKAT PRG MENU", 0, MenuXLS00_Entry1   , 0},
				{"CHIPS˜Z ˜žLEMLER", 0, MenuXLS00_Entry2   , 0},
/* @bm 13.08.2004		{"AXESS FZL˜ ˜žLEM", 0, MenuXLS00_Entry3, 0} */
				{"TL ˜ADE ˜žLEM˜  ", 0, MenuXLS00_Entry3, 0}

				};

   menu csMENU_SISTEM_menu;
	EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
	EMVAxess = FALSE;
	csMENU_SISTEM_menu.header = "    ANA MENU    ";

		if ( POS_Type == '6' )
		{
			if ( ( memcmp( YTL_Slip, "TLE", 3 ) == 0 ) )
	csMENU_SISTEM_menu.no_of_entries = 4;
			else
				csMENU_SISTEM_menu.no_of_entries = 3;
		}
		else
		{
			csMENU_SISTEM_menu.no_of_entries = 3;
		}

   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;
   Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);
   EmvIF_ClearDsp(DSP_MERCHANT);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_LoyaltyMainMenu items.
 *
 * DESCRIPTION  : Menu00_Entry0 - Menu00_Entry1  
 *
 * RETURN       : none
 *
 * NOTES        : LOYALTY R.Ý.Ö. 21/01/2001 
 *
 * --------------------------------------------------------------------------*/
void MenuXLS00_Entry0 (void *dummy)
{
	Mainloop_TransactionMenu();
	return;
}

void MenuXLS00_Entry1 (void *dummy)
{
	XLS_Menu();
	return;
	
}

void MenuXLS00_Entry2 (void *dummy)
{
	Mainloop_Transaction_CorruptedChipCard();
	return;
	
}

void MenuXLS00_Entry3 (void *dummy)
{
	byte ret_code;
	EMVTransData etdData;
	RowData rdSDAData, myrdData;
	PTStruct fpPTStruct;
	boolean IsEMV;
	ApplicationParams *pAppData;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
	EMVLanguages pelsLangs;
	byte pbErr;
	TerminalAppList fpAppList;

	Files_ReadEMVAppParam( &fpAppList );
	memcpy( &pelsLangs, &elsLangs, sizeof( pelsLangs ) );

	if ( RETURN_PERMISSION )
	{
		TL_Iade = TRUE; /* TL -> YTL */
		EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
		EMVAxess = FALSE;
		tran_type = RETURN;

		if ( Mainloop_Get_CardInfo
			 ( &pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex,
			   &lsTerminalDataStruct, &pelsLangs, &pbErr, &etdData,
			   &fpPTStruct, &IsEMV, TRUE ) )
		{
			char first_6_digit_local[7];
			
			if ( ( POS_Type == '3' ) || ( POS_Type == '5' ) ) EMVAxess = FALSE;

			if (!( EMVAxess && !EMVAxessStatu ))
			{
				if ( Host_PreDial(  ) != STAT_OK )
				{
					Mainloop_DisplayWait( "     ARAMA           HATASI     ", 1 );
					return;
				}
			}
	
			memset( Received_Buffer, 0, sizeof( Received_Buffer) );
			if ( EMVAxess && !EMVAxessStatu )
			{
				memset( fpPTStruct.Pan, 0, sizeof( fpPTStruct.Pan ) );
				EmvDb_Find( &myrdData, tagPAN );
				EmvIF_BinToAscii( myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof( fpPTStruct.Pan ), FALSE );
				while ( fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] == 'F' )
					fpPTStruct.Pan[strlen( fpPTStruct.Pan ) - 1] = 0;
				fpPTStruct.PanLength = strlen( fpPTStruct.Pan );
				memset( card_no, 0, 20 );
				memcpy( card_no, fpPTStruct.Pan, fpPTStruct.PanLength );
				if ( !EMVAxessGetAmount(  ) )
				{
					Mainloop_DisplayWait( "EMVAxessGetAmount Hatasi", 2 );
					EmvIF_ClearDsp(DSP_MERCHANT);
					EMVAxessStatu = FALSE;
				}
				else EMVAxessStatu = TRUE;
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				Kb_Insert(MMI_ESCAPE);
				return;
			}
			ret_code =
				Trans_Transaction( pAppData, &fpAppList, &pMutualApps,
								   &pbMutualAppIndex, &lsTerminalDataStruct,
								   &pelsLangs, &pbErr, &rdSDAData, &etdData,
								   &fpPTStruct, IsEMV, FALSE );
			if ( Files_WriteAllParams(  ) != STAT_OK )
				Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
			switch ( ret_code )
			{
				case STAT_OK:
					break;
				case AKNET_POS_TIMEOUT_ERROR:
					Mainloop_DisplayWait( "POS TIMEOUT", 2 );
					break;
				case STAT_NOK:
					Mainloop_DisplayWait( "      ˜ADE          HATALI      ",
										  2 );
					break;
				case TRR_NO_CONNECTION:
					Mainloop_DisplayWait( "  G™NDER˜LEMED˜ ", 2 );
					break;
			}
		}
		Host_Disconnect(  );
		Mte_Wait( 300 );
	}
	else Mainloop_DisplayWait( "    ˜ADEYE          KAPALI     ", 1 );
	Mainloop_XLSRemoveChipCard(  );
}

/** XLS_PAYMENT 25/07/2001 irfan */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_Transaction_CorruptedChipCard
 *
 * DESCRIPTION  : Displays the Transaction Menu.
 *
 * RETURN		: none
 *
 * NOTES		: GRAPHIC_DISPLAY R.Ý.Ö. 25/07/2001 
 *
 * --------------------------------------------------------------------------*/
void Mainloop_Transaction_CorruptedChipCard(void)
{
   entry csMENU_SISTEM_entry [] = {
				{"AXESS SATIS     ", 0, MenuCorruptedChip_Entry0   , 0},
				{"AXESS IADE      ", 0, MenuCorruptedChip_Entry1   , 0},
				{"AXESS IPTAL     ", 0, MenuCorruptedChip_Entry2   , 0},
				{"AXESS ON PROV.  ", 0, MenuCorruptedChip_Entry3   , 0}, /* XLS_INTEGRATION2 17/10/2001 eklendi **/
		{"AXESS MANUEL IS.", 0, MenuCorruptedChip_Entry4, 0}	/* XLS_INTEGRATION2 17/10/2001 eklendi * */
		/*@bm 24.08.2004		
		{"AXESS FAIZLI ISL", 0, MenuCorruptedChip_Entry5, 0} */
				};
   menu csMENU_SISTEM_menu;

   csMENU_SISTEM_menu.header         = "CHIPSIZ ISLEMLER";
	csMENU_SISTEM_menu.no_of_entries = 5;
   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;
   Formater_GoMenu ((struct menu *)&csMENU_SISTEM_menu, 0);
   CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;
}

/** XLS_PAYMENT Loyalty Satis(Sale) **/
void MenuCorruptedChip_Entry0 (void *dummy)
{
	/** chipsiz satýþ **/
	byte ret_code;
	boolean IsEMV;
	char first_6_digit_local[7];

	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;
	/* 01_18 14/01/00 El ile kart no'su giriþinde MANUEL_ENTRY_PERMISSION :  1: ÞÝFRE SOR  0: SORMA*/
	if(MANUEL_ENTRY_PERMISSION)
	{
		if(!Utils_AskUserPassword()) return;
	}
	tran_type = SALE;

/* Toygar EMVed */

/* Toygar - NextEMV Start - Ok */
	if ( Mainloop_Get_CardInfo( 0, 0, 0, 0, 0, 0, 0, 0, 0, &IsEMV, FALSE ) )
/* Toygar - NextEMV Start - Ok */
/* Toygar EMVed */
	{ 
		/** XLS_PAYMENT 25/07/2001 irfan. Arama islemi Kart girisinden sonraya alindi **/

		/* Check for Loyalty_Prefixes */
		memcpy( first_6_digit_local, card_no, 6 );
		first_6_digit_local[6] = 0;
		if ( strstr( Loyalty_Prefixes, first_6_digit_local ) == NULL )
		{
			Mainloop_DisplayWait( "SADAKAT PROGRAMIKARTLAR GECERLI ", 1 );
			return;
		}

		/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
		if(Host_PreDial() != STAT_OK) 
		{
			Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
			return;
		}
		 /**/ ret_code =
			Trans_Transaction( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FALSE, FALSE );
		 /**/
/*		ret_code = Trans_Transaction();*/

		/*save parameters files */
			/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
			 * eger yazamas ise idle loop'a donuyor */
			if ( Files_WriteAllParams(  ) != STAT_OK )
			Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
		switch (ret_code)
			{
			case STAT_OK:
				break;
			case AKNET_POS_TIMEOUT_ERROR:
				Mainloop_DisplayWait("POS TIMEOUT",2);
				break;
			case STAT_NOK:
				Mainloop_DisplayWait(" TAMAMLANAMADI  TEKRAR DENEY˜N˜Z",2);
				break;
			case TRR_NO_CONNECTION:
				Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
				break;
			}
	}
	Host_Disconnect();      
	Mte_Wait(300);
	/** XLS_PAYMENT 19/07/2001 irfan */
	Mainloop_XLSRemoveChipCard();
	EmvIF_ClearDsp(DSP_MERCHANT);
	return;
}

/** XLS_PAYMENT chipsiz Loyalty Iade (Return) **/
void MenuCorruptedChip_Entry1 (void *dummy)
{
	byte ret_code;
	boolean IsEMV;
	char first_6_digit_local[7];

	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;
	if(RETURN_PERMISSION)
	{
			tran_type = RETURN;

/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
		if ( Mainloop_Get_CardInfo
			 ( 0, 0, 0, 0, 0, 0, 0, 0, 0, &IsEMV, FALSE ) )
/* Toygar - NextEMV Start - Ok */
/* Toygar EMVed */
			{ 
			/* Check for Loyalty_Prefixes */
			memcpy( first_6_digit_local, card_no, 6 );
			first_6_digit_local[6] = 0;
			if ( strstr( Loyalty_Prefixes, first_6_digit_local ) == NULL )
			{
				Mainloop_DisplayWait( "SADAKAT PROGRAMIKARTLAR GECERLI ", 1 );
				return;
			}

				if(Host_PreDial() != STAT_OK) 
				{
					Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
					return;
				}
			 /**/ ret_code =
				Trans_Transaction( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FALSE,
								   FALSE );
			 /**/
/*				ret_code = Trans_Transaction();*/

				/*save parameters files */
				/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
				eger yazamas ise idle loop'a donuyor */
				if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
				switch (ret_code)
				{
				case STAT_OK:
					break;
				case AKNET_POS_TIMEOUT_ERROR:
					Mainloop_DisplayWait("POS TIMEOUT",2);
					break;
				case STAT_NOK:
					Mainloop_DisplayWait("      ˜ADE          HATALI      ",2);
					break;
				case TRR_NO_CONNECTION:
					Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
					break;
				}
			}
			Host_Disconnect();      
			Mte_Wait(300);
	}
	else
	{
		Mainloop_DisplayWait("    ˜ADEYE          KAPALI     ",1);
	}

	/** XLS_PAYMENT 19/07/2001 irfan */
	Mainloop_XLSRemoveChipCard();
	return;
}

/** XLS_PAYMENT chipsiz Loyalty Iptal(Cancel) **/
void MenuCorruptedChip_Entry2 (void *dummy)
{
	byte ret_code;
	boolean IsEMV;
	char first_6_digit_local[7];

	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;

/* Toygar EMVed */

/* Toygar - NextEMV Start - Ok */
	if ( Mainloop_Get_CardInfo( 0, 0, 0, 0, 0, 0, 0, 0, 0, &IsEMV, FALSE ) )
/* Toygar - NextEMV Start - Ok */
/* Toygar EMVed */
	{ 
		/* Check for Loyalty_Prefixes */
		memcpy( first_6_digit_local, card_no, 6 );
		first_6_digit_local[6] = 0;
		if ( strstr( Loyalty_Prefixes, first_6_digit_local ) == NULL )
		{
			Mainloop_DisplayWait( "SADAKAT PROGRAMIKARTLAR GECERLI ", 1 );
			return;
		}

		if(Host_PreDial() != STAT_OK) 
		{
			Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
			return;
		}

		ret_code = Trans_Cancel( IsEMV, FALSE );

		/*save parameters files */
		/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
		 * eger yazamas ise idle loop'a donuyor */
		if ( Files_WriteAllParams(  ) != STAT_OK )
			Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
		switch (ret_code)
		{
		case    STAT_OK:
			/* 06_08 basarili iptal sonucu verde pod'a bildiriliyor */
			if( VERDE_TRANSACTION )
			{	/* send RES4 get ACK  */
					if ( Verde_SendRes4(  ) != STAT_OK )
						Verde_SendControlChar( EOT );
			}

			break;
		case    AKNET_POS_TIMEOUT_ERROR:        /* UPDATE 01_18 02/12/99 TIMEOUT_ERROR idi.Ancak bunun kullanýlmamasý gerekir*/
			Mainloop_DisplayWait("POS TIMEOUT",2);
			break;
		case    STAT_NOK:
			Mainloop_DisplayWait("      ˜PTAL         HATALI      ",2);
			break;
		case TRR_NO_CONNECTION:
			Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
			break;
		}
	}
	Host_Disconnect();      
	Mte_Wait(300);

	/** XLS_PAYMENT 19/07/2001 irfan */
	Mainloop_XLSRemoveChipCard();
	return;
}

/** XLS_INTEGRATION2 irfan. 17/10/2001 eklendi.*/
/** chipsiz on provizyon islemi **/
void MenuCorruptedChip_Entry3(void *dummy)
{
	byte ret_code;
	boolean IsEMV;
	char first_6_digit_local[7];

	/* preprovision */
	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;
	tran_type = PREPROV;
	/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
	if(Host_PreDial() != STAT_OK) 
	{
		Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
		return;
	}

/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
	if ( Mainloop_Get_CardInfo( 0, 0, 0, 0, 0, 0, 0, 0, 0, &IsEMV, FALSE ) )
/* Toygar - NextEMV Start - Ok */
/* Toygar EMVed */
	{ 
		/* Check for Loyalty_Prefixes */
		memcpy( first_6_digit_local, card_no, 6 );
		first_6_digit_local[6] = 0;
		if ( strstr( Loyalty_Prefixes, first_6_digit_local ) == NULL )
		{
			Mainloop_DisplayWait( "SADAKAT PROGRAMIKARTLAR GECERLI ", 1 );
			return;
		}
		 /**/ ret_code =
			Trans_Transaction( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FALSE, FALSE );
		 /**/
/*		ret_code = Trans_Transaction();*/

		/*save parameters files */
		/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
			 * eger yazamas ise idle loop'a donuyor */
			if ( Files_WriteAllParams(  ) != STAT_OK )
			Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
		switch (ret_code)
		{
		case    STAT_OK:
			break;
		case    AKNET_POS_TIMEOUT_ERROR:
			Mainloop_DisplayWait("POS TIMEOUT",2);
			break;
		case    STAT_NOK:
			Mainloop_DisplayWait("  ™N PROV˜ZYON       HATALI     ",2);
			break;
		case TRR_NO_CONNECTION:
			Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
			break;
		}
	}
	Host_Disconnect();      
	Mte_Wait(300);
	/** XLS_PAYMENT 19/07/2001 irfan */
	Mainloop_XLSRemoveChipCard();
	return;	/** XLS_INTEGRATION2 irfan. 19/10/2001 **/
}


/** XLS_INTEGRATION2 irfan. 17/10/2001 eklendi.*/
/** chipsiz manuel islem **/
void MenuCorruptedChip_Entry4(void *dummy)
{
	byte ret_code;
	boolean IsEMV;
	char first_6_digit_local[7];

	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;
	/* Manual Transaction */
	/* 01_18 21/21/2000 Manuel iþlemler için kontrol konuldu. */
	if(MANUEL_PERMISSION)
	{
		tran_type = MANUEL;
		/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
		if(Host_PreDial() != STAT_OK) 
		{
			Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
			return;
		}

/* Toygar EMVed */

/* Toygar - NextEMV Start - Ok */
		if ( Mainloop_Get_CardInfo
			 ( 0, 0, 0, 0, 0, 0, 0, 0, 0, &IsEMV, FALSE ) )
/* Toygar - NextEMV Start - Ok */
/* Toygar EMVed */
		{ 
			/* Check for Loyalty_Prefixes */
			memcpy( first_6_digit_local, card_no, 6 );
			first_6_digit_local[6] = 0;
			if ( strstr( Loyalty_Prefixes, first_6_digit_local ) == NULL )
			{
				Mainloop_DisplayWait( "SADAKAT PROGRAMIKARTLAR GECERLI ", 1 );
				return;
			}

			 /**/ ret_code =
				Trans_Transaction( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FALSE,
								   FALSE );
			 /**/
/*			ret_code = Trans_Transaction();*/

			/*save parameters files */
			/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
			eger yazamas ise idle loop'a donuyor */
			if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
			switch (ret_code)
			{
			case    STAT_OK:
				break;
			case    AKNET_POS_TIMEOUT_ERROR:
				Mainloop_DisplayWait("POS TIMEOUT",2);
				break;
			case    STAT_NOK:
				Mainloop_DisplayWait(" MANUEL ˜žLEM       HATALI      ",2);
				break;
			case    TRR_NO_CONNECTION:
				Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
				break;
			}
		}
		Mte_Wait(300);
		Host_Disconnect();      
	}
	else
	{
		Mainloop_DisplayWait(" MANUEL ˜žLEME       KAPALI     ",1);
	}

	/** XLS_PAYMENT 19/07/2001 irfan */
	Mainloop_XLSRemoveChipCard();
	return;	/** XLS_INTEGRATION2 irfan. 19/10/2001 **/
}

#if 0

/** XLS_PAYMENT 18/07/2001 irfan */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ChipInserted.
 *
 * DESCRIPTION  : Read Chip Card to get track2 data.
 *
 * RETURN       : none
 *
 * NOTES        : LOYALTY R.Ý.Ö. 18/07/2001 
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_ChipInserted(boolean match/*RowData *rdSDAData, EMVTransData *etdData, PTStruct *fpPTStruct, EmvApplication *eaApplication*/)
{
	byte tran_ret;
/* Toygar EMVed */
/* Toygar - NextEMV Start - Ok */
/*
	EmvApplication eaApplication;
*/
/* Toygar - NextEMV End - Ok */
	EMVTransData etdData;
	RowData rdSDAData;
	PTStruct fpPTStruct;
/* Toygar-EMVed */
	byte rv;
/* Toygar - New EMV Start */
/*
	AID TermList[MAX_EMV_APPL];
	usint AppCount;
	usint AppIndex;
	TermAppList MutualList;
*/
/* Toygar - New EMV End */
	byte abSDADataBuf[MAX_NCA_LEN];
	boolean axesscardentry;
/* Toygar-EMVed */

/* Toygar - NextEMV Start - Ok */
	ApplicationParams *pAppData;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
/*	TerminalParams pTermData;*/
	EMVLanguages pelsLangs;
	byte pbErr;
/* Toygar - NextEMV End - Ok */
	TerminalAppList fpAppList;

	Files_ReadEMVAppParam(&fpAppList);
	memcpy(&pelsLangs,&elsLangs,sizeof(pelsLangs));

	CardRdr_Disable();
	axesscardentry = TRUE;

/*
	if (rdSDAData == 0)
	{
*/		/* read the banking file in chip card to get track2 data */
		if ((!match) || (E3744_SelectRead() != STAT_OK))
		{
			axesscardentry = FALSE;
/*			Mainloop_DisplayWait("    CHIP KART     OKUMA HATASI  ",1);
			return(STAT_NOK);*/
		}
/*	}*/

	/* Toygar-EMVed */
	if (/*(POSEntryFlag <= 0) && */(!axesscardentry)) /* 06_05 indendation duzeltildi */
	{
/* Toygar - NextEMV Start - Ok */
/*		
		if (Main_SwitchTerminalConfiguration(1, &AppCount, TermList) == STAT_OK)
*/
/* Toygar - NextEMV Start - Ok */
		{
/*			Iso78164_Init();*/
/* Toygar - NextEMV Start - Ok */
/*
			rv = EmvCmd_StartSession();
			DEBUG(8,{(rv == EMV_OK) ? Printer_WriteStr("Session opened\n") : Printer_WriteStr("Session open failure\n");Printer_WriteStr("Selecting Application\n");})
*/
/* Toygar - NextEMV End - Ok */
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait((char *)csEMV_PLEASE_WAIT,NO_WAIT);

/* Toygar - NextEMV Start - Ok */
                        pbErr = 0; 
			if (Main_ApplicationSelection (&fpAppList, &pMutualApps, &pbMutualAppIndex,
							&lsTerminalDataStruct, &pelsLangs, &pbErr) != STAT_OK)
/*	
			if (Main_ApplicationSelection(AppCount, TermList, &AppIndex, &MutualList, 
							&lsApplicationDataStruct, &lsTerminalDataStruct, &eaApplication, &etdData) != STAT_OK)
*/
/* Toygar - NextEMV End - Ok */
			{
				Mainloop_XLSRemoveChipCard();
				return;
			}
			pAppData = &fpAppList.TermApp[pMutualApps.MutualApps[pbMutualAppIndex].TermAppIndex];
			fpPTStruct.Field55Len = Trans_AddChipFields((byte *)fpPTStruct.Field55, &etdData);
/*			Trans_PassEMVData(&fpPTStruct, &etdData);*/
			if (Main_ReadApplicationData(&pbErr) != STAT_OK)
/* Toygar - NextEMV Start - Ok */
/*
			if (Main_ReadApplicationData(abSDADataBuf, &rdSDAData, &etdData, &eaApplication) != STAT_OK)
*/
/* Toygar - NextEMV End - Ok */
			{
				Mainloop_XLSRemoveChipCard();
				return;
			}
			fpPTStruct.Field55Len = Trans_AddChipFields((byte *)fpPTStruct.Field55, &etdData);
/*			Trans_PassEMVData(&fpPTStruct, &etdData);*/
			if (Main_DAMethod(pAppData, &pbErr) != STAT_OK)
/* Toygar - NextEMV Start - Ok */
/*
			if (Main_DAMethod(&rdSDAData, &eaApplication) != STAT_OK)
*/
/* Toygar - NextEMV End - Ok */
			{
				Mainloop_XLSRemoveChipCard();
				return;
			}
			fpPTStruct.Field55Len = Trans_AddChipFields((byte *)fpPTStruct.Field55, &etdData);
/*			Trans_PassEMVData(&fpPTStruct, &etdData);*/
		}
/* Toygar - NextEMV Start - Ok */
/*
		else 
		{ 
			Mainloop_XLSRemoveChipCard(); return; 
		}
*/
/* Toygar - NextEMV End - Ok */
	}
/* Toygar-EMVed */

	if(Host_PreDial() != STAT_OK) 
	{
		Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
		return(STAT_NOK);
	}

/*
	Printer_WriteStr("input_type#8\n");
	PrntUtil_BufAsciiHex((byte *)&input_type,1);
*/
	tran_type = SALE;
	if (axesscardentry)
	{
		input_type = INPUT_FROM_CHIPCARD;
		tran_ret = Trans_Transaction(0,0,0,0,0,0,0,0,0,0,FALSE,FALSE);
	}
	else 
	{
/*		if (POSEntryFlag > 0) input_type = INPUT_FROM_READER_22;*/
		if (POSEntryFlagUsed)
		{
			input_type = INPUT_FROM_READER_22;
			POSEntryFlag = 0;
		}
		tran_ret = Trans_Transaction(pAppData, &fpAppList, &pMutualApps, &pbMutualAppIndex, &lsTerminalDataStruct, &pelsLangs, &pbErr, &rdSDAData, &etdData, &fpPTStruct, TRUE);
	}
/*	tran_ret = Trans_Transaction();*/
	if(Files_WriteAllParams() != STAT_OK) Mainloop_DisplayWait("PARAMETRELER YAZILAMADI",1);
	switch (tran_ret)
	{
	case    STAT_OK:
		break;
	case    AKNET_POS_TIMEOUT_ERROR:
		Mainloop_DisplayWait("POS TIMEOUT",2);
		break;
	case    STAT_NOK:
		Mainloop_DisplayWait(" TAMAMLANAMADI  TEKRAR DENEY˜N˜Z",2);
		break;
	case    TRR_NO_CONNECTION:
		Mainloop_DisplayWait("  G™NDER˜LEMED˜ ",2);
		break;
	}
	Host_Disconnect();
	EmvIF_ClearDsp(DSP_MERCHANT);
	return(STAT_OK);
}
#endif
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_XLSRemoveChipCard.
 *
 * DESCRIPTION  : Read Chip Card to get track2 data.
 *
 * RETURN       : none
 *
 * NOTES        : LOYALTY R.Ý.Ö. 18/07/2001 
 *
 * --------------------------------------------------------------------------*/
void Mainloop_XLSRemoveChipCard(void)
{
	byte lbStatus;

	lbStatus = ResetCardExt();
	while((lbStatus == STAT_OK) || (lbStatus == STAT_MAY)) /* 06_05 06_20 06_30 emv kapatma islemi icin cikartýldý */
	{
		Mainloop_DisplayWait("   KARTINIZI      CIKARTINIZ    ",NO_WAIT);
		Mte_Wait(300);
		Kb_Read ();
		Kbhw_BuzzerPulse(200);
		lbStatus = ResetCardExt();
		Display_ClrDsp();
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_XLSIdleLoop.
 *
 * DESCRIPTION  : XLS Idle Loop Operations
 *
 * RETURN       : none
 *
 * NOTES        : LOYALTY R.Ý.Ö. 27/07/2001
 *
 * --------------------------------------------------------------------------*/
void Mainloop_XLSIdleLoop(void)
{
	char outTimerAck[13]; /* Auto. XLS_EOD Time. 'CCYYMMDDHHMM\0' */
	char today_date_time[13]; /* todays date and time 'CCYYMMDDHHMM\0' */
	boolean AUTO_XLS_EOD_PERMISSION;
	char auto_date_c[9], auto_time_c[5];
	byte lbStatus;
	char zero_timestamp[13];	/** "000000000000\0" seklinde kullanilacak **/
	boolean XLS_EOD_TIME_IS_UP;
	TerminalAppList pTerminalApps;
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
	EMVLanguages pelsLangs;
	byte pbErr;
	
	lbStatus = STAT_NOK;
	switch (ICC_Status(0))
   	{
	case ICC_NEEDS_RESET	:
		lbStatus = ResetCardExt();
		if (lbStatus == STAT_NOK)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("MANYETIK KART   OKUYUCU KULLANIN",1);
			EmvIF_ClearDsp(DSP_MERCHANT);
			BadICC = 1;
			break;
		}
	case ICC_OK		: 
		BadICC=0;
		break;
	case ICC_BAD_CARD	: 
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("CHIP BOZUK      KARTI GE€˜R˜N˜Z ",1);
		EmvIF_ClearDsp(DSP_MERCHANT);
		BadICC=1;
		break;
	case ICC_SOFTWARE_ERROR	: 
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("TEKNIK HATA     KARTI GE€˜R˜N˜Z ",1);
		EmvIF_ClearDsp(DSP_MERCHANT);
		BadICC=1;
		break;
	case ICC_CARD_BLOCKED	: 
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("CHIP BLOKE      KARTI GE€˜R˜N˜Z ",1);
		EmvIF_ClearDsp(DSP_MERCHANT);
		BadICC=1;
		break;
	default			: ;
		lbStatus = STAT_NOK;
	}
	if ( lbStatus != STAT_OK ) lbStatus = ResetCardExtLight(  );
	if ( lbStatus == STAT_OK )
	{

		Files_ReadEMVAppParam( &pTerminalApps );
		memcpy( &pelsLangs, &elsLangs, sizeof( pelsLangs ) );
/*		Iso78164_Init(  );*/
		EmvIF_ClearDsp( DSP_MERCHANT );
		Mainloop_DisplayWait( (char*)csEMV_PLEASE_WAIT, NO_WAIT );
		pbErr = 0;
		if ( Main_ApplicationSelection( &pTerminalApps, &pMutualApps, &pbMutualAppIndex, &lsTerminalDataStruct, &pelsLangs, &pbErr ) != STAT_OK )
		{
			lbStatus = ResetCardExt(  );
			if (lbStatus == STAT_OK)
			{

				if ( ( POS_Type != '3' ) && ( POS_Type != '5' ) )
				{
					byte tran_ret;

					CardRdr_Disable(  );
					ResetCardExt(  );

					if ( E3744_SelectRead(  ) == STAT_OK )
					{
						if ( Host_PreDial(  ) != STAT_OK )
						{
							Mainloop_DisplayWait( "     ARAMA           HATASI     ", 1 );
							Mainloop_XLSRemoveChipCard();
							return;
						}
						tran_type = SALE;
						input_type = INPUT_FROM_CHIPCARD;
						tran_ret = Trans_Transaction( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FALSE, FALSE );
						if ( Files_WriteAllParams(  ) != STAT_OK ) Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
						switch ( tran_ret )
						{
						case STAT_OK:
							break;
						case AKNET_POS_TIMEOUT_ERROR:
							Mainloop_DisplayWait( "POS TIMEOUT", 2 );
							break;
						case STAT_NOK:
							Mainloop_DisplayWait( " TAMAMLANAMADI  TEKRAR DENEY˜N˜Z", 2 );
							break;
						case TRR_NO_CONNECTION:
							Mainloop_DisplayWait( "  G™NDER˜LEMED˜ ", 2 );
							break;
						}
						Host_Disconnect(  );
					}
					Mainloop_XLSRemoveChipCard(  );
					EmvIF_ClearDsp(DSP_MERCHANT);
				}
				else
				{
					Mainloop_XLSRemoveChipCard(  );
					EmvIF_ClearDsp(DSP_MERCHANT);
					return;
				}
			}
			else 
			{
				Mainloop_XLSRemoveChipCard(  );
				EmvIF_ClearDsp(DSP_MERCHANT);
				return;
			}
		}
		else
		{
			byte tran_ret;
			EMVTransData etdData;
			RowData rdSDAData;
			PTStruct fpPTStruct;
			byte rv;
			boolean axesscardentry;
			ApplicationParams *pAppData;

			CardRdr_Disable(  );
			pAppData = &( pTerminalApps.TermApp[pMutualApps.MutualApps[pbMutualAppIndex].TermAppIndex] );
			fpPTStruct.Field55Len = Trans_AddChipFields( ( byte * ) fpPTStruct.Field55, &etdData );
			if ( Main_ReadApplicationData( &pbErr ) != STAT_OK )
			{
				Mainloop_XLSRemoveChipCard(  );
				EmvIF_ClearDsp(DSP_MERCHANT);
				return;
			}
			fpPTStruct.Field55Len = Trans_AddChipFields( ( byte * ) fpPTStruct.Field55, &etdData );
			if ( Main_DAMethod( pAppData, &pbErr ) != STAT_OK )
			{
				Mainloop_XLSRemoveChipCard(  );
				EmvIF_ClearDsp(DSP_MERCHANT);
				return;
			}
			fpPTStruct.Field55Len = Trans_AddChipFields( ( byte * ) fpPTStruct.Field55, &etdData );
			/* 20.07.2004 */
			if ( EMVAxess && !EMVAxessStatu ) tran_type = SALE;
			if ( EMVAxess && !EMVAxessStatu )
			{
				if ( !EMVAxessGetAmount(  ) )
				{
					Mainloop_DisplayWait( "EMVAxessGetAmount Hatasi", 3 );
					EmvIF_ClearDsp(DSP_MERCHANT);
					EMVAxessStatu = FALSE;
				}
				else
				{
					EMVAxessStatu = TRUE;
				}
				return;
			}

			if ( Host_PreDial(  ) != STAT_OK )
			{
				Mainloop_DisplayWait( "     ARAMA           HATASI     ", 1 );
				Mainloop_XLSRemoveChipCard(  );
				EmvIF_ClearDsp(DSP_MERCHANT);
				return;
			}

			if ( POSEntryFlagUsed )
			{
				input_type = INPUT_FROM_READER_22;
				POSEntryFlag = 0;
			}
			tran_ret = Trans_Transaction( pAppData, &pTerminalApps, &pMutualApps, &pbMutualAppIndex, &lsTerminalDataStruct, &pelsLangs, &pbErr, &rdSDAData, &etdData, &fpPTStruct, TRUE, FALSE );
			if ( Files_WriteAllParams(  ) != STAT_OK ) Mainloop_DisplayWait( "PARAMETRELER YAZILAMADI", 1 );
			switch ( tran_ret )
			{
			case STAT_OK:
				break;
			case AKNET_POS_TIMEOUT_ERROR:
				Mainloop_DisplayWait( "POS TIMEOUT", 2 );
				break;
			case STAT_NOK:
				Mainloop_DisplayWait( " TAMAMLANAMADI  TEKRAR DENEY˜N˜Z", 2 );
				break;
			case TRR_NO_CONNECTION:
				Mainloop_DisplayWait( "  G™NDER˜LEMED˜ ", 2 );
				break;
			}

			/* 20.07.2004 axess visa degisiklikleri */
			EMVAxessStatu = FALSE;
			tran_type = SALE; /*@bm 20.09.2004 */
			Host_Disconnect(  );
			Mainloop_XLSRemoveChipCard();
			EmvIF_ClearDsp(DSP_MERCHANT);

			/*@bm 20.09.2004 */
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			Kb_Insert(MMI_ESCAPE);
			/*@bm 20.09.2004 */
		}
	}
	else 
	{
		Mainloop_XLSRemoveChipCard(  );
		return;
	}
	XLS_GetTimerAck(outTimerAck);
	Utils_GetDateTime (auto_date_c, auto_time_c);
	memset(today_date_time, 0, sizeof(today_date_time));
	memcpy(today_date_time, "20",2);
	memcpy(&today_date_time[2], &auto_date_c[6],2);
	memcpy(&today_date_time[4], &auto_date_c[3],2);
	memcpy(&today_date_time[6], auto_date_c,2);
	memcpy(&today_date_time[8] , auto_time_c,2);
	memcpy(&today_date_time[10], &auto_time_c[3],2);
	today_date_time[12] = 0;
	memset(zero_timestamp, 0, sizeof(zero_timestamp));
	memcpy(zero_timestamp, "000000000000", 12);
	if ( memcmp(today_date_time, "200", 3)==0)
		if ( (Utils_Compare_Strings(today_date_time, outTimerAck) == 1) &&  (memcmp(outTimerAck, zero_timestamp, 12) != 0) ) XLS_EOD_TIME_IS_UP = TRUE;
		else XLS_EOD_TIME_IS_UP = FALSE;
	
	if ( (XLS_EOD_TIME_IS_UP) && (XLS_Check_Initialization_State() != XLS_NOT_INITIALIZED) )
	{
/*		Printer_WriteStr("XLS_EOD(XLS_CALL_ACK_READ)\n");*/
		XLS_EOD(XLS_CALL_ACK_READ);
	}
}




/** XLS_INTEGRATION2 05/11/2001 irfan. animation logo **/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Mainloop_IdleLogoAnimation
 *
 * DESCRIPTION          : Displays te Animation logo in the idle prompt.
 *
 * RETURN               : none
 *
 * NOTES                : Lipman and r.i.o 05/11/01
 *                        type_of_medi  - > P for Printer logo
 *                                      - > D for Display logo
 *
 * --------------------------------------------------------------------------*/
/* yeni logo eklendi. 06_32 08/05/2003 */
/* 04_15 23/06/2003 biz.card logo */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Mainloop_BizIdleLogo
 *
 * DESCRIPTION          : Displays te biz.card logo in the idle prompt.
 *
 * RETURN               : none
 *
 * NOTES                : Lipman and r.i.o 19/01/01
 *
 * --------------------------------------------------------------------------*/
void Mainloop_BizIdleLogo(void)
{
	 byte F01[] = { 
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE3, 0xE0, 0x1C, 0x3F, 0xFF, 0x00, 0x00, 0x7E, 0x00, 0x7F, 0xC0, 0x73, 0xF0, 0x0F, 0x8E,
	 0xEF, 0xF8, 0x1C, 0x3F, 0xFF, 0x00, 0x01, 0xFF, 0x81, 0xFF, 0xE0, 0x77, 0xF8, 0x3F, 0xEE,
	 0xFF, 0xFC, 0x1C, 0x3F, 0xFF, 0x00, 0x03, 0xFF, 0xC3, 0xFF, 0xF0, 0x7F, 0xFC, 0x7F, 0xFE,
	 0xFC, 0x3E, 0x1C, 0x00, 0x0E, 0x00, 0x07, 0xC1, 0xE1, 0xE0, 0xF8, 0x7E, 0x38, 0xF8, 0x7E,
	 0xF8, 0x0E, 0x1C, 0x00, 0x1E, 0x00, 0x07, 0x00, 0xF0, 0xC0, 0x38, 0x78, 0x10, 0xE0, 0x1E,
	 0xF0, 0x0F, 0x1C, 0x00, 0x3C, 0x00, 0x0F, 0x00, 0x60, 0x00, 0x38, 0x70, 0x00, 0xE0, 0x1E,
	 0xE0, 0x07, 0x1C, 0x00, 0x78, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x00, 0xF0, 0x00, 0x0E, 0x00, 0x00, 0x01, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x01, 0xE0, 0x00, 0x0E, 0x00, 0x00, 0x3F, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x03, 0xC0, 0x00, 0x0E, 0x00, 0x00, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x07, 0x80, 0x00, 0x0E, 0x00, 0x01, 0xFE, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x0F, 0x00, 0x00, 0x0E, 0x00, 0x03, 0xC0, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xF0, 0x0F, 0x1C, 0x1E, 0x00, 0x00, 0x0F, 0x00, 0x63, 0x80, 0x38, 0x70, 0x00, 0xE0, 0x1E,
	 0xF0, 0x1E, 0x1C, 0x3C, 0x00, 0x00, 0x07, 0x00, 0xF3, 0x80, 0x78, 0x70, 0x00, 0xF0, 0x1E,
	 0xFC, 0x3E, 0x1C, 0x38, 0x00, 0x00, 0x07, 0xC1, 0xE3, 0xC1, 0xF8, 0x70, 0x00, 0xF8, 0x7E,
	 0xFF, 0xFC, 0x1C, 0x7F, 0xFF, 0x0E, 0x03, 0xFF, 0xC1, 0xFF, 0xF8, 0x70, 0x00, 0x7F, 0xFE,
	 0xEF, 0xF8, 0x1C, 0x7F, 0xFF, 0x0E, 0x01, 0xFF, 0x81, 0xFF, 0xB8, 0x70, 0x00, 0x3F, 0xEE,
	 0xE3, 0xE0, 0x1C, 0x7F, 0xFF, 0x0E, 0x00, 0x7E, 0x00, 0x7E, 0x38, 0x70, 0x00, 0x0F, 0x8E
	 };
 
	 byte F02 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE3, 0xE0, 0x1C, 0x3F, 0xFF, 0x00, 0x00, 0x7E, 0x00, 0x7F, 0xC0, 0x73, 0xF0, 0x0F, 0x8E,
	 0xEF, 0xF8, 0x1C, 0x3F, 0xFF, 0x00, 0x01, 0xFF, 0x81, 0xFF, 0xE0, 0x77, 0xF8, 0x3F, 0xEE,
	 0xFF, 0xFC, 0x1C, 0x3F, 0xFF, 0x00, 0x03, 0xFF, 0xC3, 0xFF, 0xF0, 0x7F, 0xFC, 0x7F, 0xFE,
	 0xFC, 0x3E, 0x1C, 0x00, 0x1E, 0x00, 0x07, 0xC1, 0xF1, 0xE0, 0xF8, 0x7E, 0x38, 0xF8, 0x7E,
	 0xF0, 0x0F, 0x1C, 0x00, 0x3C, 0x00, 0x0F, 0x00, 0x60, 0x00, 0x38, 0x70, 0x00, 0xE0, 0x1E,
	 0xE0, 0x07, 0x1C, 0x00, 0x78, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x00, 0xF0, 0x00, 0x0E, 0x00, 0x00, 0x01, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x03, 0xE0, 0x00, 0x0E, 0x00, 0x00, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x07, 0x80, 0x00, 0x0E, 0x00, 0x01, 0xFE, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x0F, 0x00, 0x00, 0x0E, 0x00, 0x03, 0xC0, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xF0, 0x0F, 0x1C, 0x1E, 0x00, 0x00, 0x0F, 0x00, 0x63, 0x80, 0x38, 0x70, 0x00, 0xE0, 0x1E,
	 0xFC, 0x3E, 0x1C, 0x3C, 0x00, 0x00, 0x07, 0xC1, 0xF3, 0xC1, 0xF8, 0x70, 0x00, 0xF8, 0x7E,
	 0xFF, 0xFC, 0x1C, 0x7F, 0xFF, 0x0E, 0x03, 0xFF, 0xC1, 0xFF, 0xF8, 0x70, 0x00, 0x7F, 0xFE,
	 0xEF, 0xF8, 0x1C, 0x7F, 0xFF, 0x0E, 0x01, 0xFF, 0x81, 0xFF, 0xB8, 0x70, 0x00, 0x3F, 0xEE,
	 0xE3, 0xE0, 0x1C, 0x7F, 0xFF, 0x0E, 0x00, 0x7E, 0x00, 0x7E, 0x38, 0x70, 0x00, 0x0F, 0x8E,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte F03 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xEF, 0xF8, 0x1C, 0x3F, 0xFF, 0x00, 0x01, 0xFF, 0x81, 0xFF, 0xE0, 0x77, 0xF8, 0x3F, 0xEE,
	 0xFF, 0xFC, 0x1C, 0x3F, 0xFF, 0x00, 0x03, 0xFF, 0xC3, 0xFF, 0xF0, 0x7F, 0xFC, 0x7F, 0xFE,
	 0xFC, 0x3E, 0x1C, 0x00, 0x1E, 0x00, 0x07, 0xC1, 0xF1, 0xE0, 0xF8, 0x7E, 0x38, 0xF8, 0x7E,
	 0xF0, 0x0F, 0x1C, 0x00, 0x7C, 0x00, 0x0F, 0x00, 0x60, 0x00, 0x38, 0x70, 0x01, 0xE0, 0x1E,
	 0xE0, 0x07, 0x1C, 0x00, 0xF0, 0x00, 0x0E, 0x00, 0x00, 0x01, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x03, 0xE0, 0x00, 0x0E, 0x00, 0x00, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x0F, 0x80, 0x00, 0x0E, 0x00, 0x03, 0xFE, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xF0, 0x0F, 0x1C, 0x1E, 0x00, 0x00, 0x0F, 0x00, 0x63, 0x80, 0x38, 0x70, 0x00, 0xE0, 0x1E,
	 0xFC, 0x3E, 0x1C, 0x3C, 0x00, 0x00, 0x07, 0xC1, 0xF3, 0xC1, 0xF8, 0x70, 0x00, 0xF8, 0x7E,
	 0xFF, 0xFC, 0x1C, 0x7F, 0xFF, 0x0E, 0x03, 0xFF, 0xC1, 0xFF, 0xF8, 0x70, 0x00, 0x7F, 0xFE,
	 0xE3, 0xE0, 0x1C, 0x7F, 0xFF, 0x0E, 0x00, 0x7E, 0x00, 0x7E, 0x38, 0x70, 0x00, 0x0F, 0x8E,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte F04 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xEF, 0xF8, 0x1C, 0x3F, 0xFF, 0x00, 0x01, 0xFF, 0x81, 0xFF, 0xE0, 0x77, 0xF8, 0x3F, 0xEE,
	 0xFF, 0xFE, 0x1C, 0x3F, 0xFF, 0x00, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFC, 0xFF, 0xFE,
	 0xF8, 0x0F, 0x1C, 0x00, 0x7E, 0x00, 0x0F, 0x00, 0xF0, 0xC0, 0x38, 0x78, 0x11, 0xE0, 0x1E,
	 0xE0, 0x07, 0x1C, 0x01, 0xF0, 0x00, 0x0E, 0x00, 0x00, 0x3F, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x0F, 0xC0, 0x00, 0x0E, 0x00, 0x03, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xF0, 0x1F, 0x1C, 0x3E, 0x00, 0x00, 0x0F, 0x00, 0xF3, 0x80, 0x78, 0x70, 0x00, 0xF0, 0x1E,
	 0xFF, 0xFE, 0x1C, 0x7F, 0xFF, 0x0E, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x70, 0x00, 0xFF, 0xFE,
	 0xE3, 0xE0, 0x1C, 0x7F, 0xFF, 0x0E, 0x00, 0x7E, 0x00, 0x7E, 0x38, 0x70, 0x00, 0x0F, 0x8E,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte F05 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xFF, 0xFE, 0x1C, 0x3F, 0xFF, 0x00, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFC, 0xFF, 0xFE,
	 0xF8, 0x0F, 0x1C, 0x01, 0xFE, 0x00, 0x0F, 0x00, 0xF0, 0xFF, 0xF8, 0x78, 0x11, 0xE0, 0x1E,
	 0xF0, 0x1F, 0x1C, 0x3F, 0xC0, 0x00, 0x0F, 0x00, 0xF3, 0xFF, 0xF8, 0x70, 0x01, 0xF0, 0x1E,
	 0xFF, 0xFE, 0x1C, 0x7F, 0xFF, 0x0E, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x70, 0x00, 0xFF, 0xFE,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte F06 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xFF, 0xFE, 0x1C, 0x3F, 0xFF, 0x00, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFC, 0xFF, 0xFE,
	 0xF8, 0x1F, 0x1C, 0x3F, 0xFE, 0x00, 0x0F, 0x00, 0xF3, 0xFF, 0xF8, 0x78, 0x11, 0xF0, 0x1E,
	 0xFF, 0xFE, 0x1C, 0x7F, 0xFF, 0x0E, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x70, 0x00, 0xFF, 0xFE,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte FF1 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xFF, 0xFF, 0x1C, 0x7F, 0xFF, 0x0E, 0x0F, 0xFF, 0xF3, 0xFF, 0xF8, 0x70, 0x01, 0xFF, 0xFE,
	 0xFF, 0xFF, 0x1C, 0x3F, 0xFF, 0x00, 0x0F, 0xFF, 0xF3, 0xFF, 0xF8, 0x7F, 0xFD, 0xFF, 0xFE,
	 0xE3, 0xE0, 0x1C, 0x3F, 0xFF, 0x00, 0x00, 0x7E, 0x00, 0x7F, 0xC0, 0x73, 0xF0, 0x0F, 0x8E,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte FF2 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xEF, 0xF8, 0x1C, 0x7F, 0xFF, 0x0E, 0x01, 0xFF, 0x81, 0xFF, 0xB8, 0x70, 0x00, 0x3F, 0xEE,
	 0xFF, 0xFF, 0x1C, 0x7F, 0xFF, 0x0E, 0x0F, 0xFF, 0xF3, 0xFF, 0xF8, 0x70, 0x01, 0xFF, 0xFE,
	 0xE0, 0x07, 0x1C, 0x07, 0xF8, 0x00, 0x0E, 0x00, 0x01, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xFF, 0xFF, 0x1C, 0x3F, 0xFF, 0x00, 0x0F, 0xFF, 0xF3, 0xFF, 0xF8, 0x7F, 0xFC, 0xFF, 0xFE,
	 0xE3, 0xE0, 0x1C, 0x3F, 0xFF, 0x00, 0x00, 0x7E, 0x00, 0x7F, 0xC0, 0x73, 0xF0, 0x0F, 0x8E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte FF3 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xEF, 0xF8, 0x1C, 0x7F, 0xFF, 0x0E, 0x01, 0xFF, 0x81, 0xFF, 0xB8, 0x70, 0x00, 0x3F, 0xEE,
	 0xFF, 0xFE, 0x1C, 0x7F, 0xFF, 0x0E, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x70, 0x00, 0xFF, 0xFE,
	 0xF0, 0x1F, 0x1C, 0x3F, 0x00, 0x00, 0x0F, 0x00, 0xF3, 0xC0, 0x78, 0x70, 0x01, 0xF0, 0x1E,
	 0xE0, 0x07, 0x1C, 0x07, 0xC0, 0x00, 0x0E, 0x00, 0x01, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x01, 0xF8, 0x00, 0x0E, 0x00, 0x00, 0x3F, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xF8, 0x0F, 0x1C, 0x00, 0x3E, 0x00, 0x0F, 0x00, 0xF0, 0xC0, 0x38, 0x78, 0x10, 0xE0, 0x1E,
	 0xFF, 0xFE, 0x1C, 0x3F, 0xFF, 0x00, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFC, 0xFF, 0xFE,
	 0xE3, 0xE0, 0x1C, 0x3F, 0xFF, 0x00, 0x00, 0x7E, 0x00, 0x7F, 0xC0, 0x73, 0xF0, 0x0F, 0x8E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte FF4 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xE3, 0xE0, 0x1C, 0x7F, 0xFF, 0x0E, 0x00, 0x7E, 0x00, 0x7E, 0x38, 0x70, 0x00, 0x0F, 0x8E,
	 0xFF, 0xFC, 0x1C, 0x7F, 0xFF, 0x0E, 0x03, 0xFF, 0xC1, 0xFF, 0xF8, 0x70, 0x00, 0x7F, 0xFE,
	 0xFC, 0x3E, 0x1C, 0x38, 0x00, 0x00, 0x07, 0xC1, 0xE3, 0xC1, 0xF8, 0x70, 0x00, 0xF8, 0x7E,
	 0xF0, 0x1F, 0x1C, 0x3E, 0x00, 0x00, 0x0F, 0x00, 0xF3, 0x80, 0x78, 0x70, 0x00, 0xF0, 0x1E,
	 0xE0, 0x07, 0x1C, 0x0F, 0x80, 0x00, 0x0E, 0x00, 0x03, 0xFE, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x03, 0xC0, 0x00, 0x0E, 0x00, 0x00, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x01, 0xF0, 0x00, 0x0E, 0x00, 0x00, 0x3F, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xF0, 0x0F, 0x1C, 0x00, 0x7C, 0x00, 0x0F, 0x00, 0x60, 0x00, 0x38, 0x70, 0x01, 0xE0, 0x1E,
	 0xF8, 0x0E, 0x1C, 0x00, 0x1E, 0x00, 0x07, 0x00, 0xF0, 0xC0, 0x38, 0x78, 0x10, 0xE0, 0x1E,
	 0xFF, 0xFE, 0x1C, 0x3F, 0xFF, 0x00, 0x07, 0xFF, 0xE3, 0xFF, 0xF8, 0x7F, 0xFC, 0xFF, 0xFE,
	 0xEF, 0xF8, 0x1C, 0x3F, 0xFF, 0x00, 0x01, 0xFF, 0x81, 0xFF, 0xE0, 0x77, 0xF8, 0x3F, 0xEE,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte FF5 [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xE3, 0xE0, 0x1C, 0x7F, 0xFF, 0x0E, 0x00, 0x7E, 0x00, 0x7E, 0x38, 0x70, 0x00, 0x0F, 0x8E,
	 0xFF, 0xFC, 0x1C, 0x7F, 0xFF, 0x0E, 0x03, 0xFF, 0xC1, 0xFF, 0xF8, 0x70, 0x00, 0x7F, 0xFE,
	 0xFC, 0x3E, 0x1C, 0x38, 0x00, 0x00, 0x07, 0xC1, 0xE3, 0xC1, 0xF8, 0x70, 0x00, 0xF8, 0x7E,
	 0xF0, 0x1E, 0x1C, 0x3C, 0x00, 0x00, 0x07, 0x00, 0xF3, 0x80, 0x78, 0x70, 0x00, 0xF0, 0x1E,
	 0xF0, 0x0F, 0x1C, 0x1E, 0x00, 0x00, 0x0F, 0x00, 0x63, 0x80, 0x38, 0x70, 0x00, 0xE0, 0x1E,
	 0xE0, 0x07, 0x1C, 0x0F, 0x80, 0x00, 0x0E, 0x00, 0x03, 0xFE, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x03, 0xC0, 0x00, 0x0E, 0x00, 0x00, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x01, 0xE0, 0x00, 0x0E, 0x00, 0x00, 0x3F, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x00, 0xF0, 0x00, 0x0E, 0x00, 0x00, 0x01, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xF0, 0x0F, 0x1C, 0x00, 0x7C, 0x00, 0x0F, 0x00, 0x60, 0x00, 0x38, 0x70, 0x01, 0xE0, 0x1E,
	 0xF8, 0x0E, 0x1C, 0x00, 0x1E, 0x00, 0x07, 0x00, 0xF0, 0xC0, 0x38, 0x78, 0x10, 0xE0, 0x1E,
	 0xFC, 0x3E, 0x1C, 0x00, 0x0E, 0x00, 0x07, 0xC1, 0xE1, 0xE0, 0xF8, 0x7E, 0x38, 0xF8, 0x7E,
	 0xFF, 0xFC, 0x1C, 0x3F, 0xFF, 0x00, 0x03, 0xFF, 0xC3, 0xFF, 0xF0, 0x7F, 0xFC, 0x7F, 0xFE,
	 0xEF, 0xF8, 0x1C, 0x3F, 0xFF, 0x00, 0x01, 0xFF, 0x81, 0xFF, 0xE0, 0x77, 0xF8, 0x3F, 0xEE,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	 };
 
	 byte FF6 [] = { 
	 0xE3, 0xE0, 0x1C, 0x7F, 0xFF, 0x0E, 0x00, 0x7E, 0x00, 0x7E, 0x38, 0x70, 0x00, 0x0F, 0x8E,
	 0xEF, 0xF8, 0x1C, 0x7F, 0xFF, 0x0E, 0x01, 0xFF, 0x81, 0xFF, 0xB8, 0x70, 0x00, 0x3F, 0xEE,
	 0xFF, 0xFC, 0x1C, 0x7F, 0xFF, 0x0E, 0x03, 0xFF, 0xC1, 0xFF, 0xF8, 0x70, 0x00, 0x7F, 0xFE,
	 0xFC, 0x3E, 0x1C, 0x38, 0x00, 0x00, 0x07, 0xC1, 0xE3, 0xC1, 0xF8, 0x70, 0x00, 0xF8, 0x7E,
	 0xF0, 0x1E, 0x1C, 0x3C, 0x00, 0x00, 0x07, 0x00, 0xF3, 0x80, 0x78, 0x70, 0x00, 0xF0, 0x1E,
	 0xF0, 0x0F, 0x1C, 0x1E, 0x00, 0x00, 0x0F, 0x00, 0x63, 0x80, 0x38, 0x70, 0x00, 0xE0, 0x1E,
	 0xE0, 0x07, 0x1C, 0x0F, 0x00, 0x00, 0x0E, 0x00, 0x03, 0xC0, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x07, 0x80, 0x00, 0x0E, 0x00, 0x01, 0xFE, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x03, 0xC0, 0x00, 0x0E, 0x00, 0x00, 0xFF, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x01, 0xE0, 0x00, 0x0E, 0x00, 0x00, 0x3F, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x00, 0xF0, 0x00, 0x0E, 0x00, 0x00, 0x01, 0xF8, 0x70, 0x01, 0xC0, 0x0E,
	 0xE0, 0x07, 0x1C, 0x00, 0x78, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x38, 0x70, 0x01, 0xC0, 0x0E,
	 0xF0, 0x0F, 0x1C, 0x00, 0x3C, 0x00, 0x0F, 0x00, 0x60, 0x00, 0x38, 0x70, 0x00, 0xE0, 0x1E,
	 0xF8, 0x0E, 0x1C, 0x00, 0x1E, 0x00, 0x07, 0x00, 0xF0, 0xC0, 0x38, 0x78, 0x10, 0xE0, 0x1E,
	 0xFC, 0x3E, 0x1C, 0x00, 0x0E, 0x00, 0x07, 0xC1, 0xE1, 0xE0, 0xF8, 0x7E, 0x38, 0xF8, 0x7E,
	 0xFF, 0xFC, 0x1C, 0x3F, 0xFF, 0x00, 0x03, 0xFF, 0xC3, 0xFF, 0xF0, 0x7F, 0xFC, 0x7F, 0xFE,
	 0xEF, 0xF8, 0x1C, 0x3F, 0xFF, 0x00, 0x01, 0xFF, 0x81, 0xFF, 0xE0, 0x77, 0xF8, 0x3F, 0xEE,
	 0xE3, 0xE0, 0x1C, 0x3F, 0xFF, 0x00, 0x00, 0x7E, 0x00, 0x7F, 0xC0, 0x73, 0xF0, 0x0F, 0x8E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	 0xE0, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E
	 };

	GrphText_ClrArea (2, 20, 6, 120, FALSE);
	GrphText_ClrArea (5, 0, 7, 128, FALSE);
	GrphText_ClrArea (4, 120, 4, 128, FALSE);
        
		switch(rState)
        {
		case 0:GrphText_ShowImage ((byte *)F01,120,25,60,5,FALSE);break;
        case 1:
		case 2:GrphText_ShowImage ((byte *)F01,120,25,60,5,FALSE);break;
		case 3:
		case 4:GrphText_ShowImage ((byte *)F02,120,25,60,5,FALSE);break;
        case 5:
		case 6:GrphText_ShowImage ((byte *)F02,120,25,60,5,FALSE);break;
		case 7:
		case 8:GrphText_ShowImage ((byte *)F03,120,25,60,5,FALSE);break;
        case 9:
		case 10:GrphText_ShowImage ((byte *)F03,120,25,60,5,FALSE);break;
		case 11:
		case 12:GrphText_ShowImage ((byte *)F04,120,25,60,5,FALSE);break;
        case 13:
		case 14:GrphText_ShowImage ((byte *)F04,120,25,60,5,FALSE);break;
		case 15:
		case 16:GrphText_ShowImage ((byte *)F05,120,25,60,5,FALSE);break;
        case 17:
		case 18:GrphText_ShowImage ((byte *)F05,120,25,60,5,FALSE);break;
		case 19:
		case 20:GrphText_ShowImage ((byte *)F06,120,25,60,5,FALSE);break;
        case 21:
		case 22:GrphText_ShowImage ((byte *)F06,120,25,60,5,FALSE);break;
		case 23:
		case 24:GrphText_ShowImage ((byte *)FF1,120,25,60,5,FALSE);break;
        case 25:
		case 26:GrphText_ShowImage ((byte *)FF1,120,25,60,5,FALSE);break;
		case 27:
		case 28:GrphText_ShowImage ((byte *)FF2,120,25,60,5,FALSE);break;
        case 29:
		case 30:GrphText_ShowImage ((byte *)FF2,120,25,60,5,FALSE);break;
		case 31:
		case 32:GrphText_ShowImage ((byte *)FF3,120,25,60,5,FALSE);break;
        case 33:
		case 34:GrphText_ShowImage ((byte *)FF3,120,25,60,5,FALSE);break;
		case 35:
		case 36:GrphText_ShowImage ((byte *)FF4,120,25,60,5,FALSE);break;
        case 37:
		case 38:GrphText_ShowImage ((byte *)FF4,120,25,60,5,FALSE);break;
		case 39:
		case 40:GrphText_ShowImage ((byte *)FF5,120,25,60,5,FALSE);break;
        case 41:
		case 42:GrphText_ShowImage ((byte *)FF5,120,25,60,5,FALSE);break;
		case 43:
		case 44:GrphText_ShowImage ((byte *)FF6,120,25,60,5,FALSE);break;
        case 45:
		case 46:GrphText_ShowImage ((byte *)FF6,120,25,60,5,FALSE);break;
		case 47:
		case 48:GrphText_ShowImage ((byte *)FF5,120,25,60,5,FALSE);break;
        case 49:
		case 50:GrphText_ShowImage ((byte *)FF5,120,25,60,5,FALSE);break;
		case 51:
		case 52:GrphText_ShowImage ((byte *)FF4,120,25,60,5,FALSE);break;
        case 53:
		case 54:GrphText_ShowImage ((byte *)FF4,120,25,60,5,FALSE);break;
		case 55:
		case 56:GrphText_ShowImage ((byte *)FF3,120,25,60,5,FALSE);break;
        case 57:
		case 58:GrphText_ShowImage ((byte *)FF3,120,25,60,5,FALSE);break;
		case 59:
		case 60:GrphText_ShowImage ((byte *)FF2,120,25,60,5,FALSE);break;
        case 61:
		case 62:GrphText_ShowImage ((byte *)FF2,120,25,60,5,FALSE);break;
		case 63:
		case 64:GrphText_ShowImage ((byte *)FF1,120,25,60,5,FALSE);break;
        case 65:
		case 66:GrphText_ShowImage ((byte *)FF1,120,25,60,5,FALSE);break;
		case 67:
		case 68:GrphText_ShowImage ((byte *)F06,120,25,60,5,FALSE);break;
        case 69:
		case 70:GrphText_ShowImage ((byte *)F06,120,25,60,5,FALSE);break;
		case 71:
		case 72:GrphText_ShowImage ((byte *)F05,120,25,60,5,FALSE);break;
        case 73:
		case 74:GrphText_ShowImage ((byte *)F05,120,25,60,5,FALSE);break;
		case 75:
		case 76:GrphText_ShowImage ((byte *)F04,120,25,60,5,FALSE);break;
        case 77:
		case 78:GrphText_ShowImage ((byte *)F04,120,25,60,5,FALSE);break;
		case 79:
		case 80:GrphText_ShowImage ((byte *)F03,120,25,60,5,FALSE);break;
        case 81:
		case 82:GrphText_ShowImage ((byte *)F03,120,25,60,5,FALSE);break;
		case 83:
		case 84:GrphText_ShowImage ((byte *)F02,120,25,60,5,FALSE);break;
        case 85:
		case 86:GrphText_ShowImage ((byte *)F02,120,25,60,5,FALSE);break;
		case 87:
		case 88:GrphText_ShowImage ((byte *)F01,120,25,60,5,FALSE);break;
		default:GrphText_ShowImage ((byte *)F01,120,25,60,5,FALSE);break;

        }
        GrphText_DrawDisp ();

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Mainloop_IdleLogoAnimation
 *
 * DESCRIPTION: Displays te logo in the idle prompt.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_IdleLogoAnimationNew(boolean myLogoCount)
{
	byte a1Disp [] = {
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x03, 0x1A, 0xCF, 0x9F, 0x7C, 0x00,
	 0x00, 0x00, 0x07, 0x8D, 0x98, 0xDB, 0x6C, 0x00,
	 0x00, 0x00, 0x07, 0x87, 0x18, 0xDC, 0x70, 0x00,
	 0x00, 0x00, 0x0C, 0xC3, 0x1F, 0xCE, 0x38, 0x00,
	 0x00, 0x00, 0x0F, 0xC7, 0x98, 0x03, 0x0C, 0x00,
	 0x00, 0x00, 0x1F, 0xED, 0xD8, 0xD3, 0x4C, 0x00,
	 0x00, 0x00, 0x38, 0x78, 0xEF, 0x9F, 0x7C, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


	 byte a2Disp [] = {
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0xFF, 0xFC, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x03, 0xFF, 0xFE, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x07, 0xFF, 0xFF, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0x07, 0xF8, 0x7F, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0x0F, 0xE0, 0x1F, 0xC0, 0x00, 0x00,
	 0x00, 0x00, 0x1F, 0xC0, 0x0F, 0xC0, 0x00, 0x00,
	 0x00, 0x00, 0x1F, 0x80, 0x07, 0xE0, 0x00, 0x00,
	 0x00, 0x00, 0x1F, 0x00, 0x03, 0xE0, 0x00, 0x00,
	 0x00, 0x00, 0x1F, 0x1F, 0xC3, 0xE0, 0x00, 0x00,
	 0x00, 0x00, 0x1F, 0x0F, 0x83, 0xE0, 0x00, 0x00,
	 0x00, 0x00, 0x1C, 0x07, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x1B, 0x1A, 0xCF, 0x9F, 0x7C, 0x00,
	 0x00, 0x00, 0x17, 0x8D, 0x98, 0xDB, 0x6C, 0x00,
	 0x00, 0x00, 0x17, 0x87, 0x18, 0xDC, 0x70, 0x00,
	 0x00, 0x00, 0x0C, 0xC3, 0x1F, 0xCE, 0x38, 0x00,
	 0x00, 0x00, 0x0F, 0xC7, 0x98, 0x03, 0x0C, 0x00,
	 0x00, 0x00, 0x1F, 0xED, 0xD8, 0xD3, 0x4C, 0x00,
	 0x00, 0x00, 0x38, 0x78, 0xEF, 0x9F, 0x7C, 0x00,
	 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x7F, 0xF8, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
       
	 byte a3Disp [] = {
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0xFF, 0xFC, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x03, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
	 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xF0, 0x00, 0x00,
	 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xF8, 0x00, 0x00,
	 0x00, 0x00, 0xFF, 0x80, 0x07, 0xFC, 0x00, 0x00,
	 0x00, 0x01, 0xFE, 0x00, 0x01, 0xFE, 0x00, 0x00,
	 0x00, 0x03, 0xF8, 0x00, 0x00, 0x7F, 0x00, 0x00,
	 0x00, 0x07, 0xF0, 0x07, 0x80, 0x3F, 0x80, 0x00,
	 0x00, 0x0F, 0xE0, 0x3F, 0xF0, 0x1F, 0xC0, 0x00,
	 0x00, 0x0F, 0xC0, 0xFF, 0xFC, 0x0F, 0xC0, 0x00,
	 0x00, 0x1F, 0x83, 0xFF, 0xFE, 0x07, 0xE0, 0x00,
	 0x00, 0x1F, 0x07, 0xFF, 0xFF, 0x87, 0xE0, 0x00,
	 0x00, 0x3F, 0x07, 0xF8, 0x7F, 0x83, 0xF0, 0x00,
	 0x00, 0x3E, 0x0F, 0xE0, 0x1F, 0xC1, 0xF0, 0x00,
	 0x00, 0x3E, 0x1F, 0xC0, 0x0F, 0xC1, 0xF0, 0x00,
	 0x00, 0x7E, 0x1F, 0x80, 0x07, 0xE1, 0xF8, 0x00,
	 0x00, 0x7E, 0x1F, 0x00, 0x03, 0xE1, 0xF8, 0x00,
	 0x00, 0x7C, 0x1F, 0x1F, 0xC3, 0xE1, 0xF8, 0x00,
	 0x00, 0x7C, 0x1F, 0x0F, 0x83, 0xE1, 0xF8, 0x00,
	 0x00, 0x7C, 0x1C, 0x07, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x7C, 0x1B, 0x1A, 0xCF, 0x9F, 0x7C, 0x00,
	 0x00, 0x7E, 0x17, 0x8D, 0x98, 0xDB, 0x6C, 0x00,
	 0x00, 0x7E, 0x17, 0x87, 0x18, 0xDC, 0x70, 0x00,
	 0x00, 0x3E, 0x0C, 0xC3, 0x1F, 0xCE, 0x38, 0x00,
	 0x00, 0x3E, 0x0F, 0xC7, 0x98, 0x03, 0x0C, 0x00,
	 0x00, 0x3F, 0x1F, 0xED, 0xD8, 0xD3, 0x4C, 0x00,
	 0x00, 0x1F, 0x38, 0x78, 0xEF, 0x9F, 0x7C, 0x00,
	 0x00, 0x1F, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x0F, 0xC1, 0xFF, 0xFF, 0x8F, 0xE0, 0x00,
	 0x00, 0x0F, 0xE0, 0x7F, 0xF8, 0x1F, 0xC0, 0x00,
	 0x00, 0x07, 0xF0, 0x07, 0x80, 0x3F, 0x80, 0x00,
	 0x00, 0x03, 0xF8, 0x00, 0x00, 0x7F, 0x00, 0x00,
	 0x00, 0x03, 0xFE, 0x00, 0x01, 0xFE, 0x00, 0x00,
	 0x00, 0x01, 0xFF, 0x80, 0x07, 0xFC, 0x00, 0x00,
	 0x00, 0x00, 0x7F, 0xF8, 0x7F, 0xF8, 0x00, 0x00,
	 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xF0, 0x00, 0x00,
	 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
	 0x00, 0x00, 0x07, 0xFF, 0xFF, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0xFF, 0xFC, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
       
	 byte a4Disp [] = {
	 0x00, 0x00, 0x00, 0xFF, 0xFC, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xF0, 0x00, 0x00,
	 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00,
	 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	 0x00, 0x0F, 0xFF, 0x80, 0x07, 0xFF, 0x80, 0x00,
	 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x7F, 0xC0, 0x00,
	 0x00, 0x3F, 0xE0, 0x00, 0x00, 0x1F, 0xF0, 0x00,
	 0x00, 0x7F, 0x80, 0x00, 0x00, 0x07, 0xF8, 0x00,
	 0x00, 0xFF, 0x00, 0xFF, 0xFC, 0x03, 0xFC, 0x00,
	 0x01, 0xFC, 0x03, 0xFF, 0xFF, 0x00, 0xFE, 0x00,
	 0x03, 0xF8, 0x0F, 0xFF, 0xFF, 0xC0, 0x7F, 0x00,
	 0x07, 0xF0, 0x3F, 0xFF, 0xFF, 0xF0, 0x3F, 0x80,
	 0x07, 0xE0, 0x7F, 0xFF, 0xFF, 0xF8, 0x1F, 0x80,
	 0x0F, 0xC0, 0xFF, 0x80, 0x07, 0xFC, 0x0F, 0xC0,
	 0x1F, 0x81, 0xFE, 0x00, 0x01, 0xFE, 0x0F, 0xE0,
	 0x1F, 0x83, 0xF8, 0x00, 0x00, 0x7F, 0x07, 0xE0,
	 0x3F, 0x07, 0xF0, 0x07, 0x80, 0x3F, 0x83, 0xF0,
	 0x3F, 0x0F, 0xE0, 0x3F, 0xF0, 0x1F, 0xC3, 0xF0,
	 0x7E, 0x0F, 0xC0, 0xFF, 0xFC, 0x0F, 0xC1, 0xF0,
	 0x7E, 0x1F, 0x83, 0xFF, 0xFE, 0x07, 0xE1, 0xF8,
	 0x7C, 0x1F, 0x07, 0xFF, 0xFF, 0x87, 0xE0, 0xF8,
	 0x7C, 0x3F, 0x07, 0xF8, 0x7F, 0x83, 0xF0, 0xF8,
	 0xFC, 0x3E, 0x0F, 0xE0, 0x1F, 0xC1, 0xF0, 0xFC,
	 0xFC, 0x3E, 0x1F, 0xC0, 0x0F, 0xC1, 0xF0, 0xFC,
	 0xF8, 0x7E, 0x1F, 0x80, 0x07, 0xE1, 0xF8, 0x7C,
	 0xF8, 0x7E, 0x1F, 0x00, 0x03, 0xE1, 0xF8, 0x7C,
	 0xF8, 0x7C, 0x1F, 0x1F, 0xC3, 0xE1, 0xF8, 0x7C,
	 0xF8, 0x7C, 0x1F, 0x0F, 0x83, 0xE1, 0xF8, 0x7C,
	 0xF8, 0x7C, 0x1C, 0x07, 0x00, 0x00, 0x00, 0x7C,
	 0xF8, 0x7C, 0x1B, 0x1A, 0xCF, 0x9F, 0x7C, 0x7C,
	 0xF8, 0x7E, 0x17, 0x8D, 0x98, 0xDB, 0x6C, 0x7C,
	 0xF8, 0x7E, 0x17, 0x87, 0x18, 0xDC, 0x70, 0x7C,
	 0xF8, 0x3E, 0x0C, 0xC3, 0x1F, 0xCE, 0x38, 0xFC,
	 0xFC, 0x3E, 0x0F, 0xC7, 0x98, 0x03, 0x0C, 0xFC,
	 0x7C, 0x3F, 0x1F, 0xED, 0xD8, 0xD3, 0x4C, 0xF8,
	 0x7C, 0x1F, 0x38, 0x78, 0xEF, 0x9F, 0x7C, 0xF8,
	 0x7E, 0x1F, 0x83, 0x00, 0x00, 0x00, 0x01, 0xF8,
	 0x7E, 0x0F, 0xC1, 0xFF, 0xFF, 0x8F, 0xE1, 0xF0,
	 0x3F, 0x0F, 0xE0, 0x7F, 0xF8, 0x1F, 0xC3, 0xF0,
	 0x3F, 0x07, 0xF0, 0x07, 0x80, 0x3F, 0x83, 0xF0,
	 0x1F, 0x83, 0xF8, 0x00, 0x00, 0x7F, 0x07, 0xE0,
	 0x1F, 0x83, 0xFE, 0x00, 0x01, 0xFE, 0x07, 0xE0,
	 0x0F, 0xC1, 0xFF, 0x80, 0x07, 0xFC, 0x0F, 0xC0,
	 0x07, 0xE0, 0x7F, 0xF8, 0x7F, 0xF8, 0x1F, 0x80,
	 0x07, 0xF0, 0x3F, 0xFF, 0xFF, 0xF0, 0x3F, 0x80,
	 0x03, 0xF8, 0x1F, 0xFF, 0xFF, 0xC0, 0x7F, 0x00,
	 0x01, 0xFC, 0x07, 0xFF, 0xFF, 0x80, 0xFE, 0x00,
	 0x00, 0xFE, 0x00, 0xFF, 0xFC, 0x01, 0xFC, 0x00,
	 0x00, 0x7F, 0x80, 0x07, 0x80, 0x07, 0xF8, 0x00,
	 0x00, 0x3F, 0xE0, 0x00, 0x00, 0x1F, 0xF0, 0x00,
	 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x7F, 0xE0, 0x00,
	 0x00, 0x0F, 0xFF, 0x00, 0x03, 0xFF, 0xC0, 0x00,
	 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00,
	 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xF0, 0x00, 0x00,
	 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0xFF, 0xFC, 0x00, 0x00, 0x00,
	};
       
	if (myLogoCount)
        {
	        Mte_Wait(100);
	GrphText_ClrArea (2, 30, 6, 100, FALSE);
	        switch(rState)
	        {
	        /*(byte *)&MONO_AXESS_f01Disp [0]*/
		case 0:
		case 1:GrphText_ShowImage ((byte*)&a1Disp[0], 64, 58, 24, 32, FALSE);break;
	        case 2:
		case 3:GrphText_ShowImage ((byte*)&a2Disp[0], 64, 58, 24, 32, FALSE);break;
	        case 4:
		case 5:GrphText_ShowImage ((byte*)&a3Disp[0], 64, 58, 24, 32, FALSE);break;
	        case 6:
		case 7:GrphText_ShowImage ((byte*)&a4Disp[0], 64, 58, 24, 32, FALSE);break;
		default:GrphText_ShowImage ((byte*)&a1Disp[0], 64, 58, 24, 32, FALSE);break;
	        }
	        GrphText_DrawDisp ();
		if(rState>7) rState=0;
		rState++;
	}
}

/* Toygar EMVed */
/* GSMPOS_04_01 16/02/2001 irfan */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSMMenu
 *
 * DESCRIPTION  : Displays the GSM Menu.
 *
 * RETURN               : none
 *
 * NOTES                : GSMPOS_04_01 R.Ý.Ö. 16/02/2001 
 *
 * --------------------------------------------------------------------------*/
void Mainloop_GSMMenu(void)
{
   entry csMENU_SISTEM_entry [] = {
				{"GSM/DIALUP", 0, Menu5_Entry  , 0},
				{"BAUDRATE AYARLARI", 0, Menu5_Entry0  , 0},
				{"SESL˜ ARAMA", 0, Menu5_Entry1  , 0},
				{"DATA ARAMA ", 0, Menu5_Entry5  , 0},
				{"SMS GONDERME", 0, Menu5_Entry2  , 0},
				{"SMS OKUMA   ", 0, Menu5_Entry3  , 0},

				{"SIM PIN OZELL˜KLER˜", 0, Menu5_Entry4  , 0},
/*				{"SIM DE¦˜žT˜R", 0, Menu5_Entry7  , 0},*/

				{"P˜L SEV˜YES˜", 0, Menu5_Entry6  , 0},
 				{"GSM / GPRS",0,Menu5_Entry8,0}			
  };
   menu csMENU_SISTEM_menu;

   csMENU_SISTEM_menu.header         = "   GSM MENšSš   ";
   csMENU_SISTEM_menu.no_of_entries  = 9;
   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = CYCLIC_MENU|DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

	Formater_GoMenu (&csMENU_SISTEM_menu, 0);
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_INITBaudRate.
 *
 * DESCRIPTION: receive sms
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_GSM_INITBaudRate(void)
{
    entry csMENU_GSMBAUD_entry []= {
									{"GSM_AUTOBAUD", 0, 0, 0},
									{"GSM_300_V21 ", 0, 0, 0},
									{"GSM_1200_V22", 0, 0, 0},
									{"GSM_1200_V23", 0, 0, 0},
									{"GSM_2400_V22BIS", 0, 0, 0},
									{"GSM_2400_V26TER", 0, 0, 0},
									{"GSM_4800_V32", 0, 0, 0},
									{"GSM_9600_V32", 0, 0, 0},
									{"GSM_7200_V32BIS", 0, 0, 0},
									{"GSM_12000_V32BIS", 0, 0, 0},
									{"GSM_14400_V32BIS", 0, 0, 0},									
									{"GSM_9600_V34", 0, 0, 0},
									{"GSM_12000_V34", 0, 0, 0},
									{"GSM_14400_V34", 0, 0, 0},
									{"GSM_19200_V34", 0, 0, 0},
									{"GSM_28800_V34", 0, 0, 0},
									{"GSM_300_V110", 0, 0, 0},
									{"GSM_1200_V110", 0, 0, 0},
									{"GSM_2400_V110", 0, 0, 0},
									{"GSM_4800_V110", 0, 0, 0},
									{"GSM_9600_V110", 0, 0, 0},
									{"GSM_12000_V110", 0, 0, 0},
									{"GSM_14400_V110", 0, 0, 0},
									{"GSM_19200_V110", 0, 0, 0},
									{"GSM_38400_V110", 0, 0, 0},
									{"GSM_UNKNOWN", 0, 0, 0}
									};
    menu csMENU_GSMBAUD;
	csMENU_GSMBAUD.header		    = "GSM Baud Rate";
	csMENU_GSMBAUD.no_of_entries	= 26;
    csMENU_GSMBAUD.menu_entries	    = &csMENU_GSMBAUD_entry[0];
    csMENU_GSMBAUD.mode_flags	    = CYCLIC_MENU|DEFAULT_MODE;
	switch (Formater_GoMenu(&csMENU_GSMBAUD,CHOICE_MODE))
	{
		case 1 :return GSM_AUTOBAUD;
		case 2 :return GSM_300_V21;
		case 3 :return GSM_1200_V22;
		case 4 :return GSM_1200_V23;
		case 5 :return GSM_2400_V22BIS;
		case 6 :return GSM_2400_V26TER;
		case 7 :return GSM_4800_V32;
		case 8 :return GSM_9600_V32;
		case 9 :return GSM_7200_V32BIS;
		case 10 :return GSM_12000_V32BIS;
		case 11 :return GSM_14400_V32BIS;
		case 12 :return GSM_9600_V34;
		case 13 :return GSM_12000_V34;
		case 14 :return GSM_14400_V34;
		case 15 :return GSM_19200_V34;
		case 16 :return GSM_28800_V34;
		case 17 :return GSM_300_V110;
		case 18 :return GSM_1200_V110;
		case 19 :return GSM_2400_V110;
		case 20 :return GSM_4800_V110;
		case 21 :return GSM_9600_V110;
		case 22 :return GSM_12000_V110;
		case 23 :return GSM_14400_V110;
		case 24 :return GSM_19200_V110;
		case 25 :return GSM_38400_V110;
		case 26 :return GSM_UNKNOWN;
		default:return  GSM_9600_V110;
	}
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_NOSInit.
 *
 * DESCRIPTION: 
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_GSM_NOSInit(byte flag,byte staistFlag)
{
	GSMRadioParamStruct	MPrmBuffer;
	GSMRadioParamStruct	*MPrm;
	char buffer[LEN_GETKEYBOARD];
	int strLen;
	ulint Timeout=Scan_MillSec();
	RadioDescrStr Radio;
	byte statusFlag;
	
	oldbatlevel=0;
	sarj=FALSE;
	
	ComHndlr_RadioOff();
	memset(buffer,0,LEN_GETKEYBOARD);
		memset(&MPrmBuffer,0,sizeof(MPrmBuffer));
		memset(&MPrmBuffer,0xff,LEN_GETKEYBOARD);
	MPrmBuffer.PswdControlFlag=GSM_NOS_PIN_CTRL;
	if( flag )	
	{
		GSMDialPrm.Rate=GSM_9600_V110;
		if(staistFlag)
		{
			while(1)
			{
				memset(GsmPin,0,LEN_GETKEYBOARD);
				if(Mainloop_Edit(GsmPin,1))
				{
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait(" žEBEKE ARIYOR",0);
					break;
				}
			}
		}
		if(GsmPin[0]!=0)
			strcpy(MPrmBuffer.PIN,GsmPin);
/*		else
		strcpy(MPrm->PIN,"0000");*/
	}
	else
	{
		if(staistFlag)
			GSMDialPrm.Rate=GSM_9600_V110;
		else
		{
			GSMDialPrm.Rate=Mainloop_GSM_INITBaudRate();
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait(" žEBEKE ARIYOR",0);
		}
		strcpy(MPrmBuffer.PIN,GsmPin);		/*Pin*/
	}
	GSMDialPrm.TimeOut=25;
	GSMDialPrm.NumberOfAttempts=3;
	GSMDialPrm.CallType=GSM_CALL_DATA;

	if(ComHndlr_SetRadioDriverMode(COMHNDLR_RADIO_DRV_ENABLE)!=OK){}
	switch(ComHndlr_SetupRadio((void *)&MPrmBuffer,192))
	{
		case COMHNDLR_NO_SUPPORT:
		case FAIL_WRONG_INPUT:
		case FAILURE:	/*Printer_WriteStr("Params set fail \n");*/break;
		case OK:	/*Printer_WriteStr("Params set...OK \n");*/break;
		default:	/*Printer_WriteStr("Unknown error. \n");*/break;
	}
	Kb_Insert(MMI_ESCAPE);

	ComHndlr_RadioOn();
	while(!statusFlag)
	{
		if(Kb_Read()==MMI_ESCAPE) break;
		ComHndlr_GetRadioStatus(&Radio);
		if((Scan_MillSec()-Timeout) > 3000)
		{
			if (Radio.Rssi>10)
			{
				statusFlag=2;	
				break;
			}
			else
			if (Radio.Rssi>0)
				Timeout=Scan_MillSec();	
			
			switch(Radio.ConnectStatus)
			{
			case  RADIONET_REGISTERING:
			default:break;
			}
			if(((Scan_MillSec()-Timeout) == 30000))
			{

				ComHndlr_RadioOff();
				Mte_Wait(1000);
				ComHndlr_RadioOn();
			}
			if((Scan_MillSec()-Timeout) > 70000)
			{
				ComHndlr_RadioOff();
				Mte_Wait(1000);
				ComHndlr_RadioOn();
				statusFlag=1;
			}
		}
	}
	
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_Dialup.
 *
 * DESCRIPTION: 
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_GSM_Dialup(void)
{
/*	PrntUtil_BufAsciiHex((byte*)&GSMDialPrm,sizeof(GSMDialPrm));*/
	sint RetVal=ComHndlr_CallDirectRadio (&GSMDialPrm  , MASC_NORMAL_DATA );

	switch (RetVal) 
	{
		case SWITCHED_TO_RADIO:	/*stGSMSTATIST.siSWITCHED_TO_RADIO++;*/return STAT_OK;
/*		case FAIL_CONNECT:		stGSMSTATIST.siFAIL_CONNECT++;break;
		case FAIL_WRONG_INPUT:	stGSMSTATIST.siFAIL_WRONG_INPUT++;break;
		case USER_BREAK:		stGSMSTATIST.siUSER_BREAK++;break;
		case X_DIAL_UP_PRESSED:
		*/default:/*stGSMSTATIST.siX_DIAL_UP_PRESSED++;*/break;
	}
	return STAT_NOK;
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_PwrDownCnt
 *
 * DESCRIPTION:		PwrDownCnt FOR IDLE
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
BYTE Mainloop_PwrDownCnt(void)
{
	SystemStatistics1 SysPower;
	Statistics_System((void *)&SysPower, (usint)sizeof(SysPower), TRUE);
	if(SysPower.PwrDownCnt>0)	return FALSE;
	return TRUE;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSMStatistInit
 *
 * DESCRIPTION: 
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Mainloop_GSMStatistInit(void)
{/*
	memset(&stGSMSTATIST,0,sizeof(stGSMSTATIST));

	stGSMSTATIST.siSWITCHED_TO_RADIO= stGSMSTATIST.siFAIL_CONNECT=
	stGSMSTATIST.siFAIL_WRONG_INPUT = stGSMSTATIST.siUSER_BREAK  =
	stGSMSTATIST.siX_DIAL_UP_PRESSED=0;*/
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_SendData.
 *
 * DESCRIPTION: 
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_GSM_SendData(char *Buf,usint Size)
{
	ulint	Timeout;
	Timeout=Scan_MillSec();
	switch(ComHndlr_TxRadio ((byte *)Buf, &Size))
	{
		case X_OK:return STAT_OK;
		case X_TIMEOUT:
		case X_ESC_PRESSED:
		case X_RADIO_BUFFER_TOO_BIG:
		case X_NOT_SWITCHED_TO_RADIO:
		case X_NO_MSG:
		case X_NEW_MSG:
		case X_MSG_RETURNED:
		default:ComHndlr_EndCall();
	}
	return STAT_NOK;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_ReceiveData
 *
 * DESCRIPTION: 
 *
 * RETURN:     
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Mainloop_GSM_ReceiveData(char *RxMessage, usint *RxLength, ulint TimeOut, byte delim1, byte delim2, byte ExtraChars)
{
	sint j, stop=1,status, startIdx;
	ulint Timeout = Scan_MillSec();
	usint len;
	byte Buffer[800];

	memset (Buffer, 0, 800);
    while ((Scan_MillSec()-Timeout) <= TimeOut*1000)
	{
		*RxLength = sizeof(Buffer);
		memset (Buffer, 0, *RxLength);
		Kb_Read();		/* 04_03 21/11/2002 */
		status = ComHndlr_RxRadio((byte *)Buffer, RxLength,10*1000);

		if (status >= 0) 
		{
			len = strlen(RxMessage);
			memcpy (RxMessage+len, Buffer, *RxLength);
			RxMessage [len+(*RxLength)] = 0;

			if (strlen(RxMessage) >= 800) return (STAT_NOK);

			if (delim1==EOT &&delim2==EOT )	return (STAT_OK);
		for (startIdx=0;startIdx<*RxLength;startIdx++)
			if (RxMessage [startIdx] == (char)delim1) break;
			if (RxMessage [startIdx] == (char)delim1)/* || RxMessage [1] == (char)delim1)||
			    (RxMessage[0] == (char)STX || RxMessage[1] == (char)STX))*/
			{
			   if((RxMessage[0]==ENQ || RxMessage[0]==EOT )&&
				   (RxMessage[0]==delim1 && RxMessage[0]==delim2))	return (STAT_OK);
				if(len-1<0) len=1;

				for(j=len-1; j<strlen(RxMessage); j++)
				{
					Kb_Read();
					
					if (RxMessage [j] == ETX)
					{
						if (j< (strlen(RxMessage)-1))
						{
							RxMessage [j+2] = 0;
							*RxLength = j+2;
							return (STAT_OK);
						}
						*RxLength = 1;
						memset (Buffer, 0, sizeof(Buffer));
						if (ComHndlr_RxRadio((byte *)Buffer, RxLength,TimeOut*1000) >= 0) 
						{
							len = strlen(RxMessage);
			   				memcpy (RxMessage+len, Buffer, 1);
							RxMessage [len+1] = 0;			   			
							*RxLength = len+1;
							return (STAT_OK);
						}
						EmvIF_ClearDsp(DSP_MERCHANT);
						Mainloop_DisplayWait((char *)"LRC ALINAMADI", 1);
						Printer_WriteStr("\nLRC ALINAMADI\n");
						
						return (STAT_NOK);
					}				
				}
			}
		}
		else if (status == X_ESC_PRESSED)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait((char *)"KULLANICI KEST˜", 1);
			Printer_WriteStr("\nKULLANICI KEST˜\n");

			return (STAT_NOK);
		}
	}
	return (AKNET_POS_TIMEOUT_ERROR);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_Register
 *
 * DESCRIPTION  : Entry0 - Entry4  
 *
 * RETURN       : none
 *
 * NOTES        : R.Ý.Ö GSMPOS_04_01 irfan 16/02/01 
 *
 * --------------------------------------------------------------------------*/
void Menu5_Entry (void *dummy)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];

	memset(value_null, 0 , sizeof(value_null));

	/*              GMS/DIALUP MENU                      */
	/*----------------------------------------------*/
	if(!Mainloop_GetKeyboardInput((char *)"GSM/DIAL>0:G 1:D",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE))
	{
		Mainloop_DisplayWait("  GSM / DIAL      G˜R˜ž HATASI  ",2);
		return;
	}

	if(Buffer[0] == '0')
	{
		GSMCALL = TRUE;
	}
	else
	{
		GSMCALL = FALSE;          
	}

}
void Menu5_Entry0 (void *dummy)
{
	if(Mainloop_RegisterNetwork(FALSE,FALSE) != STAT_OK) 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" žEBEKEYE G˜R˜ž      HATALI     ",1);
	}
}

void Menu5_Entry1 (void *dummy)
{
	if(Mainloop_MakeVoiceCall() != STAT_OK )
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  SESLI ARAMA      YAPILAMADI   ",1);
	}
	/* Disconnect call */
	ComHndlr_EndCall();
}

void Menu5_Entry5 (void *dummy)
{
	char Buf[800];
	usint lenght;
	/* DATA Call */
	if(Mainloop_MakeDataCall() != STAT_OK) 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  DATA  ARAMA      YAPILAMADI   ",1);
		return;
	}

	memset(Buf,0,800);
	strcpy(Buf, "Akbank Test");
	lenght=strlen(Buf);
	Buf[lenght++]=0x0D;
	if(Mainloop_GSM_SendData(Buf,(usint)lenght)==STAT_OK)
	{
		memset(Buf,0,800);
		lenght=1;
		Mainloop_GSM_ReceiveData(Buf,&lenght, 30, STX, ETX, 1);
	
		Mte_Wait(100);
		/* Disconnect call */
		ComHndlr_EndCall();
	}
}

void Menu5_Entry2 (void *dummy)
{
	/* Mesaj Gonderme*/
	ComHndlr_RadioDisableRxMessages(COMHNDLR_RADIO_MSG_SMS_TXT);
	if( Mainloop_GSM_SMSSend() != STAT_OK) 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("G”nd. BaŸarsz",1);
	}
	Host_ConnectStatist(3);
}

void Menu5_Entry3 (void *dummy)
{
	/* Mesaj Alma*/
	if( Mainloop_GSM_NOS_SMSRead() != STAT_OK )
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("Alma BaŸarsz",1);
	}
	Host_ConnectStatist(3);
}

void Menu5_Entry4 (void *dummy)
{
   entry csMENU_PIN_entry [] = {
				{"PIN A€/KAPA  ", 0, Menu5_Entry4_1 , 0},
				{"PIN1 DE¦˜žT˜R", 0, Menu5_Entry4_2 , 0}
				/*,
				{"PIN2 DE¦˜žT˜R", 0, Menu5_Entry4_3 , 0},
				{"PUK1 DE¦˜žT˜R", 0, Menu5_Entry4_4 , 0},
				{"PUK2 DE¦˜žT˜R", 0, Menu5_Entry4_5 , 0}*/
   };

   menu csMENU_PIN_menu;
   csMENU_PIN_menu.header         = "™ZELL˜KLER";
   csMENU_PIN_menu.no_of_entries  = 2;
   csMENU_PIN_menu.menu_entries   = &csMENU_PIN_entry[0];
   csMENU_PIN_menu.mode_flags     = CYCLIC_MENU|DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

	Formater_GoMenu (&csMENU_PIN_menu, 0);
}

void Menu5_Entry4_1 (void *DUMMY)
{
	char localGsmPin[LEN_GETKEYBOARD];
   entry csMENU_PIN_entry [] = {
				{"PIN A€  ", 0, 0, 0},
				{"PIN KAPA", 0, 0, 0}};
   menu csMENU_PIN_menu;
   csMENU_PIN_menu.header         = "PIN A€/KAPA";
   csMENU_PIN_menu.no_of_entries  = 2;
   csMENU_PIN_menu.menu_entries   = &csMENU_PIN_entry[0];
   csMENU_PIN_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

	switch(Formater_GoMenu (&csMENU_PIN_menu, CHOICE_MODE))
	{
		case 1:
			memcpy(localGsmPin,GsmPin,4);
			memset(GsmPin,0,LEN_GETKEYBOARD);
			EmvIF_ClearDsp(DSP_MERCHANT);
			if(Mainloop_Edit(GsmPin,1))
			{
				sint i;
				/*	if(memcmp(localGsmPin,GsmPin,4))
				{
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait ("   P˜N A€MA        BAžARISIZ", WAIT_RESPONSE);
					break;
				}*/
				if(Mainloop_RegisterNetwork(TRUE,FALSE) != STAT_OK) Mainloop_DisplayWait("REGISTER OLUNAMADI",1);
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait((char*)MESSAGE_WAIT,0);
				
				for(i=0;i<3;i++)
				{
					Kb_Read();
					if(GSMRadio_SetSIMLockState(FALSE)==GSMRADIO_OK)
					{
						if(GSMRadio_SetSIMLockState(TRUE)==GSMRADIO_OK)
						{
							EmvIF_ClearDsp(DSP_MERCHANT);
							Mainloop_DisplayWait ("   P˜N A€MA        BAžARILI", WAIT_RESPONSE);
							FirstRun=TRUE;
							return;
						}
					}
					else 
					{
						if(GSMRadio_SetSIMLockState(TRUE)==GSMRADIO_OK)
						{
							EmvIF_ClearDsp(DSP_MERCHANT);
							Mainloop_DisplayWait ("   P˜N A€MA        BAžARILI", WAIT_RESPONSE);
							FirstRun=TRUE;
							return;
						}
					}
				}
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait ("   P˜N A€MA        BAžARISIZ", WAIT_RESPONSE);

			}
			break;
		case 2:
			memcpy(localGsmPin,GsmPin,4);
			memset(GsmPin,0,LEN_GETKEYBOARD);
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait((char*)MESSAGE_WAIT,0);
				if(GSMRadio_SetSIMLockState(FALSE)==GSMRADIO_OK)
				{
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait ("   P˜N KAPAMA       BAžARILI", WAIT_RESPONSE);
				}
				else
				{
					EmvIF_ClearDsp(DSP_MERCHANT);
					Mainloop_DisplayWait ("   P˜N KAPAMA       BAžARISIZ", WAIT_RESPONSE);
				}
			
			break;
	}
}

void Menu5_Entry4_2 (void *DUMMY)
{
	char pinEntry1[LEN_GETKEYBOARD],pinEntry2[LEN_GETKEYBOARD],pinEntry3[LEN_GETKEYBOARD];

	memset(pinEntry1,0,LEN_GETKEYBOARD);
	memset(pinEntry2,0,LEN_GETKEYBOARD);
	memset(pinEntry3,0,LEN_GETKEYBOARD);
	if(Mainloop_Edit(pinEntry1,2))
	if(Mainloop_Edit(pinEntry2,3))
	if(Mainloop_Edit(pinEntry3,4))
	if(!memcmp(pinEntry1,GsmPin,4) && !memcmp(pinEntry2,pinEntry3,4) )
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait((char*)MESSAGE_WAIT,0); 
		if(GSMRadio_ChangePIN(pinEntry2)==GSMRADIO_OK)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("PIN1 DE¦˜žT˜RME     BAžARILI", WAIT_RESPONSE);
		}
	}
	else
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait ("PIN1 DE¦˜žT˜RME    BAžARISIZ", WAIT_RESPONSE);
	}
}

void Menu5_Entry6 (void *DUMMY)
{
	char str[5],STR1[20];
	strcpy(STR1,"P˜L SEV˜YES˜%");
	
	sprintf(str,"%02d",Cpuhw_GetBatteryEnergy());
	strcat(STR1,str);
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait (STR1, WAIT_WARNING);
	
}
void Menu5_Entry7(void *DUMMY)
{
	boolean LockState,PinState=TRUE;
	char chr;

	Mainloop_DisplayWait ("SIM DE¦˜žT˜R ve E/H sec", 0);
	chr=Kb_WaitForKey();
	if(chr =='E' ||chr ==MMI_ENTER)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		memset(GsmPin,0,LEN_GETKEYBOARD);

		if(Mainloop_RegisterNetwork(TRUE,TRUE) != STAT_OK) 
		{
			Mainloop_DisplayWait(" SIM DE¦˜žT˜RME    BAžARISIZ",WAIT_RESPONSE);
			return;
		}
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait((char*)MESSAGE_WAIT,0);
		Mte_Wait(500);
		PinState=Mainloop_GSM_StaistCTRL();
		if(GSMRadio_GetSIMLockState(&LockState)!=GSMRADIO_OK) 
			Mainloop_DisplayWait (" SIM DE¦˜žT˜RME    BAžARILI", WAIT_RESPONSE);
		else
			if(!PinState)
				Mainloop_DisplayWait(" SIM DE¦˜žT˜RME    BAžARISIZ",WAIT_RESPONSE);
			else
				Mainloop_DisplayWait (" SIM DE¦˜žT˜RME    BAžARILI", WAIT_RESPONSE);
	}
}

void Menu5_Entry8 (void *DUMMY)
{
	sint entry_no;
	entry csMENU_GSM_GPRS_entry []= { {"GPRS", 0, 0, 0},
		           		  {"GSM ", 0, 0, 0}
					};

   menu csMENU_GSM_GPRS_menu;
   csMENU_GSM_GPRS_menu.header		= "GSM - GPRS>";
   csMENU_GSM_GPRS_menu.no_of_entries	= 2;
   csMENU_GSM_GPRS_menu.menu_entries	= &csMENU_GSM_GPRS_entry[0];
   csMENU_GSM_GPRS_menu.mode_flags	= DEFAULT_MODE;
   
   entry_no = Formater_GoMenu (&csMENU_GSM_GPRS_menu,CHOICE_MODE);
   EmvIF_ClearDsp(DSP_MERCHANT);
   
	switch(entry_no)
	{
	case 1 :	Mainloop_SetGPRSParams(TRUE);		
			GPRS_USEGPRS = TRUE;
			GPRS_Disconnect(APN);
			if(GPRS_Init(TRUE)==STAT_OK)
						{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait(" GPRS BAžARILI",0);
				Mte_Wait(2500);
				lsGPRS_Connect_Param.Connected = TRUE;
			}			
			break;
	case 2 :	
			GPRS_USEGPRS = FALSE;
			GPRS_Disconnect(NULL);
			break;
	}   	
	Files_WriteAllParams();

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GSM_StaistCTRL
 *
 * DESCRIPTION: 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
boolean Mainloop_GSM_StaistCTRL(void)
{
	GSMStatistics   gsmstat;
	GSMStatistics   *GSMSTAT;
	memset (&gsmstat, 0, sizeof(gsmstat));
	GSMSTAT=&gsmstat;
	GSMSTAT->Size = sizeof (gsmstat);
	GSMRadio_GetStatistics(GSMSTAT, FALSE);

	if(	(GSMSTAT->PIN1Required>0)	||	(GSMSTAT->PUK1Required>0)||
		(GSMSTAT->PIN2Required>0)	||	(GSMSTAT->PUK2Required>0))
	{
		GSMRadio_GetStatistics(GSMSTAT, TRUE);
		return FALSE;
	}
	return TRUE;
}
/* --------------------------------------------------------------------------*/
void Debug_GrafPrint2 (char *Message, byte line)
{
	char Temp[100];


GrphText_ClrLine (line, FALSE);	/**/

	memset (Temp, 0, 100);
	/*memset (Temp,' ', 4);*/
	/*strcat (Temp+4, Message);*/
	strcpy(Temp, Message);
    GrphText_Write ((byte) line, (sint) 0, Temp,
                    (byte) FONT_13X7, (byte) strlen(Temp),
                    FALSE, FALSE);
    GrphText_DrawDisp ();

}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: GSM_NOS_DisplaySignalBateryLevel
 *
 * DESCRIPTION: 
 *
 * RETURN:      None.
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void GSM_NOS_DisplaySignalLevel(void)
{
	RadioDescrStr Radio;
	ComHndlr_GetRadioStatus(&Radio);
		GrphText_ClrArea(1,100,4,128,FALSE);
	
	if (Radio.Rssi>0)
	{
			GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,43-10,124,FALSE);
		if (Radio.Rssi>8)
		{
				GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,36-10,122,FALSE);
				GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,36-10,124,FALSE);
			if (Radio.Rssi>16)
			{
					GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,29-10,120,FALSE);
					GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,29-10,122,FALSE);
					GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,29-10,124,FALSE);
			
				if (Radio.Rssi>23)
				{
						GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,22-10,118,FALSE);
						GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,22-10,120,FALSE);
						GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,22-10,122,FALSE);
						GrphText_ShowImage((byte *)&_SIGNAL_LEVEL[0],8,6,22-10,124,FALSE);
				}
			}
		}
		else
			GrphText_ClrArea(1,114,2,128,FALSE);
	}
}

	
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: GSM_NOS_DisplayBatteryStatus
 *
 * DESCRIPTION: 
 *
 * RETURN:      None.
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void GSM_NOS_DisplayBatteryStatus(void)
{
	CpuhwBatSysType batStatus;	
	Cpuhw_GetBatSystemStatus(&batStatus);
	
	switch(batStatus.BatState)
	{
		case CPUHW_BATSYS_CHARGE:/* Battery is charging    */
			GrphText_ShowImage((byte *)_TILDA2,16,4,44,2,FALSE);
			if(!sarj)	sarj=TRUE;
			else	
			{
				sarj=FALSE;
				/*				Mainloop_ClearDsp();
				Mainloop_DisplayWait ("   žARJ OLUYOR",1);
				Mainloop_ClearDsp();*/
			}
			break;
		case CPUHW_BATSYS_DISCHARGE:/* Battery is discharging */
			break;		
		case CPUHW_BATSYS_NO_CHARGE:/* Battery is idle        */
			break;		
		case CPUHW_BATSYS_NO_BAT:/* Battery does not exist 	*/
			break;
	}
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: GSM_NOS_DisplayBatteryLevel
 *
 * DESCRIPTION: 
 *
 * RETURN:      None.
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void GSM_NOS_DisplayBatteryLevel(void)
{
	InfoBattery battery;
	byte batlevel;
	Test_Battery((void *)&battery);
		GrphText_ClrArea(1,0,4,30,FALSE);
	batlevel = Cpuhw_GetBatteryEnergy();
		Mte_Wait(50);
	if(batlevel < 100)
	{
			if ((batlevel - oldbatlevel) < 0) GrphText_ClrArea(2,0,3,26,TRUE);

		if(battery.power_status!=EX_POWER_OPERATING)
			if (batlevel<6)	return;
		if (batlevel<15)
			GrphText_ShowImage((byte *)&_PIL_L0[0],16,18,24,0,FALSE);

		else if(batlevel<35)
			GrphText_ShowImage((byte *)&_PIL_L1[0],16,18,24,0,FALSE);

		else if(batlevel<55)
			GrphText_ShowImage((byte *)&_PIL_L2[0],16,18,24,0,FALSE);

		else if(batlevel<75)
			GrphText_ShowImage((byte *)&_PIL_L3[0],16,18,24,0,FALSE);
		else
			GrphText_ShowImage((byte *)&_PIL_L4[0],16,18,24,0,FALSE);
	}
	else
	{
			if ((batlevel - oldbatlevel) < 0) GrphText_ClrArea(2,0,3,26,TRUE);
			GrphText_ShowImage((byte *)&_PIL_L5[0],16,18,24,0,FALSE);
	}
	oldbatlevel = batlevel;
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Debug_SetFont
 *
 * DESCRIPTION: 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
void Debug_SetFont (boolean type)
{

		byte LineHight_7x5  [15];
		byte LineHight_13x7 [12];
byte LineHight_10x7 [6];

    LineHight_7x5[0] =
    LineHight_7x5[1] =
    LineHight_7x5[2] =
    LineHight_7x5[3] =
    LineHight_7x5[4] =
    LineHight_7x5[5] =
		LineHight_7x5[6] =
		LineHight_7x5[7] =
		LineHight_7x5[ 8] =
		LineHight_7x5[ 9] =
		LineHight_7x5[10] =
		LineHight_7x5[11] =
		LineHight_7x5[12] =
		LineHight_7x5[13] = FORMAT_FOR_7X5;
		LineHight_7x5[14] = END_OF_LINES;

		LineHight_10x7[0] =
		LineHight_10x7[1] =
		LineHight_10x7[2] =
		LineHight_10x7[3] =
		LineHight_10x7[4] = FORMAT_FOR_10X7;
		LineHight_10x7[5] = END_OF_LINES;

    LineHight_13x7[0] =
    LineHight_13x7[1] =
    LineHight_13x7[2] =
		LineHight_13x7[3] =
		LineHight_13x7[4] =
		LineHight_13x7[5] =
		LineHight_13x7[6] =
		LineHight_13x7[7] =
		LineHight_13x7[8] =
		LineHight_13x7[9] =
		LineHight_13x7[10] = FORMAT_FOR_13X7;
		LineHight_13x7[11] = END_OF_LINES;



		/*if (type)*/ GrphText_Format (&LineHight_13x7[0]);
/*else      GrphText_Format (&LineHight_7x5[0]);*/

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Auto_IDLEUpdate
 *
 * DESCRIPTION:Updates the display every minute for updating time every minute.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Auto_IDLEUpdate(char *OldTime,int TransOnlineNo,int TransOfflineNo, boolean myLogoCount)
{
	char  DateTime [20], TimeDate[20];
	char  TransOnlineAnsi[20], TransOfflineAnsi[20];
	sint  tt;
	
	Utils_IToA(TransOnlineNo,TransOnlineAnsi);
	Utils_LeftPad(TransOnlineAnsi,'0',3);
	Utils_IToA(TransOfflineNo,TransOfflineAnsi);
	Utils_LeftPad(TransOfflineAnsi,'0',3);
	Mainloop_GetDateTimeFormatted (DateTime, TRUE);
	tt = atoi(&DateTime [17]);
	if (tt%2) Mainloop_GetDateTimeIdle (TimeDate, TRUE);
	else Mainloop_GetDateTimeIdle (TimeDate, FALSE);
	if (!myLogoCount) return;
	if (memcmp(DateTime,OldTime,18))
	{
		Debug_SetFont(FALSE);
		/*MK DEF:3 SATIR 7 DEN 8 E ALINDI*/
		/*bizzcard i?lemleri  de buraya yazmak lazym my?*/
		GrphText_Write ((byte) 8, (sint) 0, (char *) TransOnlineAnsi,
			(byte) FONT_13X7, (byte) strlen (TransOnlineAnsi),
			FALSE, FALSE);
		GrphText_Write ((byte) 8, (sint) 103, (char *) TransOfflineAnsi,
			(byte) FONT_13X7, (byte) strlen (TransOfflineAnsi),
			FALSE, FALSE);
/*
		GrphText_Write ((byte) 7, (sint) 35, (char *) TimeDate,
			(byte) FONT_7X5, (byte) strlen (TimeDate),
			FALSE, FALSE);
*/
		GrphText_DrawDisp ();
	}
	Debug_SetFont(TRUE);
}


/* 04_10 07/01/2003 genius icin 2'den fazla satir basan fonksiyon yazildi */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Get Genius Input From Keyboard
 *
 * DESCRIPTION: Gets input from keyboard. 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
boolean Mainloop_GeniusGetKeyboardInput ( const char *UpperLine, char *Buffer, 
				    sint DownLimit, sint UpLimit,
				    boolean First, boolean Restricted,
				    boolean Masked, boolean Comma,
				    boolean Alpha, byte *Len, char *ExDefault,
				    boolean EmptyOk)
{
  sint i,j,k;
/*@bm 01.09.2004  boolean RestrictedFlag;*/
  boolean DefaultFirst;
  char MaskedBuffer[LEN_GETKEYBOARD];
  char Default[LEN_GETKEYBOARD];
  char ret;
  sint MaskedLen;
  char ToDisplay[40];

	/*1_18 Ek kontroller 05/04/2000 */
	memset(MaskedBuffer,0,LEN_GETKEYBOARD);
	if (UpLimit > LEN_GETKEYBOARD) UpLimit = LEN_GETKEYBOARD; /* UpLimit LEN_GETKEYBOARD
													   uzunluðunu geçemez */
	memset (ToDisplay,0,40);

	EmvIF_ClearDsp(DSP_MERCHANT);

  	/* 1_18 29/03/2000 initialize Default */
	memset(Default, 0 , sizeof(Default));

	if((byte)ExDefault[0] != 0 && (ExDefault != 0) )	strcpy(Default,ExDefault);

	DefaultFirst = FALSE;
	if ((Default[0] == 0) || (!isalnum(Default[0])))
	if (!First)
	{
		memset(Buffer,0,LEN_PAN);
		i = 0;
	}
	else
	{
		memset(Buffer+1,0,LEN_PAN-1);
		i = 1;
	}
	else  if((byte )Default[0]!=0)
	{
		DefaultFirst = TRUE;
		strcpy(Buffer,Default);
		i = strlen(Buffer);
	}

  RestrictedFlag = TRUE;
  do
  {
	  j = strlen(UpperLine);
	  k = 0;
	  for(;;)
	  {
		  Kb_Read();
		  if(j <= 0) break;
		  
		  if( (j <16 ) && (j >= 0) )
		  {
			  memcpy(ToDisplay,UpperLine+k*16,j);
			  ToDisplay[j] = 0;
			  Debug_GrafPrint2 ((char *)ToDisplay, 2+k);
			  break;
		  }

		  memcpy(ToDisplay,UpperLine+k*16,16);
		  ToDisplay[16] = 0;
		  Debug_GrafPrint2 ((char *)ToDisplay, 2+k);
		  
		  k = k+1;

		  j = j - 16;	
  	  }

	  /**********************/
	
	  if (Masked)
	  {

		memset(MaskedBuffer,0,LEN_GETKEYBOARD);
			GrphText_ClrLine (7, FALSE);
		memcpy(MaskedBuffer, "****************", i);

			Debug_GrafPrint2 ((char *)MaskedBuffer,7);
	  }
	  else
	  {
		  if (Comma)
		  {
			  Mainloop_WithComma(Buffer,MaskedBuffer,i);
			  MaskedLen = strlen(MaskedBuffer);

			  MaskedBuffer[MaskedLen++] = 0xBE;
			  MaskedBuffer[MaskedLen] = 0;
			  Utils_LeftPad(MaskedBuffer,' ',16);
				Debug_GrafPrint2 ((char *)MaskedBuffer,7);
		  }
			else if (i>16) Debug_GrafPrint2((char *)Buffer + (i-16),7); /* for scrolling */
			else  Debug_GrafPrint2((char *)Buffer, 7);
	  }

	  ret = Kb_WaitForKey();
		switch (ret)
	  {
	  case MMI_CLEAR :
		  /* 1_18 05/04/2000 boþluk yerine Null ile dolduruluyor */
		  /*memset(Buffer,' ',LEN_PAN);*/
			memset(Buffer,0,LEN_PAN);
			if(Default!=0)
			{
				strcpy(Buffer,Default);
				i = strlen(Buffer);
			}
			DefaultFirst = TRUE;
		  break;
	  case MMI_BACK_SPACE :

		  if (i > 0) i--;
		  Buffer[i] = ' ';
		  DefaultFirst = FALSE;
		  break;
	  case MMI_FORCED :
/*	  case MMI_ALPHA  :
		  if ((Alpha) && (i>0)) Buffer[i-1] = Mainloop_GetAlpha(Buffer[i-1],TRUE);
		  break;*/
	  case MMI_RETURN :
		  if ((Alpha) && (i>0)) Buffer[i-1] = Mainloop_GetAlpha(Buffer[i-1],FALSE);
		  break;
	  case MMI_POINT :
		if ( ( i != 0 ) && ( Comma ) )
		{
			DefaultFirst = FALSE;
			if ( i < UpLimit )
				Buffer[i++] = '0';
			if ( i < UpLimit )
				Buffer[i++] = '0';
			if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
			{			/*bm YTL 30/06/2004 */
				if ( i < UpLimit )
					Buffer[i++] = '0';
			}
			else if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
			{
				if (TL_Iade)
				{
					if ( i < UpLimit ) Buffer[i++] = '0';
				}
			}
		}
		break;
	  default:
		 if (isdigit(ret) && 
			  !((i==0) && (ret=='0') && Comma)) 
		  {
			  if ((DefaultFirst) && (!First))
			  {
				  i = 0;
				  /* 1_18 05/04/2000 boþluk yerine Null ile dolduruluyor */
				  /*memset(Buffer,' ',LEN_PAN);*/
					memset(Buffer,0,LEN_PAN);
			  }
			  if (i < UpLimit)
			  {
				  DefaultFirst = FALSE;
				  Buffer[i] = ret;
				  i++;
			  }
		  }
	  }
	  if (Restricted)
	  {
		  if ((i==UpLimit) && (ret == MMI_ENTER) && ((Buffer[0] != ' ') || (EmptyOk)))
			  RestrictedFlag = FALSE;
		  else if ((ret == MMI_ESCAPE)) RestrictedFlag = FALSE;
	  }
	  else if ((ret == MMI_ESCAPE) || ((ret == MMI_ENTER) && ((Buffer[0] != ' ') || (EmptyOk))
				&& (i>=DownLimit) && (i<=UpLimit)))
		  RestrictedFlag = FALSE;

  } while(RestrictedFlag);
  *Len = i;
  return (ret == MMI_ENTER);
}

/*  06_30 *************** offline islemler ***************************************/
void Menu0_Entry5 (void *dummy)
{
	if(Offline_AskOfflinePassword() != STAT_OK)	return;
	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE;
	Mainloop_ShowOfflineMenu(  );
	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_ShowOfflineMenu
 *
 * DESCRIPTION  : Displays the Mainloop_ShowOfflineMenu
 *
 * RETURN       : none
 *
 * NOTES        : R.Ý.Ö. 06/02/2003 
 *
 * --------------------------------------------------------------------------*/
void Mainloop_ShowOfflineMenu(void)
{
   entry csMENU_SISTEM_entry [] = {
		    /*1234567890123456*/
				{"OFFLINE SATIS   ", 0, OfflineMenu_Entry0 , 0},
				{"OFFLINE IPTAL   ", 0, OfflineMenu_Entry1 , 0}
				};
   menu csMENU_SISTEM_menu;


   csMENU_SISTEM_menu.header         = "OFFLINE ANA MENU";
   csMENU_SISTEM_menu.no_of_entries  = 2;
   csMENU_SISTEM_menu.menu_entries   = &csMENU_SISTEM_entry[0];
   csMENU_SISTEM_menu.mode_flags     = DEFAULT_MODE|MENU_TIMEOUT_30_SEC;

   Formater_GoMenu (&csMENU_SISTEM_menu, 0);

}

void OfflineMenu_Entry0 (void *dummy)
{	/* offline satis */
	
	byte lbret_code;
	boolean Lbdummy;

	/* read global parameters */
	Files_ReadAllParams();


	tran_type = SALE;
	OFFLINE_CARREFOURSA = TRUE;
/* Toygar - NextEMV Start - Ok */
	if ( Mainloop_Get_CardInfo( 0, 0, 0, 0, 0, 0, 0, 0, 0, &Lbdummy, FALSE ) )
/* Toygar - NextEMV Start - Ok */
	{
		Offline_Transaction();
	}

	OFFLINE_CARREFOURSA = FALSE;

	/*save parameters files */
	Files_WriteAllParams();
	
}

void OfflineMenu_Entry1 (void *dummy)
{	/* offline iptal */
	
	byte lbret_code;
	boolean Lbdummy;

	/* read global parameters */
	OFFLINE_CARREFOURSA = TRUE;
	Files_ReadAllParams();

/* Toygar - NextEMV Start - Ok */
	if ( Mainloop_Get_CardInfo( 0, 0, 0, 0, 0, 0, 0, 0, 0, &Lbdummy, FALSE ) )
/* Toygar - NextEMV Start - Ok */
	{ 
		Offline_Cancel();
	}

	OFFLINE_CARREFOURSA = FALSE;

	/* save parameters */
	Files_WriteAllParams();

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Mainloop_GetKasierCardData
 *
 * DESCRIPTION	: 
 *	
 * RETURN		:      None.
 *
 * NOTES		:  01/08/04
 *
 * --------------------------------------------------------------------------*/
boolean Mainloop_GetKasierCardData(void)
{


	sint track1_err;
	sint track2_err;
	char dummyy;
	boolean KeyPressedFlag;

	CardRdr_Enable();
	KeyPressedFlag = FALSE;
	CORRUPTED_CHIP_TRANSACTION_FLAG = TRUE; /*@bm 30.08.2004 */

#ifndef GRAFDISP
	Display_ClrDsp();
#else
    GrphText_Cls (TRUE);
#endif

	Mainloop_DisplayWait("KART NO ?",NO_WAIT);

	while (!CardRdr_IsCardCompleted(&track1_err,&track2_err))
	if (dummyy = Kb_Read())
	{
	    KeyPressedFlag = TRUE;
		break;
    }

    if (!KeyPressedFlag)
	{
		if (!track1_err)
		{
			TRACK1ERROR = TRUE;
		}
		else
		{
			TRACK1ERROR = FALSE;
		}

		if(!track2_err)
		{
			if(!Mainloop_CheckSwipe(FALSE))
			{
				Mainloop_DisplayWait("Mainloop_CheckSwipe hatasi",2);
				return(FALSE);	
			}
		}
		else
		{
//			Mainloop_DisplayWait("KART OKUMA HATASI",2);
			return(FALSE);	;
		}
	}
    else
    {
		Mainloop_DisplayWait("KARTI GE€˜R˜N˜Z",2);
		return(FALSE);	
	}

	return(TRUE);	

}


void Mainloop_SetGPRSParams(boolean mode){
	char Buffer[50];

	memset(Buffer,0,50);
//	strcpy(Buffer,APN);
	if(!Utils_GetEditScreen1(33,0, (char *)Buffer,12))//APN 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("G˜R˜ž HATASI",1);
		return ;
	}
	memset(APN,0,12);
	memcpy(APN,Buffer,strlen(Buffer)-1);
//	PrntUtil_BufAsciiHex((byte*)Buffer,strlen(Buffer)+1);
	memset(Buffer,0,50);
	if(!Utils_GetEditScreen1(37,0, (char *)Buffer,10))//KULLANICI ADI 
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("G˜R˜ž HATASI",1);
		return ;
	}
//	PrntUtil_BufAsciiHex((byte*)Buffer,strlen(Buffer)+1);
	memset(GPRS_User,0,10);
	memcpy(GPRS_User,Buffer,strlen(Buffer)-1);
	memset(Buffer,0,50);
	if(!Utils_GetEditScreen1(38,0, (char *)Buffer,10))//SIFRE
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("G˜R˜ž HATASI",1);
		return ;
	}
//	PrntUtil_BufAsciiHex((byte*)Buffer,strlen(Buffer)+1);
	memset(GPRS_Passwd,0,10);
	memcpy(GPRS_Passwd,Buffer,strlen(Buffer)-1);

	memset(glb_host_ip,0,17);
//	memcpy(glb_host_ip,"192.168.3.23",12);

	memset(Buffer,0,50);
	if(!Utils_GetEditScreen1(35,0, (char *)Buffer,16))//HOST
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("G˜R˜ž HATASI",1);
		return ;
	}
	memset(glb_host_ip,0,17);
	memcpy(glb_host_ip,Buffer,strlen(Buffer)-1);

	
	glb_host_port = 28002;
	memset(Buffer,0,50);
	if(!Utils_GetEditScreen1(39,0, (char *)Buffer,6))//PORT
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("G˜R˜ž HATASI",1);
		return ;
	}
	glb_host_port = atoi(Buffer);
	

	if(mode)Files_WriteAllParams();

}
