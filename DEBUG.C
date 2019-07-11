
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
#include <string.h>

#include  NOS_H
#include  EMVDEF_H
#include MAINLOOP_H
#include  DEBUG_H


	  /*==========================================*
	   *           D E F I N I T I O N S          *
	   *==========================================*/
extern boolean Debug;


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

#if 0
void Debug_GrafPrint2 (char *Message, byte line)
{
	char Temp[100];

/*    GrphText_Format (&LineHight_13x7[0]);*/
    GrphText_ClrLine (line, FALSE);

	memset (Temp, 0, 100);
	strcpy(Temp, Message);
    GrphText_Write ((byte) line, (sint) 0, (char *) Temp,
                    (byte) FONT_13X7, (byte) strlen(Temp),
                    FALSE, FALSE);
    GrphText_DrawDisp ();
}

#endif

void DebugPrint(const char *Message)
{
  if (Debug)
  {
#ifndef GRAFDISP
	Display_UpDisplay((char *)Message);
	Mte_Wait(1000);
#else
/*    GrphText_Format (&LineHight_7x5[0]);
    GrphText_ClrLine (6, TRUE);
    GrphText_Write ((byte) 6, (sint) 15, (char *) Message,
                    (byte) FONT_7X5, (byte) strlen(Message),
                    TRUE, FALSE);
	Mte_Wait(1000);
*/
/*    GrphText_Format (&LineHight_7x5[0]);*/
    GrphText_ClrLine (6, TRUE);
    GrphText_Write ((byte) 6, (sint) 15, (char *) Message,
                    (byte) FONT_13X7, (byte) strlen(Message),
                    TRUE, FALSE);
	Mte_Wait(1000);

#endif
  }
}


void DebugPrintOnPaper(char *Message)
{
 if (Debug)
 {
	int i;

	PrntUtil_Cr(1);
	Printer_WriteStr("<");
	for(i=0;i<strlen(Message);i++)
	{
		if (Message[i] == 1) Printer_WriteStr("<SOH>");
		else if (Message[i] == 2) Printer_WriteStr("<STX>");
		else if (Message[i] == 3) Printer_WriteStr("<ETX>");
		else if (Message[i] == 4) Printer_WriteStr("<EOT>");
		else if (Message[i] == 5) Printer_WriteStr("<ENQ>");
		else if (Message[i] == 6) Printer_WriteStr("<ACK>");
		else if (Message[i] == 7) Printer_WriteStr("<BEL>");
		else if (Message[i] == 0x1C) Printer_WriteStr("<FS>");
		else if (Message[i] < 0x20) Printer_WriteStr(".");
		else Printer_Write(Message+i,1);
	}
	Printer_WriteStr(">");
	PrntUtil_Cr(1);
 }
}
