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
*   This file contains the header file of the Plugin, FrontendPlugin,
*	CommChannelPlugin and CommChannelPluginObserver classes and implementation
*	of DummyCommObserver.
*/

// INCLUDES
#include "IPCommPlugin.h"
#include "plugin.h"
#include "util.h"
#include "serialcommplugin.h"


extern DWORD g_ErrorCode;

//**********************************************************************************
// Class Plugin
//
//**********************************************************************************

Plugin::Plugin()
{
	Init();
}

DWORD Plugin::Init()
{
	m_Name = "NULL Plugin";
	return NO_ERRORS;
}

const string& Plugin::GetName(string *name)
{
	if (name != NULL)
	{
		*name = m_Name;
	}
	return m_Name;
}

//**********************************************************************************
// Class CommChannelPlugin
//
// This class is the parent class of all communication channel plugins used
// in DataGateway.
//**********************************************************************************

CommChannelPlugin* CommChannelPlugin::m_Self = NULL;

/*
 * This method creates instance of specified plugin  
 */
CommChannelPlugin* CommChannelPlugin::Instance(const string& pluginname,
											   const CommChannelPluginObserver* observer)
{
	if (m_Self == NULL)
	{
		if (pluginname.compare("SERIAL") == 0 )
		{
			m_Self = new SerialCommPlugin(observer);
		}
		else if (pluginname.compare("IPCOMM") == 0 )
		{
			m_Self = new IPCommPlugin(observer);
		}
		else
		{
			string err = "Error when creating communication channel plugin.\n";
			err += "Unknown plugin '" + pluginname + "'";
			Util::Error(err);
			m_Self = NULL;
			g_ErrorCode = ERR_DG_UNKNOWN_COMMCHANNEL;
		}
	}
	return m_Self;
}

/*
 * This method initializes and connects the instance of plugin
 */
DWORD CommChannelPlugin::Connect()
{
	if (m_Self == NULL) return ERR_DG_UNINITIALISED_COMMCHANNEL;
	DWORD res;
	if ((res = m_Self->Init()) != NO_ERRORS) return res;
	return m_Self->Open();
}

/*
 * This method closes and deletes the instance of plugin
 */
DWORD CommChannelPlugin::Disconnect()
{
	if (m_Self == NULL) return ERR_DG_UNINITIALISED_COMMCHANNEL;
	Util::Info("[DataGateway] Waiting Communication Channel Plugin to shutdown");
	m_Self->Close();
	delete m_Self;
	m_Self = NULL;
	return NO_ERRORS;
}

CommChannelPlugin::CommChannelPlugin(const CommChannelPluginObserver* observer)
	: Plugin()
{
	if (observer == NULL)
	{
		m_Observer = new DummyCommObserver;
	}
	else
	{
		m_Observer = observer;
	}
}

DWORD CommChannelPlugin::Init()
{
	DWORD res;
	res = Plugin::Init();
	m_Open = false;
	return res;
}

// End of the file