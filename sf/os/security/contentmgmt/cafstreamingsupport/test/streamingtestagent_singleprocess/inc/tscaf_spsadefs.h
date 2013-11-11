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
 @test 
*/

#ifndef TSCAF_SPSADEFS_H_
#define TSCAF_SPSADEFS_H_

namespace StreamAccess
	{
	
	#ifdef STREAMAGENT_SP_01
	// Test Stream Agent Factory Implementation UID
	static const TUint KTestStreamAgentFactoryImpId = 0x10285A41;
	
	// Supported KMS Id's
	_LIT8(KSupportedKmsIds,"1234;2468;9876;");
	#endif
	
	}

#endif /*TSCAF_SPSADEFS_H_*/
