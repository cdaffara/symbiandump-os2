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
//

#include "outputframework.h"
#include "uloggeroutputplugin.h" // MOutputPlugin, ...


namespace Ulogger
{
/** Static Factory Construction 
@param aOutputPlugin the output plugin reference. This plugin will be used to send data.
@return a pointer to the created object
@leave KErrNoMemory if not enough memory available
*/
EXPORT_C COutputFramework* COutputFramework::NewL(MOutputPlugin& aOutputPlugin, const RPointerArray<TPluginConfiguration>& aOutputSettings)
	{
	COutputFramework* me = new (ELeave) COutputFramework(aOutputPlugin);
	CleanupStack::PushL(me);
	me->ConstructL(aOutputSettings);
	CleanupStack::Pop();
	return me;
	}

/**
Public Destructor 
*/
EXPORT_C COutputFramework::~COutputFramework()
	{
	iOutputPlugin.CloseOutputPlugin();
	iOutputSettings.ResetAndDestroy();
	iOutputSettings.Close();
	}

/*!This function send data trough active output channel.
@param aData reference to data
@return KErrNone if successfull otherwise Symbian OS error code
*/
EXPORT_C TInt COutputFramework::SendData(const TDesC8& aData)
	{
	return iOutputPlugin.Write(aData);
	}

//Default constructor
COutputFramework::COutputFramework(MOutputPlugin& aOutputPlugin) 
: iOutputPlugin(aOutputPlugin)
	{
	}
	
//Second stage constructL function
//This will create a local mutex and initialize all the channels
void COutputFramework::ConstructL(const RPointerArray<TPluginConfiguration>& aOutputSettings)
	{	
	//copy settings
	for(TInt i=0; i<aOutputSettings.Count(); ++i)
		{
		TPluginConfiguration* pluginConfig = new TPluginConfiguration();
		pluginConfig->SetKey(aOutputSettings[i]->Key());
		pluginConfig->SetValue(aOutputSettings[i]->Value());
		iOutputSettings.AppendL(pluginConfig);
		}
	
	User::LeaveIfError(iOutputPlugin.ConfigureOutputPlugin(iOutputSettings));
	}

/**Unconfigure active output channel and release all locked resources.
*/
EXPORT_C void COutputFramework::ReleaseOutputResources()
	{
	iOutputPlugin.CloseOutputPlugin();
	}

} // namespace
