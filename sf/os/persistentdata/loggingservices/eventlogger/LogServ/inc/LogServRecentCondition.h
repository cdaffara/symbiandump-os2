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

#ifndef __LOGSERVRECENTCONDITION_H__
#define __LOGSERVRECENTCONDITION_H__

#include <e32base.h>

// Classes referenced
class CLogEvent;
class TResourceReader;

/**
Holds an event field type and value pair.
Used for performing event field value matching operations.

For event field types, look at these macfro definitions:
	-ELogContactField;
	-ELogDirectionField;
	-ELogDurationTypeField;
	-ELogEventTypeField;
	-ELogNumberField;
	-ELogRemotePartyField;
	-ELogStatusField;
	-ELogStartTimeField;
	-ELogEndTimeField;
	-ELogFlagsField;
	-ELogSubjectField;
	-ELogLinkField;
	-ELogDataField;
	-ELogSimIdField;

@see CLogEvent
@see CLogServRecentList
@see CLogServRecentListManager
@internalComponent
*/
class CLogServRecentCondition : public CBase
	{
public:
	static CLogServRecentCondition* NewL(TResourceReader& aReader);
	static CLogServRecentCondition* TestNewL(TUint16 aField);
	~CLogServRecentCondition();

private:
	void ConstructL(TResourceReader& aReader);
	void TestConstructL(TUint16 aField);

public:
	TBool IsMatch(const CLogEvent& aEvent) const;
	inline TUint16 Field() const;

private:
	TUint16 iField; //It must be 16-bit. The iField value is read from a resource file. 
					//See ConstructL() implementation. The iField type width then restricts
					//the max number of field macro values - 
					//ELogContactField...ELogSimIdField. 
	TInt32 iValue;
	HBufC* iString;
	};

inline TUint16 CLogServRecentCondition::Field() const
	{
	return iField;
	}

#endif
