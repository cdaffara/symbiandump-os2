/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*  Name        : filesocketaddrmanager.h
*  Part of     : ipc server
* This manages the localsocket address
* 
*
*/

#ifndef FILESOCKETADDRMANAGER_H_
#define FILESOCKETADDRMANAGER_H_

#include "ipcutils.h"
#include <e32hashtab.h>

class CLocalSocketPortAddrMap;
class CLocSocketAddrManager: public CBase, MRequestHandler
	{

public:
	void ServiceL(const RMessage2& aMessage);
	static MRequestHandler* NewL();
	void RemovePathBySession(CSession2 *aSession);
	~CLocSocketAddrManager();
private:
	void GetPathNameByPort(const RMessage2& aMessage);
	void GetPortByPathName(const RMessage2& aMessage);
	void AddPathNameL(const RMessage2& aMessage);
	void RemovePathName(const RMessage2& aMessage);  
    TInt FindBYPortNum(TUint aPortNum,const CLocalSocketPortAddrMap*&aLocSocketAddrPortMap);
private:	

	RPtrHashMap<TDesC8,CLocalSocketPortAddrMap> iPortAddrmap;

	};

class CLocalSocketPortAddrMap:public CBase
	{
public:
	
	~CLocalSocketPortAddrMap()
		{
		iPort = 0;
		delete iPath;
		}
	static CLocalSocketPortAddrMap* NewL(const RMessage2& aMessage);
	inline const TDesC8& GetPathName() const
		{
		return *iPath;
		}
	inline TUint GetPortNum() const
		 {
		 return iPort;
		 }
	inline TProcessId GetPid() 
		 {
		 return iPid;
		 }
	inline const CSession2* GetSession() const
		{
        return iSessionPtr;
		}
private:
	
	CLocalSocketPortAddrMap():iPath(NULL){}
	void ConstructL(const RMessage2& aMessage);
	
private:
	HBufC8 *iPath; //This is the key
	TUint iPort;  
	TProcessId iPid;
	CSession2* iSessionPtr;

	};
#endif /*FILESOCKETPORTPATHMAP_H_*/
