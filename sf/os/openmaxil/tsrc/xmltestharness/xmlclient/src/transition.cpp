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


#include "transition.h"
#include <openmax/il/khronos/v1_x/OMX_Component.h>

void CloseSupplierRelations(TAny* aPtr)
	{
	RSupplierRelations& links = *reinterpret_cast<RSupplierRelations*>(aPtr);
	for(TInt i = 0, count = links.Count(); i < count; i++)
		{
		links[i].Close();
		}
	links.Close();
	}

/**
 * Finds a transition order in which no component is transitioned until all components it suppliers to have been transitioned.
 * If no such order exists (this happens if and only if a cyclic buffer supply chain exists in the graph), this method leaves
 * with KErrNotFound.
 *
 * The result of this algorithm is appropriate for transitioning components from the Loaded state to the Idle state without incurring
 * errors calling OMX_UseBuffer. For transitions Executing->Idle and Executing->Paused, the reverse order can be used.
 *
 * @leave KErrNoMemory
 * @leave KErrNotFound iff a transition order for the graph does not exist
 * @leave KErrArgument if aOrder passed in is not empty
 */
void FindTransitionOrderL(const RSupplierRelations& aSupplierRelations, RArray<TInt>& aOrder)
	{
	if(aOrder.Count() > 0)
		{
		User::Leave(KErrArgument);
		}
	
	TInt numComponents = aSupplierRelations.Count();
	TInt numPendingComponents = numComponents;
	RArray<TBool> pendingComponents;
	CleanupClosePushL(pendingComponents);
	for(TInt i = 0; i < numComponents; i++)
		{
		pendingComponents.AppendL(ETrue);
		}

	while(numPendingComponents > 0)
		{
		TInt nextComponent = KErrNotFound;
		for(TInt i = 0; i < numComponents && nextComponent == KErrNotFound; i++)
			{
			// try each component not yet transitioned
			if(pendingComponents[i])
				{
				// have all this component's non-supplier peers been transitioned?
				nextComponent = i;
				for(TInt j = 0, numSupplied = aSupplierRelations[i].Count(); j < numSupplied && nextComponent != KErrNotFound; j++)
					{
					if(pendingComponents[aSupplierRelations[i][j]])
						{
						// no, this component can't transition yet
						nextComponent = KErrNotFound;
						}
					}
				}
			}
		// couldn't find a component to transition? so no solution
		if(nextComponent == KErrNotFound)
			{
			aOrder.Reset();
			User::Leave(KErrNotFound);
			}

		// this component can transition
		aOrder.AppendL(nextComponent);
		pendingComponents[nextComponent] = EFalse;
		numPendingComponents--;
		}

	CleanupStack::PopAndDestroy(&pendingComponents);
	}
