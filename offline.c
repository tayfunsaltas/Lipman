/**************************************************************************
  FILE NAME		: OFFLINE.C
  MODULE NAME	: OFFLINE
  PROGRAMMER	: R.IRFAN OZTURK
  DESCRIPTION	: AKBANK OFFLINE FUNCTIONS
  REVISION		: 06/02/2003
  LABLE			: 06_30
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

/*#include LIPTYPES_DEF*/

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
#include OFFLINE_H

/*#include XLSCHIP_H*/
#include VERDE_H
#include SECURITY_H
#include EMVDEF_H
#include EMVDEFIN_H
#include EMVIF_H

/* Offline Files */
const char OfflineParameter_File[] = "OFFPAR"; /* offline parameters file */
const char OfflineAkbBatch_File[]  = "OFFAKB"; /* offline batch files includes the offline transactions */

/* Variables */
sint Offline_record_index;


/* Booleans */
extern char myinput_type;
extern boolean SECOND_COPY;
extern boolean error_printed;
extern boolean VOID_FLAG;
extern boolean AMEX_FLAG;
extern boolean EOD_flag;
extern boolean INSTALLATION;
extern boolean ATS_FLAG; 
extern boolean TRACK1ERROR;
extern boolean ISIMYAZMA_FLAG;
extern boolean WARNING_FLAG;
extern boolean ERROR_RECEIPT;
extern boolean CHIP_ERROR;
extern boolean BUYUKFIRMA;
extern boolean DEBUG_MODE;
extern boolean OFFLINE_ACIK_FLAG;

/* structure variables */
//bm 15.12.2004 extern char Received_Buffer[900];
extern struct Receipt_Structure         d_Receipt;
extern char device_serial_no;                           
/* variables */
extern char msg[25];
extern char merch_device_no[9];			/* merchant device no */
extern char merch_no[13];               /* merchant no */
extern char merch_addr[73];             /* merch name , merch addr1+ merchaddr2 */
extern char merch_AMEX_no[11];			/* merchant AMEX NO */
extern int  seq_no,batch_no;
extern char v_seq_no_c[5], seq_no_c[5], batch_no_c[4];
extern char amount_surcharge[14];
extern char input_type;
extern int  tran_type;
extern char card_no[20];
extern char exp_date[5];
extern char v_approval_code[7];
extern char v_amount_surcharge[14];
extern char card_no[20];
extern char input_type;
extern char remained_chip_data[17];
extern char net_total[14];
extern char POS_software_ver;
/* -------------------------- */
extern char last_transact_batch_no[4];         
extern char last_transact_seq_no[5];           
extern char last_transact_status;                      
extern char agreement_total[14];
extern char amount_surcharge[14];
extern char exp_date[5];
extern char cvv_2[4];

#define POS_VERSION_NUMBER "0702"	  // bm OFF 21.10.2004
extern sint Cashier_Flag;	  
extern char Kampanya_Data[15];
extern char Kasiyer_No[20];

/* 01_18 irfan 27/06/2000 card track 1 datasý icin tanýmlandý*/
extern char card_info[50];
extern char card_info1[50];
extern char card_info1_name[24];
extern char card_info1_surname[24];

/*Structures*/
extern struct Card_Type_Table d_Card_Type_Table[CARDTYPETABLESIZE];
extern struct Receipt_Structure			d_Receipt;
extern struct Param_Continue_Send		d_Param_Continue_Send;
extern struct Param_Continue_Receive	d_Param_Continue_Receive;
extern struct Accummulators				d_Totals_Counts[4];
extern char POS_Type;			/* Default axess secili. 0:Axess  1:Akbank */


/********************************************************
 ********************** FUCNTIONS ***********************
 *********************************************************/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Offline_InitOfflineParameterFile
 *
 * DESCRIPTION: Inits the Offline Parameters file "OFFPARAM"
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Offline_InitOfflineParameterFile(void)
{
	OfflineParametersStruct fpOfflineParametersStruct;

	/* create offline parameters file */
	if ( Files_InitFile((char *)OfflineParameter_File, Length_OfflinePar) != STAT_OK) 
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("   OFFLINE PARAMETRE    ");
		Slips_PrinterStr("  DOSYASI YARATILAMADI  ");
		PrntUtil_Cr(2);
		Mainloop_DisplayWait("OFFLINE.PAR.DOS.YARATILAMADI    ",1);
		return (STAT_NOK); 
	}
	memset(&fpOfflineParametersStruct,0,sizeof(OfflineParametersStruct));
	/* initialize the parameters and store them*/
	fpOfflineParametersStruct.offline_flag = FALSE;

	memcpy(fpOfflineParametersStruct.max_offline_amount, "0000000000000", 13);

	fpOfflineParametersStruct.max_offline_trx_no_card = 0;

	fpOfflineParametersStruct.max_offline_trx_no_concecutive = 0;
	fpOfflineParametersStruct.VAR_max_offline_trx_no_concecutive = 0;

	memcpy(fpOfflineParametersStruct.OfflinePassword, "0000", 4);

	fpOfflineParametersStruct.VAR_max_daily_offline_trx_count = 0;
	fpOfflineParametersStruct.max_daily_offline_trx_count = 0;

	memcpy(fpOfflineParametersStruct.last_reset_date, "00000000", 8);
	
	/* Write Offline parameters to the RamDisk */
	if(Offline_WriteOfflineParameters(&fpOfflineParametersStruct) != STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    YAZMA  HATASI  ***");
		PrntUtil_Cr(2);
		Mainloop_DisplayWait("OFF.PAR.YAZMA.HT",1);
		return (STAT_NOK); 
	}
	
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Offline_InitOfflineAkbBatchFile
 *
 * DESCRIPTION: Inits the Offline Akbank Batch File OFFAKB
 *
 * RETURN: STAT_OK or STAT_NOK      
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Offline_InitOfflineAkbBatchFile(void)
{
	/* create offline parameters file */
	if ( Files_InitFile((char *)OfflineAkbBatch_File, Length_OfflineAkbBatch_FileStruct) != STAT_OK) 
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("   OFFAKB  BATCH    ");
		Slips_PrinterStr("  DOSYASI YARATILAMADI  ");
		PrntUtil_Cr(2);
		Mainloop_DisplayWait("OFFAKB DOSYASI YARATILAMADI  ",1);
		return (STAT_NOK); 
	}
	
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Offline_ReadOfflineParameters
 *
 * DESCRIPTION: Reads Offline parameters from the Offline parameters file
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte Offline_ReadOfflineParameters(OfflineParametersStruct *fpOfflineParametersStruct)
{
	sint lbStatus, lwLastSeq;
	byte lwStatus;
	//File_Entry *fe;
	sint Handle;
	
	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)OfflineParameter_File) == RD_CHECK_OK)
	{
		Handle = RamDisk_Reopen((byte *)OfflineParameter_File)->Handle;

		if (RamDisk_Seek(Handle, 0) >= 0)
		{
			lbStatus = RamDisk_Read(Handle, fpOfflineParametersStruct);

			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			{
				lwStatus = STAT_OK;
			}
		}
		else 
		{
			lwStatus = STAT_NOK;
		}
	}

	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Offline_WriteOfflineParameters 
 *
 * DESCRIPTION		: Writes general parameters.
 *
 * RETURN			: lwStatus ( STAT_OK, STAT_NOK)     
 *
 * NOTES:			: rio. 20/02/2003
 *
 * ------------------------------------------------------------------------ */
byte Offline_WriteOfflineParameters(OfflineParametersStruct *fpOfflineParametersStruct)
{
	byte lwStatus;
	//File_Entry *fe;
	sint Handle;

	lwStatus = STAT_NOK;

	if (RamDisk_ChkDsk((byte *)OfflineParameter_File)==RD_CHECK_OK)
	{
		Handle = RamDisk_Reopen((byte *)OfflineParameter_File)->Handle;

		if (RamDisk_Seek(Handle, 0) >= 0)
		{
			if (RamDisk_Overwrite (Handle,(byte *)fpOfflineParametersStruct) == RD_OK) 
				lwStatus = STAT_OK;
		}
		else
		{
			if (RamDisk_Append(Handle, fpOfflineParametersStruct) > 0)
				lwStatus = STAT_OK;
		}
	}

	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_AskOfflinePassword
 *
 * DESCRIPTION          : Ask the Oflfine password
 *
 * RETURN               : TRUE, FALSE
 *
 * NOTES                : r.i.o  07/02/2003
 *
 * --------------------------------------------------------------------------*/
byte Offline_AskOfflinePassword(void)
{
	char Buffer[20];
	byte Len;
	char EnteredOperatorPassword[5];
	OfflineParametersStruct fpOfflineParametersStruct;
	char value_null[2];
	char lb_temp_str[20];

	memset(value_null, 0 , sizeof(value_null));

	/* read offline paramter from ram disk */
	if(Offline_ReadOfflineParameters(&fpOfflineParametersStruct) !=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    OKUMA  HATASI  ***");
		PrntUtil_Cr(2);
		Mainloop_DisplayWait("OFF.PAR.OKUMA.HT",1);
		return (STAT_NOK); 
	}
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	memcpy(lb_temp_str, fpOfflineParametersStruct.OfflinePassword,4);

	
	if(!Mainloop_GetKeyboardInput(" ž˜FREY˜ G˜R˜N ?",Buffer,4,4,
				       FALSE, FALSE, TRUE, FALSE, TRUE,&Len,value_null, FALSE))
	{
			return(STAT_OK);
	}
    else
	{

		memcpy(EnteredOperatorPassword,Buffer,Len);
		EnteredOperatorPassword[Len]=0;

		if (Utils_Compare_Strings(EnteredOperatorPassword, lb_temp_str) == 0)
		{
			return(STAT_OK);
		}
		else
		{
			Mainloop_DisplayWait("  YANLIž ž˜FRE  ",2);
			return(STAT_NOK);
		}

	}
}

/************* 06_30 offline islemler ****************************************************************
0433    offline pos parameters (Format of DATA: faaaaaaaaaaaaaaabbccccddddeeee
		f: offline_flag						: 2: offline islemlere acik ve active 1: offline islemlere acik ama active degil 0: offline islemlere kapali
		a: max_offline_amount				: iþlem bazýnda max. offine tutar 
		b: max_offline_trx_no_card			: ayni kart ile ayni batch icinde yapilacak islem sayisi
		c: max_offline_trx_no_concecutive	: arka arkaya yapilabilecek max. offline islem sayisi 
											  bu sayiya ulasildiginda islem online'a force edilir
		d: offline_password					: offline password
		e: max_offline_trx_no_concecutive	: günlük max. offline iþlem sayýsý limiti
*****************************************************************************************************/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_UpdateOfflineParameters
 *
 * DESCRIPTION          : Update the received paramters to the structure and save to the ramdisk.
 *
 * RETURN               : STAT_OK or STAT_NOK
 *
 * NOTES                : bm OFF 21.10.2004
 *
 * --------------------------------------------------------------------------*/
byte Offline_UpdateOfflineParameters(void)
{
	OfflineParametersStruct fpOfflineParametersStruct;
	char temp_str_offline[50];
	sint lbLen;

	/* get the length of the received data */
	memset(temp_str_offline, 0, sizeof(temp_str_offline));
	memcpy(temp_str_offline, d_Param_Continue_Receive.param_data_length,3);
	temp_str_offline[3]=0;
	lbLen=atoi(temp_str_offline);

	/*Numeric Digit Control of offline data */
	if ( ( Utils_NumericCheck(d_Param_Continue_Receive.param_data, lbLen)) != STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***");
		Slips_PrinterStr("\nOffline Parametreler HATALI\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}

	/* read offline paramter from ram disk */
	if(Offline_ReadOfflineParameters(&fpOfflineParametersStruct) !=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    OKUMA  HATASI  ***");
		PrntUtil_Cr(2);
		Mainloop_DisplayWait("OFF.PAR.OKUMA.HT",1);
		return (STAT_NOK); 
	}

	/* offline flag */
/*
	if(d_Param_Continue_Receive.param_data[0] == '2') 
	{
		fpOfflineParametersStruct.offline_flag = TRUE;
		OFFLINE_ACIK_FLAG = TRUE;
	}
	else*/
	{
		fpOfflineParametersStruct.offline_flag = FALSE;
		OFFLINE_ACIK_FLAG = FALSE;
	}

	/* max offline limit */
	memcpy(fpOfflineParametersStruct.max_offline_amount, d_Param_Continue_Receive.param_data+1, 13);

	/* max offline transaction number for each card */
	memset(temp_str_offline, 0, sizeof(temp_str_offline));
	memcpy(temp_str_offline, d_Param_Continue_Receive.param_data+1+13, 2);
	fpOfflineParametersStruct.max_offline_trx_no_card = atoi(temp_str_offline);

	/* max offline transaction number for cosecutive trx in the same pos */
	memset(temp_str_offline, 0, sizeof(temp_str_offline));
	memcpy(temp_str_offline, d_Param_Continue_Receive.param_data+1+13+2, 4);
	fpOfflineParametersStruct.max_offline_trx_no_concecutive = atoi(temp_str_offline);
	
	/* set zero the variable of VAR_max_offline_trx_no_concecutive */
	fpOfflineParametersStruct.VAR_max_offline_trx_no_concecutive = 0;

	/* offline password */
	memcpy(fpOfflineParametersStruct.OfflinePassword, d_Param_Continue_Receive.param_data+1+13+2+4, 4);

	/* max offline transaction number for cosecutive trx in the same pos */
	memset(temp_str_offline, 0, sizeof(temp_str_offline));
	memcpy(temp_str_offline, d_Param_Continue_Receive.param_data+1+13+2+4+4, 4);
	fpOfflineParametersStruct.max_daily_offline_trx_count = atoi(temp_str_offline);

	/* Write Offline parameters to the RamDisk */
	if(Offline_WriteOfflineParameters(&fpOfflineParametersStruct) != STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    YAZMA  HATASI  ***");
		PrntUtil_Cr(2);
		Mainloop_DisplayWait("OFF.PAR.YAZMA.HT",1);
		return (STAT_NOK); 
	}
	
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_PrintOfflineParameters
 *
 * DESCRIPTION          : Ask the Oflfine password
 *
 * RETURN               : TRUE, FALSE
 *
 * NOTES                : r.i.o  07/02/2003
 *
 * --------------------------------------------------------------------------*/
byte Offline_PrintOfflineParameters(void)
{
	OfflineParametersStruct fpOfflineParametersStruct;
	char lb_temp_str[100];
	char lb_temp_str2[100];
	char value_null[2];

	memset(value_null, 0 , sizeof(value_null));

	/* read offline paramter from ram disk */
	if(Offline_ReadOfflineParameters(&fpOfflineParametersStruct) !=STAT_OK)
	{
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    OKUMA  HATASI  ***");
		PrntUtil_Cr(4);
		Mainloop_DisplayWait("OFF.PAR.OKUMA.HT",1);
		return (STAT_NOK); 
	}

	/* Header */
	Slips_PrinterStr("--OFFLINE PARAMETRELER--");
	Slips_PrinterStr("------------------------");

	/* offline flag */
	PrntUtil_Cr(2);
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	sprintf(lb_temp_str, "offline flag : %d \n", fpOfflineParametersStruct.offline_flag);
	Slips_PrinterStr(lb_temp_str);

	/* max_offline_amount */
	PrntUtil_Cr(2);
	memset(lb_temp_str2, 0, sizeof(lb_temp_str2));
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	memcpy(lb_temp_str2, fpOfflineParametersStruct.max_offline_amount, 13);
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		sprintf( lb_temp_str, "max_offline_amount: \n   %sYTL \n",
				 lb_temp_str2 );
	else
		sprintf( lb_temp_str, "max_offline_amount: \n   %s TL \n",
				 lb_temp_str2 );
	Slips_PrinterStr(lb_temp_str);

	/* max_offline_trx_no_card */
	PrntUtil_Cr(2);
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	sprintf(lb_temp_str, "max_offline_trx_no_card: %d \n", fpOfflineParametersStruct.max_offline_trx_no_card);
	Slips_PrinterStr(lb_temp_str);

	/* max_offline_trx_no_concecutive */
	PrntUtil_Cr(2);
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	sprintf(lb_temp_str, "max_offline_trx_no_concecutive : %d \n", fpOfflineParametersStruct.max_offline_trx_no_concecutive);
	Slips_PrinterStr(lb_temp_str);

	/* VAR_max_offline_trx_no_concecutive */
	PrntUtil_Cr(2);
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	sprintf(lb_temp_str, "VAR_max_offline_trx_no_concecutive : %d \n", fpOfflineParametersStruct.VAR_max_offline_trx_no_concecutive);
	Slips_PrinterStr(lb_temp_str);
	
	/* offline password */
	PrntUtil_Cr(2);
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	memset(lb_temp_str2, 0, sizeof(lb_temp_str2));
	memcpy(lb_temp_str2, fpOfflineParametersStruct.OfflinePassword,4);
	sprintf(lb_temp_str, "offline password : %s \n", lb_temp_str2);
	Slips_PrinterStr(lb_temp_str);

	/* max_daily_offline_trx_count */
	PrntUtil_Cr(2);
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	sprintf(lb_temp_str, "max_daily_offline_trx_count : %d \n", fpOfflineParametersStruct.max_daily_offline_trx_count);
	Slips_PrinterStr(lb_temp_str);

	/* VAR_max_daily_offline_trx_count */
	PrntUtil_Cr(2);
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	sprintf(lb_temp_str, "VAR_max_daily_offline_trx_count : %d \n", fpOfflineParametersStruct.VAR_max_daily_offline_trx_count);
	Slips_PrinterStr(lb_temp_str);

	/* last_rest_date */
	PrntUtil_Cr(2);
	memset(lb_temp_str2, 0, sizeof(lb_temp_str2));
	memset(lb_temp_str, 0, sizeof(lb_temp_str));
	memcpy(lb_temp_str2, fpOfflineParametersStruct.last_reset_date, 8);
	sprintf(lb_temp_str, "last_reset_date:%s", lb_temp_str2);
	Slips_PrinterStr(lb_temp_str);

	PrntUtil_Cr(5);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_Check_max_offline_trx_no_card
 *
 * DESCRIPTION          : Checks that if the offline limit with this card is exceeded or not
 *
 * RETURN               : TRUE: trx can be performed, FALSE: Offline limit is exceeded
 *
 * NOTES                : r.i.o  24/02/2003
 *
 * --------------------------------------------------------------------------*/
byte Offline_Check_max_offline_trx_no_card(char *temp_VAR_max_offline_trx_no_card)
{
	OfflineAkbBatch_FileStruct lbOfflineAkbBatch_FileStruct;
	sint lbStatus, lwLastSeq, i;
	byte lwStatus;
//	File_Entry *fe;
	sint lbVAR_max_offline_trx_no_card;
	sint Handle;
	
	lbVAR_max_offline_trx_no_card = atoi(temp_VAR_max_offline_trx_no_card);

	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)OfflineAkbBatch_File)==RD_CHECK_OK)
	{
		Handle = RamDisk_Reopen((byte *)OfflineAkbBatch_File)->Handle;

		RamDisk_Seek(Handle, SEEK_FIRST);
		lwLastSeq = RamDisk_Seek(Handle,SEEK_LAST);

		for (i=0; i<lwLastSeq; i++)
		{
			if (RamDisk_Seek(Handle, i) >= 0)
			{

				lbStatus = RamDisk_Read(Handle, &lbOfflineAkbBatch_FileStruct);
				if ((lbStatus >= 0) || (lbStatus == RD_LAST))
				{
					if (memcmp(lbOfflineAkbBatch_FileStruct.card_no, card_no, strlen(card_no)) == 0)
					{
						lbVAR_max_offline_trx_no_card++;
						lwStatus = STAT_OK;
					}
				}
				else if (lbStatus == RD_EOF)
					lwStatus = STAT_EOF;
			}
			else lwStatus = STAT_EOF;
		}
		lwStatus = STAT_EOF;
	}
	
	/* convert to ascii */
	Utils_IToA(lbVAR_max_offline_trx_no_card,temp_VAR_max_offline_trx_no_card);
	Utils_LeftPad(temp_VAR_max_offline_trx_no_card, '0', 4);

	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_AllowOffline
 *
 * DESCRIPTION          : Checks the neccessary criteria for offline trx. and decide whether to go on or not
 *
 * RETURN               : TRUE: Offline trx can be performed, FALSE: Offline trx. cannot be performed.(Not allowed)
 *
 * NOTES                : r.i.o  24/02/2003
 *						check_what = 1	; Check If Pos is open to Offline transaction
 *						check_what = 2	; Check If the card is debit or not
 *						check_what = 3	; Check for max_offline_trx_no_card Card_based
 *						check_what = 4	; check for max_offline_trx_no_concecutive. pos_based
 *						check_what = 5	; check for max_offline_amount pos_based
 *						chect_what = 6	: check for max_daily_offline_trx_count
 * --------------------------------------------------------------------------*/
boolean Offline_AllowOffline(int check_what)
{
	OfflineParametersStruct lbOfflineParametersStruct;
	usint lbVAR_max_offline_trx_no_card; 
	char temp_VAR_max_offline_trx_no_card[5];
	char lbStr[50];
	char auto_date_c[9], auto_time_c[5];

	/* read offline paramter from ram disk */
	if(Offline_ReadOfflineParameters(&lbOfflineParametersStruct) !=STAT_OK)
	{
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    OKUMA  HATASI  ***");
		PrntUtil_Cr(4);
		Mainloop_DisplayWait("OFF.PAR.OKUMA.HT",1);
		return (FALSE); 
	}

	switch (check_what)
	{
		case 1:	/* Check If Pos is open to Offline transaction */
			if(!lbOfflineParametersStruct.offline_flag) 
			{
				Mainloop_DisplayWait("TERMINAL OFFLINEISLEMLERE KAPALI",2);
				return(FALSE);
			}
			break;

		case 2:	/* Check If the card is debit or not */

			/* Check for Debit - 1 - */
			if( (card_no[0] == '1') || (card_no[0] == '2') || (card_no[0] == '6') || (card_no[0] == '7') ||
				(card_no[0] == '8') || (card_no[0] == '9') 
			  )
			{
				Mainloop_DisplayWait("DEBIT KART.OFFL.ISLEM YAPILAMAZ ",2);
				return(FALSE);
			}

			/* Check for Debit - 2 - */
			if(Utils_Debit_Check())
			{
				Mainloop_DisplayWait("DEBIT KART.OFFL.ISLEM YAPILAMAZ ",2);
				return(FALSE);
			}

			/* Check for Debit - 3 - */
			if ( (card_no[0] == '1') == 3)
			{
				if ((memcmp(card_no, "34", 2) == 0) || (memcmp(card_no, "37", 2) == 0 ))
				{
					return(TRUE);
				}
				else
				{
					Mainloop_DisplayWait("DEBIT KART.OFFL.ISLEM YAPILAMAZ ",2);
					return(FALSE);
				}
			}
			break;

		case 3:	/* Check for max_offline_trx_no_card Card_based */
				memset(temp_VAR_max_offline_trx_no_card, 0, sizeof(temp_VAR_max_offline_trx_no_card));
				if( Offline_Check_max_offline_trx_no_card(temp_VAR_max_offline_trx_no_card) != STAT_EOF)
				{
					Mainloop_DisplayWait("  OFFAKB BATCH  FILE OKUMA HATAS",2);
					return(FALSE);
				}

				/* convert to integer */
				lbVAR_max_offline_trx_no_card = atoi(temp_VAR_max_offline_trx_no_card);

				if(lbVAR_max_offline_trx_no_card > (lbOfflineParametersStruct.max_offline_trx_no_card-1) ) 
				{

					Mainloop_DisplayWait("AYNI KRTLA ISLEM LIMITI ASILDI  ",2);
					return(FALSE);
				}
			break;

		case 4:	/* check for max_offline_trx_no_concecutive. pos_based */
			if(lbOfflineParametersStruct.VAR_max_offline_trx_no_concecutive > (lbOfflineParametersStruct.max_offline_trx_no_concecutive -1))
			{
				Mainloop_DisplayWait("  ONLINE ISLEM      YAPINIZ     ",2);
				return(FALSE);
			}

			break;

		case 5:	/* check for max_offline_amount pos_based */
			memset(lbStr, 0, sizeof(lbStr));
			memcpy(lbStr, lbOfflineParametersStruct.max_offline_amount, 13);

			if(Utils_Compare_Strings(amount_surcharge, lbStr) == 1)
			{  /* 1 : amount_surcharge > lbStr */
				Mainloop_DisplayWait(" OFFLINE TUTAR    LIMIT ASIMI   ",2);
				return(FALSE);
			}

			break;

		case 6:	/* check for max_daily_offline_trx_count */

			/* reset max_daily_offline_trx_count if date is different */
			memset(auto_date_c, 0, sizeof(auto_date_c));
			memset(auto_time_c, 0, sizeof(auto_time_c));
			memset(lbStr, 0, sizeof(lbStr));
			memcpy(lbStr, lbOfflineParametersStruct.last_reset_date, 8);
			/* pos tarih ve saatini alir */
			Utils_GetDateTime (auto_date_c, auto_time_c);

			if(Utils_Compare_Strings(auto_date_c, lbStr) != 0)
			{
				lbOfflineParametersStruct.VAR_max_daily_offline_trx_count = 0 ;
				memcpy(lbOfflineParametersStruct.last_reset_date, auto_date_c, 8);

				/* write the modifed value to the parameters file */
				if(Offline_WriteOfflineParameters(&lbOfflineParametersStruct) != STAT_OK)
				{
					PrntUtil_Cr(2);
					Slips_PrinterStr("** OFFLINE PARAMETRE ***");
					Slips_PrinterStr("**    YAZMA  HATASI  ***");
					PrntUtil_Cr(2);
					Mainloop_DisplayWait("OFF.PAR.YAZMA.HT",1);
					return (STAT_NOK); 
				}
			}
	
			if( lbOfflineParametersStruct.max_daily_offline_trx_count < (lbOfflineParametersStruct.VAR_max_daily_offline_trx_count + 1) )
			{
				Mainloop_DisplayWait("  GUNLUK LIMIT        ASILDI    ",2);
				return(FALSE);
			}

			break;

		default:
			break;

	}
		
	return(TRUE);
}

/* --------------------------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_Transaction
 *
 * DESCRIPTION          : Hande offline sale 
 *
 * RETURN               : STAT OK, STAT_NOK
 *
 * NOTES                : r.i.o. 
 *
 * -------------------------------------------------------------------------------------------- */
byte Offline_Transaction(void)
{
	byte Transaction_Ret;
	OfflineParametersStruct	lbOfflineParametersStruct;
	char tmp_exp_date[5];
	char local_device_date[8], local_device_time[6];
	char tmp_card_no[6];
	tran_type = SALE;

	/* Check for installation*/
	if ( !INSTALLATION)
	{
		Mainloop_DisplayWait(" KURULUž YAPIN  ",1);
		return(STAT_NOK);
	}

	/* Check for incomplete EOD transcation*/
	if (EOD_flag)
	{
		Mainloop_DisplayWait(" GšNSONU YAPIN  ",1);
		return(STAT_NOK);
	}

	/* Set the flags */
	VOID_FLAG       = FALSE;
	SECOND_COPY     = FALSE;

	/* decision mechanism for offline trx */
	/*==================================== */
	/* check for expire date  13.07.2004 */
	memset( tmp_exp_date, 0, sizeof( tmp_exp_date ) );
	memset( local_device_date, 0, sizeof( local_device_date ) );
	memset( local_device_time, 0, sizeof( local_device_time ) );

	/** get pos date and time **/
	Utils_GetDateTime( local_device_date, local_device_time );
	tmp_exp_date[0] = local_device_date[6];
	tmp_exp_date[1] = local_device_date[7];
	tmp_exp_date[2] = local_device_date[3];
	tmp_exp_date[3] = local_device_date[4];

	if ( memcmp( exp_date, tmp_exp_date, 4 ) < 0 )
	{
		Mainloop_DisplayWait( "SON KULLANIM    TARIHI GECMIS   ", 2 );
		return ( STAT_NOK );
	}
	/* input type control only card swipe is valid*/
	if ( ( input_type == INPUT_FROM_KEYPAD ) ||
		 ( input_type == INPUT_FROM_CHIPCARD ) )
	{
		Mainloop_DisplayWait("   MANYETIK     OKUYUCU KULLANIN",1);	
		return ( STAT_NOK );
	}

		/* check_what = 1	; Check If Pos is open to Offline transaction */
		if(!Offline_AllowOffline(1)) return(STAT_NOK);

	memset( tmp_card_no, 0, sizeof( tmp_card_no ) );
	memcpy( tmp_card_no, card_no, 6 );

	if ( Files_Find_Offline_BIN( tmp_card_no ) )
	{
		Mainloop_DisplayWait( "BU KARTLA OFF.  ISLEM YAPILAMAZ ", 2 );
		return ( STAT_NOK );
	}
		/*	check_what = 2	; Check If the card is debit or not */
		if(!Offline_AllowOffline(2)) return(STAT_NOK);

		/*	check_what = 3	; Check for max_offline_trx_no_card Card_based */
		if(!Offline_AllowOffline(3)) return(STAT_NOK);

		/*	check_what = 4	; check for max_offline_trx_no_concecutive. pos_based */
		if(!Offline_AllowOffline(4)) return(STAT_NOK);

		/*	check_what = 6	; check for max_daily_offline_trx_count */
		if(!Offline_AllowOffline(6)) return(STAT_NOK);


	/* get amount */
	if ( !Offline_GetAmount(  ) )
		return ( STAT_NOK );

	/* Increment sequence number*/
	seq_no++;
	Utils_IToA(seq_no,seq_no_c);
	Utils_LeftPad(seq_no_c, '0', 4);


	/*	check_what = 5	; check for max_offline_amount pos_based */
	if ( !Offline_AllowOffline( 5 ) )
		return ( STAT_NOK );

	/************* update the counters ****************/
	/*************************************************/
	/* read offline paramter from ram disk */
	if ( Offline_ReadOfflineParameters( &lbOfflineParametersStruct ) !=
		 STAT_OK )
	{
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    OKUMA  HATASI  ***");
		PrntUtil_Cr(4);
		Mainloop_DisplayWait("OFF.PAR.OKUMA.HT",1);
		return (FALSE); 
	}
	/* increment dailly counter */
	lbOfflineParametersStruct.VAR_max_daily_offline_trx_count =
		lbOfflineParametersStruct.VAR_max_daily_offline_trx_count + 1;
	/* increment consevutive counter */
	lbOfflineParametersStruct.VAR_max_offline_trx_no_concecutive =
		lbOfflineParametersStruct.VAR_max_offline_trx_no_concecutive + 1;

	if ( Offline_WriteOfflineParameters( &lbOfflineParametersStruct ) !=
		 STAT_OK )
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("** OFFLINE PARAMETRE ***");
		Slips_PrinterStr("**    YAZMA  HATASI  ***");
		PrntUtil_Cr(2);
		Mainloop_DisplayWait("OFF.PAR.YAZMA.HT",1);
		return (STAT_NOK); 
	}
	/********************************************************************/
	
	/* record to the offline batch file */
	if(Offline_WriteOFFAKB() != STAT_OK)
	return(STAT_NOK);


	/* Fill slip */
	if(!Offline_FillReceiptStructure()) 
	{
		Mainloop_DisplayWait("Offline_FillReceiptStructure hatasi",2);
		return(STAT_NOK);
	}

	/* write the globals to the file */
	Files_WriteAllParams();

	/* print the slip */
	Offline_PrintReceipt();

	if ( WARNING_FLAG )
	{
		Mte_Wait(1000);
		Kbhw_BuzzerPulse(200);

		Mte_Wait(300);
		Kbhw_BuzzerPulse(200);
	}

	SECOND_COPY = TRUE;
	
	if(SECOND_COPY)
	{
		Mainloop_DisplayWait(" 2. KOPYA ˜€˜N  G˜R˜ž'E BASINIZ.",10);
		Offline_PrintReceipt();
	}
	ERROR_RECEIPT = FALSE;

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_GetAmount 
 *
 * DESCRIPTION          : Gets the offline amount  
 *
 * RETURN               : TRUE or FALSE
 *
 * NOTES                : r.i.o 26/02/2003
 *
 * ------------------------------------------------------------------------ */
boolean Offline_GetAmount(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char value_null[2];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));
	
	if(!Mainloop_GetKeyboardInput((char *)MESSAGE_ENTER_AMOUNT,Buffer,0,11,
					   FALSE, FALSE, FALSE, TRUE, FALSE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("  TUTAR GIRISI       HATALI     ",2);
		return(FALSE);
	}
	else
	{
		memcpy(amount_surcharge,Buffer,Len);
		amount_surcharge[Len]=0;
		Utils_LeftPad(amount_surcharge, '0', 13);
	}

	return(TRUE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Offline_Cancel
 *
 * DESCRIPTION		: execute offine cancel transaction
 *
 * RETURN			: STAT_OK, STAT_NOK
 *
 * NOTES			: r.i.o  01/08/01
 *
 * ------------------------------------------------------------------------ */
byte Offline_Cancel(void) 
{
/*	boolean void_msg_OK;
	byte void_ret_code;

	void_msg_OK = FALSE;*/

	char entered_seq_no[5], input_card_no[20];
	char Buffer[LEN_GETKEYBOARD];
	int i;
	byte Len;
	boolean card_number_match, check_card_number;
	char value_null[2];
	char zero_amount_local[14];

	OfflineAkbBatch_FileStruct lbOfflineAkbBatch_FileStruct;

/**************************************/

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));


/* Set the Preprov Specific Parameters */
	tran_type = CANCEL;
	VOID_FLAG = TRUE;

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

	/* read global parameters */
	if(Files_ReadAllParams() != STAT_OK) return (STAT_NOK);

	/*	Get the Transaction Sequence Number */
	if(!Mainloop_GetKeyboardInput("OFFLINE SIRA NO?",Buffer,1,4,
				      FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, FALSE))
	{
		return(STAT_NOK);
	}
	else
	{
		memcpy(entered_seq_no,Buffer,Len);
		entered_seq_no[Len]=0;
	}
	Utils_LeftPad(entered_seq_no,'0',4);
	memcpy(lbOfflineAkbBatch_FileStruct.seq_no, entered_seq_no, 4);

	card_number_match = FALSE;

	if (Offline_ReadAKBOFF(&lbOfflineAkbBatch_FileStruct) == STAT_OK)
	{
		if (lbOfflineAkbBatch_FileStruct.void_status == '1')
		{
			Mainloop_DisplayWait("˜PTAL ED˜LM˜ž",2);
			return(STAT_NOK);
		}

		card_number_match = TRUE;
		check_card_number = TRUE;

		memcpy(input_card_no, lbOfflineAkbBatch_FileStruct.card_no, 19);
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
			memcpy(v_approval_code, lbOfflineAkbBatch_FileStruct.approval_code, 6);
			v_approval_code[6] = 0;

			memcpy(v_amount_surcharge, lbOfflineAkbBatch_FileStruct.amount_surcharge, 13);
			v_amount_surcharge[13] = 0;

			memcpy(v_seq_no_c, lbOfflineAkbBatch_FileStruct.seq_no, 4);
			v_seq_no_c[4] = 0;

			Utils_IToA(batch_no,batch_no_c);        
			Utils_LeftPad(batch_no_c, '0', 3);

			/* Fill slip */
			if(!Offline_FillReceiptStructure()) 
			{
				Mainloop_DisplayWait("Offline_FillReceiptStructure hatasi",2);
				return(STAT_NOK);
			}

			/* print the slip */
			Offline_PrintReceipt();

			ERROR_RECEIPT = FALSE;

			if (Offline_VoidTransaction(&lbOfflineAkbBatch_FileStruct) == STAT_OK)
			{
				/*Trans_UpdateVoidCardTypeTable(fpTransStruct);*//* bu kisim batch upload sirasinda dusunulecek */
				return(STAT_OK);
			}
			else
			{
				Mainloop_DisplayWait("UPDATE BAžARISIZ",2);
				return(STAT_NOK);
			}
		}
		else
		{
			Mainloop_DisplayWait("KART NO YANLIž",2);
		}
	}
	else
		Mainloop_DisplayWait("KAYIT YOK",2);

	return(STAT_NOK);

/*********************************************/


}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_WriteOFFAKB
 *
 * DESCRIPTION          : Write offline record to the OFFAKB batch file
 *
 * RETURN               : STAT_OK, STAT_NOK or STAT_INV    
 *
 * NOTES                : r.i.o. 26/02/99       
 *
 * ------------------------------------------------------------------------ */
byte Offline_WriteOFFAKB( void)
{
	OfflineAkbBatch_FileStruct lbOfflineAkbBatch_FileStruct;
	char lboffline_approval_code[7];			
	char lbdevice_date[8], lbdevice_time[6];	
	sint Status;
	char lbStr[900];
	char device_date[8], device_time[6];
		
	/* initialize the record */
	memset(&lbOfflineAkbBatch_FileStruct,' ',sizeof(lbOfflineAkbBatch_FileStruct));

	memcpy(lbOfflineAkbBatch_FileStruct.seq_no, seq_no_c, 4);
	memcpy(lbOfflineAkbBatch_FileStruct.card_type, "15", 2);
	memcpy(lbOfflineAkbBatch_FileStruct.amount_surcharge, amount_surcharge, 13);

	/** offline approval code is generated **/
	memset(lboffline_approval_code, 0, sizeof(lboffline_approval_code));
	memcpy(lboffline_approval_code, batch_no_c,3);
	memcpy(lboffline_approval_code+3,seq_no_c+1,3);
	memcpy(lbOfflineAkbBatch_FileStruct.approval_code, lboffline_approval_code, 6);
		
	/** assign pos date and time **/
	Utils_GetDateTime(lbdevice_date,lbdevice_time);
	memcpy(&lbOfflineAkbBatch_FileStruct.date[0], &lbdevice_date,8);
	memcpy(lbOfflineAkbBatch_FileStruct.time, lbdevice_time,5);
	lbOfflineAkbBatch_FileStruct.tran_type   = tran_type + '0';
	
	memcpy(lbOfflineAkbBatch_FileStruct.card_no, card_no, strlen(card_no));
	lbOfflineAkbBatch_FileStruct.void_status = '0';

	memcpy(lbOfflineAkbBatch_FileStruct.amount_surcharge, amount_surcharge, 13);

	/* move message part */
	memset(lbStr, 0, sizeof(lbStr));
	Offline_FillTransactionSend(lbStr);
	memcpy(&lbOfflineAkbBatch_FileStruct.lb_Transaction_Send , lbStr, sizeof(lbOfflineAkbBatch_FileStruct.lb_Transaction_Send));

	/* write the filled record to the AKBANK batch */
	Status = Offline_AddToOFFAKB(&lbOfflineAkbBatch_FileStruct);
	if (Status > 0)
	{
		return(STAT_OK);
	}
	else
	{
		switch(Status)
		{
		case  RD_OUT_OF_MEM:
			Mainloop_DisplayWait("OFFLINE YIGIN   DOLU. GNS. YAPIN",1);
		 	return(STAT_NOK);
		default:
			Mainloop_DisplayWait("KAYIT YAZILAMADI",1);
			return(STAT_NOK);
		}
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Offline_AddToOFFAKB
 *
 * DESCRIPTION	: Adds record to the OFFAKB
 *
 * RETURN		: STAT_OK, STAT_NOK or STAT_INV
 *
 * NOTES		: r.i.o. 26/02/2003       
 *
 * ------------------------------------------------------------------------ */
sint Offline_AddToOFFAKB(OfflineAkbBatch_FileStruct *fpOfflineAkbBatch_FileStruct)
{
	//File_Entry *fe;
	sint RamDisk_ret;
	sint Handle;

	if (RamDisk_ChkDsk((byte *)OfflineAkbBatch_File)==RD_CHECK_OK)
	{
		Handle = RamDisk_Reopen((byte *)OfflineAkbBatch_File)->Handle;
		RamDisk_ret = RamDisk_Append(Handle,fpOfflineAkbBatch_FileStruct);
	
		if(RamDisk_ret > 0)	return(RamDisk_ret);
	}

	return (RD_ERROR);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Offline_ReadAKBOFF
 *
 * DESCRIPTION		: Reads from the transaction database.
 *
 * RETURN			: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:			: r.i.o. 28/02/2003
 *
 * ------------------------------------------------------------------------ */
byte Offline_ReadAKBOFF(OfflineAkbBatch_FileStruct *fpOfflineAkbBatch_FileStruct )
{
	sint lbStatus, lwLastSeq, i;
	byte lwStatus;
//	File_Entry *fe;
	OfflineAkbBatch_FileStruct TempOfflineAkbBatch_FileStruct;
	sint Handle;

	lwStatus = STAT_INV;
	if (RamDisk_ChkDsk((byte *)OfflineAkbBatch_File)==RD_CHECK_OK)
	{
		Handle = RamDisk_Reopen((byte *)OfflineAkbBatch_File)->Handle;

		RamDisk_Seek(Handle, SEEK_FIRST);
		lwLastSeq = RamDisk_Seek(Handle,SEEK_LAST);

		for (i=0; i<lwLastSeq; i++)
		{
			if (RamDisk_Seek(Handle, i) >= 0)
			{
				lbStatus = RamDisk_Read(Handle, &TempOfflineAkbBatch_FileStruct);
				if ((lbStatus >= 0) || (lbStatus == RD_LAST))
				{
					if (memcmp(TempOfflineAkbBatch_FileStruct.seq_no, fpOfflineAkbBatch_FileStruct->seq_no, length_seq_no) == 0)
					{
						Offline_record_index = i;
						memcpy(fpOfflineAkbBatch_FileStruct, &TempOfflineAkbBatch_FileStruct, Length_OfflineAkbBatch_FileStruct);
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
 * FUNCTION NAME	: Offline_VoidTransaction
 *
 * DESCRIPTION		: Offline Transaction is marked as void in Ramdisk.
 *
 * RETURN			: STAT_OK, STAT_NOK or STAT_INV    
 *
 * NOTES:			: r.i.o. 28/02/2003
 *
 * ------------------------------------------------------------------------ */
byte Offline_VoidTransaction(OfflineAkbBatch_FileStruct *fpOfflineAkbBatch_FileStruct)
{
	byte lwStatus;
	//File_Entry *fe;
	sint void_tran_type,i;
	sint Handle;

	lwStatus = STAT_INV;

	if (RamDisk_ChkDsk((byte *)OfflineAkbBatch_File)==RD_CHECK_OK)
	{
		Handle = RamDisk_Reopen((byte *)OfflineAkbBatch_File)->Handle;

		if (RamDisk_Seek(Handle, Offline_record_index) >= 0)
		{                       
			fpOfflineAkbBatch_FileStruct->void_status = '1';

			void_tran_type = fpOfflineAkbBatch_FileStruct->tran_type - '0';

			if (RamDisk_Overwrite (Handle,(byte *)fpOfflineAkbBatch_FileStruct) == RD_OK) 
			{
					lwStatus = STAT_OK;
			}
		}
	}

	return(lwStatus);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Offline_FillReceiptStructure.
 *
 * DESCRIPTION		: Fill the receipt structure
 *
 * RETURN			: TRUE or FALSE
 *      
 * NOTES			: r.i.o. 26/02/2003
 *
 * ------------------------------------------------------------------------ */
boolean Offline_FillReceiptStructure(void)
{
	char lboffline_approval_code[7];
	char lbdevice_date[8], lbdevice_time[5];
	char lbformatted_card_no[24];
	char lbformatted_exp_date[6];
	char amount_temp[25];
	char received_amount[14];
	int number_of_zeros,Status;
	char temp_name[25];							/* 02_02 irfan 15/08/2000 
													isim ve soyismi tutuyor */

	/* fill d_Receipt structure by space and initialize flags */
	memset(&d_Receipt, ' ', sizeof(d_Receipt));
	d_Receipt.SURCHARGE=FALSE;              /* default FALSE */
	d_Receipt.R_VOID=FALSE;                 /* default FALSE */

	d_Receipt.R_VOID = VOID_FLAG;

	/* terminal no */
	memcpy(d_Receipt.term_no, merch_device_no, 8);
	d_Receipt.term_no[8] = 0;
	
	/* get date and time and format them */
	Utils_GetDateTime(lbdevice_date,lbdevice_time);

	memcpy(d_Receipt.date,lbdevice_date,8);
	d_Receipt.date[8]=0;

	memcpy(d_Receipt.time,lbdevice_time,5);
	d_Receipt.time[5] = 0;

	/* Formatting and filling the card no and exp date */
	memset(lbformatted_card_no, 0, sizeof(lbformatted_card_no));
	memset(lbformatted_exp_date, 0, sizeof(lbformatted_exp_date));
	Slips_FormatCardExp(lbformatted_card_no, lbformatted_exp_date, card_no, exp_date);
	memcpy(d_Receipt.card_no, lbformatted_card_no, strlen(lbformatted_card_no));
	d_Receipt.card_no[strlen(lbformatted_card_no)] = 0;

	memcpy(d_Receipt.exp_date, lbformatted_exp_date, strlen(lbformatted_exp_date));
	d_Receipt.exp_date[strlen(lbformatted_exp_date)] = 0;

	if (input_type == '1' )
		/*d_Receipt.input_type*/ myinput_type = 'T';
	else if  (input_type == '2' )
		/*d_Receipt.input_type*/ myinput_type = 'D';
	else if  (input_type == '3' )
		/*d_Receipt.input_type*/ myinput_type = 'C';	
	else if  (input_type == '4' )
		/*d_Receipt.input_type*/ myinput_type = 'E';
	else if  (input_type == '5' )
		/*d_Receipt.input_type*/ myinput_type = 'B';
	else if  (input_type == '6' )
		/*d_Receipt.input_type*/ myinput_type = 'F';
	else
	{
		Mainloop_DisplayWait("G˜R˜ž T˜P˜ HATASI",1);
		/*d_Receipt.input_type*/ myinput_type = 'X';
	}

	/*ref no: batch no + seq no */
	memcpy(d_Receipt.ref_no,batch_no_c,3);
	d_Receipt.ref_no[3]='/';


	/** XLS 04_02. 19/12/2001 irfan.track 1 hatalý olduðunda bir onceki islemin ismi basýlýyordu. engellendi**/
	if ( ( TRACK1ERROR && ( /*d_Receipt.input_type*/ myinput_type == 'D' ) )
		 || ( CHIP_ERROR && ( /*d_Receipt.input_type*/ myinput_type == 'C' ) ) )
	{
		memset(temp_name, 0 , sizeof(temp_name));
		sprintf(temp_name, "%s %s", card_info1_name, card_info1_surname);
		temp_name[strlen(card_info1_name) + strlen(card_info1_surname)+1] = 0;
		memcpy(d_Receipt.Card_Holder_Name, temp_name, strlen(temp_name));
		d_Receipt.Card_Holder_Name[24] = 0; /* ends with null */
	}
	else
	{
		/* initialize the card holder name */
		memset(d_Receipt.Card_Holder_Name , 0, 24);
	}

	if(!VOID_FLAG)
	{
		/* tran type */
		d_Receipt.tran_type = tran_type;
		
		/* ref no */
		memcpy(&d_Receipt.ref_no[4],seq_no_c,4);
		d_Receipt.ref_no[8]=0;

		/* amount_surcharge for OFFLINE SALE*/
		Slips_FormatAmount(amount_surcharge,amount_temp);
		number_of_zeros = (int)((21-strlen(amount_temp))/2);
		memcpy( &d_Receipt.amount[number_of_zeros], amount_temp,
				strlen( amount_temp ) );
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
			strcpy( &d_Receipt.
					amount[number_of_zeros + strlen( amount_temp )], "YTL" );
		else
			strcpy( &d_Receipt.
					amount[number_of_zeros + strlen( amount_temp )], " TL" );
		d_Receipt.amount[24]=0;

		/* offline approval code for OFFLINE SALE*/
		memset(lboffline_approval_code, 0, sizeof(lboffline_approval_code));
		memcpy(lboffline_approval_code, batch_no_c,3);
		memcpy(lboffline_approval_code+3,seq_no_c+1,3);
		memcpy(d_Receipt.approval_code, lboffline_approval_code,6);
		d_Receipt.approval_code[6] = 0; 

	}
	else
	{
		/* tran type */
		d_Receipt.tran_type = 6;
		d_Receipt.SURCHARGE=FALSE;

		/* ref no */
		memcpy(&d_Receipt.ref_no[4],v_seq_no_c,4);
		d_Receipt.ref_no[8]=0;

		/* amount_shurcarge for OFFLINE CANCEL */
		Slips_FormatAmount(v_amount_surcharge,amount_temp);
		number_of_zeros = (int)((21-strlen(amount_temp))/2);
		memcpy( &d_Receipt.amount[number_of_zeros], amount_temp,
				strlen( amount_temp ) );
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
			strcpy( &d_Receipt.
					amount[number_of_zeros + strlen( amount_temp )], "YTL" );
		else
			strcpy( &d_Receipt.
					amount[number_of_zeros + strlen( amount_temp )], " TL" );
		d_Receipt.amount[24]=0;

		/* offline approval code for OFFLINE CANCEL*/
		memcpy(d_Receipt.approval_code, v_approval_code, 6);
		d_Receipt.approval_code[6] = 0;
	}

	return(TRUE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Offline_PrintReceipt
 *
 * DESCRIPTION		: Print the offline receipt structure
 *
 * RETURN			:
 *      
 * NOTES			: r.i.o.  26/02/2003
 *
 * ------------------------------------------------------------------------ */
void Offline_PrintReceipt (void)
{

	const byte aklogoPrint [] = { 
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFC, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF,
	 0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80,
	 0xE0, 0xF0, 0xF8, 0xFC, 0x3F, 0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x1F, 0xFF, 0xFE, 0xFC, 0xF8, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0xE0, 0xFC, 0xFF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
	 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0xF8, 0xFC,
	 0x3F, 0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF8, 0xFF, 0xFF, 0xFF, 0xCF, 0xC1, 0xC0, 0xC1, 0xCF,
	 0xFF, 0xFF, 0xFF, 0xF0, 0x80, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3F, 0x1F,
	 0x1F, 0x7F, 0xFF, 0xFC, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x3F, 0xFF, 0xFF, 0xF3, 0xE0, 0x00, 0x00, 0x00, 0x80,
	 0xF8, 0xFF, 0xFF, 0xFF, 0xCF, 0xC1, 0xC0, 0xC1, 0xCF, 0xFF, 0xFF, 0xFF, 0xF0, 0x80, 0x00, 0x00,
	 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x07, 0x3F, 0xFF, 0xFC, 0xF0, 0xC0, 0xFF, 0xFF, 0xFF,
	 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3F, 0x1F, 0x1F, 0x7F, 0xFF, 0xFC,
	 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x60, 0x7C, 0x7F, 0x7F, 0x3F, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	 0x03, 0x03, 0x3F, 0x7F, 0x7F, 0x7C, 0x60, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x01, 0x0F, 0x3F, 0x7F, 0x7E, 0x78, 0x60, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F,
	 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x7C, 0x7F, 0x3F, 0x1F, 0x07, 0x00, 0x60, 0x7C, 0x7F,
	 0x7F, 0x3F, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x3F, 0x7F, 0x7F, 0x7C, 0x60,
	 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0x7F,
	 0x7F, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0F,
	 0x3F, 0x7F, 0x7E, 0x78, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	char temp_approvalcode[7];

	Offline_PrintCommonReceipt();

	PrntUtil_Cr(1);
	
	/* PRINT THE AMOUNT */
	Slips_PrinterStr(d_Receipt.amount);
	PrntUtil_Cr(2);

	if (!d_Receipt.R_VOID) 
	{
		memcpy(msg,MSGSlipFixed1,24);
		msg[24]=0;
		Slips_PrinterStr(msg);
		PrntUtil_Cr(2); 
	}

	if (!SECOND_COPY)
	{
		memcpy(msg,MSGSlipFixed2,24);
		msg[24]=0;
		Slips_PrinterStr(msg);

		memcpy(msg,MSGSlipFixed4,24);
		msg[24]=0;
		Slips_PrinterStr(msg);

		PrntUtil_Cr(2); 
	}

	memset(temp_approvalcode, 0, 7);
	memcpy(temp_approvalcode, d_Receipt.approval_code, 6);
	temp_approvalcode[6] = 0;

	sprintf(msg, "PROV : %s 0000460000", temp_approvalcode);
	msg[24]=0;
	Slips_PrinterStr(msg);

	memcpy(msg,MSGSlipFixed3,24);
	msg[24]=0;
	Slips_PrinterStr(msg);

	/* logo basimi */
	if(SECOND_COPY || d_Receipt.R_VOID )
	{
		Utils_PrintImage((byte *)aklogoPrint, 0, 128, 5);
	}

	/* wait until completing the printing */
	Slips_WaitForPrinting ();
	PrntUtil_Cr(5); 
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Offline_PrintCommonReceipt
 *
 * DESCRIPTION                  : Print the receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 20/05/99
 *
 * ------------------------------------------------------------------------ */
void Offline_PrintCommonReceipt (void)
{

	char temp_local[24] ; 
	char * Tran_Types[] = { NULL, "O.SATIž", "ON PROV", "MANUEL", "˜ADE ", NULL, "O.˜PTAL", NULL};
	char masked_card_no[24];

	/* Print Merch Name ; Merch Adress1 ; Merch Adress2*/
	Slips_PrinterStr(merch_addr);

	/* Print Amex No: */
	if(AMEX_FLAG)
	{
		sprintf(msg, "AMEX NO: %s\n", merch_AMEX_no);
		msg[20]=0;
		Slips_PrinterStr(msg);
	}

	/* MERCHANT NAME */
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "˜žYER˜ : %s\n", merch_no);
	msg[24] = 0;
	Slips_PrinterStr(msg);

	/* terminal no 8 digit basiliyor */
	memset (msg, 0, sizeof(msg));
	merch_device_no[8] = 0;
	sprintf( msg,"CIHAZ NO: %s\n", merch_device_no);
	msg[24]=0;
	Slips_PrinterStr(msg);

	/* DATE , TIME and transaction type */
	sprintf(msg, "%s %s %s\n\n", d_Receipt.date, d_Receipt.time, Tran_Types[d_Receipt.tran_type]);
	msg[24] = 0;
	Slips_PrinterStr(msg);
	
	if(!SECOND_COPY)
	{
		/* formatted card no */
		Slips_PrinterStr(d_Receipt.card_no);
		Slips_PrinterStr("\n");
	}
	else
	{
		if( (TRACK1ERROR && (/*d_Receipt.input_type*/ myinput_type == 'D')) || 
			(CHIP_ERROR  && (/*d_Receipt.input_type*/ myinput_type == 'C')) 
		  )
		{	/* isim yaziliyor */
			memset(masked_card_no, 0, sizeof(masked_card_no));
			strcpy(masked_card_no, d_Receipt.card_no);
			if(AMEX_FLAG)
			{
				memcpy(masked_card_no+4, " **** ***", 9);
			}
			else
			{
				memcpy(masked_card_no+4, " **** ****", 10);
			}

			Slips_PrinterStr(masked_card_no);
			Slips_PrinterStr("\n");
		}
		else
		{	/* isim yazilmiyor */
			/* formatted card no */
			Slips_PrinterStr(d_Receipt.card_no);
			Slips_PrinterStr("\n");
		}

	}

	/** XLS_PAYMENT 19/07/2001 irfan. printing name and surname in case of chip **/
	/** XLS 04_02 19/12/2001 irfan. track 1 hatali oldugunda bir onceki baþarýlý iþelmin ismin basýlmasý engellendi **/
	if( (TRACK1ERROR && (/*d_Receipt.input_type*/ myinput_type == 'D')) || 
		(CHIP_ERROR  && (/*d_Receipt.input_type*/ myinput_type == 'C')) 
	  )
	{	
		Slips_PrinterStr(d_Receipt.Card_Holder_Name);
	}

	PrntUtil_Cr(2);	
	
	/** XLS_04_02 irfan. 06/12/2001. customer copy'den gecerlilik tarihi kalkiyor **/
	if(!SECOND_COPY)
	{	/* formatted exparation date */
		sprintf(msg, "GE€. TAR.: %s\n", d_Receipt.exp_date);
		msg[24] = 0;
		Slips_PrinterStr(msg);
	}

	
	/* ref no: batch_no_c + '/' +seq_no_c */
	sprintf(msg, "REF NO   : %s  (%c)\n\n", d_Receipt.ref_no, /*d_Receipt.input_type*/ myinput_type);
	msg[24] = 0;
	Slips_PrinterStr(msg);
	PrntUtil_Cr(1);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Offline_PrintDetailRecord
 *
 * DESCRIPTION		: Prints the details for Report
 *
 * RETURN			: None
 *      
 * NOTES			: r.i.o. 28/02/2003
 *
 * ------------------------------------------------------------------------ */
void Offline_PrintDetailRecord(void)
{
	char toprinter[25];
	char index;
	sint i, j, lbStatus, lwLastSeq;
	char input_card_no[20], output_card_no[25], 
	     output_exp_date[6], card_name[5],
		 temporary_string1[14], temporary_string2[9], temporary_string3[6];
	OfflineAkbBatch_FileStruct lbOfflineAkbBatch_FileStruct;
	//File_Entry        *fe;
	sint Handle;
	boolean display_transaction;
	char tempSTR[100];

	char * Tran_Types[] = { NULL, "SAT.", "ONP.", "MAN.", "IADE", NULL, NULL, NULL};

	/* Check AKBOFF File for offline transactions */
	if (RamDisk_ChkDsk((byte *)OfflineAkbBatch_File) != RD_CHECK_OK)
	{
		strcpy(toprinter, "OFFLINE YIGIN ACILAMADI\n");
		toprinter[20] = 0;
		Slips_PrinterStr(toprinter);
		PrntUtil_Cr(2);
		return;
	}

	Handle = RamDisk_Reopen((byte *)OfflineAkbBatch_File)->Handle;
	RamDisk_Seek(Handle, SEEK_FIRST);
	lwLastSeq = RamDisk_Seek(Handle,SEEK_LAST);

	/* test amacli */
	/*memset(tempSTR, 0, sizeof(tempSTR));
	sprintf(tempSTR, " \n lwLastSeq : %d \n", lwLastSeq);
	Slips_PrinterStr(tempSTR);*/

	if(lwLastSeq <= 0)
	{
		return; /* kayit yok ise hic bir sey basilmadan geri donulur */
	}

	Slips_PrinterStr(" OFFLINE SLIP BILGILERI ");
	Slips_PrinterStr("------------------------");
	PrntUtil_Cr(1);

	for (i=0; i < lwLastSeq; i++)   
	{
		if (RamDisk_Seek(Handle, i) >= 0)
		{
			lbStatus = RamDisk_Read(Handle, &lbOfflineAkbBatch_FileStruct);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
				display_transaction = TRUE;
			else
				display_transaction = FALSE;
		}
		else
			display_transaction = FALSE;

		if (display_transaction)
		{
			index = lbOfflineAkbBatch_FileStruct.tran_type - '0';

			memcpy(temporary_string1, lbOfflineAkbBatch_FileStruct.seq_no, 4);
			temporary_string1[4] = 0;
			memcpy(temporary_string2, lbOfflineAkbBatch_FileStruct.date, 8);
			temporary_string2[8] = 0;
			memcpy(temporary_string3, lbOfflineAkbBatch_FileStruct.time, 5);
			temporary_string3[5] = 0;

			sprintf(toprinter, "%s %s %s %s", temporary_string1, Tran_Types[index], temporary_string2, temporary_string3);
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			memcpy(input_card_no, lbOfflineAkbBatch_FileStruct.card_no, 19);
			input_card_no[19] = 0;

			for (j=12; j<19; j++)
			{
				if (input_card_no[j] == ' ')
					input_card_no[j] = 0;
			}

			Slips_FormatCardExp(output_card_no, output_exp_date, 
					    input_card_no,  "1111");
			sprintf(toprinter, "%s", output_card_no);
			toprinter[strlen(output_card_no)] = 0;
			Slips_PrinterStr(toprinter);
			PrntUtil_Cr(1);

			/* card tablosu iþlem sýrasýnda doldurulmadigi icin asagisi kapatildi */
			/*j = ((lbOfflineAkbBatch_FileStruct.card_type[0] - '0') * 10) +
			     (lbOfflineAkbBatch_FileStruct.card_type[1] - '0');

			if (j>0)
			{
				j--;
				memcpy(card_name, d_Card_Type_Table[j].short_name, 4);
			}
			else
				memset(card_name, ' ', 4);*/
			/* yerine dummy bir isim veriliyor */
			memcpy(card_name, "OFFL",4); /* bu kisim offline kisim icin yeni eklendi */


			card_name[4] = 0;

			memcpy(temporary_string1, lbOfflineAkbBatch_FileStruct.amount_surcharge, 13);
			temporary_string1[13] = 0;
			memcpy(temporary_string2, lbOfflineAkbBatch_FileStruct.approval_code, 6);
			temporary_string2[6] = 0;

			sprintf(toprinter, "%s%s %s", card_name, temporary_string2, temporary_string1);
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			if(lbOfflineAkbBatch_FileStruct.void_status == '1')
			{
				strcpy(toprinter, "         IPTAL\n");
				toprinter[15] = 0;
				Slips_PrinterStr(toprinter);
			}

			PrntUtil_Cr(2);
		}
	}

	PrntUtil_Cr(2);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Offline_GetOffTrxCounts
 *
 * DESCRIPTION	: Returns the offline transaction counter
 *
 * RETURN		: offline transaction counter 
 *
 * NOTES		: r.i.o. 03/03/2003       
 *
 * ------------------------------------------------------------------------ */
sint Offline_GetoffTrxCounts(void)
{
//	File_Entry *fe;
	sint Handle;
	sint lbOffTrxCount;
	char lbStr[100];	/* test amacli */

	Handle = RamDisk_Reopen((byte *)OfflineAkbBatch_File)->Handle;
	lbOffTrxCount = RamDisk_Seek(Handle, SEEK_LAST);

	/* test amacli */
	/*memset(lbStr, 0, sizeof(lbStr));
	sprintf(lbStr, "\nlbOffTrxCount: %d \n", lbOffTrxCount);
	Slips_PrinterStr(lbStr);*/

	if (lbOffTrxCount < 0) lbOffTrxCount = 0;
	return(lbOffTrxCount);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: Offline_BatchUpload
 *
 * DESCRIPTION		:
 *             
 *
 * RETURN			:      
 *
 * NOTES			:       
 *
 * ------------------------------------------------------------------------ */
byte Offline_BatchUpload(void)
{
	OfflineAkbBatch_FileStruct lbOfflineAkbBatch_FileStruct;
	struct Transaction_Receive lbTransaction_Receive;
	sint lwLastSeq, i;
	byte lbStatus, Transaction_Ret;
//	File_Entry *fe;
	char temp[30];
	char lbStr[900];
	char lbSend_Buffer[900];
	char testSTR[100];
	char lb_temp_STR[100];
	byte Ramdis_Ret;
	sint Handle;

	/* eger offline islem yok ise hic bir sey yapilmadan donulur */
	if(Offline_GetoffTrxCounts() <= 0)  
	{
		return(STAT_OK);
	}

	/* ekranda bilgilendirme satirlari */
	GrphText_Cls (TRUE);
	Debug_GrafPrint2("OFFLINE ISLEMLER", 2);
	Debug_GrafPrint2(" GONDERILIYOR.....", 3);

	if (RamDisk_ChkDsk((byte *)OfflineAkbBatch_File) != RD_CHECK_OK) return STAT_NOK;

	Handle = RamDisk_Reopen((byte *)OfflineAkbBatch_File)->Handle;

	RamDisk_Seek(Handle, SEEK_FIRST);
	lwLastSeq = RamDisk_Seek(Handle,SEEK_LAST);

	for (i = lwLastSeq - 1; i >= 0 ; i--)   
	{
		Ramdis_Ret = RamDisk_Seek(Handle, i);

		/* test amacli */
		/*memset(lb_temp_STR, 0, sizeof(lb_temp_STR));
		sprintf(lb_temp_STR, "\nRamDisk_Seek : %d \n", Ramdis_Ret);
		Slips_PrinterStr(lb_temp_STR);*/

		if (Ramdis_Ret >= 0)
		{
			lbStatus = RamDisk_Read(Handle, &lbOfflineAkbBatch_FileStruct);

			/* test amacli */
			/*memset(lb_temp_STR, 0, sizeof(lb_temp_STR));
			sprintf(lb_temp_STR, "\nRamDisk_Read : %d \n", lbStatus);
			Slips_PrinterStr(lb_temp_STR);*/

			if ((lbStatus >= 0) || (lbStatus == RD_LAST) )
			{
				if(lbOfflineAkbBatch_FileStruct.void_status != '1') /* iptal edilmiþ ise gonderilmeyecek ama kayit silinecek */
				{
					memset( lbSend_Buffer, 0, 900 );
//bm 15.12.2004 	memset(Received_Buffer,0,sizeof(Received_Buffer));
					memcpy (lbSend_Buffer , &lbOfflineAkbBatch_FileStruct.lb_Transaction_Send,sizeof(lbOfflineAkbBatch_FileStruct.lb_Transaction_Send));

					/* Bilgi amacli. Gonderilen islem no ekrana basiliyor */
					memset(testSTR, 0, sizeof(testSTR));
					memcpy(testSTR,lbOfflineAkbBatch_FileStruct.seq_no,4);
					testSTR[4] = 0;
					memset(lb_temp_STR, 0, sizeof(lb_temp_STR));
					sprintf(lb_temp_STR, " ISLEM NO: %s ", testSTR);
					Debug_GrafPrint2(lb_temp_STR, 4);

					if (BUYUKFIRMA) Transaction_Ret = Host_BFirmaSendReceive(lbSend_Buffer, TRUE, TRUE, TRUE, TRUE);
					else Transaction_Ret = Host_SendReceive(lbSend_Buffer, TRUE, TRUE, TRUE);
					if (Transaction_Ret != STAT_OK) return STAT_NOK;

//bm 15.12.2004 	memcpy(&lbTransaction_Receive,Received_Buffer+1,sizeof(lbTransaction_Receive));

					/* check received message field by filed */
					if(Offline_CheckReceivedMessage(&lbTransaction_Receive) != STAT_OK) return(STAT_NOK);
					
					if (memcmp(lbTransaction_Receive.confirmation, "000",3) != 0)
					{
						PrntUtil_Cr(5);
						Printer_WriteStr("**** OFFLINE TRX    ****");
						Printer_WriteStr("**** UPLOAD  HATASI ****");
						Printer_WriteStr("****");
						memset(temp,0,sizeof(temp));
						memcpy(temp,lbTransaction_Receive.message,sizeof(lbTransaction_Receive));
						Printer_WriteStr(temp);
						PrntUtil_Cr(5);

						GrphText_Cls (TRUE);
						Debug_GrafPrint2(" OFFLINE TRX    ", 2);
						Debug_GrafPrint2(" UPLOAD  HATASI ", 3);

						return(STAT_NOK);
					}
					else
					{
						/* update the card type_type_table and agreement_total */
						memset(lbStr, 0, sizeof(lbStr));
//bm 15.12.2004 		memcpy(lbStr,Received_Buffer+1,sizeof(lbTransaction_Receive));

						if(Offline_UpdateCardTypeTable(lbStr) != STAT_OK)
						{
							Slips_PrinterStr("\nOFFLINE PARAMETRE UPDATE HATASI\n");
							return(STAT_NOK);
						}
					}
				}

				Ramdis_Ret = RamDisk_ClearFile (Handle, i, i);

				if (Ramdis_Ret != RD_OK)
				{
					Printer_WriteStr("\nOffline DeleteError\n");
					return STAT_NOK;
				}
			}
		}
	}
	return STAT_OK;
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_FillTransactionSend
 *
 * DESCRIPTION          : 
 *
 * RETURN               :
 *
 * NOTES                :
 *
 * REVISED ON           : r.i.o. 03/03/03 
 * ------------------------------------------------------------------------ */
void Offline_FillTransactionSend(char *fpStr)
{
	char lboffline_approval_code[7];
	struct Transaction_Send	lb_Transaction_Send;
	char device_date[8], device_time[6];

	memset(&lb_Transaction_Send,' ',sizeof(lb_Transaction_Send));

// bm OFF 21.10.2004

	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		lb_Transaction_Send.Header_Transact.Cur_Index = 'X';
	else
		lb_Transaction_Send.Header_Transact.Cur_Index = 'Y';
	lb_Transaction_Send.Header_Transact.ActAppCode = POS_Type;
	lb_Transaction_Send.Header_Transact.SecAppCode = '0';	/* 0-nobiz, 1-bizpos */
	memcpy( lb_Transaction_Send.Header_Transact.PosType, "LIP", 3 );
	memcpy( lb_Transaction_Send.Header_Transact.PosModel, "8000", 4 );
	memcpy( lb_Transaction_Send.Header_Transact.PosVersion, POS_VERSION_NUMBER, 4 );
	if (Cashier_Flag == 2)  /* Kasiyer tanýmlý deðil 13.08.2004*/ 
		memset( lb_Transaction_Send.Header_Transact.Cashier_Id, ' ', 16 );
	else
		memcpy( lb_Transaction_Send.Header_Transact.Cashier_Id, Kasiyer_No, 16 );
	memcpy( lb_Transaction_Send.Header_Transact.IMEI_ID, Kampanya_Data, 15 );
	memset( lb_Transaction_Send.Header_Transact.Filler, ' ', 19 );
	Utils_GetDateTime( device_date, device_time );
	lb_Transaction_Send.Header_Transact.PosDate[0] = device_date[6];
	lb_Transaction_Send.Header_Transact.PosDate[1] = device_date[7];
	lb_Transaction_Send.Header_Transact.PosDate[2] = device_date[3];
	lb_Transaction_Send.Header_Transact.PosDate[3] = device_date[4];
	lb_Transaction_Send.Header_Transact.PosDate[4] = device_date[0];
	lb_Transaction_Send.Header_Transact.PosDate[5] = device_date[1];
	lb_Transaction_Send.Header_Transact.PosTime[0] = device_time[0];
	lb_Transaction_Send.Header_Transact.PosTime[1] = device_time[1];
	lb_Transaction_Send.Header_Transact.PosTime[2] = device_time[3];
	lb_Transaction_Send.Header_Transact.PosTime[3] = device_time[4];

// bm OFF 2.10.2004
	/* FILL  THE  HEADER  PART */
	lb_Transaction_Send.Header_Transact.MSG_INDICATOR ='B';

	lb_Transaction_Send.Header_Transact.MSG_TYPE='2';
	lb_Transaction_Send.Header_Transact.POS_software_ver = POS_software_ver;
	memcpy(lb_Transaction_Send.Header_Transact.merch_device_no , merch_device_no , 8);
	lb_Transaction_Send.Header_Transact.device_serial_no = device_serial_no;
	
	lb_Transaction_Send.Header_Transact.transaction_type = 1 + '0';

	/*FILL THE DETAIL */
	Utils_IToA(batch_no,batch_no_c);
	Utils_LeftPad(batch_no_c, '0', 3);
	memcpy(lb_Transaction_Send.batch_no, batch_no_c,3);

	Utils_IToA(seq_no,seq_no_c);
	Utils_LeftPad(seq_no_c, '0', 4);
	memcpy(lb_Transaction_Send.transaction_seq_no, seq_no_c,4);

	memset(lb_Transaction_Send.card_info, ' ', sizeof(lb_Transaction_Send.card_info));
	lb_Transaction_Send.input_type = input_type;
	
	if (lb_Transaction_Send.input_type == INPUT_FROM_READER)  
	{
		memcpy(lb_Transaction_Send.card_info,card_info,strlen(card_info));
	}
	else if (lb_Transaction_Send.input_type == INPUT_FROM_CHIPCARD)
	{
		memcpy(lb_Transaction_Send.card_info, card_no,strlen(card_no));
		lb_Transaction_Send.card_info[strlen(card_no)] = '=';
		memcpy(&lb_Transaction_Send.card_info[strlen(card_no)+1], remained_chip_data,strlen(remained_chip_data));
	}
	else /* manual entry */
	{
			memcpy(lb_Transaction_Send.card_info, card_no,strlen(card_no));
			memcpy(&lb_Transaction_Send.card_info[19], exp_date,4);
			memcpy(&lb_Transaction_Send.card_info[23], cvv_2,3);
	}

	memset(lb_Transaction_Send.encrypted_PIN, '0', sizeof(lb_Transaction_Send.encrypted_PIN));

	/* offline approval code for OFFLINE SALE*/
	memset(lboffline_approval_code, 0, sizeof(lboffline_approval_code));
	memcpy(lboffline_approval_code, batch_no_c,3);
	memcpy(lboffline_approval_code+3,seq_no_c+1,3);
	lboffline_approval_code[6] = 0;

	memcpy(lb_Transaction_Send.approval_code, lboffline_approval_code, 6);

	memcpy(lb_Transaction_Send.amount_surcharge, amount_surcharge, 13);

	memcpy(lb_Transaction_Send.last_transact_batch_no, last_transact_batch_no, 3);
	memcpy(lb_Transaction_Send.last_transact_seq_no, last_transact_seq_no, 4);
	lb_Transaction_Send.last_transact_status =  '1';

	lb_Transaction_Send.PINPad_status = '2';       

	/* copy agreement total */
	memcpy(lb_Transaction_Send.agreement_total, "0000000000000", 13);

	/* copy the ATS_Count */
	memcpy(lb_Transaction_Send.ATS_Count, "01", 2);

	if (input_type == INPUT_FROM_KEYPAD) 
	{
		lb_Transaction_Send.input_type = '1';
	}
	else if (input_type == INPUT_FROM_READER)
	{
		lb_Transaction_Send.input_type = '2';
	}
	else if (input_type == INPUT_FROM_CHIPCARD)
	{
		lb_Transaction_Send.input_type = '3';
	}

	/* move prepared message to the buffer to be sent back where this fucntion is called */
	memcpy(fpStr, &lb_Transaction_Send, (sizeof(lb_Transaction_Send) - sizeof(lb_Transaction_Send.EMVFields)) );

	/* test amacli  */
	/*Printer_WriteStr("fpStr:\n");
	PrntUtil_BufAsciiHex((byte *)fpStr,( sizeof(lb_Transaction_Send)-sizeof(lb_Transaction_Send.EMVFields) ) );*/
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_UpdateCardTypeTable
 *
 * DESCRIPTION          : Updates the Card Type Table for offline transaction in upload step
 *
 * RETURN               : STAT_OK, STAT_NOK
 *
 * NOTES                : r.i.o 03/03/2003
 *
 * ------------------------------------------------------------------------ */
byte Offline_UpdateCardTypeTable(char *lbStr)
{
	struct Transaction_Receive lbTransaction_Receive;

	sint received_card_type, tran_index;
	char received_amount[14];
	char result_amount[14];
	char temporary_amount[13];

	memcpy(&lbTransaction_Receive, lbStr, sizeof(lbTransaction_Receive));
	
	received_card_type = ((lbTransaction_Receive.card_type[0] - '0') * 10) + (lbTransaction_Receive.card_type[1] - '0');
/*	if (received_card_type > 15) received_card_type = 16;*/
	if (received_card_type > (CARDTYPETABLESIZEMINUSONE)) received_card_type = CARDTYPETABLESIZE;
	received_card_type--;
	d_Card_Type_Table[received_card_type].transaction_occured = TRUE;
 
	memcpy(d_Card_Type_Table[received_card_type].long_name, lbTransaction_Receive.card_type_long_name, 18);
	d_Card_Type_Table[received_card_type].long_name[18] = 0;
	memcpy(d_Card_Type_Table[received_card_type].short_name, lbTransaction_Receive.card_type_short_name, 6);
	d_Card_Type_Table[received_card_type].short_name[6] = 0;
	memcpy(received_amount, lbTransaction_Receive.amount_surcharge, 13);
	received_amount[13] = 0;

	/*tran_index = tran_type - 1; sadece satiþ olacagindan direkt sifir yapildi*/ 
	tran_index = 0;

	if (tran_index > 3) return(STAT_NOK);
	Utils_Add_Strings(received_amount, d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_amount, result_amount);
	Utils_LeftPad(result_amount, '0', 13);
	memcpy(d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_amount, result_amount,13);
	d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_amount[13] = 0;

	d_Card_Type_Table[received_card_type].d_Accummulators[tran_index].tran_count++;
	Utils_Add_Strings(received_amount, d_Totals_Counts[tran_index].tran_amount, result_amount);
	Utils_LeftPad(result_amount, '0', 13);
	memcpy(d_Totals_Counts[tran_index].tran_amount, result_amount,13);
	d_Totals_Counts[tran_index].tran_amount[13] = 0;

	d_Totals_Counts[tran_index].tran_count++;

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

	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_CheckReceivedMessage
 *
 * DESCRIPTION          : Check the received message
 *
 * RETURN               : STAT_OK, STAT_NOK
 *
 * NOTES                : r.i.o 05/03/2003
 *
 * ------------------------------------------------------------------------ */
byte Offline_CheckReceivedMessage(struct Transaction_Receive *lbTransaction_Receive)
{
	char lbtemp_string[100];
	
	/* check for host confirmation */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, lbTransaction_Receive->confirmation, 3);
/*
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\nconfirmation:");
		Slips_PrinterStr(lbtemp_string);
	}
*/
	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("\nconfirmation HATASI\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}

	/* check for host amount_surcharge */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, lbTransaction_Receive->amount_surcharge, 13);
/*
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\namount_surcharge:");
		Slips_PrinterStr(lbtemp_string);
	}
*/
	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("\namount_surcharge HATASI\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}

	/* check for host date */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, lbTransaction_Receive->date, 6);
/*
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\ndate:");
		Slips_PrinterStr(lbtemp_string);
	}
*/
	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("\ndate HATASI\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}

	/* check for host time */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, lbTransaction_Receive->time, 4);
/*
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\ntime:");
		Slips_PrinterStr(lbtemp_string);
	}
*/
	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("\ntime HATASI\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}

	/* check for host transact_seq_no */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, lbTransaction_Receive->transact_seq_no, 4);
/*
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\ntransact_seq_no:");
		Slips_PrinterStr(lbtemp_string);
	}
*/
	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("\ntransact_seq_no HATASI\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}

	/* check for host card_type */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, lbTransaction_Receive->card_type, 2);
/*
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\ncard_type:");
		Slips_PrinterStr(lbtemp_string);
	}
*/
	if(Utils_NumericCheck(lbtemp_string, strlen(lbtemp_string))!=STAT_OK)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("***BOZUK MESAJ ALINDI***\n");
		Slips_PrinterStr("\ncard_type HATASI\n");
		PrntUtil_Cr(7);
		GrphText_Cls (TRUE);
		Mainloop_DisplayWait("BOZUK MSJ ALINDI",1);
		return (STAT_NOK);
	}

	/* check for host generated cash back */
	memset(lbtemp_string,0,sizeof(lbtemp_string));
	memcpy(lbtemp_string, lbTransaction_Receive->HostGeneratedCashback, 13);
/*
	if(DEBUG_MODE)
	{
		Slips_PrinterStr("\nHostGeneratedCashback:");
		Slips_PrinterStr(lbtemp_string);
	}
*/
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
	return ( STAT_OK );
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Offline_BIN_Add_TABLE
 *
 * DESCRIPTION          : 
 *
 * RETURN               : STAT_OK, STAT_NOK
 *
 * NOTES                : bm OFF 21.10.2004 offline bin dosyasý oluþturma ve BIN ekleme 
 * ------------------------------------------------------------------------ */
byte Offline_BIN_Add_TABLE( void )
{
	OfflineBinStruct FPOfflineBinStruct;
	char temp_str_offline[50];
	sint lbLen;
	char *p, *s;
	int i;
	int len;
	char temp[800];

	/* get the length of the received data */
	memset( temp_str_offline, 0, sizeof( temp_str_offline ) );
	memcpy( temp_str_offline, d_Param_Continue_Receive.param_data_length, 3 );
	temp_str_offline[3] = 0;
	lbLen = atoi( temp_str_offline );
	memcpy( temp, d_Param_Continue_Receive.param_data, lbLen );

	for ( i = 0, s = temp, p = temp; ( *( s + 1 ) != '+' ) && ( *( s + 1 ) != '*' ); p = ++s )	/* s ve p de temp den oce & kaldirildi 09/08/2001 */
	{
		*( s = strchr( s, ',' ) ) = 0;
		if ( ( *p > '9' ) || ( *p < '0' ) )
			break;
		/* strcpy( debit_prefix[i], p ); */
		memset( FPOfflineBinStruct.OfflineBIN, 0, 6 );
		memcpy( FPOfflineBinStruct.OfflineBIN, p, 6 );

		/* write the filled record to the AKBANK batch */
		if ( Offline_Add_BIN( FPOfflineBinStruct.OfflineBIN ) != STAT_OK )
		{
			Slips_PrinterStr( "\n\OFFLINE BIN YAZILAMADI\n\n" );
			Mainloop_DisplayWait( "OFFLINE BIN.TABLE  YAZILAMADI   ", 1 );
			return ( STAT_NOK );
		}
		/*  Printer_WriteStr("FPOfflineBinStruct.OfflineBIN :\n");
				PrntUtil_BufAsciiHex((byte *)&FPOfflineBinStruct.OfflineBIN, 6);	*/
		i++;
	}

	return ( STAT_OK );

}
