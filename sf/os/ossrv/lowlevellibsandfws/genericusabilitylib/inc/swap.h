// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SWAP_H
#define SWAP_H


/** Function template for exchanging the values stored in two locations */
template<typename T>
void Swap(T& aT1, T& aT2)
	{
	T temp = aT1;
	aT1 = aT2;
	aT2 = temp;
	}


#endif //! SWAP_H

