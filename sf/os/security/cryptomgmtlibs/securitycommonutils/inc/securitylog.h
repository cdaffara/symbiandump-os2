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
* Implements common print functions used in debug mode for all security components.
* A component that wants to use these common debugging functions should define its
* own log header file and include this header file. In the component's header file,
* the followings should be defined:
* _LIT8(KComponentName, "[TheComponentName]");
*
*/


/**
 @file
 @internalTechnology
 @released
*/
 
#ifndef SECURITYLOG_H
#define SECURITYLOG_H

#include <e32debug.h>


class TTruncateOverflowHandler16 : public TDes16Overflow
	{
	public:
		virtual void Overflow(TDes16& aDes);
	};
	
inline void TTruncateOverflowHandler16::Overflow(TDes16&)
	{

	}
	
class TTruncateOverflowHandler8 : public TDes8Overflow
	{
	public:
		virtual void Overflow(TDes8& aDes);
	};
	
inline void TTruncateOverflowHandler8::Overflow(TDes8&)
	{
   
	}

#ifdef _DEBUG

const TInt KMaxLogBufferSize8   = 512;
const TInt KMaxLogBufferSize16  = 256;

#define SEC_DEBUG_PRINTF(x, a) {::CommonDebugPrintf(x, __LINE__, __FILE__, a);}
#define SEC_DEBUG_PRINTF2(x, a, b) {::CommonDebugPrintf(x, __LINE__, __FILE__, a, b);}
#define SEC_DEBUG_PRINTF3(x, a, b, c) {::CommonDebugPrintf(x, __LINE__, __FILE__, a, b, c);}
#define SEC_DEBUG_PRINTF4(x, a, b, c, d) {::CommonDebugPrintf(x, __LINE__, __FILE__, a, b, c, d);}
#define SEC_DEBUG_PRINTF5(x, a, b, c, d, e) {::CommonDebugPrintf(x, __LINE__, __FILE__, a, b, c, d, e);}

#define SEC_DEBUG_CODE_SECTION(a) TRAP_IGNORE({ a; }) 

inline void FileFormatLine(TDes8& aBuffer, const TDesC8& aComponent, TInt aLine, char* aFile)
	{
	TTime now;
	now.HomeTime();
	
	_LIT8(KLineFileFormat, "%Ld Line: % 5d, File: %s -- ");
	aBuffer.Append(aComponent);
	aBuffer.AppendFormat(KLineFileFormat, now.Int64(), aLine, aFile);
	}

// UTF-8 overload of the CoomonDebufPrintf method. Should be used by default,
// since it's cheaper both in CPU cycles and stack space.
inline void CommonDebugPrintf(const TDesC8& aComponent, TInt aLine, char* aFile, TRefByValue<const TDesC8> aFormat, ...)
	{
	TTruncateOverflowHandler8 overflowHandler8;
	VA_LIST list;
	VA_START(list, aFormat);
	
	TBuf8<KMaxLogBufferSize8> buffer;
	FileFormatLine(buffer, aComponent, aLine, aFile);
	
	buffer.AppendFormatList(aFormat, list, &overflowHandler8);
	_LIT8(KLogMsgEnd, "\r\n");
	if(buffer.MaxLength() >= (buffer.Length() + KLogMsgEnd().Length()))
		{
		buffer.Append(KLogMsgEnd);
		}
	
	RDebug::RawPrint(buffer);
	
	VA_END(list);
	}
	
// Unicode CommonDebufPrintf overload
inline void CommonDebugPrintf(const TDesC8& aComponent, TInt aLine, char* aFile, TRefByValue<const TDesC16> aFormat, ...)
	{
	TTruncateOverflowHandler16 overflowHandler16;
	VA_LIST list;
	VA_START(list, aFormat);
	
	TBuf8<KMaxLogBufferSize16> header;
	FileFormatLine(header, aComponent, aLine, aFile);
	
	TBuf<KMaxLogBufferSize16> buffer;
	buffer.Copy(header);
	buffer.AppendFormatList(aFormat, list , &overflowHandler16);
	
	_LIT(KLogMsgEnd, "\r\n");
	if(buffer.MaxLength() >= (buffer.Length() + KLogMsgEnd().Length()))
		{
		buffer.Append(KLogMsgEnd);
		}
	
	RDebug::RawPrint(buffer);
	
	VA_END(list);
	}

#else

#define SEC_DEBUG_PRINTF(x, a)
#define SEC_DEBUG_PRINTF2(x, a, b)
#define SEC_DEBUG_PRINTF3(x, a, b, c)
#define SEC_DEBUG_PRINTF4(x, a, b, c, d)
#define SEC_DEBUG_PRINTF5(x, a, b, c, d, e)

#define SEC_DEBUG_CODE_SECTION(a)

#endif  // _DEBUG

#endif // SECURITYLOG_H
