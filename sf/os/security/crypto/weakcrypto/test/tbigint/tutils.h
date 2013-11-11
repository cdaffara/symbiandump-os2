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


#ifndef __TUTILS_H__
#define __TUTILS_H__

#include "t_testaction.h"
#include <e32std.h>
#include <random.h>

class RInteger;
class Output;

class Utils 
	{
public:
	static void DumpInteger(Output& aOut, const TDesC& aDesc, 
		const RInteger& aThat);
	};

class CRandomSetSource : public CRandom
	{
public:
	CRandomSetSource(const TDesC8& aSource);
	virtual void GenerateBytesL(TDes8& aDest);
	void SetSource(const TDesC8& aSource);
	void Reset(void);

private:
	TPtrC8 iValue;
	TInt iCounter;
	};

#endif
