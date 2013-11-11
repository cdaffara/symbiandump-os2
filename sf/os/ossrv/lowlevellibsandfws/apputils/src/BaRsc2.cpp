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

#include <barsc2.h>
#include "BaRscImpl.h"
#include "BaAssert.h"

/** Creates CResourceFile instance.
@param aFs Handle to a file server session.
@param aName File to open as a resource file.
@param aFileOffset The resource file section offset from the beginning of the file.
@param aFileSize The resource file section size.
@leave KErrCorrupt if the file is corrupted.
@leave KErrNoMemory if there is not enough memory for the object. */
EXPORT_C CResourceFile* CResourceFile::NewL(RFs& aFs, const TDesC& aName, 
											TUint aFileOffset, TInt aFileSize)
	{
	CResourceFile* self = CResourceFile::NewLC(aFs, aName, aFileOffset, aFileSize);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates CResourceFile instance.
@param aRscFileBuffer a buffer containing one entire resource file
@return a CResourceFile instance corresponding to the rsc file passed
through the file buffer
@leave KErrCorrupt if the file buffer is corrupted.
@leave KErrNoMemory if there is not enough memory for the object. */
EXPORT_C CResourceFile* CResourceFile::NewL(const TDesC8& aRscFileBuffer)
	{
	CResourceFile* self = new (ELeave) CResourceFile;
	CleanupStack::PushL(self);
	self->ConstructL(aRscFileBuffer);
	CleanupStack::Pop(self);
	return self;
	}

/** Creates CResourceFile instance.
@param aFs Handle to a file server session.
@param aName File to open as a resource file.
@param aFileOffset The resource file section offset from the beginning of the file.
@param aFileSize The resource file section size.
@leave KErrCorrupt if the file is corrupted.
@leave KErrNoMemory if there is not enough memory for the object. */
EXPORT_C CResourceFile* CResourceFile::NewLC(RFs& aFs, const TDesC& aName, 
											 TUint aFileOffset, TInt aFileSize)
	{
	CResourceFile* self = new (ELeave) CResourceFile;
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aName, aFileOffset, aFileSize);
	return self;
	}

/** Frees the resources, allocated by the instance.*/
EXPORT_C CResourceFile::~CResourceFile()
	{
	RResourceFileImpl* impl = Impl();
	impl->Close();
	//RResourceFileImpl doesn't have a user defined destructor.
	//All resources deallocation must be done in the RResourceFileImpl::Close() method.
	}

/** Returns this resource file's UIDs.

@return The UIDs of the loaded resource file. */
EXPORT_C TUidType CResourceFile::UidType() const
	{
	return Impl()->UidType();
	}

/** Reads a resource specified by resource id into the specified descriptor.

The descriptor must be long enough to contain the entire resource

The search for the resource uses the following algorithm:

A resource id in the range 1 to 4095 is looked up in this resource file. The 
function leaves if there is no matching resource.

If the resource id is greater than 4095, then the most significant 20 bits 
of the resource id is treated as an offset and the least significant 12 bits 
is treated as the real resource id. If the offset matches the offset value 
defined for this file, then the resource is looked up in this resource file 
using the real resource id (i.e. the least significant 12 bits). If the offset 
does not match, then the function leaves.

Note, do not call this function until a call to ConfirmSignatureL() has completed 
successfully.

@param aDes On return, contains the resource that has been read.
The function leaves if the descriptor is not long enough to contain the entire resource.
@param aResourceId The numeric id of the resource to be read. The function 
leaves if this resource id is not in this resource file.
@leave The function leaves if this resource id is not in this
resource file or the file is corrupted. */
EXPORT_C void CResourceFile::ReadL(TDes8 &aDes,TInt aResourceId) const
	{
	Impl()->ReadL(aDes, aResourceId);
	}

/** Reads a resource into a heap descriptor and returns a pointer to that descriptor.

A heap descriptor of appropriate length is allocated for the resource. Ownership 
of the heap descriptor passes to the caller who must destroy it when it is 
no longer needed.

The search for the resource uses the following algorithm:

A resource id in the range 1 to 4095 is looked up in this resource file. The 
function leaves if there is no matching resource.

If the resource id is greater than 4095, then the most significant 20 bits 
of the resource id is treated as an offset and the least significant 12 bits 
is treated as the real resource id. If the offset matches the offset value 
defined for this file, then the resource is looked up in this resource file 
using the real resource id (i.e. the least significant 12 bits). If the offset 
does not match, then the function leaves.

Note, do not call this function until a call to ConfirmSignatureL() has completed 
successfully.

@param aResourceId The numeric id of the resource to be read.
@return Pointer to a heap descriptor containing the resource.
@leave  The function leaves if this resource id is not in this
resource file or the file is corrupted.
@see RResourceFile::Offset() */
EXPORT_C HBufC8* CResourceFile::AllocReadL(TInt aResourceId) const
	{
	HBufC8* resource = AllocReadLC(aResourceId);
	CleanupStack::Pop(resource);
	return resource;
	}

/** Reads a resource into a heap descriptor, returns a pointer to that descriptor 
and pushes the pointer onto the cleanup stack.

A heap descriptor of appropriate length is allocated for the resource. Ownership 
of the heap descriptor passes to the caller who must destroy it and pop its 
pointer off the cleanup stack when it is no longer needed.

The search for the resource uses the following algorithm:

A resource id in the range 1 to 4095 is looked up in this resource file. The 
function leaves if there is no matching resource.

If the resource id is greater than 4095, then the most significant 20 bits 
of the resource id is treated as an offset and the least significant 12 bits 
is treated as the real resource id. If the offset matches the offset value 
defined for this file, then the resource is looked up in this resource file 
using the real resource id (i.e. the least significant 12 bits). If the offset 
does not match, then the function leaves.

Note, do not call this function until a call to ConfirmSignatureL() has completed 
successfully.

@param aResourceId The numeric id of the resource to be read.
@return Pointer to a heap descriptor containing the resource.
@leave The function leaves if this resource id is not in this
resource file or the file is corrupted.
@see RResourceFile::Offset() */
EXPORT_C HBufC8* CResourceFile::AllocReadLC(TInt aResourceId) const
	{
	return Impl()->AllocReadLC(aResourceId);
	}

/** Initialises the offset value from the first resource.

The function assumes that the first resource in the file consists of
two 32-bit integers. The first integer contains the version number and
the second is a self-referencing link whose value is the offset for
the resources in the file, plus 1.This function must be called before
calling Offset(), AllocReadL(), AllocReadLC() or ReadL().

@param aSignature This argument value is not used by the function.
@leave The function leaves if this resource id is not in this
resource file or the file is corrupted. */
EXPORT_C void CResourceFile::ConfirmSignatureL(TInt aSignature)
	{
	Impl()->ConfirmSignatureL(aSignature);
	}

/** Initialises the offset value from the first resource.

The function tests to catch cases where the first resource is not an RSS_SIGNATURE.
It assumes that the first resource in the file consists of
two 32-bit integers. The first integer contains the version number and
the second is a self-referencing link whose value is the offset for
the resources in the file, plus 1.This function must be called before
calling Offset(), AllocReadL(), AllocReadLC() or ReadL().
@leave The function leaves if this resource id is not in this
resource file or the file is corrupted. */
EXPORT_C void CResourceFile::ConfirmSignatureL()
	{
	Impl()->ConfirmSignatureL();
	}

/** Returns this resource file's version number.

The function assumes that the first resource in the file consists of two 32-bit integers. 
The first integer contains the version number.
@return The version number.
@leave KErrCorrupt - the file is corrupted.
@see RResourceFile::ConfirmSignatureL() */
EXPORT_C TInt CResourceFile::SignatureL() const
	{
	return Impl()->SignatureL();
	}

/** Tests whether the resource file owns the specified resource id.
The resource file owns the resource id if the most significant 20 bits of 
the resource id are zero or match the offset value as returned from a call 
to the Offset() member function or if the resource id is not out of range.
@param aResourceId The resource id to test.
@return True, if the resource file owns the id, false otherwise.
@leave KErrCorrupt - the file is corrupted. Some other error codes are possible. */
EXPORT_C TBool CResourceFile::OwnsResourceIdL(TInt aResourceId) const
	{
	return Impl()->OwnsResourceIdL(aResourceId);
	}

/** Returns the offset value defined for this resource file.

This function must not be called until a call to ConfirmSignatureL()
has completed successfully, otherwise the value returned by this 
function may be meaningless.
@return The offset value defined for this resource file. */
EXPORT_C TInt CResourceFile::Offset() const
	{
	return Impl()->Offset(); 
	}

/**
@internalComponent
Default constructor.
Initializes in place the implementation object.
*/
CResourceFile::CResourceFile()
	{
	//Creating the implementation instance with a placement new operator.
	new (iImpl) RResourceFileImpl;
	}

/** @internalComponent
@param aFs Handle to a file server session.
@param aName File to open as a resource file.
@param aFileOffset The resource file section offset from the beginning of the file.
@param aFileSize The resource file section size.
@leave KErrCorrupt if the file is corrupted.
@leave KErrNoMemory if there is not enough memory for the object. */
void CResourceFile::ConstructL(RFs& aFs, const TDesC& aName, 
								TUint aFileOffset, TInt aFileSize)
	{
	TBaAssert assertObj(TBaAssert::ELeave);
	Impl()->OpenL(aFs, aName, assertObj, aFileOffset, aFileSize);
	}

/** @internalComponent
@param aRscFileBuffer a buffer containing a full rsc file
@leave KErrCorrupt if the file buffer is corrupted.
@leave KErrNoMemory if there is not enough memory for the object. */
void CResourceFile::ConstructL(const TDesC8& aRscFileBuffer)
	{
	TBaAssert assertObj(TBaAssert::ELeave);
	Impl()->OpenL(aRscFileBuffer,assertObj);
	}

/** @internalComponent
@return Non-const pointer to the implementation. */
RResourceFileImpl* CResourceFile::Impl()
	{
	return reinterpret_cast <RResourceFileImpl*> (iImpl);
	}

/** @internalComponent
@return Const pointer to the implementation. */
const RResourceFileImpl* CResourceFile::Impl() const
	{
	return reinterpret_cast <const RResourceFileImpl*> (iImpl);
	}

