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


#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>
#include <caf/agent.h>
class CDir;

namespace ContentAccess
	{
	class CAgentResolver;
	class CRightsManager;
	class RAttributeSet;
	class RStringAttributeSet;
	class CDirStreamable;
	class CAgentManager;
	class TVirtualPathPtr;
	

	/** 
	Manages files and content access agents
	
	@publishedPartner
	@released
	*/
	class CManager : public CBase
		{
	public:

		/** Creates a CManager 

		@return A CManager object
		*/
		IMPORT_C static CManager* NewL();

		/** Creates a CManager 

		@return A CManager object
		*/
		IMPORT_C static CManager* NewLC();

		/** destructor 
		*/
		virtual ~CManager();


		/** 
		Delete a file on the device

		The agent who manages the file will be used to delete it.
		   
		In the case of a DRM agent implementation it may delete rights that 
		were associated with the file at the same time. If an agent deletes 
		rights at the same time as the content it will display a confirmation 
		dialog that makes it clear that the rights will be also deleted. 
		 
		Access to the agent private directories is permitted at the 
		agents discretion.

		@param aFileName The full pathname of the file to delete.
		@return The outcome of the delete operation.
		@return KErrNone if the file was deleted successfully.
		@return KErrCancel if the user selects cancel in an agent supplied confirmation screen.
		@return KErrAccessDenied if the agent does not allow the file to be deleted.
		@return KErrCANotSupported if the agent does not support file deletion
		@return KErrPermissionDenied if the client does not have the necessary capabilities to delete the file.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Deleting DRM protected content is not permitted for processes without DRM capability. 
		*/
		IMPORT_C TInt DeleteFile(const TDesC &aFileName) const;

		/**
		Make a copy of a file (eg to removable media) 

		Access to the agents private directory is permitted at the 
		agents discretion.

  		@param aSource The full pathname of the source file.
		@param aDestination The full pathname of the destination file.
		@return The outcome of the copy operation.
		@return KErrNone if the file was copied successfully.
		@return KErrAccessDenied if the agent does not allow the file to be copied.
		@return KErrCANotSupported if the agent does not support file copying.
		@return KErrPermissionDenied if the client does not have the necessary capabilities to copy the file.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
  		@capability DRM Copying DRM protected files is not permitted for processes without DRM capability. Copying unprotected files is permitted
		*/
		IMPORT_C TInt CopyFile(const TDesC& aSource, const TDesC& aDestination) const;

		/**
		Make a copy of a file (eg to removable media) using a file handle

		Access to the agents private directory is permitted at the 
		agents discretion.

  		@param aSourceFile The handle the source file.
		@param aDestination The full pathname of the destination file.
		@return The outcome of the copy operation.
		@return KErrNone if the file was copied successfully.
		@return KErrAccessDenied if the agent does not allow the file to be copied.
		@return KErrCANotSupported if the agent does not support file copying.
		@return KErrPermissionDenied if the client does not have the necessary capabilities to copy the file.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
  		@capability DRM Copying DRM protected files is not permitted for processes without DRM capability. Copying unprotected files is permitted
		*/
		IMPORT_C TInt CopyFile(RFile& aSourceFile, const TDesC &aDestination) const;
				
		/**
		Rename or move the content file (eg to removable media)

		Access to the agent private directories is permitted at the 
		agents discretion.

		@param aSource The full pathname of the source file.
		@param aDestination The full pathname of the destination file.
		@return The outcome of the Rename operation.
		@return KErrNone if the file was moved or renamed successfully.
		@return KErrAccessDenied if the agent does not allow the file to be moved or renamed.
		@return KErrCANotSupported if the agent does not support file renaming.
		@return KErrPermissionDenied if the client does not have the necessary capabilities to move or rename the file.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
  		@capability DRM Moving DRM protected files is not permitted for processes without DRM capability. Moving unprotected files is permitted
		*/
		IMPORT_C TInt RenameFile(const TDesC& aSource, const TDesC& aDestination) const;

		/** Create a directory

		This function can be used to create a directory within in the agent's 
		private directory. Access to the agents private directory is permitted at the 
		agents discretion. 

		@param aPath The full pathname of the directory to create.
		@return The outcome of the MkDir operation.
		@return KErrNone if the directory was created successfully.
		@return KErrAccessDenied if the agent does not allow the directory to be created.
		@return KErrCANotSupported if the agent does not support directory creation.
		@return KErrPermissionDenied if the client does not have the necessary capabilities to create directories.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Required when attempting to access an agents private directory.
		*/
		IMPORT_C TInt MkDir(const TDesC& aPath) const;

		/** Create all directories in the given path if they do not exist

		This function can be used to create directories within in the agent's 
		private directory. Access to the agents private directory is permitted at the 
		agents discretion. 
		
		@param aPath The full pathname of the directory to create.
		@return The outcome of the MkDirAll operation.
		@return KErrNone if the directory was created successfully.
		@return KErrAccessDenied if the agent does not allow the directory to be created.
		@return KErrPermissionDenied if the client does not have the necessary capabilities to create directories.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Required when attempting to access an agents private directory.
		*/
		IMPORT_C TInt MkDirAll(const TDesC& aPath) const;

		/** Remove a directory

		This function can be used to remove a directory from within the agent's 
		private directory. Access to the agents private directory is permitted 
		at the agents discretion. 
		
		@param aPath The full pathname of the directory to remove.
		@return The outcome of the RmDir operation.
		@return KErrNone if the directory was removed successfully.
		@return KErrAccessDenied if the agent does not allow the directory to be removed.
		@return KErrCANotSupported if the agent does not support directory removal.
		@return KErrPermissionDenied if the client does not have the necessary capabilities to remove directories.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Required when attempting to access an agents private directory
		*/
		IMPORT_C TInt RmDir(const TDesC& aPath) const;


		/** Rename a directory

		This function can be used to rename a directory from within the agent's 
		private directory. Access to the agents private directory is permitted 
		at the agents discretion. 
		
		@param aOldName The existing pathname of the directory to rename.
		@param aNewName The new pathname of the directory.
		@return The outcome of the RenameDir operation.
		@return KErrNone if the directory was removed successfully.
		@return KErrAccessDenied f the agent does not allow the directory to be removed.
		@return KErrCANotSupported if the agent does not support directory removal.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@return Otherwise one of the other system-wide error codes for any other errors.
		@capability DRM Required when attempting to access an agents private directory		
		*/
		IMPORT_C TInt RenameDir(const TDesC& aOldName, const TDesC& aNewName) const;


		/**  Gets a filtered list of a directory's contents. 

		The bitmask determines which file and directory entry types should be listed. The sort key determines the order in which they are listed.

		Notes:
		-# If sorting by UID (as indicated when the ESortByUid bit is OR'ed with the sort key), then UID information will be included in the listing whether or not KEntryAttAllowUid is specified in aEntryAttMask.
		-# The function sets aFileList to NULL, and then allocates memory for it before appending entries to the list. Therefore, aFileList should have no memory allocated to it before this function is called, otherwise this memory will become orphaned.
		-# The caller of this function is responsible for deleting aFileList after the function has returned.

		Access to agent private directories is permitted at 
		the agents discretion. 
		
		@see TEntryKey

  		@param aName The name of the directory for which a listing is required. Wildcards may be used to specify particular files.
		@param aEntryAttMask Bitmask indicating the attributes of interest. Only files and directories whose attributes match those specified here can be included in the listing. For more information, see KEntryAttMatchMask and the other directory entry details. Also see KEntryAttNormal and the other file or directory attributes 
		@param aEntrySortKey The sort key. This is a set of flags indicating the order in which the entries are to be sorted. These flags are defined by TEntryKey. 
		@param aEntryList On return contains a filtered list of directory and file entries.
		@return The outcome of the GetDir operation.
		@return KErrNone if the directory contents were listed successfully.
		@return KErrCANotSupported if the agent does not allow clients to view its private directory.
		@return KErrPermissionDenied if the process does not have the correct capabilities to view the directory.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Required when attempting to access an agents private directory
		*/
		IMPORT_C TInt GetDir(const TDesC& aName, TUint aEntryAttMask, TUint aEntrySortKey, CDir*& aEntryList) const;

		/**  Gets a filtered list of the directory and file entries contained in a directory and a list of the directory entries only

		The bitmask determines which file and directory entry types should be listed in aFileList. The contents of the second list, aDirList are not affected by the bitmask; it returns all directory entries contained in directory aName. The sort key determines the order in which both lists are sorted.

		Notes:
		-# If sorting by UID (as indicated when the ESortByUid bit is OR'ed with the sort key), then UID information will be included in the listing whether or not KEntryAttAllowUid is specified in aEntryAttMask.
		-# The function sets both aFileList and aDirList to NULL, and then allocates memory to them before appending entries to the lists. Therefore, aFileList and aDirList should have no memory allocated to them before this function is called, otherwise the allocated memory will become orphaned.
		-# The caller of this function is responsible for deleting aFileList and aDirList after the function has returned.

		Access to agent private directories is permitted at 
		the agents discretion. 

		@see TEntryKey

		@param aName The name of the directory for which a listing is required. Wildcards may be used to specify particular files. 
		@param aEntryAttMask Bitmask indicating the attributes of interest. Only files and directories whose attributes match those specified here can be included in aFileList. aDirList is unaffected by this mask. For more information, see KEntryAttMatchMask and the other directory entry details. Also see KEntryAttNormal and the other file or directory attributes.
		@param aEntrySortKey The sort key. This is a set of flags indicating the order in which the entries in both lists are to be sorted. These flags are defined by TEntryKey. 
		@param aEntryList On return contains a filtered list of directory and file entries. 
		@param aDirList On return contains a filtered list of directory entries only.
		@return The outcome of the GetDir operation.
		@return KErrNone The directory contents were listed successfully.
		@return KErrCANotSupported The agent does not allow clients to view its private directory.
		@return KErrPermissionDenied If the process does not have the correct capabilities to view the directory.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Required when attempting to access an agents private directory.
		*/
		IMPORT_C TInt GetDir(const TDesC& aName, TUint aEntryAttMask, TUint aEntrySortKey, CDir*& aEntryList, CDir*& aDirList) const;

		/**  Gets a filtered list of a directory's contents by UID

		The aUidType parameter determines which file entry types should be listed. The sort key determines the order in which they are listed.

		Notes:
		-# The function sets aFileList to NULL, and then allocates memory for it before appending entries to the list. Therefore, aFileList should have no memory allocated to it before this function is called, otherwise this memory will become orphaned.
		-# The caller of this function is responsible for deleting aFileList after the function has returned.

		Access to agent private directories is permitted at 
		the agents discretion. 

		@see TEntryKey

		@param aName The name of the directory for which a listing is required. Wildcards may be used to specify particular files. 
		@param aEntryUid Only those files whose UIDs match those specified within this UID type will be included in the file list. Any, or all, of the three UIDs within the UID type may be omitted. Any UID which is omitted acts in a similar manner to a wildcard character, matching to all UIDs. 
		@param aEntrySortKey The sort key. This is a set of flags indicating the order in which the entries are to be sorted. These flags are defined by TEntryKey.
		@param aFileList On return contains a filtered list of directory and file entries.
		@return The outcome of the GetDir operation.
		@return KErrNone The directory contents were listed successfully.
		@return KErrCANotSupported The agent does not allow clients to view its private directory.
		@return KErrPermissionDenied If the process does not have the correct capabilities to view the directory.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Required when attempting to access an agents private directory.
		*/
		IMPORT_C TInt GetDir(const TDesC& aName, const TUidType& aEntryUid, TUint aEntrySortKey, CDir*& aFileList) const;

		/**  Get an attribute from a content object

		@see ContentAccess::TAttribute

		e.g.

		@code
		TInt value = 0;
		CManager* manager = CManager::NewL();
		
		err = manager->GetAttribute(EIsProtected, value, aVirtualPath);
		if(err == KErrNone && value)
			{
			DisplayPadLock();
			}
		@endcode
	
		@param aAttribute The attribute to retrieve, from ContentAccess::TAttribute.
		@param aValue Used to return the value of the attribute.
		@param aVirtualPath The content object whose attributes are to be queried.
		@return Whether the attribute value was updated.
		@return KErrNone if the attribute value was updated.
		@return KErrNotFound if the URI or the object with the given UniqueId inside the file was not found.
		@return KErrCANotSupported if the requested attribute is not supported for this content object.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttribute(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath) const;

		/**  Get a content's attribute from a file specified by file handle. Can be used when the source file is in the client's private directory.
	
		@param aAttribute The attribute to retrieve, from ContentAccess::TAttribute.
		@param aValue Used to return the value of the attribute.
		@param aFile The file handle for the file containing the content object.
		@param aUniqueId The unique id of the content object.
		@return Whether the attribute value was updated.
		@return KErrNone if the attribute value was updated.
		@return KErrNotFound if the URI or the object with the given UniqueId inside the file was not found.
		@return KErrCANotSupported if the feature or the requested attribute is not supported for this content object.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttribute(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId);

		/** Get a set of attributes from a content object

		@see ContentAccess::TAttribute

		The following example determines whether the content object is protected
		and has rights that will enable it to be viewed by the user
		
		@code
		TInt err = KErrNone;
		TInt value = 0;
		
		// The manager		
		CManager* manager = CManager::NewLC();

		// Prepare the RAttributeSet object with the attributes to query		
		RAttributeSet attributeSet;
		CleanupClosePushL(attributeSet);
		attributeSet.AddL(EProtected);
		attributeSet.AddL(ECanView);
		
		// Retrieve the attributes from the agent
		User::LeaveIfError(manager->GetAttributeSet(attributeSet, aVirtualPath));
		
		// Check if the content object is protected
		err =attributeSet.GetValue(EProtected, value);
		if(err == KErrNone && value)
			{
			// content object is DRM protected
			}
		
		// Check if the content object can be display on screen
		err = attributeSet.GetValue(ECanView, value);
		if(err == KErrNone && value)
			{
			// content object is DRM protected
			}
	
  		// Can reuse the RAttributeSet if necessary
		User::LeaveIfError(manager->GetAttributeSet(attributeSet, aVirtualPath));
		
		...
		// Finished
		CleanupStack::PopAndDestroy(2);		// manager, attributeSet.Close()
		
		@endcode


		@param aAttributeSet The set of attributes to query and update.
		@param aVirtualPath The content object to retrieve attributes from.
		@return Whether the attribute set was updated.
		@return KErrNone if the attribute set was updated successfully.
		@return KErrNotFound if the content object was not found.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath) const;

		/** Get a content's set of attributes from a file specified by file handle. Can be used when the source file is in the client's private directory.

		@param aAttributeSet The set of attributes to query and update.
		@param aFile The file handle for the file containing the content object.
		@param aUniqueId The unique id of the content object.
		@return Whether the attribute set was updated.
		@return KErrNone if the attribute set was updated successfully.
		@return KErrNotFound if the content object was not found.
		@return KErrCANotSupported if the feature not supported.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttributeSet(RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId);	

		/**  Get text string attributes or meta-data from the file 
		
		@see ContentAccess::TStringAttribute

		e.g.
		@code
		CManager* manager = CManager::NewLC();
		
		TBuf <MAX_PATH> previewUri;
		if(manager->GetStringAttribute(EPreviewURI, previewUri, uri) == KErrNone)
			{
			DisplayPreview(previewUri);
			}
			
		CleanupStack::PopAndDestroy();	// manager
		@endcode

		@param aAttribute The attribute to retrieve, from ContentAccess::TStringAttribute.
		@param aValue Used to return the value of the attribute.
		@param aVirtualPath The content object whose attributes are to be retrieved.
		@return Whether the value was updated.
		@return KErrNone if the attribute was retrieved successfully.
		@return KErrNotFound if the content object does not exist.
		@return KErrCANotSupported if the requested attribute does not apply to this content object.
		@return KErrOverflow if the buffer was not large enough to return the result.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		*/
		IMPORT_C TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath) const;

		/**  Get a content's text string attribute from a file specified by file handle. Can be used when the source file is in the client's private directory.
	
		@param aAttribute The attribute to retrieve, from ContentAccess::TAttribute.
		@param aValue Used to return the value of the attribute.
		@param aFile The file handle for the file containing the content object.
		@param aUniqueId The unique id of the content object whose attributes are to be retrieved.
		@return Whether the attribute value was updated.
		@return KErrNone if the attribute value was updated.
		@return KErrNotFound if the content object does not exist.
		@return KErrCANotSupported if the feature or the requested attribute is not supported for this content object.
		@return KErrOverflow if the buffer was not large enough to return the result.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/		
		IMPORT_C TInt GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId);
		
		/** Used to obtain a set of string attributes 

		@see ContentAccess::TStringAttribute

		@code
		TInt err = KErrNone;
		TInt value = 0;
		
		CManager* manager = CManager::NewLC();
		
		// Prepare the RStringAttributeSet object with the attributes to query		
		RStringAttributeSet stringAttributeSet;
		CleanupClosePushL(stringAttributeSet);
		stringAttributeSet.AddL(EPreviewURI);
		
		// Retrieve the attributes from the agent
		User::LeaveIfError(manager->GetAttributeSet(stringAttributeSet, aVirtualPath));
		
		// display preview
		TBuf <MAX_PATH> previewUri;
		if(stringAttributeSet.GetValue(ECanView, previewUri))
			{
			DisplayPreview(previewUri);
			}
	
		// Finished
		CleanupStack::PopAndDestroy(2);		// manager, stringAttributeSet.Close()
		@endcode


		@param aStringAttributeSet The set of attributes to query and update.
		@param aVirtualPath The content object whose attributes are to be retrieved.
		@return Whether the string attribute set was updated.
		@return KErrNone if the attribute set was updated successfully.
		@return KErrNotFound if the object with the given virtual path was not found.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TVirtualPathPtr& aVirtualPath) const;

		/** Get a content's set of string attributes from a file specified by file handle. Can be used when the source file is in the client's private directory.

		@param aStringAttributeSet The set of attributes to query and update.
		@param aFile The file handle for the file containing the content object.
		@param aUniqueId The unique id of the content object whose attributes are to be retrieved	
		@return Whether the string attribute set was updated.
		@return KErrNone if the attribute set was updated successfully.
		@return KErrNotFound if the object with the given virtual path was not found.
		@return KErrCANotSupported if the feature not supported.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId);

		/** Notify the caller when the status of a DRM protected content object changes

		@see ContentAccess::TEventMask

		@param aURI The location of the file.
		@param aMask Bitmask of events the caller is interested in.
		@param aStatus The TRequestStatus object to complete if the event occurs, or KErrCANotSupported if the agent does not support asynchronous notifications.
		@capability DRM Deleting DRM protected content is not permitted for processes without DRM capability. 
		*/
		IMPORT_C void NotifyStatusChange(const TDesC& aURI, TEventMask aMask, TRequestStatus& aStatus);

		/** Cancel a previous notification request
		@param aURI The URI supplied in the call to NotifyStatusChange().
		@param aStatus The TRequestStatus supplied in the call to NotifyStatusChange().
		@return The outcome of the cancel request.
		@return KErrNone if the notification was cancelled.
		@return KErrNotFound if there was no matching request outstanding.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt CancelNotifyStatusChange(const TDesC& aURI, TRequestStatus& aStatus);

		/** Request all agents to set a property value. If the property is set
		it is only set for this CManager session and does not impact other CAF
		users.

		@see ContentAccess::TAgentProperty

		@param aProperty The property to set.
		@param aValue The value of the property.
		@return The outcome of the set property command.
		@return KErrNone if the property was set by all agents.
		@return KErrCANotSupported if one of the agent does not support the property or value.
		@return KErrAccessDenied if one of the agents does not permit the property to be changed.
		@return KErrPermissionDenied if the application does not have the necessary capability to change the property.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt SetProperty(TAgentProperty aProperty, TInt aValue);

		/** View information associated with a single content object
		
		This call blocks execution and only returns once the display is dismissed 
		by the user

		@see ContentAccess::TDisplayInfo

		@param aInfo The information to display.
		@param aVirtualPath The content object.
		@leave KErrCANotSupported if agent cannot display the requested information.
		@leave ...		One of the other CAF error codes defined in \c caferr.h  
		 				or one of the system-wide error codes 
						for any other errors.		
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C void DisplayInfoL(TDisplayInfo aInfo, const TVirtualPathPtr& aVirtualPath);
		
		/** View information associated with a single content object in a file specified by file handle. Can be used when the source file is in the client's private directory.
		
		This call blocks execution and only returns once the display is dismissed 
		by the user.

		@param aInfo The information to display.
		@param aFile The file handle for the file containing the content object.
		@param aUniqueId The unique id of the content object.	
		@leave KErrCANotSupported if the feature not supported or if agent cannot display the requested information.
		@leave ...		One of the other CAF error codes defined in \c caferr.h  
		 				or one of the system-wide error codes 
						for any other errors.		
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C void DisplayInfoL(TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId);

		/** List all the agents installed on the device (except F32Agent) 

		The caller must call RPointerArray::ResetAndDestroy() on the array 
		when it has finished.

		@param aAgents On completion this will contain a list of all the agents.
		*/
		IMPORT_C void ListAgentsL(RArray <TAgent>& aAgents);

		/** Allows extended synchronous calls to an agent
		
		@param aAgent The agent.
		@param aCommand The agent defined command.
		@param aInputBuffer Non modifyable input data buffer.
		@param aOutputBuffer Modifyable output buffer to hold the result of the command.
		@return The outcome of the agent specific command.
		@return KErrNone if the command was successful.
		@return KErrCANotSupported if the agent does not recognize the command.
		@return KErrPermissionDenied if the agent does not permit the client to execute this command.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  
				or one of the other system-wide error codes 
				for any other errors.		
  		@capability DRM Access to extended DRM agent functions is not permitted for processes without DRM capability
		*/
		IMPORT_C  TInt AgentSpecificCommand(TAgent& aAgent, TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);

		/** Allows extended asynchronous calls to an agent.
		NB: It is important that the descriptor passed to 
		aOutputBuffer remains in scope until the request has completed.

		@param aAgent The agent.
  		@param aCommand The agent defined command.
		@param aInputBuffer Non modifyable input data buffer.
		@param aOutputBuffer Modifyable output buffer to hold the result of the command.
		@param aStatus 	Asynchronous request status. On completion this will 
						contain one of the following error codes: KErrNone if the command
						was successful. KErrCANotSupported if the agent does 
						not recognize the command. KErrPermissionDenied if the 
						agent does not permit the client to execute this command.
						Otherwise one of the other CAF error codes 
						defined in \c caferr.h  or one of the other 
						system-wide error codes for any other errors.
		@capability DRM Access to extended DRM agent functions is not permitted for processes without DRM capability
		*/
		IMPORT_C void AgentSpecificCommand(TAgent& aAgent, TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);
	
		/** Allows a client to display management information for a particular agent
		
		This allows a user to see all the specific information associated
		with a particular agent. In the case of an agent implementing DRM this could 
		include the ability to list, view or delete DRM rights objects.

		@param aAgent The agent.
		@leave KErrCANotSupported If the specified agent does not support the display of management information.
		@capability DRM Agents implementing DRM may not display their management dialog to processes without DRM capability
		*/
		IMPORT_C void DisplayManagementInfoL(TAgent& aAgent);  

		/** Allow clients to list, view and delete the rights contained by an agent 
		in a generic manner.
		
		This function is only relevant for agents implementing a DRM scheme. Other agents
		will leave with KErrCANotSupported if an application attempts to invoke the
		rights manager.

		To manage the rights in a more comprehensive manner the application should use the 
		DisplayManagementInfoL() function where the agent can present its own 
		management information.

		@param aAgent The agent to create a rights manager object.
		@return A CRightsManager object for the specified agent.

		@leave KErrCANotSupported If the agent does not implement DRM or support rights management.
		@leave KErrPermissionDenied If the agent does not allow the client to create a rights manager.
  		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		IMPORT_C CRightsManager* CreateRightsManagerL(TAgent& aAgent) const;
		
#ifndef REMOVE_CAF1
		/** Delete a file
		@param aFileName The file to delete.
		@deprecated Use DeleteFile() instead.
		*/
		IMPORT_C static void DeleteFileL (const TDesC &aFileName);
#endif // REMOVE_CAF1

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		/**  Get an attribute from WMDRM content.
	
		@param aAttribute	The attribute to retrieve, from ContentAccess::TAttribute.
		@param aValue		Used to return the value of the attribute.
		@param aHeaderData	Header data of WMDRM content.
		@return				Whether the attribute value was updated.
		@return				KErrNone if the attribute value was updated.
		@return				KErrCANotSupported if the requested attribute is not supported for this content.
		@return				Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		@capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue) const;
		
		/** Get a set of attributes from WMDRM content.

		@param aAttributeSet	The set of attributes to query and update.
		@param aHeaderData		Header data of WMDRM content.
		@return 				Whether the attribute set was updated.
		@return 				KErrNone if the attribute set was updated successfully.
		@return 				Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
								other system-wide error codes for any other errors.
		@capability DRM 		Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttributeSet(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet) const;
		
		
		/**  Get text string attributes or meta-data from WMDRM content.

		@param aAttribute	The attribute to retrieve, from ContentAccess::TStringAttribute.
		@param aValue		Used to return the value of the attribute.
		@param aHeaderData	Header data of WMDRM content.
		@return				Whether the value was updated.
		@return				KErrNone if the attribute was retrieved successfully.
		@return				KErrNotFound if the content object does not exist.
		@return				KErrCANotSupported if the requested attribute does not apply to this content object.
		@return				KErrOverflow if the buffer was not large enough to return the result.
		@return				Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		@capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue) const;
		
		/** Used to obtain a set of string attributes from WMDRM content.

		@param aStringAttributeSet	The set of attributes to query and update.
		@param aHeaderData			Header data of WMDRM content.
		@return 					Whether the string attribute set was updated.
		@return 					KErrNone if the attribute set was updated successfully.
		@return 					KErrNotFound if the object with the given virtual path was not found.
		@return 					Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
									other system-wide error codes for any other errors.
		@capability DRM 			Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetStringAttributeSet(const TDesC8& aHeaderData, RStringAttributeSet& aStringAttributeSet) const;

#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

	private:
		CManager();
		void ConstructL();

		/** Populate the list of agents in the CDirStreamable 
		@param aDir Reference to a CDir pointer, this pointer must be set to NULL
		*/
		void GetListOfAgentPrivateDirectoriesL(CDir *&aDir) const;
		
		// Implemenetation functions for public exports
		void DoDeleteFileL(const TDesC &aFileName) const;
		void DoCopyFileL(const TDesC& aSource, const TDesC& aDestination) const;		
		void DoCopyFileL(RFile& aSourceFile, const TDesC &aDestination) const;
		void DoRenameFileL(const TDesC& aSource, const TDesC& aDestination) const;		
		void DoMkDirL(const TDesC &aPath) const;
		void DoMkDirAllL(const TDesC &aPath) const;
		void DoRmDirL(const TDesC &aPath) const;
		void DoRenameDirL(const TDesC &aOldName, const TDesC& aNewName) const;
		void DoGetDirL(const TDesC& aName, TUint aEntryAttMask, TUint aEntrySortKey, CDir*& aEntryList) const;
		void DoGetDirL(const TDesC& aName, TUint aEntryAttMask, TUint anEntrySortKey, CDir*& aEntryList, CDir*& aDirList) const;
		void DoGetDirL(const TDesC &aName, const TUidType &aEntryUid, TUint aEntrySortKey, CDir *&aFileList) const;
		void DoGetAttributeL(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath) const;
		void DoGetAttributeL(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId) const;
		void DoGetAttributeSetL(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath) const;		
		void DoGetAttributeSetL(RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId) const;
		void DoGetStringAttributeL(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath) const;		
		void DoGetStringAttributeL(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId) const;
		void DoGetStringAttributeSetL(RStringAttributeSet& aStringAttributeSet, const TVirtualPathPtr& aVirtualPath) const;		
		void DoGetStringAttributeSetL(RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId) const;
		void DoNotifyStatusChangeL(const TDesC& aURI, TEventMask aMask, TRequestStatus& aStatus);		
		void DoCancelNotifyStatusChangeL(const TDesC& aURI, TRequestStatus& aStatus);
		void DoSetPropertyL(TAgentProperty aProperty, TInt aValue);
		
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		void DoGetAttributeL(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue) const;
		void DoGetAttributeSetL(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet) const;
		void DoGetStringAttributeL(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue) const;
		void DoGetStringAttributeSetL(const TDesC8& aHeaderData, RStringAttributeSet& aStringAttributeSet) const;	
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	
	private:
		// Holds instances of all the agents
		CAgentResolver* iResolver;
		};

	} // namespace ContentAccess

#endif /* __MANAGER_H__ */
