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
#include "HtiFtpH.h"

#include "HtiPlugin.h"
#include <string.h>

#include "HtiSoapHandlerInterface.h"

const unsigned char CMD_FTP_STOR	    = 0x03;
const unsigned char CMD_FTP_RETR	    = 0x05;
const unsigned char CMD_FTP_LIST	    = 0x07;
const unsigned char CMD_FTP_LISTDIR	    = 0x11;
const unsigned char CMD_FTP_LISTSIZE    = 0x13;
const unsigned char CMD_FTP_LISTDRIVES  = 0x15;
const unsigned char CMD_FTP_MKD		    = 0x09;
const unsigned char CMD_FTP_RMD	    	= 0x0B;
const unsigned char CMD_FTP_DELE	    = 0x0D;
const unsigned char CMD_FTP_CANCEL	    = 0x0E;
const unsigned char CMD_FTP_FILESIZE    = 0x0F;

const unsigned char CMD_FTP_SETFORCE	= 0x20;
const unsigned char CMD_FTP_CHECKSUM	= 0x30;
const unsigned char CMD_FTP_CHECKSUM_U	= 0x31;
const unsigned char CMD_FTP_FORMAT      = 0x40;

const unsigned char CMD_FTP_OK		= 0xF0;
const unsigned char CONTROL_PRIORITY = 2;

const int MAX_HTI_MSG_SIZE = 8192;

char fileMimeType[] = "application/octet-stream"; //??

const static int g_ftpHtiTimeoutControl = 10000;
const static int g_ftpHtiTimeoutData = 60*60000;

int sendHtiMessage(struct soap *soap,
				   const char cmd,
				   const wchar_t* arg )
{
	int resultCode = SOAP_OK;
	
	int textLen = 0;
	int bodyLen = 0;
	if ( arg )
	{
		textLen = (int)wcslen( arg );
		if ( textLen > 0xFF )
		{
			soap->error = soap_sender_fault(soap, "Invalid arguments", NULL);
			resultCode = SOAP_FAULT;
			return resultCode;
		}
		bodyLen = 2*textLen + 2;
	}
	else
	{
        bodyLen = 1;
	}

	BYTE* msgBody = new BYTE[ bodyLen ];
	msgBody[0] = cmd;

	if ( arg )
	{
		msgBody[1] = textLen;
		memcpy( msgBody + 2, arg, 2*textLen );
	}

	HtiSoapHandlerInterface* handler = static_cast<HtiSoapHandlerInterface*>(soap->user);

	handler->SendHtiMessage(HTI_UID, msgBody, bodyLen, CONTROL_PRIORITY );

	delete[] msgBody;
	return resultCode;
}

int receiveFtpOk( struct soap *soap, DWORD waitTimeout )
{
	int resultCode = SOAP_OK;
	
	HtiSoapHandlerInterface* handler = static_cast<HtiSoapHandlerInterface*>(soap->user);

	if ( handler->WaitForHtiMessage( waitTimeout ) )
	{
		if ( !handler->IsReceivedHtiError() )
		{
			DWORD bodySize = handler->ReceivedHtiMessageBodySize();
			BYTE* body = (BYTE*)(handler->ReceivedHtiMessageBody());

			if ( ! ( bodySize == 1 && body[0] == CMD_FTP_OK ) )
			{
				soap->error = soap_receiver_fault(soap,
												"Received invalid hti message", NULL);
				resultCode = SOAP_FAULT;
			}
		}
		else
		{
			handler->SendSoapFaultFromReceivedHtiError();
			resultCode = SOAP_FAULT;
		}
	}
	else
	{
		soap->error = soap_receiver_fault(soap,
										  "Failed receive hti message", NULL);
		resultCode = SOAP_FAULT;
	}
	
	return resultCode;
}

int sendHtiMessageAndReceiveOK(struct soap *soap,
							   const char cmd,
							   const wchar_t* arg )
{
	if ( arg == NULL )
	{
		soap->error = soap_sender_fault(soap, "Invalid arguments", NULL);
		return SOAP_FAULT;
	}
	int resultCode = sendHtiMessage( soap, cmd, arg );
	if ( resultCode != SOAP_OK )
	{
		return resultCode;
	}
	
	return receiveFtpOk(soap, g_ftpHtiTimeoutControl);
}

int sendHtiMessageAndReceiveStringArray(struct soap *soap,
										const char cmd,
										const wchar_t* arg,
										struct ArrayOfStrings &array )
{
	if ( arg == NULL )
	{
		soap->error = soap_sender_fault(soap, "Invalid arguments", NULL);
		return SOAP_FAULT;
	}

	int resultCode = SOAP_OK;
	resultCode = sendHtiMessage( soap, cmd, arg );
	if ( resultCode != SOAP_OK )
	{
		return resultCode;
	}
	
	HtiSoapHandlerInterface* handler = static_cast<HtiSoapHandlerInterface*>(soap->user);

	if (handler->WaitForHtiMessage(g_ftpHtiTimeoutData) )
	{
		if ( !handler->IsReceivedHtiError() )
		{
            DWORD bodySize = handler->ReceivedHtiMessageBodySize();
			BYTE* body = (BYTE*)(handler->ReceivedHtiMessageBody());
			//check that body size is valid
			if ( bodySize > 0 )
			{
				//calculate number of elements
				DWORD i = 0;
				int numOfElements = 0;
				while ( i < bodySize )
				{
                    ++numOfElements;
					i += 2*body[i] + 1;
				}

				//printf("strings: %d\n", numOfElements);

				if ( i != bodySize )
				{
					soap->error = soap_receiver_fault(soap,
													"Invalid hti message", NULL);
					resultCode = SOAP_FAULT;
				}
				else
				{
					array.__size = numOfElements;
					array.__ptr = (wchar_t**)soap_malloc(soap, sizeof(wchar_t*)*array.__size );
					int j;
					for( j=0, i=0; j<array.__size; ++j, i += 2*body[i] + 1)
					{
						//printf("%d size=%d ", j, body[i]);
						array.__ptr[j] = (wchar_t*)soap_malloc(soap, 2*body[i] + 2 );
						memcpy(array.__ptr[j],
								body + i + 1,
								2*body[i] );
						(array.__ptr[j])[body[i]] = 0; //null char
						int strLen = (int)wcslen( array.__ptr[j] );

						//wprintf(L">%S<\n", array.__ptr[j] );
						//printf("\n");
					}
				}
			}
			else
			{
				array.__size = 0;
				array.__ptr = 0;
			}
		}
		else
		{
			handler->SendSoapFaultFromReceivedHtiError();
			resultCode = SOAP_FAULT;
		}
	}
	else
	{
		soap->error = soap_receiver_fault(soap,
										  "Failed receive hti message", NULL);
		resultCode = SOAP_FAULT;
	}
	
	return resultCode;
}

int sendHtiMessageAndReceiveHtiFileInfoArray(struct soap *soap,
										const char cmd,
										const wchar_t* arg,
										struct ArrayOfHtiFileInfos &array )
{
	if ( arg == NULL )
	{
		soap->error = soap_sender_fault(soap, "Invalid arguments", NULL);
		return SOAP_FAULT;
	}

	int resultCode = SOAP_OK;
	resultCode = sendHtiMessage( soap, cmd, arg );
	if ( resultCode != SOAP_OK )
	{
		return resultCode;
	}
	
	HtiSoapHandlerInterface* handler =
        static_cast<HtiSoapHandlerInterface*>(soap->user);

	if (handler->WaitForHtiMessage(g_ftpHtiTimeoutData) )
	{
		if ( !handler->IsReceivedHtiError() )
		{
            DWORD bodySize = handler->ReceivedHtiMessageBodySize();
			BYTE* body = (BYTE*)(handler->ReceivedHtiMessageBody());
			//check that body size is valid
			if ( bodySize > 0 )
			{
				//calculate number of elements
				DWORD i = 0;
				int numOfElements = 0;
				while ( i < bodySize )
				{
                    ++numOfElements;
					i += 2*body[i] + 1 + 4;
				}

                if ( i != bodySize )
				{
					soap->error = soap_receiver_fault(soap,
											"Invalid hti message", NULL);
					resultCode = SOAP_FAULT;
				}
				else
				{
					array.__size = numOfElements;
					array.__ptr = (ns1__HtiFileInfo*) soap_malloc(
                            soap, sizeof(ns1__HtiFileInfo)*array.__size );
					int j;
					for( j=0, i=0; j<array.__size; ++j, i += 2*body[i] + 1 + 4)
					{
						array.__ptr[j].fileName =
                            (wchar_t*)soap_malloc(soap, 2*body[i] + 2 );
						memcpy(array.__ptr[j].fileName, body + i + 1, 2*body[i]);
                        (array.__ptr[j].fileName)[body[i]] = 0; //null char
                        // the file size bytes
                        array.__ptr[j].fileSize = *(int*)(body + i + 2*body[i] + 1);
                    }
                }
            }
			else
			{
				array.__size = 0;
				array.__ptr = 0;
			}
		}
		else
		{
			handler->SendSoapFaultFromReceivedHtiError();
			resultCode = SOAP_FAULT;
		}
	}
	else
	{
		soap->error = soap_receiver_fault(soap,
										  "Failed receive hti message", NULL);
		resultCode = SOAP_FAULT;
	}

    return resultCode;
}

int sendHtiMessageAndReceiveHtiDriveInfoArray(struct soap *soap,
                                        const char cmd,
                                        struct ArrayOfHtiDriveInfos &array )
{
	int resultCode = SOAP_OK;
	resultCode = sendHtiMessage( soap, cmd, NULL );
	if ( resultCode != SOAP_OK )
	{
		return resultCode;
	}
	
	HtiSoapHandlerInterface* handler =
        static_cast<HtiSoapHandlerInterface*>(soap->user);

	if (handler->WaitForHtiMessage(g_ftpHtiTimeoutData) )
	{
		if (!handler->IsReceivedHtiError())
		{
			array.__size = 0;
			array.__ptr = 0;
            DWORD bodySize = handler->ReceivedHtiMessageBodySize();
			BYTE* body = (BYTE*)(handler->ReceivedHtiMessageBody());
            int offset = 0;
			//first byte is the number of elements
			int numOfElements = body[offset];
            offset++;

            array.__size = numOfElements;
			array.__ptr = (ns1__HtiDriveInfo*) soap_malloc(
                    soap, sizeof(ns1__HtiDriveInfo)*array.__size );

			int i;
			for(i=0; i<array.__size; ++i)
			{
                int rootPathLength = body[offset];
                offset++;
				array.__ptr[i].rootPath =
                    (wchar_t*)soap_malloc(soap, 2*rootPathLength + 2);
				memcpy(array.__ptr[i].rootPath, body + offset, 2*rootPathLength);
                (array.__ptr[i].rootPath)[rootPathLength] = 0; //null char
                offset += 2*rootPathLength;
                array.__ptr[i].mediaType = (ns1__driveMediaType)body[offset];
                offset++;
                array.__ptr[i].uniqueID = *(int*)(body + offset);
                offset += sizeof(int);
                array.__ptr[i].driveSize = *(ULONG64*)(body + offset);
                offset += sizeof(ULONG64);
                array.__ptr[i].freeSpace = *(ULONG64*)(body + offset);
                offset += sizeof(ULONG64);
                int nameLength = body[offset];
                offset++;
                array.__ptr[i].driveName = 
                    (wchar_t*)soap_malloc(soap, 2*nameLength + 2);
                memcpy(array.__ptr[i].driveName, body + offset, 2*nameLength);
                (array.__ptr[i].driveName)[nameLength] = 0; //null char
                offset += 2*nameLength;
            }
		}
		else
		{
			handler->SendSoapFaultFromReceivedHtiError();
			resultCode = SOAP_FAULT;
		}
	}
	else
	{
		soap->error = soap_receiver_fault(soap,
										  "Failed receive hti message", NULL);
		resultCode = SOAP_FAULT;
	}

    return resultCode;
}

int sendBinary(struct soap *soap, char* data, const DWORD dataSize )
{
	HtiSoapHandlerInterface* handler = static_cast<HtiSoapHandlerInterface*>(soap->user);

	DWORD offset = 0;

    // dataSize == 0 means that uploading empty file - we need to send one
    // empty data message, so needs to go to following while once
	while( offset < dataSize || ( offset == 0 && dataSize == 0 ) )
	{
		handler->SendHtiMessage( HTI_UID,
					data + offset,
					offset + MAX_HTI_MSG_SIZE > dataSize ? dataSize - offset : MAX_HTI_MSG_SIZE
					);
		offset += MAX_HTI_MSG_SIZE;
		//check for error messages
		if (handler->WaitForHtiMessage(0) ) 
		{
			if ( handler->IsReceivedHtiError() )
			{
				handler->SendSoapFaultFromReceivedHtiError();
			}
			else
			{
				soap->error = soap_receiver_fault(soap,
												"Not expected hti message", NULL);
			}
			return SOAP_FAULT;
		}
	}

	return SOAP_OK;
}

int sendSTOR(struct soap *soap,
             const wchar_t* arg,
			 const DWORD fileSize )
{
	int resultCode = SOAP_OK;
	
	int textLen = (int)wcslen( arg );

	if ( !arg || textLen > 0xFF )
	{
		soap->error = soap_sender_fault(soap, "Invalid arguments", NULL);
		resultCode = SOAP_FAULT;
		return resultCode;
	}

	int	bodyLen = 2*textLen + 6;

	BYTE* msgBody = new BYTE[ bodyLen ];
	msgBody[0] = CMD_FTP_STOR;
	*((DWORD*)(msgBody + 1)) = fileSize;
	msgBody[5] = textLen;
	memcpy( msgBody + 6, arg, 2*textLen );

	HtiSoapHandlerInterface* handler = static_cast<HtiSoapHandlerInterface*>(soap->user);

	handler->SendHtiMessage(HTI_UID, msgBody, bodyLen, CONTROL_PRIORITY );

	delete[] msgBody;
	return resultCode;
}

int ns1__putFile(struct soap* soap,
				 struct ns1__HtiSoapAttachment *file,
				 wchar_t *targetPath,
				 struct ns1__putFileResponse *out)
{
	int resultCode;
	//send attachment as several hti ftp data messages
	if ( file == NULL || targetPath == NULL )
	{
		soap->error = soap_sender_fault(soap, "Invalid arguments", NULL);
		return SOAP_FAULT;
	}
	else if ( file->href != NULL )
	{
		for (soap_multipart::iterator attachment = (*soap).dime.begin();
				attachment != (*soap).dime.end(); ++attachment) 
		{
			if ( (*attachment).id && strcmp((*attachment).id, file->href)==0 )
			{
				resultCode = sendSTOR(soap, targetPath, (*attachment).size );
				if ( resultCode != SOAP_OK )
				{
					return resultCode;
				}
				resultCode = receiveFtpOk(soap, g_ftpHtiTimeoutControl);
				if ( resultCode != SOAP_OK )
				{
					return resultCode;
				}
				resultCode = sendBinary( soap, (*attachment).ptr, (*attachment).size );
				if ( resultCode != SOAP_OK )
				{
					return resultCode;
				}
				return receiveFtpOk(soap, g_ftpHtiTimeoutData); 
			}
		} 
	}
	else
	{
		soap->error = soap_sender_fault(soap, "Invalid arguments", NULL);
		return SOAP_FAULT;
	}

	soap->error = soap_sender_fault(soap, "Invalid attachment", NULL);
	resultCode = SOAP_FAULT;

	return resultCode;
}

int ns1__getFile(struct soap* soap,
				 wchar_t *filePath,
				 struct ns1__getFileResponse &r)
{
	int resultCode = SOAP_OK;

	if ( filePath == NULL )
	{
		soap->error = soap_sender_fault(soap, "Invalid arguments", NULL);
		return SOAP_FAULT;
	}

	resultCode = sendHtiMessage( soap, CMD_FTP_RETR, filePath );

	HtiSoapHandlerInterface* handler = static_cast<HtiSoapHandlerInterface*>(soap->user);

    //receive FILESIZE message
	DWORD fileSize = 0;
	if (handler->WaitForHtiMessage(g_ftpHtiTimeoutControl) )
	{
		if ( !handler->IsReceivedHtiError() )
		{
			DWORD bodySize = handler->ReceivedHtiMessageBodySize();
			BYTE* body = (BYTE*)(handler->ReceivedHtiMessageBody());

			if ( bodySize == 5 && body[0] == CMD_FTP_FILESIZE )
			{
				fileSize = *((DWORD*)(body + 1));
			}
			else
			{
				soap->error = soap_receiver_fault(soap,
												"Received invalid hti message", NULL);
				return SOAP_FAULT;
			}
		}
		else
		{
			handler->SendSoapFaultFromReceivedHtiError();
			return SOAP_FAULT;
		}
	}
	else
	{
		soap->error = soap_receiver_fault(soap,
										  "Failed receive hti message", NULL);
		return SOAP_FAULT;
	}
	
	//receive data messages
	DWORD receivedFile = 0;
	char* body = (char*)soap_malloc(soap, fileSize );

	while ( receivedFile < fileSize )
	{
		if (handler->WaitForHtiMessage(g_ftpHtiTimeoutData) )
		{
			if ( !handler->IsReceivedHtiError() )
			{
				//extract response
				DWORD bodySize = handler->ReceivedHtiMessageBodySize();
				memcpy( body + receivedFile,
					   (char*)(handler->ReceivedHtiMessageBody()),
					   bodySize );
				receivedFile += bodySize;
			}
			else
			{
				handler->SendSoapFaultFromReceivedHtiError();
				return SOAP_FAULT;
			}
		}
		else
		{
			soap->error = soap_receiver_fault(soap,
											"Failed receive hti message", NULL);
			return SOAP_FAULT;
		}
	}

	//create attachment
	int pLen = (int)wcslen(filePath)+1;
	r._return.href = (char*)soap_malloc(soap, pLen );
	WideCharToMultiByte(CP_ACP, 0, filePath, -1, r._return.href, pLen, NULL, NULL);

	r._return.mimeType = (char*)soap_malloc(soap, strlen(fileMimeType)+1 );
	strcpy( r._return.mimeType, fileMimeType );

	//pass result as DIME attachment
	soap_set_dime(soap);
	resultCode = soap_set_dime_attachment(soap,
								body,
								fileSize,
								r._return.mimeType,
								r._return.href,
								0, NULL);
	if (  resultCode != SOAP_OK )
	{
		//Util::Debug("soap_set_dime_attachment failed");
		soap_clr_dime(soap);
		soap->error = soap_receiver_fault(soap,
								"Failed create DIME attachment", NULL);
	}
	
	return resultCode;
}

int ns1__cancelFileTransfer(struct soap* soap,
							void *_,
							struct ns1__cancelFileTransferResponse *out)
{
	int resultCode = sendHtiMessage( soap, CMD_FTP_CANCEL, NULL );
	if ( resultCode != SOAP_OK )
	{
		return resultCode;
	}
	
	return receiveFtpOk(soap, g_ftpHtiTimeoutControl);
}

int ns1__listFiles(struct soap* soap,
				   wchar_t *targetDir,
				   struct ArrayOfStrings &dirs)
{
	return sendHtiMessageAndReceiveStringArray( soap, CMD_FTP_LIST, targetDir, dirs );
}

int ns1__listFilesSizes(struct soap* soap,
                        wchar_t *targetDir,
                        struct ArrayOfHtiFileInfos &fileInfos)
{
    return sendHtiMessageAndReceiveHtiFileInfoArray( soap, CMD_FTP_LISTSIZE,
                                                     targetDir, fileInfos );
}

int ns1__listDirs(struct soap* soap,
				  wchar_t *targetDir,
				  struct ArrayOfStrings &files)
{
	return sendHtiMessageAndReceiveStringArray( soap, CMD_FTP_LISTDIR, targetDir, files );
}

int ns1__createDir(struct soap* soap,
				   wchar_t *targetDir,
				   struct ns1__createDirResponse *out)
{
	return sendHtiMessageAndReceiveOK( soap, CMD_FTP_MKD, targetDir );
}

int ns1__deleteDir(struct soap* soap,
				   wchar_t *targetDir,
				   struct ns1__deleteDirResponse *out)
{
	return sendHtiMessageAndReceiveOK( soap, CMD_FTP_RMD, targetDir );
}

int ns1__deleteFile(struct soap* soap,
					wchar_t *targetFile,
					struct ns1__deleteFileResponse *out)
{
	return sendHtiMessageAndReceiveOK( soap, CMD_FTP_DELE, targetFile );
}


int send_receive_htimessage(struct soap* soap, BYTE **msgBody, int &msgBodyLen, int timeout)
{
	// Send the message to symbian side
	HtiSoapHandlerInterface* handler = static_cast<HtiSoapHandlerInterface*>(soap->user);
	handler->SendHtiMessage(HTI_UID, *msgBody, msgBodyLen, CONTROL_PRIORITY);

	// Clean these up for received HTI message
	delete *msgBody;
	*msgBody = NULL;
	msgBodyLen = 0;

	// ...and wait for OK or error msg
	if (handler->WaitForHtiMessage(timeout))
	{
		if ( !handler->IsReceivedHtiError() )
		{
			// Get received message
			*msgBody = (BYTE*) handler->ReceivedHtiMessageBody();
			msgBodyLen = handler->ReceivedHtiMessageBodySize();
			return ERROR_SUCCESS;
		}
		else
		{
			handler->SendSoapFaultFromReceivedHtiError();
			return ERROR_GEN_FAILURE;
		}
	}
	// ...or timeout
	else
	{
		soap->error = soap_receiver_fault(soap, "HtiGateway", "No response from symbian side");
		return WAIT_TIMEOUT;
	}
}

int ns1__setForcedOperations(struct soap* soap,
							 bool state, 
                             struct ns1__setForcedOperationsResponse *out)
{
	int msgBodyLen = 1+1;
	BYTE *msgBody = new BYTE[msgBodyLen];
	msgBody[0] = CMD_FTP_SETFORCE;
	msgBody[1] = (BYTE) state;

	if( send_receive_htimessage(soap, &msgBody, msgBodyLen, g_ftpHtiTimeoutControl) 
        != ERROR_SUCCESS )
		return SOAP_FAULT;

	if(msgBody[0] != CMD_FTP_OK)
	{
		soap->error = soap_receiver_fault(soap, "HtiError", "Operation failed. Response code not ok.");
		return SOAP_FAULT;
	}

	return SOAP_OK;
}

int ns1__fileChecksum(struct soap* soap, 
					  unsigned char algorithmId, 
					  wchar_t *targetFile, 
                      struct ArrayOfBytes &checksumByteArray)
{
	int targetFileLen = targetFile ? (int) wcslen(targetFile) : 0;
	if(targetFileLen == 0)
	{
		soap->error = soap_receiver_fault(soap, "HtiGateway", "targetFile parameter missing");
		return SOAP_FAULT;
	}

	int msgBodyLen = 1+1+1+targetFileLen*2;
	BYTE *msgBody = new BYTE[msgBodyLen];
	msgBody[0] = CMD_FTP_CHECKSUM_U;
	msgBody[1] = algorithmId;
	msgBody[2] = targetFileLen;
	memcpy(msgBody+3, targetFile, targetFileLen*2);

	if( send_receive_htimessage(soap, &msgBody, msgBodyLen, g_ftpHtiTimeoutControl) 
        != ERROR_SUCCESS )
		return SOAP_FAULT;

	checksumByteArray.__size = msgBodyLen;
	checksumByteArray.__ptr = (BYTE*)soap_malloc(soap, sizeof(BYTE)*checksumByteArray.__size );

	memcpy(checksumByteArray.__ptr, msgBody, msgBodyLen);

	return SOAP_OK;
}

int ns1__format(struct soap* soap,
				unsigned char drive, 
                unsigned char formatMode,
                struct ns1__formatResponse *out)
{
	int msgBodyLen = 3;
	BYTE *msgBody = new BYTE[msgBodyLen];
	msgBody[0] = CMD_FTP_FORMAT;
	msgBody[1] = (BYTE) drive;
    msgBody[2] = (BYTE) formatMode;

    // might take long with large memory cards
	if( send_receive_htimessage(soap, &msgBody, msgBodyLen, g_ftpHtiTimeoutData) 
        != ERROR_SUCCESS )
		return SOAP_FAULT;

	if(msgBody[0] != CMD_FTP_OK)
	{
		soap->error = soap_receiver_fault(soap, "HtiError", "Operation failed. Response code not ok.");
		return SOAP_FAULT;
	}

	return SOAP_OK;
}

int ns1__listDrives(struct soap* soap,
                    void *_,
                    struct ArrayOfHtiDriveInfos &driveInfos)
{
    return sendHtiMessageAndReceiveHtiDriveInfoArray( soap, CMD_FTP_LISTDRIVES,
                                                     driveInfos );
}

