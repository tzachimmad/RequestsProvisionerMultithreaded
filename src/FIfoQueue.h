#ifndef FIFOQUEUE_H_
#define FIFOQUEUE_H_

#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>
#include <atomic>
using namespace std;



class StringWrapper
{
public:
	StringWrapper(string str) : m_str(str){}
	virtual ~StringWrapper(){}
	string m_str;
};

template<typename T>
class PCQueue {
    const size_t size;
    std::unique_ptr<T[]> contents;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
public:
    explicit PCQueue(size_t s) : size(s), contents(new T[s]), head(0), tail(0) {}

    //num of elements in ringbuffer (size - room())
    size_t count() {
        return (tail + size - head) % size;
    }

    //num of free spaces in ringbuffer (size - count())
    size_t room() {
        return (head + size - tail) % size;
    }

    void push(T t) {
        size_t newTail = (tail + 1) % size;
        if (newTail == head) {
            throw std::runtime_error("Pushing Full PCQueue");
        }
        contents[tail] = t;
        tail = newTail;//std::atomic implicitly memfences, so this op must occur after the "contents[tail] = t;"
    }

    T pop() {
        if (tail == head) {
            throw std::runtime_error("Popping Empty PCQueue");
        }
        T ret = contents[head];
        size_t newHead = (head + 1) % size;
        head = newHead;
        return ret;
    }
};
#endif
