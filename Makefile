# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

ifdef IDE
CIDE=--ide ${IDE}
endif

ifdef BOARD
ENV=--environment ${BOARD}
endif

all:
	platformio -f -c vim run ${ENV}

upload:
	platformio -f -c vim run --target upload ${ENV}

clean:
	platformio -f -c vim run --target clean

program:
	platformio -f -c vim run --target program ${ENV}

uploadfs:
	platformio -f -c vim run --target uploadfs ${ENV}

update:
	platformio -f -c vim update

init:
	platformio init --ide vim $(subst environment,board,${ENV})
