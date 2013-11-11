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

#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    if(argc == 0 || argc == 1)
    {
    cout << "MacroInstrumenterTool - help\n";      
    cout << "argv[1] - input file (file for modification)\n";
    cout << "argv[2] - macro to include \n";
    cout << "argv[3] - macro mode mode: 0 - define style; 1 - MACRO style (default); \n";
    cout << "example: MacroInstrumenter myFile.mmp __LIGHTLOGGER_ENABLED\n";
    }
    else if(argc >= 2)
    {
        ofstream file;
        file.open(argv[1], fstream::app);
        if(file)
        {
            file.seekp (0, ofstream::end);
            char* ptr = "MACRO ";
            if(argc >= 3 && strcmp(argv[3], "0") == 0)
                ptr = "#define ";

            file <<"\n" <<ptr <<argv[2]; //instrument file with macro
            cout<<"File " <<argv[1] <<" instrumented!\n";
        }
        file.close();     
    }
    
    return EXIT_SUCCESS;
}
