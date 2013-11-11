/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_output.h"
#include <securityerr.h>

// Size used for stack based buffers
const static TInt KMaxLineLength = 200;

_LIT(KNewLine, "\r\n");
	
// Output //////////////////////////////////////////////////////////////////////

EXPORT_C void Output::writeString(const TDesC& aString)
	{
	FixNewlinesAndWriteL(aString);
	}

EXPORT_C void Output::writeString(const TDesC8& aString)
	{
	HBufC16* des = HBufC16::NewLC(aString.Length());
	des->Des().Copy(aString);
	FixNewlinesAndWriteL(*des);
	CleanupStack::PopAndDestroy(des);
	}

EXPORT_C void Output::write(TRefByValue<const TDesC16> aFmt, ...)
	{
	TBuf<KMaxLineLength> buf;
	VA_LIST args;
	VA_START(args, aFmt);
	buf.AppendFormatList(aFmt, args);
	VA_END(args);

	FixNewlinesAndWriteL(buf);
	}

EXPORT_C void Output::writeSpaces(TInt aNum)
	{
	for (TInt i = 0; i < aNum; i++)
		{
		DoWriteL(_L(" "));
		}
	}

EXPORT_C void Output::writeNewLine()
	{
	DoWriteL(KNewLine);
	}

EXPORT_C void Output::writeNum(TInt aNum)
	{
	write(_L("%d"), aNum);
	}

EXPORT_C void Output::writeHex(TInt aHex)
	{
	write(_L("%x"), aHex);	
	}

EXPORT_C void Output::writeError(TInt aError)
	{
	switch (aError)
		{
	case KErrNone:	// 0
		DoWriteL(_L("KErrNone"));
		break;

	case KErrNotFound:	// -1
		DoWriteL(_L("KErrNotFound"));
		break;

	case KErrNotSupported:	// -5
		DoWriteL(_L("KErrNotSupported"));
		break;

	case KErrInUse:	// -14
		DoWriteL(_L("KErrInUse"));
		break;

	case KErrNotReady:	// -18
		DoWriteL(_L("KErrNotReady"));
		break;

	case KRequestPending:
		DoWriteL(_L("KRequestPending"));
		break;

	case KErrAlreadyExists:
		DoWriteL(_L("KErrAlreadyExists"));
		break;

	case KErrArgument:
		DoWriteL(_L("KErrArgument"));
		break;

	case KErrBadName: // -28
		DoWriteL(_L("KErrBadName"));
		break;
	case KErrPrivateKeyNotFound:
		writeString(_L("KErrPrivateKeyNotFound"));
		break;

	default:
		writeNum(aError);
		break;
		}
	}

EXPORT_C void Output::writeOctetString(const TDesC8& aString)
	{
	writeOctetStringL(aString);
	}

EXPORT_C void Output::writeOctetStringL(const TDesC8& aString)
	{
    TInt len = aString.Length();
	HBufC* buf = HBufC::NewLC(len * 3);
	TPtr pBuf = buf->Des();
    for (TInt index = 0; index < len; ++index)
		{
		pBuf.AppendFormat(_L("%02x "),aString[index]);
		}
	DoWriteL(*buf);
	CleanupStack::PopAndDestroy(buf);
	}

EXPORT_C void Output::writeBoolL(TBool aBool)
	{
	if (aBool)
		{
		DoWriteL(_L("ETrue"));
		}
	else
		{
		DoWriteL(_L("EFalse"));
		}
	}

// Fix up newlines by turning any occurences of just "\n" into "\r\n".
void Output::FixNewlinesAndWriteL(const TDesC& aString)
	{
	TPtrC remainder(aString);
	TInt index = 0;
	while (remainder.Length())
		{
		index = remainder.Locate('\n');

		if (index == KErrNotFound)
			{
			DoWriteL(remainder);
			break;
			}

		if (index == 0)
			{
			DoWriteL(KNewLine);
			}
		else 
			{
			if (remainder[index - 1] == '\r')
				{
				DoWriteL(remainder.Left(index + 1));
				}
			else
				{
				DoWriteL(remainder.Left(index));
				DoWriteL(KNewLine);					 
				}
			}
		remainder.Set(remainder.Mid(index + 1));
		}
	}

// writeCapabilityL, writeCapabilitySetL, writeSecurityPolicyL implemented in
// t_capability.cpp

// NullOutput //////////////////////////////////////////////////////////////////

EXPORT_C NullOutput::NullOutput()
	{
	}

void NullOutput::DoWriteL(const TDesC& /*aString*/)
	{
	}

// FileOutput //////////////////////////////////////////////////////////////////

EXPORT_C FileOutput::FileOutput(RFile& aFile) :
	iFile(aFile)
	{
	}

void FileOutput::DoWriteL(const TDesC& aString)
	{
	TBuf8<KMaxLineLength> buf;
	buf.Copy(aString.Left(KMaxLineLength));
	User::LeaveIfError(iFile.Write(buf));
	User::LeaveIfError(iFile.Flush()); // Commit data
	}

// ConsoleOutput ///////////////////////////////////////////////////////////////

EXPORT_C ConsoleOutput::ConsoleOutput(CConsoleBase& aConsole) :
	iConsole(aConsole)
	{
	}

void ConsoleOutput::DoWriteL(const TDesC& aString)
	{
	iConsole.Printf(aString);
	}

// COutputTee //////////////////////////////////////////////////////////////////

COutputTee::COutputTee()
	{
	}

COutputTee::~COutputTee()
	{
	iChildren.ResetAndDestroy();
	}

void COutputTee::AddChildL(Output* aChild)
	{
	User::LeaveIfError(iChildren.Append(aChild));
	}

void COutputTee::DoWriteL(const TDesC& aString)
	{
	for (TInt i = 0 ; i < iChildren.Count() ; ++i)
		{
		iChildren[i]->writeString(aString);
		}
	}

