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


#ifndef PROFILERERRORCHECKER_H_
#define PROFILERERRORCHECKER_H_

// CLASS DECLARATIONS
class MProfilerErrorObserver
    {
    public: // New
        virtual void HandleProfilerErrorChangeL( TInt aError ) = 0;
    };


class CProfilerErrorChecker : public CActive
    {
public:
    static CProfilerErrorChecker* CProfilerErrorChecker::NewL();
    ~CProfilerErrorChecker();
    void SetObserver(MProfilerErrorObserver* aObserver);
private:
    CProfilerErrorChecker();
    void ConstructL();
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();
private:
    MProfilerErrorObserver*     iObserver;
    RProperty                   iErrorStatus;
    };

#endif /* PROFILERERRORCHECKER_H_ */
