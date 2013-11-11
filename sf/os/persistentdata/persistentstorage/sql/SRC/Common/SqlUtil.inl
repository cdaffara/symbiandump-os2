// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 
 SQL server message code maker.
 
 The function accepts as arguments the server function code, handle type and handle,
 and makes from them a 32-bit value, which is used as a function code in RMessage2 objects.
 
 @param aFunction   function code
 @param aHandleType handle type - one of TSqlSrvHandleType enum item values
 @param aHandle     handle
 
 @return The assembled from aFunction, aHandleType and aHandle message code which will be sent
 to the SQL server.
 
 @see RMessage2
 @see TSqlSrvFunction
 
 @internalComponent
*/
inline TInt MakeMsgCode(TSqlSrvFunction aFunction, TSqlSrvHandleType aHandleType, TInt aHandle)
	{
	return (aHandleType | (aHandle << KSqlSrvHandleShiftBits) | aFunction);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                  Case insensitive string comparisons                                     ////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Used for comparing database names, table names, column names and parameter names when the text encoding is UTF8.

@internalComponent
*/
inline TInt CompareNoCase8(const TDesC8& aLeft, const TDesC8& aRight)
	{
	return aLeft.CompareF(aRight);
	}
	
/**
Used for comparing database names, table names, column names and parameter names when the text encoding is UTF16.

@internalComponent
*/
inline TInt CompareNoCase16(const TDesC16& aLeft, const TDesC16& aRight)
	{
	return aLeft.CompareF(aRight);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                      Buffer alignment functions                                     /////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Returns aLen 8-byte aligned.

@param aLen Length value which needs alignment

@return 8-byte aligned aLen value

@internalComponent
*/
inline TInt AlignedLen8(TInt aLen)
	{
	return (aLen + 7) & (TUint32)~0x07;
	}
	
#ifdef _DEBUG
/**
Returns true if aLen is 8-byte aligned
The function is implemented only in _DEBUG releases.

@param aLen Length value which will be checked

@return True if aLen is 8-byte aligned

@internalComponent
*/
inline TBool IsAligned8(TInt aLen)
	{
	return (aLen & 0x07) == 0;	
	}
#endif
