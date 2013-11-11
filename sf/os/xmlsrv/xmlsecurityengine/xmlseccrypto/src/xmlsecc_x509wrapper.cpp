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
* Description:  Methods that allows to process X509 certificates.
*
*/



/** A wrapper of OpenSSL x509.c functions to Symbian **/

#ifndef XMLSEC_NO_X509
#include <stdlib.h>
#include <string.h>
#include <x509cert.h>
#include <e32std.h>

#include "xmlsecc_config.h"
#include "xmlsecc_x509wrapper.h"
#include "xmlsecc_evpwrapper.h"

#include "xmlsecmsymbiancertchain.h"
#include "xmlsecmsymbiancertstore.h"
#include "xmlsec_error_flag.h"

struct ScX509Store
{	
	RPointerArray< CX509Certificate>	iRootCerts;
	CSymbianCertChain			*iCertChain;	
	CSymbianCertStore                             *iSymbianCertStore;	
	RPointerArray< CX509Certificate>    iRootCertsSCS;
};

void X509_free(X509* aCert)
{
	if (aCert)
	{
		if (aCert->der)
		{
			memset(aCert->der, 0, aCert->derlen);
			free (aCert->der);
		}
		
		free(aCert);
	}
}

void X509_crl_free(X509_CRL* aCrl)
{
}


X509* X509_dup(X509* aCert)
{
	return aCert;
}


/* Get the public key */
EVP_PKEY* X509_get_pubkey(X509* aCert)
{
	EVP_PKEY *pKey = NULL;
	TInt err;
	
	// Create the pKey structure
	pKey = sc_pkey_new(EVP_PKEY_UNKNOWN, NULL);
	if (!pKey)
	{
		return NULL;
	}
	
	//Assign the public key
	err = sc_pkey_setPublic(pKey, aCert);
	if (err != KErrNone)
	{
		sc_pkey_free(pKey);
		return NULL;
	}

	return pKey;
}

TInt testValidityPeriodL(X509 *aCert)
{
	// convert aCert to TDesC8
	TPtrC8 certPtr((const unsigned char*)aCert->der, aCert->derlen);   

	CX509Certificate *cert = CX509Certificate::NewLC(certPtr);
	CValidityPeriod validPeriod = cert->ValidityPeriod();	
	
	TTime currentTime;
	currentTime.HomeTime();	
	
	TBool res = validPeriod.Valid(currentTime);
	CleanupStack::PopAndDestroy(cert);
	
	return (TInt)res;
	
}


/* Test the validity period from the certificate */
TInt X509_test_validityPeriod(X509* aCert)
{
	TInt err;
	TBool res=FALSE;

	TRAP(err, res = testValidityPeriodL(aCert));
	   
	if (err)
	    {
	    xmlSecSetErrorFlag( err );
	    return err;    
	    }
	else
		return res;
}

/* Read the certificate from DER format */
X509* d2i_X509_bio(BIO *aBio)
{
	X509 *cert = (X509 *)malloc(sizeof(X509));
	if (!cert)
	    {
	    xmlSecSetErrorFlag( KErrNoMemory );
		return NULL;
	    }
	
	// Duplicate the certificate
	cert->der = (char *)malloc(aBio->len * sizeof(char));
    if (!cert->der)
        {
        free( cert );
        xmlSecSetErrorFlag( KErrNoMemory );
        return NULL;    
        }
	memcpy(cert->der, aBio->mem, aBio->len);
	
	// Set length
	cert->derlen = aBio->len;
	
	return cert;
		 
}


X509_STORE *X509_STORE_new( void )
{
	X509_STORE *certStore = (X509_STORE *)malloc(sizeof(X509_STORE));
	
	if (certStore)
	{
            certStore->iRootCerts = RPointerArray<CX509Certificate> (2);
            certStore->iRootCertsSCS = RPointerArray<CX509Certificate> (2);
            certStore->iCertChain = NULL;
            certStore->iSymbianCertStore = NULL;
	}
	else
	    {
	    xmlSecSetErrorFlag( KErrNoMemory );
	    }
	
	
	return certStore;
}

void X509_STORE_free(X509_STORE *aCertStore)
{
	if (aCertStore)
	    {
	    if (aCertStore->iCertChain)
	        {
	        delete aCertStore->iCertChain;
	        aCertStore->iCertChain = NULL;
	        }
	    if (aCertStore->iSymbianCertStore)
	        {
	        delete aCertStore->iSymbianCertStore;
	        aCertStore->iSymbianCertStore = NULL;
	        }
            aCertStore->iRootCertsSCS.ResetAndDestroy();
            aCertStore->iRootCerts.ResetAndDestroy();
            free(aCertStore);
	}
}

void doAddCertL(X509_STORE *aCertStore, X509 *aCert)
{
	// convert aCert to TDesC8
	TPtrC8 certPtr((const unsigned char*)aCert->der, aCert->derlen);   		
	CX509Certificate *cert = CX509Certificate::NewLC(certPtr);
	aCertStore->iRootCerts.AppendL((CX509Certificate *)cert);	
	CleanupStack::Pop(cert);
}

/* Add certificate to the cert store */
int X509_STORE_add_cert(X509_STORE *aCertStore, X509 *aCert)
{
	TInt err;

	TRAP(err, doAddCertL(aCertStore, aCert));
	
	return err;

}

void doCertChainInitL(X509_STORE *aCertStore, STACK_OF(X509) *aCert)
{
	if (aCertStore->iCertChain)
	{
		delete aCertStore->iCertChain;
		aCertStore->iCertChain = NULL;
	}
	aCertStore->iCertChain = CSymbianCertChain::NewL();
	aCertStore->iCertChain->InitializeL((unsigned char*)aCert->der, 
	                                                    (unsigned int)aCert->derlen, 
	                                                    aCertStore->iRootCerts);
}

/* Init certchain using certs from iRootCerts */
int X509_STORE_certchain_init (X509_STORE *aCertStore, STACK_OF(X509) *aCert)
{
	TInt err;

	TRAP(err, doCertChainInitL(aCertStore, aCert));
    if ( err != KErrNone )
        {
        xmlSecSetErrorFlag( err );            
        }
	return err;		   		
}

void doCertChainInitfromCertStoreL(X509_STORE *aCertStore, STACK_OF(X509) *aCert)
{
        TInt numCert ;
        RMPointerArray<CCTCertInfo> listCerts;        
        HBufC8* certdata;
        CX509Certificate* certX509;
        
        if (aCertStore->iCertChain)
            {
            delete aCertStore->iCertChain;
            aCertStore->iCertChain = NULL;
            }   
        if(!aCertStore->iSymbianCertStore)
            {
            aCertStore->iSymbianCertStore = CSymbianCertStore::NewL();
            aCertStore->iSymbianCertStore->CreateUnifiedCertStoreL();
            CActiveScheduler::Start();   
            User::LeaveIfError( aCertStore->iSymbianCertStore->GetError() );
            }
       if (aCertStore->iRootCertsSCS.Count())
            {
            aCertStore->iRootCertsSCS.ResetAndDestroy();    
            }
        aCertStore->iSymbianCertStore->ListCertL();
        CActiveScheduler::Start();
        User::LeaveIfError( aCertStore->iSymbianCertStore->GetError() );
        listCerts=aCertStore->iSymbianCertStore->GetCertList();
        numCert = listCerts.Count();
   	
        for (int i=0;i<numCert;i++)
            {
            CCTCertInfo* cert = (CCTCertInfo *)listCerts[i];          
            aCertStore->iSymbianCertStore->SetCert(cert);      
            aCertStore->iSymbianCertStore->RetrieveCertDataL();
            CActiveScheduler::Start();
            User::LeaveIfError( aCertStore->iSymbianCertStore->GetError() );
            certdata=aCertStore->iSymbianCertStore->GetRetrieveCertData();      
            certX509 = CX509Certificate::NewLC(*certdata);
            User::LeaveIfError(aCertStore->iRootCertsSCS.Append((CX509Certificate *)certX509));     
            CleanupStack::Pop(certX509);   
            }	
	aCertStore->iCertChain = CSymbianCertChain::NewL();
	aCertStore->iCertChain->InitializeL((unsigned char*)aCert->der, 
	                                                (unsigned int)aCert->derlen,
	                                                aCertStore->iRootCertsSCS);
}

/*Init certchain using root certs from SymbianCertStore stored in iRootCertsSCS */
int X509_STORE_certchain_init_fromCertStore (X509_STORE *aCertStore, STACK_OF(X509) *aCert)
{
	TInt err;
	TRAP(err, doCertChainInitfromCertStoreL(aCertStore, aCert));
	if ( err != KErrNone )
	    {
	    xmlSecSetErrorFlag( err );
	    }
	return err;		   		
}

int X509_STORE_certchain_validate (X509_STORE *aCertStore)
{
	TInt err;

	TRAP(err, aCertStore->iCertChain->ValidateL());
	
	if (err==KErrNone)
	    {
		CActiveScheduler::Start();
		err = aCertStore->iCertChain->GetError();
		if ( err != KErrNone )
		    {
		    xmlSecSetErrorFlag( err );
		    }
	    }
	else
	    {
	    xmlSecSetErrorFlag( err );
	    }
	
	return err;
		   		
}


int X509_STORE_certchain_getValidateResult (X509_STORE *aCertStore)
{
	TInt ret;

	ret = aCertStore->iCertChain->GetValidateResult();
	
	return ret;
		   		
}

#endif /* XMLSEC_NO_X509 */
