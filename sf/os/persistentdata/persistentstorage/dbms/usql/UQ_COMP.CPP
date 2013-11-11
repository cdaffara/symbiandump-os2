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
// SQL Comparison predicate node class
// 
//

#include "UQ_STD.H"
#include "D32COMP.H"

// class CSqlCompPredicate

CSqlCompPredicate::CSqlCompPredicate(TType aType,const TDesC& aColumn,const RSqlLiteral& aLiteral)
	: CSqlLiteralNode(aType,aColumn,aLiteral)
	{
	__ASSERT(aType==ELess||aType==ELessEqual||aType==EEqual||aType==EGreaterEqual||aType==EGreater||aType==ENotEqual);
	}

TInt CSqlCompPredicate::CompareLongText8L(const TDbBlob& aBlob,const TTextOps& aTextOp) const
	{
	TInt r=Comp::Compare8L(StreamLC(aBlob),aBlob.Size(),Value().Text8(),aTextOp);
	CleanupStack::PopAndDestroy();
	return r;
	}

TInt CSqlCompPredicate::CompareLongText16L(const TDbBlob& aBlob,const TTextOps& aTextOp) const
	{
	TInt r=Comp::Compare16L(StreamLC(aBlob),aBlob.Size(),Value().Text16(),aTextOp);
	CleanupStack::PopAndDestroy();
	return r;
	}

TBool CSqlCompPredicate::EvaluateL(const TTextOps& aTextOp) const
//
// Evaluate a comparison between the columns and the data
// NULL numeric data always compares false
//
	{
	TDbColumnC col=Column();
	TDbColType t=ColType();
	if (t<=EDbColDateTime && col.IsNull())
		return EFalse;	// NULL always compares false for non-text columns
	TInt r;
	switch (t)
		{
	default:
		__ASSERT(0);
	case EDbColBit:
	case EDbColUint8:
	case EDbColUint16:
	case EDbColUint32:
		r=Comp::Compare(TInt64(TUint(col.Uint32())),Value().Int64());
		break;
	case EDbColInt8:
	case EDbColInt16:
	case EDbColInt32:
		r=Comp::Compare(TInt64(TInt(col.Int32())),Value().Int64());
		break;
	case EDbColInt64:
		r=Comp::Compare(col.Int64(),Value().Int64());
		break;
	case EDbColReal32:
		r=Comp::Compare(TReal(col.Real32()),Value().Real64());
		break;
	case EDbColReal64:
		r=Comp::Compare(col.Real64(),Value().Real64());
		break;
	case EDbColDateTime:
		r=Comp::Compare(col.Time(),Value().Time());
		break;
	case EDbColText8:
		r=aTextOp.Compare(col.PtrC8(),Value().Text8());
		break;
	case EDbColText16:
		r=aTextOp.Compare(col.PtrC16(),Value().Text16());
		break;
	case EDbColLongText8:
		{
		const TDbBlob& blob=col.Blob();
		if (blob.IsInline())
			r=aTextOp.Compare(blob.PtrC8(),Value().Text8());
		else
			r=CompareLongText8L(blob,aTextOp);
		}
		break;
	case EDbColLongText16:
		{
		const TDbBlob& blob=col.Blob();
		if (blob.IsInline())
			r=aTextOp.Compare(blob.PtrC16(),Value().Text16());
		else
			r=CompareLongText16L(blob,aTextOp);
		}
		break;
		}
	switch (NodeType())
		{
	default:
		__ASSERT(0);
	case EGreater:
		r=-r;
		// drop throught to ELess
	case ELess:
		return r<0;
	case EGreaterEqual:
		r=-r;
		// drop throught to LessEqual
	case ELessEqual:
		return r<=0;
	case EEqual:
		return r==0;
	case ENotEqual:
		return r!=0;
		}
	}
