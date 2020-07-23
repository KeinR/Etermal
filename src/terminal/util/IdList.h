#ifndef ETERMAL_IDLIST_H_INCLUDED
#define ETERMAL_IDLIST_H_INCLUDED

#include <vector>

namespace etm {

    /**
    * An IdList is a list that allows the lookup of a value
    * via an index ID irrespective of container size changes.
    * This is done by keeping track of a index offset.
    * 
    * Originally made to reduce the time complexity of
    * lookup operations for @ref tm::Mod "mods" in
    * @ref TextState. Before, a map was used so that the
    * elements' IDs would be constant, however that was logorithmic
    * in complexity, and the storage was node-based.
    * In an IdList, the storage is contiguous, and lookup is constant.
    */
    template<class T>
    class IdList {
    public:
        /// Container type
        typedef std::vector<T> container_t;
        /// Element ID type
        typedef typename container_t::size_type id_t;
    private:
        /// Container holding all the elements
        container_t container;
        /// The id index offset.
        /// Central to the function of the list.
        id_t indexOffset;
    public:
        /**
        * Construct an empty list.
        */
        IdList();

        /**
        * Get an element via its ID.
        * @note Does not do range checks.
        * Somehow assert yourself that
        * the element actually exists.
        * @param [in] id The id
        * @return The element
        */
        T &get(id_t id);
        /**
        * Add an element to the list and get its ID.
        * @param [in] val The element to insert
        * @return The element's ID
        */
        id_t add(const T &val);

        /**
        * Erase elements at the front, aka the start.
        * @param [in] count Number of elements to erase
        */
        void eraseFront(id_t count);
        /**
        * Erase elements at the back, aka the end.
        * Increments the @ref indexOffset to keep element
        * ID validity.
        * @param [in] count Number of elements to erase
        */
        void eraseBack(id_t count);

        /**
        * Erase all elements and zero the @ref indexOffset.
        */
        void clear();
    };
}

#endif
