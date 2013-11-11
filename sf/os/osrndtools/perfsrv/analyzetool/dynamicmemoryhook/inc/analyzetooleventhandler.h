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
* Description:  Declaration of the class CLibraryEventHandler.
*
*/


#ifndef ANALYZETOOLEVENTHANDLER_H
#define ANALYZETOOLEVENTHANDLER_H

// INCLUDES
#include <e32cmn.h>
#include <e32base.h>
#include "codeblock.h"
#include <analyzetool/analyzetool.h>
#include "analyzetoolfilelog.h"

// FORWARD DECLARATIONS
class MAnalyzeToolEventhandlerNotifier;

// CLASS DECLARATION

/**
*  Class for receiving library load/unlaod events from the kernel   
*/
class CLibraryEventHandler : public CActive
    {
    
    public: 

        /**
        * C++ default constructor.
        * @param aAnalyzeTool A reference to the <code>RAnalyzeTool</code> 
                 which is used to observe kernel events 
        * @param aCodeblocks A reference to array of code segments
        * @param aProcessId A reference to the observed process id
        * @param aMutex A reference to mutex to schedule access to the 
        *                   shared resources
        * @param aNotifier A reference to notifier object which is used to 
        * 					inform killed threads
        * @param aLogOption Current used log option on allocator.
        */
        CLibraryEventHandler( RAnalyzeTool& aAnalyzeTool, 
                RArray<TCodeblock>& aCodeblocks,
                TUint aProcessId,
                RMutex& aMutex,
                MAnalyzeToolEventhandlerNotifier& aNotifier,
                TUint32 aLogOption,
                RATFileLog& iLogFile );

        /**
        * Destructor.
        */
        ~CLibraryEventHandler();

        /* Start receiving events from the kernel */
        void Start();
        
        /**
        * Returns eventhandler's state.
        * @return TBool ETrue if eventhandler is started, EFalse otherwise
        */
        TBool IsStarted();

    protected: // Functions from base classes
        
        /**
        * Process active object's task
        */
        void RunL();

        /**
        * Cancels active object's task
        */
        void DoCancel();
 
    private: // Member variables

        /* Handle to the analyze tool device driver*/
        RAnalyzeTool& iAnalyzeTool; 

        /* A reference to codeblocks of the observed process */
        RArray<TCodeblock>& iCodeblocks;

        /* The observered process id */ 
        TUint iProcessId;

        /* The library info */
        TLibraryEventInfo iLibraryInfo;

        /* The mutex for serializing access to the shared resources */
        RMutex& iMutex;
        
        /* Inform if handler is started */
        TBool iStarted;
        
        /* A reference to event handler notifier */
        MAnalyzeToolEventhandlerNotifier& iNotifier;

        /* Current used log option */
        TUint32 iLogOption;
        
        // TODO comment
		/* */
		RATFileLog& iLogFile;
    };

#endif // ANALYZETOOLEVENTHANDLER_H

// End of File
