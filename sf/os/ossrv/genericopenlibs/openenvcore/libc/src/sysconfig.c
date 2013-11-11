/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


// connectors for re-entrant system calls
 
#include <sys/param.h>
#include <sys/time.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <sys/socket.h>

#include <errno.h>
#include <limits.h>
#include <paths.h>
#include <pthread.h>		/* we just need the limits */
#include <time.h>
#include <unistd.h>

 
EXPORT_C long
sysconf(int name)
	
{
	int retval = 0 ;

	switch (name) 
	{
		case -1:
			{
			retval = -1 ;
			errno = EINVAL ;
			break ;
			}
	    case _SC_ARG_MAX:
	    	{
	    	retval =   20 ; //KMaxArgC ; 
		    break;
	    	}
	
	    case _SC_CLK_TCK:
	       	{
	       	retval = CLK_TCK ;
		    break ;
	       	}
		
		case _SC_OPEN_MAX:
			{
			retval =  OPEN_MAX ;
		    break ;
			}
		   
		case _SC_JOB_CONTROL :
			{
			retval =  1 ;
		    break ;
			}

	
	    case _SC_IOV_MAX :
	    	{
	    	 retval = IOV_MAX ;
	    	 break ;
	    	}
	       
		case _SC_SAVED_IDS  :
			{
			retval =  -1 ;
		    break ;
			}
			
	    case _SC_NGROUPS_MAX :           //Suplimentary groups not supported    
        case _SC_CHILD_MAX	:            //Needs imp of getrlimit
        	{
        	errno  = ENOSYS ;
            retval =  -1 ;
            break ;
        	}
          
		case _SC_2_C_DEV :   //c99, yac lex utilities are not there
		case _SC_2_C_BIND :
		case _SC_2_FORT_DEV: //  FORTAN development utilities not supported
		case _SC_2_FORT_RUN :// FORTAN Run time utilities not supported
		case _SC_2_UPE :  //User portability not supported
		case _SC_2_CHAR_TERM : //as UPE is not supported
		case _SC_2_SW_DEV:    //utilities such as nm and strip are not supported
		case _SC_2_LOCALEDEF ://No support for supports  Locale Creation Utilities option
			{
			retval = -1 ;
			break ;
			}
		
		case _SC_PAGESIZE :
			{
			 retval = getpagesize() ;
			 break ;
			}
	
        default :
        	{
        	errno = ENOSYS ;
            retval =  -1 ;
            break ;
        	}
	}
	
	return retval ;
}
