/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#ifndef __WMDRMCONTENTPARSER_H__
#define __WMDRMCONTENTPARSER_H__

#include <e32base.h>
#include <f32file.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>

namespace ReferenceTestAgent
	{
	NONSHARABLE_CLASS(CWmdrmContentParser) : public CBase
		{
	public:
		IMPORT_C static CWmdrmContentParser* NewL(const TDesC8& aHeaderData);
		virtual ~CWmdrmContentParser();
		
		IMPORT_C TInt GetAttribute(TInt aAttribute, TInt& aValue) const;
		IMPORT_C TInt GetStringAttribute(TInt aAttribute, TDes& aValue) const;
		IMPORT_C TInt GetAttributeSet(ContentAccess::RAttributeSet &aAttributeSet) const; 
		IMPORT_C void GetStringAttributeSetL(ContentAccess::RStringAttributeSet &aStringAttributeSet) const;
		
	private:
		CWmdrmContentParser();
		void ConstructL(const TDesC8& aHeaderData);
		void CreateAttributeSetL();
		void CreateStringAttributeSetL();
		void SetAttributeL(TInt aAttribute, TInt aValue, TInt aErrorCode);
		void SetStringAttributeL(TInt aAttribute, const TDesC& aValue, TInt aErrorCode);
						
	private:
		HBufC8* iHeaderData;
	
		ContentAccess::RAttributeSet iAttributeSet;
		ContentAccess::RStringAttributeSet iStringAttributeSet;

		};	
	}
#endif
