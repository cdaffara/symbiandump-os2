/* Portions Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */

/* compress.cpp -- compress a memory buffer
 * Copyright (C) 1995-2003 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#define ZLIB_INTERNAL
#include "libzcore.h"

/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/

#ifdef __SYMBIAN32__
EXPORT_C int  compress2_r (Bytef * dest, uLongf * destLen,const Bytef *  source,uLong  sourceLen,int level)
#else
int ZEXPORT compress2 (dest, destLen, source, sourceLen, level)
    Bytef *dest;
    uLongf *destLen;
    const Bytef *source;
    uLong sourceLen;
    int level;
#endif //__SYMBIAN32__
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit_r(&stream, level);
    if (err != Z_OK) return err;

    err = deflate_r(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd_r(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = deflateEnd_r(&stream);
    return err;
}

/* ===========================================================================
 */

#ifdef __SYMBIAN32__
EXPORT_C  int compress_r (Bytef * dest,    uLongf *  destLen,   const Bytef * source,  uLong  sourceLen)
#else
int ZEXPORT compress (dest, destLen, source, sourceLen)
    Bytef *dest;
    uLongf *destLen;
    const Bytef *source;
    uLong sourceLen;
#endif //__SYMBIAN32__
{
    return compress2_r(dest, destLen, source, sourceLen, Z_DEFAULT_COMPRESSION);
}


/* ===========================================================================
     If the default memLevel or windowBits for deflateInit() is changed, then
   this function needs to be updated.
 */
 

#ifdef __SYMBIAN32__
EXPORT_C  uLong  compressBound_r (uLong sourceLen)
#else  
uLong ZEXPORT compressBound (sourceLen)
    uLong sourceLen;
#endif //__SYMBIAN32__
{
    return sourceLen + (sourceLen >> 12) + (sourceLen >> 14) + 11;
}



