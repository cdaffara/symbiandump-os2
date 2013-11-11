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
// ULogger plug-in allocator header file
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef PLUGINALLOCATOR_H_
#define PLUGINALLOCATOR_H_

#include <e32base.h>
#include <e32cmn.h>
#include "uloggeroutputplugin.h"
#include "uloggerinputplugin.h"


namespace Ulogger
{

/*!CPluginAllocator class.
*/
class CPluginAllocator : public CBase
	{
public:
	IMPORT_C static CPluginAllocator* NewL(const TPtrC8& aOutputPluginName, const TPtrC8& aInputPluginName);
	IMPORT_C static CPluginAllocator* NewLC(const TPtrC8& aOutputPluginName, const TPtrC8& aInputPluginName);
	IMPORT_C ~CPluginAllocator();
	IMPORT_C void ConstructL(const TPtrC8& aOutputPluginName, const TPtrC8& aInputPluginName);
	IMPORT_C MOutputPlugin* GetOutputPlugin();
	IMPORT_C MInputPlugin*  GetInputPlugin();

	/**
	Requests a list of all available implementations which 
	satisfy this given interface in a ECOM framework
	*/
	IMPORT_C static void ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray);

private:
	CPluginAllocator();
	TInt CreateChannelsL(const TPtrC8& aOutputPluginName, const TPtrC8& aInputPluginName);

private:
	CPlugin     *iOutputPluginBase; //output plugin instance
	RLibrary 	iOutputLib;
	TFileName 	iOutputLibName;

	CPlugin     *iInputPluginBase; //input plugin instance
	RLibrary 	iInputLib;
	TFileName 	iInputLibName;
	}; //</CPluginAllocator>

} //</namespace ULogger>
#endif /*PLUGINALLOCATOR_H_*/
