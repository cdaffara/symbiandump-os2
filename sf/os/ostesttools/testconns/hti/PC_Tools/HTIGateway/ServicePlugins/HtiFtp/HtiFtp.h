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
*/
// HTI service functions
//gsoap ns1 service name: HtiFtp
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char * xsd__string;
typedef wchar_t * xsd__string_;
typedef bool xsd__boolean;
typedef unsigned char xsd__unsignedByte;
typedef int xsd__int;
typedef unsigned int xsd__unsignedInt;
typedef ULONG64	xsd__unsignedLong;

enum ns1__driveMediaType { NotPresent, UnknownType, Floppy, HardDisk, CDROM,
                           RAM, Flash, ROM, Remote, NANDFlash, RotatingMedia };

struct ns1__HtiSoapAttachment
{
	//@xsd__string href;
	xsd__string href;
	xsd__string mimeType;
};

struct ArrayOfStrings
{
	xsd__string_* __ptr;
	int __size;
};

struct ArrayOfBytes
{
	xsd__unsignedByte* __ptr;
	int __size;
};

struct ns1__HtiFileInfo
{
	xsd__string_     fileName;
    xsd__unsignedInt fileSize;
};

struct ArrayOfHtiFileInfos
{
	struct ns1__HtiFileInfo* __ptr;
	int __size;
};

struct ns1__HtiDriveInfo
{
    xsd__string_ rootPath;
    enum ns1__driveMediaType mediaType;
    xsd__unsignedInt uniqueID;
    xsd__unsignedLong driveSize;
    xsd__unsignedLong freeSpace;
    xsd__string_ driveName;
};

struct ArrayOfHtiDriveInfos
{
	struct ns1__HtiDriveInfo* __ptr;
	int __size;
};

//gsoap ns1 service method-action: putFile "HtiFtp"
int ns1__putFile(
	struct ns1__HtiSoapAttachment *file,
    xsd__string_ targetPath,
	struct ns1__putFileResponse{} *out
);
struct ns1__getFileResponse{struct ns1__HtiSoapAttachment _return;};
//gsoap ns1 service method-action: getFile "HtiFtp"
int ns1__getFile(
	xsd__string_ filePath,
	struct ns1__getFileResponse &r
);
//gsoap ns1 service method-action: cancelFileTransfer "HtiFtp"
int ns1__cancelFileTransfer(
	void*_ ,
	struct ns1__cancelFileTransferResponse{} *out
);
//gsoap ns1 service method-action: listFiles "HtiFtp"
int ns1__listFiles(
	xsd__string_ targetDir,
	struct ArrayOfStrings &dirs
);
//gsoap ns1 service method-action: listFilesSizes "HtiFtp"
int ns1__listFilesSizes(
	xsd__string_ targetDir,
	struct ArrayOfHtiFileInfos &fileInfos
);
//gsoap ns1 service method-action: listDirs "HtiFtp"
int ns1__listDirs(
	xsd__string_ targetDir,
	struct ArrayOfStrings &files
);
//gsoap ns1 service method-action: createDir "HtiFtp"
int ns1__createDir(
	xsd__string_ targetDir,
	struct ns1__createDirResponse{} *out
);
//gsoap ns1 service method-action: deleteDir "HtiFtp"
int ns1__deleteDir(
	xsd__string_ targetDir,
	struct ns1__deleteDirResponse{} *out
);
//gsoap ns1 service method-action: deleteFile "HtiFtp"
int ns1__deleteFile(
	xsd__string_ targetFile,
	struct ns1__deleteFileResponse{} *out
);
//gsoap ns1 service method-action: setForcedOperations "HtiFtp"
int ns1__setForcedOperations(
	xsd__boolean state,
	struct ns1__setForcedOperationsResponse{} *out
);
//gsoap ns1 service method-action: fileChecksum "HtiFtp"
int ns1__fileChecksum(
	xsd__unsignedByte algorithmId,
	xsd__string_ targetFile,
	struct ArrayOfBytes &checksumByteArray
);
//gsoap ns1 service method-action: format "HtiFtp"
int ns1__format(
	xsd__unsignedByte drive,
	xsd__unsignedByte formatMode,
  	struct ns1__formatResponse{} *out
);
//gsoap ns1 service method-action: listDrives "HtiFtp"
int ns1__listDrives(
	void*_ ,
	struct ArrayOfHtiDriveInfos &driveInfos
);
