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

#ifndef __TCTFINDTOKENTYPESBYINTERFACEANDATTRIBUTE_H__
#define __TCTFINDTOKENTYPESBYINTERFACEANDATTRIBUTE_H__

#include <ct/mcttokentypefilter.h>


struct TCTTokenTypeAttribute;


/** 
 * A filter that returns only those token types that match the required set of 
 * interfaces and attributes.
 * 
 * On construction, an array containing the required interfaces and an array 
 * of required attributes/values is passed in.
 * 
 * Token types will be matched if every interface in the supplied array is supported 
 * by the token type, and every attribute is defined and has the same value. 
 *
 * @since v7.0 
 */
class TCTFindTokenTypesByInterfaceAndAttribute : public MCTTokenTypeFilter
	{
 public:
	/** 
	 * Constructor.
	 *	
	 * @param aInterfaces	An array of Uids corresponding to the interfaces that the 
	 * 						token type must support.
	 * @param aAttributes	An array of the attributes and values that must be defined 
 	 * 						for the token type. 
 	 */
	IMPORT_C TCTFindTokenTypesByInterfaceAndAttribute(const TArray<TUid> aInterfaces,
													  const TArray<TCTTokenTypeAttribute> aAttributes);

	/** 
	 * Tests whether all the interfaces and attributes associated with the filter 
	 * are supported by the token type.
	 *
	 * @param aTokenType	The token type to be tested.
	 * @return				ETrue if all the interfaces and attributes associated with the  
	 * 						filter are supported by the token type; EFalse, otherwise. 
	 */
	virtual TBool Accept(const CCTTokenTypeInfo& aTokenType) const;

 private:
	const TArray<TUid> iInterfaces;
	const TArray<TCTTokenTypeAttribute> iAttributes;
	};


#endif // __TCTFINDTOKENTYPESBYINTERFACEANDATTRIBUTE_H__
