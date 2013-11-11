/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @publishedPartner
 @released
*/

#include <usb/hostms/policyplugin.hrh>

inline CMsmmPolicyPluginBase::~CMsmmPolicyPluginBase()
	{
	REComSession::DestroyedImplementation (iDtor_ID_Key);
	}

inline CMsmmPolicyPluginBase* CMsmmPolicyPluginBase::NewL()
	{
    RImplInfoPtrArray   pluginImpArray;
    const TUid policyInterfaceUid = {KUidMountPolicyInterface};
    REComSession::ListImplementationsL(policyInterfaceUid, pluginImpArray);
    if (pluginImpArray.Count())
        {
        const TUid firstImplementationUid(
                pluginImpArray[0]->ImplementationUid());
        pluginImpArray.ResetAndDestroy();
        
        TAny* interface = REComSession::CreateImplementationL (
                firstImplementationUid, 
                _FOFF (CMsmmPolicyPluginBase, iDtor_ID_Key));

        return reinterpret_cast <CMsmmPolicyPluginBase*> (interface);
        }
    return NULL;
	}

inline  CMsmmPolicyPluginBase::CMsmmPolicyPluginBase():
CActive(EPriorityStandard)
    {
    }

// End of file
