target_sources(
    actorlib
    
    PUBLIC
    AbstractChannel.hpp
    Actor.hpp
    ActorGraph.hpp
    Channel.hpp
    DummyRemoteActor.hpp
    InPort.hpp
    LocalChannel.hpp
    OutPort.hpp
    Port.hpp
    RemoteChannel.hpp
    utils/gpi_utils.hpp
    utils/connection_type_util.hpp
    
    PRIVATE
    AbstractChannel.cpp
    Actor.cpp
    ActorGraph.cpp
    DummyRemoteActor.cpp
)
