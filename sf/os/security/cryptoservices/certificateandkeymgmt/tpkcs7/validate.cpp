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


#include "validate.h"

CTPKCS7Validator::CTPKCS7Validator (CPKIXCertChain* aChain, CPKIXValidationResult* aResult, TTime* aTime)
	: CActive (0), iChain (aChain), iResult (aResult), iTime (aTime)
	{
	CActiveScheduler::Add (this);
	}

CTPKCS7Validator::~CTPKCS7Validator ()
	{
	Cancel ();
	}

void CTPKCS7Validator::doValidate ()
	{
	iStatus = KRequestPending;
	iChain->ValidateL (*iResult, *iTime, iStatus);
	SetActive ();
	}

void CTPKCS7Validator::RunL ()
	{
	CActiveScheduler::Stop ();
	}

void CTPKCS7Validator::DoCancel ()
	{
	}
