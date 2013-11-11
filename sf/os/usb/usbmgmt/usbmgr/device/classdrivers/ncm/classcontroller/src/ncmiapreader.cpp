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
*
*/

/**
 * @file
 * @internalComponent
 */

#include <e32debug.h>
#include <commdb.h>

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_essock.h>
#else
#include <commdbconnpref.h>
#endif

#include <commsdattypesv1_1.h> 

#include "ncmiapreaderobserver.h"
#include "ncmiapreader.h"

// For OST Tracing
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmiapreaderTraces.h"
#endif


_LIT(KNcmIapReaderPanic, "UsbNcmIapReader"); // must be <=16 chars

// Panic codes
enum TNcmIapReaderPanicCode
    {
    ENcmIapReaderPanicOutstandingRequest = 1,
    ENcmIapReaderPanicTooManyIAPs = 2,
    ENcmIapReaderEndMark
    };

// NCM configuration item names
_LIT(KBearerNifName, "ethint");
_LIT(KBearerLddFileName, "eusbcsc");
_LIT(KBearerPktDrvName, "ncmpkt.drv");

/**
 * Constructs a CNcmIapReader object.
 * @param aObserver An MNcmIAPReaderObserver instance which handles IAP reading completing
 * @param aConnPref The reference of the NCM connection aConnPref
 */
CNcmIapReader::CNcmIapReader(MNcmIAPReaderObserver& aObserver, TCommDbConnPref& aConnPref)
: CActive(CActive::EPriorityStandard),
      iObserver(aObserver),
      iConnPref(aConnPref)
	{
	OstTraceFunctionEntryExt( CNCMIAPREADER_CNCMIAPREADER_ENTRY, this );
	CActiveScheduler::Add(this);		
	OstTraceFunctionExit1( CNCMIAPREADER_CNCMIAPREADER_EXIT, this );
	}

/**
 * Destructor.
 */
CNcmIapReader::~CNcmIapReader()
	{
	OstTraceFunctionEntry0( CNCMIAPREADER_CNCMIAPREADER_ENTRY_DESTRUCTOR );
	
	Cancel();
	OstTraceFunctionExit0( CNCMIAPREADER_CNCMIAPREADER_ENTRY_DESTRUCTOR_EXIT );
	}

/**
 * Start the connection.
 */
void CNcmIapReader::Start()
	{
	OstTraceFunctionEntry0( CNCMIAPREADER_START_ENTRY );
	
	__ASSERT_ALWAYS(!IsActive(), 
	        User::Panic(KNcmIapReaderPanic, ENcmIapReaderPanicOutstandingRequest));
    
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
	
	OstTraceFunctionExit0( CNCMIAPREADER_START_EXIT );
	}

/**
 * Implements cancellation of an outstanding request.
 */
void CNcmIapReader::DoCancel()
	{
	OstTraceFunctionEntry0( CNCMIAPREADER_DOCANCEL_ENTRY );
	
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrCancel);
	OstTraceFunctionExit0( CNCMIAPREADER_DOCANCEL_EXIT );
	}

/**
 * Simulate an async way and give
 * the scheduler a chance to schedule all AOs.
 */
void CNcmIapReader::RunL()
	{
	OstTraceFunctionEntry0( CNCMIAPREADER_RUNL_ENTRY );
	
	// Reading
	TRAPD(err, SearchNcmIapL())
	OstTrace1( TRACE_NORMAL, CNCMIAPREADER_RUNL_GETIAP, "Get NCM IAP with error code %d", err );
	

	// Complete connection manager
	iObserver.MicoHandleIAPReadingComplete(err);
	OstTraceFunctionExit0( CNCMIAPREADER_RUNL_EXIT );
	}

/**
 * Search NCM IAP in current commDB and set it into the ncm connection manager.
 */
void CNcmIapReader::SearchNcmIapL()
    {
    OstTraceFunctionEntry0( CNCMIAPREADER_GETNCMIAPIDL_ENTRY );
    
    using namespace CommsDat;
    CMDBSession* dbs = CMDBSession::NewLC(KCDLatestVersion); // Current version is 1.1
    
    CMDBRecordSet<CCDLANBearerRecord>* lanBearerTable = 
            new (ELeave) CMDBRecordSet<CCDLANBearerRecord>(KCDTIdLANBearerRecord);
    CleanupStack::PushL(lanBearerTable);
    // Load the table into memory
    lanBearerTable->LoadL(*dbs);
 
    TInt numLanBearers = lanBearerTable->iRecords.Count();    
    OstTrace1( TRACE_NORMAL, CNCMIAPREADER_GETNCMIAPIDL_COUNT_LAN_BEARER, "Found %d Lan Bearer!", numLanBearers );
    TInt chosenLanBearer = KErrNotFound;
    TInt i = 0;
    for(i = 0; i<numLanBearers; i++)
        {
        CCDLANBearerRecord* lanBearer = 
                static_cast<CCDLANBearerRecord*>(lanBearerTable->iRecords[i]);
        if ((lanBearer->iLanBearerNifName.GetL().Compare(KBearerNifName()) == 0) &&
            (lanBearer->iLanBearerLddFilename.GetL().Compare(KBearerLddFileName()) == 0) &&
            (lanBearer->iLanBearerPacketDriverName.GetL().Compare(KBearerPktDrvName()) == 0))
            {
            // Matched all check points. We found LAN bearer for NCM
            chosenLanBearer = lanBearer->RecordId();
            break;
            }
        }
    CleanupStack::PopAndDestroy(lanBearerTable); // lanBearerTable
    if (KErrNotFound == chosenLanBearer)
        {
        OstTrace0( TRACE_ERROR, CNCMIAPREADER_GETNCMIAPIDL, "Can not find NCM Lan Bearer!" );
        User::Leave(KErrNotFound);
        }
    OstTrace1( TRACE_NORMAL, CNCMIAPREADER_GETNCMIAPIDL_CHOSEN_LAN_BEARER, "Chosen Lan Bearer %d!", chosenLanBearer );
    
    CMDBRecordSet<CCDIAPRecord> * iapTable = 
            new (ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
    CleanupStack::PushL(iapTable);
    // Load the table into memory
    iapTable->LoadL(*dbs);

    TInt numIaps = iapTable->iRecords.Count();
    TInt chosenIAP = KErrNotFound;
    TInt iapLanBearer = KErrNotFound;
    for (i = 0; i < numIaps; i++)
        {
        CCDIAPRecord* iap =
                static_cast<CCDIAPRecord*> (iapTable->iRecords[i]);
        OstTrace1( TRACE_NORMAL, CNCMIAPREADER_GETNCMIAPIDL_RECORD_ID, "Id of current IAP is %d!", iap->RecordId() );
        OstTrace1( TRACE_NORMAL, CNCMIAPREADER_GETNCMIAPIDL_RECORD_BEARER_ID, "Lan Bearer Id of current IAP is %d!", TMDBElementId(iap->iBearer) );
        iapLanBearer = TMDBElementId(iap->iBearer);
        if ((iap->iBearerType.GetL().Compare(TPtrC(KCDTypeNameLANBearer)) == 0) && 
                (iapLanBearer == chosenLanBearer))
            {
            // NCM IAP found!
            // Panic if there is more than NCM IAP found
            OstTrace1( TRACE_NORMAL, CNCMIAPREADER_SEARCHNCMIAPL_ONLY_ONE_NCM_IAP, "NCM IAP found: Current chosenIAP value is %d", chosenIAP );
            
            __ASSERT_ALWAYS(KErrNotFound == chosenIAP, User::Panic(KNcmIapReaderPanic, ENcmIapReaderPanicTooManyIAPs));
            chosenIAP = iap->RecordId();
            break;
            }
        }
    CleanupStack::PopAndDestroy(2, dbs); // iapTable, dbs    
    if(chosenIAP == KErrNotFound)
        {
        OstTrace0( TRACE_ERROR, CNCMIAPREADER_GETNCMIAPIDL_NOT_FOUND, "Can not find NCM IAP!" );
        User::Leave(KErrNotFound);
        }
    OstTrace1( TRACE_NORMAL, CNCMIAPREADER_GETNCMIAPIDL_CHOSEN_IAP_ID, "Chosen IAP is %d!", chosenIAP );
    iConnPref.SetIapId(chosenIAP);
    OstTraceFunctionExit0( CNCMIAPREADER_GETNCMIAPIDL_EXIT );
    }
