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

#include <langutil.h>
#include "T_LangUtilsTestShareFunc.h"

void DoCopyFileL(RFs& aFs, const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch)
    {
    CFileMan* fileMan=CFileMan::NewL(aFs);
    CleanupStack::PushL(fileMan);
    User::LeaveIfError(fileMan->Copy(aSourceFullName,aTargetFullName,aSwitch));
    CleanupStack::PopAndDestroy(); // fileMan
    }


TInt LangUtilsTest::CopyFile(RFs& aFs, const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch)
/** Copies one or more files.

For more details, 
@see CFileMan::Copy()
@since     5.1
@param     aFs File server session
@param     aSourceFullName Path indicating the file(s) to be copied. Any path
components that are not specified here will be taken from the session path.
@param     aTargetFullName Path indicating the directory into which the file(s)
are to be copied
@param     aSwitch=CFileMan::EOverWrite Set this to zero for no overwriting and 
no recursion; CFileMan::EOverWrite to overwrite files with the same name; or 
CFileMan::ERecurse for recursion.
@return   KErrNone if successful, otherwise another of the system-wide error 
codes.*/
    {
    TRAPD(err,DoCopyFileL(aFs,aSourceFullName,aTargetFullName,aSwitch));
    return err;
    }

TBool LangUtilsTest::FileExists(const RFs& aFileSession,const TDesC& aFileName)
/** Checks if the specified file exists.
    
@param aFs File server session
@param aFileName File to check
@return ETrue if the file exists, otherwise EFalse */
    { // static
    TEntry entry;
    return(aFileSession.Entry(aFileName,entry)==KErrNone);
    }

void DoDeleteFileL(RFs& aFs, const TDesC& aSourceFullName, TUint aSwitch)
    {
    CFileMan* fileMan=CFileMan::NewL(aFs);
    CleanupStack::PushL(fileMan);
    User::LeaveIfError(fileMan->Delete(aSourceFullName,aSwitch));
    CleanupStack::PopAndDestroy(); // fileMan
    }


EXPORT_C TInt LangUtilsTest::DeleteFile(RFs& aFs, const TDesC& aSourceFullName, TUint aSwitch)
/** Deletes one or more files.

For more details,
@see CFileMan::Delete().
@since 5.1
@param aFs File server session
@param aSourceFullName Path indicating the file(s) to be deleted. May either
be a full path, or relative to the session path. Use wildcards to specify 
more than one file.
@param aSwitch=0  Specify CFileMan::ERecurse for recursion,
zero for no recursion.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */   
    {
    TRAPD(err,DoDeleteFileL(aFs,aSourceFullName,aSwitch));
    return err;
    }

NONSHARABLE_CLASS(TKeyDesC16Array) : public TKeyArrayFix
    {
public:
    TKeyDesC16Array(TKeyCmpText aType);
private: // from TKey
    TInt Compare(TInt aLeft,TInt aRight) const;
    };

TKeyDesC16Array::TKeyDesC16Array(TKeyCmpText aType)
    : TKeyArrayFix(0,aType,0)
    {
    }

TInt TKeyDesC16Array::Compare(TInt aLeft,TInt aRight) const
    {
    TDesC16* left=(*(TDesC16**)At(aLeft));
    TDesC16* right=(*(TDesC16**)At(aRight));
    switch (iCmpType)
        {
    case ECmpFolded:
        return(left->CompareF(*right));
    case ECmpCollated:
        return(left->CompareC(*right));
    default:
        break;
        }
    return(left->Compare(*right));
    }

EXPORT_C CDesC16Array::CDesC16Array(TBufRep aRep,TInt aGranularity)
    : CArrayFixBase(aRep,sizeof(HBufC16*),aGranularity)
    {
    }

EXPORT_C CDesC16Array::~CDesC16Array()
/** Frees all resources owned by the object, prior to its destruction. In particular, 
it deletes all descriptors from the array and frees the memory allocated to 
the array buffer. */
    {
    Reset();
    }

EXPORT_C void CDesC16Array::Reset()
/** Deletes all descriptors from the array and frees the memory allocated to the 
array buffer.

@see CDesC16Array::Delete() */
    {
    Delete(0,Count());
    }

EXPORT_C void CDesC16Array::Delete(TInt aIndex)
    {
    Delete(aIndex,1);
    }

/**
 *
 * Deletes one or more logically contiguous descriptor elements from the
 * array.
 *
 * The deletion starts at the specified position.Deleting descriptor
 * elements from the array frees the memory occupied by the associated
 * heap descriptors and removes their pointers from the array buffer but
 * does not cause the array buffer to be automatically compressed. Call
 * the Compress() function to return excess space in the array buffer to
 * the heap.
 *
 * @param     " TInt aPos "
 *            The starting position in the array from which
 *            descriptor elements are to be deleted. The position is
 *            relative to zero, i.e. zero implies the first
 *            descriptor element. This value must not be negative
 *            and must not be greater than the number of descriptor
 *            elements currently in the array, otherwise the
 *            functions raise an E32USER-CBase 21 panic.
 * @param     " TInt aCount "
 *            If specified, the number of contiguous descriptor
 *            elements to be deleted from the array. This value must
 *            not be negative otherwise the function raises an
 *            E32USER-CBase 22 panic. This value must not be
 *            negative otherwise the function raises an
 *            E32USER-CBase 22 panic. This value must not be
 *            negative otherwise the function raises an
 *            E32USER-CBase 22 panic. This value plus the value of
 *            the starting position must not be greater than the
 *            number of descriptor elements in the array, otherwise
 *            the function raises an E32USER-CBase 29 panic. If not
 *            specified, one element is assumed.
 */
EXPORT_C void CDesC16Array::Delete(TInt aIndex,TInt aCount)
    {
    TInt count=aCount;
    for (TInt ii=aIndex; count--; ii++)
        User::Free(*(HBufC16**)At(ii));
    CArrayFixBase::Delete(aIndex,aCount);
    }

EXPORT_C TInt CDesC16Array::MdcaCount() const
/** Returns the number of descriptor elements in the array. The function implements 
the function interface MDesC16Array::MdcaCount().

@return The number of descriptor elements. */
    {
    return Count();
    }

EXPORT_C TPtrC16 CDesC16Array::MdcaPoint(TInt anIndex) const
/** Indexes into a descriptor array. The function implements the interface MDesC16Array::MdcaPoint().
    
    @param aIndex The position of the descriptor element within the array. The 
    position is relative to zero; i.e. zero implies the first descriptor element 
    in the array. This value must be non-negative and less than the number of 
    descriptors currently within the array otherwise the operator panics with 
    EArrayIndexOutOfRange. 
    @return A non-modifiable pointer descriptor representing the descriptor element 
    located at position aIndex within the array. */ 
    {
    HBufC16* temp=(*(HBufC16**)At(anIndex));
    return (TPtrC16)(*temp);
    }

EXPORT_C void CDesC16Array::AppendL(const TDesC16& aPtr)
/** Appends a descriptor onto the end of any existing descriptor elements in the 
array.

@param aPtr A reference to the descriptor to be appended to the array. */
    {
    InsertL(Count(),aPtr);
    }

EXPORT_C void CDesC16Array::InsertL(TInt aPos,const TDesC16& aPtr)
/** Inserts a descriptor into the array at the specified position. 

If the specified position is the same as the current number of descriptor 
elements in the array, this has the effect of appending the element.

@param aPos The position within the array where the descriptor element is 
to be inserted. The position is relative to zero, i.e. zero implies the first 
descriptor element. This value must not be negative and must not be greater 
than the number of descriptor elements currently in the array, otherwise the 
function raises an E32USER-CBase 21 panic. 
@param aPtr The descriptor to be inserted into the array. */
    {
    HBufC16* bufPtr=aPtr.AllocLC();
    CArrayFixBase::InsertL(aPos,&bufPtr);
    CleanupStack::Pop();
    }

EXPORT_C TInt CDesC16Array::InsertIsqL(const TDesC16& aPtr,TKeyCmpText aTextComparisonType)
/** Inserts a descriptor into the array at a position which maintains the sequence 
of the descriptors.

The sequence is determined by comparing descriptors using one of the TDesC 
comparison functions. The enumeration aTextComparisonType governs how the 
descriptors are to be compared.

The array should already be in sequence, otherwise the position of the new 
descriptor element is unpredictable. Descriptor elements which are the same 
are not permitted.

@param aPtr A reference to the descriptor to be inserted into the array. 
@param aTextComparisonType An enumeration which determines the type of comparison 
to be made between descriptors for the purpose of choosing the insertion position. 
If no parameter is explicitly passed, ECmpFolded is used by default.
@return The position within the array of the inserted descriptor. */
    {
    HBufC16* bufPtr=aPtr.AllocLC();
    TKeyDesC16Array key(aTextComparisonType);
    TInt found=CArrayFixBase::InsertIsqL(&bufPtr,key);
    CleanupStack::Pop();
    return(found);
    }

EXPORT_C TInt CDesC16Array::InsertIsqAllowDuplicatesL(const TDesC16& aPtr,TKeyCmpText aTextComparisonType)
/** Inserts a descriptor into the array at a position which maintains the sequence 
of the descriptors; allows duplicates.

The sequence is determined by comparing descriptors using one of the TDesC 
comparison functions. The enumeration aTextComparisonType governs how the 
descriptors are to be compared.

The array should already be in sequence, otherwise the position of the new 
descriptor element is unpredictable. Descriptor elements which are the same 
are permitted; if the descriptor aPtr is a duplicate of an existing descriptor 
within the array, then the new descriptor element is positioned after the 
existing descriptor element.

@param aPtr A reference to the descriptor to be inserted into the array. 
@param aTextComparisonType An enumeration which determines the type of comparison 
to be made between descriptors for the purpose of choosing the insertion position. 
If no parameter is explicitly passed, ECmpFolded is used by default.
@return The position within the array of the inserted descriptor. */
    {
    HBufC16* bufPtr=aPtr.AllocLC();
    TKeyDesC16Array key(aTextComparisonType);
    TInt found=CArrayFixBase::InsertIsqAllowDuplicatesL(&bufPtr,key);
    CleanupStack::Pop();
    return(found);
    }

EXPORT_C void CDesC16Array::Sort(TKeyCmpText aTextComparisonType)
/** Sorts the descriptor elements into sequence.

The sequence is determined by comparing descriptors using one of the member 
functions of the descriptor base class TDesC.

@param aTextComparisonType An enumeration which defines the type of comparison 
to be made between descriptors. By default the comparison type is ECmpFolded. */
    {
    TKeyDesC16Array key(aTextComparisonType);
    CArrayFixBase::Sort(key);
    }

EXPORT_C TInt CDesC16Array::Find(const TDesC16& aPtr,TInt& aPos,TKeyCmpText aTextComparisonType) const
/** Finds the position of a descriptor element within the array which matches the 
specified descriptor, using a sequential search.

The array is searched sequentially for a matching descriptor element, starting 
with the first descriptor element in the array. Descriptors are compared using 
one of the TDesC comparison functions. The enumeration aTextComparisonType 
governs how the descriptors are to be compared.

Where an array has duplicate descriptor elements, the function only supplies 
the position of the first descriptor element.

@param aPtr A reference to a descriptor to be used for comparison. 
@param aPos If the descriptor element is found, this reference is set to the 
position of that descriptor element within the array. The position is relative 
to zero, (i.e. the first descriptor element in the array is at position 0). 
If the descriptor element is not found and the array is not empty, then the 
value of the reference is set to the number of descriptor elements in the 
array. If the descriptor element is not found and the array is empty, then 
the reference is set to zero. 
@param aTextComparisonType An enumeration which determines the type of comparison 
to be made between descriptors. If no parameter is explicitly passed,ECmpFolded 
is used by default. 
@return Zero, if a matching descriptor element is found. Non-zero, if no matching 
descriptor element can be found. */
    {
    TKeyDesC16Array key(aTextComparisonType);
    const TDesC16* tmp=(&aPtr);
    return(CArrayFixBase::Find(&tmp,key,aPos));
    }

EXPORT_C TInt CDesC16Array::FindIsq(const TDesC16& aPtr,TInt& aPos,TKeyCmpText aTextComparisonType) const
/** Finds the position of a descriptor element within the array which matches the 
specified descriptor, using a binary search technique. The array must 
be in sequence, otherwise the outcome is unpredictable.

Descriptors are compared using one of the TDesC comparison functions. The 
enumeration aTextComparisonType governs how the descriptors are to be compared.

Where an array has duplicate descriptor elements, the function cannot guarantee 
which matching descriptor element it will return; except that it will find 
one of them.

@param aPtr A reference to a descriptor to be used for comparison. 
@param aPos If the descriptor element is found, the reference is set to the 
position of that descriptor element within the array. The position is relative 
to zero, (i.e. the first descriptor element in the array is at position 0). 
If the descriptor element is not found and the array is not empty, then the 
reference is set to the position of the first descriptor element in the array 
whose content is greater than the content of aPtr. If the descriptor element 
is not found and the array is empty, then the reference is set to zero.
@param aTextComparisonType An enumeration which determines the type of comparison 
to be made between descriptors. If no parameter is explicitly passed, ECmpFolded 
is used by default. 
@return Zero, if a matching descriptor element is found. Non-zero, if no matching 
descriptor element can be found. */
    {
    TKeyDesC16Array key(aTextComparisonType);
    const TDesC16* tmp=(&aPtr);
    return(CArrayFixBase::FindIsq(&tmp,key,aPos));
    }


EXPORT_C CDesC16ArraySeg::CDesC16ArraySeg(TInt aGranularity)
    : CDesC16Array((TBufRep)CBufSeg::NewL,aGranularity)
/** Construct a segmented descriptor array with the specified granularity.

No memory is allocated to the array buffer by this C++ constructor.

@param aGranularity The granularity of the array. This value must be positive 
otherwise the constructor raises an E32USER-CBase 18 panic. */
    {
    __DECLARE_NAME(_S("CDesC16ArraySeg"));
    }

EXPORT_C CDesC16ArraySeg::~CDesC16ArraySeg()
/** Frees all resources owned by the object, prior to its destruction. */
    {}


/* The following 3 function is defined so that the functions IdealLanguage,
SetIdealLanguage, ReleaseIdealLanguage will compile as part as an .exe
These functions use a global TAny* to store the data that would have been
stored in the TLS under normal compilation (ie If the BaUtils was compilled
as part of a DLL).
*/

TAny* ptr(NULL);

TAny* Dll::Tls()
    {
    return ptr;
    }

TInt Dll::SetTls(TAny* aPtr)
    {
    ptr = aPtr;
    return KErrNone;
    }

void Dll::FreeTls()
    {
    ptr = NULL;
    }



