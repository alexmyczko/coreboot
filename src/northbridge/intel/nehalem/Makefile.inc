#
# This file is part of the coreboot project.
#
# Copyright (C) 2010 Google Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

ifeq ($(CONFIG_NORTHBRIDGE_INTEL_NEHALEM),y)

bootblock-y += bootblock.c

ramstage-y += memmap.c
ramstage-y += northbridge.c
ramstage-y += smi.c
ramstage-y += gma.c

ramstage-y += acpi.c

romstage-y += memmap.c
romstage-y += raminit.c
romstage-y += raminit_tables.c
romstage-y += early_init.c
romstage-y += romstage.c

smm-y += finalize.c

postcar-y += memmap.c

endif
