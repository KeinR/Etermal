include objects.mk
# OBJS = bin/glad.o bin/shell.Command.o bin/shell.Shell.o bin/terminal.gui.Image.o bin/terminal.gui.Rectangle.o bin/terminal.gui.ScrollBox.o bin/terminal.gui.Text.o bin/terminal.gui.TextInput.o bin/terminal.render.Buffer.o bin/terminal.render.Color.o bin/terminal.render.Font.o bin/terminal.render.Model.o bin/terminal.render.Texture.o bin/terminal.Resources.o bin/terminal.shader.Primitive.o bin/terminal.shader.Shader.o bin/terminal.shader.Texture.o bin/terminal.Terminal.o bin/terminal.util.error.o bin/terminal.util.singleton.o bin/terminal.util.util.o
CFLAGS = -Wall -g -Iextern `pkg-config --cflags freetype2`
LIBS = -lglfw3dll -lfreetype.dll
CC = g++

MK = g++ $(CFLAGS)
OUT = term.exe

all: $(OUT)

run: all
	@./$(OUT)

$(OUT): bin/ $(OBJS)
	$(MK) $(OBJS) $(LIBS) -o $@

include depconfig.mk

bin/:
	mkdir bin/

$(OBJS):
	$(MK) -c $< -o $@

clean:
	rm -f $(OBJS)

clean-all: clean
	rm -f $(OUT)
