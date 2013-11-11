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


/**
 @file
 @publishedPartner
 @deprecated
*/

#ifndef __CAF_ATTRIBUTE_H__
#define __CAF_ATTRIBUTE_H__

#ifndef REMOVE_CAF1

#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>


namespace ContentAccess
{
	class CAgentContent;
	class CAgentFactory;
	class CBitset;
	
	/**
	 * Encapsulates the attributes of a piece of content. 
	 *
	 * These attributes will be required by DRM-aware
	 * applications in order that they can uphold DRM policies in a
	 * well-behaved manner. In general, clients set one or more attributes
	 * in the <i>Query</i> set, make a <i>GetL</i> call, and then
	 * read the agent's reponse from the <i>Response</i> set.
	 *
	 * CAttribute is a container for a number of separate
	 * ContentAccess::CBitset objects. These individual bitsets contain:
	 * -# Queries - what the client is interested in knowing.
	 * -# Responses - the answers to the queries.
	 * -# Capabilities - which attributes the system is able to support.
	 *
	 * Access to the bitset operations themselves can be achieved via the
	 * ContentAccess::CAttribute::AttributeSetL operation.
	 *
	 * @publishedPartner
	 * @deprecated 
	 */
	class CAttribute : public CBase
		{
	public:
		/**
		 Constructs a new attribute given an agent and file handle.
		 
		 @param aAgentUid	The agent Uid
		 @param aFile		A file handle 
		 @return			The new CAttribute.
		 
		 @internalComponent
		 @released		 
		 */
		static CAttribute* NewLC(TUid aAgentUid, RFile& aFile);
		
		/**
		 Constructs a new attribute given an agent and content.
		 
		 @param aAgentUid	The agent Uid
		 @param aURI		The pathname of the target content.
		 @param aShareMode Optional share mode for opening attributes of this content, defaults to EContentShareReadOnly. This parameter has no effect unless the content is a local file on the device.
		 @return			The new CAttribute.
		 
		 @internalComponent
		 @released
		 */
		static CAttribute* NewLC(TUid aAgentUid, const TDesC& aURI, TContentShareMode aShareMode=EContentShareReadOnly);

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT		
		/**
		 Constructs a new attribute given an agent and WMDRM header data.
		 
		 @param aAgentUid	The agent Uid
		 @param aHeaderData	Header data of WMDRM content
		 @return			The new CAttribute.
		 
		 @internalComponent
		 @released		 
		 */
		static CAttribute* NewLC(TUid aAgentUid, const TDesC8& aHeaderData);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		
		virtual ~CAttribute();

		/**
		 * Returns a reference to the query set
		 *
		 * The bits of the query-set should be set or cleared prior to the "GetL()" 
		 * call to specify which attributes the client is interested in.
		 *
		 * @note	Set is probably best used inline to access some of the 
		 * 			CBitset operations directly.
		 * 			@code 
		 * 			// Set EIsProtected in the Query set.
		 * 			MyAttr->QuerySet(EQuerySet).Set(EIsProtected);
		 * 			@endcode
		 *
		 * @return	A CBitset refererence.
		 */
		IMPORT_C CBitset& QuerySet();

		/**
		 * Returns a reference to the response set. 
		 * 
		 * The response set will only be valid after a call to the "GetL()" operation.
		 *
		 * @return	A CBitset refererence.
		 */
		IMPORT_C const CBitset& ResponseSet() const;
		
		/**
		 * Resets the attribute object completely. 
		 */ 
		IMPORT_C void Reset();
		
		/**
		 * Delivers the attribute object to the relevant agent for
		 * querying. 
		 * 
		 * The agent will set or reset the ResponseSet() bits for any QuerySet() bits 
		 * that are set
		 */
		IMPORT_C void GetL();
		
	private:
		void ConstructL(TUid aAgentUid, RFile& aFile);
		void ConstructL(TUid aAgentUid, const TDesC& aURI, TContentShareMode aShareMode=EContentShareReadOnly);

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		void ConstructL(TUid aAgentUid, const TDesC8& aHeaderData);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	
		CAttribute();
				
	private:
		/** The real object used to retrieve attributes. */
		CAgentContent* iAgentContent;
		CAgentFactory* iAgentFactory;

		CBitset* iQuerySet;
		CBitset* iResponseSet;
		};
	}

#endif // REMOVE_CAF1

#endif // __ATTRIBUTE_H__

