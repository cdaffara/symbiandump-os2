// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : MRT_WCHARCNVT.CPP
// Part of     : MRT LIBC
// Contains the source for the helper functions used by wchar 
// restartable conversion API's in libc
// Version     : 1.0
//



// Copyright (c) 1997-2003 Symbian Ltd.  All rights reserved.

// system includes
#include <e32std.h>
#include <e32base.h>
#include <utf.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wchar.h>

#include "wcharcnv.h"

#define  KSURROGATE_OFFSET 0x10000 - (0xD800 << 10) - 0xDC00

//-----------------------------------------------------------------------------
//Function Name : TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const 
//                           TDesC8& aUtf8, mbstate_t *state)
//Description   : Converts the unicode to UTF8 
//Return Value  : The number of unconverted bytes left at the end of the input
//descriptor, or one of the error values defined in TError.
//-----------------------------------------------------------------------------
TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8, mbstate_t *state)
{
	aUnicode.SetLength(0);
	if (aUtf8.Length()==0)
	{
		return 0;
	}
	if (aUnicode.MaxLength()==0)
	{
		return aUtf8.Length();
	}
	
	HBufC8* utf8 = NULL;
	if ( state->__count > 0)
        {
	        // state have some information, use that.
	        utf8 = HBufC8::NewLC ( state->__count + aUtf8.Length() );
	        TPtr8 tempBuf = utf8->Des();
	        TPtr8 temp ((TUint8*)state->__value.__wchb, state->__count);
	        tempBuf.Copy(temp);
	        tempBuf.Append(aUtf8);
        }
    
	TUint16* pointerToCurrentUnicodeCharacter=CONST_CAST(TUint16*, aUnicode.Ptr());
	const TUint16* pointerToLastUnicodeCharacter=pointerToCurrentUnicodeCharacter+(aUnicode.MaxLength()-1);
	const TUint8* pointerToCurrentUtf8Byte= utf8 ? utf8->Des().Ptr() : aUtf8.Ptr();
	const TUint8* pointerToPendingUtf8Byte=utf8 ? utf8->Des().Ptr() : aUtf8.Ptr();
	TInt length = utf8 ? utf8->Des().Length() : aUtf8.Length();
	const TUint8* pointerToLastUtf8Byte=pointerToCurrentUtf8Byte+(length-1);
	TUint16 replacementcharacter = 0xFFFD;
	TUint8 currentUtf8Byte;
	TUint currentUnicodeCharacter;
	TInt sequenceLength;		
	
	
	FOREVER
	{
		currentUtf8Byte=*pointerToCurrentUtf8Byte;
		pointerToPendingUtf8Byte = pointerToCurrentUtf8Byte;
		sequenceLength=100;
        
		for(TInt i=0;i<7;i++)
		{
			if ((currentUtf8Byte&(0xf8<<i))==(static_cast<TUint8>(0xF0<<i)))
			{
				sequenceLength = 4-i;
				break;
			}
		}

		if ((sequenceLength<2 || sequenceLength>6) && sequenceLength!=0)
		{
			currentUnicodeCharacter=replacementcharacter;
		}
		else
		{		
			if ((pointerToLastUtf8Byte-pointerToCurrentUtf8Byte+1)<sequenceLength)
			{
				// we dnt have enough UTF-8 bytes to complete the Muti-Byte character.
				// store the character within the state.
				state->__count = 0;
            			while (pointerToCurrentUtf8Byte <= pointerToLastUtf8Byte)
        		        {
            			        state->__value.__wchb[state->__count++] = *(pointerToCurrentUtf8Byte++);
		                }
            			// reset the current pointer
    	        		pointerToCurrentUtf8Byte -= state->__count;
				if((pointerToCurrentUnicodeCharacter-aUnicode.Ptr())==0)
			        {
				        // still nothing is decoded.
				        if ( utf8 )
			                {
	        			        CleanupStack::PopAndDestroy(); // utf8
			                }
				        return -2;
				        //return -1;
				}
        			// something is already decoded, so return the no of bytes that use for 
	        		// decoding.
		        	break;
		        }			
		    
                        // reset the state				
                    	state->__count = 0;
			currentUnicodeCharacter = currentUtf8Byte&(0x7F>>sequenceLength);
			
        		for(TInt i=sequenceLength;i>1; i--)
	                {
        			currentUtf8Byte = *(++pointerToCurrentUtf8Byte);
        			if ((currentUtf8Byte&0xc0)==0x80)
        			{
	       				currentUnicodeCharacter = (currentUnicodeCharacter<<6)|(currentUtf8Byte&0x3F);
        			}
        			else
        			{
        				// Encoding error occured.
        				// store the contained within the state and return -1.
        				// set the error EILSEQ to errno
                		        if ( utf8 )
               			        {
                			        CleanupStack::PopAndDestroy(); // utf8
               			        }
        				errno = EILSEQ;
               				return -1;
        				//currentUnicodeCharacter=replacementcharacter;
        				//--pointerToCurrentUtf8Byte;
        			}
	       		}
	        }
			
       		if (currentUnicodeCharacter > 0xFFFF)
       		{
	        	if(pointerToCurrentUnicodeCharacter>=pointerToLastUnicodeCharacter)
	        	{
        			// unicode descriptor dnt have 2 wchar bytes to hold the data.
	       			pointerToCurrentUtf8Byte=pointerToPendingUtf8Byte;
	        		break;
	        	}
		
        		TUint surrogate = (currentUnicodeCharacter>>10) + 0xD7C0;
	       		*pointerToCurrentUnicodeCharacter=static_cast<TUint16>(surrogate);			
	        	++pointerToCurrentUnicodeCharacter;
				
		        surrogate = (currentUnicodeCharacter&0x3FF)+0xDC00;
        		*pointerToCurrentUnicodeCharacter=static_cast<TUint16>(surrogate);			
	       		++pointerToCurrentUnicodeCharacter;
	        	++pointerToCurrentUtf8Byte;
	        }
        	else
		{
	        	*pointerToCurrentUnicodeCharacter=static_cast<TUint16>(currentUnicodeCharacter);
		        ++pointerToCurrentUnicodeCharacter;
        		++pointerToCurrentUtf8Byte;
		}
	        
	        if ((pointerToCurrentUtf8Byte>pointerToLastUtf8Byte) || (pointerToCurrentUnicodeCharacter>pointerToLastUnicodeCharacter))
	        {
        		// checking the boundary condition.
        		// Here either the UTF-8 or Unicode descriptor reached to the end.
        		break;
        	}
	} // forever
        // decoding finished.
	aUnicode.SetLength(pointerToCurrentUnicodeCharacter-aUnicode.Ptr());
        if ( utf8 )
        {
                CleanupStack::PopAndDestroy(); // utf8
        }
       	//return pointerToLastUtf8Byte-pointerToCurrentUtf8Byte+1;
	// returns the number of bytes used to complete a valid multibyte character.
	return pointerToCurrentUtf8Byte - aUtf8.Ptr();
} //end of function

//-----------------------------------------------------------------------------
//Function Name : TInt _Utf16ToUtf8(char* aDst, wchar_t aSrc, mbstate_t* ps, int aLen )
//Description   : Converts wide char in UCS2 format to UTF8 equivalent
//Return Value  : The number of bytes converted, 0 if L'\0\' was translated, -1 on
//generic error and errno set appropriately, -2 if len is not sufficient to store aSrc wide char
//-----------------------------------------------------------------------------
TInt _Utf16ToUtf8(char* dst, wchar_t aSrc, mbstate_t* ps, int aLen)
{
	int retval = 0;
	// check the state 
	if(ps->__count !=_EUTF16InitialState && ps->__count != _EUTF16_21BitExtensionState)
	{
		errno = EINVAL;
		return -1;
	}
	
	//following characters are illegal
	//see http://www.unicode.org/faq/utf_bom.html#40
	if(aSrc == 0xFFFE || aSrc == 0xFFFF || (aSrc >= 0xFDD0 && aSrc <= 0xFDEF) )
	{
		errno = EILSEQ;
		return -1;
	}
	
			
	if(ps->__count == _EUTF16InitialState)
	{
	
		//following characters in addition are illegal in initial state
		//see http://www.unicode.org/faq/utf_bom.html#40
		if((aSrc >= 0xDC00 && aSrc <= 0xDFFF) )
		{
			errno = EILSEQ;
			return -1;
		}

	
		if ((aSrc & 0xff80)==0x0000)
		{
			if(aLen >= 1)
			{
				*dst++ = static_cast<TUint8>(aSrc);
				retval = 1;
			}
			else
			{
				return -2;
			}
			
		}
		else if ((aSrc & 0xf800)==0x0000)
		{
			if (aLen >= 2)
			{
				*dst++ = static_cast<TUint8>(0xc0|(aSrc>>6));
				*dst++ = static_cast<TUint8> (0x80|(aSrc&0x3f));
				retval = 2;
			}
			else
			{
				return -2;
			}
		}
		else if ((aSrc & 0xfc00)==0xd800)
		{
			 ps->__value.lead = aSrc;
		 	 ps->__count = _EUTF16_21BitExtensionState;
		 	retval = 0; //nothing written out just yet
		}
		else
		{
			if ( aLen >= 3)
			{
				*dst++ = static_cast<TUint8>(0xe0|(aSrc>>12));
				*dst++ = static_cast<TUint8>(0x80|((aSrc>>6)&0x3f));
				*dst++ = static_cast<TUint8>(0x80|(aSrc&0x3f));
				retval = 3;
			}
			else
			{
				return -2;
			}
		}
		
		
	}
	else //ps->__count == _EUCS2_21BitExtensionState)
	{
		//characters outside this range are illegal in this state
		//see http://www.unicode.org/faq/utf_bom.html#40
		if((aSrc < 0xDC00 || aSrc > 0xDFFF) )
		{
			errno = EILSEQ;
			return -1;
		}
		
		if ((aSrc & 0xfc00)!=0xdc00)
		{
			errno = EILSEQ;
			return -1;
		}
		if ( aLen >= 4)
		{
			//snippet taken from unicode faq
			//http://www.unicode.org/faq/utf_bom.html#39
			
			unsigned long  codepoint = (ps->__value.lead << 10) + aSrc + KSURROGATE_OFFSET;
			
			*dst++ = static_cast<TUint8>( 0xf0|(codepoint>>18));
			*dst++ = static_cast<TUint8>(0x80|((codepoint>>12)&0x3f));
			*dst++ = static_cast<TUint8>(0x80|((codepoint>>6)&0x3f));
			*dst++ = static_cast<TUint8>(0x80|(codepoint&0x3f));
			retval = 4;
		}
		else
		{
			return -2;
		}
		ps->__count = _EUTF16InitialState;
	}
	return retval;
	
	
}//end of function

