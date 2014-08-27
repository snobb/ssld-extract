#!/usr/bin/env python
#
# { ssld-extract.py }
# Copyright (C) 2012-2013 Alex Kozadaev [akozadaev at yahoo com]
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.#

import sys, getopt, time

conf = { "ports": [], "conns": [], "date": False, "colour": False }

version = "0.12j" # python < 2.7 compatible

# Styles
BOLD_SET = "\x1B[1m"
BOLD_UNSET = "\x1B[22m"

# Colours
getcolour = lambda c: "\033[3%dm" % ((c % 6) + 1)
CLEAR   = "\033[0m"

def parse(infile):
    """ main parsing logic """
    needclose = False
    inside = False
    cl = 0

    if infile == "-":
        fh = sys.stdin
    else:
        try:
            fh = open(infile, "r")
        except IOError:
            die("cannot open file %s" % infile)
        needclose = True

    # main logic here
    for line in fh:
        if line[:4] == "New ":
            try:
                port = line[ line.index("(")+1 : line.index(")") ]
                conn = line[ line.index("#")+1 : line.index(":") ]
            except ValueError:
                warn("cannot parse connection record")
                next

            if ((conn.isdigit() and int(conn) in conf["conns"]) or
                (port.isdigit() and int(port) in conf["ports"])):
                ci = int(conn)
                if not ci in conf["conns"]:
                    conf["conns"].append(ci)
                if conf["colour"]:
                    cl = conf["conns"].index(int(conn))
                cprint(line, getcolour(cl), bold=True)
                inside = True
        elif line[0].isspace():
            if inside: print line,
        elif line[0].isdigit():
            try:
                conn = line[: line.index(" ") ]
            except ValueError:
                inside = False
                next
            if conn.isdigit() and int(conn) in conf["conns"]:
                if conf["date"]:
                    line = replace_date(line)
                if conf["colour"]:
                    cl = conf["conns"].index(int(conn))
                cprint(line, getcolour(cl))
                inside = True
            else:
                inside = False

    if needclose:
        fh.close()

def replace_date(line):
    """ replace the unix timestamp with the human readable datetime """
    unx, start = "", 0
    try:
        for idx, ch in enumerate(line):
            if ch.isdigit():
                if start == 0: start = idx
                unx += ch
            else:
                if len(unx) >= 10:
                    break
                unx, start = "", 0
        date = time.strftime("%d-%m-%Y %H:%M:%S", time.localtime(int(unx)))
        end = start + len(unx) + 5
        date += line[end-5:end]
        return line[:start] + " [" + date + "] " + line[end:]
    except ValueError, TypeError:
        return line

def readargs():
    if (len(sys.argv) == 1): usage()

    sys.argv.pop(0)
    opts, infile = getopt.getopt(sys.argv, "hctp:n:")

    for k,v in opts:
        if k == "-p":
            readvalues(v, "ports")
        elif k == "-n":
            readvalues(v, "conns")
        elif k == "-t":
            conf["date"] = True
        elif k == "-c":
            conf["colour"] = True
        elif k == "-h":
            usage()
            exit()
    if len(infile) == 0:
        die("no source given")

    return infile[0]

def readvalues(values, cfglst):
    for p in values.split(","):
        if p.isdigit():
            if not int(p) in conf[cfglst]:
                conf[cfglst].append(int(p))
        else:
            die("Invalid format %s" % p)

def cprint(msg, colour, bold=False, stream=sys.stdout):
    if conf["colour"]:
        if bold:
            print >> stream, (BOLD_SET + colour + msg.rstrip("\n") +
                              CLEAR + BOLD_UNSET)
        else:
            print >> stream, colour + msg.rstrip("\n") + CLEAR
    else:
        print >> stream, msg,

def warn(msg):
    cprint("error: " + msg, colour(0), stream = sys.stderr)

def die(msg):
    warn(msg)
    exit(1)

def usage():
    print """ssld-extract (python-edition) v%s

    ssld-extact [-n x,y | -p n,m] [-d] [-c] [ssldump filename | - (pipe)]

    Extract one or more connections from a SSL dump file.

        Usage:
            -n    comma separated list of connections (no spaces allowed)
            -p    comma separated list of client ports (no spaces allowed)
            -t    convert unix timestamps
            -c    colourize output
            -h    this text

    Firstly python version was as twise as slower then the perl version.
    But after refactoring I managed  to make it even faster. I tested both
    versions by parsing 173 megabyte ssldump file and python was ~0.7sec
    faster this time.\n """ % version
    exit(0)

if __name__ == "__main__":
    try:
        parse(readargs())
    except KeyboardInterrupt:
        print >> sys.stderr, "\ninterrupted..."
        exit(1)
    except IOError, e:
        exit(0) # SIGPIPE handling

# vim: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab ai expandtab list
