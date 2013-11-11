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
// Node list inline functions
// Retrieves number of items in node list
//

template<class T> inline TInt RXmlEngNodeList<T>::Count() const
    {
    return iList.Count();
    }
    
// -----------------------------------------------------------------------------
// Returns true if this node has next sibling, false otherwise.
// -----------------------------------------------------------------------------
//
template<class T> inline TBool RXmlEngNodeList<T>::HasNext() const
    {
    return iList.HasNext();
    }    

// -----------------------------------------------------------------------------
// Retrieves next node
// -----------------------------------------------------------------------------
//
template<class T> inline T RXmlEngNodeList<T>::Next()
    {
    TXmlEngNode t = iList.Next(); 
    return *(static_cast<T*>(&t));
    }

// -----------------------------------------------------------------------------
// Closes node list
// -----------------------------------------------------------------------------
//
template<class T> inline void RXmlEngNodeList<T>::Close()
    {
    iList.Close();
    }

// -----------------------------------------------------------------------------
// Opens node list
// -----------------------------------------------------------------------------
//
template<class T> inline void RXmlEngNodeList<T>::OpenL( 
			void* aHead,
			TXmlEngNode::TXmlEngDOMNodeType aType,
			const TDesC8&	aName,
			const TDesC8&	aNs) 
    {
    iList.OpenL(aHead, aType, aName, aNs);    
    }

// -----------------------------------------------------------------------------
// Opens node list
// -----------------------------------------------------------------------------
//
template<class T> inline void RXmlEngNodeList<T>::Open( 
			void* aHead,
			TXmlEngNode::TXmlEngDOMNodeType aType)
    {
    iList.Open(aHead, aType);    
    }

