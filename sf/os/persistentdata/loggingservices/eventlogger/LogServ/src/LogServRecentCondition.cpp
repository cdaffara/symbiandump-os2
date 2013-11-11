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

#include <barsread.h>
#include <logcli.h>
#include <logfilterandeventconstants.hrh>
#include "LogServRecentCondition.h"
#include "logservpanic.h"
#include "LogServSqlStrings.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServRecentCondition (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServRecentCondition* CLogServRecentCondition::NewL(TResourceReader& aReader)
	{
	CLogServRecentCondition* self = new(ELeave)CLogServRecentCondition();
	CleanupStack::PushL(self);
	self->ConstructL(aReader);
	CleanupStack::Pop();
	return self;
	}

CLogServRecentCondition::~CLogServRecentCondition()
	{
	delete iString;
	}

TBool CLogServRecentCondition::IsMatch(const CLogEvent& aEvent) const
	{
	TBool match = EFalse;
	switch (iField)
		{
		case ELogContactField:
			match = (aEvent.Contact() == (TLogContactItemId )iValue);
			break;

		case ELogDirectionField:
			match = (iString && aEvent.Direction().CompareF(*iString) == 0);
			break;

		case ELogDurationTypeField:
			match = (aEvent.DurationType() == (TLogDurationType)iValue);
			break;

		case ELogEventTypeField:
			match = (aEvent.EventType() == TUid::Uid(iValue));
			break;

		case ELogNumberField:
			match = (iString && aEvent.Number().CompareF(*iString) == 0);
			break;

		case ELogRemotePartyField:
			match = (iString && aEvent.RemoteParty().CompareF(*iString) == 0);
			break;

		case ELogStatusField:
			match = (iString && aEvent.Status().CompareF(*iString) == 0);
			break;

		case ELogFlagsField:
			match = (aEvent.Flags() & iValue);
			break;

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		case ELogSimIdField:
			match = (aEvent.SimId() == (TSimId)iValue);
			break;
#endif			
			
		default:
			__ASSERT_DEBUG(EFalse, Panic(ELogUnknownField));
			break;
		}

	return match;
	}

void CLogServRecentCondition::ConstructL(TResourceReader& aReader)
	{
	iField = (TUint16)aReader.ReadUint16();
	TPtrC string(aReader.ReadTPtrC());
	iValue = aReader.ReadInt32();

	if (string.Length() > 0)
		{
		iString = string.AllocL();
		}
	}

#ifdef SYSLIBS_TEST

#pragma BullseyeCoverage off

//When SYSLIBS_TEST macro is defined, creates a test recent list condition.
//Used from the unit tests to cover many of the production code branches, otherwise uncovered,
//because the recent list conditions can be loaded only from the resource file.
CLogServRecentCondition* CLogServRecentCondition::TestNewL(TUint16 aField)
	{
	CLogServRecentCondition* self = new(ELeave)CLogServRecentCondition;
	CleanupStack::PushL(self);
	self->TestConstructL(aField);
	CleanupStack::Pop();
	return self;
	}

//When SYSLIBS_TEST macro is defined, creates a test recent list condition.
//Used from the unit tests to cover many of the production code branches, otherwise uncovered,
//because the recent list conditions can be loaded only from the resource file.
void CLogServRecentCondition::TestConstructL(TUint16 aField)
	{
	iField = aField;
	switch(iField)
		{
	case ELogDirectionField:
		iString = HBufC::NewL(16);
		*iString = KLogRctTListDirection;
		break;
	case ELogDurationTypeField:
		iValue = KLogRctTListDurationType;
		break;
	case ELogNumberField:
		iString = HBufC::NewL(16);
		*iString = KLogRctTListNumber;
		break;
	case ELogRemotePartyField:
		iString = HBufC::NewL(16);
		*iString = KLogRctTListRemoteParty;
		break;
	case ELogStatusField:
		iString = HBufC::NewL(16);
		*iString = KLogRctTListStatus;
		break;
	case ELogFlagsField:
		iValue = KLogRctTListFlags;
		break;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	case ELogSimIdField:
		iValue = KLogRctTListSimId;
		break;
#endif		
	default:
		__ASSERT_DEBUG(EFalse, Panic(ELogUnknownField));
		break;
		}
	}

#pragma BullseyeCoverage on

#endif//SYSLIBS_TEST
