/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Cache Deletion for fntstore multi-processed test code
*
*/


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>
#include <e32std.h>
#include <e32debug.h>
#include "FNTSTORE.H"
#include "FNTBODY.H"
#include "FNTSTD.H"
#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>

_LIT(KOpenFont, "DejaVu Sans Condensed");

#ifdef __WINSCW__
//this is used for winscw breakpoints
#define BR _asm( int 3);
#endif


/* it is expected that the main in this file will be called to test multiple 
process output at the same time a process is being deleted (font and bitmap server
disconnection.  There are  normally two instances of this process.  Two virtually
identical processes are required to ensure that the session ID is the same.

The first is with aThirdProcess set. This sets output to run in a loop until
the timeout is completed.  These values  are input via program arguments.

If aThirdProcess is false then only one font creation, draw text,
font deletion cycle is completed. The test code will then repeatedly run
this process with aThirdProcess set to false.
*/
class TRunProc2: public CBase
	{
public:
	static TRunProc2* NewL();
	void RunTestL(TInt aThirdProcess, TInt aTimeout);
	~TRunProc2();
private:
	TRunProc2(){};
	void ConstructL();
	void DrawText();
	void CreateFont();

private:
	RFbsSession* iFbs;
	CFbsBitGc* iGc;
	CFbsScreenDevice* iDev;
	};

TRunProc2::~TRunProc2()
	{
	delete iGc;
	delete iDev;
	iFbs->Disconnect();
	}

void TRunProc2::ConstructL()
	{
	TInt err = RFbsSession::Connect();
	User::LeaveIfError(err);
	iFbs = RFbsSession::GetSession();
	User::LeaveIfNull(iFbs);

	TDisplayMode mode[13];
	mode[0]=EGray2;
	mode[1]=EGray4;
	mode[2]=EGray16;
	mode[3]=EGray256;
	mode[4]=EColor16;
	mode[5]=EColor256;
	mode[6]=EColor64K;
	mode[7]=EColor16M;
	mode[8]=ERgb;
	mode[9]=EColor4K;
	mode[10]=EColor16MU;
	mode[11]=EColor16MA;
	mode[12]=EColor16MAP;
	

	TInt count;
	for (count=0;count<13;count++)
		{
		TRAP(err, iDev = CFbsScreenDevice::NewL(KNullDesC, mode[count]));
		if (err!=KErrNotSupported)
			{
			break;
			}
		}

	User::LeaveIfNull(iDev);

	if(err == KErrNone)
		{
		iDev->ChangeScreenDevice(NULL);
		iDev->SetAutoUpdate(ETrue);
		iDev->CreateContext(iGc);
		}
	User::LeaveIfNull(iGc);
	}

TRunProc2* TRunProc2::NewL()
	{
	TRunProc2 *ptr = new (ELeave) TRunProc2;
	CleanupStack::PushL(ptr);
	ptr->ConstructL();
	CleanupStack::Pop();
	return ptr;
	}

void TRunProc2::CreateFont()
	{
	//make sure that the font is large enough to ensure that the session
	//cache is used.
	TInt height=220;

	TOpenFontSpec openFontSpec;
	openFontSpec.SetName(KOpenFont);
	openFontSpec.SetHeight(height);
	openFontSpec.SetItalic(EFalse);
	openFontSpec.SetBold(EFalse);

	TTypeface Typeface;
	Typeface.iName = KOpenFont;
	TFontSpec fs;
	fs.iTypeface = Typeface;

	fs.iHeight = height;
	CFbsFont* font = NULL;
	TInt err = iDev->GetNearestFontToDesignHeightInPixels(font, fs);

	User::LeaveIfNull(font);

	// Use the font
	iGc->UseFont(font);
	iGc->Clear();
	}

void TRunProc2::RunTestL(TInt aThirdProcess, TInt aTimeout)
	{
	if (aThirdProcess)
		{
		RTimer timer;

		timer.CreateLocal();
		TRequestStatus timerStatus=KRequestPending;
		TTimeIntervalMicroSeconds32 timeout(aTimeout);
		timer.After(timerStatus, timeout);

		CreateFont();
		do
			{
			DrawText();
			}
		while (timerStatus==KRequestPending);
		timer.Cancel();
		iGc->DiscardFont();
		RProcess::Rendezvous(10);
		}
	else
		{
		CreateFont();
		DrawText();
		iGc->DiscardFont();
		RProcess::Rendezvous(1);
		}
	}

void TRunProc2::DrawText()
	{
	TText ch[2];
	ch[1]='\0';
	for (ch[0]='A';ch[0]<='Z';ch[0]++)
		{
		TBufC<2> buf(ch);
		iGc->DrawText(buf,TPoint(10,100));
		}
	for (ch[0]='a';ch[0]<='z';ch[0]++)
		{
		TBufC<2> buf(ch);
		iGc->DrawText(buf,TPoint(10,100));
		}
	}

void MainL()
	{
	TRunProc2* test= TRunProc2::NewL();
	CleanupStack::PushL(test);
	TInt param;
	TInt timeoutParam;
	User::LeaveIfError(User::GetTIntParameter(1,param));
	User::LeaveIfError(User::GetTIntParameter(2,timeoutParam));
	test->RunTestL(param,timeoutParam);
	CleanupStack::PopAndDestroy();
	}

// Cleanup stack harness
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	TRAPD(error, MainL());
	_LIT(KTCacheDeletionProcess,"TCacheDeletionProcess");
	__ASSERT_ALWAYS(!error, User::Panic(KTCacheDeletionProcess, error));
	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}
