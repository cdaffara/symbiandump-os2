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
 @publishedAll
 @released
*/

#ifndef __CAF_AGENT_H__
#define __CAF_AGENT_H__

#include <e32base.h>

/** Maximum length of an Agent name, a longer name will be truncated */
const TInt KMaxAgentNameLength = KMaxFileName;
const TInt KMaxSIDLength = 8;

namespace ContentAccess
	{

	/** Represents the name of a single CAF agent. This allows 
	applications to reference a particular agent within the 
	Content Access Framework.
	*/
	class TAgent 
		{
	public:
		/** The name of the agent 
		@return The name of the agent
		*/
		IMPORT_C const TDesC& Name() const;

		/** The UID of the agent DLL
		@return The UID of the agent ECOM implementation DLL 
		*/
		TUid ImplementationUid() const;

		/** Set the value of this TAgent
		@param aName The name of the agent
		@param aUid The agent implementation Uid
		*/
		void SetValue(const TDesC& aName, TUid aUid);

	private:

		TBuf <KMaxAgentNameLength> iName;
		TUid iImplementationUid;
		};
	}

#endif
