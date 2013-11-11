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


#ifndef __TIMERCLIENT_H__
#define __TIMERCLIENT_H__

#include <e32std.h> //RSessionBase
#include <e32def.h> //Varargs

class RTimerSession : public RSessionBase
	{
	public:
		RTimerSession():iIsConnected(EFalse)
			{iLock.CreateLocal();}
		TInt Connect(RServer2 aServer);
		TInt DeleteTimer(const TInt);
		TInt OnDemandConnect(RServer2 aServer);
		TInt SetTime(const TInt);
		void ResetConnectionFlag()
			{
			iIsConnected = EFalse;
			}
	private:
		void Lock()
			{
			iLock.Wait();
			}
		void UnLock()
			{
			iLock.Signal();
			}
		RFastLock iLock;
		TInt iIsConnected;
	};	
	
#endif //__TIMERCLIENT_H__
