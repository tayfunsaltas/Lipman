/*-----------------------------------------------------------------------------*
* Copyright Welcome Real-time
* File Name    	 : INTERFAC.C
* Created      	 : 11/07/2001
* Author       	 : O.RICHARD
* Library      	 : Interface
* Portability  	 : LIPMAN
*
* Purpose          : Interface functions between loyalty and payment application
*
* Revision history :
*	Date 		Author		Description
*  xx/xx/2001                       
*-----------------------------------------------------------------------------*/

/*............. HEADER FILES ..............................................*/
#include <inc_term.h>
#include <confapp.h>
#include <inc_xls.h>

#include <xlstypes.h>
#include <xlsconst.h>
#include <io_xls.h>

#include <interfac.h>

#include <etatterm.h>
#include <xlslib.h>
#include <data.h>
#include <lib_aff.h>

#include <menu.h>
#include <tpemenu.h>
#include <collecte.h>

#include <datejour.h>
#include <asc_tool.h>
#include <asctobcd.h>

#include <loycred.h>
#include <introcrd.h>
#include <imp_cred.h>
#include <events.h>

#include <trace.h>

/* on-line loyalty reconciliation  */
#include <loypay.h>


#include <xlecuser.h>
#include <xleccrd.h>

#ifdef TL_TO_YTL_ACTIVATED
/*extern int16 GFlag_YTL;*/
extern void XLS_ConvParamTLToYTL(void);
#endif
extern void xls_reset_etat_xls(void);

/*******************************************************************************
* Function : TPE_StringAsciiConvert
*
* Description :
*
* Input : inString :
*
* Output : None	
*
* Return : None
*
*******************************************************************************/
void TPE_StringAsciiConvert(byte *ioString);
void TPE_StringAsciiConvert(byte *ioString)
{
  int16 i;
	i = 0;

	while(ioString[i] != 0x00)
	{
		if(ioString[i] > 127)
    {
      switch (ioString[i])
      {
        case 199: ioString[i] = 0x80;
          break;
        case 208: ioString[i] = 0xA6;
          break;
        case 214: ioString[i] = 0x99;
          break;
        case 220: ioString[i] = 0x9A;
          break;
        case 221: ioString[i] = 0x98;
          break;
        case 222: ioString[i] = 0x9E;
          break;
        case 231: ioString[i] = 0x87;
          break;
        case 240: ioString[i] = 0xA7;
          break;
        case 246: ioString[i] = 0x94;
          break;
        case 252: ioString[i] = 0x81;
          break;
        case 253: ioString[i] = 0x8D;
          break;
        case 254: ioString[i] = 0x9F;
          break;
      }  
    }
		i++;
  };
}
/******************** End of Function TPE_StringAsciiConvert **********************/

#ifdef TL_TO_YTL_ACTIVATED
/*******************************************************************************
* Function : XLS_GetCurcyMode
*
* Description : Initialize global variables before entering the idle loop
*
* Input         :  none	
*
* Output        : CurcyMode TL_MODE or YTL_MODE
*
* Return : TRUE or FALSE
*
*******************************************************************************/
int16 XLS_GetCurcyMode(byte *outCurcyMode)
{
	char theYTLFlag[Size_YTL_Flag];
	
	XLS_GetYTLFlag(theYTLFlag);
	if (xtl_memcmp(theYTLFlag,"YTL",Size_YTL_Flag)==0) /* YTL activated */
		*outCurcyMode = YTL_MODE;
	else
	{
		if (xtl_memcmp(theYTLFlag,"TLE",Size_YTL_Flag)==0) /* YTL activated */
			*outCurcyMode = TL_MODE;
		else
			return FALSE;
	}
	return TRUE;
}
#endif

/**************************************************************
*
*  Function name : XLS_Start
*
*  Author        : O.RICHARD
*
*  Purpose       : Initialize global variables before entering the idle loop
*
*  Input         :
*
*  Output        : 
*
*  Return        : 
*
***************************************************************/

void	XLS_Start(void)
{
	byte theEtat;
	uint16 theDate,theTime;
	uint16 theDay,theMonth,theYear;
	char str[Size_TimerAck];
	char	buffer1[LG_MSG_ECRAN];
	char	buffer2[LG_MSG_ECRAN];
#ifdef TL_TO_YTL_ACTIVATED
	byte theParameterInUpdate;	
	int16 theErreur;
#endif

	if (!XLS_ResetApplication())
	{
		TPE_GetMessage(sAff_ResetAppli1,buffer1);
		TPE_GetMessage(sAff_Error,buffer2);
		TPE_CopieAffiche(buffer1,buffer2);
	}
	else
	{
		ReadETAT_ETAT(&theEtat);
#ifdef TL_TO_YTL_ACTIVATED
		/* Check if one Tl to YTL conversion has failed */
		ReadETAT_PARAM_IN_UPDATE(&theParameterInUpdate) ;

		if (theParameterInUpdate == TRUE )
		{
			if (theEtat == ETAT_TELECOLLECTE_NECESSAIRE || theEtat == ETAT_OPERATIONNEL)
			{
				theEtat = ETAT_TELECOLLECTE_OBLIGATOIRE;
				WriteETAT_ETAT(&theEtat);				
			}
			theParameterInUpdate = FALSE;
			WriteETAT_PARAM_IN_UPDATE(&theParameterInUpdate) ;			
			XLS_ResetFile(F_PARAM,&theErreur);
		}			
#endif
		switch (theEtat)
		{
			case ETAT_INUTILISABLE :
			case ETAT_TELECOLLECTE_IMPOSSIBLE :
			case ETAT_NON_INITIALISE :				Glob_Init_State=XLS_NOT_INITIALIZED;
													break;
			case ETAT_TELECOLLECTE_OBLIGATOIRE :	Glob_Init_State=XLS_COLLECTION_MANDATORY;
													break;
  			case ETAT_TELECOLLECTE_NECESSAIRE :
			case ETAT_OPERATIONNEL :				Glob_Init_State=XLS_OPERATIVE;
													break;
		}
		ReadETAT_DATE_NEXT_CALL(&theDate);
		ReadETAT_TIME_NEXT_CALL(&theTime);
		if ( (theDate == 0 && theTime == 0) || (Glob_Init_State != XLS_OPERATIVE) )
			xtl_memset(Glob_TimerAck,'0',Size_TimerAck);
		else
		{
			TEMPS_NombreDeJourToDate(theDate,&theDay,&theMonth,&theYear);
			sprintf(str,"%04u%02u%02u",theYear,theMonth,theDay);
			sprintf(str+8,"%02u%02u",theTime/60,theTime%60);
			str[Size_TimerAck-1]='\0';
			xtl_strncpy(Glob_TimerAck,str,Size_TimerAck);
		}
   }
   TRACE_Info((L1,"theEtat"),(L2,"0x%.2x",Glob_Init_State));
   TRACE_Info((L1,"TimerAck"),(L2,"%s",Glob_TimerAck));
}

/*-------------------------- End of Function XLS_Start -------------------------*/


/**************************************************************
*
*  Function name : XLS_Menu
*
*  Author        : O.RICHARD
*
*  Purpose       : Loyalty menu
*
*  Input         : 
*
*  Output        : 
*
*  Return        : 
*
***************************************************************/

void XLS_Menu(void)
{
	char choice;

	choice=TPE_MENU_Choix((struct TXLS_MENU *)Menu_XLS,0)-1;

	Menu((int)choice);
}

/*-------------------------- End of Function XLS_Menu -------------------------*/



/**************************************************************
*
*  Function name : XLS_EOD
*
*  Author        : O.RICHARD
*
*  Purpose       : End Of Day entry
*
*  Input         : inTrigger : Type of EOD call (see interface.h)
*
*  Output        : 
*
*  Return        : 
*
***************************************************************/

void	XLS_EOD(byte inTrigger)
{
	char		theModeCollecte;
	int16		theErreur;
	byte		theMaxRetry;
	txls_etat_term	theEtatTerm;
/* transition phase */
#ifdef TL_TO_YTL_ACTIVATED
	txls_etat_collecte  *etat_collecte;
	header_table theHeader;
#endif
	/*if (GFlag_YTL == 1)
	{
		memcpy(Glob_YTL_flag,"YTL",3);
	}*/

	if (inTrigger==XLS_CALL_FULL)
	{
		XLS_Collecte(CALL_FULL);
	}
	else
	{
		XLS_ReadFile(F_ETAT_TERM,0,LG_ETAT_TERM,&theEtatTerm,&theErreur);

		TRACE_Info((L1,"Nb essai collecte"),(L2,"%d",theEtatTerm.NbEssaiCollecte));

		/* Test du nombre de tentative */
		if (theEtatTerm.NbEssaiCollecte)
		{
			TRACE_Info((L1,"collecte auto"),(L2,"1"));
			theModeCollecte = CALL_AUTOMATIC;
			theEtatTerm.NbEssaiCollecte--;
		}
		else
		{
			if (theEtatTerm.NbEssaiLectureAck)
			{
				TRACE_Info((L1,"collecte ack read"),(L2,"1"));
				theModeCollecte = CALL_ACK_READ;
				theEtatTerm.NbEssaiLectureAck--;
			}
			else
			{
				TRACE_Info((L1,"collecte auto"),(L2,"2"));
				ReadP1_MAX_RETRY(&theMaxRetry);
				TRACE_Info((L1,"theMaxRetry"),(L2,"%d",theMaxRetry));
				theEtatTerm.NbEssaiCollecte = theMaxRetry;
				theModeCollecte = CALL_AUTOMATIC;
			}
		}
		TRACE_Info((L1,"Nb essai collecte"),(L2,"%d",theEtatTerm.NbEssaiCollecte));
		XLS_WriteFile(F_ETAT_TERM,0,LG_ETAT_TERM,&theEtatTerm,&theErreur);
		XLS_Collecte(theModeCollecte);
	}

/* transition phase */
#ifdef TL_TO_YTL_ACTIVATED
    etat_collecte = DATA_GetContextCollecte();
	
	if (etat_collecte->CurcyMode == YTL_MODE) /* YTL activated */
	{
		ReadP1_HEADER_P1(&theHeader);
		if (theHeader.version_mapping == XLS_VERSION_TABLE_P1) /* conversion to be done */			
			XLS_ConvParamTLToYTL();					
	}

#endif
}

/*-------------------------- End of Function XLS_EOD -------------------------*/


/**************************************************************
*
*  Function name : XLS_PrintTran
*
*  Author       : O.RICHARD
*
*  Purpose       : Slip printing entry
*
*  Input         : inSlipTyp : Slip type			(see interface.h)
*				   inTranTyp : Transaction type		(see interface.h)
*
*  Output        : 
*
*  Return        : 
*
***************************************************************/
 
void	XLS_PrintTran(byte inSlipTyp, byte inTranTyp)
{
	char buf[17];
	/*sprintf(buf,"Slýp %d Tran %d", inSlipTyp, inTranTyp);TPE_Print(buf);*/
	LOY_PrintCredit(inSlipTyp,inTranTyp);
}

/*-------------------------- End of Function XLS_PrintTran -------------------------*/


/**************************************************************
*
*  Function name : XLS_VirtualRedemption
*
*  Author        : H. MARTINOT
*
*  Purpose       : Redemption entry
*
*  Input         : inPurchaseAmount  : 13 ASCII digits
*				   inCardHolderId	 : 19 ASCII digits
  *				   inFlagPaymentCard : LOYALTY_ONLY_CARD or PAYMENT_CREDIT_CARD
*
*  Output        : outModifiedAmount : 13 ASCII digits
*
*  Return        : Return code (see interface.h)
*
***************************************************************/

byte XLS_VirtualRedemption( char *inPurchaseAmount, char *inCardHolderId, char *outModifiedAmount, byte inFlagPaymentCard )
{
	byte	theRetour;
	uint16	theErreur;
	uint16	i;
	byte	CrdHoldId[LG_CARD_HOLDER_ID+LG_PAD_CARD_HOLDER_ID];
	byte	pan[LG_PAN_PORTEUR];
	s_etat_xls *etat_xls;

	/* Terminated amount */
    byte thePurchaseAmount[LG_PURCH_AMOUNT+1];		
    byte theModifiedAmount[LG_PURCH_AMOUNT+1];

    etat_xls = (s_etat_xls *)DATA_GetContext();

/*#ifdef EMV_PAYMENT_APP_ACTIVATED
#ifdef MPCOS_EMV
	etat_xls->df_current = EMV;
	etat_xls->ef_current = 0;
#endif
#endif*/

	xtl_memset(thePurchaseAmount,'0',LG_PURCH_AMOUNT);
    xtl_memset(theModifiedAmount,'0',LG_PURCH_AMOUNT);
	thePurchaseAmount[LG_PURCH_AMOUNT]=0;
	theModifiedAmount[LG_PURCH_AMOUNT]=0;
	xtl_memcpy(thePurchaseAmount,inPurchaseAmount,LG_PURCH_AMOUNT);

	if( !XLS_CheckInitialisation((int16 *)&theErreur) )
    {
    	TRACE_Erreur((L1,"Terminal"),(L2,"Not Initialized"));
		return XLS_CARD_REMOVED;
    }
	else
	{
		if( XLS_Check_Allow_Transaction((int16 *)&theErreur) )
		{
			xls_reset_etat_xls();
#ifdef TL_TO_YTL_ACTIVATED
					if (!XLS_GetCurcyMode (&etat_xls->curcy_mode))
						return XLS_CURCY_MODE_CORRUPTED;
					etat_xls->curcy_rule = etat_xls->curcy_mode;
#endif
			if (XLS_IntroductionCarte(FALSE))
			{
				etat_xls->fidelite_chainee = TRUE;
				for (i=0; i<LG_PAD_CARD_HOLDER_ID; i++)
					CrdHoldId[i]='F';
				xtl_memcpy(CrdHoldId + LG_PAD_CARD_HOLDER_ID,inCardHolderId,LG_CARD_HOLDER_ID);
				xtl_conv_in_bcd(pan,(char *)CrdHoldId,LG_CARD_HOLDER_ID+LG_PAD_CARD_HOLDER_ID);
#ifdef NO_PAN_CONSOLIDATION
				if (TRUE == TRUE)
#else
				if (xtl_memcmp(pan,etat_xls->InfoUser.pan,LG_PAN_PORTEUR)==0)
#endif
				{
					/* Terminated amount */
					theRetour = LOY_Redemption((byte *)thePurchaseAmount,(byte *)theModifiedAmount,(int16 *)&theErreur);
					if (theRetour != TRUE)
					{
						theRetour = (byte) theErreur;
						/* Akbank Reported bug  */
/*						if (theErreur != XLS_CANCELED && theErreur != XLS_CARD_REMOVED)							
							XLS_ClearFile(F_RESULTAT_CREDIT,(int16 *)&theErreur);*/
					}
					else
					{
						theRetour = XLS_NO_ERROR;

						/* Return XLS_CARD_REMOVED if card is removed */
						if( TPE_carte_absente(LECTEUR_CLIENT))
							theRetour = XLS_CARD_REMOVED;	
					}
			

					xtl_memcpy(outModifiedAmount,theModifiedAmount,LG_PURCH_AMOUNT);
					return theRetour;
				}
				else
				{
					TRACE_Info((L1,"CrdHolderId"),(L2,"Nok"));
					TRACE_Info((L1,"%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",pan[0],pan[1],pan[2],pan[3],pan[4],pan[5],pan[6],pan[7]),(L2,"%.2x%.2x%.2x",pan[8],pan[9],pan[10]));
					TRACE_Info((L1,"%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",etat_xls->InfoUser.pan[0],etat_xls->InfoUser.pan[1],etat_xls->InfoUser.pan[2],etat_xls->InfoUser.pan[3],etat_xls->InfoUser.pan[4],etat_xls->InfoUser.pan[5],etat_xls->InfoUser.pan[6],etat_xls->InfoUser.pan[7]),(L2,"%.2x%.2x%.2x",etat_xls->InfoUser.pan[8],etat_xls->InfoUser.pan[9],etat_xls->InfoUser.pan[10]));
					TPE_Print("Pan consolidation");
					return XLS_CARD_REMOVED;
				}
			}
			else
			{
				TRACE_Info((L1,"IntroCrd"),(L2,"Nok"));
				return XLS_CARD_REMOVED;
			}			
		}
		else
		{
			TRACE_Erreur((L1,"Transac"),(L2,"Not Allowed"));

			return XLS_CARD_REMOVED;
		}
		/* Return XLS_CARD_REMOVED if card is removed */
		if( TPE_carte_absente(LECTEUR_CLIENT))
			return XLS_CARD_REMOVED;	
			
		return XLS_NO_ERROR;
	}
}
/*-------------------------- End of Function XLS_Redemption -------------------------*/



/**************************************************************
*
*  Function name : XLS_VirtualReward
*
*  Author        : H.MARTINOT
*
*  Purpose       : Virtual entry
*
*  Input         : inInstallmentNo		   : 2 ASCII digits
*
*  Output        : 
*
*  Return        : Return code (see interface.h)
*
***************************************************************/

byte XLS_VirtualReward(char *inInstallmentNo, txls_transactiondetail *outTransactionDetail)
{
	int16	theErreur;
	byte	theRetour;
	s_etat_xls *etat_xls;

	etat_xls = (s_etat_xls *)DATA_GetContext();

#ifdef EMV_PAYMENT_APP_ACTIVATED
#ifdef MPCOS_EMV
	etat_xls->df_current = EMV;
	etat_xls->ef_current = 0;
#endif
#endif


	theRetour = LOY_Reward((byte *)inInstallmentNo, (txls_transactiondetail *) outTransactionDetail, &theErreur);
	if (!theRetour)
	{
		theRetour = (byte)theErreur;
/*		XLS_ClearFile(F_RESULTAT_CREDIT,&theErreur);*/
	}
	else
	{
		theRetour = XLS_NO_ERROR;
	
		/* Return XLS_CARD_REMOVED if card is removed */
		if( TPE_carte_absente(LECTEUR_CLIENT))
			theRetour = XLS_CARD_REMOVED;	
	}			

	return theRetour;

}
/*-------------------------- End of Function XLS_VirtualReward -------------------------*/


/**************************************************************
*
*  Function name : XLS_CommitRwdTrx
*
*  Author        : H.MARTINOT
*
*  Purpose       : Commit data
*
*  Input         : inHostGeneratedCashback : 13 ASCII digits
*				   inApprovalCode		   : 6 ASCII digits
*
*  Output        : 
*
*  Return        : Return code (see interface.h)
*
***************************************************************/

byte XLS_CommitRwdTrx( char inHostCashBackOperation, char *inHostGeneratedCashback, char *inApprovalCode )
{

	uint32	theHostCB;
	byte	theRetour;
	int16	theErreur;
	s_etat_xls *etat_xls;

	etat_xls = (s_etat_xls *)DATA_GetContext();

#ifdef EMV_PAYMENT_APP_ACTIVATED
#ifdef MPCOS_EMV
	etat_xls->df_current = EMV;
	etat_xls->ef_current = 0;
#endif
#endif

	if(AsciiCmp(inHostGeneratedCashback, LG_PURCH_AMOUNT, MAX_CASHBACK_AMOUNT, 10) > 0)
		theHostCB=4294967295UL;
	else
		theHostCB=clib_atol(inHostGeneratedCashback);

	/* Add some Pan Consolidation at this step */
    if (etat_xls->typ_carte == C_MPCOS_EMV)
    {
    	if( !XLS_ReadCardFile(NULL,&theErreur))
		{
			theRetour = XLS_CARD_UNRECOGNIZED;
			return(theRetour);
		}
    }
    else
    {
	    if (!XLS_ReadUserFile(&theErreur))
		{
			theRetour = XLS_CARD_UNRECOGNIZED;
			return(theRetour);
		}
    }
	/* Add some Pan Consolidation at this step */

	theRetour = LOY_CardProcessing(inHostCashBackOperation, theHostCB, inApprovalCode, &theErreur);
	if (!theRetour)
	{
		theRetour = (byte) theErreur;
		XLS_ClearFile(F_RESULTAT_CREDIT,&theErreur);
	}
	else
		theRetour = XLS_NO_ERROR;
	
	return theRetour;

}
/*-------------------------- End of Function XLS_CommitRwdTrx -------------------------*/




/**************************************************************
*
*  Function name : XLS_VirtualReverse
*
*  Author        : H.MARTINOT
*
*  Purpose       : Return entry
*
*  Input         : inTranTyp      : type of transaction (see interface.h)
*				 : inAutorizationAmount
*				 : inReverseAmount
*				 : inInstallmentNo
*				 : inCardHolderId
*  Output        : 
*
*  Return        : Return code (see interface.h)
*
***************************************************************/

byte XLS_VirtualReverse(byte inTranTyp,unsigned char inCurrency, char *inAutorizationAmount, char *inReverseAmount, char *inInstallmentNo, char *inCardHolderId, txls_Returndetail *outReturnDetail)
{
	byte	theRetour;
	int16	theErreur;
	uint16	i;
	byte	CrdHoldId[LG_CARD_HOLDER_ID+LG_PAD_CARD_HOLDER_ID];
	byte	pan[LG_PAN_PORTEUR];
	s_etat_xls *etat_xls;

	/* Terminated amount */
    byte theAutorizationAmount[LG_PURCH_AMOUNT+1];		
	byte theReverseAmount[LG_PURCH_AMOUNT+1];		
    
	etat_xls = (s_etat_xls *)DATA_GetContext();

/*#ifdef EMV_PAYMENT_APP_ACTIVATED
#ifdef MPCOS_EMV
	etat_xls->df_current = EMV;
	etat_xls->ef_current = 0;
#endif
#endif*/

	xtl_memset(theAutorizationAmount,0x00,LG_PURCH_AMOUNT+1);
	xtl_memcpy(theAutorizationAmount,inAutorizationAmount,LG_PURCH_AMOUNT);

    xtl_memset(theReverseAmount,0x00,LG_PURCH_AMOUNT+1);
	xtl_memcpy(theReverseAmount,inReverseAmount,LG_PURCH_AMOUNT);
    
	if( !XLS_CheckInitialisation((int16 *)&theErreur) )
    {
    	TRACE_Erreur((L1,"Terminal"),(L2,"Not Initialized"));
		return XLS_CARD_REMOVED;
    }
	else
	{
		if( XLS_Check_Allow_Transaction((int16 *)&theErreur) )
		{
			xls_reset_etat_xls();
#ifdef TL_TO_YTL_ACTIVATED
					if (!XLS_GetCurcyMode (&etat_xls->curcy_mode))
						return XLS_CURCY_MODE_CORRUPTED;

					if (etat_xls->curcy_mode == YTL_MODE)
					{
						if (inCurrency == YTL_MODE)
							etat_xls->curcy_rule = YTL_RULE;
						else
						{
							if (inCurrency == TL_MODE)
								etat_xls->curcy_rule =  TL_RETURNED_YTL_RULE;
							else
								return XLS_RETURN_CURRENCY_WRONG;														
						}
					}
					else
					{
						if (inCurrency == TL_MODE)
							etat_xls->curcy_rule = TL_RULE;
						else
							return XLS_RETURN_CURRENCY_NOT_COMPLIANT;
					}
					
#endif
			if (XLS_IntroductionCarte(FALSE))
			{
				etat_xls->fidelite_chainee = TRUE;

				for (i=0; i<LG_PAD_CARD_HOLDER_ID; i++)
					CrdHoldId[i]='F';
				xtl_memcpy(CrdHoldId + LG_PAD_CARD_HOLDER_ID,inCardHolderId,LG_CARD_HOLDER_ID);
				xtl_conv_in_bcd(pan,(char *)CrdHoldId,LG_CARD_HOLDER_ID+LG_PAD_CARD_HOLDER_ID);
#ifdef NO_PAN_CONSOLIDATION
				if (TRUE == TRUE)
#else
				if (xtl_memcmp(pan,etat_xls->InfoUser.pan,LG_PAN_PORTEUR)==0)
#endif
				{
					/* Terminated amount */
					theRetour = LOY_ReverseReward((char *)theAutorizationAmount,(char *)theReverseAmount,inTranTyp,(byte *)inInstallmentNo, (txls_Returndetail *) outReturnDetail, &theErreur);

					/* error return mamagement sept 4th 2001 */
					if( !theRetour )
					{
						theRetour = (byte) theErreur; /* before callýng clear fýle */
						TRACE_Erreur((L1,"Return NOK"),(L2,"0x%.2x",theErreur));
/*						XLS_ClearFile(F_RESULTAT_CREDIT,(int16 *)&theErreur);*/
					}
					else
					{
						theRetour = XLS_NO_ERROR;

						/* Return XLS_CARD_REMOVED if card is removed */
						if( TPE_carte_absente(LECTEUR_CLIENT))
							theRetour = XLS_CARD_REMOVED;	
					}			

					TRACE_Info((L1,"return LoyReverse"),(L2,"0x%.2x",theErreur));				

					/* set the previous font */
					/*if(memcmp(theTmpBuffer2,"DEFAULT", 7)!=0)
					{
						//select the previous font 
						set_font(theTmpBuffer2);
					}*/

					return ( theRetour);
				}
				else
				{
					TRACE_Info((L1,"CrdHolderId"),(L2,"Nok"));
					return XLS_CARD_REMOVED;
				}
			}
			else
			{
				TRACE_Info((L1,"IntroCrd"),(L2,"Nok"));
				return XLS_CARD_REMOVED;
			}
		}
		else
		{
			TRACE_Erreur((L1,"Transac"),(L2,"Not Allowed"));
			return XLS_CARD_REMOVED;
		}
		if( TPE_carte_absente(LECTEUR_CLIENT))
			return(XLS_CARD_REMOVED);	

		return XLS_NO_ERROR;
	}
}
/*-------------------------- End of Function XLS_VirtualReverse -------------------------*/


/**************************************************************
*
*  Function name : XLS_CommitRevTrx
*
*  Author        : H.MARTINOT
*
*  Purpose       : Commit data
*
*  Input         : inApprovalCode		   : 6 ASCII digits
*				 : 
*  Output        : 
*
*  Return        : Return code (see interface.h)
*
***************************************************************/

byte XLS_CommitRevTrx( char *inApprovalCode )
{
	byte	theRetour;
	int16	theErreur;
	s_etat_xls *etat_xls;

	etat_xls = (s_etat_xls *)DATA_GetContext();

#ifdef EMV_PAYMENT_APP_ACTIVATED
#ifdef MPCOS_EMV
	etat_xls->df_current = EMV;
	etat_xls->ef_current = 0;
#endif	
#endif
	
	
	/* Add some Pan Consolidation at this step */
    if (etat_xls->typ_carte == C_MPCOS_EMV)
    {
    	if( !XLS_ReadCardFile(NULL,&theErreur))
		{
			theRetour = XLS_CARD_UNRECOGNIZED;
			return(theRetour);
		}
    }
    else
    {
	    if (!XLS_ReadUserFile(&theErreur))
		{
			theRetour = XLS_CARD_UNRECOGNIZED;
			return(theRetour);
		}
    }
	/* Add some Pan Consolidation at this step */


	theRetour = LOY_CardRevProcessing( inApprovalCode, &theErreur );
	if (!theRetour)
	{
		theRetour = (byte)theErreur;
		XLS_ClearFile(F_RESULTAT_CREDIT,&theErreur);
	}
	else
		theRetour = XLS_NO_ERROR;
	
	return theRetour;

}
/*-------------------------- End of Function XLS_CommitRevTrx -------------------------*/

/*-------------------------- End of File INTERFAC.C --------------------------*/



