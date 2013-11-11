// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSPConvUtil class
// 
//

#include <f32file.h>
#include <e32cons.h>
#include "D32Assert.h"
#include "cn_util.h"
#include "cn_main.h"
#include "cn_cmdparse.h"

TBool TSPConvUtil::iPromptOnError = ETrue;

_LIT(KTxt0, "Program information");
_LIT(KTxt1, "EDbSpConv version 1.0\n");
_LIT(KTxt2, "Symbian OS text policy file/binary policy file converter\n");
_LIT(KTxt3, "Copyright (c) 2004 Symbian Software Ltd. All rights reserved.\n");
_LIT(KTxt4, "\nUsage:\n");
_LIT(KTxt5, "EDbSpConv /h\n");
_LIT(KTxt6, "EDbSpConv /f=<text file> /b=[path]\\<uid>[.spd] [/s]\n");
_LIT(KTxt7, "\nWhere:\n");
_LIT(KTxt8, "/h          - displays program information\n");
_LIT(KTxt9, "<text file> - security policy text file, including path\n");
_LIT(KTxt10,"<uid>[.spd] - security policy binary file\n");
_LIT(KTxt11,"[path]      - optional - binary policy file path.\n");
_LIT(KTxt12,"              By default the file will be placed in the current directory.\n");
_LIT(KTxt13,"<uid>       - database format uid - hex\n");
_LIT(KTxt14,"[/s]        - optional - does not wait for a button pressing in case\n");
_LIT(KTxt15,"              of an error (when an error message is displayed).\n");

/**
Prints program information.
*/
static void PrintInfoL()
	{
	CConsoleBase* con = Console::NewL(KTxt0, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(con);
	con->Printf(KTxt1);
	con->Printf(KTxt2);
	con->Printf(KTxt3);
	con->Printf(KTxt4);
	con->Printf(KTxt5);
	con->Printf(KTxt6);
	con->Printf(KTxt7);
	con->Printf(KTxt8);
	con->Printf(KTxt9);
	con->Printf(KTxt10);
	con->Printf(KTxt11);
	con->Printf(KTxt12);
	con->Printf(KTxt13);
	con->Printf(KTxt14);
	con->Printf(KTxt15);
	con->Getch();
	CleanupStack::PopAndDestroy(con);
	}

/**
Parses command line arguments and fills TCmdLinePrm structure with the parsed data.
A a result of method's execution, the following items will be stored in aCmdLinePrm:
requested action: (BIN->TXT) or (TXT->BIN), text policy file path and
binary policy file path.
If there is a "/s" command line argument and there are parsing errors, the error message
will be stored in epocwind.out file only. By default (no "/s" command) the error messages 
will be displayed on the screen and stored in epocwind.out file.
@param aCmdLineParser A reference to a command line argument parser instance.
@param aCmdLinePrm A reference to a TCmdLinePrm instance. Output parameter. The parsed
                   command line arguments will be stored there.
@leave KErrArgument Not enough command line arguments, bad argument, non-recognizable argument.
@leave KErrNotFound Missing command line argument.
*/
void TSPConvUtil::ParseL(const CCommandLineArguments& aCmdLineParser, 
						 TCmdLinePrm& aCmdLinePrm)
	{
	aCmdLinePrm.Zero();
	TInt prmCnt = aCmdLineParser.Count();
	if(prmCnt < 2)
		{//Print program information and exit
		::PrintInfoL();
		User::Exit(KErrNone);
		}
	else
		{
		//The comand is fixed explicitly, because now there is only one conversion:
		//from text policy file to binary policy file.
		aCmdLinePrm.iAction = TCmdLinePrm::ETxt2Bin;
		for(TInt i=0;i<prmCnt;++i)
			{
			TPtrC arg = aCmdLineParser.Arg(i);
			_LIT(KPrintInfoCmd, "/H");
			_LIT(KSuppressPromptCmd, "/S");
			_LIT(KTextFilePrm, "/F=");
			_LIT(KBinFilePrm, "/B=");
			TInt pos;
			if(arg == KPrintInfoCmd)
				{//Print program information and exit
				::PrintInfoL();
				User::Exit(KErrNone);
				}
			else if(arg == KSuppressPromptCmd)
				{
				TSPConvUtil::iPromptOnError = EFalse;
				}
			if((pos = arg.Find(KTextFilePrm)) != KErrNotFound)
				{
				if(pos != 0)
					{
					__LEAVE(KErrArgument);
					}
				aCmdLinePrm.iTxtFile.Copy(arg.Right(arg.Length() - KTextFilePrm().Length()));
				}
			else if((pos = arg.Find(KBinFilePrm)) != KErrNotFound)
				{
				if(pos != 0)
					{
					__LEAVE(KErrArgument);
					}
				aCmdLinePrm.iBinFile.Copy(arg.Right(arg.Length() - KBinFilePrm().Length()));
				}
			}//end of - for(TInt i=0;i<prmCnt;++i)
		}//end of else - if(prmCnt < 2)
	if(aCmdLinePrm.iAction == TCmdLinePrm::ENone)
		{
		_LIT(KText, "Invalid command line arguments, use \"/h\" option for help\n");
		TSPConvUtil::Print(KText);
		__LEAVE(KErrArgument);
		}
	if(aCmdLinePrm.iTxtFile.Length() == 0)
		{
		_LIT(KText, "No text file, use \"/h\" option for help\n");
		TSPConvUtil::Print(KText);
		__LEAVE(KErrArgument);
		}
	if(aCmdLinePrm.iBinFile.Length() == 0)
		{
		_LIT(KText, "No UID, use \"/h\" option for help\n");
		TSPConvUtil::Print(KText);
		__LEAVE(KErrArgument);
		}
	}

/**
The method checks if the file path (aFile parameter) exists.
@param aFs File server session.
@param aFile File path.
@return ETrue - file exists, EFalse - file not found. EFalse may also reflect some kind of
        file system error.
*/
TBool TSPConvUtil::FileExists(RFs& aFs, const TDesC& aFile)
	{
	TEntry fileEntry;
	return aFs.Entry(aFile, fileEntry) == KErrNone;
	}

/**
The method checks and constructs full binary policy file path from aFile parameter.
@param aFile The expected format is:
             1) <drive:>\<path>\<UID string>
             2) <drive:>\<path>\<UID string>.spd
             3) <UID string>
             4) <UID string>.spd
			 '/' symbol might be used as a directory separator as well.
			 Output parameter - the created binary policy file path will be stored there.
@leave KErrArgument Bad format of aFile input parameter.
*/
void TSPConvUtil::ConstructBinFileNameL(TDes& aFile)
	{
	//Replace all '/' in aFile with '\', otherwise TParse won't work properly.
	TInt pos;
	while((pos = aFile.Locate('/')) != KErrNotFound)
		{
		aFile[pos] = '\\';
		}
	TParse fileNameParser;
	__LEAVE_IF_ERROR(fileNameParser.Set(aFile, NULL, NULL));
	TPtrC fileName = fileNameParser.Name();
	TUid dbUid;
	TLex lex(fileName);
	if(lex.Val(*(TUint32*)&dbUid, EHex) == KErrNone && lex.Eos())
		{
		if(dbUid != KNullUid)
			{
			_LIT(KExt, ".SPD");
			TPtrC fileExt = fileNameParser.Ext();
			if(fileExt.Length() == 0)
				{
				aFile.Append(KExt);
				}
			else if(fileExt != KExt)
				{
				_LIT(KText, "Invalid \"UID\" file extension: \"%S\"\n");
				TSPConvUtil::Print(KText, fileExt);
				__LEAVE(KErrArgument);
				}
			return;
			}
		}
	_LIT(KText, "Invalid \"UID\" file: \"%S\"\n");
	TSPConvUtil::Print(KText, aFile);
	__LEAVE(KErrArgument);
	}

/**
The method extracts the UID from aFile parameter, which is expected to represent 
binary security policy file path.
The method asserts that the extracted UID is not KNullUid.
@param aFile Binary policy file path
@leave System-wide error codes from file name parsing or KErrNoMemory (from the parser creation).
*/
TUid TSPConvUtil::UidFromFileNameL(const TDesC& aFile)
	{
	TParse* parser = new (ELeave) TParse;
	CleanupStack::PushL(parser);
	__LEAVE_IF_ERROR(parser->Set(aFile, NULL, NULL));
	TPtrC fileName = parser->Name();
	TUid domainUid;
	TLex lex(fileName);
	if(lex.Val(*(TUint32*)&domainUid, EHex) == KErrNone && lex.Eos() && domainUid != KNullUid)
		{
		}
	CleanupStack::PopAndDestroy(parser);
	__ASSERT(domainUid != KNullUid);
	return domainUid;
	}

/**
The method prints aText string to epocwnd.out file and on the screen and waits for
a button pressing.
If "/s" command line argument is presented, the method won't wait for a button pressing.
@param aText The text which has to be printed.
*/
void TSPConvUtil::Print(const TDesC& aText)
	{
	RDebug::Print(aText);
	if(TSPConvUtil::iPromptOnError)
		{
		RNotifier notify;
		TInt err = notify.Connect();
		if(err == KErrNone)
			{
			TRequestStatus stat;
			TInt but;
			_LIT(KNotify,"EDBSPConv");
			_LIT(KContinue,"Continue");
			notify.Notify(KNotify, aText, KContinue, KNullDesC, but, stat);
			User::WaitForRequest(stat);
			notify.Close();
			}
		else
			{
			RDebug::Print(_L("Error=%d connecting notifier session!\n"), err);
			}
		}
	User::InfoPrint(aText);
	}

/**
The method outputs a formatted text to epocwnd.out file and on the screen using aFormat 
parameter as a format string and aNumber as a number which has to be printed out with 
the supplied format string.
If "/s" command line argument is presented, the method won't wait for a button pressing.
@param aFormat The number format string.
@param aNumber The number, which has to be printed together with the text. There must be a "%d"
               format specification somewhere in aFormat parameter.
*/
void TSPConvUtil::Print(const TDesC& aFormat, TInt aNumber)
	{
	TBuf<300> buf;
	buf.Format(aFormat, aNumber);
	TSPConvUtil::Print(buf);
	}

/**
The method outputs a formatted text to epocwnd.out file and on the screen using aFormat 
parameter as a format string and aText as a text which has to be printed out with the 
supplied format string.
If "/s" command line argument is presented, the method won't wait for a button pressing.
@param aFormat The number format string.
@param aText The text, which has to be formatted. There must be a "%S"
               format specification somewhere in aFormat parameter.
*/
void TSPConvUtil::Print(const TDesC& aFormat, const TDesC& aText)
	{
	TBuf<300> buf;
	buf.Format(aFormat, &aText);
	TSPConvUtil::Print(buf);
	}

/**
The method outputs a formatted text to epocwnd.out file and on the screen using aFormat 
parameter as a format string and aText1 and aText2 as texts which have to be printed 
out with the supplied format string.
If "/s" command line argument is presented, the method won't wait for a button pressing.
@param aFormat The number format string.
@param aText1 The text, which has to be printed out. There must be a "%S"
               format specification somewhere in aFormat parameter.
@param aText2 The text, which has to be printed out. There must be a "%S"
               format specification somewhere in aFormat parameter.
*/
void TSPConvUtil::Print(const TDesC& aFormat, const TDesC& aText1, const TDesC& aText2)
	{
	TBuf<500> buf;
	buf.Format(aFormat, &aText1, &aText2);
	TSPConvUtil::Print(buf);
	}

