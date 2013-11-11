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


#include "slogger.h"

#include <e32base.h>
#include <e32svr.h>
#include <f32file.h>


// Very simple logging code. This will thrash the file server by
// creating a new session to it for every line. Create the file
// c:\logs\ct.txt to turn on logging.
EXPORT_C void SLogger::Log(const TDesC& aLogFileName, const TDesC& aString,
							const TDesC8& aSourceFileName, TInt aLineNumber)
	{
	// Open the file server and file
	RFs fs;
	fs.Connect();
	RFile file;
	TInt error = file.Open(fs, aLogFileName, EFileWrite|EFileShareAny);
	// If the file doesn't exist, exit
	if (error != KErrNone)
		{
		fs.Close();
		return;
		}
	// Seek to the end of the file
	TInt tmp = 0;
	file.Seek(ESeekEnd, tmp);

	// And do some logging
	// Name of the file where the Log function was called
	file.Write(aSourceFileName);
	// Number of the line where the Log function was called
	_LIT8(KLineNumber, ",%d:");
	TBuf8<80> buf;
	buf.Format(KLineNumber, aLineNumber);
	file.Write(buf);
	buf.Copy(aString);
	file.Write(buf);
	_LIT8(KEnd, "\r\n");
	file.Write(KEnd());

	// Close and tidy up
	file.Close();
	fs.Close();
	}
	
EXPORT_C void SLogger::Log(const TDesC& aLogFileName, TInt aInt, 
							const TDesC8& aSourceFileName, TInt aLineNumber)
	{
	TBuf<20> str;
	str.Num(aInt);
	Log(aLogFileName, str, aSourceFileName, aLineNumber);
	}

