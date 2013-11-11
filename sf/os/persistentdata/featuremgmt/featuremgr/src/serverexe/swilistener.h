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
//


#ifndef SWILISTENER_H
#define SWILISTENER_H

#include <e32base.h>
#include <e32property.h>

class CFeatMgrFeatureRegistry;


class CSWIListener : public CActive 
    {
    public:

		static CSWIListener* NewL(CFeatMgrFeatureRegistry*);
		~CSWIListener();

	private:

		void RunL();
		void DoCancel();
        CSWIListener(CFeatMgrFeatureRegistry*);
		void ConstructL();

	private:

		CFeatMgrFeatureRegistry* iRegistry;
		RProperty iProperty;
    };

    
class MSWICallBack 
	{
	public:
		virtual void SWITimedOut() = 0;
	};

class CSWITimer : public CTimer
	{
	public:
	
		static CSWITimer* NewL(const TTimeIntervalMicroSeconds32, MSWICallBack*);
		~CSWITimer();
		
	private:
	
		void RunL();
		void DoCancel();
		CSWITimer(MSWICallBack*);
		void ConstructL(const TTimeIntervalMicroSeconds32);
		
	private:
		
		MSWICallBack* iCallBack;
	};

#endif  // SWILISTENER_H

// End of File
