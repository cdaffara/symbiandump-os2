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


CRichText* CWordAppUi::Text()const
	{return iModel->Text();}


CWordDocument* CWordAppUi::Document()const
	{return STATIC_CAST(CWordDocument*,iDocument);}


CTextPageRegionPrinter& CWordAppUi::PageRegionPrinter()const
	{return Document()->PageRegionPrinter();}


TBool CWordAppUi::WysiwygModeIsSet()const
	{return iWordFlags&EWysiwygModeOn;}


TBool CWordAppUi::ToggleWysiwygMode()
// Change the state of the wysiwyg mode,
// return the NEW setting.
//
	{
	iWordFlags^=EWysiwygModeOn;
	return iWordFlags&EWysiwygModeOn;
	}

TBool CWordAppUi::SketchAsGlass()const
	{return iPicturePrefs&ESketchAsGlass;}


void CWordAppUi::SetSketchAsGlass(TBool aChartAsGlass)
	{
	if (aChartAsGlass)
		iPicturePrefs|=ESketchAsGlass;
	else
		iPicturePrefs&=~ESketchAsGlass;
	}


TBool CWordAppUi::GraphAsGlass()const
	{return iPicturePrefs&EGraphAsGlass;}


void CWordAppUi::SetGraphAsGlass(TBool aGraphAsGlass)
	{
	if (aGraphAsGlass)
		iPicturePrefs|=EGraphAsGlass;
	else
		iPicturePrefs&=~EGraphAsGlass;
	}


void CWordAppUi::SetPaginationOutOfDate(TBool aPaginationOutOfDate)
	{iPaginationOutOfDate=aPaginationOutOfDate;}


TBool CWordAppUi::PaginationOutOfDate()const
	{return iPaginationOutOfDate;}


void CWordAppUi::SetReadOnly(TBool aReadOnly)
	{iRichEd->SetReadOnly(aReadOnly);}


void CWordAppUi::NotifyNewTextL()
	{
	iModel=Document()->Model();
	iRichEd->SetDocumentContentL(*Text(),CEikEdwin::EUseText);
	iRichEd->InitialiseUnifiedEditorL();
	}
