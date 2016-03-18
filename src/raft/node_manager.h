// libraft - Quorum-based replication of states across machines.
// Copyright (c) 2015 Baidu.com, Inc. All Rights Reserved

// Author: Zhangyi Chen (chenzhangyi01@baidu.com)
// Date: 2015/12/24 15:25:32

#ifndef  PUBLIC_RAFT_NODE_MANAGER_H
#define  PUBLIC_RAFT_NODE_MANAGER_H

#include <base/memory/singleton.h>
#include <base/containers/doubly_buffered_data.h>
#include "raft/raft.h"
#include "raft/util.h"

namespace raft {

class NodeImpl;

class NodeManager {
public:
    static NodeManager* GetInstance() {
        return Singleton<NodeManager>::get();
    }

    // add raft node
    bool add(NodeImpl* node);

    // remove raft node
    bool remove(NodeImpl* node);

    // get node by group_id and peer_id
    scoped_refptr<NodeImpl> get(const GroupId& group_id, const PeerId& peer_id);

    // get all the nodes of |group_id|
    void get_nodes_by_group_id(const GroupId& group_id, 
                               std::vector<scoped_refptr<NodeImpl> >* nodes);

    void get_all_nodes(std::vector<scoped_refptr<NodeImpl> >* nodes);

    // Add service to |server| at |listen_addr|
    int add_service(baidu::rpc::Server* server, 
                    const base::EndPoint& listen_addr);

    // Return true if |addr| is reachable by a RPC Server
    bool server_exists(base::EndPoint addr);

    // Remove the addr from _addr_set when the backing service is destroyed
    void remove_address(base::EndPoint addr);

private:
    NodeManager();
    ~NodeManager();
    DISALLOW_COPY_AND_ASSIGN(NodeManager);
    friend struct DefaultSingletonTraits<NodeManager>;
    
    // TODO(chenzhangyi01): replace std::map with FlatMap
    // To make implementation simplicity, we use two maps here, although
    // it works practically with only one GroupMap
    typedef std::map<NodeId, NodeImpl*> NodeMap;
    typedef std::multimap<GroupId, NodeImpl*> GroupMap;
    struct Maps {
        NodeMap node_map;
        GroupMap group_map;
    };
    // Functor to modify DBD
    static size_t _add_node(Maps&, const NodeImpl* node);
    static size_t _remove_node(Maps&, const NodeImpl* node);

    base::DoublyBufferedData<Maps> _nodes;

    raft_mutex_t _mutex;
    std::set<base::EndPoint> _addr_set;
};

}   // namespace raft

#endif  // PUBLIC_RAFT_NODE_MANAGER_H