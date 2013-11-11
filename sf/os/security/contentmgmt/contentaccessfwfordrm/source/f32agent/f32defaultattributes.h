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


#ifndef __F32DEFAULTATTRIBUTES_H__
#define __F32DEFAULTATTRIBUTES_H__

namespace ContentAccess
	{
	class TVirtualPathPtr;
	class RAttributeSet;
	class RStringAttributeSet;

	/**
	All attribute values are common between the CAgentData, CAgentContent and
	CAgentManager. The F32 attributes are mostly fixed anyway so this class
	implements helper functions for all of the attribute functions
		 
	 @internalComponent
	 @released
	 */
	class TF32DefaultAttributes
		{
	public:
		static TInt GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUri);
		static TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUri);
		static TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUri);
		static TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUri);
		static void GetMimeTypeL(const TDesC& aUri, TDes8& aMimeType);
		
		static TInt GetAttribute(TInt aAttribute, TInt& aValue, RFile& aFile);
		static TInt GetAttributeSet(RAttributeSet& aAttributeSet, RFile& aFile);
		static TInt GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile);
		static TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, RFile& aFile);
		static void GetMimeTypeL(RFile& aFile, TDes8& aMimeType);
		
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT		
		static TInt GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue);
		static TInt GetAttributeSet(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet);
		static TInt GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue);
		static TInt GetStringAttributeSet(const TDesC8& aHeaderData, RStringAttributeSet& aStringAttributeSet);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		static TInt CheckUniqueId(const TDesC& aUniqueId);
		static TInt CheckVirtualPath(const TVirtualPathPtr& aVirtualPath);	
		static TUint GetFileMode(TContentShareMode aMode);
		};
} // namespace ContentAccess
#endif // __F32DEFAULTATTRIBUTES_H__
