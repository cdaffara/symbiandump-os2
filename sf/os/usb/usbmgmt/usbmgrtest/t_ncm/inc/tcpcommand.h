/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
 @internalComponent
 @test
 */

#ifndef TCPCOMMAND_H
#define TCPCOMMAND_H

#include "commandengine.h"

//The max count of tcp test in a same time
const static TInt KMaxTcpTestCount = 5;

class CTcpTestConsole;

NONSHARABLE_CLASS(CTcpCommand) : public CNcmCommandBase
/**
Make TCP test: Create a tcp connection and tansfer data on it.
*/
	{
public:
	static CTcpCommand* NewL(CUsbNcmConsole& aUsb, TUint aKey, TBool aIsTcp, TBool aIsServer);
	~CTcpCommand();

public:
	//From CNcmCommandBase
	void DoCommandL();

public:
	void CloseTcpTest(TInt aIndex);
	
private:
	CTcpCommand(CUsbNcmConsole& aUsb, TUint aKey, TBool aIsTcp, TBool aIsServer);
	void ConstructL();
	
private:
	//Indicate the tcp type: ETrue - as TCP server; EFalse - as TCP client
	TBool iIsServer;
	TBool iIsTcp;
	//Store the instances of tcp test console
	CTcpTestConsole* iTestArray[KMaxTcpTestCount];
	};

#endif // TCPCOMMAND_H
