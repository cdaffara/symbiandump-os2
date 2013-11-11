// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <utf.h>
#include <flogger.h>
#include "FLOGSTD.H"

/**
16-bit tab char width for symbian OS build.
*/
const TText KTabChar='\t';

/**
8-bit FullStop char for non-unicode
*/
const TText8 KFullStopChar8='.';

/**
8-bit tab char for unicode
*/
const TText8 KTabChar8='\t';

/**
Max length for a TBuf date or time variable.
*/
const TInt KDateOrTimeMaxLength=30;

/**
Constructs a 8 bit constant literal descriptor for
end of line.
*/
_LIT8(KEndOfLineCharacters8,"\r\n");

/**
Constructs a 16 bit constant literal descriptor for
end of line.
*/
_LIT(KEndOfLineCharacters,"\r\n");

/**
Date format for European style
*/
_LIT(KDateFormat,"%1%/1%2%/2%3\t");

/**
24 hour time format
*/
_LIT(KTimeFormat,"%J%:1%T%:2%S\t");


/**
TLogFile class definition
*/

TLogFile::TLogFile()
/**
Sets initial values for iValid and iMode.
*/
	{

	iValid=EFalse;
	iMode=EFileLoggingModeUnknown;
	}

TLogFile::TLogFile(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode)
/**
Sets initial values for iValid,iDirectory,iName and iMode.

@param aDir Full Path of the log file.
@param aName Name of the log file.
@param aMode Specifies whether data is appended or file is truncated.
*/
	{

	iValid=EFalse;
	iDirectory=aDir;
	iName=aName;
	iMode=aMode;
	}

TBool TLogFile::operator==(const TLogFile& aLogFile) const
/**
iValid members need not be equal

@param aLogFile log file.
*/
	{

	if (iDirectory!=aLogFile.iDirectory)
		return EFalse;
	if (iName!=aLogFile.iName)
		return EFalse;
	if (iMode!=aLogFile.iMode)
		return EFalse;
	return ETrue;
	}

void TLogFile::Set(const TDesC& aDir, const TDesC& aName, TFileLoggingMode aMode)
/**
Sets values for iValid,iDirectory,iName and iMode.

@param aDir Full Path of the log file.
@param aName Name of the log file.
@param aMode Specifies whether data is appended or file is truncated.
*/
	{

	iValid=EFalse;
	iDirectory=aDir;
	iName=aName;
	iMode=aMode;
	}

/**
TLogFormatterOverflow class definition
*/

void TLogFormatter16Overflow::Overflow(TDes16& /*aDes*/)
/** 
TDes16Overflow pure virtual 
This member is internal and not intended for use.
*/	
	{
	}

void TLogFormatter8Overflow::Overflow(TDes8& /*aDes*/)
/** 
TDes16Overflow pure virtual 
This member is internal and not intended for use.
*/
	{
	}

/**
TLogFormat class definition
*/

TLogFormatter::TLogFormatter()
	: iUseDate(ETrue), iUseTime(ETrue)
/**
Sets iUseDate and iUseTime to ETrue.
*/
	{}

void TLogFormatter::SetDateAndTime(TBool aUseDate, TBool aUseTime)
/**
Sets whether to use date and/or time
*/
	{

	iUseDate=aUseDate;
	iUseTime=aUseTime;
	}

TInt TLogFormatter::FormatTextToWritableBuffer(TDes8& aBuf, const TDesC16& aText) const
/**
Returns result in aBuf
*/
	{

	TRAPD(ret,WriteL(aBuf,aText));
	return ret;
	}

TInt TLogFormatter::FormatTextToWritableBuffer(TDes8& aBuf, const TDesC8& aText) const
/**
Returns result in aBuf
*/
	{

	TRAPD(ret,WriteL(aBuf,aText));
	return ret;
	}

TInt TLogFormatter::ConvertToWritableBuffer(TDes8& aBuf, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/**
Formats string to aBuf
*/
	{

	TBuf16<KLogBufferSize> buf;
	buf.AppendFormatList(aFmt,aList,&iOverflow16);
	TRAPD(ret,WriteL(aBuf,buf));
	return ret;
	}

TInt TLogFormatter::ConvertToWritableBuffer(TDes8& aBuf, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/**
Formats string to aBuf
*/
	{

	TBuf8<KLogBufferSize> buf;
	buf.AppendFormatList(aFmt,aList,&iOverflow8);
	TRAPD(ret,WriteL(aBuf,buf));
	return ret;
	}

void TLogFormatter::GetDateAndTimeL(TDes& aDate, TDes& aTime) const
/**
Gets date and time according to flags to buffer aBuf
*/
	{

	aDate.SetLength(0);
	aTime.SetLength(0);
		
	if (!iUseTime && !iUseDate)
		return;

	TTime t;
	t.HomeTime();
	if (iUseDate)
		t.FormatL(aDate,KDateFormat);
	
	if (iUseTime)
		t.FormatL(aTime,KTimeFormat);
	}

void TLogFormatter::WriteL(TDes8& aTrg, const TDesC16& aSrc) const
/**
Appends date/time as specified and truncate aSrc and convert 
unprintable characters to '.'.  Convert unicode to UTF8 and 
return the result in aTrg
*/
	{	

	aTrg.SetLength(0);
	TBuf16<KDateOrTimeMaxLength> dateBuf;
	TBuf16<KDateOrTimeMaxLength> timeBuf;
	GetDateAndTimeL(dateBuf,timeBuf);
	TBuf16<KLogBufferSize> buf;
	__ASSERT_DEBUG(buf.MaxLength()>=dateBuf.Length(), User::Invariant());
	buf.Append(dateBuf);
	__ASSERT_DEBUG((buf.MaxLength()-buf.Length())>=timeBuf.Length(), User::Invariant());
	buf.Append(timeBuf);
	buf.Append(aSrc.Left(Min(aSrc.Length(),(KLogBufferSize-buf.Length()-2))));	// -2 to allow for CRLF
	TChar ch;
	for (TInt i=0; i<buf.Length(); i++)
		{
		ch=buf[i];
		if(!((ch.IsPrint()) || (ch==KTabChar)))
			buf[i]=KFullStopChar;
		}
	buf.Append(KEndOfLineCharacters);

	TBuf8<KLogBufferSize> utfBuf;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(utfBuf,buf);
	aTrg.Copy(utfBuf.Left(Min(utfBuf.Length(),aTrg.MaxLength())));
	}

void TLogFormatter::WriteL(TDes8& aTrg, const TDesC8& aSrc) const
/**
Append date/time as specified and truncate aSrc and convert 
unprintable characters to '.'.
*/
	{	

	aTrg.SetLength(0);
	TBuf16<KDateOrTimeMaxLength> dateBuf;
	TBuf16<KDateOrTimeMaxLength> timeBuf;
	GetDateAndTimeL(dateBuf,timeBuf);
	TBuf8<KDateOrTimeMaxLength> eightBitDateBuf;
	eightBitDateBuf.Copy(dateBuf);
	TBuf8<KDateOrTimeMaxLength> eightBitTimeBuf;
	eightBitTimeBuf.Copy(timeBuf);
	TBuf8<KLogBufferSize> buf;
	__ASSERT_DEBUG(buf.MaxLength()>=dateBuf.Length(), User::Invariant());
	buf.Append(eightBitDateBuf);
	__ASSERT_DEBUG((buf.MaxLength()-buf.Length())>=timeBuf.Length(), User::Invariant());
	buf.Append(eightBitTimeBuf);
	buf.Append(aSrc.Left(Min(aSrc.Length(),(KLogBufferSize-buf.Length()-2))));	// -2 to allow for CRLF
	TChar ch;
	for (TInt i=0; i<buf.Length(); i++)
		{
		ch=buf[i];
		if(!((ch.IsPrint()) || (ch==KTabChar8)))
			buf[i]=KFullStopChar8;
		}
	buf.Append(KEndOfLineCharacters8);
	aTrg.Copy(buf.Left(Min(buf.Length(),aTrg.MaxLength())));
	}
