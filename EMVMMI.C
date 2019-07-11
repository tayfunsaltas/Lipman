/****************************************************************************
 * FILE NAME:   emvmmi.C                                                    *
 * MODULE NAME: EMV (Europay/Mastercard/Visa Smart card application)        *
 * PROGRAMMER:  CL                                                          *
 * DESCRIPTION: EMV MMI submodule                                           *
 * REVISION:    01.00 01/10/99 13:34                                        *
 ****************************************************************************/

          /*==========================================*
           *         I N T R O D U C T I O N          *
           *==========================================*/
          /*==========================================*
           *             I N C L U D E S              *
           *==========================================*/

#include "project.def"
#include <string.h>
#include  INTERFAC_H
#include  LIPTYPES_DEF
#include  EMVMSG_H
#include  DEFINE_H
#include  DEBUG_H
#include  FILES_H
#include  UTILS_H
#include  ERRORS_H
#include  EMVDEF_H
#include  EMVDEFIN_H
#include  EMVDB_H
#include  EMVACT_H
#include  MAINLOOP_H
#include  NURIT_DEF
/*#include  RTC_H*/
#include  EMVIF_H
#include  SECURITY_H
#include VERDE_H

          /*==========================================*
           *           D E F I N I T I O N S          *
           *==========================================*/

#define MAX_MSG_LENGTH    17
#define MAX_MESSAGES      19
#define PINPAD_PORT      UART_A
#define XLSAID		"\xA0\x00\x00\x00\x78\x58\x4C\x53\x01"

boolean   PinPadPresent;

extern char master_key_no;
extern boolean BUYUKFIRMA;
extern char card_no[20];
extern int card_no_length;
extern char working_key[16];
extern char amount_surcharge[14];
extern boolean PIN_BLOCK;
extern char Not_Certified_Prefixes[35];
extern char encrypted_PIN[17];
extern boolean OnlinePinEntry;
boolean EMVAxess;

extern boolean EMVAxessStatu;	/* 19.07.2004 axess visa degisiklikleri */
extern char POS_Type; /* Default axess secili. 0:Axess  1:Akbank */ /* 20.07.2004 axess visa degisiklikleri */
extern boolean VERDE_TRANSACTION;	/* 1: VERDE transaction 0: Normal Transaction */
extern char glAmount_Req_Type;
extern boolean VERDE_TRX_CONTINUE;	/* verde 13/12/2002. TRUE: Trx. continue. FALSE: Trx. stop*/
extern char verde_amount[14];
extern char verde_discount[14];

          /*==========================================*
           *       P R I V A T E  T Y P E S           *
           *==========================================*/

          /*==========================================*
           *        P R I V A T E  D A T A            *
           *==========================================*/

          /*==========================================*
           *        P R I V A T E  D A T A            *
           *==========================================*/
          /*==========================================*
           *        M I S C E L L A N E O U S         *
           *==========================================*/
          /*==================================================*
           * I N C L U D E  P R I V A T E  F U N C T I O N S  *
           *==================================================*/
          /*==========================================*
           *   P U B L I C     F U N C T I O N S      *
           *==========================================*/

/* Toygar - NextEMV Start - Ok */
/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: PinPad_CheckConnection
 *
 * DESCRIPTION  : PinPad_CheckConnection
 *
 * RETURN       : STAT_OK, STAT_NOK 
 *
 * NOTES        : 
 *
 * --------------------------------------------------------------------------*
byte PinPad_CheckConnection(void)
{
	byte Status;
	char WrkBuf[20];

	memset (WrkBuf,0,20);
	memcpy (WrkBuf, "11", 2);
	WrkBuf[2] = 0;

	if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,FALSE,FALSE);
	else Status = Host_SendReceive (WrkBuf,FALSE,TRUE,FALSE);

	if (Status == ACK) return (STAT_OK);
	return (STAT_NOK);
}
*/

/****************************************************************************
 * FUNCTION NAME: EMVMMI_MenuAppl
 * DESCRIPTION:   provides interface with cardholder in form of menu in order
 *                to choose one from a list of mutually supported (by ICC
 *                and by Terminal) applications
 * RETURN:        EMV_OK    Application selected & Index returned by parameter SelAppIndex
 *                EMV_FAIL  User cancels selection
 ****************************************************************************/
byte EmvMmi_MenuAppl(EMVLanguages *pelsLangs, MutualAppList *pMutualApps, usint *pusAppIndex)
{
  usint     i;
  byte      *AddCaps = EmvDb_GetDataPointer(tagADDIT_TERML_CAPABILITIES);
  byte      bICTI, bByte, bShift;
  boolean   blPref;
  entry     my_entries[MAX_MUTUAL_APPL];
  menu      my_menu; 

  /*Initializing structures*/
  memset(&my_entries, 0, sizeof(entry) *  MAX_MUTUAL_APPL);
  memset(&my_menu, 0, sizeof(menu));

  for (i = 0; i < pMutualApps->bAppCount; i++)
  {
    bICTI   = pMutualApps->MutualApps[i].IssuerCodeTableIndex;
    if (bICTI > 0x0A)   /* Not defined in 96 specs - ignore it */
      blPref = FALSE;
    else
    {
      bByte   = (bICTI > 8) ? 3 : 4;
      bShift  = (bICTI > 8) ? (bICTI - 9) : (bICTI - 1);
      blPref  = *(AddCaps + bByte) & (1 << bShift);
    }

    my_entries[i].text = ((pMutualApps->MutualApps[i].AppPrefName[0] > 0) && blPref) ? 
      (char *)pMutualApps->MutualApps[i].AppPrefName : (char *)pMutualApps->MutualApps[i].AppLabel;
    my_entries[i].type = 0;
  }

  /*Hide other menu entries*/
  for (i = pMutualApps->bAppCount; i < MAX_MUTUAL_APPL; i++)
    my_entries[i].type  = 1;  

  my_menu.header        = pelsLangs->elLangs[pelsLangs->bCardholderIndex].acMessages[msg_EMV_SELECT_APPLICATION];
  my_menu.no_of_entries = pMutualApps->bAppCount;
  my_menu.menu_entries  = my_entries;
  my_menu.mode_flags    = DEFAULT_MODE;

	EMVAxess = FALSE;
  /* If an AID or PREFERRED NAME not available perform implicit selection */
  for (i = 0; i < pMutualApps->bAppCount; i++)
    if (my_entries[i].text[0] == 0) /* Avoiding missing AID and App Label */
    {
      *pusAppIndex = 0;		/* Application Index is zero based */
      return EMV_OK;
    }
		else
		{
/* Toygar EMVAxess Start */
/* 		 /*bm gec */
/*
			Printer_WriteStr(pMutualApps->MutualApps[i].AID);
			PrntUtil_Cr(1);
			Printer_WriteStr(XLSAID);
			PrntUtil_Cr(1);
			Printer_WriteStr(POS_Type);
			PrntUtil_Cr(1);
*/
/*
			if (!strcmp(my_entries[i].text,"XLS LOYALTY")) EMVAxess = TRUE;
*/
			if ( (!memcmp(pMutualApps->MutualApps[i].AID, XLSAID, sizeof(XLSAID))) && ((POS_Type == '4')||(POS_Type == '6')) ) 
					EMVAxess = TRUE; /* 20.07.2004 */
/* Toygar EMVAxess End */
		}
/* bm gec 	*/
/*
	if (EMVAxess) Printer_WriteStr("1-EMVAxess == TRUE;\n");
	else Printer_WriteStr("1-EMVAxess == FALSE;\n");
	Kb_WaitForKey(  );	   
*/
/*bm gec*/

/* */
/* Toygar - NextEMV Start */
/*
  if (bEMVTestMod == eTestEVALAuto)
  {
    *pusAppIndex = 0;
    return EMV_OK;
  }
  else 
*/
/* Toygar - NextEMV End */
	if (!(EMVAxess && (pMutualApps->bAppCount == 2)))
	{
  if (i = Formater_GoMenu(&my_menu, NUMBERS_SELECT))
  { 
    *pusAppIndex = --i;		/* Application Index is zero based */
    return EMV_OK;
  }
	}
	else
	{
/* bm gec */
/*
		Printer_WriteStr("EMVAxess Present/Hidden\n");
		Printer_WriteStr("0. Application Selected\n");
*/
/* bm gec */
		*pusAppIndex = 0;		/* Application Index is zero based */
		return EMV_OK;
	}
  return EMV_FAIL;
}




/****************************************************************************
 * FUNCTION NAME: EMVMMI_GetOption
 * DESCRIPTION:   Captures user preference on a question
 * RETURN:        TRUE	          Option selected
 *                FALSE					  Option declined
 * NOTES:         
 ****************************************************************************/
boolean EMVMMI_GetOption(EMVLanguages *pelsLangs, byte bDisplay, byte bOption)
{
	EmvIF_ClearDsp(bDisplay);
	EmvIF_DisplayWait(pelsLangs, bDisplay, bOption,NO_WAIT);
	return (Kb_WaitForKey() == MMI_ENTER);
}



/****************************************************************************
 * FUNCTION NAME: Emv_Amount
 * DESCRIPTION:   Amount entry for EMV
 * RETURN:        TRUE/FALSE -- confirmed/cancelled by user
 * NOTES:
 ****************************************************************************/
boolean Emv_Amount(EMVLanguages *pelsLangs)
{
	byte      abAmount[13];		
	byte      abAmountNum[6];		
	byte      abBuffer[13];
  ulint     ulAmBin;
  byte      bLen;
  byte      i;
	byte			*pbAmount = EmvDb_GetDataPointer(tagAUTH_AMOUNT_BIN);

	if( ( EMVAxess && !EMVAxessStatu) || !EMVAxess )	/* 19.07.2004 axess visa degisiklikleri */
	{
	/* Check presence of amount field */
	if (pbAmount != 0) return TRUE;

	memset(abBuffer, 0, sizeof(abBuffer));
	EmvIF_ClearDsp(DSP_MERCHANT);
	if(!EmvIF_GetKeyboardInput((char *)csEMV_AMOUNT, (char *)abBuffer, 1, 12,
					FALSE, FALSE, FALSE, TRUE, FALSE, &bLen, 0, FALSE/*bm ec ,0*/)) 
	{
	  	EmvIF_ClearDsp(DSP_MERCHANT);
		return FALSE;
	}
	EmvIF_ClearDsp(DSP_MERCHANT);
/*
	memcpy(petdData->Amount, abBuffer, bLen);
	for(i = bLen; i < 15; i++) petdData->Amount[i] = ' ';
*/
  memset(abAmount, '0', 13 - bLen -1);							
	memcpy(abAmount + (13 - bLen -1), abBuffer, bLen);							
  abAmount[12]	= 0;				/* NULL termination */
		strcpy(amount_surcharge,(char *)abAmount);

		if(VERDE_TRANSACTION)
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
					Utils_Subtract_Strings(amount_surcharge, verde_discount, verde_amount);
					memcpy(amount_surcharge, verde_amount,13);
					amount_surcharge[13] = 0;
					Utils_LeftPad(amount_surcharge, '0', 13);
				}
			}
		}
	}
  	else 
  	{
  		memset(abAmount, 0 , sizeof(abAmount));
/*20.07.2004
		strcpy(abAmount,amount_surcharge);  */
		memcpy(abAmount,amount_surcharge+1,sizeof(abAmount));  /*20.07.2004*/
		abAmount[12] = 0;				/* NULL termination */	/*20.07.2004*/
  	}
  ulAmBin				= atoi((char *)abAmount);
	Util_AsciiToBcd((char *)abAmount, abAmountNum, 12);

  /* Set transaction type and amount in the internal structure */
	bLen = 0;					/* Processing Code for goods and services  */
	return ((EmvDb_StoreData(tagAUTH_AMOUNT_BIN, BerTlv_AsBufToRd((byte *)&ulAmBin, sizeof(ulAmBin)), srcTERMINAL) == EMV_OK) &&
		  (EmvDb_StoreData(tagAUTH_AMOUNT_NUM, BerTlv_AsBufToRd(abAmountNum, sizeof(abAmountNum)), srcTERMINAL) == EMV_OK) &&
		  (EmvDb_StoreData(tagTRAN_TYPE, BerTlv_AsBufToRd(&bLen, sizeof(bLen)), srcTERMINAL) == EMV_OK));
}


/****************************************************************************
 * FUNCTION NAME: EMVMMI_PIN
 * DESCRIPTION:   PIN entry for EMV
 * RETURN:        EMV_OK          PIN entered
 *                EMV_PIN_CANCEL  PIN entry is canceled
 *                EMV_FAIL        PIN Pad is malfunctioning
 *                EMV_TERMINATE_TRN Amount not confirmed
 * NOTES:         8 bytes buffer is required
 ****************************************************************************/
byte EMVMMI_PIN(EMVLanguages *pelsLangs, byte *abBuf, byte *pbPINOptions, byte *pbPINLength)
{
	char      buffer[24];
	char	*pcMessage;
	byte      abPINBlock[8];
	byte      bPINLen;
	byte      i, j, k;
	byte      abAmount[14];
	byte      *AmountNum = EmvDb_GetDataPointer(tagAUTH_AMOUNT_NUM);
	char	WrkBuf[50];
	byte Status;
	byte	display_length, additional;
	char display_amount[14];
	char	myprefix[7];
	RowData myrdData;

        OnlinePinEntry = FALSE;
	/* Bonus and AKK fix && For offline PIN */
	if (!(*pbPINOptions & pin_ONLINE))
	{
		byte    *PAN = EmvDb_GetDataPointer(tagPAN);
		byte    bLeft;
		byte    *pbErr;
		RowData rdPIN;

		EmvIF_BinToAscii(PAN, myprefix, 3, 6, FALSE);
		myprefix[6] = 0;
		if (PIN_BLOCK)
		if (strstr(Not_Certified_Prefixes, myprefix) != NULL)
		{
			memcpy(abPINBlock, "\x2C\x00\x00\x00\x00\x00\xFF", 8); /* Setting all zero */
			rdPIN = BerTlv_AsBufToRd(abPINBlock, sizeof(abPINBlock));
			while (1)
			{
				switch(EmvCmd_Verify(rdPIN, VERIFY_OFFLINE_PIN, &bLeft, pbErr))
				{
				case VERIFY_6983:
				case VERIFY_6984:
					return EMV_BLOCK;
				case VERIFY_63C0:
					if (bLeft == 0) return EMV_BLOCK;
					continue; /* sending the command again */
				default:
					continue; /* sending the command again */
				}
			}
		}
	}
/*	if (*pbPINOptions & pin_AMOUNT_CONFIRMATION) *//* Amount validation */
	{
		*pbPINOptions &= ~pin_AMOUNT_CONFIRMATION;
		memset(abAmount, 0, sizeof(abAmount));
		Util_BcdToAscii(AmountNum, (char *)abAmount, 6);
		for (i = 0; i < 14; i++) if (abAmount[i] == '0') abAmount[i] = ' '; else break;

		{
			byte Len;
			char UpperWord[50], TempBuffer[20], Buffer[24], lbTemp[40], FormattedTempBuffer[40];
			sint i, j, k;
			boolean doit;

			/* Internal PinPad */
			/* PIN Entry */ 
			if (EmvDb_CheckKernelOption(8, opt_8_ASHRAIT | opt_8_VSDC) && (*pbPINOptions & pin_LAST_TRY))
				pcMessage = pelsLangs->elLangs[pelsLangs->bCardholderIndex].acMessages[msg_EMV_LAST_PIN_TRY];  
			else pcMessage = pelsLangs->elLangs[pelsLangs->bCardholderIndex].acMessages[msg_EMV_ENTER_PIN];
			strcpy (UpperWord,(char *)pcMessage);
			doit = TRUE;
			j = 0;
			memset(TempBuffer, 0, 20);
			for (i = 0; i < 14; i++)
				if ((abAmount[i] == ' ') && doit) continue;
				else { doit = FALSE; TempBuffer[j++] = abAmount[i]; }

			Slips_FormatAmount(TempBuffer, FormattedTempBuffer);
/*			Mainloop_WithComma(TempBuffer, TempBuffer, strlen(TempBuffer));*/
			if (strlen (TempBuffer) > 11)
			{
				Mainloop_DisplayWait("MEBLA¦ BšYšK",2);
				return(EMV_TERMINATE_TRN);
			}

			strcat(UpperWord, "   ");
			strcat(UpperWord, FormattedTempBuffer);
		  	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
				strcat(UpperWord, " \xBE");
			else 
			{
				strcat(UpperWord, " YTL");
			}

		  	if (*pbPINOptions & pin_ONLINE)
  			{
				BYTE result;
				PinStruct stPinStruct;
				sint i, j;	

				memset(buffer, 0, 24);
				strcpy((char *)stPinStruct.DisplayMessage, UpperWord);
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
				else stPinStruct.MasterKeyIndex = master_key_no;
				memcpy(stPinStruct.WorkingKey, working_key, 16);
				stPinStruct.PinSize=6;
				stPinStruct.PinRetry=3;
				stPinStruct.Timeout=30;
				stPinStruct.PinMode=0;
				stPinStruct.SecurityMode=0;
				if(result==STAT_OK) result = Security_GetPINBlock (&stPinStruct);
				if(result == PIN_OK)	
				{
					char str[10];

					memcpy(str,stPinStruct.PinBlock,8);
					str[8]=0;
					Utils_BinToAscii(str, buffer, 8);
					bPINLen = 16;

					result=STAT_OK;
#if toygar
					Printer_WriteStr("\nPinBlock\n");
					PrntUtil_BufAsciiHex((byte *)buffer,16);
#endif
					memcpy(encrypted_PIN, buffer, bPINLen);
					OnlinePinEntry = TRUE;

				}
				else result=STAT_NOK;	
				if (result != STAT_OK) { Mainloop_DisplayWait("PARAMETRE HATASI",1); return(EMV_TERMINATE_TRN); }
	  		}
			else 
			{
/* Alp - BKM Testleri
				strcpy(lbTemp,"MEBLA¦ DO¦RU MU?");
				strcat(lbTemp, FormattedTempBuffer);
			  	if ( ( POS_Type == '3' ) || ( POS_Type == '4' ) )
					strcat(lbTemp, " \xBE");
				else 
				{
					strcat(lbTemp, " YTL");
				}
				
				if(!EmvIF_GetKeyboardInput(lbTemp, Buffer, 0, 0, FALSE, FALSE, TRUE, FALSE,
								FALSE, &Len, 0, FALSE, 0)) return (EMV_TERMINATE_TRN);
*/
				if(!EmvIF_GetKeyboardInput(UpperWord, Buffer, 4, 6, FALSE, FALSE, TRUE, FALSE,
								FALSE, &Len, 0, FALSE, 0)) return (EMV_PIN_CANCEL);
				memcpy(buffer,Buffer,Len);
				buffer[Len]=0;
				bPINLen = Len;
			}
			if (*pbPINOptions & pin_ONLINE) EmvDb_StoreData(tagTRAN_PIN_DATA, BerTlv_AsBufToRd((byte *)buffer, bPINLen), srcTERMINAL);
			else
			{
				/* Producing offline PIN data */
				memset(abPINBlock, 0, sizeof(abPINBlock));
				abPINBlock[0] = 0x20 | bPINLen;
				i = 1;
				j = 0;
				while(j != bPINLen)
				{
					abPINBlock[i]    = (buffer[j++] - '0') << 4;
					if (j == bPINLen) break;
					abPINBlock[i++] += buffer[j++] - '0';
				}
				if (j % 2) abPINBlock[i++]     += '\xF';
				for( ; i<8 ; i++) abPINBlock[i] = '\xFF';
			  	memcpy(abBuf, abPINBlock, 8);
			}
			return EMV_OK;
		}
	}
	
	/* PIN Entry */ 
	if (EmvDb_CheckKernelOption(8, opt_8_ASHRAIT | opt_8_VSDC) && (*pbPINOptions & pin_LAST_TRY))
		pcMessage = pelsLangs->elLangs[pelsLangs->bCardholderIndex].acMessages[msg_EMV_LAST_PIN_TRY];  
	else pcMessage = pelsLangs->elLangs[pelsLangs->bCardholderIndex].acMessages[msg_EMV_ENTER_PIN];  

  	if (*pbPINOptions & pin_ONLINE)
  	{
  		Files_ReadGenParams();
/*  		Printer_WriteStr("Pin ONLINE\n");*/
  		EmvDb_Find(&myrdData, tagPAN);
  		EmvIF_BinToAscii(myrdData.value, card_no, myrdData.length, sizeof(card_no), FALSE);
		while (card_no[strlen(card_no) - 1] == 'F') card_no[strlen(card_no) - 1] = 0;
  		card_no_length= strlen(card_no);
		i = card_no_length;
		memset(WrkBuf,0,50);
		memcpy(WrkBuf,  "70.", 3);
		memcpy(WrkBuf+3, card_no, card_no_length);
		WrkBuf[3+i++] = 0x1C;
		additional = 3+i;
		memcpy(WrkBuf+additional, working_key, 16);
		memcpy(display_amount, amount_surcharge, 13);
		display_amount[13] = 0;
		Utils_ClearInsZeros(display_amount);	
		display_length = strlen(display_amount);
		if (display_length > 11)
		{
			Mainloop_DisplayWait("MEBLA¦ BšYšK",2);
			return(EMV_TERMINATE_TRN);
		}
		additional = 19+i;
		memcpy(WrkBuf+additional, display_amount, display_length);

/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,TRUE,FALSE);
		else Status = Host_SendReceive (WrkBuf,FALSE,TRUE,TRUE);
/* Toygar 13.05.2002 End */

		memset (buffer,0,16);
		if(Status == EOT)
		{
			memset(buffer,'0',16);
			return(EMV_OK);
		}
		if (Status!=STAT_OK) return(EMV_PIN_CANCEL);
		if(memcmp(WrkBuf+1,"71",2)) return (EMV_PIN_CANCEL);
		else 
		{
			memcpy(buffer,WrkBuf+9,16);
			bPINLen = 16;
			memcpy(encrypted_PIN, buffer, bPINLen);
			OnlinePinEntry = TRUE;
			WrkBuf[0] = ACK;
			WrkBuf[1] = 0;
			if (Uart_SendBuf (PINPAD_PORT, (byte *)WrkBuf, strlen(WrkBuf) > 0)) return(EMV_OK);
		}
		return (EMV_PIN_CANCEL);
	}
	else
	{
		memset(WrkBuf,0,50);
		memcpy(WrkBuf,  "Z2", 2);
		WrkBuf[2] = 0x1A;
		memcpy(WrkBuf+3, pcMessage, strlen(pcMessage));

/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,FALSE,FALSE);
		else Status = Host_SendReceive (WrkBuf,FALSE,TRUE,FALSE);
/* Toygar 13.05.2002 End */

		if (Status != ACK) return (EMV_PIN_CANCEL);
		memset(WrkBuf,0,50);
		memcpy(WrkBuf,  "Z50", 3);
		WrkBuf[3] = '0';
		memcpy(WrkBuf+4,  "100", 3);
		memcpy(WrkBuf+7,  "12", 2);

/* Toygar 13.05.2002 Start */
		if (BUYUKFIRMA) Status = Host_BFirmaSendReceive (WrkBuf,FALSE,TRUE,TRUE,FALSE);
		else Status = Host_SendReceive (WrkBuf,FALSE,TRUE,TRUE);
/* Toygar 13.05.2002 End */

		if(Status == EOT) return (EMV_PIN_CANCEL);
		if (Status!=STAT_OK) return(EMV_PIN_CANCEL);
		if(memcmp(WrkBuf+1,"Z51",3)) return (EMV_PIN_CANCEL);
		else 
		{
			for(i=0;i<12;i++) if (WrkBuf[4+i] == 3) break;
			memcpy(buffer,WrkBuf+4,i);
			buffer[i]=0;
			bPINLen = i;
			WrkBuf[0] = ACK;
			WrkBuf[1] = 0;
			if (Uart_SendBuf (PINPAD_PORT, (byte *)WrkBuf, strlen(WrkBuf) <= 0)) return (EMV_PIN_CANCEL);
		}
	}
	/* Saving encrypted online PIN data to send in authorization messages */
	if (*pbPINOptions & pin_ONLINE) EmvDb_StoreData(tagTRAN_PIN_DATA, BerTlv_AsBufToRd((byte *)buffer, bPINLen), srcTERMINAL);
	else
	{
		/* Producing offline PIN data */
		memset(abPINBlock, 0, sizeof(abPINBlock));
		abPINBlock[0] = 0x20 | bPINLen;
		i = 1;
		j = 0;
		while(j != bPINLen)
			{
			abPINBlock[i]    = (buffer[j++] - '0') << 4;
			if (j == bPINLen) break;
			abPINBlock[i++] += buffer[j++] - '0';
			}
		if (j % 2) abPINBlock[i++]     += '\xF';
		for( ; i<8 ; i++) abPINBlock[i] = '\xFF';
  		memcpy(abBuf, abPINBlock, 8);
	}
	return EMV_OK;
}
/* Toygar - NextEMV End - Ok */
