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
// Node functions implementation
//

inline TXmlEngNode::TXmlEngNode()
        : iInternal(NULL) {}

inline TXmlEngNode::TXmlEngNode(void* aInternal)
        : iInternal(aInternal) {}

inline TBool TXmlEngNode::IsNull() const
    {return iInternal == NULL;}

inline TBool TXmlEngNode::NotNull() const
    {return iInternal != NULL;}

// ------------------------------------------------------------------------
// Compares nodes.
//
// The nodes are the same if they are referring to the same in-memory
// data structure. 
// ------------------------------------------------------------------------
//
inline TBool TXmlEngNode::IsSameNode(TXmlEngNode aOther) const
    {return iInternal == aOther.iInternal;}

inline TXmlEngNode TXmlEngNode::MoveToL(TXmlEngNode aParent)
    {return aParent.AppendChildL(Unlink());}

inline TXmlEngNode TXmlEngNode::MoveTo(TXmlEngNode aParent)
    {return aParent.AppendChildL(Unlink());}

inline TXmlEngAttr& TXmlEngNode::AsAttr() const                    
    {return *reinterpret_cast<TXmlEngAttr*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngElement&     TXmlEngNode::AsElement() const                    
    {return *reinterpret_cast<TXmlEngElement*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngTextNode& TXmlEngNode::AsText() const    
    {return *reinterpret_cast<TXmlEngTextNode*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngBinaryContainer& TXmlEngNode::AsBinaryContainer() const    
    {return *reinterpret_cast<TXmlEngBinaryContainer*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngChunkContainer& TXmlEngNode::AsChunkContainer() const    
    {return *reinterpret_cast<TXmlEngChunkContainer*>(const_cast<TXmlEngNode*>(this));}   

inline TXmlEngFileContainer& TXmlEngNode::AsFileContainer() const    
    {return *reinterpret_cast<TXmlEngFileContainer*>(const_cast<TXmlEngNode*>(this));} 

inline TXmlEngDataContainer& TXmlEngNode::AsDataContainer() const    
    {return *reinterpret_cast<TXmlEngDataContainer*>(const_cast<TXmlEngNode*>(this));}      

inline TXmlEngNamespace& TXmlEngNode::AsNamespace() const
    {return *reinterpret_cast<TXmlEngNamespace*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngComment& TXmlEngNode::AsComment() const        
    {return *reinterpret_cast<TXmlEngComment*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngCDATASection& TXmlEngNode::AsCDATASection()    const
    {return *reinterpret_cast<TXmlEngCDATASection*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngEntityReference& TXmlEngNode::AsEntityReference() const
    {return *reinterpret_cast<TXmlEngEntityReference*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngProcessingInstruction& TXmlEngNode::AsProcessingInstruction() const
    {return *reinterpret_cast<TXmlEngProcessingInstruction*>(const_cast<TXmlEngNode*>(this));}

inline TXmlEngDocumentFragment& TXmlEngNode::AsDocumentFragment() const
    {return *reinterpret_cast<TXmlEngDocumentFragment*>(const_cast<TXmlEngNode*>(this));}
