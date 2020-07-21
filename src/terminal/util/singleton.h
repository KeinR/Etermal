#ifndef ETERMAL_NOCOPY_H_INCLUDED
#define ETERMAL_NOCOPY_H_INCLUDED

namespace etm {
    /**
    * Used to label a singleton class, one that should
    * not be copied or moved.
    * @todo Move semantics should be implemented project wide, so
    * as to make this class obsolete.
    */
    class singleton {
    public:
        singleton() = default;
        virtual ~singleton() = 0;
        singleton(singleton&&) = delete;
        singleton(const singleton&) = delete;
        singleton &operator=(singleton&&) = delete;
        singleton &operator=(const singleton&) = delete;
    };
}

#endif
