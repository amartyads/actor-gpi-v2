#include "Actor.hpp"
#include "InPort.hpp"
#include "OutPort.hpp"
#include <stdlib.h>
#include <vector>
#include <string>
#include <utility>
#include <cstdint>
#include <stdexcept>

#include <iostream>

Actor::Actor(std::string name, uint64_t threadRank, uint64_t actorSrNo)
{
	this->name = name;
	this->threadRank = threadRank;
	this->actorSrNo = actorSrNo;
	this->actorGlobID = Actor::encodeGlobID(threadRank, actorSrNo);
	this->triggers = 0;
	finished = false;
}
Actor::Actor(uint64_t rank, uint64_t srno) : Actor("A-"+std::to_string(rank)+"-"+std::to_string(srno), rank, srno) { }

Actor::~Actor()
{
	for(auto &ip : inPortList)
		delete ip.second;
	for(auto &op : outPortList)
		delete op.second;
}

void Actor::addInPort(Port* inPort)
{
	inPortList[inPort->name] = inPort;
}
void Actor::addOutPort(Port* outPort)
{
	outPortList[outPort->name] = outPort;
}

uint64_t Actor::trigger()
{
	triggers++;
	return triggers;
}
uint64_t Actor::detrigger()
{
	triggers--;
	return triggers;
}
bool Actor::isTriggered()
{
	return (triggers > 0);
}

uint64_t Actor::encodeGlobID(uint64_t procNo, uint64_t actNo) //static
{
	return (procNo << 20) | actNo;
}

std::pair<uint64_t,uint64_t> Actor::decodeGlobID(uint64_t inpGlobId) //static
{
	uint64_t procNo = inpGlobId >> 20;
	uint64_t actNo = inpGlobId & ((1 << 20) - 1);
	return (std::make_pair(procNo, actNo));
}