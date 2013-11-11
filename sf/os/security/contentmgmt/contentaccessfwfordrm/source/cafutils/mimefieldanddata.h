/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MIMEFIELDANDDATA_H__
#define __MIMEFIELDANDDATA_H__

#ifndef REMOVE_CAF1

#include <caf/caftypes.h>
#include <e32std.h>

namespace ContentAccess
	{

	/**
	* This class is used to store non-standard mime header information.
	*
	* An example of this would be the <code>X-Oma-Drm-Separate-Delivery</code> field in 
	* the OMA DRM scheme, e.g.,
	* @code
	* X-Oma-Drm-Separate-Delivery: 12
	* @endcode
	* Here the Field is "<code>X-Oma-Drm-Separate-Delivery</code>" and the Data is "<code>12</code>"
	* @file
	*
	* @internalComponent
	* @deprecated
	*/
	class CMimeFieldAndData
		{
	public:
		/** Construct a mime field and corresponding data */
		static CMimeFieldAndData* NewL(const TDesC8& aFieldName, const TDesC8& aData);

		~CMimeFieldAndData();

		/** Accessor for field name*/
		TPtrC8 FieldName() const;
		
		/** Accessor for data */
		TPtrC8 Data() const;

	private:
		CMimeFieldAndData();
		void ConstructL(const TDesC8& aFieldName, const TDesC8& aData);
		

		HBufC8* iFieldName;
		HBufC8* iData;
		};
	}
#endif // #ifndef REMOVE_CAF1

#endif

