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
// DBMS server and support classes
// 
//

#include "SD_STD.H"

// Class RDbsObject
void RDbsObject::OpenL(const RDbsObject& aDbs,TDbsFunction aFunction,const TIpcArgs* aArgs)
	{
	__ASSERT(!iHandle);
	iHandle=aDbs.SendReceiveL(TDbsFunction(aFunction|KDbsObjectReturn),aArgs);
	RDbs::operator=(STATIC_CAST(const RDbs&,aDbs));
	}

void RDbsObject::Close()
	{
	if (iHandle)
		{
		SendReceive(EDbsClose);
		iHandle=0;
		SetHandle(KNullHandle);		// set session handle
		}
	}

TInt RDbsObject::SendReceive(TDbsFunction aFunction,const TIpcArgs* aArgs) const
	{
	__ASSERT(iHandle);
	if(aArgs)
		return RDbs::SendReceive(DbsMessage(aFunction,iHandle),*aArgs);
	else
		return RDbs::SendReceive(DbsMessage(aFunction,iHandle),TIpcArgs());
	}

TInt RDbsObject::SendReceive(TDbsFunction aFunction) const
	{
	return SendReceive(aFunction,0);
	}

TInt RDbsObject::SendReceiveL(TDbsFunction aFunction,const TIpcArgs* aArgs) const
	{
	return __LEAVE_IF_ERROR(SendReceive(aFunction,aArgs));
	}

TInt RDbsObject::SendReceiveL(TDbsFunction aFunction) const
	{
	return SendReceiveL(aFunction,0);
	}

void RDbsObject::SendReceive(TDbsFunction aFunction,const TIpcArgs* aArgs,TRequestStatus& aStatus) const
	{
	__ASSERT(iHandle);
	if(aArgs)
		RDbs::SendReceive(DbsMessage(aFunction,iHandle),*aArgs,aStatus);
	else
		RDbs::SendReceive(DbsMessage(aFunction,iHandle),TIpcArgs(),aStatus);
	}

// Class TDbsParam

const TInt KBufGranularity=0x200;

TPtrC8 TDbsParam::PrepareLC(const CDbColSet& aColSet)
	{
	CBufFlat* buf=CBufFlat::NewL(KBufGranularity);
	CleanupStack::PushL(buf);
	RBufWriteStream strm(*buf);
	strm<<aColSet;
	return buf->Ptr(0);
	}

TPtrC8 TDbsParam::PrepareLC(const CDbKey& aKey)
	{
	CBufFlat* buf=CBufFlat::NewL(KBufGranularity);
	CleanupStack::PushL(buf);
	RBufWriteStream strm(*buf);
	strm<<aKey;
	return buf->Ptr(0);
	}

TPtrC8 TDbsParam::PrepareLC(const TDbLookupKey& aKey)
	{
	TInt tsize=0;
	const TDbLookupKey::SColumn* iter=aKey.First();
	const TDbLookupKey::SColumn* end=iter+aKey.Count();
	do
		{
		switch (iter->iType)
			{
		default:
			break;
		case EDbColText8:
			tsize+=iter->iDes8.iLength;
			break;
		case EDbColText16:
			tsize+=iter->iDes16.iLength<<1;
			break;
			};
		} while (++iter<end);
	//
	TInt size=(TUint8*)end-(TUint8*)&aKey;
	TDbLookupKey* key=(TDbLookupKey*)User::AllocLC(size+tsize);
	TUint8* text=Mem::Copy(key,&aKey,size);
	if (tsize)
		{
		TDbLookupKey::SColumn* iter=CONST_CAST(TDbLookupKey::SColumn*,key->First());
		TDbLookupKey::SColumn* end=(TDbLookupKey::SColumn*)text;
		do
			{
			switch (iter->iType)
				{
			default:
				break;
			case EDbColText8:
				{
				const TUint8* p=iter->iDes8.iPtr;
				iter->iDes8.iPtr=REINTERPRET_CAST(const TUint8*,(text-(TUint8*)key));
				text=Mem::Copy(text,p,iter->iDes8.iLength);
				}
				break;
			case EDbColText16:
				{
				const TUint16* p=iter->iDes16.iPtr;
				iter->iDes16.iPtr=REINTERPRET_CAST(const TUint16*,(text-(TUint8*)key));
				text=Mem::Copy(text,p,iter->iDes16.iLength<<1);
				}
				break;
				};
			} while (++iter<end);
		}
	return TPtrC8((TUint8*)key,size+tsize);
	}
