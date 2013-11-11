/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __DATA_H__
#define __DATA_H__

#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>

namespace ContentAccess
{
	class CAgentFactory;
	class CAgentInfo;
	class TVirtualPathPtr;
	class CAgentData;
	class RAttributeSet;
	class RStringAttributeSet;
	

	/**
 	Allows clients to read data from a content object. 
	 
 	This class is initialised with an agent implementation that is
 	responsible for this content object.
 
 	*/
	class CData : public CBase
		{
	public:
		/** 
		 Creates a new CData object. 
		 
		 @param aVirtualPath The content object to read
		 @param aIntent	The intended use of the content.
		 @param aShareMode The file share mode used to open this content.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...		One of the other CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes 
						for any other errors.
		 */
		IMPORT_C static CData* NewL(const TVirtualPathPtr& aVirtualPath,
							TIntent aIntent, TContentShareMode aShareMode); 

		/** 
		 Creates a new CData object. 
		 
		 @param aVirtualPath The content object to read.
		 @param aShareMode The file share mode used to open this content.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ... 	One of the other CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes for 
						any other errors.		 
		 */
		IMPORT_C static CData* NewL(const TVirtualPathPtr& aVirtualPath,
							TContentShareMode aShareMode); 
		

		/** 
		 Creates a new CData object 
		 
		 @param aVirtualPath The content object to read.
		 @param aIntent	The intended use of the content.
		 @param aShareMode The file share mode used to open this content.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...		One of the other CAF error codes defined in \c caferr.h  
		 				or one of the other system-wide error codes for 
						any other errors.
		 */
		IMPORT_C static CData* NewLC(const TVirtualPathPtr& aVirtualPath,
							TIntent aIntent, TContentShareMode aShareMode); 

		/** 
		 Creates a new CData object 
		 
		 @param aVirtualPath The content object to read.
		 @param aShareMode The file share mode used to open this content.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...			One of the other CAF error codes defined in \c caferr.h  
		 					or one of the other system-wide error codes for 
							any other errors.
		 */					
		IMPORT_C static CData* NewLC(const TVirtualPathPtr& aVirtualPath,
							TContentShareMode aShareMode); 

							
		/** 
		 Creates a new CData object. 
		 
		 @param aFile An open RFile handle, the agent will make a duplicate of this handle. 
		 The underlying RFs session should be shared for transfer to other processes using RFs::ShareProtected(), since the agent may use transfer this handle to its own or one of the system servers.
		 @param aUniqueId The content object to read.
		 @param aIntent	The intended use of the content.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...			One of the other CAF error codes defined in \c caferr.h  
		 					or one of the other system-wide error codes for 
							any other errors.
		 */
		IMPORT_C static CData* NewL(RFile& aFile, const TDesC& aUniqueId, TIntent aIntent);

		/** 
		 Creates a new CData object. 
		 
		 @param aFile An open RFile handle, the agent will make a duplicate of this handle. 
		 The underlying RFs session should be shared for transfer to other processes using RFs::ShareProtected(), since the agent may use transfer this handle to its own or one of the system servers.
		 @param aUniqueId The content object to read.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...			One of the other CAF error codes defined in \c caferr.h  
		 					or one of the other system-wide error codes for 
							any other errors.
		 */
		IMPORT_C static CData* NewL(RFile& aFile, const TDesC& aUniqueId);
		
		/** 
		 Creates a new CData object. 
		 
		 @param aFile An open RFile handle, the agent will make a duplicate of this handle. 
		 The underlying RFs session should be shared for transfer to other processes using RFs::ShareProtected(), since the agent may use transfer this handle to its own or one of the system servers.
		 @param aUniqueId The content object to read.
		 @param aIntent	The intended use of the content.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...				One of the other CAF error codes defined in \c caferr.h  
		 						or one of the other system-wide error codes 
								for any other errors.
		 */
		IMPORT_C static CData* NewLC(RFile& aFile, const TDesC& aUniqueId, TIntent aIntent);
		
		/** 
		 Creates a new CData object. 
		 
		 @param aFile An open RFile handle, the agent will make a duplicate of this handle. 
		 The underlying RFs session should be shared for transfer to other processes using RFs::ShareProtected(), since the agent may use transfer this handle to its own or one of the system servers.
		 @param aUniqueId The content object to read.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...				One of the other CAF error codes defined in \c caferr.h  
		 						or one of the other system-wide error codes 
								for any other errors.
		 */
		IMPORT_C static CData* NewLC(RFile& aFile, const TDesC& aUniqueId);
		
		/** 
		 Creates a new CData object. 
		 
		 @param aAgentUid The agent determined to support this content.
		 @param aVirtualPath The content object to read.
		 @param aIntent	The intended use of the content.
		 @param aShareMode The file share mode used to open this content.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...				One of the other CAF error codes defined in \c caferr.h  
		 						or one of the other system-wide error codes 
								for any other errors.
		 */
		static CData* NewLC(TUid aAgentUid, const TVirtualPathPtr& aVirtualPath,
							TIntent aIntent, TContentShareMode aShareMode); 
		
		/** 
		 Creates a new CData object. 
		 
		 @param aAgentUid The Uid of the agent who supports this content.
		 @param aFile An open RFile handle, the agent will make a duplicate of this handle. 
		 The underlying RFs session should be shared for transfer to other processes using RFs::ShareProtected(), since the agent may use transfer this handle to its own or one of the system servers.
		 @param aUniqueId The content object to read.
		 @param aIntent	The intended use of the content.
		 @return The new CData object.
		  
		 @leave KErrNotFound		The content object with the given UniqueId does not exist.
		 @leave KErrCAPendingRights The rights have not yet arrived but are expected soon.
		 @leave KErrCANoPermission	Rights exist but the specified intent is not permitted.
		 @leave KErrCANoRights		No rights exist for the content object.
		 @leave KErrCANotSupported	Unable to open the object with the specified UniqueId, it may be a container object.
		 @leave KErrPermissionDenied The agent does not allow the client to access the content object.
		 @leave KErrAccessDenied	The content is already in use.
		 @leave ...				One of the other CAF error codes defined in \c caferr.h  
		 						or one of the other system-wide error codes for 
								any other errors.

		 */
		static CData* NewLC(TUid aAgentUid, RFile& aFile, const TDesC& aUniqueId, TIntent aIntent);

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		
		/** 
		 Creates a new CData object. 
		 @param aHeaderData				Header data of WMDRM file/stream content. 
		 @return 						CData object.
		 @leave							One of the CAF error codes defined in caferr.h or one of the 
										system-wide error for any other errors.
		 @capability 					DRM Access to DRM protected content is not permitted for processes without DRM capability.
										Access to unprotected content is unrestricted.
		*/
			
		IMPORT_C static CData* NewL(const TDesC8& aHeaderData);
		
		/** 
		 Creates a new CData object. 
		 @param aHeaderData				Header data of WMDRM file/stream content.
		 @param aIntent					The intended use of the content.
		 @return 						CData object.
		 @leave							One of the CAF error codes defined in caferr.h or one of the 
										system-wide error for any other errors.
		 @capability 					DRM Access to DRM protected content is not permitted for processes without DRM capability.
										Access to unprotected content is unrestricted.
		*/		
		
		IMPORT_C static CData* NewL(const TDesC8& aHeaderData, TIntent aIntent);
		
		/** 
		 Creates a new CData object. 
		 @param aHeaderData				Header data of WMDRM file/stream content. 
		 @return 						CData object.
		 @leave							One of the CAF error codes defined in caferr.h or one of the 
										system-wide error for any other errors.
		 @capability 					DRM Access to DRM protected content is not permitted for processes without DRM capability.
										Access to unprotected content is unrestricted.
		*/
		
		IMPORT_C static CData* NewLC(const TDesC8& aHeaderData);
		
		/** 
		 Creates a new CData object. 
		 @param aHeaderData				Header data of WMDRM file/stream content.
		 @param aIntent					The intended use of the content. 
		 @return 						CData object.
		 @leave							One of the CAF error codes defined in caferr.h or one of the 
										system-wide error for any other errors.
		 @capability 					DRM Access to DRM protected content is not permitted for processes without DRM capability.
										Access to unprotected content is unrestricted.
		*/
		
		IMPORT_C static CData* NewLC(const TDesC8& aHeaderData, TIntent aIntent);
		
		/** 
		 Creates a new CData object.
		 @param	aAgentUid				UID of an agent which supports this content. 
		 @param aHeaderData				Header data of WMDRM file/stream content.
		 @param aIntent					The intended use of the content.
		 @return 						CData object.
		 @leave							One of the CAF error codes defined in caferr.h or one of the 
										system-wide error for any other errors.
		 @capability 					DRM Access to DRM protected content is not permitted for processes without DRM capability.
										Access to unprotected content is unrestricted.
		 
		*/
		
		static CData* NewLC(TUid aAgentUid, const TDesC8& aHeaderData, TIntent aIntent);
		
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

		/** destructor */
		virtual ~CData();
		
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
		 @return		KErrNone if successful.
		 @return		One of the CAF error codes defined in \c caferr.h  or 
		 				one of the other system-wide error codes.
 		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt Read(TDes8& aDes) const;

		/** 
		 Reads from a content object up to the specified length of the
		 descriptor or the end of the content object.
		 	
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
		IMPORT_C TInt Read(TDes8& aDes,TInt aLength) const;
		
		/**
		 Read content asynchronously up to the maximum length of the descriptor
		 or until the end of the content object is reached.
		 NB: It is important that the descriptor passed to 
		 aDes remains in scope until the request has completed.		
		 
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
		IMPORT_C void Read(TDes8& aDes, TRequestStatus& aStatus) const;

		/**
		 Read content asynchronously up to the specified length 
		 or until the end of the content object is reached.
		 NB: It is important that the descriptor passed to 
		 aDes remains in scope until the request has completed.		 	

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
		IMPORT_C void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus) const;
		
		/**
		 Cancels asynchronous read.
		 
		 @param aStatus		Asynchronous request status. This parameter should have been supplied earlier to
		 					an asynchronous Read call. If it has not been previously supplied to a Read call,
		 					this function will not have any effect.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C void ReadCancel(TRequestStatus &aStatus) const;		

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		/**
		 Reads content asynchronously. The data is read from a specified offset 
		 up to a specified number of bytes or until the end of the content object 
		 is reached. The data is read into the descriptor buffer supplied.
		 NB: It is important that the descriptor passed to 
		 aDes remains in scope until the request has completed.
		 If agent does not support 64bit, fallback to 32bit Read is provided automatically by CAF			
				 
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
		 @return KErrArgument if a negative offset is supplied.
		 @return KErrCANotSupported if the agent does not support this operation.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt Read(TInt64 aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus) const;
#else
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
		 @return KErrArgument if a negative offset is supplied.
		 @return KErrCANotSupported if the agent does not support this operation.
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus) const;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

		/**
		 Gets the data size in bytes.
		 
		 @param aSize	On return this will contain the size of the plaintext data in bytes.
		 @leave KErrCASizeNotDetermined	Size could not be determined by the managing agent.
		 @leave ...		One of the other CAF error codes defined in \c caferr.h  or one of the 
						system-wide error codes for any other errors.		 
		 @capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted
		 */
		IMPORT_C void DataSizeL(TInt& aSize);

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		/**
		 This is the 64bit version of CData::DataSizeL
		 Client can call this function instead of CData::DataSizeL. If it's not implemented by the agent,
		 fallback to 32bit counterpart will be provided automatically
		 
		 @see DataSizeL(TInt& aSize)
		*/
		IMPORT_C void DataSize64L(TInt64& aSize);
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
				
		/**
		 Changes or retrieves the location of the file pointer within 
		 the content object.
		 
 		 NOTE: Some operations may fail if the content data is sourced over a network connection.		 
		 
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
		IMPORT_C TInt Seek(TSeek aMode,TInt& aPos) const;
		
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		/**
		 This is the 64bit version of CData::Seek
		 Client can call this function instead of CData::Seek. If it's not implemented by the agent,
		 fallback to 32bit counterpart will be provided automatically
		 
		 @see Seek(TSeek aMode,TInt& aPos)
		*/
		IMPORT_C TInt Seek64(TSeek aMode,TInt64& aPos) const;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

		/** Request the agent handling this content to set a property value. If the property is set
		it is only set for this CData session and does not impact other CAF	users.

		 For example setting the buffer-size to 256 bytes can be achieved as follows:

					@code
		  			MyData->SetProperty(EAgentPropertyBufferSize, 256);
		  			@endcode

  		@see ContentAccess::TAgentProperty
	
		@param aProperty The property to set.
		@param aValue The value of the property.
		@return Whether or not the property was set.
		@return KErrNone if the property was set.
		@return KErrCANotSupported if the agent does not support the property or value.
		@return KErrAccessDenied if the agent does not permit the property to be changed.
		@return KErrPermissionDenied if the application does not have the necessary capability to change the property.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted.
		*/
		IMPORT_C TInt SetProperty(TAgentProperty aProperty, TInt aValue);
		
		/**
		 Allows a client to verify that the intent is supported by the
		 current rights for the data object. 
		 
		 @note Since the intent is only evaluated the agent will not decrement 
		 any rights-state such as play counts.
		  
		 @see ContentAccess::TIntent

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
		IMPORT_C TInt EvaluateIntent(TIntent aIntent);
		
		/**
		 Execute intent allows the application to signal it is about to carry out
		 the specified intent on protected content.The agent will decrement applicable 
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
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the.
				other system-wide error codes for any other errors.
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/		
		IMPORT_C TInt ExecuteIntent(TIntent aIntent);
		
		/**  Get an attribute for the content object

		@see ContentAccess::TAttribute

		@code
		CContent* content = CContent::NewL(uri);
		CData* data = content->OpenContentL(EPlay, uniqueId);
		delete content;

		TInt value = 0;
		TInt err = data->GetAttribute(EIsProtected, value);
		if(err == KErrNone && value)
			{
			DisplayPadLock();
			}

		delete data;
		@endcode

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
		IMPORT_C TInt GetAttribute(TInt aAttribute, TInt& aValue) const;

		/** Get a set of attributes for the content object 

		@see ContentAccess::TAttribute

		The following example determines whether the content object 
		is protected and has rights that will enable it to be viewed by the 
		user

		@code
		// CData* data = a data object
		
		RAttributeSet attributeSet;
		CleanupClosePushL(attributeSet);
		attributeSet->AddL(EProtected);
		attributeSet->AddL(ECanView);

		User::LeaveIfError(data->GetAttributeSet(attributeSet);

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
		CleanupStack::PopAndDestroy(); 	// attributeSet.Close()
		@endcode


		@param aAttributeSet The set of attributes to query and update.
		@return Whether the attribute set was updated.
		@return KErrNone if the attributes were retrieved successfully.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.				 
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt GetAttributeSet(RAttributeSet& aAttributeSet) const;


		/**  Get text string attributes or meta-data for the content object
	
		@see ContentAccess::TStringAttribute

		@code
		CContent* content = CContent::NewLC(uri);
		CData* data = content->OpenContentLC(EPlay, uniqueId);
		CleanupStack::PopAndDestroy(content);

		TBuf <MAX_PATH> previewUri;
		TInt err = data->GetStringAttribute(EPreviewURI, previewUri);
		if(err == KErrNone)
			{
			DisplayPreview(previewUri);
			}

		CleanupStack::PopAndDestroy(data);
		@endcode

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
		IMPORT_C TInt GetStringAttribute(TInt aAttribute, TDes& aValue) const;

		
		
		/** Obtain a set of string attributes for the content object 
		
		@see ContentAccess::TStringAttribute

		@code
		CContent* content = CContent::NewLC(uri);
		CData* data = content->OpenContentLC(EPlay, uniqueId);

		// create the attribute set, add the attributes we are interested in
		RStringAttributeSet stringAttributeSet;
		CleanupClosePushL(stringAttributeSet);
		stringAttributeSet.AddL(EPreviewURI);
		stringAttributeSet.AddL(ETitle);
		
		User::LeaveIfError(data->GetStringAttributeSet(stringAttributeSet));
	
		// Pass the value of the string attribute to DisplayPreview()
		TFileName previewUri;
		TInt err = stringAttributeSet.GetValue(EPreviewURI, previewUri);
		if(err == KErrNone)
			{
			DisplayPreview(previewUri);
			}
		
		CleanupStack::PopAndDestroy(3);	// content, data, stringAttributeSet
		@endcode
	

		@param aStringAttributeSet The set of attributes to query and update.
		@return Whether the string attribute set was updated.
		@return KErrNone if the attributes were retrieved successfully.
		@return KErrNotFound if the default content object was not found.
		@return KErrPermissionDenied if the access to the protected content is not permitted by the CAF Agent.
		@return Otherwise one of the CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.				 
		@capability DRM Access to DRM protected content is not permitted for processes without DRM capability. Access to unprotected content is unrestricted 
		*/
		IMPORT_C TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet) const;
				
				
#ifndef REMOVE_CAF1
		/** Set Qos attribute
		@param aQosAttr The Qos attribute to set.
		@param aValue The value of the attribute.
		@deprecated 
		*/
		IMPORT_C void SetQosL(TQosAttribute aQosAttr, TInt aValue);

		/** Get the mime type of the content
		@param aMimeType Buffer to store the mime type provided by the agent.
		@return ETrue if the mime type was retrieved successfully.
		@deprecated 
		*/
		IMPORT_C TBool GetMimeTypeL(TDes8& aMimeType) const;
#endif	// REMOVE_CAF1

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		/**
		Decrypts the encrypted input data packet.
		
		@param aEncryptedInputDataPacket	Buffer descriptor containing the encrypted data packet supplied by client application. 
		@param aDecryptedOutputDataPacket	Buffer descriptor supplied by the client application into which the decrypted data is written.
											The length of this descriptor must be equal to or greater than the input packet.
		@return								KErrNone if successful.KErrInsufficientDataPacketLength if a part of input packet is provided,
											otherwise one of the CAF error codes defined in \c caferr.h  or 
		 									one of the other system-wide error codes.
		@capability 						DRM Access to DRM protected content is not permitted for processes without DRM capability.
											Access to unprotected content is unrestricted.
		*/

		IMPORT_C TInt Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket) const;
		
		/**
		Decrypts the encrypted input data packet asynchronously.
		@param aEncryptedInputDataPacket	Buffer descriptor containing the encrypted data packet supplied by client application. 
		@param aDecryptedOutputDataPacket	Buffer descriptor supplied by the client application into which the decrypted data is written.
											The length of this descriptor must be equal to or greater than the input packet.
		@param aStatus						Asynchronous request status. On completion this will contain one of the following error codes:
											KErrNone if the data packet was successfully decrypted.KErrInsufficientDataPacketLength if a 
											part of input packet is provided, otherwise one of the CAF error codes defined in \c caferr.h or 
		 									one of the other system-wide error codes.
		@capability 						DRM Access to DRM protected content is not permitted for processes without DRM capability.
										    Access to unprotected content is unrestricted. 
		*/
		IMPORT_C void Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket, TRequestStatus& aStatus) const;

#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	
	private:
		CData();
		void ConstructL(RFile& aFile, const TDesC& aUniqueId, TIntent aIntent);
		void ConstructL(const TVirtualPathPtr& aVirtualPath, TIntent aIntent, TContentShareMode aShareMode);
		void ConstructL(TUid aAgentUid, RFile& aFile, const TDesC& aUniqueId, TIntent aIntent);
		void ConstructL(TUid aAgentUid, const TVirtualPathPtr& aVirtualPath, TIntent aIntent, TContentShareMode aShareMode); 

		void ConstructL(RFile& aFile, const TDesC& aUniqueId);
		void ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
		void ConstructL(TUid aAgentUid, RFile& aFile, const TDesC& aUniqueId);
		void ConstructL(TUid aAgentUid, const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode); 

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		
		void ConstructL(TUid aAgentUid, const TDesC8& aHeaderData, TIntent aIntent);
		void ConstructL(const TDesC8& aHeaderData);
		void ConstructL(const TDesC8& aHeaderData, TIntent aIntent);
		void ConstructL(TUid aAgentUid, const TDesC8& aHeaderData);
		
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		/*
		 * This is the obselete 32bit Read and replaced by its 64bit counterpart
		 * TInt Read(TInt64 aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus) const
		 * However, this function still exits at its original ordinal to avoid BC break. 
		 * Upgrade to 64bit Read is done automatically upon recompling the client code which uses CAF interfaces 
		*/
		IMPORT_C TInt Read_Unused(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus) const;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	private:
		// The agent handling this content
		CAgentData* iAgentData; 

		// Defacto ECOM session handle
		CAgentFactory* iAgentFactory;

		// Uid of the agent
		TUid iAgentUid;
		};

} // namespace ContentAccess
#endif /* __DATA_H__ */

