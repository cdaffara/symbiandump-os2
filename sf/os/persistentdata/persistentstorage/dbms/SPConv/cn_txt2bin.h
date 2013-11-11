// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CSPTxt2Bin class
// 
//

#ifndef __CN_TXT2BIN_H__
#define __CN_TXT2BIN_H__

#include "cn_proc.h"

//Forward declarations
struct TCmdLinePrm;
class TCmdProcessorFactory;
class RFs;

/**
CSPTxt2Bin class implements CCmdProcessor interface and describes an object which 
takes a text policy file as an input, loads all policies in memory and then stores
them in a binary policy file. 
@see CCmdProcessor
@see TCmdProcessorFactory
@see TCmdLinePrm
@internalComponent
*/
class CSPTxt2Bin : private CCmdProcessor
	{
	friend class TCmdProcessorFactory;

public:
	virtual void RunL();
	virtual ~CSPTxt2Bin();

private:
	inline CSPTxt2Bin(RFs& aFs, const TCmdLinePrm& aCmdLinePrm);
	static CSPTxt2Bin* NewLC(RFs& aFs, const TCmdLinePrm& aCmdLinePrm);
	void ConstructL();

	};

#endif//__CN_TXT2BIN_H__
