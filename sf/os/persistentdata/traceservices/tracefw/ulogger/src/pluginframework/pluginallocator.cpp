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


#include "pluginallocator.h"
#include "uloggeroutputplugin.h"
#include <e32cmn.h>
#include <ecom/ecom.h>

namespace Ulogger
{

/*!Default constructor.
*/
CPluginAllocator::CPluginAllocator()
	{
	iOutputPluginBase = NULL;
	iInputPluginBase = NULL;
	}


/*!Destructor.
*/
EXPORT_C CPluginAllocator::~CPluginAllocator()
	{	
	if(iOutputPluginBase)
		{		
		delete iOutputPluginBase;
		iOutputPluginBase = NULL;
		}
	iOutputLib.Close();
		
	if((iOutputLibName.Compare(iInputLibName)!=0) && iInputPluginBase)
		{		
		delete iInputPluginBase;
		iInputPluginBase = NULL;
		}
	iInputLib.Close();

	REComSession::FinalClose();
	}


/*!Standard Symbian OS construction method which leaves object on CleanupStack.
*/
EXPORT_C CPluginAllocator* CPluginAllocator::NewLC(const TPtrC8& aOutputPluginName, const TPtrC8& aInputPluginName)
	{
	CPluginAllocator* obj = new (ELeave) CPluginAllocator();
	CleanupStack::PushL(obj);
	obj->ConstructL(aOutputPluginName, aInputPluginName);
	return obj;
	}


/*!Standard Symbian OS construction method which does not leaves object on CleanupStack.
*/
EXPORT_C CPluginAllocator* CPluginAllocator::NewL(const TPtrC8& aOutputPluginName, const TPtrC8& aInputPluginName)
	{
	CPluginAllocator* obj = CPluginAllocator::NewLC(aOutputPluginName, aInputPluginName);
	CleanupStack::Pop(); //obj
	return obj;
	}


/*!Standard Symbian OS construction method.
*/
EXPORT_C void CPluginAllocator::ConstructL(const TPtrC8& aOutputPluginName, const TPtrC8& aInputPluginName)
	{	
	//create output and control channels
	User::LeaveIfError(this->CreateChannelsL(aOutputPluginName, aInputPluginName));
	}

/*!This function create Output channel according to aOutputSettings.
*/
TInt CPluginAllocator::CreateChannelsL(const TPtrC8& aOutputPluginName, const TPtrC8& aInputPluginName)
	{
	TInt retVal = KErrNone;
	
	iOutputLibName.Copy(aOutputPluginName);
	iInputLibName.Copy(aInputPluginName);

	//create output plugin
	iOutputPluginBase = (CPlugin*)(CPlugin::NewL(aOutputPluginName));
	
	//create control plugin
	if(iOutputLibName.Compare(iInputLibName) == 0)
		{
		//share implementation
		if(aInputPluginName.Length()>0)
			iInputPluginBase = iOutputPluginBase;
		}
	else
		{
		//create new instance of control plugin
		//ulogger can work without control plugin as this is optional functionality
		if(aInputPluginName.Length()>0)
			iInputPluginBase = (CPlugin*)(CPlugin::NewL(aInputPluginName));
		}
	
	return retVal;
	}


EXPORT_C MOutputPlugin* CPluginAllocator::GetOutputPlugin()
	{
	if(iOutputPluginBase)
		return (MOutputPlugin*)iOutputPluginBase->GetInterfaceL(MOutputPlugin::iInterfaceId);
	else
		return NULL;
	}

EXPORT_C MInputPlugin* CPluginAllocator::GetInputPlugin()
	{
	if(iInputPluginBase)
		return (MInputPlugin*)iInputPluginBase->GetInterfaceL(MInputPlugin::iInterfaceId);
	else
		return NULL;
	}

EXPORT_C void CPluginAllocator::ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray)
	{
	REComSession::ListImplementationsL(KCPluginUid, aImplInfoArray);
	}

} //</namespace Ulogger>
