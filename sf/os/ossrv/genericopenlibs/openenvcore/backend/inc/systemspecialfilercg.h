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
* Description:  Helper class to deal with special system files.
*
*/




#ifndef SYSTEM_SPECIAL_FILE_RCG_H
#define SYSTEM_SPECIAL_FILE_RCG_H

// INCLUDES
#include <sys/cdefs.h>
#include <sys/types.h>
#include <wchar.h>
#include <e32cmn.h>

#define KMaxEncDecBuf 9

// DATA TYPES

enum TSpecialFileType
    {
    EFileGeneralError = -1,
    EFileNotFound,
    EFileTypeGeneral, // not a system specific special file
    EFileTypeMkFifo, // temp file of mkfifo type
    EFileTypeSymLink, // symbolic link
    EFileTypeSocket,
    EFileTypeMax // no enums beyond this.
    };

struct SSpecialFileMagicHeader {
   char iMagicBuffer[KMaxEncDecBuf];
   unsigned char iFileType;
};


/*
 * Encode the system special file magic header.
 * @param aEncBuf Encoded buffer
 * @param aFileType Type of file
 * @return 0 for success, else -1
 */
int _EncodeSystemSplFileMagicHeader(
                    struct SSpecialFileMagicHeader *aEncBuf, 
                    TSpecialFileType aFileType);
                    
/*
 * Try to retrieve file type from buffer.
 * @param buf pointer to the contained buffer.
 * @return File type. 
 *      EFileTypeGeneral For normal file
 *      EFileTypeMkFifo  For mkfifo
 *      EFileTypeSymLink For Symbolic link.
 */
TSpecialFileType _SystemSpecialFileBasedBuffer(const char *buf);

/*
 * Try to retrieve file type from file name (char*).
 * @param aFullFileName pointer to the file name.
 * @param anErrno Ref to the error no.
 * @param aSession File server session.
 * @return File type. 
 *      EFileTypeGeneral For normal file
 *      EFileTypeMkFifo  For mkfifo
 *      EFileTypeSymLink For Symbolic link.
 */
/*
TSpecialFileType _SystemSpecialFileBasedFilePath 
        (const char* aFullFileName, TInt& aErrno, RSessionBase& aSession);*/

/*
 * Try to retrieve file type from file name (wchar_t*).
 * @param aPathName pointer to the file name.
 * @param anErrno Ref to the error no. 
 * @param aSession File server session.
 * @return File type. 
 *      EFileTypeGeneral For normal file
 *      EFileTypeMkFifo  For mkfifo
 *      EFileTypeSymLink For Symbolic link.
 */
IMPORT_C TSpecialFileType _SystemSpecialFileBasedFilePath
        (const wchar_t* aPathName, TInt& aErrno, RSessionBase& aSession);

/*
 * Create a system file.
 * @param aFullFileName pointer to the file name.
 * @param aData Pointer to the data to be writen within the new file
 * @param aLen Length of the data.
 * @param anErrno Ref to the error no.
 * @param aSession File server session.
 * @return 0 for success, else -1
 */
int _CreateSysSplFile(const wchar_t *aFullFileName, 
                    const char* aData, 
                    const int aLen,
                    int& anErrno,
                    RSessionBase& aSession);

/*
 * delete a system file.
 * @param aFullFileName pointer to the file name.
 * @param aSession File server session.
 * @return 0 for success, else -1
 */
/*
TInt _DeleteSystemSpecialFileBasedFilePath
        (const char* aFullFileName, RSessionBase& aSession);*/

/*
 * delete a system file. (Wide char)
 * @param aFullFileName pointer to the file name.
 * @param aSession File server session.
 * @return 0 for success, else -1
 */
TInt _DeleteSystemSpecialFileBasedFilePath
        (const wchar_t* aFullFileName, RSessionBase& aSession);

/*
 * Read a system file.
 * @param aFullFileName pointer to the file name.
 * @param aBuf Pointer to the buffer for read data
 * @param aLen Length of the data.
 * @param anErrno Ref to the error no.
 * @param aSession File server session.
 * @return number of bytes read on success, else -1
 */
/*
IMPORT_C int _ReadSysSplFileL(const char *aFullFileName,
                    char* aBuf, 
                    const int aLen,
                    int& anErrno,
                    RSessionBase& aSession);*/

/*
 * Read a system file (wide char variant).
 * @param aFullFileName pointer to the file name.
 * @param aBuf Pointer to the buffer for read data
 * @param aLen Length of the data.
 * @param anErrno Ref to the error no.
 * @param aSession File server session.
 * @return number of bytes read on success, else -1
 */
IMPORT_C int _ReadSysSplFile(const wchar_t *aFullFileName,
                    char* aBuf, 
                    const int aLen,
                    int& anErrno,
                    RSessionBase& aSession);



#endif      // SYSTEM_SPECIAL_FILE_RCG_H

// End of File

