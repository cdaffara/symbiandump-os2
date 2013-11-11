// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LOGSMSPDUDATA_H
#define LOGSMSPDUDATA_H

#include <e32std.h>

class TLogSmsPduData
/**
The following should be used to store SMS PDU info in event data
@publishedAll
@released
*/
	{
public:
	TInt iType; // message type;
	TInt iTotal;   //  total number of parts
	TInt iSent;   //  total number of parts sent
	TInt iDelivered;  //  total number of parts delivered to destination
	TInt iFailed;  //  total number of parts failed to be delivered to destination
	TInt iReceived;  //  total number of parts received for DELIVERs or SUBMITs read from the phone/SIM 
	};

#endif
