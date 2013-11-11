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
//

#include <logcli.h>
#include <logwraplimits.h>

EXPORT_C CLogEventType* CLogEventType::NewL()
/** Creates a new event type detail object.
 
@return Pointer to the new event type detail object. */
	{
	CLogEventType* self = new(ELeave)CLogEventType;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

void CLogEventType::ConstructL()
	{
	iDescription = HBufC::NewL(KLogMaxDescriptionLength);
	}

EXPORT_C CLogEventType::~CLogEventType()
/** Frees all resources owned by the event type detail object. */
	{
	delete iDescription;
	}

void CLogEventType::InternalizeL(RReadStream& aStream)
	{
	aStream >> iUid;
	iLoggingEnabled = aStream.ReadInt32L();
	InternalizeBufL(aStream, iDescription);
	}

void CLogEventType::InternalizeBufL(RReadStream& aStream, HBufC*& aDes)
	{
	TPtr ptr(aDes->Des());
	HBufC* temp = HBufC::NewL(aStream, ptr.MaxLength());
	ptr.Zero();
	ptr.Copy(*temp);
	delete temp;
	}

void CLogEventType::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iUid;
	aStream.WriteInt32L(iLoggingEnabled);
	aStream << *iDescription;
	}

CLogEventType::CLogEventType()
: iLoggingEnabled(ETrue)
	{
	}

EXPORT_C void CLogEventType::Copy(const CLogEventType& aType)
/** Makes a copy of the specified event type.

@param aType The event type to be copied. */
	{
	SetUid(aType.Uid());
	SetDescription(aType.Description());
	SetLoggingEnabled(aType.LoggingEnabled());
	}

