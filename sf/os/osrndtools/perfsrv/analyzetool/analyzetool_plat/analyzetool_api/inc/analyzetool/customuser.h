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
* Description:  Declaration of the class CustomUser containing overloaded User static functions.
*
*/


#ifndef CUSTOMUSER_H
#define CUSTOMUSER_H

// INCLUDES
#include <u32std.h>
#include <e32svr.h>

// CONSTANTS
const TInt KATVersionLength = 20;
const TInt KATDefaultLogOption = 0;
const TInt KATDefaultDebug = 1;
const TInt KATDefaultAllocCallStackSize = 40;
const TInt KATDefaultFreeCallStackSize = 0;
    
// TYPEDEFS
typedef TBuf<KATVersionLength> TATVersion;

// Argument list for SetupThreadHeap function parameters. (currently not used)
// When needed, update the argument type directly inside _LIT macro.
_LIT( KATArgumentList, "%i%i" ); //etc. 

// CLASS DECLARATION

/**
*  Class which overloads the User functions and provides access to 
*  the overloaded functions  
*/
class CustomUser
    {
    public: // Enumerations
        enum TATOptions
            {
            /** Acquiring the log filename */
            ELogFileName = 1,   
            /** Acquiring the version number */
            EVersion,
            /** Acquiring logging option */
            ELogOption,
            /** Acquiring UDEB/UREL information */  
            EDebug,
            /** Acquiring max allocation call stack size */
            EAllocCallStackSize,
            /** Acquiring max free call stack size */
            EFreeCallStackSize,
            /** Acquiring the path of logfile */
            ELogFilePath
            };
        
    public:

        /**
        * Overloaded version of User::Exit()
        * Terminates the current thread, specifying a reason. All child 
        * threads are terminated and all resources are cleaned up.If the 
        * current thread is the main thread in a process, the process is
        * also terminated.
        * @param aReason The reason code.
        */
        IMPORT_C static void Exit( TInt aReason );

        /**
        * Overloaded version of User::Panic()
        * Panics the current thread, specifying a category name and panic
        * number. Keep the length of the category name small;
        * a length of 16 is ideal.
        * @param aCategory A reference to the descriptor containing the text 
        * that defines the category for this panic.
        * @param aReason The panic number. 
        */   
        IMPORT_C static void Panic( const TDesC& aCategory, TInt aReason );

        /**
        * Overloaded version of UserHeap::SetupThreadHeap()
        * Setups the threads heap.
        * @param aNotFirst Is this first thread using specified heap
        * @param aInfo Specifies the thread heap properties
        * @param aFileName The name of the log file
        * @param aLogOption The logging option
        * @param aIsDebug Determines whether a binary is UDEB or UREL
        * @param aVersion Atool version number
        * @param aAllocCallStackSize Max number of stored callstack items when memory allocated
        * @param aFreeCallStackSize Max number of stored callstack items when memory freed
        * @param aFmt A descriptor containing the format string
        * @return TInt KErrNone, if the insertion is successful, otherwise 
        * one of the system wide error codes.
        */   
        IMPORT_C static TInt SetupThreadHeap( 
                             TBool aNotFirst, 
                             SStdEpocThreadCreateInfo& aInfo,
                             const TFileName& aFileName,
                             const TPath& aFilePath,
                             TUint32 aLogOption, TUint32 aIsDebug,
                             const TATVersion& aVersion,
                             TUint32 aAllocCallStackSize,
                             TUint32 aFreeCallStackSize,
                             TRefByValue<const TDesC> aFmt, ... );
                             
        /**
        * Overloaded version of UserHeap::SetCritical()
        * Sets up or changes the effect that termination of the current 
        * thread has, either on its owning process, or on the whole system.
        * The precise effect of thread termination is defined by the following
        *  specific values of the TCritical enum:
        * ENotCritical
        * EProcessCritical
        * EProcessPermanent
        * ESystemCritical
        * ESystemPermanent
        * Notes: The enum value EAllThreadsCritical cannot be set using this
        * function. It is associated with a process, not a thread, and, if 
        * appropriate, should be set using User::SetProcessCritical().
        * The states associated with ENotCritical, EProcessCritical, 
        * EProcessPermanent, ESystemCritical and ESystemPermanent are all 
        * mutually exclusive, i.e. the thread can only be in one of these 
        * states at any one time.
        * @param aCritical The state to be set.
        * @return TInt KErrNone, if successful; KErrArgument, if 
        * EAllThreadsCritical is passed - this is a state associated with a 
        * process, and you use User::SetProcessCritical() to set it.
        */ 
        IMPORT_C static TInt SetCritical( User::TCritical aCritical );
        
        /**
        * Overloaded version of UserHeap::SetCritical()
        * Sets up or changes the effect that termination of subsequently 
        * created threads will have, either on the owning process, 
        * or on the whole system. It is important to note that we are not
        * referring to threads that have already been created, but threads
        * that will be created subsequent to a call to this function.
        * The precise effect of thread termination is defined by the following
        * specific values of the TCritical enum:
        * ENotCritical
        * EAllThreadsCritical
        * ESystemCritical
        * ESystemPermanent
        * Notes:
        * The enum values EProcessCritical and EProcessPermanent cannot be set
        * using this function. They are states associated with a thread, not a
        * process, and, if appropriate, should be set using 
        * User::SetCritical(). The states associated with ENotCritical, 
        * EAllThreadsCritical, ESystemCritical and ESystemPermanent are all 
        * mutually exclusive, i.e. the process can only be in one of these 
        * states at any one time.
        * @param aCritical The state to be set.
        * @return TInt KErrNone, if successful; KErrArgument, if either 
        * EProcessCritical or EProcessPermanent is passed - these are states
        * associated with a thread, and you use User::SetCritical() 
        * to set them.
        */ 
        IMPORT_C static TInt SetProcessCritical( User::TCritical aCritical );
                
    private: // Private functions
        
        /**
        * Factory function for creating RAllocator instances.
        * @param aNotFirst Is this first thread using specified heap
        * @param aLogOption The logging option
        * @param aFileName The name of the logging file
        * @param aIsDebug Determines whether a binary is UDEB or UREL
        * @param aAllocCallStackSize Max number of stored callstack items when memory allocated
        * @param aFreecallStackSize Max number of stored callstack items when memory freed
        * @param aAtoolVersion Version of atool
        * @param aApiVersion  Version of atool's api
        * @return RAllocator& A reference to created allocator
        */  
        static RAllocator& InstallAllocator( TBool aNotFirst,
                                             const TFileName& aFileName,
                                             const TPath& aFilePath,
                                             TUint32 aLogOption, TUint32 aIsDebug,
                                             TUint32 aAllocCallStackSize,
                                             TUint32 aFreeCallStackSize, 
                                             const TDesC8& aAtoolVersion,
                                             const TDesC8& aApiVersion );
        
		/**
		* Check atool version
		* @param aVersion - Atool version number.
		* @param aToolVersion The atool version number
		* @return KErrNone if correct version found, otherwise one of the system wide 
		* error codes.
		*/
        static TInt CheckVersion( const TATVersion& aVersion, TDes& aToolVersion, TDes& aApiVersion ); 
        

        /**
		* Function for showing incorrect version information (file or debug channel).
		* @param aLogOption The logging option
		* @param aFileName The name of the log file
		* @param aToolVersion The atool version number
		*/
        static void ReportIncorrectVersion( const TUint32 aLogOption, 
											const TFileName& aFileName,
											const TPath& aFilePath,
											const TDes& aToolVersion );
        
    };

#endif // CUSTOMUSER_H

// End of File
