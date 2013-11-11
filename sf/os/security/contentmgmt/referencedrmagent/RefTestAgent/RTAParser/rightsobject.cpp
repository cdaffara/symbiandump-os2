/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <s32strm.h>
#include <caf/caf.h>
#include "rightsobject.h"
#include "intent.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;

CRightsObject* CRightsObject::NewL(const TDesC8& aCid, TBool aForwardLock, TBool aPending, TInt aPendingDelay)
	{
	CRightsObject* self = new (ELeave) CRightsObject(aForwardLock, aPending, aPendingDelay);
	CleanupStack::PushL(self);
	self->ConstructL(aCid);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRightsObject* CRightsObject::NewL(RReadStream& aStream, TBool aHaveDeleteKey)
	{
	CRightsObject* self = new (ELeave) CRightsObject;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream, aHaveDeleteKey);
	CleanupStack::Pop(self);
	return self;
	}


CRightsObject::CRightsObject()
	{
	}

CRightsObject::CRightsObject(TBool aForwardLock, TBool aPending, TInt aPendingDelay)
 : iForwardLock(aForwardLock), iPending(aPending), iPendingDelay(aPendingDelay)
	{
	}

CRightsObject::~CRightsObject()
	{
	iIntents.Close();
	delete iCid;
	delete iDeleteKey;
	}

void CRightsObject::ConstructL(const TDesC8& aCid)
	{
	iCid = aCid.AllocL();
	}


EXPORT_C const TDesC8& CRightsObject::Cid()
	{
	return *iCid;
	}

EXPORT_C TInt CRightsObject::EvaluateIntent(TIntent aIntent)
	{
	TInt err = KErrCANoRights;
	if(iForwardLock)
		{
		err = KErrNone;
		}
	else if(iPending)
		{
		err = KErrCAPendingRights;
		}
	else
		{
		for(TInt i = 0; i < iIntents.Count(); i++)
			{
			err = KErrCANoPermission;		// at least one intent type 
			if(iIntents[i]->Intent() == aIntent)
				{
				err = iIntents[i]->Evaluate();
				break;
				}
			}
		}
	return err;
	}
	
EXPORT_C void CRightsObject::ExecuteIntentL(TIntent aIntent)
	{
	TInt err = KErrCANoRights;
	if(!iForwardLock)
		{
		if(iPending)
			{
			User::Leave(KErrCAPendingRights);
			}
		else
			{
			for(TInt i = 0; i < iIntents.Count(); i++)
				{
				err = KErrCANoPermission; // at least one intent type
				if(iIntents[i]->Intent() == aIntent)
					{
					iIntents[i]->ExecuteL();
					return;
					}
				}
			// couldn't find matching intent
			// leave with either KErrCANoRights if there were no rights
			// or KErrCANoPermission if the speicifed intent was not found
			User::Leave(err);
			}
		}
	}

void CRightsObject::AddIntentL(CIntent* aIntent)
	{
	TBool merged = EFalse;
	for(TInt i = 0; i < iIntents.Count(); i++)
		{
		if(iIntents[i]->Intent() == aIntent->Intent())
			{
			iIntents[i]->MergeL(aIntent);
			delete aIntent;
			aIntent = NULL;
			merged = ETrue;
			break;
			}
		}
	if(!merged)
		{
		iIntents.AppendL(aIntent);	
		}
	}

EXPORT_C void CRightsObject::WriteDescription(RFile& aFile)
	{
	TBuf8 <10> delay;
	aFile.Write(_L8("\r\ncid: "));
	aFile.Write(*iCid);
	aFile.Write(_L8("\r\n"));

	TPtrC8 deleteKey(KNullDesC8);
	if (iDeleteKey)
		{
		deleteKey.Set(*iDeleteKey);
		}
	aFile.Write(_L8("delete key: "));
	aFile.Write(deleteKey);
	aFile.Write(_L8("\r\n"));

	if(iForwardLock)
		{
		aFile.Write(_L8("	 Forward Lock use enabled\r\n"));
		}
	else if(iPending)
		{
		aFile.Write(_L8("	 Rights pending, expected in "));
		delay.Num(iPendingDelay);
		aFile.Write(delay);
		aFile.Write(_L8(" seconds\r\n"));
		}
	else
		{
		for(TInt i = 0; i < iIntents.Count(); i++)
			{
			iIntents[i]->WriteDescription(aFile);
			}
		}
	}

const RStreamablePtrArray <CIntent>& CRightsObject::IntentArray() const
	{
	return iIntents;	
	}


EXPORT_C void CRightsObject::MergeL(CRightsObject& aRightsObject)
	{
	if(!iForwardLock && !aRightsObject.Pending() && !aRightsObject.ForwardLock())
		{
		iPending = EFalse;
		iPendingDelay = 0;
		for(TInt i = 0; i < aRightsObject.IntentArray().Count(); i++)
			{
			CIntent* intent = aRightsObject.IntentArray()[i]->CopyL();
			CleanupStack::PushL(intent);
			AddIntentL(intent);
			CleanupStack::Pop(intent);
			}
		}
	else
		{
		// Content is already forward locked, adding the new 
		// rights object would only introduce restritions
		// so ignore it
		}

	// update iDeleteKey
	const TDesC8& newDeleteKey = aRightsObject.DeleteKey();
	if (iDeleteKey && newDeleteKey.Length())
		{
		delete iDeleteKey;
		iDeleteKey = NULL;
		}
	SetDeleteKeyL(newDeleteKey);
	}


TBool CRightsObject::ForwardLock()
	{
	return iForwardLock;
	}
	
TBool CRightsObject::Pending()
	{
	return iPending;
	}
	
TInt CRightsObject::PendingDelay()
	{
	return iPendingDelay;
	}

void CRightsObject::InternalizeL(RReadStream& aStream, TBool aHaveDeleteKey)
	{
	// read Cid length
	TInt cidLength = aStream.ReadInt32L();	
	iCid = HBufC8::NewL(cidLength);
	TPtr8 cidPtr = iCid->Des();
	aStream.ReadL(cidPtr, cidLength);
	
	iForwardLock = static_cast <TBool>(aStream.ReadInt32L());
	iPending = static_cast <TBool>(aStream.ReadInt32L());
	iPendingDelay = static_cast <TInt>(aStream.ReadInt32L());

	iIntents.InternalizeL(aStream);

	// Version 0 of rights.db does not have the deleteKey.
	if (aHaveDeleteKey)
		{
		if (iDeleteKey)
			{
			delete iDeleteKey;
			iDeleteKey = NULL;
			}

		TInt deleteKeyLen = aStream.ReadInt32L();
		if (deleteKeyLen)
			{
			iDeleteKey = HBufC8::NewL(deleteKeyLen);
			TPtr8 ptr8 = iDeleteKey->Des();
			aStream.ReadL(ptr8, deleteKeyLen);
			}
		}
	}
	
EXPORT_C void CRightsObject::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteInt32L(iCid->Des().Length());
	TPtr8 cidPtr = iCid->Des();
	aStream.WriteL(cidPtr);
	aStream.WriteInt32L(static_cast <TInt32>(iForwardLock));
	aStream.WriteInt32L(static_cast <TInt32>(iPending));
	aStream.WriteInt32L(static_cast <TInt32>(iPendingDelay));
	
	iIntents.ExternalizeL(aStream);

	TInt deleteKeyLen = iDeleteKey ? iDeleteKey->Length() : 0;
	aStream.WriteInt32L(deleteKeyLen);
	if (deleteKeyLen)
		{
		aStream.WriteL(*iDeleteKey);
		}
	}

EXPORT_C void CRightsObject::SetDeleteKeyL(const TDesC8& aDeleteKey)
	{
	if (aDeleteKey == KNullDesC8)
		{
		delete iDeleteKey;
		iDeleteKey = NULL;
		}
	else if (iDeleteKey == NULL)
		{
		iDeleteKey = aDeleteKey.AllocL();
		}
	else
		{
		User::Leave(KErrAlreadyExists);
		}
	}

EXPORT_C const TDesC8& CRightsObject::DeleteKey()
	{
	return (iDeleteKey ? *iDeleteKey : KNullDesC8());
	}
