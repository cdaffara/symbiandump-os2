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


// INCLUDE FILES
#include <bt_sock.h>

#include "messageservicesearcher.h"
#include "btservicesearcher.pan"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSdpAttributeParser::SSdpAttributeNode[]
// Construct a TSdpAttributeParser.
// -----------------------------------------------------------------------------
//
static const TSdpAttributeParser::SSdpAttributeNode gSerialPortProtocolArray[] =
    {
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckValue, ETypeUUID, KL2CAP },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckType, ETypeDES },
        { TSdpAttributeParser::ECheckValue, ETypeUUID, KRFCOMM }, // KRFCOMM defined in bt_sock.h
        { TSdpAttributeParser::EReadValue, ETypeUint, KRfcommChannel }, // KRfcommChannel is custom key value used in FoundElementL. If there were more than one value to be read, the key value would be usefull
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::EFinished }
    };

// ----------------------------------------------------------------------------
// TStaticArrayC
// Provides a type and size, safe method of using static arrays.
// ----------------------------------------------------------------------------
//
static const TStaticArrayC<TSdpAttributeParser
::SSdpAttributeNode> gSerialPortProtocolList = CONSTRUCT_STATIC_ARRAY_C(
                                               gSerialPortProtocolArray );

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMessageServiceSearcher* CMessageServiceSearcher::NewL()
    {
    CMessageServiceSearcher* self = CMessageServiceSearcher::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMessageServiceSearcher* CMessageServiceSearcher::NewLC()
    {
    CMessageServiceSearcher* self = new (ELeave) CMessageServiceSearcher();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::CMessageServiceSearcher()
// Construcor.
// ----------------------------------------------------------------------------
//
CMessageServiceSearcher::CMessageServiceSearcher()
: CBTServiceSearcher(),
  iServiceClass( KServiceClass )
    {
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::~CMessageServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CMessageServiceSearcher::~CMessageServiceSearcher()
    {
    iPortNumbers.Close();
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMessageServiceSearcher::ConstructL()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::ServiceClass()
// The service class to search.
// ----------------------------------------------------------------------------
//
const TUUID& CMessageServiceSearcher::ServiceClass() const
    {
    return iServiceClass;
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::ProtocolList()
// The list of Protocols required by the service.
// ----------------------------------------------------------------------------
//
const TSdpAttributeParser::TSdpAttributeList& CMessageServiceSearcher
::ProtocolList() const
    {
    return gSerialPortProtocolList;
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::FoundElementL()
// Read the data element.
// ----------------------------------------------------------------------------
//
void CMessageServiceSearcher::FoundElementL( TInt aKey, CSdpAttrValue& aValue )
    {
    __ASSERT_ALWAYS( aKey == static_cast<TInt>( KRfcommChannel ),
        Panic( EBTServiceSearcherProtocolRead ) );
    AppendPort( aValue.Uint() );
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::Port()
// Port connection on the remote machine.
// ----------------------------------------------------------------------------
//
TInt CMessageServiceSearcher::Port( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iPortNumbers.Count() )
        {
        return KErrNotFound;
        }
    return iPortNumbers[aIndex];
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::AppendPort()
// Adds a new port number to the port number array. If the port number
// already exists in the array a duplicate is not added.
// ----------------------------------------------------------------------------
//
void CMessageServiceSearcher::AppendPort( TInt aPort )
    {
    if ( iPortNumbers.Find( aPort ) == KErrNotFound )
        {
        iPortNumbers.Append( aPort );
        }
    }

// ----------------------------------------------------------------------------
// CMessageServiceSearcher::ServiceCount()
// The amount of requested services found from the remote machine.
// ----------------------------------------------------------------------------
//
TInt CMessageServiceSearcher::ServiceCount()
    {
    return iPortNumbers.Count();
    }

// End of File
