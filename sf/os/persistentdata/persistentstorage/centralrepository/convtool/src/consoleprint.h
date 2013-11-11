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

#ifndef __CONSOLEPRINT_H__
#define __CONSOLEPRINT_H__

#include <e32base.h>
#include <e32cons.h>

const TUint32 KTimeToWait = 30000000;

// class CConsolePrint - provide functions to do screen output.
class CConsolePrint : public CBase
	{
public:
	static CConsolePrint* NewL(TBool aWaitForAck);
	virtual ~CConsolePrint();
    void Printf(TRefByValue<const TDesC> aFmt, ...);
	void SetWaitMode(TBool aWaitForAck);

private:	
	CConsolePrint(TBool aWaitForAck);
	void ConstructL();
	void WaitForUserAck();

private:
	CConsoleBase* iConsole;
	TBool iWaitForAck;
	TBuf<256> iBuf;
	};

#endif // __CONSOLEPRINT_H__
