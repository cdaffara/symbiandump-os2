// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Harness for Mstr test code
// 
//

/**
 @file 
 @internalComponent - Internal Symbian test code 
*/


#include <ecom/ecom.h>
#include "T_MstrStep.h"

/**
   Constructor
 */
CT_MstrStep::CT_MstrStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_MstrStep);
	}
	
/**
   Destructor
 */
CT_MstrStep::~CT_MstrStep()
	{
	}

TVerdict CT_MstrStep::doTestStepL()
	{	
	INFO_PRINTF1(_L("Test Started"));
    TInt r = KErrNone;
    r = iFs.Connect();
    if (r != KErrNone)
        {
            User::Leave(r);
        }
	__UHEAP_MARK;
	TRAPD(ret,doMenuL());		
	TEST(ret==KErrNone);
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	iFs.Close();

	INFO_PRINTF1(_L("Test Finished"));
	return TestStepResult();
	}
	
/**
   @SYMTestCaseID		T_MstrStep_doMenuL
  
   @SYMPREQ			
  
   @SYMTestCaseDesc 	Tests data type storage
  
   @SYMTestPriority 	High
  
   @SYMTestStatus 		Implemented
   
   @SYMTestActions  	The test creates a storage manager object, which is filled with some mapings.
   The first thing to verify is the correct insertion of these mappings, checking the type, uid
   and priority. Then the mappings are stored in an INI file, and restored from there to a new 
   storage manager object. The test checks that this new object is correct and equal to the
   previous one. The test also tries to find a non-existing app mapping, which gives a 0 Uid.
   API Calls:\n	
   CTypeStoreManager::NewL(RFs& aFs) \n
   CTypeStoreManager::InsertDataMappingL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid) \n
   CTypeStoreManager::DeleteDataMapping(const TDataType& aDataType) \n
   CTypeStoreManager::GetAppByDataType(const TDataType& aDataType, TUid& aUid) const \n
   CTypeStoreManager::StoreL() \n
   CTypeStoreManager::ReStoreL() \n
   
   @SYMTestExpectedResults Test should complete without any panic.
   
 */
void CT_MstrStep::doMenuL()
	{
	INFO_PRINTF1(_L("Testing Data Type storage"));
	INFO_PRINTF1(_L("Creating CTypeStoreManager object"));
	CTypeStoreManager* mman = CTypeStoreManager::NewL(iFs);
	CleanupStack::PushL(mman);

	INFO_PRINTF1(_L("Adding some mappings to table"));
	lPopulateMappingTablesL(mman);

	INFO_PRINTF1(_L("Checking mappings in table"));
	lCheckMappingsL(mman);
		
	INFO_PRINTF1(_L("Storing to INI file"));
	mman->StoreL();

	INFO_PRINTF1(_L("Restoring from INI file to new object"));
	CTypeStoreManager* mman2 = CTypeStoreManager::NewL(iFs);
	CleanupStack::PushL(mman2);
	mman2->RestoreL();

	INFO_PRINTF1(_L("Checking restored correctly"));

	lCheckMappingsL(mman2);

	INFO_PRINTF1(_L("Trying to find non-existent app mappings"));
	TUid scratchUid;
	mman->GetAppByDataType(TDataType(_L8("non/existent")),scratchUid);
	TEST(scratchUid.iUid==0);

	INFO_PRINTF1(_L("Clearing table to original state"));
	lClearMappingTablesL(mman2);

	CleanupStack::PopAndDestroy(2); // managers
	}

const TUid KTestUidValue1={10101010}; 
const TUid KTestUidValue2={20202020}; 
const TUid KTestUidValue3={30303030}; 

void CT_MstrStep::lPopulateMappingTablesL(CTypeStoreManager* aMan)
	{
	aMan->InsertDataMappingL(TDataType(_L8("image/gif")),KDataTypePriorityNormal,KTestUidValue1);
	aMan->InsertDataMappingL(TDataType(_L8("text/plain")),KDataTypePriorityNormal,KTestUidValue2);
	aMan->InsertDataMappingL(TDataType(_L8("text/plain")),KDataTypePriorityNormal,KTestUidValue1,KTestUidValue3);
	aMan->InsertDataMappingL(TDataType(_L8("text/word")),KDataTypePriorityNormal,KTestUidValue2);
	aMan->InsertDataMappingL(TDataType(_L8("something/else")),KDataTypePriorityNormal,KTestUidValue3);
	}

void CT_MstrStep::lClearMappingTablesL(CTypeStoreManager* aMan)
	{
	aMan->DeleteDataMapping(TDataType(_L8("image/gif")));
	aMan->DeleteDataMapping(TDataType(_L8("text/plain")));
	aMan->DeleteDataMapping(TDataType(_L8("text/plain")), KTestUidValue3);
	aMan->DeleteDataMapping(TDataType(_L8("text/word")));
	aMan->DeleteDataMapping(TDataType(_L8("something/else")));

	aMan->StoreL();
	}

void CT_MstrStep::lCheckMappingsL(CTypeStoreManager* aTypeMan)
	{
	TUid scratchUid;
	aTypeMan->GetAppByDataType(TDataType(_L8("image/gif")),scratchUid);
	TEST(scratchUid==KTestUidValue1);
	aTypeMan->GetAppByDataType(TDataType(_L8("text/plain")),scratchUid);
	TEST(scratchUid==KTestUidValue2);
	aTypeMan->GetAppByDataType(TDataType(_L8("text/plain")),KTestUidValue3,scratchUid);
	TEST(scratchUid==KTestUidValue1);
	aTypeMan->GetAppByDataType(TDataType(_L8("text/word")),scratchUid);
	TEST(scratchUid==KTestUidValue2);
	aTypeMan->GetAppByDataType(TDataType(_L8("something/else")),scratchUid);
	TEST(scratchUid==KTestUidValue3);
	aTypeMan->GetAppByDataType(TDataType(_L8("image/*")),scratchUid);
	TEST(scratchUid==KTestUidValue1);
	
	TDataType nativeData(KTestUidValue3);
	TEST(nativeData.IsNative());

	CArrayFixFlat<TDataType>* array=new(ELeave) CArrayFixFlat<TDataType>(1);
	CleanupStack::PushL(array);
	aTypeMan->GetDataTypesByAppL(KTestUidValue1,array);
	TEST(array->Count()==1);
	TEST((*array)[0]==TDataType(_L8("image/gif")));
	array->Delete(0);
	aTypeMan->GetDataTypesByAppL(KTestUidValue2,array);
	TEST(array->Count()==2);
	TEST((*array)[0]==TDataType(_L8("text/plain")));
	TEST((*array)[1]==TDataType(_L8("text/word")));
	array->Delete(0,2);
	aTypeMan->GetDataTypesByAppL(KTestUidValue3,array);
	TEST(array->Count()==1);
	TEST((*array)[0]==TDataType(_L8("something/else")));
	array->Delete(0);
	TUid uid;
	uid.iUid=0;
	aTypeMan->GetDataTypesByAppL(uid,array);
	TEST(array->Count()==4);
	CleanupStack::PopAndDestroy(); // array
	}
