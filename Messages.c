/****************************************************************************
 *                                                                          *
 * FILE NAME:   MESSAGES                                                    *
 *                                                                          *
 * MODULE NAME:                                                             *
 *                                                                          *
 * PROGRAMMER:                                                              *
 *                                                                          *
 * DESCRIPTION: Menu Text Definition										*
 *                                                                          *
 * REVISION:    01.00 10/04/98.                                             *
 *                                                                          *
 * HISTORY:     <Version history>.                                          *
 *                                                                          *
 ****************************************************************************/

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
#include <stdlib.h>

#include NOS_H

#include DEFINE_H
#include APLMAIN_H
#include MAINLOOP_H
#include EMVDEF_H

#define THISE_MODULLE_IS_MESSAGE_C
#define NB_FILES	5


	  /*==========================================*
	   *          D E F I N I T I O N S           *
	   *==========================================*/


/*--- Files name ---
 The files must stay together in the same order */
#define NB_FILES	5

const char FILE_GP[]	= "genprm";			/*name of the general parameter file */
const char FILE_TR[]	= "trans_";			/*name of the TRansaction file */
const char FILE_R1[]	= "slip_1";			/*name of the transaction ticket 1 file */
const char FILE_R2[]	= "slip_2";			/*name of the transaction ticket 2 file */
const char FILE_AS[]    = "aktest";			/*AKBANK TEST file */

/*--- Displayed messages --- */
const char MESSAGE_BANKNAME[]            = "    AKBANK";
const char MESSAGE_KEY_TO_STOP[]         = " B�T�RMEK ���N    B�R TU�A BAS";
const char MESSAGE_SWIPE_CARD[]          = "KART GE��R�N�Z ";
const char MESSAGE_ENTER_AMOUNT[]        = "TUTAR";
const char MESSAGE_ENTER_PIN[]           ="��FRE?";
const char MESSAGE_HOST_PHONE1[]         ="TELEFON NO 1:   ";
const char MESSAGE_HOST_PHONE2[]         ="TELEFON NO 2:   ";
const char MESSAGE_HOST_PHONE3[]         ="TELEFON NO 3:   ";
const char MESSAGE_MERCHANT_ID[]         ="��YER� NO:  ";
const char MESSAGE_TERMINAL_ID[]         ="TERM�NAL NO:";
const char MESSAGE_WAIT[]                = "   BEKLEY�N�Z   ";
const char MESSAGE_PLEASE_WAIT[]         = "L�TFEN BEKLEY�N";
const char MESSAGE_OUTSIDE[]             = "SANTRAL �IKI� NO";
const char MESSAGE_BUZZER_OPEN[]			= "Tu� sesi -> A��k  ";
const char MESSAGE_BUZZER_CLOSE[]		= "Tu� sesi -> Kapal�";
const char MESSAGE_DEBUG_OPEN[]			= "Debug -> A��k";
const char MESSAGE_DEBUG_CLOSE[]			= "Debug -> Kapal�";

/* print process related messages */
const char MESSAGE_PRINTING[]            = "    BASILIYOR";
const char MESSAGE_SECOND_TICKET[]       = 	" B�R TU�A BASIN ";
const char MESSAGE_NOPAPER[]             = " PR�NTER KA�IDI      YOK";

/* Messages displayed during connection */
const char MESSAGE_DIALING[]             = "    ARANIYOR";
const char MESSAGE_NO_RESPONSE[]         = "   CEVAP YOK";
const char MESSAGE_ANSWERED[]            = "  BA�LANIYOR..";
const char MESSAGE_CONNECTED[]           = "  BA�LANILDI ";
const char MESSAGE_USERBREAK[]           = "KULLANICI KEST�";


/*--- Print messages --- */
/* Transaction Tickets printout */
const char MESSAGE_T_CARDNR[]            = "KART NO";
const char MESSAGE_T_EXP[]               = "SON KUL. TAR�H�:";
const char MESSAGE_T_SN[]                = "NO:";
const char MESSAGE_T_AUTHCODE[]          = "ONAY KODU:";
const char MESSAGE_T_SALE[]              = "SATI�";
const char MESSAGE_T_AMOUNT[]            = "TUTAR";
const char MESSAGE_T_KEEPTHISPAPER[]     = "BU BELGEY� SAKLAYINIZ";
const char MESSAGE_T_GOODSRECEIVED[]     = "TUTAR KAR�ILI�I MAL VEYAH�ZMET ALDIM.";
const char MESSAGE_T_SIGN[]              = "�MZA:___________________";
const char csZEROS[]                    = "0000";
const char csSTARS[]                    = "*****************";

/*--- Warning and error messages --- */
const char MESSAGE_IMPOSS[]	            = "    �MKANSIZ    ";
const char MESSAGE_REFUSED[]	            = "   REDDED�LD�   ";
const char MESSAGE_ERRAPPL[]	            = " PROGRAM HATASI";
const char MESSAGE_SYSTEM_ERROR[]        =" S�STEM HATASI ";
const char My_Message [NUM_OF_MSGS][TEXT_MSG_LEN_SIZE] =
				{
					"    HAT YOK                     ",
					" �EV�R SES� YOK                 ",
					"   CEVAP YOK      TEKRAR ARAMA  ",
					"   CEVAP YOK                    ",
					"  MODEM HATASI    TEKRAR ARAMA  ",
					" YANLI� TEL.No                  ",
					"  TEL. No YOK                   ",
					" PREF�X HATASI                  ",
					"  BYTE FORMAT        HATASI     ",
					"     COM1`e       BA�LANAMADI   ",
					"     ME�GUL       TEKRAR ARAMA  ",
					"BA�LANILDI 300                  ",
					"BA�LANILDI 1200                 ",
					"BAGLANILDI 2400                 ",
					"    ARANIYOR                    "
				};

TerminalParams lsTerminalDataStruct;


EMVLanguages elsLangs = 
{
  0x01,
  0x00,
  {
    {
      "tr",
      0x09,
      "TURKCE",
      {
        "DIL SECINIZ",
        "TUTAR",
        "TUTAR DOGRU MU?",
        "ONAYLANDI",
        "    BANKANIZI     ARAYIN  ",
        "IPTAL VEYA GIRIS",
        "KART HATASI",
        "ONAYLANMADI/RED",
        "TUTAR GIRINIZ",
        "SIFRE GIRINIZ",
        "HATALI SIFRE",
        "KARTI TAKINIZ",
        "KABUL EDILMEDI",
        "SIFRE           KABUL EDILDI",
        "    LUTFEN        BEKLEYIN",
        "SISTEM HATASI",
        "KARTI CIKARTINIZ",
        "CIPI OKUTUNUZ",
        "MANYETIK SERIDI OKUTUNUZ",
        "   TEKRAR     DENEYINIZ",
        "ONAYA ZORLA",
        "ONLINE ISLEM",
        "ONAYLANDI MI?",
        "  SON SIFRE   DENEMESI",
        "ONLINE PIN GIR",
        "UYGULAMA SECINIZ",
        "NAKIT ALIM"
      }
    }
  }
	};


