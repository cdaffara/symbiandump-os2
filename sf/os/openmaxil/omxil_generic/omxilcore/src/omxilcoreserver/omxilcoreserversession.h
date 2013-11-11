// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef OMXILCORESERVERSESSION_H
#define OMXILCORESERVERSESSION_H

#include <e32base.h>
#include "omxilcoreclientserver.h"

class COmxILCore;

NONSHARABLE_CLASS(COmxILCoreServerSession) : public CSession2
	{
public:
	static COmxILCoreServerSession* NewL(COmxILCore& aCore, COmxILCoreServer& aCoreServer);

	~COmxILCoreServerSession();

	// from CSession2
	void ServiceL(const RMessage2& aMessage);

private:
	COmxILCoreServerSession(COmxILCore& aCore, COmxILCoreServer& aCoreServer);
	TInt DispatchMessageL(const RMessage2& aMessage);

private:

	COmxILCore& iCore;
	COmxILCoreServer& iCoreServer;

	};

#endif // OMXILCORESERVERSESSION_H
