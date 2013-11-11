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
// Implementation of XOP serializer Output Stream
//

#include <bafl/sysutil.h>
#include "xmlengsxopoutputstream.h"

TInt TXmlEngSXOPOutputStream::Write(const TDesC8 &aBuffer)
	{
	iStream->Write(aBuffer);
	};
	
TInt TXmlEngSXOPOutputStream::Close()
	{
	return KErrNone;
	}
	
TInt TXmlEngSXOPOutputStream::CheckError()
    {
    return iError;
    }
	
TXmlEngSXOPOutputStream::TXmlEngSXOPOutputStream(MXmlEngOutputStream& aStream)
	{
	iStream = &aStream;
	}
 
