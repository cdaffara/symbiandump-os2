/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Abstract base class for usbman extension plugins.
*
*/

/**
 @file
 @publishedPartner
 @released
*/

#ifndef USBMANEXTENSIONPLUGINDUMMY_H
#define USBMANEXTENSIONPLUGINDUMMY_H

#include <e32base.h>
#include "dummyusblddapi.h"

class MUsbmanExtensionPluginObserver;
class MUsbDeviceNotify;

/**
The UID of the Usbman Extension Plugin interface.
*/
const TInt KUsbmanExtensionPluginInterfaceUid = 0x10208DD6;

class CUsbmanExtensionPlugin : public CBase
	{
public:
	/** 
	Constructor.
	@param aImplementationUid The UID of the implementation.
	@param aObserver The observer of the plugin.
	*/
	IMPORT_C static CUsbmanExtensionPlugin* NewL(const TUid aImplementationUid, 
		MUsbmanExtensionPluginObserver& aObserver);

	/** Destructor. */
	IMPORT_C ~CUsbmanExtensionPlugin();

public:
	/**
	Called by Usbman server to get a pointer to an object which implements the 
	ExtensionPlugin interface with UID aUid. This is a mechanism for allowing future 
	change to the plugin API without breaking BC in existing (non-updated) 
	plugins.
	*/
	virtual TAny* GetInterface(TUid aUid) = 0;

protected:
	/** 
	Constructor.
	@param aObserver The observer of the plugin.
	*/
	IMPORT_C CUsbmanExtensionPlugin(MUsbmanExtensionPluginObserver& aObserver);

protected:
	/**
	Accessor for the observer.
	@return The observer.
	*/
	IMPORT_C MUsbmanExtensionPluginObserver& Observer();

private: // owned
	/**
	UID set by ECOM when the instance is created. Used when the instance is 
	destroyed.
	*/
	TUid iInstanceId;

private: // unowned
	MUsbmanExtensionPluginObserver& iObserver;
	};

#endif // USBMANEXTENSIONPLUGINDUMMY_H
