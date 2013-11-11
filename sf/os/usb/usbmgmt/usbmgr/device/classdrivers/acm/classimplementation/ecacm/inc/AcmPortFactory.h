/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ACMPORTFACTORY_H__
#define __ACMPORTFACTORY_H__

#include <cs_port.h>
#include "AcmPortObserver.h"
#include "RegistrationPort.h"
#include "acmcontroller.h"
#include <e32property.h>
#include <usb/acmconfig.h>

class CCdcAcmClass;
class CAcmPort;
class CAcmServer;

NONSHARABLE_CLASS(CAcmPortFactory) : public CSerial, 
						public MAcmController,
						public MAcmPortObserver
/**
 * Definition of concrete CSerial-derived type.
 */
	{
public:
	static CAcmPortFactory* NewL();
	~CAcmPortFactory();
	
private:
	CAcmPortFactory();
	void ConstructL();

private: // from CSerial
	TSecurityPolicy PortPlatSecCapability(TUint aPort) const;
	virtual CPort* NewPortL(const TUint aUnit);
	virtual void Info(TSerialInfo &aSerialInfo);

private: // from MAcmController
	TInt CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC16& aAcmControlIfcName, const TDesC16& aAcmDataIfcName);
	void DestroyFunctions(const TUint aNoAcms);

private: // from MAcmPortObserver
	void AcmPortClosed(const TUint aUnit);

private: // utility
	void CreateFunctionL(const TUint8 aProtocolNum, const TDesC16& aAcmControlIfcName, const TDesC16& aAcmDataIfcName);
	void CheckAcmArray();
	void LogPortsAndFunctions();
	void PublishAcmConfig();

private: // unowned
	RPointerArray<CAcmPort> iAcmPortArray;
	
private: // owned
	RPointerArray<CCdcAcmClass>  iAcmClassArray;
	CAcmServer* iAcmServer;
	RProperty iAcmProperty;
	TPckgBuf<TPublishedAcmConfigs> iConfigBuf;
	TBool iOwned;
	};

#endif // __ACMPORTFACTORY_H__
