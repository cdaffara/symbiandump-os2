// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ULogger plug-in base class
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef ULOGGERPLUGIN_H
#define ULOGGERPLUGIN_H

#include <e32base.h>
#include <ecom/implementationinformation.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/ecom.h>

namespace Ulogger
{
//! Base class for all ULogger plug-ins.
/*!
ULogger is extensible through a plug-in framework that uses ECom for plug-in
discovery. Deriving from this class means that all the ECom-specific logic is
already provided for the plug-in implementer, out-of-the-box, leaving the
plug-in code to deal with the domain-specific logic that the plug-in is supposed
to implement only.

Among the plug-in types that are currently supported are output plug-ins (see
class ULogger::MOutputPlugin in uloggeroutputplugin.h) and input plug-ins (see
class ULogger::MInputPlugin in uloggerinputplugin.h).

Plug-ins must derive from this class in order to be compatible with ULogger.
*/
class CPlugin : public CBase
	{
public:
	/**
	Enum to signify plug-in interface types. Used by each specific plug-in
	interface (M-class) to identify itself as being of a particular type.
	*/
	enum TPluginInterface
	{
	EOutput,//!< output plug-in interface type
	EInput  //!< input plug-in interface type
	};

public:
	/**
	Creates a CPlugin instance of the specified type and returns a pointer to
	it. The type is specified as the name of the ECom plug-in DLL (without the
	dll	extension).

	@param aCue a descriptor containing the name of the plug-in to be created
	@return A pointer to the newly created CPlugin object.
	*/
	static CPlugin* NewL(const TDesC8& aCue);

	/**
	Virtual destructor.
	*/
	virtual ~CPlugin();

	/**
	Return pointer to requested interface. 
	If plug-in implements multiple interfaces, it should return pointer to
	proper interface trough this method.

	@param aInterfaceId Number of requested interface.
	@return Pointer to requested interface or NULL if requested interface is not
	        supported.
	*/
	virtual TAny* GetInterfaceL(TPluginInterface aInterfaceId) = 0;

private:
	TUid iDtor_ID_Key;		 
	};

} //end of namespace

#include "uloggerplugin.inl" // Our own base implementations for ECOM

#endif /* ULOGGERPLUGIN_H */
