/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A wrapper of OpenSSL bio.c functions to Symbian.
*
*/


/** A wrapper of OpenSSL bio.c functions to Symbian **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <e32base.h>
#include "xmlsec_error_flag.h"
#include "xmlsecc_config.h"
#include "xmlsecc_bio.h"

BIO* BIO_new_buffer(const char *aData, unsigned int aSize, const char *aName)
    {
    BIO	*bio;
    char *tmpname = NULL;
    			
    char* buf = (char *)malloc(aSize + 1);
    if(!buf){	   
	    xmlSecSetErrorFlag( KErrNoMemory );
	    return(NULL);
	}
	memcpy(buf, aData, aSize);
	
    if (aName)
	{
		tmpname = (char *)malloc(sizeof(char)*(strlen(aName)+1));
		if (!tmpname)
		{
		    xmlSecSetErrorFlag( KErrNoMemory );
			free(buf);
			return (NULL);
		}		
		strcpy(tmpname, aName);
	}

	bio = (BIO *)malloc(sizeof(BIO));
	if (!bio)
	{
		xmlSecSetErrorFlag( KErrNoMemory );
	    free(buf);
	    free(tmpname);
	    return(NULL);
	}
	
	bio->mem = buf;
	bio->len = aSize;
	bio->name = tmpname;
	
	return bio;
    }

BIO* BIO_new_file(const char *aFilename, const char *aMode, const char *name)
{
    BIO *bio;
	FILE *fp;
    char *buf;
    long fileLen;
    int byteRead;
    char *tmpname = NULL;

    //Read file to memory
	fp = fopen(aFilename, aMode);
	if(!fp)
	    {
	    if ( errno == ENOMEM )
	        {
		    xmlSecSetErrorFlag( KErrNoMemory );
	        }
	    return(NULL);
	    }
	fseek(fp, 0, SEEK_END);
	fileLen = ftell(fp);
	if(fileLen < 0)
		{
			xmlSecSetErrorFlag( KErrGeneral );
			fclose(fp);
			return(NULL);
		}
	fseek ( fp , 0L , SEEK_SET );
	
	buf = (char *)malloc(sizeof(char)*(fileLen+1));
    if(!buf) {	   
		xmlSecSetErrorFlag( KErrNoMemory );
		fclose(fp);
	    return(NULL);
	}    
				
	byteRead = fread(buf, sizeof(char), fileLen, fp);
    if(byteRead != fileLen) {
	    free(buf);
	    fclose(fp);
	    return(NULL);
	}   
	
	if (name)
	{
		tmpname = (char *)malloc(sizeof(char)*(strlen(name)+1));
		if (!tmpname)
		{
		    xmlSecSetErrorFlag( KErrNoMemory );
			free(buf);
			fclose(fp);
			return (NULL);
		}		
		strcpy(tmpname, name);
	}

	bio = (BIO *)malloc(sizeof(BIO));
	if (!bio)
	{
		xmlSecSetErrorFlag( KErrNoMemory );
		if (tmpname)
		    {
		    free(tmpname);
		    }
	    free(buf);
	    fclose(fp);
	    return(NULL);
	}
	
	bio->mem = buf;
	bio->len = fileLen;
	bio->name = tmpname;
	fclose(fp);
	return bio; 	

}

void BIO_free(BIO *bio)
{
	if (bio)
	{
		if (bio->mem)
		{
			memset(bio->mem, 0, bio->len);
			free(bio->mem);			
		}
		if (bio->name)
		{
		    free(bio->name);        
		}
		free(bio);
	}
}

BIO* BIO_new()
{
	BIO *bio = (BIO *)malloc(sizeof(BIO));
	if ( !bio ) 
	    {
	    xmlSecSetErrorFlag( KErrNoMemory );
	    return NULL;
	    }
	bio->mem = NULL;
	bio->len = 0;
	bio->name = NULL;
	return bio;
}

int BIO_write(BIO *bio, const unsigned char *buf, unsigned int size)
{
	bio->mem = (char *)malloc(size *sizeof(char));
	if (!bio->mem)
	    {
	    xmlSecSetErrorFlag( KErrNoMemory );
	    return -1;    
	    }
	
	memcpy((unsigned char*)bio->mem, buf, size);
	bio->len = size;
	
	return 0;
}
