// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//


#include <f32file.h>
#include <e32std.h>
#include <e32test.h>
#include <s32file.h>
#include <numberconversion.h>
#include "t_logutil.h"

_LIT(KSeparator, ";"); // Invalid filepath char used to separate filenames

RTest TheTest(_L("t_logapi_helper"));

/*
This method helps to do operations on files and folders, like copy, delete, rename, create directory (MkDir)
This process has a capabilities to write in all files of the system.
-param: anOld: source file
-param: aNew : destination file
-param: aCode: The operation to do. 
*/
void OperationOnFilesAndFolders(const TDesC& anOld, const TDesC& aNew, TInt aCode)
	{
	RFs fs;
	TInt err = fs.Connect();
	TEST2(err, KErrNone);
	
	CFileMan* fileMan = NULL; 
	TRAP(err, fileMan = CFileMan::NewL(fs));
    TEST2(err, KErrNone);
    
	switch(aCode)
		{
		case 0:
			TheTest.Printf(_L("Copying source file: %S %S\r\n"), &anOld, &aNew);
			err = fileMan->Copy(anOld, aNew);		
			TEST2(err, KErrNone);
			break;

		case 1:
			TheTest.Printf(_L("Renaming file: %S %S\r\n"), &anOld, &aNew);
			err = fileMan->Rename(anOld, aNew);
            TEST2(err, KErrNone);
			break;

		case 2:
			TheTest.Printf(_L("Deleting file: %S\r\n"), &anOld);
			err = fileMan->Attribs(anOld, 0, KEntryAttReadOnly, 0, CFileMan::EOverWrite);
			if(err == KErrNone)
			    {
			    err = fileMan->Delete(anOld);
			    }
            TEST(err == KErrNone || err == KErrNotFound);
			break;
		case 3:
			TheTest.Printf(_L("Creating folder: %S\r\n"), &anOld);
			err = fs.MkDir(anOld);		
			TEST(err == KErrNone || err == KErrAlreadyExists);
			break;
		default: 
		    break;
		}
	delete fileMan;
	fs.Close();
	}

TInt E32Main()
	{
	CTrapCleanup* tc = CTrapCleanup::New();
	TheTest(tc != NULL);
	
	__UHEAP_MARK;
	
	TheTest.Start(_L("Start: t_logapi_helper process started... "));
	TheTest.Title();
	
	TBuf<100> args;
	User::CommandLine(args);
	TInt cmdLength = User::CommandLineLength();
	
	TInt pos = args.Find(KSeparator);
	TBuf<50> oldName(args.Mid(0,pos));
	TInt oldNameLength = oldName.Length();
	TInt remainderLength = cmdLength - oldNameLength;
	
	TBuf<50> remainder(args.Mid(pos+1, remainderLength-1));
	pos = remainder.Find(KSeparator);
	
	TDigitType digitType;
	TInt length;
	TBuf<50> newName(remainder.Mid(0, pos));
	TBuf<1> code = remainder.Mid(pos+1, 1);
	TInt numberLength = code.Length();
	
	TInt operationMode = NumberConversion::ConvertFirstNumber(code, length, digitType);
	
	OperationOnFilesAndFolders(oldName, newName, operationMode);
	
    TheTest.End();
    TheTest.Close();
	
	__UHEAP_MARKEND;
	
	delete tc;
	
	return 0;
	}

