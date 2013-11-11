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


#ifndef __WPDOC_H__
#define __WPDOC_H__

#include <gdi.h>
#include <fldbltin.h>
#include <txtmrtsr.h>
#include <eikdoc.h>
#include <eikapp.h>
#include "WNGMODEL.H"	// only #included for Model() inline
#include <e32base.h>

#include "WPTESTPICTURE.H"

/**
@internalComponent
*/
#if !defined KUidWordAppValue
#ifdef _UNICODE
#define KUidWordAppValue KUidWordAppValue16
#else
#define KUidWordAppValue KUidWordAppValue8
#endif
#endif

/**
@internalComponent
*/
const TUid KUidWordAppValue8={0x1000007F};
const TUid KUidWordAppValue16={0x10003A64};

class CFileStore;
class CWordAppUi;
class TWordUiData;

class CWordDocument : public CEikDocument, public MRichTextStoreResolver, public MFieldFileNameInfo, public MFieldNumPagesInfo
/**
@internalComponent
*/
	{
public:
	static CWordDocument* NewL(CEikApplication& aApp);
	~CWordDocument();
	//
	// CApaDocument implementation
	virtual void StoreL(CStreamStore& aStore,CStreamDictionary& aStreamDic) const;
	virtual void RestoreL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic);
	virtual void DetachFromStoreL(CPicture::TDetach aDegree);
	void StoreUiDataL(CStreamStore& aStore,CStreamDictionary& aStreamDic)const;
	void RestoreUiDataL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic);
	virtual void NewDocumentL();
	//
	virtual const CStreamStore& StreamStoreL(TInt aPos)const;  // MRichTextStoreResolver
	virtual TInt UpdateFieldFileName(TPtr& aValueText)const;  // MFieldFileNameInfo
	virtual TInt UpdateFieldNumPages()const;  // MFieldNumPagesInfo
	//
	// File
	void DoFileNewL(const TFileName& aFileName, const TFileName& aTemplateFileName);
	void DoFileOpenL(const TFileName& aFileName,TUint aFileMode=EFileWrite);
	void GetFileOpenModeL(TUint& aFileMode,const TDesC& aFilename,RFs& aSession);
	void DoFileSaveToCurrentL();
	void DoFileSaveToNewL(const TFileName& aFileName);
	void DoFileSaveToNewNoSwitchL(const TFileName& aNewFileName);
	void DoFileRevertL();
	//
	// Handles
	inline TWordUiData& UiData();
	inline CWordModel* Model()const;
	inline CWordAppUi& WordAppUi();
	inline const CWordAppUi& WordAppUi()const;
	inline CTextPageRegionPrinter& PageRegionPrinter()const;
	//
	// Utilities
	inline TBool FileNameExists(const TFileName& aFileName) const;
private:
	enum TKeepStoreOpen
		{
		EKeepFalse,
		EKeepTrue
		};
private:
	inline CWordDocument(CEikApplication& aApp);
	void ConstructL();
	void CreateModelL();
	void ConstructPrintL();
	void LocateTemplateL(const TDes& aFullFileName)const;
	void VerifyDocumentTypeL(const TFileName& aFileName)const;
	CFileStore* DoNewDocumentL(const TDesC& aStandardDocument,TKeepStoreOpen aKeepStoreOpen=EKeepFalse);
	void SetMainStoreAndMainDocFileName(CFileStore* aMainStore,const TDesC& aMainDocFileName);
	//
	// CApaDocument implementation
	CEikAppUi* CreateAppUiL();
private:
	CWordModel* iModel;
	CTextPageRegionPrinter* iPrint;
	TWordUiData* iUiData;
	TBool iNewDocument;
	TWordTestPictureFactory iPictureFactory;
	};


#include "WPDOC.INL"


#endif
