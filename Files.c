
/**************************************************************************
  FILE NAME:   FILES.C
  MODULE NAME: 
  PROGRAMMER:  
  DESCRIPTION: 
  REVISION:    
 **************************************************************************/

	  /*==========================================*
	   *         I N T R O D U C T I O N          *
	   *==========================================*/
/* void */

	  /*==========================================*
	   *             I N C L U D E S              *
	   *==========================================*/
/* General Header File */
/*#include "config.h"*/

#include "project.def"

/* MCC68K Header Files */

/*UPDATE 01_16 26/10/99 sprintf gibi komutlarýn prototype'ý için konuldu*/
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/*#include <mriext.h>*/

#include NOS_H
#include DEFINE_H
#include ERRORS_H
#include UTILS_H
#include MESSAGE_H
#include FILES_H
#include SLIPS_H
#include TRANS_H
#include DEBUG_H
#include MAINLOOP_H		/* XLS_PAYMENT kapali idi. acildi. irfan 06/08/2001 */

extern boolean Debug;
/* bizim tanýmlar */
extern struct Transaction_Receive d_Transaction_Receive;	/* defined in trans.c*/
extern char card_no[20];					/* defined in trans.c*/
extern int record_counter;
extern int ATS_Count;
extern int tran_type;
extern char ATS_Count_c[3];
extern char msgSlips[25];

const char Transaction_File[]   = "AKBANK";
const char ATS_File[]           = "TAKSIT";
const char EMVOffline_File[]	= "OFFEMV";
const char EMVOnline_File[]	= "ONLEMV";
const char LastTrans_File[]	= "LASTTR";
const char Exception_File[]	= "EXCTFL";
const char Revocation_File[]	= "RVCTFL";
const char GeneralParams_File[] = "PARAMS";
const char TransParams_File[]   = "TRANSP";
const char Prefix_File[]        = "PREFIX";
const char Statistics_File[]    = "STATIS";
const char GSM_File[]			= "GSM";
const char Transaction_GFile[]   = "GENIUS";
const char TerminalAppList_File[] = "emvapp";
const char PublicKey_File[] 	= "emvpk_";       
const char OfflineBin_File[] = "OFFBIN";	/* bm OFF 21.10.2004 */

sint Batch_record_index;

/* genius */
sint GBatch_record_index;
extern boolean ReversalPending;	/* 04_03 19/11/2002 */
extern char last_genius_eod_date[9];
extern char POS_Type;
extern char genius_flag;


extern char GPOS_Type[3];
extern char GPOS_Model[4];
extern char GEMV_Key_Version_No;
extern char GConfig_Version_No;
extern char GPOS_Serial_No[9];
extern char GPOS_Akbank_No_My2[7];
extern char GPinpad_Serial_No[9];
extern char GPinpad_Akbank_No[7];
extern char GActive_Application_Code;
extern char GConnection_Type;
extern boolean BUYUKFIRMA;

typedef struct
{
   sint   Handle;
   byte   FileName[RD_FILENAME_LEN];
   usint  Clusters;
   lint   ByteSize;
   byte   RecordSize;
   usint  RecordVer;
   usint  RcrdSize2Byte;
}My_File_Entry;
extern boolean AutoEOD;
extern boolean LAST_4_DIGIT_FLAG;
extern boolean INSTALLATION;
extern boolean EOD_flag;
extern boolean ATS_FLAG;
extern char EOD_pointer;
extern char Line_Type;                                        
extern char device_serial_no;                          
extern char master_key_no;
extern char POS_software_ver;
extern char UserPassword[5];
extern char OperatorPassword[15];
extern char YTL_Slip[3];		/* bm YTL 25/06/2004 TLY - YTL */
/* bm opt extern char OperatorPassword[15]; */
extern char merch_AMEX_no[11];         /* merch name , merch addr1+ merchaddr2 */                      
extern char merch_addr[73];                            
extern char merch_no[13];                                      
extern char merch_device_no[9];                                
extern char auto_EOD_time[5]; 
extern int auto_EOD_time_int;
extern char tel_no_1[16];      
extern char tel_no_2[16];
extern char working_key[16];
extern char batch_no_c[4];
extern char param_ver_no[4];
extern char last_eod_date[9];
extern char last_transact_status;
extern char seq_no_c[5];
extern int  seq_no;
extern char last_transact_batch_no[4];
extern char last_transact_seq_no[5];
extern char net_total[14];
extern char agreement_total[14];                       
extern char ATS_Prefixes[90];
extern struct Statistics	d_Statistics1;
extern struct Statistics	d_Statistics2;
extern struct Accummulators	d_Totals_Counts[4];
extern ulint total_batch_time;
extern boolean FAIZLI_FLAG;
extern char amount_faiz_kismi[14];	/* faizli islemde faiz kismini icerir */
extern boolean OFFLINE_TRX;		/* TRUE	: Offline Trx. FALSE : Online Trx. */
extern boolean CallFlag;
extern char amount_surcharge[14];
extern struct Transaction_Send d_Transaction_Send;
extern char GEMV_Key_Version_No;
extern char GConfig_Version_No;
extern boolean ReversalPending;
extern boolean VERDE_TRANSACTION;
extern boolean EXTERNAL_PINPAD;
extern char amount_surcharge_kismi[14];	/* surcharge islemde surcharege kismini icerir */
extern char POS_Type;
extern int KAMPANYA_FLAG;
extern struct Receipt_Structure d_Receipt;
extern char Reversal_Buffer[900];
extern boolean ERROR_RECEIPT;

const char ReversalTrans_File[] = "REVERSAL";	/*bm opt */
sint Batch_record_index;

extern char Kasiyer_No[20];			/*bm YTL 13.08.2004 */
//extern sint Cashier_Flag;			/*bm YTL 01/08/2004 Cashier menu */
extern boolean KampanyaFlag; /*@bm 16.09.2004 */

extern char APN[12];
extern char glb_host_ip[17];
extern usint glb_host_port;
extern boolean GPRS_USEGPRS;
extern boolean GPRS_CheckMode;
extern ulint GPRS_TryTime;
extern boolean autoGSMRetry;
extern char GPRS_User[10];
extern char GPRS_Passwd[10];

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
/******************* genius ********************/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Files_GRecordWrite
 *
 * DESCRIPTION          : Write a genius record to genius batch file
 *
 * RETURN                       : STAT_OK, STAT_NOK or STAT_INV    
 *
 * NOTES                        : r.i.o. 09/10/2002       
 *
 * ------------------------------------------------------------------------ */
byte Files_GRecordWrite( void);

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GAddTransaction
 *
 * DESCRIPTION: Adds information to the Genius transaction database when EOF is received. 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_INV )      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
sint Files_GAddTransaction(GTransactionStruct *);

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GInitFileGenius
 *
 * DESCRIPTION: Inits the Genius transaction file "GENIUS"
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_GInitFileGenius(void);

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GReadTransaction
 *
 * DESCRIPTION	: Reads from the genius transaction genius database.
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_GReadTransaction(GTransactionStruct *);

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GVoidTransaction
 *
 * DESCRIPTION	: Transaction is marked as void in Genius Database in Ramdisk.
 *
 * RETURN		: STAT_OK, STAT_NOK or STAT_INV    
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_GVoidTransaction(GTransactionStruct *);

/* EMV Code Insertion Starts - MTK - 10.04.2002 */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_ExceptionRead
 *
 * DESCRIPTION: 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_ExceptionRead(byte *aExceptionByteList, sint index)
{
	sint lbStatus;
	byte lwStatus;
	sint Handle1;
	 sint handle;

	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)Exception_File)==RD_CHECK_OK)
	{
		handle = RamDisk_Reopen((byte *)Exception_File)->Handle;
		if (RamDisk_Seek(handle, index - 1) >= 0)
		{
			lbStatus = RamDisk_Read(handle,aExceptionByteList);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
			    lwStatus = STAT_OK ;
			}
			else if (lbStatus == RD_EOF) lwStatus = STAT_EOF;
		}
		else lwStatus = STAT_EOF;
	}
	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_ExceptionUpdate
 *
 * DESCRIPTION: 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_ExceptionUpdate(byte *aExceptionByteList, sint index)
{
	sint Handle1;
	 sint handle;
	sint lwLastSeq;

	if (RamDisk_ChkDsk((byte *)Exception_File)==RD_CHECK_OK)
	{
		handle = RamDisk_Reopen((byte *)Exception_File)->Handle;
		if (RamDisk_Seek(handle, index-1) >= 0)
		{
			if (RamDisk_Overwrite(handle,(byte *)aExceptionByteList) == RD_OK)  return(STAT_OK);
			else Printer_WriteStr("Overwrite Fail\n");
		}
		else Printer_WriteStr("File Find Fail\n");
		return (STAT_NOK);
	}
	Printer_WriteStr("ChkDsk Fail\n");
	return (STAT_INV);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_ExceptionAppend
 *
 * DESCRIPTION: 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_ExceptionAppend(byte *aExceptionByteList)
{
	sint Handle1;
	 sint handle;
	
	if (RamDisk_ChkDsk((byte *)Exception_File)==RD_CHECK_OK)
	{
		handle = RamDisk_Reopen((byte *)Exception_File)->Handle;
		if (RamDisk_Append(handle,(byte *)aExceptionByteList)>0)  return(STAT_OK);
		Printer_WriteStr("Exception Append Error\n");
		return (STAT_NOK);
	}
	Printer_WriteStr("Exception Chkdsk Error\n");
	return (STAT_INV);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_RevocationRead
 *
 * DESCRIPTION: 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_RevocationRead(byte *aRevocationByteList, sint index)
{
	sint lbStatus;
	byte lwStatus;
	sint Handle1;
	 sint handle;

	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)Revocation_File)==RD_CHECK_OK)
	{
		handle = RamDisk_Reopen((byte *)Revocation_File)->Handle;
		if (RamDisk_Seek(handle, index - 1) >= 0)
		{
			lbStatus = RamDisk_Read(handle,aRevocationByteList);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
			    lwStatus = STAT_OK ;
			}
			else if (lbStatus == RD_EOF) lwStatus = STAT_EOF;
		}
		else lwStatus = STAT_EOF;
	}
	return(lwStatus);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_RevocationUpdate
 *
 * DESCRIPTION: 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_RevocationUpdate(byte *aRevocationByteList, sint index)
{
	sint Handle1;
	 sint handle;
	sint lwLastSeq;

	if (RamDisk_ChkDsk((byte *)Revocation_File)==RD_CHECK_OK)
	{
		handle = RamDisk_Reopen((byte *)Revocation_File)->Handle;
		if (RamDisk_Seek(handle, index-1) >= 0)
		{
			if (RamDisk_Overwrite(handle,(byte *)aRevocationByteList) == RD_OK)  return(STAT_OK);
			else Printer_WriteStr("Overwrite Fail\n");
		}
		else Printer_WriteStr("File Find Fail\n");
		return (STAT_NOK);
	}
	Printer_WriteStr("ChkDsk Fail\n");
	return (STAT_INV);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_RevocationAppend
 *
 * DESCRIPTION: 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_RevocationAppend(byte *aRevocationByteList)
{
	sint Handle1;
	 sint handle;
	
	if (RamDisk_ChkDsk((byte *)Revocation_File)==RD_CHECK_OK)
	{
		handle = RamDisk_Reopen((byte *)Revocation_File)->Handle;
		if (RamDisk_Append(handle,(byte *)aRevocationByteList)>0)  return(STAT_OK);
		Printer_WriteStr("Revocation Append Error\n");
		return (STAT_NOK);
	}
	Printer_WriteStr("Revocation Chkdsk Error\n");
	return (STAT_INV);
}
/* EMV Code Insertion Ends   - MTK - 10.04.2002 */

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_InitFile
 *
 * DESCRIPTION: Initializes a chosen file.
 *
 * RETURN: STAT_OK or STAT_NOK
 *
 * NOTES                : t.c.a & r.i.o  15/06/99
 *
 * ------------------------------------------------------------------------ */
byte Files_InitFile(char *lbaFileName, usint Record_Size)
{
	sint NewFileRet;
	char lbtemp[100];


	RamDisk_EraseFile((byte *)lbaFileName);
	NewFileRet = RamDisk_NewFile((byte *)lbaFileName,Record_Size,0);
	if ((NewFileRet == RD_OUT_OF_ENTRIES) ||
		(NewFileRet == RD_OUT_OF_MEM) ||
		(NewFileRet == RD_OUT_OF_DIR) ||
		(NewFileRet == RD_ERROR))

		return(STAT_NOK);

	return(STAT_OK);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_InitEMVTransaction
 *
 * DESCRIPTION: Inits the transaction file "AKBANK"
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_InitEMVTransaction(boolean Light)
{
	PTStruct ptPTStruct;

	if (Light)
	{
		if (Files_InitFile((char *)EMVOnline_File, sizeof(d_Transaction_Send)) == STAT_OK) return(STAT_OK);

	}
	else
	{
		if ((Files_InitFile((char *)EMVOffline_File, sizeof(d_Transaction_Send)) == STAT_OK) &&
			(Files_InitFile((char *)EMVOnline_File, sizeof(d_Transaction_Send)) == STAT_OK) &&
			(Files_InitFile((char *)LastTrans_File,sizeof(ptPTStruct)) == STAT_OK)) return(STAT_OK);
	}
	return (STAT_NOK); 
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_AddEMVTransaction
 *
 * DESCRIPTION: Adds information to the transaction database when EOF is received. 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_INV )      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
sint Files_AddEMVTransaction(byte *FileName, PTStruct *ptPTStruct)
{
	sint Handle1;
	sint RamDisk_ret;

	if (Files_LastTransWrite(ptPTStruct) == STAT_OK)
		if (RamDisk_ChkDsk((byte *)FileName)==RD_CHECK_OK)
		{
			Handle1 = RamDisk_Reopen((byte *)FileName)->Handle;
			RamDisk_ret = RamDisk_Append(Handle1,&d_Transaction_Send);
			return(RamDisk_ret);
		}
	return (RD_ERROR);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_RemoveEMVTransaction
 *
 * DESCRIPTION: Reads from the transaction database.
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
sint Files_RemoveEMVTransaction(byte *FileName, TransactionStruct *fpTransStruct)
{
	sint lbStatus, lwLastSeq, i;
	byte lwStatus;
	sint Handle1;
	struct Transaction_Send Temp_Transaction_Send;

	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)FileName)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)FileName)->Handle;
		lwLastSeq = RamDisk_Seek(Handle1,SEEK_LAST);
                RamDisk_Seek(Handle1, SEEK_FIRST);		
		for (i=0; i<lwLastSeq; i++)
		{
			if (RamDisk_Seek(Handle1, i) >= 0)
			{
				lbStatus = RamDisk_Read(Handle1, &Temp_Transaction_Send);
				if ((lbStatus >= 0) || (lbStatus == RD_LAST))
				{
					if (memcmp(Temp_Transaction_Send.transaction_seq_no, fpTransStruct->seq_no, length_seq_no) == 0)
					{
						RamDisk_Delete(Handle1);
						lwStatus = STAT_OK;
					}
				}
				else 
				{
					if (lbStatus == RD_EOF)
					{
						lwStatus = STAT_EOF;
					}
				}
			}
			else 
			{
				lwStatus = STAT_EOF;
			}
		}
	}
	return(lwStatus);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_AddTransaction
 *
 * DESCRIPTION: Adds information to the transaction database when EOF is received. 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_INV )      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
sint Files_AddTransaction(TransactionStruct *fpTransStruct)
{
	sint Handle1;
	sint RamDisk_ret;


	if (RamDisk_ChkDsk((byte *)Transaction_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)Transaction_File)->Handle;
                if (RamDisk_Seek(Handle1, SEEK_LAST) >= 0)		
                      RamDisk_ret = RamDisk_Append(Handle1,fpTransStruct);
	
		if(RamDisk_ret > 0)
			record_counter = RamDisk_ret;

		return(RamDisk_ret);
	}

	return (RD_ERROR);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_ReadTransaction
 *
 * DESCRIPTION: Reads from the transaction database.
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_ReadTransaction(TransactionStruct *fpTransStruct)
{
	sint lbStatus, lwLastSeq, i;
	byte lwStatus;
	sint Handle1;
	TransactionStruct TempTransStruct;

	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)Transaction_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)Transaction_File)->Handle;
		lwLastSeq = RamDisk_Seek(Handle1,SEEK_LAST);
                RamDisk_Seek(Handle1, SEEK_FIRST);		
		for (i=0; i<lwLastSeq; i++)
		{
			if (RamDisk_Seek(Handle1, i) >= 0)
			{
				lbStatus = RamDisk_Read(Handle1, &TempTransStruct);
				if ((lbStatus >= 0) || (lbStatus == RD_LAST))
				{
					if (memcmp(TempTransStruct.seq_no, fpTransStruct->seq_no, length_seq_no) == 0)
					{
						Batch_record_index = i;
						memcpy(fpTransStruct, &TempTransStruct, Length_Transaction);
						lwStatus = STAT_OK;
					}
				}
				else if (lbStatus == RD_EOF)
					lwStatus = STAT_EOF;
			}
			else lwStatus = STAT_EOF;
		}
	}

	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_VoidTransaction
 *
 * DESCRIPTION: Transaction is marked as void in Ramdisk.
 *
 * RETURN: STAT_OK, STAT_NOK or STAT_INV    
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_VoidTransaction(TransactionStruct *fpTransStruct)
{
	byte lwStatus;
	sint Handle1;
	sint void_tran_type,i;
	/* FAIZLI 02_00 28/07/2000 kaydýn faizli olup olmadýgýný gösterir*/
	boolean transaction_with_credit;
	char temp_faiz_amount[14];

	lwStatus = STAT_INV;

	if (RamDisk_ChkDsk((byte *)Transaction_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)Transaction_File)->Handle;

		if (RamDisk_Seek(Handle1, Batch_record_index) >= 0)
		{                       
			fpTransStruct->void_status = '1';

			ATS_Count = ((fpTransStruct->installment_number[0] - '0')*10) + 
					    (fpTransStruct->installment_number[1] - '0');
			void_tran_type = fpTransStruct->tran_type - '0';

			/* FAIZLI 02_00 28/07/2000 iptal edilecek kaydýn faizli olup olmadýðý tespit ediliyor */
			memset(temp_faiz_amount, 0 , sizeof(temp_faiz_amount));
			memcpy(temp_faiz_amount, fpTransStruct->amount_faiz, 13);
			temp_faiz_amount[13] = 0;

			transaction_with_credit = FALSE;
			for (i=0; i<13 ; i++)
			{
				if (temp_faiz_amount[i] != '0')
					transaction_with_credit = TRUE;
			}


			if (RamDisk_Overwrite(Handle1,(byte *)fpTransStruct) == RD_OK) 
			{
				/* FAIZLI 02_00 25/07/2000 Eðer faizli taksitli ise ATS database i update edilmiyor */
				if ((ATS_Count > 1) && 
				(!transaction_with_credit) &&
			    ((void_tran_type == RETURN) || 
			     (void_tran_type == SALE  ) || 
			     (void_tran_type == MANUEL))   )

				{
					return(Files_VoidAtsTransaction(fpTransStruct));
				}
				else
					lwStatus = STAT_OK;
			}
		}
	}

	return(lwStatus);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Files_RecordWrite
 *
 * DESCRIPTION          : Write a record to batch file
 *
 * RETURN                       : STAT_OK, STAT_NOK or STAT_INV    
 *
 * NOTES                        : r.i.o. & t.c.a 07/06/99       
 *
 * ------------------------------------------------------------------------ */
byte Files_RecordWrite(boolean IsEMVOffline)
{
	TransactionStruct fpTransStruct;
	sint Status;
	
	char offline_approval_code[7];			/** XLS_PAYMENT 01/08/2001 irfan **/
	char device_date[8], device_time[6];	/** XLS_PAYMENT 01/08/2001 irfan **/

	/* fill the record */
	memset(&fpTransStruct,' ',sizeof(fpTransStruct));

	/** XLS_PAYMENT 01/08/2001 irfan **/
	/* 06_30 06/03/2003 OFFLINE_TRX kaldirma */
	if (IsEMVOffline)/*(OFFLINE_TRX)*/
	{
		memcpy(fpTransStruct.seq_no, seq_no_c, 4);
/*		memcpy(fpTransStruct.card_type, "15", 2);*/
		memcpy(fpTransStruct.card_type, "29", 2);
		memcpy(fpTransStruct.amount_surcharge, amount_surcharge, 13);

		/** offline approval code is generated **/
		memset(offline_approval_code, 0, sizeof(offline_approval_code));
		memcpy(offline_approval_code, batch_no_c,3);
		memcpy(offline_approval_code+3,seq_no_c+1,3);
		memcpy(fpTransStruct.approval_code, offline_approval_code, 6);
		
		/** assign pos date and time **/
		Utils_GetDateTime(device_date,device_time);
		memcpy(&fpTransStruct.date[0], &device_date,8);
		memcpy(fpTransStruct.time, device_time,5);
		fpTransStruct.tran_type   = tran_type + '0';
		fpTransStruct.agreement   = '1';
		fpTransStruct.Offline = 1;
	}
	else
	{
		memcpy(fpTransStruct.seq_no, d_Transaction_Receive.transact_seq_no, 4);
		memcpy(fpTransStruct.card_type, d_Transaction_Receive.card_type, 2);
		memcpy(fpTransStruct.amount_surcharge, d_Transaction_Receive.amount_surcharge, 13);
		memcpy(fpTransStruct.approval_code, d_Transaction_Receive.approval_code, 6);
		memcpy(&fpTransStruct.date[0], &d_Transaction_Receive.date[4], 2);
		memcpy(&fpTransStruct.date[3], &d_Transaction_Receive.date[2], 2);
		memcpy(&fpTransStruct.date[6], &d_Transaction_Receive.date[0], 2);
		fpTransStruct.date[2] = '/';
		fpTransStruct.date[5] = '/';
		memcpy(fpTransStruct.time, d_Transaction_Receive.time, 2);
		memcpy(&fpTransStruct.time[3], &d_Transaction_Receive.time[2], 2);
		fpTransStruct.time[2] = ':';    
		fpTransStruct.tran_type   = d_Transaction_Receive.Header_Transact.transaction_type;
		fpTransStruct.agreement   = d_Transaction_Receive.agreement;
/*		memcpy( fpTransStruct.kampanya_str, d_Receipt.kampanya_str, sizeof( fpTransStruct.kampanya_str ) );*/
		if (KampanyaFlag == TRUE) memcpy( fpTransStruct.kampanya_str, msgSlips, sizeof( fpTransStruct.kampanya_str ) );
		else memset( fpTransStruct.kampanya_str, ' ', sizeof( fpTransStruct.kampanya_str ) );
		fpTransStruct.Offline = 0;
	}
	memcpy(fpTransStruct.card_no, card_no, strlen(card_no));
	fpTransStruct.void_status = '0';

	/* FAIZLI 02_00 25/07/2000 faiz kýsmý saklanýyor*/
	memcpy(fpTransStruct.amount_faiz, amount_faiz_kismi, 13);
	memcpy(fpTransStruct.installment_number, ATS_Count_c, 2);

	memcpy(fpTransStruct.Kasiyer_Id, Kasiyer_No , 16); /*bm YTL 13.08.2004*/

	/* 07_15. 25/11/2002 */
	memcpy(fpTransStruct.surcharge , amount_surcharge_kismi, 13);

/* Toygar EMVed */
/*
	memcpy(fpTransStruct.install_amount_1, d_Transaction_Receive.ATS_Amount_1, length_amount_surcharge);
	memcpy(fpTransStruct.install_amount_2, d_Transaction_Receive.ATS_Amount_2, length_amount_surcharge);
*/
	memcpy(fpTransStruct.install_amount_1, d_Transaction_Receive.ATS_Amount, length_amount_surcharge);
	memcpy(fpTransStruct.install_amount_2, d_Transaction_Receive.ATS_Amount, length_amount_surcharge);
/* Toygar EMVed */

	/* verde "d_Transaction_Receive.ATS_Date[0]" yerine "d_Transaction_Receive.ATS_Date" yapýldý.*/
	/*memcpy(fpTransStruct.first_date, d_Transaction_Receive.ATS_Date[0], length_due_date);*/
	memcpy(fpTransStruct.first_install_date, d_Transaction_Receive.ATS_Date, length_due_date);

	/* write the filled record to the AKBANK batch */
	Status = Files_AddTransaction(&fpTransStruct);
	if (Status > 0)
	{
		/* FAIZLI 02_00 25/07/2000 Eðer faizli taksitli ise ATS database i update edilmiyor */
		if ( (ATS_Count > 1) && 
			 ((tran_type == RETURN) || (tran_type == SALE  ) || (tran_type == MANUEL)) &&
			 (!FAIZLI_FLAG)
		   )
			return(Files_AtsTransWrite());
		else return(STAT_OK);
	}
	else
	{
		switch(Status)
		{
		case  RD_OUT_OF_MEM:
			Mainloop_DisplayWait("   YI¦IN DOLU   GšNSONU YAPINIZ ",1);
		 	return(STAT_NOK);
		default:
			Mainloop_DisplayWait("KAYIT YAZILAMADI",1);
			return(STAT_NOK);
		}
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_WriteTaksitli
 *
 * DESCRIPTION  : Adds/updates information to/in the ATS database. 
 *
 * RETURN       :  RD_ERROR, RD_NOT_FOUND or 
 *                 (when > 0) number of records in database
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
sint Files_WriteTaksitli(AmexTaksitStruct *fpTransStruct, boolean amount_add, boolean cancel_transaction)
{
	sint lbStatus, lwLastSeq, i, record_index;
	sint lwStatus;
	sint Handle1;
	AmexTaksitStruct ReadAtsStruct;
	char result_amount[14], result_table[18][13];	/* XLS_PAYMENT 03/08/2001 irfan */
	int k;				/* 04_10 */

	lwStatus = RD_ERROR;
	if (RamDisk_ChkDsk((byte *)ATS_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)ATS_File)->Handle;

		lwLastSeq = RamDisk_Seek(Handle1,SEEK_LAST);
		RamDisk_Seek(Handle1, SEEK_FIRST);

		lwStatus = RD_OK;

		for (i=0; (i<lwLastSeq) && (lwStatus != 1); i++)
		{
			if (RamDisk_Seek(Handle1, i) >= 0)
			{
				lbStatus = RamDisk_Read(Handle1, &ReadAtsStruct);
				if ((lbStatus >= 0) || (lbStatus == RD_LAST))
				{
					if (memcmp(fpTransStruct->first_date, ReadAtsStruct.first_date, length_due_date) == 0)
					{
						record_index = i;
						lwStatus = 1;
					}
				}
			}
		}
	}

	if (lwStatus > 0)
	{
		/* 04_10 irfan 28/02/2003 toplamlari duzeltmek icin*/
		char IncominglbStr[500],ReadlbStr[500];
		memset(IncominglbStr, 0, sizeof(IncominglbStr));
		memcpy(IncominglbStr, fpTransStruct, sizeof(ReadAtsStruct));
		memset(ReadlbStr, 0, sizeof(ReadlbStr));
		memcpy(ReadlbStr, &ReadAtsStruct, sizeof(ReadAtsStruct));


		Handle1= RamDisk_Reopen((byte *)ATS_File)->Handle;
		lwStatus = RD_ERROR;

		if (RamDisk_Seek(Handle1, record_index) >= 0)
		{
			char original_amount[14], incoming_amount[14];
			char negative_original[13];
			boolean mark_negative;


			for (i=0; i<18; i++) /* XLS_PAYMENT 03/08/2001 irfan */
			{
				memcpy(result_table[i], ReadAtsStruct.payment_amount[i], length_amount_surcharge);
			}

			if (amount_add)
			{
				for (i=0; i<ATS_Count ; i++)
				{
					/* 04_10 irfan 28/02/2003*/
					k = 6+(i*13);
					memcpy(incoming_amount, IncominglbStr +k, length_amount_surcharge);
					memcpy(original_amount, ReadlbStr     +k, length_amount_surcharge);
					
					/* 04_10 toplamlari duzeltme de kapatildi */
					/*memcpy(incoming_amount, fpTransStruct->payment_amount[i], length_amount_surcharge);
					memcpy(original_amount, ReadAtsStruct.payment_amount[i], length_amount_surcharge);*/

					original_amount[length_amount_surcharge] = 0;
					incoming_amount[length_amount_surcharge] = 0;
					mark_negative = FALSE;

					if (original_amount[0] == '-')
					{
						memcpy(negative_original, original_amount+1, (length_amount_surcharge-1));
						negative_original[length_amount_surcharge-1] = 0;

						switch (Utils_Compare_Strings(negative_original, incoming_amount))
						{
						case 1 :
							Utils_Subtract_Strings(negative_original, incoming_amount, result_amount);
							mark_negative = TRUE;
							break;
						case 0 :
							result_amount[0] = 0;
							break;
						case 2 :
							Utils_Subtract_Strings(incoming_amount, negative_original, result_amount);
						}

					}
					else
					{
						Utils_Add_Strings(original_amount, incoming_amount, result_amount);
					}

					Utils_LeftPad(result_amount, '0', length_amount_surcharge);
					if (mark_negative)
						result_amount[0] = '-';

					memcpy(result_table[i], result_amount, length_amount_surcharge);
				}
			}
			else
			{
				for (i=0; i<ATS_Count ; i++)
				{
					/* 04_10 irfan 28/02/2003*/
					k = 6+(i*13);
					memcpy(incoming_amount, IncominglbStr +k, length_amount_surcharge);
					memcpy(original_amount, ReadlbStr     +k, length_amount_surcharge);
				
					/*memcpy(incoming_amount, fpTransStruct->payment_amount[i], length_amount_surcharge);
					memcpy(original_amount, ReadAtsStruct.payment_amount[i], length_amount_surcharge);*/
					
					original_amount[length_amount_surcharge] = 0;
					incoming_amount[length_amount_surcharge] = 0;
					mark_negative = FALSE;

					if (original_amount[0] == '-')
					{
						memcpy(negative_original, original_amount+1, (length_amount_surcharge-1));
						negative_original[length_amount_surcharge-1] = 0;

						Utils_Add_Strings(negative_original, incoming_amount, result_amount);
						mark_negative = TRUE;
					}
					else
					{
						switch (Utils_Compare_Strings(original_amount, incoming_amount))
						{
						case 1 :
							Utils_Subtract_Strings(original_amount, incoming_amount, result_amount);
							break;
						case 0 :
							result_amount[0] = 0;
							break;
						case 2 :
							Utils_Subtract_Strings(incoming_amount, original_amount, result_amount);
							mark_negative = TRUE;
						}
					}

					Utils_LeftPad(result_amount, '0', length_amount_surcharge);
					if (mark_negative)
						result_amount[0] = '-';

					memcpy(result_table[i], result_amount, length_amount_surcharge);
				}
			}

			/*for (i=0; i<12; i++)*/ /* 04_10 irfan 28/02/2003 */
			for (i=0; i<ATS_Count; i++)
			{
				memcpy(ReadAtsStruct.payment_amount[i], result_table[i], length_amount_surcharge);
			}

			if (RamDisk_Overwrite(Handle1, (byte *)&ReadAtsStruct) == RD_OK)
				lwStatus = 1;
		}
	}
	else if (lwStatus == RD_OK)
	{
		if (cancel_transaction)
			lwStatus = RD_NOT_FOUND;
		else
		{
			if (!amount_add)
			{
				for (i=0; i<ATS_Count; i++)
					fpTransStruct->payment_amount[i][0] = '-';
			}
	
			Handle1= RamDisk_Reopen((byte *)ATS_File)->Handle;  
			lwStatus = RamDisk_Append(Handle1,fpTransStruct);
		}
	}

	return (lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_AtsTransWrite
 *
 * DESCRIPTION  : Adds/updates information to/in the ATS database. 
 *
 * RETURN       : STAT_OK or STAT_NOK
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_AtsTransWrite (void)
{
	AmexTaksitStruct fpTransStruct;
	sint i, status;
	boolean trans_addition;

	memset(&fpTransStruct,'0',sizeof(fpTransStruct));

	/* verde "d_Transaction_Receive.ATS_Date[0]" yerine "d_Transaction_Receive.ATS_Date" yapýldý.*/
	/*memcpy(fpTransStruct.first_date, d_Transaction_Receive.ATS_Date[0], length_due_date);*/
	memcpy(fpTransStruct.first_date, d_Transaction_Receive.ATS_Date, length_due_date);
/* Toygar EMVed */
/*
	memcpy(fpTransStruct.payment_amount[0], d_Transaction_Receive.ATS_Amount_1, length_amount_surcharge);
	for (i=1; i<ATS_Count; i++)
		memcpy(fpTransStruct.payment_amount[i], d_Transaction_Receive.ATS_Amount_2, length_amount_surcharge);
*/
	memcpy(fpTransStruct.payment_amount[0], d_Transaction_Receive.ATS_Amount, length_amount_surcharge);
	for (i=1; i<ATS_Count; i++)
		memcpy(fpTransStruct.payment_amount[i], d_Transaction_Receive.ATS_Amount, length_amount_surcharge);
/* Toygar EMVed */

	if (tran_type == RETURN)
		trans_addition = FALSE;
	else
		trans_addition = TRUE;

	status = Files_WriteTaksitli(&fpTransStruct, trans_addition, FALSE);

	if (status > 0)
	{
		return(STAT_OK);
	}
	else
	{
		switch(status)
		{
			case  RD_OUT_OF_MEM:    
				Mainloop_DisplayWait("   YI¦IN DOLU   GšNSONU YAPINIZ ",1);
				return(STAT_NOK);

			default:
				Mainloop_DisplayWait("ATS YAZILAMADI",1);
				return(STAT_NOK);
		}
	}

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_VoidAtsTransaction
 *
 * DESCRIPTION  : Updates information in the ATS database for a canceled transaction
 *
 * RETURN       : STAT_OK or STAT_NOK
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_VoidAtsTransaction(TransactionStruct *fpTransStruct)
{
	sint void_tran_type, i, status;
	AmexTaksitStruct fpAtsStruct;
	boolean trans_addition;

	memset(&fpAtsStruct,'0',sizeof(fpAtsStruct));
	memcpy(fpAtsStruct.first_date, fpTransStruct->first_install_date, length_due_date);
	memcpy(fpAtsStruct.payment_amount[0], fpTransStruct->install_amount_1, length_amount_surcharge);
	for (i=1; i<ATS_Count; i++)
		memcpy(fpAtsStruct.payment_amount[i], fpTransStruct->install_amount_2, length_amount_surcharge);
	void_tran_type = fpTransStruct->tran_type - '0';
	if (void_tran_type == RETURN) trans_addition = TRUE;
	else trans_addition = FALSE;
	status = Files_WriteTaksitli(&fpAtsStruct, trans_addition, TRUE);
	if (status > 0) return(STAT_OK);
	else
	switch(status)
	{
	case  RD_OUT_OF_MEM:    
		Mainloop_DisplayWait("   YI¦IN DOLU   GšNSONU YAPINIZ ",1);
		return(STAT_NOK);
	case  RD_NOT_FOUND:
		Mainloop_DisplayWait("ATS KAYIT YOK",1);
		return(STAT_NOK);
	default:
		Mainloop_DisplayWait("ATS ˜PTAL HATALI",1);
		return(STAT_NOK);
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_InitParameters
 *
 * DESCRIPTION: Inits the parameter files
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_InitParameters(void)
{
	if	( (Files_InitFile((char *)GeneralParams_File, Length_GenPar) == STAT_OK) &&  
	     (Files_InitFile((char *)TransParams_File, Length_TranPar)  == STAT_OK) &&
	     (Files_InitFile((char *)TerminalAppList_File, LG_EMV_APP_STRUCT) == STAT_OK)
	   ) return(STAT_OK);
	return(STAT_NOK);
}

		

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_ReadGenParams 
 *
 * DESCRIPTION	: Reads general parameters.
 *
 * RETURN	: lwStatus ( STAT_OK, STAT_NOK)     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_ReadGenParams(void)
{
	sint lbStatus;
	byte lwStatus;
	sint Handle1;
	GenParStructure	fpStruct;

	lwStatus = STAT_NOK;

	if (RamDisk_ChkDsk((byte *)GeneralParams_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)GeneralParams_File)->Handle;

		if (RamDisk_Seek(Handle1, 0) >= 0)
		{
			lbStatus = RamDisk_Read(Handle1, &fpStruct);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
				AutoEOD 	  = fpStruct.AutoEOD;
				LAST_4_DIGIT_FLAG = fpStruct.LAST_4_DIGIT_FLAG;
				INSTALLATION 	  = fpStruct.INSTALLATION;
				EOD_flag 	  = fpStruct.EOD_flag;
				ATS_FLAG 	  = fpStruct.ATS_FLAG;
				EOD_pointer 	  = fpStruct.EOD_pointer;
				VERDE_TRANSACTION = fpStruct.VERDE_TRANSACTION;	/* verde 04/11/2002 */
				EXTERNAL_PINPAD = fpStruct.EXTERNAL_PINPAD;	/* verde 07/11/2002 */
				Line_Type 	  = fpStruct.Line_Type;
				device_serial_no  = fpStruct.device_serial_no;
				master_key_no 	  = fpStruct.master_key_no;
				POS_software_ver  = fpStruct.POS_software_ver;

				memcpy(UserPassword, fpStruct.UserPassword, 5);
				memcpy(OperatorPassword, fpStruct.OperatorPassword, 15);
				memcpy(merch_AMEX_no, fpStruct.merch_AMEX_no, 11);
				memcpy(merch_no, fpStruct.merch_no, 13);
				memcpy(merch_addr, fpStruct.merch_addr, 73);
				memcpy(merch_device_no, fpStruct.merch_device_no, 9);
		/*
				Printer_WriteStr("merch_device_no:\n");
				PrntUtil_BufAsciiHex((byte *)merch_device_no,9);
		*/
				memcpy(auto_EOD_time, fpStruct.auto_EOD_time, 5);
				memcpy(tel_no_1, fpStruct.tel_no_1, 16);
				memcpy(tel_no_2, fpStruct.tel_no_2, 16);
				memcpy(working_key, fpStruct.working_key, 16);
				memcpy(batch_no_c, fpStruct.batch_no_c, 4);
				memcpy(param_ver_no, fpStruct.param_ver_no, 4);
				memcpy( YTL_Slip, fpStruct.YTL_Slip, 3 );	/*bm YTL */

/* Toygar Debug
		Printer_WriteStr("\nParamVerNo(R):\n");
		PrntUtil_BufAsciiHex((byte *)param_ver_no, 4);
*/
				memcpy(last_eod_date, fpStruct.last_eod_date, 9);
				memcpy(&lsTerminalDataStruct, &fpStruct.lsTerminalDataStruct, sizeof(lsTerminalDataStruct));

				/*UPDATE 01_17 02/11/99 */
				auto_EOD_time_int = atoi(auto_EOD_time);
				GEMV_Key_Version_No = fpStruct.EMV_Key_Version_No;
				GConfig_Version_No = fpStruct.Config_Version_No;
				ReversalPending = fpStruct.ReversalPending;
				memcpy(last_genius_eod_date,fpStruct.last_genius_eod_date,9);	

				POS_Type = fpStruct.POS_Type ;	
				genius_flag = fpStruct.genius_flag;
				
				memcpy(GPOS_Type,fpStruct.GPOS_Type , 3);
				memcpy(GPOS_Model ,fpStruct.GPOS_Model,4 );
				GEMV_Key_Version_No = fpStruct.GEMV_Key_Version_No;	
				GConfig_Version_No = fpStruct.GConfig_Version_No;	
				memcpy(GPOS_Serial_No ,fpStruct.GPOS_Serial_No,9 );
				memcpy(GPOS_Akbank_No_My2 ,fpStruct.GPOS_Akbank_No,7 );
				memcpy(GPinpad_Serial_No ,fpStruct.GPinpad_Serial_No,9 );
				memcpy(GPinpad_Akbank_No,fpStruct.GPinpad_Akbank_No, 7 );
				GActive_Application_Code =fpStruct.GActive_Application_Code;	
				GConnection_Type = fpStruct.GConnection_Type;	

				BUYUKFIRMA = fpStruct.BUYUKFIRMA;	

				KAMPANYA_FLAG = fpStruct.KAMPANYA_FLAG;
				ERROR_RECEIPT = fpStruct.ERROR_RECEIPT;
				memcpy(APN,fpStruct.APN,12);
				memcpy(glb_host_ip,fpStruct.glb_host_ip,17);
				glb_host_port = fpStruct.glb_host_port;
				memcpy(GPRS_User,fpStruct.GPRS_User,10);
				memcpy(GPRS_Passwd,fpStruct.GPRS_Passwd,10);
/*		 		GPRS_CheckMode= fpStruct.GPRS_CheckMode;
				GPRS_TryTime = fpStruct.GPRS_TryTime;
*/  		 		autoGSMRetry = fpStruct.autoGSMRetry;
  				GPRS_USEGPRS = fpStruct.GPRS_USEGPRS;


				lwStatus = STAT_OK;
			}
		}
	}
	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_WriteGenParams 
 *
 * DESCRIPTION	: Writes general parameters.
 *
 * RETURN	: lwStatus ( STAT_OK, STAT_NOK)     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_WriteGenParams(void)
{
	byte lwStatus;
	sint Handle1;
	GenParStructure	fpStruct;

	lwStatus = STAT_NOK;

	if (RamDisk_ChkDsk((byte *)GeneralParams_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)GeneralParams_File)->Handle;

		fpStruct.AutoEOD 	   = AutoEOD;
		fpStruct.LAST_4_DIGIT_FLAG = LAST_4_DIGIT_FLAG;
		fpStruct.INSTALLATION 	   = INSTALLATION;
		fpStruct.EOD_flag 	   = EOD_flag;
		fpStruct.ATS_FLAG 	   = ATS_FLAG;
		fpStruct.VERDE_TRANSACTION	= VERDE_TRANSACTION;	/* verde 04/11/2002 */
		fpStruct.EXTERNAL_PINPAD	= EXTERNAL_PINPAD;		/* verde 07/11/2002 */
		fpStruct.EOD_pointer 	   = EOD_pointer;
		fpStruct.Line_Type 	   = Line_Type;
		fpStruct.device_serial_no  = device_serial_no;
		fpStruct.master_key_no 	   = master_key_no;
		fpStruct.POS_software_ver  = POS_software_ver;

		memcpy(fpStruct.UserPassword, UserPassword, 5);
		memcpy(fpStruct.OperatorPassword, OperatorPassword, 15);
		memcpy(fpStruct.merch_AMEX_no, merch_AMEX_no, 11);
		memcpy(fpStruct.merch_no, merch_no, 13);
		memcpy(fpStruct.merch_addr, merch_addr, 73);
		memcpy(fpStruct.merch_device_no, merch_device_no, 9);
		memcpy( fpStruct.YTL_Slip, YTL_Slip, 3 );	/*bm YTL 25/06/2004 */

		/* UPDATE 01_17 02/11/99 */		
		Utils_IToA(auto_EOD_time_int, auto_EOD_time);

		memcpy(fpStruct.auto_EOD_time, auto_EOD_time, 5);
		memcpy(fpStruct.tel_no_1, tel_no_1, 16);
		memcpy(fpStruct.tel_no_2, tel_no_2, 16);
		memcpy(fpStruct.working_key, working_key, 16);
		memcpy(fpStruct.batch_no_c, batch_no_c, 4);
/* Toygar Debug
		Printer_WriteStr("\nParamVerNo(W):\n");
		PrntUtil_BufAsciiHex((byte *)param_ver_no, 4);
*/
		memcpy(fpStruct.param_ver_no, param_ver_no, 4);
		memcpy(fpStruct.last_eod_date, last_eod_date, 9);
		memcpy(&fpStruct.lsTerminalDataStruct, &lsTerminalDataStruct, sizeof(lsTerminalDataStruct));
		fpStruct.EMV_Key_Version_No = GEMV_Key_Version_No;
		fpStruct.Config_Version_No = GConfig_Version_No;		
		fpStruct.ReversalPending = ReversalPending;
		memcpy(fpStruct.last_genius_eod_date,last_genius_eod_date,9);	
		fpStruct.POS_Type = POS_Type;	
		fpStruct.genius_flag = genius_flag;
		
		memcpy(fpStruct.GPOS_Type,GPOS_Type , 3);
		memcpy(fpStruct.GPOS_Model,GPOS_Model ,4 );
		fpStruct.GEMV_Key_Version_No = GEMV_Key_Version_No;	
		fpStruct.GConfig_Version_No = GConfig_Version_No;	
		memcpy(fpStruct.GPOS_Serial_No,GPOS_Serial_No ,9 );
		memcpy(fpStruct.GPOS_Akbank_No,GPOS_Akbank_No_My2 ,7 );
		memcpy(fpStruct.GPinpad_Serial_No,GPinpad_Serial_No ,9 );
		memcpy(fpStruct.GPinpad_Akbank_No, GPinpad_Akbank_No,7 );
		fpStruct.GActive_Application_Code = GActive_Application_Code;	
		fpStruct.GConnection_Type = GConnection_Type;	

		memcpy(fpStruct.APN,APN,12);
		memcpy(fpStruct.glb_host_ip,glb_host_ip,17);
		fpStruct.glb_host_port = glb_host_port;

		memcpy(fpStruct.GPRS_User,GPRS_User,10);
		memcpy(fpStruct.GPRS_Passwd,GPRS_Passwd,10);
/*	 	fpStruct.GPRS_CheckMode = GPRS_CheckMode;
		fpStruct.GPRS_TryTime = GPRS_TryTime;
*/  		fpStruct.autoGSMRetry = autoGSMRetry;
  		fpStruct.GPRS_USEGPRS = GPRS_USEGPRS;
  		


		fpStruct.BUYUKFIRMA = BUYUKFIRMA;	
		fpStruct.KAMPANYA_FLAG = KAMPANYA_FLAG;
		fpStruct.ERROR_RECEIPT = ERROR_RECEIPT;

		if (RamDisk_Seek(Handle1, 0) >= 0)
		{
			if (RamDisk_Overwrite(Handle1,(byte *)&fpStruct) == RD_OK) 
			{
/*
				PrntUtil_BufAsciiHex((byte *)&fpStruct, sizeof(GenParStructure));
				Printer_WriteStr("ParamWrite RamDisk_Overwrite Ok\n");
*/
				lwStatus = STAT_OK;
		}
/*			else Printer_WriteStr("ParamWrite RamDisk_Overwrite NOk\n");*/
		}
		else
		{
			if (RamDisk_Append(Handle1, &fpStruct) > 0)
			{
/*				
				PrntUtil_BufAsciiHex((byte *)&fpStruct, sizeof(GenParStructure));
				Printer_WriteStr("ParamWrite RamDisk_Append Ok\n");
*/
				lwStatus = STAT_OK;
		}
/*			else Printer_WriteStr("ParamWrite RamDisk_Append NOk\n");*/
		}
	}

	return(lwStatus);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_ReadTransParams 
 *
 * DESCRIPTION	: Reads transaction specific parameters.
 *
 * RETURN	: lwStatus ( STAT_OK, STAT_NOK)     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_ReadTransParams(void)
{
	sint lbStatus;
	byte lwStatus;
	sint Handle1;
	TranParStructure fpStruct;

	lwStatus = STAT_NOK;

	if (RamDisk_ChkDsk((byte *)TransParams_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)TransParams_File)->Handle;
	
		if (RamDisk_Seek(Handle1, 0) >= 0)
		{
			lbStatus = RamDisk_Read(Handle1, &fpStruct);
			
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
				last_transact_status = fpStruct.last_transact_status;

				memcpy(seq_no_c, fpStruct.seq_no_c, 5);
				memcpy(last_transact_batch_no, fpStruct.last_transact_batch_no, 4);
				memcpy(last_transact_seq_no, fpStruct.last_transact_seq_no, 5);
				memcpy(net_total, fpStruct.net_total, 14);
				memcpy(agreement_total, fpStruct.agreement_total, 14);

				seq_no = atoi(seq_no_c);

				lwStatus = STAT_OK;
			}
		}

	}

	return(lwStatus);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_WriteTransParams 
 *
 * DESCRIPTION	: Writes transaction specific parameters.
 *
 * RETURN	: lwStatus ( STAT_OK, STAT_NOK)     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_WriteTransParams(void)
{
	byte lwStatus;
	sint Handle1;
	TranParStructure fpStruct;

	lwStatus = STAT_NOK;

	if (RamDisk_ChkDsk((byte *)TransParams_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)TransParams_File)->Handle;

		fpStruct.last_transact_status = last_transact_status;

		Utils_IToA(seq_no, seq_no_c);
		Utils_LeftPad(seq_no_c, '0', 4);

		memcpy(fpStruct.seq_no_c, seq_no_c, 5);
		memcpy(fpStruct.last_transact_batch_no, last_transact_batch_no, 4);
		memcpy(fpStruct.last_transact_seq_no, last_transact_seq_no, 5);
		memcpy(fpStruct.net_total, net_total, 14);
		memcpy(fpStruct.agreement_total, agreement_total, 14);

		if (RamDisk_Seek(Handle1, 0) >= 0)
		{
			if (RamDisk_Overwrite(Handle1,(byte *)&fpStruct) == RD_OK) 
				lwStatus = STAT_OK;
		}
		else
		{
			if (RamDisk_Append(Handle1, &fpStruct) >= 0)
				lwStatus = STAT_OK;
		}

	}

	return(lwStatus);
}




/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_ReadStatistics
 *
 * DESCRIPTION	: Reads statistics.
 *
 * RETURN	: lwStatus ( STAT_OK, STAT_NOK)     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_ReadStatistics(void)
{
	sint lbStatus;
	byte lwStatus;
	sint Handle1;
	StatisticsStructure fpStruct;

	lwStatus = STAT_NOK;

	if (RamDisk_ChkDsk((byte *)Statistics_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)Statistics_File)->Handle;

		if (RamDisk_Seek(Handle1, 0) >= 0)
		{
			lbStatus = RamDisk_Read(Handle1, &fpStruct);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
				total_batch_time = fpStruct.total_batch_time;
								
				memcpy(&d_Statistics1, &fpStruct.d_Statistics1, sizeof(d_Statistics1));
				memcpy(&d_Statistics2, &fpStruct.d_Statistics2, sizeof(d_Statistics2));
				memcpy(d_Totals_Counts, fpStruct.d_Totals_Counts, sizeof(d_Totals_Counts));

				lwStatus = STAT_OK;
			}
		}
	}

	return(lwStatus);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_WriteStatistics 
 *
 * DESCRIPTION	: Writes statistics.
 *
 * RETURN	: lwStatus ( STAT_OK, STAT_NOK)     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_WriteStatistics(void)
{
	byte lwStatus;
	sint Handle1;
	StatisticsStructure fpStruct;

	lwStatus = STAT_NOK;

	if (RamDisk_ChkDsk((byte *)Statistics_File)==RD_CHECK_OK)
	{
		Handle1= RamDisk_Reopen((byte *)Statistics_File)->Handle;

		fpStruct.total_batch_time = total_batch_time;

		memcpy(&fpStruct.d_Statistics1, &d_Statistics1, sizeof(d_Statistics1));
		memcpy(&fpStruct.d_Statistics2, &d_Statistics2, sizeof(d_Statistics2));
		memcpy(fpStruct.d_Totals_Counts, d_Totals_Counts, sizeof(d_Totals_Counts));

		if (RamDisk_Seek(Handle1, 0) >= 0)
		{
			if (RamDisk_Overwrite(Handle1,(byte *)&fpStruct) == RD_OK) 
				lwStatus = STAT_OK;
		}
		else
		{
			if (RamDisk_Append(Handle1, &fpStruct) >= 0)
				lwStatus = STAT_OK;
		}
	}

	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_WriteAllParams
 *
 * DESCRIPTION	: Writes All the Global Parameters.
 *
 * RETURN	: 
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_WriteAllParams(void)
{

	if(Files_WriteGenParams() != STAT_OK)
	{
		Slips_PrinterStr("\n\nGENEL PARAMETRE YAZILAMADI\n\n");
		Mainloop_DisplayWait("GENEL PARAMETRE    YAZILAMADI   ",1);
		return(STAT_NOK);
	}

	if(Files_WriteTransParams() != STAT_OK)
	{
		Slips_PrinterStr("\n\nTRANS PARAMETRE YAZILAMADI\n\n");
		Mainloop_DisplayWait("TRANS PARAMETRE    YAZILAMADI   ",1);
		return(STAT_NOK);
	}

	/*if(Files_WritePrefix() != STAT_OK)
	{
		Mainloop_DisplayWait("PREFIX PARAMETRE   YAZILAMADI   ",1);
	}

	if(Files_WriteStatistics() != STAT_OK)
	{
		Mainloop_DisplayWait("ISTATISTIK PRM.     YAZILAMADI  ",1);
	}*/
	return(STAT_OK);

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_ReadAllParams
 *
 * DESCRIPTION	: Reads All the Global Parameters.
 *
 * RETURN	: 
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_ReadAllParams(void)
{
	if(Files_ReadGenParams() != STAT_OK)
	{
		Slips_PrinterStr("\n\nGENEL PARAMETRE OKUNAMADI\n\n");
		Mainloop_DisplayWait("GENEL PARAMETRE     OKUNAMADI   ",1);
		return (STAT_NOK);
	}

	if(Files_ReadTransParams() != STAT_OK)
	{
		Slips_PrinterStr("\n\nGENEL PARAMETRE OKUNAMADI\n\n");
		Mainloop_DisplayWait("TRANS PARAMETRE     OKUNAMADI   ",1);
		return (STAT_NOK);
	}

	/*	if(Files_ReadPrefix() != STAT_OK)
	{
		Mainloop_DisplayWait("PREFIX PARAMETRE    OKUNAMADI   ",1);
	}

	if(Files_ReadStatistics() != STAT_OK)
	{
		Mainloop_DisplayWait("ISTATISTIK PRM.      OKUNAMADI  ",1);
	}*/
	return (STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GetTransactionCounts
 *
 * DESCRIPTION	: 
 *
 * RETURN	: 
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Files_GetTransactionCounts(int *TransOnlineNo, int *TransOfflineNo)
{
	sint Handle1;

	if (RamDisk_ChkDsk((byte *)EMVOffline_File)==RD_CHECK_OK)
	{
	Handle1 = RamDisk_Reopen((byte *)EMVOffline_File)->Handle;
		if(Handle1 != RD_ERROR  && Handle1 != RD_FILENOTFOUND)
		{
	*TransOfflineNo = RamDisk_Seek(Handle1, SEEK_LAST);
	if (*TransOfflineNo < 0) *TransOfflineNo = 0;
		}
		else *TransOfflineNo = 0;
	}
	else *TransOfflineNo = 0;
	if (RamDisk_ChkDsk((byte *)EMVOnline_File)==RD_CHECK_OK)
	{
	Handle1 = RamDisk_Reopen((byte *)EMVOnline_File)->Handle;
		if(Handle1 != RD_ERROR  && Handle1 != RD_FILENOTFOUND)
		{
	*TransOnlineNo = RamDisk_Seek(Handle1, SEEK_LAST);
	if (*TransOnlineNo < 0) *TransOnlineNo = 0;	
		}
		else *TransOnlineNo = 0;		
	}
	else *TransOnlineNo = 0;	
}


/* LastTrans File *****************************************************************/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_LastTransWrite
 *
 * DESCRIPTION: 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_LastTransWrite(PTStruct *ptPTStruct)
{
	sint Handle1;

	if (Files_InitFile((char *)LastTrans_File,sizeof(ptPTStruct)) != STAT_OK)
	{
		Printer_WriteStr("\n\nFiles_LastTransWrite-Files_InitFile-Failed\n\n");
		return(STAT_NOK);
	}

/*	if (RamDisk_ChkDsk((byte *)LastTrans_File) == RD_CHECK_OK) Printer_WriteStr("\n\nFiles_LastTransWrite-RD_CHECK_OK\n\n");
	else if (RamDisk_ChkDsk((byte *)LastTrans_File) == RD_CHECK_FAIL) Printer_WriteStr("\n\nFiles_LastTransWrite-RD_CHECK_FAIL\n\n");
	else if (RamDisk_ChkDsk((byte *)LastTrans_File) == RD_ERROR) Printer_WriteStr("\n\nFiles_LastTransWrite-RD_ERROR\n\n");
	else if (RamDisk_ChkDsk((byte *)LastTrans_File) == RD_FILENOTFOUND) Printer_WriteStr("\n\nFiles_LastTransWrite-RD_FILENOTFOUND\n\n");
*/
	Handle1 = RamDisk_Reopen((byte *)LastTrans_File)->Handle;
	if (RamDisk_Append(Handle1,(byte *)ptPTStruct)>0) return(STAT_OK);
	else Printer_WriteStr("\n\nFiles_LastTransWrite-RamDisk_Append Error\n\n");
	
	return (STAT_NOK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_LastTransRead
 *
 * DESCRIPTION: 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_LastTransRead(PTStruct *ptPTStruct)
{
	sint Handle1;
	sint ReadRet;
	sint lastSeq;
	PTStruct tmpPTStruct;

	if (RamDisk_ChkDsk((byte *)LastTrans_File)==RD_CHECK_OK)
	{
		Handle1 = RamDisk_Reopen((byte *)LastTrans_File)->Handle;
		lastSeq = RamDisk_Seek(Handle1, SEEK_LAST );
	        if (lastSeq < 0) return STAT_NOK;
		ReadRet = RamDisk_Seek(Handle1,  lastSeq - 1);
		if (ReadRet < 0) return STAT_NOK;
		ReadRet = RamDisk_Read( Handle1, &tmpPTStruct );
		if ( ( ReadRet >= 0 ) || ( ReadRet == RD_LAST ) )
		{		    
			if (!memcmp(ptPTStruct->Pan, tmpPTStruct.Pan, ptPTStruct->PanLength))
			{
				memcpy(ptPTStruct->Amount, tmpPTStruct.Amount,sizeof(tmpPTStruct.Amount) );
				return ( STAT_OK );
			}
		}
	}
	return(STAT_NOK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_EMVAppInit
 *
 * DESCRIPTION: 
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_EMVAppInit(void)
{
	return (Files_InitFile((char *) TerminalAppList_File, LG_EMV_APP_STRUCT));
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Files_ReadEMVAppParam
 * DESCRIPTION:   Fills EMV application in the structure from the file
 * RETURN:        TRUE      Success
 *                FALSE     Error occured
 * NOTES:         
 * ------------------------------------------------------------------------ */
boolean Files_ReadEMVAppParam(TerminalAppList *ptaList)
{
	ApplicationParams apTemp;
	sint        siHandle, siResult;
	sint        siLastSeq, i;

	if (RamDisk_ChkDsk((byte *)TerminalAppList_File) != RD_CHECK_OK) return FALSE;
	memset(ptaList, 0, sizeof(ptaList));
	siHandle  = RamDisk_Reopen((byte *)TerminalAppList_File)->Handle;
	RamDisk_Seek(siHandle, SEEK_FIRST);
	for(i = 0; ; i++)
	{
		siResult = RamDisk_Read(siHandle, &ptaList->TermApp[i]);
		if ((siResult >= 0) || (siResult == RD_LAST)) ptaList->bTermAppCount++;
		else break;
	}
	return (ptaList->bTermAppCount > 0) ;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Files_SaveEMVAppParam
 * DESCRIPTION:   Stores given EMV application in the structure
 * RETURN:        TRUE      Success
 *                FALSE     Error occured
 * NOTES:         
 * ------------------------------------------------------------------------ */
boolean Files_SaveEMVAppParamIn(ApplicationParams *paApp)
	{
	ApplicationParams apTemp;
	sint        siHandle, siResult;
	sint        siLastSeq, i;

	if (RamDisk_ChkDsk((byte *)TerminalAppList_File) != RD_CHECK_OK) 
		if (Files_EMVAppInit() != STAT_OK) return FALSE;
	siHandle  = RamDisk_Reopen((byte *)TerminalAppList_File)->Handle;
	siLastSeq = RamDisk_Seek(siHandle, SEEK_LAST);
	if ((siLastSeq = RamDisk_Seek(siHandle, SEEK_LAST)) >= 0) 
		for (i = 0; i < siLastSeq; i++)
		{
			siResult = RamDisk_Read(siHandle, &apTemp);
			if ((siResult >= 0) || (siResult == RD_LAST))
				if (!memcmp(apTemp.AID, paApp->AID, paApp->AID_Length) && (paApp->AID_Length == apTemp.AID_Length))
			{
					Printer_WriteStr("Replacing AppParam...\n");
					RamDisk_Delete(siHandle);
			}
		}
	return (RamDisk_Append(siHandle,paApp) > 0);
	}
  
/* --------------------------------------------------------------------------
 * FUNCTION NAME: Files_SaveEMVAppParam
 * DESCRIPTION:   Stores given EMV application in the structure
 * RETURN:        TRUE      Success
 *                FALSE     Error occured
 * NOTES:         
 * ------------------------------------------------------------------------ */
boolean Files_SaveEMVAppParam(TerminalAppList *ptaList)
{
	sint        siHandle, siResult;
	sint        i;

	if (Files_EMVAppInit() != STAT_OK) return FALSE;
	siHandle  = RamDisk_Reopen((byte *)TerminalAppList_File)->Handle;
	for(i=0;i<ptaList->bTermAppCount;i++)
		if (!Files_SaveEMVAppParamIn(&ptaList->TermApp[i])) return(FALSE);
	return(TRUE);
}  


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_CAPublicKeyInit
 *
 * DESCRIPTION: 
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES			:       
 *
 * ------------------------------------------------------------------------ */
byte Files_CAPublicKeyInit(void)
{
	return (Files_InitFile((char *) PublicKey_File, LG_CA_PK_STRUCT));
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Files_SearchCAPublicKey
 * DESCRIPTION:   Search the public key specified by the parameters.
 * RETURN:        TRUE      Public Key found
 *                FALSE     Public Key doesn't exit
 * NOTES:       
 * ------------------------------------------------------------------------ */
boolean Files_SearchCAPublicKey(CAPublicKey *ppkKey, RowData *prdRID, byte bPKI)
	{
	CAPublicKey caTemp;
	sint        siHandle, siResult;
	sint        siLastSeq, i;

	if (RamDisk_ChkDsk((byte *)PublicKey_File) != RD_CHECK_OK) return FALSE;
	siHandle  = RamDisk_Reopen((byte *)PublicKey_File)->Handle;
	siLastSeq = RamDisk_Seek(siHandle, SEEK_LAST);
	if (RamDisk_Seek(siHandle, SEEK_FIRST) < 0) return FALSE;
	for (i = 0; i < siLastSeq; i++)
	{
		siResult = RamDisk_Read(siHandle, &caTemp);
		if ((siResult >= 0) || (siResult == RD_LAST))
			if (!memcmp(caTemp.RID, prdRID->value, prdRID->length) && (bPKI == caTemp.CA_PKIndex))
		{
				memcpy(ppkKey, &caTemp, LG_CA_PK_STRUCT);
				return TRUE;
			}
	}
	return FALSE;
		}



/* --------------------------------------------------------------------------
 * FUNCTION NAME: Files_SaveCAPublicKey
 * DESCRIPTION:   Stores given public key in the PK database.
 * RETURN:        TRUE      Public Key found
 *                FALSE     Public Key doesn't exit
 * NOTES:         Overwrites if the key already exists
 * ------------------------------------------------------------------------ */
boolean Files_SaveCAPublicKey(CAPublicKey *ppkKey)
{
	CAPublicKey caTemp;
	sint        siHandle, siResult;
	sint        siLastSeq, i;

	if (RamDisk_ChkDsk((byte *)PublicKey_File) != RD_CHECK_OK) 
		if (Files_CAPublicKeyInit() != STAT_OK) return FALSE;

	siHandle  = RamDisk_Reopen((byte *)PublicKey_File)->Handle;
	siLastSeq = RamDisk_Seek(siHandle, SEEK_LAST);
	if ((siLastSeq = RamDisk_Seek(siHandle, SEEK_LAST)) >= 0) 
	for (i = 0; i < siLastSeq; i++)
	{
		siResult = RamDisk_Read(siHandle, &caTemp);
		if ((siResult >= 0) || (siResult == RD_LAST))
			if (!memcmp(caTemp.RID, ppkKey->RID, sizeof(ppkKey->RID)) && (ppkKey->CA_PKIndex == caTemp.CA_PKIndex))
        		{
        			RamDisk_Delete(siHandle);
				DEBUG(1,
		{
					Printer_WriteStr("Duplicate key erased\n");
				})
			}
		}
	return (RamDisk_Append(siHandle,ppkKey) > 0);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_InitTransaction
 *
 * DESCRIPTION: Inits the transaction file "AKBANK"
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_InitTransaction(void)
{
	if ((Files_InitFile((char *)Transaction_File, Length_Transaction) == STAT_OK) &&  
		(Files_InitFile((char *)ATS_File, Length_AmexTaksit) == STAT_OK))
		return(STAT_OK);
	return (STAT_NOK); 
	}
/********************************* genius   ********************************/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Files_GRecordWrite
 *
 * DESCRIPTION          : Write a genius record to genius batch file
 *
 * RETURN                       : STAT_OK, STAT_NOK or STAT_INV    
 *
 * NOTES                        : r.i.o. 09/10/2002       
 *
 * ------------------------------------------------------------------------ */
byte Files_GRecordWrite( void)
{
	GTransactionStruct fpGTransStruct;
	sint Status;
	Tarih_Vade Gtarih_vade[12];
	char lbtemp[500];

	/* fill the record */
	memset(&fpGTransStruct,' ',sizeof(fpGTransStruct));

	memcpy(fpGTransStruct.seq_no, d_GTransaction_Receive.transaction_seq_no, 4);
	memcpy(fpGTransStruct.card_no, card_no, strlen(card_no));
	memcpy(fpGTransStruct.garanti_asim_tutar, d_GTransaction_Receive.garanti_asim_tutari, 13);
	memcpy(fpGTransStruct.garanti_tutar, d_GTransaction_Receive.garanti_tutar, 13);
	memcpy(fpGTransStruct.sorgulama_tutar, d_GTransaction_Receive.sorgulama_tutar, 13);

	memcpy(fpGTransStruct.kullanilan_kredi_limiti, d_GTransaction_Receive.kullanilan_kredi_limiti, 13);
	memcpy(fpGTransStruct.available_balance, d_GTransaction_Receive.available_balance, 13);
	memcpy(fpGTransStruct.bayii_garantisiz_borc, d_GTransaction_Receive.bayii_garantisiz_borc, 13);
	memcpy(fpGTransStruct.kart_islem_toplami, d_GTransaction_Receive.kart_islem_toplami, 13);

	memcpy(fpGTransStruct.garanti_komisyon_orani, d_GTransaction_Receive.garanti_komisyon_orani, 5);
	memcpy(fpGTransStruct.islem_komisyon_orani, d_GTransaction_Receive.islem_komisyon_orani, 5);	
	fpGTransStruct.mevduat_hes_kodu = d_GTransaction_Send.mev_hes_kodu;
	memcpy(fpGTransStruct.bayii_no, d_GTransaction_Receive.bayii_no, 7);
	memcpy(fpGTransStruct.belge_no, d_GTransaction_Send.belge_no, 10);

	memcpy(fpGTransStruct.approval_code, d_GTransaction_Receive.approval_code, 6);

	memcpy(&fpGTransStruct.date[0], &d_GTransaction_Receive.date[4], 2);
	memcpy(&fpGTransStruct.date[3], &d_GTransaction_Receive.date[2], 2);
	memcpy(&fpGTransStruct.date[6], &d_GTransaction_Receive.date[0], 2);
	fpGTransStruct.date[2] = '/';
	fpGTransStruct.date[5] = '/';

	memcpy(fpGTransStruct.time, d_GTransaction_Receive.time, 2);
	memcpy(&fpGTransStruct.time[3], &d_GTransaction_Receive.time[2], 2);
	fpGTransStruct.time[2] = ':';    

	fpGTransStruct.MSG_TYPE = d_GTransaction_Receive.GHeader_Transact.MSG_TYPE;
	fpGTransStruct.void_status = '0';

	memcpy(fpGTransStruct.vade_sayisi, d_GTransaction_Send.vade_sayisi, 2);
	memcpy(fpGTransStruct.DB_Tarih_Vade ,d_GTransaction_Send.Gtarih_vade , sizeof(Gtarih_vade));

	/* 04_04 22/11/2002 */
	memcpy(fpGTransStruct.bayii_adi, d_GTransaction_Receive.bayii_adi, 24);
	memcpy(fpGTransStruct.vade_sabitleme_tarihi, d_GTransaction_Receive.vade_sabitleme_tarihi, 8);

	/* test amacli */
	/*memset(lbtemp, 0, sizeof(lbtemp));
	Slips_PrinterStr("\n\n\nfpGTransStruct:\n");
	memcpy(lbtemp,&fpGTransStruct, sizeof(fpGTransStruct));
	Slips_PrinterStr(lbtemp);
	Slips_PrinterStr("\n\n\n");*/

	/* write the filled record to the GENIUS batch */
	Status = Files_GAddTransaction((GTransactionStruct *)&fpGTransStruct);


	if (Status > 0)
	{
		return(STAT_OK);
	}
	else
	{
		switch(Status)
		{
			  case  RD_OUT_OF_MEM:    Mainloop_DisplayWait("   YI¦IN DOLU   GšNSONU YAPINIZ ",1);
		 							return(STAT_NOK);

			  default:				Mainloop_DisplayWait("KAYIT YAZILAMADI",1);
									return(STAT_NOK);
		}
	}

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GAddTransaction
 *
 * DESCRIPTION: Adds information to the Genius transaction database when EOF is received. 
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_INV )      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
sint Files_GAddTransaction(GTransactionStruct *fpGTransStruct)
{
	sint RamDisk_ret;
	sint handle2;

	if (RamDisk_ChkDsk((byte *)Transaction_GFile)==RD_CHECK_OK)
	{
		handle2 = RamDisk_Reopen((byte *)Transaction_GFile)->Handle;

		if (RamDisk_Seek(handle2, SEEK_LAST) >= 0)	
		{

			RamDisk_ret = RamDisk_Append(handle2,fpGTransStruct);
		}
		
		if(RamDisk_ret > 0)
		{
			Grecord_counter = RamDisk_ret;
		}

		return(RamDisk_ret);
	}

	return (RD_ERROR);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GInitFileGenius
 *
 * DESCRIPTION: Inits the Genius transaction file "GENIUS"
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_GInitFileGenius(void)
{

	if (Files_InitFile((char *)Transaction_GFile, GLength_Transaction) == STAT_OK)
		return(STAT_OK);
	return (STAT_NOK); 
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GReadTransaction
 *
 * DESCRIPTION	: Reads from the genius transaction genius database.
 *
 * RETURN		: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_GReadTransaction(GTransactionStruct *fpGTransStruct)
{
	sint lbStatus, lwLastSeq, i;
	byte lwStatus;
	sint handle3;
	GTransactionStruct GTempTransStruct;

	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)Transaction_GFile)==RD_CHECK_OK)
	{
		handle3 = RamDisk_Reopen((byte *)Transaction_GFile)->Handle;
		lwLastSeq = RamDisk_Seek(handle3,SEEK_LAST);
		RamDisk_Seek(handle3, SEEK_FIRST);
		for (i=0; i<lwLastSeq; i++)
		{
			lbStatus = RamDisk_Read(handle3, &GTempTransStruct);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
				if (memcmp(GTempTransStruct.seq_no, fpGTransStruct->seq_no, length_seq_no) == 0)
				{
					GBatch_record_index = i;
					memcpy(fpGTransStruct, &GTempTransStruct, sizeof(GTransactionStruct));
					lwStatus = STAT_OK;
				}
			}
			else	if (lbStatus == RD_EOF)	lwStatus = STAT_EOF;
		}
	}
	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_GVoidTransaction
 *
 * DESCRIPTION	: Transaction is marked as void in Genius Database in Ramdisk.
 *
 * RETURN		: STAT_OK, STAT_NOK or STAT_INV    
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Files_GVoidTransaction(GTransactionStruct *fpGTransStruct)
{
	byte lwStatus;
	sint void_tran_type,i;
	sint handle4;
	boolean transaction_with_credit;
	char temp_faiz_amount[14];

	lwStatus = STAT_INV;

	if (RamDisk_ChkDsk((byte *)Transaction_GFile)==RD_CHECK_OK)
	{
		handle4 = RamDisk_Reopen((byte *)Transaction_GFile)->Handle;

		if (RamDisk_Seek(handle4, GBatch_record_index) >= 0)
		{
			fpGTransStruct->void_status = '1';

			if (RamDisk_Overwrite (handle4,(byte *)fpGTransStruct) != RD_OK)  return(STAT_NOK);

			return(STAT_OK);
		}
	}
	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Offline_BIN_File
 *
 * DESCRIPTION	: 
 *
 * RETURN		: STAT_OK or STAT_NOK      
 *
 * NOTES		:  bm OFF      21.10.2004
 *
 * ------------------------------------------------------------------------ */
byte Offline_BIN_File( void )
{
	return ( Offline_BIN_InitFile( ( char * ) OfflineBin_File, sizeof( OfflineBinStruct ) ) );
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: OfflineBIN_InitFile
 *
 * DESCRIPTION	: Initializes a chosen file.
 *
 * RETURN		: STAT_OK or STAT_NOK
 *
 * NOTES        :  bm OFF      21.10.2004
 *
 * ------------------------------------------------------------------------ */
byte Offline_BIN_InitFile( char *lbaFileName, usint Record_Size )
{
	sint NewFileRet;
	char lbtemp[100];
	RamDisk_EraseFile( ( byte * ) lbaFileName );
	NewFileRet = RamDisk_NewFile( ( byte * ) lbaFileName, Record_Size, 0 );
	if ( ( NewFileRet == RD_OUT_OF_ENTRIES ) ||
		 ( NewFileRet == RD_OUT_OF_MEM ) ||
		 ( NewFileRet == RD_OUT_OF_DIR ) || ( NewFileRet == RD_ERROR ) )

		return ( STAT_NOK );

	return ( STAT_OK );

}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Offline_Add_BIN
 *
 * DESCRIPTION	: Save OFFLINE BIN. 
 *
 * RETURN		: lwStatus ( STAT_OK, STAT_NOK )      
 *
 * NOTES		: bm OFF      21.10.2004
 *
 * ------------------------------------------------------------------------ */
sint Offline_Add_BIN( char *OfflineBIN )
{
	File_Entry *fe;
	sint RamDisk_ret;
	static sint handle;
	OfflineBinStruct my_OfflineBinStruct;

	memset( my_OfflineBinStruct.OfflineBIN, 0, 6 );
	memcpy( my_OfflineBinStruct.OfflineBIN, OfflineBIN, 6 );

/*	Slips_WaitForPrinting(  ); 
	Printer_WriteStr( "OfflineBIN:\n" );   
	PrntUtil_BufAsciiHex( ( byte * ) my_OfflineBinStruct.OfflineBIN,  6 );  */

	if ( RamDisk_ChkDsk( ( byte * ) OfflineBin_File ) == RD_CHECK_OK )
	{
		fe = RamDisk_Reopen( ( byte * ) OfflineBin_File );
		handle = fe->Handle;
		if ( RamDisk_Append( handle, ( byte * ) & my_OfflineBinStruct ) > 0 )
/*			Printer_WriteStr ("OfflineBIN bm gec\n");
			PrntUtil_BufAsciiHex((byte *)my_OfflineBinStruct.OfflineBIN, 6 );*/
			return ( STAT_OK );
		Printer_WriteStr( "Offline_Add_BIN Append Error\n" );
		return ( STAT_NOK );
	}
/*
	Printer_WriteStr( " Offline_Add_BIN \n" );
	Printer_WriteStr( " RamDisk_ChkDsk Error\n" );
*/
	return ( STAT_NOK );

}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_Find_Offline_BIN
 *
 * DESCRIPTION	: Find  OFFLINE BIN. from offline bin file
 *
 * RETURN		: lwStatus ( STAT_OK, STAT_NOK )      
 *
 * NOTES		: bm OFF      21.10.2004
 *
 * ------------------------------------------------------------------------ */
byte Files_Find_Offline_BIN( char *strBIN )
{
	OfflineBinStruct XXX;
	File_Entry *fe;
	sint lbStatus;
	static sint handle;

	if ( RamDisk_ChkDsk( ( byte * ) OfflineBin_File ) == RD_CHECK_OK )
	{
		fe = RamDisk_Reopen( ( byte * ) OfflineBin_File );
		handle = fe->Handle;
		if ( RamDisk_Seek( handle, SEEK_FIRST ) >= 0 )
		{
			do
			{
				lbStatus = RamDisk_Read( handle, &XXX );
				if ( ( lbStatus >= 0 ) || ( lbStatus == RD_LAST ) )
				{
					if ( !memcmp( XXX.OfflineBIN, strBIN, 6 ) )
						return ( STAT_OK );
				}
			}while ((lbStatus >= 0) || (lbStatus == RD_LAST));			
//			Printer_WriteStr( "Offline_BIN_File_bos\n" );
			return ( STAT_NOK );
		}
//		Printer_WriteStr( "Offline_BIN_File_bos\n" );
		return ( STAT_NOK );
	}
//	Printer_WriteStr( "RamDisk_ChkDsk Error\n" );
	return ( STAT_NOK );

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Files_Print_Offline_BIN
 *
 * DESCRIPTION	: Print OFFLINE BIN. from offline bin file
 *
 * RETURN		: lwStatus ( STAT_OK, STAT_NOK )      
 *
 * NOTES		: bm OFF      21.10.2004
 *
 * ------------------------------------------------------------------------ */
byte Files_Print_Offline_BIN(  )
{
	OfflineBinStruct XXX;
	File_Entry *fe;
	sint lbStatus;
	static sint handle;
	char temp1[7];
	char temp2[7];
	char temp3[7];
	char toprint[25];
	sint kacinci;
	kacinci = 1;
	if ( RamDisk_ChkDsk( ( byte * ) OfflineBin_File ) == RD_CHECK_OK )
	{
		fe = RamDisk_Reopen( ( byte * ) OfflineBin_File );
		handle = fe->Handle;
		if ( RamDisk_Seek( handle, SEEK_FIRST ) >= 0 )
		{
			Printer_WriteStr( "OFFLINE PREFIXLER\n" );	/*BM GEC */
			Printer_WriteStr( "=================\n" );	/*BM GEC */
			do
			{
				lbStatus = RamDisk_Read( handle, &XXX );
				if ( ( lbStatus >= 0 ) || ( lbStatus == RD_LAST ) )
				{
					if ( ( kacinci == 1 ) || ( kacinci == 5 ) )
					{
						memset( temp1, 0, sizeof( temp1 ) );
						memset( temp2, 0, sizeof( temp2 ) );
						memset( temp3, 0, sizeof( temp3 ) );
						memcpy( temp1, XXX.OfflineBIN,sizeof( XXX.OfflineBIN ) );
						memset( toprint, ' ', sizeof( toprint ) );
						kacinci = 2;
					}else if (kacinci==2)
					{
						memcpy( temp2, XXX.OfflineBIN,sizeof( XXX.OfflineBIN ) );
						kacinci = 3;
					} else  {
						memcpy( temp3, XXX.OfflineBIN,sizeof( XXX.OfflineBIN ) );
						kacinci = 4;
					}
					if ( kacinci == 4 )
					{
						kacinci = 5;
						memcpy( toprint, temp1, 6 );
						toprint[6] = ' ';
						memcpy( &toprint[7], temp2, 6 );
						toprint[13] = ' ';
						memcpy( &toprint[14], temp3, 6 );
						toprint[24] = 0;

						Slips_WaitForPrinting(  ); /**/
						Slips_PrinterStr( toprint );
						PrntUtil_Cr( 1 );
					}
				}

			}while ((lbStatus >= 0) || (lbStatus == RD_LAST));
			if ( kacinci <= 4 )	/* son kayitlari basmak icin eklendi */
			{
				memcpy( toprint, temp1, 6 );
				toprint[6] = ' ';
				memcpy( &toprint[7], temp2, 6 );
				toprint[13] = ' ';
				memcpy( &toprint[14], temp3, 6 );
				toprint[24] = 0;
				Slips_PrinterStr( toprint );
				PrntUtil_Cr( 1 );

			}  
		} 
//		else Printer_WriteStr( "RamDisk_Seek Error\n" );	/*BM GEC */
	}
//	else	Printer_WriteStr( "RamDisk_ChkDsk Error\n" );	/*BM GEC */

	PrntUtil_Cr( 4 );

}