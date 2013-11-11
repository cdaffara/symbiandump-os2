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
// ULogger
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/
#ifndef DATATYPES_INL_
#define DATATYPES_INL_

#include <e32base.h>

namespace Ulogger
{

const TInt KPluginConfigMaxKeySize = 256;
const TInt KPluginConfigMaxValSize = KPluginConfigMaxKeySize;

/**
Class to store 'key - value' data on heap.
*/
class TPluginConfiguration
	{
public:
	void SetKey(const TDesC& aKey)
		{
		iKey.Copy(aKey);
		}

	void SetKey(const TDesC8& aKey)
		{
		iKey.Copy(aKey);
		}

	const TBuf<KPluginConfigMaxKeySize>& Key() const
		{
		return iKey;
		}

	void SetValue(const TDesC& aValue)
		{
		iValue.Copy(aValue);
		}

	void SetValue(const TDesC8& aValue)
		{
		iValue.Copy(aValue);
		}

	const TBuf<KPluginConfigMaxValSize>& Value() const
		{
		return iValue;
		}

private:
    TBuf<KPluginConfigMaxKeySize> iKey;
	TBuf<KPluginConfigMaxValSize> iValue;
	};

	enum //TBufferMode
	{
		ECircularBuffer = 0,
		EStraightBuffer
	};
} //end of namespace
#endif /*DATATYPES_INL_*/
