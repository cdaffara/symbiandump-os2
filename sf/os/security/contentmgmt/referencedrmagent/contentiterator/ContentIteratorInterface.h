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


#ifndef __CONTENTITERATORINTERFACE_H__
#define __CONTENTITERATORINTERFACE_H__

#include <e32base.h>

namespace ContentAccess 
	{
	/**
	@internalComponent
	@released
	*/
	class MContentIterator :public CBase
		{
	public:
		/** The location of the current content object 
		*/
		virtual const TDesC& FileName() const = 0;
		virtual const TDesC& UniqueId() const = 0;
		virtual const TDesC& Name() const = 0;
		virtual const TDesC8& MimeType() const = 0;
		
	
		/** Move iterator to the next content object 

		@return KErrNone if successful, KErrNotFound if there are no more content objects in the specified search
		*/
		virtual TInt Next() = 0;
		};
	}
#endif
