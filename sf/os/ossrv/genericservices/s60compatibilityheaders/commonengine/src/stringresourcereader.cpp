// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//




#include <tulstringresourcereader.h>
#include "stringresourcereader.h"

EXPORT_C CStringResourceReader* CStringResourceReader::NewL(TFileName& aResFile)
	{ return reinterpret_cast<CStringResourceReader*>(CTulStringResourceReader::NewL(aResFile)); }
	
EXPORT_C CStringResourceReader* CStringResourceReader::NewLC(TFileName& aResFile)
	{ return reinterpret_cast<CStringResourceReader*>(CTulStringResourceReader::NewLC(aResFile)); }

EXPORT_C CStringResourceReader* CStringResourceReader::NewL(TFileName& aResFile, RFs& aFs)
	{ return reinterpret_cast<CStringResourceReader*>(CTulStringResourceReader::NewL(aResFile, aFs)); }

EXPORT_C CStringResourceReader* CStringResourceReader::NewLC(TFileName& aResFile, RFs& aFs)
	{ return reinterpret_cast<CStringResourceReader*>(CTulStringResourceReader::NewLC(aResFile, aFs)); }

EXPORT_C const TDesC& CStringResourceReader::ReadResourceString(TInt aResourceId)
	{ return reinterpret_cast<CTulStringResourceReader*>(this)->ReadResourceString(aResourceId); }
