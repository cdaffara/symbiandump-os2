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
// Node filter interface
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGNODEFILTER_H
#define XMLENGNODEFILTER_H

#include <e32base.h>

class TXmlEngNode;

/** 
Results that can be returned by the node filter
*/
enum TXmlEngNodeFilterResult
    {
	/** Accept the node */
    EAccept = 1,
	/** Skip the node, including the start and end tags and its children */
    EReject = 2,
	/** Skip the node, including the start and end tags, but consider its children */
    ESkip = 3,
	/** Accept the node as an empty node, but skip the end tag and the nodes
	children (Extension to DOM Spec).  */ 
    ESkipContents = 4 
    }; 

/** 
Provides an interface to filter nodes when traversing a tree.
@see http://www.w3.org/TR/2000/REC-DOM-Level-2-Traversal-Range-20001113/traversal.html#Traversal-NodeFilter
@see TXmlEngSerializationOptions
*/
class MXmlEngNodeFilter
    {
public:
    /** 
	Examines the node and determines what action should be taken.  
	@see TXmlEngNodeFilterResult
    
    @param aNode The node to examine
    @return The filter result
    */ 
    virtual TXmlEngNodeFilterResult AcceptNode(TXmlEngNode aNode) = 0;    
    };

#endif /* XMLENGNODEFILTER_H */  

