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
// Interface class describing class that may be used as
// output stream for dom tree
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGMOUTPUTSTREAM_H
#define XMLENGMOUTPUTSTREAM_H

#include <e32base.h>



/**
Provides an interface for writing serialized XML data to a stream.  This
interface should be implemented by client applications when the
RXmlEngDocument::SaveL() method is used to serialize XML data to a stream.

No ownership is transferred by RXmlEngDocument::SaveL(), thus an object
of the implementing class should be freed after use.

@see RXmlEngDocument::SaveL()
*/
class MXmlEngOutputStream
    {
public:
    /**
	Writes the specified buffer to the output stream.
	@param aBuffer The data to write
    @return The number of bytes written or -1 in the case of an error
    */
    virtual TInt Write(const TDesC8& aBuffer) = 0;

    /**
	Closes the output stream.
	@return 0 if successful or -1 in the case of an error
    */
    virtual TInt Close() = 0;
    };

#endif /* XMLENGMOUTPUTSTREAM_H */  

