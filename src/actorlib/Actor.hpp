#ifndef ACTOR_HPP
#define ACTOR_HPP

#pragma once

#include <stdlib.h>
#include <utility>
#include <unordered_map>
#include <string>
#include <cstdint>

#include "Port.hpp"
#include "InPort.hpp"
#include "OutPort.hpp"


class Actor
{
public:
	std::string name;
	uint64_t threadRank;
	uint64_t actorSrNo;
	uint64_t actorGlobID;
	uint64_t triggers;

	std::unordered_map<std::string, Port* > inPortList;
	std::unordered_map<std::string, Port* > outPortList;

	void addInPort(Port* inPort);
	void addOutPort(Port* outPort);
	uint64_t trigger();
	uint64_t detrigger();
	bool isTriggered();

    template<typename T, int capacity> InPort<T, capacity> * getInPort(const std::string &portName);
    template<typename T, int capacity> OutPort<T, capacity> * getOutPort(const std::string &portName);

	Actor(std::string name, uint64_t threadRank, uint64_t actorSrNo);
	Actor(uint64_t threadRank, uint64_t ActorSrNo);

	virtual ~Actor();
	Actor(Actor &other) = delete;
	Actor & operator=(Actor &other) = delete;
	
	virtual void act() = 0;
	//void act();
	bool finished;
	//statics
	static uint64_t encodeGlobID(uint64_t procNo, uint64_t actNo);
	static std::pair<uint64_t,uint64_t> decodeGlobID(uint64_t inpGlobId);

};

template <typename T, int capacity> InPort <T, capacity> * Actor :: getInPort(const std::string &portName)
{
    auto it = inPortList.find(portName);
    if (it == inPortList.end())
        throw std::runtime_error("Could not find InPort " + portName);
    return static_cast <InPort <T, capacity> *> (it->second);
}
template <typename T, int capacity> OutPort <T, capacity> * Actor :: getOutPort(const std::string &portName)
{
    auto it = outPortList.find(portName);
    if (it == outPortList.end())
        throw std::runtime_error("Could not find OutPort " + portName);
    return static_cast <OutPort <T, capacity> *> (it->second);
}

#endif