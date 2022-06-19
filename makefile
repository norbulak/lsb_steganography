CC=gcc
CFLAGS=-DDBUG=0
DBFLAGS=-O0 -DDBUG=1 -g -ggdb3
SRC= main.c\
	 encode.c\
	 decode.c

MKD=mkdir -p 
BUILD_DIR=Release
DEBUG_DIR=Debug
TEST_BMP=Resources/yet_another.bmp
TEST_SECRET=Resources/secret.jpg
TEST_OUTPUT=test.bmp
IMAGE_VIEWER=viewnior
BIN=stego

all:
	$(MKD) $(BUILD_DIR)
	$(CC) $(SRC) -o ./$(BUILD_DIR)/$(BIN) $(CFLAGS)
debug:
	$(MKD) $(DEBUG_DIR)
	$(CC) $(SRC) -o $(DEBUG_DIR)/$(BIN) $(DBFLAGS)
test:debug
	@echo "###################################################################"
	@echo "TESTING ENCODING"
	@echo "###################################################################"
	./$(DEBUG_DIR)/$(BIN) -e $(TEST_BMP) $(TEST_SECRET) $(TEST_OUTPUT) 
	@echo "###################################################################"
	@echo "TESTING DECODING"
	@echo "###################################################################"
	./$(DEBUG_DIR)/$(BIN) -d $(TEST_OUTPUT)
test_release:all
	@echo "####################HIDING JPEG INSIDE BMP#########################"
	@echo "###################################################################"
	@echo "TESTING ENCODING"
	@echo "###################################################################"
	./$(BUILD_DIR)/$(BIN) -e $(TEST_BMP) $(TEST_SECRET) $(TEST_OUTPUT) 
	@echo "###################################################################"
	@echo "TESTING DECODING"
	@echo "###################################################################"
	./$(BUILD_DIR)/$(BIN) -d $(TEST_OUTPUT) 
	$(IMAGE_VIEWER) output.jpg
clean:
	rm -fr $(TEST_OUTPUT) $(DEBUG_DIR) $(BUILD_DIR) output.*
