TARGET = thing
SRC_DIRS = ./src
INC_DIRS = ./include
OBJ_DIRS = ./obj
PREFIX = /usr/local/bin
# LIB_DIRS = include
# LIB_NAMES = boost_filesystem

CXX = clang++
CXXFLAGS = -Wall -O2 -pedantic
IFLAGS = $(patsubst %,-I %,$(INC_DIRS))
LFLAGS := $(patsubst %,-L %,$(LIB_DIRS))
LFLAGS += $(patsubst %,-l %,$(LIB_NAMES))

OBJECTS = $(patsubst $(SRC_DIRS)/%.cpp,$(OBJ_DIRS)/%.o,$(wildcard $(SRC_DIRS)/*.cpp))

.PHONY: default all clean

default: $(TARGET)
all: default

strict: CXXFLAGS = -Werror -Wall -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes -Wpointer-arith -Wwrite-strings -Wcast-qual -Wcast-align -Wbad-function-cast -Wformat-security -Wformat-nonliteral -Wmissing-format-attribute -Winline -O2 -funsigned-char -W -pedantic
strict: $(TARGET)

.PRECIOUS: $(TARGET) $(OBJECTS)

.SECONDARY: $(OBJECTS) $(OBJ_DIRS)

$(OBJECTS): | $(OBJ_DIRS)
$(OBJ_DIRS):
	mkdir $(OBJ_DIRS)

$(OBJ_DIRS)/%.o: $(SRC_DIRS)/%.cpp
	$(CXX) $(IFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LFLAGS) -o $@ $^

clean:
	-rm -rf $(OBJ_DIRS)
	-rm -f $(TARGET)

fresh: clean $(TARGET)

install:
	strip $(TARGET)
	install -t $(PREFIX) $(TARGET)
