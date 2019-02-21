TARGET=tofu

COMPILER=cc
CWARNINGS=-Wall -Wextra -Werror -Wno-unused-parameter
#CFLAGS=-O0 -DDEBUG -g -D_DEFAULT_SOURCE -std=c99 -Iexternal
CFLAGS=-O2 -g -D_DEFAULT_SOURCE -std=c99 -Iexternal

LINKER=cc
LFLAGS=-Wall -Wextra -Werror -Lexternal/raylib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SOURCES:= $(wildcard src/*.c  src/modules/*.c external/jsmn/*.c external/wren/*.c)
INCLUDES:= $(wildcard src/*.h src/modules/*.h external/jsmn/*.h external/wren/*.h external/raylib/*.h)
OBJECTS:= $(SOURCES:%.c=%.o)
rm=rm -f

default: $(TARGET)
all: default

$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): %.o : %.c $(INCLUDES)
	@$(COMPILER) $(CFLAGS) $(CWARNINGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

bunnymark: $(TARGET)
	@echo "Launching Bunnymark application!"
	./$(TARGET) ./demos/bunnymark

fire: $(TARGET)
	@echo "Launching Fire application!"
	./$(TARGET) ./demos/fire

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(TARGET)
	@echo "Executable removed!"