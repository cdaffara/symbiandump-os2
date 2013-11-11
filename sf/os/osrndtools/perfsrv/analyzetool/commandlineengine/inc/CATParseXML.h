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
* Description:  Declarations for the class CATParseXML.
*
*/


#ifndef __CATPARSEXML_H__
#define __CATPARSEXML_H__

#include "../inc/ATCommonDefines.h"

/**
* CATParseXML is used to parse xml data.
*/
class CATParseXML
{
public:
	/**
	* Constructor
	*/
	CATParseXML(void);
	
	/**
    * Destructor
    */
	virtual ~CATParseXML(void);

	/**
	* Find epocroot path in xml file
	* @param sourcePath Location of the xml file
	*/
	string GetEpocRootPathFromXML(const char* sourcePath);
};
#endif
