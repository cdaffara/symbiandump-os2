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
#include<stdlib.h>
#include<string> 
using namespace std;

int char_test()
{
 char* s=(char*)malloc(strlen("string-string"));
 char c;
 char* str=(char*)malloc(strlen("Hello this is string"));
 cout<<"Test case for string"<<endl<<endl;
 
 cout<<"Enter the following string:  string-string "<<endl;
 cin>>s;
 cout<<"Expected output is string-string "<<endl;
 cout<<"Observed output is "<<s<<endl<<endl;


 cout<<"Enter the following string: Hello this is string"<<endl;
 cin>>str;
 cout<<"Expected output is: Hello"<<endl;
 cout<<"Observed output is "<<str<<endl<<endl;


  cout<<"Press 1 to PASS or 0 to FAIL this sub-case:\r\n";
 do{
    c=getchar();
   }while(c!='1' && c!='0');
 

if('1'==c)
 return 0;
else
 return 1;
}

int main()
{

int failures=0;

failures=char_test();
if(failures)
 cout<<"Fail"<<endl;
else
 cout<<"Pass"<<endl;
return 0;
}