/****************************************************************************
 *                                                                          *
 * FILE NAME:   MONDEX.C                                                    *
 *                                                                          *
 * MODULE NAME: Mondex.                                                     *
 *                                                                          *
 * PROGRAMMER:  Nimrod Stoler.                                              *
 *                                                                          *
 * DESCRIPTION: An implementation of the Mondex International Application   *
 *              Protocol.                                                   *
 *              When this application is running both cards should be       *
 *              personalized and customized                                 *
 *                                                                          *
 *              This is a test version. The PAYER's smart card is in this   *
 *              Pin Pad's socket, the PAYEE's smart card is in the other    *
 *              Pin Pad's socket. Both Pin Pads are connected via RS232     *
 *                                                                          *
 * REVISION:    01.00 05/05/97.                                             *
 * NOTE:        This implementation is based on the 1996(Version 3-0) specs *
 *              This module will not work under INTEL processors because    *
 *              it assumes MOTOROLA type ordering.                                                            *
 ****************************************************************************/
#define _E3744_DBG

	  /*==========================================*
	   *         I N T R O D U C T I O N          *
	   *==========================================*/

	  /*==========================================*
	   *             I N C L U D E S              *
	   *==========================================*/
/*#include "config.h"*/

#include "project.def"

#define NURIT_WITH_SMART_CARD
#ifdef NURIT_WITH_SMART_CARD

#include INTERFAC_H

/*#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <mriext.h>

#include NOS_H

#include ICCTRANS_H*/

#include DEFINE_H
#include ERRORS_H
#include HOSTCOMM_H
#include SMART_H


	  /*==========================================*
	   *           D E F I N I T I O N S          *
	   *==========================================*/

#define    SmartOK           0
#define    SmartNotPresent   1
#define    SmartNeedsVCC     2
#define    SmartNoAnswer     3
#define    SmartTimeOut      4
#define    SmartParityError  5
#define    SmartWriteError   6
#define    SmartWronge       7  /* smart card isn't SOLAIC card */
#define    SmartDataError    8  /* input smart card data is invalid */


/*------ taken from CDC_SMRT -------*/
#define  E3744_PIN_ADR             0x20
#define  E3744_SYSTEM_ADR          0x30
#define  E3744_TYPE_ADR            0x38
#define  E3744_AMOUNT1_ADR         0x40
#define  E3744_AMOUNT2_ADR         0x48
#define  E3744_ANSWER_SIZE        6
#define  E3744_HISTORY_SIZE       6

/* common return values */
#define   OK               1
#define   RESET            0
#define   NULL             0

/* Required for Pin Pad communcations */
#define   STX              0x02
#define   ETX              0x03
#define   ACK              0x06

#define  GRAFDISP
/*#define  E3744_DBG*/

	  /*==========================================*
	   *       P R I V A T E  T Y P E S           *
	   *==========================================*/

/* This  is used inorder to communicate with the transport layer */
     IccInstructStruct Transporter;

/* for E3744 smart card */
     const  usint PIN_NUM[2]={0,0xffff};     /* DIRECT */
     const  char  CURRENT_BALANCE[]="BAK˜YE :";
     const  char  SMART_CARD_ERROR[]="KART HATASI";


            byte  SerialNum[6];
            byte  TESTED_CARD_PORT;


	  /*=========================================*
	   *        M I S C E L L A N E O U S        *
	   *=========================================*/
int errno;

	   /*=========================================*
	    *   P U B L I C     F U N C T I O N S     *
	    *=========================================*/

#if 0
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
byte E3744_AkbankSelect( void) 
{
byte ret;
byte buf_in[2];
byte buf_out[256];
char buf_ascii[5];


  buf_in[0]=0x29;
  buf_in[1]=0x01;
  memset(buf_ascii,0,5);
  TESTED_CARD_PORT=0;
  memset( &Transporter, 0, sizeof( IccInstructStruct));
  Transporter.Class        =0xBC;
  Transporter.Instruct     =0xA4;
  Transporter.P1           =0x00;
  Transporter.P2           =0x00;
  Transporter.DataToCard   =buf_in;
  Transporter.DataFromCard =buf_out;
  Transporter.Le           = 255;
  Transporter.Lc           = 2;
  Transporter.port_indx    = TESTED_CARD_PORT;
  ret = E3744_RetConv( ICC_Command( &Transporter));
  if( ret == SmartOK)
  {
	  
     if((Transporter.SW1==0x90) && (Transporter.SW2==0x00) )
     {
		memcpy(buf_ascii,"9000",4);
		Display_UpDisplay ("Select File :");
		Display_DownDisplay (buf_ascii);
		Kb_WaitForKey();
        return( SmartOK);
     }
     ret = SmartDataError;
  }
  Display_UpDisplay("SELECT ERROR");
  Kb_WaitForKey();
  return( ret);
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
byte E3744_AkbankRead( void)
{
byte ret;
byte buf_out[8];
char buf_ascii[8];

  TESTED_CARD_PORT=0;
  Transporter.Class        =0xBC;
  Transporter.Instruct     =0xB0;
  Transporter.P1           =0x00;
  Transporter.P2           =0x00;
  Transporter.DataFromCard =buf_out;
  Transporter.Le           = 8;
  Transporter.port_indx    = TESTED_CARD_PORT;
	 ret = E3744_RetConv( ICC_Command( &Transporter));
  if( ret == SmartOK)
  {
     if((Transporter.SW1==0x90) && (Transporter.SW2==0x00) )
     {
		Utils_BinToAscii ((char *)buf_out, buf_ascii, Transporter.Lr);
		Display_UpDisplay ("Read From File :");
		Display_DownDisplay (buf_ascii);
		Kb_WaitForKey();
	return( SmartOK);
     }
     ret = SmartDataError;
  }
  Display_UpDisplay ("SMART READ ERROR");
  Kb_WaitForKey();
  return( ret);
}


byte SmartAkTest_ResetCardInt (void) /* data from smart card */
{
IccAnswerToResetStruct answer;
byte ret,Status;
byte buf[16];
char str[40];

    answer.Data       = buf;
    answer.SizeOfData = 16;
    answer.port_indx  = TESTED_CARD_PORT;
    ICC_Status( TESTED_CARD_PORT);
    ret = E3744_RetConv( ICC_AnswerToReset( &answer) );

	if ((buf[5] == 0x90) && (buf[6] == 0x00))
	{
      sprintf(str,"RESET :           %.2X%.2X%.2X%.2X%.2X%.2X%.2X",
		   buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
      Display_WriteStr(DSPL1, str);
	  return(ret);
	}
	Display_UpDisplay ("SMART CARD ERROR");
	Kb_WaitForKey();
    return(ret);
}


/*-------------------------------------------------------*
 * FUNC
 * DESCR
 * RET
 * NOTE
 *-------------------------------------------------------*/
void Smart_AkBankDemoExt ()
{
  byte Status;

  TESTED_CARD_PORT=0;
	SmartTest_IsInsertOrNot (&Status);
	switch (Status)
	{
	case ICC_OK :
		Mainloop_DisplayWait("Card ok",WAIT_WARNING);
		E3744_AkbankSelect();
		E3744_AkbankRead();
		break;
	case ICC_NEEDS_RESET :
		Mainloop_DisplayWait("Scratched Card",WAIT_WARNING);
		SmartAkTest_ResetCardExt();
		break;
	case ICC_BAD_CARD :
		Mainloop_DisplayWait("Bad Card",WAIT_WARNING);
		break;
	case ICC_SOCKET_EMPTY :
	default :
		break;
	}
}

/*-------------------------------------------------------*
 * FUNC
 * DESCR
 * RET
 * NOTE
 *-------------------------------------------------------*/
void Smart_AkBankDemoInt (sint PortNumber)
{
  byte Status;

  TESTED_CARD_PORT=PortNumber;

	for(;;)
     {
	SmartTest_IsInsertOrNot (&Status);
	switch (Status)
	{
	case ICC_OK :
		Display_ClrDsp ();
		Display_UpDisplay ("Card Ok");
		break;
	case ICC_NEEDS_RESET :
		Display_ClrDsp ();
		Display_UpDisplay ("Scratched Card");
		SmartAkTest_ResetCardExt();
		break;
	case ICC_BAD_CARD :
		Display_ClrDsp ();
		Display_UpDisplay ("Bad Card");
		break;
	case ICC_SOCKET_EMPTY :
		Display_ClrDsp ();
		Display_UpDisplay ("No Card");
		break;
	default :
		break;
	}
       if (Kb_Read() == MMI_ESCAPE) break;
     }
}



/*-------------------------------------------------------*
 * FUNC
 * DESCR
 * RET
 * NOTE
 *-------------------------------------------------------*/
void Smart_AkBankTest8 (sint PortNumber)
{
  byte Status;

  TESTED_CARD_PORT=PortNumber;
  Display_ClrDsp ();
  Display_UpDisplay ("KARTI TAKINIZ");
	
  do{
  SmartTest_IsInsertOrNot (&Status);
  if(Kb_Read()) break;
  }
  while (Status ==ICC_SOCKET_EMPTY);

    SmartAkTest_ResetCardInt();
	ICC_Status( TESTED_CARD_PORT);
    Kb_WaitForKey();

	SmartTest_IsInsertOrNot (&Status);
	switch (Status)
	{
	case ICC_OK :
		Display_ClrDsp ();
		Display_UpDisplay ("Card Ok");
		Kb_WaitForKey();
		break;
	case ICC_NEEDS_RESET :
		Display_ClrDsp ();
		Display_UpDisplay ("Scratched Card");
		Kb_WaitForKey();
		break;
	case ICC_BAD_CARD :
		Display_ClrDsp ();
		Display_UpDisplay ("Bad Card");
		Kb_WaitForKey();
		break;
	case ICC_SOCKET_EMPTY :
		Display_ClrDsp ();
		Display_UpDisplay ("No Card");
		Kb_WaitForKey();
		break;
	default :
		break;
	}
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: SmartTest_SetSmartPort
 * DESCRIPTION:   Set current smart port.
 * RETURN:        void.
 * NOTES:         none.
 * ------------------------------------------------------------------------ */
void SmartTest_SetSmartPort(byte port_indx)
{
  TESTED_CARD_PORT=port_indx;
}



/* --------------------------------------------------------------------------
 * FUNCTION NAME: SmartTest_IsInsert
 * DESCRIPTION:   Check card in current smart port.
 * RETURN:        TRUE, if smart card is insert.
 * NOTES:         none.
 * ------------------------------------------------------------------------ */
void SmartTest_IsInsertOrNot(byte *Status)
{
   *Status = ICC_Status(TESTED_CARD_PORT);
}



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

byte SmartAkTest_ResetCardExt (void) /* data from smart card */
{
IccAnswerToResetStruct answer;
byte buf[16];
char str[40];
byte ret;

    answer.Data       = buf;
    answer.SizeOfData = 16;
    answer.port_indx  = TESTED_CARD_PORT;
    ICC_Status( TESTED_CARD_PORT);
    ret = E3744_RetConv( ICC_AnswerToReset( &answer) );

	if ((buf[5] == 0x90) && (buf[6] == 0x00))
	{
      sprintf(str,"RESET :           %.2X%.2X%.2X%.2X%.2X%.2X%.2X",
		   buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
      Display_WriteStr(DSPL1, str);
      for(;;) if(Kb_Read()) break;
	  return(ret);
	}
	Display_UpDisplay ("SMART CARD ERROR");
	Kb_WaitForKey();
    return(ret);
}

#endif





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
byte E3744_AnsToReset (byte  *buf) /* data from smart card */
{
IccAnswerToResetStruct answer;
byte ret;

    answer.Data       = buf;
    answer.SizeOfData = 16;
    answer.port_indx  = TESTED_CARD_PORT;
    ICC_Status( TESTED_CARD_PORT);
    ret = E3744_RetConv( ICC_AnswerToReset( &answer) );

#ifdef E3744_DBG
{
char str[40];
   sprintf(str,"R=%2d S=%2d H=%2d  %.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",ret,
                 answer.SizeOfData,answer.SizeOfHistory,
                 buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
   Display_WriteStr(DSPL1, str);
   for(;;) if(Kb_Read()) break;
}
#endif

    if(ret == SmartOK)
    {
        if( (answer.SizeOfData == E3744_ANSWER_SIZE) &&
            (answer.SizeOfHistory == E3744_HISTORY_SIZE) )
        {
#ifdef E3744_DBG
{
   Display_WriteStr(DSPL1, "ATR OK!");
   for(;;) if(Kb_Read()) break;
}
#endif
                  return(SmartOK);
        }
        else
        {
#ifdef E3744_DBG
{
   Display_WriteStr(DSPL1, "ATR: WRONG SIZE!");
   for(;;) if(Kb_Read()) break;
}
#endif
                  return(SmartWronge);
        }
    }
    return(ret);
}


/* --------------------------------------------------------------------------
 *
 * FUNCTION NAME: E3744_RetConv
 *
 * DESCRIPTION:   Converts the TransStatusEnum error codes to Solaic errors
 *                codes.
 *
 * RETURN:        SmartOK or error code.
 *
 * NOTES:         none.
 *
 * ------------------------------------------------------------------------ */

byte E3744_RetConv(byte trans_ret)
{

    if(trans_ret == ICC_OK)
       return(SmartOK);
    if(trans_ret==ICC_SOCKET_EMPTY)
       return(SmartNotPresent);
    return(SmartWronge);
}


/*************************************************************************/
#endif /*of NURIT_WITH_SMART_CARD*/
