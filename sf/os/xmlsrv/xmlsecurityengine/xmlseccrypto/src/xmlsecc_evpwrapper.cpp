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
* Description:  Methods that allows to sign and verify data.
*
*/


/** A wrapper of OpenSSL evp.c functions to Symbian **/

#include <stdlib.h>
#include <string.h>
#include <signed.h>
#include "xmlsecc_config.h"
#ifndef XMLSEC_NO_X509
#include <x509cert.h>
#endif //XMLSEC_NO_X509

#include "xmlsecc_evpwrapper.h"
#include "xmlsecc_cryptowrapper.h"

#include "xmlsecmsymbiankeystore.h"

#include "xmlsec_error_flag.h"

_LIT8(KRsaKeyName, "xmlSecRsaKey");

const TInt KAssertionLeave = -32380;

// MK: Flag - is active scheduler created by xml sec or not.
static TBool desActiv = FALSE;

struct ScKeyStore
{
    // A handle to Symbian key store 
	CSymbianKeyStore				*iKeyStore;
	// A handle to the public key read from the certificate
	CSubjectPublicKeyInfo			*iSubjectPublicKeyInfo;
};

// -----------------------------------------------------------------------------
// doInstallActiveSchedulerL Install ActiveScheduler
// -----------------------------------------------------------------------------
void doInstallActiveSchedulerL()
{
   // Construct active scheduler
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(activeScheduler) ;

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install(activeScheduler);   
    
    CleanupStack::Pop(activeScheduler);   
		
}

// -----------------------------------------------------------------------------
// sc_initialize Symbian key initialization
// -----------------------------------------------------------------------------
int sc_pkey_init()
{
	TInt leaveValue = KErrNone;
	// Check if there is an active scheduler in current thread
	if(!CActiveScheduler::Current())
	    {
	    TRAP(leaveValue, doInstallActiveSchedulerL());
	    desActiv = TRUE;
	    }
	return leaveValue;
}

// -----------------------------------------------------------------------------
// sc_pkey_shutdown Shutdown ActiveScheduler
// -----------------------------------------------------------------------------
void sc_pkey_shutdown()
{
    if(desActiv)
        {
        CActiveScheduler* activeScheduler = CActiveScheduler::Current(); 
    	CActiveScheduler::Install(NULL); 
    	if (activeScheduler)
	    	delete activeScheduler;
        }	
}

// -----------------------------------------------------------------------------
// doKeyStoreCreateL Create a new key store 
// Arguments: aKeyStore ScKeyStorePtr structure
// -----------------------------------------------------------------------------
void doKeyStoreCreateL(ScKeyStorePtr aKeyStore)
{

	aKeyStore->iKeyStore = CSymbianKeyStore::NewL();
	aKeyStore->iKeyStore->CreateUnifiedKeyStoreL();
	CActiveScheduler::Start();
			
	User::LeaveIfError( aKeyStore->iKeyStore->GetError() );
}

// -----------------------------------------------------------------------------
// sc_pkey_new Create a new key store structure
// Arguments: aKeyType the type of key to be generated, enum sc_key_algos
// Returns: the pointer of the EVP_PKEY generated
// -----------------------------------------------------------------------------
EVP_PKEY *sc_pkey_new(int aKeyType, char *keyname)
{
    ScKeyStorePtr keyStore;
    TInt err=KErrNone;
    
    EVP_PKEY *pkey = (EVP_PKEY*) malloc(sizeof(EVP_PKEY));
    
    if (pkey)
    {
    	if (keyname)
    	{
    		pkey->name = (char *)malloc(sizeof(char)*(strlen(keyname)+1));
    		if (!pkey->name)
    		{
    			free(pkey);
    			xmlSecSetErrorFlag( KErrNoMemory );
    			return NULL;
    		}
    		strcpy(pkey->name, keyname);
    	}
    	else
    	{
    		pkey->name = NULL;    	
    	}    			

    	keyStore = (ScKeyStorePtr) malloc(sizeof(ScKeyStore));    	
    	if (keyStore)
    	{
    		keyStore->iSubjectPublicKeyInfo = NULL;
    		keyStore->iKeyStore = NULL;
    		
 	   		// Initialize the ScKeyStore - iKeyStore
    		TRAP(err, doKeyStoreCreateL(keyStore));
 			   		
    		if (err)
    		{
    		    delete keyStore->iKeyStore; 
    			free(keyStore);    			
    			free(pkey->name);
    			free (pkey);
    			xmlSecSetErrorFlag( err );
    			return NULL;
    		}
    		
    		pkey->keyStore = keyStore;
    		
    	}
    	else 
    	{
    	    free(pkey->name);
    	    free(pkey);
    		xmlSecSetErrorFlag( KErrNoMemory );
    	    return NULL;    
    	}
    	
    	pkey->type = aKeyType;
    	pkey->load = NOKEY;
    	pkey->bitsize = 0;
    	pkey->duplicate = 0;    	   	
    }
    else 
        {
        xmlSecSetErrorFlag( KErrNoMemory );
        }
    
   return pkey;
}

// -----------------------------------------------------------------------------
// sc_load_key Try to load usable key from the SymbianKeyStore
// Arguments: aPKey a EVP_PKEY key structure
// Returns: KErrNone if no error
//			KErrArgument if the arguments have error
//			KErrNotSupported if the key type is not supported
//			KErrNotFound if no key is found
// -----------------------------------------------------------------------------
int sc_pkey_load(EVP_PKEY *aPKey)
{
	TInt err=KErrNone;
	
	// Check arguments
	if (!aPKey || !aPKey->name)
	{
		return KErrArgument;
	}

	// Load key
	TPtrC8 keyNamePtr((const unsigned char*)aPKey->name, strlen(aPKey->name));
	switch (aPKey->type)
	{
		case EVP_PKEY_RSA:			
			aPKey->keyStore->iKeyStore->FindKey(keyNamePtr, CCTKeyInfo::ERSA);
			break;
		case EVP_PKEY_UNKNOWN:	
			aPKey->keyStore->iKeyStore->FindKey(keyNamePtr, CCTKeyInfo::EInvalidAlgorithm);
			break;			
		default:
			return KErrNotSupported;		
	}
	err = aPKey->keyStore->iKeyStore->GetError();
	if ( err != KErrNone ) 
	    {
	    return err;
	    }
	
	// Run asynchronous key load until done
	CActiveScheduler::Start();		
	err = aPKey->keyStore->iKeyStore->GetError();
	if ( err == KErrNone )
	    {
	    err = aPKey->keyStore->iKeyStore->hasKey();	   
	    }
	if (!err)
	{
		aPKey->load = HASKEY;
		aPKey->bitsize = aPKey->keyStore->iKeyStore->GetKeySize();
		if (aPKey->type == EVP_PKEY_UNKNOWN)
		{
			switch (aPKey->keyStore->iKeyStore->GetKeyAlgorithm())
			{
			case CCTKeyInfo::ERSA:
				aPKey->type = EVP_PKEY_RSA;
				break;
			case CCTKeyInfo::EDSA:
				aPKey->type = EVP_PKEY_DSA;
				break;
			default:
				aPKey->type = EVP_PKEY_UNKNOWN;	
				break;	
			}
		}
	}	
	else
	{
		aPKey->load = NOKEY;
		aPKey->bitsize = 0;
	}
	
	return err;

}

// -----------------------------------------------------------------------------
// sc_generate_key Try to generate key from the SymbianKeyStore
// Arguments: aPKey a EVP_PKEY key structure
//			  aSizeBits bit size of the key generated
// Returns: KErrNone if no error
//			KErrArgument if the arguments have error
// -----------------------------------------------------------------------------
int sc_pkey_generate(EVP_PKEY *aPKey, unsigned int aSizeBits)
{
	TInt err=KErrNone;	
	
	// Check arguments
	if (!aPKey || (aSizeBits <= 0))
	{
		err = KErrArgument;
		return err;
	}
	
	// Use SymbianKeyStore to create RSA key
	aPKey->keyStore->iKeyStore->CreateRSAKey(aSizeBits, KRsaKeyName);
	err = aPKey->keyStore->iKeyStore->GetError();
	if ( err != KErrNone) 
	    {
	    return err;
	    }
	
	// Run asynchronous key creation until done
	CActiveScheduler::Start();		
    err = aPKey->keyStore->iKeyStore->GetError();
    if ( err == KErrNone )
	    err = aPKey->keyStore->iKeyStore->hasKey();	
	if (!err)
	{
		aPKey->load = HASKEY;
		aPKey->bitsize = aSizeBits;
	}	
	else
	{
		aPKey->load = NOKEY;
		aPKey->bitsize = 0;
	}
		
	return err;
}

// -----------------------------------------------------------------------------
// sc_pkey_free Free the EVP_PKEY structure
// Arguments: aPKey a EVP_PKEY structure
// Returns: None
// -----------------------------------------------------------------------------
void sc_pkey_free(EVP_PKEY *aPKey)
{
	if (!aPKey)
	  return;
	
	if (aPKey->keyStore && !aPKey->duplicate)
	{
		if (aPKey->name)
		{
			free(aPKey->name);
			aPKey->name = NULL;
		}	
		
		if (aPKey->keyStore->iKeyStore)
		{
			delete aPKey->keyStore->iKeyStore;
			aPKey->keyStore->iKeyStore = NULL;
		}
		
		if (aPKey->keyStore->iSubjectPublicKeyInfo)
		{
			delete aPKey->keyStore->iSubjectPublicKeyInfo;
			aPKey->keyStore->iSubjectPublicKeyInfo = NULL;
		}
						
		free(aPKey->keyStore);
		aPKey->keyStore = NULL;
	}
	   
	
	free(aPKey);
}

// -----------------------------------------------------------------------------
// sc_pkey_duplicate Duplicate the EVP_PKEY structure
// Arguments: aPKey a EVP_PKEY structure to be copied
// Returns: EVP_PKEY* the new EVP_PKEY structure pointer, NULL if failed
// -----------------------------------------------------------------------------
EVP_PKEY *sc_pkey_duplicate(EVP_PKEY *aPKey)
{

    EVP_PKEY *pKeyNew = (EVP_PKEY*) malloc(sizeof(EVP_PKEY));
    
    if (pKeyNew)
    {
    	pKeyNew->type = aPKey->type;
		pKeyNew->bitsize = aPKey->bitsize;
		pKeyNew->load = aPKey->load;
		pKeyNew->duplicate = 1;
		pKeyNew->keyStore = aPKey->keyStore;		
		pKeyNew->name = aPKey->name;
				
	}
	else
	{
	   xmlSecSetErrorFlag( KErrNoMemory );
	}
	
	return pKeyNew;
}

unsigned int sc_pkey_size(EVP_PKEY */*aPKey*/)
{
	// temporary fix (max 16358 bits = 2048 bytes)
	return 2048;		
}

// -----------------------------------------------------------------------------
// sc_sign_final Finalize signing
// Arguments: aCtx EVP_MD_CTX structure, aOutbuf buffer, 
//                       aOutlen size of buffer,aPKey a EVP_PKEY structure, 
// Returns: NULL if correct operation, or error code
// -----------------------------------------------------------------------------
TInt sc_sign_final(EVP_MD_CTX aCtx, unsigned char *aOutbuf, unsigned int *aOutlen, EVP_PKEY *aPkey)
{
  const byte *hash;
  unsigned int hashLen;
  const unsigned char *outbuf = NULL;
  TInt err;
  
  hashLen = sc_md_get_algo_dlen(aCtx);
  hash = sc_md_read(aCtx, 0);		// the algo is not used currently
  if (!hash)
  {
    return KErrNoMemory;  
  }

  switch(aPkey->type)
  {
  	case EVP_PKEY_RSA:
		  TRAP(err, aPkey->keyStore->iKeyStore->RSASignL(hash, hashLen)); 	  
		  if (err!=KErrNone)
		  {
		     xmlSecSetErrorFlag( err );
		  	 return err;
		  }		  
		  // Run asynchronous RSA signing until done
		  CActiveScheduler::Start();			  
		  outbuf = aPkey->keyStore->iKeyStore->GetSignedData(aOutlen);
  	break;
  	default:
  	return KErrNotSupported;
  }
  
  if (outbuf) 
  {
  	 memcpy(aOutbuf, outbuf, *aOutlen);
  	 return KErrNone;
  }  	
  else 
    {
    	TInt error = aPkey->keyStore->iKeyStore->GetError();
		xmlSecSetErrorFlag( error );
		return error;
    } 
  
}

// -----------------------------------------------------------------------------
// doVerifyFinalL Finalize verification
// Arguments: aHash buffer with hash, aHashLen size of hash, aSignature buffer, 
//                       aLen size of buffer,aPKey a EVP_PKEY structure, 
// Returns: NULL if correct operation, or error code
// -----------------------------------------------------------------------------
TInt doVerifyFinalL(const byte *aHash, 
                                unsigned int aHashLen, 
                                unsigned char *aSignature, 
                                unsigned int aLen, 
                                EVP_PKEY *aPKey)
{
    TInt res = 0;
	switch(aPKey->type)
	{
		case EVP_PKEY_RSA:		
			if (aPKey->keyStore->iSubjectPublicKeyInfo)
			{
				res = (TInt)aPKey->keyStore->iKeyStore->RSAVerifyWithPublicKeyL(aHash, 
				                                        aHashLen, 
				                                        aSignature, 
				                                        aLen, 
				                                        aPKey->keyStore->iSubjectPublicKeyInfo);		
			}
			else
			{
				aPKey->keyStore->iKeyStore->RSAVerifyL(aHash, aHashLen, aSignature, aLen);
				// Run asynchronous RSA signing until done
		  		CActiveScheduler::Start();
                User::LeaveIfError( aPKey->keyStore->iKeyStore->GetError() );
		  		res = (TInt)aPKey->keyStore->iKeyStore->GetVerifyResult();
			};
		break;
		default:
		    res = KErrNotSupported;
	}
    return res;
}

// -----------------------------------------------------------------------------
// sc_verify_final Finalize verification
// Arguments: aCtx EVP_MD_CTX structure, aSignature buffer, 
//                       aLen size of buffer,aPKey a EVP_PKEY structure, 
// Returns: NULL if correct operation, or error code
// -----------------------------------------------------------------------------
TInt sc_verify_final(EVP_MD_CTX aCtx, 
                                unsigned char *aSignature, 
                                unsigned int aLen, 
                                EVP_PKEY *aPKey)
{
  const byte *hash;
  unsigned int hashLen;
  TInt err;
  TInt ret=0;
  
  hashLen = sc_md_get_algo_dlen(aCtx);
  hash = sc_md_read(aCtx, 0);		// the algo is not used currently
  if (!hash)
  {
    return KErrNoMemory;  
  }
  
  TRAP(err, ret = doVerifyFinalL(hash, hashLen, aSignature, aLen, aPKey));  
  
  if (err != KErrNone)
  {
     xmlSecSetErrorFlag( err );
  	 return err;
  }
  else 
  {
  	 return ret;
  }
  
}

// -----------------------------------------------------------------------------
// d2i_PUBKEY_bio Read the public key from ASN.1 DER encoded format
// Arguments: aBio BIO structure
// Returns: EVP_PKEY structure
// -----------------------------------------------------------------------------
EVP_PKEY* d2i_PUBKEY_bio(BIO *aBio)
{
	// Import key to keystore
	TInt err;
	EVP_PKEY *pKey;	
	
	// Remember to get the key type later
	pKey = sc_pkey_new(EVP_PKEY_UNKNOWN, aBio->name);
	if (!pKey)
	{
		return NULL;
	}
	
	// Import key if key is not found
	TPtrC8 keyPtr((const unsigned char*)aBio->mem, aBio->len);
	TRAP(err, pKey->keyStore->iSubjectPublicKeyInfo = CX509SubjectPublicKeyInfo::NewL(keyPtr));	
	if (err!=KErrNone)
	{
		sc_pkey_free(pKey);
		xmlSecSetErrorFlag( err );
		return NULL;
	}	

	pKey->load = HASKEY;	
	switch (pKey->keyStore->iSubjectPublicKeyInfo->AlgorithmId())
	{
		case ERSA:
			pKey->type = EVP_PKEY_RSA;
			break;
		case EDSA:
			pKey->type = EVP_PKEY_DSA;
			break;
		default:
			pKey->type = EVP_PKEY_UNKNOWN;	
			break;	
	}		
		
	return pKey;	
}

// -----------------------------------------------------------------------------
// doKeyImportL Imports key
// Arguments: EVP_PKEY structure, aBio BIO structure
// -----------------------------------------------------------------------------
void doKeyImportL(EVP_PKEY *aPKey, BIO *aBio)
{
	__ASSERT_ALWAYS(aPKey, User::Leave(KAssertionLeave));
	__ASSERT_ALWAYS(aPKey->name, User::Leave(KAssertionLeave));
	
	TPtrC8 keyPtr((const unsigned char*)aBio->mem, aBio->len);	
	TPtrC8 keyNamePtr((const unsigned char*)aPKey->name, strlen(aPKey->name));
	
	aPKey->keyStore->iKeyStore->ImportKey(keyPtr, keyNamePtr);
	User::LeaveIfError( aPKey->keyStore->iKeyStore->GetError() );
	
	// Run asynchronous key creation until done
	CActiveScheduler::Start();	
    User::LeaveIfError( aPKey->keyStore->iKeyStore->GetError() );
}

// -----------------------------------------------------------------------------
// d2i_PKCS8PrivateKey_bio Read the private key from ASN.1 DER encoded PKCS#8 format 
// Arguments: aBio BIO structure, aPwdCallback callback, aPwdCallbackCtx callback context
// Returns: EVP_PKEY structure
// -----------------------------------------------------------------------------
EVP_PKEY* d2i_PKCS8PrivateKey_bio(BIO *aBio, void *aPwdCallback, void *aPwdCallbackCtx)
{
	// Import key to keystore
	TInt err;
	EVP_PKEY *pKey;
	
	// Remember to get the key type later
	pKey = sc_pkey_new(EVP_PKEY_UNKNOWN, aBio->name);
	if (!pKey)
	{
		return NULL;
	}

	// Check if there is existing key first
	err = sc_pkey_load(pKey);
	if (err == 0 && pKey->load)
	{
		// Return key if it is found
		return pKey;
	}	
	else if ( err != KErrNotFound ) 
	    {
	    sc_pkey_free(pKey);
	    xmlSecSetErrorFlag( err );
	    return NULL;
	    }
	
	// Import key if key is not found
	TRAP(err, doKeyImportL(pKey, aBio));
	if (err!=KErrNone)
	{
		sc_pkey_free(pKey);
		xmlSecSetErrorFlag( err );
		return NULL;
	}	


	// Check to see if the import key is successful
	err = pKey->keyStore->iKeyStore->hasKey();	
	if (err)
	{
		sc_pkey_free(pKey);
		xmlSecSetErrorFlag( err );
		return NULL;
	}

	pKey->load = HASKEY;
	pKey->bitsize = pKey->keyStore->iKeyStore->GetKeySize();
	switch (pKey->keyStore->iKeyStore->GetKeyAlgorithm())
	{
		case CCTKeyInfo::ERSA:
			pKey->type = EVP_PKEY_RSA;
			break;
		case CCTKeyInfo::EDSA:
			pKey->type = EVP_PKEY_DSA;
			break;
		default:
			pKey->type = EVP_PKEY_UNKNOWN;	
			break;	
	}

		
	return pKey;
	
}

// -----------------------------------------------------------------------------
// d2i_PKCS8PrivateKey Read the private key from Unified Key Store
// Arguments: keyname name of the key
// Returns: EVP_PKEY structure
// -----------------------------------------------------------------------------
EVP_PKEY* d2i_PKCS8PrivateKey(char *keyname)
{
	TInt err;
	EVP_PKEY *pKey;
	
	// Remember to get the key type later
	pKey = sc_pkey_new(EVP_PKEY_UNKNOWN, keyname);
	if (!pKey)
	{
		return NULL;
	}

	// Check if there is existing key first
	err = sc_pkey_load(pKey);
	if (err == 0 && pKey->load)
	{
		// Return key if it is found
		return pKey;
	}	
	else if ( err != KErrNotFound ) 
	    {
	    sc_pkey_free(pKey);
	    xmlSecSetErrorFlag( err );
	    return NULL;
	    }
	
	// Check to see if the find key is successful
	err = pKey->keyStore->iKeyStore->hasKey();	
	if (err)
	{
		sc_pkey_free(pKey);
		xmlSecSetErrorFlag( err );
		return NULL;
	}

	pKey->load = HASKEY;
	pKey->bitsize = pKey->keyStore->iKeyStore->GetKeySize();
	switch (pKey->keyStore->iKeyStore->GetKeyAlgorithm())
	{
		case CCTKeyInfo::ERSA:
			pKey->type = EVP_PKEY_RSA;
			break;
		case CCTKeyInfo::EDSA:
			pKey->type = EVP_PKEY_DSA;
			break;
		default:
			pKey->type = EVP_PKEY_UNKNOWN;	
			break;	
	}

		
	return pKey;
	
}

#ifndef XMLSEC_NO_X509

// -----------------------------------------------------------------------------
// doGetPubKeyL Set the public key info 
// Arguments: EVP_PKEY structure, aCert X509 structure
// -----------------------------------------------------------------------------
void doGetPubKeyL(EVP_PKEY *aPKey, X509 *aCert)
{
	// Convert X509 to CX509Certificate
	TPtrC8 certPtr((const unsigned char*)aCert->der, aCert->derlen);    
	CX509Certificate *cert = CX509Certificate::NewLC(certPtr);
	
	// Retrieve the public key
	aPKey->keyStore->iSubjectPublicKeyInfo = CSubjectPublicKeyInfo::NewL(cert->PublicKey());
	
	// Cleanup	
	CleanupStack::PopAndDestroy(cert);
	
}

// -----------------------------------------------------------------------------
// sc_pkey_setPublic Set the public key info 
// Arguments: EVP_PKEY structure, aCert X509 structure
// Returns: 0 if operation correct, or error code
// -----------------------------------------------------------------------------
TInt sc_pkey_setPublic(EVP_PKEY* aPKey, X509 *aCert)
{
	TInt err;
	
	if (aPKey->keyStore->iSubjectPublicKeyInfo)
	{
		delete aPKey->keyStore->iSubjectPublicKeyInfo;
		aPKey->keyStore->iSubjectPublicKeyInfo = NULL;
	}

	TRAP(err, doGetPubKeyL(aPKey, aCert));
	
	if (err == KErrNone)
	{
		aPKey->load = HASKEY;
		switch (aPKey->keyStore->iSubjectPublicKeyInfo->AlgorithmId())
		{
			case ERSA:
				aPKey->type = EVP_PKEY_RSA;
				break;
			case EDSA:
				aPKey->type = EVP_PKEY_DSA;
				break;
			default:
				aPKey->type = EVP_PKEY_UNKNOWN;	
				break;	
		}	
		
	}
	else 
	    {
	    xmlSecSetErrorFlag( err );
	    }
	
	return err;
}

#endif // XMLSEC_NO_X509
