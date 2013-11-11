// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// concrete file finders for MIME type recognizers
// 
//

#include "APMFNDR.H"

#include <e32uid.h>
#include "APMPAN.H"

#ifdef USING_ECOM_RECOGS
#include <ecom/ecom.h>
#endif

const TInt KRecognizerDllGranularity=2;

#ifdef USING_ECOM_RECOGS
const TUid KUidDataRecogInterFaceUid = {0x101F7D87};
#endif

//
// class TRecognizer
//

CApaScanningDataRecognizer::TRecognizer::TRecognizer(HBufC* aName)
	{
	iName=aName;
	}
	
//
// class CApaRecognizerDll
//

CApaRecognizerDll::~CApaRecognizerDll()
	{
	delete iId.iName;
	iLibrary.Close();
	delete iNext;
	}

CApaRecognizerDll::CApaRecognizerDll(HBufC* aName) 
	: iId(aName)
	{
	}

#ifdef USING_ECOM_RECOGS
// Used for cleanup support of locally declared arrays
void CleanUpEComInfoArray(TAny* aInfoArray)
	{
	RImplInfoPtrArray* infoArray = (static_cast<RImplInfoPtrArray*>(aInfoArray));
	infoArray->ResetAndDestroy();
	infoArray->Close();
	}
#endif

// class CApaBackupOperationObserver
CApaScanningDataRecognizer::CApaBackupOperationObserver::CApaBackupOperationObserver(CApaScanningDataRecognizer& aRecognizer)
	: iRecognizer(aRecognizer)
	{
	TRAP_IGNORE(iSession = CBaBackupSessionWrapper::NewL());
	}

CApaScanningDataRecognizer::CApaBackupOperationObserver::~CApaBackupOperationObserver()
	{
	if( iSession )
		{
		iSession->DeRegisterBackupOperationObserver(*this);
		delete iSession;
		}
	}

void CApaScanningDataRecognizer::CApaBackupOperationObserver::RegisterObserverL()
	{
	if(iSession)
		{
		iSession->RegisterBackupOperationObserverL(*this);
		}
	}

void CApaScanningDataRecognizer::CApaBackupOperationObserver::HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	if (!iRecognizer.iExtra->iRecognizersLoaded)
		{
		// Then the recognizers have been unloaded, so there's nothing to do
		return;
		}

	if ((aBackupOperationAttributes.iFileFlag == MBackupObserver::EReleaseLockReadOnly ||
		aBackupOperationAttributes.iFileFlag == MBackupObserver::EReleaseLockNoAccess)
		&& aBackupOperationAttributes.iOperation == EStart)
		{
		// release recognisers not in rom
		TInt count = iRecognizer.RecognizerCount();
		for (TInt i=count-1; i>=0; i--)
			{
				CApaRecognizerDll* lib=iRecognizer.iRecognizerLib[i];
				TDriveUnit drive(lib->iId.iDrive);
				TChar driveName=drive.Name()[0];
				if(driveName != TChar('Z'))
					{
					iRecognizer.RemoveRecognizer(lib); // ignore any errors
					}
			}
		}

	if (aBackupOperationAttributes.iFileFlag == MBackupObserver::ETakeLock &&
		(aBackupOperationAttributes.iOperation == EEnd ||
		aBackupOperationAttributes.iOperation == EAbort))
		{
		iRecognizer.ScanForRecognizersL();
		}
	}

TInt CApaScanningDataRecognizer::CApaBackupOperationObserver::UpdateCounter() const
	{
	return iUpdateCounter;
	}

void CApaScanningDataRecognizer::CApaBackupOperationObserver::SetUpdateCounter(TInt aValue)
	{
	iUpdateCounter = aValue;
	}


//
// class CApaScanningDataRecognizer
// 

EXPORT_C CApaScanningDataRecognizer* CApaScanningDataRecognizer::NewL(RFs& aFs)
	{
	return NewL(aFs,ETrue);
	}

/**
@internalTechnology
*/
EXPORT_C CApaScanningDataRecognizer* CApaScanningDataRecognizer::NewL(RFs& aFs, TBool aLoadDataRecognizers)
	{
	CApaScanningDataRecognizer* self=new(ELeave) CApaScanningDataRecognizer(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aLoadDataRecognizers);
	CleanupStack::Pop(); // self
	return self;
	}

EXPORT_C CApaScanningDataRecognizer::~CApaScanningDataRecognizer()
	{
	UnloadRecognizers();
	delete iExtra->iObserver;
	delete iExtra;
	}

EXPORT_C void CApaScanningDataRecognizer::ScanForRecognizersL()
	{
	// set all recognizers to not present - pending rediscovery
	TInt count=iRecognizerLib.Count();
	TInt i;
	for (i=0; i<count; i++)
		iRecognizerLib[i]->iPresent=CApaRecognizerDll::ENotPresent;

	ScanForEcomRecognizersL();
	//
	// remove any recognizers that are no longer present
	for (TInt ii=iRecognizerLib.Count()-1;ii>=0;ii--)
		{
		CApaRecognizerDll* lib=iRecognizerLib[ii];
		if (lib->iPresent==CApaRecognizerDll::ENotPresent)
			{
			RemoveRecognizer(lib); // ignore any errors
			}
		}
	iMaxBufferSize = -1; // Recognizers were re-loaded so need to recalculate the maximum buffer size
	}

EXPORT_C TInt CApaScanningDataRecognizer::RecognizerCount()
	{
	return iRecognizerLib.Count();
	}

EXPORT_C CApaScanningDataRecognizer::CRecognizerArray* CApaScanningDataRecognizer::RecognizerListLC()const
	{
	CRecognizerArray* list=new(ELeave) CArrayFixFlat<TRecognizer>(2);
	CleanupStack::PushL(list);
	TInt count=iRecognizerLib.Count();
	for (TInt i=0; i<count; i++)
		list->AppendL(iRecognizerLib[i]->iId);
	return list;
	}

EXPORT_C const CApaScanningDataRecognizer::TRecognizer& CApaScanningDataRecognizer::operator[](TInt aIndex)const
	{
	return iRecognizerLib[aIndex]->iId;	
	}

EXPORT_C TInt CApaScanningDataRecognizer::UpdateCounter()const
	{
	return iExtra->iObserver->UpdateCounter();
	}


EXPORT_C CApaScanningDataRecognizer::CApaScanningDataRecognizer(RFs& aFs)
	:CApaDataRecognizer(aFs),
	iRecognizerLib(KRecognizerDllGranularity)
	{
	__DECLARE_NAME(_S("CApaScanningDataRecognizer"));
	}

EXPORT_C void CApaScanningDataRecognizer::ConstructL()
	{
	ConstructL(ETrue);
	}

void CApaScanningDataRecognizer::ConstructL(TBool aLoadDataRecognizers)
	{
	iExtra = new (ELeave) CApaScanningDataRecognizerExtra();
	iExtra->iObserver = new (ELeave) CApaBackupOperationObserver( *this );
	iExtra->iObserver->RegisterObserverL();
	if(aLoadDataRecognizers)
		{
		LoadRecognizersL();
		}
	}

EXPORT_C void CApaScanningDataRecognizer::SetEcomRecognizersFromListL(const CRecognizerArray& aList)
	{
	//for ecom style
	for (TInt i=0 ; i<aList.Count() ; i++)
		{
		TDriveUnit drive(aList[i].iDrive);
		TRAPD(ret, LoadEcomRecognizerL(drive,aList[i].iUid));
		if (ret==KErrNoMemory)
			{
			User::Leave(ret); 
			}
		}
	}

EXPORT_C void CApaScanningDataRecognizer::SetEcomRecognizerL(const TRecognizer& aRecognizer)
	{
	//for ecom style 
	TDriveUnit drive(aRecognizer.iDrive);
	LoadEcomRecognizerL(drive,aRecognizer.iUid);
	}

TInt CApaScanningDataRecognizer::RemoveRecognizer(CApaRecognizerDll* aDll)
	{
	__ASSERT_DEBUG(iRecognizerLib.Count()>0,User::Invariant());
	// try to remove the recognizer from the list
	TInt ret=RemoveDataRecognizerType(aDll->iRec);
	if (ret!=KErrNone && ret!=KErrNotFound)
		return ret;
	//
	// get a handle to the previous entry in the list
	TInt count=iRecognizerLib.Count();
	TInt i;
	for (i=0; i<count; i++)
		{
		if (iRecognizerLib[i]==aDll)
			break;
		}
	__ASSERT_ALWAYS(i<count,Panic(EPanicLibraryNotInList));
	
	// remove the dll
	delete iRecognizerLib[i];
	iRecognizerLib.Remove(i);
	iRecognizerLib.Compress();
	if(iExtra->iObserver)
		iExtra->iObserver->SetUpdateCounter(iExtra->iObserver->UpdateCounter() +1);
	return KErrNone;
	}

#ifdef USING_ECOM_RECOGS

//load the ecom style data recognizer
void CApaScanningDataRecognizer::LoadEcomRecognizerL(TDriveUnit& aDrive,TUid aUid)
	{
	//Get implementation information of the recognizer with UID aUid
	RImplInfoPtrArray implementationInfoArray;
	TCleanupItem cleanup(CleanUpEComInfoArray, &implementationInfoArray);
	CleanupStack::PushL(cleanup);
	REComSession::ListImplementationsL(KUidDataRecogInterFaceUid,implementationInfoArray);
	const TInt availCount = implementationInfoArray.Count();
	CImplementationInformation* info=NULL;
	TInt ii;
	for (ii=0;ii<availCount;++ii)
		{
		info = implementationInfoArray[ii];
		TUid uid = info->ImplementationUid();
		
		if(uid.iUid == aUid.iUid )
			{
			break;
			}
		}

	//If the implementation not found just return
	if(ii==availCount)
		{
		CleanupStack::PopAndDestroy(&implementationInfoArray);		
		return;
		}
	
	//Get the version of implementation
	TInt version=info->Version();
	
	// check we haven't loaded this one already
	TInt count=iRecognizerLib.Count();
	TInt i;
	CApaRecognizerDll* lib=NULL; // the previous one in the list
	for (i=0; i<count; i++)
		{
		lib=iRecognizerLib[i];
		if (lib->iId.iUid==aUid)
			{// we may have to override one
			if (lib->iPresent!=CApaRecognizerDll::ENotPresent)
				{
				CleanupStack::PopAndDestroy(&implementationInfoArray);				
				return; // already found
				}
			else
				{
				if (lib->iVersion < version)
					{
					//Upgrade a plugin if a plugin with higher version than loaded plugin is installed.  
					lib->iPresent = CApaRecognizerDll::ESuperseded;
					break;
					}
				else
					{
					CleanupStack::PopAndDestroy(&implementationInfoArray);					
					lib->iPresent = CApaRecognizerDll::EIsPresent;
					return;
					}
				}
			}			
		}
	//Remove the old version and let it continue to load new version
	if ((i<count)&&(lib->iPresent==CApaRecognizerDll::ESuperseded))
		{
		RemoveRecognizer(lib);
		}

	// load the library
	HBufC* recogName=info->DisplayName().AllocL();
	CleanupStack::PushL(recogName);
	lib=NULL;
	lib = new(ELeave) CApaRecognizerDll(recogName);
	CleanupStack::Pop(recogName);
	CleanupStack::PushL(lib);
	TRAPD(error,lib->iRec = CApaDataRecognizerType::CreateDataRecognizerL(aUid));
	if(!error && lib->iRec)
		{
		lib->iPresent = CApaRecognizerDll::EIsPresent;
		AddDataRecognizerTypeL(lib->iRec);
		lib->iId.iUid = aUid;
		lib->iId.iDrive = aDrive;
		lib->iVersion=version;
		iExtra->iObserver->SetUpdateCounter(iExtra->iObserver->UpdateCounter() + 1);
		//
		// add lib to the library list
		iRecognizerLib.AppendL(lib);
		CleanupStack::Pop(lib); // lib
		}
	else
		{
		CleanupStack::PopAndDestroy(lib); // lib
		}
	CleanupStack::PopAndDestroy(&implementationInfoArray);	
	}
#else
void CApaScanningDataRecognizer::LoadEcomRecognizerL(TDriveUnit&,TUid)
	{
	}
#endif

void CApaScanningDataRecognizer::ScanForEcomRecognizersL()
	{
#ifdef USING_ECOM_RECOGS
	//scan ecom plugin
	RImplInfoPtrArray implementationArray;
	TCleanupItem cleanup(CleanUpEComInfoArray, &implementationArray);
	CleanupStack::PushL(cleanup);
	REComSession::ListImplementationsL(KUidDataRecogInterFaceUid,implementationArray);
	const TInt availCount = implementationArray.Count();
	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		TUid uid = info->ImplementationUid();
		TDriveUnit drive=info-> Drive();
		TRAP_IGNORE(LoadEcomRecognizerL(drive,uid));
		}
	CleanupStack::PopAndDestroy(&implementationArray);
#endif
	}

/**
@internalTechnology
*/
EXPORT_C void CApaScanningDataRecognizer::LoadRecognizersL()
	{
	iExtra->iRecognizersLoaded = ETrue;
	ScanForRecognizersL();
	UpdateDataTypesL();
	}

/**
@internalTechnology
*/
EXPORT_C void CApaScanningDataRecognizer::UnloadRecognizers()
	{	
	iExtra->iRecognizersLoaded = EFalse;
	DestroyRecognizerList();
	iRecognizerLib.ResetAndDestroy();
	}

