/**
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
* Implements a utitility class which holds the information about a USB device personality
* 
*
*/



/**
 @file
*/

#ifndef __CPERSONALITY_INL__
#define __CPERSONALITY_INL__

/**
 * @internalComponent
 * @return version
 */ 
inline TInt CPersonality::Version() const
	{
	return iVersion;
	}

/**
 * @internalComponent
 * @return the property information
 */
inline TUint32 CPersonality::Property() const
	{
	return iProperty;
	}

/**
 * @internalComponent
 * @return the Device Class information
 */
inline TUint8 CPersonality::DeviceClass() const
    {
    return iDeviceClass;
    }

/**
 * @internalComponent
 * @return the Device Sub Class information
 */
inline TUint8 CPersonality::DeviceSubClass() const
    {
    return iDeviceSubClass;
    }

/**
 * @internalComponent
 * @return the Device Protocol information
 */
inline TUint8 CPersonality::DeviceProtocol() const
    {
    return iDeviceProtocol;
    }

/**
 * @internalComponent
 * @return the Configurations Number information
 */
inline TUint8 CPersonality::NumConfigurations() const
    {
    return iNumConfigurations;
    }

/**
 * @internalComponent
 * @return the Product Id information
 */
inline TUint16 CPersonality::ProductId() const
    {
    return iProductId;
    }

/**
 * @internalComponent
 * @return the Feature Id information
 */
inline TInt CPersonality::FeatureId() const
    {
    return iFeatureId;
    }

/**
 * @internalComponent
 * @return the Feature Id information
 */
inline TUint16 CPersonality::BcdDevice() const
    {
    return iBcdDevice;
    }

/**
 * @internalComponent
 * @return personality id
 */ 
inline TInt CPersonality::PersonalityId() const
    {
    return iPersonalityId;
    }

/**
 * @internalComponent
 * @return a const pointer to description string
 */
inline const TDesC* CPersonality::Description() const
    {
    return iDescription;
    }

/**
 * @internalComponent
 * @return a const pointer to Personality Configs
 */
inline const RPointerArray<CPersonalityConfigurations>& CPersonality::PersonalityConfigs() const
    {
    return iPersonalityConfigs;
    }


/**
 * @internalComponent
 * @return a const to Classes
 */
inline const RArray<CPersonalityConfigurations::TUsbClasses> & CPersonalityConfigurations::Classes() const
    {
    return iClasses;
    }

#endif // __PERSONALITY_INL__

