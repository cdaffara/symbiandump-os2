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
//

// Class RSqlLiteral
inline RSqlLiteral::RSqlLiteral()
	:iType(ENull) 
	{iBuffer=NULL;}

inline TBool RSqlLiteral::IsAlloc() const
	{return iType>=EBuf8;}
inline TBool RSqlLiteral::IsBasic() const
	{return iType>=EInt64&&iType<=EPtr;}

inline TInt64& RSqlLiteral::SetInt()
	{__ASSERT(!IsAlloc());iType=EInt64;return iVal.iInt64();}
inline TReal64& RSqlLiteral::SetReal()
	{__ASSERT(!IsAlloc());iType=EReal64;return iVal.iReal64;}
inline TTime& RSqlLiteral::SetTime()
	{__ASSERT(!IsAlloc());iType=ETime;return iVal.iTime();}
inline void RSqlLiteral::SetText(const TText* aPtr,const TText* aEnd)
	{__ASSERT(!IsAlloc());iType=EPtr;iVal.iPtr.iPtr=aPtr;iVal.iPtr.iEnd=aEnd;}

// Class TSqlToken
inline TBool TSqlToken::operator==(TSqlTokenType aType) const
	{return iType==aType;}
inline TBool TSqlToken::operator!=(TSqlTokenType aType) const
	{return iType!=aType;}
inline TSqlTokenType TSqlToken::operator=(TSqlTokenType aType)
	{iType=aType;return aType;}
inline void TSqlToken::SetError(TInt aError)
	{iType=aError;}
inline TInt TSqlToken::Error() const
	{return Min(iType,0);}
inline TSqlTokenType TSqlToken::Type() const
	{return TSqlTokenType(iType);}
inline RSqlLiteral& TSqlToken::Literal()
	{__ASSERT(iType>=ESqlIdentifier&&iType<=ESqlLiteralText);return iLiteral;}
inline const RSqlLiteral& TSqlToken::Literal() const
	{__ASSERT(iType>=ESqlIdentifier&&iType<=ESqlLiteralText);return iLiteral;}

// class TSqlLexer
inline TSqlTokenType TSqlLexer::NextToken(TSqlToken& aToken)
	{return aToken=GetNextToken(aToken);}
inline TSqlTokenType TSqlLexer::SqlError(TInt aError)
	{return TSqlTokenType(aError);}
inline TBool TSqlLexer::IsKeyword(TSqlKeyword aKeyword,const TSqlToken& aToken)
	{return aToken==ESqlIdentifier && CompareKeyword(aKeyword,aToken.Literal())==0;}

// class TSqlParser
inline TInt TSqlParser::Error() const
	{return iToken.Error();}

// Class CSqlBoundNode
inline TDbColumnC CSqlBoundNode::Column() const
	{return TDbColumnC(*iSource,iColumn.iBound.iNumber);}

// type-matching compiler support
inline TUint8* PtrType(const TDesC8&)
	{return 0;}
inline TUint16* PtrType(const TDesC16&)
	{return 0;}
