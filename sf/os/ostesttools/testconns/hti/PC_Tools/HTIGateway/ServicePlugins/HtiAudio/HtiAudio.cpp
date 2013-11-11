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
#include "HtiAudioH.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"

#include <iosfwd>
#include <sstream>

// Command codes
const unsigned char CMD_LIST_AUDIOFILES	= 0x01;
const unsigned char CMD_PLAY_FILE		= 0x02;
const unsigned char CMD_PLAY_TONE		= 0x03;
const unsigned char CMD_PLAY_DTMF		= 0x04;
const unsigned char CMD_STOP			= 0x05;
const unsigned char CMD_GET_DURATION	= 0x06;
const unsigned char CMD_GET_MAXVOLUME	= 0x07;
const unsigned char CMD_SET_VOLUME		= 0x08;

// enums
enum PlayState {
	EStopped,
	EPlaying,
	EError
};

enum PlayCommand {
	EPlayAudioFile = 0,
	EPlayTone,
	EPlayDtmf,
	ENrOfPlayCommands
};

// global variable(s)
int g_PlayCommandState[ENrOfPlayCommands] = { EStopped, EStopped, EStopped };


// predeclarations
bool handle_async_msg(HtiSoapHandlerInterface* handler);
void SetSoapFaultFromReceivedHtiAudioError(struct soap *soap,
										   BYTE *receivedMsgBody,
										   int receivedMsgBodyLen);

//**********************************************************************************
// HELPER CLASSES
//
//**********************************************************************************

//**********************************************************************************
// CLASS HtiAudioMsg
//**********************************************************************************
class HtiAudioMsg : public HtiMsgHelper
{
public:
	HtiAudioMsg( struct soap *soap );
	HtiAudioMsg( struct soap *soap, DWORD serviceId, BYTE command );
	int ReceiveMsgWithAsyncHandler( int timeout );
	int SendReceiveMsgWithAsyncHandler( int timeout );

public: // from HtiMsgHelper
	// We need to use SetSoapFaultFromReceivedHtiAudioError handler
	int ReceiveMsg( int timeout );
};

//**********************************************************************************
// HtiAudioMsg::HtiAudioMsg
//**********************************************************************************
HtiAudioMsg::HtiAudioMsg( struct soap *soap ) : HtiMsgHelper( soap )
{
}

//**********************************************************************************
// HtiAudioMsg::HtiAudioMsg
//**********************************************************************************
HtiAudioMsg::HtiAudioMsg( struct soap *soap, DWORD serviceId, BYTE command )
                          : HtiMsgHelper( soap, serviceId, command )
{
}

//**********************************************************************************
// HtiAudioMsg::ReceiveMsg
//**********************************************************************************
int HtiAudioMsg::ReceiveMsg( int timeout )
{
	// Clean these up for received HTI message
	if(m_msgBody)
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
		// (handler has ownership of the message body)
		m_msgBody = (BYTE*) handler->ReceivedHtiMessageBody();
		m_msgBodyLen = handler->ReceivedHtiMessageBodySize();

		if ( !handler->IsReceivedHtiError() )
		{
			// Get received message
			return SOAP_OK;
		}
		else
		{
			/// Fill the error description
			SetSoapFaultFromReceivedHtiAudioError(
					m_soap, m_msgBody, m_msgBodyLen);
				return SOAP_FAULT;
		}
	}
	// ...or timeout
	else
	{
		m_soap->error = soap_receiver_fault(m_soap,
			"HtiGateway", "No response from symbian side");
		return SOAP_FAULT;
	}
}

//**********************************************************************************
// HtiAudioMsg::SendReceiveMsg
//**********************************************************************************
int HtiAudioMsg::SendReceiveMsgWithAsyncHandler( int timeout )
{
	SendMsg();
	return ReceiveMsgWithAsyncHandler( timeout );
}

//**********************************************************************************
// HtiAudioMsg::ReceiveMsgWithAsyncHandler
//**********************************************************************************
int HtiAudioMsg::ReceiveMsgWithAsyncHandler( int timeout )
{
	// Clean these up for to be received HTI message
	if( m_msgBody )
	{
		delete m_msgBody;
		m_msgBody = NULL;
		m_msgBodyLen = 0;
	}

	HtiSoapHandlerInterface* handler =
		static_cast<HtiSoapHandlerInterface*>( m_soap->user );

	while(1)
	{
		if ( handler->WaitForHtiMessage( timeout ) )
		{
			// NOTE: this will be destroyed by gateway
			m_msgBody = (BYTE*) handler->ReceivedHtiMessageBody();
			m_msgBodyLen = handler->ReceivedHtiMessageBodySize();

			if ( handle_async_msg( handler ) )
				continue; // async msg received wait for next msg
			else
			{
				// error msg received ?
				if( handler->IsReceivedHtiError() )
				{
					SetSoapFaultFromReceivedHtiAudioError(
						m_soap, m_msgBody, m_msgBodyLen );
					return SOAP_FAULT;
				}
				return SOAP_OK;
			}
		}
		else
		{
			// timeout
			m_soap->error = soap_receiver_fault(m_soap,
				"HtiGateway", "No response from symbian side");
			return SOAP_FAULT;
		}
	}
}


//**********************************************************************************
// HELPER FUNCTIONS
//
//**********************************************************************************

//**********************************************************************************
// isAudioPlaying
//**********************************************************************************
bool isAudioPlaying(struct soap *soap)
{
	if( (g_PlayCommandState[EPlayAudioFile] == EPlaying) ||
	    (g_PlayCommandState[EPlayTone] == EPlaying) ||
	    (g_PlayCommandState[EPlayDtmf] == EPlaying) )
	{
		soap->error = soap_receiver_fault(soap,
			"HtiGateway", "already playing audio");
		return TRUE;
	}
	return FALSE;
}

//**********************************************************************************
// SetSoapFaultFromReceivedHtiAudioError
//**********************************************************************************
void SetSoapFaultFromReceivedHtiAudioError(struct soap *soap,
										   BYTE *receivedMsgBody,
										   int receivedMsgBodyLen)
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
// handle_async_msg
// Will return TRUE if message is handled
//**********************************************************************************
bool handle_async_msg( HtiSoapHandlerInterface* handler )
{
	// Get message
	BYTE *msgBody = (BYTE*) handler->ReceivedHtiMessageBody();
	int msgBodyLen = handler->ReceivedHtiMessageBodySize();

	if(handler->IsReceivedHtiError())
	{
		// First byte of service error description is the command it concers
		int serviceErrorDesPos = 1+1+4+4;
		switch(msgBody[serviceErrorDesPos])
		{
		case CMD_PLAY_FILE:
			g_PlayCommandState[EPlayAudioFile] = EError;
			return TRUE;

		case CMD_PLAY_TONE:
			g_PlayCommandState[EPlayTone] = EError;
			return TRUE;

		case CMD_PLAY_DTMF:
			g_PlayCommandState[EPlayDtmf] = EError;
			return TRUE;

		default:
			return FALSE;
		}
	}

	switch(msgBody[0])
	{
	// Received from symbian side to indicate playing is complete
	case CMD_PLAY_FILE:
		g_PlayCommandState[EPlayAudioFile] = EStopped;
		return TRUE;

	case CMD_PLAY_TONE:
		g_PlayCommandState[EPlayTone] = EStopped;
		return TRUE;

	case CMD_PLAY_DTMF:
		g_PlayCommandState[EPlayDtmf] = EStopped;
		return TRUE;

	default:
		return FALSE;
	}

	// Should never come here
	return FALSE;
}

//**********************************************************************************
// EXPORTED FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// hti_serve()
//**********************************************************************************
int hti_serve(HtiSoapHandlerInterface* handler)
{
	handle_async_msg( handler );
	return 0;
}

//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// ns1__listAudioFiles()
//**********************************************************************************
int ns1__listAudioFiles(struct soap* soap,
						char *directory, // optional
                        struct ArrayOfAudioFiles *audiofiles)
{
	// construct & send & receive HTI message
	HtiAudioMsg msg( soap, HTI_AUDIO_UID, CMD_LIST_AUDIOFILES );
	msg.AddStringWithLengthByte( directory );
	if ( msg.SendReceiveMsgWithAsyncHandler( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	// Check CommandCode
	if ( msg.CheckCommandCode( CMD_LIST_AUDIOFILES ) )
		return SOAP_FAULT;

	// get size of array & alloc space for pointers
	audiofiles->__size  = (int) msg.GetWord(1);
	audiofiles->__ptr = (char**) soap_malloc(soap, sizeof(char**)*audiofiles->__size);

	// read files
	int offset = 3;
	for ( int i = 0; i < audiofiles->__size; i++ )
	{
		int fileNameLen = msg.GetByte(offset);
		offset += 1;
		audiofiles->__ptr[i] = (char*) soap_malloc( soap, fileNameLen+1 ); // +1 for nul
		memcpy( audiofiles->__ptr[i], msg.GetMsgBody()+offset, fileNameLen );
		( audiofiles->__ptr[i] )[fileNameLen] = 0x0; // add nul
		offset += fileNameLen;
	}

	return SOAP_OK;
}

//**********************************************************************************
// ns1__startPlayAudioFile()
//**********************************************************************************
int ns1__startPlayAudioFile(struct soap* soap,
							char *fileName,
							unsigned char volume,
							int startPosition,
							int stopPosition,
							unsigned char nrOfRepeats,
							int silenceBetweenRepeats,
							char *audioSettings,
                            struct ns1__startPlayAudioFileResponse *out)
{
	if( check_mandatory_string_parameter(soap, fileName, "fileName") ||
		check_mandatory_string_parameter(soap, audioSettings, "audioSettings") )
		return SOAP_FAULT;

	if( isAudioPlaying( soap ) )
		return SOAP_FAULT;

	// construct & send HTI message
	HtiAudioMsg msg( soap, HTI_AUDIO_UID, CMD_PLAY_FILE );
	msg.AddStringWithLengthByte( fileName );
	msg.AddByte( volume );
	msg.AddInt( startPosition );
	msg.AddInt( stopPosition );
	msg.AddByte( nrOfRepeats );
	msg.AddInt( silenceBetweenRepeats );
	if(!strcmp("Default", audioSettings))
		msg.AddByte( 0x00 );
	else if(!strcmp("General Music", audioSettings))
		msg.AddByte( 0x01 );
	else if(!strcmp("Ring Tone Preview", audioSettings))
		msg.AddByte( 0x02 );
	else
	{
		soap->error = soap_receiver_fault(soap, "HtiGateway", "unknown audio setting");
		return SOAP_FAULT;
	}
	msg.SendMsg();

	g_PlayCommandState[EPlayAudioFile] = EPlaying;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__playAudioFile()
//**********************************************************************************
int ns1__playAudioFile(struct soap* soap,
					   char *fileName,
					   unsigned char volume,
					   int startPosition,
					   int stopPosition,
					   unsigned char nrOfRepeats,
					   int silenceBetweenRepeats,
					   char *audioSettings,
					   int timeout,
                       struct ns1__playAudioFileResponse *out)
{
	// Start playing
	if( ns1__startPlayAudioFile(soap,
		                        fileName,
                                volume,
    	                        startPosition,
	     					    stopPosition,
		     				    nrOfRepeats,
		 	    			    silenceBetweenRepeats,
		 		    		    audioSettings,
		 					    NULL) )
	{
		return SOAP_FAULT;
	}

	// Set stopped immidietly as this is synchronous call
	g_PlayCommandState[EPlayAudioFile] = EStopped;

	// get response
	HtiAudioMsg msg( soap );
	if ( msg.ReceiveMsg( timeout ) )
		return SOAP_FAULT;

	return msg.CheckCommandCode( CMD_PLAY_FILE );
}

//**********************************************************************************
// ns1__startPlayTone()
//**********************************************************************************
int ns1__startPlayTone(struct soap* soap,
					   unsigned short frequency,
					   int duration,
					   unsigned char volume,
					   unsigned char nrOfRepeats,
					   int silenceBetweenRepeats,
                       struct ns1__startPlayToneResponse *out)
{
	if( isAudioPlaying( soap ) )
		return SOAP_FAULT;

	HtiAudioMsg msg( soap, HTI_AUDIO_UID, CMD_PLAY_TONE );
	msg.AddWord( frequency );
	msg.AddInt( duration );
	msg.AddByte( volume );
	msg.AddByte( nrOfRepeats );
	msg.AddInt( silenceBetweenRepeats );
	msg.SendMsg();

	g_PlayCommandState[EPlayTone] = EPlaying;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__playTone()
//**********************************************************************************
int ns1__playTone(struct soap* soap,
				  unsigned short frequency,
				  int duration,
				  unsigned char volume,
				  unsigned char nrOfRepeats,
				  int silenceBetweenRepeats,
                  struct ns1__playToneResponse *out)
{
	// Start playing
	if(ns1__startPlayTone(soap,
		                  frequency,
						  duration,
						  volume,nrOfRepeats,
						  silenceBetweenRepeats,
						  NULL))
		return SOAP_FAULT;

	// Calculate timeout (in milliseconds)
	int timeout = duration/1000 +
		          duration/1000*nrOfRepeats +
				  silenceBetweenRepeats*nrOfRepeats +
				  HTIMSG_TIMEOUT_10_SECONDS;

	// Set stopped immidietly as this is synchronous call
	g_PlayCommandState[EPlayTone] = EStopped;

	// get response
	HtiAudioMsg msg( soap );
	if ( msg.ReceiveMsg( timeout ) )
		return SOAP_FAULT;

	return msg.CheckCommandCode( CMD_PLAY_TONE );
}

//**********************************************************************************
// ns1__startPlayDtmf()
//**********************************************************************************
int ns1__startPlayDtmf(struct soap* soap,
					   char *dtmfString,
					   int toneLength,
					   int gapLength,
					   unsigned char volume,
					   unsigned char nrOfRepeats,
					   int silenceBetweenRepeats,
                       struct ns1__startPlayDtmfResponse *out)
{
	if(check_mandatory_string_parameter(soap, dtmfString, "dtmfString"))
		return SOAP_FAULT;

	if(isAudioPlaying(soap))
		return SOAP_FAULT;

	HtiAudioMsg msg( soap, HTI_AUDIO_UID, CMD_PLAY_DTMF );
	msg.AddStringWithLengthByte( dtmfString );
	msg.AddInt( toneLength );
	msg.AddInt( gapLength );
	msg.AddByte( volume );
	msg.AddByte( nrOfRepeats );
	msg.AddInt( silenceBetweenRepeats );
	msg.SendMsg();

	g_PlayCommandState[EPlayDtmf] = EPlaying;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__playDtmf()
//**********************************************************************************
int ns1__playDtmf(struct soap* soap,
				  char *dtmfString,
				  int toneLength,
				  int gapLength,
				  unsigned char volume,
				  unsigned char nrOfRepeats,
				  int silenceBetweenRepeats,
                  struct ns1__playDtmfResponse *out)
{
	// Start playing
	if(ns1__startPlayDtmf(soap,
		                  dtmfString,
						  toneLength,
						  gapLength,
						  volume,
						  nrOfRepeats,
						  silenceBetweenRepeats,
						  NULL))
		return SOAP_FAULT;

	// Calculate timeout
	int nrOfTones = (int) strlen(dtmfString);
	int timeout = toneLength/1000*nrOfTones +
		          gapLength/1000*nrOfTones +
				  toneLength/1000*nrOfTones*nrOfRepeats +
		          gapLength/1000*nrOfTones*nrOfRepeats +
				  silenceBetweenRepeats/1000*nrOfRepeats +
				  HTIMSG_TIMEOUT_10_SECONDS;

	// Set stopped immidietly as this is synchronous call
	g_PlayCommandState[EPlayDtmf] = EStopped;

	// get response
	HtiAudioMsg msg( soap );
	if ( msg.ReceiveMsg( timeout ) )
		return SOAP_FAULT;

	return msg.CheckCommandCode( CMD_PLAY_DTMF );
}

//**********************************************************************************
// ns1__getPlayStatus()
//**********************************************************************************
int ns1__getPlayStatus(struct soap* soap,
					   char* type,
					   char* &status)
{
	PlayCommand command;

	if( !strcmp(type, "audiofile") )
		command = EPlayAudioFile;
	else if( !strcmp(type, "tone") )
		command = EPlayTone;
	else if( !strcmp(type, "dtmf") )
		command = EPlayDtmf;
	else
	{
		soap->error = soap_receiver_fault(soap, "HtiGateway", "unknown type");
		return SOAP_FAULT;
	}

	switch(g_PlayCommandState[command])
	{
	case EStopped:
		status = (char*) soap_malloc(soap, strlen("stopped")+1); // +1 for nul char
		strcpy(status, "stopped");
		break;

	case EPlaying:
		status = (char*) soap_malloc(soap, strlen("playing")+1); // +1 for nul char
		strcpy(status, "playing");
		break;

	case EError:
		g_PlayCommandState[command] = EStopped;
		status = (char*) soap_malloc(soap, strlen("error")+1); // +1 for nul char
		strcpy(status, "error");
		break;

	default:
		// should not happen ever
		soap->error = soap_receiver_fault(soap, "HtiGateway", "unknown state");
		return SOAP_FAULT;
	}

	return SOAP_OK;
}

//**********************************************************************************
// ns1__stopPlayback()
//**********************************************************************************
int ns1__stopPlayback(struct soap* soap,
					  void *_,
                      struct ns1__stopPlaybackResponse *out)
{
	for(int i=0; i < ENrOfPlayCommands; i++)
        g_PlayCommandState[i] = EStopped;

	HtiAudioMsg msg( soap, HTI_AUDIO_UID, CMD_STOP );
	if ( msg.SendReceiveMsgWithAsyncHandler( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckCommandCode( CMD_STOP ) )
		return SOAP_FAULT;

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getDuration()
//**********************************************************************************
int ns1__getDuration(struct soap* soap,
					 char *fileName, // optional
					 int &duration)
{
	HtiAudioMsg msg( soap, HTI_AUDIO_UID, CMD_GET_DURATION );
	msg.AddStringWithLengthByte( fileName );
	if ( msg.SendReceiveMsgWithAsyncHandler( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckCommandCode( CMD_GET_DURATION ) )
		return SOAP_FAULT;

	duration = msg.GetInt(1);

	return SOAP_OK;
}

//**********************************************************************************
// ns1__getMaxVolume()
//**********************************************************************************
int ns1__getMaxVolume(struct soap* soap,
					  char *fileName, // optional
					  unsigned char &volume)
{
	HtiAudioMsg msg( soap, HTI_AUDIO_UID, CMD_GET_MAXVOLUME );
	msg.AddStringWithLengthByte( fileName );
	if ( msg.SendReceiveMsgWithAsyncHandler( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckCommandCode( CMD_GET_MAXVOLUME ) )
		return SOAP_FAULT;

	volume = msg.GetByte(1);

	return SOAP_OK;
}

//**********************************************************************************
// ns1__setVolume()
//**********************************************************************************
int ns1__setVolume(struct soap* soap,
				   unsigned char volume,
				   unsigned char &volumeSet)
{
	HtiAudioMsg msg( soap, HTI_AUDIO_UID, CMD_SET_VOLUME );
	msg.AddByte( volume );
	if ( msg.SendReceiveMsgWithAsyncHandler( HTIMSG_TIMEOUT_10_SECONDS ) )
		return SOAP_FAULT;

	if ( msg.CheckCommandCode( CMD_SET_VOLUME ) )
		return SOAP_FAULT;

	volumeSet = msg.GetByte(1);

	return SOAP_OK;
}



