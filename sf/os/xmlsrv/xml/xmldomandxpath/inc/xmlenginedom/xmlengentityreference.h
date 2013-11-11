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
// Entity reference node functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGENTITYREFERENCE_H
#define XMLENGENTITYREFERENCE_H

#include <xml/dom/xmlengnode.h>

/** 
This class represents an XML entity reference in the DOM tree.
*/
class TXmlEngEntityReference : public TXmlEngNode
{
public:
	/** Default constructor */
	inline TXmlEngEntityReference();

protected:
	/** 
    Constructor
    @param aInternal Entity reference pointer
    */
	inline TXmlEngEntityReference(void* aInternal);
};

#include <xml/dom/xmlengentityreference.inl>

#endif /* XMLENGENTITYREFERENCE_H */

