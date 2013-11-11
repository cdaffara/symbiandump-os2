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
// Serialization options functions
//

#include <xml/dom/xmlengserializationoptions.h>

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TXmlEngSerializationOptions::TXmlEngSerializationOptions( TUint aOptionFlags,
												  	   const TDesC8& aEncoding)
        : iOptions(aOptionFlags), 
          iEncoding(aEncoding), 
          iNodeFilter(NULL),
          iDataSerializer(NULL)
    {
    }
    
// -----------------------------------------------------------------------------
// Sets node filter
// -----------------------------------------------------------------------------
//
EXPORT_C void TXmlEngSerializationOptions::SetNodeFilter(MXmlEngNodeFilter* aFilter) 
    {
    iNodeFilter = aFilter;
    }

EXPORT_C void TXmlEngSerializationOptions::SetDataSerializer(MXmlEngDataSerializer* aSerializer)
    {
    iDataSerializer = aSerializer;
    }
