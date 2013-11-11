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
// MDXMLCharacterData.cpp
// @file
// This class represents a text section of an XML file -
// basically it is a chunk of text which has a place in the
// object tree but has no children.
// 
//

#include <gmxmldomconstants.h>
#include <gmxmlnode.h>
#include <gmxmlcharacterdata.h>

CMDXMLCharacterData::CMDXMLCharacterData( TDOMNodeType aNodeType, CMDXMLDocument* aOwnerDocument ):
CMDXMLNode( aNodeType, false, aOwnerDocument)
//
// Constructor
// @param aNodeType The node type of the derived object
// @param aOwnerDocument The Document at the root of the DOM tree.
//
	{
	}

EXPORT_C CMDXMLCharacterData::~CMDXMLCharacterData()
	{
	delete iData;
	}

EXPORT_C TDesC& CMDXMLCharacterData::Data()
//
// @return Returns the text of the Character Data section
//
	{
	return *iData;
	}

EXPORT_C void CMDXMLCharacterData::SetDataL( TDesC& aData )
// 
// Sets the whole of the Character Data section text
// @param aData Text to set.
// @leave Can Leave due to OOM
//
	{
	if( iData != NULL )
		{
		delete iData;
		iData = NULL;
		}
	iData = aData.AllocL();
	}

EXPORT_C TInt CMDXMLCharacterData::Length()
//
// @return Returns the length of the text
//
	{
	return iData->Length();;
	}

EXPORT_C void CMDXMLCharacterData::AppendL( TDesC& aData )
//
// Appends text to the Character Data section
// @param aData Text to append
// @leave Can Leave due to OOM
//
	{
	TInt newLength = aData.Length() + iData->Length();
	if(newLength > iData->Des().MaxLength())
		{
		HBufC* newData = HBufC::NewMaxL(newLength);
		newData->Des().Copy(iData->Des());
		delete iData;
		iData = newData;
		}
	iData->Des().Append(aData);
	}

EXPORT_C TInt CMDXMLCharacterData::InsertL( TInt aInsertPos, TDesC& aData )
//
// Inserts text at a point in the Character Data section
// @param aInsertPos The position to insert the text (0 means insert at the start)
// @param aData Text to insert
// @leave Can Leave due to OOM
// @return Returns KErrNone if successful or KErrNotFound if the insert position is out of range
//
	{
	TInt retVal = KErrNone;
	if((aInsertPos >= 0) && (aInsertPos < iData->Length()))
		{
		TInt newLength = aData.Length() + iData->Length();
		if(newLength > iData->Des().MaxLength())
			{
			HBufC* newData = HBufC::NewMaxL(newLength);
			newData->Des().Copy(iData->Des());
			delete iData;
			iData = newData;
			}
		iData->Des().Insert(aInsertPos, aData);
		}
	else
		{
		retVal = KErrNotFound;
		}
	return retVal;
	}

EXPORT_C TInt CMDXMLCharacterData::ReplaceL( TInt aInsertPos, TInt aLength, TDesC& aData )
//
// Replaces a block of text in the Character Data section
// @param aInsertPos The position to insert the text (0 means insert at the start)
// @param aLength The number of characters to replace
// @param aData Text to insert
// @leave Can Leave due to OOM
// @return Returns KErrNone if successful or KErrNotFound if the replace block is out of range
//
	{
	TInt retVal = KErrNone;
	if((aInsertPos >= 0) && (aInsertPos < iData->Length()) && (aLength >= 0))
		{
		TInt newLength = aData.Length() + iData->Length() - aLength;
		if(newLength > iData->Des().MaxLength())
			{
			HBufC* newData = HBufC::NewMaxL(newLength);
			newData->Des().Copy(iData->Des());
			delete iData;
			iData = newData;
			}
		iData->Des().Replace(aInsertPos, aLength, aData);
		}
	else
		{
		retVal = KErrNotFound;
		}
	return retVal;
	}

EXPORT_C TBool CMDXMLCharacterData::CheckChildren()
//
// Check the children of this node for legality - must be defined based on DTD.
// For a Character Data section there can be no children so this always returns true.
// @return True if the node has legitimate children
//
	{
	return true;
	}

// End Of File
