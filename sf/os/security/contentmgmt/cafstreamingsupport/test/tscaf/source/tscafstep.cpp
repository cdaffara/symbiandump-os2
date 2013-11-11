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
// Implements the basic test step for the Streaming CAF test harness
// 
//

#include "tscafstep.h"
#include "sdpconnectionfield.h"
#include "sdporiginfield.h"

TSdpAttribute::TSdpAttribute()
	{
	}

CTestDecoderConfiguration* CTestDecoderConfiguration::NewLC()
	{
	CTestDecoderConfiguration* self = new (ELeave) CTestDecoderConfiguration();
	CleanupStack::PushL(self);
	return self;	
	}
	
CTestDecoderConfiguration::~CTestDecoderConfiguration()
	{
	iAttributeArray.Close();	
	}

CTestDecoderConfiguration::CTestDecoderConfiguration()
	{	
	} 

TExpectedKeyStreamDecoderAttributes::TExpectedKeyStreamDecoderAttributes()
	{
	}

CScafStep::CScafStep(CScafServer& aParent)
/**
 	Constructor.
 */
	: iParent(aParent), iThreadId(0), iOOMTest(EFalse)
	{
	//empty
	}

CScafStep::~CScafStep()
/**
 	Destructor.
 */
	{
	//empty
	}
	
	
TVerdict CScafStep::doTestStepPreambleL()
/**
 	From CTestStep. Creates an active scheduler for the test step.
 */
	{
	__UHEAP_MARK;
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	iActiveScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	
	ReadTestConfigurationL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CScafStep::doTestStepL()
/**
 * From CTestStep. Default behaviour of doTestStepL() allows for the test case to be run both
 * under 'Normal' and 'Out of Memory' Conditions. 
 * 
 * Implementation of the test case itself is called from the doTestL() of the derived test step.
 * 
 * The state of the iOOMTest member variable determines the type of test conditons:
 * EFalse - Normal Test 
 * ETrue - Out of Memory Test
 */
	{
	if (!iOOMTest)
		{
		doTestL();
		}
	else
		{
		doOOMTestL();
		}	
	
	return TestStepResult();
	}

TVerdict CScafStep::doTestStepPostambleL()
/**
 	From CTestStep. Destroys the active scheduler of the test step.
 */
	{
	CActiveScheduler::Install(NULL);
	delete iActiveScheduler;
	
	iDecoderConfigurationArray.ResetAndDestroy();
	iExpectedKeyStreamDecoderData.Close();
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"),User::CountAllocCells());
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

// We need this dummy function because TCleanupItem c'tor (see below) does not accept functions without parameters
void CloseSdpCodecPool(TAny *)
	{
	SdpCodecStringPool::Close();
	}

CSdpDocument* CScafStep::CreateSdpDocumentLC()
/**
 	Creates an SDP document object which is a collection of all media fields and session attributes.
 	@return A pointer to the SDP document object. 
 			The ownership is transferred. Please note that
 			the returned object must be deleted by DeleteSdpAndCloseCodecPool method.
 */
	{
	// Open the string pool to access all predefined SDP constants
	SdpCodecStringPool::OpenL();
	CleanupStack::PushL(TCleanupItem(CloseSdpCodecPool,this)); //Pass the class pointer to make armv5 compiler (for urel) happy
	
	// Create the CSdpDocument object
	CSdpDocument* sdpDocument = CSdpDocument::NewLC();
	
	// Define the session name
	sdpDocument->SetSessionNameL(_L8("ScafTest"));
	
	// Set the origin field. The values are not important. 
	// Because they are not used in our tests.
	TInt64 sessionId(TUint(2055478987));
	TInt64 sessionVersion(TUint(2027813655));
	TInetAddr address;
	const TUint32 KInetAddr = INET_ADDR(192,168,0,3);
	address.SetAddress( KInetAddr );
	CSdpOriginField* originField = CSdpOriginField::NewLC(_L8("scafuser"), sessionId, sessionVersion, address);
	sdpDocument->SetOriginField(originField);
	CleanupStack::Pop(originField);
	
	CleanupStack::Pop(); // Pop the temporary guard on SDP Codec string pool close
	CleanupStack::Pop(); // Pop sdpDocument object
	CleanupStack::PushL(TCleanupItem(CScafStep::DeleteSdpDocAndCloseCodecPool, reinterpret_cast<TAny *>(sdpDocument)));
	
	return sdpDocument;
	}

void CScafStep::AddMediaFieldL(CSdpDocument& aSdpDoc, const CSdpMediaField* aSdpKeyStream)
/**
 	Adds an SDP media field object to a given SDP document object.
 	@param aSdpDoc The SDP document object
 	@param aSdpKeyStream Sdp media field which will be appended to the SDP document object.
 						 This object should be popped from the cleanupstack after this function runs successfully.
 */
	{
	// Set the key stream field given
	User::LeaveIfError(aSdpDoc.MediaFields().Append(aSdpKeyStream));
	}

CSdpMediaField* CScafStep::CreateSdpLC(TInt aSdpNum)
/**
 	Creates a simple SDP media field object.
 	@param aSdpNum An integer representing the accessor to the SDP parameter
 	 		array and the required data 
 	@return A pointer to the SDP media field object. 
 			The ownership is transferred. Please note that
 			the returned object must be deleted by DeleteSdpAndCloseCodecPool method.
 	@see CScafStep::DeleteSdp
 */
	{
	TPtrC pMedia;
	pMedia.Set(iDecoderConfigurationArray[aSdpNum]->iMedia);
	
	TInt mIndex = 0;
	if(!pMedia.CompareF(KSdpMediaAudio))
		{
		mIndex = SdpCodecStringConstants::EMediaAudio;
		}
	else if(!pMedia.CompareF(KSdpMediaVideo))
		{
		mIndex = SdpCodecStringConstants::EMediaVideo;
		}
	else if(!pMedia.CompareF(KSdpMediaData))
		{
		mIndex = SdpCodecStringConstants::EMediaData;
		}
	else
		{
		ERR_PRINTF2(_L("Unsupported media type: '%S'"),&pMedia);
		SetTestStepResult(EFail);
		User::Leave(KErrNotFound);
		}
	
	TPtrC pProtocol;
	pProtocol.Set(iDecoderConfigurationArray[aSdpNum]->iProtocol);
	
	TInt mProtocol = 0;
	if(!pProtocol.CompareF(KSdpProtocolUdp))
		{
		mProtocol = SdpCodecStringConstants::EProtocolUdp;
		}
	else if(!pProtocol.CompareF(KSdpProtocolTcp))
		{
		mProtocol = SdpCodecStringConstants::EProtocolTcp;
		}
	else if(!pProtocol.CompareF(KSdpProtocolRtp))
		{
		mProtocol = SdpCodecStringConstants::EProtocolRtpAvp;
		}
	else
		{
		ERR_PRINTF2(_L("Unsupported protocol type: '%S'"),mProtocol);
		SetTestStepResult(EFail);
		User::Leave(KErrNotFound);
		}
	
	TInt port = iDecoderConfigurationArray[aSdpNum]->iPort;
	
	TPtrC pFormat;
	pFormat.Set(iDecoderConfigurationArray[aSdpNum]->iFormat);
	
	//Convert 16-bit to 8-bit
	TPtr8 ptrFormat(Convert16To8LC(pFormat));
		
	RStringPool pool = SdpCodecStringPool::StringPoolL();	
	RStringF mediaData = pool.StringF(mIndex, SdpCodecStringPool::StringTableL());
	CleanupClosePushL(mediaData);
	RStringF protocol = pool.StringF(mProtocol, SdpCodecStringPool::StringTableL());
	CleanupClosePushL(protocol);

	CSdpMediaField* sdp = CSdpMediaField::NewL(mediaData, port, protocol, ptrFormat);
	CleanupStack::PopAndDestroy(3);
	CleanupStack::PushL(sdp);
	
	//If a connection address is defined, create an SDP connection field and add it to the SDP media field
	if(iDecoderConfigurationArray[aSdpNum]->iConnAddr.Length()>0)
		{
		//The only supported network type is ENetType
		RStringF netType = pool.StringF(SdpCodecStringConstants::ENetType, SdpCodecStringPool::StringTableL());
		CleanupClosePushL(netType);
		//The only supported address type is IP v4
		RStringF addressType = pool.StringF(SdpCodecStringConstants::EAddressTypeIP4, SdpCodecStringPool::StringTableL());
		CleanupClosePushL(addressType);
		//Create the connection field
		CSdpConnectionField* connField = CSdpConnectionField::NewL(netType, addressType, Convert16To8LC(iDecoderConfigurationArray[aSdpNum]->iConnAddr));
		CleanupStack::PushL(connField);
		//Set the connection field into the SDP media field
		sdp->ConnectionFields().AppendL(connField);
		CleanupStack::Pop(connField);
		CleanupStack::PopAndDestroy(3, &netType); //netType, addressType, Convert16To8LC
		}
	
	//Add atrributes
	AddAttributes2SdpL(*sdp, aSdpNum);
	
	return sdp;
	}

CKeyStreamSink* CScafStep::CreateKeyStreamSinkLC(const TDesC& aFileName, const TDesC& aPrivPath)
/**
 	Creates a test key stream sink object.
 	@param aFilePath The output file name of the test key stream sink.
 	@param aPrivPath Stream Agents Private Folder Path
 	@return A pointer to the test key stream sink object. 
 			The ownership is transferred.
 */
	{
	HBufC* privFolder = GetFullPathLC(aPrivPath, aFileName);
	CTestKeyStreamSink* sink = CTestKeyStreamSink::NewL(*privFolder);
	CleanupStack::PopAndDestroy(privFolder);
	CleanupStack::PushL(sink);
	return sink;
	}

void CScafStep::CleanAgentsPrivateFolderL(const TDesC& aPrivatePath)
/**
 	Delete all files and folders under the private directory of the test agent.
 */
	{
	HBufC* agentPrivFol = GetFullPathLC(aPrivatePath, _L("*.*"));
	CFileMan *fm = CFileMan::NewL(iParent.Fs());
	CleanupStack::PushL(fm);
	
	TInt ret = fm->Delete(*agentPrivFol,0);
	if((ret != KErrNone)&&(ret != KErrNotFound)&&(ret != KErrPathNotFound))
		{
		User::Leave(ret);
		}
	CleanupStack::PopAndDestroy(2, agentPrivFol);
	}

 
void CScafStep::DeleteSdpDocAndCloseCodecPool(TAny* aSdpDoc)
/**
 	Delete the SDP document object and close the codec pool
 	@param aSdp The SDP object which will be deleted.
 */
	{
	delete reinterpret_cast<CSdpDocument *>(aSdpDoc);
	SdpCodecStringPool::Close();
	}

void CScafStep::CopyFile2AgentsPrivateFolderL(RFs& aFs, const TDesC& aFileName, const TDesC& aPrivPath)
/**
 	Copy a test file from Z drive to the private folder of the test agent server.
 	@param aFs File Server session.
 	@param aPrivPath Stream Agents Private Folder Path
 	@param aFileName The name of the file which lives in the folder of Z drive.
 */
	{
	//Gets the target file path
	HBufC* fTarget = GetFullPathLC(aPrivPath, aFileName);
	//Make sure that the path exists
	TInt err = aFs.MkDirAll(*fTarget);
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	//Get the file source path
	TFileName fSource(KDataFilesPath);
	fSource.Append(aFileName);
	
	//Create a file manager
	CFileMan *fm = CFileMan::NewL(aFs);
	CleanupStack::PushL(fm);
	
	//Copy the source file to the target
	User::LeaveIfError(fm->Copy(fSource, *fTarget));
	// Make the file writeable 
    User::LeaveIfError(fm->Attribs(*fTarget, 0, KEntryAttReadOnly, TTime(0), 0));
	CleanupStack::PopAndDestroy(2, fTarget);
	}

void CScafStep::AddAttributes2SdpL(CSdpMediaField& aSdp, TInt aSdpNum)
/**
 * Add attributes from the instance within the CSdpConfiguration array, where 
 * the attribute information is stored, into the SDP object being constructed.
 * @param aSdp The SDP object where the attribute is added.
 */
	{	
	TInt attrCount = iDecoderConfigurationArray[aSdpNum]->iAttributeArray.Count();

	RStringPool pool = SdpCodecStringPool::StringPoolL();
	
	for(TInt i=0; i < attrCount; ++i)
		{
		TPtrC pAttrType;
		pAttrType.Set(iDecoderConfigurationArray[aSdpNum]->iAttributeArray[i].iAttributeType);
		
		TPtrC pAttrName;
		pAttrName.Set(iDecoderConfigurationArray[aSdpNum]->iAttributeArray[i].iAttributeName);
		
		//Convert 16-bit to 8-bit
		TPtr8 ptrAttrName(Convert16To8LC(pAttrName));
		
		TPtrC pAttrValue;
		pAttrValue.Set(iDecoderConfigurationArray[aSdpNum]->iAttributeArray[i].iAttributeValue);
		
		//Convert 16-bit to 8-bit
		TPtr8 ptrAttrValue(Convert16To8LC(pAttrValue));
		
		if(pAttrType.Compare(_L("Format")))
			{
			RStringF attrName = pool.OpenFStringL(ptrAttrName);
			CleanupClosePushL(attrName);
			CSdpAttributeField *attribute = CSdpAttributeField::NewLC(attrName, ptrAttrValue);
			User::LeaveIfError((aSdp.AttributeFields()).Append(attribute));
			CleanupStack::Pop(attribute);
			}
		else
			{
			RStringF fmtpStr = pool.StringF(SdpCodecStringConstants::EAttributeFmtp, SdpCodecStringPool::StringTableL());
			CleanupClosePushL(fmtpStr);
			CSdpFmtAttributeField* fmtpAttribute = CSdpFmtAttributeField::NewLC(fmtpStr, ptrAttrName, ptrAttrValue);
			User::LeaveIfError(aSdp.FormatAttributeFields().Append(fmtpAttribute));
			CleanupStack::Pop(fmtpAttribute);
			}
		CleanupStack::PopAndDestroy(3);
		}//for
	}//End of function

void CScafStep::PrintErrorAndLeaveL(TDesC& aKey)
/**
 	Prints an error message and then leaves with not found error.
 	@param aKey The name of the key which is not found.
 */
	{
	ERR_PRINTF2(_L("Failed to read '%S' key of configuration"), &aKey);
	SetTestStepResult(EFail);
	User::Leave(KErrNotFound);
	}

TPtr8 CScafStep::Convert16To8LC(TDesC& aDes)
/**
 	Convert a 16-bit descriptor into 8-bit.
 	@param aVal The 16-bit descriptor.
 	@return The descriptor converted to 8-bit.
 */
	{
	HBufC8* buf = HBufC8::NewLC(aDes.Size());
	TPtr8 ptr(buf->Des());
	ptr.Copy(aDes);
	return ptr;
	}

HBufC* CScafStep::GetFullPathLC(const TDesC& aPath, const TDesC& aFileName)
/**
 	Create a fully qualified file path.
 	@param aPath The folder path of the file.
 	@param aFileName The name of the file.
 	@return A pointer to the fully qualified file path.
 */
	{
	HBufC* fullPath = HBufC::NewLC(aPath.Length()+aFileName.Length()+1);
	TPtr ptr(fullPath->Des());
	ptr.Copy(aPath);
	ptr[0] = iParent.Fs().GetSystemDriveChar();
	if(aFileName.Length()>0)
		{
		ptr.Append(aFileName);
		}
	return fullPath;
	}

CSraRightsObject* CScafStep::GetRightsObjectLC(const TDesC& aName, const TDesC& aPrivPath)
/**
 	Reads the given RO file and creates a RO object.
 	@param aName The name of the RO file.
 	@param aPrivPath Stream Agents Private Folder Path
 	@return The RO object.
 */
	{
	RFile f;
	HBufC* path = GetFullPathLC(aPrivPath, aName);
	User::LeaveIfError(f.Open(iParent.Fs(), *path, EFileRead));
	CleanupStack::PopAndDestroy(path);
	CleanupClosePushL(f);
	RFileReadStream stream(f);
	CleanupClosePushL(stream);
	CSraRightsObject* ro = CSraRightsObject::NewL(stream);
	CleanupStack::PopAndDestroy(2, &f);
	CleanupStack::PushL(ro);
	return ro;
	}

TVerdict CScafStep::doOOMTestL()
/**
 * 	Runs the test step under OOM Conditions checking that each heap allocation is fail safe
 */
	{	
	// Pre and Post test heap cell allocation counts
 	TInt cellCountAfter = 0;
	TInt cellCountBefore = 0;
	
	/**
	 * The loop tests each heap allocation under out of memory conditions to determine whether
	 * the framework cleans up correctly without leaking memory.
	 * 
	 * The 'for' loop does not have any completion criteria, so the loop breaks out as soon 
	 * as any of the following events occur:
	 * a) The pre and post heap cell counts mismatch signalling a memory leakage
	 * b) An unexpected leave (any leave with an error code other than 'KErrNoMemory')
	 * c) All heap allocations have been tested and the test returns 'KErrNone'
	 */
	for (TInt testCount = 0; ; ++testCount)
 		{
 		__UHEAP_MARK;
 		__UHEAP_SETFAIL(RHeap::EDeterministic, testCount+1);
 		cellCountBefore = User::CountAllocCells();
 		TRAPD(err, doTestL());
 		cellCountAfter = User::CountAllocCells();
 		__UHEAP_MARKEND;
 		
 		INFO_PRINTF3(_L("OOM Test %d: Status = %d"),testCount,err);
 		
 		if (err == KErrNone)
 			{
			INFO_PRINTF1(_L("OOM Test Finished"));
 			break;
 			}
 		else if(err == KErrNoMemory)
 			{
 			if (cellCountBefore != cellCountAfter)
 				{
 				ERR_PRINTF2(_L("OOM Test Result: Failed - Memory leakage on iteration %d"), testCount);
 				ERR_PRINTF2(_L("Pre-Test Heap Cell Count: %d"), cellCountBefore);
 				ERR_PRINTF2(_L("Post-Test Heap Cell Count: %d"), cellCountAfter);
 				SetTestStepResult(EFail);
 				break;
 				}
 			}
 		else
 			{
 			User::Leave(err);
 			break;
 			}
		}
	
	return TestStepResult();
	}

TVerdict CScafStep::doTestL()
	{
	return TestStepResult();
	}

void CScafStep::ReadTestConfigurationL()
	{
	TInt baseIndex=0;
	
	// Read SDP Configuration Data
	TName fMedia;
	TName fProtocol;
	TName fPort;
	TName fFormat;
	TName fConnAddr;
	
	fMedia.Format(KSdpMedia, baseIndex);
	fProtocol.Format(KSdpProtocol, baseIndex);
	fPort.Format(KSdpPort, baseIndex);
	fFormat.Format(KSdpFormat, baseIndex);
	fConnAddr.Format(KSdpConnAddr, baseIndex);
	
	TPtrC mediaData;
	TPtrC protocolData;
	TInt portData;
	TPtrC formatData;
	TPtrC connAddrData;
	
	while (GetStringFromConfig(ConfigSection(), fMedia, mediaData) &&
			GetStringFromConfig(ConfigSection(), fProtocol, protocolData) &&
			GetIntFromConfig(ConfigSection(), fPort, portData) &&
			GetStringFromConfig(ConfigSection(), fFormat, formatData))
		{
		
		CTestDecoderConfiguration* newSdpConfig = CTestDecoderConfiguration::NewLC();
		
		newSdpConfig->iMedia.Set(mediaData);
		newSdpConfig->iProtocol.Set(protocolData);
		newSdpConfig->iPort = portData;
		newSdpConfig->iFormat.Set(formatData);
		
		if(GetStringFromConfig(ConfigSection(), fConnAddr, connAddrData))
			{
			newSdpConfig->iConnAddr.Set(connAddrData);
			}
		
		// Read Stream Associated Agents Private Folder Path
		TName fPrivateFolderPath;
		TName fSingleProcessAgent;
		
		fPrivateFolderPath.Format(KScafConfigPrivateFolderPath, baseIndex);
		fSingleProcessAgent.Format(KScafConfigSingleProcessAgent, baseIndex);
		
		TPtrC privateFolderPath;
		TBool singleProcessAgent;
		
		if(GetStringFromConfig(ConfigSection(), fPrivateFolderPath, privateFolderPath) &&
			GetBoolFromConfig(ConfigSection(), fSingleProcessAgent, singleProcessAgent))
			{
			newSdpConfig->iPrivateFolderPath.Set(privateFolderPath);
			newSdpConfig->iSingleProcessAgent = singleProcessAgent;
			}
		
		// Read SDP Attribute Data
		TInt attributeIndex = 0;
		
		TName fAttributeType;
		TName fAttributeName;
		TName fAttributeValue;
		
		fAttributeType.Format(KSdpAttributeType,baseIndex,attributeIndex);
		fAttributeName.Format(KSdpAttributeName,baseIndex,attributeIndex);
		fAttributeValue.Format(KSdpAttributeValue,baseIndex,attributeIndex);
		
		TPtrC attributeType;
		TPtrC attributeName;
		TPtrC attributeValue;
		
		while (GetStringFromConfig(ConfigSection(), fAttributeType, attributeType) &&
				GetStringFromConfig(ConfigSection(), fAttributeName, attributeName) &&
				GetStringFromConfig(ConfigSection(), fAttributeValue, attributeValue))
				{
				TSdpAttribute newAttribute;
				
				newAttribute.iAttributeType.Set(attributeType);
				newAttribute.iAttributeName.Set(attributeName);
				newAttribute.iAttributeValue.Set(attributeValue);
				
				newSdpConfig->iAttributeArray.Append(newAttribute);
				
				attributeIndex++;
				fAttributeType.Format(KSdpAttributeType,baseIndex,attributeIndex);
				fAttributeName.Format(KSdpAttributeName,baseIndex,attributeIndex);
				fAttributeValue.Format(KSdpAttributeValue,baseIndex,attributeIndex);
				}
		
		iDecoderConfigurationArray.Append(newSdpConfig);
		
		CleanupStack::Pop(newSdpConfig);
		
		baseIndex++;
		fMedia.Format(KSdpMedia, baseIndex);
		fProtocol.Format(KSdpProtocol, baseIndex);
		fPort.Format(KSdpPort, baseIndex);
		fFormat.Format(KSdpFormat, baseIndex);
		fConnAddr.Format(KSdpConnAddr, baseIndex);
		}
	
	// Read Expected Key Stream Decoder Attribute Values
	TName fIsProgramProtected;
	TName fIsServiceProtected;
	TName fCanExport;
	TName fMustProtectIfRecording;
	TName fCanPlay;
	TName fContentId;
	TName fRightsIssuerUri;
	
	baseIndex = 0;
	
	fIsProgramProtected.Format(KScafConfigIsProgramProtected,baseIndex);
	fIsServiceProtected.Format(KScafConfigIsServiceProtected,baseIndex);
	fCanExport.Format(KScafConfigCanExport,baseIndex);
	fMustProtectIfRecording.Format(KScafConfigMustProtectIfRecording,baseIndex);
	fCanPlay.Format(KScafConfigCanPlay,baseIndex);
	fContentId.Format(KScafConfigContentId,baseIndex);
	fRightsIssuerUri.Format(KScafConfigRightsIssuerUri,baseIndex);
	
	TBool isProgramProtected;
	TBool isServiceProtected;
	TBool canExport;
	TBool mustProtectIfRecording;
	TBool canPlay;
	TPtrC contentId;
	TPtrC rightsIssuerUri;
	
	while(GetBoolFromConfig(ConfigSection(),fIsProgramProtected,isProgramProtected) &&
		GetBoolFromConfig(ConfigSection(),fIsServiceProtected,isServiceProtected) &&
		GetBoolFromConfig(ConfigSection(),fCanExport,canExport) &&
		GetBoolFromConfig(ConfigSection(),fMustProtectIfRecording,mustProtectIfRecording) &&
		GetBoolFromConfig(ConfigSection(),fCanPlay,canPlay) &&
		GetStringFromConfig(ConfigSection(),fContentId,contentId) &&
		GetStringFromConfig(ConfigSection(),fRightsIssuerUri,rightsIssuerUri))
		{
		TExpectedKeyStreamDecoderAttributes newExpectedValues;
		
		newExpectedValues.IsProgramProtected = isProgramProtected;
		newExpectedValues.IsServiceProtected = isServiceProtected;
		newExpectedValues.CanExport = canExport;
		newExpectedValues.MustProtectIfRecording = mustProtectIfRecording;
		newExpectedValues.CanPlay = canPlay;
		newExpectedValues.ContentId.Set(contentId);
		newExpectedValues.RightsIssuerUri.Set(rightsIssuerUri);
		
		iExpectedKeyStreamDecoderData.Append(newExpectedValues);
		
		baseIndex++;
		fIsProgramProtected.Format(KScafConfigIsProgramProtected,baseIndex);
		fIsServiceProtected.Format(KScafConfigIsServiceProtected,baseIndex);
		fCanExport.Format(KScafConfigCanExport,baseIndex);
		fMustProtectIfRecording.Format(KScafConfigMustProtectIfRecording,baseIndex);
		fCanPlay.Format(KScafConfigCanPlay,baseIndex);
		fContentId.Format(KScafConfigContentId,baseIndex);
		fRightsIssuerUri.Format(KScafConfigRightsIssuerUri,baseIndex);
		}
	
	// Read OOM Test Flag
	GetBoolFromConfig(ConfigSection(), KScafConfigOOMTest, iOOMTest);
	}

TBool CScafStep::CheckKeyStreamDecoderAttributesL(const CKeyStreamDecoder& aDecoder,
													const TExpectedKeyStreamDecoderAttributes& aExpectedData)
	{
	// Overall result of the attribute value checks (ETrue = Pass / EFalse = Fail)
	TBool result = ETrue;
	
	_LIT(KIsProgramProtectedDescription,"IsProgramProtected");
	_LIT(KIsServiceProtectedDescription,"IsServiceProtected");
	_LIT(KCanExportDescription,"CanExport");
	_LIT(KMustProtectIfRecordingDescription,"MustProtectIfRecording");
	_LIT(KCanPlayDescription,"CanPlay");
	_LIT(KContentIDDescription,"ContentID");
	_LIT(KRightIssuerURIDescription,"RightIssuerURI");
	
	// Query the stream agent to determine whether the program is protected
	if(!CompareBooleanAttributeL(aDecoder,
								EIsProgramProtected,
								KIsProgramProtectedDescription,
								aExpectedData.IsProgramProtected))
		{
		result = EFalse;
		}	
	
	// Query the stream agent to determine whether the whole service is protected
	if(!CompareBooleanAttributeL(aDecoder,
								EIsServiceProtected,
								KIsServiceProtectedDescription,
								aExpectedData.IsServiceProtected))
		{
		result = EFalse;
		}
	
	// Query the stream agent to determine whether the content can be exported
	if(!CompareBooleanAttributeL(aDecoder,
								ECanExport,
								KCanExportDescription,
								aExpectedData.CanExport))
		{
		result = EFalse;
		}
	
	// Query the stream agent to determine whether the content must be protected whilst recording
	if(!CompareBooleanAttributeL(aDecoder,
								EMustProtectIfRecording,
								KMustProtectIfRecordingDescription,
								aExpectedData.MustProtectIfRecording))
		{
		result = EFalse;
		}	
	
	// Query the stream agent to determine whether the content can be played
	if(!CompareBooleanAttributeL(aDecoder,
								ECanPlay,
								KCanPlayDescription,
								aExpectedData.CanPlay))
		{
		result = EFalse;
		}	
	
	// Query the stream agent to retrieve the Content ID
	if(!CompareStringAttributeL(aDecoder,
								EContentID,
								KContentIDDescription,
								aExpectedData.ContentId))
		{
		result = EFalse;
		}
	
	// Query the stream agent to retrieve the Rights Issuer URI
	if(!CompareStringAttributeL(aDecoder,
								ERightsIssuerURI,
								KRightIssuerURIDescription,
								aExpectedData.RightsIssuerUri))
		{
		result = EFalse;
		}
	
	return result;
	}

TBool CScafStep::CompareBooleanAttributeL(const CKeyStreamDecoder& aDecoder,
											const TAttribute& aAttributeEnum,
											const TDesC& aAttributeDescription,
											const TBool aExpectedValue)
	{
	TBool retrievedValue;
	
	// Query the stream agent to determine the actual value of the attribute
	TRAPD(err,aDecoder.GetAttributeL(aAttributeEnum,retrievedValue));
	
	if(err != KErrNone)
		{
		ERR_PRINTF4(_L("*** Thread %d: Failed to Retrieve '%S' Value - %d ***"),iThreadId,&aAttributeDescription,err);
		User::Leave(err);
		}
	
	if(retrievedValue != aExpectedValue)
		{
		INFO_PRINTF4(_L("Thread %d: Expected '%S' Value = %d"),iThreadId,&aAttributeDescription,aExpectedValue);
		INFO_PRINTF4(_L("Thread %d: Retrieved '%S' Value = %d"),iThreadId,&aAttributeDescription,retrievedValue);
		ERR_PRINTF3(_L("*** Thread %d: Unexpected '%S' Value ***"),iThreadId,&aAttributeDescription);
		return EFalse;
		}
	
	return ETrue;
	}

TBool CScafStep::CompareStringAttributeL(const CKeyStreamDecoder& aDecoder,
											const TStringAttribute& aAttributeEnum,
											const TDesC& aAttributeDescription,
											const TDesC& aExpectedValue)
	{
	TBool result = ETrue;
	
	// Query the stream agent to determine the actual value of the attribute
	HBufC* retrievedValue = aDecoder.GetStringAttributeLC(aAttributeEnum);
	
	if(retrievedValue->Des() != aExpectedValue)
		{
		INFO_PRINTF4(_L("Thread %d: Expected '%S' Value = %S"),iThreadId,&aAttributeDescription,&aExpectedValue);
		INFO_PRINTF4(_L("Thread %d: Retrieved '%S' Value = %S"),iThreadId,&aAttributeDescription,retrievedValue);
		ERR_PRINTF3(_L("*** Thread %d: Unexpected '%S' Value ***"),iThreadId,&aAttributeDescription);
		result = EFalse;
		}
	
	// Destroy the heap based descriptor
	CleanupStack::PopAndDestroy(retrievedValue);
	
	return result;
	}
