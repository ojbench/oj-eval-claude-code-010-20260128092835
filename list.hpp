#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;
        node(const T &val, node *p = nullptr, node *n = nullptr) : data(new T(val)), prev(p), next(n) {}
        node(node *p = nullptr, node *n = nullptr) : data(nullptr), prev(p), next(n) {}
        ~node() { if (data) delete data; }
    };

    node *head;
    size_t _size;

    void init() {
        head = new node();
        head->prev = head;
        head->next = head;
        _size = 0;
    }

    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        _size++;
        return cur;
    }

    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        _size--;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
        friend class list<T>;
        friend class const_iterator;
    private:
        node *current;
        const list<T> *container;
    public:
        iterator() : current(nullptr), container(nullptr) {}
        iterator(node *c, const list<T> *l) : current(c), container(l) {}

        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }
        iterator & operator++() {
            if (!current || !container || current == container->head) throw invalid_iterator();
            current = current->next;
            return *this;
        }
        iterator operator--(int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }
        iterator & operator--() {
            if (!current || !container || current->prev == container->head) throw invalid_iterator();
            current = current->prev;
            return *this;
        }
        T & operator *() const {
            if (!current || !container || current == container->head) throw invalid_iterator();
            return *(current->data);
        }
        T * operator ->() const {
            if (!current || !container || current == container->head) throw invalid_iterator();
            return current->data;
        }
        bool operator==(const iterator &rhs) const {
            return current == rhs.current;
        }
        bool operator==(const const_iterator &rhs) const;
        bool operator!=(const iterator &rhs) const {
            return current != rhs.current;
        }
        bool operator!=(const const_iterator &rhs) const;
    };

    class const_iterator {
        friend class list<T>;
        friend class iterator;
    private:
        node *current;
        const list<T> *container;
    public:
        const_iterator() : current(nullptr), container(nullptr) {}
        const_iterator(node *c, const list<T> *l) : current(c), container(l) {}
        const_iterator(const iterator &it) : current(it.current), container(it.container) {}

        const_iterator operator++(int) {
            const_iterator temp = *this;
            ++(*this);
            return temp;
        }
        const_iterator & operator++() {
            if (!current || !container || current == container->head) throw invalid_iterator();
            current = current->next;
            return *this;
        }
        const_iterator operator--(int) {
             const_iterator temp = *this;
             --(*this);
             return temp;
        }
        const_iterator & operator--() {
            if (!current || !container || current->prev == container->head) throw invalid_iterator();
            current = current->prev;
            return *this;
        }
        const T & operator *() const {
            if (!current || !container || current == container->head) throw invalid_iterator();
            return *(current->data);
        }
        const T * operator ->() const {
            if (!current || !container || current == container->head) throw invalid_iterator();
            return current->data;
        }
        bool operator==(const iterator &rhs) const {
            return current == rhs.current;
        }
        bool operator==(const const_iterator &rhs) const {
            return current == rhs.current;
        }
        bool operator!=(const iterator &rhs) const {
            return current != rhs.current;
        }
        bool operator!=(const const_iterator &rhs) const {
            return current != rhs.current;
        }
    };

    list() { init(); }
    list(const list &other) {
        init();
        for (auto it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }
    virtual ~list() {
        clear();
        delete head;
    }
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (auto it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
        return *this;
    }
    const T & front() const {
        if (empty()) throw container_is_empty();
        return *(head->next->data);
    }
    const T & back() const {
        if (empty()) throw container_is_empty();
        return *(head->prev->data);
    }
    iterator begin() { return iterator(head->next, this); }
    const_iterator cbegin() const { return const_iterator(head->next, this); }
    iterator end() { return iterator(head, this); }
    const_iterator cend() const { return const_iterator(head, this); }

    virtual bool empty() const { return _size == 0; }
    virtual size_t size() const { return _size; }

    virtual void clear() {
        node *curr = head->next;
        while (curr != head) {
            node *next = curr->next;
            delete curr;
            curr = next;
        }
        head->next = head;
        head->prev = head;
        _size = 0;
    }

    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) throw invalid_iterator();
        node *new_node = new node(value);
        insert(pos.current, new_node);
        return iterator(new_node, this);
    }

    virtual iterator erase(iterator pos) {
        if (empty()) throw container_is_empty();
        if (pos.container != this || pos.current == head) throw invalid_iterator();

        node *next_node = pos.current->next;
        node *removed = erase(pos.current);
        delete removed;
        return iterator(next_node, this);
    }

    void push_back(const T &value) { insert(end(), value); }
    void pop_back() { if (empty()) throw container_is_empty(); erase(iterator(head->prev, this)); }
    void push_front(const T &value) { insert(begin(), value); }
    void pop_front() { if (empty()) throw container_is_empty(); erase(begin()); }

    void sort() {
        if (_size <= 1) return;
        node **nodes = new node*[_size];
        node *curr = head->next;
        for (size_t i = 0; i < _size; ++i) {
            nodes[i] = curr;
            curr = curr->next;
        }
        auto cmp = [](const node *a, const node *b) {
            return *(a->data) < *(b->data);
        };
        sjtu::sort<node*>(nodes, nodes + _size, cmp);
        head->next = nodes[0];
        nodes[0]->prev = head;
        for (size_t i = 0; i < _size - 1; ++i) {
            nodes[i]->next = nodes[i+1];
            nodes[i+1]->prev = nodes[i];
        }
        nodes[_size-1]->next = head;
        head->prev = nodes[_size-1];
        delete[] nodes;
    }

    void merge(list &other) {
        if (this == &other) return;
        if (other.empty()) return;
        if (empty()) {
            head->next = other.head->next;
            head->prev = other.head->prev;
            head->next->prev = head;
            head->prev->next = head;
            _size = other._size;
            other.head->next = other.head;
            other.head->prev = other.head;
            other._size = 0;
            return;
        }
        node *first1 = head->next;
        node *last1 = head;
        node *first2 = other.head->next;
        node *last2 = other.head;

        while (first1 != last1 && first2 != last2) {
            if (*(first2->data) < *(first1->data)) {
                 node *next2 = first2->next;
                 first2->prev = first1->prev;
                 first2->next = first1;
                 first1->prev->next = first2;
                 first1->prev = first2;
                 first2 = next2;
                 _size++;
                 other._size--;
            } else {
                first1 = first1->next;
            }
        }
        while (first2 != last2) {
             node *next2 = first2->next;
             first2->prev = head->prev;
             first2->next = head;
             head->prev->next = first2;
             head->prev = first2;
             _size++;
             other._size--;
             first2 = next2;
        }
        other.head->next = other.head;
        other.head->prev = other.head;
    }

    void reverse() {
        if (_size <= 1) return;
        node *curr = head;
        do {
            node *temp = curr->next;
            curr->next = curr->prev;
            curr->prev = temp;
            curr = curr->prev;
        } while (curr != head);
    }

    void unique() {
        if (_size <= 1) return;
        node *curr = head->next;
        while (curr != head && curr->next != head) {
            if (*(curr->data) == *(curr->next->data)) {
                node *duplicate = curr->next;
                erase(duplicate);
                delete duplicate;
            } else {
                curr = curr->next;
            }
        }
    }
};

template<typename T>
bool list<T>::iterator::operator==(const const_iterator &rhs) const {
    return current == rhs.current;
}

template<typename T>
bool list<T>::iterator::operator!=(const const_iterator &rhs) const {
    return current != rhs.current;
}

}
#endif //SJTU_LIST_HPP
