/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CCTTokenTypeFinder.cpp
*
*/


#include <e32base.h>
#include <ecom/ecom.h>
#include "ct/ccttokentypeinfo.h"
#include "ct/interfaceuid.hrh"

const TUid KInterfaceUID = {CT_INTERFACE_UID};

// Cleanup policy. If List leaves, everything it has created must be
// deleted. This includes the ImplInfoList it might have hanging
// around and any items it has added to the CCTTokenTypeInfo list, but
// not any items that were already on that list.
class TCleanupHelper
	{
public:
	// Constructor
	TCleanupHelper(TInt iFirstTokenTypeToRemove,
				   RCPointerArray<CCTTokenTypeInfo>& aTokenTypes,
				   RImplInfoPtrArray& aInfoArray);
	// Do the cleanup
	void Close();

private:
	TInt iFirstTokenTypeToRemove;
	RCPointerArray<CCTTokenTypeInfo>& iTokenTypes;
	RImplInfoPtrArray& iInfoArray;
	};

TCleanupHelper::TCleanupHelper(TInt aFirstTokenTypeToRemove,
							   RCPointerArray<CCTTokenTypeInfo>& aTokenTypes,
							   RImplInfoPtrArray& aInfoArray)
		: iFirstTokenTypeToRemove(aFirstTokenTypeToRemove),
		  iTokenTypes(aTokenTypes), iInfoArray(aInfoArray)
	{
	}

void TCleanupHelper::Close()
	{
	// Delete all token types that have been added by this operation
	// (backwards to avoid the soon-to-be deleted ones being copied about)
	for (TInt ii = iTokenTypes.Count() - 1; ii >= iFirstTokenTypeToRemove;ii--)
		{
		delete iTokenTypes[ii];
		iTokenTypes.Remove(ii);
		}

	// And delete any unprocessesd info objects
	iInfoArray.ResetAndDestroy();
	}

EXPORT_C 
void CCTTokenTypeInfo::ListL(RCPointerArray<CCTTokenTypeInfo>& aTokenTypes, 
							const MCTTokenTypeFilter& aFilter)
	{
	RImplInfoPtrArray infoArray;
	TCleanupHelper ch(aTokenTypes.Count(), aTokenTypes, infoArray);
	CleanupClosePushL(ch);

	REComSession::ListImplementationsL(KInterfaceUID, infoArray);

	// For each ECom interface we've found, create a token type info
	// and see if we want it.
	TInt count = infoArray.Count();
	for (TInt ii = 0; ii < count; ii++)
		{
		// Don't accept RAM based plugins
		if (!infoArray[ii]->RomBased())
			{
			continue;
			}
		
		CCTTokenTypeInfo* info = new (ELeave) CCTTokenTypeInfo(infoArray[ii]);
		// The item from the infoArray is now owned by info, so it
		// should be removed from the array.
		infoArray[ii] = NULL;

		// See if we actually want this info (using the filter)
		if (aFilter.Accept(*info))
			{
			// Yes. Put it onto the results array.
			CleanupStack::PushL(info);
			User::LeaveIfError(aTokenTypes.Append(info));
			CleanupStack::Pop(info);
			}
		else
			{
			// No. Delete it.
			delete info;
			}

		}

	infoArray.Close();
	CleanupStack::Pop(); // ch
	}

EXPORT_C void CCTTokenTypeInfo::ListL(RCPointerArray<CCTTokenTypeInfo>& aTokenTypes)
	{
	ListL(aTokenTypes, TCTAllTokenTypes());
	}

CCTTokenTypeInfo::CCTTokenTypeInfo(CImplementationInformation*
								   aEcomImplementationInfo)
		: iEcomImplementationInfo(aEcomImplementationInfo)
	{
	// Extract the list of supported interfaces and the list of token type
	// attributes from the data ecom provides about the plugin - this comes from
	// the default_data and opaque_data sections of the rss file.

	TInt interfaceCount = iEcomImplementationInfo->DataType().Size() / sizeof(TUid);
	if (interfaceCount > 0)
		{
		const TUint8* data = iEcomImplementationInfo->DataType().Ptr();
		iInterfaces = RArray<TUid>(
			sizeof(TUid),
			const_cast<TUid*>(reinterpret_cast<const TUid*>(data)),
			interfaceCount);
		}

	TInt attributeCount = iEcomImplementationInfo->OpaqueData().Size() / sizeof(TCTTokenTypeAttribute);
	if (attributeCount > 0)
		{
		const TUint8* data = iEcomImplementationInfo->OpaqueData().Ptr();
		iAttributes = RArray<TCTTokenTypeAttribute>(
			sizeof(TCTTokenTypeAttribute),
			const_cast<TCTTokenTypeAttribute*>(reinterpret_cast<const TCTTokenTypeAttribute*>(data)),
			attributeCount);
		}
	}

// Destructor
CCTTokenTypeInfo::~CCTTokenTypeInfo()
	{
	// don't close iInterfaces or iAttributes as their contents are actually
	// owned by iEcomImplementationInfo
	delete iEcomImplementationInfo;
	}

// Returns the attributes in a RArray.
EXPORT_C const RArray<TCTTokenTypeAttribute>& CCTTokenTypeInfo::Attributes() const
	{
	return iAttributes;
	}

// Return the supported interfaces in a RArray
EXPORT_C const RArray<TUid>& CCTTokenTypeInfo::Interfaces() const
	{
	return iInterfaces;
	}

// Return a label for the token type
EXPORT_C const TDesC& CCTTokenTypeInfo::Label() const
	{
	return iEcomImplementationInfo->DisplayName();
	}

// Return the UID of the token type
EXPORT_C TUid CCTTokenTypeInfo::Type() const
	{
	return iEcomImplementationInfo->ImplementationUid();
	}



