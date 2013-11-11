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
* Description:  Provides a notifier service for all HTI binaries
*
*/

#ifndef __HTINOTIFIER_H__
#define __HTINOTIFIER_H__

// INCLUDES
#include <e32base.h>
#include <e32std.h>

const static TInt KMaxHtiNotifierLength = 128;

// CLASS DECLARATION
class CHtiNotifier : public CBase
    {
public:

    /**
    * Shows a notifier with text
    * @param aText Text to display
    * @return none
    */
    static void ShowErrorL( const TDesC& aText );

    /**
    * Shows a notifier with text and symbian error code as a string
    * @param aText Formatted text to display
    * @param aErr  Errorcode to display
    * @return none
    */
    static void ShowErrorL( const TDesC& aText, TInt aErr );

    };

#endif // __HTINOTIFIER_H__
