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





#include <mctcertstore.h>



#include <x509cert.h>



#include <charconv.h>



#include <utf.h>







#include "xmlsecmsymbiancertstore.h"



// -----------------------------------------------------------------------------



// FindMatchedCert 



// Find the cert that matched iCertLabel from iCerts. 



// The cert found will be stored in iCert.



// -----------------------------------------------------------------------------



//



void CSymbianCertStore::FindMatchedCert()



    {



    TInt numCert = iCerts.Count();



    // Reset iCert



    if (iCert)



        {



        //delete iCert;	



        iCert->Release();



        iCert = NULL;



        }	



        // No label to be found



        if (!iCertLabel)



            return;			



        for (int i=0;i<numCert;i++)



            {



            CCTCertInfo* cert = (CCTCertInfo *)iCerts[i];



            if (iCertLabel->Compare(cert->Label()) == 0)



                {



                iCert = cert;



                }			



            }	



            ResetAndDestroyCertsArray();     //iCerts



    }



    



// ---------------------------------------------------------------------------



// Constructor



// ---------------------------------------------------------------------------



// 



CSymbianCertStore::CSymbianCertStore()



:	CActive( EPriorityStandard ),



	iState( EUnitialized )



    {



    }    



    



// ---------------------------------------------------------------------------



// Second phase constructor



// ---------------------------------------------------------------------------



// 



void CSymbianCertStore::ConstructL()



    {



    User::LeaveIfError(iFs.Connect()); 



    CActiveScheduler::Add(this);



    }



    



// ---------------------------------------------------------------------------



// Two phase constructor



// ---------------------------------------------------------------------------



//   



EXPORT_C CSymbianCertStore* CSymbianCertStore::NewL()



    {



    CSymbianCertStore* self = NewLC();



    CleanupStack::Pop(self);



    return self;



    }



    



// ---------------------------------------------------------------------------



// Two phase constructor



// ---------------------------------------------------------------------------



//   



EXPORT_C CSymbianCertStore* CSymbianCertStore::NewLC()



    {



    CSymbianCertStore* self = new( ELeave ) CSymbianCertStore;



    CleanupStack::PushL( self );



    self->ConstructL();



    return self;



    }



    



// ---------------------------------------------------------------------------



// Destructor



// ---------------------------------------------------------------------------



//    



CSymbianCertStore::~CSymbianCertStore()



    {



    Cancel();



    ResetAndDestroyCertsArray();     //iCerts



    // Free memory



        delete iCertFilter;



        delete iCertLabel;



        delete iCertData;



        delete iCertObject;



    if (iCert)



        {



        iCert->Release();



        }



	



        delete iCertStore;



    iFs.Close();	



    }



    



// -----------------------------------------------------------------------------



// Release all resources kept in iCerts array (exept for iCert which is released 



// separetly) and empty iCerts array



// -----------------------------------------------------------------------------



//



 void CSymbianCertStore::ResetAndDestroyCertsArray()



    {    



    TInt count = iCerts.Count();        



    for ( int i=0; i<count; ++i )



        {



        CCTCertInfo* cert = iCerts[i];



        if ( cert != iCert )



            {



            cert->Release();



            }



        }	    



        iCerts.Reset();            



    }



// -----------------------------------------------------------------------------



// Release all resources kept in iCerts array  and empty iCerts array



// -----------------------------------------------------------------------------



//



 void CSymbianCertStore::ResetCertsList()



    {    



    TInt count = iCerts.Count();        



    for ( int i=0; i<count; ++i )



        {



        CCTCertInfo* cert = iCerts[i];



            cert->Release();



        }	    



        iCerts.Reset();            



    }    



// -----------------------------------------------------------------------------



//RunL



// Handles an active object's request completion event.



// (other items were commented in a header).



// -----------------------------------------------------------------------------



//



void CSymbianCertStore::RunL()



    {



    if (iStatus!=KErrNone) 



        {



    	TInt err = iStatus.Int();



    	User::Leave(iStatus.Int());



        }



    switch(iState)



        {



        case EInitializingCertStore:



    	            CActiveScheduler::Stop();



    	            break;



        case EFindingCerts:



    	            FindMatchedCert();



    	            CActiveScheduler::Stop();   



                     break;



        case EListCerts:



    	            CActiveScheduler::Stop();   



                     break;		



        case EAddingCert:



                    CActiveScheduler::Stop();



                    break;     



        case ERetrievingCertData:



                    CActiveScheduler::Stop();



                    break;



        case ERetrievingCertObject:



                    CActiveScheduler::Stop();



                    break;



        }



    }



    



// -----------------------------------------------------------------------------



// DoCancel



// This function is called as part of the active object's Cancel().



// (other items were commented in a header).



// -----------------------------------------------------------------------------



//  



void CSymbianCertStore::DoCancel()



    {



    }



    



// -----------------------------------------------------------------------------



// RunError



// Handles Leaves from RunL function.



// (other items were commented in a header).



// -----------------------------------------------------------------------------



// 



TInt CSymbianCertStore::RunError(TInt aError)



	{



	iError=aError;



	CActiveScheduler::Stop();



	return KErrNone;



	}



	



// -----------------------------------------------------------------------------



// CreateUnifiedCertStoreL



// Create the Unified Cert Store structure iCertStore



// -----------------------------------------------------------------------------



//



EXPORT_C void CSymbianCertStore::CreateUnifiedCertStoreL()



    {



    if (iCertStore)



        {



        delete iCertStore;    



        iCertStore = NULL;



        }



  



    iCertStore = CUnifiedCertStore::NewL(iFs,ETrue);



    iCertStore->Initialize(iStatus);



    iState = EInitializingCertStore;



    SetActive();



    // RunL() called when this completes



    }



    



// -----------------------------------------------------------------------------



// FindCert 



// Lists certs from Unified Cert Store in  iCerts with label



// -----------------------------------------------------------------------------



//



EXPORT_C void CSymbianCertStore::FindCertL(const TDesC8 &aLabel)



    {



    if(iCertFilter)



        {



        delete iCertFilter;



        iCertFilter = NULL;



        }



    iCertFilter = CCertAttributeFilter::NewL();     



    iCertFilter->SetFormat(EX509Certificate);



    iCertFilter->SetOwnerType(ECACertificate);		



    // Store aLabel



    if (iCertLabel)



        {



    	delete iCertLabel;



    	iCertLabel = NULL;



        }



    TRAPD(err,iCertLabel = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aLabel));



    if (err != KErrNone)



        {



        iError=err;



        }



    iCertStore->List(iCerts, *iCertFilter, iStatus);



    iState = EFindingCerts;



    SetActive();



    // RunL() called when this completes



    }



    



// -----------------------------------------------------------------------------



// ListCert 



// List certs from Unified Cert Store in iCerts. 



// The found certs will be stored in iCerts



// -----------------------------------------------------------------------------



//



EXPORT_C void CSymbianCertStore::ListCertL()



    {



    if(iCertFilter)



        {



        delete iCertFilter;



        iCertFilter = NULL;



        }



    if(iCerts.Count())



        {



       ResetCertsList();



        }



    iCertFilter = CCertAttributeFilter::NewL();     



    iCertFilter->SetFormat(EX509Certificate);



    iCertFilter->SetOwnerType(ECACertificate);		



       



    iCertStore->List(iCerts, *iCertFilter, iStatus);



    iState = EListCerts;



    SetActive();



    // RunL() called when this completes



    }



    



// -----------------------------------------------------------------------------



// GetCertsList 



// Returns  certs list from SymbianCertStore. 				  



// Returns: RMPointerArray<CCTCertInfo> list



// -----------------------------------------------------------------------------



//



EXPORT_C RMPointerArray<CCTCertInfo> CSymbianCertStore::GetCertList()



    {



    return iCerts;



    }   



    



// -----------------------------------------------------------------------------



// hasCert  



// Check if a cert is found in the Unified Cert Store				  



// Returns: KErrNone if iCert is set 



//                  KErrNotFound in other case



// -----------------------------------------------------------------------------



EXPORT_C TInt CSymbianCertStore::hasCert()



    {



	if (iCert)



		return KErrNone;



	else



		return KErrNotFound;



    }     



    



// -----------------------------------------------------------------------------



// SetCert 



// Sets iCert. 				  



// -----------------------------------------------------------------------------



//



EXPORT_C void CSymbianCertStore::SetCert(CCTCertInfo* cert)



    {



    iCert=cert;



    }        



    



// -----------------------------------------------------------------------------



// GetCertSize 



// Returns size of the cert.			  



// Returns: TUint size



// -----------------------------------------------------------------------------



//



EXPORT_C TUint CSymbianCertStore::GetCertSize()



    {



	// assert iCert



	return iCert->Size();



    }



    



// -----------------------------------------------------------------------------



// AddCertL 



// Add cert  to Unified Cert Store



// -----------------------------------------------------------------------------



//



EXPORT_C void CSymbianCertStore::AddCert(



    const TDesC8 &aCertData, 



    const TDesC8 &aCertName)		



    {



    if (iCert)



        { 



        //delete iCert;



        iCert->Release();



        iCert = NULL;



        }   



    //iWritableCertStore=&(iCertStore->WritableCertStore(0));



    TInt num = iCertStore->WritableCertStoreCount();



	TBool found = EFalse;



   	   



	for (TInt i = 0;i<num;i++)



		{



   		iWritableCertStore = &(iCertStore->WritableCertStore(i));



		MCTToken& token = iWritableCertStore->Token();



		TUid tokenuid = token.Handle().iTokenTypeUid;



   		if ( tokenuid == TUid::Uid(KTokenTypeFileCertstore) ) // Symbian's file cert store, defined in mctcertstore.h



   			{



   	    		found = ETrue;



			break;



   	    	}



		}	







	if ( !found && (num > 0) )



		{



		// Not found, let's use the first one as a default anyway



   		iWritableCertStore = &(iCertStore->WritableCertStore(0));



		}	



   



    // Convert key name from TDesC8 to TDesC16



    HBufC16* unicodeCertName=NULL;



    TRAPD(err,unicodeCertName=CnvUtfConverter::ConvertToUnicodeFromUtf8L(aCertName));



    if (err != KErrNone)



        {



        iError=err;



        }



    iWritableCertStore->Add(*unicodeCertName, 



                                        EX509Certificate,



                                        ECACertificate, 



                                        NULL, 



                                        NULL, 



                                        aCertData, 



                                        iStatus);



    delete unicodeCertName;



    iState = EAddingCert;



    SetActive();



    // RunL() called when this completes



    }    



       



// -----------------------------------------------------------------------------



// RetrieveCertDataL 



// Sets iCertData from iCert



// -----------------------------------------------------------------------------



//



EXPORT_C void CSymbianCertStore::RetrieveCertDataL()



    {



    if(!iCert)



       User::Leave(KErrNotFound);



    if(iCertData)



        {



        delete iCertData;



        iCertData = NULL;



        }



    iCertData = HBufC8::NewMaxL(iCert->Size()); 



    TPtr8 data=iCertData->Des();



    iCertStore->Retrieve(*iCert, data, iStatus);



    iState = ERetrievingCertData;



    SetActive();



    // RunL called when this completes



    }



    



// -----------------------------------------------------------------------------



// GetRetrieveCertData 



// Returns  cert data. 				  



// Returns: HBufC8* iCertData



// -----------------------------------------------------------------------------



//



EXPORT_C HBufC8* CSymbianCertStore::GetRetrieveCertData()



    {



    return iCertData;



    }    



        



// -----------------------------------------------------------------------------



// RetrieveCertObjectL 



// Set iCertObject from iCert



// -----------------------------------------------------------------------------



//



EXPORT_C void CSymbianCertStore::RetrieveCertObjectL()



    {



    if(!iCert)



        User::Leave(KErrNotFound);



    if(iCertObject)



        {



        delete iCertObject;



        iCertObject = NULL;



        }



    iCertStore->Retrieve(*iCert, iCertObject, iStatus);



    iState = ERetrievingCertObject;



    SetActive();



    // RunL called when this completes



    }



    



// -----------------------------------------------------------------------------



// GetRetrieveCertObject 



// Returns  cert object. 				  



// Returns: CCertificate*  iCertObject



// -----------------------------------------------------------------------------



//



EXPORT_C CCertificate* CSymbianCertStore::GetRetrieveCertObject()



    {



    return iCertObject;



    }  



    



// -----------------------------------------------------------------------------



// GetError



// Get the error flag



// Returns: error code



// -----------------------------------------------------------------------------



//



EXPORT_C TInt CSymbianCertStore::GetError()



    {



    return iError;



    }













