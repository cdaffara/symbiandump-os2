// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_TESTCONSTANTS_H__
#define __T_TESTCONSTANTS_H__


#include <e32test.h>


const TInt KErrorCodeOnStartDocument			= 0x01;
const TInt KErrorCodeOnEndDocument				= 0x02;
const TInt KErrorCodeOnStartElement				= 0x03;
const TInt KErrorCodeOnEndElement				= 0x04;
const TInt KErrorCodeOnContent					= 0x05;
const TInt KErrorCodeOnStartPrefixMapping		= 0x06;
const TInt KErrorCodeOnEndPrefixMapping			= 0x07;
const TInt KErrorCodeOnIgnorableWhiteSpace		= 0x08;
const TInt KErrorCodeOnSkippedEntity			= 0x09;
const TInt KErrorCodeOnProcessingInstruction	= 0x0A;
const TInt KErrorCodeOnExtension				= 0x0B;


#endif // __T_TESTCONSTANTS_H__
