/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TSTCLIPB_H__
#define __TSTCLIPB_H__

#include <e32std.h>
#include <e32base.h>

class RFs;
class CFileStore;
class CStreamDictionary;


class CClipboard : public CBase
/**
Provdes a clipboard class, capable of storing multiple data *types*
All data *types* stored must be registered prior to closing this clipboard.
@internalComponent
*/
    {
public:
	IMPORT_C static CClipboard* NewForReadingLC(RFs& aFsSession);
	IMPORT_C static CClipboard* NewForWritingLC(RFs& aFsSession);
    IMPORT_C ~CClipboard();
	//
	// Necessary only when writing
	IMPORT_C void CommitL();
	//
	inline CFileStore& Store() const;
	inline CStreamDictionary& StreamDictionary() const;
private:
	CClipboard(RFs& aFsSession);
	void ConstructForWritingL(const TDesC& aFileName);
	void ConstructForReadingL(const TDesC& aFileName);
	void CreateDirectoryL(const TDesC& aFileName);
private:
	CFileStore* iStore;
	CStreamDictionary* iStreamDictionary;
	RFs& iFsSession;
    };

inline CFileStore& CClipboard::Store() const
	{ return *iStore; }

inline CStreamDictionary& CClipboard::StreamDictionary() const
	{ return *iStreamDictionary; }

#endif
