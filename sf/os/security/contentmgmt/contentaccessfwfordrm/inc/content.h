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

@publishedAll
@released
*/


#ifndef __CONTENT_H__
#define __CONTENT_H__

#include <e32base.h>
#include <f32file.h>

#include <caf/caftypes.h>
#include <caf/agent.h>
#include <caf/streamableptrarray.h>
#include <caf/virtualpathptr.h>

namespace ContentAccess
	{
	class CAgentResolver;
	class CAgentFactory;
	class CAgentContent;
	class CData;
	class CEmbeddedObject;
	class RAttributeSet;
	class RStringAttributeSet;

	

#ifndef REMOVE_CAF1
	class CAttribute;
#endif
	

/**
   CContent allows applications to browse the content objects contained within a 
   single file and construct a CData object for reading that content.
 
  Applications will use an object of this type rather than the traditional 
  RFile mechanisms. They will, however, be  required to indicate 
  <i>DRM Intent</i> - something that determines whether (and how) access to
  the content should be permitted by a Content-Access agent.
 
  CContent allows direct access to meta-data such as the attributes of 
  the content and indirect access to the plaintext data itself through 
  the CData class. 
  
  Applications can use CContent to browse the hierarchy of content objects 
  within a file containing several content objects.
   
  During construction CContent loads the correct CAF Agent plugin to handle 
  the file specified by the URI.
 */
	class CContent :  public CBase
		{
	public:

		/**
		 Constructs a new CContent object with a given URI. The URI can be in the same format as a virtual path for more information see CVirtualPath and TVirtualPathPtr

		 @param aURI	The filename, URI or virtual path of the DRM file.
		 @return a new CContent object.
		 @leave KErrNotFound If the file at the given URI does not exist or if an invalid URI or virtual path have been supplied.
		 @leave KErrPermissionDenied If the access to the protected content is not permitted by the CAF Agent.
		 @leave ...		One of the CAF error codes defined in \c caferr.h  or one of the 
						other system-wide error codes for any other errors.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		 */
		IMPORT_C static CContent* NewLC(const TDesC& aURI);

		/**
		 Constructs a new CContent object with a given URI. The URI can be in the same format as a virtual path for more information see CVirtualPath and TVirtualPathPtr

		 @param aURI	The filename, URI or virtual path of the DRM file.
		 @return a new CContent object.
		 @leave KErrNotFound If the file at the given URI does not exist or if an invalid URI or virtual path have been supplied.
		 @leave KErrPermissionDenied If the access to the protected content is not permitted by the CAF Agent.
		 @leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		 */
		IMPORT_C static CContent* NewL(const TDesC& aURI);
		
		/**
		 Constructs a new CContent object with a given URI. The URI can be in the same format as a virtual path for more information see CVirtualPath and TVirtualPathPtr

		 @param aURI	The filename, URI or virtual path of the DRM file.
		 @param aShareMode The sharing mode used when accessing the content.
		 @return a new CContent object.
		 @leave KErrNotFound If the file at the given URI does not exist or if an invalid URI or virtual path have been supplied.
		 @leave KErrAccessDenied If the file is already open with a different share mode.
		 @leave KErrPermissionDenied If the access to the protected content is not permitted by the CAF Agent.
		 @leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		 */
		IMPORT_C static CContent* NewLC(const TDesC& aURI, TContentShareMode aShareMode);
		
		/** 
		Constructs a new CContent object with a given URI. The URI can be in the same format as a virtual path for more information see CVirtualPath and TVirtualPathPtr

		@param aURI	The filename, URI or virtual path of the DRM file.
		@param aShareMode The sharing mode used when accessing the content.
		@return a new CContent object.
		@leave KErrNotFound If the file at the given URI does not exist or if an invalid URI or virtual path have been supplied.
		@leave KErrAccessDenied If the file is already open with a different share mode.
		@leave KErrPermissionDenied If the access to the protected content is not permitted by the CAF Agent.
		@leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C static CContent* NewL(const TDesC& aURI, TContentShareMode aShareMode);

		/**
		 Constructs a new CContent object with an open file handle.

  		 CContent will use a duplicate of this file handle, the client is 
		 free to close its file handle any time after creating the CContent 
		 object.

		 @param aFile The file handle. The file must already be open before calling NewLC. 
		 The underlying RFs session should be shared for transfer to other processes using RFs::ShareProtected(), since the agent may use transfer this handle to its own or one of the system servers.
		 @return a new CContent object.
		 @leave KErrPermissionDenied If the access to the protected content is not permitted by the CAF Agent.
		 @leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		 */
		IMPORT_C static CContent* NewLC(RFile& aFile);
		
		/** 
		 Constructs a new CContent object with an open file handle.

  		 CContent will use a duplicate of this file handle, the client is 
		 free to close its file handle any time after creating the CContent 
		 object.

		 @param aFile The file handle. The file must already be open before calling this method. 
		 The underlying RFs session should be shared for transfer to other processes using RFs::ShareProtected(), since the agent may use transfer this handle to its own or one of the system servers.
		 @return a new CContent object.
		 @leave KErrPermissionDenied If the access to the protected content is not permitted by the CAF Agent.	 
		 @leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C static CContent* NewL(RFile& aFile);

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT		
		/** 
		 Constructs a new CContent object.

  		 @param aHeaderData	Header data of WMDRM file/stream content. 
		 @return 			CContent object.
		 @leave 			KErrMissingHeaderData if the header data is NULL or one of the CAF error codes defined in caferr.h  
		 					or one of the system-wide error codes for any other errors.
		 @capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
	 	 */
		IMPORT_C static CContent* NewL(const TDesC8& aHeaderData);
		
		/** 
		 Constructs a new CContent object.
  		 
  		 @param aHeaderData	Header data of WMDRM file/stream content.  
		 @return 			CContent object.
		 @leave 			KErrMissingHeaderData if the header data is NULL or one of the CAF error codes defined in caferr.h  or one of the 
							system-wide error codes for any other errors.
		 @capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		 */
		IMPORT_C static CContent* NewLC(const TDesC8& aHeaderData);
		
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		/** destructor */
		virtual ~CContent();

 		/** Open a Container object within the file.
 		
 		Allows an application to access the content objects and 
 		any nested container objects within the specified container.
  		
 		@param aUniqueId The container object's unique ID.
		@return Whether the container was opened.
 		@return KErrNone if the container was opened successfully.
 		@return KErrNotFound if the container does not exist.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
 		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
 		*/
 		IMPORT_C TInt OpenContainer(const TDesC& aUniqueId);

 		/** Close the current container object and go back to previous enclosing
 		container within the file.
 		
 		Allows an application to access the content objects and 
 		any nested container objects within parent container.
  		
		@return Whether the container was closed.
  		@return KErrNone if the container was closed.
  		@return KErrNotFound if there is no enclosing container.
  		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 		
  		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
 		*/
		IMPORT_C TInt CloseContainer();

		/** List all the embedded container objects and content objects within the 
		current container
 		 		
		The UniqueId() member of CEmbeddedObject can be used to refer directly to 
		a particular object within the file.
		
		@code
		// Create a ccontent object for the file of interest
		CContent* content = CContent::NewLC(uri);
		
		// Define an array to store the results
		RStreamablePtrArray <CEmbeddedObject> array;
		CleanupClosePushL(array);
		
		// Find all the content and container objects within the current container
		content->GetEmbeddedObjectsL(array);
		
		// Do something with the array
		....
		
		// Finished
		CleanupStack::PopAndDestroy(2);
		@endcode
		
		@param aArray The array to be populated with the embedded objects. 
		@leave KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
  		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
 		*/
		IMPORT_C void GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray) const;

		/** List all the embedded objects within the current container that
		are of the specified type

		@code
		// Create a ccontent object for the file of interest
		CContent* content = CContent::NewLC(uri);
		
		// Define an array to store the results
		RStreamablePtrArray <CEmbeddedObject> array;
		CleanupClosePushL(array);
		
		// Find all the content objects within the current container
		content->GetEmbeddedObjectsL(array, EContentObject);
		
		// Do something with the array
		....
		
		// Finished
		CleanupStack::PopAndDestroy(2);
		@endcode
 		 		
		@param aArray The array to be populated with the embedded objects. 
		@param aType The type of objects to list.
		@leave KErrPermissionDenied If the access to the protected content is not permitted by the CAF Agent.		 
		@leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
  		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
 		*/
		IMPORT_C void GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType) const;
		
		/** Search the current container for content objects with a particular mime type

		@code
		_LIT8(KTextPlainMimeType, "text/plain");
		
		// Create a ccontent object for the file of interest
		CContent* content = CContent::NewLC(uri);
		
		// Define an array to store the results
		RStreamablePtrArray <CEmbeddedObject> array;
		CleanupClosePushL(array);
		
		// Recursively search for all the content objects with the 
		// specified mime type
		User::LeaveIfError(content->Search(array, KTextPlainMimeType(), ETrue));
		
		// Do something with the array
		....
		
		// Finished
		CleanupStack::PopAndDestroy(2);
		@endcode

  		@param aArray The array to store the results of the search. The agent will add CEmbeddedObject objects to the supplied array.
		@param aMimeType The mime type to search for.
		@param aRecursive ETrue to search inside containers embedded within the current container, EFalse to search only the current container.
		@return The result of the search.
		@return KErrNone if the search was successful, even if no content objects were found.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt Search(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive);


		/**  Get an attribute for the default content object within the file

  		@see ContentAccess::TAttribute

		@code
		TInt value = 0;
		CContent* c = CContent::NewL(uri);
		TInt err =c->GetAttribute(EIsProtected, value);
		if(err == KErrNone && value)
			{
			DisplayPadLock();
			}
		@endcode

		@param aAttribute The attribute to query, from ContentAccess::TAttribute.
		@param aValue Used to return the attribute value.
		@return Whether the attribute value was updated.
		@return KErrNone if the value of the attribute was updated.
		@return KErrNotFound if the object with the given UniqueId was not found.
		@return KErrCANotSupported if the requested attribute does not exist.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.	 
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttribute(TInt aAttribute, TInt& aValue) const;		

		/**  Get an attribute for an object within the file

		@see ContentAccess::TAttribute

		@code
		TInt value = 0;
		CContent* c = CContent::NewL(uri);
		TInt err =c->GetAttribute(EIsProtected, value, uniqueId);
		if(err == KErrNone && value)
			{
			DisplayPadLock();
			}
		@endcode

		@param aAttribute The attribute to query, from ContentAccess::TAttribute.
		@param aValue Used to return the attribute value.
		@param aUniqueId The UniqueId of the object within the file.
		@return Whether the attribute value was updated.
		@return KErrNone if the value of the attribute was updated.
		@return KErrNotFound if the object with the given UniqueId was not found.
		@return KErrCANotSupported if the requested attribute does not exist.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.	 
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId) const;
		

		/** Get a set of attributes for the default content object within the file

		@see ContentAccess::TAttribute

		The following example determines whether the default content object 
		is protected and has rights that will enable it to be viewed by the 
		user

		@code
		CContent* content = CContent::NewLC(uri);
				
		RAttributeSet attributeSet;
		CleanupClosePushL(attributeSet);
		attributeSet->AddL(EProtected);
		attributeSet->AddL(ECanView);

		User::LeaveIfError(content->GetAttributeSet(attributeSet);

		TInt err = attributeSet.GetValue(EProtected, value);
		if(err == KErrNone && value)
				{
				// file is DRM protected
				}

		err = attributeSet.GetValue(ECanView, value);
		if(err == KErrNone && value)
				{
				// File has rights that allow it to be displayed on screen
				}
		
		// Finished
		CleanupStack::PopAndDestroy(2); // content, attributeSet.Close()
		@endcode


		@param aAttributeSet The set of attributes to query and update.
		@return Whether the attribute set was updated.
		@return KErrNone if the attributes were retrieved successfully.
		@return KErrNotFound if the default content object was not found.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttributeSet(RAttributeSet& aAttributeSet) const;		
		
		/** Get a set of attributes from an object within the file

		@see ContentAccess::TAttribute

		The following example determines whether the default content object 
		is protected and has rights that will enable it to be viewed by the 
		user

		@code
		CContent* content = CContent::NewLC(uri);
				
		RAttributeSet attributeSet;
		CleanupClosePushL(attributeSet);
		attributeSet->AddL(EProtected);
		attributeSet->AddL(ECanView);

		User::LeaveIfError(content->GetAttributeSet(attributeSet, uniqueId);

		TInt err = attributeSet.GetValue(EProtected, value);
		if(err == KErrNone && value)
				{
				// file is DRM protected
				}

		err = attributeSet.GetValue(ECanView, value);
		if(err == KErrNone && value)
				{
				// File has rights that allow it to be displayed on screen
				}
		
		// Finished
		CleanupStack::PopAndDestroy(2); // content, attributeSet.Close()
		@endcode


		@param aAttributeSet The set of attributes to query and update.
		@param aUniqueId The unique ID of the object within the file.
		@return Whether that attribute set was updated.
		@return KErrNone if the attributes were retrieved successfully.
		@return KErrNotFound if the default content object was not found.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId) const;
		
		/**  Get text string attributes or meta-data for the default content object within the file
	
		@see ContentAccess::TStringAttribute

		@code
		TInt err = KErrNone;
		
		CContent* c = CContent::NewL(uri);
		TBuf <MAX_PATH> previewUri;
		err = c->GetStringAttribute(EPreviewURI, previewUri);
		if(err == KErrNone)
			{
			DisplayPreview(previewUri);
			}
		@endcode

		@param aAttribute The attribute to query, from ContentAccess::TStringAttribute.
		@param aValue Returns the value of the attribute.
		@return Whether the attribute value was updated.
		@return KErrNone if the attribute was retrieved.
		@return KErrNotFound if the default content object was not found.
		@return KErrOverflow if the buffer was not large enough to return the result.
		@return KErrCANotSupported if the requested attribute does not exist.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetStringAttribute(TInt aAttribute, TDes& aValue) const;
		
		/** Get text string attributes or meta-data for an object within the file

		@see ContentAccess::TStringAttribute

		@code
		TInt err = KErrNone;
		
		CContent* c = CContent::NewL(uri);
		TBuf <MAX_PATH> previewUri;
		err = c->GetStringAttribute(EPreviewURI, previewUri, uniqueId);
		if(err == KErrNone)
			{
			DisplayPreview(previewUri);
			}
		@endcode

		@param aAttribute The attribute to query, from ContentAccess::TStringAttribute.
		@param aValue Returns the value of the attribute.
		@param aUniqueId The UniqueId of the object within the file. 
		@return Whether the attribute value was updated.
		@return KErrNone if the attribute was retrieved.
		@return KErrNotFound if the object with the given UniqueId was not found.
		@return KErrOverflow if the buffer was not large enough to return the result.
		@return KErrCANotSupported if the requested attribute does not exist.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.	 
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId) const;
		
		/** Obtain a set of string attributes for the default content object within the file

		@see ContentAccess::TStringAttribute
	  
		@code
		CContent* content = CContent::NewLC(uri);
		
		// create the attribute set, add the attributes we are interested in
		RStringAttributeSet stringAttributeSet;
		CleanupClosePushL(stringAttributeSet);
		stringAttributeSet.AddL(EPreviewURI);
		stringAttributeSet.AddL(ETitle);
		
		User::LeaveIfError(content->GetStringAttributeSet(stringAttributeSet));
	
		// Pass the value of the string attribute to DisplayPreview()
		TFileName previewUri;		
		TInt err = stringAttributeSet.GetValue(EPreviewURI, previewUri);
		if(err == KErrNone)
			{
			DisplayPreview(previewUri);
			}
		
		CleanupStack::PopAndDestroy(2); // content, stringAttributeSet.Close()
		@endcode
	

		@param aStringAttributeSet The set of attributes to query and update.
		@return Whether the attribute set was updated.
		@return KErrNone if the attributes were retrieved successfully.
		@return KErrNotFound if the default content object was not found.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet) const;
	
		/** Used to obtain a set of string attributes for an object within the file

		@see ContentAccess::TStringAttribute
	
		@code
		CContent* content = CContent::NewLC(uri);
		
		// create the attribute set, add the attributes we are interested in
		RStringAttributeSet stringAttributeSet;
		CleanupClosePushL(stringAttributeSet);
		stringAttributeSet.AddL(EPreviewURI);
		stringAttributeSet.AddL(ETitle);
		
		User::LeaveIfError(content->GetStringAttributeSet(stringAttributeSet, uniqueId));
	
		// Pass the value of the string attribute to DisplayPreview()
		TFileName previewUri;		
		TInt err = stringAttributeSet.GetValue(EPreviewURI, previewUri);
		if(err == KErrNone)
			{
			DisplayPreview(previewUri);
			}
		
		CleanupStack::PopAndDestroy(2); // content, stringAttributeSet.Close()
		@endcode
	

		@param aStringAttributeSet The set of attributes to query and update.
		@param aUniqueId The UniqueId of the container or content. 
		@return Whether the attribute set was updated.
		@return KErrNone if the attributes were retrieved successfully.
		@return KErrNotFound if the object with the given UniqueId was not found.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId) const;

		/** Allows extended synchronous calls to the CAF agent handling this file
		
		Applications familiar with the agent can pass objects in and out using serialization.

		@param aCommand The agent defined command.
		@param aInputBuffer Non modifyable input data buffer.
		@param aOutputBuffer Modifyable output buffer to hold the result of the command.
		@return The outcome of the agent specific command.
		@return KErrCANotSupported if the agent does not recognize the command.
		@return KErrOverflow if the output buffer supplied is too small.
		@return KErrPermissionDenied if the agent does not allow the client to execute the command.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);

		/** Allows extended synchronous calls to the CAF agent handling this file
		Applications familiar with the agent can pass objects in and out using serialization.
		NB: It is important that the descriptor passed to 
		aOutputBuffer remains in scope until the request has completed.		

		@param aCommand The agent defined command.
		@param aInputBuffer Non modifyable input data buffer.
		@param aOutputBuffer Modifyable output buffer to hold the result of the command.
		@param aStatus Asynchronous request status. On completion this will contain 
						one of the following error codes:
						KErrNone if the command was successfully executed.
						KErrCANotSupported  if the agent does not recognize the command.
						KErrOverflow if the output buffer supplied is too small.
						KErrPermissionDenied if the agent does not allow the client 
						to execute the command. Otherwise one of the other CAF error codes defined 
						in \c caferr.h  or one of the other system-wide error codes 
						for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);


		/** Request notification for an event for the default content object within the file

		@see ContentAccess::TEventMask

		@param aMask Bitmask of events the caller is interested in.	
		@param aStatus The TRequestStatus object to complete if the event occurs.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C void NotifyStatusChange(TEventMask aMask, TRequestStatus& aStatus);

		/** Request notification for an event for an object within the file

		@see ContentAccess::TEventMask

		@param aMask Bitmask of events the caller is interested in.
		@param aStatus The TRequestStatus object to complete if the event occurs.
		@param aUniqueId The UniqueId of the container or content. 						 
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C void NotifyStatusChange(TEventMask aMask, TRequestStatus& aStatus, const TDesC& aUniqueId);

		/** Cancel a previous notification request for the default content object

		@param aStatus The TRequestStatus supplied in the call to NotifyStatusChange().
		@return The outcome of the cancel request.
		@return KErrNone if the request was cancelled.
		@return KErrNotFound if there was no matching request outstanding.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt CancelNotifyStatusChange(TRequestStatus& aStatus);

		/** Cancel a previous notification request

		@param aStatus The TRequestStatus supplied in the call to NotifyStatusChange().
		@param aUniqueId The UniqueId of the content object within the file.
		@return The outcome of the cancel request.
		@return KErrNone if the request was cancelled.
		@return KErrNotFound if there was no matching request outstanding.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt CancelNotifyStatusChange(TRequestStatus& aStatus, const TDesC& aUniqueId);
		
		/** Request the agent to obtain rights for the default content object

		This request may be handled differently by different agents. 
		Some agents may open a browser and direct the user to a URL. Others may 
		download and install the rights in the background.

		The call should not block execution, applications can wait for a notification 
		if they are interested in the outcome.

		@param aStatus Asynchronous request status. On completion this will contain 
						one of the following error codes:
						KErrNone if the rights request was successful.
						KErrCANotSupported  if the agent does not allow rights requests.
						Otherwise one of the other CAF error codes defined in \c caferr.h  
						or one of the other system-wide error codes 
						for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C void RequestRights(TRequestStatus& aStatus);

		/** Request the agent to obtain rights for a protected content object

		This request may be handled differently by different agents. 
		Some agents may open a browser and direct the user to a URL. Others may 
		download and install the rights in the background.

		The call should not block execution, applications can wait for a notification 
		if they are interested in the outcome.

		@param aStatus Asynchronous request status. On completion this will contain 
						one of the following error codes:
						KErrNone if the rights request was successful.
						KErrCANotSupported  if the agent does not allow rights requests.
						Otherwise one of the other CAF error codes defined in \c caferr.h  
						or one of the other system-wide error codes 
						for any other errors.

		@param aUniqueId The unique id of the object within the file.					
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C void RequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId);


		/** Cancel a previous RequestRights() request for the default content object

		@param aStatus The TRequestStatus that was supplied to the RequestRights() function.
		@return The result of the cancel rights request.
		@return KErrNone if the rights request was cancelled.
		@return KErrNotFound if there was no matching rights request outstanding.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt CancelRequestRights(TRequestStatus& aStatus);

		/** Cancel a previous RequestRights() request

		@param aStatus The TRequestStatus that was supplied to the RequestRights() function.
		@param aUniqueId The objects Unique Id that was supplied to the RequestRights() function.
		@return The result of the cancel rights request.
		@return KErrNone if the rights request was cancelled.
		@return KErrNotFound if there was no matching rights request outstanding.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt CancelRequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId);

		/** View information associated with the default content object
	
		This call blocks execution and only returns once the display is dismissed 
		by the user.

		@see ContentAccess::TDisplayInfo

		@param aInfo The information to display.
		@leave KErrCANotSupported if information cannot be displayed or does not exist.
		@leave ...			One of the other CAF error codes defined in \c caferr.h  
							or one of the system-wide error codes for 
							any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C void DisplayInfoL(TDisplayInfo aInfo) const;
		
		/** View information associated with a single content object
		
		This call blocks execution and only returns once the display is dismissed 
		by the user.

		@see ContentAccess::TDisplayInfo

		@param aInfo The information to display.
		@param aUniqueId The unique id of the object within the file.
		@leave KErrCANotSupported if information cannot be displayed or does not exist.
		@leave ...			One of the other CAF error codes defined in \c caferr.h  
							or one of the system-wide error codes for 
							any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C void DisplayInfoL(TDisplayInfo aInfo, const TDesC& aUniqueId) const;


		/** Request the agent handling this content to set a property value. If 
		the property is set it is only set for this CContent session and does not 
		impact other CAF users and is not set for CData products created by the 
		CContent.

  		@see ContentAccess::TAgentProperty

		@param aProperty The property to set.
		@param aValue The value of the property.
		@return Whether the property was set.
		@return KErrNone if the property was set.
		@return KErrCANotSupported if the agent does not support the property or value.
		@return KErrAccessDenied if the agent does not permit the property to be changed.
		@return KErrPermissionDenied if the application does not have the necessary capability to change the property.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt SetProperty(TAgentProperty aProperty, TInt aValue);
		
		/** 
		 Grants read access to the default content object based on 
		 evaluation of the supplied intent information. 
		 
		 Note that the intent is simply evaluated and not executed. 
		 To force the intent to be executed, clients must use the call
		 ContentAccess::CData::ExecuteIntent(). 
		 
		 A CData object can be obtained as follows:
		 @code
		 CData* MyData = myContent->OpenContentL(ChosenIntent);
		 ...
		 delete MyData; // when finished accessing plaintext content
		 @endcode

		 @see ContentAccess::TIntent

		 @param aIntent	The intent indicator.
		 @return A CData instance. The caller gets ownership of this object.
		 
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...				One of the other CAF error codes defined in \c caferr.h  
		 						or one of the other system-wide error codes for any other 
								errors.		 
 		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted
		 */
		IMPORT_C CData* OpenContentL(TIntent aIntent);

		/** 
		 Grants read access to the default content object based on 
		 evaluation of the supplied intent information. 
	
		 Note that the intent is simply evaluated and not executed. 
		 To force the intent to be executed, clients must use the call
		 ContentAccess::CData::ExecuteIntent(). 
		 
		 A CData object can be obtained as follows:
		 @code
		 CData* myData = myContent->OpenContentLC(ChosenIntent);
		 ...
		 // when finished accessing plaintext content
		 CleanupStack::PopAndDestroy(myData); 
		 @endcode

		 @see ContentAccess::TIntent

		 @param aIntent	The intent indicator.
		 @return A CData instance. The caller gets ownership of this object.
		 
		 @leave KErrNotFound			The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights		The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission		Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights			No rights exist for the content object.
		 @leave KErrCANotSupported		Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied	The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied		The content is already in use.
		 @leave ...				One of the other CAF error codes defined in \c caferr.h  
		 						or one of the other system-wide error codes for any other 
								errors.		 
 		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted.
		 */
		IMPORT_C CData* OpenContentLC(TIntent aIntent);
		
		/** 
		 Grants read access to the content object based on evaluation of the
		 supplied intent information. 
		 
		 Note that the intent is simply evaluated and not executed. 
		 To force the intent to be executed, clients must use the call
		 ContentAccess::CData::ExecuteIntent(). 
		 
		 A CData object can be obtained as follows:
		 @code
		 CData* MyData = myContent->OpenContentL(ChosenIntent, uniqueId);
		 ...
		 delete MyData; // when finished accessing plaintext content
		 @endcode
		 	
		 @see ContentAccess::TIntent

		 @param aIntent	The intent indicator.
		 @param aUniqueId The object to open for reading. If the UniqueId is set to KNullDesC16() the entire file will be opened for reading with no transformation.
		 @return A CData instance. The caller gets ownership of this object.
		 
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...				One of the other CAF error codes defined in \c caferr.h  
		 						or one of the other system-wide error codes for any other 
								errors.		 
 		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted
		 */
		IMPORT_C CData* OpenContentL(TIntent aIntent, const TDesC& aUniqueId);


		/** 
		 Grants read access to the content object based on evaluation of the
		 supplied intent information. 
		 
		 Note that the intent is simply evaluated and not executed. 
		 To force the intent to be executed, clients must use the call
		 ContentAccess::CData::ExecuteIntent(). 
		 
		 A CData object can be obtained as follows:
		 @code
		 CData* MyData = myContent->OpenContentLC(ChosenIntent, uniqueId);
		 ...
		 PopAndDestroy(MyData); // when finished accessing plaintext content
		 @endcode
		 	
		 @see ContentAccess::TIntent

		 @param aIntent	The intent indicator.
		 @param aUniqueId The object to open for reading. If the UniqueId is set to KNullDesC16() the entire file will be opened for reading with no transformation.
		 @return A CData instance. The caller gets ownership of this object.
		 
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...				One of the other CAF error codes defined in \c caferr.h  
		 						or one of the other system-wide error codes for any other 
								errors.		 
 		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted
		 */
		IMPORT_C CData* OpenContentLC(TIntent aIntent, const TDesC& aUniqueId);


		/** Find out which agent is handling this file
		@return The agent handling the File
		*/
		IMPORT_C const TAgent& Agent() const;
		
		/** Indicates the mode in which the content is shared.
			@return The content sharing mode
		*/
		inline TContentShareMode ShareMode() const;
		
#ifndef REMOVE_CAF1
		/** Create a CData object for reading the content 

		@see ContentAccess::TIntent
		@param aIntent The intended use of the content
		@param aShareMode The mode used to open the file. If EContentShareExclusive is required, use CData::NewL() instead
		@return A new CData object
		@leave One of the CAF error codes defined in \c caferr.h  or one of the system-wide error codes for any errors.
		@capability DRM Access to DRM agents is not permitted for processes without DRM capability
		@deprecated
		*/
		IMPORT_C CData* OpenContentL(TIntent aIntent, TContentShareMode aShareMode);

		/** Create a CAttribute object to determine the attributes of the default content object
		
		@return The agent handling the File
		@capability DRM Access to DRM agents is not permitted for processes without DRM capability
		@deprecated 
		*/
		IMPORT_C CAttribute* NewAttributeL(TBool aPreloaded);

		/** Create a CAttribute object to determine the attributes of the default content object
		
		@return The agent handling the File
		@capability DRM Access to DRM agents is not permitted for processes without DRM capability
		@deprecated 
		*/
		IMPORT_C CAttribute* NewAttributeL(TBool aPreloaded, TContentShareMode aShareMode);

#endif  // REMOVE_CAF1
		
	private:
		CContent(); 
		void ConstructL(RFile& aFile); 
		void ConstructL(const TDesC& aURI, TContentShareMode aShareMode); 
		
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		void ConstructL(const TDesC8& aHeaderData);

		/**
		Create a CData object for reading WMDRM content.
		
		@param	aIntent		The intended use of the content.
		@param	aHeaderData	Header data of WMDRM file/stream content.
		@return				CData object.
		@leave 				One of the CAF error codes defined in caferr.h  or one of the system-wide error codes for any errors.
		@capability DRM 	Access to DRM agents is not permitted for processes without DRM capability.
		*/
		
		CData* OpenContentL(TIntent aIntent, const TDesC8& aHeaderData);
		
		/**
		Create a CData object for reading WMDRM content.
		
		@param	aIntent		The intended use of the content.
		@param	aHeaderData	Header data of WMDRM file/stream content.
		@return				CData object.
		@leave 				One of the CAF error codes defined in caferr.h  or one of the system-wide error codes for any errors.
		@capability DRM 	Access to DRM agents is not permitted for processes without DRM capability.
		*/
		CData* OpenContentLC(TIntent aIntent, const TDesC8& aHeaderData);
		
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

	private:	
	
		/** CAgentFactory object is effectively the	ECOM session handle */
		CAgentFactory* iAgentFactory;

		/** 
		 Agent object used for browsing the contents of a file
		 */
		CAgentContent* iAgentContent;

		/** Points to the default content object if the file is opened by name */
		CVirtualPath* iVirtualPath;

		/** The content object referred to by CContent, either points to iVirtualPath
		or points to (KNullDesC(), KDefaultContentObject()) 
		*/
		TVirtualPathPtr iDefaultVirtualPath;

		/* Stores the name and UID of the agent
		*/
		TAgent iAgent;
		
		/** The share mode used to open the content file*/
		TContentShareMode iShareMode;

		/** Reference to the file handle if the content was opened with a file handle */
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		RFile64 iFile;
#else
		RFile iFile;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		HBufC8* iHeaderData;
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		};

#include <caf/content.inl>
	} // namespace ContentAccess

#endif // __CONTENT_H__ 

