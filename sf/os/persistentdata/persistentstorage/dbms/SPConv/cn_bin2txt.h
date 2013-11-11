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
// CSPBin2Txt class
// 
//

#ifndef __CN_BIN2TXT_H__
#define __CN_BIN2TXT_H__

#include "cn_proc.h"

//Forward declarations
struct TCmdLinePrm;
class TCmdProcessorFactory;
class RFs;

/**
CSPBin2Txt class implements CCmdProcessor interface and describes an object which 
takes a binary policy file as an input, loads all policies in memory and then stores
them in a text file. 
@see CCmdProcessor
@see TCmdProcessorFactory
@see TCmdLinePrm
@internalComponent
*/
class CSPBin2Txt : private CCmdProcessor
	{
	friend class TCmdProcessorFactory;

public:
	virtual void RunL();
	virtual ~CSPBin2Txt();

private:
	inline CSPBin2Txt(RFs& aFs, const TCmdLinePrm& aCmdLinePrm);
	static CSPBin2Txt* NewLC(RFs& aFs, const TCmdLinePrm& aCmdLinePrm);
	void ConstructL();

	};

#endif//__CN_BIN2TXT_H__
