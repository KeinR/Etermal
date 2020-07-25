#ifndef ETERMAL_SCROLL_H_INCLUDED
#define ETERMAL_SCROLL_H_INCLUDED

// Resources
namespace etm { class Resources; }

namespace etm {

    /**
    * @ref Scrollbar backend, keeps track of scrolling values.
    */
    class Scroll {
        /// Handle to the resources manager.
        /// Notifies it when there's been a change in the scroll
        Resources *res;
        /// Total height of all elements if
        /// they were to all be displayed
        int grossHeight;
        /// Actual displayable height, the
        /// viewport height
        int netHeight;
        /// The scroll offset, how much the
        /// viewport has been moved down.
        float offset;
        /// Maximum scroll @ref offset
        float maxOffset;

        /// Minumun scroll at a time.
        /// Until it reaches this value, the
        /// scroll ammount builds up in the
        /// @ref alignBuffer
        float align;
        /// The progress towards one @ref align
        float alignBuffer;

        /**
        * Recalculate @ref maxOffset
        */
        void recalc();
    public:
        /**
        * Create a new object, with values set to 0 or 1.
        * @param [in] res Handle to the resources manager,
        * used to report when the scroll changes
        */
        Scroll(Resources *res);

        // Must be positive and not equal to zero
        /**
        * Set the scroll @ref align, the units in which the scrolling
        * will happen.
        * @note The align must be > 0
        * @param [in] value The new @ref align
        */
        void setAlign(float value);

        /**
        * Sets the net/viewport/displayable height
        * of the scroll.
        * @param [in] height The new @ref netHeight
        */
        void setNetHeight(int height);
        /**
        * Sets the gross/total height of the elements.
        * @param [in] height The new @ref grossHeight
        */
        void setGrossHeight(int height);

        /**
        * Get the net height
        * @return The @ref netHeight
        */
        int getNetHeight();
        /**
        * Get the gross height
        * @return The @ref grossHeight
        */
        int getGrossHeight();
        /**
        * Get the current scroll y-offset
        * that should be applied to elements.
        * @return The scroll offset
        */
        float getOffset();
        /**
        * Get the maximum possible scroll y-offset
        * @return The max scroll offset
        */
        float getMaxOffset();

        // Returns true if actually scrolled
        /**
        * Scroll by an ammount.
        * Will not actually scroll until the @ref alignBuffer
        * has been filled.
        * @param [in] ammount The ammount of distance to scroll
        */
        void scroll(float ammount);
        /**
        * Scroll by one entire align unit.
        * Equivilent to `scroll(ammount * align)`
        * @param [in] ammount Number of aligns to scroll
        */
        void scrollByAlign(int ammount);
        /**
        * Set the current scroll offset to the max offset.
        */
        void jump();
    };
}

#endif
