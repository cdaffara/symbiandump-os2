/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declares test step classes for reuse cases of PKCS#10 tests.
*
*/


/**
 @file
 @internalTechnology
*/

#include <f32file.h> 
#include <x500dn.h>
#include <pkcs10.h>  
#include <pkcs10attr.h>
#include <pkcs9attr.h>
#include <unifiedkeystore.h>
#include <asn1dec.h>
#include <asn1enc.h>

#if (!defined __PKCS10MINORTESTS_STEP_H__)
#define __PKCS10MINORTESTS_STEP_H__
#include <test/testexecutestepbase.h>
#include "tpkcs10stepbasev2.h"
#include "tcertrequeststep.h"

_LIT(KPKCS10ReuseAttributeStep,"PKCS10ReuseAttribute");
_LIT(KPKCS10ReuseChallengePasswordAttr,"PKCS10ReuseChallengePasswordAttr");
_LIT(KPKCS10ReuseExtensionAttr,"PKCS10ReuseExtensionAttr");
_LIT(KPKCS10CancelRequestA,"PKCS10CancelRequestA");

// forward declarations:
class CCertificateRequestStep;

// Resuse test steps
class CPKCS10ReuseAttributeStep: public CCertificateRequestStep
{
	public:
	virtual void doTestL();
};

class CPKCS10ReuseChallengePasswordAttrStep: public CCertificateRequestStep
{
	public:
	virtual void doTestL();
};

class CPKCS10ReuseExtensionAttrStep: public CCertificateRequestStep
{
	public:
	virtual void doTestL();
};



class CPKCS10TesterActiveCancelRequestA : public CPKCS10TesterActive
{
	public:
 	CPKCS10TesterActiveCancelRequestA( CTestExecuteLogger& aLogger );
	void RunL();
	TVerdict doActiveCertRequestL(CCertificateRequestStep* aStep);
	
};

class CPKCS10CancelRequestAStep: public CCertificateRequestStep
{
	public:
	virtual void doTestL();
	
	//data
	CPKCS10TesterActiveCancelRequestA* 		iActiveObjTest;
};

#endif

