// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : extraExports.cpp
// Part of     : 

// This test is to verify the behaviour of RVCT compiler on Key-functions (i.e., the first 
// non-pure, non-inlined virtual function of that class) in a templated function. 
// This test-dll is created just to verify, hence will not freeze this test-dll's exports.

#include <sstream>

EXPORT_C void test()
{
    std::stringstream test;
}
