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

#include <barsc.h>
#include "BaRscImpl.h"
#include "BaAssert.h"
#include "BaCompileAssert.h"

#define UNUSED_VAR(a) a = a

/** Constructs a default resource file object. */
EXPORT_C RResourceFile::RResourceFile()
	{
	COMPILE_TIME_ASSERT(sizeof(RResourceFile)==sizeof(RResourceFileImpl));
	//Creating the implementation instance with a placement new operator.
	//All RResourceFileImpl resources deallocation must be done in its Close() method.
	//There are some special requirements about RResourceFileImpl implementation:
	//  - it must be the same size as the RResourceFile implementation;
	//  - the offset of iOffset data member must be the same as the offset
	//    of iOffset data member of RResourceFile class before the changes - 
	//    make the class using RResourceFileImpl;
	new (iImpl) RResourceFileImpl;
	}

/** Closes the resource file reader.
This function is called after finishing reading all resources. */
EXPORT_C void RResourceFile::Close()
	{
	Impl()->Close();
	}

/** Opens the resource file reader.

The resource file reader must be opened before reading resources or
checking the signature of the resource file. This function initially
closes the resource-file object if it is currently open. If a leave
occurs during the function, the object is reverted to its closed state.

@param aFs Handle to a file server session.
@param aName File to open as a resource file
@leave The function leaves if the named file cannot be opened or the header 
record at the beginning of the file cannot be read. 
@panic If the file is corrupted - the method will panic in debug mode.
@see TBafPanic for panic codes. */
EXPORT_C void RResourceFile::OpenL(RFs &aFs,const TDesC &aName)
	{
	DoOpenL(aFs, aName, 0, 0);
	}

/** Opens the resource file reader.

The resource file reader must be opened before reading resources or
checking the signature of the resource file. This function initially
closes the resource-file object if it is currently open. If a leave
occurs during the function, the object is reverted to its closed state.

@param aFs Handle to a file server session
@param aName File to open as a resource file
@param aFileOffset Resource file section offset from the beginning of the file.
@param aFileSize Resource file section size.
@leave Function leaves if the named file cannot be opened or the header 
record at the beginning of the file cannot be read. 
@panic If the file is corrupted - the method will panic in debug mode.
@see TBafPanic for panic codes. */
EXPORT_C void RResourceFile::OpenL(RFs& aFs, const TDesC& aName, TUint aFileOffset, TInt aFileSize)
	{
	DoOpenL(aFs, aName, aFileOffset, aFileSize);
	}

/** 
Retrieve the UID tuple of the opened resource file.

@pre OpenL() has been called successfully.
@return The UIDs of the loaded resource file.
@panic If the file is not opened or class data members initialization fails - 
the method will panic always.
@see TBafPanic for panic codes. */
EXPORT_C TUidType RResourceFile::UidType() const
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
@param aResourceId The numeric id of the resource to be read.
@leave The function leaves if this resource id is not in this
resource file.
@panic If the file is corrupted - the method will panic in debug mode.
@see TBafPanic for panic codes. */
EXPORT_C void RResourceFile::ReadL(TDes8 &aDes,TInt aResourceId) const
	{
	Impl()->ReadL(aDes, aResourceId);
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
@leave KErrNotFound - there is no resource with aResourceId in the file.
@panic If the file is corrupted - the method will panic in debug mode.
@see RResourceFile::Offset()
@see TBafPanic for panic codes. */
EXPORT_C HBufC8* RResourceFile::AllocReadLC(TInt aResourceId) const
	{
	return Impl()->AllocReadLC(aResourceId);
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
@return Pointer to an 8 bit heap descriptor containing the resource.
@leave KErrNotFound - there is no resource with aResourceId in the file.
@panic If the file is corrupted - the method will panic in debug mode. 
@see RResourceFile::Offset()
@see TBafPanic for panic codes. */
EXPORT_C HBufC8* RResourceFile::AllocReadL(TInt aResourceId) const
	{
	HBufC8* resource = AllocReadLC(aResourceId);
	CleanupStack::Pop(resource);
	return resource;
	}

/** Initialises the offset value from the first resource.

The function assumes that the first resource in the file consists of
two 32-bit integers. The first integer contains the version number and
the second is a self-referencing link whose value is the offset for
the resources in the file, plus 1.This function must be called before
calling Offset(), AllocReadL(), AllocReadLC() or ReadL().

@param aSignature This argument value is not used by the function.
@leave KErrCorrupt - wrong size of the first resource in the file.
Probably the file is corrupted.
@panic If the file is corrupted - the method will panic in debug mode.
@see TBafPanic for panic codes. */
EXPORT_C void RResourceFile::ConfirmSignatureL(TInt aSignature)
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

@leave KErrCorrupt - wrong size of the first resource in the file.
Probably the file is corrupted.
@panic If the file is corrupted - the method will panic in debug mode.
@see TBafPanic for panic codes. */
EXPORT_C void RResourceFile::ConfirmSignatureL()
	{
	Impl()->ConfirmSignatureL();
	}

/** Returns this resource file's version number.

The function assumes that the first resource in the file consists of two 32-bit integers. 
The first integer contains the version number.

@return The version number.
@leave KErrCorrupt Wrong size of the first resource in the file.
Probably the file is corrupted.
@panic If the file is corrupted - the method will panic in debug mode. 
@see RResourceFile::ConfirmSignatureL()
@see TBafPanic for panic codes. */
EXPORT_C TInt RResourceFile::SignatureL() const
	{
	return Impl()->SignatureL();
	}

/** Tests whether the resource file owns the specified resource id.

The resource file owns the resource id if the most significant 20 bits of 
the resource id are zero or match the offset value as returned from a call 
to the Offset() member function.
@deprecated Interface is deprecated because it is unsafe as it may leave.
@see RResourceFile::OwnsResourceIdL
@param aResourceId The resource id to test or if the resource id is not out of range.
@return True, if the resource file owns the id, false otherwise.
*/
EXPORT_C TBool RResourceFile::OwnsResourceId(TInt aResourceId) const
	{
	TBool retCode=EFalse;
	TRAPD(errCode, retCode = OwnsResourceIdL (aResourceId));
    UNUSED_VAR(errCode);
	return retCode;
	}


/** Tests whether the resource file owns the specified resource id.

The resource file owns the resource id if the most significant 20 bits of 
the resource id are zero or match the offset value as returned from a call 
to the Offset() member function or if the resource id is not out of range.

@param aResourceId The resource id to test.
@return True, if the resource file owns the id, false otherwise.
@panic If the file is corrupted - the method will panic in debug mode.
@see TBafPanic for panic codes. */
EXPORT_C TBool RResourceFile::OwnsResourceIdL(TInt aResourceId) const
	{
	return Impl()->OwnsResourceIdL(aResourceId);
	}

/** Opens the resource file reader.

@internalComponent
@param aFs Handle to a file server session.
@param aName File to open as a resource file.
@param aFileOffset Resource file section offset from the beginning of the file.
@param aFileSize Resource file section size.
@leave The function leaves if the named file cannot be opened or the header 
record at the beginning of the file cannot be read. 
@panic If the file is corrupted - the method will panic in debug mode.
@see TBafPanic for panic codes. */
void RResourceFile::DoOpenL(RFs& aFs, const TDesC& aName, TUint aFileOffset, TInt aFileSize)
	{
	Close();
	TBaAssert assertObj(TBaAssert::EPanic);
	Impl()->OpenL(aFs, aName, assertObj, aFileOffset, aFileSize);
	}

/** Returns the offset value defined for this resource file.

This function must not be called until a call to ConfirmSignatureL() has completed successfully, 
otherwise the value returned by this function may be meaningless.

@internalComponent
@return The offset value defined for this resource file. */
EXPORT_C TInt RResourceFile::Offset2() const
	{
	return Impl()->Offset();
	}

/** The method returns a pointer to the object implementing resource file reader
functionality.

@internalComponent
@return Pointer to the implementation instance. */
RResourceFileImpl* RResourceFile::Impl()
	{
	return reinterpret_cast <RResourceFileImpl*> (iImpl);
	}

/** The method returns a const pointer to the object implementing resource file reader
functionality.

@internalComponent
@return Const pointer to the implementation instance. */
const RResourceFileImpl* RResourceFile::Impl() const
	{
	return reinterpret_cast <const RResourceFileImpl*> (iImpl);
	}
