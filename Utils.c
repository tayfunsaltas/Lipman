/**************************************************************************
  FILE NAME:   MAINLOOP.C
  MODULE NAME: 
  PROGRAMMER:
  DESCRIPTION: Main Loop
  REVISION:    01.00
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
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
/*#include <mriext.h>*/
#include <ctype.h>

#include  NOS_H

#include DEFINE_H
#include UTILS_H
#include ERRORS_H
#include MESSAGE_H

/* verde */
#include FILES_H

extern struct Transaction_Receive       d_Transaction_Receive;
/*extern char debit_prefix[10][30][7];*/	/* verde. kaldýrýldý. artýk ramdisk de saklanýyor */
extern char debit_prefix[07][10][7]; 	/* verde. 30 yerine 10 yapýldý */
extern char UserPassword[5];
extern char OperatorPassword[15];
extern char SoftPassword[5];

extern char master_key_no;		/* UPDATE 01_15 24/10/99*/

/* Toygar 13.05.2002 Start */
extern boolean BUYUKFIRMA;
/* Toygar 13.05.2002 End */

/** XLS_PAYMENT 01/08/2001 irfan **/
extern char merch_device_no[9];                                



extern char	merch_no[13];

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
 * FUNCTION NAME: Main_CharConv
 *
 * DESCRIPTION:Replaces the Turkish characters.
 *
 * RETURN:none
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
void Utils_CharConv(char *fpWrkBuf, int Len)
{
	int i;
	byte temp;

	for(i=0;i<Len;)
	{
		Kb_Read();
		temp=fpWrkBuf[i];
		     if (temp == (BYTE)0xdd) fpWrkBuf[i] = '˜';
		else if (temp == (BYTE)0xd6) fpWrkBuf[i] = '™';
		else if (temp == (BYTE)0xd0) fpWrkBuf[i] = '¦';
		else if (temp == (BYTE)0xde) fpWrkBuf[i] = 'ž';
		else if (temp == (BYTE)0xc7) fpWrkBuf[i] = '€';
		else if (temp == (BYTE)0xdc) fpWrkBuf[i] = 'š';
		else if (temp == (BYTE)0xf0) fpWrkBuf[i] = '§';
		else if (temp == (BYTE)0xfc) fpWrkBuf[i] = '';
		else if (temp == (BYTE)0xfd) fpWrkBuf[i] = '';
		else if (temp == (BYTE)0xe7) fpWrkBuf[i] = '‡';
		else if (temp == (BYTE)0xf6) fpWrkBuf[i] = '”';
		else if (temp == (BYTE)0xfe) fpWrkBuf[i] = 'Ÿ';
		else if (temp == (BYTE)0xc8)	fpWrkBuf[i] = 'I';
		i++;
	}
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_LeftPad
 *
 * DESCRIPTION: Displays as left padded.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
void Utils_LeftPad(char *Dest, char PaddingChar, usint LenToPad)
{
	char TempStr[50];
	usint Len;
	usint i;

	memset(TempStr,0,50);
	Len = strlen(Dest);
	if (Len < LenToPad)
	{
		for(i=0;i<LenToPad;i++)
			if (i<(LenToPad-Len)) TempStr[i] = PaddingChar;
				else TempStr[i] = Dest[i-(LenToPad-Len)];
		strcpy(Dest,TempStr);
	}
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_IToA
 *
 * DESCRIPTION:   Converts an integer to string
 *
 * RETURN:        None
 *
 * NOTES:    
 *
 * ------------------------------------------------------------------------ */
void Utils_IToA(int dv, char *out)
{
	int index;
	int index2;
	char aidarray[20];
	
	/* Converting to string using the following theorem :   */
	/* Every whole number can be written as: (digit)*[base] */

	index=0;
	do
	{
		aidarray[index] = (dv % 10) + '0'; 
		dv /= 10;
		index++;
	} while(dv);
	for (index2 = 0; index2<index; index2++) /* Invert array */
		out[index2] = aidarray[index-index2-1];
	out[index]=0; /* Add NULL at the edge of the string */
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_BinToAscii
 *
 * DESCRIPTION: Converts bcd to ASCII.
 *
 * RETURN: none
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
void Utils_BinToAscii(char *fpStrBin,char *fpStrAscii,int fbLen)
{
	char lbTempBin[20];
	char lbTempAscii[40];
	byte nibble;
	byte tempbyte;
	int counter;
	int i;

	memset(lbTempBin,0,20);
	memcpy(lbTempBin,fpStrBin,fbLen);
	memset(lbTempAscii,0,40);
	counter = 0;
	for(i=0;i<fbLen;i++)
	{
		tempbyte = lbTempBin[i];
		nibble  = ((tempbyte & 0xF0) >> 4) & 0x0F;
		if (nibble > 0x09) lbTempAscii[counter] = 'A' - 1 + (nibble-9);
		else Utils_IToA((int)nibble,lbTempAscii+counter);
		counter++;
		nibble  = tempbyte & 0x0F;
		if (nibble > 0x09) lbTempAscii[counter] = 'A' - 1 + (nibble-9);
		else Utils_IToA((int)nibble,lbTempAscii+counter);
		counter++;
	}
	strcpy(fpStrAscii,lbTempAscii);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_ConvAscBcd1
 *
 * DESCRIPTION: 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Utils_ConvAscBcd1(char *fpAsc, char *fpBcd1, int fpLength)
{
	int lbCounter;

	for (lbCounter=0;lbCounter<fpLength;lbCounter++)
		fpBcd1[lbCounter]=fpAsc[lbCounter]-0x30;
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_ConvBcd1Asc
 *
 * DESCRIPTION: 
 *
 * RETURN: none     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
void Utils_ConvBcd1Asc(char *fpBcd1, char *fpAsc, int fpLength)
{
	int lbCounter;

	for (lbCounter=0;lbCounter<fpLength;lbCounter++)
		fpAsc[lbCounter]=fpBcd1[lbCounter]+0x30;
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_ClearInsZeros
 *
 * DESCRIPTION: 
 *
 * RETURN:none
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
void Utils_ClearInsZeros(char *StrToCleared)
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

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_LuhnCheck
 *
 * DESCRIPTION  : This function is called to check the LUHN character of the card number
 *
 * RETURN               : TRUE -> if Luhn Check is Ok.  FALSE -> If Luhn Check is not Ok.
 *
 * NOTES                :       
 *
 * --------------------------------------------------------------------------*/
boolean Utils_LuhnCheck(void)
{

	extern char card_no[20];
	char SiraNum;
	char Carpi1Veya2;
	uchar Toplam;	/* 01_18 15/08/2000 19 Digit uzunluðundaki kartlar için eðer Toplam
						 127 yi geçerse char (127) sýnýr geçiliyor ve lunh check hatasý 
						 oluþuyordu. uchar ile sýnýr (255) yapýldý ve sorun çözüldü. */
	char IkiVeyaBir;
	char Basla;
	int  len;
	char s[20];

	Basla  = 0;
	Toplam = 0;

	memcpy(s, card_no, strlen(card_no)-1);
	s[strlen(card_no)-1] = 0;

	len = strlen(s);
	if (len<12 || len>19) return(-1);
	if (len % 2 == 0) IkiVeyaBir=1; else IkiVeyaBir=2;

	for (SiraNum=Basla;SiraNum<len;SiraNum++)
	{
		Carpi1Veya2 = s[SiraNum]-48;
		Carpi1Veya2 *= IkiVeyaBir;
		if (Carpi1Veya2 > 9) Carpi1Veya2 -= 9;
		Toplam += Carpi1Veya2;
		IkiVeyaBir = IkiVeyaBir ^ 3 ;

	}

	if (Toplam % 10 == 0)
		Toplam = 0;
	else Toplam = 10 - (Toplam % 10);
		s[len] = Toplam + 48;
	s[len+1] = 0;


	if (s[len] != card_no[strlen(card_no)-1])
		return(FALSE);

 return(TRUE);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Utils_GetDateTime
 *
 * DESCRIPTION                  : Gets the date and time 
 *
 * RETURN                               :
 *      
 * NOTES                                : r.i.o. & t.c.a. 22/05/99
 *
 * ------------------------------------------------------------------------ */
void Utils_GetDateTime (char *date_device, char *time_device)
{

    real_time_clock Today;
	int count;

	memset(date_device,' ',8);
	memset(time_device,' ',5);

	Rtc_Read(&Today);

	count = 0;
    date_device[count++]=(Today.day>>4)+'0';
    date_device[count++]=(Today.day&0x0f)+'0';
    date_device[count++]='/';
    date_device[count++]=(Today.month>>4)+'0';
    date_device[count++]=(Today.month&0x0f)+'0';
    date_device[count++]='/';
    date_device[count++]=(Today.year>>4)+'0';
    date_device[count++]=(Today.year&0x0f)+'0';
	

	count = 0;

    time_device[count++]=(Today.hours>>4)+'0';
    time_device[count++]=(Today.hours&0x0f)+'0';
    time_device[count++]=':';
    time_device[count++]=(Today.minutes>>4)+'0';
    time_device[count++]=(Today.minutes&0x0f)+'0';

}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Utils_Compare_Strings
 *
 * DESCRIPTION                  : Compares two strings. 
 *
 * RETURN                               :  2 --> first_string < second_string
 *                                         0 --> first_string = second_string
 *                                         1 --> first_string > second_string
 *      
 * NOTES                                : t.c.a. 25/05/99
 *
 * ------------------------------------------------------------------------ */
int Utils_Compare_Strings(char *first_string, char *second_string)
{
	char first_char_array[21], second_char_array[21];
	int i;

	memcpy(first_char_array, first_string, strlen(first_string));
	memcpy(second_char_array, second_string, strlen(second_string));
	first_char_array[strlen(first_string)] = 0;
	second_char_array[strlen(second_string)] = 0;

	Utils_LeftPad(first_char_array, '0', 20);
	Utils_LeftPad(second_char_array, '0', 20);

	for (i=0;i<20;i++) 
	{
		if (first_char_array[i] != second_char_array[i])
		{
			if (first_char_array[i] > second_char_array[i])
			   return(1);
			else
			   return(2);
		}

	}

	return(0);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Utils_Subtract_Strings
 *
 * DESCRIPTION                  : Substract the second_string from first_string. 
 *
 * RETURN                       :
 *      
 * NOTES                        : t.c.a. 25/05/99
 *
 * ------------------------------------------------------------------------ */
void Utils_Subtract_Strings(char *first_string, char *second_string, char *result_string)
{
	char first_char_array[21], second_char_array[21], result_char_array[20];
	int i, subtract_one_or_zero;
	boolean paste_digit,result_index;

	memcpy(first_char_array, first_string, strlen(first_string));
	memcpy(second_char_array, second_string, strlen(second_string));
	first_char_array[strlen(first_string)] = 0;
	second_char_array[strlen(second_string)] = 0;

	Utils_LeftPad(first_char_array, '0', 20);
	Utils_LeftPad(second_char_array, '0', 20);

	/*memcpy(result_char_array,'0',20);*/	/*UPDATE 01_15 24/10/99 memcpy yanlýþ kullanýlor. Aþaðýda memset e çevrildi*/
	memset(result_char_array,'0',20);
	subtract_one_or_zero = 0;

	for (i=19;i>=0;i--)
	{
		result_char_array[i] = ('0' + first_char_array[i] - second_char_array[i] - subtract_one_or_zero);
		if (result_char_array[i] < '0')
		{
			result_char_array[i] = result_char_array[i] + 10;
			subtract_one_or_zero = 1;
		}
		else
			subtract_one_or_zero = 0;
	}

	paste_digit = FALSE;
	result_index = 0;

	for (i=0;i<20;i++)
	{
		if (paste_digit)
		{
			result_string[result_index++] = result_char_array[i];
		}
		else 
		{
			if (!(result_char_array[i] == '0'))
			{
				result_string[result_index++] = result_char_array[i];
				paste_digit = TRUE;
			}
		}
	}

	result_string[result_index] = 0;

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME                : Utils_Add_Strings
 *
 * DESCRIPTION                  : Adds two strings. 
 *
 * RETURN                               :
 *      
 * NOTES                                : t.c.a. 25/05/99
 *
 * ------------------------------------------------------------------------ */
void Utils_Add_Strings(char *first_string, char *second_string, char *result_string)
{
	char first_char_array[21], second_char_array[21], result_char_array[22];
	int i, add_one_or_zero;
	boolean paste_digit,result_index;

	memcpy(first_char_array, first_string, strlen(first_string));
	memcpy(second_char_array, second_string, strlen(second_string));
	first_char_array[strlen(first_string)] = 0;
	second_char_array[strlen(second_string)] = 0;

	Utils_LeftPad(first_char_array, '0', 20);
	Utils_LeftPad(second_char_array, '0', 20);

	memset(result_char_array,'0',21);
	result_char_array[21] = 0;
	add_one_or_zero = 0;

	for (i=19;i>=0;i--)
	{
		result_char_array[i+1] = (first_char_array[i] + second_char_array[i] - '0' + add_one_or_zero);
		if (result_char_array[i+1] > '9')
		{
			result_char_array[i+1] = result_char_array[i+1] - 10;
			add_one_or_zero = 1;
		}
		else
			add_one_or_zero = 0;
	}

	if (add_one_or_zero == 1)
		result_char_array[0] = '1';

	paste_digit = FALSE;
	result_index = 0;

	for (i=0;i<22;i++)
	{
		if (paste_digit)
		{
			result_string[result_index++] = result_char_array[i];
		}
		else 
		{
			if (!(result_char_array[i] == '0'))
			{
				result_string[result_index++] = result_char_array[i];
				paste_digit = TRUE;
			}
		}
	}

	result_string[result_index] = 0;

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_SetDateTime.
 *
 * DESCRIPTION          : set date and time
 *
 * RETURN                       : none     
 *
 * NOTES                        :       r.i.o & t.c.a  12/06/99
 *
 * --------------------------------------------------------------------------*/
void Utils_SetDateTime(char *DateTime)
{
    real_time_clock Today;


	Today.day =     (DateTime[4] -'0')*16 + DateTime[5] -'0';
	Today.month =   (DateTime[2] -'0')*16 + DateTime[3] -'0';
	Today.year =    (DateTime[0] -'0')*16 + DateTime[1] -'0';
	Today.hours =   (DateTime[6] -'0')*16 + DateTime[7] -'0';
	Today.minutes = (DateTime[8]- '0')*16 + DateTime[9] -'0';
	Today.seconds = 0;
	Rtc_SetTime(&Today); 
	Rtc_SetDate(&Today);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_AskOperatorPassword
 *
 * DESCRIPTION          : Ask the Operator password
 *
 * RETURN                       : TRUE, FALSE
 *
 * NOTES                        :       r.i.o & t.c.a  16/06/99
 *
 * --------------------------------------------------------------------------*/
boolean Utils_AskOperatorPassword(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len=0;
	char EnteredOperatorPassword[15];
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	strcpy(OperatorPassword, "47326848773614");

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));
	memset(Buffer, 0 , LEN_GETKEYBOARD);
	if(!Mainloop_GetKeyboardInput(" ž˜FREY˜ G˜R˜N ?",Buffer,14,14,
				       FALSE, FALSE, TRUE, FALSE, TRUE,&Len,value_null, FALSE))
/*	if(!Mainloop_GetKeyboardInput(" ž˜FREY˜ G˜R˜N ?",Buffer,0,14,
				       FALSE,TRUE, TRUE, FALSE, TRUE,&Len,0, FALSE))*/
	{
			return(FALSE);
	}
    else
	{
		memcpy(EnteredOperatorPassword,Buffer,Len);
		EnteredOperatorPassword[Len]=0;

		if (Utils_Compare_Strings(EnteredOperatorPassword, OperatorPassword) == 0)
		{
			return(TRUE);
		}
		else
		{
			Mainloop_DisplayWait("  YANLIž ž˜FRE  ",2);
			return(FALSE);
		}

	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_AskOperatorPassword
 *
 * DESCRIPTION          : Ask the Operator password
 *
 * RETURN                       : TRUE, FALSE
 *
 * NOTES                        :       r.i.o & t.c.a  16/06/99
 *
 * --------------------------------------------------------------------------*/
boolean Utils_AskNOSPassword(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len=0;
	char EnteredOperatorPassword[15];
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	strcpy(OperatorPassword, "010471");

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));
	memset(Buffer, 0 , LEN_GETKEYBOARD);
	if(!Mainloop_GetKeyboardInput(" ž˜FREY˜ G˜R˜N ?",Buffer,6,6,
				       FALSE, FALSE, TRUE, FALSE, TRUE,&Len,value_null, FALSE))
/*	if(!Mainloop_GetKeyboardInput(" ž˜FREY˜ G˜R˜N ?",Buffer,0,14,
				       FALSE,TRUE, TRUE, FALSE, TRUE,&Len,0, FALSE))*/
	{
			return(FALSE);
	}
    else
	{
		memcpy(EnteredOperatorPassword,Buffer,Len);
		EnteredOperatorPassword[Len]=0;

		if (Utils_Compare_Strings(EnteredOperatorPassword, OperatorPassword) == 0)
		{
			return(TRUE);
		}
		else
		{
			Mainloop_DisplayWait("  YANLIž ž˜FRE  ",2);
			return(FALSE);
		}

	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_AskUserPassword
 *
 * DESCRIPTION          : Ask the User password
 *
 * RETURN                       : none     
 *
 * NOTES                        : r.i.o & t.c.a  16/06/99
 *
 * --------------------------------------------------------------------------*/
boolean Utils_AskUserPassword(void)
{
	char Buffer[20];
	byte Len;
	char EnteredUserPassword[5];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];
//	strcpy(UserPassword, "4321");
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));


	if(!Mainloop_GetKeyboardInput(" ž˜FREY˜ G˜R˜N ?",Buffer,4,4,
				       FALSE, FALSE, TRUE, FALSE, TRUE,&Len,value_null, FALSE))
	{
			return(FALSE);
	}
    else
	{

		memcpy(EnteredUserPassword,Buffer,Len);
		EnteredUserPassword[Len]=0;

		if (Utils_Compare_Strings(EnteredUserPassword, UserPassword) == 0)
		{
			return(TRUE);
		}
		else
		{
			Mainloop_DisplayWait("  YANLIž ž˜FRE  ",2);
			return(FALSE);
		}
	
	}
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_AskSoftPassword
 *
 * DESCRIPTION          : Ask the User password
 *
 * RETURN               : none     
 *
 * NOTES                : r.i.o & t.c.a  26/07/99
 *
 * --------------------------------------------------------------------------*/
boolean Utils_AskSoftPassword(void)
{
	char Buffer[20];
	byte Len;
	char EnteredUserPassword[5];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	strcpy(SoftPassword, "1971");


	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));


	if(!Mainloop_GetKeyboardInput(" ž˜FREY˜ G˜R˜N ?",Buffer,4,4,
				       FALSE, FALSE, TRUE, FALSE, TRUE,&Len,value_null, FALSE))
	{
			return(FALSE);
	}
    else
	{

		memcpy(EnteredUserPassword,Buffer,Len);
		EnteredUserPassword[Len]=0;

		if (Utils_Compare_Strings(EnteredUserPassword, SoftPassword) == 0)
		{
			return(TRUE);
		}
		else
		{
			Mainloop_DisplayWait("  YANLIž ž˜FRE  ",2);
			return(FALSE);
		}
	
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_AskMerchatPassword
 *
 * DESCRIPTION          : Ask the Merchant password
 *
 * RETURN               : TRUE, FALSE
 *
 * NOTES                :r.i.o   28/08/02
 *
 * --------------------------------------------------------------------------*/
boolean Utils_AskMerchantPassword(void)
{
	char Buffer[LEN_GETKEYBOARD];
	byte Len=0;
	char EnteredMerchantPassword[13];
	char merch_no_reformatted[10];
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* reformat the merchant no for 9 digits long */
	memset(merch_no_reformatted, 0, sizeof(merch_no_reformatted));
	memcpy(merch_no_reformatted, merch_no, 9);
	
	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));
	memset(Buffer, 0 , LEN_GETKEYBOARD);
	if(!Mainloop_GetKeyboardInput(" TEDARIKCI KODU?",Buffer,1,9,
				       FALSE, FALSE, TRUE, FALSE, TRUE,&Len,value_null, FALSE))
	{
		return(FALSE);
	}
    else
	{
		memcpy(EnteredMerchantPassword,Buffer,Len);
		EnteredMerchantPassword[Len]=0;
		/* 04_04 22/11/2002 */
		Utils_LeftPad(EnteredMerchantPassword, '0', 9);

		if (Utils_Compare_Strings(EnteredMerchantPassword, merch_no_reformatted) == 0)
		{
			return(TRUE);
		}
		else
		{					/*   "123456789012345601234567890123456"*/	
			Mainloop_DisplayWait(" TEDARIKCI KODU      YANLIS      ",2);
			return(FALSE);
		}

	}
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_ChangeUserPassword
 *
 * DESCRIPTION          : Change the User password
 *
 * RETURN                       : none     
 *
 * NOTES                        : r.i.o & t.c.a  29/06/99
 *
 * --------------------------------------------------------------------------*/
boolean Utils_ChangeUserPassword(void)
{
	char Buffer[20];
	byte Len;
	char EnteredUserPassword1[5],EnteredUserPassword2[5];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	char value_null[2];

	/* 1_18 29/03/2000 Key giriþinde null giriþi için kullanýlýyor. */
	memset(value_null, 0 , sizeof(value_null));


	if(!Mainloop_GetKeyboardInput("  YEN˜ ž˜FRE ?  ",Buffer,4,4,
				       FALSE, FALSE, TRUE, FALSE, TRUE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("    ž˜FRE       DE¦˜žT˜R˜LEMED˜ ",2);
			return(FALSE);
	}

	memcpy(EnteredUserPassword1,Buffer,Len);
	EnteredUserPassword1[Len]=0;


	if(!Mainloop_GetKeyboardInput("  YEN˜ ž˜FRE ?  ",Buffer,4,4,
				       FALSE, FALSE, TRUE, FALSE, TRUE,&Len,value_null, FALSE))
	{
		Mainloop_DisplayWait("    ž˜FRE       DE¦˜žT˜R˜LEMED˜ ",2);
			return(FALSE);
	}

	memcpy(EnteredUserPassword2,Buffer,Len);
	EnteredUserPassword2[Len]=0;


	if (Utils_Compare_Strings(EnteredUserPassword1, EnteredUserPassword2) == 0)
	{
		memcpy(UserPassword, EnteredUserPassword2, 4);
		UserPassword[4] = 0;
		Mainloop_DisplayWait("    ž˜FRE        DE¦˜žT˜R˜LD˜   ",2);
			return(TRUE);
	}
	else
	{
		Mainloop_DisplayWait("    ž˜FRE       DE¦˜žT˜R˜LEMED˜ ",2);
			return(FALSE);
	}
	

}




/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_PinPadInitial
 *
 * DESCRIPTION          : Loads the Master Keys to the Pin Pad
 *
 * RETURN                       : STAT_OK, STAT_NOK  
 *
 * NOTES                        : r.i.o & t.c.a  16/06/99
 *
 * --------------------------------------------------------------------------*/
byte Utils_PinPadInitial(void)
{
	char master_key_table[10][16];
	
	/* Master Keyler Yüklenmiyor */
	/*memcpy(master_key_table[0], "9812765497285691", 16);
	memcpy(master_key_table[1], "1111111122222222", 16);
	memcpy(master_key_table[2], "1234322345434567", 16);
	memcpy(master_key_table[3], "9182917722451928", 16);
	memcpy(master_key_table[4], "3491857293746298", 16);
	memcpy(master_key_table[5], "8938474524516273", 16);
	memcpy(master_key_table[6], "8373651425423637", 16);
	memcpy(master_key_table[7], "4847462242412627", 16);
	memcpy(master_key_table[8], "9796785764652342", 16);
	memcpy(master_key_table[9], "5537378284428293", 16);

	for (i=0; i<10; i++)
	{
		if (Utils_TransferMasterKey ((byte *)master_key_table[i], i) != STAT_OK)
			return (STAT_NOK);
	}*/

	if (Host_PinpadIdlePrompt() == STAT_OK)
		return (Host_SelectLanguage());

	return (STAT_NOK);

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_TransferMasterKey
 *
 * DESCRIPTION          : Transfers a Master Key to the Pin Pad
 *
 * RETURN                       : STAT_OK, STAT_NOK, STAT_NO_CONNECTION      
 *
 * NOTES                        : r.i.o & t.c.a  16/06/99
 *
 * --------------------------------------------------------------------------*/
byte Utils_TransferMasterKey (byte *key, int key_address)
{
	byte Status;
	char WrkBuf[30];

	memset (WrkBuf,0,30);
	memcpy (WrkBuf,  "02", 2);
	WrkBuf[2] = key_address + '0';

	memcpy (WrkBuf+3, (char *)key, 16);

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
 *
 * FUNCTION NAME        : Utils_SelectMasterKey
 *
 * DESCRIPTION          : Selects the Master Key
 *
 * RETURN                       : STAT_OK, STAT_NOK 
 *
 * NOTES                        : r.i.o & t.c.a  16/06/99
 *
 * --------------------------------------------------------------------------*/
/*byte Utils_SelectMasterKey (char *key_address)*/	/*UPDATE 01_15 24/10/99 */
byte Utils_SelectMasterKey (void)
{
	byte Status;
	char WrkBuf[20];

	memset (WrkBuf,0,20);
	memcpy (WrkBuf,  "08", 2);
	WrkBuf[2] = master_key_no;

/* Toygar 13.05.2002 Start */
	if (BUYUKFIRMA) Status = Host_SendReceive(WrkBuf,FALSE,FALSE,FALSE);
	else Status = Host_SendReceive(WrkBuf,FALSE,FALSE,TRUE);
/* Toygar 13.05.2002 End */

	if (Status == EOT)
		   return (STAT_OK);
	return (STAT_NOK);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Trans_AnalyseCancelReceive
 *
 * DESCRIPTION          : Analyzes the void response message
 *
 * RETURN                       : none.
 *
 * NOTES                        : r.i.o & t.c.a 22/06/99
 *
 * ------------------------------------------------------------------------ */
/*void Utils_DisplayBalance(void)
{

	char temp_buff[17];
	char balance_message[33];


	memcpy(temp_buff,d_Transaction_Receive.message,16);
	temp_buff[16] = 0;
	sprintf(balance_message,"     BAKÝYE     %s",temp_buff);
	Mainloop_DisplayWait(balance_message,10);

}*/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_PinPadConnection
 *
 * DESCRIPTION          : Checks communication between POS & Pinpad
 *
 * RETURN               : STAT_OK, STAT_NOK 
 *
 * NOTES                : 
 *
 * --------------------------------------------------------------------------*/
/*byte Utils_PinpadConnection (void)
{
	byte Status;
	char WrkBuf[10];

	memset (WrkBuf,0,10);
	memcpy (WrkBuf, "11", 2);

	Status = Host_SendReceive(WrkBuf, FALSE, FALSE, FALSE);
	if (Status == ACK)
		   return (STAT_OK);
	return (STAT_NOK);
}*/

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_Debit_Check
 *
 * DESCRIPTION          : Debit check
 *
 * RETURN                       : STAT_OK, STAT_NOK 
 *
 * NOTES                : 
 *
 * --------------------------------------------------------------------------*/
boolean Utils_Debit_Check (void)
{
	sint i, j;

	for (i=0; i<7; i++)		/* 06_05 10 yerine 7 yapýldý. */
	{
		for (j=0; j<10; j++)	/* verde 30 yerine 10 yapýldý */
		{
			if (strlen(debit_prefix[i][j]) == 0) 
				break;

			if (memcmp(card_no, debit_prefix[i][j], strlen(debit_prefix[i][j])) == 0)
				return(TRUE);
		}
	}

	return(FALSE);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : ReadSYS_DEVICE_ID
 *
 * DESCRIPTION          : Get the device ID.
 *
 * RETURN				: none
 *
 * NOTES                : r.i.o. 01/08/2001 
 *
 * --------------------------------------------------------------------------*/
void ReadSYS_DEVICE_ID(char *pay_device_id)
{
	/** generates device id **/
	memcpy(pay_device_id,merch_device_no,8);
	pay_device_id[8] = 0;	/** XLS_INTEGRATION2 11/10/2001 irfan. null ile sonlandiriliyor **/

}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_SetPwrOnAlarm 
 *
 * DESCRIPTION          : 
 *
 * RETURN               : 
 *
 * NOTES                : 
 *
 * --------------------------------------------------------------------------*/
byte Utils_SetPwrOnAlarm (char *auto_date_c, char *auto_time_c)
{
	real_time_clock Today;

	Today.day =     (auto_date_c[0] -'0')*16 + auto_date_c[1] -'0';
	Today.month =   (auto_date_c[3] -'0')*16 + auto_date_c[4] -'0';
	Today.year =    (auto_date_c[6] -'0')*16 + auto_date_c[7] -'0';
	Today.hours =   (auto_time_c[0] -'0')*16 + auto_time_c[1] -'0';
	Today.minutes = (auto_time_c[2]-'0')*16 + auto_time_c[3]-'0';
	Today.seconds = 0;
	
	RtcUtil_ResetPwrOnAlarm();
	if( RtcUtil_SetPwrOnAlarm(&Today)==RTCUTIL_ALARM_SET_OK)
	{
		return TRUE;
	}
	return FALSE;
}

/***** genius  ***************/
/* --------------------------------------------------------------------------------------------
 *
 * FUNCTION NAME        : Utils_NumericCheck
 *
 * DESCRIPTION          : Checks that the string is numerci or not.
 * PARAMETERS			: pstring : string to be checked.
 *                        plength : length of the string.
 *
 * RETURN               : STAT OK : String is numerci , STAT_NOK: string is not numeric
 *
 * NOTES                : r.i.o. 12/10/2002
 *
 * -------------------------------------------------------------------------------------------- */
byte Utils_NumericCheck(char *pstring, int plength)
{
	int i,j;

	for (i=0;i<plength;i++)
	{
		if(!(pstring[i] == '0' || pstring[i] == '1' || pstring[i] == '2' || pstring[i] == '3' || pstring[i] == '4' ||
			pstring[i] == '5' || pstring[i] == '6' || pstring[i] == '7' || pstring[i] == '8' || pstring[i] == '9') )
		{
			return (STAT_NOK);
		}
	}
	return (STAT_OK);

}

/* -------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_PrintImage
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------*/
void Utils_PrintImage(byte* cPrintBuf,int MargenIzq, int Ancho, int NumLineas)
{
	int nRightLim;
	int i, j;
	char bufaux[320];
	int nBitsMaxPrintWidth = 144;

	PrntUtil_Cr(1);
	Printer_LineSpacing (0, 0);

	nRightLim = MargenIzq + Ancho;
	for (i=0; i < NumLineas; i++)
	{
	  for (j=0; j<MargenIzq; j++) { bufaux[2*j] = GRAPHICS; bufaux[2*j+1] =
0; }
	  for (j=MargenIzq; j<nRightLim; j++)
	  	{ bufaux[2*j] = GRAPHICS; bufaux[2*j+1] = cPrintBuf[i*Ancho +
j-MargenIzq];}
	  for (j=nRightLim; j<nBitsMaxPrintWidth; j++)
		{ bufaux[2*j] = GRAPHICS; bufaux[2*j+1] = 0; }
	  Slips_Printer(bufaux, nBitsMaxPrintWidth * 2);
	}
	PrntUtil_Cr(1);
}
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Get Input From Keyboard1
 *
 * DESCRIPTION: Gets input from keyboard. 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
boolean Utils_GetKeyboardInput1 ( const char *UpperLine, char *Buffer, 
								    sint DownLimit, sint UpLimit,
									boolean First, boolean Restricted,
									boolean Masked, boolean Comma,
									boolean Alpha, byte *Len, char *ExDefault, 
									boolean EmptyOk, boolean DisplMessage)
{
	sint i,rightZer;
	boolean RestrictedFlag,DefaultFirst;
	char MaskedBuffer[LG_GETKEYBOARD],Default[LG_GETKEYBOARD];
	char ToDisplay[30];
	byte ret;
	int MaskedLen;
	ulint waitTimeout;
	
	ExtendKb_Enable();
	SoftKB_Enable();
	
	Debug_SetFont(FONT_13X7);

	if (!Comma&&!DisplMessage) EmvIF_ClearDsp(DSP_MERCHANT);

	strcpy(Default,ExDefault);

	DefaultFirst = FALSE;
	if ((Default == 0) || (!isalnum(Default[0])))
	
	if (!First)
	{
		i = 0;
		memset(Buffer,' ',LG_PAN);
	}
	else
	{
		i = 1;
		memset(Buffer+1,' ',LG_PAN-1+4);
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
		{
			if (Comma||DisplMessage)	
					Debug_GrafPrint2((char *)UpperLine,3);
			else	Debug_GrafPrint2((char *)UpperLine,2);
		}
	    else
		{
			Debug_SetFont(FONT_10X7);
			memcpy(ToDisplay,UpperLine,16);
			ToDisplay[16] = 0;
			Debug_GrafPrint2 ((char *)ToDisplay, 2);

			Debug_SetFont(FONT_13X7);
			memcpy(ToDisplay,UpperLine+16,16);
			ToDisplay[16] = 0;
			Debug_GrafPrint2 ((char *)ToDisplay, 3);
		}

		if (Masked&&!Comma)
		{
			memset(MaskedBuffer,0,LG_GETKEYBOARD);
			memcpy(MaskedBuffer,csSTARS,i);
			Debug_GrafPrint2 ((char *)MaskedBuffer, 4);
		}
		else
		{
			if (Comma) 
			{
				if(Masked)
				{
					Utils_WithComma(Buffer,MaskedBuffer,UpLimit,FALSE);
					MaskedLen = strlen(MaskedBuffer);
					for(rightZer=0; rightZer<strlen(MaskedBuffer); rightZer++)
					if(MaskedBuffer[rightZer]==' ')	MaskedBuffer[rightZer]='0';
				}
				else
				{
					Utils_WithComma(Buffer,MaskedBuffer,i,TRUE);
					MaskedLen = strlen(MaskedBuffer);
				}
				if(!EmptyOk)
				{
					if(Masked)
						MaskedBuffer[MaskedLen++] = '%';
					else
						MaskedBuffer[MaskedLen++] = 0xBE;
					MaskedBuffer[MaskedLen] = 0;
				}
				Utils_LeftPad(MaskedBuffer,' ',16);
				Debug_GrafPrint2 ((char *)MaskedBuffer, 3);
			}
			else if (i>16) 
				Debug_GrafPrint2((char *)Buffer + (i-16),3);/* for scrolling */
		     else 
				Debug_GrafPrint2((char *)Buffer,3);/* for scrolling */
		}
		if (Comma) 
		{
			waitTimeout=Scan_MillSec();
			do{	
				if((Scan_MillSec() - waitTimeout) > 29*1000)	
				{
					ret=0xBF;
					break;
				}
				
				ret = ExtendKb_WaitForKey();
				
				if(ret)	break;
			} while(((Scan_MillSec() - waitTimeout) < 30*1000)); 
		}		
		else	ret = ExtendKb_WaitForKey();
	
		if(ret!=0xBC && ret!=0xC1)
		{
			switch (ret)
			{  
			/*
			case MMI_CLEAR :
				memset(Buffer,0,LG_GETKEYBOARD);
				if (Comma)
				{
					strcpy(Buffer,Default);
					i = strlen(Buffer);
				}
				else	i=0;
				DefaultFirst = TRUE;
				break;*/
			case 0x08 :
				if (i > 0) i --;
				DefaultFirst = FALSE;
				if (Comma&&Masked) 
				{
					Buffer[i] = '0';
					if (i == 0)
					{
						strcpy(Buffer,Default);
						i=strlen(Buffer);
						continue;
					}
				}
				else	Buffer[i] = ' ';
				break;
			case 0xc6 :
				if ((Alpha) && (i>0)) Buffer[i-1] = Mainloop_GetAlpha(Buffer[i-1],TRUE);
				break;
			case 0xc5 :
	
				if ((Alpha) && (i>0)) Buffer[i-1] = Mainloop_GetAlpha(Buffer[i-1],FALSE);		  
				break;
/*				
			case MMI_POINT :
				if ((i!=0) && (Comma))
				{
				  DefaultFirst = FALSE;
				  if (i < UpLimit) Buffer[i++] = '0';
				  if (i < UpLimit) Buffer[i++] = '0';
				  if (i < UpLimit) Buffer[i++] = '0';
				}
				break;*/		
			default:
		
					if ((DefaultFirst) && (!First))
					{
						i = 0;
						memset(Buffer,' ',LG_PAN+4);
					}
					if (i < UpLimit)
					{
						DefaultFirst = FALSE;
						/*
						if(i==0)
						{
							if(ret!='0')Buffer[i++] = ret;
						}
						else */
						Buffer[i++] = ret;
					}
			}
			
			if (Restricted)
			{
				if ((i==UpLimit) && (ret == MMI_ENTER) && ((Buffer[0] != ' ') || (EmptyOk)))
					RestrictedFlag = FALSE;
				else if (ret == 0xBF)
				{
				 	RestrictedFlag = FALSE;
				}
			}
			else if ((ret == 0xBF) ||  ((ret == MMI_ENTER) && ((Buffer[0] != ' ') || (EmptyOk))
					&& (i>=DownLimit) && (i<=UpLimit))) 
			{
				RestrictedFlag = FALSE;
			}
		}
	} while(RestrictedFlag);
	*Len = i;
	
	SoftKB_Disable();
	ExtendKb_Disable();
	EmvIF_ClearDsp(DSP_MERCHANT);
	
	return (ret == MMI_ENTER);
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_GetEditScreen1
 *
 * DESCRIPTION: 
 *
 * RETURN: 
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte Utils_GetEditScreen1(byte bEditType,char *ExtDefaultStr, char *Buffer,sint UpLimit)
{
	sint DownLimit;
	boolean First,Restricted,Masked, Comma, Alpha; 
	byte Len;
	char DefaultStr[50];
	boolean EmptyOk, DisplMessage;
	char tempstr[20];
	EmvIF_ClearDsp(DSP_MERCHANT);

	DownLimit=0;
	First=Restricted=Masked=Comma=Alpha=EmptyOk=DisplMessage=FALSE; 
	memset(DefaultStr,0,50);

	if(ExtDefaultStr[0] != 0 && (ExtDefaultStr != 0))
		strcpy(DefaultStr,ExtDefaultStr);

	memset(tempstr,0,20);
	switch(bEditType)
	{
/*		case 0:
			memcpy(DefaultStr,ExtDefaultStr,16);
			DefaultStr[16]=0;
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			strcpy(DefaultStr,"TUTAR ?");
			DownLimit=1;
			Comma=TRUE;
		    break;
		case 1:
			strcat(DefaultStr,"MŸteri žifresi?");
			UpLimit=LG_PIN;
			Masked=TRUE;
			Restricted=TRUE;
			break;
		case 2:
			strcat(DefaultStr,csMESS_ENTER_PIN);
			UpLimit=LG_PASSWORD;
			Restricted=TRUE;
			Masked=TRUE;break;
		case 3:
			strcpy(DefaultStr,"Child?");
			UpLimit=3;
			break;
		case 4:
			strcpy(DefaultStr,"PAX?");
			UpLimit=3;
			break;
		case 5:
			strcat(DefaultStr,csMESS_OUTSIDE);
			UpLimit=LG_OUTSIDE;
			EmptyOk=TRUE;
			break;
		case 6:
			strcat(DefaultStr,csMESS_HOST_PHONE1);
			UpLimit=LG_PHONE;
			break;
		case 7:
			strcat(DefaultStr,csMESS_HOST_PHONE2);
			UpLimit=LG_PHONE;
			break;
		case 8:
			strcat(DefaultStr,csMESS_HOST_PHONE3);/*SMS PHONE*/
/*			UpLimit=LG_PHONE;
			break;
		case 9:
			strcat(DefaultStr,csMESS_HOST_PHONE4);/*VOICE CALL*/
/*			UpLimit=LG_PHONE;		break;
		case 10:
			strcat(DefaultStr,csMESS_ENTER_AUTHCODE);
			UpLimit=LG_AUTHCODE;
			Alpha=TRUE;
			Restricted=TRUE;
			break;
		case 11:
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)ExtDefaultStr,2);
			memset(DefaultStr,' ',50);
			memcpy(DefaultStr,ExtDefaultStr,strlen(ExtDefaultStr));
			strcpy(DefaultStr+16,"REF.NUM.G˜R˜N˜Z");
			UpLimit=LG_REFNO;
			break;
		case 12:
			memcpy(DefaultStr,ExtDefaultStr,16);
			DefaultStr[20]=0;
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			First=TRUE;
			strcpy(DefaultStr,csMESS_MANUAL_CARD);
			DownLimit=13;
			UpLimit=LG_PAN-1;
			DisplMessage=TRUE;
			break;
		case 13:
			strcpy(DefaultStr,"MŸteri Ad/Soyad");
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			
			UpLimit=20;
			DisplMessage=TRUE;
			Alpha=TRUE;
			break;
		case 14:
			strcat(DefaultStr,"TAR˜H?");
			DownLimit=6;
			UpLimit=10;
			Alpha=TRUE;
			break;
		case 15:
			strcat(DefaultStr,"CHD?");
			UpLimit=2;
			break;
			
		case 16:
			strcat(DefaultStr,"˜žYER˜ No:");
			UpLimit=LG_MERCHID0-3;
			Restricted=TRUE;
			break;
		case 17:
			strcat(DefaultStr,csMESS_ENTER_TAKSIT);
			UpLimit=2;
			break;
		case 18:
			memcpy(DefaultStr, ExtDefaultStr, 16);
			DefaultStr[16]=0;
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			strcpy(Buffer, "0000");
			strcpy(DefaultStr,"PUAN KATKI PAYI?");
			DownLimit=1;
			UpLimit=4;
			Comma=TRUE;
			Masked=TRUE;
			break;
		case 19:
			strcpy(DefaultStr,"Taksit tarihi?");
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			strcpy(DefaultStr,"GGAA giriniz");
			UpLimit=LG_EXPDATE;
			Restricted=TRUE;
			DisplMessage=TRUE;
			break;
		case 20:
			strcpy(DefaultStr,"˜žLEM YAPAB˜LMEK ˜€˜N");
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			strcpy(DefaultStr,"KAS˜YER NUMARASI");
			UpLimit=LG_EXPDATE;
			Restricted=TRUE;
			DisplMessage=TRUE;
			break;
		case 24:
			strcpy(DefaultStr,"ESKI PIN?");
		case 23:
			if(bEditType==23)
			{
				memset(DefaultStr,0,50);
				strcpy(DefaultStr,"2.");
			}
		case 22:
			if(bEditType==22)
			{
				memset(DefaultStr,0,50);
				strcpy(DefaultStr,"1.");
			}
		case 21:
			if(bEditType!=24 )
			{
				if(bEditType!=21 )	strcat(DefaultStr,"YENI PIN?");
				else			strcat(DefaultStr,"PIN?");
			}
			UpLimit=4;
			Masked=TRUE;
			Restricted=TRUE;
			break;
		case 25:
			strcat(DefaultStr,"SMS:");
			UpLimit=24;
			Alpha=TRUE;
			break;
		case 26:
			
			memcpy(DefaultStr,"IN MINUTES (mm)",16);
			DefaultStr[16]=0;
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			First=TRUE;
			strcpy(DefaultStr,"STAND BY TIME?  ");
			UpLimit=3;
			DisplMessage=TRUE;
			break;
		case 27:
		
			strcat(DefaultStr,csMESS_ENTER_PIN);
			UpLimit=6;
			Restricted=TRUE;
			Masked=TRUE;break;
			
		case 28:
			Debug_SetFont(FONT_10X7);
			strcat(DefaultStr,"GšN SAYISINI G˜R˜N˜Z");
			Debug_GrafPrint2((char *)DefaultStr,2);
			memset(DefaultStr,0,sizeof(DefaultStr));
			UpLimit=2;
			DisplMessage=TRUE;break;
		
		case 30:
			strcat(DefaultStr,"MOB˜L ™DEME NO?");
			UpLimit=LG_AUTHCODE;
			Alpha=TRUE;
			Restricted=TRUE;
			break;
		case 32:
			strcat(DefaultStr,csMESS_ENTER_PIN);
			UpLimit=3;/*julian date*/
/*			Restricted=TRUE;
			Masked=TRUE;break;*/
		case 33:
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)"APN?",2);
//			strcpy(DefaultStr,"APN?");
			Alpha=TRUE;
			DisplMessage=TRUE;
			break;
		case 34:
			strcpy(DefaultStr,"HOST NAME?");
			UpLimit=24;
			Alpha=TRUE;
			break;
		case 35:


			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)"HOST IP?",2);
			UpLimit=16;
			Alpha=TRUE;
			DisplMessage=TRUE;
			break;

		case 36:
			/*Master Key ? strcat(DefaultStr,"APN?");*/
			UpLimit=16;
			Alpha=TRUE;
			break;
		case 37:
			Debug_SetFont(FONT_10X7);
//			Debug_GrafPrint2((char *)DefaultStr,3);
//			strcpy(DefaultStr,"KULLANICI ADI?");
			Debug_GrafPrint2((char *)"KULLANICI ADI?",2);
			Alpha=TRUE;
			DisplMessage=TRUE;
			break;
		case 38:
		 	
			Debug_SetFont(FONT_10X7);
//			Debug_GrafPrint2((char *)DefaultStr,3);
		 	strcpy(tempstr,"ÞÝFRE?");
			Utils_CharConv(tempstr,20);
//			strcpy(DefaultStr,tempstr);			
			Debug_GrafPrint2((char *)tempstr,2);
			Alpha=TRUE;
			DisplMessage=TRUE;
			break;
		case 39:

			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)"PORT NO?",2);
			Alpha=FALSE;
			DisplMessage=TRUE;
			break;
		case 40:
		 	memcpy(tempstr,"Gönderi Adresi:",20);
		 	Utils_CharConv(tempstr,20);
			strcpy(DefaultStr,tempstr);
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			
			UpLimit=20;
			DisplMessage=TRUE;
			Alpha=TRUE;
			break;
		case 41:
		 	memcpy(tempstr,"Gönderen:",20);
		 	Utils_CharConv(tempstr,20);
			strcpy(DefaultStr,tempstr);
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			
			UpLimit=20;
			DisplMessage=TRUE;
			Alpha=TRUE;
			break;
		case 42:
		 	memcpy(tempstr,"Alýcý:",20);
		 	Utils_CharConv(tempstr,20);
			strcpy(DefaultStr,tempstr);
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			
			UpLimit=20;
			DisplMessage=TRUE;
			Alpha=TRUE;
			break;
		case 43:
		 	memcpy(tempstr,"Gönderi Cinsi:",20);
		 	Utils_CharConv(tempstr,20);
			strcpy(DefaultStr,tempstr);
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			
			UpLimit=20;
			DisplMessage=TRUE;
			Alpha=TRUE;
			break;
		case 44:
		 	memcpy(tempstr,"Gönderi Kg:",20);
		 	Utils_CharConv(tempstr,20);
			strcpy(DefaultStr,tempstr);
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			UpLimit=2;
			DisplMessage=TRUE;
			break;
			
		case 45:
		 	memcpy(tempstr,"SMS þifresi:",20);
		 	Utils_CharConv(tempstr,20);
			strcpy(DefaultStr,tempstr);
			Debug_SetFont(FONT_10X7);
			Debug_GrafPrint2((char *)DefaultStr,2);
			
			UpLimit=20;
			DisplMessage=TRUE;
			Alpha=TRUE;
			break;
			

		default:return FALSE;
	}
	if (Utils_GetKeyboardInput1 (DefaultStr, Buffer, DownLimit, UpLimit,	First,Restricted,Masked, Comma, Alpha,
								&Len, Buffer, EmptyOk,DisplMessage))
	{
		Buffer[Len]=0;
		
		return TRUE;
	}
	
	return FALSE;
}
/* -------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_WithComma
 *
 * DESCRIPTION:	   YTL
 *
 * RETURN:
 *
 * NOTES:	   bm YTL 14/062004
 *
 * --------------------------------------------------------------------------*/
void Utils_WithComma( char *fpInStr, char *fpOutStr, int fcLength,
					  boolean CommaType )
{
	sint liOutLen, liInLen, i;
	sint liCounter, liOutMax;

	if ( CommaType )
	{
		liOutMax = fcLength + ( fcLength - 1 ) / 3;	/* length of output string */
		for ( liCounter = 0; liCounter < ( fcLength - 1 ) / 3; liCounter++ )
		{

			fpOutStr[liOutMax - liCounter * 4 - 1] =
				fpInStr[fcLength - liCounter * 3 - 1];

			fpOutStr[liOutMax - liCounter * 4 - 2] =
				fpInStr[fcLength - liCounter * 3 - 2];

			fpOutStr[liOutMax - liCounter * 4 - 3] =
				fpInStr[fcLength - liCounter * 3 - 3];
			fpOutStr[liOutMax - liCounter * 4 - 4] = ',';
		}
		liInLen = fcLength - liCounter * 3 - 1;
		liOutLen = liOutMax - liCounter * 4 - 1;
		for ( liCounter = liInLen; liCounter >= 0; liCounter-- )
			fpOutStr[liOutLen--] = fpInStr[liInLen--];
		if ( liOutMax == 0 )	/* For empty amount put 0 */
			fpOutStr[liOutMax++] = '0';
		fpOutStr[liOutMax] = 0;
	}
	else
	{
		liOutMax = fcLength + ( fcLength - 1 ) / 3;
		if ( ( fcLength != 0 ) && ( !( fcLength % 3 ) ) )
			liOutMax += 1;
		if ( liOutMax > 3 )
		{
			fpOutStr[liOutMax - 1] = fpInStr[fcLength - 1];
			fpOutStr[liOutMax - 2] = fpInStr[fcLength - 2];
			fpOutStr[liOutMax - 3] = '.';
		}
		for ( liCounter = 0; liCounter < ( fcLength - 1 ) / 3; liCounter++ )
		{

			fpOutStr[liOutMax - liCounter * 4 - 4] =
				fpInStr[fcLength - liCounter * 3 - 3];

			fpOutStr[liOutMax - liCounter * 4 - 5] =
				fpInStr[fcLength - liCounter * 3 - 4];

			fpOutStr[liOutMax - liCounter * 4 - 6] =
				fpInStr[fcLength - liCounter * 3 - 5];
			fpOutStr[liOutMax - liCounter * 4 - 7] = ',';
		}
		liOutLen = liInLen = fcLength % 3;
		for ( liCounter = liInLen; liCounter >= 0; liCounter-- )
			fpOutStr[liOutLen--] = fpInStr[liInLen--];
		if ( liOutMax >= 0 && liOutMax < 3 )
		{
			for ( i = 0; i <= 3 - liOutMax; i++ )
				fpOutStr[i] = '0';
			for ( i = 1; i <= liOutMax; i++ )
				fpOutStr[3 - i + 1] = fpInStr[fcLength - i];
			fpOutStr[1] = '.';
			liOutMax = 4;
		}
		fpOutStr[liOutMax] = 0;
	}
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Auto_CheckCallDateTime
 *
 * DESCRIPTION: Checks if it is auto call time.
 *
 * RETURN: lbStatus (TRUE orFALSE)
 *
 * NOTES:
 *
 * ------------------------------------------------------------------------ */
/*
boolean Auto_CheckCallDateTime (HDDStruct *lsHDDBuf, char *CurDateTime, boolean flag, 
							    boolean dldFlag, sint *DenSay)
{
	char NextDate[6], NextTime[6];
	char LastDate[6], LastTime[6];
	char TimeBuffer[10], DateTemp[10], TimeTemp[5];
	sint Time,Times;

	memcpy(LastDate,CurDateTime,6);
	memcpy(LastTime,CurDateTime+6,6);

	if (flag)
	{
		memcpy (DateTemp, lsHDDBuf->NextParamDldDate, 10);
		memcpy (TimeTemp, lsHDDBuf->NextParamDldTime,  5);
	}
	else 
	{
		memcpy (DateTemp, lsHDDBuf->NextProgDldDate, 10);
		memcpy (TimeTemp, lsHDDBuf->NextProgDldTime,  5);	
	}

	NextDate[0] = DateTemp[0];
	NextDate[1] = DateTemp[1];
	NextDate[2] = DateTemp[3];
	NextDate[3] = DateTemp[4];
	NextDate[4] = DateTemp[8];
	NextDate[5] = DateTemp[9];

	NextTime[0] = TimeTemp[0];
	NextTime[1] = TimeTemp[1];
	NextTime[2] = TimeTemp[3];
	NextTime[3] = TimeTemp[4];

	if(!dldFlag)
	{
		memcpy(TimeBuffer,LastTime+2,2);
		TimeBuffer[2] = 0;
		Time = atoi(TimeBuffer);

		memcpy(TimeBuffer,LastTime+4,2);
		TimeBuffer[2] = 0;
		Times = atoi(TimeBuffer);

		if( !(Time%3) && (Times <= 59 ))
		{			
			(*DenSay)++;
			return TRUE;
		}
		return FALSE;
	}

	NextTime[4] = 
	NextTime[5] = '0';
	if(	(!memcmp(LastDate,NextDate,6)) && (!memcmp(LastTime,NextTime,6)))
		return TRUE;
	else
	{
		NextTime[4] = '0';
		NextTime[5] = '3';
		if(	(!memcmp(LastDate,NextDate,6)) && (!memcmp(LastTime,NextTime,6)))
			return TRUE;
		else
		{
			NextTime[4] = '0';
			NextTime[5] = '5';
			if(	(!memcmp(LastDate,NextDate,6)) && (!memcmp(LastTime,NextTime,6)))
				return TRUE;
			else
			{
				NextTime[4] = '1';
				NextTime[5] = '5';
				if(	(!memcmp(LastDate,NextDate,6)) && (!memcmp(LastTime,NextTime,6)))
				return TRUE;
				else
				{
					NextTime[4] = '2';
					NextTime[5] = '5';
					if(	(!memcmp(LastDate,NextDate,6)) && (!memcmp(LastTime,NextTime,6)))
					return TRUE;
					else
					{
						NextTime[4] = '3';
						NextTime[5] = '5';
						if(	(!memcmp(LastDate,NextDate,6)) && (!memcmp(LastTime,NextTime,6)))
						return TRUE;
						else
						{
							NextTime[4] = '4';
							NextTime[5] = '5';
							if(	(!memcmp(LastDate,NextDate,6)) && (!memcmp(LastTime,NextTime,6)))
							return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}
*/


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: Utils_CmpTime
 *
 * DESCRIPTION: Compares two time formats.
 *
 * RETURN:TRUE or FALSE
 *
 * NOTES:	  
 *			  YYMMDDHHMM
 * ------------------------------------------------------------------------ */
boolean Utils_CmpDateTime( char *fpTime1, char *fpTime2 )
{
	int liCounter;

	for ( liCounter = 0; liCounter < 10; liCounter++ )
		if ( fpTime1[liCounter] > fpTime2[liCounter] )
			return ( TRUE );
		else if ( fpTime1[liCounter] < fpTime2[liCounter] )
			return ( FALSE );
	return ( FALSE );
}
#if 0
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: StrCmp
 *
 * DESCRIPTION: 
 *
 * RETURN:0
 *
 * NOTES:       
 *
 * -------------------------------------------------------------------------- */
int StrCmp( char *X, int Lx, char *Y, int Ly )
{
	int i;						/* first buffer index */
	int j;						/* second buffer index */

	i = j = 0;
	/* if first buffer is longer than second buffer then */
	if ( Lx > Ly )
	{
		/* suppress the unsignificant zero in the first buffer */
		for ( i = 0; ( i < ( Lx - Ly ) ) && ( X[i] == 0 ); i++ );
		/* if first buffer is still longer then */
		if ( i != Lx - Ly )
			/* first > second */
			return ( 1 );
	}

	/* if second buffer is longer than first buffer then */
	if ( Lx < Ly )
	{
		/* suppress the unsignificant zero in the second buffer */
		for ( j = 0; ( j < ( Ly - Lx ) ) && ( Y[j] == 0 ); j++ );
		/* if second buffer is still longer then */
		if ( j != Ly - Lx )
			/* second > first */
			return ( -1 );
	}

	/* here the two buffers have the same significant size
	 * for each character until the end of the buffers */
	for ( ; i < Lx; i++, j++ )
	{
		/* if char of the first buffer > char of the second buffer then */
		if ( X[i] > Y[j] )
			/* first > second */
			return ( 1 );
		/* else if char of the second buffer > char of the first buffer then */
		else if ( X[i] < Y[j] )
			/* second > first */
			return ( -1 );
	}
	/* first = second if we run until here */
	return ( 0 );
}
#endif

