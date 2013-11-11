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
#include "HtiStifH.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"
#include "../../htigateway/inc/DllModule.h"


#include <iosfwd>
#include <sstream>


//**********************************************************************************
// CONSTANTS
//
//**********************************************************************************

// commands
const BYTE CMD_STIF_OPEN				= 0x01;
const BYTE CMD_STIF_CLOSE				= 0x02;
const BYTE CMD_STIF_LOAD_MODULE			= 0x03;
const BYTE CMD_STIF_UNLOAD_MODULE		= 0x04;
const BYTE CMD_STIF_LIST_CASES			= 0x05;
const BYTE CMD_STIF_START_CASE			= 0x06;
const BYTE CMD_STIF_CANCEL_CASE			= 0x07;
const BYTE CMD_STIF_PAUSE_CASE			= 0x08;
const BYTE CMD_STIF_RESUME_CASE			= 0x09;
const BYTE CMD_STIF_ADD_CASE_FILE		= 0x0A;
const BYTE CMD_STIF_REMOVE_CASE_FILE	= 0x0B;
const BYTE CMD_STIF_CASE_MESSAGE		= 0x0C;
const BYTE CMD_STIF_SET_DEVICEID		= 0x0D;
const BYTE CMD_STIF_TEST_COMPLETED		= 0x0E;
const BYTE CMD_STIF_ATS_MESSAGE			= 0x0F;
const BYTE CMD_STIF_SET_ATTRIBUTE		= 0x10;

// timeouts
const int TIMEOUT_10_SECONDS = 10000;

// soap-action for this plugin
const char soapAction[] = "HtiStif";

// error codes for handle_received_message()
#define HTI_RECV_ASYNC_CALL_SERVICED      1
#define HTI_RECV_OK                       0
#define HTI_RECV_GEN_ERROR                -1
#define HTI_RECV_ERROR_MESSAGE            -2
#define HTI_RECV_TIMEOUT                  -3
#define HTI_RECV_ASYNC_CALL_SERVICE_ERROR -4
#define HTI_RECV_ROUGUE_MESSAGE           -5
#define HTI_RECV_UNKOWN_MESSAGE           -6

//**********************************************************************************
// FORWARD DECLARATIONS
//
//**********************************************************************************

class HtiStifMsg;
class HtiStifClients;
class HtiStifSenderDll;
//class HtiStifResults

//int GetParameters(char* in, int inLen, char** parameters, int &size);
int handle_received_message(HtiSoapHandlerInterface* handler, BOOL isSoapCallActive);
void SetSoapFaultFromReceivedHtiError(struct soap *soap, BYTE *receivedMsgBody, int receivedMsgBodyLen);


//**********************************************************************************
// GLOBAL VARIABLES
//
//**********************************************************************************
//HtiStifResults g_testCaseResults;
HtiStifSenderDll *g_HtiStifSenderDll;
HtiStifClients *g_HtiStifClients;

//**********************************************************************************
// HELPER FUNCTIONS/CLASSES
//
//**********************************************************************************

//**********************************************************************************
// HtiStifMsg
//**********************************************************************************
class HtiStifMsg
{
public:
	HtiStifMsg(BYTE command);

	BYTE* GetMsgBody() { return msgBody; };
	int GetMsgLen() { return msgBodyLen; };

	void AddString(char* string);
	void AddIntAsString(int value);
	void AddByte(BYTE byte);
	void AddParameterSeparator();

	int CheckResponseCommandCode(struct soap *soap, BYTE commandCode);

	int SendReceiveMsg(struct soap *soap, int timeout);

private:
	void IncBodySize(int size);
	BYTE *msgBody; // NOTE: deleted when sent or by gateway when received
	int msgBodyLen;
};

HtiStifMsg::HtiStifMsg(BYTE command)
{
	msgBody = new BYTE[1];
	msgBody[0] = command;
	msgBodyLen = 1;
}

void HtiStifMsg::IncBodySize(int size)
{
	BYTE *newMsgBody = new BYTE[msgBodyLen+size];
	memcpy(newMsgBody, msgBody, msgBodyLen);
	delete msgBody;
	msgBody = newMsgBody;
	msgBodyLen += size;
}

void HtiStifMsg::AddString(char* string)
{
	int stringLen = (int) strlen(string);
	int writePos = msgBodyLen;
	IncBodySize(stringLen);
	memcpy(msgBody+writePos, string, stringLen);
}

void HtiStifMsg::AddIntAsString(int value)
{
	char buf[50];
	_itoa(value, buf, 10);
	AddString(buf);
}

void HtiStifMsg::AddByte(BYTE byte)
{
	int writePos = msgBodyLen;
	IncBodySize(1);
	msgBody[writePos] = byte;
}

void HtiStifMsg::AddParameterSeparator()
{
	AddByte('|');
}

int HtiStifMsg::SendReceiveMsg(struct soap* soap, int timeout)
{
	//dump(msgBody, msgBodyLen);

	BYTE commandCode = msgBody[0];

	// Send the message to symbian side
	HtiSoapHandlerInterface* handler = static_cast<HtiSoapHandlerInterface*>(soap->user);
	handler->SendHtiMessage(HTI_STIF_UID, msgBody, msgBodyLen);

	// Clean these up for received HTI message
	delete msgBody;
	msgBody = NULL;
	msgBodyLen = 0;

	while(1)
	{
		if (handler->WaitForHtiMessage(timeout))
		{
			int err = handle_received_message(handler, TRUE);

			// NOTE: this will be destroyed by gateway
			msgBody = (BYTE*) handler->ReceivedHtiMessageBody();
			msgBodyLen = handler->ReceivedHtiMessageBodySize();
			
			// Received a HTI initiated msg
			// Start waiting again
			if( (err == HTI_RECV_ASYNC_CALL_SERVICED) || 
				(err == HTI_RECV_ASYNC_CALL_SERVICE_ERROR) )
			{
                continue;
			}

			if( err == HTI_RECV_ERROR_MESSAGE )
				SetSoapFaultFromReceivedHtiError(soap, msgBody, msgBodyLen);

			return err;
		}
		else
		{
			// timeout
			soap->error = soap_receiver_fault(soap, "HtiGateway", "No response from symbian side");
			return HTI_RECV_TIMEOUT;
		}
	}
}

int HtiStifMsg::CheckResponseCommandCode(struct soap *soap, BYTE commandCode)
{
	int result = SOAP_OK;
	
	if( msgBody[0] != commandCode )
	{
		char msg[] = "incorrect response CommandCode (expected 0x%x, got 0x%x)";
		int msgLen = _scprintf(msg, commandCode, msgBody[0])+1; // +1 for nul char
		char *buf = new char[msgLen];
		sprintf(buf, msg, commandCode, msgBody[0]);
		soap->error = soap_receiver_fault(soap, "HtiError", buf);
		delete buf;
		result = SOAP_FAULT;
	}

	return result;
}

//**********************************************************************************
// HtiStifClients
//**********************************************************************************
class HtiStifClients
{
public:
	HtiStifClients() {m_size = 0; m_clients = NULL;};
	~HtiStifClients();
	int Add(char* host);
	int Remove(char* host);
	int NrOfClients() {return m_size;};
	const char* Get(int index) {return m_clients[index];};
private:
	int m_size;
	char **m_clients;
};

HtiStifClients::~HtiStifClients()
{
	for(int i=0; i<m_size; i++)
		delete m_clients[i];
	free(m_clients);
	m_size = 0;
}

int HtiStifClients::Add(char* host)
{
	// check if host already exist
	for(int i=0; i<m_size; i++)
	{
		if(!strcmp(m_clients[i], host))
			return -1;
	}

	// alloc space for new pointer array
	char **new_clients = (char**) malloc(sizeof(char*)*(m_size+1));
	
	// copy data
	for(int i=0; i<m_size; i++)
		new_clients[i] = m_clients[i];

	// free previous pointer array & set new one
	free(m_clients);
	m_clients = new_clients;

	// alloc space for host
	char* buf = new char[strlen(host)+1]; // +1 for nul
	strcpy(buf, host);
	new_clients[m_size] = buf;

	// update size
	m_size += 1;

	return 0;
}

int HtiStifClients::Remove(char* host)
{
	int err = -1;

	// alloc a smaller pointer array
	char **new_clients = (char**) malloc(sizeof(char*)*(m_size-1));
	
	// copy data except removed
	int j=0;
	for(int i=0; i<m_size; i++)
	{
		if(!strcmp(m_clients[i],host))
		{
			delete m_clients[i]; // delete removed host
			err = 0;
			continue;
		}

		new_clients[j] = m_clients[i];
		j++;
	}

	// did not find host?
	if(err != 0)
	{
		free(new_clients);
		return err;
	}

	// free previous pointer array & set new one
	free(m_clients);
	m_clients = new_clients;

	// update size
	m_size -= 1;

	return err;
}



//**********************************************************************************
// HtiStifSenderDll
//**********************************************************************************
class HtiStifSenderDll : public CDLLModule
{
public:

	DECLARE_DLL_FUNCTION(int, __cdecl,
						soap_send_ns1__stifMessage, (soap *, const char *, const char *, char *, void *))
	DECLARE_DLL_FUNCTION(int, __cdecl,
						soap_send_ns1__stifResult, (soap *, const char *, const char *, ns1__HtiStifResult, void *))

    BEGIN_DLL_INIT()
		INIT_DLL_FUNCTION(int, __cdecl,
			 soap_send_ns1__stifMessage, (soap *, const char *, const char *, char *, void *), "soap_send_ns1__stifMessage")
		INIT_DLL_FUNCTION(int, __cdecl,
			 soap_send_ns1__stifResult, (soap *, const char *, const char *, ns1__HtiStifResult, void *), "soap_send_ns1__stifResult")
	END_DLL_INIT()
};

void SetSoapFaultFromReceivedHtiError(struct soap *soap, BYTE *receivedMsgBody, int receivedMsgBodyLen)
{
	if( receivedMsgBodyLen == 10 )
	{
		// This is a standard error message
		// (eg. not authenticated)
		HtiSoapHandlerInterface* handler =
			static_cast<HtiSoapHandlerInterface*>(soap->user);
		handler->SendSoapFaultFromReceivedHtiError();
		return;
	}

	// Get error codes
	int frameworkErrorCode = *((BYTE*)(receivedMsgBody + 1));
	int serviceErrorCode = *((DWORD*)(receivedMsgBody + 2));

	// Get error description
	// NOTE: first byte is skipped because it contains the command code
	int serviceErrorDescLen = receivedMsgBodyLen - 11;
	char* serviceErrorDesc = new char[receivedMsgBodyLen - 11 + 1];
	memcpy(serviceErrorDesc, receivedMsgBody+11, serviceErrorDescLen);
	serviceErrorDesc[serviceErrorDescLen] = 0x0;

	// Fill the xml struct
	std::stringstream s;
	s<<"<htiError xmlns=\'urn:hti/fault\'><frameworkErrorCode>";
	s<<frameworkErrorCode;
	s<<"</frameworkErrorCode><serviceErrorCode>";
	s<<serviceErrorCode;
	s<<"</serviceErrorCode><serviceErrorDescription>";
	s<<serviceErrorDesc;
    s<<"</serviceErrorDescription>";
	s<<"</htiError>";
	
	soap->error = soap_receiver_fault(soap, "HtiError", s.str().c_str() );

	delete serviceErrorDesc;
}

//**********************************************************************************
// handle_received_message
//**********************************************************************************
int handle_received_message(HtiSoapHandlerInterface* handler, BOOL isSoapCallActive)
{
	if(handler->IsReceivedHtiError())
		return HTI_RECV_ERROR_MESSAGE;

	// Get message
	BYTE *msgBody = (BYTE*) handler->ReceivedHtiMessageBody();
	int msgBodyLen = handler->ReceivedHtiMessageBodySize();

	switch(msgBody[0])
	{
	// Initiated by SOAP
	case CMD_STIF_OPEN:
	case CMD_STIF_CLOSE:		
	case CMD_STIF_LOAD_MODULE:
	case CMD_STIF_UNLOAD_MODULE:
	case CMD_STIF_LIST_CASES:
	case CMD_STIF_START_CASE:
	case CMD_STIF_CANCEL_CASE:
	case CMD_STIF_PAUSE_CASE:
	case CMD_STIF_RESUME_CASE:
	case CMD_STIF_ADD_CASE_FILE:
	case CMD_STIF_REMOVE_CASE_FILE:
	case CMD_STIF_CASE_MESSAGE:
	case CMD_STIF_SET_DEVICEID:
	case CMD_STIF_SET_ATTRIBUTE:
		if(isSoapCallActive)
			return HTI_RECV_OK;
		else
		{
			printf("HtiStif: Received a known but unexpected message (command code 0x%x)\n", msgBody[0]);
			return HTI_RECV_ROUGUE_MESSAGE;
		}
        break;

	// Initiated by HTI
	case CMD_STIF_TEST_COMPLETED:
		{
			// Make copy of parameters
			char *params = new char[msgBodyLen];
			memcpy(params, msgBody+1, msgBodyLen-1);
			params[msgBodyLen-1] = 0x0; // add nul to make strtok easier
			
			// Get data from params
			char *testCaseId = strtok(params, "|");
			char *testResult = strtok(NULL, "|");
			char *testExecutionResult = strtok(NULL, "|");
			char *resultDescription = strtok(NULL, "|");

			// Check if all is there
			if( !testCaseId || !testResult || !testExecutionResult)
			{
				printf("HtiStif: Received incomplete testcase result\n" );
				delete params;
				return HTI_RECV_ASYNC_CALL_SERVICE_ERROR;
			}
			
			// Get soap env
			struct soap soap;
			soap_init(&soap);
			soap.namespaces = namespaces;

			// Fill HtiStifResult
			struct ns1__HtiStifResult result;
			result.caseId = atoi(testCaseId);
			result.caseResult = atoi(testResult);
			result.executionResult = atoi(testExecutionResult);
			if(resultDescription)
			{
				result.resultDescription = (char*) soap_malloc( &soap, strlen(resultDescription)+1 );
				strcpy(result.resultDescription, resultDescription);
			}
			else
				result.resultDescription = NULL;
			
			delete params; // not needed anymore
			
			// Send the HtiStifResult to registered clients
			for(int i=0; i<g_HtiStifClients->NrOfClients(); i++)
			{
				if(g_HtiStifSenderDll->soap_send_ns1__stifResult(&soap, g_HtiStifClients->Get(i), soapAction, result, NULL) != SOAP_OK)
					printf("HtiStif: error sending stifResult to %s\n", g_HtiStifClients->Get(i) );
			}
			soap_destroy(&soap);
			soap_end(&soap);
			soap_done(&soap);

			return HTI_RECV_ASYNC_CALL_SERVICED;
		}
		break;

	case CMD_STIF_ATS_MESSAGE:
		{
			// Get soap env
			struct soap soap;
			soap_init(&soap);
			soap.namespaces = namespaces;

			// Get message
			// NOTE: messages are in string format so they don't have nul-characters
			// This might change in the future and then xsd__string will no do for this task.
			char* msg = (char*) soap_malloc(&soap, msgBodyLen);
			memcpy(msg, msgBody+1, msgBodyLen-1);
			msg[msgBodyLen-1] = 0x0; // add nul

			// Send the HtiStifResult to registered clients
			for(int i=0; i<g_HtiStifClients->NrOfClients(); i++)
			{
				if(g_HtiStifSenderDll->soap_send_ns1__stifMessage(&soap, g_HtiStifClients->Get(i), soapAction, msg, NULL) != SOAP_OK)
					printf("HtiStif: error sending stifMessage to %s\n", g_HtiStifClients->Get(i) );
			}
			soap_destroy(&soap);
			soap_end(&soap);
			soap_done(&soap);
			
			return HTI_RECV_ASYNC_CALL_SERVICED;
		}
		break;


	default:
		printf("HtiStif: Received an unkown message (command code 0x%x, sync call active %d)\n", msgBody[0], isSoapCallActive);
		return HTI_RECV_UNKOWN_MESSAGE;
		break;
	}

	// Should never come here
	return HTI_RECV_GEN_ERROR;
}



//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************

//**********************************************************************************
// ns1__openStif()
//**********************************************************************************
int ns1__openStif(struct soap* soap, char *iniFile, struct ns1__openStifResponse *out)
{
	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_OPEN);
	if(iniFile != NULL)
	{
		if(strlen(iniFile) != 0)
			msg.AddString(iniFile);
	}

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_OPEN) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__closeStif()
//**********************************************************************************
int ns1__closeStif(struct soap* soap, void *_, struct ns1__closeStifResponse *out)
{
	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_CLOSE);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_CLOSE) )
		return SOAP_FAULT;

	return SOAP_OK;
}


//**********************************************************************************
// ns1__loadStifTestModule()
//**********************************************************************************
int ns1__loadStifTestModule(struct soap* soap, char *moduleName, char *iniFile, char *&moduleNameLoaded)
{
	// Parameter check
	if(check_mandatory_string_parameter(soap, moduleName, "moduleName"))
		return SOAP_FAULT;

	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_LOAD_MODULE);
	msg.AddString(moduleName);
	if(iniFile != NULL)
	{
		if(strlen(iniFile) != 0)
		{
			msg.AddParameterSeparator();
			msg.AddString(iniFile);
		}
	}

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check CommandCode
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_LOAD_MODULE) )
		return SOAP_FAULT;

	int moduleNameLoadedLen = msg.GetMsgLen()-1; // remaining msgBody
	moduleNameLoaded = (char*) soap_malloc(soap, moduleNameLoadedLen+1); // +1 for nul char
	memcpy(moduleNameLoaded, msg.GetMsgBody()+1, moduleNameLoadedLen);
	moduleNameLoaded[moduleNameLoadedLen] = 0x0; // add nul char
	
	return SOAP_OK;
}

//**********************************************************************************
// ns1__unloadStifTestModule()
//**********************************************************************************
int ns1__unloadStifTestModule(struct soap* soap, char *moduleName, char *&moduleNameUnloaded)
{
	// Parameter check
	if(check_mandatory_string_parameter(soap, moduleName, "moduleName"))
		return SOAP_FAULT;

		// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_UNLOAD_MODULE);
	msg.AddString(moduleName);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check CommandCode
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_UNLOAD_MODULE) )
		return SOAP_FAULT;

	int moduleNameUnloadedLen = msg.GetMsgLen()-1; // remaining msgBody
	moduleNameUnloaded = (char*) soap_malloc(soap, moduleNameUnloadedLen+1); // +1 for nul char
	memcpy(moduleNameUnloaded, msg.GetMsgBody()+1, moduleNameUnloadedLen);
	moduleNameUnloaded[moduleNameUnloadedLen] = 0x0; // add nul char

	return SOAP_OK;
}

//**********************************************************************************
// ns1__listStifTestCases()
//**********************************************************************************
int ns1__listStifTestCases(struct soap* soap, char *moduleName, struct ArrayOfTestCases *testCases)
{
	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_LIST_CASES);
	if(moduleName != NULL)
	{
		if(strlen(moduleName) != 0)
			msg.AddString(moduleName);
	}

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check CommandCode
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_LIST_CASES) )
		return SOAP_FAULT;

	// No testcases found
	if(msg.GetMsgLen() <= 1)
		return SOAP_OK;

	// make a copy of the list
	char* testCaseTitleList = new char[msg.GetMsgLen()];
	memcpy(testCaseTitleList, msg.GetMsgBody()+1, msg.GetMsgLen());
	testCaseTitleList[msg.GetMsgLen()-1] = 0x0; //add nul to make life easier

	// Get number of testcases
	char* testcase = strtok( testCaseTitleList, "|" );

	// Only one testcase ?
	if(testcase == NULL)
	{
		testCases->__size = 1;
		testCases->__ptr = (char**) soap_malloc(soap, sizeof(char**));
		testCases->__ptr[0] = (char*) soap_malloc(soap, msg.GetMsgLen());
		memcpy(testCases->__ptr[0], testCaseTitleList, msg.GetMsgLen()); // (remember we added nul)	
		delete testCaseTitleList;
		return SOAP_OK;
	}

	// ..no! we have at least two
	while(testcase != NULL)
	{
		testCases->__size++;
		testcase = strtok( NULL, "|" ); // continue with testCaseTitleList
	}
	
	// Now that we know the number of testcases title we can alloc soap space for 
	// pointers and copy testcase titles to soap space.
	testCases->__ptr = (char**) soap_malloc(soap, sizeof(char**)*testCases->__size);

	// reset testCaseTitleList from previous strtok
	memcpy(testCaseTitleList, msg.GetMsgBody()+1, msg.GetMsgLen());
	testCaseTitleList[msg.GetMsgLen()-1] = 0x0; //add nul to make life easier

	int i = 0;
	testcase = strtok( testCaseTitleList, "|" );
	while(testcase != NULL)
	{
		int testCaseLen = (int) strlen(testcase);
		testCases->__ptr[i] = (char*) soap_malloc(soap, testCaseLen+1);
		memcpy(testCases->__ptr[i], testcase, testCaseLen);
		testCases->__ptr[i][testCaseLen] = 0x0; // add nul char
		i++;
		testcase = strtok( NULL, "|" );
	}
	delete testCaseTitleList;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__startStifTestCase()
//**********************************************************************************
int ns1__startStifTestCase(struct soap* soap, char *moduleName, int testCaseIndex, int &testCaseId)
{
	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_START_CASE);

	if(moduleName != NULL)
	{
		if(strlen(moduleName) != 0)
		{
			msg.AddString(moduleName);
			msg.AddParameterSeparator();
		}
	}
    msg.AddIntAsString(testCaseIndex);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_START_CASE) )
		return SOAP_FAULT;

	// Get testCaseId
	char* tmp = new char[msg.GetMsgLen()];
	memcpy(tmp, msg.GetMsgBody()+1, msg.GetMsgLen()-1);
	tmp[msg.GetMsgLen()-1] = 0x0; // add nul
	testCaseId = atoi(tmp);

	return SOAP_OK;
}

//**********************************************************************************
// ns1__cancelStifTestCase()
//**********************************************************************************
int ns1__cancelStifTestCase(struct soap* soap, int testCaseId, struct ns1__cancelStifTestCaseResponse *out)
{
	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_CANCEL_CASE);
	msg.AddIntAsString(testCaseId);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_CANCEL_CASE) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__pauseStifTestCase()
//**********************************************************************************
int ns1__pauseStifTestCase(struct soap* soap, int testCaseId, struct ns1__pauseStifTestCaseResponse *out)
{
	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_PAUSE_CASE);
	msg.AddIntAsString(testCaseId);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_PAUSE_CASE) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__resumeStifTestCase()
//**********************************************************************************
int ns1__resumeStifTestCase(struct soap* soap, int testCaseId, struct ns1__resumeStifTestCaseResponse *out)
{
	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_RESUME_CASE);
	msg.AddIntAsString(testCaseId);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_RESUME_CASE) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__addStifTestCaseFile()
//**********************************************************************************
int ns1__addStifTestCaseFile(struct soap* soap, char *moduleName, char *testCaseFile, struct ns1__addStifTestCaseFileResponse *out)
{
	// Parameter check
	if( check_mandatory_string_parameter(soap, moduleName, "moduleName") ||
		check_mandatory_string_parameter(soap, testCaseFile, "testCaseFile") )
		return SOAP_FAULT;

	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_ADD_CASE_FILE);
	msg.AddString(moduleName);
	msg.AddParameterSeparator();
	msg.AddString(testCaseFile);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_ADD_CASE_FILE) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__removeStifTestCaseFile()
//**********************************************************************************
int ns1__removeStifTestCaseFile(struct soap* soap, char *moduleName, char *testCaseFile, struct ns1__removeStifTestCaseFileResponse *out)
{
	// Parameter check
	if( check_mandatory_string_parameter(soap, moduleName, "moduleName") ||
		check_mandatory_string_parameter(soap, testCaseFile, "testCaseFile") )
		return SOAP_FAULT;

	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_REMOVE_CASE_FILE);
	msg.AddString(moduleName);
	msg.AddParameterSeparator();
	msg.AddString(testCaseFile);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_REMOVE_CASE_FILE) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__setDeviceId()
//**********************************************************************************
int ns1__setDeviceId(struct soap* soap, int deviceId, struct ns1__setDeviceIdResponse *out)
{
	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_SET_DEVICEID);
	msg.AddIntAsString(deviceId);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_SET_DEVICEID) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__queryStifTestCaseResult()
//**********************************************************************************
int ns1__queryStifTestCaseResult(struct soap* soap, int testCaseId, queryStifTestCaseResultResponse &r)
{
	soap->error = soap_receiver_fault(soap, "HtiGateway", "queryStifTestCaseResult: not supported");
	return SOAP_FAULT;
}

//**********************************************************************************
// ns1__runStifTestCase()
//**********************************************************************************
int ns1__runStifTestCase(struct soap* soap, char *moduleName, int testCaseIndex, struct runStifTestCaseResponse &r)
{
	soap->error = soap_receiver_fault(soap, "HtiGateway", "runStifTestCase: not supported");
	return SOAP_FAULT;
}

//**********************************************************************************
// ns1__register()
//**********************************************************************************
int ns1__register(struct soap* soap, char *host, struct ns1__registerResponse *out)
{
	if(check_mandatory_string_parameter(soap, host, "host"))
		return SOAP_FAULT;

	if(g_HtiStifClients->Add(host) != 0)
	{
		soap->error = soap_receiver_fault(soap, "HtiGateway", "register: host already registered");
		return SOAP_FAULT;
	}

	return SOAP_OK;
}

//**********************************************************************************
// ns1__deregister()
//**********************************************************************************
int ns1__deregister(struct soap* soap, char *host, struct ns1__deregisterResponse *out)
{
	if(check_mandatory_string_parameter(soap, host, "host"))
		return SOAP_FAULT;

	if(g_HtiStifClients->Remove(host) != 0)
	{
		soap->error = soap_receiver_fault(soap, "HtiGateway", "deregister: did not find host");
        return SOAP_FAULT;
	}

	return SOAP_OK;
}

//**********************************************************************************
// ns1__stifMessage()
//**********************************************************************************
int ns1__stifMessage(struct soap* soap, char *message, struct ns1__stifMessageResponse *out)
{
	// Parameter check
	if( check_mandatory_string_parameter(soap, message, "message") )
		return SOAP_FAULT;

	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_CASE_MESSAGE);
	msg.AddString(message);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_CASE_MESSAGE) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__setAttribute()
//**********************************************************************************
int ns1__setAttribute(struct soap* soap, 
					  char *attribute, 
					  char *value, 
                      struct ns1__SetAttributeResponse *out)
{
	// Parameter check
	if( check_mandatory_string_parameter(soap, attribute, "attribute") ||
		check_mandatory_string_parameter(soap, value, "value") )
		return SOAP_FAULT;

	// Assemble HTI message
	HtiStifMsg msg(CMD_STIF_SET_ATTRIBUTE);
	msg.AddString(attribute);
	msg.AddParameterSeparator();
	msg.AddString(value);

	// Send msg and receive response
	if( msg.SendReceiveMsg(soap, TIMEOUT_10_SECONDS) != ERROR_SUCCESS )
		return SOAP_FAULT;

	// Check response command code
	if( msg.CheckResponseCommandCode(soap, CMD_STIF_SET_ATTRIBUTE) )
		return SOAP_FAULT;

	return SOAP_OK;
}


//**********************************************************************************
// EXPORTED FUNCTIONS
//
//**********************************************************************************

//**********************************************************************************
// DllMain
//**********************************************************************************
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:

			g_HtiStifClients = new HtiStifClients;
			
			// Load HtiStifSender
			g_HtiStifSenderDll = new HtiStifSenderDll;
			if( g_HtiStifSenderDll->Init("serviceplugins/HtiStifSender.dll") != TRUE)
			{
				printf("HtiStif: could not load HtiStifSender.dll");
				return FALSE;
			}
			
			// Check exported functions
			if( !g_HtiStifSenderDll->soap_send_ns1__stifMessage )
			{
				printf("HtiStif: HtiStifSender.dll - exported function soap_send_ns1__stifMessage() not found");
				return FALSE;
			}

			if( !g_HtiStifSenderDll->soap_send_ns1__stifResult )
			{
				printf("HtiStif: HtiStifSender.dll - exported function soap_send_ns1__stifResult() not found");
				return FALSE;
			}
	        break;

        case DLL_PROCESS_DETACH:

			// Delete HtiStifSender
 			delete g_HtiStifSenderDll;

			delete g_HtiStifClients;

            break;
    }
    return TRUE;
}

//**********************************************************************************
// hti_serve
//**********************************************************************************
int hti_serve(HtiSoapHandlerInterface* handler)
{
	return handle_received_message(handler, FALSE);
}




