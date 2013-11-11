/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef FDFSESSION_H
#define FDFSESSION_H

#include <e32base.h>

class CFdfServer;
class CFdf;
class TDeviceEvent;

NONSHARABLE_CLASS(CFdfSession) : public CSession2
	{
public:
	CFdfSession(CFdf& iFdf, CFdfServer& aServer);
	~CFdfSession();

public: // called by CFdfServer
	void DeviceEvent(const TDeviceEvent& aEvent);
	void DevmonEvent(TInt aEvent);

public:
	TBool NotifyDeviceEventOutstanding() const;
	TBool NotifyDevmonEventOutstanding() const;

private: // from CSession2
	/**
	Called when a message is received from the client.
	@param aMessage Message received from the client.
	*/
	void ServiceL(const RMessage2& aMessage);

private: // utility- IPC command handlers
	void EnableDriverLoading(const RMessage2& aMessage);
	void DisableDriverLoading(const RMessage2& aMessage);
	void NotifyDeviceEvent(const RMessage2& aMessage);
	void NotifyDeviceEventCancel(const RMessage2& aMessage);
	void NotifyDevmonEvent(const RMessage2& aMessage);
	void NotifyDevmonEventCancel(const RMessage2& aMessage);
	void GetSingleSupportedLanguageOrNumberOfSupportedLanguages(const RMessage2& aMessage);
	void GetSupportedLanguages(const RMessage2& aMessage);
	void GetManufacturerStringDescriptor(const RMessage2& aMessage);
	void GetProductStringDescriptor(const RMessage2& aMessage);
	
	void GetOtgDeviceDescriptor(const RMessage2& aMessage);
	void GetConfigurationDescriptor(const RMessage2& aMessage);

private:
	enum TStringType
		{
		EManufacturer,
		EProduct,
		};

private: // utility
	void CompleteClient(const RMessage2& aMessage, TInt aError);
	void CompleteDeviceEventNotification(const TDeviceEvent& aEvent);
	void CompleteDeviceEventNotificationL(const TDeviceEvent& aEvent);
	void CompleteDevmonEventNotification(TInt aEvent);
	void CompleteDevmonEventNotificationL(TInt aEvent);
	void GetStringDescriptor(const RMessage2& aMessage, TStringType aStringType);
	void GetStringDescriptorL(const RMessage2& aMessage, TStringType aStringType);
	void GetSingleSupportedLanguageOrNumberOfSupportedLanguagesL(const RMessage2& aMessage);
	void GetSupportedLanguagesL(const RMessage2& aMessage);

private: // unowned
	CFdf& iFdf;
	CFdfServer& iServer;

	RMessage2 iNotifyDeviceEventMsg;
	RMessage2 iNotifyDevmonEventMsg;
	};

#endif // FDFSESSION_H
