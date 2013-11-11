/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* CERTIFICATEAPPS.H
* CCertificateAppInfoManager class implementation
*
*/


/**
 @file 
 @publishedAll
 @released 
*/
 
#ifndef __CERTIFICATEAPPS_H__
#define __CERTIFICATEAPPS_H__

#include <f32file.h>
#include <s32file.h>
#include <mctcertapps.h>

class MCTCertApps;
/**
 * This class is used to access a persistent store which contains the available
 * applications on a device.  It provides an interface to add and retrieve
 * applications.
 * 
 * @publishedAll
 * @released
 */
class CCertificateAppInfoManager : public CBase
	{
public:
	/**
	* Creates a new instance of the app info manager and puts a pointer to it onto the cleanup stack.
	*
	* @return	A new CCertificateAppInfoManager object.
	*/
	IMPORT_C static CCertificateAppInfoManager* NewLC();
	
	/**
	* Creates a new instance of the app info manager.
	* 
	* @return	A new CCertificateAppInfoManager object.
	*/
	IMPORT_C static CCertificateAppInfoManager* NewL();

	/**
	 * Creates a new instance of the app info manager.
	 * 
	 * @deprecated	Clients should use the no-argument version.
	 */
	IMPORT_C static CCertificateAppInfoManager* NewLC(RFs& aFs, TBool aOpenedForWrite);
	
	/**
	 * Creates a new instance of the app info manager.
	 * 
	 * @deprecated	Clients should use the no-argument version.
	 */
	IMPORT_C static CCertificateAppInfoManager* NewL(RFs& aFs, TBool aOpenedForWrite);

	/** Virtual destructor. Frees all resources owned by the object, prior to its destruction.
  	*/
	IMPORT_C virtual ~CCertificateAppInfoManager();

private:
	CCertificateAppInfoManager();
	void ConstructL();

public:
	/**
	 * Adds a new application.
	 * 
	 * @param aClient				The new application to add.
	 * @capability WriteDeviceData	The caller requires the WriteDeviceData capability.
	 * @leave KErrPermissionDenied	If the caller does not have the required capability.
	 */
	IMPORT_C void AddL(const TCertificateAppInfo& aClient);
	
	/**
	 * Removes an existing application.
	 * 
	 * @param aUid					The UID of the application to remove.
	 * @capability WriteDeviceData	The caller requires the WriteDeviceData capability.
	 * @leave KErrPermissionDenied	If the caller does not have the required capability.
	 * @leave KErrNotFound			If there is no application matching the specified UID.
	 */
	IMPORT_C void RemoveL(const TUid& aUid);

	/**
	 * Gets an application by UID.
	 *
	 * @param aUid			The UID of the application.
	 * @param aIndex		On return, the index of the application in the store.
	 * @return				Information about the application if it is present.
	 * @leave KErrNotFound	If there is no application matching the specified UID.
	 */
	IMPORT_C const TCertificateAppInfo& ApplicationL(const TUid& aUid, TInt& aIndex) const;

	/**
	 * Gets a list of all applications in the store.
	 *
	 * @return	An array containing all applications in the store.
	 */
	IMPORT_C const RArray<TCertificateAppInfo>& Applications() const;

private:
	MCTCertApps* iCertAppsIf;
	/// A cache of the contents of the store 
	RArray<TCertificateAppInfo> iClients;
	};

#endif
