                                                                                                                                   /**************************************************************************
  FILE NAME:   XLSCHIP.C
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

/* MCC68K Header Files */
/*UPDATE 01_16 26/10/99 sprintf gibi komutlarýn prototype'ý için konuldu*/
/*#include <stdio.h> 

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <mriext.h>


#include NOS_H*/

#include INTERFAC_H

#include ICCTRANS_H
#include APLMAIN_H
#include SMART_H
#include XLSCHIP_H

#include DEFINE_H
#include ERRORS_H
#include UTILS_H
#include FILES_H
#include MESSAGE_H
#include TRANS_H
#include HOSTCOMM_H
#include SLIPS_H
#include MAINLOOP_H
#include DEBUG_H

#define GND_CARD   199
#define GND_CARD_STARCOS 200
#define GEM_PLUS_CARD 299

static IccInstructStruct Transporter;
char remained_chip_data[17];
boolean CHIP_ERROR;		/* TRUE : correct reading , FALSE : incorrect reading. */
int card_type;           /* defined in mainloop.c */

extern char card_no[20];
extern char card_info1_name[25];
extern char card_info1_surname[25];
extern char exp_date[5];
extern char cvv_2[4];
extern boolean LAST_4_DIGIT_FLAG;
extern char last_4_digit[5];
extern boolean DEBUG_MODE;
extern char POS_Type;	/* 0:Axess  1:Akbank */
extern boolean EMVAxess;
extern boolean AlreadyChecked;

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

/*
byte CheckEMVAxess(void)
{
	MutualAppList pMutualApps;
	byte pbMutualAppIndex;
	EMVLanguages pelsLangs;
	byte pbErr;
	TerminalAppList fpAppList;

	if (AlreadyChecked && EMVAxess) return STAT_MAY;
	else if (AlreadyChecked) return STAT_OK;
	Files_ReadEMVAppParam(&fpAppList);
	memcpy(&pelsLangs,&elsLangs,sizeof(pelsLangs));
	Iso78164_Init();
	pbErr = 0; 
	memset(&pMutualApps, 0, sizeof(MutualAppList));
	switch(Emv_ApplSelect(&fpAppList, &pMutualApps, &pbMutualAppIndex, &lsTerminalDataStruct, &pelsLangs, &pbErr))
	{
		case EMV_OK:
			break;
		case EMV_FALLBACK :
		default : ;
	}
	AlreadyChecked = TRUE;
	if (EMVAxess) return STAT_MAY;
	return STAT_OK;
}
*/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: E3744_AnsToReset
 *
 * DESCRIPTION:   Does the E3744 card reset, check answer to reset data and
 *                history sizes and moves the history to buf.
 *
 * RETURN:        SmartOK or error code.
 *
 * NOTES:         OK (NS).
 *
 * ------------------------------------------------------------------------ */

byte ResetCardExtLight(void)
{
	IccAnswerToResetStruct answer;
	byte buf[16];

	memset (buf, 0, 16);
	answer.Data       = buf;
	answer.SizeOfData = 16;
	answer.port_indx  = TESTED_CARD_PORT;
	ICC_Status( TESTED_CARD_PORT);
	if (E3744_RetConv( ICC_AnswerToReset( &answer) ) != SmartOK) return STAT_NOK;
	if (DEBUG_MODE)
	{
		Printer_WriteStr("LightATR:\n");
		PrntUtil_BufAsciiHex((byte *)buf,sizeof(buf));
	}
	return STAT_OK;
}

byte ResetCardExt (void)
{
	IccAnswerToResetStruct answer;
	byte buf[16];

	memset (buf, 0, 16);
	answer.Data       = buf;
	answer.SizeOfData = 16;
	answer.port_indx  = TESTED_CARD_PORT;
	ICC_Status( TESTED_CARD_PORT);
	if (E3744_RetConv( ICC_AnswerToReset( &answer) ) != SmartOK) return STAT_NOK;

	/* 05_02 irfan 17/09/2002 debug mode icine alindi */

	if (DEBUG_MODE)
	{
		Printer_WriteStr("HardATR:\n");
		PrntUtil_BufAsciiHex((byte *)buf,sizeof(buf));
	}
	
	if((buf[2] == 0xA2) && (buf[3] == 0x01) && (buf[4] == 0x01) && (buf[5] == 0x01) && (buf[6] == 0x3D) && (buf[7] == 0x72))
	{
		/* GEM PLUS */
		if (DEBUG_MODE) Printer_WriteStr("GEM_PLUS_CARD\n");
		card_type = GEM_PLUS_CARD;
	}
	else if ((buf[2] == 0x80) && (buf[3] == 0x71) && (buf[4] == 0x86) && (buf[5] == 0x65) && (buf[6] == 0x47) && (buf[7] == 0x44) && (buf[8] == 0x24) && (buf[9] == 0x01) && (buf[10] == 0x81))
	{
		/* STARD DC */
		if (DEBUG_MODE) Printer_WriteStr("GND_CARD\n");
		card_type = GND_CARD;
	}
	else if ( (buf[2] == 0x4B) && (buf[3] == 0x32) && (buf[4] == 0x33) )
	{
		if (DEBUG_MODE) Printer_WriteStr("GND_CARD_STARCOS\n");
		card_type = GND_CARD_STARCOS;
	}
	else 
	{
		if (DEBUG_MODE) Printer_WriteStr("EMV_CARD\n");
		return STAT_MAY;
	}
	return(STAT_OK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: E3744_Read4
 *
 * DESCRIPTION:   Reads 4 bytes from Zone2 and checks it.
 *
 * RETURN:        SmartOK or error code.
 *
 * NOTES:         The PIN must be presented before this function.
 *
 * ------------------------------------------------------------------------ */
byte E3744_SelectRead( void) 
{
	byte buf_in[2];
	byte buf_out[256];
	char buf_ascii[256];
	sint i,j;
	byte Count;
	char Buffer[LEN_GETKEYBOARD];
	byte Len;
	char manual_last_4_digit_local[5];
	char value_null[2];
	char surname_name_chip[27];

	CHIP_ERROR = TRUE;
	
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));

	/* v04_05 GND UPDATES 16/07/2002. GEMPLUS kartlarýn okunamsý   */
	  /* SELECT  MF FILE*/
	if(card_type == GEM_PLUS_CARD)
	{
	  buf_in[0]=0x3F;
	  buf_in[1]=0x00;

	  memset( &Transporter, 0, sizeof( IccInstructStruct));
	  memset(buf_out,0,255);
	  Transporter.Class        =0x00;
	  Transporter.Instruct     =0xA4;
	  Transporter.P1           =0x00;
	  Transporter.P2           =0x00;
	  Transporter.DataToCard   =buf_in;
	  Transporter.DataFromCard =buf_out;
	  Transporter.Le           = 12;
	  Transporter.Lc           = 2;
	  Transporter.port_indx    = TESTED_CARD_PORT;

	  if (E3744_RetConv( ICC_Command( &Transporter)) != SmartOK) return STAT_NOK;
	  if(!((Transporter.SW1==0x61) && (Transporter.SW2==0x12)))  return STAT_NOK;

	  /* SELECT  DF FILE*/
	  buf_in[0]=0x20;
      buf_in[1]=0x00;
  
	  memset( &Transporter, 0, sizeof( IccInstructStruct));
	  memset(buf_out,0,255);
	  Transporter.Class        =0x00;
	  Transporter.Instruct     =0xA4;
	  Transporter.P1           =0x01;
	  Transporter.P2           =0x00;
	  Transporter.DataToCard   =buf_in;
	  Transporter.DataFromCard =buf_out;
	  Transporter.Le           = 12;
	  Transporter.Lc           = 2;
	  Transporter.port_indx    = TESTED_CARD_PORT;
/*		Printer_WriteStr( "bm gec 22:\n" );*/
	  if (E3744_RetConv( ICC_Command( &Transporter)) != SmartOK) 
	  {
		  CHIP_ERROR = FALSE;
		  return STAT_NOK;
	  }
	  if(!((Transporter.SW1==0x61) && (Transporter.SW2==0x12)))
	  {
		  CHIP_ERROR = FALSE;
		  return STAT_NOK;
	  }


	  
	  /* SELECT  EF FILE*/
	  buf_in[0]=0x20;
      buf_in[1]=0x01;

	  memset( &Transporter, 0, sizeof( IccInstructStruct));
	  memset(buf_out,0,255); 
	  Transporter.Class        =0x00;
	  Transporter.Instruct     =0xA4;
	  Transporter.P1           =0x02;
	  Transporter.P2           =0x00;
	  Transporter.DataToCard   =buf_in;
	  Transporter.DataFromCard =buf_out;
	  Transporter.Le           = 12;
	  Transporter.Lc           = 2;
	  Transporter.port_indx    = TESTED_CARD_PORT;
/*		Printer_WriteStr( "bm gec 25:\n" );*/
	  if (E3744_RetConv( ICC_Command( &Transporter)) != SmartOK)
	  {
		  CHIP_ERROR = FALSE;
		  return STAT_NOK;
	  }
	  if(!((Transporter.SW1==0x61) && (Transporter.SW2==0x12)))
	  {
		  CHIP_ERROR = FALSE;
		  return STAT_NOK;
	  }


	   
	  /* READ  EF FILE*/
	  memset( &Transporter, 0, sizeof( IccInstructStruct));
	  memset(buf_out,0,255);
	  Transporter.Class        =0x00;
	  Transporter.Instruct     =0xB0;
	  Transporter.P1           =0x00;
	  Transporter.P2           =0x00;
	  Transporter.DataToCard   =buf_in;
	  Transporter.DataFromCard =buf_out;
	  Transporter.Le           = 44;
	  Transporter.port_indx    = TESTED_CARD_PORT;
	}


	/* v04_05 GND UPDATES. GND KART OKUMA */
	if ( (card_type == GND_CARD) || (card_type == GND_CARD_STARCOS) )
	{
	  buf_in[0]=0x00;
      buf_in[1]=0x00;
   
	  /* SELECT and READ  EF FILE*/
	  memset( &Transporter, 0, sizeof( IccInstructStruct));
	  memset(buf_out,0,255);
	  Transporter.Class        =0x00;
	  Transporter.Instruct     =0xB2;
	  Transporter.P1           =0x01;
	  Transporter.P2           =0x54;
	  Transporter.DataToCard   =buf_in;
	  Transporter.DataFromCard =buf_out;
	  Transporter.Le           = 44;
	  Transporter.port_indx    = TESTED_CARD_PORT;
	
	}
	  if (E3744_RetConv( ICC_Command( &Transporter)) != SmartOK)
		  	  {
		  CHIP_ERROR = FALSE;
		  return STAT_NOK;
	  }
/*	Printer_WriteStr("\nICC_CommandOut\n");
	PrntUtil_BufAsciiHex((byte*)&Transporter,sizeof(Transporter));
*/
	
	  if(!((Transporter.SW1==0x90) && (Transporter.SW2==0x00)))
	  {
		  CHIP_ERROR = FALSE;
		  return STAT_NOK;
	  }

		  conv_hex2ascii((byte*)buf_ascii, buf_out+1, 43);

		if(buf_ascii[0] == '3' && buf_ascii[1] == '7')
		{	/* Amex Card */

			memset (card_no,0,sizeof(card_no));
			memset (exp_date, 0, sizeof(exp_date));
			memset (cvv_2, 0, sizeof(cvv_2));
			memset (last_4_digit, 0, sizeof(last_4_digit));
			memset(remained_chip_data,0, sizeof(remained_chip_data));
			memset(surname_name_chip,0, sizeof(surname_name_chip));
			
			memcpy(card_no, buf_ascii,15);
			memcpy(exp_date, buf_ascii+17,4);
			memcpy(cvv_2, buf_ascii+29,3);
			memcpy(last_4_digit, card_no+12,4);
			memcpy(remained_chip_data, buf_ascii+17, 16);
			memcpy(surname_name_chip, buf_out+18, 26);
			
			/*test amacli 18/07/2001 irfan */
			/*
			PrntUtil_Cr(2);
			Slips_PrinterStr("Amex card_no\n");
			Slips_PrinterStr(card_no);
			PrntUtil_Cr(2);
			Slips_PrinterStr("Amex remained data\n");
			Slips_PrinterStr(remained_chip_data);
			PrntUtil_Cr(2);
			Slips_PrinterStr("Amex surnamename\n");
			Slips_PrinterStr(surname_name_chip);
			PrntUtil_Cr(2);
			*/
		}
		else
		{	/* non Amex Card */

			memset (card_no, 0,sizeof(card_no));
			memset (exp_date, 0, sizeof(exp_date));
			memset (cvv_2, 0, sizeof(cvv_2));
			memset (last_4_digit, 0, sizeof(last_4_digit));
			memset(remained_chip_data,0, sizeof(remained_chip_data));
			memset(surname_name_chip,0, sizeof(surname_name_chip));

			memcpy(card_no, buf_ascii,16);
			memcpy(exp_date, buf_ascii+18,4);
			memcpy(cvv_2, buf_ascii+30,3);
			memcpy(last_4_digit, card_no+13,4);
			memcpy(remained_chip_data, buf_ascii+18,15);
			memcpy(surname_name_chip, buf_out+18, 26);
			
			/* test amacli 18/07/2001 irfan */
			/*
			PrntUtil_Cr(2);
			Slips_PrinterStr("non Amex card_no\n");
			Slips_PrinterStr(card_no);
			PrntUtil_Cr(2);
			Slips_PrinterStr("non Amex remained data\n");
			Slips_PrinterStr(remained_chip_data);
			PrntUtil_Cr(2);
			Slips_PrinterStr("non Amex surnamename\n");
			Slips_PrinterStr(surname_name_chip);
			PrntUtil_Cr(2);
			*/
		}

		/* card name and surname are calculated */
		memset(card_info1_surname, 0, sizeof(card_info1_surname));
		memset(card_info1_name, 0, sizeof(card_info1_name));

		for (i=0; i<24; i++)
		{
			if(surname_name_chip[i] != '/') 
			{
				card_info1_surname[i] = surname_name_chip[i];
			}
			else
			{
				for(j=0; j<(24-i);j++)
					card_info1_name[j] = surname_name_chip[i+1+j];
				break;
			}
				
		}

		card_info1_surname[23] = 0;
		card_info1_name[23] = 0;

		/** test amacli **/
		/*PrntUtil_Cr(2);
		Slips_PrinterStr("card_info1_surname\n");
		Slips_PrinterStr(card_info1_surname);
		PrntUtil_Cr(2);
		Slips_PrinterStr("card_info1_name\n");
		Slips_PrinterStr(card_info1_name);
		PrntUtil_Cr(2);*/


		/* Analyze the chip card data */
		/******************************/
		/* LAST 4 DIGIT  control */
	
		if(LAST_4_DIGIT_FLAG)
		{
			/* get the manual last 4 digit from keyboard */
			if(!Mainloop_GetKeyboardInput("SON 4 DIGIT?  ",Buffer,4,4,    
					       FALSE, FALSE, FALSE, FALSE, FALSE,&Len,value_null, FALSE))
			{
				Mainloop_DisplayWait("   SON 4 HANE   G˜R˜ž YAPILAMADI",2);
				return(STAT_NOK);
			}
			else
			{
				memcpy(manual_last_4_digit_local,Buffer,Len);
				manual_last_4_digit_local[Len]=0;
			}
		 
			if (strcmp(last_4_digit,manual_last_4_digit_local) != 0 )
			{
				Mainloop_DisplayWait("KARTA EL KOYUNUZ",2);
				return(STAT_NOK);
			}

		}

		/* LuhnCheck Control.*/
		if (!Utils_LuhnCheck())
		{
			Mainloop_DisplayWait(" GE€ERS˜Z KART  ",2);
			return (STAT_NOK);
		}


/*
		  Printer_WriteStr ("\nPAN Num:");
		  Printer_Write    (buf_ascii, 16);
		  Printer_WriteStr ("\nEXP Date  :");
		  Printer_Write    (buf_ascii+18,4);
		  Printer_WriteStr ("\nServ.Code :");
		  Printer_Write    (buf_ascii+22,3);
		  Printer_WriteStr ("\nPVKI      :");
		  Printer_Write    (buf_ascii+25,1);
		  Printer_WriteStr ("\nPVV       :");
		  Printer_Write    (buf_ascii+26,4);
		  Printer_WriteStr ("\nCVV       :");
		  Printer_Write    (buf_ascii+30,3);
		  Printer_WriteStr ("\nSurname   :\n");
		  PrntUtil_BufAsciiHex((byte *)buf_out+18, 26);
*/
  return( STAT_OK);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME	: conv_hex2ascii
 *
 * DESCRIPTION		: Converts hex to ascii
 *
 * RETURN			: NA
 *
 * NOTES			: r.i.o. 17/07/2001
 *					  destn		: destination string
 *					  source	: source string
 *					  len		: length of the string
 *
 * ------------------------------------------------------------------------ */
void conv_hex2ascii(byte *destn,byte *source, byte len )
{
	byte i;
	byte ch;

	byte hexChars[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

	i=0;

	while( len--)
	{
		ch = (byte)((*source) >> 4);
		if (ch > 15) return;
		destn[i++] = hexChars[ ch ];

		ch=(byte)((*source++) & 0x0f);
		if (ch > 15) return;
		destn[i++] = hexChars[ ch ];
	}
	destn[i] = 0;
}


