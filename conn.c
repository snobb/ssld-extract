/*  conn.c  */

/*  Copyright (C) 2012 Alex Kozadaev [akozadaev at yahoo com]
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *  * Neither the name of the nor the names of its
 *  contributors may be used to endorse or promote products derived from
 *  this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "conn.h"

static int *connp;         /* connection numbers */
static int *portp;         /* ephimeral ports */
static int size, max;      /* current/max size of the array */

/* =================================================================== */
bool conn_exists(int conn, int port)
{
    for (int i=0; i < size; i++)
        if (connp[i] == conn || portp[i] == port) {
            connp[i] = conn;
            return true;
        }
    return false;
}

/* =================================================================== */
bool conn_add(int conn, int port)
{
    connp[size] = conn;
    portp[size++] = port;

    if (size >= max) {
        connp = realloc(connp, max *= 2);
        portp = realloc(portp, max);
        if (connp == NULL || portp == NULL) {
            fputs("ERROR: realloc failed", stderr);
            exit(1);
        }
    }
    return true;
}

/* =================================================================== */
void conn_init(void)
{
    connp = calloc(INIT, sizeof(*connp));
    portp = calloc(INIT, sizeof(*portp));
    if (connp == NULL || portp == NULL) {
        fputs("ERROR: calloc failed", stderr);
        exit(1);
    }
    size = 0;
    max = INIT;
}

/* =================================================================== */
void conn_free(void)
{
    free(connp);
    free(portp);
}

/* vim: ts=4 sts=8 sw=4 smarttab et si ci cino+=t0 list */

