/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Print functions used in debug mode 
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef UPSLOG_H
#define UPSLOG_H

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

namespace UserPromptService
{

#ifdef _DEBUG

#define DEBUG_PRINTF(a) {UserPromptService::DebugPrintf(__LINE__, __FILE__, a);}
#define DEBUG_PRINTF2(a, b) {UserPromptService::DebugPrintf(__LINE__, __FILE__, a, b);}
#define DEBUG_PRINTF3(a, b, c) {UserPromptService::DebugPrintf(__LINE__, __FILE__, a, b, c);}
#define DEBUG_PRINTF4(a, b, c, d) {UserPromptService::DebugPrintf(__LINE__, __FILE__, a, b, c, d);}
#define DEBUG_PRINTF5(a, b, c, d, e) {UserPromptService::DebugPrintf(__LINE__, __FILE__, a, b, c, d, e);}

#define DEBUG_CODE_SECTION(a) TRAP_IGNORE({ a; }) 

// UTF-8 overload of the DebufPrintf method. Should be used by default,
// since it's cheaper both in CPU cycles and stack space.

inline void DebugPrintf(TInt aLine, char* aFile, TRefByValue<const TDesC8> aFormat, ...)
	{
	TTruncateOverflowHandler8 overflowHandler8;
	VA_LIST list;
	VA_START(list, aFormat);
	
	TTime now;
	now.HomeTime();
	
	TBuf8<512> buffer;
	_LIT8(KUserPromptServiceLogPrefix, "[UPS] ");
	_LIT8(KUserPromptServiceLineFileFormat, "%Ld Line: % 5d, File: %s -- ");
	buffer.Append(KUserPromptServiceLogPrefix);
	buffer.AppendFormat(KUserPromptServiceLineFileFormat, now.Int64(), aLine, aFile);
	buffer.AppendFormatList(aFormat, list, &overflowHandler8);
	_LIT8(KUserPromptServiceMsgEnd, "\r\n");
	if(buffer.MaxLength() >= (buffer.Length() + KUserPromptServiceMsgEnd().Length()))
		{
		buffer.Append(KUserPromptServiceMsgEnd);
		}
	
	RDebug::RawPrint(buffer);
	
	VA_END(list);
	}
	
// Unicode DebugPrintf overload
inline void DebugPrintf(TInt aLine, char* aFile, TRefByValue<const TDesC16> aFormat, ...)
	{
	TTruncateOverflowHandler16 overflowHandler16;
	VA_LIST list;
	VA_START(list, aFormat);
	
	TTime now;
	now.HomeTime();
	
	TBuf8<256> header;
	_LIT8(KUserPromptServiceLogPrefix, "[UPS] ");
	_LIT8(KUserPromptServiceLineFileFormat, "%Ld Line: % 5d, File: %s -- ");
	header.Append(KUserPromptServiceLogPrefix);
	header.AppendFormat(KUserPromptServiceLineFileFormat, now.Int64(), aLine, aFile);
	
	TBuf<256> buffer;
	buffer.Copy(header);
	buffer.AppendFormatList(aFormat, list , &overflowHandler16);
	_LIT(KUserPromptServiceMsgEnd, "\r\n");
	if(buffer.MaxLength() >= (buffer.Length() + KUserPromptServiceMsgEnd().Length()))
		{
		buffer.Append(KUserPromptServiceMsgEnd);
		}
	
	RDebug::RawPrint(buffer);
	
	VA_END(list);
	}

#else

#define DEBUG_PRINTF(a)
#define DEBUG_PRINTF2(a, b)
#define DEBUG_PRINTF3(a, b, c)
#define DEBUG_PRINTF4(a, b, c, d)
#define DEBUG_PRINTF5(a, b, c, d, e)

#define DEBUG_CODE_SECTION(a)

#endif


} // namespace UserPromptService

#endif // UPSLOG_H
