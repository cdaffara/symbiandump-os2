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
//

/**
 @file 
 @internalComponent - Internal Symbian test code 
*/



#include <e32uid.h>
#include <f32file.h>
#include <apmrec.h>
#include <datastor.h>
#include <ecom/ecom.h>

#include "T_Maps.h"


/**
  Auxiliary Fn for Test Case ID T-Maps-testTMappingDataTypeToAppL
 
  This function receives two mappings, and verifies if they data members 
  are the same.
  
*/
TBool CT_Maps::cmpMappingDataTypeToAdd( TMappingDataTypeToApp& map1, TMappingDataTypeToApp& map2 )
	{
	return TBool( map1.iAppUid == map2.iAppUid &&
		map1.iDataType == map2.iDataType && 
		map1.iPriority == map2.iPriority &&
		map1.iServiceUid == map2.iServiceUid);
	}	


//const TInt KErrTestFailed = -1;


/**
   @SYMTestCaseID		T-Maps-testTMappingDataTypeToAppL
  
   @SYMPREQ			
  
   @SYMTestCaseDesc 	Tests TMappingDataTypeToApp class 
  
   @SYMTestPriority 	High
  
   @SYMTestStatus 		Implemented
   
   @SYMTestActions  	The test creates 3 different mappings, identified by a TUid, TDataType
   and TDataTypePriority, verifying that they are correctly created. The consitency of these 
   mappings through the process of writing and reading to and from a stream is also checked.\n
   Also, a store of mappings is created, verifying the insertion, and the insertion depending
   on the TDataTypePriority, checking that it handles the insertion in accordance.\n 
   API Calls:\n	
   TMappingDataTypeToApp::TMappingDataTypeToApp(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid) \n
   CTypeStoreManager::NewL(RFs& aFs) \n
   CTypeStoreManager::InsertDataMappingL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid) \n
   CTypeStoreManager::GetAppByDataType(const TDataType& aDataType, TUid& aUid) const \n
   CTypeStoreManager::InsertIfHigherL(const TDataType& aDataType, TDataTypePriority aPriority, TUid aUid) \n
   
   @SYMTestExpectedResults Test should complete without any panic.
   
 */
void CT_Maps::testTMappingDataTypeToAppL()
	{
	const TUid uid1 = { 0x11111111 };
	const TUid uid2 = { 0x22222222 };
	const TUid uid3 = { 0x33333333 };
	const TUid uid4 = { 0x44444444 };
	const TUid uid5 = { 0x55555555 };
	const TUid uid6 = { 0x66666666 };
	
	const TUid serviceUid1 = { 0x00001111 };
	const TUid serviceUid2 = { 0x00002222 };
	//const TUid serviceUid3 = { 0x00003333 };
	const TUid serviceUid4 = { 0x00004444 };

	const TDataType data1( uid1 );
	const TDataType data2( uid2 );
	const TDataType data3( uid3 );
	const TDataType data4( uid4 );
	
	TUid result;
	
	// Test that mappings are serialized correctly
	TMappingDataTypeToApp mapping1( data1, TDataTypePriority( 100 ), uid3 );
	TMappingDataTypeToApp mapping2( data1, TDataTypePriority( 100 ), uid3 );
	TMappingDataTypeToApp mapping3( data2, TDataTypePriority( 200 ), uid4 );

	RFileBuf buf1;
	RFileBuf buf2;
	RFileBuf buf3;
	
	TBuf<256> tmpfilename;
	
	buf1.Temp( iFs, KTempRootDir, tmpfilename, EFileWrite|EFileStream );
	buf2.Temp( iFs, KTempRootDir, tmpfilename, EFileWrite|EFileStream );
	buf3.Temp( iFs, KTempRootDir, tmpfilename, EFileWrite|EFileStream );

	RWriteStream write1( &buf1 );
	RWriteStream write2( &buf2 );
	RWriteStream write3( &buf3 );
	
	RReadStream read1( &buf1 );
	RReadStream read2( &buf2 );
	RReadStream read3( &buf3 );
	
	TEST(  cmpMappingDataTypeToAdd( mapping1, mapping2 ) );
	TEST( !cmpMappingDataTypeToAdd( mapping1, mapping3 ) );
	TEST( !cmpMappingDataTypeToAdd( mapping2, mapping3 ) );
		
	INFO_PRINTF1(_L("Testing storage of objects to stream"));
	
	write1 << mapping1;
	write2 << mapping2;
	write3 << mapping3;
	
	write1.CommitL();
	write2.CommitL();
	write3.CommitL();
		
	TEST(  cmpMappingDataTypeToAdd( mapping1, mapping2 ) );
	TEST( !cmpMappingDataTypeToAdd( mapping1, mapping3 ) );
	TEST( !cmpMappingDataTypeToAdd( mapping2, mapping3 ) );
		
	INFO_PRINTF1(_L("Testing reconstruction from stream"));
	
	read1 >> mapping2;
	read2 >> mapping3;
	read3 >> mapping1;
		
	TEST( !cmpMappingDataTypeToAdd( mapping1, mapping2 ) );
	TEST( !cmpMappingDataTypeToAdd( mapping1, mapping3 ) );
	TEST(  cmpMappingDataTypeToAdd( mapping2, mapping3 ) );
		
	write1.Close();
	write2.Close();
	write3.Close();
	
	read1.Close();
	read2.Close();
	read3.Close();
	
	buf1.Close();
	buf2.Close();
	buf3.Close();
	
	
	// Test the insert and find functions
	INFO_PRINTF1(_L("Data map insertion"));
	
	CTypeStoreManager* tsm = CTypeStoreManager::NewL( iFs );
	CleanupStack::PushL( tsm );
	
	tsm->InsertDataMappingL( data1, TDataTypePriority( 100 ), uid1 ); //data1 mapped to uid1
	tsm->InsertDataMappingL( data2, TDataTypePriority( 200 ), uid2 ); //data2 mapped to uid2
	tsm->InsertDataMappingL( data1, TDataTypePriority( 100 ), uid1, serviceUid1 ); 
	tsm->InsertDataMappingL( data2, TDataTypePriority( 100 ), uid6, serviceUid2 ); 
	tsm->InsertDataMappingL( data1, TDataTypePriority( 200 ), uid3 ); //data1 mapped to uid3, not uid1 anymore
	tsm->InsertDataMappingL( data4, TDataTypePriority( 200 ), uid4, serviceUid4 ); 
	tsm->InsertDataMappingL( data4, TDataTypePriority( 200 ), uid5 ); 

	tsm->GetAppByDataType( data1, result );
	TEST( uid3 == result );
	tsm->GetAppByDataType( data2, result );
	TEST( uid2 == result );
	tsm->GetAppByDataType( data1, serviceUid1, result);
	TEST( uid1 == result );
	tsm->GetAppByDataType( data2, serviceUid2, result);
	TEST( uid6 == result );
	tsm->GetAppByDataType( data4, result);
	TEST( uid5 == result );
	tsm->GetAppByDataType( data4, serviceUid4, result);
	TEST( uid4 == result );
			
	INFO_PRINTF1(_L("Data map insertion by priority"));
	
	TEST(  tsm->InsertIfHigherL( data2, TDataTypePriority( 900 ), uid4 ) ); //Should be higher priority
	tsm->GetAppByDataType( data2, result );
	TEST( uid4 == result ); 
	
	TEST( !tsm->InsertIfHigherL( data1, TDataTypePriority( 200 ), uid1 ) ); //Should be lower priority
	tsm->GetAppByDataType( data1, result );
	TEST( uid3 == result ); 
	
	TEST( !tsm->InsertIfHigherL( data1, TDataTypePriority( 100 ), uid2 ) ); //Should be lower priority
	tsm->GetAppByDataType( data1, result );
	TEST( uid3 == result ); 
	
	TEST(  tsm->InsertIfHigherL( data1, TDataTypePriority( 400 ), uid4 ) ); //Should be higher priority
	tsm->GetAppByDataType( data1, result );
	TEST( uid4 == result ); 
	
	TEST( !tsm->InsertIfHigherL( data1, TDataTypePriority( 300 ), uid3 ) ); //Should be lower priority
	tsm->GetAppByDataType( data1, result );
	TEST( uid4 == result );
	
	TEST( tsm->InsertIfHigherL( data1, TDataTypePriority( 300 ), uid3, serviceUid1 ) ); 
	tsm->GetAppByDataType( data1, serviceUid1, result);
	TEST( uid3 == result ); 
	
	TEST( !tsm->InsertIfHigherL( data1, TDataTypePriority( 100 ), uid5, serviceUid1 ) ); 
	tsm->GetAppByDataType( data1, serviceUid1, result);
	TEST( uid3 == result ); 
	
	tsm->InsertDataMappingL( data1, TDataTypePriority( 100 ), uid5, serviceUid1 ); 
	tsm->GetAppByDataType( data1, serviceUid1, result);
	TEST( uid5 == result );
	
	tsm->GetAppByDataType( data4, serviceUid2, result);
	TEST( KNullUid == result );
		
	CleanupStack::PopAndDestroy(tsm);
	}
	

CT_Maps::~CT_Maps()
/**
   Destructor
 */
	{
	}

CT_Maps::CT_Maps()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_Maps);
	}

/* @SYMTestCaseID    T-Maps-doTestStepL
  
   @SYMPREQ DEF032304

   @SYMTestCaseDesc     Testing of TMappingDataTypeToApp perisitance.
      
   @SYMTestPriority 	High
  
   @SYMTestStatus 		Implemented
      
   @SYMTestActions Just execute program.  Will pause and report any failure.

   @SYMTestExpectedResults  List of sucessfull tests should appear in a console window.
 */
TVerdict CT_Maps::doTestStepL()
	{
	//
	INFO_PRINTF1(_L("Test Started"));
	INFO_PRINTF1(_L("Testing the TMappingDataTypeToApp"));
	
	//
	// set up the directory structure
	iFs.Connect();
	//
	// run the testcode (inside an alloc heaven harness)

 	__UHEAP_MARK;
 	
	TRAPD(r,testTMappingDataTypeToAppL());
		TEST(r==KErrNone);

	iFs.Close();
	INFO_PRINTF1(_L("TMappingDataTypeToApp test finished\n"));

	REComSession::FinalClose();
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

