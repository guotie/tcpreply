/* $Id: timer.c 767 2004-10-06 12:48:49Z aturner $ */

/*
 * Copyright (c) 2001-2004 Aaron Turner, Matt Bing.
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

#include "config.h"
#include "timer.h"

/* Miscellaneous timeval routines */

/* Divide tvp by div, storing the result in tvp */
inline void
timerdiv(struct timeval *tvp, float div)
{
    double interval;

    if (div == 0 || div == 1)
        return;

    interval = ((double)tvp->tv_sec * 1000000 + tvp->tv_usec) / (double)div;
    tvp->tv_sec = interval / (int)1000000;
    tvp->tv_usec = interval - (tvp->tv_sec * 1000000);
}

/*
 * converts a float to a timeval structure
 */
inline void
float2timer(float time, struct timeval *tvp)
{
    float n;

    n = time;

    tvp->tv_sec = n;

    n -= tvp->tv_sec;
    tvp->tv_usec = n * 100000;

}
