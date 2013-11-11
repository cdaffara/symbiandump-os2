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
// SQL Literal value and node classes
// 
//

#include "UQ_STD.H"

// internalising literal strings, overload selectors

inline HBufC8* Alloc8L(const TDesC8& aDes)
	{return aDes.AllocL();}
inline HBufC8* Alloc8L(const TDesC16& aDes)
	{HBufC8* cell=HBufC8::NewL(aDes.Length());cell->Des().Copy(aDes);return cell;}
inline HBufC16* Alloc16L(const TDesC16& aDes)
	{return aDes.AllocL();}
inline HBufC16* Alloc16L(const TDesC8& aDes)
	{HBufC16* cell=HBufC16::NewL(aDes.Length());cell->Des().Copy(aDes);return cell;}

template <class B,class T>
inline B* DoInternString(B* aBuf,T*)
	{
	const T* base=aBuf->Ptr();
	const T* from=base;
	const T* end=base+aBuf->Length();
	for (;;)
		{
		if (from==end)
			return aBuf;
		if (*from++=='\'')
			break;
		}
	T* to=(T*)from;
	++from;
	while (from<end)
		{
		T c=*from++;
		if (c=='\'')
			++from;
		*to++=c;
		}
	aBuf->Des().SetLength(to-base);
	return aBuf;
	}

template <class B>
inline B* InternString(B* aBuf)
	{return DoInternString(aBuf,PtrType(*aBuf));}

// Class RSqlLiteral

void RSqlLiteral::Close()
//
// Free any allocated cell
//
	{
	if(iBuffer)
		delete iBuffer;
	if (IsAlloc())
		User::Free(iVal.iAlloc);

		
	}

void RSqlLiteral::ToInt32L()
//
// Convert a numeric type to Int32
//
	{
	ToInt64L();
	const TInt64& v=iVal.iInt64;
	TInt32 l = I64LOW(v);
	TInt32 h = I64HIGH(v);
	if (h==(l>>31))	// domain check, in signed 32-bit range
		{
		iVal.iInt32=l;
		iType=EInt32;
		}
	else
		__LEAVE(KErrOverflow);	// cannot convert
	}

void RSqlLiteral::ToUint32L()
//
// Convert a numeric type to Uint32
//
	{
	ToInt64L();
	const TInt64& v=iVal.iInt64;
	if (I64HIGH(v)==0)		// domain check, in unsigned 32-bit range
		{
		iVal.iUint32 = I64LOW(v);
		iType=EUint32;
		}
	else
		__LEAVE(KErrOverflow);	// cannot convert
	}

void RSqlLiteral::ToInt64L()
//
// Convert a numeric type to int64
//
	{
	__ASSERT(IsBasic());	// only apply conversions once
	TType t=iType;
	if (t==EReal64)
		{
		iVal.iInt64() = static_cast<TInt64>(Real64());
		iType=EInt64;
		}
	else if (t!=EInt64)
		__LEAVE(KErrGeneral);
	}

void RSqlLiteral::ToReal64L()
//
// Convert a numeric type to real64
//
	{
	__ASSERT(IsBasic());	// only apply conversions once
	TType t=iType;
	if (t==EInt64)
		{
		iVal.iReal64=I64REAL(Int64());
		iType=EReal64;
		}
	else if (t!=EReal64)
		__LEAVE(KErrGeneral);
	}

void RSqlLiteral::ToReal32L()
//
// Convert a numeric type to real32
//
	{
	ToReal64L();
	__LEAVE_IF_ERROR(TRealX(iVal.iReal64).GetTReal(iVal.iReal32));
	iType=EReal32;
	}

void RSqlLiteral::ToTimeL()
//
// Ensure we are a time
//
	{
	__ASSERT(IsBasic());	// only apply conversions once
	if (iType!=ETime)
		__LEAVE(KErrGeneral);	// type mismatch
	}

void RSqlLiteral::ToText8L()
//
// Convert a ptr to a text8
//
	{
	__ASSERT(IsBasic());	// only apply conversions once
	if (iType==EPtr)
		{
		iVal.iAlloc=InternString(Alloc8L(DesC()));
		iType=EBuf8;
		}
	else
		__LEAVE(KErrGeneral);	// type mismatch
	}

void RSqlLiteral::ToText16L()
//
// Convert a ptr to a text8
//
	{
	__ASSERT(IsBasic());	// only apply conversions once
	if (iType==EPtr)
		{
		iVal.iAlloc=InternString(Alloc16L(DesC()));
		iType=EBuf16;
		}
	else
		__LEAVE(KErrGeneral);	// type mismatch
	}



// class CSqlLiteralNode

CSqlLiteralNode::CSqlLiteralNode(TType aType,const TDesC& aColumn,const RSqlLiteral& aLiteral)
	:CSqlBoundNode(aType,aColumn),iLiteral(aLiteral)
	{}

CSqlLiteralNode::~CSqlLiteralNode()
	{
	iLiteral.Close();
	}

void CSqlLiteralNode::BindL(const RDbTableRow& aSource)
	{
	CSqlBoundNode::BindL(aSource);
	switch (ColType())
		{
	default:	// type not allowed in evaluation expressions
		__LEAVE(KErrGeneral);
		break;
	case EDbColBit:
	case EDbColInt8:
	case EDbColUint8:
	case EDbColInt16:
	case EDbColUint16:
	case EDbColInt32:
	case EDbColUint32:
	case EDbColInt64:
		iLiteral.ToInt64L();
		break;
	case EDbColReal32:
	case EDbColReal64:
		iLiteral.ToReal64L();
		break;
	case EDbColDateTime:
		iLiteral.ToTimeL();
		break;
	case EDbColText8:
	case EDbColLongText8:
		iLiteral.ToText8L();
		break;
	case EDbColText16:
	case EDbColLongText16:
		iLiteral.ToText16L();
		break;
		}
	}
// copy text to buffer for LIKE Escape support
TInt RSqlLiteral::CopyText()
	{
	if (iType==EPtr)
		{
		TInt length = iVal.iPtr.iEnd - iVal.iPtr.iPtr;
		if(iBuffer)
			delete iBuffer;
		if((iBuffer=HBufC::New(length+1)) == NULL)
	        	{
			return KErrNoMemory;
			}
		iBuffer->Des().Copy(iVal.iPtr.iPtr,length);
		iVal.iPtr.iPtr= &iBuffer->Des()[0];
		iBuffer->Des().Append(iVal.iPtr.iEnd,1);
		iVal.iPtr.iEnd=&iBuffer->Des()[length]; 
		}
	else
		return KErrGeneral;	// type mismatch
	return KErrNone;
	}

