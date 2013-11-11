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
*   This file contains implementation of SerialComm communication channel plugin objects. These classes are SerialComm, SerialCommPlugin and
*	SerialCommIoThread classes.
*/

// INCLUDES
#include "SerialCommPlugin.h"
#include "util.h"

#define HIGH_COM_PORT "\\\\.\\"
const static int g_SerialMaxResendNumber = 2;

//**********************************************************************************
// Class SerialCommPlugin
//
// This class implements a CommChannelPlugin used in serial communication with device
//**********************************************************************************

SerialCommPlugin::SerialCommPlugin(const CommChannelPluginObserver* observer)
    : CommChannelPlugin(observer),
      m_TxQueue(),
      m_RxQueue(),
      m_PropertySerialCommTimeout(g_SerialCommDefaultTimeout),
	  m_PropertySerialMaxSendSize(0),
	  m_ProperySerialSendPause(0),
	  m_PropertySerialHwFlowControl(false)
{
    m_IoThread = NULL;
}

SerialCommPlugin::~SerialCommPlugin()
{
    Util::Debug("SerialCommPlugin::~SerialCommPlugin()");
    if ( m_Open )
    {
        Close();
    }
    if ( m_IoThread != NULL )
    {
        delete m_IoThread;
        m_IoThread = NULL;
    }
}

/*
 * This method initializes SerialCommPlugin and starts SerialCommPluginIoThread
 */
DWORD SerialCommPlugin::Init()
{
    Util::Debug("SerialCommPlugin::Init()");

    std::string filename = SERIAL_INI_FILE_NAME;
	//read properties from serialplugin.ini file
	Util::ReadProperties(filename.c_str(), m_SerialCommPluginProperties);
    
	CheckProperties(m_SerialCommPluginProperties);

    CommChannelPlugin::Init();

    m_IoThread = new SerialCommIoThread(
						&m_RxQueue,
						&m_TxQueue,
                        m_PropertySerialCommInitString,
						m_PropertySerialCommPort,
						m_PropertySerialCommTimeout,
						m_PropertySerialHwFlowControl,
						m_ProperySerialSendPause,
						m_PropertySerialMaxSendSize
						);

	m_IoThread->Start();

	Event e = m_IoThread->GetOpenedEvent();
    if ( e.Wait(m_PropertySerialCommTimeout) == WAIT_TIMEOUT )
    {
        Util::Debug("SerialCommPlugin::Init()   SerialComm opening timed out");
        m_Open = false;
        return ERR_DG_COMM_OPEN_TIMEOUT;
    }
    Util::Debug("SerialCommPlugin::Init() SerialComm opened");
    m_Open = true;
    Util::Debug("SerialCommPlugin::Init() OK");
    return NO_ERRORS;
}

/*
 * This method initializes class member variables from values in map
 */
void SerialCommPlugin::CheckProperties(map<string, string>& props)
{
    char tmp[256];

    // Init values for connection
    m_PropertySerialCommInitString = props[SERIAL_INI_INIT_PARAM];
	Util::CheckCommandlineParam( PARAM_SWITCH_INIT_PARAM, m_PropertySerialCommInitString );
    if ( m_PropertySerialCommInitString.empty() )
    {
        m_PropertySerialCommInitString = SERIAL_DEFAULT_INIT_STRING;
    }
    sprintf_s(tmp, 256, "Connection Init : '%s'", m_PropertySerialCommInitString.c_str());
    string s(tmp);
    Util::Debug(s);

    // comm port
    m_PropertySerialCommPort = props[SERIAL_INI_COMPORT_PARAM];
	Util::CheckCommandlineParam( PARAM_SWITCH_COMPORT_PARAM, m_PropertySerialCommPort );
    if ( m_PropertySerialCommPort.empty() )
    {
        m_PropertySerialCommPort = SERIAL_DEFAULT_COMPORT;
    }
    sprintf_s(tmp, 256, "Connection port : '%s'", m_PropertySerialCommPort.c_str());
    s = tmp;
    Util::Debug(s);

    // Serial operations timeout
    string val = props[SERIAL_INI_COMPORT_TIMEOUT];
	Util::CheckCommandlineParam( PARAM_SWITCH_COMPORT_TIMEOUT, val );
	m_PropertySerialCommTimeout = 0;
    if ( !val.empty() )
    {
        m_PropertySerialCommTimeout = atol(val.c_str());
    }

    if ( m_PropertySerialCommTimeout <= 0 )
    {
        m_PropertySerialCommTimeout = g_SerialCommDefaultTimeout;
    }
	sprintf_s(tmp, 256, "Timeout: '%d'", m_PropertySerialCommTimeout);
    s = tmp;
    Util::Debug(s);

	// flow control
	m_PropertySerialHwFlowControl = false;
	val = props[SERIAL_INI_USE_HW_FLOW_CONTROL];
	Util::CheckCommandlineParam( PARAM_SWITCH_USE_HW_FLOW_CONTROL, val );
	if ( !val.empty() && val == "1" )
	{
		m_PropertySerialHwFlowControl = true;
		Util::Debug("Use serial HW flow control");
	}
	else
	{
		Util::Debug("Do not use serial HW flow control");
	}

	// send pause
	val = props[SERIAL_INI_SEND_SLEEP_PAUSE];
	Util::CheckCommandlineParam( PARAM_SWITCH_SEND_SLEEP_PAUSE, val );
    if ( !val.empty() )
    {
        m_ProperySerialSendPause = atol(val.c_str());
    }
    if ( m_ProperySerialSendPause <= 0 )
    {
        m_ProperySerialSendPause = g_SerialCommDefaultSendPause;
    }
	sprintf_s(tmp, 256, "Send pause: '%d'", m_ProperySerialSendPause);
    s = tmp;
    Util::Debug(s);

	// send size
	val = props[SERIAL_INI_SEND_SIZE];
	Util::CheckCommandlineParam( PARAM_SWITCH_SEND_SIZE, val );
    if ( !val.empty() )
    {
        m_PropertySerialMaxSendSize = atol(val.c_str());
    }
    if ( m_PropertySerialMaxSendSize <= 0 )
    {
        m_PropertySerialMaxSendSize = 0;
    }
	sprintf_s(tmp, 256, "Send max size: '%d'", m_PropertySerialMaxSendSize);
    s = tmp;
    Util::Debug(s);
}

/*
 * This method checks if data is available on incoming queue
 */
bool SerialCommPlugin::IsDataAvailable()
{
    return ( !m_RxQueue.empty() );
}

/*
 * This method is used to:
 * -push given data to outgoing queue
 * -wait for data to become available in incoming queue
 * -pop the first Data object from queue and store it to the Data object given as parameter
 */
DWORD SerialCommPlugin::SendReceive(Data* data_in, Data** data_out, long timeout)
{
    DWORD res;
    if ( ( res = Send(data_in, timeout)) == NO_ERRORS &&
         ( res = ReceiveWait(data_out, timeout)) == NO_ERRORS )
    {
        return NO_ERRORS;
    }
    cout << "SerialCommPlugin::SendReceive: error" << endl;
    return res;
}

/*
 * This method pushes the given Data object(of type Data::EData) to outgoing queue
 */
DWORD SerialCommPlugin::Send(Data* data_in, long timeout)
{
    Data::DataType type = data_in->GetType();
    if ( type == Data::EData )
    {
        DWORD length = data_in->GetLength();
        m_TxQueue.push(data_in);
        return NO_ERRORS;
    }
    else if ( type == Data::EControl )
    {
        Util::Debug("SerialCommPlugin::Send: Control Message Received");
        return NO_ERRORS;
    }
    return ERR_DG_COMM_DATA_SEND;
}

/*
 * This method is used to wait for data to become available in incoming queue 
 * and then pop the first Data object from the queue and and store it to Data object given as parameter.
 */
DWORD SerialCommPlugin::ReceiveWait(Data** data_out, long timeout)
{
    long elapsed = 0;
    while (elapsed < timeout && !IsDataAvailable())
    {
        elapsed += 25;
        Sleep(25);
    }
    if ( elapsed >= timeout )
    {
        return ERR_DG_COMM_DATA_RECV_TIMEOUT;
    }
    return Receive(data_out, timeout);
}

/*
 * This method is used to pop the first data object from incoming queue and store it to data object given as parameter
 */
DWORD SerialCommPlugin::Receive(Data** data_out, long timeout)
{
    if ( !m_RxQueue.empty() )
    {
        *data_out = m_RxQueue.front();
        m_RxQueue.pop();
        return NO_ERRORS;
    }
    return ERR_DG_COMM_DATA_RECV;
}


DWORD SerialCommPlugin::Open()
{
    return ( m_Open ? NO_ERRORS : ERR_DG_COMM_OPEN );
}

/*
 * This method stops SerialCommIoThread and waits for it to be in signaled state
 */
DWORD SerialCommPlugin::Close()
{
    m_IoThread->Stop();
    WaitForSingleObject( m_IoThread->ThreadHandle(), g_MaximumShutdownWaitTime );
    return NO_ERRORS;
}

//**********************************************************************************
// Class SerialComm
//
// This class implements the actual serial communication using Windows API
//**********************************************************************************

SerialComm::SerialComm()
    : m_Open(false),
	m_SendPause(0),
	m_SendMaxSize(0)
{

}

SerialComm::~SerialComm()
{
    Util::Debug("SerialComm::~SerialComm()");
    if ( m_Open )
    {
        Close();
    }
}
/*
 * This method opens a communication device that uses serial communication and
 * configures it according initialization parameters
 */
WORD SerialComm::Open( const string& commPort, const string& initParameters,
					   long timeout,
					   bool hwFlowControl, long sendPause, long sendMaxSize)
{
    Util::Debug("SerialComm::Open()");

	m_SendPause = sendPause;

	if ( sendMaxSize>0 )
	{
		m_SendMaxSize = sendMaxSize;
	}

	DWORD err;
	//check commPort and add "\\.\" prefix if port is larger than 9
	string localCommPort(commPort);
	static const basic_string <char>::size_type npos = -1;
	if ( localCommPort.find(HIGH_COM_PORT) == npos &&
		 localCommPort.find("COM") == 0 &&
		 localCommPort.length() > 4 //two digit port number
	   )
	{
		localCommPort.insert(0, HIGH_COM_PORT);
	}
	Util::Debug(localCommPort.c_str());

    // If COM-port opening fails, it's retried until HtiGateway is killed.
    m_Com = INVALID_HANDLE_VALUE;
    while ( m_Com == INVALID_HANDLE_VALUE )
    {
	    // Open file (COM port)
	    m_Com = CreateFile( localCommPort.c_str(),
					    GENERIC_READ | GENERIC_WRITE,
					    0, //excusive access
					    NULL, //no security
					    OPEN_EXISTING, //must use
					    0, //not overlapped i/o
					    NULL // must be NULL for comm
					    );

	    if ( m_Com == INVALID_HANDLE_VALUE )
	    {
		    err = GetLastError();
            Util::Error("[SerialComm] Failed to open COM port", err);
            Util::Info("[SerialComm] Retrying...");
            Sleep(5000);
        }
    }

    // Serial port cfg
	Util::Debug(initParameters.c_str());
	DCB dcb;

	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);
	if ( !BuildCommDCB(initParameters.c_str(), &dcb) )
	{
		err = GetLastError();
		Util::Error("[SerialComm] Failed to build port settings ", err);
		return ERR_DG_COM_INIT;
	}

	dcb.fBinary = TRUE ; 

	//set hw handshaking
	dcb.fOutX  = FALSE;
    dcb.fInX   = FALSE;
    dcb.fNull   = FALSE;
    dcb.fAbortOnError = TRUE;
	
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity= FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;

	if ( hwFlowControl )
	{
		dcb.fOutxCtsFlow = TRUE;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	}
	else
	{
		dcb.fOutxCtsFlow = FALSE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
	}

	if ( !SetCommState(m_Com, &dcb) )
	{
		err = GetLastError();
		Util::Error("[SerialComm] Failed to set port settings ", err);
		return ERR_DG_COM_INIT;
	}

	COMMTIMEOUTS timeouts;

	timeouts.ReadIntervalTimeout = MAXDWORD; 

	timeouts.ReadTotalTimeoutMultiplier = timeout;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = timeout;

	if ( !SetCommTimeouts(m_Com, &timeouts) )
	{
		// Error setting time-outs.
		err = GetLastError();
		Util::Error("[SerialComm] Failed to set port timeouts ", err);
		return ERR_DG_COM_INIT;
	}


	m_Open = true;
    Util::Debug("SerialComm::Open() OK");
    Util::Info("[SerialComm] COM port open");
    return NO_ERRORS;
}

/*
 * This method is used to send Data object using serial communication
 */
DWORD SerialComm::Send(const Data& data_in)
{
    Util::Debug("SerialComm::Send()");
	if ( data_in.GetLength() > 0 )
	{
		DWORD sentBytes = 0;
		void* data = data_in.GetData();
		int numOfTries = 0;
		//loop until whole Data object is sent
		while ( sentBytes < data_in.GetLength() )
		{
			//Util::Debug("while");
			DWORD sent;
			BOOL r;
			if ( m_SendMaxSize>0 )
			{
				//Write data to the communication device				
				r = WriteFile(m_Com,
						((char*)data) + sentBytes,
						//data_in.GetLength() - sentBytes,
						min(data_in.GetLength() - sentBytes, m_SendMaxSize),
						&sent,
						NULL);
			}
			else
			{
				r = WriteFile(m_Com,
						((char*)data) + sentBytes,
						data_in.GetLength() - sentBytes,
						&sent,
						NULL);
			}

			if ( r && sent > 0 )
			{
				if (Util::GetVerboseLevel() == Util::debug)
				{
					Util::Hex(((char*)data) + sentBytes, min(sent,16));
				}
				sentBytes+=sent;
			}
			else
			{
				if ( !r )
				{
					DWORD err = GetLastError();
					Util::Error("[SerialComm] Send error", err);
				}
				else
				{
					Util::Error("[SerialComm] Send failed");
				}
				if ( numOfTries++==g_SerialMaxResendNumber )
				{
					return ERR_DG_COMM_DATA_RECV;
				}
			}
			//used to "solve" overflow problem
			if ( m_SendPause > 0 )
			{
				Sleep(m_SendPause);
			}
		}
	}

    Util::Debug("SerialComm::Send() OK");
    return NO_ERRORS;
}

/*
 * This method reads all available data from communication media 
 * and returns it in the Data object given as parameter
 */
DWORD SerialComm::Receive(Data* data_out)
{
    Util::Debug("SerialComm::Receive()");

	//get available bytes in incoming communication channel	
	LONG rxBytes = GetRxBytesAvailable();

	if ( rxBytes > 0 )
	{
		char* data = new char[rxBytes];
		DWORD readBytes = 0;
		//read data until all available bytes have been red		
		do
		{
			DWORD reallyRead;
			if ( ReadFile(m_Com,
					 data + readBytes,
					 rxBytes - readBytes,
					 &reallyRead,
					 NULL) )
			{
				if (Util::GetVerboseLevel() == Util::debug)
				{
					char temp[64];
					sprintf_s(temp, 64, "Received %d bytes", reallyRead);
					Util::Debug( temp );
					Util::Hex(data + readBytes, min(16,reallyRead));
				}
				readBytes += reallyRead;
			}
			else
			{
				delete[] data;
				DWORD err = GetLastError();
				Util::Error("[SerialComm] Error %d", err);
				return ERR_DG_COMM_DATA_RECV;
			}
			if ( readBytes > rxBytes )
			{
				Util::Error("[SerialComm] !!! readBytes > rxBytes !!!");
			}
		}
		while( readBytes < rxBytes );

		data_out->SetData( data, rxBytes, Data::EData);
		delete[] data;
	}
	else if ( rxBytes < 0 ) //err
	{
        return ERR_DG_COMM_DATA_RECV;
	}
	return NO_ERRORS;
}

/*
 * This method returns number of bytes available in incoming queue or
 * -1 if communication error has occurred
 */
LONG SerialComm::GetRxBytesAvailable()
{
    COMSTAT res;
	DWORD err;

	if ( ClearCommError(m_Com, &err, &res ) )
	{
		return res.cbInQue;
	}
	else
    {
		DWORD err = GetLastError();
        Util::Error("[SerialComm] Error getting Rx bytes %d", err);
        return -1;
    }
}

DWORD SerialComm::Close()
{
    Util::Debug("SerialComm::Close()");
    m_Open = false;

	if ( m_Com != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_Com);
		m_Com = INVALID_HANDLE_VALUE;
	}

    Util::Debug("SerialComm::Close() OK");
    return NO_ERRORS;
}

//**********************************************************************************
// Class SerialCommIoThread
//
// This thread is used to send data from outgoing queue to serial port
// and read bytes from serial port and push it to incoming queue 
//**********************************************************************************

SerialCommIoThread::SerialCommIoThread(SafeQueue<Data*>* in,
                           SafeQueue<Data*>* out,
                           const string& init,  const string& commPort, long timeout,
						   bool hwFlowControl, long sendPause, long sendMaxSize)
    : m_InitString(init),
	  m_CommPort( commPort ),
	  m_PropertySerialCommTimeout( timeout ),
	  m_PropertySerialHwFlowControl( hwFlowControl ),
	  m_ProperySerialSendPause(sendPause),
	  m_PropertySerialMaxSendSize(sendMaxSize)
{
    m_InQueue = in;
    m_OutQueue = out;
    m_Running = true;
    m_OpenedEvent.Reset();
}

/*
 * Main execution loop which is used to send Data from outgoing queue to serial port
 * and read bytes from serial port, encapsulate it to Data objects and push them to incoming queue 
 */
void SerialCommIoThread::Run()
{
    while ( m_Running )
    {
	    Util::Debug("SerialCommIoThread::Run()");
        SerialComm m_SerialComm;

        if ( m_SerialComm.Open(m_CommPort, m_InitString, m_PropertySerialCommTimeout,
							    m_PropertySerialHwFlowControl, m_ProperySerialSendPause,
							    m_PropertySerialMaxSendSize) != NO_ERRORS )
        {
            m_Running = false;
            return;
        }
        m_OpenedEvent.Set();

	    Util::Debug("SerialCommIoThread::Run() start loop");
        while ( m_Running )
        {
            // Sending data
            try
            {
                Data* out = m_OutQueue->front(30);
                if ( m_SerialComm.Send(*out) != NO_ERRORS )
                {
                //Data* d = new Data("Error sending data", 19, Data::EControl);
				Data* d = CREATE_CONTROL_MESSAGE("[HtiGateway]: Error sending data");
                    m_InQueue->push(d);
                    d = NULL;
                }
                m_OutQueue->pop();
                delete out;
                out = NULL;
            } catch (TimeoutException te) {}


		    // Receiving data
		    LONG rxBytes = 0;
            while ( ( rxBytes = m_SerialComm.GetRxBytesAvailable() ) > 0 )
            {
                Data* in = new Data;
                if ( m_SerialComm.Receive(in) == NO_ERRORS )
                {
				    m_InQueue->push(in);
				    in = NULL;
                }
                else
                {
                    delete in;
                    in = NULL;
                }
            }
            if ( rxBytes < 0 )
            {
                Util::Info("[SerialCommIoThread] COM-port lost. Trying to close & reopen");
                break; // break out but keep the outermost while loop running
            }
        }
        m_OpenedEvent.Reset();
        m_SerialComm.Close();
    }
    Util::Debug("SerialCommIoThread::Run() exiting");
}

void SerialCommIoThread::Stop()
{
    Util::Debug("SerialCommIoThread::Stop()");
    m_Running = false;
}

Event SerialCommIoThread::GetOpenedEvent()
{
    return m_OpenedEvent;
}

void SerialCommIoThread::SetTimeout(long timeout)
{
    if ( timeout > 0 )
    {
        m_PropertySerialCommTimeout = timeout;
    }
    else
    {
        m_PropertySerialCommTimeout = g_SerialCommDefaultTimeout;
    }
}

long SerialCommIoThread::GetTimeout() const
{
    return m_PropertySerialCommTimeout;
}

// End of the file
