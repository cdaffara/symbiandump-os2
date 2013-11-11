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
* testutil - server classes
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include <e32base.h>
#include <f32file.h>
#include "testutilssessioncommon.h"

class CTestUtilServer : public CServer2
	{
public:
	static CServer2* NewLC();
	virtual ~CTestUtilServer();
	void AddSession();
	void DropSession();
	void Send(const TDesC& aMessage);
	inline RFs& FS();
	inline CFileMan& FileMan();
private:
	CTestUtilServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	TInt iSessionCount;
	RFs iFs;
	CFileMan* iFileMan;
	CTestUtilSessionCommon* iTestUtilSessionCommon;
	};

// CTestUtilServer inlines
inline CTestUtilServer::CTestUtilServer()
	:CServer2(0,ESharableSessions)
	{}

inline RFs& CTestUtilServer::FS()
	{
	return iFs;
	}

inline CFileMan& CTestUtilServer::FileMan()
	{
	return *iFileMan;
	}
