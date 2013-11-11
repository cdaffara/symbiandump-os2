/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <e32base.h>

/** RSupplierRelations represents the buffer supplier links in a graph */
typedef RArray<RArray<TInt> > RSupplierRelations;

void CloseSupplierRelations(TAny* aPtr);

/*

For example, consider the following graph: (<-- indicates direction of buffer supply)

	| 0 | <-- 1 <-- 2
	| 0 | --> 3 <-- 4 --> 5
	  
	The supplier relations table is:
	
	0: 3
	1: 0
	2: 1
	3:
	4: 3, 5
	5:
	
	A valid transition order for this graph is { 3, 0, 1, 2, 5, 4 }

*/

void FindTransitionOrderL(const RSupplierRelations& aSupplierRelations, RArray<TInt>& aOrder);

#endif /*TRANSITION_H_*/
