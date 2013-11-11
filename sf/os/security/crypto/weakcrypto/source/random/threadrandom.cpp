/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <random.h>

_LIT(KThreadRandom, "threadrandom.cpp");

EXPORT_C void SetThreadRandomL(CRandom* aRNG)
	{
	User::LeaveIfError(Dll::SetTls(aRNG));	
	}

EXPORT_C void SetThreadRandomLC(CRandom* aRNG)
	{
	CleanupStack::PushL(aRNG);
	SetThreadRandomL(aRNG);
	//This pop before the push isn't a problem as the PushL can't fail. 
	//We just did a push before this and thus there is enough room on the
	//cleanupstack such that OOM is not possible.
	CleanupStack::Pop(aRNG);
	CleanupStack::PushL(TCleanupItem(&DeleteThreadRandom, Dll::Tls()));
	}

void DeleteThreadRandom(TAny* aPtr)
	{
	CRandom* random = reinterpret_cast<CRandom*>(aPtr);
	delete random;
	TInt result = Dll::SetTls(0);
	__ASSERT_ALWAYS(result == 0, User::Panic(KThreadRandom, 1));
	}

EXPORT_C void DestroyThreadRandom(void)
	{
	delete (CRandom*)(Dll::Tls());
	TInt result = Dll::SetTls(0);
	__ASSERT_ALWAYS(result == 0, User::Panic(KThreadRandom, 1));
	}

EXPORT_C void GenerateRandomBytesL(TDes8& aDest)
	{
	TAny* tls = Dll::Tls();
	if(tls)
		{
		((CRandom*)tls)->GenerateBytesL(aDest);
		}
	else
		{
		TRandom::RandomL(aDest);
		}
	}
