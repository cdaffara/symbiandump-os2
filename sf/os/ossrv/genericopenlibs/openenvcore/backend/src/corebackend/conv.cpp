// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <sys/errno.h>
#include "sysreent.h"
#include <charconv.h>
#include <utf.h>
#include "sysif.h"

#define CHARACTER_SET_NAME 32
#define UNICODE_TEXT_LEGTH 1
#define BUFFER_LENGTH 4

extern "C" {

EXPORT_C iconv_t _iconv_open_r(int* aErrno, const char* aTocode, const char* aFromcode)
	{
	_iconv_t* __iconv_t = NULL;
	TInt toCode = 0;
	TInt fromCode = 0;
	
	//Get reference file session from backend
	RFs& fileSession = Backend()->FileSession();
			
	//Allocate and constructs a CCnvCharacterSetConverter object
	CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewLC() ;
			
	//Create an array identifying all the character sets for which conversion is available
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet> *charSet = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL(fileSession);
	CleanupStack::PushL( charSet );
	
	TInt i = 0;
	//Get the number of character sets for which conversion is available
	TInt count = charSet->Count();
	while(i < count)
		{
		CCnvCharacterSetConverter::SCharacterSet characterSet = charSet->At(i);	
		i++;
		
		//Get the UID of character set for which conversion is available
		TUint charSetUID = characterSet.Identifier();
		const TBufC<CHARACTER_SET_NAME> charSetName = characterSet.NameIsFileName()?TParsePtrC(characterSet.Name()).Name():characterSet.Name();								
		TBuf8<CHARACTER_SET_NAME*4> characterSetname;
		if(CnvUtfConverter::ConvertFromUnicodeToUtf8(characterSetname,charSetName) != 0)
			{
			*aErrno = EINVAL;
			CleanupStack::PopAndDestroy(2);  //charSet, conv, fileSession
			return (iconv_t) -1;
			}
		characterSetname.ZeroTerminate();
	 	//Check for source or destination charcatre set 
		if(!characterSetname.Compare(TPtrC8((const TText8*) aTocode)))
			{
			toCode = charSetUID;
			}
		if(!characterSetname.Compare(TPtrC8((const TText8*) aFromcode)))
			{
			fromCode = charSetUID;
			}

		HBufC8*  stdInterName = conv->ConvertCharacterSetIdentifierToStandardNameL(charSetUID, fileSession);					
		if(NULL != stdInterName)
			{
			if(!stdInterName->Compare(TPtrC8((const TText8*) aTocode)))
				{
				toCode = charSetUID;
				}
			
			if(!stdInterName->Compare(TPtrC8((const TText8*) aFromcode)))
				{
				fromCode = charSetUID;
				}
			delete stdInterName;
			stdInterName = NULL;
			}

		//Check for both source and destination character sets are available
		if(	toCode && fromCode)
			{
			__iconv_t =  new _iconv_t ;
			if(NULL == __iconv_t)
				{
				*aErrno = ENOMEM;
				CleanupStack::PopAndDestroy(2); 
				return (iconv_t) -1;
				}
			__iconv_t->toCode = toCode;
			__iconv_t->fromCode = fromCode;
			break;
			}
		}
	//If any of the charcter set is not avaibale ,set the errno and return -1
	if(	!toCode ||  !fromCode)
		{
		*aErrno = EINVAL;
		CleanupStack::PopAndDestroy(2);  //charSet, conv, fileSession
		return (iconv_t) -1;
		}
	
	//delete the objects from cleanupstack and return the conversion descriptor
	CleanupStack::PopAndDestroy(2); 
	return 	(iconv_t) __iconv_t;
	}

EXPORT_C size_t _iconv_r(int* aErrno, iconv_t aCD, const char** inbuf, size_t* inbytesleft, char** outbuf, size_t* outbytesleft)
 	{
	_iconv_t* __iconv_t = (_iconv_t*) aCD;
	TInt retVal = KErrNone;
	TInt returnCode = -1;
	
	//If the conversion decriptor is a bad descriptor, set the error number and return -1
	if(aCD == (iconv_t) -1)
		{
		*aErrno = EBADF;
		return (size_t) -1;
		}
	
	//When inbuf and *inbuf is not NULL
	if(inbuf && *inbuf)
		{
		if((NULL == outbuf) || (NULL == *outbuf) || (inbytesleft == NULL) || (outbytesleft == NULL))
			{
			return 0;
			}
		
		//If output buffer does not have space, set the error number and return -1
		if(!*outbytesleft)
			{
			*aErrno = E2BIG;
			return (size_t)-1;
			}
		
		//If all the bytes in input buffer is converted
		if(!*inbytesleft)
			{
			return 0;
			}
		if(!outbuf || !*outbuf)
			{
			return 0;
			}
		
		//Create a handle to a file server session and connect to the file server. 
		//Push it on to cleanup stack
		RFs& fileSession = Backend()->FileSession();
			
		TInt state = CCnvCharacterSetConverter::KStateDefault;
	
	 	//Allocate and constructs a CCnvCharacterSetConverter object
		CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewLC() ;
			
		while(*inbuf != NULL && *inbytesleft )
			{
			TPtrC8 remainderOfForeignText((const TText8*) *inbuf,*inbytesleft );
			TBuf16<UNICODE_TEXT_LEGTH> UnicodeText;
			
			//Search the character set array  containing all of the character sets for which conversion is available
			CCnvCharacterSetConverter::TAvailability  avail = conv->PrepareToConvertToOrFromL(__iconv_t->fromCode, fileSession);
			if(CCnvCharacterSetConverter::ENotAvailable == avail)
				{
				break;
				}
				
			TInt numberOfUnconvertibleCharacters = 0;
			TInt indexOfFirstByteOfFirstUnconvertibleCharacter = 0;
			TInt inbufIncr = 0;
			
			//Convert text encoded in a non-Unicode character set into the Unicode character set (UCS-2).
			retVal = conv->ConvertToUnicode(UnicodeText, remainderOfForeignText, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
			
			//Check for number of unconverted bytes left at the end of the input descriptor 
			//or one of the error values defined in TError
			if(retVal < 0 && (retVal != CCnvCharacterSetConverter::EErrorIllFormedInput))
				{
				break;
				}
			
			//Check for incomplete characters in the input, set the errno accordingly
			if(numberOfUnconvertibleCharacters)
				{
				*aErrno = EILSEQ;
				break;
				}
			
			//Check for illegal characters in the input, set the errno accordingly
			if(retVal == CCnvCharacterSetConverter::EErrorIllFormedInput)
				{
				*aErrno = EILSEQ;//Set Error number to EILSEQ
				break;
				}
			inbufIncr = remainderOfForeignText.Size() - retVal;
			remainderOfForeignText.Set(remainderOfForeignText.Right(retVal));
						
			//Search the character set array  containing all of the character sets for which conversion is available
			avail = conv->PrepareToConvertToOrFromL(__iconv_t->toCode, fileSession);
			if(CCnvCharacterSetConverter::ENotAvailable == avail)
				{
				break;
				}
			
			numberOfUnconvertibleCharacters = 0;
			indexOfFirstByteOfFirstUnconvertibleCharacter = 0;
			TBuf8<BUFFER_LENGTH> outputBuffer;
			
			//Convert text encoded in the Unicode character set (UCS-2) into other character sets
			retVal = conv->ConvertFromUnicode(outputBuffer,UnicodeText, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
			 	
			if(retVal < 0 && (retVal != CCnvCharacterSetConverter::EErrorIllFormedInput))
				{
				break;
				}
			
			//Check for illegal characters in the input, set the errno accordingly
			if(retVal == CCnvCharacterSetConverter::EErrorIllFormedInput)
				{
				*aErrno = EILSEQ;//Set Error number to EILSEQ
				break;
				}
			
			//Check for incomplete characters in the input, set the errno accordingly
			if(numberOfUnconvertibleCharacters)
				{
				*aErrno = EILSEQ;
				break;
				}

			TInt outputbufferLength = outputBuffer.Length();
			if(*outbytesleft >= outputbufferLength)
				{
				*inbytesleft = *inbytesleft - inbufIncr;
				*inbuf = *inbuf + inbufIncr;
				memcpy(*outbuf,(const char*) outputBuffer.Ptr(),outputbufferLength);
				*outbuf = *outbuf + outputbufferLength;
				*outbytesleft -= outputbufferLength;	
				if(!*inbytesleft || !*inbuf)
					{
					returnCode =  0;
					break;
					}
				}
			else
				{
				*aErrno = E2BIG;
				break;
				}
			
			}//end of while
		CleanupStack::PopAndDestroy(1);  //conv
		return returnCode;
		}
	
	// when inbuf is NULL or *inbuf is NULL
	else if((NULL == inbuf) || (NULL == *inbuf))
		{
		if(outbuf && *outbuf)
			{
			return 0;
			}
		if(outbytesleft && !*outbytesleft)
			{
			*aErrno = E2BIG;
			return (size_t)-1;
			}
		}
	
	// when inbuf or *inbuf is NULL and outbuf or *outbuf is NULL
	else if (((NULL == inbuf) || (NULL == *inbuf)) && ((NULL == outbuf) || (NULL == *outbuf))) 
		{
		return 0;
		}
	return retVal;
	}

EXPORT_C int _iconv_close_r(int* aErrno, iconv_t cd)
	{
	//If the conversion decriptor is a bad descriptor, set the error number and return -1
	if(cd == (iconv_t) -1)
		{
		*aErrno = EBADF;
		return -1;
		}
	
  //Deallocate the conversion descriptor
	if(cd != NULL)
		{
		//cast it back to _iconv_t and issue a delete
		_iconv_t* convp = (_iconv_t*)cd;
		delete convp;
		cd = NULL;
		}
	return 0;
	}
	
} //extern 



