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
* Description:  Implementation of recognizer that automatically starts HTI
*               on device start-up if the device is configured to load
*               data recognizers during start-up.
*
*/


#ifndef __HTIAUTOSTART_H
#define __HTIAUTOSTART_H


//  INCLUDES
#include <apmrec.h>
#include <e32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Implements CHtiAutostart
*/
class CHtiAutostart : public CApaDataRecognizerType
    {
    public: // new functions
        static CApaDataRecognizerType* CreateRecognizerL();
        CHtiAutostart();
        static void StartThread();
        static TInt StartAppThreadFunction( TAny* aParam );
        static void StartAppThreadFunctionL();

    public: // from CApaDataRecognizerType

        TUint PreferredBufSize();
        TDataType SupportedDataTypeL( TInt aIndex ) const;

    private: // from CApaDataRecognizerType

        void DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer );
    };

#endif // __HTIAUTOSTART_H

// End of File
