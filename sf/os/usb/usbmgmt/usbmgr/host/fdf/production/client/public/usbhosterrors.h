/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @prototype
*/

#ifndef USBHOSTERROR_H
#define USBHOSTERROR_H


/** The device is unsupported- the FDF will not attempt to load drivers for
it. */
const TInt KErrUsbUnsupportedDevice				= -6620;

/** Driver loading has not been enabled. */
const TInt KErrUsbDriverLoadingDisabled			= -6621;

/** The configuration descriptor's NumInterfaces is 0. */
const TInt KErrUsbConfigurationHasNoInterfaces	= -6622;

/** The configuration descriptor's NumInterfaces does not equal the actual
number of (alternate 0) interface descriptors in the configuration bundle. */
const TInt KErrUsbInterfaceCountMismatch			= -6623;

/** A Function Driver was not found for at least one interface in the device's
selected configuration. */
const TInt KErrUsbFunctionDriverNotFound			= -6624;

/** Two of the interfaces in the configuration bundle have the same interface
numbers. */
const TInt KErrUsbDuplicateInterfaceNumbers		= -6625;

/** A Function Driver has found a problem with the device's descriptors. Such
problems include (non-exhaustively) formatting or layout of Class-specific
descriptors and alternate interface numbering. */
const TInt KErrUsbBadDescriptor					= -6626;

/** While FDF was busy trying to load FDs the current attached device has been
detached by USBDI (physically unplugged or was draining too much current) */
const TInt KErrUsbDeviceDetachedDuringDriverLoading			= -6627;

/** During an attachment failure, FDF receives an error from USBDI
If the error is not a known expected error then FDF will report it
with the following error code -6628, as a KErrUsbAttachmentFailureGeneralError
*/
const TInt KErrUsbAttachmentFailureGeneralError			= -6628;

/** If a Function Driver or Function Driver upgrade is installed or uninstalled then the FDF 
receives notification of this from ECom and will update its list of Function Driver Proxies 
accordingly. This may involve an allocation which therefore may fail however there is no point
in the FDF propegating this failure (via a leave) back up the call stack as the originator is ECom.
In the event that the FDF does encounter OOM then its list of proxy function drivers will be out of
date, this error code indicates such a situation.
*/
const TInt KErrUsbUnableToUpdateFDProxyList				= -6629;

#endif // USBHOSTERROR_H
