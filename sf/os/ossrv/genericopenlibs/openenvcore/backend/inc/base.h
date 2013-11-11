/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for all objects created by backend
*
*/


#ifndef BASE_H
#define BASE_H

#include "sysif.h"

class BBase : public CBase
	{
public:
	/*
	Default constructor
	*/
	inline BBase()	{}
	virtual ~BBase() { }
	
	/* *****************************************************************
	Overloading new and delete operators so that they will
	allocate and deallocare memory from/to the private heap of backend
	********************************************************************/
	inline TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW
		{
		Mem::FillZ(aBase, aSize); return aBase;
		}
		
	inline TAny* operator new(TUint aSize) __NO_THROW
		{
		return Backend()->Alloc(aSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave)
		{
		TAny* ptr = Backend()->Alloc(aSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
		
	inline TAny* operator new(TUint aSize, TUint aExtraSize) __NO_THROW
		{
		return Backend()->Alloc(aSize + aExtraSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave, TUint aExtraSize)
		{
		TAny* ptr = Backend()->Alloc(aSize + aExtraSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
		
/*corresponding overloaded delete operators	*/

	inline void operator delete(TAny *aPtr, TAny*) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
		
	inline void operator delete(TAny *aPtr) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
		
	inline void operator delete(TAny *aPtr, TLeave) 
		{
		Backend()->Free( aPtr );
		}
		
	inline void operator delete(TAny *aPtr, TUint) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
		
	inline void operator delete(TAny *aPtr, TLeave, TUint) 
		{
		Backend()->Free( aPtr );
		}
		
	static void Delete(BBase* aPtr)
		{
		delete aPtr;
		}
		
/*
protected:
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
		{
		a0 = NULL;
		return KErrExtensionNotSupported;
		}*/
		
private:
	BBase(const BBase&);
	BBase& operator=(const CBase&);
	};


#endif //BASE_H
