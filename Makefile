include objects.mk
CFLAGS = -Wall -g -Iextern `pkg-config --cflags freetype2`
LIBS = -lglfw3dll -lfreetype.dll -lstb
CC = g++

MK = g++ $(CFLAGS)
OUT = term.exe

all: $(OUT)

run: all
	@./$(OUT)

$(OUT): bin/ $(OBJS)
	$(MK) $(OBJS) $(LIBS) -o $@

include depconfig.mk

resources/shaders/%.h: resources/shaders/%.vert resources/shaders/%.frag
	./shaders.sh

bin/:
	mkdir bin/

$(OBJS):
	$(MK) -c $< -o $@

clean:
	rm -f $(OBJS)

clean-all: clean
	rm -f $(OUT)
