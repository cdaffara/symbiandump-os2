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
* Description:  Definitions for the class CATDataSaver.
*
*/


#include "../inc/catdatasaver.h"

#include <xercesc/util/OutOfMemoryException.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif

// Line feed char sequence used in XML report
wchar_t AT_XML_LINEFEEDS[3] = L"\r\n";

// -----------------------------------------------------------------------------
// CATDataSaver::CATDataSaver
// Constructor.
// -----------------------------------------------------------------------------
CATDataSaver::CATDataSaver( void )
{
	LOG_FUNC_ENTRY("CATDataSaver::CATDataSaver");
	m_iLoggingLevel = DEFAULT_LOGGING_LEVEL;
	m_bPrintImmediately = true;
	m_bXMLInitOk = false;
	m_bUdebBuild = true;

	m_iRunNumber = 1;

	m_pDomDoc = NULL;
	m_pRootElem = NULL;
	m_Serializer = NULL;
	m_pCurrentLeakElem = NULL;
	m_pRunElement = NULL;
	m_pMemoryLeaks = NULL;
	m_pHandleLeaks = NULL;
	m_pCurrentSubTestElem = NULL;
	m_pSubtestMemoryLeaks = NULL;
}

// -----------------------------------------------------------------------------
// CATDataSaver::~CATDataSaver
// Destructor.
// -----------------------------------------------------------------------------
CATDataSaver::~CATDataSaver(void)
{
	LOG_FUNC_ENTRY("CATDataSaver::~CATDataSaver");
	if( m_bXMLInitOk )
	{
		if( m_Serializer )
			delete m_Serializer;

		m_pDomDoc->release();
		xercesc::XMLPlatformUtils::Terminate();
	}
}

// -----------------------------------------------------------------------------
// CATDataSaver::SaveLinesToFile
// Gets logging level.
// -----------------------------------------------------------------------------
void CATDataSaver::SaveLinesToFile( const char* pFileName, int iDataToSave )
{
	LOG_FUNC_ENTRY("CATDataSaver::SaveLinesToFile");

	// Nothing to print?
	if( m_vLines.empty() )
	{
		printf( "No output data." );
		return;
	}
	if( iDataToSave != XML_DATA )
	{
		ofstream out( pFileName );

		if( !out.good() )
		{
			printf( "Can not open file: %s\n", pFileName );
			return;
		}
		switch( iDataToSave )
		{
			case TEXT_DATA:
				for( int i = 0 ; i < (int)m_vLines.size() ; i++ )
				{
					out << m_vLines[i].c_str();
				}
			break;
		}
		out.close();
	}
	else
	{
		if( m_bXMLInitOk )
		{
			xercesc::XMLFormatTarget* myFormTarget = NULL;
			try
			{
				// Create format
				myFormTarget = new xercesc::LocalFileFormatTarget( pFileName );
				
				// Set line-feeds to dom writer
				m_Serializer->setNewLine( AT_XML_LINEFEEDS );
				
				// Set human-readable property. Note! Api already changed in >2.7
				// so this will cause error/problems if linked to newer library.
				m_Serializer->setFeature( xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true );
				
				// Write document
				m_Serializer->writeNode(myFormTarget, *m_pDomDoc);
			}
			catch(...)
			{
				printf( "Can not save output file: %s.", pFileName );
			}
			if( myFormTarget )
				delete myFormTarget; //lint !e118
		}
	}
}

// -----------------------------------------------------------------------------
// CATDataSaver::PrintLinesToScreen
// Prints all saved lines to screen.
// -----------------------------------------------------------------------------
void CATDataSaver::PrintLinesToScreen( void )
{
	LOG_FUNC_ENTRY("CATDataSaver::PrintLinesToScreen");
	// Nothing to print?
	if( m_vLines.empty() )
	{
		printf( "No output data." );
		return;
	}
	for( int i = 0 ; i < (int)m_vLines.size() ; i++ )
	{
		printf( m_vLines[i].c_str() );	
	}
}

// -----------------------------------------------------------------------------
// CATDataSaver::AddLineToFirst
// Adds saved line to first in database.
// -----------------------------------------------------------------------------
void CATDataSaver::AddLineToFirst( void )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::AddLineToFirst");
	m_sLine.append( "\n" );
	m_vLines.insert( m_vLines.begin(), m_sLine );
	m_sLine.clear();
}

// -----------------------------------------------------------------------------
// CATDataSaver::AddLineToLast
// Adds saved line to last in database.
// -----------------------------------------------------------------------------
void CATDataSaver::AddLineToLast()
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::AddLineToLast");
	m_sLine.append( "\n" );
	
	string sTempDataLine;

	m_vLines.push_back( m_sLine );

	SaveXML( m_sCarbideDataLine, ITEM );

	if( m_bPrintImmediately )
	{
		printf( m_sLine.c_str() );
	}

	m_sCarbideDataLine.clear();
	m_sLine.clear();
}

// -----------------------------------------------------------------------------
// CATDataSaver::AddString
// Adds string to current line.
// -----------------------------------------------------------------------------
void CATDataSaver::AddString( const char* pData, bool bSaveCarbideData )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::AddString");
	m_sLine.append( pData );

	if( bSaveCarbideData )
	{
		m_sCarbideDataLine.append( pData );
		m_sCarbideDataLine.append(";");
	}
}

// -----------------------------------------------------------------------------
// CATDataSaver::AddInteger
// Converts integer to string and adds it to current line.
// -----------------------------------------------------------------------------
void CATDataSaver::AddInteger( int iValue, bool bSaveCarbideData )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::AddInteger");
	char cTemp[128];
	string sValue( itoa( iValue, cTemp, 10 ) );
	m_sLine.append( sValue );

	if( bSaveCarbideData )
	{
		m_sCarbideDataLine.append( sValue );
		m_sCarbideDataLine.append(";");
	}
}

// -----------------------------------------------------------------------------
// CATDataSaver::SetLoggingLevel
// Sets logging level.
// -----------------------------------------------------------------------------
void CATDataSaver::SetLoggingLevel( int iLoggingLevel )
{
	LOG_FUNC_ENTRY("CATDataSaver::SetLoggingLevel");
	// Check that new logging level is valid 
	// Acceptable values are between MIN_LOGGING_LEVEL and 
    // MAX_LOGGING_LEVEL including them
	if( iLoggingLevel >= MIN_LOGGING_LEVEL && iLoggingLevel <= MAX_LOGGING_LEVEL )
	{
		m_iLoggingLevel = iLoggingLevel;
	}
	else
	{
		// New logging level value is invalid => set default logging level
		m_iLoggingLevel = DEFAULT_LOGGING_LEVEL;
	}
}

// -----------------------------------------------------------------------------
// CATDataSaver::GetLoggingLevel
// Gets logging level.
// -----------------------------------------------------------------------------
int CATDataSaver::GetLoggingLevel( void )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::GetLoggingLevel");
	return m_iLoggingLevel;
}

// -----------------------------------------------------------------------------
// CATDataSaver::SetPrintFlag
// Sets print immediately flag.
// -----------------------------------------------------------------------------
void CATDataSaver::SetPrintFlag( bool bPrintImmediately )
{
	LOG_FUNC_ENTRY("CATDataSaver::SetPrintFlag");
	m_bPrintImmediately = bPrintImmediately;
}

// -----------------------------------------------------------------------------
// CATDataSaver::SaveCarbideDataHeader
// Sets data header for Carbide data.
// -----------------------------------------------------------------------------
void CATDataSaver::SaveCarbideDataHeader( void )
{
	LOG_FUNC_ENTRY("CATDataSaver::SaveCarbideDataHeader");
	SaveXML( m_sCarbideDataLine, LEAK );
	m_sCarbideDataLine.clear();
}

// -----------------------------------------------------------------------------
// CATDataSaver::InitXML
// Initializes xerces xml parser.
// -----------------------------------------------------------------------------
bool CATDataSaver::InitXML( void )
{
	LOG_FUNC_ENTRY("CATDataSaver::InitXML");
	try 
	{
		xercesc::XMLPlatformUtils::Initialize();
	}
	catch ( ... )//(const XMLException& toCatch) 
	{
		// Do your failure processing here
		printf("XML initialization failed.\n");
		return false;
	}
	// Error code.
	int errorCode = 0;
	// getDomIMplementation returns null if source has none.
	xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(L"Core");
	if (impl != NULL)
    {
		// Create new DOMWriter.
		m_Serializer = ((xercesc::DOMImplementationLS*)impl)->createDOMWriter();
		// New document.
        try
        {
			m_pDomDoc = impl->createDocument(
                        0,                    // Root element namespace URI.
                        L"results",         // Root element name
                        0);                   // Document type object (DTD).

            m_pRootElem = m_pDomDoc->getDocumentElement();
        }
        catch (const xercesc::OutOfMemoryException&)
        {
            XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
            errorCode = 5;
        }
        catch (const xercesc::DOMException& e)
        {
            XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
            errorCode = 2;
        }
        catch (...)
        {
            XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
            errorCode = 3;
        }
    }  // (inpl != NULL)
    else
    {
        XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }
	if( !errorCode )
	{
		m_bXMLInitOk = true;
		return true;
	}
	else
	{
		return false;
	}
}

// -----------------------------------------------------------------------------
// CATDataSaver::WCharToChar
// Converts wchar_t* -> char*.
// -----------------------------------------------------------------------------
void CATDataSaver::WCharToChar( string& sInput, const WCHAR* Source )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::WCharToChar");
	if( !Source )
		return;
    int i = 0;

    while( Source[i] != '\0' )
    {
		char c = (CHAR)Source[i];
		sInput.append( &c, 1 );
        ++i;
    }
}

// -----------------------------------------------------------------------------
// CATDataSaver::CharToWChar
// Converts char* -> wchar_t*.
// -----------------------------------------------------------------------------
LPWSTR CATDataSaver::CharToWChar( const char* str )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::CharToWChar");
    LPWSTR out = NULL;
    if( str != NULL )
    {
        int in_len = (int)strlen( str );
        int out_len = MultiByteToWideChar(CP_ACP, 0, str, in_len, NULL, 0) + 2;
        out = new WCHAR[out_len];

        if (out)
        {
            memset(out, 0x00, sizeof(WCHAR)*out_len);
            MultiByteToWideChar(CP_ACP, 0, str, in_len, out, in_len);
        }
    }
    return out;
}

// -----------------------------------------------------------------------------
// CATDataSaver::SaveXML
// Writes data to xml tree.
// -----------------------------------------------------------------------------
void CATDataSaver::SaveXML( string sInput, int iElementType )
{
	LOG_FUNC_ENTRY("CATDataSaver::SaveXML");
	// Variables ok?
	if( sInput.empty() || m_pDomDoc == NULL )
	{
		return;
	}
	try
	{
		switch( iElementType )
		{
			case RESULT:
			{
				// Print number of runs
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				m_pRootElem->setAttribute( L"runs", (const LPWSTR)wTemp );

				// Print failed runs
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );
				m_pRootElem->setAttribute( L"failed", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case RUN:
			{
				if( m_pRootElem == NULL )
					return;
				xercesc::DOMElement* runElem = m_pDomDoc->createElement( L"run" );
				m_pRootElem->appendChild( runElem );

				// Reset handle leaks.
				m_pHandleLeaks = NULL;

				// Print start time
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				runElem->setAttribute( L"start_time", (const LPWSTR)wTemp );
				runElem->setAttribute( L"end_time", NULL );
				if( wTemp )
					delete[] wTemp;

				// Print build target
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				wTemp = CharToWChar( sTemp.c_str() );
				runElem->setAttribute( L"build_target", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;

				// Print process name
				wTemp = CharToWChar( sInput.c_str() );
				runElem->setAttribute( L"process_name", (const LPWSTR)wTemp );

				m_pRunElement = runElem;

				char cTemp[128];
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( itoa( m_iRunNumber, cTemp, 10 ) );
				runElem->setAttribute( L"id", (const LPWSTR)wTemp );
				m_iRunNumber++;
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case LEAK:
			{
				m_pCurrentLeakElem = m_pDomDoc->createElement( L"leak" );


				if( m_pCurrentLeakElem == NULL || m_pRunElement == NULL )
					return;

				// Sub test?
				if( m_pCurrentSubTestElem )
					m_pCurrentSubTestElem->appendChild( m_pCurrentLeakElem );
				else
					m_pRunElement->appendChild( m_pCurrentLeakElem );

				// Print leak ID
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				m_pCurrentLeakElem->setAttribute( L"id", (const LPWSTR)wTemp );

				// Print leak size
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );
				m_pCurrentLeakElem->setAttribute( L"size", (const LPWSTR)wTemp );

				// Print leak address
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );
				m_pCurrentLeakElem->setAttribute( L"memaddress", (const LPWSTR)wTemp );

				// Print leak time
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );
				m_pCurrentLeakElem->setAttribute( L"time", (const LPWSTR)wTemp );

				// Print leak module
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );
				m_pCurrentLeakElem->setAttribute( L"module", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case ITEM:
			{
				xercesc::DOMNode* callstackNode = NULL;

				xercesc::DOMElement* callstackElem = NULL;

				if( m_pCurrentLeakElem  == NULL )
					return;

				// Print module name
				if( !m_pCurrentLeakElem->hasChildNodes() )
				{
					callstackElem = m_pDomDoc->createElement( L"callstack" );
					m_pCurrentLeakElem->appendChild( callstackElem );
					callstackNode = callstackElem;
				}
				else
				{
					callstackNode = m_pCurrentLeakElem->getFirstChild();
				}

				// Add callstack item
				xercesc::DOMElement* itemElem = m_pDomDoc->createElement( L"item" );
				callstackNode->appendChild( itemElem );

				// Print memory address name
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );

				itemElem->setAttribute( L"memaddress", (const LPWSTR)wTemp );

				// Print calculated memory address
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );

				itemElem->setAttribute( L"calc_addr", (const LPWSTR)wTemp );

				// Print module name
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );

				itemElem->setAttribute( L"module", (const LPWSTR)wTemp );

				// Print function name
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );

				itemElem->setAttribute( L"function", (const LPWSTR)wTemp );

				sTemp = GetStringUntilNextGivenChar( sInput, ';' );

				// Print function line from urel build
				if( !m_bUdebBuild )
				{
					if( wTemp )
						delete[] wTemp;
					wTemp = CharToWChar( sTemp.c_str() );
					itemElem->setAttribute( L"function_line", (const LPWSTR)wTemp );
					sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				}

				// Print file name
				if( wTemp )
					delete[] wTemp;
				// Erase if path found from sTemp.
				if ( sTemp.rfind( "/" ) != string::npos )
				{
					sTemp.erase(0, sTemp.rfind( "/" )+1 );
				}
				if ( sTemp.rfind( "\\" ) != string::npos )
				{
					sTemp.erase(0, sTemp.rfind( "\\" )+1 );
				}
				wTemp = CharToWChar( sTemp.c_str() );

				itemElem->setAttribute( L"file", (const LPWSTR)wTemp );

				// Print line of file
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );

				if( m_bUdebBuild )
					itemElem->setAttribute( L"line", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case RUN_END:
			{
				if( m_pRunElement == NULL )
					return;
				const LPWSTR wTemp = CharToWChar( sInput.c_str() );
				m_pRunElement->setAttribute( L"end_time", wTemp );
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case ERROR_IN_RUN:
			{
				if( m_pRunElement == NULL )
					return;
				// Add error item
				xercesc::DOMElement* errorElem = m_pDomDoc->createElement( L"error" );
				m_pRunElement->appendChild( errorElem );

				// Print error code
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				errorElem->setAttribute( L"code", (const LPWSTR)wTemp );

				// Print error time
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				if( wTemp )
					delete[] wTemp;
				wTemp = CharToWChar( sTemp.c_str() );
				errorElem->setAttribute( L"time", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case MEM_LEAKS:
			{
				if( m_pRunElement == NULL )
					return;
				xercesc::DOMElement* memoryLeaksElement = m_pDomDoc->createElement( L"mem_leaks" );
				m_pRunElement->appendChild( memoryLeaksElement );
				m_pMemoryLeaks = memoryLeaksElement;

				// Print number of leaks
				LPWSTR wTemp = CharToWChar( sInput.c_str() );
				memoryLeaksElement->setAttribute( L"count", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case MEM_LEAK_MODULE:
			{
				if( m_pMemoryLeaks == NULL )
					return;
				xercesc::DOMElement* moduleElement = m_pDomDoc->createElement( L"module" );
				m_pMemoryLeaks->appendChild( moduleElement );

				// Print module name
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				moduleElement->setAttribute( L"name", (const LPWSTR)wTemp );

				if( wTemp )
					delete[] wTemp;
				// Print number of memory leaks
				wTemp = CharToWChar( sInput.c_str() );
				moduleElement->setAttribute( L"leaks", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case HANDLE_LEAKS:
			{
				if( m_pRunElement == NULL )
					return;
				if( m_pHandleLeaks )
				{
					// Update number of leaks
					LPWSTR wTemp = CharToWChar( sInput.c_str() );
					m_pHandleLeaks->setAttribute( L"count", (const LPWSTR)wTemp );
					if( wTemp )
						delete[] wTemp;
				}
				else
				{
					xercesc::DOMElement* handleLeaksElement = m_pDomDoc->createElement( L"handle_leaks" );
					m_pRunElement->appendChild( handleLeaksElement );
					m_pHandleLeaks = handleLeaksElement;

					// Print number of leaks
					LPWSTR wTemp = CharToWChar( sInput.c_str() );
					handleLeaksElement->setAttribute( L"count", (const LPWSTR)wTemp );
					if( wTemp )
						delete[] wTemp;
				}
			}
			break;
			case HANDLE_LEAK_MODULE:
			{
				if( m_pHandleLeaks == NULL )
					return;
				xercesc::DOMElement* moduleElement = m_pDomDoc->createElement( L"module" );
				m_pHandleLeaks->appendChild( moduleElement );

				// Print module name
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				moduleElement->setAttribute( L"name", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;

				// Print number of memory leaks
				wTemp = CharToWChar( sInput.c_str() );
				moduleElement->setAttribute( L"leaks", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
			}
			break;
			case TEST_START:
			{
				m_pCurrentSubTestElem = m_pDomDoc->createElement( L"subtest" );

				if( m_pCurrentSubTestElem == NULL || m_pRunElement == NULL )
					return;

				m_pRunElement->appendChild( m_pCurrentSubTestElem );

				// Print sub test name
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				m_pCurrentSubTestElem->setAttribute( L"name", (const LPWSTR)wTemp );
				if( wTemp )
				{
					delete[] wTemp;
					wTemp = NULL;
				}

				// Print sub test time
				sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				wTemp = CharToWChar( sTemp.c_str() );
				m_pCurrentSubTestElem->setAttribute( L"start_time", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
				break;
			}
			case TEST_END:
			{
				if( m_pCurrentSubTestElem == NULL )
					return;
				// Print end time
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				m_pCurrentSubTestElem->setAttribute( L"end_time", (const LPWSTR)wTemp );
				m_pCurrentSubTestElem = NULL;
				if( wTemp )
					delete[] wTemp;
				break;
			}
			case SUBTEST_MEM_LEAKS:
			{
				if( m_pCurrentSubTestElem == NULL )
					return;
				xercesc::DOMElement* memoryLeaksElement = m_pDomDoc->createElement( L"mem_leaks" );
				m_pCurrentSubTestElem->appendChild( memoryLeaksElement );
				m_pSubtestMemoryLeaks = memoryLeaksElement;

				// Print number of leaks
				LPWSTR wTemp = CharToWChar( sInput.c_str() );
				memoryLeaksElement->setAttribute( L"count", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
				break;
			}
			case SUBTEST_MEM_LEAK_MODULE:
			{
				if( m_pSubtestMemoryLeaks == NULL )
					return;
				xercesc::DOMElement* moduleElement = m_pDomDoc->createElement( L"module" );
				m_pSubtestMemoryLeaks->appendChild( moduleElement );

				// Print module name
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				moduleElement->setAttribute( L"name", (const LPWSTR)wTemp );

				if( wTemp )
					delete[] wTemp;
				// Print number of memory leaks
				wTemp = CharToWChar( sInput.c_str() );
				moduleElement->setAttribute( L"leaks", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
				break;
			}
			case SUBTEST_HANDLE_LEAKS:
			{
				if( m_pCurrentSubTestElem == NULL )
					return;
				xercesc::DOMElement* handleLeaksElement = m_pDomDoc->createElement( L"handle_leaks" );
				m_pCurrentSubTestElem->appendChild( handleLeaksElement );

				//Print number of handle leaks
				string sTemp = GetStringUntilNextGivenChar( sInput, ';' );
				LPWSTR wTemp = CharToWChar( sTemp.c_str() );
				handleLeaksElement->setAttribute( L"count", (const LPWSTR)wTemp );
				if( wTemp )
					delete[] wTemp;
				break;
			}
			default:
			break;
		}
	}
	catch( ... )
	{
		printf( "Error when writing data to XML file." );
	}
}

// -----------------------------------------------------------------------------
// CATDataSaver::GetStringUntilNextGivenChar
// Function returns string from begin of given string until next given char,
// characters until given char are removed from sInput string.
// -----------------------------------------------------------------------------
string CATDataSaver::GetStringUntilNextGivenChar( string& sInput, char cCharacter )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::GetStringUntilNextGivenChar");
	string sRet;
	size_t iPos = sInput.find( cCharacter );
	if( sInput.size() > 1 && iPos != string::npos )
	{
		sRet = sInput.substr( 0, iPos );
		sInput.erase( 0, (iPos + 1) );
	}
	return sRet;
}

// -----------------------------------------------------------------------------
// CATDataSaver::SetBuild
// Function sets build target info.
// -----------------------------------------------------------------------------
void CATDataSaver::SetBuild( bool bUdebBuild )
{
	LOG_FUNC_ENTRY("CATDataSaver::SetBuild");
	m_bUdebBuild = bUdebBuild;
}

// -----------------------------------------------------------------------------
// CATDataSaver::AddCarbideData
// Function adds string to Carbide data.
// -----------------------------------------------------------------------------
void CATDataSaver::AddCarbideData( const string& sInput )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::AddCarbideData");
	m_sCarbideDataLine.append( sInput );
	m_sCarbideDataLine.append(";");
}

// -----------------------------------------------------------------------------
// CATDataSaver::IntegerToString
// Converts integer to string.
// -----------------------------------------------------------------------------
string CATDataSaver::IntegerToString( int iValueToConvert )
{
	LOG_LOW_FUNC_ENTRY("CATDataSaver::IntegerToString");
	char cTemp[128];
	string sValue( itoa( iValueToConvert, cTemp, 10 ) );
	return sValue;
}
