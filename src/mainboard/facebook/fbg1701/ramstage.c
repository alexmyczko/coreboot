/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018-2019 Facebook, Inc
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

#include <arch/io.h>
#include <console/console.h>
#include <soc/ramstage.h>
#include <soc/smbus.h>
#include "mainboard.h"
#include "onboard.h"

struct edp_data {
	u8 payload_length;
	u8 address;
	/* data: reg[15:8],reg[7:0], data bytes starting with data[7:0] */
	u8 data[6];
} __packed;

static const struct edp_data b101uan01_table[] = {
	/* set eDP bridge to 1200x1920 */
	/* IO */
	{6, 0x68, {0x08, 0x00, 0x01, 0x00, 0x00, 0x00} },
	/* Boot */
	{6, 0x68, {0x10, 0x00, 0x78, 0x69, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x04, 0x02, 0x08, 0x02, 0x00} },
	{6, 0x68, {0x10, 0x08, 0x23, 0x00, 0x87, 0x02} },
	{6, 0x68, {0x10, 0x0C, 0x19, 0x04, 0x00, 0x23} },
	{6, 0x68, {0x10, 0x10, 0x06, 0x00, 0x67, 0x00} },
	{6, 0x68, {0x10, 0x14, 0x01, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x18, 0xFF, 0xFF, 0xFF, 0xFF} },
	/* Internal */
	{3, 0x68, {0xB0, 0x05, 0x0A, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x06, 0x03, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x07, 0x16, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x08, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x09, 0x21, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x0A, 0x07, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x14, 0x03, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x18, 0xFF, 0xFF, 0xFF, 0xFF} },
	/* eDP */
	{3, 0x68, {0x80, 0x03, 0x41, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB4, 0x00, 0x0D, 0x00, 0x00, 0x00} },
	/* DPRX */
	{3, 0x68, {0xB8, 0x8E, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8F, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x9A, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x9B, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x00, 0x0E, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x26, 0x02, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x01, 0x20, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC0, 0xF1, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC1, 0xF1, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC2, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC3, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC4, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC5, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC6, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC7, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x0B, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x33, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x5B, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x10, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x38, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x60, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x15, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x3D, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x65, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x1A, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x42, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x6A, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x1F, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x47, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x6F, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x24, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x4C, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x74, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x29, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x51, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x79, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x2E, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x56, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x7E, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x90, 0x10, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x91, 0x0F, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x92, 0xF6, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x93, 0x10, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x94, 0x0F, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x95, 0xF6, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x96, 0x10, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x97, 0x0F, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x98, 0xF6, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x99, 0x10, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x9A, 0x0F, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x9B, 0xF6, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8A, 0x03, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x96, 0x03, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0xD1, 0x07, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0xB0, 0x07, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8B, 0x04, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8C, 0x45, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8D, 0x05, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x97, 0x04, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x98, 0xE0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x99, 0x2E, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x80, 0x0E, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x14, 0x07, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x18, 0xFF, 0xFF, 0xFF, 0xFF} },
	/* Video size */
	{6, 0x68, {0x01, 0x48, 0xB0, 0x04, 0x00, 0x00} },
	{6, 0x68, {0x29, 0x20, 0x10, 0x0E, 0x0B, 0x3E} },
	/* eDP */
	{3, 0x68, {0xB6, 0x31, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x80, 0x01, 0x14, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x80, 0x02, 0x02, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB6, 0x08, 0x0B, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x00, 0x1E, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x87, 0x00, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x50, 0x10, 0x00, 0x00, 0x9D, 0x00} },
	{6, 0x68, {0x00, 0x8C, 0x40, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x80, 0x02, 0x00, 0x00, 0x00} },
	/* Link Training */
	{3, 0x68, {0x82, 0x02, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x82, 0x03, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x82, 0x04, 0xFF, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x21, 0x58, 0x09, 0x00, 0x28, 0x00} },
	{6, 0x68, {0x21, 0x60, 0x07, 0x00, 0x0F, 0x00} },
	{6, 0x68, {0x21, 0x64, 0x28, 0x23, 0x00, 0x00} },
	{6, 0x68, {0x21, 0x68, 0x0E, 0x00, 0x00, 0x00} },
	/* DSI */
	{6, 0x68, {0x20, 0x7C, 0x81, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x20, 0x50, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x20, 0x1C, 0x01, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x20, 0x60, 0xFF, 0xFF, 0xFF, 0xFF} },
	/* GPIO */
	{6, 0x68, {0x08, 0x04, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x80, 0x0F, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x84, 0x0F, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x84, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x84, 0x0F, 0x00, 0x00, 0x00} },
	/* DSI clock */
	{6, 0x68, {0x20, 0x50, 0x20, 0x00, 0x00, 0x00} },
	/* LCD init */
	{6, 0x68, {0x22, 0xFC, 0x15, 0x01, 0x00, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x8C, 0x80, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0xC7, 0x50, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0xC5, 0x50, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x85, 0x04, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x86, 0x08, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x83, 0xAA, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x84, 0x11, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x9C, 0x10, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0xA9, 0x4B, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x05, 0x11, 0x00, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x05, 0x29, 0x00, 0x81} },
	{6, 0x68, {0x2A, 0x10, 0x10, 0x00, 0x04, 0x80} },
	{6, 0x68, {0x2A, 0x04, 0x01, 0x00, 0x00, 0x00} },
	/* Check Video */
	{6, 0x68, {0x01, 0x54, 0x01, 0x00, 0x00, 0x00} },
	/* End of table */
	{0, 0x00, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
};

static const struct edp_data b101uan08_table[] = {
	/* set eDP bridge to 1200x1920 */
	/* IO Voltage Setting */
	{6, 0x68, {0x08, 0x00, 0x01, 0x00, 0x00, 0x00} },
	/* Boot Settings */
	{6, 0x68, {0x10, 0x00, 0x78, 0x69, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x04, 0x02, 0x08, 0x02, 0x00} },
	{6, 0x68, {0x10, 0x08, 0x22, 0x00, 0xA0, 0x02} },
	{6, 0x68, {0x10, 0x0C, 0x50, 0x04, 0x00, 0x03} },
	{6, 0x68, {0x10, 0x10, 0x10, 0x0D, 0x06, 0x01} },
	{6, 0x68, {0x10, 0x14, 0x01, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x18, 0xFF, 0xFF, 0xFF, 0xFF} },
	/* Internal PCLK settings for Non Present or REFCLK=26MHz */
	{3, 0x68, {0xB0, 0x05, 0x0A, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x06, 0x03, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x07, 0x16, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x08, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x09, 0x21, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB0, 0x0A, 0x07, 0x00, 0x00, 0x00} },
	/* DSI Clock setting for Non Preset or REFCLK=26MHz */
	{6, 0x68, {0x41, 0xB0, 0xC1, 0x22, 0x04, 0x00} },
	{6, 0x68, {0x41, 0xBC, 0x01, 0x0E, 0x00, 0x00} },
	{6, 0x68, {0x41, 0xC0, 0x30, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x14, 0x03, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x18, 0xFF, 0xFF, 0xFF, 0xFF} },
	/* Additional Settng for eDP */
	{3, 0x68, {0x80, 0x03, 0x41, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB4, 0x00, 0x0D, 0x00, 0x00, 0x00} },
	/* DPRX CAD Register Setting */
	{3, 0x68, {0xB8, 0x8E, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8F, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x9A, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x9B, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x00, 0x0E, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x26, 0x02, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x01, 0x20, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC0, 0xF1, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC1, 0xF1, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC2, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC3, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC4, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC5, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC6, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0xC7, 0xF0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x0B, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x33, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x5B, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x10, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x38, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x60, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x15, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x3D, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x65, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x1A, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x42, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x6A, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x1F, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x47, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x6F, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x24, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x4C, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x74, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x29, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x51, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x79, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x2E, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x56, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x7E, 0x00, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x90, 0x10, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x91, 0x0F, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x92, 0xF6, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x93, 0x10, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x94, 0x0F, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x95, 0xF6, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x96, 0x10, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x97, 0x0F, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x98, 0xF6, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x99, 0x10, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x9A, 0x0F, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0x9B, 0xF6, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8A, 0x03, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x96, 0x03, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0xD1, 0x07, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xBB, 0xB0, 0x07, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8B, 0x04, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8C, 0x45, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x8D, 0x05, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x97, 0x04, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x98, 0xE0, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x99, 0x2E, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x80, 0x0E, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x14, 0x07, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x10, 0x18, 0xFF, 0xFF, 0xFF, 0xFF} },
	/* Video size Related Settings for Non Present */
	{6, 0x68, {0x01, 0x48, 0xB0, 0x04, 0x00, 0x00} },
	{6, 0x68, {0x29, 0x20, 0x10, 0x0E, 0x0B, 0x3E} },
	/* eDP Settings for Link Training*/
	{3, 0x68, {0xB6, 0x31, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x80, 0x01, 0x14, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x80, 0x02, 0x02, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB6, 0x08, 0x0B, 0x00, 0x00, 0x00} },
	{3, 0x68, {0xB8, 0x00, 0x1E, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x87, 0x00, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x50, 0x10, 0x00, 0x00, 0x9D, 0x00} },
	{6, 0x68, {0x00, 0x8C, 0x40, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x80, 0x02, 0x00, 0x00, 0x00} },
	/* Link Training */
	{3, 0x68, {0x82, 0x02, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x82, 0x03, 0xFF, 0x00, 0x00, 0x00} },
	{3, 0x68, {0x82, 0x04, 0xFF, 0x00, 0x00, 0x00} },
	/* DSI Transition Time Setting for Non Preset */
	{6, 0x68, {0x21, 0x54, 0x0D, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x21, 0x58, 0x06, 0x00, 0x2A, 0x00} },
	{6, 0x68, {0x21, 0x5C, 0x07, 0x00, 0x0E, 0x00} },
	{6, 0x68, {0x21, 0x60, 0x07, 0x00, 0x10, 0x00} },
	{6, 0x68, {0x21, 0x64, 0x10, 0x27, 0x00, 0x00} },
	{6, 0x68, {0x21, 0x68, 0x0E, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x21, 0x6C, 0x0A, 0x00, 0x0E, 0x00} },
	{6, 0x68, {0x21, 0x78, 0x0E, 0x00, 0x0D, 0x00} },
	/* DSI Start */
	{6, 0x68, {0x20, 0x7C, 0x81, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x20, 0x50, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x20, 0x1C, 0x01, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x20, 0x60, 0xFF, 0xFF, 0xFF, 0xFF} },
	/* GPIO for LCD control*/
	{6, 0x68, {0x08, 0x04, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x80, 0x0F, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x84, 0x0F, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x84, 0x00, 0x00, 0x00, 0x00} },
	{6, 0x68, {0x00, 0x84, 0x0F, 0x00, 0x00, 0x00} },
	/* DSI Hs Clock Mode */
	{6, 0x68, {0x20, 0x50, 0x20, 0x00, 0x00, 0x00} },
	/* LCD Initialization */
	{6, 0x68, {0x22, 0xFC, 0x15, 0xBF, 0xA5, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x01, 0x00, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x8F, 0xA5, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x83, 0xAA, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x84, 0x11, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0xA9, 0x48, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x83, 0x00, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x84, 0x00, 0x81} },
	{6, 0x68, {0x22, 0xFC, 0x15, 0x8F, 0x00, 0x81} },
	{6, 0x68, {0x2A, 0x10, 0x10, 0x00, 0x04, 0x80} },
	{6, 0x68, {0x2A, 0x04, 0x01, 0x00, 0x00, 0x00} },
	/* Check if eDP video is coming */
	{6, 0x68, {0x01, 0x54, 0x01, 0x00, 0x00, 0x00} },
	/* End of table */
	{0, 0x00, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
};

static void mainboard_configure_edp_bridge(void)
{
	const struct edp_data *edptable;
	unsigned int loops;
	int status;

	if (mainboard_read_pcb_version() < 7)
		edptable = b101uan01_table;
	else
		edptable = b101uan08_table;

	/* reset bridge */
	outb(CPLD_CMD_RESET_DSI_BRIDGE_ACTIVE, CPLD_RESET_PORT);
	outb(CPLD_CMD_RESET_DSI_BRIDGE_INACTIVE, CPLD_RESET_PORT);

	while (edptable->payload_length) {
		loops = 5;
		do {
			status = smbus_i2c_block_write(edptable->address,
						       edptable->payload_length,
						       (u8 *)&edptable->data[0]);
		} while (--loops && (status < 0));

		if (loops == 0) {
			printk(BIOS_ERR, "Writing eDP bridge failed!\n");
			return;
		}
		edptable++;
	};
}

void mainboard_silicon_init_params(SILICON_INIT_UPD *params)
{
	mainboard_configure_edp_bridge();

	if (CONFIG(FSP1_1_DISPLAY_LOGO)) {
		size_t logo_len;
		void *logo = NULL;

		logo = load_logo(&logo_len);

		if (logo) {
			params->PcdLogoPtr = (u32)logo;
			params->PcdLogoSize = logo_len;
		}
	}
}
