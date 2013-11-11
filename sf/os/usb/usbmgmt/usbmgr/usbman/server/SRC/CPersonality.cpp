/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements a utility class which holds information about a USB personality
*
*/

/**
 @file
 @internalAll
*/

#include "CPersonality.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CPersonalityTraces.h"
#endif

// Panic category only used in debug builds
#ifdef _DEBUG
_LIT(KUsbPersonalityPanicCategory, "CUsbPersonality");
#endif

/**
 * Panic codes for the USB Personality Class
 */
enum TUsbPersonalityPanic
    {
    EPersonalityConfigsArrayEmpty, 
    };

/**
 * Factory method. Constructs a CPersonality object. 
 *
 * @return a pointer to CPersonality object.
 */
CPersonality* CPersonality::NewL()
	{
	OstTraceFunctionEntry0( CPERSONALITY_NEWL_ENTRY );

	CPersonality* self = new(ELeave) CPersonality;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CPERSONALITY_NEWL_EXIT );
	return self;
	}

/**
 * Allocates max amount of memory for description string
 */	
void CPersonality::ConstructL()
	{
	OstTraceFunctionEntry0( CPERSONALITY_CONSTRUCTL_ENTRY );
	iDescription	= HBufC::NewL(KUsbStringDescStringMaxSize);
	OstTraceFunctionExit0( CPERSONALITY_CONSTRUCTL_EXIT );
	}
	
/**
 * standard constructor
 */
CPersonality::CPersonality()
	{
	}

/**
 * destructor
 */
CPersonality::~CPersonality()
	{
	OstTraceFunctionEntry0( CPERSONALITY_CPERSONALITY_DES_ENTRY );
	
	iPersonalityConfigs.ResetAndDestroy();
	delete iDescription;
	OstTraceFunctionExit0( CPERSONALITY_CPERSONALITY_DES_EXIT );
	}

/**
 * @return supported class uids
 */
const RArray<CPersonalityConfigurations::TUsbClasses>& CPersonality::SupportedClasses() const
    {
    //we only support configuration 0 now
    if(iPersonalityConfigs.Count() == 0)
        {
        OstTrace1( TRACE_FATAL, CPERSONALITY_SUPPORTEDCLASSES, "CPersonality::SupportedClasses;Panic error=%d", EPersonalityConfigsArrayEmpty );
        __ASSERT_DEBUG( EFalse, User::Panic(KUsbPersonalityPanicCategory, EPersonalityConfigsArrayEmpty) );
        }
    return iPersonalityConfigs[0]->Classes();
    }

/**
 * @return ETrue if this class is supported  
 * otherwise return EFalse
 */
TBool CPersonality::ClassSupported(TUid aClassUid) const
	{
    //we only support configuration 0 now
    if(iPersonalityConfigs.Count() == 0)
        {
        OstTrace1( TRACE_FATAL, CPERSONALITY_CLASSSUPPORTED, "CPersonality::ClassSupported;Panic error=%d", EPersonalityConfigsArrayEmpty );
        __ASSERT_DEBUG( EFalse, User::Panic(KUsbPersonalityPanicCategory, EPersonalityConfigsArrayEmpty) );
        }
    const RArray<CPersonalityConfigurations::TUsbClasses> &classes = iPersonalityConfigs[0]->Classes();
    TInt classesCount = classes.Count();
    for(TInt classesIndex = 0; classesIndex < classesCount; ++classesIndex)
        {
        if(aClassUid == classes[classesIndex].iClassUid)
            {
            return ETrue;
            }
        }
    return EFalse;
	}

/**
 * Compares if two class uids are equal
 *
 * @return 1 if they are equal or 0 otherwise
 */
TInt CPersonality::Compare(const TUid&  aFirst, const TUid& aSecond)
	{
	return aFirst == aSecond;
	};

/**
 * Sets version
 */
void CPersonality::SetVersion(TInt aVersion)
	{
	iVersion = aVersion;
	}

/**
 * Sets property
 */
void CPersonality::SetProperty(TUint32 aProperty)
	{
	iProperty = aProperty;
	}

/**
 * Sets DeviceClass
 */
void CPersonality::SetDeviceClass(TUint8 aDeviceClass)
    {
    iDeviceClass = aDeviceClass;
    }

/**
 * Sets DeviceSubClass
 */
void CPersonality::SetDeviceSubClass(TUint8 aDeviceSubClass)
    {
    iDeviceSubClass = aDeviceSubClass;
    }

/**
 * Sets DeviceProtocol
 */
void CPersonality::SetDeviceProtocol(TUint8 aDeviceProtocol)
    {
    iDeviceProtocol = aDeviceProtocol;
    }

/**
 * Sets NumConfigurations
 */
void CPersonality::SetNumConfigurations(TUint8 aNumConfigurations)  
    {
    iNumConfigurations = aNumConfigurations;
    }
    
/**
 * Sets ProductId
 */
void CPersonality::SetProductId(TUint16 aProductId)
    {
    iProductId = aProductId;
    }
    
/**
 * Sets FeatureId
 */
void CPersonality::SetFeatureId(TInt aFeatureId)
    {
    iFeatureId = aFeatureId;
    }
  
/**
 * Sets BcdDevice
 */
void CPersonality::SetBcdDevice(TUint16 aBcdDevice)
    {
    iBcdDevice = aBcdDevice;
    }

/**
 * Sets personality id
 */
void CPersonality::SetPersonalityId(TInt aPersonalityId)
    {
    iPersonalityId = aPersonalityId;
    }

/**
 * Sets Description
 */
void CPersonality::SetDescription(const TDesC* aDescription)
    {
    iDescription->Des().Copy((*aDescription).Left(KUsbStringDescStringMaxSize-1));
    }

/**
 * Append PersonalityConfig
 */
void CPersonality::AppendPersonalityConfigsL(const CPersonalityConfigurations *aPersonalityConfig )
    {
    iPersonalityConfigs.AppendL(aPersonalityConfig);
    }

/**
 * Sets personality id
 */
void CPersonalityConfigurations::SetPersonalityId(TInt aPersonalityId)
    {
    iPersonalityId = aPersonalityId;
    }

/**
 * Sets Config Id 
 */
void CPersonalityConfigurations::SetConfigId(TInt aConfigId)
    {
    iConfigId = aConfigId;
    }


/**
 * Append PersonalityConfig
 */
void CPersonalityConfigurations::AppendClassesL(const TUsbClasses &aClasses )
    {
    iClasses.AppendL(aClasses);
    }

/**
 * De-Constructor
 */
CPersonalityConfigurations::~CPersonalityConfigurations()
    {
    OstTraceFunctionEntry0( CPERSONALITYCONFIGURATIONS_CPERSONALITYCONFIGURATIONS_DES_ENTRY );
    iClasses.Close();
    OstTraceFunctionExit0( CPERSONALITYCONFIGURATIONS_CPERSONALITYCONFIGURATIONS_DES_EXIT );
    }
