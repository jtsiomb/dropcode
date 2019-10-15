/* MD5 message digest
 * Author: John Tsiombikas <nuclear@member.fsf.org>
 *
 * This software is public domain. Feel free to use it any way you like.
 *
 * If public domain is not applicable in your part of the world, you may use
 * this under the terms of the Creative Commons CC-0 license:
 * http://creativecommons.org/publicdomain/zero/1.0/
 */
#include <stdio.h>
#include <string.h>
#include "md5.h"

#define BLOCKSZ 64

static const int shifttab[] = {
	7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
	6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

static const uint32_t sintab[] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static void sum_block(uint32_t *blk, uint32_t *sum);
static uint32_t rol(uint32_t x, int s);

void md5_begin(struct md5_state *md)
{
	md->sum[0] = 0x67452301;
	md->sum[1] = 0xefcdab89;
	md->sum[2] = 0x98badcfe;
	md->sum[3] = 0x10325476;
	md->len = 0;
	md->bblen = 0;
}

void md5_msg(struct md5_state *md, void *msg, int msg_size)
{
	int pending, sz;

	while(msg_size) {
		pending = BLOCKSZ - md->bblen;
		sz = msg_size > pending ? pending : msg_size;

		memcpy(md->blockbuf + md->bblen, msg, sz);
		msg = (char*)msg + sz;
		md->bblen += sz;
		msg_size -= sz;

		if(md->bblen >= BLOCKSZ) {
			sum_block((uint32_t*)md->blockbuf, md->sum);
			md->bblen = 0;
			md->len += BLOCKSZ * 8;
		}
	}
}

static void sum_block(uint32_t *blk, uint32_t *sum)
{
	int i, f, g;
	int a = sum[0];
	int b = sum[1];
	int c = sum[2];
	int d = sum[3];

	for(i=0; i<64; i++) {
		switch(i >> 4) {
		case 0:
			f = d ^ (b & (c ^ d));
			g = i;
			break;

		case 1:
			f = c ^ (d & (b ^ c));
			g = (i * 5 + 1) & 0xf;
			break;

		case 2:
			f = b ^ c ^ d;
			g = (i * 3 + 5) & 0xf;
			break;

		case 3:
			f = c ^ (b | ~d);
			g = (i * 7) & 0xf;
		}

		f += a + sintab[i] + blk[g];
		a = d;
		d = c;
		c = b;
		b += rol(f, shifttab[i]);
	}

	sum[0] += a;
	sum[1] += b;
	sum[2] += c;
	sum[3] += d;
}

static uint32_t rol(uint32_t x, int s)
{
	return (x << s) | (x >> (32 - s));
}

void md5_end(struct md5_state *md)
{
	md->len += md->bblen * 8;
	md->blockbuf[md->bblen++] = 0x80;	/* append 1-bit plus 8 zeros padding */

	if(md->bblen > BLOCKSZ - 8) {
		/* pad to BLOCKSZ, sum, then continue with further padding */
		memset(md->blockbuf + md->bblen, 0, BLOCKSZ - md->bblen);
		sum_block((uint32_t*)md->blockbuf, md->sum);
		md->bblen = 0;
	}

	/* pad to BLOCKSZ - 8 */
	memset(md->blockbuf + md->bblen, 0, BLOCKSZ - 8 - md->bblen);

	/* add the length */
	*(uint64_t*)(md->blockbuf + BLOCKSZ - 8) = md->len;

	/* then sum for the last time */
	sum_block((uint32_t*)md->blockbuf, md->sum);
}

const char *md5_sumstr(struct md5_state *md)
{
	int i;
	unsigned char *sum = (unsigned char*)md->sum;
	char *s = (char*)md->blockbuf;

	for(i=0; i<16; i++) {
		s += sprintf(s, "%02x", sum[i]);
	}
	return (char*)md->blockbuf;
}
