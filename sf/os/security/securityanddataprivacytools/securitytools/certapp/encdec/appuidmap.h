#ifndef __APPUIDMAP_H__
#define __APPUIDMAP_H__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "encdec.h"
/**
 * @file
 * @internalComponent
 */

class AppUidMap
	{
public:
	// Insert new entry, if ok returns true and aLastIndex/aFirstDef
	// are not set.
	//
	// Duplicate Uids are NOT allowed. Duplicate definitions/names
	// (mapping to different UIDs) are allowed, but not recommended.
	//
	// If a duplicate UID is seen, then aLastIndex is set to the file
	// index of the LAST definition seen and aFirstName is set to the
	// FIRST definition/name saved for aUid. Then aInputFileIndex is
	// saved as the new LAST definition file index, but the saved
	// definition/name is NOT updated.
	//
	// The returned aLastIndex is used to check if there is more than
	// one definition within a single input file (which is an error if
	// the input file is a human readable config file).
	//
	// The returned aFirstDef gives the first/winning definition.
	static bool InsertUidDefinition(TInt32 aUid, const std::string &aDef, TInt32 aInputFileIndex,
										TInt32 &aLastIndex, std::string &aFirstDef);

	static void GenerateEnumEntries();
	static EnumEntry *EnumEntries();
private:
	};




#endif
