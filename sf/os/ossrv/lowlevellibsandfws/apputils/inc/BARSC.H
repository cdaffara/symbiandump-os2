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
//

#ifndef __BARSC_H__
#define __BARSC_H__

#include <f32file.h>

//Forward declarations
class RResourceFileImpl;

/** Accesses a resource file and reads the resource data into a buffer. 

Interpretation of the data is achieved using the TResourceReader class.

RResourceFile instance behaviour when some problem occurs - it panics. Or asserts,
if the used method is not "L" method.

Expected behaviour when assignment operator or copy constructor is called:
The class doesn't have assignment operator and copy constructor, so the compiler generated
ones will be used. The heap buffers used by the source class instance will be shared with the 
destination class instance.

Expected behaviour when the RResourceFile instance is about to be destroyed:
The class doesn't have destructor so compiler generated one will be used.
Always call Close() to free allocated by the instance resources.

@see TResourceReader 
@publishedAll
@released
*/
class RResourceFile
	{
public:
	IMPORT_C RResourceFile();
	IMPORT_C void Close();
	IMPORT_C void OpenL(RFs &aFs,const TDesC &aName);
	IMPORT_C TUidType UidType() const;
	IMPORT_C void ReadL(TDes8 &aDes,TInt aResourceId) const;
	IMPORT_C HBufC8 *AllocReadL(TInt aResourceId) const;
	IMPORT_C HBufC8 *AllocReadLC(TInt aResourceId) const;
	IMPORT_C void ConfirmSignatureL(TInt aSignature);
	IMPORT_C void ConfirmSignatureL();
	IMPORT_C TInt SignatureL() const; // not available before Release 005
	IMPORT_C TBool OwnsResourceId(TInt aResourceId) const;
	IMPORT_C TBool OwnsResourceIdL(TInt aResourceId) const;
	IMPORT_C void OpenL(RFs& aFs, const TDesC& aName, TUint aFileOffset, TInt aFileSize);
	inline TInt Offset() const;
private:
	void DoOpenL(RFs& aFs, const TDesC& aName, TUint aFileOffset, TInt aFileSize);
	IMPORT_C TInt Offset2() const;
	RResourceFileImpl* Impl();
	const RResourceFileImpl* Impl() const;
private:
	enum
		{
		KRscFileSize = 24
		};
	TUint8 iImpl[KRscFileSize];
	};

/** Returns the offset value defined for this resource file. 
   	
This function must not be called until a call to ConfirmSignatureL() has 
completed successfully, otherwise the value returned by this function may 
be meaningless.

@return The offset value defined for this resource file.
@see RResourceFile::ConfirmSignatureL() */
inline TInt RResourceFile::Offset() const 
	{ 
	return Offset2(); 
	}

#endif//__BARSC_H__
