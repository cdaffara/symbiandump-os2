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
* UTF8STR.CPP
* This file contains the implementation of the UTF8 String ASN1 class.
* The IA5 string can contain the following characters:
* \<character set unknown pending arrival of ITU spec\>
*
*/


/**
 @file
*/

#include <asn1dec.h>

TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8);

EXPORT_C TASN1DecUTF8String::TASN1DecUTF8String(void)
	{
	}

EXPORT_C HBufC* TASN1DecUTF8String::DecodeDERL(const TASN1DecGeneric& aSource)
	{
	return DecodeContentsL(aSource.GetContentDER());
	}

EXPORT_C HBufC* TASN1DecUTF8String::DecodeDERL(const TDesC8& aSource,TInt& aPos)

	{
	TPtrC8 Source=aSource.Mid(aPos);
	TASN1DecGeneric gen(Source);
	gen.InitL();
	HBufC* res = DecodeContentsL(gen.GetContentDER());
	aPos+=gen.LengthDER();
	return res;
	}

HBufC* TASN1DecUTF8String::DecodeContentsL(const TDesC8& aSource)
	{
	HBufC* res = HBufC::NewLC(aSource.Length());
	TPtr pRes = res->Des();
	User::LeaveIfError(ConvertToUnicodeFromUtf8(pRes, aSource));
	CleanupStack::Pop(res);
	return res;
	}

/**
 * Converts text encoded using the Unicode transformation format UTF-8
 * into the Unicode UCS-2 character set.
 *
 * @param aUnicode	On return, contains the Unicode encoded output string.
 * @param aUtf8		The UTF-8 encoded input string
 * @return			The number of unconverted bytes left at the end of the
 *					input descriptor, or one of the error values defined
 *					in <code>TError</code>.
 */
TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8)
	{
	if (aUtf8.Length()==0)
		{
		aUnicode.SetLength(0);
		return 0;
		}
	if (aUnicode.MaxLength()==0)
		{
		return aUtf8.Length();
		}
	TUint16* pointerToCurrentUnicodeCharacter=CONST_CAST(TUint16*, aUnicode.Ptr());
	const TUint16* pointerToLastUnicodeCharacter=pointerToCurrentUnicodeCharacter+(aUnicode.MaxLength()-1);
	const TUint8* pointerToCurrentUtf8Byte=aUtf8.Ptr();
	const TUint8* pointerToLastUtf8Byte=pointerToCurrentUtf8Byte+(aUtf8.Length()-1);
	TBool inputIsTruncated=EFalse;
	TUint16 replacementcharacter = 0xFFFD;
	FOREVER
		{
		//__ASSERT_DEBUG(pointerToCurrentUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicBadUnicodePointers8));
		//__ASSERT_DEBUG(pointerToCurrentUtf8Byte<=pointerToLastUtf8Byte, Panic(EPanicBadUtf8Pointers3));
		TUint currentUtf8Byte=*pointerToCurrentUtf8Byte;
		if ((currentUtf8Byte&0x80)==0x00)
			{
			*pointerToCurrentUnicodeCharacter=STATIC_CAST(TUint16, currentUtf8Byte);
			}
		else if ((currentUtf8Byte&0xe0)==0xc0)
			{
			//__ASSERT_DEBUG(pointerToCurrentUtf8Byte<=pointerToLastUtf8Byte, Panic(EPanicBadUtf8Pointers4));
			if (pointerToCurrentUtf8Byte>=pointerToLastUtf8Byte)
				{
				--pointerToCurrentUnicodeCharacter;
				--pointerToCurrentUtf8Byte;
				inputIsTruncated=ETrue;
				break;
				}
			TUint currentUnicodeCharacter=((currentUtf8Byte&0x1f)<<6);
			++pointerToCurrentUtf8Byte;
			currentUtf8Byte=*pointerToCurrentUtf8Byte;
			if ((currentUtf8Byte&0xc0)==0x80)
				{
				currentUnicodeCharacter|=(currentUtf8Byte&0x3f);
				*pointerToCurrentUnicodeCharacter=STATIC_CAST(TUint16, currentUnicodeCharacter);
				}
			else
				{
				*pointerToCurrentUnicodeCharacter=replacementcharacter;
				}
			}
		else if ((currentUtf8Byte&0xf0)==0xe0)
			{
			//__ASSERT_DEBUG(pointerToCurrentUtf8Byte<=pointerToLastUtf8Byte, Panic(EPanicBadUtf8Pointers5));
			if (pointerToLastUtf8Byte-pointerToCurrentUtf8Byte<2)
				{
				--pointerToCurrentUnicodeCharacter;
				--pointerToCurrentUtf8Byte;
				inputIsTruncated=ETrue;
				break;
				}
			TUint currentUnicodeCharacter=((currentUtf8Byte&0x0f)<<12);
			++pointerToCurrentUtf8Byte;
			currentUtf8Byte=*pointerToCurrentUtf8Byte;
			if ((currentUtf8Byte&0xc0)==0x80)
				{
				currentUnicodeCharacter|=((currentUtf8Byte&0x3f)<<6);
				++pointerToCurrentUtf8Byte;
				currentUtf8Byte=*pointerToCurrentUtf8Byte;
				if ((currentUtf8Byte&0xc0)==0x80)
					{
					currentUnicodeCharacter|=(currentUtf8Byte&0x3f);
					*pointerToCurrentUnicodeCharacter=STATIC_CAST(TUint16, currentUnicodeCharacter);
					}
				else
					{
					*pointerToCurrentUnicodeCharacter=STATIC_CAST(TUint16, currentUnicodeCharacter);
					}
				}
			else
				{
				*pointerToCurrentUnicodeCharacter=replacementcharacter;
				}
			
			}
		else if ((currentUtf8Byte&0xf8)==0xf0)
			{
			//__ASSERT_DEBUG(pointerToCurrentUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicBadUnicodePointers9));
			if (pointerToCurrentUnicodeCharacter>=pointerToLastUnicodeCharacter)
				{
				--pointerToCurrentUnicodeCharacter;
				--pointerToCurrentUtf8Byte;
				break;
				}
			//__ASSERT_DEBUG(pointerToCurrentUtf8Byte<=pointerToLastUtf8Byte, Panic(EPanicBadUtf8Pointers6));
			if (pointerToLastUtf8Byte-pointerToCurrentUtf8Byte<3)
				{
				--pointerToCurrentUnicodeCharacter;
				--pointerToCurrentUtf8Byte;
				inputIsTruncated=ETrue;
				break;
				}
			TUint currentUnicodeCharacter=((currentUtf8Byte&0x07)<<8);
			++pointerToCurrentUtf8Byte;
			currentUtf8Byte=*pointerToCurrentUtf8Byte;
			if ((currentUtf8Byte&0xc0)==0x80)
				{
				currentUnicodeCharacter|=((currentUtf8Byte&0x3f)<<2);
				if (currentUnicodeCharacter>=0x0040)
					{
					currentUnicodeCharacter-=0x0040;
					if (currentUnicodeCharacter<0x0400)
						{
						++pointerToCurrentUtf8Byte;
						currentUtf8Byte=*pointerToCurrentUtf8Byte;
						if ((currentUtf8Byte&0xc0)==0x80)
							{
							currentUnicodeCharacter|=((currentUtf8Byte&0x30)>>4);
							*pointerToCurrentUnicodeCharacter=STATIC_CAST(TUint16, 0xd800|currentUnicodeCharacter);
							currentUnicodeCharacter=((currentUtf8Byte&0x0f)<<6);
							++pointerToCurrentUtf8Byte;
							currentUtf8Byte=*pointerToCurrentUtf8Byte;
							if ((currentUtf8Byte&0xc0)==0x80)
								{
								currentUnicodeCharacter|=(currentUtf8Byte&0x3f);
								++pointerToCurrentUnicodeCharacter;
								*pointerToCurrentUnicodeCharacter=STATIC_CAST(TUint16, 0xdc00|currentUnicodeCharacter);
								}
							}
						}
					}
				}
			}
		else
			{
			*pointerToCurrentUnicodeCharacter=replacementcharacter;
			}
		//__ASSERT_DEBUG(pointerToCurrentUtf8Byte<=pointerToLastUtf8Byte, Panic(EPanicBadUtf8Pointers7));
		//__ASSERT_DEBUG(pointerToCurrentUnicodeCharacter<=pointerToLastUnicodeCharacter, Panic(EPanicBadUnicodePointers10));
		if ((pointerToCurrentUtf8Byte>=pointerToLastUtf8Byte) || (pointerToCurrentUnicodeCharacter>=pointerToLastUnicodeCharacter))
			{
			break;
			}
		++pointerToCurrentUnicodeCharacter;
		++pointerToCurrentUtf8Byte;
		}
	if ((pointerToCurrentUtf8Byte<aUtf8.Ptr()) && inputIsTruncated)
		{
		return KErrArgument;
		}
	aUnicode.SetLength((pointerToCurrentUnicodeCharacter-aUnicode.Ptr())+1);
	return pointerToLastUtf8Byte-pointerToCurrentUtf8Byte;
	}

