#include "IdList.h"

#include <memory>
#include <iostream>

#include "../textmods/Mod.h"

template<class T>
etm::IdList<T>::IdList(): indexOffset(0) {
}

template<class T>
T &etm::IdList<T>::get(id_t id) {
    return container[id - indexOffset];
}
template<class T>
typename etm::IdList<T>::id_t etm::IdList<T>::add(const T &val) {
    container.push_back(val);
    return indexOffset + container.size() - 1;
}

template<class T>
void etm::IdList<T>::eraseFront(id_t count) {
    container.erase(container.begin(), container.begin() + count);
    indexOffset += count;
}
template<class T>
void etm::IdList<T>::eraseBack(id_t count) {
    container.erase(container.end() - count, container.end());
}

template<class T>
void etm::IdList<T>::clear() {
    container.clear();
    container.shrink_to_fit();
    indexOffset = 0;
}

template class etm::IdList<std::shared_ptr<etm::tm::Mod>>;
