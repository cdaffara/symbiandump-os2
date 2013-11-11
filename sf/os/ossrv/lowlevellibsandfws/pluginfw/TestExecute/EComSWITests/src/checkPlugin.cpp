// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This File Provides implementations of CCheckListImplementation, which Checks the plugin for availble implementation
// and CCheckCreateImplementation, which uses the method provided the ECom Plugin.
// 
//

/**
 @file
*/

#include <ecom/resolver.h>
#include "checkPlugin.h"


CCheckListImplementation::~CCheckListImplementation()
	{
	}


CCheckListImplementation::CCheckListImplementation(TOperationType aOperationType)
	: iOperationType(aOperationType)
	{
    SetTestStepName(KListImplUidStep);
	}
	
	
	
TVerdict CCheckListImplementation::doTestStepPreambleL()
    {
    SetTestStepResult(EPass);
    // get step parameters
	
    TInt tempInterfaceuid=0;
 
    if (!GetHexFromConfig(ConfigSection(), KInterfaceUID, tempInterfaceuid))
		{
		INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1(_L("FAIL: Missing Interface UID"));
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        return TestStepResult();
		}    
		
	 iInterfaceUid.iUid = tempInterfaceuid;
	  
    return TestStepResult();
	}
    
    
    
TVerdict CCheckListImplementation::doTestStepL()
    {
 	__UHEAP_MARK; 
  	
  	//First find out if the Check Plugin step is supposed to be successful.
  	TInt testStepOutcome=KErrNone;
  	TInt err = GetIntFromConfig(ConfigSection(), KExpectedErrorCheckPlugin, testStepOutcome);
  	if(err !=1) testStepOutcome=KErrNone;

	
	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

   
    switch (iOperationType)
        {
        case EUseListImplUid:
          	{
          	        	
	        REComSession::ListImplementationsL(iInterfaceUid, iImplyArrayPtr);
          	
          	TInt eImplCount(0);   
    		iImplCount = iImplyArrayPtr.Count();

	    	if(!GetIntFromConfig(ConfigSection(), KExpectedCount, eImplCount))
          		{
          		INFO_PRINTF1(HTML_RED);
		        ERR_PRINTF1(_L("FAIL: Missing Implementation Count in the INI File"));
		        INFO_PRINTF1(HTML_RED_OFF);
		        SetTestStepResult(EFail);
		        return TestStepResult();
               	}

          	if(eImplCount != iImplCount)
          		{
          		INFO_PRINTF1(HTML_RED);
		       	ERR_PRINTF3( _L("The number Implemenation(s) in this ECom plugin do not match, expected %d implementation(s), got %d implementation(s)."), eImplCount, iImplCount );
        		INFO_PRINTF1(HTML_RED_OFF);
		        SetTestStepResult(EFail);
		        return TestStepResult();
          		}

          	//Ensuring that the ECom Plugin does have Implementations
	   		if( iImplCount > 0 && testStepOutcome == KErrNone)
	          	{
	          	TInt arrayIndex(0);
	          	//Checking Implementation UID
         		TUid eImplUid = KNullUid;
		        TInt tempImplUid(0);
		        
		        if (!GetHexFromConfig(ConfigSection(), KImplUID, tempImplUid))
					{
					INFO_PRINTF1(HTML_RED);
		        	ERR_PRINTF1(_L("FAIL: Missing Implementtaion UID"));
		        	INFO_PRINTF1(HTML_RED_OFF);
		        	SetTestStepResult(EFail);
		        	return TestStepResult();
					}
						
				eImplUid.iUid = tempImplUid;
				
	          	TBool foundImplementationUid(EFalse);
		          			          	
		        for(TInt i = 0; (i < iImplCount)  && !(foundImplementationUid); i++)
		        	{
		          	iImplUid = iImplyArrayPtr[i]->ImplementationUid();
		          		
		          	if(eImplUid == iImplUid)
		          		{
		          		foundImplementationUid = ETrue;
		          		arrayIndex = i;
		          		}
	       			}
		          	
	          	if(eImplUid != iImplUid)
	          		{
	          		INFO_PRINTF1(HTML_RED);
			       	ERR_PRINTF3( _L("The Implemenation UID  in this ECom plugin do not match, expected  %d Implementation UID  , got %d iImplementation UID."), eImplUid, iImplUid );
	        		INFO_PRINTF1(HTML_RED_OFF);
			        SetTestStepResult(EFail);
			        return TestStepResult();
	          		}
		  		
		  		//Checking Implementation's Version Numbers
	          	TInt eVersion(0);
	          		          	
		        if (!GetIntFromConfig(ConfigSection(), KExpectedVersion, eVersion))
		           	{
		         	INFO_PRINTF1(HTML_RED);
				    ERR_PRINTF1(_L("FAIL: Missing Version Number in the INI File"));
				    INFO_PRINTF1(HTML_RED_OFF);
				    SetTestStepResult(EFail);
				    return TestStepResult();
		            }
		            
				iVersion = iImplyArrayPtr[arrayIndex]->Version();
		            
		     	if(eVersion != iVersion)
		         	{
		          	INFO_PRINTF1(HTML_RED);
				   	ERR_PRINTF3( _L("The ECom plugin's implementation versions  do not match, expected %d version, got %d version."), eVersion, iVersion );
		        	INFO_PRINTF1(HTML_RED_OFF);
				    SetTestStepResult(EFail);
				    return TestStepResult();
		          	}
		          	
		          	
		        //Checking Implementation's Drive Letter
		        TDriveUnit eDrive;
		        TPtrC tempDrive;
		        
	        	iDrive = iImplyArrayPtr[arrayIndex]->Drive();
		         
		    	if (!GetStringFromConfig(ConfigSection(), KExpectedDrive, tempDrive))
		           	{
		         	INFO_PRINTF1(HTML_RED);
				    ERR_PRINTF1(_L("FAIL: Missing Plugin Drive Letter in the INI File"));
				    INFO_PRINTF1(HTML_RED_OFF);
				    SetTestStepResult(EFail);
				    return TestStepResult();
		        	}
		          	
		        eDrive = tempDrive;
		        		        
		     	if(eDrive!= iDrive)
		        	{
		        	TPtrC eDrv(eDrive.Name());
			        TPtrC iDrv(iDrive.Name());
		        	INFO_PRINTF1(HTML_RED);
				   	ERR_PRINTF3( _L("The ECom plugin's drive letters do not match, expected %S drive, got %S drive."), &eDrv, &iDrv);
				   	INFO_PRINTF1(HTML_RED_OFF);
				    SetTestStepResult(EFail);
				    return TestStepResult();
		        	}
		          	
		        //Checking Implementation's Vendor ID
		        TInt tempVendorID(0);
		        TUid eVendorID = KNullUid; 
								
		        iVendorID = iImplyArrayPtr[arrayIndex]->VendorId();
				  	    		
	    		if (!GetHexFromConfig(ConfigSection(), KExpectedVendorID, tempVendorID))
	    			{
					INFO_PRINTF1(HTML_RED);
			        ERR_PRINTF1(_L("FAIL: Missing vendor UID In the INI File"));
			        INFO_PRINTF1(HTML_RED_OFF);
			        SetTestStepResult(EFail);
			        return TestStepResult();
					}    
		    		
		    	eVendorID.iUid = tempVendorID;
		    		
		    		if(eVendorID != iVendorID)
		          		{
		          		INFO_PRINTF1(HTML_RED);
				       	ERR_PRINTF3( _L("The ECom Vendor IDs do not match, expected %d VID, got %d VID."), eVendorID, iVendorID );
		        		INFO_PRINTF1(HTML_RED_OFF);
				        SetTestStepResult(EFail);
				        return TestStepResult();
		          		}
		          
	          	}
	          		
	          else
	          	{
	          	INFO_PRINTF1(HTML_RED);
	          	INFO_PRINTF2(_L("This is a Negative test whose expected outcome is %d "), testStepOutcome);
	          	INFO_PRINTF1(HTML_RED_OFF);         	
	          	}
          	
           	}
     
        }
    
   	//Reseting The ECOM Array
   	iImplyArrayPtr.ResetAndDestroy();     
   	
   	//Closing session with ECom Server
    REComSession::FinalClose();
    
   	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
 
 	if(startThreadHandleCount != endThreadHandleCount)
 		{
 		INFO_PRINTF1(HTML_RED);
		ERR_PRINTF1( _L("There is a memory leak as the number of thread handles open at the start do not equal the number of handles opened at the end."));
        ERR_PRINTF3(_L("At the start there %d handles open and the end there are %d"), startThreadHandleCount, endThreadHandleCount);
        INFO_PRINTF1(HTML_RED_OFF);
		SetTestStepResult(EFail);
		return TestStepResult();
 		}
 
    thisThread.Close();
    __UHEAP_MARKEND;    
	
	return TestStepResult();
	}
    




TVerdict CCheckListImplementation::doTestStepPostambleL()
	{
	 return TestStepResult();
	}




CCheckCreateImplementation::CCheckCreateImplementation(TOperationType aOperationType)
	: iOperationType(aOperationType)
	{
    SetTestStepName(KCreateImpl_ImpId_DtorId_Step);
	}

TVerdict CCheckCreateImplementation::doTestStepPreambleL()
    {
    SetTestStepResult(EPass);
   
   TInt tempImplUid=0;
   
    if (!GetHexFromConfig(ConfigSection(), KImplUID, tempImplUid))
		{
		INFO_PRINTF1(HTML_RED);
        ERR_PRINTF1(_L("FAIL: Missing Implementation UID"));
        INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        return TestStepResult();
		}    
		
	iImplUid.iUid = tempImplUid;

    return TestStepResult();
	}


TVerdict CCheckCreateImplementation::doTestStepL()
 	{
 	__UHEAP_MARK; 
	
	
	// Check for Memory leaks and also leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

		
	TInt eInt(0);
	
	CEComSwiPluginInterfaceDefiniton* iPluginPtr = CEComSwiPluginInterfaceDefiniton::NewL(iImplUid);
	
	iPluginPtr->ReturnInt(iInt);
	
	
	 if (!GetIntFromConfig(ConfigSection(), KExpectedPluginInt, eInt))
       	{
     	INFO_PRINTF1(HTML_RED);
	    ERR_PRINTF1(_L("FAIL: Missing ReturnInt value Number in the INI File"));
	    INFO_PRINTF1(HTML_RED_OFF);
	    SetTestStepResult(EFail);
	    return TestStepResult();
        }
        
        
      if(eInt != iInt)
  		{
  		INFO_PRINTF1(HTML_RED);
       	ERR_PRINTF3(_L("The values returned from the Plugins ReturnInt() do not match, expected Int Value %d, got %d."), eInt, iInt);
		INFO_PRINTF1(HTML_RED_OFF);
        SetTestStepResult(EFail);
        return TestStepResult();
  		}

  	delete iPluginPtr;		

    REComSession::FinalClose();   

	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
 
 	if(startThreadHandleCount != endThreadHandleCount)
 		{
 		INFO_PRINTF1(HTML_RED);
		ERR_PRINTF1( _L("There is a memory leak as the number of thread handles open at the start do not equal the number of handles opened at the end."));
        ERR_PRINTF3(_L("At the start there %d handles open and the end there are %d"), startThreadHandleCount, endThreadHandleCount);
        INFO_PRINTF1(HTML_RED_OFF);
		SetTestStepResult(EFail);
		return TestStepResult();
 		}
 
    thisThread.Close();

    __UHEAP_MARKEND;    
	
	return TestStepResult();
	}



TVerdict CCheckCreateImplementation::doTestStepPostambleL()
	{
	 return TestStepResult();
	}

CCheckCreateImplementation::~CCheckCreateImplementation()
	{
	}

/** Test step to issue a custom resolver list request so that
the resolver is cached. */

// constructor
CUseCustomResolver::CUseCustomResolver()
	{
    SetTestStepName(KUseCustomResolverStep);
	}

// destructor
CUseCustomResolver::~CUseCustomResolver()
	{
	}

// doTestStepL
TVerdict CUseCustomResolver::doTestStepL()
	{
	TUid resolverUid = KNullUid;
	TInt tempImplUid(0);
	TESTL(GetHexFromConfig(ConfigSection(), KImplUID, tempImplUid));

	resolverUid.iUid = tempImplUid;
	TEComResolverParams resolverparams;
	_LIT8(KDummyData,"dummy");
	resolverparams.SetDataType(KDummyData);
	RImplInfoPtrArray ifArray;
	TRAPD(err, REComSession::ListImplementationsL(KCustomResolverInterfaceUid,
		resolverparams, resolverUid, ifArray) );
	if (err != KErrNone)
		{
		// In particular looking for KEComErrMismatchedTags
        INFO_PRINTF1(HTML_RED);
		ERR_PRINTF2( _L("Custom Resolver list request failed with %d"), err);
        INFO_PRINTF1(HTML_RED_OFF);
		SetTestStepResult(EFail);
		return EFail;
		}

	_LIT(KExpectedVersion, "expectedversion");
	TInt expectedVersion;
  	if (GetIntFromConfig(ConfigSection(), KExpectedVersion, expectedVersion))
		{
		for (TInt i = 0; i < ifArray.Count(); i++)
			{
			const CImplementationInformation& impl = *ifArray[i];
			if (impl.ImplementationUid() == resolverUid)
				{
				if (impl.Version() !=  expectedVersion)
					{
					INFO_PRINTF1(HTML_RED);
					ERR_PRINTF3( _L("Expecting ver %d, got %d"), expectedVersion, impl.Version());
					INFO_PRINTF1(HTML_RED_OFF);
					SetTestStepResult(EFail);
					}
				break;
				}
			}
		}
	ifArray.ResetAndDestroy();
    REComSession::FinalClose();
	return TestStepResult();
	}
