// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LOGFILTQ.H"
#include <logcli.h>
#include <logcntdef.h>
#include "logservpanic.h"
#include "LogServDatabaseTransactionInterface.h"
#include <logfilterandeventconstants.hrh>
#include "LogServCacheStrings.h"
#include "LogServCacheTypes.h"
#include "LogServSqlStrings.h"

#ifdef _DEBUG
#  define __LOGFILTQ_INVARIANT()  Invariant()
#else
#  define __LOGFILTQ_INVARIANT()  void(0)
#endif

/**
KFilterFields array contains all fields that can participate in a filter.
The term "field" refers a constant, which value is power of two.
Every "field" uniquely identifies one of the event properties.
The field values are used in the implementation of the TLogFilterExprBuilder class for initializing the 
iField data member, used to identify the currently processed event property value.  
@internalComponent 
*/
const TUint16 KFilterFields[] = 
	{
	ELogContactField,
	ELogDirectionField,
	ELogDurationTypeField,
	ELogEventTypeField,
	ELogNumberField,
	ELogRemotePartyField,
	ELogStatusField,
	ELogStartTimeField,
	ELogEndTimeField,
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	ELogSimIdField,		//The new ELogSimIdField must be before the flags: see 
						//DoNextProcessFieldByFieldFilterByFilterL() implementation and the assert.
#endif	
	ELogFlagsField
	};

/**
The size of the KFilterFields array.
@internalComponent 
*/
const TInt KFilterFieldsSize = sizeof(KFilterFields) / sizeof(KFilterFields[0]);

// Constants
const TInt KLogBlockSize = 128;
const TInt KLogPredicateListGranuality = 10;
const TInt KLogMaxPredicateLength = 128;
const TInt KLogNumberCharsToMatch = 9;

/**
Sets the iDatabase data member.
The rest of the data member is left uninitialized. They will be initialized later by the BuildExprLC() call.
@param aDatabase A reference to MLogServDatabaseTransactionInterface interface.  
*/
TLogFilterExprBuilder::TLogFilterExprBuilder(MLogServDatabaseTransactionInterface& aDatabase) :
    iDatabase(aDatabase)
	{
	}

/**
Builds the WHERE expresssion of an SQL statement used later to filter events from the Event table.
How the builder works? - BuildExprLC() gets as a parameter list of filters: CLogFilterList.
Each filter is an object of CLogFilter type. The CLogFilter class contains the same set of properties as the CLogEvent
class: phone number, SIM id, call type, etc. Some of the filter property values are set, defining the desired set
of events that can pass the filter.

For example, the filter list has two filters:
 - Filter 1: number is "1011", SimId is 200;
 - Filter 2: duration type is 2;
 
BuildExprLC() takes the filter list and depending on passed TLogFilterConstructionType type:
 - ELogFilterConstructFilterByFilterFieldByField
   Organises two loops: the outer loop iterates over all field types, the inner loop iterates over the filters;
 - ELogFilterConstructFieldByFieldFilterByFilter  
   Organises two loops: the outer loop iterates over the filters, the inner loop iterates over all field types;

On each inner loop iteration the current field value of the filter will be checked and if it is not null then a predicate 
will be created of form "<column name> = <field value>" and added to the list of predicates.
If the field value is null but it is allowed to include that field in the expression (the related bit of 
CLogFilter::iNullFields is set. This is tested by using CLogFilter::NullFields()), then the predicate format will be
"<column name> IS NULL".
So using the example filters above, the predicates lists will be:
 - "Number = '1011'", "SimId - 200";
 - "DType = 2";

At the end of each outer loop iteration the predicates will be "linked" using either "AND" if the field type is "flags"
or "OR" for the rest of the fields types.
So, after the first outer loop iteration, the constructed part of the WHERE expression will be:
 "Number = '1011' OR SimId = 200". 
After the second outer loop iteration the expression will be: "DType = 2".
Also, depending on the TLogFilterConstructionType type these expressions will be "linked" either by "AND" or "OR":
 "(Number = '1011' OR SimId = 200) AND (DType = 2)".
 
On the first outer loop iteration, the predicates expression will be prefixed by the string refered by the aInitial
parameter (that could be "WHERE " or " AND " or " OR ").

When the processing of the filters finishes, the ready for use expression will be in the aExpr out parameter.

@param aExpr Out parameter. If BuildExprLC() completes successfully, aExpr contrains the generated expression.
             The caller is responsible for the destruction of aExpr. BuildExprLC() will create aExpr and put it on
             the cleanup stack; 
       aFilterList Filters list;
       aInitial String that has to be added in front of the generated expresssion;
       aType Defines the order of the processing of the filters and the fields;
@leave KErrNoMemory Out of memory condition has occured.             
*/
void TLogFilterExprBuilder::BuildExprLC(RLogDynBuf& aExpr, const CLogFilterList& aFilterList, const TDesC& aInitial, 
                                        TLogFilterConstructionType aType)
	{
	aExpr.CreateLC(KLogBlockSize);
    iPredicateList = new (ELeave) CDesCArrayFlat(KLogPredicateListGranuality);
    CleanupStack::PushL(iPredicateList);

    iFilterList = &aFilterList;
    iInitial = &aInitial;
    iConstructionType = aType;
    iFilterIndex = 0;
    iField = KFilterFields[0];
    iFlagIndex = 0;
    
    __LOGFILTQ_INVARIANT();
    
    switch(iConstructionType)
        {
        case ELogFilterConstructFilterByFilterFieldByField:
            DoNextProcessFilterByFilterFieldByFieldL(aExpr);
            break;
        case ELogFilterConstructFieldByFieldFilterByFilter:
            DoNextProcessFieldByFieldFilterByFilterL(aExpr);
            break;
        default:
            __ASSERT_DEBUG(EFalse, Panic(ELogInvalidConstructionType));
            break;
        }

#ifdef _DEBUG
    iFilterList = NULL;
    iInitial = NULL;
    iFilterIndex = -1;
    iField = 0;
    iFlagIndex = -1;
#endif    
    CleanupStack::PopAndDestroy(iPredicateList);
	}

/**
Processes the filters and the fields.
Organises two loops:
 - the outer loop is on the fields;
 - the inner loop is on the filters; 

@param aExpr Out parameter. The place where the expression is generated. 
@leave KErrNoMemory Out of memory condition has occured.             

@see BuildExprLC() 
*/
void TLogFilterExprBuilder::DoNextProcessFilterByFilterFieldByFieldL(RLogDynBuf& aExpr)
	{
    __LOGFILTQ_INVARIANT();
	const TInt KCount = iFilterList->Count();
	for(TInt i=0;i<KFilterFieldsSize;++i)
	    {
	    iField = KFilterFields[i];
	    for(iFilterIndex=0; iFilterIndex<KCount; ++iFilterIndex)
	        {
	        iField != ELogFlagsField ? MakePredicatesL() : MakeFlagPredicatesL();  
	        }
        MakeConditionL(aExpr);
        iPredicateList->Reset();
	    }
	}

/**
Processes the filters and the fields.
Organises two loops:
 - the outer loop is on the filters; 
 - the inner loop is on the fields;
 
@param aExpr Out parameter. The place where the expression is generated. 
@leave KErrNoMemory Out of memory condition has occured.             

@see BuildExprLC() 
*/
void TLogFilterExprBuilder::DoNextProcessFieldByFieldFilterByFilterL(RLogDynBuf& aExpr)
	{
    __LOGFILTQ_INVARIANT();
    const TInt KCount = iFilterList->Count();
    for(iFilterIndex=0; iFilterIndex<KCount; ++iFilterIndex)
        {
        //Process all filter fields except flags.
    	for(TInt i=0;i<(KFilterFieldsSize-1);++i)
            {
    	    iField = KFilterFields[i];
            MakePredicatesL();
            }
    	//Process the flag fields last
	    iField = KFilterFields[KFilterFieldsSize - 1];
        __ASSERT_DEBUG(iField == ELogFlagsField, User::Invariant());
        MakeFlagPredicatesL();
        MakeConditionL(aExpr);
        iPredicateList->Reset();
        }
	}

/**
Called on each inner iteration from DoNextProcessFieldByFieldFilterByFilterL() and 
DoNextProcessFilterByFilterFieldByFieldL().
Generates a predicate in one of the following formats:
 - "<column name> = <field value>"
 - "<column name> IS NULL"
The generated predicate will be added to the predicates list (iPredicateList data member).

@leave KErrNoMemory Out of memory condition has occured.              

@see DoNextProcessFilterByFilterFieldByFieldL()
@see DoNextProcessFieldByFieldFilterByFilterL()
*/
void TLogFilterExprBuilder::MakePredicatesL()
	{
    __LOGFILTQ_INVARIANT();
	
	const CLogFilter* filter = iFilterList->At(iFilterIndex);
	__ASSERT_DEBUG(filter, Panic(ELogNullFilterInList1));

	// Check for null field in filter and if the field has already been used
	TBuf<KLogMaxPredicateLength> predicate;
	if (!IsFieldEmpty(*filter) && !IsDuplicateField(*filter))
		{
		MakePredicateL(predicate, *filter);
		if (predicate.Length() > 0)
			iPredicateList->AppendL(predicate);
		}

	// Add Null predicate if required and not already specified
	if (filter->NullFields() & iField && !IsDuplicateNullField())
		{
		predicate.Zero();
		MakeNullPredicate(predicate);
		if (predicate.Length() > 0)
			iPredicateList->AppendL(predicate);
		}

    __LOGFILTQ_INVARIANT();
	}

/**
Called on each outer loop iteration.
At this time, all generated predicates are in the predicates list (iPredicateList data member).
The predicates will be "linked" into the expression using "AND" for the "flags" and "OR" for the rest of the fields.
Depending on the TLogFilterConstructionType type (iConstructionType data member) either "AND" or "OR" will be used 
to "link" pedicates from two different predicates lists.  

@param aExpr Out parameter. The place where the expression is generated. 
@leave KErrNoMemory Out of memory condition has occured.

@see DoNextProcessFilterByFilterFieldByFieldL()
@see DoNextProcessFieldByFieldFilterByFilterL()
@see MakePredicatesL()
*/
void TLogFilterExprBuilder::MakeConditionL(RLogDynBuf& aExpr)
	{
    __ASSERT_DEBUG(iFilterList != NULL, User::Invariant());
    __ASSERT_DEBUG(iInitial != NULL, User::Invariant());
    __ASSERT_DEBUG(iConstructionType == ELogFilterConstructFilterByFilterFieldByField || iConstructionType == ELogFilterConstructFieldByFieldFilterByFilter, User::Invariant());
    __ASSERT_DEBUG(iPredicateList != NULL, User::Invariant());
	
	// Are there any predicates to add?
	TInt total = iPredicateList->MdcaCount();
	if (total == 0)
		return;

	// Add separator between conditions
	if(aExpr.Length() == 0)
	    {
	    aExpr.AppendL(*iInitial);
	    }
	else
		{
		switch(iConstructionType)
			{
		case ELogFilterConstructFilterByFilterFieldByField:
			aExpr.AppendL(KLogAnd);
			break;
		case ELogFilterConstructFieldByFieldFilterByFilter:
			aExpr.AppendL(KLogOr);
			break;
		default:
            __ASSERT_DEBUG(EFalse, Panic(ELogInvalidConstructionType));
		    break;
			}
		}

	aExpr.AppendL(KLogOpenBracket);

	// Add Predicates
	TInt count = 0;
	while(count < total)
		{
		// Add separator between predicates
		if (count > 0)
			{
			if (iField != ELogFlagsField)
				aExpr.AppendL(KLogOr);
			else
				aExpr.AppendL(KLogAnd);
			}

		aExpr.AppendL((*iPredicateList)[count]);
		count++;
		}

	// Close the brackets
	aExpr.AppendL(KLogCloseBracket);
	}

/**
Called on each inner loop iteration.
Processes the "flags" filter fields and generates predicates.

@leave KErrNoMemory Out of memory condition has occured.

@see DoNextProcessFilterByFilterFieldByFieldL()
@see DoNextProcessFieldByFieldFilterByFilterL()
*/
void TLogFilterExprBuilder::MakeFlagPredicatesL()
    {
    __LOGFILTQ_INVARIANT();
    
    const CLogFilter* filter = iFilterList->At(iFilterIndex);
    __ASSERT_DEBUG(filter, Panic(ELogNullFilterInList2));

    // Return if no flags are set
    if (filter->Flags() == KLogNullFlags)
        return;

    TBuf<KLogMaxPredicateLength> predicate;

    // Go through each bit in turn
    iFlagIndex = KLogFlagsCount;
    while(iFlagIndex--)
        {
        // See if the current flag is set in filter and bit wasn't set in any previous filters
        if (filter->Flags() & 0x1 << iFlagIndex && !IsDuplicateField(*filter))
            {
            // Generate predicate - if null field flag set we don't want the flag to be set
            predicate.Format(KLogFlagPredicate, &KLogFlagString, iFlagIndex + 1, (filter->NullFields() & iField) ? 0 : 1);
            iPredicateList->AppendL(predicate);
            }
        }
    iFlagIndex = 0;
    
    __LOGFILTQ_INVARIANT();
    }

/**
Depending on the currently processed field (iField data member) the function returns the column name that
has to be used in the predicate being generated.

@return A const reference to the column name.

@see MakeNullPredicate()
@see MakePredicateL()   
*/
const TDesC& TLogFilterExprBuilder::ColumnName() const
	{
    __LOGFILTQ_INVARIANT();
	switch (iField)
		{
		case ELogContactField:
			return KLogFieldEventContactString();
		case ELogDirectionField:
			return KLogFieldEventDirectionString();
		case ELogDurationTypeField:
			return KLogFieldEventDTypeString();
		case ELogEventTypeField:
			return KLogFieldEventTypeString();
		case ELogNumberField:
			return KLogFieldEventNumberString();
		case ELogRemotePartyField:
			return KLogFieldEventRemoteString();
		case ELogStatusField:
			return KLogFieldEventStatusString();
		case ELogSubjectField:
			return KLogFieldEventSubjectString();
		case ELogLinkField:
			return KLogFieldEventLinkString();
		case ELogDataField:
			return KLogFieldEventDataString();
		case ELogStartTimeField:
		case ELogEndTimeField:
			return KLogFieldEventTimeString();
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		case ELogSimIdField:
			return KLogFieldEventSimId();
#endif			
		default:
			__ASSERT_DEBUG(EFalse, Panic(ELogNoSuchState3));
			break;
		}
	return KLogUnknownString();
	}

/**
Generates a predicate of form "<column name> IS NULL" in the passed descriptor.

@param aDes Out parameter. The place where the predicate will be generated.

@see MakePredicatesL() 
*/
void TLogFilterExprBuilder::MakeNullPredicate(TDes& aDes)
    {
    __LOGFILTQ_INVARIANT();
    aDes.Format(KLogNullPredicate, &ColumnName());
    __LOGFILTQ_INVARIANT();
    }

/**
Generates a predicate of form "<column name> = <field value>" in the passed descriptor.

@param aDes Out parameter. The place where the predicate will be generated.
@param aFilter The filter where the field values will be picked from.
               The current field is identified by the value of the iField data member.

@leave The leaving codes from TTime::FormatL().

@see MakePredicatesL() 
*/
void TLogFilterExprBuilder::MakePredicateL(TDes& aDes, const CLogFilter& aFilter)
	{
    __LOGFILTQ_INVARIANT();
	const TDesC& columnName = ColumnName();
	switch (iField)
		{
		case ELogContactField:
			aDes.Format(KLogNumberPredicate, &columnName, aFilter.Contact());
			break;

		case ELogDirectionField:
			{
			TLogStringId id = iDatabase.DTICacheStrings().FindId(aFilter.Direction());
			aDes.Format(KLogNumberPredicate, &columnName, id);
			}
			break;

		case ELogDurationTypeField:
			aDes.Format(KLogNumberPredicate, &columnName, aFilter.DurationType());
			break;

		case ELogEventTypeField:
			{
			const TLogServCacheTypeEntry& entry = iDatabase.DTICacheTypes().FindByUid(aFilter.EventType());
			aDes.Format(KLogNumberPredicate, &columnName, entry.iEventTypeId);
			break;
			}

		// If the phone number in the filter is at least KLogNumberCharsToMatch long
		// then it does a wild card search for numbers that match the last KLogNumberCharsToMatch chars
		case ELogNumberField:
			{
			if (aFilter.Number().Length() < KLogNumberCharsToMatch)
				aDes.Format(KLogStringPredicate, &columnName, &aFilter.Number());
			else
				{
				TPtrC number(aFilter.Number().Right(KLogNumberCharsToMatch));
				aDes.Format(KLogLikePredicate, &columnName, &number);
				}
			break;
			}

		case ELogRemotePartyField:
			//We need to check RemoteParty string for any single quotes and 
			//add a second single quote to properly handle strings such as 
			//"Sam's Wife"	
		    {
			//If single quotes are found we need to modify the string
			TInt quoteIndex =  aFilter.RemoteParty().Locate('\'');			
			if( quoteIndex >= 0)
				{	
				//Allocate a buffer twice the length of the string to cater for 
				//the worst case when every character is a single quote
				TPtrC temp = aFilter.RemoteParty();
				TBuf<KLogMaxRemotePartyLength * 2> buf;

				//loop through and replace all single quotes
				//with two quote characters
				while(quoteIndex >= 0)
					{	
					_LIT(KQuoteStr, "''");
					//Append the characters before the single quote followed
					//by two quote characters
					buf.Append(temp.Left(quoteIndex));	
					buf.Append(KQuoteStr);

					//Set the substring to the remaining characters after the
					//single quote and look for the next single quote character
					temp.Set(temp.Mid(quoteIndex + 1));
					quoteIndex = temp.Locate('\'');
					}		

				//No quotes left so append the remaining data
				buf.Append(temp);
				aDes.Format(KLogStringPredicate, &columnName, &buf);
				}
			else
				{
				aDes.Format(KLogStringPredicate, &columnName, &aFilter.RemoteParty());
				}
		    }
			break;

		case ELogStatusField:
			{
			TLogStringId id = iDatabase.DTICacheStrings().FindId(aFilter.Status());
			aDes.Format(KLogNumberPredicate, &columnName, id);
			}
			break;
			
		case ELogStartTimeField:
		    {
			TTime time = aFilter.StartTime();
		    TBuf<KLogMaxDateLength> buf;
			time.FormatL(buf, LogUtils::DateFormatForLocale());
			aDes.Format(KLogDateAfterPredicate, &columnName, &buf);
		    }
			break;  

		case ELogEndTimeField:
		    {
			TTime time = aFilter.EndTime();
		    TBuf<KLogMaxDateLength> buf;
			time.FormatL(buf, LogUtils::DateFormatForLocale());
			aDes.Format(KLogDateBeforePredicate, &columnName, &buf);
		    }
			break;
			
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		case ELogSimIdField:
			aDes.Format(KLogUNumberPredicate, &columnName, aFilter.SimId());
			break;
#endif	
			
		default:
			__ASSERT_DEBUG(EFalse, Panic(ELogNoSuchState4));
			break;
		};
    __LOGFILTQ_INVARIANT();
	}

/**
Determines whether the current field identified by the value of the iField data member, is a duplicate, used/defined
by the previously processed filters. For example, if there are two filters and both define the "number" field value, then
only the "number" value from the first filter will be used.

@param aFilter The current filter being processed;
@return True the current field value has been already used in some of the previously processed filters, false otherwise.

@see IsDuplicateNullField()
@see MakePredicatesL() 
*/
TBool TLogFilterExprBuilder::IsDuplicateField(const CLogFilter& aFilter) const
    {
    __LOGFILTQ_INVARIANT();
    TInt count = iFilterIndex;
    while(count--)
        {
        const CLogFilter* testFilter = iFilterList->At(count);
        __ASSERT_DEBUG(testFilter, Panic(ELogNullFilterInList3));
        if (IsDuplicate(aFilter, *testFilter))
            return ETrue;
        }
    return EFalse;
    }

/**
Determines whether the current "null" field identified by the value of the iField data member, is a duplicate, used/defined
by the previously processed filters.

@param aFilter The current filter being processed;
@return True the current "null" field value has been already used in some of the previously processed filters, false otherwise.

@see IsDuplicateField()
@see MakePredicatesL() 
*/
TBool TLogFilterExprBuilder::IsDuplicateNullField() const
    {
    __LOGFILTQ_INVARIANT();
    TInt count = iFilterIndex;
    while(count--)
        {
        const CLogFilter* testFilter = iFilterList->At(count);
        __ASSERT_DEBUG(testFilter, Panic(ELogNullFilterInList4));
        if (testFilter->NullFields() & iField)
            return ETrue;
        }
    return EFalse;  
    }

/**
Determines whether the current field value, identified by the value of the iField data member, is a null value.

@param aFilter The current filter being processed;
@return True The current field value is null, false otherwise.

@see MakePredicatesL()  
*/
TBool TLogFilterExprBuilder::IsFieldEmpty(const CLogFilter& aFilter) const
	{
    __LOGFILTQ_INVARIANT();
	switch (iField)
		{
		case ELogContactField:
			return aFilter.Contact() == KLogNullContactId;

		case ELogDirectionField:
			return aFilter.Direction().Length() == 0;

		case ELogDurationTypeField:
			return aFilter.DurationType() == KLogNullDurationType;

		case ELogEventTypeField:
			return aFilter.EventType() == KNullUid;

		case ELogNumberField:
			return aFilter.Number().Length() == 0;

		case ELogRemotePartyField:
			return aFilter.RemoteParty().Length() == 0;

		case ELogStatusField:
			return aFilter.Status().Length() == 0;

		case ELogStartTimeField:
			return (aFilter.StartTime() == TTime(0));

		case ELogEndTimeField:
			return (aFilter.EndTime() == TTime(0));

		// These fields are not part of the filter
		case ELogSubjectField:
		case ELogLinkField:
		case ELogDataField:
			return ETrue;

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		case ELogSimIdField:
			return (aFilter.SimId() == KLogNullSimId);
#endif
			
		default:
			__ASSERT_DEBUG(EFalse, Panic(ELogNoSuchState5));
			break;
		};
	return ETrue;
	}

/**
The function checks whether the field identified by the value of the iField data member has the same value in the
passed two filter objects. 

@param  aFilter1 Filter 1
@param  aFilter1 Filter 2
@return True The values of the current field are identical in both filters, false otherwise. 
 
@see IsDuplicateField() 
*/
TBool TLogFilterExprBuilder::IsDuplicate(const CLogFilter& aFilter1, const CLogFilter& aFilter2) const
	{
	__LOGFILTQ_INVARIANT();
	switch (iField)
		{
		case ELogContactField:
			return aFilter1.Contact() == aFilter2.Contact();

		case ELogDirectionField:
			return aFilter1.Direction().CompareF(aFilter2.Direction()) == 0;

		case ELogDurationTypeField:
			return aFilter1.DurationType() == aFilter2.DurationType();

		case ELogEventTypeField:
			return aFilter1.EventType() == aFilter2.EventType();

		// The number fields in filters are considered to be duplicates if the last KLogNumberCharsToMatch chars match
		case ELogNumberField:
			{
			TPtrC number1(aFilter1.Number().Right(KLogNumberCharsToMatch));
			TPtrC number2(aFilter2.Number().Right(KLogNumberCharsToMatch));
			return number1.CompareF(number2) == 0;
			}

		case ELogRemotePartyField:
			return aFilter1.RemoteParty().CompareF(aFilter2.RemoteParty()) == 0;

		case ELogStatusField:
			return aFilter1.Status().CompareF(aFilter2.Status()) == 0;

		case ELogStartTimeField:
			return aFilter1.StartTime() == aFilter2.StartTime();

		case ELogEndTimeField:
			return aFilter1.EndTime() == aFilter2.EndTime();

		case ELogFlagsField:
			return aFilter2.Flags() & iFlagIndex;

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		case ELogSimIdField:
			return aFilter1.SimId() == aFilter2.SimId();
#endif			
			
		default:
			__ASSERT_DEBUG(EFalse, Panic(ELogNoSuchState6));
			break;
		};
	return EFalse;
	}

#ifdef _DEBUG
void TLogFilterExprBuilder::Invariant() const
    {
    __ASSERT_DEBUG(KFilterFields[KFilterFieldsSize - 1] == ELogFlagsField, User::Invariant());
    __ASSERT_DEBUG(iFilterList != NULL, User::Invariant());
    __ASSERT_DEBUG(iInitial != NULL, User::Invariant());
    __ASSERT_DEBUG(iConstructionType == ELogFilterConstructFilterByFilterFieldByField || iConstructionType == ELogFilterConstructFieldByFieldFilterByFilter, User::Invariant());
    __ASSERT_DEBUG((TUint)iFilterIndex < iFilterList->Count(), User::Invariant());
    TInt idx = KMaxTInt;
    for(idx=0;idx<KFilterFieldsSize && iField!=KFilterFields[idx];++idx)
    	{
    	}
    __ASSERT_DEBUG(idx < KFilterFieldsSize, User::Invariant());
    __ASSERT_DEBUG((iField & (iField - 1)) == 0, User::Invariant());
    __ASSERT_DEBUG((TUint)iFlagIndex < KLogFlagsCount, User::Invariant());
    __ASSERT_DEBUG(iPredicateList != NULL, User::Invariant());
    }
#endif
