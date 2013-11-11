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
*
*/


#include <s32file.h>
#include "t_input.h"
#include "t_errorconverter.h"

const TUint KMaxTagLength = 80;

EXPORT_C TPtrC8 Input::ParseElement(const TDesC8& aBuf, const TDesC8& aStart, const TDesC8& aEnd)
	{
	TInt pos  = 0;
	return Input::ParseElement(aBuf, aStart, aEnd, pos);
	}

EXPORT_C TPtrC8 Input::ParseElement(const TDesC8& aBuf, const TDesC8& aStart)
	{
	TInt pos = 0;
	TBuf8<KMaxTagLength> end;
	end.Copy(aStart);
	end.Insert(1, _L8("/"));
	return Input::ParseElement(aBuf, aStart, end, pos);	
	}

EXPORT_C TPtrC8 Input::ParseElement(const TDesC8& aBuf, 
										const TDesC8& aStart, 
										const TDesC8& aEnd, 
										TInt& aPos)
	{
	TInt err = KErrNone;
	return Input::ParseElement(aBuf, aStart, aEnd, aPos, err);
	}


// Find the portion of aBuf between aStart and aEnd, allowing for nesting
EXPORT_C TPtrC8 Input::ParseElement(const TDesC8& aBuf, 
									const TDesC8& aStart,
									const TDesC8& aEnd, 
									TInt& aPos,
									TInt& aError)
	{
	aError = KErrNone;
	TInt startPos = KErrNotFound;
	TInt level = 0;
	do
		{
		TPtrC8 data = aBuf.Mid(aPos);

		// Find the first start or end tag
		TInt nextStart = data.Find(aStart);
		TInt nextEnd = data.Find(aEnd);

		if (nextStart == KErrNotFound && nextEnd == KErrNotFound)
			{
			// None found
			aError = KErrNotFound;
			return TPtrC8();
			}
		else if (nextEnd == KErrNotFound || (nextStart != KErrNotFound && nextStart < nextEnd))
			{
			// Start tag found first
			++level;
			aPos += nextStart + aStart.Length();

			if (startPos == KErrNotFound)
				{
				// Record position of first start tag
				startPos = aPos;
				}
			}
		else
			{
			// End tag found first
			--level;
			aPos += nextEnd + aEnd.Length();
			}
		}
	while (level > 0);

	if (level == -1)
		{
		// End tag found before start tag
		aError = KErrArgument;
		return TPtrC8();
		}

	ASSERT(startPos != KErrNotFound);
	return aBuf.Mid(startPos, aPos - startPos - aEnd.Length());
	}

EXPORT_C TPtrC16 Input::ParseElement(const TDesC16& aBuf, const TDesC16& aStart, const TDesC16& aEnd)
	{
	TInt pos  = 0;
	return Input::ParseElement(aBuf, aStart, aEnd, pos);
	}


EXPORT_C TPtrC16 Input::ParseElement(const TDesC16& aBuf, 
										const TDesC16& aStart, 
										const TDesC16& aEnd, 
										TInt& aPos)
	{
	TInt err = KErrNone;
	return Input::ParseElement(aBuf, aStart, aEnd, aPos, err);
	}


EXPORT_C TPtrC16 Input::ParseElement(const TDesC16& aBuf, 
									const TDesC16& aStart,
									const TDesC16& aEnd, 
									TInt& aPos,
									TInt& aError)
	{
	aError = KErrNone;
	const TInt length = aBuf.Length();
	TPtrC16 temp1 = aBuf.Right(length - aPos);
	TInt elementStart = temp1.Find(aStart);
	if (elementStart < 0)
		{
		aError = elementStart;
		TPtrC16 res(KNullDesC16);
		return res;
		}
	elementStart+= aStart.Length();
	aPos+=elementStart;

	TPtrC16 temp2 = temp1.Right(temp1.Length() - elementStart);
	TInt elementEnd = temp2.Find(aEnd);
	if (elementEnd < 0)
		{
		TPtrC16 res(KNullDesC16);
		return res;
		}

	aPos+=elementEnd;
	aPos+=aEnd.Length();

	TPtrC16 element = temp2.Left(elementEnd);
	return element;
	}

EXPORT_C HBufC8* Input::ParseElementHexL(const TDesC8& aBuf, const TDesC8& aStart)
	{
	TPtrC8 data = Input::ParseElement(aBuf, aStart);
	__ASSERT_ALWAYS(data.Size() % 2 == 0, User::Panic(_L("ParseElementHexL"), 1));
	TInt bytes = data.Size()/2;
	HBufC8* buf = HBufC8::NewLC(bytes);
	TPtr8 ptr = buf->Des();
	ptr.SetLength(bytes);

	if(bytes)
		{
		for (TInt i = 0 ; i < data.Length() ; i += 2)
			{
			TUint8 tmp;
			tmp=(TUint8)(data[i]-(data[i]>'9'?('A'-10):'0'));
			tmp*=16;
			tmp|=(TUint8)(data[i+1]-(data[i+1]>'9'?('A'-10):'0'));
			ptr[i/2] = tmp;
			}
		}
	CleanupStack::Pop(buf);
	return buf;
	}

EXPORT_C TBool Input::ParseElementBoolL(const TDesC8& aBuf, const TDesC8& aStart)	
	{
	TPtrC8 value = Input::ParseElement(aBuf, aStart);
	if( value.CompareF(_L8("ETrue")) == 0 || value.CompareF(_L8("True")) == 0 )
		{
		return ETrue;
		}
	else if( value.CompareF(_L8("EFalse")) == 0 || value.CompareF(_L8("False")) == 0 )
		{
		return EFalse;
		}
	//if it's neither false nor true, leave
	User::Leave(KErrArgument);
	return EFalse;
	}


EXPORT_C TBool Input::GetExpectedResultL(const TDesC& aResult /* in */, TInt &aError /* out */)
	{
	TBool result;
	CErrorConverter *errorConverter = CErrorConverter::NewLC();

	result = errorConverter->GetExpectedResultL(aResult, aError);
	CleanupStack::PopAndDestroy(errorConverter);
	
	return(result);
	}


EXPORT_C TBool Input::GetExpectedResultL(const TInt &aError /* in */, HBufC*& aResult /* out */)
	{
	TBool result;
	CErrorConverter *errorConverter = CErrorConverter::NewLC();

	result = errorConverter->GetExpectedResultL(aError, aResult);
	CleanupStack::PopAndDestroy(errorConverter);
	
	return(result);
	}

// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C void Input::ParseElementL(HBufC*& aMember,
								   const TDesC& aBuf, 
								   const TDesC& aStart)
	{
	TInt pos = 0;
	ParseElementL(aMember, aBuf, aStart, pos);
	};

// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C void Input::ParseElementL(HBufC8*& aMember,
								   const TDesC& aBuf, 
								   const TDesC& aStart)
	{
	TInt pos = 0;
	ParseElementL(aMember, aBuf, aStart, pos);
	};

// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C void Input::ParseElementL(HBufC*& aMember,
								   const TDesC& aBuf, 
								   const TDesC& aStart,
								   TInt& aPos)
	{
	TInt error = KErrNone;
	ParseElementL(aMember, aBuf, aStart, aPos, error);
	};

// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C void Input::ParseElementL(HBufC8*& aMember,
								   const TDesC& aBuf, 
								   const TDesC& aStart,
								   TInt& aPos)
	{
	TInt error = KErrNone;
	ParseElementL(aMember, aBuf, aStart, aPos, error);
	};

// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C void Input::ParseElementL(HBufC*& aMember,
								   const TDesC& aBuf, 
								   const TDesC& aStart,
								   TInt& aPos,
								   TInt& aError)
	{
	__ASSERT_DEBUG(!aMember, User::Panic(_L("Input"), 1));


	TPtrC value = ParseElement(aBuf, aStart, aPos, aError);
	// copy file name to unicode buffer
	aMember = HBufC::NewL(value.Length());
	aMember->Des().Copy(value);
	}

// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C void Input::ParseElementL(HBufC8*& aMember,
								   const TDesC& aBuf, 
								   const TDesC& aStart,
								   TInt& aPos,
								   TInt& aError)
	{
	__ASSERT_DEBUG(!aMember, User::Panic(_L("Input"), 1));


	TPtrC value = ParseElement(aBuf, aStart, aPos, aError);
	// copy file name to narrow buffer
	aMember = HBufC8::NewL(value.Length());
	aMember->Des().Copy(value);
	}

// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C TPtrC Input::ParseElement(const TDesC& aBuf, const TDesC& aStart)
	{
	TInt pos = 0;
	return(ParseElement(aBuf, aStart, pos));
	};


// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C TPtrC Input::ParseElement(const TDesC& aBuf, const TDesC& aStart, TInt& aPos)
	{
	TInt error = KErrNone;
	return(ParseElement(aBuf, aStart, aPos, error));
	};


// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C TPtrC Input::ParseElement(const TDesC& aBuf, const TDesC& aStart, TInt& aPos, TInt& aError)
	{
	// using the start tag <abcd> create a matching tag terminator <\abcd>
	HBufC* hend = HBufC::NewMaxLC(aStart.Length()+1);	

	TPtr end = hend->Des();
	end = aStart; // copy in start tag
	end.Insert(1,_L("/"));  // make it into a terminator tag with same element name
	// get value from element
	TPtrC value = Input::ParseElement(aBuf, aStart, end, aPos, aError);

	CleanupStack::PopAndDestroy(hend);
	return(value);
	}

EXPORT_C HBufC8* Input::ReadFileL(const TDesC& aFilename, RFs& aFs)
	{
	HBufC8* ret = ReadFileLC(aFilename, aFs);
	CleanupStack::Pop();
	return ret;
	}

EXPORT_C HBufC8* Input::ReadFileL(const TDesC& aFilename, const TDesC& aPath, RFs& aFs)
	{
	HBufC8* ret = ReadFileLC(aFilename, aPath, aFs);
	CleanupStack::Pop();
	return ret;
	}

EXPORT_C HBufC8* Input::ReadFileLC(const TDesC& aFilename, RFs& aFs)
	{
	RFile file;
	User::LeaveIfError(file.Open(aFs, aFilename, EFileRead));
	CleanupClosePushL(file);
	TInt size;
	file.Size(size);
	CleanupStack::PopAndDestroy();//fileClose

	HBufC8* res = HBufC8::NewLC(size);
	TPtr8 p(res->Des());
	p.SetLength(size);

	RFileReadStream stream;
	User::LeaveIfError(stream.Open(aFs, aFilename, EFileStream));
	CleanupClosePushL(stream);
	stream.ReadL(p, size);
	CleanupStack::PopAndDestroy();//streamClose
	return res;
	}

EXPORT_C HBufC8* Input::ReadFileLC(const TDesC& aFilename, const TDesC& aPath, RFs& aFS)
	{
	TFileName fullname;
	fullname.Append(aPath);
	fullname.Append(aFilename);

	RFile file;
	User::LeaveIfError(file.Open(aFS, fullname, EFileRead));
	CleanupClosePushL(file);
	TInt size;
	file.Size(size);
	CleanupStack::PopAndDestroy();//fileClose

	HBufC8* res = HBufC8::NewLC(size);
	TPtr8 p(res->Des());
	p.SetLength(size);

	RFileReadStream stream;
	User::LeaveIfError(stream.Open(aFS, fullname, EFileStream));
	CleanupClosePushL(stream);
	stream.ReadL(p, size);
	CleanupStack::PopAndDestroy();//streamClose
	return res;
	}

EXPORT_C void Input::ParseElementListL(const TDesC& aBuf, 
									   const TDesC& aStart,
									   const TDesC& aEnd, 
									   RPointerArray<HBufC>& aElements,
									   TInt& aPos)
	{
	TPtrC currentElement = ParseElement(aBuf, aStart, aEnd, aPos);
	while (currentElement != KNullDesC)
		{
		HBufC* newElement = HBufC::NewL(currentElement.Size());
		*newElement = currentElement;
		User::LeaveIfError(aElements.Append(newElement));
		currentElement.Set(ParseElement(aBuf, aStart, aEnd, aPos));
		}
	}

EXPORT_C TUint Input::ParseIntElement(const TDesC8& aBuf, const TDesC8& aStart, const TDesC8& aEnd)
	{
	TInt pos  = 0;
	return Input::ParseIntElement(aBuf, aStart, aEnd, pos);
	}


EXPORT_C TUint Input::ParseIntElement(const TDesC8& aBuf, 
										const TDesC8& aStart, 
										const TDesC8& aEnd, 
										TInt& aPos)
	{
	TInt err = KErrNone;
	return Input::ParseIntElement(aBuf, aStart, aEnd, aPos, err);
	}


EXPORT_C TUint Input::ParseIntElement(const TDesC8& aBuf, 
									const TDesC8& aStart,
									const TDesC8& aEnd, 
									TInt& aPos,
									TInt& aError)
	{
	aError = KErrNone;
	TPtrC8 temp = ParseElement(aBuf, aStart, aEnd, aPos, aError);

	TUint result = 0;

	if(aError == KErrNone)
		{
		TLex8 lex;
		if(temp.Left(2) == _L8("0x"))
			{
			lex = temp.Mid(2);
			aError = lex.Val(result, EHex);
			}
		else
			{
			lex = temp;
			aError = lex.Val(result);
			};
		
		if(aError == KErrNone)
			{
			if(!lex.Eos())
				{
				aError = KErrArgument;
				result = 0;
				};
			};
		};
	return(result);
	}

EXPORT_C TUint Input::ParseIntElement(const TDesC16& aBuf, const TDesC16& aStart, const TDesC16& aEnd)
	{
	TInt pos  = 0;
	return Input::ParseIntElement(aBuf, aStart, aEnd, pos);
	}


EXPORT_C TUint Input::ParseIntElement(const TDesC16& aBuf, 
										const TDesC16& aStart, 
										const TDesC16& aEnd, 
										TInt& aPos)
	{
	TInt err = KErrNone;
	return Input::ParseIntElement(aBuf, aStart, aEnd, aPos, err);
	}


EXPORT_C TUint Input::ParseIntElement(const TDesC16& aBuf, 
									const TDesC16& aStart,
									const TDesC16& aEnd, 
									TInt& aPos,
									TInt& aError)
	{
	aError = KErrNone;
	TPtrC16 temp = ParseElement(aBuf, aStart, aEnd, aPos, aError);

	TUint result = 0;

	if(aError == KErrNone)
		{
		TLex16 lex;
		if(temp.Left(2) == _L16("0x"))
			{
			lex = temp.Mid(2);
			aError = lex.Val(result, EHex);
			}
		else
			{
			lex = temp;
			aError = lex.Val(result);
			};
		
		if(aError == KErrNone)
			{
			if(!lex.Eos())
				{
				aError = KErrArgument;
				result = 0;
				};
			};
		};
	return(result);
	}


// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C TUint Input::ParseIntElement(const TDesC& aBuf, const TDesC& aStart)
	{
	TInt pos = 0;
	return(ParseIntElement(aBuf, aStart, pos));
	};


// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C TUint Input::ParseIntElement(const TDesC& aBuf, const TDesC& aStart, TInt& aPos)
	{
	TInt error = KErrNone;
	return(ParseIntElement(aBuf, aStart, aPos, error));
	};


// Allocate memory on the heap and copy in the value of a tagged element
EXPORT_C TUint Input::ParseIntElement(const TDesC& aBuf, const TDesC& aStart, TInt& aPos, TInt& aError)
	{
	// get value from element
	TPtrC temp = Input::ParseElement(aBuf, aStart, aPos, aError);
	TUint result = 0;

	if(aError == KErrNone)
		{
		TLex16 lex;
		if(temp.Left(2) == _L16("0x"))
			{
			lex = temp.Mid(2);
			aError = lex.Val(result, EHex);
			}
		else
			{
			lex = temp;
			aError = lex.Val(result);
			};
		
		if(aError == KErrNone)
			{
			if(!lex.Eos())
				{
				aError = KErrArgument;
				result = 0;
				};
			};
		};

	return(result);	
	}
