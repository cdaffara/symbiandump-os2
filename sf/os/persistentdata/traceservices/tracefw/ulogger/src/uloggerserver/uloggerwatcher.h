// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ULogger data watcher
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/


#ifndef __ULOGGERWATCHER_H__
#define __ULOGGERWATCHER_H__

#include <e32base.h>

namespace Ulogger
{
	

/**MDataWatcherObserver class
*/
class MDataWatcherObserver
	{
	public:
		virtual void DataNotification() = 0;
		
	};



/**CULoggerWatcher class
*/
class CULoggerWatcher : public CActive
	{
	public:
		static CULoggerWatcher* NewL();
		static CULoggerWatcher* NewLC();
		~CULoggerWatcher();
		
		//from CActive
		void RunL();
		
		//own methods
		inline TRequestStatus& GetStatus(){return iStatus;}
		void StartWatching(MDataWatcherObserver* aObserver);
		inline void StopWatching()
		{
			Cancel();
		}
	
	private:
		CULoggerWatcher();
		void ConstructL();
		
		//from CActive
		void DoCancel();
	
		//data 
		MDataWatcherObserver* iObserver;
	};
}
#endif //__ULOGGERWATCHER_H__
