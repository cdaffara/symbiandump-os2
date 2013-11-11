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

#ifndef __MCTTOKENTYPE_H__
#define __MCTTOKENTYPE_H__

#include <f32file.h>
#include <ct/rcpointerarray.h>

class CCTTokenTypeInfo;
class MCTToken;
class TCTTokenHandle;


/** 
 * A token type.
 *
 * This abstract class is instantiated using the ECom Plug-in Architecture 
 * for a particular token type. It allows a list of available tokens of that 
 * type to be obtained, and particular tokens to be opened.
 * 
 * All implementation classes are in fact a subclass of CCTTokenType; refer to 
 * its documentation for implementation details. Client access is entirely through 
 * this M class.
 * 
 * The difference betweeen a token type and a token is best explained with an 
 * example. Suppose a device has two identical WIM slots, the code to handle WIMs 
 * is one token type, which will have two tokens for the two WIMs. 
 *
 * @since v7.0 
 */
 class MCTTokenType
	{
 public:
	// Static constructor functions that use ECom to load the actual implementation.
	/** 
	 * Creates a MCTTokenType given it's CCTTokenTypeInfo
	 *
	 * The definition of this inline function is in CCTTokenType.h.  If you call
	 * this function, you need to link against ctfinder.dll as well as
	 * ctframework.dll
	 *
	 * @param aInfo	Information about the token type.
	 * @param aFs	An open file server session.
	 * @return		The new token type object. 
	 */
	inline static MCTTokenType* NewL(const CCTTokenTypeInfo& aInfo, RFs aFs);
	
	/** 
	 * Creates a MCTTokenType given the UID of the token type. 
	 *
	 * The definition of this inline function is in CCTTokenType.h.  If you call
	 * this function, you need to link against ctfinder.dll as well as
	 * ctframework.dll
	 *
	 * @param aUID	The UID of the token type.
	 * @param aFs	An open file server session.
	 * @return		A new token type object. 
	 */
	inline static MCTTokenType* NewL(TUid aUID, RFs aFs);
	
	/** 
	 * Lists all the tokens of this type.
	 *
	 * This is an asynchronous request.
	 *
	 * Tokens are defined by name, so instead of returning a CCTTokenTypeInfo object, 
	 * this function just gives a list of pointers to HBufC objects.
	 *
	 * @param aTokens	On return, a list of all the tokens.
	 * @param aStatus	The request status object; contains the result of the List() 
	 * 					request when complete. Set to KErrCancel if an outstanding request is cancelled. 
	 */
	virtual void List(RCPointerArray<HBufC>& aTokens, 
					  TRequestStatus& aStatus) = 0;
	
	/** 
	 * Cancels an asynchronous List() operation.
	 *
	 * The operation completes with KErrCancel. 
	 */
	virtual void CancelList() = 0;
	
	/** 
	 * Opens the specified token.	
	 *
	 * @param aTokenInfo	Information about the required token.
	 * @param aToken		On return, the token to be opened.
	 * @param aStatus		The request status object; contains the result of the OpenToken() 
	 * 						request when complete. Set to KErrCancel if an outstanding request is cancelled. 
	 */
	virtual void OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, 
						   TRequestStatus& aStatus) = 0;
	
	/** 
	 * Opens the specified token. 	
	 *
	 * @param aHandle	The handle of the token.
	 * @param aToken	On return, the token to be opened.
	 * @param aStatus	The request status object; contains the result of the OpenToken() 
	 * 					request when complete. Set to KErrCancel if an outstanding request is cancelled. 
	 */
	virtual void OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, 
						   TRequestStatus& aStatus) = 0;

	/** 
	 * Cancels an asynchronous OpenToken() operation.
	 *
	 * The operation completes with KErrCancel. 
	 */
	virtual void CancelOpenToken() = 0;
	
	/** 
	 * Releases the token type object.
	 *
	 * To be called when you have finished with the object.
	 *
	 * The API does not allow the destructor to be directly called as this object 
	 * could remain in existence for longer to hold onto the ECom handle on the DLL; 
	 * for instance, it may not be deleted until all tokens and interfaces have also 
	 * been released. 
	 */
	virtual void Release() = 0;

	/** 
	 * Gets the UID of the token type.
	 *
	 * This function is implemented by CCTTokenType, so token type implementers need 
	 * not implement it.
	 *
	 * @return	The UID of the token type. 
	 */
	virtual TUid Type() const = 0;

	/** 
	 * Gets the label of the token type.
	 *
	 * This function is implemented by CCTTokenType, so token type implementers need 
	 * not implement it.
	 *
	 * @return	The label to the token type. 
	 */
	virtual const TDesC& Label() const = 0;
	};

#endif //__MCTTOKENTYPE_H__
