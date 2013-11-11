/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Declaration of the UmacDot11Roam class
*
*/

/*
* %version: 8 %
*/

#ifndef WLANDOT11ROAM_H
#define WLANDOT11ROAM_H

#include "UmacDot11State.h"

/**
*  Roaming base class.
*  A composite state, which holds the common behaviour of its 
*  substates.
*
*  @lib wlanumac.lib
*  @since Series 60 3.1
*/
class WlanDot11Roam : public WlanDot11State
    {    
    public:

        /**
        * Destructor.
        */
        virtual ~WlanDot11Roam();
        
    protected:  // New functions

        /**
        * C++ default constructor.
        */
        WlanDot11Roam();

    private:

        // Prohibit copy constructor 
        WlanDot11Roam( const WlanDot11Roam& );
        // Prohibit assigment operator
        WlanDot11Roam& operator= ( const WlanDot11Roam& );         
    };

#endif      // WLANDOT11ROAM_H
            
// End of File
