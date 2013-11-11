// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : SdpRtpmapValue.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//



#include <s32strm.h>
#include "sdprtpmapvalue.h"
#include "sdputil.h"
#include "sdpcodecerr.h"
#include "sdpcodecconstants.h"
#include "_sdpdefs.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSdpRtpmapValue::TSdpRtpmapValue
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TSdpRtpmapValue::TSdpRtpmapValue( 
    const TPtrC8& aEncodingName,
    const TPtrC8& aClockrate,
    const TPtrC8& aEncodingParameters ) :
    iEncName( aEncodingName ),
    iClockrate( aClockrate ),
    iEncParams( aEncodingParameters )
    {
    }

// -----------------------------------------------------------------------------
// TSdpRtpmapValue::DecodeL
// Decodes string and puts it into parts
// -----------------------------------------------------------------------------
//
EXPORT_C TSdpRtpmapValue TSdpRtpmapValue::DecodeL( 
    const TDesC8& aText )
    {
    __ASSERT_ALWAYS(aText.Length() > 0 && 
                    aText.Locate( KSlashChar ) != KErrNotFound, 
        User::Leave(KErrSdpCodecMediaAttributeField));

    TInt length( aText.Length() );
    if ( aText[length - 1] == KLFChar )
        {
        if ( length > 1 && aText[length - 2] == KCRChar )
            {
            length--;
            }
        length--;
        }  
    TPtrC8 restValue( aText.Left( length ) );

    __ASSERT_ALWAYS(SdpUtil::IsByteString(restValue), 
                    User::Leave(KErrSdpCodecMediaAttributeField)); 

    TInt pos = restValue.Locate( KSlashChar );
    
    // <encoding name>
    TPtrC8 encName( restValue.Left( pos ) );    
    
    restValue.Set( restValue.Right( restValue.Length() - pos - 1 ) );
    pos = restValue.Locate( KSlashChar );

    // <clock rate> <encoding parameters>
    TPtrC8 encParam( KNullDesC8 );
    TPtrC8 clockRate( KNullDesC8 );
   
    if ( pos == KErrNotFound )
        {              
        clockRate.Set( restValue );      
        
        __ASSERT_ALWAYS( clockRate.Length() > 0 && encParam.Length() == 0,
                         User::Leave( KErrSdpCodecMediaAttributeField ) );          
        }
    else
        {
        clockRate.Set( restValue.Left( pos ) );      
        encParam.Set( restValue.Right( restValue.Length() - pos - 1 ) );

        __ASSERT_ALWAYS( clockRate.Length() > 0 && encParam.Length() > 0,
                         User::Leave( KErrSdpCodecMediaAttributeField ) );
        }
   
    return TSdpRtpmapValue( encName, clockRate, encParam );
    }

// -----------------------------------------------------------------------------
// TSdpRtpmapValue::EncodeL
// Encodes a string into valid output string format
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* TSdpRtpmapValue::EncodeL() const
    {
    TUint length = iEncName.Length() + iClockrate.Length() + 2;

    if (iEncParams.Length() > 0)
        {
        length += iEncParams.Length() + 1;
        }

    HBufC8* returnValue = HBufC8::NewL(length);

    TPtr8 retValPtr(returnValue->Des());
    retValPtr.Append(iEncName);
    retValPtr.Append(KSlashStr);
    retValPtr.Append(iClockrate);

    if (iEncParams.Length() > 0)
        {
        retValPtr.Append(KSlashStr);
        retValPtr.Append(iEncParams);
        }

    return returnValue;
    }
