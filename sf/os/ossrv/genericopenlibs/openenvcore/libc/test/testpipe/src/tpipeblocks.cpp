/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h>
#include <string.h>
#include "testpipe.h"
#include <stdio.h>

#include <sys/select.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <wchar.h>

#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>

#define MAX_SIZE 	50
#define TEST_PIPE_VAL(val, exp, text)				  {\
												if ((val==exp)) \
													{ \
													INFO_PRINTF1(text); \
													INFO_PRINTF1(_L("...passed"));\
													} \
												else \
													{ \
													ERR_PRINTF1(text); \
													ERR_PRINTF3(_L("...failed with Actual: %d Expected: %d"),(val), (exp));\
													return EFail;\
													}\
												}

#define TEST_PIPE_COND(cond, text)				  {\
												if ((cond)) \
													{ \
													INFO_PRINTF1(text); \
													INFO_PRINTF1(_L("...passed"));\
													} \
												else \
													{ \
													ERR_PRINTF1(text); \
													ERR_PRINTF1(_L("...failed"));\
													return EFail;\
													}\
												}
												

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CTestPipe::Pipe
// -----------------------------------------------------------------------------
//
TInt CTestPipe::TestPipe(  )
	{
	INFO_PRINTF1(_L("Test Pipe"));

	int fds[2];
        
    TInt err = pipe (fds);
    
    if ( err < 0)
        {
         INFO_PRINTF1(_L("pipe():Error"));        
        }
    else
    	{
    	err = close(fds[0]);
    	err = close(fds[1]);	
    	}
    
    return err;
	}

// -----------------------------------------------------------------------------
// CTestPipe::PipeCheckWriteOnReadfd
// -----------------------------------------------------------------------------
//
TInt CTestPipe::PipeCheckWriteOnReadfd(  )
	{
	_LIT(KFunc, "PipeCheckWriteonReadfd");
	INFO_PRINTF1(KFunc);
	int fds[2];
    const char* buf = "somejunk";
    
    TInt err = pipe (fds);
    
    if ( err < 0)
        {
         INFO_PRINTF1(_L("Pipe Error!"));        
         return err;
        }
    
    err = write(fds[0], buf, 9); // write on the read desc
    
    if(err < 0 && errno == EBADF)
    {
       err = KErrNone;
    }
    else
    {
       INFO_PRINTF1( KFunc());
	   err = KErrGeneral;
    }
    
    close(fds[0]);
    close(fds[1]);    
    INFO_PRINTF1(KFunc());
    return err;
	}

// -----------------------------------------------------------------------------
// CTestPipe::PipeCheckReadOnWritefd
// -----------------------------------------------------------------------------
//
TInt CTestPipe::PipeCheckReadOnWritefd()
	{
	_LIT(KFunc, "PipeCheckReadOnWritefd");
	INFO_PRINTF1(KFunc);

	int fds[2];
    char buf[128];
    
    TInt err = pipe (fds);
    
    if ( err < 0)
        {
         INFO_PRINTF1(_L("Pipe Error!"));        
         return err;
        }
    
    err = read(fds[1], buf, 128); // read on the write desc
    
    if(err < 0 && errno == EBADF)
    	{
       err = KErrNone;
    	}
    else
    	{
       INFO_PRINTF1( KFunc());
	   err = KErrGeneral;
    	}
    
    close(fds[0]);
    close(fds[1]);    
    INFO_PRINTF1( KFunc());
    return err;
	}

// -----------------------------------------------------------------------------
// CTestPipe::PipeWriteRead
// -----------------------------------------------------------------------------
//
TInt CTestPipe::PipeWriteRead(  )
	{
	_LIT(KFunc, "PipeWriteRead");
	INFO_PRINTF1(KFunc);
	int fd[2];
    const char* bufW = "somejunk";
    char bufR[128];
    
    TInt err = pipe (fd);
    
    if ( err < 0)
        {
         INFO_PRINTF1( _L("Pipe Error!"));        
         return err;
        }
    err = write(fd[1], bufW, 9); 
    
    if(err < 0)
    	{
       	INFO_PRINTF1(_L("Write Error!"));
       	close(fd[0]);
       	close(fd[1]);
       	return err;
    	}
    
    err = read(fd[0], bufR, 128 );
    
    if(err < 0)
    	{
       INFO_PRINTF1(_L("Read Error!"));
       close(fd[0]);
       close(fd[1]);
       return err;
    	}
    
    if(strcmp(bufW, bufR) != 0)
    	{
    	err = KErrGeneral;
    	INFO_PRINTF1( KFunc());
    	}
    else
    	{
    	err = KErrNone;
    	}
    close(fd[0]);
    close(fd[1]);    
    return err;
	}

// -----------------------------------------------------------------------------
// CTestPipe::PipeCheckWriteFd
// -----------------------------------------------------------------------------
//
TInt CTestPipe::PipeCheckWriteFd(  )
	{
	_LIT(KFunc, "PipeCheckFd");
	INFO_PRINTF1(KFunc);
	int fds[2];
    const char* buf = "somejunk";
    
    TInt err = pipe (fds);
    
    if ( err < 0)
        {
         INFO_PRINTF1 (_L("Pipe() Error!"));        
         return err;
        }
    
    TInt saved_fd = fds[1];
    fds[1] = 99; // change value of the returned write fd
    
    err = write(fds[1], buf, 9);
    
    if(err < 0 && errno == EBADF)
    	{
       	err = KErrNone;
    	}
    else
    	{
		INFO_PRINTF1(_L("Write Error!")) ;      
	    err = KErrGeneral;
    	}
    
    close(fds[0]);
    close(saved_fd);        
    return err;
	}

// -----------------------------------------------------------------------------
// CTestPipe::PipeCheckReadFd
// -----------------------------------------------------------------------------
//
TInt CTestPipe::PipeCheckReadFd(  )
	{
	_LIT(KFunc, "PipeCheckReadFd");
	INFO_PRINTF1(KFunc);
	int fds[2];
    char buf[128];
        
    TInt err = pipe (fds);
    if ( err < 0)
        {
         INFO_PRINTF1( _L("Pipe() Error!"));        
         return err;
        }
    
    TInt saved_fd = fds[0];    
    fds[0] = 99; // change value of the returned read fd
    
    err = read(fds[0], buf, 128);
    
    if(err < 0 && errno == EBADF)
    	{
       err = KErrNone;
    	}
    else
    	{
       INFO_PRINTF1 (_L("Read Error!"));
	   err = KErrGeneral;
    	}

    close(saved_fd);
    close(fds[1]);        
    return err;
	}

// -----------------------------------------------------------------------------
// CTestPipe::PopenPipeCommandRead
// -----------------------------------------------------------------------------
// Tests the popen() by creating a child process and reading message from it.
// -----------------------------------------------------------------------------
TInt CTestPipe::PopenPipeCommandRead()
	{	
	_LIT(KCommand,	"Command");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	
	/*read the command from data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<MAX_SIZE> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';
	char mode='r';
	
	char readbuf[100];
	FILE* fp = popen(command, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp!=NULL), _L("popen()..."));
	
	/*read data into the buffer using the file pointer*/
	fgets(readbuf, sizeof (readbuf), fp);			
	
	/*close the opened pipe*/
	TInt ret=pclose(fp);
	TEST_PIPE_VAL(ret,0, _L("pclose()..."));	
		
	return EPass;
	}
	
// -----------------------------------------------------------------------------
// CTestPipe::MultiplePopenPipeCommandRead
// -----------------------------------------------------------------------------
TInt CTestPipe::MultiplePopenPipeCommandRead()
	{	
	_LIT(KCommand,	"Command1");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	
	/*read the command from data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<MAX_SIZE> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';
	char mode='r';
	
	char readbuf[100];
	FILE* fp = popen(command, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp!=NULL), _L("popen()..."));
	
	/*read data into the buffer using the file pointer*/
	fgets(readbuf, sizeof (readbuf), fp);			
	
	/*close the opened pipe*/
	TInt ret=pclose(fp);
	
	fp = popen(command, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp!=NULL), _L("popen()..."));
	
	/*read data into the buffer using the file pointer*/
	fgets(readbuf, sizeof (readbuf), fp);				

	/*close the opened pipe*/
	ret=pclose(fp);
	
	TEST_PIPE_VAL(ret,0, _L("pclose()..."));	
	
	fp = popen(command, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp!=NULL), _L("popen()..."));
	
	/*read data into the buffer using the file pointer*/
	fgets(readbuf, sizeof (readbuf), fp);				
	
	/*close the opened pipe*/
	ret=pclose(fp);

	TEST_PIPE_VAL(ret,0, _L("pclose()..."));	
		
	fp = popen(command, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp!=NULL), _L("popen()..."));
	
	/*read data into the buffer using the file pointer*/
	fgets(readbuf, sizeof (readbuf), fp);			
	
	/*close the opened pipe*/
	ret=pclose(fp);
	
	TEST_PIPE_VAL(ret,0, _L("pclose()..."));		
	
	fp = popen(command, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp!=NULL), _L("popen()..."));
	
	/*read data into the buffer using the file pointer*/
	fgets(readbuf, sizeof (readbuf), fp);				
	
	/*close the opened pipe*/
	ret=pclose(fp);
	
	TEST_PIPE_VAL(ret,0, _L("pclose()..."));		
	return EPass;
	}	
	

// -----------------------------------------------------------------------------
// CTestPipe::PopenPipeOEExeRead
// -----------------------------------------------------------------------------
// tests popen for a OE exe(entry thru main()).
TInt CTestPipe::PopenPipeOEExeRead()
	{	
	_LIT(KCommand2,	"Command2");		
	TBufC<MAX_SIZE> bcommand2;
	TPtrC pcommand2 = bcommand2;	
	
	/*read the command from data file*/
	GetStringFromConfig(ConfigSection(),KCommand2, pcommand2);
	
	TBuf8<MAX_SIZE> buf2;
	buf2.Copy(pcommand2);	
	char* command2=(char*)buf2.Ptr();
	command2[buf2.Length()]='\0';
	char mode='r';	
	
	char readbuf2[100];
	FILE* fp = popen(command2, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp!=NULL), _L("popen()..."));
	
	/*read data into the buffer using the file pointer*/
	fgets(readbuf2, sizeof (readbuf2), fp);		
	
	/*close the opened pipe*/
	TInt ret=pclose(fp);
	TEST_PIPE_VAL(ret,0, _L("pclose()..."));	
	
	return EPass;
	}
	

// -----------------------------------------------------------------------------
// CTestPipe::PopenPipeInvalidPathRead
// -----------------------------------------------------------------------------
// tests popen for executable not in //sys//bin

TInt CTestPipe::PopenPipeInvalidPathRead()
	{	
	_LIT(KCommand3,	"Command3");		
	TBufC<MAX_SIZE> bcommand3;
	TPtrC pcommand3 = bcommand3;	
	
	/*read the command from data file*/
	GetStringFromConfig(ConfigSection(),KCommand3, pcommand3);
	
	TBuf8<MAX_SIZE> buf3;
	buf3.Copy(pcommand3);	
	char* command3=(char*)buf3.Ptr();
	command3[buf3.Length()]='\0';
	char mode='r';	
		
	FILE* fp = popen(command3, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp==NULL), _L("popen()..."));
	
	/*close the opened pipe*/
	TInt ret=pclose(fp);
	TEST_PIPE_VAL(ret,-1, _L("pclose()..."));	

	return EPass;
	}

// -----------------------------------------------------------------------------
// CTestPipe::PopenPipeInvalidCommandAndModeRead
// -----------------------------------------------------------------------------	
// tests popen with invalid arguments

TInt CTestPipe::PopenPipeInvalidCommand()
	{
    /*tests the invalid command*/
	_LIT(KInvalidCmd,	"InvalidCommand");			
	TBufC<MAX_SIZE> invcommand;
	TPtrC pinvcommand = invcommand;			
	GetStringFromConfig(ConfigSection(),KInvalidCmd, pinvcommand);	

	TBuf8<MAX_SIZE> bufinv;
	bufinv.Copy(pinvcommand);	
	char* ptrinvcommand=(char*)bufinv.Ptr();
	ptrinvcommand[bufinv.Length()]='\0';
	char mode='r';
	
	FILE* fp=popen(ptrinvcommand, &mode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND(fp==NULL, _L("popen()..."));

	TInt ret=pclose(fp);
	TEST_PIPE_VAL(ret,-1, _L("pclose()..."));	
	
	return EPass;
	}	

// -----------------------------------------------------------------------------
// CTestPipe::PopenPipeInvalidMode
// -----------------------------------------------------------------------------
TInt CTestPipe::PopenPipeInvalidMode()
	{
	_LIT(KCommand1,	"Command1");		
	TBufC<MAX_SIZE> bcommand1;
	TPtrC pcommand1 = bcommand1;	
	
	/*read the command from data file*/
	GetStringFromConfig(ConfigSection(),KCommand1, pcommand1);
	
	TBuf8<MAX_SIZE> buf1;
	buf1.Copy(pcommand1);	
	char* command1=(char*)buf1.Ptr();
	command1[buf1.Length()]='\0';
	char* rwmode="x";
	
	FILE* fp = popen(command1, rwmode); /*create a child process and a pipe for reading*/	
	TEST_PIPE_COND( (fp==NULL), _L("popen()..."));
	
    TInt ret=pclose(fp);
	TEST_PIPE_VAL(ret,-1, _L("pclose()..."));		
	
	return EPass;
	}

// -----------------------------------------------------------------------------
// CTestPipe::PopenPipeWrite
// -----------------------------------------------------------------------------
// test popen's write operation

TInt CTestPipe::PopenPipeWrite()
	{
	_LIT(KCommand4,	"Command4");		
	TBufC<MAX_SIZE> bcommand4;
	TPtrC pcommand4 = bcommand4;	
	
	/*read the command from data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand4, pcommand4);
	
	TBuf8<MAX_SIZE> buf4;
	buf4.Copy(pcommand4);	
	char* command4=(char*)buf4.Ptr();
	command4[buf4.Length()]='\0';
	char mode='w';
	
	char *writebuf="Message from parent\n";
	FILE* fp = popen(command4, &mode); /*create a child process and a pipe for writing*/	
	TEST_PIPE_COND( (fp!=NULL), _L("popen()..."));
	
	/*write data into the buffer using the file pointer*/
	fputs(writebuf, fp);			

	fflush(fp);	
	/*close the opened pipe*/
	TInt ret=pclose(fp);
	TEST_PIPE_VAL(ret,0, _L("pclose()..."));	
		
	return EPass;
	}

void* ReadData(void *data)
	{	
	DATA* dataptr = (DATA*)data;

	if(dataptr)
		{
		char* cptr = (char*)calloc(dataptr->write_size, sizeof(char));
					
		FILE* ptr=fdopen(dataptr->fds[0], "r"); 					
		dataptr->read_size=fread(cptr, sizeof(char), dataptr->write_size, ptr);
				
		free(cptr);
		fclose(ptr);
		}
	return (void*)0;	
	}

// -----------------------------------------------------------------------------
// CTestPipe::PopenBlockingRead
// -----------------------------------------------------------------------------
TInt CTestPipe::PopenBlockingRead()
	{
	DATA data;
	pthread_t threadID = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init( &threadAttr );		
	
	int fds[2];
    TInt err=pipe (fds);
    TEST_PIPE_VAL(err,0, _L("pipe().."));
    	
	char* ptr="Message to child";
	data.write_size = strlen(ptr);
	data.fds[0]=fds[0];
	data.fds[1]=fds[1];
	
	err=pthread_create( &threadID, &threadAttr, ReadData, (void*)&data);
	TEST_PIPE_VAL(err,0, _L("pthread_create()..."));
	
	size_t ndata=write(fds[1], ptr, strlen(ptr)-5);
	TEST_PIPE_VAL(ndata,(strlen(ptr)-5), _L("write()..."));
	
	err=pthread_create( &threadID, &threadAttr, ReadData, (void*)&data);
	TEST_PIPE_VAL(err,0, _L("pthread_create()..."));
	
	ptr="Another message from the parent to the child";
	ndata=write(fds[1], ptr, strlen(ptr));		
	TEST_PIPE_VAL(ndata,(strlen(ptr)), _L("write()..."));	

    close(fds[0]);
    close(fds[1]);
    		
	return EPass;
	}
		
// -----------------------------------------------------------------------------
// CTestPipe::Popen3PipeCommandRead
// -----------------------------------------------------------------------------
// test popen's read operation

TInt CTestPipe::Popen3PipeCommandRead()
	{
	_LIT(KCommand,	"Command");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<40> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';

	char readdata[15];
	
	int fds[3]; /*3 file descriptors, to be returned by popen3*/
	
	TInt childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));

	/*read data and error from the child*/		
	size_t bytes_read=read(fds[1],readdata, sizeof (readdata));
	TEST_PIPE_COND( (bytes_read>0), _L("read()..."));	

	return EPass;				
	}

// -----------------------------------------------------------------------------
// CTestPipe::MultiplePopen3PipeCommandRead
// -----------------------------------------------------------------------------
TInt CTestPipe::MultiplePopen3PipeCommandRead()
	{
	_LIT(KCommand,	"Command1");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<MAX_SIZE> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';

	char readdata[15];
	
	int fds[3]; /*3 file descriptors, to be returned by popen3*/
	
	TInt childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));

	/*read data and error from the child*/		
	size_t bytes_read=read(fds[1],readdata, sizeof (readdata));
	TEST_PIPE_COND( (bytes_read>0), _L("read()..."));	

	childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));

	/*read data and error from the child*/		
	bytes_read=read(fds[1],readdata, sizeof (readdata));
	TEST_PIPE_COND( (bytes_read>0), _L("read()..."));	

	childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));

	/*read data and error from the child*/		
	bytes_read=read(fds[1],readdata, sizeof (readdata));
	TEST_PIPE_COND( (bytes_read>0), _L("read()..."));	

	childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));

	/*read data and error from the child*/		
	bytes_read=read(fds[1],readdata, sizeof (readdata));
	TEST_PIPE_COND( (bytes_read>0), _L("read()..."));	
	
	return EPass;				
	}

// -----------------------------------------------------------------------------
// CTestPipe::Popen3PipeOEExeRead
// -----------------------------------------------------------------------------
TInt CTestPipe::Popen3PipeOEExeRead()
	{
	_LIT(KCommand,	"Command2");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<MAX_SIZE> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';

	char readdata[15];
	
	int fds[3]; /*3 file descriptors, to be returned by popen3*/
	
	TInt childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));

	/*read data and error from the child*/		
	size_t bytes_read=read(fds[1],readdata, sizeof (readdata));
	TEST_PIPE_COND( (bytes_read>0), _L("read()..."));	
	
	return EPass;					
	}

// -----------------------------------------------------------------------------
// CTestPipe::Popen3PipeInvalidPathRead
// -----------------------------------------------------------------------------
TInt CTestPipe::Popen3PipeInvalidPathRead()
	{
	_LIT(KCommand,	"Command3");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<MAX_SIZE> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';
	
	int fds[3]; /*3 file descriptors, to be returned by popen3*/
	
	TInt childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_VAL(childid,-1, _L("popen3()..."));

	return EPass;					
	}

// -----------------------------------------------------------------------------
// CTestPipe::Popen3PipeInvalidCommandAndModeRead
// -----------------------------------------------------------------------------
TInt CTestPipe::Popen3PipeInvalidCommand()
	{
	_LIT(KCommand,	"InvalidCommand");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<MAX_SIZE> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';

	char readdata[15];
	
	int fds[3]; /*3 file descriptors, to be returned by popen3*/
	
	TInt childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_VAL(childid,-1, _L("popen3()..."));

	/*read data and error from the child*/		
	size_t bytes_read=read(fds[1],readdata, sizeof (readdata));
	TEST_PIPE_COND( (bytes_read>0), _L("read()..."));
			
	return EPass;					
	}

// -----------------------------------------------------------------------------
// CTestPipe::TestProcessPopen3ChitChat
// -----------------------------------------------------------------------------
// Creates a child process(using popen3) and tests the message transfer between parent and 
// the child using the file descriptors
//
TInt CTestPipe::TestProcessPopen3ChitChat()
	{
	_LIT(KCommand,	"CommandChitChat");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<40> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';

	char writedata[MAX_SIZE];	
	char readdata[MAX_SIZE];
	char readerror[MAX_SIZE];
	
	int fds[3]; /*3 file descriptors, to be returned by popen3*/
	
	strcpy(writedata, "Message from parent\n");
	size_t num_bytes=strlen(writedata);
	size_t bytes_written, bytes_read, bytes_error;

	TInt childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));
			
	/*write to the child*/
	bytes_written=write(fds[0],writedata, num_bytes);
	TEST_PIPE_VAL( bytes_written, num_bytes, _L("write message to child..."));

	/*read data and error from the child*/		
	bytes_read=read(fds[1],readdata,num_bytes);
	TEST_PIPE_COND(bytes_read>0, _L("read message from child..."))
	
	bytes_error=read(fds[2], readerror, MAX_SIZE);		
	TEST_PIPE_COND(bytes_error>0, _L("read error from child ..."))	
	
	TInt status=-1;	
	TEST_PIPE_VAL( waitpid(childid, &status, WUNTRACED), childid, _L("waitpid()..."));
	
	return EPass;
	}


// -----------------------------------------------------------------------------
// CTestPipe::Popen3ReadWriteTest
// -----------------------------------------------------------------------------
// Creates a child process(using popen3) and tests the message transfer between parent and 
// the child using the file descriptors and also checks if the data recieved is not reapeated
// after 512 bytes. In regard to DEF113501
//

TInt CTestPipe::Popen3ReadWriteTest()
	{
	_LIT(KCommand,	"Command6");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	TInt ret = KErrNone;
	
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<40> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';

	
	int fds[3]; /*3 file descriptors, to be returned by popen3*/
	int i,a1;
	char tmp;
	wchar_t buffer[1000];
	char *data = "START_Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead_\
					_Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead_\
					_Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead_\
					_Datalongerthan512repeateswithpiperead__Datalongerthan512repeateswithpiperead__";
	size_t bytes_written;	

	TInt childid= popen3( command,NULL, NULL, fds); /*create a child*/	
	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));
			
	bytes_written=write(fds[0],data, 568);
	INFO_PRINTF2(_L("Pipe written: %d"), bytes_written);
	TEST_PIPE_COND((bytes_written==568), _L("write() to the pipe..."));
	
	i = 0;
      do
      {
      	a1=read(fds[1],&tmp,1);
      	buffer[++i] = tmp;
      }while (a1 !=0);

      if(buffer[513]==buffer[1])
      	{
      	INFO_PRINTF1(_L("Repeated data after 512 bytes.. Test case failed"));
      	ret = KErrGeneral;
      	}
      	
      else
      	INFO_PRINTF1(_L("Non-Repeated data read from pipe.. Test case passed"));
      	
      buffer[i] = '\0';
      write(1,buffer,i);
	return ret;
	}


// -----------------------------------------------------------------------------
// CTestPipe::TestSystem
// -----------------------------------------------------------------------------
// Tests the system interface by passing valid and valid arguments

TInt CTestPipe::TestSystem()
	{	
	_LIT(KCommand,	"Command");		
	TBufC<40> bcommand;
	TPtrC pcommand = bcommand;	
	
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<40> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';

	/*should return zero for a NULL command*/
	TEST_PIPE_COND((command && system(command)) || (!command && !system(command)), _L("system()..."));
	
	command=NULL;
	TEST_PIPE_COND((!command && system(command)), _L("system()..."));
	
	return EPass;
	}

//*Start: Test cases for DEF111452*//	
TInt CTestPipe::Testwsystem_LongName()
{
	int ret=EFail;
	const wchar_t* exepath= L"testforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname.exe";
	
	if((wsystem(exepath)==-1) && 
			errno==ENAMETOOLONG)
		{
		ret=EPass;	
		}
	else
		{
		ret=EFail;	
		}
	
	return ret;
}

TInt CTestPipe::Testsystem_LongName()
{
	int ret=EFail;
	const char* exepath="testforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname.exe";
	
	if((system(exepath)==-1) && 
			errno==ENAMETOOLONG)
		{
		ret=EPass;	
		}
	else
		{
		ret=EFail;	
		}
	
	return ret;
}

TInt CTestPipe::Testpopen_LongName()
{
	int ret=EFail;
	const char* exepath="testforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname.exe";
	
	if((popen(exepath, "r")==NULL) && 
			errno==ENAMETOOLONG)
		{
		ret=EPass;	
		}
	else
		{
		ret=EFail;	
		}
	
	return ret;
}

TInt CTestPipe::Testwpopen_LongName()
{
	int ret=EFail;
	const wchar_t* exepath=L"testforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname.exe";
	
	if((wpopen(exepath, L"r")==NULL) && 
			errno==ENAMETOOLONG)
		{
		ret=EPass;	
		}
	else
		{
		ret=EFail;	
		}
	
	return ret;
}

TInt CTestPipe::Testpopen3_LongName()
{
	int ret=EFail;
	int fds[3];
	const char* exepath="testforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname.exe";
	
	if((popen3(exepath, NULL, NULL, fds)==-1) && 
			errno==ENAMETOOLONG)
		{
		ret=EPass;	
		}
	else
		{
		ret=EFail;	
		}
	
	return ret;
}

TInt CTestPipe::Testwpopen3_LongName()
{
	int ret=EFail;
	int fds[3];
	const wchar_t* exepath=L"testforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname\
	testforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongnametestforlongname.exe";
	
	if((wpopen3(exepath, NULL, NULL, fds)==-1) && 
			errno==ENAMETOOLONG)
		{
		ret=EPass;	
		}
	else
		{
		ret=EFail;	
		}
	
	return ret;
}
//*End: Test cases for DEF111452*//	


// -----------------------------------------------------------------------------
// CTestPipe::Lseekpipetest
// -----------------------------------------------------------------------------
// Tests the lseek function on the write descriptor

TInt CTestPipe::Lseekpipetest()
	{
	int fds[2];
    const char* buf = "somedata";
      
    TInt err = pipe (fds);
    
    if ( err < 0)
        {
         INFO_PRINTF1( _L("pipe() failed"));        
         return err;
        }
    TInt len = strlen(buf);
    err = write(fds[1], buf, len+1); // write on the read desc
        
    if(err < 0 )
    	{
       	INFO_PRINTF1(_L("Pipe write error"));
       	}
	else
		{
		INFO_PRINTF1(_L("Write Success!"));
		}
	close(fds[1]);
	
	//int ret = fcntl(fds[0],F_SETFD, 1);
	//seek with the read descriptor
	
	int nCnt= lseek(fds[0],SEEK_SET, 0);
	if (nCnt < 0)
		{
		if(errno == ESPIPE )
			{
			INFO_PRINTF1(_L("lseek failure"));	
			}
		}
	
	char buf1[MAX_SIZE];
	TBuf<MAX_SIZE> dispBuf;
    
	err = read(fds[0], buf1, 100 /* len*/ ); // read on the pipe
	buf1[len] = '\0';
	len = strlen(buf1);
    for (TInt j =0; j<len;j++)
    	{
    	dispBuf.Append(buf1[j]);
    	}
    
    if(err < 0 )
    	{
       	INFO_PRINTF1(_L("Pipe read error"));
    	}
    else
    	{
    	INFO_PRINTF2(_L("Pipe read: %S"), &dispBuf);
        err = KErrNone;
    	}
	close(fds[0]);
	return EPass;	
 
	}

// -----------------------------------------------------------------------------
// CTestPipe::OpenMaxfdPipetest
// -----------------------------------------------------------------------------
// Tests the maximum number of file descriptors that can be opened		
TInt CTestPipe::OpenMaxfdPipetest()
	{
	int fds[2];
	int fdarr[MAX_SIZE];
    const char* buf = "somedata";  
    TInt i = 0;
    TInt j = 0;
      do
    	{
       	TInt err = pipe (fds);
       	if ( err < 0)
    		{
     		INFO_PRINTF1( _L("pipe() failed")); 
     		break;
    		}
    	else
    		{
    		//Check the descriptors
    		//write with the write descriptor
    		TInt err = write(fds[1], buf, strlen(buf)+1); // write on the  desc
    		TEST_PIPE_COND((err != -1),_L("pipe write"));
    
		    //Read descriptor
		    char buf1[10];
			err = read(fds[0], buf1,5); // read on the pipe
			TEST_PIPE_COND((err != -1),_L("pipe read"));
    		}
    	fdarr[j++]=fds[0];
    	fdarr[j++]=fds[1];
    	i++;
	    }while(i<21/*MAX_SIZE*/);
	
	INFO_PRINTF2(_L("Maximum File descriptors opened: %d "), 2*i);
	
	INFO_PRINTF1(_L("Creating pipe"));
	TInt err = pipe (fds);
    if ( err < 0)
    	{
     	INFO_PRINTF1( _L("pipe() failed")); 
     	}
    else
    	{
    	INFO_PRINTF1( _L("pipe() success"));	
    	}
    
    //Close the descriptors
	for(TInt k =0; k < j ; k++)
		{
		close(fdarr[k]);
		}
	return EPass;
	}

// -----------------------------------------------------------------------------
// Function Name:SimultaneousOpenfdPipetest
// Description:Tests the maximum number of file descriptors that can be opened		
// Return : EPass if operation is successful , EFail otherwise
// -----------------------------------------------------------------------------
TInt CTestPipe::SimultaneousOpenfdPipetest()
	{
	int fds[2];
	int fdarr[MAX_SIZE];
	TInt i;
	TInt j = 0;
	for (i = 0; i < 1024 ; i++)
		{
		int err = pipe (fds);
    	if ( err < 0)
    		{
     		INFO_PRINTF1( _L("pipe() failed"));        
     		break;
    		}
    	fdarr[j++]=fds[0];
    	fdarr[j++]=fds[1];
		}
	//Close the descriptors
	for(int k =0; k < j; k++)
		{
		close(fdarr[k]);
		}
	
    INFO_PRINTF2(_L("Descriptors open  :%d"), 2*i);
    return EPass;
	}

// -----------------------------------------------------------------------------
// Function Name:FopenMaxPopenTest
// Description:Tests the maximum number of file descriptors that can be opened
// 			    using popen()
// Return:	EPass if operation is successful , EFail otherwise		
// -----------------------------------------------------------------------------
TInt CTestPipe::FopenMaxPopenTest()
	{
	FILE * fd[MAX_SIZE];
	FILE* fp1;
	
	_LIT(KCommand1,	"Command1");		
	TPtrC pcommand1 ;	
	
	/*read the command from data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand1, pcommand1);
	
	TBuf8<MAX_SIZE> buf1;
	buf1.Copy(pcommand1);	
	char* command1=(char*)buf1.Ptr();
	command1[buf1.Length()]='\0';
	char mode = 'w';
	
	int i ;
	for (i = 0 ; i < MAX_SIZE ; i++)
		{
		fp1 = popen(command1, &mode);
		if ( fp1 == NULL)
			{
			int n = errno;
			if (n== EMFILE)
				{
				INFO_PRINTF1(_L("Error: EMFILE-The mode parameter is invalid."));
				}
			else if( n== EINVAL)
				{
				INFO_PRINTF1(_L("Error: EINVAL-No more files or streams can be created."));
				}
			break;
			}
		INFO_PRINTF1(_L("popen() success"));
		char *writebuf="Message from parent\n";
		int ret = fputs(writebuf, fp1);	
		if (ret < 0)
			{
			break;
			}
		fflush(fp1);
		fd[i] = fp1;
		}
	for (int k=0; k < i;k++)
		{
		fclose(fd[k]);
		}
	INFO_PRINTF2(_L("No of streams open :%d"), i);
	return EPass;	
	}

// -----------------------------------------------------------------------------
// Function Name:FopenMaxPopen3Test
// Description:Tests the maximum number of file descriptors that can be opened
// 			    using popen3()
// Return:	EPass if operation is successful , EFail otherwise		
// -----------------------------------------------------------------------------
TInt CTestPipe::FopenMaxPopen3Test()
	{
	_LIT(KCommand,	"Command1");		
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	int fdarr[MAX_SIZE];
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<MAX_SIZE> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';

	int i;
	int j=0;
	int fds[3]; /*3 file descriptors, to be returned by popen3*/
	for (i = 0 ; i < MAX_SIZE ; i++)
		{
		TInt childid= popen3(command,NULL, NULL, fds); /*create a child*/
		if (childid == -1)
			{
			TInt n = errno;
			if (n== EMFILE)
				{
				INFO_PRINTF1(_L("Error EMFILE"));
				}
			else if( n== EINVAL)
				{
				INFO_PRINTF1(_L("Error EINVAL"));
				}
			break;
			}
		else
			{
			char *writebuf="Message from parent\n";
			FILE *fp = fdopen(fds[0],"w");
			fputs(writebuf, fp);
			}
		fdarr[j++] = fds[0];
		fdarr[j++] = fds[1];
		fdarr[j++] = fds[2];
		}
	for (int k=0;k<j;k++)
		{
		close(fdarr[k]);
		}
	INFO_PRINTF2(_L("No. of successful calls to popen3:%d"), i);
	return EPass;
	}
	
// -----------------------------------------------------------------------------
// Function Name:TestEnvPopen3
// Description:Tests the maximum number of file descriptors that can be opened
// 			    using popen3()
// Return:	EPass if operation is successful , EFail otherwise		
// -----------------------------------------------------------------------------
TInt CTestPipe::TestEnvPopen3()
	{
	_LIT(KCommand,	"Command5");
	TBufC<MAX_SIZE> bcommand;
	TPtrC pcommand = bcommand;	
	/*read command from the data file*/
	TBool alright = GetStringFromConfig(ConfigSection(),KCommand, pcommand);
	
	TBuf8<MAX_SIZE> buf;
	buf.Copy(pcommand);	
	char* command=(char*)buf.Ptr();
	command[buf.Length()]='\0';
		
	char readdata[MAX_SIZE];
	char **env1p;
	env1p = new char*[2];
	
	env1p[0]= new char[15];
	strcpy(env1p[0],"user = \\home");
	

	env1p[1]= 0;
		
	TInt fds[3];
	TInt childid= popen3(command, NULL, env1p, fds); /*create a child*/

	TEST_PIPE_COND((childid!=-1), _L("popen3()..."));
	
	/*read data and error from the child*/		
	size_t bytes_read=read(fds[1],readdata, sizeof (readdata));
	
	TBuf<MAX_SIZE> buf1;
	TInt len = strlen(readdata);
	TInt i;
	for (i=0; i< bytes_read;i++)
		{
		buf1.Append(readdata[i]);
		}
	INFO_PRINTF2(_L("Data read:' %S '"), &buf1);
	TEST_PIPE_COND( (bytes_read>0), _L("read()..."));
	return EPass;	
	}

// -----------------------------------------------------------------------------
// Function Name:TestPipeWaitForRead
// Description:Tests whether the code waits until there is no data in the pipe.
// Return:	EPass if operation is successful , EFail otherwise		
// -----------------------------------------------------------------------------
TInt CTestPipe::TestPipeWaitForData()
	{
	fd_set read_fds;
	char pipeName[256];
	sprintf(pipeName,"c:\\telnetd_pipe");
    unlink(pipeName);
	if (mkfifo(pipeName, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH) < 0)
		{
		INFO_PRINTF1(_L("Failed to create FIFO special file\n"));
		return EFail;
		}

	TInt pipe = open(pipeName, O_RDWR | O_NONBLOCK);
	if(pipe<0)
		{
		INFO_PRINTF1(_L("Failed to create Pipe\n"));
		return EFail;
		}

		FD_ZERO(&read_fds);
		FD_SET(pipe, &read_fds);

		TInt ret=select(pipe+1, &read_fds, NULL, NULL, NULL); 
		if(ret)
			{
			INFO_PRINTF1(_L("Select returned unexpectedly\n"));
			return EFail;
			}
		return EPass;
		}

//  End of File
