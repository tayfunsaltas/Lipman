/**************************************************************************
  FILE NAME:   TRANS.C
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
/* General Header File */
/*#include "config.h"*/

#include "project.def"
#define ARC_APPROVED         0x00
#define ARC_DECLINED         0x51
#define ARC_REFERRAL         0x01
#define ARC_NO_CONNECTION    0xFF

#define POS_VERSION_NUMBER "0701"

/* MCC68K Header Files */
/*UPDATE 01_16 26/10/99 sprintf gibi komutlarýn prototype'ý için konuldu*/
/*#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include NOS_H*/

/**XLS_PAYMENT 20/07/2001 irfan **/
#include INTERFAC_H

#include DEFINE_H
#include ERRORS_H
#include UTILS_H
#include FILES_H
#include MESSAGE_H
#include TRANS_H
#include HOSTCOMM_H
#include SLIPS_H
#include MAINLOOP_H
#include PROTOCOL_H
#include DEBUG_H

#include EMVDEF_H
#include EMVDEFIN_H
#include EMVIF_H
#include EMVMMI_H
#include VERDE_H
#include GPRS_H

/* 06_30 */
#include OFFLINE_H

/* verde. kullanýlmadýgý icin kaldýrýldý 23/10/2002*/
/*boolean Debit, InitMasterKey;           /* SILINECEK */

/* PERMANENT FLAGS */
#define NORMAL_REVERSAL	0x01
#define ADVICE		0x02
#define CHANGES		0x03
#define MAX_PK_DATA_LEN 268  /*Total constructed size for Nca + RID + Exp + PKI*/

boolean INSTALLATION;   
boolean ERROR_RECEIPT;
boolean LAST_4_DIGIT_FLAG;
extern boolean SECOND_COPY;                             /* asýl taným Slips.c nin icinde */
extern boolean IS_HOST_CONNECTED_FLAG;  /* defined in Hostcomm.c */
boolean INSTALLATION_ONGOING;
/* TEMPORARY FLAGS */
boolean DEBIT_FLAG;
boolean AMEX_FLAG; 
boolean VOID_FLAG;
boolean FIRST_RUN;

extern char Received_Buffer[800];
extern const char Transaction_File[];
extern const char CAPK_File[];
extern const char Exception_File[];
extern const char Revocation_File[];
extern boolean BizSlip; //@bm  15.12.2004
extern int BFIRMA_BAUD;

/* ATS related variables */
boolean ATS_FLAG; 
int ATS_Count;
char ATS_Count_c[3];
boolean ATS_PERMISSION_FLAG;
char ATS_Prefixes[90];

/* FAIZLI 02_00 25.07.2000 Ýþlemin faizli olup olmadýðýný gösterir*/
boolean FAIZLI_FLAG; 

/* v04_05 aile puan sorgulama flag 15/07/2002 */
boolean AILE_PUAN_SORGULAMA;

/* FAIZLI 02_00 25/07/2000 TRUE = Faizli iþlem FALSE = Faizli iþlem deðil*/
extern char amount_faiz_kismi[14];      /* faizli islemde faiz kismini icerir */

extern GSMSTATIST stGSMSTATIST;
extern char last_genius_eod_date[9];
extern boolean GSMCALL;


extern char APN[12];
extern char glb_host_ip[17];
extern usint glb_host_port;
extern boolean GPRS_USEGPRS;
extern boolean GPRS_CheckMode;
extern ulint GPRS_TryTime;
extern boolean autoGSMRetry;

extern GPRS_Connect_Param lsGPRS_Connect_Param;


/************************** Message Structure ***********************************/
struct Transaction_Send			d_Transaction_Send;
struct Transaction_Receive		d_Transaction_Receive;

struct Void_Send				d_Void_Send;
struct Void_Send                d_Confirmation_Send;
struct Void_Receive				d_Void_Receive;

struct EOD_Send					d_EOD_Send;
struct EOD_Receive				d_EOD_Receive;
struct Accummulators			d_Totals_Counts[4];

struct Statistics				d_Statistics1;
struct Statistics				d_Statistics2;

struct Param_Send				d_Param_Send;
struct Param_Receive			d_Param_Receive;

struct Param_Continue_Send		d_Param_Continue_Send;
struct Param_Continue_Receive	d_Param_Continue_Receive;
struct Card_Type_Table d_Card_Type_Table[CARDTYPETABLESIZE];
struct Receipt_Structure		d_Receipt;

/* Genius 04_00 */
char vade_sayisi_c[3];
int vade_sayisi;

char Kampanya_Data[15]; 		/*@bm 17.09.2004 */

/*************************************************************************************
* POS permanent variables
*************************************************************************************/

/* EOD Related */
char EOD_pointer;                                       
boolean EOD_flag;                                       
char auto_EOD_time[5];                          
int auto_EOD_time_int;
extern char last_eod_date[9];
boolean AutoEOD;                                                

boolean EOD_OK;

/* EOD Related temp variables*/
char EOD_error_message[25];
/* EOD Related End.*/

const int Timeout_Server_Open = 10;     /* gecici 10 yerine 1 yapýldý */
const int POS_TIMEOUT = 45;        
const int PinPad_Timeout = 2;                                     

int Statistics_Server_Open;                     
char merch_AMEX_no[11];         

char merch_addr[73];
char tel_no_1[16];      
char tel_no_2[16];
char tel_no_1_temp[16];
char tel_no_2_temp[16];

char v_approval_code[7];
char v_amount_surcharge[14];
char v_seq_no_c[5];
char amount_surcharge[14];
char amount_surcharge_saved[14];
char amount_surcharge_iade[14];
char input_type;
char card_no[20];
int card_no_length;

/* 01_18 irfan 27/06/2000 card track 1 datasý icin tanýmlandý*/
char card_info1[50];
extern char card_info1_name[25];
extern char card_info1_surname[25];

char card_info[50];
char last_4_digit[5];
char exp_date[5];
char cvv_2[4];
char approval_code[7];
char encrypted_PIN[17];
char PINPad_status;
char date[7];
char time[5];
char confirmation[4];
char message[17];
char card_type_long_name[19];
char card_type_short_name[7];
char agreement;

extern int tran_type;           /* defined in mainloop.c */
char merch_device_no[9];                                
int seq_no;                                                     
char seq_no_c[5];
int batch_no;                                           
char batch_no_c[4];                                     
char const akbank_BKM_code[11];         
char merch_no[13];                                      
char device_serial_no;                          
char param_ver_no[4];                           
char POS_software_ver;                          
boolean keypad_entry_permission;                
boolean check_last_4_digit;                     
char admin_password[15];                                
char operator_password[5];                      
char SoftPassword[5];
/* bm opt char SoftPassword[5]; */
int  taksit_tipi; /* 13.08.2004 */
char last_transact_batch_no[4];         
char last_transact_seq_no[5];           
char last_transact_status;                      
int record_counter;       
int Grecord_counter;
                              
char net_total[14];                                     
char agreement_total[14];                       
/*char debit_prefix[10][30][7];           /* stores the debit prefixes received from HOST */ /*verde*/
char debit_prefix[07][10][7];           /* stores the debit prefixes received from HOST */ /*verde*/
char working_key[16];
char master_key_no;

ulint total_batch_time;
char total_batch_time_c[9];
char Line_Type;                                         /* Tone veya Pulse olabilir. */


boolean OFFLINE_ACIK_FLAG; /* bm OFF 21.10.2004 */

char UserPassword[5];
char OperatorPassword[15];

extern boolean TIME_IS_UP;              /* auto gun sonu saatinin gelip gelmedigini gosterir */
							/* TRUE: auto eod saati geldi, FALSE : auto eod saati gelmedi */
/* bm opt char OperatorPassword[15]; */
/* bm opt char YTL_Password[5];			/* bm YTL 25/06/2004 -1234- */
char YTL_Slip[3];				/* bm YTL 04/08/2004 TLY - YTL WRT*/
int Set_Mode;					/*bm YTL */
boolean WARNING_FLAG;                                   
extern boolean FIRST_PHONE_FLAG;
extern boolean SERVER_OPEN_TIMEOUT_FLAG;

/* UPDATE_01_18 20/21/2000*/
extern boolean RETURN_PERMISSION;       /* Manuel iþlem fonksiyonuna izin verme flag'ý. 1: MANUEL ÝÞLEM AÇIK. 0: MANUEL ÝÞLEM KAPALI

/* UPDATE_01_14 20/10/99*/
extern boolean MANUEL_PERMISSION;       /* MANUEL ÝÞLEM fonksiyonuna izin verme flag'ý. 1: MANUEL ÝÞLEM AÇIK. 0: MANUEL ÝÞLEM KAPALI

/* UPDATE_01_18 14/01/00*/
extern boolean MANUEL_ENTRY_PERMISSION; /* manuel iþlem fonksiyonuna izin verme flag'ý. 1: AÇIK. 0: KAPALI*/
  
/* UPDATE_01_18 28/06/00 isim yazma flag*/
extern boolean ISIMYAZMA_FLAG;  /* defalut deger acýk yapýlýyor */
extern boolean TRACK1ERROR;     /* Track1 in hatalý okunup okunmadýðýný gösterir
								TRUE : Track1 baþarý ile okundu
								FALSE : Track1 okunamadý */

/* FAIZLI 02_00 28/07/2000 iade edilecek satiþ tarihini ifade eder*/
char Sale_Date_Char[7];

/* 02_01 08/09/2000 Pinpad den taksit giriþ flag irfan*/
/* TRUE : Pinpad den giriþ   FALSE : POS den giriþ*/
extern boolean PINPAD_TAKSIT_FLAG; 

/** XLS_PAYMENT related variables 18/07/2001 irfan **/
extern char remained_chip_data[17];
extern char surname_name_chip[27];
char Loyalty_Prefixes[90];
boolean LOYALTY_PERMISSION_FLAG;
char HostGeneratedCashback[14];
extern boolean CORRUPTED_CHIP_TRANSACTION_FLAG;  /* TRUE: courrupted chip  FALSE: Not corrupted chip */
extern boolean IS_LOYALTY_INSTALLMENT_INTEREST;	/* For Sale and Return Transactions 30/07/2001
											   TRUE  : Loyalty transaction with interes
											   FALSE : Loyalty transaction without interest */
/*boolean OFFLINE_TRX;							/*	TRUE	: Offline Trx.
													FALSE	: Online Trx. */
/*06_30 offline kaldyryldy*/

boolean ReversalPending;
boolean KeyDownloadStarted;
boolean AppListDownloadStarted;
boolean ExceptionDownloadStarted;
boolean RevocationDownloadStarted;

/** XLS_04_02 irfan. 03/12/2001. **/
char faiz_amount_global[14];
extern char genius_flag; /*true = bizzpos*/

/* Toygar 13.05.2002 Start */
/*Buyuk Firma */
extern boolean BUYUKFIRMA;
const int Bfirma_Ack_Timeout	 =  3000;	/* 3 seconds*/
const int Bfirma_Status_Timeout	 = 60000;	/*60 seconds*/
const int Bfirma_Message_Timeout = 35000;   /*35 seconds*/
char POS_Type; /* Default axess secili. 0:Axess  1:Akbank */
/*  1: Akbank 								2: Axess 
	3: Akbank(new pos) 						4: Axess(new pos)
	5: Akbank(new pos operating with YTL)	6: Axess(new pos operating with YTL)*/
char Host_YTL_Password[5];		/* bm YTL 25/06/2004 -4321- */
char YTL_Flag[3]; 				/*bm YTL 04.08.2004*/
byte XLS_Currency; /*04.08.2004 */
boolean BFirma_Cancel_Allowed;
boolean CallFlag;
int POSEntryFlag;
int BadICC;
/* Toygar 13.05.2002 End */

/* Envanter Bilgileri 25.07.2002 */
char GPOS_Type[3];
char GPOS_Model[4];
char GEMV_Key_Version_No;
char GConfig_Version_No;
char GPOS_Serial_No[9];
char GPOS_Akbank_No[7];
char GPinpad_Serial_No[9];
char GPinpad_Akbank_No[7];
char GActive_Application_Code;
char GConnection_Type;
/* Envanter Bilgileri 25.07.2002 */
char TempEMV_Key_Version_No;
char TempConfig_Version_No;


sint Cashier_Flag;				/*bm YTL 13.08/2004 Cashier menu */
char Kasiyer_No[20];			/*bm YTL 13.08.2004 */
boolean TL_Iade;
int Reversal_Buffer_Len;				/*bm YTL 13.08.2004 */
extern char amount_Iade[14];	/*bm YTL 13.08.2004 */
extern boolean KampanyaFlag; /*@bm 16.09.2004 */
extern boolean OldKampanyaFlag;


/* v04_05 CALISMA_KOSULLARI_UPDATE  0:KAPALI 1:ACIK(default)*/
extern boolean CALISMA_KOSULLARI_MODE;

/*ara rapor iptal */
extern boolean ARA_RAPOR_IPTAL;
			   
/* 05_02 16/09/2002 NOT_CERTIFIED_CARD irfan */
extern char Not_Certified_Prefixes[35];
char amount_surcharge_reverse[14];

/*************************************************************************************
* POS temporary variables
*************************************************************************************/

/* Toygar EMVed */

/* 06_01 RECONCILATION irfan 20/09/2002 */
/*char lbSend_Buffer[700];                          /* general send buffer */
char Reversal_Buffer[800];                          /* general send buffer */
char ContTypeFlag;
/*char lbSend_Buffer[256];                      *//* general send buffer */
extern const char EMVOffline_File[];
extern const char EMVOnline_File[];
extern boolean DEBUG_MODE;

#define ARC_APPROVED         0x00
#define ARC_DECLINED         0x51
#define ARC_REFERRAL         0x01

/* Toygar EMVed */
int ret_code;                                   /* to analyze return code */

/******* VERDE ***********/
extern boolean VERDE_TRANSACTION;	/* 1: VERDE transaction 0: Normal Transaction */
//bm 15.12.2004 extern char verde_amount[14];
extern char verde_discount[14];
//bm 15.12.2004  extern char verde_amount1[14];
char glAmount_Req_Type;
boolean VERDE_TRX_CONTINUE;	/* verde 13/12/2002. TRUE: Trx. continue. FALSE: Trx. stop*/
boolean IsEMVReversal;
extern boolean EXTERNAL_PINPAD;

/* GENIUS */
char amount_genius[14];
char genius_seq_no_c[5];
int genius_seq_no;
struct GTransaction_Send		d_GTransaction_Send;
struct GTransaction_Receive		d_GTransaction_Receive;
struct GReceipt_Structure		d_GReceipt;
struct GSorgulamaReceipt_Structure		d_GSorgulamaReceipt;
char	v_Gapproval_code[7];
struct GBalanceQueryReceipt_Structure	d_GBalanceQueryReceipt; /* 04_16 06/08/2003 irfan balance query icin konuldu*/
boolean GVOID_FLAG;
boolean SORGULAMA;		/*1:TRUE:sorgulama 0:FALSE:gerçek transaction*/
char belge_no[11];
char mev_hes_kodu;
char v_genius_seq_no_c[5];
int		Grecord_counter;

boolean OFFLINE_TRX; /*	TRUE	: Offline Trx. FALSE	: Online Trx. */
extern boolean EMV_DEBUG_MODE;
boolean TVR_TSI;
boolean ChangesPending;
extern boolean PIN_BLOCK;
extern boolean SEND_EMV_FIELDCHANGES;
extern boolean OnlinePinEntry;
extern boolean EMVAxess;
extern boolean PRINTALLTAGVALUES;
extern const EMVDataDef deEMVDef[112];
extern char kampanya_str[15];
extern int KAMPANYA_FLAG;
extern boolean EMVAxessStatu;		/* 19.07.2004 axess visa degisiklikleri */

boolean POSEntryFlagUsed;
boolean ConnectionSuccessfull;
char GPOS_Akbank_No_My[7];
char GPOS_Akbank_No_My2[7];


	  /*==========================================*
	   *           D E F I N I T I O N S          *
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
 * FUNCTION NAME	: TransXLSOfflineCancel
 *
 * DESCRIPTION		: execute offine cancel transaction
 *
 * RETURN			: STAT_OK, STAT_NOK
 *
 * NOTES			: r.i.o  01/08/01
 *
 * ------------------------------------------------------------------------ */
byte TransXLSOfflineCancel(TransactionStruct *fpTransStruct); /* 06_30 IsEmv flag ikinci parametre idi kaldirildi */

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Trans_Update_Cancel
 *
 * DESCRIPTION		: Sends void message, receives the response and analyzes it
 *
 * RETURN			: none.
 *
 * NOTES			: r.i.o & t.c.a 19/05/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_Update_Cancel(TransactionStruct *, boolean, boolean);
  
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Trans_IsFallbackAllowed
 *
 * DESCRIPTION:
 *             
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
boolean Trans_IsFallbackAllowed()
{
  RowData rdData;

  /* Only Mastercard applications are allowed for fallback */
  if (EmvDb_Find(&rdData, tagDF_NAME) && 
    (!memcmp(rdData.value, "\xA0\x00\x00\x00\x04", 5) || 
    !memcmp(rdData.value, "\xA0\x00\x00\x00\x10", 5))) 
    return TRUE;
  
  return FALSE;
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Host_AsciiToBin
 *
 * DESCRIPTION: Converts ASCII to bcd. 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Host_AsciiToBinMy(char *fpStrAscii,char *fpStrBin,int fbLen)
{
	byte lbNumber;
/*	byte subt;*/
	int liCounter, lbStart;
	
	lbStart = 0;
	if (fbLen%2)
	{
		if (fpStrAscii[0] >= 'A') lbNumber = (fpStrAscii[0]-'A'+10)*16;
		else lbNumber = (fpStrAscii[0]-'0')*16;
		lbStart = 1;
		fpStrBin[0]=lbNumber;
	}
	for (liCounter = 0; liCounter<(fbLen/2); liCounter++)
	{
		if (fpStrAscii[liCounter*2+lbStart] >= 'A') lbNumber = (fpStrAscii[liCounter*2+lbStart]-'A'+10)*16;
		else lbNumber = (fpStrAscii[liCounter*2+lbStart]-'0')*16;
		if (fpStrAscii[liCounter*2+1+lbStart] >= 'A') lbNumber += fpStrAscii[liCounter*2+1+lbStart]-'A'+10;
		else lbNumber += fpStrAscii[liCounter*2+1+lbStart]-'0';
		fpStrBin[liCounter+lbStart]=lbNumber;
	}
}

/* 04_15 10/06/2003 */
char net_total_GT1_amt[14];
char net_total_GT2_amt[14];
char rev_total_GT1_amt[14];
char rev_total_GT2_amt[14];

/**************************************************************************************
 ****************** TRANSACTIONS FOR AKBANK         ***********************************
 **************************************************************************************/


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Trans_AskPin
 *
 * DESCRIPTION  : Ask the PIN
 *
 * RETURN               : STAT_OK, STAT_NOK
 *
 * NOTES                : t.c.a & r.i.o  15/06/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_AskPin(char *fpCpin)
{
	byte out [17];

	memset (out, 0, 17);
	
	switch(Host_ExternalPinpad (out))
	{
	case STAT_OK:
			Host_CancelSessionRequest();
			strcpy (fpCpin, (char *)out);
			return (STAT_OK);

	case STAT_NO_CONNECTION:
		Mainloop_DisplayWait("  ž˜FRE C˜HAZI    BA¦LI DE¦˜L   ",2);
		return (STAT_NOK);

	default :
		Mainloop_DisplayWait("ž˜FRE SORMA HATASI",2);
		return (STAT_NOK);
	}

}

/* --------------------------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_Transaction
 *
 * DESCRIPTION          : Transaction which handles Sale, Preprovision, Manuel, Return and Balance
 *
 * RETURN                       : STAT OK, STAT_NOK
 *
 * NOTES                        : r.i.o. & t.c.a & m.t.k & h.f.o
 *
 * -------------------------------------------------------------------------------------------- */
byte Trans_Transaction( ApplicationParams * pAppData,
						TerminalAppList * pTerminalApps,
						MutualAppList * pMutualApps, byte * pbMutualAppIndex,
						TerminalParams * pTermData, EMVLanguages * pelsLangs,
						byte * pbErr, RowData * rdSDAData,
						EMVTransData * etdData, PTStruct * fpPTStruct,
						boolean IsEMV, boolean Kampanya )
/*byte Trans_Transaction(void)*/
{
	/* balance related variables */
	char temp_buff[17];
	char balance_message[33];
	char first_6_digit[7];
	boolean confirmation_ret;
	char TransReceived_DateTime[11];
	byte Transaction_Ret;
/* Toygar EMVed */
	sint i,j;
	byte *pbIndex;
	char temp_cn[20];
	char tempchnm[50];
	char temporary[256];
	RowData rdData;
/* Toygar EMVed */
	char XLS_Cardno[20];
/* 06_01 RECONCILATION irfan 20/09/2002 */
	char lbSend_Buffer[900];
/*	char lConcilation_Data[504];*/ /* 06_00 reconcilation 24/09/2002 irfan*/
/* Toygar - NextEMV Start - Ok */
	byte pbMessage;
	byte bCommResult;
	byte bFakeARC;
	byte  usARC[2];
/* Toygar - NextEMV End - Ok */
	RowData myrdData;
	char tel_no_1_temp[16];
	char tel_no_2_temp[16];
	byte usAuthResCode[5];

	/* 06_30 */
	OfflineParametersStruct	lbOfflineParametersStruct;

	bCommResult = ARC_NO_CONNECTION;
	if ( ( tran_type == BALANCE ) || ( tran_type == PREPROV ) ||
		 ( tran_type == MANUEL )  || (tran_type == RETURN) )
	{
		if (IsEMV)
		{
			memset(fpPTStruct->Pan, 0, sizeof(fpPTStruct->Pan));
			EmvDb_Find(&myrdData, tagPAN);
			EmvIF_BinToAscii(myrdData.value, fpPTStruct->Pan, myrdData.length, sizeof(fpPTStruct->Pan), FALSE);
			while (fpPTStruct->Pan[strlen(fpPTStruct->Pan) - 1] == 'F') fpPTStruct->Pan[strlen(fpPTStruct->Pan) - 1] = 0;
			fpPTStruct->PanLength = strlen(fpPTStruct->Pan);
			memset(card_info,' ',40);
			memset(card_no,0,20);
			memset(exp_date,0,5);
			memset(cvv_2,0,4);
			memset(card_info1_name, 0, sizeof(card_info1_name));
			memset(card_info1_surname, 0, sizeof(card_info1_surname));		

			memset(tempchnm,' ',50);	
			pbIndex = (byte *)tempchnm;
			EmvIF_AppendData(&pbIndex, tagCARDHOLDER_NAME, FALSE, 26);
			memcpy(card_info1_name, tempchnm, sizeof(card_info1_name));

			memset(tempchnm,' ',50);	
			pbIndex = (byte *)tempchnm;
			EmvIF_AppendData(&pbIndex, tagCRDHLDR_NAME_EXT, FALSE, 45);
			memcpy(card_info1_surname, tempchnm, sizeof(card_info1_surname));

			memcpy(card_info, fpPTStruct->Pan, fpPTStruct->PanLength);
			memset(temp_cn,' ',20);	
			pbIndex = (byte *)temp_cn;
			EmvIF_AppendData(&pbIndex, tagAPPL_EXPIRATION_DATE, FALSE, 0);
			card_info[fpPTStruct->PanLength] = '=';
			EmvIF_BinToAscii(temp_cn,card_info+fpPTStruct->PanLength+1,2,4,FALSE);
			memcpy(card_info+fpPTStruct->PanLength+5,"000",3);
			memcpy(card_no, fpPTStruct->Pan, fpPTStruct->PanLength);
			memcpy(exp_date,card_info+fpPTStruct->PanLength+1,4);
			memcpy(cvv_2,"000",3);
			memset(remained_chip_data,0,sizeof(remained_chip_data));
			memcpy(remained_chip_data,card_info+fpPTStruct->PanLength+1,7);
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
			if (tran_type == BALANCE) IsEMV = FALSE;
		}

	}

	/* 06_09 25/02/2003 approval_code space deðerine initialize edildi */
	if (!EMVAxessStatu)
	{
		memset(approval_code, ' ', sizeof(approval_code));
		approval_code[6] = 0;
		memset(encrypted_PIN, '0', sizeof(encrypted_PIN));
		encrypted_PIN[16] = 0;
	}

	/* 06_20 default deger set ediliyor irfan. 09/01/2003*/
	CallFlag = TRUE;
	BadICC = 0;

	/* verde */
	VERDE_TRX_CONTINUE = TRUE;

	/** XLS_PAYMENT 02/08/2001 irfan **/
	OFFLINE_TRX = FALSE;

	/* Check for installation*/
	if ( !INSTALLATION)
	{
		Mainloop_DisplayWait(" KURULUž YAPIN  ",1);                             /* DEGISECEK_MESAJ*/
		return(STAT_NOK);
	}

	/* Check for incomplete EOD transcation*/
	if (EOD_flag)
	{
		Mainloop_DisplayWait(" GšNSONU YAPIN  ",1);                             /* DEGISECEK_MESAJ*/
		return(STAT_NOK);
	}

	/*read parameters files */
	if(Files_ReadAllParams() != STAT_OK) return (STAT_NOK);


	/* Initialize the Send Buffer*/
	memset( lbSend_Buffer, 0, 900 );

	
	if (!EMVAxessStatu)
	{
	/* FAIZLI 02_00 28/07/2000 initialize the sale date */
/*	memcpy(Sale_Date_Char, '0', 6);*//*M.K.12.01.2003 18:01*/
	memset(Sale_Date_Char,'0',6);
	Sale_Date_Char[6] = 0;
	}
	/* Set Debit and Amex flags and second copy flag*/

	VOID_FLAG       = FALSE;
	GVOID_FLAG      = FALSE;
	DEBIT_FLAG      = FALSE;
	AMEX_FLAG       = FALSE;
	SECOND_COPY     = FALSE;
	

	/* FAIZLI 02_00 25/07/2000 her iþlem baþlarken faizli taksitli deðilmiþ olarak kabul edilir */
	FAIZLI_FLAG = FALSE;

	/* debit prefix check */
	if (!IsEMV) DEBIT_FLAG = Utils_Debit_Check();
	else DEBIT_FLAG = FALSE;


	/* LRC Control */
	/* Check for AMEX*/
	if ((memcmp(card_no, "34", 2) == 0) || (memcmp(card_no, "37", 2)==0))
	{
	  AMEX_FLAG = TRUE;
	}

	/* Check for ATS_Prefixes*/
	memcpy(first_6_digit,card_no,6);
	first_6_digit[6] = 0;

	/** XLS_INTEGRATION2 10/10/2001 irfan. taksit sormak icin hem ats hem de loyalty prefixleri search ediliyor **/
	if( (strstr(ATS_Prefixes, first_6_digit) != NULL) || (strstr(Loyalty_Prefixes, first_6_digit) != NULL) )
	{
		ATS_PERMISSION_FLAG = TRUE;
	}
	else
	{
		ATS_PERMISSION_FLAG = FALSE;
	}

	/* XLS_PAYMENT 12/07/2001 irfan */
	/* Check for Loyalty_Prefixes*/
	memcpy(first_6_digit,card_no,6);
	first_6_digit[6] = 0;

	if((strstr(Loyalty_Prefixes, first_6_digit) != NULL) && (!CORRUPTED_CHIP_TRANSACTION_FLAG) &&
		((tran_type == SALE) || (tran_type == MANUEL) || (tran_type == RETURN)))
	{
		LOYALTY_PERMISSION_FLAG = TRUE;
	}
	else
	{
		LOYALTY_PERMISSION_FLAG = FALSE;
	}

	/* VERDE send card no in res1 and receive Req1 */
	if(	VERDE_TRANSACTION)
	{
		glAmount_Req_Type = '1';
		if(Verde_SendRes1_GetReq1(IsEMV) != STAT_OK)
		{
			VERDE_TRX_CONTINUE = FALSE;		/* verde trx. must be stop. normal trx. goes on */
			if((Verde_SendControlChar(EOT)) != STAT_OK) 
			{
				/* 06_04 debug mode kontrolu eklendi*/
				if (DEBUG_MODE) Slips_PrinterStr("\n EOT GONDERILEMEDI-1 \n "); 
				return(STAT_NOK);
			}
		}

		/* amount verde pod tarafýndan saglanacak */
		if( (glAmount_Req_Type == '2') && VERDE_TRX_CONTINUE)
		{
			if(Verde_IdleLoopForReq2() == STAT_OK)
			{
//bm 15.12.2004 Utils_Subtract_Strings(verde_amount1, verde_discount, verde_amount);
//bm 15.12.2004 memcpy(amount_surcharge, verde_amount,13);
				amount_surcharge[13] = 0;
				Utils_LeftPad(amount_surcharge, '0', 13);
			}
			else
			{
				VERDE_TRX_CONTINUE = FALSE;		/* verde trx. must be stop. normal trx. goes on */
				if((Verde_SendControlChar(EOT)) != STAT_OK) 
				{
					/* 06_04 debug mode kontrolu eklendi*/
					if (DEBUG_MODE) Slips_PrinterStr("\n EOT GONDERILEMEDI-2 \n "); 
					return(STAT_NOK);
				}
				return(STAT_NOK);

			}
		}
	}

	/* Toygar EMVed */
	if (IsEMV)
	{
		if (Main_CheckEMV(pelsLangs, etdData, pbErr) != STAT_OK)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("KART EMV        OLARAK OKUNAMADI",1);
			Mainloop_DisplayWait("   KARTINIZI      CIKARTINIZ    ",NO_WAIT);
			/*Host_PinpadIdlePrompt ();*/
			return STAT_NOK;
		}
		/*Host_PinpadIdlePrompt();*/
		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
/*		Trans_PassEMVData(fpPTStruct, etdData);*/

		/* Terminal Risk Management - BlackList - ForcedOnline - Limit - Lookup*/
/* Toygar - NextEMV Start - Ok */
/*
		EmvAct_ManageTerminalRisk(eaApplication);
*/
		Emv_ManageTerminalRisk(pelsLangs, pAppData, pbErr);
/* Toygar - NextEMV End - Ok */
		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
/*		Trans_PassEMVData(fpPTStruct, etdData);*/

		/* Terminal Action Analysis */
/* Toygar - NextEMV Start - Ok */
/*		
		switch(EmvAct_AnalyzeTerminalAction(eaApplication, etdData))
*/
/* Toygar - NextEMV End - Ok */

		switch(Emv_AnalyzeTerminalAndCardAction(pelsLangs, pAppData, &pbMessage, pbErr))		
		{
		case EMV_TERMINATE_TRN: 
			if (EMV_DEBUG_MODE || TVR_TSI)
			{
				Printer_WriteStr("EMV_TERMINATE_TRN\n");
				if(EmvDb_Find(&rdData, tagTVR))
				{
					Printer_WriteStr("TVR:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
				if(EmvDb_Find(&rdData, tagTSI))
				{
					Printer_WriteStr("TSI:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
					Printer_WriteStr("\n\n\n");
				}
			}
			Main_TerminateTran(BadICC);
			Mainloop_DisplayWait("KART ISLEMI     OFFLINE KESTI",1);
								return STAT_NOK;
		case EMV_ONLINE:
			if (EMV_DEBUG_MODE || TVR_TSI)
			{
				Printer_WriteStr("EMV_ONLINE\n");
				if(EmvDb_Find(&rdData, tagTVR))
				{
					Printer_WriteStr("TVR:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
				if(EmvDb_Find(&rdData, tagTSI))
				{
					Printer_WriteStr("TSI:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
					Printer_WriteStr("\n\n\n");
				}
			}
			CallFlag = TRUE;
								break;
		case EMV_APPROVED:
			if (EMV_DEBUG_MODE || TVR_TSI)
			{
				Printer_WriteStr("EMV_APPROVED\n");
				if(EmvDb_Find(&rdData, tagTVR))
				{
					Printer_WriteStr("TVR:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
				if(EmvDb_Find(&rdData, tagTSI))
				{
					Printer_WriteStr("TSI:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
					Printer_WriteStr("\n\n\n");
				}
			}
			CallFlag = FALSE;
			fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
			memcpy(fpPTStruct->AID, pAppData->AID, pAppData->AID_Length);
			fpPTStruct->AIDLen = pAppData->AID_Length;
/*			Trans_PassEMVData(fpPTStruct, etdData);*/
			ICC_Disactive (0);
			break;
		case EMV_DECLINED:
			if (EMV_DEBUG_MODE || TVR_TSI)
			{
				Printer_WriteStr("EMV_DECLINED\n");
				if(EmvDb_Find(&rdData, tagTVR))
				{
					Printer_WriteStr("TVR:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
				if(EmvDb_Find(&rdData, tagTSI))
				{
					Printer_WriteStr("TSI:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
					Printer_WriteStr("\n\n\n");
				}
			}
			EmvIF_DisplayWait((EMVLanguages *)&elsLangs, DSP_MERCHANT, msg_EMV_DECLINED, 1); 
			CallFlag = FALSE;
			fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
/*			Trans_PassEMVData(fpPTStruct, etdData);*/
			ICC_Disactive (0);
//			Mainloop_DisplayWait("KART ISLEMI     OFFLINE REDDETTI",1);
			Mainloop_DisplayWait("KART ISLEMI     REDDETTI",1);
			return(STAT_NOK);
			break;
		case EMV_FALLBACK: 
	      		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
    			if (Trans_IsFallbackAllowed())
      			{
				BadICC = 1;
				Main_TerminateTran(BadICC);
				Mainloop_DisplayWait("ISLEMI FALLBACK OLARAK YAPIN",1);
				return STAT_NOK;
			}
		/* Otherwise terminate the transaction */
		default: /* EMV_TERMINATE_TRN */
			fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
			EmvIF_ClearDsp(DSP_MERCHANT);
			EmvIF_DisplayWait((EMVLanguages *)&elsLangs, DSP_MERCHANT, msg_EMV_DECLINED, 1);
			Main_TerminateTran(0);
			Mainloop_DisplayWait("BILINMEYEN EMV  OFFLINE HATASI",1);
			return STAT_NOK;
		}
		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
/*		Trans_PassEMVData(fpPTStruct, etdData);*/
		memset(fpPTStruct->Pan, 0, sizeof(fpPTStruct->Pan));
		EmvDb_Find(&myrdData, tagPAN);
/*
		Printer_WriteStr("myrdData.value\n");
		PrntUtil_BufAsciiHex((byte *)myrdData.value,myrdData.length);
		Printer_WriteStr("fpPTStruct->Pan\n");
		PrntUtil_BufAsciiHex((byte *)fpPTStruct->Pan,sizeof(fpPTStruct->Pan));
*/		
		EmvIF_BinToAscii(myrdData.value, fpPTStruct->Pan, myrdData.length, sizeof(fpPTStruct->Pan), FALSE);
		while (fpPTStruct->Pan[strlen(fpPTStruct->Pan) - 1] == 'F') fpPTStruct->Pan[strlen(fpPTStruct->Pan) - 1] = 0;
		fpPTStruct->PanLength = strlen(fpPTStruct->Pan);
		if ((POSEntryFlag == 2) || (POSEntryFlag == 1)) CallFlag = TRUE;
		memset(card_info,' ',40);
		memset(card_no,0,20);
		memset(exp_date,0,5);
		memset(cvv_2,0,4);
		memset(card_info1_name, 0, sizeof(card_info1_name));
		memset(card_info1_surname, 0, sizeof(card_info1_surname));		

		memset(tempchnm,' ',50);	
		pbIndex = (byte *)tempchnm;
		EmvIF_AppendData(&pbIndex, tagCARDHOLDER_NAME, FALSE, 26);
		memcpy(card_info1_name, tempchnm, sizeof(card_info1_name));

		memset(tempchnm,' ',50);	
		pbIndex = (byte *)tempchnm;
		EmvIF_AppendData(&pbIndex, tagCRDHLDR_NAME_EXT, FALSE, 45);
		memcpy(card_info1_surname, tempchnm, sizeof(card_info1_surname));

		memcpy(card_info, fpPTStruct->Pan, fpPTStruct->PanLength);
		memset(temp_cn,' ',20);	
		pbIndex = (byte *)temp_cn;
		EmvIF_AppendData(&pbIndex, tagAPPL_EXPIRATION_DATE, FALSE, 0);
		card_info[fpPTStruct->PanLength] = '=';
		EmvIF_BinToAscii(temp_cn,card_info+fpPTStruct->PanLength+1,2,4,FALSE);
		memcpy(card_info+fpPTStruct->PanLength+5,"000",3);
		memcpy(card_no, fpPTStruct->Pan, fpPTStruct->PanLength);
		memcpy(exp_date,card_info+fpPTStruct->PanLength+1,4);
		memcpy(cvv_2,"000",3);
		memset(remained_chip_data,0,sizeof(remained_chip_data));
		memcpy(remained_chip_data,card_info+fpPTStruct->PanLength+1,7);
/**/
		if (DEBUG_MODE)
		{
			Printer_WriteStr("card_info1\n");
			PrntUtil_BufAsciiHex((byte *)card_info,40);
		}
/**/
		if (EmvDb_Find(&rdData, tagTRACK2_EQUIVALENT_DATA))
		{
/**/
			if (DEBUG_MODE)
			{
				Printer_WriteStr("tagTRACK2_EQUIVALENT_DATA:\n");
				PrntUtil_BufAsciiHex((byte *)rdData.value,rdData.length);
			}
/**/
			EmvIF_BinToAscii(rdData.value,card_info,rdData.length,40,FALSE);
			j = rdData.length * 2;
			for(i=0;i<40;i++)
				if (i >= j) card_info[i] = ' ';
				else if (card_info[i] == 'D') card_info[i] = '=';
				else if (card_info[i] == 'F')
				{
					card_info[i] = ' ';
					j = i;
				}
		}

/*
		if (EmvDb_Find(&rdData, tagPAN_SEQUENCE_NUMBER))
		{
		if (DEBUG_MODE)
		{
				Printer_WriteStr("tagPAN_SEQUENCE_NUMBER:\n");
				PrntUtil_BufAsciiHex((byte *)rdData.value,rdData.length);
			}
		}
		
		if (DEBUG_MODE)
		{
			Printer_WriteStr("card_info2\n");
			PrntUtil_BufAsciiHex((byte *)card_info,40);
		}
*/
/* Toygar - New EMV - Start */
/*
		EmvIF_InsertTransData(etdData, eaApplication);
*/
/* Toygar - New EMV - End */
		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
/*		Trans_PassEMVData(fpPTStruct, etdData);
		memcpy(amount_surcharge,fpPTStruct->Amount,sizeof(amount_surcharge));*/
		for(i=0;i<sizeof(amount_surcharge);i++) if (!isdigit(amount_surcharge[i])) break;
		if (i<sizeof(amount_surcharge)) amount_surcharge[i] = 0;
		Utils_LeftPad(amount_surcharge, '0', 13);

		if (!EMVAxessStatu)
		{
		ATS_Count=1;
		strcpy(ATS_Count_c,"01");
		ATS_Count_c[2] = 0;
		}
		if ( EMVAxess )
		{
			if ( tran_type != BALANCE )
			{
				/* 20.07.2004
				 * if (!Trans_GetAmount(lbSend_Buffer)) 
				 * {
				 * if(VERDE_TRANSACTION)
				 * {
				 * if((Verde_SendControlChar(EOT)) != STAT_OK) 
				 * {
				 * if (DEBUG_MODE) Slips_PrinterStr("\n EOT GONDERILEMEDI-1 \n "); 
				 * }
				 * }
				 * return(STAT_NOK);
 * } *//* 20.07.2004 comment icindeki kaldirildi */

				/* 20.07.2004 global da saklanan concilation data alindi */
				memcpy( lbSend_Buffer, Received_Buffer, 900 );
			}
			else
				memset( amount_surcharge, 0, sizeof( amount_surcharge ) );
		}

	}
	else
	{
		if (tran_type != BALANCE)
		{
			/* 06_00 concilation data parametre olarak eklendi */
/*
			memset(lConcilation_Data, 0, sizeof(lConcilation_Data));
			if (!Trans_GetAmount(lConcilation_Data)) return(STAT_NOK);
*/
/* LastToygar */
			/* if (!Trans_GetAmount(lbSend_Buffer)) return(STAT_NOK); *//* 06_33 21/11/2003 irfan . verde icin amount girisi cancel ile kesilirse EOT gonderilmesi saglandi */
			if ( !Trans_GetAmount( lbSend_Buffer, Kampanya ) )
			{
				if(VERDE_TRANSACTION)
				{
					if((Verde_SendControlChar(EOT)) != STAT_OK) 
					{
						/* 06_04 debug mode kontrolu eklendi*/
						if (DEBUG_MODE) Slips_PrinterStr("\n EOT GONDERILEMEDI-1 \n "); 
					}
				}
				return(STAT_NOK);
			}
/* LastToygar */
		}
		else memset(amount_surcharge, 0, sizeof(amount_surcharge));
	}
/* Toygar EMVed */


	/* 06_07 verde den tutar alýnýyor */
	/* VERDE. amount pos tarafýndan saglanacak ise calistirilir */
	if(VERDE_TRANSACTION && EMVAxessStatu)
	{
		if( (glAmount_Req_Type == '1') && VERDE_TRX_CONTINUE )
		{
			if(Verde_SendRes2_GetReq2(amount_surcharge_saved, verde_discount) != STAT_OK)
			{
				VERDE_TRX_CONTINUE = FALSE;		/* verde trx. must be stop. normal trx. goes on */
				if((Verde_SendControlChar(EOT)) != STAT_OK) 
				{
					/* 06_04 debug mode kontrolu eklendi*/
					return(STAT_NOK);
				}
			}
			else
			{		
//bm 15.12.2004 Utils_Subtract_Strings(amount_surcharge, verde_discount, verde_amount);
//bm 15.12.2004 memcpy(amount_surcharge, verde_amount,13);
				amount_surcharge[13] = 0;
				Utils_LeftPad(amount_surcharge, '0', 13);
			}
		}
	}


	/*----------- GET APPROVAL CODE -----------*/
	if ( ( tran_type == MANUEL && !EMVAxess) || ( IsEMV && ( tran_type == RETURN ) && !EMVAxessStatu ) )
	{
		boolean FAIZLI_IADE_OK;
		char sale_date_day[3], sale_date_month[3];
		sint sale_date_day_value, sale_date_month_value;
		char Buffer[LEN_GETKEYBOARD];
		byte Len;
		
		if (!Trans_GetApprovalCode())
		{
			EMVAxessStatu=FALSE; /*@bm 16.09.2004 */
			EMVAxess = FALSE;
			return ( STAT_NOK );
		}
		if (tran_type == RETURN)
		do
		{
			/** XLS_INTEGRATION2 irfan. 08/11/2001. GGAAYY formatina cevrildi **/
			if ( !Mainloop_GetKeyboardInput
				 ( "SATIž TRH?GGAAYY", Buffer, 6, 6, FALSE, FALSE,
				   FALSE, FALSE, FALSE, &Len, " ", TRUE ) )
			{
				EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				return ( FALSE );
			}
			else
			{
				if ( Len == 0 )
				{
					FAIZLI_IADE_OK = FALSE;
				}
				else
				{
					sale_date_month[0] = Buffer[2];
					sale_date_month[1] = Buffer[3];
					sale_date_month[2] = 0;
					sale_date_month_value = atoi( sale_date_month );
					sale_date_day[0] = Buffer[0];
					sale_date_day[1] = Buffer[1];
					sale_date_day[2] = 0;
					sale_date_day_value = atoi( sale_date_day );

					if ( ( sale_date_month_value <= 12 ) &&
						 ( sale_date_month_value >= 1 ) &&
						 ( sale_date_day_value <= 31 ) &&
						 ( sale_date_day_value >= 1 ) )
					{
						if ( sale_date_month_value == 2 )
						{
							if ( sale_date_day_value > 29 )
							{
								FAIZLI_IADE_OK = FALSE;
							}
							else
							{
								/** sisteme gidecek tarih yyaagg olmalý **/
								/*memcpy(Sale_Date_Char, Buffer, 6); */
								Sale_Date_Char[0] = Buffer[4];
								Sale_Date_Char[1] = Buffer[5];
								Sale_Date_Char[2] = Buffer[2];
								Sale_Date_Char[3] = Buffer[3];
								Sale_Date_Char[4] = Buffer[0];
								Sale_Date_Char[5] = Buffer[1];
								Sale_Date_Char[6] = 0;
								FAIZLI_IADE_OK = TRUE;
							}
						}
						else
						{
							/** sisteme gidecek tarih yyaagg olmalý **/
							/*memcpy(Sale_Date_Char, Buffer, 6); */
							Sale_Date_Char[0] = Buffer[4];
							Sale_Date_Char[1] = Buffer[5];
							Sale_Date_Char[2] = Buffer[2];
							Sale_Date_Char[3] = Buffer[3];
							Sale_Date_Char[4] = Buffer[0];
							Sale_Date_Char[5] = Buffer[1];
							Sale_Date_Char[6] = 0;
							FAIZLI_IADE_OK = TRUE;
						}
					}
					else
					{
						FAIZLI_IADE_OK = FALSE;
					}
				}
			}
		}
		while ( !FAIZLI_IADE_OK );
	}

	/** XLS_PAYMENT 01/08/2001 irfan **/
/*	if(OFFLINE_TRX) return(TransXLSOfflineTransaction());*/	/* 06_30 kaldirildi */
/* Toygar EMVed *//* 06_20 IsEMV condition controlu eklendi */
	if (IsEMV)
	{
	     if(!CallFlag) 
	    {
		Trans_FillTransactionSend(fpPTStruct, IsEMV, Kampanya);
		return(TransEMVOfflineTransaction(fpPTStruct));
	    }
	}
/* Toygar EMVed */

	if(DEBIT_FLAG)
	{

#ifndef _8010
		if( (Test_IsEeprom() > 0) && (!EXTERNAL_PINPAD) )
#else
		if(TRUE)
#endif
		{
			if(Verde_AskPIN_Embedded (encrypted_PIN) != STAT_OK)
			{
				Mainloop_DisplayWait("EMBEDDED PINPAD SIF. SORMA HATA ",1);      
				return(STAT_NOK);
			}
		}
		else
		{/* embedded pinpad is not available . external pinpad must be used*/
			GrphText_Cls (TRUE);
			Debug_GrafPrint2(" ž˜FREY˜ G˜R˜N ?",2);

			/*Select Which Master Key is used */
			if(Utils_SelectMasterKey () != STAT_OK)
			{
				Mainloop_DisplayWait("  ž˜FRE C˜HAZI    BA¦LI DE¦˜L   ",2);
				return(STAT_NOK);
			}

			if(Host_PinpadIdlePrompt() != STAT_OK)
			{
				Mainloop_DisplayWait("  ž˜FRE CÝHAZI    BA¦LI DE¦˜L   ",2);
				return(STAT_NOK);
			}

			/*Get the PIN from PINPAD and prepare the PIN Block */
			if(Trans_AskPin (encrypted_PIN) != STAT_OK)
			{
				Mainloop_DisplayWait("SIF. SORMA HATA",1);      
				return(STAT_NOK);
			}
		}
	}

	/******* 06_30  Offline consecutive counter'in resetlenmesi saglanir ********/
	/* read offline paramter from ram disk */
	if(Offline_ReadOfflineParameters(&lbOfflineParametersStruct) !=STAT_OK)
	{
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    OKUMA  HATASI  ***");
		PrntUtil_Cr(4);
		Mainloop_DisplayWait("OFF.PAR.OKUMA.HT",1);
		return (FALSE); 
	}
	lbOfflineParametersStruct.VAR_max_offline_trx_no_concecutive = 0 ;

	if(Offline_WriteOfflineParameters(&lbOfflineParametersStruct) != STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    YAZMA  HATASI  ***");
		PrntUtil_Cr(2);
		Mainloop_DisplayWait("OFF.PAR.YAZMA.HT",1);
		return (STAT_NOK); 
	}

	/**********************************************************************************/
	
	/* initialize ilk telefon arama flag to false */
	FIRST_PHONE_FLAG         = FALSE;
	SERVER_OPEN_TIMEOUT_FLAG = FALSE;



	ConnectionSuccessfull = TRUE;
	/* Provide the connection with HOST. Host Predial had been provided beforehand */
	Transaction_Ret = Host_Connect(TRUE);
	/* eger server time out oldu ise ve birinci tel arandý ise*/
	if ((Transaction_Ret !=STAT_OK) && (FIRST_PHONE_FLAG))
	{
		Host_Disconnect();
		Mainloop_DisplayWait("2. SERVER ARAMA ",0);
		Mte_Wait(2500); /* disconnection ýn saðlanmasý icin bekleniyor */
		memcpy(tel_no_1_temp, tel_no_1, strlen(tel_no_1));
		tel_no_1_temp[strlen(tel_no_1)] = 0;
		memcpy(tel_no_2_temp, tel_no_2, strlen(tel_no_2));
		tel_no_2_temp[strlen(tel_no_2)] = 0;
		memcpy(tel_no_1, tel_no_2, strlen(tel_no_2));
		tel_no_1[strlen(tel_no_2)] = 0;
		tel_no_2[0] = 0;
		SERVER_OPEN_TIMEOUT_FLAG = TRUE;

		/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
		/*Host_PreDial();*/
		if(Host_PreDial() != STAT_OK) 
		{
			/* 07_11 20/11/2002 arama probleminde Verde POD'a EOT gonderiliyor */
			/* 07_16 26/11/2002 sadece verde pos icin gonderimi saglandi */
			
			/* 06_08 EOT gonderimi Verde transaction devam edecekse olmali */
			/*if(VERDE_TRANSACTION) Verde_SendControlChar(EOT);*/
			if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) Verde_SendControlChar(EOT);

			Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
			if (IsEMV) 
			{
				if (DEBUG_MODE) Printer_WriteStr("ConnectionSuccessfull#1\n");
				ConnectionSuccessfull = FALSE; 
			}
			else return(STAT_NOK);
		}
		Transaction_Ret = Host_Connect(TRUE);

		/* tel no larý eski haline getiriliyor*/
		memcpy(tel_no_1, tel_no_1_temp, strlen(tel_no_1_temp));
		tel_no_1[strlen(tel_no_1_temp)] = 0;
		memcpy(tel_no_2, tel_no_2_temp, strlen(tel_no_2_temp));
		tel_no_2[strlen(tel_no_2_temp)] = 0;
	}
	if(Transaction_Ret != STAT_OK)
	{
		Host_Disconnect();
		/* 07_11 20/11/2002 arama probleminde Verde POD'a EOT gonderiliyor */
		/* 07_16 Sadece verde pos olmasi durumunda gonderimi saglandi */
		/* 06_08 EOT gonderimi Verde transaction devam edecekse olmali */
		/*if(VERDE_TRANSACTION) Verde_SendControlChar(EOT);*/
		if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) Verde_SendControlChar(EOT);
		if (IsEMV)
		{
			if (DEBUG_MODE) Printer_WriteStr("ConnectionSuccessfull#2\n");
			ConnectionSuccessfull = FALSE;
		}
		else return(Transaction_Ret);
	}

	if (ConnectionSuccessfull)
	{
	/* Toygar - After 8320 */
		if (ChangesPending)
		{

			Trans_SendReversal(CHANGES, fpPTStruct, IsEMVReversal);
		}
		else if (ReversalPending)
		{
			if(Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMVReversal) != STAT_OK) 
			return (STAT_NOK);
		}
	
	/* Prepare the transaction Message*/
		IsEMVReversal = IsEMV;
		Trans_FillTransactionSend( fpPTStruct, IsEMV, Kampanya );
		/* bm opt 09/07/2004   stack    */
		memset( Reversal_Buffer, 0, 900 );
		memcpy( Reversal_Buffer, lbSend_Buffer, 900 );

/*
		Printer_WriteStr("Reversal_Buffer#1:\n");
		PrntUtil_BufAsciiHex((byte *)Reversal_Buffer,strlen(Reversal_Buffer));
*/

		memset( lbSend_Buffer, 0, sizeof( lbSend_Buffer ) );
	if (IsEMV) 
	{
			memcpy( lbSend_Buffer, &d_Transaction_Send,
					sizeof( d_Transaction_Send ) );
	}
	else
	{
			memcpy( lbSend_Buffer, &d_Transaction_Send,
					sizeof( d_Transaction_Send ) -
					sizeof( d_Transaction_Send.EMVFields ) );
		}

/*
	Printer_WriteStr("lbSend_Buffer:\n");
	PrntUtil_BufAsciiHex((byte *)lbSend_Buffer,strlen(lbSend_Buffer));
*/
	/* 06_00 axess islemleri icin concialtion data ekleniyor. irfan 25/09/2002 */
		if ( ( XLS_Check_Initialization_State(  ) == XLS_OPERATIVE )
			 && ( ( !IsEMV ) || ( EMVAxess ) ) && ( LOYALTY_PERMISSION_FLAG )
			 && ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )	/* bm OPT 24/06%2004 */
			 && ( ( tran_type == SALE ) || ( tran_type == MANUEL ) ||
				  ( tran_type == RETURN ) ) )
	{
		strcat( lbSend_Buffer, Reversal_Buffer);
	}

		/*bm OPT */
	memset(Reversal_Buffer,0,sizeof(Reversal_Buffer));
	strcpy(Reversal_Buffer,lbSend_Buffer);
	Reversal_Buffer_Len = strlen(Reversal_Buffer);

/*
	Printer_WriteStr("Reversal_Buffer#2:\n");
	PrntUtil_BufAsciiHex((byte *)Reversal_Buffer,strlen(Reversal_Buffer));
*/

	/* Send the message to the Host and get the response*/
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("YANIT BEKL˜YOR!. ",NO_WAIT);
/*
	Printer_WriteStr("Sent:\n");
	PrntUtil_BufAsciiHex((byte *)lbSend_Buffer,strlen(lbSend_Buffer));
*/
/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
	else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);
/* Toygar 13.05.2002 End */
/*
	Printer_WriteStr("Receive:\n");
	PrntUtil_BufAsciiHex((byte *)lbSend_Buffer,strlen(lbSend_Buffer));
*/
	if(Transaction_Ret != STAT_OK)
	{
		/* 06_07 24/12/2002 A.Y. */
		if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE )
		{	 
			Verde_SendControlChar(EOT);
			VERDE_TRX_CONTINUE = FALSE;
		}
	

/* Toygar 13.05.2002 Start */
		if ((Transaction_Ret == AKNET_POS_TIMEOUT_ERROR) || (Transaction_Ret == STAT_TIMEOUT)) 
		{
			if (tran_type != BALANCE)
			{
				 ReversalPending = TRUE;
				if (Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV) != STAT_OK) return (STAT_NOK);
			}
		}

		if (tran_type != BALANCE)
		{
			if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) /*bm 21.10.2004 */
			{	/* send RES3 get ACK  */
				if(Verde_SendRes3("9999") != STAT_OK)	Verde_SendControlChar(EOT);
			}

			if(!Slips_FillErrorReceiptStructure())
			{
				Slips_PrinterStr("\nSlips_FillErrorReceiptStructure hatasi\n");
				return(STAT_NOK);
			}
			Slips_PrintErrorReceipt(FALSE);
			ERROR_RECEIPT = TRUE;
		}
		return(Transaction_Ret);
/* Toygar 13.05.2002 End */
	}

	/* UPDATE 01_16 26/10/99
	   Transaction_Ret kontrolünün üzerinde idi. Altýna alýndý. */
	/* Move Received_Buffer to the Transaction_Receive */
	memcpy(&d_Transaction_Receive,Received_Buffer+1,sizeof(d_Transaction_Receive));

	/* Analyze the response*/
	ret_code = Trans_AnalyseSaleTransactionReceive();

	if (ret_code == PIN_REQUESTED_MESSAGE)
	{
		DEBIT_FLAG = TRUE;

		/* GETPIN FUNCTION */
#ifndef _8010
		if( (Test_IsEeprom() > 0) && (!EXTERNAL_PINPAD) )
#else
		if(TRUE)
#endif
		{
			if(Verde_AskPIN_Embedded (encrypted_PIN) != STAT_OK)
			{
				Mainloop_DisplayWait("EMBEDDED PINPAD SIF. SORMA HATA ",1);      
				return(STAT_NOK);
			}
		}
		else
		{/* embedded pinpad is not available . external pinpad must be used*/
			/*Select Which Master Key is used */
			GrphText_Cls (TRUE);
			Debug_GrafPrint2(" ž˜FREY˜ G˜R˜N ?",2);

			if(Utils_SelectMasterKey () != STAT_OK)
			{
				Mainloop_DisplayWait("  ž˜FRE C˜HAZI    BA¦LI DE¦˜L   ",2);
				return(STAT_NOK);
			}

			if(Host_PinpadIdlePrompt() != STAT_OK)
			{
				Mainloop_DisplayWait("  ž˜FRE CÝHAZI    BA¦LI DE¦˜L   ",2);
				return(STAT_NOK);
			}

			/*Get the PIN from PINPAD and prepare the PIN Block */
			if(Trans_AskPin (encrypted_PIN) != STAT_OK)
			{
				Mainloop_DisplayWait("SIF. SORMA HATA",1);      
				return(STAT_NOK);
			}
		}

		
		memcpy(d_Transaction_Send.encrypted_PIN, encrypted_PIN, 16);

		/* Prepare the transaction Message*/   /* UPDATE_0_12 20/09/99 */
			memset( lbSend_Buffer, 0, 900 );
		memset(Reversal_Buffer,0,sizeof(Reversal_Buffer));
		memcpy (Reversal_Buffer,&d_Transaction_Send,sizeof(d_Transaction_Send));    /* UPDATE_0_12 20/09/99 */
		memcpy( lbSend_Buffer, &d_Transaction_Send, sizeof( d_Transaction_Send ) );	/* UPDATE_0_12 20/09/99 */
		/* RESend the message to the Host and get the response*/
		EmvIF_ClearDsp(DSP_MERCHANT);

		Mainloop_DisplayWait("YANIT BEKL˜YOR.. ",NO_WAIT);
/*
			Printer_WriteStr("Reversal_Buffer#3:\n");
			PrntUtil_BufAsciiHex((byte *)Reversal_Buffer,strlen(Reversal_Buffer));
*/

/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
		else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);
/* Toygar 13.05.2002 End */

		if(Transaction_Ret != STAT_OK)
		{			
			/* 06_07 24/12/2002 A.Y. */
			if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE )
			{	 
				Verde_SendControlChar(EOT);
				VERDE_TRX_CONTINUE = FALSE;
			}

/* Toygar 13.05.2002 Start */
			if ((Transaction_Ret == AKNET_POS_TIMEOUT_ERROR) || (Transaction_Ret == STAT_TIMEOUT)) 
			{
				if (tran_type != BALANCE)
				{
						if (DEBUG_MODE) Printer_WriteStr("Check #2\n");
					 ReversalPending = TRUE;
					if (Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV) != STAT_OK) return (STAT_NOK);
				}
			}
/*
			if (BUYUKFIRMA)
			{
				if (Transaction_Ret != STAT_TIMEOUT) confirmation_ret = Trans_FillnSend_Confirmation(FALSE);
			}
*/
/* Toygar 13.05.2002 End */
			if (tran_type != BALANCE)
			{
				if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) /*bm 21.10.2004 */
				{	/* send RES3 get ACK  */
					if(Verde_SendRes3("9999") != STAT_OK)	Verde_SendControlChar(EOT);
				}
				if(!Slips_FillErrorReceiptStructure())
				{
					Mainloop_DisplayWait("Slips_FillErrorReceiptStructure hatasi",3);	
					return(STAT_NOK);
				}

				Slips_PrintErrorReceipt(FALSE);
				ERROR_RECEIPT = TRUE;
			}
			return(Transaction_Ret);
		}

		/* Move Received_Buffer to the Transaction_Receive */
		memcpy(&d_Transaction_Receive,Received_Buffer+1,sizeof(d_Transaction_Receive));

		ret_code = Trans_AnalyseSaleTransactionReceive();
	}
/* Toygar EMVed */

	}

	if (IsEMV)
	{
			/*char ResCode;*/
			byte bConnection;
			int i, isec;
			RowData rdIAD;
			RowData rdIST;
			byte abPK[MAX_PK_DATA_LEN];        /*Stores value for final constructed object*/
  			RowData rdPK;
			byte *pbIndex;       /*Position on PK buffer*/
			byte *pbEndAddress;  /*End Address for PK buffer*/
			byte esRes = EMV_OK;
			byte esRessec = EMV_OK;
			char temporary[256];
			char temporarysec[256];
			char scriptone[128];
			char scripttwo[128];
			RowData rdISTsec;

			memset(abPK, 0, sizeof(abPK));
/* Toygar - NextEMV Start - Ok */
			rdPK = BerTlv_AsBufToRd(abPK, sizeof(abPK));
/*
			BerTlv_AssignBufferToRd(&rdPK, abPK, sizeof(abPK));
*/
/* Toygar - NextEMV End - Ok */
			pbIndex = abPK;
			pbEndAddress = rdPK.value + rdPK.length;

			memset(temporary, 0, 256);
		if (ConnectionSuccessfull)
		{
			for(i=0;i<32;i++) if (d_Transaction_Receive.EMVFields.IAD[i] == ' ') break;
			Host_AsciiToBinMy(d_Transaction_Receive.EMVFields.IAD,(char*)temporary, i);
			i /= 2;
/**/
			if (DEBUG_MODE)
			{
				Printer_WriteStr("IAD:\n");
				PrntUtil_BufAsciiHex((byte *)temporary,i);
				Printer_WriteStr("ISTNo:\n");
				PrntUtil_BufAsciiHex((byte *)&d_Transaction_Receive.EMVFields.ISTNo,1);
			}
/**/
/* Toygar - NextEMV Start - Ok */
			rdIAD = BerTlv_AsBufToRd((byte *)temporary, i);
/*
			BerTlv_AssignBufferToRd(&rdIAD, temporary, i);
*/
/* Toygar - NextEMV End - Ok */

				esRes = EmvDb_AppendTLVDataToBuffer(tagISSUER_AUTH_DATA, rdIAD, &pbEndAddress, &pbIndex);
				if (esRes == EMV_FAIL)
				{
				if ((tran_type != BALANCE) && (ConnectionSuccessfull))
					{
						if (ret_code == APPROVED_MESSAGE)
						{
						if (DEBUG_MODE) Printer_WriteStr("Check #3\n");
							ReversalPending = TRUE;
							Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);
						}
						if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) /*bm 21.10.2004 */
						{	/* send RES3 get ACK  */
							if(Verde_SendRes3("9999") != STAT_OK)	Verde_SendControlChar(EOT);
						}
						if(!Slips_FillErrorReceiptStructure())
						{
							Mainloop_DisplayWait("Slips_FillErrorReceiptStructure hatasi",3);
							return(STAT_NOK);
						}
						Slips_PrintErrorReceipt(FALSE);
						ERROR_RECEIPT = TRUE;
					}
/*				Printer_WriteStr("EMV_FAIL #1\n");*/
				Mainloop_DisplayWait("ISSUER SCRIPT   HATASI",1);
					return EMV_FAIL;
				}
			if (d_Transaction_Receive.EMVFields.ISTNo != '0')
			{
				if ((d_Transaction_Receive.EMVFields.ISTNo == '1') || (d_Transaction_Receive.EMVFields.ISTNo == '2'))
				{
				memset(temporary, 0, 256);
				for(i=0;i<256;i++) if (d_Transaction_Receive.EMVFields.IST[i] == ' ') break;
					Host_AsciiToBinMy(d_Transaction_Receive.EMVFields.IST,temporary, i);
				i /= 2;
/**/
				if (DEBUG_MODE)
				{
					Printer_WriteStr("IST:\n");
					PrntUtil_BufAsciiHex((byte *)temporary,i);
				}
/**/

					rdIST = BerTlv_AsBufToRd((byte *)temporary, i);
			}
				else if (d_Transaction_Receive.EMVFields.ISTNo == '3') 
				{
					memset(temporary, 0, 256);
					memset(temporarysec, 0 ,256);
					memset(scriptone, 0 ,128);
					memset(scripttwo, 0 ,128);
					
					memcpy(scriptone,d_Transaction_Receive.EMVFields.IST,128);
					memcpy(scripttwo,d_Transaction_Receive.EMVFields.IST+128,128);


					for(i=0;i<128;i++) if (scriptone[i] == ' ') break;
					Host_AsciiToBinMy(scriptone,temporary, i);
					i /= 2;

					for(isec=0;isec<128;isec++) if (scripttwo[isec] == ' ') break;
					Host_AsciiToBinMy(scripttwo,temporarysec, isec);
					isec /= 2;
/**/
					if (DEBUG_MODE)
			{
						Printer_WriteStr("IST1(Ascii):\n");
						PrntUtil_BufAsciiHex((byte *)scriptone,i*2);
						Printer_WriteStr("IST2(Ascii):\n");
						PrntUtil_BufAsciiHex((byte *)scripttwo,isec*2);
						Printer_WriteStr("IST1(Hex):\n");
						PrntUtil_BufAsciiHex((byte *)temporary,i);
						Printer_WriteStr("IST2(Hex):\n");
						PrntUtil_BufAsciiHex((byte *)temporarysec,isec);
					}
/**/

					rdIST = BerTlv_AsBufToRd((byte *)temporary, i);
					rdISTsec = BerTlv_AsBufToRd((byte *)temporarysec, isec);
				}
			}
			
			if (d_Transaction_Receive.EMVFields.ISTNo == '1') esRes = EmvDb_AppendTLVDataToBuffer(tagISSUER_SCRIPT_TEMPLATE_1, rdIST, &pbEndAddress, &pbIndex);
			else if (d_Transaction_Receive.EMVFields.ISTNo == '2') esRes = EmvDb_AppendTLVDataToBuffer(tagISSUER_SCRIPT_TEMPLATE_2, rdIST, &pbEndAddress, &pbIndex);
			else if (d_Transaction_Receive.EMVFields.ISTNo == '3')
			{
				esRes = EmvDb_AppendTLVDataToBuffer(tagISSUER_SCRIPT_TEMPLATE_1, rdIST, &pbEndAddress, &pbIndex);
				esRessec = EmvDb_AppendTLVDataToBuffer(tagISSUER_SCRIPT_TEMPLATE_2, rdISTsec, &pbEndAddress, &pbIndex);
			}


			if ((esRes == EMV_FAIL) || (esRessec == EMV_FAIL))
			{
				if (tran_type != BALANCE)
				{
					/* 06_03 29/11/2002 Ayhan */
					/*verde Baglanti sonrasi hata oldugu icin reversal gonderilmesi*/
					if (ret_code == APPROVED_MESSAGE)
					{
						if (DEBUG_MODE) Printer_WriteStr("Check #4\n");
						ReversalPending = TRUE;
						Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);
					}
					if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) /*bm 21.10.2004 */
					{	/* send RES3 get ACK  */
						if(Verde_SendRes3("9999") != STAT_OK)	Verde_SendControlChar(EOT);
					}
					if(!Slips_FillErrorReceiptStructure())
					{
						Mainloop_DisplayWait("Slips_FillErrorReceiptStructure hatasi",3);
						return(STAT_NOK);
					}
					Slips_PrintErrorReceipt(FALSE);
					ERROR_RECEIPT = TRUE;
				}
/*				Printer_WriteStr("EMV_FAIL #2\n");*/
				Mainloop_DisplayWait("ISSUER SCRIPT   HATASI",1);
				return EMV_FAIL;
			}
		}
			rdPK.length   = pbIndex - rdPK.value;

/* 15.08 */
			rdSDAData->value = rdPK.value;
			rdSDAData->length = rdPK.length;
/**/
			if (DEBUG_MODE)
			{
				Printer_WriteStr("To Card Start.\n");
				PrntUtil_BufAsciiHex((byte *)rdSDAData->value,rdSDAData->length);
				Printer_WriteStr(".Finish\n");
			}
/**/
		if (!ConnectionSuccessfull) bCommResult = ARC_NO_CONNECTION;
		else if (ret_code == APPROVED_MESSAGE) bCommResult = ARC_APPROVED;
			else if (ret_code == REJECTED_MESSAGE) bCommResult = ARC_DECLINED;
			else bCommResult = ARC_REFERRAL;
			bConnection = STAT_OK;

/**/
			if (EMV_DEBUG_MODE)
			{
				Printer_WriteStr("ret_code:\n");
				PrntUtil_BufAsciiHex((byte *)&ret_code, 2);
			}
/**/			
		EmvUtil_BinToAscii((char *)&bConnection, (char *)&usAuthResCode, 1);

		if (!ConnectionSuccessfull) bCommResult = ARC_NO_CONNECTION;
/* Toygar - Parametre Listesine Atilacak */
			if (EMV_DEBUG_MODE)
			{
				if(EmvDb_Find(&rdData, tagTERML_FLOOR_LIMIT))
				{
					Printer_WriteStr("FLOOR_LIMIT:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
				if(EmvDb_Find(&rdData, tagCVM_LIST))
				{
					Printer_WriteStr("CVM_LIST:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
				if(EmvDb_Find(&rdData, tagISSUER_ACTION_DEFAULT))
				{
					Printer_WriteStr("tagISSUER_ACTION_DEFAULT:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
				if(EmvDb_Find(&rdData, tagISSUER_ACTION_DENIAL))
				{
					Printer_WriteStr("tagISSUER_ACTION_DENIAL:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
				if(EmvDb_Find(&rdData, tagISSUER_ACTION_ONLINE))
				{
					Printer_WriteStr("tagISSUER_ACTION_ONLINE:\n");
					PrntUtil_BufAsciiHex(rdData.value, rdData.length);
				}
			}
/**/
			
/* Toygar - NextEMV Start - Ok */
/*
			switch(EmvAct_OnlineProcessing(eaApplication, ResCode, *rdSDAData, bConnection, etdData))
*/
/* Toygar - NextEMV End - Ok */
			etdData->bIssScrResults_Len = 0;
      			pbMessage = 0;
      			
      			/*bFakeARC = ret_code;
      			bFakeARC = bCommResult;
      			EmvUtil_BinToAscii((char *)&bFakeARC, (char *)&usAuthResCode, 1);*/
      		if (ConnectionSuccessfull) memcpy((char *)&usAuthResCode, d_Transaction_Receive.confirmation + 1, 2);

      			if ((tran_type != RETURN) && (tran_type != MANUEL))
			switch(Emv_ProcessOnlineHostData(pelsLangs, pAppData, bCommResult, usAuthResCode, *rdSDAData, etdData, &pbMessage, pbErr))
			{
				case EMV_REF_APPROVED:
				/* Sending reversal for the app - continue normal processing */
					if (EMV_DEBUG_MODE || TVR_TSI)
					{
						Printer_WriteStr("EMV_REF_APPROVED\n");
						if(EmvDb_Find(&rdData, tagTVR))
						{
							Printer_WriteStr("TVR:\n");
							PrntUtil_BufAsciiHex(rdData.value, rdData.length);
						}
						if(EmvDb_Find(&rdData, tagTSI))
						{
							Printer_WriteStr("TSI:\n");
							PrntUtil_BufAsciiHex(rdData.value, rdData.length);
							Printer_WriteStr("\n\n\n");
						}
					}
				if (DEBUG_MODE) Printer_WriteStr("Check #5\n");
				if (ConnectionSuccessfull) 
				{
					ReversalPending = TRUE;
					Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);	/* Don't break */
				}
			case EMV_APPROVED:
					if (EMV_DEBUG_MODE || TVR_TSI)
					{
						Printer_WriteStr("EMV_APPROVED\n");
						if(EmvDb_Find(&rdData, tagTVR))
						{
							Printer_WriteStr("TVR:\n");
							PrntUtil_BufAsciiHex(rdData.value, rdData.length);
						}
						if(EmvDb_Find(&rdData, tagTSI))
						{
							Printer_WriteStr("TSI:\n");
							PrntUtil_BufAsciiHex(rdData.value, rdData.length);
							Printer_WriteStr("\n\n\n");
						}
					}
					fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
					memcpy(fpPTStruct->AID, pAppData->AID, pAppData->AID_Length);
					fpPTStruct->AIDLen = pAppData->AID_Length;
	/*				Trans_PassEMVData(fpPTStruct, etdData);*/
					ReversalPending = FALSE;
				if (!ConnectionSuccessfull)
				{
					CallFlag = FALSE;
					Trans_FillTransactionSend(fpPTStruct, IsEMV, Kampanya);
					return(TransEMVOfflineTransaction(fpPTStruct));
				}
				break;
			case EMV_TERMINATE_TRN	: 
					if (EMV_DEBUG_MODE || TVR_TSI)
					{
						Printer_WriteStr("EMV_TERMINATE_TRN\n");
						Printer_WriteStr("bCommResult:\n");
						PrntUtil_BufAsciiHex((byte *)&bCommResult,sizeof(bCommResult));
						if(EmvDb_Find(&rdData, tagTVR))
						{
							Printer_WriteStr("TVR:\n");
							PrntUtil_BufAsciiHex(rdData.value, rdData.length);
						}
						if(EmvDb_Find(&rdData, tagTSI))
						{
							Printer_WriteStr("TSI:\n");
							PrntUtil_BufAsciiHex(rdData.value, rdData.length);
							Printer_WriteStr("\n\n\n");
						}
					}
/*			        Main_TerminateTran();*/
					if (ret_code == APPROVED_MESSAGE)
					{
					if (DEBUG_MODE) Printer_WriteStr("Check #6\n");
					if (ConnectionSuccessfull) 
					{
				ReversalPending = TRUE;
						Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);
					}
				}
				if ((tran_type != BALANCE) && (ConnectionSuccessfull))
				{
					if (memcmp(d_Transaction_Receive.confirmation, "000", 3) == 0) 
					{
						strcpy(d_Transaction_Receive.message,"    ONAYLANMADI   ");
						memcpy(d_Transaction_Receive.confirmation, "ICC", 3);
					}
					if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) /*bm 21.10.2004 */
					{	/* send RES3 get ACK  */
						if(Verde_SendRes3("9999") != STAT_OK)	Verde_SendControlChar(EOT);
					}
					if(!Slips_FillErrorReceiptStructure())
					{
						Slips_PrinterStr("\nSlips_FillErrorReceiptStructure hatasi\n");
						return(STAT_NOK);
					}
					Slips_PrintErrorReceipt(FALSE);
					ERROR_RECEIPT = TRUE;
				}
//				Mainloop_DisplayWait("KART ISLEMI     ONLINE KESTI",1);
				Mainloop_DisplayWait("KART ISLEMI     KESTI",1);
				return STAT_NOK;
	/* Toygar - NextEMV Start - Ok */
	/*
			case EMV_REV_DECLINED:
				if (DEBUG_MODE) Printer_WriteStr("EMV_REV_DECLINED\n");
	*/
	/* Toygar - NextEMV Start - Ok */
			case  EMV_DECLINED:
			default:
				if (EMV_DEBUG_MODE || TVR_TSI)
				{
					Printer_WriteStr("EMV_DECLINED\n");
					if(EmvDb_Find(&rdData, tagTVR))
					{
						Printer_WriteStr("TVR:\n");
						PrntUtil_BufAsciiHex(rdData.value, rdData.length);
					}
					if(EmvDb_Find(&rdData, tagTSI))
					{
						Printer_WriteStr("TSI:\n");
						PrntUtil_BufAsciiHex(rdData.value, rdData.length);
						Printer_WriteStr("\n\n\n");
					}
				}
				fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
	/*			Trans_PassEMVData(fpPTStruct, etdData);*/
				if (ret_code == APPROVED_MESSAGE)
				{
					if (DEBUG_MODE) Printer_WriteStr("Check #7\n");
					if (ConnectionSuccessfull) 
					{
				ReversalPending = TRUE;
					Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);
				}
				}
				if ((tran_type != BALANCE) && (ConnectionSuccessfull))
				{
					if (memcmp(d_Transaction_Receive.confirmation, "000", 3) == 0) 
					{
						strcpy(d_Transaction_Receive.message,"    ONAYLANMADI   ");
						memcpy(d_Transaction_Receive.confirmation, "ICC", 3);
					}
					if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) /*bm 21.10.2004 */
					{	/* send RES3 get ACK  */
						if(Verde_SendRes3("9999") != STAT_OK)	Verde_SendControlChar(EOT);
					}
					if(!Slips_FillErrorReceiptStructure())
					{
						Slips_PrinterStr("\nSlips_FillErrorReceiptStructure hatasi\n");
						return(STAT_NOK);
					}
					Slips_PrintErrorReceipt(FALSE);
					ERROR_RECEIPT = TRUE;
				}
//				Mainloop_DisplayWait("KART ISLEMI     ONLINE REDDETTI",1);
				Mainloop_DisplayWait("KART ISLEMI     REDDETTI",1);
				return STAT_NOK;
			}
			else 
			{
				fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
				memcpy(fpPTStruct->AID, pAppData->AID, pAppData->AID_Length);
				fpPTStruct->AIDLen = pAppData->AID_Length;
				ReversalPending = FALSE;
			}
			ICC_Disactive (0);
		}
/* Toygar EMVed*/
 	if (!ConnectionSuccessfull) return(STAT_NOK);

	if ((IsEMV) && (!ReversalPending) && (tran_type != RETURN) && (tran_type != MANUEL))
	{
		fpPTStruct->Field55Len = Trans_AddChipFields((byte *)fpPTStruct->Field55, etdData);
		if (d_Transaction_Receive.EMVFields.ISTNo != '0')
		{
/*			if(Trans_SendReversal(ADVICE, fpPTStruct, IsEMV) != STAT_OK) return (STAT_NOK);*/
			Trans_SendReversal(ADVICE, fpPTStruct, IsEMV);
		}
/*		if(Trans_SendReversal(CHANGES, fpPTStruct, IsEMV) != STAT_OK) return (STAT_NOK);*/
		Trans_SendReversal(CHANGES, fpPTStruct, IsEMV);
	}

	switch (ret_code)
	{
	case UNEXPECTED_MESSAGE :
								/* 06_07 STAT_NOK öncesi Verde'ye EOT gonderiliyor */
								if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE )
								{	 
									Verde_SendControlChar(EOT);
									VERDE_TRX_CONTINUE = FALSE;
								}
								return(STAT_NOK);


		/* Toygar 13.05.2002 End */
	case APPROVED_MESSAGE   : 
		if ( tran_type != BALANCE ) 
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("ONAYLANDI",1);
			/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
			if (( POS_Type == '4' ) || ( POS_Type == '6' ))	/* axess POS icin yapiliyor */
			{
				/** XLS_PAYMENT 23/07/2001 irfan. Call XLS module for rewarding **/
				if ( ( ( !IsEMV ) || ( EMVAxess ) ) && ( LOYALTY_PERMISSION_FLAG ) )
				{
					/* 20.07.2004 axess visa degisiklikleri. mpcos os'e gecmek icin reset atildi */
					if ( EMVAxess )
					{
						if ( ResetCardExtLight(  ) != STAT_OK )
						{
							ReversalPending = TRUE; //bm 23.10.2004
							Slips_PrinterStr( "\nCard Reset Hatasi\n" );
							return ( STAT_NOK );
						}
					}
					if(Trans_XLSUpdateChip() != STAT_OK) 
					{
						/*verde Baglanti sonrasi hata oldugu icin reversal gonderilmesi*/
						ReversalPending = TRUE;
						Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);

						/* 06_07 STAT_NOK öncesi Verde'ye EOT gonderiliyor */
						if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE )
						{	 
							Verde_SendControlChar(EOT);
							VERDE_TRX_CONTINUE = FALSE;
						}
						
						return(STAT_NOK);
					}
				}
			}
			
			/* Toygar 13.05.2002 End */
			if (KampanyaFlag == TRUE) Kampanya = TRUE;

			if ( !Slips_FillReceiptStructure( IsEMV, fpPTStruct, Kampanya ) )
			{
				Slips_PrinterStr("\nSlips_FillReceiptStructure hatasi\n");
				return(STAT_NOK);
			}

/* Toygar EMVed */
			if (Files_AddEMVTransaction((byte*)EMVOnline_File, fpPTStruct) <= 0) 
			{
				/*verde Baglanti sonrasi hata oldugu icin reversal gonderilmesi*/
				ReversalPending = TRUE;
				Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);
				return(STAT_NOK);
			}
/* Toygar EMVed */
			if(Files_RecordWrite(FALSE) != STAT_OK) 
			{
				/*verde Baglanti sonrasi hata oldugu icin reversal gonderilmesi*/
				ReversalPending = TRUE;
				Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);
				return(STAT_NOK);
			}

			if(Trans_UpdateCardTypeTable() != STAT_OK)
			{
				/*verde Baglanti sonrasi hata oldugu icin reversal gonderilmesi*/
				ReversalPending = TRUE;
				Trans_SendReversal(NORMAL_REVERSAL, fpPTStruct, IsEMV);
				Slips_PrinterStr("\nPARAMETRE UPDATE    HATASI\n");
				return(STAT_NOK);
			}

			/* update last transaction variables just for successful transactions */
			memcpy(last_transact_batch_no, batch_no_c,3);
			Utils_IToA(seq_no,last_transact_seq_no);
			Utils_LeftPad(last_transact_seq_no, '0', 4);
			
			/* 06_07 xls modullerinden gelen hatalarin handle edilebilmesi icin verde'ye sonuc mesajinin her sey
			   tamamlandiktan sonra ancak slip basimindan once gonderilmesi gerekiyordu */
			/* verde */
			if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE )
			{	/* send RES3 get ACK  */
				if(Verde_SendRes3("0000") != STAT_OK)	Verde_SendControlChar(EOT);
			}

			/*slip basýmý oncesinde hat kapatýlýyor. sadece dial up icin 13.05.2004 */
			Host_Disconnect(  );
			EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
			Slips_PrintReceipt( IsEMV,FALSE );
			Mte_Wait(1000);
			Kbhw_BuzzerPulse(200);
			Mte_Wait(300);
			Kbhw_BuzzerPulse(200);
			
			if(tran_type == PREPROV) SECOND_COPY = FALSE;
			else SECOND_COPY = TRUE;
			if(SECOND_COPY)
			{
				Mainloop_DisplayWait(" 2. KOPYA ˜€˜N  G˜R˜ž'E BASINIZ.",10);
				EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
				Slips_PrintReceipt( IsEMV,FALSE );
				OldKampanyaFlag = KampanyaFlag;
				KampanyaFlag = FALSE; /*@bm 20.09.2004 */
				EmvIF_ClearDsp(DSP_MERCHANT);
			}
			else { OldKampanyaFlag = KampanyaFlag; KampanyaFlag = FALSE; }
			ERROR_RECEIPT = FALSE;
		}
		else
		{	
			char temp_buff[17];
			
				/* display balance*/
				memcpy(temp_buff,d_Transaction_Receive.message,16);
				temp_buff[16] = 0;
				PrntUtil_Cr(3);
				Slips_PrinterStr("          BAK˜YE       \n");
				Slips_PrinterStr("=======================\n");
				Slips_PrinterStr(temp_buff);
				PrntUtil_Cr(5);
		}
		break;
	case PIN_REQUESTED_MESSAGE:
		Mainloop_DisplayWait("Y˜NE PIN ˜STEND˜",2);

		/*InitialiseLocalVariables();*/ /* buraya hata mesajý verilip transaction sonladýrýlmalý */
		/* UPDATE 01_15 24/10/99 ikinci defa PIN istenirse iþlem baþarýsýz olarak sonlandýrýlýr*/
		return(STAT_NOK);
	case REJECTED_MESSAGE:
		if (tran_type != BALANCE)
		{
			if(!Slips_FillErrorReceiptStructure())
			{
				Slips_PrinterStr("\nSlips_FillErrorReceiptStructure hatasi\n");
				return(STAT_NOK);
			}
			/* 06_07 xls modullerinden gelen hatalarin handle edilebilmesi icin verde'ye sonuc mesajinin her sey
			   tamamlandiktan sonra ancak slip basimindan once gonderilmesi gerekiyordu */
			/* verde */
			if( VERDE_TRANSACTION && VERDE_TRX_CONTINUE ) /*bm 21.10.2004 */
			{	/* send RES3 get ACK  */
				if(Verde_SendRes3("9999") != STAT_OK)	Verde_SendControlChar(EOT);
			}
			
			Slips_PrintErrorReceipt(FALSE);
			ERROR_RECEIPT = TRUE;
		}
		break;
	}

	/* update last transaction variables*/
	last_transact_status = d_Transaction_Receive.agreement;

	{
		char TransReceived_DateTime[11];

	/* update real time clock*/ 
	memset(TransReceived_DateTime,'0',10);  /* initialize the date and time */
	memcpy(TransReceived_DateTime,d_Transaction_Receive.date,6);
	memcpy(&TransReceived_DateTime[6] , d_Transaction_Receive.time,4);
	TransReceived_DateTime[10] = 0;
	
	/*Utils_SetDateTime(&TransReceived_DateTime);*/ /*UPDATE 01_15 & kaldýrýldý */
	Utils_SetDateTime(TransReceived_DateTime);
	}
	
	/* Update the transaction variables*/
	/* Increment sequence number*/
	/* Toygar EMVed */
	/*	seq_no++;*/
	/* Toygar EMVed */
		/* UPDATE 01_16 30/10/99 OLUMLU DURUMDA RETURN ETMIYORDU. MISSING RETURN VALUE ONLENDI*/

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_CANCEL
 *
 * DESCRIPTION          : 
 *
 * RETURN                       :
 *
 * NOTES                        : 
 *
 * DATE                         : 24/05/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_Cancel(boolean IsEMV, boolean Kampanya )
{

	char entered_seq_no[5], input_card_no[20];
	char Buffer[LEN_GETKEYBOARD];
	int i;
	byte Len;
	boolean card_number_match, check_card_number;
	TransactionStruct fpTransStruct;

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/** XLS_PAYMENT 02/08/2001 irfan **/
	char zero_amount_local[14];
	char first_6_digit_local[7];

	/* XLS_INTEGRATION2 12/10/2001 irfan */
	char temp_first_6_digit[7];


	/** XLS_INTEGRATION2 12/10/2001 irfan. Cancel islemi icin Eger chipsiz bir islem ise
	    LOYALTY_PERMISSION_FLAG  false olmali. **/
		memcpy(first_6_digit_local,card_no,6);
		first_6_digit_local[6] = 0;

	if( (strstr(Loyalty_Prefixes, first_6_digit_local) != NULL) && 
		(!CORRUPTED_CHIP_TRANSACTION_FLAG) 
	  )
	{
		LOYALTY_PERMISSION_FLAG = TRUE;
	}
	else
	{
		LOYALTY_PERMISSION_FLAG = FALSE;
	}


	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));


/* Set the Preprov Specific Parameters */
	tran_type = CANCEL;
	VOID_FLAG = TRUE;

	/*@bm 02.09.2004 ISSUE_69_ */
	FAIZLI_FLAG = FALSE;

/* Set Debit and Amex flags*/
	DEBIT_FLAG = FALSE;

	if ( !INSTALLATION)
	{
		Mainloop_DisplayWait(" KURULUž YAPIN  ",1);                             /* DEGISECEK_MESAJ*/
		return(STAT_NOK);
	}


/* Check for incomplete EOD transcation*/

	if (EOD_flag)
	{
		Mainloop_DisplayWait(" GšNSONU YAPIN  ",1);                             /* DEGISECEK_MESAJ*/
		return(STAT_NOK);
	}

	
	/* UPDATE 01_18 06/12/99 Parametrelerin okunmasý saðlanýyor */
	/*read parameters files */
	/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya alýndýgýndan kontrol yapýlýyor. hata var ýse
	    idle loop a donmesi saglanýyor */
	if(Files_ReadAllParams() != STAT_OK) return (STAT_NOK);

	if (IsEMV)
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
		memset(card_info1_name, 0, sizeof(card_info1_name));
		memset(card_info1_surname, 0, sizeof(card_info1_surname));

		memset(tempchnm,' ',50);	
		pbIndex = (byte *)tempchnm;
		EmvIF_AppendData(&pbIndex, tagCARDHOLDER_NAME, FALSE, 26);
		memcpy(card_info1_name, tempchnm, sizeof(card_info1_name));

		memset(tempchnm,' ',50);	
		pbIndex = (byte *)tempchnm;
		EmvIF_AppendData(&pbIndex, tagCRDHLDR_NAME_EXT, FALSE, 45);
		memcpy(card_info1_surname, tempchnm, sizeof(card_info1_surname));

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
		input_type = INPUT_FROM_READER_22;
	}

	/*@bm 06.09.2004 ISSUE_70_ card noý emvxls olunca IsEMV de doluyor ve 
	LOYALTY_PERMISSION_FLAG ondan sonra kontrol edilmeli */		
	memcpy( first_6_digit_local, card_no, 6 );
	first_6_digit_local[6] = 0;

	if ( ( strstr( Loyalty_Prefixes, first_6_digit_local ) != NULL ) &&
		 ( !CORRUPTED_CHIP_TRANSACTION_FLAG ) )
	{
		LOYALTY_PERMISSION_FLAG = TRUE;
	}
	else
	{
		LOYALTY_PERMISSION_FLAG = FALSE;
	}


/* Prepare the transaction Message*/

	if(!Mainloop_GetKeyboardInput("SIRA NO?",Buffer,1,4,    /*DEGISECEK_MESAJ*/
				      FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, FALSE))
	{
		EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
		EMVAxess = FALSE;
		return(STAT_NOK);
	}
	else
	{
		memcpy(entered_seq_no,Buffer,Len);
		entered_seq_no[Len]=0;
	}

	Utils_LeftPad(entered_seq_no,'0',4);

	memcpy(fpTransStruct.seq_no, entered_seq_no, 4);

	card_number_match = FALSE;

	if (Files_ReadTransaction(&fpTransStruct) == STAT_OK)
	{
		/** XLS_PAYMENT 03/08/2001 irfan. Restrict cancel of RETURN for loyalty cards**/
		memcpy(first_6_digit_local,card_no,6);
		first_6_digit_local[6] = 0;

		/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
		if ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )	/* axess pos icin yapiliyor */
		{
			if( (fpTransStruct.tran_type == '4')  && (strstr(Loyalty_Prefixes, first_6_digit_local) != NULL ))
			{
				Mainloop_DisplayWait(" IADE'NIN IPTALI    YAPILAMAZ    ",2);
				return(STAT_NOK);
			}
		}
		
		if (fpTransStruct.void_status == '1')
		{
			Mainloop_DisplayWait("˜PTAL ED˜LM˜ž",2);
			return(STAT_NOK);
		}

		card_number_match = TRUE;
		check_card_number = TRUE;

		memcpy(input_card_no, fpTransStruct.card_no, 19);
		input_card_no[19] = 0;
		for (i=12; i<19; i++)
		{
			if (input_card_no[i] == ' ')
				input_card_no[i] = 0;
		}

		i = 0;
		do
		{
			if (card_no[i] != input_card_no[i])
			{
				card_number_match = FALSE;
				check_card_number = FALSE;
			}
			else if ((card_no[i] == 0) &&
				 (input_card_no[i] == 0))
			{
				check_card_number = FALSE;
			}
			i++;
		} while (check_card_number && (i<19));


		/** XLS_PAYMENT 02/08/2001 irfan **/
		OFFLINE_TRX = FALSE;
		memset(zero_amount_local,'0', sizeof(zero_amount_local));
		zero_amount_local[13] = 0;
		if(memcmp(fpTransStruct.amount_surcharge,zero_amount_local,13) == 0)
		{
			/* 06_00 offline transactionlar iptal edildi. hersey online
			/*OFFLINE_TRX = TRUE;
			Host_Disconnect();      
			Mte_Wait(300);*/
			OFFLINE_TRX = FALSE;

		}

		/** XLS_PAYMENT 02/08/2001 irfan **/
		if (card_number_match)
		{
/* 	@bm #6 14.05.2004 KVK  */
/* 
		Printer_WriteStr("Buffer_:\n");
		PrntUtil_BufAsciiHex((byte *)Buffer_, Len);
		Printer_WriteStr("fpTransStruct.kampanya_str:\n");
		PrntUtil_BufAsciiHex((byte *)fpTransStruct.kampanya_str, sizeof(fpTransStruct.kampanya_str));
/**/
			if ( ( fpTransStruct.kampanya_str[10] != ' ' ) &&
				 ( fpTransStruct.kampanya_str[11] != ' ' ) )
			{
				Kampanya = TRUE;
				KampanyaFlag = TRUE;
				memset(Kampanya_Data,'0',15);
				memcpy( Kampanya_Data, fpTransStruct.kampanya_str + 8, 15 );
/* @bm 
			Printer_WriteStr("!encrypted_PIN:\n");
			PrntUtil_BufAsciiHex((byte *)encrypted_PIN, sizeof(encrypted_PIN));
*/
			}
/*    @bm #6 14.05.2004 KVK */
			return ( Trans_Update_Cancel( &fpTransStruct, IsEMV, Kampanya ) );	/* 06_30 yukarida kaldirilan satirlarin yerine ekelendi */

		}
		else
		{
			Mainloop_DisplayWait("KART NO YANLIž",2);
		}
	}
	else
		Mainloop_DisplayWait("KAYIT YOK",2);

	EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
	EMVAxess = FALSE;
	return(STAT_NOK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_Show
 *
 * DESCRIPTION          : 
 *
 * RETURN                       :
 *
 * NOTES                        :
 *
 * ------------------------------------------------------------------------ */
byte Trans_Show(void)
{
	char lbChar;
	char ToDisplay[25];
	char negative_amount[13];
	char formatted_amount[17];
	sint i=0, j, lbStatus, lwLastSeq,handle,p,q,len;
	char temp_index1, temp_index2, temp_string[14];

	boolean           display_transaction;
	TransactionStruct fpTransStruct;
	sint Handle1;
	
	char * Tran_Types[] = { NULL, " SATIS ", "ON PROV", "MANUEL ", " IADE  ", NULL, NULL, NULL};
	char * Evet_Hayir[] = { "H", "E"};



	memset(ToDisplay,0,25);
	memcpy(ToDisplay, "   GENEL NET    ",16);
	ToDisplay[16] = 0;      

	GrphText_Cls (TRUE);
	Debug_GrafPrint2((char *) ToDisplay,1); /*MK DEF:8*/
	memset(ToDisplay, ' ', 16);

	if (net_total[0] == '-')
	{
		if ((net_total[1] == '0') &&
		    (net_total[2] == '0') &&
		    (net_total[3] == '0'))
		{
			memcpy(negative_amount, &net_total[4], 9);
			negative_amount[9] = 0;
			Slips_FormatAmount(negative_amount, formatted_amount);
		}
		else
		{
			memcpy(negative_amount, &net_total[1], 12);
			negative_amount[12] = 0;
			Utils_ClearInsZeros(negative_amount);
			len=strlen(negative_amount);
			memcpy(formatted_amount, negative_amount, len);
			formatted_amount[len] = 0;
		}
		len=strlen(formatted_amount);
		if(len<12)	i=((12-len)/2);
		else		i=0;
		ToDisplay[i++] = '-';

		memcpy(&ToDisplay[i], formatted_amount, len);
		len+=i;
		memcpy(&ToDisplay[len], " TL", 3);
	}
	else
	{
		if ((net_total[0] == '0') &&
		    (net_total[1] == '0') &&
		    (net_total[2] == '0'))
		{
			Slips_FormatAmount(net_total, formatted_amount);
		}
		else
		{
			memcpy(formatted_amount, net_total, 13);
			formatted_amount[13] = 0;
			Utils_ClearInsZeros(formatted_amount);
		}
		len=strlen(formatted_amount);
		if(len<13)	i=(13-len)/2;
		else		i=0;
		memcpy(&ToDisplay[i], formatted_amount, len);
		len+=i;
		memcpy(&ToDisplay[len], " TL", 3);
	}

	ToDisplay[16] = 0;

	Debug_GrafPrint2((char *)ToDisplay, 2);
	lbChar = Kb_WaitForKey();

	/* Display Transactions */
	display_transaction = FALSE;
	if (RamDisk_ChkDsk((byte *)Transaction_File)==RD_CHECK_OK)
	{
		Handle1 = RamDisk_Reopen((byte *)Transaction_File)->Handle;


		lwLastSeq = RamDisk_Seek(Handle1,SEEK_LAST);
		display_transaction = TRUE;
		i = 0;
	}
	else
	{	EmvIF_ClearDsp(DSP_MERCHANT);
//		Mainloop_DisplayWait("OKUMA HATASI",2);
		return(STAT_NOK);
	}
	RamDisk_Seek(Handle1, SEEK_FIRST);
	do
	{
		switch (lbChar)
		{
		case MMI_CIKIS:
			display_transaction = FALSE;
			break;

		/*case MMI_ENTER:*/
		default:
			if ((i >= lwLastSeq) || (lbStatus == RD_LAST))
				display_transaction = FALSE;
			else
			{
				if (RamDisk_Seek(Handle1, i) >= 0)
				{
					lbStatus = RamDisk_Read(Handle1, &fpTransStruct);
					if ((lbStatus >= 0) || (lbStatus == RD_LAST))
						display_transaction = TRUE;
					else
						display_transaction = FALSE;
				}
				else display_transaction = FALSE;                       
			}
		}

		if (display_transaction)
		{    
			memcpy(temp_string, fpTransStruct.seq_no, 4);
			temp_string[4] = 0;
			temp_index1 = fpTransStruct.tran_type - '0';
			temp_index2 = fpTransStruct.void_status - '0';

			sprintf(ToDisplay, "%s %s I:%s", temp_string, Tran_Types[temp_index1], Evet_Hayir[temp_index2]);
			ToDisplay[16] = 0;  
		
		Debug_GrafPrint2((char *)ToDisplay, 3);
			memset(ToDisplay, 0, 25);
			memset(ToDisplay, ' ', 16);
			memset(temp_string,'0',13);

		for (p=0,q=0;p<13;p++)
		{
			if(isdigit(fpTransStruct.amount_surcharge[p]))
			{
				temp_string[q]=fpTransStruct.amount_surcharge[p];
				q++;
				/*memcpy(temp_string, fpTransStruct.amount_surcharge, 13);*/
			}
		}
			temp_string[13] = 0;

			if ((temp_string[0] == '0') &&
			    (temp_string[1] == '0') &&
			    (temp_string[2] == '0'))
			{
				Slips_FormatAmount(temp_string, formatted_amount);
			}
			else
			{
				memcpy(formatted_amount, temp_string, 13);
				formatted_amount[13] = 0;
				Utils_ClearInsZeros(formatted_amount);
			}
			len=strlen(formatted_amount);
			if(len<13)	j=(13-len)/2;
			else		j=0;
			memcpy(&ToDisplay[j], formatted_amount, len);
			len+=j;
			memcpy(&ToDisplay[len], " TL",3);
			ToDisplay[16] = 0;

		Debug_GrafPrint2((char *)ToDisplay, 4);

			i++;

			lbChar = Kb_WaitForKey();
		}

	} while (display_transaction);


	return(STAT_OK);

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_Installation.
 *
 * DESCRIPTION          : Installation
 *
 * RETURN                       : TRUE or FALSE, STAT_OK, STAT_NOK
 *
 * NOTES                        : r.i.o & t.c.a 04/06/99
 *                                      : EOD_pointer = shows the step of the EOD process
 *                                        EOD_OK          = shows that EOD is finished successfully.
 *                                        1: Report is printed successfully
 *                                        2: Agreement is done successfully
 *                                        3: NewBatchNo is done successfully
 *                                        0: Parameter Loding is ok.
 *
 * ------------------------------------------------------------------------ */
byte Trans_Installation(void)
{

	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	byte installation_ret;
	char merch_device_no_temp[9];
	
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* 1_18 28/07/2000 keyboard dan giriþ en fazla 20 digit olup, ATS prefix giriþi uyarlamasi*/
	char temp[20];
	sint i,j,k;
	char ATS_Prefixes_temp[7];
	int TransOnlineNo, TransOfflineNo;


	Files_GetTransactionCounts(&TransOnlineNo, &TransOfflineNo);
	if (TransOfflineNo > 0)
	{
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("OFFLINE ˜žLEM !!GšNSONU YAPINIZ.",3);
		return(STAT_NOK);
	}

	/*bm YTL 13.08.2004 */
	memset(Kasiyer_No, '0' , sizeof(Kasiyer_No));

	/* XLS 04_02 20/12/2001 irfan. xls initialization ve ACK bekleyen mail ve Aktif xls batch dosya kontrolu*/
	/** Create XLS Files*/
	GrphText_Cls (TRUE);
	XLS_Start();

	if(XLS_Check_Initialization_State() != XLS_NOT_INITIALIZED)
	{/* xls is initialized */
	
		if (!TCPIP_TpeVide() || !XLS_TpeVide())
		{ /* RAM DISK te ACK bekleyen mail var ya da current batch file var */
			Slips_PrinterStr ("\nPOS BOS DEGIL.\n");
			Slips_PrinterStr ("XLS GUNSONUNU TAMAMLAYINIZ\n");
			Mainloop_DisplayWait(" XLS GUNSONUNU   TAMAMLAYINIZ   ",1);
			return(STAT_NOK);
		}
	}

//#if 0
	for(; ;)
	{
		/* get POS type. BizzPos : 0 Akbank: 1 */
		/*--------------------------------------------------*/
		if(!Mainloop_GetKeyboardInput((char *)"   BIZZ POS ?   EVET :0 HAYIR :1",Buffer,1,1,
						   FALSE, FALSE, FALSE, FALSE, TRUE,&Len,value_null, FALSE))
		{
			Mainloop_DisplayWait("    BIZZ POS      G˜R˜ž HATASI  ",2);
			return(STAT_NOK);
		}

		/* move the entered POS type to the temp buff. */
		if(Buffer[0] == '0')
		{
			genius_flag = '0';
			break;
		}
		if(Buffer[0] == '1')
		{
			genius_flag = '1';
			break;
		}
		else 
		{
			GrphText_Cls (TRUE);
			Mainloop_DisplayWait("   0 YA DA 1       G˜R˜LEB˜L˜R  ",1);
		}
	}
//#endif genius_flag = '1';
	
	/** XLS 04_02 19/12/2001 irfan. POS tipi seciliyor **/
	for(; ;)
	{
		/* get POS type. Axess : 0 Akbank: 1 Default Akbank */
		/*--------------------------------------------------*/
		if(!Mainloop_GetKeyboardInput((char *)"   POS T˜P˜ ?   AXESS:0 AKBANK:1",Buffer,1,1,
						   FALSE, FALSE, FALSE, FALSE, TRUE,&Len,value_null, FALSE))
		{
			Mainloop_DisplayWait("    POS T˜P˜      G˜R˜ž HATASI  ",2);
			return(STAT_NOK);
		}

		/* move the entered POS type to the temp buff. */
		if((Buffer[0] == '0') || (Buffer[0] == '1'))
		{
			if ( Buffer[0] == '0' )	/* bm OPT 0-XLS, 1-AKB */
			{
				Buffer[0] = '4';
				POS_Type = Buffer[0];
				 ICC_Disactive(EX_CARD);
				 break;
			}
			else
			{
				Buffer[0] = '3';
				POS_Type = Buffer[0];
				ICC_Status(EX_CARD);				
				break;
			}
		}
		else 
		{
			GrphText_Cls (TRUE);
			Mainloop_DisplayWait("   0 YA DA 1       G˜R˜LEB˜L˜R  ",1);
		}
	}

	/* initialize the ramdisk */
	if(RamDisk_Reset() != RD_OK)
	{
		Slips_PrinterStr ("\nRAMDISK RESET HATASI\n\n");
		Mainloop_DisplayWait("    RAMDISK       RESET HATASI  ",1);
		return(STAT_NOK);
	}

	ReversalPending = FALSE;
	INSTALLATION_ONGOING = TRUE;

/* Toygar EMVed */
	EmvIF_InitRNG();
/*
	if (Main_InitExceptionList() != STAT_OK)
	{
		Mainloop_DisplayWait("EXCEPTION FILE  YARATILAMADI",2);
		return(STAT_NOK);
	}
	if (Main_InitRevocationList() != STAT_OK)
	{
		Mainloop_DisplayWait("REVOCATION FILE YARATILAMADI",2);
		return(STAT_NOK);
	}
*/
/* Toygar - New EMV Start */
/*
 	Main_LoadCAPK();
*/
/* Toygar - New EMV End */
	CallFlag = TRUE;
	if (Files_InitEMVTransaction(FALSE) != STAT_OK)
	{
		Mainloop_DisplayWait("FILE YARATILAMADI",2);
		return(STAT_NOK);
	}
/* Toygar EMVed */

/* bm OPT 	if ( POS_Type == '0' )		/* axess pos */
	if ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
	{							/* axess pos */
		/** Create XLS Files*/
		GrphText_Cls (TRUE);
		XLS_Start();
	}

		
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));

	/*01_18 30/06/2000 Kuruluþ oncesinde parametreler initialize ediliyor ve Ramdisk dosyalarý
	  yeniden yaratýlýyor*/
	Trans_Init_Permanent_Params();
	if(Files_InitTransaction() != STAT_OK)
	{
		Mainloop_DisplayWait("FILE YARATILAMADI",2);
		return(STAT_NOK);
	}
/*MK DEF:21*/
	if(Files_GInitFileGenius() !=STAT_OK)
	{
		Slips_PrinterStr("\n\nBIZ.CARD FILE YARATILAMADI\n\n\n\n");
		Mainloop_DisplayWait("BIZ.CARD FILE YARATILAMADI",1);
                return(STAT_NOK);
	}

	/* 06_30 Radisk create edildiginden akb_offline dosyalarinin yeniden yaratilmesi gerekir */
		if( Offline_InitOfflineParameterFile() != STAT_OK )
	{
		Slips_PrinterStr("\n\nOFFLINE PARAM FILE YARATILAMADI\n\n\n\n");
		Mainloop_DisplayWait("OFFLINE PARAM FILE YARATILAMADI",1);
                return(STAT_NOK);
	}
		if( Offline_InitOfflineAkbBatchFile()  != STAT_OK ) 
        {
		Slips_PrinterStr("\n\nOFFLINE AKB BATCH YARATILAMADI\n\n\n\n");
		Mainloop_DisplayWait("OFFLINE AKB BATCH YARATILAMADI",1);
                return(STAT_NOK);
        }
	/* init flags related with server open timeout  */      
	FIRST_PHONE_FLAG			= FALSE;
	SERVER_OPEN_TIMEOUT_FLAG	= FALSE;
	EOD_pointer					= '0'; /* 01_18 10/02/2000 Kuruluþ iþleminde yarým kalan bir kuruluþ bile olsa
									   baþtan baþlamasýný saðlar */

	FIRST_RUN = FALSE;      /*ilk islemin kurulus olmasý garantilenir */
	
#ifndef GRAFDISP
	Display_ClrDsp();
	Display_UpDisplay("   KURULUS...   ");
#else
	GrphText_Cls (TRUE);
	Debug_GrafPrint2("   KURULUS...   ", 2);
#endif


	/* get the device no */
	/*-------------------*/
	/* UPDATE 01_18 06/12/99 Cihaz giriþinde alphanumeric giriþe müsade edildi 5. flag TRUE yapýldý*/
	if(!Mainloop_GetKeyboardInput((char *)"   CIHAZ NO ?   ",Buffer,8,8,
				       FALSE, FALSE, FALSE, FALSE, TRUE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("C˜HAZ NO G˜R˜ž HATASI",2);
		return(STAT_NOK);
	}

	/* move the entered merch_device_no to the temp buff. */
	memcpy(merch_device_no_temp,Buffer,Len);
	merch_device_no_temp[Len]=0;

	/*              LAN/DialUp Tipi                 */
	/*----------------------------------------------*
	do
	{       
		if(!Mainloop_GetKeyboardInput((char *)"DIAL/LAN>0:D 1:L",Buffer,0,1,
					       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
		{
			Mainloop_DisplayWait("   LAN/DIAL       G˜R˜ž HATASI  ",2);
			return(STAT_NOK);
		}
		
		if(Len != 0)
		{
			if (Buffer[0] == '1') BUYUKFIRMA = TRUE;
			else BUYUKFIRMA = FALSE;
		}
		else
		{
			BUYUKFIRMA = FALSE;
		}
	}
	while((Buffer[0] != '0') && (Buffer[0] != '1'));
	
	if ( !BUYUKFIRMA ) Trans_EnterPhoneNum();
	else Trans_SetBaudRate();
*/
	Trans_EnterPhoneNum();
/* Envanter Bilgileri 25.07.2002 */
	if (POS_Type == '4') GActive_Application_Code = '2';
	else GActive_Application_Code = '1';

	if (BUYUKFIRMA) GConnection_Type = '2';
	else GConnection_Type = '1';

/*
memcpy(GPOS_Serial_No,"123456789",9);
memcpy(GPOS_Akbank_No,"1234567",7);
memcpy(GPinpad_Serial_No,"123456789",9);
memcpy(GPinpad_Akbank_No,"1234567",7);
*/

/*#ifdef 1 irfan 02/09/2002 */
	/* get pos serial no */
	/*-------------------*/
	if(!Mainloop_GetKeyboardInput((char *)" POS SERI NO  ? ",Buffer,9,9,
				       FALSE, FALSE, FALSE, FALSE, TRUE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("POS SERI NO     G˜R˜ž HATASI",2);
		return(STAT_NOK);
	}
	memcpy(GPOS_Serial_No,Buffer,Len);
/*	GPOS_Serial_No[Len]=0;*/

	/* get pos akbank no */
	/*-------------------*/
	if(!Mainloop_GetKeyboardInput((char *)" POS AKBANK NO? ",Buffer,7,7,
				       FALSE, FALSE, FALSE, FALSE, TRUE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("AKBANK SERI NO  G˜R˜ž HATASI",2);
		return(STAT_NOK);
	}
	memcpy(GPOS_Akbank_No,Buffer,Len);
	memcpy(GPOS_Akbank_No_My,Buffer,7);
	memcpy(GPOS_Akbank_No_My2,Buffer,7);
/*	GPOS_Akbank_No[Len]=0;*/

	/* get pinpad serial no */
	/*-------------------*/
	/*MK DEF:8  DEFAULT SERIAL NO*/
/*	if(!Mainloop_GetKeyboardInput((char *)"PINPAD SERI NO ?",Buffer,9,9,
				       FALSE, FALSE, FALSE, FALSE, TRUE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("PINPAD SERI NO  G˜R˜ž HATASI",2);
		return(STAT_NOK);
	}*/
	memcpy(GPinpad_Serial_No,"000000000",9);
/*	GPinpad_Serial_No[9]=0;*/

	/* get pinpad akbank no */
	/*-------------------*/
	/*MK DEF:8  DEFAULT SERIAL NO*/
/*	if(!Mainloop_GetKeyboardInput((char *)"PINPAD AKBNK NO?",Buffer,7,7,
				       FALSE, FALSE, FALSE, FALSE, TRUE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("PINPAD AKBANK NOG˜R˜ž HATASI",2);
		return(STAT_NOK);
	}*/
	memcpy(GPinpad_Akbank_No,"0000000",7);
/*	GPinpad_Akbank_No[7]=0;*/
/*#endif irfan 02/09/2002 */
/* Envanter Bilgileri 25.07.2002 */


	/* 05_02 NOT_CERTIFIED CARDS irfan 16/09/2002 */
	memset(Not_Certified_Prefixes,0 ,sizeof(Not_Certified_Prefixes));
	strcpy(Not_Certified_Prefixes, "554960,");

	
	/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
	if ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )		/* axess icin yapiliyor */
	{/* axess POS */
		/** XLS_INTEGRATION2 04/10/2001 irfan. Default xls prefix yukleniyor **/
		strcpy(Loyalty_Prefixes, "557113,557829,552608,552609,435508,435509,521807.");
	}
	else
	{/* akbank POS */
		memset(Loyalty_Prefixes,0 ,sizeof(Loyalty_Prefixes));
	}

	/*// GET THE ATS PREFIXES
	//-----------------------*/

	memset(ATS_Prefixes, 0, sizeof(ATS_Prefixes));
	memset(ATS_Prefixes_temp, 0, sizeof(ATS_Prefixes_temp));
	i = 1;
	k = 0;

	/* GRAFPOS 09/02/2001 irfan ATS prefix'leri tek tusla kabul edilir hale getirildi */

	if(!Mainloop_GetKeyboardInput((char *)"AMEX PRE.EKLEME?EVET=1   HAYIR=0",Buffer,1,1,
					   FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
	{
		Mainloop_DisplayWait("  G˜R˜ž HATASI  ",2);
		return(STAT_NOK);
	}

	if(Buffer[0] == '1')
	{
		strcpy(ATS_Prefixes, "375550,375551,375552,375553,375554,375555,375556,375557,375558,");
		k = 9;
	}

	for(i=1;i<(35-k);i++ )
	{
		memset(ATS_Prefixes_temp, 0, sizeof(ATS_Prefixes_temp));
		sprintf(temp,"TAKSIT PRFX(%d)?" , i);
		temp[strlen(temp)] = 0;

		if(!Mainloop_GetKeyboardInput(temp,Buffer,0,6,
			       FALSE, FALSE, FALSE, FALSE, TRUE,&Len,value_null, TRUE))
		{
			Mainloop_DisplayWait(" ATS PREF˜XLER˜   G˜R˜ž HATASI  ",2);
			return(STAT_NOK);
		}

		if (Len == 0) 
				break;

		memset(ATS_Prefixes_temp, 0 , sizeof(ATS_Prefixes_temp));
		memcpy(ATS_Prefixes_temp, Buffer, Len);
		ATS_Prefixes_temp[Len] = 0;

		strcat(ATS_Prefixes, ATS_Prefixes_temp);
		j = strlen(ATS_Prefixes);
		ATS_Prefixes[j] = ',';
		ATS_Prefixes[j+1] = 0;

	}

	if(!Mainloop_GetKeyboardInput((char *)"GPRS>0:ON 1:OFF",Buffer,0,1,
					   FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("   GPRS           G˜R˜ž HATASI  ",2);
		return(STAT_NOK);
	}
	if(Len != 0)
	{
		/* initialize the Line_Type */
		if ( Buffer[0] == '1') GPRS_USEGPRS=FALSE;
		else {
			GPRS_USEGPRS=TRUE;
			GPRS_Disconnect(APN);
			Mainloop_SetGPRSParams(FALSE);
		}
	}
	else
	{
		GPRS_USEGPRS=FALSE;
	}


	/*              Onay Ýsteme              */
	/*---------------------------------------*/
	memset(Buffer,0,LEN_GETKEYBOARD);
	Buffer[0]='0';
	if(!Mainloop_GetKeyboardInput((char *)"ONAY > 0:E   1:H",Buffer,1,1,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
	{
		Mainloop_DisplayWait("     ONAY         G˜R˜ž HATASI  ",2);
		return(STAT_NOK);
	}

	if(Buffer[0] != '0')
	{
		/* deðiþtirilmek istenen parametreleri saklamak icin. ilk calýþmada yazamadý hatasý verebilir*/
		/* cunku TRANSP  ve GENPAR yaratýlmamýs olacak. Ancak parametere deðiþikliði yapamanýn baska yolu yok*/
		/*save parameters files */
		/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
		   eger yazamas ise idle loop'a donuyor */
		if(Files_WriteAllParams() != STAT_OK) return(STAT_OK);

		return(STAT_NOK);
	}
#ifndef GRAFDISP
	Display_DownDisplay("                "); /* Ýkinci satýrý temizliyor */
#else
	Debug_GrafPrint2("                ", 3);
#endif


	/* eger onay alýndý ise aþaðýdaki kýsým çalýþtýrýlýr.*/
	INSTALLATION=FALSE;     /* kurulus yapýlmadý anlamýna geliyor */
	EOD_flag=TRUE;          /* gunsonu yapýlmadý anlamýna geliyor */

	/* update  merch_device_no with the entered one */
	memcpy(merch_device_no,merch_device_no_temp, sizeof(merch_device_no_temp));
	merch_device_no[sizeof(merch_device_no)-1]=0;

	/* initialize merch amex no */
	memset(merch_AMEX_no,'0',10);
	merch_AMEX_no[10] = 0;

	/* initialize merchant no */
	memset(merch_no,'0',12);
	merch_no[12] = 0;

	/* initialize merch address */
	memset(merch_addr,' ',72);
	merch_addr[72] = 0;
	
	memset(param_ver_no,'0',3);
	param_ver_no[3] = 0;

	memset(batch_no_c,'0',3);
	batch_no_c[3] = 0;

	batch_no = 0;

	device_serial_no = '0';
	POS_software_ver = 'B';

	/* erases and recreates the parameter files */
	if(Files_InitParameters() != STAT_OK)
	{
		Mainloop_DisplayWait("PARAMS ve TRANSP  YARATILAMADI  ",2);
		return(STAT_NOK);
	}

	/* verde kuruluþ icin reversal gonderilmesi engellendi */
	 ReversalPending = FALSE;

	/*save parameters files */
	/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
	eger yazamas ise idle loop'a donuyor */
	if(Files_WriteAllParams() != STAT_OK) return(STAT_NOK);

	/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
	/*Host_PreDial();*/
	if(Host_PreDial() != STAT_OK) 
	{
		Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
		return(STAT_NOK);
	}

	installation_ret = Trans_EODTransaction();

	/*save parameters files */
	/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya cevridi. Dolayýsý ile kontrol ediliyor
	   eger yazamas ise idle loop'a donuyor */
	if(Files_WriteAllParams() != STAT_OK) return(STAT_NOK);

	if (installation_ret == STAT_OK)
	{
		EOD_flag=FALSE;                                 /* gun sonu tamamlandý anlamýna geliyor */
		Trans_Initialize_Variables();
		INSTALLATION = TRUE;                    /* Kurulus tamamlandý anlamýna geliyor */
		INSTALLATION_ONGOING=FALSE;
	}
	else
	{
#ifndef GRAFDISP
		Display_DownDisplay("  BAžARISIZ!!   ");
#else
		Debug_GrafPrint2("  BAžARISIZ!!   ", 3);
#endif
		return(installation_ret);
	}

	return(installation_ret);       /* UPDATE 01_15 24/10/99  Baþarýlý olmasý durumunda dönüþ kodunu 
								return etmek gerekiyor */

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_EODTransaction.
 *
 * DESCRIPTION          : Main fuction for EOD
 *
 * RETURN                       : TRUE or FALSE, STAT_OK, STAT_NOK
 *
 * NOTES                        : r.i.o & t.c.a 30/05/99
 *                                      : EOD_pointer = shows the step of the EOD process
 *                                        EOD_OK          = shows that EOD is finished successfully.
 *                                        1: Report is printed successfully
 *                                        2: Agreement is done successfully
 *                                        3: NewBatchNo is done successfully
 *                                        0: Parameter Loding is ok.
 *
 * ------------------------------------------------------------------------ */
byte Trans_EODTransaction(void)
{
	char EOD_msg[35];
	byte agreement_ret;
	byte newbatchno_ret;
	byte EOD_Status, Status;
	char device_date[8], device_time[6];


	OfflineParametersStruct lbOfflineParametersStruct; /*bm OFF 21.10.2004 */

	 /*read parameters files */
	/* 01_18 irfan 02/07/2000 void yapýdan byte yapýya alýndýgýndan kontrol yapýlýyor. hata var ýse
	    idle loop a donmesi saglanýyor */
	if(Files_ReadAllParams() != STAT_OK) return (STAT_NOK);

	/* Host Connect with FALSE parameter to not Predial*/
	Status = Host_Connect(TRUE);    /* sonra false ile cagrýlacak */
	if(Status != STAT_OK)
	{
		Host_Disconnect();
		return(Status);
	}

	if(Trans_SendReversal(NORMAL_REVERSAL, 0, FALSE) != STAT_OK) return (STAT_NOK);
	
	KeyDownloadStarted = FALSE;
	AppListDownloadStarted = FALSE;
	ExceptionDownloadStarted = FALSE;
	RevocationDownloadStarted = FALSE;

	/* init flags related with server open timeout  */      
	FIRST_PHONE_FLAG	= FALSE;
	SERVER_OPEN_TIMEOUT_FLAG= FALSE;
	EOD_flag = TRUE;        /* means that EOD is not completed */

	/*bm YTL 13.08.2004 */
	memset(Kasiyer_No, '0' , sizeof(Kasiyer_No));


	if (EOD_pointer == '0')
	{
		EOD_OK=FALSE;
		if (Slips_Report(TRUE) != STAT_OK) return(STAT_NOK);
		GrphText_Cls (TRUE);
		Debug_GrafPrint2(" GšNS.YAPILIYOR ", 2);
		Debug_GrafPrint2("RAPOR BASILIYOR.", 3);

		EOD_pointer = '1';
	}

	if (Trans_BatchUpload(TRUE) != STAT_OK) return(STAT_NOK);

	if (EOD_pointer == '1')
	{
		Debug_GrafPrint2("   MUTABAKAT    ", 3);

		EOD_OK=FALSE;
		agreement_ret = Trans_Agreement();

		if (agreement_ret == STAT_CALL_AGAIN)
		{
			if (Trans_BatchUpload(FALSE) != STAT_OK) return(STAT_NOK);
			else Files_InitEMVTransaction(TRUE);
		}
		else if (agreement_ret != STAT_OK) return(STAT_NOK); 
		else Files_InitEMVTransaction(TRUE);
		EOD_pointer = '2';

	}
	
	if (EOD_pointer == '2')
	{
#ifndef GRAFDISP
		Display_DownDisplay(" YEN˜ YI¦IN NO  ");
#else
		Debug_GrafPrint2(" YEN˜ YI¦IN NO  ", 3);
#endif

		if(!IS_HOST_CONNECTED_FLAG)
		{
			/* Host Connect with FALSE parameter to not Predial*/
			agreement_ret = Host_Connect(TRUE);     /* sonra false ile cagrýlacak */
			if(agreement_ret != STAT_OK)
			{
				Host_Disconnect();
				return(agreement_ret);
			}
		}

		EOD_OK=FALSE;
		newbatchno_ret = Trans_NewBatchNo();
		if (( newbatchno_ret == STAT_OK ) &&(EOD_pointer == '2'))
		{
			EOD_pointer = '3';      /* go on with the parameter donwloading */
		}
		else 
		{
			Host_Disconnect();
		}
	}


	if (EOD_pointer == '3')
	{
		if(!IS_HOST_CONNECTED_FLAG)
		{

			/* Host Connect with FALSE parameter to not Predial*/
			agreement_ret = Host_Connect(TRUE);     /* sonra false ile cagrýlacak */
			if(agreement_ret != STAT_OK)
			{
				Host_Disconnect();
				EOD_pointer = '2';
				return(agreement_ret);
			}
		}

		Debug_GrafPrint2(" PARAMETRE YšK. ", 3);

		EOD_OK=FALSE;
				
		EOD_Status = Trans_ParameterDownload();

		if ( EOD_Status == STAT_OK)     
		{
			EOD_pointer =      '0';      /* EOD transaction is OK */
			EOD_flag = FALSE;
			INSTALLATION = TRUE;
		}
		if (!EOD_OK)
		{
			EOD_pointer = '2';
			strcpy(EOD_error_message, "PAR. YšKLENEMED˜");
		}
	}

	/* offline pos icin acceptable bin yukleme. bm OFF 21.10.2004 */
	/*=====================================================*/
	lbOfflineParametersStruct.offline_flag = FALSE;
	Offline_ReadOfflineParameters( &lbOfflineParametersStruct );

	if ( ( EOD_pointer == '0' ) &&	 lbOfflineParametersStruct.offline_flag )
	{
		EOD_pointer = '4';
	}

	if ( EOD_pointer == '4' )
	{
		if ( !IS_HOST_CONNECTED_FLAG )
		{
			/* Host Connect with FALSE parameter to not Predial */
			agreement_ret = Host_Connect( TRUE );	/* sonra false ile cagrýlacak */
			if ( agreement_ret != STAT_OK )
			{
				Host_Disconnect(  );
				return ( agreement_ret );
			}
		}

		Debug_GrafPrint2( " OFFL. BIN YšK. ", 3 );
		EOD_flag = TRUE;
		EOD_OK = FALSE;

		EOD_Status = Trans_Bin_Download(  );

		if ( EOD_Status == STAT_OK )
		{
			EOD_pointer = '0';	/* EOD transaction is OK */
			EOD_flag = FALSE;
			INSTALLATION = TRUE;
		}
		if ( !EOD_OK )
		{
			strcpy( EOD_error_message, "BIN. YšKLENEMED˜" );
		}
	}



	if (!EOD_OK)            /* in case of error coming from the previous steps */
	{
		if (EOD_error_message[0] == ' ')
		{
			strcpy(EOD_error_message, "GšNSNU YAPILAMADI");
			EOD_error_message[17] = 0;
		}

		Utils_GetDateTime(device_date,device_time);
		device_time[5] = 0;
	
		sprintf(EOD_msg, "%s %s", EOD_error_message,device_time);
		EOD_msg[24]=0;

		/* XLS_INTEGRATION 15/08/2001 Printer_WriteStr Slips_PrinterStr yapildi. irfan */
		Slips_PrinterStr(EOD_msg);

		PrntUtil_Cr(6);
		return(STAT_NOK);
	}
	else
	{
		/*bm YTL 13/07/2004 POS tipi kuruluþ sonunda deðþecek */
		if ( Set_Mode == 0 )
		{
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
		else
		{
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

		PrntUtil_Cr(1);

		Utils_GetDateTime(device_date,device_time);
		device_time[5] = 0;

		sprintf(EOD_msg,"GšN SONU B˜TT˜     %s",device_time);
		EOD_msg[24]=0;

	}

	PrntUtil_Cr(1);
	/* XLS_INTEGRATION 15/08/2001 Printer_WriteStr Slips_PrinterStr yapildi. irfan */
	Slips_PrinterStr(EOD_msg);
	PrntUtil_Cr(6);

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_Agreement.
 *
 * DESCRIPTION          : Main fuction for EOD
 *
 * RETURN                       : STAT_OK
 *                                        STAT_NOK
 *                                        TRR_NO_CONNECTION
 *
 * NOTES                        : r.i.o & t.c.a 01/06/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_Agreement(void)
{
	char EODReceived_DateTime[11];
	char agreement_temp[33];
	byte Trans_Agree_Ret;
	byte Status;
/* 06_01 RECONCILATION irfan 20/09/2002 */
	char lbSend_Buffer[900];

/* Toygar EMVed */
/*	memset(lbSend_Buffer,0,256);                    *//* initialize the lbSend_Buffer . Provides Null at the end of mess. as well */
	memset( lbSend_Buffer, 0, 900 );	/* initialize the lbSend_Buffer . Provides Null at the end of mess. as well */
/* Toygar EMVed */
	memset(EOD_error_message, ' ', 25);

	/* Prepare the agreement Message*/
	Trans_FillEODSendStructure();

/* Prepare the Send Message*/
	memcpy (lbSend_Buffer,&d_EOD_Send,sizeof(d_EOD_Send));

/* Send the Prepared message to the Host */

/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Trans_Agree_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
	else Trans_Agree_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);
/* Toygar 13.05.2002 End */

	if(Trans_Agree_Ret != STAT_OK)
	{               
		return(Trans_Agree_Ret);
	}
    

	/* Analyze the response */

	/* Move Received_Buffer to the d_EOD_Receive */
	memcpy(&d_EOD_Receive,Received_Buffer+1,sizeof(d_EOD_Receive));

	if ( d_EOD_Receive.Header_EOD.MSG_TYPE != '7')
	{
		memset(agreement_temp,0,sizeof(agreement_temp));
		/*UPDATE 01_15 24/10/99 & kaldýrýldý. */
		/*sprintf(agreement_temp,"BEKLENMEYEN MES.   KODU: %c",&d_EOD_Receive.Header_EOD.MSG_TYPE);*/
		sprintf(agreement_temp,"BEKLENMEYEN MES.   KODU: %c",d_EOD_Receive.Header_EOD.MSG_TYPE);
		Mainloop_DisplayWait(agreement_temp,1);
		return(STAT_NOK);
	}


	/* UPDATE 01_16 24/10/99. Dönen mesaj da ortak field'lar kontrol ediliyor */
	/* bozuk gelen ama LRC den gecen mesaj larda bozuk mesajý sense edebilmek icin yapýlýyor*/
	if(Trans_CheckEODAgreementReturnMessage() != STAT_OK)
	{
		return(STAT_NOK);
	}

	/* update real time clock*/ 
	memset(EODReceived_DateTime,'0',10);    /* initialize the date and time */
	memcpy(EODReceived_DateTime, d_EOD_Receive.date,6);
	memcpy(&EODReceived_DateTime[6] , d_EOD_Receive.time,4);
	EODReceived_DateTime[10] = 0;

	/*Utils_SetDateTime(&EODReceived_DateTime);*/ /*UPDATE 01_15 & kaldýrýldý */
	Utils_SetDateTime(EODReceived_DateTime);

	/*copy received message and confirmation code*/
	memcpy(agreement_temp, d_EOD_Receive.message, 16);
	agreement_temp[16]=' ';
	memcpy(agreement_temp+17, d_EOD_Receive.confirmation_code, 3);
	agreement_temp[20]=0;

/* Toygar EMVed */
	if (d_EOD_Receive.agreement == '0') return(STAT_CALL_AGAIN);
/* Toygar EMVed */
	/* confirmation code check*/
	if ( memcmp(d_EOD_Receive.confirmation_code, "200", 3)==0)
	{
		Debug_GrafPrint2((char *)agreement_temp, 3);

		Slips_PrintAgreementReport();
		EOD_OK = TRUE;
		return(STAT_OK);
	}
	else if ( memcmp(d_EOD_Receive.confirmation_code, "207", 3)==0)
	{
		/* negative balance, so stop EOD transaction*/
		Mainloop_DisplayWait(agreement_temp,2); 
		strcpy(EOD_error_message, agreement_temp);
		
		/* cancel EOD operation*/
		EOD_OK = FALSE;
		EOD_pointer='0';
		EOD_flag = FALSE;       /* eksi bakiyeden kurtulma imkaný vermek icin FALSE yapýldý */
		return(STAT_NOK);
	}
	else
	{       /* hata durumu */
		EOD_OK=FALSE;
		PrntUtil_Cr(2); 
		Slips_PrinterStr(agreement_temp);
		PrntUtil_Cr(7);
		Display_ClrDsp();
		Mainloop_DisplayWait(agreement_temp,2);
		return(STAT_NOK);
	}

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_FillEODSendStructure.
 *
 * DESCRIPTION          : Fill the EODSend Structure
 *
 * RETURN                       : none
 *
 * NOTES                        : r.i.o & t.c.a 31/05/99
 *
 * ------------------------------------------------------------------------ */
void Trans_FillEODSendStructure(void)
{
	char count_temp[5];
	char device_date[8], device_time[6];

	/* initialize the send structure */
	/*memcpy(&d_EOD_Send,'0',sizeof(d_EOD_Send));*/ /* UPDATE 01_15 24/10/99 yanlýþ memcpy kullanýmý */
	memset(&d_EOD_Send,'0',sizeof(d_EOD_Send));

/* bm YTL 21/06/2004 */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		d_EOD_Send.Header_EOD.Cur_Index = 'X';
	else
		d_EOD_Send.Header_EOD.Cur_Index = 'Y';
	d_EOD_Send.Header_EOD.ActAppCode = POS_Type;
	d_EOD_Send.Header_EOD.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
	memcpy( d_EOD_Send.Header_EOD.PosType, "LIP", 3 );
	memcpy( d_EOD_Send.Header_EOD.PosModel, "8000", 4 );
	memcpy( d_EOD_Send.Header_EOD.PosVersion, POS_VERSION_NUMBER, 4 );
	memset( d_EOD_Send.Header_EOD.Cashier_Id, '0', 16 );
	memset( d_EOD_Send.Header_EOD.IMEI_ID, '0', 15 );
	memset( d_EOD_Send.Header_EOD.Filler, ' ', 19 );

	Utils_GetDateTime( device_date, device_time );
	d_EOD_Send.Header_EOD.PosDate[0] = device_date[6];
	d_EOD_Send.Header_EOD.PosDate[1] = device_date[7];
	d_EOD_Send.Header_EOD.PosDate[2] = device_date[3];
	d_EOD_Send.Header_EOD.PosDate[3] = device_date[4];
	d_EOD_Send.Header_EOD.PosDate[4] = device_date[0];
	d_EOD_Send.Header_EOD.PosDate[5] = device_date[1];
	d_EOD_Send.Header_EOD.PosTime[0] = device_time[0];
	d_EOD_Send.Header_EOD.PosTime[1] = device_time[1];
	d_EOD_Send.Header_EOD.PosTime[2] = device_time[3];
	d_EOD_Send.Header_EOD.PosTime[3] = device_time[4];
/* bm YTL */

/* Toygar EMVed */
	d_EOD_Send.Header_EOD.MSG_INDICATOR = 'E';
/* Toygar EMVed */
	d_EOD_Send.Header_EOD.MSG_TYPE = '6';
	d_EOD_Send.Header_EOD.POS_software_ver = POS_software_ver;
	memcpy(d_EOD_Send.Header_EOD.merch_device_no, merch_device_no, 8);
	d_EOD_Send.Header_EOD.device_serial_no = device_serial_no;
	d_EOD_Send.Header_EOD.transaction_type = '1';

	/* convert the batch_no to ascii, pad it with '0' by rigth justifying and move it to the memeber */
	Utils_IToA(batch_no,batch_no_c);        
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(d_EOD_Send.batch_no, batch_no_c, 3);

	Utils_IToA(d_Totals_Counts[SALE-1].tran_count,count_temp);      
	Utils_LeftPad(count_temp, '0', 4);
	memcpy(d_EOD_Send.sales_count, count_temp,4);

	Utils_IToA(d_Totals_Counts[MANUEL-1].tran_count,count_temp);    
	Utils_LeftPad(count_temp, '0', 4);
	memcpy(d_EOD_Send.manuel_count, count_temp,4);

	Utils_IToA(d_Totals_Counts[RETURN-1].tran_count,count_temp);    
	Utils_LeftPad(count_temp, '0', 4);
	memcpy(d_EOD_Send.return_count, count_temp,4);

	Utils_IToA(d_Totals_Counts[PREPROV-1].tran_count,count_temp);   
	Utils_LeftPad(count_temp, '0', 4);
	memcpy(d_EOD_Send.preprov_count, count_temp,4);


	/* move general tansaction type based totals of amount */
	memcpy(d_EOD_Send.sales_totals, d_Totals_Counts[SALE-1].tran_amount, 13);
	memcpy(d_EOD_Send.manuel_totals,d_Totals_Counts[MANUEL-1].tran_amount, 13);
	memcpy(d_EOD_Send.return_totals,d_Totals_Counts[RETURN-1].tran_amount, 13);
	memcpy(d_EOD_Send.preprov_totals,d_Totals_Counts[PREPROV-1].tran_amount, 13);

	memcpy(d_EOD_Send.last_transact_batch_no, last_transact_batch_no, 3);
	memcpy(d_EOD_Send.last_transact_seq_no, last_transact_seq_no, 4);
	d_EOD_Send.last_transact_status = last_transact_status;

	/* copy agreement total*/
	/*memcpy(d_EOD_Send.agreement_total, ((char*)agreement_total)+3, 13);*/
	memcpy(d_EOD_Send.agreement_total, agreement_total, 13);

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_NewBatchNo
 *
 * DESCRIPTION          : New batch no request and receive
 *
 * RETURN                       : STAT_OK
 *                                        STAT_NOK
 *                                        TRR_NO_CONNECTION
 *
 * NOTES                        : r.i.o & t.c.a 02/06/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_NewBatchNo(void)
{
	char NewBatch_temp[33];
	byte Trans_NewBatch_ret;
	sint i,k;
	char ch;
/* 06_01 RECONCILATION irfan 20/09/2002 */
	char lbSend_Buffer[900];


/* Toygar EMVed */
/*	memset(lbSend_Buffer,0,256);                    *//* initialize the lbSend_Buffer . Provides Null at the end of mess. as well */
	memset( lbSend_Buffer, 0, 900 );	/* initialize the lbSend_Buffer . Provides Null at the end of mess. as well */
/* Toygar EMVed */
	memset(EOD_error_message, ' ', sizeof(EOD_error_message));

	/* Prepare the NewBatchNo Message by Filling the Param_Send structure */

	Trans_FillParamSendStructure();

	/* eger agreement islemi basarili ise hala connection olduðundan newbatchno gonderilebilir*/
	
	/* Prepare the Send Message*/
	memcpy (lbSend_Buffer,&d_Param_Send,sizeof(d_Param_Send));

	/* Send the Prepared message to the Host */

/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Trans_NewBatch_ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
	else Trans_NewBatch_ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);
/* Toygar 13.05.2002 End */

	if(Trans_NewBatch_ret != STAT_OK)
	{               
		return(Trans_NewBatch_ret);
	}
    
	
	/* Analyze the response */

	/* Move Received_Buffer to the d_Param_Receive */
	memcpy(&d_Param_Receive,Received_Buffer+1,sizeof(d_Param_Receive));

	/*if ( d_EOD_Receive.Header_EOD.MSG_TYPE != '7')*/      /* UPDATE 01_16 26/10/99 Yeni Yýðýn mesajýnda
														Mutabakat mesajýna ait MSG TYPE kontrol ediliyordu.
														Düzeltildi.*/
	if ( d_Param_Receive.Header_EOD.MSG_TYPE != '7')
		
	{
		memset(NewBatch_temp,0,sizeof(NewBatch_temp));
		/*UPDATE 01_15 24/10/99 & kaldýrýldý */
		/*sprintf(NewBatch_temp,"BEKLENMEYEN MES.   KODU: %c",&d_Param_Receive.Header_EOD.MSG_TYPE);*/
		sprintf(NewBatch_temp,"BEKLENMEYEN MES.   KODU: %c",d_Param_Receive.Header_EOD.MSG_TYPE);
#ifndef GRAFDISP
		Display_DownDisplay(NewBatch_temp);
#else
		Debug_GrafPrint2((char *)NewBatch_temp, 3);
#endif

		return(STAT_NOK);
	}


	/* UPDATE 01_16 24/10/99. Dönen mesaj da ortak field'lar kontrol ediliyor */
	/* bozuk gelen ama LRC den gecen mesaj larda bozuk mesajý sense edebilmek icin yapýlýyor*/
	if(Trans_CheckEODNewBatchNoReturnMessage() != STAT_OK)
	{
		return(STAT_NOK);
	}

	if ( memcmp(d_Param_Receive.confirmation_code, "320", 3) == 0 )
	{
		/* approved by host*/
		/* delete batch and create batch*/
		Trans_NewBatch_ret = Files_InitTransaction();
		if(Trans_NewBatch_ret != STAT_OK)
		{
		Debug_GrafPrint2("YI¦IN S˜LME HATA", 3);
			return(Trans_NewBatch_ret);
		}

		/* 06_30 Yeni yigin mesajý yanýtý alýndýðýnga OFFAKB iþlem dosyasýnýn yeniden yaratilmesi gerekir */
		if( Offline_InitOfflineAkbBatchFile()  != STAT_OK ) return(STAT_NOK);


#ifndef GRAFDISP
		Display_DownDisplay("YEN˜ YI¦INI ALDI");
#else
		Debug_GrafPrint2("YEN˜ YI¦INI ALDI", 3);
#endif

		/* change batch number*/
		memcpy(batch_no_c, d_Param_Receive.new_batch_no, 3);
		batch_no_c[3]=0;

		batch_no = atoi(batch_no_c);

		/* UPDATE 01_18 02/12/99 en son batch_no_c:1.1. þekline dönüþüyor.( . ->Null) Bu yanlýþ*/
		/*Utils_IToA(batch_no,batch_no_c);
		batch_no_c[sizeof(batch_no_c)]=0;*/

		Trans_Initialize_Variables();

		/* parametere update leri sadece param ver no farklý olduðunda yapýlmalý. ama burada her zaman yapýlýyor */
		memcpy(param_ver_no, d_Param_Receive.new_param_ver_no, 3);
		param_ver_no[3]=0;
		device_serial_no = d_Param_Receive.new_device_serial_no;
		memcpy(merch_no, d_Param_Receive.merch_no, 12);
		merch_no[12] = 0;
/*
	 	Printer_WriteStr("d_Param_Receive.merch_name:\n");
		PrntUtil_BufAsciiHex((byte *)d_Param_Receive.merch_name,24);
	 	Printer_WriteStr("d_Param_Receive.merch_address1:\n");
		PrntUtil_BufAsciiHex((byte *)d_Param_Receive.merch_address1,24);
	 	Printer_WriteStr("d_Param_Receive.merch_address2:\n");
		PrntUtil_BufAsciiHex((byte *)d_Param_Receive.merch_address2,24);
*/
		memcpy(merch_addr, d_Param_Receive.merch_name, 24);
		memcpy(merch_addr+24, d_Param_Receive.merch_address1, 24);
		memcpy(merch_addr+48, d_Param_Receive.merch_address2, 24);
		merch_addr[72]=0;

		/* phone number update 1 */
		k=0;
		for (i=0; i<15 ; i++)
		{
			ch = d_Param_Receive.tel1[i];
			
			if(isdigit(ch) == 0)
			{       /* charactare is not digit */
				if ((ch == '.') || (ch == ','))
				{
					tel_no_1[k] = ch;
					k++;
				}
				else if(ch == '-')
				{
					tel_no_1[k] = '.';
					k++;
				}
			}
			else
			{       /* charactare is digit */

				tel_no_1[k] = ch;
				k++;
			}
		}
		tel_no_1[k] = 0;

		/* update phone number 2 */
		k=0;
		for (i=0; i<15 ; i++)
		{
			ch = d_Param_Receive.tel2[i];

			if(isdigit(ch) == 0)
			{       /* charactare is not digit */
				if ((ch == '.') || (ch == ','))
				{
					tel_no_2[k] = ch;
					k++;
				}
				else if(ch == '-')
				{
					tel_no_2[k] = '.';
					k++;
				}
			}
			else
			{       /* charactare is digit */

				tel_no_2[k] = ch;
				k++;
			}
		}
		tel_no_2[k] = 0;


		/* update line type */
		Line_Type = d_Param_Receive.line_type;

		/* upate working key */
		memcpy(working_key, d_Param_Receive.PINPad_working_key,16);

		/* update master key no */
		master_key_no = d_Param_Receive.master_key_no;

		/* update amex merch no */
		memcpy(merch_AMEX_no, d_Param_Receive.AMEX_merch_no, 10);
		merch_AMEX_no[10]=0;

		/* update auot eod time */
		memcpy(auto_EOD_time, d_Param_Receive.auto_EOD_time, 4);
		auto_EOD_time[4]=0;

		auto_EOD_time_int = atoi(auto_EOD_time);

/*
		Printer_WriteStr("auto_EOD_time:\n");
		PrntUtil_BufAsciiHex((byte *)auto_EOD_time,5);
		Printer_WriteStr("auto_EOD_time_int:\n");
		PrntUtil_BufAsciiHex((byte *)&auto_EOD_time_int,4);
*/


		/* ATS_FLAG update */
		if(d_Param_Receive.ATS_FLAG == '1')
		{
			ATS_FLAG = TRUE;
		}
		else
		{
			ATS_FLAG = FALSE;
		}
#if 0
		if (d_Param_Receive.cont == '1')
		{
			/* continue EOD transaction with Parameter Download*/
			EOD_OK= TRUE;
			return(STAT_OK);        /* cont. with parameter downloding */
		}
		else            /*  (d_Param_Receive.cont == '2') or any other value*/
		{
			/* stop EOD transaction*/
			EOD_OK=TRUE;
			EOD_pointer='0';
			EOD_flag=FALSE;
			INSTALLATION = TRUE;
			return(STAT_OK);        /* finish the EOD process */
		}
#endif
		switch(d_Param_Receive.cont)
		{
		case '1': /* continue to download params */
		case '3': /* continue to download EMV Keys. */
		case '4': /* continue to download EMV Config. */
		case '5': /* continue to download EMV Keys and Config. */
		case '6': /* continue to download Params and EMV Keys. */
		case '7': /* continue to download Params and EMV Config. */
		case '8': /* continue to download Params and EMV Keys and EMV Config. */
			ContTypeFlag = d_Param_Receive.cont;
/*
			Printer_WriteStr("ContTypeFlag:\n");
			PrntUtil_BufAsciiHex((byte *)&ContTypeFlag,1);
*/
			break;
		case '2': /* do not continoue and end the EOD. */
		default:
			/* stop EOD transaction*/
			EOD_OK=TRUE;
			EOD_pointer='0';
			EOD_flag=FALSE;
			INSTALLATION = TRUE;
			ContTypeFlag = 'S';
/*
			Printer_WriteStr("ContTypeFlag:\n");
			PrntUtil_BufAsciiHex((byte *)&ContTypeFlag,1);
*/
			return(STAT_OK);        /* finish the EOD process */
		}
		EOD_OK= TRUE;
		return(STAT_OK);        /* cont. with downloding */
	}
	else
	{
		memcpy(EOD_error_message, d_Param_Receive.message, 16);
		EOD_error_message[16]=0;
		EOD_OK=FALSE;
		return(STAT_NOK);       /* newbatch no ends with error */
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_FillParamSendStructure.
 *
 * DESCRIPTION          : Fill the Param Send Structure in EOD process
 *
 * RETURN                       : none
 *
 * NOTES                        : r.i.o & t.c.a 02/06/99
 *
 * ------------------------------------------------------------------------ */
void Trans_FillParamSendStructure(void)
{

	char temp[28];
	int new_batch;                  /* uint int yapýldý */
	char new_batch_c[4];    /* ascii conversion icin tanýmlandý.*/
	char total_batch_time_c[9];
	char device_date[8], device_time[6];

	/* initialize the send structure */
	/*memcpy(&d_Param_Send,'0',sizeof(d_Param_Send));*/     /* UPDATE 01_15 24/10/99 yanlýþ memcpy kullanýmý*/
	memset(&d_Param_Send,'0',sizeof(d_Param_Send));
/* bm YTL 21/06/2004 */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		d_Param_Send.Header_Param.Cur_Index = 'X';
	else
		d_Param_Send.Header_Param.Cur_Index = 'Y';
	d_Param_Send.Header_Param.ActAppCode = POS_Type;
	d_Param_Send.Header_Param.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
	memcpy( d_Param_Send.Header_Param.PosType, "LIP", 3 );
	memcpy( d_Param_Send.Header_Param.PosModel, "8000", 4 );
	memcpy( d_Param_Send.Header_Param.PosVersion, POS_VERSION_NUMBER, 4 );
	memset( d_Param_Send.Header_Param.Cashier_Id, '0', 16 );
	memset( d_Param_Send.Header_Param.IMEI_ID, '0', 15 );
	memset( d_Param_Send.Header_Param.Filler, ' ', 19 );

	Utils_GetDateTime( device_date, device_time );
	d_Param_Send.Header_Param.PosDate[0] = device_date[6];
	d_Param_Send.Header_Param.PosDate[1] = device_date[7];
	d_Param_Send.Header_Param.PosDate[2] = device_date[3];
	d_Param_Send.Header_Param.PosDate[3] = device_date[4];
	d_Param_Send.Header_Param.PosDate[4] = device_date[0];
	d_Param_Send.Header_Param.PosDate[5] = device_date[1];
	d_Param_Send.Header_Param.PosTime[0] = device_time[0];
	d_Param_Send.Header_Param.PosTime[1] = device_time[1];
	d_Param_Send.Header_Param.PosTime[2] = device_time[3];
	d_Param_Send.Header_Param.PosTime[3] = device_time[4];

/* bm YTL */

/* Toygar EMVed */
	d_Param_Send.Header_Param.MSG_INDICATOR = 'E';
/* Toygar EMVed */
	/* fill Param_Send structure */
	d_Param_Send.Header_Param.MSG_TYPE = '6';
	d_Param_Send.Header_Param.POS_software_ver = POS_software_ver;
	memcpy(d_Param_Send.Header_Param.merch_device_no, merch_device_no, 8);
	d_Param_Send.Header_Param.device_serial_no = device_serial_no;
	d_Param_Send.Header_Param.transaction_type = '2';

	/* convert the batch_no to ascii, pad it with '0' by rigth justifying and move it to the memeber */
	memset(batch_no_c,0,4);
	Utils_IToA(batch_no,batch_no_c);        
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(d_Param_Send.batch_no, batch_no_c, 3);

	memcpy(d_Param_Send.param_ver_no, param_ver_no, 3);

	/* increment batch number*/
	/*ascii_to_uint( &new_batch, (byte *)batch_no, 3);*/
	new_batch = batch_no; /* yukarýdaki satýr yerine yazýldý. mevcut batch_no new_batch'e atandý */
	new_batch++;
	if (new_batch > 999) new_batch=1;	/* 0 idi. 1 yapýldi 03/05/2001*/
	/* convert the new_batch to ascii, pad it with '0' by rigth justifying and move it to the memeber */
	Utils_IToA(new_batch,new_batch_c);      
	Utils_LeftPad(new_batch_c, '0', 3);
	memcpy(d_Param_Send.new_batch_no, new_batch_c, 3);


	/* copy batch_statistics to Param_Send structure*/

	/* initialise statistsics and collect the datas of statistics for both access server*/

	sprintf(temp, "%03d%03d%03d%03d%03d%03d%03d%03d%03d",   d_Statistics1.successful,
															d_Statistics1.no_tone,
															d_Statistics1.no_answer,
															d_Statistics1.handshake_error,
															d_Statistics1.busy,
															d_Statistics1.carrier_lost,
															d_Statistics1.no_response_from_host,
															d_Statistics1.power_fail,
															d_Statistics1.no_enq_from_host);
	temp[27]=0;


	memcpy(d_Param_Send.batch_statistics1, temp,27);


	/*memset(d_Param_Send.batch_statistics2, '0', sizeof(d_Param_Send.batch_statistics2));*/
	sprintf(temp, "%03d%03d%03d%03d%03d%03d%03d%03d%03d",   d_Statistics2.successful,
															d_Statistics2.no_tone,
															d_Statistics2.no_answer,
															d_Statistics2.handshake_error,
															d_Statistics2.busy,
															d_Statistics2.carrier_lost,
															d_Statistics2.no_response_from_host,
															d_Statistics2.power_fail,
															d_Statistics2.no_enq_from_host);
	temp[27]=0;

	memcpy(d_Param_Send.batch_statistics2, temp,27);


	/* copy total time to Param_Send structure*/
	/* sadece toplam süre gerekiyor */
	
	Utils_IToA(total_batch_time,total_batch_time_c);        
	Utils_LeftPad(total_batch_time_c, '0', 8);
	memcpy(d_Param_Send.batch_time, total_batch_time_c, 8);

	/** XLS_PAYMENT 01/08/2001 irfan **/
/* bm YTL 01/07/2004  memcpy( d_Param_Send.POS_Version, POS_VERSION_NUMBER, 4 );	/* 0640 bm YTL 24/06/2004 */
/*	memcpy( d_Param_Send.POS_Type, GPOS_Type, 3 ); bm YTL 24/06/2004 */
/*	memcpy( d_Param_Send.POS_Model, GPOS_Model, 4 );  bm YTL 24/06/2004 */
	memcpy(d_Param_Send.POS_Serial_No, GPOS_Serial_No, 9);
	memcpy(d_Param_Send.POS_Akbank_No, GPOS_Akbank_No_My2, 7);
/*	memcpy(d_Param_Send.POS_Akbank_No, GPOS_Akbank_No_My, 7);
/*	memcpy(d_Param_Send.POS_Akbank_No, GPOS_Akbank_No, 7);*/
	memcpy(d_Param_Send.Pinpad_Serial_No, GPinpad_Serial_No, 9);
	memcpy(d_Param_Send.Pinpad_Akbank_No, GPinpad_Akbank_No, 7);
/*	d_Param_Send.Active_Application_Code = GActive_Application_Code; bm YTL 24/06/2004 */
	d_Param_Send.Connection_Type = GConnection_Type;
	d_Param_Send.EMV_Key_Version_No = GEMV_Key_Version_No;
	d_Param_Send.Config_Version_No = GConfig_Version_No;
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_ParameterDownload.
 *
 * DESCRIPTION          : Donwload the parameters like Debit Prefixes
 *
 * RETURN               : STAT_OK
 *                        STAT_NOK
 *                        TRR_NO_CONNECTION
 *
 * NOTES                : r.i.o & t.c.a 03/06/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_ParameterDownload(void)
{
	boolean DONE;
	byte ParamDonwload_ret;
	char ParamLoad_temp[33];
	int i;
	byte download_ret;
/* 06_01 RECONCILATION irfan 20/09/2002 */
	char lbSend_Buffer[900];
	char device_date[8], device_time[6];

	DONE = FALSE;
	memset(EOD_error_message, ' ', sizeof(EOD_error_message));

	/* fill Param_Continue_Send structure*/
	Trans_FillParamContinueSendStructure();
	GrphText_Cls (TRUE);
	Debug_GrafPrint2("PARAMET. YšKLEME", 2);
	while (!DONE)
	{
		
		/* irfan 02/09/2002 loop'un icinde pos ver no degisebildigi icin atanmasi gerekir*/
		d_Param_Continue_Send.Header_Param.POS_software_ver = POS_software_ver;


		/* initialize the lbSend_Buffer . Provides Null at the end of mess. as well */
/* Toygar EMVed */
/*		memset(lbSend_Buffer,0,256);*/
		memset( lbSend_Buffer, 0, 900 );
/* Toygar EMVed */

		/* bm YTL 21/06/2004 */
		if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
			d_Param_Continue_Send.Header_Param.Cur_Index = 'X';
		else
			d_Param_Continue_Send.Header_Param.Cur_Index = 'Y';
		d_Param_Continue_Send.Header_Param.ActAppCode = POS_Type;
		d_Param_Continue_Send.Header_Param.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
		memcpy( d_Param_Continue_Send.Header_Param.PosType, "LIP", 3 );
		memcpy( d_Param_Continue_Send.Header_Param.PosModel, "8000", 4 );
		memcpy( d_Param_Continue_Send.Header_Param.PosVersion, POS_VERSION_NUMBER, 4 );
		memset( d_Param_Continue_Send.Header_Param.Cashier_Id, '0', 16 );
		memset( d_Param_Continue_Send.Header_Param.IMEI_ID, '0', 15 );
		memset( d_Param_Continue_Send.Header_Param.Filler, ' ', 19 );

		Utils_GetDateTime( device_date, device_time );
		d_Param_Continue_Send.Header_Param.PosDate[0] = device_date[6];
		d_Param_Continue_Send.Header_Param.PosDate[1] = device_date[7];
		d_Param_Continue_Send.Header_Param.PosDate[2] = device_date[3];
		d_Param_Continue_Send.Header_Param.PosDate[3] = device_date[4];
		d_Param_Continue_Send.Header_Param.PosDate[4] = device_date[0];
		d_Param_Continue_Send.Header_Param.PosDate[5] = device_date[1];
		d_Param_Continue_Send.Header_Param.PosTime[0] = device_time[0];
		d_Param_Continue_Send.Header_Param.PosTime[1] = device_time[1];
		d_Param_Continue_Send.Header_Param.PosTime[2] = device_time[3];
		d_Param_Continue_Send.Header_Param.PosTime[3] = device_time[4];

		/* bm YTL */

		/* Prepare the Send Message*/
		memcpy (lbSend_Buffer,&d_Param_Continue_Send,sizeof(d_Param_Continue_Send));

		/* eger NewBatchNo islemi basarili ise hala connection olduðundan Parameter Donwloading mesajý gonderilebilir*/
		/* Send the Prepared message to the Host */

/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) ParamDonwload_ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
		else ParamDonwload_ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);
/* Toygar 13.05.2002 End */

		if(ParamDonwload_ret != STAT_OK)
		{               
			Debug_GrafPrint2(" BAžARISIZ OLDU ", 3);
			EmvIF_ClearDsp(DSP_MERCHANT);
			return(ParamDonwload_ret);
		}

		/* analyse the response */
		/* Move Received_Buffer to the d_Param_Continue_Receive */
		memcpy(&d_Param_Continue_Receive,Received_Buffer+1,sizeof(d_Param_Continue_Receive));
		if (d_Param_Continue_Receive.Header_Param.MSG_TYPE != '7')
		{
			memset(ParamLoad_temp,0,sizeof(ParamLoad_temp));
			sprintf(ParamLoad_temp,"BEKLENMEYEN MES.   KODU: %c",d_Param_Continue_Receive.Header_Param.MSG_TYPE);
			Mainloop_DisplayWait(ParamLoad_temp,1); /* NO_WAIT yapýlabilir */
			return(STAT_NOK);
		}

		/*UPDATE 01_16 26/10/99 giden ve gelen mesajlarda LRC kontrolunden gecse dahi hatalý gelen
		  mesajýn kontrolu icin cihaz no  larý kontrol ediliyor*/
		/* Compare merch_device_no*/
		if (memcmp(d_Param_Continue_Send.Header_Param.merch_device_no, d_Param_Continue_Receive.Header_Param.merch_device_no, 8) != 0)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("YAZILIM YUKLEME TERMINAL NO UYUSMUYOR");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("YAZILIM YUKLEME TERMINAL NO UYUSMUYOR",1);
			return (STAT_NOK);
		}
		POS_software_ver = (d_Param_Continue_Receive.Header_Param.POS_software_ver);/* asagýda var sonra silinecek */
		
		Debug_GrafPrint2("YAZ. VER.NO DE¦.", 3);
		download_ret = Trans_UpdateParameters();	
		if (download_ret == STAT_OK) POS_software_ver = (d_Param_Continue_Receive.Header_Param.POS_software_ver);
		else
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Debug_GrafPrint2("PARAM.UPDA. HATA", 3);
			return(STAT_NOK);
		}
		if (d_Param_Continue_Receive.cont == '1')       /* continue parameter loading*/
		{
			d_Param_Continue_Send.param_location_no[0] = d_Param_Continue_Receive.param_location_no[0];
			d_Param_Continue_Send.param_location_no[1] = d_Param_Continue_Receive.param_location_no[1];
			d_Param_Continue_Send.param_location_no[2] = d_Param_Continue_Receive.param_location_no[2];
			d_Param_Continue_Send.param_location_no[3] = d_Param_Continue_Receive.param_location_no[3];
			memcpy(ParamLoad_temp,d_Param_Continue_Receive.param_location_no,4);
			ParamLoad_temp[4]=0;
			Debug_GrafPrint2((char *)ParamLoad_temp, 3);
		}
		else if (d_Param_Continue_Receive.cont == '2')  /* stop parameter loading*/
		{
/*
			Printer_WriteStr("ContTypeFlag:\n");
			PrntUtil_BufAsciiHex((byte *)&ContTypeFlag,1);
*/			
			switch(ContTypeFlag)
			{
			case '5':
			case '7':
/*
				d_Param_Continue_Send.param_location_no[0] = '7';
				d_Param_Continue_Send.param_location_no[1] = '7';
				d_Param_Continue_Send.param_location_no[2] = '7';
				d_Param_Continue_Send.param_location_no[3] = '7';
				memcpy(ParamLoad_temp,"7777",4);
*/
				d_Param_Continue_Send.param_location_no[0] = '9';
				d_Param_Continue_Send.param_location_no[1] = '9';
				d_Param_Continue_Send.param_location_no[2] = '9';
				d_Param_Continue_Send.param_location_no[3] = '6';
				memcpy(ParamLoad_temp,"9996",4);
				ParamLoad_temp[4]=0;
				Debug_GrafPrint2((char *)ParamLoad_temp, 3);
				ContTypeFlag = 'S';
/*
				Printer_WriteStr("ContTypeFlag:\n");
				PrntUtil_BufAsciiHex((byte *)&ContTypeFlag,1);
*/
				break;
			case '3':
			case '6':
			case '8':
/*
				d_Param_Continue_Send.param_location_no[0] = '8';
				d_Param_Continue_Send.param_location_no[1] = '8';
				d_Param_Continue_Send.param_location_no[2] = '8';
				d_Param_Continue_Send.param_location_no[3] = '8';
				memcpy(ParamLoad_temp,"8888",4);
*/
				d_Param_Continue_Send.param_location_no[0] = '9';
				d_Param_Continue_Send.param_location_no[1] = '9';
				d_Param_Continue_Send.param_location_no[2] = '9';
				d_Param_Continue_Send.param_location_no[3] = '7';
				memcpy(ParamLoad_temp,"9997",4);
				ParamLoad_temp[4]=0;
				Debug_GrafPrint2((char *)ParamLoad_temp, 3);
				if (ContTypeFlag == '8') ContTypeFlag = '5';
				else ContTypeFlag = 'S';
/*
				Printer_WriteStr("ContTypeFlag:\n");
				PrntUtil_BufAsciiHex((byte *)&ContTypeFlag,1);
*/

				break;
			default:
				EOD_OK=TRUE;
				DONE = TRUE;
				EmvIF_ClearDsp(DSP_MERCHANT);
				Debug_GrafPrint2("     B˜TT˜.     ", 3);
			}
		}
/* Toygar EMVed */
/*
		else if (d_Param_Continue_Receive.cont == '3')
		{
			EOD_OK=TRUE;
			DONE = TRUE;
			return(STAT_MAY);
		}
*/
		else
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("   DEVAM KODU     ANLAžILAMADI  ",2);
			EOD_OK=TRUE;
			DONE=TRUE;
			return(STAT_NOK);
		}
	}

	return(STAT_OK);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_FillParamContinueSendStructure.
 *
 * DESCRIPTION          : Fill the Param Continue Send Structure in EOD process
 *
 * RETURN                       : none
 *
 * NOTES                        : r.i.o & t.c.a 03/06/99
 *
 * ------------------------------------------------------------------------ */
void Trans_FillParamContinueSendStructure(void)
{
	char last_location[5];
	char device_date[8], device_time[6];

/*
	Printer_WriteStr("ContTypeFlag:\n");
	PrntUtil_BufAsciiHex((byte *)&ContTypeFlag,1);
*/
	switch(ContTypeFlag)
	{
	case '3':
/*		strcpy(last_location,"8888"); *//* continue to download EMV Keys. */
		strcpy(last_location,"9997"); /* continue to download EMV Keys. */
		break;
	case '4':
/*		strcpy(last_location,"7777"); *//* continue to download EMV Config. */
		strcpy(last_location,"9996"); /* continue to download EMV Config. */
		break;
	case '5':
/*		strcpy(last_location,"8888"); *//* continue to download params */
		strcpy(last_location,"9997"); /* continue to download params */
		break;
	case '1':
	case '6':
	case '7':
	case '8':
	default:
/*		strcpy(last_location,"9999");*/ /* continue to download params */
		strcpy(last_location,"9998"); /* continue to download params */
		break;
	}
	last_location[4]=0;

	/* initialize the send structure */
	memset(&d_Param_Continue_Send,'0',sizeof(d_Param_Continue_Send));

/* bm YTL 21/06/2004 */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		d_Param_Continue_Send.Header_Param.Cur_Index = 'X';
	else
		d_Param_Continue_Send.Header_Param.Cur_Index = 'Y';
	d_Param_Continue_Send.Header_Param.ActAppCode = POS_Type;
	d_Param_Continue_Send.Header_Param.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
	memcpy( d_Param_Continue_Send.Header_Param.PosType, "LIP", 3 );
	memcpy( d_Param_Continue_Send.Header_Param.PosModel, "8000", 4 );
	memcpy( d_Param_Continue_Send.Header_Param.PosVersion, POS_VERSION_NUMBER, 4 );
	memset( d_Param_Continue_Send.Header_Param.Cashier_Id, '0', 16 );
	memset( d_Param_Continue_Send.Header_Param.IMEI_ID, '0', 15 );
	memset( d_Param_Continue_Send.Header_Param.Filler, ' ', 19 );

	Utils_GetDateTime( device_date, device_time );
	d_Param_Continue_Send.Header_Param.PosDate[0] = device_date[6];
	d_Param_Continue_Send.Header_Param.PosDate[1] = device_date[7];
	d_Param_Continue_Send.Header_Param.PosDate[2] = device_date[3];
	d_Param_Continue_Send.Header_Param.PosDate[3] = device_date[4];
	d_Param_Continue_Send.Header_Param.PosDate[4] = device_date[0];
	d_Param_Continue_Send.Header_Param.PosDate[5] = device_date[1];
	d_Param_Continue_Send.Header_Param.PosTime[0] = device_time[0];
	d_Param_Continue_Send.Header_Param.PosTime[1] = device_time[1];
	d_Param_Continue_Send.Header_Param.PosTime[2] = device_time[3];
	d_Param_Continue_Send.Header_Param.PosTime[3] = device_time[4];

/* bm YTL */

/* Toygar EMVed */
	d_Param_Continue_Send.Header_Param.MSG_INDICATOR = 'E';
/* Toygar EMVed */
	/* fill Param_Continue_Send structure */
	d_Param_Continue_Send.Header_Param.MSG_TYPE = '6';
	d_Param_Continue_Send.Header_Param.POS_software_ver = POS_software_ver;
	memcpy(d_Param_Continue_Send.Header_Param.merch_device_no, merch_device_no, 8);
	d_Param_Continue_Send.Header_Param.device_serial_no = device_serial_no;
	d_Param_Continue_Send.Header_Param.transaction_type = '3';
	memcpy(d_Param_Continue_Send.param_location_no, last_location, 4);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GetAmount and the # of installment
 *
 * DESCRIPTION          : Gets the amount  and the # of installment (if ATS_FLAG is TRUE)
 *
 * RETURN                       : TRUE or FALSE
 *
 * NOTES                        : r.i.o & t.c.a 14/06/99
 *
 * ------------------------------------------------------------------------ */
/* 06_00 concilation data parametre olarak geciliyor 25/09/2002 irfan */
boolean Trans_GetAmount( char *passConcilation_Data, boolean Kampanya )
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	boolean ATS_COUNT_OK;
	byte temp_taksit[3];    /* 02_01 08/09/2000 irfan pinpad den taksit alma */

   /* XLS_INTEGRATION2 IRFAN 23/10/20018/2001*/
	byte xls_ret_local;
	char xls_temp_local[33];
/*	char XLS_Cardno_local[20];	*/
	int YTL_Digit;				/* bm YTL 29/06/2004 */
	/* FAIZLI 02_00 28/07/2000 */
	boolean FAIZLI_IADE_OK;                 /* TRUE: giriþ geçerli, FALSE: giriþ geçersiz */

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* FAIZLI 02_00 28/07/2000 */
	char sale_date_day[3],sale_date_month[3];
	sint sale_date_day_value,sale_date_month_value;

	/** XLS_PAYMENT 20/07/2001 irfan **/
	char XLS_Cardno[20];	/** XLS_INTEGRATION 16/08/2001 21 idi. 20 yapýldý. **/
/*	char zero_amount[14];*/
	char first_6_digit_local[7];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));
	
	/** 06_00 card no local bulunuyor 26/09/2002 irfan **/
	memset(XLS_Cardno, 'F', sizeof(XLS_Cardno));
	memcpy(XLS_Cardno+(sizeof(XLS_Cardno)-1)-strlen(card_no),card_no,strlen(card_no));
	XLS_Cardno[19] = 0;

	memcpy(first_6_digit_local,card_no,6);
	first_6_digit_local[6] = 0;

	/* verde */
	if( !VERDE_TRANSACTION || (glAmount_Req_Type == '1') ) 
	{
		if ( ( strstr( Loyalty_Prefixes, first_6_digit_local ) != NULL ) &&
			 ( ( POS_Type == '4' ) || ( POS_Type == '6' ) ) &&
			 ( tran_type == RETURN ) )
		{
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) ) YTL_Digit = 10;
			else YTL_Digit = 11;
			if ( !Mainloop_GetKeyboardInput
				 ( "PROVIZYON TUTARI", Buffer, 0, YTL_Digit, FALSE, FALSE,
				   FALSE, TRUE, FALSE, &Len, value_null, FALSE ) )
			{
				EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				TL_Iade = FALSE;
				return(FALSE);
			}
			else
			{
				if( TL_Iade == TRUE)
				{					
					memset(amount_Iade, 0 , sizeof(amount_Iade));
					memcpy(amount_Iade,Buffer,Len);

					memcpy( amount_surcharge, Buffer, Len );
					amount_surcharge[Len] = 0;
					Utils_LeftPad( amount_surcharge, '0', 13 );

					Main_AddAscii(Buffer,sizeof(Buffer),"5000",4,Buffer, sizeof(Buffer));

					memcpy(amount_surcharge_iade + 4,Buffer,Len);
					amount_surcharge_iade[Len]=0;
					Utils_LeftPad(amount_surcharge_iade, '0', 13);
				}
				else
				{
					memcpy(amount_surcharge,Buffer,Len);
					amount_surcharge[Len]=0;
					Utils_LeftPad(amount_surcharge, '0', 13);
				}
			}
		}
		else
		{	/*bm YTL 29/06/2004 8-YTL, 11-TL */
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) ) YTL_Digit = 10;
			else YTL_Digit = 11;
			if ( !Mainloop_GetKeyboardInput
				 ( ( char * ) MESSAGE_ENTER_AMOUNT, Buffer, 0, YTL_Digit,
				   FALSE, FALSE, FALSE, TRUE, FALSE, &Len, value_null,
				   FALSE ) )
			{
				EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				TL_Iade = FALSE;
				return ( FALSE );
			}
			else
			{
				if( TL_Iade == TRUE)
				{
					memset(amount_Iade, 0 , sizeof(amount_Iade));
					memcpy(amount_Iade,Buffer,Len);
					memcpy( amount_surcharge, Buffer, Len );
					amount_surcharge[Len] = 0;
					Utils_LeftPad( amount_surcharge, '0', 13 );
					Main_AddAscii(Buffer,sizeof(Buffer),"5000",4,Buffer, sizeof(Buffer));
					memcpy(amount_surcharge_iade + 4,Buffer,Len);
					amount_surcharge_iade[Len]=0;
					Utils_LeftPad(amount_surcharge_iade, '0', 13);
				}
				else
				{
					memcpy(amount_surcharge,Buffer,Len);
					amount_surcharge[Len]=0;
					Utils_LeftPad(amount_surcharge, '0', 13);
				}
			}
		}
	}

	/** 06_00 26/09/2002 irfan.  **/
	memset(amount_surcharge_reverse, 0, sizeof(amount_surcharge_reverse));
	if ( ( strstr( Loyalty_Prefixes, first_6_digit_local ) != NULL ) &&
/*bm OPT ( POS_Type == '0' ) && ( tran_type == RETURN ) ) */
		 ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
		 && ( tran_type == RETURN ) )
	{	/* axess POS icin gecerli */
		/* 06_00 her durumda authorized amount soruluyor */
		/*bm YTL 29/06/2004 8-YTL, 11-TL */
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
			YTL_Digit = 10;
		else
			YTL_Digit = 11;
		if ( !Mainloop_GetKeyboardInput
			 ( "IADE TUTARI ?", Buffer, 0, YTL_Digit, FALSE, FALSE, FALSE,
			   TRUE, FALSE, &Len, value_null, FALSE ) )
		{
			EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
			EMVAxess = FALSE;
			TL_Iade = FALSE;
				return(FALSE);
		}
		else
		{
			if( TL_Iade == TRUE) /*@04.09.2004 */
			{					
				memset(amount_Iade, 0 , sizeof(amount_Iade));
				memcpy(amount_Iade,Buffer,Len);
				memcpy(amount_surcharge_reverse,Buffer,Len);
				amount_surcharge_reverse[Len]=0;
				Utils_LeftPad(amount_surcharge_reverse, '0', 13);
				Main_AddAscii(Buffer,sizeof(Buffer),"5000",4,Buffer, sizeof(Buffer));
/*				memcpy(amount_surcharge_iade + 4,Buffer,Len);
				amount_surcharge_iade[Len]=0;
				Utils_LeftPad(amount_surcharge_iade, '0', 13); */

				if ( Utils_Compare_Strings ( amount_surcharge_reverse, amount_surcharge ) != 1 )
				{	/* amount_surcharge_reverse < and = amount_surcharge */
					memcpy(amount_surcharge_iade + 4,Buffer,Len);
					amount_surcharge_iade[Len]=0;
					Utils_LeftPad(amount_surcharge_iade, '0', 13);
				}
			}
			else
			{		
				memcpy(amount_surcharge_reverse,Buffer,Len);
				amount_surcharge_reverse[Len]=0;
				Utils_LeftPad(amount_surcharge_reverse, '0', 13);
			}
		}
	}

	/* 02_02 irfan 15/09/00 Taksit sayýsý tutar alýndýktan sonra initialize ediliyor
	Böylece tutarý alýnamadan kesilen bir iþlem icin Recopy (2.kopya) dogru degerler
	ile calýsmýs olacak*/
	/* Initialize the ATS_Count*/
	ATS_Count=1;
	strcpy(ATS_Count_c,"01");
	ATS_Count_c[2] = 0;

/* 06_00 taksit sorma modulunun yeri xls redemption'dan onceye alindi. 24/09/2002 irfan */
	/** XLS_PAYMENT 01/08/2001 if full redemption(Offline trx) do not ask ins. number **/

	/*if( (tran_type != PREPROV) &&  ATS_FLAG && ATS_PERMISSION_FLAG && !OFFLINE_TRX)*/ /* 06_30  */
	if( (tran_type != PREPROV) &&  ATS_FLAG && ATS_PERMISSION_FLAG) /* 06_30 OFFLINE_TRX Kaldiriliyor */
	{
/*			Printer_WriteStr("ATS_PERMISSION_FLAG Normal = TRUE;\n");*/

			ATS_COUNT_OK = TRUE;
			do
			{
				if ( !Mainloop_GetKeyboardInput
					 ( "TAKS˜T SAYISI ?", Buffer, 0, 2, FALSE, FALSE, FALSE,
					   FALSE, FALSE, &Len, value_null, TRUE ) )
				{
					EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
					EMVAxess = FALSE;
					return(FALSE);
				}
				else
				{
					if ( Len == 0 )
					{
						ATS_Count = 1;
						Utils_IToA( ATS_Count, ATS_Count_c );
						ATS_COUNT_OK = FALSE;
					}
					else
					{
						memcpy( ATS_Count_c, Buffer, Len );
						ATS_Count_c[Len] = 0;
				ATS_Count = atoi(ATS_Count_c);

				if (  /*(ATS_Count > 18) || */ ( ATS_Count <= 0 ) )
				{
					ATS_COUNT_OK = TRUE;
				}
				else
				{
					ATS_COUNT_OK = FALSE;
				}
					}
				}
					
				Utils_LeftPad( ATS_Count_c, '0', 2 );

			}
			while ( ATS_COUNT_OK );	/* 02_01 irfan 08/09/2000 taksit sayýsý sorma pinpad den */

		/* FAIZLI 02_00 28/07/2000 Eðer taksitli bir iþlem ise, iade için satýþ tarihi sorulacak */
		/* 06_00 26/09/2002 tum iadeler icin satis tarihi soruluyor. */
		/*if ((tran_type == RETURN) && (ATS_Count !=1)) */


/*13.08.2004   */
			ATS_COUNT_OK = TRUE;
			if (( ATS_Count > 1 ) && (!AMEX_FLAG))
		{
			do
			{
					if ( !Mainloop_GetKeyboardInput
						 ( "FAIZSIZ TAKS˜T:1FAIZLI  TAKS˜T:2", Buffer, 0, 1, FALSE, FALSE, FALSE,
						   FALSE, FALSE, &Len, value_null, TRUE ) )
				{
						EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
						EMVAxess = FALSE;
						return(FALSE);
				}
				else
				{
					if(Len == 0)
					{
							taksit_tipi = 1;
						ATS_COUNT_OK = FALSE;
					}
					else
					{
							taksit_tipi = atoi( Buffer );
							if (taksit_tipi==2)
								IS_LOYALTY_INSTALLMENT_INTEREST = TRUE;
							else
								IS_LOYALTY_INSTALLMENT_INTEREST = FALSE;

							if (  taksit_tipi > 2 )
						{
							ATS_COUNT_OK = TRUE;
						}
						else
						{
							ATS_COUNT_OK = FALSE;
						}
					}
				}
				}
				while ( ATS_COUNT_OK );	
			}
			else if (AMEX_FLAG)
			{
				taksit_tipi = 1;
				ATS_COUNT_OK = FALSE;
			}

/*13.08.2004 */

		}

		if (tran_type == RETURN)
		{
		if ( !Trans_GetApprovalCode(  ) )
		{
			EMVAxessStatu=FALSE; /*@bm 16.09.2004 */
			EMVAxess = FALSE;
			return ( FALSE );
		}
				do
				{
					/** XLS_INTEGRATION2 irfan. 08/11/2001. GGAAYY formatina cevrildi **/
			if ( !Mainloop_GetKeyboardInput
				 ( "SATIž TRH?GGAAYY", Buffer, 6, 6, FALSE, FALSE,
				   FALSE, FALSE, FALSE, &Len, " ", TRUE ) )
					{
						EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
						EMVAxess = FALSE;
						return(FALSE);
					}
					else
					{
						if(Len == 0)
						{
							FAIZLI_IADE_OK = FALSE;
						}
						else
						{
							sale_date_month[0] = Buffer[2];
							sale_date_month[1] = Buffer[3];
							sale_date_month[2] = 0;
							sale_date_month_value = atoi(sale_date_month);

							sale_date_day[0] = Buffer[0];
							sale_date_day[1] = Buffer[1];
							sale_date_day[2] = 0;
							sale_date_day_value = atoi(sale_date_day);

					if ( ( sale_date_month_value <= 12 ) &&
						 ( sale_date_month_value >= 1 ) &&
						 ( sale_date_day_value <= 31 ) &&
						 ( sale_date_day_value >= 1 ) )
							{
								if (sale_date_month_value == 2)
								{
									if (sale_date_day_value > 29)
									{
										FAIZLI_IADE_OK = FALSE;
									}
									else
									{
										/** sisteme gidecek tarih yyaagg olmalý **/
										/*memcpy(Sale_Date_Char, Buffer, 6);*/
										Sale_Date_Char[0] = Buffer[4];
										Sale_Date_Char[1] = Buffer[5];
										Sale_Date_Char[2] = Buffer[2];
										Sale_Date_Char[3] = Buffer[3];
										Sale_Date_Char[4] = Buffer[0];
										Sale_Date_Char[5] = Buffer[1];
										Sale_Date_Char[6] = 0;
										FAIZLI_IADE_OK = TRUE;
									}
								}
								else
								{
									/** sisteme gidecek tarih yyaagg olmalý **/
									/*memcpy(Sale_Date_Char, Buffer, 6);*/
									Sale_Date_Char[0] = Buffer[4];
									Sale_Date_Char[1] = Buffer[5];
									Sale_Date_Char[2] = Buffer[2];
									Sale_Date_Char[3] = Buffer[3];
									Sale_Date_Char[4] = Buffer[0];
									Sale_Date_Char[5] = Buffer[1];
									Sale_Date_Char[6] = 0;
									FAIZLI_IADE_OK = TRUE;
								}
							}
							else
							{
								FAIZLI_IADE_OK = FALSE;
							}
						}
					}
		}
		while ( !FAIZLI_IADE_OK );
	}
	
	/* 06_07 verde den tutar alýnýyor */
	/* VERDE. amount pos tarafýndan saglanacak ise calistirilir */
	if(	VERDE_TRANSACTION)
	{
		if( (glAmount_Req_Type == '1') && VERDE_TRX_CONTINUE )
		{
			if(Verde_SendRes2_GetReq2(amount_surcharge, verde_discount) != STAT_OK)
			{
				VERDE_TRX_CONTINUE = FALSE;		/* verde trx. must be stop. normal trx. goes on */
				if((Verde_SendControlChar(EOT)) != STAT_OK) 
				{
					/* 06_04 debug mode kontrolu eklendi*/
					return(STAT_NOK);
				}
			}
			else
			{		
//bm 15.12.2004 Utils_Subtract_Strings(amount_surcharge, verde_discount, verde_amount);
//bm 15.12.2004 memcpy(amount_surcharge, verde_amount,13);
				amount_surcharge[13] = 0;
				Utils_LeftPad(amount_surcharge, '0', 13);
			}
		}
	}

	if ( (Kampanya == TRUE) && (kampanya_str != 0))
	{
		memset( xls_temp_local, 0, 32 );
		strcpy( xls_temp_local, kampanya_str );
		strcat( xls_temp_local, " ?" );
		if ( !Mainloop_GetKeyboardInput( xls_temp_local, Buffer, 0, 15, FALSE,
						 FALSE, FALSE, FALSE, TRUE, &Len, " ", TRUE ) )
		{
			EMVAxessStatu=FALSE;
			EMVAxess = FALSE;
			return ( FALSE );
		}
		memset( Kampanya_Data, ' ', sizeof( Kampanya_Data ) );
		memcpy( Kampanya_Data, Buffer, Len );
	}
	else memset( Kampanya_Data, '0', sizeof(Kampanya_Data) );		

	/** XLS_PAYMENT 20/07/2001 irfan */
	/** Call XLS module to get modified (purchase amount) amount **/
	if ( (XLS_Check_Initialization_State() == XLS_OPERATIVE) &&
/* bm OPT ( ( LOYALTY_PERMISSION_FLAG ) && ( POS_Type == '0' ) && */
		 ( ( LOYALTY_PERMISSION_FLAG ) &&
		   ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
		   && ( ( tran_type == SALE ) || ( tran_type == MANUEL ) ||
				( tran_type == RETURN ) ) ) )
	{
		if(tran_type != RETURN)
		{
			/** XLS_INTEGRATION 09/08/2001 irfan **/
			memset(XLS_Cardno, 'F', sizeof(XLS_Cardno));
			memcpy( XLS_Cardno + ( sizeof( XLS_Cardno ) - 1 ) -
					strlen( card_no ), card_no, strlen( card_no ) );
			XLS_Cardno[19] = 0;  /** XLS_INTEGRATION 16/08/2001 20 idi. 19 yapýldý. **/
			/* if ( DEBUG_MODE )
			{
				PrntUtil_Cr(2);
				Slips_PrinterStr("XLS_VirtualRedemption\n");
				Slips_PrinterStr("amount_surcharge:\n");
				Slips_PrinterStr(amount_surcharge);
  				PrntUtil_Cr(2);
				Slips_PrinterStr("XLS_Cardno\n");
				Slips_PrinterStr(XLS_Cardno);
				PrntUtil_Cr(2);
			}
			*/
			/** XLS_INTEGRATION2 23/10/2001 irfan. Eger hatalý ise donus kodunu basar**/
			/* 06_00 virtual redemption 24/09/2002 irfan */
			/*xls_ret_local = XLS_Redemption(amount_surcharge,XLS_Cardno,amount_surcharge,1);*/
			/*xls_ret_local = XLS_VirtualRedemption( char *inPurchaseAmount, char *inCardHolderId, char *outModifiedAmount, byte inFlagPaymentCard )*/
			xls_ret_local = XLS_VirtualRedemption( amount_surcharge, XLS_Cardno, amount_surcharge,1);

			/* 06_33 09/12/2003 irfan amount sonuna null eklendi. Cunku WRT eklemiyor */
			amount_surcharge[13] = 0;

/**/
			if(DEBUG_MODE)
			{
				PrntUtil_Cr(2);
				Slips_PrinterStr("XLS_VirtualRedemption donus\n");
				Slips_PrinterStr("modified amount_surcharge\n");
				Slips_PrinterStr(amount_surcharge);
  				PrntUtil_Cr(2);
				Slips_PrinterStr("XLS_Cardno\n");
				Slips_PrinterStr(XLS_Cardno);
				PrntUtil_Cr(2);
			}
/**/
			if( xls_ret_local != XLS_NO_ERROR)
			{
				/** XLS_INTEGRATION2 23/10/2001 irfan. **/
				if(xls_ret_local != 1) /** islem kesilirse asagidaki mesaj verilmeyecek **/
				{
					memset(xls_temp_local, 0, sizeof(xls_temp_local));
					sprintf(xls_temp_local, " virtual REDEMPT. HATALI Hata Kodu: %d ", xls_ret_local);
					Mainloop_DisplayWait(xls_temp_local,1);
					PrntUtil_Cr(2);
					Slips_PrinterStr(xls_temp_local);
					PrntUtil_Cr(2);
				}

				EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				return(FALSE); /** XLS_INTEGRATION 04/09/2001 irfan. STAT_NOK yerine FALSE yapildi. **/

			}

		   /* 06_00 virtual reward eklendi 24/09/2002 irfan */
/**/
			if(DEBUG_MODE)
			{
				PrntUtil_Cr(2);
				Slips_PrinterStr("XLS_VirtualReward ONCESI\n");
				Slips_PrinterStr("ATS_Count_c\n");
				Slips_PrinterStr(ATS_Count_c);
  				PrntUtil_Cr(2);
				Slips_PrinterStr("passConcilation_Data\n");
				Slips_PrinterStr(passConcilation_Data);
  				PrntUtil_Cr(2);
			}
/**/
			
			xls_ret_local = XLS_VirtualReward(ATS_Count_c, (txls_transactiondetail*)passConcilation_Data);
/**/
			if(DEBUG_MODE)
			{
				PrntUtil_Cr(2);
				Slips_PrinterStr("XLS_VirtualReward SONRASI\n");
				Slips_PrinterStr("ATS_Count_c\n");
				Slips_PrinterStr(ATS_Count_c);
  				PrntUtil_Cr(2);
				Slips_PrinterStr("passConcilation_Data\n");
				Slips_PrinterStr(passConcilation_Data);
  				PrntUtil_Cr(2);
			}
/**/
			if( xls_ret_local != XLS_NO_ERROR)
			{
				/** XLS_INTEGRATION2 23/10/2001 irfan. **/
				if(xls_ret_local != 1) /** islem kesilirse asagidaki mesaj verilmeyecek **/
				{
					memset(xls_temp_local, 0, sizeof(xls_temp_local));
					sprintf(xls_temp_local, " Virtual REWARD. HATALI Hata Kodu: %d ", xls_ret_local);
					Mainloop_DisplayWait(xls_temp_local,1);
					PrntUtil_Cr(2);
					Slips_PrinterStr(xls_temp_local);
					PrntUtil_Cr(2);
				}
				EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				return(FALSE); 
			}

		}
		/* 06_00 iade icin virtual reverse eklendi */
		else
		{
/*
			if(DEBUG_MODE)
			{
				PrntUtil_Cr(2);
				Slips_PrinterStr("IADE TRANSACTION\n");
				Slips_PrinterStr("XLS_VirtualReverse ONCESI\n");
				Slips_PrinterStr("amount_surcharge\n");
				Slips_PrinterStr(amount_surcharge);
  				PrntUtil_Cr(2);
				Slips_PrinterStr("amount_surcharge_reverse\n");
				Slips_PrinterStr(amount_surcharge_reverse);
  				PrntUtil_Cr(2);
				Slips_PrinterStr("ATS_Count_c\n");
				Slips_PrinterStr(ATS_Count_c);
				PrntUtil_Cr(2);
				Slips_PrinterStr("XLS_Cardno\n");
				Slips_PrinterStr(XLS_Cardno);
				PrntUtil_Cr(2);
				Slips_PrinterStr("passConcilation_Data\n");
				Slips_PrinterStr(passConcilation_Data);
				PrntUtil_Cr(2);
			}
			*/
			if (( POS_Type == '5' ) || ( POS_Type == '6' ))	/*04.08.2004*/
			{
				  if (TL_Iade) XLS_Currency = 0;
				  else XLS_Currency = 1;
			}
			else XLS_Currency = 0;		 
			
			xls_ret_local = XLS_VirtualReverse( 4, XLS_Currency, amount_surcharge,
													amount_surcharge_reverse,
													ATS_Count_c,
													XLS_Cardno,
													(txls_Returndetail*)passConcilation_Data );
			/* 06_00 iade isleminde reverse tutarý giden mesajdaki tutar alanina yansitiliyor. Aksi taktirde host toplma satiþ tutarýný iade eder */
			/* verde */
			if ( Utils_Compare_Strings
				 ( amount_surcharge_reverse, amount_surcharge ) != 1 )
			{/* amount_surcharge_reverse < and = amount_surcharge */
				memcpy(amount_surcharge, amount_surcharge_reverse,13);
				amount_surcharge[13] = 0;
			}
/**/
			if(DEBUG_MODE)
			{
				PrntUtil_Cr(2);
				Slips_PrinterStr("IADE TRANSACTION\n");
				Slips_PrinterStr("XLS_VirtualReverse SONRASI\n");
				Slips_PrinterStr("XLS_VirtualReverse\n");
				Slips_PrinterStr("amount_surcharge\n");
				Slips_PrinterStr(amount_surcharge);
  				PrntUtil_Cr(2);
				Slips_PrinterStr("amount_surcharge_reverse\n");
				Slips_PrinterStr(amount_surcharge_reverse);
  				PrntUtil_Cr(2);
				Slips_PrinterStr("ATS_Count_c\n");
				Slips_PrinterStr(ATS_Count_c);
				PrntUtil_Cr(2);
				Slips_PrinterStr("XLS_Cardno\n");
				Slips_PrinterStr(XLS_Cardno);
				PrntUtil_Cr(2);
				Slips_PrinterStr("passConcilation_Data\n");
				Slips_PrinterStr(passConcilation_Data);
				PrntUtil_Cr(2);
			}
/**/
			if( xls_ret_local != XLS_NO_ERROR)
			{
				/** XLS_INTEGRATION2 23/10/2001 irfan. **/
				if(xls_ret_local != 1) /** islem kesilirse asagidaki mesaj verilmeyecek **/
				{
					memset(xls_temp_local, 0, sizeof(xls_temp_local));
					sprintf(xls_temp_local, " virtual REVERSE. HATALI Hata Kodu: %d ", xls_ret_local);
					Mainloop_DisplayWait(xls_temp_local,1);
					PrntUtil_Cr(2);
					Slips_PrinterStr(xls_temp_local);
					PrntUtil_Cr(2);
				}
				EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				return(FALSE); /** XLS_INTEGRATION 04/09/2001 irfan. STAT_NOK yerine FALSE yapildi. **/
			}

		}

		OFFLINE_TRX = FALSE;
		/*memset(zero_amount, '0', sizeof(zero_amount));
		zero_amount[13] = 0;

		if (Utils_Compare_Strings(amount_surcharge, zero_amount) == 0)
		{
			/* 06_00 irfan 24/09/2002 offline iptal edildi. hersey online */
			/*OFFLINE_TRX = TRUE;
			Host_Disconnect();      
			Mte_Wait(300);*/
			/*OFFLINE_TRX = FALSE;
		}*/

 	}

	/* UPDATE 01_16 30/10/99 YUKARI PARANTEZIN USTUNDEN ALTINA ALINDI MISSING RETURN VALUE ONLENDI*/
	return(TRUE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GetApprovalCode
 *
 * DESCRIPTION          : Gets the approval code in case of Manuel Transaction
 *
 * RETURN                       : TRUE or FALSE
 *
 * NOTES                        : r.i.o & t.c.a 14/06/99
 *
 * ------------------------------------------------------------------------ */
boolean Trans_GetApprovalCode(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));

	do
		if(!Mainloop_GetKeyboardInput("PROVIZYON NO ?  ",Buffer,6,6,
					       FALSE, FALSE, FALSE, FALSE, TRUE , &Len, " ", FALSE)) return(FALSE);
		else
		{
			memcpy(approval_code,Buffer,Len);
			approval_code[Len]=0;
		}
	while (memcmp(approval_code, "000000", 6) == 0);
	return(TRUE);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_Init_Permanent_Params
 *
 * DESCRIPTION          : Initialize the permanent parameters for the first time
 *
 * RETURN                       : 
 *
 * NOTES                        : 
 *
 * ------------------------------------------------------------------------ */
void Trans_Init_Permanent_Params(void)
{
	ComHndlrParamType  ComHndlrParam_p1;
	int i,j;
	char Debit_Initial[7][66];
	char * s, * p;


	GSMCALL = TRUE;		/* GSMPOS_04_01 16/02/2001 irfan */
	memset(APN,0,12);
	memset(glb_host_ip,0,17);
		
	memcpy(APN,"akbank",6);
	memcpy(glb_host_ip,"192.168.3.23",12);
	glb_host_port=28002;		
	GPRS_USEGPRS = FALSE;
	lsGPRS_Connect_Param.Connected = FALSE;
	lsGPRS_Connect_Param.tcpavailable = FALSE;
	
/*
	GPRS_CheckMode=TRUE;
	GPRS_TryTime =0;*/
	autoGSMRetry=FALSE;
	KAMPANYA_FLAG = 0;
	/* 20.07.2004 */
	EMVAxessStatu = FALSE;

	/*verde */
	VERDE_TRANSACTION = FALSE;

	/*verde */
	EXTERNAL_PINPAD = FALSE;

	/* bm OFF 21.10.2004  */
	OFFLINE_ACIK_FLAG = FALSE;


	/* ara rapor iptali */
	ARA_RAPOR_IPTAL = FALSE;	/* TRUE: ara rapor basilmaz FALSE:ara rapor basilir*/
	
	/* v04_05 aile puan sorgulama 15/07/2002 */
	AILE_PUAN_SORGULAMA = FALSE;		/* aile puan sorgulama flag off. */
	record_counter = 0;
	Grecord_counter = 0;
	
	SORGULAMA   = TRUE ; /* default 1: sorgulama */
	/* 02_00 25/07/00 Faizli iþlem flag i initialize ediliyor. ilk deðeri iþlemin faizsiz olduðu þeklinde*/
	FAIZLI_FLAG = FALSE;    

	/* UPDATE_01_14 20/10/99*/
	RETURN_PERMISSION = TRUE;       /* iade fonksiyonuna izin verme flag'ý. 1: IADE AÇIK. 0: IADE KAPALI*/

	/* UPDATE_01_18 21/06/2000*/
	MANUEL_PERMISSION = TRUE;       /* Manuel iþlem fonksiyonuna izin verme flag'ý. 1: MANUEL ÝÞLEM AÇIK. 0: MANUEL ÝÞLEM KAPALI*/
	
	/* UPDATE 02_03 15/01/01 isim yazma default kapatildi. Finans bank kartlarinda cikan sorundan dolayi */
	/** XLS_INTEGRATION2 22/10/2001. irfan. default acik hale getirildi.*/
	ISIMYAZMA_FLAG = TRUE; /* ýsým yazma kontrolunu saglayan flag default olarak acik yapýlýyor*/

	TRACK1ERROR        = FALSE;     /* default track1 in okunamadýðý yöndedir.*/

	/* UPDATE_01_14 20/10/99*/
	MANUEL_ENTRY_PERMISSION = FALSE;        /* manuel kart giriþe izin verme flag'ý. 1: Þifre  Sor. 0: Þifre sorma*/

	
	/* set click time */
	Kb_ClickTimeSet(60);            /* set buzzer on */
	
	strcpy(SoftPassword, "1971");
	SoftPassword[4]=0;
	strcpy(UserPassword, "4321");
	UserPassword[4]=0;
	strcpy(OperatorPassword, "47326848773614");
	OperatorPassword[14]=0;
	/* 02_01 08/09/2000 pinpad den taksit sorma flag irfan*/
	/* TRUE: pinpad den taksit sorulacak   FALSE: POS den taksit sorulacak */
	PINPAD_TAKSIT_FLAG                              = FALSE;


	WARNING_FLAG                    = TRUE;         /* Slip Çýktý uyarýsý On/Off    */
	FIRST_PHONE_FLAG                = FALSE;        /* birinci tel aranmadý*/
	
	VOID_FLAG                       = FALSE;                
	GVOID_FLAG			= FALSE;                
	INSTALLATION                    = FALSE;
	EOD_flag                        = TRUE;			/* gunsonu yapýlamdý anlamýnda */
	ERROR_RECEIPT                   = FALSE;
	LAST_4_DIGIT_FLAG               = FALSE;                
	AMEX_FLAG                       = FALSE;                
	ATS_FLAG                        = FALSE; 
	AutoEOD                         = TRUE;
	EOD_OK                          = TRUE;
	EOD_pointer                     = '0';			/* start EOD from scratch */
	IS_HOST_CONNECTED_FLAG          = FALSE;        /* at first the host is disconnected */
	ATS_PERMISSION_FLAG             = FALSE;
	FIRST_RUN                       = TRUE;			/* ilk yuklemeden sonra ilk calýsma olduðu anlamýnda */
	TIME_IS_UP                                              = FALSE;                /* auot eod zamaný gelmedi */
	
	/* XLS_PAYMENT related 12/07/2001 irfan */
	LOYALTY_PERMISSION_FLAG         = FALSE;		/*  TRUE  --> prefix of the card is ORTAK
														FALSE --> prefix of the card is not ORTAK*/
	CORRUPTED_CHIP_TRANSACTION_FLAG = FALSE;		/*  TRUE  --> transaction with uncorrupted chip
													    FALSE --> transaction with corrupted chip */ 

	Trans_ChangeParam();

	/* initialize merch amex no */
	memset(merch_AMEX_no,'0',10);
	merch_AMEX_no[10] = 0;

	/* initialize merchant no */
	memset(merch_no,'0',12);
	merch_no[12] = 0;

	/* initialize merch address */
	memset(merch_addr,' ',72);
	merch_addr[72] = 0;

	/* initialize merch address */
	memset(merch_device_no,'0',8);
	merch_device_no[8] = 0;

	auto_EOD_time_int = 9999;
	
	/* initialize line type */
	Line_Type = '0';

	/* initialize tel. no */
	memset(tel_no_1, 0, 16);
	memset(tel_no_2, 0, 16);

	/* initialize statistics */
	memset(&d_Statistics1,0,sizeof(d_Statistics1));
	
	d_Statistics1.busy = 0;
	d_Statistics1.carrier_lost = 0;
	d_Statistics1.handshake_error = 0;
	d_Statistics1.no_answer = 0;
	d_Statistics1.no_enq_from_host = 0;
	d_Statistics1.no_response_from_host = 0;
	d_Statistics1.no_tone = 0;
	d_Statistics1.power_fail = 0;
	d_Statistics1.successful = 0;
	d_Statistics1.total_time = 0;

	memset(&d_Statistics2,0,sizeof(d_Statistics2));
	d_Statistics2.busy = 0;
	d_Statistics2.carrier_lost = 0;
	d_Statistics2.handshake_error = 0;
	d_Statistics2.no_answer = 0;
	d_Statistics2.no_enq_from_host = 0;
	d_Statistics2.no_response_from_host = 0;
	d_Statistics2.no_tone = 0;
	d_Statistics2.power_fail = 0;
	d_Statistics2.successful = 0;
	d_Statistics2.total_time = 0;

	total_batch_time = 0;

	memset(amount_surcharge, '0', 13);      /* ends with null character */
	amount_surcharge[13] = 0;
		
	memset(&stGSMSTATIST,0,sizeof(stGSMSTATIST));
	
/*****  01_18 15/08/2000 ************/
	memset(param_ver_no,'0',3);
	param_ver_no[3] = 0;

	memset(batch_no_c,'0',3);
	batch_no_c[3] = 0;

	batch_no = 0;

	device_serial_no = '0';

	memset(seq_no_c, '0' ,4);
	seq_no_c[4] = 0;

	/*batch_no=001;*/

	/*strcpy(param_ver_no,"000");*/

/************** END  ***************/

	/* genius */
	/* 04_00 genius */
	memset(genius_seq_no_c, '0' ,4);
	genius_seq_no_c[4] = 0;
	memset(last_genius_eod_date,'0',sizeof(last_genius_eod_date));
	last_genius_eod_date[8] = 0;

/*	memset(last_genius_eod_date,0,sizeof(last_genius_eod_date));*/
	/* 04_09 default 0 a alýndý. Transaction oncesi artýrým yapýldýgýndan */
	genius_seq_no = 0;


	seq_no=0;
	
	POS_software_ver = 'B';                                 /* program_ver_no  ..... POS_software_ver   */

	strcpy(admin_password,"47326848773614");
	admin_password[14]=0;
	strcpy(operator_password,"4321");
	operator_password[4]=0;
	
	memset(last_transact_batch_no,'0',3);
	last_transact_batch_no[3] = 0;

	memset(last_transact_seq_no,'0',4);
	last_transact_seq_no[4] = 0;

	last_transact_status = '1';

	memset(last_eod_date,0,sizeof(last_eod_date));
	
	memset(net_total,'0',13);
	net_total[13] = 0;

	memset(agreement_total,'0',13);
	agreement_total[13] = 0;
	
	PINPad_status = '2';    /* silinecek. cunku statu check edilerek alýnacak */

	/* initialize the Total Counts of Amount and transactions on both card and transaction basis*/
	for (i=0 ; i<4; i++)
	{
		d_Totals_Counts[i].tran_count = 0;
		memset(d_Totals_Counts[i].tran_amount,'0',13);
		d_Totals_Counts[i].tran_amount[13] = 0;
		/* FAIZLI 02_00 26/07/2000 faizli kismi tutan meblag initialize ediliyor*/
		memset(d_Totals_Counts[i].tran_interest_amount,'0',13);
		d_Totals_Counts[i].tran_interest_amount[13] = 0;
		Kb_Read();
		Mte_Wait(50);


		for (j=0;j<CARDTYPETABLESIZE;j++)
		{
			Kb_Read();
			d_Card_Type_Table[j].transaction_occured = FALSE;
			d_Card_Type_Table[j].d_Accummulators[i].tran_count = 0;
			memset(d_Card_Type_Table[j].d_Accummulators[i].tran_amount,'0',13);
			d_Card_Type_Table[j].d_Accummulators[i].tran_amount[13]=0;
			/* FAIZLI 02_00 26/07/2000 faizli kismi tutan meblag initialize ediliyor*/
			memset(d_Card_Type_Table[j].d_Accummulators[i].tran_interest_amount,'0',13);
			d_Card_Type_Table[j].d_Accummulators[i].tran_interest_amount[13]=0;

		}
	}

/* initialization of debit_prefixes */
	
	memset(debit_prefix, 0, sizeof(debit_prefix));

	strcpy(Debit_Initial[0], "589004,454314,501774,588691,588992,601500,479207,588936,440776,*,");
	strcpy(Debit_Initial[1], "676166,589117,494314,639004,588843,639001,640000,601050,447501,*,");
	strcpy(Debit_Initial[2], "490983,601568,589318,589311,447505,676258,447502,676170,676255,*,");
	strcpy(Debit_Initial[3], "601354,440295,405919,1948,0015,101,491005,589072,425979,676284,*,");
	strcpy(Debit_Initial[4], "676283,589283,455652,676366,480998,676345,413382,589416,445988,*,");
	strcpy(Debit_Initial[5], "490808,408593,676372,589713,676401,676206,601397,676402,460346,*,");
	strcpy(Debit_Initial[6], "603005,601354,601739,603066,415515,447503,+");

	for(j = 0; j < 7 ; j++)
	{
		for ( i=0, s=Debit_Initial[j], p=Debit_Initial[j]; (*(s+1) != '+') && (*(s+1) != '*'); p=++s ) /* debit_initial dan once  & kaldirildi 09/08/2001 */
		{
			* ( s = strchr(s, ',')) = 0;
			if ((*p > '9') || (*p < '0')) break;
			strcpy(debit_prefix[j][i], p);
			i++;
		}
	}
	
	
/* Toygar EMVed Envanter Bilgileri 25.07.2002 */
	memcpy(GPOS_Type,"LIP",3);
	memcpy(GPOS_Model,"8000",4);
	GEMV_Key_Version_No = '1';
	GConfig_Version_No = '1';
/* Toygar EMVed Envanter Bilgileri 25.07.2002 */

	Cashier_Flag = 2;	/*bm YTL 13.08.2004    */
	TL_Iade == FALSE;   /*bm YTL 13.08.2004    */
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_Initialize_Variables.
 *
 * DESCRIPTION          : Initialize the parameters
 *
 * RETURN                       : none
 *
 * NOTES                        : r.i.o. & t.c.a 05/06/99
 *
 * ------------------------------------------------------------------------ */
void Trans_Initialize_Variables(void)
{

	sint i,j;
	
	seq_no = 0;     
	record_counter = 0;
	Grecord_counter = 0;
	
	memset(net_total,'0',13);
	net_total[13] = 0;

	memset(agreement_total,'0',13);
	agreement_total[13] = 0;

	/* initialize the Total Counts of Amount and transactions on both card and transaction basis*/
	for (i=0 ; i<4;i++)
	{
		d_Totals_Counts[i].tran_count = 0;
		memset(d_Totals_Counts[i].tran_amount,'0',13);
		d_Totals_Counts[i].tran_amount[13] = 0;
		/* FAIZLI 02_00 26/07/2000 faizli kismi tutan meblag initialize ediliyor*/
		memset(d_Totals_Counts[i].tran_interest_amount,'0',13);
		d_Totals_Counts[i].tran_interest_amount[13] = 0;

/* ToBeDone-Toygar */
/*
	Printer_WriteStr("merch_device_no ***2:\n");
	PrntUtil_BufAsciiHex((byte *)merch_device_no,sizeof(merch_device_no));
*/

		for (j=0;j<CARDTYPETABLESIZE;j++)
		{
			d_Card_Type_Table[j].transaction_occured = FALSE;
			d_Card_Type_Table[j].d_Accummulators[i].tran_count = 0;
			memset(d_Card_Type_Table[j].d_Accummulators[i].tran_amount,'0',13);
			d_Card_Type_Table[j].d_Accummulators[i].tran_amount[13]=0;
			/* FAIZLI 02_00 26/07/2000 faizli kismi tutan meblag initialize ediliyor*/
			memset(d_Card_Type_Table[j].d_Accummulators[i].tran_interest_amount,'0',13);
			d_Card_Type_Table[j].d_Accummulators[i].tran_interest_amount[13]=0;

		}
	}

	/* initialize statistics */

	d_Statistics1.busy = 0;
	d_Statistics1.carrier_lost = 0;
	d_Statistics1.handshake_error = 0;
	d_Statistics1.no_answer = 0;
	d_Statistics1.no_enq_from_host = 0;
	d_Statistics1.no_response_from_host = 0;
	d_Statistics1.no_tone = 0;
	d_Statistics1.power_fail = 0;
	d_Statistics1.successful = 0;
	d_Statistics1.total_time = 0;

	d_Statistics2.busy = 0;
	d_Statistics2.carrier_lost = 0;
	d_Statistics2.handshake_error = 0;
	d_Statistics2.no_answer = 0;
	d_Statistics2.no_enq_from_host = 0;
	d_Statistics2.no_response_from_host = 0;
	d_Statistics2.no_tone = 0;
	d_Statistics2.power_fail = 0;
	d_Statistics2.successful = 0;
	d_Statistics2.total_time = 0;

	total_batch_time = 0;

	ChangesPending = FALSE;
	ReversalPending = FALSE;
	IsEMVReversal = FALSE;

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_UpdateParameters.
 *
 * DESCRIPTION          : Update the parameters received from Host as prefixes
 *
 * RETURN                       : none
 *
 * NOTES                        : r.i.o. & t.c.a 05/06/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_UpdateParameters(void) 
{
	char Update_Temp[30];
	int i;
	int len;
	char temp[900];
	char location[5];
	char temp_print[25];
	int loc, index, newindex;
	char * p, * s;
/* Toygar EMVed */
	CAPublicKey aKey;
	byte aExceptionByteList[EXCEPTIONBYTELIST];
	byte aRevocationByteList[REVOCATIONBYTELIST];
	int templen;
	ApplicationParams paApp;
/*bm opt	int Status; */
	char Host_YTL;				/* bm YTL 25/06/2004 1-YTL print, 2-YTL&TL print */
	char device_date[8], device_time[6];
/*bm opt	char Tmp_date[6]; */
/*bm opt 	char Tmp_time[4]; */
	char lbaCurrentDTime[10];	/* bm YTL */
/*bm opt	char TmpCurrentDTime[10];	/* bm YTL */


/* Toygar EMVed */
/*
129	Last_4_Digit flag (1: TRUE 0:FALSE)
0431	Data to be displayed just after EOD slip.
0432	Loyalty Prefixes (Format of DATA: xxxxxx,yyyyyy,zzzzzz,………..)
   
************* 06_30 offline islemler ****************************************************************
0433    offline pos parameters (Format of DATA: faaaaaaaaaaaaaaabbccccdddd
		f: offline_flag						: 2: offline islemlere acik ve active 1: offline islemlere acik ama active degil 0: offline islemlere kapali
		a: max_offline_amount				: iþlem bazýnda max. offine tutar 
		b: max_offline_trx_no_card			: ayni kart ile ayni batch icinde yapilacak islem sayisi
		c: max_offline_trx_no_concecutive	: arka arkaya yapilabilecek max. offline islem sayisi 
											  bu sayiya ulasildiginda islem online'a force edilir
		d: offline_password					: offline password
		e: max_offline_trx_no_concecutive	: günlük max. offline iþlem sayýsý limiti

*****************************************************************************************************

0420-0429	Debit prefixes (Format of DATA: xxxxxx,yyyyyy,zzzzzz,…….)
1001-1200	EMV Keys (will be defined later)
1201-1300	EMV Config Parameters.(will be defined later)
*/
	/* received parameter length*/
	memcpy(Update_Temp, d_Param_Continue_Receive.param_data_length,3);
	Update_Temp[3]=0;
	len=atoi(Update_Temp);

	/*received memory location*/
	memcpy(location, d_Param_Continue_Receive.param_location_no, 4);
	location[4]=0;
	loc = atoi(location);
	memcpy(temp, d_Param_Continue_Receive.param_data, len);
	temp[len]=0;
	index = loc - 420;
	newindex = loc - 1311;
/*
	Printer_WriteStr("Location:\n");
	PrntUtil_BufAsciiHex((byte *)d_Param_Continue_Receive.param_location_no, 4);
	Printer_WriteStr("Data(10 bytes):\n");
	PrntUtil_BufAsciiHex((byte *)d_Param_Continue_Receive.param_data, 10);
*/	
	/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
/* bm OPT	if ( POS_Type == '0' )		/* axess pos icin yapiliyor */
	if ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
	{
		/*** XLS_PAYMENT 12/07/2001 irfan Handling LOYALTY prefixes loding to the string**/
		if (( loc == 432) || ( loc == 1317))
		{
			memset(Loyalty_Prefixes, 0, sizeof(Loyalty_Prefixes));
			memcpy(Loyalty_Prefixes, d_Param_Continue_Receive.param_data, len);
			Loyalty_Prefixes[len] = 0;
/*
			Printer_WriteStr("Loyalty_Prefixes:\n");
			PrntUtil_BufAsciiHex((byte *)Loyalty_Prefixes, strlen(Loyalty_Prefixes));
*/
			/*strcat(ATS_Prefixes,Loyalty_Prefixes);*/ /** XLS_INTEGRATION2 10/10/2001 irfan. */
			return(STAT_OK);
		}
	}

	/* v04_05 CALISMA_KOSULLARI_UPDATE 16/07/2002 */
	if(CALISMA_KOSULLARI_MODE)
	{
		/* update GRAPHIC DISPLAY irfan 24/01/2001 */
		/* Host dan gonderilen bilgi mesajlarýnýn basýlmasý. */
		if ( loc == 431)
		{
			if ( ((int)(len/24)) > 5 ) 
			{
				Mainloop_DisplayWait("431.SATIR HATALI",NO_WAIT);
				return (STAT_OK);
			}
			
			PrntUtil_Cr(2);
			for (i = 0; i< ((int)(len/24)); i++)
			{
				memset( temp_print, 0, sizeof(temp_print));
				memcpy(temp_print, temp+i*24, 24);
				if( strcmp(temp_print, "                        ") != 0 )
				{
					Slips_PrinterStr(temp_print);
					PrntUtil_Cr(1);
				}
			}
			
			PrntUtil_Cr(1);
			return(STAT_OK);
		}
	}


/************* 06_30 offline islemler ****************************************************************
0433    offline pos parameters (Format of DATA: faaaaaaaaaaaaaaabbccccdddd
		f: offline_flag						: 2: offline islemlere acik ve active 1: offline islemlere acik ama active degil 0: offline islemlere kapali
		a: max_offline_amount				: iþlem bazýnda max. offine tutar 
		b: max_offline_trx_no_card			: ayni kart ile ayni batch icinde yapilacak islem sayisi
		c: max_offline_trx_no_concecutive	: arka arkaya yapilabilecek max. offline islem sayisi 
											  bu sayiya ulasildiginda islem online'a force edilir
		d: offline_password					: offline password
		e: max_offline_trx_no_concecutive	: günlük max. offline iþlem sayýsý limiti

*****************************************************************************************************/
	if ( loc == 433) return(Offline_UpdateOfflineParameters());


	if ( loc == 129)
	{
		if ( temp[0] == '1' )
		{

			LAST_4_DIGIT_FLAG = TRUE;
		}
		else
		{

			LAST_4_DIGIT_FLAG = FALSE;
		}
		return(STAT_OK);
	}
	if ( (loc > 419) && (loc < 430) )
	{
		for (i=0; i<10; i++)	/* verde 30 yerine 10 yapýldý 24/12/2002 */
			memset(debit_prefix[index][i], 0, 7);

		for ( i=0, s=temp, p=temp; (*(s+1) != '+') && (*(s+1) != '*'); p=++s ) /* s ve p de temp den oce & kaldirildi 09/08/2001 */
		{
			* ( s = strchr(s, ',')) = 0;
			if ((*p > '9') || (*p < '0')) break;
			strcpy(debit_prefix[index][i], p);
			i++;
		}

		return(STAT_OK);
	}

	if ( (loc > 1310) && (loc < 2001) )
	{
		if (loc != 1317)
		{
			for (i=0; i<10; i++)	/* verde 30 yerine 10 yapýldý 24/12/2002 */
				memset(debit_prefix[newindex][i], 0, 7);
	
			for ( i=0, s=temp, p=temp; (*(s+1) != '+') && (*(s+1) != '*'); p=++s ) /* s ve p de temp den oce & kaldirildi 09/08/2001 */
			{
				* ( s = strchr(s, ',')) = 0;
				if ((*p > '9') || (*p < '0')) break;
				strcpy(debit_prefix[newindex][i], p);
				i++;
			}
		}
		return(STAT_OK);
	}


	if(d_Param_Continue_Receive.cont == '2')
	{
		if ((loc > 2000) && (loc < 2201))
		{
			GEMV_Key_Version_No = TempEMV_Key_Version_No;
/*
			Printer_WriteStr("Gelen GEMV_Key_Version_No:\n");                             
			PrntUtil_BufAsciiHex((byte *)&GEMV_Key_Version_No,sizeof(GEMV_Key_Version_No));
*/
		}
		if ((loc > 2201) && (loc < 2254))  
		{
			GConfig_Version_No = TempConfig_Version_No;
/*
			Printer_WriteStr("Gelen GConfig_Version_No:\n");                             
			PrntUtil_BufAsciiHex((byte *)&GConfig_Version_No,sizeof(GConfig_Version_No));
*/
		}
	}

/* Toygar EMVed */
	/* Key Locations */
/*	if ((loc > 1000) && (loc < 1201))*/
	if ((loc > 2000) && (loc < 2201))
	{
		if (!KeyDownloadStarted)
		{
			if (Files_CAPublicKeyInit() != STAT_OK) return(STAT_NOK);
			KeyDownloadStarted = TRUE;
		}
		memset(&aKey, 0, sizeof(aKey));
		Host_AsciiToBinMy(d_Param_Continue_Receive.param_data,(char *)&aKey, len);
/*		memcpy(&aKey, d_Param_Continue_Receive.param_data, len);*/
/*
		Printer_WriteStr("Key:\n");
		PrntUtil_BufAsciiHex((byte *)&aKey, sizeof(aKey));
*/
		if (!Files_SaveCAPublicKey(&aKey)) return(STAT_NOK);
	}
	
	/* Application Data Location */
	if (loc == 1201)
	{
/* Toygar - New EMV Start */
/*
		
		Host_AsciiToBinMy(d_Param_Continue_Receive.param_data,(byte*)&lsApplicationDataStruct, len);
*/
/* Toygar - New EMV End */
/*
		Printer_WriteStr("lsApplicationDataStructDownloaded:\n");
		PrntUtil_BufAsciiHex((byte *)&lsApplicationDataStruct,len);
/**/
/*		memcpy(&lsApplicationDataStruct, d_Param_Continue_Receive.param_data, sizeof(lsApplicationDataStruct));*/
		/*GConfig_Version_No = TempConfig_Version_No;*/ /* irfan 02/09/2002 yanlis yerde update ediliyor */
	}

	/* Terminal Data Location */
/*	if (loc == 1202) */
	if (loc == 2201)
	{
		Host_AsciiToBinMy(d_Param_Continue_Receive.param_data,(char *)&lsTerminalDataStruct, len);
/*		if (EMV_DEBUG_MODE) lsTerminalDataStruct.LipmanKernelOpt[7] |= opt_8_PRINT_TRANSPORT;*/
/*		
		Printer_WriteStr("lsTerminalDataStruct:\n");
		PrntUtil_BufAsciiHex((byte *)&lsTerminalDataStruct,sizeof(lsTerminalDataStruct));
*/
/*		memcpy(&lsTerminalDataStruct+18, d_Param_Continue_Receive.param_data, sizeof(lsTerminalDataStruct));*/
		/*GConfig_Version_No = TempConfig_Version_No;*/ /* irfan 02/09/2002 yanlis yerde update ediliyor */
	}

	/* Exception List Location */
	if ((loc > 1203) && (loc < 1251))
	{
		if (!ExceptionDownloadStarted)
		{
			if (Files_InitFile((char*)Exception_File,sizeof(aExceptionByteList)) != STAT_OK) return(STAT_NOK);
			ExceptionDownloadStarted = TRUE;
		}
		memset(aExceptionByteList, 0, sizeof(aExceptionByteList));
		Host_AsciiToBinMy(d_Param_Continue_Receive.param_data,(char*)aExceptionByteList, len);
/*		memcpy(aExceptionByteList, d_Param_Continue_Receive.param_data, sizeof(aExceptionByteList));*/
		if (Files_ExceptionAppend(aExceptionByteList) != STAT_OK) return(STAT_NOK);
	}

	/* Revocation List Location */
	if ((loc > 1250) && (loc < 1301))
	{
		if (!RevocationDownloadStarted)
		{
			if (Files_InitFile((char*)Revocation_File,sizeof(aRevocationByteList)) != STAT_OK) return(STAT_NOK);
			RevocationDownloadStarted = TRUE;
		}			
		memset(aRevocationByteList, 0, sizeof(aRevocationByteList));
		Host_AsciiToBinMy(d_Param_Continue_Receive.param_data,(char*)aRevocationByteList, len);
/*		memcpy(aRevocationByteList, d_Param_Continue_Receive.param_data, sizeof(aRevocationByteList));*/
		if (Files_RevocationAppend(aRevocationByteList) != STAT_OK) return(STAT_NOK);
	}
/* Toygar EMVed */

/* 05_02 Not_Certified Prefix yukleme   irfan. 16/09/2002 */
	if ((loc == 1302) || (loc == 2252))
	{
		memset(Not_Certified_Prefixes, 0, sizeof(Not_Certified_Prefixes));
		memcpy(Not_Certified_Prefixes, d_Param_Continue_Receive.param_data, len);
		Not_Certified_Prefixes[len] = 0;
		return(STAT_OK);
	}

	if (loc == 2253)
	{
		if ( d_Param_Continue_Receive.param_data[0] == '0' )
			PIN_BLOCK = TRUE;
		else
			PIN_BLOCK = FALSE;
		if ( d_Param_Continue_Receive.param_data[1] == '0' )
			SEND_EMV_FIELDCHANGES = FALSE;
		else
			SEND_EMV_FIELDCHANGES = TRUE;
/* bm YTL 25/06/2004 */
		Host_YTL = d_Param_Continue_Receive.param_data[2];	/* YTL flag */
		if ( d_Param_Continue_Receive.param_data[2] == '0' )
			memcpy( YTL_Slip, "YTL", 3 );
		else if ( d_Param_Continue_Receive.param_data[2] == '1' )
			memcpy( YTL_Slip, "TLE", 3 );
		else
			return ( STAT_NOK );

		memcpy( Host_YTL_Password, d_Param_Continue_Receive.param_data + 3,	4 );

		/** get pos date and time **/
		Utils_GetDateTime( device_date, device_time );
		lbaCurrentDTime[0] = device_date[6];
		lbaCurrentDTime[1] = device_date[7];
		lbaCurrentDTime[2] = device_date[3];
		lbaCurrentDTime[3] = device_date[4];
		lbaCurrentDTime[4] = device_date[0];
		lbaCurrentDTime[5] = device_date[1];
		lbaCurrentDTime[6] = device_time[0];
		lbaCurrentDTime[7] = device_time[1];
		lbaCurrentDTime[8] = device_time[3];
		lbaCurrentDTime[9] = device_time[4];

		/* bm YTL 29/06/2004 kontrol tarihi */
		Set_Mode = strncmp( Host_YTL_Password,"1234", 4 );

		/* Cashier_Flag 13.08.2004 */
		if ( d_Param_Continue_Receive.param_data[8] == '1' )
		{
			Cashier_Flag = 1;
			/*Slips_PrinterStr("\nCashier_Flag=1\n");  */
		}
		else if ( d_Param_Continue_Receive.param_data[8] == '2' )
		{
			Cashier_Flag= 2;
			/*Slips_PrinterStr("\nCashier_Flag=2\n"); */
		}
		else
		{
			Slips_PrinterStr("\nPAR.YUK.HATA *2253*B9\n"); 
			return ( STAT_NOK );
		}
		/* Cashier_Flag 13.08.2004 */

		return(STAT_OK);
	}

	/* Terminal Application List Location */
	if ((loc > 2201) && (loc < 2251))  
	{
		if (!AppListDownloadStarted)
		{
			AppListDownloadStarted = TRUE;
			if (Files_EMVAppInit() != STAT_OK) return STAT_NOK;
		}
		memset(&paApp,0,sizeof(paApp));
		Host_AsciiToBinMy(d_Param_Continue_Receive.param_data,(char *)&paApp, len);
		if (memcmp((byte *)paApp.AID, XLSAID, 9)) Files_SaveEMVAppParamIn(&paApp);
		else if ((POS_Type == '4') || (POS_Type == '6')) Files_SaveEMVAppParamIn( &paApp );
		/* 
		if (EMV_DEBUG_MODE)
		{
			Printer_WriteStr("AID:\n");
			PrntUtil_BufAsciiHex((byte *)paApp.AID,16);
			Printer_WriteStr("TAC_Default:\n");
			PrntUtil_BufAsciiHex((byte *)paApp.TAC_Default,5);
			Printer_WriteStr("TAC_Denial:\n");
			PrntUtil_BufAsciiHex((byte *)paApp.TAC_Denial,5);
			Printer_WriteStr("TAC_Online:\n");
			PrntUtil_BufAsciiHex((byte *)paApp.TAC_Online,5);
		}
		*/
/*
		if (DEBUG_MODE)
		{
			Printer_WriteStr("Len:\n");
			PrntUtil_BufAsciiHex((byte *)&len,sizeof(len));
			Printer_WriteStr("Data:\n");
			PrntUtil_BufAsciiHex((byte *)&d_Param_Continue_Receive.param_data,len);
			Printer_WriteStr("lsEMVAPPStruct:\n");
			PrntUtil_BufAsciiHex((byte *)&fpAppList,sizeof(fpAppList));
		}
*/
	}

	if ( ( loc >= 8000 ) && ( loc < 8050 ) ) /*bm OFF 21.10.2004*/
	{
		if ( Offline_BIN_Add_TABLE(  ) != STAT_OK )
			return ( STAT_NOK );
	}
	return (STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_InputCardNo
 *
 * DESCRIPTION          : Read the card no from the keypadRead the card 
 *
 * RETURN                       :
 *
 * NOTES                        :
 *
 * ------------------------------------------------------------------------ */
boolean Trans_InputCardNo(void)
{
	int i, int_exp_date;
	boolean read_cvv_code, read_exp_date;
	char Buffer[LEN_GETKEYBOARD];
	char temp_exp_date[5];
	byte Len;

		/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* XLS_PAYMENT 24/07/2001 irfan */
	char temp_first_6_digit[7];


	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));


	/* ***** GET THE  CARD NO **********/
	if(!Mainloop_GetKeyboardInput("KART NO ?",Buffer,13,19,
				       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, FALSE))
	{
		EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
		EMVAxess = FALSE;
			return(FALSE);
	}
    else
	{
		memcpy(card_no,Buffer,Len);
		card_no[Len]=0;

	}       

	/** XLS 04_02 20/12/2001 irfan. xls payment switch **/
	/* bm OPT if ( POS_Type == '0' )        /* axess pos icin yapiliyor */
	if ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )	/* axess pos icin yapiliyor */
	{
		/* XLS_PAYMENT 24/07/2001 irfan */
		/* Check for Loyalty_Prefixes*/
		memcpy(temp_first_6_digit,card_no,6);
		temp_first_6_digit[6] = 0;

		if(!CORRUPTED_CHIP_TRANSACTION_FLAG)
		{
			if( strstr(Loyalty_Prefixes, temp_first_6_digit) != NULL) 
			{
				Mainloop_DisplayWait("   SMART KART   OKUYUCU KULLANIN",1);	
				return (FALSE);
			}
		}
	}

		/* LuhnCheck Control.*/
	if (!Utils_LuhnCheck())
	{
		Mainloop_DisplayWait(" GE€ERS˜Z KART  ",2);     /*DEGISECEK_MESAJ*/
		return (FALSE);
	}


	/* ***** GET THE  EXP DATE **********/
	
	read_exp_date = TRUE;
	i=0;
	do
	{

		if(!Mainloop_GetKeyboardInput("SON KULL. TARIH?",Buffer,4,4,    /*DEGISECEK_MESAJ*/
					       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, FALSE))
		{
			EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
			EMVAxess = FALSE;
				return(FALSE);
		}
		else
		{

			
			memcpy(temp_exp_date,Buffer,Len);
			temp_exp_date[Len]=0;
		
			/*check boundary conditions for date*/
			int_exp_date=atoi(temp_exp_date);
			if      ((int_exp_date >= 100) && (int_exp_date <= 1299))
			{       

				read_exp_date=FALSE;
			}

		}

		i++;

	}while(i<3 && read_exp_date);

	if(read_exp_date) 
		return(FALSE);

	exp_date[0]=temp_exp_date[2];
	exp_date[1]=temp_exp_date[3];
	exp_date[2]=temp_exp_date[0];
	exp_date[3]=temp_exp_date[1];
	exp_date[4]=0;

		
	/* ***** GET THE  CVV **********/
	read_cvv_code = TRUE;
	i=0;
	do
	{
		
		if(!Mainloop_GetKeyboardInput("CVV CODE ?",Buffer,0,3,
					       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
		{
			EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
			EMVAxess = FALSE;
				return(FALSE);
		}
		else
		{
			
			if (Len == 0) 
			{
				strcpy(cvv_2,"000");
				read_cvv_code = FALSE;
			}

			if (Len == 3)
			{
				memcpy(cvv_2,Buffer,Len);
				read_cvv_code = FALSE;
			}
		}

		i++;

	}while(i<3 && read_cvv_code);
	if(read_cvv_code) 
		return(FALSE);

	cvv_2[3]=0;
	return(TRUE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_FillTransactionSend
 *
 * DESCRIPTION          : 
 *
 * RETURN                       :
 *
 * NOTES                        :
 *
 * REVISED ON           ; 06/05/99 BY: REFIK IRFAN OZTURK
 * ------------------------------------------------------------------------ */
void Trans_FillTransactionSend( PTStruct * fpPTStruct, boolean IsEMV,
								boolean Kampanya )
{
	char first_6_digit_local[7]; /** XLS_PAYMENT 31/07/2001 irfan **/
	byte temppsn, temppsnchar, tempbyte;
	RowData rdData;

	char device_date[8], device_time[6];
	memset( &d_Transaction_Send, ' ', sizeof( d_Transaction_Send ) );
/* bm YTL 21/06/2004 */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		d_Transaction_Send.Header_Transact.Cur_Index = 'X';
	else
		d_Transaction_Send.Header_Transact.Cur_Index = 'Y';
	d_Transaction_Send.Header_Transact.ActAppCode = POS_Type;
	d_Transaction_Send.Header_Transact.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
	memcpy( d_Transaction_Send.Header_Transact.PosType, "LIP", 3 );
	memcpy( d_Transaction_Send.Header_Transact.PosModel, "8000", 4 );
	memcpy( d_Transaction_Send.Header_Transact.PosVersion, POS_VERSION_NUMBER, 4 );
	if (Cashier_Flag == 2)  /* Kasiyer tanýmlý deðil 13.08.2004*/ 
		memset( d_Transaction_Send.Header_Transact.Cashier_Id, ' ', 16 );
	else
		memcpy( d_Transaction_Send.Header_Transact.Cashier_Id, Kasiyer_No, 16 );
	memcpy( d_Transaction_Send.Header_Transact.IMEI_ID, Kampanya_Data, 15 );
	memset( d_Transaction_Send.Header_Transact.Filler, ' ', 19 );
	Utils_GetDateTime( device_date, device_time );
	if (tran_type != RETURN)
	{
		d_Transaction_Send.Header_Transact.PosDate[0] = device_date[6];
		d_Transaction_Send.Header_Transact.PosDate[1] = device_date[7];
		d_Transaction_Send.Header_Transact.PosDate[2] = device_date[3];
		d_Transaction_Send.Header_Transact.PosDate[3] = device_date[4];
		d_Transaction_Send.Header_Transact.PosDate[4] = device_date[0];
		d_Transaction_Send.Header_Transact.PosDate[5] = device_date[1];
		d_Transaction_Send.Header_Transact.PosTime[0] = device_time[0];
		d_Transaction_Send.Header_Transact.PosTime[1] = device_time[1];
		d_Transaction_Send.Header_Transact.PosTime[2] = device_time[3];
		d_Transaction_Send.Header_Transact.PosTime[3] = device_time[4];
	} 
	else
	{	/* iade isleminde girilen tarih gönderilecek 13.08.2004 */
		d_Transaction_Send.Header_Transact.PosDate[0] = Sale_Date_Char[0];
		d_Transaction_Send.Header_Transact.PosDate[1] = Sale_Date_Char[1];
		d_Transaction_Send.Header_Transact.PosDate[2] = Sale_Date_Char[2];
		d_Transaction_Send.Header_Transact.PosDate[3] = Sale_Date_Char[3];
		d_Transaction_Send.Header_Transact.PosDate[4] = Sale_Date_Char[4];
		d_Transaction_Send.Header_Transact.PosDate[5] = Sale_Date_Char[5];
		if (TL_Iade)
		{
			d_Transaction_Send.Header_Transact.PosTime[0] = '1';
			d_Transaction_Send.Header_Transact.PosTime[1] = '1';
			d_Transaction_Send.Header_Transact.PosTime[2] = '1';
			d_Transaction_Send.Header_Transact.PosTime[3] = '1';
		}
		else
		{
			d_Transaction_Send.Header_Transact.PosTime[0] = '0';
			d_Transaction_Send.Header_Transact.PosTime[1] = '0';
			d_Transaction_Send.Header_Transact.PosTime[2] = '0';
			d_Transaction_Send.Header_Transact.PosTime[3] = '0';
		}
	}

	
	/* FILL  THE  HEADER  PART */
	if ((IsEMV)/* MTK 25.06 [&& (POS_Type == '1')] */) d_Transaction_Send.Header_Transact.MSG_INDICATOR = 'E';
	else d_Transaction_Send.Header_Transact.MSG_INDICATOR='N';

	d_Transaction_Send.Header_Transact.MSG_TYPE='2';
	d_Transaction_Send.Header_Transact.POS_software_ver = POS_software_ver;
	memcpy(d_Transaction_Send.Header_Transact.merch_device_no , merch_device_no , 8);
	d_Transaction_Send.Header_Transact.device_serial_no = device_serial_no;
	
	/** XLS_PAYMENT 30/07/2001 irfan loyalty install. tran. with interest **/
	/* Check for Loyalty_Prefixes*/
	memcpy(first_6_digit_local,card_no,6);
	first_6_digit_local[6] = 0;

	/* 06_31 15/04/2003 Co-Brand taksitli faizli islemleri degisiklikleri */
	/* Taksitli faizli islem tiplerinin fill edilmesi icin prefix ve pos tipi kontrolleri kaldirildi*/
	/*if( (strstr(Loyalty_Prefixes, first_6_digit_local) != NULL) && 
		ATS_PERMISSION_FLAG && (POS_Type == '0') && /** XLS 04_02 20/12/2001 irfan. xls payment switch **/
	/*	IS_LOYALTY_INSTALLMENT_INTEREST && 
		(ATS_Count !=1) )*/
	if( ATS_PERMISSION_FLAG && IS_LOYALTY_INSTALLMENT_INTEREST && (ATS_Count !=1) )
	{
		if(tran_type == SALE)
		{	/** loyalty installment SALE transaction with interest **/
			d_Transaction_Send.Header_Transact.transaction_type = 6 + '0';
		}
		
		if(tran_type == RETURN)
		{  /** loyalty installment RETURN transaction with interest **/
			d_Transaction_Send.Header_Transact.transaction_type = 7 + '0'; 
		}

		if(tran_type == MANUEL)
		{  /** loyalty installment MANUEL SALE transaction with interest **/
			d_Transaction_Send.Header_Transact.transaction_type = 8 + '0'; 
		}

	}
	else
	{
		d_Transaction_Send.Header_Transact.transaction_type = tran_type + '0';
	}

	IS_LOYALTY_INSTALLMENT_INTEREST = FALSE; /*@bm 24.08.2004 */

	/*FILL THE DETAIL */
	Utils_IToA(batch_no,batch_no_c);
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(d_Transaction_Send.batch_no, batch_no_c,3);
/* Toygar EMVed */
	seq_no++;
/* Toygar EMVed */
	Utils_IToA(seq_no,seq_no_c);
	Utils_LeftPad(seq_no_c, '0', 4);
/*MKMK*/
	/* 04_00 genius */
	Utils_IToA(genius_seq_no,genius_seq_no_c);
	Utils_LeftPad(genius_seq_no_c, '0', 4);

	memcpy(d_Transaction_Send.transaction_seq_no, seq_no_c,4);

	memset(d_Transaction_Send.card_info, ' ', sizeof(d_Transaction_Send.card_info));
	d_Transaction_Send.input_type = input_type;
	
	/** XLS_PAYMENT  18/07/2001 irfan. fill chip data */
	if ((d_Transaction_Send.input_type == INPUT_FROM_READER) || (d_Transaction_Send.input_type == INPUT_FROM_READER_90) ||
		 (d_Transaction_Send.input_type == INPUT_FROM_READER_91) || (d_Transaction_Send.input_type == INPUT_FROM_READER_92) ||
		 (d_Transaction_Send.input_type == INPUT_FROM_READER_22))
	{
		memcpy(d_Transaction_Send.card_info,card_info,strlen(card_info));
	}
	else if (d_Transaction_Send.input_type == INPUT_FROM_CHIPCARD)
	{
/* Toygar EMVed */
		if (IsEMV) memcpy(d_Transaction_Send.card_info, card_info, 40);
		else
		{
			memcpy(d_Transaction_Send.card_info, card_no,strlen(card_no));
			d_Transaction_Send.card_info[strlen(card_no)] = '=';
			memcpy(&d_Transaction_Send.card_info[strlen(card_no)+1], remained_chip_data,strlen(remained_chip_data));
		}
/* Toygar EMVed */
	}
	else /* manual entry */
	{
			memcpy(d_Transaction_Send.card_info, card_no,strlen(card_no));
			memcpy(&d_Transaction_Send.card_info[19], exp_date,4);
			memcpy(&d_Transaction_Send.card_info[23], cvv_2,3);
	}

	if (IsEMV)
	{
		if (OnlinePinEntry) memcpy(d_Transaction_Send.encrypted_PIN, encrypted_PIN, 16);
		else memset(d_Transaction_Send.encrypted_PIN, '0', sizeof(d_Transaction_Send.encrypted_PIN));
	}
	else if (!DEBIT_FLAG)
	{
		memset(d_Transaction_Send.encrypted_PIN, '0', sizeof(d_Transaction_Send.encrypted_PIN));
	}
	else
	{
		memcpy(d_Transaction_Send.encrypted_PIN, encrypted_PIN, 16);
	}

	memset(d_Transaction_Send.approval_code, ' ', sizeof(d_Transaction_Send.approval_code));

	if (IsEMV)
	{
		/*	Printer_WriteStr("tagPAN_SEQUENCE_NUMBER\n");*/
		if (EmvDb_Find(&rdData, tagPAN_SEQUENCE_NUMBER))
		{
			temppsn = rdData.value[0];
			if (temppsn < 10) temppsnchar = '0' + temppsn;
			else temppsnchar = 'A' + temppsn - 10;
			d_Transaction_Send.last_transact_status = temppsnchar;
/*
			if (EMV_DEBUG_MODE)
			{
				Printer_WriteStr("PSN Debug Values:\n");
				PrntUtil_BufAsciiHex((byte *)&temppsn, 1);
				PrntUtil_BufAsciiHex((byte *)&temppsnchar, 1);
			}
*/
		}
	}
	if (tran_type == MANUEL)
	{
		memcpy(d_Transaction_Send.approval_code, approval_code, sizeof(d_Transaction_Send.approval_code));
	}
	/* FAIZLI 02_00 28/07/2000 Eðer iade ise satýþ tarihi approval code sahasýna move edilir*/
	else if (tran_type == RETURN)
	{
/*
		if(ATS_Count > 1)
		{
			memcpy(d_Transaction_Send.approval_code, Sale_Date_Char, 6);
		}
		else
		{
*/			memcpy(d_Transaction_Send.approval_code, approval_code, sizeof(d_Transaction_Send.approval_code));
/*		}*/
	}

	if (tran_type != BALANCE)
	{
		if (TL_Iade) memcpy( d_Transaction_Send.amount_surcharge, amount_surcharge_iade, 13 );
		else memcpy( d_Transaction_Send.amount_surcharge, amount_surcharge, 13 );
/*
		Printer_WriteStr( "d_Transaction_Send.amount_surcharge:\n" );
		PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.amount_surcharge,13);
*/
	}
	else
	{
		memset(d_Transaction_Send.amount_surcharge, '0', 13);
	}

	/* @bm 24.09.2004 */
	memset(fpPTStruct->Amount, 0, sizeof(fpPTStruct->Amount));
	memcpy(fpPTStruct->Amount, amount_surcharge, 13);

	memcpy( d_Transaction_Send.last_transact_batch_no, last_transact_batch_no,
			3 );
	memcpy( d_Transaction_Send.last_transact_seq_no, last_transact_seq_no,
			4 );
	if ( ( !IsEMV ) || ( EMVAxess ) )
		d_Transaction_Send.last_transact_status = last_transact_status;

	d_Transaction_Send.PINPad_status = PINPad_status;       

	/* copy agreement total */
	memcpy(d_Transaction_Send.agreement_total, agreement_total, 13);

	/* copy the ATS_Count */
	memcpy(d_Transaction_Send.ATS_Count, ATS_Count_c, 2);
/* Toygar EMVed */	/* 06_05 indentation'a uygun duzeltildi */
	if (IsEMV)
	{
		memcpy(&d_Transaction_Send.EMVFields, fpPTStruct->Field55, sizeof(d_Transaction_Send.EMVFields));
		if (POSEntryFlag == -1) 
		{
			memcpy(d_Transaction_Send.EMVFields.PEM,"0091",4);
			d_Transaction_Send.input_type = INPUT_FROM_READER_91;
		}
		else if (POSEntryFlag == 1) 
		{
			memcpy(d_Transaction_Send.EMVFields.PEM,"0090",4);
			d_Transaction_Send.input_type = INPUT_FROM_READER_90;
		}
		else if (POSEntryFlag == 2)
		{
			memcpy(d_Transaction_Send.EMVFields.PEM,"0092",4);
			d_Transaction_Send.input_type = INPUT_FROM_READER_92;
		}
		else 
		{
			memcpy(d_Transaction_Send.EMVFields.PEM,"0022",4);
			d_Transaction_Send.input_type = INPUT_FROM_READER_22;
		}
	}
	else
		{
		if (input_type == INPUT_FROM_KEYPAD) 
		{
			d_Transaction_Send.input_type = '1';
		}
		else if (input_type == INPUT_FROM_READER)
		{
			d_Transaction_Send.input_type = '2';
		}
		else if (input_type == INPUT_FROM_CHIPCARD)
		{
			d_Transaction_Send.input_type = '3';
		}
	}

	POSEntryFlag = 0;
	POSEntryFlagUsed = TRUE;
	BadICC = 0;
	
	if (IsEMV)
	{
		int k;
		RowData myrdData;
		char TempSpace[20];
/*		
		Printer_WriteStr("Interface Device Serial Number :\n");
		PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.IDSN, sizeof(d_Transaction_Send.EMVFields.IDSN));
		memcpy((byte *)d_Transaction_Send.EMVFields.IDSN, "3139393131383333", 16);
*/
		Host_AsciiToBinMy(d_Transaction_Send.EMVFields.IDSN,d_Transaction_Send.EMVFields.IDSN, sizeof(d_Transaction_Send.EMVFields.IDSN));
		for(k = sizeof(d_Transaction_Send.EMVFields.IDSN)/2; k < sizeof(d_Transaction_Send.EMVFields.IDSN); k++)
			d_Transaction_Send.EMVFields.IDSN[k] = ' ';
		if (EmvDb_Find(&myrdData, tagTERML_COUNTRY_CODE))
		{
/*
			Printer_WriteStr("tagTERML_COUNTRY_CODE:\n");
			PrntUtil_BufAsciiHex(myrdData.value, myrdData.length);
			Printer_WriteStr("\n");
*/
			EmvIF_BinToAscii(myrdData.value, d_Transaction_Send.EMVFields.IDSN+8, myrdData.length, 4, FALSE);
			memcpy(d_Transaction_Send.EMVFields.IDSN+12, "    ", 4);
		}
		if (EmvDb_Find(&myrdData, tagTRAN_CURR_CODE))
		{
/*
			Printer_WriteStr("tagTRAN_CURR_CODE:\n");
			PrntUtil_BufAsciiHex(myrdData.value, myrdData.length);
			Printer_WriteStr("\n");
*/
			EmvIF_BinToAscii( myrdData.value, d_Transaction_Send.EMVFields.IDSN + 12, myrdData.length, 4,
							  FALSE );
		}
		tempbyte = d_Transaction_Send.EMVFields.CID[0];
		if ( EmvDb_Find( &myrdData, tagAPPL_CRYPTOGRAM ) )
		{
/*
			Printer_WriteStr("tagAPPL_CRYPTOGRAM:\n");
			PrntUtil_BufAsciiHex(myrdData.value, myrdData.length);
			Printer_WriteStr("\n");
*/
			EmvIF_BinToAscii( myrdData.value, d_Transaction_Send.EMVFields.AC, myrdData.length, sizeof(d_Transaction_Send.EMVFields.AC),
							  FALSE );
		}
		d_Transaction_Send.EMVFields.CID[0] = tempbyte;
/*
		if ( EmvDb_Find( &myrdData, tagCRYPTOGRAM_INFO ) )
		{
			Printer_WriteStr("tagCRYPTOGRAM_INFO:\n");
			PrntUtil_BufAsciiHex(myrdData.value, myrdData.length);
			Printer_WriteStr("\n");
			EmvIF_BinToAscii( myrdData.value, d_Transaction_Send.EMVFields.CID, myrdData.length, sizeof(d_Transaction_Send.EMVFields.CID),
							  FALSE );
	}
*/	}
	memcpy(d_Transaction_Send.EMVFields.ARC, "  ",2);
	
/*
		if ((DEBUG_MODE) && (IsEMV))
		{
			Printer_WriteStr("POS Entry Mode :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.PEM,sizeof(d_Transaction_Send.EMVFields.PEM));
			Printer_WriteStr("Application Interchange Profile :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.AIP,sizeof(d_Transaction_Send.EMVFields.AIP));
			Printer_WriteStr("Dedicated File Name :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.DFN,sizeof(d_Transaction_Send.EMVFields.DFN));
			Printer_WriteStr("Terminal Verification Result :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.TVR,sizeof(d_Transaction_Send.EMVFields.TVR));
			Printer_WriteStr("Issuer Application Data :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.IAD,sizeof(d_Transaction_Send.EMVFields.IAD));
			Printer_WriteStr("Interface Device Serial Number :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.IDSN,sizeof(d_Transaction_Send.EMVFields.IDSN));
			Printer_WriteStr("Application Cyrptogram :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.AC,sizeof(d_Transaction_Send.EMVFields.AC));
			Printer_WriteStr("Cyrptogram Information Data :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.CID,sizeof(d_Transaction_Send.EMVFields.CID));
			Printer_WriteStr("Terminal Capabilities :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.TC,sizeof(d_Transaction_Send.EMVFields.TC));
			Printer_WriteStr("CVM Result :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.CVMR,sizeof(d_Transaction_Send.EMVFields.CVMR));
			Printer_WriteStr("Terminal Type :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.TT,sizeof(d_Transaction_Send.EMVFields.TT));
			Printer_WriteStr("Application Transaction Counter :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.ATC,sizeof(d_Transaction_Send.EMVFields.ATC));
			Printer_WriteStr("Unpredictable Number :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.UN,sizeof(d_Transaction_Send.EMVFields.UN));
			Printer_WriteStr("Transaction Sequence Number :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.TSN,sizeof(d_Transaction_Send.EMVFields.TSN));
			Printer_WriteStr("Transaction Category Code :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.TCC,sizeof(d_Transaction_Send.EMVFields.TCC));
			Printer_WriteStr("Application Response Code :\n");
			PrntUtil_BufAsciiHex((byte *)d_Transaction_Send.EMVFields.ARC,sizeof(d_Transaction_Send.EMVFields.ARC));
		};
*/

/* Toygar EMVed */
/*
	Printer_WriteStr("d_Transaction_Send:\n");
	PrntUtil_BufAsciiHex((byte *)&d_Transaction_Send,sizeof(d_Transaction_Send));
*/
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_AnalyseSaleTransactionReceive.
 *
 * DESCRIPTION          : Analyze the receive message
 *
 * RETURN                       : ret_code
 *
 * NOTES                        : r.i.o & t.c.a 19/05/99
 *
 * ------------------------------------------------------------------------ */
int Trans_AnalyseSaleTransactionReceive(void)
{
	char msg[40];
//	char MessageSecondLine[16];
	short int ret;

	/* UPDATE 01_16 30/10/99 Bakýye sormada hatalý mesaj alýnmasý durumu ýcýn tanýmlandý */
	char balance_msg[21];

	if (d_Transaction_Receive.Header_Transact.MSG_TYPE != '3')
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("MESAJ T˜P˜ FARKLI");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MESAJ T˜P˜ FARKLI",1);
		return (UNEXPECTED_MESSAGE);
	}


	/* UPDATE 01_16 24/10/99. Dönen mesaj da ortak field'lar kontrol ediliyor */
	/* bozuk gelen ama LRC den gecen mesaj larda bozuk mesajý sense edebilmek icin yapýlýyor*/
	if(Trans_CheckReturnMessage() != STAT_OK)
	{
		return(UNEXPECTED_MESSAGE);
	}
	
//	memset(MessageSecondLine, 0, sizeof(MessageSecondLine));
	if (!memcmp(d_Transaction_Receive.confirmation, "000", 3))
	{
//		memcpy(d_Transaction_Receive.message,"ONAY KODU XXXXXX",16);
		ret = APPROVED_MESSAGE;
	}
	else if (!memcmp(d_Transaction_Receive.confirmation, "008", 3))
	{
		memcpy(d_Transaction_Receive.message,"KIMLIK  KONTROLU",16);
//		memcpy(MessageSecondLine,"ONAY KODU XXXXXX",16);
		ret = APPROVED_MESSAGE;
	}
	else if (!memcmp(d_Transaction_Receive.confirmation, "011", 3))
	{
//		memcpy(d_Transaction_Receive.message,"ONAY KODU XXXXXX",16);
		ret = APPROVED_MESSAGE;
	}
	else if ( ( memcmp(d_Transaction_Receive.confirmation, "156", 3) == 0 )  ||
			( memcmp(d_Transaction_Receive.confirmation, "157", 3) == 0 ) )
	{
		/* Enter PIN and resend the message*/
		ret = PIN_REQUESTED_MESSAGE;

	}
	/* 06_06 900 lu mesajlarda reversal set ediliyor */
	else if ( ( memcmp(d_Transaction_Receive.confirmation, "902", 3) == 0 )  ||
		 ( memcmp(d_Transaction_Receive.confirmation, "901", 3) == 0 ) ||
		 ( memcmp(d_Transaction_Receive.confirmation, "903", 3) == 0 ))
	{
/*		if (EMV_DEBUG_MODE) Printer_WriteStr ("901,902,903 case\n");*/
		ret = REJECTED_MESSAGE;
		if (DEBUG_MODE) Printer_WriteStr("Check #12\n");
		ReversalPending = TRUE;
/*		Trans_SendReversal();*/
	}
	else
	{
		ret = REJECTED_MESSAGE;
	}

	/* display the message from host and return*/
	memcpy(msg, d_Transaction_Receive.message, 16);
	msg[16] = 0;
/*
	memcpy(msg+16, MessageSecondLine, 16);	
	msg[32] = 0;
*/
	
	/* UPDATE 01_16 30/10/99 yeniden duzenlendý. Eðer mesaj onaylanmadý ise*/
	if( ret != APPROVED_MESSAGE)
	{
		/* Eðer þifre sorma mesajý donmuþ ise */
		if(ret == PIN_REQUESTED_MESSAGE)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait(msg,1);
		}
		else
		{       /*eðer reject edilmiþ bir mesaj ise*/
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait(msg,1);

			/* eðer iþlem bakiye sorma ise*/
			if(tran_type == BALANCE)
			{
				/*hata kodu ile birlikte hata mesajýný printer dan bas*/
				memset(balance_msg , 0 , sizeof(balance_msg));
				memcpy(balance_msg,d_Transaction_Receive.message,16);
				memcpy(balance_msg+16, d_Transaction_Receive.confirmation,3);
				balance_msg[19]=0;
				PrntUtil_Cr(3);
				Slips_PrinterStr("***HATALI BAKIYE SORMA**\n");
				Slips_PrinterStr(balance_msg);
				PrntUtil_Cr(7);

			}

		}
			
	}
	else
	{
		if (memcmp(msg,"ONAY KODU",9))
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait(msg,1);
		}
	}
	return (ret);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_UpdateCardTypeTable
 *
 * DESCRIPTION          : Updates the Card Type Table
 *
 * RETURN                       : STAT_OK, STAT_NOK
 *
 * NOTES                        : r.i.o & t.c.a 19/05/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_UpdateCardTypeTable(void)
{
	sint received_card_type, tran_index;
	char received_amount[14];
	char result_amount[14];
	char temporary_amount[13];
	/* FAIZLI 02_00 26/07/2000 Faizli için tanýmlandý. */
	boolean transaction_with_credit;

	/* FAIZLI 02_00 27/07/2000 faizli olup olmadýðý tespit ediliyor */
	memset(received_amount, '0', 13);
	if (memcmp(received_amount, amount_faiz_kismi, 13) != 0) transaction_with_credit = TRUE;
	else transaction_with_credit = FALSE;
	
	/** XLS_PAYMENT 01/08/2001 irfan **/
	/* 06_30 xls icin gecerli olan OFFLINE_TRX kaldirildi. EMV offline islemler icin ise CallFlag yeterli */
	/*if (OFFLINE_TRX || !CallFlag) received_card_type = 15;*/
	if (!CallFlag) received_card_type = (CARDTYPETABLESIZEMINUSONE);	
	else received_card_type = ((d_Transaction_Receive.card_type[0] - '0') * 10) + (d_Transaction_Receive.card_type[1] - '0');
/*	if (received_card_type > 15) received_card_type = 16;*/
/*
	Printer_WriteStr("received_card_type :\n");
	PrntUtil_BufAsciiHex((byte *)&received_card_type,sizeof(received_card_type));
*/		
	if (received_card_type > (CARDTYPETABLESIZEMINUSONE)) received_card_type = CARDTYPETABLESIZE;
	received_card_type--;
	d_Card_Type_Table[received_card_type].transaction_occured = TRUE;

	/** XLS_PAYMENT 01/08/2001 irfan **/
	/* 06_30 xls icin gecerli olan OFFLINE_TRX kaldirildi. EMV offline islemler icin ise CallFlag yeterli */
	/*if (OFFLINE_TRX || !CallFlag)*/
	if (!CallFlag)
	{
		memcpy(d_Card_Type_Table[received_card_type].long_name, "DIGER ISLEMLER    ", 18);
		d_Card_Type_Table[received_card_type].long_name[18] = 0;
		memcpy(d_Card_Type_Table[received_card_type].short_name, "DIGER ", 6);
		d_Card_Type_Table[received_card_type].short_name[6] = 0;
		memcpy(received_amount, amount_surcharge, 13);
		received_amount[13] = 0;
	}
	else
	{
		memcpy(d_Card_Type_Table[received_card_type].long_name, d_Transaction_Receive.card_type_long_name, 18);
		d_Card_Type_Table[received_card_type].long_name[18] = 0;
		memcpy(d_Card_Type_Table[received_card_type].short_name, d_Transaction_Receive.card_type_short_name, 6);
		d_Card_Type_Table[received_card_type].short_name[6] = 0;
		memcpy(received_amount, d_Transaction_Receive.amount_surcharge, 13);
		received_amount[13] = 0;
	}
	tran_index = tran_type - 1;
	if (tran_index > 3) return(STAT_NOK);
	Utils_Add_Strings(received_amount, d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_amount, result_amount);
	Utils_LeftPad(result_amount, '0', 13);
	memcpy(d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_amount, result_amount,13);
	d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_amount[13] = 0;

	/** XLS_PAYMENT 01/08/2001 irfan **/
	/* 06_30 xls icin gecerli olan OFFLINE_TRX kaldirildi. EMV offline islemler icin ise CallFlag yeterli */
	/*if (!OFFLINE_TRX || CallFlag) d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_count++;*/
	/*if (CallFlag)*/
	d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_count++;
	Utils_Add_Strings(received_amount, d_Totals_Counts[tran_index].tran_amount, result_amount);
	Utils_LeftPad(result_amount, '0', 13);
	memcpy(d_Totals_Counts[tran_index].tran_amount, result_amount,13);
	d_Totals_Counts[tran_index].tran_amount[13] = 0;

	/** XLS_PAYMENT 01/08/2001 irfan **/
	/* 06_30 xls icin gecerli olan OFFLINE_TRX kaldirildi. EMV offline islemler icin ise CallFlag yeterli */
	/*if (!OFFLINE_TRX || CallFlag) d_Totals_Counts[tran_index].tran_count++;*/
	/*if (CallFlag) */
	 d_Totals_Counts[tran_index].tran_count++;

	/* FAIZLI 02_00 26/07/2000 eðer faizli taksitli ise sadece faiz kýsmýný saklýyor */
	if (transaction_with_credit)
	{
		Utils_Add_Strings(amount_faiz_kismi, d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_interest_amount, result_amount);
		Utils_LeftPad(result_amount, '0', 13);
		memcpy(d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_interest_amount, result_amount,13);
		d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_interest_amount[13] = 0;
	}
	if ((tran_type == SALE) || (tran_type == MANUEL))
	{
		Utils_Add_Strings(agreement_total, received_amount, result_amount);
		Utils_LeftPad(result_amount, '0', 13);
		memcpy(agreement_total, result_amount, 13);
		agreement_total[13] = 0;
		if (net_total[0] == '-')
		{
			memcpy(temporary_amount, net_total+1, 12);
			temporary_amount[12] = 0;
			switch (Utils_Compare_Strings(received_amount, temporary_amount))
			{
			case 1:
				Utils_Subtract_Strings(received_amount, temporary_amount, result_amount);
				Utils_LeftPad(result_amount, '0', 13);
				memcpy(net_total, result_amount,13);
				break;
			case 0:
				memcpy(net_total,"0000000000000",13);
				break;
			case 2:
				Utils_Subtract_Strings(temporary_amount, received_amount, result_amount);
				Utils_LeftPad(result_amount, '0', 13);
				result_amount[0] = '-';
				memcpy(net_total, result_amount, 13);
			}
		}
		else
		{
			Utils_Add_Strings(net_total, received_amount, result_amount);
			Utils_LeftPad(result_amount, '0', 13);
			memcpy(net_total, result_amount, 13);
		}
		net_total[13] = 0;
	}
	else if (tran_type == RETURN)
	{
		Utils_Add_Strings(agreement_total, received_amount, result_amount);
		Utils_LeftPad(result_amount, '0', 13);
		memcpy(agreement_total, result_amount, 13);
		agreement_total[13] = 0;
		if (net_total[0] == '-')
		{
			memcpy(temporary_amount, net_total+1, 12);
			temporary_amount[12] = 0;
			Utils_Add_Strings(temporary_amount, received_amount, result_amount);
			Utils_LeftPad(result_amount, '0', 13);
			result_amount[0] = '-';
			memcpy(net_total, result_amount, 13);
		}
		else
		{
			switch (Utils_Compare_Strings(received_amount, net_total))
			{
			case 1:
				Utils_Subtract_Strings(received_amount, net_total, result_amount);
				Utils_LeftPad(result_amount, '0', 13);
				result_amount[0] = '-';
				memcpy(net_total, result_amount, 13);
				break;
			case 0:
				memcpy(net_total,"0000000000000",13);
				break;
			case 2:
				Utils_Subtract_Strings(net_total, received_amount, result_amount);
				Utils_LeftPad(result_amount, '0', 13);
				memcpy(net_total, result_amount, 13);
			}
		}
		net_total[13] = 0;
	}

	if(Files_WriteAllParams() != STAT_OK) return(STAT_NOK);

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_UpdateVoidCardTypeTable
 *
 * DESCRIPTION          : Updates the Card Type Table for Void Function
 *
 * RETURN                       : none.
 *
 * NOTES                        : r.i.o & t.c.a 19/05/99
 *
 * ------------------------------------------------------------------------ */
void Trans_UpdateVoidCardTypeTable(TransactionStruct *fpTransStruct)
{
	char temporary_amount[13];
	sint void_card_type;
	char void_amount[14];
	char result_amount[14];
	sint tran_index;
	int void_tran_type;
	/* FAIZLI 02_00 26/07/2000 faizli için tanýmlandý. */
	boolean transaction_with_credit;


	/* FAIZLI 02_00 27/07/2000 iptal edilecek kaydýn faizli kýsmý alýnýyor */
	memcpy(amount_faiz_kismi, fpTransStruct->amount_faiz, 13);
	amount_faiz_kismi[13] = 0;

	/* FAIZLI 02_00 27/07/2000 Kaydýn faizli olup olmadýðý tespit ediliyor */
	memset(void_amount, '0', 13);
	if (memcmp(void_amount, amount_faiz_kismi, 13) != 0)
	{
		 transaction_with_credit = TRUE;
		 FAIZLI_FLAG = TRUE;
	}
	else
	{
		transaction_with_credit = FALSE;
		FAIZLI_FLAG = FALSE;
	}


	void_card_type = ((fpTransStruct->card_type[0] - '0') * 10) +
			  (fpTransStruct->card_type[1] - '0');

/*	if (void_card_type > 15) void_card_type = 16;*/
	if (void_card_type > (CARDTYPETABLESIZEMINUSONE)) void_card_type = CARDTYPETABLESIZE;

	void_card_type--;

	memcpy(void_amount, fpTransStruct->amount_surcharge, 13);
	void_amount[13] = 0;

	tran_index = (fpTransStruct->tran_type - '0') - 1;

	if ((Utils_Compare_Strings(d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_amount, void_amount) != 2) &&
	    (d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_count > 0))
	{
		Utils_Subtract_Strings(d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_amount, void_amount, result_amount);

		Utils_LeftPad(result_amount, '0', 13);
		memcpy(d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_amount, result_amount, 13);
		d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_amount[13] = 0;
		d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_count--;

		Utils_Subtract_Strings(d_Totals_Counts[tran_index].tran_amount, void_amount, result_amount);

		Utils_LeftPad(result_amount, '0', 13);
		memcpy(d_Totals_Counts[tran_index].tran_amount, result_amount,13);
		d_Totals_Counts[tran_index].tran_amount[13] = 0;
		d_Totals_Counts[tran_index].tran_count--;

		/* FAIZLI 02_00 26/07/2000 faizli kýsým ýcýn yazýldý*/
		if (transaction_with_credit)
		{
			Utils_Subtract_Strings(d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_interest_amount, amount_faiz_kismi, result_amount);

			Utils_LeftPad(result_amount, '0', 13);
			memcpy(d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_interest_amount, result_amount,13);
			d_Card_Type_Table[void_card_type].d_Accummulators[tran_index].tran_interest_amount[13] = 0;
		}


		void_tran_type = fpTransStruct->tran_type - '0';

		if ((void_tran_type == SALE) || (void_tran_type == MANUEL))
		{
		
			Utils_Subtract_Strings(agreement_total, void_amount, result_amount);
			Utils_LeftPad(result_amount, '0', 13);
			memcpy(agreement_total, result_amount, 13);
			agreement_total[13] = 0;

			if (net_total[0] == '-')
			{
				memcpy(temporary_amount, net_total+1, 12);
				temporary_amount[12] = 0;

				Utils_Add_Strings(temporary_amount, void_amount, result_amount);
				Utils_LeftPad(result_amount, '0', 13);
				result_amount[0] = '-';
				memcpy(net_total, result_amount, 13);
			}
			else
			{
				switch (Utils_Compare_Strings(net_total, void_amount))
				{
				case 1:
					Utils_Subtract_Strings(net_total, void_amount, result_amount);
					Utils_LeftPad(result_amount, '0', 13);
					memcpy(net_total, result_amount, 13);
					break;
				case 0:
					memcpy(net_total,"0000000000000",13);
					break;
				case 2:
					Utils_Subtract_Strings(void_amount, net_total, result_amount);
					Utils_LeftPad(result_amount, '0', 13);
					result_amount[0] = '-';
					memcpy(net_total, result_amount, 13);
				}
			}
			net_total[13] = 0;
		}
		else if (void_tran_type == RETURN)
		{
			Utils_Subtract_Strings(agreement_total, void_amount, result_amount);
			Utils_LeftPad(result_amount, '0', 13);
			memcpy(agreement_total, result_amount, 13);
			agreement_total[13] = 0;

			if (net_total[0] == '-')
			{
				memcpy(temporary_amount, net_total+1, 12);
				temporary_amount[12] = 0;

				switch (Utils_Compare_Strings(temporary_amount, void_amount))
				{
				case 1:
					Utils_Subtract_Strings(temporary_amount, void_amount, result_amount);
					Utils_LeftPad(result_amount, '0', 13);
					result_amount[0] = '-';
					memcpy(net_total, result_amount, 13);
					break;
				case 0:
					memcpy(net_total,"0000000000000",13);
					break;
				case 2:
					Utils_Subtract_Strings(void_amount, temporary_amount, result_amount);
					Utils_LeftPad(result_amount, '0', 13);
					memcpy(net_total, result_amount, 13);
				}
			}
			else
			{
				Utils_Add_Strings(net_total, void_amount, result_amount);
				Utils_LeftPad(result_amount, '0', 13);
				memcpy(net_total, result_amount, 13);

			}
			net_total[13] = 0;
		}

	}

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_Update_Cancel
 *
 * DESCRIPTION          : Sends void message, receives the response and analyzes it
 *
 * RETURN                       : none.
 *
 * NOTES                        : r.i.o & t.c.a 19/05/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_Update_Cancel( TransactionStruct * fpTransStruct, boolean IsEMV,
						  boolean Kampanya )
{
	boolean void_msg_OK;
	byte void_ret_code;
/* 06_01 RECONCILATION irfan 20/09/2002 */
	char lbSend_Buffer[900];
	char XLS_Cardno_local[20];	
	char faiz_amount_global[14];
	char result_amount_local[14];	/** 06_00 irfan **/
	char Concilation_Data_Cancel[900];				/** 06_00 irfan gerekmiyor. **/
	byte xls_ret_local;
	char xls_temp_local[100];
	char local_amount_surcharge_kismi[14];			/* 07_15 25/11/2002 */
	char device_date[8], device_time[6];



	void_msg_OK = FALSE;

/* bm YTL 02/07/2004 */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		d_Void_Send.Header_Void.Cur_Index = 'X';
	else
		d_Void_Send.Header_Void.Cur_Index = 'Y';
	d_Void_Send.Header_Void.ActAppCode = POS_Type;
	d_Void_Send.Header_Void.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
	memcpy( d_Void_Send.Header_Void.PosType, "LIP", 3 );
	memcpy( d_Void_Send.Header_Void.PosModel, "8000", 4 );
	memcpy( d_Void_Send.Header_Void.PosVersion, POS_VERSION_NUMBER, 4 );
	if (Cashier_Flag == 2)  /* Kasiyer tanýmlý deðil 13.08.2004*/ 
		memset( d_Void_Send.Header_Void.Cashier_Id, ' ', 16 );
	else
		memcpy( d_Void_Send.Header_Void.Cashier_Id, Kasiyer_No, 16 );
	memcpy( d_Void_Send.Header_Void.IMEI_ID, Kampanya_Data, 15 );
	memset( d_Void_Send.Header_Void.Filler, ' ', 19 );

	Utils_GetDateTime( device_date, device_time );
	d_Void_Send.Header_Void.PosDate[0] = device_date[6];
	d_Void_Send.Header_Void.PosDate[1] = device_date[7];
	d_Void_Send.Header_Void.PosDate[2] = device_date[3];
	d_Void_Send.Header_Void.PosDate[3] = device_date[4];
	d_Void_Send.Header_Void.PosDate[4] = device_date[0];
	d_Void_Send.Header_Void.PosDate[5] = device_date[1];
	d_Void_Send.Header_Void.PosTime[0] = device_time[0];
	d_Void_Send.Header_Void.PosTime[1] = device_time[1];
	d_Void_Send.Header_Void.PosTime[2] = device_time[3];
	d_Void_Send.Header_Void.PosTime[3] = device_time[4];

/* bm YTL */

	if ( IsEMV )
		d_Void_Send.Header_Void.MSG_INDICATOR = 'E';
	else
		d_Void_Send.Header_Void.MSG_INDICATOR = 'N';
	/******************** FILL  HEADER ********************/
	d_Void_Send.Header_Void.MSG_TYPE='4';
	d_Void_Send.Header_Void.POS_software_ver = POS_software_ver;
	memcpy(d_Void_Send.Header_Void.merch_device_no , merch_device_no , 8);
	d_Void_Send.Header_Void.device_serial_no = device_serial_no;
	d_Void_Send.Header_Void.transaction_type = fpTransStruct->tran_type;

	memcpy(v_approval_code, fpTransStruct->approval_code, 6);
	v_approval_code[6] = 0;

	memcpy(v_amount_surcharge, fpTransStruct->amount_surcharge, 13);
	v_amount_surcharge[13] = 0;
	
	memcpy(v_seq_no_c, fpTransStruct->seq_no, 4);
	v_seq_no_c[4] = 0;

	/** XLS_04_02 30/11/2001. irfan. taksit sayýsý iptal iþleminde batch'den alýnmalý **/
	ATS_Count = ((fpTransStruct->installment_number[0] - '0')*10) + 
				(fpTransStruct->installment_number[1] - '0');
	Utils_IToA(ATS_Count, ATS_Count_c);
	Utils_LeftPad(ATS_Count_c, '0', 2);
	ATS_Count_c[2] = 0;

	/******************** FILL DETAIL ********************/
	Utils_IToA(batch_no,batch_no_c);        
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(d_Void_Send.batch_no, batch_no_c,3);

	memcpy(d_Void_Send.transaction_seq_no, fpTransStruct->seq_no, 4);
	memcpy(d_Void_Send.approval_code, fpTransStruct->approval_code, 6);
	memcpy(d_Void_Send.void_code, "02", 2);

	
	/* Toygar EMVed */
/*	memset(lbSend_Buffer, 0, 256);*/
	memset( lbSend_Buffer, 0, 900 );
/* Toygar EMVed */
	memcpy (lbSend_Buffer,&d_Void_Send,sizeof(d_Void_Send));

	/* verde . iptal islemi akbank kurulusunda calismiyordur. Cunku xls modulu cagriliyordu */
	/** Call XLS module to get modified (purchase amount) amount **/
	if ( (XLS_Check_Initialization_State() == XLS_OPERATIVE) &&
		 ( ( !IsEMV ) || ( EMVAxess ) ) && ( LOYALTY_PERMISSION_FLAG ) &&
/*bm OPT ( POS_Type == '0' ) &&	  /** XLS 04_02 20/12/2001 irfan. xls payment switch **/
		 ( ( POS_Type == '4' ) || ( POS_Type == '6' ) )
		 && ( ( tran_type == SALE ) || ( tran_type == MANUEL ) ||
			  ( tran_type == RETURN ) ) )
	{

		/** XLS_04_02 irfan. 03/12/2001. faiz kismi aliniyor **/
		memcpy(faiz_amount_global, fpTransStruct->amount_faiz, 13);
		faiz_amount_global[13] = 0;

		/* 07_15 25/11/2002 */
		memset(local_amount_surcharge_kismi, 0, sizeof(local_amount_surcharge_kismi));
		memcpy(local_amount_surcharge_kismi, fpTransStruct->surcharge, 13);
		local_amount_surcharge_kismi[13] = 0;

		/* 06_00 virtual reverse for cancel eklendi. Ancak dönen data host'a gonderilmeyecek. */
		/** XLS_04_02 03/12/2001 irfan. iptal iþleminde xls fonksiyonu cagrilirken original
								 amount kullanilacak **/
		memset(result_amount_local, 0, sizeof(result_amount_local));
		memset(Concilation_Data_Cancel, 0, sizeof(Concilation_Data_Cancel));

		Utils_Subtract_Strings(v_amount_surcharge, faiz_amount_global, result_amount_local);
		Utils_LeftPad(result_amount_local, '0', 13);
		result_amount_local[13] = 0;

		/* 06_00 01/10/2002 */
		/* IPTAL ISLEMINDE ISLEM TUTARI ILE iptal edilecek tutar aynidir */
		memset(amount_surcharge_reverse, 0, sizeof(amount_surcharge_reverse));
		memcpy(amount_surcharge_reverse, result_amount_local,13);

		/* 07_15 25/11/2002 */ /* 06_05 surcharge kismi sifirdan farkli ise asagidaki kisim eklendi */
		if (memcmp(local_amount_surcharge_kismi, "0000000000000", 13) != 0)
		{
			Utils_Subtract_Strings(result_amount_local, local_amount_surcharge_kismi, amount_surcharge_reverse);
			Utils_LeftPad(amount_surcharge_reverse, '0', 13);
			amount_surcharge_reverse[13] = 0;
		}
		
		/** 06_00 card no local bulunuyor 26/09/2002 irfan **/
		memset(XLS_Cardno_local, 'F', sizeof(XLS_Cardno_local));
		memcpy(XLS_Cardno_local+(sizeof(XLS_Cardno_local)-1)-strlen(card_no),card_no,strlen(card_no));
		XLS_Cardno_local[19] = 0;

		/* 06_00 01/10/2002 */
/**/
		if(DEBUG_MODE)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("CANCEL XLS_VirtualReverse oncesi\n");
			
			Slips_PrinterStr("local_amount_surcharge_kismi\n");						
			Slips_PrinterStr(local_amount_surcharge_kismi);						
			PrntUtil_Cr(2);

			Slips_PrinterStr("faiz_amount_global\n");						
			Slips_PrinterStr(faiz_amount_global);						
			PrntUtil_Cr(2);

			Slips_PrinterStr("v_amount_surcharge\n");						
			Slips_PrinterStr(v_amount_surcharge);						
			PrntUtil_Cr(2);

			Slips_PrinterStr("result_amount_local:\n");
			Slips_PrinterStr(result_amount_local);
  			PrntUtil_Cr(2);
			Slips_PrinterStr("amount_surcharge_reverse\n");
			Slips_PrinterStr(amount_surcharge_reverse);
			PrntUtil_Cr(2);
			Slips_PrinterStr("ATS_Count_c\n");
			Slips_PrinterStr(ATS_Count_c);
			PrntUtil_Cr(2);
			Slips_PrinterStr("XLS_Cardno_local\n");
			Slips_PrinterStr(XLS_Cardno_local);
			PrntUtil_Cr(2);
			Slips_PrinterStr("Concilation_Data_Cancel\n");
			Slips_PrinterStr(Concilation_Data_Cancel);
			PrntUtil_Cr(2);
		}
/**/
		/* 07_15 25/11/2002 iptal isleminde reverse tutarý ve provizyon tutarlarý ayný olmalý */
			if (( POS_Type == '5' ) || ( POS_Type == '6' ))	/*04.08.2004*/
			{  
				  if (TL_Iade) XLS_Currency = 0;
				  else XLS_Currency = 1;
			}
			else XLS_Currency = 0;

			xls_ret_local =	XLS_VirtualReverse( 6, XLS_Currency, amount_surcharge_reverse,
								amount_surcharge_reverse, ATS_Count_c,
								XLS_Cardno_local, (txls_Returndetail*)Concilation_Data_Cancel );
		/*xls_ret_local = XLS_VirtualReverse(6, result_amount_local, amount_surcharge_reverse, ATS_Count_c, XLS_Cardno_local, Concilation_Data_Cancel);*/
		/*xls_ret_local = XLS_LoyaltyReverse(6, result_amount_local, ATS_Count_c, XLS_Cardno_local, v_approval_code);*/

		/* 06_00 01/10/2002 */
/**/
		if(DEBUG_MODE)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr( "CANCEL XLS_VirtualReverse sonrasi\n" );

			Slips_PrinterStr("v_approval_code\n");						
			Slips_PrinterStr(v_approval_code);						
			PrntUtil_Cr(2);

			Slips_PrinterStr("result_amount_local:\n");
			Slips_PrinterStr(result_amount_local);
  			PrntUtil_Cr(2);
			Slips_PrinterStr("amount_surcharge_reverse\n");
			Slips_PrinterStr(amount_surcharge_reverse);
			PrntUtil_Cr(2);
			Slips_PrinterStr("ATS_Count_c\n");
			Slips_PrinterStr(ATS_Count_c);
			PrntUtil_Cr(2);
			Slips_PrinterStr("XLS_Cardno_local\n");
			Slips_PrinterStr(XLS_Cardno_local);
			PrntUtil_Cr(2);
			Slips_PrinterStr("Concilation_Data_Cancel\n");
			Slips_PrinterStr(Concilation_Data_Cancel);
			PrntUtil_Cr(2);
		}
/**/
		if( xls_ret_local != XLS_NO_ERROR)
		{
			/** XLS_INTEGRATION2 23/10/2001 irfan. **/
			if(xls_ret_local != 1) /** islem kesilirse asagidaki mesaj verilmeyecek **/
			{
				memset(xls_temp_local, 0, sizeof(xls_temp_local));
				sprintf(xls_temp_local, " virtual REVERSE. HATALI Hata Kodu: %d ", xls_ret_local);
				Mainloop_DisplayWait(xls_temp_local,1);
				PrntUtil_Cr(2);
				Slips_PrinterStr(xls_temp_local);
				PrntUtil_Cr(2);
			}

			return(FALSE); /** XLS_INTEGRATION 04/09/2001 irfan. STAT_NOK yerine FALSE yapildi. **/
		}

		/* 07_15 25/11/2002 Sefanin talebi uzerine kontrol eklendi. */
		if (memcmp(Concilation_Data_Cancel+199,"021",3) == 0) 
		{
			Mainloop_DisplayWait(" YETERSIZ CHIP  PARA. IADE YAPIN",1);
			return STAT_NOK;
		}

		/* 07_15. 25/11/2002 25/11/2002. ÝPTAL iþleminde de concilation data ekleniyor */
		strcat(lbSend_Buffer,Concilation_Data_Cancel);
	}

	if (fpTransStruct->Offline != 1)
	{
		/* 07_15. 25/11/2002 ARama islemi xls modulu cagrilmadan once idi. sonraya alindi */
		void_ret_code = Host_Connect(TRUE);
		if (void_ret_code !=STAT_OK)
		{
			Host_Disconnect();
			return(void_ret_code);
		}
	
		if(Trans_SendReversal(NORMAL_REVERSAL, 0, IsEMV) != STAT_OK) return (STAT_NOK);
	
		
	/* Send the Prepared message to the Host */
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("YANIT BEKL˜YOR.. ",NO_WAIT);
	
	/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) void_ret_code = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
		else void_ret_code = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);
	/* Toygar 13.05.2002 End */
	
		Host_Disconnect();
		memcpy(&d_Void_Receive, Received_Buffer+1, sizeof(d_Void_Receive));
		if(void_ret_code != STAT_OK)
		{               
			return(void_ret_code);
		}

		if ( Trans_AnalyseCancelReceive( IsEMV, Kampanya, FALSE ) )
		{
			if (Files_VoidTransaction(fpTransStruct) == STAT_OK)
			{
				Trans_UpdateVoidCardTypeTable(fpTransStruct);
				return(STAT_OK);
			}
			else Mainloop_DisplayWait("UPDATE BAžARISIZ",2);
		}
	}
	else if ( Trans_AnalyseCancelReceive( IsEMV, Kampanya, TRUE ) )
	{
		if (Files_VoidTransaction(fpTransStruct) == STAT_OK)
		{
			Trans_UpdateVoidCardTypeTable(fpTransStruct);
			if (Files_RemoveEMVTransaction((byte*)EMVOffline_File, fpTransStruct) != STAT_OK)
			{ 
				Mainloop_DisplayWait("Files_RemoveEMVTransaction hatasi",2); 
				return(STAT_NOK);
			}
			else return(STAT_OK);
		}
		else Mainloop_DisplayWait("UPDATE BAžARISIZ",2);
	}

	return(STAT_NOK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_AnalyseCancelReceive
 *
 * DESCRIPTION          : Analyzes the void response message
 *
 * RETURN                       : none.
 *
 * NOTES                        : r.i.o & t.c.a 19/05/99
 *
 * ------------------------------------------------------------------------ */
boolean Trans_AnalyseCancelReceive( boolean IsEMV, boolean Kampanya, boolean Offline )
{

	if (!Offline)
	{
		if (d_Void_Receive.Header_Void.MSG_TYPE != '5')
		{
			Mainloop_DisplayWait("BEKLENMEYEN MESAJ",2);
			return(FALSE);
		}
	
	
		/* UPDATE 01_16 24/10/99. Dönen mesaj da ortak field'lar kontrol ediliyor */
		/* bozuk gelen ama LRC den gecen mesaj larda bozuk mesajý sense edebilmek icin yapýlýyor*/
		if(Trans_CheckVoidReturnMessage() != STAT_OK)
		{
			return(FALSE);
		}
	}

	if ((d_Void_Receive.void_status == '1') || Offline)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Debug_GrafPrint2((char *)"    ONAYLANDI   ", 2); /*MK DEF:8a*/
		/** XLS_PAYMENT 25/07/2001 irfan. loyalty cancel process **/
		if ( ( ( !IsEMV ) || ( EMVAxess ) ) && ( LOYALTY_PERMISSION_FLAG ) &&
			 ( ( POS_Type == '4' ) || ( POS_Type == '6' ) ) )
																						 /** XLS 04_02 20/12/2001 irfan. xls payment switch **/
		{
			if(TransXLSUpdateChip_Cancel() != STAT_OK) return(FALSE); /** XLS_INTEGRATION 04/09/2001 STAT_NOK yerine FALSE yapildi **/

		}
		if ( !Slips_FillReceiptStructure( FALSE, 0, Kampanya ) )
			return ( FALSE );
		EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
		Slips_PrintReceipt( IsEMV ,FALSE );
		OldKampanyaFlag = KampanyaFlag;
		KampanyaFlag = FALSE; /*@bm 20.09.2004 */
		ERROR_RECEIPT = FALSE;  /* 02_02 irfan 15/09/2000 flag degerinin korunmasý saglanýyor */
		EmvIF_ClearDsp(DSP_MERCHANT);
		return(TRUE);   
	}
	else
	{
		if(!Slips_FillErrorReceiptStructure())
		{
			Slips_PrinterStr("\nSlips_FillErrorReceiptStructure hatasi\n");
			return(STAT_NOK);
		}
		Slips_PrintErrorReceipt(FALSE);
		ERROR_RECEIPT = TRUE;   /* 02_02 irfan 15/09/2000 flag degerinin korunmasý saglanýyor */
		EmvIF_ClearDsp(DSP_MERCHANT);
		return(FALSE);
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_ChangeParam
 *
 * DESCRIPTION          : Changes the communications params
 *
 * RETURN                       : STAT_OK, STAT_NOK.
 *
 * NOTES                        : r.i.o & t.c.a 25/06/99
 *
 * ------------------------------------------------------------------------ */
void Trans_ChangeParam(void)
{

	ComHndlrParamType  Param_p1;

	Param_p1.TxLineTimeout = 2000;
	Param_p1.TxRadioTimeout = 2000;
	Param_p1.TimeBetweenTrials = 1;
	Param_p1.ExternalModem = 0;
	Param_p1.NoLineRetrys = 1;

      ComHndlr_ChangeParam ((DialTargetType *)0,
				    (ComHndlrParamType *)&Param_p1,
				    (RadioParamType *)0);

}


/*UPDATE 01_16 25/10/99. Bu versiyon için yeni eklendi.*/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_CheckReturnMessage
 *
 * DESCRIPTION          : Checks the command fields of the return message to make sure that
 *                                        received message is correct.
 *
 * RETURN                       : STAT_OK, STAT_NOK.
 *
 * NOTES                        : r.i.o & t.c.a 25/10/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_CheckReturnMessage(void)
{

	int lbStatus;
	int i;
	char received_amount_test[14];
	char send_amount_test[14];
	char result_amount_test[14];
	char lbtemp_string[100];	/* verde */

	EmvIF_ClearDsp(DSP_MERCHANT);
	/* Compare POS_software_ver*/
	if(     d_Transaction_Send.Header_Transact.POS_software_ver !=  d_Transaction_Receive.Header_Transact.POS_software_ver)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("YAZILIM VER NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("YAZILIM VER NO UYUSMUYOR",1);
		return (STAT_NOK);
	}

	/* Compare merch_device_no*/
	if (memcmp(d_Transaction_Send.Header_Transact.merch_device_no, d_Transaction_Receive.Header_Transact.merch_device_no, 8) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("TERMINAL NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("TERMINAL NO UYUSMUYOR",1);
		return (STAT_NOK);
	}


	/* Compare device_serial_no*/
	if (d_Transaction_Send.Header_Transact.device_serial_no != d_Transaction_Receive.Header_Transact.device_serial_no)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("CIHAZ SERI NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("C˜HAZ SER˜ NO UYUžMUYOR",1);
		return (STAT_NOK);
	}

	
	/** XLS_INTEGRATION 22/08/2001 irfan. loyalty faizli taksitli de 6 ve 7 icin 1 ve 4 geliyor.
	    onun icin bu kontrol kaldirildi **/
	/* Compare transaction_type*/
	/*if (d_Transaction_Send.Header_Transact.transaction_type != d_Transaction_Receive.Header_Transact.transaction_type)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("TRANSACTION TIPI UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("TRANSACTION T˜P˜ UYUžMUYOR",1);
		return (STAT_NOK);
	}*/


	/* Compare batch_no*/
	if (memcmp(d_Transaction_Send.batch_no, d_Transaction_Receive.batch_no, 3) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("YIGIN NO UYUžMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("YIGIN NO UYUžMUYOR",1);
		return (STAT_NOK);
	}

	/* Compare seq_no*/
	if (memcmp(d_Transaction_Send.transaction_seq_no, d_Transaction_Receive.transact_seq_no, 4) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("SIRA NO UYUžMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("SIRA NO UYUžMUYOR",1);
		return (STAT_NOK);
	}


	/*Numeric Digit Control of Received Date  */
	for (i = 0; i < 6; i++)
	{
		if( isdigit(d_Transaction_Receive.date[i]) == 0 )
		{

			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("TAR˜H NšMER˜K DEG˜L");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("TAR˜H NšMER˜K DEG˜L",1);
			return (STAT_NOK);
		}
	}

	/*Numeric Digit Control of Received Time  */
	for (i = 0; i < 4; i++)
	{
		if( isdigit(d_Transaction_Receive.time[i]) == 0 )
		{

			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("SAAT NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("SAAT NšMER˜K DEG˜L",1);
			return (STAT_NOK);

		}

	}


	/*Numeric Digit Control of Received Confirmation Code */
	for (i = 0; i < 3; i++)
	{
		if( isdigit(d_Transaction_Receive.confirmation[i]) == 0 )
		{

			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("KABUL RED KODU NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("KABUL RED KODU NUMERIK DEGIL",1);
			return (STAT_NOK);

		}

	}

	
	/*AlphaNumeric and space Control, (A to Z and a to z and 0 to 9 and space) of Received Approval Code */
	for (i = 0; i < 6; i++)
	{
		if( isalnum(d_Transaction_Receive.approval_code[i]) == 0 ) 
		{
			if(d_Transaction_Receive.approval_code[i] != ' ')
			{
				PrntUtil_Cr(2);
				Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
				Slips_PrinterStr("ONAY KODU BOZUK");
				PrntUtil_Cr(7);
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("ONAY KODU BOZUK",1);
				return (STAT_NOK);
			}

		}

	}

	
	/*Numeric Digit Control of Received Amount */
	for (i = 0; i < 13; i++)
	{
		if( isdigit(d_Transaction_Receive.amount_surcharge[i]) == 0 )
		{

			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("ALINAN MEBLAG NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("ALINAN MEBLAG NUMERIK DEGIL",1);
			return (STAT_NOK);

		}

	}


	/* check for host generated cash back */
	/* verde 25/10/2002 hostgenerated alaný numerik kontrolu eklendi */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, &d_Transaction_Receive.HostGeneratedCashback, 13);
/**/
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\nHostGeneratedCashback:");
		Slips_PrinterStr(lbtemp_string);
		PrntUtil_Cr(2);
	}
/**/
	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("\nHostGeneratedCashback HATASI\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}

	
	/* gecici olarak kapatildi SEfa hostdaki duzeltmeyi yaptiktan sonra acilacak. Taksitsiz islemlerde atsdate alani bosluk geliyor*/
	/* check for ATS Date */
	/* verde 31/10/2002  */
	/*memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, &d_Transaction_Receive.ATS_Date, 6);
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\nATS Date HATASI:\n");
		Slips_PrinterStr(lbtemp_string);
	}

	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("\nATS Date HATASI:\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}*/

	
	/*Received Amount_Surcharge must be less or equal than the twice of send Amount_Surcharge */
	
	memcpy(received_amount_test, d_Transaction_Receive.amount_surcharge,13);
	received_amount_test[13]=0;

	memcpy(send_amount_test, d_Transaction_Send.amount_surcharge,13);
	send_amount_test[13]=0;

	Utils_LeftPad(received_amount_test,'0',13);
	Utils_LeftPad(send_amount_test,'0',13);

	Utils_Add_Strings(send_amount_test, send_amount_test, result_amount_test);
	Utils_LeftPad(result_amount_test,'0',13);

	/* FAIZLI 02_00 25/07/2000 Donen meblag giden meblagýn 4 katýndan daha buyuk olamaz */
	/* ayný toplama iþlemi bir kez daha yapýlarak result amount_test de giden amount un
	   4 katý oluþturuluyor */

	memcpy(send_amount_test, result_amount_test,13);

	Utils_Add_Strings(send_amount_test, send_amount_test, result_amount_test);
	Utils_LeftPad(result_amount_test,'0',13);

	
	lbStatus = Utils_Compare_Strings(received_amount_test, result_amount_test);
	

	if ( lbStatus == 1 )                    /* received_amount_test > result_amount_test */
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("ALINAN MEBLAG GONDERILEN*4 DEN BUYUK");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("ALINAN MEBLAG GONDERILEN*2 DEN BUYUK",1);
		return(STAT_NOK);
	}
	

	return (STAT_OK);
	
}

/*UPDATE 01_16 25/10/99. Bu versiyon için yeni eklendi.*/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_CheckVoidReturnMessage
 *
 * DESCRIPTION          : Checks the command fields of the return message to make sure that
 *                                        received message is correct.
 *
 * RETURN                       : TRUE or FALSE.
 *
 * NOTES                        : r.i.o & t.c.a 25/10/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_CheckVoidReturnMessage(void)
{


	/* Compare POS_software_ver*/
	if(     d_Void_Send.Header_Void.POS_software_ver !=  d_Void_Receive.Header_Void.POS_software_ver)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("IPTAL YAZILIM VER NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("IPTAL YAZILIM VER NO UYUSMUYOR",1);
		return (STAT_NOK);
	}

	/* Compare merch_device_no*/
	if (memcmp(d_Void_Send.Header_Void.merch_device_no, d_Void_Receive.Header_Void.merch_device_no, 8) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("IPTAL TERMINAL NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("IPTAL TERMINAL NO UYUSMUYOR",1);
		return (STAT_NOK);
	}


	/* Compare device_serial_no*/
	if (d_Void_Send.Header_Void.device_serial_no != d_Void_Receive.Header_Void.device_serial_no)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("IPTAL CIHAZ SERI NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("IPTAL CIHAZ SERI NO UYUSMUYOR",1);
		return (STAT_NOK);
	}

	

	/* Compare batch_no*/
	if (memcmp(d_Void_Send.batch_no, d_Void_Receive.batch_no, 3) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("IPTAL YIGIN NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("IPTAL YIGIN NO UYUSMUYOR",1);
		return (STAT_NOK);
	}

	return (STAT_OK);


}


/*UPDATE 01_16 25/10/99. Bu versiyon için yeni eklendi.*/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_CheckEODReturnMessage
 *
 * DESCRIPTION          : Checks the command fields of the return message to make sure that
 *                                        received message is correct.
 *
 * RETURN                       : STAT_OK, STAT_NOK.
 *
 * NOTES                        : r.i.o & t.c.a 25/10/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_CheckEODAgreementReturnMessage(void)
{
	int i;

	/* Compare POS_software_ver*/
	/** XLS_PAYMENT 02/08/2001 irfan loyalty icin kaldirildi. Cunku giden "B" iken gelen "A" oluyor */
	/*if(     d_EOD_Send.Header_EOD.POS_software_ver !=  d_EOD_Receive.Header_EOD.POS_software_ver)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("MUTABAKAT YAZILIM VER NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MUTABAKAT YAZILIM VER NO UYUSMUYOR",1);
		return (STAT_NOK);
	}*/

	/* Compare merch_device_no*/
	if (memcmp(d_EOD_Send.Header_EOD.merch_device_no, d_EOD_Receive.Header_EOD.merch_device_no, 8) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("MUTABAKAT TERMINAL NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MUTABAKAT TERMINAL NO UYUSMUYOR",1);
		return (STAT_NOK);
	}


	/* Compare device_serial_no*/
	if (d_EOD_Send.Header_EOD.device_serial_no != d_EOD_Receive.Header_EOD.device_serial_no)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("MUTABAKAT CIHAZ SERI NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MUTABAKAT CIHAZ SERI NO UYUSMUYOR",1);
		return (STAT_NOK);
	}

	
	/* Compare transaction_type*/
	if (d_EOD_Send.Header_EOD.transaction_type != d_EOD_Receive.Header_EOD.transaction_type)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("MUTABAKAT TRANSACTION TIPI UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MUTABAKAT TRANSACTION TIPI UYUSMUYOR",1);
		return (STAT_NOK);
	}


	/* Compare batch_no*/
	if (memcmp(d_EOD_Send.batch_no, d_EOD_Receive.batch_no, 3) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("MUTABAKAT YIGIN NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MUTABAKAT YIGIN NO UYUSMUYOR",1);
		return (STAT_NOK);
	}



	/*Numeric Digit Control of Received Date  */
	for (i = 0; i < 6; i++)
	{
		if( isdigit(d_EOD_Receive.date[i]) == 0 )
		{

			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("MUTABAKAT TARIH NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("MUTABAKAT TARIH NUMERIK DEGIL",1);
			return (STAT_NOK);

		}

	}

	/*Numeric Digit Control of Received Time  */
	for (i = 0; i < 4; i++)
	{
		if( isdigit(d_EOD_Receive.time[i]) == 0 )
		{

			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("MUTABAKAT SAAT NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("MUTABAKAT SAAT NUMERIK DEGIL",1);
			return (STAT_NOK);

		}

	}


	/*Numeric Digit Control of Received Confirmation Code */
	for (i = 0; i < 3; i++)
	{
		if( isdigit(d_EOD_Receive.confirmation_code[i]) == 0 )
		{

			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("MUTABAKAT KABUL RED KODU NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("MUTABAKAT KABUL RED KODU NUMERIK DEGIL",1);
			return (STAT_NOK);

		}

	}


	return (STAT_OK);
	
}

/*UPDATE 01_16 25/10/99. Bu versiyon için yeni eklendi.*/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_CheckEODNewBatchNoReturnMessage
 *
 * DESCRIPTION          : Checks the command fields of the return message to make sure that
 *                                        received message is correct.
 *
 * RETURN                       : STAT_OK, STAT_NOK.
 *
 * NOTES                        : r.i.o & t.c.a 25/10/99
 *
 * ------------------------------------------------------------------------ */
byte Trans_CheckEODNewBatchNoReturnMessage(void)
{
	int i;
	
	/* Compare POS_software_ver*/
	/** XLS_PAYMENT 02/08/2001 irfan loyalty icin kaldirildi. Cunku giden "B" iken gelen "A" oluyor */
	/*if(     d_Param_Send.Header_Param.POS_software_ver !=  d_Param_Receive.Header_EOD.POS_software_ver)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("YENI YIGIN NO YAZILIM VER NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MUTABAKAT YAZILIM VER NO UYUSMUYOR",1);
		return (STAT_NOK);
	}*/

	/* Compare merch_device_no*/
	if (memcmp(d_Param_Send.Header_Param.merch_device_no, d_Param_Receive.Header_EOD.merch_device_no, 8) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("YENI YIGIN NO TERMINAL NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("YENI YIGIN NO TERMINAL NO UYUSMUYOR",1);
		return (STAT_NOK);
	}

	/* Compare device_serial_no*/
	if (d_Param_Send.Header_Param.device_serial_no != d_Param_Receive.Header_EOD.device_serial_no)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("YENI YIGIN NO CIHAZ SERI NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("YENI YIGIN NO CIHAZ SERI NO UYUSMUYOR",1);
		return (STAT_NOK);
	}
	
	/* Compare transaction_type*/
	if (d_Param_Send.Header_Param.transaction_type != d_Param_Receive.Header_EOD.transaction_type)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("YENI YIGIN NO TRANSACTION TIPI UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("YENI YIGIN NO TRANSACTION TIPI UYUSMUYOR",1);
		return (STAT_NOK);
	}

	/* Compare batch_no*/
	if (memcmp(d_Param_Send.batch_no, d_Param_Receive.batch_no, 3) != 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("YENI YIGIN NO YIGIN NO UYUSMUYOR");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("YENI YIGIN NO YIGIN NO UYUSMUYOR",1);
		return (STAT_NOK);
	}

	/*Numeric Digit Control of Received Date  */
	for (i = 0; i < 6; i++)
	{
		if( isdigit(d_Param_Receive.date[i]) == 0 )
		{

			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("YENI YIGIN TARIH NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("YENI YIGIN TARIH NUMERIK DEGIL",1);
			return (STAT_NOK);
		}
	}

	/*Numeric Digit Control of Received Time  */
	for (i = 0; i < 4; i++)
	{
		if( isdigit(d_Param_Receive.time[i]) == 0 )
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("YENIYIGIN SAAT NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("YENIYIGIN SAAT NUMERIK DEGIL",1);
			return (STAT_NOK);
		}
	}

	/*Numeric Digit Control of Received Confirmation Code */
	for (i = 0; i < 3; i++)
	{
		if( isdigit(d_Param_Receive.confirmation_code[i]) == 0 )
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("YENIYIGIN KABUL RED KODU NUMERIK DEGIL");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("YENIYIGIN KABUL RED KODU NUMERIK DEGIL",1);
			return (STAT_NOK);
		}

	}
	TempEMV_Key_Version_No = d_Param_Receive.EMV_Key_Version_No;
	TempConfig_Version_No = d_Param_Receive.Config_Version_No;

/*
	Printer_WriteStr("From Host:\n");
	PrntUtil_BufAsciiHex((byte *)&TempEMV_Key_Version_No,1);
	PrntUtil_BufAsciiHex((byte *)&TempConfig_Version_No,1);	
/* */

	return (STAT_OK);
}


/* 02_01 08/09/2000 irfan pinpad den taksit sorma */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Trans_AskInstallment
 *
 * DESCRIPTION  : Ask the Installment count
 *
 * RETURN               : STAT_OK, STAT_NOK
 *
 * NOTES                : r.i.o  08/09/00
 *
 * ------------------------------------------------------------------------ */
byte Trans_AskInstallment(char *Pin_Installment)
{
	byte out [3];

	memset (out, 0, 3);
	
	switch(Host_GetInstallment (out))
	{
	case STAT_OK:
			Host_CancelSessionRequest();
			strcpy (Pin_Installment, (char *)out);
			return (STAT_OK);

	case STAT_NO_CONNECTION:
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("  ž˜FRE C˜HAZI    BA¦LI DE¦˜L   ",1);
		return (STAT_NOK);

	case EOT:
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("   ISLEM IPTAL        EDILDI    ",1);
		return (STAT_NOK);

	default :
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("TAKSIT SORMA HATASI",2);
		return (STAT_NOK);
	}

}
/********************************************
/** XLS Related Functions 23/07/2001 irfan **
/********************************************/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Trans_XLSUpdateChip
 *
 * DESCRIPTION		: Call XLS module to update the chip card
 *
 * RETURN			: STAT_OK, STAT_NOK
 *
 * NOTES			: r.i.o  23/07/01
 *
 * ------------------------------------------------------------------------ */
byte Trans_XLSUpdateChip(void)
{
	char offline_approval_code[7];
	byte xls_ret_local;			/** XLS_INTEGRATION2 23/10/2001 irfan  **/
	char xls_temp_local[50];	/** XLS_INTEGRATION2 23/10/2001 irfan **/
	char theHostCashBackOperationFlag;

	theHostCashBackOperationFlag = 0;
	memset (approval_code, 0, sizeof(approval_code));
	memcpy(approval_code, d_Transaction_Receive.approval_code, sizeof(d_Transaction_Receive.approval_code));
	approval_code[6] = 0;
	memset(HostGeneratedCashback, 0, sizeof(HostGeneratedCashback));
	if(tran_type != RETURN)
	{
		memcpy(HostGeneratedCashback, d_Transaction_Receive.HostGeneratedCashback,13);
/**/
		if(DEBUG_MODE)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("XLS_CommitRwdTrx ONCESI\n");
			Slips_PrinterStr("HostGeneratedCashback\n");
			Slips_PrinterStr(HostGeneratedCashback);
  			PrntUtil_Cr(2);
			Slips_PrinterStr("approval_code\n");
			Slips_PrinterStr(approval_code);
		}
/**/

		xls_ret_local = XLS_CommitRwdTrx(  theHostCashBackOperationFlag, HostGeneratedCashback, approval_code );
/**/
		if ( DEBUG_MODE )
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("XLS_CommitRwdTrx SONRASI\n");
			Slips_PrinterStr("HostGeneratedCashback\n");
			Slips_PrinterStr(HostGeneratedCashback);
  			PrntUtil_Cr(2);
			Slips_PrinterStr("approval_code\n");
			Slips_PrinterStr(approval_code);
		}
		if ( xls_ret_local == XLS_NO_ERROR )
		{
			return(STAT_OK);
		}
		else
		{
			memset(xls_temp_local, 0, sizeof(xls_temp_local));
			sprintf(xls_temp_local, "REWARD  HATALI  Hata Kodu: %d\n", xls_ret_local);
			Mainloop_DisplayWait(xls_temp_local,1);
			PrntUtil_Cr(2);
			Slips_PrinterStr(xls_temp_local);						
			PrntUtil_Cr(2);
			EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
			EMVAxess = FALSE;
			return(STAT_NOK);
		}
	}
	else
	{
/**/
		if(DEBUG_MODE)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("XLS_CommitRevTrx ONCESI\n");
  			PrntUtil_Cr(2);
			Slips_PrinterStr("approval_code\n");
			Slips_PrinterStr(approval_code);
		}
/**/
		xls_ret_local = XLS_CommitRevTrx( approval_code );
/**/
		if(DEBUG_MODE)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("XLS_CommitRevTrx SONRASI\n");
  			PrntUtil_Cr(2);
			Slips_PrinterStr("approval_code\n");
			Slips_PrinterStr(approval_code);
		}
/**/

		if (xls_ret_local == XLS_NO_ERROR) return(STAT_OK);
		else
		{
			/** XLS_INTEGRATION2 23/10/2001 irfan. Hatali durumda donus kodunu bastýrýr.**/
			memset(xls_temp_local, 0, sizeof(xls_temp_local));
			sprintf(xls_temp_local, "commit REVERSE HATALI  Hata Kodu: %d\n", xls_ret_local);
			Mainloop_DisplayWait(xls_temp_local,1);
			PrntUtil_Cr(2);
			Slips_PrinterStr(xls_temp_local);						
			PrntUtil_Cr(2);
			EMVAxessStatu = FALSE; /*@bm 15.09.2004 */
			EMVAxess = FALSE;
			return(STAT_NOK);
		}
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: TransXLSUpdateChip_Cancel
 *
 * DESCRIPTION		: Call XLS module to update the chip card for cancel process
 *
 * RETURN			: STAT_OK, STAT_NOK
 *
 * NOTES			: r.i.o  25/07/01
 *
 * ------------------------------------------------------------------------ */
byte TransXLSUpdateChip_Cancel(void)
{

	byte xls_ret_local;			/** XLS_INTEGRATION2 23/10/2001 irfan  **/
	char xls_temp_local[33];	/** XLS_INTEGRATION2 23/10/2001 irfan **/

	/*   Cancel */

	/** XLS_INTEGRATION wrt nin istegi ile amount_surcharge ve ats_count_c eklendi. 15/08/2001 **/
	/** ver_04_04 15/03/2002 irfan. approval_code  v_approval_code yapildi. **/

	/* 06_00 commit rever eklendi */
	/*xls_ret_local = XLS_LoyaltyReverse(6, result_amount_local, ATS_Count_c, XLS_Cardno_local, v_approval_code);*/
/**/
	if(DEBUG_MODE)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("CANCEL XLS_CommitRevTrx ONCESI\n");
  		PrntUtil_Cr(2);
		Slips_PrinterStr("v_approval_code\n");
		Slips_PrinterStr(v_approval_code);
	}
/**/
	xls_ret_local = XLS_CommitRevTrx( v_approval_code );
/**/
	if(DEBUG_MODE)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("CANCEL XLS_CommitRevTrx SONRASI\n");
  		PrntUtil_Cr(2);
		Slips_PrinterStr("v_approval_code\n");
		Slips_PrinterStr(v_approval_code);
	}
/**/
	
	if( xls_ret_local == XLS_NO_ERROR)
	{

		return(STAT_OK);
	}
	else
	{
		/** XLS_INTEGRATION2 23/10/2001 irfan. Hatali durumda donus kodunu bastýrýr.**/
		memset(xls_temp_local, 0, sizeof(xls_temp_local));
		sprintf(xls_temp_local, "REVERSE2 HATALI Hata Kodu: %d\n", xls_ret_local);
		Mainloop_DisplayWait(xls_temp_local,1);
		PrntUtil_Cr(2);
		Slips_PrinterStr(xls_temp_local);						
		PrntUtil_Cr(2);
		EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
		EMVAxess = FALSE;
		return(STAT_NOK);
	}
}

/* Toygar EMVed */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: TransEMVOfflineTransaction
 *
 * DESCRIPTION		: 
 *
 * RETURN			: STAT_OK, STAT_NOK
 *
 * NOTES			: 
 *
 * ------------------------------------------------------------------------ */
byte TransEMVOfflineTransaction(PTStruct *fpPTStruct)
{
	int TransOnlineNo, TransOfflineNo, myseq_no;
	char myseq_no_c[5];
	
	if ( !Slips_FillReceiptStructure( TRUE, fpPTStruct, FALSE ) )
	{
		Mainloop_DisplayWait("Slips_FillReceiptStructure hatasi",2);
		CallFlag = TRUE;
		return(STAT_NOK);
	}

	/* eger offline ise approval code generate ediliyor. */
	Files_GetTransactionCounts( &TransOnlineNo, &TransOfflineNo );
	myseq_no = TransOnlineNo + TransOfflineNo + 1;
	Utils_IToA(batch_no,batch_no_c);
	Utils_LeftPad(batch_no_c, '0', 3);
	Utils_IToA(myseq_no,myseq_no_c);
	Utils_LeftPad(myseq_no_c, '0', 4);
	memset(d_Receipt.approval_code, 0, sizeof(d_Receipt.approval_code));
	memcpy(d_Receipt.approval_code, batch_no_c,3);
	memcpy(d_Receipt.approval_code+3,myseq_no_c+1,3);
	memcpy(approval_code, d_Receipt.approval_code,6);
/*	seq_no++;*/

	if (Files_AddEMVTransaction((byte*)EMVOffline_File, fpPTStruct) <= 0) 
	{ 
		Mainloop_DisplayWait("Files_AddEMVTransaction hatasi",2); 
		CallFlag = TRUE; 
		EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
		EMVAxess = FALSE;
		return(STAT_NOK);
	}
	if(Files_RecordWrite(TRUE) != STAT_OK) return(STAT_NOK);
	if(Trans_UpdateCardTypeTable() != STAT_OK)
	{
		Mainloop_DisplayWait("PARAMETRE UPDATE    HATASI",2);
		EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
		EMVAxess = FALSE;
		return(STAT_NOK);
	}
	EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
	Slips_PrintReceipt( TRUE ,FALSE);
	Mte_Wait(1000);
	Kbhw_BuzzerPulse(200);
	Mte_Wait(300);
	Kbhw_BuzzerPulse(200);

	SECOND_COPY = TRUE;
	if(SECOND_COPY)
	{
		Mainloop_DisplayWait(" 2. KOPYA ˜€˜N  G˜R˜ž'E BASINIZ.",10);
		EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
		Slips_PrintReceipt( TRUE ,FALSE);
		OldKampanyaFlag = KampanyaFlag;
		KampanyaFlag = FALSE; /*@bm 20.09.2004 */
		EmvIF_ClearDsp(DSP_MERCHANT);
	}
	else { OldKampanyaFlag = KampanyaFlag; KampanyaFlag = FALSE; }

	ERROR_RECEIPT = FALSE;
	CallFlag = TRUE;

	/* Increment sequence number*/
/*	seq_no++;*/
	return(STAT_OK);
}
/* Toygar EMVed */

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: TransXLSOfflineTransaction
 *
 * DESCRIPTION		: executte offine transaction
 *
 * RETURN			: STAT_OK, STAT_NOK
 *
 * NOTES			: r.i.o  01/08/01
 *
 * ------------------------------------------------------------------------ */
byte TransXLSOfflineTransaction(void)
{
	/* 06_30 axess islemler offline islemler yapamazlar */
	/*if(Trans_XLSUpdateChip() != STAT_OK) return(STAT_NOK);*/
	
	if(!Slips_FillReceiptStructure(FALSE,0,FALSE)) 
	{
		Mainloop_DisplayWait("Slips_FillReceiptStructure hatasi",2);
		return(STAT_NOK);
	}

	if(Files_RecordWrite(FALSE) != STAT_OK)
	return(STAT_NOK);


	if(Trans_UpdateCardTypeTable() != STAT_OK)
	{
		Mainloop_DisplayWait("PARAMETRE UPDATE    HATASI",2);

		return(STAT_NOK);
	}
	EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
	Slips_PrintReceipt( FALSE,FALSE );
	Mte_Wait(1000);
	Kbhw_BuzzerPulse(200);
	Mte_Wait(300);
	Kbhw_BuzzerPulse(200);

	SECOND_COPY = TRUE;
	
	if(SECOND_COPY)
	{
		Mainloop_DisplayWait(" 2. KOPYA ˜€˜N  G˜R˜ž'E BASINIZ.",10);
		EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
		Slips_PrintReceipt( FALSE,FALSE );
		OldKampanyaFlag = KampanyaFlag;
		KampanyaFlag = FALSE; /*@bm 20.09.2004 */
		EmvIF_ClearDsp(DSP_MERCHANT);
	}
	else { OldKampanyaFlag = KampanyaFlag; KampanyaFlag = FALSE; }

	ERROR_RECEIPT = FALSE;

	/* Increment sequence number*/
	seq_no++;
	Utils_IToA(seq_no,seq_no_c);
	Utils_LeftPad(seq_no_c, '0', 4);

	/* UPDATE 01_16 30/10/99 OLUMLU DURUMDA RETURN ETMIYORDU. MISSING RETURN VALUE ONLENDI*/
	return(STAT_OK);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: TransXLSOfflineCancel
 *
 * DESCRIPTION		: execute offine cancel transaction
 *
 * RETURN			: STAT_OK, STAT_NOK
 *
 * NOTES			: r.i.o  01/08/01
 *
 * ------------------------------------------------------------------------ */
byte TransXLSOfflineCancel(TransactionStruct *fpTransStruct) /* 06_30 IsEmv flag ikinci parametre idi kaldirildi */
{
	boolean void_msg_OK;
	byte void_ret_code;

	void_msg_OK = FALSE;

	memcpy(v_approval_code, fpTransStruct->approval_code, 6);
	v_approval_code[6] = 0;

	memcpy(v_amount_surcharge, fpTransStruct->amount_surcharge, 13);
	v_amount_surcharge[13] = 0;

	
	memcpy(v_seq_no_c, fpTransStruct->seq_no, 4);
	v_seq_no_c[4] = 0;


	Utils_IToA(batch_no,batch_no_c);        
	Utils_LeftPad(batch_no_c, '0', 3);

	/* 06_30 axess islemleri offline yapilamaz o yuzden kapatildi IsEMV flag'i da fonksiyon parametrelerinden biri idi. Kaldirildi. */
	/** XLS_PAYMENT 25/07/2001 irfan. loyalty cancel process **/
/*	if ((LOYALTY_PERMISSION_FLAG) && (!IsEMV))
	{
		if(TransXLSUpdateChip_Cancel() != STAT_OK) return(FALSE); /** XLS_INTEGRATION 04/09/2001 STAT_NOK yerine FALSE yapildi **/
/*	}*/

	if(!Slips_FillReceiptStructure(FALSE,0,FALSE)) return(FALSE);
	EMVAxessStatu = FALSE; /*@bm 16.09.2004*/
	Slips_PrintReceipt( FALSE ,FALSE);
	OldKampanyaFlag = KampanyaFlag;
	KampanyaFlag = FALSE; /*@bm 20.09.2004 */
	ERROR_RECEIPT = FALSE;  /* 02_02 irfan 15/09/2000 flag degerinin korunmasý saglanýyor */

	if (Files_VoidTransaction(fpTransStruct) == STAT_OK)
	{
		Trans_UpdateVoidCardTypeTable(fpTransStruct);
		return(STAT_OK);
	}
	else
	{
		Mainloop_DisplayWait("UPDATE BAžARISIZ",2);
		return(STAT_NOK);
	}

}


/* Toygar 13.05.2002 Start */

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME  : Trans_FillnSend_Confirmation
 *
 * DESCRIPTION    : Fills and sends negative or positive confirmation
 *
 * RETURN         : True if ACK received, False otherwise
 *
 * NOTES          : 22/10/1999  Buyuk Firma
 *
 * ------------------------------------------------------------------------ */
boolean Trans_FillnSend_Confirmation(boolean confirmation_positive)
{
	byte confirmation_ret_code;
/* 06_01 RECONCILATION irfan 20/09/2002 */
	char lbSend_Buffer[900];
	char device_date[8], device_time[6];
	struct Void_Send d_Confirmation_Send;

	/******************** FILL  HEADER ********************/

/* bm YTL 21/06/2004 */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		d_Confirmation_Send.Header_Void.Cur_Index = 'X';
	else
		d_Confirmation_Send.Header_Void.Cur_Index = 'Y';
	d_Confirmation_Send.Header_Void.ActAppCode = POS_Type;
	d_Confirmation_Send.Header_Void.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
	memcpy( d_Confirmation_Send.Header_Void.PosType, "LIP", 3 );
	memcpy( d_Confirmation_Send.Header_Void.PosModel, "8000", 4 );
	memcpy( d_Confirmation_Send.Header_Void.PosVersion, POS_VERSION_NUMBER, 4 );
	memset( d_Confirmation_Send.Header_Void.Cashier_Id, '0', 16 );
	memcpy( d_Confirmation_Send.Header_Void.IMEI_ID, Kampanya_Data , 15 );
	memset( d_Confirmation_Send.Header_Void.Filler, ' ', 19 );
	Utils_GetDateTime( device_date, device_time );
	d_Confirmation_Send.Header_Void.PosDate[0] = device_date[6];
	d_Confirmation_Send.Header_Void.PosDate[1] = device_date[7];
	d_Confirmation_Send.Header_Void.PosDate[2] = device_date[3];
	d_Confirmation_Send.Header_Void.PosDate[3] = device_date[4];
	d_Confirmation_Send.Header_Void.PosDate[4] = device_date[0];
	d_Confirmation_Send.Header_Void.PosDate[5] = device_date[1];
	d_Confirmation_Send.Header_Void.PosTime[0] = device_time[0];
	d_Confirmation_Send.Header_Void.PosTime[1] = device_time[1];
	d_Confirmation_Send.Header_Void.PosTime[2] = device_time[3];
	d_Confirmation_Send.Header_Void.PosTime[3] = device_time[4];

/* bm YTL */
	d_Confirmation_Send.Header_Void.MSG_TYPE='4';
	d_Confirmation_Send.Header_Void.POS_software_ver = POS_software_ver;
	memcpy(d_Confirmation_Send.Header_Void.merch_device_no , merch_device_no , 8);
	d_Confirmation_Send.Header_Void.device_serial_no = device_serial_no;
	d_Confirmation_Send.Header_Void.transaction_type = tran_type + '0';

	/******************** FILL DETAIL ********************/
	Utils_IToA(batch_no,batch_no_c);        
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(d_Confirmation_Send.batch_no, batch_no_c,3);

	memcpy(d_Confirmation_Send.transaction_seq_no, seq_no_c, 4);
	
	if (confirmation_positive)
	{
		memcpy(d_Confirmation_Send.void_code, "01", 2);
		memcpy(d_Confirmation_Send.approval_code, d_Transaction_Receive.approval_code, 6);
	}
	else
	{
		memcpy(d_Confirmation_Send.void_code, "03", 2);
		memcpy(d_Confirmation_Send.approval_code, "      ", 6);
	}

/* Toygar EMVed */
/*	memset(lbSend_Buffer, 0, 256);*/
	memset( lbSend_Buffer, 0, 900 );
/* Toygar EMVed */
	memcpy (lbSend_Buffer,&d_Confirmation_Send,sizeof(d_Confirmation_Send));

	/* Send the Prepared message to the PC */
	confirmation_ret_code = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, FALSE);

	if(confirmation_ret_code == STAT_OK) 
		return (TRUE);
	else
		return (FALSE);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Trans_PassEMVData
 *
 * DESCRIPTION:
 *             
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ *
void Trans_PassEMVData(PTStruct *fpPTStruct, EMVTransData *petdData)
{
  memcpy(fpPTStruct->AID, petdData->AID, petdData->AIDLen);
  fpPTStruct->AIDLen = petdData->AIDLen;
  memcpy(fpPTStruct->Amount, petdData->Amount, 15);
  memcpy(fpPTStruct->Field55, petdData->Field55 , petdData->Field55Len);
  fpPTStruct->Field55Len = petdData->Field55Len;
  memcpy(fpPTStruct->Pan, petdData->Pan, petdData->PanLength);
  fpPTStruct->PanLength = petdData->PanLength;
  memcpy(fpPTStruct->PIN, petdData->PIN, sizeof(petdData->PIN));
  fpPTStruct->Signature = petdData->bSignature;
  memcpy(fpPTStruct->TrackISO2, petdData->TrackISO2, sizeof(petdData->TrackISO2))
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Trans_AddChipFields
 *
 * DESCRIPTION:
 *             
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Trans_AddChipFields(byte *pbField, EMVTransData *petdData)
{
	byte *pbIndex;

	/* Field55 generation */
	pbIndex = pbField;
/*	Printer_WriteStr("tagPOS_ENTRY_MODE\n");*/
	EmvIF_AppendData(&pbIndex, tagPOS_ENTRY_MODE, TRUE, 4);
/*	Printer_WriteStr("tagAIP\n");*/
	EmvIF_AppendData(&pbIndex, tagAIP, TRUE, 4);
/*	Printer_WriteStr("tagDF_NAME\n");*/
	EmvIF_AppendData(&pbIndex, tagDF_NAME, TRUE, 32);
/*	Printer_WriteStr("tagTVR\n");*/
	EmvIF_AppendData(&pbIndex, tagTVR, TRUE, 10);
/*	Printer_WriteStr("tagISSUER_APPL_DATA\n");*/
	EmvIF_AppendData(&pbIndex, tagISSUER_APPL_DATA, TRUE, 64);
/*	Printer_WriteStr("tagIFD_SERIAL_NUM\n");*/
	EmvIF_AppendData(&pbIndex, tagIFD_SERIAL_NUM, TRUE, 16);
/*	Printer_WriteStr("tagAPPL_CRYPTOGRAM\n");*/
	EmvIF_AppendData(&pbIndex, tagAPPL_CRYPTOGRAM, TRUE, 16);
/*	Printer_WriteStr("tagCRYPTOGRAM_INFO\n");*/
	EmvIF_AppendData(&pbIndex, tagCRYPTOGRAM_INFO, TRUE, 2);
/*	Printer_WriteStr("tagTERML_CAPABILITIES\n");*/
 	EmvIF_AppendData(&pbIndex, tagTERML_CAPABILITIES, TRUE, 6);
/*	Printer_WriteStr("tagCVM_RESULT\n");*/
	EmvIF_AppendData(&pbIndex, tagCVM_RESULT, TRUE, 6);
/*	Printer_WriteStr("tagTERML_TYPE\n");*/
	EmvIF_AppendData(&pbIndex, tagTERML_TYPE, TRUE, 2);
/*	Printer_WriteStr("tagATC\n");*/
	EmvIF_AppendData(&pbIndex, tagATC, TRUE, 4);
/*	Printer_WriteStr("tagUNPREDICTABLE_NUMBER\n");*/
	EmvIF_AppendData(&pbIndex, tagUNPREDICTABLE_NUMBER, TRUE, 8);
/*	Printer_WriteStr("tagTRAN_SEQUENCE_COUNTER\n");*/
	EmvIF_AppendData(&pbIndex, tagTRAN_SEQUENCE_COUNTER, TRUE, 8);
/*	Printer_WriteStr("tagTRN_CATEGORY_CODE\n");*/
	EmvIF_AppendData(&pbIndex, tagTRN_CATEGORY_CODE, TRUE, 2);
/*	Printer_WriteStr("tagAUTH_RESPONSE_CODE\n");*/
	EmvIF_AppendData(&pbIndex, tagAUTH_RESPONSE_CODE, FALSE, 2);
 
	/* Issuer Script Results */
	EmvIF_BinToAscii(petdData->abIssuerScrResults, pbIndex, petdData->bIssScrResults_Len, petdData->bIssScrResults_Len*2, TRUE);
	pbIndex += petdData->bIssScrResults_Len*2;

/*
	Printer_WriteStr("Script field\n");
	PrntUtil_BufAsciiHex(petdData->abIssuerScrResults, petdData->bIssScrResults_Len);
	Printer_WriteStr("Field55\n");
	PrntUtil_BufAsciiHex(pbField, pbIndex - pbField);
*/
	return (pbIndex - pbField);
}

byte Trans_AddChipFieldsWithoutScript( byte * pbField)
{
	byte *pbIndex;

	/* Field55 generation */
	pbIndex = pbField;
/*	Printer_WriteStr("tagPOS_ENTRY_MODE\n");*/
	EmvIF_AppendData( &pbIndex, tagPOS_ENTRY_MODE, TRUE, 4 );
/*	Printer_WriteStr("tagAIP\n");*/
	EmvIF_AppendData( &pbIndex, tagAIP, TRUE, 4 );
/*	Printer_WriteStr("tagDF_NAME\n");*/
	EmvIF_AppendData( &pbIndex, tagDF_NAME, TRUE, 32 );
/*	Printer_WriteStr("tagTVR\n");*/
	EmvIF_AppendData( &pbIndex, tagTVR, TRUE, 10 );
/*	Printer_WriteStr("tagISSUER_APPL_DATA\n");*/
	EmvIF_AppendData( &pbIndex, tagISSUER_APPL_DATA, TRUE, 64 );
/*	Printer_WriteStr("tagIFD_SERIAL_NUM\n");*/
	EmvIF_AppendData( &pbIndex, tagIFD_SERIAL_NUM, TRUE, 16 );
/*	Printer_WriteStr("tagAPPL_CRYPTOGRAM\n");*/
	EmvIF_AppendData( &pbIndex, tagAPPL_CRYPTOGRAM, TRUE, 16 );
/*	Printer_WriteStr("tagCRYPTOGRAM_INFO\n");*/
	EmvIF_AppendData( &pbIndex, tagCRYPTOGRAM_INFO, TRUE, 2 );
/*	Printer_WriteStr("tagTERML_CAPABILITIES\n");*/
	EmvIF_AppendData( &pbIndex, tagTERML_CAPABILITIES, TRUE, 6 );
/*	Printer_WriteStr("tagCVM_RESULT\n");*/
	EmvIF_AppendData( &pbIndex, tagCVM_RESULT, TRUE, 6 );
/*	Printer_WriteStr("tagTERML_TYPE\n");*/
	EmvIF_AppendData( &pbIndex, tagTERML_TYPE, TRUE, 2 );
/*	Printer_WriteStr("tagATC\n");*/
	EmvIF_AppendData( &pbIndex, tagATC, TRUE, 4 );
/*	Printer_WriteStr("tagUNPREDICTABLE_NUMBER\n");*/
	EmvIF_AppendData( &pbIndex, tagUNPREDICTABLE_NUMBER, TRUE, 8 );
/*	Printer_WriteStr("tagTRAN_SEQUENCE_COUNTER\n");*/
	EmvIF_AppendData( &pbIndex, tagTRAN_SEQUENCE_COUNTER, TRUE, 8 );
/*	Printer_WriteStr("tagTRN_CATEGORY_CODE\n");*/
	EmvIF_AppendData( &pbIndex, tagTRN_CATEGORY_CODE, TRUE, 2 );
/*	Printer_WriteStr("tagAUTH_RESPONSE_CODE\n");*/
	EmvIF_AppendData( &pbIndex, tagAUTH_RESPONSE_CODE, FALSE, 2 );

	return ( pbIndex - pbField );
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Trans_BatchUpload
 *
 * DESCRIPTION:
 *             
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Trans_BatchUpload(boolean Offline)
{
	struct Transaction_Send fpTransaction_Send;
	struct Transaction_Receive fpTransaction_Receive;
	sint lwLastSeq, i;
	byte lbStatus, Transaction_Ret;
	sint Handle1;
	char temp[20];
/* 06_01 RECONCILATION irfan 20/09/2002 */
	char lbSend_Buffer[900];

	if (Offline)
	{
		if (RamDisk_ChkDsk((byte *)EMVOffline_File) != RD_CHECK_OK) return STAT_NOK;
		Handle1 = RamDisk_Reopen((byte *)EMVOffline_File)->Handle;
	}
	else
	{
		if (RamDisk_ChkDsk((byte *)EMVOnline_File) != RD_CHECK_OK) return STAT_NOK;
		Handle1 = RamDisk_Reopen((byte *)EMVOnline_File)->Handle;
	}
	RamDisk_Seek(Handle1, SEEK_FIRST);
	lwLastSeq = RamDisk_Seek(Handle1,SEEK_LAST);
	for (i = lwLastSeq - 1; i >= 0 ; i--)   
	{
		if (RamDisk_Seek(Handle1, i) >= 0)
		{
			lbStatus = RamDisk_Read(Handle1, &fpTransaction_Send);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
/* Toygar 13.05.2002 Start */
				int substval;

				if (fpTransaction_Send.Header_Transact.MSG_INDICATOR != 'E') substval = sizeof(fpTransaction_Send.EMVFields);
				else substval = 0;
				fpTransaction_Send.Header_Transact.MSG_INDICATOR = 'B';
				memset(lbSend_Buffer,0,900);
				memset(Received_Buffer,0,900);
				memcpy (lbSend_Buffer,&fpTransaction_Send,sizeof(fpTransaction_Send)-substval);
				if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
				else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);
				if (Transaction_Ret != STAT_OK) return STAT_NOK;
/*				if (memcmp(Received_Buffer+1,"ACK",3) != 0) return STAT_NOK;*/
/* Change of Flow Control 25.07.2002 */
				memcpy(&fpTransaction_Receive,Received_Buffer+1,sizeof(fpTransaction_Receive));
				if (memcmp(fpTransaction_Receive.confirmation, "000",3) != 0)
				{
					PrntUtil_Cr(5);
					Printer_WriteStr("****      BATCH     ****");
					Printer_WriteStr("**** UPLOAD  HATASI ****");
					Printer_WriteStr("****");
					memset(temp,0,sizeof(temp));
					memcpy(temp,fpTransaction_Receive.message,sizeof(fpTransaction_Receive.message));
					Printer_WriteStr(fpTransaction_Receive.message);
					Printer_WriteStr("****");
					PrntUtil_Cr(5);

					GrphText_Cls (TRUE);
					Debug_GrafPrint2("      BATCH     ", 2);
					Debug_GrafPrint2(" UPLOAD  HATASI ", 3);

					return(STAT_NOK);
				}
/* Change of Flow Control 25.07.2002 */
				if (RamDisk_ClearFile (Handle1, i, i) != RD_OK)
				{
					Printer_WriteStr("DeleteError\n");
					return STAT_NOK;
				}
/* Toygar 13.05.2002 End */
			}
			else break;
		}
		else break;
	}
	return STAT_OK;
}
/* Toygar 13.05.2002 End */

/********************************** GENIUS FUNCTIONS *****************************************/
/* 03_01 27/09/2002 */

/* --------------------------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GeniusTransaction
 *
 * DESCRIPTION          : Transaction which handles Genius Sale, Genius Cancel, Genius Sale(installment)
 *                         Genius Sale(installment) Cancel
 *
 * RETURN                       : STAT OK, STAT_NOK
 *
 * NOTES                        : r.i.o.
 *                        Gtran_type : indicates the type of the genius transaction
 *                           GENIUS_SATIS			1
 *							 GENIUS_VADELI			2
 *							 GENUIS_IPTAL			3
 *
 * -------------------------------------------------------------------------------------------- */
byte Trans_GeniusTransaction(int gtran_type)
{
	/* balance related variables */
	char temp_buff[17];
	char balance_message[33];
	char TransReceived_DateTime[11];
	byte Transaction_Ret=0;
	char lbtemp[50];
	
	char lbSend_Buffer[800];
	
	
	Printer_WriteStr("trans_Genius\n");
	
	/* Check for installation*/
	if ( !INSTALLATION)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" KURULUž YAPIN  ",1);
		return(STAT_NOK);
	}

	/* Check for incomplete EOD transcation*/
	if (EOD_flag)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" GšNSONU YAPIN  ",1);
		return(STAT_NOK);
	}

	/*read parameters files */
	if(Files_ReadAllParams() != STAT_OK) return (STAT_NOK);


	/* Initialize the Send Buffer*/
	memset(lbSend_Buffer,0,sizeof(lbSend_Buffer));
	
	/* intialize the Send structure */
	memset(&d_GTransaction_Send,'0',sizeof(d_GTransaction_Send));

	/* Set Debit and Amex flags and second copy flag*/
	DEBIT_FLAG      = FALSE;
	SECOND_COPY     = FALSE;
	
	/* debit prefix check */
	DEBIT_FLAG = Utils_Debit_Check();

	if(input_type !=INPUT_FROM_KEYPAD)
	{
		
#ifndef _8010
		if( (Test_IsEeprom() > 0) && (!EXTERNAL_PINPAD) )
		Printer_WriteStr("8010\n");
#else
		Printer_WriteStr("8000\n");
		if(TRUE)
#endif
		{
			if(Verde_AskPIN_Embedded (encrypted_PIN) != STAT_OK)
			{
				Mainloop_DisplayWait("EMBEDDED PINPAD SIF. SORMA HATA ",1);      
				return(STAT_NOK);
			}
		}
		else{

 			/* þifre girisi */	
			GrphText_Cls (TRUE);
			Debug_GrafPrint2(" ž˜FREY˜ G˜R˜N ?",3);

			/*Select Which Master Key is used */
			if(Utils_SelectMasterKey () != STAT_OK)
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("  ž˜FRE C˜HAZI    BA¦LI DE¦˜L   ",2);
				return(STAT_NOK);
			}
			if(Host_PinpadIdlePrompt() != STAT_OK)
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("  ž˜FRE CÝHAZI    BA¦LI DE¦˜L   ",2);
				return(STAT_NOK);
			}

			/*Get the PIN from PINPAD and prepare the PIN Block */
			if(Trans_AskPin (encrypted_PIN) != STAT_OK)
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("SIF. SORMA HATA",1);      
				return(STAT_NOK);
			}
		}
	}
	else
	{
		memset(encrypted_PIN, '0', sizeof(encrypted_PIN));
		encrypted_PIN[16] = 0;
	}

	/* belge no girisi */
	if(Trans_GetBelgeNo()!= STAT_OK) return(STAT_NOK);

	/* tutar girisi sadece pesin satis icin gecerlidir*/
	memset(amount_genius, 0, sizeof(amount_genius));

	if (!Trans_GetGeniusAmount(gtran_type))
	{
		return(STAT_NOK);
	}
	
	/* Eger transaciton tipi vadeli satis ise */
	if(gtran_type == GENIUS_VADELI)
	{
		if(Trans_GetGeniusTutar_Vade(vade_sayisi) != STAT_OK) return(STAT_NOK);
	}

	if(gtran_type == GENIUS_SATIS)
	{
		if(Trans_GAskConfirmation() != STAT_OK) return(STAT_NOK);
	}
	else
	{
		mev_hes_kodu = '0';
	}

	/* initialize ilk telefon arama flag to false */
	FIRST_PHONE_FLAG         = FALSE;
	SERVER_OPEN_TIMEOUT_FLAG = FALSE;

	/* Provide the connection with HOST. Host Predial had been provided beforehand */

	Printer_WriteStr("going host_connect\n");
	
	
	Transaction_Ret = Host_Connect(TRUE);

	
	if (Transaction_Ret ==STAT_OK) Printer_WriteStr("Connection is ok\n");
	
	/* eger server time out oldu ise ve birinci tel arandý ise*/
	if ((Transaction_Ret !=STAT_OK) && (FIRST_PHONE_FLAG))
	{
			Host_Disconnect();
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("2. SERVER ARAMA ",0);
			Mte_Wait(2500); /* disconnection ýn saðlanmasý icin bekleniyor */

			memcpy(tel_no_1_temp, tel_no_1, strlen(tel_no_1));
			tel_no_1_temp[strlen(tel_no_1)] = 0;

			memcpy(tel_no_2_temp, tel_no_2, strlen(tel_no_2));
			tel_no_2_temp[strlen(tel_no_2)] = 0;

			memcpy(tel_no_1, tel_no_2, strlen(tel_no_2));
			tel_no_1[strlen(tel_no_2)] = 0;

			tel_no_2[0] = 0;

			SERVER_OPEN_TIMEOUT_FLAG = TRUE;

			/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
			/*Host_PreDial();
			if(Host_PreDial() != STAT_OK) 
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
				return(STAT_NOK);
			}*/
	

			Mte_Wait(2000);
			Transaction_Ret = Host_Connect(TRUE);

			/* tel no larý eski haline getiriliyor*/
			memcpy(tel_no_1, tel_no_1_temp, strlen(tel_no_1_temp));
			tel_no_1[strlen(tel_no_1_temp)] = 0;

			memcpy(tel_no_2, tel_no_2_temp, strlen(tel_no_2_temp));
			tel_no_2[strlen(tel_no_2_temp)] = 0;
	}

	if(Transaction_Ret != STAT_OK)
	{
		Host_Disconnect();
		return(Transaction_Ret);
	}

	/* 04_03 19/11/2002 send reversal if necessary */
	if(Trans_SendReversal(NORMAL_REVERSAL, 0, FALSE) != STAT_OK) return (STAT_NOK);

	EmvIF_ClearDsp(DSP_MERCHANT);
	Debug_GrafPrint2("YANIT BEKL˜YOR.. ",3);

	SORGULAMA = TRUE;		/* sorgulama = TRUE gerçek transaction = FALSE */

	/* 04_03 21/11/2002 . genius islemler icin islem sira no trx oncesinde artiriliyor */
	genius_seq_no++;

	/* Prepare the genius transaction Message*/
	if(Trans_GeniusFillTransactionSend(gtran_type)!= STAT_OK) return (STAT_NOK);

	memcpy (lbSend_Buffer,&d_GTransaction_Send,sizeof(d_GTransaction_Send));

	lbSend_Buffer[sizeof(d_GTransaction_Send)]=0;

	/* Send the message to the Host and get the response*/
/*	Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);*/
	if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
	else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);

	
	ReversalPending=FALSE; //MK Reversal zaten gönderilmis oluyor Sorgulama reversal'i set edemiyor
	
	if(Transaction_Ret != STAT_OK)
	{
		return(Transaction_Ret);
	}

	memcpy(&d_GTransaction_Receive,Received_Buffer+1,sizeof(d_GTransaction_Receive));

	/* Analyze the response*/
	ret_code = Trans_GAnalyseSaleTransactionReceive();

	if (ret_code == PIN_REQUESTED_MESSAGE)
	{
		/*DEBIT_FLAG = TRUE;*/
		/* GETPIN FUNCTION */
		/* verde . 07/11/2002 check that embedded pin pad is available */
#ifndef _8320
		if( (Test_IsEeprom() > 0) && (!EXTERNAL_PINPAD) )
#else
		if(TRUE)
#endif
		{	/* embedded pinpad is available and external pinpad is not required*/
			if(Verde_AskPIN_Embedded (encrypted_PIN) != STAT_OK)
			{
				Mainloop_DisplayWait("EMBEDDED PINPAD SIF. SORMA HATA ",1);      
				return(STAT_NOK);
			}
		}
		else
		{/* embedded pinpad is not available . external pinpad must be used*/

		   /* GETPIN FUNCTION */
		   GrphText_Cls (TRUE);
		   Debug_GrafPrint2(" ž˜FREY˜ G˜R˜N ?",3);

		   /*Select Which Master Key is used */
		   if(Utils_SelectMasterKey () != STAT_OK)
		   {
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  MASTER KEY      YUKLENMEMIS   ",1);
		   }

		   /*Get the PIN from PINPAD and prepare the PIN Block */
		   if(Trans_AskPin (encrypted_PIN) != STAT_OK)
		   {
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("ž˜FRE SORMA HATASI",2);
		   }
		}
		memcpy(d_GTransaction_Send.encrypted_PIN, encrypted_PIN, 16);
		memset(lbSend_Buffer,0,800);

		memcpy (lbSend_Buffer,&d_Transaction_Send,sizeof(d_Transaction_Send));    /* UPDATE_0_12 20/09/99 */
        	lbSend_Buffer[sizeof(d_Transaction_Send)]=0;
		/* RESend the message to the Host and get the response*/
		EmvIF_ClearDsp(DSP_MERCHANT);
		Debug_GrafPrint2("YANIT BEKL˜YOR.. ",3);
/*		Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);*/
		if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
		else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);

		if(Transaction_Ret != STAT_OK)
		{ 
			return(Transaction_Ret);
		}

		/* Move Received_Buffer to the Transaction_Receive */
		memcpy(&d_GTransaction_Receive, Received_Buffer+1, sizeof(d_GTransaction_Receive));

		ret_code = Trans_GAnalyseSaleTransactionReceive();
	}
	switch (ret_code)
	{
		case UNEXPECTED_MESSAGE :       return(STAT_NOK);
									
		
		case APPROVED_MESSAGE   :		/*approved inquiry transaction*/

										if(!Slips_GFillSorgulamaReceiptStructure())   
										{
											EmvIF_ClearDsp(DSP_MERCHANT);
											Mainloop_DisplayWait("Slips_GFillSorgulamaReceiptStructure hatasi",2);
											return(STAT_NOK);
										}

										
										Slips_GPrintSorgulama_Slip();

										Mte_Wait(1000);	/* wait for printing sorgulama slip */


										if(Trans_GAsk_Confirmation() != STAT_OK) return (STAT_DECLINED);


										return(Trans_GTrans2(gtran_type));

		case PIN_REQUESTED_MESSAGE: 
									EmvIF_ClearDsp(DSP_MERCHANT);
									Mainloop_DisplayWait("Y˜NE PIN ˜STEND˜",2);
									return(STAT_NOK);

		case REJECTED_MESSAGE   :	
							if(!Slips_GFillErrorReceiptStructure())
									{
										EmvIF_ClearDsp(DSP_MERCHANT);
										Mainloop_DisplayWait("Slips_GFillErrorReceiptStructure hatasi",3);
										return(STAT_NOK);
									}

									Slips_GPrintErrorReceipt();
									ERROR_RECEIPT = TRUE;
										
									break;
	}

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GetGeniusAmount and the number of Genius installment
 *
 * DESCRIPTION          : Gets the amount  and the # of Genius installment 
 *
 * RETURN               : TRUE or FALSE
 *
 * NOTES                : r.i.o 27/09/2002
 * ------------------------------------------------------------------------ */
boolean Trans_GetGeniusAmount(int gtran_type)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	boolean GENIUS_COUNT_OK;
	char value_null[2];
	int YTL_Digit, i;

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));
	
	memset(Buffer, 0 , LEN_GETKEYBOARD);

	if(gtran_type == GENIUS_SATIS)
	{	/* pesin satýþ durumu*/
		/* Get Genius Amount */
		for (i=0; i<12; i++)
		{
			memset(d_GTransaction_Send.Gtarih_vade[i].vade_tutari,'0',13);
			memset(d_GTransaction_Send.Gtarih_vade[i].vade_tarihi,'0',8);
		}
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) ) YTL_Digit = 10;
		else YTL_Digit = 11;
		if ( !Mainloop_GetKeyboardInput
			 ( ( char * ) "TUTARI GIRINIZ ?", Buffer, 0, YTL_Digit,
			   FALSE, FALSE, FALSE, TRUE, FALSE, &Len, value_null,
			   FALSE ) )
		{
			return ( FALSE );
		}
		else
		{
			memcpy(amount_genius,Buffer,Len);
			amount_genius[Len]=0;
			Utils_LeftPad(amount_genius, '0', 13);
		}
	}
	else
	{	/*vadeli satýþ durumu */
		memcpy(amount_genius, "0000000000000",13);
		amount_genius[13] = 0;
	}
	
	/* Initialize the vade_sayisi*/
	vade_sayisi=1;
	strcpy(vade_sayisi_c,"01");
	vade_sayisi_c[2] = 0;

	/* Eger vadeli satis ise Get the Genius_Count (# of installment) */
	if(gtran_type == GENIUS_VADELI)
	{
		GENIUS_COUNT_OK = TRUE;
		do
		{
			if(!Mainloop_GetKeyboardInput("VADE SAYISI ?",Buffer,0,2,
					   FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE))
			{
					return(FALSE);
			}
			else
			{
				if(Len == 0)
				{
					vade_sayisi = 1;
					Utils_IToA(vade_sayisi,vade_sayisi_c);      
					GENIUS_COUNT_OK = FALSE;
				}
				else
				{
					memcpy(vade_sayisi_c,Buffer,Len);
					vade_sayisi_c[Len] = 0;
					vade_sayisi = atoi(vade_sayisi_c);
					if ((vade_sayisi > 12) || (vade_sayisi <= 0))
					{
						GENIUS_COUNT_OK = TRUE;
					}
					else
					{
						GENIUS_COUNT_OK = FALSE;
					}
				}
			}
			Utils_LeftPad(vade_sayisi_c, '0', 2);

		}while(GENIUS_COUNT_OK); /* 02_01 irfan 08/09/2000 taksit sayýsý sorma pinpad den */
	}

	return(TRUE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GetGeniusTutar_Vade
 *
 * DESCRIPTION          : Gets the installment amount  and  installment dates
 *
 * RETURN               : STAT_OK or STAT_NOK
 *
 * NOTES                : r.i.o 02/10/2002
 * ------------------------------------------------------------------------ */
byte Trans_GetGeniusTutar_Vade(int Vade_count)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	boolean GENIUS_COUNT_OK;
	char value_null[2];
	int i, YTL_Digit;
	char lbtemp2[50];
	char lbtemp[50];
	boolean DATE_ENTRY_OK;
	char lbtemp_month[3];
	char lbtemp_day[3];
	int lbtemp_month_value, lbtemp_day_value;

	memset(value_null, 0 , sizeof(value_null));
	memset(Buffer, 0 , LEN_GETKEYBOARD);

	/* vade tutar ve tarihlerin alýnmasý */
	for (i=0; i<Vade_count; i++)
	{
		memset(lbtemp2,0,sizeof(lbtemp2));
		sprintf(lbtemp2,"%d.TUTARI GIRIN?",(i+1));

		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) ) YTL_Digit = 10;
		else YTL_Digit = 11;
		if ( !Mainloop_GetKeyboardInput
			 ( lbtemp2, Buffer, 0, YTL_Digit,
			   FALSE, FALSE, FALSE, TRUE, FALSE, &Len, value_null,
			   FALSE ) )
		{
			Mainloop_DisplayWait("TUTAR GIRIS HATASI",1);
			return(STAT_NOK);
		}
		else
		{
			memset(lbtemp,0,sizeof(lbtemp));
			memcpy(lbtemp ,Buffer,Len);
			lbtemp[Len]=0;
			Utils_LeftPad(lbtemp, '0', 13);
			memcpy(d_GTransaction_Send.Gtarih_vade[i].vade_tutari,lbtemp,13);
			if(DEBUG_MODE)
			{
				Printer_WriteStr("\nTutar\n");
				PrntUtil_BufAsciiHex((byte*)d_GTransaction_Send.Gtarih_vade[i].vade_tutari,13);
			}
		}

		memset(lbtemp2,0,sizeof(lbtemp2));
		sprintf(lbtemp2,"%d.VADE TARIHINI GIRIN[GGAAYYYY]?",(i+1));
		DATE_ENTRY_OK = FALSE;
		do
		{
			if(!Mainloop_GetKeyboardInput(lbtemp2,Buffer,0,8,
					   FALSE, FALSE, FALSE, FALSE, FALSE,&Len, " ", TRUE))
			{
				Mainloop_DisplayWait("VADE GIRIS HATASI",1);
				return(STAT_NOK);
			}
			else
			{
				if(Len == 0)
				{
					memcpy(lbtemp, "00000000", 8);
					lbtemp[8] = 0;

					DATE_ENTRY_OK = TRUE;
				}
				else
				{
					lbtemp_month[0] = Buffer[2];
					lbtemp_month[1] = Buffer[3];
					lbtemp_month[2] = 0;
					lbtemp_month_value = atoi(lbtemp_month);

					lbtemp_day[0] = Buffer[0];
					lbtemp_day[1] = Buffer[1];
					lbtemp_day[2] = 0;
					lbtemp_day_value = atoi(lbtemp_day);


					if ((lbtemp_month_value <=12 ) && (lbtemp_month_value >= 1) && 
						(lbtemp_day_value <=31   ) && (lbtemp_day_value >= 1  )    ) 
					{
						if (lbtemp_month_value == 2)
						{
							if (lbtemp_day_value > 29)
							{
								DATE_ENTRY_OK = FALSE;
							}
							else
							{
								memcpy(lbtemp ,Buffer,Len);
								lbtemp[8]=0;
								DATE_ENTRY_OK = TRUE;
							}
						}
						else
						{
							memcpy(lbtemp ,Buffer,Len);
							lbtemp[8]=0;
							DATE_ENTRY_OK = TRUE;
						}
					}
					else
					{
						DATE_ENTRY_OK = FALSE;
					}
					

				}
			}
		}while(!DATE_ENTRY_OK);

		memcpy(d_GTransaction_Send.Gtarih_vade[i].vade_tarihi,lbtemp,8);
/*		if(DEBUG_MODE){
			Printer_WriteStr("\nTarih\n");
			PrntUtil_BufAsciiHex((byte*)d_GTransaction_Send.Gtarih_vade[i].vade_tarihi,8);				
		}*/

/*		if(!Mainloop_GetKeyboardInput(lbtemp2,Buffer,0,8,
						   FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
		{
			Mainloop_DisplayWait("VADE GIRIS HATASI",1);
			return(STAT_NOK);
		}
		else
		{
			memset(lbtemp,0,sizeof(lbtemp));

			if(Len == 0)
			{
				memcpy(lbtemp, "00000000", 8);
				lbtemp[8] = 0;
			}
			else
			{
				memcpy(lbtemp ,Buffer,Len);
				lbtemp[8]=0;
			}
				
			memcpy(d_GTransaction_Send.Gtarih_vade[i].vade_tarihi,lbtemp,8);
		}*/
/*		if(DEBUG_MODE){
			Printer_WriteStr("\ntarih_vade\n");
			PrntUtil_BufAsciiHex((byte*)&d_GTransaction_Send.Gtarih_vade[i],21);				
		}*/
	}

	return(STAT_OK);
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Trans_GetBelgeNo
 *
 * DESCRIPTION		: 
 *
 * RETURN			:
 *
 * NOTES			:
 *
 * ------------------------------------------------------------------------ */
byte Trans_GetBelgeNo(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];
	char lbtemp[50];

	memset(value_null, 0 , sizeof(value_null));
	memset(Buffer, 0 , LEN_GETKEYBOARD);

	memset(belge_no, 0, sizeof(belge_no));

	if(!Mainloop_GetKeyboardInput("BELGE NO ?",Buffer,0,10,
		       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
	{
		Mainloop_DisplayWait("GIRIS HATASI...",1);
		return(STAT_NOK);
	}
	else
	{	/* 04_04 22/11/2002. talep uzerine 10 digitden az girilebilme imkaný verildi. */
		if(Len == 0)
		{
			Utils_LeftPad(lbtemp, '0', 10);
		}
		else
		{
			memset(lbtemp,0,sizeof(lbtemp));
			memcpy(lbtemp ,Buffer,Len);
			lbtemp[Len]=0;
			Utils_LeftPad(lbtemp, '0', 10);
		}
	}    
	memcpy(belge_no ,lbtemp,10);
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Trans_GAskConfirmation
 *
 * DESCRIPTION		: 
 *
 * RETURN			:
 *
 * NOTES			:
 *
 * ------------------------------------------------------------------------ */
byte Trans_GAskConfirmation(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];
	char lbtemp[50];

	memset(value_null, 0 , sizeof(value_null));
	memset(Buffer, 0 , LEN_GETKEYBOARD);

	for(;;)
	{
		Kb_Read();
		/*if(!Mainloop_GetKeyboardInput("DIREKT OLARAK KREDI HESABININ DEVREYE GIRMESI ICIN SECINIZ (E:1/H:0)",Buffer,1,1,
				   FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))*/
		/*if(!Mainloop_GetKeyboardInput("KREDI HSB.DEVRE.GIRMESI E:1/H:0 ",Buffer,1,1,
				   FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE)) */
		/* 04_07 A.Y. 18/12/2002 */
	   if(!Mainloop_GetKeyboardInput("DIREKT KREDI KULLANIMI E:1/H:0 ",Buffer,1,1,
				   FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))

		{
			Mainloop_DisplayWait("GIRIS HATASI",1);
			return(STAT_NOK);
		}
		else
		{
			if(Buffer[0] == '0')
			{
				mev_hes_kodu = '0';
				break;
			}

			if(Buffer[0] == '1')
			{
				mev_hes_kodu = '1';
				break;
			}

		}    
	}
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GeniusFillTransactionSend
 *
 * DESCRIPTION          : 
 *
 * RETURN                       :
 *
 * NOTES                        :
 *
 * REVISED ON           ; 27/09/02 BY: REFIK IRFAN OZTURK
 * ------------------------------------------------------------------------ */
byte Trans_GeniusFillTransactionSend(int gtran_type)
{
	char device_date[8], device_time[6];

	/* FILL  THE  HEADER  PART */
	memset(&d_GTransaction_Send, ' ', sizeof(d_GTransaction_Send)-sizeof(d_GTransaction_Send.Gtarih_vade));
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) d_GTransaction_Send.GHeader_Transact.Cur_Index = 'X';
	else d_GTransaction_Send.GHeader_Transact.Cur_Index = 'Y';
	d_GTransaction_Send.GHeader_Transact.ActAppCode = POS_Type;
	d_GTransaction_Send.GHeader_Transact.SecAppCode = '1';	/* 0-nobiz, 1-bizpos */
	memcpy(d_GTransaction_Send.GHeader_Transact.PosType, "LIP", 3);
	memcpy(d_GTransaction_Send.GHeader_Transact.PosModel, "8000", 4);
	memcpy(d_GTransaction_Send.GHeader_Transact.PosVersion, POS_VERSION_NUMBER, 4);
	if (Cashier_Flag == 2) memset(d_GTransaction_Send.GHeader_Transact.Cashier_Id, ' ', 16 );
	else memcpy(d_GTransaction_Send.GHeader_Transact.Cashier_Id, Kasiyer_No, 16 );
	memcpy(d_GTransaction_Send.GHeader_Transact.IMEI_ID, Kampanya_Data, 15 );
	memset(d_GTransaction_Send.GHeader_Transact.Filler, ' ', 19 );
	Utils_GetDateTime( device_date, device_time );

	d_GTransaction_Send.GHeader_Transact.PosDate[0] = device_date[6];
	d_GTransaction_Send.GHeader_Transact.PosDate[1] = device_date[7];
	d_GTransaction_Send.GHeader_Transact.PosDate[2] = device_date[3];
	d_GTransaction_Send.GHeader_Transact.PosDate[3] = device_date[4];
	d_GTransaction_Send.GHeader_Transact.PosDate[4] = device_date[0];
	d_GTransaction_Send.GHeader_Transact.PosDate[5] = device_date[1];
	d_GTransaction_Send.GHeader_Transact.PosTime[0] = device_time[0];
	d_GTransaction_Send.GHeader_Transact.PosTime[1] = device_time[1];
	d_GTransaction_Send.GHeader_Transact.PosTime[2] = device_time[3];
	d_GTransaction_Send.GHeader_Transact.PosTime[3] = device_time[4];

	d_GTransaction_Send.GHeader_Transact.MSG_INDICATOR = 'G';
	d_GTransaction_Send.GHeader_Transact.MSG_TYPE = gtran_type + '0';
	d_GTransaction_Send.GHeader_Transact.POS_software_ver = POS_software_ver;
	memcpy(d_GTransaction_Send.GHeader_Transact.merch_device_no , merch_device_no , 8);
	d_GTransaction_Send.GHeader_Transact.device_serial_no = device_serial_no;
	
	/* sorgulama */
	d_GTransaction_Send.GHeader_Transact.transaction_type = '1';
	memcpy(d_GTransaction_Send.available_balance, "0000000000000", 13);
	memcpy(d_GTransaction_Send.bayii_garantisiz_borc, "0000000000000", 13);
	memcpy(d_GTransaction_Send.kart_islem_toplami, "0000000000000", 13);
	memcpy(d_GTransaction_Send.kullanilan_kredi_limiti, "0000000000000", 13);

	/*FILL THE DETAIL */

	Utils_IToA(batch_no,batch_no_c);
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(d_GTransaction_Send.batch_no, batch_no_c,3);

	/* 04_00 genius */
	Utils_IToA(genius_seq_no,genius_seq_no_c);
	Utils_LeftPad(genius_seq_no_c, '0', 4);
	memcpy(d_GTransaction_Send.transaction_seq_no, genius_seq_no_c,4);

	memset(d_GTransaction_Send.card_info, ' ', sizeof(d_GTransaction_Send.card_info));
	d_GTransaction_Send.input_type = input_type;
	
	if (d_GTransaction_Send.input_type == INPUT_FROM_READER)  
	{
		memcpy(d_GTransaction_Send.card_info,card_info,strlen(card_info));
	}
	else
	{
		memcpy(d_GTransaction_Send.card_info, card_no,strlen(card_no));
		memcpy(&d_GTransaction_Send.card_info[19], exp_date,4);
		memcpy(&d_GTransaction_Send.card_info[23], cvv_2,3);
	}

	/* pinpad status */
	d_GTransaction_Send.PINPad_status = PINPad_status;       

	/* encrypted pin */
	if(input_type ==INPUT_FROM_KEYPAD)
	{
		memset(d_GTransaction_Send.encrypted_PIN, '0', sizeof(d_GTransaction_Send.encrypted_PIN));
	}
	else
	{
		memcpy(d_GTransaction_Send.encrypted_PIN, encrypted_PIN, 16);
	}

	memset(d_GTransaction_Send.approval_code,' ', sizeof(d_GTransaction_Send.approval_code));

	/* sorgulama tutarý */
	memcpy(d_GTransaction_Send.sorgulama_tutar, amount_genius, 13);
	
	/* belge no */
	memcpy(d_GTransaction_Send.belge_no, belge_no, 10);
	
	/* mevduat hesap kodu */
	d_GTransaction_Send.mev_hes_kodu = mev_hes_kodu;


	/* vade sayýsý */
	memcpy(d_GTransaction_Send.vade_sayisi, vade_sayisi_c , 2);

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GeniusFillTransactionSend2
 *
 * DESCRIPTION          : 
 *
 * RETURN                       :
 *
 * NOTES                        :
 *
 * REVISED ON           ; 27/09/02 BY: REFIK IRFAN OZTURK
 * ------------------------------------------------------------------------ */
byte Trans_GeniusFillTransactionSend2(int gtran_type)
{
	/* gerçek transaction */
	d_GTransaction_Send.GHeader_Transact.transaction_type = '2';
	memcpy(d_GTransaction_Send.available_balance, d_GTransaction_Receive.available_balance, 13);
	memcpy(d_GTransaction_Send.bayii_garantisiz_borc, d_GTransaction_Receive.bayii_garantisiz_borc, 13);
	memcpy(d_GTransaction_Send.kart_islem_toplami, d_GTransaction_Receive.kart_islem_toplami, 13);
	memcpy(d_GTransaction_Send.kullanilan_kredi_limiti, d_GTransaction_Receive.kullanilan_kredi_limiti, 13);
	/* 04_05 sorgulama da gelen approval code 2. req. mesajina aktariliyor */
	memcpy(d_GTransaction_Send.approval_code, d_GTransaction_Receive.approval_code, 6);

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GAnalyseSaleTransactionReceive.
 *
 * DESCRIPTION          : Analyze the receive message
 *
 * RETURN               : ret_code
 *
 * NOTES                : r.i.o  02/10/2002
 *
 * ------------------------------------------------------------------------ */
int Trans_GAnalyseSaleTransactionReceive(void)
{
	char msg[17];
	short int ret;
	char balance_msg[21];
	char lbtemp_string[100];
	char tempch,tempch2;

	tempch = d_GTransaction_Receive.GHeader_Transact.Cur_Index;
	tempch2 = d_GTransaction_Receive.GHeader_Transact.MSG_INDICATOR;

	if (
		(d_GTransaction_Receive.GHeader_Transact.MSG_TYPE != '1' &&
		d_GTransaction_Receive.GHeader_Transact.MSG_TYPE != '2' &&
		d_GTransaction_Receive.GHeader_Transact.MSG_TYPE != '3' &&
		d_GTransaction_Receive.GHeader_Transact.MSG_TYPE != '4'  &&
		d_GTransaction_Receive.GHeader_Transact.MSG_TYPE != '5'  )	/* 04_16 06/08/2003 irfan biz bakiye sorgulama */
		||
		((tempch != 'X') && (tempch != 'Y'))		
		||
		((tempch2 != 'G') && (tempch2 != 'H'))
		)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("MESAJ T˜P˜ FARKLI");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("MESAJ T˜P˜ FARKLI",1);
		return (UNEXPECTED_MESSAGE);
	}
	
	/* check for batch no */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, d_GTransaction_Receive.batch_no, 3);
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\nbatch_no:");
		Slips_PrinterStr(lbtemp_string);
	}

	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("BATCHNO HATASI   ");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (UNEXPECTED_MESSAGE);
	}
	
	/* check for seq_no */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, d_GTransaction_Receive.transaction_seq_no, 4);
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\nTransaction_seq_no:");
		Slips_PrinterStr(lbtemp_string);
	}

	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("SEQ NO  HATASI   ");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (UNEXPECTED_MESSAGE);
	}

	/* check for DATE*/
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, d_GTransaction_Receive.date, 6);

	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\ndate:");
		Slips_PrinterStr(lbtemp_string);
	}

	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("DATE    HATASI   ");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (UNEXPECTED_MESSAGE);
	}
	
	/* check for TIME*/
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, d_GTransaction_Receive.time, 4);
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\ntime:");
		Slips_PrinterStr(lbtemp_string);
	}

	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("TIME    HATASI   ");
		PrntUtil_Cr(7);
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (UNEXPECTED_MESSAGE);
	}


	/* eger conf. code 800 veya 899 dan farkli ise kalan field'larin kontrol edilmesine gerek yok */
	if ( (memcmp(d_GTransaction_Receive.confirmation, "800", 3) == 0) ||
		 (memcmp(d_GTransaction_Receive.confirmation, "899", 3) == 0) )
	{
		/* check for BAYII NO*/
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, d_GTransaction_Receive.bayii_no, 7);
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nbayii_no:");
			Slips_PrinterStr(lbtemp_string);
		}

		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("BAYII NO HATASI ");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (UNEXPECTED_MESSAGE);
		}


		/* check for SORGULAMA TUTARI*/
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, d_GTransaction_Receive.sorgulama_tutar, 13);
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nsorgulama_tutar:\n");
			Slips_PrinterStr(lbtemp_string);
		}

		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("SORGULAMA TUTAR HATASI");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (UNEXPECTED_MESSAGE);
		}

		/* check for GARANTI   TUTARI*/
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, d_GTransaction_Receive.garanti_tutar, 13);
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\ngaranti_tutar:\n");
			Slips_PrinterStr(lbtemp_string);
		}

		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("GARANTI   TUTAR HATASI");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (UNEXPECTED_MESSAGE);
		}

		/* check for GARANTI ASIM TUTARI*/
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, d_GTransaction_Receive.garanti_asim_tutari,13);
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\ngaranti_asim_tutari:\n");
			Slips_PrinterStr(lbtemp_string);
		}

		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("GARANTI ASIM    TUTAR HATASI");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (UNEXPECTED_MESSAGE);
		}

		/* check for KULLANILAN KREDI LIMITI*/
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, d_GTransaction_Receive.kullanilan_kredi_limiti,13);
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nkullanilan_kredi_limiti:\n");
			Slips_PrinterStr(lbtemp_string);
		}

		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("KULLANILAN KREDILIMITI HATASI");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (UNEXPECTED_MESSAGE);
		}

		/* check for AVAILABLE BALANCE*/
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, d_GTransaction_Receive.available_balance,13);
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\navailable_balance:\n");
			Slips_PrinterStr(lbtemp_string);
		}

		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("AVAILABLE BALANCE HATASI");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (UNEXPECTED_MESSAGE);
		}

		/* check for BAYII GARANTISIZ BORC*/
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, d_GTransaction_Receive.bayii_garantisiz_borc,13);
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nbayii_garantisiz_borc:\n");
			Slips_PrinterStr(lbtemp_string);
		}

		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("BAYII GARANTISIZ BORC HATASI");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (UNEXPECTED_MESSAGE);
		}

		/* check for KART ISLEM TOPLAMI*/
		memset(lbtemp_string,0,sizeof(lbtemp_string));
		memcpy(lbtemp_string, d_GTransaction_Receive.kart_islem_toplami,13);
		if(DEBUG_MODE)
		{
			Slips_PrinterStr("\nkart_islem_toplami:\n");
			Slips_PrinterStr(lbtemp_string);
			PrntUtil_Cr(3);
		}

		if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
		{
			PrntUtil_Cr(2);
			Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
			Slips_PrinterStr("KART ISLEM TOPLAMI HATASI");
			PrntUtil_Cr(7);
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
			return (UNEXPECTED_MESSAGE);
		}
	}


	if(SORGULAMA)
	{	/* sorgulama */
		if (memcmp(d_GTransaction_Receive.confirmation, "800", 3) == 0)
		{
			/* transaction has been approved*/
			ret = APPROVED_MESSAGE;
		}
		else if ( ( memcmp(d_Transaction_Receive.confirmation, "156", 3) == 0 )  ||
				( memcmp(d_Transaction_Receive.confirmation, "157", 3) == 0 ) )
		{
			/* Enter PIN and resend the message*/
			ret = PIN_REQUESTED_MESSAGE;
		}
		else
		{
			ret = REJECTED_MESSAGE;
		}
	}
	else
	{	/* gercek transaction */
		if (memcmp(d_GTransaction_Receive.confirmation, "899", 3) == 0)
		{
			/* transaction has been approved*/
			ret = APPROVED_MESSAGE;
		}
		else if ( ( memcmp(d_Transaction_Receive.confirmation, "156", 3) == 0 )  ||
				( memcmp(d_Transaction_Receive.confirmation, "157", 3) == 0 ) )
		{
			/* Enter PIN and resend the message*/
			ret = PIN_REQUESTED_MESSAGE;
		}
		else
		{
			ret = REJECTED_MESSAGE;
		}
	}

	/* display the message from host and return*/
	memcpy(msg, d_GTransaction_Receive.message, 16);
	msg[16] = 0;

	
	/* UPDATE 01_16 30/10/99 yeniden duzenlendý. Eðer mesaj onaylanmadý ise*/
	if( ret != APPROVED_MESSAGE)
	{
		/* Eðer þifre sorma mesajý donmuþ ise */
		if(ret == PIN_REQUESTED_MESSAGE)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Debug_GrafPrint2((char *)msg, 3);
		}
		else
		{	/*eðer reject edilmiþ bir mesaj ise*/
			GrphText_Cls (TRUE);
			Debug_GrafPrint2("BASARISIZ ISLEM", 3);
			Debug_GrafPrint2((char *)msg, 3);
			Mte_Wait (600);
		}
	}
	else
	{
		/* onaylanmýþ bir mesaj durumunda onay kodu yazýsý 600 msec ekranda duracak*/
		EmvIF_ClearDsp(DSP_MERCHANT);
		Debug_GrafPrint2((char *)msg, 3);

		Mte_Wait (600);
	}


	if((ret != PIN_REQUESTED_MESSAGE )&&(ret != APPROVED_MESSAGE))        /* so far message is received from HOST. so disconnect the connection */
		Host_Disconnect();

	return (ret);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Trans_GAsk_Confirmation
 *
 * DESCRIPTION		: 
 *
 * RETURN			:
 *
 * NOTES			:
 *
 * ------------------------------------------------------------------------ */
byte Trans_GAsk_Confirmation(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];
	char lbtemp[50];

	memset(value_null, 0 , sizeof(value_null));
	memset(Buffer, 0 , LEN_GETKEYBOARD);

	for(;;)
	{
		Kb_Read();
		if(!Mainloop_GetKeyboardInput("ONAY:1 RET:0",Buffer,1,1,
				   FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, TRUE))
		{
			Mainloop_DisplayWait("SADECE IZIN VERILEN TUSLARI KULLANIN",1);
		}
		else
		{
			if(Buffer[0] == '0')
			{
				return(STAT_DECLINED);
			}

			if(Buffer[0] == '1')
			{
				return(STAT_OK);

			}

		}    
	}
}


/* --------------------------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GTrans2
 *
 * DESCRIPTION          : Transaction which handles Genius Sale, Genius Cancel, Genius Sale(installment)
 *                         Genius Sale(installment) Cancel
 *
 * RETURN                       : STAT OK, STAT_NOK
 *
 * NOTES                        : r.i.o.
 *
 * -------------------------------------------------------------------------------------------- */
byte Trans_GTrans2(int gtran_type)
{
	/* balance related variables */
	char temp_buff[17];
	char balance_message[33];
	char TransReceived_DateTime[11];
	byte Transaction_Ret=0;
	char lbtemp[50];
	char lbSend_Buffer[800];


	/* set the sorgulama flag as false to indicate that this is the real transaction */
	SORGULAMA = FALSE;
	
	/* Initialize the Send Buffer*/
	memset(lbSend_Buffer,0,sizeof(lbSend_Buffer));
	
	/* Prepare the genius transaction Message for approval*/
	if(Trans_GeniusFillTransactionSend2(gtran_type)!= STAT_OK) return (STAT_NOK);

	/* 04_03 19/11/2002 */
	memset(Reversal_Buffer,0,sizeof(Reversal_Buffer));
	memcpy (Reversal_Buffer,&d_GTransaction_Send,sizeof(d_GTransaction_Send));
	memcpy (lbSend_Buffer,&d_GTransaction_Send,sizeof(d_GTransaction_Send));
	lbSend_Buffer[sizeof(d_GTransaction_Send)]=0;
	Reversal_Buffer[sizeof(d_GTransaction_Send)]=0;

	if(DEBUG_MODE)
	{
		Printer_WriteStr("\n\n send message2:\n");
		Printer_WriteStr(lbSend_Buffer);
		memset(lbtemp, 0, sizeof(lbtemp));
		sprintf(lbtemp, "\n\nMessage Length2: %d\n\n",strlen(lbSend_Buffer));
		Printer_WriteStr(lbtemp);
	}

	/* hat koptu ise tekrar kuruluyor */
	if(!IS_HOST_CONNECTED_FLAG)
	{
		Transaction_Ret = Host_Connect(TRUE);
		if(Transaction_Ret != STAT_OK)
		{
			Host_Disconnect();
			return(Transaction_Ret);
		}
	}

	/* Send the message to the Host and get the response*/
/*	Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);*/
	if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
	else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);

	if(Transaction_Ret != STAT_OK)
	{	/* 04_14 06/06/2003 */
		Printer_WriteStr ("\nBAGLANTI HATASI");
		Printer_WriteStr ("\nTEKRAR DENEYIN\n");
		Printer_WriteStr("\n\n\n\n\n\n");		
		
		ReversalPending = TRUE;	
		if(Trans_SendReversal(NORMAL_REVERSAL, 0, FALSE) != STAT_OK) return (STAT_NOK);

		return(Transaction_Ret);
	}

	memcpy(&d_GTransaction_Receive,Received_Buffer+1,sizeof(d_GTransaction_Receive));

	/* Analyze the response*/
	ret_code = Trans_GAnalyseSaleTransactionReceive();

	switch (ret_code)
	{
		case UNEXPECTED_MESSAGE :       ReversalPending = TRUE;	/* 04_03 19/11/2002*/
										return(STAT_NOK);
									
		case APPROVED_MESSAGE   :		/*approved */

										/* 04_04 22/11/2002 vade sabitleme durumunda*/
										if ( (gtran_type == GENIUS_VADELI) && (vade_sayisi == 1) )
										{
											memcpy(d_GTransaction_Send.Gtarih_vade, d_GTransaction_Receive.vade_sabitleme_tarihi, 8);
										}

										if(!Slips_GFillReceiptStructure())   
										{
											EmvIF_ClearDsp(DSP_MERCHANT);
											Mainloop_DisplayWait("Slips_GFillReceiptStructure hatasi",2);
											ReversalPending = TRUE;	/* 04_03 19/11/2002*/
											return(STAT_NOK);
										}
									
										if(Files_GRecordWrite() != STAT_OK)
										{
											ReversalPending = TRUE;	/* 04_03 19/11/2002*/
											return(STAT_NOK);
										}

										Slips_GPrintReceipt();

										/*WARN THE USER BY SIGNAL */

										if(WARNING_FLAG)
										{
											Mte_Wait(1000);
											Kbhw_BuzzerPulse(200);

											Mte_Wait(300);
											Kbhw_BuzzerPulse(200);
										}

										SECOND_COPY = TRUE;
										
										if(SECOND_COPY)
										{
											EmvIF_ClearDsp(DSP_MERCHANT);
											Mainloop_DisplayWait(" 2. KOPYA ˜€˜N  G˜R˜ž'E BASINIZ.",10);
											Slips_GPrintReceipt();
											EmvIF_ClearDsp(DSP_MERCHANT);
										}
										ERROR_RECEIPT = FALSE;

										/* update real time clock. genius islemler icin ozellikle onay almis islemden sonra yapiliyor.
										  onaylanmayan islemlerde tarih ve saat yanlis geliyor.*/ 
										memset(TransReceived_DateTime,'0',10);  /* initialize the date and time */
										memcpy(TransReceived_DateTime,d_GTransaction_Receive.date,6);
										memcpy(&TransReceived_DateTime[6] , d_GTransaction_Receive.time,4);
										TransReceived_DateTime[10] = 0;
										
										Utils_SetDateTime(TransReceived_DateTime);

									break;

		case PIN_REQUESTED_MESSAGE: 
									EmvIF_ClearDsp(DSP_MERCHANT);
									Mainloop_DisplayWait("Y˜NE PIN ˜STEND˜",2);
									return(STAT_NOK);

		case REJECTED_MESSAGE   :   	if(!Slips_GFillErrorReceiptStructure())
										{
											EmvIF_ClearDsp(DSP_MERCHANT);
											Mainloop_DisplayWait("Slips_GFillErrorReceiptStructure hatasi",3);
											ReversalPending = TRUE;	/* 04_03 19/11/2002*/
											return(STAT_NOK);
										}
					
										Slips_GPrintErrorReceipt();
										ERROR_RECEIPT = TRUE;
									break;
	}


	/* Increment sequence number*/
	/*genius_seq_no++;*/
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GCANCEL
 *
 * DESCRIPTION          : cancellation of the genius transactions
 *
 * RETURN               :
 *
 * NOTES                : 
 *
 * DATE                 : r.i.ö. 17/06/02
 *
 * ------------------------------------------------------------------------ */
byte Trans_GCancel(int gtran_type)
{
	char entered_seq_no[5], input_card_no[20];
	char Buffer[LEN_GETKEYBOARD];
	int i;
	byte Len;
	boolean card_number_match, check_card_number;
	GTransactionStruct fpGTransStruct;
	char lbtemp[600];

	char value_null[2];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));

	if ( !INSTALLATION)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" KURULUž YAPIN  ",1);                             /* DEGISECEK_MESAJ*/
		return(STAT_NOK);
	}

/* Check for incomplete EOD transcation*/
	if (EOD_flag)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" GšNSONU YAPIN  ",1);                             /* DEGISECEK_MESAJ*/
		return(STAT_NOK);
	}

	if(Files_ReadAllParams() != STAT_OK) return (STAT_NOK);

	/* Prepare the transaction Message*/
	if(!Mainloop_GetKeyboardInput("SIRA NO?",Buffer,1,4,    /*DEGISECEK_MESAJ*/
				      FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("SIRA NO GIRIS HATASI",1);
		return(STAT_NOK);
	}
	else
	{
		memcpy(entered_seq_no,Buffer,Len);
		entered_seq_no[Len]=0;
	}

	Utils_LeftPad(entered_seq_no,'0',4);

	memcpy(fpGTransStruct.seq_no, entered_seq_no, 4);

	card_number_match = FALSE;

	if (Files_GReadTransaction(/*(struct GTransactionStruct *)*/&fpGTransStruct) == STAT_OK)
	{
		if (fpGTransStruct.void_status == '1')
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("˜PTAL ED˜LM˜ž",2);
			return(STAT_NOK);
		}

		card_number_match = TRUE;
		check_card_number = TRUE;

		memcpy(input_card_no, fpGTransStruct.card_no, 19);
		input_card_no[19] = 0;
		for (i=12; i<19; i++)
		{
			if (input_card_no[i] == ' ')
				input_card_no[i] = 0;
		}

		i = 0;
		do
		{
			if (card_no[i] != input_card_no[i])
			{
				card_number_match = FALSE;
				check_card_number = FALSE;
			}
			else if ((card_no[i] == 0) &&
				 (input_card_no[i] == 0))
			{
				check_card_number = FALSE;
			}
			i++;
		} while (check_card_number && (i<19));

		if (card_number_match)
		{
			/* belge no kontrolu */
			if(Trans_GetBelgeNo()!= STAT_OK) return(STAT_NOK);

			if(memcmp(belge_no, fpGTransStruct.belge_no, 10) != 0) 
			{
				Mainloop_DisplayWait("BELGE NO YANLIS",1);
				return(STAT_NOK);
			}

			return (Trans_GUpdate_Cancel(gtran_type,&fpGTransStruct));
		}
		else
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("KART NO YANLIž",2);
		}
	}
	else
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait("KAYIT YOK",2);
	}
	return(STAT_NOK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GUpdate_Cancel
 *
 * DESCRIPTION          : Sends void message, receives the response and analyzes it
 *
 * RETURN               : none.
 *
 * NOTES                : r.i.o  17/10/2002
 *
 * ------------------------------------------------------------------------ */
byte Trans_GUpdate_Cancel(int gtran_type, GTransactionStruct *fpGTransStruct)
{
	boolean void_msg_OK;
	byte void_ret_code;
	byte lbret_code;
	char lbSend_Buffer[800];
	char device_date[8], device_time[6];

	void_msg_OK = FALSE;


	/* pin girisi */
	if(input_type !=INPUT_FROM_KEYPAD)
	{
#ifndef _8010
		if( (Test_IsEeprom() > 0) && (!EXTERNAL_PINPAD) )
#else
		if(TRUE)
#endif
		{
			if(Verde_AskPIN_Embedded (encrypted_PIN) != STAT_OK)
			{
				Mainloop_DisplayWait("EMBEDDED PINPAD SIF. SORMA HATA ",1);      
				return(STAT_NOK);
			}
		}
		else{
			/* þifre girisi */	
			GrphText_Cls (TRUE);
			Debug_GrafPrint2(" ž˜FREY˜ G˜R˜N ?",3);

			/*Select Which Master Key is used */
			if(Utils_SelectMasterKey () != STAT_OK)
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("  ž˜FRE C˜HAZI    BA¦LI DE¦˜L   ",2);
				return(STAT_NOK);
			}
			if(Host_PinpadIdlePrompt() != STAT_OK)
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("  ž˜FRE CÝHAZI    BA¦LI DE¦˜L   ",2);
				return(STAT_NOK);
			}

			/*Get the PIN from PINPAD and prepare the PIN Block */
			if(Trans_AskPin (encrypted_PIN) != STAT_OK)
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("SIF. SORMA HATA",1);      
				return(STAT_NOK);
			}
		}
	}
	else
	{
		memset(encrypted_PIN, '0', sizeof(encrypted_PIN));
		encrypted_PIN[16] = 0;
	}

/**************************************************************************************/
	/* FILL  THE  HEADER  PART */
	memset(&d_GTransaction_Send, ' ', sizeof(d_GTransaction_Send));
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) d_GTransaction_Send.GHeader_Transact.Cur_Index = 'X';
	else d_GTransaction_Send.GHeader_Transact.Cur_Index = 'Y';
	d_GTransaction_Send.GHeader_Transact.ActAppCode = POS_Type;
	d_GTransaction_Send.GHeader_Transact.SecAppCode = '1';	/* 0-nobiz, 1-bizpos */
	memcpy(d_GTransaction_Send.GHeader_Transact.PosType, "LIP", 3);
	memcpy(d_GTransaction_Send.GHeader_Transact.PosModel, "8000", 4);
	memcpy(d_GTransaction_Send.GHeader_Transact.PosVersion, POS_VERSION_NUMBER, 4);
	if (Cashier_Flag == 2) memset(d_GTransaction_Send.GHeader_Transact.Cashier_Id, ' ', 16 );
	else memcpy(d_GTransaction_Send.GHeader_Transact.Cashier_Id, Kasiyer_No, 16 );
	memcpy(d_GTransaction_Send.GHeader_Transact.IMEI_ID, Kampanya_Data, 15 );
	memset(d_GTransaction_Send.GHeader_Transact.Filler, ' ', 19 );
	Utils_GetDateTime( device_date, device_time );
	{
		d_GTransaction_Send.GHeader_Transact.PosDate[0] = device_date[6];
		d_GTransaction_Send.GHeader_Transact.PosDate[1] = device_date[7];
		d_GTransaction_Send.GHeader_Transact.PosDate[2] = device_date[3];
		d_GTransaction_Send.GHeader_Transact.PosDate[3] = device_date[4];
		d_GTransaction_Send.GHeader_Transact.PosDate[4] = device_date[0];
		d_GTransaction_Send.GHeader_Transact.PosDate[5] = device_date[1];
		d_GTransaction_Send.GHeader_Transact.PosTime[0] = device_time[0];
		d_GTransaction_Send.GHeader_Transact.PosTime[1] = device_time[1];
		d_GTransaction_Send.GHeader_Transact.PosTime[2] = device_time[3];
		d_GTransaction_Send.GHeader_Transact.PosTime[3] = device_time[4];
	} 

	d_GTransaction_Send.GHeader_Transact.MSG_INDICATOR = 'G';
	d_GTransaction_Send.GHeader_Transact.MSG_TYPE = gtran_type + '0';
	d_GTransaction_Send.GHeader_Transact.POS_software_ver = POS_software_ver;
	memcpy(d_GTransaction_Send.GHeader_Transact.merch_device_no , merch_device_no , 8);
	d_GTransaction_Send.GHeader_Transact.device_serial_no = device_serial_no;
	
	d_GTransaction_Send.GHeader_Transact.transaction_type = '2';
	memcpy(d_GTransaction_Send.available_balance, fpGTransStruct->available_balance, 13);
	memcpy(d_GTransaction_Send.bayii_garantisiz_borc, fpGTransStruct->bayii_garantisiz_borc, 13);
	memcpy(d_GTransaction_Send.kart_islem_toplami, fpGTransStruct->kart_islem_toplami, 13);
	memcpy(d_GTransaction_Send.kullanilan_kredi_limiti, fpGTransStruct->kullanilan_kredi_limiti , 13);


	/* FILL THE DETAIL */
	Utils_IToA(batch_no,batch_no_c);
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(d_GTransaction_Send.batch_no, batch_no_c,3);

	memcpy(d_GTransaction_Send.transaction_seq_no, fpGTransStruct->seq_no,4);

	memset(d_GTransaction_Send.card_info, ' ', sizeof(d_GTransaction_Send.card_info));
	d_GTransaction_Send.input_type = input_type;
	
	if (d_GTransaction_Send.input_type == INPUT_FROM_READER)  
	{
		memcpy(d_GTransaction_Send.card_info,card_info,strlen(card_info));
	}
	else
	{
		memcpy(d_GTransaction_Send.card_info, card_no,strlen(card_no));
		memcpy(&d_GTransaction_Send.card_info[19], exp_date,4);
		memcpy(&d_GTransaction_Send.card_info[23], cvv_2,3);
	}

	/* pinpad status */
	d_GTransaction_Send.PINPad_status = PINPad_status;       

	/* encrypted pin */
	if(input_type ==INPUT_FROM_KEYPAD)
	{
		memset(d_GTransaction_Send.encrypted_PIN, '0', sizeof(d_GTransaction_Send.encrypted_PIN));
	}
	else
	{
		memcpy(d_GTransaction_Send.encrypted_PIN, encrypted_PIN, 16);
	}

	memcpy(d_GTransaction_Send.approval_code, fpGTransStruct->approval_code, 6);

	/* sorgulama tutarý */
	memcpy(d_GTransaction_Send.sorgulama_tutar, fpGTransStruct->sorgulama_tutar, 13);
	
	/* belge no */
	memcpy(d_GTransaction_Send.belge_no, fpGTransStruct->belge_no, 10);
	
	/* mevduat hesap kodu */
	d_GTransaction_Send.mev_hes_kodu = fpGTransStruct->mevduat_hes_kodu;


	/* vade sayýsý */
	memcpy(d_GTransaction_Send.vade_sayisi, fpGTransStruct->vade_sayisi , 2);
	
	memcpy(d_GTransaction_Send.Gtarih_vade, fpGTransStruct->DB_Tarih_Vade, sizeof(d_GTransaction_Send.Gtarih_vade));
/**************************************************************************************/	

	/* provide connection */
	void_ret_code = Host_Connect(TRUE);
	if (void_ret_code !=STAT_OK)
	{
		Host_Disconnect();
		return(void_ret_code);
	}

	/* 04_03 19/11/2002 send reversal if necessary */
	/* 04_07 A.Y. 18/12/2002 Istek uzerine iptal iþleminde reversal kaldirildi */
	/* 04_15 A.Y. 10/06/2003 Eminenin talebi ile acildi */
	if(Trans_SendReversal(NORMAL_REVERSAL, 0, FALSE) != STAT_OK) return (STAT_NOK);
	EmvIF_ClearDsp(DSP_MERCHANT);
	Debug_GrafPrint2("YANIT BEKL˜YOR.. ",3);
	
	/* Initialize the Send Buffer*/
	memset(lbSend_Buffer,0,sizeof(lbSend_Buffer));

	memcpy (lbSend_Buffer,&d_GTransaction_Send,sizeof(d_Transaction_Send));
    lbSend_Buffer[sizeof(d_Transaction_Send)]=0;

	/* 04_07 A.Y. 18/12/2002 Reversal iptal iþleminde olmayacak */
	/* 04_03 19/11/2002 Fill reversal buffer */

	/* 04_15 A.Y. 12/06/2003 Iptal isleminin reversi olmayacak */
	/* memset(Reversal_Buffer,0,sizeof(Reversal_Buffer));
	memcpy (Reversal_Buffer,&d_GTransaction_Send,sizeof(d_GTransaction_Send)); */

	/* 04_15 13/06/2003 iptal oncesi SORGULAMA = TRUE yapýldý */
	SORGULAMA = FALSE; /*Tum iptaller sorgulama olarak algilaniyordu*/
		
	/* Send the Prepared message to the Host */
/*	void_ret_code = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);*/
	if (BUYUKFIRMA) void_ret_code = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
	else void_ret_code = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);

	Host_Disconnect();
	if(void_ret_code != STAT_OK)
	{	/* 04_03 19/11/2002*/
		/* 04_07 A.Y. 18/12/2002 Reversal iptal iþleminde olmayacak */
		/* 04_15 A.Y. 10/06/2003 Eminenin talebi ile acildi */
		/* 04_15 A.Y. 12/06/2003 Iptal isleminin reversi olmayacak */	
		/*ReversalPending = TRUE;	
		Trans_SendReversal(); */

		Printer_WriteStr ("\nIPTAL ESNASINDA");
		Printer_WriteStr ("\nBAGLANTI HATASI");
		Printer_WriteStr ("\nTEKRAR DENEYIN\n");
		Printer_WriteStr("\n\n\n\n");		
		
		return(void_ret_code);
	}
	
	/*void_ret_code = Trans_GAnalyseSaleTransactionReceive();*/ /* 04_13 zaten asagida yapýlýyor. kaldýrýldý 16/05/2003 */

	/* 04_15 13/06/2003  if (void_ret_code != STAT_OK) kontrolunden onceydi */
	memcpy(&d_GTransaction_Receive, Received_Buffer+1, sizeof(d_GTransaction_Receive));

	/* Analyze the response*/
	lbret_code = Trans_GAnalyseSaleTransactionReceive();

	switch (lbret_code)
	{
		case UNEXPECTED_MESSAGE :   /* 04_07 A.Y. 18/12/2002 Reversal iptal iþleminde olmayacak */
									/* 04_15 A.Y. 10/06/2003 Eminenin talebi ile acildi */
									/* 04_15 A.Y. 12/06/2003 Iptal isleminin reversi olmayacak */
									/* ReversalPending = TRUE;	*/

									return(STAT_NOK);
									
		case APPROVED_MESSAGE   :		/*approved */
									if (Files_GVoidTransaction(/*(struct GTransactionStruct *)*/fpGTransStruct) != STAT_OK) 
									{
										/* 04_07 A.Y. 18/12/2002 Reversal iptal iþleminde olmayacak */
										/* 04_15 A.Y. 10/06/2003 Eminenin talebi ile acildi */
										/* 04_15 A.Y. 12/06/2003 Iptal isleminin reversi olmayacak */
										/* ReversalPending = TRUE; */

										EmvIF_ClearDsp(DSP_MERCHANT);
										Mainloop_DisplayWait("GUPDATE BAžARISIZ",2);
										return(STAT_NOK);
									}

									if(!Slips_GFillReceiptStructure())   
									{
										EmvIF_ClearDsp(DSP_MERCHANT);
										Mainloop_DisplayWait("iptalSlips_GFillReceiptStructure hatasi",2);
										return(STAT_NOK);
									}
								

									Slips_GPrintReceipt();

									/*WARN THE USER BY SIGNAL */

									if(WARNING_FLAG)
									{
										Mte_Wait(1000);
										Kbhw_BuzzerPulse(200);
										Mte_Wait(300);
										Kbhw_BuzzerPulse(200);
									}
									break;

		case REJECTED_MESSAGE   :   	if(!Slips_GFillErrorReceiptStructure())
										{
											/* 04_07 A.Y. 18/12/2002 Reversal iptal iþleminde olmayacak */
											/* 04_15 A.Y. 10/06/2003 Eminenin talebi ile acildi */
											/* 04_15 A.Y. 12/06/2003 Iptal isleminin reversi olmayacak */
											/* ReversalPending = TRUE; */

											EmvIF_ClearDsp(DSP_MERCHANT);
											Mainloop_DisplayWait("Slips_GFillErrorReceiptStructure hatasi",3);
											return(STAT_NOK);
										}
					
										Slips_GPrintErrorReceipt();
										ERROR_RECEIPT = TRUE;
									break;
	}

		
	return(STAT_OK);
}


/* 04_15 10/06/2003 irfan & Ayhan */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Trans_GFindTrxTotals
 *
 * DESCRIPTION	: Find the total amount of all type off trx.
 *             
 *
 * RETURN		: STAT_OK. STAT_NOK      
 *
 * NOTES		:       
 *
 * ------------------------------------------------------------------------ */
byte Trans_GFindTrxTotals(void)
{

	int j;
	GTransactionStruct	GTempTransStruct;
	sint lbStatus, lwLastSeq,lwStatus;
	sint handle3;
	char temp_total_amt[14]; 

	memset(temp_total_amt, 0, sizeof(temp_total_amt));
	
	memset(net_total_GT1_amt,'0', sizeof(net_total_GT1_amt));
	net_total_GT1_amt[13] = 0;

	memset(net_total_GT2_amt,'0', sizeof(net_total_GT2_amt));
	net_total_GT2_amt[13] = 0;

	memset(rev_total_GT1_amt,'0', sizeof(rev_total_GT1_amt));
	rev_total_GT1_amt[13] = 0;
	
	memset(rev_total_GT2_amt,'0', sizeof(rev_total_GT2_amt));
	rev_total_GT2_amt[13] = 0;

	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)Transaction_GFile)==RD_CHECK_OK)
	{
		handle3 = RamDisk_Reopen((byte *)Transaction_GFile)->Handle;
		lwLastSeq = RamDisk_Seek(handle3,SEEK_LAST);
		RamDisk_Seek(handle3, SEEK_FIRST);
		for (j=0; j<lwLastSeq; j++)
		{
			lbStatus = RamDisk_Read(handle3, &GTempTransStruct);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
				if (GTempTransStruct.MSG_TYPE == '1')
				{
					if (GTempTransStruct.void_status == '0')
					{
						memcpy(temp_total_amt, GTempTransStruct.sorgulama_tutar, length_amount_surcharge);
						Utils_Add_Strings(temp_total_amt, net_total_GT1_amt, net_total_GT1_amt);
						lwStatus = STAT_OK;
					}
					else
					{
						memcpy(temp_total_amt, GTempTransStruct.sorgulama_tutar, length_amount_surcharge);
						Utils_Add_Strings(temp_total_amt, rev_total_GT1_amt, rev_total_GT1_amt);
						lwStatus = STAT_OK;
					}

				}
				else if (GTempTransStruct.MSG_TYPE == '2')
				{
					if (GTempTransStruct.void_status == '0')
					{
						memcpy(temp_total_amt, GTempTransStruct.sorgulama_tutar, length_amount_surcharge);
						Utils_Add_Strings(temp_total_amt, net_total_GT2_amt, net_total_GT2_amt);
						lwStatus = STAT_OK;
					}
					else
					{
						memcpy(temp_total_amt, GTempTransStruct.sorgulama_tutar, length_amount_surcharge);
						Utils_Add_Strings(temp_total_amt, rev_total_GT2_amt, rev_total_GT2_amt);
						lwStatus = STAT_OK;
					}
				}

			}
			else	if (lbStatus == RD_EOF)	lwStatus = STAT_EOF;
		}
	}
	else
	{
		memset(net_total_GT1_amt, '0', sizeof(net_total_GT2_amt));
		net_total_GT1_amt[13] = 0;
		memset(rev_total_GT1_amt, '0', sizeof(rev_total_GT2_amt));
		rev_total_GT1_amt[13] = 0;

		memset(net_total_GT2_amt, '0', sizeof(net_total_GT2_amt));
		net_total_GT2_amt[13] = 0;
		memset(rev_total_GT2_amt, '0', sizeof(rev_total_GT2_amt));
		rev_total_GT2_amt[13] = 0;

	}
}

/* 04_16 06/08/2003 Biz Card Bakiye Sorma Fonksiyonu irfan */
/* --------------------------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_GeniusBakiyeSorma
 *
 * DESCRIPTION          : Performs Genius Balance Query for information
 *
 * RETURN               : STAT OK, STAT_NOK
 *
 * NOTES                : r.i.o.
 *							Gtran_type : indicates the type of the genius transaction
 *                          GENIUS_SATIS			1
 *							GENIUS_VADELI			2
 *							GENUIS_IPTAL			3
 *							GENUIS_BAKIYE_SORMA		5
 *
 * -------------------------------------------------------------------------------------------- */
byte Trans_GeniusBakiyeSorma(int gtran_type)
{
	/* balance related variables */
	char temp_buff[17];
	char balance_message[33];
	char TransReceived_DateTime[11];
	byte Transaction_Ret=0;
	char lbtemp[50];
	char lbSend_Buffer[800];
	char device_date[8], device_time[6];


	/* Check for installation*/
	if ( !INSTALLATION)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" KURULUž YAPIN  ",1);
		return(STAT_NOK);
	}

	/* Check for incomplete EOD transcation*/
	if (EOD_flag)
	{
		EmvIF_ClearDsp(DSP_MERCHANT);
		Mainloop_DisplayWait(" GšNSONU YAPIN  ",1);
		return(STAT_NOK);
	}

	/*read parameters files */
	if(Files_ReadAllParams() != STAT_OK) return (STAT_NOK);


	/* Initialize the Send Buffer*/
/*	memset(Send_Buffer,0,sizeof(Send_Buffer));*/
	memset(lbSend_Buffer,0,sizeof(lbSend_Buffer));
	
	/* intialize the Send structure */
	memset(&d_GTransaction_Send,'0',sizeof(d_GTransaction_Send));

	/* Set Debit and Amex flags and second copy flag*/
	DEBIT_FLAG      = FALSE;
	SECOND_COPY     = FALSE;
	
	/* debit prefix check */
	DEBIT_FLAG = Utils_Debit_Check();

	if(input_type !=INPUT_FROM_KEYPAD)
	{
		/* þifre girisi */	
		GrphText_Cls (TRUE);
		Debug_GrafPrint2(" ž˜FREY˜ G˜R˜N ?",3);

#ifndef _8010
		if( (Test_IsEeprom() > 0) && (!EXTERNAL_PINPAD) )
#else
		if(TRUE)
#endif
		{
		if(Verde_AskPIN_Embedded (encrypted_PIN) != STAT_OK)
		{
			Mainloop_DisplayWait("EMBEDDED PINPAD SIF. SORMA HATA ",1);      
			return(STAT_NOK);
		}
		}else
		{
		/*Select Which Master Key is used */
		if(Utils_SelectMasterKey () != STAT_OK)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  ž˜FRE C˜HAZI    BA¦LI DE¦˜L   ",2);
			return(STAT_NOK);
		}
		if(Host_PinpadIdlePrompt() != STAT_OK)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  ž˜FRE CÝHAZI    BA¦LI DE¦˜L   ",2);
			return(STAT_NOK);
		}

		/*Get the PIN from PINPAD and prepare the PIN Block */
		if(Trans_AskPin (encrypted_PIN) != STAT_OK)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("SIF. SORMA HATA",1);      
			return(STAT_NOK);
		}
		}
	}
	else
	{
		memset(encrypted_PIN, '0', sizeof(encrypted_PIN));
		encrypted_PIN[16] = 0;
	}


	/* initialize ilk telefon arama flag to false */
	FIRST_PHONE_FLAG         = FALSE;
	SERVER_OPEN_TIMEOUT_FLAG = FALSE;

	/* Provide the connection with HOST. Host Predial had been provided beforehand */

	Transaction_Ret = Host_Connect(TRUE);

	/* eger server time out oldu ise ve birinci tel arandý ise*/
	if ((Transaction_Ret !=STAT_OK) && (FIRST_PHONE_FLAG))
	{
			Host_Disconnect();
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("2. SERVER ARAMA ",0);
			Mte_Wait(2500); /* disconnection ýn saðlanmasý icin bekleniyor */

			memcpy(tel_no_1_temp, tel_no_1, strlen(tel_no_1));
			tel_no_1_temp[strlen(tel_no_1)] = 0;

			memcpy(tel_no_2_temp, tel_no_2, strlen(tel_no_2));
			tel_no_2_temp[strlen(tel_no_2)] = 0;

			memcpy(tel_no_1, tel_no_2, strlen(tel_no_2));
			tel_no_1[strlen(tel_no_2)] = 0;

			tel_no_2[0] = 0;

			SERVER_OPEN_TIMEOUT_FLAG = TRUE;

			/* 01_18 08/12/99 Host_Predial() Fonksiyonu if ile kontrol edilerek kullanýlýyor*/
			/*Host_PreDial();*/
			if(Host_PreDial() != STAT_OK) 
			{
				EmvIF_ClearDsp(DSP_MERCHANT);
				Mainloop_DisplayWait("     ARAMA           HATASI     ",1);
				return(STAT_NOK);
			}
	

			Mte_Wait(2000);
			Transaction_Ret = Host_Connect(TRUE);

			/* tel no larý eski haline getiriliyor*/
			memcpy(tel_no_1, tel_no_1_temp, strlen(tel_no_1_temp));
			tel_no_1[strlen(tel_no_1_temp)] = 0;

			memcpy(tel_no_2, tel_no_2_temp, strlen(tel_no_2_temp));
			tel_no_2[strlen(tel_no_2_temp)] = 0;
	}

	if(Transaction_Ret != STAT_OK)
	{
		Host_Disconnect();
		return(Transaction_Ret);
	}

	/* 04_03 19/11/2002 send reversal if necessary */
	if(Trans_SendReversal(NORMAL_REVERSAL, 0, FALSE) != STAT_OK) return (STAT_NOK);


	Debug_GrafPrint2("YANIT BEKL˜YOR.. ",4);

	SORGULAMA = TRUE;		/* sorgulama = TRUE gerçek transaction = FALSE */

	/****************************/
	/* Prepare the genius balance query Message*/
	/* FILL  THE  HEADER  PART */
	memset(&d_GTransaction_Send, ' ', sizeof(d_GTransaction_Send));
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) ) d_GTransaction_Send.GHeader_Transact.Cur_Index = 'X';
	else d_GTransaction_Send.GHeader_Transact.Cur_Index = 'Y';
	d_GTransaction_Send.GHeader_Transact.ActAppCode = POS_Type;
	d_GTransaction_Send.GHeader_Transact.SecAppCode = '1';	/* 0-nobiz, 1-bizpos */
	memcpy(d_GTransaction_Send.GHeader_Transact.PosType, "LIP", 3);
	memcpy(d_GTransaction_Send.GHeader_Transact.PosModel, "8000", 4);
	memcpy(d_GTransaction_Send.GHeader_Transact.PosVersion, POS_VERSION_NUMBER, 4);
	if (Cashier_Flag == 2) memset(d_GTransaction_Send.GHeader_Transact.Cashier_Id, ' ', 16 );
	else memcpy(d_GTransaction_Send.GHeader_Transact.Cashier_Id, Kasiyer_No, 16 );
	memcpy(d_GTransaction_Send.GHeader_Transact.IMEI_ID, Kampanya_Data, 15 );
	memset(d_GTransaction_Send.GHeader_Transact.Filler, ' ', 19 );
	Utils_GetDateTime( device_date, device_time );
	{
		d_GTransaction_Send.GHeader_Transact.PosDate[0] = device_date[6];
		d_GTransaction_Send.GHeader_Transact.PosDate[1] = device_date[7];
		d_GTransaction_Send.GHeader_Transact.PosDate[2] = device_date[3];
		d_GTransaction_Send.GHeader_Transact.PosDate[3] = device_date[4];
		d_GTransaction_Send.GHeader_Transact.PosDate[4] = device_date[0];
		d_GTransaction_Send.GHeader_Transact.PosDate[5] = device_date[1];
		d_GTransaction_Send.GHeader_Transact.PosTime[0] = device_time[0];
		d_GTransaction_Send.GHeader_Transact.PosTime[1] = device_time[1];
		d_GTransaction_Send.GHeader_Transact.PosTime[2] = device_time[3];
		d_GTransaction_Send.GHeader_Transact.PosTime[3] = device_time[4];
	} 

	d_GTransaction_Send.GHeader_Transact.MSG_INDICATOR = 'G';
	d_GTransaction_Send.GHeader_Transact.MSG_TYPE = gtran_type + '0';
	d_GTransaction_Send.GHeader_Transact.POS_software_ver = POS_software_ver;
	memcpy(d_GTransaction_Send.GHeader_Transact.merch_device_no , merch_device_no , 8);
	d_GTransaction_Send.GHeader_Transact.device_serial_no = device_serial_no;
	
	/* sorgulama */
	d_GTransaction_Send.GHeader_Transact.transaction_type = '1';
	memcpy(d_GTransaction_Send.available_balance, "0000000000000", 13);
	memcpy(d_GTransaction_Send.bayii_garantisiz_borc, "0000000000000", 13);
	memcpy(d_GTransaction_Send.kart_islem_toplami, "0000000000000", 13);
	memcpy(d_GTransaction_Send.kullanilan_kredi_limiti, "0000000000000", 13);
	memcpy(d_GTransaction_Send.sorgulama_tutar, "0000000000000", 13);

	/*FILL THE DETAIL */
	Utils_IToA(batch_no,batch_no_c);
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(d_GTransaction_Send.batch_no, batch_no_c,3);

	/* 04_00 genius */
	Utils_IToA(genius_seq_no,genius_seq_no_c);
	Utils_LeftPad(genius_seq_no_c, '0', 4);
	memcpy(d_GTransaction_Send.transaction_seq_no, genius_seq_no_c,4);

	memset(d_GTransaction_Send.card_info, ' ', sizeof(d_GTransaction_Send.card_info));
	d_GTransaction_Send.input_type = input_type;
	
	if (d_GTransaction_Send.input_type == INPUT_FROM_READER)  
	{
		memcpy(d_GTransaction_Send.card_info,card_info,strlen(card_info));
	}
	else
	{
		memcpy(d_GTransaction_Send.card_info, card_no,strlen(card_no));
		memcpy(&d_GTransaction_Send.card_info[19], exp_date,4);
		memcpy(&d_GTransaction_Send.card_info[23], cvv_2,3);
	}

	/* pinpad status */
	d_GTransaction_Send.PINPad_status = PINPad_status;       

	/* encrypted pin */
	if(input_type ==INPUT_FROM_KEYPAD)
	{
		memset(d_GTransaction_Send.encrypted_PIN, '0', sizeof(d_GTransaction_Send.encrypted_PIN));
	}
	else
	{
		memcpy(d_GTransaction_Send.encrypted_PIN, encrypted_PIN, 16);
	}

	memset(d_GTransaction_Send.approval_code,' ', sizeof(d_GTransaction_Send.approval_code));

	
	/* belge no */
	memcpy(d_GTransaction_Send.belge_no, "0000000000", 10);
	
	/* mevduat hesap kodu */
	d_GTransaction_Send.mev_hes_kodu = 0;


	/* vade sayýsý */
	memcpy(d_GTransaction_Send.vade_sayisi, "00", 2);

	d_GTransaction_Send.mev_hes_kodu = '0';
	memset(d_GTransaction_Send.Gtarih_vade,'0', sizeof(d_GTransaction_Send.Gtarih_vade));
	/**************************/

	memcpy (lbSend_Buffer,&d_GTransaction_Send,sizeof(d_GTransaction_Send));

	lbSend_Buffer[sizeof(d_GTransaction_Send)]=0;

	/* Send the message to the Host and get the response*/
	/*Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);*/
	if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
	else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);

	if(Transaction_Ret != STAT_OK)
	{
		return(Transaction_Ret);
	}

	memcpy(&d_GTransaction_Receive,Received_Buffer+1,sizeof(d_GTransaction_Receive));

	/* Analyze the response*/
	ret_code = Trans_GAnalyseSaleTransactionReceive();

	if (ret_code == PIN_REQUESTED_MESSAGE)
	{
		/*DEBIT_FLAG = TRUE;*/

		/* GETPIN FUNCTION */
		GrphText_Cls (TRUE);
		Debug_GrafPrint2(" ž˜FREY˜ G˜R˜N ?",3);

		/*Select Which Master Key is used */
		if(Utils_SelectMasterKey () != STAT_OK)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("  MASTER KEY      YUKLENMEMIS   ",1);
		}

		/*Get the PIN from PINPAD and prepare the PIN Block */
		if(Trans_AskPin (encrypted_PIN) != STAT_OK)
		{
			EmvIF_ClearDsp(DSP_MERCHANT);
			Mainloop_DisplayWait("ž˜FRE SORMA HATASI",2);
		}
		
		memcpy(d_GTransaction_Send.encrypted_PIN, encrypted_PIN, 16);

		memcpy (lbSend_Buffer,&d_Transaction_Send,sizeof(d_Transaction_Send));    /* UPDATE_0_12 20/09/99 */
        lbSend_Buffer[sizeof(d_Transaction_Send)]=0;
		/* RESend the message to the Host and get the response*/
		GrphText_Cls (TRUE);
		Debug_GrafPrint2("YANIT BEKL˜YOR.. ",4);
/*		Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);*/
		if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
		else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);

		if(Transaction_Ret != STAT_OK)
		{ 
			return(Transaction_Ret);
		}

		/* Move Received_Buffer to the Transaction_Receive */
		memcpy(&d_GTransaction_Receive, Received_Buffer+1, sizeof(d_GTransaction_Receive));

		ret_code = Trans_GAnalyseSaleTransactionReceive();
	}

	switch (ret_code)
	{
		case UNEXPECTED_MESSAGE :       return(STAT_NOK);
									
		
		case APPROVED_MESSAGE   :		/*approved inquiry transaction*/
										if(!Slips_GFillBalanceQueryReceiptStructure())   
										{
											EmvIF_ClearDsp(DSP_MERCHANT);
											Mainloop_DisplayWait("Slips_GFillBalanceQueryReceiptStructure",2);
											return(STAT_NOK);
										}

										Slips_GPrintBalanceQuery_Slip();

										Mte_Wait(1000);	/* wait for printing sorgulama slip */
										break;


		case PIN_REQUESTED_MESSAGE: 
									EmvIF_ClearDsp(DSP_MERCHANT);
									Mainloop_DisplayWait("Y˜NE PIN ˜STEND˜",2);
									return(STAT_NOK);

		case REJECTED_MESSAGE   :	Slips_GBakiyePrintErrorReceipt();
										
										break;
	}

	return(STAT_OK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Trans_SendReversal
 *
 * DESCRIPTION:
 *             
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Trans_SendReversal(byte mode, PTStruct *fpPTStruct, byte IsEMV)
{
	byte Transaction_Ret, tempbyte;
//	struct GTransaction_Receive fpGReversal;
	struct Transaction_Receive fpReversal;
        boolean  genius_rev;
        char lbtemp[4]; 
/*@bm opt 30.08.2004	
	int BufLen; */
	
	if(INSTALLATION_ONGOING) return STAT_OK;
	Transaction_Ret = STAT_OK;
	switch(mode)
	{
	case NORMAL_REVERSAL:
		if (ReversalPending)
		{
			Mainloop_DisplayWait( "REVERSAL        GONDERILIYOR",
									  NO_WAIT );
/*
			if (IsEMV)
			{
				int k;
				RowData myrdData;

				fpPTStruct->Field55Len = Trans_AddChipFieldsWithoutScript( ( byte * ) fpPTStruct->Field55);

					memcpy( &d_Transaction_Send.EMVFields,
							fpPTStruct->Field55,
							sizeof( d_Transaction_Send.EMVFields ) );

					Host_AsciiToBinMy( d_Transaction_Send.EMVFields.IDSN,
									   d_Transaction_Send.EMVFields.IDSN,
									   sizeof( d_Transaction_Send.EMVFields.
											   IDSN ) );
					for ( k = sizeof( d_Transaction_Send.EMVFields.IDSN ) / 2;
						  k < sizeof( d_Transaction_Send.EMVFields.IDSN );
						  k++ )
				d_Transaction_Send.EMVFields.IDSN[k] = ' ';

				if (EmvDb_Find(&myrdData, tagTERML_COUNTRY_CODE))
				{
						EmvIF_BinToAscii( myrdData.value,
										  d_Transaction_Send.EMVFields.IDSN +
										  8, myrdData.length, 4, FALSE );
						memcpy( d_Transaction_Send.EMVFields.IDSN + 12,
								"    ", 4 );
				}
				if (EmvDb_Find(&myrdData, tagTRAN_CURR_CODE))
				{
						EmvIF_BinToAscii( myrdData.value, d_Transaction_Send.EMVFields.IDSN + 12,
										  myrdData.length, 4, FALSE );
				}

				tempbyte = d_Transaction_Send.EMVFields.CID[0];
				if ( EmvDb_Find( &myrdData, tagAPPL_CRYPTOGRAM ) )
				{
					EmvIF_BinToAscii( myrdData.value, d_Transaction_Send.EMVFields.AC, myrdData.length, sizeof(d_Transaction_Send.EMVFields.AC),FALSE);
				}
				d_Transaction_Send.EMVFields.CID[0] = tempbyte;

				memcpy( d_Transaction_Send.EMVFields.ARC, "  ", 2 );

				memcpy(Reversal_Buffer,&d_Transaction_Send,
							sizeof(d_Transaction_Send));
				Reversal_Buffer[Reversal_Buffer_Len] = 0;
			}
*/
			if((Reversal_Buffer[74] == 'G')||(Reversal_Buffer[74] == 'H'))
			{
				genius_rev=TRUE;
				Reversal_Buffer[74] = 'H'; /*genius reversal*/
			}
			else
			{
				genius_rev=FALSE;
				Reversal_Buffer[74] = 'R';
			}
//			Reversal_Buffer[74] = 'R';	/*bm YTL 08/07/2004 */
			if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(Reversal_Buffer, TRUE, TRUE, TRUE, TRUE);
			else Transaction_Ret = Host_SendReceive(Reversal_Buffer, TRUE, TRUE, TRUE);
		/*
				 * Printer_WriteStr("Reversal_Buffer_Send:\n");
				 * PrntUtil_BufAsciiHex ( Reversal_Buffer, 65 );
		*/	
			memset(&fpReversal, 0, sizeof(fpReversal));
			if(Transaction_Ret == STAT_OK)
			{
/*
                	        if(genius_rev) memcpy(&fpGReversal,Received_Buffer+1,sizeof(fpGReversal));
				else
*/
				memcpy(&fpReversal,Received_Buffer+1,sizeof(fpReversal));
			}
			else
			{
				Printer_WriteStr ("Reversal Gonderilemedi\n\n");
				return(STAT_NOK);
			}
	
			/* 05_03 mutabakatsýzlýk problemini cozmek icin eklendi 30/09/2002 */
			memset(lbtemp, 0, sizeof(lbtemp));
/*
			if(genius_rev) memcpy(lbtemp, &fpGReversal.confirmation,3);
			else
*/
			memcpy(lbtemp, &fpReversal.confirmation,3);
			if ( memcmp(lbtemp, "899", 3) == 0 || memcmp(lbtemp, "000", 3) == 0) 	/* 04_05 28/11/2002 000 eklendi.*/
			{
				ReversalPending = FALSE;
				ChangesPending = FALSE;
			}
			else 
			{
				Printer_WriteStr ("Reversal Hatasi: ");
				Printer_WriteStr (lbtemp);
				Printer_WriteStr("\n\n");
				return(STAT_NOK);
			}
		}
		break;
	case CHANGES	:
		if ( !SEND_EMV_FIELDCHANGES )
		{
			int k;
			RowData myrdData;

//			Mainloop_DisplayWait( "DEGISIKLIKLER   GONDERILIYOR",
//									  NO_WAIT );
			if (ChangesPending != TRUE)
			{
				ChangesPending = TRUE;

				fpPTStruct->Field55Len = Trans_AddChipFieldsWithoutScript( ( byte * ) fpPTStruct->Field55);
	
				memcpy( &d_Transaction_Send.EMVFields, fpPTStruct->Field55,
							sizeof( d_Transaction_Send.EMVFields ) );

				Host_AsciiToBinMy( d_Transaction_Send.EMVFields.IDSN,
							   d_Transaction_Send.EMVFields.IDSN,
							   sizeof( d_Transaction_Send.EMVFields.
											   IDSN ) );
				for ( k = sizeof( d_Transaction_Send.EMVFields.IDSN ) / 2;
					  k < sizeof( d_Transaction_Send.EMVFields.IDSN ); k++ )
						d_Transaction_Send.EMVFields.IDSN[k] = ' ';
	
				if (EmvDb_Find(&myrdData, tagTERML_COUNTRY_CODE))
				{
					EmvIF_BinToAscii( myrdData.value,
							  d_Transaction_Send.EMVFields.IDSN + 8,
							  myrdData.length, 4, FALSE );
					memcpy( d_Transaction_Send.EMVFields.IDSN + 12, "    ",
							4 );
				}
				if (EmvDb_Find(&myrdData, tagTRAN_CURR_CODE))
				{
						EmvIF_BinToAscii( myrdData.value, d_Transaction_Send.EMVFields.IDSN + 12,
										  myrdData.length, 4, FALSE );
				}
				tempbyte = d_Transaction_Send.EMVFields.CID[0];
				if ( EmvDb_Find( &myrdData, tagAPPL_CRYPTOGRAM ) )
				{
						EmvIF_BinToAscii( myrdData.value, d_Transaction_Send.EMVFields.AC, myrdData.length, sizeof(d_Transaction_Send.EMVFields.AC),FALSE);
				}
				d_Transaction_Send.EMVFields.CID[0] = tempbyte;
				memcpy( Reversal_Buffer, &d_Transaction_Send,
							sizeof( d_Transaction_Send ) );
				Reversal_Buffer[Reversal_Buffer_Len] = 0;
				Reversal_Buffer[74] = 'C';	/*bm YTL 0 */
			}
/*
				if ( DEBUG_MODE )
			{
					Printer_WriteStr( "Changes_Send:\n" );
					PrntUtil_BufAsciiHex( ( byte * ) Reversal_Buffer,
										  strlen( Reversal_Buffer ) );
			}
*/
			if ( BUYUKFIRMA )
				Transaction_Ret =
					Host_BFirmaSendReceive( Reversal_Buffer, TRUE, TRUE,
												TRUE, TRUE); /* @bm 27.08.2004 TRUE );*/
			else
				Transaction_Ret =
					Host_SendReceive( Reversal_Buffer, TRUE, TRUE, TRUE);

			if ( Transaction_Ret != STAT_OK )
			{
				Printer_WriteStr( "Degisiklikler Hatasi\n\n" );
				return(STAT_NOK);
			}

			Reversal_Buffer[74] = 'R';	/*bm YTL 08/07/2004 */

			memset(&fpReversal, 0, sizeof(fpReversal));
			memcpy(&fpReversal,Received_Buffer+1,sizeof(fpReversal));

			memset(lbtemp, 0, sizeof(lbtemp));
			memcpy(lbtemp, &fpReversal.confirmation,3);
			if (memcmp(lbtemp, "000", 3) != 0)
			{
				Printer_WriteStr ("Degisiklikler Hatasi: ");
				Printer_WriteStr (lbtemp);
				Printer_WriteStr("\n\n");
				return(STAT_OK); /* Problemli bir coding -- Kontrol edilecek. */
			}
			else
			{ 
				ChangesPending = FALSE;
				ReversalPending = FALSE;
			}
			break;
		}
	}
	if (Files_WriteAllParams() != STAT_OK)
	{
		Printer_WriteStr ("Files_WriteAllParams hatasi:\n");
		Printer_WriteStr("\n\n");
		return(STAT_NOK);
	}
	return(STAT_OK);
}
/* 20.07.2004 */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : EMVAxessGetAmount and the # of installment
 *
 * DESCRIPTION          : Gets the amount  and the # of installment (if ATS_FLAG is TRUE)
 *
 * RETURN                       : TRUE or FALSE
 *
 * NOTES                        : r.i.o & b.m 20.07.2004
 *
 * ------------------------------------------------------------------------ */
boolean EMVAxessGetAmount(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	boolean ATS_COUNT_OK;
	byte xls_ret_local;
	char xls_temp_local[33];
	char XLS_Cardno_local[20];	
	boolean FAIZLI_IADE_OK;                 /* TRUE: giriþ geçerli, FALSE: giriþ geçersiz */
	char value_null[2];
	char sale_date_day[3],sale_date_month[3];
	sint sale_date_day_value,sale_date_month_value;
	char XLS_Cardno[20];	/** XLS_INTEGRATION 16/08/2001 21 idi. 20 yapýldý. **/
	char first_6_digit_local[7];
	char first_6_digit [7];
	byte *pbIndex;
	char temp_cn[20];
	char tempchnm[50];
	RowData myrdData;
	PTStruct fpPTStruct;
	int YTL_Digit;				/* bm YTL 29/06/2004 */



	memset(value_null, 0 , sizeof(value_null));
	memset(Received_Buffer, 0, sizeof(Received_Buffer));
	memset(fpPTStruct.Pan, 0, sizeof(fpPTStruct.Pan));
	EmvDb_Find(&myrdData, tagPAN);
	EmvIF_BinToAscii(myrdData.value, fpPTStruct.Pan, myrdData.length, sizeof(fpPTStruct.Pan), FALSE);
	while (fpPTStruct.Pan[strlen(fpPTStruct.Pan) - 1] == 'F') fpPTStruct.Pan[strlen(fpPTStruct.Pan) - 1] = 0;
	fpPTStruct.PanLength = strlen(fpPTStruct.Pan);
	memset(card_info,' ',40);
	memset(card_no,0,20);
	memset(exp_date,0,5);
	memset(cvv_2,0,4);
	memset(card_info1_name, 0, sizeof(card_info1_name));
	memset(card_info1_surname, 0, sizeof(card_info1_surname));		
	memset(tempchnm,' ',50);	
	pbIndex = (byte*)tempchnm;
	EmvIF_AppendData(&pbIndex, tagCARDHOLDER_NAME, FALSE, 26);
	memcpy(card_info1_name, tempchnm, sizeof(card_info1_name));
	memset(tempchnm,' ',50);	
	pbIndex = (byte*)tempchnm;
	EmvIF_AppendData(&pbIndex, tagCRDHLDR_NAME_EXT, FALSE, 45);
	memcpy(card_info1_surname, tempchnm, sizeof(card_info1_surname));
	memcpy(card_info, fpPTStruct.Pan, fpPTStruct.PanLength);
	memset(temp_cn,' ',20);	
	pbIndex = (byte*)temp_cn;
	EmvIF_AppendData(&pbIndex, tagAPPL_EXPIRATION_DATE, FALSE, 0);
	card_info[fpPTStruct.PanLength] = '=';
	EmvIF_BinToAscii(temp_cn,card_info+fpPTStruct.PanLength+1,2,4,FALSE);
	memcpy(card_info+fpPTStruct.PanLength+5,"000",3);
	memcpy(card_no, fpPTStruct.Pan, fpPTStruct.PanLength);
	memcpy(exp_date,card_info+fpPTStruct.PanLength+1,4);
	memcpy(cvv_2,"000",3);
	memset(remained_chip_data,0,sizeof(remained_chip_data));
	memcpy(remained_chip_data,card_info+fpPTStruct.PanLength+1,7);
	memset(XLS_Cardno_local, 'F', sizeof(XLS_Cardno_local));
	memcpy(XLS_Cardno_local+(sizeof(XLS_Cardno_local)-1)-strlen(card_no),card_no,strlen(card_no));
	XLS_Cardno_local[19] = 0;
 	memcpy(first_6_digit_local,card_no,6);
	first_6_digit_local[6] = 0;
	memcpy( first_6_digit, card_no, 6 );
	first_6_digit[6] = 0;
	if ( ( strstr( ATS_Prefixes, first_6_digit ) != NULL ) ||
		 ( strstr( Loyalty_Prefixes, first_6_digit ) != NULL ) ) ATS_PERMISSION_FLAG = TRUE;
	else ATS_PERMISSION_FLAG = FALSE;

	memcpy( first_6_digit, card_no, 6 );
	first_6_digit[6] = 0;

	if ( ( strstr( Loyalty_Prefixes, first_6_digit ) != NULL ) &&
		 ( !CORRUPTED_CHIP_TRANSACTION_FLAG ) && ( ( tran_type == SALE ) ||
		   ( tran_type == MANUEL ) || ( tran_type == RETURN ) ) )
	  LOYALTY_PERMISSION_FLAG = TRUE;
	else 
	  LOYALTY_PERMISSION_FLAG = FALSE;

	/* verde */
/*	if( !VERDE_TRANSACTION || (glAmount_Req_Type == '1') )*/
	{
		/*bm YTL 29/06/2004 8-YTL, 11-TL */
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) ) YTL_Digit = 10;
		else YTL_Digit = 11;
	
		if( (strstr(Loyalty_Prefixes, first_6_digit_local) != NULL) &&
		 ((POS_Type == '4') || (POS_Type == '6'))  && (tran_type == RETURN) )
		{	/* axess POS icin gecerli */
			if(!Mainloop_GetKeyboardInput("PROVIZYON TUTARI",Buffer,0,YTL_Digit,
							   FALSE, FALSE, FALSE, TRUE, FALSE,&Len,value_null, FALSE))
			{
				EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				TL_Iade = FALSE;
				return(FALSE);
			}
			else
			{
				if( TL_Iade == TRUE)
				{					
					memset(amount_Iade, 0 , sizeof(amount_Iade));
					memcpy(amount_Iade,Buffer,Len);
					memcpy( amount_surcharge, Buffer, Len );
					amount_surcharge[Len] = 0;
					Utils_LeftPad( amount_surcharge, '0', 13 );
				memset(amount_surcharge_saved, 0 , sizeof(amount_surcharge_saved));
				memcpy(amount_surcharge_saved,amount_surcharge,sizeof(amount_surcharge));
					Main_AddAscii(Buffer,sizeof(Buffer),"5000",4,Buffer, sizeof(Buffer));
					memcpy(amount_surcharge_iade + 4,Buffer,Len);
					amount_surcharge_iade[Len]=0;
					Utils_LeftPad(amount_surcharge_iade, '0', 13);
				}
				else
				{
					memcpy(amount_surcharge,Buffer,Len);
					amount_surcharge[Len]=0;
					Utils_LeftPad(amount_surcharge, '0', 13);
				memset(amount_surcharge_saved, 0 , sizeof(amount_surcharge_saved));
				memcpy(amount_surcharge_saved,amount_surcharge,sizeof(amount_surcharge));
				}
			}
		}
		else
		{
			/*bm YTL 29/06/2004 8-YTL, 11-TL */
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
				YTL_Digit = 10;
			else
				 YTL_Digit=11;
			if(!Mainloop_GetKeyboardInput((char *)MESSAGE_ENTER_AMOUNT,Buffer,0,YTL_Digit,
							   FALSE, FALSE, FALSE, TRUE, FALSE,&Len,value_null, FALSE))
			{
				TL_Iade = FALSE;
					EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
					EMVAxess = FALSE;
				return(FALSE);
			}
			else
			{
				if( TL_Iade == TRUE)
				{					
					memset(amount_Iade, 0 , sizeof(amount_Iade));
					memcpy(amount_Iade,Buffer,Len);
					Main_AddAscii(Buffer,sizeof(Buffer),"5000",4,Buffer, sizeof(Buffer));
					memcpy(amount_surcharge + 4,Buffer,Len);
					amount_surcharge[Len]=0;
					Utils_LeftPad(amount_surcharge, '0', 13);
				memset(amount_surcharge_saved, 0 , sizeof(amount_surcharge_saved));
				memcpy(amount_surcharge_saved,amount_surcharge,sizeof(amount_surcharge));
				}
				else
				{
					memcpy(amount_surcharge,Buffer,Len);
					amount_surcharge[Len]=0;
					Utils_LeftPad(amount_surcharge, '0', 13);
				memset(amount_surcharge_saved, 0 , sizeof(amount_surcharge_saved));
				memcpy(amount_surcharge_saved,amount_surcharge,sizeof(amount_surcharge));
				}
			}
		}
	}
	memset(amount_surcharge_reverse, 0, sizeof(amount_surcharge_reverse));
	if( (strstr(Loyalty_Prefixes, first_6_digit_local) != NULL) && ((POS_Type == '4') || (POS_Type == '6'))  && (tran_type == RETURN) )
	{	/* axess POS icin gecerli */
		if(!Mainloop_GetKeyboardInput("IADE TUTARI ?",Buffer,0,11,
						   FALSE, FALSE, FALSE, TRUE, FALSE,&Len,value_null, FALSE))
		{
			EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
			EMVAxess = FALSE;
			TL_Iade = FALSE;
			return(FALSE);
		}
		else
		{
				if( TL_Iade == TRUE) /*@04.09.2004 */
				{					
					memset(amount_Iade, 0 , sizeof(amount_Iade));
					memcpy(amount_Iade,Buffer,Len);
					memcpy(amount_surcharge_reverse,Buffer,Len);
					amount_surcharge_reverse[Len]=0;
					Utils_LeftPad(amount_surcharge_reverse, '0', 13);
					Main_AddAscii(Buffer,sizeof(Buffer),"5000",4,Buffer, sizeof(Buffer));
/*					memcpy(amount_surcharge_iade + 4,Buffer,Len);
					amount_surcharge_iade[Len]=0;
					Utils_LeftPad(amount_surcharge_iade, '0', 13); */

					if ( Utils_Compare_Strings ( amount_surcharge_reverse, amount_surcharge ) != 1 )
					{	/* amount_surcharge_reverse < and = amount_surcharge */
						memcpy(amount_surcharge_iade + 4,Buffer,Len);
						amount_surcharge_iade[Len]=0;
						Utils_LeftPad(amount_surcharge_iade, '0', 13);
					}
				}
				else
				{
					memcpy( amount_surcharge_reverse, Buffer, Len );
					amount_surcharge_reverse[Len] = 0;
					Utils_LeftPad( amount_surcharge_reverse, '0', 13 );
				}
		}
	}

	/* Initialize the ATS_Count*/
	ATS_Count=1;
	strcpy(ATS_Count_c,"01");
	ATS_Count_c[2] = 0;
	if( (tran_type != PREPROV) &&  ATS_FLAG && ATS_PERMISSION_FLAG)
	{
		ATS_COUNT_OK = TRUE;
		do
		{
			if(!Mainloop_GetKeyboardInput("TAKS˜T SAYISI ?",Buffer,0,2,
					   FALSE, FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE))
			{
				EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
					return(FALSE);
			}
			else
			{
				if(Len == 0)
				{
					ATS_Count = 1;
					Utils_IToA(ATS_Count,ATS_Count_c);      
					ATS_COUNT_OK = FALSE;
				}
				else
				{
					memcpy(ATS_Count_c,Buffer,Len);
					ATS_Count_c[Len] = 0;
					ATS_Count = atoi(ATS_Count_c);

					if (/*(ATS_Count > 18) ||*/ ATS_Count <= 0) /** XLS_PAYMENT 03/08/2001 irfan **/
					{
						ATS_COUNT_OK = TRUE;
					}
					else
					{
						ATS_COUNT_OK = FALSE;
					}
				}
			}
			Utils_LeftPad(ATS_Count_c, '0', 2);
		}while(ATS_COUNT_OK); /* 02_01 irfan 08/09/2000 taksit sayýsý sorma pinpad den */
		ATS_COUNT_OK = TRUE;
		if (( ATS_Count > 1 ) && (!AMEX_FLAG))
		{
			do
			{
				if ( !Mainloop_GetKeyboardInput
					 ( "FAIZSIZ TAKS˜T:1FAIZLI  TAKS˜T:2", Buffer, 0, 1, FALSE, FALSE, FALSE,
					   FALSE, FALSE, &Len, value_null, TRUE ) )
				{
						EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
						EMVAxess = FALSE;
					return ( FALSE );
				}
				else
				{
					if ( Len == 0 )
					{
						taksit_tipi = 1;
						ATS_COUNT_OK = FALSE;
					}
					else
					{
						taksit_tipi = atoi( Buffer );
						if (taksit_tipi==2) IS_LOYALTY_INSTALLMENT_INTEREST = TRUE;
						else IS_LOYALTY_INSTALLMENT_INTEREST = FALSE;
						if (  taksit_tipi > 2 )
						{
							ATS_COUNT_OK = TRUE;
						}
						else
						{
							ATS_COUNT_OK = FALSE;
						}
					}
				}
			}
			while ( ATS_COUNT_OK );	
		}
		else if (AMEX_FLAG)
		{
			taksit_tipi = 1;
			ATS_COUNT_OK = FALSE;
		}
	}

	if ((tran_type == RETURN) || (tran_type == MANUEL))
			if (!Trans_GetApprovalCode()) 
			{
				EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				return(FALSE);
			 }
	if (tran_type == RETURN)
	{
		do
		{
			if(!Mainloop_GetKeyboardInput("SATIž TRH?GGAAYY",Buffer,6,6,
					   FALSE, FALSE, FALSE, FALSE, FALSE,&Len," ", TRUE))
			{
						EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
						EMVAxess = FALSE;
				return(FALSE);
			}
			else
			{
				if(Len == 0)
				{
					FAIZLI_IADE_OK = FALSE;
				}
				else
				{
					sale_date_month[0] = Buffer[2];
					sale_date_month[1] = Buffer[3];
					sale_date_month[2] = 0;
					sale_date_month_value = atoi(sale_date_month);
					sale_date_day[0] = Buffer[0];
					sale_date_day[1] = Buffer[1];
					sale_date_day[2] = 0;
					sale_date_day_value = atoi(sale_date_day);
					if ((sale_date_month_value <=12 ) && (sale_date_month_value >= 1) && 
						(sale_date_day_value <=31   ) && (sale_date_day_value >= 1  )    ) 
					{
						if (sale_date_month_value == 2)
						{
							if (sale_date_day_value > 29)
							{
								FAIZLI_IADE_OK = FALSE;
							}
							else
							{
								/** sisteme gidecek tarih yyaagg olmalý **/
								/*memcpy(Sale_Date_Char, Buffer, 6);*/
								Sale_Date_Char[0] = Buffer[4];
								Sale_Date_Char[1] = Buffer[5];
								Sale_Date_Char[2] = Buffer[2];
								Sale_Date_Char[3] = Buffer[3];
								Sale_Date_Char[4] = Buffer[0];
								Sale_Date_Char[5] = Buffer[1];
								Sale_Date_Char[6] = 0;
								FAIZLI_IADE_OK = TRUE;
							}
						}
						else
						{
							/** sisteme gidecek tarih yyaagg olmalý **/
							/*memcpy(Sale_Date_Char, Buffer, 6);*/
							Sale_Date_Char[0] = Buffer[4];
							Sale_Date_Char[1] = Buffer[5];
							Sale_Date_Char[2] = Buffer[2];
							Sale_Date_Char[3] = Buffer[3];
							Sale_Date_Char[4] = Buffer[0];
							Sale_Date_Char[5] = Buffer[1];
							Sale_Date_Char[6] = 0;
							FAIZLI_IADE_OK = TRUE;
						}
					}
					else
					{
						FAIZLI_IADE_OK = FALSE;
					}
				}
			}
		}while(!FAIZLI_IADE_OK);
	}

	if ( (KampanyaFlag == TRUE) && (kampanya_str != 0))
	{
		char tempKampanya[32];
		char Buffer_[LEN_GETKEYBOARD];
		
		if ( DEBUG_MODE )
		{
	   		Printer_WriteStr("kampanya_str:\n");
			PrntUtil_BufAsciiHex((byte *)kampanya_str, sizeof(kampanya_str)); 
		}

		memset( tempKampanya, 0, 32 );
		strcpy( tempKampanya, kampanya_str );
		strcat( tempKampanya, " ?" );

/*	   	Printer_WriteStr("tempKampanya:\n");
		PrntUtil_BufAsciiHex((byte *)tempKampanya, sizeof(tempKampanya)); */
/*@bm  if(!Mainloop_GetKeyboardInput(tempKampanya,Buffer_,0,16, FALSE,
					 FALSE, FALSE, FALSE, FALSE,&Len, value_null, TRUE)) */

		if ( !Mainloop_GetKeyboardInput( tempKampanya, Buffer_, 0, 15, FALSE,
										 FALSE, FALSE, FALSE, TRUE, &Len,
										 " ", TRUE ) )
		{
			EMVAxessStatu=FALSE; /*@bm 14.09.2004 */
			EMVAxess = FALSE;
			return ( FALSE );
		}
		memset( Kampanya_Data, ' ', sizeof( Kampanya_Data ) );
		memcpy( Kampanya_Data, Buffer_, Len );

/*		Printer_WriteStr("encrypted_PIN:\n");
		PrntUtil_BufAsciiHex((byte *)encrypted_PIN, sizeof(encrypted_PIN));
		Printer_WriteStr("Buffer_:\n");
		PrntUtil_BufAsciiHex((byte *)Buffer_, sizeof(Buffer_)); */
	}
	else memset( Kampanya_Data, '0', sizeof(Kampanya_Data) );		

	if ( (XLS_Check_Initialization_State() == XLS_OPERATIVE) &&
	     ( (LOYALTY_PERMISSION_FLAG) && ((POS_Type == '4') || (POS_Type == '6')) &&
		    ( (tran_type == SALE )   ||
		      (tran_type == MANUEL)  ||
		      (tran_type == RETURN))))
	{
		if(tran_type != RETURN)
		{
			/** XLS_INTEGRATION 09/08/2001 irfan **/
			memset(XLS_Cardno, 'F', sizeof(XLS_Cardno));
			memcpy(XLS_Cardno+(sizeof(XLS_Cardno)-1)-strlen(card_no),card_no,strlen(card_no));
			XLS_Cardno[19] = 0;  /** XLS_INTEGRATION 16/08/2001 20 idi. 19 yapýldý. **/
			xls_ret_local = XLS_VirtualRedemption( amount_surcharge, XLS_Cardno, amount_surcharge,1);
			amount_surcharge[13] = 0;
			if( xls_ret_local != XLS_NO_ERROR)
			{
				if(xls_ret_local != 1) 
				{
					memset(xls_temp_local, 0, sizeof(xls_temp_local));
					sprintf(xls_temp_local, " virtual REDEMPT. HATALI Hata Kodu: %d ", xls_ret_local);
					Mainloop_DisplayWait(xls_temp_local,1);
					PrntUtil_Cr(2);
					Slips_PrinterStr(xls_temp_local);
					PrntUtil_Cr(2);
				}
				EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				return(FALSE); 
 			}
 			xls_ret_local = XLS_VirtualReward(ATS_Count_c, (txls_transactiondetail*)Received_Buffer);
			if( xls_ret_local != XLS_NO_ERROR)
			{
				if(xls_ret_local != 1) /** islem kesilirse asagidaki mesaj verilmeyecek **/
				{
					memset(xls_temp_local, 0, sizeof(xls_temp_local));
					sprintf(xls_temp_local, " Virtual REWARD. HATALI Hata Kodu: %d ", xls_ret_local);
					Mainloop_DisplayWait(xls_temp_local,1);
					PrntUtil_Cr(2);
					Slips_PrinterStr(xls_temp_local);
					PrntUtil_Cr(2);
				}
				EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				return(FALSE); 
			}
			
		}
		else
		{
			if (( POS_Type == '5' ) || ( POS_Type == '6' ))	/*04.08.2004*/
			{
				  if (TL_Iade) XLS_Currency = 0;
				  else XLS_Currency = 1;
			}
			else XLS_Currency = 0;

			xls_ret_local = XLS_VirtualReverse(4, XLS_Currency, amount_surcharge, amount_surcharge_reverse, ATS_Count_c, XLS_Cardno_local, (txls_Returndetail*)Received_Buffer);
			if(Utils_Compare_Strings(amount_surcharge_reverse, amount_surcharge) != 1)
			{/* amount_surcharge_reverse < and = amount_surcharge */
				memcpy(amount_surcharge, amount_surcharge_reverse,13);
				amount_surcharge[13] = 0;
			}
			if( xls_ret_local != XLS_NO_ERROR)
			{
				if(xls_ret_local != 1) /** islem kesilirse asagidaki mesaj verilmeyecek **/
				{
					memset(xls_temp_local, 0, sizeof(xls_temp_local));
					sprintf(xls_temp_local, " virtual REVERSE. HATALI Hata Kodu: %d ", xls_ret_local);
					Mainloop_DisplayWait(xls_temp_local,1);
					PrntUtil_Cr(2);
					Slips_PrinterStr(xls_temp_local);
					PrntUtil_Cr(2);
				}
				EMVAxessStatu = FALSE; /*@bm 14.09.2004 */
				EMVAxess = FALSE;
				return(FALSE); 
			}
		}
		OFFLINE_TRX = FALSE;
 	}
	return(TRUE);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_Get_KasierID 
 *
 * DESCRIPTION          : Get Kasier ID 
 *
 * RETURN               : 
 *
 * NOTES                : bm YTL  01/08/04
 *
 * ------------------------------------------------------------------------ */
boolean Trans_Get_KasierID(void)
{

	char temp_print[25];

	/* Check for installation */
	if ( !INSTALLATION)
	{
		Mainloop_DisplayWait(" KURULUž YAPIN  ",1);   
		return(STAT_NOK);
	}

	/* Check for incomplete EOD transcation */
	if (EOD_flag)
	{
		Mainloop_DisplayWait(" GšNSONU YAPIN  ",1);   
		return(STAT_NOK);
	}

	memset(Kasiyer_No, ' ' , sizeof(Kasiyer_No));
	memcpy(Kasiyer_No, card_no,strlen(card_no));

	/* bm gec   
	memset(temp_print, 0 , sizeof(temp_print));
	memcpy(temp_print,Kasiyer_No,sizeof(Kasiyer_No));
	Slips_PrinterStr("\n");
	Slips_PrinterStr(temp_print);
	Slips_PrinterStr("\n");
	/* bm gec */

	Mainloop_DisplayWait(" KAS˜YER ID      TANIMLANDI",2);

}

void Trans_SetBaudRate(void)
{
		byte entry_no;
		menu csMENU_menu;
		entry csMENU_entry []=
					{
						{"Baud  4800bps", 0, 0,  0},
						{"Baud  9600bps", 0, 0,  0},
						{"Baud 19200bps", 0, 0,  0}					
					};
		csMENU_menu.header		= "BAUD RATE";
		csMENU_menu.no_of_entries	= 3;
		csMENU_menu.menu_entries	= &csMENU_entry[0];
		csMENU_menu.mode_flags		= DEFAULT_MODE|MENU_TIMEOUT_30_SEC;
		entry_no = Formater_GoMenu ((struct menu *)&csMENU_menu,CHOICE_MODE);
		switch(entry_no)
		{
		case 1:
			BFIRMA_BAUD = 4800;
			break;
		case 2:
			BFIRMA_BAUD = 9600;
			break;
		case 3:
			BFIRMA_BAUD = 19200;
			break;
		default:
			BFIRMA_BAUD = 4800;
		}
		EmvIF_ClearDsp(DSP_MERCHANT);
}


void Trans_EnterPhoneNum()
{
	char Buffer[LEN_GETKEYBOARD];
	char value_null[2];
	byte Len;

	if(INSTALLATION) /*@bm 20.09.2004 */
	{
		if ( !Mainloop_GetKeyboardInput
			 ( ( char * ) "  TELEFON NO 1 ?", Buffer, 0, 15, FALSE, FALSE,
			   FALSE, FALSE, TRUE, &Len, value_null, TRUE ) )
		{
			Mainloop_DisplayWait( "TEL NO G˜R˜ž HATASI", 2 );
			return;
		}
		if ( Len != 0 )
		{
			memset( tel_no_1, 0, 16 );
			memcpy( tel_no_1, Buffer, Len );
			tel_no_1[Len] = 0;
		}
		if ( !Mainloop_GetKeyboardInput
			 ( ( char * ) "  TELEFON NO 2 ?", Buffer, 0, 15, FALSE, FALSE,
			   FALSE, FALSE, TRUE, &Len, value_null, TRUE ) )
		{
			Mainloop_DisplayWait( "TEL NO G˜R˜ž HATASI", 2 );
			return;
		}
		if ( Len != 0 )
		{
			memset( tel_no_2, 0, 16 );
			memcpy( tel_no_2, Buffer, Len );
			tel_no_2[Len] = 0;
		}	  
		if ( Files_WriteGenParams(  ) != STAT_OK )
		{
			Slips_PrinterStr( "\n\nTELEFON NO PAR. YAZILAMADI\n\n" );
			Mainloop_DisplayWait( "TELEFON NO PAR.    YAZILAMADI   ", 1 );
			return ;
		}
	}
	else
	{
		if ( !Mainloop_GetKeyboardInput
			 ( ( char * ) "  TELEFON NO   ?", Buffer, 0, 15, FALSE, FALSE,
			   FALSE, FALSE, TRUE, &Len, value_null, TRUE ) )
		{
			Mainloop_DisplayWait( "TEL NO G˜R˜ž HATASI", 2 );
			return;
		}
		if ( Len != 0 )
		{
			/* initialie the tel no 1 */
			memset( tel_no_1, 0, 16 );
			memcpy( tel_no_1, Buffer, Len );
			tel_no_1[Len] = 0;
			memset( tel_no_2, 0, 16 );
			memcpy( tel_no_2, Buffer, Len );
			tel_no_2[Len] = 0;
		}	
	}

	/*              Hat Tipi                 */
	/*-----------------------*/
	do
	{
		if ( !Mainloop_GetKeyboardInput
			 ( ( char * ) "HAT T˜P˜>0:T 1:P", Buffer, 0, 1, FALSE, FALSE,
			   FALSE, FALSE, FALSE, &Len, value_null, TRUE ) )
		{
			Mainloop_DisplayWait( "   HAT T˜P˜       G˜R˜ž HATASI  ", 2 );
			return;
		}
		if ( Len != 0 )
		{
			/* initialie the Line_Type */
			Line_Type = Buffer[0];
		}
		else
		{
			Line_Type = '0';
		}
	}
	while ( ( Line_Type != '0' ) && ( Line_Type != '1' ) );

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_BinDownload.
 *
 * DESCRIPTION          : Donwload the Acceptable BIN's for offline pos
 *
 * RETURN               : STAT_OK
 *                        STAT_NOK
 *                        TRR_NO_CONNECTION
 *
 * NOTES                : bm OFF  21.10.2004
 *
 * ------------------------------------------------------------------------ */
byte Trans_Bin_Download( void )
{
	boolean DONE;
	byte ParamDonwload_ret;
	char ParamLoad_temp[33];
	int i;
	byte download_ret;
	char lbSend_Buffer[900];
	char last_location[5];
	char device_date[8], device_time[6];


	if ( Offline_BIN_File(  ) != STAT_OK )	/*14.07.2004 Offline BIN dosyasý oluþturma   */
	{
		PrntUtil_Cr( 2 );
		Slips_PrinterStr( "***OFFLINE BIN DOSYA***\n" );
		Slips_PrinterStr( "*** OLUSTURULAMADI  ***" );
		PrntUtil_Cr( 2 );
		Mainloop_DisplayWait( "OFFLINE BIN DOSYASI OLUSTURULAMADI   ", 2 );
		return ( STAT_NOK );
	}

	DONE = FALSE;
	memset( EOD_error_message, ' ', sizeof( EOD_error_message ) );


	/* initialize the send structure */
	memset( &d_Param_Continue_Send, 0, sizeof( d_Param_Continue_Send ) );

/* bm OFF  21.10.2004  */
	/* irfan 02/09/2002 loop'un icinde pos ver no degisebildigi icin atanmasi gerekir*/
	d_Param_Continue_Send.Header_Param.POS_software_ver = POS_software_ver;

	/* bm YTL 21/06/2004 */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		d_Param_Continue_Send.Header_Param.Cur_Index = 'X';
	else
		d_Param_Continue_Send.Header_Param.Cur_Index = 'Y';
	d_Param_Continue_Send.Header_Param.ActAppCode = POS_Type;
	d_Param_Continue_Send.Header_Param.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
	memcpy( d_Param_Continue_Send.Header_Param.PosType, "LIP", 3 );
	memcpy( d_Param_Continue_Send.Header_Param.PosModel, "8000", 4 );
	memcpy( d_Param_Continue_Send.Header_Param.PosVersion, POS_VERSION_NUMBER, 4 );
	memset( d_Param_Continue_Send.Header_Param.Cashier_Id, '0', 16 );
	memset( d_Param_Continue_Send.Header_Param.IMEI_ID, '0', 15 );
	memset( d_Param_Continue_Send.Header_Param.Filler, ' ', 19 );

	Utils_GetDateTime( device_date, device_time );
	d_Param_Continue_Send.Header_Param.PosDate[0] = device_date[6];
	d_Param_Continue_Send.Header_Param.PosDate[1] = device_date[7];
	d_Param_Continue_Send.Header_Param.PosDate[2] = device_date[3];
	d_Param_Continue_Send.Header_Param.PosDate[3] = device_date[4];
	d_Param_Continue_Send.Header_Param.PosDate[4] = device_date[0];
	d_Param_Continue_Send.Header_Param.PosDate[5] = device_date[1];
	d_Param_Continue_Send.Header_Param.PosTime[0] = device_time[0];
	d_Param_Continue_Send.Header_Param.PosTime[1] = device_time[1];
	d_Param_Continue_Send.Header_Param.PosTime[2] = device_time[3];
	d_Param_Continue_Send.Header_Param.PosTime[3] = device_time[4];

	/* fill Param_Continue_Send structure */
	strcpy( last_location, "9995" );	/* continue to download BIN params */
	last_location[4] = 0;

	d_Param_Continue_Send.Header_Param.MSG_INDICATOR = 'E';
	/* fill Param_Continue_Send structure */
	d_Param_Continue_Send.Header_Param.MSG_TYPE = '6';
	d_Param_Continue_Send.Header_Param.POS_software_ver = POS_software_ver;
	memcpy( d_Param_Continue_Send.Header_Param.merch_device_no,	merch_device_no, 8 );
	d_Param_Continue_Send.Header_Param.device_serial_no = device_serial_no;
	d_Param_Continue_Send.Header_Param.transaction_type = '3';
	memcpy( d_Param_Continue_Send.param_location_no, last_location, 4 );

	GrphText_Cls( TRUE );
	Debug_GrafPrint2( "BIN      YšKLEME", 2 );

	while ( !DONE )
	{
		/* irfan 02/09/2002 loop'un icinde pos ver no degisebildigi icin atanmasi gerekir */
		d_Param_Continue_Send.Header_Param.POS_software_ver =	POS_software_ver;

		memset( lbSend_Buffer, 0, 800 );

		/* Prepare the Send Message */
		memcpy( lbSend_Buffer, &d_Param_Continue_Send,	sizeof( d_Param_Continue_Send ) );

		if ( BUYUKFIRMA )
			ParamDonwload_ret =
				Host_BFirmaSendReceive( lbSend_Buffer, TRUE, TRUE, TRUE, TRUE );
		else
			ParamDonwload_ret =
				Host_SendReceive( lbSend_Buffer, TRUE, TRUE, TRUE );

		if ( ParamDonwload_ret != STAT_OK )
		{
			Debug_GrafPrint2( "BIN.YUKLENEMEDI ", 3 );
			return ( ParamDonwload_ret );
		}

		/* analyse the response */
		/* Move Received_Buffer to the d_Param_Continue_Receive */
		memcpy( &d_Param_Continue_Receive, Received_Buffer + 1,	sizeof( d_Param_Continue_Receive ) );
		if ( d_Param_Continue_Receive.Header_Param.MSG_TYPE != '7' )
		{
			Slips_PrinterStr( "BEKLENMEYEN MESAJ KODU\n" );
			memset( ParamLoad_temp, 0, sizeof( ParamLoad_temp ) );
			sprintf( ParamLoad_temp, "BEKLENMEYEN MES.   KODU: %c",
					 d_Param_Continue_Receive.Header_Param.MSG_TYPE );
			Mainloop_DisplayWait( ParamLoad_temp, 1 );	/* NO_WAIT yapýlabilir */
			return ( STAT_NOK );
		}

		/*UPDATE 01_16 26/10/99 giden ve gelen mesajlarda LRC kontrolunden gecse dahi hatalý gelen
		 * mesajýn kontrolu icin cihaz no  larý kontrol ediliyor */
		/* Compare merch_device_no */
		if ( memcmp	( d_Param_Continue_Send.Header_Param.merch_device_no,
			   d_Param_Continue_Receive.Header_Param.merch_device_no,
			   8 ) != 0 )
		{
			PrntUtil_Cr( 2 );
			Slips_PrinterStr( "***BOZUK MESAJ ALINDI***\n" );
			Slips_PrinterStr( "YAZILIM YUKLEME TERMINAL NO UYUSMUYOR" );
			PrntUtil_Cr( 7 );
			Mainloop_DisplayWait( "YAZILIM YUKLEME TERMINAL NO UYUSMUYOR",
								  1 );
			return ( STAT_NOK );
		}
		POS_software_ver = ( d_Param_Continue_Receive.Header_Param.POS_software_ver );	/* asagýda var sonra silinecek */
		Debug_GrafPrint2( "YAZ. VER.NO DE¦.", 3 );
		download_ret = Trans_UpdateParameters(  );
		if ( download_ret == STAT_OK )
			POS_software_ver =	( d_Param_Continue_Receive.Header_Param.POS_software_ver );
		else
		{
			Debug_GrafPrint2( "BIN UPDATE  HATA", 3 );
			return ( STAT_NOK );
		}
		if ( d_Param_Continue_Receive.cont == '1' )	/* continue parameter loading */
		{
			d_Param_Continue_Send.param_location_no[0] =
				d_Param_Continue_Receive.param_location_no[0];
			d_Param_Continue_Send.param_location_no[1] =
				d_Param_Continue_Receive.param_location_no[1];
			d_Param_Continue_Send.param_location_no[2] =
				d_Param_Continue_Receive.param_location_no[2];
			d_Param_Continue_Send.param_location_no[3] =
				d_Param_Continue_Receive.param_location_no[3];
			memcpy( ParamLoad_temp,
					d_Param_Continue_Receive.param_location_no, 4 );
			ParamLoad_temp[4] = 0;
			Debug_GrafPrint2( ( char * ) ParamLoad_temp, 3 );
		}
		else if ( d_Param_Continue_Receive.cont == '2' )	/* stop parameter loading */
		{
/*
			Printer_WriteStr("ContTypeFlag:\n");
			PrntUtil_BufAsciiHex((byte *)&ContTypeFlag,1);
*/
			EOD_OK = TRUE;
			DONE = TRUE;
			Debug_GrafPrint2( "     B˜TT˜.     ", 3 );

		}
		else
		{
			Mainloop_DisplayWait( "   DEVAM KODU     ANLAžILAMADI  ", 2 );
			EOD_OK = FALSE;
			DONE = TRUE;
			return ( STAT_NOK );
		}
	}

	return ( STAT_OK );

}

