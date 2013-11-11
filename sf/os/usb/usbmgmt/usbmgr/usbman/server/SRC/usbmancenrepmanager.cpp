// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements a utility class which read information from Central Repository



#include <centralrepository.h>
#include <usb/usblogger.h>
#ifdef SYMBIAN_FEATURE_MANAGER
    #include <featureuids.h>
    #ifndef __DUMMY_LDD__
        #include <featdiscovery.h>
    #else
        #include "featurediscovery.h"
    #endif
#endif
#include "usbmanprivatecrkeys.h"
#include "UsbSettings.h"
#include "CPersonality.h"
#include "usbmancenrepmanager.h"
#include "CUsbDevice.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "usbmancenrepmanagerTraces.h"
#endif

 
_LIT(KUsbCenRepPanic, "UsbCenRep");

/**
 * Panic codes for the USB Central Repository Manager
 */
enum TUsbCenRepPanic
    {
    ECenRepObserverNotStopped = 0,
    ECenRepObserverAlreadySet,
    ECenRepConfigError,
    ECenRepFeatureManagerError,
    };

// ---------------------------------------------------------------------------
// Private consctruction   
// ---------------------------------------------------------------------------
//
CUsbManCenRepManager::CUsbManCenRepManager(CUsbDevice& aUsbDevice)
  : iUsbDevice( aUsbDevice )
	{
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_CUSBMANCENREPMANAGER_CONS_ENTRY );
	OstTraceFunctionExit0( CUSBMANCENREPMANAGER_CUSBMANCENREPMANAGER_CONS_EXIT );
	}

// ---------------------------------------------------------------------------
// The first phase construction   
// ---------------------------------------------------------------------------
//
CUsbManCenRepManager* CUsbManCenRepManager::NewL(CUsbDevice& aUsbDevice)
    {
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_NEWL_ENTRY );
    CUsbManCenRepManager* self = new (ELeave) CUsbManCenRepManager(aUsbDevice);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// The second phase construction   
// ---------------------------------------------------------------------------
//
void CUsbManCenRepManager::ConstructL()
    {
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_CONSTRUCTL_ENTRY );
    // Open the Central Repository
    iRepository = CRepository::NewL( KCRUidUSBManagerConfiguration );
    CheckSignatureL();
    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Deconstruction   
// ---------------------------------------------------------------------------
//
CUsbManCenRepManager::~CUsbManCenRepManager()
	{
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_CUSBMANCENREPMANAGER_DES_ENTRY );
    delete iRepository;
	OstTraceFunctionExit0( CUSBMANCENREPMANAGER_CUSBMANCENREPMANAGER_DES_EXIT );
	}

// ---------------------------------------------------------------------------
// Read specific Key whose value type is String   
// ---------------------------------------------------------------------------
//
HBufC* CUsbManCenRepManager::ReadStringKeyLC( TUint32 aKeyId )
	{
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_READSTRINGKEYLC_ENTRY );
    HBufC* keyBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr key = keyBuf->Des();

    TInt err = iRepository->Get( aKeyId, key );
    LEAVEIFERRORL( err, OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READSTRINGKEYLC, "CUsbManCenRepManager::ReadStringKeyLC;Leave err=%d", err ););
	OstTraceExt2( TRACE_NORMAL, CUSBMANCENREPMANAGER_READSTRINGKEYLC_DUP1, "CUsbManCenRepManager::ReadStringKeyLC;aKeyId=%x;key=%S", aKeyId, key );

    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_READSTRINGKEYLC_EXIT );
    return keyBuf;
    }

// ---------------------------------------------------------------------------
// Read specific Key whose value type is TInt   
// ---------------------------------------------------------------------------
//
TInt CUsbManCenRepManager::ReadKeyL( TUint32 aKeyId )
    {
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_READKEYL_ENTRY );
    TInt key;
    
    TInt err = iRepository->Get( aKeyId, key );
    LEAVEIFERRORL( err, OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READKEYL, "CUsbManCenRepManager::ReadKeyL;Leave err=%d", err ); );
    OstTraceExt2( TRACE_NORMAL, CUSBMANCENREPMANAGER_READKEYL_DUP1, "CUsbManCenRepManager::ReadKeyL;LocSets: ReadKeyL id: 0x%x, val: 0x%x", aKeyId, (TInt32)key );

    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_READKEYL_EXIT );
    return key;
    }

// ---------------------------------------------------------------------------
// Check wheather cenrep's version is supported by cenrep manager 
// ---------------------------------------------------------------------------
//
void CUsbManCenRepManager::CheckSignatureL()
    {
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_CHECKSIGNATUREL_ENTRY );
    iSignature = ReadKeyL( KUsbManConfigSign );
    
    if ( iSignature < TUsbManagerSupportedVersionMin ||
            iSignature > TUsbManagerSupportedVersionMax )
        {
        OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_CHECKSIGNATUREL, "CUsbManCenRepManager::CheckSignatureL;Leave reason=%d", KErrNotSupported );
        }    
    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_CHECKSIGNATUREL_EXIT );
    }

// ---------------------------------------------------------------------------
// Read Device configuration table 
// ---------------------------------------------------------------------------
//
void CUsbManCenRepManager::ReadDeviceConfigurationL(CUsbDevice::TUsbDeviceConfiguration& aDeviceConfig)
    {
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_READDEVICECONFIGURATIONL_ENTRY );
    //Only support version four right now.
    if(TUsbManagerSupportedVersionFour != iSignature)
        {
        OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READDEVICECONFIGURATIONL, "CUsbManCenRepManager::ReadDeviceConfigurationL;Panic error=%d", ECenRepConfigError );
        __ASSERT_DEBUG( EFalse, User::Panic( KUsbCenRepPanic, ECenRepConfigError ) );
        }
    
    
    //Shall only have on device configuration setting.
    TUint32 devConfigCount = ReadKeyL( KUsbManDeviceCountIndexKey );
    if(devConfigCount != 1)
        {
        OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READDEVICECONFIGURATIONL_DUP1, "CUsbManCenRepManager::ReadDeviceConfigurationL;Panic error=%d", ECenRepConfigError );
        __ASSERT_DEBUG( EFalse, User::Panic( KUsbCenRepPanic, ECenRepConfigError ) );
        }
    
        
    RArray<TUint32> keyArray;
    CleanupClosePushL( keyArray );
    TInt err = iRepository->FindL( KUsbManDevicePartialKey, KUsbManConfigKeyMask, keyArray );
    LEAVEIFERRORL( err, OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READDEVICECONFIGURATIONL_DUP2, "CUsbManCenRepManager::ReadDeviceConfigurationL;Leave err=%d", err ); );
    
    TInt keyCount = keyArray.Count();
    OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READDEVICECONFIGURATIONL_DUP3, "CUsbManCenRepManager::ReadDeviceConfigurationL;keyCount of device config = %d", keyCount );
    
    //Get each extension type key value and store in iExtList array
    for( TInt index = 0; index < keyCount; index++ )
        {
        TUint32 key = keyArray[index];
        TUint32 fieldId = ( key & KUsbManConfigFieldMask );
        OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READDEVICECONFIGURATIONL_DUP4, "CUsbManCenRepManager::ReadDeviceConfigurationL;fieldId=%d", fieldId );
        if( fieldId == KUsbManDeviceVendorIdKey )
            {
            aDeviceConfig.iVendorId = ReadKeyL( key );
            }
        else if( fieldId == KUsbManDeviceManufacturerNameKey )
            {
            HBufC* manufacturer = ReadStringKeyLC( key );
            TPtr manufacturerPtr = manufacturer->Des();
            aDeviceConfig.iManufacturerName->Des().Copy( manufacturerPtr ); 
            CleanupStack::PopAndDestroy( manufacturer );
            }
        else if( fieldId == KUsbManDeviceProductNameKey )
            {
            HBufC* product = ReadStringKeyLC( key );
            TPtr productName = product->Des();
            aDeviceConfig.iProductName->Des().Copy( productName ); 
            CleanupStack::PopAndDestroy( product );
            }
        else
            {
            OstTrace1( TRACE_FATAL, CUSBMANCENREPMANAGER_READDEVICECONFIGURATIONL_DUP5, "CUsbManCenRepManager::ReadDeviceConfigurationL;panic error=%d", ECenRepConfigError );
            User::Panic( KUsbCenRepPanic, ECenRepConfigError );
            }
        }
    CleanupStack::PopAndDestroy( &keyArray );  
    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_READDEVICECONFIGURATIONL_EXIT );
    }


// ---------------------------------------------------------------------------
// Read personality table 
// ---------------------------------------------------------------------------
//
void CUsbManCenRepManager::ReadPersonalitiesL(RPointerArray<CPersonality>& aPersonalities)
	{
	OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_READPERSONALITIESL_ENTRY );
    
	//Only support version four right now.
	if(TUsbManagerSupportedVersionFour != iSignature)
	    {
        OstTrace1( TRACE_FATAL, CUSBMANCENREPMANAGER_READPERSONALITIESL, "CUsbManCenRepManager::ReadPersonalitiesL;ECenRepConfigError=%d", ECenRepConfigError );
        __ASSERT_DEBUG( EFalse, User::Panic( KUsbCenRepPanic, ECenRepConfigError ) );
	    }

	// Get the personality count.
	TUint32 personalityCount = ReadKeyL( KUsbManDevicePersonalitiesCountIndexKey );
	OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READPERSONALITIESL_DUP3, "CUsbManCenRepManager::ReadPersonalitiesL;personalityCount=%d", personalityCount );
	
	RArray<TUint32> keyArray;
	CleanupClosePushL( keyArray ); 
	

	// Go through all personalities and store them.
    for( TInt personalityIdx = 0; personalityIdx < personalityCount; personalityIdx++ )
        {
        CPersonality* personality = CPersonality::NewL();
        CleanupStack::PushL( personality );
        
        // Find the keys belonging to the personality
        TUint32 devicePersonalitiesKey = KUsbManDevicePersonalitiesPartialKey | ( personalityIdx << KUsbManPersonalitiesOffset );
        TInt err = iRepository->FindL( devicePersonalitiesKey, KUsbManConfigKeyMask, keyArray );
        LEAVEIFERRORL( err, OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READPERSONALITIESL_DUP1, "CUsbManCenRepManager::ReadPersonalitiesL;Leave err=%d", err ); );
        
        TInt keyCount = keyArray.Count();
        OstTrace1( TRACE_NORMAL, DUP2_CUSBMANCENREPMANAGER_READPERSONALITIESL_DUP2, "CUsbManCenRepManager::ReadPersonalitiesL; keyCount of personality = %d", keyCount );
        
        // Get each key value of the personality and store it.
        for( TInt keyIdx = 0; keyIdx < keyCount; keyIdx++ )
            {
            TUint32 key = keyArray[keyIdx];
            TUint32 fieldId = (key & KUsbManConfigFieldMask);
            OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READPERSONALITIESL_DUP4, "CUsbManCenRepManager::ReadPersonalitiesL;key id of personality=%d", fieldId );
            switch( fieldId )
                {
                case KUsbManDevicePersonalitiesDeviceClassKey:
                    personality->SetDeviceClass(ReadKeyL( key ));
                    break;
                case KUsbManDevicePersonalitiesDeviceSubClassKey:
                    personality->SetDeviceSubClass(ReadKeyL( key ));
                    break;
                case KUsbManDevicePersonalitiesProtocolKey:
                    personality->SetDeviceProtocol(ReadKeyL( key ));
                    break;
                case KUsbManDevicePersonalitiesNumConfigKey:
                    personality->SetNumConfigurations(ReadKeyL( key ));
                    break;
                case KUsbManDevicePersonalitiesProductIdKey:
                    personality->SetProductId(ReadKeyL( key ));
                    break;
                case KUsbManDevicePersonalitiesBcdDeviceKey:
                    personality->SetBcdDevice(ReadKeyL( key ));
                    break;
                case KUsbManDevicePersonalitiesFeatureIdKey:
                    personality->SetFeatureId(ReadKeyL( key ));
                    break;
                case KUsbManDevicePersonalitiesPersonalityIdKey:
                    personality->SetPersonalityId(ReadKeyL( key ));
                    ReadConfigurationsForPersonalityL( personality->PersonalityId(), *personality );
                    break;
                case KUsbManDevicePersonalitiesPropertyKey:
                    personality->SetProperty(ReadKeyL( key ));
                    break;
                case KUsbManDevicePersonalitiesDescriptionKey:
                    {
                    HBufC* description;
                    description = ReadStringKeyLC( key );
                    personality->SetDescription( description );
                    CleanupStack::PopAndDestroy( description );
                    break;
                    }
                default:
                    OstTrace1( TRACE_FATAL, CUSBMANCENREPMANAGER_READPERSONALITIESL_DUP5, "CUsbManCenRepManager::ReadPersonalitiesL;Panic error=%d", ECenRepConfigError );
                    User::Panic( KUsbCenRepPanic, ECenRepConfigError );
                    break;
                }
            }
        
        personality->SetVersion(iSignature);
        
        //The following code is to check whether we support this personality. 
        if(personality->FeatureId() != KUsbManFeatureNotConfigurable)
            {
            if(!IsFeatureSupportedL(personality->FeatureId()))
                {
                CleanupStack::PopAndDestroy(personality);
                continue;           
                }
            }
        
        //The following code is to check whether we support this personality. It will not include:
        //1)the personality which contains single class which is not supported
        //2)the personality which contains multiple classes which are all not supported
        TBool isPersonalitySupport = EFalse;
        TInt configurationCount = personality->PersonalityConfigs().Count();
        for(TInt configurationIdx = 0; configurationIdx < configurationCount; ++configurationIdx)
            {
            const RPointerArray<CPersonalityConfigurations>& personalityConfigs = personality->PersonalityConfigs();
            CPersonalityConfigurations *personalityConfigurations = personalityConfigs[configurationIdx];
            TInt classesCount = personalityConfigurations->Classes().Count();
            if(0 != classesCount)
                {
                isPersonalitySupport = ETrue;
                break;
                }
            }
       
        if(isPersonalitySupport)
            {
            OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READPERSONALITIESL_DUP6, "CUsbManCenRepManager::ReadPersonalitiesL; Personality ID: %d is supported", personality->PersonalityId() );
            aPersonalities.Append( personality );
            CleanupStack::Pop( personality );
            }
        else
            {
            OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READPERSONALITIESL_DUP7, "CUsbManCenRepManager::ReadPersonalitiesL;Personality ID: %d is not supported", personality->PersonalityId() );
            CleanupStack::PopAndDestroy(personality);
            }
        }
    CleanupStack::PopAndDestroy( &keyArray );  	
    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_READPERSONALITIESL_EXIT );
    }

// ---------------------------------------------------------------------------
// Read configuration table for specific personality
// ---------------------------------------------------------------------------
//
void CUsbManCenRepManager::ReadConfigurationsForPersonalityL(TInt aPersonalityId, CPersonality& aPersonality)
    {
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_READCONFIGURATIONSFORPERSONALITYL_ENTRY );
    RArray<TUint32> configArray;
    CleanupClosePushL(configArray);
 
    //Only support version four right now.
    if(TUsbManagerSupportedVersionFour != iSignature)
        {
        OstTrace1( TRACE_FATAL, CUSBMANCENREPMANAGER_READCONFIGURATIONSFORPERSONALITYL, "CUsbManCenRepManager::ReadConfigurationsForPersonalityL;Panic error=%d", ECenRepConfigError );
        __ASSERT_DEBUG( EFalse, User::Panic( KUsbCenRepPanic, ECenRepConfigError ) );
        }

    TInt err = iRepository->FindEqL( KUsbManDeviceConfigurationsPartialKey, KUsbManConfigFirstEntryMask, aPersonalityId, configArray );
    LEAVEIFERRORL( err, OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_READCONFIGURATIONSFORPERSONALITYL_DUP1, "CUsbManCenRepManager::ReadConfigurationsForPersonalityL;Leave err=%d", err ); );
    
    // Get the configuration count.
    TUint32 configCount = configArray.Count();
    TUint32 totalConfigCount = ReadKeyL( KUsbManDeviceConfigurationsCountIndexKey );

    OstTraceExt3( TRACE_NORMAL, CUSBMANCENREPMANAGER_READCONFIGURATIONSFORPERSONALITYL_DUP2, 
            "CUsbManCenRepManager::ReadConfigurationsForPersonalityL;aPersonalityId = %d total configCount = %d configArray.Count() = %d", aPersonalityId, totalConfigCount, configArray.Count() );
    
    //This is intend to handle one special case that key 0x2ff00's value
    // equal our target personality id.
    if(totalConfigCount == aPersonalityId)
        {
        --configCount;
        }
    
    TInt keyCount = 0;
    if(TUsbManagerSupportedVersionFour == iSignature)
        {
        keyCount = KUsbManDeviceConfigurationsKeyCountVersionFour;
        }
    
    // Go through all configurations belonging to this personality 'aPersonalityId'
    for ( TInt configIdx = 0; configIdx < configCount; configIdx++ )
        {
        CPersonalityConfigurations* config = new ( ELeave ) CPersonalityConfigurations;
        CleanupStack::PushL( config );
        TUint32 key = configArray[configIdx];

        // Get each key value in the configuration and store it
        for ( TInt keyIdx = 0; keyIdx < keyCount; keyIdx++ )
            {
            TUint32 fieldId = ( (key + keyIdx ) & KUsbManConfigFieldMask );
            TInt keyValue = -1;
            OstTraceExt3( TRACE_NORMAL, CUSBMANCENREPMANAGER_READCONFIGURATIONSFORPERSONALITYL_DUP3, "CUsbManCenRepManager::ReadConfigurationsForPersonalityL;fieldId=%d;configIdx=%d;keyIdx=%d", fieldId, configIdx, keyIdx );
            
            if(KUsbManDeviceConfigurationsPersonalityIdKey == fieldId)
                {
                TRAPD( err, keyValue = ReadKeyL( key + keyIdx ) );
                if( err == KErrNone )
                    {
                    if(keyValue != aPersonalityId)
                        {
                        OstTrace1( TRACE_FATAL, CUSBMANCENREPMANAGER_READCONFIGURATIONSFORPERSONALITYL_DUP4, "CUsbManCenRepManager::ReadConfigurationsForPersonalityL;Panic error=%d", ECenRepConfigError );
                        __ASSERT_DEBUG( EFalse, User::Panic( KUsbCenRepPanic, ECenRepConfigError ) );
                        }
                    config->SetPersonalityId( keyValue );
                    }
                }
            else if(KUsbManDeviceConfigurationsIdKey == fieldId)
                {
                TRAPD( err, keyValue = ReadKeyL( key + keyIdx ) );
                if( err == KErrNone )
                    {
                    config->SetConfigId(keyValue);
                    }
                }
            else if(KUsbManDeviceConfigurationsClassUidsKey == fieldId)
                {
                HBufC* keyValueBuf = ReadStringKeyLC( key + keyIdx );                
                TPtr keyPtr = keyValueBuf->Des();
                
                RArray<TUint> classUids;
                CleanupClosePushL( classUids );

                iUsbDevice.ConvertUidsL( keyPtr, classUids );
                TInt uidsCnt = classUids.Count();
                 
                // Get featureId of each class and store each class.
                TInt featureId = KUsbManFeatureNotConfigurable;
                CPersonalityConfigurations::TUsbClasses usbClass;                     
                for ( TInt uidIdx = 0; uidIdx < uidsCnt; uidIdx++ )
                    {
                    usbClass.iClassUid = TUid::Uid( classUids[uidIdx] );
                    usbClass.iFeatureId = featureId; // By default
                    if ( IsClassConfigurableL( classUids[uidIdx], featureId ) )
                        {                                
                        usbClass.iFeatureId = featureId;
                        if(IsFeatureSupportedL(featureId))
                            {
                            config->AppendClassesL( usbClass );
                            }
                        }
                    else
                        {
                        config->AppendClassesL( usbClass );
                        }
                    }

                CleanupStack::PopAndDestroy( &classUids ); // close uid array 
                CleanupStack::PopAndDestroy( keyValueBuf );
                }
            else
                {
                OstTrace1( TRACE_FATAL, CUSBMANCENREPMANAGER_READCONFIGURATIONSFORPERSONALITYL_DUP5, "CUsbManCenRepManager::ReadConfigurationsForPersonalityL;Panic error=%d", ECenRepConfigError );
                User::Panic( KUsbCenRepPanic, ECenRepConfigError );
                }
            }
        aPersonality.AppendPersonalityConfigsL( config );

        CleanupStack::Pop( config );
        }

    CleanupStack::PopAndDestroy( &configArray );     
    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_READCONFIGURATIONSFORPERSONALITYL_EXIT );
    }

// ---------------------------------------------------------------------------
// Check the class belonging to a personality configurable or not.
// ---------------------------------------------------------------------------
//
TBool CUsbManCenRepManager::IsClassConfigurableL(TUint aClassId, TInt& aFeatureId)
    {
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_ISCLASSCONFIGURABLEL_ENTRY );
    TBool classConfigurable = EFalse;
    RArray<TUint32> keyArray;
    CleanupClosePushL(keyArray);
    
    TInt err = iRepository->FindEqL( KUsbManDeviceConfigurableClassesPartialKey, KUsbManConfigFirstEntryMask, (TInt)aClassId, keyArray );
    OstTraceExt2( TRACE_NORMAL, CUSBMANCENREPMANAGER_ISCLASSCONFIGURABLEL, "CUsbManCenRepManager::IsClassConfigurableL;aClassId=0x%x;err=%d", aClassId, err );
    switch ( err )
        {
        case KErrNotFound:
            break;
        case KErrNone:
            {
#ifdef _DEBUG
            if(keyArray.Count() != 1)
                {
                OstTrace1( TRACE_FATAL, DUP1_CUSBMANCENREPMANAGER_ISCLASSCONFIGURABLEL_DUP1, "CUsbManCenRepManager::IsClassConfigurableL;panic error=%d", ECenRepConfigError );
                User::Panic( KUsbCenRepPanic, ECenRepConfigError );
                }
#endif
            // The array size always is 1, so here using 0 as index.
            aFeatureId = ReadKeyL( keyArray[0] | KUsbManDeviceConfigurableClassesFeatureIdKey );
            classConfigurable = ETrue;
            break;
            }
        default:
            OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_ISCLASSCONFIGURABLEL_DUP2, "CUsbManCenRepManager::IsClassConfigurableL;Leave err=%d", err );
            User::Leave( err );
            break;
        }    
    
    CleanupStack::PopAndDestroy( &keyArray );     
    OstTraceFunctionExit0( CUSBMANCENREPMANAGER_ISCLASSCONFIGURABLEL_EXIT );
    return classConfigurable;
    }

// ---------------------------------------------------------------------------
// Check the class belonging to a personality support or not.
// ---------------------------------------------------------------------------
//
TBool CUsbManCenRepManager::IsFeatureSupportedL(TInt aFeatureId)
    {
    OstTraceFunctionEntry0( CUSBMANCENREPMANAGER_ISFEATURESUPPORTEDL_ENTRY );
#ifdef SYMBIAN_FEATURE_MANAGER
    if(CFeatureDiscovery::IsFeatureSupportedL(TUid::Uid(aFeatureId)))
        {
        OstTrace1( TRACE_NORMAL, DUP1_CUSBMANCENREPMANAGER_ISFEATURESUPPORTEDL_DUP1, "CUsbManCenRepManager::IsFeatureSupportedL;featureId = 0x%x supported", aFeatureId );
        OstTraceFunctionExit0( CUSBMANCENREPMANAGER_ISFEATURESUPPORTEDL_EXIT );
        return ETrue;
        }
    else
        {
        OstTrace1( TRACE_NORMAL, CUSBMANCENREPMANAGER_ISFEATURESUPPORTEDL_DUP2, "CUsbManCenRepManager::IsFeatureSupportedL;featureId = 0x%x not supported", aFeatureId );
        OstTraceFunctionExit0( CUSBMANCENREPMANAGER_ISFEATURESUPPORTEDL_EXIT_DUP1 );
        return EFalse;
        }
#else
    OstTrace1( TRACE_FATAL, CUSBMANCENREPMANAGER_ISFEATURESUPPORTEDL, "CUsbManCenRepManager::IsFeatureSupportedL;panic error code=%d", ECenRepFeatureManagerError );
    User::Panic( KUsbCenRepPanic, ECenRepFeatureManagerError )
#endif    
    }
