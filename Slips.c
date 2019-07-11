/*************************************************************************
  FILE NAME		: MAINLOOP.C
  MODULE NAME	:
  PROGRAMMER	:  
  DESCRIPTION	: Main Loop
  REVISION		: 01.00
**************************************************************************/

/*=========================================*
*         I N T R O D U C T I O N          *
*==========================================*/
/*=========================================*
*             I N C L U D E S              *
*==========================================*/
/* General Header File */
/*#include "config.h"*/

#include "project.def"

/* MCC68K Header Files */
/*UPDATE 01_16 26/10/99 sprintf gibi komutlarýn prototype'ý için konuldu*/
/*#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <mriext.h>
#include NOS_H*/

/** XLS_PAYMENT 25/07/2001 irfan **/
#include INTERFAC_H
#include DEFINE_H
#include ERRORS_H
#include UTILS_H
#include MESSAGE_H
#include MAINLOOP_H
#include FILES_H
#include SLIPS_H
#include TRANS_H
#include EMVIF_H
#include DEBUG_H

/* Booleans */
boolean SECOND_COPY;
boolean error_printed;
char msg[25];
char amount_faiz_kismi[14];		/* faizli islemde faiz kismini icerir */
char amount_surcharge_kismi[14];	/* surcharge islemde surcharge kismini icerir */
char amount_Iade[14]; /* 13.08.2004 */
//extern char Kasiyer_No[17];				/*bm YTL 13.08.2004 */
extern sint Cashier_Flag;				/*bm YTL 13.08.2004 Cashier menu */
extern boolean TL_Iade;					/*bm YTL 13.08.2004 */
extern boolean DEBIT_FLAG;
extern boolean VOID_FLAG;
extern boolean AMEX_FLAG;
/* structure variables */
extern struct Receipt_Structure         d_Receipt;
extern struct Transaction_Receive       d_Transaction_Receive;
extern struct Transaction_Send          d_Transaction_Send;
extern struct Message_Header            d_Message_Header;
extern struct EOD_Receive                       d_EOD_Receive;
extern struct Card_Type_Table d_Card_Type_Table[CARDTYPETABLESIZE];
extern boolean EOD_flag;
extern char device_serial_no;   
                        
/* variables */
char msgSlips[25];
char myinput_type;


extern char merch_device_no[9]; 		/* merchant device no */
extern char merch_no[13];               /* merchant no */
extern char merch_addr[73];             /* merch name , merch addr1+ merchaddr2 */
extern char merch_AMEX_no[11];			/* merchant AMEX NO */
extern int seq_no,batch_no;
extern char v_seq_no_c[5], seq_no_c[5], batch_no_c[4];
extern char amount_surcharge[14];
extern char input_type;
extern int  tran_type;
extern char card_no[20];
extern char exp_date[5];
extern char net_total[14];                                      
extern char v_approval_code[7];
extern char v_amount_surcharge[14];
extern const char Transaction_File[];
extern const char ATS_File[];
extern boolean INSTALLATION;

/* ATS Related Variables */
extern int ATS_Count;
extern char ATS_Count_c[3];
extern boolean ATS_FLAG; 

/* 01_18 irfan 27/06/2000 card track 1 datasý icin tanýmlandý*/
extern char card_info1_name[24];
extern char card_info1_surname[24];

/* UPDATE_01_18 28/06/00 ISIM YAZMA FLAG TANIMI*/
extern boolean ISIMYAZMA_FLAG;

/* FAIZLI 02_00 25/07/2000 TRUE = Faizli iþlem FALSE = Faizli iþlem deðil*/
extern boolean FAIZLI_FLAG;
extern boolean TRACK1ERROR;
/** XLS_PAYMENT 19/07/2001 irfan related variables **/
extern boolean CHIP_ERROR;		/* TRUE : correct reading , FALSE : incorrect reading. */
extern boolean LOYALTY_PERMISSION_FLAG;
extern boolean RECOPY_FLAG;		/* TRUE : ikinci kopya basilacak FALSE : ikinci kopya basilmayacak */
extern boolean OFFLINE_TRX;		/* TRUE	: Offline Trx. FALSE : Online Trx. */
extern boolean CallFlag;
extern int POSEntryFlag;

/*ara rapor iptal */
extern boolean ARA_RAPOR_IPTAL;
extern boolean ARA_RAPOR;	/* TRUE:ara rapor transactioninin cagrildigini gosterir*/

/* genius 04_00 */
extern int Gtran_type;
extern char mev_hes_kodu;
extern char genius_seq_no_c[5];
extern int genius_seq_no;
extern char v_genius_seq_no_c[5];
extern char genius_flag;

/* 04_15 10/06/2003 */
extern char net_total_GT1_amt[14];
extern char net_total_GT2_amt[14];
extern char rev_total_GT1_amt[14];
extern char rev_total_GT2_amt[14];

extern boolean EMVAxess;
extern boolean DEBUG_MODE;
extern char encrypted_PIN[17];
extern char kampanya_str[15];
extern char YTL_Slip[3];		/* bm YTL 25/06/2004 TLY - YTL */
extern char POS_Type;
extern char amount_surcharge_iade[17]; /* 14 idi 17 yapýldý. 04.09.04 */
extern boolean KampanyaFlag; /*@bm 20.09.2004 */
extern char Kampanya_Data[15]; 		/*@bm 21.09.2004 */
extern char TL_Amount[25];				/* bm YTL temp area to format amount values */
/*	extern char MyTL_Amount[40];
	extern char MyMyTL_Amount[40];
*/
extern boolean BizSlip; //@bm  15.12.2004
boolean Kopya_ATS_FLAG;			/*@bm 14.09.2004 */
int Kopya_ATS_Count;
boolean	Kopya_LOYALTY_PERMISSION_FLAG;
boolean	Kopya_EMVAxess;
boolean Kopya_IsEMV;
boolean Kopya_FAIZLI_FLAG;
char Kopya_ATS_Count_c[3]; /*@bm 15.09.2004 */ 
byte PanLength;					/* @bm slip 22.09.2004*/
char Kopya_card_no[20];			/* @bm slip 22.09.2004*/
char OldAID[24];
char OldAPPL[24];
int OldAIDLen;
int OldAPPLLen;

/*==========================================*
*           D E F I N I T I O N S          *
*==========================================*/
/*==========================================*
*       P R I V A T E  T Y P E S           *
*==========================================*/
/*==========================================*
*        P R I V A T E  D A T A            *
*==========================================*/
/*==========================================*
*     PRIVATE  FUNCTION   PROTOTYPES       *
*==========================================*/
/*==========================================*
*    P U B L I C     F U N C T I O N S     *
*==========================================*/


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Slips_FormatAmount
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
void Slips_FormatAmount(char *fpAmount, char *fpFJAmount)
{
	char lpAmount [LEN_AMOUNT+1];
	sint Counter;
	boolean CommaType;

	memcpy(lpAmount,fpAmount,LEN_AMOUNT);
	lpAmount[LEN_AMOUNT]= 0;
	Utils_ClearInsZeros(lpAmount);
	for (Counter=0;Counter<(LEN_AMOUNT+1);Counter++)
		if (!isdigit(lpAmount[Counter])) 
		{
			lpAmount[Counter]=0;
			break;
		}
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		CommaType = TRUE;
	else if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		CommaType = FALSE;
	Utils_WithComma( lpAmount, fpFJAmount, Counter, CommaType );
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: No Paper Warning
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
void Slips_Printer (char *buf, usint length)
{
	if ( length == 0 )
		return;
   for(;;)
   {
      if( Printer_Write (buf, length) > 0)
	return;      
       Mte_Wait (60);
       Kb_Read ();
   }
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: No Paper Warning
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
void Slips_PrinterStr (char *buf)
{

	if ( buf[0] == 0 )
		return;
  for(;;)
   {
      if( Printer_WriteStr (buf) > 0)
	 return;      
      Mte_Wait (60);
      Kb_Read ();
   }

}
void Slips_WaitForPrinting (void)
{
    PrntUtil_WaitForPrinting();
    for (;;)
     if (Kb_Read () == 0)
       break;
}


/*****************************************************************
 *************************  our functions ************************
 *****************************************************************/

 /* --------------------------------------------------------------------------
  * *
  * * FUNCTION NAME                : Slips_FillCommanReceiptStructure.
  * *
  * * DESCRIPTION                  : Fill the common receipt structure
  * *
  * * RETURN                               :
  * *      
  * * NOTES                                : r.i.o. & t.c.a. 19/05/99
  * *
  * * ------------------------------------------------------------------------ */
boolean Slips_FillCommonReceiptStructure( boolean IsEMV,
										  PTStruct * fpPTStruct,
										  boolean Kampanya )
{
	int number_of_zeros, Status;

	char amount_temp[50];           /* temp area to format amount values */

	char received_amount[28];                       /*received amount_surcharge as char*/
	char received_amount_temp[28];	/*received amount_surcharge as char */
	char surcharge[28];                                     /* surcharge = received_amount - send_amount*/
	char TL_amount_temp[50];	/* bm YTL temp area to format amount values */
	char Amt_TMP[40];			/* bm YTL temp area to format amount values */

	char formatted_card_no[48];
	char formatted_exp_date[12];
	char device_date[16], device_time[10];
/* Toygar EMVed */
	char received_ATS_Amount[28];        /* ATS related amount */
/* Toygar EMVed */
/* @bm opt 30.08.2004	int i;			 /* ATS related variables */

	char temp_name[50];			/* 02_02 irfan 15/08/2000  isim ve soyismi tutuyor */
	char temp_Amount_Iade[28];	/* @bm 24.08.2004 */

/* @bm opt 30.08.2004	char temp_year[3], temp_month[3]; */
/* @bm opt 30.08.2004	int int_temp_year, int_temp_month; */

	memset(&d_Receipt, ' ', sizeof(d_Receipt));
	/* FAIZLI 02_00 25/07/2000 initial deger atýlýyor */
	memset(amount_faiz_kismi, 0 , sizeof(amount_faiz_kismi));
	strcpy(amount_faiz_kismi, "0000000000000");
	amount_faiz_kismi[13] = 0;

	/* 07_15 25/11/2002 initial deger atýlýyor */
	memset(amount_surcharge_kismi, 0 , sizeof(amount_surcharge_kismi));
	strcpy(amount_surcharge_kismi, "0000000000000");
	amount_surcharge_kismi[13] = 0;

	/* fill d_Receipt structure by space and initialize flags */
	memset(&d_Receipt, ' ', sizeof(d_Receipt));
	d_Receipt.SURCHARGE=FALSE;              /* default FALSE */
	d_Receipt.R_VOID=FALSE;                 /* default FALSE */

	/* move the current transaction type (for void. it is different)*/
	if (!VOID_FLAG)
	{
		d_Receipt.tran_type = tran_type;
		d_Transaction_Send.input_type = input_type;
	}
	else
	{
		d_Receipt.tran_type = 6;
		d_Receipt.SURCHARGE=FALSE;
		d_Transaction_Send.input_type = input_type;
		SECOND_COPY = TRUE;
	}

	d_Receipt.R_VOID = VOID_FLAG;

	/* GRAFPOS irfan 08/02/2001 Terminal no 8 digiti olarak basilacak */
/*	memcpy(d_Receipt.term_no, &merch_device_no[6], 2);
	d_Receipt.term_no[2] = 0;*/
	memcpy(d_Receipt.term_no, merch_device_no, 8);
	d_Receipt.term_no[8] = 0;

	/* get date and time and format them */

	Utils_GetDateTime(device_date,device_time);

	memcpy(d_Receipt.date,device_date,8);
	d_Receipt.date[8]=0;

	memcpy(d_Receipt.time,device_time,5);
	d_Receipt.time[5] = 0;

	/* Formatting and filling the card no and exp date */
	Slips_FormatCardExp(formatted_card_no, formatted_exp_date, card_no, exp_date);
	memcpy(d_Receipt.card_no, formatted_card_no, strlen(formatted_card_no));
	d_Receipt.card_no[strlen(formatted_card_no)] = 0;

	memcpy( d_Receipt.exp_date, formatted_exp_date,
			strlen( formatted_exp_date ) );
	d_Receipt.exp_date[strlen(formatted_exp_date)] = 0;
	
	if (d_Transaction_Send.input_type == '1' ) /*d_Receipt.input_type*/ myinput_type = 'T';
	else if  (d_Transaction_Send.input_type == '2' ) /*d_Receipt.input_type*/ myinput_type = 'D';
	else if  (d_Transaction_Send.input_type == '3' ) /*d_Receipt.input_type*/ myinput_type = 'C';
	else if  (d_Transaction_Send.input_type == '4' ) /*d_Receipt.input_type*/ myinput_type = 'E';
	else if  (d_Transaction_Send.input_type == '5' ) /*d_Receipt.input_type*/ myinput_type = 'B';
	else if  (d_Transaction_Send.input_type == '6' ) /*d_Receipt.input_type*/ myinput_type = 'F';
	else
	{
		Mainloop_DisplayWait("G˜R˜ž T˜P˜ HATASI",1);
		/*d_Receipt.input_type*/ myinput_type = 'X';
	}

	memcpy(d_Receipt.ref_no,batch_no_c,3);
	d_Receipt.ref_no[3]='/';

	if(!VOID_FLAG)
	{
		memcpy(&d_Receipt.ref_no[4],seq_no_c,4);
		d_Receipt.ref_no[8]=0;
	}
	else
	{
		memcpy(&d_Receipt.ref_no[4],v_seq_no_c,4);
		d_Receipt.ref_no[8]=0;
	}
	memset( msgSlips, ' ', sizeof( msgSlips ) );
	if( KampanyaFlag == TRUE ) 
	{
		Kampanya = TRUE;
		memset( d_Receipt.kampanya_str, ' ', sizeof( d_Receipt.kampanya_str ) );
		memcpy( d_Receipt.kampanya_str, kampanya_str, strlen( kampanya_str ) );
		d_Receipt.kampanya_str[strlen( kampanya_str )] = ':';
		memcpy( d_Receipt.kampanya_str + strlen( kampanya_str ) + 1,
				Kampanya_Data, sizeof( Kampanya_Data ) );

		memset( msgSlips, ' ', sizeof( msgSlips ) );
		memcpy( msgSlips, d_Receipt.kampanya_str , sizeof( d_Receipt.kampanya_str ) );
/* @bm
	 	Printer_WriteStr("!d_Receipt.kampanya_str:\n");
		PrntUtil_BufAsciiHex((byte *)d_Receipt.kampanya_str,sizeof(d_Receipt.kampanya_str));
		Printer_WriteStr("!msgSlips:\n");
		PrntUtil_BufAsciiHex((byte *)msgSlips,sizeof(msgSlips));
*/
	}
/* Toygar EMVed */
	if (IsEMV)
	{
		RowData   rdData;

		/* Adding AID */
		EmvDb_Find(&rdData, tagAID_TERMINAL);
		EmvIF_BinToAscii( rdData.value, d_Receipt.AID, rdData.length, 16,
						  FALSE );
		d_Receipt.AIDLen = rdData.length * 2;
/*
		EmvIF_BinToAscii(fpPTStruct->AID, d_Receipt.AID, 16, sizeof(d_Receipt.AID), FALSE);
		d_Receipt.AIDLen = fpPTStruct->AIDLen * 2;
*/
		 /* if ( DEBUG_MODE )
		{
			Printer_WriteStr( "AID:\n" );
			PrntUtil_BufAsciiHex( ( byte * ) d_Receipt.AID,
								  d_Receipt.AIDLen );
		}
*/
	}
/* Toygar EMVed */
	/* 02_02 irfan 15/09/2000 2. kopya icin basýlan bilgierin kaybolmamasýný saglar */
	/** XLS_PAYMENT 19/07/2001 irfan. printing name and surname in case of chip **/

	/** XLS 04_02. 19/12/2001 irfan.track 1 hatalý olduðunda bir onceki islemin ismi basýlýyordu. engellendi**/
	if ( ( TRACK1ERROR && ( /*d_Receipt.input_type*/ myinput_type == 'D' ) )
		 || ( CHIP_ERROR && ( /*d_Receipt.input_type*/ myinput_type == 'C' ) ) ||
		 ( ( /*d_Receipt.input_type*/ myinput_type == 'E' ) ) ||
/*		(ISIMYAZMA_FLAG && (myinput_type == 'T'))  ||*/
		 ( ( /*d_Receipt.input_type*/ myinput_type == 'B' ) ) ||
		 ( ( /*d_Receipt.input_type*/ myinput_type == 'F' ) ) )
	{
		memset(temp_name, 0 , sizeof(temp_name));
		sprintf(temp_name, "%s %s", card_info1_name, card_info1_surname);
		temp_name[strlen( card_info1_name ) + strlen( card_info1_surname ) +
				  1] = 0;
		memcpy( d_Receipt.Card_Holder_Name, temp_name, strlen( temp_name ) );
		d_Receipt.Card_Holder_Name[24] = 0;	/* ends with null */
	}
	else
	{
		/* initialize the card holder name */
		memset( d_Receipt.Card_Holder_Name, 0, 24 );
	}
	
	if(!VOID_FLAG)
	{
		/* check if surcharge */
		/** XLS_PAYMENT 01/08/2001 irfan **/
/* Toygar EMVed */
/*if(OFFLINE_TRX || !CallFlag) *//* 06_30 06/03/2003 kapatildi ve */
		if ( !CallFlag )		/* 06_30 06/03/2003 OFFLINE_TRX kaldiriliyor */
/* Toygar EMVed */
		{
			memcpy( received_amount, amount_surcharge, 13 );
		}
		else
		{
			memcpy( received_amount, d_Transaction_Receive.amount_surcharge,13 );
		}
		received_amount[13]=0;
		Utils_LeftPad(received_amount,'0',13);
		Utils_LeftPad(amount_surcharge,'0',13);

		if (TL_Iade == TRUE) /*@bm 24.08.2004 */
		{
			memset( temp_Amount_Iade, 0, sizeof( temp_Amount_Iade ) );
			memcpy( temp_Amount_Iade, amount_surcharge,13);
			memcpy( amount_surcharge, amount_surcharge_iade,13);
		}

		strcpy(received_amount_temp,received_amount);
		Status = Utils_Compare_Strings(received_amount, amount_surcharge);

		/* approved message and in case of comission */
		/* ----------------------------------------- */

		if ( Status == 1 )                              /* received_amount > amount_surcharge */
		{
			/* FAIZLI 02_00 25/07/2000 eðer taksit sayýsý 1 den buyuk ise iþlem faizli demektir */
			if ( ATS_Count > 1 )
				FAIZLI_FLAG = TRUE;

			/*set SURCHARGE flag to TRUE */
			d_Receipt.SURCHARGE = TRUE;
/*
			Printer_WriteStr( "received_amount:\n" );
			PrntUtil_BufAsciiHex( ( byte * ) received_amount, 13 );
			Printer_WriteStr( "received_amount_temp:\n" );
			PrntUtil_BufAsciiHex( ( byte * ) received_amount_temp, 13 );
			Printer_WriteStr( "amount_surcharge:\n" );
			PrntUtil_BufAsciiHex( ( byte * ) amount_surcharge, 13 );
*/
			/*@bm 04.09.2004 */
			if ( ( memcmp( YTL_Slip, "TLE", 3 ) == 0 ) )
			{
				memset( TL_Amount, ' ', sizeof( TL_amount_temp ) );
				memset( TL_amount_temp, ' ', sizeof( TL_Amount ) );
				memset( Amt_TMP, 0, sizeof( Amt_TMP ) );
				strcpy( Amt_TMP, "00000000000000000000" );
				Amt_TMP[20] = 0;

/*				if (TL_Iade == TRUE)
					memcpy(&Amt_TMP[4], &temp_Amount_Iade[0],13);
				else*/
				    memcpy( &Amt_TMP[0], &received_amount[0], 13 );
				Amt_TMP[20] = 0;
				Slips_FormatAmountTL( Amt_TMP, TL_amount_temp );
				number_of_zeros = ( int ) ( ( 21 - strlen( TL_amount_temp ) ) / 2 );
				memcpy( &TL_Amount[number_of_zeros], TL_amount_temp, strlen( TL_amount_temp ) );
				strcpy( &TL_Amount [number_of_zeros + strlen( TL_amount_temp )]," TL" );
			}
			TL_Iade = FALSE;
			/*@bm 04.09.2004 */

			/* entered amount by keypad or sent amount */
			Slips_FormatAmount(amount_surcharge,amount_temp); /*amount_surcharge is the sent_amount*/
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
				strcpy( d_Receipt.amount, "SAT:                 YTL" );
			else
			        strcpy(d_Receipt.amount,"SAT:                  TL");

			memcpy( &d_Receipt.amount[21 - strlen( amount_temp )],
					amount_temp, strlen( amount_temp ) );
			d_Receipt.amount[24]=0;

/* calculate surcharge *//* 07_15 25/11/2002 */
			memset(surcharge, 0,sizeof(surcharge));
			Utils_Subtract_Strings( received_amount_temp, amount_surcharge,	surcharge );
			Utils_LeftPad(surcharge,'0',13);
			surcharge[13] = 0;
/*
			Printer_WriteStr( "surcharge:\n" );
			PrntUtil_BufAsciiHex( surcharge, 13 );
*/

			/* FAIZLI 02_00 25/07/2000 faizli kisim hesaplaniyor */
			if(FAIZLI_FLAG)
			{
				memset(amount_faiz_kismi, 0 , sizeof(amount_faiz_kismi));
				memcpy(amount_faiz_kismi, surcharge, 13);
				Utils_LeftPad(amount_faiz_kismi,'0',13);
				amount_faiz_kismi[13] = 0;
/*
				Printer_WriteStr( "amount_faiz_kismi:\n" );
				PrntUtil_BufAsciiHex( amount_faiz_kismi, 13 );
*/
			}
			else	/* 06_05. faizli degil ise surchargli demektir. */
			{
				/* 07_15 25/11/2002 surcharge kismi ataniyor */
				memset( amount_surcharge_kismi, 0,
						sizeof( amount_surcharge_kismi ) );
				memcpy(amount_surcharge_kismi, surcharge, 13);
				amount_surcharge_kismi[13] = 0;
				Utils_LeftPad(amount_surcharge_kismi,'0',13);
				amount_surcharge_kismi[13] = 0;
			}


			Slips_FormatAmount(surcharge,amount_temp);
/*
			Printer_WriteStr( "amount_temp1 :\n" );
			PrntUtil_BufAsciiHex( amount_temp, 13 );
*/
			/* FAIZLI 02_00 25/07/2000 Eðer faizli iþlem ise FAIZ yazýlacaksa */
			if ( FAIZLI_FLAG )
			{
				if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
					strcpy( d_Receipt.surcharge, "FA˜Z:                YTL" );
				else
					strcpy( d_Receipt.surcharge, "FA˜Z:                 TL" );
			}
			else
			{
				if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
					strcpy( d_Receipt.surcharge, "KOM:                 YTL" );
				else
					strcpy( d_Receipt.surcharge, "KOM:                  TL" );
			}

			memcpy( &d_Receipt.surcharge[21 - strlen( amount_temp )],
					amount_temp, strlen( amount_temp ) );
			d_Receipt.surcharge[24]=0;

			/** XLS_INTEGRATION2 30/10/2001 irfan. toplam basiliyor **/
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
				strcpy( d_Receipt.total_amount, "TOPLAM:              YTL" );
			else
			strcpy(d_Receipt.total_amount,"TOPLAM:               TL");
			Slips_FormatAmount( received_amount_temp, amount_temp );
/*
			Printer_WriteStr( "amount_temp2 :\n" );
			PrntUtil_BufAsciiHex( amount_temp, 13 );
*/
			memcpy( &d_Receipt.total_amount[21 - strlen( amount_temp )],
					amount_temp, strlen( amount_temp ) );
			d_Receipt.total_amount[24]=0;

		}

		/* approved message and no comission */
		else if ( Status == 0)          /* received_amount = amount_surcharge*/
		{						/*bm YTL 28/06/2004 */

			/* bm YTL   TL koruma   */
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		{
				/*	24.08.2004
				if (TL_Iade == TRUE)  /* 13.08.2004 
				   memcpy( received_amount_temp,temp_Amount_Iade, 
				   		   sizeof(amount_surcharge_iade) );
				*/
				if ( ( memcmp( YTL_Slip, "TLE", 3 ) == 0 ) )
				{
					memset( TL_Amount, ' ', sizeof( TL_amount_temp ) );
					memset( TL_amount_temp, ' ', sizeof( TL_Amount ) );
					memset( Amt_TMP, 0, sizeof( Amt_TMP ) );
					strcpy( Amt_TMP, "00000000000000000000" );
					Amt_TMP[20] = 0;

					if (TL_Iade == TRUE) /*@bm 24.08.2004 */
						memcpy(&Amt_TMP[4], &temp_Amount_Iade[0],13);
					else /*@bm */
					memcpy( &Amt_TMP[0], &received_amount[0], 13 );
					Amt_TMP[20] = 0;
					Slips_FormatAmountTL( Amt_TMP, TL_amount_temp );
					number_of_zeros =
						( int ) ( ( 21 - strlen( TL_amount_temp ) ) / 2 );
					memcpy( &TL_Amount[number_of_zeros], TL_amount_temp,
							strlen( TL_amount_temp ) );
					strcpy( &TL_Amount[number_of_zeros + strlen( TL_amount_temp )],	" TL" );
				}
				TL_Iade = FALSE;
			}
			/* bm YTL - TL     */
			/* received amount in case of no comission */
			Slips_FormatAmount(received_amount,amount_temp);
			number_of_zeros = (int)((21-strlen(amount_temp))/2);
			memcpy( &d_Receipt.amount[number_of_zeros], amount_temp,
					strlen( amount_temp ) );
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
				strcpy( &d_Receipt.
						amount[number_of_zeros + strlen( amount_temp )],
						" YTL" );
			else
				strcpy( &d_Receipt.amount[number_of_zeros + strlen( amount_temp )]," TL" );
			d_Receipt.amount[24]=0;
		}
		/* received amount is less than sent amount. so the process is cancelled */
		else
		{
			Slips_FormatAmount(amount_surcharge,amount_temp);
			number_of_zeros = (int)((21-strlen(amount_temp))/2);
			memcpy( &d_Receipt.amount[number_of_zeros], amount_temp,
					strlen( amount_temp ) );
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
				strcpy( &d_Receipt.
						amount[number_of_zeros + strlen( amount_temp )],
						" YTL" );
			else
				strcpy( &d_Receipt.amount[number_of_zeros + strlen( amount_temp )],	" TL" );
				d_Receipt.amount[24]=0;
	
			return(FALSE);
		}
	}
	else
	{
		Slips_FormatAmount( v_amount_surcharge, amount_temp );
		number_of_zeros = ( int ) ( ( 21 - strlen( amount_temp ) ) / 2 );
		memcpy( &d_Receipt.amount[number_of_zeros], amount_temp, strlen( amount_temp ) );
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) ) strcpy( &d_Receipt.amount[number_of_zeros + strlen( amount_temp )], " YTL" );
		else strcpy( &d_Receipt.amount[number_of_zeros + strlen( amount_temp )], " TL" );
		d_Receipt.amount[24] = 0;

		if ( ( memcmp( YTL_Slip, "TLE", 3 ) == 0 ) )
		{
			memset( TL_Amount, ' ', sizeof( TL_amount_temp ) );
			memset( TL_amount_temp, ' ', sizeof( TL_Amount ) );
			memset( Amt_TMP, 0, sizeof( Amt_TMP ) );
			strcpy( Amt_TMP, "00000000000000000000" );
			Amt_TMP[20] = 0;
			memcpy( &Amt_TMP[0], &v_amount_surcharge[0], 13 );
			Amt_TMP[20] = 0;
			Slips_FormatAmountTL( Amt_TMP, TL_amount_temp );
			number_of_zeros = ( int ) ( ( 21 - strlen( TL_amount_temp ) ) / 2 );
			memcpy( &TL_Amount[number_of_zeros], TL_amount_temp, strlen( TL_amount_temp ) );
			strcpy( &TL_Amount[number_of_zeros + strlen( TL_amount_temp )], " TL" );
		}
	}
	
	/*********************************** Fill ATS Related Fields **********************/
	/*--------------------------------------------------------------------------------*/

	/* Move Received ATS amount to the variables */
	memcpy(received_ATS_Amount, d_Transaction_Receive.ATS_Amount,13);
	received_ATS_Amount[13]=0;
	Utils_LeftPad(received_ATS_Amount,'0',13);

	/* Format ATS Amount */
	Slips_FormatAmount(received_ATS_Amount,amount_temp);
	if(strlen(amount_temp) >15)
	{
		strcpy(d_Receipt.ATS_Amount,received_ATS_Amount);
		d_Receipt.ATS_Amount[strlen(received_ATS_Amount)] = 0;
		Utils_LeftPad(d_Receipt.ATS_Amount,' ',15);
	}
	else
	{
		strcpy(d_Receipt.ATS_Amount,amount_temp);
		d_Receipt.ATS_Amount[strlen(amount_temp)] = 0;
		Utils_LeftPad(d_Receipt.ATS_Amount,' ',15);
	}

	d_Receipt.ATS_Date[0] = d_Transaction_Receive.ATS_Date[4];
	d_Receipt.ATS_Date[1] = d_Transaction_Receive.ATS_Date[5];
	d_Receipt.ATS_Date[2] = '/';
	d_Receipt.ATS_Date[3] = d_Transaction_Receive.ATS_Date[2];
	d_Receipt.ATS_Date[4] = d_Transaction_Receive.ATS_Date[3];
	d_Receipt.ATS_Date[5] = '/';
	d_Receipt.ATS_Date[6] = '2';
	d_Receipt.ATS_Date[7] = '0';
	d_Receipt.ATS_Date[8] = d_Transaction_Receive.ATS_Date[0];
	d_Receipt.ATS_Date[9] = d_Transaction_Receive.ATS_Date[1];
	d_Receipt.ATS_Date[10] = 0;

	return(TRUE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_FillErrorReceiptStructure.
 *
 * DESCRIPTION                  : Fill the receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 25/05/99
 *
 * ------------------------------------------------------------------------ */
boolean Slips_FillErrorReceiptStructure()
{
	BizSlip = FALSE;
	Slips_FillCommonReceiptStructure(FALSE,0,FALSE);
	if(CallFlag)
	{
		if (!VOID_FLAG)
		{
			memcpy(d_Receipt.message,d_Transaction_Receive.message,16);
			memcpy(&d_Receipt.message[17], d_Transaction_Receive.confirmation,3);
			d_Receipt.message[20]=0;
		}
		else
		{
			strcpy(d_Receipt.message,"IPTAL ONAYLANMADI");
		}
	}
	return(TRUE);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_FillReceiptStructure.
 *
 * DESCRIPTION                  : Fill the receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 19/05/99
 *
 * ------------------------------------------------------------------------ */
boolean Slips_FillReceiptStructure( boolean IsEMV, PTStruct * fpPTStruct,
									boolean Kampanya )
{
	char offline_approval_code[7];	/** XLS_PAYMENT 01/08/2001 irfan **/

	BizSlip = FALSE;

	if ( !Slips_FillCommonReceiptStructure( IsEMV, fpPTStruct, Kampanya ) )
	{
		Mainloop_DisplayWait("HATALI TUTAR D™NDš",2);   /* DEÐÝÞECEK MESAJ */
		return(FALSE);
	}
/* Toygar EMVed */	
	/*if(!OFFLINE_TRX || CallFlag)*/	/* 06_30 06/03/2003 OFFLINE_TRX kaldirildi */
	if(CallFlag)						/* 06_30 06/03/2003 OFFLINE_TRX kaldirildi */
/* Toygar EMVed */	
	{
		/* fill the approval code */
		if (!VOID_FLAG)
		{
			memcpy(d_Receipt.approval_code, d_Transaction_Receive.approval_code, 6);
			d_Receipt.approval_code[6] = 0;
		}
		else
		{
			memcpy(d_Receipt.approval_code, v_approval_code, 6);
			d_Receipt.approval_code[6] = 0;
		}
	}
	else
	{	/*fill offline approval code */
		/** XLS_INTEGRATION2 19/09/2001 irfan **/
		/* offline islem icin approval code'un eger iptal islemi ise batch den almasi saglandi*/
		if (!VOID_FLAG)
		{
			memset(offline_approval_code, 0, sizeof(offline_approval_code));
			memcpy(offline_approval_code, batch_no_c,3);
			memcpy(offline_approval_code+3,seq_no_c+1,3);
			memcpy(d_Receipt.approval_code, offline_approval_code,6);
			d_Receipt.approval_code[6] = 0; /** XLS_INTEGRATION2 19/09/2001 irfan **/
		}
		else
		{
			memcpy(d_Receipt.approval_code, v_approval_code, 6);
			d_Receipt.approval_code[6] = 0;
		}
	}
	return(TRUE);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_FormatCardExp
 *
 * DESCRIPTION                  : Formats Card No and Exp. date
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 20/05/99
 *
 * ------------------------------------------------------------------------ */
Slips_FormatCardExp(char *output_card_no, char *output_exp_date, char *input_card_no, char *input_exp_date)
{
	int Card_Length;
	int Number_of_Spaces;
	int Output_Length;
	int Card_Counter;

	Card_Length = strlen(input_card_no);
	Number_of_Spaces = (Card_Length-1) / 4;
	Output_Length = Card_Length + Number_of_Spaces;        /* length of output string */

	for (Card_Counter=0;Card_Counter<=Number_of_Spaces;Card_Counter++)
	{
		output_card_no[Card_Counter*5]=input_card_no[Card_Counter*4];
		output_card_no[Card_Counter*5+1]=input_card_no[Card_Counter*4+1];
		output_card_no[Card_Counter*5+2]=input_card_no[Card_Counter*4+2];
		output_card_no[Card_Counter*5+3]=input_card_no[Card_Counter*4+3];
		output_card_no[Card_Counter*5+4]=' ';
	}
	output_card_no[Output_Length]=0;

	output_exp_date[0]=input_exp_date[2];
	output_exp_date[1]=input_exp_date[3];
	output_exp_date[2]='/';
	output_exp_date[3]=input_exp_date[0];
	output_exp_date[4]=input_exp_date[1];
	output_exp_date[5]=0;

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_PrintReceipt
 *
 * DESCRIPTION                  : Print the receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 20/05/99
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintReceipt( boolean IsEMV, boolean Kopya )
{
	char amount_temp[25];		/* temp area to format amount values */
	char lpAmount[14];
	sint Counter;
	boolean CommaType;
	char *fpAmount;
	char *fpFJAmount;
	byte mode[2];
	char ch;

	/*verde */
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


	/*01_18 08/12/99 */
	char temp_approvalcode[7];

	/** XLS_PAYMENT 25/07/2001 irfan */
	byte inSlipType, inTranType;

	if (!Kopya)  /*@bm 14.09.2004 */
	{ 
		Kopya_ATS_FLAG = ATS_FLAG;
		Kopya_ATS_Count = ATS_Count;
		Kopya_LOYALTY_PERMISSION_FLAG = LOYALTY_PERMISSION_FLAG;
		Kopya_EMVAxess = EMVAxess;
		Kopya_IsEMV = IsEMV;
		Kopya_FAIZLI_FLAG = FAIZLI_FLAG;
	} 

	if (Kopya == TRUE) Slips_PrintCommonReceipt(Kopya_IsEMV, Kopya, TRUE );
	else Slips_PrintCommonReceipt(Kopya_IsEMV, Kopya, FALSE );

	/* PRINT THE AMOUNT */
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )	/* bm YTL 28/06/2004 */
		Slips_PrinterStr( d_Receipt.amount );
	else if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
	{
		Slips_PrinterStr( d_Receipt.amount );
		PrntUtil_Cr( 1 );
		if (!memcmp( YTL_Slip, "TLE", 3 ) && !Kopya_FAIZLI_FLAG) Slips_PrinterStr( TL_Amount );
/*			if (Kopya) Slips_PrinterStr( MyTL_Amount );
			else Slips_PrinterStr( TL_Amount );*/
	}

	if ( d_Receipt.SURCHARGE )	/* if surcharge print the surcharge */
	{
		Slips_PrinterStr( d_Receipt.surcharge );
		Slips_PrinterStr( d_Receipt.total_amount );
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ))	 /*@bm 04.09.2004 */
		{
			if ( ( memcmp( YTL_Slip, "TLE", 3 ) == 0 ) )
			{
			  if(Kopya_FAIZLI_FLAG)
				Slips_PrinterStr( TL_Amount );
			} 
	    }  /*@bm 04.09.2004 */
	}

	PrntUtil_Cr( 2 );

	if ((!d_Receipt.R_VOID) && (tran_type != PREPROV))
	{
		if (d_Receipt.tran_type != RETURN)
		{
			memcpy(msg,MSGSlipFixed1,24);
			msg[24]=0;
			Slips_PrinterStr(msg);
			PrntUtil_Cr(2); 
		}

		/* print the ATS part just in case of ATS */
		if ( Kopya_ATS_FLAG && ( Kopya_ATS_Count > 1 ) ) /* @bm 14.09.2004 */
		{
			Slips_PrintATSPart( Kopya );
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
	}

	/* basmasý önlenmiþ olacak */
	memset(temp_approvalcode, 0, 7);
	memcpy(temp_approvalcode, d_Receipt.approval_code, 6);
	temp_approvalcode[6] = 0;

	sprintf(msg, "PROV : %s 0000460000", temp_approvalcode);
	msg[24]=0;
	Slips_PrinterStr(msg);

	memcpy(msg,MSGSlipFixed3,24);
	msg[24]=0;
	Slips_PrinterStr(msg);

	/* VERDE akbank logosu basilir */
	if(SECOND_COPY || d_Receipt.R_VOID )
		Utils_PrintImage((byte *)aklogoPrint, 0, 128, 5);

	Printer_WaitForTextReady ();
	/** XLS_PAYMENT 23/07/2001 irfan. Call XLS module for Printing **/
	if (Kopya_LOYALTY_PERMISSION_FLAG && (( !Kopya_IsEMV ) || ( Kopya_EMVAxess ))) /*@bm 14.09.2004 */
	{	/* merchant copy */
		if(!SECOND_COPY)
		{
			if(RECOPY_FLAG)
			{
				inSlipType = 2;
				inTranType = d_Receipt.tran_type;
			}
			else
			{
				inSlipType = 0;
				inTranType = d_Receipt.tran_type;
			}
		}
		else /* customer copy */
		{
			if(RECOPY_FLAG)
			{
				inSlipType = 3;
				inTranType = d_Receipt.tran_type;
			}
			else
			{
				inSlipType = 1;
				inTranType = d_Receipt.tran_type;
			}
		}
	
		/** XLS_INTEGRATION 04/09/2001 irfan. Provide time to finish first slip without any line missing **/
		XLS_PrintTran(inSlipType, inTranType);
	}
		
	PrntUtil_Cr(3); /*06_05 slip sonundaki bosluk 6 dan 4 ye indirildi ve ilk kopya icin 2 satýr daha fazla basýldý */
	if(!SECOND_COPY) PrntUtil_Cr(3);
	Slips_WaitForPrinting ();

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_PrintErrorReceipt.
 *
 * DESCRIPTION                  : Fill the receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 25/05/99
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintErrorReceipt(boolean Kopya)
{

/*@bm 21.09.2004  Start */
	if ( !VOID_FLAG )
	{
		d_Receipt.tran_type = tran_type;
		d_Transaction_Send.input_type = input_type;
	}
	else
	{
		d_Receipt.tran_type = 6;
		d_Receipt.SURCHARGE = FALSE;
		d_Transaction_Send.input_type = input_type;
	}
/*@bm 21.09.2004 End*/

		if (Kopya == TRUE) Slips_PrintCommonReceipt(FALSE, Kopya, TRUE );
		else Slips_PrintCommonReceipt(FALSE, Kopya, FALSE );
		PrntUtil_Cr(1);                 
		Slips_PrinterStr(d_Receipt.amount);
		sprintf(msg,"  %s  ",d_Receipt.message);
		msg[24]=0;
		PrntUtil_Cr(2);
		Slips_PrinterStr(msg);
		PrntUtil_Cr(6);
		Mte_Wait(800);
		Kbhw_BuzzerPulse(800);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_PrintCommonReceipt
 *
 * DESCRIPTION                  : Print the receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 20/05/99
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintCommonReceipt (boolean IsEMV , boolean Kopya, boolean SkipEntryMode )
{

	char temp_local[24] ; /** XLS_INTEGRATION2 30/10/2001 irfan. test amacli **/
	char * Tran_Types[] = { NULL, "SATIž", "ON PROV", "MANUEL", "˜ADE ", NULL, "˜PTAL", NULL};

	char masked_card_no[24]; 	/** XLS 04_02 irfan. 06/12/2001.  **/

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


	/* GRAFPOS terminal no 8 digit basiliyor */
	memset (msg, 0, sizeof(msg));
	merch_device_no[8] = 0;
	sprintf( msg,"CIHAZ NO: %s\n", merch_device_no);
	msg[24]=0;
	Slips_PrinterStr(msg);

	

	/* DATE , TIME and transaction type */
	sprintf(msg, "%s %s %s\n\n", d_Receipt.date, d_Receipt.time, Tran_Types[d_Receipt.tran_type]);
	msg[24] = 0;
	Slips_PrinterStr(msg);  /* sonuna null koymak gerekebilir */
	
	if (!Kopya)  /*@bm 22.09.2004 */
	{ 
		memset( Kopya_card_no, 0, sizeof( Kopya_card_no ) );
		strcpy( Kopya_card_no, card_no );
		PanLength = strlen(Kopya_card_no);	
	} 
	
//	Printer_WriteStr("\nbm gec03\n");
//	PrntUtil_BufAsciiHex((byte *)d_Receipt.card_no,24);

	/** XLS_04_02 irfan. 06/12/2001. customer copy'de kart no'nun sadece son 4 digit'i basiliyor **/
	if ( !SECOND_COPY )
	{
		/*@bm  20.08.2004  */
		if (tran_type == PREPROV)
		{
			if (!DEBIT_FLAG)
			{
				/* formatted card no @bm 20.08.2004 */
				Slips_PrinterStr(d_Receipt.card_no);
				Slips_PrinterStr("\n");
			}
			else
			{
				memset( masked_card_no, 0, sizeof( masked_card_no ) );
				strcpy( masked_card_no, d_Receipt.card_no );
				 if ( PanLength == 0xF )  		 /*15*/
					memcpy(masked_card_no, "**** **** ***", 13);
				else if ( PanLength == 0x12 )
					memcpy(masked_card_no, "**** **** **** **",17);
				 else if ( PanLength == 0x13 )	 /*19*/
					memcpy(masked_card_no, "**** **** **** ***", 18); 
				 else							 /*16*/
					memcpy(masked_card_no, "**** **** ****", 14); 
				Slips_PrinterStr(masked_card_no);
				Slips_PrinterStr("\n");
		    }
		}
		else
		{
			memset(masked_card_no, 0, sizeof(masked_card_no));
			strcpy(masked_card_no, d_Receipt.card_no);
			if ( PanLength == 0xF )
				memcpy(&masked_card_no[4], " **** ***",9);
			else if ( PanLength == 0x12 )
				memcpy(&masked_card_no[4], " **** **** **",13);
			else if ( PanLength == 0x13 )
				memcpy(&masked_card_no[4], " **** **** ***",14);
			else
				memcpy(&masked_card_no[4], " **** ****",10);
			Slips_PrinterStr(masked_card_no);
			Slips_PrinterStr("\n");
		}
	}
	else
	{
		if ( ( TRACK1ERROR && ( /*d_Receipt.input_type*/ myinput_type == 'D' ) ) ||
			 ( CHIP_ERROR && ( /*d_Receipt.input_type*/ myinput_type == 'C' ) ) ||
			 ( ( /*d_Receipt.input_type*/ myinput_type == 'E' ) ) ||
			 ( ( /*d_Receipt.input_type*/ myinput_type == 'T' ) ) ||
			 ( ( /*d_Receipt.input_type*/ myinput_type == 'B' ) ) ||
			 ( ( /*d_Receipt.input_type*/ myinput_type == 'F' ) ) )
		{	/* isim yaziliyor */
			memset( masked_card_no, 0, sizeof( masked_card_no ) );
			strcpy( masked_card_no, d_Receipt.card_no );
			/*if ( VOID_FLAG )	 /*31.08.2004
			{
				 if ( PanLength == 0xF )		   /*15
					memcpy( masked_card_no + 4, " **** ***", 9 );
				 else if ( PanLength == 0x13 )	   /*19
					memcpy( masked_card_no + 4, " **** **** ***", 14 );
				 else							   /*16
					memcpy( masked_card_no + 4, " **** ****", 10 );
			}
			else
			{*/					
				 if ( PanLength == 0xF )  		 /*15*/
					memcpy(masked_card_no, "**** **** ***", 13);
				else if ( PanLength == 0x12 )
					memcpy(masked_card_no, "**** **** **** **",17);
				 else if ( PanLength == 0x13 )	 /*19*/
					memcpy(masked_card_no, "**** **** **** ***", 18); 
				 else							 /*16*/
					memcpy(masked_card_no, "**** **** ****", 14); 
			/*} */
			Slips_PrinterStr( masked_card_no );
			Slips_PrinterStr( "\n" );
		}
		else
		{	/* isim yazilmiyor formatted card no */
			memset( masked_card_no, 0, sizeof( masked_card_no ) );
			strcpy( masked_card_no, d_Receipt.card_no );

		    if ( PanLength == 0xF )	   		/*15*/
			 memcpy( masked_card_no + 4, " **** ***", 9 );
			else if ( PanLength == 0x12 )
				memcpy(masked_card_no + 4, " **** **** **",13);
		    else if ( PanLength == 0x13 )	/*19*/
			 memcpy( masked_card_no + 4, " **** **** ***", 14 );
			else						    /*16*/
			 memcpy( masked_card_no + 4, " **** ****", 10 );
		
			Slips_PrinterStr( masked_card_no );
			Slips_PrinterStr( "\n" );
		}

	}

/* Toygar EMVed */
	if ((SkipEntryMode == TRUE) && (OldAIDLen != 0))
	{
			Slips_PrinterStr("\n"); /*@bm 01.09.2004 */
			Slips_PrinterStr("AID: ");
			memset(temp_local, 0, sizeof(temp_local));
			memcpy(temp_local, OldAID, OldAIDLen);
			Slips_PrinterStr(temp_local);
			Slips_PrinterStr("\n");
	
			Slips_PrinterStr("APPL: ");
			memset(temp_local, 0, sizeof(temp_local));
			memcpy(temp_local,OldAPPL,OldAPPLLen);
			Slips_PrinterStr(temp_local);
			Slips_PrinterStr("\n");	
	}
	else if (IsEMV)
	{
		if ((POSEntryFlag != 2) && (POSEntryFlag != 1))
		{
			RowData   rdData;

			/* Adding AID */
			EmvDb_Find(&rdData, tagAID_TERMINAL);
			EmvIF_BinToAscii(rdData.value,d_Receipt.AID,rdData.length, 16,FALSE);
			d_Receipt.AIDLen = rdData.length * 2;

			Slips_PrinterStr("\n"); /*@bm 01.09.2004 */
			Slips_PrinterStr("AID: ");
			memset(temp_local, 0, sizeof(temp_local));
			memcpy(temp_local, d_Receipt.AID, d_Receipt.AIDLen);
			memset(OldAID, 0, sizeof(OldAID));
			memcpy(OldAID, d_Receipt.AID, d_Receipt.AIDLen);
			OldAIDLen = d_Receipt.AIDLen;
			Slips_PrinterStr(temp_local);
			Slips_PrinterStr("\n");		

			/* Adding APPL_LABEL */
			EmvDb_Find(&rdData, tagAPPL_LABEL);

			Slips_PrinterStr("APPL: ");
			memset(temp_local, 0, sizeof(temp_local));
			memcpy(temp_local,rdData.value,rdData.length);
			memset(OldAPPL, 0, sizeof(OldAPPL));
			memcpy(OldAPPL,rdData.value,rdData.length);
			OldAPPLLen = rdData.length;
			Slips_PrinterStr(temp_local);
			Slips_PrinterStr("\n");		
		}
	}
	else OldAIDLen = 0;
/* Toygar EMVed */

	/** XLS_PAYMENT 19/07/2001 irfan. printing name and surname in case of chip **/
	/** XLS 04_02 19/12/2001 irfan. track 1 hatali oldugunda bir onceki baþarýlý iþelmin ismin basýlmasý engellendi **/
	if ( ( TRACK1ERROR && ( /*d_Receipt.input_type*/ myinput_type == 'D' ) )
		 || ( CHIP_ERROR &&
			  ( /*d_Receipt.input_type*/ myinput_type == 'C' ) ) ||
		 ( ( /*d_Receipt.input_type*/ myinput_type == 'E' ) ) ||
/*		(ISIMYAZMA_FLAG && (myinput_type == 'T'))  ||*/
		 ( ( /*d_Receipt.input_type*/ myinput_type == 'B' ) ) ||
		 ( ( /*d_Receipt.input_type*/ myinput_type == 'F' ) ) )
	{	
		Slips_PrinterStr(d_Receipt.Card_Holder_Name);
	}

	PrntUtil_Cr(2);	
	
	/** XLS_04_02 irfan. 06/12/2001. customer copy'den gecerlilik tarihi kalkiyor **/
	if(!SECOND_COPY && !(d_Receipt.tran_type==6))
	{	/* formatted exparation date */
		sprintf(msg, "GE€. TAR.: %s\n", d_Receipt.exp_date);
		msg[24] = 0;
		Slips_PrinterStr(msg);
	}

	
	/* ref no: batch_no_c + '/' +seq_no_c */
	sprintf(msg, "REF NO   : %s  (%c)\n\n", d_Receipt.ref_no, /*d_Receipt.input_type*/ myinput_type);
/*	sprintf(msg, "REF NO   : %s      \n\n", d_Receipt.ref_no);*/
	msg[24] = 0;
	Slips_PrinterStr(msg);
	PrntUtil_Cr(2);


/*
		Printer_WriteStr("d_Receipt.kampanya_str:\n");
		PrntUtil_BufAsciiHex((byte *)d_Receipt.kampanya_str,sizeof(d_Receipt.kampanya_str));
		Printer_WriteStr("msgSlips:\n");
		PrntUtil_BufAsciiHex((byte *)msgSlips,sizeof(msgSlips));
*/

	/*@bm 15.05.2004 KVK boþ satýr basýlmayacak */
	if(KampanyaFlag == TRUE) 
		if ( ( msgSlips[10] != ' ' ) && ( msgSlips[11] != ' ' ) )
		{
			char tempkampanya[30];
	
			memset( tempkampanya, 0, 30 );
			memcpy( tempkampanya,msgSlips,
					sizeof( msgSlips ) );
			Slips_PrinterStr( tempkampanya );
			PrntUtil_Cr( 2 );
		}
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_PrintATSPart
 *
 * DESCRIPTION                  : Print the ATS part of the receipt structure
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. & t.c.a. 17/06/99
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintATSPart( boolean Kopya )
{
	int i;

	/* Print the fixed title of the ATS list */
	Slips_PrinterStr("    TAKSIT BILGILERI    ");
	Slips_PrinterStr("~~~~~~~~~~~~~~~~~~~~~~~~");

	/* Print ATS part */
	if (tran_type == RETURN)	/* @bm 13.08.2004 */
		sprintf( msg, "ILK IADE TRH:%s", d_Receipt.ATS_Date );
	else
	sprintf(msg, "ILK ODEME TRH:%s",d_Receipt.ATS_Date);

	if (!Kopya)  /*@bm 15.09.2004 */
	{ 
		memcpy( Kopya_ATS_Count_c, ATS_Count_c, sizeof( ATS_Count_c ) );
	} 
	msg[24] = 0;
	Slips_PrinterStr(msg);
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		sprintf( msg, "%sYTL X %s\n", d_Receipt.ATS_Amount, Kopya_ATS_Count_c ); 
	else
		sprintf( msg, "%s TL X %s\n", d_Receipt.ATS_Amount, Kopya_ATS_Count_c );
	msg[24] = 0;

	Slips_PrinterStr(msg);
	
	Slips_PrinterStr("~~~~~~~~~~~~~~~~~~~~~~~~");
	PrntUtil_Cr(2); 
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_PrintAgreementReport
 *
 * DESCRIPTION                  : Prints the agreement totals
 *
 * RETURN                               : None
 *      
 * NOTES                                : r.i.o. & t.c.a. 02/06/99
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintAgreementReport(void)
{
	char msg_agreement[25];
	char temp[20];



	PrntUtil_Cr(2); 
	if (d_EOD_Receive.agreement == '0' )
	{
		strcpy(msg_agreement,".....MUTABAKATSIZLIK....");
		msg_agreement[24]=0;
		Slips_PrinterStr(msg_agreement);
	}
	else
	{
		strcpy(msg_agreement,"........MUTABAKAT.......");
		msg_agreement[24]=0;
		Slips_PrinterStr(msg_agreement);
	}
	

	memset(temp, ' ', sizeof(temp));
	memcpy(temp, d_EOD_Receive.sales_count_dif, 4);
	memcpy(temp+5, d_EOD_Receive.sales_totals_dif, 13);
	temp[18]=0;
	sprintf(msg_agreement, "SATIž %s", temp);
	msg_agreement[24]=0;
	Slips_PrinterStr(msg_agreement);

	memset(temp, ' ', sizeof(temp));
	memcpy(temp, d_EOD_Receive.manuel_count_dif, 4);
	memcpy(temp+5, d_EOD_Receive.manuel_totals_dif, 13);
	temp[18]=0;
	sprintf(msg_agreement, "MANUEL%s", temp);
	msg_agreement[24]=0;
	Slips_PrinterStr(msg_agreement);

	memset(temp, ' ', sizeof(temp));
	memcpy(temp, d_EOD_Receive.return_count_dif, 4);
	memcpy(temp+5, d_EOD_Receive.return_totals_dif, 13);
	temp[18]=0;
	sprintf(msg_agreement, "˜ADE  %s", temp);
	msg_agreement[24]=0;
	Slips_PrinterStr(msg_agreement);

	memset(temp, ' ', sizeof(temp));
	memcpy(temp, d_EOD_Receive.preprov_count_dif, 4);
	memcpy(temp+5, d_EOD_Receive.preprov_totals_dif, 13);
	temp[18]=0;
	sprintf(msg_agreement, "ONPROV%s", temp);
	msg_agreement[24]=0;
	Slips_PrinterStr(msg_agreement);

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_PrintReportHeader
 *
 * DESCRIPTION                  : Prints the header for Report
 *
 * RETURN                               : None
 *      
 * NOTES                                : r.i.o. & t.c.a. 02/06/99
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintReportHeader(boolean IsEOD)
{
	char toprinter[25];
	char header_date[9], header_time[6];


	if (IsEOD)
		strcpy(toprinter, "        GUN SONU\n\n");
	else
		strcpy(toprinter, "       ARA RAPOR\n\n");

	toprinter[18] = 0;
	Slips_PrinterStr(toprinter);
	PrntUtil_Cr(1);

	strcpy(toprinter, "      HESAP OZETI\n\n");
	toprinter[19] = 0;
	Slips_PrinterStr(toprinter);

	Slips_PrinterStr(merch_addr);
	PrntUtil_Cr(1);

	sprintf(toprinter, "ISYERI NO : %s", merch_no);
	toprinter[24] = 0;
	Slips_PrinterStr(toprinter);

	sprintf(toprinter, "AMEX NO   : %s\n", merch_AMEX_no);
	toprinter[23] = 0;
	Slips_PrinterStr(toprinter);
	
	sprintf(toprinter, "CIHAZ NO  : %s\n", merch_device_no);
	toprinter[21] = 0;
	Slips_PrinterStr(toprinter);

	sprintf(toprinter, "SERI NO   : %c\n", device_serial_no);
	toprinter[14] = 0;
	Slips_PrinterStr(toprinter);

	sprintf(toprinter, "YIGIN NO  : %s\n\n", batch_no_c);
	toprinter[17] = 0;
	Slips_PrinterStr(toprinter);

	PrntUtil_Cr(1);


	Utils_GetDateTime(header_date, header_time);
	header_date[8] = 0;
	header_time[5] = 0;

	sprintf(toprinter, "%s         %s\n", header_date, header_time);
	toprinter[23] = 0;
	Slips_PrinterStr(toprinter);
	
	PrntUtil_Cr(2);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_PrintDetailRecord
 *
 * DESCRIPTION                  : Prints the details for Report
 *
 * RETURN                               : None
 *      
 * NOTES                                : r.i.o. & t.c.a. 02/06/99
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintDetailRecord(void)
{
	char toprinter[25];
	char index;
	sint i, j, lbStatus, lwLastSeq;
	char input_card_no[20], output_card_no[25], 
	     output_exp_date[6], card_name[5],
		 temporary_string1[14], temporary_string2[9], temporary_string3[6];
	TransactionStruct fpTransStruct;
	
	sint Handle1;
	boolean display_transaction;
	char temporary_string4[17];
/*	char temporary_string5[14]; /*@bm 24.08.2004 */
/*	char temporary_string6[25]; /*@bm 24.08.2004 */
	char *Tran_Types[] =
		{ NULL, "SAT.", "ONP.", "MAN.", "IADE", NULL, NULL, NULL };

	strcpy(toprinter, "  AKPOS SLIP BILGILERI\n");
	toprinter[23] = 0;
	Slips_PrinterStr(toprinter);
	Slips_PrinterStr ("------------------------");	/* 06_30 */
	PrntUtil_Cr(1);									/* 06_30 2 idi 1 yapýldý */
	
	/* Display Transactions */
	if (RamDisk_ChkDsk((byte *)Transaction_File) != RD_CHECK_OK)
	{
		strcpy(toprinter, "    YIGIN ACILAMADI\n");
		toprinter[20] = 0;
		Slips_PrinterStr(toprinter);
		PrntUtil_Cr(2);
		return;
	}

	Handle1 = RamDisk_Reopen((byte *)Transaction_File)->Handle;

	RamDisk_Seek(Handle1, SEEK_FIRST);
	lwLastSeq = RamDisk_Seek(Handle1,SEEK_LAST);


	for (i=0; i < lwLastSeq; i++)   
	{
		if (RamDisk_Seek(Handle1, i) >= 0)
		{
			lbStatus = RamDisk_Read(Handle1, &fpTransStruct);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
				display_transaction = TRUE;
			else
				display_transaction = FALSE;
		}
		else
			display_transaction = FALSE;

		if (display_transaction)
		{

			if (fpTransStruct.agreement == '2')
			{
				strcpy(toprinter, "    MUTABAKATSIZLIK\n");
				toprinter[20] = 0;
				Slips_PrinterStr(toprinter);
			}

			index = fpTransStruct.tran_type - '0';

			memcpy(temporary_string1, fpTransStruct.seq_no, 4);
			temporary_string1[4] = 0;
			memcpy(temporary_string2, fpTransStruct.date, 8);
			temporary_string2[8] = 0;
			memcpy(temporary_string3, fpTransStruct.time, 5);
			temporary_string3[5] = 0;

		
			sprintf( toprinter, "%s %s %s %s", temporary_string1,
					 Tran_Types[index], temporary_string2,
					 temporary_string3 );
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			memcpy(input_card_no, fpTransStruct.card_no, 19);
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

			j = ((fpTransStruct.card_type[0] - '0') * 10) +
			     (fpTransStruct.card_type[1] - '0');

			if (j>0)
			{
				j--;
				memcpy(card_name, d_Card_Type_Table[j].short_name, 4);
			}
			else
				memset(card_name, ' ', 4);

			card_name[4] = 0;

/*@bm 24.08.2004 
			Printer_WriteStr( "fpTransStruct.amount_surcharge\n" ); 
			PrntUtil_BufAsciiHex( ( byte * ) fpTransStruct.amount_surcharge,	13);
/*			memset( temporary_string5, 0, sizeof( temporary_string5 ) );
			memset( temporary_string6, 0, sizeof( temporary_string6 ) );
			memcpy( temporary_string5, fpTransStruct.amount_surcharge , 13 );
/*			Utils_WithComma( temporary_string5, temporary_string6, 14, FALSE );	/*bm YTL */
/*			Slips_FormatAmount( temporary_string5, temporary_string6 );
			Printer_WriteStr( "temporary_string5\n" ); 
			PrntUtil_BufAsciiHex( ( byte * ) temporary_string6,	25);
/*@bm 24.08.2004 */
			/*@bm 26.08.2004 YTL POS'un raporunda "." olacak  */
			if( ( POS_Type == '5' ) || ( POS_Type == '6' ) )	
			{
				memcpy( temporary_string1, fpTransStruct.amount_surcharge + 1 , 10 );
				temporary_string1[10] ='.';
				temporary_string1[11] =fpTransStruct.amount_surcharge[11];
				temporary_string1[12] =fpTransStruct.amount_surcharge[12];
			} 
			else 
			memcpy(temporary_string1, fpTransStruct.amount_surcharge, 13);
			temporary_string1[13] = 0;
			memcpy(temporary_string2, fpTransStruct.approval_code, 6);
			temporary_string2[6] = 0;

			sprintf( toprinter, "%s%s %s", card_name, temporary_string2,
					 temporary_string1 );
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			strcpy(temporary_string1, "01");
			memcpy(temporary_string2, fpTransStruct.installment_number, 2);
			temporary_string2[2] = 0;

			if (memcmp(temporary_string2, temporary_string1, 2) != 0)
			{
				/* FAIZLI 02_00 26/07/2000 Faiz kismi detay rapora yazýlýyor */
				memset(temporary_string1 , 0 , sizeof(temporary_string1));
				memset(temporary_string1, '0', 13);
				if (memcmp(temporary_string1, fpTransStruct.amount_faiz, 13) != 0)
				{
					memset(temporary_string1 , 0 , sizeof(temporary_string1));
					/*@bm 03.09.2004 YTL POS'un raporunda "." olacak ISSUE_72_ */
					if (( POS_Type == '5' ) || ( POS_Type == '6' ) )		
					{
						memcpy( temporary_string1, fpTransStruct.amount_faiz + 1 , 10 );
						temporary_string1[10] ='.';
						temporary_string1[11] =fpTransStruct.amount_faiz[11];
						temporary_string1[12] =fpTransStruct.amount_faiz[12];
					} 
					else 
					{
					memcpy(temporary_string1, fpTransStruct.amount_faiz, 13);
					}
					temporary_string1[13] = 0;
					memset(toprinter, 0, 25);
					sprintf(toprinter, "FA˜Z:      %s\n", temporary_string1);
					Slips_PrinterStr(toprinter);
				}

				memset(toprinter, 0, 25);
				sprintf( toprinter, "TAKSIT SAYISI: %s\n", temporary_string2 );
				Slips_PrinterStr( toprinter );
			}

			/*bm YTL Kasier bilgisi slipe basýlacak 13.08.2004	 */
			if (Cashier_Flag==1) 
			{
				PrntUtil_Cr(1);
				memset(toprinter, 0 , sizeof(toprinter));
				memset(temporary_string4, 0 , sizeof(temporary_string4));
				memcpy(temporary_string4,fpTransStruct.Kasiyer_Id,16);
				temporary_string4[16]= 0 ;
				sprintf(toprinter,"KSYR.ID:%s\n",temporary_string4);
				toprinter[24] = 0;
				Slips_PrinterStr(toprinter);
			}

			/*@bm 15.05.2004 KVK */
			if ( ( fpTransStruct.kampanya_str[10] != ' ' ) &&
				 ( fpTransStruct.kampanya_str[11] != ' ' ) )
			{
				memset( toprinter, 0, 25 );
				memcpy( toprinter, fpTransStruct.kampanya_str,
						sizeof( fpTransStruct.kampanya_str ) );
				Slips_PrinterStr( toprinter );
			}
			if(fpTransStruct.void_status == '1')
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
 * FUNCTION NAME                : Slips_PrintCardTypeTotals
 *
 * DESCRIPTION                  : Prints the CardTypeTotals for Report
 *
 * RETURN                       : None
 *      
 * NOTES                        : r.i.o. & t.c.a. 02/06/99
 *								: FAIZLI 02_00 26/07/2000. Faizli taksitli sisteme
 *                                göre tamaiyle revize edildi. Bu kisim, Türker'in
 *                                Büyük Firma Uygulamasýndan aynen alýndý.
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintCardTypeTotals(void)
{
	sint i, j;
	char toprinter[25], formatted_result[18];
	char arti_result[14], overall_result[14];
	char temp_amount_1[14], temp_amount_2[14];
	char islem_sayisi[5];
	boolean transaction_with_credit, credit_transaction_exists;
	boolean brut_negative, faiz_negative;
	char brut_char[14], faiz_char[14];
	char total_faiz_amount [14];
	boolean total_faiz_negative;

	char Temp_Amount_YTL[14];  	/*@bm 26.08.2004 */

	credit_transaction_exists = FALSE;
	total_faiz_negative = FALSE;
	total_faiz_amount[0] = '0';
	total_faiz_amount[1] = 0;

	for (i=0; i<CARDTYPETABLESIZE; i++)
	{
		if (d_Card_Type_Table[i].transaction_occured)
		{
			memcpy(toprinter, d_Card_Type_Table[i].long_name, strlen(d_Card_Type_Table[i].long_name));
			toprinter[strlen(d_Card_Type_Table[i].long_name)] = 0;
			Slips_PrinterStr(toprinter);
			PrntUtil_Cr(1);
			if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
				strcpy( toprinter, "......ADET.....YTL......" );
			else
				strcpy(toprinter, "......ADET......TL......");
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			Utils_IToA(d_Card_Type_Table[i].d_Accummulators[SALE-1].tran_count, islem_sayisi);
			Utils_LeftPad(islem_sayisi, '0', 4);

			/*@bm 26.08.2004 */
			if( ( POS_Type == '5' ) || ( POS_Type == '6' ) )	
			{
				memset(Temp_Amount_YTL,'0',sizeof(Temp_Amount_YTL));
				memcpy( Temp_Amount_YTL, d_Card_Type_Table[i].d_Accummulators[SALE - 1].tran_amount + 1 , 10 );
				Temp_Amount_YTL[10] ='.';
				Temp_Amount_YTL[11] =d_Card_Type_Table[i].d_Accummulators[SALE - 1].tran_amount[11];
				Temp_Amount_YTL[12] =d_Card_Type_Table[i].d_Accummulators[SALE - 1].tran_amount[12];
			} else {
				memset(Temp_Amount_YTL,'0',sizeof(Temp_Amount_YTL));
				memcpy( Temp_Amount_YTL, d_Card_Type_Table[i].d_Accummulators[SALE - 1].tran_amount , 13 );
			}
			Temp_Amount_YTL[13]=0;
			sprintf( toprinter, "SATIS %s %s", islem_sayisi,Temp_Amount_YTL);
			/*@bm 26.08.2004 d_Card_Type_Table[i].d_Accummulators[SALE - 1].tran_amount ); */
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			Utils_IToA(d_Card_Type_Table[i].d_Accummulators[MANUEL-1].tran_count, islem_sayisi);
			Utils_LeftPad(islem_sayisi, '0', 4);


			/*@bm 26.08.2004 */
			if( ( POS_Type == '5' ) || ( POS_Type == '6' ) )	
			{
				memset(Temp_Amount_YTL,'0',sizeof(Temp_Amount_YTL));
				memcpy( Temp_Amount_YTL, d_Card_Type_Table[i].d_Accummulators[MANUEL - 1].tran_amount + 1 , 10 );
				Temp_Amount_YTL[10] ='.';
				Temp_Amount_YTL[11] =d_Card_Type_Table[i].d_Accummulators[MANUEL - 1].tran_amount[11];
				Temp_Amount_YTL[12] =d_Card_Type_Table[i].d_Accummulators[MANUEL - 1].tran_amount[12];
			} else {
				memset(Temp_Amount_YTL,'0',sizeof(Temp_Amount_YTL));
				memcpy( Temp_Amount_YTL, d_Card_Type_Table[i].d_Accummulators[MANUEL - 1].tran_amount , 13 );
			}
			Temp_Amount_YTL[13]=0;
			sprintf( toprinter, "MANUE %s %s", islem_sayisi,Temp_Amount_YTL);
			/*@bm 26.08.2004 d_Card_Type_Table[i].d_Accummulators[MANUEL - 1].tran_amount ); */
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			Utils_IToA(d_Card_Type_Table[i].d_Accummulators[RETURN-1].tran_count, islem_sayisi);
			Utils_LeftPad(islem_sayisi, '0', 4);
			/*@bm 26.08.2004 */
			if( ( POS_Type == '5' ) || ( POS_Type == '6' ) )	
			{
				memset(Temp_Amount_YTL,'0',sizeof(Temp_Amount_YTL));
				memcpy( Temp_Amount_YTL, d_Card_Type_Table[i].d_Accummulators[RETURN - 1].tran_amount + 1 , 10 );
				Temp_Amount_YTL[10] ='.';
				Temp_Amount_YTL[11] =d_Card_Type_Table[i].d_Accummulators[RETURN - 1].tran_amount[11];
				Temp_Amount_YTL[12] =d_Card_Type_Table[i].d_Accummulators[RETURN - 1].tran_amount[12];
			} else {
				memset(Temp_Amount_YTL,'0',sizeof(Temp_Amount_YTL));
				memcpy( Temp_Amount_YTL, d_Card_Type_Table[i].d_Accummulators[RETURN - 1].tran_amount , 13 );
			}
			Temp_Amount_YTL[13]=0;
			sprintf( toprinter, "IADE  %s %s", islem_sayisi,Temp_Amount_YTL);
			/*@bm 26.08.2004 d_Card_Type_Table[i].d_Accummulators[RETURN - 1].tran_amount );*/
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			Utils_IToA(d_Card_Type_Table[i].d_Accummulators[PREPROV-1].tran_count, islem_sayisi);
			Utils_LeftPad(islem_sayisi, '0', 4);
			/*@bm 26.08.2004 */
			if( ( POS_Type == '5' ) || ( POS_Type == '6' ) )	
			{
				memset(Temp_Amount_YTL,'0',sizeof(Temp_Amount_YTL));
				memcpy( Temp_Amount_YTL, d_Card_Type_Table[i].d_Accummulators[PREPROV - 1].tran_amount + 1 , 10 );
				Temp_Amount_YTL[10] ='.';
				Temp_Amount_YTL[11] =d_Card_Type_Table[i].d_Accummulators[PREPROV - 1].tran_amount[11];
				Temp_Amount_YTL[12] =d_Card_Type_Table[i].d_Accummulators[PREPROV - 1].tran_amount[12];
			} else {
				memset(Temp_Amount_YTL,'0',sizeof(Temp_Amount_YTL));
				memcpy( Temp_Amount_YTL, d_Card_Type_Table[i].d_Accummulators[PREPROV - 1].tran_amount , 13 );
			}
			Temp_Amount_YTL[13]=0;
			sprintf( toprinter, "ONPRO %s %s", islem_sayisi,Temp_Amount_YTL);
			/*@bm 26.08.2004 d_Card_Type_Table[i].d_Accummulators[PREPROV - 1].tran_amount ); */
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			PrntUtil_Cr(2);


			transaction_with_credit = FALSE;
			brut_negative = FALSE;
			faiz_negative = FALSE;
			memset(temp_amount_1, '0', 13);
			if (memcmp(temp_amount_1, d_Card_Type_Table[i].d_Accummulators[SALE-1].tran_interest_amount, 13) != 0)
				 transaction_with_credit = TRUE;
			if (memcmp(temp_amount_1, d_Card_Type_Table[i].d_Accummulators[MANUEL-1].tran_interest_amount, 13) != 0)
				 transaction_with_credit = TRUE;
			if (memcmp(temp_amount_1, d_Card_Type_Table[i].d_Accummulators[RETURN-1].tran_interest_amount, 13) != 0)
				 transaction_with_credit = TRUE;

			if (transaction_with_credit)
			{
				if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
					strcpy( toprinter, "BRšT                 YTL" );
				else
					strcpy(toprinter, "BRšT                  TL");
				credit_transaction_exists = TRUE;
			}
			else
			{
				if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
					strcpy( toprinter, "NET                  YTL" );
				else
					strcpy(toprinter, "NET                   TL");
			}

			memcpy(temp_amount_1, d_Card_Type_Table[i].d_Accummulators[SALE-1].tran_amount, 13);
			memcpy(temp_amount_2, d_Card_Type_Table[i].d_Accummulators[MANUEL-1].tran_amount, 13);
			temp_amount_1[13] = 0;
			temp_amount_2[13] = 0;
			Utils_Add_Strings(temp_amount_1, temp_amount_2, arti_result);

			memcpy(temp_amount_1, d_Card_Type_Table[i].d_Accummulators[RETURN-1].tran_amount, 13);
			temp_amount_1[13] = 0;

			switch (Utils_Compare_Strings(arti_result, temp_amount_1))
			{
			case 1 :
				Utils_Subtract_Strings(arti_result, temp_amount_1, overall_result);
				break;
			case 0 :
				overall_result[0] = '0';
				overall_result[1] = 0;				
				break;
			case 2 :
				brut_negative = TRUE;
				Utils_Subtract_Strings(temp_amount_1, arti_result, overall_result);
				break;
			}

			Slips_FormatAmount(overall_result, formatted_result);
			j = (21 - strlen(formatted_result));
			memcpy(&toprinter[j], formatted_result, strlen(formatted_result));
			if (brut_negative)
				toprinter[j - 1] = '-';
			
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);
			PrntUtil_Cr(1);

			if (transaction_with_credit)
			{
				memcpy(brut_char, overall_result, strlen(overall_result));
				brut_char[strlen(overall_result)] = 0;
				if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
					strcpy( toprinter, "FA˜Z                 YTL" );
				else
				strcpy(toprinter, "FA˜Z                  TL");
				memcpy(temp_amount_1, d_Card_Type_Table[i].d_Accummulators[SALE-1].tran_interest_amount, 13);
				memcpy(temp_amount_2, d_Card_Type_Table[i].d_Accummulators[MANUEL-1].tran_interest_amount, 13);
				temp_amount_1[13] = 0;
				temp_amount_2[13] = 0;
				Utils_Add_Strings(temp_amount_1, temp_amount_2, arti_result);

				memcpy(temp_amount_1, d_Card_Type_Table[i].d_Accummulators[RETURN-1].tran_interest_amount, 13);
				temp_amount_1[13] = 0;

				switch (Utils_Compare_Strings(arti_result, temp_amount_1))
				{
				case 1 :
					Utils_Subtract_Strings(arti_result, temp_amount_1, overall_result);
					break;
				case 0 :
					overall_result[0] = '0';
					overall_result[1] = 0;				
					break;
				case 2 :
					faiz_negative = TRUE;
					Utils_Subtract_Strings(temp_amount_1, arti_result, overall_result);
					break;
				}		

				Slips_FormatAmount(overall_result, formatted_result);
				j = (21 - strlen(formatted_result));
				memcpy(&toprinter[j], formatted_result, strlen(formatted_result));

				if (faiz_negative)
					toprinter[j - 1] = '-';

				toprinter[24] = 0;
				Slips_PrinterStr(toprinter);
				PrntUtil_Cr(1);

				if (faiz_negative)
				{
					if (total_faiz_negative)
						Utils_Add_Strings(overall_result, total_faiz_amount, temp_amount_1);
					else
					{			
						switch (Utils_Compare_Strings(overall_result, total_faiz_amount))
						{
						case 1 :
							total_faiz_negative = TRUE;
							Utils_Subtract_Strings(overall_result, total_faiz_amount, temp_amount_1);
							break;
						case 0 :
							temp_amount_1[0] = '0';
							temp_amount_1[1] = 0;
							break;
						case 2 :
							Utils_Subtract_Strings(total_faiz_amount, overall_result, temp_amount_1);
							break;
						}		
					}
				}
				else
				{
					if (total_faiz_negative)
					{	
						switch (Utils_Compare_Strings(overall_result, total_faiz_amount))
						{
						case 1 :
							total_faiz_negative = FALSE;
							Utils_Subtract_Strings(overall_result, total_faiz_amount, temp_amount_1);
							break;
						case 0 :
							total_faiz_negative = FALSE;
							temp_amount_1[0] = '0';
							temp_amount_1[1] = 0;
							break;
						case 2 :
							Utils_Subtract_Strings(total_faiz_amount, overall_result, temp_amount_1);
							break;
						}		
					}
					else
						Utils_Add_Strings(overall_result, total_faiz_amount, temp_amount_1);
				}
				memcpy(total_faiz_amount, temp_amount_1, strlen(temp_amount_1));
				total_faiz_amount[strlen(temp_amount_1)] = 0;

				memcpy(faiz_char, overall_result, strlen(overall_result));
				faiz_char[strlen(overall_result)] = 0;
				if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
					strcpy( toprinter, "NET                  YTL" );
				else
				strcpy(toprinter, "NET                   TL");

				switch (Utils_Compare_Strings(brut_char, faiz_char))
				{
				case 1 :
					if (brut_negative)
					{
						if (faiz_negative)
							Utils_Subtract_Strings(brut_char, faiz_char, overall_result);
						else
							Utils_Add_Strings(brut_char, faiz_char, overall_result);
						Slips_FormatAmount(overall_result, formatted_result);
						j = (21 - strlen(formatted_result));
						toprinter[j - 1] = '-';
					}
					else
					{
						if (faiz_negative)
							Utils_Add_Strings(brut_char, faiz_char, overall_result);
						else
							Utils_Subtract_Strings(brut_char, faiz_char, overall_result);
						Slips_FormatAmount(overall_result, formatted_result);
						j = (21 - strlen(formatted_result));
					}
					memcpy(&toprinter[j], formatted_result, strlen(formatted_result));
					break;

				case 0 :
					toprinter[20] = '0';
					break;

				case 2 :
					if (faiz_negative)
					{
						if (brut_negative)
							Utils_Subtract_Strings(faiz_char, brut_char, overall_result);
						else
							Utils_Add_Strings(brut_char, faiz_char, overall_result);
						Slips_FormatAmount(overall_result, formatted_result);
						j = (21 - strlen(formatted_result));
					}
					else
					{
						if (brut_negative)
							Utils_Add_Strings(brut_char, faiz_char, overall_result);
						else
							Utils_Subtract_Strings(faiz_char, brut_char, overall_result);
						Slips_FormatAmount(overall_result, formatted_result);
						j = (21 - strlen(formatted_result));
						toprinter[j - 1] = '-';
					}
					memcpy(&toprinter[j], formatted_result, strlen(formatted_result));

					break;			
				}

				toprinter[24] = 0;
				Slips_PrinterStr(toprinter);
				PrntUtil_Cr(1);

			}
			PrntUtil_Cr(1);
		}
	}
	PrntUtil_Cr(1);


	if (credit_transaction_exists)
		strcpy(toprinter, ".......GENEL BRšT.......");
	else
		strcpy(toprinter, ".......GENEL NET........");

	toprinter[24] = 0;
	Slips_PrinterStr(toprinter);

	memset(toprinter, ' ', 24);
	if (net_total[0] == '-')
	{
		memcpy(overall_result, &net_total[1], 12);
		overall_result[12] = 0;
		Slips_FormatAmount(overall_result, formatted_result);
		j = (int) ((22 - strlen(formatted_result))/2);
		toprinter[j - 1] = '-';
	}
	else
	{
		Slips_FormatAmount(net_total, formatted_result);
		j = (int) ((21 - strlen(formatted_result))/2);
	}

	memcpy(&toprinter[j], formatted_result, strlen(formatted_result));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		memcpy( &toprinter[j + strlen( formatted_result )], "YTL", 3 );
	else
		memcpy(&toprinter[j + strlen(formatted_result)], " TL", 3);
	toprinter[24] = 0;
	Slips_PrinterStr(toprinter);

	if (credit_transaction_exists)
	{
		strcpy(toprinter, ".......GENEL FA˜Z.......");

		toprinter[24] = 0;
		Slips_PrinterStr(toprinter);
		memset(toprinter, ' ', 24);

		Slips_FormatAmount(total_faiz_amount, formatted_result);
		if (total_faiz_negative)
		{
			j = (int) ((22 - strlen(formatted_result))/2);
			toprinter[j - 1] = '-';
		}
		else
			j = (int) ((21 - strlen(formatted_result))/2);

		memcpy(&toprinter[j], formatted_result, strlen(formatted_result));
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
			memcpy( &toprinter[j + strlen( formatted_result )], "YTL", 3 );
		else
			memcpy(&toprinter[j + strlen(formatted_result)], " TL", 3);
		toprinter[24] = 0;
		Slips_PrinterStr(toprinter);

		strcpy(toprinter, ".......GENEL NET........");

		toprinter[24] = 0;
		Slips_PrinterStr(toprinter);
		memset(toprinter, ' ', 24);

		if (net_total[0] == '-')
		{
			memcpy(temp_amount_1, &net_total[1], 12);
			temp_amount_1[12] = 0;

			if (total_faiz_negative)
			{
				switch (Utils_Compare_Strings(temp_amount_1, total_faiz_amount))
				{
				case 1 :
					total_faiz_negative = TRUE;
					Utils_Subtract_Strings(temp_amount_1, total_faiz_amount, overall_result);
					break;
				case 0 :
					total_faiz_negative = FALSE;
					temp_amount_1[0] = '0';
					temp_amount_1[1] = 0;
					break;
				case 2 :
					total_faiz_negative = FALSE;
					Utils_Subtract_Strings(total_faiz_amount, temp_amount_1, overall_result);
					break;
				}		
			}
			else
			{
				Utils_Add_Strings(temp_amount_1, total_faiz_amount, overall_result);				
				total_faiz_negative = TRUE;
			}
		}
		else
		{
			if (total_faiz_negative)
			{
				Utils_Add_Strings(net_total, total_faiz_amount, overall_result);				
				total_faiz_negative = FALSE;
			}
			else
			{
				switch (Utils_Compare_Strings(net_total, total_faiz_amount))
				{
				case 1 :
					total_faiz_negative = FALSE;
					Utils_Subtract_Strings(net_total, total_faiz_amount, overall_result);
					break;
				case 0 :
					total_faiz_negative = FALSE;
					temp_amount_1[0] = '0';
					temp_amount_1[1] = 0;
					break;
				case 2 :
					total_faiz_negative = TRUE;
					Utils_Subtract_Strings(total_faiz_amount, net_total, overall_result);
					break;
				}		
			}

		}

		Slips_FormatAmount(overall_result, formatted_result);
		if (total_faiz_negative)
		{
			j = (int) ((22 - strlen(formatted_result))/2);
			toprinter[j - 1] = '-';
		}
		else
			j = (int) ((21 - strlen(formatted_result))/2);

		memcpy(&toprinter[j], formatted_result, strlen(formatted_result));
		if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
			memcpy( &toprinter[j + strlen( formatted_result )], "YTL", 3 );
		else
			memcpy(&toprinter[j + strlen(formatted_result)], " TL", 3);
		toprinter[24] = 0;
		Slips_PrinterStr(toprinter);

	}

	PrntUtil_Cr(1);

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_PrintATSRecords
 *
 * DESCRIPTION                  : Prints the details for ATS transactions
 *
 * RETURN                       : None
 *      
 * NOTES                        : 
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintATSRecords(void)
{
	char toprinter[25];
	char due_date[9], due_amount[length_amount_surcharge+1], 
	     formatted_amount[16];

	/** XLS_INTEGRATION2 08/11/2001. irfan. yy/aa/gg formati gg/aa/yy a cevriliyor **/
	char new_due_date[9];

	sint int_month, int_year;
	sint i, j, lbStatus, lwLastSeq, index;
	sint Handle1;
	boolean    display_transaction, info_displayed, print_minus;
	AmexTaksitStruct fpTransStruct;
	int k;		/* test amacli irfan */

	/* test amacli */
	char lbStr[500];

	if (RamDisk_ChkDsk((byte *)ATS_File) != RD_CHECK_OK)
	{
		strcpy(toprinter, "ATS YIGINI ACILAMADI\n");
		toprinter[21] = 0;
		Slips_PrinterStr(toprinter);
		PrntUtil_Cr(2);
		return;
	}

	Handle1 = RamDisk_Reopen((byte *)ATS_File)->Handle;

	RamDisk_Seek(Handle1, SEEK_FIRST);
	lwLastSeq = RamDisk_Seek(Handle1, SEEK_LAST);

	if (lwLastSeq > 0)
	{
		PrntUtil_Cr(2);
		Slips_PrinterStr("   TAKSITLI SATISLAR\n");
		Slips_PrinterStr("========================");
		PrntUtil_Cr(2);
	}       

	info_displayed = FALSE;

	for (i=0; i < lwLastSeq; i++)   
	{
		display_transaction = FALSE;

		if (RamDisk_Seek(Handle1, i) >= 0)
		{
			lbStatus = RamDisk_Read(Handle1, &fpTransStruct);
			if ((lbStatus >= 0) || (lbStatus == RD_LAST))
				display_transaction = TRUE;
		}

		if (display_transaction)
		{

			if (info_displayed)
			{
				Slips_PrinterStr("------------------------");
				PrntUtil_Cr(2);
			}

			info_displayed = FALSE;

			memcpy(due_date, fpTransStruct.first_date, length_due_date);

			int_month = ((due_date[2] - '0') * 10) + (due_date[3] - '0');
			int_year = ((due_date[0] - '0') * 10) + (due_date[1] - '0');
			due_date[6] = due_date[4];
			due_date[7] = due_date[5];
			due_date[8] = 0;
			due_date[2] = '/';
			due_date[5] = '/';
			/* 04_10 28/02/2003 irfan */
			memset(lbStr, 0, sizeof(lbStr));
			memcpy(lbStr, &fpTransStruct, sizeof(fpTransStruct));


			for (j=0; j<18; j++) /** XLS_PAYMENT 03/08/2001 irfan. **/
			{
				/* 04_10 . toplamlari duzeltmek icin  irfan 28/02/2003*/
				k = 6+(j*13);
				memcpy(due_amount, lbStr+k, length_amount_surcharge);

				/* 04_10 */
				/*memcpy(due_amount, fpTransStruct.payment_amount[j], length_amount_surcharge);*/
				due_amount[length_amount_surcharge] = 0;

				if (due_amount[0] == '-')
				{
					due_amount[0] = '0';
					print_minus = TRUE;
				}
				else
					print_minus = FALSE;

				Utils_ClearInsZeros(due_amount);
				
				if (strlen(due_amount) > 0)
				{
					due_date[0] = (int_year / 10) + '0';
					due_date[1] = (int_year % 10) + '0';
					due_date[3] = (int_month / 10) + '0';
					due_date[4] = (int_month % 10) + '0';
					info_displayed = TRUE;

					if (strlen(due_amount) < 12)
						Slips_FormatAmount(due_amount, formatted_amount);
					else
					{
						memcpy(formatted_amount, due_amount, strlen(due_amount));
						formatted_amount[strlen(due_amount)] = 0;
					}

					memset(toprinter, ' ', 24);

					/** XLS_INTEGRATION2 08/11/2001. irfan. yy/aa/gg formati gg/aa/yy a cevriliyor **/
					/*memcpy(toprinter, due_date, 8);*/
					memset(new_due_date, 0, sizeof(new_due_date));
					new_due_date[0] = due_date[6];
					new_due_date[1] = due_date[7];
					new_due_date[2] = due_date[2];
					new_due_date[3] = due_date[3];
					new_due_date[4] = due_date[4];
					new_due_date[5] = due_date[5];
					new_due_date[6] = due_date[0];
					new_due_date[7] = due_date[1];
					memcpy(toprinter, new_due_date, 8);

					index = (24 - strlen(formatted_amount));
					memcpy(&toprinter[index], formatted_amount, strlen(formatted_amount));
					if (print_minus)
						toprinter[index-1] = '-';

					toprinter[24] = 0;

					Slips_PrinterStr(toprinter);
					PrntUtil_Cr(1);
				}

				int_month++;
				if (int_month > 12)
				{
					int_month = 1;
					int_year++;
					if (int_year > 99)
						int_year = 0;
				}
			}

			if (info_displayed) 
				PrntUtil_Cr(1);
		}
	}

	if (lwLastSeq > 0)
	{
		Slips_PrinterStr("========================");
		PrntUtil_Cr(2);
	}       
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_Report
 *
 * DESCRIPTION                  : Prints the CardTypeTotals for Report
 *
 * RETURN                               : None
 *      
 * NOTES                                : r.i.o. & t.c.a. 02/06/99
 *
 * ------------------------------------------------------------------------ */
byte Slips_Report(boolean IsEOD)
{
	
	Slips_PrintReportHeader(IsEOD);

        if(genius_flag=='0')	Slips_GPrintDetailRecord();

	if(ARA_RAPOR)
	{
		Offline_PrintDetailRecord();		/* 06_30. eger offline islem var ie detay rapor basar */
		Slips_PrintDetailRecord();
	}
	else
	{
		if(!ARA_RAPOR_IPTAL)
		{
			Offline_PrintDetailRecord();	/* 06_30. eger offline islem var ie detay rapor basar */
			Slips_PrintDetailRecord();		
		}
	}

	/* 06_30 akbank offline islemlerin sadece gunsonu icin (emv offline ile ilgisi yok) upload islemi yapiliyor */
	if(!ARA_RAPOR)
	{
		if(Offline_BatchUpload() != STAT_OK) return(STAT_NOK);
	}

	Slips_PrintCardTypeTotals();
	Slips_PrintATSRecords();
	
	PrntUtil_Cr(4);
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Slips_FormatAmountTL
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
void Slips_FormatAmountTL( char *fpAmount, char *fpFJAmount )
{
	char lpAmount[LEN_AMOUNT + 3];	/* LEN_AMOUNT + 1 */
	sint Counter;
	boolean CommaType;

	memcpy( lpAmount, fpAmount, LEN_AMOUNT + 1 );	/* LEN_AMOUNT */
	lpAmount[LEN_AMOUNT + 1] = 0;	/* LEN_AMOUNT */
	Utils_ClearInsZeros( lpAmount );
	for ( Counter = 0; Counter < ( LEN_AMOUNT + 3 ); Counter++ )	/* LEN_AMOUNT + 1 */
		if ( !isdigit( lpAmount[Counter] ) )
		{
			lpAmount[Counter] = 0;
			break;
		}
	CommaType = TRUE;			/* bm FALSE-YTL, TRUE-TL */
	Utils_WithComma( lpAmount, fpFJAmount, Counter, CommaType );	/* bm YTL */
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Slips_LenStr
 *
 * DESCRIPTION: Calculates the length of a string. 
 *
 * RETURN:  fbLen+1    
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
int Slips_LenStr(char *fpStr, int fbLen)
{
	if (fbLen) fbLen--;
	while((fpStr[fbLen]==' ') || (fpStr[fbLen]==0))
		if ( (fbLen--) == 0) return(0);
	return(fbLen+1);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Slips_AdjustAndPrint
 *
 * DESCRIPTION: Adjusts the string to be printed and prints.
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Slips_AdjustAndPrint (char *fpStr1,char *fpStr2,int fbLen2)
{
	char lpLine[24];		
	sint lbLen1, lbLen2;			

	lbLen1 = strlen (fpStr1);
	memset (lpLine,' ',24);
	memcpy (lpLine,fpStr1,lbLen1);		/* String on the left */
	/* Clear right padded spaces from fpStr2 */
	lbLen2 = Slips_LenStr(fpStr2,fbLen2);
	memcpy (lpLine + 24 - lbLen2,fpStr2,lbLen2);
	Slips_Printer (lpLine,24);
}

#if 0
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Slips_PrintParamHeader
 *
 * DESCRIPTION: 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Slips_PrintParamHeader (void)
{
	char toprinter[25];
	char header_date[9], header_time[6];
	char  strVersion[6], Temp[6];

	Utils_IToA (ApplMngr_GetNosVers(), strVersion);
	Utils_LeftPad (strVersion, '0', 4);
	memcpy(Temp, strVersion,2);
	Temp[2] = '.';
	memcpy(Temp+3,strVersion+2,2);
	memcpy(strVersion,Temp,5);
	strVersion[5]=0;

	sprintf(toprinter, "ISYERI NO : %s", merch_no);
	toprinter[24] = 0;
	Slips_PrinterStr(toprinter);

	sprintf(toprinter, "AMEX NO   : %s\n", merch_AMEX_no);
	toprinter[23] = 0;
	Slips_PrinterStr(toprinter);
	
	sprintf(toprinter, "CIHAZ NO  : %s\n", merch_device_no);
	toprinter[21] = 0;
	Slips_PrinterStr(toprinter);

	sprintf(toprinter, "SERI NO   : %c\n", device_serial_no);
	toprinter[14] = 0;
	Slips_PrinterStr(toprinter);
	PrntUtil_Cr(1);

	Slips_AdjustAndPrint("NOS Versiyonu:",strVersion,5);
	strcpy(strVersion,AKB_SW_VERSION);

	memcpy(strVersion,id.appl_ver,APPLICATION_HEADER_VER_SIZE);
	strVersion[APPLICATION_HEADER_VER_SIZE]=0;
	Slips_AdjustAndPrint("SOFT.Versiyonu:",strVersion,5);
	memcpy(header_date,id.date,APPLICATION_HEADER_DATE_SIZE);
	strVersion[APPLICATION_HEADER_DATE_SIZE]=0;

	Slips_AdjustAndPrint("SOFT.Tarihi:",header_date,8);

	PrntUtil_Cr(1);

	PrntUtil_Line();

	PrntUtil_Cr(1);
}
#endif

/****************** GENIUS FUNCTIONS *********************/
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GFillErrorReceiptStructure.
 *
 * DESCRIPTION                  : Fill the receipt structure
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o.  02/10/2002
 *
 * ------------------------------------------------------------------------ */
boolean Slips_GFillErrorReceiptStructure(void)
{
	BizSlip = TRUE; //@bm 15.12.2004

	Slips_GFillCommonReceiptStructure();

	memcpy(d_GReceipt.message,d_GTransaction_Receive.message,16);
	memcpy(&d_GReceipt.message[17], d_GTransaction_Receive.confirmation,3);
	d_GReceipt.message[20] = 0;

	/* @bm 09.12.2004 get bayii_adi*/	
	memset(d_GReceipt.bayii_adi,' ',24);
	d_GReceipt.bayii_adi[24] = 0;

	return(TRUE);
}

 /* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GFillCommanReceiptStructure.
 *
 * DESCRIPTION                  : Fill the common receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. 02.10.2002
 *
 * ------------------------------------------------------------------------ */
boolean Slips_GFillCommonReceiptStructure(void)
{
	int number_of_zeros,Status;	
	char amount_temp[25];           /* temp area to format amount values */
	char received_amount[16];       /*received amount_surcharge as char*/
	char surcharge[14];             /* surcharge = received_amount - send_amount*/
	char formatted_card_no[24];
	char formatted_exp_date[6];
	char device_date[8], device_time[5];
	int i,minint;								/* ATS related variables */
	char temp_name[55];							/* 02_02 irfan 15/08/2000  isim ve soyismi tutuyor */

	/* fill d_Receipt structure by space and initialize flags */
	memset(&d_GReceipt, ' ', sizeof(d_GReceipt));

	d_GReceipt.tran_type = Gtran_type;
	d_GReceipt.R_VOID = GVOID_FLAG;

	memcpy(d_GReceipt.term_no, merch_device_no, 8);
	d_GReceipt.term_no[8] = 0;	

	/* get date and time and format them */
	Utils_GetDateTime(device_date,device_time);

	memcpy(d_GReceipt.date,device_date,8);
	d_GReceipt.date[8]=0;

	memcpy(d_GReceipt.time,device_time,5);
	d_GReceipt.time[5] = 0;

	/* Formatting and filling the card no and exp date */
	Slips_FormatCardExp(formatted_card_no, formatted_exp_date, card_no, exp_date);
	memcpy(d_GReceipt.card_no, formatted_card_no, strlen(formatted_card_no));
	d_GReceipt.card_no[strlen(formatted_card_no)] = 0;

	memcpy(d_GReceipt.exp_date, formatted_exp_date, strlen(formatted_exp_date));
	d_GReceipt.exp_date[strlen(formatted_exp_date)] = 0;

	/* set input type */
	if (input_type == '1' )
		d_GReceipt.input_type = 'T';
	else if  (input_type == '2' )
		d_GReceipt.input_type = 'D';
	else
	{
		Mainloop_DisplayWait("G˜R˜ž T˜P˜ HATASI",1);
		d_GReceipt.input_type = 'X';
	}

	/*ref no: batch no + seq no */
	memcpy(d_GReceipt.ref_no,batch_no_c,3);
	d_GReceipt.ref_no[3]='/';

	if(!GVOID_FLAG)
	{
		memcpy(&d_GReceipt.ref_no[4],genius_seq_no_c,4);
		d_GReceipt.ref_no[8]=0;
	}
	else
	{	/* iptal islemi icin batch den okunan ve send mesajina konulan deger aliniyor */
		memcpy(&d_GReceipt.ref_no[4],d_GTransaction_Send.transaction_seq_no,4);
		d_GReceipt.ref_no[8]=0;
	}

	PanLength = strlen(card_no);	// @bm 13.12.2004	

	/* 02_02 irfan 15/09/2000 2. kopya icin basýlan bilgierin kaybolmamasýný saglar*/
	if(ISIMYAZMA_FLAG && TRACK1ERROR)
	{
		if(d_GReceipt.input_type == 'D')
		{

			memset(temp_name, 0 , sizeof(temp_name));
			sprintf(temp_name, "%s %s", card_info1_name, card_info1_surname);
			temp_name[strlen(card_info1_name) + strlen(card_info1_surname)+1] = 0;
			minint=min(24,strlen(temp_name));
			memcpy(d_Receipt.Card_Holder_Name, temp_name,minint);
			memcpy(d_GReceipt.Card_Holder_Name, temp_name, minint);

			/* 04_09 23/12/2002 transaction slibindeki isim yazma problemi giderildi. */
/*			memcpy(&d_GReceipt.Card_Holder_Name, &d_GSorgulamaReceipt.Card_Holder_Name, 24);*/
			
			d_GReceipt.Card_Holder_Name[24] = 0; /* ends with null */
		}
	}

	/* get sorgulama veya islem tutar */
	if (!GVOID_FLAG)
	{
		memcpy(received_amount, d_GTransaction_Receive.sorgulama_tutar,13);
	}
	else
	{
		memcpy(received_amount, d_GTransaction_Send.sorgulama_tutar,13);
	}
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);

	/* format the amount*/
/*
	Slips_FormatAmount(received_amount,amount_temp);
*/
{
	char lpAmount [LEN_AMOUNT+1];
	sint Counter;
	boolean CommaType;

	memcpy(lpAmount,received_amount,LEN_AMOUNT);
	lpAmount[LEN_AMOUNT]= 0;
	Utils_ClearInsZeros(lpAmount);
	for (Counter=0;Counter<(LEN_AMOUNT+1);Counter++)
		if (!isdigit(lpAmount[Counter])) 
		{
			lpAmount[Counter]=0;
			break;
		}
	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
		CommaType = TRUE;
	else if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		CommaType = FALSE;
	Utils_WithComma( lpAmount, amount_temp, Counter, CommaType );
}

	number_of_zeros = (int)((21-strlen(amount_temp))/2);

	memcpy(&d_GReceipt.sorgulama_tutar[number_of_zeros],amount_temp,strlen(amount_temp));

	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GReceipt.sorgulama_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GReceipt.sorgulama_tutar[number_of_zeros+strlen(amount_temp)]," TL");

	d_GReceipt.sorgulama_tutar[24]=0;

	/* get garanti_tutar tutar */
	memcpy(received_amount, d_GTransaction_Receive.garanti_tutar,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);

	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);

	memcpy(&d_GReceipt.garanti_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GReceipt.garanti_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GReceipt.garanti_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GReceipt.garanti_tutar[24]=0;

	/* get garanti_asim_tutari */
	memcpy(received_amount, d_GTransaction_Receive.garanti_asim_tutari,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);

	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);

	memcpy(&d_GReceipt.garanti_asim_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GReceipt.garanti_asim_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GReceipt.garanti_asim_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GReceipt.garanti_asim_tutar[24]=0;

	return (TRUE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GFillReceiptStructure.
 *
 * DESCRIPTION                  : Fill the genius receipt structure
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. 03/10/02
 *
 * ------------------------------------------------------------------------ */
boolean Slips_GFillReceiptStructure(void)
{
	char received_amount[14];
	char amount_temp[25];
	int number_of_zeros,Status;

	BizSlip = TRUE; //@bm 15.12.2004

	if(!Slips_GFillCommonReceiptStructure())
	{
		Mainloop_DisplayWait("SLIP HATASI",2);   /* DEÐÝÞECEK MESAJ */
		return(FALSE);
	}


	/* get sorgulama veya islem tutar */
	if (!GVOID_FLAG)
	{
		memcpy(received_amount, d_GTransaction_Receive.sorgulama_tutar,13);
	}
	else
	{
		memcpy(received_amount, d_GTransaction_Send.sorgulama_tutar,13);
	}
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);

	
	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);

	memcpy(&d_GReceipt.sorgulama_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GReceipt.sorgulama_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GReceipt.sorgulama_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GReceipt.sorgulama_tutar[24]=0;


	/* get garanti_tutar tutar */
	memcpy(received_amount, d_GTransaction_Receive.garanti_tutar,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);

	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GReceipt.garanti_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GReceipt.garanti_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GReceipt.garanti_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GReceipt.garanti_tutar[24]=0;

	/* get garanti_asim_tutari */
	memcpy(received_amount, d_GTransaction_Receive.garanti_asim_tutari,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);

	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GReceipt.garanti_asim_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GReceipt.garanti_asim_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GReceipt.garanti_asim_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GReceipt.garanti_asim_tutar[24]=0;


	/* get approval code */
	if (!d_GReceipt.R_VOID) 
	{
		memcpy(d_GReceipt.approval_code, d_GTransaction_Receive.approval_code, 6);
	}
	else
	{
		memcpy(d_GReceipt.approval_code, d_GTransaction_Send.approval_code, 6);
	}

	d_GReceipt.approval_code[6] = 0;
		
	/* get bayii_no*/
	memcpy(d_GReceipt.bayii_no, d_GTransaction_Receive.bayii_no,7);
	d_GReceipt.bayii_no[7] = 0;

	/* get bayii_adi*/	/* 04_04 22/11/2002 */
	memcpy(d_GReceipt.bayii_adi, d_GTransaction_Receive.bayii_adi,24);
	d_GReceipt.bayii_adi[24] = 0;

	/* get belge_no*/
	memcpy(d_GReceipt.belge_no, d_GTransaction_Send.belge_no,10);
	d_GReceipt.belge_no[10] = 0;

	/* get islem_komisyon_orani*/
	memcpy(d_GReceipt.islem_komisyon_orani, d_GTransaction_Receive.islem_komisyon_orani,5);
	d_GReceipt.islem_komisyon_orani[5] = 0;

	/* get garanti_komisyon_orani*/
	memcpy(d_GReceipt.garanti_komisyon_orani, d_GTransaction_Receive.garanti_komisyon_orani,5);
	d_GReceipt.garanti_komisyon_orani[5] = 0;

	PanLength = strlen(card_no);	// @bm 13.12.2004	

	return(TRUE);

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GPrintReceipt
 *
 * DESCRIPTION                  : Print the receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 20/05/99
 *
 * ------------------------------------------------------------------------ */
void Slips_GPrintReceipt (void)
{
	char temp_approvalcode[7];
	Tarih_Vade lbTarih_Vade[12];
	int lvade_sayisi;
	char lvade_sayisi_c[3];
	char lbvade_tarih_string[300];
	
	BizSlip = TRUE; //@bm 15.12.2004

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

	
	Slips_GPrintCommonReceipt();

	PrntUtil_Cr(1);

	/* 04_10 garanti tutarý iptal slibinden çýkartýldý. */	
	if (!d_GReceipt.R_VOID) 
	{
		/* 04_08 A.Y. 19/12/2002 pesin satis icin garanti tutarlari kalkti */
		if (Gtran_type != GENIUS_SATIS)
		{
			/* garanti tutarý */
			Slips_PrinterStr("\n---- KREDILI TUTAR -----\n"); /* ver_04_12 */
			Slips_PrinterStr(d_GReceipt.garanti_tutar);
			PrntUtil_Cr(2);
		}
	}

	if (!d_GReceipt.R_VOID) 
	{
		/* 04_08 A.Y. 19/12/2002 pesin satis icin garanti tutarlari kalkti */
		if (Gtran_type != GENIUS_SATIS)
		{
			/* garanti aþým tutarý */
			Slips_PrinterStr("--- KREDISIZ TUTAR -----\n");	/* ver_04_12 */
			Slips_PrinterStr(d_GReceipt.garanti_asim_tutar);
		}

		PrntUtil_Cr(1);

		/* iþlem komisyon oraný*/
		/* 04_09 24/12/2002 talep uzerine kapatildi. */
		/*memset(msg, 0, sizeof(msg));
		sprintf(msg, "ISLEM KOMIS.ORANI: %s\n", d_GReceipt.islem_komisyon_orani);
		msg[24] = 0;
		Slips_PrinterStr(msg);*/

		/* garanti komisyon oraný*/
		/* 04_09 24/12/2002 talep uzerine kapatildi. */
		/*if (Gtran_type == GENIUS_VADELI)
		{		
			memset(msg, 0, sizeof(msg));
			sprintf(msg, "GARANTI.KMS.ORANI: %s\n\n", d_GReceipt.garanti_komisyon_orani);
			msg[24] = 0;
			Slips_PrinterStr(msg);
		}
		PrntUtil_Cr(2);*/

		memcpy(msg,MSGSlipFixed1,24);
		msg[24]=0;
		Slips_PrinterStr(msg);
		PrntUtil_Cr(1); 
		
		/* 04_04 22/11/2002 imza kýsmýnýn onune alýndý. */
		/* genius specific text */
		if(mev_hes_kodu == '1')
		{
			Slips_PrinterStr("DIREKT OLARAK KREDI\n");
			Slips_PrinterStr("HESABIMIN DEVREYE\n");
			Slips_PrinterStr("GIRMESINI KABUL ETTIM\n\n");
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

	}


	/* approval code */
	memset(temp_approvalcode, 0, 7);
	memcpy(temp_approvalcode, d_GReceipt.approval_code, 6);
	temp_approvalcode[6] = 0;


	sprintf(msg, "PROV : %s 0000460000", temp_approvalcode);
	msg[24]=0;
	Slips_PrinterStr(msg);

	memcpy(msg,MSGSlipFixed3,24);
	msg[24]=0;
	Slips_PrinterStr(msg);
	PrntUtil_Cr(1); 

	if(SECOND_COPY || d_Receipt.R_VOID )
	{
		Utils_PrintImage((byte *)aklogoPrint, 0, 128, 5);
	}else 	PrntUtil_Cr(1); 


	Printer_WaitForTextReady ();


	if (!d_GReceipt.R_VOID) 
	{
		/* Vade tarihi ve meblaglari alinmasi */
		memset(lvade_sayisi_c,0,sizeof(lvade_sayisi));
		memcpy(lvade_sayisi_c, d_GTransaction_Send.vade_sayisi,2);

		memset(lbvade_tarih_string, 0, sizeof(lbvade_tarih_string));
		memcpy(lbvade_tarih_string, d_GTransaction_Send.Gtarih_vade, sizeof(lbTarih_Vade));

		/* vade sayisi integer'a donusturuluyor */
		lvade_sayisi = atoi(lvade_sayisi_c);

		/*if(lvade_sayisi > 1 )*/
		/* eger vadeli iþlem ise */
		if(d_GTransaction_Receive.GHeader_Transact.MSG_TYPE == '2' )
		{
			Slips_GPrint_VadeTarih(lvade_sayisi, lbvade_tarih_string);
		}
	}

	PrntUtil_Cr(6); 
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GPrintCommonReceipt
 *
 * DESCRIPTION                  : Print the receipt structure
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 20/05/99
 *
 * ------------------------------------------------------------------------ */
void Slips_GPrintCommonReceipt (void)
{
	char * GTran_Types[] = { NULL, "G.SATIž", "V.SATIž", "G.IPTAL", "V.IPTAL"};
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
	sprintf(msg, "˜žYER˜  : %s\n", merch_no);
	msg[24] = 0;
	Slips_PrinterStr(msg);

	/* GRAFPOS terminal no 8 digit basiliyor */
	memset (msg, 0, sizeof(msg));
	merch_device_no[8] = 0;
	sprintf( msg,"CIHAZ NO: %s\n", merch_device_no);
	msg[24]=0;
	Slips_PrinterStr(msg);

	/* Bayi no 7 digit basiliyor */
	memset(msg, 0, sizeof(msg));
	sprintf( msg,"BAYII NO: %s\n", d_GReceipt.bayii_no);
	msg[24]=0;
	Slips_PrinterStr(msg);

	/* Bayi adi 24 digit basiliyor */		/* 04_04 22/11/2002 */
	Slips_PrinterStr(d_GReceipt.bayii_adi);


	/* DATE , TIME and transaction type */
	sprintf(msg, "%s %s %s\n\n", d_GReceipt.date, d_GReceipt.time, GTran_Types[d_GReceipt.tran_type]);
	msg[24] = 0;
	Slips_PrinterStr(msg);  /* sonuna null koymak gerekebilir */
	
/* formatted card no */
/*	Slips_PrinterStr(d_GReceipt.card_no);
	msg[24] = 0;
	Slips_PrinterStr("\n");
*/
	PanLength = strlen(card_no);	// @bm 13.12.2004	

	if(!SECOND_COPY && !(d_GReceipt.tran_type==6)) /*MK  iptal ve müþteri slibi*/
	{  	/* formatted card no */

		memset(masked_card_no, 0, sizeof(masked_card_no));
		strcpy(masked_card_no, d_GReceipt.card_no);
		if ( PanLength == 0xF )
			memcpy(&masked_card_no[4], " **** ***",9);
		else if ( PanLength == 0x12 )
			memcpy(&masked_card_no[4], " **** **** **",13);
		else if ( PanLength == 0x13 )
			memcpy(&masked_card_no[4], " **** **** ***",14);
		else
			memcpy(&masked_card_no[4], " **** ****",10);
		Slips_PrinterStr(masked_card_no);
		Slips_PrinterStr("\n");

	}
	else
	{
		memset(masked_card_no, 0, sizeof(masked_card_no));
		strcpy(masked_card_no, d_GReceipt.card_no);
		if ( PanLength == 0xF )  		 /*15*/
			memcpy(masked_card_no, "**** **** ***", 13);
		else if ( PanLength == 0x12 )
			memcpy(masked_card_no, "**** **** **** **",17);
		else if ( PanLength == 0x13 )	 /*19*/
			memcpy(masked_card_no, "**** **** **** ***", 18); 
		else							 /*16*/
			memcpy(masked_card_no, "**** **** ****", 14); 

		Slips_PrinterStr(masked_card_no);
		Slips_PrinterStr("\n");

/*		memset(masked_card_no, 0, sizeof(masked_card_no));
		strcpy(masked_card_no, d_GReceipt.card_no);
		if(AMEX_FLAG)
			memcpy(masked_card_no+4, " **** ***", 9);
		else
			memcpy(masked_card_no+4, " **** ****", 10);
		masked_card_no[24] = 0;
		Slips_PrinterStr(masked_card_no);
		Slips_PrinterStr("\n");
*/
	}


	if(ISIMYAZMA_FLAG && TRACK1ERROR)
	{
		if(d_GReceipt.input_type == 'D')
		{
			memset(msg, 0, sizeof(msg));
			memcpy(msg,d_GReceipt.Card_Holder_Name,24);
			msg[24]=0;
			Slips_PrinterStr(msg);

/*			Slips_PrinterStr(d_GReceipt.Card_Holder_Name);*/
		}
	}

	
	PrntUtil_Cr(2);	
	
	/* formatted exparation date */
	if(!SECOND_COPY)
	{
		sprintf(msg, "GE€. TAR.: %s\n", d_GReceipt.exp_date);
		msg[24] = 0;
		Slips_PrinterStr(msg);
	}
	

	/* ref no: batch_no_c + '/' +seq_no_c */
	sprintf(msg, "REF NO   : %s  (%c)\n", d_GReceipt.ref_no, d_GReceipt.input_type);
	msg[24] = 0;
	Slips_PrinterStr(msg);
	PrntUtil_Cr(1);

	/* belge no */
	sprintf(msg, "BELGE NO : %s\n\n", d_GReceipt.belge_no);
	msg[24] = 0;
	Slips_PrinterStr(msg);

	/* sorgulama tutarý */
	Slips_PrinterStr("\n----- ISLEM TUTARI -----");
	Slips_PrinterStr(d_GReceipt.sorgulama_tutar);
	Slips_PrinterStr("\n");
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GPrintErrorReceipt.
 *
 * DESCRIPTION                  : Fill the receipt structure
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. 03/10/2002
 *
 * ------------------------------------------------------------------------ */
void Slips_GPrintErrorReceipt(void)
{

	char * GTran_Types[] = { NULL, "G.SATIž", "V.SATIž", "G.IPTAL", "V.IPTAL"};

	char masked_card_no[24];

	BizSlip = TRUE; //@bm 15.12.2004
	Slips_GPrintCommonReceipt();

	PrntUtil_Cr(1);                 

	/* 04_08 A.Y. 19/12/2002 pesin satis icin garanti tutarlari kalkti */
	if (Gtran_type != GENIUS_SATIS)
	{

		/* garanti tutarý */
		Slips_PrinterStr("\n---- KREDILI TUTAR -----\n");	/* ver_04_12 */
		Slips_PrinterStr(d_GReceipt.garanti_tutar);
		PrntUtil_Cr(2);

		/* garanti aþým tutarý */
		Slips_PrinterStr("--- KREDISIZ  TUTAR ----\n");	/* ver_04_12 */
		Slips_PrinterStr(d_GReceipt.garanti_asim_tutar);
	}

	PrntUtil_Cr(2);

	/* PRINT THE RECEIVED MESSAGE FROM HOST */
	sprintf(msg,"  %s  ",d_GReceipt.message);
	msg[24]=0;
	PrntUtil_Cr(1);
	Slips_PrinterStr(msg);

	if (!d_GReceipt.R_VOID) 
	{

		if (memcmp(d_GTransaction_Receive.confirmation, "888", 3) == 0)
		{	/* hatali tarihlerin basilmasi*/
			PrntUtil_Cr(2);
			Slips_GPrintHataliTarihler();
		}
	}

	PrntUtil_Cr(6);

	Mte_Wait(800);
	Kbhw_BuzzerPulse(800);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GFillSorgulamaReceiptStructure.
 *
 * DESCRIPTION                  : Fill the genius sorgulama receipt structure
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. 03/10/02
 *
 * ------------------------------------------------------------------------ */
boolean Slips_GFillSorgulamaReceiptStructure(void)
{
	char amount_temp[25];
	int number_of_zeros,Status;
	char received_amount[14];                       /*received amount_surcharge as char*/
	char surcharge[14];                                     /* surcharge = received_amount - send_amount*/
	char formatted_card_no[24];
	char formatted_exp_date[6];
	char device_date[8], device_time[5];
	int i;								/* ATS related variables */
	char temp_name[25];					/* 02_02 irfan 15/08/2000 isim ve soyismi tutuyor */

	BizSlip = TRUE; //@bm 15.12.2004

	/* fill d_Receipt structure by space and initialize flags */
	memset(&d_GSorgulamaReceipt, ' ', sizeof(d_GSorgulamaReceipt));

	d_GSorgulamaReceipt.tran_type = Gtran_type;

	memcpy(d_GSorgulamaReceipt.term_no, merch_device_no, 8);
	d_GSorgulamaReceipt.term_no[8] = 0;
	
	/* get date and time and format them */
	Utils_GetDateTime(device_date,device_time);

	memcpy(d_GSorgulamaReceipt.date,device_date,8);
	d_GSorgulamaReceipt.date[8]=0;

	memcpy(d_GSorgulamaReceipt.time,device_time,5);
	d_GSorgulamaReceipt.time[5] = 0;

	PanLength = strlen(card_no);	// @bm 13.12.2004	

	/* Formatting and filling the card no and exp date */
	Slips_FormatCardExp(formatted_card_no, formatted_exp_date, card_no, exp_date);
	memcpy(d_GSorgulamaReceipt.card_no, formatted_card_no, strlen(formatted_card_no));
	d_GSorgulamaReceipt.card_no[strlen(formatted_card_no)] = 0;

	memcpy(d_GSorgulamaReceipt.exp_date, formatted_exp_date, strlen(formatted_exp_date));
	d_GSorgulamaReceipt.exp_date[strlen(formatted_exp_date)] = 0;

	/* set input type */
	if (input_type == '1' )
		d_GSorgulamaReceipt.input_type = 'T';
	else if  (input_type == '2' )
		d_GSorgulamaReceipt.input_type = 'D';
	else
	{
		Mainloop_DisplayWait("G˜R˜ž T˜P˜ HATASI",1);
		d_GSorgulamaReceipt.input_type = 'X';
	}

	/*ref no: batch no + seq no */
	
	memcpy(d_GSorgulamaReceipt.ref_no,batch_no_c,3);
	d_GSorgulamaReceipt.ref_no[3]='/';

	memcpy(&d_GSorgulamaReceipt.ref_no[4],genius_seq_no_c,4);
	d_GSorgulamaReceipt.ref_no[8]=0;

	/* 02_02 irfan 15/09/2000 2. kopya icin basýlan bilgierin kaybolmamasýný saglar*/

	if(ISIMYAZMA_FLAG && TRACK1ERROR)
	{
		if(d_GSorgulamaReceipt.input_type == 'D')
		{
			memset(temp_name, 0 , sizeof(temp_name));
			sprintf(temp_name, "%s %s", card_info1_name, card_info1_surname);
			temp_name[strlen(card_info1_name) + strlen(card_info1_surname)+1] = 0;
			memcpy(d_GSorgulamaReceipt.Card_Holder_Name, temp_name, strlen(temp_name));
			d_GSorgulamaReceipt.Card_Holder_Name[24] = 0; /* ends with null */

		}
	}


	/* get sorgulama tutar */
	memcpy(received_amount, d_GTransaction_Receive.sorgulama_tutar,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);


	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GSorgulamaReceipt.sorgulama_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GSorgulamaReceipt.sorgulama_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GSorgulamaReceipt.sorgulama_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GSorgulamaReceipt.sorgulama_tutar[24]=0;

	/* get garanti_tutar tutar */
	memcpy(received_amount, d_GTransaction_Receive.garanti_tutar,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);

	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GSorgulamaReceipt.garanti_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GSorgulamaReceipt.garanti_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GSorgulamaReceipt.garanti_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GSorgulamaReceipt.garanti_tutar[24]=0;

	/* get garanti_asim_tutari */
	memcpy(received_amount, d_GTransaction_Receive.garanti_asim_tutari,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);

	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GSorgulamaReceipt.garanti_asim_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GSorgulamaReceipt.garanti_asim_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GSorgulamaReceipt.garanti_asim_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GSorgulamaReceipt.garanti_asim_tutar[24]=0;

	
	/* get bayii_no*/
	memcpy(d_GSorgulamaReceipt.bayii_no, d_GTransaction_Receive.bayii_no,7);
	d_GSorgulamaReceipt.bayii_no[7] = 0;

	/* get bayii_no*/ /* 04_04 22/11/2002 */
	memcpy(d_GSorgulamaReceipt.bayii_adi, d_GTransaction_Receive.bayii_adi,24);
	d_GSorgulamaReceipt.bayii_adi[24] = 0;

	/* get belge_no*/
	memcpy(d_GSorgulamaReceipt.belge_no, d_GTransaction_Send.belge_no,10);
	d_GSorgulamaReceipt.belge_no[10] = 0;

	/* get islem_komisyon_orani*/
	memcpy(d_GSorgulamaReceipt.islem_komisyon_orani, d_GTransaction_Receive.islem_komisyon_orani,5);
	d_GSorgulamaReceipt.islem_komisyon_orani[5] = 0;

	/* get garanti_komisyon_orani*/
	memcpy(d_GSorgulamaReceipt.garanti_komisyon_orani, d_GTransaction_Receive.garanti_komisyon_orani,5);
	d_GSorgulamaReceipt.garanti_komisyon_orani[5] = 0;

	return(TRUE);

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GPrintSorgulama_Slip.
 *
 * DESCRIPTION                  : Print the Sorgulama Slip
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. 08/10/2002
 *
 * ------------------------------------------------------------------------ */
void Slips_GPrintSorgulama_Slip(void)
{
	char masked_card_no[24]; 	/* bm  13.12.2004 */
	char * GTran_Types[] = { NULL, "G.SATIž", "V.SATIž", "G.IPTAL", "V.IPTAL"};
	char temp_approvalcode[7];

	BizSlip = TRUE; //@bm 15.12.2004

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
	sprintf(msg, "˜žYER˜  : %s\n", merch_no);
	msg[24] = 0;
	Slips_PrinterStr(msg);


	/* GRAFPOS terminal no 8 digit basiliyor */
	memset (msg, 0, sizeof(msg));
	merch_device_no[8] = 0;
	sprintf( msg,"CIHAZ NO: %s\n", merch_device_no);
	msg[24]=0;
	Slips_PrinterStr(msg);


	/* Bayi no 7 digit basiliyor */
	memset(msg, 0, sizeof(msg));
	sprintf( msg,"BAYII NO: %s\n", d_GSorgulamaReceipt.bayii_no);
	msg[24]=0;
	Slips_PrinterStr(msg);

	/* Bayi adi 24 digit basiliyor */
	Slips_PrinterStr(d_GSorgulamaReceipt.bayii_adi);
	

	/* DATE , TIME and transaction type */
	sprintf(msg, "%s %s %s\n\n", d_GSorgulamaReceipt.date, d_GSorgulamaReceipt.time, GTran_Types[d_GSorgulamaReceipt.tran_type]);
	msg[24] = 0;
	Slips_PrinterStr(msg);  /* sonuna null koymak gerekebilir */

	
	/* formatted card no */
/* bm gec 13.12.2004
	Slips_PrinterStr(d_GSorgulamaReceipt.card_no);
	msg[24] = 0;
	Slips_PrinterStr("\n");
*/

//	Printer_WriteStr("\nbm gec01\n");
//	PrntUtil_BufAsciiHex((byte *)d_GSorgulamaReceipt.card_no,24);

	memset( masked_card_no, 0, sizeof( masked_card_no ) );
	strcpy( masked_card_no, d_GSorgulamaReceipt.card_no );

//	PanLength = strlen(masked_card_no);	
	PanLength = strlen(card_no);	// @bm 13.12.2004	

	if ( PanLength == 0xF )
		memcpy(&masked_card_no[4], " **** ***",9);
	else if ( PanLength == 0x12 )
		memcpy(&masked_card_no[4], " **** **** **",13);
	else if ( PanLength == 0x13 )
		memcpy(&masked_card_no[4], " **** **** ***",14);
	else
		memcpy(&masked_card_no[4], " **** ****",10);

	
	Slips_PrinterStr(masked_card_no);
	Slips_PrinterStr("\n");

	if(ISIMYAZMA_FLAG && TRACK1ERROR)
	{
		if(d_GReceipt.input_type == 'D')
		{
			Slips_PrinterStr(d_GSorgulamaReceipt.Card_Holder_Name);
		}
	}

	
	PrntUtil_Cr(2);	
	
	/* formatted exparation date */
	if(!SECOND_COPY)
	{
		sprintf(msg, "GE€. TAR.: %s\n", d_GSorgulamaReceipt.exp_date);
		msg[24] = 0;
		Slips_PrinterStr(msg);
	}
	
	/* ref no: batch_no_c + '/' +seq_no_c */
	sprintf(msg, "REF NO   : %s  (%c)\n", d_GSorgulamaReceipt.ref_no, d_GSorgulamaReceipt.input_type);
	msg[24] = 0;
	Slips_PrinterStr(msg);
	PrntUtil_Cr(1);

	/* belge no */
	sprintf(msg, "BELGE NO : %s\n\n", d_GSorgulamaReceipt.belge_no);
	msg[24] = 0;
	Slips_PrinterStr(msg);

	/* sorgulama tutarý */
	Slips_PrinterStr("--- SORGULAMA TUTARI ---\n");
	Slips_PrinterStr(d_GSorgulamaReceipt.sorgulama_tutar);
	Slips_PrinterStr("\n\n");

	PrntUtil_Cr(1);	
	
	/* 04_08 A.Y. 19/12/2002 pesin satis icin garanti tutarlari kalkti */
	if (Gtran_type != GENIUS_SATIS)
	{
		/* garanti tutarý */
		Slips_PrinterStr("---- KREDILI TUTAR -----\n");		/* ver_04_12 */
		Slips_PrinterStr(d_GSorgulamaReceipt.garanti_tutar);
		PrntUtil_Cr(2);
	}

	/* garanti aþým tutarý */
	/*Slips_PrinterStr(d_GSorgulamaReceipt.garanti_asim_tutar);
	PrntUtil_Cr(1);*/

	/* iþlem komisyon oraný*/
	/* 04_04 22/11/2002. Talep uzerine kaldýrýldý */
	/*memset(msg, 0, sizeof(msg));
	sprintf(msg, "ISLEM KOMIS.ORANI: %s\n", d_GSorgulamaReceipt.islem_komisyon_orani);
	msg[24] = 0;
	Slips_PrinterStr(msg);*/

	/* garanti komisyon oraný*/
	/* 04_04 22/11/2002. Talep uzerine kaldýrýldý */
	/*memset(msg, 0, sizeof(msg));
	sprintf(msg, "GARANTI.KMS.ORANI: %s\n", d_GSorgulamaReceipt.garanti_komisyon_orani);
	msg[24] = 0;
	Slips_PrinterStr(msg);*/

		
	PrntUtil_Cr(4); 
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GPrintDetailRecord
 *
 * DESCRIPTION                  : Prints the genius details for Report
 *
 * RETURN                       : None
 *      
 * NOTES                        : r.i.o.  10/10/02
 *
 * ------------------------------------------------------------------------ */
void Slips_GPrintDetailRecord(void)
{
	char toprinter[25];
	char index;
	sint i, j, lbStatus, lwLastSeq;
	char input_card_no[20], output_card_no[25], 
	     output_exp_date[6], card_name[5],
		 temporary_string1[14], temporary_string2[9], temporary_string3[6], lb_belge_no[11],lb_bayi_no[8];
	char formatted_amount[25];
			 
	GTransactionStruct fpGTransStruct;
	Tarih_Vade lbTarih_Vade[12];
	sint handle1;
	boolean display_transaction;
	char * GTran_Types[] = { NULL, "GSAT", "VAD.", "IPT.", "NULL"};
	int	lvade_sayisi;
	char lbvade_tarih_string[300];

	/* 04_15 13/06/2003 */
	char total_GT1_amt[14];
	char total_GT2_amt[14];
	int temp_number_of_zeros;
	char temp_str[25];
	char temp_total[14];


	strcpy(toprinter, "BIZ.CARD SLIP BILGILERI\n");	/* 04_10 GENIUS, yerine BIZ.CARD yapýldý. 03/03/2003 */
	toprinter[23] = 0;
	Slips_PrinterStr(toprinter);

	strcpy(toprinter, " ---------------------\n");
	toprinter[23] = 0;
	Slips_PrinterStr(toprinter);

	PrntUtil_Cr(2);
	
	/* Display Transactions */
	if (RamDisk_ChkDsk((byte *)Transaction_GFile) != RD_CHECK_OK)
	{
		strcpy(toprinter, "BIZCARD YIG.ACAMADI\n");		/* 04_13 GENÝUS ifadesi Biz card oldu  15/05/2003 */
		toprinter[20] = 0;
		Slips_PrinterStr(toprinter);
		PrntUtil_Cr(2);
		return;
	}

	handle1 = RamDisk_Reopen((byte *)Transaction_GFile)->Handle;

	lwLastSeq = RamDisk_Seek(handle1,SEEK_LAST);
	RamDisk_Seek(handle1, SEEK_FIRST);

	for (i=0; i < lwLastSeq; i++)   
	{
		lbStatus = RamDisk_Read(handle1, &fpGTransStruct);
		if ((lbStatus >= 0) || (lbStatus == RD_LAST))
			display_transaction = TRUE;
		else
			display_transaction = FALSE;

		if (display_transaction)
		{
			/*index = fpGTransStruct.tran_type - '0';*/
			index = fpGTransStruct.MSG_TYPE - '0';

			memcpy(temporary_string1, fpGTransStruct.seq_no, 4);
			temporary_string1[4] = 0;
			memcpy(temporary_string2, fpGTransStruct.date, 8);
			temporary_string2[8] = 0;
			memcpy(temporary_string3, fpGTransStruct.time, 5);
			temporary_string3[5] = 0;
		
			sprintf(toprinter, "%s %s %s %s", temporary_string1, GTran_Types[index], temporary_string2, temporary_string3);
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			memcpy(input_card_no, fpGTransStruct.card_no, 19);
			input_card_no[19] = 0;

			for (j=12; j<19; j++)
			{
				if (input_card_no[j] == ' ')
					input_card_no[j] = 0;
			}

			Slips_FormatCardExp(output_card_no, output_exp_date,input_card_no,  "1111");

			sprintf(toprinter, "%s", output_card_no);
			toprinter[strlen(output_card_no)] = 0;
			Slips_PrinterStr(toprinter);
			PrntUtil_Cr(1);

			/* fix short name for genius kart */
			memcpy(card_name, "GNUS", 4);
			card_name[4] = 0;
/*
			memcpy(temporary_string1, fpGTransStruct.sorgulama_tutar, 13);
*/
			/*@bm 26.08.2004 YTL POS'un raporunda "." olacak  */
			if( ( POS_Type == '5' ) || ( POS_Type == '6' ) )	
			{
				memcpy( temporary_string1, fpGTransStruct.sorgulama_tutar + 1 , 10 );
				temporary_string1[10] ='.';
				temporary_string1[11] =fpGTransStruct.sorgulama_tutar[11];
				temporary_string1[12] =fpGTransStruct.sorgulama_tutar[12];
			} 
			else memcpy(temporary_string1, fpGTransStruct.sorgulama_tutar, 13);
			temporary_string1[13] = 0;

			memcpy(temporary_string2, fpGTransStruct.approval_code, 6);
			temporary_string2[6] = 0;

			sprintf(toprinter, "%s%s %s", card_name, temporary_string2, temporary_string1);
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			/* belge no ve bayi no */
			memset(lb_belge_no, 0, sizeof(lb_belge_no));
			memset(lb_bayi_no, 0, sizeof(lb_bayi_no));
			memcpy(lb_belge_no, fpGTransStruct.belge_no, 10);
			lb_belge_no[10] = 0;
			memcpy(lb_bayi_no, fpGTransStruct.bayii_no, 7);
			lb_bayi_no[7] = 0;

			sprintf(toprinter, "%s %s", lb_belge_no, lb_bayi_no);
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);
//			Slips_PrinterStr("\n");		/* 04_11 Bayi adinin satir basindan baslamasini saglar */
			PrntUtil_Cr(1);
			/* 04_04 22/11/2002 */
			/* bayi adi */
			memset(toprinter, 0, sizeof(toprinter));
			memcpy(toprinter, fpGTransStruct.bayii_adi, 24);
			toprinter[24] = 0;
			Slips_PrinterStr(toprinter);

			/* vade sayisi aliniyor */
			strcpy(temporary_string1, "01");
			memcpy(temporary_string2, fpGTransStruct.vade_sayisi, 2);
			temporary_string2[2] = 0;

			/* vade sayisi integer'a donusturuluyor */
			lvade_sayisi = atoi(temporary_string2);

			/* tarih ve vade bilgileri aliniyor */
			memset(lbvade_tarih_string, 0, sizeof(lbvade_tarih_string));
			memcpy(lbvade_tarih_string, fpGTransStruct.DB_Tarih_Vade, sizeof(lbTarih_Vade));

			/*if (memcmp(temporary_string2, temporary_string1, 2) != 0)*/
			if(fpGTransStruct.MSG_TYPE == '2' )
			{	/* vadeli kismin detay rapora yazýlýyor */
				memset(toprinter, 0, 25);
				sprintf(toprinter, "\nVADE SAYISI: %s\n", temporary_string2);
				Slips_PrinterStr(toprinter);

				Slips_GPrint_VadeTarih(lvade_sayisi, lbvade_tarih_string);
			}

			if(fpGTransStruct.void_status == '1')
			{
				strcpy(toprinter, "         IPTAL\n");
				toprinter[15] = 0;
				Slips_PrinterStr(toprinter);
			}

			PrntUtil_Cr(2);
		}
	}
	PrntUtil_Cr(2);

	/********* 04_15 12/06/2003 A.Y. Genius toplamlarin yazimi ********/
	/******************************************************************/
	/* sorgulama tutarý */

	memset(total_GT1_amt,'0', sizeof(total_GT1_amt));
	total_GT1_amt[13] = 0;

	memset(total_GT2_amt,'0', sizeof(total_GT2_amt));
	total_GT2_amt[13] = 0;

	Trans_GFindTrxTotals();

	Utils_Add_Strings(net_total_GT1_amt, rev_total_GT1_amt, total_GT1_amt);
	Utils_Add_Strings(net_total_GT2_amt, rev_total_GT2_amt, total_GT2_amt);
	
	Slips_PrinterStr("~~~~~~~~~~~~~~~~~~~~~~~~");
        Slips_PrinterStr("    BIZ.CARD ˜žLEM      ");
	Slips_PrinterStr("      TOPLAMLARI        ");
	Slips_PrinterStr("~~~~~~~~~~~~~~~~~~~~~~~~");
	
    Slips_PrinterStr("-- PEž˜N ˜žLEM TUTARI --");
	memset(formatted_amount, 0, sizeof(formatted_amount));
	Slips_FormatAmount(total_GT1_amt,formatted_amount); 
	formatted_amount[24] = 0;
	temp_number_of_zeros = (int)((21-strlen(formatted_amount))/2);
	memcpy(temp_str, "                        ", 24);
	temp_str[24] = 0;
	memcpy(&temp_str[temp_number_of_zeros],formatted_amount,strlen(formatted_amount));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," YTL");
	else
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," TL");
	temp_str[24] = 0;
	Slips_PrinterStr(temp_str);
	Slips_PrinterStr("\n\n");
	
    Slips_PrinterStr("----- ˜PTAL ED˜LEN -----");
	memset(formatted_amount, 0, sizeof(formatted_amount));
	Slips_FormatAmount(rev_total_GT1_amt,formatted_amount); 
	formatted_amount[24] = 0;
	temp_number_of_zeros = (int)((21-strlen(formatted_amount))/2);
	memcpy(temp_str, "                        ", 24);
	temp_str[24] = 0;
	memcpy(&temp_str[temp_number_of_zeros],formatted_amount,strlen(formatted_amount));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," YTL");
	else
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," TL");
	temp_str[24] = 0;
	Slips_PrinterStr(temp_str);
	Slips_PrinterStr("\n\n");

    Slips_PrinterStr("-NET PEž˜N ˜žLEM TUTARI-");
	memset(formatted_amount, 0, sizeof(formatted_amount));
	Slips_FormatAmount(net_total_GT1_amt,formatted_amount); 
	formatted_amount[24] = 0;
	temp_number_of_zeros = (int)((21-strlen(formatted_amount))/2);
	memcpy(temp_str, "                        ", 24);
	temp_str[24] = 0;
	memcpy(&temp_str[temp_number_of_zeros],formatted_amount,strlen(formatted_amount));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," YTL");
	else
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," TL");
	temp_str[24] = 0;
	Slips_PrinterStr(temp_str);
	Slips_PrinterStr("\n\n");

    Slips_PrinterStr("- VADEL˜ ˜žLEM TUTARI --");
	memset(formatted_amount, 0, sizeof(formatted_amount));
	Slips_FormatAmount(total_GT2_amt,formatted_amount); 
	formatted_amount[24] = 0;
	temp_number_of_zeros = (int)((21-strlen(formatted_amount))/2);
	memcpy(temp_str, "                        ", 24);
	temp_str[24] = 0;
	memcpy(&temp_str[temp_number_of_zeros],formatted_amount,strlen(formatted_amount));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," YTL");
	else
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," TL");
	temp_str[24] = 0;
	Slips_PrinterStr(temp_str);
	Slips_PrinterStr("\n\n");
	
    Slips_PrinterStr("----- ˜PTAL ED˜LEN -----");
	memset(formatted_amount, 0, sizeof(formatted_amount));
	Slips_FormatAmount(rev_total_GT2_amt,formatted_amount); 
	formatted_amount[24] = 0;
	temp_number_of_zeros = (int)((21-strlen(formatted_amount))/2);
	memcpy(temp_str, "                        ", 24);
	temp_str[24] = 0;
	memcpy(&temp_str[temp_number_of_zeros],formatted_amount,strlen(formatted_amount));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," YTL");
	else
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," TL");
	temp_str[24] = 0;
	Slips_PrinterStr(temp_str);
	Slips_PrinterStr("\n\n");

    Slips_PrinterStr("NET VADEL˜ ˜žLEM TUTARI ");
	memset(formatted_amount, 0, sizeof(formatted_amount));
	Slips_FormatAmount(net_total_GT2_amt,formatted_amount); 
	formatted_amount[24] = 0;
	temp_number_of_zeros = (int)((21-strlen(formatted_amount))/2);
	memcpy(temp_str, "                        ", 24);
	temp_str[24] = 0;
	memcpy(&temp_str[temp_number_of_zeros],formatted_amount,strlen(formatted_amount));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," YTL");
	else
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," TL");
	temp_str[24] = 0;
	Slips_PrinterStr(temp_str);
	Slips_PrinterStr("\n\n");

	
	Slips_PrinterStr("--- NET TOPLAM TUTAR ---");
	memset(temp_total, 0, sizeof(temp_total));
	Utils_Add_Strings(net_total_GT1_amt, net_total_GT2_amt, temp_total);
	temp_total[13] = 0;
	memset(formatted_amount, 0, sizeof(formatted_amount));
	Slips_FormatAmount(temp_total,formatted_amount); 
	formatted_amount[24] = 0;
	temp_number_of_zeros = (int)((21-strlen(formatted_amount))/2);
	memcpy(temp_str, "                        ", 24);
	temp_str[24] = 0;
	memcpy(&temp_str[temp_number_of_zeros],formatted_amount,strlen(formatted_amount));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," YTL");
	else
		strcpy(&temp_str[temp_number_of_zeros+strlen(formatted_amount)]," TL");
	temp_str[24] = 0;
	Slips_PrinterStr(temp_str);
	Slips_PrinterStr("\n\n");

	Slips_PrinterStr("\n========================\n\n");


}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GPrint_VadeTarih
 *
 * DESCRIPTION                  : Prints the vadeli part of the genius trx.
 *
 * RETURN                       : None
 *      
 * NOTES                        : r.i.o.  10/10/02
 *
 * ------------------------------------------------------------------------ */
void Slips_GPrint_VadeTarih(int lvade_sayisi, char *vade_tarih_string)
{
	char toprinter[25];
	sint K;

	char lbformatted_amount[16], lbamount[14];
	char day[3], month[3], year[3];
	sint i,index;
	Tarih_Vade lbTarih_Vade[12];
	
	BizSlip = TRUE; //@bm 15.12.2004

	memcpy(&lbTarih_Vade, vade_tarih_string, sizeof(vade_tarih_string));

	/*PrntUtil_Cr(2);*/
	Slips_PrinterStr(" VADELI ODEME PLANI \n");
	Slips_PrinterStr("========================");
	PrntUtil_Cr(1);

	/*if(lvade_sayisi == 1) return;*/

	K=0;
	for (i=0; i<lvade_sayisi;i++)
	{
		K=21*i;
		/* initialize varibales*/
		memset(day,0,sizeof(day));
		memset(month,0,sizeof(month));
		memset(year,0,sizeof(year));
		memset(lbamount,0,sizeof(lbamount));
		memset(toprinter,0,sizeof(toprinter));

		
		/* get tarih from structure */
		memcpy(day,vade_tarih_string+K,2);
		memcpy(month,vade_tarih_string+K+2,2);
		memcpy(year,vade_tarih_string+K+6,2);

		/* get vade amount from genius batch file and format it*/
		memcpy(lbamount,vade_tarih_string+K+8,13);
		/*memcpy(lbamount,&lbTarih_Vade[i]+8,13);*/

		Slips_FormatAmount(lbamount, lbformatted_amount);

		memset(toprinter, ' ', 24);
		sprintf(toprinter, "%s/%s/%s                ",day,month,year);          
		index = (24 - strlen(lbformatted_amount));
		memcpy(&toprinter[index], lbformatted_amount, strlen(lbformatted_amount));
		toprinter[24] = 0;
		Slips_PrinterStr(toprinter);
		PrntUtil_Cr(1);
	}
	
		Slips_PrinterStr("========================");
		PrntUtil_Cr(2);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GPrintHataliTarihler.
 *
 * DESCRIPTION                  : Confirmation Code:888 ise hatali tarihler basilir.
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. 16/10/2002
 *
 * ------------------------------------------------------------------------ */
void Slips_GPrintHataliTarihler(void)
{
	sint i;
	char local_date[9];					/* ggaayyyy */
	char local_date_formatted[11];		/* gg/aa/yyyy */
	char local_string[50];
	char local_message[17];

	BizSlip = TRUE; //@bm 15.12.2004

	memset(local_message, 0, sizeof(local_message));
	memcpy(local_message, d_GTransaction_Receive.message, 16);
	local_message[16];
	

	for(i=0;i<12;i++)
	{
		if(local_message[i] == 'H')
		{
			memset(local_date,0,sizeof(local_date));
			memcpy(local_date, &d_GTransaction_Send.Gtarih_vade[i],8);
			local_date[8] = 0;

			/* format the date */
			memset(local_date_formatted, 0, sizeof(local_date_formatted));
			local_date_formatted[0]  = local_date[0];
			local_date_formatted[1]  = local_date[1];
			local_date_formatted[2]  = '/';
			local_date_formatted[3]  = local_date[2];
			local_date_formatted[4]  = local_date[3];
			local_date_formatted[5]  = '/';
			local_date_formatted[6]  = local_date[4];
			local_date_formatted[7]  = local_date[5];
			local_date_formatted[8]  = local_date[6];
			local_date_formatted[9]  = local_date[7];
			local_date_formatted[10] = 0;
			
			memset(local_string, 0, sizeof(local_string));
			sprintf(local_string,"%d.Vade : %s\n",(i+1),local_date_formatted);
			Slips_PrinterStr(local_string);
		}
	}
		Slips_PrinterStr("\nTARIHLERI TATIL GUNUDUR\n");
		Slips_PrinterStr("DOGRU TARIHLER ILE\n");
		Slips_PrinterStr("TEKRAR DENEYINIZ...\n");
		PrntUtil_Cr(6);

}



/* 04_16 06/08/2003 balance query slip irfan */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GFillBalanceQueryReceiptStructure.
 *
 * DESCRIPTION                  : Fill the genius balance query receipt structure
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. 06/08/2003
 *
 * ------------------------------------------------------------------------ */
boolean Slips_GFillBalanceQueryReceiptStructure(void)
{
	char amount_temp[25];
	int number_of_zeros,Status;
	char received_amount[14];                  /*received amount_surcharge as char*/
	char surcharge[14];                        /* surcharge = received_amount - send_amount*/
	char formatted_card_no[24];
	char formatted_exp_date[6];
	char device_date[8], device_time[5];
	int i;								/* ATS related variables */
	char temp_name[25];							/* 02_02 irfan 15/08/2000 
													isim ve soyismi tutuyor */

	BizSlip = TRUE; //@bm 15.12.2004

	/* fill d_Receipt structure by space and initialize flags */
	memset(&d_GBalanceQueryReceipt, ' ', sizeof(d_GBalanceQueryReceipt));

	d_GBalanceQueryReceipt.tran_type = Gtran_type;

	memcpy(d_GBalanceQueryReceipt.term_no, merch_device_no, 8);
	d_GBalanceQueryReceipt.term_no[8] = 0;
	
	/* get date and time and format them */
	Utils_GetDateTime(device_date,device_time);

	memcpy(d_GBalanceQueryReceipt.date,device_date,8);
	d_GBalanceQueryReceipt.date[8]=0;

	memcpy(d_GBalanceQueryReceipt.time,device_time,5);
	d_GBalanceQueryReceipt.time[5] = 0;

	/* Formatting and filling the card no and exp date */
	Slips_FormatCardExp(formatted_card_no, formatted_exp_date, card_no, exp_date);
	memcpy(d_GBalanceQueryReceipt.card_no, formatted_card_no, strlen(formatted_card_no));
	d_GBalanceQueryReceipt.card_no[strlen(formatted_card_no)] = 0;

	memcpy(d_GBalanceQueryReceipt.exp_date, formatted_exp_date, strlen(formatted_exp_date));
	d_GBalanceQueryReceipt.exp_date[strlen(formatted_exp_date)] = 0;

	/* set input type */
	if (input_type == '1' )
		d_GBalanceQueryReceipt.input_type = 'T';
	else if  (input_type == '2' )
		d_GBalanceQueryReceipt.input_type = 'D';
	else
	{
		Mainloop_DisplayWait("G˜R˜ž T˜P˜ HATASI",1);
		d_GBalanceQueryReceipt.input_type = 'X';
	}

	/* 02_02 irfan 15/09/2000 2. kopya icin basýlan bilgierin kaybolmamasýný saglar*/

	if(ISIMYAZMA_FLAG && TRACK1ERROR)
	{
		if(d_GBalanceQueryReceipt.input_type == 'D')
		{
			memset(temp_name, 0 , sizeof(temp_name));
			sprintf(temp_name, "%s %s", card_info1_name, card_info1_surname);
			temp_name[strlen(card_info1_name) + strlen(card_info1_surname)+1] = 0;
			memcpy(d_GBalanceQueryReceipt.Card_Holder_Name, temp_name, strlen(temp_name));
			d_GBalanceQueryReceipt.Card_Holder_Name[24] = 0; /* ends with null */

		}
	}


	/* get PESIN MEVDUAT TUTARI */
	memcpy(received_amount, d_GTransaction_Receive.available_balance,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);
	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GBalanceQueryReceipt.pesin_mevduat_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GBalanceQueryReceipt.pesin_mevduat_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GBalanceQueryReceipt.pesin_mevduat_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	if(d_GTransaction_Receive.approval_code[0] == '1') d_GBalanceQueryReceipt.pesin_mevduat_tutar[number_of_zeros - 1] = '-';
	d_GBalanceQueryReceipt.pesin_mevduat_tutar[24]=0;

	/* get PESIN ARTI PARA */
	memcpy(received_amount, d_GTransaction_Receive.kullanilan_kredi_limiti,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);
	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GBalanceQueryReceipt.pesin_artipara_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GBalanceQueryReceipt.pesin_artipara_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GBalanceQueryReceipt.pesin_artipara_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	if(d_GTransaction_Receive.approval_code[1] == '1') d_GBalanceQueryReceipt.pesin_artipara_tutar[number_of_zeros - 1] = '-';
	d_GBalanceQueryReceipt.pesin_artipara_tutar[24]=0;

	/* get PESIN KREDI TUTARI */
	memcpy(received_amount, d_GTransaction_Receive.kart_islem_toplami,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);
	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GBalanceQueryReceipt.pesin_kredi_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GBalanceQueryReceipt.pesin_kredi_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GBalanceQueryReceipt.pesin_kredi_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GBalanceQueryReceipt.pesin_kredi_tutar[24]=0;

	/* get PESIN MAX. ISLEM TUTARI */
	memcpy(received_amount, d_GTransaction_Receive.garanti_tutar,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);
	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GBalanceQueryReceipt.pesin_maxislem_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GBalanceQueryReceipt.pesin_maxislem_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GBalanceQueryReceipt.pesin_maxislem_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GBalanceQueryReceipt.pesin_maxislem_tutar[24]=0;

	/* get VADELI KREDI TUTARI */
	memcpy(received_amount, d_GTransaction_Receive.kart_islem_toplami,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);
	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GBalanceQueryReceipt.vadeli_kredi_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GBalanceQueryReceipt.vadeli_kredi_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GBalanceQueryReceipt.vadeli_kredi_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GBalanceQueryReceipt.vadeli_kredi_tutar[24]=0;

	/* get VADELI KREDISIZ TUTARI */
	memcpy(received_amount, d_GTransaction_Receive.bayii_garantisiz_borc,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);
	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GBalanceQueryReceipt.vadeli_kredisiz_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GBalanceQueryReceipt.vadeli_kredisiz_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GBalanceQueryReceipt.vadeli_kredisiz_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GBalanceQueryReceipt.vadeli_kredisiz_tutar[24]=0;

	/* get VADELI MAX ISLEM TUTARI */
	memcpy(received_amount, d_GTransaction_Receive.garanti_tutar,13);
	received_amount[13]=0;
	Utils_LeftPad(received_amount,'0',13);
	/* format the amount*/
	Slips_FormatAmount(received_amount,amount_temp);
	number_of_zeros = (int)((21-strlen(amount_temp))/2);
	memcpy(&d_GBalanceQueryReceipt.vadeli_maxislem_tutar[number_of_zeros],amount_temp,strlen(amount_temp));
	if ( ( POS_Type == '5' ) || ( POS_Type == '6' ) )
		strcpy(&d_GBalanceQueryReceipt.vadeli_maxislem_tutar[number_of_zeros+strlen(amount_temp)]," YTL");
	else
		strcpy(&d_GBalanceQueryReceipt.vadeli_maxislem_tutar[number_of_zeros+strlen(amount_temp)]," TL");
	d_GBalanceQueryReceipt.vadeli_maxislem_tutar[24]=0;
	
	/* get bayii_no*/
	memcpy(d_GBalanceQueryReceipt.bayii_no, d_GTransaction_Receive.bayii_no,7);
	d_GBalanceQueryReceipt.bayii_no[7] = 0;

	/* get bayii_no*/ /* 04_04 22/11/2002 */
	memcpy(d_GBalanceQueryReceipt.bayii_adi, d_GTransaction_Receive.bayii_adi,24);
	d_GBalanceQueryReceipt.bayii_adi[24] = 0;

	/* get belge_no*/
	memcpy(d_GBalanceQueryReceipt.belge_no, d_GTransaction_Send.belge_no,10);
	d_GBalanceQueryReceipt.belge_no[10] = 0;

	return(TRUE);
}


/* 04_16 06/08/2003 balance query slip irfan */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GPrintBalanceQuery_Slip.
 *
 * DESCRIPTION                  : Print the Balance Query Slip
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. 06/08/2003
 *
 * ------------------------------------------------------------------------ */
void Slips_GPrintBalanceQuery_Slip(void)
{
	char masked_card_no[24]; 	/* bm  13.12.2004 */
	char temp_approvalcode[7];

	BizSlip = TRUE; //@bm 15.12.2004

	/* Print Merch Name ; Merch Adress1 ; Merch Adress2*/
	Slips_PrinterStr(merch_addr);

	/* MERCHANT NAME */
	memset(msgSlips, 0, sizeof(msgSlips));
	sprintf(msgSlips, "˜žYER˜  : %s\n", merch_no);
	msgSlips[24] = 0;
	Slips_PrinterStr(msgSlips);


	/* GRAFPOS terminal no 8 digit basiliyor */
	memset (msgSlips, 0, sizeof(msgSlips));
	merch_device_no[8] = 0;
	sprintf( msgSlips,"CIHAZ NO: %s\n", merch_device_no);
	msgSlips[24]=0;
	Slips_PrinterStr(msgSlips);


	/* Bayi no 7 digit basiliyor */
	memset(msgSlips, 0, sizeof(msgSlips));
	sprintf( msgSlips,"BAYII NO: %s\n", d_GBalanceQueryReceipt.bayii_no);
	msgSlips[24]=0;
	Slips_PrinterStr(msgSlips);

	/* Bayi adi 24 digit basiliyor */
	Slips_PrinterStr(d_GBalanceQueryReceipt.bayii_adi);
	

	/* DATE , TIME and transaction type */
	sprintf(msgSlips, "%s %s \n\n", d_GBalanceQueryReceipt.date, d_GBalanceQueryReceipt.time);
	msgSlips[24] = 0;
	Slips_PrinterStr(msgSlips);  /* sonuna null koymak gerekebilir */

	
	/* formatted card no */
/* @bm 13.12.2004 
	Slips_PrinterStr(d_GBalanceQueryReceipt.card_no);
	msgSlips[24] = 0;
	Slips_PrinterStr("\n");
*/
//	Printer_WriteStr("\nbm gec02\n");
//	PrntUtil_BufAsciiHex((byte *)d_GBalanceQueryReceipt.card_no,24);
	
	memset( masked_card_no, 0, sizeof( masked_card_no ) );
	strcpy( masked_card_no, d_GBalanceQueryReceipt.card_no );
//	PanLength = strlen(masked_card_no);
	if ( PanLength == 0xF )
		memcpy(&masked_card_no[4], " **** ***",9);
	else if ( PanLength == 0x12 )
		memcpy(&masked_card_no[4], " **** **** **",13);
	else if ( PanLength == 0x13 )
		memcpy(&masked_card_no[4], " **** **** ***",14);
	else
		memcpy(&masked_card_no[4], " **** ****",10);

	Slips_PrinterStr(masked_card_no);
	Slips_PrinterStr("\n");


	if(ISIMYAZMA_FLAG && TRACK1ERROR)
	{
		if(d_GReceipt.input_type == 'D')
		{
			Slips_PrinterStr(d_GBalanceQueryReceipt.Card_Holder_Name);
		}
	}

	PrntUtil_Cr(2);	
	
	/* formatted exparation date */
	if(!SECOND_COPY)
	{
		sprintf(msgSlips, "GE€. TAR.: %s\n", d_GBalanceQueryReceipt.exp_date);
		msgSlips[24] = 0;
		Slips_PrinterStr(msgSlips);
	}
	
	/* belge no */
/*	sprintf(msgSlips, "BELGE NO : %s\n\n", d_GBalanceQueryReceipt.belge_no);
	msgSlips[24] = 0;
	Slips_PrinterStr(msgSlips);*/

	/* fix ifade */
	Slips_PrinterStr("\n--- BAKIYE BILGILERI ---\n");
	Slips_PrinterStr("========================\n");


	/* fix ifade */
	Slips_PrinterStr("- P E S I N  I S L E M -\n");
	Slips_PrinterStr("========================\n");
	/* Mevduat tutarý */
	Slips_PrinterStr("--- MEVDUAT   TUTARI ---\n");
	Slips_PrinterStr(d_GBalanceQueryReceipt.pesin_mevduat_tutar);
	Slips_PrinterStr("\n\n");

	/* Arti Para */
	Slips_PrinterStr("--- ARTI PARA TUTARI ---\n");
	Slips_PrinterStr(d_GBalanceQueryReceipt.pesin_artipara_tutar);
	Slips_PrinterStr("\n\n");

	/* Kredi  */
	Slips_PrinterStr("----- KREDI  TUTARI ----\n");
	Slips_PrinterStr(d_GBalanceQueryReceipt.pesin_kredi_tutar);
	Slips_PrinterStr("\n\n");

	/* Max Ýslem Tutarý */
	Slips_PrinterStr("-- MAX. ISLEM TUTARI ---\n");
	Slips_PrinterStr(d_GBalanceQueryReceipt.pesin_maxislem_tutar);
	Slips_PrinterStr("\n\n");

	/* fix ifade */
	Slips_PrinterStr(" V A D E L I  I S L E M \n");
	Slips_PrinterStr("========================\n");

	/* Kredi  */
	Slips_PrinterStr("----- KREDI  TUTARI ----\n");
	Slips_PrinterStr(d_GBalanceQueryReceipt.vadeli_kredi_tutar);
	Slips_PrinterStr("\n\n");

	/* Kredisiz Tutar  */
	Slips_PrinterStr("---- KREDISIZ TUTAR ----\n");
	Slips_PrinterStr(d_GBalanceQueryReceipt.vadeli_kredisiz_tutar);
	Slips_PrinterStr("\n\n");
	
	/* Max Ýslem Tutarý */
	Slips_PrinterStr("-- MAX. ISLEM TUTARI ---\n");
	Slips_PrinterStr(d_GBalanceQueryReceipt.vadeli_maxislem_tutar);
	Slips_PrinterStr("\n\n");
	
	PrntUtil_Cr(4); 
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Slips_GBakiyePrintErrorReceipt.
 *
 * DESCRIPTION                  : Print the error slip for bakiye sorma
 *
 * RETURN                       :
 *      
 * NOTES                        : r.i.o. 27/08/2003
 *
 * ------------------------------------------------------------------------ */
void Slips_GBakiyePrintErrorReceipt(void)
{

	char mes_string[24];

	BizSlip = TRUE; //@bm 15.12.2004

	/* fill the message part */
	memset(mes_string, 0, sizeof(mes_string));
	memcpy(mes_string,d_GTransaction_Receive.message,16);
	memcpy(mes_string+16, d_GTransaction_Receive.confirmation,3);
	mes_string[20] = 0;

	PrntUtil_Cr(2);

	/* PRINT THE RECEIVED MESSAGE FROM HOST */
	PrntUtil_Cr(1);
	Slips_PrinterStr("\nHATALI BAKIYE SORMA!!!\n");
	Slips_PrinterStr(mes_string);

	PrntUtil_Cr(6);

	Mte_Wait(800);
	Kbhw_BuzzerPulse(800);
}





