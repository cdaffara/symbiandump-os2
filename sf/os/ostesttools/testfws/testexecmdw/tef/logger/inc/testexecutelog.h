/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file TestExecuteLog.h
*/

#if !(defined __TESTEXECUTE_LOG_H__)
#define __TESTEXECUTE_LOG_H__

#include <e32base.h>
#include <test/tefexportconst.h>

_LIT8(KTEFOverflowMessage8, "\t..Message Truncated");
_LIT16(KTEFOverflowMessage16, "\t..Message Truncated");
const TInt overflowMessageLen = 21; // Size of the truncation message. To be changed if the text above is changed

/**
 @internalComponent
 @test
 * Derived class implementation of TDes16Overflow class
 * Implements the base class pure virtual Overflow() to handle descriptor overflow
 * during AppendFormatList() operations on TDes16 objects
 */
class TTEFDes16Overflow : public TDes16Overflow
	{
	inline virtual void Overflow(TDes16& aDes)
		{
		TInt actualMessageLen = aDes.Length();
		TInt maxLengthOfDes = aDes.MaxLength();

		TInt midPosForEdit = maxLengthOfDes - overflowMessageLen;
		if (actualMessageLen <= midPosForEdit)
			{
			aDes.Append(KTEFOverflowMessage16());
			}
		else
			{
			aDes.SetLength(midPosForEdit + overflowMessageLen);
			aDes.Replace(midPosForEdit, overflowMessageLen, KTEFOverflowMessage16());
			}
		}
	};

/**
 @internalComponent
 @test
 * Derived class implementation of TDes8Overflow class
 * Implements the base class pure virtual Overflow() to handle descriptor overflow
 * during AppendFormatList() operations on TDes8 objects
 */
class TTEFDes8Overflow : public TDes8Overflow
	{
	inline virtual void Overflow(TDes8& aDes)
		{
		TInt actualMessageLen = aDes.Length();
		TInt maxLengthOfDes = aDes.MaxLength();

		TInt midPosForEdit = maxLengthOfDes - overflowMessageLen;
		if (actualMessageLen <= midPosForEdit)
			{
			aDes.Append(KTEFOverflowMessage8());
			}
		else
			{
			aDes.SetLength(midPosForEdit + overflowMessageLen);
			aDes.Replace(midPosForEdit, overflowMessageLen, KTEFOverflowMessage8());
			}
		}
	};

class RTestExecuteLogServ : public RSessionBase
/**
@internalComponent
@test
*/
	{
public:
	enum TLogMode{ELogModeAppend,ELogModeOverWrite};
	// Logging level
	enum TLogCommand{ECreateLog,EWriteLog};

	IMPORT_C TInt Connect();
	IMPORT_C TInt CreateLog(const TDesC& aLogFilePath,TLogMode aMode);
	IMPORT_C void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,TRefByValue<const TDesC> aFmt,...);
	IMPORT_C void Write(const TDesC& aLogBuffer);
	IMPORT_C void WriteFormat(TRefByValue<const TDesC> aFmt,...);
	IMPORT_C void Write(const TDesC8& aLogBuffer);
	IMPORT_C void WriteFormat(TRefByValue<const TDesC8> aFmt,...);
	IMPORT_C void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,TRefByValue<const TDesC> aFmt, VA_LIST aList);
	IMPORT_C void WriteFormat(TRefByValue<const TDesC> aFmt, VA_LIST aList);
	IMPORT_C void WriteFormat(TRefByValue<const TDesC8> aFmt, VA_LIST aList);
	IMPORT_C void SetLogLevel(TLogSeverity aSeverity);
private:
	void WriteL(const TDesC& aLogBuffer);
	void WriteL(TDes8& aLogBuffer);
	void GetCPPModuleName(TDes& aModuleName, const TText8* aCPPFileName);
	void AddTime(TDes8& aTime);
	TLogSeverity iLogLevel;
	};
#endif
