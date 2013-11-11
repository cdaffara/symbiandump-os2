
// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the post-acquisition content protection test.
// 
//

/**
 @file 
 @internalComponent
 @test
*/

#ifndef TPOSTACQUISITION_H_
#define TPOSTACQUISITION_H_

#include "tscafstep.h"
#include <caf/importfile.h>

// Constant used to name test cases 
_LIT(KScafPostAcquisitionStep,"SCAFPostAcquisition");

class CScafPostAcquisition : public CScafStep
/**
 	Defines the post-acqusition test showing how to record
 	protected content.
 */
	{
public:
	CScafPostAcquisition(CScafServer& aParent);
	~CScafPostAcquisition();
	TVerdict doTestL();
private:
	TInt ProvideNewOutputFileL(ContentAccess::CImportFile& aImport, TDes& aName);
	};
	
#endif /*TPOSTACQUISITION_H_*/
