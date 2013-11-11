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
* Implementation of Usbman extension plugin.
*
*/

/**
 @file
 @internalComponent
*/
#ifndef __DUMMY_LDD__
#include "cusbmanextensionplugin.h"
#include "musbmanextensionpluginobserver.h"
#else
#include <cusbmanextensionplugindummy.h>
#include <musbmanextensionpluginobserverdummy.h>
#endif
#include <ecom/ecom.h>

EXPORT_C CUsbmanExtensionPlugin::~CUsbmanExtensionPlugin()
	{
	REComSession::DestroyedImplementation(iInstanceId);
	}

EXPORT_C CUsbmanExtensionPlugin::CUsbmanExtensionPlugin(MUsbmanExtensionPluginObserver& aObserver)
:	iObserver(aObserver)
	{
	}

EXPORT_C CUsbmanExtensionPlugin* CUsbmanExtensionPlugin::NewL(const TUid aImplementationUid, MUsbmanExtensionPluginObserver& aObserver)
	{
	return reinterpret_cast<CUsbmanExtensionPlugin*>(
		REComSession::CreateImplementationL(
			aImplementationUid, 
			_FOFF(CUsbmanExtensionPlugin, iInstanceId),
			(TAny*)&aObserver)
		);
	}

EXPORT_C MUsbmanExtensionPluginObserver& CUsbmanExtensionPlugin::Observer()
	{
	return iObserver;
	}

