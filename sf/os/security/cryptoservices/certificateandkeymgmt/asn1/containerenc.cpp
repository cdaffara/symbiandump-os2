/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Methods common to all container-data-type encoding classes
*
*/


#include "panic.h"

#include <asn1enc.h>

CASN1EncContainer::CASN1EncContainer(const TTagType aType)
: CASN1EncConstructed(aType, EUniversal)
	{
	}


EXPORT_C void CASN1EncContainer::AddChildL(CASN1EncBase* aChild)
	{
	__ASSERT_ALWAYS(aChild, Panic(KErrArgument));

	AddChildIntL(aChild);
	aChild->SetParent(this);

	ContentsLengthChanged();
	}

EXPORT_C void CASN1EncContainer::AddAndPopChildL(CASN1EncBase* aChild)
	{
	AddChildL(aChild);
	CleanupStack::Pop(aChild);
	}
