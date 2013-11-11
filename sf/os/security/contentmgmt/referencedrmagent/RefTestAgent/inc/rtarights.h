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
* RTA - client Reference Test Server interface
*
*/


/**
 @file 
 @test
 @internalAll
*/

#ifndef __RTARIGHTS_H__
#define __RTARIGHTS_H__

#include <e32std.h>
#include "rtaclient.h"

namespace ReferenceTestAgent
	{
	class CRightsObject;
	class RRtaRights : public RRtaClient
		{
	public:
		// protected constructor so only derived classes can access it
		IMPORT_C RRtaRights();
		IMPORT_C TInt Connect();

		IMPORT_C void AddRightsL(CRightsObject& aRights);
		IMPORT_C void ClearAllRightsL();
		IMPORT_C void DumpRightsDatabaseL(const TDesC& aFileName);
		IMPORT_C void DeleteRightsByKeyL(const TDesC8& aDeleteKey);
		IMPORT_C void EraseDeleteKeyL(const TDesC8& aDeleteKey);
		IMPORT_C void ReloadRightsDatabaseL(const TDesC& aFileName);
		};
	} // namespace
#endif
