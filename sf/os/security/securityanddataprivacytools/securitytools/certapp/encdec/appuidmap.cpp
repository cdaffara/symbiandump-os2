/*
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


#include "appuidmap.h"
#include <map>

struct AppMapEntry
	{
	std::string iDef;
	TInt32 iLastDefFileIndex;
	};
typedef std::map<TUint32, AppMapEntry> AppMap;

//
// Local static vars. This avoids including map etc in the header
//
static AppMap sAppMap;

bool AppUidMap::InsertUidDefinition(TInt32 aUid, const std::string &aDef, TInt32 aInputFileIndex,
									TInt32 &aLastIndex, std::string &aFirstDef)
{
	AppMapEntry val;
	val.iDef = aDef;
	val.iLastDefFileIndex = aInputFileIndex;
	AppMap::value_type e(aUid, val);
	std::pair<AppMap::iterator,bool> result = sAppMap.insert(e);
	if(result.second == true)
		{
		// Inserted new entry
		return true;
		}
	// Duplicate.
	// Save fileIndex of previous definition
	aLastIndex = (result.first)->second.iLastDefFileIndex;
	aFirstDef = (result.first)->second.iDef;
	// Update location of last seen def
	(result.first)->second.iLastDefFileIndex = aInputFileIndex;

	return false;
}






static EnumEntry *sEnumEntries = 0;

void AppUidMap::GenerateEnumEntries()
{
	delete [] sEnumEntries;
	sEnumEntries = new EnumEntry[sAppMap.size()+1];
	sEnumEntries[sAppMap.size()].iName=0; // Terminate array

	AppMap::const_iterator it = sAppMap.begin();
	for(TUint32 i=0; i < sAppMap.size(); ++i)
		{
		sEnumEntries[i].iName = (*it).second.iDef.c_str();
		sEnumEntries[i].iValue = (*it).first;
		++it;
		}
}


EnumEntry *AppUidMap::EnumEntries()
{
BULLSEYE_OFF
	if(sEnumEntries == 0) FatalError();
BULLSEYE_RESTORE
	return sEnumEntries;
}

// End of file
