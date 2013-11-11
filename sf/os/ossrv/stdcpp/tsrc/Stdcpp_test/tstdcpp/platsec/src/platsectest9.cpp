/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include<iostream>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

using namespace std;

int main()
{
	errno = 0;
	fstream outfile;
	outfile.open("c:\\private\\test.txt",fstream::out | fstream::trunc);

	if (outfile.is_open())
  {
  	outfile<<"test line\n";
    cout << "File successfully open";
    outfile.close();
  
  }
  else
  {
    cout << "Error opening file";

	cout <<"\nErro = "<<errno;
	assert_failed = true;
    
  }
  testResultXml("platsectest9");
  close_log_file();
  outfile.close();
  return 0;

}
