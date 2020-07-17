bin/EShell.o: src/EShell.cpp src/EShell.h
bin/ETerminal.o: src/ETerminal.cpp src/ETerminal.h
bin/main.o: src/main.cpp src/EShell.h src/ETerminal.h src/shell/ArgFilter.h src/shell/Args.h src/shell/Shell.h src/shell/data/ArgData.h src/terminal/Line.h src/terminal/Resources.h src/terminal/Scroll.h src/terminal/Terminal.h src/terminal/TextBuffer.h src/terminal/gui/Rectangle.h src/terminal/gui/Scrollbar.h src/terminal/render/Buffer.h src/terminal/render/Color.h src/terminal/render/Font.h src/terminal/render/FontLibrary.h src/terminal/render/Model.h src/terminal/render/Texture.h src/terminal/render/ftype.h src/terminal/shader/Primitive.h src/terminal/shader/Shader.h src/terminal/shader/Text.h src/terminal/textmods/Mod.h src/terminal/util/Timer.h src/terminal/util/enums.h src/terminal/util/singleton.h src/terminal/util/util.h
bin/shell.ArgFilter.o: src/shell/ArgFilter.cpp src/shell/ArgFilter.h src/shell/Args.h src/shell/data/ArgData.h src/shell/data/data.h
bin/shell.Args.o: src/shell/Args.cpp src/shell/Args.h src/shell/data/ArgData.h
bin/shell.data.ArgData.o: src/shell/data/ArgData.cpp src/shell/data/ArgData.h
bin/shell.data.data.o: src/shell/data/data.cpp src/shell/data/ArgData.h src/shell/data/data.h
bin/shell.error.o: src/shell/error.cpp src/shell/error.h
bin/shell.Shell.o: src/shell/Shell.cpp src/EShell.h src/ETerminal.h src/shell/ArgFilter.h src/shell/Args.h src/shell/Shell.h src/shell/data/ArgData.h
bin/terminal.gui.Rectangle.o: src/terminal/gui/Rectangle.cpp src/terminal/Resources.h src/terminal/gui/Rectangle.h src/terminal/render/Buffer.h src/terminal/render/Color.h src/terminal/render/Font.h src/terminal/render/FontLibrary.h src/terminal/render/Model.h src/terminal/render/Texture.h src/terminal/render/ftype.h src/terminal/shader/Primitive.h src/terminal/shader/Shader.h src/terminal/shader/Text.h src/terminal/util/singleton.h
bin/terminal.gui.Scrollbar.o: src/terminal/gui/Scrollbar.cpp src/terminal/Scroll.h src/terminal/gui/Rectangle.h src/terminal/gui/Scrollbar.h src/terminal/render/Color.h src/terminal/render/Model.h
bin/terminal.Line.o: src/terminal/Line.cpp src/terminal/Line.h src/terminal/env.h
bin/terminal.render.Buffer.o: src/terminal/render/Buffer.cpp src/terminal/render/Buffer.h src/terminal/render/glfw.h
bin/terminal.render.Color.o: src/terminal/render/Color.cpp src/terminal/render/Color.h src/terminal/render/glfw.h src/terminal/shader/Shader.h src/terminal/util/singleton.h
bin/terminal.render.Font.o: src/terminal/render/Font.cpp src/terminal/render/Font.h src/terminal/render/FontLibrary.h src/terminal/render/Texture.h src/terminal/render/ftype.h src/terminal/render/glfw.h src/terminal/util/error.h src/terminal/util/singleton.h
bin/terminal.render.FontLibrary.o: src/terminal/render/FontLibrary.cpp src/terminal/render/FontLibrary.h src/terminal/render/ftype.h src/terminal/util/error.h src/terminal/util/singleton.h src/terminal/util/util.h
bin/terminal.render.Model.o: src/terminal/render/Model.cpp src/terminal/render/Model.h src/terminal/render/glfw.h src/terminal/shader/Shader.h src/terminal/util/singleton.h
bin/terminal.render.Texture.o: src/terminal/render/Texture.cpp src/terminal/render/Texture.h src/terminal/render/glfw.h src/terminal/util/error.h
bin/terminal.Resources.o: src/terminal/Resources.cpp src/terminal/Resources.h src/terminal/render/Buffer.h src/terminal/render/Font.h src/terminal/render/FontLibrary.h src/terminal/render/Texture.h src/terminal/render/ftype.h src/terminal/shader/Primitive.h src/terminal/shader/Shader.h src/terminal/shader/Text.h src/terminal/util/error.h src/terminal/util/singleton.h
bin/terminal.Scroll.o: src/terminal/Scroll.cpp src/terminal/Resources.h src/terminal/Scroll.h src/terminal/render/Buffer.h src/terminal/render/Font.h src/terminal/render/FontLibrary.h src/terminal/render/Texture.h src/terminal/render/ftype.h src/terminal/shader/Primitive.h src/terminal/shader/Shader.h src/terminal/shader/Text.h src/terminal/util/singleton.h
bin/terminal.shader.Primitive.o: src/terminal/shader/Primitive.cpp resources/shaders/primitive.h src/terminal/render/glfw.h src/terminal/shader/Primitive.h src/terminal/shader/Shader.h src/terminal/util/error.h src/terminal/util/singleton.h
bin/terminal.shader.Shader.o: src/terminal/shader/Shader.cpp src/terminal/render/glfw.h src/terminal/shader/Shader.h src/terminal/util/error.h src/terminal/util/singleton.h
bin/terminal.shader.Text.o: src/terminal/shader/Text.cpp resources/shaders/text.h src/terminal/render/glfw.h src/terminal/shader/Shader.h src/terminal/shader/Text.h src/terminal/util/error.h src/terminal/util/singleton.h
bin/terminal.State.o: src/terminal/State.cpp src/terminal/State.h src/terminal/render/Model.h src/terminal/render/glfw.h src/terminal/util/singleton.h
bin/terminal.Terminal.o: src/terminal/Terminal.cpp src/EShell.h src/ETerminal.h src/TermInput.h src/terminal/Line.h src/terminal/Resources.h src/terminal/Scroll.h src/terminal/State.h src/terminal/Terminal.h src/terminal/TextBuffer.h src/terminal/env.h src/terminal/gui/Rectangle.h src/terminal/gui/Scrollbar.h src/terminal/render/Buffer.h src/terminal/render/Color.h src/terminal/render/Font.h src/terminal/render/FontLibrary.h src/terminal/render/Model.h src/terminal/render/Texture.h src/terminal/render/ftype.h src/terminal/render/glfw.h src/terminal/shader/Primitive.h src/terminal/shader/Shader.h src/terminal/shader/Text.h src/terminal/textmods/Mod.h src/terminal/textmods/mods.h src/terminal/util/Timer.h src/terminal/util/enums.h src/terminal/util/singleton.h src/terminal/util/util.h
bin/terminal.TextBuffer.o: src/terminal/TextBuffer.cpp src/terminal/Line.h src/terminal/Resources.h src/terminal/Scroll.h src/terminal/TextBuffer.h src/terminal/env.h src/terminal/gui/Rectangle.h src/terminal/render/Buffer.h src/terminal/render/Color.h src/terminal/render/Font.h src/terminal/render/FontLibrary.h src/terminal/render/Model.h src/terminal/render/Texture.h src/terminal/render/ftype.h src/terminal/shader/Primitive.h src/terminal/shader/Shader.h src/terminal/shader/Text.h src/terminal/textmods/Lookbehind.h src/terminal/textmods/Mod.h src/terminal/textmods/RenderState.h src/terminal/textmods/TextState.h src/terminal/util/singleton.h src/terminal/util/util.h
bin/terminal.textmods.Lookbehind.o: src/terminal/textmods/Lookbehind.cpp src/terminal/textmods/Lookbehind.h src/terminal/textmods/TextState.h
bin/terminal.textmods.Mod.o: src/terminal/textmods/Mod.cpp src/terminal/textmods/Mod.h
bin/terminal.textmods.mods.o: src/terminal/textmods/mods.cpp src/terminal/render/Color.h src/terminal/textmods/Mod.h src/terminal/textmods/TextState.h src/terminal/textmods/mods.h
bin/terminal.textmods.RenderState.o: src/terminal/textmods/RenderState.cpp src/terminal/render/Color.h src/terminal/textmods/RenderState.h src/terminal/textmods/TextState.h
bin/terminal.textmods.TextState.o: src/terminal/textmods/TextState.cpp src/terminal/textmods/TextState.h
bin/terminal.util.error.o: src/terminal/util/error.cpp src/terminal/util/error.h
bin/terminal.util.singleton.o: src/terminal/util/singleton.cpp src/terminal/util/singleton.h
bin/terminal.util.Timer.o: src/terminal/util/Timer.cpp src/terminal/util/Timer.h
bin/terminal.util.util.o: src/terminal/util/util.cpp src/terminal/render/glfw.h src/terminal/util/util.h
bin/TermInput.o: src/TermInput.cpp src/TermInput.h
bin/glad.glad.o: extern/glad/glad.c
