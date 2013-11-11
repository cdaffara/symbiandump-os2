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
* Description:         This module is a plug-in module for Shift-JIS with Pictograph.
*                Basicaly, Vodafone Pictograph is encoded by ISO2022, 
*                but Japanese FEP needs a pictograph as one character code 
*                in Shift-JIS character code set.
*
*/










// INCLUDE FILES
#include "CnvShiftJisDirectmap.h"
#include "convgeneratedcpp.h"
#include <e32std.h>


// FORWARD DECLARATIONS
struct SCnvConversionData;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CnvShiftJisDirectmap::ConversionData()
// This class has a conversion data for Vodafone pictograph page1.
// CnvShiftJisDirectmap only to be used by CnvPictographShared.dll.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const SCnvConversionData& CnvShiftJisDirectmap::ConversionData()
    {
    return conversionData;
    }

//  End of File  
