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
 @publishedPartner
 @released
*/

#ifndef FDCPLUGIN_H
#define FDCPLUGIN_H

#include <e32base.h>

class MFdcPluginObserver;

/**
Base class for FDC plugins.
*/
class CFdcPlugin : public CBase
	{
	// So we can make most of this class private- validly callable only by 
	// CFdcProxy.
	friend class CFdcProxy;

private: // used by FDF to create and destroy FDC instances
	/** 
	Constructor.
	@param aImplementationUid UID of implementation to create.
	@param aObserver Handle back onto the FDF for the FDC to use.
	@return Ownership of a new FDC plugin.
	*/
	IMPORT_C static CFdcPlugin* NewL(TUid aImplementationUid, MFdcPluginObserver& aObserver);

	/**
	Called by FDF to get a pointer to an object which implements the 
	FDF API with UID aUid. This is a mechanism for allowing future change 
	to the FDF API without breaking BC in existing (non-updated) FDF 
	plugins.
	*/
	virtual TAny* GetInterface(TUid aUid) = 0;

protected: // called by concrete FDF plugins
	/** 
	Constructor
	*/
	IMPORT_C CFdcPlugin(MFdcPluginObserver& aObserver);

	/** Destructor. */
	IMPORT_C ~CFdcPlugin();

	/** 
	Accessor for the interface to the FDF. 
	@return Interface to the FDF.
	*/
	IMPORT_C MFdcPluginObserver& Observer();

	/**
	From CBase.
	*/
	IMPORT_C TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

private: // owned
	/**
	UID set by ECOM when the instance is created. Used when the instance is 
	destroyed.
	*/
	TUid iInstanceId;

	/**
	Interface for calls up to the FDF.
	*/
	MFdcPluginObserver& iObserver;

	/** 
	Pad for BC-friendly future change.
	*/
	TAny* iPad;
	};

#endif // FDCPLUGIN_H
