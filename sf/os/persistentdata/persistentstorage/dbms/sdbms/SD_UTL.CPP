// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SD_STD.H"

GLDEF_C void Panic(TDbsPanic aPanic)
//
// Panic the client
//
	{
	_LIT(KCategory,"DBMS-server");
	Dbms::Panic(KCategory,aPanic);
	}

// Class Dbms

EXPORT_C void Dbms::Panic(const TDesC& aCategory,TInt aCode)
//
// Panic the client
// Outside of the server this calls User::Panic, within it calls the server and LEAVES!
//
	{
	__TRACE(aCategory);
	__TRACE(aCode);
	CDbsServer* server=CDbsServer::Instance();
	if (server)
		{
		server->Panic(aCategory,aCode);
		__LEAVE(KDbsLeavePanic);
		}
	else
		User::Panic(aCategory,aCode);
	}

// streaming functions

GLDEF_C void ExternalizeL(const TDbCol& aCol,RWriteStream& aStream)
	{
	aStream<<aCol.iName<<TUint8(aCol.iType)<<TInt32(aCol.iMaxLength)<<TUint8(aCol.iAttributes);
	}

GLDEF_C void ExternalizeL(const CDbColSet& aColSet,RWriteStream& aStream)
	{
	TInt cc=aColSet.Count();
	aStream.WriteInt32L(cc);
	for (TInt ii=0;++ii<=cc;)
		aStream<<aColSet[ii];
	}

GLDEF_C void InternalizeL(CDbColSet& aColSet,RReadStream& aStream)
	{
	__ASSERT(aColSet.Count()==0);
	TDbCol col;
	TPtr name(col.iName.Des());
	TInt cc=aStream.ReadInt32L();
	while (--cc>=0)
		{
		aStream>>name;
		col.iType=TDbColType(aStream.ReadUint8L());
		col.iMaxLength=aStream.ReadInt32L();
		col.iAttributes=aStream.ReadUint8L();
		aColSet.AddL(col);
		}
	}

GLDEF_C void ExternalizeL(const CDbKey& aKey,RWriteStream& aStream)
	{
	TInt cc=aKey.Count();
	aStream.WriteInt32L(cc);
	for (TInt ii=0;ii<cc;++ii)
		{
		const TDbKeyCol& col=aKey[ii];
		aStream<<col.iName<<TUint8(col.iOrder)<<TInt32(col.iLength);
		}
	aStream<<TUint8(aKey.Comparison())<<TUint8(aKey.IsUnique());
	}

GLDEF_C void InternalizeL(CDbKey& aKey,RReadStream& aStream)
	{
	__ASSERT(aKey.Count()==0);
	TDbKeyCol col;
	TPtr name(col.iName.Des());
	TInt cc=aStream.ReadInt32L();
	while (--cc>=0)
		{
		aStream>>name;
		TUint8 uInt = aStream.ReadUint8L();
		col.iOrder=TDbKeyCol::TOrder(uInt);
		col.iLength=aStream.ReadInt32L();
		aKey.AddL(col);
		}
	aKey.SetComparison(TDbTextComparison(aStream.ReadUint8L()));
	if (aStream.ReadUint8L())
		aKey.MakeUnique();
	}

GLDEF_C void ExternalizeL(const CDbNames& aNames,RWriteStream& aStream)
	{
	TInt cc=aNames.Count();
	aStream.WriteInt32L(cc);
	for (TInt ii=0;ii<cc;++ii)
		aStream<<aNames[ii];
	}

GLDEF_C void InternalizeL(CDbNames& aNames,RReadStream& aStream)
	{
	__ASSERT(aNames.Count()==0);
	TDbName name;
	TInt cc=aStream.ReadInt32L();
	while (--cc>=0)
		{
		aStream>>name;
		aNames.AddL(name);
		}
	}

/**
A helper function, used in
"RWriteStream& operator<<(RWriteStream& aStream,const CDbStrings& aStrings)".
@internalComponent
*/
GLDEF_C void ExternalizeL(const CDbStrings& aStrings,RWriteStream& aStream)
	{
	TInt cc=aStrings.Count();
	aStream.WriteInt32L(cc);
	for (TInt ii=0;ii<cc;++ii)
		aStream<<aStrings[ii];
	}

/** 
Represents a generic read/write DBMS string. It maps to a modifiable buffer descriptor 
with maximum size KDbMaxStrLen.

@see TBuf 
@internalComponent
*/
typedef TBuf<KDbMaxStrLen> TDbString;

/**
A helper function, used in
"RReadStream& operator>>(RReadStream& aStream,CDbStrings& aStrings)".
@internalComponent
*/
GLDEF_C void InternalizeL(CDbStrings& aStrings,RReadStream& aStream)
	{
	__ASSERT(aStrings.Count()==0);
	TDbString str;
	TInt cc=aStream.ReadInt32L();
	while (--cc>=0)
		{
		aStream>>str;
		aStrings.AddL(str);
		}
	}
