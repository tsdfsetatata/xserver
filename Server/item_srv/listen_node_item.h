#ifndef LISTEN_NODE_CLIENT_H__
#define LISTEN_NODE_CLIENT_H__
#include "listen_node.h"
#include "conn_node_item.h"

class listen_node_item: public listen_node_base
{
public:
	listen_node_item();
	~listen_node_item();
	virtual int listen_pre_func();
	virtual int listen_after_func(evutil_socket_t fd);
	virtual conn_node_base * get_conn_node(evutil_socket_t fd);
};



#endif
