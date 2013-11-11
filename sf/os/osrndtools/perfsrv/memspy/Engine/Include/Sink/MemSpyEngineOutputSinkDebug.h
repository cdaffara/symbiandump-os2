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

#ifndef MEMSPYENGINEOUTPUTSINKDEBUG_H
#define MEMSPYENGINEOUTPUTSINKDEBUG_H

// System includes
#include <e32base.h>

// User includes
#include <memspy/engine/memspyengineoutputsink.h>

// Classes referenced
class CMemSpyEngine;



NONSHARABLE_CLASS( CMemSpyEngineOutputSinkDebug ) : public CMemSpyEngineOutputSink
    {
public:
    static CMemSpyEngineOutputSinkDebug* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineOutputSinkDebug();

public:
    CMemSpyEngineOutputSinkDebug( CMemSpyEngine& aEngine );
    void ConstructL();

private: // From CMemSpyEngineOutputSink
    TMemSpySinkType Type() const;
    void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension );
    void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite );
    void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseTimeStamp );
    void DataStreamEndL();
    void DoOutputLineL( const TDesC& aLine );
    void DoOutputRawL( const TDesC8& aData );
    };



#endif