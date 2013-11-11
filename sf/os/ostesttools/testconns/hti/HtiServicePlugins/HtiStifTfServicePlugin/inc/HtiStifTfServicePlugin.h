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
* Description:  Defines the ECom plugin for HTI STIF Test Framework control
*                service.
*
*/



#ifndef HTISTIFTFSERVICEPLUGIN_H
#define HTISTIFTFSERVICEPLUGIN_H

//  INCLUDES
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHtiStifTfIf;

// CLASS DECLARATION

/**
*  The ECom plugin for HTI STIF Test Framework control service.
*
*/
class CHtiStifTfServicePlugin : public CHTIServicePluginInterface
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CHtiStifTfServicePlugin* NewL();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CHTIServicePluginInterface
        * Called by the HTI Framework when sending message to this service.
        * @param aMessage message body destinated to a servive
        * @param aPriority message priority
        */
        void ProcessMessageL( const TDesC8& aMessage,
            THtiMessagePriority aPriority );

        /**
        * From CHTIServicePluginInterface
        * Called by HTI Framework to tell the service how much memory is
        * available for messages in the message queue.
        * @param aAvailableMemory amount of currently available memory
        *        in the message queue
        */
        void NotifyMemoryChange( TInt aAvailableMemory );

        /**
        * From CHTIServicePluginInterface
        * Indicates to HTI Framework whether the plugin is ready to process
        * a new message or if it's busy processing previous message.
        */
        TBool IsBusy();

    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CHtiStifTfServicePlugin();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CHtiStifTfServicePlugin();


    protected:  // Functions from base classes

    private:


    public:     // Data

    protected:  // Data

    private:    // Data

        // Instance of the class derived from CStifTFwIf
        CHtiStifTfIf* iStifTfInterface;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#endif      // HTISTIFTFSERVICEPLUGIN_H

// End of File
