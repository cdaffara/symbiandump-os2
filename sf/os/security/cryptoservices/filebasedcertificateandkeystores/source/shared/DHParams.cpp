/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CDHParams.cpp
*
*/


#include "fsdatatypes.h"

EXPORT_C CDHParams::~CDHParams()
	{
	iN.Close();
	iG.Close();
	}

EXPORT_C CDHParams::CDHParams(const RInteger aN, const RInteger aG)
	: iN(aN), iG(aG)
	{
	}

EXPORT_C CDHParams* CDHParams::NewL(const TInteger& aN, const TInteger& aG)
	{
	RInteger n = RInteger::NewL(aN);
	CleanupStack::PushL(n);
	RInteger g = RInteger::NewL(aG);
	CleanupStack::PushL(g);
	CDHParams* self = new (ELeave) CDHParams(n, g);
	CleanupStack::Pop(2); // g, n
	return self;
	}

EXPORT_C RInteger CDHParams::TakeN()
	{
	RInteger result = iN;
	iN = RInteger();
	return result;
	}

EXPORT_C RInteger CDHParams::TakeG()
	{
	RInteger result = iG;
	iG = RInteger();
	return result;
	}

