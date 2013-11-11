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
// Name        : tzlibadvanced.cpp
// 
//

#include "tzlib.h"
#include "zutil.h"
//#include "deflate.h"

#define COMPRESSED_DATA_BUFFER_LENGTH 200*sizeof(int) 
#define FAILURE_RATE_LIMIT 100
#define TEST_MID_MEM_LEVEL 5

// Add this to the windowBits value to use a GZip header instead of a Zlib header
#define TEST_GZIP_HEADER 16

// Add this to the windowBits when calling inflateInit2() to use automatic header detection
#define TEST_AUTO_HEADER_DETECTION 32

#define CHECK_CONDITION0L(condition, leaveErr, errMessage) \
	if (condition) \
		{ \
		ERR_PRINTF1(_L(errMessage)); \
		User::Leave(leaveErr); \
    	}

#define CHECK_CONDITION1L(condition, leaveErr, errMessage, p1) \
	if (condition) \
		{ \
		ERR_PRINTF2(_L(errMessage), p1); \
		User::Leave(leaveErr); \
    	}

_LIT(KParam1, "Param1");
_LIT(KParam2, "Param2");
_LIT(KParam3, "Param3");
_LIT(KParam4, "Param4");

// bufferDescriptor holds extra information required by in_func and out_func to perform their jobs
struct bufferDescriptor
	{
	Byte *buffer;
	int next;
	int length;
	};

/*
 * Helper member function that sets up the deflate stream for use.
 */
void CTestZlib::DeflateInitL(z_stream &aStream, const int aLevel, const int expectedResult)
	{
	TInt err = Z_OK;
	
	aStream.zalloc = Z_NULL;
	aStream.zfree = Z_NULL;
	aStream.opaque = Z_NULL;
    
	err = deflateInit(&aStream, aLevel);
	CHECK_CONDITION1L(err != expectedResult, KErrNoMemory, "deflateInit error: %d", err);
	}

/*
 * Helper member function that sets up the deflate stream for use.
 */
void CTestZlib::DeflateInit2L(z_stream &aStream, const int aLevel, const int aMethod, const int aWindowBits, const int aMemLevel, const int aStrategy, const int expectedResult)
	{
	int err;
	
	aStream.zalloc = Z_NULL;
	aStream.zfree = Z_NULL;
	aStream.opaque = Z_NULL;
    
	err =  deflateInit2(&aStream, aLevel, aMethod, aWindowBits, aMemLevel, aStrategy);
	CHECK_CONDITION1L(err != expectedResult, KErrNoMemory, "deflateInit2 error: %d", err);
	}

/*
 * Helper member function that compresses data using the deflate function.
 */
TInt CTestZlib::DeflateCompress(z_stream &aStream, Byte *aInputData, int aInputDataLength, Byte *aCompressedDataBuffer, int aCompressedDataBufferLength)
	{
	int err;
	int flush;
	
	// Compress data in the input buffer and place compressed data in the output buffer
	aStream.next_in  = aInputData;
	aStream.next_out = aCompressedDataBuffer; 
    
    do 
    	{
    	if (aStream.total_in < aInputDataLength)
    		{
			aStream.avail_in = 1; // force small buffer
    		}
    	
    	flush = (aStream.total_in == aInputDataLength) ?  Z_FINISH : Z_NO_FLUSH;
    	
	    // Run deflate() on input until output buffer not full
    	// Finish compression if all of input buffer has been read in
	    do 
	    	{
	    	if (aStream.total_out < aCompressedDataBufferLength)
	    		{
				aStream.avail_out = 1; // force small buffer
	    		}

	        err = deflate(&aStream, flush); 
	        if(err != Z_OK && err != Z_STREAM_END) 
	    		{ 
	    		return err; 
	    	    } 
	    	} while(aStream.avail_out == 0 && err == Z_OK);
    	} while(err != Z_STREAM_END); 
    
    return Z_OK;
	}

/*
 * Helper member function that cleans up a deflate stream.
 */
void CTestZlib::DeflateEnd(TAny *aStream)
	{
	if(((z_stream *)aStream)->state != NULL)
		{
		deflateEnd((z_stream *)aStream);  
		}
	}

/*
 * Helper member function that sets up the inflate stream for use.
 */
void CTestZlib::InflateInitL(z_stream &aStream, const int expectedResult)
	{
	TInt err = Z_OK;
	
	aStream.zalloc = Z_NULL;
	aStream.zfree = Z_NULL;
	aStream.opaque = Z_NULL;
    
	err = inflateInit(&aStream);
	CHECK_CONDITION1L(err != expectedResult, KErrNoMemory, "inflateInit error: %d", err);	
	}

/*
 * Helper member function that sets up the inflate stream for use.
 */
void CTestZlib::InflateInit2L(z_stream &aStream, const int aWindowBits, const int expectedResult)
	{
	TInt err = Z_OK;
	
	aStream.zalloc = Z_NULL;
	aStream.zfree = Z_NULL;
	aStream.opaque = Z_NULL;
    
	err = inflateInit2(&aStream, aWindowBits);
	CHECK_CONDITION1L(err != expectedResult, KErrNoMemory, "inflateInit2 error: %d", err);
	}

void CTestZlib::InflateBackInitL(z_stream &aStream, const int aWindowBits, Bytef *aWindow, const int expectedResult)
	{
	int err;
	
	aStream.zalloc = Z_NULL;
	aStream.zfree = Z_NULL;
	aStream.opaque = Z_NULL;
	
	err = inflateBackInit(&aStream, aWindowBits, aWindow);
	CHECK_CONDITION1L(err != expectedResult, KErrNoMemory, "inflateBackInit error: %d", err);
	}

/*
 * Helper member function that decompresses data using the inflate function.
 */
TInt CTestZlib::InflateDecompress(z_stream &aStream, Byte *aCompressedDataBuffer, int aCompressedDataLength, Byte *aDecompressedDataBuffer, int aDecompressedDataBufferLength)
	{
	int err;
	
	// Decompress data in the aCompressedDataBuffer and place it in the aDecompressedDataBuffer
	aStream.next_in  = aCompressedDataBuffer;
	aStream.next_out = aDecompressedDataBuffer;    
    
	// Keep providing input data and output space for deflate()
	do 
		{
		if(aStream.total_in < aCompressedDataLength)
    		{
    		aStream.avail_in = 1; // force small buffer 
    		}

		if (aStream.total_out < aDecompressedDataBufferLength)
    		{
			aStream.avail_out = 1; // force small buffer
    		}
    	
        err = inflate(&aStream, Z_NO_FLUSH); 
        if(err != Z_OK && err != Z_STREAM_END) 
    		{  
    		return err; 
    	    } 
		} while(err != Z_STREAM_END); 
	
	return Z_OK;
	}

/*
 * Helper member function that cleans up a inflate stream.
 */
void CTestZlib::InflateEnd(TAny *aStream)
	{
	if(((z_stream *)aStream)->state != NULL)
		{
		inflateEnd((z_stream *)aStream);  
		}
	}

/*
 * Helper member function that cleans up an inflate back stream.
 */
void CTestZlib::InflateBackEnd(TAny *aStream)
	{
	if(((z_stream *)aStream)->state != NULL)
		{
		inflateBackEnd((z_stream *)aStream);  
		}
	}

/*
 * Helper member function that compares the contents of two GZip headers to make
 * sure they are the same.
 */
TBool CTestZlib::GZipHeadersEqual(const gz_header &header1, const gz_header &header2)
	{
	TBool headersEqual = true;
	
	if(header1.text != header2.text)
		{
		INFO_PRINTF3(_L("readGZipHeader.text - Expected %d and got %d"), header1.text, header2.text);
		headersEqual = false;
		}
	
	if(header1.time != header2.time)
		{
		INFO_PRINTF3(_L("readGZipHeader.time - Expected %d and got %d"), header1.time, header2.time);
		headersEqual = false;
		}
	
	if(header1.xflags != header2.xflags)
		{
		INFO_PRINTF3(_L("readGZipHeader.xflags - Expected %d and got %d"), header1.xflags, header2.xflags);
		headersEqual = false;
		}
	
	if(header1.os != header2.os)
		{
		INFO_PRINTF3(_L("readGZipHeader.os - Expected %d and got %d"), header1.os, header2.os);
		headersEqual = false;
		}

	if(header1.extra != NULL & header2.extra != NULL)
		{
		if(header1.extra_len != header2.extra_len)
			{
			INFO_PRINTF1(_L("readGZipHeader.extra_len - Unexpected value."));
			headersEqual = false;
			}
		else if(memcmp(header1.extra, header2.extra, header1.extra_len) != 0)
			{
			INFO_PRINTF1(_L("readGZipHeader.extra - Unexpected value."));
			headersEqual = false;
			}
		}
	else if(header1.extra == NULL && header2.extra != NULL
			|| header1.extra != NULL && header2.extra == NULL)
		{
		INFO_PRINTF1(_L("readGZipHeader.extra - Unexpected value."));
		headersEqual = false;
		}
	
	if(header1.name != NULL && header2.name != NULL)
		{
		if(strcmp((char *)header1.name, (char *)header2.name) != 0)
			{
			INFO_PRINTF1(_L("readGZipHeader.name - Unexpected value. The headers contained different strings."));
			headersEqual = false;
			}
		}
	else if((header1.name == NULL && header2.name != NULL)
			|| (header1.name != NULL && header2.name == NULL))
		{
		INFO_PRINTF1(_L("readGZipHeader.name - Unexpected value. One of the headers contained a NULL value."));
		headersEqual = false;
		}
	
	if(header1.comment != NULL && header2.comment != NULL)
		{
		if(strcmp((char *)header1.comment, (char *)header2.comment) != 0)
			{
			INFO_PRINTF1(_L("readGZipHeader.comment - Unexpected value. The headers contained different strings."));
			headersEqual = false;
			}
		}
	else if((header1.comment == NULL && header2.comment != NULL)
			|| (header1.comment != NULL && header2.comment == NULL))
		{
		INFO_PRINTF1(_L("readGZipHeader.comment - Unexpected value. One of the headers contained a NULL value."));
		headersEqual = false;
		}
	
	if(header1.hcrc != header2.hcrc)
		{
		INFO_PRINTF3(_L("readGZipHeader.hcrc - Expected %d and got %d"), header1.hcrc, header2.hcrc);
		headersEqual = false;
		}

	if(header1.done != header2.done)
		{
		INFO_PRINTF3(_L("readGZipHeader.done - Expected %d and got %d"), header1.done, header2.done);
		headersEqual = false;
		}
	
	return headersEqual;
	}

/*
 * Helper member function that checks to see if the header is the default GZip header
 */
TBool CTestZlib::IsDefaultGZipHeader(const gz_header &header)
	{
	gz_header defaultHeader;
	
	// gz_header default values
	defaultHeader.text = 0;
	defaultHeader.time = 0;
	defaultHeader.xflags = 0;
	defaultHeader.os = OS_CODE;
	defaultHeader.extra = NULL;
	defaultHeader.name = NULL;
	defaultHeader.comment = NULL;
	defaultHeader.hcrc = 0;
	defaultHeader.done = 1;
	
	return GZipHeadersEqual(defaultHeader, header);
	}

/*
 * Helper member function that compresses an input buffer and place it in a compressed output buffer
 */
void CTestZlib::CompressDataL(StreamSettings &aStreamSettings, z_stream &aStream, Byte *aInputData, int aInputDataLength, Byte *aCompressedDataBuffer, int aCompressedDataBufferLength, gz_headerp aHeader)
	{
	TInt err = Z_OK;
	
	// Initialise the deflate stream
	if(aStreamSettings.deflateInit2 == false)
		{
		this->DeflateInitL(aStream, aStreamSettings.level);
		}
	else
		{
		this->DeflateInit2L(aStream, aStreamSettings.level, aStreamSettings.method, aStreamSettings.deflateWindowBits, aStreamSettings.memLevel, aStreamSettings.strategy);
		}
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &aStream));
	
	// If a GZip header is specified insert the gzip header information to the 
	// start of the output when deflate is called
	if(aHeader != NULL)
		{
		err = deflateSetHeader(&aStream, aHeader);
		CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflateSetHeader error: %d", err);
		}
	
	// Compress the input
	err = this->DeflateCompress(aStream, aInputData, aInputDataLength, aCompressedDataBuffer, aCompressedDataBufferLength);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflate error: %d", err);
	
	// Clean up the deflateStream
	err = deflateEnd(&aStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflateEnd error: %d", err);
	CleanupStack::Pop(1);
	}

/*
 * Helper function that deflates some input data as a gzip stream and then inflates 
 * the compressed data.
 */
TVerdict CTestZlib::DefInfGZipHeaderL(const TBool aIgnoreHeader, const TBool aAutoDetectHeader, gz_headerp aSpecifiedGZipHeader)
	{
	int err = Z_OK;
	TVerdict verdict = EPass;
	
	// Streams
	z_stream deflateStream;
	z_stream inflateStream;
	
	// Headers
	gz_header readGZipHeader;
	readGZipHeader.extra = NULL;
	readGZipHeader.name = NULL;
	readGZipHeader.comment = NULL;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0"; // data to compress
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    Byte *decompressedDataBuffer = NULL;  
    uLong decompressedDataBufferLength = inputDataLength;
    
    // deflateInit2 and inflateInit2 arguments
    StreamSettings streamSettings;
    streamSettings.deflateInit2 = true;
    streamSettings.level = Z_DEFAULT_COMPRESSION;
    streamSettings.method = Z_DEFLATED;
    streamSettings.deflateWindowBits = MAX_WBITS + TEST_GZIP_HEADER;
    streamSettings.inflateWindowBits = MAX_WBITS;
    streamSettings.memLevel = TEST_MID_MEM_LEVEL;
    streamSettings.strategy = Z_DEFAULT_STRATEGY;

    streamSettings.inflateWindowBits += (aAutoDetectHeader == true) ? TEST_AUTO_HEADER_DETECTION : TEST_GZIP_HEADER;

    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));
    
    decompressedDataBuffer = (Byte *)User::AllocZ(decompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(decompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for decompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, decompressedDataBuffer));
    
    // Allocate memory, if required, for readGZipHeader if header data is being 
    // checked and if a specified GZip header has been given. This is because 
    // extra, name and comment may have been set in the specified GZip header.
    if(aIgnoreHeader == false && aSpecifiedGZipHeader != NULL) 
    	{
    	if(aSpecifiedGZipHeader->extra != NULL)
    		{
    		readGZipHeader.extra_max = aSpecifiedGZipHeader->extra_max;
    		readGZipHeader.extra = (Bytef *)User::AllocZ(aSpecifiedGZipHeader->extra_max * sizeof(Bytef));
    		CHECK_CONDITION0L(readGZipHeader.extra == NULL, KErrNoMemory, "Failed to allocate memory for readGZipHeader.extra.");
		    CleanupStack::PushL(TCleanupItem(User::Free, readGZipHeader.extra));
    		}

    	if(aSpecifiedGZipHeader->name != NULL)
    		{
    		readGZipHeader.name_max = aSpecifiedGZipHeader->name_max;
		    readGZipHeader.name = (Bytef *)User::AllocZ(aSpecifiedGZipHeader->name_max * sizeof(Bytef));
		    CHECK_CONDITION0L(readGZipHeader.name == NULL, KErrNoMemory, "Failed to allocate memory for readGZipHeader.name.");
		    CleanupStack::PushL(TCleanupItem(User::Free, readGZipHeader.name));
    		}

    	if(aSpecifiedGZipHeader->comment != NULL)
    		{
    		readGZipHeader.comm_max = aSpecifiedGZipHeader->comm_max;
		    readGZipHeader.comment = (Bytef *)User::AllocZ(aSpecifiedGZipHeader->comm_max * sizeof(Bytef));
		    CHECK_CONDITION0L(readGZipHeader.comment == NULL, KErrNoMemory, "Failed to allocate memory for readGZipHeader.comment.");
		    CleanupStack::PushL(TCleanupItem(User::Free, readGZipHeader.comment));
    		}
    	}
	this->CompressDataL(streamSettings, deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength, aSpecifiedGZipHeader);

	// Initialise the inflateStream
	this->InflateInit2L(inflateStream, streamSettings.inflateWindowBits);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStream));

	// If the header information is not to be ignored get the GZip 
	// header information when inflate is called
	if(aIgnoreHeader == false)
		{
		err = inflateGetHeader(&inflateStream, &readGZipHeader);
		CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateGetHeader error: %d", err);
		}

	err = this->InflateDecompress(inflateStream, compressedDataBuffer, deflateStream.total_out, decompressedDataBuffer, decompressedDataBufferLength);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflate error: %d", err);

	// Clean up the inflateStream
	err = inflateEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateEnd error: %d", err);
	CleanupStack::Pop(1);
	
	// If the header information hasn't been ignored Check the GZip header 
	// contains the expected information
	if(aIgnoreHeader == false)
		{
		if(aSpecifiedGZipHeader != NULL)
			{
			verdict = GZipHeadersEqual(*aSpecifiedGZipHeader, readGZipHeader) ? EPass : EFail;
			}
		else
			{
			verdict = IsDefaultGZipHeader(readGZipHeader) ? EPass : EFail;
			}
		}

	if(readGZipHeader.comment != NULL)
		{
		CleanupStack::PopAndDestroy(1);
		}
	
	if(readGZipHeader.name != NULL)
		{
		CleanupStack::PopAndDestroy(1);
		}
	
	if(readGZipHeader.extra != NULL)
		{
		CleanupStack::PopAndDestroy(1);
		}

	// Free buffers
	CleanupStack::PopAndDestroy(2);
	    
	return verdict;
	}

/*
 * Helper function that deflates some input data as a gzip stream with a specified header
 * and then inflates the compressed data.
 */
TVerdict CTestZlib::DefInfGZipSpecifiedHeaderL(TBool aIgnoreHeader, TBool aAutoDetectHeader)
	{
	gz_header specifiedGZipHeader;
	
	Bytef extra[] = "12345"; 
	Bytef name[] = "TestDefInfGZipSpecifiedHeaderManual\0";  
	Bytef comment[] = "This is a test comment.\0"; 

	// gz_header user specified values. Set the specifiedGZipHeader ready for deflateSetHeader
	specifiedGZipHeader.text = 1;
	specifiedGZipHeader.time = 101;
	specifiedGZipHeader.xflags = 0; // Set so that the value can be compared to the value in the header read from the deflated data
	specifiedGZipHeader.os = 1; // Amiga
	specifiedGZipHeader.extra = extra;
	specifiedGZipHeader.extra_len = TEST_MID_MEM_LEVEL;
	specifiedGZipHeader.extra_max = specifiedGZipHeader.extra_len + 10; // Add extra 10 to check readGZipHeader.extra_len is set properly
	specifiedGZipHeader.name = name;
	specifiedGZipHeader.name_max = strlen((char *)name) + 1;
	specifiedGZipHeader.comment = comment;
	specifiedGZipHeader.comm_max = strlen((char *)comment) + 1;
	specifiedGZipHeader.hcrc = 0;
	specifiedGZipHeader.done = 1; // Finished reading header. Set so that the value can be compared to the value in the header read from the deflated data
	
	return DefInfGZipHeaderL(aIgnoreHeader, aAutoDetectHeader, &specifiedGZipHeader);
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4259
@SYMTestCaseDesc     	To ensure deflation works after tuning the deflate stream using 
                        deflateTune().
@SYMTestPriority     	Medium
@SYMTestActions      	1.	Create a stream and initialise it. 
                        2.	Call deflateTune() passing it the specified parameters.
                        3.	Check the state of the stream to make sure that the values 
                            are identical to the specified parameters. Also check that 
                            Z_OK is returned.
                        4.	Deflate the input and cleanup using deflateEnd(), checking 
                            Z_OK is returned.
                        
                        Note: The test should be repeated for deflateInit() parameter values:
                        •	3 and 4 for level
                        
                        and for deflateTune() parameter values:
                        •	0 and a high value for good_length
                        •	0 and a high value for max_lazy
                        •	0 and a high value for nice_length
                        •	0 and a high value for max_chain                        
                        Appropriate high values can be picked based on the global 
                        configuration_table array in deflate.cpp.
@SYMTestExpectedResults deflateTune() should return Z_OK and the input should be compressed.
@SYMDEF                 REQ7362
*/
TVerdict CTestZlib::TestDeflateTuneL()
	{	
	int err = Z_OK;
	TBool ret;
	
	// Streams
	z_stream deflateStream;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer;
    
    // deflateInit arguments
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    int level = Z_DEFAULT_COMPRESSION;	
	int goodLength = 0;
    int maxLazy = 0;
    int niceLength = 0;
    int maxChain = 0;
	
    // Allocate memory for output buffer
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for output buffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));

	// Read in the values for deflate tune from the ini file
	ret = GetIntFromConfig(ConfigSection(), KParam1, goodLength);
	CHECK_CONDITION0L(!ret, KErrGeneral, "Failed to read Param1 from ini file");
	
	ret = GetIntFromConfig(ConfigSection(), KParam2, maxLazy);
	CHECK_CONDITION0L(!ret, KErrGeneral, "Failed to read Param2 from ini file");
	
	ret = GetIntFromConfig(ConfigSection(), KParam3, niceLength);
	CHECK_CONDITION0L(!ret, KErrGeneral, "Failed to read Param3 from ini file");
	
	ret = GetIntFromConfig(ConfigSection(), KParam4, maxChain);
	CHECK_CONDITION0L(!ret, KErrGeneral, "Failed to read Param4 from ini file");
	
	 // Initialise the stream
	this->DeflateInitL(deflateStream, level);
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &deflateStream));
	
	// Tune the deflate stream
	err = deflateTune(&deflateStream, goodLength, maxLazy, niceLength, maxChain);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflateTune error: %d", err);
	
	// Check deflateTune has set the streams state values correctly
	/*
	internal_state *s = deflateStream.state;
	CHECK_CONDITION0L(s->good_match != goodLength || s->max_lazy_match != maxLazy || s->nice_match != niceLength || s->max_chain_length != maxChain, KErrGeneral, "deflateTune did not set the stream->state values correctly");
	*/

	err = this->DeflateCompress(deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflate error: %d", err);
	
	// Clean up the deflate stream
	err = deflateEnd(&deflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflateEnd error: %d", err);
	CleanupStack::Pop(1);

	// Free buffer
	CleanupStack::PopAndDestroy(1);  
	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4260
@SYMTestCaseDesc     	Check deflateTune() fails when given an invalid stream.
@SYMTestPriority     	Medium
@SYMTestActions      	1.	Create a stream and initialise it.
                        2.	Call deflateTune() passing it parameter values:
                            a.	NULL for the stream argument
                            b.	a stream whose state is NULL for the stream argument
@SYMTestExpectedResults deflateTune() fails returning Z_STREAM_ERROR in both cases.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDeflateTuneFailL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream deflateStream;
	
	// deflateInit2 arguments
	int level = Z_DEFAULT_COMPRESSION;	
	
	// Initialise the stream
	this->DeflateInitL(deflateStream, level);
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &deflateStream));
	
	// Try tuning a NULL deflate stream
	err = deflateTune(NULL, 0, 0, 0, 0);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "deflateTune on a NULL stream returned an unexpected value: %d", err);
	
	// Keep a pointer to the streams state so we can clean up properly with deflateEnd()
	internal_state *deflateState  = deflateStream.state;
	
	// Try tuning a deflate stream that has a NULL state
	deflateStream.state = NULL;
	err = deflateTune(&deflateStream, 0, 0, 0, 0);
	deflateStream.state = deflateState;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "deflateTune on a stream with a NULL state returned an unexpected value: %d", err);
	
	// Clean up the deflate stream
	err = deflateEnd(&deflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflateEnd error: %d", err);
	CleanupStack::Pop(1);
	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4261
@SYMTestCaseDesc     	Check input can be compressed and decompressed with default gzip 
                        header not being read.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it using deflateInit2(), 
                            adding 16 to windowBits.
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using inflateInit2(), 
                            adding 16 to windowBits.
                        4.	Inflate the input and cleanup using inflateEnd().
@SYMTestExpectedResults The data should be compressed with deflateEnd() returning Z_OK and 
                        decompressed with inflateEnd() returning Z_OK. There should be no 
                        header information stored.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDefInfGZipDefaultHeaderIgnoreL()
	{
	return DefInfGZipHeaderL(true, false, NULL);
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4262
@SYMTestCaseDesc     	Check input can be compressed and decompressed with default gzip 
                        header being read using automatic header detection.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it using deflateInit2(), 
                            adding 16 to windowBits.
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using inflateInit2(), 
                            adding 32 to windowBits. 
                        4.	Use inflateGetHeader() to get the default header information 
                            and make sure the gzip header information is set correctly.
                        5.	Inflate the input and cleanup using inflateEnd().
@SYMTestExpectedResults The data should be compressed with deflateEnd() returning Z_OK and 
                        decompressed with inflateEnd() returning Z_OK. There should be 
                        correct header information stored.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDefInfGZipDefaultHeaderAutoL()
	{
	return DefInfGZipHeaderL(false, true, NULL);
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4263
@SYMTestCaseDesc     	Check input can be compressed and decompressed with specified gzip 
                        header being read.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it using deflateInit2(), 
                            adding 16 to windowBits.
                        2.	Use deflateSetHeader() to specify header information and check 
                            that the gzip header information is set correctly. 
                        3.	Deflate the input and cleanup using deflateEnd(). 
                        4.	Create an inflate stream and initialise it using inflateInit2(), 
                            adding 16 to windowBits. 
                        5.	Use inflateGetHeader() to get the header information and make 
                            sure that it matches the header information originally entered. 
                        6.	Inflate the input and cleanup using inflateEnd().
@SYMTestExpectedResults The data should be compressed with deflateEnd() returning Z_OK and 
                        the header details being set correctly. The data should also be 
                        decompressed with inflateEnd() returning Z_OK and the header details 
                        matching the original header information. There should also be header 
                        information stored.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDefInfGZipSpecifiedHeaderManualL()
	{
	return DefInfGZipSpecifiedHeaderL(false, false);
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4264
@SYMTestCaseDesc     	Check input can be compressed and decompressed with specified gzip 
                        header being read using automatic header detection.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it using deflateInit2(), 
                            adding 16 to windowBits. 
                        2.	Use deflateSetHeader() to specify header information and check 
                            that the gzip header information is set correctly. 
                        3.	Deflate the input and cleanup using deflateEnd(). 
                        4.	Create an inflate stream and initialise it using inflateInit2(), 
                            adding 32 to windowBits. 
                        5.	Use inflateGetHeader() to get the header information and make 
                            sure that it matches the header information originally entered. 
                        6.	Inflate the input and cleanup using inflateEnd().
@SYMTestExpectedResults The data should be compressed with deflateEnd() returning Z_OK and 
                        the header details being set correctly. The data should also be 
                        decompressed with inflateEnd() returning Z_OK and the header details 
                        matching the original header information. There should also be header 
                        information stored.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDefInfGZipSpecifiedHeaderAutoL()
	{
	return DefInfGZipSpecifiedHeaderL(false, true);
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4265
@SYMTestCaseDesc     	Check input can be compressed and decompressed with zlib header 
                        using automatic header detection.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it. 
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using inflateInit2(), 
                            adding 32 to windowBits. 
                        4.	Inflate the input and cleanup using inflateEnd().
@SYMTestExpectedResults The data should be compressed with deflateEnd() returning Z_OK 
                        and decompressed with inflateEnd returning Z_OK.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDefInfZlibHeaderAutoL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream deflateStream;
	z_stream inflateStream;
	
	// Stream settings
	StreamSettings streamSettings;
	streamSettings.deflateInit2 = false;
	streamSettings.level = Z_DEFAULT_COMPRESSION;
	streamSettings.inflateWindowBits = MAX_WBITS + TEST_AUTO_HEADER_DETECTION;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0"; // data to compress
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    Byte *decompressedDataBuffer = NULL;  
    uLong decompressedDataBufferLength = inputDataLength;
	
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));

    decompressedDataBuffer = (Byte *)User::AllocZ(decompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(decompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for decompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, decompressedDataBuffer));

    this->CompressDataL(streamSettings, deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);
 
	// Initialise the inflateStream
	this->InflateInit2L(inflateStream, streamSettings.inflateWindowBits);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStream));
	
	err = this->InflateDecompress(inflateStream, compressedDataBuffer, deflateStream.total_out, decompressedDataBuffer, decompressedDataBufferLength);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflate error: %d", err);
	
	// Clean up the inflateStream
	err = inflateEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateEnd error: %d", err);
	CleanupStack::Pop(1);
	
	CleanupStack::PopAndDestroy(2);	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4266
@SYMTestCaseDesc     	Check deflateSetHeader() fails when given an invalid stream.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a stream and initialise it using deflateInit2(), adding 
                            16 to windowBits. 
                        2.	Call deflateSetHeader() passing it parameter values:
                            a.	NULL for the stream argument
                            b.	a stream whose state is set to NULL for the stream argument
                            c.	a zlib stream for the stream argument
                            d.	a raw stream for the stream argument
@SYMTestExpectedResults deflateSetHeader() fails returning Z_STREAM_ERROR in all four cases.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDeflateSetHeaderFailsL()
	{
	int err;
	
	// Streams
	z_stream gZipDeflateStream;
	z_stream zLibDeflateStream;
	z_stream rawDeflateStream;
	
	// GZip headers
	gz_header specifiedGZipHeader;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    
    // deflateInit2 arguments
	int level = Z_DEFAULT_COMPRESSION;
	int method = Z_DEFLATED;
	int gZipDeflateWindowBits = MAX_WBITS + TEST_GZIP_HEADER;
	int memLevel = TEST_MID_MEM_LEVEL;
	int strategy = Z_DEFAULT_STRATEGY;
	
	// GZip header user specified values
	Bytef extra[] = "12345"; 
	Bytef name[] = "TestDefInfGZipSpecifiedHeaderManual\0";  
	Bytef comment[] = "This is a test comment.\0"; 
	
	// Set the specifiedGZipHeader ready for deflateSetHeader
	specifiedGZipHeader.text = 1;
	specifiedGZipHeader.time = 101;
	specifiedGZipHeader.os = 1;
	specifiedGZipHeader.extra = extra;
	specifiedGZipHeader.extra_len = 5;
	specifiedGZipHeader.extra_max = specifiedGZipHeader.extra_len + 10; // Add extra 10 to check readGZipHeader.extra_len is set properly
	specifiedGZipHeader.name = name;
	specifiedGZipHeader.name_max = strlen((char *)name) + 1;
	specifiedGZipHeader.comment = comment;
	specifiedGZipHeader.comm_max = strlen((char *)comment) + 1;
	specifiedGZipHeader.hcrc = 0;
	
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));
	
	// Initialise the deflate streams
	this->DeflateInit2L(gZipDeflateStream, level, method, gZipDeflateWindowBits, memLevel, strategy);
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &gZipDeflateStream));
	
	this->DeflateInitL(zLibDeflateStream, level);
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &zLibDeflateStream));

	this->DeflateInit2L(rawDeflateStream, level, method, -MAX_WBITS, memLevel, strategy);
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &rawDeflateStream));
	
	// Try setting a NULL streams header
	err = deflateSetHeader(NULL, &specifiedGZipHeader);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "deflateSetHeader on a NULL stream returned an unexpected value: %d", err);

	// Try setting the header for a deflate stream that has a NULL state
	// we must keep a pointer to the streams state so we can clean up properly with deflateEnd
	internal_state *deflateState  = gZipDeflateStream.state;
	gZipDeflateStream.state = NULL;
	
	err = deflateSetHeader(&gZipDeflateStream, &specifiedGZipHeader);
	
	gZipDeflateStream.state = deflateState;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "deflateSetHeader on a stream with a NULL state returned an unexpected value: %d", err);
	
	// Try setting a header for a zlib stream
	err = deflateSetHeader(&zLibDeflateStream, &specifiedGZipHeader);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "deflateSetHeader on a zlib stream returned an unexpected value: %d", err);
	
	// Try setting a header for a raw stream
	err = deflateSetHeader(&rawDeflateStream, &specifiedGZipHeader);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "deflateSetHeader on a raw stream returned an unexpected value: %d", err);
	
	// Clean up the deflate streams	
	err = deflateEnd(&gZipDeflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "gZipDeflateStream - deflateEnd error: %d", err);
	
	err = deflateEnd(&zLibDeflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "zLibDeflateStream - deflateEnd error: %d", err);
	
	// deflateInit2() leaves a raw stream in a busy state. This results in deflateEnd()
	// returning a Z_DATA_ERROR, unless deflate() has been called on the stream.
	err = deflateEnd(&rawDeflateStream);
	CHECK_CONDITION1L(err != Z_DATA_ERROR, KErrGeneral, "rawDeflateStream - deflateEnd error: %d", err);
	
	CleanupStack::Pop(3);
	CleanupStack::PopAndDestroy(1);	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4267
@SYMTestCaseDesc     	Check inflateGetHeader() fails when given an invalid gzip stream.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it using deflateInit2(), adding 16 to windowBits. 
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using inflateInit2(), adding 16 to windowBits.
                        4.	Call inflateGetHeader() passing it parameter values:
                            e.	NULL for the stream argument
                            f.	a stream whose state is set to NULL for the stream argument
                            g.	a zlib stream for the stream argument
                            h.	a raw stream for the stream argument
@SYMTestExpectedResults inflateGetHeader() should fail returning Z_STREAM_ERROR for the 
                        first 2 cases and Z_DATA_ERROR for the second 2 case.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateGetHeaderFailsL()
	{	
	int err = Z_OK;
	
	// Streams
	z_stream gZipDeflateStream;
	z_stream zLibDeflateStream;
	z_stream rawDeflateStream;
	z_stream gZipInflateStream;
	z_stream zLibInflateStream;
	z_stream rawInflateStream;
	
	// GZip headers
	gz_header gZipHeader;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *gZipCompressedDataBuffer = NULL;
    Byte *zLibCompressedDataBuffer = NULL;
    Byte *rawCompressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    
    // deflateInit2 and inflateInit2 arguments
    StreamSettings gZipStreamSettings;
    gZipStreamSettings.deflateInit2 = true;
    gZipStreamSettings.level = Z_DEFAULT_COMPRESSION;
    gZipStreamSettings.method = Z_DEFLATED;
    gZipStreamSettings.deflateWindowBits = MAX_WBITS + TEST_GZIP_HEADER;
    gZipStreamSettings.inflateWindowBits = MAX_WBITS + TEST_GZIP_HEADER;
    gZipStreamSettings.memLevel = TEST_MID_MEM_LEVEL;
    gZipStreamSettings.strategy = Z_DEFAULT_STRATEGY;  
    
    StreamSettings zLibStreamSettings;
    zLibStreamSettings.deflateInit2 = false;
    zLibStreamSettings.level = Z_DEFAULT_COMPRESSION;
    
    StreamSettings rawStreamSettings;
    rawStreamSettings.deflateInit2 = true;
    rawStreamSettings.level = Z_DEFAULT_COMPRESSION;
    rawStreamSettings.method = Z_DEFLATED;
    rawStreamSettings.deflateWindowBits = -MAX_WBITS;
    rawStreamSettings.inflateWindowBits = -MAX_WBITS;
    rawStreamSettings.memLevel = TEST_MID_MEM_LEVEL;
    rawStreamSettings.strategy = Z_DEFAULT_STRATEGY; 
	
	// Allocate memory for buffers
	gZipCompressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
	CHECK_CONDITION0L(gZipCompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for gZipCompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, gZipCompressedDataBuffer));
	
	zLibCompressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
	CHECK_CONDITION0L(zLibCompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for zLibCompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, zLibCompressedDataBuffer));
    
	rawCompressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
	CHECK_CONDITION0L(rawCompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for rawCompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, rawCompressedDataBuffer));
    
    // Compress data
    INFO_PRINTF1(_L("Compress gzip:"));
    this->CompressDataL(gZipStreamSettings, gZipDeflateStream, (Byte *)inputData, inputDataLength, gZipCompressedDataBuffer, compressedDataBufferLength);
    INFO_PRINTF1(_L("Compress zlib:"));
    this->CompressDataL(zLibStreamSettings, zLibDeflateStream, (Byte *)inputData, inputDataLength, zLibCompressedDataBuffer, compressedDataBufferLength);
    INFO_PRINTF1(_L("Compress raw:"));
    this->CompressDataL(rawStreamSettings, rawDeflateStream, (Byte *)inputData, inputDataLength, rawCompressedDataBuffer, compressedDataBufferLength);

	// Initialise the inflateStreams
    INFO_PRINTF1(_L("gZipInflateStream:"));
	this->InflateInit2L(gZipInflateStream, gZipStreamSettings.inflateWindowBits);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &gZipInflateStream));
	
	INFO_PRINTF1(_L("zLibInflateStream:"));
	this->InflateInitL(zLibInflateStream);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &zLibInflateStream));
	
	INFO_PRINTF1(_L("rawInflateStream:"));
	this->InflateInit2L(rawInflateStream, rawStreamSettings.inflateWindowBits);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &rawInflateStream));
	
	// Try and get header information from a NULL stream
	err = inflateGetHeader(NULL, &gZipHeader);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateGetHeader on a NULL stream returned an unexpected value: %d", err);

	// Try getting the header for a deflate stream that has a NULL state
	// we must keep a pointer to the streams state so we can clean up properly with deflateEnd
	internal_state *deflateState  = gZipDeflateStream.state;
	gZipDeflateStream.state = NULL;
	
	err = inflateGetHeader(&gZipDeflateStream, &gZipHeader);
	
	gZipDeflateStream.state = deflateState;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateGetHeader on a stream with a NULL state returned an unexpected value: %d", err);
	
	// Try and get header information from a zlib stream
	err = inflateGetHeader(&zLibDeflateStream, &gZipHeader);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateGetHeader on a zlib stream returned an unexpected value: %d", err);
		
	// Try and get header information from a raw stream
	err = inflateGetHeader(&rawDeflateStream, &gZipHeader);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateGetHeader on a raw stream returned an unexpected value: %d", err);
	
	// Clean up the inflateStreams
	err = inflateEnd(&gZipInflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "gZipInflateStream - inflateEnd error: %d", err);
	
	err = inflateEnd(&zLibInflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "zLibInflateStream - inflateEnd error: %d", err);
	
	err = inflateEnd(&rawInflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "rawInflateStream - inflateEnd error: %d", err);
	
	CleanupStack::Pop(3);
    CleanupStack::PopAndDestroy(3);
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4268
@SYMTestCaseDesc     	Check output can be deflated in raw format and inflated.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a stream and initialise it using deflateInit2(), 
                            setting windowBits between -8 and -15. 
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using inflateInit2(), 
                            setting windowBits to be equal or less than the windowBits 
                            value used for deflateInit2(). 
                        4.	Inflate the input using inflate() and cleanup using inflateEnd().
@SYMTestExpectedResults The data should be compressed with deflateEnd returning Z_OK and 
                        decompressed with inflateEnd returning Z_OK.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDefInfRawL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream deflateStream;	
	z_stream inflateStream;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    Byte *decompressedDataBuffer = NULL;
    uLong decompressedDataBufferLength = inputDataLength;
    
    // deflateInit2 and inflateInit2 arguments
    StreamSettings streamSettings;
    streamSettings.deflateInit2 = true;
    streamSettings.level = Z_DEFAULT_COMPRESSION;
    streamSettings.method = Z_DEFLATED;
    streamSettings.deflateWindowBits = -MAX_WBITS;
    streamSettings.inflateWindowBits = streamSettings.deflateWindowBits;
    streamSettings.memLevel = TEST_MID_MEM_LEVEL;
    streamSettings.strategy = Z_DEFAULT_STRATEGY;
	
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));

    decompressedDataBuffer = (Byte *)User::AllocZ(decompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(decompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for decompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, decompressedDataBuffer));

    this->CompressDataL(streamSettings, deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);

	// Initialise the inflateStream
	this->InflateInit2L(inflateStream, streamSettings.inflateWindowBits);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStream));
	
	err = this->InflateDecompress(inflateStream, compressedDataBuffer, deflateStream.total_out, decompressedDataBuffer, decompressedDataBufferLength);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflate error: %d", err);
	
	// Clean up the inflateStream
	err = inflateEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateEnd error: %d", err);

	CleanupStack::Pop(1);
	CleanupStack::PopAndDestroy(2);
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4269
@SYMTestCaseDesc     	Check output is not generated in raw format when windowBits < -15 
                        or > -8.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a stream and initialise it using deflateInit2() passing 
                            it the specified parameters.

                        Note: The test should be repeated for deflateInit2() parameter values:
                        •	< -15 and > -8 for windowBits
@SYMTestExpectedResults deflateInit2() should fail returning Z_STREAM_ERROR. Note: if 
                        windowBits is set between 8-15 and 24-31 there will be no error 
                        as the input will be deflated as a zlib/gzip stream.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDefRawFailsL()
	{
	TBool ret;
	
	// Streams
	z_stream deflateStream;	 
	
    // deflateInit2 and inflateInit2 arguments
	int level = Z_DEFAULT_COMPRESSION;
	int method = Z_DEFLATED;
	int deflateWindowBits;
	int memLevel = TEST_MID_MEM_LEVEL;
	int strategy = Z_DEFAULT_STRATEGY;
    
    // Read in the value for windowBits from the ini file
	ret = GetIntFromConfig(ConfigSection(), KParam1, deflateWindowBits);
	CHECK_CONDITION0L(!ret, KErrGeneral, "Failed to read Param1 from ini file");

    // Initialise the deflate stream
	this->DeflateInit2L(deflateStream, level, method, deflateWindowBits, memLevel, strategy, Z_STREAM_ERROR);

	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4270
@SYMTestCaseDesc     	Check raw input is not inflated when windowBits < -15 or > -8.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a stream and initialise it using deflateInit2(), 
                            setting windowBits between -8 and -15. 
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using inflateInit2() 
                            passing it the specified parameters.
                            
                        Note: The test should be repeated for inflateInit2() parameter values:
                        •	< -15, > -8 & < 8, >= 8 & <= 15 and >= 24 & <= 31 for windowBits
                        For cases >= 8 & <= 15 and >= 24 & <= 31 inflate the compressed data 
                        and call inflateEnd() to clean up.
@SYMTestExpectedResults inflateInit2() should fail returning Z_STREAM_ERROR. Note: if 
                        windowBits is set between 8-15 and 24-31 a Z_DATA_ERROR will be 
                        given as it will inflate as a zlib/gzip stream.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDefInfRawFailsL()
	{
	int err = Z_OK;
	TBool ret;
	
	// Streams
	z_stream deflateStream;	
	z_stream inflateStream;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    Byte *decompressedDataBuffer = NULL;
    uLong decompressedDataBufferLength = inputDataLength;
    
    // deflateInit2 and inflateInit2 arguments
	int level = Z_DEFAULT_COMPRESSION;
	int method = Z_DEFLATED;
	int deflateWindowBits = -MAX_WBITS;
	int inflateWindowBits;
	int memLevel = TEST_MID_MEM_LEVEL;
	int strategy = Z_DEFAULT_STRATEGY;
	
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));

    decompressedDataBuffer = (Byte *)User::AllocZ(decompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(decompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for decompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, decompressedDataBuffer));
    
    // Read in the value for windowBits from the ini file
	ret = GetIntFromConfig(ConfigSection(), KParam1, inflateWindowBits);
	CHECK_CONDITION0L(!ret, KErrGeneral, "Failed to read Param1 from ini file");

    // Initialise the deflate stream
	this->DeflateInit2L(deflateStream, level, method, deflateWindowBits, memLevel, strategy);
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &deflateStream));
	
	// Compress the input
	err = this->DeflateCompress(deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);
	CHECK_CONDITION1L(err!= Z_OK, KErrGeneral, "deflate error: %d", err);
	
	// Clean up the deflate stream
	err = deflateEnd(&deflateStream);
	CHECK_CONDITION1L(err!= Z_OK, KErrGeneral, "deflateEnd error: %d", err);
	CleanupStack::Pop(1);
	
	// Initialise the inflateStream
	// If windowBits are used that specify a zlib or gzip stream we get a different error
	if((inflateWindowBits >= 8 && inflateWindowBits <= 15) || (inflateWindowBits >= 24 && inflateWindowBits <= 31))
		{	
		this->InflateInit2L(inflateStream, inflateWindowBits);
		CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStream));
		
		err = this->InflateDecompress(inflateStream, compressedDataBuffer, deflateStream.total_out, decompressedDataBuffer, decompressedDataBufferLength);
		CHECK_CONDITION1L(err!= Z_DATA_ERROR, KErrGeneral, "inflate error: %d", err);
		
		// Clean up the inflateStream
		err = inflateEnd(&inflateStream);
		CHECK_CONDITION1L(err!= Z_OK, KErrGeneral, "inflateEnd error: %d", err);
		CleanupStack::Pop(1);
		}
	else 
		{
		this->InflateInit2L(inflateStream, inflateWindowBits, Z_STREAM_ERROR);
		}

	CleanupStack::PopAndDestroy(2);	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4271
@SYMTestCaseDesc     	To check the specified bits are added to the start of a raw stream 
                        correctly using deflatePrime().
@SYMTestPriority     	Low
@SYMTestActions      	1.	Create a stream and initialise it using deflateInit2(), setting 
                            windowBits between -8 and -15. 
                        2.	Call deflatePrime() passing it the specified parameters.
                        3.	Deflate the input and cleanup using deflateEnd(). 
                        4.	Check the bits at the front of the output are the expected ones.

                        Note: The test should be repeated for deflatePrime() parameter values:
                        •	0, 8 and 16 for bits
                        •	0, 216  and a number between 0 - 216 for value
@SYMTestExpectedResults deflatePrime() should return Z_OK and the bits at the front of the 
                        output are the expected ones.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDeflatePrimeL()
	{
	int err = Z_OK;
	TBool ret;
	
	// Streams
	z_stream deflateStream;	
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;

    // deflateInit2 and inflateInit2 arguments
	int level = Z_DEFAULT_COMPRESSION;
	int method = Z_DEFLATED;
	int deflateWindowBits = -MAX_WBITS;
	int memLevel = TEST_MID_MEM_LEVEL;
	int strategy = Z_DEFAULT_STRATEGY;
	
	// deflatePrime arguments
	int bits;
	int value;
	
	// Bits added to the start of the compressed stream
	Byte bit1 = 0;
	Byte bit2 = 0;
    
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));
    
    // Read in the values for bits and value from the ini file
	ret = GetIntFromConfig(ConfigSection(), KParam1, bits);
	CHECK_CONDITION0L(!ret, KErrGeneral, "Failed to read Param1 from ini file.");
	
	ret = GetIntFromConfig(ConfigSection(), KParam2, value);
	CHECK_CONDITION0L(!ret, KErrGeneral, "Failed to read Param2 from ini file.");

    // Initialise the deflate stream
	this->DeflateInit2L(deflateStream, level, method, deflateWindowBits, memLevel, strategy);
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &deflateStream));
	
	// Call deflatePrime on the stream
	err = deflatePrime(&deflateStream, bits, value);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflatePrime error: %d", err);
	
	// Compress the input
	err = this->DeflateCompress(deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflate error: %d", err);
	
	// Clean up the deflate stream
	err = deflateEnd(&deflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "deflateEnd error: %d", err);
	CleanupStack::Pop(1);
	
	// Check the bits at the start of the compressed data are the same as intended
	// when using deflatePrime()
	switch(bits)
	{
	case 16:
		bit1 = value & 0xFF;
		bit2 = (value >> 8) & 0xFF;
		break;
	case 8:
		bit1 = value & 0xFF;
		bit2 = compressedDataBuffer[1];
		break;
	case 0:
		bit1 = compressedDataBuffer[0];
		bit2 = compressedDataBuffer[1];
		break;
	default:
		INFO_PRINTF1(_L("The test only works with bits set to 0, 8 or 16."));
		User::Leave(KErrGeneral);
	}
	CHECK_CONDITION0L(compressedDataBuffer[0] != bit1 || compressedDataBuffer[1] != bit2, KErrGeneral, "The bits at the start of the compressed data buffer do not match the specified bits in deflatePrime.");
	
	CleanupStack::PopAndDestroy(1);		
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4272
@SYMTestCaseDesc     	Check deflatePrime() fails when given an invalid stream.
@SYMTestPriority     	Medium
@SYMTestActions      	1.	Create a stream and initialise it using deflateInit2(), 
                            setting windowBits between -8 and -15. 
                        2.	Call deflatePrime() passing it parameter values:
                            a.	NULL for the stream argument
                            b.	a stream whose state is NULL for the stream argument
@SYMTestExpectedResults deflatePrime() fails returning Z_STREAM_ERROR in both cases.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestDeflatePrimeFailsL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream deflateStream;	 
    
    // deflateInit2 and inflateInit2 arguments
	int level = Z_DEFAULT_COMPRESSION;
	int method = Z_DEFLATED;
	int deflateWindowBits = -MAX_WBITS;
	int memLevel = TEST_MID_MEM_LEVEL;
	int strategy = Z_DEFAULT_STRATEGY;
	
	// deflatePrime arguments
	int bits = 0;
	int value = 0;

    // Initialise the deflate stream
	this->DeflateInit2L(deflateStream, level, method, deflateWindowBits, memLevel, strategy);
	CleanupStack::PushL(TCleanupItem(DeflateEnd, &deflateStream));
	
	// Call deflatePrime on a NULL stream
	err = deflatePrime(NULL, bits, value);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "deflatePrime on a NULL stream returned an unexpected value: %d", err);
	
	// Try calling deflatePrime on a deflate stream that has a NULL state
	// We must keep a pointer to the streams state so we can clean up properly with deflateEnd
	internal_state *deflateState = deflateStream.state;
	deflateStream.state = NULL;	
	
	err = deflatePrime(&deflateStream, bits, value);
	
	deflateStream.state = deflateState;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "deflatePrime on a stream with a NULL state returned an unexpected value: %d", err);
	
	// deflateInit2() leaves a raw stream in a busy state. This results in deflateEnd()
	// returning a Z_DATA_ERROR, unless deflate() has been called on the stream, 
	// meaning we have to set err back to Z_OK for the test to pass
	err = deflateEnd(&deflateStream);
	CHECK_CONDITION1L(err != Z_DATA_ERROR, KErrGeneral, "deflateEnd error: %d", err);
	CleanupStack::Pop(1);
	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4274
@SYMTestCaseDesc     	Check inflatePrime() fails when given an invalid stream or parameters.
@SYMTestPriority     	Medium
@SYMTestActions      	1.	Create an inflate stream and initialise it using inflateInit2(), 
                            setting windowBits to be equal or less than the windowBits value 
                            used for deflateInit2().
                        2.	Call inflatePrime() passing it parameter values:
                            a.	NULL for the stream argument
                            b.	a stream whose state is NULL for the stream argument
                            c.	> 16 for bits
                            d.	(32 – stream->bits) for bits 
@SYMTestExpectedResults inflatePrime() fails returning Z_STREAM_ERROR in all cases.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflatePrimeFailsL()
	{
	int err = Z_OK;
	 
	// Streams
	z_stream inflateStream;
    
    // inflateInit2 argument
	int inflateWindowBits = -MAX_WBITS;
	
	// deflatePrime arguments
	int bits = 0;
	int value = 0;
 
	// Initialise the inflate stream
	this->InflateInit2L(inflateStream, inflateWindowBits);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStream));

	// Call inflatePrime on a NULL stream
	err = inflatePrime(NULL, bits, value);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflatePrime on a NULL stream returned an unexpected value: %d", err);
	
	// Try calling inflatePrime on a deflate stream that has a NULL state
	// We must keep a pointer to the streams state so we can clean up properly with deflateEnd
	struct internal_state FAR *inflateState = inflateStream.state;
	inflateStream.state = NULL;
	
	err = inflatePrime(&inflateStream, bits, value);
	
	inflateStream.state = inflateState;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflatePrime on a stream with a NULL state returned an unexpected value: %d", err);
	
	// Call inflatePrime with bits > 16
	err = inflatePrime(&inflateStream, 17, value);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflatePrime using bits > 16 returned an unexpected value: %d", err);
	
	// Call inflatePrime with inflateStream->state->bits + bits > 32
	// This test cannot be implemented due to inflate_state requiring the definition for
	// the code struct which is in a header file that is included by one of the zlib
	// cpp files.
	/*
	inflateState = inflateStream.state;
	inflateState->bits(33 - bits);
	err = inflatePrime(&inflateStream, bits, value);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflatePrime, setting inflateStream->state->bits, returned an unexpected value: %d", err);
	*/
	
	// Clean up the inflate stream
	err = inflateEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateEnd error: %d", err);
	CleanupStack::Pop(1);
	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4275
@SYMTestCaseDesc     	Check that a stream is duplicated correctly when using inflateCopy().
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a stream and initialise it using deflateInit(). 
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using inflateInit(). 
                        4.	Create a second inflate stream and copy the first stream to the 
                            second stream using inflateCopy(). 
                        5.	Call inflate() and inflateEnd() on both streams. 
                        6.	Compare the output of both streams.
@SYMTestExpectedResults inflateCopy() should return Z_OK and the output of both streams will 
                        be identical.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateCopyL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream deflateStream;	 
	z_stream inflateStream;
	z_stream inflateStreamCopy;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    Byte *decompressedDataBuffer = NULL;
    uLong decompressedDataBufferLength = inputDataLength;
    Byte *copiedDecompressedDataBuffer = NULL;
    uLong copiedDecompressedDataBufferLength = decompressedDataBufferLength;
    
    // deflateInit argument
    StreamSettings streamSettings;
    streamSettings.deflateInit2 = false;
    streamSettings.level = Z_DEFAULT_COMPRESSION;
    
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));

    decompressedDataBuffer = (Byte *)User::AllocZ(decompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(decompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for decompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, decompressedDataBuffer));

    copiedDecompressedDataBuffer = (Byte *)User::AllocZ(copiedDecompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(copiedDecompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for copiedDecompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, copiedDecompressedDataBuffer));
 
    this->CompressDataL(streamSettings, deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);
	
	// Initialise the inflate streams
	this->InflateInitL(inflateStream);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStream));

	// Copy the inflateStream
	err = inflateCopy(&inflateStreamCopy, &inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrNoMemory, "inflateCopy error: %d", err);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStreamCopy));
	
	// Decompress the data in the compressedDataBuffer
	err = this->InflateDecompress(inflateStream, compressedDataBuffer, deflateStream.total_out, decompressedDataBuffer, decompressedDataBufferLength);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflate error: %d", err);
	
	// Decompress the data in the copiedDecompressedDataBuffer
	err = this->InflateDecompress(inflateStreamCopy, compressedDataBuffer, deflateStream.total_out, copiedDecompressedDataBuffer, copiedDecompressedDataBufferLength);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflate error: %d", err);
	
	// Clean up the inflate streams
	err = inflateEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateStream - inflateEnd error: %d", err);
	
	err = inflateEnd(&inflateStreamCopy);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateStreamCopy - inflateEnd error: %d", err);
	CleanupStack::Pop(2);

	// Check that both inflate streams produced identical output
	CHECK_CONDITION0L(memcmp((char *)copiedDecompressedDataBuffer, (char *)decompressedDataBuffer, decompressedDataBufferLength) != 0, KErrGeneral, "The copied stream did not produce the same decompressed output as the original stream.");
	
	CleanupStack::PopAndDestroy(3);		
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4276
@SYMTestCaseDesc     	Check inflateCopy() fails when given an invalid stream or parameters.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create an inflate stream and initialise it using inflateInit(). 
                        2.	Create a second inflate stream and call inflateCopy() passing 
                            it parameter values:
                            a.	NULL for the dest stream argument
                            b.	NULL for the source stream argument
                            c.	a stream whose state is NULL for the source stream argument
                            d.	a stream whose zalloc is Z_NULL for the source stream argument
                            e.	a stream whose zfree is Z_NULL for the source stream argument
@SYMTestExpectedResults inflateCopy() fails returning Z_STREAM_ERROR in all cases.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateCopyFailsParamsL()
	{
	int err = Z_OK;
	 
	// Streams
	z_stream inflateStream;
	z_stream inflateStreamCopy;
	
	// Initialise the inflate streams
	this->InflateInitL(inflateStream);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStream));

	// Try to copy from a NULL stream
	err = inflateCopy(&inflateStreamCopy, NULL);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateCopy from a NULL stream returned an unexpected value: %d", err);
	
	// Try to copy to a NULL stream
	err = inflateCopy(NULL, &inflateStream);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateCopy to a NULL stream returned an unexpected value: %d", err);
	
	// Try calling deflateCopy on a deflate stream that has a NULL state
	// We must keep a pointer to the streams state so we can clean up properly with deflateEnd
	struct internal_state FAR *state = inflateStream.state;
	inflateStream.state = NULL;
	
	err = inflateCopy(&inflateStreamCopy, &inflateStream);
	
	inflateStream.state = state;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateCopy from a stream whose state is NULL returned an unexpected value: %d", err);
	
	// Try to copy from a stream with zalloc set to Z_NULL
	inflateStream.zalloc = Z_NULL;
	err = inflateCopy(&inflateStreamCopy, &inflateStream);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateCopy from a stream with zalloc set to NULL returned an unexpected value: %d", err);
	
	// Try to copy from a stream with zfree set to Z_NULL
	free_func zfree = inflateStream.zfree;
	inflateStream.zfree = Z_NULL;
	
	err = inflateCopy(&inflateStreamCopy, &inflateStream);
	
	inflateStream.zfree = zfree;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "inflateCopy from a stream with zfree set to NULL returned an unexpected value: %d", err);
	
	// Clean up the inflate streams
	err = inflateEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateStream - inflateEnd error: %d", err);
	CleanupStack::Pop(1);
	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4277
@SYMTestCaseDesc     	Check inflateCopy() fails when there is not enough memory to copy 
                        the stream.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create an inflate stream and initialise it using inflateInit(). 
                        2.	Create a second inflate stream and set memory allocation to 
                            fail on the first attempt. 
                        3.	Copy the first stream to the second stream using inflateCopy(). 
                        4.	Check that the memory is the same before and after calling 
                            inflateCopy(). Note: If Z_OK is returned it will be necessary 
                            to call inflateEnd() before checking the amount of memory.
                        5.	Repeat this process until inflateCopy() returns Z_OK, increasing 
                            the number of allocations that can be performed before failing.
@SYMTestExpectedResults inflateCopy() fails at first, returning Z_MEM_ERROR, and then it 
                        will succeed with Z_OK. No memory should be leaked.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateCopyFailsMemL()
	{
	int err;
	TVerdict verdict = EPass;	
	
	// Streams
	z_stream inflateStream;
	z_stream inflateStreamCopy;	
	
	// Initialise the inflate streams
	this->InflateInitL(inflateStream);
	CleanupStack::PushL(TCleanupItem(InflateEnd, &inflateStream));
	
	TInt failureRate;
	for(failureRate = 1, err = Z_MEM_ERROR; err != Z_OK && failureRate <= FAILURE_RATE_LIMIT; failureRate++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, failureRate);
		__UHEAP_MARK;
		
		// Copy the inflateStream
		err = inflateCopy(&inflateStreamCopy, &inflateStream);
		
		// Memory has been allocated so we need to clean up
		if(err == Z_OK)
			{
			err = inflateEnd(&inflateStreamCopy);
			if(err != Z_OK) 
				{ 
			    INFO_PRINTF2(_L("inflateStreamCopy - inflateEnd error: %d"), err);
			    verdict = EAbort;
			    break;
			    }		
			}	
		else if(err != Z_MEM_ERROR)
			{
			INFO_PRINTF2(_L("inflateStreamCopy - unexpected error: %d"), err);
			verdict = EFail;
		    break;
			}
		
		__UHEAP_MARKEND;
		__UHEAP_RESET;		
		}

	if(err == Z_OK)
		{
		INFO_PRINTF2(_L("The test succeeded at heap failure rate = %d."), --failureRate);
		}
	else if(failureRate > FAILURE_RATE_LIMIT)
		{
		INFO_PRINTF1(_L("Exceeded FAILURE_RATE_LIMIT. Either the test has failed or the limit needs increasing."));
		verdict = EFail;
		}	
		
	CleanupStack::PopAndDestroy(1);
	
	return verdict;
	}

/*
 * in function required by inflateBack.
 * Provides a pointer to the next piece of input data and returns the length of the data
 */
unsigned in OF((void FAR *in_desc, unsigned char FAR * FAR *in_buf))
	{	
	struct bufferDescriptor *compressedDataBufferDescriptor = (struct bufferDescriptor *)in_desc;			
	*in_buf = &compressedDataBufferDescriptor->buffer[compressedDataBufferDescriptor->next];
	
	// Check that there is more input
	if(compressedDataBufferDescriptor->next + 1 < compressedDataBufferDescriptor->length)
		{
		compressedDataBufferDescriptor->next++;
		return 1;
		}
	
	return 0;
	}

/*
 * out function required by inflateBack.
 * Provides a pointer to which the next space in memory data can be output and returns the length of this space
 */
int out OF((void FAR *out_desc, unsigned char FAR *out_buf, unsigned len))
	{
	struct bufferDescriptor *decompressedDataBufferDescriptor = (struct bufferDescriptor *)out_desc;
		
	// Make sure there is output space
	if(decompressedDataBufferDescriptor->next + len <= decompressedDataBufferDescriptor->length)
		{	
		memcpy(decompressedDataBufferDescriptor->buffer + decompressedDataBufferDescriptor->next, out_buf, len);
		decompressedDataBufferDescriptor->next += len;	
		}
	else 
		{
		int leftOver = decompressedDataBufferDescriptor->length - decompressedDataBufferDescriptor->next;
		memcpy(decompressedDataBufferDescriptor->buffer + decompressedDataBufferDescriptor->next, out_buf, leftOver);
		}
	
	return 0;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4278
@SYMTestCaseDesc     	Check a stream can be inflated using inflateBackInit(), 
                        inflateBack() and inflateBackEnd().
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it using deflateInit2(), 
                            setting windowBits between -8 and -15. 
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using 
                            inflateBackInit() setting windowBits to be equal to or greater 
                            than the windowBits value used for deflateInit2(). 
                        4.	Call inflateBack() to uncompress the stream and call 
                            inflateBackEnd() to clean up. 
                        5.	Compare the original stream with the uncompressed stream.
@SYMTestExpectedResults inflateBackInit() and inflateBackEnd() should both return Z_OK. 
                        inflateBack() should return Z_STREAM_END when it has finished 
                        inflating the input stream. The original stream should be 
                        identical to the final uncompressed stream.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateBackL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream deflateStream;	 
	z_stream inflateStream;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    Byte *decompressedDataBuffer = NULL;
    uLong decompressedDataBufferLength = inputDataLength;
    
    // deflateInit2 and inflateBackInit arguments
    StreamSettings streamSettings;
    streamSettings.deflateInit2 = true;
    streamSettings.level = Z_DEFAULT_COMPRESSION;
    streamSettings.method = Z_DEFLATED;
    streamSettings.deflateWindowBits = -MAX_WBITS;
    streamSettings.inflateWindowBits = -streamSettings.deflateWindowBits;
    streamSettings.memLevel = TEST_MID_MEM_LEVEL;
    streamSettings.strategy = Z_DEFAULT_STRATEGY;
	Bytef inflateWindow[32 * 1024];
    
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));

    decompressedDataBuffer = (Byte *)User::AllocZ(decompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(decompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for decompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, decompressedDataBuffer));

    this->CompressDataL(streamSettings, deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);
	
	// Initialise the inflate stream
	InflateBackInitL(inflateStream, streamSettings.inflateWindowBits, inflateWindow);
	CleanupStack::PushL(TCleanupItem(InflateBackEnd, &inflateStream));
	
	// This must be set to NULL otherwise inflateBack will assume there is input and try to decompress it
	inflateStream.next_in = NULL;
	
	struct bufferDescriptor compressedDataBufferDescriptor;	
	compressedDataBufferDescriptor.buffer = compressedDataBuffer;
	compressedDataBufferDescriptor.next = 0;
	compressedDataBufferDescriptor.length = compressedDataBufferLength;

	struct bufferDescriptor decompressedDataBufferDescriptor;	
	decompressedDataBufferDescriptor.buffer = decompressedDataBuffer;
	decompressedDataBufferDescriptor.next = 0;
	decompressedDataBufferDescriptor.length = decompressedDataBufferLength;
	
	// Decompress the input
	err = inflateBack(&inflateStream, &in, &compressedDataBufferDescriptor, &out, &decompressedDataBufferDescriptor);
	CHECK_CONDITION1L(err != Z_STREAM_END, KErrGeneral, "inflateBack error: %d", err);
	
	// Clean up the inflate stream
	err = inflateBackEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateBackEnd error: %d", err);
	CleanupStack::Pop(1);
	
	// Check that the decompressed data is identical to the original data
	CHECK_CONDITION0L(strcmp(inputData, (char *)decompressedDataBuffer) != 0, KErrGeneral, "The deflated data did no match the original data.");

	CleanupStack::PopAndDestroy(2);	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4279
@SYMTestCaseDesc     	Check inflateBackEnd() fails when given an invalid stream or parameters.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it using deflateInit2(), 
                            setting windowBits between -8 and -15.
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using 
                            nflateBackInit() setting windowBits to be equal to or greater 
                            than the windowBits value used for deflateInit2(). 
                        4.	Call inflateBack() to uncompress the stream. 
                        5.	Call inflateBackEnd() passing it parameter values:
                            a.	NULL for the stream argument
                            b.	a stream whose state is NULL for the stream argument
                            c.	a stream whose zfree is Z_NULL for the stream argument
@SYMTestExpectedResults inflateBackEnd() fails returning Z_STREAM_ERROR in all cases.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateBackEndFailsL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream deflateStream;	 
	z_stream inflateStream;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    Byte *decompressedDataBuffer = NULL;
    uLong decompressedDataBufferLength = inputDataLength;
    
    // deflateInit2 and inflateBackInit arguments
    StreamSettings streamSettings;
    streamSettings.deflateInit2 = true;
    streamSettings.level = Z_DEFAULT_COMPRESSION;
    streamSettings.method = Z_DEFLATED;
    streamSettings.deflateWindowBits = -MAX_WBITS;
    streamSettings.inflateWindowBits = -streamSettings.deflateWindowBits;
    streamSettings.memLevel = TEST_MID_MEM_LEVEL;
    streamSettings.strategy = Z_DEFAULT_STRATEGY;
	Bytef inflateWindow[32 * 1024];
    
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));

    decompressedDataBuffer = (Byte *)User::AllocZ(decompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(decompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for decompressedDataBuffer."); 
    CleanupStack::PushL(TCleanupItem(User::Free, decompressedDataBuffer));

    this->CompressDataL(streamSettings, deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);
	
	// Initialise the inflate stream
	InflateBackInitL(inflateStream, streamSettings.inflateWindowBits, inflateWindow);
	CleanupStack::PushL(TCleanupItem(InflateBackEnd, &inflateStream));
	
	inflateStream.next_in = (unsigned char *)compressedDataBuffer;
	inflateStream.avail_in = compressedDataBufferLength;
	inflateStream.next_out = (unsigned char *)decompressedDataBuffer;
	inflateStream.avail_out = decompressedDataBufferLength;
	
	struct bufferDescriptor compressedDataBufferDescriptor;	
	compressedDataBufferDescriptor.buffer = compressedDataBuffer;
	compressedDataBufferDescriptor.next = 0;
	compressedDataBufferDescriptor.length = compressedDataBufferLength;

	struct bufferDescriptor decompressedDataBufferDescriptor;	
	decompressedDataBufferDescriptor.buffer = decompressedDataBuffer;
	decompressedDataBufferDescriptor.next = 0;
	decompressedDataBufferDescriptor.length = decompressedDataBufferLength;
	
	// Decompress the input
	err = inflateBack(&inflateStream, &in, &compressedDataBufferDescriptor, &out, &decompressedDataBufferDescriptor);
	CHECK_CONDITION1L(err != Z_STREAM_END, KErrGeneral, "inflateBack error: %d", err);
	
	// Try cleaning up a NULL stream
	err = inflateBackEnd(NULL);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBackEnd on a NULL stream returned an unexpected value: %d", err);
	
	// Keep a pointer to the streams state so we can clean up properly with inflateBackEnd()
	struct internal_state FAR *inflateState = inflateStream.state;

	// Try cleaning up an inflate stream with a NULL state
	inflateStream.state = NULL;
	err = inflateBackEnd(&inflateStream);
	inflateStream.state = inflateState;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBackEnd on a stream with a NULL state returned an unexpected value: %d", err);

	// Try cleaning up an inflate stream with no free function
	free_func zfree = inflateStream.zfree;
	inflateStream.zfree = Z_NULL;
	err = inflateBackEnd(NULL);
	inflateStream.zfree = zfree;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBackEnd on a stream with zfree set to NULL returned an unexpected value: %d", err);
	
	// Clean up the inflate stream
	err = inflateBackEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateBackEnd error: %d", err);
	CleanupStack::Pop(1);
	
	CleanupStack::PopAndDestroy(2);	
	return EPass;
	}

/*
 * in function required by inflateBack.
 * Returning 0 indicates failure.
 */
unsigned inNoInput OF((void FAR *, unsigned char FAR * FAR *))
	{
	return 0;
	}

/*
 * out function required by inflateBack.
 * Returning non 0 indicates failure.
 */
int outNoOutput OF((void FAR *, unsigned char FAR *, unsigned))
	{
	return 1;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4280
@SYMTestCaseDesc     	Check inflateBack() fails when given an invalid stream or parameters.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create a deflate stream and initialise it using deflateInit2(), 
                            setting windowBits between -8 and-15. 
                        2.	Deflate the input and cleanup using deflateEnd(). 
                        3.	Create an inflate stream and initialise it using 
                            inflateBackInit() setting windowBits to be equal to or greater 
                            than the windowBits value used for deflateInit2(). 
                        4.	Call inflateBack() passing it parameter values:
                            a.	NULL for the stream argument
                            b.	a stream whose state is NULL for the stream argument
                            c.	an input function that returns 0 for the in argument
                            d.	an output function that doesn’t return 0 for the out argument
                            e.	corrupted deflate output
                        5.	Call inflateBackEnd() to clean up
@SYMTestExpectedResults inflateBack() fails returning Z_STREAM_ERROR for the first two 
                        cases, Z_BUF_ERROR for the second two cases and Z_DATA_ERROR 
                        for the final case.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateBackFailsL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream deflateStream;	 
	z_stream inflateStream;
	
	// Buffers
	const char inputData[] = "This is a piece of data to compress.\0";
    uLong inputDataLength = (uLong)strlen(inputData) + 1;     
    Byte *compressedDataBuffer = NULL;
    uLong compressedDataBufferLength = COMPRESSED_DATA_BUFFER_LENGTH;
    Byte *decompressedDataBuffer = NULL;
    uLong decompressedDataBufferLength = inputDataLength;
    
    // deflateInit2 and inflateBackInit arguments
    StreamSettings streamSettings;
    streamSettings.deflateInit2 = true;
    streamSettings.level = Z_DEFAULT_COMPRESSION;
    streamSettings.method = Z_DEFLATED;
    streamSettings.deflateWindowBits = -MAX_WBITS;
    streamSettings.inflateWindowBits = -streamSettings.deflateWindowBits; // inflateBackInit expects the positive equivalent of the windowBits used to make a raw stream
    streamSettings.memLevel = TEST_MID_MEM_LEVEL;
    streamSettings.strategy = Z_DEFAULT_STRATEGY;
	Bytef inflateWindow[32 * 1024];
    
	// Allocate memory for buffers
    compressedDataBuffer = (Byte *)User::AllocZ(compressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(compressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for compressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, compressedDataBuffer));

    decompressedDataBuffer = (Byte *)User::AllocZ(decompressedDataBufferLength * sizeof(Byte));
    CHECK_CONDITION0L(decompressedDataBuffer == NULL, KErrNoMemory, "Failed to allocate memory for decompressedDataBuffer.");
    CleanupStack::PushL(TCleanupItem(User::Free, decompressedDataBuffer));
    
    this->CompressDataL(streamSettings, deflateStream, (Byte *)inputData, inputDataLength, compressedDataBuffer, compressedDataBufferLength);
	
	// Initialise the inflate stream
	InflateBackInitL(inflateStream, streamSettings.inflateWindowBits, inflateWindow);
	CleanupStack::PushL(TCleanupItem(InflateBackEnd, &inflateStream));
	
	// This must be set to NULL otherwise inflateBack will assume there is input and try to decompress it
	inflateStream.next_in = NULL;
	
	struct bufferDescriptor compressedDataBufferDescriptor;	
	compressedDataBufferDescriptor.buffer = compressedDataBuffer;
	compressedDataBufferDescriptor.next = 0;
	compressedDataBufferDescriptor.length = compressedDataBufferLength;

	struct bufferDescriptor decompressedDataBufferDescriptor;	
	decompressedDataBufferDescriptor.buffer = decompressedDataBuffer;
	decompressedDataBufferDescriptor.next = 0;
	decompressedDataBufferDescriptor.length = decompressedDataBufferLength;
	
	// Try inflating a NULL stream
	err = inflateBack(NULL, &in, &compressedDataBufferDescriptor, &out, &decompressedDataBufferDescriptor);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBack on a NULL stream returned an unexpected value: %d", err);
	
	// Try calling inflateBack on a deflate stream that has a NULL state
	// We must keep a pointer to the streams state so we can clean up properly with deflateBackEnd
	struct internal_state FAR *inflateState = inflateStream.state;
	inflateStream.state = NULL;
	
	err = inflateBack(&inflateStream, &in, &compressedDataBufferDescriptor, &out, &decompressedDataBufferDescriptor);
	
	inflateStream.state = inflateState;
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBack on a stream with a NULL state returned an unexpected value: %d", err);
	
	// Try inflating a stream with an in_func that returns no input data
	err = inflateBack(&inflateStream, &inNoInput, &compressedDataBufferDescriptor, &out, &decompressedDataBufferDescriptor);
	CHECK_CONDITION1L(err != Z_BUF_ERROR, KErrGeneral, "Calling inflateBack with an in function that returns no input returned an unexpected value: %d", err);
	
	// Try inflating a stream with an out_func that does not output any data
	err = inflateBack(&inflateStream, &in, &compressedDataBufferDescriptor, &outNoOutput, &decompressedDataBufferDescriptor);
	CHECK_CONDITION1L(err != Z_BUF_ERROR, KErrGeneral, "Calling inflateBack with an in function that returns no input returned an unexpected value: %d", err);
	
	// Try inflating a corrupt stream
	compressedDataBuffer[1] = 'a';
	compressedDataBuffer[2] = 'a';
	compressedDataBuffer[3] = 'a';
	err = inflateBack(&inflateStream, &in, &compressedDataBufferDescriptor, &out, &decompressedDataBufferDescriptor);
	CHECK_CONDITION1L(err != Z_DATA_ERROR, KErrGeneral, "Calling inflateBack on a corrupt stream returned an unexpected value: %d", err);
	
	// Clean up the inflate stream
	err = inflateBackEnd(&inflateStream);
	CHECK_CONDITION1L(err != Z_OK, KErrGeneral, "inflateBackEnd error: %d", err);
	CleanupStack::Pop(1);
	
	CleanupStack::PopAndDestroy(2);	
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4281
@SYMTestCaseDesc     	Check inflateBackInit() fails when given an invalid stream or parameters.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create an inflate stream and initialise it using 
                            inflateBackInit() passing it parameter values:
                            a.	NULL for the stream argument
                            b.	a stream whose window is NULL for the stream argument
                            c.	< 8 for the windowBits argument
                            d.	> 5 for the windowBits argument
@SYMTestExpectedResults inflateBackInit() should fail returning Z_STREAM_ERROR for all cases.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateBackInitFailsParamsL()
	{
	int err = Z_OK;
	
	// Streams
	z_stream inflateStream;
    
    // inflateBackInit arguments
	int inflateBackWindowBits = MAX_WBITS;
	Bytef inflateWindow[32 * 1024];
 
	// Try initialising a NULL stream
	inflateStream.zalloc = Z_NULL;
	inflateStream.zfree = Z_NULL;
	inflateStream.opaque = Z_NULL;
	
	err = inflateBackInit(NULL, inflateBackWindowBits, inflateWindow);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBackInit on a NULL stream returned an unexpected value: %d", err);
	
	// Try initialising a stream with a NULL inflate window
	err = inflateBackInit(&inflateStream, inflateBackWindowBits, NULL);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBackInit on a stream with a NULL window returned an unexpected value: %d", err);
	
	// Try initialising a stream with window bits < 8
	err = inflateBackInit(&inflateStream, 7, inflateWindow);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBackInit on a stream with windowBits < 8 returned an unexpected value: %d", err);
	
	// Try initialising a stream with window bits > 15
	err = inflateBackInit(&inflateStream, 16, inflateWindow);
	CHECK_CONDITION1L(err != Z_STREAM_ERROR, KErrGeneral, "Calling inflateBackInit on a stream with windowBits > 15 returned an unexpected value: %d", err);
		
	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4282
@SYMTestCaseDesc     	Check inflateBackInit() fails when there is not enough memory.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create an inflate stream and set the next memory allocation to fail. 
                        2.	Initialise the stream using inflateBackInit() setting 
                            windowBits to be between 8 and 15. 
                        3.	Check that the memory is the same before and after calling 
                            inflateBackInit(). Note: If Z_OK is returned it will be 
                            necessary to call inflateEnd() before checking the amount of memory.
                        4.	Repeat this process until inflateBackInit() returns Z_OK, 
                            increasing the number of allocations that can be performed before failing.
@SYMTestExpectedResults inflateBackInit () fails at first, returning Z_MEM_ERROR, and then 
                        it will succeed with Z_OK. No memory should be leaked.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestInflateBackInitFailsMem()
	{
	TInt err;
	TVerdict verdict = EPass;
	
	// Streams
	z_stream inflateStream;

    // inflateBackInit arguments
	int inflateBackWindowBits = MAX_WBITS;
	Bytef inflateWindow[32 * 1024];

	inflateStream.zalloc = Z_NULL;
	inflateStream.zfree = Z_NULL;
	inflateStream.opaque = Z_NULL;
		
	TInt failureRate;
	for(failureRate = 1, err = Z_MEM_ERROR; err != Z_OK && failureRate <= FAILURE_RATE_LIMIT; failureRate++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, failureRate);
		__UHEAP_MARK;
	
		err = inflateBackInit(&inflateStream, inflateBackWindowBits, inflateWindow);
		
		// Memory has been allocated so we need to clean up
		if(err == Z_OK)
			{
			err = inflateBackEnd(&inflateStream);
			if(err != Z_OK) 
				{ 
				INFO_PRINTF2(_L("inflateBackInit error: %d"), err); 
			    verdict = EAbort;
			    break;
			    }		
			}	
		else if(err != Z_MEM_ERROR)
			{
			INFO_PRINTF2(_L("inflateBackInit unexpected error: %d"), err);
			verdict = EFail;
		    break;
			}
		
		__UHEAP_MARKEND;
		__UHEAP_RESET;		
		}
	
	if(err == Z_OK)
		{
		INFO_PRINTF2(_L("The test succeeded at heap failure rate = %d."), --failureRate);
		}
	else if(failureRate > FAILURE_RATE_LIMIT)
		{
		INFO_PRINTF1(_L("Exceeded FAILURE_RATE_LIMIT. Either the test has failed or the limit needs increasing."));
		verdict = EFail;
		}

	return verdict;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4283
@SYMTestCaseDesc     	Check adler32_combine() generates the correct adler32 checksum.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create two byte buffers, both containing different data, and 
                            use these to create two checksums using adler32(). 
                        2.	Create a third checksum using adler32_combine(), providing it 
                            with the first two checksums and the length of the second checksum. 
                        3.	Concatenate the two byte buffers and create a fourth checksum 
                            using adler32() and the concatenated buffer as input. 
                        4.	Compare the third and fourth checksums.
@SYMTestExpectedResults The third and fourth checksums should be identical.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestAdler32CombineL()
	{
	// Byte buffers from which the adler32 checksums will be generated
	const Byte *string1 = (Byte *)"Hello";
	const Byte *string2 = (Byte *)"World";
	const int string1Length = 5;	
	const int string2Length = 5;	
    const int string3Length = 10;	
	
	// Initialise the adler32 variables
	uLong adlerString1 = adler32(0L, NULL, 0);
	uLong adlerString2 = adler32(0L, NULL, 0);
	uLong adlerString3 = adler32(0L, NULL, 0);
	uLong adlerCombined;
	
	// Generate the checksums from the byte buffers
	adlerString1 = adler32(adlerString1, string1, string1Length);
	adlerString2 = adler32(adlerString2, string2, string2Length);
	
	// Generate the checksum from combining adlerString1 and adlerString2
	adlerCombined = adler32_combine(adlerString1, adlerString2, string2Length);
	
	// Concatenate the byte buffers so that a checksum can be generated
	Byte string3[string3Length];
	memcpy((char *)string3, (char *)string1, string1Length);            
	memcpy((char *)string3 + string1Length, (char *)string2, string2Length);
	
	// Generate checksum
	adlerString3 = adler32(adlerString3, string3, string3Length);

	// Compare the checksums to see if they are the same
	CHECK_CONDITION0L(adlerString3 != adlerCombined, KErrGeneral, "The combined checksum is not correct.");

	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4284
@SYMTestCaseDesc     	Check crc32_combine() generates the correct crc32 checksum.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create two byte buffers, both containing different data, and 
                            use these to create two checksums using crc32(). 
                        2.	Create a third checksum using crc32_combine(), providing it 
                            with the first two checksums and the length of the second checksum. 
                        3.	Concatenate the two byte buffers and create a fourth checksum 
                            using crc32() and the concatenated buffer as input. 
                        4.	Compare the third and fourth checksums.
@SYMTestExpectedResults The third and fourth checksums should be identical.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestCrc32CombineL()
	{
	// Byte buffers from which the crc32 checksums will be generated
	const Byte *string1 = (Byte *)"Hello";
	const Byte *string2 = (Byte *)"World";
	const int string1Length = 5;	
	const int string2Length = 5;	
    const int string3Length = 10;	
	
	// Initialise the crc32 variables
	uLong crcString1 = crc32(0L, NULL, 0);
	uLong crcString2 = crc32(0L, NULL, 0);
	uLong crcString3 = crc32(0L, NULL, 0);
	uLong crcCombined;
	
	// Generate the checksums from the byte buffers
	crcString1 = crc32(crcString1, string1, string1Length);
	crcString2 = crc32(crcString2, string2, string2Length);
	
	// Generate the checksum from combining adlerString1 and adlerString2
	crcCombined = crc32_combine(crcString1, crcString2, string2Length);
	
	// Concatenate the byte buffers so that a checksum can be generated
	Byte string3[string3Length];
	memcpy((char *)string3, (char *)string1, string1Length);            
	memcpy((char *)string3 + string1Length, (char *)string2, string2Length);
	
	// Generate checksum
	crcString3 = crc32(crcString3, string3, string3Length);

	// Compare the checksums to see if they are the same
	CHECK_CONDITION0L(crcString3 != crcCombined, KErrGeneral, "The combined checksum is not correct.");

	return EPass;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4284
@SYMTestCaseDesc     	Check zlibCompileFlags() returns a uLong with the correct 
                        compile flags set.
@SYMTestPriority     	Medium
@SYMTestActions      	1.	Call zlibCompileFlags() and compare the first 2 bits with the 
                            size of uInt, the second 2 bits with the size of uLong and 
                            bits 16 and 17 with 0 (GZip APIs are being used).
@SYMTestExpectedResults zlibCompileFlags() will return a uLong and all the comparisons 
                        will return true.
@SYMDEF                 REQ7362
*/

TVerdict CTestZlib::TestZlibCompileFlagsL()
	{
	// Get the compilerFlags
	uLong compileFlags = zlibCompileFlags();
	int compileFlagsUInt = compileFlags & 0x3;
	int compileFlagsULong = (compileFlags >> 2) & 0x3;
	int compileFlagsGZip = (compileFlags >> 15) & 0x3;
	int machineUInt;
	int machineULong;
	
	// Get the size of uInt
	switch(sizeof(uInt))
		{
		case 2: machineUInt = 0;
				break;
		case 4: machineUInt = 1;
				 break;
		case 8: machineUInt = 2;
				 break;
		default: machineUInt = 3;
		}

	// Check the compiler flag for uInt is correct
	CHECK_CONDITION0L(machineUInt != compileFlagsUInt, KErrGeneral, "zlibCompileFlags reports an incorrect size for uInt.");
	
	// Get the size of uLong
	switch(sizeof(uLong))
		{
		case 2: machineULong = 0;
				break;
		case 4: machineULong = 1;
				 break;
		case 8: machineULong = 2;
				 break;
		default: machineULong = 3;
		}

	// Check the compiler flag for uLong is correct
	CHECK_CONDITION0L(machineULong != compileFlagsULong, KErrGeneral, "zlibCompileFlags reports an incorrect size for uLong.");
	
	// Check the compiler flags for GZip compression are correct
	CHECK_CONDITION0L(compileFlagsGZip != 0, KErrGeneral, "zlibCompileFlags reports GZip functionality is disabled.");

	return EPass;
	}
