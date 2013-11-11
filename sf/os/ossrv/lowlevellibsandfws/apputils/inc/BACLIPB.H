// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by DS, October 1996
// Clipboard
// 
//

#if !defined(__BACLIPB_H__)
#define __BACLIPB_H__

#if !defined(__e32std_h__)
#include <e32std.h>
#endif

#if !defined(__e32base_h__)
#include <e32base.h>
#endif

#if !defined(__s32file_h__)
#include <s32file.h>
#endif

class CClipboard : public CBase
/** Clipboard. A repository of copied data which is available for pasting. The 
clipboard storage drive is defined in the HAL layer, in HAL::EClipboardDrive. 
@publishedAll
@released
*/
    {
public:
	IMPORT_C static CClipboard* NewForReadingLC(RFs& aFs);
	IMPORT_C static CClipboard* NewForReadingL(RFs& aFs);
	IMPORT_C static CClipboard* NewForWritingLC(RFs& aFs);
	IMPORT_C static TInt Clear(RFs& aFs);
	IMPORT_C ~CClipboard();
	//
	// Necessary only when writing
	IMPORT_C void CommitL();
	//
	inline CStreamStore& Store() const;
	inline CStreamDictionary& StreamDictionary() const;

    // Provide functionality to read and write floating point numbers
    // in a native format.
    // (see note below)

    IMPORT_C void  CopyToL     ( TReal aReal ) __SOFTFP;
    IMPORT_C TBool PasteFromL  ( TReal& aReal );
    // Get a number from the clipboard, return false if it is not present
    // in which case the parameter is unmodified.

	// Gets the drive where the clipboard file is stored
	IMPORT_C static TDriveName ClipboardFileDrive();
	 

private:
	inline CClipboard(RFs& aFs);
	static CClipboard* NewLC(RFs& aFs);
	void ConstructReadL();
private:
	CFileStore* iStore;
	CStreamDictionary* iStreamDictionary;
	RFs& iFs;
    };


inline CStreamStore& CClipboard::Store() const
	/** Returns a reference to the clipboard's file store.
	
	@return A reference to the clipboard's store. */
	{ return *iStore; }


inline CStreamDictionary& CClipboard::StreamDictionary() const
	/** Returns a reference to the clipboard's stream dictionary.
	
	@return A reference to the clipboard's stream dictionary. */
	{ return *iStreamDictionary; }


// NB the floating point copy and paste member functions are currently the only
// ones of this type defined in the class.  Most clipboard data consists of
// more complex objects known only at a higher level than BAFL.  This raises the
// question of whether the clipboard should provide equivalent functionality
// for other basic data types or whether there should be a utility class
// elsewhere to do all such operations in a standard format and handle the
// multiple representations too. - PNJ, January 1997.






#endif
