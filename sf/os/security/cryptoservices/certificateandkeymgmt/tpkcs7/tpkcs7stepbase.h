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


/**
 @file 
 @internalTechnology
*/

#if (!defined __TPKCS7_STEP_BASE_H__)
#define __TPKCS7_STEP_BASE_H__
#include <test/testexecutestepbase.h>
#include "tpkcs7server.h"
#include <x509cert.h>

class CTPKCS7StepBase : public CTestStep
	{
public:
	CTPKCS7StepBase ();
	~CTPKCS7StepBase();
	inline void SetServer(CTPKCS7Server * aServer);

	// From CTestStep
	TVerdict			doTestStepPreambleL ();
	HBufC8*				readFileL (TPtrC tag);
protected:
	HBufC8*				iRawData;
	HBufC8*	            iSaltValue;
	HBufC8*	            iContentData;
	HBufC8*             iDigest;
	HBufC8*             iEncodedParams;
	CX509Certificate*	iRootCertificate;
	RFs					iFs;
	CTPKCS7Server* iServer;
	};
	
inline void CTPKCS7StepBase::SetServer(CTPKCS7Server* aServer)
 	{
 	iServer = aServer;
 	}
#endif
