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
 @internalComponent
 @released
*/

#ifndef __F32AGENTMANAGER_H__
#define __F32AGENTMANAGER_H__

#include <caf/agentinterface.h>

namespace ContentAccess
	{
	class CF32AgentUi;

	/** 	
	* CF32AgentManager is an Ecom implementation of interface CAgentManager, 
	* and provides  a fall-back agent for content which is not restricted 
	* (i.e. backward compatibility for clients switching to 
	* Content-Access-Framework APIs).
	* 
	* Essentially, if the CA framework does not identify a piece of
	* content as restricted (or rights managed) then this agent
	* implementation is created to handle access to the content. As such the
	* content will be treated as a plaintext file.
	*
	* @internalComponent 
	* @released 
	*/
	class CF32AgentManager : public CAgentManager
		{
	public:
		/** 
		* 	Standard C-Class NewL() constructor 
		*/
		static CF32AgentManager* NewL();
		
		~CF32AgentManager(void);


	public:
		virtual TInt DeleteFile(const TDesC &aFileName);
		virtual TInt CopyFile(const TDesC& aSource, const TDesC& aDestination);
		virtual TInt CopyFile(RFile& aSource, const TDesC& aDestination);
		virtual TInt RenameFile(const TDesC& aSource, const TDesC& aDestination);
		virtual TInt MkDir(const TDesC& aPath);
		virtual TInt MkDirAll(const TDesC& aPath);
		virtual TInt RmDir(const TDesC& aPath);
		virtual TInt GetDir(const TDesC& aName,TUint aEntryAttMask,TUint aEntrySortKey, CDir*& aEntryList) const;
		virtual TInt GetDir(const TDesC& aName,TUint aEntryAttMask,TUint aEntrySortKey, CDir*& aEntryList,CDir*& aDirList) const;
		virtual TInt GetDir(const TDesC& aName,const TUidType& aEntryUid,TUint aEntrySortKey, CDir*& aFileList) const;
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath);
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath);
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath);
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId);
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId);	
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId);
		virtual void NotifyStatusChange(const TDesC& aURI, TEventMask aMask, TRequestStatus& aStatus);
		virtual TInt CancelNotifyStatusChange(const TDesC& aURI, TRequestStatus& aStatus);
		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue);	
		virtual void DisplayInfoL(TDisplayInfo aInfo, const TVirtualPathPtr& aVirtualPath);
		virtual void DisplayInfoL(TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId);
		virtual TBool IsRecognizedL(const TDesC& aURI, TContentShareMode aShareMode) const;
		virtual TBool IsRecognizedL(RFile& aFile) const;
		virtual TBool RecognizeFileL(const TDesC& aFileName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType) const;
		virtual TInt AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer);
		virtual void AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus);
		virtual void DisplayManagementInfoL();
		virtual void PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const;
		virtual TInt RenameDir(const TDesC& aOldName, const TDesC& aNewName);

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		virtual TBool IsRecognizedL(const TDesC8& aHeaderData) const;
		virtual TBool RecognizeContentL(const TDesC8& aHeaderData, TDes8& aFileMimeType, TDes8& aContentMimeType) const;
		virtual TInt GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue);
		virtual TInt GetAttributeSet(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet);
		virtual TInt GetStringAttributeSet(const TDesC8& aHeaderData, RStringAttributeSet& aAttributeSet);
		virtual TInt GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue);
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

	protected:
		CF32AgentManager();

	private:
		/** 
		* 	Second stage in two-phase create. Parameters may also be
		* 	passed to a new Agent implementation.
		*
		*  	@param aParams	A parameter structure containing
		* 					any necessary agent parameters.
		*/
		void ConstructL();
		
		CF32AgentUi& AgentUiL();

	private:
		RFs iFs;
		CFileMan *iFileMan;
		CF32AgentUi *iAgentUi;
		};
} // namespace ContentAccess
#endif // __F32AGENTMANAGER_H__
