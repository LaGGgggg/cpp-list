#ifndef LIST_H
#define LIST_H


#include <stdexcept>
#include <functional>


template <typename T>
class List {
private:

  struct Node {

    T data;
    Node* next;
    Node* prev;

    explicit Node(const T& data) : data(data), next(nullptr), prev(nullptr) {}
    explicit Node(T&& data) : data(std::move(data)), next(nullptr), prev(nullptr) {}
  };

  Node* first;
  Node* last;
  size_t size_;

  void swap(List<T>& other) noexcept {
    std::swap(first, other.first);
    std::swap(last, other.last);
    std::swap(size_, other.size_);
  }

  void merge_sort(Node*& head, const std::function<bool(const T&, const T&)>& compare) {

    if (!head || !head->next) {
      return;
    }

    Node* middle = get_middle(head);
    Node* half = middle->next;
    middle->next = nullptr;

    merge_sort(head, compare);
    merge_sort(half, compare);

    head = merge(head, half, compare);
  }
  Node* get_middle(Node* head) {

    if (!head) {
      return head;
    }

    Node* slow = head;
    Node* fast = head->next;

    while (fast && fast->next) {
      slow = slow->next;
      fast = fast->next->next;
    }

    return slow;
  }
  Node* merge(Node* left, Node* right, const std::function<bool(const T&, const T&)>& compare) {

    if (!left) {
      return right;
    }
    if (!right) {
      return left;
    }

    if (compare(left->data, right->data)) {
      left->next = merge(left->next, right, compare);
      left->next->prev = left;
      left->prev = nullptr;
      return left;
    }

    right->next = merge(left, right->next, compare);
    right->next->prev = right;
    right->prev = nullptr;
    return right;
  }

public:

  List() : first(nullptr), last(nullptr), size_(0) {}

  void clear() {

    while (first) {
      const Node* tmp = first;
      first = first->next;
      delete tmp;
    }

    size_ = 0;
    last = nullptr;
  }

  ~List() { clear(); }

  [[nodiscard]] size_t size() const { return size_; }

  void push_back(const T& other) {

    if (first == nullptr) {
      first = new Node(other);
    } else {
      last->next = new Node(other);
      last->next->prev = last;
      last = last->next;
    }

    ++size_;
  }
  size_t insert_if(T& new_element, const std::function<bool(const T&, const T&)>& compare) {

    size_t index = 0;
    Node* current = first;
    while (current) {

      if (compare(new_element, current->data)) {

        Node* new_node = new Node(std::move(new_element));

        new_node->next = current;
        new_node->prev = current->prev;

        if (current->prev) {
          current->prev->next = new_node;
        } else {
          first = new_node;
        }

        current->prev = new_node;

        ++size_;

        return index;
      }

      current = current->next;
      ++index;
    }

    push_back(new_element);

    return size_ - 1;
  }
  void insert(T& other, const size_t index) {

    if (index > size_) {
      throw std::out_of_range("Index out of range");
    }

    Node* new_node = new Node(std::move(other));

    if (index == 0) {

      new_node->next = first;
      if (first) {
        first->prev = new_node;
      }

      first = new_node;
      if (size_ == 0) {
        last = new_node;
      }

    } else if (index == size_) {

      new_node->prev = last;
      if (last) {
        last->next = new_node;
      }
      last = new_node;

    } else {

      Node* current = first;
      for (size_t i = 0; i < index; ++i) {
        current = current->next;
      }

      new_node->next = current;
      new_node->prev = current->prev;

      if (current->prev) {
        current->prev->next = new_node;
      }
      current->prev = new_node;
    }

    ++size_;
  }
  void remove(const size_t index) {

    if (index >= size_) {
      throw std::out_of_range("Index out of range");
    }

    Node* current = first;
    for (size_t i = 0; i < index; ++i) {
      current = current->next;
    }

    if (current->prev) {
      current->prev->next = current->next;
    } else {
      first = current->next;
    }

    if (current->next) {
      current->next->prev = current->prev;
    } else {
      last = current->prev;
    }

    delete current;
    --size_;
  }

  const T& operator[](size_t index) const {

    if (index >= size_) {
      throw std::out_of_range("Index out of range");
    }

    Node* current = first;
    for (size_t i = 0; i < index; ++i) {
      current = current->next;
    }

    return current->data;
  }
  T& operator[](size_t index) {

    if (index >= size_) {
      throw std::out_of_range("Index out of range");
    }

    Node* current = first;
    for (size_t i = 0; i < index; ++i) {
      current = current->next;
    }

    return current->data;
  }

  void merge_sort(const std::function<bool(const T&, const T&)>& compare) {

    merge_sort(first, compare);

    last = first;
    while (last && last->next) {
      last = last->next;
    }
  }

  class Iterator {
  public:

    explicit Iterator(Node* ptr=nullptr) : current(ptr) {}

    T& operator*() const { return current->data; }
    Iterator& operator++() {

      if (current) {
        current = current->next;
      } else {
        throw std::out_of_range("Iterator out of range");
      }

      return *this;
    }
    bool operator==(const Iterator& other) const { return current == other.current; }

  private:
    Node* current;
  };

  [[nodiscard]] Iterator begin() const { return Iterator(first); }
  [[nodiscard]] Iterator end() const { return Iterator(nullptr); }

  class ReverseIterator {
  public:

    explicit ReverseIterator(Node* ptr=nullptr) : current(ptr) {}

    T& operator*() const { return current->data; }
    ReverseIterator& operator++() {

      if (current) {
        current = current->prev;
      } else {
        throw std::out_of_range("Iterator out of range");
      }

      return *this;
    }
    bool operator==(const ReverseIterator& other) const { return current == other.current; }

  private:
    Node* current;
  };

  [[nodiscard]] ReverseIterator rbegin() const { return ReverseIterator(last); }
  [[nodiscard]] ReverseIterator rend() const { return ReverseIterator(nullptr); }
};


#endif // LIST_H
