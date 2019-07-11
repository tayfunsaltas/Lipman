/******************************************************************************/
/*                  A P P L I C A T I O N     H E A D E R                     */
/******************************************************************************/

/*#include "config.h"*/
#include "project.def"

#include NURIT_DEF
#include DLMLINKR_DEF
#include APPLHEAD_DEF

extern void static_data_size(void);


const application_header id =
{
                APPL_SIGNATURE,                     /* Signature              */
				((ulint)static_data_size),          /* End of Static data     */
				{'0','7','.','0','7'},	            /* minimum NOS Version    */	
                {"Version: 07.01"},         /* Appl. Description      */
				{'0','7','.','0','1'},	            /* Application Version    */
				{"AKB8000"},                       /* Application Name       */
				{"17/12/04"},                       /* Date */
				{0},			                    /* New Format Header Equ to 0 */
				{0},	  		                    /* Number of used dlms */
				 0                                  /* Pointer to Dlms name table */
};



