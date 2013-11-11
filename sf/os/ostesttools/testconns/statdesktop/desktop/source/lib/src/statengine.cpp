/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#include "stdafx.h"
#include "STATEngine.h"
#include "STATCommon.h"
#include "INI.h"

#include <string>

//----------------------------------------------------------------------------
// Constructor
CSTATEngine::CSTATEngine()
: pImageVerify(NULL), pConverter(NULL), pComms(NULL), pDecoder(NULL), pLog(NULL),
  minimumdelay(100), maximumdelay(30000), eStopProcessing(STAT_RUN), iCurrentCommand(0), bMultithreaded(false),
  eConnectType(SymbianInvalid), pDeviceInfo(NULL), iDeviceCode(0), iMaxTimeLimit(STAT_MAXTIME), dataSocket(NULL), settingSocket(true), iTEFSharedData(NULL)
{	
	memset(&szAddress, 0, sizeof(szAddress));
	statIniFile.SetIniFileName(STAT_INI_NAME);
}


//----------------------------------------------------------------------------
// Destructor
CSTATEngine::~CSTATEngine()
{
	Release();
}


//----------------------------------------------------------------------------
// Set things up
int
CSTATEngine::Initialise(const STATCONNECTTYPE eConnect, const char *pAddress)
{
	iDelay = minimumdelay;




	// logging - should be the first to be created for all other classes that depend on it
	pLog = new CSTATLogFile;
 	if (!pLog)
 		return E_OUTOFMEM;

	int ret = SetScreenshotDefaultDirectory();
	if (ret != ITS_OK)
		return ret;

	// image verification
 	pImageVerify = new CSTATImageVerify(pLog);
 	if (!pImageVerify)
 		return E_OUTOFMEM;

	// bitmap conversion
	pConverter = new CSTATDataFormatConverter(pLog);
 	if (!pConverter)
 		return E_OUTOFMEM;

	// script decoder
	pDecoder = new CSTATScriptDecoder(pLog);
 	if (!pDecoder)
 		return E_OUTOFMEM;

	// communications
	pComms = new STATComms();
 	if (!pComms)
 		return E_OUTOFMEM;
	else
	{
		EnterCriticalSection(&CriticalSection);

		ret = pComms->SetTransport(eConnect);
		if (ret == ITS_OK)
		{
			ret = pComms->Initialise();

			if (ret == ITS_OK)
				ret = pComms->Connect(pAddress);

			if (ret == ITS_OK)
			{
				strcpy(szAddress, pAddress);	// save port in case of restart after error
				eConnectType = eConnect;
			}
		}

		LeaveCriticalSection(&CriticalSection);
	}
	return ret;
}



//----------------------------------------------------------------------------
// Release resources
int
CSTATEngine::Release(void)
{
	if (pImageVerify)
	{
		delete pImageVerify;
		pImageVerify = NULL;
	}

	if (pConverter)
	{
		delete pConverter;
		pConverter = NULL;
	}

	if (pDecoder)
	{
		delete pDecoder;
		pDecoder = NULL;
	}

	if (pDeviceInfo)
	{
		delete [] pDeviceInfo;
		pDeviceInfo = NULL;
	}

	if (iTEFSharedData)
	{
		delete [] iTEFSharedData;
		iTEFSharedData = NULL;
	}
	
	EnterCriticalSection(&CriticalSection);

	if (pComms)
	{
		delete pComms;
		pComms = NULL;
	}

	// release this one last in case other objects are using it...
	if (pLog)
	{
		pLog->CloseLogFile();
		delete pLog;
		pLog = NULL;
	}

	LeaveCriticalSection(&CriticalSection);
	return ITS_OK;
}


//----------------------------------------------------------------------------
// Sets up log file
int
CSTATEngine::SetLogging(const CString& logfilename, const char* prefix, bool append, bool bMessages, bool bFile,
	MessageReporter *const aMessageReporter)
{
	EnterCriticalSection(&CriticalSection);
	//set default logging path
	CString defaultDirectory=STAT_LOGFILEPATH_VALUE;
	//read from inifile if entry exists
	if(statIniFile.SectionExists(ST_TEST_KEY) )
	{
		CString setting;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_LOGFILEPATH,ST_TEST_KEY);
		if(!setting.IsEmpty())
			defaultDirectory = setting;
	}
	int ret = pLog->CreateLogFile(logfilename,defaultDirectory, prefix, append, bMessages, bFile);

	pLog->SetMessageReporter(aMessageReporter);

	// in case our transport was initialised before logging was
	// display the initialisation message if there is one
	if (ret == LOG_FILE_OK)
		Message(pComms->Error());

	LeaveCriticalSection(&CriticalSection);
	return ret;
}


//----------------------------------------------------------------------------
// Sets up log file
void
CSTATEngine::SetCommandDelay(int iMillisecondDelay)
{
	iDelay = iMillisecondDelay;

	// set some limits
	if (iDelay < 100)
		iDelay = minimumdelay;

	if (iDelay > 30000)
		iDelay = maximumdelay;
}


//----------------------------------------------------------------------------
// Open, read and count the amount of commands on this script
int
CSTATEngine::GetCommandCount(CString file, int *pCount)
{
	EnterCriticalSection(&CriticalSection);

	int ret = pDecoder->Initialise(file, true);
	if (ret != ITS_OK)
		ret = pDecoder->Initialise(file, false);

	if (ret == ITS_OK)
	{
		CSTATScriptCommand *pSendCommand;
		int iCount = 0;
		
		// get a command from the script
		while (pDecoder->GetNextCommand(&pSendCommand) && ret == ITS_OK)
		{
			iCount++;
			if (pSendCommand->cCommandID == STAT_END)
				break;
		}

		// set the count
		if (ret == ITS_OK)
			(*pCount) = iCount;
	}

	pDecoder->Release();

	LeaveCriticalSection(&CriticalSection);
	return ret;
}


//----------------------------------------------------------------------------
// Open and read a script file's contents
int
CSTATEngine::OpenScriptFile(CString file, bool bIsFile)
{
	EnterCriticalSection(&CriticalSection);

	int ret = pDecoder->Initialise(file, bIsFile);

	LeaveCriticalSection(&CriticalSection);
	return ret;
}


//----------------------------------------------------------------------------
// Execute a script file
int
CSTATEngine::RunScript(ScriptProgressMonitor *const monitor)
{
	int ret = ITS_OK;
	iCurrentCommand = 0;
	eStopProcessing = STAT_RUN;
	iDeviceCode = 0;

	// anything smaller can cause problems and doesn't make sense anyway!
	if (iMaxTimeLimit < 1000)
		iMaxTimeLimit = 1000;

	// pointers to our command structures
	CSTATScriptCommand *pSendCommand;
	CSTATScriptCommand *pRecvCommand;

	char lastCommand = NULL;

	receivedData.Empty( );
	
	

	// get a command from the script
	while (pDecoder->GetNextCommand(&pSendCommand) && ret == ITS_OK)
	{
		iCurrentCommand++;

		if (StopProcessing())
		{
			pComms->Send(STAT_RESYNCID);
			ret = E_USERCANCEL;
			break;
		}

		if (lastCommand == STAT_REBOOT)
		{
			ret = ITS_OK;
			break;
		}

		switch(pSendCommand->cCommandID)
		{
			case 'P':
				Message(pSendCommand->Command());
				Sleep(atol(pSendCommand->Command()));
				break;
			case '/':
				Message(pSendCommand->Command());
				break;
			case '#':
				{
					Message(pSendCommand->Command());
					cScreenshotDirectory = pSendCommand->Command();
					if(cScreenshotDirectory.Right(1) != _T("\\"))
						cScreenshotDirectory += _T("\\");
					CreateAllDirectories(cScreenshotDirectory);
				}
				break;
			
			default:
				{

					// send the command and retrieve a response
					int iResyncErrors = 0;
					while ((ret = SendCommand(pSendCommand, &pRecvCommand)) == E_RESYNCCOMMAND)
					{
						Sleep(STAT_RETRYDELAY);
						iResyncErrors++;
						if (iResyncErrors > STAT_MAXERRORS)
						{
							Message("Too many resync errors - stopping");
							ret = E_COMMANDFAILED;
							break;
						}
						
					}

					if (ret == ITS_OK)
					{
						// perform special operations for these commands
						switch(pSendCommand->cCommandID)
						{
							case 'D':
								StoreData(pRecvCommand->Command(), pRecvCommand->Length(), pDeviceInfo);
								AppendCommandToSTATLog("*** DEVICE INFORMATION ***", pRecvCommand->Command(), pRecvCommand->Length());
								break;
							case 'S':
							{
								// convert and save the data returned in the response
								CString image = pSendCommand->Command();
								ret = ConvertAndSaveScreeenshot(image, pRecvCommand->Command(), pRecvCommand->Length());

								// imave verification
								if (ret == ITS_OK)
								{
									if (pImageVerify->IsActive() && pConverter->bWriteToFile)
									{
										ret = pImageVerify->VerifyImage(image);
										if (ret == VERIFICATION_PASS)
											ret = ITS_OK;
									}
								}
								break;
							}
						
							case 'T':
							{
								
								if(dataSocket==NULL)
								{

									// filename has been sent, now send the file itself
									CSTATScriptCommand oSendCommand;
									oSendCommand.cCommandID = pRecvCommand->cCommandID;
									
										// read and send the file contents
									if ((ret = ReadTransferFile(pSendCommand->Command(), &oSendCommand)) == ITS_OK)
									{
										int iResyncErrors = 0;
										while ((ret = SendCommand(&oSendCommand, &pRecvCommand)) == E_RESYNCCOMMAND)
										{
											Sleep(STAT_RETRYDELAY);
											iResyncErrors++;
											if (iResyncErrors > STAT_MAXERRORS)
											{
												Message("Too many resync errors - stopping");
												ret = E_COMMANDFAILED;
												break;
											}
										}
									}
									
								}
								else
								{
									//release the socket
									ret = ReleaseSocket();
									
								}
								

								break;
							}
							case 'R':
							case 'X':
							{	
								if(dataSocket==NULL)
								{
									// save the file contents
									ret = SaveTransferFile(pSendCommand->Command(), pRecvCommand->Command(), pRecvCommand->Length());
								}
								else
								{
									//release the socket
									ret = ReleaseSocket();
								}
								break;
							}
							case 'G':
							{
								// upload the device log file and write to STAT log file
								AppendCommandToSTATLog("*** DEVICE LOG ***", pRecvCommand->Command(), pRecvCommand->Length());
								break;
							}
							case STAT_REFRESH:
							case STAT_END:
							{
								ret = END_SCRIPT;
								break;
							}
							case 'N':
							{
								// Retrieve the TEF shared data
								StoreData(pRecvCommand->Command(), pRecvCommand->Length(), iTEFSharedData);
								AppendCommandToSTATLog("*** RETRIEVE TEF SHARED DATA ***", pRecvCommand->Command(), pRecvCommand->Length());
							}
							break;
							default:
							{
								Sleep(iDelay);
								break;
							}
						}
					}

					if (ret == ITS_OK)
					{
						// Data received from certain of the commands is stored
						// for retreival later.
						switch(pSendCommand->cCommandID)
						{
							case 'W':
							case 'V':
							//execute returns pid
							case 'J':
							//poll returns 0 1
							case '3':
								receivedData += oRecvCommand.Command();
								break;
							default:
								break;
						}
					}
				}
				break;
		}

		lastCommand = pSendCommand->cCommandID;

		if(monitor)
		{
			monitor->OnCompleteCommand( iCurrentCommand );
		}
	}

	pDecoder->Release();

	return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Converts a char * to it's Unicode equivalent
//
LPTSTR
CSTATEngine::ToUnicode(const char *string)
{
#ifdef UNICODE
	static TCHAR szBuffer[MAX_UNICODE_LEN + 1] = {0};
	szBuffer[0] = (TCHAR)0;

    // Convert to UNICODE.
    if (!MultiByteToWideChar(CP_ACP,					// conversion type
							 0,							// flags
							 string,					// source
							 -1,						// length
							 szBuffer,					// dest
							 MAX_UNICODE_LEN))			// length
    {
        return _T("Could not convert");
    }
    return szBuffer;
#else
	return (LPTSTR)string;
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// Converts a Unicode to it's char * equivalent
//
char *
CSTATEngine::ToAnsi(LPCTSTR string)
{
#ifdef UNICODE
	static char szBuffer[MAX_UNICODE_LEN + 1] = {0};
	szBuffer[0] = (char)0;

    // Convert to ANSI.
    if (!WideCharToMultiByte(CP_ACP,					// conversion type
							 0,							// flags
							 string,					// source
							 -1,						// length
							 szBuffer,					// dest
							 MAX_UNICODE_LEN,			// length
							 NULL,
							 NULL ))
    {
        return "Could not convert";
    }
    return szBuffer;
#else
	return (char *)string;
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
// sets screenshot image directory
int CSTATEngine::SetScreenshotDefaultDirectory(void)
{
	//Set default logfile path
	CString path = ST_LOGFILEPATH_VALUE;
	//read from inifile if entry exists
	if(statIniFile.SectionExists(ST_TEST_KEY) )
	{
		CString setting;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_LOGFILEPATH,ST_TEST_KEY);
		if(!setting.IsEmpty())
			path = setting;
	}

	
	cScreenshotDirectory = path;

	// add a backslash
	if (cScreenshotDirectory.Right(1) != _T('\\'))
		cScreenshotDirectory += _T('\\');

	cScreenshotDirectory += _T("Images\\");

	return ITS_OK;
}


//////////////////////////////////////////////////////////////////////////////////////
// Execute a complete command
//
// Sends a command, breaking it up into separate commands if required
//
// When sending, if larger than our buffer , break it up into correctly sized
// pieces and send these.  The first will be an empty one containing the total
// size, followed by buffer-sized pieces.  The last will be an empty command.
//

//////////////////////////////////////////////////////////////////////////////////////
int CSTATEngine::SendCommand(CSTATScriptCommand *pSendCommand, CSTATScriptCommand **ppRecvCommand)
{
	int ret = GENERAL_FAILURE;
	CSTATScriptCommand tempCommand;
	tempCommand.cCommandID = pSendCommand->cCommandID;


	// the first instance of a 'R' or 'T' command, it could set a data socket instead of a file
	if(pSendCommand->cCommandID=='T' || pSendCommand->cCommandID=='R')
	{
		if(dataSocket==NULL && settingSocket && pSendCommand->Command()!=NULL)
		{

			char *comma = strchr(pSendCommand->Command(),',');
			
			if(!(comma > pSendCommand->Command() + pSendCommand->Length()))
			{
	
				switch(pSendCommand->cCommandID)
				{
				case 'R':
					{
						char *colon = strchr(comma,':');
						
						if(colon==NULL)
							break;
	
						comma += 1;
						std::string ipAdd( comma, colon - comma );
						colon += 1;
						std::string ipPort( colon );
	
						ret = SetSocket(ipAdd.c_str() , atoi( ipPort.c_str() ));

						if(ret!=ITS_OK)
						{
							return ret;
						}
	
						break;
					}
	
				case 'T':
					{
						char *colon = strchr( pSendCommand->Command() , ':' );
	
						if(colon==NULL)
							break;
	
						std::string ipAdd( pSendCommand->Command() , colon - pSendCommand->Command() );
						colon += 1;
						
						if(comma < colon)
							break;
	
						std::string ipPort( colon , comma - colon );
	
						ret = SetSocket(ipAdd.c_str() , atoi(ipPort.c_str()));
						
						if(ret!=ITS_OK)
						{
							return ret;
						}

						break;
					}
				}
			}
		}

		settingSocket=false;
	}
	
	
	
	// send the first command
	if ((ret = SendSingleCommand(pSendCommand, ppRecvCommand)) == ITS_OK)
	{
		// check our response - if Serial/Infra-red, need to break it down
		if (pComms->GetMaxPacketSize() != 0)
		{

			
			// break up the command into buffer-sized chunks
			if (pSendCommand->Length() > pComms->GetMaxPacketSize() || (dataSocket!=NULL && pSendCommand->cCommandID=='T'))
			{
				int i = 0;
				unsigned long offset = 0;
				unsigned long ulTotalWritten = 0;
				unsigned long AmountToWrite = pComms->GetMaxPacketSize();
				unsigned long OriginalLength = pSendCommand->Length();
				
				if(dataSocket==NULL)
				{
					
					int iWrites = pSendCommand->Length() / pComms->GetMaxPacketSize() + 1;

					Message("Writing %d bytes of data in %d separate writes...", OriginalLength, iWrites);
					for (i=0;i<iWrites;i++)
					{
						offset = i * pComms->GetMaxPacketSize();

						if ((pSendCommand->Length() - offset) < pComms->GetMaxPacketSize())
							AmountToWrite = (pSendCommand->Length() - offset);

						if (AmountToWrite)
						{
							//Sleep(100);		// pause a bit for slower machines - probably doesn't need it but it can't hurt...

							// now send the command
							Message("Writing %d bytes from offset %d", AmountToWrite, offset);
							tempCommand.SetData(pSendCommand->Command() + ulTotalWritten, AmountToWrite);
							if ((ret = SendSingleCommand(&tempCommand, ppRecvCommand)) == ITS_OK)
							{
								//Message("%d bytes successfully written", AmountToWrite);
							}
							else
								return ret;

							ulTotalWritten += AmountToWrite;
						}
					}
					
				}
				else
				{

					//uses socket transmission

					char *pData = new char[AmountToWrite];
					
					tempCommand.ulLength = (unsigned long) -1;
					
					if((ret = SendSingleCommand(&tempCommand, ppRecvCommand)) != ITS_OK)
					{	
						delete [] pData;
						return ret;
					}

					while(true){


						int AmountToWrite = pComms->GetMaxPacketSize();
						
						ReadFromSocket(pData, &AmountToWrite );
											
						if(AmountToWrite>=0)
						{
							Message("Writing %d bytes", AmountToWrite);
							tempCommand.SetData(pData , AmountToWrite);
						
							if ((ret = SendSingleCommand(&tempCommand, ppRecvCommand)) == ITS_OK)
							{
							//Message("%d bytes successfully written", AmountToWrite);
							}
							else
							{
								settingSocket=true;
								delete [] pData;
								return ret;
							}

							ulTotalWritten +=AmountToWrite;
						}
						else
						{
							eStopProcessing = STAT_PAUSE;
							break;
						}
						
						if(AmountToWrite==0)
						{
							break;
						}
						
					}

					delete [] pData;
				
				}
			
				// once completely sent, send an empty command to show that we've finished
				
				
				if(dataSocket == NULL )
				{
					if (ulTotalWritten == OriginalLength)
					{
						// send the command that signals the end of our transmission
						Message("Sending completion command %c", tempCommand.cCommandID);
						tempCommand.SetData(NULL, 0);
						if ((ret = SendSingleCommand(&tempCommand, ppRecvCommand)) == ITS_OK)
						{
							//Message("Completion command successfully written");
						}
					}
					else
					{
						Message("Incorrect number of bytes written - expected %ld got %ld", ulTotalWritten, OriginalLength);
						ret = E_BADNUMBERBYTES;
					}
				}
			}
		}
	}

	if (ret == ITS_OK)
	{
		// check our response - if Serial/Infra-red, need to break it down
		// if < STAT_BUFFERSIZE, nothing more to do
		// if not, we need to read in as many times as it takes to
		// assemble our complete command response
		if (pComms->GetMaxPacketSize() != 0)
		{
			if (ppRecvCommand && (*ppRecvCommand)->Length() > pComms->GetMaxPacketSize())
			{
				unsigned long TotalLength = (*ppRecvCommand)->Length();

				Message("%s: About to read %d bytes of data", GetConnection(eConnectType), TotalLength);

				// allocate memory to hold entire command
				char *pTemp = new char [TotalLength];
				if (pTemp)
				{
					unsigned long ulTotalRead = 0;

					// empty packets
					tempCommand.SetData(NULL, 0);

					// now read data until we get an empty packet
					while((*ppRecvCommand)->Length())
					{
						//Sleep(100);		// pause a bit for slower machines - probably doesn't need it but it can't hurt...

						//Message("Sending continuation command %c", tempCommand.cCommandID);
						if ((ret = SendSingleCommand(&tempCommand, ppRecvCommand)) == ITS_OK)
						{
							if ((*ppRecvCommand)->Length())
							{
								Message("%s to offset %d", pComms->Error(), ulTotalRead);
								
								//copy the data into the buffer
								memcpy(pTemp + ulTotalRead, (*ppRecvCommand)->Command(), (*ppRecvCommand)->Length());
								
								if(dataSocket!=NULL)
								{
									int bytesSent = (int)(*ppRecvCommand)->Length();
								
									WriteToSocket((*ppRecvCommand)->Command() , &bytesSent);
									
									if(bytesSent <= 0)
									{
										eStopProcessing = STAT_PAUSE;
									}
								}
								
								

								//increment the pointer to the end of the first chunk received
								ulTotalRead += (*ppRecvCommand)->Length();
							}
						}
						else
						{
							settingSocket=true;
							delete [] pTemp;
							return ret;
						}
					}

					// make sure we got what we expected
					if (ulTotalRead == TotalLength)
					{
						// set our final response to the complete data transmission
						(*ppRecvCommand)->SetData(pTemp, ulTotalRead);
						//Message("Received successfully %ld bytes", ulTotalRead);
					}
					else
					{
						Message("Incorrect number of bytes read - expected %ld got %ld", ulTotalRead, TotalLength);
						ret = E_BADNUMBERBYTES;
					}

					// finished with it
					delete [] pTemp;
				}
				else
				{
					Message("Could not allocate %d bytes of memory", TotalLength);
					ret = E_OUTOFMEM;
				}
			}
			else if(dataSocket!=NULL && pSendCommand->cCommandID=='R')
			{
				int bytesSent = (int)(*ppRecvCommand)->Length();
				WriteToSocket((*ppRecvCommand)->Command(), &bytesSent);
			}
		}
	}

	

	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
// Execute a single command
//
// Once the command is sent, wait for a response.  If received and of the correct
// ID, save the contents (if any) and quit.  If no response, send RESYNC commands
// every few seconds to get response from other end.  On receipt of a RESYNC
// command, resend the previous command.  After no response for a minute or so, quit.
//////////////////////////////////////////////////////////////////////////////////////
int CSTATEngine::SendSingleCommand(CSTATScriptCommand *pSendCommand, CSTATScriptCommand **ppRecvCommand)
{
	int iTotalCommandTime = 0;
	int ret = GENERAL_FAILURE;

	LogDetails("Send", pSendCommand);

	// send the command
	EnterCriticalSection(&CriticalSection);
	ret = pComms->Send(pSendCommand->cCommandID, pSendCommand->Command(), pSendCommand->Length());
	LeaveCriticalSection(&CriticalSection);
	if (ret != ITS_OK)
	{
		Message(pComms->Error());
		return ret;
	}

	// The refresh command will not return anything due to the transport getting restarted.
	// So, don't wait for a response.
	if( (ret == ITS_OK) && (pSendCommand->cCommandID == STAT_REFRESH || pSendCommand->cCommandID == STAT_REBOOT)  )
	{
		return ret;
	}

	// wait for a response
	unsigned long ulLength = 0;
	char *pData = NULL;
	while(iTotalCommandTime < iMaxTimeLimit)
	{
		if (StopProcessing())
		{
			pComms->Send(STAT_RESYNCID);
			ret = E_USERCANCEL;
			break;
		}

		EnterCriticalSection(&CriticalSection);
		ret = pComms->Receive(&oRecvCommand.cCommandID, &pData, &ulLength);
		LeaveCriticalSection(&CriticalSection);
		
		if (ret == ITS_OK)
		{
			if (oRecvCommand.SetData(pData, ulLength))
			{
				oRecvCommand.ulLength = ulLength;
				*ppRecvCommand = &oRecvCommand;

				// invalid response received
				if (pSendCommand->cCommandID != oRecvCommand.cCommandID)
				{
					// need to resync back to start of this command
					if (oRecvCommand.cCommandID == STAT_RESYNCID)
					{
						Message("RESYNC response received - retrying command...");
						ret = E_RESYNCCOMMAND;
					}
					else
					{
						if (oRecvCommand.cCommandID == STAT_FAILURE)
						{
							if (oRecvCommand.Command() && oRecvCommand.Length())
							{
								Message("Command failed with error code %s",
										oRecvCommand.Command());
								iDeviceCode = atoi(oRecvCommand.Command());
							}
						}
						else
							Message("Invalid response received - expected %c received %c",
									pSendCommand->cCommandID, oRecvCommand.cCommandID);
						ret = GENERAL_FAILURE;
					}
				}
				else
					LogDetails("Receive", *ppRecvCommand);
			}
			else
				ret = E_OUTOFMEM;

			// at this point we have received something whether it's what we were expecting or not so exit loop
			break;
		}
		else if (ret == NO_DATA_AT_PORT)
		{
//			Message("Waiting for response");
			Sleep(STAT_RETRYDELAY);
			iTotalCommandTime += STAT_RETRYDELAY;
			ret = E_TOOMUCHTIME;
		}
		else
		{
			Message(pComms->Error());
			break;
		}
	}

	return ret;
}


//----------------------------------------------------------------------------
// Write command details to log
void CSTATEngine::LogDetails(const char *prefix, CSTATScriptCommand *pCommand)
{
	if (pCommand->Length() && (pCommand->Length() < MAX_LOG_MSG_LEN) && 
		pCommand->cCommandID != 'D' && pCommand->cCommandID != 'T' && pCommand->cCommandID != 'R' && pCommand->cCommandID != 'X' && pCommand->cCommandID != 'G')
	{
		char szContents[MAX_LOG_MSG_LEN + 1];
		unsigned long actuallen = MAX_LOG_MSG_LEN - 30 - strlen(prefix) - 6;  // max actual message length
		if (pCommand->Length() < actuallen)
			actuallen = pCommand->Length();

		strncpy(szContents, pCommand->Command(), actuallen);
		*(szContents + actuallen) = (char)0;
		Message("%s: ID: %c Contents: [%s] Length: [%lu]", prefix, pCommand->cCommandID, szContents, pCommand->Length());
	}
	else
		Message("%s: ID: %c Length: [%lu]", prefix, pCommand->cCommandID, pCommand->Length());
}


//----------------------------------------------------------------------------
// Read in a file's contents
//
// pFile will contain "pc path,device path" of which we want the 'pc path' part
int CSTATEngine::ReadTransferFile(const char *pFile, CSTATScriptCommand *pCommand)
{
	int ret = E_BADFILENAME;

	// get the default file location
	CString path = ST_WORKINGPATH_VALUE;
	//read from inifile if entry exists
	if(statIniFile.SectionExists(ST_TEST_KEY) )
	{
		CString setting;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_WORKINGPATH,ST_TEST_KEY);
		if(!setting.IsEmpty())
			path = setting;
	}

	// add a backslash
	if (path.Right(1) != _T('\\'))
		path += _T('\\');

	// get the name of the file
	CString filename = pFile;
	int index = filename.ReverseFind(_T(','));
	if (index == -1)
		return E_BADFILENAME;

	// add the name
	path += filename.Left(index);

	Message("Opening file [%s]", ToAnsi(path));

	EnterCriticalSection(&CriticalSection);

	CFile script_file;
	if (script_file.Open(path, CFile::modeRead))
	{
		try
		{
			char *pData = new char [script_file.GetLength()];
			if (pData)
			{
				script_file.Read(pData, script_file.GetLength());

				if(pCommand->SetData(pData, script_file.GetLength()))
					ret = ITS_OK;
				else
					ret = E_OUTOFMEM;

				delete [] pData;
			}
			else
				ret = E_OUTOFMEM;
		}
		catch(CFileException *e)
		{
			e->Delete();
			ret = GENERAL_FAILURE;
		}

		script_file.Abort();
	}

	LeaveCriticalSection(&CriticalSection);
	return ret;
}







//----------------------------------------------------------------------------
// Save a data stream to file
//
// pFile will contain "device path,pc folder" of which we want the 'pc folder' part
int CSTATEngine::SaveTransferFile(const char *pFile, char *pContents, unsigned long ulLength)
{
	int ret = E_BADFILENAME;

	CString path = ST_WORKINGPATH_VALUE;
	//read from inifile if entry exists
	if(statIniFile.SectionExists(ST_TEST_KEY) )
	{
		CString setting;
		setting.Empty();
		setting=statIniFile.GetKeyValue(ST_WORKINGPATH,ST_TEST_KEY);
		if(!setting.IsEmpty())
			path = setting;
	}
	// add a backslash
	if (path.Right(1) != _T('\\'))
		path += _T('\\');

	// get any additional sub folders
	CString folder = pFile;
	int index = folder.Find(_T(','));
	if (index == -1)
		return E_BADFILENAME;
	// if the comma is the last char there are no folders to add
	if (folder.Right(1) != _T(','))
	{
		path += folder.Mid(index + 1);

		// add a backslash
		if (path.Right(1) != _T('\\'))
			path += _T('\\');
	}

	if (bMultithreaded)
	{
		// add a sub-folder for the connection type
		path += GetConnection(eConnectType);
		path += _T(".");
		path += szAddress;
		path += _T('\\');
	}

	// now extract the filename from the first argument
	CString filename = folder.Left(index);
	index = filename.ReverseFind(_T('\\'));
	if (index == -1)
		path += filename;
	else
		path += filename.Mid(index + 1);

	Message("Saving file [%s]", ToAnsi(path));

	ret = SaveTheFile(path, pContents, ulLength);
	if (ret != ITS_OK)
		Message("Error saving [%s] (%d)", ToAnsi(path), GetLastError());
	return ret;
}


//----------------------------------------------------------------------------
// Convert and save a screenshot
int CSTATEngine::ConvertAndSaveScreeenshot(CString &file, const char *pContents, const unsigned long ulLength)
{
	int ret = E_BADFILENAME;

	// clean out any previous mbm files lying around
	if (!RemoveLeftoverMBMFiles())
		return E_SCREENSHOT_LEFTOVERFILES;

	// get the destination screenshot name
	CString destfile = file;
	if (destfile.IsEmpty())
	{
		// construct a filename from date/time
		char szName[20] = {0};
		time_t curTime;
		time (&curTime);
		strftime (szName, 
					sizeof (szName),
					"%Y%m%d%H%M%S",
					localtime (&curTime));

		destfile = szName;
	}

	// get the full name of the source image
	CString SrcPath = cScreenshotDirectory;

	if (bMultithreaded)
	{
		// add a sub-folder for the connection type
		SrcPath += GetConnection(eConnectType);
		SrcPath += _T(".");
		SrcPath += szAddress;
		SrcPath += _T('\\');
	}

	// attach the filename
	CString path = SrcPath;

	path += destfile;
	path += _T(".mbm");

//	Message("Saving screenshot [%s]", ToAnsi(path));

	// save bitmap data to file
	if ((ret = SaveTheFile(path, pContents, ulLength)) == ITS_OK)
		ret = pConverter->ConvertScreenShot(SrcPath, SrcPath);

	// save the full path to the converted image
	if (ret == ITS_OK)
		file = SrcPath;
	return ret;
}


//----------------------------------------------------------------------------
// Save data to file
int CSTATEngine::SaveTheFile(CString path, const char *pContents, const unsigned long ulLength)
{
	int ret = E_BADFILENAME;

	EnterCriticalSection(&CriticalSection);

	// ensure the path exists
	CreateAllDirectories(path);

	DeleteFile(path);

	CFile script_file;
	if (script_file.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
	{
		try
		{
			// might be a zero-length file
			if (pContents && ulLength)
				script_file.Write(pContents, ulLength);

			ret = ITS_OK;
		}
		catch(CFileException *e)
		{
			e->Delete();
			ret = GENERAL_FAILURE;
		}

		script_file.Abort();
	}

	LeaveCriticalSection(&CriticalSection);
	return ret;
}


//----------------------------------------------------------------------------
// Write a message to the log
void CSTATEngine::Message(const char *pMsg, ...)
{
	char szMessage[MAX_LOG_MSG_LEN + 1];

	if (pMsg)
	{
		memset(&szMessage, 0, sizeof(szMessage));
		va_list pCurrent = (va_list)0;
		va_start (pCurrent, pMsg);
		vsprintf (szMessage, pMsg, pCurrent);
		va_end (pCurrent);
	}
	else
		strcpy(szMessage, "No message associated with this error");

	EnterCriticalSection(&CriticalSection);
	pLog->Set(szMessage);
	LeaveCriticalSection(&CriticalSection);
}

//----------------------------------------------------------------------------
// Try to create all directories within a supplied path
void CSTATEngine::CreateAllDirectories(CString &fullpath)
{
	CString path;
	int index = fullpath.Find(_T('\\'));
	while (index != -1)
	{
		path = fullpath.Left(index);
		CreateDirectory(path, NULL);

		index = fullpath.Find(_T('\\'), index + 1);
	}
}

//----------------------------------------------------------------------------
// check if we want to quit
bool CSTATEngine::StopProcessing()
{
	// check if instructed to stop processing (will only happen on Desktop version)
	if (eStopProcessing == STAT_PAUSE)
	{
		eStopProcessing = STAT_STOP;
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//Remove any leftover .mbm files prior to retrieving a screenshot
bool CSTATEngine::RemoveLeftoverMBMFiles()
{
	CFileFind imagefinder;
	CString imagedir = cScreenshotDirectory;
	bool valid = true;

	// add the search pattern
	if (imagedir.Right(1) != _T('\\'))
		imagedir += _T("\\");

	// now add a folder for the connection type
	if (bMultithreaded)
	{
		imagedir += GetConnection(eConnectType);
		imagedir += _T(".");
		imagedir += szAddress;
		imagedir += _T('\\');
	}

	imagedir += _T("*.mbm");

//	Message("Searching for [%s]", ToAnsi(imagedir));

	EnterCriticalSection(&CriticalSection);

	// locate files
	int iLeftFiles = imagefinder.FindFile(imagedir, 0);
	while (iLeftFiles)
	{
		iLeftFiles = imagefinder.FindNextFile();
//		Message("Deleting file [%s]", ToAnsi(imagefinder.GetFilePath()));

		if (!DeleteFile(imagefinder.GetFilePath()))
		{
			valid = false;
			break;
		}
	}

	imagefinder.Close();
	LeaveCriticalSection(&CriticalSection);
	return valid;
}


//----------------------------------------------------------------------------
// Append the device log to the end of the STAT log
void CSTATEngine::AppendCommandToSTATLog(char *heading, char *log, unsigned long length)
{
	if (length && log)
	{
		EnterCriticalSection(&CriticalSection);
		Message(DOTTED_LINE);
		Message(heading);

		char *start = log;
		char *eol = log;
		unsigned long count = 0;

		*(log + length) = (char)0;

		while (count < length - 1)
		{
			// find the end of the line
			while ((count < length - 1) && (*eol != '\r') && (*eol != '\n'))
			{
				count++;
				eol++;
			}

			// null it
			if (count < length - 1)
			{
				*eol = (char)0;
				count++;
				eol++;
			}

			if (start && *start)
				Message("%s", start);

			// step over newline
			while ((count < length - 1) && (*eol == '\r') || (*eol == '\n'))
			{
				count++;
				eol++;
			}

			if (count < length - 1)
				start = eol;
		}

		Message(DOTTED_LINE);
		LeaveCriticalSection(&CriticalSection);
	}
	else
		Message("No information returned from device");
}

//----------------------------------------------------------------------------
// Store some data in a public defined char* pointer
//  (currently iTEFSharedData and pDeviceInfo)
void CSTATEngine::StoreData( char* aData, unsigned long aLength, char*& aOutputPtr )
{

	receivedData.Empty();
	receivedData = aData;

	if( aOutputPtr )
	{
		delete [] aOutputPtr;
		aOutputPtr = NULL;
	}

	if( aData && aLength )
	{
		aOutputPtr = new char[aLength + 1];
		if( aOutputPtr )
		{
			// skip over leading CR/LF's
			char *p = aData;
			while (p && (*p) && (*p == '\r') || (*p == '\n'))
				p++;
			strcpy(aOutputPtr, p);
		}
	}
}

//----------------------------------------------------------------------------
// Translate a connection type to a string
CString CSTATEngine::GetConnection(STATCONNECTTYPE eConnectType)
{
	CString connection;

	switch(eConnectType)
	{
	case SymbianSocket:
		connection = _T("SymbianSocket");
		break;
	case SymbianSerial:
		connection = _T("SymbianSerial");
		break;
	case SymbianInfrared:
		connection = _T("SymbianInfrared");
		break;
	case SymbianBluetooth:
		connection = _T("SymbianBluetooth");
		break;
	case SymbianUsb:
		connection = _T("SymbianUsb");
		break;
	default:
		connection = _T("SymbianInvalid");
		break;
	};

	return connection;
}


//----------------------------------------------------------------------------
//  Sets a data socket to read/write file streams
//
int CSTATEngine::SetSocket(const char *ip, const int port)
{

	//initialise winsock
	WSAStartup( MAKEWORD(2,2), &wsaData );
	
	dataSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( dataSocket == INVALID_SOCKET ) 
	{
        dataSocket=NULL;
		return E_SOCKETCREATE; 
    }

	// Connect to a server.
    sockaddr_in clientService;

    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr( ip );
    clientService.sin_port = htons( port );

    if ( connect( dataSocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) 
	{
        ReleaseSocket();
		return E_SOCKETCONNECT;
    }
	else
	{
		return ITS_OK;
	}


	
}

int CSTATEngine::ReleaseSocket()
{
	int ret = ITS_OK;

	//release the socket
	if( closesocket(dataSocket) == SOCKET_ERROR)
	{
		ret = E_SOCKETCLOSE;
	}


	//cleanup winsock
	WSACleanup();

	dataSocket=NULL;

	return ret;
	
}


//----------------------------------------------------------------------------
// Read from the board and write to the data socket
//

void CSTATEngine::WriteToSocket(const char *data, int *length)
{
	// Send data.
   	*length = send( dataSocket, data, *length , 0 );
}



//----------------------------------------------------------------------------
// Read from the data socket and send to the board
//
void CSTATEngine::ReadFromSocket(char *data, int *length)
{
	int bufferSize = 0xFF;

	if(*length >= bufferSize)
	{
	
		int packetLength = *length;
		
		int totalRead = 0;
		int iterRead = 0;

		// Receive data to fill a packet
		while(totalRead <= packetLength - bufferSize)
		{
			iterRead = recv( dataSocket, data + totalRead, bufferSize, 0 );
			if(iterRead == 0)
				break;
			totalRead += iterRead;
		}

		*length = totalRead;
	}
	else
	{
		*length = recv( dataSocket, data, *length, 0 );
	}
}







 

