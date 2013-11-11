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
*   This file contains headers of HtiMessage class.
*/
#ifndef __HTI_MSG__
#define __HTI_MSG__

#include <windows.h>

static const DWORD HTI_SYSTEM_SERVICE_UID = 0x1020DEB6;

const int KMsgServiceNameOffset = 0;
const int KHtiMsgServiceUidLen = 4;
const int KMsgBodySizeLen = 4;
const int KMsgBodySizeOffset = KMsgServiceNameOffset + KHtiMsgServiceUidLen;
const int KMsgVersionOffset = KMsgBodySizeOffset+KMsgBodySizeLen;
const int KMsgPriorityOffset = KMsgVersionOffset+1;
const int KMsgWrapFlagOffset = KMsgPriorityOffset+1;
const int KMsgExtSizeOffset = KMsgWrapFlagOffset+1;
const int KMsgCrcOffset = KMsgExtSizeOffset+1; //two bytes field
const int KMsgExtOffset = KMsgCrcOffset+2;
const int KMsgHeaderMinSize = KMsgExtOffset;

const BYTE KDefaultVersion = 1;

//error message
const int HtiErrCodeOffset = 1;
const int HtiServiceErrCodeOffset = HtiErrCodeOffset + 1;
const int HtiErrServiceUidOffset = HtiServiceErrCodeOffset + 4;
const int HtiErrDescrOffset = HtiErrServiceUidOffset + 4;

class HtiMessage
{
public:
	/**
	* Creates HTIMessage using given parameters and message body
	*/
	HtiMessage(DWORD serviceId, void* body, DWORD len, BYTE priority=0);
	
	/**
	* Creates HTIMessage parsing ready HTI message (with header)
	* message may contain only message beginning but should include valid
	* HTI header (see CheckValidHtiHeader())
	* The rest of message can be added using AddToBody()
	*/
    HtiMessage(void* message, DWORD len);

	/**
	* Destructor
	*/
	~HtiMessage();

	/**
	* Add body parts if HTIMessage was constructed with incomplete data
	* Returns number of added bytes.
	*/
	DWORD AddToBody(void* data, DWORD len);

	inline bool IsMessageComplete();
	inline int Reminder();

	/**
	* Returns whole HTI message including header
	*/
	void* HtiData();
	
	/**
	*
	*/
	DWORD HtiDataSize();

	DWORD GetBodySize();
	DWORD GetExtSize();
	void* GetBody();
	DWORD GetServiceUID();

	/**
	* Attemt to read HTI header in provided data.
	* Pointer should reference to data at least MinHeaderSize() size.
	*/
	static bool CheckValidHtiHeader(void* header);

	/**
	* 
	*/
	static DWORD ExtractMsgSizeFromHeader(void* header);

	/**
	* Returns minimal HTI message header
	*/
	static int MinHeaderSize();

	//buit-in support for default error messages
	static HtiMessage* CreateErrorMessage(int errorCode, const char* errDescr);

	bool IsErrorMessage();
	int HtiErrorCode();
	int ServiceErrorCode();
	char* ErrorDescription();
	int ErrorServiceUid();

protected:
	int GetBodyStart();

	DWORD m_Size; //size of whole message (inc. header)
	BYTE* m_Message; //whole message (inc. header)

	/**
	* Number of bytes left to copy to m_Message
	* Used when constructing message from data packages
	*/
	DWORD m_MessageReminderSize;
	//temp string for err descr with 0-ending
	char* m_ErrDescr;
};

inline bool HtiMessage::IsMessageComplete(){return m_MessageReminderSize==0;}
inline int HtiMessage::Reminder(){return m_MessageReminderSize;}

#endif //