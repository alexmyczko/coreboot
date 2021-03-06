/*
 * This file is part of the coreboot project.
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

#include <bootblock_common.h>
#include <device/pnp_type.h>
#include <southbridge/amd/pi/hudson/hudson.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>

#define SIO_PORT 0x2e
#define SERIAL1_DEV PNP_DEV(SIO_PORT, NCT5104D_SP1)
#define SERIAL2_DEV PNP_DEV(SIO_PORT, NCT5104D_SP2)

void bootblock_mainboard_early_init(void)
{
	hudson_lpc_port80();
	hudson_clk_output_48Mhz();

	/* COM2 on apu5 is reserved so only COM1 should be supported */
	if ((CONFIG_UART_FOR_CONSOLE == 1) &&
		!CONFIG(BOARD_PCENGINES_APU5))
		nuvoton_enable_serial(SERIAL2_DEV, CONFIG_TTYS0_BASE);
	else if (CONFIG_UART_FOR_CONSOLE == 0)
		nuvoton_enable_serial(SERIAL1_DEV, CONFIG_TTYS0_BASE);
}
