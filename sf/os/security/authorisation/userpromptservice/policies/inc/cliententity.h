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
* Class declaration for CClientEntity.
*
*/


/**
 @file
 @publishedPartner
 @released 
*/

#ifndef CLIENTENTITY_H
#define CLIENTENTITY_H

#include <e32base.h>
#include <e32cmn.h>
#include <ups/upsconst.h>

namespace UserPromptService
	{		
	/**
	Represents an entity executing within a client process e.g. a script or midlet.		
	
	The maximum length of the client entity name is KUpsMaxClientEntityLength (32) 
	characters. This is limited to 32 characters because it forms part of the composite
	key in the decision database and the key is limited to 255 bytes.		
	*/
	NONSHARABLE_CLASS(CClientEntity) : public CBase
		{
	public:		
		IMPORT_C static CClientEntity* NewL(const TDesC8& aName);
		IMPORT_C static CClientEntity* NewLC(const TDesC8& aName);

		IMPORT_C const TDesC8& Name() const;
		~CClientEntity();
	private:
		CClientEntity();
		void ConstructL(const TDesC8& aName);

		/** Identifies an entity with the client process */
		HBufC8* iName;
		};
	}	

#endif // CLIENTENTITY_H
