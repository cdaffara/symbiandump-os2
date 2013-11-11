/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file te_btracecmds.h
 @internalTechnology
*/
#ifndef TRACER_CMD
#define TRACER_CMD

#include <d32btrace.h>
#include <f32file.h>

static const int KPrimaryFilterMaxSize = 256;
extern char iPrimaryFilters[];

int strlen(const char* string);

class Tracer
	{
public:
	Tracer();
	~Tracer();
	int SetTraceFilters();
	int OpenFile(const TDesC& aName);
	int ReplaceFile(const TDesC& aName);
	int DumpTracesToFile();
public:
	RBTrace iTrace;
	RFs iFs;
	RFile iFile;
	RArray<TUint32> iSecondaryFilters;
	TPtrC iFileName;
	};

#endif
