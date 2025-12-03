PSUEODAPPNAME = a_hexagon
SRC_DIR = src/$(PSUEODAPPNAME)

default: all

all:
	@$(MAKE) -f ./makefile_white.mk exec --no-print-directory SRC_DIR=$(SRC_DIR) PSUEODAPPNAME=$(PSUEODAPPNAME)

.PHONY: clean
clean:
	@$(MAKE) -f ./makefile_white.mk clean SRC_DIR:=$(SRC_DIR) --no-print-directory 
