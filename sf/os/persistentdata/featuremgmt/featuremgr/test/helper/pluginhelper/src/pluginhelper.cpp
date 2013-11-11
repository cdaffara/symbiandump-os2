// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
#include <f32file.h>
#include <e32cmn.h>
#include <e32std.h>
#include <e32const.h>
#include <e32def_private.h>

void CopyFileL(const TDesC& aSourceFile, const TDesC& aDestFile)
{
   RFs fs;
   fs.Connect();
   CleanupClosePushL(fs);
   fs.MkDirAll(aDestFile);
   CFileMan* fileMan = CFileMan::NewL( fs );
   // Copy feature file to file manager private data cage.
   //Ignore any errors as the file or path may not exist
   fileMan->Copy(aSourceFile, aDestFile);   
   CleanupStack::PopAndDestroy(&fs);
   }
    
void DeleteFileL(const TDesC& aFile)
   {
   RFs fs;
   fs.Connect();
   CleanupClosePushL(fs);	
   fs.SetAtt(aFile, 0, KEntryAttReadOnly);
   //Ignore any errors as the file or path may not exist
   fs.Delete( aFile );
   CleanupStack::PopAndDestroy(&fs);
   
   }

TInt CheckFile(const TDesC& aFile)
	{
	RFs fs;
	fs.Connect();
	RFile file;
	TInt err = file.Open(fs,aFile,EFileRead);	   
	if (err!=KErrNotFound)
		{
		file.Close();
		fs.Close();
		return KErrNone;
		}
	else
		{
		fs.Close();
		return KErrNotFound;
		}
	}

TInt ParseSwitch(const TDesC& aSwitch, const TDesC& aFile)
	{
	if (aSwitch==_L("?"))
		return CheckFile(aFile);
	else 
		return KErrNotSupported;
	}

static void DoMainL()
{
	HBufC* cmdLine = HBufC::NewL(User::CommandLineLength());
   CleanupStack::PushL(cmdLine);
   
   TPtr cmdLinePtr(cmdLine->Des());
   User::CommandLine(cmdLinePtr);
   
   const TInt KMaxNumTokens = 2; 
   TPtrC tokens[KMaxNumTokens];
   TLex lex(cmdLinePtr);
   TInt i;
   for (i = 0; !lex.Eos() && i < KMaxNumTokens; i++)
      {
      tokens[i].Set(lex.NextToken());
      }

   TInt numTokens = i;
   if (numTokens == 1)
	   {        
	   DeleteFileL(tokens[0]);    
	   }
   else 
   if (numTokens == 2)
	   {
	   if (tokens[0][0]=='-')
		   {
		   User::LeaveIfError(ParseSwitch(tokens[0].Right(tokens[0].Length()-1),tokens[1]));
		   }
	   else
		   {
		   CopyFileL(tokens[0],tokens[1]);
		   }
	   }

   CleanupStack::PopAndDestroy(cmdLine);	
}

TInt E32Main()
   {    
   CTrapCleanup* cleanup = CTrapCleanup::New();

   TRAPD(err, ::DoMainL());	
   

   delete cleanup;
   
   return(err);
   }

