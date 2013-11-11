// LIBZCORE.H 
//
// Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
//
// Portions Copyright (c) 2007 1995-2005 Jean-loup Gailly and Mark Adler.
//

#ifndef LIBZCORE_H
#define LIBZCORE_H


#if (defined(__TOOLS2__) ||defined(__TOOLS__))
	// A tools build picks up the zconf.h file from the user include path
	#include "zlib.h"
#else	
	#include <e32def.h>
	// Any other build picks up zconf.h from stdapis system include directory
	#include <zlib.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif


//=========================================================================
// The application can compare zlibVersion and ZLIB_VERSION for consistency.
//=========================================================================
ZEXTERN const char * ZEXPORT zlibVersion_r OF((void));



//=========================================================================
// This function shall compresses as much as possible.
//=========================================================================
ZEXTERN int ZEXPORT deflate_r OF((z_streamp strm, int flush));



//=========================================================================
// This function shall deallocate all the dynamically allocated data 
// structures.
//=========================================================================
ZEXTERN int ZEXPORT deflateEnd_r OF((z_streamp strm));



//=========================================================================
// This function shall decompresses as much as possible.
//=========================================================================
ZEXTERN int ZEXPORT inflate_r OF((z_streamp strm, int flush));



//=========================================================================
// This function shall deallocate all the dynamically allocated data 
// structures.
//=========================================================================
ZEXTERN int ZEXPORT inflateEnd_r OF((z_streamp strm));





//=========================================================================
// This function shall Initializes the compression dictionary from the given 
// byte sequence without producing any compressed output.
//=========================================================================
ZEXTERN int ZEXPORT deflateSetDictionary_r OF((z_streamp strm,
                                             const Bytef *dictionary,
                                             uInt  dictLength));


//=========================================================================
// This function shall sets the destination stream as a complete copy of 
// the source stream
//=========================================================================
ZEXTERN int ZEXPORT deflateCopy_r OF((z_streamp dest,
                                    z_streamp source));


//=========================================================================
// This function does not free and reallocate all the internal compression
// state.
//=========================================================================
ZEXTERN int ZEXPORT deflateReset_r OF((z_streamp strm));



//=========================================================================
// This function shall update the compression level and compression strategy.
//=========================================================================
ZEXTERN int ZEXPORT deflateParams_r OF((z_streamp strm,
                                      int level,
                                      int strategy));
                                      
                                  
//=========================================================================
// This function shall fine tune deflate's internal compression parameters.
//=========================================================================
ZEXTERN int ZEXPORT deflateTune_r OF((z_streamp strm,
                                    int good_length,
                                    int max_lazy,
                                    int nice_length,
                                    int max_chain));    



//=========================================================================
// This function shall an upper bound on the compressed size after deflation
// of sourceLen bytes.
//=========================================================================
ZEXTERN uLong ZEXPORT deflateBound_r OF((z_streamp strm,
                                       uLong sourceLen));
                                      

//=========================================================================
// This function shall insert bits in the deflate output stream.
//=========================================================================
ZEXTERN int ZEXPORT deflatePrime_r OF((z_streamp strm,
                                     int bits,
                                     int value));

//=========================================================================
// This function shall provides gzip header information for when a gzip
// stream is requested by deflateInit2().
//=========================================================================
ZEXTERN int ZEXPORT deflateSetHeader_r OF((z_streamp strm,
                                         gz_headerp head));
                                                                             
                                     

                                     
//=========================================================================
// This function initializes the decompression dictionary from the given 
// uncompressed byte sequence.
//=========================================================================
ZEXTERN int ZEXPORT inflateSetDictionary_r OF((z_streamp strm,
                                             const Bytef *dictionary,
                                             uInt  dictLength));
                                             

//=========================================================================
// This function shall skips invalid coompressed data until a full flush 
// point can be found, or unil all the available input is skipped.
//=========================================================================
ZEXTERN int ZEXPORT inflateSync_r OF((z_streamp strm));


//=========================================================================
// This function shall set the destination stream as a complete copy of 
// the source stream.
//=========================================================================
ZEXTERN int ZEXPORT inflateCopy_r OF((z_streamp dest,
                                    z_streamp source));



//=========================================================================
// This function shall not free and reallocate all the internal decompression
// state.
//=========================================================================
ZEXTERN int ZEXPORT inflateReset_r OF((z_streamp strm));


//=========================================================================
// This function inserts bits in the inflate input stream.
//=========================================================================
ZEXTERN int ZEXPORT inflatePrime_r OF((z_streamp strm,
                                     int bits,
                                     int value));

//=========================================================================
// This function requests that gzip header information be stored in the
// provided gz_header structure.
//=========================================================================
ZEXTERN int ZEXPORT inflateGetHeader_r OF((z_streamp strm,
                                         gz_headerp head));

//=========================================================================
// This function does a raw inflate with a single call using a call-back
// interface for input and output.
//=========================================================================                                         
ZEXTERN int ZEXPORT inflateBack_r OF((z_streamp strm,
                                    in_func in, void FAR *in_desc,
                                    out_func out, void FAR *out_desc));
                                    
//=========================================================================
// This function frees all memory allocated by inflateBackInit().
//=========================================================================
ZEXTERN int ZEXPORT inflateBackEnd_r OF((z_streamp strm));


//=========================================================================
// This function returns flags indicating compile-time options.
//=========================================================================
ZEXTERN uLong ZEXPORT zlibCompileFlags_r OF((void));




//=========================================================================
// This function compressed the source buffer into the destination buffer.
//=========================================================================
ZEXTERN int ZEXPORT compress_r OF((Bytef *dest,   uLongf *destLen,
                                 const Bytef *source, uLong sourceLen));
                                 


//=========================================================================
// This function compresses the source buffer into the destination buffer.
//=========================================================================
ZEXTERN int ZEXPORT compress2_r OF((Bytef *dest,   uLongf *destLen,
                                  const Bytef *source, uLong sourceLen, int level));


//=========================================================================
// This function returns an upper bound on the compressed size after the 
// compress or compress2 on the sourceLen bytes.
//=========================================================================
ZEXTERN uLong ZEXPORT compressBound_r OF((uLong sourceLen));


//=========================================================================
// This function decompresses the source buffer into the destination buffer.
//=========================================================================
ZEXTERN int ZEXPORT uncompress_r OF((Bytef *dest,   uLongf *destLen,
                                   const Bytef *source, uLong sourceLen));


//=========================================================================
// Update a running Adler-32 checksum with the bytes buf[0..len-1] and 
// return the updated checksum.
//=========================================================================
ZEXTERN uLong ZEXPORT adler32_r OF((uLong adler, const Bytef *buf, uInt len));

//=========================================================================
// This function combines two Adler-32 checksums into one.
//=========================================================================
ZEXTERN uLong ZEXPORT adler32_combine_r OF((uLong adler1, uLong adler2,
                                          z_off_t len2));
                                                                                

//=========================================================================
// Update a running CRC-32 checksum with the bytes buf[0..len-1] and 
// return the updated CRC-32.
//=========================================================================
ZEXTERN uLong ZEXPORT crc32_r OF((uLong crc, const Bytef *buf, uInt len));


//=========================================================================
// This function combines two CRC-32 check values into one.
//=========================================================================
ZEXTERN uLong ZEXPORT crc32_combine_r OF((uLong crc1, uLong crc2, 
										z_off_t len2));
										


/* deflateInit and inflateInit are macros to allow checking the zlib version
 * and the compiler's view of z_stream:
 */
ZEXTERN int ZEXPORT deflateInit__r OF((z_streamp strm, int level,
                                     const char *version, int stream_size));
ZEXTERN int ZEXPORT inflateInit__r OF((z_streamp strm,
                                     const char *version, int stream_size));
ZEXTERN int ZEXPORT deflateInit2__r OF((z_streamp strm, int  level, int  method,
                                      int windowBits, int memLevel,
                                      int strategy, const char *version,
                                      int stream_size));
ZEXTERN int ZEXPORT inflateInit2__r OF((z_streamp strm, int  windowBits,
                                      const char *version, int stream_size));                                     
ZEXTERN int ZEXPORT inflateBackInit__r OF((z_streamp strm, int windowBits,
                                         unsigned char FAR *window,
                                         const char *version,
                                         int stream_size));
                                      

#define deflateInit_r(strm, level) \
        deflateInit__r((strm), (level),       ZLIB_VERSION, sizeof(z_stream))
#define inflateInit_r(strm) \
        inflateInit__r((strm),                ZLIB_VERSION, sizeof(z_stream))
#define deflateInit2_r(strm, level, method, windowBits, memLevel, strategy) \
        deflateInit2__r((strm),(level),(method),(windowBits),(memLevel),\
                      (strategy),           ZLIB_VERSION, sizeof(z_stream))
#define inflateInit2_r(strm, windowBits) \
        inflateInit2__r((strm), (windowBits), ZLIB_VERSION, sizeof(z_stream))
        
#define inflateBackInit_r(strm, windowBits, window) \
        inflateBackInit__r((strm), (windowBits), (window), \
        ZLIB_VERSION, sizeof(z_stream))        


ZEXTERN const char   * ZEXPORT zError_r           OF((int));
ZEXTERN int            ZEXPORT inflateSyncPoint_r OF((z_streamp z));
ZEXTERN const uLongf * ZEXPORT get_crc_table_r    OF((void));


#ifdef __cplusplus
}
#endif

#endif /* LIBZCORE_H */
