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
*   This file contains headers of SerialComm communication channel plugin objects. 
*   These classes are SerialComm, SerialCommPlugin and SerialCommIoThread classes.
*/

#ifndef SERIALCOMMPLUGIN_H
#define SERIALCOMMPLUGIN_H

#pragma warning ( disable : 4786 )
#include <iostream>
#include <process.h>
#include <string>
#include <map>
#include <comdef.h>

#include "common.h"
#include "plugin.h"
#include "safequeue.h"
#include "thread.h"

using namespace std;

#define SERIAL_INI_FILE_NAME             "serialplugin.ini"

#define SERIAL_INI_INIT_PARAM            "INIT_STRING"
#define PARAM_SWITCH_INIT_PARAM          "-INIT_STRING"
#define SERIAL_INI_COMPORT_PARAM         "COMPORT"
#define PARAM_SWITCH_COMPORT_PARAM       "-COMPORT"
#define SERIAL_INI_COMPORT_TIMEOUT       "TIMEOUT"
#define PARAM_SWITCH_COMPORT_TIMEOUT     "-TIMEOUT"
#define SERIAL_INI_USE_HW_FLOW_CONTROL   "HW_FLOWCONTROL"
#define PARAM_SWITCH_USE_HW_FLOW_CONTROL "-HW_FLOWCONTROL"
#define SERIAL_INI_SEND_SLEEP_PAUSE      "SEND_PAUSE"
#define PARAM_SWITCH_SEND_SLEEP_PAUSE    "-SEND_PAUSE"
#define SERIAL_INI_SEND_SIZE             "SEND_SIZE"
#define PARAM_SWITCH_SEND_SIZE           "-SEND_SIZE"

#define SERIAL_DEFAULT_INIT_STRING       "115200,n,8,1"
#define SERIAL_DEFAULT_COMPORT           "COM2"

const DWORD g_SerialCommDefaultTimeout    = 40000; //ms
const DWORD g_SerialPluginTimeout         = 10000; //not used
const long  g_SerialCommDefaultSendPause  = 0;

// Maximum time to wait
extern long g_MaximumShutdownWaitTime;

//**********************************************************************************
// Class SerialComm
//
// This class implements the actual serial communication
//**********************************************************************************

class SerialComm
{
public:
	SerialComm();
	~SerialComm();
	/*
	 * This method opens a communication device that uses serial communication and
	 * configures it according initialization parameters
	 */	
	WORD Open( const string& commPort, const string& initParameters, long timeout,
				bool hwFlowControl, long sendPause, long sendMaxSize);
	DWORD Close();
	/*
	 * This method is used to send Data object using serial communication
	 */		
	DWORD Send(const Data& data_in);
	/*
	 * This method reads all available data from communication media 
	 * and returns it in the Data object given as parameter
	 */	
	DWORD Receive(Data* data_out);
	/*
	 * This method returns number of bytes available in incoming queue or
	 * -1 if communication error has occurred
	 */	
	LONG GetRxBytesAvailable();
private:
	//handle to communication resource
	HANDLE m_Com;
	bool m_Open;
	long m_SendPause;
	long m_SendMaxSize;
};

//**********************************************************************************
// Class SerialCommIoThread
//
// This thread is used to send data from outgoing queue to serial port
// and read bytes from serial port and push it to incoming queue 
//**********************************************************************************

class SerialCommIoThread : public Thread<SerialCommIoThread>
{
public:
	SerialCommIoThread(SafeQueue<Data*>* in, SafeQueue<Data*>* out,
						const string& init, const string& commPort, long timeout,
						bool hwFlowControl, long sendPause, long sendMaxSize);
	/*
	 * Main execution loop which is used to send Data from outgoing queue to serial port
	 * and read bytes from serial port, encapsulate it to Data objects and push them to incoming queue 
	 */		
	void Run();
	void Stop();
	void SetTimeout(long timeout);
	long GetTimeout() const;
	Event GetOpenedEvent();
private:
	//incoming queue
	SafeQueue<Data*>* m_InQueue;
	//outgoing queue
	SafeQueue<Data*>* m_OutQueue;
	Event m_OpenedEvent;
	bool m_Running;
	long m_PropertySerialCommTimeout;
	const string& m_InitString;
	const string& m_CommPort;
	long m_ProperySerialSendPause;
	bool m_PropertySerialHwFlowControl;
	long m_PropertySerialMaxSendSize;
};

//**********************************************************************************
// Class SerialCommPlugin
//
// This class implements a CommChannelPlugin used in serial communication with device
//**********************************************************************************

class SerialCommPlugin : public CommChannelPlugin
{
public:
	/*
	 * This method initializes SerialCommPlugin and starts SerialCommPluginIoThread
	 */
	DWORD Init();
	/*
	 * This method is used to:
	 * -push given data to outgoing queue
	 * -wait for data to become available in incoming queue
	 * -pop the first Data object from queue and store it to the Data object given as parameter
	 */		
	DWORD SendReceive(Data* data_in, Data** data_out, long timeout = g_SerialPluginTimeout);
	/*
	 * This method pushes the given Data object(of type Data::EData) to outgoing queue
	 */		
	DWORD Send(Data* data_in, long timeout = g_SerialPluginTimeout);
	/*
	 * This method is used to pop the first data object from incoming queue and store it to data object given as parameter
	 */		
	DWORD Receive(Data** data_out, long timeout = g_SerialPluginTimeout);
	/*
	 * This method is used to wait for data to become available in incoming queue 
	 * and then pop the first Data object from the queue and and store it to Data object given as parameter.
	 */		
	DWORD ReceiveWait(Data** data_out, long timeout = g_SerialPluginTimeout);
	/*
	 * This method checks if data is available on incoming queue
	 */		
	bool IsDataAvailable();
	DWORD Open();
	/*
	 * This method stops SerialCommIoThread and waits for it to be in signaled state
	 */		
	DWORD Close();
	SerialCommPlugin(const CommChannelPluginObserver* observer = NULL);
	~SerialCommPlugin();
private:
	void CheckProperties(map<string, string>& props);
private:
    //outgoing queue
	SafeQueue<Data*> m_TxQueue;
	//incoming queue	
	SafeQueue<Data*> m_RxQueue;
	SerialCommIoThread* m_IoThread;
	//map that properties are read to from ini file
	map<string, string> m_SerialCommPluginProperties;
	long m_PropertySerialCommTimeout;
	string m_PropertySerialCommInitString;
	string m_PropertySerialCommPort;
	long m_ProperySerialSendPause;
	bool m_PropertySerialHwFlowControl;
	long m_PropertySerialMaxSendSize;
};

#endif

// End of the file