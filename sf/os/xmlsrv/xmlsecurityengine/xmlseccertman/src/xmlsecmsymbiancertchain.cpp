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
* Description: An XmlSec interface to the Symbian Unified Certificate Store       
*
*/

#include "xmlsecmsymbiancertchain.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
CSymbianCertChain::CSymbianCertChain()
:	CActive( EPriorityStandard ),
	iCertChain( NULL ),
	iValidationResult( NULL ),
	iState( EUnitialized )
    {
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
// 
void CSymbianCertChain::ConstructL()
    {
    User::LeaveIfError(iFs.Connect()); 
    CActiveScheduler::Add(this);    
    }

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//   
EXPORT_C CSymbianCertChain* CSymbianCertChain::NewL()
    {
    CSymbianCertChain* self = new( ELeave ) CSymbianCertChain;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//     
EXPORT_C CSymbianCertChain::~CSymbianCertChain()
    {
   	Cancel(); 
    
    if (iCertChain)  
        {
    	delete iCertChain;
        }
    
    if (iValidationResult)
        {
    	delete iValidationResult;
        }

    iFs.Close();  
    }

// -----------------------------------------------------------------------------
// RunL
// Handles an active object's request completion event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSymbianCertChain::RunL()
    {
    TInt errorCode = iStatus.Int();
    if ( errorCode ) 
        {
    	User::Leave(errorCode);
        }

    switch(iState)
        {
        case EValidate:
    	    CActiveScheduler::Stop();
    	    break;
	    default:
            break;

        }
    }   

// -----------------------------------------------------------------------------
// DoCancel
// This function is called as part of the active object's Cancel().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//         
void CSymbianCertChain::DoCancel()
    {
    }
         
// -----------------------------------------------------------------------------
// CSymbianCertStore::RunError
// Handles Leaves from RunL function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CSymbianCertChain::RunError(TInt aError)
	{
	iError=aError;
         CActiveScheduler::Stop();	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// InitializeL
// Creates the CPKIXCertChain
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianCertChain::InitializeL(
	TUint8 *aEncodedCerts,			// One or more concatenated DER encoded X.509 certificates
	TUint aEncodedCertsLen,		// Length of the DER encoded X.509 certificates
	const RPointerArray< CX509Certificate > &aRootCerts)  // An array of certificates which the chain will treat as candidate root certificates
    {

	TPtrC8 certPtr(aEncodedCerts, aEncodedCertsLen);  
	
	if (iCertChain)
    	{
		delete iCertChain;
		iCertChain = NULL;
	    }

    iCertChain = CPKIXCertChain::NewL(iFs, certPtr, aRootCerts);    
    }

// -----------------------------------------------------------------------------
// ValidateL
// Validate the certificate
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianCertChain::ValidateL()
    {
	// iValidationResult will contain the result of the validation
	__ASSERT_ALWAYS(iCertChain, User::Leave(KErrGeneral));
	
	if (iValidationResult)
	    {
		delete iValidationResult;
		iValidationResult = NULL;
	    }
	
	TTime validationTime;
	validationTime.HomeTime();
	
	iValidationResult = CPKIXValidationResult::NewL();
	
    iCertChain->ValidateL(*iValidationResult, validationTime, iStatus);

    iState = EValidate;
    SetActive();    
    }
    
// -----------------------------------------------------------------------------
// GetValidateResult
// Get the result of the validation
// Returns: EValidatedOK (0) if validation succeeds
//			-1 if no result can be fetched
//			enum TValidationError if validation fails
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSymbianCertChain::GetValidateResult()	
    {
    // iValidationResult contains the result of the validation	
	if (!iValidationResult)
		return -1;
		
	return iValidationResult->Error().iReason; 

    }
    
// -----------------------------------------------------------------------------
// GetError
// Get the error flag
// Returns: error code
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSymbianCertChain::GetError()
    {
    return iError;
    }
