#pragma once

#include <cstddef>

using ValueType = int;
class List {
    class Node {
    public:
        ValueType data;
        Node* next;
        Node* prev;
        Node(ValueType data, Node* prev, Node* next): data(data), prev(prev), next(next) {};
    };
    Node* _head;
    Node* _tail;
    size_t _size;

    void _mutableMap(ValueType (* callback)(const ValueType& currVal));
    void _mutableFilter(bool (* callback)(const ValueType& currVal));
    void _reverse(List::Node* begin, Node* end);
public:
    class Iterator;
    class ConstIterator;
    class ReverseIterator;

    List();
    List(const List& other);
    List(List&& other) noexcept;

    List& operator=(const List& other);
    List& operator=(List&& other) noexcept;

    ~List();

    size_t size() const;
    bool empty() const;

    const ValueType& operator[](size_t pos) const;
    ValueType& operator[](size_t pos);

    const ValueType& front() const;
    ValueType& front();

    const ValueType& back() const;
    ValueType& back();

    void insert(size_t pos, const ValueType& val);
    void insert(const List::Iterator& pos, const ValueType& val);
    void pushBack(const ValueType& val);
    void pushFront(const ValueType& val);

    void popFront();
    void popBack();
    void remove(size_t pos);
    void remove(const Iterator& pos);

    void clear();

    void forEach(void (* callback)(ValueType& currVal, size_t idx));
    List map(ValueType (* callback)(const ValueType& currVal)) const;
    List filter(bool (* callback)(const ValueType& currVal)) const;

    void reverse(size_t begin, size_t end);

    Node* findNode(size_t pos) const;


    class Iterator {
    protected:
        Node* _ptr;
    public:
        friend class List;

        Iterator(Node* ptr);
        Iterator(const Iterator& it);

        Iterator operator+(size_t n);
        Iterator operator-(size_t n);

        Iterator& operator++ ();
        Iterator& operator-- ();

        Iterator operator++ (int);
        Iterator operator-- (int);

        bool operator!=(const Iterator& it) const;
        bool operator==(const Iterator& it ) const;

        ValueType& operator*() const;
    };
    Iterator begin() const { return Iterator(_head); };
    Iterator end() const { return Iterator(_tail->next); };


    class ReverseIterator {
    private:
        Node* _ptr;
    public:
        friend class List;

        ReverseIterator(Node* ptr);
        ReverseIterator(const ReverseIterator& it);

        ReverseIterator operator+(size_t n);
        ReverseIterator operator-(size_t n);

        ReverseIterator& operator++ ();
        ReverseIterator& operator-- ();

        ReverseIterator operator++ (int);
        ReverseIterator operator-- (int);

        bool operator!=(const ReverseIterator& it) const;
        bool operator==(const ReverseIterator& it ) const;

        ValueType& operator*() const;
    };
    ReverseIterator rbegin() const { return ReverseIterator(_tail); };
    ReverseIterator rend() const { return ReverseIterator(_head->prev); };


    class ConstantIterator: public Iterator {
    public:
        ConstantIterator(Node* ptr): Iterator(ptr) {};
        ConstantIterator(const ConstantIterator& it): Iterator(it) {};

        
        const ValueType& operator*() const { return _ptr->data; };
    };
    ConstantIterator cbegin() const { return ConstantIterator(_head); };
    ConstantIterator cend() const { return ConstantIterator(_tail->next); };
};
