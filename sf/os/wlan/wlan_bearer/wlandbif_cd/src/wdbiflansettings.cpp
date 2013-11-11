/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* %version: 3 %
*/

// INCLUDE FILES
#include <e32svr.h>

#include "wdbiflansettings.h"
#include "gendebug.h"

// ---------------------------------------------------------
// Class CLanSettings
// ---------------------------------------------------------

// MEMBER FUNCTIONS

// ---------------------------------------------------------
// CLanSettings::CLanSettings()
// C++ default constructor can NOT contain any code, that
// might leave
// ---------------------------------------------------------
EXPORT_C CLanSettings::CLanSettings() : iLANServiceRecord( NULL), iDBSession( NULL)
    {

    }

// Destructor
//
EXPORT_C CLanSettings::~CLanSettings() 
    {
    delete iDBSession;
    delete iLANServiceRecord;
    }

// ---------------------------------------------------------
// TInt CLanSettings::Connect()
// returns KErrNone on success
// ---------------------------------------------------------
//
EXPORT_C TInt CLanSettings::Connect()
    {
    TraceDump(WARNING_LEVEL,_L("CLanSettings::Connect()"));
    
    TRAPD(err,DoConnectL());
    return err;
    }

// ---------------------------------------------------------
// TInt CLanSettings::Disconnect()
// ---------------------------------------------------------
//
EXPORT_C void CLanSettings::Disconnect()
    {
    TraceDump(WARNING_LEVEL,_L("CLanSettings::Disconnect()"));
    
    delete iDBSession;
    iDBSession = NULL;
    
    delete iLANServiceRecord;
    iLANServiceRecord = NULL;
    }



// ---------------------------------------------------------
// TInt CLanSettings::GetLanSettings( TUint32 aId,SLanSettings &aSettings)
// returns KErrNone on success, LAN settings in aSettings
// ---------------------------------------------------------
//
EXPORT_C TInt CLanSettings::GetLanSettings( TUint32 aId,SLanSettings &aSettings)
    {
    TraceDump(WARNING_LEVEL,_L("CLanSettings::GetLanSettings()"));
    
    if( iLANServiceRecord)
        {
        delete iLANServiceRecord;
        iLANServiceRecord = NULL;
        }
    
    TRAPD( err, iLANServiceRecord = static_cast<CCDLANServiceRecord*>( CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord)));
    
    if( !err)
        {
        iLANServiceRecord->SetRecordId( aId);
        TRAP( err, iLANServiceRecord->LoadL( *iDBSession));
        
        if( !err)
            {
            aSettings.Name = iLANServiceRecord->iRecordName;
            aSettings.Id = iLANServiceRecord->RecordId();
            aSettings.AddrFromServer = iLANServiceRecord->iIpAddrFromServer;
            aSettings.Addr = iLANServiceRecord->iIpAddr;
            }
        }
    return err;
    }



// ---------------------------------------------------------
// TInt CLanSettings::WriteLanSettings( SLanSettings &aSettings)
// returns KErrNone on success, writes LAN settings from aSettings
// ---------------------------------------------------------
//
EXPORT_C TInt CLanSettings::WriteLanSettings( SLanSettings &aSettings)
    {
    TraceDump(WARNING_LEVEL,_L("CLanSettings::WriteLanSettings()"));
    
    if( iLANServiceRecord)
        {
        delete iLANServiceRecord;
        iLANServiceRecord = NULL;
        }
        
    TRAPD( err, iLANServiceRecord = static_cast<CCDLANServiceRecord*>( CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord)));
    
    if( !err)
        {
        if( aSettings.Id) // updating existing
            {
            iLANServiceRecord->SetRecordId( aSettings.Id);
            TRAP( err, iLANServiceRecord->LoadL( *iDBSession));
            if( err)
                {
                return err;
                }
            }
        
        TRAP( err, iLANServiceRecord->iRecordName.SetL( aSettings.Name));
        if( err)
            {
            return err;
            }
        TRAP( err, iLANServiceRecord->iIpAddrFromServer.SetL( aSettings.AddrFromServer));
        if( err)
            {
            return err;
            }
        TRAP( err, iLANServiceRecord->iIpAddr.SetL( aSettings.Addr));
        if( err)
            {
            return err;
            }
        
        if( aSettings.Id == 0) // create new
            {
            iLANServiceRecord->SetRecordId( KCDNewRecordRequest);
            TRAP( err, iLANServiceRecord->StoreL( *iDBSession));
            aSettings.Id = iLANServiceRecord->RecordId(); // return new id to caller
            }
        else
            {
            TRAP( err, iLANServiceRecord->ModifyL( *iDBSession));
            }
        
        }
    
    return err;
    }


// ---------------------------------------------------------
// TInt CLanSettings::DeleteLanSettings( TUint32 aId)
// returns KErrNone on success, deletes LANService record with ID aId
// ---------------------------------------------------------
//
EXPORT_C TInt CLanSettings::DeleteLanSettings( TUint32 aId)
    {
    TraceDump(WARNING_LEVEL,_L("CLanSettings::DeleteLanSettings()"));
    
    if( iLANServiceRecord)
        {
        delete iLANServiceRecord;
        iLANServiceRecord = NULL;
        }
    
    TRAPD( err, iLANServiceRecord = static_cast<CCDLANServiceRecord*>( CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord)));
    
    if( !err)
        {
        iLANServiceRecord->SetRecordId( aId);
        TRAP( err, iLANServiceRecord->DeleteL( *iDBSession));
        }
    return err;
    
    }

//
// L-methods called and trapped by the public non-leaving methods
//

// ---------------------------------------------------------
// void CLanSettings::DoConnectL()
// Connects to the CommsDat storage server
// ---------------------------------------------------------
//
void CLanSettings::DoConnectL()
    {
    TraceDump(ERROR_LEVEL,_L("CLanSettings::DoConnectL()"));

    if( !iDBSession)
        {
        iDBSession = CMDBSession::NewL( KCDCurrentVersion);
        }
    
    }
