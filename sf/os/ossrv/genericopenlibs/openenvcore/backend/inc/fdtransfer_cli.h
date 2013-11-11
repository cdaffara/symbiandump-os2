// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// File descriptor transfer server - client interface
// 
//

#ifndef __FDTRANSFER_CLI_H__
#define __FDTRANSFER_CLI_H__

#include <e32std.h>
#include <f32file.h>

NONSHARABLE_CLASS(RFileDesTransferSession) : public RSessionBase
    {
public:
    TInt Connect(TDesC& aServerName);
    TInt TransferPipesToServer(); //this transfers pipes
    TInt TransferFilesToServer();  //this transfers files
    
    };

#endif // __FDTRANSFER_CLI_H__
