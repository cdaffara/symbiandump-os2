// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// LogPackage.h
// 
//

#include "logpackage.h"

// Constants
const TInt KLogPackageGranuality = 0x200;


CLogPackage::CLogPackage()
: iPtr(NULL, 0)
	{
	}

EXPORT_C CLogPackage::~CLogPackage()
	{
	delete iBuffer;
	}

void CLogPackage::ConstructL()
	{
	iBuffer = CBufFlat::NewL(KLogPackageGranuality);
	}

EXPORT_C CLogPackage* CLogPackage::NewL()
	{
	CLogPackage* self = new(ELeave)CLogPackage;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CLogPackage::SetLogEventL(const CLogEvent& aEvent)
	{
	RBufWriteStream stream(*iBuffer);
	stream << aEvent;
	stream.CommitL();
	}

EXPORT_C void CLogPackage::GetLogEventL(CLogEvent& aEvent) const
	{
	RBufReadStream stream(*iBuffer);
	stream >> aEvent;
	}

EXPORT_C void CLogPackage::SetLogEventTypeL(const CLogEventType& aType)
	{
	RBufWriteStream stream(*iBuffer);
	stream << aType;
	stream.CommitL();
	}

EXPORT_C void CLogPackage::GetLogEventTypeL(CLogEventType& aType) const
	{
	RBufReadStream stream(*iBuffer);
	stream >> aType;
	}

EXPORT_C void CLogPackage::SetLogConfigL(const TLogConfig& aConfig)
	{
	RBufWriteStream stream(*iBuffer);
	stream << aConfig;
	stream.CommitL();
	}

EXPORT_C void CLogPackage::GetLogConfigL(TLogConfig& aConfig) const
	{
	RBufReadStream stream(*iBuffer);
	stream >> aConfig;
	}

EXPORT_C void CLogPackage::SetLogFilterListL(const CLogFilterList& aFilterList)
	{
	RBufWriteStream stream(*iBuffer);
	aFilterList.ExternalizeL(stream);
	stream.CommitL();
	}

EXPORT_C void CLogPackage::GetLogFilterListL(CLogFilterList& aFilterList) const
	{
	RBufReadStream stream(*iBuffer);
	aFilterList.InternalizeL(stream);
	}
