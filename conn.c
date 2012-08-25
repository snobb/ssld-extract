/*  conn.c  */
/*  Copyright (C) 2012 Alex Kozadaev [akozadaev at yahoo com]  */

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

