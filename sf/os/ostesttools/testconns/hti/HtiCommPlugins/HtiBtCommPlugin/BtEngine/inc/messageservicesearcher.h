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
* Description:  Message service searcher.
*
*/


#ifndef __MESSAGESERVICESEARCHER_H__
#define __MESSAGESERVICESEARCHER_H__

// INCLUDES
#include <e32base.h>
#include "btservicesearcher.h"

// CLASS DECLARATIONS
/**
* CMessageServiceSearcher
* Searches for a message service.
*/
class CMessageServiceSearcher : public CBTServiceSearcher
    {
    public:     // Constructors and destructor

        /*
        * NewL()
        * Create a CMessageServiceSearcher object
        * @return a pointer to the created instance of
        * CMessageServiceSearcher
        */
        static CMessageServiceSearcher* NewL();

        /**
        * NewLC()
        * Create a CMessageServiceSearcher object
        * @return a pointer to the created instance of
        * CMessageServiceSearcher
        */
        static CMessageServiceSearcher* NewLC();

        /**
        * ~CMessageServiceSearcher()
        * Destroy the object and release all memory objects.
        */
        virtual ~CMessageServiceSearcher();

    public: // New functions

        /**
        * Port()
        * Retrieve the port on which the service is installed
        * @param aIndex index number of the service whose port number to get
        * @return the port number or KErrNotFound if aIndex negative or greater
        * than returned by ServiceCount()
        */
        TInt Port( TInt aIndex = 0 );

        /**
        * AppendPort()
        * Adds a new port number to the port number array. If the port number
        * already exists in the array a duplicate is not added.
        * @param aPort the port number to add
        * */
        void AppendPort( TInt aPort );

        /**
         * ServiceCount()
         * Get the amount of requested services found from the remote machine
         * @return the count of services (= ports)
         */
         TInt ServiceCount();

    protected:  // New functions

        /**
        * ServiceClass()
        * @return the service class uid.
        */
        const TUUID& ServiceClass() const;

        /**
        * ProtocolList()
        * @return the attribute list.
        */
        const TSdpAttributeParser::TSdpAttributeList& ProtocolList() const;

        /**
        * FoundElementL()
        * Read the data element
        * @param aKey a key that identifies the element
        * @param aValue the data element
        */
        virtual void FoundElementL( TInt aKey, CSdpAttrValue& aValue );

    private:    // Constructors

        /**
        * CMessageServiceSearcher()
        * Constructs this object
        * @param aLog the log to send output to
        */
        CMessageServiceSearcher();

        /**
        * ConstructL()
        * Performs second phase construction of this object
        */
        void ConstructL();

    private:    // data

        /** iServiceClass the service class UUID to search for */
        TUUID iServiceClass;

        /**
        * iPortNumbers array of port numbers that has the remote
        * service installed
        */
        RArray<TInt> iPortNumbers;
    };

#endif // __MESSAGESERVICESEARCHER_H__

// End of File
