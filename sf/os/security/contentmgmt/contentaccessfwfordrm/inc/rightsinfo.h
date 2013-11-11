/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
 @publishedPartner
 @released
*/


#ifndef __RIGHTSINFO_H__
#define __RIGHTSINFO_H__

#include <e32base.h>
#include <caf/caftypes.h>

class RWriteStream;
class RReadStream;

namespace ContentAccess
{

	/** Rights class to store a summary description of the rights and a unique 
	reference to the rights object within an Agent

	*/
	class CRightsInfo : public CBase
		{
	public:

		/** Construct a new CRightsInfo
		
		@param aDescription A generic text description of the rights supplied by the agent "Content XYZ Expiry date mm/dd/yy" etc
		@param aUniqueId A uniqueId used to refer to this particular rights object within the agent it came from
		@param aRightsTypeMask A bitmask of TRightsType entries applicable to this rights object
		@param aRightsStatus The current status of this rights object
		@return A CRightsInfo object
		*/
		IMPORT_C static CRightsInfo* NewL(const TDesC& aDescription, const TDesC& aUniqueId, TInt aRightsTypeMask, TRightsStatus aRightsStatus);
		
		/** Construct a new CRightsInfo from a stream */
		IMPORT_C static CRightsInfo* NewL(RReadStream& aStream);

		virtual ~CRightsInfo();

		/** returns a string describing the rights object
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C const TDesC& Description() const;

		/** returns a string with a unique Id used to identify a particular rights object
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C const TDesC& UniqueId() const;

		/** Returns a bit mask of TRightsTypeMask flags. A rights object can be stateless and/or consumable
		
		@see ContentAccess::TRightsTypeMask

		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 		
		*/
		IMPORT_C TInt RightsType() const;

		/** The state of the rights

		@see ContentAccess::TRightsStatus

		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 		
		*/
		IMPORT_C TRightsStatus RightsStatus() const;

		/** Write this CRightsInfo object to a stream

		@param aStream The stream
		*/
		IMPORT_C void ExternalizeL(RWriteStream &aStream) const;

	private:
		CRightsInfo();
		
		CRightsInfo(TInt aRightsType, TRightsStatus aRightsStatus);
		void ConstructL(const TDesC& aDescription, const TDesC& aUniqueId);
		
		void InternalizeL(RReadStream& aStream);
		
	private:
		
		HBufC* iDescription;
		HBufC* iUniqueId;

		TInt iRightsType;
		TRightsStatus iRightsStatus;
		};

	/** Interface used by agents as a base class for their own rights objects

	Agent derived classes will contain a complete object capable of describing
	rights for content managed by that agent.

	All derived classes must implement the serialization functions InternalizeL() 
	and ExternalizeL()

	*/
	class MAgentRightsBase 
		{
	public:
		virtual void ExternalizeL(RWriteStream& aStream) const = 0;
		virtual void InternalizeL(RReadStream& aStream) = 0;
		};	
	}

#endif
