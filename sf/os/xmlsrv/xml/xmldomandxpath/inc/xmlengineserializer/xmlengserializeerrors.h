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
// Errors code for serialization and deserialization
//



/**
 @file
 @publishedAll
 @released
*/
#include <e32base.h>

#ifndef XMLENGSERIALIZERERRORS_H
#define XMLENGSERIALIZERERRORS_H

/** Serializer or deserializer was not configured correctly */
const TInt KXmlEngErrNoParameters = -32384;

/** No binary data was found for XOP data Id */
const TInt KXmlEngErrBinDataNotFound = -32385;

/** Base 64 decoding failed */
const TInt KXmlEngErrDecodingFailed = -32386;

/** DOM tree does not contain XOP Infoset */
const TInt KXmlEngErrBadInfoset = -32387;

/** The multipart document could not be parsed */
const TInt KXmlEngErrBadMultipart = -32388;

#endif // XMLENGSERIALIZERERRORS_H

