/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanPRNGenerator class.
*
*/

/*
* %version: 6 %
*/

#ifndef WLANPNRGENERATOR_H
#define WLANPNRGENERATOR_H

/**
*  Class implements a pseudorandom number generator of 32 bit unsigned integers
*
*  Full periodic PRN generator. 
*  All the numbers between 0 and 2pow31 - 1 are generated before repeating.
*  Executes efficiently with 32-bit arithmetic
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanPRNGenerator
    {
    enum { EMultiplier = 16807 /* 7pow5 */};
    enum { EModulus = 2147483647 /* 2pow31 - 1 */};

public:

    /**
    * C++ default constructor.
    */
    inline WlanPRNGenerator();

    /**
    * PRN generation function.
    * @since S60 3.1
    * @return generated PRN
    */
    inline TUint32 Generate();
                        
private:

    // Prohibit copy constructor
    WlanPRNGenerator( const WlanPRNGenerator& );
    // Prohibit assigment operator
    WlanPRNGenerator& operator= ( const WlanPRNGenerator& );   

private: // Data

    /** PRN value */
    TUint32 iValue;
    };

#include "UmacPRNGenerator.inl"

#endif      // WLANPNRGENERATOR_H
