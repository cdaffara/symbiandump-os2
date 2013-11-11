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
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CCTTOKENTYPEINFO_H__
#define __CCTTOKENTYPEINFO_H__

#include <ct/tctalltokentypes.h>
#include <ct/tcttokentypeattribute.h>
#include <ct/rcpointerarray.h>
class CImplementationInformation;


/** 
 * Information about a token type.
 *
 * A list of CCTTokenTypeInfo objects can be obtained using the static ListL() 
 * functions. Each CCTTokenTypeInfo object has a list of interfaces it supports, 
 * a list of TCTTokenTypeAttribute objects associated with it, and a label. Based 
 * on this, the client (or the user via a UI) can chose the required token type 
 * and load it via CCTTokenType::NewL(). 
 * 
 * @since v7.0 
 */
class CCTTokenTypeInfo : public CBase
	{
 public:
	
	/** 
	 * Lists all the CCTTokenTypeInfo objects that match the filter expression.
	 *
	 * This is the only way of creating CCTTokenTypeInfo objects. 
 	 *
	 * @param aTokenTypes	An array to which the found token types will be appended. 
	 * 						Ownership of the CCTTokenTypeInfo objects, whose pointers 
	 *						are held by the array, are transferred to the caller.
	 * @param aFilter		An object that is used to filter tokens, by being offered each 
	 *						token in turn and declining the ones that are not required. 
	 */
	IMPORT_C static void ListL(RCPointerArray<CCTTokenTypeInfo>& aTokenTypes,
							  const MCTTokenTypeFilter& aFilter);

	/** 
	 * Lists all the CCTTokenTypeInfo objects.
	 *
	 * This version does not take a filter, but returns all token types.
	 *
	 * @param aTokenTypes	An array to which the found token types will be appended. 
	 * 						Ownership of the CCTTokenTypeInfo objects, whose pointers 
	 *						are held by the array, are transferred to the caller. 
	 */
	IMPORT_C static void ListL(RCPointerArray<CCTTokenTypeInfo>& aTokenTypes);
	
	/** 
	 * Destructor.
	 *
	 * Frees all resources owned by the object. 
	 * @internalComponent
	 */
	~CCTTokenTypeInfo();

	/** 
	 * Gets a list of all interfaces supported by this token type.
	 *
	 * @return	A list of all the interfaces supported by this token type. 
	 */
	IMPORT_C const RArray<TUid>& Interfaces() const;

	/** 
	 * Gets a list of all attributes of this token type.
	 *
	 * @return	A list of all attributes of this token type. 
	 */
	IMPORT_C const RArray<TCTTokenTypeAttribute>& Attributes() const;

	/** 
	 * Gets a human-readable name for the token type.
	 *
	 * @return	A human-readable name for the token type. 
	 */
	IMPORT_C const TDesC& Label() const;

	/** 
	 * Gets the UID of this token type.
	 *
	 * @return	The UID of this token type. 
	 */
	IMPORT_C TUid Type() const;

 private:
	/** Constructor */
	CCTTokenTypeInfo(CImplementationInformation* aEcomImplementationInfo);

 private:
	CImplementationInformation* iEcomImplementationInfo;
	RArray<TUid> iInterfaces;
	RArray<TCTTokenTypeAttribute> iAttributes;
	};

#endif // __CCTTOKENTYPEINFO_H__
