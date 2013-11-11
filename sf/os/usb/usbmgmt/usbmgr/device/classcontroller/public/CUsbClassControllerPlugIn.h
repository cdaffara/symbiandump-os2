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
* Implements part of UsbMan USB Class Framework.
* All USB classes to be managed by UsbMan must derive
* from this class.
*
*/

/**
 @file
*/

#ifndef CUSBCLASSCONTROLLERPLUGIN_H__
#define CUSBCLASSCONTROLLERPLUGIN_H__

#include <cusbclasscontrollerbase.h>


class MUsbClassControllerNotify;

class CUsbClassControllerPlugIn : public CUsbClassControllerBase
/** Base class for dynamic Class Controllers, implemented as ECOM plugins.

  @publishedPartner
  @released
  */
	{
public: 
	/** Factory function
		@param aImplementationId The UID of this implementation.
		@param aOwner Interface to talk to the server.
		@return Ownership of a new CUsbClassControllerPlugIn.
		*/
	IMPORT_C static CUsbClassControllerPlugIn* NewL(TUid aImplementationId, 
													MUsbClassControllerNotify& aOwner);

	/** Destructor
	*/
	IMPORT_C virtual ~CUsbClassControllerPlugIn();



protected:
	/** Constructor
		@param aOwner Interface to talk to the server.
		@param aStartupPriority The relative priority of this class controller.
		*/
	IMPORT_C CUsbClassControllerPlugIn(MUsbClassControllerNotify& aOwner, TInt aStartupPriority);

protected:
	/**
		UID given to us by ECOM when the instance is created. Used when the 
		instance is destroyed.
	*/
	TUid iPrivateEComUID;
	};

#endif

