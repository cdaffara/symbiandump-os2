/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The code in this file is not executed.
* It is compiled to ensure that types in the ContentAccess namespace can be used
* when the headers are included but the namespace is not in scope.
* It will ensure that a regression cannot occur if during development the ContentAccess
* namespace is brought into scope in error by any of the public header files.
* Any new types added to the ContentAccess namespace must also be added to this file.
* ContentAccess namespace when it is not in scope.
* This test does not execute.
* This test is a compile time test.
*
*/


/**
 @SYMTestCaseID			SYSLIB-ECOM-CT-3404
 @SYMTestCaseDesc		Ensures there are no name collisions with types defined in the
 @SYMTestPriority			High
 @SYMTestActions			For each type in the ContentAccess namespace reuse the type name.
 @SYMTestExpectedResults	This is a compile time test.
 @SYMDEF				PDEF102493
*/
#include <caf/caf.h>

// streamableptrarray.h
#include <caf/streamableptrarray.h>

typedef TInt RStreamablePtrArray;
RStreamablePtrArray streamablePtrArray=0;

// agent.h
#include <caf/agent.h>

typedef TInt TAgent;
TAgent agent=0;

// agentfactory.h
#include <caf/agentfactory.h>

typedef TInt CAgentFactory;
CAgentFactory agentFactory=0;

// agentinterface.h
#include <caf/agentinterface.h>

typedef TInt CAgentData;
CAgentData agentData=0;

typedef TInt CAgentContent;
CAgentContent agentContent=0;

typedef TInt CAgentImportFile;
CAgentImportFile agentImportFile=0;

typedef TInt CAgentManager;
CAgentManager agentManager=0;

typedef TInt CAgentRightsManager;
CAgentRightsManager agentRightsManager=0;

// attribute.h
#include <caf/attribute.h>

typedef TInt CAttribute;
CAttribute attribute=0;

// attributeset.h
#include <caf/attributeset.h>

typedef TInt RAttributeSet;
RAttributeSet attributeSet=0;

// bitset.h
#include <caf/bitset.h>

typedef TInt CBitset;
CBitset bitset=0;

// CafMimeHeader.h
#include <caf/cafmimeheader.h>

typedef TInt CCafMimeHeader;
CCafMimeHeader cafMimeHeader=0;


// caftypes.h
#include <caf/caftypes.h>

typedef TInt TQosAttribute;
TQosAttribute qosAttribute=0;
typedef TInt EQosBufferSize;
EQosBufferSize qosBufferSize=0;
typedef TInt EQosAttrTop;
EQosAttrTop qosAttrTop=0;

typedef TInt TMimeFields;
TMimeFields mimeFields=0;
typedef TInt EContentType;
EContentType contentType=0;
typedef TInt EContentLength;
EContentLength contentLength=0;
typedef TInt EContentId;
EContentId contentId=0;
typedef TInt EContentTransferEncoding;
EContentTransferEncoding contentTransferEncoding=0;
typedef TInt EBoundary;
EBoundary boundary=0;
typedef TInt EMimeMax;
EMimeMax mimeMax=0;

typedef TInt TContentShareMode;
TContentShareMode contentShareMode=0;
typedef TInt EContentShareReadOnly;
EContentShareReadOnly contentShareReadOnly=0;
typedef TInt EContentShareReadWrite;
EContentShareReadWrite contentShareReadWrite=0;
typedef TInt EContentShareExclusive;
EContentShareExclusive contentShareExclusive=0;

typedef TInt TIntent;
TIntent intent=0;
typedef TInt EPeek;
EPeek peek=0;
typedef TInt EPlay;
EPlay play=0;
typedef TInt EView;
EView view=0;
typedef TInt EExecute;
EExecute execute=0;
typedef TInt EPrint;
EPrint print=0;
typedef TInt EPause;
EPause pause=0;
typedef TInt EContinue;
EContinue continue1=0;
typedef TInt EStop;
EStop stop=0;
typedef TInt EUnknown;
EUnknown unknown=0;
typedef TInt EInstall;
EInstall install=0;
	
typedef TInt TOutputType;
TOutputType outputType=0;
typedef TInt EContent;
EContent content=0;
typedef TInt EReceipt;
EReceipt receipt=0;

typedef TInt TImportStatus;
TImportStatus importStatus=0;
typedef TInt EInProgress;
EInProgress inProgress=0;
typedef TInt EWaitingForNewOutputFile;
EWaitingForNewOutputFile waitingForNewOutputFile=0;
typedef TInt EComplete;
EComplete complete=0;
typedef TInt EFailed;
EFailed failed=0;
typedef TInt EUnsupportedMedia;
EUnsupportedMedia unsupportedMedia=0;

typedef TInt TAttribute;
TAttribute attribute2=0;
typedef TInt EIsProtected;
EIsProtected isProtected=0;
typedef TInt EIsForwardable;
EIsForwardable isForwardable=0;
typedef TInt EIsModifyable;
EIsModifyable isModifyable=0;
typedef TInt EIsCopyable;
EIsCopyable isCopyable=0;
typedef TInt ECanPlay;
ECanPlay canPlay=0;
typedef TInt ECanPrint;
ECanPrint canPrint=0;
typedef TInt ECanExecute;
ECanExecute canExecute=0;
typedef TInt ECanView;
ECanView canView=0;
typedef TInt ERightsNone;
ERightsNone rightsNone=0;
typedef TInt ERightsPending;
ERightsPending rightsPending=0;
typedef TInt ERightsHaveExpired;
ERightsHaveExpired rightsHaveExpired=0;
typedef TInt EPreviewAvailable;
EPreviewAvailable previewAvailable=0;
typedef TInt EContentCDataInUse;
EContentCDataInUse contentCDataInUse=0;
typedef TInt ECanRewind;
ECanRewind canRewind=0;
typedef TInt ECopyPaste;
ECopyPaste copyPaste=0;
typedef TInt ERightsConsumable;
ERightsConsumable rightsConsumable=0;
typedef TInt ERightsStateless;
ERightsStateless rightsStateless=0;
typedef TInt ECanMove;
ECanMove canMove=0;
typedef TInt ECanRename;
ECanRename canRename=0;
typedef TInt ECanAutomaticConsume;
ECanAutomaticConsume canAutomaticConsume=0;
typedef TInt EAgentSpecificAttributeBase;
EAgentSpecificAttributeBase agentSpecificAttributeBase=0;

typedef TInt TStringAttribute;
TStringAttribute stringAttribute=0;
typedef TInt EDescription;
EDescription description=0;
typedef TInt EMimeType;
EMimeType mimeType=0;
typedef TInt ETitle;
ETitle title=0;
typedef TInt EAuthor;
EAuthor author=0;
typedef TInt EIconURI;
EIconURI iconURI=0;
typedef TInt EPreviewURI;
EPreviewURI previewURI=0;
typedef TInt EContentURI;
EContentURI contentURI=0;
typedef TInt EContentID;
EContentID contentID=0;
typedef TInt EInfoURL;
EInfoURL infoURL=0;
typedef TInt EPendingRightsETA;
EPendingRightsETA pendingRightsETA=0;
typedef TInt ETransactionID;
ETransactionID transactionID=0;
typedef TInt EAgentSpecificStringAttributeBase;
EAgentSpecificStringAttributeBase agentSpecificStringAttributeBase=0;
		
typedef TInt TEventMask;
TEventMask eventMask=0;
typedef TInt ERightsAvailable;
ERightsAvailable rightsAvailable=0;
typedef TInt ERightsExpired;
ERightsExpired rightsExpired=0;

typedef TInt TEmbeddedType;
TEmbeddedType embeddedType=0;
typedef TInt EContainerObject;
EContainerObject containerObject=0;
typedef TInt EContentObject;
EContentObject contentObject=0;
typedef TInt EAgentSpecificObject;
EAgentSpecificObject agentSpecificObject=0;

typedef TInt TDisplayInfo;
TDisplayInfo displayInfo=0;
typedef TInt EFileProperties;
EFileProperties fileProperties=0;
typedef TInt EFileAndRights;
EFileAndRights fileAndRights=0;
typedef TInt ERights;
ERights rights=0;

typedef TInt TRightsTypeMask;
TRightsTypeMask rightsTypeMask=0;
typedef TInt ERightsTypeConsumable;
ERightsTypeConsumable rightsTypeConsumable=0;
typedef TInt ERightsTypeStateless;
ERightsTypeStateless rightsTypeStateless=0;

typedef TInt TRightsStatus;
TRightsStatus rightsStatus=0;
typedef TInt ERightsStatusNone;
ERightsStatusNone rightsStatusNone=0;
typedef TInt ERightsStatusValid;
ERightsStatusValid rightsStatusValid=0;
typedef TInt ERightsStatusExpired;
ERightsStatusExpired rightsStatusExpired=0;
typedef TInt ERightsStatusPending;
ERightsStatusPending rightsStatusPending=0;

typedef TInt TAgentProperty;
TAgentProperty agentProperty=0;
typedef TInt EAgentPropertyBufferSize;
EAgentPropertyBufferSize agentPropertyBufferSize=0;
typedef TInt EAgentPropertyAgentUI;
EAgentPropertyAgentUI agentPropertyAgentUI=0;
typedef TInt EAgentPropertyMultipleSequence;
EAgentPropertyMultipleSequence agentPropertyMultipleSequence=0;

typedef TInt KMaxCafUniqueId;
KMaxCafUniqueId maxCafUniqueId=0;
typedef TInt KMaxCafContentName;
KMaxCafContentName maxCafContentName=0;

// content.h
#include <caf/content.h>

typedef TInt CContent;
CContent content2=0;

// data.h
#include <caf/content.h>

typedef TInt CData;
CData data=0;

// dirstreamable.h
#include <caf/dirstreamable.h>

typedef TInt CDirStreamable;
CDirStreamable dirStreamable=0;

// embeddedobject.h
#include <caf/embeddedobject.h>

typedef TInt CEmbeddedObject;
CEmbeddedObject embeddedObject=0;

// f32agentui.h
#include <caf/f32agentui.h>

typedef TInt CF32AgentUi;
CF32AgentUi f32AgentUi=0;
typedef TInt TF32AgentUiFactory;
TF32AgentUiFactory f32AgentUiFactory=0;

// importfile.h
#include <caf/importfile.h>

typedef TInt CImportFile;
CImportFile importFile=0;

// manager.h
#include <caf/manager.h>

typedef TInt CManager;
CManager manager=0;

// metadata.h
#include <caf/metadata.h>

typedef TInt CMetaData;
CMetaData metaData=0;

// metadataarray.h
#include <caf/metadataarray.h>

typedef TInt CMetaDataArray;
CMetaDataArray metaDataArray=0;

// rightsInfo.h
#include <caf/rightsinfo.h>

typedef TInt CRightsInfo;
CRightsInfo rightsInfo=0;
typedef TInt MAgentRightsBase;
MAgentRightsBase agentRightsBase=0;

// rightsmanager.h
#include <caf/rightsmanager.h>

typedef TInt CRightsManager;
CRightsManager rightsManager=0;

// stringattributeset.h
#include <caf/stringattributeset.h>

typedef TInt RStringAttributeSet;
RStringAttributeSet stringAttributeSet=0;

// supplier.h
#include <caf/supplier.h>

typedef TInt CSupplier;
CSupplier supplier=0;

// supplieroutputfile.h
#include <caf/supplieroutputfile.h>

typedef TInt CSupplierOutputFile;
CSupplierOutputFile supplierOutputFile=0;

// virtualpath.h
#include <caf/virtualpath.h>

typedef TInt CVirtualPath;
CVirtualPath virtualPath=0;

// virtualpathptr.h
#include <caf/virtualpathptr.h>

typedef TInt TVirtualPathPtr;
TVirtualPathPtr virtualPathPtr=0;

