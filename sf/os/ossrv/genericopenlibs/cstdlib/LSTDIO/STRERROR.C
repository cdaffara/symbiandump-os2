/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* *** CAUTION!!! KEEP DOC CONSISTENT---if you change text of a message
* ***            here, change two places:
* ***            1) the leading doc section (alphabetized by macro)
* ***            2) the real text inside switch(errnum)
* FUNCTION
* <<strerror>>---convert error number to string
* INDEX
* strerror
* ANSI_SYNOPSIS
* #include <string.h>
* char *strerror(int <[errnum]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char *strerror(<[errnum]>)
* int <[errnum]>;
* <<strerror>> converts the error number <[errnum]> into a
* string.  The value of <[errnum]> is usually a copy of <<errno>>.
* If <<errnum>> is not a known error number, the result points to an
* empty string.
* RETURNS
* This function returns a pointer to a string.  Your application must
* not modify that string.
* PORTABILITY
* ANSI C requires <<strerror>>, but does not specify the strings used
* for each error number.
* Although this implementation of <<strerror>> is reentrant, ANSI C
* declares that subsequent calls to <<strerror>> may overwrite the
* result string; therefore portable code cannot depend on the reentrancy
* of this subroutine.
* <<strerror>> requires no supporting OS subroutines.
* QUICKREF
* strerror ansi pure
* 
*
*/



#include <errno.h>
#include <string.h>

/**
Get pointer to error message string.
Returns a pointer to a string with the error message
corresponding to the errnum error number.
The returned pointer points to a statically allocated string. 
Further calls to this function will overwrite its content.
@param errnum Error number.
*/
EXPORT_C char *
strerror (int errnum)
{
  char *error;

  switch (errnum)
    {
    case EPERM:
      error = "Not owner";
      break;
    case ENOENT:
      error = "No such file or directory";
      break;
    case ESRCH:
      error = "No such process";
      break;
    case EINTR:
      error = "Interrupted system call";
      break;
    case EIO:
      error = "I/O error";
      break;
    case ENXIO:
      error = "No such device or address";
      break;
    case E2BIG:
      error = "Arg list too long";
      break;
    case ENOEXEC:
      error = "Exec format error";
      break;
    case EBADF:
      error = "Bad file number";
      break;
    case ECHILD:
      error = "No children";
      break;
    case EAGAIN:
      error = "No more processes";
      break;
    case ENOMEM:
      error = "Not enough space";
      break;
    case EACCES:
      error = "Permission denied";
      break;
    case EFAULT:
      error = "Bad address";
      break;
    case ENOTBLK:
      error = "Block device required";
      break;
    case EBUSY:
      error = "Device or resource busy";
      break;
    case EEXIST:
      error = "File exists";
      break;
    case EXDEV:
      error = "Cross-device link";
      break;
    case ENODEV:
      error = "No such device";
      break;
    case ENOTDIR:
      error = "Not a directory";
      break;
    case EISDIR:
      error = "Is a directory";
      break;
    case EINVAL:
      error = "Invalid argument";
      break;
    case ENFILE:
      error = "Too many open files in system";
      break;
    case EMFILE:
      error = "Too many open files";
      break;
    case ENOTTY:
      error = "Not a character device";
      break;
    case ETXTBSY:
      error = "Text file busy";
      break;
    case EFBIG:
      error = "File too large";
      break;
    case ENOSPC:
      error = "No space left on device";
      break;
    case ESPIPE:
      error = "Illegal seek";
      break;
    case EROFS:
      error = "Read-only file system";
      break;
    case EMLINK:
      error = "Too many links";
      break;
    case EPIPE:
      error = "Broken pipe";
      break;
    case EDOM:
      error = "Math argument";
      break;
    case ERANGE:
      error = "Result too large";
      break;
    case ENOMSG:
      error = "No message of desired type";
      break;
    case EIDRM:
      error = "Identifier removed";
      break;
    case EDEADLK:
      error = "Deadlock";
      break;
    case ENOLCK:
      error = "No lock";
      break;
    case ENOTSOCK:	error = "Not a socket";				break;
    case EADDRNOTAVAIL:	error = "Remote address not available";		break;
    case EAFNOSUPPORT:	error = "Address not supported by protocol";	break;
    case EISCONN:	error = "Socket already connected";		break;
    case ECONNREFUSED:	error = "Connection refused by remote host";	break;
    case EADDRINUSE:	error = "Address already in use";		break;
    case ETIMEDOUT:	error = "Connection timed out";			break;
    case ENOSTR:
      error = "Not a stream";
      break;
    case ETIME:
      error = "Stream ioctl timeout";
      break;
    case ENOSR:
      error = "No stream resources";
      break;
    case ENONET:
      error = "Machine is not on the network";
      break;
    case ENOPKG:
      error = "No package";
      break;
    case EREMOTE:
      error = "Resource is remote";
      break;
    case ENOLINK:
      error = "Virtual circuit is gone";
      break;
    case EADV:
      error = "Advertise error";
      break;
    case ESRMNT:
      error = "Srmount error";
      break;
    case ECOMM:
      error = "Communication error";
      break;
    case EPROTO:
      error = "Protocol error";
      break;
    case EMULTIHOP:
      error = "Multihop attempted";
      break;
    case EBADMSG:
      error = "Bad message";
      break;
    case ELIBACC:
      error = "Cannot access a needed shared library";
      break;
    case ELIBBAD:
      error = "Accessing a corrupted shared library";
      break;
    case ELIBSCN:
      error = ".lib section in a.out corrupted";
      break;
    case ELIBMAX:
      error = "Attempting to link in more shared libraries than system limit";
      break;
    case ELIBEXEC:
      error = "Cannot exec a shared library directly";
      break;
    case ENOSYS:
      error = "Function not implemented";
      break;
    case ENMFILE:
      error = "No more files";
      break;
    case ENOTEMPTY:
      error = "Directory not empty";
      break;
    case ENAMETOOLONG:
      error = "File or path name too long";
      break;
	case EILSEQ:
	  error = "invalid wide-character encoding";
      break;
	default:
	error = "";
      break;
    }

  return error;
}
