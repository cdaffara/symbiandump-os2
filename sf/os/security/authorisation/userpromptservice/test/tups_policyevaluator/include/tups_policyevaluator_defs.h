/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file  
 @test
*/

#ifndef TUPSPOLICYEVALUATORDEFS_H
#define TUPSPOLICYEVALUATORDEFS_H

namespace UserPromptService
	{
	
	#ifdef POLICY_01
	
	static const TInt32 KTestPolicyEvaluatorImplementationId = 0xE1111E11;
	static const TInt KPolicyEvaluatorInstance = 1;
	
	#endif
	
	
	#ifdef POLICY_02
	
	static const TInt32 KTestPolicyEvaluatorImplementationId = 0xE1111E21;
	static const TInt KPolicyEvaluatorInstance = 2;
	
	#endif
	
	
	#ifdef POLICY_03
	
	static const TInt32 KTestPolicyEvaluatorImplementationId = 0xE1111E31;
	static const TInt KPolicyEvaluatorInstance = 3;
	
	#endif
	
	
	#ifdef POLICY_NOPROPS
	
	static const TInt32 KTestPolicyEvaluatorImplementationId = 0xE1111E41;
	static const TInt KPolicyEvaluatorInstance = 4;
	
	#endif

	#ifdef POLICY_05
	
	static const TInt32 KTestPolicyEvaluatorImplementationId = 0xE1111E51;
	static const TInt KPolicyEvaluatorInstance = 5;
	
	#endif
	
	}
#endif // TUPSPOLICYEVALUATORDEFS_H