/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef EMULATOR
#define EMULATOR ((defined(__WINS__) || defined(__WINSCW__)))
#endif

#if EMULATOR
#include <sys/types.h>

typedef struct TLibcryptWsd
{
	/* encrypt.cpp desKey */
	unsigned char desKey[64];				// For persistence between calls 
											// to setkey() and encrypt()
	int bSetkeyInvoked;
	/* encrypt.cpp desKey */
	
	/* crypt_md5() crypt-md5.c */
	const char *sp, *ep;
	char passwd[120], *p;
	/* crypt_md5() crypt-md5.c */
	
	/* crypt-des.c */
	u_int32_t	bits32[32];		/* Initialialize this array */
	
	u_char	inv_key_perm[64];   /* No specific initial values. However, initialize
								 * the array to zero.
								 */
								 
	u_char	inv_comp_perm[56];	/* No specific initial values. However, initialize
								 * the array to zero.
								 */
	u_char	u_sbox[8][64];
	
	u_char	un_pbox[32];
	
	u_int32_t	saltbits;
	u_int32_t	old_salt;
	u_int32_t	*bits28, *bits24;
	u_char		init_perm[64], final_perm[64];
	u_int32_t	en_keysl[16], en_keysr[16];
	u_int32_t	de_keysl[16], de_keysr[16];
	int		des_initialised; /*  = 0 ... inportant */
	
	u_char      **m_sbox;   		// [4][4096]      bytes of dynamic storage
	__uint32_t  **psbox;			// ([4][256] * 4) bytes of dynamic storage

	__uint32_t	**ip_maskl,		    // ([8][256] * 4) bytes of dynamic storage
			    **ip_maskr;    		// ([8][256] * 4) bytes of dynamic storage

	__uint32_t	**fp_maskl,			// ([8][256] * 4) bytes of dynamic storage
			    **fp_maskr;			// ([8][256] * 4) bytes of dynamic storage

	__uint32_t	**key_perm_maskl,	// ([8][128] * 4) bytes of dynamic storage
			    **key_perm_maskr;	// ([8][128] * 4) bytes of dynamic storage

	__uint32_t	**comp_maskl,		// ([8][128] * 4) bytes of dynamic storage
			    **comp_maskr;		// ([8][128] * 4) bytes of dynamic storage

	u_int32_t	old_rawkey0, old_rawkey1;	/* initialize this to zero */
	
	char	output[21];
	/* crypt-des.c */
	
}TLibcryptWsd;

#ifdef __cplusplus
extern "C" TLibcryptWsd *GetGlobals();
#else
TLibcryptWsd *GetGlobals();
#endif	/* End __cplusplus */

#endif
