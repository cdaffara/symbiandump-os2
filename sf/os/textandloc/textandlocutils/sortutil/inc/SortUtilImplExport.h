/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:      Entry point of the polymorphic DLL (SortUtilImpl.dll)
*
*/






#ifndef SORTUTILIMPLEXPORT_H
#define SORTUTILIMPLEXPORT_H

class MSortUtil;
// Only export in DLL
IMPORT_C MSortUtil* SortUtilFactoryFunctionL();

#endif // SORTUTILIMPLEXPORT_H

// End of file
