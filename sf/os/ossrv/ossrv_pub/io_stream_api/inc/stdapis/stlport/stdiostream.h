/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/

#ifndef _STLP_misc_stdiostream_h
# define _STLP_misc_stdiostream_h
# if (__SUNPRO_CC >= 0x500 )
#  include <../CCios/stdiostream.h>
# elif defined (__SUNPRO_CC)
#  include <../CC/stdiostream.h>
# else
#  warning "This file is for SUN CC only. Please remove it if it causes any harm for other compilers."
# endif
#endif
