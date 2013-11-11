/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <fcntl.h>
#include <sys/errno.h>
#include <sys/serial.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/syslimits.h>
#include <utf.h>
#include "sysreent.h"
#include <sys/socket.h>
#include <sys/un.h>
#include "sysif.h"
#include "fdesc.h"
#include "ltime.h"
#include "lposix.h"
#include "systemspecialfilercg.h"
#include "link.h"
#ifdef SYMBIAN_OE_POSIX_SIGNALS
#include <stdlib.h>
#include <signal.h>
#include "tsignalmessage.h"
#endif // SYMBIAN_OE_POSIX_SIGNALS


#ifdef SYMBIAN_OE_POSIX_SIGNALS
#define SIG_SIGNAL_PRESENT_IN_SIGSET(sig,set) ((set & (1ULL << (sig-1))) != 0x0000000000000000ULL)
#define SIG_ISEMPTY_SIGSET(set) (set == 0x0000000000000000ULL)
#define SIG_EMPTY_SIGSET(set) {set = 0x0000000000000000ULL;}
#define SIG_FILL_SIGSET(set) {set = 0xFFFFFFFFFFFFFFFFULL;}
#define SIG_ADD_SIGNAL_TO_SET(sig,set) {set |= (1ULL << (sig-1));}
#define SIG_REMOVE_SIGNAL_FROM_SET(sig,set) {set &= ~(1ULL << (sig-1));}

static void sig_exit();

const TUint KSignalPipeSize = 1024;
#endif


#ifdef __WINSCW__
#include <pls.h> // For emulator WSD API
const TUid KBackendUid3 = {0x102828D5};
#elif defined __X86GCC__
// GCC compiler generates an undefined reference error if static CLocalSystemInterface object is created in a local function. Therefore give it global scope on this platform.
CLocalSystemInterface TheBackend;
#elif defined __EPOC32__
//Globally declaring the CLSI object to overcome the DCLP implementation if given a static scope.
CLocalSystemInterface backend;
#endif

const TUint KConvertToMicroSecond = 1000000;

// Create static singleton backend object for the Process and return the same
EXPORT_C CLocalSystemInterface* Backend()
		{
#ifdef __WINSCW__
		// Access the PLS of this process
		CLocalSystemInterface* backend = Pls<CLocalSystemInterface>(KBackendUid3);
		return backend;
#elif defined __X86GCC__
		return &TheBackend;
#else
		return &backend;
#endif
		}
		
EXPORT_C RTz& CLocalSystemInterface::TZServer(TInt &aStatus)
	{
	aStatus = OnDemandTZServerConnection();
	return iTzServer;
	} 
		
TInt CLocalSystemInterface::OnDemandTZServerConnection()
        {
		TInt v = EFalse;
		TInt err = KErrNone;
		
		if(__e32_atomic_load_acq32(&iIsRTzConnected))
			return err;
			
		iTzServerLock.Wait();
		if (!iIsRTzConnected) 
			{
			err = iTzServer.Connect();
			if ( err == KErrNone ) 
				{
				err = iTzServer.ShareAuto();
				if( err == KErrNone) {
					v = ETrue;
					}
				}
			__e32_atomic_store_rel32(&iIsRTzConnected, v); 
			}
		iTzServerLock.Signal();
		
        return err;
        } 

// Construction of Backend Object which is going to be singleton object for the process
EXPORT_C CLocalSystemInterface::CLocalSystemInterface() : iOpenDirList(CLocalSystemInterface::KDirGran),
iTLDInfoList(CLocalSystemInterface::KTLDInfoListGran), iDefConnResurrect(ETrue), iDefConnPref(NULL), iIsRTzConnected(EFalse)
		{
#ifdef SYMBIAN_OE_POSIX_SIGNALS
		iSignalsInitialized = EFalse;
		iSigInitWaitCount = 0;
		iSigInitWaitMutex.CreateLocal();
		iSigInitWaitSemaphore.CreateLocal(0);
		iBlockedSAMutex.CreateLocal();
		iSignalWaiterMutex.CreateLocal();
		iSignalInitSemaphore.CreateLocal(0);
#endif
		//Create the Heap for all the libraries (local to this process)
		const TInt KMaxHeapSize = 1048576 ; // 1MB
		iPrivateHeap = UserHeap::ChunkHeap(NULL, KMinHeapSize, KMaxHeapSize);
		if (!iPrivateHeap)
			{
			User::Panic(KEstlibInit, KErrNoMemory);
			}

		// Switch to the backend private heap
		RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);

		TInt err = iFids.Init(iPrivateHeap);
		if ( !err )
			{
			CRedirDesc* redirMedia = new CRedirDesc();
			if (redirMedia)
				{
				CStdErrDesc* errMedia = new CStdErrDesc();
				if(errMedia)
					{
					iFids.Default(redirMedia, errMedia);
					redirMedia->Close();
					errMedia->Close();
					}
				else
					{
					iPrivateHeap->Free(redirMedia);
					}
				}
			}
#if (defined SYMBIAN_OE_POSIX_SIGNALS && defined SYMBIAN_OE_LIBRT)
		iTimerOverrunsMutex.CreateLocal(); //use backend heap
#endif

		// Reset to the thread's default heap
		User::SwitchHeap(oldHeap);

		if (!err)
			{
			err=iFs.Connect();
			if(!err)
				{
				err = iFs.ShareAuto();
				}
			}

		if (!err)
			{
			err = PosixFilesystem::SetDefaultDir(iFs);
			}

		//Cache the session path to avoid querying RFs every time it is needed
		if(err == KErrNone)
			{
			err = iFs.SessionPath(iSessionPath);
			}


		if(err == KErrNone)
			{	
			//Create Server Locks
			err |= iSSLock.CreateLocal();
			err |= iCSLock.CreateLocal();
			err |= iIpcS.iLock.CreateLocal();
			//Create TLDList Lock
			err |= iTLDListLock.CreateLocal();
			//Create the lock for Session Path
			err |= iSessionPathLock.CreateLocal();
			//Lock for protecting iASelectRequest across threads
			err |=  iASelectLock.CreateLocal();
	        //Protect the iDefConnection from concurrent GetDefaultConnection calls
	        err |= iDefConnLock.CreateLocal();
			//Protect the time zone server while connecting
			err |= iTzServerLock.CreateLocal();
			}

		//Panic if any of the above operation returns with error
		if (err)
			{
			User::Panic(KEstlibInit, err);
			}

		iCleanup.StorePtrs(iPrivateHeap, &iFs, &iSs, &iCs, &iSSLock, &iCSLock,&iDefConnLock,&iASelectLock,&iTzServerLock);

		}

EXPORT_C CLocalSystemInterface::~CLocalSystemInterface()
//
// Shut down all server connections in use
//
	{
	iTLDListLock.Close();
	iSessionPathLock.Close();
	
	//close the default RConnection
	if(iDefConnection.SubSessionHandle() != 0)
	    {
		iDefConnection.Close();
	    }

	RHeap* oHeap = User::SwitchHeap(iPrivateHeap);
	for (TInt i = 0, count = iTLDInfoList.Count(); i < count; i++ )
		{
		iTLDInfoList[i].Close(iPrivateHeap);
		}

	// close the RArray itself
	iTLDInfoList.Close();
	// Cleanup the threads serving aselect requests
	int err;
	// passing 1 to cancelaselect will kill all the threads serving aselect
	cancelaselect(NULL,err,1);

	// Close the array that maintains aselect request details 
	iASelectRequest.Close();
	//close the RTz connection
	iTzServer.Close();


	if( iDefConnPref )
	    {
	    switch( iDefConnPref->ExtensionId() )
	        {
	        case TConnPref::EConnPrefSnap:
	            {
	            delete (TCommSnapPref*)iDefConnPref;
	            iDefConnPref = NULL;              
	            }
	            break;

	        case TConnPref::EConnPrefCommDb:
	            {
	            delete (TCommDbConnPref*)iDefConnPref;
	            iDefConnPref = NULL;
	            }
	            break;              

	        default:
	            {
                // Unknown type of Connection Pref, do nothing
	            }
	        }
	    }

#if (defined SYMBIAN_OE_POSIX_SIGNALS && defined SYMBIAN_OE_LIBRT)
	iTimerOverrunsMutex.Close();	
	iTimerOverruns.Close(); 
#endif	
	
#if (defined SYMBIAN_OE_POSIX_SIGNALS)
	iSigInitWaitMutex.Close();
	iSigInitWaitSemaphore.Close();
	iBlockedSAMutex.Close();
	iSignalWaiterMutex.Close();
	iSignalInitSemaphore.Close();
#endif
	//close the RpointerArray
	iOpenDirList.Close();
	
	iSSLock.Close();
	iCSLock.Close();
	iSignalSession.Close();
	iIpcS.Close();
		
	User::SwitchHeap(oHeap);
	}

#ifdef __SYMBIAN_COMPILE_UNUSED__
CLocalSystemInterface& CLocalSystemInterface::Clone()
	{
	return *this;
	}

void CLocalSystemInterface::Release()
	{
	// nada. TODO: Remove this method
	}


// -----------------------------------------------------------------------------
// CLocalSystemInterface::TerminateProcess
// Sends a message to Posix Server to Terminate itself!
// And delete the Process (self)
// -----------------------------------------------------------------------------
//
void CLocalSystemInterface::TerminateProcess(int status)
	{
	//delete this;
	RProcess().Terminate(status);
	}
#endif //__SYMBIAN_COMPILE_UNUSED__


// -----------------------------------------------------------------------------
// CLocalSystemInterface::Exit
// Close all the open FIDS,Terminate the process
//
// -----------------------------------------------------------------------------
//
void CLocalSystemInterface::Exit(int code)
	{
#ifdef SYMBIAN_OE_POSIX_SIGNALS
    if(iSignalsInitialized)
        {
        TRequestStatus status = KRequestPending;
        iSignalHandlerThread.Logon(status);
        iSignalLoopRunning = EFalse;
        iSignalHandlerThread.RequestSignal();
        User::WaitForRequest(status);
        }
#endif
	iFids.Close();
	User::SetCritical(User::EProcessPermanent);
	User::Exit(code);
	}

// Simple layer over PosixFilesystem

wchar_t * CLocalSystemInterface::getcwd (wchar_t* buf, unsigned long len, int& anErrno)
	{
	return PosixFilesystem::getcwd(iFs, buf, len, anErrno);
	}

int CLocalSystemInterface::chdir (const wchar_t* aPath, int& anErrno)
	{
	return PosixFilesystem::chdir(iFs, aPath, anErrno);
	}

int CLocalSystemInterface::rmdir (const wchar_t* aPath, int& anErrno)
	{
	return PosixFilesystem::rmdir(iFs, aPath, anErrno);
	}

int CLocalSystemInterface::mkdir (const wchar_t* aPath, int perms, int& anErrno)
	{
	return PosixFilesystem::mkdir(iFs, aPath, perms, anErrno);
	}

int CLocalSystemInterface::stat (const wchar_t* name, struct stat *st, int& anErrno)
    {    
    const wchar_t* filename  = name;
    // This needs to be zero terminated
    TBuf<KMaxFileName> inputName;
    TUint pathAtt = 0;
    TInt err = GetFullFile(inputName,(const TText16*)name,iFs);
    if( !err )
        {
        TInt err = iFs.Att(inputName,pathAtt);
        if ( (err == KErrNone) && (pathAtt & KEntryAttDir) )
            {                    
            inputName.Append(_L("\\"));            
            }
        filename = (wchar_t*)inputName.PtrZ();
        }
    TSpecialFileType fileType;
    struct SLinkInfo enBuf;
    // Check the type of file
    fileType = _SystemSpecialFileBasedFilePath(filename, err, iFs);
    // If it is a symbolic link, follow the link
    // If _SystemSpecialFileBasedFilePath fails, treat it as normal file
    // and try to proceed
    if( fileType == EFileTypeSymLink && err == KErrNone )
        {
        err = _ReadSysSplFile(filename, (char*)&enBuf, sizeof(struct SLinkInfo), anErrno, iFs);
        if (err == KErrNone)
            {
            filename = (wchar_t*)enBuf.iParentPath;
            }
        else
            {
            // errno is already set by _ReadSysSplFile
            return -1;
            }
        }
    else if ( fileType != EFileGeneralError && err != KErrNone )
        {
        return MapError(err,anErrno);
        }
    return PosixFilesystem::statbackend(iFs, filename, st, anErrno);
    }

int CLocalSystemInterface::lstat (const wchar_t* name, struct stat *st, int& anErrno)
    {
    return PosixFilesystem::statbackend(iFs, name, st, anErrno);
    }

int CLocalSystemInterface::utime (const wchar_t* name, const struct utimbuf *filetimes, int& anErrno)
	{
	return PosixFilesystem::utime(iFs, name, filetimes, anErrno);
	}

int CLocalSystemInterface::chmod (const wchar_t* name, int perms, int& anErrno)
	{
	return PosixFilesystem::chmod(iFs, name, perms, anErrno);
	}

int CLocalSystemInterface::rename (const wchar_t* oldname, const wchar_t* newname, int& anErrno)
	{
	return PosixFilesystem::rename(iFs, oldname, newname, anErrno);
	}


EXPORT_C TInt CLocalSystemInterface::ResolvePath (TParse& aResult, const wchar_t* path, TDes* aFilename)
	{
	return PosixFilesystem::ResolvePath(aResult, path, aFilename);
	}

// Simple layer over CFileTable synchronous routines

int CLocalSystemInterface::open (const wchar_t* name, int mode, int perms, int& anErrno)
	{
	return iFids.open(name,mode,perms,anErrno);
	}

int CLocalSystemInterface::dup (int fid, int& anErrno)
	{
	return iFids.dup(fid,anErrno);
	}

int CLocalSystemInterface::dup2 (int fid, int fid2, int& anErrno)
	{
	return iFids.dup2(fid,fid2,anErrno);
	}

int CLocalSystemInterface::close (int fid, int& anErrno)
	{
	return iFids.UserClose(fid,anErrno);
	}

off_t CLocalSystemInterface::lseek (int fid, off_t offset, int whence, int& anErrno)
	{
	return iFids.lseek(fid,offset,whence,anErrno);
	}

int CLocalSystemInterface::fstat (int fid, struct stat *st, int& anErrno)
	{
	return iFids.fstat(fid,st,anErrno);
	}

int CLocalSystemInterface::socket (int family, int style, int protocol, int& anErrno)
	{
	//Acquire the Lock before issuing a request to the server and release it later
	iSSLock.Wait();
	int retVal = iFids.socket(family,style,protocol,anErrno,iSs);
	iSSLock.Signal();
	return retVal;
	}

int CLocalSystemInterface::listen (int fid, int n, int& anErrno)
	{
	return iFids.listen(fid,n,anErrno);
	}

int CLocalSystemInterface::bind (int fid, const struct sockaddr* addr, unsigned long size, int& anErrno)
	{
	return iFids.bind(fid,addr,size,anErrno);
	}

int CLocalSystemInterface::sockname (int fid, struct sockaddr* addr, unsigned long* size, int anEnd, int& anErrno)
	{
	return iFids.sockname(fid,addr,size,anEnd,anErrno);
	}

int CLocalSystemInterface::getsockopt (int fid, int level, int opt, void* buf, unsigned long* len, int& anErrno)
	{
	return iFids.getsockopt(fid,level,opt,buf,len,anErrno);
	}

int CLocalSystemInterface::setsockopt (int fid, int level, int opt, void* buf, unsigned long len, int& anErrno)
	{
	return iFids.setsockopt(fid,level,opt,buf,len,anErrno);
	}

int CLocalSystemInterface::popen3 (const wchar_t* file, const wchar_t* cmd,
		wchar_t** env, int fids[3], int& anErrno)
	{
	return iFids.popen3(file, cmd, env, fids, anErrno);
	}

int CLocalSystemInterface::waitpid (int pid, int* status, int options, int& anErrno)
	{
	return iFids.waitpid(pid, status, options, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::popen
// Implementation for popen
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::popen (const wchar_t* file, const wchar_t* cmd,
		const char* mode, int& anErrno)
	{
	return iFids.popen(file, cmd, mode, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::pclose
// Implementation for pclose
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::pclose (int aFid, int& anErrno)
	{
	return iFids.pclose(aFid, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::CheckOrigins
// Implementation for CheckPipe
// -----------------------------------------------------------------------------
//
EXPORT_C void CLocalSystemInterface::CheckOrigins (wchar_t**& wenvp, int& aCount)
	{
	//Acquire the Lock before issuing a request to the server and release it later
#ifdef SYMBIAN_OE_POSIX_SIGNALS
	iFids.CheckOrigins(wenvp, aCount, iSigMask);
#else
	iFids.CheckOrigins(wenvp, aCount);
#endif
	}

#ifdef SYMBIAN_OE_POSIX_SIGNALS
// -----------------------------------------------------------------------------
// CLocalSystemInterface::WaitOrHandleSignal
// -----------------------------------------------------------------------------
//
void CLocalSystemInterface::WaitOrHandleSignal(TSignalMessage aMsg)
	{
	TInt lSigNum = 0,lIdx = 0,lSigVal = 0;
	TSignalWaiter lTempWaiter(0,0);

	if(aMsg.mType == TSignalMessage::ESignal)
		lSigNum = aMsg.mSignal;
	else if(aMsg.mType == TSignalMessage::ESignalValuePair)
		{
		lSigNum = aMsg.mSigVal.mSignal;
		lSigVal = aMsg.mSigVal.mValue;
		}
	else if(aMsg.mType == TSignalMessage::ERtTimerSignal)
		{
		lSigNum = aMsg.mRtSignal.mSignal;
		}

	// check and deliver to waiter (if any)
	TUint64 lMask;
	SIG_EMPTY_SIGSET(lMask);
	SIG_ADD_SIGNAL_TO_SET(lSigNum,lMask);
	lTempWaiter.iSigWaitMask = lMask;

	lIdx = iSignalWaiterArray.Find(lTempWaiter,TSignalWaiter::MatchByMask);
	if(lIdx != KErrNotFound)
		{
		iSignalWaiterMutex.Wait();
		iSignalWaiterArray[lIdx].iSigWaitMessagePtr->mType = aMsg.mType;
		if(aMsg.mType == TSignalMessage::ESignal)
			iSignalWaiterArray[lIdx].iSigWaitMessagePtr->mSignal = lSigNum;
		else if(aMsg.mType == TSignalMessage::ESignalValuePair)
			{
			iSignalWaiterArray[lIdx].iSigWaitMessagePtr->mSigVal.mSignal = lSigNum;
			iSignalWaiterArray[lIdx].iSigWaitMessagePtr->mSigVal.mValue = lSigVal;
			}
		iSignalWaiterArray[lIdx].iSigWaitSemaphore.Signal();
		RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);
		iSignalWaiterArray.Remove(lIdx);
		User::SwitchHeap(oldHeap);
		iSignalWaiterMutex.Signal();
		}
	// queueing if blocked or sigqueue
	else
		{
		HandleSignal(aMsg);
		}
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::SignalHandler
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::SignalHandler()
	{
	// TODO: Put memory barrier to ensure SMP safety
	TInt lRetVal = KErrGeneral;
	InitializeSignalHandlers();
	RPipe::Init(); // check return value
	lRetVal = RPipe::Create(KSignalPipeSize,iSignalReadPipe,iSignalWritePipe);
	if(lRetVal != KErrNone)
		{
		iSignalInitSemaphore.Signal();
		return lRetVal;
		}
	lRetVal = iSignalSession.SendPipe(iSignalWritePipe); // check return value
	if(lRetVal != KErrNone)
		{
		iSignalInitSemaphore.Signal();
		return lRetVal;
		}
	
	/* Closing the end of the pipe that's been sent to the server */
	iSignalWritePipe.Close();
	
	iSignalsInitialized = ETrue;
	iSignalInitSemaphore.Signal();
	iSignalLoopRunning = ETrue;
	iPipeReadStatus = KRequestPending;
	iSignalReadPipe.NotifyDataAvailable(iPipeReadStatus);
	while(iSignalLoopRunning)
		{
		User::WaitForAnyRequest();
		if(iSignalLoopRunning == false) {
            break;
		}
		// Check if it is a pipe read
		if(iPipeReadStatus != KRequestPending)
			{
			if(iPipeReadStatus == KErrNone)
				{
				TBuf8<KSigMsgLength> lMsgBuf;
				TInt lRetVal = KErrNone;
				lRetVal = iSignalReadPipe.Read(lMsgBuf,KSigMsgLength);
				TSignalMessage lMsg;
				TInt lIdx = 0;

				if(lMsg.Unmarshall(lMsgBuf)==KErrNone && lRetVal >= 0)

					{
					TInt lIdx1 = 0, lIdx2 = 0, lIdx3 = 0;
					TSignalMessage lTmpMsg1,lTmpMsg2,lTmpMsg3;
					TBool lSignalsFound = ETrue;

					TChildWaiter lWaiter;

					switch(lMsg.mType)
						{
						case TSignalMessage::ESignalValuePair:
						case TSignalMessage::ESignal:
						case TSignalMessage::ERtTimerSignal:
							WaitOrHandleSignal(lMsg);
							break;
						case TSignalMessage::EDequeueSignal:
							lTmpMsg1.mType = TSignalMessage::ESignal;
							lTmpMsg1.mSignal = lMsg.mSignal;

							lTmpMsg2.mType = TSignalMessage::ESignalValuePair;
							lTmpMsg2.mSigVal.mSignal = lMsg.mSignal;

							lTmpMsg3.mType = TSignalMessage::ERtTimerSignal;
							lTmpMsg3.mRtSignal.mSignal = lMsg.mSignal;
							while(lSignalsFound)
								{
								iBlockedSAMutex.Wait();
								lIdx1 = iBlockedSigArray.Find(lTmpMsg1,TSignalMessage::SignalMatchBySigNum);
								lIdx2 = iBlockedSigArray.Find(lTmpMsg2,TSignalMessage::SigValMatchBySigNum);
								lIdx3 = iBlockedSigArray.Find(lTmpMsg3,TSignalMessage::SigValMatchBySigNum);
								iBlockedSAMutex.Signal();
								if(lIdx1 == KErrNotFound && lIdx2 == KErrNotFound)
									{
									lSignalsFound = EFalse;
									}
								else
									{
									if(lIdx1 != KErrNotFound)
										{
										HandleSignal(lTmpMsg1,ETrue);
										iBlockedSAMutex.Wait();
										iBlockedSigArray.Remove(lIdx1);
										iBlockedSAMutex.Signal();
										}
									if(lIdx2 != KErrNotFound)
										{
										HandleSignal(lTmpMsg2,ETrue);
										iBlockedSAMutex.Wait();
										iBlockedSigArray.Remove(lIdx2);
										iBlockedSAMutex.Signal();
										}
									if(lIdx3 != KErrNotFound)
										{
										HandleSignal(lTmpMsg3, ETrue);
										iBlockedSAMutex.Wait();
										iBlockedSigArray.Remove(lIdx3);
										iBlockedSAMutex.Signal();
										}
									}
								}
							break;
						case TSignalMessage::EAlarmRegistration:
							if((iAlarmStatus == KRequestPending) && iIsTimerActive)
								{
								iAlarmTimer.Cancel();
								iIsTimerActive = EFalse;
								}
							if(lMsg.mTimeOut)
								{
								iAlarmTimer.After(iAlarmStatus,lMsg.mTimeOut*1000000);
								iAlarmStartTime.HomeTime();
								iIsTimerActive = ETrue;
								}
							break;
						case TSignalMessage::EWaitOnChild:
							lWaiter.iWaiterPid = TProcessId(lMsg.mPid);
							lWaiter.iWaiterStatus = KRequestPending;
							lIdx = iChildWaiterArray.Find(lWaiter,TChildWaiter::MatchByPid);
							if(lIdx == KErrNotFound)
								{
								lWaiter.iWaiterPid = TProcessId(lMsg.mPid);
								lWaiter.iWaiterStatus = KRequestPending;
								iChildWaiterArray.Append(lWaiter);
								lIdx = iChildWaiterArray.Find(lWaiter,TChildWaiter::MatchByPid);
								if(lIdx != KErrNotFound)
									{
									RProcess lProcess;
									if(lProcess.Open(TProcessId(iChildWaiterArray[lIdx].iWaiterPid)) == KErrNone)
										{
										lProcess.Logon(iChildWaiterArray[lIdx].iWaiterStatus);
										}
									}
								}
							else
								{
								TProcessId lPid(iChildWaiterArray[lIdx].iWaiterPid);
								iChildWaiterArray[lIdx].iWaiterStatus = KRequestPending;
								RProcess lProcess(lPid);
								lProcess.Logon(iChildWaiterArray[lIdx].iWaiterStatus);
								}
							break;
						default:
							break;
						}
					}
				}
			iPipeReadStatus = KRequestPending;
			iSignalReadPipe.NotifyDataAvailable(iPipeReadStatus);
			}
		if((iAlarmStatus != KRequestPending) && iIsTimerActive)
			{
			if(iAlarmStatus == KErrNone)
				{
				TSignalMessage lMsg;
				lMsg.mType = TSignalMessage::ESignal;
				lMsg.mSignal = SIGALRM;
				WaitOrHandleSignal(lMsg);
				}
			iIsTimerActive = EFalse;
			}
		TBool lChildCheckRunning = ETrue;
		while(lChildCheckRunning)
			{
			TChildWaiter lWaiter;
			lWaiter.iWaiterStatus = KErrNone; // dummy waiter to match non pending waiters
			TInt lTimerIdx = iChildWaiterArray.Find(lWaiter,TChildWaiter::MatchNotPending);
			if(lTimerIdx != KErrNotFound)
				{
				TSignalMessage lSigChldMsg;
				lSigChldMsg.mType = TSignalMessage::ESignal;
				lSigChldMsg.mSignal = SIGCHLD;
				TBuf8<KSigMsgLength> lMsgBuf;
				lSigChldMsg.Marshall(lMsgBuf);
				iSignalWritePipe.Write(lMsgBuf,KSigMsgLength);
				iChildWaiterArray.Remove(lTimerIdx);
				}
			else
				{
				lChildCheckRunning = EFalse;
				}
			}
		}
	if(iPipeReadStatus == KRequestPending)
		{
		iSignalReadPipe.CancelDataAvailable();
		}
	iSignalReadPipe.Close();
	
	if(iAlarmStatus == KRequestPending)
		{
		iAlarmTimer.Cancel();
		}
	TInt lCounterIdx = 0;
	for(lCounterIdx = 0; lCounterIdx < iChildWaiterArray.Count(); lCounterIdx++)
		{
		if(iChildWaiterArray[lCounterIdx].iWaiterStatus == KRequestPending)
			{
			RProcess lProcess;
			if(lProcess.Open(TProcessId(iChildWaiterArray[lCounterIdx].iWaiterPid)) == KErrNone)
				{
				lProcess.LogonCancel(iChildWaiterArray[lCounterIdx].iWaiterStatus);
				lProcess.Close();
				}
			}
		}                                                                             
	iSignalSession.Close();
	return KErrNone;
	}

static TInt sSignalHandler(TAny* aData)
	{
	CLocalSystemInterface* lClsi = 0;
	TInt ret = 0;
	if(aData == 0)
		return KErrGeneral;

	lClsi = static_cast<CLocalSystemInterface*>(aData);
	CTrapCleanup* pCleanup = CTrapCleanup::New();
	if (pCleanup)
		{
		ret = lClsi->SignalHandler();
		delete pCleanup;
		}	
	else
		{
		ret = KErrNoMemory;
		}	
	return ret;	
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::InitSignalHandler
// -----------------------------------------------------------------------------
//
EXPORT_C void CLocalSystemInterface::InitSignalHandler()
	{
	if(iSignalsInitialized)
		return;

	// Create thread
	iSignalHandlerThread.Create(KNullDesC,sSignalHandler,KDefaultStackSize,NULL,this);
	// Raise priority
	iSignalHandlerThread.SetPriority(EPriorityAbsoluteHigh);
	iSignalHandlerThread.Resume();

	iSignalInitSemaphore.Wait();
	return;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::EnsureSignalsInitialized
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::EnsureSignalsInitialized()
	{
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::SigWaitInternal
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::SigWaitInternal(TUint64 aWaitSet,TSignalMessage* aMsg,TInt aTimeout,TBool aTimeCheckFailed)
	{
	if(aWaitSet == 0)
		return KErrArgument;

	TInt lArrayIdx = 0,lSignalIdx = 1;
	while(lSignalIdx <= SIGRTMAX)
		{
		if(SIG_SIGNAL_PRESENT_IN_SIGSET(lSignalIdx,aWaitSet) && lSignalIdx != SIGKILL && lSignalIdx != SIGSTOP)
			{
			TSignalMessage lTmpMsg;

			lTmpMsg.mType = TSignalMessage::ESignal;
			lTmpMsg.mSignal = lSignalIdx;
			iBlockedSAMutex.Wait();
			lArrayIdx = iBlockedSigArray.Find(lTmpMsg,TSignalMessage::SignalMatchBySigNum);
			iBlockedSAMutex.Signal();
			if(lArrayIdx != KErrNotFound)
				{
				if(aMsg)
					{
					aMsg->mType = TSignalMessage::ESignal;
					iBlockedSAMutex.Wait();
					aMsg->mSignal = iBlockedSigArray[lArrayIdx].mSignal;
					iBlockedSAMutex.Signal();
					}
				iBlockedSAMutex.Wait();
				iBlockedSigArray.Remove(lArrayIdx);
				iBlockedSAMutex.Signal();
				return KErrNone;
				}

			lTmpMsg.mType = TSignalMessage::ESignalValuePair;
			lTmpMsg.mSigVal.mSignal = lSignalIdx;
			iBlockedSAMutex.Wait();
			lArrayIdx = iBlockedSigArray.Find(lTmpMsg,TSignalMessage::SigValMatchBySigNum);
			iBlockedSAMutex.Signal();
			if(lArrayIdx != KErrNotFound)
				{
				if(aMsg)
					{
					aMsg->mType = TSignalMessage::ESignalValuePair;
					iBlockedSAMutex.Wait();
					aMsg->mSigVal.mSignal = iBlockedSigArray[lArrayIdx].mSigVal.mSignal;
					aMsg->mSigVal.mValue  = iBlockedSigArray[lArrayIdx].mSigVal.mValue;
					iBlockedSAMutex.Signal();
					}
				iBlockedSAMutex.Wait();
				iBlockedSigArray.Remove(lArrayIdx);
				iBlockedSAMutex.Signal();
				return KErrNone;
				}

			lTmpMsg.mType = TSignalMessage::ERtTimerSignal;
			lTmpMsg.mRtSignal.mSignal = lSignalIdx;
			iBlockedSAMutex.Wait();
			lArrayIdx = iBlockedSigArray.Find(lTmpMsg,TSignalMessage::SigValMatchBySigNum);
			iBlockedSAMutex.Signal();
			if(lArrayIdx != KErrNotFound)
				{
				if(aMsg)
					{
					aMsg->mType = TSignalMessage::ERtTimerSignal;
					iBlockedSAMutex.Wait();
					aMsg->mRtSignal.mSignal = iBlockedSigArray[lArrayIdx].mRtSignal.mSignal;
					iBlockedSAMutex.Signal();
					}
				iBlockedSAMutex.Wait();
				iBlockedSigArray.Remove(lArrayIdx);
				iBlockedSAMutex.Signal();
				return KErrNone;
				}			
			}
		lSignalIdx++;
		}

	if(aTimeCheckFailed)
		return KErrGeneral;

	TSignalMessage lMsg;
	TSignalWaiter lWaiter(aWaitSet,&lMsg);

	iSignalWaiterMutex.Wait();
	RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);
	iSignalWaiterArray.Append(lWaiter);
	User::SwitchHeap(oldHeap);

	iSignalWaiterMutex.Signal();
	if(aTimeout > 0)
		{
		TInt lRetVal = lWaiter.iSigWaitSemaphore.Wait(aTimeout);
		if(lRetVal != KErrNone)
			{
			// Timed out
			TSignalWaiter lTempWaiter(0,0);
			TInt lIdx;
			iSignalWaiterMutex.Wait();
			lIdx = iSignalWaiterArray.Find(lTempWaiter,TSignalWaiter::MatchByThreadId);
			if(lIdx != KErrNotFound)
				{
				oldHeap = User::SwitchHeap(iPrivateHeap);
				iSignalWaiterArray.Remove(lIdx);
				User::SwitchHeap(oldHeap);
				}
			iSignalWaiterMutex.Signal();
			return KErrTimedOut;
			}
		}
	else
		lWaiter.iSigWaitSemaphore.Wait();

	TSignalWaiter lTempWaiter(0,0);
	TInt lIdx = 0;
	iSignalWaiterMutex.Wait();
	lIdx = iSignalWaiterArray.Find(lTempWaiter,TSignalWaiter::MatchByThreadId);
	if(lIdx != KErrNotFound)
		{
		oldHeap = User::SwitchHeap(iPrivateHeap);
		iSignalWaiterArray.Remove(lIdx);
		User::SwitchHeap(oldHeap);
		}
	iSignalWaiterMutex.Signal();

	if(lMsg.mType == TSignalMessage::ESignal)
		{
		aMsg->mType = TSignalMessage::ESignal;
		aMsg->mSignal = lMsg.mSignal;
		}
	else if(lMsg.mType == TSignalMessage::ESignalValuePair)
		{
		aMsg->mType = TSignalMessage::ESignalValuePair;
		aMsg->mSigVal.mSignal = lMsg.mSigVal.mSignal;
		aMsg->mSigVal.mValue = lMsg.mSigVal.mValue;
		}
	else if(lMsg.mType == TSignalMessage::ERtTimerSignal)
		{
		aMsg->mType = TSignalMessage::ERtTimerSignal;
		aMsg->mRtSignal.mSignal = lMsg.mRtSignal.mSignal;
		}
	else
		{
		return KErrGeneral;
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::RegisterExitNotification
// -----------------------------------------------------------------------------
//
void CLocalSystemInterface::RegisterExitNotification(TProcessId aPid)
	{
	TSignalMessage lMsg;
	lMsg.mType = TSignalMessage::EWaitOnChild;
	lMsg.mPid = aPid.Id();
	TBuf8<KSigMsgLength> lMsgBuf;
	lMsg.Marshall(lMsgBuf);
	iSignalWritePipe.Write(lMsgBuf,KSigMsgLength);
	return;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::InitializeSigHandlers
// -----------------------------------------------------------------------------
//
void CLocalSystemInterface::InitializeSignalHandlers()
	{
	iSigHandlerMutex.CreateLocal();
	iAlarmTimer.CreateLocal();  // what should be done if fails
	iIsTimerActive = EFalse;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::HandleSignal
// -----------------------------------------------------------------------------
//
void CLocalSystemInterface::HandleSignal(TSignalMessage& aMsg,TBool aIsDequeued)
	{
	TUint8 lSigNum = 0;

	if(aMsg.mType == TSignalMessage::ESignal)
		lSigNum = aMsg.mSignal;
	else if(aMsg.mType == TSignalMessage::ERtTimerSignal)
		lSigNum = aMsg.mRtSignal.mSignal;
	else
		lSigNum = aMsg.mSigVal.mSignal;

	if(lSigNum <= 0 || lSigNum > SIGRTMAX)
		return;

	if(SIG_SIGNAL_PRESENT_IN_SIGSET(lSigNum,iSigMask) && !aIsDequeued)
		{
		TInt lIdx = 0;
		TSignalMessage lTmpMsg;

		lTmpMsg.mType = aMsg.mType;
		if(aMsg.mType == TSignalMessage::ESignal)
			{
			lTmpMsg.mSignal = aMsg.mSignal;
			iBlockedSAMutex.Wait();
			lIdx = iBlockedSigArray.Find(lTmpMsg,TSignalMessage::SignalMatchBySigNum);
			iBlockedSAMutex.Signal();
			if(lIdx == KErrNotFound || lSigNum >= SIGRTMIN)
				{
				lTmpMsg.mType = TSignalMessage::ESignal;
				lTmpMsg.mSignal = aMsg.mSignal;
				iBlockedSAMutex.Wait();
				iBlockedSigArray.Append(lTmpMsg);
				iBlockedSAMutex.Signal();
				}
			}
		else if(aMsg.mType == TSignalMessage::ESignalValuePair)
			{
			lTmpMsg.mSigVal.mSignal = aMsg.mSigVal.mSignal;
			lTmpMsg.mSigVal.mValue = aMsg.mSigVal.mValue;
			iBlockedSAMutex.Wait();
			iBlockedSigArray.Append(lTmpMsg);
			iBlockedSAMutex.Signal();
			}
		else if(aMsg.mType == TSignalMessage::ERtTimerSignal)
			{
			lTmpMsg.mSignal = aMsg.mRtSignal.mSignal;
			iBlockedSAMutex.Wait();
			lIdx = iBlockedSigArray.Find(lTmpMsg,TSignalMessage::SignalMatchBySigNum);
			iBlockedSAMutex.Signal();
			if(lIdx == KErrNotFound || lSigNum >= SIGRTMIN)
				{
				lTmpMsg.mType = TSignalMessage::ERtTimerSignal;
				lTmpMsg.mRtSignal.mSignal = aMsg.mRtSignal.mSignal;
				iBlockedSAMutex.Wait();
				iBlockedSigArray.Append(lTmpMsg);
				iBlockedSAMutex.Signal();
				}
			}

		return;
		}
	else
		{
		SuspendOtherThreads();// suspend all other threads
		TUint32 lSigVal = 0;
		TSignalHandler lCurHandler = 0;
		TSignalAction lCurAction = 0;

		if(aMsg.mType == TSignalMessage::ESignal)
			lSigNum = aMsg.mSignal;
		else if(aMsg.mType == TSignalMessage::ESignalValuePair)
			{
			lSigNum = aMsg.mSigVal.mSignal;
			lSigVal = aMsg.mSigVal.mValue;
			}
		else if(aMsg.mType == TSignalMessage::ERtTimerSignal)
			{
			lSigNum = aMsg.mRtSignal.mSignal;
#ifdef SYMBIAN_OE_LIBRT
			//reset the overrun count before calling the handler/action.
			ResetOverrun(aMsg.mRtSignal.mTimerId); 
#endif //SYMBIAN_OE_LIBRT			
			}
		else
			{
			ResumeOtherThreads();
			return;
			}
		iSigHandlerMutex.Wait();
		lCurHandler = iSigActions[lSigNum -1].iSigHandler;
		lCurAction = iSigActions[lSigNum-1].iSigAction;

		if(lCurAction)
			{
			siginfo_t lsiginfo;
			lsiginfo.si_signo = lSigNum;
			lsiginfo.si_value.sival_int = lSigVal;
			lCurAction(lSigNum,&lsiginfo,(void*)0); // setting context as 0
			}
		else
			{
			if(lCurHandler == SIG_DFL)
				{
				CallDefaultHandler(lSigNum);
				}
			else if(lCurHandler != SIG_IGN)
				{
				lCurHandler(lSigNum);
				}
			}
		ResumeOtherThreads();// resume all other threads
		}

	if(iSigActions[lSigNum -1].iSigFlags & SA_RESETHAND)
		{
		iSigActions[lSigNum -1].iSigHandler = SIG_DFL;
		}
	iSigHandlerMutex.Signal();

	return;
	}

// -----------------------------------------------------------------------------
// sig_exit
// -----------------------------------------------------------------------------
//
static void sig_exit()
	{
	Backend()->Exit(0);  // Check if clean exit or terminate is to be done
	return;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::CallDefaultHandler
// -----------------------------------------------------------------------------
//
void CLocalSystemInterface::CallDefaultHandler(int aSigNum)
	{
	if(aSigNum < 1 || aSigNum > SIGRTMAX)
		return;

	switch(aSigNum)
		{
		case SIGTERM:
		case SIGKILL:
		case SIGALRM:
		case SIGPIPE:
			sig_exit();
			break;
		case SIGQUIT:
		default:
			// SIG_IGN - Do nothing;
			break;
		}
	return;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::SuspendOtherThreads
// -----------------------------------------------------------------------------
//
_LIT(KThreadFormatString,"*");

TInt CLocalSystemInterface::SuspendOtherThreads()
	{
	TFileName lProcessName = RProcess().FileName();
	TBuf<256> lThreadNameString;
	TFullName lThreadName;

#ifdef SYMBIAN_OE_LIBRT	
	TBool lFoundTimerServ = EFalse;
#endif			

	TParsePtr lParser(lProcessName);
	TPtrC lName = lParser.NameAndExt();


	lThreadNameString.Append(lName);
	lThreadNameString.Append(KThreadFormatString);

	TFindThread lFindPattern(lThreadNameString);
	TInt lRetVal = KErrNone;

	while(lRetVal == KErrNone)
		{
		lRetVal = lFindPattern.Next(lThreadName);
		if(lRetVal == KErrNone)
			{
#ifdef SYMBIAN_OE_LIBRT			
			if(!lFoundTimerServ && (lThreadName.Match(_L("*LibrtTimerServ")) != KErrNotFound))
				{
				lFoundTimerServ = ETrue;
				continue;	
				}
#endif				
			RThread  lThread;
			RProcess lProcess;
			if(lThread.Open(lThreadName) == KErrNone)
				{
				lThread.Process(lProcess);	
				if( (lProcess.Id() == RProcess().Id()) && 
						(lThread.Id() != RThread().Id()) )
					lThread.Suspend();
				lThread.Close();
				}
			}
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::ResumeOtherThreads
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::ResumeOtherThreads()
	{
	TFileName lProcessName = RProcess().FileName();
	TBuf<256> lThreadNameString;
	TFullName lThreadName;
#ifdef SYMBIAN_OE_LIBRT		
	TBool lFoundTimerServ = EFalse;
#endif	
	TParsePtr lParser(lProcessName);
	TPtrC lName = lParser.NameAndExt();


	lThreadNameString.Append(lName);
	lThreadNameString.Append(KThreadFormatString);

	TFindThread lFindPattern(lThreadNameString);
	TInt lRetVal = KErrNone;

	while(lRetVal == KErrNone)
		{
		lRetVal = lFindPattern.Next(lThreadName);
		if(lRetVal == KErrNone)
			{
#ifdef SYMBIAN_OE_LIBRT						
			if(!lFoundTimerServ && (lThreadName.Match(_L("*LibrtTimerServ")) != KErrNotFound))
				{
				lFoundTimerServ = ETrue;
				continue;	
				}
#endif			
			RThread  lThread;
			RProcess lProcess;
			if(lThread.Open(lThreadName) == KErrNone)
				{
				lThread.Process(lProcess);				
				if( (lProcess.Id() == RProcess().Id()) && 
						(lThread.Id() != RThread().Id()) )
					lThread.Resume();
				lThread.Close();
				}
			}
		}

	return KErrNone;
	}

#endif // SYMBIAN_OE_POSIX_SIGNALS

// -----------------------------------------------------------------------------
// CLocalSystemInterface::fcntl
// Implementation for fcntl
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::fcntl (int aFid, int aCmd, long anArg, int& anErrno)
	{
	return iFids.fcntl(aFid, aCmd, anArg, anErrno);
	}


int CLocalSystemInterface::fchmod (int fd , mode_t  perms, int& anErrno)
	{
	CFileDescBase *f = NULL;

	TInt err = iFids.At(fd, f);

	if(!err)
		{
		switch(perms & (S_IWUSR | S_IRUSR ))
			{
			case S_IRUSR | S_IWUSR :
			case S_IWUSR :
				{
				err = f->SetAtt(0, KEntryAttReadOnly);
				break;
				}
			case S_IRUSR :
				{
				err = f->SetAtt(KEntryAttReadOnly, 0);
				break;
				}
			default :
				{
				err = KErrArgument;
				break;
				}
			}
		}
	return MapError(err, anErrno);
	}

// Synchronous layer over CFileTable asynchronous routines
int CLocalSystemInterface::read (int fid, char* buf, unsigned long len, int& anErrno)
	{
	CFileDescBase* f=0;
	TBool patchErr = EFalse;

	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		//if the file is WRONLY then return Error here
		TUint flgs = f->Fcntl(fid, F_GETFL);
		if ((flgs & O_ACCMODE) == O_WRONLY)
			{
			anErrno = EBADF;
			f->Close();	// balances the Dup() in CFileTable::Asynch()
			return -1;
			}

		TRequestStatus readStatus;
		TRequestStatus timerStatus(KRequestPending);
		RTimer theTimer;
		TBool timerRunning = EFalse;

		TPtr8 ptr((TText8 *)buf, len);

		if (f->TimedRead())
			{
			TTimeIntervalMicroSeconds32 timeout(f->TimeoutValue()*1000);
			theTimer.CreateLocal();
			theTimer.After(timerStatus, timeout);
			timerRunning = ETrue;
			}

		//If its console based, fid = {0, 1, 2 }
		if( fid >= 0 &&	fid <= 2 )
			{
			f->SetFid(fid);
			}

		f->Read(ptr, readStatus);		

		User::WaitForRequest(readStatus, timerStatus);

		if (timerRunning)
			{
			if (timerStatus.Int() != KRequestPending)
				{
				//cancel the read and wait for it
				f->ReadCancel();
				patchErr = ETrue;	//report this as a timeout not a cancel!!
				User::WaitForRequest(readStatus);
				}
			else
				{
				//if the timer was in operation
				//cancel the timer
				theTimer.Cancel();
				User::WaitForRequest(timerStatus);
				}
			theTimer.Close();
			}

		err=f->ReadCompletion(ptr, readStatus.Int());
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		if (err>0)
			return err;
		else if( (f->Attributes() & KPipeFd )|| (fid == 0))
			{
			// if EOF is reached in case of pipes or stdin
			// TODO: Verify that this check is valid for console or comm-based stdin?
			if(err == KErrEof)
				{
				MapError(err,anErrno);
				return 0;
				}
			}
		else if( f->Type()==CFileDescBase::ESocketDesc || f->Type()==CFileDescBase::EFileSocketDesc)
			{
			// if EOF is reached for the Socket return 0
			if(err == KErrEof)
				{
				return 0;
				}
	    	if(err == KErrDisconnected)
	    		{
	    		err = ECONNRESET;
	    		}
			}
		}
	if (patchErr)
		err = ETIMEDOUT;

	return MapError(err,anErrno);
	}

int CLocalSystemInterface::write (int fid, const char* buf, unsigned long len, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		TPtr8 ptr((TText8 *)buf, len, len);

		//If its console based, fid = {0, 1, 2 }
		if( fid >= 0 &&	fid <= 2 )
			{
			f->SetFid(fid);
			}

		TUint flgs = f->Fcntl(fid, F_GETFL);

		if((flgs & O_ACCMODE) == O_RDONLY)
			{
			anErrno = EBADF;
			f->Close();	// balances the Dup() in CFileTable::Asynch()
			return -1;
			}
		f->Write(ptr,status);
		User::WaitForRequest(status);
		err=f->WriteCompletion(ptr, status.Int());
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		if (err > 0)
			return err;
    	if((f->Type() == CFileDescBase::EFileSocketDesc || f->Type() == CFileDescBase::ESocketDesc) && err == KErrDisconnected)
    		{
    		err = ECONNRESET;
    		}
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::recvfrom (int fid, char* buf, unsigned long len, int flags, struct sockaddr* from, unsigned long* fromsize, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		TPtr8 ptr((TText8 *)buf, len);
		struct sockaddr tmpAddr;
		TUSockAddr addr(&tmpAddr);
		f->RecvFrom(ptr,addr,flags,status);
		User::WaitForRequest(status);
		TInt ret=0;
		err=f->CompletionStatus(ret, status.Int());
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		if (err==0)
			{
			if(f->Type() == CFileSocketDesc::EFileSocketDesc)
				{
				((CFileSocketDesc*)f)->GetLocalSockAddrByPort((sockaddr_un*)from,fromsize,addr.Port());
				}
			else
				{
				addr.Get(from,fromsize);
				}

			return ptr.Length();		
			}
		}
	if(err == KErrDisconnected)
		{
		anErrno = ECONNRESET;
		return -1;
		}
	if (err == KErrWouldBlock)
		{
		return MapError(EAGAIN, anErrno);
		}
	else
		{
		return MapError(err,anErrno);
		}
	}

int CLocalSystemInterface::sendto (int fid, const char* buf, unsigned long len, int flags, struct sockaddr* to, unsigned long tosize, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		TPtr8 ptr((TText8 *)buf, len, len);
		TInt bytesSent = 0;
		f->SendTo(ptr,to,tosize,flags,status);
		User::WaitForRequest(status);
		err=f->CompletionStatus(bytesSent, status.Int());
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		if (err==0)
			{
			return bytesSent;			
			}
		}
	if (err == KErrNotReady)
		{
		anErrno = EPIPE;
		return -1;
		}
	if(err == KErrDisconnected)
		{
		anErrno = ECONNRESET;
		return -1;
		}

	// A Hack to convert KErrTooBig to EMSGSIZE for this API.
	// For any KErrTooBig, MapError will convert it to EFAULT.
	if (err == KErrTooBig)
		{
		anErrno = EAGAIN;
		return -1;
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::fsync (int fid, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		f->Sync(status);
		User::WaitForRequest(status);
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		err=status.Int();
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::shutdown (int fid, int how, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		TRequestStatus status;
		f->Shutdown(how,status);
		User::WaitForRequest(status);
		err=status.Int();
		// SOS does not allow to RSocket::Shutdown() immediately.
		if (err == KErrNotSupported)
			{
			err = 0;
			}
		f->Close();	// balances the Dup() in CFileTable::Asynch()
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::connect (int fid, const struct sockaddr* addr, unsigned long size, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	TInt mapErr = err;

	if(!err)
		{
		// Check if a connection request is already in progress for a non-blocking socket.
		if (f->GetConnectionProgress())
			{
			mapErr = f->iPollErr;
			
			if (f->iPollErr < 0)
			    {
                mapErr = f->iPollErr;
                f->iPollErr = 0;
                f->SetConnectionProgress(EFalse);
                }
			else
			    {
                // Poll to see if the connect() is completed
                err = f->Poll(EReadyForWriting);
                // The above Poll is common for all descriptors. 
                // In case of socket-descriptors, Poll will either return "KErrNone"
                // or any of the requested events. To check for Poll error in case of 
                // socket-descriptors, "iPollErr" should be checked.
                if (f->iPollErr < 0)
                    {
                    mapErr = f->iPollErr;
                    f->iPollErr = 0;
                    }
                else if (err == 0) //Connect still in progress
                    mapErr = EALREADY;
                else if (err & EReadyForWriting)
                    {
                    mapErr = EISCONN; 
                    }
			    }
			}

		if(!mapErr)
			{
			TRequestStatus status;
			f->Connect(addr,size,status);
			err = status.Int();

			if (err == KErrWouldBlock)
				{
				mapErr = EINPROGRESS;
				}
			else if (err == KErrAlreadyExists)
				{
				mapErr = EISCONN;
				}
			else
				mapErr = err;
			}

		f->Close(); // balances the Dup() in CFileTable::Asynch()
		}
	
	return MapError(mapErr, anErrno);

	}

//
// The CSocketDesc performing the Accept is responsible for creating the new CSocketDesc
//
int CLocalSystemInterface::accept (int fid, struct sockaddr *addr, size_t *size, int& anErrno)
	{
	
	   size_t addrlen = 0;
	    if (addr && !size)
	        {
	        anErrno = EINVAL;
	        return -1;
	        }
	    else if (size)
	        {
	        addrlen = *size;
	        }
	        
	    TUSockAddr address((TAny*)addr, addrlen);
	    /* Accept implementations that use the TUSockAddr* param, first need to assert address.iError == 0 */
	    
	    CFileDescBase* f = 0;
	    TInt err=iFids.Asynch(fid,f);
	    if (!err)
	        {
	        CFileDescBase* newf=0;
	        int fd=iFids.Reserve();
	        if (fd>=0)
	            {
	            TRequestStatus status;
	            //Acquire the Lock before issuing a request to the server and release it later
	            iSSLock.Wait();

	            RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);
		    //coverity[alloc_fn]
	            f->Accept(newf,status,iSs, &address);
	            User::SwitchHeap(oldHeap);

			iSSLock.Signal();
			User::WaitForRequest(status);
			//coverity[leave_without_push]
			f->Close();	// balances the Dup() in CFileTable::Asynch()
			err=status.Int();
			if (!err)
				{
				err=iFids.Attach(fd,newf);
				if (!err)
					return fd;				
				newf->Close();
				}
			else if(newf != NULL)
				{
				//coverity[leave_without_push]
				newf->Close();
				}
			iFids.Attach(fd,0);	// cancel the reservation
			}
		//coverity[memory_leak]
		}
	if (err == KErrNotReady)
		{
		return MapError(EINVAL,anErrno);
		}
	else if (err == KErrWouldBlock)
		{
		return MapError(EWOULDBLOCK, anErrno);
		}
	else
		{
		return MapError(err,anErrno);
		}
	}

int CLocalSystemInterface::ioctl (int fid, int cmd, void* param, int& anErrno)
	{
	TRequestStatus ioctlStatus;
	TInt err=ioctl(fid,cmd,param,ioctlStatus,anErrno);
	if (err==KErrNone)
		{
		User::WaitForRequest(ioctlStatus);
		err=ioctl_complete(fid,cmd,param,ioctlStatus,anErrno);
		}
	return err;
	}

// C++ version of asynchronous ioctl

int CLocalSystemInterface::ioctl (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)
	{
	CFileDescBase* f=0;
	TInt err=iFids.Asynch(fid,f);
	if (!err)
		{
		f->Ioctl(cmd,param,aStatus);
		f->Close();	// balances the Dup() in CFileTable::Asynch() - live dangerously!
		}
	return MapError(err,anErrno);
	}

int CLocalSystemInterface::ioctl_complete (int fid, int cmd, void* param, TRequestStatus& aStatus, int& anErrno)
	{
	return iFids.ioctlcomplete(fid,cmd,param,aStatus,anErrno);
	}

#ifdef __SYMBIAN_COMPILE_UNUSED__
int CLocalSystemInterface::ioctl_cancel (int fid, int& anErrno)
	{
	return iFids.ioctlcancel(fid,anErrno);
	}
#endif //__SYMBIAN_COMPILE_UNUSED__


int CLocalSystemInterface::pipe(int fildes[2], int& anErrno )
	{
	return iFids.pipe(fildes, anErrno);
	}

int CLocalSystemInterface::mkfifo(const wchar_t *pathName, mode_t mode, int& anErrno )
	{
	return iFids.mkfifo(pathName, mode, anErrno, iFs);
	}

int CLocalSystemInterface::link(const wchar_t *oldpathName, const wchar_t *newpathName, int& anErrno )
	{
	return iFids.link(oldpathName, newpathName, anErrno, iFs);
	}

int CLocalSystemInterface::unlink(const wchar_t *pathName, int& anErrno )
	{
	return iFids.unlink(iFs, pathName, anErrno);
	}

int CLocalSystemInterface::select(int maxfd, fd_set* readfds,fd_set* writefds, 
		fd_set* exceptfds, struct timeval* tvptr, int& anErrno) 
	{ 
	// select is nothing but eselect without any TRequestStatus array
	return eselect(maxfd,readfds,writefds,exceptfds,tvptr,0,NULL,anErrno);
	}
// -----------------------------------------------------------------------------
// CLocalSystemInterface::aselect
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::aselect(int maxfd, fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, struct timeval *tvptr, TRequestStatus* requeststatus,
		int& anErrno)
	{
	// Create a seperate thread to handle the aselect
	return CreateaselectThread(maxfd, readfds, writefds, exceptfds, tvptr,
			requeststatus,anErrno);
	}
// -----------------------------------------------------------------------------
// CLocalSystemInterface::aselect
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::cancelaselect(TRequestStatus* requeststatus, int& anErrno, TBool perform_cleanup)
	{
	iASelectLock.Wait();
	// Search for the aselect request entry in the aselect request array
	for (TInt i = 0; i < iASelectRequest.Count(); ++i)
		{
		if (iASelectRequest[i].iRequestStatus == requeststatus || perform_cleanup)
			{
			// The specified request exists
			RThread thread;
			// Open a handle to the service thread
			TInt res = thread.Open(iASelectRequest[i].iThreadId,EOwnerThread);
			if (res == KErrNone)
				{
				// Kill the service thread
				thread.Kill(KErrCancel);
				thread.Close();
				if (!perform_cleanup)
					{
					User::RequestComplete(iASelectRequest[i].iRequestStatus,KErrCancel);
					}
				// Switch to backend heap
				RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);
				// Remove the request details from the array
				iASelectRequest.Remove(i);
				// Switch back to old heap
				User::SwitchHeap(oldHeap);
				if (!perform_cleanup)
					{
					iASelectLock.Signal();
					return KErrNone;
					}
				}
			else
				{
				if (!perform_cleanup)
					{
					iASelectLock.Signal();
					// unable to open a handle to the service thread
					return MapError(res, anErrno);
					}
				}
			}
		}
	iASelectLock.Signal();
	// No request found with the specified TRequestStatus object
	return MapError((!perform_cleanup ? KErrNotFound : KErrNone), anErrno);
	}
// -----------------------------------------------------------------------------
// CLocalSystemInterface::ASelectRequest
// -----------------------------------------------------------------------------
//
RArray<TASelectRequest>& CLocalSystemInterface::ASelectRequest()
	{
	// Return a reference to the array
	return iASelectRequest;
	}
// -----------------------------------------------------------------------------
// CLocalSystemInterface::ASelectLock
// -----------------------------------------------------------------------------
//
RFastLock& CLocalSystemInterface::ASelectLock()
	{
	// Return a reference to the array
	return iASelectLock;
	}
// -----------------------------------------------------------------------------
// CLocalSystemInterface::eselect
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::eselect(int maxfd, fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, struct timeval *tvptr, int arraycount,
		TRequestStatus* waitarray,int& anErrno)
    {
    unsigned long timeout = 0; 
    if (tvptr) 
        { 
        if (tvptr->tv_sec < 0 || tvptr->tv_usec < 0) 
            { 
            anErrno = EINVAL; 
            return -1; 
            } 
        // converting to micro seconds 
        timeout = KConvertToMicroSecond * tvptr->tv_sec + tvptr->tv_usec; 
        } 

    // validate maxfd 
    if (maxfd < 0 || maxfd > FD_SETSIZE) 
        { 
        anErrno = EINVAL; 
        return -1; 
        }
    // maxfd should not be more than OPEN_MAX
    if(maxfd > OPEN_MAX)
        {
        maxfd = OPEN_MAX;	
        }

    if (!readfds && !writefds && !exceptfds && ((arraycount == 0) || (waitarray == NULL)) ) 
        {
        if (tvptr) 
            { 
            // select is being used as a better resolution timer 
            User::After(timeout); 
            anErrno = ETIMEDOUT;
            return 0; 
            } 
        // hang forever? - according to spec, until interuppted by signal 
        TRequestStatus req = KRequestPending; 
        User::WaitForRequest(req); 
        }

    fd_set retreadfds; 
    fd_set retwritefds; 
    fd_set retexceptfds; 
    FD_ZERO(&retreadfds); 
    FD_ZERO(&retwritefds); 
    FD_ZERO(&retexceptfds);

    // Intially perform synchronous non-blocking polls to see if any of the 
    // input file descriptors are ready for read/write/error conditions. 
    // In the meantime also determine, how many TRequestStatus objects we will 
    // need for the next async run. 
    CFileDescBase* fdesc = NULL; 
    TInt nDescriptorsSet = 0; 
    TInt numReqs = 0; 
    TInt err = 0; 

    for (int i = 0; i < maxfd; ++i) 
        { 
        TUint events = 0; 	
        if (readfds && FD_ISSET(i, readfds)) 
            { 
            events = EReadyForReading; 
            } 
        if (writefds && FD_ISSET(i, writefds)) 
            { 
            events |= EReadyForWriting; 
            } 
        if (exceptfds && FD_ISSET(i, exceptfds)) 
            { 
            events |= EAnyException; 
            } 
        if (!events) 
            { 
            continue; 
            } 
        if (iFids.At(i, fdesc) != KErrNone) 
            { 
            // Bad input file descriptor 
            anErrno = EBADF; 
            return -1; 
            } 
        // Poll on this descriptor 
        err = fdesc->Poll(events); 
        if (err < 0) 
            { 
            if(err == KErrNotSupported)
                {
                // continue if a write event is being set on the read end of pipe or vice-versa
                continue;            
                }
            // An error - we should return 
            return MapError(err, anErrno); 
            } 
        else 
            { 
            if (err & EReadyForReading) 
                { 
                FD_SET(i, &retreadfds); 
                ++nDescriptorsSet; 
                } 
            if (err & EReadyForWriting) 
                { 
                FD_SET(i, &retwritefds); 
                ++nDescriptorsSet; 
                } 
            if (err & EAnyException) 
                { 
                FD_SET(i, &retexceptfds); 
                ++nDescriptorsSet;	                  
                } 
            ++numReqs; 
            } 
        } 

    if (nDescriptorsSet || (tvptr && timeout == 0)) 
        { 
        // Some of the descriptors are ready for requested operations. Return. 
        if (readfds) 
            { 
            FD_COPY(&retreadfds, readfds); 
            } 
        if (writefds) 
            { 
            FD_COPY(&retwritefds, writefds); 
            } 
        if (exceptfds) 
            { 
            FD_COPY(&retexceptfds, exceptfds); 
            } 
        return nDescriptorsSet; 
        } 

    if (numReqs == 0 && ((arraycount == 0) || (waitarray == NULL)) ) 
        { 
        if (tvptr) 
            { 
            User::After(timeout); 
            anErrno = ETIMEDOUT;
            return 0; 
            } 
        // hang forever? - according to spec, until interuppted by signal 
        TRequestStatus req = KRequestPending; 
        User::WaitForRequest(req); 
        } 

    // Now prepare to issue the asynchronous requests. 
    TRequestStatus** reqarray;
    TRequestStatus* reqobjarray;

    // Create the array of TRequestStatus objects
    reqobjarray = (TRequestStatus *) Alloc((numReqs + (timeout ? 1 : 0)) * sizeof(TRequestStatus));
    if (!reqobjarray)
        {
        anErrno = ENOMEM;
        return -1;
        }

    reqarray = (TRequestStatus **) Alloc((numReqs+arraycount + (timeout ? 1 : 0)) * sizeof(TRequestStatus*));
    if (!reqarray)
        {
        Free(reqobjarray);
        anErrno = ENOMEM;
        return -1;
        }

    // Append all the created TRequestStatus objects to the array
    for (int i = 0; i<numReqs; i++)
        {
        reqarray[i] = &reqobjarray[i]; 
        }

    // Append the array of TRequestStatus passed in waitarray to the array
    for (int i = numReqs, j=0; i<numReqs+arraycount; i++, j++)
        {
        reqarray[i] = &waitarray[j];
        }

    if (timeout)
        {
        // Append the TRequestStatus used for timer to the array
        reqarray[numReqs+arraycount] = &reqobjarray[numReqs];
        }

    TInt *reqfds = (TInt *)Alloc(numReqs * sizeof(TInt)); 
    if (!reqfds) 
        { 
        Free(reqarray);
        Free(reqobjarray);
        anErrno = ENOMEM; 
        return -1; 
        }

    // Reset numReqs. We will use it to keep track of how many objects in the 
    // reqarray are valid requests. 
    numReqs = 0; 

    // Used to detect one request gathered when this code reaches WaitForNReqs
    TBool onedown = EFalse;

    // define the RTimer here to work-around a GCC warning 
    RTimer timer;
    if (timeout)
        {
        // Create the timer to handle the timeout
        err = timer.CreateLocal();
        if (err != KErrNone)
            {			
            Free(reqarray);
            Free(reqobjarray);
            Free(reqfds);
            return MapError(err, anErrno);
            }
        }

    // Start processing requested-for events
    for (int i = 0; i < maxfd; ++i) 
        { 
        TUint events = 0; 

        if (readfds && FD_ISSET(i, readfds)) 
            { 
            events = EReadyForReading; 
            } 

        if (writefds && FD_ISSET(i, writefds)) 
            { 
            events |= EReadyForWriting;			
            } 

        if (exceptfds && FD_ISSET(i, exceptfds)) 
            { 
            events |= EAnyException; 
            } 

        if (!events)
            {
            continue;
            }

        if (iFids.At(i, fdesc) == KErrNone) 
            {
            fdesc->TweakWatchedEvents(events);
            err = fdesc->NotifyActivity(events, *reqarray[numReqs], timeout);
            if (err == KErrCompletion)
                {
                // No async call was issued. Try a Poll
                TInt ready = fdesc->Poll(events);
                if (ready & EReadyForReading) 
                    { 
                    FD_SET(i, &retreadfds); 
                    ++nDescriptorsSet; 
                    } 
                if (ready & EReadyForWriting) 
                    { 
                    FD_SET(i, &retwritefds); 
                    ++nDescriptorsSet; 
                    } 
                if (ready & EAnyException) 
                    {			
                    FD_SET(i, &retexceptfds); 
                    ++nDescriptorsSet;
                    }
                break;
                } 
            else if (err != KErrNone) 
                { 
                // Notification registration failed. Cleanup and return. 
                nDescriptorsSet = MapError(err, anErrno);
                break; 
                } 
            else 
                { 
                // Issued an asynchronus request. Increment valid requests count. 
                reqfds[numReqs++] = i; 
                } 
            }
        }

    if (err != KErrNone)
        {
        if (timeout)
            {
            timer.Close();
            }
        for(TInt i= 0; i < numReqs; i++)
            {
            if (iFids.At(reqfds[i], fdesc) != KErrNone)
                {
                continue;
                }
            fdesc->CancelNotify();
            }
        goto bailout;
        }	

    if (timeout) 
        { 
        timer.After(*reqarray[numReqs+arraycount], timeout); 
        // Wait for any request to complete 
        CLocalSystemInterface::WaitForNRequest(*reqarray, numReqs+arraycount+1);     
        if( (*reqarray[numReqs+arraycount]).Int() == KRequestPending) 
            {
            // The timer hasn't fired yet.
            timer.Cancel(); 
            }
        else
            {
            anErrno = ETIMEDOUT;
            }
        timer.Close();
        // No need to set onedown to True as numReqs does not include the Timer request
        } 
    else 
        { 
        CLocalSystemInterface::WaitForNRequest(*reqarray, numReqs+arraycount);
        // Completion Status of one request has been gathered
        onedown = ETrue;
        }

    for (int i = 0; i < numReqs; ++i) 
        { 
        if (iFids.At(reqfds[i], fdesc) != KErrNone)
            {
            continue;
            }

        if ( (*reqarray[i]).Int() != KRequestPending ) 
            {
            TInt readyevents = fdesc->TweakReadyEvents((*reqarray[i]).Int());
            TInt event_marked = EFalse;
            if (readfds && FD_ISSET(reqfds[i], readfds) && (readyevents & EReadyForReading)) 
                { 
                FD_SET(reqfds[i], &retreadfds); 
                ++nDescriptorsSet;
                event_marked = ETrue;

                }
            if(writefds && FD_ISSET(reqfds[i], writefds) && (readyevents & EReadyForWriting) ) 
                { 
                FD_SET(reqfds[i], &retwritefds); 

                event_marked = ETrue;
                ++nDescriptorsSet; 
                }
            if(exceptfds && FD_ISSET(reqfds[i], exceptfds) && (readyevents & EAnyException))
                {
                if (!fdesc->GetConnectionProgress() || !event_marked)
                    {
                FD_SET(reqfds[i], &retexceptfds); 
                ++nDescriptorsSet;
                    }
                }
            }
        else
            {
            //Cancel all other requests.
            fdesc->CancelNotify();
            }
        }

    if (readfds) 
        { 
        FD_COPY(&retreadfds, readfds); 
        } 

    if (writefds) 
        { 
        FD_COPY(&retwritefds, writefds); 
        } 

    if (exceptfds) 
        { 
        FD_COPY(&retexceptfds, exceptfds); 
        } 

    bailout:
    
    if (onedown && numReqs)
        {
        --numReqs;
        }
    // consume all completed/cancelled requests 
    while(numReqs--)
        {
        User::WaitForAnyRequest();
        }

    Free(reqarray); 
    Free(reqobjarray);
    Free(reqfds); 
    return nDescriptorsSet;	
    }



// -----------------------------------------------------------------------------
// CLocalSystemInterface::system
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::system(const wchar_t* aCmd, const wchar_t* aCmdArg, int& anErrno)
	{
	return iFids.system(aCmd, aCmdArg, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::posix_spawn
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::posix_spawn(int* pid, const wchar_t* wpath,
		const posix_spawn_file_actions_t* file_actions,
		const posix_spawnattr_t* attrp,
		const wchar_t* wargs,
		wchar_t** wenvp)
	{
	return iFids.posix_spawn(pid, wpath, file_actions, attrp, wargs, wenvp);
	}

#ifdef SYMBIAN_OE_POSIX_SIGNALS
// -----------------------------------------------------------------------------
// CLocalSystemInterface::kill
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::kill(pid_t pid, int sig , int& anErrno)
	{
	EnsureSignalsInitialized();

	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}	
	
	TInt err = KErrArgument;
	TProcessId lPid(pid);
	RProcess lTargetProcess;

	if(pid <= 0)
		{
		anErrno = ENOTSUP;
		return -1;
		}

	if(lTargetProcess.Open(lPid) != KErrNone)
		{
		anErrno = ESRCH;
		return -1;
		}
	else
		{
		lTargetProcess.Close();
		}

	if(sig == 0)
		{
		return MapError(KErrNone, anErrno);
		}

	if(sig < 1 || sig > SIGRTMAX)
		{
		return MapError(err, anErrno); // check for linux return value. standard says != 0
		}

	err = iSignalSession.SendSignal(pid,sig);
	return MapError(err, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::raise
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::raise(int sig , int& anErrno)
	{
	EnsureSignalsInitialized();

	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}

	TInt err = KErrArgument;
	if(sig < 1 || sig > SIGRTMAX)
		{
		return MapError(err, anErrno); // check for linux return value. standard says != 0
		}
	// mutex protect
	TSignalMessage lMsg;
	lMsg.mType = TSignalMessage::ESignal;
	lMsg.mSignal = (TUint8)sig;
	TBuf8<KSigMsgLength> lMsgBuf;
	lMsg.Marshall(lMsgBuf);

	err = iSignalWritePipe.Write(lMsgBuf,KSigMsgLength);
	if(err >= 0)
		return MapError(KErrNone,anErrno);
	else
		return MapError(err, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigqueue
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigqueue(pid_t pid, int sig, const union sigval val, int& anErrno)
	{
	EnsureSignalsInitialized();

	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}

	TInt err = KErrArgument;

	TProcessId lPid(pid);
	RProcess lTargetProcess;
	if(lTargetProcess.Open(lPid) != KErrNone)
		{
		anErrno = ESRCH;
		return -1;
		}
	else
		{
		lTargetProcess.Close();
		}

	if(sig == 0)
		{
		return MapError(KErrNone, anErrno);
		}

	if(sig < 1 || sig > SIGRTMAX)
		{
		return MapError(err, anErrno); // check for linux return value. standard says != 0
		}

	err = iSignalSession.SendSignalValue(pid,sig,val.sival_int);
	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigfillset
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigfillset(sigset_t *set, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	if(set == 0)
		return MapError(KErrArgument, anErrno);

	SIG_FILL_SIGSET(*set);

	return MapError(KErrNone, anErrno);
	}


// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigaddset
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigaddset(sigset_t *set, int signo, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	if(signo < 1 || signo > SIGRTMAX || set == 0)
		return MapError(KErrArgument, anErrno);

	SIG_ADD_SIGNAL_TO_SET(signo,*set);

	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigdelset
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigdelset(sigset_t *set, int signo, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	if(signo < 1 || signo > SIGRTMAX || set == 0)
		return MapError(KErrArgument, anErrno);

	SIG_REMOVE_SIGNAL_FROM_SET(signo,*set);

	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigismember
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigismember(const sigset_t *set, int signo, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	if(signo < 1 || signo > SIGRTMAX || set == 0)
		return MapError(KErrArgument, anErrno);

	if(SIG_SIGNAL_PRESENT_IN_SIGSET(signo,*set))
		return 1;
	else
		return 0;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigandset
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigandset(sigset_t * set, const sigset_t * left, const sigset_t * right, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
		
	if(set == 0 || left == 0 || right == 0)
		return MapError(KErrArgument, anErrno);

	*set = *left & *right;

	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigorset
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigorset(sigset_t * set, const sigset_t * left, const sigset_t * right, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	if(set == 0 || left == 0 || right == 0)
		return MapError(KErrArgument, anErrno);

	*set = *left | *right;

	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigisemptyset
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigisemptyset(const sigset_t * set, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	if(set == 0)
		return MapError(KErrArgument, anErrno);

	anErrno = 0;
	if(SIG_ISEMPTY_SIGSET(*set))
		return 1;
	else
		return 0;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigemptyset
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigemptyset(sigset_t * set, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	if(set == 0)
		return MapError(KErrArgument, anErrno);

	SIG_EMPTY_SIGSET(*set);
	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigprocmask
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigprocmask(int how, const sigset_t *set, sigset_t *oset, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	EnsureSignalsInitialized();

	TUint64 lOldMask = iSigMask;
	TBool lIsUnblocked = EFalse;

	if(oset)
		{
		*oset = (sigset_t)iSigMask;
		}

	if(set)
		{
		switch(how)
			{
			case SIG_BLOCK:
				iSigMask |= (TUint64)*set;
				break;
			case SIG_SETMASK:
				iSigMask = (TUint64)*set;
				lIsUnblocked = ETrue;
				break;
			case SIG_UNBLOCK:
				iSigMask &= ~(TUint64)*set;
				lIsUnblocked = ETrue;
				break;
			default:
				return MapError(KErrArgument, anErrno);
			}
		}

	// unsetting the bits of signals that cannot be blocked
	// no error is to be thrown in this case
	SIG_REMOVE_SIGNAL_FROM_SET(SIGKILL,iSigMask);
	SIG_REMOVE_SIGNAL_FROM_SET(SIGSTOP,iSigMask);

	if(lIsUnblocked)
		{
		TUint64 lDiffSet = (lOldMask & (~iSigMask));
		if(lDiffSet != 0)
			{
			TInt lIdx = 0;
			for(lIdx = 1;lIdx <= SIGRTMAX;lIdx++)
				{
				if(SIG_SIGNAL_PRESENT_IN_SIGSET(lIdx,lDiffSet))
					{
					TSignalMessage lMsg;
					lMsg.mType = TSignalMessage::EDequeueSignal;
					lMsg.mSignal = (TUint8)lIdx;
					TBuf8<KSigMsgLength> lMsgBuf;
					lMsg.Marshall(lMsgBuf);
					iSignalWritePipe.Write(lMsgBuf,KSigMsgLength);

					}
				}
			// scan and dequeue
			}
		}

	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sighold
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sighold(int signo, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	EnsureSignalsInitialized();

	if(signo < 1 || signo > SIGRTMAX)
		return MapError(KErrArgument, anErrno);

	SIG_ADD_SIGNAL_TO_SET(signo,iSigMask);
	SIG_REMOVE_SIGNAL_FROM_SET(SIGKILL,iSigMask);
	SIG_REMOVE_SIGNAL_FROM_SET(SIGSTOP,iSigMask);

	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigrelse
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigrelse(int signo, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
		
	EnsureSignalsInitialized();
	TUint64 lPrevMask = iSigMask;

	if(signo < 1 || signo > SIGRTMAX)
		return MapError(KErrArgument, anErrno);

	SIG_REMOVE_SIGNAL_FROM_SET(signo,iSigMask);

	if(lPrevMask != iSigMask)
		{
		TSignalMessage lMsg;
		lMsg.mType = TSignalMessage::EDequeueSignal;
		lMsg.mSignal = (TUint8)signo;
		TBuf8<KSigMsgLength> lMsgBuf;
		lMsg.Marshall(lMsgBuf);
		iSignalWritePipe.Write(lMsgBuf,KSigMsgLength);
		}

	return MapError(KErrNone, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigpause
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigpause(int signo, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
		
	EnsureSignalsInitialized();

	TUint64 lPrevMask = iSigMask;
	TUint64 lWaitSet = 0;
	TSignalMessage lMsg;

	if(signo < 1 || signo > SIGRTMAX)
		return MapError(KErrArgument, anErrno);

	SIG_REMOVE_SIGNAL_FROM_SET(signo,iSigMask);
	SIG_ADD_SIGNAL_TO_SET(signo,lWaitSet);

	SigWaitInternal(lWaitSet,&lMsg,0);

	iSigMask = lPrevMask;

	HandleSignal(lMsg,ETrue);

	anErrno = EINTR;
	return -1;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigwait
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigwait(const sigset_t *set, int *sig, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		return ENOTSUP;
		}
		
	EnsureSignalsInitialized();

	anErrno = anErrno; // to supress unused arg warning. sigwait doesn't set errno, it returns it.

	if(set == 0)
		return EINVAL;

	if(SIG_ISEMPTY_SIGSET(*set))
		return EINVAL;

	TUint64 lTemp = *set;
	SIG_REMOVE_SIGNAL_FROM_SET(SIGKILL,lTemp);
	SIG_REMOVE_SIGNAL_FROM_SET(SIGSTOP,lTemp);
	if(lTemp == 0 && !SIG_ISEMPTY_SIGSET(set))
		return 0;

	TInt lRetVal = KErrNone;
	TSignalMessage lMsg;
	lRetVal = SigWaitInternal((TUint64)*set,&lMsg,0);
	if(lRetVal == KErrNone)
		{
		if(lMsg.mType == TSignalMessage::ESignal)
			{
			if(sig)
				*sig = lMsg.mSignal;
			}
		else if(lMsg.mType == TSignalMessage::ESignalValuePair)
			{
			if(sig)
				*sig = lMsg.mSigVal.mSignal;
			}
		else if(lMsg.mType == TSignalMessage::ERtTimerSignal)
			{
			if(sig)
				*sig = lMsg.mRtSignal.mSignal;
			}
		}
	else
		{
		return EINVAL;
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigtimedwait
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigtimedwait(const sigset_t *set,
		siginfo_t *info, const struct timespec *timeout, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	TBool lTimeCheckFailed = EFalse;

	EnsureSignalsInitialized();

	if(set == 0 || timeout == 0)
		{
		anErrno = EINVAL;
		return -1;
		}

	if(SIG_ISEMPTY_SIGSET(*set))
		{
		anErrno = EINVAL;
		return -1;
		}

	if(timeout->tv_sec == 0 && timeout->tv_nsec == 0)
		{
		anErrno = EINVAL;
		return -1;
		}

	if( (timeout->tv_sec*1000*1000 > 0x7FFFFFFF) ||	// 0x7FFFFFFF (maximum positive value of int32)
			(timeout->tv_sec < 0) ||
			(timeout->tv_nsec >= 1000000000) ||
			(timeout->tv_nsec < 0) )
		{
		lTimeCheckFailed = ETrue;
		}

	TUint64 lTemp = *set;
	SIG_REMOVE_SIGNAL_FROM_SET(SIGKILL,lTemp);
	SIG_REMOVE_SIGNAL_FROM_SET(SIGSTOP,lTemp);
	if(lTemp == 0 && !SIG_ISEMPTY_SIGSET(set))
		return 0;

	TInt lTimeout = timeout->tv_sec*1000*1000 + timeout->tv_nsec/1000;
	if(lTimeout < 0)
		{
		lTimeCheckFailed = ETrue;
		}

	TInt lRetVal = KErrNone;
	TInt lSigNum = 0;
	TSignalMessage lMsg;

	lRetVal = SigWaitInternal((TUint64)*set,&lMsg,lTimeout,lTimeCheckFailed);

	if(lRetVal == KErrNone)
		{
		if(lMsg.mType == TSignalMessage::ESignal)
			{
			lSigNum = lMsg.mSignal;
			if(info)
				{
				info->si_signo = lSigNum;
				info->si_value.sival_int = 0;
				}
			}
		else if(lMsg.mType == TSignalMessage::ESignalValuePair)
			{
			lSigNum = lMsg.mSigVal.mSignal;
			if(info)
				{
				info->si_signo = lSigNum;
				info->si_value.sival_int = (int)lMsg.mSigVal.mValue;
				}
			}
		else if(lMsg.mType == TSignalMessage::ERtTimerSignal)
			{
			lSigNum = lMsg.mRtSignal.mSignal;
			if(info)
				{
				info->si_signo = lSigNum;
				info->si_value.sival_int = 0;
				}
			}
		}
	else if (lRetVal == KErrTimedOut)
		{
		anErrno = EAGAIN;
		return -1;
		}
	else
		{
		anErrno = EINVAL;
		return -1;
		}
	return lSigNum;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigwaitinfo
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigwaitinfo(const sigset_t *set, siginfo_t *info, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	EnsureSignalsInitialized();

	if(set == 0)
		{
		anErrno = EINVAL;
		return -1;
		}

	if(SIG_ISEMPTY_SIGSET(*set))
		{
		anErrno = EINVAL;
		return -1;
		}

	TUint64 lTemp = *set;
	SIG_REMOVE_SIGNAL_FROM_SET(SIGKILL,lTemp);
	SIG_REMOVE_SIGNAL_FROM_SET(SIGSTOP,lTemp);
	if(lTemp == 0 && !SIG_ISEMPTY_SIGSET(set))
		return 0;

	TInt lRetVal = KErrNone;
	TInt lSigNum = 0;
	TSignalMessage lMsg;

	lRetVal = SigWaitInternal((TUint64)*set,&lMsg,0);

	if(lRetVal == KErrNone)
		{
		if(lMsg.mType == TSignalMessage::ESignal)
			{
			lSigNum = lMsg.mSignal;
			if(info)
				{
				info->si_signo = lSigNum;
				info->si_value.sival_int = 0;
				}
			}
		else if(lMsg.mType == TSignalMessage::ESignalValuePair)
			{
			lSigNum = lMsg.mSigVal.mSignal;
			if(info)
				{
				info->si_signo = lSigNum;
				info->si_value.sival_int = (int)lMsg.mSigVal.mValue;
				}
			}
		else if(lMsg.mType == TSignalMessage::ERtTimerSignal)
			{
			lSigNum = lMsg.mRtSignal.mSignal;
			if(info)
				{
				info->si_signo = lSigNum;
				info->si_value.sival_int = 0;
				}
			}		
		else
			{
			anErrno = EINVAL;
			return -1;
			}
		}

	return lSigNum;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigaction
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigaction(int aSigNum, const struct sigaction *act, struct sigaction *oact, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	EnsureSignalsInitialized();

	if(aSigNum < 1 || aSigNum > SIGRTMAX || aSigNum == SIGKILL ||  aSigNum == SIGSTOP)
		{
		return MapError(KErrArgument,anErrno);
		}

	if(act && (act->sa_flags & (SA_NOCLDSTOP | SA_ONSTACK | SA_RESTART| SA_NOCLDWAIT))) //Whar abt SA_NODEFER
		{
		anErrno = ENOTSUP;
		return -1;
		}

	iSigHandlerMutex.Wait();
	if(oact)
		{
		if( (iSigActions[aSigNum-1].iSigFlags & SA_SIGINFO)
				&& iSigActions[aSigNum-1].iSigAction)
			{
			oact->sa_flags = iSigActions[aSigNum-1].iSigFlags;
			oact->sa_sigaction = iSigActions[aSigNum-1].iSigAction;
			}
		else
			{
			oact->sa_flags = iSigActions[aSigNum-1].iSigFlags;
			oact->sa_handler = iSigActions[aSigNum-1].iSigHandler;
			}
		}

	if(act)
		{
		if(act->sa_flags & SA_SIGINFO)
			{
			iSigActions[aSigNum-1].iSigAction = act->sa_sigaction;
			iSigActions[aSigNum-1].iSigFlags = act->sa_flags;
			}
		else
			{
			iSigActions[aSigNum-1].iSigHandler = act->sa_handler;
			iSigActions[aSigNum-1].iSigAction = 0;
			iSigActions[aSigNum-1].iSigFlags = act->sa_flags;
			}
		}
	iSigHandlerMutex.Signal();

	return MapError(KErrNone,anErrno);

	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigset
// -----------------------------------------------------------------------------
//
TSignalHandler CLocalSystemInterface::sigset(int aSigNum, TSignalHandler disp, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return SIG_ERR;
		}
	
	TUint64 lOldMask = iSigMask;
	EnsureSignalsInitialized();

	if(aSigNum < 1 || aSigNum > SIGRTMAX || aSigNum == SIGKILL ||  aSigNum == SIGSTOP)
		{
		anErrno = EINVAL;
		return SIG_ERR;
		}
	TSignalHandler lOldHandler = iSigActions[aSigNum-1].iSigHandler;

	if(disp == SIG_HOLD)
		{
		SIG_ADD_SIGNAL_TO_SET(aSigNum,iSigMask);
		}
	else
		{
		iSigHandlerMutex.Wait();
		iSigActions[aSigNum-1].iSigHandler = disp;
		iSigActions[aSigNum-1].iSigAction = 0;
		iSigActions[aSigNum-1].iSigFlags = 0;
		iSigHandlerMutex.Signal();
		SIG_REMOVE_SIGNAL_FROM_SET(aSigNum,iSigMask);
		}

	if(SIG_SIGNAL_PRESENT_IN_SIGSET(aSigNum,lOldMask))
		return SIG_HOLD;
	else
		return lOldHandler;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::bsd_signal
// -----------------------------------------------------------------------------
//
TSignalHandler CLocalSystemInterface::bsd_signal(int aSigNum, TSignalHandler aFunc,int& anErrno)
	{
	return signal(aSigNum,aFunc,anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::signal
// -----------------------------------------------------------------------------
//
TSignalHandler CLocalSystemInterface::signal(int aSigNum, TSignalHandler aFunc,int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return SIG_ERR;
		}
	
	EnsureSignalsInitialized();

	if(aSigNum < 1 || aSigNum > SIGRTMAX || aSigNum == SIGKILL ||  aSigNum == SIGSTOP)
		{
		anErrno = EINVAL;
		return SIG_ERR;
		}

	iSigHandlerMutex.Wait();
	TSignalHandler lOldHandler = iSigActions[aSigNum-1].iSigHandler;
	iSigActions[aSigNum-1].iSigHandler = aFunc;
	iSigActions[aSigNum-1].iSigAction = 0;
	iSigActions[aSigNum-1].iSigFlags = 0;
	iSigHandlerMutex.Signal();
	return lOldHandler;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigignore
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigignore(int aSigNum, int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	EnsureSignalsInitialized();

	if(aSigNum < 1 || aSigNum > SIGRTMAX || aSigNum == SIGKILL ||  aSigNum == SIGSTOP)
		{
		anErrno = EINVAL;
		return -1;
		}

	iSigHandlerMutex.Wait();
	iSigActions[aSigNum-1].iSigHandler = SIG_IGN;
	iSigActions[aSigNum-1].iSigAction = 0;
	iSigActions[aSigNum-1].iSigFlags = 0;
	iSigHandlerMutex.Signal();
	return 0;
	}
// -----------------------------------------------------------------------------
// CLocalSystemInterface::sigpending
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::sigpending(sigset_t *set,int& anErrno)
	{
	if(!iSignalsInitialized)
		{
		anErrno = ENOTSUP;
		return -1;
		}
	
	EnsureSignalsInitialized();

	if(set == 0)
		{
		anErrno = EFAULT;
		return -1;
		}

	SIG_EMPTY_SIGSET(*set);

	TInt lIdx = 0;

	for(lIdx = 0;lIdx < iBlockedSigArray.Count(); lIdx++)
		{
		TUint8 lSigNum = 0;
		if(iBlockedSigArray[lIdx].mType == TSignalMessage::ESignal)
			lSigNum = iBlockedSigArray[lIdx].mSignal;
		else if(iBlockedSigArray[lIdx].mType == TSignalMessage::ESignalValuePair)
			lSigNum = iBlockedSigArray[lIdx].mSigVal.mSignal;
		else if(iBlockedSigArray[lIdx].mType == TSignalMessage::ERtTimerSignal)
			lSigNum = iBlockedSigArray[lIdx].mRtSignal.mSignal;
		SIG_ADD_SIGNAL_TO_SET(lSigNum,*set);
		}

	return MapError(KErrNone,anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::alarm
// -----------------------------------------------------------------------------
//
unsigned int CLocalSystemInterface::alarm(unsigned int seconds)
	{
	if(!iSignalsInitialized)
		{
		return 0;
		}
	
	EnsureSignalsInitialized();

	TTime lCurrentTime;
	TTimeIntervalSeconds lTimeLapse;

	unsigned int lTimeLeft = 0;

	if(iIsTimerActive)
		{
		lCurrentTime.HomeTime();
		lCurrentTime.SecondsFrom(iAlarmStartTime,lTimeLapse);
		lTimeLeft = iCurrentTimeout - lTimeLapse.Int();
		if((int)lTimeLeft < 0)
			lTimeLeft = 0;
		}

	iCurrentTimeout = seconds;
	TSignalMessage lMsg;
	lMsg.mType = TSignalMessage::EAlarmRegistration;
	lMsg.mTimeOut = seconds;
	TBuf8<KSigMsgLength> lMsgBuf;
	lMsg.Marshall(lMsgBuf);
	iSignalWritePipe.Write(lMsgBuf,KSigMsgLength);

	return lTimeLeft;
	}


#ifdef SYMBIAN_OE_LIBRT
// -----------------------------------------------------------------------------
// CLocalSystemInterface::FindTimer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLocalSystemInterface::FindTimer(const TInt& aTimerId) 
	{
	iTimerOverrunsMutex.Wait();

	TInt lIndex = KErrNotFound;
	TInt lTimerCnt = iTimerOverruns.Count();
	for(TInt lIdx = 0;  lIdx < lTimerCnt; lIdx++)
		{
		if(iTimerOverruns[lIdx].iTimerId == aTimerId)
			{
			lIndex = lIdx;
			break;
			}
		}

	iTimerOverrunsMutex.Signal();	
	return lIndex;	
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::AddTimer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLocalSystemInterface::AddTimer(const TInt& aTimerId)
	{
	TInt err;
	if (FindTimer(aTimerId) >= 0)
		{
		err = KErrNone;
		}
	else
		{
		TOverrun lOverrun(aTimerId);
		iTimerOverrunsMutex.Wait();
		RHeap* oHeap = User::SwitchHeap(iPrivateHeap);
		err = iTimerOverruns.Append(lOverrun);
		User::SwitchHeap(oHeap);				
		if(!err)
			{
			iTimerOverruns[iTimerOverruns.Count()-1].iTimerId = aTimerId;	
			}

		iTimerOverrunsMutex.Signal();
		}	

	return err;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::DeleteTimer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLocalSystemInterface::DeleteTimer(const TInt& aTimerId)
	{
	TInt lRet = FindTimer(aTimerId);
	if(lRet >= 0)
		{
		iTimerOverrunsMutex.Wait();		
		RHeap* oHeap = User::SwitchHeap(iPrivateHeap);
		iTimerOverruns.Remove(lRet);	
		User::SwitchHeap(oHeap);				
		iTimerOverrunsMutex.Signal();

		return KErrNone;
		}

	return lRet;	
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::IncrementOverrun
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLocalSystemInterface::IncrementOverrun(const TInt& aTimerId)
	{
	TInt lRet = FindTimer(aTimerId);
	if(lRet >= 0)
		{
		iTimerOverrunsMutex.Wait();				
		iTimerOverruns[lRet].Overruns++;
		iTimerOverrunsMutex.Signal();
		return iTimerOverruns[lRet].Overruns;
		}

	return lRet;	
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::ResetOverrun
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLocalSystemInterface::ResetOverrun(const TInt& aTimerId)
	{
	TInt lRet = FindTimer(aTimerId);
	if(lRet >= 0)
		{
		iTimerOverrunsMutex.Wait();						
		iTimerOverruns[lRet].Overruns = 0;
		iTimerOverrunsMutex.Signal();
		return KErrNone;		
		}

	return KErrNotFound;	

	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::Overrun
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLocalSystemInterface::Overrun(const TInt& aTimerId)
	{
	TInt lRet = FindTimer(aTimerId);
	if(lRet >= 0)
		{
		return iTimerOverruns[lRet].Overruns;
		}

	return KErrNotFound;	
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::RaiseTimerSignal
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLocalSystemInterface::RaiseTimerSignal(int sig, int timerid)
	{
	EnsureSignalsInitialized();

	TInt err = KErrArgument;
	if(sig < 1 || sig > SIGRTMAX)
		{
		return err;
		}
	// mutex protect
	TSignalMessage lMsg;
	lMsg.mType = TSignalMessage::ERtTimerSignal;
	lMsg.mRtSignal.mSignal = (TUint8)sig;
	lMsg.mRtSignal.mTimerId = timerid;
	TBuf8<KSigMsgLength> lMsgBuf;
	lMsg.Marshall(lMsgBuf);

	err = iSignalWritePipe.WriteBlocking(lMsgBuf,KSigMsgLength);
	if(err >= 0)
		return KErrNone;

	return err;
	}

#endif //SYMBIAN_OE_LIBRT	

#endif // SYMBIAN_OE_POSIX_SIGNALS

// -----------------------------------------------------------------------------
// CLocalSystemInterface::GetDirName
// Implementation for GetDirName
// This will return the name of the directory corresponding to the aFid
// This wont set any error code in case of error.
// Returns the name on success else NULL
// -----------------------------------------------------------------------------
//
const wchar_t* CLocalSystemInterface::GetDirName (int aFid)
	{
	//Get the CFileDescBase at the index aFid
	CFileDescBase* desc = NULL;
	TInt err = iFids.At(aFid, desc);
	if ( !err )
		{
		//Cast it back to CDirectoryDesc
		CDirectoryDesc* dirDesc = static_cast<CDirectoryDesc*> (desc);
		return dirDesc->GetDirName();
		}
	return (const wchar_t*)NULL;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::Truncate
// Implementation for Truncate
// This will Truncate a file represented by aFid
// Returns the success/error code on completion
// -----------------------------------------------------------------------------
//
EXPORT_C int CLocalSystemInterface::Truncate(int aFid, off_t anOffset, int& anErrno)
	{
	TInt err = KErrArgument;
#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
	off_t offset = anOffset;
#else
	TInt offset = (TInt)anOffset;
#endif //SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS
	//Proceed only if offset is non-negetive number
	if ( offset >= 0 )
		{//Get the CFileDescBase at the index aFid
		CFileDescBase* desc = NULL;
		err = iFids.At(aFid, desc);
		if ( !err )
			{
			err = desc->Truncate(offset);
			if( err == KErrAccessDenied )
				{
				//Ensure that, it will set errno as EINVAL
				err = KErrArgument;
				}
			}
		}

	return MapError(err, anErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::GetDesc
// Implementation for GetDesc
// This will return the CFileDescBase object corresponding to aFid
// -----------------------------------------------------------------------------
//
EXPORT_C CFileDescBase* CLocalSystemInterface::GetDesc(int aFid)
	{
	CFileDescBase* fileDesc = NULL;
	//coverity[unchecked_value]
	iFids.At(aFid, fileDesc);
	return fileDesc;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::FileSession
// This will return the file session
// -----------------------------------------------------------------------------
//
EXPORT_C RFs& CLocalSystemInterface::FileSession()
	{
	return iFs;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::CommServSession
// This will return the communication server session
// -----------------------------------------------------------------------------
//
RCommServ& CLocalSystemInterface::CommServSession()
	{
	return iCs;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::SockServSession
// This will return the socket server session
// -----------------------------------------------------------------------------
//
RSocketServ& CLocalSystemInterface::SockServSession()
	{
	return iSs;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::Alloc
// Dynamic Memory related APIs
// Allocates memory in the private heap
// -----------------------------------------------------------------------------
//
EXPORT_C void* CLocalSystemInterface::Alloc(size_t aNBytes)
	{
	return iPrivateHeap->AllocZ(aNBytes);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::ReAlloc
// Dynamic Memory related APIs
// Allocates memory in the private heap
// -----------------------------------------------------------------------------
//
EXPORT_C void* CLocalSystemInterface::ReAlloc(void* aPtr, size_t aNBytes)
	{
	return iPrivateHeap->ReAlloc(aPtr, aNBytes);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::Free
// Dynamic Memory related APIs
// Frees memory in the private heap
// -----------------------------------------------------------------------------
//
EXPORT_C void CLocalSystemInterface::Free(void* aPtr)
	{
	iPrivateHeap->Free(aPtr);
	}


// -----------------------------------------------------------------------------
// CLocalSystemInterface::AllocTLD
// Dynamic Memory related APIs
// Allocates memory for the thread's TLD and cleans up TLD of dead threads
// -----------------------------------------------------------------------------
//
EXPORT_C void* CLocalSystemInterface::AllocTLD(TInt aSize)
	{
	// Allocate memory for reent structure on private heap
	void* ptr = iPrivateHeap->AllocZ(aSize);

	iTLDListLock.Wait();
	RHeap* oheap = User::SwitchHeap(iPrivateHeap);

	TThreadId caller = RThread().Id();
	RThread thrd;

	TInt idx = -1;

	TInt tcount  = iTLDInfoList.Count();

	for (int i = 0; i < tcount; ++i)
		{
		TTLDInfo& tldinf = iTLDInfoList[i];

		TInt ptrcount = tldinf.iPtrs.Count();

		if (!ptrcount)
			{
			// This slot has already been freed. Save here?
			idx = i;
			continue;
			}

		TInt err = thrd.Open(tldinf.iTid);

		if (err != KErrNone || thrd.ExitType() != EExitPending)
			{
			// This thread is dead. Bury it.
			for (int j = 0; j < ptrcount; ++j)
				{
				iPrivateHeap->FreeZ(tldinf.iPtrs[j]);
				}

			(tldinf.iPtrs).Reset();

			// this slot can be reused. Save here?
			idx = i;
			}
		else
			{
			// Points to a live thread
			if (tldinf.iTid == caller)
				{
				// This is the calling thread. We must save the TLD ptr here
				idx = i;
				}
			}
		}

	// Only if the initial allocation succeded
	if (ptr != NULL)
		{
		// No free slot. No cleaned-up slots.
		if (idx == -1)
			{
			iTLDInfoList.Append(TTLDInfo(caller, ptr));
			}
		else
			{
			// Wouldn't hurt even if it is already set
			iTLDInfoList[idx].iTid = caller;
			iTLDInfoList[idx].iPtrs.Append(ptr);
			}
		}

	User::SwitchHeap(oheap);
	iTLDListLock.Signal();

	return ptr;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::Heap
//Return the private Heap memory
// -----------------------------------------------------------------------------
//
EXPORT_C RHeap* CLocalSystemInterface::Heap()
	{
	return iPrivateHeap;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::CommsSessionLock
//Return the Comms Server Lock
// -----------------------------------------------------------------------------
//
RFastLock& CLocalSystemInterface::CommsSessionLock()
	{
	return iCSLock;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::CommsSessionLock
//Return reference to file table
// -----------------------------------------------------------------------------
//
const CFileTable& CLocalSystemInterface::FileTable() const
{
return iFids;
}

// ---------------------------------------------------------------------------------
// CLocalSystemInterface::WaitForNRequest
// Wait for any one of the input asynchronous requests to complete
// Used in lieu of User::WaitForNRequest because of need to support pre-Argus builds
// ---------------------------------------------------------------------------------
//
void CLocalSystemInterface::WaitForNRequest(TRequestStatus aStatusArray[], TInt aNum)
	{
	if (aNum)
		{
		// used to keep count of requests we have not been asked to wait for
		TInt nOther = -1;
		TBool done = EFalse;

		do
			{
			++nOther;
			User::WaitForAnyRequest();
			for (TInt i = 0; i < aNum; ++i)
				{
				if (aStatusArray[i].Int() != KRequestPending)
					{
					done = ETrue;
					break;
					}
				}
			} while (!done);

		if (nOther)
			{
			// Adjust the thread's signal semaphore to account for the requests
			// we were not asked to wait for.
			RThread thrd;
			for (TInt i = 0; i < nOther; ++i)
				{
				thrd.RequestSignal();
				}
			}
		}
	}
/*
CLocalSystemInterface::AddToDirList
Appends  Open Directory handle to a RPointerArray array and retuns 0
 */

int CLocalSystemInterface::AddToDirList(DIR *aDir)
	{
	RHeap* oheap = User::SwitchHeap(iPrivateHeap);
	TInt ret = iOpenDirList.Append(aDir);
	User::SwitchHeap(oheap);
	return ret;
	}


/*
CLocalSystemInterface::RemoveFromDirList
Remove the Directory handle from the array and closes the array.
 */

int CLocalSystemInterface::RemoveDirFromList(DIR* aDir)
	{

	TInt ret =	iOpenDirList.Find(aDir);
	if(ret != KErrNotFound)
		{
		RHeap* oheap = User::SwitchHeap(iPrivateHeap);
		iOpenDirList.Remove(ret);
		User::SwitchHeap(oheap);
		}
	return ret;
	}



/*
TBool CLocalSystemInterface::ValidateInDirList
Validates the Directory handle with the array and returns ETrue upon success
else returns EFalse.
 */

int CLocalSystemInterface::FindInDirList(DIR* aDir)
	{
	return(iOpenDirList.Find(aDir));
	}

//
//int CLocalSystemInterface::SetEcho(int aFd, TBool aEcho, int& aErrno)
//
//Set the echo flag of the suppported console abstractions matching
//the given fd.
//
int CLocalSystemInterface::SetEcho(int aFd, TUint8 aEcho, int& aErrno)
	{
	return iFids.SetEcho(aFd, aEcho, aErrno);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::ReadRecordFromIapTableL
//
// Helper function to read a record from the IAP table
// -----------------------------------------------------------------------------
//
void CLocalSystemInterface::ReadRecordFromIapTableL(CCommsDbTableView* aView, TAccessPointRecord &aRecord)
	{
	aView->ReadUintL(TPtrC(COMMDB_ID), aRecord.iId);
	aView->ReadTextL(TPtrC(COMMDB_NAME), aRecord.iName);
	aView->ReadUintL(TPtrC(IAP_BEARER), aRecord.iBearer);
	aView->ReadUintL(TPtrC(IAP_NETWORK), aRecord.iNetwork);
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::GetConnectionPreferencesL
//
// Get the connection preferences from an IAP name.
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::GetConnectionPreferencesL(TBuf<KCommsDbSvrMaxColumnNameLength> aIapName,
		TCommDbConnPref &aConnPref)
	{
	TInt err = KErrNone;

	//Open the database and create the IAP view.
	CCommsDatabase *apDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(apDb);
	apDb->ShowHiddenRecords();
	//The following pushes the view onto the cleanup stack
	CCommsDbTableView *view = apDb->OpenTableLC(TPtrC(IAP));

	//Iterate through the records to find the matching entry
	TAccessPointRecord apRecord;
	err = view->GotoFirstRecord();
	while (err == KErrNone)
		{
		//The following function will leave in case of error
		ReadRecordFromIapTableL(view, apRecord);
		if (aIapName == apRecord.iName)
			break;

		err = view->GotoNextRecord();
		}

	CleanupStack::PopAndDestroy(); //Free the view
	CleanupStack::PopAndDestroy(apDb); //Free the db itself

	if(err != KErrNone) //Record not found
		return KErrNotFound;

	aConnPref.SetIapId(apRecord.iId);
	aConnPref.SetNetId(apRecord.iNetwork);
	aConnPref.SetBearerSet(apRecord.iBearer);
	aConnPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	aConnPref.SetDirection(ECommDbConnectionDirectionUnknown);

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::StartDefConnection
//
// Helper function for the setdefaultif() API to start the 
// default RConnection with/without preferences
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::StartDefConnection()
	{
	TInt err = iDefConnection.Open(iSs);
	if( err != KErrNone )
		return err;
	// If connection preference is set
	if (iDefConnPref)
	    {
	    err = iDefConnection.Start(*iDefConnPref);
	    }
	else // No connection preference available
	    {
	    err = iDefConnection.Start();
	    }
	if (err != KErrNone)
		iDefConnection.Close();
	return err;
	}

TInt CLocalSystemInterface::unsetdefaultif(TBool allow_bringup)
    {
    iDefConnLock.Wait();
    if (iDefConnection.SubSessionHandle() != 0)
        {
        TUint count = iSocketArray.Count();            
        for (TInt i = 0; i < count; ++i)
            {                
            iSocketArray[i]->TempClose();
            }
        iDefConnection.Close();
        }
    
    RHeap* oheap = User::SwitchHeap(iPrivateHeap);
    iSocketArray.Reset();
    
    if (iDefConnPref)
        {
        if (iDefConnPref->ExtensionId() == TConnPref::EConnPrefSnap)
            {
            delete (TCommSnapPref*)iDefConnPref;
            }
        else
            {
            delete (TCommDbConnPref*)iDefConnPref;
            }
        }
    User::SwitchHeap(oheap);
    
    iDefConnPref = NULL;
    iDefConnResurrect = allow_bringup;
    iDefConnLock.Signal();
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CLocalSystemInterface::setdefaultif
//
// Set the default interface for network operations
// -----------------------------------------------------------------------------
//
int CLocalSystemInterface::setdefaultif(const struct ifreq* aIfReq)
    {
    // Do this in any case - whether the argument be a valid pref or NULL
    iDefConnResurrect = ETrue;
    
    // If the argument is NULL, tear down existing connection
    if (aIfReq == NULL)
        {
        return unsetdefaultif();
        }

    TPtrC8 namePtr((TText8*)aIfReq->ifr_name);
    TBuf<KCommsDbSvrMaxColumnNameLength> name;
    
    TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(name, namePtr);
    if (err != KErrNone)
        return err;
    
    if (name.Length() == 0)
        {
        // interface name is an empty string, SNAP id is specified in ifr_ifru.snap_id
        if (iDefConnPref && iDefConnPref->ExtensionId() == TConnPref::EConnPrefSnap)
            {
            ((TCommSnapPref*)iDefConnPref)->SetSnap(aIfReq->ifr_ifru.snap_id);
            return KErrNone;
            }
        
        RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);
        delete (TCommDbConnPref*)iDefConnPref; // may be a delete(NULL)
        iDefConnPref = new TCommSnapPref;
        User::SwitchHeap(oldHeap);

        if (!iDefConnPref)
            {
            return KErrNoMemory;
            }
        
        ((TCommSnapPref*)iDefConnPref)->SetSnap(aIfReq->ifr_ifru.snap_id);
        return KErrNone;
        }
    
    CTrapCleanup *cleanupStack = NULL;
    
    // Create a cleanup stack if one doesn't exist
    if (User::TrapHandler() == NULL)
        {
        cleanupStack = CTrapCleanup::New();
        if (cleanupStack == NULL)
            return KErrNoMemory;
        }
    
    if (iDefConnPref && iDefConnPref->ExtensionId() == TConnPref::EConnPrefCommDb)
        {
        TRAP(err, (err = GetConnectionPreferencesL(name, *(TCommDbConnPref*)iDefConnPref)));
        }
    else
        {
        RHeap *oldHeap = User::SwitchHeap(iPrivateHeap);
        delete (TCommSnapPref*)iDefConnPref;
        iDefConnPref = new TCommDbConnPref;
        User::SwitchHeap(oldHeap);
        
        if (iDefConnPref)
            {
            TRAP(err, (err = GetConnectionPreferencesL(name, *(TCommDbConnPref*)iDefConnPref)));
            }
        }
    
    delete cleanupStack;
    
    return (iDefConnPref ? KErrNone : KErrNoMemory);
    }


// -----------------------------------------------------------------------------
// CLocalSystemInterface::GetDefaultConnection
//
// Function to get the default connection instance from backend.
// The caller should check the handle before using the instance.
// -----------------------------------------------------------------------------
//
RConnection& CLocalSystemInterface::GetDefaultConnection()
    {
    // If GetDefaultConnection is called without calling
    // setdefaultif then the connection is started without any preferences
    // Obtain lock on the iDefConnection
    iDefConnLock.Wait();
    if(iDefConnection.SubSessionHandle() == 0 && iDefConnResurrect)
        {
        StartDefConnection();
        }
    // Release lock on the iDefConnection
    iDefConnLock.Signal();
   return iDefConnection;
    }

// -----------------------------------------------------------------------------
// CLocalSystemInterface::GetSessionPath
//
// Retrieves the cached session path from the backend
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::GetSessionPath(TDes& aPath)
	{
	if(aPath.MaxLength() < iSessionPath.Length())
		return KErrArgument;

	iSessionPathLock.Wait();
	aPath.Copy(iSessionPath);
	iSessionPathLock.Signal();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::SaveSessionPath
//
// Caches the session path in the backend
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::SaveSessionPath(const TDesC& aPath)
	{
	if(aPath.Length() > KMaxFileName)
		return KErrArgument;

	iSessionPathLock.Wait();
	iSessionPath.Copy(aPath);
	iSessionPathLock.Signal();

	return KErrNone;
	}
// -----------------------------------------------------------------------------
// handleASelect(TAny* aPtr);
// This function is called to handle aselect, aselect is implemented by
// running select on a different thread, this function is the thread function
// that is used for implementing aselect 
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::handleaselect(TAny* aPtr)
	{
	int errval;
	// Call select, when it returns signal the TRequestStatus
	// Extract request info
	TaselectRequestInfo *requestInfo = (TaselectRequestInfo *)aPtr;	
	RArray<TASelectRequest>& selectRequest = Backend()->ASelectRequest();
	RFastLock& selectLock = Backend()->ASelectLock();
	// Call select
	TInt nDesReady = Backend()->select(requestInfo->iMaxFd, requestInfo->iReadFds,requestInfo->iWriteFds,requestInfo->iExceptFds, requestInfo->itvptr,errval);
	// Open the main thread, this is required to signal it after processing	
	RThread mainThread;
	TInt err = mainThread.Open(requestInfo->iId,EOwnerProcess);
	if( err == KErrNone )
		{
		for ( TInt i=0; i<selectRequest.Count(); i++ )
			{
			if( selectRequest[i].iRequestStatus == requestInfo->iRequestStatus )
				{
				// Acquire a lock on the iSelectRequest object
				selectLock.Wait();
				// Switch to backend heap
				RHeap* oldHeap = User::SwitchHeap(Backend()->Heap());
				selectRequest.Remove(i);
				// Switch back to old heap
				User::SwitchHeap(oldHeap);
				// Release the lock
				selectLock.Signal();
				break;
				}
			}
		// Check if select has failed		
		if( nDesReady == -1 )
			{
			// There is an error, we need to propogate the errno
			// Convert errno to negative value
			errval = errval * -1;
			mainThread.RequestComplete(requestInfo->iRequestStatus,errval);
			}
		else
			{
			// Complete the request on the main thread and the TRequestStatus specified
			// in the requestInfo		
			mainThread.RequestComplete(requestInfo->iRequestStatus,nDesReady);			
			}
		mainThread.Close();
		}
	// Release the memory used for the TASelectRequestInfo
	Backend()->Free(aPtr);
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLocalSystemInterface::CreateASelectThread
// -----------------------------------------------------------------------------
//
TInt CLocalSystemInterface::CreateaselectThread(int maxfd, fd_set* readfds, fd_set* writefds,
		fd_set* exceptfds, struct timeval* tvptr, 
		TRequestStatus* requeststatus, int& anErrno)
	{
	// Create a aselect request holder, this will be destroyed by the new thread#
	// after processing
	TaselectRequestInfo *requestInfo = (TaselectRequestInfo* )Backend()->Alloc(sizeof(TaselectRequestInfo));
	// request info will be null if dynamic creation fails
	if( !requestInfo )
		{
		anErrno = ENOMEM;
		return -1;
		}
	// Set the aselect request parameters
	RThread currentThread;
	requestInfo->iId = currentThread.Id();
	currentThread.Close();
	// Fill the request struct
	requestInfo->iMaxFd = maxfd;
	requestInfo->iReadFds = readfds;
	requestInfo->iWriteFds = writefds;
	requestInfo->iExceptFds = exceptfds;
	requestInfo->itvptr = tvptr;
	requestInfo->iRequestStatus = requeststatus;
	*(requestInfo->iRequestStatus) = KRequestPending;
	RThread thread;
	// We create an anonymous thread
	TInt err = thread.Create(KNullDesC, CLocalSystemInterface::handleaselect, 4096, KMinHeapSize,
			256*KMinHeapSize, requestInfo);
	if ( err != KErrNone )
		{
		anErrno = ENOMEM;
		return -1;
		}

	// Create a aselect request object
	TASelectRequest aselectrequest;
	// retrive the service thread handle 
	aselectrequest.iThreadId = thread.Id();
	// Store the TRequestStatus used for the request
	aselectrequest.iRequestStatus = requeststatus;
	// Obtain the lock
	iASelectLock.Wait();
	// Switch to backend heap
	RHeap* oldHeap = User::SwitchHeap(iPrivateHeap);
	// Append to the array	
	iASelectRequest.Append(aselectrequest);
	// Switch back to old heap
	User::SwitchHeap(oldHeap);
	// Release the lock
	iASelectLock.Signal();
	// Execute the thread
	thread.Resume();
	// Close the handle
	thread.Close();	
	return MapError(KErrNone, anErrno);
	}

