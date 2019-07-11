/*-----------------------------------------------------------------------------*
* Copyright High Co Technologie
* File Name    	 : MENU.C
* Created      	 : 04/05/2001
* Author       	 : O.RICHARD
* Library      	 : 
* Portability  	 : LIPMAN
*
* Purpose          : Initializes parameter "Fidelité encapsulée" and runs LOY_Menu
*
* Revision history :
*	Date 		Author		Description
*  xx/xx/2001
*-----------------------------------------------------------------------------*/

/*............. HEADER FILES ..............................................*/

/* WRT 06/08/2001 Related Variables olivier */
/*#define DEF_INTERFAC*/

/* General Header File */
/*#include "config.h"*/
#include "project.def"        /* yeni */

#include INTERFAC_H
#include <inc_xls.h>
#include <inc_term.h>
#include <io_xls.h>

#define DEF_MENU
#include <menu.h>

#include <loymenu.h>


/**************************************************************
*
*  Function name : Menu
*
*  Author        : O.RICHARD
*
*  Purpose       : Initializes parameter "Fidelité encapsulée" and runs LOY_Menu
*
*  Input         : - inChoix : User choice
*
*  Output        : 
*
*  Return        : 
*
***************************************************************/

void Menu (int inChoix)
{
	byte	fidelite_encapsulee;

	switch (inChoix)
	{
		case MNU_TRANSACTION : 
#ifdef E_COUPON
		case MNU_COUPON :
#endif
		
#ifdef SHOWPACK
									fidelite_encapsulee = TRUE;
#else	
									fidelite_encapsulee = FALSE;
#endif	
									WriteETAT_FIDELITE_ENCAPSULEE(&fidelite_encapsulee);
									break;


		case MNU_ADMINISTRATION :	fidelite_encapsulee = FALSE;

									WriteETAT_FIDELITE_ENCAPSULEE(&fidelite_encapsulee);
									break;


		default:					break;
	}

	LOY_Menu (inChoix);
}

/*-------------------------- End of Function Menu -------------------------*/
 
/*-------------------------- End of File MENU.C --------------------------*/