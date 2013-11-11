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

#include <s32crypt.h>
#include "U32STD.H"

NONSHARABLE_CLASS(HEncryptFilter) : public TEncryptFilter
	{
public:
	virtual inline ~HEncryptFilter();
private:
	void DoRelease();
	};
NONSHARABLE_CLASS(HDecryptFilter) : public TDecryptFilter
	{
public:
	virtual inline ~HDecryptFilter();
private:
	void DoRelease();
	};
//
enum TEncryptionPanic
	{
	ECryptNoKey,
	ECryptBadKeyType
	};

GLREF_C void Panic(TEncryptionPanic aPanic);

#include "UE_STD.INL"

