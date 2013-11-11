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
*
*/

#include <e32base.h>
#include <e32keys.h>
#include "testmanager.h"

class CTestBase;
class CConsoleBase;

class CActiveConsole : public CActive, public MTestManager
/**
 * Active Object console class. This is the means by which the user interacts 
 * with the program.
 */
	{
public:
	static CActiveConsole* NewLC(CConsoleBase& aConsole);
	~CActiveConsole();

public:
	void RequestCharacter();
	TKeyCode Getch();

private:
	CActiveConsole(CConsoleBase& aConsole);
	void ConstructL();

private: // utility
	void SelectTestL();
	void StopCurrentTest();
	void DisplayMainMenu();
	void DoActionKeyL(TKeyCode aKey);

private: // from MTestManager
	void TestFinished();
	void Write(TRefByValue<const TDesC8> aFmt, ...);
	void WriteNoReturn(TRefByValue<const TDesC8> aFmt, ...);
	void GetNumberL(TUint& aNumber);

private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private: // owned
	CTestBase* iTest;
	TBuf8<10> iInputBuffer;

private: // unowned
	CConsoleBase& iConsole;
	};
