#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node {
        T data;
        Node *left;
        Node *right;
        int npl;  // null path length

        Node(const T &d) : data(d), left(nullptr), right(nullptr), npl(0) {}
    };

    Node *root;
    size_t sz;
    Compare cmp;

    int npl(Node *node) const {
        return node ? node->npl : -1;
    }

    void updateNpl(Node *node) {
        if (node) {
            node->npl = std::min(npl(node->left), npl(node->right)) + 1;
        }
    }

    Node *copyNode(Node *node) {
        if (!node) return nullptr;
        Node *newNode = new Node(node->data);
        newNode->left = copyNode(node->left);
        newNode->right = copyNode(node->right);
        newNode->npl = node->npl;
        return newNode;
    }

    void deleteNode(Node *node) {
        if (!node) return;
        deleteNode(node->left);
        deleteNode(node->right);
        delete node;
    }

    Node *merge(Node *h1, Node *h2) {
        if (!h1) return h2;
        if (!h2) return h1;

        if (cmp(h1->data, h2->data)) {
            std::swap(h1, h2);
        }

        h1->right = merge(h1->right, h2);

        if (npl(h1->left) < npl(h1->right)) {
            std::swap(h1->left, h1->right);
        }

        updateNpl(h1);
        return h1;
    }

public:
    priority_queue() : root(nullptr), sz(0), cmp() {}

    priority_queue(const priority_queue &other) : root(nullptr), sz(other.sz), cmp(other.cmp) {
        root = copyNode(other.root);
    }

    ~priority_queue() {
        deleteNode(root);
    }

    priority_queue &operator=(const priority_queue &other) {
        if (this == &other) return *this;

        Node *newRoot = copyNode(other.root);
        deleteNode(root);
        root = newRoot;
        sz = other.sz;
        cmp = other.cmp;
        return *this;
    }

    const T &top() const {
        if (empty()) {
            throw container_is_empty();
        }
        return root->data;
    }

    void push(const T &e) {
        Node *newNode = new Node(e);
        Node *oldRoot = root;
        size_t oldSz = sz;

        try {
            root = merge(root, newNode);
            sz++;
        } catch (...) {
            delete newNode;
            root = oldRoot;
            sz = oldSz;
            throw runtime_error();
        }
    }

    void pop() {
        if (empty()) {
            throw container_is_empty();
        }

        Node *oldRoot = root;
        size_t oldSz = sz;

        try {
            Node *newRoot = merge(root->left, root->right);
            root = newRoot;
            sz--;
            delete oldRoot;
        } catch (...) {
            root = oldRoot;
            sz = oldSz;
            throw runtime_error();
        }
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    void merge(priority_queue &other) {
        if (this == &other) return;

        Node *oldRoot1 = root;
        Node *oldRoot2 = other.root;
        size_t oldSz1 = sz;
        size_t oldSz2 = other.sz;

        try {
            root = merge(root, other.root);
            sz += other.sz;
            other.root = nullptr;
            other.sz = 0;
        } catch (...) {
            root = oldRoot1;
            other.root = oldRoot2;
            sz = oldSz1;
            other.sz = oldSz2;
            throw runtime_error();
        }
    }
};

}

#endif
