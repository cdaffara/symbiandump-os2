/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __OBEXCLASSCONTROLLER_H__
#define __OBEXCLASSCONTROLLER_H__

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#include "../../ClassControllerClientSession/inc/classControllerClientSession.h"

class MUsbClassControllerNotify;

const TInt KObexStartupPriority = 2;

//Constants used in GetDescriptorInfo function
const TInt KNumObexInterfaces = 2;
const TInt KObexDescriptorLength = 18;

NONSHARABLE_CLASS(CUsbObexClassController) : public CUsbClassControllerPlugIn
	{

	public:
		static CUsbObexClassController* NewL(MUsbClassControllerNotify& aOwner);

		virtual ~CUsbObexClassController();
	 	// Functions derived from CActive.
		virtual void RunL();
		virtual void DoCancel();
		virtual TInt RunError(TInt aError);

		// Functions derived from CUsbClassControllerBase
		virtual void Start(TRequestStatus& aStatus);
		virtual void Stop(TRequestStatus& aStatus);

		virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

	protected:
		CUsbObexClassController(MUsbClassControllerNotify& aOwner);
		void ConstructL();

	private:
		RClassControllerClient iClassContClient;

	};

#endif //__OBEXCLASSCONTROLLER_H__
