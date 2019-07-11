

/**************************************************************************
  FILE NAME		: APLMAIN.C
  MODULE NAME	: 
  PROGRAMMER	: 
  DESCRIPTION	: 
  REVISION		: 00.01   
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
#include <stdlib.h>

/* #include LIPTYPES_DEF */
#include NOS_H
#include NURIT_DEF


#include DEFINE_H
#include APLMAIN_H
#include MAINLOOP_H
#include EMVIF_H

/*#include SLIPS_H*/ /* test amacli */
/* UPDATE 01_18 07/12/99 Tanýmlanmasý zorunlu deðiþken */


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
void main(void)
{
	MainLoop(); /*go into the main loop and stay there */
}
              	