#ifndef INPORT_HPP
#define INPORT_HPP

#include "Channel.hpp"
#include "Port.hpp"

#include <string>
#include <vector>

#pragma once
template <typename T, int capacity> class InPort : public Port
{
public:
    Channel<T, capacity> *connChannel;
    //T data;

    InPort(Channel<T, capacity>* channel);
    InPort(Channel<T, capacity>* channel, std::string name);

    T read();     // read from channel, return
    uint64_t available(); // poll channel to see if data available

    ~InPort();
	InPort(InPort &other) = delete;
	InPort & operator=(InPort &other) = delete;
};

template <typename T, int capacity> InPort <T, capacity> ::InPort(Channel<T, capacity>* channel, std::string name)
{
    connChannel = channel;
    this->name = name;
}

template <typename T, int capacity> InPort <T, capacity> ::InPort(Channel<T, capacity>* channel) : InPort(channel, "temp")  { }

template <typename T, int capacity> InPort <T, capacity> :: ~InPort()
{
    delete this->connChannel;
}

template <typename T, int capacity> T InPort <T, capacity> :: read()
{
    return (connChannel->pullData());
}

template <typename T, int capacity> uint64_t InPort <T, capacity> :: available()
{
    return (connChannel->isAvailableToPull());
}

#endif