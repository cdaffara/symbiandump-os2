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
* Description:  Declaration of the class DAnalyzeToolDevice.
*
*/


#ifndef __ANALYZETOOLEVICE_H__
#define __ANALYZETOOLEVICE_H__

// INCLUDES
#include <analyzetool/analyzetool.h>
#include <kernel/kernel.h>

// CLASS DECLARATION

/**
* The implementation of the abstract base class for an LDD factory object.
*/

class DAnalyzeToolDevice : public DLogicalDevice
	{
	public:
	
        /**
        * C++ default constructor.
        */
		DAnalyzeToolDevice();
		
	public: // from DLogicalDevice
	
        /**
        * Second stage constructor.
        @return KErrNone or standard error code.
        */
		virtual TInt Install();
	
        /**
        * Gets the driver's capabilities.
        @param aDes A user-side descriptor into which capabilities 
        			information is to be written.
        */
		virtual void GetCaps( TDes8& aDes ) const;
		
        /**
        * Called by the kernel's device driver framework to 
        * create a Logical Channel.
        @param aChannel Set to point to the created Logical Channel
        @return KErrNone or standard error code.
        */
		virtual TInt Create( DLogicalChannelBase*& aChannel );
	};


#endif // __ANALYZETOOLEVICE_H__

// End of File
