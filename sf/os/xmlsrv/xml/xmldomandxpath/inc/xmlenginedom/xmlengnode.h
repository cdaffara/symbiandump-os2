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
// Node class declaration
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGNODE_H
#define XMLENGNODE_H

#include <e32base.h>

class TXmlEngNode;
template<class T> class RXmlEngNodeList;
class RXmlEngDocument;
class TXmlEngElement;
class TXmlEngAttr;
class TXmlEngTextNode;
class TXmlEngNamespace;
class TXmlEngComment;
class TXmlEngCDATASection;
class TXmlEngDocumentFragment;
class TXmlEngEntityReference;
class TXmlEngProcessingInstruction;
class MXmlEngUserData;
class TXmlEngBinaryContainer;
class TXmlEngChunkContainer;
class TXmlEngDataContainer;
class TXmlEngFileContainer;

/**
This class represents an XML node in the DOM tree.

As the base class for other node types (element, attribute, etc.) this class
implements common methods that are similar for all XML node types.

Sample code for tree manipulations:
@code  
    RXmlEngDOMImplementation domImpl;
    domImpl.OpenL();        	// opening DOM implementation object 
    RXmlEngDocument iDoc; 		// iDoc with created nodes tree
    TXmlEngNode tmp = iDoc.DocumentElement();
 	// copying first child of iDoc to tmp2 node and appending it
	TXmlEngNode tmp2 = tmp.FirstChild().CopyL();
 	tmp.AppendChildL(tmp2);
 	// copying the second child of iDoc to the last child
 	tmp.FirstChild().NextSibling().CopyToL(tmp.LastChild());
 	// replacing the second to last child with the second child 
 	tmp.LastChild().PreviousSibling().ReplaceWith(tmp.FirstChild().NextSibling());
 	// moving first child of iDoc to second child's children
 	tmp.FirstChild().MoveTo(tmp.FirstChild().NextSibling());	
    iDoc.Close();				// closing all opened objects
    domImpl.Close();
@endcode 

A node is NULL when the internal data pointer is NULL.  This is the default
state of a newly created node.  Many DOM APIs that return TXmlEngNode objects
use this NULL state to indicate a failure of some kind.  The methods IsNull()
and NotNull() can be used to determine whether a node is NULL.
*/
class TXmlEngNode
{
public:
   /**
   The different node types in a XML tree.
   */
    enum TXmlEngDOMNodeType {
        EElement                =        1,
        EAttribute              =        2,
        EText                   =        3,
        ECDATASection           =        4,
        EEntityReference        =        5,
		/** Not supported */
        EEntity                 =        6,
        EProcessingInstruction  =        7,
        EComment                =        8,
        EDocument               =        9,
		/** Not supported */
        EDocumentType           =        10,
        EDocumentFragment       =        11,
		/** Not supported */
        ENotation               =        12,
		/** Extension to the DOM spec */
        ENamespaceDeclaration   =        18,
		/** Extension to the DOM spec */
		EBinaryContainer 		= 		 30,
		/** Extension to the DOM spec */
		EChunkContainer 		= 		 31,
		/** Extension to the DOM spec */
		EFileContainer 			= 		 32
    };

public:
    /** Default constructor */
    inline TXmlEngNode();

    /**
    Constructor
    @param aInternal node pointer
    */
    inline TXmlEngNode(void* aInternal);

    /**
    Check if the node is NULL
    @return ETrue if node is NULL, otherwise EFalse 
    */
    inline TBool IsNull() const;

    /**
    Check if node is not NULL
    @return ETrue if node is not NULL, otherwise EFalse
    */
    inline TBool NotNull() const;

    /**
    Cast to an attribute node.
	Note:
         - Never cast nodes to the wrong node type!
         - Casting removes the const'ness of the node
    @return This node as an attribute node
    */
    inline TXmlEngAttr& AsAttr() const;

    /**
    Cast to a text node.
	Note:
        - Never cast nodes to the wrong node type!
        - Casting removes the const'ness of the node
    @return This node as a text node
    */
    inline TXmlEngTextNode& AsText() const;

    /**
    Cast to a binary data container
	Note:
        - Never cast nodes to the wrong node type!
        - Casting removes the const'ness of the node
    @return This node as a binary container
    */
    inline TXmlEngBinaryContainer& AsBinaryContainer() const;

    /**
    Cast to a memory chunk container
	Note:
        - Never cast nodes to the wrong node type!
        - Casting removes the const'ness of the node
    @return This node as a chunk container
    */
    inline TXmlEngChunkContainer& AsChunkContainer() const;

    /**
    Cast to a file container
	Note:
        - Never cast nodes to the wrong node type!
        - Casting removes the const'ness of the node
    @return This node as a file container
    */
    inline TXmlEngFileContainer& AsFileContainer() const;

    /**
    Cast to a memory chunk container
    Note:
        - Never cast nodes to the wrong node type!
        - Casting removes the const'ness of the node
    @return This node as a chunk container
    */
    inline TXmlEngDataContainer& AsDataContainer() const;

    /**
    Cast to an element node.
    Note:
         - Never cast nodes to the wrong node type!
         - Casting removes the const'ness of the node
    @return This node as an element node
    */
    inline TXmlEngElement& AsElement() const;

    /**
    Cast to a comment node.
    Note:
         - Never cast nodes to the wrong node type!
         - Casting removes the const'ness of the node
    @return This node as a comment node
    */
    inline TXmlEngComment& AsComment() const;

    /**
    Cast to a namespace node.
    Note:
         - Never cast nodes to the wrong node type!
         - Casting removes the const'ness of the node
    @return This node as a namespace node
    */
    inline TXmlEngNamespace& AsNamespace() const;

    /**
    Cast to a CDATA section node.
    Note:
         - Never cast nodes to the wrong node type!
         - Casting removes the const'ness of the node
    @return This node as a CDATA section node
    */
    inline TXmlEngCDATASection& AsCDATASection() const;

    /**
    Cast to an entity reference node.
    Note:
         - Never cast nodes to the wrong node type!
         - Casting removes the const'ness of the node
    @return This node as an entity reference node
    */
    inline TXmlEngEntityReference& AsEntityReference() const;

    /**
    Cast to a Document Fragment.
    Note:
         - Never cast nodes to the wrong node type!
         - Casting removes the const'ness of the node
    @return This node as a Document Fragment
    */
	inline TXmlEngDocumentFragment& AsDocumentFragment() const;

    /**
    Cast to a processing instruction node.
    Note:
         - Never cast nodes to the wrong node type!
         - Casting removes the const'ness of the node
    @return This node as a Processing instruction node
    */
    inline TXmlEngProcessingInstruction& AsProcessingInstruction() const;

	/**
	Get the inner XML string. This method returns all content of the node,
	exluding node markup.  Child nodes will be included.  Any existing contents
	in aBuffer will be destroyed.  This method allocates memory for the buffer.

    @param aBuffer A buffer owned by the caller that contains the result
    @return Length of the output buffer
	@leave KXmlEngErrWrongUseOfAPI Node is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C TInt InnerXmlL(RBuf8& aBuffer);
    
	/**
	Get the outer XML string. This method returns all content of the node,
	including node markup.  Child nodes will be included.  Any existing
	contents in aBuffer will be destroyed.  This method allocates memory for
	the buffer.

    @param aBuffer A buffer owned by the caller that contains the result
    @return Length of the output buffer
	@leave KXmlEngErrWrongUseOfAPI Node is NULL
	@leave KXmlEngErrNegativeOutputSize Output has a negative length
	@leave - One of the system-wide error codes
    */
    IMPORT_C TInt OuterXmlL(RBuf8& aBuffer);

    /**
    Moves the node to become the first in the list of its siblings
	@pre The node has a parent
    */
    IMPORT_C void SetAsFirstSibling();

    /**
    Moves the node to become the last in the list of its siblings
    @pre The node has a parent.
    */
    IMPORT_C void SetAsLastSibling();

	/**
	Moves the node to come before the specified node in the list of sibling
	nodes.  This method does nothing if aSiblingNode is not one of the node's
	siblings.
    @pre The node has a parent.
	@param aSiblingNode The node is moved to fall before this sibling
    */
    IMPORT_C void MoveBeforeSibling(TXmlEngNode aSiblingNode);

	/**
	Moves the node to come after the specified node in the list of sibling
	nodes.  This method does nothing if aSiblingNode is not one of the node's
	siblings.
	@pre The node has a parent
	@param aSiblingNode The node is moved to fall after this sibling
    */    
    IMPORT_C void MoveAfterSibling(TXmlEngNode aSiblingNode);

	/**
	Moves the node to another part of the tree or to another document.  The
	node is unlinked from its current postion (if any) and appended to its new
	parent.

	Note: In many cases this method call should be followed by a call to
	ReconcileNamespacesL().

    @param aParent This node will be moved to be a child of this node
    @return This node
	@leave KXmlEngErrNullNode The node is NULL
	@leave - One of the system-wide error codes
    */
    inline TXmlEngNode MoveToL(TXmlEngNode aParent);

    /**
	Moves the node to another part of the tree or to another document.  The
	node is unlinked from its current postion (if any) and appended to its new
	parent.
    
	Note: In many cases this method call should be followed by a call to
	ReconcileNamespacesL().

    @see     ReconcileNamespacesL()
    @param aParent This node will be moved to be a child of this node
    @return This node
	@leave KXmlEngErrNullNode The node is NULL
	@leave - One of the system-wide error codes
    
	@deprecated This method has been deprecated and will be removed in a future
	release.
    */
	inline TXmlEngNode MoveTo(TXmlEngNode aParent);

	/**
	Detaches the node from the document tree.  The document maintains ownership
	of the node until it is linked elsewhere.

	Note: Remember to use ReconcileNamespacesL() later, if the extracted node
	(or subtree) contains references to namespace declarations outside of the
	subtree.

    @see     ReconcileNamespacesL()
    @return This node
    */
    IMPORT_C TXmlEngNode Unlink();

    /**
    Ensures that namespaces referred to in the node and its descendants are
    within the scope of the node.
    
	This method checks that all the namespaces declared within the subtree
	parented by this node are properly declared. This is needed for example
	after a Copy or an Unlink followed by an Append operation. The subtree may
	still hold pointers to namespace declarations outside the subtree or they
	may be invalid/masked.  The function tries to reuse the existing namespaces
	found in the new environment as much as possible. If not possible, the new
	namespaces are redeclared at the top of the subtree.
    
	This method should be used after unlinking nodes and inserting them into
	another document tree or into another part of the original tree, if some
	nodes of the subtree are removed from the scope of the namespace
	declaration they refer to.
    
	When a node is unlinked, it may still refer to namespace declarations from
	the previous location.  It is important to reconcile the subtree's
	namespaces if the previous parent tree is to be destroyed.  On the other
	hand, if the parent tree is not changed before pasting its unlinked part
	into another tree, then reconciliation is needed only after the paste
	operation.
	@leave - One of the system-wide error codes
    */
	IMPORT_C void ReconcileNamespacesL();

	/**
	Unlinks this node and destroys it; all child nodes are also destroyed and
	all memory is freed.
    
	Note:  Document nodes cannot be removed with this method. Use 
	RXmlEngDocument::Close() instead.
    */
	IMPORT_C void Remove();

	/**
	This node is replaced with another node (or subtree).  The replacement node
	is linked into the document tree instead of this node.  The replaced node
	is destroyed.  Replacement of a node with a NULL TXmlEngNode is legal and
	equivalent to removing the node.

	aNode is unlinked from its previous location, which can be none (i.e. not
	linked), within the same document tree, or within another document tree.

	Note:  Not applicable to document nodes.

    @see SubstituteForL(TXmlEngNode)
    @param aNode Node that replaces this node
	@leave KXmlEngErrNullNode Node is NULL
	@leave KXmlEngErrWrongUseOfAPI Node is a document node
	@leave - One of the system-wide error codes
    */
    IMPORT_C void ReplaceWithL(TXmlEngNode aNode);

	/**
	This node is replaced with another node (or subtree).  The replacement node
	is linked into the document tree instead of this node.  The replaced node
	is destroyed.  Replacement of a node with a NULL TXmlEngNode is legal and
	equivalent to removing the node.

	aNode is unlinked from its previous location, which can be none (i.e. not
	linked), within the same document tree, or within another document tree.

	Note:  Not applicable to document nodes.

    @see SubstituteForL(TXmlEngNode)
    @param aNode Node that replaces this node
	@deprecated This method has been deprecated and will be removed in a future
	release.
    */
    IMPORT_C void ReplaceWith(TXmlEngNode aNode);

	/**
	Unlinks this node and puts another in its place.  This function is the same
	as ReplaceWithL(), but this node is not freed.

	aNode is unlinked from its previous location, which can be none (i.e. not
	linked), within the same document tree, or within another document tree.

	It is possible to use a NULL TXmlEngNode object as an argument. In this
	case, this node will simply be removed from the tree, but not freed.

	Note:  Not applicable to document nodes.
    
    @see ReplaceWithL()
    @param aNode Node that replaces current node
    @return The current node after unlinking it from document tree
	@leave KXmlEngErrNullNode Node is NULL
	@leave KXmlEngErrWrongUseOfAPI Node is a document node
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNode SubstituteForL(TXmlEngNode aNode);

    /**
    Retrieves the namespace declaration that applies to the node's namespace

	Note: The DOM spec does not consider namespace declarations as a kind of
	node.  This API adds TXmlEngNamespace, which is derived from TXmlEngNode.
    
	@return The namespace declaration and prefix binding that act on the node;
	returns a NULL object if no namespace associated
    */
	IMPORT_C TXmlEngNamespace NamespaceDeclaration() const;

	/**
	Attaches a user data object to this node. The ownership of the object is
	transferred.  When the node is deleted, the Destroy method of the
	MXmlEngUserData class will be called. If there is a user data object
	already associated with this node, it will be deleted before attaching the
	new object.

	Only TXmlEngElement and TXmlEngAttr nodes currently support this feature.  

    User data is not copied when the node is copied. 

    @param aData Pointer to the data object.
    @return ETrue if successful or EFalse if the node type does not support this operation
    */
    IMPORT_C TBool AddUserData(MXmlEngUserData* aData);

    /**
    Gets the user data object attached to this node. Ownership is not transferred.
    @return Pointer to data object or NULL if it doesn't exist.
    */
    IMPORT_C MXmlEngUserData* UserData() const;

	/**
	Removes the user data object attached to this node and transfers ownership
	to the caller.  The user data object is not deleted.
    @return Pointer to the user data object or NULL if it doesn't exist.
    */
    IMPORT_C MXmlEngUserData* RemoveUserData();

	/**
	Creates a deep copy of the node.  All values and children nodes are copied.
	Attributes and namespace declarations are also copied for TXmlEngElement
	nodes.  Document nodes cannot be copied with this method.  Use
	RXmlEngDocument::CloneDocumentL() instead.

	User data stored with AddUserData() is not copied.
	
    @return A complete copy of the node or NULL if the node is a document node
	@leave KXmlEngErrNullNode Node is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNode CopyL() const;

	/**
	Creates a deep copy of the node and appends the subtree as a new child to
	the provided parent node.  Document nodes cannot be copied with this
	method.  Use RXmlEngDocument::CloneDocumentL() instead.

	User data stored with AddUserData() is not copied.
	
    @return Created copy of the node after linking it into the target document tree.
	@leave KXmlEngErrNullNode Node is NULL
	@leave KXmlEngErrWrongUseOfAPI Node is document node
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNode CopyToL(TXmlEngNode aParent) const;

	/**
	Append a child node.  This is a universal operation for any type of node.
	Note that some types of nodes cannot have children and some types of nodes
	are not allowed to be children of some other types.  These relationships
	are not enforced by this function.
    
    @param aNewChild The node that should be added as a child
    @return The appended node, which could be changed as a result of adding it to
    list of child nodes (e.g. text nodes can coalesce together)
	@leave KXmlEngErrNullNode Node or aNewChild is NULL
	@leave KErrNoMemory Memory allocation failure
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngNode AppendChildL(TXmlEngNode aNewChild);

    /**
    Gets the list of children.
    @param aList The list of children returned
    */
    IMPORT_C void GetChildNodes(RXmlEngNodeList<TXmlEngNode>& aList) const;

    /**
    Gets the parent node
    @return The parent node or NULL if no parent exists
    */
    IMPORT_C TXmlEngNode ParentNode() const;

    /**
    Gets the first child
    @return The first child node or NULL if no children
    */
    IMPORT_C TXmlEngNode FirstChild() const;

    /**
    Gets the last child
    @return The last child node or NULL if no children
    */
    IMPORT_C TXmlEngNode LastChild() const;

    /**
    Gets the previous sibling
    @return The previous sibling or NULL if there is no sibling before
    */
    IMPORT_C TXmlEngNode PreviousSibling() const;

    /**
    Gets the next sibling
    @return The next sibling or NULL if there is no sibling after
    */
    IMPORT_C TXmlEngNode NextSibling() const;

	/**
	Gets the owning document 

	Note:  An instance of the RXmlEngDocument class returns itself

	@pre Node must not be NULL.
	@return The document node of the DOM tree that this node belongs to or a
	NULL document if no owning document.
    */
    IMPORT_C RXmlEngDocument OwnerDocument() const;

    /**
	Gets the value of this node.  
    
	Note:  Since this is not a virtual function, it is better to always cast
	nodes to a specific type and then use the specific method for getting the
	"node value".
    
    @return The node value
    */
    IMPORT_C TPtrC8 Value() const;

	/**
	Gets a copy of the node's text content.  What is returned depends on the
	node type.  Any existing content in the specified buffer is destroyed.
	This method allocates memory for the buffer.
    
	@param aOutput A buffer owned by the caller which holds the returned string
	@leave - One of the system-wide error codes
    */
    IMPORT_C void WholeTextContentsCopyL(RBuf8& aOutput) const;

    /**
    Copies the specified string and sets the value of this node.
    @param aValue The value to set
	@leave KXmlEngErrNullNode The node is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void SetValueL(const TDesC8& aValue);

    /**
    Check if the node content is "simple text" for element and attribute nodes.
    
	If the node content is "simple text" then it is represented by a single
	TXmlEngTextNode (or derived type).  The contents can be obtained through a
	call to Value().  A node that has multiple child text nodes does not have
	node content that is "simple text" and the contents must be obtained
	through a call to WholeTextContentsCopyL().

	The contents of TXmlEngComment, TXmlEngCDATASection, TXmlEngTextNode, and
	Processing Instuction data are always "simple".
    
    @see TXmlEngNode::Value()
	@see TXmlEngAttr::Value()
	@see TXmlEngElement::Text()
    @see TXmlEngNode::WholeTextContentsCopyL()
   
	@return ETrue if the node is an element or attribute node and content is
	represented by one TXmlEngTextNode or EFalse otherwise
    */
	IMPORT_C TBool IsSimpleTextContents() const;

	/**
	Gets the node type.  Used to find out the type of the node prior to casting
	the node to one of TXmlEngNode class to one of its derived subclasses
	(TXmlEngElement, TXmlEngAttr, TXmlEngTextNode, etc.).
    
    @see TXmlEngDOMNodeType
	@pre Node must not be NULL
    @return The type of the node
    */
    IMPORT_C TXmlEngDOMNodeType NodeType() const;

    /**
    Gets the node name.
     
    This method generally follows the DOM spec:
    -------------------------------------------------------------------------------
    The values of nodeName, nodeValue, and attributes vary according to the node
    type as follows:
    
    interface              nodeName                nodeValue            attributes
    -------------------------------------------------------------------------------
    Attr                   = Attr.name              = Attr.value             = null
    CDATASection           = "#cdata-section"       = CharacterData.data     = null
    Comment                = "#comment"             = CharacterData.data     = null
    Document               = "#document"            = null                   = null
    DocumentFragment       = "#document-fragment"   = null                   = null
    DocumentType           = DocumentType.name      = null                   = null
    Element                = Element.tagName        = null           = NamedNodeMap
    Entity                 = entity name            = null                   = null
    EntityReference        = name of entity referenced  = null               = null
    Notation               = notation name          = null                   = null
    ProcessingInstruction  = target                 = data                   = null
    Text                   = "#text"                = CharacterData.data     = null
    -------------------------------------------------------------------------------

    @return The name of the node
    */
    IMPORT_C TPtrC8 Name() const;


    /**
    Check if the node has child nodes.
    @return ETrue if the node has child nodes, EFalse otherwise
    */
    IMPORT_C TBool HasChildNodes() const;

	/**
	Check if the node has attributes.  Namespace-to-prefix bindings are not
	considered attributes.
	@return ETrue if the node is an Element node and has at least one
	attribute, EFalse otherwise
    */
	IMPORT_C TBool HasAttributes() const;

	/**
	Evaluates the active base URI for the node by processing the xml:base
	attributes of the parents of the node.  If no xml:base attributes exist,
	an empty string is returned.  Any existing content in the specified buffer
	is destroyed.  This function allocates memory for the buffer.

	@param aBaseUri A buffer owned by the caller that holds the result.
	@leave - One of the system-wide error codes
    */
    IMPORT_C void BaseUriL(RBuf8& aBaseUri) const;

	/**
	Checks if a node is the same as this node.  Nodes are considered the same
	if they refer to the same in-memory data structure.
    
    @param aOther Node to compare
    @return ETrue if the nodes are the same, EFalse otherwise
    */
    inline TBool IsSameNode(TXmlEngNode aOther) const;

    /**
    Gets the namespace URI.

	@return The namespace URI of a TXmlEngNamespace, TXmlEngAttr or
	TXmlEngElement node if bound, NULL in all other cases.
    */
    IMPORT_C TPtrC8 NamespaceUri() const;

    /**
    Gets the namespace prefix.
    
	@return The prefix of an element or attribute node if bound, NULL in all
	other cases.
    */
	IMPORT_C TPtrC8 Prefix() const;

	/**
	Check if the given namespace is the same as the default namespace for this
	node.

	Note:  "" or NULL can be used to denote undefined namespace
    
    @param aNamespaceUri Namespace URI
	@return ETrue if the node is an element node and its default namespace URI is the same as the given namespace URI, EFalse otherwise
	@leave - One of the system-wide error codes
    */
    IMPORT_C TBool IsDefaultNamespaceL(const TDesC8& aNamespaceUri) const;

	/**
	Searches for the prefix bound to the given aNamespaceUri and applicable
	within the scope of this node.

    @see TXmlEngElement::LookupNamespaceByUriL()
    
    @param aNamespaceUri The Namespace URI to search for

    @return The sought prefix or NULL if not found or if aNamespaceUri is the default namespace
    @leave KXmlEngErrNullNode The node is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C TPtrC8 LookupPrefixL(const TDesC8& aNamespaceUri) const;

    /**
    Searches for the namespace URI that is bound to the given prefix.

    @see TXmlEngElement::LookupNamespaceByPrefixL(const TDesC8&)
     
    @param aPrefix The namespace prefix to search for
    @return The sought URI or NULL if the prefix is not bound
	@leave KXmlEngErrNullNode The node is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C TPtrC8 LookupNamespaceUriL(const TDesC8& aPrefix) const;

protected:
	/**
	Unlinks the node from the double-linked list and relinks any neighbour
	nodes.  Despite being removed from the list, the node retains links to its
	old neighbours!  Use with care!!
    
	No checks are made.  Neither the parent's, nor the node's properties are
	updated.
    */
	void DoUnlinkNode();

    /**
	Inserts this node before the specified node in the double-linked list.
    
	No checks are made.  Neither the parent's, nor the node's properties are
	updated.
    
	@param aNode After insertion, this node will come before aNode in the list
    */
    void LinkBefore(TXmlEngNode aNode);

protected:
	/** Node pointer */
    void* iInternal;

};

#include <xml/dom/xmlengnode.inl>

#endif /* XMLENGNODE_H */

