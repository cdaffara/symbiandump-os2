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

#if (!defined __EFMFF_H__)
#define __EFMFF_H__

class CEFMFeatFlag : public CBase
    {
public:
    CEFMFeatFlag(TUint32, TUint32, TUint32);
    ~CEFMFeatFlag() {};
    CEFMFeatFlag(const CEFMFeatFlag &);
	TInt BinaryContent(TDes8 &);
	TUint32 UID() const;
	TUint32 StatusFlags() const;
	TUint32 UserData() const;
	TInt Compare(const CEFMFeatFlag &);

private:
	enum T_EFMSupportFlags {
			                  EFMSupported     = 0x1,
			                  EFMUpgradable    = 0x2,
			                  EFMModifiable    = 0x4,
			                  EFMBlackListed   = 0x8,
			                  EFMUninitialised = 0x10,
			                  EFMPersisted     = 0x20,
			               };
	TUint32 iuid, isflag, iuserdata;

protected:
    };

#endif  //__EFMFF_H__
