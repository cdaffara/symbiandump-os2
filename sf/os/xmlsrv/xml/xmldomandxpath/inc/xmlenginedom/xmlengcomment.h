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
// Comment node functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGCOMMENT_H
#define XMLENGCOMMENT_H

#include <xml/dom/xmlengcharacterdata.h>

/** 
This class represents XML comments in the DOM tree.
 
DOM spec: http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#ID-FF21A306
*/
class TXmlEngComment : public TXmlEngCharacterData
{
public:
    /** Default constructor */
    inline TXmlEngComment();

protected:
    /** 
    Constructor
    @param aInternal Comment pointer
    */
    inline TXmlEngComment(void* aInternal);
};

#include <xml/dom/xmlengcomment.inl>

#endif /* XMLENGCOMMENT_H */

