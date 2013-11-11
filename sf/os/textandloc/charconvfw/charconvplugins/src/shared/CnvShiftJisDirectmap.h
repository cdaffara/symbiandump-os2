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









#ifndef CNVSHIFTJISDIRECTMAP_H
#define CNVSHIFTJISDIRECTMAP_H

#include <e32std.h>
#include <charconv.h>


/**
*  This class has a conversion data for Vodafone pictograph.
*  CnvShiftJisDirectmap is only used by ShiftJisForFep.cpl.
*
*  @since 2.6
*/
class CnvShiftJisDirectmap
    {
    public:
        /**
        * It returns a conversion data for pictograph.
        * @since 2.6
        * @return the conversion data
        */
        IMPORT_C static const SCnvConversionData& ConversionData();
    };

#endif // CNVSHIFTJISDIRECTMAP_H

// End of File
