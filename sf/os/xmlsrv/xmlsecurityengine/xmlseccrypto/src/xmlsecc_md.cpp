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
* Description:  A port of gnu md.c useful functions to Symbian.
*
*/


/** A port of gnu md.c useful functions to Symbian **/

#include <stdlib.h>
#include <hash.h>
#include "xmlsecc_config.h"
#include "xmlsecc_cryptowrapper.h"
#include "xmlsec_error_flag.h"


/* This object is used to hold a handle to a message digest object.
   This structure is private - only to be used by the public sc_md_*
   macros.  */
typedef struct sc_md_handle 
{
  /* Actual context.  */
  CMessageDigest *digest;
  
  /* Buffer management.  */
  HBufC8 *hash;
  
  /* Indicate support for HMAC */
  int hmac;

} *sc_md_hd_t;

/****************
 * Open a message digest handle for use with algorithm ALGO.
 * More algorithms may be added by md_enable(). The initial algorithm
 * may be 0.
 */

static TInt
md_open (sc_md_hd_t *h, int algo, int secure, int hmac)
{
  TInt err = KErrNone;
  //int bufsize = secure ? 512 : 1024;
  sc_md_hd_t hd;
  hd = (sc_md_hd_t)malloc (sizeof (struct sc_md_handle));
  if (! hd)
    err = KErrNoMemory;
  if (! err)
    {
      /* Setup the globally visible data (bctl in the diagram).*/
      switch(algo)
      {
      	case SC_MD_SHA1:
      		TRAP(err, hd->digest = CSHA1::NewL()); 
      		hd->hash = NULL;
      		break;
      	default:
      		free(hd);
			return KErrNotSupported;	  	      		      	
      }
  	  //HMAC
  	  hd->hmac = hmac;
    }
  if (! err)
  {
    *h = hd; 
  }
  else
  {
    free(hd);  	
  }
  return err;
}

/* Create a message digest object for algorithm ALGO.  FLAGS may be
   given as an bitwise OR of the sc_md_flags values.  ALGO may be
   given as 0 if the algorithms to be used are later set using
   sc_md_enable. H is guaranteed to be a valid handle or NULL on
   error.  */
TInt
sc_md_open (sc_md_hd_t *h, int algo, unsigned int flags)
{
  TInt err = KErrNone;
  sc_md_hd_t hd=NULL;

  if ((flags & ~(SC_MD_FLAG_SECURE | SC_MD_FLAG_HMAC)))
    err = KErrArgument;
  else
    {

      err = md_open (&hd, algo, (flags & SC_MD_FLAG_SECURE),
		     (flags & SC_MD_FLAG_HMAC));

    }

  *h = err? NULL : hd;
  return err;
}

/****************
 * Return the length of the digest in bytes.
 * This function will return 0 in case of errors.
 */
unsigned int
sc_md_get_algo_dlen (sc_md_hd_t hd)
{

  TInt len = hd->digest->HashSize();
  
  return len;
}

const byte *
md_read(sc_md_hd_t hd)
{
	TRAPD(leaveValue,hd->hash = HBufC8::NewL(hd->digest->HashSize()));
	if (leaveValue != KErrNone)
	    {
	    xmlSecSetErrorFlag( leaveValue );
	    return NULL;
	    }	
	*(hd->hash)=hd->digest->Final();
	return hd->hash->Ptr();
}

/*
 * Read out the complete digest, this function implictly finalizes
 * the hash.
 */
const byte *
sc_md_read (sc_md_hd_t hd, int /*algo*/)
{
  const byte *ret;

  if (hd->hash)
  {
  	delete hd->hash;
  	hd->hash = NULL;
  } 
  ret = md_read (hd);
  return ret;
}

void
sc_md_write (sc_md_hd_t hd, unsigned char *inbuf, size_t inlen)
{

  TPtrC8 inbufPtr(inbuf, inlen);
  hd->digest->Update(inbufPtr);
  
}

void
sc_md_close (sc_md_hd_t hd)
{

  if (hd->digest)
  {
  	delete hd->digest;
  	hd->digest = NULL;  	
  }

  if (hd->hash)
  {
  	delete hd->hash;
  	hd->hash = NULL;
  }
  
  free (hd);
}

void
sc_md_final (sc_md_hd_t /*a*/)
{
 	// Do nothing, sc_md_read will call final()
}


/* Set key for HMAC */
int sc_md_setkey(sc_md_hd_t hd, unsigned char *buffer, size_t length)
{
  TInt err =KErrNone;
  
  if (!hd || !buffer || length <=0)
  	return KErrArgument;
  
  TPtrC8 keyPtr(buffer, length);
   
  if (hd->hmac) 
  {
  	// Initialize HMAC
  	 TRAP(err, hd->digest = CHMAC::NewL(keyPtr, hd->digest));
  	 if ( err != KErrNone )
  	    {
  	    xmlSecSetErrorFlag( err );
  	    }
  }  		 		
  
  return err;
}
