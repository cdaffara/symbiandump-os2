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


#ifndef __DRMFILECONTAINER_H__
#define __DRMFILECONTAINER_H__

#include <e32base.h>
#include <f32file.h>
#include <caf/attributeset.h>
#include <caf/stringattributeset.h>
#include <caf/streamableptrarray.h>
#include <caf/caftypes.h>
#include <caf/embeddedobject.h>

namespace ReferenceTestAgent
	{
	class CDrmFileContent;
	
	class CDrmFileContainer : public CBase
		{
	public:
		static CDrmFileContainer* NewL(CDrmFileContainer* aParent);
		virtual ~CDrmFileContainer();
		
		// Container related functions
		IMPORT_C void GetEmbeddedObjectsL(ContentAccess::RStreamablePtrArray<ContentAccess::CEmbeddedObject>& aArray); 
		IMPORT_C void GetEmbeddedObjectsL(ContentAccess::RStreamablePtrArray<ContentAccess::CEmbeddedObject>& aArray, ContentAccess::TEmbeddedType aType); 
		IMPORT_C TInt Search(ContentAccess::RStreamablePtrArray<ContentAccess::CEmbeddedObject> &aArray, const TDesC8 &aMimeType, TBool aRecursive); 

		// Used for tree traversal
		CDrmFileContainer* ParentContainer();	
		RPointerArray <CDrmFileContainer>& Containers();
		RPointerArray <CDrmFileContent>& Contents();
		void ListAllContentL(RPointerArray <CDrmFileContent>& aContentArray);

		
		// Add a container under this container and return a reference to the new container
		CDrmFileContainer& AddContainerL();

		
		const TDesC& UniqueId();
		const TDesC& Name();
		
		// set parameters of the Container
		void SetNameL(const TDesC& aName);
		void SetUniqueIdL(const TDesC& aUniqueId);

		// Add a content object 
		void AddContentL(const TDesC8& aCid, const TDesC& aName, const TDesC& aUniqueId, const TDesC& aFileName, ContentAccess::RAttributeSet& aAttributeSet, ContentAccess::RStringAttributeSet& aStringAttributeSet);

	private:
		CDrmFileContainer(CDrmFileContainer* aParent);
		void SearchL(ContentAccess::RStreamablePtrArray<ContentAccess::CEmbeddedObject> &aArray, const TDesC8 &aMimeType, TBool aRecursive); 
		
	private:
		HBufC* iName;
		HBufC* iUniqueId;

		CDrmFileContainer* iParentContainer;
		RPointerArray <CDrmFileContainer> iContainers;
		RPointerArray <CDrmFileContent> iContents;
		};
	}

#endif
