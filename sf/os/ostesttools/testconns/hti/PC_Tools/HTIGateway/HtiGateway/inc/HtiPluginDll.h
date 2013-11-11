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
*   This file contains headers of HtiPluginDll class.
*/
#ifndef	_HTI_PLUGIN_DLL_
#define _HTI_PLUGIN_DLL_

#include "DllModule.h"

//forward
struct soap; 
struct Namespace;
class HtiSoapHandlerInterface;

class HtiPluginDll : public CDLLModule
{
public:
    DECLARE_DLL_FUNCTION(int, __cdecl,
                          serviceUID, (void))
    DECLARE_DLL_FUNCTION(char*, __cdecl,
                          soapActionName, (void))
    DECLARE_DLL_FUNCTION(Namespace*, __cdecl,
                          serviceNamespaces, (void))
    DECLARE_DLL_FUNCTION(int, __cdecl,
                          soap_serve_request, (soap *))
    DECLARE_DLL_FUNCTION(int, __cdecl,
                          hti_serve, (HtiSoapHandlerInterface*))


    BEGIN_DLL_INIT()

		INIT_DLL_FUNCTION(int, __cdecl,
			 serviceUID, (void), "serviceUID")
		INIT_DLL_FUNCTION(char*, __cdecl,
			 soapActionName, (void), "soapActionName")
		INIT_DLL_FUNCTION(Namespace*, __cdecl,
			 serviceNamespaces, (void), "serviceNamespaces")
        INIT_DLL_FUNCTION(int, __cdecl,
			 soap_serve_request, (soap *), "soap_serve_request")
        INIT_DLL_FUNCTION(int, __cdecl,
			 hti_serve, (HtiSoapHandlerInterface *), "hti_serve")

	END_DLL_INIT()
};

#endif