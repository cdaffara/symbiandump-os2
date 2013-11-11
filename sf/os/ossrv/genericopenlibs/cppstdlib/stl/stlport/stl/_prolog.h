/* NOTE : this header has no guards and is MEANT for multiple inclusion!
 * If you are using "header protection" option with your compiler,
 * please also find #pragma which disables it and put it here, to
 * allow reentrancy of this header.
 */

#ifdef std
#  undef std /* We undef "std" on entry , as STLport headers may include native ones. */
#endif

#ifdef _STLP_PROLOG_HEADER_INCLUDED
#  error STlport prolog header can not be reincluded as long as epilog has not be included.
#endif

#define _STLP_PROLOG_HEADER_INCLUDED

#ifndef _STLP_FEATURES_H
#  include <stl/config/features.h>
#endif

/* If the platform provides any specific prolog actions,
 * like #pragmas, do include platform-specific prolog file */
#if defined (_STLP_HAS_SPECIFIC_PROLOG_EPILOG)
#  include <stl/config/_prolog.h>
#endif

#  if defined (_STLP_DESIGNATED_DLL) /* This is a lib which will contain STLport exports */
#    if defined (_STLP_DECLSPEC)
#	undef _STLP_DECLSPEC
#    endif
#      if (__ARMCC_VERSION >= 220435 && __ARMCC_VERSION < 230000)
#        define  _STLP_DECLSPEC        _STLP_EXPORT_DECLSPEC   //RVCT 2.2 requires __declspec(dllexport) at declaration!
#      else
#        define  _STLP_DECLSPEC        _STLP_IMPORT_DECLSPEC   //RVCT 3.1 requires __declspec(dllimport) at declaration.
#      endif
#    if defined (_STLP_CLASS_DECLSPEC)
#	undef _STLP_CLASS_DECLSPEC
#    endif
#    define  _STLP_CLASS_DECLSPEC  _STLP_CLASS_IMPORT_DECLSPEC
#  endif
