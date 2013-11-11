/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : signalclient.h
* Part of     : Signal server
* 
*
*/



#ifndef SIGNALSESSION_H
#define SIGNALSESSION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "rpipe.h"

// CLASS DECLARATION

/**
 *  RSignalSession
 *
 */
class RSignalSession : public RSessionBase
	{
public:
	RSignalSession():iIsConnected(EFalse)
				{iLock.CreateLocal();}

	TInt SendPipe(RPipe& aPipe);
	TInt SendSignal(TInt aPid,TInt aSignal);
	TInt SendSignalValue(TInt aPid,TInt aSignal,TInt aValue);
	inline void Close()
	    {
	    iLock.Close();
	    RSessionBase::Close();
	    }

private:
	TInt OnDemandConnection();

	void Lock() { iLock.Wait(); }

	void UnLock() { iLock.Signal(); }
	RFastLock iLock;
	TInt Connect();
	TInt iIsConnected;
	//todo put a lock around this list
	};

#endif // SIGNALSESSION_H
