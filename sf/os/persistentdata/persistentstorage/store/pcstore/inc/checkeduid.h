// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__CHECKEDUID_H__)
#define __CHECKEDUID_H__

#include <pcstore/pcstoredef.h>
#include <pcstore/uid.h>

namespace PCStore
{
class CDes8;

/**
@internalComponent

Encapsulates a set of three unique identifiers (UIDs) which, in combination, 
identify a system object such as a GUI application or a DLL. The three
component UIDs are referred to as UID1, UID2 and UID3.

An object of this type is referred to as a compound identifier or a UID type.
*/
class TUidType
	{
public:
	TUidType();
	TUidType(TUid aUid1);
	TUidType(TUid aUid1, TUid aUid2);
	TUidType(TUid aUid1, TUid aUid2, TUid aUid3);
	TBool operator !=(const TUidType& aUidType) const;
	TBool operator ==(const TUidType& aUidType) const ;
	const TUid& operator[](TInt aIndex) const;
	TBool IsValid() const ;

private:
	TUid MostDerived() const;

private:
	TUid iUid[3];
	};


/**
@internalComponent

Packages a Uid type together with a checksum.

@see TUidType
*/
class TCheckedUid
	{
public:
	TCheckedUid();
	TCheckedUid(const TUidType& aUidType);
	TCheckedUid(const CDes8& aDes);
	
	void Set(const TUidType& aUidType);
	void Set(const CDes8& aDes);
	CDes8 Des() const;
	const TUidType&  UidType() const;

private:
	TUint Check() const;

private:
	TUidType iType;
	TUint32 iCheck;
	};

class CStoreWriteStream;
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TCheckedUid& aUid);

class CStoreReadStream;
CStoreReadStream& operator>>(CStoreReadStream& aStream, TCheckedUid& aUid);
}
#endif // !defined(__CHECKEDUID_H__)
