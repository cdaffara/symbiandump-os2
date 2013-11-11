// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CENTREPCONVTOOL_H__
#define __CENTREPCONVTOOL_H__

#include <e32base.h>
#include <f32file.h>
#include "consoleprint.h"
#include "shrepos.h"

const TInt KPrivatePathSize = 24;

// class CCentRepConvTool - parse the command line args to 
//     generate the input and output filenames. Then use objects
//     of centrep server to do the convertion.
class CCentRepConvTool : public CBase
	{
public:

	// By default error msgs wait for user ack. Test code should set
	// the bool to false.
	static CCentRepConvTool* NewL(const TDesC& aCmd, RFs& aFs, TBool aWaitForAck);
	virtual ~CCentRepConvTool();
    void ProcessCmdL();
	void SetOutputMode(TBool aWaitForAck);

private:	
	CCentRepConvTool(const TDesC& aCmd, RFs& aFs);
	void ConstructL(TBool aWaitForAck);
	void ParseCmdLineL(TPtrC& aInputPath, TPtrC& aOutputPath);
	void VerifyInputPathL();
	void VerifyOutputPathL();
	void DoConversionL();
	void ExternalizeToCreL();
	TBool InOthersPrivatePath(const TDesC& aFullPathName);

private:
	TPtrC iCmd;
	RFs&  iFs;
	CConsolePrint* iScrnOutput;
	TParse iInputPath;
	TParse iOutputPath;
	CSharedRepository* iCentRepShrepos;
	TBool iTextToBin;
	TUid  iRepUid;
	TBuf<KPrivatePathSize> iMyDataCage;
	TBuf<KMaxDriveName + 1> iDefaultPath;
	};

#endif // __CENTREPCONVTOOL_H__
