/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ACMSERVER_H__
#define __ACMSERVER_H__

#include <e32base.h>

class CAcmServerImpl;

/**
Client interface to the ACM server to create new ACM functions.
This class is intended to be used both inside and outside Symbian to bring up 
and tear down ACM functions.
*/
NONSHARABLE_CLASS(RAcmServer)
	{
public:
	/** 
	Standard constructor. Does not connect the handle to the service provider. 
	*/
	IMPORT_C RAcmServer();

	/** 
	Standard destructor. Does not disconnect the handle from the service 
	provider. 
	*/
	IMPORT_C ~RAcmServer();

public:
	/**
	Connects this handle to the service provider.
	@return Error.
	*/
	IMPORT_C TInt Connect();

	/**
	Closes this handle to the service provider. If the handle has been 
	successfully Connected, it must eventually be Closed.
	*/
	IMPORT_C void Close();

public:
	/**
	Creates aNoAcms ACM functions using the default settings.
	The default setting includes KDefaultAcmProtocolNum- protocol code 0x01 
	(Hayes compatible, from USBCDC 1.1 Table 17).
	@param aNoAcms Number of ACM functions to be created.
	@return Error.
	*/
	IMPORT_C TInt CreateFunctions(const TUint aNoAcms);

	/**
	Creates aNoAcms ACM functions using the protocol number given (as defined 
	in USBCDC 1.1 - Table 17).
	@param aNoAcms Number of ACM functions to be created.
	@param aProtocolNum Protocol setting for the ACM functions to be created.
	@return Error.
	*/
	IMPORT_C TInt CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum);

	/**
	Creates aNoAcms ACM functions using the protocol number given (as defined 
	in USBCDC 1.1 - Table 17).
	@param aNoAcms Number of ACM functions to be created.
	@param aProtocolNum Protocol setting for the ACM function to be created.
	@param aAcmControlIfcName Control Interface Name for the ACM function to be created.
	@param aAcmDataIfcName Data Interface Name for the ACM function to be created.
	@return Error.
	*/
	IMPORT_C TInt CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC& aAcmControlIfcName, const TDesC& aAcmDataIfcName);

	/**
	Destroys aNoAcms ACM functions.
	Class Controllers MUST NOT destroy more ACM functions than they have 
	successfully created.
	@param aNoAcms Number of ACM interfaces to destroy.
	@return Error.
	*/
	IMPORT_C TInt DestroyFunctions(const TUint aNoAcms);

private:
	/** 
	Private copy constructor to avoid problems with multiple ownership of 
	RAcmServerImpls.
	*/
	RAcmServer(const RAcmServer& aObjectToCopy);

private: // owned
	// The 'body' used by this handle.
	CAcmServerImpl* iImpl;	
	};

#endif // __ACMSERVER_H__
