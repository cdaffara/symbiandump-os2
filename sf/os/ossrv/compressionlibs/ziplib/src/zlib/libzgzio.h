// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// GZIO.H
// 
//

// This header guard define is used subsequently in zutil.h to allow standard libarary functions
// (in stdlib.h and string.h) to be included for libz.dll and excluded for libzcore.dll. 
#ifndef LIBZGZIO_H
#define LIBZGZIO_H

#if (defined(__TOOLS2__) || defined(__TOOLS__))
	#include "zconf.h"
#else
	#include <zconf.h> 
#endif

#include <stdarg.h>
 

#ifdef SYMBIAN_EZLIB_DEVICE
	#include <e32def.h>
#endif

#ifdef __cplusplus
	extern "C" {
#endif

typedef voidp gzFile;

/* 
 * Flushes all pending output if necessary, closes the compressed file
 * and deallocates all the (de)compression state.
 */
int gzclose_r (gzFile file);


/*
 * Clears the error and end-of-file flags for file.This is analogous to the
 * clearerr() function in stdio.
 */
void gzclearerr_r (gzFile file);

 
/* 
 * gzdopen() associates a gzFile with the file descriptor fd.  File
 * descriptors are obtained from calls like open, dup, creat, pipe or
 * fileno (in the file has been previously opened with fopen).
 * The mode parameter is as in gzopen.
 */
gzFile gzdopen_r (int fd, const char *mode);


/*
 * Returns 1 if file is being read directly without decompression, otherwise
 * zero.
 */
int gzdirect_r (gzFile file);


/*
 * Returns 1 when EOF has previously been detected reading the given
 * input stream, otherwise zero.
 */ 
int gzeof_r (gzFile file);


/*
 * Returns the error message for the last error which occurred on the
 * given compressed file. errnum is set to zlib error number. If an
 * error occurred in the file system and not in the compression library,
 * errnum is set to Z_ERRNO and the application may consult errno
 * to get the exact error code.
 */
const char* gzerror_r (gzFile file, int *errnum);


/* 
 * Flushes all pending output into the compressed file.gzflush should 
 * be called only when strictly necessary because it can degrade compression.
 */
int gzflush_r (gzFile file, int flush);


/*
 * Reads one byte from the compressed file. gzgetc returns this byte
 * or -1 in case of end of file or error.
 */
int gzgetc_r (gzFile file);


/*
 * Reads bytes from the compressed file until len-1 characters are read, or
 * a newline character is read and transferred to buf, or an end-of-file
 * condition is encountered.  The string is then terminated with a null
 * character.
 */
char* gzgets_r (gzFile file, char *buf, int len);

/*
 * Opens a gzip (.gz) file for reading or writing. The mode parameter
 * is as in fopen ("rb" or "wb") but can also include a compression level
 * ("wb9") or a strategy: 'f' for filtered data as in "wb6f", 'h' for
 * Huffman only compression as in "wb1h", or 'R' for run-length encoding
 * as in "wb1R".
 */ 
gzFile gzopen_r (const char *path, const char *mode);


/*
 * Converts, formats, and writes the args to the compressed file under
 * control of the format string, as in fprintf. gzprintf returns the number of
 * uncompressed bytes actually written (0 in case of error).
 */
int gzprintf_r (gzFile file, const char *format, va_list va);


/*
 * Writes c, converted to an unsigned char, into the compressed file.
 * gzputc returns the value that was written, or -1 in case of error.
 */
int gzputc_r (gzFile file, int c);


/*
 * Writes the given null-terminated string to the compressed file, excluding
 * the terminating null character. gzputs returns the number of characters 
 * written, or -1 in case of error.
 */
int gzputs_r (gzFile file, const char *s);


/*
 * Reads the given number of uncompressed bytes from the compressed file.
 * gzread returns the number of uncompressed bytes actually read (0 for
 * end of file, -1 for error).
 */
int gzread_r (gzFile file, voidp buf, unsigned len);


/*
 * Rewinds the given file. This function is supported only for reading.
 * gzrewind(file) is equivalent to (int)gzseek(file, 0L, SEEK_SET)
 */
 
int gzrewind_r (gzFile file);


/*
 * Sets the starting position for the next gzread or gzwrite on the
 * given compressed file. If the file is opened for writing, only 
 * forward seeks are supported.
 */ 
z_off_t gzseek_r (gzFile file, z_off_t offset, int whence);


/*
 * Dynamically update the compression level or strategy. See the description
 * of deflateInit2 for the meaning of these parameters.
 */                                    
int gzsetparams_r (gzFile file, int level, int strategy);


/*
 * Returns the starting position for the next gzread or gzwrite on the
 * given compressed file. This position represents a number of bytes in the
 * uncompressed data stream.
 */ 
z_off_t gztell_r (gzFile file);


/*
 * Push one character back onto the stream to be read again later.
 * Only one character of push-back is allowed. 
 */ 
int gzungetc_r (int c, gzFile file);


/*
 * Writes the given number of uncompressed bytes into the compressed file.
 * gzwrite returns the number of uncompressed bytes actually written
 * (0 in case of error).
 */ 
int gzwrite_r (gzFile file, voidpc buf, unsigned len);

                                   
#ifdef __cplusplus
	}
#endif

#endif /* LIBZGZIO_H */






