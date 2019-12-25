/* MD5 message digest
 * Author: John Tsiombikas <nuclear@member.fsf.org>
 *
 * This software is public domain. Feel free to use it any way you like.
 *
 * If public domain is not applicable in your part of the world, you may use
 * this under the terms of the Creative Commons CC-0 license:
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 *
 * usage example 1: compute the md5 sum of the string "hello world"
 *
 *    struct md5_state md;
 *    md5_begin(&md);
 *    md5_msg(&md, "hello world", 11);
 *    md5_end(&md);
 *
 * md5 sum in: md5_state.sum[0..4]
 * To print it you may use the helper function md5_sumstr:
 *    printf("%s\n", md5_sumstr(&md));
 *
 * usage example 2: comput the md5 sum of whatever comes through stdin
 *
 *     int sz;
 *     struct md5_state md;
 *     char buf[1024];
 *
 *     md5_begin(&md);
 *     while((sz = fread(buf, 1, sizeof buf, stdin)) > 0) {
 *         md5_msg(&md, buf, sz);
 *     }
 *     md5_end(&md);
 */
#ifndef NUCLEAR_DROPCODE_MD5_H_
#define NUCLEAR_DROPCODE_MD5_H_

#include <stdint.h>

struct md5_state {
	uint32_t sum[4];
	uint64_t len;
	unsigned char blockbuf[64];
	int bblen;
};

void md5_begin(struct md5_state *md);
void md5_msg(struct md5_state *md, void *msg, int msg_size);
void md5_end(struct md5_state *md);

/* after computing the md5 sum, you may use this function to get a pointer
 * to a canonical string representation of the sum (identical to GNU md5sum)
 */
const char *md5_sumstr(struct md5_state *md);

#endif	/* NUCLEAR_DROPCODE_MD5_H_ */
