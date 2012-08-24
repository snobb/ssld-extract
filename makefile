TARGET = ssld-extract 
SRC = ${wildcard *.c}
OBJ = ${SRC:.c=.o}
CC ?= cc
CFLAGS = -Wall 
LFLAGS =
STRIP = strip
INSTALL = install
INSTALL_ARGS = -o root -g wheel -m 755 
INSTALL_DIR = /usr/local/bin/

ifeq (${CC}, cc)
	CFLAGS += -std=c99 -pedantic
endif

all: debug

debug: CFLAGS += -g -DDEBUG
debug: LFLAGS += -g
debug: build

release: CFLAGS += -Os
release: LFLAGS += 
release: clean build
ifeq (${CC}, cc)
	${STRIP} ${TARGET}
endif

build: build_host.h ${TARGET}

build_host.h:
	@echo "#define BUILD_HOST \"`hostname`\""      > build_host.h
	@echo "#define BUILD_OS \"`uname`\""          >> build_host.h
	@echo "#define BUILD_PLATFORM \"`uname -m`\"" >> build_host.h
	@echo "#define BUILD_KERNEL \"`uname -r`\""   >> build_host.h

${TARGET}: build_host.h ${OBJ}
	${CC} ${LFLAGS} -o $@ ${OBJ}	

install: release
	${INSTALL} ${INSTALL_ARGS} ${TARGET} ${INSTALL_DIR}
	@echo "DONE"

clean:
	-rm -f build_host.h 
	-rm -f *.o ${TARGET}

.PHONY : all debug release build install clean 
