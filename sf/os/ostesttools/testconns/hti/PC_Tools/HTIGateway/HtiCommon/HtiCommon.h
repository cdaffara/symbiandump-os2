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
#ifndef __HTI_COMMON__
#define __HTI_COMMON__

#ifdef _EXPORT_HTI_COMMON_
	#define EXPORT_FROM_DLL __declspec(dllexport)
#else
	#define EXPORT_FROM_DLL __declspec(dllimport)
#endif

#include <windows.h>

// CONSTANTS
const int HTIMSG_TIMEOUT_5_SECONDS  =  5000; // milliseconds
const int HTIMSG_TIMEOUT_10_SECONDS = 10000;
const int HTIMSG_TIMEOUT_30_SECONDS = 30000;
const int HTIMSG_TIMEOUT_60_SECONDS = 30000;

// GENERAL UTIL FUNCTIONS
/**
* Checks if a string parameter is valid. Not null or empty.
*/
EXPORT_FROM_DLL int check_mandatory_string_parameter(struct soap* soap,
													 char* parameter,
													 char* parameterName);
/**
* Checks if a wide character string parameter is valid. Not null or empty.
*/
EXPORT_FROM_DLL int check_mandatory_wcstring_parameter(struct soap* soap,
													   wchar_t* parameter,
													   char* parameterName);
/**
* A little addition to basic soap functions. Same as soap_receiver_fault 
* but can add formatted data to faultstring.
*/
EXPORT_FROM_DLL int soap_receiver_fault_format(struct soap *soap,
											   const char *faultstring,
											   const char *faultdetail, ...);
/**
* Prints bytes to the screen
*/
EXPORT_FROM_DLL void dump( BYTE *data, int dataLen );


// CLASSES
// Class for HTI message manipulation
class HtiMsgHelper
{
public:
	/**
	* Constructors.
	*/
	EXPORT_FROM_DLL HtiMsgHelper( struct soap *soap );
	EXPORT_FROM_DLL HtiMsgHelper( struct soap *soap, DWORD serviceId );
	EXPORT_FROM_DLL HtiMsgHelper( struct soap *soap, DWORD serviceId, BYTE command );

	/**
	* Destructor.
	*/
	EXPORT_FROM_DLL ~HtiMsgHelper();
	
	/**
	* General util methods.
	*/
	EXPORT_FROM_DLL BYTE* GetMsgBody() { return m_msgBody; };
	EXPORT_FROM_DLL int   GetMsgLen() { return m_msgBodyLen; };

	/**
	* Methods for getting data from HTI message.
	*/
	EXPORT_FROM_DLL BYTE  GetByte( int offset ) { return *(BYTE*)(m_msgBody+offset); };
	EXPORT_FROM_DLL WORD  GetWord( int offset ) { return *(WORD*)(m_msgBody+offset); };
	EXPORT_FROM_DLL DWORD GetDWord( int offset ) { return *(DWORD*)(m_msgBody+offset); };
	EXPORT_FROM_DLL int   GetInt( int offset ) { return *(int*)(m_msgBody+offset); };
	EXPORT_FROM_DLL char* GetSoapString( int offset, int len );
	EXPORT_FROM_DLL wchar_t* GetSoapWCString( int offset, int len );

	/**
	* Received HTI message checking methods
	*/
	EXPORT_FROM_DLL int CheckMsgExactLen( int len );
	EXPORT_FROM_DLL int CheckMsgMinLen( int len );
	EXPORT_FROM_DLL int CheckCommandCode( BYTE code );

	/**
	* Methods for adding data to HTI message.
	*/
	EXPORT_FROM_DLL void AddByte( BYTE data );
	EXPORT_FROM_DLL void AddWord( WORD data );
	EXPORT_FROM_DLL void AddDWord( DWORD data );
	EXPORT_FROM_DLL void AddInt( int data );
	EXPORT_FROM_DLL void AddUInt64( unsigned __int64 data );
	EXPORT_FROM_DLL void AddString( char* string );
	EXPORT_FROM_DLL void AddWCString( wchar_t* string );
	EXPORT_FROM_DLL void AddData( void* data, int dataLen );

	// does not add if null or length is zero
	EXPORT_FROM_DLL void AddStringWithLengthByte( char* string ); 
	EXPORT_FROM_DLL void AddWCStringWithLengthByte( wchar_t* string );

	// adds at least the lenght byte
	EXPORT_FROM_DLL void AddStringWithLengthByteZero( char* string ); 
	EXPORT_FROM_DLL void AddStringWithLengthWordZero( char* string );
    EXPORT_FROM_DLL void AddWCStringWithLengthByteZero( wchar_t* string );
	
	/**
	* Methods for sending and receiving.
	* Can be replaced if needed.
	*/
	virtual EXPORT_FROM_DLL void SendMsg();
	virtual EXPORT_FROM_DLL int ReceiveMsg( int timeout );
	virtual EXPORT_FROM_DLL int SendReceiveMsg( int timeout );

protected:
	/**
	* Increases m_msgBody for adding new data
	*/
	void IncBodySize( int size );

protected:
	/**
	* soap env.
	*/
	struct soap *m_soap;

	/**
	* Service id used in sending the message.
	*/
	DWORD        m_serviceId;

	/**
	* The actual message and its length in bytes.
	* Destroyed when message is sent. Received message is not destroyed 
	* because we dont have ownership.
	*/
	BYTE*        m_msgBody; 
	int          m_msgBodyLen;
};

#endif //__HTI_COMMON__