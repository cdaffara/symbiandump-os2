/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Console redirection server 
* 
*
*/



#include <redirstr.h>
#include <e32keys.h>		//for TKeyCode


class CActiveConsole;
NONSHARABLE_CLASS(CTestConsole) : public CStreamBase2
	{
public:
	CTestConsole();
	~CTestConsole();
	virtual void Write(TDes8& aDes);
	virtual void Read(const RMessage2& aMessage);
	void DoneReading(TKeyCode aKeyCode);
private:
	CConsoleBase* iConsole;
	CActiveConsole* iActiveChap;
	RMessage2 iMessage;

	};


NONSHARABLE_CLASS(CTestConsoleFactory) : public CStreamFactoryBase2
	{
public:
	static CTestConsoleFactory* NewL();
	virtual CStreamBase2* GetStream();
	virtual void Configure();
	};


