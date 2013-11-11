// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the declaration of the CMDXMLNode class which
// is the base class for the Mini-DOM.
// 
//

/**
 @file
*/

#ifndef __GMXMLNODE_H__
#define __GMXMLNODE_H__

#include <e32base.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <gmxmldocument.h>
#endif

// forward references
class CMDXMLComposer;
class CMDXMLDocument;
class CMDXMLEntityConverter;

class CMDXMLNode : public CBase
/** Abstract base class for DOM node classes.

It stores the node type and name, and manages a list of child nodes. The node 
name and type are set in the derived class's constructor and cannot be changed.

The list of child nodes is managed as a doubly-linked list, not an array 
or similar structure.

Derived classes that are not allowed children override the operations to manipulate 
the child list. 
@publishedPartner
@released
*/
{
public:
	// XML DOM Node Types - The actual values are taken from the DOM definition but we only use some of them.
	/** XML DOM node types.

	The values are taken from the DOM definition, but only some of them are used 
	for the SMIL API. 
@publishedPartner
@released
*/
	enum TDOMNodeType
		{
		EElementNode=1,					//< Element
		ETextNode=3,					//< Text
		ECDATASectionNode=4,			//< CDATA Section
		EProcessingInstructionNode=7,	//< Processing Instruction
		ECommentNode=8,					//< Comment
		EDocumentNode=9					//< Document
		};

	/** Destructor.
	
	When a node is deleted, it:
	
	1. links its previous and next siblings
	
	2. deletes all its children
	
	3. corrects its parents first and last child pointers if necessary */
	IMPORT_C virtual ~CMDXMLNode();

	/** Gets the node type of the node.
	
	@return The node type of the node */
	inline TDOMNodeType NodeType() {return iNodeType;}

	/** Gets the name of the node.
	
	@return The name of the node */
	IMPORT_C TPtrC NodeName();

	/** Tests if the node can have child nodes.
	
	@return True if the node can have child nodes, false otherwise */
	inline TBool CanHaveChildNodes() {return iCanHaveChildren;}

	/** Tests if the node has any child nodes.
	
	@return True if the node has any child nodes, false otherwise */
	inline TBool HasChildNodes() {return (iFirstChildPtr != NULL);}

	/** Gets a pointer to the document which is the owner of the DOM tree.
	
	@return A pointer to the document which is the owner of the DOM tree */
	inline CMDXMLDocument* OwnerDocument() {return (CMDXMLDocument*)iOwnerDocument;}

	/** Gets a pointer to the first child node, if any.
	
	@return A pointer to the first child node if any, otherwise NULL */
	inline CMDXMLNode* FirstChild() {return iFirstChildPtr;}

	/** Gets a pointer to the last child node, if any.
	
	@return A pointer to the last child node if any, otherwise NULL */
	inline CMDXMLNode* LastChild() {return iLastChildPtr;}

	/** Gets a pointer to the next sibling node, if any.
	
	@return A pointer to the next sibling node if any, otherwise NULL */
	inline CMDXMLNode* NextSibling() {return iNextSiblingPtr;}

	/** Gets a pointer to the previous sibling node, if any.
	
	@return A pointer to the previous sibling node if any, otherwise NULL */
	inline CMDXMLNode* PreviousSibling() {return iPrevSiblingPtr;}

	/** Gets a pointer to the parent node, if any.
	
	@return A pointer to the parent node if any, otherwise NULL (only at root 
	of tree) */
	inline CMDXMLNode* ParentNode() {return iParentPtr;}

	/** Gets the element type as an enumerated value.
	
	This function is quicker to use than NodeName().
	
	@return A CMDXMLNode::TDomNodeType value identifying the node type */
	inline TInt ElementType() {return iElementType;}

	/** Removes a child from the list of child nodes. 
	
	The child node is not deleted: that is the responsibility of the caller.
	
	@param aChildToRemove Pointer to child to remove from the list
	@return KErrNone if sucessful or KErrNotFound if the referenced child node 
	is not found */
	IMPORT_C TInt RemoveChild(CMDXMLNode* aChildToRemove);

	/** Inserts a new child node at a specific point in the child list. 
	
	@param aInsertBeforeChild Pointer to the element before which the new child 
	should be inserted. UseNULL to insert at the start of the list.
	@param aChildToInsert Pointer to the new child node to insert
	@return KErrNone if successful, KErrNotSupported if the node cannot have children 
	or KErrNotFound if it cannot find the child before which to insert */
	IMPORT_C TInt InsertBefore(CMDXMLNode* aInsertBeforeChild, CMDXMLNode* aChildToInsert);

	/** Appends a new child at the end of the child list. 
	
	@param aChildToInsert Pointer to the new child node to append.
	@return KErrNone if successful or KErrNotSupported if the node cannot have 
	children */
	IMPORT_C TInt AppendChild(CMDXMLNode* aChildToInsert);

	/** Inserts a new child node while removing an existing one. 
	
	The old child is not deleted. 
	
	@param aChildToInsert Pointer to the new child node to insert
	@param aChildToReplace Pointer to the child node to be replaced
	@return KErrNone if successful, KErrNotSupported if the node cannot have children 
	or KErrNotFound if it cannot find the child to replace. */
	IMPORT_C TInt ReplaceChild(CMDXMLNode* aChildToInsert, CMDXMLNode* aChildToReplace);


	/** Checks the children of this node for validity.
	
	@return True if the children are valid according to the DTD. */
	IMPORT_C virtual TBool CheckChildren()=0;

protected:
	/*
	 * The node type will normally be hard-wired in any derived class constructor.
	 * The node name and HaveChildren flag will normally be hard-wired in a derived class constructor unless it
     * covers a generic element.
	 * @param aNodeType The tpe of node to be created
	 * @param aCanHaveChildren Flag to indicate if the node represents a node which is allowed children
	 * @param aOwnerDocument Pointer to the document at the root of the DOM tree - if NULL then assume this is the root
	 */
	CMDXMLNode( TDOMNodeType aNodeType, TBool aCanHaveChildren, CMDXMLDocument* aOwnerDocument );

// make these public? sjr
	/**
	 * Gets a pointer to the previous sibling node of the same type if any,
	 * otherwise returns NULL
	 * @return Previous sibling node
	 */
	IMPORT_C CMDXMLNode* PreviousSameTypeSibling();

	/**
	 * Gets a pointer to the next sibling node of the same type if any,
	 * otherwise returns NULL
	 * @return Next sibling node
	 */
	IMPORT_C CMDXMLNode* NextSameTypeSibling();

	/**
	 * Set the node name - normally on creation
	 * @param aNodeName The node name - commonly an element tag name - _not_ any unique identifier
	 * @leave Can Leave due to OOM
	 */
	IMPORT_C void SetNodeNameL(const TDesC& aNodeName);
	
	/**
	 * Sets the enumerated element type (same information as node name but faster to access)
	 */
	inline void SetElementType( TInt aElementType ) {iElementType = aElementType;}

protected:
	CMDXMLDocument* iOwnerDocument;	//< Pointer to owning document	


private:
	TInt iElementType;				// Enumerated element type
	TDOMNodeType iNodeType;			// Node type
	HBufC* iNodeName;				// Name of the node, e.g. element tag name - not unique id
	CMDXMLNode* iParentPtr;			// Pointer to parent node if any
	CMDXMLNode* iFirstChildPtr;		// Pointer to first child node if any
	CMDXMLNode* iLastChildPtr;		// Pointer to last child node if any
	CMDXMLNode* iNextSiblingPtr;	// Pointer to next sibling if any
	CMDXMLNode* iPrevSiblingPtr;	// Pointer to previous sibling if any
	TBool iCanHaveChildren;			// true if this node is allowed to have children
};

#endif
// End Of File
