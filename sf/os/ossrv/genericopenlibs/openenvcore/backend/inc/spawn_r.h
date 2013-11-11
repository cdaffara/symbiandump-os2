/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _SPAWN_R_H_
#define _SPAWN_R_H_

#ifdef __cplusplus
extern "C" {
#endif

enum TFileOp
	{
	EOpen,
	EClose,
	EDup
	};

struct TFileAction
	{
	TFileOp iOp;
	TInt iFid1;
	TInt iFid2;
	TInt iOFlag;
	TInt iMode;
	TFileName* iPath;
	TSglQueLink iLink;
	};	

struct file_actions_t
	{
	TSglQue<TFileAction> iActions;
	TSglQueIter<TFileAction> iIter;
	
	file_actions_t() : iActions(_FOFF(TFileAction, iLink)), iIter(iActions)
		{
		// nada
		}
	};

#ifdef __cplusplus
}
#endif

#endif /* _SPAWN_R_H_ */
