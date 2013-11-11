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
* Description:  HtiNotifier implementation
*
*/

#include "HtiNotifier.h"

_LIT( KHtiError, "HtiError" );
_LIT( KHtiOkButton, "Ok" );


/**
*
* Helper function to convert Symbian os error codes
* to readable format.
*
*/
LOCAL_C HBufC* SOSErr2DesL( TInt err )
{
    TBuf<50> s;
    switch ( err )
    {
    case KErrNone:                  s.Append( _L("KErrNone") ); break;
    case KErrNotFound:              s.Append( _L("KErrNotFound") ); break;
    case KErrGeneral:               s.Append( _L("KErrGeneral") ); break;
    case KErrCancel:                s.Append( _L("KErrCancel") ); break;
    case KErrNoMemory:              s.Append( _L("KErrNoMemory") ); break;
    case KErrNotSupported:          s.Append( _L("KErrNotSupported") ); break;
    case KErrArgument:              s.Append( _L("KErrArgument") ); break;
    case KErrTotalLossOfPrecision:  s.Append( _L("KErrTotalLossOfPrecision") ); break;
    case KErrBadHandle:             s.Append( _L("KErrBadHandle") ); break;
    case KErrOverflow:              s.Append( _L("KErrOverflow") ); break;
    case KErrUnderflow:             s.Append( _L("KErrUnderflow") ); break;
    case KErrAlreadyExists:         s.Append( _L("KErrAlreadyExists") ); break;
    case KErrPathNotFound:          s.Append( _L("KErrPathNotFound") ); break;
    case KErrDied:                  s.Append( _L("KErrDied") ); break;
    case KErrInUse:                 s.Append( _L("KErrInUse") ); break;
    case KErrServerTerminated:      s.Append( _L("KErrServerTerminated") ); break;
    case KErrServerBusy:            s.Append( _L("KErrServerBusy") ); break;
    case KErrCompletion:            s.Append( _L("KErrCompletion") ); break;
    case KErrNotReady:              s.Append( _L("KErrNotReady") ); break;
    case KErrUnknown:               s.Append( _L("KErrUnknown") ); break;
    case KErrCorrupt:               s.Append( _L("KErrCorrupt") ); break;
    case KErrAccessDenied:          s.Append( _L("KErrAccessDenied") ); break;
    case KErrLocked:                s.Append( _L("KErrLocked") ); break;
    case KErrWrite:                 s.Append( _L("KErrWrite") ); break;
    case KErrDisMounted:            s.Append( _L("KErrDisMounted") ); break;
    case KErrEof:                   s.Append( _L("KErrEof") ); break;
    case KErrDiskFull:              s.Append( _L("KErrDiskFull") ); break;
    case KErrBadDriver:             s.Append( _L("KErrBadDriver") ); break;
    case KErrBadName:               s.Append( _L("KErrBadName") ); break;
    case KErrCommsLineFail:         s.Append( _L("KErrCommsLineFail") ); break;
    case KErrCommsFrame:            s.Append( _L("KErrCommsFrame") ); break;
    case KErrCommsOverrun:          s.Append( _L("KErrCommsOverrun") ); break;
    case KErrCommsParity:           s.Append( _L("KErrCommsParity") ); break;
    case KErrTimedOut:              s.Append( _L("KErrTimedOut") ); break;
    case KErrCouldNotConnect:       s.Append( _L("KErrCouldNotConnect") ); break;
    case KErrCouldNotDisconnect:    s.Append( _L("KErrCouldNotDisconnect") ); break;
    case KErrDisconnected:          s.Append( _L("KErrDisconnected") ); break;
    case KErrBadLibraryEntryPoint:  s.Append( _L("KErrBadLibraryEntryPoint") ); break;
    case KErrBadDescriptor:         s.Append( _L("KErrBadDescriptor") ); break;
    case KErrAbort:                 s.Append( _L("KErrAbort") ); break;
    case KErrTooBig:                s.Append( _L("KErrTooBig") ); break;
    case KErrDivideByZero:          s.Append( _L("KErrDivideByZero") ); break;
    case KErrBadPower:              s.Append( _L("KErrBadPower") ); break;
    case KErrDirFull:               s.Append( _L("KErrDirFull") ); break;
    case KErrHardwareNotAvailable:  s.Append( _L("KErrHardwareNotAvailable") ); break;
    case KErrSessionClosed:         s.Append( _L("KErrSessionClosed") ); break;
    case KErrPermissionDenied:      s.Append( _L("KErrPermissionDenied") ); break;
    case KErrExtensionNotSupported: s.Append( _L("KErrExtensionNotSupported") ); break;
    case KErrCommsBreak:            s.Append( _L("KErrCommsBreak") ); break;
    default:
            s.AppendFormat( _L("errorcode %d"), err );
            return s.AllocL();
    }
    s.AppendFormat( _L("(%d)"), err );
    return s.AllocL();
}

/**
*
* CHtiNotifier implementation
*
*/
void CHtiNotifier::ShowErrorL( const TDesC& aText )
{
    RNotifier notifier;
    User::LeaveIfError( notifier.Connect() );

    TRequestStatus status;
    TInt button;
    notifier.Notify( KHtiError, aText,
                     KHtiOkButton, KNullDesC, button, status );
    User::WaitForRequest( status );
    notifier.Close();
}

void CHtiNotifier::ShowErrorL( const TDesC& aText, TInt aErr )
{
    RNotifier notifier;
    User::LeaveIfError( notifier.Connect() );

    TBuf<KMaxHtiNotifierLength> errorMsg;
    errorMsg.Append( aText );
    errorMsg.Append( _L("\n") );
    HBufC* errString = SOSErr2DesL( aErr );
    errorMsg.Append( errString->Des() );
    delete errString;

    TRequestStatus status;
    TInt button;
    notifier.Notify( KHtiError, errorMsg,
                     KHtiOkButton, KNullDesC, button, status );
    User::WaitForRequest( status );
    notifier.Close();
}

