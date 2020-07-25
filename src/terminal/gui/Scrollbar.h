#ifndef ETERMAL_SCROLLBAR_H_INCLUDED
#define ETERMAL_SCROLLBAR_H_INCLUDED

#include "Rectangle.h"
#include "Triangle.h"
#include "Button.h"
#include "../util/Timer.h"

namespace etm {
    // ../Scroll
    class Scroll;
    // ../Resources
    class Resources;
}

namespace etm {

    /**
    * @ref Scroll frontend, displays the visible scrollbar
    * and takes care of user input related to it.
    * @see Scroll
    */
    class Scrollbar {
        /// Handle to the @ref Resources object.
        Resources *res;
        /// Handle to the @ref Scroll backend
        Scroll *scroll;
        /// The actual visible scrollbar (not the @ref slider)
        Rectangle bar;
        /// The color of the @ref slider
        Color sliderColor;
        /// Color of the @ref slider upon hover
        Color sliderColorHover;
        /// The actual visible slider, the one the user drags
        /// (not the @ref bar)
        Rectangle slider;
        /// Is mouse dragging?
        bool dragging;
        /// The last y mouse coord.
        /// Only relevent when dragging.
        float dragY;
        /// Whether the @ref slider will render or not upon
        /// calls to @ref render().
        /// If there's no overflow (nowhere to scroll),
        /// should be `false`.
        bool showingSlider;
        /// If hovering, @ref slider should highlight
        bool sliderHovering;
        /// Margin between the @ref slider and the @ref bar
        int sideMargin;

        /**
        * One of those arrows you see so frequently on scrollbars.
        * Seperated into seperate objects is perhaps unnecessary, however
        * I felt like it was the right thing to do.
        * @see Triangle
        * @see Scroll
        */
        class Arrow {
            /// The encapsulating @ref Scrollbar
            Scrollbar *parent;
            /// The background color
            Color bgColor;
            /// The foreground color
            Color arrowColor;
            /// The actual visible arrow
            Triangle arrow;
            /// Used to detect user click/release/etc
            Button button;
            /// `-1` if the Arrow is facing @e up,
            /// `1` if the Arrow is facing @e down
            int directionMod;
            /// Is the arrow being pressed?
            bool pressed;
            /// Is the arrow active?
            /// Goes by the same princible as @ref showingSlider:
            /// if there's no scroll, should not be showing.
            /// However, instead of not showing, Arrow is just grayed out.
            /// @see showingSlider
            bool active;
            /// How long to wait between scrolls while
            /// user is holding down the arrow (so that one click doesn't
            /// scroll the whole page).
            /// @see wait
            Timer tick;
            /// How long to wait after the user first clicks the arrow to
            /// wait until continuously scrolling.
            /// This is so that you can easily scroll one row at a time.
            /// @see tick
            Timer wait;
            /// Scroll by one unit in the preset @ref directionMod
            void doScroll();
        public:
            // Arrow size as a proportion of the container (0-1)
            Arrow(Scrollbar *parent, int tickTimeMillis, int arrowWaitMillis, int directionMod);

            /**
            * Set the x coordinate of the arrow.
            * @param [in] x The x coordinate
            * @see getX()
            */
            void setX(float x);
            /**
            * Set the y coordinate of the arrow.
            * @param [in] y The y coordinate
            * @see getY()
            */
            void setY(float y);
            /**
            * Set the width of the arrow.
            * @param [in] width The width
            * @see getWidth()
            */
            void setWidth(float width);
            /**
            * Set the height of the arrow.
            * @param [in] height The height
            * @see getHeight()
            */
            void setHeight(float height);

            /**
            * Gets the x coordinate of the arrow.
            * @return The x coordiante
            * @see setX(float x)
            */
            float getX();
            /**
            * Gets the y coordinate of the arrow.
            * @return The y coordiante
            * @see setY(float y)
            */
            float getY();
            /**
            * Gets the width of the arrow.
            * @return The width
            * @see setWidth(float width)
            */
            float getWidth();
            /**
            * Gets the height of the arrow.
            * @return The height
            * @see setHeight(float height)
            */
            float getHeight();

            /**
            * Time between scrolls when holding
            * down the scrollbar arrow.
            * @param [in] millis Time in milliseconds
            * @see Terminal::setScrollCooldown(int millis)
            */
            void setScrollCooldown(int millis);

            /**
            * Time to wait until doing scroll repeat
            * from when user first presses scroll button.
            * @param [in] millis Time in milliseconds
            * @see Terminal::setScrollWait(int millis)
            */
            void setScrollWait(int millis);

            /**
            * Sets the background color
            * @param [in] color The color
            */
            void setColor(const Color &color);

            /**
            * Sets the foreground (arrow) color
            * @param [in] color The color
            */
            void setArrowColor(const Color &color);

            /**
            * Notify the Arrow of a mouse button action.
            * @param [in] isPressed Is the button pressed?
            * @param [in] mouseX The current x coordinate of the mouse
            * @param [in] mouseY The current y coordinate of the mouse
            * @see mouseMove(float mouseX, float mouseY)
            */
            void mouseClick(bool isPressed, float mouseX, float mouseY);

            /**
            * Notify the Arrow of a mouse movement.
            * @param [in] mouseX The new x coordinate of the mouse
            * @param [in] mouseY The new y coordinate of the mouse
            * @see mouseClick(bool isPressed, float mouseX, float mouseY)
            */
            void mouseMove(float mouseX, float mouseY);

            /**
            * Set whether the Arrow is "active".
            * An inactive arrow will be grayed out.
            * @param [in] value Active status, where `true` is activated
            */
            void setActive(bool value);

            /**
            * Renders the arrow to the current framebuffer.
            * @note The text shader [@ref Resources::bindTextShader]
            * must be set before this function is called.
            * @note After this call, the color and model uniforms will
            * be modified.
            * @note This function must be called in order to check
            * the status of the @ref wait and @ref tick timers.
            */
            void render();

            /**
            * Checks timers.
            */
            void animate();
        };

        /// The upward facing arrow
        /// @see Arrow
        /// @see downArrow
        Arrow upArrow;
        /// The downward facing arrow
        /// @see Arrow
        /// @see upArrow
        Arrow downArrow;

        /**
        * Recalculate the x position of the @ref slider
        * @see calcSliderY()
        * @see calcSliderWidth()
        */
        void calcSliderX();

        /**
        * Recalculate the y position of the @ref slider
        * @see calcSliderX()
        * @see calcSliderWidth()
        */
        void calcSliderY();

        /**
        * Recalculate the width of the @ref slider
        * @see calcSliderX()
        * @see calcSliderY()
        */
        void calcSliderWidth();

        /**
        * Recalculate the positions of the @ref upArrow
        * and @ref downArrow
        */
        void calcArrows();

        /**
        * Obtain the resources manager.
        * For use by nested classes.
        * @return The resources manager
        */
        Resources *getRes();
    public:

        /**
        * Construct a Scrollbar.
        * @param [in] res A @ref Resources object
        * @param [in] scroll A @ref Scroll backend
        */
        Scrollbar(Resources *res, Scroll &scroll);

        /**
        * Sets the scroll backend.
        * @param [in] scroll The scroll backend
        */
        void setScroll(Scroll &scroll);

        /**
        * Time between scrolls when holding
        * down the scrollbar arrow.
        * @param [in] millis Time in milliseconds
        * @see Terminal::setScrollCooldown(int millis)
        */
        void setScrollCooldown(int millis);

        /**
        * Time to wait until doing scroll repeat
        * from when user first presses scroll button.
        * @param [in] millis Time in milliseconds
        * @see Terminal::setScrollWait(int millis)
        */
        void setScrollWait(int millis);

        /**
        * Set the x coordinate of the scrollbar.
        * @param [in] x The x coordinate
        * @see getX()
        */
        void setX(int x);
        /**
        * Set the y coordinate of the scrollbar.
        * @param [in] y The y coordinate
        * @see getY()
        */
        void setY(int y);
        /**
        * Set the width of the scrollbar.
        * @param [in] width The width
        * @see getWidth()
        */
        void setWidth(int width);
        /**
        * Set the height of the scrollbar.
        * @param [in] height The height
        * @see getHeight()
        */
        void setHeight(int height);
        /**
        * Set the side margin of the scrollbar.
        * The side margin is the distance between the
        * @ref slider and @ref bar.
        * @param [in] margin The margin
        * @see getSideMargin()
        */
        void setSideMargin(int margin);

        /**
        * Gets the x coordinate of the model.
        * @return The x coordiante
        * @see setX(int x)
        */
        int getX();
        /**
        * Gets the y coordinate of the model.
        * @return The y coordiante
        * @see setY(int y)
        */
        int getY();
        /**
        * Gets the width of the model.
        * @return The width
        * @see setWidth(int width)
        */
        int getWidth();
        /**
        * Gets the height of the model.
        * @return The height
        * @see setHeight(int height)
        */
        int getHeight();
        /**
        * Get the side margin of the scrollbar.
        * The side margin is the distance between the
        * @ref slider and @ref bar.
        * @return The margin
        * @see getSideMargin()
        */
        int getSideMargin();

        /**
        * Set the color of the @ref scrollbar, the "background"
        * if you will - @e Not the @ref slider, which is what the
        * user drags.
        * @param [in] color The color
        */
        void setBarColor(const Color &color);
        /**
        * Set the color of the @ref slider, which is what the
        * user drags.
        * @param [in] color The color
        */
        void setSliderColor(const Color &color);

        /**
        * Update the Scrollbar's positional info based off of the
        * values in @ref scroll.
        * This function will never be called by the Scrollbar itself
        * unless @e it was the one that updated the @ref scroll
        */
        void update();

        /**
        * Update the scrollbar's y value only.
        */
        void updateScroll();

        /**
        * Notify the Scrollbar of a mouse button action.
        * @param [in] isPressed is the mouse button pressed?
        * @param [in] mouseX The current x coordinate of the mouse
        * @param [in] mouseY The current y coordinate of the mouse
        * @see mouseMove(float mouseX, float mouseY)
        */
        void mouseClick(bool isPressed, float mouseX, float mouseY);
        /**
        * Notify the Scrollbar of a mouse movement.
        * @param [in] mouseX The new x coordinate of the mouse
        * @param [in] mouseY The new y coordinate of the mouse
        * @see mouseClick(bool isPressed, float mouseX, float mouseY)
        */
        void mouseMove(float mouseX, float mouseY);

        /**
        * Renders the Scrollbar to the current framebuffer.
        * @note Requires that the Primitive shader in @ref res
        * [@ref Resources::bindPrimitiveShader()] be set.
        * @note Exits with the Text shader in @ref res
        * [@ref Resources::bindTextShader()] set.
        * @note The model and back/foreground/normal color for both
        * the Primitive and Text shaders will be modified
        */
        void render();

        /**
        * Checks timers.
        */
        void animate();
    };
}

#endif
