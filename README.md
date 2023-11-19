
# Deque Template Class

## Overview
This updated Deque template class is a custom implementation of a double-ended queue in C++. It efficiently manages memory and provides easy access and manipulation of elements at both the front and the back. The class is designed to be versatile and can handle different data types.

## Features

- Efficient insertion and deletion at both ends (`push_back`, `push_front`, `pop_back`, `pop_front`).
- Random access to elements using `[]` operator and `at` method.
- Custom iterator implementation supporting random access.
- Capacity management and dynamic memory allocation.
- Support for reverse iterators.
- Exception safety and proper resource management.

## Usage

### Basic Operations
```cpp
Deque<int> intDeque;
intDeque.push_back(1);
intDeque.push_front(2);
intDeque.pop_back();
```

### Working with Iterators
```cpp
for (auto it = intDeque.begin(); it != intDeque.end(); ++it) {
  std::cout << *it << ' ';
}
```

### Accessing Elements
```cpp
int firstElement = intDeque[0];
int secondElement = intDeque.at(1);
```

### Capacity Management
```cpp
// Size of the deque
size_t size = intDeque.size();

// Inserting and erasing elements
intDeque.insert(intDeque.begin(), 3);
intDeque.erase(intDeque.begin());
```

## Special Notes
- The class uses a block size of 32, but this can be adjusted as per requirement.
- The `iterator` and `const_iterator` classes are provided for easy navigation through the Deque.
- The implementation ensures that resources are properly managed to prevent memory leaks.

## Installation
- Include the `Deque` class definition in your C++ project.
- Compile your project with a C++11 (or later) compatible compiler.
