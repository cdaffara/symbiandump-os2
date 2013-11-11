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
* Description:  Definition of the CHtiFtpBackupFakeBase class.
*
*/


#ifndef __HTIFTPBACKUPFAKEBASE_H
#define __HTIFTPBACKUPFAKEBASE_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>

// CONSTANTS
const TUid KHtiFtpBackupFakeInterfaceUid = { 0x20022D5F };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Definition of the CHtiFtpBackupFakeBase
*/
class CHtiFtpBackupFakeBase : public CBase
    {
    public:
        virtual void ConstructL( RFs* aFs ) = 0;
        virtual TInt ActivateBackup() = 0;
        virtual TInt DeactivateBackup() = 0;
    protected:
        RFs*        iFs;        // referenced
    };


#endif // __HTIFTPBACKUPFAKEBASE_H

// End of File
