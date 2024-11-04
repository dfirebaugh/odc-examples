CC = gcc
CXX = g++
CFLAGS = -Iexternal/odc/include -Iexternal/stb -g $(shell pkg-config --cflags freetype2) -Wall -Wextra -std=c11
CXXFLAGS = -Iexternal/odc/include -Iexternal/odc/external/glad/include -Wall -Wextra -std=c++11
LDFLAGS = -Lexternal/odc/build/lib -Wl,-rpath=external/odc/build/lib -lodc -lglfw -lportaudio -lfreetype -lpthread -ldl -lm

SRC_DIR = examples
OBJ_DIR = obj
BIN_DIR = bin
ODC_DIR = external/odc

EXAMPLES = balls buddymark example gol sound simple

all: $(EXAMPLES)

$(EXAMPLES): %: $(OBJ_DIR)/%.o $(ODC_DIR)/build/lib/libodc.so
	@mkdir -p $(BIN_DIR)
	$(CC) $< -o $(BIN_DIR)/$@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ODC_DIR)/build/lib/libodc.so:
	@mkdir -p $(ODC_DIR)/build
	$(MAKE) -C $(ODC_DIR)

clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean $(EXAMPLES)
