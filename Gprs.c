/**************************************************************************
  FILE NAME:   GPRS.C
  MODULE NAME: GPRS
  PROGRAMMER:  MK
  DESCRIPTION: 
  REVISION:   15.11.2002 
 **************************************************************************/

	  /*==========================================*
	   *         I N T R O D U C T I O N          *
	   *==========================================*/

	  /*==========================================*
	   *             I N C L U D E S              *
	   *==========================================*/
/* General Header File */
#include "project.def"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

#include NURIT_DEF
#include KB_H
#include KBHW_H
#include MTE_H
#include PRINTER_H
#include PRNTUTIL_H
#include DISPLAY_H
#include GRPHDISP_H
#include GRPHTEXT_H
#include FORMATER_H
#include FORMATER_DEF
#include GPRS_H
#include DEFINE_H
#include ERRORS_H
#include gsmradio_prm
#include DNS_H
#include PROTMNGR_DEF
#include EMVDEF_H

#include gsm_msg_def
#include gsmradio_h

/*=========================================*
*   P R I V A T E   F U N C T I O N S      *
*==========================================*/

char APN[12];
char ISP_NAME[32];
char GPRS_User[10];
char GPRS_Passwd[10];

GPRS_Connect_Param	lsGPRS_Connect_Param;

extern boolean DEBUG_MODE;


unsigned char MKSocketNo;
typedef struct
{
		SOCKET 		LipmanSocket;                
} MKSocketStruct;

MKSocketStruct MKSockets[254];

unsigned char GPRS_SetXLSSocket(SOCKET sd)
{
	if (MKSocketNo>254) MKSocketNo=0;
	else MKSocketNo++;
	MKSockets[MKSocketNo].LipmanSocket= sd;
	return MKSocketNo;
}

SOCKET GPRS_GetXLSSocket(unsigned char sd)
{
	return (MKSockets[sd].LipmanSocket);
}
 /* =================================================================================
 *
 * FUNCTION NAME: Dialog_Domain
 * DESCRIPTION: private function responsible to obtain the domain name from the user
 * RETURN: TRUE - the user pressed <ENTER> to input the domain name
 *         FALSE - the user pressed <ESC>
 *
 * NOTES:      none.
 *
 * ================================================================================ */
static boolean Dialog_Domain(char *domain_name)
{
    dialoge input;
    byte key;
 
    /*initializes the dialoge buffer declared above*/
    input.header       = (char *)"INSERT DOMAIN:";
    input.format       = (void *)domain_name;
    input.format_flags = __STRING;
    input.length       = 255;
    input.return_value = 0;
 
    /*enables the SoftKB module, draws the SoftKB keys and enables the Touch Panel*/
    SoftKB_Enable();
 
    /*runs the dialog*/
    key = Formater_DialogeBox((void *)&input);
 
    /*disables the software keyboard*/
    SoftKB_Disable();
 
    /*if the last key the user pressed is <ENTER> returns TRUE, else returns FALSE*/
    if (key != ENTER)
      return FALSE;
    else
      return TRUE;
 }
 

 /* =================================================================================
 *
 * FUNCTION NAME: Display_Result
 *
 * DESCRIPTION: private function responsible to display the DNS_Resolver function's
 *              return value. Upon Success, the function displays and prints the
 *              Domain name and its IP address
 * RETURN:    1 - DNS_Resolver() - returns 'DNS_SUCCESS'
 *            0 - DNS_Resolver() - returns any code but 'DNS_SUCCESS'
 *
 * NOTES:      none.
 * 
 * ================================================================================ */
usint Display_Result(sint ret_val,char* domain_name, char* host_ip)
 {
      /*format of lines for the Display_FormatWrite function*/
      byte lines[3];
      lines[0] = 12;
      lines[1] = 12;
      lines[2] = END_OF_LINES;
 
      /*switch statement of the return value of the DNS_Resolver function*/
      switch(ret_val)
      {
           /*upon success the return value is displayed and printed. The Domain name and its
            * IP address are printed/displayed*/
           case DNS_SUCCESS :
           {
                Display_FormatWrite(DSPL1,1,LEFT_JST ,"DNS_SUCCESS");
                Mte_Wait(2000);
                Printer_WriteStr("DNS_SUCCESS");
                PrntUtil_Cr(2);
 
                /*clears the screen*/
                Display_Cls(DSPL1);
 
                /*pops up a window from the (5,30) point on the graphical display*/
                GrphText_PopUp(30,5, 128,(byte*)lines, TRUE);
 
                /*displays the above buffers in lines 1 and 2*/
                Display_FormatWrite(DSPL1,1,LEFT_JST ,(char*)domain_name);
                Display_FormatWrite(DSPL1,2,LEFT_JST ,(char*)host_ip);
                Mte_Wait(2000);
 
                /*prints the domain name and the IP address*/
                Printer_WriteStr((char*)domain_name);
                Printer_WriteStr("\n");
                Printer_WriteStr((char*)host_ip);
                Printer_WriteStr("\n");
                PrntUtil_Cr(2);
                    break;
            }
            case DNS_SEND_ERROR :
            {
                Display_FormatWrite(DSPL1,1,LEFT_JST ,"DNS_SEND_ERROR");
                Mte_Wait(2000);
                Printer_WriteStr("DNS_SEND_ERROR");
                PrntUtil_Cr(2);
                    break;
            }
            case DNS_RECEIVE_ERROR :
            {
                Display_FormatWrite(DSPL1,1,LEFT_JST ,"DNS_RECEIVE_ERROR");
                Mte_Wait(2000);
                Printer_WriteStr("DNS_RECEIVE_ERROR");
                PrntUtil_Cr(2);
                    break;
            }
            case DNS_OPEN_ERROR:
            {
                Display_FormatWrite(DSPL1,1,LEFT_JST ,"DNS_OPEN_ERROR");
                Mte_Wait(2000);
                Printer_WriteStr("DNS_OPEN_ERROR");
                PrntUtil_Cr(2);
                    break;
            }
            case DNS_PACKING_ERROR:
            {
                Display_FormatWrite(DSPL1,1,LEFT_JST ,"DNS_PACKING_ERROR");
                Mte_Wait(2000);
                Printer_WriteStr("DNS_PACKING_ERROR");
                PrntUtil_Cr(2);
                    break;
            }
            case DNS_ERROR :
            {
                Display_FormatWrite(DSPL1,1,LEFT_JST ,"DNS_PACKING_ERROR");
                Mte_Wait(2000);
                Printer_WriteStr("DNS_ERROR");
                PrntUtil_Cr(2);
                    break;
            }
      }
      if ( ret_val != DNS_SUCCESS)
         return 0;
      else
         return 1;
 }
 
void GPRS_TestDNS(void)	
{
	    sint ret_val;
    byte key;
    char host_ip[IPLEN];/*16 characters*/
    char loggin_message[32];
    char domain_name[255];
    /*IP address of a certain DNS server on the net used by this demo*/
 /*   char dns_server_ip[]="216.112.42.58";*/
    char dns_server_ip[]="212.57.1.12";/*turk.net*/
 

/*GPRS tran */
	byte buffer2[512];
	usint BufferSize2;
	sint BufferSize;
	
	SOCKET sd;

	/*GPRS INIT */
	GPRS_Init(TRUE);



	                /*clears buffers*/
                memset(domain_name, 0, strlen(domain_name));
                memset(host_ip, 0, sizeof(host_ip));
 
                /*calls a private function to obtain a user domain name input*/
                if (Dialog_Domain(domain_name)== TRUE )
                {
                        /*prints the returned domain name*/
                        Printer_WriteStr((char*)domain_name);
                        Printer_WriteStr("\n");
 
                        /*converts the domain name to an IP address*/
                        ret_val = DNS_Resolver(domain_name,host_ip,dns_server_ip);
 
                        /*displays the return value; and the domain name and IP address upon success*/
                        if (Display_Result(ret_val,domain_name,host_ip))
                        {
                            /*removes the popup latest window*/
                            GrphText_PopDown(TRUE);
 
                            /*clears the screen*/
                            Display_Cls(DSPL1);
 
                            /*message display*/
                            Display_WriteStr(DSPL1,"Press 1 to run  DNS");
 
                            /*waits for user key input*/
                            key = Kb_WaitForKey();
                        }
                }



/*
	Mainloop_DisplayWait("OPENING HOST    CONNECTION 25",1);
	
	

   	if(GPRS_OpenConnection(&sd, host_ip,25) != STAT_OK) {
   		if(GPRS_Init("turkcell") != STAT_OK) 
			Mainloop_DisplayWait("OPEN FAILED     CONNECTION",1);
		Mainloop_DisplayWait("STARTING INIT",1);
   		if(GPRS_OpenConnection(&sd, "216.147.156.77",127) != STAT_OK){
			Mainloop_DisplayWait("INIT FAILED",1);
			exit;
		}
   	}
	EmvIF_ClearDsp(DSP_MERCHANT);
        byte buffer[]={0x00,0x60,0x00,0x01,0x80,0x00,0x04,0x00,0x30,0x20,0x05,
		0x80,0x20,0xC0,0x00,0x04,0x00,0x30,0x00,0x00,0x00,
		0x00,0x00,0x00,0x05,0x00,0x01,0x02,0x00,0x22,0x02,
		0x30,0x00,0x37,0x45,0x56,0x33,0x33,0x44,0x44,0x88,
		0x11,0xD0,0x30,0x41,0x01,0x17,0x52,0x99,0x55,0x00,
		0x00,0x0F,0x30,0x30,0x32,0x39,0x32,0x31,0x37,0x32,
		0x30,0x30,0x30,0x30,0x30,0x30,0x35,0x34,0x38,0x36,
		0x39,0x34,0x30,0x30,0x30,0x00,0x06,0x30,0x30,0x30,
		0x30,0x31,0x32};
	BufferSize=84;


	Mainloop_DisplayWait("SENDING         GPRSDATA",1);
   	if(GPRS_Send(sd,buffer,BufferSize)!= STAT_OK) {
   		Mainloop_DisplayWait("SEND FAILED     GPRSDATA",1);
   		exit;
   	}
   	BufferSize2=512;
	memset(buffer2,0,sizeof(buffer2));
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("RECEIVING       GPRSDATA",1);
	if(GPRS_Receive(sd,buffer2,&BufferSize2)!= STAT_OK)
			Mainloop_DisplayWait("RECEIVE FAILED  GPRSDATA",1);
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("CLOSING HOST    CONNECTION",1);
	
	GPRS_CloseConnection(sd);
	EmvIF_ClearDsp(DSP_MERCHANT);
	*/
}

usint GPRS_GetHostByName(char *anIP, char *aName,char *aDNS)	
{
    sint ret_val;
    byte key;
    char host_ip[IPLEN];/*16 characters*/
    char loggin_message[32];
    char domain_name[255];
    /*IP address of a certain DNS server on the net used by this demo*/
 /*   char dns_server_ip[]="216.112.42.58";*/
    char dns_server_ip[]="212.57.1.12";/*turk.net*/
 

/*GPRS tran */
	byte buffer2[512];
	usint BufferSize2;
	sint BufferSize;
	
	SOCKET sd;

	/*GPRS INIT */
	GPRS_Init(TRUE);
        /*clears buffers*/
         memset(domain_name, 0, strlen(domain_name));
         memset(host_ip, 0, sizeof(host_ip));
 
 
         /*converts the domain name to an IP address*/
        ret_val = DNS_Resolver(aName,anIP,aDNS);
 	if(ret_val == DNS_SUCCESS)
 	{
 		Printer_WriteStr("\naName:");
 		Printer_WriteStr(aName);
 		Printer_WriteStr("\naDNS:");
 		Printer_WriteStr(aDNS);
 		Printer_WriteStr("\nanIP:");
 		Printer_WriteStr(anIP); 	
 		Printer_WriteStr("\n");	
 		return 0;
 	}
 	return 1;
}
/* =================================================================================
 *
 * FUNCTION NAME: TCP_API_Ret
 *
 * DESCRIPTION: private function responsible to display the TCPAPI functions'
 *              return value. The function is called during the TCP/IP session
 *              to display the socket status at each stage.
 *
 * RETURN:     none.
 *
 * NOTES:      none.
 *
 * ================================================================================ */
 void TCP_API_Ret(sint ret_val)
 {
    switch(ret_val)
    {
       case  TCPAPI_SUCCESS:
       {
       	    if (DEBUG_MODE)Printer_WriteStr("TCPAPI_SUCCESS\n");
/*          Display_WriteStr(DSPL1,"TCPAPI_SUCCESS");
            Mte_Wait(1000);*/
            break;
       }
       case TCPAPI_ERROR :
       {
       		if (DEBUG_MODE)Printer_WriteStr("TCPAPI_ERROR\n");
          /*Display_WriteStr(DSPL1,"TCPAPI_ERROR");
            Mte_Wait(1000);*/
            break;
       }
       case TCPAPI_ABORTED:
       {
       	if (DEBUG_MODE)Printer_WriteStr("TCPAPI_ABORTED\n");
          /*Display_WriteStr(DSPL1,"TCPAPI_ABORTED");
            Mte_Wait(1000);*/
            break;
       }
       case  TCPAPI_TIMEOUT :
       {
       	if (DEBUG_MODE)Printer_WriteStr("TCPAPI_TIMEOUT\n");
/*          Display_WriteStr(DSPL1,"TCPAPI_TIMEOUT");
            Mte_Wait(1000);*/
            break;
       }
       case  TCPAPI_NODATA :
       {
       	if (DEBUG_MODE)Printer_WriteStr("TCPAPI_NODATA\n");
/*          Display_WriteStr(DSPL1,"TCPAPI_NODATA");
            Mte_Wait(1000);*/
            break;
       }
       case  TCPAPI_EMSGSIZE :
       {
       	if (DEBUG_MODE)Printer_WriteStr("TCPAPI_EMSGSIZE\n");
/*          Display_WriteStr(DSPL1,"TCPAPI_EMSGSIZE");
            Mte_Wait(1000);*/
            break;
       }
    }
 }
 
 
 
/* ==========================================================================
 *
 * FUNCTION NAME: GPRS_Init
 *
 * DESCRIPTION: Making initialisation for GPRS network
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
#ifdef _GPGPGP
byte GPRS_Init(  char *name,boolean mode){
#else
byte GPRS_Init(boolean mode){
#endif
/*	sint ret_val;
    	char loggin_message[32];
	char host_ip[IPLEN];
*/	
char                   host_ip[IPLEN];          /* of 16 characters */
char                   loggin_message[32];
GSMTargetParamStruct   gsm_struct;
ConnectParams          connect_prms;
lint                   ret_val;
	
	
//	Mainloop_ClearDsp();
/*	Mainloop_DisplayMessage("Init GPRS",0);
	Printer_WriteStr("Init GPRS\n");
	PrntUtil_Cr(2);*/
    	/*the terminal connects to a HOST ISP. The name of the HOST (and its IP
  	* address) must be stored in the terminal's Flash memory. You must build a TCP/IP Block in the
  	* terminal's Flash, before using this function.*/
  	
  		ComHndlr_SetUserParams( 1 , 'T' , '\0' , 0 );

  	
	memset(loggin_message,0,sizeof(loggin_message));  
#ifdef _GPGPGP
	ret_val = ProtMngr_ConnectDestination(name, loggin_message, host_ip);
#else

   memset(&gsm_struct, 0 , sizeof(GSMTargetParamStruct) );
    memset(&connect_prms, 0, sizeof( ConnectParams) );
 
    /* init the GSMTargetParamStruct struct buffer */
//    strcpy(gsm_struct.LineTelephoneNumber ,"Internet"); /* Insert Access Point Name of your own */
//    strcpy(gsm_struct.LineTelephoneNumber ,"akbanktest"); /* Insert Access Point Name of your own */
    strcpy(gsm_struct.LineTelephoneNumber ,APN); /* Insert Access Point Name of your own */
    gsm_struct.Rate = GSM_AUTOBAUD;
    gsm_struct.TimeOut  = 40;
    gsm_struct.NumberOfAttempts = 5;
    gsm_struct.CallType = GSM_CALL_GPRS ;
    gsm_struct.APNSize = 0;
 
   /* gsm_struct.Pedding = ;
      gsm_struct.FirstByteOfAPN =  ;*/
 
 
    /* Init the ConnectParams struct buffer to set the TCP/IP parameters from the application.
     * Note that this example takes all the params fields values from the Application, as set
     * in the ValidFileds member of the Connect structure.
     * However, all or part of the params can be taken from the existing TCP/IP Block that had
     * been set in advance using the SDT and the NCC Toolset (or from the NOS Menu) */
 
//    strcpy(connect_prms.Username,"");  /* taken from the SIM */
//    strcpy(connect_prms.Password,"");  /* taken from the SIM */

//    strcpy(connect_prms.Username,"pos2");  /* taken from the SIM */
//    strcpy(connect_prms.Password,"123456");  /* taken from the SIM */
    strcpy(connect_prms.Username,GPRS_User);  /* taken from the SIM */
    strcpy(connect_prms.Password,GPRS_Passwd);  /* taken from the SIM */
 
    connect_prms.LoginTimeout = 60;
    connect_prms.LoginType = PROTMNGR_LOGIN_EMPTY;
    connect_prms.PppType = PROTMNGR_PPP_STANDART;
    connect_prms.ConnType = PROTMNGR_ICT_GPRS_RADIO;
 
    strcpy(connect_prms.LocalIp,"000.000.000.000");
    strcpy(connect_prms.NetMask,"255.255.255.255");
    strcpy(connect_prms.PriDNSIpAddr,"000.000.000.000");
    strcpy(connect_prms.SecDNSIpAddr,"000.000.000.000");
 
    connect_prms.Physical = &gsm_struct;
 
     /* The following options can be set for the ValidFileds field of the Connect structure:
      * 1) Take only a part of the params:
      *         E.g. take the UserName and password from the application:
      *              connect_prms.ValidFileds = PROTMNGR_USE_USERNAME| PROTMNGR_USE_PASSWORD;
      * 2) Take all of the params from the application (way 1)
      *         E.g. connect_prms.ValidFileds = 0x3FF; */
 
 
    /* Take all of the params from the application (way 2) */
    connect_prms.ValidFileds = PROTMNGR_USE_USERNAME|
                               PROTMNGR_USE_PASSWORD|
                               PROTMNGR_USE_LOGIN_TO|
                               PROTMNGR_USE_LOGIN   |
                               PROTMNGR_USE_PPP     |
                               PROTMNGR_USE_PHYSICAL|
                               PROTMNGR_USE_LOCALIP |
                               PROTMNGR_USE_NETMASK |
                               PROTMNGR_USE_PRIDNS  |
                               PROTMNGR_USE_SECDNS;
 
     /* connect to the ISP, set the TCP/IP params from the application (see above), and
     * return the connection result in ret_val */
      ret_val = ProtMngr_ConnectDestination2(
//                  "akbanktest",              /* use NULL to order that existing HOST name will get used           */
//                  NULL,              /* use NULL to order that existing HOST name will get used           */
                  APN,              /* use NULL to order that existing HOST name will get used           */
                    &loggin_message[0],   /* store the logging message of the connection in this buffer        */
                    &host_ip[0],          /* return the HOST IP here                                           */
                    &connect_prms,        /* take all TCP/IP user params set above                             */
//                    TRUE);                /* TRUE: Blocking function                                           */
                    mode);                /* TRUE: Blocking function                                           */
 
                                          /* FALSE: Non-Blocking function. If FALSE is used, you must call the */
                                          /* ProtMngr_ConnectForegrnd(), after this function is called.        */
 

	
	
#endif	
	
  	switch(ret_val)
 	{
 		case ISP_OK:
 		case ISP_ALREADY_CONNECTED:
			/*Mte_Wait(1000);*/
			if (DEBUG_MODE) Printer_WriteStr("ISP_OK\n");
          		return STAT_OK;
 		case ISP_INPROGRESS: 
			/*Mte_Wait(1000);*/
			if(mode){
				if (DEBUG_MODE)Printer_WriteStr("ISP_INPROGRESS\n");
				lsGPRS_Connect_Param.Connected=FALSE;
				lsGPRS_Connect_Param.tcpavailable=FALSE;
        	  		return STAT_NOK;
        	  	}else return STAT_OK;
              	case ISP_NOT_FOUND:
          	        if (DEBUG_MODE)Printer_WriteStr("ISP or Destination are not found in database");
			break;
          	case  ISP_NOT_CONNECTED: 
          	  	if (DEBUG_MODE)Printer_WriteStr("- ISP is not connected, when trying to disconnect it.");
			break;
          	case  ISP_LOGIN_SCRIPT_ERR:
          	  	if (DEBUG_MODE)Printer_WriteStr(" - syntax error in the login script");
			break;
          	case  ISP_LOGIN_TIMEOUT :
          		if (DEBUG_MODE)Printer_WriteStr("- timeout waiting for a string when logging in");
			break;
          	case  ISP_NO_LINK :
          	  	if (DEBUG_MODE)Printer_WriteStr("- the indicated link protocol cannot be negotiated");
			break;
          	case  ISP_NO_ROUTE:
          	  	if (DEBUG_MODE)Printer_WriteStr(" - cannot find the route to the specified IP address.");
			break;
          	case  ISP_USER_BREAK: 
          	  	if (DEBUG_MODE)Printer_WriteStr("- the user pressed ESC when waiting for logging in");
			break;
          	case  ISP_LINE_CONNECTED: 
          		if (DEBUG_MODE)Printer_WriteStr("- the line is connected");
			break;
          	case  ISP_PPP_OPTIONS_ERR: 
			if (DEBUG_MODE)Printer_WriteStr("- Ppp_SetOptions function failed to set options");
			break;
          	case  ISP_LOGIN_ERR :
          		if (DEBUG_MODE)Printer_WriteStr("- incorrect username and/or password provided.");
			break;
          	case  ISP_INVALID_PARAMS:
          		if (DEBUG_MODE)Printer_WriteStr("invalid parameters");
			break;
          		
		default:
			TCP_API_Ret(ret_val);		
		break;
	}    
	/*disconnects from the ISP*/
	EmvIF_ClearDsp(DSP_MERCHANT);
	Mainloop_DisplayWait("GPRS HATASI",1);
	EmvIF_ClearDsp(DSP_MERCHANT);
	lsGPRS_Connect_Param.Connected=FALSE;
	lsGPRS_Connect_Param.tcpavailable=FALSE;

	ProtMngr_DisconnectDestination(APN);
	return STAT_NOK;
	
}
/* ==========================================================================
 *
 * FUNCTION NAME: GPRS_Disconnect
 *
 * DESCRIPTION: Disconnecting from GPRS network
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
byte GPRS_Disconnect(  char *name){
	ProtMngr_DisconnectDestination(APN);
	lsGPRS_Connect_Param.Connected=FALSE;
	lsGPRS_Connect_Param.tcpavailable=FALSE;

	return STAT_OK;
}

/* ==========================================================================
 *
 * FUNCTION NAME: GPRS_OpenConnection
 *
 * DESCRIPTION: Making connection to the host on GPRS network
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
byte GPRS_OpenConnectionXLS( unsigned char *XLSsd, char *host_ip, usint port){
	SOCKET sd;
	if(GPRS_OpenConnection(&sd,host_ip,port)==STAT_OK)
	{
		*XLSsd=GPRS_SetXLSSocket(sd);
		
	}else return STAT_NOK;
}
/* ==========================================================================
 *
 * FUNCTION NAME: GPRS_OpenConnection
 *
 * DESCRIPTION: Making connection to the host on GPRS network
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
byte GPRS_OpenConnection( SOCKET *sd, char *host_ip, usint port){
	
	SOCKET localSd;
	
	if(GPRS_Init(TRUE)!=STAT_OK) {
		return STAT_NOK;		
	}
	memset(&localSd,0,sizeof(localSd));
     	TCPAPI_SetTimeout(30000); /*receive timeout in milli seconds*/

   	localSd = TCPAPI_Open((char*)host_ip, port, TCP_protocol);
    	
		if (DEBUG_MODE)  {    	
    	Printer_WriteStr("host ip\n");
    	Printer_WriteStr(host_ip);
	PrntUtil_Cr(1);
    	PrntUtil_Int((int)port,"port\n");
	PrntUtil_Cr(3);
	}

     /*	if (localSd == INVALID_SOCKET)*/
     	if(localSd < 0)
     	{
		Display_WriteStr(DSPL1,"SOKET HATASI");
		Mte_Wait(1000);
/*		PrntUtil_Cr(2);	
		Printer_WriteStr("Invalid socket\n");
		PrntUtil_BufAsciiHex((byte*)&localSd,4);
		PrntUtil_Cr(2);*/
		lsGPRS_Connect_Param.tcpavailable=FALSE;
		return STAT_NOK;
     	}
     	else
     	{
/*		if (TCPAPI_GetStatus(localSd) != TCPAPI_ERROR )
		{*/
			*sd=localSd;
		if (DEBUG_MODE)  {    	
			PrntUtil_Cr(2);
		Printer_WriteStr("Connected socket\n");
		PrntUtil_Int((int)localSd,"Soket\n");
			PrntUtil_Cr(2);
		}	
			lsGPRS_Connect_Param.tcpavailable=TRUE;
     			return STAT_OK;
     		/*} else return STAT_NOK;*/
	}
}


/* ==========================================================================
 *
 * FUNCTION NAME: GPRS_CloseConnection
 *
 * DESCRIPTION: Closing connection to the host on GPRS network
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
byte GPRS_CloseConnectionXLS( unsigned char XLSsd){
	SOCKET sd;
	sd = GPRS_GetXLSSocket(XLSsd);
	return(GPRS_CloseConnection(sd));
}
/* ==========================================================================
 *
 * FUNCTION NAME: GPRS_CloseConnection
 *
 * DESCRIPTION: Closing connection to the host on GPRS network
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
byte GPRS_CloseConnection( SOCKET sd){
	/*closes the socket*/
   	TCPAPI_Close(sd);
/*	ProtMngr_DisconnectDestination(APN);*/
	lsGPRS_Connect_Param.tcpavailable=FALSE;
	return STAT_OK;

}
 
 /* =================================================================================
 *
 * FUNCTION NAME: GPRS_SendXLS
 *
 * DESCRIPTION: 
 *              
 * RETURN:     none.
 *
 * NOTES:      none.
 *
 * ================================================================================ */
byte GPRS_SendXLS(unsigned char XLSsd, byte *buffer, sint BufferSize){
	SOCKET sd;
	sd = GPRS_GetXLSSocket(XLSsd);
	return (GPRS_Send(sd,buffer,BufferSize));
}
 /* =================================================================================
 *
 * FUNCTION NAME: GPRS_Send
 *
 * DESCRIPTION: 
 *              
 * RETURN:     none.
 *
 * NOTES:      none.
 *
 * ================================================================================ */
byte GPRS_Send(SOCKET sd, byte *buffer, sint BufferSize){

	sint ret_val;
	
	ret_val = TCPAPI_Send(sd, buffer, BufferSize);
	
         /*if other than success value is returned, breaks*/
        if(ret_val != TCPAPI_SUCCESS){
       			TCP_API_Ret(ret_val);		
        		return STAT_NOK;
        }
/*	if (DEBUG_MODE)
	{
		Printer_WriteStr ("GSM GPRS Send :\n");
		PrntUtil_BufAsciiHex((byte *)buffer,BufferSize);
	}
*/
	return STAT_OK;

}

/* =================================================================================
 *
 * FUNCTION NAME: GPRS_Receive
 *
 * DESCRIPTION: 
 *              
 * RETURN:     none.
 *
 * NOTES:      none.
 *
 * ================================================================================ */
byte GPRS_ReceiveXLS(unsigned char XLSsd, byte *buffer, usint *BufferSize){
	SOCKET sd;
	sd = GPRS_GetXLSSocket(XLSsd);
	return (GPRS_Receive(sd,buffer,BufferSize));
}

 /* =================================================================================
 *
 * FUNCTION NAME: GPRS_Receive
 *
 * DESCRIPTION: 
 *              
 * RETURN:     none.
 *
 * NOTES:      none.
 *
 * ================================================================================ */
byte GPRS_Receive(SOCKET sd, byte *buffer, usint *BufferSize){
	sint ret_val;
		
	/*receives the the sent data*/

	ret_val = TCPAPI_Receive(sd, buffer, BufferSize);

	TCP_API_Ret(ret_val);
	/*if other than success value is returned, breaks*/
	if(ret_val != TCPAPI_SUCCESS)return STAT_NOK;
/*	if (DEBUG_MODE)
	{
		Printer_WriteStr ("GSM GPRS Receive :\n");
		PrntUtil_BufAsciiHex((byte *)buffer,*BufferSize);
	}
*/
	return STAT_OK;
}

 
 /* =================================================================================
 *
 * FUNCTION NAME: GPRS_TCPAPIhost_Send
 *
 * DESCRIPTION: 
 *              
 * RETURN:     none.
 *
 * NOTES:      none.
 *
 * ================================================================================ */
byte GPRS_TCPAPIhost_Send(SOCKET *sd, char *host_ip, byte *buffer, sint BufferSize)
{
	sint ret_val;
	usint size=256, port=127;
	char buf1[32];
	SOCKET localSd;
     
    	localSd = TCPAPI_Open((char*)host_ip, port, TCP_protocol);
    	
     if (localSd == INVALID_SOCKET)
     {
		Display_WriteStr(DSPL1,"INVALID_SOCKET");
		Mte_Wait(1000);
		Printer_WriteStr("INVALID_SOCKET\n");
		return STAT_NOK;
     }
     else
     {
		/*send some data*/
		ret_val = TCPAPI_Send(localSd, (byte*)buffer, BufferSize);
		*sd=localSd;

         /*if other than success value is returned, breaks*/
         if(ret_val != TCPAPI_SUCCESS)	return STAT_NOK;
#if 0
/*writes the sent data in buf1*/
sprintf(buf1,"SENT:%s",buffer);
/*prints buf1, containing the sent data*/
Printer_WriteStr((char*)buf1);
Printer_WriteStr("\n");
#endif

		return STAT_OK;
	}
	return STAT_NOK;
}		
		
 /* =================================================================================
 *
 * FUNCTION NAME: GPRS_TCPAPIhost_Receive
 *
 * DESCRIPTION: 
 *              
 * RETURN:     none.
 *
 * NOTES:      none.
 *
 * ================================================================================ */
byte GPRS_TCPAPIhost_Receive(SOCKET sd, char *host_ip, char *buffer, usint *BufferSize)
{		
	sint ret_val;
	char buf1[32];
		
	/*receives the the sent data*/
	ret_val = TCPAPI_Receive(sd, (byte*)buffer, BufferSize);
	
TCP_API_Ret(ret_val);

	Mte_Wait(1000);
   	/*closes the socket*/
   	TCPAPI_Close(sd);
	ProtMngr_DisconnectDestination(APN);

	/*if other than success value is returned, breaks*/
	if(ret_val != TCPAPI_SUCCESS)return STAT_NOK;
	return STAT_OK;
}

byte GPRS_SetTimeout(  lint to)
{
	TCPAPI_SetTimeout(to);
	return TRUE;
}
/* ==========================================================================
 *
 * FUNCTION NAME: GPRS_Set
 *
 * DESCRIPTION: function main is responsible for all the tasks listed in the
 *              introduction to this demo.
 *
 * RETURN:        none.
 *
 * NOTES:         none.
 *
 * ========================================================================== */
byte GPRS_Set(  char *host_ip)
{
    sint ret_val;
    char loggin_message[32];

    /*the terminal connects to a HOST ISP. The name of the HOST (and its IP
  	* address) must be stored in the terminal's Flash memory. You must build a TCP/IP Block in the
  	* terminal's Flash, before using this function.*/
  	
	memset(loggin_message,0,sizeof(loggin_message));  
	strcpy(loggin_message,APN);
	ret_val = ProtMngr_ConnectDestination(ISP_NAME, loggin_message, host_ip);
	
  	switch(ret_val)
 	{
 		case ISP_OK:
 		case ISP_ALREADY_CONNECTED:
			Mte_Wait(1000);
          	return STAT_OK;
/*           case ISP_NOT_FOUND :
           	Display_WriteStr(DSPL1,"ISP_NOT_FOUND");
           	break;
           case ISP_LOGIN_SCRIPT_ERR: 
           	Display_WriteStr(DSPL1,"ISP_LOGIN_SCRIPT_ERR");
           	break;
		case ISP_LOGIN_TIMEOUT:
			Display_WriteStr(DSPL1,"ISP_LOGIN_TIMEOUT");
			break;
		case ISP_NO_LINK :
			Display_WriteStr(DSPL1,"ISP_NO_LINK");
			break;
		case ISP_NO_ROUTE:
			Display_WriteStr(DSPL1,"ISP_NO_ROUTE");
			break;
		case ISP_USER_BREAK: 
			Display_WriteStr(DSPL1,"ISP_USER_BREAK");
			break;
		case ISP_NOT_CONNECTED :*/
		default:
			TCP_API_Ret(ret_val);		
		break;
			/*if ProtMngr_ConnectDestination attempt fails, displays a message and disconnects from the ISP*/
/*			Display_WriteStr(DSPL1,"ISP_NOT_CONNECTED");*/
	}    
	/*disconnects from the ISP*/
	Mte_Wait(1000);
	ProtMngr_DisconnectDestination(APN);
	return STAT_NOK;
}
#if 0
 /* =================================================================================
 *
 * FUNCTION NAME: Dialog_Domain
 * DESCRIPTION: private function responsible to obtain the domain name from the user
 * RETURN: TRUE - the user pressed <ENTER> to input the domain name
 *         FALSE - the user pressed <ESC>
 *
 * NOTES:      none.
 *
 * ================================================================================ */
boolean Dialog_Domain(char *domain_name)
{
	dialoge input;
	byte key;
	/*initializes the dialoge buffer declared above*/
	input.header       = (char *)"INSERT DOMAIN:";
	input.format       = (void *)domain_name;
	input.format_flags = __STRING;
	input.length       = 255;
	input.return_value = 0;
	
Printer_WriteStr("input\n");
PrntUtil_BufAsciiHex((byte*)&input, 2*sizeof(dialoge));
Printer_WriteStr("\n");
	
	/*enables the SoftKB module, draws the SoftKB keys and enables the Touch Panel*/
	SoftKB_Enable();
	ExtendKb_Enable();
	do
	{	
		/*runs the dialog*/
		key = Formater_DialogeBox((void *)&input);
		
Printer_WriteStr("key=\n");
PrntUtil_BufAsciiHex((byte*)&key, 1);

Printer_WriteStr("input\n");
PrntUtil_BufAsciiHex((byte*)&input, sizeof(dialoge));
Printer_WriteStr("\n");

		if (key == ENTER || key == MENU)break;
	}while(key != ENTER && key != MENU);
	
	
	SoftKB_Disable();
	ExtendKb_Disable();
	/*if the last key the user pressed is <ENTER> returns TRUE, else returns FALSE*/
	if (key != ENTER)
		/*disables the software keyboard*/
		return FALSE;
	else	
		/*disables the software keyboard*/
		return TRUE;
 }
 
#endif