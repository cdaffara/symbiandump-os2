/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Methods that allows to encrypt and decrypt data.
*
*/


/* A wrapper for gcrypt.h */

#ifndef __SYMBIANCRYPTO_CRYPTO_WRAPPER_H__
#define __SYMBIANCRYPTO_CRYPTO_WRAPPER_H__    

#include <e32def.h>
#include "xmlsecc_config.h"

#include "xmlsecc_evpwrapper.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 


/************************************
 *                                  
 *   symmetric cipher functions     
 *                                  
 ************************************/

/* Port SC_CIPHER_HD */

#ifndef _SC_GCC_ATTR_DEPRECATED
#define _SC_GCC_ATTR_DEPRECATED
#endif

struct sc_cipher_handle;
typedef struct sc_cipher_handle *sc_cipher_hd_t;
typedef struct sc_cipher_handle *SC_CIPHER_HD _SC_GCC_ATTR_DEPRECATED;
typedef struct sc_cipher_handle *ScCipherHd _SC_GCC_ATTR_DEPRECATED;
typedef sc_cipher_hd_t 	ScCipherHd;

#define	ISLAST	1
#define NOTLAST	0

/* data type compatibility */
typedef unsigned char 			byte;


/* Module specification structure for ciphers.  */
typedef struct sc_cipher_spec
{
  const char *name;
  const char **aliases;
  size_t blocksize;
  size_t keylen;
  size_t contextsize;
} sc_cipher_spec_t;

/* All symmetric encryption algorithms are identified by their IDs.
   More IDs may be registered at runtime. */
enum sc_cipher_algos
  {
    SC_CIPHER_NONE        = 0,
    SC_CIPHER_IDEA        = 1,
    SC_CIPHER_3DES        = 2,
    SC_CIPHER_CAST5       = 3,
    SC_CIPHER_BLOWFISH    = 4,
    SC_CIPHER_SAFER_SK128 = 5,
    SC_CIPHER_DES_SK      = 6,
    SC_CIPHER_AES128      = 7,
    SC_CIPHER_AES192      = 8,
    SC_CIPHER_AES256      = 9,
    SC_CIPHER_TWOFISH     = 10,

    /* Other cipher numbers are above 300 for OpenPGP reasons. */
    SC_CIPHER_ARCFOUR     = 301,  /* Fully compatible with RSA's RC4 (tm). */
    SC_CIPHER_DES         = 302,  /* Yes, this is single key 56 bit DES. */
    SC_CIPHER_TWOFISH128  = 303,
    SC_CIPHER_SERPENT128  = 304,
    SC_CIPHER_SERPENT192  = 305,
    SC_CIPHER_SERPENT256  = 306,
    SC_CIPHER_RFC2268_40  = 307,  /* Ron's Cipher 2 (40 bit). */
    SC_CIPHER_RFC2268_128 = 308   /* Ron's Cipher 2 (128 bit). */
  };

/* The supported encryption modes.  Note that not all of them are
   supported for each algorithm. */
enum sc_cipher_modes 
  {
    SC_CIPHER_MODE_NONE   = 0,  /* Not yet specified. */
    SC_CIPHER_MODE_ECB    = 1,  /* Electronic codebook. */
    SC_CIPHER_MODE_CFB    = 2,  /* Cipher feedback. */
    SC_CIPHER_MODE_CBC    = 3,  /* Cipher block chaining. */
    SC_CIPHER_MODE_STREAM = 4,  /* Used with stream ciphers. */
    SC_CIPHER_MODE_OFB    = 5,  /* Outer feedback. */
    SC_CIPHER_MODE_CTR    = 6   /* Counter. */
  };

/* Flags used with the open function. */ 
enum sc_cipher_flags
  {
    SC_CIPHER_SECURE      = 1,  /* Allocate in secure memory. */
    SC_CIPHER_ENABLE_SYNC = 2,  /* Enable CFB sync mode. */
    SC_CIPHER_CBC_CTS     = 4,  /* Enable CBC cipher text stealing (CTS). */
    SC_CIPHER_CBC_MAC     = 8   /* Enable CBC message auth. code (MAC). */
  };

/* To avoid that a compiler optimizes certain memset calls away, these
   macros may be used instead. */
#define wipememory2(_ptr,_set,_len) do { \
              volatile char *_vptr=(volatile char *)(_ptr); \
              size_t _vlen=(_len); \
              while(_vlen) { *_vptr=(_set); _vptr++; _vlen--; } \
                  } while(0)
#define wipememory(_ptr,_len) wipememory2(_ptr,0,_len)



/************************************
 *                                  
 *   random generating functions    
 *                                  
 ************************************/

/* The possible values for the random quality.  The rule of thumb is
   to use STRONG for session keys and VERY_STRONG for key material.
   WEAK is currently an alias for STRONG and should not be used
   anymore - use sc_create_nonce instead. */
typedef enum sc_random_level
  {
    SC_WEAK_RANDOM = 0,
    SC_STRONG_RANDOM = 1,
    SC_VERY_STRONG_RANDOM = 2
  }
sc_random_level_t;

/* Fill BUFFER with LENGTH bytes of random, using random numbers of
   quality LEVEL. */
TInt sc_randomize (unsigned char *buffer, size_t length,
                     enum sc_random_level level);

/* Retrieve the block length used with algorithm A. */
size_t sc_cipher_get_algo_blklen (int algo);

/* Set the IV to be used for the encryption context C to IV with
   length IVLEN.  The length should match the required length. */
int sc_cipher_setiv ( sc_cipher_hd_t c, const byte *iv, unsigned int ivlen );

/* Encrypt the plaintext of size INLEN in IN using the cipher handle H
   into the buffer OUT which has an allocated length of OUTSIZE.  For
   most algorithms it is possible to pass NULL for in and 0 for INLEN
   and do a in-place decryption of the data provided in OUT.  */
int sc_cipher_encrypt (sc_cipher_hd_t h,
                                  void *out, size_t outsize,
                                  const void *in, size_t inlen, int last);


/* The counterpart to sc_cipher_encrypt.  */
int sc_cipher_decrypt (sc_cipher_hd_t h,
                                  void *out, size_t outsize,
                                  const void *in, size_t inlen, int *outlen, int last);

/* Create a handle for algorithm ALGO to be used in MODE.  FLAGS may
   be given as an bitwise OR of the sc_cipher_flags values. */
int sc_cipher_open (sc_cipher_hd_t *handle,
                              int algo, int mode, unsigned int flags);


/* Close the cioher handle H and release all resource. */
void sc_cipher_close (sc_cipher_hd_t h);

/* Retrieved the key length used with algorithm A. */
size_t sc_cipher_get_algo_keylen (int algo);

/* Set the key to be used for the encryption context C to KEY with
   length KEYLEN.  The length should match the required length. */
int
sc_cipher_setkey (sc_cipher_hd_t c, byte *key, unsigned int keylen);

/* Set specification blocksize in context */
void set_ctx_blocksize(sc_cipher_hd_t c, size_t bklen);


/************************************
 *                                  
 *   cryptograhic hash functions    
 *                                  
 ************************************/

#define XMLSEC_SYMBIAN_MAX_DIGEST_SIZE		32

/* Algorithm IDs for the hash functions we know about. Not all of them
   are implemnted. */
enum sc_md_algos
  {
    SC_MD_NONE    = 0,  
    SC_MD_MD5     = 1,
    SC_MD_SHA1    = 2,
    SC_MD_RMD160  = 3,
    SC_MD_MD2     = 5,
    SC_MD_TIGER   = 6,   /* TIGER/192. */
    SC_MD_HAVAL   = 7,   /* HAVAL, 5 pass, 160 bit. */
    SC_MD_SHA256  = 8,
    SC_MD_SHA384  = 9,
    SC_MD_SHA512  = 10,
    SC_MD_MD4     = 301,
    SC_MD_CRC32               = 302,
    SC_MD_CRC32_RFC1510       = 303,
    SC_MD_CRC24_RFC2440       = 304
  };

/* Flags used with the open function.  */
enum sc_md_flags
  {
    SC_MD_FLAG_SECURE = 1,  /* Allocate all buffers in "secure"
                                 memory.  */
    SC_MD_FLAG_HMAC   = 2   /* Make an HMAC out of this
                                 algorithm.  */
  };
    
/* Forward declaration.  */
struct sc_md_handle;
typedef struct sc_md_handle *sc_md_hd_t;


/* Compatibility types, do not use them.  */
typedef struct sc_md_handle *SC_MD_HD _SC_GCC_ATTR_DEPRECATED;
typedef struct sc_md_handle *ScMDHd _SC_GCC_ATTR_DEPRECATED;

/* Create a message digest object for algorithm ALGO.  FLAGS may be
   given as an bitwise OR of the sc_md_flags values.  ALGO may be
   given as 0 if the algorithms to be used are later set using
   gcry_md_enable.  */
int sc_md_open (sc_md_hd_t *h, int algo, unsigned int flags);

/* Release the message digest object HD.  */
void sc_md_close (sc_md_hd_t hd);

/* Pass LENGTH bytes of data in BUFFER to the digest object HD so that
   it can update the digest values.  This is the actual hash
   function. */
void sc_md_write (sc_md_hd_t hd, unsigned char *buffer, size_t length);

/* Read out the final digest from HD return the digest value for
   algorithm ALGO. */
const unsigned char *sc_md_read (sc_md_hd_t hd, int algo);


/* Retrieve the length in bytes of the digest yielded by algorithm
   ALGO. */
unsigned int sc_md_get_algo_dlen (sc_md_hd_t hd);

/* Finalize the digest calculation.  This is not really needed because
   sc_md_read() does this implicitly. */
void sc_md_final(sc_md_hd_t a);

/* Set key for HMAC */
int sc_md_setkey(sc_md_hd_t hd, unsigned char *buffer, size_t length);

/************************************
 *                                  
 *            RSA Sign functions    
 *                                  
 ************************************/

/* data type compatibility */

// OpenSSL

//typedef ScMDHd				EVP_MD_CTX;
#define EVP_MD_CTX	ScMDHd

#define sc_sign_init(a,b)		sc_md_open(a,b,SC_MD_FLAG_SECURE)	
#define sc_sign_update(a,b,c)	sc_md_write(a,b,c)


/* Signature final */
TInt sc_sign_final(sc_md_hd_t hd,unsigned char *outbuf, unsigned int *outlen, EVP_PKEY *pkey);

/************************************
 *                                  
 *          RSA Verify functions    
 *                                  
 ************************************/
 
#define sc_verify_init(a,b)  		sc_md_open(a,b,SC_MD_FLAG_SECURE)
#define sc_verify_update(a,b,c) 	sc_md_write(a,b,c)

/* Verify final */
TInt sc_verify_final(sc_md_hd_t hd, unsigned char *signature, unsigned int len, EVP_PKEY *pkey);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SYMBIANCRYPTO_CRYPTO_WRAPPER_H__ */

#define __SYMBIANCRYPTO_CRYPTO_WRAPPER_H__    
