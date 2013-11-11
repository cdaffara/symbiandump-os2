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
*
*/


#include "fsdatatypes.h"

EXPORT_C CKeyInfo* CKeyInfo::NewL(RReadStream& aStream)
	{
	CKeyInfo* me = new (ELeave) CKeyInfo();
	me->CleanupPushL();
	me->ConstructL(aStream);
	CleanupStack::Pop(me);
	return me;
	}

EXPORT_C void CKeyInfo::SetUsePolicy(const TSecurityPolicy& aPolicy)
	{
	iUsePolicy = aPolicy;
	}

EXPORT_C void CKeyInfo::SetManagementPolicy(const TSecurityPolicy& aPolicy)
	{
	iManagementPolicy = aPolicy;
	}

EXPORT_C void CKeyInfo::SetPKCS8AttributeSet(HBufC8* aPKCS8AttributeSet)
	{
	delete iPKCS8AttributeSet;
	iPKCS8AttributeSet = aPKCS8AttributeSet;
	}
