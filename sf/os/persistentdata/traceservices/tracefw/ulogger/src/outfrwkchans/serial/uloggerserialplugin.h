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
// ULogger serial plugin header file
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef ULOGGERSERIALPLUGIN_H
#define ULOGGERSERIALPLUGIN_H

#include "uloggeroutputplugin.h"
#include <d32comm.h> // RBusDevComm
#include <c32comm.h>

namespace Ulogger
{
class CSerialWriter : public CPlugin, public MOutputPlugin
	{
public:
	static CSerialWriter* NewL();
	~CSerialWriter();
public://from MOutputPlugin
	TInt ConfigureOutputPlugin(const RPointerArray<TPluginConfiguration>& aSettings);
	TInt Write(const TDesC8& aData);
	void CloseOutputPlugin();
public: //from CPlugin
	TAny* GetInterfaceL(TPluginInterface aInterfaceId);
private:
	CSerialWriter();
	void ConstructL();
	TInt Connect();
    TInt Config();
	TInt Disconnect();
private:
	RBusDevComm iComm;
    TBool 		iReady;
	TInt 		iPortNum;
	};
} // namespace

#endif // ULOGGERSERIALPLUGIN_H
