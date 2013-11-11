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
* Description:  Definitions for the class CATParseTraceFile.
*
*/


#include "../inc/ATCommonDefines.h"
#include "../inc/CATParseBinaryFile.h"
#include "../inc/catdatasaver.h"
#include "../inc/CATDatParser.h"
#include "../inc/CATProcessData.h"

#include <time.h>



// -----------------------------------------------------------------------------
// CATParseBinaryFile::CATParseTraceFile
// Constructor.
// -----------------------------------------------------------------------------
CATParseBinaryFile::CATParseBinaryFile()
{
	LOG_FUNC_ENTRY("CATParseTraceFile::CATParseTraceFile");
	m_DataSaver.SetPrintFlag( false );
}

// -----------------------------------------------------------------------------
// CATParseBinaryFile::StartParse
// Main function to start trace parsing.
// -----------------------------------------------------------------------------
bool CATParseBinaryFile::StartParse( const char* pFileName, const char* pOutputFileName )
{
	LOG_FUNC_ENTRY("CATParseTraceFile::StartParse");

	// Return value, will be changed to true if process start found.
	bool bRet = false;

	// Check pointers
	if ( pFileName == NULL  )
		return bRet;


	if ( ! FileExists( pFileName ) )
	{
		cout << AT_MSG << "Error, input file \""
			<< pFileName
			<< "\" does not exist." << endl;
		return bRet;
	}

	// Open input and output file
	ifstream in( pFileName, ios::binary );
	ofstream sDataToParse( pOutputFileName );

	// Check file opened ok
	if ( !in.good() )
		return false;

	// Get stream size
	size_t streamPos = in.tellg();
	in.seekg( 0, ios::end);
	size_t streamEnd = in.tellg();
	in.seekg( 0, ios::beg );

	//Origianl characters (not filtered).

	unsigned char cDataFromFile[MAX_LINE_LENGTH];

	bool bProcessEndReached = false;
	bool bError = false;

	char messageType = 0;
	unsigned __int64 messageTime = 0;

	unsigned long iProcessId(0);
	int iAddressCount(0);
	string sTemp;

	//first parse version info
	GetString( cDataFromFile, in, streamPos, streamEnd );
	//todo check if version is correct	

	while( !bError && !bProcessEndReached )
	{
		// get time
		if( !GetNum64( cDataFromFile, in, streamPos, streamEnd ) )
		{
			bError = true;
			break;
		}
		messageTime = StringToNum64( cDataFromFile );

		// get message type
		if( !GetNum8( cDataFromFile, in, streamPos, streamEnd ) )
		{
			bError = true;
			break;
		}
		messageType = (int)cDataFromFile[0];

		switch (messageType)
		{
			case EProcessStart:
				{
				//temp string to store data until we get to processID
				sTemp.clear();

                //PCS MemoryLeaker.exe[edf5a8b2]0001 17b 48939b0f5c04f 1 3 1.10.0 1.7.5
				sTemp.append( LABEL_PROCESS_START );
				sTemp.append( " " );

				// get process name
				if( !GetString( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}

				sTemp.append( (char *)cDataFromFile );
				sTemp.append( " " );

				//get process id
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
			    {
					bError = true;
					break;
				}
				iProcessId = StringToNum32(cDataFromFile);

				// write stored data
				sDataToParse << std::hex << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << std::hex << iProcessId;
				sDataToParse << " ";
				sDataToParse << sTemp;
				sDataToParse <<  std::hex << iProcessId;
				sDataToParse << " ";

				//write time
				sDataToParse <<  std::hex << messageTime;
				sDataToParse << " ";

				//get udeb/urel
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::dec << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				//add trace version - TODO check
				sDataToParse <<  std::dec << 3;
				sDataToParse << " ";

				// get atool version
				if( !GetString( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse << (char *)cDataFromFile;
				sDataToParse << " ";

				// get api version
				if( !GetString( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse << (char *)cDataFromFile;
				sDataToParse << "\n";

				//process start found
				bRet = true;

				}
				break;

			case EProcessEnd:
				{
				sDataToParse << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				//PCE
				sDataToParse << LABEL_PROCESS_END ;
				sDataToParse << "\n";
				bProcessEndReached = true;
				}
				break;

			case EDllLoad:
				{
				sDataToParse << std::hex << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				//DLL AToolMemoryLeakerDll3.dll 3ff80000 3ff92000
				sDataToParse << LABEL_DLL_LOAD ;
				sDataToParse << " ";

				// get dll name
				if( !GetString( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse << (char *)cDataFromFile;
				sDataToParse << " ";

				// get start address
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get end address
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << "\n";
				}
				break;
			
			case EDllUnload:
				{
				sDataToParse << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				//DLU AToolMemoryLeakerDll3.dll 3ff80000 3ff92000
				sDataToParse << LABEL_DLL_UNLOAD ;
				sDataToParse << " ";

				// get dll name
				if( !GetString( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse << (char *)cDataFromFile;
				sDataToParse << " ";

				// get start address
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get end address
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << "\n";
				}
				break;

			case EAllocH:
				{
				sDataToParse << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				//ALH 5a7a6734 5c 17c 11 6003ded4 60010df2 40001bff 40001c39 ...
				// no fragments in log file
				sDataToParse << ALLOCH_ID ;
				sDataToParse << " ";

				// get mam address
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get size of allocation
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get thread id
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get address count
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				int iAddressCount = StringToNum32(cDataFromFile);
				sDataToParse <<  std::hex << iAddressCount;

				// get callstack
				for(int i=0; i<iAddressCount; i++)
				{
					sDataToParse << " ";
					if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
					{
						bError = true;
						break;
					}
				    sDataToParse <<  std::hex << StringToNum32(cDataFromFile); 
				}
				sDataToParse << "\n";
				}
				break;

			case EFreeH:
				{
				sDataToParse << std::hex <<messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				//FRH 5a7a679c 17c 0 (6003ded4 60010df2 40001bff 40001c39 ...)
				// no fragments in log file
				sDataToParse << FREEH_ID ;
				sDataToParse << " ";

				// get mem address
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get thread id
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get address count
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				iAddressCount = StringToNum32(cDataFromFile);
				sDataToParse <<  std::hex << iAddressCount;

				// get callstack
				for(int i=0; i<iAddressCount; i++)
				{
					sDataToParse << " ";
					if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
					{
						bError = true;
						break;
					}
				    sDataToParse <<  std::hex << StringToNum32(cDataFromFile); 
				}
				sDataToParse << "\n";

				break;

			case EReallocH:
				sDataToParse << std::hex << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				// RAH 0 5a7a6f30 30 17c 17 6003e02b 60010ef8 600083e5 ...
				// no fragments in log file
				sDataToParse << REALLOCH_ID ;
				sDataToParse << " ";

				// get freed mem address
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get allocated mem address
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get size of allocation
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get thread id
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << " ";

				// get address count
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				iAddressCount = StringToNum32(cDataFromFile);
				sDataToParse <<  std::hex << iAddressCount;

				// get callstack
				for(int i=0; i<iAddressCount; i++)
				{
					sDataToParse << " ";
					if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
					{
						bError = true;
						break;
					}
				    sDataToParse <<  std::hex << StringToNum32(cDataFromFile); 
				}
				sDataToParse << "\n";
				}
				break;

			case EHandleLeak:
				{
				sDataToParse << std::hex << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				//HDL handleLeakCount
				sDataToParse << LABEL_HANDLE_LEAK ;
				sDataToParse << " ";

				//get handle leak count
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << "\n";
				}
				break;

			case EThreadStart:
				{
				sDataToParse << std::hex << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				// TDS 17c
				sDataToParse << LABEL_THREAD_START;
				sDataToParse << " ";

				// get thread ID
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << "\n";
				}
				break;

			case EThreadEnd:
				{
				sDataToParse << std::hex << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";
				sDataToParse << iProcessId;
				sDataToParse << " ";
				// TDE 17c
				sDataToParse << LABEL_THREAD_END;
				sDataToParse << " ";

				// get thread ID
				if( !GetNum32( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				sDataToParse <<  std::hex << StringToNum32(cDataFromFile);
				sDataToParse << "\n";
				}
				break;

			case EDeviceInfo:
				{
				// DEVINFO swVersion  romChecksum //both are descriptors
				sDataToParse << std::hex << messageTime;
				sDataToParse << " ";
				sDataToParse << MAIN_ID;
				sDataToParse << " ";

				sDataToParse << LABEL_DEVICE_INFO;
				sDataToParse << " ";

				// get swVersion
				if( !GetString( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}			
				sDataToParse << (char *)cDataFromFile;
				sDataToParse << " ";

				// todo check
				// get romChecksum
				if( !GetString( cDataFromFile, in, streamPos, streamEnd ) )
				{
					bError = true;
					break;
				}
				
				sDataToParse << (char *)cDataFromFile;
				sDataToParse << "\n";
				}
				break;

			case EError:
				//currently not used
				break;
			case ETestStart:
			case ETestEnd:
				// currently can not occure in bin log file, this is added from carbide in traces
				break;
			default:
				bError = true;
				break;				
		}
	}

	// Close file.
	in.close();

	sDataToParse.close();
	return !bError;
}

// -----------------------------------------------------------------------------
// CATParseBinaryFile::GetDataSaver
// Gets data saver object.
// -----------------------------------------------------------------------------
CATDataSaver* CATParseBinaryFile::GetDataSaver(void)	
{
	LOG_LOW_FUNC_ENTRY("CATParseTraceFile::GetDataSaver");
	return &m_DataSaver;
}

// -----------------------------------------------------------------------------
// CATParseBinaryFile::StringToNum64
// Gets 64bit number from input string
// -----------------------------------------------------------------------------
unsigned __int64 CATParseBinaryFile::StringToNum64( unsigned char* cVal )
{
	unsigned __int64 iRetVal(0);

	for(int i=7; i>=0; i--)
		iRetVal = ( iRetVal << 8 ) + cVal[i];

	return iRetVal;
}

// -----------------------------------------------------------------------------
// CATParseBinaryFile::StringToNum32
// Gets 32bit number from input string
// -----------------------------------------------------------------------------
unsigned long CATParseBinaryFile::StringToNum32( unsigned char* cVal )
{
	unsigned long iRetVal(0);

	for(int i=4; i>=0; i--)
		iRetVal = ( iRetVal << 8 ) + cVal[i];

	return iRetVal;
}

// -----------------------------------------------------------------------------
// CATParseBinaryFile::GetString
// Get next string from file
// -----------------------------------------------------------------------------
bool CATParseBinaryFile::GetString( unsigned char* pData, ifstream &pIn, size_t &pStreamPos, size_t pStreamEnd )
{
	int	numOfRead = 1;
	if(pStreamPos + numOfRead <= pStreamEnd)
	{
		pIn.read((char*)pData, numOfRead);
		pStreamPos = pIn.tellg();
	}
	else
	{
		return false;
	}

	numOfRead = (int)pData[0] >> 2;
	if(pStreamPos + numOfRead <= pStreamEnd)
	{
		pIn.read((char*)pData, numOfRead);
		pStreamPos = pIn.tellg();
		pData[numOfRead] = 0;
	}
	else
	{
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
// CATParseBinaryFile::GetNum8
// Get next 8bit number from file
// -----------------------------------------------------------------------------
bool CATParseBinaryFile::GetNum8( unsigned char* pData, ifstream &pIn, size_t &pStreamPos, size_t pStreamEnd )
{
	int	numOfRead = 1;
	if(pStreamPos + numOfRead <= pStreamEnd)
	{
		pIn.read((char*)pData, numOfRead);
		pStreamPos = pIn.tellg();
	}
	else
	{
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
// CATParseBinaryFile::GetNum32
// Get next 32bit number from file
// -----------------------------------------------------------------------------
bool CATParseBinaryFile::GetNum32( unsigned char* pData, ifstream &pIn, size_t &pStreamPos, size_t pStreamEnd )
{
	int	numOfRead = 4;
	if(pStreamPos + numOfRead <= pStreamEnd)
	{
		pIn.read((char*)pData, numOfRead);
		pStreamPos = pIn.tellg();
	}
	else
	{
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
// CATParseBinaryFile::GetNum64
// Get next 64bit number from file
// -----------------------------------------------------------------------------
bool CATParseBinaryFile::GetNum64( unsigned char* pData, ifstream &pIn, size_t &pStreamPos, size_t pStreamEnd )
{
	int	numOfRead = 8;
	if(pStreamPos + numOfRead <= pStreamEnd)
	{
		pIn.read((char*)pData, numOfRead);
		pStreamPos = pIn.tellg();
	}
	else
	{
		return false;
	}

	return true;
}

//EOF
