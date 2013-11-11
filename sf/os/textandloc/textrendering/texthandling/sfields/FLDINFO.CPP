/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>

#include "FLDINFO.H"


 
EXPORT_C TBool TFindFieldInfo::operator==(const TFindFieldInfo& aInfo)const
// Equality operator.
//
/** Compares two field information objects for equality.

@param aInfo The field information object to compare with this one. 
@return ETrue if the two objects are the same. EFalse if they are different. */
	{
	if (iFieldCountInRange!=aInfo.iFieldCountInRange)
		return EFalse;
	if (iFirstFieldLen!=aInfo.iFirstFieldLen)
		return EFalse;
	if (iFirstFieldPos!=aInfo.iFirstFieldPos)
		return EFalse;
	return ETrue;
	}


 

EXPORT_C TBool TFindFieldInfo::operator!=(const TFindFieldInfo& aInfo)const
// Inequality operator.
//
/** Compares two field information objects for inequality.

@param aInfo The field information object to compare with this one. 
@return ETrue if the two objects are different. EFalse if they are the same. */
	{return !(operator==(aInfo));}


