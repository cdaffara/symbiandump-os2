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
 @file
*/

#ifndef USBMANCENREPMANAGER_H
#define USBMANCENREPMANAGER_H

#include <e32base.h>

class CRepository;
class CPersonality;
class TUsbDeviceDescriptor;
class CUsbDevice;

/** Helper class for tasks related to central repository. It reads settings from
 *  appropriate cenrep keys */
NONSHARABLE_CLASS( CUsbManCenRepManager ) : public CBase
{
	public:

        /** Two-phased constructor. */
        static CUsbManCenRepManager* NewL(CUsbDevice& aUsbDevice);

        /** Destructor. */
        virtual ~CUsbManCenRepManager();
        
        void ReadPersonalitiesL(RPointerArray<CPersonality>& aPersonalities);

        void ReadDeviceConfigurationL(CUsbDevice::TUsbDeviceConfiguration& aDeviceConfig);

	private:
		CUsbManCenRepManager(CUsbDevice& aUsbDevice);
		
		void ConstructL();
		
        void CheckSignatureL();
		
		void ReadConfigurationsForPersonalityL(TInt aPersonalityId, CPersonality& aPersonality);

		HBufC* ReadStringKeyLC( TUint32 aKeyId );
	    TInt   ReadKeyL ( TUint32 aKeyId );

        TBool IsClassConfigurableL(TUint aClassId, TInt& aFeatureId);
		TBool IsFeatureSupportedL(TInt aFeatureId);

	private:
	    /** Own repository instance */
		CRepository* 			iRepository;
        /**reference to CUsbDevice*/
        CUsbDevice& iUsbDevice;
        /*the USB Configuratiron signature*/
        TInt iSignature;
};

#endif /*USBMANCENREPMANAGER_H*/
