/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <complex.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "modem_table.h"
#include "lte_tables.h"

/** Internal functions */
static int table_create(modem_table_t* q) {
	q->symbol_table = malloc(q->nsymbols*sizeof(cf));
	return q->symbol_table==NULL;
}

void modem_table_init(modem_table_t* q) {
	bzero((void*)q,sizeof(modem_table_t));
}
void modem_table_free(modem_table_t* q) {
	if (q->symbol_table) {
		free(q->symbol_table);
	}
}
void modem_table_reset(modem_table_t* q) {
	modem_table_free(q);
	modem_table_init(q);
}

int modem_table_set(modem_table_t* q, cf* table, soft_table_t *soft_table, int nsymbols, int nbits_x_symbol) {
	if (q->nsymbols) {
		return -1;
	}
	q->nsymbols = nsymbols;
	if (table_create(q)) {
		return -1;
	}
	memcpy(q->symbol_table,table,q->nsymbols*sizeof(cf));
	memcpy(&q->soft_table,soft_table,sizeof(soft_table_t));
	q->nbits_x_symbol = nbits_x_symbol;
	return 0;
}

int modem_table_std(modem_table_t* q, enum modem_std  std, bool compute_soft_demod) {
	switch(std) {
	case LTE_BPSK:
		q->nbits_x_symbol = 1;
		q->nsymbols = 2;
		if (table_create(q)) {
			return -1;
		}
		set_BPSKtable(q->symbol_table, &q->soft_table, compute_soft_demod);
		break;
	case LTE_QPSK:
		q->nbits_x_symbol = 2;
		q->nsymbols = 4;
		if (table_create(q)) {
			return -1;
		}
		set_QPSKtable(q->symbol_table, &q->soft_table, compute_soft_demod);
		break;
	case LTE_QAM16:
		q->nbits_x_symbol = 4;
		q->nsymbols = 16;
		if (table_create(q)) {
			return -1;
		}
		set_16QAMtable(q->symbol_table, &q->soft_table, compute_soft_demod);
		break;
	case LTE_QAM64:
		q->nbits_x_symbol = 6;
		q->nsymbols = 64;
		if (table_create(q)) {
			return -1;
		}
		set_64QAMtable(q->symbol_table, &q->soft_table, compute_soft_demod);
		break;
	}
	return 0;
}
