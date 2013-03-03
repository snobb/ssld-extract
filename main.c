/*  main.c  */

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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "conn.h"

#define VERSION "1.04c"
#define MAX     4096

/*
 * The reason for creating this is to compare python, perl and C
 * implementations (perl uses regexp hence its the slowest version. Even
 * in perl - regexp sucks)
 */

static void parse(const char *fname, const bool show_datetime); /* main parsing logic */
static bool check_next_ssl(const char line[]);
static bool readline(FILE *in, char *str, const int max); 
static void readvalues(char *line, bool iscn);  /* true = conn, false = port */
static void usage(void);
static size_t timestamp_to_date(char *line, size_t maxlen);

/* =================================================================== */
int main(int argc, char **argv)
{
    char *fname = NULL;
    bool show_datetime = false;

    if (argc == 1) {
        usage();
        exit(1);
    }

    conn_init();
    atexit(conn_free);

    for (int i=1; i < argc; i++) { /* reading arguments */
        if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
            switch(argv[i][1]) {
                case 'n': case 'p':
                    readvalues(argv[i+1], argv[i][1] == 'n');
                    i++;
                    break;
                case 'h':
                    usage(); exit(0);
                case 't':
                    show_datetime = true;
                    break;
                default:
                    usage();
                    fputs("ERROR: Invalid argument\n", stderr);
                    exit(1);
            }
        } else {
            fname = argv[i];
        }
    } /* reading arguments */

    parse(fname, show_datetime);
    return 0;
}

/* ===================================================================
 * Main parsing logic
 * Looking for lines starting with following:
 *  1. "Connection #" <== start of the connection
 *  2. <connection number> = next packet in the given connection
 *  3. empty char <== depending on whether we're inside or not (see 2.)
 */
static void parse(const char *fname, const bool show_datetime)
{
    int cn, port;
    FILE *in;
    bool closable = false;
    bool inside = false;
    char line[MAX];

    if (fname && strlen(fname) == 1 && fname[0] == '-') 
        in = stdin;
    else {
        if ((in = fopen(fname, "r")) == NULL) {
            fputs("ERROR: input is not specified\n", stderr);
            exit(1);
        }
        closable = true;
    }

    while (readline(in, line, MAX)) {
        cn = port = 0;
        if (strncmp(line, "New ", 4) == 0) { /* New connection */
            char *strp;
            for (strp = line; *strp != '#' && *strp != '\0'; strp++)
                ;
            sscanf(strp, "#%d:", &cn); 

            for(; *(++strp) != '(' && *strp != '\0';)
                ;
            sscanf(strp, "(%d)", &port);

            if(conn_exists(cn, port)) {
                puts(line);
                inside = true;
            }
        } else if (check_next_ssl(line)) { /* existing conn. (start) */
            sscanf(line, "%d", &cn);
            if (conn_exists(cn, -1)) {
                if(show_datetime) {
                    timestamp_to_date(line,MAX);
                }
                puts(line);
                inside = true;
            } else
                inside = false;
        } else if (isspace(line[0]) && inside) /* existing conn. (cont.) */
            puts(line);
    }
    if (closable)
        fclose(in);
}

/* ===================================================================
 * Checking for condition where a line starts with a number separated
 * by space or tab
 */
static bool check_next_ssl(const char *line)
{
    int i;
    for (i = 0; isdigit(line[i]); i++)
        ;
    return i > 0 && line[i] == ' ';
}


/* ===================================================================
 * reading a line of text from stdin
 */
static bool readline(FILE *in, char *str, const int max)
{
    int i = 0;
    char ch;

    while ((ch = fgetc(in)) != EOF && ch != '\n') {
        if (i >= max) break;
        str[i++] = ch;
    }
    str[i] = '\0';
    return ch != EOF;
}

/* ===================================================================
 * Reading arguments for -c/-p
 * Eg. -c 23,45,67
 */
static void readvalues(char *line, bool iscn)
{
    int value;
    char *strp = line;

    if (!line) {
        puts("ERROR: argument is NULL");
        exit(1);
    }

    /* parsing comma separated parameters */
    while(true) {
        if (isdigit(strp[0])) {
            value = 0;
            if (sscanf(strp, "%d", &value) != EOF) {
                if (iscn)
                    conn_addc(value);
                else
                    conn_addp(value);
            }
            for (; *strp != ',' && *strp != '\0'; strp++)
                ;
            if (*strp == '\0') break;
            strp++;
        }
    }
}

/* =================================================================== */
static void usage(void)
{
    puts(
        "ssld-extract v" VERSION " Alex Kozadaev(c)                              \n\n"
        "ssld-extact [-n x,y | -p n,m] [ssldump filename | - (pipe)]             \n\n"
        "Extract one or more connections from a SSL dump file.                   \n\n"
        "    Usage:                                                              \n"
        "        -n    comma separated list of connections (no spaces allowed)   \n"
        "        -p    comma separated list of ephimeral port (no spaces allowed)\n"
        "        -t    convert unix timestamps to human-readable dates           \n"
        "        -h    this text                                                 \n"
        );
}

/* ===================================================================
 * read the starting line of an existing connection and convert
 * the timestamp to a human-readable date
 */
size_t timestamp_to_date(char *line, size_t maxlen){
    long int seconds;
    char subseconds[10];
    time_t timestamp;
    struct tm tm;
    char *strp,*strp_start;
    char line_format[maxlen];

    /* move strp to the start of the timestamp in the line */
    for (strp=line; isdigit(*strp) && *strp != '\0'; strp++);
    for (; *strp == ' ' && *strp != '\0'; strp++);
    for (; isdigit(*strp) && *strp != '\0'; strp++);
    for (; *strp == ' ' && *strp != '\0'; strp++);
    if (strp == line || *strp == '\0'){
        return -1;  /* unexpected line format (conn# and packet#) */
    }
    strp_start = strp;

    /* timestamp to struct tm with timezone info */
    if (sscanf(strp, "%ld.%9s", &seconds, subseconds) != 2){
        return -2;  /* unexpected line format (timestamp.subseconds) */
    }
    timestamp = (time_t) seconds;
    tm = *localtime(&timestamp);

    /* move strp to the end of the timestamp in the line */
    for(; isdigit(*strp) && *strp != '\0'; strp++);
    strp++;     /* skip the . */
    for(; isdigit(*strp) && *strp != '\0'; strp++);
    for(; *strp == ' ' && *strp != '\0'; strp++);

    /* re-formatting of the line, assumes no % in the original line */
    snprintf(line_format,maxlen,"%%Y-%%m-%%d %%H:%%M:%%S.%s %%Z %s",subseconds,strp);
    strftime(strp_start, maxlen, line_format, &tm);
    return strlen(line);
}

/* vim: ts=4 sts=8 sw=4 smarttab et si ci cino+=t0 list */

