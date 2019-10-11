/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 * Copyright (C) 2018-2019 Eltan B.V.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef ONBOARD_H
#define ONBOARD_H

/* SD CARD gpio */
#define SDCARD_CD 81 /* Not used */


/* CPLD definitions */
#define CPLD_PCB_VERSION_PORT 0x283
#define CPLD_PCB_VERSION_MASK 0xF0
#define CPLD_PCB_VERSION_BIT  4

#define CPLD_RESET_PORT 0x287
#define CPLD_CMD_RESET_DSI_BRIDGE_ACTIVE 0x20
#define CPLD_CMD_RESET_DSI_BRIDGE_INACTIVE 0x00
#define ITE8528_CMD_PORT 0x6E
#define ITE8528_DATA_PORT 0x6F

/* Define the items to be measured or verified */
#define FSP (const char *)"fsp.bin"
#define CMOS_LAYOUT (const char *)"cmos_layout.bin"
#define RAMSTAGE (const char *)"fallback/ramstage"
#define ROMSTAGE (const char *)"fallback/romstage"
#define PAYLOAD (const char *)"fallback/payload"
#define POSTCAR (const char *)"fallback/postcar"
#define OP_ROM_VBT (const char *)"vbt.bin"
#define MICROCODE (const char *)"cpu_microcode_blob.bin"

#endif
