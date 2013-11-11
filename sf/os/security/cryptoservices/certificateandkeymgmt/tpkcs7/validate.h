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
*
*/


/**
 @file 
 @internalTechnology
*/

#if (!defined __TPKCS7_VALIDATE_H__)
#define __TPKCS7_VALIDATE_H__
#include <e32base.h>
#include "pkcs7signedobject.h"
#include <pkcs7contentinfo_v2.h>
#include <pkixcertchain.h>

class CTPKCS7Validator : public CActive
	{
public:
	CTPKCS7Validator (CPKIXCertChain* aChain, CPKIXValidationResult* aResult, TTime* aTime);
	~CTPKCS7Validator ();
	void doValidate();

private:
	void RunL ();
	void DoCancel ();

	CPKIXCertChain*			iChain;
	CPKIXValidationResult*	iResult;
	TTime*					iTime;
	};
#endif
