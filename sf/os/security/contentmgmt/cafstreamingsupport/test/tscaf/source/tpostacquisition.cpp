// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the post-acquisition content protection test.
// 
//

#include "tpostacquisition.h"
#include "testprotstrdesc.h"
#include "testagentkeystreamdecoder.h"
#include <caf/data.h>
#include <caf/content.h>
#include <caf/supplier.h>
#include <caf/metadataarray.h>
#include <caf/supplieroutputfile.h>
#include <caf/streaming/keystreamdecoder.h>

using namespace StreamAccess;
using namespace ContentAccess;

CScafPostAcquisition::CScafPostAcquisition(CScafServer& aParent)
/**
 	Constructor for the postacquisition test object.
 */
	:	CScafStep(aParent)
	{
	SetTestStepName(KScafPostAcquisitionStep);
	}

CScafPostAcquisition::~CScafPostAcquisition()
/**
 	Destructor for the postacquisition test object.
 */
	{
	//empty
	}

void DeleteFileL(RFs& aFs, const TDesC& aPath)
	{
	TInt err = 0;
	err = aFs.Delete(aPath);
	if(err != KErrNone && err != KErrNotFound && err != KErrPathNotFound)
		{
		User::Leave(err);
		}
	}

TVerdict CScafPostAcquisition::doTestL()
	{
	TBool createOutputFile = EFalse;
	
	if(!GetBoolFromConfig(ConfigSection(), KCreateOutputFiles, createOutputFile))
		{
		ERR_PRINTF2(_L("Failed to read '%S' key of configuration"), &KCreateOutputFiles);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	//Ensure pre-conditions
	CleanAgentsPrivateFolderL(KRtaPrivateFolder());
	CleanAgentsPrivateFolderL(KStaPrivateFolder());
	CleanAgentsPrivateFolderL(KTestServerPrivateFolder());
	CleanAgentsPrivateFolderL(KRtaTestFolder());
	DeleteFileL(iParent.Fs(), _L("c:\\PostAcquisitionTest.dr"));
	DeleteFileL(iParent.Fs(), _L("c:\\PostAcquisitionTest.TXT"));

	//First copy the RO, which allows to record the protected content, into the test agent's private directory
	CopyFile2AgentsPrivateFolderL(iParent.Fs(), KRecordableRoFileName(), KStaPrivateFolder());
	
	//Secondly copy the program protected RO into the test agent's private directory
	CopyFile2AgentsPrivateFolderL(iParent.Fs(), KPostAcquisitionRoFileName(), KStaPrivateFolder());
	
	//Create an SDP document object and set the created key stream field object
	CSdpDocument* sdpDoc = CreateSdpDocumentLC();
	//Create a SDP object for RO-Recordable
	CSdpMediaField* sdp = CreateSdpLC(0);
	AddMediaFieldL(*sdpDoc, sdp);
	CleanupStack::Pop(sdp);
	
	// Generate a protected key stream description. 
	HBufC* filePath = GetFullPathLC(KStaPrivateFolder, KNullDesC);
	CTestProtectedStreamDesc* protectStreamDesc = CTestProtectedStreamDesc::NewL(*filePath);
	CleanupStack::PopAndDestroy(filePath);
	CleanupStack::PushL(protectStreamDesc);
	
	//Create a key stream decoder
	CKeyStreamDecoder* decoder = CKeyStreamDecoder::NewL(*protectStreamDesc, *sdp, *sdpDoc); 
	CleanupStack::PushL(decoder);
	
	//Check whether the content is recordable
	TBool value = EFalse;
	decoder->GetAttributeL(ECanExport, value);
	if(!value)
		{
		ERR_PRINTF1(_L("The content can not be exported. Unexpected result!"));
		SetTestStepResult(EFail);
		User::Leave(KErrCANotSupported);
		}
	
	CImportFile* import(0);
	_LIT(KPostAcqContentFileName, "postacquisition.content");
	if(!createOutputFile)
		{
		HBufC* outputDir = GetFullPathLC(KRtaPrivateFolder(), KNullDesC());
		import = decoder->CreateImportSessionLC(_L8("text/plain"), KPostAcqContentFileName(), *outputDir);
		CleanupStack::Pop(import);
		CleanupStack::PopAndDestroy(outputDir);
		CleanupStack::PushL(import);
		}
	else
		{
		import = decoder->CreateImportSessionLC(_L8("text/plain")); 
		//To get a better coverage, delete this import object and create another one with zero-length-output-directory.
		CleanupStack::PopAndDestroy(import);
		DeleteFileL(iParent.Fs(), _L("c:\\PostAcquisitionTest.dr"));
		DeleteFileL(iParent.Fs(), _L("c:\\PostAcquisitionTest.TXT"));
		import = decoder->CreateImportSessionLC(_L8("text/plain"), _L("NULL"), KNullDesC()); 
		}
	
	_LIT8(KStreamingData1,"SampleStreamingDataBegan");
	_LIT8(KStreamingData2,"TheEndOfStreamingData");
	_LIT8(KStreamingDataFull,"SampleStreamingDataBeganTheEndOfStreamingData");
	
	User::LeaveIfError(import->WriteData(KStreamingData1()));
	User::LeaveIfError(import->WriteData(KStreamingData2()));
	//Import has been completed
	TFileName fileName;
	TInt err = 0;
	err = import->WriteDataComplete();
	while (err == KErrCANewFileHandleRequired)              
        {
        err = ProvideNewOutputFileL(*import, fileName);
        if(err == KErrNone)
        	{
        	err = import->WriteDataComplete();
        	}
        }
	User::LeaveIfError(err);
	
	CleanupStack::PopAndDestroy(3, protectStreamDesc);
	CleanupStack::PopAndDestroy(); //sdpDoc
	
	//Try to read what we have imported
	CContent* content(0);
	if(!createOutputFile)
		{
		HBufC* filePath = GetFullPathLC(KRtaPrivateFolder(), KPostAcqContentFileName());
		content = CContent::NewL(*filePath);
		CleanupStack::PopAndDestroy(filePath);
		CleanupStack::PushL(content);
		}
	else
		{
		content = CContent::NewLC(fileName);
		}
	CData* mydata = content->OpenContentL(EView);
	CleanupStack::PushL(mydata);
	
	RBuf8 rdata;
	rdata.CreateL(KStreamingDataFull().Length());
	rdata.CleanupClosePushL();
	User::LeaveIfError(mydata->Read(rdata));
	if(rdata.Compare(KStreamingDataFull()))
		{
		SetTestStepResult(EFail);		
		}
	CleanupStack::PopAndDestroy(3, content);
	return TestStepResult();
	}

TInt CScafPostAcquisition::ProvideNewOutputFileL(CImportFile& aImport, TDes& aName)
	{
	TInt err = 0;
	// supply new file in order to continue writing
	aImport.GetSuggestedOutputFileName(aName);
    HBufC* newFullName = GetFullPathLC(KRtaTestFolder(), aName);
    RFile newFile;
    User::LeaveIfError(newFile.Replace(iParent.Fs(), *newFullName, EFileWrite));
    err = aImport.ContinueWithNewOutputFile(newFile, *newFullName);
    newFile.Close();
    aName.Copy(*newFullName);
    CleanupStack::PopAndDestroy(newFullName);
    return err;
	}
