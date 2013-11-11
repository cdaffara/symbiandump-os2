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
* Description:  Definitions for the class CATParseXML.
*
*/


#include "../inc/CATParseXML.h"
#include "../inc/catdatasaver.h"
#include "../inc/CATBase.h"

#include <xercesc/parsers/XercesDOMParser.hpp>

const char cCanNotFindEpocroot[] = "Can not find EPOCROOT from devices.xml.\n";
const char cErrorInDeviceXml[] = "Error in devices.xml!\n";
const char cCanNotFind[] = "Can not find file: %s.\n";

CATParseXML::CATParseXML(void)
{
	LOG_FUNC_ENTRY("CATParseXML::CATParseXML");
	try 
	{
		xercesc::XMLPlatformUtils::Initialize();
	}
	catch ( ... )
	{
		//Print error
		printf("XML initialization failed.\n");
	}
}

CATParseXML::~CATParseXML(void)
{
	LOG_FUNC_ENTRY("CATParseXML::~CATParseXML");
}

// -----------------------------------------------------------------------------
// CATParseXML::GetEpocRootPathFromXML
// Find epocroot path in xml file
// -----------------------------------------------------------------------------
string CATParseXML::GetEpocRootPathFromXML(const char* pSourcePath)
{
	LOG_FUNC_ENTRY("CATParseXML::GetEpocRootPathFromXML");
	string sEpocRootPath;
	// Check that source exists
	if ( ! CATBase::FileExists( pSourcePath ) )
	{
		LOG_STRING( "Source xml not found." );
		return sEpocRootPath;
	}
	try 
	{
		xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
		xercesc::DOMDocument* pDomDoc;

		// Get devices.xml document
		parser->parse( pSourcePath );
		pDomDoc = parser->getDocument();

		// Get root element
		xercesc::DOMElement* pRootElem = pDomDoc->getDocumentElement();

		if( pRootElem )
		{
			// Get all "device" elements
			LPWSTR wTemp = CATDataSaver::CharToWChar( "device" );
			xercesc::DOMNodeList* pDeviceNodeList = pRootElem->getElementsByTagName( wTemp );
			if( wTemp )
				delete[] wTemp;

			// Find default SDK

			bool bEpocRootFound = false;
			int iLength = pDeviceNodeList->getLength();
			for( int i = 0 ; i < iLength ; i++ )
			{
				xercesc::DOMNode* pDeviceNode = pDeviceNodeList->item(i);

				xercesc::DOMNamedNodeMap* pAttributeList = pDeviceNode->getAttributes();

				// Find attribute "default"

				int iAttribListLength = pAttributeList->getLength();
				for( int x = 0 ; x < iAttribListLength ; x++ )
				{
					xercesc::DOMNode* pAttribNode = pAttributeList->item(x);
					const LPWSTR pNodeName = (const LPWSTR)pAttribNode->getNodeName();

					if( wcscmp( pNodeName, L"default" ) == 0 )
					{
						const LPWSTR pNodeValue = (const LPWSTR)pAttribNode->getNodeValue();
						
						// Find node value 'yes'
						if( wcscmp( pNodeValue, L"yes" ) == 0 )
						{
							// Find <epocroot> node
							xercesc::DOMNode* pChildNode = pDeviceNode->getFirstChild();
							if( !pChildNode )
								break;
							while( !bEpocRootFound )
							{
								if( wcscmp( pChildNode->getNodeName() , L"epocroot" ) == 0 )
								{
									bEpocRootFound = true;

									// Node value is child text node
									xercesc::DOMNode* pTempTextNode = pChildNode->getFirstChild();
									const LPWSTR pPathNodeValue = (const LPWSTR)pTempTextNode->getNodeValue();

									CATDataSaver::WCharToChar( sEpocRootPath, pPathNodeValue );

									break;
								}
								pChildNode = pChildNode->getNextSibling();
								if( !pChildNode )
									break;
							}
						} // If node value yes
					} // If node name default
					if( bEpocRootFound )
						break;
				} // for x
				if( bEpocRootFound )
					break;
			} // for i
		}
		if(parser)
			delete parser; //lint !e118
 		xercesc::XMLPlatformUtils::Terminate();
	}
    catch (...)
	{
        printf("XML parsing failed.");
    }
	return sEpocRootPath;
}
// End of file
