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
// Document & node callbacks
//

#include <stdapis/libxml2/xmlengconfig.h>
#include <xml/utils/xmlengmem.h>
#include <xml/dom/xmlenguserdata.h>
#include <xml/dom/xmlengdocument.h>
#include "xmlengdomdefs.h"
#include "xmlengownednodescontainer.h"
#include <xml/dom/xmlengnode.h>
#include <xml/dom/xmlengnodefilter.h>
#include <stdapis/libxml2/libxml2_tree.h>
#include "libxml2_globals_private.h"
#include <string.h>


void NodeConstructCallback(
    xmlNodePtr aNode)
    {
    // NOTE for further evaluation of component _private is not copied by libxml
    if (aNode && aNode->_private) 
        {
        if (aNode->type == XML_ELEMENT_NODE || aNode->type == XML_ATTRIBUTE_NODE) 
            {
            MXmlEngUserData* data = static_cast<MXmlEngUserData*>(aNode->_private);
            TInt err;
            TRAP(err,aNode->_private = data->CloneL());
            if(err == KErrNoMemory)
            	xmlSetOOM();
            }
        }
    }

void NodeDestructCallback(
    xmlNodePtr aNode )
    {
    if (aNode && aNode->_private) 
        {
        if (aNode->type == XML_ELEMENT_NODE || aNode->type == XML_ATTRIBUTE_NODE) 
            {
            MXmlEngUserData* data = static_cast<MXmlEngUserData*>(aNode->_private);
            aNode->_private = NULL;
            data->Destroy();
            }
        }
    }

int XMLCALL NodeFilterCallback(void* aFilter, xmlNodePtr aNode)
    { 
    MXmlEngNodeFilter* filter = reinterpret_cast<MXmlEngNodeFilter*>(aFilter);
    return (int)filter->AcceptNode(*reinterpret_cast<TXmlEngNode*>(&aNode));
    }

void SetUserDataCallbacks()
    {
    xmlRegisterNodeDefault(NodeConstructCallback);
    xmlDeregisterNodeDefault(NodeDestructCallback);
    }

xmlChar* xmlCharFromDesC8L(const TDesC8& aDesc)
    {
    if(!aDesc.Length())
        {
        return NULL;
        }
    xmlChar* newContent = (xmlChar*)new(ELeave) TUint8[aDesc.Length() + 1];
    *(Mem::Copy((TAny*)newContent, aDesc.Ptr(), aDesc.Length())) = 0;
    return newContent;
    }

void xmlCharAssignToRbuf8L(RBuf8& aOutput,xmlChar* text)
{
if(aOutput.Length())
    {
    aOutput.Close();
    }
if(text)
    {
    aOutput.Assign(text,strlen((char*)text),strlen((char*)text));
    }
else
    {
    aOutput.Assign(text,0);
    }
}
//--------------------------------------------
//using CXmlEngOwnedNodesContainer;

void LibxmlDocumentCleanup(
    TAny* aDocPtr )
    {
    if(((xmlDocPtr)aDocPtr)->ownedNodes)
        {
        CXmlEngOwnedNodesContainer* nc = (CXmlEngOwnedNodesContainer*)(((xmlDocPtr)aDocPtr)->ownedNodes);
        nc->FreeAll();
        xmlFree(nc->iNodes);
        xmlFree(nc);
        } 
	xmlFreeDoc((xmlDocPtr)aDocPtr);
    }

void LibxmlNodeCleanup(
    TAny* aNodePtr )
    {
	xmlFreeNode((xmlNodePtr)aNodePtr);
    }
