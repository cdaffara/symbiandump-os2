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
// This file contains the C interface for all the APIs provided by the 
// compression Library on Symbian. The actual functionlaity of all the 
// API's are divided over two different libraries, libzcore.dll and libz.dll
// All the API's are exported from this library and will be the only interface 
// for the user of compression library
// 
//

#ifndef SYMBIAN_EZLIB_EXCLUDE_GZ_FUNCTIONS
	#include "libzgzio.h"
#endif

#include "libzcore.h"


//================================================================= 

EXPORT_C uLong  adler32(uLong adler,const Bytef *  buf,uInt len)
	{
	return(adler32_r(adler, buf, len));
	}

//================================================================= 

EXPORT_C  int compress (Bytef * dest, uLongf *  destLen,   
										 const Bytef * source, uLong sourceLen)
	{
	return (compress_r(dest, destLen, source, sourceLen));
	}

//================================================================= 	

EXPORT_C int  compress2 (Bytef * dest, uLongf * destLen,
							 const Bytef *  source,uLong  sourceLen, int level)
	{
	return (compress2_r(dest, destLen, source, sourceLen, level));
	}
	
//================================================================= 	
EXPORT_C  uLong  compressBound (uLong sourceLen)
	{
	return(compressBound_r(sourceLen));
	}
	
//================================================================= 	
EXPORT_C unsigned long  crc32( unsigned long crc,  
								 const unsigned char FAR *  buf, unsigned len)
	{
	return (crc32_r(crc, buf, len));
	}
	
//================================================================= 	
EXPORT_C int   deflate (z_streamp strm, int  flush)
	{
	return(deflate_r(strm, flush));
	}
	
//================================================================= 	
EXPORT_C  uLong deflateBound(z_streamp strm, uLong  sourceLen)
	{
	return (deflateBound_r(strm, sourceLen));
	}
	
//================================================================= 	
EXPORT_C int deflateCopy (z_streamp dest, z_streamp source)
	{
	return (deflateCopy_r(dest, source));
	}
	
//================================================================= 	
EXPORT_C int   deflateEnd (z_streamp strm)
	{
	return (deflateEnd_r(strm));
	}
	
//================================================================= 	
EXPORT_C int  deflateInit2_(z_streamp strm, int level, int method,
									int windowBits, int memLevel, int strategy,
										 const char * version, int stream_size)
	{
	return (deflateInit2__r(strm, level, method, windowBits, memLevel, strategy, version, stream_size));
	}
	
//================================================================= 	
EXPORT_C int  deflateInit_( z_streamp strm, int  level,
										const char * version, int  stream_size)
	{
	return(deflateInit__r(strm, level, version,stream_size));
	}
	
//================================================================= 	
EXPORT_C int deflateParams(z_streamp strm, int  level, int  strategy)
	{
	return (deflateParams_r(strm, level, strategy));
	}
	
//================================================================= 	
EXPORT_C int  deflateReset (z_streamp strm)
	{
	return (deflateReset_r(strm));
	}
	
//================================================================= 	
EXPORT_C int  deflateSetDictionary (z_streamp strm, const Bytef *  dictionary,uInt   dictLength)
	{
	return (deflateSetDictionary_r(strm, dictionary, dictLength ));
	}
	
	
//================================================================= 	
EXPORT_C  const unsigned long FAR * get_crc_table()
	{
	return (get_crc_table_r());
	}
#ifndef SYMBIAN_EZLIB_EXCLUDE_GZ_FUNCTIONS	
//=================================================================	
EXPORT_C int gzclose (gzFile file)
	{
	return (gzclose_r(file));
	}
	
//=================================================================
EXPORT_C void gzclearerr (gzFile file)
	{
	gzclearerr_r(file);
	return;
	}
	
//================================================================= 
EXPORT_C gzFile gzdopen (int fd, const char *  mode)
	{
	return (gzdopen_r(fd, mode));
	}
	
//=================================================================
EXPORT_C int gzdirect (gzFile file)
	{
	return ( gzdirect_r(file) );
	}
	
//================================================================= 	
EXPORT_C  int gzeof (gzFile file)
	{
	return (gzeof_r(file));
	}
	
//================================================================= 	
EXPORT_C const char *  gzerror (gzFile file, int *  errnum)
	{
	return (gzerror_r(file, errnum));
	}
	
//================================================================= 	
EXPORT_C  int gzflush (gzFile file,int flush)
	{
	return ( gzflush_r(file, flush) );
	}
	
//================================================================= 
EXPORT_C int gzgetc(gzFile file)
	{
	return ( gzgetc_r(file) ); 
	}
	
//================================================================= 	
EXPORT_C char * gzgets(gzFile file, char * buf, int len)
	{
	return ( gzgets_r(file, buf, len) );
	}

//================================================================= 	
EXPORT_C gzFile gzopen (const char * path, const char * mode)	
	{
	return ( gzopen_r (path, mode) );
	}
	

//================================================================= 
EXPORT_C int gzprintf (gzFile file, const char *format, /*args*/ ...)
	{
	va_list va;
	va_start(va, format);
	return (gzprintf_r(file, format , va));
	}


//================================================================= 
EXPORT_C int  gzputc(gzFile file, int c)
	{
	return ( gzputc_r (file, c) );
	}
	
//================================================================= 
EXPORT_C int  gzputs(gzFile file, const char * s)
	{
	return (gzputs_r(file, s) );
	}

//================================================================= 
EXPORT_C int gzread (gzFile file, voidp buf,unsigned len)
	{
	return ( gzread_r(file, buf, len) );
	}

//================================================================= 
EXPORT_C int  gzrewind (gzFile file)
	{
	return ( gzrewind_r(file) );
	}

//================================================================= 
EXPORT_C  z_off_t gzseek (gzFile file, z_off_t  offset, int whence)
	{
	return (gzseek_r (file, offset, whence) );
	}

//================================================================= 
EXPORT_C   int gzsetparams (gzFile file, int  level, int  strategy)
	{
	return ( gzsetparams_r (file, level, strategy ) );
	}

//================================================================= 
EXPORT_C z_off_t gztell (gzFile file)
	{
	return ( gztell_r(file) );
	}

//================================================================= 
EXPORT_C int gzungetc (int c, gzFile file)
	{
	return (gzungetc_r (c, file) );
	}

//================================================================= 
EXPORT_C  int gzwrite (gzFile file,voidpc  buf, unsigned len)
	{
	return ( gzwrite_r (file , buf, len) );
	}
#endif //end SYMBIAN_EZLIB_EXCLUDE_GZ_FUNCTIONS

//================================================================= 
EXPORT_C  int inflate(z_streamp strm,int  flush)
	{
	return ( inflate_r(strm, flush) );
	}

//================================================================= 
EXPORT_C  int inflateEnd(z_streamp strm)
	{
	return ( inflateEnd_r(strm) );
	}

//================================================================= 
EXPORT_C int inflateInit2_(z_streamp strm, int windowBits,
										const char * version, int  stream_size)
	{
	return (inflateInit2__r(strm, windowBits, version, stream_size ));
	}
	
//================================================================= 
EXPORT_C  int inflateInit_(z_streamp strm, const char * version, 
															int stream_size)
	{
	return ( inflateInit__r(strm, version, stream_size) );
	}
	
//================================================================= 
EXPORT_C  int inflateReset(z_streamp strm)
	{
	return ( inflateReset_r(strm) );
	}

//=================================================================
EXPORT_C int  inflateSetDictionary(z_streamp strm,
									const Bytef *  dictionary, uInt dictLength)
	{
	return(inflateSetDictionary_r(strm, dictionary, dictLength));
	}
	
//================================================================= 
EXPORT_C int  inflateSync(z_streamp strm)
	{
	return ( inflateSync_r(strm) );
	}

//================================================================= 
EXPORT_C  int inflateSyncPoint(z_streamp strm)
	{
	return ( inflateSyncPoint_r(strm) );
	}

//=================================================================
EXPORT_C int  uncompress (Bytef * dest,uLongf *  destLen,  
								const Bytef * source, uLong  sourceLen)
	{
	return ( uncompress_r(dest, destLen, source, sourceLen) );
	}

//=================================================================
EXPORT_C const char *  zlibVersion()
	{
	return ( zlibVersion_r() );
	}

//================================================================= 
EXPORT_C const char *  zError(int err)
	{
	return ( zError_r(err) );
	}
	
//================================================================= 
EXPORT_C uLong adler32_combine(uLong adler1, uLong adler2, z_off_t len2)
	{
	return ( adler32_combine_r (adler1, adler2, len2) );
	}

//=================================================================
EXPORT_C uLong crc32_combine (uLong crc1, uLong crc2, z_off_t len2)
	{
	return ( crc32_combine_r (crc1, crc2, len2) );
	}

//=================================================================
EXPORT_C uLong zlibCompileFlags ()
	{
	return ( zlibCompileFlags_r () );
	}

//=================================================================
EXPORT_C int deflateSetHeader (z_streamp strm, gz_headerp head)
	{
	return ( deflateSetHeader_r (strm, head) );
	}

//=================================================================
EXPORT_C int deflatePrime (z_streamp strm, int bits, int value)
	{
	return ( deflatePrime_r (strm, bits, value) );
	}

//=================================================================
EXPORT_C int deflateTune (z_streamp strm, int good_length, int max_lazy, int nice_length, int max_chain)
	{
	return ( deflateTune_r (strm, good_length, max_lazy, nice_length, max_chain) );
	}

//=================================================================
EXPORT_C int inflateCopy (z_streamp dest, z_streamp source)
	{
	return ( inflateCopy_r (dest, source) );
	}

//=================================================================
EXPORT_C int inflatePrime (z_streamp strm, int bits, int value)
	{
	return ( inflatePrime_r (strm, bits, value) );
	}

//=================================================================
EXPORT_C int inflateGetHeader (z_streamp strm, gz_headerp head)
	{
	return ( inflateGetHeader_r (strm, head) );
	}

//=================================================================
EXPORT_C int inflateBackInit_ (z_streamp strm, int windowBits, unsigned char FAR *window, 
											const char *version,
											int stream_size)
	{
	return ( inflateBackInit__r (strm, windowBits, window,  version, stream_size) );
	}

//=================================================================
EXPORT_C int inflateBack (z_streamp strm, in_func in, void FAR *in_desc,
        									out_func out, void FAR *out_desc)
	{
	return ( inflateBack_r (strm, in, in_desc,out, out_desc) );
	}


//=================================================================
EXPORT_C int inflateBackEnd (z_streamp strm)
	{
	return	( inflateBackEnd_r (strm) );
	}
