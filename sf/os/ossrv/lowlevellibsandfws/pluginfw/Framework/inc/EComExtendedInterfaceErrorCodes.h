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
// All the ECOM Extended Intefcae specific error codes returned by the ECOM API to the phone manufacturers
// are defined here.
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __ECOMEXTENDEDINTERFACEERRORCODES_H__
#define __ECOMEXTENDEDINTERFACEERRORCODES_H__

//used to get the base value for the error codes
#include <ecom/ecomerrorcodes.h>


/**
An attempt was made to release an extended object, or the extended interface
indicated that ECOM should perform automatic release but no release function
pointer existed.
@publishedPartner
@released
*/
const TInt KEComErrNoExtendedInterfaceReleaseFunction = -17031;

/**
An attempt was made to release an extended object, or the extended interface
indicated that ECOM should perform automatic release but no release object
existed.
@publishedPartner
@released
*/
const TInt KEComErrNoExtendedInterfaceReleaseObject = -17032;


#endif	// __ECOMEXTENDEDINTERFACEERRORCODES_H__
