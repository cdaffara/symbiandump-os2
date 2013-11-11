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
 @released
*/


#ifndef __RIGHTSMANAGER_H__
#define __RIGHTSMANAGER_H__

#include <e32std.h>
#include <caf/caftypes.h>
#include <caf/streamableptrarray.h>
#include <f32file.h>

namespace ContentAccess
	{

	class TVirtualPathPtr;
	class CVirtualPath;
	class CRightsInfo;
	class CAgentInfo;
	class CAgentFactory;
	class CAgentRightsManager;
	class MAgentRightsBase;

	/** 
	Manages and retrieves rights objects from an agent.

	@publishedPartner
	@released
	*/
	class CRightsManager : public CBase
		{
	public:
		virtual ~CRightsManager();

	public:
		/** List all rights held by the agent.
		@param aArray The client supplied array used to store the CRightsInfo objects. The agent will add CRightsInfo objects to the supplied array.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability.
		*/
		IMPORT_C void ListAllRightsL(RStreamablePtrArray<CRightsInfo>& aArray) const;

		/** List all rights associated with a particular file.
		@param aArray The client supplied array used to store the CRightsInfo objects. The agent will add CRightsInfo objects to the supplied array.
		@param aUri The name of the file.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, const TDesC& aUri) const;

		/** List all rights associated with a particular content object
		@param aArray The client supplied array used to store the CRightsInfo objects. The agent will add CRightsInfo objects to the supplied array.
		@param aVirtualPath The content object.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, TVirtualPathPtr& aVirtualPath) const;

		/** List all rights associated with a particular content object in a file specified by file handle. Can be used when the source file is in the client's private directory.
		
		@param aArray  		The client supplied array used to store the CRightsInfo objects. The agent will add CRightsInfo objects to the supplied array.
		@param aFile  		The file handle for the file containing the content object.
		@param aUniqueId	The unique id of the content object.
		@leave KErrCANotSupported if the feature not supported.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, RFile& aFile, const TDesC& aUniqueId) const;			
		
		/** List all content associated with a particular rights object.
		@param aArray The client supplied array used to store the list of content objects. The agent will add CVirtualPath objects to the supplied array.
		@param aRightsInfo The rights object.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C void ListContentL(RStreamablePtrArray<CVirtualPath>& aArray, CRightsInfo& aRightsInfo) const;

		/** This will be used by applications to retrieve an agent specific rights object
		@param aRightsInfo The rights object.
		@return An agent specific rights object.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C MAgentRightsBase* GetRightsDataL(const CRightsInfo& aRightsInfo) const;

		/** Asks the DRM agent to delete a rights object 

		The agent may display a dialog asking the user to confirm the delete. Execution
		will be blocked until the dialog is complete. Applications can request 
		to disable the agents user interface using the SetProperty() command.
	
		@param aRightsInfo The rights object.
		@return The outcome of the delete operation.
		@return KErrNone if the rights were deleted.
		@return KErrCancel if the user cancels an agent supplied confirmation dialog.
		@return KErrNotFound if the rights object does not exist.
		@return KErrPermissionDenied if the agent does not permit the client to access rights information.
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C TInt DeleteRightsObject(const CRightsInfo& aRightsInfo);

		/** Deletes all rights associated with a particular content object

		The agent may display a dialog asking the user to confirm the delete. Execution
		will be blocked until the dialog is complete. Applications can request 
		to disable the agents user interface using the SetProperty() command.

		@param aVirtualPathPtr The content object.
		@return KErrNone if the rights were deleted.
		@return KErrNotFound if no rights objects exist for the specified content object.
		@return KErrCancel if the user cancels an agent supplied confirmation dialog.
		@return KErrPermissionDenied if the agent does not permit the client to access rights information.
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C TInt DeleteAllRightsObjects(const TVirtualPathPtr& aVirtualPathPtr);
		
		/** Deletes all rights associated with a particular content object in a file specified by file handle. Can be used when the source file is in the client's private directory.

		The agent may display a dialog asking the user to confirm the delete. Execution
		will be blocked until the dialog is complete. Applications can request 
		to disable the agents user interface using the SetProperty() command.

		@param aFile The file handle for the file containing the content object.
		@param aUniqueId The unique id of the content object.
		@return The outcome of the delete operation.
		@return KErrNone if the rights were deleted.
		@return KErrNotFound if no rights objects exist for the specified content object.
		@return KErrCancel if the user cancels an agent supplied confirmation dialog.
		@return KErrPermissionDenied if the agent does not permit the client to access rights information.
		@return KErrCANotSupported if the feature not supported.
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C TInt DeleteAllRightsObjects (RFile& aFile, const TDesC& aUniqueId); 
		
		/** Request the agent to set a property value. If the property is set
		it is only set for this CRightsManager session and does not impact other CAF users.
	
	  	@see ContentAccess::TAgentProperty

		@param aProperty The property to set.
		@param aValue The value of the property.
		@return KErrNone if the property was set.
		@return KErrCANotSupported if the agent does not support the property or value.
		@return KErrAccessDenied if the agent does not permit the property to be changed.
		@return KErrPermissionDenied if the application does not have the necessary capability to change the property.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt SetProperty(TAgentProperty aProperty, TInt aValue);

	private:

		friend class CManager;

		/** Only created by CManager */
		static CRightsManager* NewL(TUid aUid);
	
		/** Constructor */
		CRightsManager();

		/** 2nd phase constructor */
		virtual void ConstructL(TUid aUid);

	private:
		CAgentFactory* iAgentFactory;
		CAgentRightsManager* iAgentRightsManager;
		};
	}
#endif
