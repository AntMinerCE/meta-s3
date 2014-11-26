/*
 * Copyright 2013 Con Kolivas
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#ifndef BITFURY_H
#define BITFURY_H

#include "miner.h"
#include "usbutils.h"
#include "mcp2210.h"

#define BXF_CLOCK_DEFAULT 54
#define BXF_CLOCK_OFF 0
#define BXF_CLOCK_MIN 32
#define BXF_CLOCK_MAX 63 // Not really used since we only get hw errors above default

/* In tenths of a degree */
#define BXF_TEMP_TARGET 820
#define BXF_TEMP_HYSTERESIS 30

extern int opt_bxf_temp_target;
extern int opt_nf1_bits;

#define NF1_PIN_LED 0
#define NF1_PIN_SCK_OVR 5
#define NF1_PIN_PWR_EN 6

#define SPIBUF_SIZE 16384
#define BITFURY_REFRESH_DELAY 100

struct bitfury_payload {
	unsigned char midstate[32];
	unsigned int junk[8];
	unsigned m7;
	unsigned ntime;
	unsigned nbits;
	unsigned nnonce;
};

struct bitfury_info {
	struct cgpu_info *base_cgpu;
	struct thr_info *thr;
	enum sub_ident ident;
	int nonces;
	int total_nonces;
	double saved_nonces;
	int cycles;
	bool valid; /* Set on first valid data being found */

	/* BF1 specific data */
	uint8_t version;
	char product[8];
	uint32_t serial;
	struct timeval tv_start;

	/* BXF specific data */
	pthread_mutex_t lock;
	pthread_t read_thr;
	double temperature;
	int last_decitemp;
	int max_decitemp;
	int temp_target;
	int work_id; // Current work->subid
	int no_matching_work;
	int maxroll; // Last maxroll sent to device
	int ver_major;
	int ver_minor;
	int hw_rev;
	int chips;
	uint8_t clocks; // There are two but we set them equal
	int filtered_hw[2]; // Hardware errors we're told about but are filtered
	int job[2]; // Completed jobs we're told about
	int submits[2]; // Submitted responses

	/* NF1 specific data */
	struct mcp_settings mcp;
	char spibuf[SPIBUF_SIZE];
	unsigned int spibufsz;
	int osc6_bits;
	struct bitfury_payload payload;
	struct bitfury_payload opayload;
	unsigned newbuf[17];
	unsigned oldbuf[17];
	bool job_switched;
	bool second_run;
	struct work *work;
	struct work *owork;
};

#endif /* BITFURY_H */
