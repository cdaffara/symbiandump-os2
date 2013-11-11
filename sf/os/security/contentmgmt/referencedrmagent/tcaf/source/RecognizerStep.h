/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#if (!defined __RECOGNIZER_STEP_H__)
#define __RECOGNIZER_STEP_H__
#include <test/testexecutestepbase.h>

#include "cafstep.h"

// Constants used to name test cases 
_LIT(KCAFRecognizeStep,"CAFRecognizeStep");
_LIT(KCAFRecognizerSpeedStep,"CAFRecognizerSpeedStep");
_LIT(KCAFBufferSizeStep,"CAFBufferSizeStep");
_LIT(KCAFApparcStep,"CAFApparcStep");
_LIT(KCAF_DEF077443_Step,"CAF_DEF077443_Step");
_LIT(KCAF_DEF078413_Step,"CAF_DEF078413_Step");

class CCAFServer;
class CApaDataRecognizerType;

/* 
* Used to test the CAgentResolver::DoRecognize() function
* The ini file used in this test expects the following parameters to be specified
*
* FileName - full path and filename of the file to be investigated
*
* Container - the container mime type expected
*
* Content - the content mime type expected 
*
* Recognized - whether or not CAF should recognize this file
*
* If an agent recognizes the file, it fills in what it thinks the container and content mime 
* types of the file are. The test verifies:
* The file was recognized correctly (either recognized, or not recognized)
* If it was recognized verify that the container mime type is correct
* If it was recognized verify that the content mime type is correct
* @internalComponent Exposure internally
*/
class CCAFRecognizeStep : public CCAFStep
	{
public:
	CCAFRecognizeStep(CCAFServer& aParent);
	~CCAFRecognizeStep();
	virtual TVerdict doTestStepL();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    TVerdict doWmdrmTestStepL();     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

private:
	void CheckResultL(TBool aResult, TDes8& aContainerMimeType, TDes8& aContentMimeType, TBool aExpectedResult, TDesC16& aExpectedContainerMime, TDesC16& aExpectedContentMime);
	CCAFServer& iParent;
	};

/* 
* Used to test the speed of the CAgentResolver::DoRecognize() function
* Creates and instance of CAgentResolver and calls DoRecognize() 1000 times for the file
* specified in the INI file.
* The ini file used in this test only needs one parameter specified
*
* FileName - full path and filename of the file to be investigated
*
* Timing information in the TestExecute log file indicates how long this takes
* @internalComponent Exposure internally
*/
class CCAFRecognizerSpeedStep : public CCAFStep
	{
public:
	CCAFRecognizerSpeedStep(CCAFServer& aParent);
	~CCAFRecognizerSpeedStep ();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
* Used to test CAgentResolver::PreferredBufferSize() function
* CAgentResolver should use the static configuration data in each agent's resource file
* to determine the buffer size required for apparc recognition
* @internalComponent Exposure internally
*/
class CCAFBufferSizeStep : public CCAFStep
	{
public:
	CCAFBufferSizeStep(CCAFServer& aParent);
	~CCAFBufferSizeStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


/* 
This test verifys that the CApaCafRecognizer correctly identifies the mime type 
* The ini file specifies two parameters
*
* FileName - the file to recognize 
*
* CafMimeType - the expected mime type returned from RecognizeL()
* 
* @internalComponent Exposure internally
*/
class CCAFApparcStep : public CCAFStep
	{
public:
	CCAFApparcStep(CCAFServer& aParent);
	~CCAFApparcStep();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};

/* 
This test verifies that upper case Mime types can be recognized.
*
* See DEF077443: Propagated:CAF should not be performing case sensitive comparisons on MIME types
* 
* @internalComponent Exposure internally
*/
class CCAF_DEF077443_Step : public CCAFStep
	{
public:
	CCAF_DEF077443_Step(CCAFServer& aParent);
	~CCAF_DEF077443_Step();
	virtual TVerdict doTestStepL();
private:
	CCAFServer& iParent;
	};


#endif
