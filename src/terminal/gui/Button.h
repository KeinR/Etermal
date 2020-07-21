#ifndef ETERMAL_BUTTON_H_INCLUDED
#define ETERMAL_BUTTON_H_INCLUDED

#include <functional>

#include "../render/Model.h"

namespace etm {

    /**
    * Detects if mouse events occur in a certain area,
    * and dispatches events accordingly.
    */
    class Button {
    public:
        /// General purpose button callback
        typedef std::function<void()> callback_t;
    private:
        /// The encapsulated area
        Model area;
        /// Is the mouse hovering over the button?
        bool hovering;

        /// Called when (the) mouse button is pressed.
        /// @note Does not need to be callable
        callback_t onMousePress;
        /// Called when (the) mouse button is released.
        /// @note Does not need to be callable
        callback_t onMouseRelease;
        /// Called when mouse enters the area defined by @ref area.
        /// @note Does not need to be callable
        callback_t onMouseEnter;
        /// Called when mouse leaves the area defined by @ref area.
        /// @note Does not need to be callable
        callback_t onMouseLeave;

    public:
        /**
        * Construct a button object.
        */
        Button();

        /**
        * Set the x coordinate of the button area.
        * @param [in] x The x coordinate
        * @see getX()
        */
        void setX(float x);
        /**
        * Set the y coordinate of the button area.
        * @param [in] y The y coordinate
        * @see getY()
        */
        void setY(float y);
        /**
        * Set the width of the button area.
        * @param [in] width The width
        * @see getWidth()
        */
        void setWidth(float width);
        /**
        * Set the height of the button area.
        * @param [in] height The height
        * @see getHeight()
        */
        void setHeight(float height);

        /**
        * Gets the x coordinate of the button area.
        * @return The x coordiante
        * @see setX(float x)
        */
        float getX();
        /**
        * Gets the y coordinate of the button area.
        * @return The y coordiante
        * @see setY(float y)
        */
        float getY();
        /**
        * Gets the width of the button area.
        * @return The width
        * @see setWidth(float width)
        */
        float getWidth();
        /**
        * Gets the height of the button area.
        * @return The height
        * @see setHeight(float height)
        */
        float getHeight();

        /**
        * Set the callback for when (the) mouse button is pressed.
        * @param [in] callback The callback
        */
        void setOnMousePress(const callback_t &callback);

        /**
        * Set the callback for when (the) mouse button is released.
        * @param [in] callback The callback
        */
        void setOnMouseRelease(const callback_t &callback);

        /**
        * Set the callback for when the mouse enters the button area.
        * @param [in] callback The callback
        */
        void setOnMouseEnter(const callback_t &callback);

        /**
        * Set the callback for when mouse leaves the button area.
        * @param [in] callback The callback
        */
        void setOnMouseLeave(const callback_t &callback);

        /**
        * Notify the button of a mouse button action.
        * Depending on the position of the mouse, will invoke
        * a callback.
        * @param [in] isPressed Is the mouse button pressed?
        * @param [in] mouseX The current x coordinate of the mouse
        * @param [in] mouseY The current y coordinate of the mouse
        */
        void mouseClick(bool isPressed, float mouseX, float mouseY);

        /**
        * Notify the button of a mouse button action.
        * Depending on the position of the mouse, will invoke
        * a callback.
        * @param [in] mouseX The new x coordinate of the mouse
        * @param [in] mouseY The new y coordinate of the mouse
        */
        void mouseMove(float mouseX, float mouseY);
    };
}

#endif
