/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tlibcbackend.h
*
*/



 /*
* ==============================================================================
*  Name        : tlibcbackend.h
*  Part of     : backend/testlibcbackend
*
*  Description : ?Description
*  Version:   0.5
*
*  Copyright (C) 2002 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing,  adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
*
* ============================================================================
*/

#ifndef __TESTLIBCBACKEND_H__
#define __TESTLIBCBACKEND_H__

#include <test/testexecutestepbase.h>

_LIT(KSocketIOTest, "SocketIOTest");
_LIT(KFileIOTest, "FileIOTest");
_LIT(KServerLockTest, "ServerLockTest");
_LIT(KOpenDirTest, "OpenDirTest");
_LIT(KPopenPipeTest, "PopenPipeTest");



class CTestLibcbackend : public CTestStep
	{
public:
	~CTestLibcbackend(); 
	CTestLibcbackend(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostAmbleL();
private:
	TInt SocketIOTest(void);
	TInt FileIOTest(void);
	TInt ServerLockTest(void);
	TInt OpenDirTest(void);
	TInt PopenPipeTest(void);
	
	//utility methods
	void ReadIntParam(TInt &aInt);

private: //data
	TInt iParamCnt;
	
	};
#endif 
