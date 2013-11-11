/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TRanspEditor.h
* TTranspEditor test header file. A base set of classes, neeeded for transparent editors
* functionality testing, is declared here.
*
*/


#ifndef __TTRANSPEDITOR_H__
#define __TTRANSPEDITOR_H__

/////////////////////////////////////////////////////////////////////////////////////////////
//Picture
//CTestPicture test class is used to check the effect of opaque drawing, applied on pictures
//inserted into the text.

class CTestPicture : public CPicture
	{
public:
	static CTestPicture* NewL();
	virtual ~CTestPicture();
	void Draw(CGraphicsContext&, const TPoint&, const TRect&, MGraphicsDeviceMap*) const;
	void ExternalizeL(RWriteStream&) const;
	void GetOriginalSizeInTwips(TSize& aSize) const;
	TInt ScaleFactorWidth() const;
	TInt ScaleFactorHeight() const;

private:
	CTestPicture();
	void ConstructL();

private:
	CFbsBitmap* iBitmap;

	};

/////////////////////////////////////////////////////////////////////////////////////////////
//Application 

class CTranspEditorApp : public CEikApplication
	{
public:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;

	};

/////////////////////////////////////////////////////////////////////////////////////////////
//Document

class CTranspEditorDoc : public CEikDocument
	{
public:
	CTranspEditorDoc(CEikApplication& aApp);

private:
	CEikAppUi* CreateAppUiL();

	};

/////////////////////////////////////////////////////////////////////////////////////////////
//View1
//CTranspEditorView1 class is used for displaying a bitmap, which occupies the whole screen
//and is used as a background for a transparent text view, displayed on top of it.

class CTranspEditorView1 : public CCoeControl
	{
public:
	static CTranspEditorView1* NewL();
	~CTranspEditorView1();

private:
	void ContructL();
	void Draw(const TRect&) const;

private:
	CFbsBitmap* iBitmap;

	};

/////////////////////////////////////////////////////////////////////////////////////////////
//View2
//CTranspEditorView2 class is used for displaying a transparent text view on top of a 
//background bitmap. The class offers functions for inserting texts, pictures, switching
//on/off text selection and opaque drawing mode.

class CTranspEditorView2 : public CCoeControl
	{
public:
	static CTranspEditorView2* NewL();
	~CTranspEditorView2();
	void InsertTextL();
	void SwitchOpaque();
	void SwitchSelectL();
	void InsertPictureL();
	void SetCharFormatL();

private:
	void ContructL();
	void Draw(const TRect&) const;

private:
	CRichText*			iRichText;
	CTextLayout*		iLayout;
	CTextView*			iTextView;
	TBool				iOpaque;
	TBool				iSelect;

	};

/////////////////////////////////////////////////////////////////////////////////////////////
//UI

class CTranspEditorUi : public CEikAppUi
	{
public:
	void ConstructL();
	~CTranspEditorUi();

private:
	void HandleCommandL(TInt aCommand);

private: 
	CTranspEditorView1* iTranspEditorView1;
	CTranspEditorView2* iTranspEditorView2;
	
	};

#endif//__TTRANSPEDITOR_H__
