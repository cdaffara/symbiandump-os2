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
*/
// HTI service functions
//gsoap ns1 service name: HtiSystem
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char* xsd__string;
typedef long xsd__int;
typedef unsigned int xsd__unsignedInt;

struct ns1__HtiService
{
	xsd__string serviceName;
	xsd__int serviceUid;
};

struct ArrayOfHtiService
{
	struct ns1__HtiService *__ptrHtiService;
	int __size;
};

enum ns1__restoreMode { ModeNormal, ModeDeep };

//gsoap ns1 service method-action: authentication "HtiSystem"
int ns1__authentication(
	xsd__string securityToken,
	xsd__string* result
);
//gsoap ns1 service method-action: getVersion "HtiSystem"
int ns1__getVersion(
	void*_ ,
	xsd__string* result
);
//gsoap ns1 service method-action: stop "HtiSystem"
int ns1__stop(
	void*_ ,
	struct ns1__stopResponse{} *out
);
//gsoap ns1 service method-action: reset "HtiSystem"
int ns1__reset(
	void*_ ,
	struct ns1__resetResponse{} *out
);
//gsoap ns1 service method-action: reboot "HtiSystem"
int ns1__reboot(
	void*_ ,
	struct ns1__rebootResponse{} *out
);
//gsoap ns1 service method-action: listServices "HtiSystem"
int ns1__listServices(
	void*_ ,
	struct ArrayOfHtiService& array
);
//gsoap ns1 service method-action: restoreFactorySettings "HtiSystem"
int ns1__restoreFactorySettings(
	enum ns1__restoreMode mode,
	struct ns1__restoreFactorySettingsResponse{} *out
);
//gsoap ns1 service method-action: instanceID "HtiSystem"
int ns1__instanceID(
	void*_ ,
	xsd__unsignedInt &instanceID
);
//gsoap ns1 service method-action: showConsole "HtiSystem"
int ns1__showConsole(
	void*_ ,
	struct ns1__showConsoleResponse{} *out
);
//gsoap ns1 service method-action: hideConsole "HtiSystem"
int ns1__hideConsole(
	void*_ ,
	struct ns1__hideConsoleResponse{} *out
);
//gsoap ns1 service method-action: debugPrint "HtiSystem"
int ns1__debugPrint(
	xsd__string debugMessage,
	struct ns1__debugPrintResponse{} *out
);
