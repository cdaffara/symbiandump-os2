/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/*
 * Purpose of this file :
 *
 * To hold COMPILER-SPECIFIC portion of STLport settings.
 * In general, user should not edit this file unless 
 * using the compiler not recognized below.
 *
 * If your compiler is not being recognized yet, 
 * please look for definitions of macros in stl_mycomp.h,
 * copy stl_mycomp.h to stl_YOUR_COMPILER_NAME, 
 * adjust flags for your compiler, and add  <include config/stl_YOUR_COMPILER_NAME>
 * to the secton controlled by unique macro defined internaly by your compiler.
 *
 * To change user-definable settings, please edit <../stl_user_config.h> 
 *
 */

#ifndef _STLP_COMP_H
# define _STLP_COMP_H

#include <_ansi.h>
#ifdef __SYMBIAN32__
/*
#ifndef __NO_THROW
#define __NO_THROW throw()
#endif //__NO_THROW
#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif
*/
#else
#   define EXPORT_C
#   define IMPORT_C
#endif



#if 0
#ifdef __SYMBIAN32__
#warning __SYMBIAN32__
#endif

#ifdef EKA2
#warning EKA2
#endif

#ifdef __WINSCW__
#warning __WINSCW__
#endif 


#ifdef __MWERKS__
#warning __MWERKS__
#endif

#ifdef __CC_ARM
#warning __CC_ARM
#endif

#ifdef __ARMCC__
#warning __ARMCC__
#endif
#endif


#  define __GIVE_UP_WITH_STL(message) void give_up() \
   { upgrade_the_compiler_to_use_STL;}

/* distinguish real MSC from Metrowerks and Intel */
# if defined(_MSC_VER) && !defined(__MWERKS__) && !defined (__ICL) && !defined (__COMO__)
#  define _STLP_MSVC _MSC_VER
# endif

# if defined (__xlC__)  || defined (__IBMC__) || defined ( __IBMCPP__ ) 
/* AIX xlC, Visual Age C++ , OS-390 C++ */
#  include <config/stl_ibm.h>
# elif defined (__INTEL_COMPILER) && defined(__unix__)
/* Check intel before gcc, since newer versions define GNUC */
#  include <config/stl_icc.h>
# elif !defined(__SYMBIAN32__) && defined (__GNUC__ )
#  include <config/stl_gcc.h>
# elif defined (__KCC)
#  include <config/stl_kai.h>
# elif defined(__sgi)
#  include <config/stl_sgi.h>
# elif (defined(__OS400__))
/* AS/400 C++ */
#  include <config/stl_as400.h>
# elif defined(_STLP_MSVC)
/* Microsoft Visual C++ 4.0, 4.1, 4.2, 5.0 */
#  include <config/stl_msvc.h>
# elif defined ( __BORLANDC__ )
/* Borland C++ ( 4.x - 5.x ) */
#  include <config/stl_bc.h>
# elif defined(__SUNPRO_CC) || defined (__SUNPRO_C)
/* SUN CC 4.0.1-5.0  */
#  include <config/stl_sunpro.h>
# elif defined (__WATCOM_CPLUSPLUS__) || defined (__WATCOMC__)
/* Watcom C++ */
#  include <config/stl_watcom.h>
# elif defined(__COMO__) || defined (__COMO_VERSION_)
#  include <config/stl_como.h>
# elif defined (__DMC__)
/* Digital Mars C++ */
#  include <config/stl_dm.h>
# elif defined (__SC__) && (__SC__ < 0x800)
/* Symantec 7.5 */
#  include <config/stl_symantec.h>
# elif defined (__MRC__) || (defined (__SC__) && (__SC__ >= 0x882))
/* Apple MPW SCpp 8.8.2  
 * Apple MPW MrCpp 4.1.0 */
#  include <config/stl_apple.h>
# elif defined(__SYMBIAN32__) && defined(EKA2) && defined (__GCCE__)
//#warning ***************USING GCCE COMPILER **************
#   include <config/stl_epoc.h>
#   include <config/_gcc.h>
#   include <config/stl_gcce.h>
//#  warning *****************************************
# elif defined(__SYMBIAN32__) && defined(EKA2) && defined (__WINSCW__)
/* Metrowerks CodeWarrior for Symbian EKA2 */
//#  warning ********** USING METROWERKS COMPILER **********
# include <config/stl_epoc.h>
#  include <config/stl_winscw.h>
//#  warning *****************************************
#elif defined (__SYMBIAN32__) && defined(EKA2) && defined(__ARMCC__)
/* ARM RVCT for Symbian EKA2 */
//#  warning ********** USING RVCT COMPILER **********
# include <config/stl_epoc.h>
#  include <config/stl_rvct.h>
//#  warning *****************************************
# elif defined(__SYMBIAN32__) && defined(EKA2) && defined (__GCCXML__)
//#warning ***************USING GCCXML COMPILER **************
#   include <config/stl_epoc.h>
#   include <config/_gcc.h>
#   include <config/stl_gccxml.h>
//#  warning *****************************************
#elif defined (__SYMBIAN32__)

/* NO-OP: This is just for the Symbian build process, to silence
   the warning that no compiler config file is available during
   the include dependency check step.  The warning is generated
   because the preprocesser is being used to process the files
   with no compiler flag set.
*/

# elif defined(__hpux)
/* HP compilers */
#  include <config/stl_hpacc.h>
# elif defined(__ICL)
/* Intel reference compiler for Win */
#  include <config/stl_intel.h>
/* SCO UDK 7 compiler (UnixWare 7x, OSR 5, UnixWare 2x) */
# elif defined(__USLC__)
#  include <config/stl_sco.h>
/* Apogee 4.x */
# elif defined (__APOGEE__)
#  include <config/stl_apcc.h>
# elif defined (__DECCXX) || defined (__DECC)
#  ifdef __vms
#    include <config/stl_dec_vms.h>
#  else
#    include <config/stl_dec.h>
#  endif
# elif defined (__ISCPP__)
#  include <config/stl_is.h>
# elif defined (__FCC_VERSION)
/* Fujutsu Compiler, v4.0 assumed */
#  include <config/stl_fujitsu.h>
# elif defined(_CRAY)
/* Cray C++ 3.4 or 3.5 */
#  include <config/stl_cray.h>
# else
/* Unable to identify the compiler, issue error diagnostic.
 * Edit <config/stl_mycomp.h> to set STLport up for your compiler. */
#  include <config/stl_mycomp.h>
# endif /* end of compiler choice */
# undef __GIVE_UP_WITH_STL
#endif

