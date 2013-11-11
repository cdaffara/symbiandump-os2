// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <bautils.h>
#include "logservpanic.h"
#include "LogServResourceInterpreter.h"

//LogWrap resoure file name
_LIT(KResourceFileWrapper, "\\resource\\logeng\\logwrap.rsc");

const TInt KStringsArrayGranularity = 20;

/**
Creates a CLogServResourceInterpreter instance.
The created CLogServResourceInterpreter instance will load into memory the content of the LogWrap
resource file.
  
@param aFs Reference to a file session object, used later for reading the content of the resource file.

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes.
*/
CLogServResourceInterpreter* CLogServResourceInterpreter::NewL(RFs& aFs)
	{
	CLogServResourceInterpreter* self = new (ELeave) CLogServResourceInterpreter(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destroys CLogServResourceInterpreter object deallocating all previously allocated resources
(the resource file, the memory for the loaded resource strings) 
*/
CLogServResourceInterpreter::~CLogServResourceInterpreter()
	{
	iFile.Close();
	for(TInt i=iStrings.Count()-1;i>=0;--i)
		{
		delete iStrings[i].iString;
		}
	iStrings.Close();
    }

/**
Creates a resource reader object for the resource identified by the aId parameter.

@param aReader Output parameter. The resource reader object, created by this call.
@param aId Resource item identifier.

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes. 
*/
void CLogServResourceInterpreter::CreateResourceReaderLC(TResourceReader& aReader, TInt aId)
	{
	HBufC8* string = GetStringL(aId);		
	//Resource reader needs to be created with a copy of the string resource as otherwise
	//our "HBufC8*" string will be deleted when the resource reader is deleted!
	aReader.SetBuffer(string->AllocLC());
	}

/**
Initializes CLogServResourceInterpreter data members with their default values.
@param aFs Reference to a file session object, used later for reading the content of the resource file.  
*/
CLogServResourceInterpreter::CLogServResourceInterpreter(RFs& aFs) :	
	iFs(aFs), 
	iStrings(KStringsArrayGranularity)
	{
	}

/**
Loads the content of the LogWrap resource file.
*/
void CLogServResourceInterpreter::ConstructL()
	{
	// Find the resource file
  TFileName fileName;
	fileName.Copy(RProcess().FileName());

  TParse parse;
  parse.Set(KResourceFileWrapper, &fileName, NULL);
	fileName = parse.FullName();

	// Get language of resource file
	BaflUtils::NearestLanguageFile(iFs, fileName);

	// Check the entry exists on this drive (e.g. if we are running the log server
	// from RAM, then default to the ROM if no RSC on the current drive exists).
	TEntry fsEntry;
	TInt err = iFs.Entry(fileName, fsEntry);
	if(err == KErrNotFound || err == KErrPathNotFound)
		{
		// Switch to ROM (we might already have been launching from the ROM,
		// in which case this will have no effect anyway).
		fileName[0] = 'Z';
		}
	iFile.OpenL(iFs, fileName);
	}

/**
Attempts to read the resource string, identified by the aId parameter, from 
the iStrings array. If the resource stringis not there, it will be loaded from the
resource file and put in iStrings array.
@param aId Resource string identifier.
@return HBufC8 object containing the requested string 
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes. 
*/
HBufC8* CLogServResourceInterpreter::GetStringL(TInt aId)
	{
	TLinearOrder<TResourceString> order(&CLogServResourceInterpreter::Compare);
	//Try to find the requested resource in the cached strings
	TInt idx = iStrings.FindInOrder(TResourceString(aId, NULL), order);
	if(idx == KErrNotFound)
		{
		//String not in the cache, load it from the resource file
		iStrings.ReserveL(iStrings.Count() + 1);
		HBufC8* buf = iFile.AllocReadL(aId);
		TResourceString entry(aId, buf);
		#ifdef _DEBUG
		TInt err = 
		#endif	
		iStrings.InsertInOrder(entry, order);
		__ASSERT_DEBUG(err == KErrNone, User::Invariant());
		return buf;
		}
	return iStrings[idx].iString;
	}

/**
Compares two iStrings entries.
Used for sorting/finding entries in iStrings sorted array.

@param  aLeft  Left entry to be compared
@param  aRight Right entry to be compared
@return -1 aLeft is less than aRight, 0 entries are equal, 1 aLeft is bigger than aRight 
*/
TInt CLogServResourceInterpreter::Compare(const CLogServResourceInterpreter::TResourceString& aLeft, 
 										  const CLogServResourceInterpreter::TResourceString& aRight)
	{
	TInt64 res = (TInt64)aLeft.iId - (TInt64)aRight.iId;
	return res > 0LL ? 1 : (res < 0LL ? -1 : 0);
	}
