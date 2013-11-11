/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Wrapper for RTest which logs to a file, coun1ts failures (without panicing) and
* generates a final result line the ONB will parse.
*
*/




/**
 @file
*/
#include "rtestwrapper.h"
#include <e32base.h>
#include <f32file.h>

class CTestConsole: public CConsoleBase
	{
public:
	static CTestConsole* NewL(CConsoleBase* aCon, const TDesC &aFileName);
	TInt Create(const TDesC16& aTitle,TSize aSize) {return iCon->Create(aTitle,aSize);};
	void Read(TRequestStatus& aStatus) {iCon->Read(aStatus);};
	void ReadCancel(void) {iCon->ReadCancel();};
	void Write(const TDesC16& aString);
	TPoint CursorPos(void) const {return iCon->CursorPos();};
	void SetCursorPosAbs(const TPoint& aPos) {iCon->SetCursorPosAbs(aPos);};
	void SetCursorPosRel(const TPoint& aPos) {iCon->SetCursorPosRel(aPos);};
	void SetCursorHeight(TInt aHeight) {iCon->SetCursorHeight(aHeight);};
	void SetTitle(const TDesC16& aTitle) {iCon->SetTitle(aTitle);};
	void ClearScreen(void) {iCon->ClearScreen();};
	void ClearToEndOfLine(void) {iCon->ClearToEndOfLine();};
	TSize ScreenSize(void) const {return iCon->ScreenSize();};
	TKeyCode KeyCode(void) const {return iCon->KeyCode();};
	TUint KeyModifiers(void) const {return iCon->KeyModifiers();};
	~CTestConsole(void);
private:
	void ConstructL(const TDesC &aFileName);
	CTestConsole(void);
	CConsoleBase* iCon;
	RFs iFs;
	RFile iFile;
	};

CTestConsole* CTestConsole::NewL(CConsoleBase* aCon, const TDesC &aFileName)
	{
	CTestConsole* self;
	self=new (ELeave) CTestConsole;
	CleanupStack::PushL(self);
	self->iCon=aCon;
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);
	return self;
	}

void CTestConsole::ConstructL(const TDesC &aFileName)
	{
	User::LeaveIfError(iFs.Connect());
	(void) iFs.Delete(aFileName);
	User::LeaveIfError(iFile.Create(iFs, aFileName, EFileShareAny | EFileWrite));
	}


CTestConsole::CTestConsole(void):CConsoleBase()
	{
	}

CTestConsole::~CTestConsole(void)

	{
	delete iCon;
	iFile.Close();
	iFs.Close();
	}

void CTestConsole::Write(const TDesC16& aString)

	{
	iCon->Write(aString);
	TUint8 space[200];
	TPtr8 ptr(space,200);
	ptr.Copy(aString);
	iFile.Write(ptr);
	}

EXPORT_C RTestWrapper::RTestWrapper(const TDesC &aTitle,TInt aThrowaway,const TText* anOtherThrowaway)
	: RTest(aTitle, aThrowaway, anOtherThrowaway),
	  iNestLevel(0),
	  iTestCount(0),
	  iThisTestFailed(EFalse),
	  iFailedCount(0),
	  iTitle(aTitle)
	{
	}

EXPORT_C void RTestWrapper::operator()(TInt aResult,TInt aLineNum,const TText* aFileName)
	{
	if(!aResult)
		{
		RTest::Printf(_L("RTEST: Test FAILED %s:%d\n"), aFileName, aLineNum);
		if(!iThisTestFailed)
			{
			++iFailedCount;
			}
		iThisTestFailed = ETrue;
		}
	}

EXPORT_C void RTestWrapper::Title(const TDesC &aFileName)
	{
	//	CheckConsoleCreated(); // Can not use this because they forgot to export it!
	RTest::Title(); // Make sure console is created before we try and wrap it
	CConsoleBase *console(0);
	TRAPD(err, console = CTestConsole::NewL(RTest::Console(), aFileName));
	if(err == KErrNone)
	{
		RTest::SetConsole(console);
	}

	RTest::Title();
	}


EXPORT_C void RTestWrapper::Start(const TDesC16 &aHeading)
	{
	RTest::Start(aHeading);
	++iNestLevel;
	++iTestCount;
	iThisTestFailed = EFalse;
	}

EXPORT_C void RTestWrapper::Next(const TDesC16 &aHeading)
	{
	RTest::Next(aHeading);
	++iTestCount;
	iThisTestFailed = EFalse;
	}

EXPORT_C void RTestWrapper::End()
	{
	RTest::End();
	--iNestLevel;
	if(iNestLevel == 0)
		{
		RTest::Printf(_L("\r\n%d tests failed out of %d\r\n"), iFailedCount, iTestCount);
		}
	}

// End of file
