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
*   This file contains implementation of HtiMessage class.
*/

#include "htimessage.h"
#include "crc16.h"
#include <memory.h>
#include <crtdbg.h>
#include <stdio.h>

static const BYTE HTI_ERR_CMD = 0xFF;
static const int HTI_MIN_ERR_MSG_SIZE = 10;
static const int HTI_MAX_ERR_MSG_SIZE = 0xFF;


HtiMessage::HtiMessage(DWORD serviceId, void* body, DWORD len, BYTE priority)
{
	m_ErrDescr = NULL;
	m_Size = len + KMsgHeaderMinSize;
	m_Message = new BYTE[m_Size];
	//_RPT1(_CRT_WARN,"HtiMessage::HtiMessage %x\n", body);
	//_RPT2(_CRT_WARN,"HtiMessage::HtiMessage m_Message %x <%d>\n", m_Message, m_Size);
	memset(m_Message, 0, KMsgHeaderMinSize);
	memcpy(m_Message + KMsgHeaderMinSize, body, len);
	*((DWORD*)(m_Message + KMsgServiceNameOffset)) = serviceId;
	*((DWORD*)(m_Message + KMsgBodySizeOffset)) = len;
	m_Message[KMsgVersionOffset] = KDefaultVersion;
	m_Message[KMsgPriorityOffset] = priority;
	//CRC
	*((WORD*)(m_Message + KMsgCrcOffset)) = CRC16CCITT(m_Message, KMsgHeaderMinSize-2);
}

HtiMessage::HtiMessage(void* message, DWORD len)
{
	m_ErrDescr = NULL;
	BYTE* srcMsg = (BYTE*)message;
    int extSize = srcMsg[KMsgExtSizeOffset];
    int headerSize = MinHeaderSize() + extSize;

    m_Size = *((DWORD*)(srcMsg + KMsgBodySizeOffset));
	m_Size += headerSize;
    
    //allocate header space
    m_Message = new BYTE[m_Size];
	//_RPT1(_CRT_WARN,"HtiMessage::HtiMessage %x \n", message);
	//_RPT2(_CRT_WARN,"HtiMessage::HtiMessage m_Message %x <%d>\n", m_Message, m_Size);
    
    //copy message
	m_MessageReminderSize = m_Size;
	AddToBody( message, len);
	//memcpy( m_Message, message, min(m_MessageReminderSize, len) );
	//m_MessageReminderSize -= min(m_MessageReminderSize, len);
}

DWORD HtiMessage::AddToBody(void* data, DWORD len)
{
	//_RPT1(_CRT_WARN,"HtiMessage::AddToBody %x\n", data);
	DWORD copyLen = min(m_MessageReminderSize, len);
	memcpy( m_Message + m_Size - m_MessageReminderSize, data, copyLen);
	m_MessageReminderSize -= copyLen;
	return copyLen;

/*
	if ( m_MessageReminderSize <= len) //last part
	{
		memcpy(m_Message + m_Size - m_MessageReminderSize, data, m_MessageReminderSize);
		DWORD copyLen = m_MessageReminderSize;
		m_MessageReminderSize = 0;
		return copyLen;
	}
	else //in the middle
	{
		memcpy(m_Message + m_Size - m_MessageReminderSize, data, len);
		m_MessageReminderSize -= len;
	}

	return len;
*/
}

HtiMessage::~HtiMessage()
{
	//_RPT1(_CRT_WARN,"~HtiMessage err %x\n", m_ErrDescr);
	//_RPT1(_CRT_WARN,"~HtiMessage msg %x\n", m_Message);
	delete[] m_ErrDescr;
	delete[] m_Message;
}

/**
* Returns whole HTI message including header
*/
void* HtiMessage::HtiData()
{
	return m_Message;
}

DWORD HtiMessage::HtiDataSize()
{
	return m_Size;
}

DWORD HtiMessage::GetBodySize()
{
	return  *((DWORD*)(m_Message + KMsgBodySizeOffset));
}

DWORD HtiMessage::GetExtSize()
{
	return m_Message[KMsgExtSizeOffset];
}

int HtiMessage::GetBodyStart()
{
	return GetExtSize()+MinHeaderSize();
}

void* HtiMessage::GetBody()
{
	return m_Message+GetBodyStart();
}

DWORD HtiMessage::GetServiceUID()
{
	return *((DWORD*)(m_Message + KMsgServiceNameOffset));
}

bool HtiMessage::CheckValidHtiHeader(void* header)
{
	//check CRC16
	WORD headerCrc16 = *((WORD*)((BYTE*)header+KMsgCrcOffset));
	WORD calcCrc16 = CRC16CCITT(header, KMsgCrcOffset);

	return headerCrc16 == calcCrc16;
}

DWORD HtiMessage::ExtractMsgSizeFromHeader(void* header)
{
	return *((DWORD*)(((BYTE*)header)+KMsgBodySizeOffset));
}

int HtiMessage::MinHeaderSize()
{
	return KMsgHeaderMinSize;
}

HtiMessage* HtiMessage::CreateErrorMessage(int errorCode, const char* errDescr)
{
	BYTE* msg = new BYTE[ HTI_MAX_ERR_MSG_SIZE ];
	memset(msg, 0, HTI_MAX_ERR_MSG_SIZE );
	msg[0] = HTI_ERR_CMD;
	*((DWORD*)(msg + HtiServiceErrCodeOffset)) = errorCode;
	*((DWORD*)(msg + HtiErrServiceUidOffset)) = HTI_SYSTEM_SERVICE_UID;
	int descrLen = min(strlen( errDescr ), HTI_MAX_ERR_MSG_SIZE - HtiErrDescrOffset );
	memcpy( msg + HtiErrDescrOffset,
			errDescr,
			descrLen
			);
	HtiMessage* newMsg = new HtiMessage(HTI_SYSTEM_SERVICE_UID,
										msg,
										descrLen + HtiErrDescrOffset + MinHeaderSize());
	return newMsg;
}

bool HtiMessage::IsErrorMessage()
{
	if ( m_Message )
	{
		return GetServiceUID() == HTI_SYSTEM_SERVICE_UID &&
		   m_Message[GetBodyStart()] == HTI_ERR_CMD &&
		   GetBodySize()>= HTI_MIN_ERR_MSG_SIZE ;
	}
	return false;
}

int HtiMessage::HtiErrorCode()
{
	if ( IsErrorMessage() )
	{
		return m_Message[GetBodyStart()+HtiErrCodeOffset];
	}
	return -1;
}
int HtiMessage::ServiceErrorCode()
{
	if ( IsErrorMessage() )
	{
		return  *((DWORD*)(m_Message + GetBodyStart() + HtiServiceErrCodeOffset));
	}
	return -1;
}

char* HtiMessage::ErrorDescription()
{
	if ( IsErrorMessage() )
	{
		int desrLen = GetBodySize() - HtiErrDescrOffset;
		m_ErrDescr = new char[desrLen+1];
		//_RPT2(_CRT_WARN,"HtiMessage::ErrorDescription %x <%d>\n", m_ErrDescr, desrLen+1);
		memcpy( m_ErrDescr,
				m_Message + GetBodyStart() + HtiErrDescrOffset,
				desrLen );
		m_ErrDescr[desrLen] = 0;
		return m_ErrDescr;
	}
	return NULL;
}

int HtiMessage::ErrorServiceUid()
{
	if ( IsErrorMessage() )
	{
		return  *((DWORD*)(m_Message + GetBodyStart() + HtiErrServiceUidOffset));
	}
	return -1;
}