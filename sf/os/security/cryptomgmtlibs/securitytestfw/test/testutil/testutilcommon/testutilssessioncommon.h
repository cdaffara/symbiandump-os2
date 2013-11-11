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
* Common class for the testutil and testutilswi
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include <e32base.h>

enum TTestUtilPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving
	};

void PanicClient(const RMessagePtr2& aMessage,TTestUtilPanic TTestUtilPanic);

class CTestUtilSessionCommon : public CBase
	{
public:
	CTestUtilSessionCommon();
	~CTestUtilSessionCommon();
private:
	static HBufC* AllocateInputBufferLC(const RMessage2& aMessage,TInt aParam);
	void Send(const TDesC& aMessage);
	inline TBool ReceivePending() const;
	
	friend class CTestUtilSessionSwi;
	friend class CTestUtilServerSwi;
	friend class CTestUtilSession;
	friend class CTestUtilServer;
	friend class CTestFileDetector;
private:
	RMessagePtr2 iReceiveMsg;
	};

// CTestUtilSession inlines
inline TBool CTestUtilSessionCommon::ReceivePending() const
	{return !iReceiveMsg.IsNull();}

