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
// Node list implementation functions
//

#include <xml/dom/xmlengnodelist.h>
#include "xmlengdomdefs.h"
#include <stdapis/libxml2/libxml2_tree.h>
#include <xml/utils/xmlengutils.h>

#define IS_NAME_MATCHED(type) (type & 0x10)
#define MATCH_TYPE(type)      (type & 0x0F)


/**
*  Default constructor
*/
EXPORT_C RXmlEngNodeListImpl::RXmlEngNodeListImpl()
    {
    iCurrentNode = NULL; 
	iType	 = 0;
	iName    = NULL;
    iNsUri   = NULL;
    }

// Finds next node
// 
// @param aMatch Current node
// @return Next node 
// -------------------------------------------------------------------------------
//
void* RXmlEngNodeListImpl::FindNextNode(void* aCurrentNode) const
    {
    const void* node_href;

    xmlNodePtr node = (xmlNodePtr) aCurrentNode;
    TInt nodeType  = MATCH_TYPE(iType); // mask NodeType bits only

    if(nodeType)
        {
        // any node is not suitable --> search at least by type
        TInt matchName = IS_NAME_MATCHED(iType); // mask bit with matchName flag

        while (node)
            {
            if(node->type == nodeType)
                {
                if(!matchName)
                    {
                    break; // type matches;  no need to aMatch name/URI
                    }
                // continue matching name/URI

                // NOTE: if NodeType is ANY then names/URIs are not checked
                if (StrEqualOrNull(iName, node->name))
                    {
                    // Internally node->ns->href is "" OR node->ns is NULL
                    //      stand for undefined namespace of the node
                    // but both NULL and "" are allowed as namespace URI criteria ("" is converted to NULL)
                    node_href = node->ns && node->ns->href && node->ns->href[0]
                                ? node->ns->href
                                : NULL;
                    if (StrEqualOrNull(iNsUri, node_href))
                        {
                        break; // Matching node with  type/name/URI is found
                        }
                    }
                }
            // current node does not fulfil criteria
            node = node->next;
            }
        }
    return node;
    }

/**
@return Number of nodes in a node list
*/
EXPORT_C TInt RXmlEngNodeListImpl::Count() const
    {
    // Not really efficient, but it will be used probably only once
    // (eg. to know how much memory to allocate)
    xmlNodePtr start = (xmlNodePtr) iCurrentNode;
    TInt cnt = 0;
    xmlNodePtr theNext;
    theNext = (xmlNodePtr) FindNextNode( start );
	while (theNext)
        {
        cnt++;
        theNext = theNext->next;
        theNext = (xmlNodePtr) FindNextNode( theNext );
        }
    return cnt;
    }

/**
Checks whether next node exists in a node list

@return TRUE if next node exists, FALSE otherwise
*/
EXPORT_C TBool RXmlEngNodeListImpl::HasNext() const
{
    xmlNodePtr node = (xmlNodePtr) FindNextNode( iCurrentNode );
    return ((TBool)node);
}

/**
Retrieves next node from a node list

@return Next node 
*/
EXPORT_C TXmlEngNode RXmlEngNodeListImpl::Next()
    {
    xmlNodePtr valid = (xmlNodePtr) FindNextNode( iCurrentNode );
    iCurrentNode = valid
                        ? valid->next
                        : NULL;
    return TXmlEngNode(valid);
    }
    
// -----------------------------------------------------------------------------
// Initializes node list
//
// @note If aType is NULL, aName and aNs are ignored
// -----------------------------------------------------------------------------
//
void RXmlEngNodeListImpl::OpenL(
			void* aHead, 
			TXmlEngNode::TXmlEngDOMNodeType aType,
			const TDesC8& aName,
			const TDesC8& aNs)
    {
	iCurrentNode = aHead;
	iType	 = aName.Length() ? aType | 0x10 : aType;
	iName    = (unsigned char*) XmlEngXmlCharFromDes8L( aName );
	iNsUri = NULL;
	if(aNs.Length())
        	{
        	unsigned char*	tmp = iName;
        	CleanupStack::PushL(tmp);
         	iNsUri = (unsigned char*) XmlEngXmlCharFromDes8L( aNs );
         	CleanupStack::Pop(tmp);
         	}        
    }  

// -----------------------------------------------------------------------------
// Initializes node list
//
// @note If aType is NULL, aName and aNs are ignored
// -----------------------------------------------------------------------------
//
void RXmlEngNodeListImpl::Open(
			void* aHead, 
			TXmlEngNode::TXmlEngDOMNodeType aType)
    {
	iCurrentNode = aHead; 
	iType	 = aType;
    } 

EXPORT_C void RXmlEngNodeListImpl::Close()
    {
    delete[] iName;
    delete[] iNsUri;
    }
    
// -----------------------------------------------------------------------------
// Compares two strings
// -----------------------------------------------------------------------------
//
TBool StrEqualOrNull(const void* aStr1, const void* aStr2)
{
	if (aStr1 && aStr2)
		return !strcmp((char*)aStr1, (char*)aStr2);
	return !aStr1 && !aStr2;
}

