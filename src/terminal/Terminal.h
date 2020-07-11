#ifndef ETERMAL_TERMINAL_H_INCLUDED
#define ETERMAL_TERMINAL_H_INCLUDED

#include <memory>

#include "util/enums.h"
#include "gui/TextInput.h"
#include "gui/ScrollBox.h"
#include "Resources.h"

// ../shell/Shell
class Shell;
// ../render/glfw
class GLFWwindow;

/*
* Frontend for the shell
*/

namespace etm {
    class Terminal {
        std::unique_ptr<Resources> resources;

        Model viewport;

        std::string line1text; // TEMP
        std::vector<Text> lines;
        ScrollBox output;
        TextInput input;
        Rectangle outputBG;
        Rectangle inputBG;

        Shell *shell;
    public:

        // Initializes with the current context.
        // NOTE: THE DESIRED CONTEXT MUST BE CURRENT WHEN IT IS INITIALIZED!!!
        // FAILURE TO DO SO WILL RESULT IN BAD THIGNS HAPPENING
        Terminal();
        Terminal(Terminal &&other) = default;
        Terminal &operator=(Terminal &&other) = default;

        // The shell is where user input will be directed
        void setShell(Shell &shell);

        // Display text in output
        void dispText(const std::string &str);
        // Send text to shell
        void inputText(const std::string &str);

        void setX(float x);
        void setY(float y);
        void setWidth(float width);
        void setHeight(float height);

        // Position elemnts according to coords
        void updatePosition();

        // Input events

        // All printable keys
        void userKeyPress(char c);
        void userPaste(const std::string &text);
        // Backspace, enter, up, down, left, right keys
        void userActionKey(actionKey key);
        // Only the y offset is relevent.
        // Value is difference from the last scroll
        // position
        void userScroll(float yOffset);
        // Viewport coordinates of the mouse
        // (model coords are used to calculate if mouse clicked)
        void userClick(bool isPressed, float mouseX, float mouseY);
        void userMove(float mouseX, float mouseY);

        // Renders to the current context
        void render();
    };
}

#endif
