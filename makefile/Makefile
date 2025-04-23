PROJECT_NAME := test
VERSION := 1.0.0
DATE := $(shell date +%Y%m%d)
BIN := xxx_$(DATE)_$(VERSION)
MAP := $(BIN).map

HOST := arm-linux-
CXX := $(HOST)g++

SRC_DIR :=
INC_DIR :=
LD_DIR :=

INC_PATH := $(foreach dir,$(INC_DIR),-I$(dir))
CXX_BASE_FLAGS := -Wall -Wextra -Werror -fPIC
CXX_FLAGS := $(CXX_BASE_FLAGS) -std=c++11 -g -O2
CXX_FLAGS += -MMP -MD 
CXX_FLAGS += $(INC_PATH)

LD_PATH := $(foreach dir,$(LD_DIR),-L$(dir))
LD_LIB := -lstdc++ -lm
LD_BASE_FLAGS := -Wl,-Map,$(MAP) -Wl,--gc-sections

LD_FLAGS := $(LD_PATH) $(LD_LIB) $(LD_BASE_FLAGS)

SRC := $(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.cpp))
OBJ := $(SRC:%.cpp=%.o)
DEP := $(OBJ:%.o=%.d)

.PHONY: all package clean
all: $(BIN) package

$(BIN): $(OBJ)
	@echo "Linking $@"
	$(CXX) -o $@ $^ $(LD_FLAGS)

%.o: %.cpp
	@echo "Compiling $<"
	$(CXX) $(CXX_FLAGS) -c $< -o $@

-include $(DEP)

package: $(BIN)
	@echo "Packaging..."
	
clean:
	@echo "Cleaning up..."
	@rm -f $(OBJ) $(DEP) $(BIN) $(MAP)

# 支持并行编译
MAKEFLAGS += -j$(shell nproc)
