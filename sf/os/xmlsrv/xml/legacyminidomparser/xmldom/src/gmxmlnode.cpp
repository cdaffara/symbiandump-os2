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
// MDXMLNode.CPP
// @file
// This file contains the implementation of the CMDXMLNode class which
// is the base class for the Mini-DOM.
// This class forms the basis for the Mini-DOM.  It is a
// pure abstract class and it's sole concern is the node
// type and name and the management of the list of child
// nodes.
// The node name and type are set in the constructor and
// cannot be changed thereafter.
// The list of child nodes is managed as a doubly-linked
// list, not an array or similar structure.  Derived
// classes which are not allowed children will override the
// operations to manipulate the child list.
// 
//

#include <gmxmlnode.h>
#include <gmxmldomconstants.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif

CMDXMLNode::CMDXMLNode( TDOMNodeType aNodeType, TBool aCanHaveChildren, CMDXMLDocument* aOwnerDocument ):
	iElementType(KXMLUndefinedElement),
	iNodeType(aNodeType),
	iCanHaveChildren(aCanHaveChildren)
//
// The node type will normally be hard-wired in any derived class constructor.
// The node name and HaveChildren flag will normally be hard-wired in a derived class constructor unless it
// covers a generic element.
// @param aNodeType The tpe of node to be created
// @param aCanHaveChildren Flag to indicate if the node represents a node which is allowed children
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
//
	{
	iOwnerDocument = aOwnerDocument;
	}

EXPORT_C CMDXMLNode::~CMDXMLNode()
//
// When this node is deleted it will links its previous and next siblings across the gap, will
// delete all of its children and will correct its parents first and last child pointers if necessary.
//
	{
	// Perform housekeeping on parent and siblings - shouldn't be necessary as the links to
	// this element should have been removed first or we are in a tree delete but 
	// let's be helpful anyway.
	if((iNextSiblingPtr != NULL) && (iNextSiblingPtr->iPrevSiblingPtr == this))
		{
		iNextSiblingPtr->iPrevSiblingPtr = iPrevSiblingPtr;
		}
	if((iPrevSiblingPtr != NULL) && (iPrevSiblingPtr->iNextSiblingPtr == this))
		{
		iPrevSiblingPtr->iNextSiblingPtr = iNextSiblingPtr;
		}
	if( iParentPtr != NULL )
		{
		if(iParentPtr->iFirstChildPtr == this)
			{
			iParentPtr->iFirstChildPtr = iNextSiblingPtr;
			}
		if(iParentPtr->iLastChildPtr == this)
			{
			iParentPtr->iLastChildPtr = iPrevSiblingPtr;
			}
		}

	// Delete all children
	CMDXMLNode* childToDelete;
	CMDXMLNode* nextChild;
	childToDelete = iFirstChildPtr;
	while( childToDelete != NULL )
		{
		nextChild = childToDelete->iNextSiblingPtr;
		delete childToDelete;
		childToDelete = nextChild;
		}

	delete iNodeName;
	iNodeName = NULL;
	}

EXPORT_C void CMDXMLNode::SetNodeNameL( const TDesC& aNodeName)
//
// @param aNodeName The name of the node.
// @leave Can Leave due to OOM
//
	{
	iNodeName = HBufC::NewMaxL(aNodeName.Length());
	TPtr nodeAccess = iNodeName->Des();
	nodeAccess = aNodeName;
	}

EXPORT_C TPtrC CMDXMLNode::NodeName()
//
// @return Returns the name of the node
//
	{
	TPtrC retVal = iNodeName->Des();
	return retVal;
	}


EXPORT_C CMDXMLNode* CMDXMLNode::PreviousSameTypeSibling()
//
// @return Returns a pointer to the previous sibling node of the same type if any,
// oherwise returns NULL
//
	{
	CMDXMLNode* nodePtr = iPrevSiblingPtr;
	while((nodePtr != NULL) && nodePtr->iElementType != iElementType)
		{
		nodePtr = nodePtr->iPrevSiblingPtr;
		}
	return nodePtr;
	}

EXPORT_C CMDXMLNode* CMDXMLNode::NextSameTypeSibling()
//
// @return Returns a pointer to the next sibling node of the same type if any,
// oherwise returns NULL
//
	{
	CMDXMLNode* nodePtr = iNextSiblingPtr;
	while((nodePtr != NULL) && nodePtr->iElementType != iElementType)
		{
		nodePtr = nodePtr->iNextSiblingPtr;
		}
	return nodePtr;
	}

EXPORT_C TInt CMDXMLNode::RemoveChild(CMDXMLNode* aChildToRemove)
//
// Removes a child from the list of child nodes.  The child node is not deleted - that is the responsibility of the caller
// @param aChildToRemove Pointer to child to remove from the list
// @return Returns KERRNone if sucessful or KErrNotFound if the referenced child node is not found
//
	{
	TInt retVal=KErrNone;		
	
	if(aChildToRemove == NULL)
		{
		retVal = KErrNotFound;
		}
	else if( !iCanHaveChildren )
		{
		retVal = KErrXMLInvalidChild;
		}
	else if( aChildToRemove->iParentPtr != this )
		{
		retVal = KErrNotFound;
		}
	else
		{
		if(iFirstChildPtr == NULL)
			{
			retVal = KErrNotFound;
			}
		else 
			{
			if( aChildToRemove == iFirstChildPtr)
				{
				if( aChildToRemove == iLastChildPtr )
					{ // Only child so just zero pointers
					iFirstChildPtr = NULL;
					iLastChildPtr = NULL;
					}
				else
					{ // First but not only child
					iFirstChildPtr->iNextSiblingPtr->iPrevSiblingPtr = NULL;
					iFirstChildPtr = iFirstChildPtr->iNextSiblingPtr;
					}
				}
			else if( aChildToRemove == iLastChildPtr )
				{ // Last but not only child
				iLastChildPtr->iPrevSiblingPtr->iNextSiblingPtr = NULL;
				iLastChildPtr = iLastChildPtr->iPrevSiblingPtr;
				}
			else
				{ // Not first or last so somewhere in the list
				CMDXMLNode* nodeBefore = iFirstChildPtr;
				while((nodeBefore->iNextSiblingPtr != NULL) && 
					  (nodeBefore->iNextSiblingPtr != aChildToRemove))
					{
					nodeBefore = nodeBefore->iNextSiblingPtr;
					}
				if( nodeBefore->iNextSiblingPtr != aChildToRemove )
					{
					retVal = KErrNotFound;
					}
				else
					{
					nodeBefore->iNextSiblingPtr = aChildToRemove->iNextSiblingPtr;
					aChildToRemove->iNextSiblingPtr->iPrevSiblingPtr = nodeBefore;
					}
				}
			}
		}//end if

	if( retVal == KErrNone )
		{ // Having cleared this node off the list, clear its own pointers
		aChildToRemove->iPrevSiblingPtr = NULL;
		aChildToRemove->iNextSiblingPtr = NULL;
		aChildToRemove->iParentPtr = NULL;
		}
	return retVal;
	}

EXPORT_C TInt CMDXMLNode::InsertBefore(CMDXMLNode* aInsertBeforeChild, CMDXMLNode* aChildToInsert)
//
// Inserts a new child node at a specific point in the child list.
// @param aInsertBeforeChild Pointer to the element before which the new child should be inserted (NULL to insert at the start of the list)
// @param aChildToInsert Pointer to the new child node to insert.
// @return Returns KErrNone if successful, KErrXMLInvalidChild if the node cannot have children or KErrNotFound if it cannot fnd the child before which to insert
//
	{
	TInt retVal = KErrNone;	
	
	if(aChildToInsert == NULL)
		{
		retVal = KErrNotSupported;
		}
	else if( !iCanHaveChildren )
		{
		retVal = KErrXMLInvalidChild ;
		}
	else if(iFirstChildPtr == NULL && aInsertBeforeChild != NULL)
		{
		retVal = KErrNotFound;
		}
	else if((aInsertBeforeChild == NULL) || (aInsertBeforeChild == iFirstChildPtr))
		{ // Insert at start of list
		aChildToInsert->iParentPtr = this;
		if(iFirstChildPtr != NULL)
			{
			iFirstChildPtr->iPrevSiblingPtr = aChildToInsert;
			aChildToInsert->iNextSiblingPtr = iFirstChildPtr;
			}
		else
			{
			iLastChildPtr = aChildToInsert;
			aChildToInsert->iNextSiblingPtr = NULL;
			}
		iFirstChildPtr = aChildToInsert;
		}
	else
		{ // Find the insert before child as a check.
		CMDXMLNode* nodeBefore = iFirstChildPtr;
		while((nodeBefore->iNextSiblingPtr != NULL) && 
			  (nodeBefore->iNextSiblingPtr != aInsertBeforeChild))
			{
			nodeBefore = nodeBefore->iNextSiblingPtr;
			}
		if( nodeBefore->iNextSiblingPtr != aInsertBeforeChild )
			{
			retVal = KErrNotFound;
			}
		else
			{
			aChildToInsert->iParentPtr = this;
			aChildToInsert->iNextSiblingPtr = nodeBefore->iNextSiblingPtr;
			aChildToInsert->iPrevSiblingPtr = nodeBefore;
			nodeBefore->iNextSiblingPtr = aChildToInsert;
			aChildToInsert->iNextSiblingPtr->iPrevSiblingPtr = aChildToInsert;
			}
		}
	return retVal;
	}

EXPORT_C TInt CMDXMLNode::AppendChild(CMDXMLNode* aChildToInsert)
//
// Appends a new child at the end of the child list.
// @param aChildToInsert Pointer to the new child node to append.
// @return Returns KErrNone if successful or KErrXMLInvalidChild if the node cannot have children
//
	{
	TInt retVal = KErrNone;

	if(aChildToInsert == NULL)
		{
		retVal = KErrNotSupported;
		}
	else if( !iCanHaveChildren )
		{
		retVal = KErrXMLInvalidChild;
		}
	else
		{
		if(iFirstChildPtr == NULL)
			{
			iFirstChildPtr = aChildToInsert;
			}

		if(iLastChildPtr != NULL)
			{
			iLastChildPtr->iNextSiblingPtr = aChildToInsert;
			}

		aChildToInsert->iPrevSiblingPtr = iLastChildPtr;
		iLastChildPtr = aChildToInsert;
		aChildToInsert->iParentPtr = this;
		}
	return retVal;
	}

EXPORT_C TInt CMDXMLNode::ReplaceChild(CMDXMLNode* aChildToInsert, CMDXMLNode* aChildToReplace)
//
// Inserts a new child node while removing an existing one.  The old child is not deleted.
// @param aChildToInsert Pointer to the new child node to insert.
// @param aChildToReplace Pointer to the child node to be replaced.
// @return Returns KErrNone if successful, KErrXMLInvalidChild if the node cannot have children or KErrNotFound if it cannot fnd the child to replace.
//
	{
	TInt retVal = KErrNone;
	
	if(aChildToInsert == NULL)
		{
		retVal = KErrNotSupported;
		}
	else if(aChildToReplace == NULL)
		{
		retVal = KErrNotFound;
		}
	else if( !iCanHaveChildren )
		{
		retVal = KErrXMLInvalidChild ;
		}
	else
		{
		if((aChildToReplace == iFirstChildPtr) && (aChildToReplace == iLastChildPtr))
			{ // Replace only child
			iFirstChildPtr = aChildToInsert;
			iLastChildPtr = aChildToInsert;
			}
		else if(aChildToReplace == iFirstChildPtr)
			{ // Replace first but not only
			aChildToInsert->iNextSiblingPtr = iFirstChildPtr->iNextSiblingPtr;
			aChildToInsert->iNextSiblingPtr->iPrevSiblingPtr = aChildToInsert;
			iFirstChildPtr = aChildToInsert;
			}
		else if(aChildToReplace == iLastChildPtr)
			{ // Replace Last but not only
			aChildToInsert->iPrevSiblingPtr = iLastChildPtr->iPrevSiblingPtr;
			aChildToInsert->iPrevSiblingPtr->iNextSiblingPtr = aChildToInsert;
			iLastChildPtr = aChildToInsert;
			}
		else if(iFirstChildPtr != NULL)
			{ // Replace in the middle of the list
			CMDXMLNode* nodeBefore = iFirstChildPtr;
			while((nodeBefore->iNextSiblingPtr != NULL) && 
				  (nodeBefore->iNextSiblingPtr != aChildToReplace))
				{
				nodeBefore = nodeBefore->iNextSiblingPtr;
				}
			if( nodeBefore->iNextSiblingPtr != aChildToReplace )
				{
				retVal = KErrNotFound;
				}
			else
				{ // found so just cross link
				aChildToInsert->iNextSiblingPtr = aChildToReplace->iNextSiblingPtr;
				aChildToInsert->iPrevSiblingPtr = aChildToReplace->iPrevSiblingPtr;
				aChildToInsert->iPrevSiblingPtr->iNextSiblingPtr = aChildToInsert;
				aChildToInsert->iNextSiblingPtr->iPrevSiblingPtr = aChildToInsert;
				}
			}
		else
			{
			retVal = KErrNotFound;
			}
		}

	if( retVal == KErrNone )
		{ // Clear out the replaced node
		aChildToReplace->iPrevSiblingPtr = NULL;
		aChildToReplace->iNextSiblingPtr = NULL;
		aChildToReplace->iParentPtr = NULL;
		// Point the new child at its parent
		aChildToInsert->iParentPtr = this;
		}
	return retVal;
	}

// End Of File

