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
*/
#include "HtiechoH.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"

//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// ns1__echo()
//**********************************************************************************
int ns1__echo(struct soap* soap, char *echo, char **result)
{
	HtiMsgHelper msg( soap, HTI_UID );
	msg.AddString( echo );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_30_SECONDS ) )
		return SOAP_FAULT;
	
	*result = msg.GetSoapString( 0, msg.GetMsgLen() );

	return SOAP_OK; 
}
