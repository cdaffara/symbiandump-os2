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
// Node functions implementation
//

#include <stdlib.h>

#include <xml/dom/xmlengelement.h>
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengnamespace.h>
#include <xml/dom/xmlengtext.h>
#include <xml/dom/xmlenguserdata.h>
#include <xml/dom/xmlengnodelist.h>
#include <stdapis/libxml2/libxml2_parser.h>
#include <stdapis/libxml2/libxml2_globals.h>
#include <stdapis/libxml2/libxml2_xmlio.h>
#include "libxml2_xmlsave_private.h"
#include "libxml2_tree_private.h"
#include "xmlengdomdefs.h"
#include <xml/dom/xmlengerrors.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>

// ---------------------------------------------------------------------------------------------
// Unlinks the internal libxml2's node from double-linked list.
// Relinks neighbour nodes.The node stays virtually linked to its old neighbours! Use with care!!
//
// No checks are made; nor parent's, nor node's properties updated
// ---------------------------------------------------------------------------------------------
//
void TXmlEngNode::DoUnlinkNode()
    {
    xmlNodePtr& next = LIBXML_NODE->next;
    xmlNodePtr& prev = LIBXML_NODE->prev;
    // Unlink this node; relink neighbors
    if (prev)
        {
        prev->next = next;
        }
    else
        {
        // Unlinked node is the first in the list
        // 'children' or 'properties' property of the parent node
        // must be updated (depending on the type of the node: content node or attribute) 
        xmlNodePtr& parent = LIBXML_NODE->parent;
        // Ugly, but works :)
        xmlNodePtr* first = ((LIBXML_NODE->type ==  XML_ATTRIBUTE_NODE) 
                              ? reinterpret_cast<xmlNodePtr*>(&(parent->properties)) 
                              : &(parent->children));
        *first = next;
        }
    if (next)
        {
        next->prev = prev;
        }
    else
        {
        // Unlinked node is the last in the list (of elements or attributes) 
        // 'last' property of the parent element must be updated  (for content nodes only)
        if(LIBXML_NODE->type != XML_ATTRIBUTE_NODE)
            {
            LIBXML_NODE->parent->last = prev;
            }
        }
    }


// ---------------------------------------------------------------------------------------------
// Inserts the node in a double-linked list of nodes before specified node.
//
// No checks are made; nor parent's, nor node's properties updated (except prev/next)
// ---------------------------------------------------------------------------------------------
//
void TXmlEngNode::LinkBefore(TXmlEngNode aTargetNode)
    {
    xmlNodePtr node = INTERNAL_NODEPTR(aTargetNode);
    xmlNodePtr& prev = node->prev;
    LIBXML_NODE->next = node;
    LIBXML_NODE->prev = prev;
    prev->next = LIBXML_NODE;
    node->prev = LIBXML_NODE;
    }

// ---------------------------------------------------------------------------------------------
// Get innerXML string. This method return all content of the node.
// Output text does not include node markup.
//
// @note Returned TString should be freed
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TInt TXmlEngNode::InnerXmlL(RBuf8& aBuffer)
    {
    if(aBuffer.Length())
        {
        aBuffer.Close();
        }
    if(IsNull())
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	} 
    	
    if(NodeType() == TXmlEngNode::EDocument)
        {
        return OwnerDocument().SaveL(aBuffer, OwnerDocument(), NULL);
        }
    aBuffer.FillZ();
    RXmlEngNodeList<TXmlEngNode> nodeList;
    RBuf8 tmpStr;
    GetChildNodes(nodeList);
    TXmlEngNode tmpNode;
    while(nodeList.HasNext())
        {
        tmpNode = nodeList.Next();
        tmpNode.OuterXmlL(tmpStr);
        CleanupClosePushL(tmpStr);
        if((aBuffer.MaxSize() - aBuffer.Size()) < tmpStr.Size()) 
            {
            aBuffer.ReAllocL(aBuffer.MaxSize() + tmpStr.Size() + 1);
            }
        aBuffer.Append(tmpStr);
        CleanupStack::PopAndDestroy();
        }
    tmpStr.Close();
    
    return aBuffer.Size();
    }
    
// ---------------------------------------------------------------------------------------------
// Get outerXML string. This method return all content of the node.
// Output text includes node markup.
//
// @note Returned TString should be freed
//
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TInt TXmlEngNode::OuterXmlL(RBuf8& aBuffer)
    {
    TInt size = -1;
    if(aBuffer.Length())
        {
        aBuffer.Close();
        }
    if(IsNull())
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	} 
        
    if(NodeType() == TXmlEngNode::EDocument)
        {
        return OwnerDocument().SaveL(aBuffer, OwnerDocument(), NULL);
        }
    
    xmlSaveCtxt ctxt;
    xmlOutputBufferPtr buf;
    
    /* 
     * save the content to a temp buffer.
     */
    buf = xmlAllocOutputBuffer(NULL);
    OOM_IF_NULL(buf);

    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = LIBXML_NODE->doc;
    ctxt.buf = buf;
    ctxt.level = 0;
    ctxt.format = 0; 
   
    xmlSaveCtxtInit(&ctxt);

    xmlNodeDumpOutputInternal(&ctxt, LIBXML_NODE);
    if(xmlOOMFlag())
        {
        xmlOutputBufferClose(buf);
        OOM_HAPPENED;
        }
        
    size = buf->buffer->use;
    if(size > 0)
        {
        // frees any previous contents of aBuffer argument
        aBuffer.Assign(buf->buffer->content,size,size);
        // To prevent it from freeing
        buf->buffer->content = NULL; 
        }
    xmlOutputBufferClose(buf);
    
    // Leaves with KErrNoMemory
    TEST_OOM_FLAG; 
    if (size < 0)
        {
        XmlEngLeaveL(KXmlEngErrNegativeOutputSize);
        }        
        
    return size;
    }

// ---------------------------------------------------------------------------------------------
// Moves the node to become the first in the list of its siblings
// Node is expected to have a parent.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::SetAsFirstSibling()
    {
    if (LIBXML_NODE->type == XML_NAMESPACE_DECL)
        {
        return;
        }
    xmlNodePtr& prev = LIBXML_NODE->prev;
    if (prev)
        {
        DoUnlinkNode();
        // Insert as first
        prev = NULL;
        xmlNodePtr& parent = LIBXML_NODE->parent;
        // Ugly, but works :)
        xmlNodePtr* first = ((LIBXML_NODE->type == XML_ATTRIBUTE_NODE)
                              ? reinterpret_cast<xmlNodePtr*>(&(parent->properties))
                              : &(parent->children));
        
        (*first)->prev = LIBXML_NODE;
        LIBXML_NODE->next = *first;
        *first = LIBXML_NODE;
        // NOTE: it is possible to cope without having parent defined,
        //       it will need to iterate through sibling list to find the first one
        }
    }


// ---------------------------------------------------------------------------------------------
// Moves the node to become the last in the list of its siblings
// Node is expected to have a parent.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::SetAsLastSibling()
    {
    if (LIBXML_NODE->type == XML_NAMESPACE_DECL)
        {
        return;
        }
    xmlNodePtr& next = LIBXML_NODE->next;
    if (next)
        {
        DoUnlinkNode(); 
        xmlNodePtr& parent = LIBXML_NODE->parent;
        xmlNodePtr& prev = LIBXML_NODE->prev;
        if (LIBXML_NODE->type != XML_ATTRIBUTE_NODE)
            {
            if (!prev)
                parent->children = next;
            prev = parent->last;
            parent->last->next = LIBXML_NODE;
            parent->last = LIBXML_NODE;
            }
        else
            {
            if (!prev)
                parent->properties = (xmlAttrPtr)next;
            prev = next; // NOTE: 'next' is always different than NULL
            while (prev->next)
                prev = prev->next;
            prev->next = LIBXML_NODE;
            }
        next = NULL;
        }
    }

// ---------------------------------------------------------------------------------------------
// Moves the node in the list of sibling nodes before another node
// Node is expected to have a parent.
// Do nothing if aSiblingNode is not one of node's siblings
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::MoveBeforeSibling(
    TXmlEngNode aSiblingNode )
    {
    if (LIBXML_NODE->type == XML_NAMESPACE_DECL)
        {
        return;
        }
    xmlNodePtr node = INTERNAL_NODEPTR(aSiblingNode);
    if (!node->prev)
        {
        SetAsFirstSibling();
        }
    else
        {
        DoUnlinkNode();
        LinkBefore(aSiblingNode);
        }
    }

// ---------------------------------------------------------------------------------------------
// Moves the node in the list of sibling nodes after another node
// Node is expected to have a parent.
// Do nothing if aSiblingNode is not one of the node's siblings
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::MoveAfterSibling(
    TXmlEngNode aSiblingNode)
    {
    if (LIBXML_NODE->type == XML_NAMESPACE_DECL)
        {
        return;
        }
    xmlNodePtr node = INTERNAL_NODEPTR(aSiblingNode);
    if (!node->next)
        {
        SetAsLastSibling();
        }
    else
        {
        DoUnlinkNode();
        LinkBefore(aSiblingNode.NextSibling());
        }   
    }

// ---------------------------------------------------------------------------------------------
// Retrieves a "handle" for namespace declaration that applies to the node's namespace
// Note: DOM specs do not consider namespace declarations as a kind of nodes
// This API adds TXmlEngNamespace type of nodes, which is derived from TXmlEngNode.
//
// @return    Object that represents namespace declaration and prefix binding that
//            act on the node; returns NULL object (check using TXmlEngNamespace.IsNull()
//            or TXmlEngNamespace.NotNull()) if no namespace associated
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNamespace TXmlEngNode::NamespaceDeclaration() const
    {
    //
    switch(LIBXML_NODE->type)
        {
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
            return TXmlEngNamespace(LIBXML_NODE->ns);
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------------------------
// Clones the node completely: all attributes and namespace declarations (for TXmlEngElement nodes),
// values and children nodes are copied as well.
// 
// Document nodes cannot be copied with this method: RXmlEngDocument::CloneDocumentL() must be used.
//
// @return Complete copy of a node or leaves.
// @note    The node should not be NULL!
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::CopyL() const
    {
    if ( !LIBXML_NODE )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    // Copying of RXmlEngDocument should be made with RXmlEngDocument::CloneDocumentL()
    if(NodeType() == TXmlEngNode::EDocument)
        return NULL;
    //
    xmlNodePtr copy = xmlStaticCopyNode(
                            LIBXML_NODE, 
                            NULL /* doc */, 
                            NULL /* parent */, 
                            1);
    //
    if (xmlOOMFlag())
        {
        if(copy)
            xmlFreeNode(copy); // it may be a partial copy
        OOM_HAPPENED;
        }
    TXmlEngNode ncopy(copy);
    OwnerDocument().TakeOwnership(ncopy);
    return ncopy;
    }

// ---------------------------------------------------------------------------------------------
// Creates a deep copy of the node and appends the subtree as a new child
// to the provided parent node.
//
// @return Created copy of the node after linking it into the target document tree.
// @note Document nodes cannot be copied with this method; use RXmlEngDocument::CloneDocumentL()
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::CopyToL(
    TXmlEngNode aParent ) const
    {
    if ( !LIBXML_NODE )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    if ( LIBXML_NODE->type == XML_DOCUMENT_NODE )
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
   if ( aParent.IsNull() )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    TXmlEngNode ncopy = CopyL();
    return aParent.AppendChildL(ncopy);
    }

// ---------------------------------------------------------------------------------------------
// Detaches a node from document tree
//
// @return This node, which is already not a part of any document
// @note    Remember to use ReconcileNamespacesL() later, if extracted node (subtree)
//         contains references to namespace declarations outside of the subtree.
// @see     ReconcileNamespacesL()
// @note    The document, from which the node is being unlinked, becomes an owner of the node
//         until it is linked elsewhere.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::Unlink()
    {   
    //
    if(LIBXML_NODE && ParentNode().NotNull())
        {
        RXmlEngDocument tmpDoc = OwnerDocument();
        xmlUnlinkNode(LIBXML_NODE);
        tmpDoc.TakeOwnership(*this);
        }
    return *this;
    }

// ---------------------------------------------------------------------------------------------
// Unlinks the node and destroys it; all child nodes are destroyed as well and all memory is freed
//
// @note  Document nodes cannot be "removed" with this method, uses RXmlEngDocument-specific methods.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::Remove()
    {
    //
    if (!LIBXML_NODE || LIBXML_NODE->type == XML_DOCUMENT_NODE)
        {
        return;
        }
    if(ParentNode().NotNull())
        {
        xmlUnlinkNode(LIBXML_NODE);
        }
    else
        {
        OwnerDocument().RemoveOwnership(*this);
        }
    if (LIBXML_NODE->type != XML_ATTRIBUTE_NODE)
        {
     xmlFreeNode(LIBXML_NODE);
        }
	else
		{
		xmlFreeProp(INTERNAL_ATTRPTR(iInternal));
		}

    iInternal = 0;
    }

// ---------------------------------------------------------------------------------------------
// Ensures that namespaces referred to in the node and its descendants are
// in the scope the node.
//
// * This method checks that all the namespaces declared within the given
// * tree are properly declared. This is needed for example after Copy or Unlink
// * and then Append operations. The subtree may still hold pointers to
// * namespace declarations outside the subtree or they may be invalid/masked. As much
// * as possible the function try to reuse the existing namespaces found in
// * the new environment. If not possible, the new namespaces are redeclared
// * on the top of the subtree.
//
// This method should be used after unlinking nodes and inserting to another
// document tree or to a another part of the original tree, if some nodes of the subtree
// are remove from the scope of a namespace declaration they refer to.
//
// When node is unlinked, it may still refer to namespace declarations from the previous location.
// It is important to reconcile subtree's namespaces if previous parent tree is to be destroyed.
// On the other hand, if the parent tree is not changed before pasting its unlinked part into another
// tree, then reconciliation is needed only after paste operation.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::ReconcileNamespacesL()
    {
    xmlReconciliateNs(LIBXML_NODE->doc, LIBXML_NODE);
    TEST_OOM_FLAG;
    }

// ---------------------------------------------------------------------------------------------
// Current node is replaced with another node (subtree).
//
// The replacement node is linked into document tree instead of this node.
// The replaced node is destroyed.
//
// @see SubstituteForL(TXmlEngNode)
//
// In both cases the argument node is unlinked from its previous location
// (which can be NONE, i.e. not linked; SAME or ANOTHER document tree).
//
// @note Replacement of a node with NULL TXmlEngNode is legal and equivalent to removing the node.
// @note Not applicable to document nodes
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::ReplaceWithL(TXmlEngNode aNode)
    {
    if ( !LIBXML_NODE )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    if ( LIBXML_NODE->type == XML_DOCUMENT_NODE )
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}

    if(aNode.ParentNode().IsNull())
        {
    	// new node is unlinked, so owned by some (maybe other than this) document
	    // previous ownership must be withdrawn
        aNode.OwnerDocument().RemoveOwnership(aNode);
        }
    if(ParentNode().IsNull())
        {
    	// This node is unlinked, thus owned by its document
	    // Ownership must be withdrawn before removing the node
	    RXmlEngDocument doc = OwnerDocument();
	    doc.RemoveOwnership(*this);
	    // Also, there is no place to link the new node! 
	    // So it will be unlinked, but owned by this node's document 
        doc.TakeOwnership(aNode);
        }
    // put new node instead of 'this' node: it works with all combinations of linked/unlinked states of the nodes
    xmlFreeNode(xmlReplaceNode(LIBXML_NODE, INTERNAL_NODEPTR(aNode)));
    }

EXPORT_C void TXmlEngNode::ReplaceWith(TXmlEngNode aNode)
    {
    if(!LIBXML_NODE || LIBXML_NODE->type == XML_DOCUMENT_NODE)
        return;
    if(aNode.ParentNode().IsNull())
        {
    	// new node is unlinked, so owned by some (maybe other than this) document
	    // previous ownership must be withdrawn
        aNode.OwnerDocument().RemoveOwnership(aNode);
        }
    if(ParentNode().IsNull())
        {
    	// This node is unlinked, thus owned by its document
	    // Ownership must be withdrawn before removing the node
	    RXmlEngDocument doc = OwnerDocument();
	    doc.RemoveOwnership(*this);
	    // Also, there is no place to link the new node! 
	    // So it will be unlinked, but owned by this node's document 
        doc.TakeOwnership(aNode);
        }
    // put new node instead of 'this' node: it works with all combinations of linked/unlinked states of the nodes
    xmlFreeNode(xmlReplaceNode(LIBXML_NODE, INTERNAL_NODEPTR(aNode)));
    }

// ---------------------------------------------------------------------------------------------
// Another node is put instead of the current node.
//
// Does the same as ReplaceWith(TXmlEngNode) but does not free node and just returns it.
//
// @return Current node after unlinking it from document tree
// @see ReplaceWith(TXmlEngNode)
//
// In both cases the argument node is unlinked from its previous location
// (which can be NONE, i.e. not linked; SAME or ANOTHER document tree)
//
// It is possible to use NULL TXmlEngNode object as an argument. In such case
// no new node will be put instead of unlinked one.
//
// @note Not applicable to document nodes
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::SubstituteForL(TXmlEngNode aNode)
    {
    if ( !LIBXML_NODE )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    if ( LIBXML_NODE->type == XML_DOCUMENT_NODE )
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
    //
    if(aNode.ParentNode().IsNull())
        {
      	// new node is unlinked, so owned by other document
    	// it must not be owned to be linked
        aNode.OwnerDocument().RemoveOwnership(aNode);
        }
    if(ParentNode().IsNull())
        {
    	// this node is unlinked - nowhere to link new node: this node's document will own it
        OwnerDocument().TakeOwnership(aNode);
	    // this node will stay as it is
	    // new node will be unlinked by call to xmlReplaceNode
        }        
    TXmlEngNode tmpNode = TXmlEngNode(xmlReplaceNode(LIBXML_NODE, INTERNAL_NODEPTR(aNode)));
    aNode.OwnerDocument().TakeOwnership(tmpNode);
    return tmpNode;
    }

// ---------------------------------------------------------------------------------------------
// DOM Level 3 Core
// ---------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------
//Initializes a node list with all children of the node
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::GetChildNodes(RXmlEngNodeList<TXmlEngNode>& aList) const
    {
    aList.Open(LIBXML_NODE->children, TDOMNODETYPENONE);
    }

// ---------------------------------------------------------------------------------------------
// @return Parent node of the node or NULL if no parent
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::ParentNode() const
    {
    //
    return TXmlEngNode(LIBXML_NODE->parent);
    }

// ---------------------------------------------------------------------------------------------
// @return The first child node or NULL if no children
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::FirstChild() const
    {
    //
    return TXmlEngNode(LIBXML_NODE->children);
    }

// ---------------------------------------------------------------------------------------------
// @return The last child node or NULL if no children
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::LastChild() const
    {
    //
    return TXmlEngNode(LIBXML_NODE->last);
    }

// ---------------------------------------------------------------------------------------------
// @return Previous node in a child list or NULL if no sibling before
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::PreviousSibling() const
    {
    if(!LIBXML_NODE->parent)
        {
        return NULL;
        }
    return TXmlEngNode(LIBXML_NODE->prev);
    }

// ---------------------------------------------------------------------------------------------
// @return Following node in a child list or NULL if no sibling after
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::NextSibling() const
    {
    if(!LIBXML_NODE->parent)
        {
        return NULL;
        }
    return TXmlEngNode(LIBXML_NODE->next);
    }

// ---------------------------------------------------------------------------------------------
// @return A document node of the DOM tree this node belongs to
// @note    An instance of RXmlEngDocument class returns itself
// ---------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument TXmlEngNode::OwnerDocument() const
    {
    //
    return RXmlEngDocument(LIBXML_NODE->doc);
    }

// ---------------------------------------------------------------------------------------------
// Append a child node.
//
// This is universal operation for any types of nodes.
// Note, that some types of nodes cannot have children and
// some types of nodes are not allowed to be children of some other types.
//
// @return Appended node, which could changed as a result of adding it to
// list of child nodes (e.g. text nodes can coalesce together)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode TXmlEngNode::AppendChildL(
    TXmlEngNode aNewChild)
    {
    if ( !LIBXML_NODE || aNewChild.IsNull() )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    if(aNewChild.ParentNode().IsNull())
    {
        aNewChild.OwnerDocument().RemoveOwnership(aNewChild);
    }
    else
    {
         xmlUnlinkNode(INTERNAL_NODEPTR(aNewChild));
    }
   xmlNodePtr   child  =  xmlAddChild(LIBXML_NODE, INTERNAL_NODEPTR(aNewChild));

    TEST_OOM_FLAG;
    return child;
}

// ---------------------------------------------------------------------------------------------
// @return Type of the node
//
// Use NodeType() to find out the type of the node prior to casting object
// of TXmlEngNode class to one of its derived subclasses (TXmlEngElement, TXmlEngAttr, TXmlEngTextNode, etc.)
//
// @see TXmlEngDOMNodeType
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode::TXmlEngDOMNodeType TXmlEngNode::NodeType() const
    {
    //
    if((TXmlEngDOMNodeType)LIBXML_NODE->type == EText && TEXT_NODE_DATA_TYPE)
    	{
		return (TXmlEngDOMNodeType)(TUint)TEXT_NODE_DATA_TYPE;
    	}
	return (TXmlEngDOMNodeType)LIBXML_NODE->type;
    }

// Note: "" is used for undefined name;  0 means "to use libxml node's name field"
static const char* const KNodeNames[]={
    0,
    0,                  // EElement                 =       1,
    0,                  // EAttribute               =       2,
    "#text",            // EText                    =       3,
    "#cdata-section",   // ECDATASection            =       4,
    0,                  // EEntityReference         =       5,
    "",                 // EEntity                  =       6,
    0,                  // EProcessingInstruction   =       7,
    "#comment",         // EComment                 =       8,
    0,                  // EDocument                =       9,
    0,                  // EDocumentType            =       10,
    "#document-fragment", // EDocumentFragment      =       11,
    "",                 // ENotation                =       12,
    0,                  // ENamespaceDeclaration    =       18  // not in use
	"",					// EBinaryContainer 		= 		 30,
	"",					// EChunkContainer	 		= 		 31,
	""					// EFileContainer 			= 		 32    
};

const TInt KNodeNamesSize = sizeof(KNodeNames);

// ---------------------------------------------------------------------------------------------
// @return Name of the node
//
// This method generally follows DOM spec :
// \verbatim
// -------------------------------------------------------------------------------
// The values of nodeName, nodeValue, and attributes vary according to the node
// type as follows:
//
// interface              nodeName                nodeValue            attributes
// -------------------------------------------------------------------------------
// Attr                   = Attr.name              = Attr.value             = null
// CDATASection           = "#cdata-section"       = CharacterData.data     = null
// Comment                = "#comment"             = CharacterData.data     = null
// Document               = "#document"            = null                   = null
// DocumentFragment       = "#document-fragment"   = null                   = null
// DocumentType           = DocumentType.name      = null                   = null
// Element                = Element.tagName        = null           = NamedNodeMap
// Entity                 = entity name            = null                   = null
// EntityReference        = name of entity referenced  = null               = null
// Notation               = notation name          = null                   = null
// ProcessingInstruction  = target                 = data                   = null
// Text                   = "#text"                = CharacterData.data     = null
// -------------------------------------------------------------------------------
// \endverbatim
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngNode::Name() const
    {
    // DONE: OPTIMIZE: Create static array of names with nodetype as index
    //                 Use NULL if name is not constant and then use such switch..
    //
    TUint type = (TUint) LIBXML_NODE->type;
    if (type < KNodeNamesSize /sizeof(char*))
        {
        const char* KName = KNodeNames[type];
        if(KName)
            {
            return TXmlEngConstString(KName).PtrC8();
            }
        return TXmlEngConstString((char*)LIBXML_NODE->name).PtrC8();
        }
    return KNullDesC8();
    }


// ---------------------------------------------------------------------------------------------
// Fetches value of this node, depending on its type.
//
// @note It is better to always cast nodes to specific type and then use specific
//      method for getting "node value"
//
// @return Node value
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngNode::Value() const
    {
    if (iInternal)
        {
        switch(LIBXML_NODE->type)
            {
            // The content of first Text child is returned
            case XML_ATTRIBUTE_NODE:
                return AsAttr().Value();
            // Note: in DOM spec element's value is Null, but we can
            //       access it: the content of first TXmlEngTextNode child node is returned
            case XML_ELEMENT_NODE:
                return AsElement().Text();
            // TXmlEngTextNode, TXmlEngCDATASection, TXmlEngProcessingInstruction and Comments store
            // content in the same way.
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_COMMENT_NODE:      
            case XML_PI_NODE:           
                return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_NODE->content)).PtrC8();

            default:
                ;
            }
        }
    return NULL;
}
// ---------------------------------------------------------------------------------------------
// Sets value of this node.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::SetValueL(
    const TDesC8& aValue )
    {
    if (iInternal)
        {
        switch(LIBXML_NODE->type)
            {
            // The content of first TXmlEngTextNode child is returned
            case XML_ATTRIBUTE_NODE:
            case XML_ELEMENT_NODE:
                // Note: in DOM spec element's value is Null, but we can
                //       access it: the content of first TXmlEngTextNode child node is returned
                AsAttr().SetValueL(aValue); //  same as TXmlEngElement::SetTextL(aValue);
                return;
            // TXmlEngTextNode, TXmlEngCDATASection, TXmlEngProcessingInstruction and Comments store
            // content in the same way.
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_COMMENT_NODE:      
            case XML_PI_NODE:       
                AsText().SetContentsL(aValue);
                return;
            default:
                ;
            }
        }
    }


// ---------------------------------------------------------------------------------------------
// @return Whether the value of the node is presented by only one TXmlEngTextNode node
//
// If the value is <i>"simple text"</i> then it is possible to access it as TDOMString
// without making copy, which combines values of all text nodes and entity reference nodes.
//
// @see TXmlEngNode::Value(), TXmlEngAttr::Value(), TXmlEngElement::Text()
//
// This method is applicable to TXmlEngElement and TXmlEngAttr nodes. On other nodes FALSE is returned.
//
// @note
// Values (contents) of TXmlEngComment, TXmlEngCDATASection, TXmlEngTextNode, ProcessingInstuction data are
// always "simple".
//
// When the returned result is FALSE, getting value of the node would not returned
// whole contents because of either entity references present in the contents or
// the contents is mixed (for TXmlEngElement node). In this case WholeTextContentsCopyL()
// should be used.
//
// @see TXmlEngNode::WholeTextContentsCopyL()
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TBool TXmlEngNode::IsSimpleTextContents() const
    {
    if (!LIBXML_NODE)
        return false;
    xmlElementType type = LIBXML_NODE->type;
    if (type == XML_ELEMENT_NODE ||
        type == XML_ATTRIBUTE_NODE)
        {
        xmlNodePtr children = LIBXML_NODE->children;

        return children  &&  children->type == XML_TEXT_NODE &&
               !(children->next);
        }
    return false; // incorrect type node
}

// ---------------------------------------------------------------------------------------------
// @return   the content of the node
//
// What is returned depends on the node type.
// Method caller is responsible for freeing returned string.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::WholeTextContentsCopyL(RBuf8& aOutput) const
    {
    XE_ASSERT_DEBUG(LIBXML_NODE);
    //
    xmlChar* text = xmlNodeGetContent(LIBXML_NODE);
    if (xmlOOMFlag())
        { 
        if(text) 
            xmlFree(text);
        OOM_HAPPENED;
        }
    xmlCharAssignToRbuf8L(aOutput,text);
    }
// -----------------------------------------------------------------------------

EXPORT_C TBool TXmlEngNode::HasChildNodes() const
    {
    //
    return (TBool)LIBXML_NODE->children;
    }

// ---------------------------------------------------------------------------------------------
// @return  Namespace URI of a node
//           - NULL is returned for elements and attributes that do not
//             belong to any namespace.
//           - bound namespace URI is returned for namespace declaration nodes (instances of TXmlEngNamespace).
//           - NULL is returned to all other types of node.
// @note use IsNull() and NotNull() for testing returned result on the subject
//      of having some URI
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngNode::NamespaceUri() const
    {
    //
    switch(LIBXML_NODE->type)
        {
        case XML_ELEMENT_NODE:
        case XML_ATTRIBUTE_NODE:
            return TXmlEngNamespace(LIBXML_NODE->ns).Uri();
        case XML_NAMESPACE_DECL:
            return AsNamespace().Uri();
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------------------------
// @return  Prefix of a node
//           
// Returns NULL for elements and attributes that do not have prefix
// (node belongs to the default namespace or does not belong to any namespace)
// NULL is also returned for all types of node other than TXmlEngElement or TXmlEngAttr
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngNode::Prefix() const
    {
    if (LIBXML_NODE->type < XML_TEXT_NODE) // NOTE: XML_ELEMENT_NODE = 1 and XML_ATTRIBUTE_NODE = 2
        {
        xmlNs* ns = LIBXML_NODE->ns;
        if (ns)
            return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(ns->prefix)).PtrC8();
        }
    return KNullDesC8();
    }


// ---------------------------------------------------------------------------------------------
// @return    True if the node is TXmlEngElement and has at least one attribute
//
// @note Namespace-to-prefix bindings are not attributes.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TBool TXmlEngNode::HasAttributes() const
    {
    return iInternal &&
           LIBXML_NODE->type == XML_ELEMENT_NODE &&
           LIBXML_NODE->properties;
    }

// ---------------------------------------------------------------------------------------------
// Evaluates active base URI for the node by processing xml:base attributes of parents
//
// @return A copy of effective base URI for the node
// @note It's up to the caller to free the string with TDOMString::Free()
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngNode::BaseUriL(RBuf8& aBaseUri) const
    {
    //
    xmlChar* uri = xmlNodeGetBase(LIBXML_NODE->doc, LIBXML_NODE);
    if (xmlOOMFlag())
        {
        if(uri)
            xmlFree(uri); // partial construction is possible
        OOM_HAPPENED;
        }
    xmlCharAssignToRbuf8L(aBaseUri,uri);
    }

// ---------------------------------------------------------------------------------------------
// Searches the prefix that is bound to the given aNamespaceUri and
// applicable in the scope of this TXmlEngNode.
//
// @return    A sought prefix or NULL if not found or aNamespaceUri is the default namespace
// @see TXmlEngElement::LookupNamespaceByUri(TXmlEngNode,TDOMString)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngNode::LookupPrefixL(
    const TDesC8& aNamespaceUri ) const
    {
    if ( !LIBXML_NODE )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    if (LIBXML_NODE->type == XML_ATTRIBUTE_NODE ||
        LIBXML_NODE->type == XML_ELEMENT_NODE)
        {
        return AsElement().LookupNamespaceByUriL(aNamespaceUri).Prefix();
        }
	if(!ParentNode().IsNull())        
		{
		return ParentNode().AsElement().LookupNamespaceByUriL(aNamespaceUri).Prefix();
		}
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------------------------
// @return True if given namespace URI is a default one for the node (applicable to elements only)
//
// @note "" or NULL can be used to denote undefined namespace
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TBool TXmlEngNode::IsDefaultNamespaceL(
    const TDesC8& aNamespaceUri ) const
    {
    if (LIBXML_NODE->type == XML_ELEMENT_NODE) // No default namespaces for attributes
        {
        return !AsElement().DefaultNamespaceL().Uri().Compare(aNamespaceUri);
        }
    return false;
    }

// ---------------------------------------------------------------------------------------------
// Searches the namespace URI that is bound to the given prefix.
//
// @return    - a sought URI or NULL if the prefix is not bound
// @see TXmlEngElement::LookupNamespaceByPrefix(TXmlEngNode,TDOMString)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngNode::LookupNamespaceUriL(
    const TDesC8& aPrefix ) const
    {
    if ( !LIBXML_NODE )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    if (LIBXML_NODE->type == XML_ATTRIBUTE_NODE ||
        LIBXML_NODE->type == XML_ELEMENT_NODE)
        {
        return AsElement().LookupNamespaceByPrefixL(aPrefix).Uri();
        }
	if(!ParentNode().IsNull())        
		{
		return ParentNode().AsElement().LookupNamespaceByPrefixL(aPrefix).Uri();
		}
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------------------------
// Returns the user data object attached to this node. Ownership is not transferred.
//
// @return Pointer to data object or NULL if it doesn't exist.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C MXmlEngUserData* TXmlEngNode::UserData() const
    {
    return
        (LIBXML_NODE &&
            (LIBXML_NODE->type == XML_ATTRIBUTE_NODE ||
             LIBXML_NODE->type == XML_ELEMENT_NODE))
        ?
        static_cast<MXmlEngUserData*>(LIBXML_NODE->_private)
        :
        NULL;
    }

// ---------------------------------------------------------------------------------------------
// Removes the user data onject attached to this node. Ownership is transferred
// (the object is not deleted).
//
// @return Pointer to data object or NULL if it doesn't exist.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C MXmlEngUserData* TXmlEngNode::RemoveUserData()
    {
    MXmlEngUserData* data = UserData();
    if (data)
        {
        // node type has been checked by UserData()
        LIBXML_NODE->_private = NULL;
        }
    return data;
    }

// ---------------------------------------------------------------------------------------------
// Attaches a user data object to this node. The ownership of the object is transferred.
// When the (underlying) node is deleted the Destroy method of the MXmlEngUserData class will be
// called. If there already is a user data object associated with this node, it will be
// deleted before attaching the new object. Notet that only TXmlEngElement and Attribute nodes
// currently support this feature.
//
// @param[in] aData Pointer to the data object.
// @return true if successful, false if for example underlying node type doesn't support
// attaching user data.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TBool TXmlEngNode::AddUserData(
    MXmlEngUserData* aData )
    {
    TBool ret = false;
    if (iInternal)
        {
        if (LIBXML_NODE->type == XML_ATTRIBUTE_NODE ||
            LIBXML_NODE->type == XML_ELEMENT_NODE)
            {
            // release previous data
            if (LIBXML_NODE->_private)
                {
                MXmlEngUserData* oldData = RemoveUserData();
                oldData->Destroy();
                }
            LIBXML_NODE->_private = aData;
            ret = true;
            }
        }
    return ret;
    }

