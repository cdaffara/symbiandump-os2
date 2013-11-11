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
// Comparison functions used for SQl and indexing
// 
//

#ifndef __D32COMP_H__
#define __D32COMP_H__

#ifndef __D32DBAS_H__
#include "D32DBAS.H"
#endif

/**
@internalComponent
*/
class Comp
	{
public:
	static inline TInt Compare(TInt32 aLeft,TInt32 aRight);
	static inline TInt Compare(TUint32 aLeft,TUint32 aRight);
	static TInt Compare(TReal32 aLeft,TReal32 aRight);
	static TInt Compare(TReal64 aLeft,TReal64 aRight);
	static TInt Compare(const TInt64& aLeft,const TInt64& aRight);
	static inline TInt Compare(const TTime& aLeft,const TTime& aRight);
//
	static TInt Compare8L(MStreamBuf& aBufL,MStreamBuf& aBufR,TInt aSize,const TTextOps& aTextOp);
	static TInt Compare8L(MStreamBuf& aBuf,const TText8* aPtr,TInt aSize,const TTextOps& aTextOp);
	static TInt Compare8L(MStreamBuf& aBuf,TInt aSize,const TDesC8& aDes,const TTextOps& aTextOp);
//
	static TInt Compare16L(MStreamBuf& aBufL,MStreamBuf& aBufR,TInt aSize,const TTextOps& aTextOp);
	static TInt Compare16L(MStreamBuf& aBuf,const TText16* aPtr,TInt aSize,const TTextOps& aTextOp);
	static TInt Compare16L(MStreamBuf& aBuf,TInt aSize,const TDesC16& aDes,const TTextOps& aTextOp);
	};

// class Comp
inline TInt Comp::Compare(TInt32 aLeft,TInt32 aRight)
	{
	return aLeft<aRight ? -1 : aLeft!=aRight;
	}
	
inline TInt Comp::Compare(TUint32 aLeft,TUint32 aRight)
	{
	return aLeft<aRight ? -1 : aLeft!=aRight;
	}
	
inline TInt Comp::Compare(const TTime& aLeft,const TTime& aRight)
	{
	return Compare(aLeft.Int64(),aRight.Int64());
	}

#endif
