/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Inline methods of CWlanBgScanStates
*
*/

/*
* %version: 1 %
*/

// ---------------------------------------------------------
// CWlanBgScanStates::SetAwsStartupStatus
// ---------------------------------------------------------
//
inline void CWlanBgScanStates::SetAwsStartupStatus( TInt aStatus )
    {
    iAwsStartupStatus = aStatus;
    }

// ---------------------------------------------------------
// CWlanBgScanStates::SetAwsCmdStatus
// ---------------------------------------------------------
//
inline void CWlanBgScanStates::SetAwsCmdStatus( MWlanBgScanAwsComms::TAwsCommand aCmd, TInt aStatus )
    {
    iAwsCmd = aCmd;
    iAwsCmdStatus = aStatus;
    }

// ---------------------------------------------------------
// CWlanBgScanStates::SetAutoInterval
// ---------------------------------------------------------
//
inline void CWlanBgScanStates::SetAutoInterval( TUint aAutoInterval )
    {
    iAutoInterval = aAutoInterval;
    }

// End of File
