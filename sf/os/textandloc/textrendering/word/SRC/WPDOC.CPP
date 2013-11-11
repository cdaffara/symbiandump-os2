/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include <s32std.h>
#include <s32stor.h>
#include <s32file.h>

#include <f32file.h>

#include <gdi.h>

#include <txtrich.h>
#include "txtmrtsr.h"

#include <frmprint.h>

#include <prnsetup.h>

#include <basched.h>

#include <coemain.h>

#include <apparc.h>
#include <apgdoor.h>

#include <eikproc.h>
#include <techview/eikpword.h>
#include <techview/eikon.rsg>
#include <eikfutil.h>
#include <techview/eikdialg.h>
#include <eikenv.h>

#include "WNGMODEL.H"

#include "WPDOC.H"
#include "WPAPPUI.H"
#include "WPMAIN.H"
#include "WPPANIC.H"
#include "wpresources.h"
#include <word.rsg>
#include "WORD.HRH"

//#define ALLOC_TESTING

//
// CWordDocument
//

const TUid KUidWordAppUiStream={268436035};

CWordDocument* CWordDocument::NewL(CEikApplication& aApp)
// Return a handle to a new instance of this class.
//
	{
#if defined(ALLOC_TESTING)
	// reserve some space on the cleanup stack
	{for (TInt ii=0;ii<1000;++ii)
		CleanupStack::PushL(&ii);}
	CleanupStack::Pop(1000);
	//
	__UHEAP_MARK;
	{
	CWordDocument* self=NULL;
	for (TInt ii=2;;++ii)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		__UHEAP_MARK;
		TInt err=KErrNone;
		self=new(ELeave) CWordDocument(aApp);
		if (self)
			{
			TRAP(err,self->ConstructL());
			delete self;
			self=NULL;
			}
		__UHEAP_MARKEND;
		User::Heap().Check();
		if (err==KErrNone)
			break;
		}
	__UHEAP_MARKEND;
	__UHEAP_RESET;
	User::Heap().Check();
	}
#endif
	CWordDocument* self=new(ELeave) CWordDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CWordDocument::ConstructL()
// Complete initialisation
//
	{
	CreateModelL();
	ConstructPrintL();
	iUiData=new(ELeave) TWordUiData;
	}


CWordDocument::~CWordDocument()
	{
	delete iModel;
	delete iPrint;
	delete iUiData;
	}


void CWordDocument::CreateModelL()
	{iModel=CWordModel::NewL(this,this);}


void CWordDocument::ConstructPrintL()
// Print initialisation
//
	{
	CCoeEnv* coeEnv=CCoeEnv::Static();
	CWordModel& model=*Model();
	CPrintSetup& printSetup=*model.PrintSetup();
	printSetup.CreatePrinterDeviceL(KUidPrinterDevice,coeEnv->FsSession());
	iPrint=CTextPageRegionPrinter::NewL(model.Text(),printSetup.PrinterDevice());
	//
	TPageMargins margins;									 
	margins.iMargins.iLeft=1440;
	margins.iMargins.iRight=1440;			
	margins.iMargins.iTop=1440;		   
	margins.iMargins.iBottom=1440;
	margins.iHeaderOffset=720;
	margins.iFooterOffset=720;
	TPageSpec pageSpec;
	pageSpec.iPortraitPageSize=TSize(11906,16838);	   // A4 !!	
	printSetup.iPageMarginsInTwips=margins;
  	printSetup.PrinterDevice()->SelectPageSpecInTwips(pageSpec);	   // A4 !!
	}


void CWordDocument::NewDocumentL()
// Builds a new main (container) document, creating content from a standard file.
// If a leave occurs whilst loading the standard document, current document
// content will not be lost.
//
	{ 
	TFileName templateFileName;
	TWordFilePath::GetNormalTemplateFolderName(templateFileName);
	TFindFile* ff = new(ELeave) TFindFile(Process()->FsSession());
	CleanupStack::PushL(ff);
	TFileName* tempFileName = new(ELeave) TFileName();
	CleanupStack::PushL(tempFileName);
	CCoeEnv* coeEnv = CCoeEnv::Static();
	coeEnv->ReadResource(*tempFileName, R_WORD_NORMAL_TEMPLATE_FILE_NAME);
	templateFileName.Append(*tempFileName);
	// look for a normal template
	templateFileName[0] = 'Y';
	TParsePtrC tParse(templateFileName);
	TInt error = ff->FindByDir(templateFileName,tParse.DriveAndPath());
	if(error)
		{
		// no normal template, search ROM for blank
		TWordFilePath::GetNormalTemplateFolderName(templateFileName);
		coeEnv->ReadResource(*tempFileName, R_WORD_BLANK_TEMPLATE_FILE_NAME);
		templateFileName.Append(*tempFileName);
		templateFileName[0] = 'Z';
		TParsePtr blankParse(templateFileName);
		error = ff->FindByDir(blankParse.NameAndExt(),blankParse.DriveAndPath());
		}
	CleanupStack::PopAndDestroy(); // tempFileName
	if(error!=KErrNone)
		Panic(EDefaultTemplateNotPresent);
	templateFileName = ff->File();
	CleanupStack::PopAndDestroy(); // ff
	DoNewDocumentL(templateFileName);
	if (iAppUi)
		WordAppUi().NotifyNewTextL();  // edwin must know of new text for focus loss etc...
	}


CFileStore* CWordDocument::DoNewDocumentL(const TDesC& aStandardDocument,TKeepStoreOpen aKeepStoreOpen/*=EKeepFalse*/)
// Open and restore the specified valid document.
// The store that is opened over the file for restoration may be kept open, or destroyed, depending on
// the value of aKeepStoreOpen.
//
	{
	CFileStore* store=NULL;
	CStreamDictionary* dictionary=CApaProcess::ReadRootStreamLC(Process()->FsSession(),store,aStandardDocument,EFileShareExclusive|EFileRead);  // takes ownership of store
	CleanupStack::PushL(store);
	iNewDocument = ETrue;
	RestoreL(*store,*dictionary);
	iNewDocument = EFalse;
	iModel->Text()->DetachFromStoreL(CPicture::EDetachFull);
	if (aKeepStoreOpen==EKeepTrue)
		CleanupStack::Pop();  // store
	else
		CleanupStack::PopAndDestroy();  // store
	CleanupStack::PopAndDestroy();  // dictionary
	return (aKeepStoreOpen==EKeepTrue)
		? store
		: NULL;
	}


void CWordDocument::LocateTemplateL(const TDes& aFullFileName) const
// Verify that the template file selected by the user is still present
//
	{
	TFindFile ff(Process()->FsSession());
	TParsePtrC tParse(aFullFileName);
	User::LeaveIfError(ff.FindByDir(aFullFileName,tParse.DriveAndPath()));
	VerifyDocumentTypeL(aFullFileName);
	}

void CWordDocument::VerifyDocumentTypeL(const TFileName& aFileName)const
// Verify the document of the specified name is indeed of the
// same type as this document object.
//
	{
	TEntry entry;
	User::LeaveIfError(Process()->FsSession().Entry(aFileName,entry));
	if (entry.MostDerivedUid()!=Application()->AppDllUid())
		User::Leave(KErrCorrupt);
	}


const CStreamStore& CWordDocument::StreamStoreL(TInt /*aPos*/)const
// In this case the store does not vary with document position.
//
	{return *iEditStore;}


void CWordDocument::DetachFromStoreL(CPicture::TDetach aDegree)
// Propogate the detachFromStore to all picture container components.
//
	{
	iModel->Text()->DetachFromStoreL(aDegree);
	CRichText* text=iModel->PrintSetup()->Header()->Text();
	if (text)
		text->DetachFromStoreL(aDegree);
	text=iModel->PrintSetup()->Footer()->Text();
	if (text)
		text->DetachFromStoreL(aDegree);
	iEditStore=NULL;
	}

void CWordDocument::StoreUiDataL(CStreamStore& aStore,CStreamDictionary& aStreamDic)const
// Write any Ui data
//
	{
	if (iAppUi)
		WordAppUi().GetUiData(*iUiData);

	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
	stream <<*iUiData;
	stream.CommitL();
	CleanupStack::PopAndDestroy();  // stream

	aStreamDic.AssignL(KUidWordAppUiStream,id);
	}


void CWordDocument::StoreL(CStreamStore& aStore,CStreamDictionary& aStreamDic)const
// Write this object network.
//
	{
	if (iAppUi)
		CEikonEnv::Static()->BusyMsgL(R_EIK_TBUF_SAVING_FILE,KFileBusyInitialDelayInMicroSeconds);
	StoreUiDataL(aStore,aStreamDic);
	iModel->StoreL(aStore, aStreamDic, 0);
	if (iAppUi)
		CEikonEnv::Static()->BusyMsgCancel();
	}


void CWordDocument::RestoreUiDataL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic)
// Restore any ui data for future use
//
	{
	TStreamId id=aStreamDic.At(KUidWordAppUiStream);
	if (id==KNullStreamId)
		iUiData->Reset();
	else
		{
		RStoreReadStream stream;
		stream.OpenLC(aStore,id);
		stream>> *iUiData;
		CleanupStack::PopAndDestroy();  // stream
		}
	}


void CWordDocument::RestoreL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic)
// Restore this document.
// All model restoration is done here, to save having a temporary model member data,
// implementing a rollback robust model restore.
//
	{
	CEikonEnv* eikonEnv=CEikonEnv::Static();
	if (iAppUi && !iNewDocument)
		eikonEnv->BusyMsgL(R_EIK_TBUF_OPENING_FILE,KFileBusyInitialDelayInMicroSeconds);
	else if (iAppUi && iNewDocument)
		eikonEnv->BusyMsgL(R_WORD_FILE_CREATE_FILE,KFileBusyInitialDelayInMicroSeconds);
	//
	CWordModel* tempModel=CWordModel::NewL(this,this);
	CleanupStack::PushL(tempModel);
	//
	CStreamStore* oldStore=iEditStore;
	iEditStore=CONST_CAST(CStreamStore*,&aStore);
	//
	tempModel->Text()->SetPictureFactory(&iPictureFactory,this);  // rich text callbacks
	TRAPD(ret,
		tempModel->RestoreL(aStore,aStreamDic,0,this,this,eikonEnv->PictureFactory()));  // print setup field callbacks
	if (ret!=KErrNone)
		{
		iEditStore=oldStore;
		User::Leave(ret);
		}
	delete iModel;
	iModel=tempModel;  // document takes over ownership
	CleanupStack::Pop();  // tempModel

	SetChanged(EFalse);  // this is a brand new document - it cannot yet have been changed.

	TRAP(ret,
	RestoreUiDataL(aStore,aStreamDic));
		if (ret!=KErrNone)
			iUiData->Reset();  // do not propogate.  We can handle this.
	//
	if (iAppUi)
		{
		WordAppUi().SetUiData();
//		CEikonEnv::Static()->BusyMsgCancel();
		}
	}


TInt CWordDocument::UpdateFieldFileName(TPtr& aValueText)const
// Set aValueText with the current document filename only.
// (No path or extension).
//
	{
	TParsePtrC parser(Process()->MainDocFileName());
	TPtrC pp=parser.NameAndExt();
	if (pp.Length()>aValueText.MaxLength())
		return pp.Length();
	aValueText=pp;
	return 0;
	}


TInt CWordDocument::UpdateFieldNumPages()const
// Return the number of pages currently in this document.
// Precondition: pagination of the document has already occured.
//
	{return iModel->PageTable()->Count();}


void CWordDocument::DoFileNewL(const TFileName& aNewFilename, const TFileName& aTemplateFileName)
// Create an initialised document instance with the specified name,
// and initialise a new file store to store the document in.
// (A new document is not created, *this* instance is re-used).
//
	{
	//
	// Create the file store for the new document
	LocateTemplateL(aTemplateFileName);
	User::LeaveIfError(EikFileUtils::CopyFile(aTemplateFileName,aNewFilename));
	CCoeEnv* coeEnv=CCoeEnv::Static();
	User::LeaveIfError(coeEnv->FsSession().SetAtt(aNewFilename,KEntryAttNormal,KEntryAttReadOnly));
	// turn read-only file attribute if set, just to be safe.
	//
	CFileStore* store=DoNewDocumentL(aNewFilename,EKeepTrue);
	__ASSERT_DEBUG(store,Panic(ENewFileNullStore));
	//
	SetMainStoreAndMainDocFileName(store,aNewFilename);
	SetChanged(ETrue);
	TUint appFileMode=AppFileMode();
	appFileMode|=EFileWrite;
	SetAppFileMode(appFileMode);
	WordAppUi().SetReadOnly((TBool)!(appFileMode&EFileWrite));
	}


void CWordDocument::SetMainStoreAndMainDocFileName(CFileStore* aMainStore,const TDesC& aMainDocFileName)
	{
	CEikProcess& process=REINTERPRET_CAST(CEikProcess&,*Process());
	delete process.MainStore();  // remove the [original] store
	process.SetMainStore(aMainStore);  // set the new file store as the main one.
	process.SetMainDocFileName(aMainDocFileName);
	}


void CWordDocument::DoFileOpenL(const TFileName& aFileName,TUint aFileMode)
// Open the Word document specified by aFileName.
//
	{
	VerifyDocumentTypeL(aFileName);
	RFs session=Process()->FsSession();
	GetFileOpenModeL(aFileMode,aFileName,session);
//
	CFileStore* store=NULL;
	CStreamDictionary* dic=CApaProcess::ReadRootStreamLC(session,store,aFileName,EFileShareExclusive|aFileMode);
	CleanupStack::PushL(store);
	RestoreL(*store,*dic);
	CleanupStack::Pop();  // store
	CleanupStack::PopAndDestroy();  // dictionary
	//
	SetMainStoreAndMainDocFileName(store,aFileName);
	SetAppFileMode(aFileMode);
	WordAppUi().SetReadOnly((TBool)!(aFileMode&EFileWrite));
	}


void CWordDocument::GetFileOpenModeL(TUint& aFileMode,const TDesC& aFilename,RFs& aSession)
// Attempt to open the specified file in the specified mode.
// If this fails because write access was requested on a read-only file,
// the file is opened read-only.
// Any other error (leave) is propogated.
// aFileMode is set to the mode that the file was successfully opened in.
//
	{
	CFileStore* store=NULL;
	TRAPD(ret,
	store=CFileStore::OpenL(aSession,aFilename,aFileMode));
	if (ret==KErrAccessDenied && aFileMode&EFileWrite)
		{
		ret=KErrNone;
		aFileMode&=~EFileWrite;
		store=CFileStore::OpenL(aSession,aFilename,aFileMode);
		}
	//
	User::LeaveIfError(ret);
	__ASSERT_DEBUG(ret==KErrNone && store,Panic(EFileOpenIntegrityError));
	delete store;
	}


void CWordDocument::DoFileSaveToCurrentL()
	{
	SaveL();	  
	}


void CWordDocument::DoFileSaveToNewL(const TFileName& aNewFileName)
	{
	((CEikProcess*)Process())->SaveToDirectFileStoreL(this,&aNewFileName); // writes root stream
	SetAppFileMode(EFileWrite);
	}

void CWordDocument::DoFileSaveToNewNoSwitchL(const TFileName& aNewFileName)
// Save the current file to a new file, but don't open the new file
	{
	// From CEikProcess::SaveToDirectFileStoreL but without the close iMainDoc at the end
	//
	CApaProcess* proc = Process();
	RFs& fsSession=proc->FsSession();
	CDirectFileStore* store;
	store = CDirectFileStore::CreateLC(fsSession,aNewFileName,EFileWrite);
	store->SetTypeL(REINTERPRET_CAST(CEikProcess*,proc)->MainStore()->Type());
	CStreamDictionary* streamDic=CStreamDictionary::NewL();
	CleanupStack::PushL(streamDic);
	proc->MainDocument()->StoreL(*store,*streamDic);
	// write root stream
	proc->WriteRootStreamL(*store,*streamDic,*(this->Application()));
	CleanupStack::PopAndDestroy(); // streamDic
	// close the new store
	store->CommitL();
	CleanupStack::PopAndDestroy(); // store
	}

void CWordDocument::DoFileRevertL()
	{
	CStreamDictionary* streamDic=CStreamDictionary::NewL();
	CleanupStack::PushL(streamDic);
	CFileStore* mainStore=REINTERPRET_CAST(CEikProcess*,Process())->MainStore();
	RStoreReadStream root;
	root.OpenLC(*mainStore,mainStore->Root());
	root>> *streamDic;
	root.Close();
	CleanupStack::PopAndDestroy(); // root
	RestoreL(*mainStore,*streamDic);
	CleanupStack::PopAndDestroy(); // streamDic
	}

//
// CWordApplication
//

TUid CWordApplication::AppDllUid() const
	{
	return KUidWordAppValue;
	}

CApaDocument* CWordApplication::CreateDocumentL()
	{return CWordDocument::NewL(*this);}


CEikAppUi* CWordDocument::CreateAppUiL()
	{
#if defined(ALLOC_TESTING)
	// reserve some space on the cleanup stack
	{for (TInt ii=0;ii<1000;++ii)
		CleanupStack::PushL(&ii);}
	CleanupStack::Pop(1000);
	// construct the appUi before the setfail loop so that cached resources
	// do not cause alloc failure
	CWordAppUi* appUi=NULL;
	appUi=new(ELeave) CWordAppUi;
	appUi->SetDocument(this);
	appUi->ConstructL();
	delete appUi;
	appUi=NULL;
	__UHEAP_MARK;
	{
	for (TInt ii=2;;++ii)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		__UHEAP_MARK;
		appUi=new(ELeave) CWordAppUi;
		appUi->SetDocument(this);
		TInt err=KErrNone;
		if (appUi)
			{
			TRAP(err,appUi->ConstructL());
			delete appUi;
			appUi=NULL;
			}
		__UHEAP_MARKEND;
		User::Heap().Check();
		if (err==KErrNone)
			break;
		}
	__UHEAP_MARKEND;
	__UHEAP_RESET;
	User::Heap().Check();
	}
#endif
    return(new(ELeave) CWordAppUi);
	}
