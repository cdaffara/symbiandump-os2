/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* characteristicsetup.cpp
*
*/


/**
 @file
 @internalComponent
*/

#include <e32base.h>
#include <cryptospi/cryptospidef.h>
#include "cryptopluginsloader.h"

using namespace CryptoSpi;

void CreatePropertiesL()
	{
	//
	//Create plugin loader
	//
	CCryptoPluginsLoader* plugins=CCryptoPluginsLoader::NewLC();
	
	//
	//Create the plugin config property
	//	
	plugins->CreatePluginConfigPropertyL();
	
	//
	//Create crypto interface property
	//
	TInt interfaceCount=sizeof(KInterfacesUids)/sizeof(KInterfacesUids[0]);
	for (TInt i=0;i<interfaceCount;i++)
		{
		plugins->CreateInterfacePropertyL(KInterfacesUids[i].iUid);	
		}
	
	//
	//Cleanup
	//
	CleanupStack::PopAndDestroy(plugins);
	}

/**
 * @return KErrNone (0) if successful, KErrMemory if out of memory
 * otherwise error result from calling RProperty::Define/Set
 */
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}
		
	TRAPD(err, CreatePropertiesL());
	
	delete trapCleanup;
	__UHEAP_MARKEND;
	return err;
	}
		



