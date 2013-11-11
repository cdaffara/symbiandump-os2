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
* Description:   Declaration of the WlanAuthSeqNmbrExpected class.
*
*/

/*
* %version: 7 %
*/

#ifndef WLANAUTHSEQNMBREXPECTED_H
#define WLANAUTHSEQNMBREXPECTED_H

#include "802dot11.h"

/**
*  Class for counting the expected authentication sequence number
*/
class WlanAuthSeqNmbrExpected
    {
public: 

    /**
    * C++ default constructor.
    */
    WlanAuthSeqNmbrExpected() :
         iAuthSeqNmbrExpected( E802Dot11AuthenticationSeqNmbr2 ) {};

    /**
    * Prefix ++ operator
    * @return authentication sequence number expected
    */
    inline TUint8 operator++();

    /**
    * Postfix ++ operator
    * @return authentication sequence number expected
    */
    inline TUint8 operator++( TInt );

    /** Reset clas state to intial state */
    inline void Reset();

    /**
    * Gets authentication sequence number expected 
    * @return see above
    */
    inline TUint8 GetAuthSeqNmbrExpected() const;
                    
private:

    // Prohibit copy constructor.
    WlanAuthSeqNmbrExpected( const WlanAuthSeqNmbrExpected& );
    // Prohibit assigment operator.
    WlanAuthSeqNmbrExpected& operator= ( 
        const WlanAuthSeqNmbrExpected& );   

private: // Data

    /** authentication sequnce number expected */
    TUint8 iAuthSeqNmbrExpected;
    };

inline TUint8 WlanAuthSeqNmbrExpected::operator++()
    {
    iAuthSeqNmbrExpected = E802Dot11AuthenticationSeqNmbr4;
    return iAuthSeqNmbrExpected;
    }

inline TUint8 WlanAuthSeqNmbrExpected::operator++( TInt )
    {
    iAuthSeqNmbrExpected = E802Dot11AuthenticationSeqNmbr4;
    return iAuthSeqNmbrExpected;
    }

inline void WlanAuthSeqNmbrExpected::Reset()
    {
    iAuthSeqNmbrExpected = E802Dot11AuthenticationSeqNmbr2;
    }

inline TUint8 WlanAuthSeqNmbrExpected::GetAuthSeqNmbrExpected() const
    {
    return iAuthSeqNmbrExpected;
    }

#endif // WLANAUTHSEQNMBREXPECTED_H
