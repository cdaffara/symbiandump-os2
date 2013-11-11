// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __BACOMPILEASSERT_H__
#define __BACOMPILEASSERT_H__

template<TBool> class __CompileTimeAssert 
	{
	public: 
		inline static void Dummy() 
			{
			}
	};

TEMPLATE_SPECIALIZATION class __CompileTimeAssert<0> 
	{
	private: 
		inline static void Dummy() 
			{
			}
	};

#define COMPILE_TIME_ASSERT(aCondition)		\
	{										\
	__CompileTimeAssert<(aCondition)>::Dummy();\
	}

#endif// __BACOMPILEASSERT_H__
