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

#ifndef __LOGFILTQ__
#define __LOGFILTQ__

#include <badesca.h>
#include <logwrap.h>
#include "LogDynBuf.h"
#include "LogCliServShared.h"

// Classes referenced
class CLogFilter;
class CLogFilterList;
class MLogServDatabaseTransactionInterface;

/**
This class provides only one public method (apart from constructor) - BuildExprLC().
BuildExprLC() accepts as an input parameter a list with view filters and constructs from them
a "WHERE" SQL expression that is returned to the caller into the RLogDynBuf output parameter. 

@see CLogFilterList
@see RLogDynBuf
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class TLogFilterExprBuilder
	{
public:
    TLogFilterExprBuilder(MLogServDatabaseTransactionInterface& aDatabase);
	void BuildExprLC(RLogDynBuf& aExpr, const CLogFilterList& aFilterList, const TDesC& aInitial, 
                     TLogFilterConstructionType aType = ELogFilterConstructFilterByFilterFieldByField);
	
private:
	void DoNextProcessFilterByFilterFieldByFieldL(RLogDynBuf& aExpr);
	void DoNextProcessFieldByFieldFilterByFilterL(RLogDynBuf& aExpr);
	void MakePredicatesL();
	void MakeConditionL(RLogDynBuf& aExpr);
	void MakeFlagPredicatesL();
	const TDesC& ColumnName() const;
    void MakeNullPredicate(TDes& aDes);
	void MakePredicateL(TDes& aDes, const CLogFilter& aFilter);
    TBool IsDuplicateNullField() const;
	TBool IsDuplicateField(const CLogFilter& aFilter) const;
	TBool IsFieldEmpty(const CLogFilter& aFilter) const;
	TBool IsDuplicate(const CLogFilter& aFilter1, const CLogFilter& aFilter2) const;
	void Invariant() const;

private:
	MLogServDatabaseTransactionInterface& iDatabase;
    const CLogFilterList* iFilterList;
    const TDesC* iInitial;
    TLogFilterConstructionType iConstructionType;
	TInt iFilterIndex;
	TUint16 iField; //It must be 16-bit. The iField value is read from a resource file. 
					//See CLogServRecentCondition::ConstructL() implementation. 
					//The iField type width then restricts the max number of field macro values - 
					//ELogContactField...ELogSimIdField. 
	TInt iFlagIndex;
    CDesCArrayFlat* iPredicateList;
	};

#endif
