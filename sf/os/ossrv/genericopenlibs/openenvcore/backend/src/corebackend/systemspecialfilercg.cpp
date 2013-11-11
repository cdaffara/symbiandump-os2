/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of system special file recognizer.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <liblogger.h>

#include "systemspecialfilercg.h"
#include "sysif.h"
#include "lposix.h"
#include "fdesc.h"

// EXTERNAL DATA STRUCTURES
#ifndef EMULATOR
const char *KFileEncodeStr= "deadbeef";
const char *Kkey= "XXXXXXXX";
#else //EMULATOR

const char * const KFileEncodeStr= "deadbeef";
const char * const Kkey= "XXXXXXXX";
#endif //EMULATOR


// -----------------------------------------------------------------------------
// _EncodeSystemSplFileMagicHeader
// Encode the system special file magic header.
// -----------------------------------------------------------------------------
//
int _EncodeSystemSplFileMagicHeader(
                    struct SSpecialFileMagicHeader *aEncBuf, 
                    TSpecialFileType aFileType)
    {
    if ( aEncBuf == NULL 
        || (aFileType <= EFileTypeGeneral  || aFileType >= EFileTypeMax))
        {
        return -1;
        }
    TInt counter = 0;
    for (counter = 0; counter < KMaxEncDecBuf - 1; counter++) 
        {
        aEncBuf->iMagicBuffer[counter] = (KFileEncodeStr[counter] ^ Kkey[counter]);
        }
    aEncBuf->iMagicBuffer[counter] = 0;
    
    aEncBuf->iFileType = aFileType;
    
    return 0;
    }

// -----------------------------------------------------------------------------
// _SystemSpecialFileBasedBuffer
// Try to retrieve file type from buffer.
// -----------------------------------------------------------------------------
//
TSpecialFileType _SystemSpecialFileBasedBuffer(const char *buf)
    {
    struct SSpecialFileMagicHeader fInfo;
    
    TInt i = 0;
    for (i = 0; i < KMaxEncDecBuf - 1; i++) 
        {
        fInfo.iMagicBuffer[i] = (*buf++ ^ Kkey[i]);
        }
    fInfo.iMagicBuffer[i] = *buf++;  
    fInfo.iFileType = *buf;
    
    if(StrCmp(fInfo.iMagicBuffer, KFileEncodeStr) == 0) 
        {
        return (TSpecialFileType)fInfo.iFileType;
        }
    return EFileTypeGeneral;
    }
    
// -----------------------------------------------------------------------------
// _SystemSpecialFileBasedFilePath
// Try to retrieve file type from file name (wchar_t*).
// -----------------------------------------------------------------------------
//
EXPORT_C TSpecialFileType _SystemSpecialFileBasedFilePath
        (const wchar_t* aPathName, TInt& aErrno, RSessionBase& aSession)
    {
    TSpecialFileType retVal = EFileTypeGeneral;
    TFullName fullName;
    RFILE file;
    RFs& fsSession = static_cast<RFs&>(aSession);
	aErrno = GetFullFile(fullName,(const TText16*)aPathName,fsSession);
	if (!aErrno)
	    {
	    TInt encSize = sizeof(struct SSpecialFileMagicHeader);
	    TBuf8<sizeof(struct SSpecialFileMagicHeader)> encBuf;
	    //Read the file header without opening it
	    aErrno = fsSession.ReadFileSection(fullName, 0, encBuf, encSize);
	    switch (aErrno)
	        {
	        case KErrNone:
    	        retVal = _SystemSpecialFileBasedBuffer((const char*)encBuf.Ptr());
    	        break;
	        case KErrNotFound:
	            retVal = EFileNotFound;
	            break;
	        case KErrPathNotFound:
	        	retVal = EFileNotFound;
	            break; 
	        default:
	            retVal = EFileGeneralError;
	            break;
	        }
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// _DeleteSystemSpecialFileBasedFilePath
// delete a system file (wide char).
// -----------------------------------------------------------------------------
//
TInt _DeleteSystemSpecialFileBasedFilePath
        (const wchar_t* aPathName, RSessionBase& aSession)
    {
    TSpecialFileType fileType = EFileTypeGeneral;
    TFullName fullName;
    RFILE file;
    RFs& fsSession = static_cast<RFs&>(aSession);
	TInt err=GetFullFile(fullName,(const TText16*)aPathName,fsSession);
	if (!err)
	    {
	    TInt encSize = sizeof(struct SSpecialFileMagicHeader);
	    TBuf8<sizeof(struct SSpecialFileMagicHeader)> encBuf;
	    err = fsSession.ReadFileSection(fullName, 0, encBuf, encSize);
	    if (err == KErrNone)
	        {
	        fileType = _SystemSpecialFileBasedBuffer((const char*)encBuf.Ptr());
	        if (fileType > EFileTypeGeneral && fileType < EFileTypeMax)
	            {
	            err = fsSession.Delete(fullName);
	            }
	        }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// _CreateSysSplFile
// Create a system file.
// -----------------------------------------------------------------------------
//
int _CreateSysSplFile(const wchar_t *pathName, 
                    const char* aData, 
                    const int aLen,
                    int& anErrno,
                    RSessionBase& aSession)
    {
    TFullName fullName;
    RFILE file;
    mode_t mode;
    RFs& fsSession = static_cast<RFs&>(aSession);
	TInt err=GetFullFile(fullName,(const TText16*)pathName,fsSession);
	
	TParse path;
	path.Set(fullName,NULL,NULL);		
	TPtrC pth = path.Path();
	TFileName pvtPath;
	fsSession.PrivatePath(pvtPath);
	//the below code is removed from getfullfile and explicitly added here so that creation of private path unnecessarily could be avoided while calling getfullfile()
	if (!(pth.CompareF(pvtPath)))
		{
		TEntry ent; 
		TFileName Pvt;
		//returns in the format Drive:
		TPtrC ptr = path.Drive();
		Pvt.Copy(ptr);
		Pvt.Append(pvtPath);	
		if (fsSession.Entry(Pvt , ent) == KErrNotFound) 
			{
	     	TInt ret = 0;
	     	TInt driveNumber;
			fsSession.CharToDrive((TUint)ptr[0], driveNumber);
			//creates the private path
			ret = fsSession.CreatePrivatePath(driveNumber);
      		if ((ret == KErrAlreadyExists) || (ret == KErrNone))
      			{
      			err = 0;
      			}
      		else
	      		{
	      		err = ret;
	      		}
			}
		}
	if (!err)
	    {
        mode = EFileWrite| EFileStream;
        err = file.Create(fsSession, fullName, (TUint)mode);
        if(err == KErrNone)
            {
            TUint att = KEntryAttHidden | KEntryAttSystem;
            err = file.SetAtt(att, 0);	
            if(err == KErrNone)
               {
               TPtr8 data((TUint8*)aData, aLen, aLen);
               err = file.Write(data);
               if(!err)
                   {
               	   err = file.Flush();
                   if(!err)
               	       {
               	       file.Close();
                       anErrno = err;
                       return err;
               	       }
                    }             
                }
            fsSession.Delete(fullName);
            file.Close();
			anErrno = EROFS;
            return -1;
            }
        else if (err == KErrPathNotFound)
            {
            // file exist???
            TParsePtr parser(fullName);
            fullName = parser.DriveAndPath();

            TParsePtr oneStepDownParser(fullName);
            
            TFindFile findFile(fsSession);
            if ( KErrNone == findFile.FindByDir
                            (oneStepDownParser.FullName(), 
                            oneStepDownParser.DriveAndPath()))
                {
                // file exist.
                err = ENOTDIR;
                }
            
            }
	    }
	    
	if (err == KErrNotReady)
		{
		anErrno = ENOENT;
		return -1;
		}	    
		
    return MapError(err, anErrno);
    }
    
// -----------------------------------------------------------------------------
// _ReadSysSplFile
// Read a system file.
// -----------------------------------------------------------------------------
//
EXPORT_C int _ReadSysSplFile(const wchar_t *aFullFileName,
                    char* aBuf, 
                    const int aLen,
                    int& anErrno,
                    RSessionBase& aSession)
    {
    TFullName fullName;
    RFILE file;
    RFs& fsSession = static_cast<RFs&>(aSession);
    mode_t mode;
	TInt err=GetFullFile(fullName,(const TText16*)aFullFileName,fsSession);
	if (!err)
	    {
        mode = EFileRead | EFileStream;
        err = file.Open(fsSession, fullName, (TUint)mode);
        if(err == KErrNone)
            {
            TPtr8 data((TUint8*)aBuf, aLen, aLen);
            err = file.Read(data);
            file.Close();
            }
        }
    if (err != KErrNone)
        {
        MapError(err, anErrno);
        }
    return err;
    }

//  End of File
