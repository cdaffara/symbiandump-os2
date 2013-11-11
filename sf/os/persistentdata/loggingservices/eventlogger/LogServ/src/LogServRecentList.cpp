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

#include "LogServRecentList.h"
#include <barsread.h>
#include <logcntdef.h>
#include <logcli.h>
#include <logengdurations.h>
#include <logfilterandeventconstants.hrh>
#include <logwraplimits.h>
#include "logservpanic.h"
#include "LogServRecentCondition.h"
#include "LogServSqlStrings.h"

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServRecentList (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServRecentList::~CLogServRecentList()
	{
	iConditions.ResetAndDestroy();
	iConditions.Close();
	}

void CLogServRecentList::ConstructL(TResourceReader& aReader)
	{
	iId = static_cast<TLogRecentList>(aReader.ReadInt8());
	iDuplicates = static_cast<TUint16>(aReader.ReadUint16());

	TInt count = aReader.ReadInt16();
	iConditions.ReserveL(count);
	while(count--)
		{
		CLogServRecentCondition* condition = CLogServRecentCondition::NewL(aReader);
		TInt err = iConditions.Append(condition);
        __ASSERT_ALWAYS(err == KErrNone, Panic(ELogArrayReserved));
		}
	}

CLogServRecentList* CLogServRecentList::NewL(TResourceReader& aReader)
	{
	CLogServRecentList* self = new(ELeave) CLogServRecentList;
	CleanupStack::PushL(self);
	self->ConstructL(aReader);
	CleanupStack::Pop();
	return self;
	}

#ifdef SYSLIBS_TEST

#pragma BullseyeCoverage off

//Used by the unit tests when SYSLIBS_TEST macro is defined.
//Creates a complex recent list with a lot of fields.
//This helps covering most of recent list related production code branches.
CLogServRecentList* CLogServRecentList::TestNewL()
	{
	CLogServRecentList* self = new(ELeave) CLogServRecentList;
	CleanupStack::PushL(self);
	self->TestConstructL();
	CleanupStack::Pop();
	return self;
	}

void CLogServRecentList::TestConstructL()
	{
	iId = KLogRctTListId;
	iDuplicates = ELogRemotePartyField | ELogNumberField | ELogDirectionField | ELogDurationTypeField |
	              ELogStatusField | ELogFlagsField | ELogSubjectField
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	              | ELogSimIdField
#endif	
	              ;
	TInt count = 7;//7 test recent list conditions added in the code bellow
	iConditions.ReserveL(count);
	//1
	CLogServRecentCondition* condition1 = CLogServRecentCondition::TestNewL(ELogDirectionField);
	(void)iConditions.Append(condition1);
	//2
	CLogServRecentCondition* condition2 = CLogServRecentCondition::TestNewL(ELogDurationTypeField);
	(void)iConditions.Append(condition2);
	//3
	CLogServRecentCondition* condition3 = CLogServRecentCondition::TestNewL(ELogNumberField);
	(void)iConditions.Append(condition3);
	//4
	CLogServRecentCondition* condition4 = CLogServRecentCondition::TestNewL(ELogRemotePartyField);
	(void)iConditions.Append(condition4);
	//5
	CLogServRecentCondition* condition5 = CLogServRecentCondition::TestNewL(ELogStatusField);
	(void)iConditions.Append(condition5);
	//6
	CLogServRecentCondition* condition6 = CLogServRecentCondition::TestNewL(ELogFlagsField);
	(void)iConditions.Append(condition6);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	//7
	CLogServRecentCondition* condition7 = CLogServRecentCondition::TestNewL(ELogSimIdField);
	(void)iConditions.Append(condition7);
#endif	
	}

#pragma BullseyeCoverage on

#endif//SYSLIBS_TEST

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/**
Checks whether the aEvent field values satisfy the conditions described in 
in this CLogServRecentList object.

@param aEvent Reference to a CLogEvent object which field values will be tested.
@return True If the aEvent field values satisfy the conditions, false otherwise.
*/
TBool CLogServRecentList::IsRecent(const CLogEvent& aEvent) const
	{
	//The implemented algorithm is:
	//  usedFieldBitMask    - bit mask, where the "1" bits correspond to the fields used in the "recent condition" objects - 
	//                        the iConditions array. Each "recent condition" object has "field" and "value" data members,
	//                        where the "field" data member value is a power of 2.
	//                        Some of the iConditions elemens may have the same field value.
	//  matchedFieldBitMask - bit mask, where the "1" bits correspond to a "recent condition" object which field value is 
	//                        equal to the corresponding field value of the aEvent object.
	//
	//  For example, the iConditions array contains three elements initialised as:
	//            [2, "Incomming"]
	//            [8, 1000550D]
	//            [2, "Incomming Alternate"]
	//  The aEvent object contains these fields initialized:
	//            [2, "Incomming"]
	//            [8, 1000550D]
	//  Then usedFieldBitMask value will be:    0000000A -  (2 | 8).
	//       matchedFieldBitMask value will be: 0000000A -  (2 | 8).
	//  So, the operation performed on particular matchedFieldBitMask bit is bitwise OR -
	//      in case if two "recent condition" objects use the same field (but have different field values), then the result
	//      of the matching operation with the corresponding aEvent field value will be OR-ed in that matchedFieldBitMask bit.
	TUint16 usedFieldBitMask = 0;
	TUint16 matchedFieldBitMask = 0;
	for(TInt i=iConditions.Count()-1;i>=0;--i)
		{
		const CLogServRecentCondition& condition = *iConditions[i];
		usedFieldBitMask |= condition.Field();
		if(condition.IsMatch(aEvent))
			{
			matchedFieldBitMask |= condition.Field();
			}
		}
	//If both usedFieldBitMask and matchedFieldBitMask are 0, that means - the iConditions array is empty.
	//The function returns true in this case.
	return usedFieldBitMask == matchedFieldBitMask;
	}

void CLogServRecentList::GetFilter(const CLogEvent& aEvent, CLogFilter& aFilter) const
	{
	TUint16 nullFieldsBitMask = 0;
	if	(iDuplicates & ELogContactField)
		{
		aFilter.SetContact(aEvent.Contact());
      	if (aEvent.Contact() == KLogNullContactId) 
			nullFieldsBitMask |= ELogContactField;
		}

	if	(iDuplicates & ELogDirectionField)
		{
		aFilter.SetDirection(aEvent.Direction());
		if (aEvent.Direction().Length() == 0) 
			nullFieldsBitMask |= ELogDirectionField;
		}

	if	(iDuplicates & ELogDurationTypeField)
		{
		aFilter.SetDurationType(aEvent.DurationType());
		if (aEvent.DurationType() == KLogDurationNone) 
			nullFieldsBitMask |= ELogDurationTypeField;
		}

	if	(iDuplicates & ELogEventTypeField)
		{
		aFilter.SetEventType(aEvent.EventType());
		if (aEvent.EventType() == KNullUid) 
			nullFieldsBitMask |= ELogEventTypeField;
		}

	if	(iDuplicates & ELogNumberField)
		{
		aFilter.SetNumber(aEvent.Number());
		if (aEvent.Number().Length() == 0) 
			nullFieldsBitMask |= ELogNumberField;
		}

	if	(iDuplicates & ELogRemotePartyField)
		{
		aFilter.SetRemoteParty(aEvent.RemoteParty());
		if (aEvent.RemoteParty().Length() == 0) 
			nullFieldsBitMask |= ELogRemotePartyField;
		}

	if	(iDuplicates & ELogStatusField)
		{
		aFilter.SetStatus(aEvent.Status());
		if (aEvent.Status().Length() == 0) 
			nullFieldsBitMask |= ELogStatusField;
		}

	if	(iDuplicates & ELogFlagsField)
		{
		aFilter.SetFlags(aEvent.Flags());
		if (aEvent.Flags() == KLogNullFlags) 
			nullFieldsBitMask |= ELogFlagsField;
		}

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	if	(iDuplicates & ELogSimIdField)
		{
		aFilter.SetSimId(aEvent.SimId());
      	if (aEvent.SimId() == KLogNullSimId) 
			nullFieldsBitMask |= ELogSimIdField;
		}
#endif	
	
	aFilter.SetNullFields(nullFieldsBitMask); 
	// This is a special case, if all the fields are null
	// In this case we want to be a duplicate of other events where all the fields are null
	if	(IsFilterEmpty(aFilter))
		aFilter.SetNullFields(iDuplicates);
	}

TBool CLogServRecentList::IsFilterEmpty(const CLogFilter& aFilter) const
	{
	return aFilter.EventType() == KNullUid &&
	       aFilter.RemoteParty().Length() == 0 &&
	       aFilter.Direction().Length() == 0 &&
	       aFilter.Status().Length() == 0 &&
	       aFilter.Contact() == KLogNullContactId &&
	       aFilter.Number().Length() == 0 
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	       &&
	       aFilter.SimId() == KLogNullSimId
#endif	       
	       ;
	}

