/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Test types used for notifier buffer & result.
*
*/


/**
 @file
 @internalTechnology 
 @released 
*/

#ifndef TUPSNOTIFIERTYPES_H
#define TUPSNOTIFIERTYPES_H

#include <e32base.h>
#include <e32cmn.h>
#include <ups/policy.h>
#include <ups/upsconst.h>

namespace UserPromptService
	{
	/**
	Enum for bitmask that influences visual style or behavior of dialog.
	*/
	typedef enum 
		{
		ETrustedClient	= 0x0001,		///< Client SID is protected so client name is trusted
		EBuiltInApp		= 0x0002		///< Built-in application ... exe loaded for Z drive.
		} TUpsDialogFlags;
		
		
	/**
	Contains the data needed to display a prompt.
	*/
	class TPromptData
		{
	public:
		TPromptData();
		TBuf<KMaxFileName> iClientName;								///< From AppArc, SIS registry or process filename
		TBuf<256> iVendorName;										///< Localized vendor name
		TSecureId iServerSid;										///< Needed to lookup localized text for service
		TUid iServiceId;											///< Service name localized by notifier implemenation
		TInt iNumDestinations;										///< Number of destinations field.
		TBuf<KMaxFileName> iDestination1;							///< First destination, normally the one defined in the request
		TBuf<KMaxFileName> iDestination2;							///< Second destination, normally 'all destinations'
		TUint iOptions;												///< CPolicy::TOptions bit mask
		TUint iFlags;												///< Dialog style e.g. whether client identity is trusted
		};
	
	inline TPromptData::TPromptData() :
		iClientName(KNullDesC), iVendorName(KNullDesC), iServerSid(0), iServiceId(TUid::Null()),
		iNumDestinations(0), iDestination1(KNullDesC), iDestination2(KNullDesC),
		iOptions(CPolicy::EYes | CPolicy::ENo), iFlags(0)
			{			
			}
		
	class TPromptResult
		{
	public: 
		TPromptResult();
		CPolicy::TOptions iSelected;	///< The button that was pressed
		TInt iDestination;				///< The destination selected if applicable.
		};
	
	inline TPromptResult::TPromptResult() : 
		iSelected(CPolicy::ENo), iDestination(0) 
		{
		}		
	}
#endif // TUPSNOTIFIERTYPES_H
