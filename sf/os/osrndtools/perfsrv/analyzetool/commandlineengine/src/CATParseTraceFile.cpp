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
#include "../inc/CATParseTraceFile.h"
#include "../inc/catdatasaver.h"
#include "../inc/CATDatParser.h"
#include "../inc/CATProcessData.h"

#include <time.h>



// -----------------------------------------------------------------------------
// CATParseTraceFile::CATParseTraceFile
// Constructor.
// -----------------------------------------------------------------------------
CATParseTraceFile::CATParseTraceFile()
{
	LOG_FUNC_ENTRY("CATParseTraceFile::CATParseTraceFile");
	m_DataSaver.SetPrintFlag( false );
}

// -----------------------------------------------------------------------------
// CATParseTraceFile::StartParse
// Main function to start trace parsing.
// -----------------------------------------------------------------------------
bool CATParseTraceFile::StartParse( const char* pFileName, const char* pOutputFileName, const char* pCleanedTraceFile )
{
	LOG_FUNC_ENTRY("CATParseTraceFile::StartParse");

	// Return value, will be changed to true if process start found.
	bool bRet = false;
	bool bCreateCleanedTraces = false;

	// Check pointers
	if ( pFileName == NULL  )
		return bRet;

	if( pOutputFileName == NULL )
		return bRet;

	if ( ! FileExists( pFileName ) )
	{
		cout << AT_MSG << "Error, input file \""
			<< pFileName
			<< "\" does not exist." << endl;
		return bRet;
	}

	ofstream cleanedTraces;

	// check is creation of file needed
	if( pCleanedTraceFile != NULL )
	{
		// if yes open file for cleaned traces
	    // (<AT> messages with cleaned timestamps)
		bCreateCleanedTraces = true;

		cleanedTraces.open(pCleanedTraceFile);

		if( !cleanedTraces.good() )
		{
			printf( "Can not open file: %s\n", pCleanedTraceFile );
			return bRet;
		}
	}

	// Open data file
	ifstream in( pFileName, ios::binary );

	// Check file opened ok
	if ( !in.good() )
		return false;

	// Get stream size
	size_t streamPos = in.tellg();
	in.seekg( 0, ios::end);
	size_t streamEnd = in.tellg();
	in.seekg( 0, ios::beg);

	//Origianl characters (not filtered).
	char cOriginalLineFromFile[MAX_LINE_LENGTH];

	vector<CProcessData> vProcessList;
	int iProcessIDinList = -1;

	bool bFileVersionSaved = false;
	// Read lines
	while( streamPos < streamEnd )
	{
		// Get one line. Don't use stream flags to determinate end of file
		// it can be found too early because trace can contain "anything".
		in.getline( cOriginalLineFromFile, MAX_LINE_LENGTH );
		
		// Refresh position
		streamPos = in.tellg();

		// Check has bad bit flag raised. (i.e. device problems reading data)
		if( in.bad() )
		{
			cout << AT_MSG << "Integrity error reading the trace file, reading aborted." << endl;
			return false;
		}
		//Filtered characters.
		char cLineFromFile[MAX_LINE_LENGTH];
		char* pFiltered = cLineFromFile;

		//Loop thru all characters in original line.
		for( size_t i = 0 ; cOriginalLineFromFile[i] != 0 ; i++ )
		{
			//If character in line is not in invalid character array append it
			//to filtered line.
			if ( strchr( cINVALID_TRACE_FILE_CHARS, cOriginalLineFromFile[i] ) == 0 )
					*pFiltered++ =  cOriginalLineFromFile[i];
		}
		*pFiltered++ = 0; //Add null termination to filtered line.

		if( !bFileVersionSaved && *cLineFromFile != 0 )
		{
			bFileVersionSaved = true;
			m_DataSaver.AddString( AT_DATA_FILE_VERSION );
			m_DataSaver.AddLineToLast();
		}

		// Is there main ID?
		if( strstr( cLineFromFile, MAIN_ID ) != NULL )
		{
			string sRestOfLine( cLineFromFile );
			string sTemp("");
			unsigned __int64 iTimeStamp(0);
			string sTime("");
			string sLineStart("");

			// Get part of line before main id. This should contain time info
			sLineStart = GetStringUntilMainId( sRestOfLine );
			// Get message's time stamp in microseconds
			iTimeStamp = ParseTimeStamp( sLineStart );
			// store whole line from MAIN_ID - to be logged to cleaned traces file
		    string sLineToCleanedFile( sRestOfLine );

			// Get main ID
			sTemp = GetStringUntilNextSpace( sRestOfLine );

			// Is there more data in line?
			if( sRestOfLine.empty() )
			{
				continue;
			}

			// Get next argument
			sTemp = GetStringUntilNextSpace( sRestOfLine );
			// This might be process id, device info message or error message
			if ( sTemp.compare( ERROR_OCCURED ) == 0 )
			{
				// Api mismatch between s60 side and atool.exe
				if ( sRestOfLine.find( INCORRECT_ATOOL_VERSION ) != string::npos )
				{
					cout << "Test run failed because version conflict between device binaries\nand the atool.exe version used to build the application." << endl;
					size_t pS = sRestOfLine.find_first_of('[');
					size_t pE =  sRestOfLine.find_first_of(']');
					size_t pSL = sRestOfLine.find_last_of('[');
					size_t pEL = sRestOfLine.find_last_of(']');
					if ( pS != string::npos && pE != string::npos && pSL != string::npos && pEL != string::npos )
					{
						string deviceVer = sRestOfLine.substr( pS+1, pE-pS-1 );
						string atoolVer = sRestOfLine.substr( pSL+1, pEL-pSL-1 );
						cout << "\tdevice: " << deviceVer << endl 
							<<  "\tatool.exe: " << atoolVer << endl;
					}
				}
				else
					cout << sRestOfLine << endl;
				continue;
			}

			if ( sTemp.compare( LABEL_DEVICE_INFO ) == 0 ) 
			{

				if( vProcessList[iProcessIDinList].bProcessOnGoing == false )
				continue;

				// get time string from timestamp
			    sTime = GetTimeFromTimeStamp( iTimeStamp, vProcessList[iProcessIDinList].iTimeSpan );

				// device info line, log it to cleaned file for carbide
				if( bCreateCleanedTraces )
				{
				// add message to cleaned traces file
				cleanedTraces << sTime << " "; //add time
				cleanedTraces << MAIN_ID << " "; //add MAIN_ID
				cleanedTraces << LABEL_DEVICE_INFO << " "; //add Message type
				cleanedTraces << sRestOfLine << "\n"; //add the rest of the line
				}
				continue;
			}

			unsigned long iProcessID = _httoi( sTemp.c_str() );
			// todo to be removed when reallocations are implemented
			string sProcessID = sTemp;

			iProcessIDinList = -1;
			// Find process from list
			for( unsigned int i = 0 ; i < vProcessList.size() ; i++ )
			{
				if( vProcessList[i].iProcessID == iProcessID )
				{
					iProcessIDinList = i;
					break;
				}
			}
			// Is Process ID found from list?
			if( iProcessIDinList == -1 )
			{
				CProcessData ProcessData;
				ProcessData.bProcessOnGoing = false;
				ProcessData.iProcessID = iProcessID;
				vProcessList.push_back( ProcessData );
				iProcessIDinList = (int)vProcessList.size() - 1;
			}

			// Remove spaces from end of line
			while( sRestOfLine[sRestOfLine.size()-1] == ' ' )
			{
				sRestOfLine.resize( sRestOfLine.size()-1 );
			}

			string sWholeTempLine( sRestOfLine );

			// Get command
			sTemp = GetStringUntilNextSpace( sRestOfLine );

			// Use c style string for easy comparisong of command.
			const char* pCommand = sTemp.c_str();		

			// Process start.
			if( ! _stricmp( pCommand, LABEL_PROCESS_START ) )
			{
				bRet = true; // Set return value true we found start.
				vProcessList[iProcessIDinList].vData.push_back( sWholeTempLine );
				vProcessList[iProcessIDinList].bProcessOnGoing = true;

				// remove <processName> <processID> part
				GetStringUntilNextSpace( sRestOfLine );
				GetStringUntilNextSpace( sRestOfLine );
				// get time
				sTemp = GetStringUntilNextSpace( sRestOfLine );
				unsigned __int64 iTemp(0);
				sscanf_s( sTemp.c_str(), "%016I64x", &iTemp);
				//calculate span between PCS time and PCS timestamp
				vProcessList[iProcessIDinList].iTimeSpan = iTemp - iTimeStamp;

				if( bCreateCleanedTraces )
				{
				// add message to cleaned traces file
				cleanedTraces << sTemp << " "; //add time
				cleanedTraces << MAIN_ID << " "; //add MAIN_ID
				cleanedTraces << sProcessID << " "; //add process ID
				cleanedTraces << sWholeTempLine << "\n"; //add the rest of the line
				}

				continue;
			}

			// Check is process ongoing if not skip other tags.
			if( vProcessList[iProcessIDinList].bProcessOnGoing == false )
				continue;

			// get time string from timestamp
			sTime = GetTimeFromTimeStamp( iTimeStamp, vProcessList[iProcessIDinList].iTimeSpan );

			if( bCreateCleanedTraces )
			{
					cleanedTraces << sTime << " "; //add time
					cleanedTraces << sLineToCleanedFile << "\n"; //add the rest of the line
			}

			// Allocation header
			if ( ! _stricmp( pCommand, ALLOCH_ID ) )
			{
				// Add alloc
				vProcessList[iProcessIDinList].AllocH( sRestOfLine, sTime );

				// Subtests running?
				vector<CSubTestData>::iterator viSubTestIter = vProcessList[iProcessIDinList].vSubTests.begin();
				while( viSubTestIter != vProcessList[iProcessIDinList].vSubTests.end() )
				{
					if( viSubTestIter->bRunning )
					{
						// Save alloc also to sub test
						viSubTestIter->AllocH( sRestOfLine, sTime );
					}
					viSubTestIter++;
				}
			}
			// Allocation fragment (call stack).
			else if ( ! _stricmp( pCommand, ALLOCF_ID ) )
			{
				// Add alloc fragment
				vProcessList[iProcessIDinList].AllocF( sRestOfLine, sTime );
				
				// Subtests running?
				vector<CSubTestData>::iterator viSubTestIter = vProcessList[iProcessIDinList].vSubTests.begin();
				while( viSubTestIter != vProcessList[iProcessIDinList].vSubTests.end() )
				{
					if( viSubTestIter->bRunning )
					{
						// Save alloc fragment also to sub test
						viSubTestIter->AllocF( sRestOfLine, sTime );
					}
					viSubTestIter++;
				}
			}
			//Reallocation header
			else if ( ! _stricmp( pCommand, REALLOCH_ID ) )
			{
				// todo add reallocation flag?

				string sFreeLine = "";
				string sAllocLine = "";

				// Add free

				// get 'free' line from realloc line
				string sFreeRestOfLine = sRestOfLine;

				sFreeLine.append( GetStringUntilNextSpace( sFreeRestOfLine, true ) ); //append freed memory address
				sFreeLine.append( " " );
				// next two strings are for 'alloc' (address and size) - lets remove them
				GetStringUntilNextSpace( sFreeRestOfLine, true );
				GetStringUntilNextSpace( sFreeRestOfLine, true );
				// add rest of line to 'free' line
				sFreeLine.append( sFreeRestOfLine );
				//add 'free' line
				vProcessList[iProcessIDinList].FreeH( sFreeLine, sTime );

				// Add alloc

				//get 'alloc' line from realloc line
				// first string is for 'free' (address), lets remove it first
				GetStringUntilNextSpace( sRestOfLine );
				sAllocLine = sRestOfLine;
     			// add 'alloc' line
				vProcessList[iProcessIDinList].AllocH( sAllocLine, sTime );

				// Subtests running?
				vector<CSubTestData>::iterator viSubTestIter = vProcessList[iProcessIDinList].vSubTests.begin();
				while( viSubTestIter != vProcessList[iProcessIDinList].vSubTests.end() )
				{
					if( viSubTestIter->bRunning )
					{
						// Save realloc also to sub test

						//add 'free' line
						vProcessList[iProcessIDinList].FreeH( sFreeLine, sTime );

     					// add 'alloc' line
						vProcessList[iProcessIDinList].AllocH( sAllocLine, sTime );
					}
					viSubTestIter++;
				}
			}
			// rellocation fragment (call stack)
			else if ( ! _stricmp( pCommand, REALLOCF_ID ) )
			{
				string sFreeLine = "";
				string sAllocLine = "";

				// Not used currently.
				/*
				// Add free fragment 

				// get 'free' line from realloc line
				string sFreeRestOfLine = sRestOfLine;
				sFreeLine.append( GetStringUntilNextSpace( sFreeRestOfLine, true ) ); //append freed memory address
				sFreeLine.append( " " );
				// next string is for 'alloc' (address) - lets remove it
				GetStringUntilNextSpace( sFreeRestOfLine, true );
				// add rest of line to 'free' line
				sFreeLine.append( sFreeRestOfLine );

				//add 'free' line
				vProcessList[iProcessIDinList].FreeF( sFreeLine, sTime );
				*/

				// Add alloc fragment

				// first string is for 'free' (address), lets remove it first
				GetStringUntilNextSpace( sRestOfLine, true );
				sAllocLine = sRestOfLine;
				//add 'alloc' line
				vProcessList[iProcessIDinList].AllocF( sAllocLine, sTime );
				
				// Subtests running?
				vector<CSubTestData>::iterator viSubTestIter = vProcessList[iProcessIDinList].vSubTests.begin();
				while( viSubTestIter != vProcessList[iProcessIDinList].vSubTests.end() )
				{
					if( viSubTestIter->bRunning )
					{
						// Save realloc fragment also to sub test

						// Not used currently.
						/*
						//add 'free' line
						vProcessList[iProcessIDinList].FreeF( sFreeLine, sTime );
						*/

						//add 'alloc' line
						vProcessList[iProcessIDinList].AllocF( sAllocLine, sTime );
					}
					viSubTestIter++;
				}
			}
			// Free header
			else if( ! _stricmp( pCommand, FREEH_ID ) )
			{
				// Send free
				vProcessList[iProcessIDinList].FreeH( sRestOfLine, sTime );

				// Subtests running?
				vector<CSubTestData>::iterator viSubTestIter = vProcessList[iProcessIDinList].vSubTests.begin();
				while( viSubTestIter != vProcessList[iProcessIDinList].vSubTests.end() )
				{
					if( viSubTestIter->bRunning )
					{
						// Send free to subtest
						viSubTestIter->FreeH( sRestOfLine, sTime );
					}
					viSubTestIter++;
				}
			
			}
			// Free fragment
			else if( ! _stricmp( pCommand, FREEF_ID ) )
			{
				// Not used currently.
			}
			// Command process end
			else if( ! _stricmp( pCommand, LABEL_PROCESS_END ) )
			{
				// add processID and time
				sWholeTempLine.append(" ");
				sWholeTempLine.append( sProcessID );
				sWholeTempLine.append(" ");
				sWholeTempLine.append( sTime );

				// Set process has ended.
				vProcessList[iProcessIDinList].bProcessOnGoing = false;

				// Save leaks
				vector<string> vLeaks;
				vector<string>::iterator viLeaks;
				vProcessList[iProcessIDinList].GetLeakList( vLeaks );
				for ( viLeaks = vLeaks.begin(); viLeaks != vLeaks.end(); viLeaks++ )
				{
					sTemp.clear();
					sTemp.append( LABEL_MEM_LEAK );
					sTemp.append( " " );
					sTemp.append( *viLeaks );
					vProcessList[iProcessIDinList].vData.push_back( sTemp );
				}
				vProcessList[iProcessIDinList].ClearAllocs();

				vector<string>::iterator viHandleIter = vProcessList[iProcessIDinList].vHandleLeaks.begin();
				// Print handle leaks
				while( viHandleIter != vProcessList[iProcessIDinList].vHandleLeaks.end() )
				{
					sTemp.clear();
					sTemp.append( viHandleIter->c_str() );
					vProcessList[iProcessIDinList].vData.push_back( sTemp );
					viHandleIter++;
				}
				// Clear handle leaks from list
				vProcessList[iProcessIDinList].vHandleLeaks.clear();

				vector<CSubTestData>::iterator viSubTestIter = vProcessList[iProcessIDinList].vSubTests.begin();
				// Print sub test leaks
				while( viSubTestIter != vProcessList[iProcessIDinList].vSubTests.end() )
				{
					// Print sub test start
					string sLine( LABEL_TEST_START ); sLine.append( " " );
					sLine.append( viSubTestIter->sStartTime ); sLine.append( " " );
					sLine.append( viSubTestIter->sSubTestName ); sLine.append( " " );
					sLine.append( viSubTestIter->sSubTestStartHandleCount );
					vProcessList[iProcessIDinList].vData.push_back( sLine );
					sLine.clear();

					// DLL Loads.
					for( vector<string>::iterator it = viSubTestIter->vData.begin();
						it != viSubTestIter->vData.end(); it++ )
					{
						vProcessList[iProcessIDinList].vData.push_back( (*it) );
					}

					// Subtest leaks.
					vector<string> vSubLeaks;
					vector<string>::iterator viSubLeaks;
					viSubTestIter->GetLeakList( vSubLeaks );
					for ( viSubLeaks = vSubLeaks.begin(); viSubLeaks != vSubLeaks.end(); viSubLeaks++ )
					{
						sLine.append( LABEL_MEM_LEAK );
						sLine.append( " " );
						sLine.append( *viSubLeaks );
						vProcessList[iProcessIDinList].vData.push_back( sLine );
						sLine.clear();
					}
					viSubTestIter->ClearAllocs();

					if( !viSubTestIter->sEndTime.empty() )
					{
						// Print sub test end
						sLine.append( LABEL_TEST_END ); sLine.append( " " );
						sLine.append( viSubTestIter->sEndTime ); sLine.append( " " );
						sLine.append( viSubTestIter->sSubTestName ); sLine.append( " " );
						sLine.append( viSubTestIter->sSubTestEndHandleCount );
						vProcessList[iProcessIDinList].vData.push_back( sLine );
					}
					viSubTestIter++;
				}

				// Clear sub tests from list
				vProcessList[iProcessIDinList].vSubTests.clear();
				vProcessList[iProcessIDinList].vData.push_back( sWholeTempLine );
			}
			// Handle leak
			else if( ! _stricmp( pCommand, LABEL_HANDLE_LEAK ) )
			{
				// Make whole line
				sTemp.append( " " );
				sTemp.append( sRestOfLine );
				vProcessList[iProcessIDinList].vHandleLeaks.push_back( sTemp );
			}
			// Dll load
			else if( ! _stricmp( pCommand, LABEL_DLL_LOAD ) )
			{
				// add time
				sWholeTempLine.append( " " );
				sWholeTempLine.append( sTime );

				// Add module load to process data.
				vProcessList[iProcessIDinList].vData.push_back( sWholeTempLine );
				// Add module load to subtest data if test running.
				for( vector<CSubTestData>::iterator it = vProcessList[iProcessIDinList].vSubTests.begin();
					it != vProcessList[iProcessIDinList].vSubTests.end(); it++ )
				{
					if( it->bRunning )
						it->vData.push_back( sWholeTempLine );
				}

			}
			// Dll unload
			else if( ! _stricmp( pCommand, LABEL_DLL_UNLOAD ) )
			{
				// add time
				sWholeTempLine.append( " " );
				sWholeTempLine.append( sTime );

				// Add module load to process data.
				vProcessList[iProcessIDinList].vData.push_back( sWholeTempLine );
				// Add module unload to subtest data if test running.
				for( vector<CSubTestData>::iterator it = vProcessList[iProcessIDinList].vSubTests.begin();
					it != vProcessList[iProcessIDinList].vSubTests.end(); it++ )
				{
					if( it->bRunning )
						it->vData.push_back( sWholeTempLine );
				}
			}
			else if( sTemp.find( LABEL_LOGGING_CANCELLED ) != string::npos ||
				     sTemp.find( LABEL_PROCESS_END ) != string::npos || sTemp.find( LABEL_ERROR_OCCURED ) != string::npos ||
					 sTemp.find( LABEL_HANDLE_LEAK ) != string::npos )
			{
				vProcessList[iProcessIDinList].vData.push_back( sWholeTempLine );
			}
			// Subtest start
			else if( ! _stricmp( pCommand, LABEL_TEST_START ) )
			{
				bRet = true; // Set return value true we found start.
				// Get sub test time
				string sSubTestTime = GetStringUntilNextSpace( sRestOfLine );
				// Get sub test name
				string sSubTestName = GetStringUntilNextSpace( sRestOfLine );
				// Get sub test start handle count
				string sSubTestStartHandleCount = GetStringUntilNextSpace( sRestOfLine );

				CSubTestData SubTestData;
				SubTestData.bRunning = true;
				SubTestData.sStartTime = sSubTestTime;
				SubTestData.sSubTestName = sSubTestName;
				SubTestData.sSubTestStartHandleCount = sSubTestStartHandleCount.c_str();

				vProcessList[iProcessIDinList].vSubTests.push_back( SubTestData );
			}
			// Subtest end
			else if( ! _stricmp( pCommand, LABEL_TEST_END ) )
			{
				// Get sub test time
				string sSubTestEnd = GetStringUntilNextSpace( sRestOfLine );
				// Get sub test name
				string sSubTestName = GetStringUntilNextSpace( sRestOfLine );
				// Get sub test end handle count
				string sSubTestEndHandleCount = GetStringUntilNextSpace( sRestOfLine );

				// Find subtest
				vector<CSubTestData>::iterator viSubTestIter = vProcessList[iProcessIDinList].vSubTests.begin();
				while( viSubTestIter != vProcessList[iProcessIDinList].vSubTests.end() )
				{
					if( viSubTestIter->sSubTestName == sSubTestName && viSubTestIter->sEndTime.empty() )
					{
						viSubTestIter->sEndTime = sSubTestEnd;
						viSubTestIter->bRunning = false;
						viSubTestIter->sSubTestEndHandleCount = sSubTestEndHandleCount.c_str();
					}
					viSubTestIter++;
				}
			}
			// Thread start
			else if( ! _stricmp( pCommand, LABEL_THREAD_START ) )
			{
				//currently not used
			}
			// Thread end
			else if( ! _stricmp( pCommand, LABEL_THREAD_END ) )
			{
				//currently not used
			}
			else
			{
				// unknown tag, only logged it to cleaned file for carbide
			}
		}
	}

	if( bCreateCleanedTraces )
	{
	// close stream
	cleanedTraces.close();
	}

	// Print all saved data from processes
	for( unsigned int i = 0 ; i < vProcessList.size() ; i++ )
	{
		// Print saved lines
		for( unsigned int iDataCounter = 0 ; iDataCounter < vProcessList[i].vData.size() ; iDataCounter++ )
		{
			m_DataSaver.AddString( vProcessList[i].vData[iDataCounter].c_str() );
			m_DataSaver.AddLineToLast();
		}

		string sTemp;

		// Save leaks
		vector<string> vLeaks;
		vector<string>::iterator viLeaks;
		vProcessList[i].GetLeakList( vLeaks );
		for ( viLeaks = vLeaks.begin(); viLeaks != vLeaks.end(); viLeaks++ )
		{
			sTemp.clear();
			sTemp.append( LABEL_MEM_LEAK );
			sTemp.append( " " );
			sTemp.append( *viLeaks );
			m_DataSaver.AddString( sTemp.c_str() );
			m_DataSaver.AddLineToLast();
		}

		vector<string>::iterator viHandleIter = vProcessList[i].vHandleLeaks.begin();
		// Print handle leaks, if there is data left, there was no process end.
		while( viHandleIter != vProcessList[i].vHandleLeaks.end() )
		{
			sTemp.clear();
			sTemp.append( viHandleIter->c_str() );
			m_DataSaver.AddString( sTemp.c_str() );
			m_DataSaver.AddLineToLast();
			viHandleIter++;
		}
		vector<CSubTestData>::iterator viSubTestIter = vProcessList[i].vSubTests.begin();
		// Print sub test data, if there is data left, there was no process end.
		while( viSubTestIter != vProcessList[i].vSubTests.end() )
		{
			// Print sub test start
			string sLine( LABEL_TEST_START ); sLine.append( " " );
			sLine.append( viSubTestIter->sStartTime ); sLine.append( " " );
			sLine.append( viSubTestIter->sSubTestName ); sLine.append( " " );
			sLine.append( viSubTestIter->sSubTestStartHandleCount );
			m_DataSaver.AddString( sLine.c_str() );
			m_DataSaver.AddLineToLast();
			sLine.clear();

			// DLL Loads.
			for( vector<string>::iterator it = viSubTestIter->vData.begin();
				it != viSubTestIter->vData.end(); it++ )
			{
				m_DataSaver.AddString( (*it).c_str() );
				m_DataSaver.AddLineToLast();
			}

			// Subtest leaks.
			vector<string> vSubLeaks;
			vector<string>::iterator viSubLeaks;
			viSubTestIter->GetLeakList( vSubLeaks );
			for ( viSubLeaks = vSubLeaks.begin(); viSubLeaks != vSubLeaks.end(); viSubLeaks++ )
			{
				sLine.append( LABEL_MEM_LEAK );
				sLine.append( " " );
				sLine.append( *viSubLeaks );
				m_DataSaver.AddString( sLine.c_str() );
				m_DataSaver.AddLineToLast();
				sLine.clear();
			}

			// Print sub test end
			sLine.append( LABEL_TEST_END ); sLine.append( " " );
			sLine.append( viSubTestIter->sEndTime ); sLine.append( " " );
			sLine.append( viSubTestIter->sSubTestName ); sLine.append( " " );
			sLine.append( viSubTestIter->sSubTestEndHandleCount );
			m_DataSaver.AddString( sLine.c_str() );
			m_DataSaver.AddLineToLast();

			viSubTestIter++;
		}
	}
	// Save lines to file.
	m_DataSaver.SaveLinesToFile( pOutputFileName, TEXT_DATA );
	// Close file.
	in.close();
	return bRet;
}

// -----------------------------------------------------------------------------
// CATParseTraceFile::GetDataSaver
// Gets data saver object.
// -----------------------------------------------------------------------------
CATDataSaver* CATParseTraceFile::GetDataSaver(void)	
{
	LOG_LOW_FUNC_ENTRY("CATParseTraceFile::GetDataSaver");
	return &m_DataSaver;
}


// -----------------------------------------------------------------------------
// CATBase::GetTimeFromTimeStamp
// Gets time from timestamp in microseconds as string
// -----------------------------------------------------------------------------
string CATParseTraceFile::GetTimeFromTimeStamp( unsigned __int64 iTimeStamp, unsigned __int64 iTimeSpan )
{
	unsigned __int64 iTime = iTimeStamp + iTimeSpan;
	stringstream ss;
	ss <<  std::hex << iTime;

    return ss.str();
}

//EOF
