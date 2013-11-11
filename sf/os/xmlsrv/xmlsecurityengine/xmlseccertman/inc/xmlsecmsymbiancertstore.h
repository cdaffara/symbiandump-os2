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
* Description: An XmlSec interface to the Symbian Unified Certificate Store.       
*
*/

#ifndef __XMLSECCERTMAN_SYMBIANCERTSTORE_H_
#define __XMLSECCERTMAN_SYMBIANCERTSTORE_H__

#include <e32base.h>
#include <unifiedcertstore.h>
#include <e32std.h>
#include <signed.h>
#include <mctwritablecertstore.h>

class CSymbianCertStore : public CActive
    {
public:        
  
    /**
     * Create new object.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */
    IMPORT_C static CSymbianCertStore* NewL();
    
    /**
     * Create new object and put it on clean stack.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */    
     IMPORT_C static CSymbianCertStore* NewLC();
        
    /**
     * Destructor.
     */
    virtual ~CSymbianCertStore();
        
    /**
     * Create Unified Cert Store
     *
     * @since  S60 v3.2
     */
    IMPORT_C void CreateUnifiedCertStoreL();
            
    /**
     * Find a cert in the Unified Cert Store
     *
     * @since S60 v3.2 
     * @param aLabel The name of the cert to be found
     */
    IMPORT_C void FindCertL(const TDesC8 &aLabel);
        
    /**
     * List certs from the Unified Cert Store
     *
     * @since S60 v3.2 
     * @param aLabel The name of the cert to be found
     */
    IMPORT_C void ListCertL();
    
    /**
     * Get list of certs from SymbianCertStore
     *
     * @since  S60 v3.2
     * @return RMPointerArray<CCTCertInfo> list
     */            
    IMPORT_C RMPointerArray<CCTCertInfo> GetCertList();
  
    /**
     * Check if a cert is found in the Unified Cert Store
     *
     * @since  S60 v3.2
     * @return KErrNone if the cert is found
     * @return KErrNotFound if the cert is not found
     */     
    IMPORT_C TInt CSymbianCertStore::hasCert();
         
    /**
     * Sets iCert
     *
     * @since  S60 v3.2
     */  
    IMPORT_C void SetCert(CCTCertInfo* cert);

    /**
     * Get the size of the cert stored
     *
     * @since  S60 v3.2
     * @return TUint Size of the cert
     */            
    IMPORT_C TUint GetCertSize();
        
    /**
     * AddCert
     *
     * @since  S60 v3.2
     * @param aCertData The cert data
     * @param aCertName Name of the cert 
     */
    IMPORT_C void AddCert (const TDesC8 &aCertData, const TDesC8 &aCertName);
 
     /**
     * Retrieve Cert Data. Sets iCertData from iCert
     *
     * @since  S60 v3.2 
     */
    IMPORT_C void RetrieveCertDataL();
    
    /**
     * Get Cert Data.
     *
     * @since  S60 v3.2
     * @return the buffer with cert data 
     */      
    IMPORT_C HBufC8* GetRetrieveCertData();
     
     /**
     * Retrieve Cert Object. Sets iCertObject from iCert
     *
     * @since  S60 v3.2 
     */        
    IMPORT_C void RetrieveCertObjectL();
    
    /**
     * Get Cert Object.
     *
     * @since  S60 v3.2
     * @return reference to object type CCertificate 
     */            
    IMPORT_C CCertificate* GetRetrieveCertObject();
        
     /**
     * Get the error flag
     *
     * @since S60 v3.2
     * @return error code
     */           
    IMPORT_C TInt CSymbianCertStore::GetError();

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
    CSymbianCertStore();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Release all resources kept in iCerts array (exept for iCert
     * which is released separetly) and empty iCerts array
     */
    void ResetAndDestroyCertsArray();
    
    /**
     * Release all resources kept in iCerts array and empty iCerts array
     */
    void ResetCertsList();

    /**
     * To find matched key in the phase EFindingKeys
     */        
    void FindMatchedCert();

private:
    /** 
     * State of active object. 
     * EUnitialized standard state
     * EInitializingCertStore during initialize certstore
     * EFindingCerts during finding cert
     * EListCerts during listing certs
     * EAddingCert during adding cert
     * ERetrievingCertData during retrieving cert data
     * ERetrievingCertObject during retrieving cert object
     */ 	
    enum TState
        {
            EUnitialized,
            EInitializingCertStore,
            EFindingCerts,  
            EListCerts,
            EAddingCert,
            ERetrievingCertData,
            ERetrievingCertObject
            };

private:
    /**
     * A reference to the CUnifiedCertStore class
     */
    CUnifiedCertStore *iCertStore;
    
    /**
     * A reference to the WritableCertStore
     */
    MCTWritableCertStore *iWritableCertStore;
  
    /**
     * Store the retrived cert
     */
    CCTCertInfo *iCert;

    /** 
     * An internal state
     */
    TState iState;

    /** 
     * Store filter to search in SymbianCertStore
     */ 
   CCertAttributeFilter* iCertFilter; 
    
    /** 
     * To store the cert label
     */ 
    HBufC* iCertLabel;
   
    /** 
     * To store the cert data
     */ 
    HBufC8* iCertData;
    
    /** 
     * store the retrived parsed representation, derived object
     */
    CCertificate* iCertObject;
             
    /** 
     * Store the certs found
     */
    RMPointerArray< CCTCertInfo > iCerts;         
    
    /** 
     * A reference to the File Server Client
     */
    RFs iFs;
    
    /**
     * Error flag
     */ 
    TInt iError;
    };

#endif      // __XMLSECCERTMAN_SYMBIANCERTSTORE_H__  
