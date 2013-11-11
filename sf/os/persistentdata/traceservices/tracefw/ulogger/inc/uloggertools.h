// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ULogger plug-in base class
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef __ULOGGERTOOLS_H__
#define __ULOGGERTOOLS_H__

#include <e32base.h>
#include <s32mem.h>

namespace Ulogger
{

//utility templates
/*!Externilize T array to HBufC8 stream.
This is for internal use only.
*/
template<class T> HBufC8* ExternalizeToBufL(const T& aArray, TInt aElementSize)
	{
	CBufFlat* buf = CBufFlat::NewL(1 + aArray.Count()*aElementSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);

	for (TInt i=0; i<aArray.Count(); ++i)
		stream << aArray[i];

	CleanupStack::PopAndDestroy(&stream); //close
	
	HBufC8* des = HBufC8::NewL(buf->Size());
	TPtr8 ptr(des->Des());
	buf->Read(0, ptr, buf->Size());

	CleanupStack::PopAndDestroy(buf);
	return des;
	}	
	
	
	
/*!Internalize stream to array.
This is for internal use only.
*/

template<class T, class T2> void InternalizeFromBufL(const TDesC8& aBuf, T& aArray, T2 aArrayMember)
	{
	TInt elements = (TInt)aBuf.Length()/sizeof(T2);
	RDesReadStream stream(aBuf);
	CleanupClosePushL(stream);
	for(TInt i=0; i<elements; ++i)
		{
		stream >> aArrayMember;
		aArray.AppendL(aArrayMember);
		}
	CleanupStack::PopAndDestroy(&stream); //close
	}

void ResetAndDestroyPtrArray(TAny* aPtr)
	{
	(STATIC_CAST(RPointerArray<HBufC>*,aPtr))->ResetAndDestroy();
	(STATIC_CAST(RPointerArray<HBufC>*,aPtr))->Close();
	}

}

#endif //__ULOGGERTOOLS_H__
