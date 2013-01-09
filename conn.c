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

static struct conn_st conn = { NULL, NULL, 0, INIT };

/*=============================================================================*/
bool
conn_exists(int cn, int port)
{
  for (int i=0; i < conn.size; i++)
    if (conn.cn[i] == cn || conn.port[i] == port) {
      conn.cn[i] = cn;
      return true;
    }
  return false;
}

/*=============================================================================*/
bool
conn_add(int cn, int port)
{
  conn.cn[conn.size] = cn;
  conn.port[conn.size++] = port;

  if (conn.size >= conn.max) {
    conn.cn = realloc(conn.cn, conn.max *= 2);
    conn.port = realloc(conn.port, conn.max);
    if (conn.cn == NULL || conn.port == NULL) {
      fputs("ERROR: realloc failed", stderr);
      exit(1);
    }
  }
  return true;
}

/*=============================================================================*/
void
conn_init()
{
  conn.cn = calloc(INIT, sizeof(*conn.cn));
  conn.port = calloc(INIT, sizeof(*conn.port));
  if (conn.cn == NULL || conn.port == NULL) {
    fputs("ERROR: calloc failed", stderr);
    exit(1);
  }
  conn.size = 0;
  conn.max = INIT;
}

/*=============================================================================*/
void
conn_free()
{
  free(conn.cn);
  free(conn.port);
}

/*  EOF:  conn.c */

