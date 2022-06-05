CC=gcc
CFLAGS=
DBFLAGS=-DDBG -g -ggdb3
SRC= main.c\
	 encode.c

MKD=mkdir -p 
BUILD_DIR=Release
DEBUG_DIR=Debug
TEST_BMP=Resources/beautiful.bmp
TEST_SECRET=Resources/secret.txt
TEST_OUTPUT=$(DEBUG_DIR)/test.bmp
IMAGE_VIEWER=viewnior
BIN=stego

all:
	$(MKD) $(BUILD_DIR)
	$(CC) $(SRC) -o ./$(BUILD_DIR)/$(BIN) $(CFLAGS)
debug:
	$(MKD) $(DEBUG_DIR)
	$(CC) $(SRC) -o $(DEBUG_DIR)/$(BIN) $(CFLAGS) $(DBFLAGS)
test:debug
	./$(DEBUG_DIR)/$(BIN) -e $(TEST_BMP) $(TEST_SECRET) $(TEST_OUTPUT) 
	$(IMAGE_VIEWER) $(TEST_OUTPUT)
clean:
	rm -fr $(DEBUG_DIR) $(BUILD_DIR)
