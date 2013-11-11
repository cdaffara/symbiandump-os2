/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Main
*
*/


#include <T_PMLPAR.H>
#include <T_RTPAR.H>
#include <E32TWIN.H>


TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();

	// create console
	RConsole theConsole;
	TSize consSize(KConsFullScreen, KConsFullScreen);
	TInt err;
	err = theConsole.Create();
	err = theConsole.Control(_L("-Visibility"));
	err = theConsole.Set(_L("The Epoc RichText Parser (c)"), consSize);
	err = theConsole.Control(_L("Visibility Newline Max_win_size"));
 
	// Parse PML
	CParser* myParser;
	CRichText* richTextDoc;
	TRAPD(ret, myParser=CParser::NewL());
	TRAP(ret, richTextDoc = myParser->ParseL(theConsole));
	TInt dum=ret;

/* 	//enable this bit for a pause before executing test code
	// wait for keypress
	theConsole.Write(_L("\n\nPress space to execute test code & parse back to PML\n"));
	TConsoleKey keystroke;
	while (keystroke.Code() != EKeySpace)
		theConsole.Read(keystroke);
*/

// *********** insert test code here ****************


/*	TInt para=richTextDoc->CountParas();
	TInt length=richTextDoc->DocumentLength();
	CParaFormat* myformat;
	TRAP(ret,myformat=CParaFormat::NewL());
	TCharFormat charFormat; CPicture* picture; TPtrC view;
	TRAP(ret,richTextDoc->SenseParaFormatL(length,myformat));
	TRAP(ret,richTextDoc->SenseChars(0,view,charFormat,picture));
	TRAP(ret,richTextDoc->SenseChars(10,view,charFormat,picture));
	richTextDoc->Delete(4,6);
	//TRAP(ret,richTextDoc->InsertL(richTextDoc->DocumentLength(),EParagraphDelimiter));
*/

	// make rich text reader & read rich text doc & parse back to PML
	CRichTextReader* myRTReader;
	CBufSeg* returnedPmlDoc; 
	TRAP(ret, myRTReader = CRichTextReader::NewL());
	TRAP(ret, returnedPmlDoc = myRTReader->ConvertRichText(richTextDoc,theConsole));

	// save to disc
	CFileApp* myFileApp;
	TRAP(ret, myFileApp = CFileApp::NewL());
	TInt errorLevel = myFileApp->SaveFile(returnedPmlDoc,myParser->iFileName);
	if (errorLevel == 0)
		theConsole.Write(_L("\n\nFile Saved"));
	else
		theConsole.Write(_L("\n\nFile Save unsuccessful!!"));

	// wait for keypress
	theConsole.Write(_L("\n\nPress space to quit\n"));
	TConsoleKey keystroke;
	while (keystroke.Code() != EKeySpace)
		theConsole.Read(keystroke);

	// finish up
	Adt::Destroy(myFileApp);
	Adt::Destroy(myRTReader);
	Adt::Destroy(myParser);
	theConsole.Destroy();
	Adt::Destroy(cleanup);
	__UHEAP_MARKEND;
	return 0;
	}

