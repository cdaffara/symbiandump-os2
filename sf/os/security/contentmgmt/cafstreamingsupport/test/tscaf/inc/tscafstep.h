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
// Defines the basic test step for the Streaming CAF test harness
// 
//

/**
 @file 
 @internalComponent
 @test
*/

#ifndef TSCAFSTEP_H_
#define TSCAFSTEP_H_

#include <test/testexecutestepbase.h>
#include <caf/streaming/keystreamdecoder.h>
#include "testagentkeystreamdecoder.h"
#include "testsingleprocesskeystreamdecoder.h"
#include "testkeystreamsink.h"
#include "srarightsobject.h"
#include "tscafserver.h"
#include "tscafdefs.h"
#include <f32file.h>
#include <s32file.h>

using namespace StreamAccess;
class CSdpMediaField;
class CSdpDocument;

class TSdpAttribute
/**
 * Data storage class containing the information about a single SDP Attribute
 */
	{
public:
	TSdpAttribute();
	
	TPtrC	iAttributeType;
	TPtrC	iAttributeName;
	TPtrC	iAttributeValue;
	};


class CTestDecoderConfiguration : public CBase
/**
 * Container class storing the parameters and attributes retreived from the configuration file
 * that are required to construct a key stream decoder object 
 */
	{
public:
	static CTestDecoderConfiguration* NewLC();
	~CTestDecoderConfiguration();
	
	TBool	iSingleProcessAgent;
	TPtrC	iPrivateFolderPath;
	TPtrC	iMedia;					
	TPtrC	iProtocol;
	TInt	iPort;
	TPtrC	iFormat;
	TPtrC   iConnAddr; ///< SDP connection field's IP address
	
	// Array of sdp meta data attributes
	RArray<TSdpAttribute> iAttributeArray;
	
protected:
	CTestDecoderConfiguration();
	};


class TExpectedKeyStreamDecoderAttributes
	{
/**
 * Data storage class containing expected values for the attributes associated to a particular
 * Key Stream Decoder
 */
public:
	TExpectedKeyStreamDecoderAttributes();
	
	TBool IsProgramProtected;
	TBool IsServiceProtected;
	TBool CanExport;
	TBool MustProtectIfRecording;
	TBool CanPlay;
	
	TPtrC ContentId;
	TPtrC RightsIssuerUri;
	};	


class CScafStep : public CTestStep
/**
 	Base class used to provide handy functions available to test step classes.
 */
	{
public:
	virtual ~CScafStep();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	void CleanAgentsPrivateFolderL(const TDesC& aPrivatePath);
	CSdpDocument* CreateSdpDocumentLC();
	CSdpMediaField* CreateSdpLC(TInt aSdpNum);
	void AddMediaFieldL(CSdpDocument& aSdpDoc, const CSdpMediaField* aSdpKeyStream);
	CKeyStreamSink* CreateKeyStreamSinkLC(const TDesC& aFilePath, const TDesC& aPrivPath);
	void CopyFile2AgentsPrivateFolderL(RFs& aFs, const TDesC& aFileName, const TDesC& aPrivPath);
	HBufC* GetFullPathLC(const TDesC& aPath, const TDesC& aFileName);
	CSraRightsObject* GetRightsObjectLC(const TDesC& aName, const TDesC& aPrivPath);
	void ReadTestConfigurationL();
	TBool CheckKeyStreamDecoderAttributesL(const CKeyStreamDecoder& aDecoder, 
											const TExpectedKeyStreamDecoderAttributes& aExpectedData);
	
protected:
	CScafStep(CScafServer& aParent);
	TPtr8 Convert16To8LC(TDesC& aDes);
	TVerdict doOOMTestL();
	virtual TVerdict doTestL();
	TBool CompareBooleanAttributeL(const CKeyStreamDecoder& aDecoder,
									const TAttribute& aAttributeEnum,
									const TDesC& aAttributeDescription,
									const TBool aExpectedValue);
	TBool CompareStringAttributeL(const CKeyStreamDecoder& aDecoder,
									const TStringAttribute& aAttributeEnum,
									const TDesC& aAttributeDescription,
									const TDesC& aExpectedValue);
	
private:
	void AddAttributes2SdpL(CSdpMediaField& aSdp, TInt aSdpNum);
	void PrintErrorAndLeaveL(TDesC& aKey);
	static void DeleteSdpDocAndCloseCodecPool(TAny* aSdpDoc);
	
protected:
	CScafServer& iParent;
	TInt iThreadId;
	TBool iOOMTest;
	RPointerArray<CTestDecoderConfiguration> iDecoderConfigurationArray;
	RArray<TExpectedKeyStreamDecoderAttributes> iExpectedKeyStreamDecoderData;
	
private:
	CActiveScheduler *iActiveScheduler;
	};
#endif /*TSCAFSTEP_H_*/
