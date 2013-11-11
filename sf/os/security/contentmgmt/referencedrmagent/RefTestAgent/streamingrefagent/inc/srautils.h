/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Describes some utility functions for reference agent.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRAUTILS_H
#define SRAUTILS_H

#include "srarightsobject.h"
#include "sdpmediafield.h"
#include <f32file.h>


IMPORT_C TBool CheckKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream, const TDesC8& aSupportedKmsIds);
IMPORT_C void DoSetSdpMediaFieldL(RFs& aFs, CSdpMediaField*& aSdp, CSraRightsObject*& aRo, const TDesC8& aSdpData, const TDesC& aPrivateFolder);
IMPORT_C void DoSetRightsObjectL(RFs& aFs, CSdpMediaField& aSdp, CSraRightsObject*& aRo, const TDesC& aPrivateFolder);

#endif