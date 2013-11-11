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
// Messages that may be exchanged between the fd transfer server and client
// 
//

#ifndef __FDTRANSFERMSG_H__
#define __FDTRANSFERMSG_H__

#include <e32std.h>

const TInt KFDTransferMaxMessageLen = 20;

enum TFDTransferMessageType
    {
      ETransferFile,
      ETransferPipe
    };
    
#endif // __FDTRANSFERMSG_H__
