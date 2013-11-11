/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ct/logger.h"
#include <f32file.h>

#if defined(_DEBUG) && defined(__CT_LOGGING__)

_LIT(KLogFilename, "\\logs\\security\\cryptotokens.txt");	
_LIT(KIndent, "  ");
_LIT8(KNewLine, "\r\n");

#define MAX_LEN 128

static TInt Indent()
	{
	return reinterpret_cast<TInt>(Dll::Tls());
	}

static void SetIndent(TInt aIndent)
	{
	Dll::SetTls(reinterpret_cast<TAny*>(aIndent));
	}

EXPORT_C void CTLogger::Log(TAny* aObject, TRefByValue<const TDesC16> aFmt, ...)
	{
	TBuf<MAX_LEN> data;

	data.AppendFormat(_L("%08x: "), aObject);

	for (TInt i = 0 ; i < Indent() ; ++i)
		{
		data.Append(KIndent);
		}

	VA_LIST args;
	VA_START(args, aFmt);
	data.AppendFormatList(aFmt, args);
	VA_END(args);

	TRAPD(err, LogL(data));
	ASSERT(err == KErrNone);
	}

EXPORT_C void CTLogger::UpdateIndent(TInt aInc)
	{
	TInt newIndent = Indent() + aInc;
	ASSERT(newIndent >= 0);
	SetIndent(newIndent);
	}

void CTLogger::LogL(const TDesC& aString)
	{
	// Open the file server and file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	// Open the file or create it if doesn't exist, create it
	RFile file;
	TDriveUnit sysDrive (fs.GetSystemDrive());
	TBuf<128> logFile (sysDrive.Name());
	logFile.Append(KLogFilename);
	
	TInt error = file.Open(fs, logFile, EFileWrite|EFileShareAny);
	if (error == KErrNotFound)
		{
		error = file.Create(fs, logFile, EFileWrite|EFileShareAny);
		}
	User::LeaveIfError(error);
	CleanupClosePushL(file);
	
	// Seek to the end of the file
	TInt tmp = 0;
	file.Seek(ESeekEnd, tmp);

	// And do some logging
	TBuf8<MAX_LEN> buf;
	buf.Copy(aString);
	file.Write(buf);
	file.Write(KNewLine);

	// Close and tidy up
	CleanupStack::PopAndDestroy(2, &fs);
	}

#else

EXPORT_C void CTLogger::Log(TAny* /*aObject*/, TRefByValue<const TDesC16> /*aFmt*/, ...)
	{
	User::Invariant();
	}

EXPORT_C void CTLogger::UpdateIndent(TInt /*aInc*/)
	{
	User::Invariant();
	}

#endif
