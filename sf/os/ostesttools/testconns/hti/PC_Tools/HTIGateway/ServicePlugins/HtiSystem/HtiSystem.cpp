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
#include "HtiSystemH.h"
#include "hticommon.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"

// Command codes
const int CMD_HTI_AUTH = 0x01;
const int CMD_HTI_VERSION =      0x02;
const int CMD_HTI_SERVICE_LIST = 0x03;
const int CMD_HTI_STOP =         0x04;
const int CMD_HTI_REBOOT =       0x05;
const int CMD_HTI_FORMAT =       0x06;
const int CMD_HTI_RESET =        0x07;
const int CMD_HTI_SHOW_CONSOLE = 0x08;
const int CMD_HTI_HIDE_CONSOLE = 0x09;
const int CMD_HTI_INSTANCE_ID =  0x0A;
const int CMD_HTI_DEBUG_PRINT =  0x0B;
const int CMD_HTI_ERROR =        0xFF;

const int SERVICE_NAME_LEN = 124;
const int SERVICE_UID_LEN = 4;
const int SERVICE_DESCR_LEN = SERVICE_NAME_LEN + SERVICE_UID_LEN;

//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// ns1__authentication()
//**********************************************************************************
int ns1__authentication(struct soap *soap,
						char* securityToken,
						char** result)
{
	// Construct & send & receive HTI message
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_AUTH );
	msg.AddString( securityToken );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_30_SECONDS ) )
		return SOAP_FAULT;

	// Check response
	if ( msg.CheckCommandCode( CMD_HTI_AUTH ) )
		return SOAP_FAULT;

	*result = msg.GetSoapString( 1, msg.GetMsgLen() - 1 );

	return SOAP_OK;
}
//**********************************************************************************
// ns1__getVersion()
//**********************************************************************************
int ns1__getVersion(struct soap* soap,
					void *_,
					char **result)
{
	// Construct & send & receive HTI message
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_VERSION );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_30_SECONDS ) )
		return SOAP_FAULT;

	// Fill version string
	*result = (char*)soap_malloc(soap, 8 );
	sprintf(*result, "%u.%u", msg.GetByte(0), msg.GetByte(1));

	return SOAP_OK;	
}
//**********************************************************************************
// ns1__stop()
//**********************************************************************************
int ns1__stop(struct soap* soap,
			  void *_,
              struct ns1__stopResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_STOP );
	msg.SendMsg();
	return SOAP_OK;
}

//**********************************************************************************
// ns1__reset()
//**********************************************************************************
int ns1__reset(struct soap* soap,
			   void *_,
               struct ns1__resetResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_RESET );
	msg.SendMsg();
	return SOAP_OK;
}
//**********************************************************************************
// ns1__reboot()
//**********************************************************************************
int ns1__reboot(struct soap* soap,
				void *_,
                struct ns1__rebootResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_REBOOT );
	msg.SendMsg();
	return SOAP_OK;
}
//**********************************************************************************
// ns1__listServices()
//**********************************************************************************
int ns1__listServices(struct soap* soap,
					  void *_,
                      struct ArrayOfHtiService &array)
{
	// Construct & send & receive HTI message
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_SERVICE_LIST );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_30_SECONDS ) )
		return SOAP_FAULT;

	// get services
	array.__size = msg.GetMsgLen()/SERVICE_DESCR_LEN;
	array.__ptrHtiService = 
		(ns1__HtiService*)soap_malloc( soap, sizeof(ns1__HtiService)*array.__size );
	for( int i=0; i<array.__size; ++i)
	{
		array.__ptrHtiService[i].serviceUid = msg.GetDWord( i*SERVICE_DESCR_LEN );
		array.__ptrHtiService[i].serviceName = 
			msg.GetSoapString( i*SERVICE_DESCR_LEN + SERVICE_UID_LEN, SERVICE_NAME_LEN );
	}	

	return SOAP_OK;
}
//**********************************************************************************
// ns1__restoreFactorySettings()
//**********************************************************************************
int ns1__restoreFactorySettings(struct soap* soap,
								enum ns1__restoreMode mode,
                                struct ns1__restoreFactorySettingsResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_FORMAT );
	msg.AddByte( mode );
	msg.SendMsg();
	return SOAP_OK;
}
//**********************************************************************************
// ns1__restoreFactorySettings()
//**********************************************************************************
int ns1__instanceID(struct soap* soap,
					void *_,
                    unsigned int &instanceID)
{
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_INSTANCE_ID );
	if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;
    instanceID = (unsigned int) msg.GetInt( 0 );
	return SOAP_OK;
}
//**********************************************************************************
// ns1__showConsole()
//**********************************************************************************
int ns1__showConsole(struct soap* soap,
			         void *_,
                     struct ns1__showConsoleResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_SHOW_CONSOLE );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}
//**********************************************************************************
// ns1__hideConsole()
//**********************************************************************************
int ns1__hideConsole(struct soap* soap,
			         void *_,
                     struct ns1__hideConsoleResponse *out)
{
	HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_HIDE_CONSOLE );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}
//**********************************************************************************
// ns1__debugPrint()
//**********************************************************************************
int ns1__debugPrint(struct soap* soap,
			        char* debugMessage,
                    struct ns1__debugPrintResponse *out)
{
    HtiMsgHelper msg( soap, HTI_SYSTEM_UID, CMD_HTI_DEBUG_PRINT );
	msg.AddString( debugMessage );
    return msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS );
}