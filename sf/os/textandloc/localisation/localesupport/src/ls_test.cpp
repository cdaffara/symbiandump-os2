/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The LCharSet object used by the Test locale.
* Generated by COLTAB.
*
*/


#include "ls_std.h"
#include <collate.h>

const TUint KUidTestCollationMethod = 0x1000B07C;

static const TUint32 TheTestKey[] = 
	{
	0x74b0109,0x74b0121,0x71f0109,0x71f0121,0x7030109,0x7030121,0x6f70109,0x6f70121,
	0x6e30109,0x6e30121,0x6cf0109,0x6cf0121,
	};

static const TUint32 TheTestIndex[] = 
	{
	0x410001,0x420003,0x430005,0x440007,0x450009,0x46000b,0x610000,0x620002,
	0x630004,0x640006,0x650008,0x66000a,
	};

static const TCollationKeyTable TheTestTable = 
	{ TheTestKey, TheTestIndex, 12, NULL, NULL, 0 };

static const TCollationMethod TheCollationMethod[] = 
	{
		{
		KUidTestCollationMethod, // the method for the locale
		NULL, // use the standard table as the main table
		&TheTestTable, // the locale values override the standard values
		0 // the flags are standard
		},
		{
		KUidBasicCollationMethod, // the standard unlocalised method
		NULL, // null means use the standard table
		NULL, // there's no override table
		0 // the flags are standard
		}
	};

static const TCollationDataSet TheCollationDataSet =
	{
	TheCollationMethod,
	2
	};

// The one and only locale character set object.
const LCharSet TheCharSet =
	{
	NULL,
	&TheCollationDataSet
	};