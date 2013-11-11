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
* SisRegistry - server implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/
 
#include <s32file.h>
#include "RTAserver.h"
#include "RTAsession.h"
#include "clientserver.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;

_LIT(KRightsDatabaseFileName, "rights.db");

const TInt CRtaServer::iRanges[iRangeCount] = 
	{
	0, 								// and higher, accessible by DRM protected clients only
	ESeparatorUnprotected,			// and higher, accessible by all clients
	};

const TUint8 CRtaServer::iElementsIndex[iRangeCount] = 
	{
	1,  // clients must have DRM capability 
	0   // clients do not need DRM capability
	};

const CPolicyServer::TPolicyElement CRtaServer::iPolicyElements[] = 
	{
	{_INIT_SECURITY_POLICY_PASS},
	{_INIT_SECURITY_POLICY_C1(ECapabilityDRM), CPolicyServer::EFailClient},
	};

const CPolicyServer::TPolicy CRtaServer::iPolicy =
	{
	CPolicyServer::EAlwaysPass, // any client can connect	
	iRangeCount,                                   
	iRanges,
	iElementsIndex,
	iPolicyElements,
	};
 
inline CRtaServer::CRtaServer()
	:CPolicyServer(CActive::EPriorityStandard, iPolicy, ESharableSessions)
	{}
	
CRtaServer* CRtaServer::NewLC()
	{
	CRtaServer* self = new(ELeave) CRtaServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CRtaServer::ConstructL()
//
// 2nd phase construction - ensure the timer and server objects are running
//
	{
	StartL(KRtaServerName);
	// create the shutdown 
	iShutdown.ConstructL();
	
	// create the file server session
	User::LeaveIfError(iFs.Connect());

	// get the private path
	TPath privatePath;
	iFs.PrivatePath(privatePath);

	// make sure the private server directory exists 
	iFs.MkDirAll(privatePath);

	iRightsDatabaseFileName.Copy(privatePath);
	iRightsDatabaseFileName.Append(KRightsDatabaseFileName());
	
	LoadRightsDatabaseL();
	
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown.Start();
	}
	
CRtaServer::~CRtaServer()
	{
	iRights.ResetAndDestroy();
	
	// close the file server session
	iFs.Close();
	}

CSession2* CRtaServer::NewSessionL(const TVersion&, const RMessage2&) const
//
// Create a new client session. This should really check the version number.
//
	{
	return new(ELeave) CRtaSession();
	}

void CRtaServer::AddSession()
//
// A new session is being created
// Cancel the shutdown timer if it was running
//
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

void CRtaServer::DropSession()
//
// A session is being destroyed
// Start the shutdown timer if it is the last session.
//
	{
	if (--iSessionCount == 0)
		{
		iShutdown.Start();
		}
	}


void CRtaServer::AddRightsL(CRightsObject* aRightsObject)
	{
	TBool found = EFalse;
	for(TInt i = 0; i < iRights.Count(); i++)
		{
		if(iRights[i]->Cid() == aRightsObject->Cid())
			{
			found = ETrue;
			// add the new rights to the existing rights object
			iRights[i]->MergeL(*aRightsObject);
			delete aRightsObject;
			aRightsObject = NULL;
			break;
			}
		}
		
	if(!found)
		{
		iRights.AppendL(aRightsObject);
		}
	// Update rights database
	SaveRightsDatabaseL();
	}
	
void CRtaServer::EvaluateIntentL(const TDesC8& aCid, TIntent aIntent)
	{
	for(TInt i = 0; i < iRights.Count(); i++)
		{
		if(iRights[i]->Cid() == aCid)
			{
			User::LeaveIfError(iRights[i]->EvaluateIntent(aIntent));
			return;
			}
		}
	User::Leave(KErrCANoRights);
	}
	
void CRtaServer::ExecuteIntentL(const TDesC8& aCid, TIntent aIntent)
	{
	for(TInt i = 0; i < iRights.Count(); i++)
		{
		if(iRights[i]->Cid() == aCid)
			{
			iRights[i]->ExecuteIntentL(aIntent);
			SaveRightsDatabaseL();
			return;
			}
		}
	User::Leave(KErrCANoRights);
	}
	
void CRtaServer::ClearAllRightsL()
	{
	iRights.ResetAndDestroy();
	SaveRightsDatabaseL();
	}
	
void CRtaServer::DumpRightsDatabaseL(const TDesC& aFileName)
	{
	RFile file;
	TInt err = file.Replace(iFs, aFileName, EFileWrite | EFileStreamText | EFileShareAny);
	if(err == KErrNone)
		{
		CleanupClosePushL(file);
		for(TInt i = 0; i < iRights.Count(); i++)
			{
			iRights[i]->WriteDescription(file);
			}
		CleanupStack::PopAndDestroy(&file);
		}
	}

void CRtaServer::DeleteRightsByKeyL(const TDesC8& aDeleteKey)
	{
	TInt origCount = iRights.Count();
	for(TInt i = origCount - 1; i >= 0; i--)
		{
		CRightsObject* ro = iRights[i];
		if ( 0 == aDeleteKey.CompareF(ro->DeleteKey()) )
			{
			iRights.Remove(i);
			delete ro;
			}
		}

	if (iRights.Count() < origCount)
		{
		SaveRightsDatabaseL();
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CRtaServer::EraseDeleteKeyL(const TDesC8& aDeleteKey)
	{
	TInt changed = 0;
	for (TInt i = 0; i < iRights.Count(); i++)
		{
		CRightsObject* ro = iRights[i];
		if ( 0 == aDeleteKey.CompareF(ro->DeleteKey()) )
			{
			ro->SetDeleteKeyL(KNullDesC8);
			changed++;
			}
		}

	if (changed)
		{
		SaveRightsDatabaseL();
		}
	}

void CRtaServer::LoadRightsDatabaseL()
	{
	// This isn't a real database, just an array of rights objects
	// dumped to a file	
	RFile file;
	TInt err = file.Open(iFs, iRightsDatabaseFileName, EFileRead | EFileStream | EFileShareReadersOnly);
	if(err == KErrNone)
		{
		RFileReadStream stream(file);
		CleanupClosePushL(stream);
	
	// RTA supports multiple versions of rights.db. Basic format of 
	// rights.db is number of rights objects in the file followed
	// by the rights objects dump.
	// 
	// The newer versions of rights.db has a 4 byte header:
	// 'r' << 24 | 't' << 16 | 'a' << 8 | version (0x727461??).
	// If the first 3 bytes of a rights.db do not match 'r', 't', 'a',
	// it is considered version zero. Version zero does not have the
	// delete key field in the file.
	// If a version zero file has > 1920229631 (0x727461??) entries,
	// then the first
	// 3 bytes of the file might inadvertently match "rta". This is 
	// impossible because you cannot build a ROM that big (20 G). 
	// Nevertheless RTA will enforce the rule
	// that it will neither internalize nor externalize rights if 
	// number of entries > 65535. 

		TUint32 firstInt = stream.ReadUint32L();
		TUint32 signature = firstInt & KRtaSigMask;
		TUint8 version = static_cast<TInt8>(~KRtaSigMask & firstInt);

		if (signature == KRtaSignature)
			{
			TUint32 count = stream.ReadUint32L();
			InternalizeRightsL(stream, version, count);
			}
		else
			{
			// No 'r', 't', 'a' signature. Must be version zero. 
			// In that case first int in the file is the array count.
			version = 0;
			InternalizeRightsL(stream, version, firstInt);
			}

		CleanupStack::PopAndDestroy(&stream);
		}
	}

void CRtaServer::InternalizeRightsL(RReadStream& aStream, TUint8 aVersion, TUint32 aRightsCount)
	{
	if (aVersion > KRightsDbVersion)
		{
		// do not know how to internalise a future version
		User::Leave(KErrCorrupt);
		}

	if (aRightsCount > KMaxRightsObjectsInDbFile)
		{
		User::Leave(KErrTooBig);
		}

	// See comments in LoadRightsDatabaseL about maintaining
	// compatibility with original format of rights.db.
	TBool haveDeleteKey = aVersion > 0;

	for (TInt i = 0; i < aRightsCount; i++)
		{
		CRightsObject* element = CRightsObject::NewL(aStream, haveDeleteKey);
		CleanupStack::PushL(element);
		iRights.AppendL(element);

		// Finished with cleanup stack, element now owned by the array so don't delete
		CleanupStack::Pop(element);
		}
	}

void CRtaServer::SaveRightsDatabaseL()
	{
	// This isn't a real database, just an array of rights objects
	// dumped to a file	
	// Also read comments about support of multiple versions of 
	// rights.db in LoadRightsDatabaseL().

	if (iRights.Count() > KMaxRightsObjectsInDbFile)
		{
		iFs.Delete(iRightsDatabaseFileName);
		RDebug::Print(_L("%d rights objects exceeds max. allowed. Rights not saved."), iRights.Count());
		return;
		}

	RFile file;
	TInt err = file.Replace(iFs, iRightsDatabaseFileName, EFileWrite | EFileStream | EFileShareAny);
	if(err == KErrNone)
		{
		RFileWriteStream stream(file);
		CleanupClosePushL(stream);
	
		stream.WriteUint32L(KRtaSignature | KRightsDbVersion);
		stream.WriteUint32L(iRights.Count());
		for (TInt i = 0; i < iRights.Count(); i++)
			{
			iRights[i]->ExternalizeL(stream);
			}

		stream.CommitL();
		CleanupStack::PopAndDestroy(&stream);
		}
	}

TInt CRtaServer::CheckPermittedFileName(const TDesC& aFileName)
	{
	_LIT(KContentExtension,".content");
	TInt err = KErrNone;
	if(aFileName.Length() >= KRightsDatabaseFileName().Length())
		{
		TPtrC rightsDb = aFileName.Right(KRightsDatabaseFileName().Length());
		if(rightsDb.CompareF(KRightsDatabaseFileName()) == 0)
			{
			err = KErrPermissionDenied;	
			}
		}
	if(err == KErrNone && aFileName.Length() >= KContentExtension().Length())
		{
		TPtrC ext = aFileName.Right(KContentExtension().Length());
		if(ext.CompareF(KContentExtension()) != 0)
			{
			err = KErrNotSupported;	
			}
		}	
	return err;
	}
	
/**
This method is intended for unit testing. RTA server will
reload rights from the given file instead of the standard
rights.db in RTA's private data cage.
*/
void CRtaServer::ReloadRightsDatabaseL(const TDesC& aFileName)
	{
	ClearAllRightsL();

	RFile file;
	TInt err = file.Open(iFs, aFileName, EFileRead | EFileStream | EFileShareReadersOnly);

	if(err == KErrNone)
		{
		RFileReadStream stream(file);
		CleanupClosePushL(stream);
	
	// See comments about support of multiple versions of 
	// rights.db in LoadRightsDatabaseL().

		TUint32 firstInt = stream.ReadUint32L();
		TUint32 signature = firstInt & KRtaSigMask;
		TUint8 version = static_cast<TInt8>(~KRtaSigMask & firstInt);

		if (signature == KRtaSignature)
			{
			TUint32 count = stream.ReadUint32L();
			InternalizeRightsL(stream, version, count);
			}
		else
			{
			version = 0;
			InternalizeRightsL(stream, version, firstInt);
			}
	
		CleanupStack::PopAndDestroy(&stream);

		SaveRightsDatabaseL();
		}
	}
