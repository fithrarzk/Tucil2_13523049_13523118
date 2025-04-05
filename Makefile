# Compiler dan flags
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -I./src/header -I./src/external-libs

# Direktori
SRC_DIR = src
OBJ_DIR = bin
BIN = $(OBJ_DIR)/image_compressor

# Daftar semua source files
SRCS = $(SRC_DIR)/ErrorCalculator.cpp \
       $(SRC_DIR)/IO.cpp \
       $(SRC_DIR)/QuadTree.cpp \
       $(SRC_DIR)/Reconstruct.cpp \
       $(SRC_DIR)/Statistics.cpp \
       $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/external-libs/stb_img.cpp

# Mapping source jadi object
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter-out $(SRC_DIR)/external-libs/%.cpp, $(SRCS))) \
       $(patsubst $(SRC_DIR)/external-libs/%.cpp, $(OBJ_DIR)/external-libs/%.o, $(filter $(SRC_DIR)/external-libs/%.cpp, $(SRCS)))

# Target utama
all: prepare $(BIN)

# Membuat binary dari semua object files
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Kompilasi file biasa dari src/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Kompilasi file dari src/external-libs/
$(OBJ_DIR)/external-libs/%.o: $(SRC_DIR)/external-libs/%.cpp
	@mkdir -p $(OBJ_DIR)/external-libs
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Buat folder bin dan subfolder external-libs
prepare:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/external-libs

# Bersihkan hasil kompilasi
clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/external-libs/*.o $(BIN)

.PHONY: all clean prepare
