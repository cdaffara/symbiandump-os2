/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: StifMeasurement plugin names declaration
*
*/

#ifndef STIF_MEASUREMENT_H
#define STIF_MEASUREMENT_H

// INCLUDES
#include <e32base.h>

// Maximum length of measurement types(see literals below)
const TInt KStifMeasurementTypeLength = 30; 

/**
* Literals for measurement types.
*/
_LIT( KStifMeasurementEnableAll, "stifmeasurementenableall" );
_LIT( KStifMeasurementDisableAll, "stifmeasurementdisableall" );
_LIT( KStifMeasurement01, "stifmeasurementplugin01" );
_LIT( KStifMeasurement02, "stifmeasurementplugin02" );
_LIT( KStifMeasurement03, "stifmeasurementplugin03" );
_LIT( KStifMeasurement04, "stifmeasurementplugin04" );
_LIT( KStifMeasurement05, "stifmeasurementplugin05" );
_LIT( KStifMeasurementBappea, "stifbappeaprofiler" );
// Next is used in testframework.ini file
_LIT( KStifMeasurementDisableNone, "stifmeasurementdisablenone" );


// Measurement module types definition(used in keyword's parameters)
_LIT( KParamMeasurement01, "measurementplugin01" );
_LIT( KParamMeasurement02, "measurementplugin02" );
_LIT( KParamMeasurement03, "measurementplugin03" );
_LIT( KParamMeasurement04, "measurementplugin04" );
_LIT( KParamMeasurement05, "measurementplugin05" );
_LIT( KParamMeasurementBappea, "stifbappeaprofiler" );

/**
* Enumeration for measurement disable and enable.
*/
enum TDisableMeasurement
    {
    EDisableAll     = 0xFFFFFFFF,   // Disable all measurement, 1111
    EEnableAll      = ~EDisableAll, // This allowes all measurements, 0000
    EMeasurement01  = 1<<0,         // User specific measurement
    EMeasurement02  = 1<<1,         // User specific measurement
    EMeasurement03  = 1<<2,         // User specific measurement
    EMeasurement04  = 1<<3,         // User specific measurement
    EMeasurement05  = 1<<4,         // User specific measurement
    EBappea         = 1<<25,        // Disable bappea measurement
    };

#endif // STIF_MEASUREMENT_H

// End of File
