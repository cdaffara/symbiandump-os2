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
//

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif

_LIT(KLogFileName, "C:\\MsgLogs\\T_GMXMLFAILURE");
_LIT8(KLogErrorString, "** %S %d");
_LIT8(KLogCommentString, " %S");
_LIT8(KLogCR, "\r\n");
_LIT(KLogDateFormatString, "%D%M%*Y%1%/1%2%/2%3 %H%:1%T%:2%S");


// Setting to select whether a new log file will be created, 
// or existing one will be appended to...

/**
@internalTechnology
@released
*/
enum TImLogOpenMode
	{ 
	EReplace, 
	EAppend
	};

class CImLog : public CBase
/**
@internalTechnology
@released
*/
	{
public:
	static CImLog* NewL(const TDesC& aFullFilePath, TImLogOpenMode aImLogOpenMode);
	~CImLog();
	
	void AppendComment(const TDesC8& aMessage);
	void AppendError(const TDesC8& aErrorMessage, TInt aErrorCode);
private:
	CImLog();
	void ConstructL(const TDesC& aFullFilePath, TImLogOpenMode aImLogOpenMode);

	void BuildLogString(const TDesC8& aFormat,const TDesC8& aOutPut, TBool aWriteCrLf = EFalse);
	void WriteToLog(TRefByValue<const TDesC8> aFmt,...);
private:
	RFile	iFile;
	RFs		iFs;
	};

