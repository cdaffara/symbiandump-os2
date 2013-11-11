/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines class used for testing pbe param internalization and externalization.
*
*/




/**
 @file
*/

#ifndef TEXTERNPBEPARAMS_H
#define TEXTERNPBEPARAMS_H

#include <pbedata.h>
#include "t_testaction.h"
#include "t_input.h"

class CExternPbeParams : public CTestAction
/**
	This class tests thatn encryption parameters can be
	successfully externalized and restored.  It is also used
	to ensure pre-PKSC#12 parameters can be read when PKCS#12
	is enabled.
 */
	{
public:
	static CTestAction* NewL(
		RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CExternPbeParams();
	
	// implement CTestAction
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);
	
	// override CTestAction
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	
private:
	CExternPbeParams(CConsoleBase& aConsole, Output& aOut, RFs& aFs);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	
	void DoPerformPrerequisiteL();
	
	static HBufC8* ReadHexStringL(const TDesC8& aBody, const TDesC8& aTag);
	static TInt ReadDecStringL(const TDesC8& aBody, const TDesC8& aTag);
	static HBufC8* ReadStringLC(const TDesC8& aBody, const TDesC8& aTag);
	
	CPBEncryptParms* InternalizeEncryptionParamsLC(const TDesC& aFileName);
	void CompareAgainstTestFileL(const TDesC& aFileName, const CPBEncryptParms& aParams);
	void TestDecodeMatchesScriptL(const TDesC& aFileName);
	void TestReExternMatchesL(const TDesC& aFileName);
	void TestScratchExternL(const TDesC& aFileName);
	
private:
	/** File server session used to write params to, and read params from, files. */
	RFs& iFs;
	
	/** Element body text. */
	HBufC8* iBody;

	/** Original externalized filename. */
	HBufC8* iOrigFileName;
	/** Numeric value of expected cipher. */
	TInt iExpCipher;
	/** Expected salt. */
	HBufC8* iExpSalt;
	/** Expected IV. */
	HBufC8* iExpIv;
	/** Expected iteration count. */
	TInt iExpIterCount;
	/**
		Expected key derivation function.  This is not stored
		as CPBEncryptParms::TKdf, which is not available when
		SYMBIAN_PKCS12 is not defined.
	 */
	TInt iExpKdf;
	};

#endif	// #ifndef TEXTERNPBEPARAMS_H

