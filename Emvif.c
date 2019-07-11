/****************************************************************************
 * FILE NAME:   EMVIF.C                                                   *
 * MODULE NAME: EMVIF                                                     *
 * PROGRAMMER:  Fehmi Ozgun                                               *
 * DESCRIPTION: External functions that are used by EMV Kernel            *
 * REVISION:    
 ****************************************************************************/

          /*==========================================*
           *         I N T R O D U C T I O N          *
           *==========================================*/

          /*==========================================*
           *             I N C L U D E S              *
           *==========================================*/

#include "project.def"
#include <string.h>
#include  LIPTYPES_DEF
#include  DEFINE_H
#include  FILES_H
#include  MESSAGE_H
#include  EMVDEF_H
#include  EMVDEFIN_H
#include  EMVDB_H
#include  EMVACT_H
#include  MAINLOOP_H
#include  NURIT_DEF
#include  RTC_H
#include  EMVIF_H

          /*==========================================*
           *           D E F I N I T I O N S          *
           *==========================================*/
/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

 ULINT mt[N]; /* the array for the state vector  */
 int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

          /*==========================================*
           *       P R I V A T E  T Y P E S           *
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


/* EMV Code Insertion Starts - MTK - 10.04.2002 */


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: EmvIF_IToA
 *
 * DESCRIPTION:   Converts an integer to string
 *
 * RETURN:        None
 *
 * NOTES:    
 *
 * ------------------------------------------------------------------------ */
void EmvIF_IToA(int dv, char *out)
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
 * FUNCTION NAME: EmvIF_sgenrand
 *
 * DESCRIPTION: initializing the array with a NONZERO seed
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
void EmvIF_sgenrand(ULINT seed)
{
	mt[0]= seed & 0xffffffff;
	for (mti=1; mti<N; mti++) mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: EmvIF_genrand
 *
 * DESCRIPTION: 
 *
 * RETURN:      
 *
 * NOTES:       
 *
 * --------------------------------------------------------------------------*/
ULINT EmvIF_genrand(void)
{
	ULINT y;
	 ULINT mag01[2]={0x0, MATRIX_A};
	/* mag01[x] = x * MATRIX_A  for x=0,1 */
	
	if (mti >= N) 
	{ /* generate N words at one time */
		int kk;
		
		if (mti == N+1) EmvIF_sgenrand(4357); /* a default initial seed is used   */
		for (kk=0;kk<N-M;kk++)
		{
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (;kk<N-1;kk++) 
		{
			y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
		mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];
		mti = 0;
	}
	y = mt[mti++];
	y ^= TEMPERING_SHIFT_U(y);
	y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
	y ^= TEMPERING_SHIFT_L(y);
	return y; 
}

/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: EmvIF_InitRNG
 *
 * DESCRIPTION:   
 *
 * RETURN:
 *
 * NOTES:
 * ------------------------------------------------------------------------ */
void EmvIF_InitRNG(void)
{
	real_time_clock Today;
	usint seed;
	
	Rtc_Read(&Today);
	seed = Today.day*10000+Today.month*100+Today.year;
	seed += Today.hours*10000+Today.minutes*100+Today.seconds;
	EmvIF_sgenrand(seed); /* any nonzero integer can be used as a seed */
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: EmvIF_SearchExceptionListIn
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte EmvIF_SearchExceptionListIn(byte *Card)
{
	return(Main_SearchExceptionList(Card));
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: EmvIF_SearchRevocationListIn
 *
 * DESCRIPTION:
 *
 * RETURN:
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
byte EmvIF_SearchRevocationListIn(EMVRevocEntry *toSearch)
{
	return(Main_SearchRevocationList(toSearch));
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: EmvIF_CheckTRFullCall
 *
 * DESCRIPTION: Checks if the total allowed number of transactions is completed.
 *
 * RETURN: TRUE or FALSE
 *
 * NOTES:
 *
 * --------------------------------------------------------------------------*/
boolean EmvIF_CheckTRFullCall(sint *lwLastRec)
{
	sint TransOnlineNo, TransOfflineNo;
	
	Files_GetTransactionCounts((int *)&TransOnlineNo, (int *)&TransOfflineNo);
	*lwLastRec = TransOnlineNo + TransOfflineNo;
	return(FALSE);
}



/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: EmvIF_TLookUp
 *
 * DESCRIPTION: Reads from the transaction database.
 *
 * RETURN: lwStatus ( STAT_OK, STAT_NOK or STAT_EOF )     
 *
 * NOTES:       
 *
 * ------------------------------------------------------------------------ */
byte EmvIF_TLookUp(PTStruct *fpPTStruct)
{
	return(Files_LastTransRead(fpPTStruct));
}


/* EMV Code Insertion Ends   - MTK - 10.04.2002 */


/* Toygar - NextEMV Start - Ok */

/****************************************************************************
 * FUNCTION NAME: EmvIF_ClearDsp
 * DESCRIPTION:   Instructs the terminal application to clear the user display
 * INPUT:         The display type to clear its content see DISPLAY_ENUMS enum 
 *                in *.h file
 * RETURN:        none
 * NOTES:         
 ****************************************************************************/
void EmvIF_ClearDsp(byte bDisplay)
{
#ifndef GRAFDISP
	Display_ClrDsp();
#else
	GrphText_Cls (TRUE);
#endif
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_DisplayWait
 * DESCRIPTION:   Instructs the terminal application to display a message in the
 *                specified time interval
 * INPUT:         The display type to display is specified see DISPLAY_ENUMS enum 
 *                in *.h file
 * RETURN:        
 * NOTES:         
 ****************************************************************************/
boolean EmvIF_DisplayWait(EMVLanguages *pelsLangs, byte bDisplay, byte bMessageID, int iTimeInt)
{
  boolean  blPINPad;
  char     pcMessage[10];

  /* Checking PIN Pad existance - extra checks may be applied by the application*/
  blPINPad = EmvDb_CheckKernelOption(3, opt_3_EXTERNAL_PINPAD);
    
  if (!blPINPad)
  {
    if (bDisplay == DSP_CARDHOLDER)
      return Mainloop_DisplayWait(pelsLangs->elLangs[pelsLangs->bCardholderIndex].acMessages[bMessageID], iTimeInt);
    else
      return Mainloop_DisplayWait(pelsLangs->elLangs[0].acMessages[bMessageID], iTimeInt);
  }
  else
{
    if ((bDisplay == DSP_MERCHANT) || (bDisplay == DSP_BOTH))
      return Mainloop_DisplayWait(pelsLangs->elLangs[0].acMessages[bMessageID], iTimeInt);
    /* Had to use PIN Pad interface */
    if ((bDisplay == DSP_CARDHOLDER) || (bDisplay == DSP_BOTH))
      return Mainloop_DisplayWait(pelsLangs->elLangs[0].acMessages[bMessageID], iTimeInt);            
}
}


/* Toygar - NextEMV End - Ok */

/****************************************************************************
 * FUNCTION NAME: EmvIF_GetTrnSequenceCounter
 * DESCRIPTION:   Retrieves next aavailable sequence counter for the  transaction
 * INPUT:         Buffer to trn sequence counter
 * NOTES:         
 ****************************************************************************/
void EmvIF_GetTrnSequenceCounter(RowData *prdTSC)
{
	sint *psTSC;
	/* Call terminal applications trn sequence counter */
	psTSC = (sint *)prdTSC->value;
	EmvIF_CheckTRFullCall(psTSC);
	/* Set length of sequence counter - either 2 or 4 */
	prdTSC->length  = 2;
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_BinToAscii
 * DESCRIPTION:   Converts a binary block to ascii equivalent
 * INPUT:         
 * NOTES:         
 ****************************************************************************/
void EmvIF_BinToAscii(char *fpStrBin, char *fpStrAscii, int fbLen, int fbLenAscii, boolean Space)
{
	char lbTempBin[256],lbTempAscii[512];
	byte nibble, tempbyte;
	int counter = 0, i;

	memset(lbTempBin,0,256);
	memcpy(lbTempBin,fpStrBin,fbLen);
	if (Space) memset(lbTempAscii,' ',512); else memset(lbTempAscii,0,512);
	for(i=0;i<fbLen;i++)
	{
		tempbyte = lbTempBin[i];
		nibble  = ((tempbyte & 0xF0) >> 4) & 0x0F;
		if (nibble > 0x09) lbTempAscii[counter] = 'A' - 1 + (nibble-9);
		else EmvIF_IToA((int)nibble,lbTempAscii+counter);
		counter++;
		nibble  = tempbyte & 0x0F;
		if (nibble > 0x09) lbTempAscii[counter] = 'A' - 1 + (nibble-9);
		else EmvIF_IToA((int)nibble,lbTempAscii+counter);
		counter++;
	}
	if (Space) 
	{
		if (fbLenAscii != fbLen*2) lbTempAscii[fbLen*2] = ' ';
		memcpy(fpStrAscii,lbTempAscii,fbLenAscii);
	}
	else strcpy(fpStrAscii,lbTempAscii);
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_GetKeyboardInput
 * DESCRIPTION:   Wrapper for GetKeyboardInput function
 * INPUT:         
 * NOTES:         
 ****************************************************************************/
boolean EmvIF_GetKeyboardInput ( const char *UpperLine, char *Buffer, 
							    sint DownLimit, sint UpLimit,
									boolean First, boolean Restricted,
									boolean Masked, boolean Comma,
									boolean Alpha, byte *Len, char *ExDefault, 
									boolean EmptyOk, char *Amount)
{
  return Mainloop_GetKeyboardInput (UpperLine, Buffer, DownLimit, UpLimit,
									First, Restricted, Masked, Comma, Alpha, Len, ExDefault, 
									EmptyOk/*, Amount*/);
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_IncreaseBCD
 * DESCRIPTION:   Wrapper for IncreaseBCD function
 * INPUT:         
 * NOTES:         
 ****************************************************************************/
void EmvIF_IncreaseBCD(char *in,sint len)
{
	sint t;
	byte tmp1,tmp2,carry;

	for (t=len-1;t>=0;t--)
	{
		tmp1=in[t]&0x0F;
		tmp2=in[t]>>4;
		tmp1++;
		if (tmp1==10) 
		{
			tmp1=0;
			carry=1;
		}
		else carry=0;
		tmp2+=carry;
		if ((tmp2&0x0F) == 0x0A )
		{
			tmp2=0;
			carry=1;
		}
		else carry=0;
		tmp1|=(tmp2<<4);
		in[t]=tmp1;
		if (carry==0) break;
	}
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_AppendData
 * DESCRIPTION:   Appends the given data and updates the pointer
 * RETURN:        Appended data length
 * NOTES:         
 ****************************************************************************
usint EmvIF_AppendData(byte **ppIndex, usint usTag)
{
  RowData   rdData;
  if (!EmvDb_Find(&rdData, usTag))
  {
	Printer_WriteStr ("ReturnedZero!..\n");
	return 0;
  }
  memcpy(*ppIndex, rdData.value, rdData.length);

  PrntUtil_BufAsciiHex((byte *)rdData.value,rdData.length);

  *ppIndex += rdData.length;
  return rdData.length;
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_AppendData
 * DESCRIPTION:   Appends the given data and updates the pointer
 * RETURN:        Appended data length
 * NOTES:         
 ****************************************************************************/
usint EmvIF_AppendData(byte **ppIndex, usint usTag, boolean Ascii, int fulllength)
{
	RowData   rdData;

	if (!EmvDb_Find(&rdData, usTag)) 
	{
		if (Ascii) { memset(*ppIndex,' ',fulllength); *ppIndex += fulllength; }
/*		Printer_WriteStr ("ReturnedZero!..\n");*/
		return 0;
	}
	if (Ascii)
	{
		EmvIF_BinToAscii((char *)rdData.value, (char *)*ppIndex, rdData.length, fulllength, TRUE);
/*
		Printer_WriteStr("Ascii\n");
		PrntUtil_BufAsciiHex((byte *)*ppIndex,fulllength);
*/
		*ppIndex += fulllength;
	}
	else
	{
		memcpy(*ppIndex, rdData.value, rdData.length);
/*
		Printer_WriteStr("Non-Ascii\n");
		PrntUtil_BufAsciiHex((byte *)*ppIndex,rdData.length);
*/
		*ppIndex += rdData.length;
	}
	return rdData.length;
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_GetRandNumber
 * DESCRIPTION:   Retrieves 16 byte random number 
 * RETURN:        
 * NOTES:         
 ****************************************************************************/
void EmvIF_GetRandNumber(RowData *prdRand)
{
  	sint len;
  	char *randptr = (char *)prdRand->value;

	for (len = 0; len < 16; len++) randptr[len] = EmvIF_genrand();
	for (len = 0; len < 16; len++)
		if (randptr[len] == 0x00 || randptr[len] == 0x10 
			|| randptr[len] == 0x03 || randptr[len] == 0x02)
			if (len >1) randptr[len] ^= (randptr[len - 1] ^ randptr[len - 2]);
			else randptr[len]+=0x05;
	prdRand->length = 16;
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_SearchRevocationList
 * DESCRIPTION:   Returns status of an issuer certificate
 * RETURN:        EMV_OK    Not in the revocation list
 *                EMV_FAIL  In the revocation list
 * NOTES:         
 ****************************************************************************/
byte EmvIF_SearchRevocationList(EMVRevocEntry *toSearch)
{
	EMVRevocEntry rItem;
	
	/* Assign EMV revocation entry to the app's format */
	memcpy(rItem.RID, toSearch->RID, sizeof(toSearch->RID));
	rItem.CAPKIndex = toSearch->CAPKIndex;
	memcpy(rItem.Serial, toSearch->Serial, sizeof(toSearch->Serial));

	/* Search using application sepecific structure and map the output to EMV specific returns */
	return (EmvIF_SearchRevocationListIn(&rItem) == STAT_OK) ? EMV_OK : EMV_FAIL;
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_SearchExceptionList
 * DESCRIPTION:   Returns status of an issuer certificate
 * RETURN:        EMV_OK    PAN entry appears in black list
 *                EMV_FAIL  PAN entry is not in black list
 * NOTES:         
 ****************************************************************************/
byte EmvIF_SearchExceptionList(RowData rdPAN, RowData rdPANSeq)
{
	char CardPAN[25];
	
	memset(CardPAN,0,25);
	memcpy(CardPAN,rdPAN.value,rdPAN.length);
	/* No sequence number processing for this app */
	return (EmvIF_SearchExceptionListIn((byte *)CardPAN) == STAT_OK) ? EMV_OK : EMV_FAIL;
}

#if 0
/****************************************************************************
 * FUNCTION NAME: EmvIF_InsertTransData
 * DESCRIPTION:   Inserts transaction data into the struct
 * RETURN:        
 * NOTES:         
 ****************************************************************************/
/* Toygar - NextEMV Start - Ok */
/*
byte EmvIF_InsertTransData(EMVTransData *petdData, EmvApplication *pEmvApp)
*/
byte EmvIF_InsertTransData(EMVTransData *petdData)
/* Toygar - NextEMV End - Ok */
{
	RowData rdData;
	byte *pbIndex;
	byte abPAN[30];
	byte /*bPANLen,*/ lbCount;

	memset(abPAN, 0, sizeof(abPAN));
	EmvDb_Find(&rdData, tagPAN);
	EmvIF_BinToAscii(rdData.value, abPAN, rdData.length, 30, FALSE);

	/* Stripe trailing padding F's */
	while (abPAN[strlen(abPAN) - 1] == 'F') abPAN[strlen(abPAN) - 1] = 0;
	memcpy(petdData->Pan, abPAN, strlen(abPAN));
	petdData->PanLength = strlen(abPAN);
/*	
	DEBUG(8,
	{
		Printer_WriteStr("Striped PAN\n");
		PrntUtil_BufAsciiHex(petdData->Pan, petdData->PanLength);
	})
*/
	/* Field55 generation */
	pbIndex = petdData->Field55;
	Printer_WriteStr("tagPOS_ENTRY_MODE\n");
	EmvIF_AppendData(&pbIndex, tagPOS_ENTRY_MODE, TRUE, 4);
	Printer_WriteStr("tagAIP\n");
	EmvIF_AppendData(&pbIndex, tagAIP, TRUE, 4);
	Printer_WriteStr("tagDF_NAME\n");
	EmvIF_AppendData(&pbIndex, tagDF_NAME, TRUE, 32);
	Printer_WriteStr("tagTVR\n");
	EmvIF_AppendData(&pbIndex, tagTVR, TRUE, 10);
	Printer_WriteStr("tagISSUER_APPL_DATA\n");
	EmvIF_AppendData(&pbIndex, tagISSUER_APPL_DATA, TRUE, 64);
	Printer_WriteStr("tagIFD_SERIAL_NUM\n");
	EmvIF_AppendData(&pbIndex, tagIFD_SERIAL_NUM, TRUE, 16);
	Printer_WriteStr("tagAPPL_CRYPTOGRAM\n");
	EmvIF_AppendData(&pbIndex, tagAPPL_CRYPTOGRAM, TRUE, 16);
	Printer_WriteStr("tagCRYPTOGRAM_INFO\n");
	EmvIF_AppendData(&pbIndex, tagCRYPTOGRAM_INFO, TRUE, 2);
	Printer_WriteStr("tagTERML_CAPABILITIES\n");
 	EmvIF_AppendData(&pbIndex, tagTERML_CAPABILITIES, TRUE, 6);
	Printer_WriteStr("tagCVM_RESULT\n");
	EmvIF_AppendData(&pbIndex, tagCVM_RESULT, TRUE, 6);
	Printer_WriteStr("tagTERML_TYPE\n");
	EmvIF_AppendData(&pbIndex, tagTERML_TYPE, TRUE, 2);
	Printer_WriteStr("tagATC\n");
	EmvIF_AppendData(&pbIndex, tagATC, TRUE, 4);
	Printer_WriteStr("tagUNPREDICTABLE_NUMBER\n");
	EmvIF_AppendData(&pbIndex, tagUNPREDICTABLE_NUMBER, TRUE, 8);
	Printer_WriteStr("tagTRAN_SEQUENCE_COUNTER\n");
	EmvIF_AppendData(&pbIndex, tagTRAN_SEQUENCE_COUNTER, TRUE, 8);

	/* Missing: Transaction Category Code */

	Printer_WriteStr("tagAUTH_RESPONSE_CODE\n");
	EmvIF_AppendData(&pbIndex, tagAUTH_RESPONSE_CODE, TRUE, 2);
	Printer_WriteStr("tagAUTH_RESPONSE_CODE\n");
	EmvIF_AppendData(&pbIndex, tagAUTH_RESPONSE_CODE, TRUE, 2);

	/* Issuer Script Results */
/*	memcpy(pbIndex, petdData->abIssuerScrResults, petdData->bIssScrResults_Len);
	pbIndex += petdData->bIssScrResults_Len;
*/	petdData->Field55Len = pbIndex - petdData->Field55;
/*
	DEBUG(8,
	{
		Printer_WriteStr("Script field\n");
		PrntUtil_BufAsciiHex(petdData->abIssuerScrResults, petdData->bIssScrResults_Len);
		Printer_WriteStr("Field55\n");
		PrntUtil_BufAsciiHex(petdData->Field55, petdData->Field55Len);
	})
*/

	/* Adding AID */
	EmvDb_Find(&rdData, tagAID_TERMINAL);
	EmvIF_BinToAscii(rdData.value,petdData->AID,rdData.length, 16,FALSE);  
	petdData->AIDLen = rdData.length * 2;
}
#endif


/****************************************************************************
 * FUNCTION NAME: EmvIF_GetRecentLog
 * DESCRIPTION:   Returns most recent log entry for a given PAN
 * RETURN:        
 * NOTES:         
 ****************************************************************************/
ulint EmvIF_GetRecentLog(RowData *prdPAN)
{
	PTStruct ptStruct;

	memset(&ptStruct,0,sizeof(ptStruct));

	EmvUtil_BinToAscii((char *)prdPAN->value, ptStruct.Pan, prdPAN->length );
	ptStruct.PanLength = prdPAN->length * 2;
	
	while(ptStruct.Pan[ptStruct.PanLength - 1]=='F')
		  ptStruct.PanLength--;

	return (EmvIF_TLookUp(&ptStruct) == STAT_OK) ? atoi(ptStruct.Amount) : 0;
}

/* Toygar - NextEMV Start - Ok */
/****************************************************************************
 * FUNCTION NAME: EmvIF_GetReferralResults
 * DESCRIPTION:   Provides attendant manipulation flexibility in case of a referral
 * RETURN:        Referral approval status
 * NOTES:         Available only if the REFER_REVERSAL option is off in kernel options
 ****************************************************************************/
boolean EmvIF_GetReferralResults(EMVLanguages *pelsLangs)
{
  byte abPAN[LG_PAN];
  RowData   rdData;

  memset(abPAN, 0, sizeof(abPAN));
  EmvDb_Find(&rdData, tagPAN);
  EmvUtil_BinToAscii((char *)rdData.value, (char *)abPAN, rdData.length);

  /* Stripe trailing padding F's */
  while (abPAN[strlen((const char *)abPAN) - 1] == 'F') abPAN[strlen((const char *)abPAN) - 1] = 0;
  /*The application may print or display transaction details for referral */
  Printer_WriteStr("REFERRAL DATA\n");
  Printer_WriteStr("PAN:\n");
  Printer_WriteStr(abPAN);
  Printer_WriteStr("\n\n\n\n\n");
 
  /*Getting the referral result from attendant */
  EmvIF_ClearDsp(DSP_MERCHANT);
  EmvIF_DisplayWait(pelsLangs, DSP_MERCHANT, msg_EMV_CALL_YOUR_BANK, 2);
  return EMVMMI_GetOption(pelsLangs, DSP_MERCHANT, msg_EMV_ASK_APPROVAL);
}

/****************************************************************************
 * FUNCTION NAME: EmvIF_GetCAPublicKey
 * DESCRIPTION:   Retrieves CA PK identified by params
 * RETURN:        EMV_OK            PK found
 *                EMV_FAIL          Operation failed
 * NOTES:         
****************************************************************************/
byte EmvIF_GetCAPublicKey(CAPublicKey *pcaKey, RowData *prdRID, byte bCAPKIndex)
{
	if (Files_SearchCAPublicKey(pcaKey, prdRID, bCAPKIndex) == TRUE) return (EMV_OK);
	else return (EMV_FAIL);
}

/* Toygar - NextEMV End - Ok */
