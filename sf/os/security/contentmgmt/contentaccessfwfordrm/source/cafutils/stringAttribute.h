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
 @internalComponent
 @released
*/


#ifndef __STRINGATTRIBUTE_H__
#define __STRINGATTRIBUTE_H__

#include <e32base.h>

namespace ContentAccess
	{

	/** Used by RAttributeSet 

	@internalComponent
	@released
	*/
	class CStringAttribute : public CBase
		{
	public:
		static CStringAttribute* NewL(TInt aAttribute, const TDesC& aValue, TInt aError);
		virtual ~CStringAttribute();

		TInt Attribute() const;
		const TDesC& Value() const;
		TInt Error() const;

	private:
		CStringAttribute(TInt aAttribute, TInt aError);
		void ConstructL(const TDesC& aValue);

	private:
		TInt iAttribute;
		HBufC* iValue;
		TInt iError;
		};
	}

#endif
