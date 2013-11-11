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
* Description: This file contains the header file of the STIF util
* class RRefArray.
*
*/

#ifndef RREFARRAY_H
#define RREFARRAY_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION
/**
* Array class for references.
*/
template <class T>
class RRefArray
    :public RPointerArrayBase
    {
    public:
        inline RRefArray():RPointerArrayBase(){};
    	inline void Close(){ RPointerArrayBase::Close(); };
    	inline TInt Count() const{ return RPointerArrayBase::Count(); };
    	inline T const& operator[](TInt anIndex) const 
    	    { return *( ( T* ) At(anIndex)); };
    	inline T& operator[](TInt anIndex) 
    	    { return *( ( T* ) At(anIndex)); };
    	inline TInt Append( T const& anEntry )
    	    { return RPointerArrayBase::Append(( TAny*) &anEntry); };
    	inline TInt Insert( T const& anEntry, TInt aPos )
    	    { return RPointerArrayBase::Insert(( TAny*) &anEntry, aPos ); };
    	inline void Remove( TInt anIndex ) 
    	    { RPointerArrayBase::Remove( anIndex ); };
    	inline void Reset(){ RPointerArrayBase::Reset(); };
    private:
        RArray<TAny*> iArray;
    };


#endif // RREFARRAY_H

// End of File
