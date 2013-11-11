/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <stdlib.h>

const int KErrorIllFormedInput=-1;

int Utf8ToUnicode(wchar_t* aUnicode, const char* aUtf8)
// must '\0'-terminate the output
	{
	wchar_t* startOfUnicode=aUnicode;
	for (;;)
		{
		unsigned int currentUtf8Byte=*aUtf8;
		if (currentUtf8Byte=='\0')
			{
			break;
			}
		if ((currentUtf8Byte&0x80)==0x00)
			{
			if (startOfUnicode!=NULL)
				{
				*aUnicode=(wchar_t)currentUtf8Byte;
				}
			}
		else if ((currentUtf8Byte&0xe0)==0xc0)
			{
			unsigned int currentUnicodeCharacter=((currentUtf8Byte&0x1f)<<6);
			++aUtf8;
			currentUtf8Byte=*aUtf8;
			if ((currentUtf8Byte&0xc0)!=0x80)
				{
				return KErrorIllFormedInput;
				}
			currentUnicodeCharacter|=(currentUtf8Byte&0x3f);
			if (startOfUnicode!=NULL)
				{
				*aUnicode=(wchar_t)currentUnicodeCharacter;
				}
			}
		else if ((currentUtf8Byte&0xf0)==0xe0)
			{
			unsigned int currentUnicodeCharacter=((currentUtf8Byte&0x0f)<<12);
			++aUtf8;
			currentUtf8Byte=*aUtf8;
			if ((currentUtf8Byte&0xc0)!=0x80)
				{
				return KErrorIllFormedInput;
				}
			currentUnicodeCharacter|=((currentUtf8Byte&0x3f)<<6);
			++aUtf8;
			currentUtf8Byte=*aUtf8;
			if ((currentUtf8Byte&0xc0)!=0x80)
				{
				return KErrorIllFormedInput;
				}
			currentUnicodeCharacter|=(currentUtf8Byte&0x3f);
			if (startOfUnicode!=NULL)
				{
				*aUnicode=(wchar_t)currentUnicodeCharacter;
				}
			}
		else if ((currentUtf8Byte&0xf8)==0xf0)
			{
			unsigned int currentUnicodeCharacter=((currentUtf8Byte&0x07)<<8);
			++aUtf8;
			currentUtf8Byte=*aUtf8;
			if ((currentUtf8Byte&0xc0)!=0x80)
				{
				return KErrorIllFormedInput;
				}
			currentUnicodeCharacter|=((currentUtf8Byte&0x3f)<<2);
			if (currentUnicodeCharacter<0x0040)
				{
				return KErrorIllFormedInput;
				}
			currentUnicodeCharacter-=0x0040;
			if (currentUnicodeCharacter>=0x0400)
				{
				return KErrorIllFormedInput;
				}
			++aUtf8;
			currentUtf8Byte=*aUtf8;
			if ((currentUtf8Byte&0xc0)!=0x80)
				{
				return KErrorIllFormedInput;
				}
			currentUnicodeCharacter|=((currentUtf8Byte&0x30)>>4);
			if (startOfUnicode!=NULL)
				{
				*aUnicode=(wchar_t)(0xd800|currentUnicodeCharacter);
				}
			currentUnicodeCharacter=((currentUtf8Byte&0x0f)<<6);
			++aUtf8;
			currentUtf8Byte=*aUtf8;
			if ((currentUtf8Byte&0xc0)!=0x80)
				{
				return KErrorIllFormedInput;
				}
			currentUnicodeCharacter|=(currentUtf8Byte&0x3f);
			++aUnicode;
			if (startOfUnicode!=NULL)
				{
				*aUnicode=(wchar_t)(0xdc00|currentUnicodeCharacter);
				}
			}
		else
			{
			return KErrorIllFormedInput;
			}
		++aUnicode;
		++aUtf8;
		}
	if (startOfUnicode!=NULL)
		{
		*aUnicode='\0';
		}
	return aUnicode-startOfUnicode;
	}
#include <stdio.h>
int UnicodeToUtf8(char* aUtf8, const wchar_t* aUnicode)
// must '\0'-terminate the output
	{
	char* startOfUtf8=aUtf8;
	for (;;)
		{
		unsigned int currentUnicodeCharacter=*aUnicode;
		if (currentUnicodeCharacter=='\0')
			{
			break;
			}
		if ((currentUnicodeCharacter&0xff80)==0x0000)
			{
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)currentUnicodeCharacter;
				}
			}
		else if ((currentUnicodeCharacter&0xf800)==0x0000)
			{
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)(0xc0|(currentUnicodeCharacter>>6));
				}
			++aUtf8;
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)(0x80|(currentUnicodeCharacter&0x3f));
				}
			}
		else if ((currentUnicodeCharacter&0xfc00)==0xd800)
			{
			currentUnicodeCharacter+=0x0040;
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)(0xf0|((currentUnicodeCharacter>>8)&0x07));
				}
			++aUtf8;
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)(0x80|((currentUnicodeCharacter>>2)&0x3f));
				}
			{
			unsigned int currentUtf8Byte=(0x80|((currentUnicodeCharacter&0x03)<<4));
			++aUnicode;
			currentUnicodeCharacter=*aUnicode;
			if ((currentUnicodeCharacter&0xfc00)!=0xdc00)
				{
				return KErrorIllFormedInput;
				}
			currentUtf8Byte|=((currentUnicodeCharacter>>6)&0x0f);
			++aUtf8;
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)currentUtf8Byte;
				}
			}
			++aUtf8;
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)(0x80|(currentUnicodeCharacter&0x3f));
				}
			}
		else
			{
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)(0xe0|(currentUnicodeCharacter>>12));
				}
			++aUtf8;
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)(0x80|((currentUnicodeCharacter>>6)&0x3f));
				}
			++aUtf8;
			if (startOfUtf8!=NULL)
				{
				*aUtf8=(char)(0x80|(currentUnicodeCharacter&0x3f));
				}
			}
		++aUtf8;
		++aUnicode;
		}
	if (startOfUtf8!=NULL)
		{
		*aUtf8='\0';
		}
	return aUtf8-startOfUtf8;
	}


