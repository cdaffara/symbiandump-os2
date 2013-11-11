// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// SAX libxml2 parser plugin inlined definitions
// This method returns pool object. 
//



inline RStringPool& CXMLEngineSAXPlugin::StringPool()
	{
	return iStringPool;
	}

/** 
 * This method returns a user handler object. 
 */			
inline MContentHandler* CXMLEngineSAXPlugin::getContentHandler() const
	{
	return iContentHandler;
	}

/** 
 * This method returns error status. 
 */	
inline TBool CXMLEngineSAXPlugin::IsErrorStatus()
	{
	return Flag_Error_Status;
	}

/** 
 * This method sets error status. 
 */		
inline void CXMLEngineSAXPlugin::SetErrorStatus(TBool aStatus)
	{
	Flag_Error_Status = aStatus;
	}

