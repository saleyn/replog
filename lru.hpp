/*
***** BEGIN LICENSE BLOCK *****

This file is part of the EPI (Erlang Plus Interface) Library.

Copyright (C) 2010 Serge Aleynikov <saleyn@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

***** END LICENSE BLOCK *****
*/
#ifndef _REPLOG_LRU_HPP_
#define _REPLOG_LRU_HPP_

#include <exception>
#include <assert.h>

namespace replog {

/// \brienf Implements a node of a double-linked list.
template <typename T>
struct lru_node {
    T       data;
    lru<T>* prev;
    lru<T>* next;

    template <class Arg1>
    lru_node(Arg1 a1) : data(a1), prev(NULL), next(NULL) {}

    template <class Arg1, class Arg2>
    lru_node(Arg1 a1, Arg2 a2) : data(a1, a2), prev(NULL), next(NULL) {}

    template <class Arg1, class Arg2, class Arg3>
    lru_node(Arg1 a1, Arg2 a2, Arg3 a3)
        : data(a1, a2, a3), prev(NULL), next(NULL) {}

    template <class Arg1, class Arg2, class Arg3, class Arg4>
    lru_node(Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4)
        : data(a1, a2, a3, a4), prev(NULL), next(NULL) {}

    template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
    lru_node(Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5)
        : data(a1, a2, a3, a4, a5), prev(NULL), next(NULL) {}

    template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
    lru_node(Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6)
        : data(a1, a2, a3, a4, a5, a6), prev(NULL), next(NULL) {}
};

/// \brienf Implements LRU double-linked list.
template <typename T>
class lru {
protected:
    lru_node<T>*    m_head;
    lru_node<T>*    m_tail;
    bool            m_owner;
public:
    lru(bool a_owner = false) : m_head(NULL), m_tail(NULL), m_owner(a_owner) {}
    ~lru() {
        if (m_owner) {
            for (lru_node* next; m_head; m_head = next) {
               next = m_head->next;
               delete m_head;
            }
        }
    }

    lru_node<T*> node(T* a_node) const { return reinterpret_cast<lru_node<T>*>(a_node); }

    T* head() { return m_head ? m_head->data : NULL; }
    T* tail() { return m_tail ? m_tail->data : NULL; }

    T* next(T* a_node) {
        assert(a_node != NULL);
        return reinterpret_cast<lru_node<T>*>(a_node)->next;
    }

    T* prev(T* a_node) {
        assert(a_node != NULL);
        return reinterpret_cast<lru_node<T>*>(a_node)->prev;
    }

    /// Add a \a a_node to the head of the LRU list.
    void add(lru_node<T>* a_node) {
        a_node->prev = NULL;
        a_node->next = m_head;
        m_head = a_node;
        if (m_tail == NULL)
            m_tail = m_head;
    }

    /// Remove a \a a_node from the LRU list. The node
    /// must be part of the list. The caller is responsible
    /// for freeing the removed node.
    lru_node<T>* remove(T* a_node) {
        lru_node<T>* p = reinterpret_cast<lru_node<T>*>(a_node);
        remove(p);
        return p;
    }

    /// Remove a \a a_node from the LRU list. The node
    /// must be part of the list. The caller is responsible
    /// for freeing the removed node.
    void remove(lru_node<T>* a_node) {
        if (a_node->prev != NULL)
            a_node->prev->next = a_node->next;
        else {
            assert(m_head == a_node);
            m_head = NULL;
        }
        if (a_node->next != NULL)
            a_node->next->prev = a_node->prev;
        else {
            assert(m_tail == a_node);
            m_tail = a_node->prev ? a_node->prev : m_head;
        }
    }

    /// Move the \a a_node to the head of the LRU list.
    void use(T* a_node) {
        use(reinterpret_cast<lru_node<T>*>(a_node));
    }

    /// Move the \a a_node to the head of the LRU list.
    void use(lru_node<T>* a_node) {
        if (a_node == m_head)
            return;
        remove(a_node);
        add(a_node);
    }
};

/// \brienf Implements LRU double-linked list of fixed size.
/// When a node is added to the LRU list so that the list
/// reaches its maximum fixed size, the oldest node is 
/// deleted from the list.
template <typename T>
class lru_fixed_size: protected lru<T> {
    size_t m_max_size;
    size_t m_count;

    typedef lru<T> base;
public:
    lru_fixed_size(size_t max_size, bool a_owner = false)
        : base(a_owner), m_max_size(a_max_size), m_count(0)
    {}

    size_t max_size()  const { return m_max_size; }
    size_t size()      const { return m_count;    }

    /// Add a \a a_node to the head of the LRU list.
    /// If addition of \a a_node increases the node count
    /// in the list to max_size(), the oldest node is
    /// removed from the list and returned.  Otherwise
    /// the function returns NULL.  It is the responsibility
    /// of the caller to delete that node.
    lru_node<T>* add(lru_node<T>* a_node) {
        if (m_count < m_max_size) {
            base::add(a_node);
            m_count++;
            return NULL;
        }
        lru_node<T>* old = m_tail;
        base::remove(old);
        m_count--;
        return old;
    }

    /// Remove a \a a_node from the LRU list. The node
    /// must be part of the list.
    lru_node<T>* remove(T* a_node) {
        lru_node<T>* p = reinterpret_cast<lru_node<T>*>(a_node);
        remove(p);
        return p;
    }

    /// Remove a \a a_node from the LRU list. The node
    /// must be part of the list.
    void remove(lru_node<T>* a_node) {
        base::remove(a_node);
        m_count--;
    }
} // namespace replog

#endif // _REPLOG_LRU_HPP_

