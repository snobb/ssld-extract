ssld-extract v1.04b, Alex Kozadaev(c)

SSLDump filtering tool

    ssld-extact [-n x,y | -p n,m] [-c] [-t] [ssldump filename | - (pipe)]             

    Extract one or more connections from a SSL dump file.

    Usage:
      -n    comma separated list of connection numbers (no spaces allowed)
      -p    comma separated list of client port numbers (no spaces allowed)
      -t    convert unix timestamps to human-readable dates
      -c    use colours
      -h    this text

    PLEASE NOTE: -n overrides -p, so when used together -p will not have any effect.

fixed in v1.04b:

    1. properly handling mixed captures 

fixed in v1.04:

    1. added boundry checking when reading the connection line. This would prevent 
       SIGSEGV crushes on malformed ssldump captures


Also encluded:

    ssld-extract.py (python edition) v0.12e, Alex Kozadaev(c)
    ssld-extract.pl (perl edition) v0.22, Alex Kozadaev(c)


