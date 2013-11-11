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

#ifndef __DRMRIGHTS_H__
#define __DRMRIGHTS_H__

#include <f32file.h>

namespace ReferenceTestAgent
	{
	class CRightsObject;
	
	class CDrmRights : public CBase
		{
	public:
		static CDrmRights* NewL();
		virtual ~CDrmRights();
	
		IMPORT_C CRightsObject& Rights(TInt aIndex);
		IMPORT_C TInt RightsCount();
		IMPORT_C void Remove(TInt aIndex);
		void AddL(CRightsObject* aRightsObject);
		
		
	private:
		CDrmRights();
	
	private:		
		RPointerArray <CRightsObject> iRightsArray;
		};
	}

#endif

