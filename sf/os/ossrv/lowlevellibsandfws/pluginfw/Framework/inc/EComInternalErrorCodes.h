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
// These Error Codes are used Internally within ECOM
// 
// 
//

/**
 @file
 @InternalTechnology
 @released
*/

#ifndef ECOMINTERNALERRORCODES_H__
#define ECOMINTERNALERRORCODES_H__



//
// Codes which are only used for Panics
//


/**
An attempt has been made to update the registry information for a
non-existent DLL.  An ECom internal error.
*/
const TInt KEComErrInvalidRegistryData				= -17022;


//
// Error codes for internal use only
//
/**
The Client requested a service which is not supplied by this framework
*/
const TInt KEComErrInvalidUnloadPolicy				= -17003;

/**
An attempt has been made to load an implementation contained within a dll
which no longer exists (deleted or is on a CF card that has been removed).
*/
const TInt KEComErrInvalidIIC						= -17008;

/**
An attempt has been made to update registry information for a dll on a
drive that does not exist.
*/
const TInt KEComErrDriveNotFound					= -17019;

//
// Codes which are currently unused but reserved for future expansion
//
/**
Internal error code.
*/
const TInt KEComErrAlreadyReceiving					= -17001;


/**
Internal error code
*/
const TInt KEComErrIndexEntryNotFound				= -17020;

/**
Internl error code
*/
const TInt KEComErrInstantiationPointerNotFound		= -17024;

/**
Internal error code
*/
const TInt KEComErrInstantiationPointerNotAvailable = -17025;

#endif	// ECOMINTERNAL_ERRORCODES_H__
