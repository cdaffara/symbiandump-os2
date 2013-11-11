/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>
#include <hash.h>
#include <f32file.h>

//TBool gMD2Test=ETrue;
//TBool gMD5Test=ETrue;
//TBool gSHATest=ETrue;
//TBool gSHA1Test=ETrue;
//TBool gHMACTest=ETrue;
//TBool gErrorTest=ETrue;
//TBool gPerformanceTest=ETrue;
//TBool gVectorTest=ETrue;
//TBool gLogging=ETrue;
//TBool gFunctionalityTest=ETrue;
//TBool gBugs=ETrue;
//TBool gFixed=ETrue;
//TBool gUnfixed=EFalse;


class CTestData:public CBase

	{
	public:
		static CTestData* NewL(const TDesC& aFilename);
		enum TType { EMessage, EData, EFinished,EFileName,EError=-1 } ;
		TType Type(void);
		HBufC* Message(void);
		HBufC8* operator [] (TInt aIndex);
		~CTestData(void);
	private:
		CTestData(void);
		TInt iCurrentPlace;
		void ConstructL(const TDesC& aFilename);
		HBufC8* iLine;
		HBufC8* iFile;
	};


class CTestConsole:public CConsoleBase

	{
	public:
		static CTestConsole* NewL(CConsoleBase* aCon);
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
		void SetLogFile(RFile* aFile);
	private:
		CTestConsole(void);
		CConsoleBase* iCon;
		RFile* iFile;
	};

