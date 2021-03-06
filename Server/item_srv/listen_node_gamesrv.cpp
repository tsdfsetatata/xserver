#include "listen_node_gamesrv.h"
#include "game_event.h"
#include <assert.h>

listen_node_gamesrv::listen_node_gamesrv()
{
}

listen_node_gamesrv::~listen_node_gamesrv()
{
}

int listen_node_gamesrv::listen_pre_func()
{
	if (item_node_gamesrv::server_node == NULL)
		return (0);
	LOG_ERR("%s %d: only one server can connect", __PRETTY_FUNCTION__, __LINE__);
	return (-1);
}

int listen_node_gamesrv::listen_after_func(evutil_socket_t fd)
{
	return (0);		
}

conn_node_base * listen_node_gamesrv::get_conn_node(evutil_socket_t fd)
{
	item_node_gamesrv *ret = new item_node_gamesrv();
	ret->fd = fd;
	assert(item_node_gamesrv::server_node == NULL);
	item_node_gamesrv::server_node = ret;
	if (item_node_gamesrv::send_all_cached_buf() != 0) {
		delete ret;
		item_node_gamesrv::server_node = NULL;
		return NULL;
	}
	return ret;
}

