// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef STREAMAGENTRESOLVER_H
#define STREAMAGENTRESOLVER_H

#include <e32base.h>
#include <ecom/ecom.h>
#include "streamagentinfo.h"

namespace StreamAccess
	{
	
	class CStreamAgentInfo;
	
	/**
	 * Responsible for the identification of stream agent ECOM plug-in implementations and
	 * the selection of the most appropriate agent based on the key stream session description
	 * metadata and plug-in resource file information. 
	 */
	class CStreamAgentResolver : public CBase
		{
	public:
	
		/* Constructs a new instance of CStreamAgentResolver and pushs it onto the cleanup stack
		 * @return Pointer to the new CStreamAgentResolver object
		 */
		static CStreamAgentResolver* NewLC();
		
		/* Destructor
		 */
		~CStreamAgentResolver();
		
		/* Resolves a stream agent by the SDP (Session Description Protocol) media description
		 * of the key stream.
		 * @param aSdpKeyStream SDP media description of the key stream
		 * @return Reference to the CStreamAgentInfo instance for the chosen Stream Agent, capable
		 * of decoding the key stream. 
		 * NOTE: The lifetime of the returned reference is limited by the lifetime of the resolver. 
		 * The CStreamAgentInfo reference cannot be used after the CStreamAgentResolver object is
		 * destroyed.
		 * @leave KErrCANoAgent No stream agent can be found that supports the given SDP key management
		 * description
		 * @leave One of the system-wide error codes
		 */
		CStreamAgentInfo& ResolveSdpKeyStreamL(const CSdpMediaField& aSdpKeyStream) const;
		
	protected:
		
		/* Constructor
	 	*/
		CStreamAgentResolver();
	
		/* Second phase of construction for CStreamAgentResolver 
		 */
		void ConstructL();
		
	private:
	
		/* Constructs a list of information about plug-in implementations located
		 * by the ECOM framework that satisfy the stream agent interface.
		 */
		void BuildStreamAgentListL();
		
		/* Adds a new instance of a stream agent to the CStreamAgentInfo array
		 * @param aAgentInfo Registration data regarding the stream agent interface implementation,
		 * retrieved from the associated resource file
		 */
		void AddStreamAgentL(const CImplementationInformation& aAgentInfo);
			
		/* Cleanup Item Operation used when constructing the stream agent list via ECOM
		 * @param aArray Pointer to an RImplInfoPtrArray array which is used by ECOM to store 
		 * information about located stream agents implementations
		 */ 
		static void CleanAgentImplArray(TAny* aArray);
		
	private:
		
		/* Array of information regarding stream agent interface implementations located by ECOM
		 */
		RPointerArray<CStreamAgentInfo> iStreamAgentInfoArray;
		
		};
	} // namespace StreamAccess
#endif /*STREAMAGENTRESOLVER_H*/
