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
//

/**
 @file
 @internalComponent
 @test   
*/

#ifndef TSCAF_STREAMAGENTDEFS_H_
#define TSCAF_STREAMAGENTDEFS_H_

namespace StreamAccess
	{
	
	#ifdef STREAMAGENT_01
	// Test Stream Agent Factory Implementation UID
	static const TUint KTestStreamAgentFactoryImpId = 0x10285891;
	
	// Supported KMS Id's
	_LIT8(KSupportedKmsIds,"1002;1005;1008;");
	#endif
	
	
	#ifdef STREAMAGENT_02
	// Test Stream Agent Factory Implementation UID
	static const TUint KTestStreamAgentFactoryImpId = 0x10285893;
	
	// Supported KMS Id's
	_LIT8(KSupportedKmsIds,"1003;1004;1006;");
	#endif
	
	}

#endif /*TSCAF_STREAMAGENTDEFS_H_*/
