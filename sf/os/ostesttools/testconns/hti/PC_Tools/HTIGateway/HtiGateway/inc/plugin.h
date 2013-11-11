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
*   This file contains the headers of Plugin, FrontendPlugin,
*	CommChannelPlugin and CommChannelPluginObserver classes and implementation
*	of DummyCommObserver.
*/

#ifndef PLUGIN_H
#define PLUGIN_H

#pragma warning ( disable : 4786 )

#include <string>
#include "common.h"

using namespace std;

//**********************************************************************************
// Class Plugin
//
//**********************************************************************************

class Plugin
{
public:
	enum PluginType { null, dummy, frontend, comm_channel };

public:
	virtual PluginType GetType() { return null; }
	virtual DWORD Init();
	virtual const string& GetName(string *name = NULL);

protected:
	Plugin();

protected:
	string m_Name;
};

//**********************************************************************************
// Class FrontendPlugin
//
//**********************************************************************************

class FrontendPlugin : public Plugin
{
public:
	virtual PluginType GetType() { return frontend; }
	virtual DWORD Init();
};

//**********************************************************************************
// Class CommChannelPluginObserver
//
//**********************************************************************************

class CommChannelPluginObserver
{
public:
	virtual void NotifyDataAvailable() = 0;
	virtual void NotifyCommChannelOpened() = 0;
	virtual void NotifyCommChannelClosed() = 0;
};

//**********************************************************************************
// Class DummyCommObserver
//
//**********************************************************************************

class DummyCommObserver : public CommChannelPluginObserver
{
public:
	void NotifyDataAvailable() { };
	void NotifyCommChannelOpened() { };
	void NotifyCommChannelClosed() { };
};

//**********************************************************************************
// Class CommChannelPlugin
//
// This class is the parent class of all communication channel plugins used
// in DataGateway.
//**********************************************************************************

class CommChannelPlugin : public Plugin
{
public:
	PluginType GetType() { return comm_channel; }
	virtual DWORD Init();
	virtual DWORD Open() = 0;
	virtual bool IsOpen() { return m_Open; }
	/*
	 * This method closes and deletes the instance of plugin
	 */	
	virtual DWORD Close() = 0;
	/*
	 * This method is used to send Data object and Receive Data object
	 */
	virtual DWORD SendReceive(Data* data_in, Data** data_out, long timeout = 5000) = 0;
	/*
	 * This method is used to send Data object
	 */	
	virtual DWORD Send(Data* data_in, long timeout = 5000) = 0;
	/*
	 * This method is used to receive Data object
	 */	
	virtual DWORD Receive(Data** data_out, long timeout = 5000) = 0;
	/*
	 * This method creates instance of specified plugin  
	 */
	static CommChannelPlugin* Instance(const string& pluginname,
		                               const CommChannelPluginObserver* observer = NULL);
	/*
	 * This method initializes and connects the instance of plugin
	 */	
	static DWORD Connect();
	/*
	 * This method closes and deletes the instance of plugin
	 */	
	static DWORD Disconnect();

protected:
	CommChannelPlugin(const CommChannelPluginObserver* observer = NULL);
	const CommChannelPluginObserver* GetObserver() { return m_Observer; }

protected:
	const CommChannelPluginObserver* m_Observer;
	bool m_Open;

private:
	static CommChannelPlugin* m_Self;
};

#endif

// End of the file