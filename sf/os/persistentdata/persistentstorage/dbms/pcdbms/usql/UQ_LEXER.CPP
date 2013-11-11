// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SQL parser tokeniser
// 
//

#include "UQ_STD.H"

// optimised tables for ASCII character set

const TUint8 KAlpha=0x1;
const TUint8 KDigitOr_=0x2;

const TUint8 KCharAttrib[]=
	{
	KDigitOr_,KDigitOr_,KDigitOr_,KDigitOr_,KDigitOr_,KDigitOr_,KDigitOr_,KDigitOr_,
	KDigitOr_,KDigitOr_,0,0,0,0,0,0,
	0,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,
	KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,
	KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,
	KAlpha,KAlpha,KAlpha,0,0,0,0,KDigitOr_,
	0,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,
	KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,
	KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,KAlpha,
	KAlpha,KAlpha,KAlpha
	};

#define ISALPHA(aChar) (TUint(aChar-'0')<=TUint('z'-'0') && KCharAttrib[aChar-'0']&KAlpha)
#define ISALPHA_DIGIT_OR_(aChar) (TUint(aChar-'0')<=TUint('z'-'0') && KCharAttrib[aChar-'0']&(KAlpha|KDigitOr_))
#define LCASE(aChar) (aChar|0x20)

// The keywords
// These are always stored as ASCII because DBMS has its own 

const TInt KMaxKeywordLength=15;

static const TText8 KSqlKeywords[][KMaxKeywordLength+1]=
	{
#define KEYWORD(s) #s
#include "UQ_KEYWD.H"
#undef KEYWORD
	};
const TInt KSqlKeywordCount=sizeof(KSqlKeywords)/sizeof(*KSqlKeywords);

#if defined(_ASSERTIONS)
TInt CheckKeywords()
//
// ensure that the keyword table is in alphabetical order
//
	{
	for (TInt ii=1;ii<KSqlKeywordCount;++ii)
		__ASSERT(TPtrC8(KSqlKeywords[ii-1])<TPtrC8(KSqlKeywords[ii]));
	return 1;
	}
#endif

// class TSqlLexer

TInt TSqlLexer::CompareKeyword(TInt aKeyword,const RSqlLiteral& aIdentifier)
//
// Check if the identifer in aIdentifier is a keyword
// uses a case-insensitive match, not folding
//
	{
	__ASSERT(TUint(aKeyword)<TUint(KSqlKeywordCount));
//
	const TText* ptr=aIdentifier.Ptr();
	const TText* end=aIdentifier.End();
	const TText8* pk=&KSqlKeywords[aKeyword][0];
	for (;;)
		{
		TUint ck=*pk++;
		if (ptr==end)
			return ck;
		if (!ck)
			return -1;
		TInt d=ck-LCASE(TUint(*ptr++));
		if (d)
			return d;
		}
	}

TSqlKeyword TSqlLexer::Keyword(const TSqlToken& aToken)
//
// non-member function: Return the keyword value
//
	{
	if (aToken==ESqlIdentifier)
		{
		TInt r=KSqlKeywordCount;
		TInt l=0;
		while (r>l)
			{
			TInt m=(l+r)>>1;
			TInt k=CompareKeyword(m,aToken.Literal());
			if (k>0)
				r=m;
			else if (k<0)
				l=m+1;
			else
				return TSqlKeyword(m);		// keyword
			}
		}
	// identifier
	return ESqlNotKeyword;
	}

TSqlLexer::TSqlLexer(const TDesC& aSql)
	: iNext(aSql.Ptr()),iEnd(iNext+aSql.Length())
	{
	__ASSERT(CheckKeywords());
	}

TSqlTokenType TSqlLexer::GetIdentifier(TSqlToken& aToken)
//
// Get a keyword or identifier. Do not resolve a keyword at this stage
//
	{
	const TText* end=iEnd;
	const TText* next=iNext-1;
	while (++next<end)
		{
		TUint ch=*next;
		if (ISALPHA_DIGIT_OR_(ch))
			continue;
		if (!TChar(ch).IsAlphaDigit())
			break;
		}
	aToken.iLiteral.SetText(iNext-1,next);
	iNext=next;
	return ESqlIdentifier;
	}

TInt TSqlLexer::GetInteger(TInt64 &aVal)
//
// A rather more optimised version of TLex::Val(TInt64&)
// initially accumulate the value in a TUint32, and only switch to 64-bit arithmetic
// if the value overflows. Always return a 64-bit value
//
	{
	const TUint KPreMultiplyLimit32=429496728;	//(KMaxTUint-9)/10
	const TUint KPreMultiplyLimit64=214748364;	//(KMaxTInt+1)/10
//
	const TText* ptr=iNext;
	const TText* const end=iEnd;
	__ASSERT(ptr<end);
	TUint sign=0;
	TUint c=*ptr;
	if (c=='-')
		{
		sign=1;
		if (++ptr==end)
			return KErrGeneral;
		c=*ptr;
		}
	else if (c=='+')
		{
		if (++ptr==end)
			return KErrGeneral;
		c=*ptr;
		}
	c-='0';
	if (c>=10u)
		return KErrGeneral;		// no digits at all
	TUint val32=c;
	while (++ptr<end)
		{
		c=*ptr-'0';
		if (c>=10u)
			break;
		if (val32>KPreMultiplyLimit32)
			goto overflow64;	// will not fit into 32 bit arithmetic
		val32*=10;
		val32+=c;
		}
// we have result, just set the sign and finish
	aVal=val32;
	goto checksign;
//
// continue the accumulation with a 64-bit integer
overflow64:
	aVal=val32;
	for (;;)
		{
		I64MUL10(aVal);
		aVal+=c;
		if (++ptr==end)
			break;
		c=*ptr-'0';
		if (c>=10u)
			break;
		if (I64HIGH(aVal)>KPreMultiplyLimit64)
			return KErrOverflow;	// the value is certain to overflow
		}
	if (I64HIGH(aVal)&0x80000000u)
		{	// greater than the "half way mark"
		if (!sign)
			return KErrOverflow;
		if (I64LOW(aVal)!=0)
			return KErrOverflow;
		}
checksign:
	iNext=ptr;
	if (sign)
		aVal=-aVal;
	return KErrNone;
	}

TSqlTokenType TSqlLexer::GetNumber(TSqlToken& aToken)
	{
	const TText* mark=--iNext;			// rewind past initial character
	// attempt to parse a integer
	TInt r=GetInteger(aToken.iLiteral.SetInt());
	if (r==KErrNone)
		{
		if (iNext<iEnd)
			{
			TUint c=*iNext;
			if (c!='.' && c!='e' && c!='E')
				return ESqlLiteralInt;		// it is an integer
			}
		else
			return ESqlLiteralInt;		// it is an integer
		}
	TLex lex(TPtrC(mark,iEnd-mark));
	r=lex.Val(aToken.iLiteral.SetReal(),TChar('.'));
	if (r!=KErrNone)
		return SqlError(r);
	iNext=mark+lex.Offset();
	return ESqlLiteralReal;
	}

TSqlTokenType TSqlLexer::GetString(TSqlToken& aToken)
	{
	const TText* next=iNext;
	const TText* end=iEnd;
	for (;;)
		{
		if (next==end)
			return SqlError();
		TUint c=*next++;
		if (c=='\'')
			{
			if (next==end)
				break;
			if (*next!='\'')
				break;
			next++;
			}
		}
	aToken.iLiteral.SetText(iNext,next-1);
	iNext=next;
	return ESqlLiteralText;
	}

TSqlTokenType TSqlLexer::GetDate(TSqlToken& aToken)
	{
	const TText* end=iEnd;
	const TText* next=iNext;
	do
		{
		if (next==end)
			return SqlError();
		} while (*next++!='#');
	TInt r=aToken.iLiteral.SetTime().Parse(TPtrC(iNext,(next-1)-iNext));
	if (r<0)
		return SqlError(r);
	iNext=next;
	return ESqlLiteralTime;
	}

TSqlTokenType TSqlLexer::GetBlob(TSqlToken& aToken)
	{
	const TText* end=iEnd;
	const TText* next=iNext;
	// Blob literalisation format X'<hex-data>'
	// first char must be single quote - '
	if ( *next != '\'' ) {
		return SqlError(KErrArgument);
	}
	const TText* start = ++next;
	do
		{
		if (next==end)
			return SqlError();
		} while (*next++!='\'');
	const TText* blobend = next-1;

	aToken.iLiteral.SetBlob(start,blobend);
	iNext=next;
	return ESqlLiteralBlob;
	}

TSqlTokenType TSqlLexer::GetNextToken(TSqlToken& aToken)
	{
	const TText* ptr=iNext;
	const TText* const end=iEnd;
	for (;;)
		{
		if (ptr==end)
			return ESqlEos;
		TUint ch=*ptr++;
		iNext=ptr;
		switch (ch)
			{
		case ' ':			// a "normal" space
			continue;
		case '0': case '1': case '2': case '3': case '4':	// literal number
		case '5': case '6': case '7': case '8': case '9':
		case '+': case '-': case '.':
			return GetNumber(aToken);
		case '\'':
			return GetString(aToken);
		case '#':
			return GetDate(aToken);
		case 'X':
			return GetBlob(aToken);
		case '*':
			return ESqlAsterisk;
		case ',':
			return ESqlComma;
		case '(':
			return ESqlLeftBracket;
		case ')':
			return ESqlRightBracket;
		case '=':
			return ESqlEqual;
		case '<':
			{
			ch=*ptr++;				
			if (ch=='=')
				{
				iNext=ptr;
				return ESqlLessEqual;
				}
			if (ch=='>')
				{
				iNext=ptr;
				return ESqlNotEqual;
				}
			return ESqlLess;
			}
		case '>':
			{
			ch=*ptr++;			
			if (ch=='=')
				{
				iNext=ptr;
				return ESqlGreaterEqual;
				}
			return ESqlGreater;
			}
		default:
			break;
			}
		if (ISALPHA(ch))
			return GetIdentifier(aToken);		// keyword or identifier
		TChar cc(ch);
		if (cc.IsAlpha())
			return GetIdentifier(aToken);		// keyword or identifier
		if (!cc.IsSpace())
			return SqlError();
		}
	}
	
const TText* TSqlLexer::Next() const
	{
	return iNext;
	}
void TSqlLexer::Set(const TText* aNext) 
	{
	iNext = aNext ;
	}
