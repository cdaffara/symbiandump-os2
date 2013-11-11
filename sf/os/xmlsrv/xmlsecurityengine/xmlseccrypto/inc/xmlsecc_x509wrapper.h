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


/* A wrapper for x509.h in openssl */

#ifndef __XMLSEC_SYMBIANCRYPTO_X509_WRAPPER_H__
#define __XMLSEC_SYMBIANCRYPTO_X509_WRAPPER_H__    


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 
#include "xmlsecc_config.h"

#ifndef XMLSEC_NO_X509

#include "xmlsecc_evpwrapper.h"

/*
struct ScX509PubKeySt;
typedef struct ScX509PubKeySt X509_PUBKEY;
typedef struct ScX509St
{
	char*			der;		// Certificate in ASN.1 DER format 
	unsigned int	derlen;
	X509_PUBKEY		*extractedPublicKey;	

} X509;
*/

typedef struct ScX509CrlSt
{
	/* actual signature */
	int 		dummy;
} X509_CRL; /* X509_CRL */


typedef struct ScX509NameSt
{
	char *bytes;
	unsigned long hash; /* Keep the hash around for lookups */
} X509_NAME; /* X509_NAME */

/*
typedef struct ScX509Store
{
	int 		dummy;
} X509_STORE; // X509_STORE 
*/
struct ScX509Store;
typedef struct ScX509Store X509_STORE;


#define STACK_OF(type) type
//#define STACK_OF(type)	struct stack_st_##type

/** 
 * Free the X509 structure
 *
 * @param aCert X509 structure
 */
void X509_free(X509* aCert);

/** 
 * Free the X509_crl structure 
 *
 * @param aCrl X509_CRL structure
 */
void X509_crl_free(X509_CRL* aCrl);

/** 
 * Duplicate the X509 structure
 *
 * @param aCert X509 structure
 * @return reference to X509 duplicated structure 
 */
X509* X509_dup(X509* aCert);

/** 
 * Get the public key
 * 
 * @param aCert X509 structure
 * @return EVP_PKEY structure 
 */
EVP_PKEY* X509_get_pubkey(X509* aCert);

/** 
 * Read the certificate from DER format
 *
 * @param aBIO BIO structure
 * @return reference to X509 structure 
 */
X509* d2i_X509_bio(BIO *aBio);

/** 
 * Test the validity period from the certificate
 * 
 * @param aCert X509 structure
 *  @return validation period status
 */
int X509_test_validityPeriod(X509* aCert);

/************************************
 *                                          
 *   		X509 Store	        
 *                                                         
 ************************************/

/**
 * Add a X509 certificate to the X509_STORE
 *
 * @param aCertStore X509_STORE structure
 * @param aCert X509 structure
 * @return 0 if correct adding
 * @return error in the other hand
 */
int X509_STORE_add_cert(X509_STORE *aCertStore, X509 *aCert);

/**
 * Initialize the X509_STORE structure
 * 
 * @return reference to X509_STORE structure 
 */
X509_STORE *X509_STORE_new(void );

/**
 * Initialize the SymbianCertChain
 *
 * @param aCertStore X509_STORE structure
 * @param aCert STACK_OF(X509) structure
 * @return 0 if correct initializing
 * @return error in the other hand
 */
int X509_STORE_certchain_init (X509_STORE *aCertStore, STACK_OF(X509) *aCert);

/**
 * Initialize the SymbianCertChain with cert from SymbianCertStore
 *
 * @param aCertStore X509_STORE structure
 * @param aCert STACK_OF(X509) structure
 * @return 0 if correct initializing
 * @return error in the other hand
 */
int X509_STORE_certchain_init_fromCertStore (X509_STORE *aCertStore, STACK_OF(X509) *aCert);

/**
 * Validate the certificates 
 *
 * @param aCertStore X509_STORE structure
 * @return 0 if correct initializing
 * @return error in the other hand
 */
int X509_STORE_certchain_validate (X509_STORE *aCertStore);

/** 
 * Get the validation result: 1 - success; 0 - failed 
 *
 * @param aCertStore X509_STORE structure
 * @return 1 if validation succeed
 * @return 0 if validation failed
 */
int X509_STORE_certchain_getValidateResult (X509_STORE *aCertStore);

/** 
 * Free the X509_STORE structure 
 *
 * @param aCertStore X509_STORE structure
 */
void X509_STORE_free(X509_STORE *aCertStore);

#endif /* XMLSEC_NO_X509 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SYMBIANCRYPTO_X509_WRAPPER_H__ */

#define __SYMBIANCRYPTO_X509_WRAPPER_H__    
