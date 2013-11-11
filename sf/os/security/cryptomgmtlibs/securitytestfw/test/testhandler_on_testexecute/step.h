/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




/**
 @file
*/
#ifndef __STEP_H__
#define __STEP_H__

#include <e32cons.h>
#include <test/testexecutestepbase.h>

struct TScriptTests;

namespace TestExecuteInterface
    {
    class MTester;
    class CStep;
    
    class CStepConsole : public ::CConsoleBase
	{
    public:
	CStepConsole(CStep *aStep) : iStep(aStep) {}
    public: // From CConsoleBase
	virtual TInt Create(const TDesC& /*aTitle*/, TSize /*aSize*/) { return KErrNone; }
	virtual void Read(TRequestStatus &aStatus);
	virtual void ReadCancel() {}
	virtual void Write(const TDesC &aDes);
	virtual TPoint CursorPos() const { return TPoint(); }
	virtual void SetCursorPosAbs(const TPoint& /*aPoint*/) {}
	virtual void SetCursorPosRel(const TPoint& /*aPoint*/) {}
	virtual void SetCursorHeight(TInt /*aPercentage*/) {}
	virtual void SetTitle(const TDesC& /*aTitle*/) {}
	virtual void ClearScreen() {}
	virtual void ClearToEndOfLine() {}
	virtual TSize ScreenSize() const { return TSize(); }
	virtual TKeyCode KeyCode() const { return TKeyCode(); }
	virtual TUint KeyModifiers() const { return 0; }
    private:
	CStep *iStep;
	};
    
    class CStep : public ::CTestStep
	{
    public:
	CStep(TScriptTests *aTestTypes) ;
	~CStep();
	
    public: // from CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

	virtual TInt Create(const TDesC& /*aTitle*/, TSize /*aSize*/) { return KErrNone; }
	virtual void Read(TRequestStatus &aStatus);
	virtual void ReadCancel() {}
	virtual void Write(const TDesC &aDes);
	virtual TPoint CursorPos() const { return iConsole->CursorPos(); }
	virtual void SetCursorPosAbs(const TPoint& /*aPoint*/) {}
	virtual void SetCursorPosRel(const TPoint& /*aPoint*/) {}
	virtual void SetCursorHeight(TInt /*aPercentage*/) {}
	virtual void SetTitle(const TDesC& /*aTitle*/) {}
	virtual void ClearScreen() {}
	virtual void ClearToEndOfLine() {}
	virtual TSize ScreenSize() const { return iConsole->ScreenSize(); }
	virtual TKeyCode KeyCode() const { return iConsole->KeyCode(); }
	virtual TUint KeyModifiers() const { return 0; }
	
    private:
	TScriptTests* iTestTypes;
	CStepConsole *iConsole;
	};
        
    }
    
#endif
