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

@publishedPartner
@released
*/


#ifndef __IMPORTFILE_H__
#define __IMPORTFILE_H__

#include <e32base.h>
#include <f32file.h>
#include <caf/caftypes.h>

namespace ContentAccess
	{
	class CAgentFactory;
	class CSupplierOutputFile;
	class CMetaDataArray;
	class CAgentSupplier;
	class CAgentImportFile;
	class CSupplier;
	class CAgentInfo;
	class CAgentFactory;

	/**
	Created by the CSupplier to import a file. 
	
	The mime type of the file must be one of the CAF supplier mime types. 
	Applications should check using CSupplier::IsImportSupported().
	
	This class creates a CAgentSupplier and uses it to import the file into the 
	content access agent who supports the given mime type.
	
	This class is single use only and cannot be used to import multiple files. 
	The import process may result in one or more output files.
	
	@publishedPartner
	@released	
	*/
	class CImportFile: public CBase
		{
		friend class CSupplier;

	public:
		/** Destructor - deleting the CImportFile instance before calling WriteComplete() aborts the import operation 
		*/
		virtual ~CImportFile();

		/** Write a block of data from a file to an agent.
		
		@param aData The block of data.
		@return	The result of the write operation.
		@return KErrNone if the data was written successfully.
		@return	KErrCANewFileHandleRequired if the agent requires the client to provide a new file handle before proceeding. The agent must save its state and continue processing when the new handle is supplied by ContinueWithNewOutputFile().
		@return KErrCorrupt if the file does not match the format the agent is expecting.
		@return KErrNotReady if the agent was not expecting WriteData() to be called at this point.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.
		*/
		IMPORT_C TInt WriteData(const TDesC8& aData);

		/** Asynchronously write a block of data from a file to an agent.		 
		Notifies the client when the write operation is complete.
		NB: It is important that the descriptor passed to 
		aData remains in scope until the request has completed.	
		
		@param aData 	The block of data.	
		@param aStatus 	Asynchronous request status. On completion this will 
						contain one of the following error codes: KErrNone if the data was written successfully. 
						KErrCANewFileHandleRequired if the agent requires a new 
						file handle before proceeding - the agent must save its 
						state and continue processing when the new handle 
						is supplied by ContinueWithNewOutputFile(). KErrCorrupt 
						if the file does not match the format the agent is expecting.
						KErrNotReady if the agent was not expecting WriteDataComplete() 
						to be called at this point. Otherwise one of the other CAF error codes 
						defined in \c caferr.h  or one of the other standard system-wide 
						error codes for any other errors.
		*/
		IMPORT_C void WriteData(const TDesC8& aData, TRequestStatus& aStatus);

		/** Signals to the agent that the entire file has now been transferred to the agent.
		
		@return The result of the write operation. 
		@return KErrNone if the data was written successfully.
		@return	KErrCANewFileHandleRequired if the agent requires the client to provide a new file handle before proceeding. The agent must save its state and continue processing when the new handle is supplied by ContinueWithNewOutputFile().
		@return KErrCorrupt if the file does not match the format the agent is expecting.
		@return KErrNotReady if the agent was not expecting WriteDataComplete() to be called at this point.
		@return Otherwise one of the other CAF error codes defined in \c caferr.h  or one of the 
				other system-wide error codes for any other errors.		
		*/
		IMPORT_C TInt WriteDataComplete();


		/** Signals to the agent that the entire file has now been transferred to the agent
		
		Notifies the client when any final processing operation is complete.
		
		@param aStatus 	Asynchronous request status. On completion this will 
						contain one of the following error codes: KErrNone if the data was written successfully. 
						KErrCANewFileHandleRequired if the agent requires a new 
						file handle before proceeding - the agent must save its 
						state and continue processing when the new handle 
						is supplied by ContinueWithNewOutputFile(). KErrCorrupt 
						if the file does not match the format the agent is expecting.
						KErrNotReady if the agent was not expecting WriteDataComplete() 
						to be called at this point. Otherwise one of the other CAF error codes 
						defined in \c caferr.h  or one of the other standard system-wide 
						error codes for any other errors.
		*/
		IMPORT_C void WriteDataComplete(TRequestStatus& aStatus);

		/** Gets the number of output files produced so far. 
		@return The number of output files.
		*/
		IMPORT_C TInt OutputFileCountL() const;

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
		IMPORT_C CSupplierOutputFile& OutputFileL(TInt aIndex) const;

		/** Retrieve the import status

		Used to indicate non fatal problems encountered during the import operation.
		Provides feedback to the client in case any problems are encountered.

		The client can then decide whether or not to continue the import.
		 
		@return	The state of the import operation.
		*/
		IMPORT_C TImportStatus GetImportStatus() const;

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
		IMPORT_C TInt GetSuggestedOutputFileExtension(TDes& aFileExtension);

		/** Get the agents suggestion for the file name (with extension) of the output file required by the agent

		This function should only be called when the agent has returned KErrCANewFileHandleRequired from
		WriteData() or WriteDataComplete().

		The file name is of the form filename.extension.

		@param aFileName On completion this will contain the appropriate file name.
		@return KErrNone if the agent populated aFileName with the correct file name.
		@return KErrNotReady if the agent is not waiting for another file handle to be provided.
		@return KErrUnknown if the agent does not know what file name should be used.
		*/
		IMPORT_C TInt GetSuggestedOutputFileName(TDes& aFileName);

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
		IMPORT_C TInt ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName);

		/** Continue the last write operation with a new file handle and return the result asynchronously.
		NB: It is important that the handle passed to aFile and the
		descriptor passed to aFileName remain in scope until the request has completed.	

		@param aFile  	 A new file handle opened with write permission.
		@param aFileName The name of the file handle that has  been supplied. This is used by the agent when creating CSupplierOutputFile objects. It should include the full path and filename in order to be consistent with files generated by the agent		
		@param aStatus 	 Asynchronous request status. On completion this will 
						 contain one of the following error codes: KErrNone if the 
					 	 WriteData() or WriteDataComplete() operation is now complete.
					 	 KErrCANewFileHandleRequired if the agent requires a new 
						 file handle before proceeding - the agent must save its 
						 state and continue processing when the new handle 
						 is supplied by ContinueWithNewOutputFile(). Otherwise one of the 
						 other CAF error codes defined in \c caferr.h  or one of the 
						 other system-wide error codes for any other errors.		
		*/
		IMPORT_C void ContinueWithNewOutputFile(RFile& aFile, const TDesC& aFileName, TRequestStatus& aStatus);

		/** Cancel an outstanding asynchronous request to WriteData() or WriteDataComplete().
		*/
		IMPORT_C void Cancel();
		
		/** Retrieve the content MIME type based on available data

		This function should only be called when the agent has returned KErrCANewFileHandleRequired from
		WriteData() .
		Example of use:
		@code
		while(!endofsource) 
			{
			source.read(data); 
			err = import->WriteData(data); 

			// When application supplies file handles it must always check to see if 
			// the agent needs a new file handle 
			while(err == KErrCANewFileHandleRequired) 
				{               
				//We need to find out the content MIME type based on available data
				TBuf<KMaxDataTypeLength> contentMime;
				if(import->ContentMimeTypeL(contentMime) == KErrNone)
					{
					//content MIME type is discovered. Now client code could do something
					//to determine the output location and supply the appropriate file handle
					
					
					}
				else
					{
					//content MIME type can't be determined yet. Client should decide the location for
					//output file now,e.g. a default location

					}
				        
				err = import->ContinueWithNewOutputFile(outputFileHandle, outputFileName); 
				// It is possible that the agent needs yet another file handle 
				outputFileHandle.Close(); // agent makes a copy so we don't need to keep our file handle 
				}
			}

		@endcode

		@param aContentMime On return this will contain the appropriate content MIME type.
		@return The result of the request for a content MIME type.
		@return ETrue if the agent populated aContentMime with the correct content MIME type.
		@return EFalse if the agent cannot return the content MIME type (for example, there 
					is not enough data, or the content is multi-part)
		*/
		IMPORT_C TBool ContentMimeTypeL(TDes8& aContentMime); 
		
	private:
		CImportFile();

		static CImportFile* NewL(TUid aAgentUid, const TDesC8& aMimeType, const CMetaDataArray& aImportMetaData, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);
		void ConstructL(TUid aAgentUid, const TDesC8& aMimeType, const CMetaDataArray& aImportMetaData, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);
		
		static CImportFile* NewL(TUid aAgentUid, const TDesC8& aMimeType, const CMetaDataArray& aImportMetaData);
		void ConstructL(TUid aAgentUid, const TDesC8& aMimeType, const CMetaDataArray& aImportMetaData);

	private:
		// Agent within CAF used to import the file
		CAgentImportFile* iAgentImportFile;

		// Agent factory, de-facto ECOM session handle
		CAgentFactory* iAgentFactory;
		};
	}
#endif
