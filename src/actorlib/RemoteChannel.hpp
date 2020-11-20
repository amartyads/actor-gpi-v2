#ifndef REMOTE_CHANNEL_HPP
#define REMOTE_CHANNEL_HPP

#pragma once

#include "connection-type-util.hpp"
#include "gpi-utils.hpp"
#include "AbstractChannel.hpp"
#include "Channel.hpp"

#include <GASPI.h>
#include <GASPI_Ext.h>
#include <vector>
#include <iostream>

#include <stdexcept>
#include <cstdint>

template <typename T, int capacity> class RemoteChannel: public Channel<T, capacity>
{
public:

    void pushData(T ndata);
    T pullData();

    uint64_t isAvailableToPush();
    uint64_t isAvailableToPull();

    int blockSize;
    int64_t pulledDataoffset;
    
    int queueLocation;
    gaspi_queue_id_t queue_id;
    RemoteChannel(int queueCapacity, ActorConnectionType currConnectionType, uint64_t srcID, uint64_t dstID);
};

template <typename T, int capacity> class RemoteChannel<std::vector <T> ,capacity>: public Channel<std::vector<T>, capacity>
{
public:

    void pushData(std::vector<T> &ndata);
    std::vector<T> pullData();

    uint64_t isAvailableToPush();
    uint64_t isAvailableToPull();

    int blockSize;
    int64_t pulledDataoffset;
    
    int queueLocation;
    gaspi_queue_id_t queue_id;
    RemoteChannel(int queueCapacity, ActorConnectionType currConnectionType, uint64_t srcID, uint64_t dstID);
};


#ifndef ASSERT
#define ASSERT(ec) gpi_util::success_or_exit(__FILE__,__LINE__,ec)
#endif


template <typename T, int capacity> RemoteChannel<T, capacity>::RemoteChannel(int queueCapacity, ActorConnectionType currConnectionType, uint64_t srcID, uint64_t dstID)
{
    this->currConnectionType = currConnectionType;
    this->maxQueueSize = queueCapacity;
    this->curQueueSize = queueCapacity;
    this->dstID = dstID;
    this->srcID = srcID;
    queueLocation = 0;
    queue_id = 0;
    pulledDataoffset = -1;
    this->initialized = false;
}

template <typename T, int capacity> RemoteChannel<std::vector<T>, capacity>::RemoteChannel(int queueCapacity, ActorConnectionType currConnectionType, uint64_t srcID, uint64_t dstID)
{
    this->currConnectionType = currConnectionType;
    this->maxQueueSize = queueCapacity;
    this->curQueueSize = queueCapacity;
    this->dstID = dstID;
    this->srcID = srcID;
    queueLocation = 0;
    queue_id = 0;
    pulledDataoffset = -1;
    this->initialized = false;
}

template<typename T, int capacity> std::vector<T> RemoteChannel <std::vector<T>, capacity> :: pullData()
{
    //data availability already checked

    if(pulledDataoffset == -1)
        throw std::runtime_error("No data in channel");
    //gaspi read from fixedoffset * sizeof uint64_t for databank ofset (in isavailabletopull)

    //read from no of datablock offset
    uint64_t bytesUsed;
    gpi_util::wait_for_queue_entries(&queue_id, 1);
    ASSERT (gaspi_read ( 4, 0
                        , this->remoteRank, 5, ((this->fixedDataOffset * this->maxQueueSize) + queueLocation) *sizeof(uint64_t)
                        , sizeof(uint64_t), queue_id, GASPI_BLOCK
                        )
            );
    ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
    bytesUsed = ((uint64_t *)(this->cachePtr))[0];

    //calc no of datablocks
    this->noOfDatablocksUsed = ((bytesUsed - 1) / this->minBlockSize) + 1;
    //loop thru consecutive datablocks (minblocksize is the same for all ranks)
    uint64_t bytesLeft = bytesUsed;
    uint64_t vecSize, elemsPerFullCache;
    std::vector<T> readData;
    vecSize = bytesUsed / sizeof(T);
    elemsPerFullCache = this->minBlockSize / sizeof(T);
    int64_t copyOfPulledDataOffset = this->pulledDataoffset;
    for(uint64_t i = 0; i < this->noOfDatablocksUsed; i++)
    {
        gpi_util::wait_for_queue_entries(&queue_id, 1);
        ASSERT (gaspi_read ( 4, 0
                            , this->remoteRank, 3, copyOfPulledDataOffset
                            , this->minBlockSize, queue_id, GASPI_BLOCK
                            )
                );
        ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
        for(uint64_t j = 0; j < (vecSize < elemsPerFullCache ? vecSize : elemsPerFullCache); j++)
        {
            readData.push_back(((T *)(this->cachePtr))[j]);
        }
        
        vecSize -= elemsPerFullCache;
        copyOfPulledDataOffset += this->minBlockSize;
    }
    ((uint64_t *)(this->cachePtr))[0] = pulledDataoffset;

    gpi_util::wait_for_queue_entries(&queue_id, 1);
    ASSERT (gaspi_write ( 4, 0
                        , this->remoteRank, 1, ((this->fixedTriggerOffset * this->maxQueueSize) + queueLocation) *sizeof(uint64_t)
                        , sizeof(uint64_t), queue_id, GASPI_BLOCK
                        )
            );
    ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
    
    pulledDataoffset = -1;
    //else
    //{
    //}
    queueLocation++;
    queueLocation %= this->maxQueueSize;
    return readData;
}

template <typename T, int capacity> T RemoteChannel <T, capacity> :: pullData()
{
    //data availability already checked
    if(pulledDataoffset == -1)
        throw std::runtime_error("No data in channel");
    //gaspi read from fixedoffset * sizeof uint64_t for databank ofset (in isavailabletopull)

    //read from no of datablock offset
    uint64_t bytesUsed;
    gpi_util::wait_for_queue_entries(&queue_id, 1);
    ASSERT (gaspi_read ( 4, 0
                        , this->remoteRank, 5, ((this->fixedDataOffset * this->maxQueueSize) + queueLocation) *sizeof(uint64_t)
                        , sizeof(uint64_t), queue_id, GASPI_BLOCK
                        )
            );
    ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
    bytesUsed = ((uint64_t *)(this->cachePtr))[0];

    //calc no of datablocks
    this->noOfDatablocksUsed = ((bytesUsed - 1) / this->minBlockSize) + 1;
    //loop thru consecutive datablocks (minblocksize is the same for all ranks)
    uint64_t bytesLeft = bytesUsed;
    uint64_t vecSize, elemsPerFullCache;
    //prepare reading vector
    std::vector<T> readData;
    vecSize = 1;
    elemsPerFullCache = this->minBlockSize / sizeof(T);
    for(uint64_t i = 0; i < this->noOfDatablocksUsed; i++)
    {
        gpi_util::wait_for_queue_entries(&queue_id, 1);
        ASSERT (gaspi_read ( 4, 0
                            , this->remoteRank, 3, ((this->pulledDataoffset))
                            , this->minBlockSize, queue_id, GASPI_BLOCK
                            )
                );
        ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
        readData.push_back(((T *)(this->cachePtr))[0]);
    }
    ((uint64_t *)(this->cachePtr))[0] = pulledDataoffset;
    gpi_util::wait_for_queue_entries(&queue_id, 1);
    ASSERT (gaspi_write ( 4, 0
                        , this->remoteRank, 1, ((this->fixedTriggerOffset * this->maxQueueSize) + queueLocation) *sizeof(uint64_t)
                        , sizeof(uint64_t), queue_id, GASPI_BLOCK
                        )
            );
    ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
    
    pulledDataoffset = -1;
    //else
    //{
    //}
    queueLocation++;
    queueLocation %= this->maxQueueSize;
    return readData[0];
    //gaspi read from databank offset, of amount bytesUsed, minBlocksize at a time
    
    //gaspi write to localClear the databank offset, local actorgraph will divide by blocksize
    
}

template <typename T, int capacity> void RemoteChannel <std::vector<T>, capacity> :: pushData (std::vector<T> &ndata)
{
    this->noOfDatablocksUsed = (((ndata.size() * sizeof(T) )- 1) / this->minBlockSize) + 1;
    uint64_t slot;
    bool notFound = true;
    for(slot = 0; slot < AbstractChannel::lookupTable.size(); slot++)
    {
        if(AbstractChannel::lookupTable[slot] == -1)
        {
            notFound = false;
            for(uint64_t i = 1; i < this->noOfDatablocksUsed; i++)
            {
                if(AbstractChannel::lookupTable[slot + i] != -1)
                {
                    slot += i-1;
                    notFound = true;
                    break;
                }
            }
            if(!notFound)
            {
                AbstractChannel::lookupTable[slot] = this->fixedDataOffset;
                for(uint64_t i = 1; i < this->noOfDatablocksUsed; i++)
                {
                    AbstractChannel::lookupTable[slot + i] = -2;
                }
                
                break;
            }
        }
    }
    if(notFound)
    {
        throw std::runtime_error("Too much data in channel");
    }
    //multiply index with minblocksize
    uint64_t localDatabankOffset = slot * this->minBlockSize; //blocksize includes sizeof
    //write to (localCommPtr) + fixedoffset * uint64 the databank offset (index*minblocksize)
    auto commPtr = (uint64_t *)(this->fixedCommInitPtr);
    commPtr[(this->fixedDataOffset * this->maxQueueSize) + queueLocation] = localDatabankOffset;
    //write to databank (localdataptr + index*minblock)
    gaspi_pointer_t tempData = ((char*)(this->fixedDatabankInitPtr))  + localDatabankOffset;

    //must split ndata into mindatablock sizes so as to not split data across edge

    for(int i = 0; i < ndata.size(); i++)
    {
        ((T *)(tempData))[i] = ndata[i];
    }

    //write to vec size segment
    uint64_t bytesUsed;
    bytesUsed = ndata.size() * sizeof(T);
    auto sizePtr = (uint64_t *)(this->fixedVecSizeInitPtr);
    sizePtr[(this->fixedDataOffset * this->maxQueueSize) + queueLocation] = bytesUsed;

    //gaspi write trigger to remote actorgraph

   
    gpi_util::wait_for_queue_entries(&queue_id, 1);
    ((uint64_t *)(this->cachePtr))[0] = this->dstID;
    ASSERT (gaspi_write ( 4, 0
                        , this->remoteRank, 2, ((this->fixedTriggerOffset * this->maxQueueSize) + queueLocation) *sizeof(uint64_t)
                        , sizeof(uint64_t), queue_id, GASPI_BLOCK
                        )
            );

    ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
    queueLocation++;
    queueLocation %= this->maxQueueSize;
    this->curQueueSize--;
}

template <typename T, int capacity> void RemoteChannel <T, capacity> :: pushData(T ndata)
{
    //space already checked
    //calculate no of datablocks needed
    this->noOfDatablocksUsed = 1;
    
    //std::cout << this->fixedDataOffset << " " << queueLocation << std::endl;
    //reserve slots from static array
    uint64_t slot;
    bool notFound = true;
    for(slot = 0; slot < AbstractChannel::lookupTable.size(); slot++)
    {
        if(AbstractChannel::lookupTable[slot] == -1)
        {
            notFound = false;
            AbstractChannel::lookupTable[slot] = this->fixedDataOffset;
            break;
        }
    }
    if(notFound)
    {
        throw std::runtime_error("Too much data in channel");
    }
    //multiply index with minblocksize
    uint64_t localDatabankOffset = slot * this->minBlockSize; //blocksize includes sizeof
    //write to (localCommPtr) + fixedoffset * uint64 the databank offset (index*minblocksize)
    auto commPtr = (uint64_t *)(this->fixedCommInitPtr);
    commPtr[(this->fixedDataOffset * this->maxQueueSize) + queueLocation] = localDatabankOffset;
    //write to databank (localdataptr + index*minblock)
    gaspi_pointer_t tempData = ((char*)(this->fixedDatabankInitPtr))  + localDatabankOffset;

    ((T *)(tempData))[0] = ndata;

    //write to vec size segment
    gpi_util::wait_for_queue_entries(&queue_id, 2);
    uint64_t bytesUsed;

    bytesUsed = sizeof(T);

    auto sizePtr = (uint64_t *)(this->fixedVecSizeInitPtr);
    sizePtr[(this->fixedDataOffset * this->maxQueueSize) + queueLocation] = bytesUsed;

    //gaspi write trigger to remote actorgraph

   
    ((uint64_t *)(this->cachePtr))[0] = this->dstID;
    ASSERT (gaspi_write ( 4, 0
                        , this->remoteRank, 2, ((this->fixedTriggerOffset * this->maxQueueSize) + queueLocation) *sizeof(uint64_t)
                        , sizeof(uint64_t), queue_id, GASPI_BLOCK
                        )
            );

    ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
    queueLocation++;
    queueLocation %= this->maxQueueSize;
    this->curQueueSize--;
}

template <typename T, int capacity> uint64_t RemoteChannel <T, capacity> :: isAvailableToPull()
{
    //std::cout << this->fixedDataOffset << " " << queueLocation << " rem " << this->remoteRank << std::endl;
    //gaspi read databank offset
    gpi_util::wait_for_queue_entries(&queue_id, 1);
    ASSERT (gaspi_read ( 4, 0
                        , this->remoteRank, 0, ((this->fixedDataOffset * this->maxQueueSize) + queueLocation) *sizeof(uint64_t)
                        , sizeof(uint64_t), queue_id, GASPI_BLOCK
                        )
            );
    ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
    pulledDataoffset = ((int64_t *)(this->cachePtr))[0];
    //std::cout << pulledDataoffset << std::endl;
    return (pulledDataoffset > -1); 
    //return (this->curQueueSize < this->maxQueueSize);
}
template <typename T, int capacity> uint64_t RemoteChannel <T, capacity> :: isAvailableToPush()
{
    if (this->curQueueSize <= 0)
        return 0;
    
    //find longest datablock len in bytes
    uint64_t foundSlot = 0, temp = 0;
    for(uint64_t i = 0; i < AbstractChannel::lookupTable.size(); i++)
    {
        if(AbstractChannel::lookupTable[i] == -1)
        {
            temp++;
        }
        else
        {
            foundSlot = foundSlot > temp ? foundSlot:temp;
            temp = 0;
        }
    }
    foundSlot = foundSlot > temp ? foundSlot:temp;
    foundSlot *= this->minBlockSize;
    return foundSlot;
}

template <typename T, int capacity> uint64_t RemoteChannel <std::vector<T>, capacity> :: isAvailableToPull()
{
    //std::cout << this->fixedDataOffset << " " << queueLocation << " rem " << this->remoteRank << std::endl;
    //gaspi read databank offset
    gpi_util::wait_for_queue_entries(&queue_id, 1);
    ASSERT (gaspi_read ( 4, 0
                        , this->remoteRank, 0, ((this->fixedDataOffset * this->maxQueueSize) + queueLocation) *sizeof(uint64_t)
                        , sizeof(uint64_t), queue_id, GASPI_BLOCK
                        )
            );
    ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));
    pulledDataoffset = ((int64_t *)(this->cachePtr))[0];
    //std::cout << pulledDataoffset << std::endl;
    return (pulledDataoffset > -1); 
    //return (this->curQueueSize < this->maxQueueSize);
}
template <typename T, int capacity> uint64_t RemoteChannel <std::vector<T>, capacity> :: isAvailableToPush()
{
    if (this->curQueueSize <= 0)
        return 0;
    
    //find longest datablock len in bytes
    uint64_t foundSlot = 0, temp = 0;
    for(uint64_t i = 0; i < AbstractChannel::lookupTable.size(); i++)
    {
        if(AbstractChannel::lookupTable[i] == -1)
        {
            temp++;
        }
        else
        {
            foundSlot = foundSlot > temp ? foundSlot:temp;
            temp = 0;
        }
    }
    foundSlot = foundSlot > temp ? foundSlot:temp;
    foundSlot *= this->minBlockSize;
    return foundSlot;
}

#endif