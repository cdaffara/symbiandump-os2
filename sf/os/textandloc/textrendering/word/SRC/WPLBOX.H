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


#if !defined(__WPLBOX_H__)
#define __WPLBOX_H__

#if !defined(__EIKTXLBX_H__)
#include <techview/eiktxlbx.h>
#endif

#if !defined(__EIKCLB_H__)
#include <techview/eikclb.h>
#endif

class CWordTextListBox : public CEikTextListBox
/**
@internalComponent
*/
	{
public:	
	TInt MaxWidth();
	};

class CWordSnakingColumnListBox : public CEikSnakingTextListBox
/**
@internalComponent
*/
	{
public:
	void ConstructL(const CCoeControl* aParent,TInt aFlags);
	CTextListBoxModel* Model() const;
	CColumnListBoxItemDrawer* ItemDrawer() const;
	TSize BorderSize() const;
	};

#endif
