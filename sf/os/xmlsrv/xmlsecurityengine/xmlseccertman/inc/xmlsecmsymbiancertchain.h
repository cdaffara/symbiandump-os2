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
* Description: An XmlSec interface to the Symbian PKIXCertChain.       
*
*/

#ifndef __XMLSECCERTMAN_SYMBIANCERTCHAIN_H__
#define __XMLSECCERTMAN_SYMBIANCERTCHAIN_H__

// INCLUDES
#include  <e32base.h>
#include  <pkixcertchain.h>

class CSymbianCertChain : public CActive
    {
public:  // Constructors and destructor
        
    /**
     * Create new object.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */
    IMPORT_C static CSymbianCertChain* NewL();
        
    /**
     * Destructor.
     */
    IMPORT_C virtual ~CSymbianCertChain();

    /**
     * Create the PKIXCertChain iCertChain
     *
     * @since S60 v3.2
     * @param aEncodedCerts One or more concatenated DER encoded X.509 certificates in TUint8 format
     * @param aEncodedCertsLen Length of the DER encoded X.509 certificates
     * @param aRootCerts An array of certificates which the chain will treat as candidate root certificates
     */        
    IMPORT_C void CSymbianCertChain::InitializeL(TUint8 *aEncodedCerts, 
                                            TUint aEncodedCertsLen, 
                                            const RPointerArray< CX509Certificate > &aRootCerts);
 
    /**
     * Call ValidateL() of the iCertChain
     * @since S60 v3.2 
     */         
    IMPORT_C void CSymbianCertChain::ValidateL();
 		
    /**
     * Get the validation result
     *
     * @since S60 v3.2
     * @return EValidatedOK (0) if validation succeeds
     * @return -1 if no result can be fetched
     * @return enum TValidationError if validation fails
     */     		
    IMPORT_C TInt CSymbianCertChain::GetValidateResult();
    
    /**
     * Get the error flag
     *
     * @since S60 v3.2
     * @return error code
     */    		
    IMPORT_C TInt CSymbianCertChain::GetError();
        
protected:
    /**
     * From CActive Callback function, invoked to handle responses from the server
     */
    void RunL();
    
    /**
     * This function is called as part of the active object's Cancel().
     */      
    void DoCancel();        
   
    /**
     * Handles Leaves from RunL function.
     */    
    TInt RunError(TInt aError);

private:
    /**
    * C++ default constructor.
    */
    CSymbianCertChain();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    /** 
     * State of active object. 
     * EUnitialized before validation
     * EValidate after validation
     */ 	
    enum TState
        {
        EUnitialized,
        EValidate
        };
		
    /**
     * A reference to the cert chain
     */
    CPKIXCertChain *iCertChain;

    /** 
     * Contain result of the validation
     */
    CPKIXValidationResult *iValidationResult;
		
    /**
     * An internal state
     */
    TState iState;        
        
    /**
     * A reference to the File Server Client
     */
    RFs iFs;
    
    /**
     * Error flag 
     */		
    TInt iError;
    };

#endif      // __XMLSECCERTMAN_SYMBIANCERTCHAIN_H__  
