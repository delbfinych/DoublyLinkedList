#include <stdexcept>
#include <sstream>
#include <utility>
#include "List.h"

List::List(): _head(nullptr), _tail(nullptr), _size(0) {}

List::List(const List& other): _head(nullptr), _tail(nullptr), _size(0) {
    auto node = new Node(0, nullptr, nullptr);
    _head = _tail = node;
    auto otherPtr = other._head;

    while (_size != other._size - 1) {
        node->data = otherPtr->data;
        node->next = new Node(0, node, nullptr);
        otherPtr = otherPtr->next;
        node = node->next;
        ++_size;
    }
    node->data = otherPtr->data;
    _tail = node;
    ++_size;
}

List::List(List&& other) noexcept {
    _size = std::exchange(other._size, 0);
    _head = std::exchange(other._head, nullptr);
    _tail = std::exchange(other._tail, nullptr);
}

List& List::operator=(const List& other) {
    if (this != &other) {
        *this = List(other);
    }
    return *this;
}

List& List::operator=(List&& other) noexcept {
    if (this != &other) {
        clear();
        _size = std::exchange(other._size, 0);
        _head = std::exchange(other._head, nullptr);
        _tail = std::exchange(other._tail, nullptr);
    }
    return *this;
}

List::~List() {
    clear();
}

size_t List::size() const {
    return _size;
}

bool List::empty() const {
    return !_size;
}

const ValueType& List::operator[](size_t pos) const {
    return findNode(pos)->data;
}

ValueType& List::operator[](size_t pos) {
    return findNode(pos)->data;
}

const ValueType& List::front() const {
    return _head->data;
}

ValueType& List::front() {
    return _head->data;
}

const ValueType& List::back() const {
    return _tail->data;
}

ValueType& List::back() {
    return _tail->data;
}

void List::insert(size_t pos, const ValueType& val) {
    if (pos > _size) {
        std::stringstream e;
        e << "List::insert. " << pos
          << " is invalid pos";
        throw std::range_error(e.str());
    }
    else if (pos == 0) {
        auto newNode = new Node(val, nullptr, _head);
        _head = newNode;
        if (_size == 0) {
            _tail = _head;
        }
    }
    else if (pos == _size) {
        auto newNode = new Node(val, _tail, nullptr);
        _tail->next = newNode;
        _tail = newNode;
    }
    else {
        auto prev = findNode(pos - 1);
        auto next = prev->next;
        auto newNode = new Node(val, prev, next);
        prev->next = newNode;
        next->prev = newNode;
    }
    ++_size;
}

// TODO: check if pos is valid
void List::insert(const List::Iterator& pos, const ValueType& val) {
    if (pos == begin()) {
        insert(0, val);
    }
    else if (pos == end()) {
        insert(_size, val);
    }
    else {
        auto prev = pos._ptr->prev;
        auto next = prev->next;
        auto newNode = new Node(val, prev, next);
        prev->next = newNode;
        next->prev = newNode;
        ++_size;
    }
}
void List::pushBack(const ValueType& val) {
    insert(_size, val);
}

void List::pushFront(const ValueType& val) {
    insert(0, val);
}

void List::popFront() {
    remove(0);
}

void List::popBack() {
    remove(_size - 1);
}

void List::remove(size_t pos) {
    if (_size == 0) {
        return;
    }
    else if (_size == 1) {
        delete _head;
        _head = _tail = nullptr;
    }
    else if (pos >= _size) {
        std::stringstream e;
        e << "List::remove. " << pos
          << " is invalid pos";
        throw std::range_error(e.str());
    }
    else if (pos == 0) {
       auto head = _head;
       _head = head->next;
       _head->prev = nullptr;
       delete head;
    }
    else if (pos == _size - 1) {
        auto tail = _tail;
        _tail = _tail->prev;
        _tail->next = nullptr;
        delete tail;
    }
    else {
        auto node = findNode(pos);
        node->prev->next = node->next;
        node->next->prev = node->prev;
        delete node;
    }
    --_size;
}

// TODO: check if pos is valid
void List::remove(const List::Iterator& pos) {
    if (pos == begin() || _size == 1) {
        remove(0);
    }
    else if (pos == end()) {
        remove(_size - 1);
    }
    else {
        auto tmp = pos;
        tmp._ptr->prev->next = tmp._ptr->next;
        tmp._ptr->next->prev = tmp._ptr->prev;
        delete tmp._ptr;
        --_size;
    }

}
void List::clear() {
    while (_size > 0) {
        popFront();
    }
}

void List::_mutableMap(ValueType (* callback)(const ValueType&)) {
    for (ValueType& i : *this) {
        i = callback(i);
    }
}

void List::_mutableFilter(bool (* callback)(const ValueType&)) {
    for (auto it = begin(); it != end();) {
        if (!callback(*it)) {
            auto nextIt = it + 1;
            remove(it);
            it = nextIt;
        }
        else {
            ++it;
        }
    }


}


void List::forEach(void (* callback)(ValueType&, size_t)) {
    size_t i = 0;
    for (ValueType& it : *this) {
        callback(it, i);
        ++i;
    }
}

List List::map(ValueType (* callback)(const ValueType&)) const {
    auto copy(*this);
    copy._mutableMap(callback);
    return copy;
}

List List::filter(bool (* callback)(const ValueType&)) const {
    auto copy(*this);
    copy._mutableFilter(callback);
    return copy;
}

void List::_reverse(List::Node* begin, List::Node* end) {
    // разделяем список на 3 части: левая, реверсируемая, правая
    // и изолируем реверсируемую часть, как буто это полноценный список
    // затем реверсируем её обычным алгоритмом для списков
    // и потом соединяем части
    Node* beforeBegin = begin->prev;
    Node* afterEnd = end->next;

    begin->prev = nullptr;
    end->next = nullptr;

    Node* current = begin;

    while (current != nullptr) {
        std::swap(current->next, current->prev);
        current = current->prev;
    }

    std::swap(begin, end);

    if (!beforeBegin) {
        _head = begin;
    }
    else {
        beforeBegin->next = begin;
    }
    begin->prev = beforeBegin;

    if (!afterEnd) {
        _tail = end;
    }
    else {
        afterEnd->prev = end;
    }
    end->next = afterEnd;

}

void List::reverse(size_t begin, size_t end) {
    if (begin > end || begin >= _size || end >= _size) {
        std::stringstream e;
        e << "List::reverse(" << begin
          << "," << end
          << ") bad range";

        throw std::range_error(e.str());
    }
    auto start = findNode(begin);
    auto finish = start;
    for (size_t i = begin; i < end; ++i) {
        finish = finish->next;
    }
    _reverse(start, finish);
}

List::Node* List::findNode(size_t pos) const {
    if (pos >= _size) {
        throw std::range_error("findNode(): Invalid pos");
    }
    size_t i = 0;
    // TODO: начать искать с хвоста если pos больше середины
    auto ptr = _head;
    while (i < pos) {
        if (ptr->next) {
            ptr = ptr->next;
        }
        ++i;
    }
    return ptr;
}

// Iterator

List::Iterator::Iterator(List::Node* ptr): _ptr(ptr) {}

List::Iterator::Iterator(const List::Iterator& it): _ptr(it._ptr) {}

// TODO: throw range error
List::Iterator List::Iterator::operator+(size_t n) {
    auto tmp = *this;
    while (n > 0) {
        ++(tmp);
        --n;
    }
    return tmp._ptr;
}
// TODO: throw range error
List::Iterator List::Iterator::operator-(size_t n) {
    auto tmp = *this;
    while (n > 0) {
        --(tmp);
        --n;
    }
    return tmp._ptr;
}
// TODO: throw range error
List::Iterator& List::Iterator::operator++() {
    _ptr = _ptr->next;
    return *this;
}

// TODO: throw range error
List::Iterator& List::Iterator::operator--() {
    _ptr = _ptr->prev;
    return *this;
}

List::Iterator List::Iterator::operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

List::Iterator List::Iterator::operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
}


bool List::Iterator::operator!=(const List::Iterator& it) const {
    return _ptr != it._ptr;
}

bool List::Iterator::operator==(const List::Iterator& it) const {
    return _ptr == it._ptr;
}

ValueType& List::Iterator::operator*() const {
    return _ptr->data;
}


// Reverse Iterator
List::ReverseIterator::ReverseIterator(List::Node* ptr): _ptr(ptr) {}

List::ReverseIterator::ReverseIterator(const List::ReverseIterator& it): _ptr(it._ptr) {}

List::ReverseIterator& List::ReverseIterator::operator++() {
    _ptr = _ptr->prev;
    return *this;
}

List::ReverseIterator& List::ReverseIterator::operator--() {
    _ptr = _ptr->next;
    return *this;
}

List::ReverseIterator List::ReverseIterator::operator++(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
}

List::ReverseIterator List::ReverseIterator::operator--(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

bool List::ReverseIterator::operator!=(const List::ReverseIterator& it) const {
    return _ptr != it._ptr;
}

bool List::ReverseIterator::operator==(const List::ReverseIterator& it) const {
    return _ptr == it._ptr;
}

ValueType& List::ReverseIterator::operator*() const {
    return _ptr->data;
}

// TODO: throw range error
List::ReverseIterator List::ReverseIterator::operator+(size_t n) {
    while (n > 0) {
        ++(*this);
        --n;
    }
    return _ptr;
}

// TODO: throw range error
List::ReverseIterator List::ReverseIterator::operator-(size_t n) {
    while (n > 0) {
        ++(*this);
        --n;
    }
    return _ptr;
}