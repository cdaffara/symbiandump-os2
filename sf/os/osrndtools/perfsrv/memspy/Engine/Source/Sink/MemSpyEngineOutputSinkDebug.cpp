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
* Description:
*
*/

#include "MemSpyEngineOutputSinkDebug.h"

// System includes
#include <e32svr.h>

// Constants
const TInt KMemSpyEngineOutputSinkDebugMaxLineLength = 0x100;


CMemSpyEngineOutputSinkDebug::CMemSpyEngineOutputSinkDebug( CMemSpyEngine& aEngine )
:   CMemSpyEngineOutputSink( aEngine )
    {
    }


CMemSpyEngineOutputSinkDebug::~CMemSpyEngineOutputSinkDebug()
    {
    }


void CMemSpyEngineOutputSinkDebug::ConstructL()
    {
    BaseConstructL();
    }


CMemSpyEngineOutputSinkDebug* CMemSpyEngineOutputSinkDebug::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineOutputSinkDebug* self = new(ELeave) CMemSpyEngineOutputSinkDebug( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


TMemSpySinkType CMemSpyEngineOutputSinkDebug::Type() const
    {
    return ESinkTypeDebug;
    }


void CMemSpyEngineOutputSinkDebug::DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension )
    {
    DataStreamBeginL( aContext, aFolder, aExtension, ETrue );
    }


void CMemSpyEngineOutputSinkDebug::DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite )
    {
    DataStreamBeginL( aContext, aFolder, aExtension, aOverwrite, ETrue );
    }


void CMemSpyEngineOutputSinkDebug::DataStreamBeginL( const TDesC& /*aContext*/, const TDesC& /*aFolder*/, const TDesC& /*aExtension*/, TBool /*aOverwrite*/, TBool /*aUseTimeStamp*/ )
    {
    // Doesn't support data streams
    }


void CMemSpyEngineOutputSinkDebug::DataStreamEndL()
    {
    // Doesn't support data streams
    }


void CMemSpyEngineOutputSinkDebug::DoOutputLineL( const TDesC& aLine )
    {
    if  ( !aLine.Length() )
        {
        // Blank line is special case
        RDebug::Printf( " " );
        }
    else
        {
        _LIT( KPrintFormat, "%S" );

        HBufC* buf = aLine.AllocLC();
        if  ( aLine.Length() > KMemSpyEngineOutputSinkDebugMaxLineLength )
            {
            buf->Des().SetLength( KMemSpyEngineOutputSinkDebugMaxLineLength );
            }
        RDebug::Print( KPrintFormat, buf );
        CleanupStack::PopAndDestroy( buf );
        }
    }


void CMemSpyEngineOutputSinkDebug::DoOutputRawL( const TDesC8& /*aData*/ )
    {
    User::Leave( KErrNotSupported );
    }


