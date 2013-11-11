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
using namespace std;

int string_test()
{
 string s;
 char c;
// string s1("Hello this is string");
 cout<<"Test case for string"<<endl<<endl;
 
 cout<<"Enter the following string:  string-string "<<endl;
 cin>>s;
 cout<<"Expected output is string-string "<<endl;
 cout<<"Observed output is "<<s<<endl<<endl;


 cout<<"Enter the following string:  Hello this is string"<<endl;
 cin>>s;
 cout<<"Expected output is: Hello"<<endl;
 cout<<"Observed output is "<<s<<endl<<endl;


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

failures=string_test();
if(failures)
 cout<<"Fail"<<endl;
else
 cout<<"Pass"<<endl;
return 0;
}