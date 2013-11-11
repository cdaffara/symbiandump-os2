/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkixvalidationresult.h"

EXPORT_C CPKIXValidationResult* CPKIXValidationResult::NewL()
	{
	CPKIXValidationResult* s = CPKIXValidationResult::NewLC();
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CPKIXValidationResult* CPKIXValidationResult::NewLC()
	{
	CPKIXValidationResult* s = new(ELeave) CPKIXValidationResult;
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

EXPORT_C CPKIXValidationResult::~CPKIXValidationResult()
	{
	}

CPKIXValidationResult::CPKIXValidationResult()
	: CPKIXValidationResultBase()
	{
	}

EXPORT_C const TValidationStatus CPKIXValidationResult::Error() const
	{
	return CPKIXValidationResultBase::Error();
	}

EXPORT_C const CArrayFixFlat<TValidationStatus>& CPKIXValidationResult::Warnings() const
	{
	return CPKIXValidationResultBase::Warnings();
	}

EXPORT_C const CArrayPtrFlat<CX509CertPolicyInfo>& CPKIXValidationResult::Policies() const
	{
	return CPKIXValidationResultBase::Policies();
	}

EXPORT_C const RPointerArray<CCertificateValidationWarnings>& CPKIXValidationResult::ValidationWarnings() const
	{
	return CPKIXValidationResultBase::ValidationWarnings();
	}
