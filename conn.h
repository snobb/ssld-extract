#ifndef CONN_H
#define CONN_H 

/*    Desclarations    */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INIT 10

/* convenience macros for adding stuff to the configuration */
#define conn_addc(c) (conn_add(c, 0))
#define conn_addp(p) (conn_add(0, p))

struct conn_st {
  int *cn;       /* connection numbers */
  int *port;     /* ephimeral ports */
  int  size;     /* current size of the array */
  int  max;      /* maximum size (extended when reached - realloc) */
};

/* initialize global structure */
void conn_init();                    

/* check if either conn number or port exists in the records */
bool conn_exists(int cn, int port);

/* add a new record - there are two macros for that for conns and ports */
bool conn_add(int cn, int port);

/* cleanup the structure */
void conn_free();

#endif /* CONN_H */
