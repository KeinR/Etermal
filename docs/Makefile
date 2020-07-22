
# Change this to wherever you want the docs to go
output = ../doxygen/

frontendInput = ../src/ETerminal.h ../src/TermInput.h ../src/EShell.h ../src/terminal/Terminal.h ../src/shell/ ./
backendInput = ../src/ ./

frontend:
	./build.sh \
	"INPUT = $(frontendInput)" \
	"OUTPUT_DIRECTORY = $(output)"

backend:
	./build.sh \
	"INPUT = $(backendInput)" \
	"OUTPUT_DIRECTORY = $(output)"

