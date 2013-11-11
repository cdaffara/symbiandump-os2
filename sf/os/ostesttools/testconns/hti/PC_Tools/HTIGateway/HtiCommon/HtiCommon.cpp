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
#define _EXPORT_HTI_COMMON_

#include "..\libs\stdsoap2.h"
#include "hticommon.h"
#include "HtiSoapHandlerInterface.h"

//**********************************************************************************
// General util functions
//
//**********************************************************************************

//**********************************************************************************
// check_mandatory_string_parameter
//**********************************************************************************
EXPORT_FROM_DLL int check_mandatory_string_parameter(struct soap* soap,
													 char* parameter,
													 char* parameterName)
{
	int result = SOAP_OK;
	
	if(!parameter)
	{
		char msg[] = "Mandatory parameter \"%s\" missing";
		int msgLen = _scprintf(msg, parameterName)+1; //+1 for nul char
		char* buf = new char[msgLen];
		sprintf(buf, msg, parameterName);
		soap->error = soap_receiver_fault(soap, "HtiGateway", buf);
		delete buf;
		result = SOAP_FAULT;
	}
	else if(strlen(parameter) == 0)
	{
		char msg[] = "Mandatory parameter \"%s\" empty";
		int msgLen = _scprintf(msg, parameterName)+1; //+1 for nul char
		char* buf = new char[msgLen];
		sprintf(buf, msg, parameterName);
		soap->error = soap_receiver_fault(soap, "HtiGateway", buf);
		delete buf;
		result = SOAP_FAULT;		
	}

	return result;
}

//**********************************************************************************
// check_mandatory_wcstring_parameter
//**********************************************************************************
EXPORT_FROM_DLL int check_mandatory_wcstring_parameter(struct soap* soap,
													   wchar_t* parameter,
													   char* parameterName)
{
	int result = SOAP_OK;
	
	if ( !parameter )
	{
		char msg[] = "Mandatory parameter \"%s\" missing";
		int msgLen = _scprintf(msg, parameterName)+1; //+1 for nul char
		char* buf = new char[msgLen];
		sprintf(buf, msg, parameterName);
		soap->error = soap_receiver_fault(soap, "HtiGateway", buf);
		delete buf;
		result = SOAP_FAULT;
	}
	else if ( wcslen(parameter) == 0 )
	{
		char msg[] = "Mandatory parameter \"%s\" empty";
		int msgLen = _scprintf(msg, parameterName)+1; //+1 for nul char
		char* buf = new char[msgLen];
		sprintf(buf, msg, parameterName);
		soap->error = soap_receiver_fault(soap, "HtiGateway", buf);
		delete buf;
		result = SOAP_FAULT;		
	}

	return result;
}

//**********************************************************************************
// dump
//**********************************************************************************
EXPORT_FROM_DLL void dump(BYTE *data, int dataLen)
{
	printf("\nDUMP len %d:\n", dataLen);

	int hex_index = 0;
	int char_index = 0;

	char hex_buf[48+1];
	char char_buf[16+1];
	hex_buf[0]  = 0x0;
	char_buf[0] = 0x0;

	int j = 0;
	for(int i=0; i<dataLen; i++)
	{
		char tmp[3];
		sprintf(tmp, "%02x", data[i]);
		strcat(hex_buf, tmp);

		if( (data[i] != 0x0a) && (data[i] != 0x0d) && (data[i] != 0x08))
			char_buf[j] = data[i];
		else
			char_buf[j] = '.';

		if( i == dataLen-1 )
		{
			char_buf[j+1] = 0x0;
			printf("%-45s\t\t%s\n", hex_buf, char_buf);
			break;
		}

		if( !(j%15) && (j!=0) )
		{
			char_buf[j+1] = 0x0;
			printf("%-45s\t\t%s\n", hex_buf, char_buf);
			j = 0;
			hex_buf[0]  = 0x0;
			continue;
		}
		else
			strcat(hex_buf, " ");

		j++;
	}
}

//**********************************************************************************
// soap_receiver_fault_format
//**********************************************************************************
EXPORT_FROM_DLL int soap_receiver_fault_format(struct soap *soap,
											   const char *faultstring,
											   const char *faultdetail, ...)
{
	va_list marker;
	va_start( marker, faultdetail );

	char* tmp = new char[strlen(faultdetail)+1];
	memcpy(tmp, faultdetail, strlen(faultdetail)+1);
	
	char* token = strtok(tmp, "%");
	if(token == NULL)
	{
		// no parameters
		delete tmp;
		return soap_receiver_fault(soap, faultstring, faultdetail);
	}

	char* text = (char*) malloc( strlen(token) + 1);
	strcpy(text, token);
    
	token = strtok(NULL, "%");
	while( token != NULL )
	{
		char* parse = NULL;

		if(token[0] == 'd')
		{
			int var = va_arg( marker, int );
			parse = new char[ _scprintf("%d", var) + 1 ];
            sprintf( parse, "%d", var );
		}
		else if(token[0] == 'x')
		{
			int var = va_arg( marker, int );
			parse = new char[ _scprintf("%x", var) + 1 ];
            sprintf( parse, "%x", var );	
		}
		else if(token[0] == 's' )
		{
			char* var = va_arg( marker, char* );
			parse = new char[ _scprintf("%s", var) + 1 ];
            sprintf( parse, "%s", var );
		}

		// add parsed part
		if(parse)
		{
			text = (char*) realloc( text, strlen(text) + strlen(parse) + 1 );
			strcat(text, parse);
			delete parse;
		}
        
		// add remaining part
		text = (char*) realloc( text, strlen(text) + strlen(token+1) + 1 );
		strcat(text, token+1);

		token = strtok(NULL, "%");
	}
	va_end( marker );
	delete tmp;

	int res = soap_receiver_fault(soap, faultstring, text);
	free (text);
	return res;
}

//**********************************************************************************
// HtiMsgHelper class implementation
//
//**********************************************************************************
//**********************************************************************************
// HtiMsgHelper::HtiMsgHelper
//**********************************************************************************
EXPORT_FROM_DLL HtiMsgHelper::HtiMsgHelper(struct soap *soap)
{
	m_msgBody = NULL;
	m_msgBodyLen = 0;
	m_soap = soap;
	m_serviceId = 0;
}

//**********************************************************************************
// HtiMsgHelper::HtiMsgHelper
//**********************************************************************************
EXPORT_FROM_DLL HtiMsgHelper::HtiMsgHelper(struct soap *soap,
										   DWORD serviceId)
{
	m_msgBody = NULL;
	m_msgBodyLen = 0;
	m_soap = soap;
	m_serviceId = serviceId;
}

//**********************************************************************************
// HtiMsgHelper::HtiMsgHelper
//**********************************************************************************
EXPORT_FROM_DLL HtiMsgHelper::HtiMsgHelper(struct soap *soap,
										   DWORD serviceId,
										   BYTE command)
{
	m_msgBody = new BYTE[1];
	m_msgBody[0] = command;
	m_msgBodyLen = 1;
	m_soap = soap;
	m_serviceId = serviceId;
}

//**********************************************************************************
// HtiMsgHelper::~HtiMsgHelper
//**********************************************************************************
EXPORT_FROM_DLL HtiMsgHelper::~HtiMsgHelper()
{
	// m_msgBody is not deleted as we dont have ownership if
	// it is a received message
}

//**********************************************************************************
// HtiMsgHelper::IncBodySize
//**********************************************************************************
void HtiMsgHelper::IncBodySize(int size)
{
	BYTE *newMsgBody = new BYTE[m_msgBodyLen+size];
	memcpy(newMsgBody, m_msgBody, m_msgBodyLen);
	delete m_msgBody;
	m_msgBody = newMsgBody;
	m_msgBodyLen += size;
}

//**********************************************************************************
// HtiMsgHelper::CheckMsgExactLen
//**********************************************************************************
EXPORT_FROM_DLL int HtiMsgHelper::CheckMsgExactLen( int len )
{
	if( GetMsgLen() != len )
	{
		m_soap->error = soap_receiver_fault_format( m_soap, "HtiError", 
			"Incorrect HTI msg length. Expecting %d received %d", len, GetMsgLen());
        return SOAP_FAULT;
	}
	return SOAP_OK;
}

//**********************************************************************************
// HtiMsgHelper::CheckMsgMinLen
//**********************************************************************************
EXPORT_FROM_DLL int HtiMsgHelper::CheckMsgMinLen( int len )
{
	if( GetMsgLen() < len )
	{
		m_soap->error = soap_receiver_fault(
			m_soap, "HtiError", "HTI msg too small");
        return SOAP_FAULT;
	}
	return SOAP_OK;
}

//**********************************************************************************
// HtiAudioMsg::CheckCommandCode
//**********************************************************************************
EXPORT_FROM_DLL int HtiMsgHelper::CheckCommandCode(BYTE code)
{
	if(GetByte(0) != code)
	{
		m_soap->error = soap_receiver_fault_format(m_soap, "HtiError",
			"incorrect response CommandCode (0x%x)", GetByte(0));
		return SOAP_FAULT;
	}
	return SOAP_OK;
}

//**********************************************************************************
// HtiMsgHelper::AddByte
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddByte( BYTE byte )
{
	int writePos = m_msgBodyLen;
	IncBodySize( 1 );
	m_msgBody[writePos] = byte;
}

//**********************************************************************************
// HtiMsgHelper::AddWord
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddWord( WORD data )
{
	int writePos = m_msgBodyLen;
	IncBodySize( 2 );
	*(WORD*)(m_msgBody+writePos) = data;
}

//**********************************************************************************
// HtiMsgHelper::AddDWord
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddDWord( DWORD data )
{
	int writePos = m_msgBodyLen;
	IncBodySize( 4 );
	*(DWORD*)(m_msgBody+writePos) = data;
}

//**********************************************************************************
// HtiMsgHelper::AddInt
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddInt( int data )
{
	int writePos = m_msgBodyLen;
	IncBodySize( 4 );
	*(int*)(m_msgBody+writePos) = data;
}

//**********************************************************************************
// HtiMsgHelper::AddUInt64
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddUInt64( unsigned __int64 data )
{
	int writePos = m_msgBodyLen;
	IncBodySize( 8 );
	*(unsigned __int64*)(m_msgBody+writePos) = data;
}

//**********************************************************************************
// HtiMsgHelper::AddString
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddString( char* string )
{
	int stringLen = string ? (int) strlen( string ) : 0;
	if( stringLen == 0 )
		return;

	int writePos = m_msgBodyLen;
	IncBodySize( stringLen );
	memcpy(m_msgBody+writePos, string, stringLen);
}

//**********************************************************************************
// HtiMsgHelper::AddStringWithLengthByte
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddStringWithLengthByte( char* string )
{
	int stringLen = string ? (int) strlen( string ) : 0;
	if( stringLen == 0 )
		return;

	AddByte( stringLen );
	AddString( string );
}

//**********************************************************************************
// HtiMsgHelper::AddStringWithLengthByteZero
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddStringWithLengthByteZero( char* string )
{
	int stringLen = string ? (int) strlen( string ) : 0;

	AddByte( stringLen );

	if( stringLen == 0 )
		return;

	AddString( string );
}

//**********************************************************************************
// HtiMsgHelper::AddWCString
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddWCString(wchar_t* string)
{
	int stringLen = string ? (int) wcslen( string )*2 : 0;
	if( stringLen == 0 )
		return;

	int writePos = m_msgBodyLen;
	IncBodySize( stringLen );
	memcpy( m_msgBody+writePos, string, stringLen );
}

//**********************************************************************************
// HtiMsgHelper::AddWCStringWithLengthByte
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddWCStringWithLengthByte( wchar_t* string )
{
	int stringLen = string ? (int) wcslen( string ) : 0;
	if( stringLen == 0 )
		return;

	AddByte( stringLen );
	AddWCString( string );
}
//**********************************************************************************
// HtiMsgHelper::AddData
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddData( void* data, int dataLen )
{
	int writePos = m_msgBodyLen;
	IncBodySize( dataLen );
	memcpy( m_msgBody+writePos, data, dataLen );
}

//**********************************************************************************
// HtiMsgHelper::SendMsg
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::SendMsg()
{
	// Send the message to symbian side
	HtiSoapHandlerInterface* handler = 
		static_cast<HtiSoapHandlerInterface*>(m_soap->user);
	handler->SendHtiMessage(m_serviceId, m_msgBody, m_msgBodyLen);

	// destroy the sent message
	delete m_msgBody;
	m_msgBody = NULL;
	m_msgBodyLen = 0;
}

//**********************************************************************************
// HtiMsgHelper::ReceiveMsg
//**********************************************************************************
EXPORT_FROM_DLL int HtiMsgHelper::ReceiveMsg( int timeout )
{
	// Clean these up for received HTI message
	if( m_msgBody )
	{
		delete m_msgBody;
		m_msgBody = NULL;
		m_msgBodyLen = 0;
	}
	
	HtiSoapHandlerInterface* handler = 
		static_cast<HtiSoapHandlerInterface*>(m_soap->user);

	// Wait for OK or error msg
	if (handler->WaitForHtiMessage(timeout))
	{
		if ( !handler->IsReceivedHtiError() )
		{
			// Get received message
			// (handler has ownership of the message body)
			m_msgBody = (BYTE*) handler->ReceivedHtiMessageBody();
			m_msgBodyLen = handler->ReceivedHtiMessageBodySize();
			return SOAP_OK;
		}
		else
		{
			// Fill the error description
			handler->SendSoapFaultFromReceivedHtiError();
			return SOAP_FAULT;
		}
	}
	// ...or timeout
	else
	{
		m_soap->error = soap_receiver_fault(
			m_soap, "HtiGateway", "No response from symbian side");
		return SOAP_FAULT;
	}
}

//**********************************************************************************
// HtiMsgHelper::SendReceiveMsg
//**********************************************************************************
EXPORT_FROM_DLL int HtiMsgHelper::SendReceiveMsg( int timeout )
{
	SendMsg();
	return ReceiveMsg( timeout );
}

//**********************************************************************************
// HtiMsgHelper::GetSoapString
//**********************************************************************************
EXPORT_FROM_DLL char* HtiMsgHelper::GetSoapString( int offset, int len )
{
	char* string = (char*) soap_malloc( m_soap, len+1 );//+1 for the last null char
	if ( len > 0 )
        memcpy( string, m_msgBody + offset, len );
	string[len] = 0x00; //add null char
	return string;
}

//**********************************************************************************
// HtiMsgHelper::GetSoapWCString
//**********************************************************************************
EXPORT_FROM_DLL wchar_t* HtiMsgHelper::GetSoapWCString( int offset, int len )
{
	wchar_t* string = (wchar_t*) soap_malloc( m_soap, len*2+2 );//+2 for the last null char
	if ( len > 0 )
        memcpy( string, m_msgBody + offset, len*2 );
	string[len] = 0x0000; //add null char
	return string;
}

//**********************************************************************************
// HtiMsgHelper::AddStringWithLengthWordZero
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddStringWithLengthWordZero( char* string )
{
	int stringLen = string ? (int) strlen( string ) : 0;

	AddWord( stringLen );

	if( stringLen == 0 )
		return;

	AddString( string );
}

//**********************************************************************************
// HtiMsgHelper::AddWCStringWithLengthByteZero
//**********************************************************************************
EXPORT_FROM_DLL void HtiMsgHelper::AddWCStringWithLengthByteZero( wchar_t* string )
{
	int stringLen = string ? (int) wcslen( string ) : 0;

	AddByte( stringLen );

	if( stringLen == 0 )
		return;

	AddWCString( string );
}