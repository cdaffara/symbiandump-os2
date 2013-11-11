// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Define the template function for instantiating the implementation object 
// 
//

/**
 @file
 @internalComponent
 
 Get NewL pointer and implementation table in preparation for instantiating the implementation object
 @param			aUniqueImplementationUid The implementation to find.
 @param			aImplementationTableEntry An entry in implementation table that match the implementation uid.
 @param			aProxy Contains the proxy
 @return			TAny* The NewL pointer for instantiating the implementation object.
*/
template<typename TImpProxy,typename T> TAny* CLoadManager::GetNewLPointerAndProxyTableRowL(TUid aUniqueImplementationUid,
										 TImpProxy*& aImplementationTableEntry,
										 const T aProxy)
	{
	TInt count = 0;
	
	// Get the implementation table entry. Note that count gets updated after this call.
	TImpProxy* implementationTable = aProxy(count);
	// proxy function could lie to us and return an invalid pointer
	// Is there a method in kernel to see if the address is valid?
	if (implementationTable==0)
		{
		User::Leave(KErrNotFound);
		}
	TAny* newLpointer=NULL;
	// Scan the returned table for a UID match, and return the associated
	// creation method if found.
	for(TInt i = 0; i < count; ++i)
		{
		if(aUniqueImplementationUid == implementationTable[i].iImplementationUid)
			{
			newLpointer = (TAny*)implementationTable[i].iNewLFuncPtr;
			aImplementationTableEntry = &implementationTable[i];
			break;
			}
		}
		
	if(!newLpointer)
		{
		User::Leave(KErrNotFound);
		}

	return newLpointer;
	}
