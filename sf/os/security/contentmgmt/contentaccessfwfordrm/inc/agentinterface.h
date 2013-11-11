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

#ifndef __AGENTINTERFACE_H__
#define __AGENTINTERFACE_H__

#include <e32base.h>
#include <ecom/ecom.h>

#include <caf/caftypes.h>
#include <caf/streamableptrarray.h>
#include <caf/caferr.h>

class RStringPool;
class RHTTPHeaders;

namespace ContentAccess
{
	class CEmbeddedObject;
	class CSupplierOutputFile;
	class RAttributeSet;
	class RStringAttributeSet;
	class TVirtualPathPtr;
	class CRightsInfo;
	class CVirtualPath;
	class MAgentRightsBase;
	class CImportFile;
	class CMetaDataArray;
	class CDirStreamable;

	/**
	 Defines the agent interface for consumer operations such as 
	 reading and seeking within a content object in a file.

	 @publishedPartner
	 @released
     */
	class CAgentData : public CBase
		{
	public:
		/**
		 Gets the data size in bytes.
		 
		 @param aSize	On return this will contain the size of the plaintext data in bytes.
		 @leave KErrCASizeNotDetermined	Size could not be determined by the managing agent.
		 @leave ...		One of the other CAF error codes defined in \c caferr.h  or one of the 
						system-wide error codes for any other errors.		 
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted
		 */
		virtual void DataSizeL(TInt &aSize) = 0;
		
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		/**
		 This is the 64bit version of DataSizeL. Implementing this function should take advantage of 64bit
		 file system and use RFile::Size64(TInt64 &). If this function is not implemented, fallback to 32bit 
		 DataSizeL will be provided by CAF automatically.
		 
		 @see DataSizeL(TInt &aSize)
		*/
		IMPORT_C virtual void DataSize64L(TInt64 &aSize);
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		
		/**
		 Allows a client to verify that the intent is supported by the
		 current rights for the data object. 
		 
		 Doing so may cause subsequent reads (or other accesses)
		 to fail. The result of the agent's evaluation
		 of the intent is returned.
		 
		 @note Since the intent is only evaluated the agent will not decrement 
		 any rights-state such as play counts.
		  
		 @param aIntent	The intended way the content will be used.
		 @return Whether rights exist allowing the content to be used for this intent.
		 @return KErrNone if the intent is permitted.
		 @return KErrNoRights if no rights exist for the specified content object.
		 @return KErrCANoPermission	if rights exist but the specified intent is not permitted.
		 @return KErrCAPendingRights if the rights have not yet arrived but are expected soon.
		 @return KErrNoPermission if rights exist but none permit the specified intent.
		 @return KErrPermissionDenied if the client is not allowed to use this content object.
		 @return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.

		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted
		 */
		virtual TInt EvaluateIntent(TIntent aIntent) = 0;
		
		/**
		 Execute intent allows the application to signal it is about to carry out
		 the specified intent on DRM protected content.The agent will decrement applicable 
		 stateful rights such as play counts where necessary.

  		 Applications should be very careful to use this function correctly.
		 @li If ExecuteIntent() is never called stateful rights will never be decremented. Users will be able to use the content forever
		 @li If ExecuteIntent() is called too often stateful rights may be consumed prematurely. 

		 Usually it would be best to call ExecuteIntent immediately before the content 
		 is displayed or the instant playback begins. ExecuteIntent should not be considered a 
		 prerequisite for calling the Read functions. It is valid for an application to read from 
		 the file before calling ExecuteIntent. 

		 @note Since rights are executed here the agent will decrement any rights-state such as 
		 play counts that apply. 
		 
		@param aIntent	The intent indicator.
		@return The result of attempting executing the intent.
		@return KErrNone if the intent is permitted and stateful rights may have been decremented.
		@return KErrNoRights if no rights exist for the specified content object.
		@return KErrCANoPermission	if rights exist but the specified intent is not permitted.
		@return KErrCAPendingRights if the rights have not yet arrived but are expected soon.
		@return KErrNoPermission if rights exist but none permit the specified intent.
		@return KErrPermissionDenied if the client is not allowed to use this content object.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt ExecuteIntent(TIntent aIntent) = 0;

		/** 
		 Reads from a content object up to the maximum length of the
		 descriptor or the end of the content object.
		 	
		 When an attempt is made to read beyond the end of the content,
		 no error is returned. The descriptor’s length is set to the
		 number of bytes that were read into it. Therefore, when reading
		 through content, the end has been reached when the
		 descriptor length (given by TDesC::Length()) is zero.
		 
		 @param aDes	Descriptor into which binary data is read. Any existing
		  				contents are overwritten. On return, its length is set
		 				to the number of bytes read.
		 @return		KErrNone if successful, otherwise another of the
		  				system-wide error codes.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt Read(TDes8& aDes) = 0;

		/**
		 Reads a specified number of bytes of content.
		  
		 @see Read(TDes8& aDes)
		 		 
		 @param aDes	Descriptor into which binary data is read. Any existing
		  				contents are overwritten. On return, its length is set
		 				to the number of bytes read.
		 @param aLength		The number of bytes to read from the file,
		 					or to the end of the file, whichever is encountered first. 
		 					The length of the buffer is set to the number of bytes actually read.
		 @return		KErrNone if successful.
		 @return		Otherwise one of the CAF error codes defined in \c caferr.h  or 
		 				one of the other system-wide error codes.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt Read(TDes8& aDes, TInt aLength) = 0;

		/**
		 Reads content asynchronously.
		 
		 @see Read(TDes8& aDes)
		 
		 @param aDes 		Descriptor into which binary data is read. Any
		  					existing contents are overwritten. On return,
		 					its length is set to the number of bytes read. 
		 @param aStatus		Asynchronous request status. On completion this will contain one 
		 					of the following error codes: KErrNone if the data was 
							successfully read. Otherwise one of the CAF error codes defined in 
							\c caferr.h  or one of the other standard system-wide
							error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual void Read(TDes8& aDes, TRequestStatus& aStatus) = 0;

		/**
		 Reads content asynchronously (up to a specified maximum number of bytes)
		 
		 @see Read(TDes8& aDes)
		 
		 @param aDes 		Descriptor into which binary data is read. Any
		  					existing contents are overwritten. On return,
		 					its length is set to the number of bytes read. 
		 @param aLength		The number of bytes to read from the file,
		 					or to the end of the file, whichever is encountered first. 
		 					The length of the buffer is set to the number of bytes actually read.
		 @param aStatus		Asynchronous request status. On completion this will contain one 
		 					of the following error codes: KErrNone if the data was 
							successfully read. Otherwise one of the CAF error codes defined in 
							\c caferr.h  or one of the other standard system-wide
							error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus) = 0;
		
		/**
		 Cancels asynchronous read.
		 
		 @param aStatus		Asynchronous request status. This parameter should have been supplied earlier to
		 					an asynchronous Read call. If it has not been previously supplied to a Read call,
		 					this function will not have any effect.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C virtual void ReadCancel(TRequestStatus& aStatus);

		/**
		 Changes or retrieves the location of the file pointer within 
		 the content object
		 
		 There are four seek modes used: 
		 
		 @li ESeekStart - Sets the file pointer aPos bytes from the start of the 
		 content object.  aPos is not modified by the call to Seek()
		 @li ESeekEnd - Sets the file pointer aPos bytes from the end of the 
		 content object. The aPos parameter supplied should be zero or negative 
		 when using ESeekEnd. Upon completion aPos is updated with the current 
		 position relative to the start of the content object.
		 @li ESeekCurrent - Moves the file pointer aPos bytes from the current 
		 position. Upon completion aPos is updated with the new position relative 
		 to the start of the content object. Suppling a zero value for the aPos Parameter can be
		 used to retrieve the current file pointer location 
		 @li ESeekAddress Sets the aPos parameter to the address of the byte 
		 aPos bytes from the start of the content object

		 @see TSeek
		 
		 @param aMode	Seek mode - controls the destination of the
		  				seek operation.
		  
		 @param aPos	Offset from either the start, end or current position depending upon 
		 the seek mode. Negative offsets are used to seek before the current position or
		 relative to the end of the file.
		 
		 @return		KErrNone if successful. 
		 @return		Otherwise one of the CAF error codes defined in \c caferr.h  or one 
		 				of the other system-wide error codes.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt Seek(TSeek aMode, TInt& aPos) = 0;

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		/**
		  This is the 64bit version of Seek. Implementing this function should take advantage of 64bit
		  file system and use RFile::Seek64(TSeek, TInt64 &). If this function is not implemented, fallback to 32bit 
		  Seek will be provided by CAF automatically.
		  
		  @see Seek(TSeek aMode, TInt& aPos)
		*/
		IMPORT_C virtual TInt Seek64(TSeek aMode, TInt64& aPos);
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		
		/** 
		Allows an application to request the modification of a property 
		within the agent handling this content. The agent may or may not
		permit the property to be changed
		 	
		@param aProperty The property to set.
		@param aValue The value of the property.
		@return Whether or not the property was set.
		@return KErrNone if the property was set.
		@return KErrCANotSupported if the agent does not support the property or value.
		@return KErrAccessDenied if the agent does not permit the property to be changed.
		@return KErrPermissionDenied if the application does not have the necessary capability to change the property.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		 */
		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue) = 0;

		/**  Get the value of an attribute for this content object
 
		@param aAttribute The attribute to query, from ContentAccess::TAttribute.
		@param aValue Used to return the attribute value.
		@return Whether the attribute was updated.
		@return KErrNone if the value of the attribute was updated.
		@return KErrCANotSupported if the requested attribute does not exist.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue)=0;

		/** Get a set of attributes from a content object

		@param aAttributeSet The set of attributes to query and update.
		@return Whether the attribute set was updated.
		@return KErrNone if the attributes were retrieved successfully.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 				
		*/
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet) = 0;

		/**  Get text string attributes or meta-data for this content object

		@param aAttribute The attribute to query, from ContentAccess::TStringAttribute.
		@param aValue Returns the value of the attribute.
		@return Whether the value was updated.
		@return KErrNone if the attribute was retrieved.
		@return KErrOverflow if the buffer was not large enough to return the result.
		@return KErrCANotSupported if the requested attribute does not exist.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.				 
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue)=0;

	
		/** Used to obtain a set of string attributes 

		@param aStringAttributeSet The set of attributes to query and update.
		@return Whether the string attribute set was updated.
		@return KErrNone if the attributes were retrieved successfully.
		@return KErrNotFound if the default content object was not found.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.				 
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet)=0;
		
		/**  
		 Reads content asynchronously. The data is read from a specified offset 
		 up to a specified number of bytes or until the end of the content object 
		 is reached. The data is read into the descriptor buffer supplied.
		 
		 @see Read(TDes8& aDes)
		 
		 @param aPos		Position of first byte to be read. 
		 					This is an offset from the start of the file. 
		 @param aDes 		Descriptor into which binary data is read. Any
		  					existing contents are overwritten. On return,
		 					its length is set to the number of bytes read. 
		 @param aLength		The number of bytes to read from the file,
		 					or to the end of the file, whichever is encountered first. 
		 					The length of the buffer is set to the number of bytes actually read.
		 @param aStatus		Asynchronous request status. On completion this will contain one 
		 					of the following error codes: KErrNone if the data was 
							successfully read. Otherwise one of the CAF error codes defined in 
							\c caferr.h  or one of the other standard system-wide
							error codes for any other errors.
		 @return KErrNone if the async read request was successfully submitted.
		 @return KErrCANotSupported if the agent does not support this operation.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C virtual TInt Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
		
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		/**  
		 This is the 64bit version of CAgentData::Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
		 Implementing this function should take advantage of 64bit file system and use RFile::Seek64(TSeek, TInt64 &) to get to
		 the position in file specified by aPos. If this function is not implemented, fallback to 32bit 
		 Read will be provided by CAF automatically. 
		 
		 @see Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
		*/
		IMPORT_C virtual TInt Read64(TInt64 aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		/**
		Decrypts the encrypted input data packet.Default implementation of this method returns KErrCANotSupported.
		
		@param aEncryptedInputDataPacket	Buffer descriptor containing the encrypted data packet supplied by client application. 
		@param aDecryptedOutputDataPacket	Buffer descriptor supplied by the client application into which the decrypted data is written.
											The length of this descriptor must be equal to or greater than the input packet.
		@return								KErrNone if successful or KErrInsuffcientDataPacketLength, if a part of input data packet
											is provided for decryption.Otherwise one of the CAF error codes defined in \c caferr.h  or 
		 									one of the other system-wide error.
		@capability 						DRM Access to DRM protected content is not permitted for processes without DRM capability.
											Access to unprotected content is unrestricted.
		*/
		IMPORT_C virtual TInt Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket);
		
		
		/**
		Decrypts the encrypted input data packet asynchronously.Default implementation of this method returns KErrCANotSupported.
	
		@param aEncryptedInputDataPacket	Buffer descriptor containing the encrypted data packet supplied by client application. 
		@param aDecryptedOutputDataPacket	Buffer descriptor supplied by the client application into which the decrypted data is written.
											The length of this descriptor must be equal to or greater than the input packet.
		@param aStatus						Asynchronous request status. On completion this will contain one of the following error codes:
											KErrNone if the data packet was successfully decrypted or KErrInsuffcientDataPacketLength, if a part of input data packet
											is provided for decryption.Otherwise one of the CAF error codes defined in \c caferr.h  or 
		 									one of the other system-wide error codes.
		@capability 						DRM Access to DRM protected content is not permitted for processes without DRM capability.
										    Access to unprotected content is unrestricted. 
		*/
		IMPORT_C virtual void Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket, TRequestStatus& aStatus);
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		};

	/**	Defines the agent interface allowing clients to browse the objects 
	within a file

	@publishedPartner
	@released
	*/
	class CAgentContent : public CBase
		{
	public:
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
		virtual TInt OpenContainer(const TDesC& aUniqueId) = 0;

 		/** Close the current container object and go back to previous enclosing
 		container within the file.
 		
 		Allows an application to access the content objects and 
 		any nested container objects within parent container
  		
		@return Whether the container was closed.
  		@return KErrNone if the container was closed.
  		@return KErrNotFound if there is no enclosing container.
  		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 		
  		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
 		*/
		virtual  TInt CloseContainer() = 0;

		/** List all the embedded container objects and content objects within the 
		current container
 		 		
		The UniqueId() member of CEmbeddedObject can be used to refer directly to 
		a particular object within the file.

		@param aArray The array to be populated with the embedded objects. 
		@leave KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.		 
		@leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
  		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
 		*/
		virtual void GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray) = 0;
		
		/** List all the embedded objects within the current container that
		are of the specified type
 		 		
		@param aArray The array to be populated with the embedded objects. 
		@param aType The type of objects to list.
		@leave KErrPermissionDenied If the access to the protected content is not permitted by the CAF Agent.		 
		@leave ...			One of the CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
  		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
 		*/
		virtual void GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType) = 0;

		/** Search the current container for content objects with a particular mime type

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
		virtual TInt Search(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive) = 0;
		
		/**  Get an attribute for an object within the file
	
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
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId) = 0;

		/** Get a set of attributes for an object within the file

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
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId) = 0;

		/**  Get text string attributes or meta-data for an object within the file

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
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId) = 0;

		/** Used to obtain a set of string attributes for an object within the file

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
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId)=0;

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
		virtual TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer) = 0;

		/** Allows extended synchronous calls to the CAF agent handling this file.
		Applications familiar with the agent can pass objects in and out using serialization.
		NB: No assumption should be made about the scope of the descriptor 
		passed to aInputBuffer for asynchronous IPC.

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
		virtual void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus) = 0;

		/** Request notification for an event for an object within the file.
		NB: No assumption should be made about the scope of the descriptor 
		passed to aUniqueId for asynchronous IPC.

		@param aMask Bitmask of events the caller is interested in.
		@param aStatus The TRequestStatus object to complete if the event occurs.
		@param aUniqueId The UniqueId of the container or content. 
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual void NotifyStatusChange(TEventMask aMask, TRequestStatus& aStatus, const TDesC& aUniqueId) = 0;
		
		/** Cancel a previous notification request

		@param aStatus The TRequestStatus supplied in the call to NotifyStatusChange().
		@param aUniqueId The UniqueId of the content object within the file.
		@return The outcome of the cancel request.
		@return KErrNone if the request was cancelled.
		@return KErrNotFound if there was no matching request outstanding.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt CancelNotifyStatusChange(TRequestStatus& aStatus, const TDesC& aUniqueId) = 0;

		/** Request the agent to obtain rights for a protected content object.
		NB: No assumption should be made about the scope of the descriptor 
		passed to aUniqueId for asynchronous IPC.

		This request may be handled differently by different agents. 
		Some agents may open a browser and direct the user to a URL. Others may 
		download and install the rights in the background.

		The call should not block execution, applications can wait for a notification 
		if they are interested in the outcome. 
		
		If a rights request completes sucessfully it does not necessarily mean rights 
		are available. It just means the request is complete.

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
		virtual void RequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId) = 0; 

		/** Cancel a previous RequestRights() request

		@param aStatus The TRequestStatus that was supplied to the RequestRights() function.
		@param aUniqueId The objects Unique Id that was supplied to the RequestRights() function.
		@return The result of the cancel rights request.
		@return KErrNone if the rights request was cancelled.
		@return KErrNotFound if there was no matching rights request outstanding.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt CancelRequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId) = 0;

		/** View information associated with a single content object
		
		This call blocks execution and only returns once the display is dismissed 
		by the user.

		@param aInfo The information to display.
		@param aUniqueId The unique id of the object within the file.
		@leave KErrCANotSupported if information cannot be displayed or does not exist.
		@leave ...			One of the other CAF error codes defined in \c caferr.h  
							or one of the system-wide error codes for 
							any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		virtual void DisplayInfoL(TDisplayInfo aInfo, const TDesC& aUniqueId) = 0;

		/** 
		Allows an application to request the modification of a property 
		within the agent handling this content. The agent may or may not
		permit the property to be changed
		 	
		@param aProperty The property to set.
		@param aValue The value of the property.
		@return Whether the property was set.
		@return KErrNone if the property was set.
		@return KErrCANotSupported if the agent does not support the property or value.
		@return KErrAccessDenied if the agent does not permit the property to be changed.
		@return KErrPermissionDenied if the application does not have the necessary capability to change the property.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue) = 0;
		
		};

	/**
	 Defines the interface for importing files into a Content access agent. 

	The agent should treat the import as a transaction and either import the
	entire file or revert to the state before the import if the import is cancelled.
	
	The client cancels an import by deleting the CAgentImportFile object 
	before WriteDataComplete() is called. 
	 
	 @publishedPartner
	 @released
	 */
	class CAgentImportFile : public CBase
		{
	public:

		/** Writes a block of data from a file to the agent
		
		@param aData The block of data.
		@return	The result of the write operation.
		@return KErrNone if the data was written successfully.
		@return	KErrCANewFileHandleRequired if the agent requires the client to provide a new file handle before proceeding. The agent must save its state and continue processing when the new handle is supplied by ContinueWithNewOutputFile().
		@return KErrNotReady if the agent was not expecting WriteData() to be called at this point.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		*/
		virtual TInt WriteData(const TDesC8& aData)=0;

		/** Signals to the agent that the entire file has now been transferred to the agent.
		
		@return The result of the write operation. 
		@return KErrNone if the data was written successfully.
		@return	KErrCANewFileHandleRequired if the agent requires the client to provide a new file handle before proceeding. The agent must save its state and continue processing when the new handle is supplied by ContinueWithNewOutputFile().
		@return KErrNotReady if the agent was not expecting WriteDataComplete() to be called at this point.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.		
		*/
		virtual TInt WriteDataComplete()=0;

		/** Asynchronously writes a block of data from a file the agent
		 
		Notifies the client when the write operation is complete.
		
		@param aData 	The block of data.
		@param aStatus 	Asynchronous request status. On completion this will 
						contain one of the following error codes: KErrNone if the data was written successfully. 
						KErrCANewFileHandleRequired if the agent requires a new 
						file handle before proceeding - the agent must save its 
						state and continue processing when the new handle 
						is supplied by ContinueWithNewOutputFile().
						KErrNotReady if the agent was not expecting WriteDataComplete() 
						to be called at this point. Otherwise one of the other CAF error codes 
						defined in \c caferr.h  or one of the other standard system-wide 
						error codes for any other errors.
		*/
		virtual void WriteData(const TDesC8& aData, TRequestStatus& aStatus)=0;

		/** Signals to the agent that the entire file has now been transferred to the agent
		
		Notifies the client when any final processing operation is complete.
		
		@param aStatus 	Asynchronous request status. On completion this will 
						contain one of the following error codes: KErrNone if the data was written successfully. 
						KErrCANewFileHandleRequired if the agent requires a new 
						file handle before proceeding - the agent must save its 
						state and continue processing when the new handle 
						is supplied by ContinueWithNewOutputFile().
						KErrNotReady if the agent was not expecting WriteDataComplete() 
						to be called at this point. Otherwise one of the other CAF error codes 
						defined in \c caferr.h  or one of the other standard system-wide 
						error codes for any other errors.
		*/
		virtual void WriteDataComplete(TRequestStatus& aStatus)=0;

		/** Gets the number of output files produced so far. 
		@return The number of output files.
		*/
		virtual TInt OutputFileCountL() const = 0;

		/** Return information about an output file generated by the import operation. 
		
		 It is possible that the output file does not have the same name 
		 as the one suggested at the beginning of the import. An agent may
		 need to use a particular file extension or a particular name.

		The agent may decide not to store the output file in the output directory 
		suggested at the start of the import. eg. it may store the file in its private 
		server directory.

		The output files may only appear in this array after the WriteDataComplete() 
		function has been called. 

		@param aIndex The zero-based index of the file (must be less than the value returned by OutputFileCountL().
		@return The CSupplierOutputFile for this object.
		*/
		virtual CSupplierOutputFile& OutputFileL(TInt aIndex) = 0;

		/** Retrieve the import status

		Used to indicate non fatal problems encountered during the import operation.
		Provides feedback to the client in case any problems are encountered.

		The client can then decide whether or not to continue the import.
		 
		@return	The state of the import operation.
		*/
		virtual TImportStatus GetImportStatus() const = 0;

		
		/** Get the agents suggestion for the file extension of the output file required by the agent

		This function should only be called when the agent has returned KErrCANewFileHandleRequired from
		WriteData() or WriteDataComplete().

		The extension must include the dot '.' character.

		@param aFileExtension On completion this will contain the appropriate file extension.
		@return The result of the request for a file extension.
		@return KErrNone if the agent populated aFileExtension with the correct file extension.
		@return KErrNotReady if the agent is not waiting for another file handle to be provided.
		@return KErrUnknown if the agent does not know what extension should be used.
		*/
		virtual TInt GetSuggestedOutputFileExtension(TDes& aFileExtension) = 0;

		/** Get the agents suggestion for the file name (with extension) of the output file required by the agent

		This function should only be called when the agent has returned KErrCANewFileHandleRequired from
		WriteData() or WriteDataComplete().

		The file name is of the form filename.extension.

		@param aFileName On completion this will contain the appropriate filename.
		@return KErrNone if the agent populated aFileName with the correct file name.
		@return KErrNotReady if the agent is not waiting for another file handle to be provided.
		@return KErrUnknown if the agent does not know what file name should be used.
		*/
		virtual TInt GetSuggestedOutputFileName(TDes& aFileName) = 0;

		/** Continue the last write operation with a new file handle as requested by the agent

		@param aFile A new file handle opened with write permission.
		@param aFileName The name of the file handle that has  been supplied. This is used by the agent when creating CSupplierOutputFile objects. It should include the full path and filename in order to be consistent with files generated by the agent.
		@return The result of the continuing write operation.
		@return KErrNone if the WriteData() or WriteDataComplete() operation is now complete.
		@return KErrCANewFileHandleRequired if the agent requires a new 
						file handle before proceeding - the agent must save its 
						state and continue processing when the new handle 
						is supplied by ContinueWithNewOutputFile().
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.		
		*/
		virtual TInt ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName) = 0;

		/** Continue the last write operation with a new file handle and return the result asynchronously.

		@param aFile A new file handle opened with write permission
		@param aFileName The name of the file handle that has  been supplied. This is used by the agent when creating CSupplierOutputFile objects. It should include the full path and filename in order to be consistent with files generated by the agent
		
		@param aStatus 	Asynchronous request status. On completion this will 
						contain one of the following error codes: KErrNone if the 
						WriteData() or WriteDataComplete() operation is now complete.
						KErrCANewFileHandleRequired if the agent requires a new 
						file handle before proceeding - the agent must save its 
						state and continue processing when the new handle 
						is supplied by ContinueWithNewOutputFile(). Otherwise one of the 
						other CAF error codes defined in \c caferr.h  or one of the 
						other system-wide error codes for any other errors.
		*/
		virtual void ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName, TRequestStatus& aStatus) = 0;
		
		/** Cancel an outstanding asynchronous WriteData() request.
		*/
		IMPORT_C virtual void Cancel();
		
		/** Retrieve the content MIME type based on available data
		
		This function should only be called when the agent has returned KErrCANewFileHandleRequired from WriteData()
		If the agent cannot return a content MIME type, the importer should assume that no content type
		will be available until after the import session has been finished 

		@param aContentMime On return this will contain the appropriate content MIME type.
		@return The result of the request for a content MIME type.
		@return ETrue if the agent populated aContentMime with the correct content MIME type.
		@return EFalse if the agent cannot return the content MIME type (for example, there 
					is not enough data, or the content is multi-part)
		*/
		IMPORT_C virtual TBool ContentMimeTypeL(TDes8& aContentMime);
				
		};

	/**
	 Defines the agent interface for manager operations. 
	 
	 The CAgentManager is not related to any particular content object but 
	 used to manage files and settings belonging to this agent
	 
	All operations performed by the agent manager should be implemented in a 
	transactional manner by either completing or reverting to the state before
	the function was called.
	 
 	 @publishedPartner
	 @released
	 */	
	class CAgentManager : public CBase
		{
	public:
		/** 
		 Delete a file belonging to this agent

		 The agent implementation may delete rights that were associated with 
		 the content at the same time. If an agent does delete rights at the same
		 time as the content it should display a confirmation dialog that makes it
		 clear to the user the rights they paid for will be also deleted. 

		 Execution will be blocked until any dialog displayed is complete. Applications 
		 can request to disable the agents user interface using the SetProperty() command
		 
		 Access to the agents private directory is permitted at the 
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
		virtual TInt DeleteFile(const TDesC &aFileName) = 0;

		/**
		Make a copy of the content file (eg to removable media) belonging to this agent

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
		virtual TInt CopyFile(const TDesC& aSource, const TDesC& aDestination) = 0;

		/**
		Rename or move the content file (eg to removable media) belonging to this agent

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
		virtual TInt RenameFile(const TDesC& aSource, const TDesC& aDestination) = 0;

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
		
		@capability DRM Required when attempting to access an agents private directory
		*/
		virtual TInt MkDir(const TDesC& aPath) = 0;

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
		@capability DRM Required when attempting to access an agents private directory
		*/
		virtual TInt MkDirAll(const TDesC& aPath) = 0;

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
		virtual TInt RmDir(const TDesC& aPath) = 0;

		/**  Gets a filtered list of a directory's contents. 

		The bitmask determines which file and directory entry types should be listed. The sort key determines the order in which they are listed.

		Notes:
		-# If sorting by UID (as indicated when the ESortByUid bit is OR'ed with the sort key), then UID information will be included in the listing whether or not KEntryAttAllowUid is specified in aEntryAttMask.
		-# The function sets aFileList to NULL, and then allocates memory for it before appending entries to the list. Therefore, aFileList should have no memory allocated to it before this function is called, otherwise this memory will become orphaned.
		-# The caller of this function is responsible for deleting aFileList after the function has returned.


		This function can be used to list the contents of the agent's 
		private directory. Access to the agents private directory is permitted at 
		the agents discretion. 
		
		Agents can just return KErrCANotSupported if they do not wish to publish their
		private directory. Alternatively they can just populate the CDir object
		with the entries they wish to show.
	
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
		virtual TInt GetDir(const TDesC& aName,TUint aEntryAttMask,TUint aEntrySortKey, CDir*& aEntryList) const = 0;

		/**  Gets a filtered list of the directory and file entries contained in a directory and a list of the directory entries only

		The bitmask determines which file and directory entry types should be listed in aFileList. The contents of the second list, aDirList are not affected by the bitmask; it returns all directory entries contained in directory aName. The sort key determines the order in which both lists are sorted.

		Notes:
		-# If sorting by UID (as indicated when the ESortByUid bit is OR'ed with the sort key), then UID information will be included in the listing whether or not KEntryAttAllowUid is specified in aEntryAttMask.
		-# The function sets both aFileList and aDirList to NULL, and then allocates memory to them before appending entries to the lists. Therefore, aFileList and aDirList should have no memory allocated to them before this function is called, otherwise the allocated memory will become orphaned.
		-# The caller of this function is responsible for deleting aFileList and aDirList after the function has returned.

		This function can be used to list the contents of the agent's 
		private directory. Access to the agents private directory is permitted at 
		the agents discretion. 

		Agents can just return KErrCANotSupported if they do not wish to publish their
		private directory. Alternatively they can just populate the CDir object
		with the entries they wish to show.

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
		@capability DRM Required when attempting to access an agents private directory
		*/
		virtual TInt GetDir(const TDesC& aName,TUint aEntryAttMask,TUint aEntrySortKey, CDir*& aEntryList,CDir*& aDirList) const = 0;

		/**  Gets a filtered list of a directory's contents by UID

		The aUidType parameter determines which file entry types should be listed. The sort key determines the order in which they are listed.

		Notes:
		-# The function sets aFileList to NULL, and then allocates memory for it before appending entries to the list. Therefore, aFileList should have no memory allocated to it before this function is called, otherwise this memory will become orphaned.
		-# The caller of this function is responsible for deleting aFileList after the function has returned.

		This function can be used to list the contents of the agent's 
		private directory. Access to the agents private directory is permitted at 
		the agents discretion. 

		Agents can just return KErrCANotSupported if they do not wish to publish their
		private directory. Alternatively they can just populate the CDir object
		with the entries they wish to show.

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
		@capability DRM Required when attempting to access an agents private directory
		*/
		virtual TInt GetDir(const TDesC& aName,const TUidType& aEntryUid,TUint aEntrySortKey, CDir*& aFileList) const = 0;
		
		/**  Get an attribute from a content object
	
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
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath) = 0;

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
		IMPORT_C virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId);
		
		/** Get a set of attributes from a content object

		@param aAttributeSet The set of attributes to query and update.
		@param aVirtualPath The content object to retrieve attributes from.
		@return Whether the attribute set was updated.
		@return KErrNone if the attribute set was updated successfully.
		@return KErrNotFound if the content object was not found.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath) = 0;
		
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
		IMPORT_C virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId);	

		/**  Get text string attributes or meta-data from the file 

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
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath) = 0;

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
		IMPORT_C virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId);

		/** Used to obtain a set of string attributes 

		@param aStringAttributeSet The set of attributes to query and update.
		@param aVirtualPath The content object whose attributes are to be retrieved.
		@return Whether the string attribute set was updated.
		@return KErrNone if the attribute set was updated successfully.
		@return KErrNotFound if the object with the given virtual path was not found.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TVirtualPathPtr& aVirtualPath) = 0;

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
		IMPORT_C virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId);
				
		/** Notify the caller when the status of a DRM protected content object changes.
		NB: No assumption should be made about the scope of the descriptor 
		passed to aURI for asynchronous IPC.

		@param aURI The location of the file.
		@param aMask Bitmask of events the caller is interested in.
		@param aStatus The TRequestStatus object to complete if the event occurs, or KErrCANotSupported if the agent does not support asynchronous notifications.
		@capability DRM Deleting DRM protected content is not permitted for processes without DRM capability. 
		*/
		virtual void NotifyStatusChange(const TDesC& aURI, TEventMask aMask, TRequestStatus& aStatus) = 0;

		/** Cancel a previous notification request
		@param aURI The URI supplied in the call to NotifyStatusChange().
		@param aStatus The TRequestStatus supplied in the call to NotifyStatusChange().
		@return The outcome of the cancel request.
		@return KErrNone if the notification was cancelled.
		@return KErrNotFound if there was no matching request outstanding.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt CancelNotifyStatusChange(const TDesC& aURI, TRequestStatus& aStatus) = 0;

		/** 
		Allows an application to request the modification of a property 
		within the agent. The agent may or may not permit the property to be changed
		 	
		@param aProperty The property to set.
		@param aValue The value of the property.
		@return The outcome of the set property command.
		@return KErrNone if the property was set by all agents.
		@return KErrCANotSupported if one of the agent does not support the property or value.
		@return KErrAccessDenied if one of the agents does not permit the property to be changed.
		@return KErrPermissionDenied if the application does not have the necessary capability to change the property.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue) = 0;

		/** View information associated with a single content object
		
		This call blocks execution and only returns once the display is dismissed 
		by the user.

		@param aInfo The information to display.
		@param aVirtualPath The content object.
		@leave KErrCANotSupported if agent cannot display the requested information.
		@leave ...		One of the other CAF error codes defined in \c caferr.h  
		 				or one of the system-wide error codes 
						for any other errors.		
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		virtual void DisplayInfoL(TDisplayInfo aInfo, const TVirtualPathPtr& aVirtualPath) = 0;

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
		IMPORT_C virtual void DisplayInfoL(TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId);
	
		/** 
		Identifies whether or not the file at the given URI is to be handled by
		this agent.

		The agent should perform the mimimum possible processing to make the recognition
		process as fast as possible. It should avoid opening the file whenever possible.
		
		The agent should leave only if a temporary or permanent condition prevents it from knowing
		whether it can detect the content's type.
		  
		@param aURI		Location of the file to be examined.
		@param aShareMode	The share mode to use if the agent opens a file.
		@return			ETrue if this agent supports and recognises the file, EFalse otherwise. 
		*/
		virtual TBool IsRecognizedL(const TDesC& aURI, TContentShareMode aShareMode) const = 0;

		/** 
		Identifies whether or not the file is handled by this agent.

		The agent should look at the name of the file first to try and avoid the need
		to read from the file.
		
		The agent should leave only if a temporary or permanent condition prevents it from knowing
		whether it can detect the content's type.		
		  
		@param aFile An open file handle.
	    @return ETrue if this agent supports and recognises the file, EFalse otherwise. 
		*/
		virtual TBool IsRecognizedL(RFile& aFile) const = 0;

		/** 
		 Determines whether the file is recognized by this agent. 
		 If it is the agent will fill in the file mime type and content mime 
		 type. 
		 
		 The agent should attempt to recognized the file in an efficient manner
		 It should avoid opening the file to provide the best performance.

		 The agent should leave only if a temporary or permanent condition prevents it from 
		 detecting content's type.

		 The agent SHOULD NOT perform any API policing on this API since it
		 will be called from within the Apparc server's process. 
		  
		 If a file is recognized, the file mime type should always be set to
		 the correct value. For the content mime type there are three special cases:
		 
		 @li If the file needs to be imported through the Supplier API before
		 it can be used then the agent should leave the aContentMimeType
		 parameter blank (zero length). 
		 
		 @li If the file has no embedded content, eg an image/jpeg (.jpg) file 
		 the agent should leave the aContentMimeType parameter blank (zero length).
  
		 @li If more than one content object is contained within the file, the agent will
		 set aContentMimeType to "application/x-caf" indicating it is an archive that can
		 be navigated by CAF regardless of the actual file format.
		 
		 @param aFileName			The name of the file to recognize. The name is examined, not the file itself.
		 @param aBuffer				Buffer containing bytes from the start of the file.
		 @param aFileMimeType 		Used to return the mime type of the file to the caller.
		 @param aContentMimeType	Used to return the mime type of the content embedded within the file to the caller. This field should be zero length if there is no embedded content.
	     @return				Whether or not the file was recognized.
 		 @return ETrue if the file is handled by this agent. The aFileMimeType and aContentMimeType were updated.
		 @return EFalse if the file was not recognized by this agent.
		 */
		virtual TBool RecognizeFileL(const TDesC& aFileName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType) const = 0;

		
		/** Allows extended synchronous calls to the agent
		Objects are passed in and out using serialization

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
		virtual TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer) = 0;

		/** Allows extended asynchronous calls to the agent. 
		Objects can be passed in and out using serialization.
		NB: No assumption should be made about the scope of the descriptor 
		passed to aInputBuffer for asynchronous IPC.

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
		virtual void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus) = 0;

		/** Allow the agent to display management information for users to 
		manage any settings	or other information stored by the agent

		The scope of this information is left to the agent implementation but it is expected
		that a DRM agent will display the state of all DRM rights objects 
		(pending, available, expired, orphaned) and allow users to delete rights 
		that are no longer required.

		@leave KErrCANotSupported If the agent does support the display of management information.
		@capability DRM Agents implementing DRM may not display their management dialog to processes without DRM capability
		*/
		virtual void DisplayManagementInfoL() = 0;


		/** Prepare the HTTP request headers required to download files that may be 
		required for this agent.

		@param aStringPool The string pool used by the HTTP session.
		@param aRequestHeaders The agent will add any required headers to this object.
		*/
		virtual void PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const = 0;

		
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
		@return KErrPermissionDenied if the client does not have the necessary capabilities to remove directories.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		@capability DRM Required when attempting to access an agents private directory
		*/
		IMPORT_C virtual TInt RenameDir(const TDesC& aOldName, const TDesC& aNewName );
		
		/**
		Make a copy of the content file (eg to removable media) belonging to this agent 
		using a file handle

		Access to the agents private directory is permitted at the 
		agents discretion.

  		@param aSource The handle the source file.
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
		IMPORT_C virtual TInt CopyFile(RFile& aSource, const TDesC& aDestination);
		
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		/** 
		Identifies whether or not the WMDRM content is handled by this agent using the header
		data of WMDRM file or stream content.
		
		The agent should leave only if a temporary or permanent condition prevents it from knowing
		whether it can detect the content's type.

		@param aHeaderData	Header data of WMDRM file or stream content.
	    @return ETrue 		If this agent recognises the given content. 
		*/
		IMPORT_C virtual TBool IsRecognizedL(const TDesC8& aHeaderData) const;
		
		/** 
		 Determines whether or not the WMDRM content is recognized by this agent. 
		 If it is recognized, the agent will populate the file and content mime 
		 type. 
		 
		 The agent should leave only if a temporary or permanent condition prevents it from 
		 detecting content's type.

		 @param aHeaderData			Header data of WMDRM content.
		 @param aFileMimeType 		Used to return the mime type of the content to the caller.
		 @param aContentMimeType	Used to return the mime type of the content embedded within the file to the caller. This field should be zero length if there is no embedded content.
	     @return					Whether or not the content was recognized.
 		 @return ETrue 				If the content is handled by this agent. The aFileMimeType and aContentMimeType were updated.
		 @return EFalse 			If the content was not recognized by this agent.
		 */
		IMPORT_C virtual TBool RecognizeContentL(const TDesC8& aHeaderData, TDes8& aFileMimeType, TDes8& aContentMimeType) const;
		
		/**  Get an attribute from WMDRM content.
	
		@param aHeaderData	Header data of WMDRM content.
		@param aAttribute	The attribute to retrieve from ContentAccess::TAttribute.
		@param aValue		Used to return the value of the attribute.
		
		@return				KErrNone if the attribute value was updated.
		@return				KErrCANotSupported if the requested attribute is not supported for this content.
		@return				Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		@capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C virtual TInt GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue);
		
		/** Get a set of attributes from WMDRM content.

		@param aHeaderData		Header data of WMDRM content.
		@param aAttributeSet	The set of attributes to query and update.
		
		@return 				KErrNone if the attribute set was updated successfully.
		@return 				Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
								other system-wide error codes for any other errors.
		@capability DRM 		Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C virtual TInt GetAttributeSet(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet);
		
		
		/**  Get text string attributes or meta-data from WMDRM content.

		@param aHeaderData	Header data of WMDRM content.
		@param aAttribute	The attribute to retrieve from ContentAccess::TStringAttribute.
		@param aValue		Used to return the value of the attribute.
		
		@return				KErrNone if the attribute was retrieved successfully.
		@return				KErrNotFound if the content object does not exist.
		@return				KErrCANotSupported if the requested attribute does not apply to this content object.
		@return				KErrOverflow if the buffer was not large enough to return the result.
		@return				Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
							other system-wide error codes for any other errors.
		@capability DRM 	Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C virtual TInt GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue);
		
		/** Used to obtain a set of string attributes from WMDRM content.

		@param aStringAttributeSet	The set of attributes to query and update.
		@param aHeaderData			Header data of WMDRM content.
	
		@return 					KErrNone if the attribute set was updated successfully.
		@return 					KErrNotFound if the object with the given virtual path was not found.
		@return 					Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
									other system-wide error codes for any other errors.
		@capability DRM 			Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted. 
		*/
		IMPORT_C virtual TInt GetStringAttributeSet(const TDesC8& aHeaderData, RStringAttributeSet& aStringAttributeSet);


#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		
		};

		
	/** 
	Manages and retrieves rights objects from an agent.

 	@publishedPartner
	@released
	*/
	class CAgentRightsManager  : public CBase
		{
	public:
		/** List all rights held by the agent 
		@param aArray The client supplied array used to store the CRightsInfo objects. The agent will add CRightsInfo objects to the supplied array.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		virtual void ListAllRightsL(RStreamablePtrArray<CRightsInfo>& aArray) const = 0;

		/** List all rights associated with a particular file
		@param aArray The client supplied array used to store the CRightsInfo objects. The agent will add CRightsInfo objects to the supplied array.
		@param aUri The name of the file.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		virtual void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, const TDesC& aUri) const = 0;

		/** List all rights associated with a particular content object
		@param aArray The client supplied array used to store the CRightsInfo objects. The agent will add CRightsInfo objects to the supplied array.
		@param aVirtualPath The content object.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		virtual void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, TVirtualPathPtr& aVirtualPath) const = 0;

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
		IMPORT_C virtual void ListRightsL(RStreamablePtrArray<CRightsInfo>& aArray, RFile& aFile, const TDesC& aUniqueId) const;			

		/** List all content associated with a particular rights object
		@param aArray The client supplied array used to store the list of content objects. The agent will add CVirtualPath objects to the supplied array.
		@param aRightsInfo The rights object.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		virtual void ListContentL(RStreamablePtrArray<CVirtualPath>& aArray, CRightsInfo& aRightsInfo) const = 0;
		
		/** This will be used by applications to retrieve an agent specific rights object
		@param aRightsInfo The rights object.
		@return An agent specific rights object.
		@leave KErrPermissionDenied If the agent does not permit the client to access rights information.
		@leave ...		One of the CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.				
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		virtual MAgentRightsBase* GetRightsDataL(const CRightsInfo& aRightsInfo) const = 0;

		/** Asks the DRM agent to delete a rights object 

		The agent may display a dialog asking the user to confirm the delete. Execution
		will be blocked until the dialog is complete. Applications can request 
		to disable the agents user interface using the SetProperty() command
	
		@param aRightsInfo The rights object.
		@return The outcome of the delete operation.
		@return KErrNone if the rights were deleted.
		@return KErrCancel if the user cancels an agent supplied confirmation dialog.
		@return KErrNotFound if the rights object does not exist.
		@return KErrPermissionDenied if the agent does not permit the client to access rights information.
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		virtual TInt DeleteRightsObject(const CRightsInfo& aRightsInfo) = 0;
		
		/** Deletes all rights associated with a particular content object

		The agent may display a dialog asking the user to confirm the delete. Execution
		will be blocked until the dialog is complete. Applications can request 
		to disable the agents user interface using the SetProperty() command

		@param aVirtualPath The path of the content object.
		@return The outcome of the delete operation.
		@return KErrNone if the rights were deleted.
		@return KErrNotFound if no rights objects exist for the specified content object.
		@return KErrCancel if the user cancels an agent supplied confirmation dialog.
		@return KErrPermissionDenied if the agent does not permit the client to access rights information.
		@capability DRM Access to DRM rights is not permitted for processes without DRM capability. 
		*/
		virtual TInt DeleteAllRightsObjects(const TVirtualPathPtr& aVirtualPath) = 0;

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
		IMPORT_C virtual TInt DeleteAllRightsObjects (RFile& aFile, const TDesC& aUniqueId); 
			
		/** 
		Allows an application to request the modification of a property 
		within the agent. The agent may or may not permit the property to be changed
		 	
		@param aProperty The property to set.
		@param aValue The value of the property.
		@return KErrNone if the property was set.
		@return KErrCANotSupported if the agent does not support the property or value.
		@return KErrAccessDenied if the agent does not permit the property to be changed.
		@return KErrPermissionDenied if the application does not have the necessary capability to change the property.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
 		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue) = 0;
		};


} // namespace ContentAccess
#endif // __AGENTINTERFACE_H__

