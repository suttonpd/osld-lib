/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>, Vuk Marojevic <maroje@vt.edu>.
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
#include <math.h>

#include "modem_table.h"
#include "lte_tables.h"

/**
 * Set the BPSK modulation table */
void set_BPSKtable(cf* table, soft_table_t *soft_table, bool compute_soft_demod)
{
	// LTE-BPSK constellation:
	//    Q
	//    |  0
	//---------> I
	// 1  |
	table[0] = BPSK_LEVEL + BPSK_LEVEL*_Complex_I;
	table[1] = -BPSK_LEVEL -BPSK_LEVEL*_Complex_I;

	if (!compute_soft_demod) {
		return;
	}

	/* BSPK symbols containing a '0' and a '1' (only two symbols, 1 bit) */
	soft_table->idx[0][0][0] = 0;
	soft_table->idx[1][0][0] = 1;
}

/**
 * Set the QPSK modulation table */
void set_QPSKtable(cf* table, soft_table_t *soft_table, bool compute_soft_demod)
{
	int i,j;

	// LTE-QPSK constellation:
	//     Q
	// 10  |  00
	//-----------> I
	// 11  |  01
	table[0] = QPSK_LEVEL + QPSK_LEVEL*_Complex_I;
	table[1] = QPSK_LEVEL - QPSK_LEVEL*_Complex_I;
	table[2] = -QPSK_LEVEL + QPSK_LEVEL*_Complex_I;
	table[3] = -QPSK_LEVEL - QPSK_LEVEL*_Complex_I;
	for (i=0;i<6;i++) {
		for (j=0;j<32;j++) {
			soft_table->idx[0][i][j] = 0;
			soft_table->idx[1][i][j] = 0;
		}
	}

	if (!compute_soft_demod) {
		return;
	}

	/* QSPK symbols containing a '0' at the different bit positions */
	soft_table->idx[0][0][0] = 0;
	soft_table->idx[0][0][1] = 1;
	soft_table->idx[0][1][0] = 0;
	soft_table->idx[0][1][1] = 2;
	/* QSPK symbols containing a '1' at the different bit positions */
	soft_table->idx[1][0][0] = 2;
	soft_table->idx[1][0][1] = 3;
	soft_table->idx[1][1][0] = 1;
	soft_table->idx[1][1][1] = 3;
}

/**
 * Set the 16QAM modulation table */
void set_16QAMtable(cf* table, soft_table_t *soft_table, bool compute_soft_demod)
{
	int i,j;
	// LTE-16QAM constellation:
	//                Q
	//  1011	1001  |	 0001	0011
	//  1010	1000  |	 0000	0010
	//---------------------------------> I
	//  1110    1100  |  0100	0110
	//  1111    1101  |  0101	0111
	table[0] = QAM16_LEVEL_1 + QAM16_LEVEL_1*_Complex_I;
	table[1] = QAM16_LEVEL_1 + QAM16_LEVEL_2*_Complex_I;
	table[2] = QAM16_LEVEL_2 + QAM16_LEVEL_1*_Complex_I;
	table[3] = QAM16_LEVEL_2 + QAM16_LEVEL_2*_Complex_I;
	table[4] = QAM16_LEVEL_1 - QAM16_LEVEL_1*_Complex_I;
	table[5] = QAM16_LEVEL_1 - QAM16_LEVEL_2*_Complex_I;
	table[6] = QAM16_LEVEL_2 - QAM16_LEVEL_1*_Complex_I;
	table[7] = QAM16_LEVEL_2 - QAM16_LEVEL_2*_Complex_I;
	table[8] = -QAM16_LEVEL_1 + QAM16_LEVEL_1*_Complex_I;
	table[9] = -QAM16_LEVEL_1 + QAM16_LEVEL_2*_Complex_I;
	table[10] = -QAM16_LEVEL_2 + QAM16_LEVEL_1*_Complex_I;
	table[11] = -QAM16_LEVEL_2 + QAM16_LEVEL_2*_Complex_I;
	table[12] = -QAM16_LEVEL_1 - QAM16_LEVEL_1*_Complex_I;
	table[13] = -QAM16_LEVEL_1 - QAM16_LEVEL_2*_Complex_I;
	table[14] = -QAM16_LEVEL_2 - QAM16_LEVEL_1*_Complex_I;
	table[15] = -QAM16_LEVEL_2 - QAM16_LEVEL_2*_Complex_I;
	for (i=0;i<6;i++) {
		for (j=0;j<32;j++) {
			soft_table->idx[0][i][j] = 0;
			soft_table->idx[1][i][j] = 0;
		}
	}
	if (!compute_soft_demod) {
		return;
	}


	/* Matrices identifying the zeros and ones of LTE-16QAM constellation */
	for (i=0;i<8;i++) {
		soft_table->idx[0][0][i] = i;   /* symbols with a '0' at the bit0 (leftmost)*/
		soft_table->idx[1][0][i] = i+8; /* symbols with a '1' at the bit0 (leftmost)*/
	}
	/* symbols with a '0' ans '1' at the bit position 1: */
	for (i=0;i<4;i++) {
		soft_table->idx[0][1][i] = i;
		soft_table->idx[0][1][i+4] = i+8;
		soft_table->idx[1][1][i] = i+4;
		soft_table->idx[1][1][i+4] = i+12;
	}
	/* symbols with a '0' ans '1' at the bit position 2: */
	for (j=0;j<4;j++) {
		for (i=0;i<2;i++) {
			soft_table->idx[0][2][i+2*j] = i + 4*j;
			soft_table->idx[1][2][i+2*j] = i+2 + 4*j;
		}
	}
	/* symbols with a '0' ans '1' at the bit position 3: */
	for (i=0;i<8;i++) {
		soft_table->idx[0][3][i] = 2*i;
		soft_table->idx[1][3][i] = 2*i+1;
	}
}

/**
 * Set the 64QAM modulation table */
void set_64QAMtable(cf* table, soft_table_t *soft_table, bool compute_soft_demod)
{
	int i,j;
	// LTE-64QAM constellation:
	// see [3GPP TS 36.211 version 10.5.0 Release 10, Section 7.1.4]
	table[0] = QAM64_LEVEL_2 + QAM64_LEVEL_2*_Complex_I;
	table[1] = QAM64_LEVEL_2 + QAM64_LEVEL_1*_Complex_I;
	table[2] = QAM64_LEVEL_1 + QAM64_LEVEL_2*_Complex_I;
	table[3] = QAM64_LEVEL_1 + QAM64_LEVEL_1*_Complex_I;
	table[4] = QAM64_LEVEL_2 + QAM64_LEVEL_3*_Complex_I;
	table[5] = QAM64_LEVEL_2 + QAM64_LEVEL_4*_Complex_I;
	table[6] = QAM64_LEVEL_1 + QAM64_LEVEL_3*_Complex_I;
	table[7] = QAM64_LEVEL_1 + QAM64_LEVEL_4*_Complex_I;
	table[8] = QAM64_LEVEL_3 + QAM64_LEVEL_2*_Complex_I;
	table[9] = QAM64_LEVEL_3 + QAM64_LEVEL_1*_Complex_I;
	table[10] = QAM64_LEVEL_4 + QAM64_LEVEL_2*_Complex_I;
	table[11] = QAM64_LEVEL_4 + QAM64_LEVEL_1*_Complex_I;
	table[12] = QAM64_LEVEL_3 + QAM64_LEVEL_3*_Complex_I;
	table[13] = QAM64_LEVEL_3 + QAM64_LEVEL_4*_Complex_I;
	table[14] = QAM64_LEVEL_4 + QAM64_LEVEL_3*_Complex_I;
	table[15] = QAM64_LEVEL_4 + QAM64_LEVEL_4*_Complex_I;
	table[16] = QAM64_LEVEL_2 - QAM64_LEVEL_2*_Complex_I;
	table[17] = QAM64_LEVEL_2 - QAM64_LEVEL_1*_Complex_I;
	table[18] = QAM64_LEVEL_1 - QAM64_LEVEL_2*_Complex_I;
	table[19] = QAM64_LEVEL_1 - QAM64_LEVEL_1*_Complex_I;
	table[20] = QAM64_LEVEL_2 - QAM64_LEVEL_3*_Complex_I;
	table[21] = QAM64_LEVEL_2 - QAM64_LEVEL_4*_Complex_I;
	table[22] = QAM64_LEVEL_1 - QAM64_LEVEL_3*_Complex_I;
	table[23] = QAM64_LEVEL_1 - QAM64_LEVEL_4*_Complex_I;
	table[24] = QAM64_LEVEL_3 - QAM64_LEVEL_2*_Complex_I;
	table[25] = QAM64_LEVEL_3 - QAM64_LEVEL_1*_Complex_I;
	table[26] = QAM64_LEVEL_4 - QAM64_LEVEL_2*_Complex_I;
	table[27] = QAM64_LEVEL_4 - QAM64_LEVEL_1*_Complex_I;
	table[28] = QAM64_LEVEL_3 - QAM64_LEVEL_3*_Complex_I;
	table[29] = QAM64_LEVEL_3 - QAM64_LEVEL_4*_Complex_I;
	table[30] = QAM64_LEVEL_4 - QAM64_LEVEL_3*_Complex_I;
	table[31] = QAM64_LEVEL_4 - QAM64_LEVEL_4*_Complex_I;
	table[32] = -QAM64_LEVEL_2 + QAM64_LEVEL_2*_Complex_I;
	table[33] = -QAM64_LEVEL_2 + QAM64_LEVEL_1*_Complex_I;
	table[34] = -QAM64_LEVEL_1 + QAM64_LEVEL_2*_Complex_I;
	table[35] = -QAM64_LEVEL_1 + QAM64_LEVEL_1*_Complex_I;
	table[36] = -QAM64_LEVEL_2 + QAM64_LEVEL_3*_Complex_I;
	table[37] = -QAM64_LEVEL_2 + QAM64_LEVEL_4*_Complex_I;
	table[38] = -QAM64_LEVEL_1 + QAM64_LEVEL_3*_Complex_I;
	table[39] = -QAM64_LEVEL_1 + QAM64_LEVEL_4*_Complex_I;
	table[40] = -QAM64_LEVEL_3 + QAM64_LEVEL_2*_Complex_I;
	table[41] = -QAM64_LEVEL_3 + QAM64_LEVEL_1*_Complex_I;
	table[42] = -QAM64_LEVEL_4 + QAM64_LEVEL_2*_Complex_I;
	table[43] = -QAM64_LEVEL_4 + QAM64_LEVEL_1*_Complex_I;
	table[44] = -QAM64_LEVEL_3 + QAM64_LEVEL_3*_Complex_I;
	table[45] = -QAM64_LEVEL_3 + QAM64_LEVEL_4*_Complex_I;
	table[46] = -QAM64_LEVEL_4 + QAM64_LEVEL_3*_Complex_I;
	table[47] = -QAM64_LEVEL_4 + QAM64_LEVEL_4*_Complex_I;
	table[48] = -QAM64_LEVEL_2 - QAM64_LEVEL_2*_Complex_I;
	table[49] = -QAM64_LEVEL_2 - QAM64_LEVEL_1*_Complex_I;
	table[50] = -QAM64_LEVEL_1 - QAM64_LEVEL_2*_Complex_I;
	table[51] = -QAM64_LEVEL_1 - QAM64_LEVEL_1*_Complex_I;
	table[52] = -QAM64_LEVEL_2 - QAM64_LEVEL_3*_Complex_I;
	table[53] = -QAM64_LEVEL_2 - QAM64_LEVEL_4*_Complex_I;
	table[54] = -QAM64_LEVEL_1 - QAM64_LEVEL_3*_Complex_I;
	table[55] = -QAM64_LEVEL_1 - QAM64_LEVEL_4*_Complex_I;
	table[56] = -QAM64_LEVEL_3 - QAM64_LEVEL_2*_Complex_I;
	table[57] = -QAM64_LEVEL_3 - QAM64_LEVEL_1*_Complex_I;
	table[58] = -QAM64_LEVEL_4 - QAM64_LEVEL_2*_Complex_I;
	table[59] = -QAM64_LEVEL_4 - QAM64_LEVEL_1*_Complex_I;
	table[60] = -QAM64_LEVEL_3 - QAM64_LEVEL_3*_Complex_I;
	table[61] = -QAM64_LEVEL_3 - QAM64_LEVEL_4*_Complex_I;
	table[62] = -QAM64_LEVEL_4 - QAM64_LEVEL_3*_Complex_I;
	table[63] = -QAM64_LEVEL_4 - QAM64_LEVEL_4*_Complex_I;

	if (!compute_soft_demod) {
		return;
	}

	/* Matrices identifying the zeros and ones of LTE-64QAM constellation */

	for (i=0;i<32;i++) {
		soft_table->idx[0][0][i] = i;	/* symbols with a '0' at the bit0 (leftmost)*/
		soft_table->idx[1][0][i] = i+32;	/* symbols with a '1' at the bit0 (leftmost)*/
	}
	/* symbols with a '0' ans '1' at the bit position 1: */
	for (i=0;i<16;i++) {
		soft_table->idx[0][1][i] = i;
		soft_table->idx[0][1][i+16] = i+32;
		soft_table->idx[1][1][i] = i+16;
		soft_table->idx[1][1][i+16] = i+48;
	}
	/* symbols with a '0' ans '1' at the bit position 2: */
	for (i=0;i<8;i++) {
		soft_table->idx[0][2][i] = i;
		soft_table->idx[0][2][i+8] = i+16;
		soft_table->idx[0][2][i+16] = i+32;
		soft_table->idx[0][2][i+24] = i+48;
		soft_table->idx[1][2][i] = i+8;
		soft_table->idx[1][2][i+8] = i+24;
		soft_table->idx[1][2][i+16] = i+40;
		soft_table->idx[1][2][i+24] = i+56;
	}
	/* symbols with a '0' ans '1' at the bit position 3: */
	for (j=0;j<8;j++) {
		for (i=0;i<4;i++) {
			soft_table->idx[0][3][i+4*j] = i + 8*j;
			soft_table->idx[1][3][i+4*j] = i+4 + 8*j;
		}
	}
	/* symbols with a '0' ans '1' at the bit position 4: */
	for (j=0;j<16;j++) {
		for (i=0;i<2;i++) {
			soft_table->idx[0][4][i+2*j] = i + 4*j;
			soft_table->idx[1][4][i+2*j] = i+2 + 4*j;
		}
	}
	/* symbols with a '0' ans '1' at the bit position 5: */
	for (i=0;i<32;i++) {
		soft_table->idx[0][5][i] = 2*i;
		soft_table->idx[1][5][i] = 2*i+1;
	}
}
