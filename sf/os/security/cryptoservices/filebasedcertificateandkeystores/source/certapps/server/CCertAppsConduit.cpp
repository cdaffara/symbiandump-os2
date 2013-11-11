/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "CCertAppsConduit.h"
#include "CFSCertAppsServer.h"
#include <certificateapps.h>
#include "fstokencliserv.h"
#include "fstokenutil.h"
#include "fsmarshaller.h"

_LIT_SECURITY_POLICY_C1(KAddRemovePolicy, ECapabilityWriteDeviceData);

CCertAppsConduit* CCertAppsConduit::NewL(CFSCertAppsServer& aServer)
	{
	CCertAppsConduit* self = new (ELeave) CCertAppsConduit(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCertAppsConduit::CCertAppsConduit(CFSCertAppsServer& aServer) :
	iServer(aServer)
	{
	}

void CCertAppsConduit::ConstructL()
	{
	}

CCertAppsConduit::~CCertAppsConduit()
	{
	}

void CCertAppsConduit::ServiceCertAppsRequestL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case EAddApp:
			AddL(aMessage);
			break;

		case ERemoveApp:
			RemoveL(aMessage);
			break;

		case EGetAppCount:
			ApplicationCountL(aMessage);
			break;

		case EGetApps:
			ApplicationsL(aMessage);
			break;

		case EGetApplication:
			ApplicationL(aMessage);
			break;

		default:
			// Client made an illegal request
			PanicClient(aMessage, EPanicInvalidRequest);
		}
	
	aMessage.Complete(KErrNone);
	}

void CCertAppsConduit::AddL(const RMessage2& aMessage) const
	{
	// Add message is composed of the following structure
	// Parameter 1 - TPckg<TCertificateAppInfo>

	// Check the calling process has the correct capabilities
	if (!KAddRemovePolicy.CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

	TCertificateAppInfo appInfo;
	TPckg<TCertificateAppInfo> pckg(appInfo);
	aMessage.ReadL(1, pckg);

	// Now that we have extracted the appInfo, we can call the
	// real server
	iServer.AddL(appInfo);
	}

void CCertAppsConduit::RemoveL(const RMessage2& aMessage) const
	{
	// Remove message is composed of a single TPckg<TUid>

	// Check the calling process has the correct capabilities
	if (!KAddRemovePolicy.CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

	TUid uid;
	TPckg<TUid> pckg(uid);
	aMessage.ReadL(1, pckg);
	iServer.RemoveL(uid);
	}

void CCertAppsConduit::ApplicationCountL(const RMessage2& aMessage) const
	{
	// This message contains a single output descriptor of type
	// TPckg<TInt>
	TInt appCount = iServer.ApplicationCountL();
	aMessage.WriteL(1, TPckg<TInt>(appCount));
	}

void CCertAppsConduit::ApplicationsL(const RMessage2& aMessage) const
	{
	// This message contains the following parameters:
	// Param1: [IN] TInt - maximum buffer length allowed
	// Param2: [OUT] TDes8 - The buffer to write into; if buffer size too
	//               small then will return KErrOverflow with param 2 being 
	//               required size

	// Firstly, the maximum allowable length of the buffer
	
	// now get the array to be transmitted
	RArray<TCertificateAppInfo> arr;
	CleanupClosePushL(arr);

	// retrieve the array and marshall them into the message
	iServer.ApplicationsL(arr);

	TInt reqdSize = TokenDataMarshaller::Size(arr);
	TInt bufLen = User::LeaveIfError(aMessage.GetDesLength(2));

	if (reqdSize <= bufLen)
		{
		HBufC8* buf = HBufC8::NewMaxLC(reqdSize);
		TPtr8 ptr(buf->Des());
		TokenDataMarshaller::Write(arr, ptr);
		aMessage.WriteL(2, ptr);
		CleanupStack::PopAndDestroy(buf);
		}
	else
		{
		aMessage.WriteL(2, TPckg<TInt>(reqdSize));
		User::Leave(KErrOverflow);
		}

	CleanupStack::PopAndDestroy(&arr);
	}

void CCertAppsConduit::ApplicationL(const RMessage2& aMessage) const
	{
	// The parameters for the ApplicationL function are as follows:
	// Param1: [IN] TUid - The Uid of the app to retrieve
	// Param2: [OUT] TCertificateAppInfo - The app info returned

	// Read the UID first
	TUid uid;
	TPckg<TUid> pckgUid(uid);
	aMessage.ReadL(1, pckgUid);

	// Now call the server
	TCertificateAppInfo appInfo;
	iServer.ApplicationL(uid, appInfo);

	// Now wrap the returned parameters into packages
	aMessage.WriteL(2, TPckg<TCertificateAppInfo>(appInfo));
	}
