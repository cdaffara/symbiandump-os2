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
* Description:  Implementation of ECOM plug-in service interface providing
*               the FTP service.
*
*/


#ifndef HTIFTPSERVICEPLUGIN_H__
#define HTIFTPSERVICEPLUGIN_H__

// INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <HtiServicePluginInterface.h>

// CONSTANTS
enum TFtpCommand
    {
    EFtpSTOR                = 0x02,
    EFtpSTOR_u              = 0x03,
    EFtpRETR                = 0x04,
    EFtpRETR_u              = 0x05,
    EFtpLIST                = 0x06,
    EFtpLIST_u              = 0x07,
    EFtpMKD                 = 0x08,
    EFtpMKD_u               = 0x09,
    EFtpRMD                 = 0x0A,
    EFtpRMD_u               = 0x0B,
    EFtpDELE                = 0x0C,
    EFtpDELE_u              = 0x0D,
    EFtpCANCEL              = 0x0E,
    EFtpFILESIZE            = 0x0F,
    EFtpLISTDIR             = 0x10,
    EFtpLISTDIR_u           = 0x11,
    EFtpLISTSIZES           = 0x12,
    EFtpLISTSIZES_u         = 0x13,
    EFtpLISTDRIVES          = 0x14,
    EFtpLISTDRIVES_u        = 0x15,
    EFtpRENAME              = 0x16,
    EFtpRENAME_u            = 0x17,
    EFtpCOPY                = 0x18,
    EFtpCOPY_u              = 0x19,
    EFtpMOVE                = 0x1A,
    EFtpMOVE_u              = 0x1B,
    EFtpSETFORCE            = 0x20,
    EFtpCHECKSUM            = 0x30,
    EFtpCHECKSUM_u          = 0x31,
    EFtpListDetail          = 0x32,
    EFtpListDetail_u        = 0x33,
    EFtpListDirDetail       = 0x34,
    EFtpListDirDetail_u     = 0x35,
    EFtpFORMAT              = 0x40,
    EFtpOK                  = 0xF0,
    };

enum TAlgorithm
    {
    EMD5 = 0x01,
    };

_LIT(KHtiFileHlp, "HtiFileHlp.exe");

//temporary file used if file is uploaded to Tcb
_LIT(KTmpFileName, "C:\\htitemp.bin");

//tokens used for command line for HtiFileHlp.exe
_LIT( KHtiFileHlpDeleteCmd, "d" );
_LIT( KHtiFileHlpCopyCmd,   "c" );
_LIT( KHtiFileHlpMkdCmd,    "m" );
_LIT( KHtiFileHlpRmdCmd,    "r" );
_LIT( KHtiFileHlpMoveCmd,   "mv" );
_LIT( KHtiFileHlpRenameCmd, "re" );
_LIT( KHtiFileHlpDelim,     "|" );

//tokens used to id Tcb folders
_LIT(KHtiTcbSys, "\\sys\\");
_LIT(KHtiTcbResource, "\\resource\\");

//offset where first folder's backslash starts in absolute filename
                                   // 012
const static TInt KPathOffset = 2; //"C:\"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHtiFtpBackupFakeBase;

// CLASS DECLARATION
/**
* Callback interface for AO
*/
class MFtpObserverAO
    {
public:
    virtual void FtpComplete( TInt anError) = 0;
    };


// CLASS DECLARATION
/**
* AO for async request to CFileMan and RFile
*/
class CFtpHandlerAO : public CActive, public MFileManObserver
    {
public:
    CFtpHandlerAO(MFtpObserverAO* anObserver);
    ~CFtpHandlerAO();

    /** Set AO active */
    void Start();

public: //MFileManObserver
    virtual MFileManObserver::TControl NotifyFileManStarted();
    virtual MFileManObserver::TControl NotifyFileManOperation();
    virtual MFileManObserver::TControl NotifyFileManEnded();

protected: //from CActive
    void RunL();
    /**
    * no direct cancel for RFile and CFileMan
    */
    void DoCancel();
    //TInt RunError(TInt aError);

protected:
    MFtpObserverAO* iObserver;
    /** set to ERtue if CFileMan operation should be canceled */
    TBool iCancelFileMan;
    };


class CProcessLogonAO : public CActive
    {
public:
    CProcessLogonAO(MFtpObserverAO* anObserver);
    ~CProcessLogonAO();

    /** Set AO active */
    void Start(const TDesC& aCmdLine);

protected: //from CActive
    void RunL();
    void DoCancel();

protected:
    MFtpObserverAO* iObserver;
    RProcess iProcess;
    };


// CLASS DECLARATION
/**
* FTP plugin implementation
*/
class CHtiFtpServicePlugin : public CHTIServicePluginInterface,
                             public MFtpObserverAO
    {
protected:
    /**
    * Major states of FPT plug-in
    */
    enum TFtpServiceState
        {
        EIdle,          /** waits for a new command                 */
        EListBusy,      /** waits for memory to send LIST response  */
        ERmdBusy,       /** outstanding request CFileMan::RmDir()   */
        EDeleBusy,      /** outstanding request CFileMan::Delete()  */
        EStorWait,      /** waits for data packages                 */
        EStorBusy,      /** outstanding request to RFile::Write()   */
        ERetrBusy,      /** outstanding request to RFile::Read()    */
        ERetrWait,      /** waits for memory to send data package   */
        EStorTcbBusy,   /** outstandig request to HtiFileHlp.exe    */
        EDeleTcbBusy,   /** outstandig request to HtiFileHlp.exe    */
        EMkdTcbBusy,    /** outstandig request to HtiFileHlp.exe    */
        ERmdTcbBusy,    /** outstandig request to HtiFileHlp.exe    */
        ERenameBusy,    /** outstanding request CFileMan::Rename()  */
        ERenameTcbBusy, /** outstandig request to HtiFileHlp.exe   */
        ECopyBusy,      /** outstanding request CFileMan::Copy()    */
        ECopyTcbBusy,   /** outstandig request to HtiFileHlp.exe   */
        EMoveBusy,      /** outstanding request CFileMan::Move()    */
        EMoveTcbBusy    /** outstandig request to HtiFileHlp.exe   */
        };
public:

    static CHtiFtpServicePlugin* NewL();

    // Interface implementation

    /**
    * Calculates iBufferSize based on iDispatcher->FreeMemory() value
    */
    void InitL();

    /**
    * @return ETrue when in any EXXXBusy state
    */
    TBool IsBusy();

    /**
    * Process either control or data message depending on aPriority
    *
    * @param aMessage message with commands or data
    * @param aPriority indicates type of aMessage
    */
    void ProcessMessageL( const TDesC8& aMessage,
                         THtiMessagePriority aPriority );

    void NotifyMemoryChange( TInt aAvailableMemory );

public: // MFtpObserverAO
    void FtpComplete( TInt anError );

protected:
    CHtiFtpServicePlugin();
    void ConstructL();

    virtual ~CHtiFtpServicePlugin();

    /**
    * Handle FTP control messages
    *
    * @param aMessage message with command
    */
    void HandleControlMessageL( const TDesC8& aMessage );

    /**
    * Handle expected CANCEL control message
    * received in EStorWait or ERetrWait state
    * If aMessage does not contain CANCEL command,
    * BUSY message is sent, otherwise current operation is canceled
    * and plugin goes to EIdle state and sends OK message.
    *
    * @param aMessage message with command
    */
    void HandleCancelL( const TDesC8& aMessage );

    /**
    * Accepts data messages for STOR command
    *
    * @param aMessage message with file data
    */
    void HandleDataMessageL( const TDesC8& aMessage );

    //command handlers

    /**
    * Handle STOR command request
    */
    void HandleReceiveFileL();

    /**
    * Handle RETR command request
    */
    void HandleSendFileL();

    /**
    * Reads one portion of file data during RETR command handeling.
    */
    void ReadToBuffer();

    /**
    * Sends one portion of file data during RETR command handeling.
    */
    void SendBuffer();

    /**
    * Handle LIST command
    *
    * @param aUnicodText if ETrue then response in unicode
    * @param aReadingAtt specifies what entries to read from a dir
    * @param aSizes if ETrue filesizes are included in the response
    */
    void HandleListL( TBool aUnicodText, TUint aReadingAtt, TBool aSizes );
    
    /**
    * Handle LIST FILES DETAIL command
    *
    * @param aUnicodText if ETrue then response in unicode
    * @param aReadingAtt specifies what entries to read from a dir
    */
    void HandleListDetailL( TBool aUnicodText, TUint aReadingAtt);

    /**
    * Extracts and validate file name to iFileName
    * If error sends err msg
    * Return ETrue when filename is valid
    * @param aFilename descriptor with filename
    * @param aToUnicode flag indicates that data in aFilename
    *                   should be treated as an unicode string
    */
    TBool GetFileNameL( const TDesC8& aFilename, TBool aToUnicode );

    /**
    * Extracts and validates path to iFileName
    * If error sends err msg
    * Return ETrue when directory is valid
    * @param aDirname descriptor with directory name
    * @param aToUnicode flag indicates that data in aFilename
    *                   should be treated as an unicode string
    */
    TBool GetDirectoryL( const TDesC8& aDirname, TBool aToUnicode );

    /**
    * Helper to send short control messages
    * (e.g. OK, BUSY, FILESIZE)
    * @param aCmd command code
    * @param aMsg additional command parameters
    * @return KErrNone or Symbian error code
    */
    TInt SendControlMsg( TFtpCommand aCmd, const TDesC8& aMsg );

    /**
    * Helper to send error message
    * @return KErrNone or some system-wide error code
    */
    inline TInt SendErrorMsg( TInt anError, const TDesC8& aMsg );

    /**
    * Handle delete command
    * @param aFilename the file to delete
    */
    void HandleDeleteL( const TDesC& aFilename );

    /**
    * Handle rename command
    * @param aMessage message that contains
    * old and new file/directory names.
    * @param aToUnicode flag indicates that data in aFilename
    *  should be treated as an unicode string
    */
    void HandleRenameL( const TDesC8& aMessage, TBool aUnicode );

    /**
    * Handle copy command
    * @param aMessage message that contains the file/directory to be copied
    * and the location and name of the copy.
    * @param aToUnicode flag indicates that data in aFilename
    *  should be treated as an unicode string
    */
    void HandleCopyL( const TDesC8& aMessage, TBool aUnicode );

    /**
    * Handle move command
    * @param aMessage message that contains the file/directory to be moved
    * and the new location
    * @param aToUnicode flag indicates that data in aFilename
    *  should be treated as an unicode string
    */
    void HandleMoveL( const TDesC8& aMessage, TBool aUnicode );

    /**
    * Safe unicode copying from 8bit descr to 16bit buffer
    * @param aTo destination descriptor
    * @param aFrom source descriptor
    */
    void CopyUnicode( TDes& aTo, const TDesC8& aFrom );

    /**
    * Check either aFilename point in a TCB directories (sys, private, resource)
    * @param aFilename the file to check
    */
    TBool IsFileTcb( const TDesC& aFilename );

    /**
    * Uses HtiFileHlp.exe to delete file from TCB directories
    * @param aFilename the file to delete
    */
    void HandleTcbDeleteL( const TDesC& aFilename );

    /**
    * Uses HtiFileHlp.exe to copy files/folders to TCB directories
    * @param aFromFilename the source path
    * @param aToFilename the destination path
    */
    void HandleTcbCopyL( const TDesC& aFromFilename, const TDesC& aToFilename );

    /**
    * Uses HtiFileHlp.exe to makedir in TCB directories
    * @param aDirname the directory to create
    */
    void HandleTcbMkdL( const TDesC& aDirname );

    /**
    * Uses HtiFileHlp.exe to delete dir in TCB directories
    * @param aDirname the directory to delete
    */
    void HandleTcbRmdL( const TDesC& aDirname );

    /**
    * Uses HtiFileHlp.exe to do rename in TCB folders
    * @param aTargetName the path to rename
    * @param aDestName the new path name
    */
    void HandleTcbRenameL( const TDesC& aTargetName, const TDesC& aDestName );

    /**
    * Uses HtiFileHlp.exe to do move to TCB folders
    * @param aTargetName the path to move
    * @param aDestName the destination path
    */
    void HandleTcbMoveL( const TDesC& aTargetName, const TDesC& aDestName );

    /**
    * Sets the faking of backup/restore operation on/off.
    * If aOn is ETrue sets backup/restore on, otherwise sets it off.
    * @return KErrNone on success, otherwise system error code
    */
    TInt SetBURFakeState( TBool aOn );

    /**
    * Calculates a checksum for the given file with the given algorithm.
    */
    void HandleCheckSumCalcL( TAlgorithm aAlgorithm,
                              const TDesC& aFilename );

    /**
    * Formats a drive.
    */
    void HandleFormat( const TUint8 aDrive, const TUint8 aMode );

    /**
    * Creates a list of drives.
    */
    void HandleListDrivesL( TBool aUnicode );

    /*
     * Adds/Removes application registry information
     */
    void HandleAppRegFileInfoL(const TDesC& aAppRegFile, TBool aAdded);
    
private:
    void RemoveEndBackslash( TFileName& aFileName );

protected:
    /** plugin state */
    TFtpServiceState iState;
    /** File server */
    RFs iFs;
    /** File manager used for RMD and DELE */
    CFileMan* iFileMan;
    /** current file for STOR and RETR*/
    RFile iFile;
    /** file name of iFile */
    TFileName iFileName;

    /** current offset in file during STOR or RETR */
    TInt iCurrentOffset; //mean received bytes or sent bytes
    /** the final size of iFile */
    TInt iFileSize;

    /** the size of iSendBuffer, calculated in InitL() */
    TInt iBufferSize;
    /** send buffer for RETR and LIST commands */
    HBufC8* iSendBuffer;
    /** des of iSendBuffer, used for RETR */
    TPtr8 iSendBufferDes;

    /** AO to handle current asyn request */
    CFtpHandlerAO* iHandlerAO;

    /** used for HtiFileHlp.exe */
    CProcessLogonAO* iProcessLogonAO;

    /**
    * Class for activating/deactivating a backup session.
    * This is used for forced operations to locked files. Activating backup
    * should release locks on files that are in use by some other application.
    */
    CHtiFtpBackupFakeBase* iBackupFake;

    /** Handle to the DLL for activating/deactivating backup */
    RLibrary iBackupFakeLib;
    };

#endif

// End of File
