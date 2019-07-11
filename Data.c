/*-----------------------------------------------------------------------------*
* Copyright High Co Technologie
* File Name    	 : DATA.C
* Created      	 : 19/04/2001
* Author       	 : O.RICHARD
* Library      	 : 
* Portability  	 : LIPMAN
*
* Purpose          : 
*
* Revision history :
*	Date 		Author		Description
*  xx/xx/2001
*-----------------------------------------------------------------------------*/

/*............. HEADER FILES ..............................................*/


#define DEF_TPEFILE
#include <inc_term.h>
#include <confapp.h>
#include <inc_xls.h>

char L1[TR_LG];
char L2[TR_LG];

#define DEF_TCPPARAM
#include <tcpparam.h>

#define DEF_TCPIPXLS
#include <tcpipxls.h>

#define DEF_CONFFILE
#include <conffile.h>

#define DEF_XLSFAT
#include <xlsfat.h>
#include <xcfgfile.h>

#define DEF_LOYLIB
#include <inc_loy.h>

#define DEF_CONSTCRD
#include <constcrd.h>

#define DEF_VERSION
#include <version.h>

#ifdef MESURE_TEMPS
	#define DEF_MTEMPS
	#include <mtemps.h>
#endif

/*------------------------- End of file DATA.C ------------------------*/
