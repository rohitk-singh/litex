/*
 * Milkymist SoC (Software)
 * Copyright (C) 2007, 2008, 2009, 2011, 2012 Sebastien Bourdeauducq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <hw/id.h>
#include <hw/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <version.h>
#include <board.h>

static const struct board_desc boards[1] = {
	{
		.id = 0x4D31, /* M1 */
		.name = "Milkymist One",
		.ethernet_phyadr = 1
	},
};

static const struct board_desc *get_board_desc_id(unsigned short int id)
{
	unsigned int i;

	for(i=0;i<sizeof(boards)/sizeof(boards[0]);i++)
		if(boards[i].id == id)
			return &boards[i];
	return NULL;
}

static const struct board_desc *get_board_desc(void)
{
	return get_board_desc_id((CSR_ID_SYSTEMH << 8) | CSR_ID_SYSTEML);
}

int get_pcb_revision(void)
{
	/* TODO
	int r;
	unsigned int io;

	r = 0;
	io = CSR_GPIO_IN;
	if(io & GPIO_PCBREV0)
		r |= 0x1;
	if(io & GPIO_PCBREV1)
		r |= 0x2;
	if(io & GPIO_PCBREV2)
		r |= 0x4;
	if(io & GPIO_PCBREV3)
		r |= 0x8;
	return r;*/
	return 0;
}

void get_soc_version(unsigned int *major, unsigned int *minor, unsigned int *subminor, unsigned int *rc)
{
	unsigned int id;

	id = CSR_ID_VERSIONH;
	*major = (id & 0xf0) >> 4;
	*minor = id & 0x0f;
	id = CSR_ID_VERSIONL;
	*subminor = (id & 0xf0) >> 4;
	*rc = id & 0x0f;
}

void get_soc_version_formatted(char *version)
{
	unsigned int major, minor, subminor, rc;

	get_soc_version(&major, &minor, &subminor, &rc);

	version += sprintf(version, "%u.%u", major, minor);
	if(subminor != 0)
		version += sprintf(version, ".%u", subminor);
	if(rc != 0)
		sprintf(version, "RC%u", rc);
}

const struct board_desc *brd_desc;

void board_init(void)
{
	int rev;
	char soc_version[13];

	brd_desc = get_board_desc();

	if(brd_desc == NULL) {
		printf("Running on unknown board, startup aborted.\n");
		while(1);
	}
	rev = get_pcb_revision();
	get_soc_version_formatted(soc_version);
	printf("Detected SoC %s on %s (PCB revision %d)\n", soc_version, brd_desc->name, rev);
	if(strcmp(soc_version, VERSION) != 0)
		printf("SoC and BIOS versions do not match!\n");
	if(rev > 2)
		printf("Unsupported PCB revision, please upgrade!\n");
}