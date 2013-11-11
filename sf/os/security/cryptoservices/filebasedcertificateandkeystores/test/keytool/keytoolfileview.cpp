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
*
*/


#include "keytool_defs.h"
#include "keytoolfileview.h"

#include <e32cons.h>
#include "keytool_utils.h"
#include <s32file.h>


/*static*/ CKeytoolFileView* CKeytoolFileView::NewLC(const TDesC& aInputFile)
	{
	CKeytoolFileView* self = new (ELeave) CKeytoolFileView();
	CleanupStack::PushL(self);
	self->ConstructL(aInputFile);
	return self;
	}

CKeytoolFileView::CKeytoolFileView()
	{	
	}
	
CKeytoolFileView::~CKeytoolFileView()
	{
	iFs.Close();
	iFile.Close();
	iArgs.ResetAndDestroy();
	}
	
void CKeytoolFileView::ConstructL(const TDesC& aInputFile)
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFile.Open(iFs, aInputFile, EFileShareExclusive));
	}

void CKeytoolFileView::DisplayUsage()
	{	
	}
	
void CKeytoolFileView::BoilerPlate()
	{
	
	}
	
void CKeytoolFileView::DisplayKeyInfoL(CCTKeyInfo& aKey, TBool aIsDetailed, TBool /*aPageWise*/)
	{
	// Display the key infos
	KeyToolUtils::PrintKeyInfoL(aKey, aIsDetailed);
 	KeyToolUtils::PrintInfoL(_L("\n"));
 	}
	
void CKeytoolFileView::DisplayErrorL(const TDesC& aError, TInt aErrorCode, TBool/* aPageWise*/)
	{
	KeyToolUtils::PrintInfoL(aError);
	KeyToolUtils::PrintInfoL(_L("\nError code: "));
	KeyToolUtils::WriteErrorL(aErrorCode);
	KeyToolUtils::PrintInfoL(_L("\n"));
	}
	 	
 void CKeytoolFileView::DisplayErrorL(const TDesC& aError, TBool/* aPageWise*/)
	{
	KeyToolUtils::PrintInfoL(aError);
	KeyToolUtils::PrintInfoL(_L("\n"));
	}
	
 void CKeytoolFileView::DisplayCertL(CCTCertInfo& aCert, CCertificate& aCertificate, RUidArray aApps, TBool aTrusted, TBool aIsDetailed, TBool/* aPageWise*/)
 	{
	// Display the key infos
	KeyToolUtils::PrintCertInfoL(aCert, aCertificate, aApps, aTrusted, aIsDetailed);
 	KeyToolUtils::PrintInfoL(_L("\n")); 	
 	}

CArrayFixFlat<TPtrC>* CKeytoolFileView::ReadArrayArgumentsLC(TInt cmdIndex)
	{
	CArrayFixFlat<TPtrC>* currentCmd = new (ELeave) CArrayFixFlat<TPtrC> (10);
	CleanupStack::PushL(currentCmd);
	TPtr cmd = iArgs[cmdIndex]->Des();
	cmd.Trim();
	TLex arguments(cmd);
	while(!arguments.Eos())
    	{
        TPtrC token = arguments.NextToken();
        currentCmd->AppendL(token);
        }
    
    TBuf<150> string;
	string.Format(_L("Command :  %S\noutput :"), &cmd);
	KeyToolUtils::PrintInfoL(_L("================================================"));
	KeyToolUtils::PrintInfoL(_L("================================================\n"));
    DisplayErrorL(string, 0);
    return currentCmd;
	}

TInt CKeytoolFileView::SplitFileInputToArrayL()
	{
	TInt fSize;
	iFile.Size(fSize);

	HBufC8* fileContents = HBufC8::NewLC(fSize);
	TPtr8 ptr(fileContents->Des());
	ptr.SetLength(fSize);
	
	// create file stream and Read the content from the file
	RFileReadStream inputFileStream(iFile);
    CleanupClosePushL(inputFileStream);
	inputFileStream.ReadL(ptr, fSize);
	CleanupStack::PopAndDestroy(&inputFileStream);
	
	TInt readPos = 0;
	TPtrC8 lineContents;

	TInt lineCount = 0;
	while (!ReadLine(*fileContents, readPos, lineContents))
		{
		TInt lineLength = lineContents.Length();
		if (lineLength)
			{
			lineCount++;
			HBufC* currentLine = HBufC::NewLC(lineLength);
			currentLine->Des().Copy(lineContents);
			iArgs.AppendL(currentLine);
			CleanupStack::Pop(currentLine);
			}
		}
	CleanupStack::PopAndDestroy(fileContents);
	return lineCount;
	}
	
TInt CKeytoolFileView::ReadLine(const TDesC8& aBuffer, TInt& aPos, TPtrC8& aLine)
	{
	TBool endOfBuffer = EFalse;
	const TChar KCarriageReturn = '\r';
	const TChar KLineReturn = '\n';

	TInt bufferLength = aBuffer.Length();
	if ( aPos > bufferLength || aPos < 0 )
		{	
		return ETrue; // End of buffer
		}
		
	// find the position of the next delimter		
	TInt endPos = aPos;	
	while (endPos < bufferLength)
		{
		TChar c = aBuffer[endPos];

		if (c == KCarriageReturn || c == KLineReturn) 
			{
			break;
			}	
		endPos++;
		}

	if (endPos != aPos)	
		{
		TInt tokenLen = endPos - aPos;
		aLine.Set(&aBuffer[aPos], tokenLen);
		endPos += 2;
		}
	else
		{
		return ETrue; // End of buffer
		}

	aPos = endPos;
	return endOfBuffer;
	}
	
	
