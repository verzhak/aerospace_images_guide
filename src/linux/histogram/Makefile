
OUTPUT_DIR = build
FNAME = histogram
RELEASE_OUTPUT_FNAME = $(OUTPUT_DIR)/$(FNAME)
DEBUG_OUTPUT_FNAME = $(RELEASE_OUTPUT_FNAME)_debug

TEST_DIR = test

SRC = main.cpp

FLAGS = -Wall -pipe $(shell gdal-config --cflags)
FLAGS_DEBUG = -g
FLAGS_RELEASE = -O2

INCLUDE = -I../lib

LIB = $(shell gdal-config --libs)

debug: clean

	g++ $(FLAGS) $(FLAGS_DEBUG) $(LIB) $(INCLUDE) $(SRC) -o $(DEBUG_OUTPUT_FNAME)

release: clean

	g++ $(FLAGS) $(FLAGS_RELEASE) $(LIB) $(INCLUDE) $(SRC) -o $(RELEASE_OUTPUT_FNAME)

clean:

	rm -f $(TEST_DIR)/* $(OUTPUT_DIR)/*
	mkdir -p $(TEST_DIR) $(OUTPUT_DIR)/

distclean:

	rm -Rf $(TEST_DIR) $(OUTPUT_DIR)

