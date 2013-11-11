// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of the TEComResolverParams class.
// 
//

#ifndef __ECOMRESOLVERPARAMS_H__
#define __ECOMRESOLVERPARAMS_H__

#include <e32base.h>

/**
Used to pass values to a Resolver to aid in the filtering of Interface 
Implementation plugins. These values are considered match characteristics used 
during resolution of these Interface Implementation. It is a class used
in the ECOM API provided by the REComSession class.
This class DOES NOT own the descriptor data iDataType points to and so it will 
not release the data on descruction. This is the responsibility of the user of 
this class.

@see REComSession
@publishedAll
@released
*/

class TEComResolverParams
	{
public:
	
	inline TEComResolverParams();
	
	
	inline const TDesC8& DataType() const;
	
	
	inline void SetDataType(const TDesC8& aDataType);
	
	inline TBool IsGenericMatch() const;
	inline void SetGenericMatch(TBool aGenericMatch);
	
	
	inline TBool IsWildcardMatch() const;
	
	
	inline void SetWildcardMatch(TBool aWildcardMatch);

private:
	/** The Interface Implementation plugin 'datatype' match descriptor.*/
	
	TPtrC8 iDataType;
	
	/** The flag to show if generic matching is enabled */
	TBool iGenericMatch;
	};


#include <ecom/ecomresolverparams.inl>

#endif	// __ECOMRESOLVERPARAMS_H__
