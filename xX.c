/* $Id: xX.c 767 2004-10-06 12:48:49Z aturner $ */

/*
 * Copyright (c) 2001-2004 Aaron Turner.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright owners nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * xX stands for "include or exclude" which is used with the 
 * -x and -X flags
 *
 * Functions for use to process args for or check data against in
 * tcpreplay/do_packets and tcpprep.
 */

#include "config.h"
#include "tcpreplay.h"
#include "cidr.h"
#include "list.h"
#include "xX.h"
#include "err.h"

extern struct options options;


/*
 * returns the include_exclude_mode on success placing the CIDR or LIST in mybuf
 * but on failure, returns 0
 */

int
parse_xX_str(char mode, char *str, void **mybuf)
{
    int bpf = 0;
    int out = 0;

    dbg(1, "Parsing string: %s", str);
    dbg(1, "Switching on: %c", str[0]);

    switch (str[0]) {
    case 'B':                  /* both ip's */
        str = str + 2;
        out = xXBoth;
        if (!parse_cidr((CIDR **)mybuf, str, ","))
            return 0;
        break;
    case 'D':                  /* dst ip */
        str = str + 2;
        out = xXDest;
        if (!parse_cidr((CIDR **)mybuf, str, ","))
            return 0;
        break;
    case 'E':                  /* either ip */
        str = str + 2;
        out = xXEither;
        if (!parse_cidr((CIDR **)mybuf, str, ","))
            return 0;
        break;
    case 'F':                  /* bpf filter */
        bpf = 1;
        str = str + 2;
        out = xXBPF;
        options.bpf_filter = str;
        /* note: it's temping to compile the BPF here, but we don't
         * yet know what the link type is for the file, so we have 
         * to compile the BPF once we open the pcap file
         */
        break;
    case 'P':                  /* packet id */
        str = str + 2;
        out = xXPacket;
        if (!parse_list((LIST **)mybuf, str))
            return 0;
        break;
    case 'S':                  /* source ip */
        str = str + 2;
        out = xXSource;
        if (!parse_cidr((CIDR **)mybuf, str, ","))
            return 0;
        break;


    default:
        errx(1, "Invalid -%c option: %c", mode, *str);
        break;
    }

    if (mode == 'X') {          /* run in exclude mode */
        out += xXExclude;
        if (bpf)
            errx(1, "Using a BPF filter with -X doesn't work.\n"
                 "Try using -xF:\"not <filter>\" instead");
    }

    return out;
}



/*
 * compare the source/destination IP address according to the mode
 * and return 1 if we should send the packet or 0 if not
 */


int
process_xX_by_cidr(int mode, CIDR * cidr, ip_hdr_t * ip_hdr)
{

    if (mode & xXExclude) {
        /* Exclude mode */
        switch (mode ^ xXExclude) {
        case xXSource:
            return check_ip_CIDR(cidr, ip_hdr->ip_src.s_addr) ? 0 : 1;
            break;
        case xXDest:
            return check_ip_CIDR(cidr, ip_hdr->ip_dst.s_addr) ? 0 : 1;
        case xXBoth:
            return (check_ip_CIDR(cidr, ip_hdr->ip_dst.s_addr) &&
                    check_ip_CIDR(cidr, ip_hdr->ip_src.s_addr)) ? 0 : 1;
            break;
        case xXEither:
            return (check_ip_CIDR(cidr, ip_hdr->ip_dst.s_addr) ||
                    check_ip_CIDR(cidr, ip_hdr->ip_src.s_addr)) ? 0 : 1;
            break;
        }
    }
    else {
        /* Include Mode */
        switch (mode) {
        case xXSource:
            return check_ip_CIDR(cidr, ip_hdr->ip_src.s_addr) ? 1 : 0;
            break;
        case xXDest:
            return check_ip_CIDR(cidr, ip_hdr->ip_dst.s_addr) ? 1 : 0;
            break;
        case xXBoth:
            return (check_ip_CIDR(cidr, ip_hdr->ip_dst.s_addr) &&
                    check_ip_CIDR(cidr, ip_hdr->ip_src.s_addr)) ? 1 : 0;
            break;
        case xXEither:
            return (check_ip_CIDR(cidr, ip_hdr->ip_dst.s_addr) ||
                    check_ip_CIDR(cidr, ip_hdr->ip_src.s_addr)) ? 1 : 0;
            break;
        }
    }

    /* total failure */
    warnx("Unable to determine action in CIDR filter mode");
    return 0;

}
