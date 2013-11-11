/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
 


#include "liboil_wsd_solutions.h" 

#ifdef EMULATOR

#include <e32std.h> 
#include <pls.h> // For emulator WSD API 

const TUid KLibOilUid3 = {0x2001101E};     // This is the UID of the library

extern "C"  	struct liboil_global_struct *liboil_ImpurePtr()
{
	  
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct liboil_global_struct* p  = Pls<struct liboil_global_struct>(KLibOilUid3, &liboil_Init);
  	return p;

#else

	return NULL;	

#endif

	  
	  
}

int liboil_Init(liboil_global_struct *g)
{
#if defined(__WINSCW__) || defined(__WINS__)
	if(g)
	{
    	//g->GET_GSTREAMER_WSD_VAR_NAME(buffers,gstcheck,g) = NULL;
	    g->GET_OIL_WSD_VAR_NAME(_class,abs_f32_f32,g).func = NULL;
	    g->GET_OIL_WSD_VAR_NAME(_class,abs_f32_f32,g).name = "abs_f32_f32";    
	    g->GET_OIL_WSD_VAR_NAME(_class,abs_f32_f32,g).desc = NULL;    
	    g->GET_OIL_WSD_VAR_NAME(_class,abs_f32_f32,g).test_func = NULL;    
	    g->GET_OIL_WSD_VAR_NAME(_class,abs_f32_f32,g).first_impl = NULL;    
	    g->GET_OIL_WSD_VAR_NAME(_class,abs_f32_f32,g).reference_impl = NULL;    
	    g->GET_OIL_WSD_VAR_NAME(_class,abs_f32_f32,g).prototype = "uint8_t *dest, int dstr, int8_t *src, int sstr, int n" ;
	           
		return 0;
	  }
	  return 1;
#else

	return 1;

#endif		  
}

extern "C"   void _liboil_wsd_reset()
{
	struct liboil_global_struct* p  = Pls<struct liboil_global_struct>(KLibOilUid3, &liboil_Init);	
	if(p)
	{
		memset(p,0,sizeof(struct liboil_global_struct));
		liboil_Init(p);
	}
}

#endif // EMULATOR


