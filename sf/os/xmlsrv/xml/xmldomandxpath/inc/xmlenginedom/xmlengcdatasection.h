// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CDATASection node functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGCDATASECTION_H
#define XMLENGCDATASECTION_H

#include <xml/dom/xmlengtext.h>

/** 
This class represents a XML CDATASection in the DOM tree.
*/
class TXmlEngCDATASection : public TXmlEngTextNode
{
public:
    /** Default constructor */
	inline TXmlEngCDATASection();
    
protected:
    /** 
    Constructor
    @param aInternal CDATASection pointer
    */
	inline TXmlEngCDATASection(void* aInternal);
};



#include <xml/dom/xmlengcdatasection.inl>

#endif /* XMLENGCDATASECTION_H */

