/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles the LANService table in CommsDat
*
*/

/*
* %version: 6 %
*/

#ifndef LANSETTINGS_H
#define LANSETTINGS_H

//  INCLUDES
#include <commsdattypesv1_1.h>
#include <commsdat.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif


using namespace CommsDat;


//  CLASS DECLARATION

/**
* Struct for LAN table settings
*/
struct SLanSettings
{
    TBuf<KMaxMedTextLength> Name;
    TUint32										   Id;
    TBool										   AddrFromServer;
    TBuf<KMaxMedTextLength> Addr;
};

/**
* Class for handling LAN settings in commsdat
*
* @lib wlandbif.lib
* @since
*/
NONSHARABLE_CLASS( CLanSettings ) : public CBase
{
public:  // Constructors and destructor
	/**
	* Constructor
    */
    IMPORT_C CLanSettings();
    
    /**
    * Destructor
    * Also Handles disconnection
    */
    IMPORT_C virtual ~CLanSettings();
    
public:  // New functions
    /**
    * Connects to commsdat
	* @param 
    * @return KErrNone or other symbian error code
    * @remarks
    * @see
    */
	IMPORT_C TInt Connect();

    /**
    * Disconnects from commsdat.
   	* @param 
    * @return
    * @remarks
    * @see
    */
    IMPORT_C void Disconnect();
    
    /**
    * Gets LAN settings with specified ID
   	* @param aId is the id of lan settings row that is fetched, aSettings contains the lan settings
    * @return KErrNone or other symbian error code
    * @remarks
    * @see
    */
	IMPORT_C TInt GetLanSettings(TUint32 aId,SLanSettings &aSettings);
    
    /**
    * Writes LAN settings with specified ID, if ID is 0, new record is created
	* @param aSettings contains the lan settings. if id is 0, then new id is stored in settings.Id
    * @return KErrNone or other symbian error code
    * @remarks
    * @see
    */
	IMPORT_C TInt WriteLanSettings(SLanSettings &aSettings);
    
    /**
    * deletes LAN settings with specified ID
	* @param aId is the id of lan settings row that is deleted
    * @return KErrNone or other symbian error code
    * @remarks
    * @see
    */
	IMPORT_C TInt DeleteLanSettings(TUint32 aId);

private:

	void DoConnectL();
    
private:  // Data

    CCDLANServiceRecord* iLANServiceRecord;
    CMDBSession* iDBSession;
};

#endif // LANSETTINGS_H

// end of file
