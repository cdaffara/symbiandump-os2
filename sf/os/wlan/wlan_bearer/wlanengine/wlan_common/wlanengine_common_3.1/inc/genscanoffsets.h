/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of scan frame offsets.
*
*/


#ifndef GENSCANOFFSETS_H
#define GENSCANOFFSETS_H

/**
 * Definitions for static control information header preceding the
 * actual beacon/probe response frame data.
 */

/** Length of the control information header. */
const u32_t CNTRL_HEADER_LEN            = 12;
/** Offset for RCPI value. */
const u32_t CNTRL_RX_LEVEL_OFFSET       = 0;
/** Offset for SNR value. Deprecated. */
const u32_t CNTRL_RX_SNR_OFFSET         = 4;
/** Offset for beacon/probe response frame data length. */
const u32_t CNTRL_LENGTH_OFFSET         = 8;

/**
 * Offsets for actual 802.11 beacon/probe response frame data.
 */

/** Start of beacon/probe response frame data. */
const u32_t DOT11_BASE_OFFSET           = CNTRL_HEADER_LEN;
/** Start of Frame Control field (2 octets). */
const u32_t MGMT_FRAME_CONTROL_OFFSET   = DOT11_BASE_OFFSET;
/** Start of Duration field (2 octets). */
const u32_t MGMT_DURATION_OFFSET        = DOT11_BASE_OFFSET + 2;
/** Start of Destination Address field (6 octets). */
const u32_t MGMT_DA_OFFSET              = DOT11_BASE_OFFSET + 4;
/** Start of Source Address field (6 octets). */
const u32_t MGMT_SA_OFFSET              = DOT11_BASE_OFFSET + 10;
/** Start of BSSID field (6 octets ). */
const u32_t MGMT_BSSID_OFFSET           = DOT11_BASE_OFFSET + 16;
/** Start of Sequence Control field (2 octets). */
const u32_t MGMT_SEQ_CONTROL_OFFSET     = DOT11_BASE_OFFSET + 22;
/** Start of Timestamp field (8 octets). */
const u32_t MGMT_TIMESTAMP_OFFSET       = DOT11_BASE_OFFSET + 24;
/** Start of Beacon Interval field (2 octets). */
const u32_t MGMT_BEACON_INTERVAL_OFFSET = DOT11_BASE_OFFSET + 32;
/** Start of Capability field (2 octets). */
const u32_t MGMT_CAPABILITY_OFFSET      = DOT11_BASE_OFFSET + 34;
/** Start of first IE in beacon/probe response frame. */
const u32_t MGMT_BODY_OFFSET            = DOT11_BASE_OFFSET + 36;

#endif // GENSCANOFFSETS_H
