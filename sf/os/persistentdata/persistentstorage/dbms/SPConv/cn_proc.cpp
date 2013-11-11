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
// TCmdProcessorFactory class.
// 
//

#include "cn_main.h"
#include "cn_bin2txt.h"
#include "cn_txt2bin.h"

/**
Factory method for CCmdProcessor interface implementations.
It creates and returns either CSPExport or CSPImport instance, depending on
the value of aCmdLinePrm.iAction data member.
@param aFs File server session object reference.
@param aCmdLinePrm It contains parsed command line arguments.
@return A pointer to just created CCmdProcessor interface implementation.
@leave CSPBin2Txt::NewLC() and CSPTxt2Bin::NewLC() leaving error codes.
@see CCmdProcessor
@see CSPBin2Txt
@see CSPTxt2Bin
*/
CCmdProcessor* TCmdProcessorFactory::NewLC(RFs& aFs, const TCmdLinePrm& aCmdLinePrm)
	{
	CCmdProcessor* proc = NULL;
	if(aCmdLinePrm.iAction == TCmdLinePrm::EBin2Txt)
		{
		proc = CSPBin2Txt::NewLC(aFs, aCmdLinePrm);
		}
	else if(aCmdLinePrm.iAction == TCmdLinePrm::ETxt2Bin)
		{
		proc = CSPTxt2Bin::NewLC(aFs, aCmdLinePrm);
		}
	return proc;
	}
