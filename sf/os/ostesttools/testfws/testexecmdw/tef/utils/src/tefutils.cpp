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



/**
 @file TEFUtils.cpp
*/

#include <e32base.h>
#include <f32file.h>
#include <test/tefexportconst.h>
#include "tefutils.h"

EXPORT_C CWrapperUtilsPlugin* TEFUtils::WrapperPluginNew(RLibrary& aLibrary)
	{
	const TInt KDllNameLength = 32;
	TInt pluginErr = KErrGeneral;
	CWrapperUtilsPlugin* plugin = NULL;
	TInt index = KOsVersionCount;
	TBuf<KDllNameLength> wrapperDllName;
	_LIT(wrapperDllNameFormat, "WrapperUtilsPlugin%d.dll");
	pluginErr = aLibrary.Load(_L("WrapperUtilsPlugin.dll"));
	while (KErrNone != pluginErr && index != 0)
		{
		wrapperDllName.Format(wrapperDllNameFormat, index);
		pluginErr = aLibrary.Load(wrapperDllName); //KTEFWrapperPluginDll
		--index;
		}
	
	if (pluginErr == KErrNone)
		{
		TLibraryFunction newl;
		newl = aLibrary.Lookup(1);
		TRAP(pluginErr, plugin = (CWrapperUtilsPlugin*)newl());
		if (KErrNone != pluginErr)
			{
			aLibrary.Close();
			}
		}
	
	return plugin;
	}
