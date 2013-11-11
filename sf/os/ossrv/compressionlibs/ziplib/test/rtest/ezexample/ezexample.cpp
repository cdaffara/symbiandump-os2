/* Portions Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */

/* ezexample.c -- usage example of the zlib compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* @(#) $Id$ */

#include <stdio.h>
#include <ezlib.h>
#include <e32test.h>

static RTest test(_L("ezexample.exe"));

_LIT(KTestTitle, "Open Source Library Defect Tests.");

#ifdef STDC
#  include <string.h>
#  include <stdlib.h>
#else
   extern void exit  OF((int));
#endif

#if defined(VMS) || defined(RISCOS)
#  define TESTFILE "foo-gz"
#else
#  define TESTFILE "foo.gz"
#endif

/* Test macro and function */
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
    if (aValue != aExpected)
    	{
        test.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        test.operator()(EFalse, aLine);
        }
    }
#define test2(a, b) Check(a, b, __LINE__)

const char hello[] = "hello, hello!";
/* "hello world" would be more standard, but the repeated "hello"
 * stresses the compression code better, sorry...
 */

const char dictionary[] = "hello";
uLong dictId; /* Adler32 value of the dictionary */

void test_compress      OF((Byte *compr, uLong comprLen,
		            Byte *uncompr, uLong uncomprLen)); 
void test_deflate       OF((Byte *compr, uLong comprLen));
void test_inflate       OF((Byte *compr, uLong comprLen,
		            Byte *uncompr, uLong uncomprLen));
void test_large_deflate OF((Byte *compr, uLong comprLen,
		            Byte *uncompr, uLong uncomprLen));
void test_large_inflate OF((Byte *compr, uLong comprLen,
		            Byte *uncompr, uLong uncomprLen));
void test_flush         OF((Byte *compr, uLong *comprLen));
void test_sync          OF((Byte *compr, uLong comprLen,
		            Byte *uncompr, uLong uncomprLen));
void test_dict_deflate  OF((Byte *compr, uLong comprLen));
void test_dict_inflate  OF((Byte *compr, uLong comprLen,
		            Byte *uncompr, uLong uncomprLen));
void PrintString(TRefByValue<const TDesC> aFmt, char* string);
void RunTest();

/**
Test compress() and uncompress()

@SYMTestCaseID          SYSLIB-EZLIB-CT-0820-0001
@SYMTestCaseDesc	    Compression and decompression of a buffer test.
@SYMTestPriority 	    High
@SYMTestActions  	    Compress and uncompress string "hello" and check for integrity of the operation done.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void test_compress(
    Byte *compr, uLong comprLen, Byte *uncompr,
     uLong uncomprLen)
{
	test.Next(_L("test_compress"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0820-0001 "));
    int err;
    uLong len = strlen(hello)+1;

    err = compress(compr, &comprLen, (const Bytef*)hello, len);
    test2(err, Z_OK);

    strcpy((char*)uncompr, "garbage");

    err = uncompress(uncompr, &uncomprLen, compr, comprLen);
    test2(err, Z_OK);

    test2(strcmp((char*)uncompr, hello), 0); /* Fails if bad uncompress */

    PrintString(_L("uncompress(): %S\n"), (char*)uncompr);
}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0821-0001
@SYMTestCaseDesc	    Read and write of .gz files test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests deflate() with small buffers
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void test_deflate(
    Byte *compr,
    uLong comprLen)
{
	test.Next(_L("test_deflate"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0821-0001 "));
    z_stream c_stream; /* compression stream */
    int err;
    int len = strlen(hello)+1;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    test2(err, Z_OK);

    c_stream.next_in  = (Bytef*)hello;
    c_stream.next_out = compr;

    while (c_stream.total_in != (uLong)len && c_stream.total_out < comprLen) {
        c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
        err = deflate(&c_stream, Z_NO_FLUSH);
        test2(err, Z_OK);
    }
    /* Finish the stream, still forcing small buffers: */
    for (;;) {
        c_stream.avail_out = 1;
        err = deflate(&c_stream, Z_FINISH);
        if (err == Z_STREAM_END) break;
        test2(err, Z_OK);
    }

    err = deflateEnd(&c_stream);
    test2(err, Z_OK);
}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0822-0001
@SYMTestCaseDesc	    Read and write of .gz files test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests inflate() with small buffers
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void test_inflate(
    Byte *compr, uLong comprLen, Byte *uncompr,
     uLong uncomprLen)
{
	test.Next(_L("test_inflate"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0822-0001 "));
    int err;
    z_stream d_stream; /* decompression stream */

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = 0;
    d_stream.next_out = uncompr;

    err = inflateInit(&d_stream);
    test2(err, Z_OK);

    while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
        test2(err, Z_OK);
    }

    err = inflateEnd(&d_stream);
    test2(err, Z_OK);

    test2(strcmp((char*)uncompr, hello), 0); /* Fails if bad inflate */

    PrintString(_L("inflate(): %S\n"), (char*)uncompr);   
}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0823-0001
@SYMTestCaseDesc	    Deflate with large buffers test
@SYMTestPriority 	    High
@SYMTestActions  	    Test deflate() with large buffers and dynamic change of compression level
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void test_large_deflate(
    Byte *compr, uLong comprLen, Byte *uncompr,
     uLong uncomprLen)
{
	test.Next(_L("test_large_deflate"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0823-0001	"));
    z_stream c_stream; /* compression stream */
    int err;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = deflateInit(&c_stream, Z_BEST_SPEED);
    test2(err, Z_OK);

    c_stream.next_out = compr;
    c_stream.avail_out = (uInt)comprLen;

    /* At this point, uncompr is still mostly zeroes, so it should compress
     * very well:
     */
    c_stream.next_in = uncompr;
    c_stream.avail_in = (uInt)uncomprLen;
    err = deflate(&c_stream, Z_NO_FLUSH);
    test2(err, Z_OK);
    
    test2(c_stream.avail_in, 0); /* Fails if deflate not greedy */

    /* Feed in already compressed data and switch to no compression: */
    deflateParams(&c_stream, Z_NO_COMPRESSION, Z_DEFAULT_STRATEGY);
    c_stream.next_in = compr;
    c_stream.avail_in = (uInt)comprLen/2;
    err = deflate(&c_stream, Z_NO_FLUSH);
    test2(err, Z_OK);

    /* Switch back to compressing mode: */
    deflateParams(&c_stream, Z_BEST_COMPRESSION, Z_FILTERED);
    c_stream.next_in = uncompr;
    c_stream.avail_in = (uInt)uncomprLen;
    err = deflate(&c_stream, Z_NO_FLUSH);
    test2(err, Z_OK);

    err = deflate(&c_stream, Z_FINISH);
    test2(err, Z_STREAM_END);

    err = deflateEnd(&c_stream);
    test2(err, Z_OK);
}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0824-0001
@SYMTestCaseDesc	    Inflate with large buffers test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests inflate() with large buffers
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/

void test_large_inflate(
    Byte *compr, uLong comprLen, Byte *uncompr,
     uLong uncomprLen)
{
    test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0824-0001 test_large_inflate "));
    int err;
    z_stream d_stream; /* decompression stream */

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = (uInt)comprLen;

    err = inflateInit(&d_stream);
    test2(err, Z_OK);

    for (;;) {
        d_stream.next_out = uncompr;            /* discard the output */
	d_stream.avail_out = (uInt)uncomprLen;
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
        test2(err, Z_OK);
    }

    err = inflateEnd(&d_stream);
    test2(err, Z_OK);

    test2(d_stream.total_out, (2*uncomprLen + comprLen/2)); /* Fails if bad large inflate */

    test.Printf(_L("large_inflate(): OK\n"));
}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0825-0001
@SYMTestCaseDesc	    Test deflate() with full flush
@SYMTestPriority 	    High
@SYMTestActions  	    Test by flushing the output as random access is desired
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void test_flush(
    Byte *compr,
    uLong *comprLen)
{
	test.Next(_L("test_flush"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0825-0001 "));
    z_stream c_stream; /* compression stream */
    int err;
    int len = strlen(hello)+1;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
    test2(err, Z_OK);

    c_stream.next_in  = (Bytef*)hello;
    c_stream.next_out = compr;
    c_stream.avail_in = 3;
    c_stream.avail_out = (uInt)*comprLen;
    err = deflate(&c_stream, Z_FULL_FLUSH);
    test2(err, Z_OK);

    compr[3]++; /* force an error in first compressed block */
    c_stream.avail_in = len - 3;

    err = deflate(&c_stream, Z_FINISH);
    if (err != Z_STREAM_END) {
    	test2(err, Z_OK);
    }
    err = deflateEnd(&c_stream);
    test2(err, Z_OK);

    *comprLen = c_stream.total_out;
}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0826-0001
@SYMTestCaseDesc	    Test the inflateSync() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for reading of compressed data with damaged parts and check for errors.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void test_sync(
    Byte *compr, uLong comprLen, Byte *uncompr,
     uLong uncomprLen)
{
	test.Next(_L("test_sync"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0826-0001 "));
    int err;
    z_stream d_stream; /* decompression stream */

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = 2; /* just read the zlib header */

    err = inflateInit(&d_stream);
    test2(err, Z_OK);

    d_stream.next_out = uncompr;
    d_stream.avail_out = (uInt)uncomprLen;

    inflate(&d_stream, Z_NO_FLUSH);
    test2(err, Z_OK);

    d_stream.avail_in = (uInt)comprLen-2;   /* read all compressed data */
    err = inflateSync(&d_stream);           /* but skip the damaged part */
    test2(err, Z_OK);

    err = inflate(&d_stream, Z_FINISH);
    test2(err, Z_DATA_ERROR); /* inflate should report DATA_ERROR because of incorrect adler32 */

    err = inflateEnd(&d_stream);
    test2(err, Z_OK);

    PrintString(_L("after inflateSync(): hel%S\n"), (char*)uncompr);
}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0827-0001
@SYMTestCaseDesc	    Deflation functionality test
@SYMTestPriority 	    High
@SYMTestActions  	    Test deflate() function with preset dictionary
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void test_dict_deflate(
    Byte *compr,
    uLong comprLen)
{
	test.Next(_L("test_dict_deflate"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0827-0001 "));
    z_stream c_stream; /* compression stream */
    int err;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = deflateInit(&c_stream, Z_BEST_COMPRESSION);
    test2(err, Z_OK);

    err = deflateSetDictionary(&c_stream,
			       (const Bytef*)dictionary, sizeof(dictionary));
    test2(err, Z_OK);

    dictId = c_stream.adler;
    c_stream.next_out = compr;
    c_stream.avail_out = (uInt)comprLen;

    c_stream.next_in = (Bytef*)hello;
    c_stream.avail_in = (uInt)strlen(hello)+1;

    err = deflate(&c_stream, Z_FINISH);
    test2(err, Z_STREAM_END);

    err = deflateEnd(&c_stream);
    test2(err, Z_OK);
}

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0828-0001
@SYMTestCaseDesc	    Inflation functionality test
@SYMTestPriority 	    High
@SYMTestActions  	    Test inflate() with a preset dictionary
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void test_dict_inflate(
    Byte *compr, uLong comprLen, Byte *uncompr,
     uLong uncomprLen)
{
	test.Next(_L("test_dict_inflate"));
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-0828-0001 "));
    int err;
    z_stream d_stream; /* decompression stream */

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = (uInt)comprLen;

    err = inflateInit(&d_stream);
    test2(err, Z_OK);

    d_stream.next_out = uncompr;
    d_stream.avail_out = (uInt)uncomprLen;

    for (;;) {
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
		if (err == Z_NEED_DICT) {
			test2(d_stream.adler, dictId); /* Fails if unexpected dictionary */

		    err = inflateSetDictionary(&d_stream, (const Bytef*)dictionary,
					       sizeof(dictionary));
		}
		test2(err, Z_OK);
    }

    err = inflateEnd(&d_stream);
    test2(err, Z_OK);

    test2(strcmp((char*)uncompr, hello), 0); /* Fails if bad inflate with dictionary */

    PrintString(_L("inflate with dictionary: %S\n"), (char*)uncompr);
}

/* ===========================================================================
 * Usage:  example [output.gz  [input.gz]]
 */

void PrintString(TRefByValue<const TDesC> aFmt, char* string)
	{
	TPtrC8 ptrc8;
	TBuf<100> buf;
	ptrc8.Set((TUint8*)string, strlen(string));
	buf.Copy(ptrc8);
	test.Printf(aFmt, &buf);
	}

void RunTest()
{
    Byte *compr, *uncompr;
    uLong comprLen = 10000*sizeof(int); /* don't overflow on MSDOS */
    uLong uncomprLen = comprLen;
    static const char* myVersion = ZLIB_VERSION;

    test2(zlibVersion()[0], myVersion[0]); /* Fails if incompatible zlib version */
    
    if (strcmp(zlibVersion(), ZLIB_VERSION) != 0) {
    	test.Printf(_L("warning: different zlib version\n"));
    }

    compr    = (Byte*)calloc((uInt)comprLen, 1);
    uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
    /* compr and uncompr are cleared to avoid reading uninitialized
     * data and to ensure that uncompr compresses well.
     */
    test2((compr == Z_NULL || uncompr == Z_NULL), 0); /* Fails if out of memory */

    test_compress(compr, comprLen, uncompr, uncomprLen);

    test_deflate(compr, comprLen);
    test_inflate(compr, comprLen, uncompr, uncomprLen);

    test_large_deflate(compr, comprLen, uncompr, uncomprLen);
    test_large_inflate(compr, comprLen, uncompr, uncomprLen);

    test_flush(compr, &comprLen);
    test_sync(compr, comprLen, uncompr, uncomprLen);
    comprLen = uncomprLen;

    test_dict_deflate(compr, comprLen);
    test_dict_inflate(compr, comprLen, uncompr, uncomprLen);
    
    delete compr;
    delete uncompr;
}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(KTestTitle);

	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err, RunTest());
	test2(err, KErrNone);
	
	test.End();
	test.Close();
	delete cleanup;

	__UHEAP_MARKEND;
	return KErrNone;
	}
