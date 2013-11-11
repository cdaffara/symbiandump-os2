/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
#ifndef PROFILERESHELL_H_
#define PROFILERESHELL_H_

#include <e32base.h>
#include <e32std.h>  
#include <bacline.h>    // CCommandLineArguments

class CProfilerEShell : public CBase
    {
public:
    static CProfilerEShell* NewL();
    ~CProfilerEShell();
    
    void ConstructL();
private:
    CProfilerEShell();
    
    };

#endif /* PROFILERESHELL_H_ */
