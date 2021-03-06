#ifndef _CONN_NODE_GUILDSRV_H__ 
#define _CONN_NODE_GUILDSRV_H__

#include "conn_node.h"
#include <vector>

typedef size_t (*pack_func)(const void *message, uint8_t *out);

class conn_node_guildsrv: public conn_node_base
{
	typedef int (conn_node_guildsrv::*handle_func)(EXTERN_DATA*);
	typedef std::map<uint32_t, handle_func> HandleMap;
public:
	conn_node_guildsrv();
	virtual ~conn_node_guildsrv();

	void add_msg_handle(uint32_t msg_id, handle_func func);

	virtual int recv_func(evutil_socket_t fd);

	static void send_system_notice(uint64_t player_id, uint32_t id, std::vector<char*> *args, uint64_t target_id = 0);
	static int broadcast_message(uint16_t msg_id, void *msg_data, pack_func packer, std::vector<uint64_t> &players);
	
	static conn_node_guildsrv connecter;
private:
	int handle_guild_list_request(EXTERN_DATA *extern_data);
	int handle_guild_info_request(EXTERN_DATA *extern_data);
	int handle_guild_member_list_request(EXTERN_DATA *extern_data);
	int handle_guild_create_request(EXTERN_DATA *extern_data);
	int handle_guild_join_request(EXTERN_DATA *extern_data);
	int handle_guild_join_list_request(EXTERN_DATA *extern_data);
	int handle_guild_deal_join_request(EXTERN_DATA *extern_data);
	int handle_guild_turn_switch_request(EXTERN_DATA *extern_data);
	int handle_guild_set_words_request(EXTERN_DATA *extern_data);
	int handle_guild_appoint_office_request(EXTERN_DATA *extern_data);
	int handle_guild_kick_request(EXTERN_DATA *extern_data);
	int handle_guild_rename_request(EXTERN_DATA *extern_data);
	int handle_guild_exit_request(EXTERN_DATA *extern_data);
	int handle_guild_set_permission_request(EXTERN_DATA *extern_data);
	int handle_guild_invite_request(EXTERN_DATA *extern_data);
	int handle_guild_deal_invite_request(EXTERN_DATA *extern_data);
	int handle_guild_get_level_gift_request(EXTERN_DATA *extern_data);

	int handle_open_guild_answer_request(EXTERN_DATA *extern_data);

	int handle_player_online_notify(EXTERN_DATA *extern_data);
	int handle_check_and_cost_answer(EXTERN_DATA *extern_data);
	int handle_gamesrv_reward_answer(EXTERN_DATA *extern_data);
	int handle_add_guild_resource_request(EXTERN_DATA *extern_data);
	int handle_disband_request(EXTERN_DATA *extern_data);
	int handle_gamesrv_start(EXTERN_DATA *extern_data);

	int handle_guild_building_info_request(EXTERN_DATA *extern_data);
	int handle_guild_building_upgrade_request(EXTERN_DATA *extern_data);
	int handle_sub_guild_building_time_request(EXTERN_DATA *extern_data);
	int handle_guild_accept_task_request(EXTERN_DATA *extern_data);
	int handle_game_accept_task_answer(EXTERN_DATA *extern_data);
	int handle_game_task_finish_notify(EXTERN_DATA *extern_data);

	int handle_guild_skill_info_request(EXTERN_DATA *extern_data);
	int handle_guild_skill_develop_request(EXTERN_DATA *extern_data);
	int handle_guild_skill_pratice_request(EXTERN_DATA *extern_data);

	int handle_guild_shop_info_request(EXTERN_DATA *extern_data);
	int handle_guild_shop_buy_request(EXTERN_DATA *extern_data);

	int handle_guild_chat_request(EXTERN_DATA *extern_data);
	int handle_guild_prodece_medicine_request(EXTERN_DATA *extern_data);
	int handle_get_other_info_request(EXTERN_DATA *extern_data);
	int handle_trade_lot_insert(EXTERN_DATA *extern_data);

	int handle_guild_battle_call_request(EXTERN_DATA *extern_data);
	int handle_guild_battle_enter_wait_request(EXTERN_DATA *extern_data);
	int handle_guild_battle_info_request(EXTERN_DATA *extern_data);
	int handle_guild_battle_fight_reward_request(EXTERN_DATA *extern_data);
	int handle_guild_battle_sync_begin(EXTERN_DATA *extern_data);
	int handle_guild_battle_sync_end(EXTERN_DATA *extern_data);
	int handle_guild_battle_sync_settle(EXTERN_DATA *extern_data);
	int notify_guild_battle_activity_settle(EXTERN_DATA *extern_data);
	int handle_guild_battle_final_list_request(EXTERN_DATA *extern_data);
	int handle_guild_battle_add_final_id(EXTERN_DATA *extern_data);

	int handle_guild_ruqin_creat_monster_level_request(EXTERN_DATA *extern_data);
	int guild_ruqin_reward_info_notify(EXTERN_DATA *extern_data);
	int guild_ruqin_boss_creat_notify(EXTERN_DATA *extern_data);

	int handle_guild_donate_request(EXTERN_DATA *extern_data);

	int handle_activity_shidamenzong_give_reward_request(EXTERN_DATA *extern_data);

	int handle_guild_bonfire_open_request(EXTERN_DATA *extern_data);
	int handle_guild_bonfire_open_answer(EXTERN_DATA *extern_data);
	int handle_guild_bonfire_reward(EXTERN_DATA *extern_data);

private:
	HandleMap   m_handleMap;
};



#endif

