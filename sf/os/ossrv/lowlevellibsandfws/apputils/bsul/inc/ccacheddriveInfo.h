// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains declaration for BSUL class to cache drive information
// 
//

/**
 @file
*/

#ifndef CCACHEDDRIVEINFO_H
#define CCACHEDDRIVEINFO_H

#include <e32def.h>
#include <f32file.h>

namespace BSUL
	{
	/**
	 Class used to cache F32 drive information.
	 @see TDriveInfo
	 @publishedAll
	 @released
	 */
	NONSHARABLE_CLASS(CCachedDriveInfo) : public CBase
		{
	private: // private type declarations
		// Struct to hold drive and media attributes retrieved from a TDriveInfo.
		struct TDriveAndMediaAttributes
			{
			// bit-field of drive attributes
			TUint32 iDriveAttributes;
			// bit-field of media attributes
			TUint32 iMediaAttributes;
			// Media type of the drive
			TMediaType iMediaType;
			};
		
	public: // public methods
		IMPORT_C static CCachedDriveInfo* NewL(RFs& aFs);
		IMPORT_C static CCachedDriveInfo* NewLC(RFs& aFs);
		IMPORT_C ~CCachedDriveInfo();
		
		IMPORT_C TBool IsReadOnlyInternalL(const TDesC& aFullName) const;
		IMPORT_C TBool IsReadOnlyInternalL(TDriveUnit aDrive) const;
		IMPORT_C TBool IsFlagSetOnDriveL(TDriveUnit aDrive, TUint aFlags) const;
		IMPORT_C TBool MediaTypeL(TDriveUnit aDrive, TMediaType aTestValue, TMediaType* aActual = NULL) const;
		IMPORT_C TDriveUnit GetDefaultRemovableMemoryCardDriveL() const;
		
	private: // private methods
		CCachedDriveInfo();
		void ConstructL(RFs& aFs);
		
		static TDriveUnit PathToDriveUnitL(const TDesC& aFullName);
		
	private: // private data
		// Array containing the cached attributes for every drive. 
		TDriveAndMediaAttributes iDriveAndMediaAttributes[KMaxDrives];
		};
	}

#endif // #ifndef CCACHEDDRIVEINFO_H
