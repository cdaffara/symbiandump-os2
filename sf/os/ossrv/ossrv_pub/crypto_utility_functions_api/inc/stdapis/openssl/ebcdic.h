/* crypto/ebcdic.h */
/*
 © Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 */

#ifndef HEADER_EBCDIC_H
#define HEADER_EBCDIC_H

#if (defined(__SYMBIAN32__) && !defined(SYMBIAN))
#define SYMBIAN
#endif

#ifdef SYMBIAN
#include <e32def.h>
#endif
#include <sys/types.h>

/* Avoid name clashes with other applications */
#define os_toascii   _openssl_os_toascii
#define os_toebcdic  _openssl_os_toebcdic
#define ebcdic2ascii _openssl_ebcdic2ascii
#define ascii2ebcdic _openssl_ascii2ebcdic

IMPORT_C extern const unsigned char os_toascii[256];
IMPORT_C extern const unsigned char os_toebcdic[256];
IMPORT_C void *ebcdic2ascii(void *dest, const void *srce, size_t count);
IMPORT_C void *ascii2ebcdic(void *dest, const void *srce, size_t count);

#endif
