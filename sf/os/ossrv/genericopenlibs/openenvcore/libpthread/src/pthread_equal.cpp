// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name     : pthread_equal.cpp
// Part of  : PThread library
// pthread_equal call is implemented.
// Version:
//



#include <pthread.h>

EXPORT_C int pthread_equal(pthread_t t1,pthread_t t2)
{
    return (t1 == t2);
}
// End of File
