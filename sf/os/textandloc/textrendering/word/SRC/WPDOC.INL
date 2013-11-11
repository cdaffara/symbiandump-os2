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


CWordDocument::CWordDocument(CEikApplication& aApp)
	: CEikDocument(aApp),
	iNewDocument(EFalse)
	{}

CWordModel* CWordDocument::Model()const
	{return iModel;}

CWordAppUi& CWordDocument::WordAppUi()
	{return REINTERPRET_CAST(CWordAppUi&,*iAppUi);}

CTextPageRegionPrinter& CWordDocument::PageRegionPrinter()const
	{return *iPrint;}

const CWordAppUi& CWordDocument::WordAppUi()const
	{return REINTERPRET_CAST(const CWordAppUi&,*iAppUi);}

TBool CWordDocument::FileNameExists(const TFileName& aFileName)const
	{return aFileName==Process()->MainDocFileName();}

TWordUiData& CWordDocument::UiData()
	{return *iUiData;}
