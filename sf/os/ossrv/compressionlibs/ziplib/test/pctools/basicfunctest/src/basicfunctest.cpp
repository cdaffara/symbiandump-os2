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
//

#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <zlib.h>

using namespace std;

const unsigned int bufferSize = 1024;
const string filePath = "..\\..\\ezlib2_testdata\\";

/*
 * deflateInflateTest() constants
 */
const int deflateInflateTestDataItems = 24;
const string deflateInflateTestFiles[deflateInflateTestDataItems] = {"GifImage.gif", "BigSize.txt", "BitmapImage.bmp", 
                                                                      "BigSize.txt", "JpegImage.jpg", "BigSize.txt",
                                                                      "SmallSize.txt", "BigSize.txt", "HolyMoley.jar",
                                                                      "BigSize.txt", "ExifHeadFile.png", "BigSize.txt",
                                                                      "GifImage.gif", "BigSize.doc", "BitmapImage.bmp",
                                                                      "BigSize.doc", "JpegImage.jpg", "BigSize.doc",
                                                                      "SmallSize.txt", "BigSize.doc", "HolyMoley.jar",
                                                                      "BigSize.doc", "ExifHeadFile.png", "BigSize.doc"};

const int deflateInflateTestLevel[deflateInflateTestDataItems] = {Z_DEFAULT_COMPRESSION, Z_NO_COMPRESSION, Z_BEST_COMPRESSION,
                                                                   Z_DEFAULT_COMPRESSION, Z_NO_COMPRESSION, Z_BEST_COMPRESSION,
                                                                   Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION, Z_NO_COMPRESSION,
                                                                   Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION, Z_NO_COMPRESSION,
                                                                   Z_NO_COMPRESSION, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION,
                                                                   Z_NO_COMPRESSION, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION,
                                                                   Z_BEST_SPEED, Z_BEST_SPEED, Z_BEST_SPEED,
                                                                   Z_BEST_SPEED, Z_BEST_SPEED, Z_BEST_SPEED};

const int deflateInflateTestWindowBits[deflateInflateTestDataItems] = {8, MAX_WBITS, 8,
                                                                        MAX_WBITS, 8, MAX_WBITS,
                                                                        8, 8, MAX_WBITS,
                                                                        MAX_WBITS, MAX_WBITS, 8,
                                                                        MAX_WBITS, 8, 8,
                                                                        8, MAX_WBITS, MAX_WBITS,
                                                                        8, MAX_WBITS, 8,
                                                                        MAX_WBITS, MAX_WBITS, 8};

const int deflateInflateTestMemLevel[deflateInflateTestDataItems] = {1, 1, 1,
                                                                      1, 1, 1,
                                                                      MAX_MEM_LEVEL, MAX_MEM_LEVEL, MAX_MEM_LEVEL,
                                                                      MAX_MEM_LEVEL, MAX_MEM_LEVEL, MAX_MEM_LEVEL,
                                                                      8, 8, 8,
                                                                      8, 8, 8,
                                                                      1, MAX_MEM_LEVEL, 8,
                                                                      1, MAX_MEM_LEVEL, 8};

const int deflateInflateTestStrategy[deflateInflateTestDataItems] = {Z_DEFAULT_STRATEGY, Z_DEFAULT_STRATEGY, Z_FILTERED,
                                                                      Z_FILTERED, Z_HUFFMAN_ONLY, Z_HUFFMAN_ONLY,
                                                                      Z_DEFAULT_STRATEGY, Z_DEFAULT_STRATEGY, Z_FILTERED,
                                                                      Z_FILTERED, Z_HUFFMAN_ONLY, Z_HUFFMAN_ONLY,
                                                                      Z_DEFAULT_STRATEGY, Z_DEFAULT_STRATEGY, Z_FILTERED,
                                                                      Z_FILTERED, Z_HUFFMAN_ONLY, Z_HUFFMAN_ONLY,
                                                                      Z_DEFAULT_STRATEGY, Z_DEFAULT_STRATEGY, Z_FILTERED,
                                                                      Z_FILTERED, Z_HUFFMAN_ONLY, Z_HUFFMAN_ONLY};

/*
 * inflateOldZlibFilesTest() constants
 */
const int inflateOldZlibFilesTestDataItems = 24;
const string inflateOldZlibFilesTestFiles[inflateOldZlibFilesTestDataItems] = {"GifImage.gif", "BigSize.txt", "BitmapImage.bmp", 
                                                                                "BigSize.txt", "JpegImage.jpg", "BigSize.txt",
                                                                                "SmallSize.txt", "BigSize.txt", "HolyMoley.jar",
                                                                                "BigSize.txt", "ExifHeadFile.png", "BigSize.txt",
                                                                                "GifImage.gif", "BigSize.doc", "BitmapImage.bmp",
                                                                                "BigSize.doc", "JpegImage.jpg", "BigSize.doc",
                                                                                "SmallSize.txt", "BigSize.doc", "HolyMoley.jar",
                                                                                "BigSize.doc", "ExifHeadFile.png", "BigSize.doc"};

/*
 * Global variables
 */
ofstream *outputFile = NULL;
int totalPassed = 0;

/*
 * HELPER FUNCTION DECLARATIONS
 */
void closeOutputFile();
void deleteFile(const string fileLocation);
int deflateCompress(z_stream &aStream, ifstream &input, ofstream &output);
bool deflateFile(const string &aInputFileName, const string &aOutputFileName, int aLevel, int aMethod, int aWindowBits, int aMemLevel, int aStrategy);
bool doFilesMatch(const string &aFileName1, const string &aFileName2);
unsigned int getFileLength(ifstream &file);
int inflateDecompress(z_stream &aStream, ifstream &input, ofstream &output);
bool inflateFile(const string &aInputFileName, const string &aOutputFileName, int aWindowBits = 15);
bool openOutputFile(const string &aOutputFileName);
void printTestResults();

/*
 * TEST FUNCTION DECLARATIONS
 */
bool deflateInflateTest();
bool inflateOldZlibFilesTest();


/*
 * Closes the results output file and frees the memory.
 */
void closeOutputFile()
    {
    *outputFile << "\t</body>" << endl;
    *outputFile << "</html>" << endl;
    outputFile->close();
    
    delete outputFile;
    }

void deleteFile(const string fileLocation)
    {
    int err = remove(fileLocation.c_str());
    if(err != 0)
        {
        *outputFile << "\t\t" << err << " - Error deleting file: " << fileLocation << "<br />" << endl;
        cout << "Error deleting file: " << fileLocation << endl;
        }
    }

/*
 * Compresses data using the deflate function.
 */
int deflateCompress(z_stream &aStream, ifstream &input, ofstream &output)
    {
    int err = Z_OK;
    int flush = Z_NO_FLUSH;
    unsigned char in[bufferSize];
    unsigned char out[bufferSize];
    unsigned int fileSize = getFileLength(input);

    do
        {
        input.read(reinterpret_cast<char *>(in), bufferSize);

        aStream.avail_in = ((aStream.total_in + bufferSize) > fileSize) ? fileSize - aStream.total_in : bufferSize;
        aStream.next_in = in;

        flush = input.eof() ? Z_FINISH : Z_NO_FLUSH;

        // Call deflate() on input until output buffer not full
        // Finish compression if all of input buffer has been read in
        do
            {
            aStream.avail_out = bufferSize;
            aStream.next_out = out;

            err = deflate(&aStream, flush);
            if(err != Z_OK && err != Z_STREAM_END)
            {
                return err;
            }
            output.write(reinterpret_cast<char *>(out), bufferSize - aStream.avail_out);

            } while(aStream.avail_out == 0 && err == Z_OK);
        } while(err != Z_STREAM_END);

    return Z_OK;
    }

/*
 * Reads in the data from the input file, compresses it and creates a new output file for the compressed data
 */
bool deflateFile(const string &aInputFileName, const string &aOutputFileName, int aLevel, int aMethod, int aWindowBits, int aMemLevel, int aStrategy)
    {
    int err = Z_OK;

    ifstream input(aInputFileName.c_str(), ios::in | ios::binary);
    ofstream output(aOutputFileName.c_str(), ios::out | ios::trunc | ios::binary);
    z_stream stream;

    if(input.is_open() && output.is_open())
        {
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;

        deflateInit2(&stream, aLevel, aMethod, aWindowBits, aMemLevel, aStrategy);
        err = deflateCompress(stream, input, output);
        deflateEnd(&stream);

        if(err != Z_OK)
            {
            *outputFile << "\t\t" << err << " - Error deflating!<br />" << endl;
            output.close();
            input.close();
            return false;
            }
        }
    else
        {
        if(!input.is_open())
            {
            *outputFile << "\t\tDeflate could not complete due to not being able to open the input file!<br />" << endl;
            }
        else
            {
            input.close();
            }

        if(!output.is_open())
            {
            *outputFile << "\t\tDeflate could not complete due to not being able to open the output file!<br />" << endl;
            }
        else
            {
            output.close();
            }

        return false;
        }

    input.close();
    output.close();

    *outputFile << "\t\tDeflate complete!<br />" << endl;
    return true;
    }

/*
 * Checks that the two files are identical
 */
bool doFilesMatch(const string &aFileName1, const string &aFileName2)
    {
    ifstream file1(aFileName1.c_str(), ios::in | ios::binary);
    ifstream file2(aFileName2.c_str(), ios::in | ios::binary);

    if(file1.is_open() && file2.is_open())
        {
        int file1Size = getFileLength(file1);
        int file2Size = getFileLength(file2);

        if(file1Size != file2Size)
            {
            *outputFile << "\t\t" << aFileName1 << " is not the same as " << aFileName2 << " because they have different file sizes!<br />" << endl;
            return false;
            }
        else
            {
            char fileBuffer1[bufferSize];
            char fileBuffer2[bufferSize];

            for(int totalSpaceProvided = bufferSize; !file1.eof(); totalSpaceProvided += bufferSize)
                {
                file1.read(fileBuffer1, bufferSize);
                file2.read(fileBuffer2, bufferSize);

                int read = (totalSpaceProvided > file1Size) ? bufferSize - (totalSpaceProvided - file1Size) : bufferSize;
                if(memcmp(fileBuffer1, fileBuffer2, read) != 0)
                    {
                    *outputFile << "\t\t" << aFileName1 << " is not the same as " << aFileName2 << "<br />" << endl;
                    return false;
                    }
                }
            }
        }
    else
        {
        if(!file1.is_open())
            {
            *outputFile << "\t\tCould not check if files matched because " << aFileName1 << " could not be opened!<br />" << endl;
            }
        else
            {
            file1.close();
            }

        if(!file2.is_open())
            {
            *outputFile << "\t\tCould not check if files matched because " << aFileName2 << " could not be opened!<br />" << endl;
            }
        else
            {
            file2.close();
            }

        return false;
        }

    file1.close();
    file2.close();

    return true;
    }

/*
 * Returns a files size in bytes
 */
unsigned int getFileLength(ifstream &file)
    {
    file.seekg (0, ios::end);
    unsigned int fileSize = file.tellg();
    file.seekg (0, ios::beg);

    return fileSize;
    }

/*
 * Decompresses data using the inflate function.
 */
int inflateDecompress(z_stream &aStream, ifstream &input, ofstream &output)
    {
    int err = Z_OK;
    unsigned char in[bufferSize];
    unsigned char out[bufferSize];
    unsigned int fileSize = getFileLength(input);

    // Keep providing input data and output space for deflate()
    do
        {
        input.read(reinterpret_cast<char *>(in), bufferSize);

        aStream.avail_in = ((aStream.total_in + bufferSize) > fileSize) ? fileSize - aStream.total_in : bufferSize;
        aStream.next_in = in;

        // Call inflate() on input until output buffer not full
        // Finish compression if all of input buffer has been read in
        do
            {
            aStream.avail_out = bufferSize;
            aStream.next_out = out;

            err = inflate(&aStream, Z_NO_FLUSH);
            if(err != Z_OK && err != Z_STREAM_END && err != Z_BUF_ERROR)
                {
                return err;
                }
            output.write(reinterpret_cast<char *>(out), bufferSize - aStream.avail_out);

            } while(aStream.avail_out == 0);
        } while(err != Z_STREAM_END);

    return Z_OK;
    }

/*
 * Reads in the data from the input file, decompresses it and creates a new output file for the decompressed data
 *
 * NOTE: Inflate fails when windowBits is set to 8. This is because deflateInit2() changes windowBits from 8 to 9
 * due to an error in zlib. However, inflateInit2() does not make the same change and this results in the
 * inflate failing with a Z_DATA_ERROR.
 */
bool inflateFile(const string &aInputFileName, const string &aOutputFileName, int aWindowBits)
    {
    int err = Z_OK;
    
    ifstream input(aInputFileName.c_str(), ios::in | ios::binary);
    ofstream output(aOutputFileName.c_str(), ios::out | ios::binary | ios::trunc);
    z_stream stream;

    if(input.is_open() && output.is_open())
        {
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;

        inflateInit2(&stream, aWindowBits);
        err = inflateDecompress(stream, input, output);
        inflateEnd(&stream);

        if(err != Z_OK)
            {
            *outputFile << "\t\t" << err << " - Error inflating!<br />" << endl;
            output.close();
            input.close();
            return false;
            }
        }
    else
        {
        if(!input.is_open())
            {
            *outputFile << "\t\tInflate could not complete due to not being able to open the input file!<br />" << endl;
            }
        else
            {
            input.close();
            }

        if(!output.is_open())
            {
            *outputFile << "\t\tInflate could not complete due to not being able to open the output file!<br />" << endl;
            }
        else
            {
            output.close();
            }

        return false;
        }

    output.close();
    input.close();

    *outputFile << "\t\tInflate complete!<br />" << endl;
    return true;
    }

/*
 * Creates and opens the results output file.
 */
bool openOutputFile(const string &aOutputFileName)
    {
    outputFile = new ofstream((aOutputFileName + ".html").c_str(), ios::out | ios::trunc);
    
    if(outputFile != NULL)
        {
        *outputFile << "<html>" << endl;
        *outputFile << "\t<body>" << endl;
        
        return true;
        }

    return false;
    }

/*
 * Outputs the test results to the results output file.
 */
void printTestResults()
    {
    int totalFailed = deflateInflateTestDataItems + inflateOldZlibFilesTestDataItems - totalPassed;

    *outputFile << "\t\tTEST RESULTS:<br />" << endl;
    *outputFile << "\t\t<font color=00AF00>Passed = " << totalPassed << "</font><br />" << endl;
    *outputFile << "\t\t<font color=FF0000>Failed = " << totalFailed << "</font><br />" << endl;
    }

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4312
@SYMTestCaseDesc     	Check files can be deflated and inflated.
@SYMTestPriority     	High
@SYMTestActions      	1.	Open the input file for reading and create a compressed output 
                            file for writing. 
                        2.	Create a deflate stream and initialise it using deflateInit2() 
                            passing it the specified compression parameters.
                        3.	Deflate the input using deflate(), writing the deflated data 
                            to the compressed output file.
                        4.	Cleanup the deflate stream and close the input and compressed 
                            output files.
                        5.	Open the compressed file for reading and create a decompressed 
                            output file for writing
                        6.	Create an inflate stream and initialise it using inflateInit2().
                        7.	Inflate the input using inflate(), writing the inflated data to 
                            the decompressed output file.
                        8.	Cleanup the inflate stream and close the compressed input and 
                            decompressed output files.
                        9.	Open the original file (that was compressed) and the 
                            decompressed file for reading.
                        10.	Compare the contents of each file using memcmp().
                        11.	Close both the files.
                        12.	Cleanup any files created during the test.
                        
                        Note: The test should be repeated for different types of input 
                              files as well as different size of input file e.g.:
                            •	TXT
                            •	PNG
                            •	JAR
                            •	JPG
                            •	BMP
                            •	DOC
                            •	GIF
                            •	Varying sizes of each input file should be tested starting 
                                from a couple of kilobytes up to several megabytes.
                            
                        And with compression parameters as:
                            •	Z_DEFAULT_COMPRESSION, Z_NO_COMPRESSION, Z_BEST_COMPRESSION 
                                and Z_BEST_SPEED for level 
                            •	8 and MAX_WBITS for windowBits
                            •	1, 8 (default memory level) and MAX_MEM_LEVEL for memLevel
                            •	Z_DEFAULT_STRATEGY, Z_FILTERED and Z_HUFFMAN_ONLY for strategy
@SYMTestExpectedResults The input file used for deflating should be identical to the 
                        inflated output file.
*/
bool deflateInflateTest()
    {
    bool passed = true;

    cout << endl << "Starting test case deflateInflateTest:" << endl;
    for(int i = 0; i < deflateInflateTestDataItems ; i++)
        {
        stringstream ss;
        string testNum;
        ss << (i + 1);
        ss >> testNum;

        cout << "Starting test " << testNum << " - File: " << deflateInflateTestFiles[i];

        int extBegin = deflateInflateTestFiles[i].find_last_of(".", deflateInflateTestFiles[i].length());
        string fileName = deflateInflateTestFiles[i].substr(0, extBegin);
        string fileExt = deflateInflateTestFiles[i].substr(extBegin + 1);
        
        string uncompressedFileName(filePath + deflateInflateTestFiles[i]);
        string compressedFileName(filePath + fileName + testNum + ".zip");
        string decompressedFileName(filePath + "ezlib2_" + fileName + testNum + "_decompressed" + "." + fileExt);

        if(deflateFile(uncompressedFileName, compressedFileName, deflateInflateTestLevel[i], Z_DEFLATED, deflateInflateTestWindowBits[i], deflateInflateTestMemLevel[i], deflateInflateTestStrategy[i]) == false)
            {
            passed = false;
            *outputFile << "\t\t<font color=FF0000>FAILED for file: " << deflateInflateTestFiles[i] << "</font><p />" << endl << endl;
            cout << " - FAILED" << endl;
            continue;
            }

        if(inflateFile(compressedFileName, decompressedFileName, deflateInflateTestWindowBits[i]) == false)
            {
            passed = false;
            *outputFile << "\t\t<font color=FF0000>FAILED for file: " << uncompressedFileName << "</font><p />" << endl << endl;
            cout << " - FAILED" << endl;
            
            deleteFile(compressedFileName);
            deleteFile(decompressedFileName);
            continue;
            }

        if(doFilesMatch(uncompressedFileName, decompressedFileName) == false)
            {
            passed = false;
            *outputFile << "\t\t<font color=FF0000>FAILED for file: " << uncompressedFileName << "</font><p />" << endl << endl;
            cout << " - FAILED" << endl;
            
            deleteFile(compressedFileName);
            deleteFile(decompressedFileName);
            continue;
            }

        totalPassed++;
        *outputFile << "\t\t<font color=00AF00>PASSED for file: " << uncompressedFileName << "</font><p />" << endl << endl;
        cout << " - PASSED" << endl;
        
        deleteFile(compressedFileName);
        deleteFile(decompressedFileName);
        }
    cout << "Finished test case case deflateInflateTest." << endl;
    
    return passed;
    }

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4313
@SYMTestCaseDesc     	Check files deflated with EZlib can be inflated using EZlib2.
@SYMTestPriority     	High
@SYMTestActions      	1.	Open the compressed input file, which was compressed using EZlib, 
                            for reading and create a decompressed output file for writing
                        2.	Create an inflate stream and initialise it using inflateInit2().
                        3.	Inflate the input using inflate(), writing the inflated data 
                            to the decompressed output file.
                        4.	Cleanup the inflate stream and close the compressed input and 
                            decompressed output files.
                        5.	Open the compressed input file and the decompressed output 
                            file for reading.
                        6.	Compare the contents of each file using memcmp().
                        7.	Close both the files.
                        8.	Cleanup any files created during the test.
                        
                        Note: The test should be repeated for different types of files as 
                              well as different size of file e.g.:
                            •	TXT
                            •	PNG
                            •	JAR
                            •	JPG
                            •	BMP
                            •	DOC
                            •	GIF
                            •	Varying sizes of each input file should be tested starting 
                                from a couple of kilobytes up to several megabytes.

@SYMTestExpectedResults The input file used for deflating should be identical to the 
                        inflated output file.
*/
bool inflateOldZlibFilesTest()
    {
    bool passed = true;

    cout << endl << "Starting test case inflateOldZlibFilesTest:" << endl;
    for(int i = 0; i < inflateOldZlibFilesTestDataItems ; i++)
        {
        stringstream ss;
        string testNum;
        ss << (i + 1);
        ss >> testNum;
        
        cout << "Starting test " << testNum << " - File: " << inflateOldZlibFilesTestFiles[i];
        
        int extBegin = inflateOldZlibFilesTestFiles[i].find_last_of(".", inflateOldZlibFilesTestFiles[i].length());
        string fileName = inflateOldZlibFilesTestFiles[i].substr(0, extBegin);
        string fileExt = inflateOldZlibFilesTestFiles[i].substr(extBegin + 1);
        
        string uncompressedFileName(filePath + inflateOldZlibFilesTestFiles[i]);
        string compressedFileName(filePath + "ezlib_" + fileName + testNum + ".zip");
        string decompressedFileName(filePath + "ezlib_" + fileName + testNum + "_decompressed" + "." + fileExt);

        if(inflateFile(compressedFileName, decompressedFileName, 15) == false)
            {
            passed = false;
            *outputFile << "\t\t<font color=FF0000>FAILED for file: " << compressedFileName << "</font><p />" << endl << endl;
            cout << " - FAILED" << endl;
            
            deleteFile(decompressedFileName);
            continue;
            }

        if(doFilesMatch(uncompressedFileName, decompressedFileName) == false)
            {
            passed = false;
            *outputFile << "\t\t<font color=FF0000>FAILED for file: " << compressedFileName << "</font><p />" << endl << endl;
            cout << " - FAILED" << endl;
            
            deleteFile(decompressedFileName);
            continue;
            }

        totalPassed++;
        *outputFile << "\t\t<font color=00AF00>PASSED for file: " << compressedFileName << "</font><p />" << endl << endl;
        cout << " - PASSED" << endl;
        
        deleteFile(decompressedFileName);
        }
    cout << "Finished test case inflateOldZlibFilesTest." << endl;

    return passed;
    }

int main()
    {
    if(openOutputFile("basicfunctest"))
        {
        *outputFile << "\t\t<b>START TEST deflateInflateTest()</b><br />" << endl;
        if(deflateInflateTest() == false)
            {
            *outputFile << "\t\t<font color=FF0000>deflateInflateTest() FAILED!</font><br />" << endl;
            }
        else
            {
            *outputFile << "\t\t<font color=00AF00>deflateInflateTest() PASSED!</font><br />" << endl;
            }
        *outputFile << "\t\t<b>END TEST deflateInflateTest()</b><p />" << endl << endl;;

        *outputFile << "\t\t<b>START TEST inflateOldZlibFilesTest()</b><br />" << endl;
        if(inflateOldZlibFilesTest() == false)
            {
            *outputFile << "\t\t<font color=FF0000>inflateOldZlibFilesTest() FAILED!</font><br />" << endl;
            }
        else
            {
            *outputFile << "\t\t<font color=00AF00>inflateOldZlibFilesTest()  PASSED!</font><br />" << endl;
            }
        *outputFile << "\t\t<b>END TEST inflateOldZlibFilesTest()</b><p />" << endl << endl;

        printTestResults();

        closeOutputFile();
        }

    return 0;
    }
