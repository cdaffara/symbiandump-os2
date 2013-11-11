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

#include <e32def.h>
#include <string.h>
#include <random.h>
#include <stdlib.h>

// reading different cipher algorithm
#include <3des.h>
#include <des.h>
#include <rijndael.h>
#include <nullcipher.h>
#include <blocktransformation.h>
#include <padding.h>
#include <cbcmode.h>
#include <bufferedtransformation.h>
#include <msymmetriccipher.h>

/** A port of gnu useful functions to Symbian **/
#include "xmlsecc_config.h"
#include "xmlsecc_globals.h"

#include "xmlsec_error_flag.h"
#include "xmlsecc_padding.h"

#include "xmlsecc_cryptowrapper.h"

// set block size according to "Symmetric ciphers - How To"
//#define MAX_BLOCKSIZE 16
const TInt KBlockSize( 8 );
const TInt KAESBlockSize( 16 );

#define CTX_MAGIC_NORMAL 0x24091964
#define CTX_MAGIC_SECURE 0x46919042

const TInt KDESKeySize( 8 );
const TInt K3DESKeySize( 24 );
const TInt KAESKeySize128( 16 );
const TInt KAESKeySize192( 24 );
const TInt KAESKeySize256( 32 );


/* The handle structure.  */
struct sc_cipher_handle
{
  int magic;
  size_t actual_handle_size;     /* Allocated size of this handle. */
  CSymmetricCipher* iEncryptor;				// CBlockTransformation
  CSymmetricCipher* iDecryptor;
  HBufC8* iKey;
  TInt algo;
  size_t blocksize;
  int mode;
  unsigned int flags;
  HBufC8* iv; 
  int unused;  /* in IV */

};


// Call Symbian random generator 
void doRandomizeL(unsigned char* buffer, size_t length)
    {  
	HBufC8* hbuf = HBufC8::NewLC(length);   
    TPtr8 ptr = hbuf->Des();
    ptr.FillZ(ptr.MaxLength());
    
    CSystemRandom* rand=CSystemRandom::NewLC();
    rand->GenerateBytesL(ptr);
    memcpy(buffer, hbuf->Ptr(), length);
    CleanupStack::PopAndDestroy(rand);    
    CleanupStack::PopAndDestroy(hbuf);	
    }


/**
  - Function: void sc_randomize (unsigned char *BUFFER, size_t LENGTH,
           enum sc_random_level LEVEL)
 **/
TInt sc_randomize(unsigned char* buffer, size_t length, enum sc_random_level level)
    {
	TInt leaveValue( KErrNone );
    TRAP(leaveValue, doRandomizeL (buffer, length));

    if ( leaveValue != KErrNone )
        {
        xmlSecSetErrorFlag( leaveValue );
        }
    return leaveValue;
    }

/* Retrieve the block length used with algorithm A. */
size_t sc_cipher_get_algo_blklen (int algo)
{
    switch (algo) {
    	// set block size according to "Symmetric ciphers - How To"
    	case SC_CIPHER_AES128:
    	case SC_CIPHER_AES192: 
    	case SC_CIPHER_AES256: 
    	 	return KAESBlockSize;				 	
		case SC_CIPHER_NONE:
		case SC_CIPHER_3DES:
		case SC_CIPHER_DES:
		default:
		    return KBlockSize;	// 8 bytes

    }

}

/* Set the IV to be used for the encryption context C to IV with
   length IVLEN.  The length should match the required length. */
TInt
sc_cipher_setiv( sc_cipher_hd_t c, const byte *iv, unsigned int ivlen )
    {
    TInt error( KErrNone );
   
    if( iv ) {
    
	    if (c->iv) 
	    {
	    	delete c->iv;
	    	c->iv = NULL;
	    }    
    
    
		if( ivlen != c->blocksize )
		{
		/*
		    log_info("WARNING: cipher_setiv: ivlen=%u blklen=%u\n",
			     ivlen, (unsigned) c->blocksize );	
		 */     		
		}
		if (ivlen > c->blocksize)
		  ivlen = c->blocksize;

		
	 	TRAP( error, c->iv = HBufC8::NewL(ivlen) );
        if ( error )    //!= KErrNone
            {
            xmlSecSetErrorFlag( error );
            }
	 	else
	 	    {
	 	    TPtrC8 ptr(iv, ivlen);
		    *(c->iv) = ptr;   
	 	    }
		 
    }
    c->unused = 0;
    
    return error;
    }

/* Set the proper Symbian data structures */
void
cipher_encrypt_prerequisiteL (sc_cipher_hd_t c)
{

  if (c->iEncryptor)
    return;
  
  	// Generate key and encryptor/decryptor
  	CBlockTransformation* bT_encrypt = NULL;
  	CBlockTransformation* encryptor = NULL;

  	switch(c->algo)
  	{
  	
		case SC_CIPHER_AES128:
		case SC_CIPHER_AES192:
		case SC_CIPHER_AES256:
			bT_encrypt = CAESEncryptor::NewLC(c->iKey->Des());			
			CleanupStack::Pop(bT_encrypt);		
		 	break;			 	
		case SC_CIPHER_3DES:
			bT_encrypt = C3DESEncryptor::NewLC(c->iKey->Des());
			CleanupStack::Pop(bT_encrypt);			
			break;
		case SC_CIPHER_DES:
			bT_encrypt = CDESEncryptor::NewLC(c->iKey->Des());
			CleanupStack::Pop(bT_encrypt);	
			break;		
  		case SC_CIPHER_NONE:
		 	c->iEncryptor = CNullCipher::NewL();
			break;	  	
		default:
			User::Leave(KErrNotSupported);
		    break;	  		
  	}
  	
  	switch (c->mode)
  	{
  		case SC_CIPHER_MODE_ECB:
  			break;
      	case SC_CIPHER_MODE_CBC:
						
			CleanupStack::PushL(bT_encrypt);			
			encryptor = CModeCBCEncryptor::NewLC(bT_encrypt, c->iv->Des());		
			CleanupStack::Pop(encryptor);		
			CleanupStack::Pop(bT_encrypt);	
									
	  		break;
  		default:
  			break;
  	}
  	
	CleanupStack::PushL(encryptor);

	if (!c->iEncryptor)
	{
		if (!encryptor)
			{
			User::Leave(KErrGeneral);
			}
		CPadding* ePadding = CXmlSecPadding::NewLC(encryptor->BlockSize());
		c->iEncryptor = CBufferedEncryptor::NewL(encryptor, ePadding);
		CleanupStack::Pop(ePadding);
		
	}

	CleanupStack::Pop(encryptor); 	
}


static TInt
cipher_encrypt_doProcessL (sc_cipher_hd_t c, byte *outbuf, const byte *inbuf,
		unsigned int nbytes, int last)
    {
	TInt rc(KErrNone);
	
    switch( c->mode ) {
    
	  case SC_CIPHER_MODE_ECB:
      case SC_CIPHER_MODE_CBC:
        {
        TUint blockSize = c->iEncryptor->BlockSize();     
	 	 
		if ((c->mode == SC_CIPHER_MODE_CBC) 
				&& (nbytes%blockSize && !last)
	            && !(nbytes > blockSize
	                && (c->flags & SC_CIPHER_CBC_CTS)))
		{
			rc = KErrArgument;
		}
		else if ((c->mode == SC_CIPHER_MODE_ECB) 
					&& (nbytes%blockSize && !last))
		{
			rc = KErrArgument;	
		}
		else 
		{
			TInt outlen;
			if (last)
				outlen = c->iEncryptor->MaxFinalOutputLength(nbytes);
			else
				outlen = c->iEncryptor->MaxOutputLength(nbytes);
			HBufC8 *result = HBufC8::NewLC(outlen);
			TPtr8 resultActual= result->Des();
			resultActual.FillZ(resultActual.MaxLength());
			resultActual.SetLength(0);
			
			// could improve memory usage later by allocating a smaller buffer
			HBufC8 *input = HBufC8::NewLC(nbytes);
			TPtrC8 inbufPtr(inbuf, nbytes);
			*input = inbufPtr;
		   
		    TInt j(0);
			for(; j+blockSize<nbytes; j+=blockSize)
			{//	encryption in blocks of size blocksize
				c->iEncryptor->Process(input->Mid(j,blockSize), resultActual);
			}
		
			if (last)	
			{
				c->iEncryptor->ProcessFinalL(input->Mid(j), resultActual);
				// we delete iEncryptor once the decryption finishes
				// so that the next decryption can use a different key or iv
				delete c->iEncryptor;
				c->iEncryptor = NULL;		
			}
			else
				c->iEncryptor->Process(input->Mid(j,blockSize), resultActual);
			
			memcpy(outbuf, result->Ptr(), result->Size());
			CleanupStack::PopAndDestroy(input);
			CleanupStack::PopAndDestroy(result);
	    }
        }	
	break;
	/*
      case SC_CIPHER_MODE_CFB:
	do_cfb_encrypt(c, outbuf, inbuf, nbytes );
	break;
      case SC_CIPHER_MODE_CTR:
	do_ctr_encrypt(c, outbuf, inbuf, nbytes );
	break;
      case SC_CIPHER_MODE_STREAM:
        c->cipher->stencrypt ( &c->context.c,
                               outbuf, (byte*)inbuf, nbytes );
        break;
      case SC_CIPHER_MODE_NONE:
	if( inbuf != outbuf )
	    memmove( outbuf, inbuf, nbytes );
	break;
	*/
      default:
        rc = KErrNotSupported;
        break;
        
    }

	return rc;		
    }

/****************
 * Encrypt INBUF to OUTBUF with the mode selected at open.
 * inbuf and outbuf may overlap or be the same.
 * Depending on the mode some contraints apply to NBYTES.
 */
static TInt
cipher_encrypt (sc_cipher_hd_t c, byte *outbuf,
		const byte *inbuf, unsigned int nbytes, int last)
{
	TInt leaveValue;
	TInt ret(KErrNone);
	    
	// Perform Symbian prerequisite data structure initialization
    TRAP(leaveValue, cipher_encrypt_prerequisiteL (c));
	if (leaveValue) //!= KErrNone;
	    {
	    xmlSecSetErrorFlag( leaveValue );
		return leaveValue;
	    }
	
	// Symbian encryption
	TRAP(leaveValue, ret=cipher_encrypt_doProcessL (c, outbuf, inbuf, nbytes, last));
	if (leaveValue) //!= KErrNone
	    {
	    xmlSecSetErrorFlag( leaveValue );
		return leaveValue;
	    }
    return ret;
}

/****************
 * Encrypt IN and write it to OUT.  If IN is NULL, in-place encryption has
 * been requested.
 */
int
sc_cipher_encrypt (sc_cipher_hd_t h, void *out, size_t outsize,
                     const void *in, size_t inlen, int last)
{
  int err;

  if (!in)
    /* Caller requested in-place encryption. */
    /* Actullay cipher_encrypt() does not need to know about it, but
     * we may change this to get better performance. */
    
    err = cipher_encrypt (h, (byte *)out, (byte *)out, outsize, last);
  else if (outsize < ((h->flags & SC_CIPHER_CBC_MAC) ?
                      h->blocksize : inlen))
    err = KErrArgument;
  else if ((h->mode == SC_CIPHER_MODE_ECB
	    || (h->mode == SC_CIPHER_MODE_CBC
		&& (! ((h->flags & SC_CIPHER_CBC_CTS)
		       && (inlen > h->blocksize)))))
	   && ((inlen % h->blocksize) && !last))	// let Symbian do the padding
    err = KErrArgument;
  else
    err = cipher_encrypt (h, (byte *)out, (byte *)in, inlen, last);
  
  if (err && out)
    memset (out, 0x42, outsize); /* Failsafe: Make sure that the
                                    plaintext will never make it into
                                    OUT. */

  return err;
}

/* Set the proper Symbian data structures */
void
cipher_decrypt_prerequisiteL (sc_cipher_hd_t c)
{

  if (c->iDecryptor)
    return;
  
  	// Generate key and encryptor/decryptor
  	CBlockTransformation* bT_decrypt = NULL;
  	CBlockTransformation* decryptor = NULL;

  	switch(c->algo)
  	{
  	
		case SC_CIPHER_AES128:
		case SC_CIPHER_AES192:
		case SC_CIPHER_AES256:		
			bT_decrypt = CAESDecryptor::NewLC(c->iKey->Des());
			CleanupStack::Pop(bT_decrypt);		
		 	break;			 
		case SC_CIPHER_3DES:
			bT_decrypt = C3DESDecryptor::NewLC(c->iKey->Des());
			CleanupStack::Pop(bT_decrypt);	
			break;
		case SC_CIPHER_DES:
			bT_decrypt = CDESDecryptor::NewLC(c->iKey->Des());
			CleanupStack::Pop(bT_decrypt);	
			break;		
  		case SC_CIPHER_NONE:
			c->iDecryptor = CNullCipher::NewL();
			break;	  	
		default:
			User::Leave(KErrNotSupported);
		    break;	  		
  	}
  	
  	switch (c->mode)
  	{
  		case SC_CIPHER_MODE_ECB:
  			break;
      	case SC_CIPHER_MODE_CBC:						
			CleanupStack::PushL(bT_decrypt);
			decryptor = CModeCBCDecryptor::NewLC(bT_decrypt, c->iv->Des());		
			CleanupStack::Pop(decryptor);	
			CleanupStack::Pop(bT_decrypt);	
									
	  		break;
  		default:
  			break;
  	}
  	
	CleanupStack::PushL(decryptor);

	if (!c->iDecryptor)
	{
		if (!decryptor)
			{
			User::Leave(KErrGeneral);
			}
		CPadding* dPadding = CXmlSecPadding::NewLC(decryptor->BlockSize());
		c->iDecryptor = CBufferedDecryptor::NewL(decryptor, dPadding);
		CleanupStack::Pop(dPadding);
		
	}

	CleanupStack::Pop(decryptor); 	
}

static TInt
cipher_decrypt_doProcessL (sc_cipher_hd_t c, byte *outbuf, const byte *inbuf,
		unsigned int nbytes, int* outputLen, int last)
    {
	TInt rc(KErrNone);
	
    switch( c->mode ) {
	  case SC_CIPHER_MODE_ECB:
      case SC_CIPHER_MODE_CBC:
        {
        TUint blockSize = c->iDecryptor->BlockSize();     
	 	 
		if ((c->mode == SC_CIPHER_MODE_CBC) 
				&& nbytes%blockSize
	            && !(nbytes > blockSize
	                && (c->flags & SC_CIPHER_CBC_CTS)))
		{
			rc = KErrArgument;
		}
		else if ((c->mode == SC_CIPHER_MODE_ECB) 
					&& nbytes%blockSize)
		{
			rc = KErrArgument;
		}	
		else
		{
			TInt outlen;
			if (last)
				outlen = c->iDecryptor->MaxFinalOutputLength(nbytes);
			else
				outlen = c->iDecryptor->MaxOutputLength(nbytes);			
			HBufC8 *result = HBufC8::NewLC(outlen);
			TPtr8 resultActual= result->Des();
			resultActual.FillZ(resultActual.MaxLength());
			resultActual.SetLength(0);
			
			//  could improve memory usage later by allocating a smaller buffer
			HBufC8* input = HBufC8::NewLC(nbytes);
			TPtrC8 inbufPtr(inbuf, nbytes);
			*input = inbufPtr;

		    TInt j=0;
		    
			for(; j+blockSize<nbytes; j+=blockSize)
			{	//	decryption in blocks of size blocksize
				c->iDecryptor->Process(input->Mid(j,blockSize), resultActual);
			}			
		
			if (last)	
			{
				c->iDecryptor->ProcessFinalL(input->Mid(j), resultActual);
				*outputLen = result->Size();
				
				// we delete iDecryptor once the decryption finishes
				// so that the next decryption can use a different key or iv
				delete c->iDecryptor;
				c->iDecryptor = NULL;		
			}
			else
				c->iDecryptor->Process(input->Mid(j,blockSize), resultActual);
			
			memcpy(outbuf, result->Ptr(), result->Size());				
			
			CleanupStack::PopAndDestroy(input);
			CleanupStack::PopAndDestroy(result);	
			
		}
        }
	break;
	/*
      case SC_CIPHER_MODE_CFB:
	do_cfb_decrypt(c, outbuf, inbuf, nbytes );
	break;
      case SC_CIPHER_MODE_CTR:
	do_ctr_decrypt(c, outbuf, inbuf, nbytes );
	break;
      case SC_CIPHER_MODE_STREAM:
        c->cipher->stdecrypt ( &c->context.c,
                               outbuf, (byte*) inbuf, nbytes );
        break;
      case SC_CIPHER_MODE_NONE:
	if( inbuf != outbuf )
	    memmove( outbuf, inbuf, nbytes );
	break;
	    */
      default:
        rc = KErrNotSupported;
        break;
        
    }	
    
    return rc;
    }

/****************
 * Decrypt INBUF to OUTBUF with the mode selected at open.
 * inbuf and outbuf may overlap or be the same.
 * Depending on the mode some some contraints apply to NBYTES.
 */
static TInt
cipher_decrypt (sc_cipher_hd_t c, byte *outbuf, const byte *inbuf,
		unsigned int nbytes, int *outlen, int last)
{
    TInt rc( KErrNone );
    TInt leaveValue;

	// Perform Symbian prerequisite data structure initialization
    TRAP(leaveValue, cipher_decrypt_prerequisiteL (c));
	if (leaveValue != KErrNone)
	    {
	    xmlSecSetErrorFlag( leaveValue );
		return leaveValue;
	    }
		
	// Symbian decryption
	TRAP(leaveValue, rc=cipher_decrypt_doProcessL (c, outbuf, inbuf, nbytes, outlen, last));
	if (leaveValue != KErrNone)
	    {
	    xmlSecSetErrorFlag( leaveValue );
	    return leaveValue;
	    }
    return rc;
}



int
sc_cipher_decrypt (sc_cipher_hd_t h, void *out, size_t outsize,
		     const void *in, size_t inlen, int* outlen, int last)
{
  TInt err( KErrNone );

  if (last && !outlen)
    err = KErrArgument;
  else if (! in)
    /* Caller requested in-place encryption. */
    /* Actullay cipher_encrypt() does not need to know about it, but
     * we may chnage this to get better performance. */
    err = cipher_decrypt (h, (byte*)out, (byte*)out, outsize, outlen, last);
  else if (inlen == 0)
    return err;
  else if (outsize < inlen)
    err = KErrArgument;
  else if (((h->mode == SC_CIPHER_MODE_ECB)
	    || ((h->mode == SC_CIPHER_MODE_CBC)
		&& (! ((h->flags & SC_CIPHER_CBC_CTS)
		       && (inlen > h->blocksize)))))
	   && (inlen % h->blocksize))
    err = KErrArgument;
  else
    err = cipher_decrypt (h, (byte*)out, (byte*)in, inlen, outlen, last);

  return err;
}

/*
   Open a cipher handle for use with cipher algorithm ALGORITHM, using
   the cipher mode MODE (one of the SC_CIPHER_MODE_*) and return a
   handle in HANDLE.  Put NULL into HANDLE and return an error code if
   something goes wrong.  FLAGS may be used to modify the
   operation.  The defined flags are:

   SC_CIPHER_SECURE:  allocate all internal buffers in secure memory.
   SC_CIPHER_ENABLE_SYNC:  Enable the sync operation as used in OpenPGP.
   SC_CIPHER_CBC_CTS:  Enable CTS mode.
   SC_CIPHER_CBC_MAC:  Enable MAC mode.

   Values for these flags may be combined using OR.
 */
TInt 
sc_cipher_open (sc_cipher_hd_t *handle,
		  int algo, int mode, unsigned int flags)
{
  int secure = (flags & SC_CIPHER_SECURE);
  CSymmetricCipher* encryptor = NULL;	//CBlockTransformation
  CSymmetricCipher* decryptor = NULL;
  sc_cipher_hd_t h = NULL;
  TInt err( KErrNone );

  /* If the application missed to call the random poll function, we do
     it here to ensure that it is used once in a while. */
  
  /* check flags */
  if ((! err)
      && ((flags & ~(0 
		     | SC_CIPHER_SECURE
		     | SC_CIPHER_ENABLE_SYNC
		     | SC_CIPHER_CBC_CTS
		     | SC_CIPHER_CBC_MAC))
	  || (flags & SC_CIPHER_CBC_CTS & SC_CIPHER_CBC_MAC)))
    err = KErrArgument;

  /* check that a valid mode has been requested */
  if (! err)
    switch (mode)
      {//
      case SC_CIPHER_MODE_ECB:
      case SC_CIPHER_MODE_CBC:
      case SC_CIPHER_MODE_CFB:
      case SC_CIPHER_MODE_CTR:
      case SC_CIPHER_MODE_STREAM:
	break;
      case SC_CIPHER_MODE_NONE:
	break;

      default:
	err = KErrNotSupported;
      }


  if (! err)
    {
	size_t size = sizeof(*h);
	h = (sc_cipher_hd_t)malloc(size);
	
    if ( !h )
        {
	    err = KErrNoMemory;
        xmlSecSetErrorFlag( KErrNoMemory );
        }
      else
	{
	  h->magic = secure ? CTX_MAGIC_SECURE : CTX_MAGIC_NORMAL;
          h->actual_handle_size = size;
	  h->iEncryptor = encryptor;
	  h->iDecryptor = decryptor;
	  h->iKey = NULL;
	  h->algo = algo;  
	  h->iv = NULL;

	  h->mode = mode;
	  h->flags = flags;
	  
	  if (algo == SC_CIPHER_NONE)
	  {
	  TRAPD(leaveValue,h->iEncryptor = CNullCipher::NewL());
            if (leaveValue) //!= KErrNone
                {
                xmlSecSetErrorFlag( leaveValue );
                return leaveValue;
                }	
            TRAP(leaveValue,h->iDecryptor = CNullCipher::NewL());
            if (leaveValue) //!= KErrNone
                {
                xmlSecSetErrorFlag( leaveValue );
                return leaveValue;
                }	
            	  	
	  }
	}
    }

  *handle = err ? NULL : h;

  return err;
}


/* Release all resources associated with the cipher handle H. H may be
   NULL in which case this is a no-operation. */
void
sc_cipher_close (sc_cipher_hd_t h)
{
  if (! h)
    return;

  if ((h->magic != CTX_MAGIC_SECURE)
      && (h->magic != CTX_MAGIC_NORMAL))
  {
  /*
    _sc_fatal_error(GPG_ERR_INTERNAL,
		      "sc_cipher_close: already closed/invalid handle");  
  */	
     return;
  }

  else
    h->magic = 0;

  if (h->iEncryptor)
  {
  	delete h->iEncryptor;
  	h->iEncryptor = NULL;
  }
  
  if (h->iDecryptor)
  {
  	delete h->iDecryptor;
  	h->iDecryptor = NULL;
  }
  
  if (h->iv)
  {
  	delete h->iv;
  	h->iv = NULL;
  }
  
  if (h->iKey)
  {
  	delete h->iKey;
  	h->iKey = NULL;
  }
  

  /* We always want to wipe out the memory even when the context has
     been allocated in secure memory.  The user might have disabled
     do the wiping.  To accomplish this we need to keep track of the
     actual size of this structure because we have no way to known
     how large the allocated area was when using a standard malloc. */
  /*
  TInt len = h->actual_handle_size;
  memset(h, 'a', len);	// fill with something
  */

  free (h);
}

size_t
sc_cipher_get_algo_keylen (int algo) 
{
  switch (algo) {

	case SC_CIPHER_AES128:
	 	return KAESKeySize128;
	case SC_CIPHER_AES192:
	 	return KAESKeySize192;
	case SC_CIPHER_AES256:
	 	return KAESKeySize256;	 		 	
	case SC_CIPHER_3DES:
		return K3DESKeySize;
	case SC_CIPHER_DES:
		return KDESKeySize;
	case SC_CIPHER_NONE:		
	default:
	    return 0;	

  }

}


/* Set the key to be used for the encryption context C to KEY with
   length KEYLEN.  The length should match the required length. */
//sc_error_t
TInt sc_cipher_setkey (sc_cipher_hd_t c, byte *key, unsigned int keylen)
    {
    
    // Change key into the right format
    TRAPD(error, c->iKey = HBufC8::NewL(keylen));
    if(error == KErrNone)
        {
        TPtrC8 keyPtr(key, keylen);
        *(c->iKey) = keyPtr;
        }
    else 
        {
        xmlSecSetErrorFlag( error );
        }
    return error;
    }

/* Set specification blocksize in context */
void set_ctx_blocksize(sc_cipher_hd_t c, size_t bklen)
{
	c->blocksize = bklen;
}
