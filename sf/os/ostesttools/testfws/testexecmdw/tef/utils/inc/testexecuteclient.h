/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file TestExecuteClient.h
*/

#if !(defined __TESTEXECUTE_CLIENT_H__)
#define __TESTEXECUTE_CLIENT_H__

#include <e32base.h>
#include <test/tefexportconst.h>
#include <test/blockitems.h>

enum
	{
	EConnectServer,
	EOpenTestStep,
	ERunTestStep,
	EAbortTestStep,
	ECloseTestStep,
	EOpenTestBlock,
	ERunTestBlock
	};

class RTestServ : public RSessionBase
/**
@publishedPartner
@test
*/
	{
public:
	IMPORT_C TInt Connect(const TDesC& aServerName, TBool aDebugMode = EFalse);
	IMPORT_C const TDesC& ServerName() const;
protected:
private:
	RLibrary iLibrary;
	TBuf<KMaxTestExecuteNameLength> iServerName;
	TVersion Version() const;
	};

class RTestSession : public RSubSessionBase
/**
@publishedPartner
@test
*/
	{
public:
	IMPORT_C TInt Open(RTestServ& aServ, const TDesC& aStepName, TBool aSharedData = EFalse);
	IMPORT_C void RunTestStep(const TDesC& aCommandString, TDes& aPanicString, TRequestStatus& aStatus);
	IMPORT_C TInt AbortTestStep();
	IMPORT_C void Close();
	IMPORT_C TInt Open(RTestServ& aServ, TBool aSharedData = EFalse);
	IMPORT_C void RunTestBlock(const TDesC& aCommandString, TDes& aPanicString, TDes8& aBlockArray, TRequestStatus& aStatus);
	inline	RTestSession() : iBlockArrayPtr((TUint8*)(NULL),0) {}
	
protected:
private:
	TPtr8	iBlockArrayPtr;
	};

#endif
