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
// Methods for memory chunk container
//

#include <xml/dom/xmlengchunkcontainer.h>
#include "xmlengdomdefs.h"
#include <stdapis/libxml2/libxml2_tree.h>

// -------------------------------------------------------------------------------
// @return Memory chunk reference
//
// -------------------------------------------------------------------------------
//
EXPORT_C RChunk& TXmlEngChunkContainer::Chunk() const
    {
  	return (RChunk&) *LIBXML_NODE->content; 
    }

// -------------------------------------------------------------------------------
// @return Offset of binary data in memory chunk
//
// -------------------------------------------------------------------------------
//
EXPORT_C TUint TXmlEngChunkContainer::ChunkOffset() const
    {
    return (TUint)LIBXML_NODE->nsDef;
    }
