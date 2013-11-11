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


#include "intent.h"
#include "rightscriteriacount.h"
#include "rightscriteriaexpiry.h"
#include "s32mem.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;

MCriteria::~MCriteria()
	{
	}

CIntent* CIntent::NewL(TIntent aIntent)
	{
	CIntent* self = new (ELeave) CIntent(aIntent);
	return self;
	}

CIntent* CIntent::NewL(RReadStream& aStream)
	{
	CIntent* self = new (ELeave) CIntent;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}


CIntent::CIntent()
	{
	}

CIntent::CIntent(TIntent aIntent) : iIntent(aIntent)
	{
	}

CIntent::~CIntent()
	{
	iCriteria.ResetAndDestroy();
	iCriteria.Close();
	}

TInt CIntent::Evaluate()
	{
	// See if any rights allow this content to be used
	TInt err = KErrCANoPermission;
	for(TInt i = 0; i < iCriteria.Count(); i++)
		{
		err = iCriteria[i]->Evaluate();
		if(err == KErrNone)
			{
			break;
			}
		}
	return err;	
	}
	
void CIntent::ExecuteL()
	{
	// See if any rights allow this content to be used
	TInt err = KErrCANoPermission;
	for(TInt i = 0; i < iCriteria.Count(); i++)
		{
		if(iCriteria[i]->IsStateless())
			{
			TRAP(err, iCriteria[i]->ExecuteL());
			if(err == KErrNone)
				{
				break;
				}
			}
		}
		
	// If none of the stateless criteria were matched try the remaining criteria
	if(err != KErrNone)
		{
		for(TInt i = 0; i < iCriteria.Count(); i++)
			{
			if(!iCriteria[i]->IsStateless())
				{
				TRAP(err, iCriteria[i]->ExecuteL());
				if(err == KErrNone)
					{
					break;
					}
				}
			}
		}
	
	User::LeaveIfError(err);
	}

TIntent CIntent::Intent()
	{
	return iIntent;
	}
	
void CIntent::AddCriteriaL(MCriteria* aCriteria)
	{
	TBool merged = EFalse;
	for(TInt i = 0; i < iCriteria.Count(); i++)
		{
		// If this criteria already exists, merge the two together
		if(iCriteria[i]->Type() == aCriteria->Type())
			{
			if(iCriteria[i]->Merge(*aCriteria))
				{
				delete aCriteria;
				aCriteria = NULL;
				merged = ETrue;
				break;
				}
			}
		}
		
	if(!merged)
		{
		// didn't merge so just add this criteria to the array
		iCriteria.AppendL(aCriteria);	
		}
	}


const RPointerArray <MCriteria>& CIntent::CriteriaArray() const
	{
	return iCriteria;	
	}


void CIntent::MergeL(CIntent* aIntent)
	{	
	for(TInt i = 0; i < aIntent->CriteriaArray().Count(); i++)
		{
		MCriteria* criteria = aIntent->CriteriaArray()[i]->CopyL();
		CleanupStack::PushL(criteria);
		AddCriteriaL(criteria);
		CleanupStack::Pop(criteria);
		}
	}
	
CIntent* CIntent::CopyL() const
	{
	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write *this to the stream
	ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	TPtr8 source = buf->Ptr(0);
	RDesReadStream readStream(source);
	CleanupClosePushL(readStream);
	
	// Create copy 
	CIntent* copy = CIntent::NewL(readStream);
	
	CleanupStack::PopAndDestroy(2, buf); //readStream, buf
	
	return copy;
	}

void CIntent::InternalizeL(RReadStream& aStream)
	{
	iIntent = static_cast<TIntent>(aStream.ReadInt32L());
	
	// number of criteria
	TInt count = aStream.ReadInt32L();

	for(TInt i  = 0; i < count; i++)
		{
		MCriteria* criteria = NULL;

		// Write criteria type
		ECriteriaType type = static_cast<ECriteriaType>(aStream.ReadInt32L());
		
		// read criteria object from stream
		if(type == ECriteriaCount)
			{
			criteria = CRightsCriteriaCount::NewL(aStream);
			}
		else if(type == ECriteriaExpiry)
			{
			criteria = CRightsCriteriaExpiry::NewL(aStream);
			}

		// Add criteria object to array of criteria
		CleanupStack::PushL(criteria);
		AddCriteriaL(criteria);
		CleanupStack::Pop(criteria);
		}
	}
	
void CIntent::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(static_cast<TInt>(iIntent));

	// number of criteria
	aStream.WriteInt32L(iCriteria.Count());

	for(TInt i  = 0; i < iCriteria.Count(); i++)
		{
		// Write criteria type
		aStream.WriteInt32L(static_cast<TInt>(iCriteria[i]->Type()));
		
		// write criteria object
		iCriteria[i]->ExternalizeL(aStream);
		}
	}


void CIntent::WriteDescription(RFile& aFile)
	{
	for(TInt i = 0; i < iCriteria.Count(); i++)
		{
		aFile.Write(_L8("Intent "));
		if(iIntent == EPeek)
			{
			aFile.Write(_L8("Peek     - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EPlay)
			{
			aFile.Write(_L8("Play     - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EView)
			{
			aFile.Write(_L8("Display  - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EExecute)
			{
			aFile.Write(_L8("Execute  - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EPrint)
			{
			aFile.Write(_L8("Print    - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EPause)
			{
			aFile.Write(_L8("Pause    - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EContinue)
			{
			aFile.Write(_L8("Continue - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EStop)
			{
			aFile.Write(_L8("Stop     - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EUnknown)
			{
			aFile.Write(_L8("Unknown  - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		else if(iIntent == EInstall)
			{
			aFile.Write(_L8("Install  - "));
			iCriteria[i]->WriteDescription(aFile);
			}
		}
	}
