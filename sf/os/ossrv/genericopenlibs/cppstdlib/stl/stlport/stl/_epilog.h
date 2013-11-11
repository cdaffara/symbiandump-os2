/* NOTE : this header has no guards and is MEANT for multiple inclusion!
 * If you are using "header protection" option with your compiler,
 * please also find #pragma which disables it and put it here, to
 * allow reentrancy of this header.
 */

#ifndef _STLP_PROLOG_HEADER_INCLUDED
#  error STLport epilog header can not be included as long as prolog has not be included.
#endif

/* If the platform provides any specific epilog actions,
 * like #pragmas, do include platform-specific prolog file
 */
#if defined (_STLP_HAS_SPECIFIC_PROLOG_EPILOG)
#  include <stl/config/_epilog.h>
#endif

#if !defined (_STLP_NO_POST_COMPATIBLE_SECTION)
#  include <stl/_config_compat_post.h>
#endif

#if defined (_STLP_USE_OWN_NAMESPACE)

#  if !defined (_STLP_DONT_REDEFINE_STD)
/*  We redefine "std" to STLPORT, so that user code may use std:: transparently
 *  The STLPORT macro contains the STLport namespace name containing all the std
 *  stuff.
 */
#    if defined (std)
/*
 * Looks like the compiler native library on which STLport rely defined the std macro.
 * This might introduce major incompatibility so report the problem to the STLport
 * forum or comment the following #error at your own risk.
 */
#      error Incompatible native Std library.
#    endif /* std */
#    define std STLPORT
#  endif /* _STLP_DONT_REDEFINE_STD */

#endif

#undef _STLP_PROLOG_HEADER_INCLUDED /* defined in _prolog.h */

/*Below changes are added to make _STLP_DECLSPEC as _STLP_EXPORT_DECLSPEC at definition of the functions.
In _prolog.h, _STLP_DECLSPEC will be defined as _STLP_IMPORT_DECLSPEC.
*/
#  if defined (_STLP_DESIGNATED_DLL) 
#    if defined (_STLP_DECLSPEC)
#	undef _STLP_DECLSPEC
#    endif
#    define  _STLP_DECLSPEC        _STLP_EXPORT_DECLSPEC   
#    if defined (_STLP_CLASS_DECLSPEC)
#	undef _STLP_CLASS_DECLSPEC
#    endif
#    define  _STLP_CLASS_DECLSPEC  _STLP_CLASS_EXPORT_DECLSPEC
#  endif
