#ifndef ETERMAL_NOCOPY_H_INCLUDED
#define ETERMAL_NOCOPY_H_INCLUDED

namespace etm {
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
