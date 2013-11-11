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
* Description:  Declaration of the class MAnalyzeToolEventhandlerNotifier
*
*/


#ifndef ANALYZETOOLEVENTHANDLERNOTIFIER_H
#define ANALYZETOOLEVENTHANDLERNOTIFIER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  MAnalyzeToolEventhandlerNotifier class
*  An interface class for informing killed thread.
*/
class MAnalyzeToolEventhandlerNotifier
    {
    public: // New functions
           
        /**
        * Inform when thread killed.
        * @param aThreadId - Killed thread Id.
        */
        virtual void RemoveKilledThread( const TUint aThreadId ) = 0;

    };

#endif // ANALYZETOOLEVENTHANDLERNOTIFIER_H

// End of File
