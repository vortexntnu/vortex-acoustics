#include <memory>
#include "Arduino.h"
#pragma once

/**
 @brief a ring buffer is a concept to store a maximum amount of data in a "ring". when a new value has to be savec, it will be put at the "end"
 the end will be incremented so that the next value will be written one adress further. When reading it will read the first value not read.
this way reading and writing can happen in any order, as long as no more data than the SIZE is written before reading. If not, some data will be lost
without processing it
 */
// TODO:
// make a parent ringbuffer class and have the different buffer sizes inherit it.
// for variable size
// and maybe also add a parameter to choose the buffer lenght
// ! This is not really implemented !!!!!!!!!!!!!!!
class RingBuffer_16bit
{

public:
    static constexpr int SIZE = 1500; /// 1500 uint16_t can be stored in 1 ringbuffer

    RingBuffer_16bit()
        : buffer(), start{0}, end{0}
    {
    }

    bool isEmpty()
    {
        return start == end;
    }
    void reset()
    {
        start = 0;
        end = 0;
    }

    void insert(uint16_t item)
    {
        buffer[end++] = item;
        end %= SIZE; /// when at the end of ringbuffer, just restarts at the start of the buffer
    }

    uint16_t get()
    {
        uint16_t item = buffer[start++];
        start %= SIZE;
        return item;
    }

    uint16_t operator++(int)
    { /// redefines the operator ++, because it is not defined by default (Ringbuffer++ would not work)
        return this->get();
    }

private:
    uint16_t buffer[SIZE];
    uint16_t start;
    uint16_t end;
};

class RingBuffer_32bit
{

public:
    static constexpr int SIZE = 1500; /// 1500 uint32_t can be stored in 1 ringbuffer

    RingBuffer_32bit()
        : buffer(), start{0}, end{0}
    {
    }

    bool isEmpty()
    {
        return start == end;
    }
    void reset()
    {
        start = 0;
        end = 0;
    }

    void insert(uint32_t item)
    {
        buffer[end++] = item;
        end %= SIZE; /// when at the end of ringbuffer, just restarts at the start of the buffer
    }

    uint32_t get()
    {
        uint32_t item = buffer[start++];
        start %= SIZE;
        return item;
    }

    uint32_t *get_buffer_pointer()
    {
        return buffer;
    }

    uint32_t
    operator++(int)
    { // redefines the operator ++, because it is not defined by default (Ringbuffer++ would not work)
        return this->get();
    }

private:
    uint32_t buffer[SIZE];
    uint16_t start;
    uint16_t end;
};
