#ifndef _CONN_NODE_CLIENT_H__
#define _CONN_NODE_CLIENT_H__

#include "conn_node.h"
#include "game_event.h"

#define MAX_ITEMSRV_SEND_BUFFER_SIZE (1024*64)

class conn_node_item: public conn_node_base
{
public:
	conn_node_item();
	virtual ~conn_node_item();

	virtual int recv_func(evutil_socket_t fd);
	virtual int send_one_msg(PROTO_HEAD *head, uint8_t force);
	virtual struct event* get_write_event() { return &ev_write; }

	static std::map<evutil_socket_t, conn_node_item *> map_fd_nodes;
	static std::map<uint64_t, conn_node_item *> map_player_id_nodes;
	static std::map<uint32_t, conn_node_item *> map_open_id_nodes;	
	
	static int add_map_fd_nodes(conn_node_item *client);
	static int add_map_player_id_nodes(conn_node_item *client);
	static int add_map_open_id_nodes(conn_node_item *client);		
	static conn_node_item * get_nodes_by_fd(evutil_socket_t fd, uint16_t port);
	static conn_node_item * get_nodes_by_player_id(uint64_t player_id);
	static conn_node_item * get_nodes_by_open_id(uint32_t open_id);	


public:
	void send_data_to_client();


public:
	uint16_t login_seq;  //登录的seq号，登录包返回的时候比较这个seq，不一致则丢弃
	uint16_t seq;       //客户端发包的seq号，每次加1
	uint32_t open_id;
	uint64_t player_id;
	struct event ev_write;

private:	
	char send_buffer[MAX_ITEMSRV_SEND_BUFFER_SIZE];
	int32_t send_buffer_begin_pos;
	int32_t send_buffer_end_pos;

private:
	int decode_and_check_crc(PROTO_HEAD *head);
	int dispatch_message();

	int send_player_exit(bool again = false);
};

#endif
