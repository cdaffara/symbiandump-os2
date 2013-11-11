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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <e32uid.h>
#include <f32file.h>
#include <bautils.h>
#include <apmrec.h>
#include <ecom/ecom.h>
#include "T_MimeStep.h"


//
_LIT8(KTextPlainData,"text/plain");
_LIT(KTextFile,"z:\\system\\data\\emime\\testrec.text");
_LIT(KTxtFile,"z:\\system\\data\\emime\\testrec.txt");
_LIT(KWordFile,"z:\\system\\data\\emime\\word.doc");
_LIT(KLeavingRecognizer,"TLEAVINGRECOGNIZER");

const TUid KWordAppUid={0x10003A64};

const TUid KLeavingRecogniserUid={0x1d1F75EB};
const TUid KTestEcomDataRecognizerUid={0x101F7DA1};
const TInt KTestDataRecognizerError=-420;  // some random number for test error code
_LIT(KDeceptiveRecognizerToken, "c:\\test\\appfwk\\emime\\deceptive_recognizer.token");
//

/* Construction of CTestDataRecognitionType Object*/

CTestDataRecognizerType* CTestDataRecognizerType::NewDataRecogTypeL(RFs &aIfs)
	{
	CTestDataRecognizerType *dataRecogTypeObj=new(ELeave)CTestDataRecognizerType(aIfs);
	CleanupStack::PushL(dataRecogTypeObj);
	CleanupStack::Pop(dataRecogTypeObj);
	return dataRecogTypeObj;
	
	}
/* Constructor*/
	
CTestDataRecognizerType::CTestDataRecognizerType (RFs &aIfs)
			:CApaDataRecognizer(aIfs)
	{
	
	
	}

CTestDataRecognizerType::~CTestDataRecognizerType()
	{
	
	}
/**
  Auxiliary Fn for Test Case ID T-MimeStep-testScanningMimeTheRecognizerL
 
  This method is used to verify the functionality of Locking and Unlocking the recognizer 
  with the UID of the recognizer .Whenever the Recognizer is Locked the return value of 
  Locked() function should be a value greater than zero. And When the recognizer is tried to
  Unload , the unloading operation would fail with return code KErrLocked.
  When the recognizer is in Unlocked condition the return value of Locked() is zero.
  And Function to Unload the Recognizer is called again after unlocking the recognizer , and the 
  Recognizer is unloaded without any Error.And the return code will be KErrNone.

  This method is also used to verify the return values of Mimetype() and Confidence(), 
  When the recognizer is not added . And hence the values are default when the 
  Recognizers are not added.
  
*/
void CT_MimeStep::ChkLockAndUnlockL(RFs &aIfs)
	{
	
	CTestDataRecognizerType *testLockAndUnLock=CTestDataRecognizerType::NewDataRecogTypeL(aIfs);
	CleanupStack::PushL(testLockAndUnLock);
	TRAPD(ret,testLockAndUnLock->AddDataL(iData));
	INFO_PRINTF2(_L("Val of AddDataL  %d"), ret);
		
	TInt valBeforeLock=iData->Locked();
	INFO_PRINTF2(_L("Val of Lock Before %d"), valBeforeLock);
	TEST(valBeforeLock==0);

	iData->Lock();	
	TInt valAfterLock=iData->Locked();
	INFO_PRINTF2(_L("Val of Lock %d"), valAfterLock);
	TEST(valAfterLock > 0);

	TInt remDataAfterLock=testLockAndUnLock->RemoveData(iData);
	TEST(remDataAfterLock==KErrLocked);
	INFO_PRINTF2(_L("Val of remDataAfterLock %d"), remDataAfterLock);

	TRAP(ret,testLockAndUnLock->UpdateDataTypesL());
	TEST(ret==KErrNone);

	iData->Unlock();
	TInt valAfterUnlock=iData->Locked();
	INFO_PRINTF2(_L("Val of Loc k %d"), valAfterUnlock);
	TEST(valAfterUnlock==0);

	TInt remDataAfterUnLock=testLockAndUnLock->RemoveData(iData);
	TEST(remDataAfterUnLock==KErrNone);
	INFO_PRINTF2(_L("Val of remDataAfterLock %d"), remDataAfterUnLock);


	CleanupStack::PopAndDestroy(testLockAndUnLock);
	}
/**
  Auxiliary Fn for Test Case ID T-MimeStep-testScanningMimeTheRecognizerL 
  
  This method is  used to verify the return values of Mimetype() and Confidence(), 
  When the recognizer is not added . And hence the values are default when the 
  Recognizers are not added. */
  
  
void CT_MimeStep::ChkConfAndMimeL(RFs &aIfs,const TUid &aUid)
{
	CTestDataRecognizerType *confAndMimeDataRec=CTestDataRecognizerType::NewDataRecogTypeL(aIfs);
	CleanupStack::PushL(confAndMimeDataRec);
	TRAPD(ret,iData=CApaDataRecognizerType::CreateDataRecognizerL(aUid));
	TEST(ret==KErrNone);
	TDataType dataType= iData->MimeType();
	TInt confidenceVal=iData->Confidence();
	TEST(dataType.Uid().iUid==0);
	TEST(confidenceVal==0);
	INFO_PRINTF2(_L("Val of confidence  %d"), confidenceVal);
	//New tests
	TInt bufSize = iData->PreferredBufSize();
	TEST(bufSize==0);
	INFO_PRINTF2(_L("Val of Preferred buffer size  %d"), bufSize);
	INFO_PRINTF1(_L("Testing default constructor of TDataTypeWithPriority"));
	TDataTypeWithPriority dataTypeWithPriority = TDataTypeWithPriority();
	CleanupStack::PopAndDestroy(confAndMimeDataRec);	
}


/**
  Auxiliary Fn for Test Case ID T-MimeStep-testScanningMimeTheRecognizerL
 
  This function checks a return value, and prints it in an Error return value.
  
*/
void CT_MimeStep::DoTest(TInt aReturnValue)
	{
	if (aReturnValue!=KErrNone)
INFO_PRINTF2(_L("\nError: %D\n"),aReturnValue);
	TEST(aReturnValue==KErrNone);
	}


/**
  Auxiliary Fn for Test Case ID T-MimeStep-testScanningMimeTheRecognizerL
 
  This function checks the integrity of a Recognizer.
  
*/
void CT_MimeStep::testRecognizer(const CApaScanningDataRecognizer::TRecognizer& aRec)
	{
	_LIT(KRecText,"RECTXT");
	_LIT(KRecWeb,"RECWEB");
	_LIT(KRecApp2,"RECAPP2");
	_LIT(KRecJar,"RECJAR");
	_LIT(KBookMarkRec,"EBOOKMARKREC");
	_LIT(KRecMda,"RECMDA");
	_LIT(KRecOffice,"RECOFFICE");
	_LIT(KRecWap,"RECWAP");
	_LIT(KRVersit,"RVERSIT");
	_LIT(KWebUrlRec,"WEBURLREC");
	_LIT(TTestEcomDataRec,"TTESTECOMDATAREC");
		
	if (aRec.iUid.iUid==0x100012FB)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KRecText)==0);
		}
	else if (aRec.iUid.iUid==0x10001315)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KRecWeb)==0);
		}
	else if (aRec.iUid.iUid==0x1000415F)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KRecApp2)==0);
		}
	else if (aRec.iUid.iUid==0x1000967A)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KRecJar)==0);
		}
	else if (aRec.iUid.iUid==0x10008ED4)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KBookMarkRec)==0);
		}
	else if (aRec.iUid.iUid==0x10005617)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KRecMda)==0);
		}
	else if (aRec.iUid.iUid==0x10008A2F)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KRecOffice)==0);
		}
	else if (aRec.iUid.iUid==0x1000515E)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KRecWap)==0);
		}
	else if (aRec.iUid.iUid==0x100047EB)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KRVersit)==0);
		}
	else if (aRec.iUid.iUid==0x100064DE)
		{
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KWebUrlRec)==0);
		}
	else if (aRec.iUid==KLeavingRecogniserUid)
		{
INFO_PRINTF1(_L("Testing a recogniser(TLEAVINGRECOGNIZER) with >12 chars length"));
		TEST(aRec.iDrive==25);
		TEST(aRec.Name().CompareF(KLeavingRecognizer)==0);
		}
	else if(aRec.iUid.iUid==0x101F7DA0)
		{
		TEST(aRec.Name().CompareF(KRecText)==0);
		}
	else if(aRec.iUid.iUid==0x101F7D9F)
		{
		TEST(aRec.Name().CompareF(KRecApp2)==0);
		}
	else if(aRec.iUid==KTestEcomDataRecognizerUid)
		{
INFO_PRINTF1(_L("Testing the presence of ecom style TTESTECOMDATAREC recognizer"));
		TEST(aRec.Name().CompareF(TTestEcomDataRec)==0);
		}
	}


 /**
   @SYMTestCaseID		T-MimeStep-testScanningMimeTheRecognizerL
  
   @SYMPREQ			
  
   @SYMTestCaseDesc 	Tests the Scanning File-Recognizer
  
   @SYMTestPriority 	High
  
   @SYMTestStatus 		Implemented
   
   @SYMTestActions  	The test verifies the Scannig File-Recognizer by testing
   the getter and setter functions. It creates a temporal list of recognizers to
   check the getter function, and then goes through the whole list of recognizers 
   testing each one.\n
   Also, the setter function is checked, by adding a recognizer that's already
   added, and adding one that doesn't exist, and verifying that correct return 
   value. Finally, it is checked if specific recognizers are loaded or not.\n
   This test Also checks for the functionality of the Locking and Unlocking 
   Mechanism of the recognizer and checks that the When the recognizers are not 
   added in CAPARecognizertype class , The properties of the Recognizer will be 
   the defaulted.
   API Calls:\n
   CApaScanningDataRecognizer::NewL(RFs& aFs) \n
   CApaScanningDataRecognizer::TRecognizer(HBufC* aName) \n
   CApaScanningDataRecognizer::RecognizerListLC() \n
   CApaScanningDataRecognizer::RecognizerCount() \n
   CApaScanningDataRecognizer::UpdateCounter() const \n
   CApaScanningDataRecognizer::SetRecognizerL() \n
   CApaScanningDataRecognizer::SetEcomRecognizerL() \n
   CApaDataRecognizerType::MimeType() \n
   CApaDataRecognizerType::Confidence() \n
   CApaDataRecognizerType::Lock() \n
   CApaDataRecognizerType::Unlock() \n
   @SYMTestExpectedResults Test should complete without any panic.
   
 */
void CT_MimeStep::testScanningMimeTheRecognizerL()
	{
INFO_PRINTF1(_L("Testing the Scanning File-Recognizer"));
	//
	// construct - this scans for TheRecognizer plug-ins
	TRAPD(ret, iRecognizer=CApaScanningDataRecognizer::NewL(iFs) );
		DoTest(ret);
		TEST(iRecognizer->RecognizerCount()>=3);
		TEST(iRecognizer->UpdateCounter()>=3);
	//
INFO_PRINTF1(_L("Testing the getter and setter functions"));
	//
	// check the getter function
	CApaScanningDataRecognizer::CRecognizerArray* tempListOfRecognizers = iRecognizer->RecognizerListLC();
	TInt listCount = tempListOfRecognizers->Count();

	for (TInt ii=0;ii<listCount;ii++)
		{
		testRecognizer((*tempListOfRecognizers)[ii]);
		}

	CleanupStack::PopAndDestroy(1); // tmpListOfRecogniszers
	
	//New test
	INFO_PRINTF1(_L("Testing index([]) operator"));
	const CApaScanningDataRecognizer::TRecognizer& testDataRecognizer=(*iRecognizer)[0];
	TUid uid1 =  testDataRecognizer.iUid;
	TUid uid2 = ((*iRecognizer)[0]).iUid;
	TEST(uid1==uid2);
//for testing ecom style plugin
INFO_PRINTF1(_L("Testing the ecom style TTESTECOMDATAREC recognizer is loaded or not"));
	CApaScanningDataRecognizer::TRecognizer testEcomDataRecognizer;
	testEcomDataRecognizer.iUid=KTestEcomDataRecognizerUid;
	testEcomDataRecognizer.iDrive=25;
	TRAP(ret,iRecognizer->SetEcomRecognizerL(testEcomDataRecognizer));
		TEST(ret==KErrNone);
// for testing Lock() ,Unlock of the recognizers.		
INFO_PRINTF1(_L("Test Confidence and MimeType APIs of CApaDataRecognizerType "));
	TRAP(ret,ChkConfAndMimeL(iFs,testEcomDataRecognizer.iUid));
		TEST(ret==KErrNone);
			
INFO_PRINTF1(_L("Test Lock And Unlock APIs of CApaDataRecognizerType "));
	TRAP(ret,ChkLockAndUnlockL(iFs));
		TEST(ret==KErrNone);

INFO_PRINTF1(_L("Testing the TECOMLEAVINGRECOGNIZER recogniser is loaded or not"));
	CApaScanningDataRecognizer::TRecognizer ecomLeavingRecognizer;
	ecomLeavingRecognizer.iUid.iUid = 0x10203630;
	ecomLeavingRecognizer.iDrive = 25;
	TRAP(ret,iRecognizer->SetEcomRecognizerL(ecomLeavingRecognizer));
		TEST(ret==KErrNone);
		TEST(iRecognizer->UpdateCounter()>=3);
	}


 /**
   @SYMTestCaseID		T-MimeStep-testTDataTypeL
  
   @SYMPREQ			
  
   @SYMTestCaseDesc 	Test TDataType structure by creating new automatic variables
  
   @SYMTestPriority 	High
  
   @SYMTestStatus 		Implemented
   
   @SYMTestActions  	The test creates new TDataType automatic variables. 
   It creates a first TDataType using the constructor taking a UID. The data 
   type must be  a native Symbian data type, with an associated UID equal 
   to the original UID from which the UID was constructed.\n
   A second TDataType is created using a pointer descriptor to the data 
   type previously created. Also, the data type must be a native Symbian data 
   type, with the same associated UID than the previously data type created.\n
   API Calls:\n
   TDataType::TDataType() \n
   TDataType::TDataType(const TDesC8& aDataType) \n
   TDataType::IsNative() \n
   TDataType::Uid() \n
  
   @SYMTestExpectedResults Test should complete without any panic.
  
 */
void CT_MimeStep::testTDataTypeL()
// check TDataType constructors
	{
	TUid uid={0x12345678};
	TDataType dataType(uid);
	TEST(dataType.IsNative());
	TEST(dataType.Uid()==uid);
	TDataType secondType(dataType.Des8());
	TEST(secondType==dataType);
	TEST(secondType.IsNative());
	TEST(secondType.Uid()==uid);
	}


 /**
   @SYMTestCaseID		T-MimeStep-testRecognizersL
  
   @SYMPREQ			
  
   @SYMTestCaseDesc 	Tests file recognizers using RecognizerL function
  
   @SYMTestPriority 	High
  
   @SYMTestStatus 		Implemented
   
   @SYMTestActions  	The test uses the RecognizeL function to recognize the
   data type of the data in the files, corresponding to three types of file formats:\n
   (1) .text file, containing plain text. The recognized data type must be 
   equal to a plain text TDataType. When no TDataType is defined, the error must 
   be propagated accordingly.\n
   (2) .txt file, containing plain text. The recognized data type must be equal to
   a plain text TDataType.\n
   (3) Word file (.doc format). The recognized data type must be equal to
   a word file TDataType.\n
   API Calls:\n	
   CApaScanningDataRecognizer::RecognizeL(const TDesC& aName, const TDesC8& aBuffer)\n
   
   @SYMTestExpectedResults Test should complete without any panic.
   
 */
void CT_MimeStep::testRecognizersL()
	{
	//
	// do other stuff...
	
INFO_PRINTF1(_L("Testing the text file recognizer"));
	CArrayFixFlat<TDataType>* array=new(ELeave) CArrayFixFlat<TDataType>(5);
	CleanupStack::PushL(array);
	iRecognizer->DataTypeL(*array);

	TInt minNumDataTypes = 1;	// txt/plain
	minNumDataTypes++;			// dodgy/app

	TEST(array->Count()>=minNumDataTypes);
	CleanupStack::PopAndDestroy(); // array
	// I don't know what order these will be in - I can't test them

	TInt bufSize=iRecognizer->PreferredBufSize();
	HBufC8* buf=HBufC8::NewLC(bufSize);
	RFile rfile;
	TInt err=rfile.Open(iFs,KTextFile,EFileShareReadersOnly);
	TEST(err==KErrNone);
	TPtr8 des=buf->Des();
	if (err==KErrNone)
		{
		err=rfile.Read(des);
		if (err!=KErrNone)
			des.SetLength(0);
		}
	rfile.Close();
	TDataType textplain(KTextPlainData);
	// recognizes the plain text
	TEST(iRecognizer->RecognizeL(KTextFile, des).iDataType==textplain);
	// test by passing a empty file name
	TEST(iRecognizer->RecognizeL(TPtrC(), des).iDataType==textplain);

	//Test the propagation of error (leave) occured in a recognizer, 
	//if there is no probable data type matched.
	// There is a V1 version and a V2 (ECom) version of this recognizer.
	INFO_PRINTF1(_L("Propagates error if a recognizer leaves with no probable data type found"));
	TDataType dataType;
	des.SetLength(0);
	TRAPD(ret,dataType=iRecognizer->RecognizeL(KTextFile, des).iDataType);
	TEST(dataType==TDataType());
	INFO_PRINTF2(_L("Propagated error:%d, it should should be equal to KTestDataRecognizerError(-420)"), ret);
	TEST(ret==KTestDataRecognizerError);

	err=rfile.Open(iFs,KTxtFile,EFileShareReadersOnly);
	TEST(err==KErrNone);
	des=buf->Des();
	if (err==KErrNone)
		{
		err=rfile.Read(des);
		if (err!=KErrNone)
			des.SetLength(0);
		}
	rfile.Close();
	
	TRAP(ret,dataType=iRecognizer->RecognizeL(KTxtFile, des).iDataType);
	TEST(dataType==textplain);
	TEST(err==KErrNone);
	
	des.SetLength(0);
	TRAP(ret,dataType=iRecognizer->RecognizeL(KTxtFile, des).iDataType);
	TEST(dataType==textplain);
	TEST(err==KErrNone);

	INFO_PRINTF1(_L("Testing the EIKON app recognizer"));
	des.SetLength(0);
	err=rfile.Open(iFs,KWordFile,EFileShareReadersOnly);
	TEST(err==KErrNone);
	des=buf->Des();
	if (err==KErrNone)
		{
		err=rfile.Read(des);
		if (err!=KErrNone)
			des.SetLength(0);
		}

	rfile.Close();
	TRAP(ret,dataType=iRecognizer->RecognizeL(KWordFile, des).iDataType);
		TEST(dataType==TDataType(KWordAppUid));
		TEST(err==KErrNone);
	CleanupStack::PopAndDestroy(buf); // buf

	delete iRecognizer;
	}

CT_MimeStep::~CT_MimeStep()
/**
   Destructor
 */
	{
	}

CT_MimeStep::CT_MimeStep()
/**
   Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KT_MimeStep);
	}


TVerdict CT_MimeStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Started - Testing the APMIME dll"));
	
	// set up the directory structure
	User::LeaveIfError(iFs.Connect());
	

	INFO_PRINTF1(_L("Create token file for deceptive leaving recognizer activation"));
	RFile file;
	CleanupClosePushL(file);
	TInt r = iFs.MkDirAll(KDeceptiveRecognizerToken);
	TEST(r == KErrNone || r == KErrAlreadyExists);
	TEST(file.Create(iFs, KDeceptiveRecognizerToken, EFileWrite|EFileShareExclusive) == KErrNone);
	CleanupStack::PopAndDestroy(&file);

	// run the testcode (inside an alloc heaven harness)
 	__UHEAP_MARK;

	CActiveScheduler* pS=new(ELeave) CActiveScheduler;
	TEST(pS!=NULL);
	CActiveScheduler::Install(pS);
	TRAP(r,CT_MimeStep::testScanningMimeTheRecognizerL());
	TEST(r==KErrNone);
	TRAP(r,CT_MimeStep::testRecognizersL());
	TEST(r==KErrNone);
	TRAP(r,CT_MimeStep::testTDataTypeL());
	TEST(r==KErrNone);

	delete pS;
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	

	// Removes token file for deceptive leaving recognizer
	INFO_PRINTF1(_L("Remove token file for deceptive leaving recognizer deactivation"));
	TEST(iFs.Delete(KDeceptiveRecognizerToken) == KErrNone);
	
	iFs.Close();

	INFO_PRINTF1(_L("Test Finished"));
	return TestStepResult();
	}


