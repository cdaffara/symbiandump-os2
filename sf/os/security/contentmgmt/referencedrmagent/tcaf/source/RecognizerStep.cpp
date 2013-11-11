/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* cafstep.cpp
*
*/


#include <test/testexecutelog.h>
#include <apgcli.h>
#include <apmstd.h>
#include "cafserver.h"
#include "resolver.h"
#include "RecognizerStep.h"
#include "CafApaRecognizer.h"

using namespace ContentAccess;

const TInt KCafTestMaxDataTypeLength = 255;
const TInt KCAFTestApparcBufferSize = 100;

/* 
 * This step starts the CAF Apparc recognizer and checks to see that it recognizes the
 * correct files
 *
 */

CCAFRecognizeStep::~CCAFRecognizeStep()
	{
	}

CCAFRecognizeStep::CCAFRecognizeStep(CCAFServer& aParent)
: iParent(aParent)
	{
	SetTestStepName(KCAFRecognizeStep);
	}

/* Tests whether a file opened under the caf framework reports the same size as
 * RFile.  Only works for files that are owned by the f32agent which doesn't
 * change the content at all.
 */
TVerdict CCAFRecognizeStep::doTestStepL()
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    TBool wmdrmFlag = EFalse;     
    GetBoolFromConfig(ConfigSection(),_L("wmdrmEnabled"), wmdrmFlag);     
         
    if(wmdrmFlag)
        {     
        TVerdict verdict = doWmdrmTestStepL();     
        return verdict;     
        }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT    

	TBuf8 <KCAFTestApparcBufferSize> buf;

	CAgentResolver *resolver;
	
	// If we leave before our DoRecognize is complete, something must have gone seriously wrong
	SetTestStepResult(EFail);

	TBuf8 <KCafTestMaxDataTypeLength> ContainerMimeType;
	TBuf8 <KCafTestMaxDataTypeLength> ContentMimeType;
	TBool result;
	
	TPtrC fileName;
	TBool expectedresult;
	TPtrC expectedContainerMime, expectedContentMime;

	// Retrieve filename to analyse and expected results from INI file
	GetStringFromConfig(ConfigSection(),_L("FileName"),fileName);
	GetStringFromConfig(ConfigSection(),_L("Container"),expectedContainerMime);
	GetStringFromConfig(ConfigSection(),_L("Content"),expectedContentMime);
	GetBoolFromConfig(ConfigSection(),_L("Recognized"),expectedresult);
	
	if(expectedresult)
		{
		INFO_PRINTF4(_L("DoRecognize Test DRM file: %S, Container Mime Type: %S, Content Mime Type: %S"),&fileName, &expectedContainerMime, &expectedContentMime);
		}
	else
		{
		INFO_PRINTF2(_L("DoRecognize Test non DRM file: %S"), &fileName);	
		}

	__UHEAP_MARK;

	// Read the first KCAFTestApparcBufferSize bytes into the buffer in the same way apparc would do
	ReadBufferL(fileName, buf);
	
	// Pass the filename and buffer to CAF DoRecognize function
	resolver = CAgentResolver::NewLC(ETrue);
    	
	result = resolver->DoRecognizeL(fileName, buf, ContainerMimeType, ContentMimeType);

	CheckResultL(result, ContainerMimeType, ContentMimeType, expectedresult, expectedContainerMime, expectedContentMime);

	CleanupStack::PopAndDestroy(resolver); 

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CCAFRecognizeStep::CheckResultL(TBool aResult, TDes8& aContainerMimeType, TDes8& aContentMimeType, TBool aExpectedResult, TDesC16& aExpectedContainerMime, TDesC16& aExpectedContentMime)
	{
	// start off by assuming recognition was ok, then check
	SetTestStepResult(EPass);
	
	if(aResult != aExpectedResult)
		{
		if(aResult)
			{
			INFO_PRINTF1(_L("File was incorrectly recognized as DRM"));	
			}
		else
			{
			INFO_PRINTF1(_L("File was incorrectly recognized as not DRM"));	
			}
		SetTestStepResult(EFail);
		return;
		}

	if(!aResult) // not a drm file so we don't care about the mime types
		return;
	
	TInt compare;

	// Convert TDes16 mime types read from the INI file to TPtr8's
	HBufC8 *container = ConvertDes16toHBufC8LC(aExpectedContainerMime);
	TPtr8 containerptr(container->Des());

	HBufC8 *content = ConvertDes16toHBufC8LC(aExpectedContentMime);
	TPtr8 contentptr(content->Des());

	// Compare expected Mime Types vs mime type
	compare = aContainerMimeType.Compare(containerptr);
	if(compare != 0)
		{
		INFO_PRINTF1(_L("Incorrect Container Mime Type recognized"));	
		SetTestStepResult(EFail);
		}
	compare = aContentMimeType.Compare(contentptr);
	if(compare != 0)
		{
		INFO_PRINTF1(_L("Incorrect Content Mime Type recognized"));	
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(2, container);	
	}


/*
 * This step starts the CAF Apparc recognizer speed test
 * Does 1000 recognitions, log file will measure the time
 *
 */

CCAFRecognizerSpeedStep::~CCAFRecognizerSpeedStep()
	{
	}

CCAFRecognizerSpeedStep::CCAFRecognizerSpeedStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFRecognizerSpeedStep);
	}


TVerdict CCAFRecognizerSpeedStep::doTestStepL()
	{
	TBuf8 <KCAFTestApparcBufferSize> buf;

	CAgentResolver *resolver;
	
	// If we leave before our DoRecognize is complete, something must have gone seriously wrong
	SetTestStepResult(EFail);

	TBuf8 <KCafTestMaxDataTypeLength> ContainerMimeType;
	TBuf8 <KCafTestMaxDataTypeLength> ContentMimeType;
	
	TPtrC fileName;
	TBool expectedresult;
	TPtrC expectedContainerMime, expectedContentMime;

	// Retrieve filename to analyse and expected results from INI file
	GetStringFromConfig(ConfigSection(),_L("FileName"),fileName);
	GetStringFromConfig(ConfigSection(),_L("Container"),expectedContainerMime);
	GetStringFromConfig(ConfigSection(),_L("Content"),expectedContentMime);
	GetBoolFromConfig(ConfigSection(),_L("Recognized"),expectedresult);
	
	if(expectedresult)
		{
		INFO_PRINTF4(_L("DoRecognize Speed Test DRM file: %S, Container Mime Type: %S, Content Mime Type: %S"),&fileName, &expectedContainerMime, &expectedContentMime);
		}
	else
		{
		INFO_PRINTF2(_L("DoRecognize Speed Test non DRM file: %S"), &fileName);	
		}

	__UHEAP_MARK;

	// Read the first KCAFTestApparcBufferSize bytes into the buffer in the same way apparc would do
	ReadBufferL(fileName, buf);
	
	// Pass the filename and buffer to CAF DoRecognize function
	resolver = CAgentResolver::NewLC(ETrue);

	INFO_PRINTF1(_L("Entering measured mile"));	
	
	for(TInt Count=0; Count < 1000; Count++)
		resolver->DoRecognizeL(fileName, buf, ContainerMimeType, ContentMimeType);
	
	INFO_PRINTF1(_L("passing mile marker   (1000 recognitions)"));	

    CleanupStack::PopAndDestroy(resolver); 

	__UHEAP_MARKEND;
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/* 
 * This step starts the CAF Apparc recognizer and checks to see that it recognizes the
 * correct files
 *
 */

CCAFBufferSizeStep::~CCAFBufferSizeStep()
	{
	}

CCAFBufferSizeStep::CCAFBufferSizeStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFBufferSizeStep);
	}


/* Apparc uses a buffer to pass data from the start of the file into the apparc recognizer
* to help it determine what mime type the file is.
* In CAF this recognition task is actually handed over to the agents. Each one attempts to
* recognize the file until one is successful or until all agents have rejected the file.
* Each agent may have it's own preferred size for this buffer. This is configured in each
* agent's RSS file, under the default_data tag.
* CAgentResolver::PreferredBufferSize() will return the highest value returned by any agent.
*/
TVerdict CCAFBufferSizeStep::doTestStepL()
	{
	CAgentResolver *resolver;
	TInt expectedBufferSize;
	TInt bufferSize=0;

	SetTestStepResult(EFail);

	
	// Find the expected max buffer size from the INI file
	GetIntFromConfig(ConfigSection(),_L("size"),expectedBufferSize);

	INFO_PRINTF2(_L("Expected buffer size: %d"), expectedBufferSize);

	__UHEAP_MARK;

	
	resolver = CAgentResolver::NewLC(ETrue);

	bufferSize = resolver->PreferredBufferSize();

	INFO_PRINTF2(_L("Caf Preferred buffer size: %d"), bufferSize);

	if(bufferSize == expectedBufferSize)
		{	
		SetTestStepResult(EPass);
		}	
    		
	CleanupStack::PopAndDestroy(resolver); 

	__UHEAP_MARKEND;
	return TestStepResult();
	}



CCAFApparcStep::~CCAFApparcStep()
	{
	}

CCAFApparcStep::CCAFApparcStep(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAFApparcStep);
	}


/*
 * This step loads the apparc recognizer and gives it a test run by 
 * pretending to be apparc
 * 
 */
TVerdict CCAFApparcStep::doTestStepL()
	{
	TDataType dataType;
	TDataType dataType2;
	TDataType dataType3;
	TDataType dataTypeNull(_L8(""));	
	TPtrC8 mimeType(KNullDesC8);
	HBufC16 *displayMime;
	HBufC16 *displayMime2;
	TPtrC fileName;
	TPtrC uri;
	TPtrC expectedMimeType;
	TPtrC nullFileName(KNullDesC); 
	TUid uid = KNullUid;
	
	SetTestStepResult(EInconclusive);

	__UHEAP_MARK;

	// Retrieve filename to analyse and expected results from INI file
	GetStringFromConfig(ConfigSection(),_L("URI"),uri);
	GetStringFromConfig(ConfigSection(),_L("FileName"),fileName);
	GetStringFromConfig(ConfigSection(),_L("CafMimeType"),expectedMimeType);

	// Use the Application Architecture Server to find the Mime type 
	RApaLsSession apparcSession;
	User::LeaveIfError(apparcSession.Connect());
	CleanupClosePushL(apparcSession);
	User::LeaveIfError(apparcSession.AppForDocument(fileName, uid, dataType));

	RFile fileHandle;
	fileHandle.Open(iParent.Fs(), uri, EFileRead);
	User::LeaveIfError(apparcSession.AppForDocument(fileHandle, uid, dataType2));

	// Pass in a null file name to make sure it doesn't panic 
	User::LeaveIfError(apparcSession.AppForDocument(nullFileName, uid, dataType3));

	CleanupStack::PopAndDestroy(&apparcSession);	// close


	// check mime type of the file (fileName)
	mimeType.Set(dataType.Des8());
	displayMime = ConvertDes8toHBufC16LC(mimeType);
	TPtr16 displayPtr(displayMime->Des());
	if(displayPtr.Compare(expectedMimeType) != 0)
		{
		INFO_PRINTF2(_L("CAgentResolver returned a mime type of: %S"),&displayPtr);
		INFO_PRINTF1(_L("Please make sure the configuration file RecCafMimeTypes.txt exists for RECCAF.DLL."));
		SetTestStepResult(EFail);
		}
	else
		INFO_PRINTF3(_L("File - CAgentResolver returned a mime type of: %S, matching the expected mime type of: %S"),&displayPtr, &expectedMimeType);
	
		
	// check mime type of the file (fileHandle)
	mimeType.Set(dataType2.Des8());
	displayMime2 = ConvertDes8toHBufC16LC(mimeType);
	TPtr16 displayPtr2(displayMime2->Des());
	if(displayPtr2.Compare(expectedMimeType) != 0)
		{
		INFO_PRINTF2(_L("CAgentResolver returned a mime type of: %S"),&displayPtr2);
		INFO_PRINTF1(_L("Please make sure the configuration file RecCafMimeTypes.txt exists for RECCAF.DLL."));
		SetTestStepResult(EFail);
		}	
	else
		INFO_PRINTF3(_L("FileHandle - CAgentResolver returned a mime type of: %S, matching the expected mime type of: %S"),&displayPtr2, &expectedMimeType);
	
	// Check the returned datatype is null, when a null filename is passed in  
	if (dataType3 == dataTypeNull)
		INFO_PRINTF1(_L("A null datatype is returned, when a null file name is passed in"));
	else 
		SetTestStepResult(EFail);
	
	CleanupStack::PopAndDestroy(displayMime2);		
	CleanupStack::PopAndDestroy(displayMime);
	
	__UHEAP_MARKEND;
	
	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}

/* 
 * This test verifies that upper case Mime types can be recognized.
 *
 * See DEF077443: Propagated:CAF should not be performing case sensitive comparisons on MIME types
 *
 */
CCAF_DEF077443_Step::~CCAF_DEF077443_Step()
	{
	}

CCAF_DEF077443_Step::CCAF_DEF077443_Step(CCAFServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KCAF_DEF077443_Step);
	}

TVerdict CCAF_DEF077443_Step::doTestStepL()
	{
	TDataType dataType;
	TDataType dataType2;
	TPtrC8 mimeType(KNullDesC8);
	HBufC16 *displayMime;
	HBufC16 *displayMime2;
	TPtrC upperCaseFileName;
	TPtrC emptyFileName;
	TPtrC expectedContentMimeType;
	TPtrC expectedFileMimeType;
	TUid uid = KNullUid;
	
	SetTestStepResult(EInconclusive);

	__UHEAP_MARK;

	// Retrieve filename to analyse and expected results from INI file.
	// The CAF resolver forces mime types retrieved from agents to lower case.
	// When recognising the file mime type and content mine type for a file the
	// resolver passes the request to each agent. Its possible that the agent will
	// not use lower case for the file mime type and content mime type. To be 
	// consistent the resolver should set the returned data to lower case as well.
	
	// The test agent takes content mime type from the uppercasetest.drm file.
	// For this case the content mime type is upper case (e.g. TEXT/PLAIN).
	GetStringFromConfig(ConfigSection(),_L("FileName1"), upperCaseFileName);
	GetStringFromConfig(ConfigSection(),_L("CafContentMimeType"), expectedContentMimeType);

	// For a drm file with no recognised content the test agent sets the file mime type
	// as APPLICATION/TESTAGENT.DRM.
	// For this case the file emptytest.drm is used.
	GetStringFromConfig(ConfigSection(),_L("FileName2"), emptyFileName);
	GetStringFromConfig(ConfigSection(),_L("CafFileMimeType"), expectedFileMimeType);
	
	// create empty DRM file
	RFs fs;
	RFile file;

	// remove first if exists
	Delete(emptyFileName);

	fs.Connect();
	TInt result = file.Create(fs, emptyFileName, EFileWrite);
	file.Close();
	fs.Close(); 

	// Use the Application Architecture Server to find the Content Mime type 
	RApaLsSession apparcSession;
	User::LeaveIfError(apparcSession.Connect());
	CleanupClosePushL(apparcSession);
	User::LeaveIfError(apparcSession.AppForDocument(upperCaseFileName, uid, dataType));

	// Use the Application Architecture Server to find the File Mime type 
	User::LeaveIfError(apparcSession.AppForDocument(emptyFileName, uid, dataType2));

	CleanupStack::PopAndDestroy(&apparcSession);	// close

	// remove empty file
	Delete(emptyFileName);

	// check content mime type
	mimeType.Set(dataType.Des8());
	displayMime = ConvertDes8toHBufC16LC(mimeType);
	TPtr16 displayPtr(displayMime->Des());
	if(displayPtr.Compare(expectedContentMimeType) != 0)
		{
		INFO_PRINTF2(_L("CAgentResolver returned a content mime type of: %S"),&displayPtr);
		INFO_PRINTF1(_L("Please make sure the configuration file RecCafMimeTypes.txt exists for RECCAF.DLL."));
		SetTestStepResult(EFail);
		}
	else
		INFO_PRINTF3(_L("Content - CAgentResolver returned a mime type of: %S, matching the expected mime type of: %S"),&displayPtr, &expectedContentMimeType);
	
	// check file mime type
	mimeType.Set(dataType2.Des8());
	displayMime2 = ConvertDes8toHBufC16LC(mimeType);
	TPtr16 displayPtr2(displayMime2->Des());
	if(displayPtr2.Compare(expectedFileMimeType) != 0)
		{
		INFO_PRINTF2(_L("CAgentResolver returned a file mime type of: %S"),&displayPtr2);
		INFO_PRINTF1(_L("Please make sure the configuration file RecCafMimeTypes.txt exists for RECCAF.DLL."));
		SetTestStepResult(EFail);
		}	
	else
		INFO_PRINTF3(_L("File - CAgentResolver returned a mime type of: %S, matching the expected mime type of: %S"),&displayPtr2, &expectedFileMimeType);

	CleanupStack::PopAndDestroy(displayMime2);
	CleanupStack::PopAndDestroy(displayMime);
	
	__UHEAP_MARKEND;
	
	if (TestStepResult() != EFail)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}
	
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
  
// Tests DoRecognizeL API for WMDRM content .
      
TVerdict CCAFRecognizeStep::doWmdrmTestStepL()     
    {     
    SetTestStepResult(EFail);     
         
    TPtrC expectedFileMimeType;     
    GetStringFromConfig(ConfigSection(),_L("filemime"), expectedFileMimeType);     
         
    TPtrC expectedContentMimeType;     
    GetStringFromConfig(ConfigSection(),_L("contentmime"), expectedContentMimeType);     
         
    TBool expectedResult;     
    GetBoolFromConfig(ConfigSection(),_L("recognized"), expectedResult);     
         
    __UHEAP_MARK;     
    TPtrC header;     
    HBufC8* headerData = NULL;     
         
    if(GetStringFromConfig(ConfigSection(),_L("header"), header))     
        {     
        headerData = ConvertDes16toHBufC8LC(header);     
        }     
    else     
        {     
        headerData = CreateWmdrmHeaderLC();      
        }     
      
    // Pass the WMDRM header data to CAF DoRecognize function     
    CAgentResolver* resolver = CAgentResolver::NewLC(ETrue);     
             
    TBuf8 <KCafTestMaxDataTypeLength> fileMimeType;     
    TBuf8 <KCafTestMaxDataTypeLength> contentMimeType;     
             
    TBool result = resolver->DoRecognizeL(*headerData, fileMimeType, contentMimeType);     
         
    CheckResultL(result, fileMimeType, contentMimeType, expectedResult, expectedFileMimeType, expectedContentMimeType);     
      
    CleanupStack::PopAndDestroy(2, headerData);      
      
    __UHEAP_MARKEND;     
    return TestStepResult();     
    }     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

