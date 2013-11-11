/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* smoke.h - basic build & sanity test main
*
*/


#include <e32test.h>
#include <f32file.h>
#include <caf/caf.h>

class CTestData: public CBase

{
 public:
  static CTestData* NewL(const TDesC& aFilename);
  enum TType { EMessage, EData, EFinished,EError=-1 } ;
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

