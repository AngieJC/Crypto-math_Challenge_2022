EXECS=BurpDiff
MPIXX?=/usr/bin/mpicxx

all: ${EXECS}

BurpDiff: BurpDiff.cpp
    ${MPIXX} -o BurpDiff BurpDiff.cpp

clean:
    rm -f ${EXECS}
