// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sysconfigimpl.h"
#include <hal.h>
#include <hal_data.h>

using namespace BSUL;

namespace Ulogger
{

//////////////////////////////////////////////////////////////////////////////
CConfigSettingsImpl* CConfigSettingsImpl::NewL()
	{
	CConfigSettingsImpl* self=new (ELeave) CConfigSettingsImpl();
	return self;
	}

TBool CConfigSettingsImpl::Next(TPtrC8& aSetting,TPtrC8& aSettingValue)
	{
	if (!iIniSecIter)
		{
		return EFalse;
		}
	return iIniSecIter->Next(aSetting,aSettingValue);	
	}
	
void CConfigSettingsImpl::Reset()
	{	
	if (iIniSecIter)
		{
		iIniSecIter->Reset();
		}
	}

CConfigSettingsImpl::~CConfigSettingsImpl()
	{
	if (iIniSecIter)
		{
		delete iIniSecIter;
		iIniSecIter=NULL;			
		}
	}

CConfigSettingsImpl::CConfigSettingsImpl():iIniSecIter(NULL)
	{
	}	

///////////////////////////////////////////////////////////////////////////////


CConfigImpl* CConfigImpl::NewL(RHeap* aHeap,TFileName& aFileName)
	{
	CConfigImpl* self=new (ELeave)CConfigImpl(aHeap,aFileName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CConfigImpl::ConstructL()
	{
	// Create/open a handle onto global mutex used to protect
	// access to the config file.
	TInt ret = KErrAlreadyExists;
	for ( ; ret==KErrAlreadyExists; )
		{
		ret = iWriteMutex.OpenGlobal(KSysConfigMutex, EOwnerProcess);
		if (ret == KErrNotFound)
			ret = iWriteMutex.CreateGlobal(KSysConfigMutex, EOwnerProcess);
		}
	User::LeaveIfError(ret);

	//establishing connection to file server
	User::LeaveIfError(iFs.Connect());
	// allow this connection to be shared with other threads
	// - this facilitates sharing of config objects between threads
	User::LeaveIfError(iFs.ShareProtected());

	// load config
//	iIniFileWatcher = NULL;
	LoadConfigFileL();
	}

CConfigImpl::~CConfigImpl()
	{
	if (iIniFileDocument)
		{
		delete iIniFileDocument;
		iIniFileDocument=NULL;
		}	
	//closing the mutex
	if (iWriteMutex.Handle() && iWriteMutex.IsHeld())
		{
		iWriteMutex.Signal();
		}
	iWriteMutex.Close();
	//closing the file handle
	iFs.Close();	
	}




/**
Load the Ulogger config file contents.
The existing iIniFileDocument and caches are cleared.
This method assumes that iFs is connected. 
*/
void CConfigImpl::LoadConfigFileL()
	{
	//switch to private heap
	RHeap* threadHeap = NULL;
	if (iHeap)
		threadHeap = User::SwitchHeap(iHeap);		
	
	// delete existing document
	if (iIniFileDocument)
		{
		delete iIniFileDocument;
		iIniFileDocument=NULL;
	
		}
	
	TEntry dummy;
	TInt ret=iFs.Entry(iFileName,dummy);

	// grab the mutex
	iWriteMutex.Wait();
		
	//Creating an instance of IniDocument from default
	iIniFileDocument=CIniDocument8::NewL(iFs,iFileName);
	
	// release the mutex
	iWriteMutex.Signal();
	//switch back to user heap
	if (threadHeap)
		User::SwitchHeap(threadHeap);
	}


TInt CConfigImpl::PersistIniFile()
	{
	// grab the mutex
	iWriteMutex.Wait();
	
	TInt ret=iIniFileDocument->Externalise(iFileName);	
	
	// release the mutex
	iWriteMutex.Signal();
	return ret;
	}	


TInt CConfigImpl::GetKeyValue(const TDesC8& aSectionName,const TDesC8& aKeyName ,TPtrC8& aValue)
	{
	return iIniFileDocument->GetKeyValue(aSectionName,aKeyName,aValue);
	}
	
TInt CConfigImpl::RemoveKey(const TDesC8& aSectionName,const TDesC8& aKeyName)
	{
	return iIniFileDocument->RemoveKey(aSectionName,aKeyName);
	}

TInt CConfigImpl::SetKeyValue(const TDesC8& aSectionName,const TDesC8& aKey, const TDesC8& aValue)
	{
	return iIniFileDocument->SetKey(aSectionName,aKey,aValue);
	}

/*TInt CConfigImpl::AddSection(const TDesC8& aSectionName)
	{
	return iIniFileDocument->AddSection(aSectionName);
	}*/

TInt CConfigImpl::RemoveSection(const TDesC8& aSectionName)
	{
	return iIniFileDocument->RemoveSection(aSectionName);
	}

TInt CConfigImpl::GetSection(const TDesC8& aSectionName,CConfigSettingsIter& aSection)
	{
	//clear any previous internal iIniSecIterator
	if (aSection.iImpl->iIniSecIter)
		{
		delete (aSection.iImpl->iIniSecIter);
		aSection.iImpl->iIniSecIter=NULL;
		}
	//initializing its internal iIniSecIterator
	TRAPD(ret,aSection.iImpl->iIniSecIter=CIniSecIter8::NewL(aSectionName,iIniFileDocument));
	return ret;	
	}


	
//util to check any key within that section holds that value
//if not found simply return the number of keys within that section
TInt CConfigImpl::CheckValueExist(const TDesC8& aSectionName,const TDesC8& aValue,TInt& aKeyCount)
	{
	CIniSecIter8* iter=NULL;
	TRAPD(err,iter=CIniSecIter8::NewL(aSectionName,iIniFileDocument));
	//if section not found it might be the first time we are creating
	//the document so it is fine to return KErrNone
	if (err==KErrNotFound)
		{
		return KErrNone;
		}
	//return the error code if else than KErrNotFound e.g. KErrNoMemory;
	if (err!=KErrNone)
		{
		return err;
		}
	TPtrC8 key;
	TPtrC8 value;
	TInt keyCount=0;
	while (iter->Next(key,value))
		{
		if (value.Compare(aValue)==0)
			{
			delete iter;
			keyCount=0;
			return KErrAlreadyExists;
			}
		keyCount++;	 	
		}
	aKeyCount=keyCount;
	delete iter;	
	return KErrNone;	
	}

	
TInt CConfigImpl::GenerateInternalKey(const TDesC8& aSection,TBuf8<15>& aKeyName)
	{
	TPtrC8 lastKey;
	TInt ret=GetKeyCount(aSection,lastKey);
	if (ret<0)
		{
		return ret;		
		}
	//either "mediaX" or "X"
	//TInt key=ret;

	if(aSection.Compare(KPrimaryFilterSection)== 0 ) 
	{
		TInt lastKeyValue=0;
		if(lastKey.Length())
			{
			TLex8 lex(lastKey);
			TInt err = lex.Val(lastKeyValue);
			if(err != KErrNone)
				return err;
			}
		aKeyName.Format(_L8("%03d"),++lastKeyValue);
	}
	else if(aSection.Compare(KSecondaryFilterSection) == 0)
	{
		TInt lastKeyValue=0;
		if(lastKey.Length())
			{
			TLex8 lex(lastKey);
			TInt err = lex.Val(lastKeyValue);
			if(err != KErrNone)
				return err;
			}
		aKeyName.Format(_L8("%04d"),++lastKeyValue);
	}
	else
	{
		TInt lastKeyValue=0;
		if(lastKey.Length())
			{
			TLex8 lex(lastKey);
			TInt err = lex.Val(lastKeyValue);
			if(err != KErrNone)
				return err;
			}
		aKeyName.Format(_L8("%d"),++lastKeyValue);
	}	
	return KErrNone;	
}

/*TInt CConfigImpl::GenerateInternalSessionName(const TDesC8& aSection,TBuf8<15>& aKeyName)
	{
	TPtrC8 lastKey;
	TInt ret=GetKeyCount(aSection,lastKey);
	if (ret<0)
		{
		return ret;		
		}
	//either "mediaX" or "X"
	TInt index=(aSection.Compare(KActiveSection)==0?5:0);
	TInt key=0;
	if (lastKey.Length()!=0)
		{
		TLex8 lex;		
		lex.Assign(lastKey.Mid(index));
		lex.Val(key);
		}
	aKeyName.Format((aSection.Compare(KActiveSection)==0?_L8("Session%d"):_L8("%d")),++key);	
	return KErrNone;	
	}*/


TInt CConfigImpl::GetKeyCount(const TDesC8& aSectionName,TPtrC8& aLastKey)
	{
	CIniSecIter8* iter=NULL;
	TRAPD(err,iter=CIniSecIter8::NewL(aSectionName,iIniFileDocument));
	//if section not found it indicates the keycount is 0;
	if (err==KErrNotFound)
		{
		return 0;			
		}
	//return any error code e.g KErrNoMemory here;
	if (err!=KErrNone)
		{
		return err;		
		}
	TPtrC8 key;
	TInt keyCount=0x0000;
	while (iter->Next(aLastKey,key))
		{	
		keyCount++;
		}
	delete iter;
	return keyCount;
	}
	
TInt CConfigImpl::GetPointerToKeyName(const TDesC8& aSectionName,const TDesC8& aKeyName,TPtrC8& aKeyPointer)
	{
	CIniSecIter8* iter=NULL;
	TRAPD(err,iter=CIniSecIter8::NewL(aSectionName,iIniFileDocument));
	if (err!=KErrNone)
		{
		return err;		
		}
	TPtrC8 value;
	while (iter->Next(aKeyPointer,value))
		{
		if (aKeyName.Compare(aKeyPointer)==0)
			{
			delete iter;
			return KErrNone;
			}
		}
	delete iter;	
	return KErrNotFound;
	}	
}

