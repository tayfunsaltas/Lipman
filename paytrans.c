/**************************************************************
*
*  Nom du fichier :  paytrans.c
*
*  Auteur :  F. Mougenel
*
*  Objet : point d'entree active par une insertion carte
*
*	Origine :
*
* Modifications : 
*    date      auteur   description
* 16/03/2000  A.Dorado  ajout du paramètre inAmountRequest dans la fonction pay_processing( )
***************************************************************/

#include <confapp.h>
#include <inc_term.h>
#include <inc_xls.h>
#include <lib_gene.h>

#include <paytrans.h>
#include <ticket.h>
#include <intertpe.h>
#include <montant.h>

/*#define DEF_LIB_DATE*/
#include <lib_date.h>

#include <Fmtpts.h>

#define CURCY_LIB 	"TRL"

	#ifdef FRANCAIS
		/*CONST char sImp_titre[LG_MSG_IMP]         =	"  PAYMENT TRANSACTION   ";
		CONST char sImp_specimen[LG_MSG_IMP]      =	" THIS NOT A VAT RECEIPT ";
		CONST char sImp_nombre[LG_MSG_IMP]        =	"  123456789  987654321  ";
		CONST char sImp_EFT_OPER[LG_MSG_IMP]      =	"   EFT  No.69 OPER.369  ";
		CONST char sImp_total[LG_MSG_IMP]       	=	"TOTAL                   ";
		CONST char sImp_VAC[LG_MSG_IMP]        	=	"V.A.C                   ";
		CONST char sImp_card_number[LG_MSG_IMP]   =	"CARD No 1234567891231   ";
		CONST char sImp_expiry[LG_MSG_IMP]       	=	"EXPIRY DATE 05/01       ";
		CONST char sImp_authent[LG_MSG_IMP]       =	"AUTH NUMBER 147258      ";
		CONST char sImp_ligne_separateur[LG_MSG_IMP]     =	"------------------------";*/
		extern CONST char sImp_titre[LG_MSG_IMP];
		extern CONST char sImp_specimen[LG_MSG_IMP];
		extern CONST char sImp_nombre[LG_MSG_IMP];
		extern CONST char sImp_EFT_OPER[LG_MSG_IMP];
		CONST char sImp_total[LG_MSG_IMP]       	=	"TOTAL                   ";
		extern CONST char sImp_VAC[LG_MSG_IMP];
		extern CONST char sImp_card_number[LG_MSG_IMP];
		extern CONST char sImp_expiry[LG_MSG_IMP];
		extern CONST char sImp_authent[LG_MSG_IMP];
		extern CONST char sImp_ligne_separateur[LG_MSG_IMP];
	#endif
	#ifdef ANGLAIS
		/*CONST char sImp_titre[LG_MSG_IMP]         =	"  PAYMENT TRANSACTION   ";
		CONST char sImp_specimen[LG_MSG_IMP]      =	" THIS NOT A VAT RECEIPT ";
		CONST char sImp_nombre[LG_MSG_IMP]        =	"  123456789  987654321  ";
		CONST char sImp_EFT_OPER[LG_MSG_IMP]      =	"   EFT  No.69 OPER.369  ";
		CONST char sImp_total[LG_MSG_IMP]       	=	"TOTAL                   ";
		CONST char sImp_VAC[LG_MSG_IMP]        	=	"V.A.C                   ";
		CONST char sImp_card_number[LG_MSG_IMP]   =	"CARD No 1234567891231   ";
		CONST char sImp_expiry[LG_MSG_IMP]       	=	"EXPIRY DATE 05/01       ";
		CONST char sImp_authent[LG_MSG_IMP]       =	"AUTH NUMBER 147258      ";
		CONST char sImp_ligne_separateur[LG_MSG_IMP]     =	"------------------------";*/
		extern CONST char sImp_titre[LG_MSG_IMP];
		extern CONST char sImp_specimen[LG_MSG_IMP];
		extern CONST char sImp_nombre[LG_MSG_IMP];
		extern CONST char sImp_EFT_OPER[LG_MSG_IMP];
		CONST char sImp_total[LG_MSG_IMP]       	=	"TOTAL                   ";
		extern CONST char sImp_VAC[LG_MSG_IMP];
		extern CONST char sImp_card_number[LG_MSG_IMP];
		extern CONST char sImp_expiry[LG_MSG_IMP];
		extern CONST char sImp_authent[LG_MSG_IMP];
		extern CONST char sImp_ligne_separateur[LG_MSG_IMP];
	#endif
	#ifdef TURK
		/*CONST char sImp_titre[LG_MSG_IMP]         =	"  PAYMENT TRANSACTION   ";
		CONST char sImp_specimen[LG_MSG_IMP]      =	" THIS NOT A VAT RECEIPT ";
		CONST char sImp_nombre[LG_MSG_IMP]        =	"  123456789  987654321  ";
		CONST char sImp_EFT_OPER[LG_MSG_IMP]      =	"   EFT  No.69 OPER.369  ";
		CONST char sImp_total[LG_MSG_IMP]       	=	"TOTAL                   ";
		CONST char sImp_VAC[LG_MSG_IMP]        	=	"V.A.C                   ";
		CONST char sImp_card_number[LG_MSG_IMP]   =	"CARD No 1234567891231   ";
		CONST char sImp_expiry[LG_MSG_IMP]       	=	"EXPIRY DATE 05/01       ";
		CONST char sImp_authent[LG_MSG_IMP]       =	"AUTH NUMBER 147258      ";
		CONST char sImp_ligne_separateur[LG_MSG_IMP]     =	"------------------------";*/
		extern CONST char sImp_titre[LG_MSG_IMP];
		extern CONST char sImp_specimen[LG_MSG_IMP];
		extern CONST char sImp_nombre[LG_MSG_IMP];
		extern CONST char sImp_EFT_OPER[LG_MSG_IMP];
		CONST char sImp_total[LG_MSG_IMP]       	=	"TOTAL                   ";
		extern CONST char sImp_VAC[LG_MSG_IMP];
		extern CONST char sImp_card_number[LG_MSG_IMP];
		extern CONST char sImp_expiry[LG_MSG_IMP];
		extern CONST char sImp_authent[LG_MSG_IMP];
		extern CONST char sImp_ligne_separateur[LG_MSG_IMP];
	#endif
	CONST ligne_speciale adr_commer[3] = {	{ 1, "  O'BURGER              "},
														{ 0, "    675 MAIN STREET     "},
														{ 0, "    BOSTON, MA 02404    "} };


/**************************************************************
*
*  Nom de la fonction : pay_processing
*
***************************************************************/

byte pay_processing(byte * outMontant, byte inAmountRequest)
{

	if(inAmountRequest)
	{
		TPE_ClrScr();
		if( TPE_saisie_montant_valide(CURCY_LIB,(char *)outMontant, LG_PURCH_AMOUNT, 0, 1) )
		{
			TPE_CopieAffiche((sbyte*)sAff_AttenteL1,(sbyte*)sAff_AttenteL2);
			TPE_Tempo(1000);

			return(TRUE);
		}
		else
			return(FALSE);
	}
	else
	{
		TPE_CopieAffiche((sbyte*)sAff_AttenteL1,(sbyte*)sAff_AttenteL2);
		TPE_Tempo(1000);

		return(TRUE);
	}

}

/**************************************************************
*
*  Nom de la fonction : corps_ticket
*
***************************************************************/

static void corps_ticket(byte * inMontant)
{
	char 	theImpr[LG_MSG_IMP];
	char 	theTmpBuffer[LG_MSG_IMP];
	int	i;

	TPE_FormFeed(1);
	TPE_Print(sImp_titre);
	TPE_FormFeed(1);

	for(i=0;i<3;i++)
	{
		TPE_ImprimeLigne(&adr_commer[i]);
	}
	TPE_FormFeed(1);

	TPE_Print(sImp_specimen);
	TPE_FormFeed(1);

	TPE_Print(sImp_nombre);
	TPE_Print(sImp_EFT_OPER);
	XLS_TicketDate(NULL);

	TPE_FormFeed(1);

	xtl_memcpy(theImpr,sImp_total,LG_MSG_IMP);
	TPE_FormatLongAmount(NO_SIGNED,theTmpBuffer, clib_atol((char *)inMontant), 13, 2, CURCY_LIB, US_DISPLAY_FORMAT);
	xtl_strcpy(theImpr+NB_CAR_LIGNE_IMP-(xtl_strlen(theTmpBuffer)),theTmpBuffer);
	TPE_Print(theImpr);
	TPE_FormFeed(1);

	TPE_Print(sImp_VAC);
	TPE_Print(sImp_card_number);
	TPE_Print(sImp_expiry);
	TPE_Print(sImp_authent);

	TPE_FormFeed(1);
	TPE_Print(sImp_ligne_separateur);
}

/**************************************************************
*
*  Nom de la fonction : pay_printing
*
***************************************************************/
void pay_printing(byte * inMontant)
{
	TPE_CopieAffiche((sbyte*)sAff_AttenteL1,(sbyte*)sAff_AttenteL2);

	TPE_InitPrinter();

	corps_ticket(inMontant);

	XLS_TicketSuivant();
}

