// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// A simple ECOM interface for testing purpose.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __EXAMPLENINE_H__
#define __EXAMPLENINE_H__

#include <ecom/ecom.h>

const TUid KRogueInterfaceUid = {0x00001001};
const TInt32 KRogueImplUidValue = 0x10009DC3;
const TUid KRogueImplUid = {KRogueImplUidValue};

/** A dummy plugin for testing purpose */
NONSHARABLE_CLASS(CRoguePlugin) : public CBase
{
public:
	static CRoguePlugin* NewL(TAny*);
	virtual ~CRoguePlugin();

	// a virtual function to test this interface
	virtual TUid InterfaceUid();

private:
	CRoguePlugin();

private:
	TUid iDtorID;
};

#endif // __EXAMPLENINE_H__
