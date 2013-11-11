// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : filesocketaddrmanager.cpp
// Part of     : ipc server
// This manages the localsocket address 
// 
//
#include "filesocketaddrmanager.h"
#include <e32des8.h>

//----------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::NewL()
// Description   : 	Factory method to create a new instance of CLocSocketAddrManager
//----------------------------------------------------------------------------------	
MRequestHandler* CLocSocketAddrManager::NewL()
	{
	return new(ELeave) CLocSocketAddrManager;
	}

//----------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::~CLocSocketAddrManager()
// Description   : 	Cleans up the hash table
//----------------------------------------------------------------------------------	

CLocSocketAddrManager::~CLocSocketAddrManager()
	{
	const CLocalSocketPortAddrMap *locSocketAddrPortMap;
	RPtrHashMap<TDesC8,CLocalSocketPortAddrMap>::TIter hashIter(iPortAddrmap);

	locSocketAddrPortMap = hashIter.NextValue();
	while(locSocketAddrPortMap)
		{
        delete locSocketAddrPortMap;
		locSocketAddrPortMap = hashIter.NextValue();
		}
	iPortAddrmap.Close();
	}

//----------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::ServiceL()
// Description   : 	Services the requests
//----------------------------------------------------------------------------------	

void CLocSocketAddrManager::ServiceL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())	
		{
		case EFileSocketGetPathByPort:
			GetPathNameByPort(aMessage);
			break;
		case EFileSocketGetPortByPath:
			GetPortByPathName(aMessage);
			break;
		case EFileSocketAddPath:
			AddPathNameL(aMessage);
			break;
		case EFileSocketRemovePath:
			RemovePathName(aMessage);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}

	}

//----------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::RemovePathBySession()
// Description   : 	Removes a path by session pointer  
//----------------------------------------------------------------------------------	

void CLocSocketAddrManager::RemovePathBySession(CSession2 *aSession)
	{
	const CLocalSocketPortAddrMap *locSocketAddrPortMap;
	RPtrHashMap<TDesC8,CLocalSocketPortAddrMap>::TIter hashIter(iPortAddrmap);
	
	locSocketAddrPortMap = hashIter.NextValue();
	while(locSocketAddrPortMap)
		{
		if(locSocketAddrPortMap->GetSession() == aSession)
			{
			hashIter.RemoveCurrent();
			delete locSocketAddrPortMap;
			}

		locSocketAddrPortMap = hashIter.NextValue();
		}
	return ;
	}
//---------------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::FindBYPortNum()
// Description   : 	Iterate through the HashMap to get the entry associated with aPortNum
//---------------------------------------------------------------------------------------	
TInt CLocSocketAddrManager::FindBYPortNum(TUint aPortNum,const CLocalSocketPortAddrMap*&aLocSocketAddrPortMap)
	{
	RPtrHashMap<TDesC8,CLocalSocketPortAddrMap>::TIter hashIter(iPortAddrmap);
	
	aLocSocketAddrPortMap = hashIter.NextValue();
	while(aLocSocketAddrPortMap)
		{
		if(aLocSocketAddrPortMap->GetPortNum() == aPortNum)
			return KErrNone;
		aLocSocketAddrPortMap = hashIter.NextValue();
		}
	return KErrNotFound;
	
	}


//----------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::AddPathNameL()
// Description   : 	Add the path to HashMap   
//----------------------------------------------------------------------------------	
void CLocSocketAddrManager::AddPathNameL(const RMessage2& aMessage)
	{
	//Check if already there
	TUint portNum = aMessage.Int0();
	TInt err;

	if(aMessage.GetDesMaxLength(1) <= 0)
		{
		aMessage.Complete(KErrArgument);
		return;
		}
	
	TBuf8<KMaxFileName>  path;

	if((err = aMessage.Read(1,path)) != KErrNone)
		{
		aMessage.Complete(err);
		return;		
		}
	
	if(iPortAddrmap.Find(path) != NULL)
		{
		aMessage.Complete(KErrAlreadyExists);
		return;
		}
	
	CLocalSocketPortAddrMap * locSocketAddrPortMap;
	
	locSocketAddrPortMap = CLocalSocketPortAddrMap::NewL(aMessage);
	CleanupStack::PushL(locSocketAddrPortMap);
	iPortAddrmap.InsertL(&locSocketAddrPortMap->GetPathName(),locSocketAddrPortMap); 
	CleanupStack::Pop ();
	aMessage.Complete(KErrNone);
	}

//----------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::GetPathNameL()
// Description   : 	Get the path from the HashMap 
//----------------------------------------------------------------------------------	
void CLocSocketAddrManager::GetPortByPathName(const RMessage2& aMessage)
	{
	TInt err;

	if(aMessage.GetDesLength(0) <= 0)
		{
		aMessage.Complete(KErrArgument);
		return;
		}
	TBuf8<KMaxFileName> path;

	if((err = aMessage.Read(0,path)) != KErrNone)
		{
		aMessage.Complete(err);
		return;		
		}
	
	const CLocalSocketPortAddrMap * locSocketAddrPortMap;

	if((locSocketAddrPortMap = iPortAddrmap.Find(path)) == NULL)
		{
		aMessage.Complete(KErrNotFound);
		return;
		}

	TPckgBuf<TUint> portNum(locSocketAddrPortMap->GetPortNum());
	if((err = aMessage.Write(1,portNum))!= KErrNone)
		{
		aMessage.Complete(err);
		return;
		}
	aMessage.Complete(KErrNone);
	}

//----------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::GetPathNameL()
// Description   : 	Get the path from the HashMap 
//----------------------------------------------------------------------------------	
void CLocSocketAddrManager::GetPathNameByPort(const RMessage2& aMessage)
	{

	TUint portNum = aMessage.Int0();
	const CLocalSocketPortAddrMap * locSocketAddrPortMap;

	TInt addrLen;
	if((addrLen = aMessage.GetDesMaxLength(1)) <= 0)
		{
		aMessage.Complete(KErrArgument);
		return;
		}
	
	TInt ret;
	
	if((ret = FindBYPortNum(portNum,locSocketAddrPortMap)) != KErrNone)
		{
		aMessage.Complete(ret);
		return;
		}
	
	const TDesC8 &path = locSocketAddrPortMap->GetPathName();
	
	if((ret = aMessage.Write(1,path.Left(addrLen)))!= KErrNone)
		{
		aMessage.Complete(ret);
		return;
		}
	aMessage.Complete(KErrNone);

	}
//----------------------------------------------------------------------------------
// Function Name : 	CLocSocketAddrManager::RemovePathName()
// Description   : 	Remove the path from the HashMap 
//----------------------------------------------------------------------------------	
void CLocSocketAddrManager::RemovePathName(const RMessage2& aMessage)
	{
	
	CLocalSocketPortAddrMap * locSocketAddrPortMap;
	
	if(aMessage.GetDesLength(0) <= 0)
		{
		aMessage.Complete(KErrArgument);
		return;
		}
	
	TBuf8<KMaxFileName> path;

	if(aMessage.Read(0,path)!= KErrNone)
		{
		aMessage.Complete(KErrArgument);
		return;		
		}
	
	if((locSocketAddrPortMap = iPortAddrmap.Find(path)) == NULL)
		{
		aMessage.Complete(KErrNotFound);
		return;
		}
	
	TProcessId pid = IpcUtils::GetPid(aMessage);
	
	if(pid != locSocketAddrPortMap->GetPid())
		{
		aMessage.Complete(KErrAccessDenied);
		return;
		}
	TInt err;
	if((err = iPortAddrmap.Remove(&path)) != KErrNone)
		{
		aMessage.Complete(err);
		return;
		}
	delete locSocketAddrPortMap;
	aMessage.Complete(KErrNone);
	}

//----------------------------------------------------------------------------------
// Function Name : 	CLocalSocketPortAddrMap::NewL()
// Description   : 	Factory method to create new instance of 
//                  CLocalSocketPortAddrMap
//----------------------------------------------------------------------------------	
CLocalSocketPortAddrMap* CLocalSocketPortAddrMap::NewL(const RMessage2& aMessage)
	{

	CLocalSocketPortAddrMap* self = new (ELeave) CLocalSocketPortAddrMap;
	CleanupStack::PushL (self);

	self->ConstructL(aMessage);

	CleanupStack::Pop();
	return self;

	}

//----------------------------------------------------------------------------------
// Function Name : 	CLocalSocketPortAddrMap::ConstructL()
// Description   :  Initializes CLocalSocketPortAddrMap
//----------------------------------------------------------------------------------	
void CLocalSocketPortAddrMap::ConstructL(const RMessage2& aMessage)
	{
	TUint portNum = aMessage.Int0();
	TInt pathLen = aMessage.GetDesLengthL(1);
	iPath = HBufC8::NewL(pathLen);
	TPtr8 tmpPath = iPath->Des();
	aMessage.ReadL(1,tmpPath);
	iPort = portNum;
	iPid = IpcUtils::GetPid(aMessage);
	iSessionPtr = aMessage.Session();
	}

