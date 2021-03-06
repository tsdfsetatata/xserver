#include "game_event.h"
#include <string.h>
#include "time_helper.h"
#include "player.h"
#include "uuid.h"
#include "msgid.h"
#include "player_manager.h"
#include "sight_space_manager.h"
#include "raid_manager.h"
#include "conn_node_dbsrv.h"
#include "scene_manager.h"
#include "game_config.h"
#include "team.h"
#include "camp_judge.h"
#include "monster_manager.h"
#include "../../proto/cast_skill.pb-c.h"
#include "../../proto/move.pb-c.h"
#include "../../proto/raid.pb-c.h"
#include "../../proto/move_direct.pb-c.h"
#include "../../proto/login.pb-c.h"
#include "../../proto/scene_transfer.pb-c.h"
#include "../../proto/bag.pb-c.h"
#include "../../proto/relive.pb-c.h"
#include "../../proto/role.pb-c.h"
#include "../../proto/chat.pb-c.h"
#include "../../proto/collect.pb-c.h"
#include "../../proto/task.pb-c.h"
#include "../../proto/team.pb-c.h"
#include "../../proto/equip.pb-c.h"
#include "../../proto/pk.pb-c.h"
#include "../../proto/pvp_raid.pb-c.h"
#include "../../proto/hotel.pb-c.h"
#include "../../proto/mail_db.pb-c.h"
#include "../../proto/shop.pb-c.h"
#include "../../proto/yuqidao.pb-c.h"
#include "../../proto/fashion.pb-c.h"
#include "../../proto/baguapai.pb-c.h"
#include "../../proto/activity.pb-c.h"
#include "../../proto/setting.pb-c.h"
#include "../../proto/zhenying.pb-c.h"
#include "../../proto/tower.pb-c.h"
#include "../../proto/answer.pb-c.h"
#include "../../proto/player_redis_info.pb-c.h"
#include "../../proto/personality.pb-c.h"
#include "../../proto/friend.pb-c.h"
#include "../../proto/guild_battle.pb-c.h"
#include "../../proto/xunbao.pb-c.h"
#include "../../proto/partner.pb-c.h"
#include "../../proto/gift_package.pb-c.h"
#include "../../proto/achievement.pb-c.h"
#include "../../proto/trade.pb-c.h"
#include "../../proto/player_fuli.pb-c.h"
#include "../../proto/guild.pb-c.h"
#include "../../proto/marry.pb-c.h"
#include "auto_add_hp.pb-c.h"
#include "horse.pb-c.h"
#include "unit_path.h"
#include "unit.h"
#include "cached_hit_effect.h"
#include "count_skill_damage.h"
#include "skill.h"
#include "buff_manager.h"
#include "error_code.h"
#include "app_data_statis.h"
#include "chat.h"
#include "collect.h"
#include "team.h"
#include "check_range.h"
#include "player_redis_info.pb-c.h"
#include "pvp_match_manager.h"
#include "chengjie.h"
#include "zhenying_raid_manager.h"
#include "zhenying_battle.h"
#include "guild_battle_manager.h"
#include "guild_wait_raid_manager.h"
#include "register_gamesrv.h"
#include "attr_calc.h"
#include "partner_manager.h"
#include "cash_truck_manager.h"
#include "server_level.h"
#include "guild_land_raid_manager.h"
#include "guild_land_active_manager.h"
#include <assert.h>
#include <errno.h>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <map>
#include <set>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

extern uint32_t sg_gm_cmd_open;
extern std::map<uint32_t, ProtoGuildInfo> guild_summary_map;
extern uint32_t guild_battle_manager_final_list_state;
extern uint32_t guild_battle_manager_final_list_tick;
extern bool activity_is_unlock_by_config(player_struct *player, EventCalendarTable *config);

//#define db_connecter(a,b) conn_node_dbsrv::connecter.send_one_msg(a,b)
//#define connecter conn_node_gamesrv::connecter

#define get_data_len() conn_node_gamesrv::connecter.get_data_len()
#define get_data() conn_node_gamesrv::connecter.get_data()
#define get_seq() conn_node_gamesrv::connecter.get_seq()

#define buf_head() conn_node_gamesrv::connecter.buf_head()

#define get_send_data() conn_node_base::get_send_data()
#define get_send_buf(A,B) conn_node_base::get_send_buf(A,B)
#define add_extern_data(A,B) conn_node_base::add_extern_data(A,B)

static int notify_head_icon_info(player_struct *player, EXTERN_DATA *extern_data);
static int notify_task_list(player_struct *player, EXTERN_DATA *extern_data);
static int notify_equip_list(player_struct *player, EXTERN_DATA *extern_data);
static int notify_yuqidao_info(player_struct *player, EXTERN_DATA *extern_data);
static int notify_pvp_raid_info(player_struct *player, int type, EXTERN_DATA *extern_data);
static int player_online_enter_scene_after(player_struct *player, EXTERN_DATA *extern_data);
static void player_online_to_other_srvs(player_struct *player, EXTERN_DATA *extern_data, bool reconnect);
static void player_ready_enter_scene(player_struct *player, EXTERN_DATA *extern_data, bool reconnect);
static int notify_baguapai_info(player_struct *player, EXTERN_DATA *extern_data);
static int notify_setting_switch_info(player_struct *player, EXTERN_DATA *extern_data);
static int notify_transfer_out_stuck_info(player_struct *player, EXTERN_DATA *extern_data);
static int notify_partner_info(player_struct *player, EXTERN_DATA *extern_data);
void notify_server_level_info(player_struct *player, EXTERN_DATA *extern_data);
void notify_server_level_break(player_struct *player, EXTERN_DATA *extern_data);
static int notify_achievement_info(player_struct *player, EXTERN_DATA *extern_data);
static int notify_title_info(player_struct *player, EXTERN_DATA *extern_data);
static int notify_strong_info(player_struct *player, EXTERN_DATA *extern_data);
static int notify_function_info(player_struct *player, EXTERN_DATA *extern_data);
static int handle_srv_reward_request(player_struct *player, EXTERN_DATA *extern_data, uint32_t msg_id);

GameHandleMap   m_game_handle_map;

static void add_msg_handle(uint32_t msg_id, game_handle_func func)
{
	m_game_handle_map[msg_id] = func;
}

static void send_comm_answer(uint32_t msgid, uint32_t result, EXTERN_DATA *extern_data)
{
	CommAnswer resp;
	comm_answer__init(&resp);
	resp.result = result;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, msgid, comm_answer__pack, resp);
}

static int comm_check_player_valid(player_struct *player, uint64_t player_id)
{
	if (!player) {
		LOG_ERR("%s %d: do not have player[%lu]", __FUNCTION__, __LINE__, player_id);
//		player = player_manager::create_tmp_player(extern_data->player_id);
		return (-1);
	}

	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, player_id);
		return -10;
	}

	if (!player->is_alive())
	{
		LOG_ERR("%s: player[%lu] dead", __FUNCTION__, player_id);
		return -20;
	}
	return (0);
}

static void send_trade_statis(uint64_t player_id, uint32_t operate_id, uint32_t ext_num1 = 0, uint32_t ext_num2 = 0, uint32_t ext_num3 = 0, uint32_t ext_num4 = 0, uint32_t ext_num5 = 0, uint64_t ext_num6 = 0)
{
	TRADE_STATIS_INSERT *req = (TRADE_STATIS_INSERT *)get_send_data();
	uint32_t data_len = sizeof(TRADE_STATIS_INSERT);
	memset(req, 0, data_len);
	req->player_id = player_id;
	req->operate_id = operate_id;
	req->ext_num1 = ext_num1;
	req->ext_num2 = ext_num2;
	req->ext_num3 = ext_num3;
	req->ext_num4 = ext_num4;
	req->ext_num5 = ext_num5;
	req->ext_num6 = ext_num6;
	
	EXTERN_DATA ext_data;
	fast_send_msg_base(conn_node_dbsrv::connecter, &ext_data, SERVER_PROTO_TRADE_STATIS, data_len, 0);
}

/*
static int set_player_pos_impl(player_struct *player, float pos_x, float pos_z)
{
	scene_struct *scene = player->scene;
	area_struct *old_area = player->area;
	set_unit_pos(&player->data->move_path, pos_x, pos_z);
	area_struct *new_area = scene->get_area_by_pos(pos_x, pos_z);
	if (old_area != new_area)
	{
		player->update_player_sight(old_area, new_area);
	}
	return (0);
}
*/

int handle_move_y_start_request_impl(player_struct *player, EXTERN_DATA *extern_data, MoveYStartRequest *req)
{
	player->data->pos_y = req->cur_pos_y;
	MoveYStartNotify notify;
	move_y_start_notify__init(&notify);
	notify.playerid = extern_data->player_id;
	notify.cur_pos = req->cur_pos;
	notify.cur_pos_y = req->cur_pos_y;
	notify.direct_y = req->direct_y;

	player->broadcast_to_sight(MSG_ID_MOVE_Y_START_NOTIFY, &notify, (pack_func)move_y_start_notify__pack, false);

	player->interrupt();
	return (0);
}

static void send_move_answer(int msgid, int result, struct position *pos, EXTERN_DATA *extern_data)
{
	MoveAnswer ans;
	PosData pos_data;
	move_answer__init(&ans);
	ans.result = result;
	if (result != 0 && pos)
	{
		pos_data__init(&pos_data);
		pos_data.pos_x = pos->pos_x;
		pos_data.pos_z = pos->pos_z;
		ans.pos = &pos_data;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, msgid, move_answer__pack, ans);	
}

int handle_move_start_request_impl(player_struct *player, EXTERN_DATA *extern_data, MoveStartRequest *req)
{
	if (!player->data || !player->scene)
	{
//		move_request__free_unpacked(req, NULL);
		LOG_ERR("%s %d: player[%lu] do not have data[%p] or scene[%p]", __FUNCTION__, __LINE__, extern_data->player_id, player->data, player->scene);
		return (-10);
	}

	uint64_t now = time_helper::get_cached_time();
	if (now - player->last_change_area_time < 500)
	{
		LOG_DEBUG("%s %d: player[%lu] request move too much[%ld]", __FUNCTION__, __LINE__, player->data->player_id, now - player->last_change_area_time);
	}
	player->last_change_area_time = now;

	LOG_INFO("%s: player[%lu] cur_pos[%.1f][%.1f] req_pos[%.1f][%.1f] directx[%d] directz[%d]",
		__FUNCTION__, player->get_uuid(), player->get_pos()->pos_x, player->get_pos()->pos_z,
		req->cur_pos->pos_x, req->cur_pos->pos_z, req->direct_x, req->direct_z);

	if (player->check_pos_distance(req->cur_pos->pos_x, req->cur_pos->pos_z) != 0)
	{
		struct position *pos = player->get_pos();
		LOG_ERR("%s %d: player[%lu] cur_pos[%.1f][%.1f] flash to [%.1f][%.1f]", __FUNCTION__, __LINE__, player->get_uuid(),
			pos->pos_x, pos->pos_z, req->cur_pos->pos_x, req->cur_pos->pos_z);
		send_move_answer(MSG_ID_MOVE_START_ANSWER, -1, pos, extern_data);
		if (player->is_unit_in_move())
		{
			player->stop_move();
		}
		return (0);
	}

//	player->data->pos_y = req->cur_pos_y;
	player->set_pos_with_broadcast(req->cur_pos->pos_x, req->cur_pos->pos_z);
//	set_player_pos_impl(player, req->cur_pos->pos_x, req->cur_pos->pos_z);
/*
	scene_struct *scene = player->scene;
		// TODO: 1: 检查当前位置
		// TODO: 2: 检查路径的阻挡

	area_struct *old_area = player->area;
	struct position *pos = player->get_player_pos();
	pos->pos_x = req->cur_pos->pos_x;
	pos->pos_z = req->cur_pos->pos_z;
*/

	// float c = req->direct_x * req->direct_x + req->direct_z * req->direct_z;
	// c = FastSqrtQ3(c);
	// if (c == 0)
	// {
	// 	LOG_ERR("%s %d: player %lu direct wrong", __FUNCTION__, __LINE__, extern_data->player_id);
	// 	return (-15);
	// }

	// double speed = player->get_speed();
	// player->data->move_path.direct_x = req->direct_x / c * speed;
	// player->data->move_path.direct_z = req->direct_z / c * speed;

	// 检查是否归一化
	int32_t t = req->direct_x * req->direct_x + req->direct_z * req->direct_z - 1000000;
	if (t > 200000 || t < -200000)
	{
		LOG_ERR("%s: player %lu not normalizing %d", __FUNCTION__, player->get_uuid(), t);
	}
	
	player->data->move_path.direct_x = req->direct_x / 1000.0;
	player->data->move_path.direct_z = req->direct_z / 1000.0;
	player->data->move_path.start_time = time_helper::get_cached_time();
	player->data->m_angle = pos_to_angle(req->direct_x, req->direct_z);
//		float t = player->data->m_angle / M_PI * 180 * -1 + 90;
//		LOG_DEBUG("%s: m_angle = %.3f, %.3f, %.3f %.3f", __FUNCTION__, player->data->m_angle, player->data->m_angle / M_PI, player->data->m_angle / M_PI * 180, t);	
/*
	area_struct *new_area = scene->get_area_by_pos(pos->pos_x, pos->pos_z);
	if (old_area != new_area)
	{
		player->update_player_sight(old_area, new_area);
	}

		//设置玩家的移动路径
	player->data->move_path.start_time = time_helper::get_cached_time();
*/
/*
	player->data->move_path.cur_pos = 0;
	player->data->move_path.max_pos = req->n_data - 1;
	if (player->data->move_path.max_pos >= MAX_PATH_POSITION)
		player->data->move_path.max_pos = MAX_PATH_POSITION - 1;
	for (size_t i = 0; i <= player->data->move_path.max_pos; ++i)
	{
		player->data->move_path.pos[i].pos_x = req->data[i]->pos_x;
		player->data->move_path.pos[i].pos_z = req->data[i]->pos_z;
	}
*/
//	CommAnswer resp;
//	comm_answer__init(&resp);
//	uint32_t ret;

//	FAST_SEND_TO_CLIENT(MSG_ID_MOVE_ANSWER, comm_answer__pack);
//	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MOVE_START_ANSWER, comm_answer__pack, resp);
	send_move_answer(MSG_ID_MOVE_START_ANSWER, 0, NULL, extern_data);	

	MoveStartNotify notify;
	move_start_notify__init(&notify);
	notify.playerid = extern_data->player_id;
	if (player->data->truck.on_truck)
	{
		cash_truck_struct *truck = cash_truck_manager::get_cash_truck_by_id(player->data->truck.truck_id);
		if (truck != NULL)
		{
			notify.playerid = truck->get_uuid();
			if (truck != NULL)
			{
				truck->set_pos_with_broadcast(req->cur_pos->pos_x, req->cur_pos->pos_z);
			}
		}
	}
	
	notify.cur_pos = req->cur_pos;
	notify.cur_pos_y = player->data->pos_y;
	notify.direct_x = req->direct_x;
//	notify.direct_y = req->direct_y;
	notify.direct_z = req->direct_z;

	player->broadcast_to_sight(MSG_ID_MOVE_START_NOTIFY, &notify, (pack_func)move_start_notify__pack, false);
	player->send_player_move_start_to_aisrv(&notify);
	
	player->interrupt();
	return (0);
}

int handle_move_y_stop_request_impl(player_struct *player, EXTERN_DATA *extern_data, MoveYStopRequest *req)
{
	player->data->pos_y = req->cur_pos_y;
	MoveYStopNotify notify;
	move_y_stop_notify__init(&notify);
	notify.playerid = extern_data->player_id;
	notify.cur_pos = req->cur_pos;
	notify.cur_pos_y = req->cur_pos_y;

	player->broadcast_to_sight(MSG_ID_MOVE_Y_STOP_NOTIFY, &notify, (pack_func)move_y_stop_notify__pack, false);
	return (0);
}

int handle_move_stop_request_impl(player_struct *player, EXTERN_DATA *extern_data, MoveStopRequest *req)
{
	if (!player->data || !player->scene)
	{
//		move_request__free_unpacked(req, NULL);
		LOG_ERR("%s %d: player[%lu] do not have data[%p] or scene[%p]", __FUNCTION__, __LINE__, extern_data->player_id, player->data, player->scene);
		return (-10);
	}

	uint64_t now = time_helper::get_cached_time();
	if (now - player->last_change_area_time < 500)
	{
		LOG_DEBUG("%s %d: player[%lu] request move too much[%ld]", __FUNCTION__, __LINE__, player->data->player_id, now - player->last_change_area_time);
	}
	player->last_change_area_time = now;

	LOG_INFO("%s: player[%lu] cur_pos[%.1f][%.1f] req_pos[%.1f][%.1f]",
		__FUNCTION__, player->get_uuid(), player->get_pos()->pos_x, player->get_pos()->pos_z,
		req->cur_pos->pos_x, req->cur_pos->pos_z);

	if (req->check_distance == 0 && player->check_pos_distance(req->cur_pos->pos_x, req->cur_pos->pos_z) != 0)
	{
		struct position *pos = player->get_pos();
		LOG_ERR("%s %d: player[%lu] cur_pos[%.1f][%.1f] flash to [%.1f][%.1f]", __FUNCTION__, __LINE__, player->get_uuid(),
			pos->pos_x, pos->pos_z, req->cur_pos->pos_x, req->cur_pos->pos_z);
		send_move_answer(MSG_ID_MOVE_STOP_ANSWER, -1, pos, extern_data);
		if (player->is_unit_in_move())
		{
			player->stop_move();
		}
		return (0);
	}

//	player->data->pos_y = req->cur_pos_y;
	player->set_pos_with_broadcast(req->cur_pos->pos_x, req->cur_pos->pos_z);
/*
	player->data->move_path.speed_x = 0;
	player->data->move_path.speed_z = 0;
	scene_struct *scene = player->scene;
		// TODO: 1: 检查当前位置
		// TODO: 2: 检查路径的阻挡

	area_struct *old_area = player->area;
	struct position *pos = player->get_player_pos();
	pos->pos_x = req->cur_pos->pos_x;
	pos->pos_z = req->cur_pos->pos_z;
	player->data->move_path.speed_x = 0;
	player->data->move_path.speed_z = 0;
	area_struct *new_area = scene->get_area_by_pos(pos->pos_x, pos->pos_z);
	if (old_area != new_area)
	{
		player->update_player_sight(old_area, new_area);
	}
*/
		//设置玩家的移动路径
/*
	player->data->move_path.start_time = time_helper::get_cached_time();
	player->data->move_path.cur_pos = 0;
	player->data->move_path.max_pos = req->n_data - 1;
	if (player->data->move_path.max_pos >= MAX_PATH_POSITION)
		player->data->move_path.max_pos = MAX_PATH_POSITION - 1;
	for (size_t i = 0; i <= player->data->move_path.max_pos; ++i)
	{
		player->data->move_path.pos[i].pos_x = req->data[i]->pos_x;
		player->data->move_path.pos[i].pos_z = req->data[i]->pos_z;
	}
*/
//	CommAnswer resp;


//	comm_answer__init(&resp);
//	uint32_t ret;

//	FAST_SEND_TO_CLIENT(MSG_ID_MOVE_ANSWER, comm_answer__pack);
//	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MOVE_STOP_ANSWER, comm_answer__pack, resp);
	send_move_answer(MSG_ID_MOVE_STOP_ANSWER, 0, NULL, extern_data);	

	MoveStopNotify notify;
	move_stop_notify__init(&notify);
	notify.playerid = extern_data->player_id;
	if (player->data->truck.on_truck)
	{
		cash_truck_struct *truck = cash_truck_manager::get_cash_truck_by_id(player->data->truck.truck_id);
		if (truck != NULL)
		{
			truck->stop_move();
			notify.playerid = truck->get_uuid();
		}
	}
	notify.cur_pos = req->cur_pos;
	notify.cur_pos_y = player->data->pos_y;

	player->broadcast_to_sight(MSG_ID_MOVE_STOP_NOTIFY, &notify, (pack_func)move_stop_notify__pack, false);
	player->send_player_move_stop_to_aisrv(&notify);	
	return (0);
}

int handle_move_request_impl(player_struct *player, EXTERN_DATA *extern_data, MoveRequest *req)
{
		// TODO:	PROTO_ENTER_GAME_RESP *proto = (PROTO_ENTER_GAME_RESP *)conn_node_dbsrv::connecter.buf_head();
		// TODO:    player_struct * player = player_manager::create_player(proto, extern_data->player_id);

	if (!player->data || !player->scene)
	{
//		move_request__free_unpacked(req, NULL);
		LOG_ERR("%s %d: player[%lu] do not have data[%p] or scene[%p]", __FUNCTION__, __LINE__, extern_data->player_id, player->data, player->scene);
		return (-10);
	}

	if (req->n_data == 0)
		return (0);
		// TODO: 1: 检查当前位置
	if (player->check_pos_distance(req->data[0]->pos_x, req->data[0]->pos_z) != 0)
	{
		struct position *pos = player->get_pos();
		LOG_ERR("%s %d: player[%lu] cur_pos[%.1f][%.1f] flash to [%.1f][%.1f]", __FUNCTION__, __LINE__, player->get_uuid(),
			pos->pos_x, pos->pos_z, req->data[0]->pos_x, req->data[0]->pos_z);
		send_move_answer(MSG_ID_MOVE_ANSWER, -1, pos, extern_data);
		if (player->is_unit_in_move())
		{
			player->stop_move();
		}
		return (0);
	}
		// TODO: 2: 检查路径的阻挡
/*
	scene_struct *scene = player->scene;
	area_struct *old_area = player->area;
	struct position *pos = player->get_player_pos();
	pos->pos_x = req->data[0]->pos_x;
	pos->pos_z = req->data[0]->pos_z;
	area_struct *new_area = scene->get_area_by_pos(pos->pos_x, pos->pos_z);
	if (old_area != new_area)
	{
		player->update_player_sight(old_area, new_area);
	}
*/
	player->set_pos_with_broadcast(req->data[0]->pos_x, req->data[0]->pos_z);
//	set_player_pos_impl(player, req->data[0]->pos_x, req->data[0]->pos_z);
		//设置玩家的移动路径
	player->data->move_path.start_time = time_helper::get_cached_time();
	player->data->move_path.cur_pos = 0;
	player->data->move_path.max_pos = req->n_data - 1;
	if (req->n_data - 1 >= MAX_PATH_POSITION)
	{
		LOG_ERR("%s %d: player[%lu] move pos too much[%lu]", __FUNCTION__, __LINE__, player->get_uuid(), req->n_data);
		player->data->move_path.max_pos = MAX_PATH_POSITION - 1;
	}
	for (size_t i = 0; i <= player->data->move_path.max_pos; ++i)
	{
		player->data->move_path.pos[i].pos_x = req->data[i]->pos_x;
		player->data->move_path.pos[i].pos_z = req->data[i]->pos_z;
	}

	if (player->data->move_path.max_pos >= 1)
	{
		player->data->m_angle = pos_to_angle(player->data->move_path.pos[1].pos_x - player->data->move_path.pos[0].pos_x,
			player->data->move_path.pos[1].pos_z - player->data->move_path.pos[0].pos_z);

//		float t = player->data->m_angle / M_PI * 180 * -1 + 90;
//		LOG_DEBUG("%s: m_angle = %.3f, %.3f, %.3f %.3f", __FUNCTION__, player->data->m_angle, player->data->m_angle / M_PI, player->data->m_angle / M_PI * 180, t);
	}
	
//	CommAnswer resp;
//	comm_answer__init(&resp);
//	uint32_t ret;

//	FAST_SEND_TO_CLIENT(MSG_ID_MOVE_ANSWER, comm_answer__pack);
//	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MOVE_ANSWER, comm_answer__pack, resp);
	send_move_answer(MSG_ID_MOVE_ANSWER, 0, NULL, extern_data);

	MoveNotify notify;
	move_notify__init(&notify);
	if (player->data->truck.on_truck)
	{
		notify.playerid = player->data->truck.truck_id;
		cash_truck_struct *pTruck = cash_truck_manager::get_cash_truck_by_id(notify.playerid);
		if (pTruck != NULL)
		{
			pTruck->set_pos_with_broadcast(req->data[0]->pos_x, req->data[0]->pos_z);
		}
	} 
	else
	{
		notify.playerid = extern_data->player_id;
	}
	notify.n_data = req->n_data;
	notify.data = req->data;

	player->broadcast_to_sight(MSG_ID_MOVE_NOTIFY, &notify, (pack_func)move_notify__pack, false);
	player->send_player_move_to_aisrv(&notify);

	player->interrupt();
	return (0);
}

static int handle_move_y_start_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->buff_state & BUFF_STATE_STUN || player->buff_state & BUFF_STATE_CHANRAO)
	{
		LOG_ERR("%s: %lu is in lock state", __FUNCTION__, extern_data->player_id);
		return (-10);
	}
	if (player->buff_state & BUFF_STATE_TAUNT)
	{
		LOG_ERR("%s: %lu is in taunt state", __FUNCTION__, extern_data->player_id);
		return (-20);
	}

	MoveYStartRequest *req = move_y_start_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-30);
	}
	int ret = handle_move_y_start_request_impl(player, extern_data, req);
	move_y_start_request__free_unpacked(req, NULL);

	return (ret);
}
static int handle_move_y_stop_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player) {
		LOG_ERR("%s %d: do not have player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
//		player = player_manager::create_tmp_player(extern_data->player_id);
		return (-1);
	}

	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, extern_data->player_id);
		return -10;
	}

	if (player->buff_state & BUFF_STATE_STUN || player->buff_state & BUFF_STATE_CHANRAO)
	{
		LOG_ERR("%s: %lu is in lock state", __FUNCTION__, extern_data->player_id);
		return (-20);
	}
	if (player->buff_state & BUFF_STATE_TAUNT)
	{
		LOG_ERR("%s: %lu is in taunt state", __FUNCTION__, extern_data->player_id);
		return (-21);
	}

	MoveYStopRequest *req = move_y_stop_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-30);
	}
	int ret = handle_move_y_stop_request_impl(player, extern_data, req);
	move_y_stop_request__free_unpacked(req, NULL);
	return (ret);
}

static int handle_move_start_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->buff_state & BUFF_STATE_STUN || player->buff_state & BUFF_STATE_CHANRAO)
	{
		LOG_ERR("%s: %lu is in lock state", __FUNCTION__, extern_data->player_id);
		return (-10);
	}
	if (player->buff_state & BUFF_STATE_TAUNT)
	{
		LOG_ERR("%s: %lu is in taunt state", __FUNCTION__, extern_data->player_id);
		return (-20);
	}

	MoveStartRequest *req = move_start_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-30);
	}
	int ret = handle_move_start_request_impl(player, extern_data, req);
	move_start_request__free_unpacked(req, NULL);
	return (ret);
}
static int handle_move_stop_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player) {
		LOG_ERR("%s %d: do not have player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
//		player = player_manager::create_tmp_player(extern_data->player_id);
		return (-1);
	}

	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, extern_data->player_id);
		return -10;
	}

	if (player->buff_state & BUFF_STATE_STUN || player->buff_state & BUFF_STATE_CHANRAO)
	{
		LOG_ERR("%s: %lu is in lock state", __FUNCTION__, extern_data->player_id);
		return (-20);
	}
	if (player->buff_state & BUFF_STATE_TAUNT)
	{
		LOG_ERR("%s: %lu is in taunt state", __FUNCTION__, extern_data->player_id);
		return (-21);
	}

	MoveStopRequest *req = move_stop_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-30);
	}
	int ret = handle_move_stop_request_impl(player, extern_data, req);
	move_stop_request__free_unpacked(req, NULL);
	return (ret);
}

static int handle_move_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->buff_state & BUFF_STATE_STUN || player->buff_state & BUFF_STATE_CHANRAO)
	{
		LOG_ERR("%s: %lu is in lock state", __FUNCTION__, extern_data->player_id);
		return (-10);
	}
	if (player->buff_state & BUFF_STATE_TAUNT)
	{
		LOG_ERR("%s: %lu is in taunt state", __FUNCTION__, extern_data->player_id);
		return (-20);
	}

	MoveRequest *req = move_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-30);
	}
// TODO:
	if (req->n_data > 150)
	{
		LOG_DEBUG("%s: move data too much, req->n_data = %lu", __FUNCTION__, req->n_data);
	}
	
	int ret = handle_move_request_impl(player, extern_data, req);
	move_request__free_unpacked(req, NULL);
	return (ret);
}
/*
static int handle_skill_hit_notify(player_struct *player, EXTERN_DATA *extern_data)
{
	SkillHitNotify *notify = skill_hit_notify__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!notify) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	unit_broadcast_to_sight(player, MSG_ID_SKILL_HIT_NOTIFY, notify, (pack_func)skill_hit_notify__pack, false);
	skill_hit_notify__free_unpacked(notify, NULL);

	return (0);
}
*/

extern std::map<uint64_t, uint64_t> g_special_mon_map;
static int handle_open_big_map_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	OpenBigMapAns send;
	open_big_map_ans__init(&send);
	SpecialMonPos mon[20];
	SpecialMonPos *monPoint[20];
	int num = 0;
	for (std::map<uint64_t, uint64_t>::iterator it = g_special_mon_map.begin(); it != g_special_mon_map.end() && num < 20; ++it)
	{
		if (it->second != player->data->scene_id)
		{
			continue;
		}
		monster_struct *pMon = monster_manager::get_monster_by_id(it->first);
		if (pMon == NULL)
		{
			continue;
		}
		monPoint[num] = &mon[num];
		special_mon_pos__init(monPoint[num]);
		mon[num].id = pMon->data->monster_id;
		mon[num].x = pMon->get_pos()->pos_x;
		mon[num].z = pMon->get_pos()->pos_z;
		++num;
	}
	send.n_monster = num;
	send.monster = monPoint;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_OPEN_BIG_MAP_ANSWER, open_big_map_ans__pack, send);

	return 0;
}

static int handle_learn_skill_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	LearnSkillReq *req = learn_skill_req__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	
	
	if (req->autoup)
	{
		player->m_skill.AutoUpgrade();
	}
	else
	{
		skill_struct *skill = player->m_skill.GetSkill(req->id);
	if (skill != NULL)
	{
		//if (req->autoup)
		//{
		//	req->num = player->m_skill.GetLevelUpTo(req->id, skill->data->lv, player->get_attr(PLAYER_ATTR_LEVEL));
		//}
		if (req->num == 0)
		{
			req->num = 1;
		}
		LearnSkillAns notify;
		learn_skill_ans__init(&notify);
		notify.id = &req->id;
		notify.ret = player->m_skill.Learn(req->id, req->num);
		notify.lv = &skill->data->lv;
		notify.n_id = notify.n_lv = 1;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LEARN_SKILL_ANSWER, learn_skill_ans__pack, notify);
	}
	}
	

	learn_skill_req__free_unpacked(req, NULL);

	return (0);
}

static int handle_set_fuwen_old_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	LearnFuwenReq *req = learn_fuwen_req__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	LearnFuwenAns notify;
	learn_fuwen_ans__init(&notify);
	notify.skill_id = req->skill_id;
	notify.fuwen_id = req->fuwen_id;
	notify.ret = 0;
	skill_struct *skill = player->m_skill.GetSkill(req->skill_id);
	if (skill != NULL)
	{
		for (int i = 0; i < MAX_FUWEN; ++i)
		{
			if (skill->data->fuwen[i].id == req->fuwen_id)
			{
				skill->data->fuwen[i].isNew = false;
			}
		}
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_FUWEN_OLD_ANSWER, learn_fuwen_ans__pack, notify);

	learn_fuwen_req__free_unpacked(req, NULL);

	return (0);
}
static int handle_learn_fuwen_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	LearnFuwenReq *req = learn_fuwen_req__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t skill_id = req->skill_id;
	uint32_t fuwen_id = req->fuwen_id;
	bool autoup = req->autoup;
	uint32_t num = req->num;
	learn_fuwen_req__free_unpacked(req, NULL);

	LearnFuwenAns notify;
	learn_fuwen_ans__init(&notify);
	notify.skill_id = skill_id;
	notify.fuwen_id = fuwen_id;
	notify.ret = 0;
	skill_struct *skill = player->m_skill.GetSkill(skill_id);
	if (skill == NULL)
	{
		return -2;
	}
	fuwen_data *pFuwen = skill->get_fuwen(fuwen_id);
	if (pFuwen == NULL)
	{
		return -3;
	}
	std::map<uint64_t, struct SkillTable *>::iterator it = skill_config.find(fuwen_id);
	if (it == skill_config.end())
	{
		return -4;
	}

	if (autoup)
	{
		num = player->m_skill.GetLevelUpTo(fuwen_id, pFuwen->lv, skill->data->lv);
	}
	if (num == 0)
	{
		num = 1;
	}
	uint32_t toNum = 0;
	for (; toNum < num; ++toNum)
	{
		uint32_t cost = player->m_skill.CalcCost(it->second->SkillLv, pFuwen->lv, 1);
		if (player->get_coin() < cost)
		{
			notify.ret = 190500063;
			break;
		}
		else
		{
			std::map<uint64_t, struct SkillLvTable *>::iterator iter = skill_lv_config.find(it->second->SkillLv + pFuwen->lv);
			if (iter != skill_lv_config.end())
			{
				if (skill->data->lv < iter->second->NeedLv)
				{
					notify.ret = 190500068;
					break;
				}
				if (player->del_item(iter->second->CostItem, iter->second->CostNum, MAGIC_TYPE_SKILL) < 0)
				{
					notify.ret = 190500115;
					break;
				}
				player->sub_coin(cost, MAGIC_TYPE_SKILL);

				pFuwen->lv += 1;
				notify.lv = pFuwen->lv;
				player->add_achievement_progress(ACType_SKILL_FUWEN_LEVEL_UP, 0, 0, 0, 1);
			}

		}
	}
	if (toNum > 0 && notify.ret != 0)
	{
		notify.ret = 0;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LEARN_FUWEN_ANSWER, learn_fuwen_ans__pack, notify);
	player->add_achievement_progress(ACType_SKILL_FUWEN_LEVEL_NUM, 0, 0, 0, 1);
	return (0);
}
static int handle_set_skill_menu_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	SkillMenu *req = skill_menu__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	LOG_INFO("%s: player[%lu] skill mindex from %u to %u", __FUNCTION__, player->get_uuid(), player->m_skill.m_index, req->num);

	SkillMenuAns notify;
	skill_menu_ans__init(&notify);
	notify.num = req->num;
	notify.ret = 0;
	if (req->num > 0 && req->num <= MAX_CUR_FUWEN)
	{
		player->m_skill.m_index = req->num;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_SKILL_MENU_ANSWER, skill_menu_ans__pack, notify);

	skill_menu__free_unpacked(req, NULL);

	return 0;
}

extern FuwenData fuwenData[MAX_MY_SKILL_NUM][MAX_FUWEN];
extern FuwenData *fuwenDataPoint[MAX_MY_SKILL_NUM][MAX_FUWEN];
extern FuwenData fuwenData1[MAX_MY_SKILL_NUM][MAX_FUWEN];
extern FuwenData *fuwenDataPoint1[MAX_MY_SKILL_NUM][MAX_FUWEN];
extern SkillData skillData[MAX_MY_SKILL_NUM];
extern SkillData *skillDataPoint[MAX_MY_SKILL_NUM];
extern SkillData skillData1[MAX_MY_SKILL_NUM];
extern SkillData *skillDataPoint1[MAX_MY_SKILL_NUM];
static int handle_open_skill_for_guide_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->scene == NULL || player->data->scene_id != 20035)
	{
		return -2;
	}

	LearnSkillReq *req = learn_skill_req__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	SkillList send;
	skill_list__init(&send);
	int i = 0;

	skill_struct * pSkillStruct = player->m_skill.InsertSkill(req->id);
	if (pSkillStruct != NULL)
	{
		skill_data__init(skillData + i);
		skillData[i].id = req->id;
		skillData[i].lv = pSkillStruct->data->lv;
		skillData[i].cur_fuwen = pSkillStruct->data->cur_fuwen[player->m_skill.m_index - 1];
		skillData[i].n_fuwen = pSkillStruct->data->fuwen_num;
		for (int j = 0; j < pSkillStruct->data->fuwen_num; ++j)
		{
			fuwen_data__init(&fuwenData[i][j]);
			fuwenData[i][j].id = pSkillStruct->data->fuwen[j].id;
			fuwenData[i][j].lv = pSkillStruct->data->fuwen[j].lv;
			fuwenData[i][j].is_new = pSkillStruct->data->fuwen[j].isNew;
			fuwenDataPoint[i][j] = &fuwenData[i][j];
		}
		skillData[i].fuwen = fuwenDataPoint[i];

		skillDataPoint[i] = skillData + i;
		++i;
	}
	send.n_data = i;
	send.data = skillDataPoint;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_NEW_SKILL_NOTIFY, skill_list__pack, send);


	learn_skill_req__free_unpacked(req, NULL);

	return (0);
}

static int handle_set_fuwen_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	Fuwen *req = fuwen__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	FuwenAns send;
	fuwen_ans__init(&send);
	send.ret = player->m_skill.SetFuwen(req->skill, req->fuwen);
	send.fuwen = req->fuwen;
	send.skill = req->skill;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_FUWEN_ANSWER, fuwen_ans__pack, send);

	fuwen__free_unpacked(req, NULL);

	if (send.ret == 0)
	{
		player->add_achievement_progress(ACType_SKILL_FUWEN_WEAR, 0, 0, 0, player->m_skill.GetFuwenWearNum());
	}

	return 0;
}
static int get_add_medicine_exp(player_struct *player, uint32_t type, uint32_t num, uint32_t &add)
{
	if (type > MAX_LIVE_SKILL_NUM)
	{
		return 1;
	}
	LifeSkillTable *table = get_medicine_table(type, player->data->live_skill.level[type]);
	if (table == NULL)
	{
		return 2;
	}
	if (num == 0)
	{
		if (player->data->live_skill.book[type] < 1)
		{
			return 190410003;
		}
		if (player->del_item(table->NeedItem, 1, MAGIC_TYPE_LIVE_SKILL) < 0)
		{
			return 190410004;
		}
		add = 10;
		--player->data->live_skill.book[type];
	}
	else
	{
		if (player->sub_coin(table->NeedCoin * num, MAGIC_TYPE_LIVE_SKILL, false) < 0)
		{
			return 190410005;
		}
		//todo  扣帮贡
		add = table->ExpAdd * num;

		PROTO_UNDO_COST *proto_head;
		proto_head = (PROTO_UNDO_COST *)get_send_data();
		uint32_t data_len = sizeof(PROTO_UNDO_COST);
		proto_head->cost.coin = num;
		proto_head->cost.gold = table->NeedDonation * num;
		proto_head->cost.statis_id = type;

		EXTERN_DATA extern_data;
		extern_data.player_id = player->get_uuid();
		fast_send_msg_base(&conn_node_gamesrv::connecter, &extern_data, SERVER_PROTO_GUILD_PRODUCE_MEDICINE, data_len, 0);
	}

	return 0;
}
static int handle_learn_live_skill_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	LearnLiveReq *req = learn_live_req__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t type = req->type; //LIVE_SKILL_TYPE
	uint32_t num = req->num; //升级次数 0物品升级
	learn_live_req__free_unpacked(req, NULL);

	uint32_t add = 0;
	AnsLearnLiveSkill send;
	ans_learn_live_skill__init(&send);
	send.ret = get_add_medicine_exp(player, type, num, add);
	if (send.ret == 0)
	{
		if (num == 0)
		{
			send.type = type;
			player->data->live_skill.exp[type] += add;
			LifeSkillTable *table = get_medicine_table(type, player->data->live_skill.level[type]);
			while (player->data->live_skill.exp[type] >= table->Exp && table != NULL)
			{
				if (player->data->live_skill.level[type] >= player->data->live_skill.broken[type])
				{
					break;
				}
				++player->data->live_skill.level[type];
				player->data->live_skill.exp[type] -= table->Exp;
				table = get_medicine_table(type, player->data->live_skill.level[type]);
				player->add_achievement_progress(ACType_LIVE_SKILL_LEVEL, type, player->data->live_skill.level[type], 0, 1);
			}
			send.book = player->data->live_skill.book[type];
			send.lv = player->data->live_skill.level[type];
			send.exp = player->data->live_skill.exp[type];

			fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LEARN_LIVE_SKILL_ANSWER, ans_learn_live_skill__pack, send);
		}
		
	}
	else
	{
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LEARN_LIVE_SKILL_ANSWER, ans_learn_live_skill__pack, send);
	}
	
	return 0;
}
static int check_can_live_skill_break(player_struct *player, uint32_t type)
{
	if (player->data->live_skill.level[type] != player->data->live_skill.broken[type])
	{
		return 3;
	}

	LifeSkillTable *table1 = get_medicine_table(type, player->data->live_skill.level[type] + 1);
	if (table1 == NULL)
	{
		return 4;
	}
	LifeSkillTable *table = get_medicine_table(type, player->data->live_skill.level[type]);
	if (table == NULL)
	{
		return 2;
	}
	if (player->data->live_skill.exp[type] < table->Exp)
	{
		return 6;
	}
	if (player->get_attr(PLAYER_ATTR_LEVEL) < table->BreachLv)
	{
		return 190410005;
	}

	if (player->del_item(table->BreachItem, table->BreachNum, MAGIC_TYPE_LIVE_SKILL) < 0)
	{
		return 190410007;
	}
	return 0;
}
static int handle_live_skill_break_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	LearnLiveReq *req = learn_live_req__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t type = req->type; //LIVE_SKILL_TYPE
	learn_live_req__free_unpacked(req, NULL);

	AnsLearnLiveSkill send;
	ans_learn_live_skill__init(&send);

	send.ret = check_can_live_skill_break(player, type);
	if (send.ret == 0)
	{
		LifeSkillTable *table = get_medicine_table(type, player->data->live_skill.level[type] + 1);
		if (table != NULL)
		{
			player->data->live_skill.broken[type] = table->LvMax;
		}
		send.type = type;

		table = get_medicine_table(type, player->data->live_skill.level[type]);
		while (player->data->live_skill.exp[type] >= table->Exp && table != NULL)
		{
			if (player->data->live_skill.level[type] >= player->data->live_skill.broken[type])
			{
				break;
			}
			++player->data->live_skill.level[type];
			player->data->live_skill.exp[type] -= table->Exp;
			table = get_medicine_table(type, player->data->live_skill.level[type]);
		}

		send.book = player->data->live_skill.book[type];
		send.lv = player->data->live_skill.level[type];
		send.exp = player->data->live_skill.exp[type];
		send.broken = player->data->live_skill.broken[type];
		//send.lv = player->data->live_skill.broken[type];
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LIVE_SKILL_BREAK_ANSWER, ans_learn_live_skill__pack, send);

	return 0;
}
static int handle_guild_prodece_medicine(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	PROTO_UNDO_COST *req = (PROTO_UNDO_COST*)get_data();

	uint32_t type = req->cost.statis_id; //LIVE_SKILL_TYPE

	LifeSkillTable *table = get_medicine_table(type, player->data->live_skill.level[type]);
	if (table == NULL)
	{
		return -2;
	}

	uint32_t add = req->cost.gold * table->ExpAdd;
	AnsLearnLiveSkill send;
	ans_learn_live_skill__init(&send);
	send.ret = 0;
	if (req->cost.coin != 0)
	{
		player->add_coin(req->cost.coin * table->NeedCoin, MAGIC_TYPE_LIVE_SKILL, false);
		send.ret = 190410005;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LEARN_LIVE_SKILL_ANSWER, ans_learn_live_skill__pack, send);
		return -3;
	}

	send.type = type;
	player->data->live_skill.exp[type] += add;
	while (player->data->live_skill.exp[type] >= table->Exp && table != NULL)
	{
		if (player->data->live_skill.level[type] >= player->data->live_skill.broken[type])
		{
			break;
		}
		++player->data->live_skill.level[type];
		player->data->live_skill.exp[type] -= table->Exp;
		table = get_medicine_table(type, player->data->live_skill.level[type]);
		player->add_achievement_progress(ACType_LIVE_SKILL_LEVEL, type, player->data->live_skill.level[type], 0, 1);
	}
	send.book = player->data->live_skill.book[type];
	send.lv = player->data->live_skill.level[type];
	send.exp = player->data->live_skill.exp[type];

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LEARN_LIVE_SKILL_ANSWER, ans_learn_live_skill__pack, send);

	return 0;
}


static int handle_add_speed_buff_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	buff_manager::create_default_buff(114400019, player, player, true);		
	return (0);
}
static int handle_del_speed_buff_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	player->delete_one_buff(114400019, true);	
	return (0);
}

static int handle_produce_medicine_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	LearnLiveReq *req = learn_live_req__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t type = req->type; //LIVE_SKILL_TYPE
	learn_live_req__free_unpacked(req, NULL);

	LifeSkillTable *table = get_medicine_table(type, player->data->live_skill.level[type]);
	if (table == NULL)
	{
		return -2;
	}
	int ret = 0;
	
	uint64_t r = rand() % table->ProMax;
	uint64_t all = 0;
	uint32_t i = 0;
	AttrMap attrs;
	if (player->get_attr(PLAYER_ATTR_ENERGY) < table->NeedJingli)
	{
		ret = 190410001;
		goto done;
	}
	for (; i < table->n_LvPro; ++i)
	{
		all += table->LvPro[i];
		if (r <= all)
		{
			if (player->add_item(table->ItemID[i], 1, MAGIC_TYPE_LIVE_SKILL) != 0)
			{
				ret = 190410002;
				goto done;
			}
			break;
		}
	}
	player->set_attr(PLAYER_ATTR_ENERGY, player->get_attr(PLAYER_ATTR_ENERGY) - table->NeedJingli);

	attrs[PLAYER_ATTR_ENERGY] = player->get_attr(PLAYER_ATTR_ENERGY);
	player->notify_attr(attrs);
	player->add_achievement_progress(ACType_LIVE_SKILL_ENERGY, type, 0, 0, table->NeedJingli);
	player->add_task_progress(TCT_LIVE_SKILL_PRODUCE, type, 1);

done:
	send_comm_answer(MSG_ID_PRODUCE_MEDICINE_ANSWER, ret, extern_data);

	return 0;
}

// static int handle_skill_move_request(player_struct *player, EXTERN_DATA *extern_data)
// {
//	if (comm_check_player_valid(player, extern_data->player_id) != 0)
//	{
//		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
//		return (-1);
//	}

//	SkillMoveRequest *req = skill_move_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
//	if (!req) {
//		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
//		return (-10);
//	}
//	float pos_x, pos_z;
//	pos_x = req->cur_pos->pos_x;
//	pos_z = req->cur_pos->pos_z;
//	skill_move_request__free_unpacked(req, NULL);

//		// TODO: 检查位移是否合法
//	player->set_pos_with_broadcast(pos_x, pos_z);

//	SkillMoveNotify notify;
//	skill_move_notify__init(&notify);
//	notify.playerid = player->get_uuid();
//	PosData cur_pos;
//	pos_data__init(&cur_pos);
//	cur_pos.pos_x = player->get_pos()->pos_x;
//	cur_pos.pos_z = player->get_pos()->pos_z;
//	notify.cur_pos = &cur_pos;
//	player->broadcast_to_sight(MSG_ID_SKILL_MOVE_NOTIFY, &notify, (pack_func)skill_move_notify__pack, true);
//	return (0);
// }

//1表示P1胜利，2表示P2胜利，3表示平局，0表示无结果
// int check_qiecuo_finished(player_struct *p1, player_struct *p2)
// {
// 	if (!p1->is_in_qiecuo() || !p1->is_qiecuo_target(p2))
// 		return (0);

// 	int ret = 0;
// 	if (!p1->is_alive())
// 	{
// 		p1->set_attr(PLAYER_ATTR_HP, 1);
// 		ret += 2;
// 	}

// 	if (!p2->is_alive())
// 	{
// 		p2->set_attr(PLAYER_ATTR_HP, 1);
// 		ret += 1;
// 	}

// 	if (ret != 0)
// 	{
// 		LOG_INFO("%s: player[%lu][%lu] qiecuo result %d", __FUNCTION__, p1->get_uuid(), p2->get_uuid(), ret);

// 		p1->clear_debuff();
// 		p2->clear_debuff();
// 		p1->finish_qiecuo();
// 		p2->finish_qiecuo();

// 		QiecuoFinishNotify nty;
// 		EXTERN_DATA ext;
// 		qiecuo_finish_notify__init(&nty);

// 		switch (ret)
// 		{
// 			case 1:
// 				nty.result = 0;
// 				ext.player_id = p1->get_uuid();
// 				fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_FINISH_NOTIFY, qiecuo_finish_notify__pack, nty);

// 				nty.result = 1;
// 				ext.player_id = p2->get_uuid();
// 				fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_FINISH_NOTIFY, qiecuo_finish_notify__pack, nty);
// 				break;
// 			case 2:
// 				nty.result = 1;
// 				ext.player_id = p1->get_uuid();
// 				fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_FINISH_NOTIFY, qiecuo_finish_notify__pack, nty);

// 				nty.result = 0;
// 				ext.player_id = p2->get_uuid();
// 				fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_FINISH_NOTIFY, qiecuo_finish_notify__pack, nty);
// 				break;
// 			case 3:
// 				nty.result = 2;
// 				ext.player_id = p1->get_uuid();
// 				fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_FINISH_NOTIFY, qiecuo_finish_notify__pack, nty);
// 				ext.player_id = p2->get_uuid();
// 				fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_FINISH_NOTIFY, qiecuo_finish_notify__pack, nty);
// 				break;
// 		}
// 	}


// 	return (ret);
// }

// static int handle_skill_call_attack_request(player_struct *player, EXTERN_DATA *extern_data)
// {
// 	if (comm_check_player_valid(player, extern_data->player_id) != 0)
// 	{
// 		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
// 		return (-1);
// 	}
// 		// TODO: 检查是客户端召唤类技能


// 	SkillCallAttackRequest *req = skill_call_attack_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
// 	if (!req) {
// 		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
// 		return (-10);
// 	}

// 	if (req->n_target_playerid != req->n_target_pos || req->n_target_playerid != req->n_hit_index) {
// 		LOG_ERR("%s %d: player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
// 		skill_call_attack_request__free_unpacked(req, NULL);
// 		return (-20);
// 	}

// 	int n_hit_effect = 0;
// 	int n_buff = 0;
// 	struct SkillLvTable *lv_config1, *lv_config2;
// 	struct PassiveSkillTable *pas_config;
// 	struct SkillTable *ski_config;
// 	skill_struct *skill_struct = player->m_skill.GetSkillStructFromFuwen(player->get_skill_id());
// //	uint32_t skill_lv = player->m_skill.GetSkillLevel(player->get_skill_id());
// //	if (skill_lv < 1)
// 	if (!skill_struct)
// 	{
// 		LOG_ERR("%s %d: player[%lu] skill[%u] no config", __FUNCTION__, __LINE__, extern_data->player_id, player->get_skill_id());
// 		skill_call_attack_request__free_unpacked(req, NULL);
// 		return (-30);
// 	}
// 		//检查CD
// 	if (time_helper::get_cached_time() < skill_struct->data->cd_time)
// 	{
// 		LOG_ERR("%s: %lu cast skill %u already in cd", __FUNCTION__, extern_data->player_id, req->skillid);
// 		skill_call_attack_request__free_unpacked(req, NULL);
// 		return (-40);
// 	}

// 	get_skill_configs(skill_struct->data->lv, player->get_skill_id(), &ski_config, &lv_config1, &pas_config, &lv_config2);
// //	if (!lv_config1 && !lv_config2)
// 	if (!lv_config1)
// 	{
// 		LOG_ERR("%s %d: player[%lu] skill[%u] no config", __FUNCTION__, __LINE__, extern_data->player_id, player->get_skill_id());
// 		skill_call_attack_request__free_unpacked(req, NULL);
// 		return (-50);
// 	}

// 	struct ActiveSkillTable *active_config = get_config_by_id(ski_config->SkillAffectId, &active_skill_config);
// 	if (!active_config)
// 	{
// 		LOG_ERR("%s: %lu skill %u not active_config", __FUNCTION__, extern_data->player_id, req->skillid);
// 		skill_call_attack_request__free_unpacked(req, NULL);
// 		return (-60);
// 	}

// 	player->set_pos_with_broadcast(req->attack_pos->pos_x, req->attack_pos->pos_z);

// 	std::vector<unit_struct *> sight_all;
// 	std::vector<unit_struct *> dead_all;
// 	sight_all.push_back(player);

// 	skill_struct->add_cd(lv_config1, active_config);
// 	// if (lv_config1->CD != 0)
// 	// 	skill_struct->data->cd_time = time_helper::get_cached_time() + lv_config1->CD - 200;
// 	// else
// 	// 	skill_struct->data->cd_time = time_helper::get_cached_time() + active_config->TotalSkillDelay - 200;

// 	uint32_t life_steal = 0;
// 	uint32_t damage_return = 0;

// 	for (size_t i = 0; i < req->n_target_playerid; ++i)
// 	{
// 		unit_struct *target = unit_struct::get_unit_by_uuid(req->target_playerid[i]);
// 		if (!target)
// 			continue;
// //		if (target->buff_state & BUFF_STATE_GOD)
// //			continue;
// //		if (!check_can_attack(player, target))
// 		UNIT_FIGHT_TYPE fight_type = get_unit_fight_type(player, target);
// 		if (fight_type != UNIT_FIGHT_TYPE_ENEMY)
// 		{
// 			LOG_ERR("%s %d: %lu can not attack unit %lu", __FUNCTION__, __LINE__, extern_data->player_id, target->get_uuid());
// 			continue;
// 		}

// 		if (!target->is_alive())
// 		{
// 			LOG_ERR("%s %d: %lu kill already dead unit %lu", __FUNCTION__, __LINE__, extern_data->player_id, target->get_uuid());
// 			continue;
// 		}

// 			//检查距离，场景
// 		if (player->scene != target->scene)
// 		{
// 			continue;
// 		}

// //		LOG_DEBUG("%s %d: player[%lu] attack target %lu", __FUNCTION__, __LINE__, extern_data->player_id, target->get_uuid());

// 		target->set_pos(req->target_pos[i]->pos_x, req->target_pos[i]->pos_z);
// 		sight_all.push_back(target);

// 		cached_hit_effect_point[n_hit_effect] = &cached_hit_effect[n_hit_effect];
// 		skill_hit_effect__init(&cached_hit_effect[n_hit_effect]);
// 		uint32_t add_num = 0;
// 		int32_t damage;

// 		int32_t other_rate = count_other_skill_damage_effect(player, target);
// 		damage = count_skill_total_damage(fight_type, ski_config,
// 			lv_config1, pas_config, lv_config2,
// 			player, target,
// 			&cached_hit_effect[n_hit_effect].effect,
// 			&cached_buff_id[n_buff],
// 			&add_num, other_rate);

// 		life_steal += player->count_life_steal_effect(damage);
// 		damage_return += player->count_damage_return(damage, target);

// 		raid_struct *raid = player->get_raid();
// 		if (raid)
// 		{
// 			raid->on_player_attack(player, target, damage);
// 		}

// 		player->on_attack(target);

// 		LOG_DEBUG("%s: unit[%lu][%p] damage[%d] hp[%f]", __FUNCTION__, target->get_uuid(), target, damage, target->get_attr(PLAYER_ATTR_HP));

// 		if (target->get_unit_type() == UNIT_TYPE_PLAYER)
// 		{
// 			check_qiecuo_finished(player, (player_struct *)target);
// 		}

// 		if (target->is_alive())
// 		{
// 			target->on_beattack(player, player->get_skill_id(), damage);
// 		}
// 		else
// 		{
// 			dead_all.push_back(target);
// 		}
// 		cached_target_index[n_hit_effect] = req->hit_index[i];

// 		cached_hit_effect[n_hit_effect].playerid = req->target_playerid[i];
// 		cached_hit_effect[n_hit_effect].n_add_buff = add_num;
// 		cached_hit_effect[n_hit_effect].add_buff = &cached_buff_id[n_buff];
// 		cached_hit_effect[n_hit_effect].hp_delta = damage;
// 		cached_hit_effect[n_hit_effect].cur_hp = target->get_attr(PLAYER_ATTR_HP);
// //		cached_hit_effect[n_hit_effect].attack_pos = &cached_attack_pos[n_hit_effect];
// 		cached_hit_effect[n_hit_effect].target_pos = &cached_target_pos[n_hit_effect];
// //		pos_data__init(&cached_attack_pos[n_hit_effect]);
// 		pos_data__init(&cached_target_pos[n_hit_effect]);
// //		cached_attack_pos[n_hit_effect].pos_x = player->get_pos()->pos_x;
// //		cached_attack_pos[n_hit_effect].pos_z = player->get_pos()->pos_z;
// 		cached_target_pos[n_hit_effect].pos_x = target->get_pos()->pos_x;
// 		cached_target_pos[n_hit_effect].pos_z = target->get_pos()->pos_z;
// 		n_buff += add_num;
// 		++n_hit_effect;
// 	}

// 	skill_call_attack_request__free_unpacked(req, NULL);

// 	if (!player->is_alive())
// 	{
// 		player->on_dead(player);
// 	}

// //	CommAnswer resp;
// //	comm_answer__init(&resp);
// //	uint32_t ret;

// //	FAST_SEND_TO_CLIENT(MSG_ID_CAST_SKILL_ANSWER, comm_answer__pack);
// //	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SKILL_HIT_ANSWER, comm_answer__pack, resp);

// 	for (std::vector<unit_struct *>::const_iterator iter = dead_all.begin(); iter != dead_all.end(); ++iter)
// 	{
// 		unit_struct *target = (*iter);
// 			//怪物死亡的话清空视野
// 		target->on_dead(player);
// 	}

// 	SkillCallAttackNotify notify;
// 	skill_call_attack_notify__init(&notify);
// 	notify.playerid = extern_data->player_id;
// 	notify.skillid = player->get_skill_id();
// 	notify.n_target_player = n_hit_effect;
// 	notify.n_hit_index = n_hit_effect;
// 	notify.target_player = cached_hit_effect_point;
// 	notify.hit_index = cached_target_index;

// 	notify.attack_cur_hp = player->get_attr(PLAYER_ATTR_HP);
// 	notify.life_steal = life_steal;
// 	notify.damage_return = damage_return;

// 	PosData attack_pos;
// 	pos_data__init(&attack_pos);
// 	attack_pos.pos_x = player->get_pos()->pos_x;
// 	attack_pos.pos_z = player->get_pos()->pos_z;
// 	notify.attack_pos = &attack_pos;

// 	player->broadcast_to_many_sight(MSG_ID_CALL_ATTACK_NOTIFY, &notify, (pack_func)skill_call_attack_notify__pack, sight_all);

// 	return (0);
// }
/*
static void ttt(player_struct *player, uint64_t player_id, float target_pos_x, float target_pos_z)
{
	unit_struct *target = unit_struct::get_unit_by_uuid(player_id);
	if (!target)
		return;
	if (target->buff_state & BUFF_STATE_GOD)
		return;
	if (!check_can_attack(player, target))
	{
		LOG_ERR("%s %d: %lu can not attack unit %lu", __FUNCTION__, __LINE__, player->get_uuid(), target->get_uuid());
		return;
	}

	if (!target->is_alive())
	{
		LOG_ERR("%s %d: %lu kill already dead unit %lu", __FUNCTION__, __LINE__, player->get_uuid(), target->get_uuid());
		return;
	}

		//检查距离，场景
	if (player->scene != target->scene)
	{
		return;
	}

//		LOG_DEBUG("%s %d: player[%lu] attack target %lu", __FUNCTION__, __LINE__, extern_data->player_id, target->get_uuid());

	target->set_pos(target_pos_x, target_pos_z);
	sight_all.push_back(target);

	cached_hit_effect_point[n_hit_effect] = &cached_hit_effect[n_hit_effect];
	skill_hit_effect__init(&cached_hit_effect[n_hit_effect]);
	uint32_t add_num = 0;
	int32_t damage;

	damage = count_skill_total_damage(ski_config,
		lv_config1, pas_config, lv_config2,
		player, target,
		&cached_hit_effect[n_hit_effect].effect,
		&cached_buff_id[n_buff],
		&add_num);

	raid_struct *raid = player->get_raid();
	if (raid)
	{
		raid->on_player_attack(player, target, damage);
	}

	player->on_attack(target);

	LOG_DEBUG("%s: unit[%lu][%p] damage[%d] hp[%f]", __FUNCTION__, target->get_uuid(), target, damage, target->get_attr(PLAYER_ATTR_HP));

	if (target->is_alive())
	{
		target->on_beattack(player, player->data->skill.skill_id, damage);
	}
	else
	{
		dead_all.push_back(target);
	}

	cached_hit_effect[n_hit_effect].playerid = req->target_playerid[i];
	cached_hit_effect[n_hit_effect].n_add_buff = add_num;
	cached_hit_effect[n_hit_effect].add_buff = &cached_buff_id[n_buff];
	cached_hit_effect[n_hit_effect].hp_delta = damage;
	cached_hit_effect[n_hit_effect].cur_hp = target->get_attr(PLAYER_ATTR_HP);
//		cached_hit_effect[n_hit_effect].attack_pos = &cached_attack_pos[n_hit_effect];
	cached_hit_effect[n_hit_effect].target_pos = &cached_target_pos[n_hit_effect];
//		pos_data__init(&cached_attack_pos[n_hit_effect]);
	pos_data__init(&cached_target_pos[n_hit_effect]);
//		cached_attack_pos[n_hit_effect].pos_x = player->get_pos()->pos_x;
//		cached_attack_pos[n_hit_effect].pos_z = player->get_pos()->pos_z;
	cached_target_pos[n_hit_effect].pos_x = target->get_pos()->pos_x;
	cached_target_pos[n_hit_effect].pos_z = target->get_pos()->pos_z;
	n_buff += add_num;
	++n_hit_effect;
}
*/

static int handle_skill_hit_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

//	static SkillHitEffect cached_hit_effect[256];
//	static SkillHitEffect *cached_hit_effect_point[256];
//	static uint32_t cached_buff_id[512];

	if (player->get_skill_id() == 0)
		return (-1);

	SkillHitRequest *req = skill_hit_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	if (req->n_target_playerid != req->n_target_pos) {
		LOG_ERR("%s %d: player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		skill_hit_request__free_unpacked(req, NULL);
		return (-20);
	}

	if (player->check_pos_distance(req->attack_pos->pos_x, req->attack_pos->pos_z) != 0)
	{
		struct position *pos = player->get_pos();
		LOG_ERR("%s %d: player[%lu] cur_pos[%.1f][%.1f] flash to [%.1f][%.1f]", __FUNCTION__, __LINE__, player->get_uuid(),
			pos->pos_x, pos->pos_z, req->attack_pos->pos_x, req->attack_pos->pos_z);
	}

	player->deal_skill_hit_request(req);
	skill_hit_request__free_unpacked(req, NULL);
	return (0);
}

static int handle_gather_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->scene == NULL)
	{
		return -3;
	}

	StartCollect *req = start_collect__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	Collect *pCollect = Collect::GetById(req->id);
	uint32_t reqid = req->id;
	uint32_t step = req->step;
	start_collect__free_unpacked(req, NULL);

	LOG_DEBUG("%s: player[%lu] collectid[%u]", __FUNCTION__, extern_data->player_id, reqid);

	if (pCollect == NULL)
	{
		LOG_ERR("%s %d: can not player[%lu] find collect %u", __FUNCTION__, __LINE__, extern_data->player_id, reqid);
		return -11;
	}
	//player->interrupt();
	int ret = pCollect->BegingGather( player , step);
	if (ret != 0)
	{
		send_comm_answer(MSG_ID_COLLECT_BEGIN_ANSWER, ret, extern_data);
		return 0;
	}

	NotifyCollect send;
	notify_collect__init(&send);
	send.playerid = player->get_uuid();
	send.collectid = pCollect->m_uuid;

	player->broadcast_to_sight(MSG_ID_COLLECT_BEGIN_NOTIFY, &send, (pack_func)notify_collect__pack, true);

	return 0;
}
static int handle_gather_complete(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player)
		return (-1);

	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, extern_data->player_id);
		return -10;
	}

	//CollectId *req = collect_id__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	//if (!req) {
	//	LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
	//	return (-10);
	//}
	//uint32_t reqid = req->id;

	//Collect *pCollect = Collect::GetById(req->id);
	//collect_id__free_unpacked(req, NULL);

	LOG_DEBUG("%s: player[%lu] collectid[%u]", __FUNCTION__, extern_data->player_id, player->data->m_collect_uuid);
	Collect *pCollect = Collect::GetById(player->data->m_collect_uuid);
	if (pCollect == NULL)
	{
		LOG_ERR("%s %d: can not player[%lu] find collect %u", __FUNCTION__, __LINE__, extern_data->player_id, player->data->m_collect_uuid);
		return -11;
	}
	int ret = pCollect->GatherComplete(player);
	send_comm_answer(MSG_ID_COLLECT_COMMPLETE_ANSWER, ret, extern_data);
	if (ret == 0)
	{
		player->scene->on_collect(player, pCollect);
	}
	return 0;
}
static int handle_gather_interupt(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player)
		return (-1);

	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, extern_data->player_id);
		return -10;
	}

	CollectId *req = collect_id__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	Collect *pCollect = Collect::GetById(req->id);
	collect_id__free_unpacked(req, NULL);
	if (pCollect == NULL)
	{
		return -11;
	}
	pCollect->GatherInterupt(player);

	return 0;
}

static int chat_area(player_struct *player, EXTERN_DATA *extern_data, Chat *req)
{
	player->broadcast_to_sight(MSG_ID_CHAT_NOTIFY, req, (pack_func)chat__pack, true);
	if (sg_gm_cmd_open > 0)
	{
		int   argc = 0;
		char *argv[MAX_GM_ARGV];
		chat_mod::parse_cmd(req->contain, &argc, argv);
		if (chat_mod::do_gm_cmd(player, argc, argv) == 0)
		{
			for (int i = 1; i < argc; ++i) argv[i][-1] = ' ';
			LOG_INFO("%s player %lu send gm command %s", __FUNCTION__, player->get_uuid(), req->contain);
		}
	}
	return 0;
}
static int chat_menpai(player_struct *player, EXTERN_DATA *extern_data, Chat *req)
{
	PROTO_HEAD_CONN_BROADCAST *head;
	PROTO_HEAD *real_head;

	/** ================广播数据============ **/
	head = (PROTO_HEAD_CONN_BROADCAST *)conn_node_base::global_send_buf;
	head->msg_id = ENDION_FUNC_2(SERVER_PROTO_BROADCAST);
	real_head = &head->proto_head;

	real_head->msg_id = ENDION_FUNC_2(MSG_ID_CHAT_NOTIFY);
	real_head->seq = 0;
	memcpy(real_head->data, get_data(), get_data_len());
	real_head->len = ENDION_FUNC_4(sizeof(PROTO_HEAD)+get_data_len());
	uint64_t *ppp = (uint64_t*)((char *)&head->player_id + get_data_len());
	int player_num = 0;
	//todo 先遍历所有玩家 以后换成相同职业的集合
	std::map<uint64_t, player_struct *>::iterator it = player_manager_all_players_id.begin();
	for (; it != player_manager_all_players_id.end(); ++it, ++player_num)
	{
		if (it->second->get_job() != player->get_job())
		{
			continue;
		}
		if (get_entity_type(it->second->get_uuid()) == ENTITY_TYPE_AI_PLAYER)
			continue;
		ppp[player_num] = it->second->get_uuid();
	}
	head->num_player_id = player_num;
	head->len = ENDION_FUNC_4(sizeof(PROTO_HEAD_CONN_BROADCAST)+get_data_len() + sizeof(uint64_t)* head->num_player_id);

	if (conn_node_gamesrv::connecter.send_one_msg((PROTO_HEAD *)head, 1) != (int)(ENDION_FUNC_4(head->len))) {
		LOG_ERR("%s %d: send to all failed err[%d]", __FUNCTION__, __LINE__, errno);
	}
	return 0;
}
static int chat_group(player_struct *player, EXTERN_DATA *extern_data, Chat *req)
{
	if (player->get_attr(PLAYER_ATTR_ZHENYING) == 0)
	{
		return -1;
	}
	PROTO_HEAD_CONN_BROADCAST *head;
	PROTO_HEAD *real_head;

	/** ================广播数据============ **/
	head = (PROTO_HEAD_CONN_BROADCAST *)conn_node_base::global_send_buf;
	head->msg_id = ENDION_FUNC_2(SERVER_PROTO_BROADCAST);
	real_head = &head->proto_head;

	real_head->msg_id = ENDION_FUNC_2(MSG_ID_CHAT_NOTIFY);
	real_head->seq = 0;
	memcpy(real_head->data, get_data(), get_data_len());
	real_head->len = ENDION_FUNC_4(sizeof(PROTO_HEAD)+get_data_len());
	uint64_t *ppp = (uint64_t*)((char *)&head->player_id + get_data_len());
	int player_num = 0;
	//todo 先遍历所有玩家 以后换成相同阵营的集合
	std::map<uint64_t, player_struct *>::iterator it = player_manager_all_players_id.begin();
	for (; it != player_manager_all_players_id.end(); ++it, ++player_num)
	{
		if (it->second->get_attr(PLAYER_ATTR_ZHENYING) != player->get_attr(PLAYER_ATTR_ZHENYING))
		{
			continue;
		}
		if (get_entity_type(it->second->get_uuid()) == ENTITY_TYPE_AI_PLAYER)
			continue;
		ppp[player_num] = it->second->get_uuid();
	}
	head->num_player_id = player_num;
	head->len = ENDION_FUNC_4(sizeof(PROTO_HEAD_CONN_BROADCAST)+get_data_len() + sizeof(uint64_t)* head->num_player_id);

	if (conn_node_gamesrv::connecter.send_one_msg((PROTO_HEAD *)head, 1) != (int)(ENDION_FUNC_4(head->len))) {
		LOG_ERR("%s %d: send to all failed err[%d]", __FUNCTION__, __LINE__, errno);
	}

	return 0;
}
int handle_chat_no_check(player_struct *player, EXTERN_DATA *extern_data, Chat *req)
{
	if (req->channel == CHANNEL__private)
	{
		//私聊转发到好友服处理
		size_t data_len = get_data_len();
		uint8_t *pSendData = get_send_data();
		memcpy(pSendData, get_data(), data_len);
		fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_FRIEND_CHAT, data_len, 0);
		return 0;
	}
	else if (req->channel == CHANNEL__world || req->channel == CHANNEL__zhaomu)
	{
		conn_node_gamesrv::send_to_all_player(MSG_ID_CHAT_NOTIFY, req, (pack_func)chat__pack);
	}
    else if ( req->channel == CHANNEL__area )
    {
		chat_area(player, extern_data, req);
    }
    else if ( req->channel == CHANNEL__team )
    {
		if (player->m_team == NULL)
		{
			return 0;
		}
		player->m_team->BroadcastToTeam(MSG_ID_CHAT_NOTIFY, req, (pack_func)chat__pack);
	}
	else if (req->channel == CHANNEL__menpai)
	{
		return chat_menpai(player, extern_data, req);
	}
	else if (req->channel == CHANNEL__group)
	{
		return chat_group(player, extern_data, req);
	}
	else if (req->channel == CHANNEL__family)
	{
		//帮会聊天转发到帮会服处理
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_GUILD_CHAT, chat__pack, *req);
	}

	return (0);
}
static int handle_team_speek_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player)
		return (-1);

	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, extern_data->player_id);
		return -10;
	}

	Chat *req = chat__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	ParameterTable *table = get_config_by_id(161000005, &parameter_config);
	TeamNotifyCd note;
	team_notify_cd__init(&note);
	note.cd = 0;
	if (req->channel >= MAX_CHANNEL)
	{
		goto done;
	}
	if (player->m_team == NULL)
	{
		note.cd = player->m_team->m_data->speekCd[req->channel] - (time_t)time_helper::get_cached_time() / 1000;
		goto done;
	}
	if (player->m_team->m_data->speekCd[req->channel] > (time_t)time_helper::get_cached_time() / 1000)
	{
		note.cd = player->m_team->m_data->speekCd[req->channel] - (time_t)time_helper::get_cached_time() / 1000;
		goto done;
	}
	
	if (table != NULL)
	{
		player->m_team->m_data->speekCd[req->channel] = time_helper::get_cached_time() / 1000 + table->parameter1[req->channel];
	}
	
	handle_chat_no_check(player, extern_data, req);
	player->m_team->NotityXiayi();
done:
	chat__free_unpacked(req, NULL);
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TEAM_SPEEK_CD_NOTIFY, team_notify_cd__pack, note);

	return (0);
}
static int check_can_chat(player_struct *player, EXTERN_DATA *extern_data, Chat *req)
{
	if (req->channel == CHANNEL__world)
	{
		ParameterTable *table = get_config_by_id(161000248, &parameter_config);
		if (table == NULL)
		{
			return 2;
		}
		if (player->get_attr(PLAYER_ATTR_LEVEL) <= table->parameter1[0])
		{
			return 190400002;
		}
		else if (player->data->world_chat_cd > time_helper::get_cached_time() / 1000)
		{
			return 190400003;
		}
		player->data->world_chat_cd = time_helper::get_cached_time() / 1000 + table->parameter1[1];
	}
	return 0;
}
static int handle_chat_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player)
		return (-1);

	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, extern_data->player_id);
		return -10;
	}

	Chat *req = chat__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	AnsChat send;
	ans_chat__init(&send);
	send.ret = check_can_chat(player, extern_data, req);
	if (send.ret == 190400003)
	{
		send.cd = player->data->world_chat_cd - time_helper::get_cached_time() / 1000;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHAT_ANSWER, ans_chat__pack, send);
	if (send.ret == 0)
	{
		handle_chat_no_check(player, extern_data, req);
		player->add_task_progress(TCT_CHAT, req->channel, 1);
	}
	chat__free_unpacked(req, NULL);

	return (0);
}

static int handle_partner_skill_cast_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_PARTNER_SKILL_CAST_ANSWER, -1, extern_data);				
		return (-1);
	}

	if (player->get_attr(PLAYER_ATTR_PARTNER_ANGER) + __DBL_EPSILON__ < sg_partner_anger_max)
	{
		LOG_ERR("%s: %lu anger not enough", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_PARTNER_SKILL_CAST_ANSWER, -10, extern_data);				
		return (-10);		
	}

	partner_struct *partner = player->get_battle_partner();
	if (!partner)
	{
		LOG_ERR("%s: %lu do not have partner", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_PARTNER_SKILL_CAST_ANSWER, -20, extern_data);				
		return (-20);		
	}
	PartnerSkillTable *table = get_config_by_id(partner->data->partner_id, &partner_rand_skill_config);
	if (table == NULL)
	{
		return  (-21);
	}
	uint32_t skill_id = table->Angerskill;
	uint32_t type3 = 0;
	struct SkillTable *config = get_config_by_id(skill_id, &skill_config);
	if (config != NULL)
	{
		for (uint32_t i = 0; i < config->n_TimeID; ++i)
		{
			SkillTimeTable *skillTime = get_config_by_id(config->TimeID[i], &skill_time_config);
			if (skillTime == NULL)
			{
				continue;
			}
			for (uint32_t n = 0; n < skillTime->n_BuffIdFriend; ++n)
			{
				BuffTable *buffTable = get_config_by_id(skillTime->BuffIdFriend[n], &buff_config);
				if (buffTable == NULL)
				{
					continue;
				}
				if (buffTable->BuffType == 4)
				{
					type3 = skillTime->BuffIdFriend[n];
					break;
				}
			}
		}
	}
	if (type3 != 0)
	{
		buff_manager::create_default_buff(type3, player, player, true);
		player->adjust_battle_partner();
	} 
	else
	{
		unit_struct *target = partner->ai->choose_target(partner);
		if (!target)
		{
			send_comm_answer(MSG_ID_PARTNER_SKILL_CAST_ANSWER, 190500320, extern_data);
			return (0);
		}
		partner->attack_target(skill_id, -1, target);
	}

	player->reset_partner_anger(true);
	ParameterTable *tablePa = get_config_by_id(161000502, &parameter_config);
	if (tablePa != NULL)
	{
		player->data->partner_add_angry_cd = time_helper::get_cached_time() / 1000 + tablePa->parameter1[0];
	}
	send_comm_answer(MSG_ID_PARTNER_SKILL_CAST_ANSWER, 0, extern_data);
	return (0);
}

static int handle_skill_cast_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		// CommAnswer resp;
		// comm_answer__init(&resp);
		// resp.result = -1;
		// fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SKILL_CAST_ANSWER, comm_answer__pack, resp);
		send_comm_answer(MSG_ID_SKILL_CAST_ANSWER, -1, extern_data);		
		return (-1);
	}

	// if (player->buff_state & BUFF_STATE_STUN)
	// {
	// 	LOG_ERR("%s: %lu is in stun state", __FUNCTION__, extern_data->player_id);
	// 	return (-10);
	// }
	// if (player->buff_state & BUFF_STATE_TAUNT)
	// {
	//	LOG_ERR("%s: %lu is in taunt state", __FUNCTION__, extern_data->player_id);
	//	return (-20);
	// }

	SkillCastRequest *req = skill_cast_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-30);
	}

	SkillTable *config = get_config_by_id(req->skillid, &fuwen_config);
	if (!config)
	{
		LOG_ERR("%s: %lu cast skill %u but no config", __FUNCTION__, extern_data->player_id, req->skillid);
		skill_cast_request__free_unpacked(req, NULL);
		return (-40);
	}

		//不是普通攻击就检查缠绕和封印
	// if (config->SkillType != 1)
	// {
	// 	if (player->buff_state & BUFF_STATE_CHANRAO || player->buff_state & BUFF_STATE_FENGYIN)
	// 	{
	// 		LOG_ERR("%s: %lu cast skill %u but player in charao,fengying state", __FUNCTION__, extern_data->player_id, req->skillid);
	// 		skill_cast_request__free_unpacked(req, NULL);
	// 		return (-42);
	// 	}
	// }
	
	SkillTable *fuwen_config = get_config_by_id(req->skillid, &skill_config);
	if (!fuwen_config)
	{
		LOG_ERR("%s: %lu cast skill %u but no config", __FUNCTION__, extern_data->player_id, req->skillid);
		skill_cast_request__free_unpacked(req, NULL);
		return (-45);
	}
	
   
//	int fuwen_id = req->skillid;

	struct ActiveSkillTable *active_config = NULL;

	if (!player->is_in_buff3())
	{
		skill_struct *skill_struct = player->m_skill.GetSkillStructFromFuwen(config->ID);
		if (!skill_struct)
		{
			LOG_ERR("%s: %lu cast skill %u but not learned", __FUNCTION__, extern_data->player_id, req->skillid);
			skill_cast_request__free_unpacked(req, NULL);
			return (-50);
		}

		int skill_lv;
		int skill_id;
		skill_struct->get_skill_id_and_lv(player->m_skill.m_index, &skill_id, &skill_lv);
		if ((uint32_t)skill_id != req->skillid)
		{
			LOG_ERR("%s: player[%lu] skill[%u] req[%u]", __FUNCTION__, player->get_uuid(), (uint32_t)skill_id, req->skillid);
			skill_cast_request__free_unpacked(req, NULL);
			send_comm_answer(MSG_ID_SKILL_CAST_ANSWER, -53, extern_data);
			return (-53);
		}

			//检查CD
		if (time_helper::get_cached_time() < skill_struct->data->cd_time)
		{
			LOG_ERR("%s: %lu cast skill %u already in cd[%lu]", __FUNCTION__, extern_data->player_id, req->skillid, skill_struct->data->cd_time);
			skill_cast_request__free_unpacked(req, NULL);
			send_comm_answer(MSG_ID_SKILL_CAST_ANSWER, -2, extern_data);
			return (-55);
		}

		active_config = get_config_by_id(fuwen_config->SkillAffectId, &active_skill_config);
		if (!active_config)
		{
			LOG_ERR("%s: %lu skill %u not active_config", __FUNCTION__, extern_data->player_id, req->skillid);
			skill_cast_request__free_unpacked(req, NULL);
			return (-60);
		}

		// struct SkillLvTable *lv_config = get_config_by_id(fuwen_config->SkillLv + skill_lv - 1, &skill_lv_config);
		// if (!lv_config)
		// {
		// 	LOG_ERR("%s: %lu skill %u lv[%u] don't have lv_config", __FUNCTION__, extern_data->player_id, req->skillid, skill_lv);
		// 	skill_cast_request__free_unpacked(req, NULL);
		// 	return (-70);
		// }

		skill_struct->add_cd(fuwen_config->CD);
		LOG_INFO("%s: %lu cast skill %u cd[%lu]", __FUNCTION__, extern_data->player_id, req->skillid, skill_struct->data->cd_time);
	}
	else
	{
		LOG_INFO("%s player[%lu] cast skill[%u] in buff 3 state", __FUNCTION__, player->get_uuid(), req->skillid);
	}
	// if (lv_config->CD != 0)
	// 	skill_struct->data->cd_time = time_helper::get_cached_time() + lv_config->CD - 200;
	// else
	// 	skill_struct->data->cd_time = time_helper::get_cached_time() + active_config->TotalSkillDelay - 200;

//	LOG_DEBUG("%s: %lu cast %u, cd %lu", __FUNCTION__, extern_data->player_id, req->skillid, skill_struct->data->cd_time);

	if (player->check_pos_distance(req->cur_pos->pos_x, req->cur_pos->pos_z) != 0)
	{
		struct position *pos = player->get_pos();
		LOG_ERR("%s %d: player[%lu] cur_pos[%.1f][%.1f] flash to [%.1f][%.1f]", __FUNCTION__, __LINE__, player->get_uuid(),
			pos->pos_x, pos->pos_z, req->cur_pos->pos_x, req->cur_pos->pos_z);
	}

	player->deal_skill_cast_request(req, config->ID, fuwen_config, active_config);
	skill_cast_request__free_unpacked(req, NULL);

	return (0);
}

static int on_login_send_team_task(player_struct *player, EXTERN_DATA *extern_data)
{
	if (player->m_team)
	{
		player_struct *leader = player->m_team->GetLead();
		if (leader)
		{
			for (int i = 0; i < MAX_TASK_ACCEPTED_NUM; ++i)
			{
				TaskInfo *task = &leader->data->task_list[i];
				if (task->id == 0 || !task_is_team(task->id))
				{
					continue;
				}

				player->task_update_notify(task);
			}
		}
	}
	return 0;
}

static int on_login_send_fishing_info(player_struct *player, EXTERN_DATA *extern_data)
{
	FishingSetBaitAnswer data;
	fishing_set_bait_answer__init(&data);
	data.result = 0;
	data.baitid = player->data->fishing_bait_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_FISHING_SET_BAIT_ANSWER, fishing_set_bait_answer__pack, data);

	return 0;
}


static int on_login_send_live_skill(player_struct *player, EXTERN_DATA *extern_data);
static int on_login_send_yaoshi(player_struct *player, EXTERN_DATA *extern_data);
static int on_login_send_auto_add_hp_data(player_struct *player, EXTERN_DATA *extern_data);
static int notify_jijiangopen_gift_info(player_struct* player, EXTERN_DATA* extern_data);
static int handle_team_info_request(player_struct *player, EXTERN_DATA *extern_data);
//玩家上线处理
int pack_player_online(player_struct *player, EXTERN_DATA *extern_data, bool load_db, bool reconnect)
{
	EnterGameAnswer resp;
	enter_game_answer__init(&resp);

	if (!player)
	{
		resp.result = 190500481;
		LOG_DEBUG("[%s:%d] result[%d] player_id[%lu] scene[%u]", __FUNCTION__, __LINE__, resp.result, extern_data->player_id, resp.sceneid);
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ENTER_GAME_ANSWER, enter_game_answer__pack, resp);
		return (0);
	}

	if (player->data->n_horse == 0)
	{
		player->data->attrData[PLAYER_ATTR_CUR_HORSE] = DEFAULT_HORSE;
		player->add_horse(DEFAULT_HORSE, 0);
	}

		//如果是从数据库load数据的，要初始化玩家数据
	if (load_db)
	{
		if (player->data->attrData[PLAYER_ATTR_BAGUA] == 0)
		{
			player->data->attrData[PLAYER_ATTR_BAGUA] = 1;
		}
		if (player->data->server_level_break_notify == 0)
		{
			player->data->server_level_break_notify = global_shared_data->server_level.level_id;
		}
		player->init_horse();
		player->calculate_attribute();
		player->create_item_cache();
		player->fit_bag_grid_num();
		player->init_head_icon();
		player->load_task_end();
		player->login_check_task_time();
		player->check_task_time();
		player->init_yuqidao_mai(0, false);
		player->clear_team_task();
		player->load_partner_end();
		player->load_achievement_end();
		player->init_hero_challenge_data();
		player->load_strong_end();
		player->init_player_level_reward_data();
		player->init_online_reward_data();
		player->init_player_login_reward_receive_data();
		player->init_player_ci_fu_reward_receive_data();
		player->jiu_gong_ba_gua_reward_info_init();
		if(player->data->player_marry_info.statu == MARRY_STATU_SINGLE_NOT_MARRIAGE_HISTORY)
		{
			player->clean_player_all_marry_info(false);
		}
	}

	if (player->data->attrData[PLAYER_ATTR_HP] < __DBL_EPSILON__)
	{
		player->data->attrData[PLAYER_ATTR_HP] = player->data->attrData[PLAYER_ATTR_MAXHP];
	}

	if (player->data->teamid != 0)
	{
		player->m_team = Team::GetTeam(player->data->teamid);
		if (player->m_team == NULL)
		{
			player->data->teamid = 0;
		}
		else
		{
			if (!player->m_team->MemberOnLine(*player))
			{
				player->m_team = NULL;
				player->data->teamid = 0;
			}
		}
	}
	__attribute__((unused)) position *pos = player->get_pos();
	LOG_DEBUG("[%s:%d] player_id[%lu][%s] scene[%u] posx[%f] posz[%f]", __FUNCTION__, __LINE__, extern_data->player_id, player->data->name, player->data->scene_id, pos->pos_x, pos->pos_z);
	player->try_return_raid();

#ifndef NO_NEWRAID
	//如果是新手直接传入新手副本
	if (player->data->noviceraid_flag == false)
	{
		raid_struct *raid = raid_manager::create_raid(20035, player);
		if (raid != NULL)
		{	
			//player->scene = raid;
			//player->data->scene_id = 20035;//->m_id;
			//player->set_pos(-73, 115);
			//player->scene = raid;
			player->data->scene_id = raid->res_config->SceneID;
			//player->set_pos(raid->res_config->BirthPointX, raid->res_config->BirthPointZ);
			player->set_enter_raid_pos_and_scene(raid, raid->res_config->BirthPointX, raid->res_config->BirthPointZ);
			raid->set_player_info(player, &raid->data->player_info[0]);
			raid->player_return_raid(player);
				//设置一下新手副本离开的位置
			player->conserve_out_raid_pos_and_scene(raid->m_config);
//			buff_manager::create_buff(114400018, player, player, true);
			//player->data->pos_y = raid->res_config->BirthPointY;
			
		}
	}
#endif // !NO_NEWRAID

	if (!reconnect)
	{
		AttrMap attrs;
		for (int i = 1; i < PLAYER_ATTR_MAX; ++i)
		{
			attrs[i] = player->data->attrData[i];
		}
		player->notify_attr(attrs);
			//		notify_head_icon_info(player, extern_data);
		player->data->login_notify = false;
	}

//	float direct = 0.0;
//	get_scene_birth_pos(player->data->scene_id, NULL, NULL, NULL, &direct);
	float direct = c_angle_to_unity_angle(player->data->m_angle);

	player->m_skill.Init();

	resp.result = 0;
	resp.posx = player->get_pos()->pos_x;
	resp.posz = player->get_pos()->pos_z;
	resp.posy = player->data->pos_y;
	resp.sceneid = player->data->scene_id;
	resp.curtime = time_helper::get_cached_time() / 1000;
	resp.curhorse = player->data->attrData[PLAYER_ATTR_CUR_HORSE];
	resp.curhorsestate = player->data->attrData[PLAYER_ATTR_ON_HORSE_STATE];
	resp.fly = player->data->horse_attr.fly;
	resp.direct = direct;
	resp.horse_step = player->data->horse_attr.soul_step;

	LOG_DEBUG("[%s:%d] result[%d] player_id[%lu] scene[%u] posx[%f] posz[%f]", __FUNCTION__, __LINE__, resp.result, extern_data->player_id, resp.sceneid, resp.posx, resp.posz);
	//在MSG_ID_ENTER_GAME_ANSWER 之前发的消息客户端很可能没响应
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ENTER_GAME_ANSWER, enter_game_answer__pack, resp);

	//放在MSG_ID_ENTER_GAME_ANSWER后面，防止客户端出错
	if (reconnect)
	{ //如果是断线重连，不再通知玩家数据
		player->data->login_notify = true;
		player_ready_enter_scene(player, extern_data, true);
		player_online_to_other_srvs(player, extern_data, true);
	}

	ChengJieTaskManage::NotifyTargetLogin(player);
	player->clear_award_question();

	//更新玩家redis数据
	player->refresh_player_redis_info();
	return (0);
}

extern int on_login_send_tower_info(player_struct *player, EXTERN_DATA *extern_data);

//玩家登陆时进入场景完成
static int player_online_enter_scene_after(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	player->send_raid_earning_time_notify();
	on_login_send_auto_add_hp_data(player, extern_data);
	on_login_send_yaoshi(player, extern_data);
	notify_task_list(player, extern_data);
	notify_yuqidao_info(player, extern_data);
	notify_pvp_raid_info(player, -1, extern_data);
	notify_setting_switch_info(player, extern_data);
	notify_transfer_out_stuck_info(player, extern_data);
	notify_partner_info(player, extern_data);
	player->notify_fighting_partner();
	notify_jijiangopen_gift_info(player, extern_data);
	player->notify_activity_info(extern_data);
	on_login_send_team_task(player, extern_data);
	notify_server_level_info(player, extern_data);
	notify_server_level_break(player, extern_data);
	notify_achievement_info(player, extern_data);
	notify_title_info(player, extern_data);
	on_login_send_live_skill(player, extern_data);
	player->mijing_shilian_info_notify(0);
	on_login_send_fishing_info(player, extern_data);
	notify_strong_info(player, extern_data);
	player->notify_travel_task_info();
	player->player_level_reward_info_notify();
	player->player_online_reward_info_notify();
	player->player_signin_reward_info_notify();
	player->player_active_zhaohui_reward_info_notify();
	player->player_yaoqian_shu_info_notify();
	player->player_login_reward_info_notify();
	notify_function_info(player, extern_data);

	on_login_send_tower_info(player, extern_data);
	player->player_ci_fu_info_notify();
	player->guild_ruqin_activity_notify();
	player->jiu_gong_ba_gua_reward_info_notify();
	player->player_cur_marry_info_notify();

	handle_team_info_request(player, extern_data);
	
	player->data->login_notify = true;

	player_online_to_other_srvs(player, extern_data, false);
	
	return 0;
}

static void player_online_to_other_srvs(player_struct *player, EXTERN_DATA *extern_data, bool reconnect)
{
	//获取玩家离线缓存
	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_GET_OFFLINE_CACHE_REQUEST, 0, 0);
	
	//用户上线通知的其他服务器
	{
		PLAYER_ONLINE_NOTIFY *proto = (PLAYER_ONLINE_NOTIFY*)get_send_data();
		uint32_t data_len = sizeof(PLAYER_ONLINE_NOTIFY);
		proto->reconnect = reconnect;
		proto->cur_guild_build_task = player->get_guild_build_task();
		fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_PLAYER_ONLINE_NOTIFY, data_len, 0);
	}
}

static int check_can_transfer_to_player(player_struct *player, player_struct *target_player)
{
	if (player->is_in_raid())
	{
		LOG_ERR("%s: %lu is in raid", __FUNCTION__, player->get_uuid());
		return 190500045;
	}

	if (player->sight_space)
	{
		LOG_ERR("%s: %lu is in sightspace %p", __FUNCTION__, player->get_uuid(), player->sight_space);
		return 190500045;
	}
	if (player->data->truck.truck_id != 0)
	{
		LOG_ERR("%s: %lu is in yabiao %lu", __FUNCTION__, player->get_uuid(), player->data->truck.truck_id);
		return 190500305;
	}

	if (target_player->scene == player->scene)
	{
		LOG_INFO("%s: %lu target player[%lu] in the same scene", __FUNCTION__, player->get_uuid(), target_player->get_uuid());
		return 190500142;
	}

	if (target_player->check_can_transfer() != 0)
	{
		LOG_INFO("%s: %lu target player[%lu] is in raid", __FUNCTION__, player->get_uuid(), target_player->get_uuid());
		return 190500143;
	}

	if (target_player->scene == NULL)
	{
		LOG_INFO("%s: %lu target player[%lu] is in raid", __FUNCTION__, player->get_uuid(), target_player->get_uuid());
		return 190500143;
	}
	if (player->get_attr(PLAYER_ATTR_LEVEL) < target_player->scene->res_config->Level)
	{
		return 190300015;
	}

	return 0;
}
static int handle_transfer_to_player_scene_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_TRANSFER_TO_PLAYER_SCENE_ANSWER, -1, extern_data);
		return (-1);
	}

	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	TransferToPlayerSceneRequest *req = transfer_to_player_scene_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint64_t target_id = req->player_id;
	transfer_to_player_scene_request__free_unpacked(req, NULL);

	player_struct *target_player = player_manager::get_player_by_id(target_id);
	if (!target_player || !target_player->is_online() || !target_player->scene)
	{
		LOG_INFO("%s: %lu target player[%lu] not online", __FUNCTION__, extern_data->player_id, target_id);
		send_comm_answer(MSG_ID_TRANSFER_TO_PLAYER_SCENE_ANSWER, 190500144, extern_data);
		return (0);
	}

	int ret = check_can_transfer_to_player(player, target_player);
	if (ret != 0)
	{
		send_comm_answer(MSG_ID_TRANSFER_TO_PLAYER_SCENE_ANSWER, ret, extern_data);
		return -3;
	}

	player->move_to_scene(target_player->scene->m_id, extern_data);
	// scene_struct *new_scene = target_player->scene;

	// player->transfer_to_new_scene(new_scene->m_id, new_scene->m_born_x,
	// 	new_scene->m_born_y, new_scene->m_born_z, new_scene->m_born_direct, extern_data);
	return (0);
}

static int handle_boating_start_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_BOATING_START_ANSWER, -1, extern_data);
		return (-1);
	}

	const static struct position pos1 = {167.8, 344.3};
	const static struct position pos2 = {208.5, 49.6};
	
	if (player->scene->m_id != DEFAULT_SCENE_ID)
	{
		LOG_ERR("%s: player[%lu] at scene[%u][%.1f][%.1f] too far", __FUNCTION__, extern_data->player_id,
			player->scene->m_id, player->get_pos()->pos_x, player->get_pos()->pos_z);
		send_comm_answer(MSG_ID_BOATING_START_ANSWER, -2, extern_data);
		return (-5);
	}
	
	int ret = player->check_can_transfer();
	if (ret != 0)
	{
		LOG_ERR("%s: %lu check can transfer failed", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_BOATING_START_ANSWER, -3, extern_data);
		return (-10);
	}
	BoatingStartRequest *req = boating_start_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-20);
	}
	uint32_t type = req->type;
	boating_start_request__free_unpacked(req, NULL);
	uint32_t scene_id;
	float pos_x, pos_z;
	switch (type)//1: 湖畔村，2：去皆悦岛
	{
		case 1:
		{
			if (!check_circle_in_range(&pos1, player->get_pos(), 8))
			{
				LOG_ERR("%s: player[%lu] type[%u] at scene[%u][%.1f][%.1f] too far", __FUNCTION__, extern_data->player_id,
					type, player->scene->m_id, player->get_pos()->pos_x, player->get_pos()->pos_z);
				send_comm_answer(MSG_ID_BOATING_START_ANSWER, -2, extern_data);
				return (-5);
			}
			scene_id = DEFAULT_SCENE_ID;
			pos_x = pos2.pos_x;
			pos_z = pos2.pos_z;
		}
		break;
		case 2:
		{
			if (!check_circle_in_range(&pos2, player->get_pos(), 8))
			{
				LOG_ERR("%s: player[%lu] type[%u] at scene[%u][%.1f][%.1f] too far", __FUNCTION__, extern_data->player_id,
					type, player->scene->m_id, player->get_pos()->pos_x, player->get_pos()->pos_z);
				send_comm_answer(MSG_ID_BOATING_START_ANSWER, -2, extern_data);
				return (-5);
			}
			scene_id = DEFAULT_SCENE_ID;
			pos_x = pos1.pos_x;
			pos_z = pos1.pos_z;
		}
		break;
		default:
		{
			LOG_ERR("%s: %lu request type wrong[%u]", __FUNCTION__, extern_data->player_id, type);
			send_comm_answer(MSG_ID_BOATING_START_ANSWER, -4, extern_data);
			return (-30);
		}
	}

	assert(player->scene);
	scene_struct *old_scene = player->scene;
	player->scene->delete_player_from_scene(player);
	player->data->scene_id = scene_id;
	player->set_pos(pos_x, pos_z);
	player->add_task_progress(TCT_BOAT, type, 1);

	uint16_t new_region_id = get_region_id(old_scene->map_config, old_scene->region_config, pos_x, pos_z);
	player->send_enter_region_notify(new_region_id);

	send_comm_answer(MSG_ID_BOATING_START_ANSWER, 0, extern_data);	
	return (0);
}

static int handle_enter_scene_ready_request(player_struct *player, EXTERN_DATA *extern_data);
static int handle_boating_stop_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->data || player->scene || !player->is_alive()) {
		LOG_ERR("%s: player[%lu] check1 failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	return handle_enter_scene_ready_request(player, extern_data);
}

static int handle_one_way_trans_start_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_ONE_WAY_TRANSFER_START_ANSWER, -1, extern_data);
		return (-1);
	}

	//const static struct position pos1 = {347.1, 246.0};
	const static struct position pos2 = {104.0, 233.5};
	
	if (player->scene->m_id != DEFAULT_SCENE_ID)
	{
		LOG_ERR("%s: player[%lu] at scene[%u][%.1f][%.1f] too far", __FUNCTION__, extern_data->player_id,
			player->scene->m_id, player->get_pos()->pos_x, player->get_pos()->pos_z);
		send_comm_answer(MSG_ID_ONE_WAY_TRANSFER_START_ANSWER, -2, extern_data);
		return (-5);
	}
	
	int ret = player->check_can_transfer();
	if (ret != 0)
	{
		LOG_ERR("%s: %lu check can transfer failed", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_ONE_WAY_TRANSFER_START_ANSWER, -3, extern_data);
		return (-10);
	}
	uint32_t scene_id;
	float pos_x, pos_z;
	/*if (!check_circle_in_range(&pos1, player->get_pos(), 30))
	{
		LOG_ERR("%s: player[%lu] at scene[%u][%.1f][%.1f] too far", __FUNCTION__, extern_data->player_id, player->scene->m_id, player->get_pos()->pos_x, player->get_pos()->pos_z);
		send_comm_answer(MSG_ID_ONE_WAY_TRANSFER_START_ANSWER, -4, extern_data);
		return (-5);
	}*/
	scene_id = DEFAULT_SCENE_ID;
	pos_x = pos2.pos_x;
	pos_z = pos2.pos_z;

	assert(player->scene);
	scene_struct *old_scene = player->scene;
	player->scene->delete_player_from_scene(player);
	player->data->scene_id = scene_id;
	player->set_pos(pos_x, pos_z);
	//player->add_task_progress(TCT_BOAT, type, 1);

	uint16_t new_region_id = get_region_id(old_scene->map_config, old_scene->region_config, pos_x, pos_z);
	player->send_enter_region_notify(new_region_id);

	send_comm_answer(MSG_ID_ONE_WAY_TRANSFER_START_ANSWER, 0, extern_data);	
	return (0);
}

static int handle_one_way_trans_stop_request(player_struct *player, EXTERN_DATA *extern_data)
{
	return handle_boating_stop_request(player, extern_data);
}

static int handle_transfer_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		SceneTransferAnswer resp;
		scene_transfer_answer__init(&resp);
		resp.result = -1;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRANSFER_ANSWER, scene_transfer_answer__pack, resp);
		return (-1);
	}

	int ret = player->check_can_transfer();
	if (ret != 0)
	{
		SceneTransferAnswer resp;
		scene_transfer_answer__init(&resp);
		resp.result = ret;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRANSFER_ANSWER, scene_transfer_answer__pack, resp);
		return (-10);
	}

	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	SceneTransferRequest *req = scene_transfer_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-20);
	}

	uint32_t id = req->transfer_id;
	uint32_t type = req->type;
/*
	if (scene_id == player->data->scene_id)
	{
		LOG_ERR("%s %d: player[%lu] transfer to the same scene[%u]", __FUNCTION__, __LINE__, extern_data->player_id, scene_id);
		return (-20);
	}
*/
	scene_transfer_request__free_unpacked(req, NULL);

	if (!player->scene->can_transfer(type))
	{
		LOG_ERR("%s player[%lu] can not transfer type[%u] scene[%u]", __FUNCTION__, extern_data->player_id, type, player->scene->m_id);
		SceneTransferAnswer resp;
		scene_transfer_answer__init(&resp);
		resp.result = 190500045;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRANSFER_ANSWER, scene_transfer_answer__pack, resp);
		return (-30);
	}

	ret = player->move_to_transfer(id, extern_data);
	if (ret != 0)
	{
		LOG_ERR("%s player[%lu] can not transfer type[%u] scene[%u]", __FUNCTION__, extern_data->player_id, type, player->scene->m_id);
		SceneTransferAnswer resp;
		scene_transfer_answer__init(&resp);
		resp.result = ret;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRANSFER_ANSWER, scene_transfer_answer__pack, resp);
	}
//	player->transfer_to_new_scene_by_config(id, extern_data);
/*
	scene_struct *scene = scene_manager::get_scene(scene_id);
	if (!scene)
	{
		LOG_ERR("%s %d: player[%lu] transfer to the wrong scene[%u]", __FUNCTION__, __LINE__, extern_data->player_id, scene_id);
		return (-30);
	}

	player->scene->delete_player_from_scene(player);
	player->data->scene_id = scene_id;
	player->set_pos(scene->m_born_x, scene->m_born_z);
	if (scene_id == player->data->scene_id)
	{
		scene->add_player_to_scene(player);
	}

	SceneTransferAnswer resp;
	scene_transfer_answer__init(&resp);
	resp.direct = scene->m_born_direct;
	resp.new_scene_id = scene_id;
	resp.pos_x = scene->m_born_x;
	resp.pos_y = scene->m_born_y;
	resp.pos_z = scene->m_born_z;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRANSFER_ANSWER, scene_transfer_answer__pack, resp);
	player->interrupt();
*/
	return (0);
}

static int handle_enter_scene_ready_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (player->scene && player->scene->get_scene_type() == SCENE_TYPE_WILD)
	{
		LOG_ERR("%s: player[%lu] already in scene %u", __FUNCTION__, player->data->player_id, player->scene->m_id);
		return (0);
	}

	if (player->area || player->sight_space)
	{
		LOG_ERR("%s %d: player[%lu] already in scene", __FUNCTION__, __LINE__, player->data->player_id);
		return (0);
	}

	player_ready_enter_scene(player, extern_data, false);
	player->data->xunbao.door_map = 0;
	player->data->xunbao.door_id = 0;
	player->data->xunbao.cd = 0;
	if (player->m_team != NULL)
	{
		player->m_team->SendXunbaoPoint(*player);
	}
	if (player->scene != NULL && player->scene->res_config->UseMounts == 0)
	{
		player->down_horse();
	}

	return (0);
}

//前端进场景loading完，后端把玩家加入场景
static void player_ready_enter_scene(player_struct *player, EXTERN_DATA *extern_data, bool reconnect)
{
	bool bSuccess = false;
	do
	{
		raid_struct *raid = player->get_raid();
		if (raid)
		{
			raid->add_player_to_scene(player);
			if (player->data->noviceraid_flag == false && raid->res_config->SceneID == 20035 && get_entity_type(player->get_uuid()) == ENTITY_TYPE_PLAYER)
			{
					//为了通知客户端隐藏副本UI
				EXTERN_DATA extern_data;
				extern_data.player_id = player->get_uuid();

				EnterRaidNotify notify;
				enter_raid_notify__init(&notify);
				notify.raid_id = raid->m_id;
				fast_send_msg(&conn_node_gamesrv::connecter, &extern_data, MSG_ID_ENTER_RAID_NOTIFY, enter_raid_notify__pack, notify);
//				player->send_scene_transfer(raid->res_config->FaceY, raid->res_config->BirthPointX, raid->res_config->BirthPointY,
//					raid->res_config->BirthPointZ, raid->m_id, 0);
			}
			if (raid->ai && raid->ai->raid_on_player_ready && raid->m_config->DengeonRank != DUNGEON_TYPE_ZHENYING)
				raid->ai->raid_on_player_ready(raid, player);
			//player->set_attr(PLAYER_ATTR_PK_TYPE, PK_TYPE_NORMAL);
			//player->broadcast_one_attr_changed(PLAYER_ATTR_PK_TYPE, PK_TYPE_NORMAL, false, true);
			bSuccess = true;

			if (raid->is_guild_battle_raid() && !is_guild_battle_opening())
			{
				raid->player_leave_raid(player);
				if (raid->check_raid_need_delete())
				{
					if (is_guild_wait_raid(raid->m_id))
					{
						guild_wait_raid_manager::delete_guild_wait_raid((guild_wait_raid_struct*)raid);
					}
					else
					{
						raid_manager::delete_raid(raid);
					}
				}
			}
			break;
		}

		scene_struct *scene = NULL;
		scene = scene_manager::get_scene(player->data->scene_id);
//		if (player->data->scene_id > 30000)
//		{
//			scene = ZhenyingBattle::GetInstance()->GetFiled();
//		}
		if (!scene)
		{
			LOG_ERR("%s %d: player[%lu] want join scene[%u] failed", __FUNCTION__, __LINE__, player->data->player_id, player->data->scene_id);
			break;
		}

		scene->add_player_to_scene(player);
		bSuccess = true;
		player->notify_watch_pos_change();
	} while(0);

	if (bSuccess)
	{
		if (!player->data->login_notify)
		{
			player->send_buff_info();
			player_online_enter_scene_after(player, extern_data);
			raid_struct *raid = player->get_raid();
			if (raid && raid->m_config->DengeonRank == DUNGEON_TYPE_ZHENYING)
			{
				if (raid->ai && raid->ai->raid_on_player_ready)
					raid->ai->raid_on_player_ready(raid, player);
			}
		}
		//门宗传功切场景处理
		if (player->is_in_guild_chuan_gong())
		{
			player->guild_chuan_gong_deal_with();
		}
	}
}

//玩家登陆
static int handle_enter_game(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	PROTO_ENTER_GAME_REQ* req = (PROTO_ENTER_GAME_REQ*)conn_node_gamesrv::connecter.buf_head();
	uint64_t player_id = req->player_id;
	bool reconnect = req->reconnect != 0;

	player_struct* ret = player_manager::get_player_by_id(player_id);
	if (ret) {
		if (ret->data->status == ONLINE) {
				//已经在线了？
			LOG_ERR("[%s:%d] player_id[%lu]", __FUNCTION__, __LINE__, player_id);
			return (0);
		}
		else {
			//之前写回DB还没确认的玩家, 此时上线直接使用之前的内存数据
			ret->data->status = ONLINE;
		}
	}
	else {
		int result = conn_node_dbsrv::connecter->send_one_msg(&req->head, 1);
		if (result != (int)ENDION_FUNC_4(req->head.len)) {
			LOG_ERR("[%s : %d]: send data to db server failed", __FUNCTION__, __LINE__);
		}

		return 0;
	}

//  scene_struct *scene = scene_manager::get_scene(ret->data->scene_id);
//	if (scene) {
//		scene->add_player_to_scene(ret);
//	}
//	ret->data->raid_uuid = 0;

	return pack_player_online(ret, extern_data, false, reconnect);
}

//static int handle_set_fashion_request(player_struct *player, EXTERN_DATA *extern_data)
//{
//	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
//	if (!player || !player->is_online())
//	{
//		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
//		return (-1);
//	}
//
//	SetFashion *req = set_fashion__unpack(NULL, get_data_len(), (uint8_t *)get_data());
//	if (!req)
//	{
//		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
//		return (-10);
//	}
//
//	if (req->id >= PLAYER_ATTR_CLOTHES && req->id <= PLAYER_ATTR_CLOTHES_COLOR_DOWN)
//	{
//		player->set_attr(req->id, req->vaual);
//		AttrMap attrs;
//		attrs[req->id] = req->vaual;
//		player->notify_attr(attrs, true, true);
//	}
//
//	set_fashion__free_unpacked(req, NULL);
//
//	return 0;
//}

//玩家下线
static int handle_kick_player(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	LOG_DEBUG("[%s:%d] player_id:%lu, player:%p, data:%p", __FUNCTION__, __LINE__, player->data->player_id, player, player->data);

	PROTO_ROLE_KICK *notify = (PROTO_ROLE_KICK*)buf_head();
	bool again = (notify->again == 0 ? false : true);
	player->process_offline(again, extern_data);

	return 0;
}

//背包信息请求
static int handle_bag_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

//	player->tidy_bag();
	BagInfoAnswer resp;
	bag_info_answer__init(&resp);

	BagGrid bag_data[MAX_BAG_GRID_NUM];
	BagGrid *bag_data_point[MAX_BAG_GRID_NUM];
	ItemBaguaData bagua_data[MAX_BAG_GRID_NUM];
	ItemPartnerFabaoData fabao_data[MAX_BAG_GRID_NUM];
	ItemRandomBoxData box_data[MAX_BAG_GRID_NUM];
	CommonRandAttrData  bagua_attr[MAX_BAG_GRID_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* bagua_attr_point[MAX_BAG_GRID_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData  bagua_additional_attr[MAX_BAG_GRID_NUM][MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];
	CommonRandAttrData* bagua_additional_attr_point[MAX_BAG_GRID_NUM][MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];
	AttrData item_fabao_attr[MAX_BAG_GRID_NUM][MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData* item_fabao_attr_point[MAX_BAG_GRID_NUM][MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData fabao_attr[MAX_BAG_GRID_NUM];
	ItemData box_item_data[MAX_BAG_GRID_NUM];

	resp.result = 0;
	resp.curgridnum = player->data->bag_grid_num;
	resp.totalgridnum = get_bag_total_num(player->data->attrData[PLAYER_ATTR_JOB], player->data->attrData[PLAYER_ATTR_LEVEL]);
	resp.n_grids = 0;
	resp.grids = bag_data_point;
	for (uint32_t i = 0; i < player->data->bag_grid_num; ++i)
	{
		if (player->data->bag[i].id == 0)
		{
			continue;
		}

		bag_data_point[resp.n_grids] = &bag_data[resp.n_grids];
		bag_grid__init(&bag_data[resp.n_grids]);
		bag_data[resp.n_grids].index = i;
		bag_data[resp.n_grids].id = player->data->bag[i].id;
		bag_data[resp.n_grids].num = player->data->bag[i].num;
		bag_data[resp.n_grids].usedcount = player->data->bag[i].used_count;
		bag_data[resp.n_grids].expiretime = player->data->bag[i].expire_time;
		if (item_is_baguapai(player->data->bag[i].id))
		{
			bag_data[resp.n_grids].bagua = &bagua_data[resp.n_grids];
			item_bagua_data__init(&bagua_data[resp.n_grids]);
			bagua_data[resp.n_grids].star = player->data->bag[i].especial_item.baguapai.star;
			uint32_t attr_num = 0;
			for (int j = 0; j < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++j)
			{
				if (player->data->bag[i].especial_item.baguapai.minor_attrs[j].id == 0)
				{
					break;
				}

				bagua_attr_point[resp.n_grids][attr_num] = &bagua_attr[resp.n_grids][attr_num];
				common_rand_attr_data__init(&bagua_attr[resp.n_grids][attr_num]);
				bagua_attr[resp.n_grids][attr_num].pool = player->data->bag[i].especial_item.baguapai.minor_attrs[j].pool;
				bagua_attr[resp.n_grids][attr_num].id = player->data->bag[i].especial_item.baguapai.minor_attrs[j].id;
				bagua_attr[resp.n_grids][attr_num].val = player->data->bag[i].especial_item.baguapai.minor_attrs[j].val;
				attr_num++;
			}
			bagua_data[resp.n_grids].minor_attrs = bagua_attr_point[resp.n_grids];
			bagua_data[resp.n_grids].n_minor_attrs = attr_num;
			attr_num = 0;
			for (int j = 0; j < MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM; ++j)
			{
				if (player->data->bag[i].especial_item.baguapai.additional_attrs[j].id == 0)
				{
					break;
				}

				bagua_additional_attr_point[resp.n_grids][attr_num] = &bagua_additional_attr[resp.n_grids][attr_num];
				common_rand_attr_data__init(&bagua_additional_attr[resp.n_grids][attr_num]);
				bagua_additional_attr[resp.n_grids][attr_num].pool = player->data->bag[i].especial_item.baguapai.additional_attrs[j].pool;
				bagua_additional_attr[resp.n_grids][attr_num].id = player->data->bag[i].especial_item.baguapai.additional_attrs[j].id;
				bagua_additional_attr[resp.n_grids][attr_num].val = player->data->bag[i].especial_item.baguapai.additional_attrs[j].val;
				attr_num++;
			}
			bagua_data[resp.n_grids].additional_attrs = bagua_additional_attr_point[resp.n_grids];
			bagua_data[resp.n_grids].n_additional_attrs = attr_num;
		}
		if (item_is_partner_fabao(player->data->bag[i].id))
		{
			bag_data[resp.n_grids].fabao = &fabao_data[resp.n_grids];
			item_partner_fabao_data__init(&fabao_data[resp.n_grids]);
			fabao_data[resp.n_grids].main_attr = &fabao_attr[i];
			attr_data__init(&fabao_attr[i]);
			fabao_attr[i].id =  player->data->bag[i].especial_item.fabao.main_attr.id;
			fabao_attr[i].val = player->data->bag[i].especial_item.fabao.main_attr.val;
			uint32_t attr_num = 0;
			for (int j = 0; j < MAX_HUOBAN_FABAO_MINOR_ATTR_NUM; ++j)
			{
				if (player->data->bag[i].especial_item.fabao.minor_attr[j].id == 0)
				{
					break;
				}

				item_fabao_attr_point[resp.n_grids][attr_num] = &item_fabao_attr[resp.n_grids][attr_num];
				attr_data__init(&item_fabao_attr[resp.n_grids][attr_num]);
				item_fabao_attr[resp.n_grids][attr_num].id = player->data->bag[i].especial_item.fabao.minor_attr[j].id;
				item_fabao_attr[resp.n_grids][attr_num].val = player->data->bag[i].especial_item.fabao.minor_attr[j].val;
				attr_num++;
			}
			fabao_data[resp.n_grids].minor_attr = item_fabao_attr_point[resp.n_grids];
			fabao_data[resp.n_grids].n_minor_attr = attr_num;
		}
		if (item_is_random_box(player->data->bag[i].id))
		{
			bag_data[resp.n_grids].box = &box_data[resp.n_grids];
			item_random_box_data__init(&box_data[resp.n_grids]);
			box_data[resp.n_grids].randitem = &box_item_data[resp.n_grids];
			item_data__init(&box_item_data[resp.n_grids]);
			box_item_data[resp.n_grids].id = player->data->bag[i].especial_item.box.item_id;
			box_item_data[resp.n_grids].num = player->data->bag[i].especial_item.box.item_num;
		}
		resp.n_grids++;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAG_INFO_ANSWER, bag_info_answer__pack, resp);

	return 0;
}

//背包开格子请求
static int handle_bag_unlock_grid_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	int ret = 0;
	uint32_t total_grid_num = 0;
	do
	{
		uint32_t player_job = player->data->attrData[PLAYER_ATTR_JOB];
		uint32_t player_level = player->data->attrData[PLAYER_ATTR_LEVEL];
		ActorLevelTable *config = get_actor_level_config(player_job, player_level);
		if (!config)
		{
			LOG_ERR("[%s:%d] player[%lu] get ActorLevelTable failed, job:%u, level:%u", __FUNCTION__, __LINE__, extern_data->player_id, player_job, player_level);
			ret = ERROR_ID_NO_CONFIG;
			break;
		}

		total_grid_num = config->FreeGrid + config->LockGrid;

		if (player->data->bag_unlock_num >= config->LockGrid)
		{
			LOG_ERR("[%s:%d] player[%lu] has unlocked all, has_unlock:%u, can_unlock:%lu", __FUNCTION__, __LINE__, extern_data->player_id, player->data->bag_unlock_num, config->LockGrid);
			ret = ERROR_ID_BAG_UNLOCK_ALL;
			break;
		}

		const uint32_t unlock_num_per_time = 5;
		uint32_t unlock_times = ceil(player->data->bag_unlock_num / (double)unlock_num_per_time);
		uint32_t need_coin = sg_bag_unlock_base_price + sg_bag_unlock_incr_factor * unlock_times;
		uint32_t player_coin = player->get_coin();
		if (player_coin < need_coin)
		{
			LOG_ERR("[%s:%d] player[%lu] coin not enough, need_coin:%u, player_coin:%u", __FUNCTION__, __LINE__, extern_data->player_id, need_coin, player_coin);
			ret = 190300004;
			break;
		}

		//check end
		player->sub_coin(need_coin, MAGIC_TYPE_BAG_UNLOCK);
		player->data->bag_unlock_num = std::min(player->data->bag_unlock_num + unlock_num_per_time, (uint32_t)config->LockGrid);
		player->fit_bag_grid_num();

		player->add_achievement_progress(ACType_BAG_UNLOCK, 0, 0, 0, 1);
		player->add_achievement_progress(ACType_BAG_GRID_NUM, player->data->bag_grid_num, 0, 0, 1);

	} while(0);

	BagUnlockGridAnswer resp;
	bag_unlock_grid_answer__init(&resp);

	resp.result = ret;
	resp.curgridnum = player->data->bag_grid_num;
	resp.totalgridnum = (total_grid_num > 0 ? total_grid_num : resp.curgridnum);

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAG_UNLOCK_GRID_ANSWER, bag_unlock_grid_answer__pack, resp);

	return 0;
}

//背包出售物品请求
static int handle_bag_sell_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BagSellRequest *req = bag_sell_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t pos = req->index;
	uint32_t num = req->num;

	bag_sell_request__free_unpacked(req, NULL);

	int ret = 0;
	ItemData sell_item;
	item_data__init(&sell_item);
	AttrData gain_money;
	attr_data__init(&gain_money);
	do
	{
		if (!(pos < player->data->bag_grid_num))
		{
			ret = ERROR_ID_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] bag pos error, pos:%u, grid_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, player->data->bag_grid_num);
			break;
		}

		bag_grid_data& grid = player->data->bag[pos];
		ItemsConfigTable *prop_config = get_config_by_id(grid.id, &item_config);
		if (!prop_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get itemconfig failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, grid.id);
			break;
		}

		if (prop_config->Price == (uint64_t)-1)
		{
			ret = ERROR_ID_PROP_CAN_NOT_SELL;
			LOG_ERR("[%s:%d] player[%lu] prop can not be sold", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (num > grid.num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] prop num error, num:%u, bag_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, num, grid.num);
			break;
		}

		sell_item.id = grid.id;
		sell_item.num = num;
		player->del_item_by_pos(pos, num, MAGIC_TYPE_BAG_SELL);
		uint32_t sell_coin = prop_config->Price * num;
		if (sell_coin > 0)
		{
			player->add_coin(sell_coin, MAGIC_TYPE_BAG_SELL);
			gain_money.id = PLAYER_ATTR_COIN;
			gain_money.val = sell_coin;
		}
	} while(0);

	BagSellAnswer resp;
	bag_sell_answer__init(&resp);

	resp.result = ret;
	resp.sellitem = &sell_item;
	resp.gainmoney = &gain_money;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAG_SELL_ANSWER, bag_sell_answer__pack, resp);

	return 0;
}

//背包使用物品请求
static int handle_bag_use_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	if (!player->is_alive())
	{
		LOG_ERR("%s: player[%lu] dead", __FUNCTION__, extern_data->player_id);
		return (-5);
	}

	BagUseRequest *req = bag_use_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t pos = req->index;
	uint32_t use_all = req->use_all;
	bool is_easy = req->isesay;

	bag_use_request__free_unpacked(req, NULL);

	ItemsConfigTable *prop_config = NULL;
	int ret = 0;
	uint32_t item_id = 0;
	ItemUseEffectInfo *effect_info = new ItemUseEffectInfo();
	effect_info->pos = pos;
	effect_info->use_all = use_all;
	effect_info->is_easy = is_easy;
	bool need_free = true;
	std::map<uint32_t, uint32_t> *item_map = NULL;
	do
	{
		if (player->is_item_expire(pos))
		{
			ret = 190300010;
			LOG_ERR("[%s:%d] player[%lu] item is expire, pos:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos);
			break;
		}

		if (!(pos < player->data->bag_grid_num))
		{
			ret = ERROR_ID_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] bag pos error, pos:%u, grid_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, player->data->bag_grid_num);
			break;
		}

		bag_grid_data& grid = player->data->bag[pos];
		prop_config = get_config_by_id(grid.id, &item_config);
		if (!prop_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get itemconfig failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, grid.id);
			break;
		}

		if (player->check_item_cd(prop_config) != 0)
		{
			ret = ERROR_ID_PROP_IN_CD;
			LOG_ERR("[%s:%d] player[%lu] in cd, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, grid.id);
			break;
		}

		item_id = grid.id;

		if (prop_config->UseDegree == 0)
		{
			ret = ERROR_ID_PROP_CAN_NOT_USE;
			LOG_ERR("[%s:%d] player[%lu] item can't use, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, grid.id);
			break;
		}

		ret = player->try_use_prop(pos, use_all, effect_info);
		if (ret != 0)
		{
			break;
		}

		if (prop_config->ItemEffect == IUE_EXTEND_FRIEND_NUM)
		{
			//换一个消息号，转发到好友服处理
			int data_len = get_data_len();
			memcpy(get_send_data(), get_data(), data_len);
			fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_FRIEND_EXTEND_CONTACT_REQUEST, data_len, get_seq());
			return 0;
		}

		if (prop_config->CostTime > 0)
		{
			need_free = false;
			player->stop_move();
			if (prop_config->ItemEffect == IUE_XUNBAO)
			{
				player->begin_sing(SING_TYPE__XUNBAO, prop_config->CostTime, false, true, effect_info);
			}
			else
			{
				player->begin_sing(SING_TYPE__USE_PROP, prop_config->CostTime, true, true, effect_info);
			}
		}
		else
		{
			ret = player->use_prop(pos, use_all, effect_info);
			item_map = &effect_info->items;
		}
	} while(0);

	BagUseAnswer resp;
	bag_use_answer__init(&resp);

	ItemData item_data[50];
	ItemData* item_data_point[50];

	resp.result = ret;
	if (ret == 0)
	{
		assert(prop_config);
		resp.cd = player->set_item_cd(prop_config);
	}
	resp.item_id = item_id;
	resp.n_itemlist = 0;
	resp.itemlist = item_data_point;
	if (item_map)
	{
		for (std::map<uint32_t, uint32_t>::iterator iter = item_map->begin(); iter != item_map->end() && resp.n_itemlist < 50; ++iter)
		{
			item_data_point[resp.n_itemlist] = &item_data[resp.n_itemlist];
			item_data__init(&item_data[resp.n_itemlist]);
			item_data[resp.n_itemlist].id = iter->first;
			item_data[resp.n_itemlist].num = iter->second;
			resp.n_itemlist++;
		}
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAG_USE_ANSWER, bag_use_answer__pack, resp);

	if (need_free)
	{
		delete effect_info;
		effect_info = NULL;
	}

	return 0;
}

//背包堆叠物品请求
static int handle_bag_stack_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BagStackRequest *req = bag_stack_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t id = req->id;

	bag_stack_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		player->merge_item(id);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAG_STACK_ANSWER, comm_answer__pack, resp);

	return 0;
}

//背包整理请求
static int handle_bag_tidy_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}


//	int ret = 0;
	do
	{
		player->tidy_bag();
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = 0;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAG_TIDY_ANSWER, comm_answer__pack, resp);

	return 0;
}

struct BatchSellBase
{
	uint32_t pos;
	uint32_t num;
	uint32_t price;
};

//背包批量出售物品请求
static int handle_bag_batch_sell_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BagBatchSellRequest *req = bag_batch_sell_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	std::vector<uint32_t> poses;
	for (size_t i = 0; i < req->n_indexs; ++i)
	{
		poses.push_back(req->indexs[i]);
	}

	bag_batch_sell_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		std::vector<BatchSellBase> sell_infos;
		for (std::vector<uint32_t>::iterator iter = poses.begin(); iter != poses.end(); ++iter)
		{
			uint32_t pos = *iter;
			if (!(pos < player->data->bag_grid_num))
			{
				ret = ERROR_ID_BAG_POS;
				LOG_ERR("[%s:%d] player[%lu] bag pos error, pos:%u, grid_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, player->data->bag_grid_num);
				break;
			}

			bag_grid_data& grid = player->data->bag[pos];
			ItemsConfigTable *prop_config = get_config_by_id(grid.id, &item_config);
			if (!prop_config)
			{
				ret = ERROR_ID_NO_CONFIG;
				LOG_ERR("[%s:%d] player[%lu] get itemconfig failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, grid.id);
				break;
			}

			if (prop_config->Price == (uint64_t)-1)
			{
				ret = ERROR_ID_PROP_CAN_NOT_SELL;
				LOG_ERR("[%s:%d] player[%lu] prop can not be sold", __FUNCTION__, __LINE__, extern_data->player_id);
				break;
			}

			BatchSellBase info;
			info.pos = pos;
			info.num = grid.num;
			info.price = prop_config->Price;
			sell_infos.push_back(info);
		}

		if (ret != 0)
		{
			break;
		}

		uint32_t sell_coin = 0;
		for (std::vector<BatchSellBase>::iterator iter = sell_infos.begin(); iter != sell_infos.end(); ++iter)
		{
			BatchSellBase &info = *iter;
			player->del_item_by_pos(info.pos, info.num, MAGIC_TYPE_BAG_SELL);
			sell_coin += info.price * info.num;
		}
		if (sell_coin > 0)
		{
			player->add_coin(sell_coin, MAGIC_TYPE_BAG_SELL);
		}
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAG_BATCH_SELL_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int bag_item_combin(player_struct *player, uint32_t id, uint32_t num, uint32_t type)
{
	if (num == 0)
	{
		return 5;
	}
	SyntheticTable *table = get_config_by_id(id, &item_combin_config);
	if (table == NULL)
	{
		return 1;
	}
	if (player->get_coin() < table->Consume *num)
	{
		return 190500498;
	}
	if (type == 0)
	{
		for (uint32_t i = 0; i < table->n_SyntheticMaterial; ++i)
		{
			if ((uint32_t)player->get_item_can_use_num(table->SyntheticMaterial[i]) < table->SyntheticMaterialNum[i] * num)
			{
				return 190500115;
			}
		}
		for (uint32_t i = 0; i < table->n_SyntheticMaterial; ++i)
		{
			player->del_item(table->SyntheticMaterial[i], table->SyntheticMaterialNum[i] * num, 0);
		}
	}
	else//非绑定
	{
		for (uint32_t i = 0; i < table->n_SyntheticMaterial; ++i)
		{
			if ((uint32_t)player->get_item_num_by_id(table->SyntheticMaterial[i]) < table->SyntheticMaterialNum[i] * num)
			{
				return 190500115;
			}
		}
		for (uint32_t i = 0; i < table->n_SyntheticMaterial; ++i)
		{
			player->del_item_by_id(table->SyntheticMaterial[i], table->SyntheticMaterialNum[i] * num, 0);
		}
	}
	player->sub_coin(table->Consume * num, 0);
	player->add_item(table->SyntheticTarget, num, 0);
	return 0;
}
static int handle_bag_item_combin_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, extern_data->player_id);
		return -1;
	}

	ItemCombin *req = item_combin__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	send_comm_answer(MSG_ID_BAG_ITEM_COMBIN_ANSWER, bag_item_combin(player, req->id, req->num, req->cost_type), extern_data);
	item_combin__free_unpacked(req, NULL);

	return (0);
}

static int handle_relive_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player->is_avaliable())
	{
		LOG_ERR("%s: player[%lu] do not have scene or data", __FUNCTION__, extern_data->player_id);
		return -10;
	}

	if (player->is_alive())
	{
		LOG_ERR("%s: player[%lu] already alive", __FUNCTION__, extern_data->player_id);
		send_comm_answer(MSG_ID_RELIVE_ANSWER, -1, extern_data);
		return (-1);
	}
	ReliveRequest *req = relive_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;
	relive_request__free_unpacked(req, NULL);
	LOG_INFO("%s: player %lu type %u", __FUNCTION__, extern_data->player_id, type);

	player->on_relive(type);
	return (0);
}

//改名请求
static int handle_rename_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PlayerRenameRequest *req = player_rename_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	std::string name(req->name);
	player_rename_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		size_t name_len = name.length();
		if (name_len == 0 || name_len >= MAX_PLAYER_NAME_LEN)
		{
			ret = ERROR_ID_NAME_LEN;
			LOG_ERR("[%s:%d] player[%lu] name length error, name_len:%lu", __FUNCTION__, __LINE__, extern_data->player_id, name_len);
			break;
		}

		uint32_t item_id = sg_rename_item_id;
		uint32_t item_num = sg_rename_item_num;
		uint32_t bag_num = player->get_item_can_use_num(item_id);
		if (bag_num < item_num)
		{
			ret = 190300011;
			LOG_ERR("[%s:%d] player[%lu] item not enough, id:%u, need_num:%u, bag_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id, item_num, bag_num);
			break;
		}
	} while(0);

	if (ret == 0)
	{
		memcpy(get_send_data(), get_data(), get_data_len());
		fast_send_msg_base(conn_node_dbsrv::connecter, extern_data, MSG_ID_PLAYER_RENAME_REQUEST, get_data_len(), get_seq());
	}
	else
	{
		CommAnswer resp;
		comm_answer__init(&resp);

		resp.result = ret;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PLAYER_RENAME_ANSWER, comm_answer__pack, resp);
	}

	return 0;
}

//头像信息请求
static int handle_head_icon_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	return notify_head_icon_info(player, extern_data);
}

static int notify_head_icon_info(player_struct *player, EXTERN_DATA *extern_data)
{
	HeadIconInfoAnswer resp;
	head_icon_info_answer__init(&resp);

	HeadIconData head_data[MAX_HEAD_ICON_NUM];
	HeadIconData* head_data_point[MAX_HEAD_ICON_NUM];
	resp.result = 0;
	resp.n_icon_list = 0;
	resp.icon_list = head_data_point;
	for (int i = 0; i < MAX_HEAD_ICON_NUM; ++i)
	{
		if (player->data->head_icon_list[i].id == 0)
		{
			continue;
		}

		head_data_point[resp.n_icon_list] = &head_data[resp.n_icon_list];
		head_icon_data__init(&head_data[resp.n_icon_list]);
		head_data[resp.n_icon_list].id = player->data->head_icon_list[i].id;
		head_data[resp.n_icon_list].status = player->data->head_icon_list[i].status;
		resp.n_icon_list++;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HEAD_ICON_INFO_ANSWER, head_icon_info_answer__pack, resp);

	return 0;
}

//头像更换请求
static int handle_head_icon_replace_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	HeadIconReplaceRequest *req = head_icon_replace_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t replace_icon = req->icon_id;
	head_icon_replace_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		uint32_t using_icon = player->data->attrData[PLAYER_ATTR_HEAD];
		if (using_icon == replace_icon)
		{
//			ret = ERROR_ID_ICON_USING;
//			LOG_ERR("[%s:%d] player[%lu] icon is using, icon:%u", __FUNCTION__, __LINE__, extern_data->player_id, replace_icon);
			break;
		}

		HeadIconInfo *icon_info = player->get_head_icon(replace_icon);
		if (!icon_info)
		{
			ret = ERROR_ID_ICON_NEED_UNLOCK;
			LOG_ERR("[%s:%d] player[%lu] icon is not unlock, icon:%u", __FUNCTION__, __LINE__, extern_data->player_id, replace_icon);
			break;
		}

		player->data->attrData[PLAYER_ATTR_HEAD] = replace_icon;
		AttrMap attrs;
		attrs[PLAYER_ATTR_HEAD] = player->data->attrData[PLAYER_ATTR_HEAD];
		player->notify_attr(attrs, true);

		player->add_achievement_progress(ACType_HEAD_REPLACE, 0, 0, 0, 1);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HEAD_ICON_REPLACE_ANSWER, comm_answer__pack, resp);

	return 0;
}

//头像更换请求
static int handle_head_icon_old_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	HeadIconReplaceRequest *req = head_icon_replace_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t icon_id = req->icon_id;
	head_icon_replace_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		HeadIconInfo *icon_info = player->get_head_icon(icon_id);
		if (!icon_info)
		{
			ret = ERROR_ID_ICON_NEED_UNLOCK;
			LOG_ERR("[%s:%d] player[%lu] icon is not unlock, icon:%u", __FUNCTION__, __LINE__, extern_data->player_id, icon_id);
			break;
		}

		icon_info->status = 0;
	} while(0);

	HeadIconOldAnswer resp;
	head_icon_old_answer__init(&resp);

	resp.result = ret;
	resp.icon_id = icon_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HEAD_ICON_OLD_ANSWER, head_icon_old_answer__pack, resp);

	return 0;
}

// static int handle_bag_show_request(player_struct *player, EXTERN_DATA *extern_data)
// {
// 	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
// 	if (!player || !player->is_online())
// 	{
// 		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
// 		return (-1);
// 	}

// 	ShowItemRequest *req = show_item_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
// 	if (!req)
// 	{
// 		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
// 		return (-10);
// 	}

// 	//uint32_t index = req->index;
// 	show_item_request__free_unpacked(req, NULL);

// 	SHOW_ITEM_TO_ITEMSRV *pShow = (SHOW_ITEM_TO_ITEMSRV *)&conn_node_base::global_send_buf[0];

// 	ShowItemData showData;
// 	show_item_data__init(&showData);
// 	pShow->data_size = show_item_data__pack(&showData, pShow->data);
// 	showData.id = 11111;
// 	//memcpy(&(pShow->data[0]), &index, sizeof(uint32_t));
// 	pShow->head.msg_id = 0;
// 	pShow->type = 1;
// 	pShow->head.len = ENDION_FUNC_4(sizeof(SHOW_ITEM_TO_ITEMSRV) + pShow->data_size);

// 	conn_node_base::add_extern_data(&pShow->head, extern_data);

//	if (conn_node_itemsrv::connecter.send_one_msg(&pShow->head, 1) != (int)ENDION_FUNC_4(pShow->head.len)) {
//		LOG_ERR("%s %d: send to itemsrv err[%d]", __FUNCTION__, __LINE__, errno);
//	}

// 	return 0;
// }

//任务列表请求
static int handle_task_list_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	return notify_task_list(player, extern_data);
}

static int notify_task_list(player_struct *player, EXTERN_DATA *extern_data)
{
	TaskListAnswer resp;
	task_list_answer__init(&resp);

	int index = 0;
	TaskRewardData reward_data[MAX_TASK_ACCEPTED_NUM];
	ItemData item_data[MAX_TASK_ACCEPTED_NUM][MAX_SHANGJIN_AWARD_NUM];
	ItemData *item_data_point[MAX_TASK_ACCEPTED_NUM][MAX_SHANGJIN_AWARD_NUM];
	TaskData task_data[MAX_TASK_ACCEPTED_NUM];
	TaskData* task_data_point[MAX_TASK_ACCEPTED_NUM];
	TaskCount count_data[MAX_TASK_ACCEPTED_NUM][MAX_TASK_TARGET_NUM];
	TaskCount* count_data_point[MAX_TASK_ACCEPTED_NUM][MAX_TASK_TARGET_NUM];
	TaskNumberData number_data[MAX_TASK_ACCEPTED_NUM];
	resp.result = 0;
	resp.n_ongoing_list = 0;
	resp.ongoing_list = task_data_point;
	for (int i = 0; i < MAX_TASK_ACCEPTED_NUM; ++i)
	{
		TaskInfo &info = player->data->task_list[i];
		if (info.id == 0)
		{
			continue;
		}

		task_data_point[resp.n_ongoing_list] = &task_data[resp.n_ongoing_list];
		task_data__init(&task_data[resp.n_ongoing_list]);
		task_data[resp.n_ongoing_list].id = info.id;
		task_data[resp.n_ongoing_list].status = info.status;
		task_data[resp.n_ongoing_list].expiretime = player->get_task_expire_time(&info);
		LOG_INFO("[%s:%d] player[%lu] name=%s,task_id=%d, status=%d,expiretime=%d", __FUNCTION__, __LINE__, extern_data->player_id, player->get_name(), info.id, info.status, task_data[resp.n_ongoing_list].expiretime);
		uint32_t nTarget = 0;
		for (int j = 0; j < MAX_TASK_TARGET_NUM; j++)
		{
			if (info.progress[j].id == 0)
			{
				continue;
			}

			count_data_point[i][nTarget] = &count_data[i][nTarget];
			task_count__init(&count_data[i][nTarget]);
			count_data[i][nTarget].type = info.progress[j].id;
			count_data[i][nTarget].count = info.progress[j].num;
			nTarget++;
		}
		task_data[resp.n_ongoing_list].n_progress = nTarget;
		task_data[resp.n_ongoing_list].progress = count_data_point[i];

		uint32_t task_type = get_task_type(info.id);
		if (task_type == TT_SHANGJIN)
		{//只发当前赏金任务奖励
			task_reward_data__init(&reward_data[resp.n_ongoing_list]);
			task_data[resp.n_ongoing_list].reward = &reward_data[resp.n_ongoing_list];
			reward_data[resp.n_ongoing_list].exp = player->data->shangjin.task[player->data->shangjin.cur_task].exp;
			reward_data[resp.n_ongoing_list].coin = player->data->shangjin.task[player->data->shangjin.cur_task].coin;
			reward_data[resp.n_ongoing_list].n_items = pack_drop_config_item(player->data->shangjin.task[player->data->shangjin.cur_task].drop_id,
				MAX_SHANGJIN_AWARD_NUM, &index, &reward_data[resp.n_ongoing_list].items);
			
//			reward_data[resp.n_ongoing_list].items = item_data_point[resp.n_ongoing_list];
			// size_t &n_item = reward_data[resp.n_ongoing_list].n_items;
			// for (; n_item < player->data->shangjin.task[player->data->shangjin.cur_task].n_award; ++n_item)
			// {
			// 	item_data_point[resp.n_ongoing_list][n_item] = &(item_data[resp.n_ongoing_list][n_item]);
			// 	item_data__init(item_data_point[resp.n_ongoing_list][n_item]);
			// 	item_data[resp.n_ongoing_list][n_item].id = player->data->shangjin.task[player->data->shangjin.cur_task].award[n_item].id;
			// 	item_data[resp.n_ongoing_list][n_item].num = player->data->shangjin.task[player->data->shangjin.cur_task].award[n_item].val;
			// }
		}
		else if (task_type == TT_CASH_TRUCK)
		{
			BiaocheTable *table = get_config_by_id(player->data->truck.active_id, &cash_truck_config);
			if (table != NULL)
			{
				BiaocheRewardTable *reward_config = get_config_by_id(table->Reward, &cash_truck_reward_config);
				if (reward_config != NULL)
				{
					task_reward_data__init(&reward_data[resp.n_ongoing_list]);
					task_data[resp.n_ongoing_list].reward = &reward_data[resp.n_ongoing_list];
					reward_data[resp.n_ongoing_list].exp = reward_config->RewardExp1 * player->get_attr(PLAYER_ATTR_LEVEL);
					reward_data[resp.n_ongoing_list].coin = reward_config->RewardMoney1 * player->get_attr(PLAYER_ATTR_LEVEL) * (reward_config->Deposit + 10000) / 10000.0;
					reward_data[resp.n_ongoing_list].items = item_data_point[resp.n_ongoing_list];
					size_t &n_item = reward_data[resp.n_ongoing_list].n_items;
					for (uint32_t i = 0; i < reward_config->n_RewardItem1; ++n_item, ++i)
					{
						item_data_point[resp.n_ongoing_list][n_item] = &(item_data[resp.n_ongoing_list][n_item]);
						item_data__init(item_data_point[resp.n_ongoing_list][n_item]);
						item_data[resp.n_ongoing_list][n_item].id = reward_config->RewardItem1[i];
						item_data[resp.n_ongoing_list][n_item].num = reward_config->RewardNum1[i];
					}
					item_data_point[resp.n_ongoing_list][n_item] = &(item_data[resp.n_ongoing_list][n_item]);
					item_data__init(item_data_point[resp.n_ongoing_list][n_item]);
					item_data[resp.n_ongoing_list][n_item].id = 201010002;
					item_data[resp.n_ongoing_list][n_item].num = reward_config->RewardLv1 * player->get_attr(PLAYER_ATTR_LEVEL);
					++n_item;
				}
			}
		}
		else if (task_type == TT_GUILD_BUILD)
		{
			GangsBuildTaskTable *config = get_config_by_id(player->data->guild_task_config_id, &guild_build_task_config);
			if (config)
			{
				task_number_data__init(&number_data[resp.n_ongoing_list]);
				task_data[resp.n_ongoing_list].number = &number_data[resp.n_ongoing_list];
				number_data[resp.n_ongoing_list].current = player->data->guild_task_count + 1;
				number_data[resp.n_ongoing_list].total = config->Times;
			}
		}
		else if (task_type == TT_TRAVEL)
		{
			task_number_data__init(&number_data[resp.n_ongoing_list]);
			task_data[resp.n_ongoing_list].number = &number_data[resp.n_ongoing_list];
			number_data[resp.n_ongoing_list].current = player->data->travel_task_num + 1;
			number_data[resp.n_ongoing_list].total = sg_travel_task_amount;

			TravelTable *config = get_travel_config(player->get_level());
			if (config)
			{
				TaskRewardTable *reward_config = get_config_by_id(config->RewardGroup, &task_reward_config);
				if (reward_config)
				{
					task_reward_data__init(&reward_data[resp.n_ongoing_list]);
					task_data[resp.n_ongoing_list].reward = &reward_data[resp.n_ongoing_list];

					reward_data[resp.n_ongoing_list].exp = reward_config->RewardEXP;
					reward_data[resp.n_ongoing_list].coin = reward_config->RewardMoney;

					reward_data[resp.n_ongoing_list].items = item_data_point[resp.n_ongoing_list];
					reward_data[resp.n_ongoing_list].n_items = 0;
					size_t &n_item = reward_data[resp.n_ongoing_list].n_items;
					for (uint32_t i = 0; i < reward_config->n_RewardType && i < MAX_SHANGJIN_AWARD_NUM; ++i)
					{
						if (reward_config->RewardType[i] != 1)
						{
							continue;
						}

						item_data_point[resp.n_ongoing_list][n_item] = &(item_data[resp.n_ongoing_list][n_item]);
						item_data__init(item_data_point[resp.n_ongoing_list][n_item]);
						item_data[resp.n_ongoing_list][n_item].id = reward_config->RewardTarget[i];
						item_data[resp.n_ongoing_list][n_item].num = reward_config->RewardNum[i];
						n_item++;
					}
				}
			}
		}
		resp.n_ongoing_list++;
	}
	LOG_INFO("[%s:%d] player[%lu] name=%s,task_id=%lu ", __FUNCTION__, __LINE__, extern_data->player_id, player->get_name(), resp.n_ongoing_list);
	std::vector<uint32_t> finish_data;
	std::copy(player->task_finish_set.begin(), player->task_finish_set.end(), back_inserter(finish_data));
	resp.n_finish_list = finish_data.size();
	resp.finish_list = &finish_data[0];
	player->get_task_chapter_info(resp.chapterid, resp.chapterstate);

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_LIST_ANSWER, task_list_answer__pack, resp);

	return 0;
}

//接任务请求
static int handle_task_accept_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	TaskCommRequest *req = task_comm_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t task_id = req->task_id;
	task_comm_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		ret = player->accept_task(task_id);
		if (ret != 0)
		{
			LOG_ERR("[%s:%d] player[%lu], task_id:%u, ret:%d", __FUNCTION__, __LINE__, extern_data->player_id, task_id, ret);
			break;
		}
	} while(0);

	TaskCommAnswer resp;
	task_comm_answer__init(&resp);

	resp.result = ret;
	resp.task_id = task_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_ACCEPT_ANSWER, task_comm_answer__pack, resp);

	return 0;
}

//交任务请求
static int handle_task_submit_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	TaskCommRequest *req = task_comm_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t task_id = req->task_id;
	task_comm_request__free_unpacked(req, NULL);

	bool shangJin = false;
	int ret = 0;
	do
	{
		TaskTable *config = get_config_by_id(task_id, &task_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get task failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		if (config->Team == 1)
		{
			if (!player->m_team)
			{
				ret = ERROR_ID_TASK_TEAM_LEADER;
				break;
			}

			if (player->m_team->GetLeadId() != player->get_uuid())
			{
				ret = ERROR_ID_TASK_TEAM_LEADER;
				break;
			}
		}

		TaskInfo *task_info = player->get_task_info(task_id);
		if (!task_info)
		{
			ret = ERROR_ID_TASK_NOT_ACCEPT;
			LOG_ERR("[%s:%d] player[%lu] get task info failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		if (task_info->status != TASK_STATUS__ACHIEVED)
		{
			ret = ERROR_ID_TASK_NOT_ACHIEVE;
			LOG_ERR("[%s:%d] player[%lu] task not achieve, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}


		//获取地宫试炼任务相关参数表
		UndergroundTask* digong_config = NULL;
		if(config != NULL && config->TaskType == TT_DIGONG)
		{
			if(task_id != player->data->mi_jing_xiu_lian.task_id)
			{
				LOG_ERR("[%s:%d] 提交地宫修炼任务失败,任务id跟玩家身上的任务id不匹配,task_id[%u],player_task_id[%u]",
					__FUNCTION__, __LINE__, task_id, player->data->mi_jing_xiu_lian.task_id);
				ret = ERROR_ID_CONFIG;
				return ret;
			}
			digong_config = get_config_by_id(player->data->mi_jing_xiu_lian.digong_id, &mijing_xiulian_config);
			if(digong_config == NULL)
			{
				LOG_ERR("[%s:%d] 地宫修炼交任务,配置错误[%u]", __FUNCTION__, __LINE__, player->data->mi_jing_xiu_lian.digong_id);
				ret = ERROR_ID_NO_CONFIG;
				return ret;
			}
			if(player->data->mi_jing_xiu_lian.reward_beilv >= digong_config->n_ExpReward || player->data->mi_jing_xiu_lian.reward_beilv >= digong_config->n_MoneyReward || player->data->mi_jing_xiu_lian.reward_beilv >= digong_config->n_RewardGroup)
			{
				LOG_ERR("[%s:%d] 地宫修炼交任务,玩家身上星级下标超过最大值player_star_xiabiao[%u]", __FUNCTION__, __LINE__, player->data->mi_jing_xiu_lian.reward_beilv);
				ret = ERROR_ID_CONFIG;
				break;
			}
		
		}
		//这里获取等级系数(地宫修炼给奖励需要用到)
		uint32_t actor_id = 1041*100000 + player->get_level();
		ActorLevelTable *level_config = get_config_by_id(actor_id, &actor_level_config);
		if(level_config == NULL)
		{
			LOG_INFO("[%s:%d]提交任务失败，获取ActorLevelTable等级配置失败id[%u]", __FUNCTION__, __LINE__, actor_id);
			ret = ERROR_ID_CONFIG;
			break;
		}



		GangsBuildTaskTable *build_config = get_config_by_id(player->data->guild_task_config_id, &guild_build_task_config);
		if (config->TaskType == TT_GUILD_BUILD && !build_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get guild task config faild, task_id:%u, build_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, player->data->guild_task_config_id);
			break;
		}

		//检查背包空间
		std::map<uint32_t, uint32_t> item_list;
		std::map<uint32_t, uint32_t> mijing_drop;
		std::map<uint32_t, uint32_t> build_drop;
		player->get_task_event_item(task_id, TEC_SUBMIT, item_list);
		player->get_task_reward_item(task_id, item_list);
		//地宫修炼任务获取额外奖励
		if(digong_config != NULL)
		{
			get_drop_item(digong_config->RewardGroup[player->data->mi_jing_xiu_lian.reward_beilv], mijing_drop);
			if(player->data->mi_jing_xiu_lian.lun_num + 1 >=  sg_digong_xiulian_sum_lun_num)
			{
				get_drop_item(digong_config->DropID, mijing_drop);
			}
			for(std::map<uint32_t, uint32_t>::iterator itr = mijing_drop.begin(); itr != mijing_drop.end(); itr++)
			{
				item_list[itr->first] += itr->second;
			}
		}
		if (config->TaskType == TT_GUILD_BUILD)
		{
			get_drop_item(build_config->DropID, build_drop);
			for (std::map<uint32_t, uint32_t>::iterator iter = build_drop.begin(); iter != build_drop.end(); ++iter)
			{
				item_list[iter->first] += iter->second;
			}
		}
		else if (config->TaskType == TT_TRAVEL)
		{
			TravelTable *travel_table = get_travel_config(player->get_level());
			if (travel_table)
			{
				get_task_reward_item_from_config(travel_table->RewardGroup, item_list);
				if (player->data->travel_task_num  + 1 >= sg_travel_task_amount)
				{
					get_task_reward_item_from_config(travel_table->TimeRewardGroup, item_list);
				}
			}
		}
		if (item_list.size() > 0 && !player->check_can_add_item_list(item_list))
		{
			ret = 190300007;
			LOG_ERR("[%s:%d] player[%lu] bag not enough, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		TaskInfo tmp_info;
		memcpy(&tmp_info, task_info, sizeof(TaskInfo));
		memset(task_info, 0, sizeof(TaskInfo));
		tmp_info.status = TASK_STATUS__FINISH;
		player->get_task_event_partner(task_id, TEC_SUBMIT);
		player->task_update_notify(&tmp_info);
		player->touch_task_event(task_id, TEC_SUBMIT);
		player->give_task_reward(task_id);
		if(digong_config != NULL)
		{
			uint32_t max_lunshu = sg_digong_xiulian_sum_lun_num;
			//加经验和银两
			uint32_t exp_dengji_xishu = level_config->QueLvExp / 100;
			uint32_t coin_dengji_xishu = level_config->QueLvCoin / 100;
			uint32_t digong_get_exp = digong_config->ExpReward[player->data->mi_jing_xiu_lian.reward_beilv] * exp_dengji_xishu;
			uint32_t digong_get_coin = digong_config->MoneyReward[player->data->mi_jing_xiu_lian.reward_beilv] * coin_dengji_xishu;
			player->add_coin(digong_get_coin, MAGIC_TYPE_TASK_REWARD);
			player->add_exp(digong_get_exp, MAGIC_TYPE_TASK_REWARD);
			player->data->mi_jing_xiu_lian.lun_num += 1;
			player->check_activity_progress(AM_MIJINGXIULIAN, 0);
			if(player->data->mi_jing_xiu_lian.time_state == 0)
			{
				if(player->data->mi_jing_xiu_lian.lun_num >= max_lunshu)
				{
					player->data->mi_jing_xiu_lian.huan_num += 1;
					player->data->mi_jing_xiu_lian.lun_num = 0;
				}
				player->add_item_list(mijing_drop, MAGIC_TYPE_TASK_REWARD);
			}
			else 
			{
				player->data->mi_jing_xiu_lian.lun_num = 0;
				player->data->mi_jing_xiu_lian.huan_num = 0;
				player->data->mi_jing_xiu_lian.time_state = 0;
			}
			player->data->mi_jing_xiu_lian.digong_id = 0;
			player->data->mi_jing_xiu_lian.task_id = 0;
			player->data->mi_jing_xiu_lian.reward_beilv = 0;
			
			player->task_remove_notify(task_id);
			player->mijing_shilian_info_notify(2);
		}
		else 
		{
			player->add_finish_task(task_id);
		}
		player->delete_one_buff(114400019, true);	
		player->finish_jiu_gong_bagua_task(task_id);

		//章节、主线下一个任务
		if (config->TaskType == TT_TRUNK)
		{
			if (config->FollowTask > 0)
			{
				player->add_task(config->FollowTask, TASK_STATUS__NOT_ACCEPT_YET, true);
				if (config->ChapterId != get_task_chapter_id(config->FollowTask))
				{
					player->update_task_chapter_info();
					player->add_achievement_progress(ACType_TASK_CHAPTER, config->ChapterId, 0, 0, 1);
				}
			}
		}
		else if (config->TaskType == TT_BRANCH)
		{
			if (config->FollowTask > 0)
			{
				player->accept_task(config->FollowTask, true);
			}
		}
		else if (config->TaskType == TT_SHANGJIN)
		{
			shangJin = true;
		}
		else if (config->TaskType == TT_GUILD_BUILD)
		{
			player->add_item_list(build_drop, MAGIC_TYPE_TASK_REWARD);
			if (config->FollowTask > 0)
			{ //有后续任务，要等后续任务全部做完才算完成一环
				player->accept_task(config->FollowTask, false);

				GUILD_ACCEPT_TASK_ANSWER *resp = (GUILD_ACCEPT_TASK_ANSWER*)get_send_data();
				uint32_t data_len = sizeof(GUILD_ACCEPT_TASK_ANSWER);
				memset(resp, 0, data_len);
				resp->result = 0;
				resp->task_id = config->FollowTask;
				resp->is_next = true;
				fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_GUILD_ACCEPT_TASK_ANSWER, data_len, 0);
			}
			else
			{
				uint32_t *resp = (uint32_t*)get_send_data();
				uint32_t data_len = sizeof(uint32_t);
				memset(resp, 0, data_len);
				*resp = task_id;
				fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_GUILD_TASK_FINISH, data_len, 0);

				player->check_activity_progress(AM_GUILD_BUILD, 0);
			}
		}
		else if (config->TaskType == TT_TRAVEL)
		{
			TravelTable *travel_table = get_travel_config(player->get_level());
			if (travel_table)
			{
				player->give_task_reward_by_reward_id(travel_table->RewardGroup, MAGIC_TYPE_TASK_REWARD);
			}
			player->on_travel_task_finish(task_id);
		}
	} while(0);

	TaskCommAnswer resp;
	task_comm_answer__init(&resp);

	resp.result = ret;
	resp.task_id = task_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_SUBMIT_ANSWER, task_comm_answer__pack, resp);

	if (shangJin)
	{
		ShangjinManage::CompleteTask(player, task_id);
	}

	return 0;
}

//放弃任务请求
static int handle_task_abandon_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	TaskCommRequest *req = task_comm_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t task_id = req->task_id;
	task_comm_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		TaskTable *config = get_config_by_id(task_id, &task_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get task config failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		TaskInfo *task_info = player->get_task_info(task_id);
		if (!task_info)
		{
			ret = ERROR_ID_TASK_NOT_ACCEPT;
			LOG_ERR("[%s:%d] player[%lu] get task info failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		if (task_info->status != TASK_STATUS__UNACHIEVABLE)
		{
			ret = ERROR_ID_TASK_CAN_NOT_ABANDON;
			LOG_ERR("[%s:%d] player[%lu] task can not abandon, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		task_info->status = TASK_STATUS__NOT_ACCEPT_YET;
		player->task_update_notify(task_info);

		memset(task_info, 0, sizeof(TaskInfo));

		if (config->TaskType == TT_QUESTION) //todo 删除完成列表里面的答题任务 重置答题信息
		{
		}
	} while(0);

	TaskCommAnswer resp;
	task_comm_answer__init(&resp);

	resp.result = ret;
	resp.task_id = task_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_ABANDON_ANSWER, task_comm_answer__pack, resp);

	return 0;
}

//任务刷怪请求
static int handle_task_monster_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	TaskEventRequest *req = task_event_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t task_id = req->task_id;
	uint32_t event_id = req->event_id;
	task_event_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		TaskInfo *info = player->get_task_info(task_id);
		if (!info || info->status != TASK_STATUS__ACCEPTED)
		{
			ret = ERROR_ID_TASK_ID;
			LOG_ERR("[%s:%d] player[%lu] get task failed, task_id:%u, event_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, event_id);
			break;
		}

//		if (player->sight_space != NULL)
//		{
//			ret = ERROR_ID_PLAYER_IN_SIGHT_SPACE;
//			LOG_ERR("[%s:%d] player[%lu] in sight space, task_id:%u, event_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, event_id);
//			break;
//		}

		TaskTable *config = get_config_by_id(task_id, &task_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get task failed, task_id:%u, event_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, event_id);
			break;
		}

		bool event_fit = false;
		for (uint32_t i = 0; i < config->n_EventID; ++i)
		{
			if ((uint32_t)config->EventID[i] == event_id)
			{
				event_fit = true;
				break;
			}
		}

		if (!event_fit)
		{
			ret = ERROR_ID_TASK_EVENT_ID;
			LOG_ERR("[%s:%d] player[%lu] has not event, task_id:%u, event_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, event_id);
			break;
		}

		TaskEventTable *event_config = get_config_by_id(event_id, &task_event_config);
		if (!event_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get event config failed, task_id:%u, event_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, event_id);
			break;
		}

		uint32_t event_class = (uint32_t)event_config->EventClass;
		if (!(event_class == TEC_ACCEPT || event_class == TEC_TALK))
		{
			ret = ERROR_ID_TASK_EVENT_CLASS;
			LOG_ERR("[%s:%d] player[%lu] event class error, task_id:%u, event_id:%u, event_class:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, event_id, event_class);
			break;
		}

		if (event_config->EventType != TET_ADD_MONSTER)
		{
			ret = ERROR_ID_TASK_EVENT_TYPE;
			LOG_ERR("[%s:%d] player[%lu] event type error, task_id:%u, event_id:%u, event_type:%lu", __FUNCTION__, __LINE__, extern_data->player_id, task_id, event_id, event_config->EventType);
			break;
		}

		player->execute_task_event(event_id, event_class, false, task_id);
	} while(0);

	TaskCommAnswer resp;
	task_comm_answer__init(&resp);

	resp.result = ret;
	resp.task_id = task_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_MONSTER_ANSWER, task_comm_answer__pack, resp);

	return 0;
}

int check_task_complete_pos(TaskConditionTable *config, player_struct *player, uint32_t task_id, uint32_t cond_id)
{
	position *pos = player->get_pos();
	uint32_t target_scene = 0;
	if (get_transfer_point((uint32_t)config->Scene, &target_scene, NULL, NULL, NULL, NULL) != 0)
	{
		LOG_ERR("[%s:%d] player[%lu] get transfer point failed, task_id:%u, cond_id:%u, id:%u",
			__FUNCTION__, __LINE__, player->get_uuid(), task_id, cond_id, (uint32_t)config->Scene);
		return ERROR_ID_NO_CONFIG;
	}

	position target_pos;
	target_pos.pos_x = config->PointX;
	target_pos.pos_z = config->PointZ;

	if (player->data->scene_id != target_scene)
	{
		LOG_ERR("[%s:%d] player[%lu] scene:%u, posx:%f, posz:%f, targetscene:%u, targetx:%f, targetz:%f, task_id:%u, cond_id:%u radius:%lu",
			__FUNCTION__, __LINE__, player->get_uuid() , player->data->scene_id, pos->pos_x, pos->pos_z, target_scene,
			target_pos.pos_x, target_pos.pos_z, task_id, cond_id, config->Radius);
		return ERROR_ID_TASK_POSITION;
	}
	
	if (!check_circle_in_range(pos, &target_pos, config->Radius))
	{
		LOG_ERR("[%s:%d] player[%lu] scene:%u, posx:%f, posz:%f, targetscene:%u, targetx:%f, targetz:%f, task_id:%u, cond_id:%u radius:%lu",
			__FUNCTION__, __LINE__, player->get_uuid() , player->data->scene_id, pos->pos_x, pos->pos_z, target_scene,
			target_pos.pos_x, target_pos.pos_z, task_id, cond_id, config->Radius);
		EXTERN_DATA extern_data;
		extern_data.player_id = player->get_uuid();
		send_move_answer(MSG_ID_MOVE_STOP_ANSWER, -1, pos, &extern_data);		
		return ERROR_ID_TASK_POSITION;
	}
	return 0;
}
extern int direct_next_script(raid_struct *raid, struct raid_script_data *script_data);
//任务完成请求
static int handle_task_complete_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	TaskCompleteRequest *req = task_complete_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t task_id = req->task_id;
	uint32_t cond_id = req->condition_id;
	task_complete_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		TaskInfo *info = player->get_task_info(task_id);
		if (!info || info->status != TASK_STATUS__ACCEPTED)
		{
			ret = ERROR_ID_TASK_ID;
			LOG_ERR("[%s:%d] player[%lu] get task failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		TaskCountInfo *count_info = NULL;
		for (int i = 0; i < MAX_TASK_TARGET_NUM; ++i)
		{
			if (info->progress[i].id == cond_id)
			{
				count_info = &info->progress[i];
				break;
			}
		}

		if (!count_info)
		{
			ret = ERROR_ID_TASK_CONDITION_ID;
			LOG_ERR("[%s:%d] player[%lu] condition not found, task_id:%u, condition_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, cond_id);
			break;
		}

		TaskConditionTable *config = get_config_by_id(cond_id, &task_condition_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get config failed, task_id:%u, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, cond_id);
			break;
		}

		if (count_info->num >= (uint32_t)config->ConditionNum)
		{
			break;
		}

		uint32_t truck_type = 0;
		switch (config->ConditionType)
		{
		case TCT_EXPLORE:
		{
			ret = check_task_complete_pos(config, player,task_id, cond_id);
			if (ret != 0)
			{
				break;
			}
		}
		break;
		case TCT_TRUCK:
		{
			ret = check_task_complete_pos(config, player, task_id, cond_id);
			if (ret != 0)
			{
				break;
			}
			if (!player->data->truck.on_truck)
			{
				ret = ERROR_ID_TASK_CONDITION_ID;
				break;
			}
			player->go_down_cash_truck();
			BiaocheTable *table = get_config_by_id(player->data->truck.active_id, &cash_truck_config);
			if (table)
			{
				truck_type = table->Type;
			}
			cash_truck_struct *truck = cash_truck_manager::get_cash_truck_by_id(player->data->truck.truck_id);
			if (truck != NULL)
			{
				if (truck->sight_space != NULL)
				{
					truck->sight_space->broadcast_truck_delete(truck, true);							
					sight_space_manager::del_player_from_sight_space(truck->sight_space, player, true);
				}
				else
				{
					//player->data->truck.truck_id = 0;
					//player->data->truck.active_id = 0;			
					truck->scene->delete_cash_truck_from_scene(truck, true);
				}
				cash_truck_manager::delete_cash_truck(truck); 
				
			}
			player->data->truck.truck_id = 0;
			//player->data->truck.active_id = 0;
		}
		break;
			case TCT_TALK:
				{
					if (get_task_type(task_id) == TT_QUESTION)
					{
						//if (player->data->award_answer.question != 0)
						//{
						//	send_comm_answer(MSG_ID_GET_AWARD_QUESTION_FAILE_NOTIFY, 0, extern_data);
						//	break;
						//}
					}
				}
				break;
			case TCT_TRACK:
			case TCT_PUZZLE:
			case TCT_CLIENT_QUESTION:
				break;
			default:
				{
					ret = ERROR_ID_TASK_CONDITION_ID;
					LOG_ERR("[%s:%d] player[%lu] condition illegal, task_id:%u, condition_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id, cond_id);
					break;
				}
		}

		if (ret != 0)
		{
			break;
		}

		player->add_task_progress(config->ConditionType, config->ConditionTarget, 1, task_id, cond_id);
		if (config->ConditionType == TCT_TRUCK)
		{
			player->add_task_progress(TCT_TRUCK_NUM, 0, 1);
			player->add_achievement_progress(ACType_TRUCK, truck_type, 0, 0, 1);
		}
	} while(0);

	TaskCommAnswer resp;
	task_comm_answer__init(&resp);

	resp.result = ret;
	resp.task_id = task_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_COMPLETE_ANSWER, task_comm_answer__pack, resp);

	return 0;
}

//任务失败请求
static int handle_task_fail_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	TaskCommRequest *req = task_comm_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t task_id = req->task_id;
	task_comm_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		TaskInfo *info = player->get_task_info(task_id);
		if (!info || info->status != TASK_STATUS__ACCEPTED)
		{
			ret = ERROR_ID_TASK_ID;
			LOG_ERR("[%s:%d] player[%lu] get task failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		if (!player_struct::task_condition_can_fail(task_id))
		{
			ret = ERROR_ID_TASK_ID;
			LOG_ERR("[%s:%d] player[%lu] condition can't fail, task_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		player->set_task_fail(info);
	} while(0);

	TaskCommAnswer resp;
	task_comm_answer__init(&resp);

	resp.result = ret;
	resp.task_id = task_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_FAIL_ANSWER, task_comm_answer__pack, resp);

	return 0;
}

//任务章节奖励
static int handle_task_chapter_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	int ret = 0;
	do
	{
		uint32_t chapter_id = 0, chapter_state = 0;
		if (player->get_task_chapter_info(chapter_id, chapter_state) != 0)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get task chapter failed, task_chapter_reward:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->data->task_chapter_reward);
			break;
		}

		if (chapter_state == 0)
		{
			ret = ERROR_ID_TASK_CHAPTER_REWARD;
			LOG_ERR("[%s:%d] player[%lu] chapter reward not achieve, chapter_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, chapter_id);
			break;
		}

		TaskChapterTable *config = get_task_chapter_config(chapter_id);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get chapter config failed, chapter_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, chapter_id);
			break;
		}

		std::map<uint32_t, uint32_t> item_list;
		for (uint32_t i = 0; i < config->n_RewardID; ++i)
		{
			get_task_reward_item_from_config(config->RewardID[i], item_list);
		}

		if (!player->check_can_add_item_list(item_list))
		{
			ret = ERROR_ID_BAG_GRID_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] bag not enough, chapter_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, chapter_id);
			break;
		}

		for (uint32_t i = 0; i < config->n_RewardID; ++i)
		{
			player->give_task_reward_by_reward_id(config->RewardID[i], MAGIC_TYPE_TASK_CHAPTER);
		}
		
		player->data->task_chapter_reward = chapter_id;
		player->update_task_chapter_info();
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_CHAPTER_REWARD_ANSWER, comm_answer__pack, resp);

	return 0;
}

//任务进入位面请求
static int handle_task_enter_planes_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	TaskCommRequest *req = task_comm_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t task_id = req->task_id;
	task_comm_request__free_unpacked(req, NULL);

	int ret = 0;
	std::vector<uint32_t> client_events;
	std::vector<std::pair<uint32_t, uint32_t> > events;
	do
	{
		TaskInfo *info = player->get_task_info(task_id);
		if (!info || info->status != TASK_STATUS__ACCEPTED)
		{
			ret = ERROR_ID_TASK_ID;
			LOG_ERR("[%s:%d] player[%lu] get task failed, task_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		TaskTable *config = get_config_by_id(task_id, &task_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get task failed, task_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		for (uint32_t i = 0; i < config->n_EventID; ++i)
		{
			uint32_t event_id = config->EventID[i];
			TaskEventTable *event_config = get_config_by_id(event_id, &task_event_config);
			if (!event_config)
			{
				continue;
			}

			uint32_t event_class = (uint32_t)event_config->EventClass;
			if (!(event_class == TEC_ACCEPT || event_class == TEC_TALK))
			{
				continue;
			}

			switch (event_config->EventType)
			{
				case TET_ADD_MONSTER:
				case TET_PLANES_ADD_COLLECT:
					events.push_back(std::make_pair(event_id, event_class));
					break;
				case TET_PLANES_ADD_NPC:
					client_events.push_back(event_id);
					break;
			}
		}

		if (events.empty() && client_events.empty())
		{
			ret = ERROR_ID_TASK_EVENT_ID;
			LOG_ERR("[%s:%d] player[%lu] has not enter planes event, task_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);
			break;
		}

		for (std::vector<std::pair<uint32_t, uint32_t> >::iterator iter = events.begin(); iter != events.end(); ++iter)
		{
			player->execute_task_event(iter->first, iter->second, false, task_id);
		}

		if (events.empty() && !client_events.empty() && !player->sight_space)
		{
			player->sight_space = sight_space_manager::create_sight_space(player);
		}
	} while(0);

	TaskEnterPlanesAnswer resp;
	task_enter_planes_answer__init(&resp);

	resp.result = ret;
	resp.task_id = task_id;
	resp.events = &client_events[0];
	resp.n_events = client_events.size();
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TASK_ENTER_PLANES_ANSWER, task_enter_planes_answer__pack, resp);

	return 0;
}

int handle_create_team_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->m_team != NULL)
	{
		return 2;
	}

	TeamTarget *req = team_target__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	player_struct *tmpArr[MAX_TEAM_MEM] = {player};
	int ret = 0;
	if (Team::CreateTeam(tmpArr, 1, req->type, req->target) == NULL)
	{
		ret = 190500002;
	}
	send_comm_answer(MSG_ID_CREATE_TEAM_ANSWER, ret, extern_data);
	team_target__free_unpacked(req, NULL);


	return 0;
}

int handle_apply_team_request_impl(player_struct &player, uint64_t teamid)
{
	Team *pTeam = Team::GetTeam(teamid);
	if (pTeam == NULL)
	{
		return 190500025;
	}
	if (!pTeam->CheckLevel(player.get_attr(PLAYER_ATTR_LEVEL)))
	{
		return 190500026;
	}
	if (pTeam->GetLead() == NULL)
	{
		return 190500064;
	}
	if (pTeam->m_data->m_raid_uuid > 0)
	{
		raid_struct *raid = raid_manager::get_raid_by_uuid(pTeam->m_data->m_raid_uuid);
		if (!raid || !raid->check_can_add_team_mem(&player))
		{
			return 190500066;
		}
	}
	if (pTeam->IsFull())
	{
		return 190500024;
	}
	int ret = pTeam->AddApply(player);
	if (ret > 0)
	{
		return ret;
	}
	else if (ret == 0)
	{
		player_struct *lead = pTeam->GetLead();
		if (lead != NULL && !pTeam->IsAutoAccept())
		{
				TeamMemInfo notice;
				pTeam->PackMemberInfo(notice, player);

				EXTERN_DATA toExtern;
				toExtern.player_id = lead->get_uuid();
				fast_send_msg(&conn_node_gamesrv::connecter, &toExtern, MSG_ID_APPLY_TEAM_NOTIFY, team_mem_info__pack, notice);
		}
	}


	return 0;
}
int handle_apply_team_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team != NULL)
	{
		return (-2);
	}

	Teamid *req = teamid__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int ret = handle_apply_team_request_impl(*player, req->id);
	uint64_t teamId = req->id;
	teamid__free_unpacked(req, NULL);

	TeamApplyAnswer send;
	team_apply_answer__init(&send);
	send.errcode = ret;
	send.teamid = teamId;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_APPLY_TEAM_ANSWER, team_apply_answer__pack, send);
	//send_comm_answer(MSG_ID_APPLY_TEAM_ANSWER, ret, extern_data);
	if (ret == 0)
	{
		Team *pTeam = Team::GetTeam(teamId);
		if (pTeam == NULL)
		{
			return 190500025;
		}
		if (pTeam->IsAutoAccept())
		{
			pTeam->AddMember(*player);
		}
	}

	return 0;
}

int handle_chat_broadcast_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	//todo 扣喇叭
	std::map<uint64_t, struct ParameterTable *>::iterator it = parameter_config.find(161000004);
	if (it == parameter_config.end())
	{
		return -3;
	}
	Chat *req = chat__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	if (req->costtype == CHAT_BROADCAST_TYPE__ITEM)
	{
		if (player->del_item(201070023, it->second->parameter1[0], MAGIC_TYPE_BAG_USE) != 0)
			goto done;
	}
	else
	{
		if (player->sub_comm_gold(it->second->parameter1[1], MAGIC_TYPE_BAG_USE) != 0)
			goto done;
	}
	conn_node_gamesrv::send_to_all_player(MSG_ID_CHAT_BROADCAST_NOTIFY, req, (pack_func)chat__pack);
	player->add_achievement_progress(ACType_CHAT_BOARDCAST, 0, 0, 0, 1);

done:
	chat__free_unpacked(req, NULL);

	return 0;
}
int handle_private_chat_find_target_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	ReqFindTarget *req = req_find_target__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	if (req->name != NULL)
	{
		AnsFindTarget send;
		ans_find_target__init(&send);
		send.ret = 0;
		if (strcmp(req->name, player->get_name()) == 0)
		{
			send.ret = 190500128;
		}
		uint64_t pid = strtoull(req->name, NULL, 10);
		if (pid == player->get_uuid())
		{
			send.ret = 190500128;
		}
		if (send.ret != 0)
		{
			fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PRIVATE_CHAT_FIND_TARGET_ANSWER, ans_find_target__pack, send);
		}
		else
		{
			player_struct *target = player_manager::get_player_by_id(pid);
			if (target == NULL)
			{
				PROTO_FIND_PLAYER_REQ *todb = (PROTO_FIND_PLAYER_REQ *)&conn_node_base::global_send_buf[0];
				strncpy(todb->name, req->name, 33);
				todb->head.msg_id = ENDION_FUNC_2(SERVER_PROTO_FIND_PLAYER_REQUEST);
				todb->head.len = ENDION_FUNC_4(sizeof(PROTO_FIND_PLAYER_REQ));
				todb->org_msg = MSG_ID_PRIVATE_CHAT_FIND_TARGET_REQUEST;

				conn_node_base::add_extern_data(&todb->head, extern_data);
				if (conn_node_dbsrv::connecter->send_one_msg(&todb->head, 1) != (int)ENDION_FUNC_4(todb->head.len)) {
					LOG_ERR("%s %d: send to dbsrv err[%d]", __FUNCTION__, __LINE__, errno);
				}
			}
			else
			{
				send.pid = target->get_uuid();
				send.name = target->get_name();
				send.lv = target->get_attr(PLAYER_ATTR_LEVEL);
				fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PRIVATE_CHAT_FIND_TARGET_ANSWER, ans_find_target__pack, send);
				ChengJieTaskManage::AddRoleLevel(send.pid, send.lv, target->data->chengjie.rest);
			}

		}

	}
	req_find_target__free_unpacked(req, NULL);

	return 0;
}

static char *gen_rand_name_and_write_data(char *data, int len)
{
	static char rand_name[256];
	char path[256];
	char filename[128];
	if (len > 10000 || len <= 0)  //数据不应该这么大
	{
		LOG_ERR("%s: len too big %d", __FUNCTION__, len);
		return NULL;
	}
	sprintf(filename, "%lx_%x_%lx", time_helper::get_cached_time(), getpid(), random());
	sprintf(path, "%s/%s", sg_show_path, filename);
	sprintf(rand_name, "%s/%s", sg_show_url, filename);
	int fd = creat(path, S_IRUSR | S_IROTH);
	if (fd < 0)
	{
		LOG_ERR("%s: create %s fail, err = %d", __FUNCTION__, path, errno);
		return NULL;
	}
	int ret = write(fd, data, len);
	if (ret != len)
	{
		LOG_ERR("%s: write %s fail, ret %d, err = %d", __FUNCTION__, path, ret, errno);
	}
	close(fd);

	return rand_name;
}

static int handle_chat_show_equip_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	ChatShowEquipRequest *req = chat_show_equip_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t pos = req->pos;
	pos -= 1;
	chat_show_equip_request__free_unpacked(req, NULL);
	
	LOG_INFO("[%s:%d] player[%lu] pos[%u] ", __FUNCTION__, __LINE__, extern_data->player_id, pos);
	if (pos >= MAX_EQUIP_NUM)
	{
		return (-20);
	}
	
	EquipData equip_data;
//	EquipData* equip_data_point;
	EquipEnchantData enchant_data[MAX_EQUIP_ENCHANT_NUM];
	EquipEnchantData* enchant_data_point[MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData cur_attr[MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData* cur_attr_point[MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData rand_attr[MAX_EQUIP_ENCHANT_NUM][MAX_EQUIP_ENCHANT_RAND_NUM];
	CommonRandAttrData* rand_attr_point[MAX_EQUIP_ENCHANT_NUM][MAX_EQUIP_ENCHANT_RAND_NUM];
	
	EquipInfo &equip_info = player->data->equip_list[pos];
	// if (equip_info.stair == 0)
	// {
	// 	return (0);
	// }
//	equip_data_point = &equip_data;
	equip_data__init(&equip_data);

	equip_data.type = pos + 1;
	equip_data.stair = equip_info.stair;
	equip_data.starlv = equip_info.star_lv;
	equip_data.starexp = equip_info.star_exp;
	size_t enchant_num = 0;
	for (int i = 0; i < MAX_EQUIP_ENCHANT_NUM; ++i)
	{
		EquipEnchantInfo &enchant_info = equip_info.enchant[i];

		enchant_data_point[enchant_num] = &enchant_data[enchant_num];
		equip_enchant_data__init(&enchant_data[enchant_num]);
		enchant_data[enchant_num].index = i;

		cur_attr_point[enchant_num] = &cur_attr[enchant_num];
		common_rand_attr_data__init(&cur_attr[enchant_num]);
		cur_attr[enchant_num].pool = enchant_info.cur_attr.pool;
		cur_attr[enchant_num].id = enchant_info.cur_attr.id;
		cur_attr[enchant_num].val = enchant_info.cur_attr.val;
		enchant_data[enchant_num].curattr = cur_attr_point[enchant_num];

		size_t rand_num = 0;
		if (enchant_info.rand_attr[0].id > 0)
		{
			for (int j = 0; j < MAX_EQUIP_ENCHANT_RAND_NUM; ++j)
			{
				rand_attr_point[enchant_num][rand_num] = &rand_attr[enchant_num][rand_num];
				common_rand_attr_data__init(&rand_attr[enchant_num][rand_num]);
				rand_attr[enchant_num][rand_num].pool = enchant_info.rand_attr[j].pool;
				rand_attr[enchant_num][rand_num].id = enchant_info.rand_attr[j].id;
				rand_attr[enchant_num][rand_num].val = enchant_info.rand_attr[j].val;
				rand_num++;
			}
		}
		enchant_data[enchant_num].randattr = rand_attr_point[enchant_num];
		enchant_data[enchant_num].n_randattr = rand_num;

		enchant_num++;
	}
	equip_data.n_enchant = enchant_num;
	equip_data.enchant = enchant_data_point;

	equip_data.inlay = &equip_info.inlay[0];
	equip_data.n_inlay = MAX_EQUIP_INLAY_NUM;

	
	int size = equip_data__pack(&equip_data, conn_node_base::global_send_buf);
	char *url = gen_rand_name_and_write_data((char *)conn_node_base::global_send_buf, size);
	if (url)
	{
		ChatShowEquipAnswer ans;
		chat_show_equip_answer__init(&ans);
		ans.pos = pos;
		ans.url = url;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHAT_SHOW_EQUIP_ANSWER, chat_show_equip_answer__pack, ans);		
	}
	return (0);
}
static int handle_chat_show_partner_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	ChatShowPartnerRequest *req = chat_show_partner_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t uuid = req->uuid;
	chat_show_partner_request__free_unpacked(req, NULL);
	LOG_INFO("[%s:%d] player[%lu] uuid[%lu]", __FUNCTION__, __LINE__, extern_data->player_id, uuid);

	PartnerData  partner_data;
	PartnerAttr partner_cur_attr;
	PartnerAttr partner_cur_flash;
//	PartnerData* partner_point;
	AttrData  attr_data[MAX_PARTNER_ATTR];
	AttrData* attr_point[MAX_PARTNER_ATTR];
	PartnerSkillData  skill_data[MAX_PARTNER_SKILL_NUM];
	PartnerSkillData* skill_point[MAX_PARTNER_SKILL_NUM];
	PartnerSkillData  partner_skill_data_flash[MAX_PARTNER_SKILL_NUM];
	PartnerSkillData* partner_skill_point_flash[MAX_PARTNER_SKILL_NUM];
	PartnerCurFabaoInfo partner_cur_fabao;
	AttrData partner_fabao_minor_attr[MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData* partner_fabao_minor_attr_point[MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData partner_fabao_main_attr;

	partner_struct *partner = player->get_partner_by_uuid(uuid);	
	if (partner == NULL)
	{
		LOG_ERR("%s: player[%lu] can not find partner[%lu]", __FUNCTION__, player->get_uuid(), uuid);
		return (-20);
	}

//	partner_point = &partner_data;
	partner_data__init(&partner_data);
	partner_data.uuid = partner->data->uuid;
	partner_data.partnerid = partner->data->partner_id;
	partner_data.relivetime = partner->data->relive_time;
	partner_data.name = partner->data->name;
	partner_data.rename_free = partner->data->partner_rename_free;
		
	uint32_t attr_num = 0;
	for (int i = 1; i < MAX_PARTNER_ATTR; ++i)
	{
		attr_point[attr_num] = &attr_data[attr_num];
		attr_data__init(&attr_data[attr_num]);
		attr_data[attr_num].id = i;
		attr_data[attr_num].val = partner->data->attrData[i];
		attr_num++;
	}
	partner_data.attrs = attr_point;
	partner_data.n_attrs = attr_num;

	uint32_t skill_num = 0;
	if (partner->data->attr_cur.base_attr_up[0] != 0)
	{
		partner_data.cur_attr = &partner_cur_attr;
		partner_attr__init(&partner_cur_attr);
		for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
		{
			skill_point[skill_num] = &skill_data[skill_num];
			partner_skill_data__init(&skill_data[skill_num]);
			skill_data[skill_num].id = partner->data->attr_cur.skill_list[i].skill_id;
			skill_data[skill_num].lv = partner->data->attr_cur.skill_list[i].lv;
			skill_data[skill_num].lock = partner->data->attr_cur.skill_list[i].lock;
			skill_data[skill_num].exp = partner->data->attr_cur.skill_list[i].exp;
			skill_num++;
		}
		partner_cur_attr.skills = skill_point;
		partner_cur_attr.n_skills = skill_num;
		partner_cur_attr.base_attr_id = (uint32_t *)&base_attr_id[0];
		partner_cur_attr.n_base_attr_id = MAX_PARTNER_BASE_ATTR;
		partner_cur_attr.base_attr_cur = partner->data->attr_cur.base_attr_vaual;
		partner_cur_attr.n_base_attr_cur = MAX_PARTNER_BASE_ATTR;
		partner_cur_attr.base_attr_up = partner->data->attr_cur.base_attr_up;
		partner_cur_attr.n_base_attr_up = MAX_PARTNER_BASE_ATTR;
		partner_cur_attr.base_attr_up = partner->data->attr_cur.base_attr_up;
		partner_cur_attr.n_base_attr_up = MAX_PARTNER_BASE_ATTR;
		partner_cur_attr.detail_attr_id = partner->data->attr_cur.detail_attr_id;
		partner_cur_attr.n_detail_attr_id = partner->data->attr_cur.n_detail_attr;
		partner_cur_attr.detail_attr_cur = partner->data->attr_cur.detail_attr_vaual;
		partner_cur_attr.n_detail_attr_cur = partner->data->attr_cur.n_detail_attr;
		partner_cur_attr.type = partner->data->attr_cur.type;
	}
		
	if (partner->data->attr_flash.base_attr_up[0] != 0)
	{
		partner_data.flash_attr = &partner_cur_flash;
		partner_attr__init(&partner_cur_flash);
		skill_num = 0;
		for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
		{
			partner_skill_point_flash[skill_num] = &partner_skill_data_flash[skill_num];
			partner_skill_data__init(&partner_skill_data_flash[skill_num]);
			partner_skill_data_flash[skill_num].id = partner->data->attr_flash.skill_list[i].skill_id;
			partner_skill_data_flash[skill_num].lv = partner->data->attr_flash.skill_list[i].lv;
			partner_skill_data_flash[skill_num].exp = partner->data->attr_flash.skill_list[i].exp;
			skill_num++;
		}
		partner_cur_flash.skills = partner_skill_point_flash;
		partner_cur_flash.n_skills = skill_num;
		partner_cur_flash.base_attr_id = (uint32_t *)base_attr_id;
		partner_cur_flash.n_base_attr_id = MAX_PARTNER_BASE_ATTR;
		partner_cur_flash.base_attr_cur = partner->data->attr_flash.base_attr_vaual;
		partner_cur_flash.n_base_attr_cur = MAX_PARTNER_BASE_ATTR;
		partner_cur_flash.base_attr_up = partner->data->attr_flash.base_attr_up;
		partner_cur_flash.n_base_attr_up = MAX_PARTNER_BASE_ATTR;
		partner_cur_flash.base_attr_up = partner->data->attr_flash.base_attr_up;
		partner_cur_flash.n_base_attr_up = MAX_PARTNER_BASE_ATTR;
		partner_cur_flash.detail_attr_id = partner->data->attr_flash.detail_attr_id;
		partner_cur_flash.n_detail_attr_id = partner->data->attr_flash.n_detail_attr;
		partner_cur_flash.detail_attr_cur = partner->data->attr_flash.detail_attr_vaual;
		partner_cur_flash.n_detail_attr_cur = partner->data->attr_flash.n_detail_attr;
		partner_cur_flash.type = partner->data->attr_flash.type;
		partner_data.power = partner->data->attr_flash.power_refresh;
	}
		
	partner_data.god_id = partner->data->god_id;
	partner_data.n_god_id = partner->data->n_god;
	partner_data.god_level = partner->data->god_level;
	partner_data.n_god_level = partner->data->n_god;

	if(partner->data->cur_fabao.fabao_id != 0)
	{
		partner_data.cur_fabao = &partner_cur_fabao;
		partner_cur_fabao_info__init(&partner_cur_fabao);
		partner_cur_fabao.fabao_id = partner->data->cur_fabao.fabao_id;
		partner_cur_fabao.main_attr = &partner_fabao_main_attr;
		attr_data__init(&partner_fabao_main_attr);
		partner_fabao_main_attr.id = partner->data->cur_fabao.main_attr.id;
		partner_fabao_main_attr.val = partner->data->cur_fabao.main_attr.val;
		uint32_t attr_num = 0;
		for (int j = 0; j < MAX_HUOBAN_FABAO_MINOR_ATTR_NUM; ++j)
		{

			partner_fabao_minor_attr_point[attr_num] = &partner_fabao_minor_attr[attr_num];
			attr_data__init(&partner_fabao_minor_attr[attr_num]);
			partner_fabao_minor_attr[attr_num].id = partner->data->cur_fabao.minor_attr[j].id;
			partner_fabao_minor_attr[attr_num].val = partner->data->cur_fabao.minor_attr[j].val;
			attr_num++;
		}
		partner_cur_fabao.minor_attr = partner_fabao_minor_attr_point;
		partner_cur_fabao.n_minor_attr = attr_num;
	}

	int size = partner_data__pack(&partner_data, conn_node_base::global_send_buf);
	char *url = gen_rand_name_and_write_data((char *)conn_node_base::global_send_buf, size);
	if (url)
	{
		ChatShowPartnerAnswer ans;
		chat_show_partner_answer__init(&ans);
		ans.uuid = uuid;
		ans.url = url;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHAT_SHOW_PARTNER_ANSWER, chat_show_partner_answer__pack, ans);		
	}

	return (0);
}

int handle_lately_chat_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	ReqLatelyChat *req = req_lately_chat__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	AnsLatelyChat res;
	ans_lately_chat__init(&res);
	static const int MAX_LATELY_CHAT = 10;
	LatelyChat arr[MAX_LATELY_CHAT];
	LatelyChat *arrPoint[MAX_LATELY_CHAT];
	uint32_t i = 0;
	bool toFr = false;
	for (; i < req->n_playerid && i < MAX_LATELY_CHAT; ++i)
	{
		lately_chat__init(&arr[i]);
		arrPoint[i] = &arr[i];
		arr[i].playerid = req->playerid[i];
		player_struct *tmp = player_manager::get_player_by_id(req->playerid[i]);
		if (tmp != NULL)
		{
			arr[i].online = true;
			arr[i].lv = tmp->get_attr(PLAYER_ATTR_LEVEL);
			arr[i].job = tmp->get_attr(PLAYER_ATTR_JOB);
			arr[i].name = tmp->get_name();
			arr[i].head = tmp->get_attr(PLAYER_ATTR_HEAD);
			arr[i].zhenying = tmp->get_attr(PLAYER_ATTR_ZHENYING);
		}
		else
		{
			arr[i].name = (char *)g_tmp_name;
			toFr = true;
		}
	}
	res.n_player = i;
	res.player = arrPoint;
	if (toFr)
	{
		conn_node_gamesrv::send_to_friend(extern_data, MSG_ID_LATELY_CHAT_ANSWER, &res, (pack_func)ans_lately_chat__pack);;
	} 
	else
	{
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LATELY_CHAT_ANSWER, ans_lately_chat__pack, res);
	}
	
	req_lately_chat__free_unpacked(req, NULL);
	return 0;
}

int handle_quit_team_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	Team *pTeam = player->m_team;
	if (pTeam == NULL)
	{
		return 2;
	}
	if (player->is_in_pvp_raid())
	{
		return 3;
	}

	pTeam->RemoveMember(*player);
	if (pTeam->GetMemberSize() == 0)
	{
		Team::DestroyTeam(pTeam);
	}
	return 0;
}

int handle_team_change_lead_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}
	if (player->is_in_pvp_raid())
	{
		return 5;
	}
	TeamPlayerid *req = team_playerid__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	player_struct *otherPlayer = player_manager::get_player_by_id(req->id);
	team_playerid__free_unpacked(req, NULL);
	if (otherPlayer == NULL)
	{
		return 3;
	}
	if (player->get_uuid() != player->m_team->GetLeadId())
	{
		return 4;
	}

	if (player->m_team->ChangeLeader(*otherPlayer))
	{
		TeamPlayerid send;
		team_playerid__init(&send);
		send.id = otherPlayer->get_uuid();
		player->m_team->BroadcastToTeam(MSG_ID_TEAM_CHANGE_LEAD_NOTIFY, &send, (pack_func)team_playerid__pack);
		otherPlayer->broadcast_to_sight(MSG_ID_IS_TEAM_LEAD_NOTIFY, &send, (pack_func)team_playerid__pack, false);
		send.id = player->get_uuid();
		player->broadcast_to_sight(MSG_ID_NOT_TEAM_LEAD_NOTIFY, &send, (pack_func)team_playerid__pack, false);
		player->m_team->SendApplyList(*otherPlayer);
	}

	return 0;
}

int handle_team_list_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team != NULL)
	{
		return 2;
	}
	TeamTarget *req = team_target__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	if (req->type == TEAM_TARGET_TYPE__fb)
	{
		Team::GetTargetTeamList(req->target, *player);
	}
	else
	{
		Team::GetNearTeamList(*player);
	}
	team_target__free_unpacked(req, NULL);

	return 0;
}

int handle_be_team_lead_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 1;
	}
	if (player->m_team->GetLeadId() != player->get_uuid())
	{
		return 2;
	}

	TeamBeLead *req = team_be_lead__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	player_struct *otherPlayer = player_manager::get_player_by_id(req->playerid);
	int chose = req->chose;

	BeLeadAnswer send;
	be_lead_answer__init(&send);
	int ret = 0;
	if (otherPlayer == NULL)
	{
		ret = 190500023;

	}
	else
	{
		if (chose == TEAM_CHOSE__YES)
		{
			if (player->m_team->ChangeLeader(*otherPlayer))
			{
				TeamPlayerid send;
				team_playerid__init(&send);
				send.id = otherPlayer->get_uuid();
				player->m_team->BroadcastToTeam(MSG_ID_TEAM_CHANGE_LEAD_NOTIFY, &send, (pack_func)team_playerid__pack);
				otherPlayer->broadcast_to_sight(MSG_ID_IS_TEAM_LEAD_NOTIFY, &send, (pack_func)team_playerid__pack, false);
				send.id = player->get_uuid();
				player->broadcast_to_sight(MSG_ID_NOT_TEAM_LEAD_NOTIFY, &send, (pack_func)team_playerid__pack, false);
				player->m_team->SendApplyList(*otherPlayer);
			}
			else
			{
				ret = 190500018;
			}
		}
		else
		{
			TeamMemInfo mem;
			player->m_team->PackMemberInfo(mem, *player);
			EXTERN_DATA ext_data;
			ext_data.player_id = otherPlayer->get_uuid();
			fast_send_msg(&conn_node_gamesrv::connecter, &ext_data, MSG_ID_TEAM_REFUCE_BE_LEAD_NOTIFY, team_mem_info__pack, mem);
		}
	}

	send.ret = ret;
	send.name = req->name;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TEAM_HANDLE_BE_LEAD_ANSWER, be_lead_answer__pack, send);

	team_be_lead__free_unpacked(req, NULL);
	return 0;
}

int handle_team_be_lead_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}
	if (player->is_in_pvp_raid())
	{
		return 4;
	}

	int cd = player->m_team->CkeckApplyCd(extern_data->player_id);
	TeamNotifyCd note;
		team_notify_cd__init(&note);
		note.cd = cd;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TEAM_BE_LEAD_ANSWER, team_notify_cd__pack, note);
	if (cd != 0)
	{
		return 3;
	}

	player_struct *lead = player->m_team->GetLead();
	if (lead != NULL)
	{
		TeamPlayerid send;
		team_playerid__init(&send);
		send.id = player->get_uuid();
		EXTERN_DATA ext_data;
		ext_data.player_id = lead->get_uuid();
		fast_send_msg(&conn_node_gamesrv::connecter, &ext_data, MSG_ID_TEAM_BE_LEAD_NOTIFY, team_playerid__pack, send);
	}

	return 0;
}

int handle_kick_team_mem_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}
	TeamPlayerid *req = team_playerid__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	player_struct *otherPlayer = player_manager::get_player_by_id(req->id);
	uint64_t otherid = req->id;
	team_playerid__free_unpacked(req, NULL);

	if (player->is_in_pvp_raid())
	{
		return 5;
	}
	if (player->get_uuid() != player->m_team->GetLeadId())
	{
		return 4;
	}
	if (otherPlayer == NULL)
	{
		player->m_team->RemoveMember(otherid, true);
	}
	else
	{
		player->m_team->RemoveMember(*otherPlayer, true);
	}

	return 0;
}

int handle_team_invite_request_impl(player_struct *player, uint64_t id)
{
	player_struct *otherPlayer = player_manager::get_player_by_id(id);

	if (otherPlayer == NULL)
	{
		return 190500034;
	}
	if (otherPlayer->m_team != NULL)
	{
		return 190500033;
	}
	if (!scene_can_make_team(otherPlayer->data->scene_id))
	{
		return 190500065;
	}
	if (otherPlayer->data->team_invite_switch != 0)
	{
		return 190500159;
	}
	if (player->m_team != NULL && !player->m_team->CheckLevel(otherPlayer->get_attr(PLAYER_ATTR_LEVEL)))
	{
		return 190500086;
	}
	if (otherPlayer->m_inviter.find(player->get_uuid()) != otherPlayer->m_inviter.end())
	{
		return 0;
	}
	TeamInvite send;
	team_invite__init(&send);
	TeamMemInfo mem;
	player->m_team->PackMemberInfo(mem, *player);
	if (player->m_team != NULL)
	{
		send.teamid = player->m_team->GetId();
		send.target = player->m_team->GetTarget();
	}
	send.lead = &mem;

	EXTERN_DATA ext_data;
	ext_data.player_id = otherPlayer->get_uuid();
	fast_send_msg(&conn_node_gamesrv::connecter, &ext_data, MSG_ID_INVITE_TEAM_NOTIFY, team_invite__pack, send);
	//otherPlayer->m_inviter.insert(otherPlayer->get_uuid());

	return 0;
}
int handle_team_invite_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (!scene_can_make_team(player->data->scene_id))
	{
		return 3;
	}

	//if (player->m_team == NULL)
	//{
	//	int ret = Team::CreateTeam(*player);
	//	if (ret != 0)
	//	{
	//		send_comm_answer(MSG_ID_CREATE_TEAM_ANSWER, ret, extern_data);
	//		return 2;
	//	}

	//}
	TeamPlayerid *req = team_playerid__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int ret = handle_team_invite_request_impl(player, req->id);
	team_playerid__free_unpacked(req, NULL);

	send_comm_answer(MSG_ID_INVITE_TEAM_ANSWER, ret, extern_data);

	return 0;
}

int handle_team_invite_handle_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	player->m_inviter.erase(player->get_uuid());
	HandleTeamInvite *req = handle_team_invite__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	//Team *pTeam = Team::GetTeam(req->teamid);
	player_struct *otherPlayer = player_manager::get_player_by_id(req->playerid);
	int accept = req->accept;
	handle_team_invite__free_unpacked(req, NULL);
	//if (pTeam == NULL)
	//{
	//	return 2;
	//}

	int ret = 0;
	if (otherPlayer == NULL)
	{
		ret = 190500084;
	}
	else if (!scene_can_make_team(player->data->scene_id))
	{
		ret = 190500002;
	}
	else if (player->m_team != NULL)
	{
		ret = 190500469;
	}
	if (ret != 0)
	{
		send_comm_answer(MSG_ID_CREATE_TEAM_ANSWER, ret, extern_data);
		return -2;
	}

	EXTERN_DATA ext_data;
	ext_data.player_id = otherPlayer->get_uuid();
	if (accept == TEAM_CHOSE__YES)
	{
		if (otherPlayer->m_team == NULL)
		{
			player_struct *tmpArr[MAX_TEAM_MEM] = {otherPlayer, player};
			if (Team::CreateTeam(tmpArr, 2) == NULL)
			{
				send_comm_answer(MSG_ID_CREATE_TEAM_ANSWER, 190500002, &ext_data);
				send_comm_answer(MSG_ID_CREATE_TEAM_ANSWER, 190500002, extern_data);
			}
		}
		else //(otherPlayer->m_team != NULL)
		{
			otherPlayer->m_team->AddMember(*player);
		}
	}
	else //if (otherPlayer != NULL)
	{
		TeamMemInfo mem;
		Team::PackMemberInfo(mem, *player);
		fast_send_msg(&conn_node_gamesrv::connecter, &ext_data, MSG_ID_REFUCE_INVITE_TEAM_NOTIFY, team_mem_info__pack, mem);
	}


	return 0;
}

int handle_team_setlimit_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}

	if (player->get_uuid() != player->m_team->GetLeadId())
	{
		return 4;
	}

	TeamLimited *req = team_limited__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	player->m_team->SetLimited(*req, *player);
	player->m_team->BroadcastToTeam(MSG_ID_TEAM_LIMITED_NOTIFY, req, (pack_func)team_limited__pack);
	team_limited__free_unpacked(req, NULL);

	return 0;
}

int handle_handle_apply_team_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}
	if (player->get_uuid() != player->m_team->GetLeadId())
	{
		return 3;
	}
	HandleTeamApply *req = handle_team_apply__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	player->m_team->RemoveApply(req->id);

	int ret = 0;
	player_struct *applyer = player_manager::get_player_by_id(req->id);
	if (req->chose == TEAM_CHOSE__YES)
	{
		if (applyer == NULL)
		{
			ret = 190500084;
			goto done;
		}
		if (applyer->m_team != NULL)
		{
			ret = 190500085;
			goto done;
		}
		if (!player->m_team->CheckLevel(applyer->get_attr(PLAYER_ATTR_LEVEL)))
		{
			ret = 190500086;
			goto done;
		}
		player->m_team->AddMember(*applyer);
	}
	else
	{
		if (applyer != NULL)
		{
			RefuceApplyTeam note;
			refuce_apply_team__init(&note);
			note.name = player->data->name;
			EXTERN_DATA ext;
			ext.player_id = applyer->get_uuid();
			fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_REFUCE_APPLY_TEAM_NOTIFY, refuce_apply_team__pack, note);
		}
	}

done:
	send_comm_answer(MSG_ID_HANDLE_APPLY_TEAM_ANSWER, ret, extern_data);

	handle_team_apply__free_unpacked(req, NULL);

	//player->m_team->SendApplyList(*player);

	return 0;
}

static int handle_team_match_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team != NULL)
	{
		return 2;
	}
	TeamTarget *req = team_target__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	TeamMatch::AddRole(*player, req->target);

	MatchAnser resp;
	match_anser__init(&resp);
	resp.target = req->target;
	resp.ret = 0;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TEAM_MATCH_ANSWER, match_anser__pack, resp);

	team_target__free_unpacked(req, NULL);

	return 0;
}

static int handle_team_lead_pos_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}

	player_struct *lead = player->m_team->GetLead();
	if (lead == NULL) //不在线
	{
		return 3;
	}

	SyncPlayerPosNotify resp;
	sync_player_pos_notify__init(&resp);
	resp.player_id = lead->get_uuid();
	resp.scene_id = lead->data->scene_id;
	resp.pos_x = lead->get_pos()->pos_x;
	resp.pos_z = lead->get_pos()->pos_z;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TEAM_LEAD_POS_ANSWER, sync_player_pos_notify__pack, resp);

	return 0;
}

static int handle_team_summon_mem_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}
	if (player->m_team->GetLeadId() != player->get_uuid())
	{
		return 3;
	}
	player->m_team->SummonMem();
	return 0;
}

static int handle_team_set_follow_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}

	Follow *req = follow__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	player->m_team->SetFollow(*player, req->state);
	follow__free_unpacked(req, NULL);
	return 0;
}

static int handle_cancel_team_match_request(player_struct *player, EXTERN_DATA *extern_data)
{
	uint64_t target = TeamMatch::DelRole(player->get_uuid());

	MatchAnser resp;
	match_anser__init(&resp);
	resp.target = target;
	resp.ret = 0;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TEAM_CANCEL_MATCH_ANSWER, match_anser__pack, resp);
	return 0;
}

static void on_login_send_all_horse(player_struct *player, EXTERN_DATA *extern_data)
{/*
	std::map<uint64_t, struct SpiritTable*>::iterator it = spirit_config.begin();
	for (int i = 0; i < MAX_HORSE_ATTR_NUM; ++i)
	{
		player->data->horse_attr.attr[i] = it->second->SpiritAttribute[i];
	}
	if (player->data->horse_attr.step == 0)
	{
		//player->add_horse(DEFAULT_HORSE, 0);
		player->data->horse_attr.step = 1;
		player->data->horse_attr.soul = 1;
		for (int i = 0; i < MAX_HORSE_ATTR_NUM; ++i)
		{
			player->data->horse_attr.attr_exp[i] = 0;
		}
		memset(player->data->horse_attr.soul_exp, 0, sizeof(player->data->horse_attr.soul_exp));
		player->data->horse_attr.cur_soul = 1;
		player->data->horse_attr.soul_full = false;
		player->data->horse_attr.fly = 1;
	}*/
	HorseData horse[MAX_HORSE_NUM];
	HorseData *horsePoint[MAX_HORSE_NUM];
	HorseList sendHorse;
	horse_list__init(&sendHorse);
	uint32_t i = 0;
	uint64_t now = time_helper::get_cached_time() / 1000;
	for (uint32_t j = 0; j < player->data->n_horse; ++j)
	{
		horse_data__init(&horse[i]);
		horse[i].id = player->data->horse[i].id;
		horse[i].isnew = player->data->horse[i].isNew;
		horse[i].step = player->data->horse[i].step;
		horse[i].star = player->data->horse[i].star;

		if (player->data->horse[i].timeout != 0)
		{
			if (player->data->horse[i].timeout <= (time_t)now)
			{
				horse[i].isexpire = true;
				//continue;
			}
			else
			{
				horse[i].cd = player->data->horse[i].timeout - now;
				horse[i].isexpire = false;
			}
		}
		else
		{
			horse[i].isexpire = false;
			horse[i].cd = 0;
		}
		if (horse[i].id == player->get_attr(PLAYER_ATTR_CUR_HORSE))
		{
			horse[i].is_current = true;
		}
		else
		{
			horse[i].is_current = false;
		}
		horsePoint[i] = &horse[i];
		++i;
	}

	HorseCommonAttr sendAttr;
	horse_common_attr__init(&sendAttr);
	sendAttr.step = player->data->horse_attr.step;
	//sendAttr.cur_soul = player->data->horse_attr.cur_soul;
	//sendAttr.soul_level = player->data->horse_attr.soul;
	//sendAttr.n_soul_level = MAX_HORSE_SOUL;
	//sendAttr.soul_num = &(player->data->horse_attr.soul_exp[1]);
	//sendAttr.n_soul_num = MAX_HORSE_SOUL;
	sendAttr.attr = player->data->horse_attr.attr;
	sendAttr.attr_level = player->data->horse_attr.attr_exp;
	sendAttr.n_attr = sendAttr.n_attr_level = MAX_HORSE_ATTR_NUM;
	//player->calc_horse_attr();
	//player->calculate_attribute(true);
	sendAttr.power = player->data->horse_attr.power;
	//sendAttr.soul_full = player->data->horse_attr.soul_full;
	sendAttr.soul_step = player->data->horse_attr.soul_step;
	sendAttr.soul_star = player->data->horse_attr.soul_star;

	sendHorse.n_data = i;
	sendHorse.data = horsePoint;
	sendHorse.attr = &sendAttr;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HORSE_LIST_NOTIFY, horse_list__pack, sendHorse);
}
void pack_fashion_info(FashionData &data, player_struct *player, int ind)
{
	fashion_data__init(&data);
	data.id = player->data->fashion[ind].id;
	data.color = player->data->fashion[ind].color;
	data.isnew = player->data->fashion[ind].isNew;
	data.colordown = player->data->fashion[ind].colordown;
	data.ison = false;

	if (player->data->fashion[ind].timeout != 0)
	{
		if (player->data->fashion[ind].timeout <= (time_t)time_helper::get_cached_time() / 1000)
		{
			data.isexpire = true;
		}
		else
		{
			data.cd = player->data->fashion[ind].timeout - time_helper::get_cached_time() / 1000;
			data.isexpire = false;
		}
	}
	else
	{
		data.isexpire = false;
		data.cd = 0;
	}
	std::map<uint64_t, struct ActorFashionTable *>::iterator itFashion = fashion_config.find(data.id);
	if (itFashion != fashion_config.end())
	{
		if (itFashion->second->Type == FASHION_TYPE_WEAPON)
		{
			if (data.id != player->get_attr(PLAYER_ATTR_WEAPON))
			{
				data.ison = false;
			}
			else
			{
				data.ison = true;
			}
		}
		else if (itFashion->second->Type == FASHION_TYPE_HAT)
		{
			if (data.id != player->get_attr(PLAYER_ATTR_HAT))
			{
				data.ison = false;
			}
			else
			{
				data.ison = true;
			}
		}
		else if (itFashion->second->Type == FASHION_TYPE_CLOTHES)
		{
			if (data.id != player->get_attr(PLAYER_ATTR_CLOTHES))
			{
				data.ison = false;
			}
			else
			{
				data.ison = true;
			}
		}
	}
}
static void on_login_send_all_fashion(player_struct *player, EXTERN_DATA *extern_data)
{
	if (player->data->n_fashion == 0)
	{
		std::map<uint64_t, struct ActorFashionTable *>::iterator itFashion = fashion_config.begin();
		for (; itFashion != fashion_config.end(); ++itFashion)
		{
			if (itFashion->second->Occupation == player->get_job()
				&& itFashion->second->Lock == 2 && itFashion->second->ListAcc == 1)
			{
				player->add_fashion(itFashion->first, itFashion->second->Colour, 0);
			}
		}
		for (uint32_t i = 0; i < player->data->n_fashion; ++i)
		{
			player->data->fashion[i].isNew = false;
		}
		player->data->charm_level = 1;
	}

	//染色
	uint32_t color[MAX_COLOR_NUM] = { 131000001,131000002,131000003,131000004,131000005,};
	bool colorNew[MAX_COLOR_NUM] = { 0,0,0,0,0, };
	ColorList sendColor;
	color_list__init(&sendColor);
	if (player->data->n_color == 0)
	{
		player->data->n_color = 5;
		memcpy(player->data->color, color, sizeof(uint32_t)*player->data->n_color);
		memcpy(player->data->color_is_new, colorNew, sizeof(uint32_t) * player->data->n_color);
	}
	sendColor.n_isnew = sendColor.n_color = player->data->n_color;
	sendColor.color = player->data->color;
	sendColor.isnew = player->data->color_is_new;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_COLOR_LIST_NOTIFY, color_list__pack, sendColor);
	sendColor.n_isnew = sendColor.n_color = player->data->n_weapon_color;
	sendColor.color = player->data->weapon_color;
	sendColor.isnew = player->data->weapon_color_is_new;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_WEAPON_COLOR_LIST_NOTIFY, color_list__pack, sendColor);

	//时装
	FashionData fashion[MAX_FASHION_NUM];
	FashionData *fashionPoint[MAX_FASHION_NUM];
	FashionList sendFashion;
	fashion_list__init(&sendFashion);
	int num = 0;
	for (uint32_t i = 0; i < player->data->n_fashion; ++i)
	{
		if (player->data->fashion[i].timeout != 0)
		{
			if (player->data->fashion[i].timeout <= (time_t)time_helper::get_cached_time() / 1000)
			{
				continue;
			}
		}
		pack_fashion_info(fashion[num], player, i);
		fashionPoint[num] = &fashion[num];
		++num;
	}
	sendFashion.n_data = num;
	sendFashion.data = fashionPoint;
	sendFashion.level = player->data->charm_level;
	sendFashion.charm = player->data->charm_total;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_FASHION_LIST_NOTIFY, fashion_list__pack, sendFashion);
}


static int handle_handle_refuce_apply_team_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return 2;
	}
	if (player->get_uuid() != player->m_team->GetLeadId())
	{
		return 3;
	}

	RefuceApplyTeam note;
	refuce_apply_team__init(&note);
	note.name = player->data->name;
	uint64_t *ppp = conn_node_gamesrv::prepare_broadcast_msg_to_players(MSG_ID_REFUCE_APPLY_TEAM_NOTIFY, &note, (pack_func)refuce_apply_team__pack);
	PROTO_HEAD_CONN_BROADCAST *head;
	head = (PROTO_HEAD_CONN_BROADCAST *)conn_node_base::global_send_buf;
	head->num_player_id = player->m_team->m_data->m_applySize;
	memcpy(ppp, player->m_team->m_data->m_applyList, sizeof(uint64_t) * head->num_player_id);
	if (head->num_player_id > 0)
	{
		head->len += sizeof(uint64_t) * head->num_player_id;
		conn_node_gamesrv::broadcast_msg_send();
	}

	player->m_team->RemoveAllApply();

	CommAnswer resp;
	comm_answer__init(&resp);
	resp.result = 0;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HANDLE_REFUCE_APPLY_TEAM_ANSWER, comm_answer__pack, resp);

	//player->m_team->SendApplyList(*player);

	return 0;
}

//吟唱中断请求
static int handle_sing_interrupt_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	int ret = 0;
	do
	{
		player->interrupt_sing();
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SING_INTERRUPT_ANSWER, comm_answer__pack, resp);

	return 0;
}

//吟唱结束请求
static int handle_sing_end_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	int ret = 0;
	do
	{
		if (player->data->sing_info.type == 0)
		{
			ret = ERROR_ID_NOT_SINGING;
			LOG_ERR("[%s:%d] player[%lu] not singing", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		uint64_t cur_time = time_helper::get_cached_time();
		if (player->data->sing_info.start_ts + (uint64_t)player->data->sing_info.time > cur_time + 500)
		{
			ret = ERROR_ID_SING_TIME;
			LOG_ERR("[%s:%d] player[%lu] sing time, start_ts:%lu, time:%u, cur_time:%lu", __FUNCTION__, __LINE__,
				extern_data->player_id, player->data->sing_info.start_ts, player->data->sing_info.time, cur_time);
			break;
		}

		player->end_sing();
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SING_END_ANSWER, comm_answer__pack, resp);

	return 0;
}

//吟唱开始请求
static int handle_sing_begin_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	SingBeginRequest *req = sing_begin_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t sing_type = req->type;
	uint32_t sing_time = req->time;

	sing_begin_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		bool broadcast = false;
		bool include_myself = false;
		if (sing_type == SING_TYPE__TASK)
		{
			broadcast = false;
			include_myself = false;
		}
		player->begin_sing(sing_type, sing_time, broadcast, include_myself, NULL);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SING_BEGIN_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int send_transfer_to_leader_notify(uint64_t player_id, uint64_t leader_player_id)
{
	EXTERN_DATA extern_data;
	extern_data.player_id = player_id;
	TeamMemberRefuseTransferNotify notify;
	team_member_refuse_transfer_notify__init(&notify);
	notify.player_id = leader_player_id;

	fast_send_msg(&conn_node_gamesrv::connecter, &extern_data,
		MSG_ID_TRANSFER_TO_LEADER_NOTIFY, team_member_refuse_transfer_notify__pack, notify);
//	fast_send_msg_base(&conn_node_gamesrv::connecter, &extern_data, MSG_ID_TRANSFER_TO_LEADER_NOTIFY, 0, 0);
	return (0);
}

static int send_team_member_refuse_transfer_notify(player_struct *player, player_struct *leader)
{
	EXTERN_DATA extern_data;
	extern_data.player_id = leader->get_uuid();
	TeamMemberRefuseTransferNotify notify;
	team_member_refuse_transfer_notify__init(&notify);
	notify.player_id = player->get_uuid();
	fast_send_msg(&conn_node_gamesrv::connecter, &extern_data,
		MSG_ID_TEAM_MEMBER_REFUSE_TRANSFER_NOTIFY, team_member_refuse_transfer_notify__pack, notify);
	return (0);
}

// static int send_enter_raid_fail(player_struct *player, uint32_t err_code, uint32_t n_reason_player, uint64_t *reason_player_id, uint32_t item_id)
// {
//	EnterRaidAnswer resp;
//	enter_raid_answer__init(&resp);
//	resp.result = err_code;
//	resp.n_reson_player_id = n_reason_player;
//	resp.reson_player_id = reason_player_id;
//	resp.item_id = item_id;

//	EXTERN_DATA extern_data;
//	extern_data.player_id = player->get_uuid();
//	fast_send_msg(&conn_node_gamesrv::connecter, &extern_data, MSG_ID_ENTER_RAID_ANSWER, enter_raid_answer__pack, resp);
//	return (0);
// }

// static int check_enter_raid_reward_time(player_struct *player, uint32_t id, struct ControlTable *config)
// {
//	if (config->TimeType != 1)
//		return (0);
//	uint32_t count = player->get_raid_reward_count(id);
//	if (count >= config->RewardTime)
//		return 10;
//	return (0);
// }

// static int check_enter_raid_cost(player_struct *player, struct DungeonTable *r_config)
// {
//		//不消耗
//	if (r_config->CostItemID == 0)
//		return (0);

//	switch (get_item_type(r_config->CostItemID))
//	{
//		case ITEM_TYPE_COIN: //金钱
//			if (player->get_attr(PLAYER_ATTR_COIN) < r_config->CostNum)
//				return 5;
//			break;
//		case ITEM_TYPE_BIND_GOLD: //元宝
//		case ITEM_TYPE_GOLD:
//			if (player->get_comm_gold() < (int)r_config->CostNum)
//				return 6;
//			break;
//		default: //道具
//			if (player->get_item_can_use_num(r_config->CostItemID) < (int)r_config->CostNum)
//				return 4;
//			break;
//	}
//	return (0);
// }

// static int do_enter_raid_cost(player_struct *player, uint32_t item_id, uint32_t item_num)
// {
//	if (item_id == 0 || item_num == 0)
//		return (0);
//	switch (get_item_type(item_id))
//	{
//		case ITEM_TYPE_COIN: //金钱
//			player->sub_coin(item_num, MAGIC_TYPE_ENTER_RAID, true);
//			break;
//		case ITEM_TYPE_BIND_GOLD: //元宝
//		case ITEM_TYPE_GOLD:
//			player->sub_comm_gold(item_num, MAGIC_TYPE_ENTER_RAID, true);
//			break;
//		default: //道具
//			player->del_item(item_id, item_num, MAGIC_TYPE_ENTER_RAID, true);
//			break;
//	}
//	return (0);
// }

// static int check_player_enter_raid(player_struct *player, uint32_t raid_id)
// {
//	uint32_t n_reason_player = 0;
//	uint64_t reason_player_id[MAX_TEAM_MEM];
//	struct DungeonTable *r_config = get_config_by_id(raid_id, &all_raid_config);
//	struct ControlTable *control_config = get_config_by_id(r_config->ActivityControl, &all_control_config);

//	struct SceneResTable *s_config = get_config_by_id(raid_id, &scene_res_config);
//	if (!r_config || !s_config)
//	{
//		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//		return (-1);
//	}

//		//万妖谷小关卡不能直接进入
//	if (r_config->DengeonRank == 4)
//	{
//		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//		return (-2);
//	}

//		//当前在副本中
//	if (player->scene && player->scene->get_scene_type() == SCENE_TYPE_RAID)
//	{
//		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//		return (-5);
//	}

//		//当前在位面副本中
//	if (player->sight_space)
//	{
//		LOG_ERR("%s %d: player[%lu] sightspace[%p]", __FUNCTION__, __LINE__, player->get_uuid(), player->sight_space);
//		return (-6);
//	}

//		//检查等级
//	if (player->get_attr(PLAYER_ATTR_LEVEL) < s_config->level)
//	{
//		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//		return (-10);
//	}
//		// TODO: 表已经改过了
//		//检查时间
//	// if (control_config->n_OpenTime > 0 && control_config->OpenTime[0] != 0)
//	// {
//	//	bool pass = false;
//	//	uint32_t week = time_helper::getWeek(time_helper::get_cached_time() / 1000);
//	//	for (size_t i = 0; i < control_config->n_OpenTime; ++i)
//	//	{
//	//		if (week == control_config->OpenTime[i])
//	//		{
//	//			pass = true;
//	//			break;
//	//		}
//	//	}
//	//	if (pass == false)
//	//	{
//	//		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//	//		send_enter_raid_fail(player, 8, 0, NULL, 0);
//	//		return (-20);
//	//	}
//	// }

//		//个人副本通过
//	if (r_config->DengeonType == 2)
//	{
//		if (player->m_team)
//		{
//			LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//			return (-30);
//		}

//		int ret = check_enter_raid_reward_time(player, raid_id, control_config);
//		if (ret != 0)
//		{
//			reason_player_id[0] = player->data->player_id;
//			send_enter_raid_fail(player, 10, 1, reason_player_id, 0);
//			return (-32);
//		}

//		ret = check_enter_raid_cost(player, r_config);
//		if (ret != 0)
//		{
//			reason_player_id[0] = player->data->player_id;
//			send_enter_raid_fail(player, ret, 1, reason_player_id, r_config->CostItemID);
//			return (-35);
//		}
//		do_enter_raid_cost(player, r_config->CostItemID, r_config->CostNum);

//		return (0);
//	}

//	if (r_config->DengeonRank == 9) //阵营战副本
//	{
//		return (0);
//	}

//		//组队副本检查队伍
//	if (!player->m_team)
//	{
//		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//		return (-40);
//	}

//		//是否是队长
//	if (player->get_uuid() != player->m_team->GetLeadId())
//	{
//		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//		return (-50);
//	}

//		//检查人数
//	uint32_t team_mem_num = player->m_team->GetMemberSize();
//	if (control_config->MinActor > team_mem_num
//		|| control_config->MaxActor < team_mem_num)
//	{
//		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
//		return (-60);
//	}

//	bool pass = true;
//		//检查是否有人离线
//	for (int pos = 0; pos < player->m_team->m_data->m_memSize; ++pos)
//	{
//		if (player->m_team->m_data->m_mem[pos].timeremove != 0)
//		{
// //			send_enter_raid_fail(player, 7, player->m_team->m_data->m_mem[pos].id, 0);
// //			return (-70);
//			reason_player_id[n_reason_player++] = player->m_team->m_data->m_mem[pos].id;
//			pass = false;
//		}
//	}
//	if (!pass)
//	{
//		send_enter_raid_fail(player, 7, n_reason_player, reason_player_id, 0);
//		return (-70);
//	}


//		//检查等级, 消耗
//	player_struct *team_players[MAX_TEAM_MEM];
//	uint32_t i = 0;
//	for (int pos = 0; pos < player->m_team->m_data->m_memSize; ++pos)
//	{
//		player_struct *t_player = player_manager::get_player_by_id(player->m_team->m_data->m_mem[pos].id);
//		if (!t_player)
//		{
//			LOG_ERR("%s: can not find team mem %lu", __FUNCTION__, player->m_team->m_data->m_mem[pos].id);
// //			send_enter_raid_fail(player, 2, player->m_team->m_data->m_mem[pos].id, 0);
//			return (-50);
//		}
//		if (t_player->get_attr(PLAYER_ATTR_LEVEL) < s_config->level)
//		{
//			pass = false;
//			reason_player_id[n_reason_player++] = player->m_team->m_data->m_mem[pos].id;
// //			send_enter_raid_fail(player, 2, t_player->get_uuid(), 0);
// //			return (-60);
//		}

// //		int ret = check_enter_raid_cost(t_player, r_config);
// //		if (ret != 0)
// //		{
// //			send_enter_raid_fail(player, ret, t_player->data->player_id, r_config->CostItemID);
// //			return (-65);
// //		}

//		team_players[i++] = t_player;
//	}
//	if (!pass)
//	{
//		send_enter_raid_fail(player, 2, n_reason_player, reason_player_id, 0);
//		return (-60);
//	}

// //// 检查收益次数
//	for (int pos = 0; pos < player->m_team->m_data->m_memSize; ++pos)
//	{
//		player_struct *t_player = team_players[pos];
//		int ret = check_enter_raid_reward_time(t_player, raid_id, control_config);
//		if (ret != 0)
//		{
//			pass = false;
//			reason_player_id[n_reason_player++] = player->m_team->m_data->m_mem[pos].id;
//		}
//	}
//	if (!pass)
//	{
//		send_enter_raid_fail(player, 10, n_reason_player, reason_player_id, r_config->CostItemID);
//		return (-62);
//	}
// //// 检查消耗
//	int fail_ret;
//	for (int pos = 0; pos < player->m_team->m_data->m_memSize; ++pos)
//	{
//		player_struct *t_player = team_players[pos];
//		int ret = check_enter_raid_cost(t_player, r_config);
//		if (ret != 0)
//		{
//			fail_ret = ret;
//			pass = false;
//			reason_player_id[n_reason_player++] = player->m_team->m_data->m_mem[pos].id;
// //			send_enter_raid_fail(player, ret, t_player->data->player_id, r_config->CostItemID);
// //			return (-65);
//		}
//	}
//	if (!pass)
//	{
//		send_enter_raid_fail(player, fail_ret, n_reason_player, reason_player_id, r_config->CostItemID);
//		return (-65);
//	}
// ////


// //	if (i != team_mem_num)
// //	{
// //		LOG_ERR("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid_id);
// //		return (-70);
// //	}

//		//检查距离
//	struct position *leader_pos = player->get_pos();
// //	uint64_t too_far_player_id = 0;
//	for (i = 1; i < team_mem_num; ++i)
//	{
//		player_struct *t_player = team_players[i];

//		if (player->scene != t_player->scene)
//		{
//			pass = false;
//			reason_player_id[n_reason_player++] = t_player->get_uuid();
//			continue;
//		}

//		struct position *pos = t_player->get_pos();
//		uint64_t x = (pos->pos_x - leader_pos->pos_x);
//		uint64_t z = (pos->pos_z - leader_pos->pos_z);
//		if (x * x + z * z > max_team_mem_distance)
//		{
//			pass = false;
//			reason_player_id[n_reason_player++] = t_player->get_uuid();
// //			too_far_player_id = t_player->get_uuid();
// //			send_transfer_to_leader_notify(too_far_player_id);
//		}
//	}

// //	if (too_far_player_id != 0)
//	if (!pass)
//	{
//		send_enter_raid_fail(player, 3, n_reason_player, reason_player_id, 0);
//		return (-80);
//	}

//	for (int pos = 0; pos < player->m_team->m_data->m_memSize; ++pos)
//	{
//		player_struct *t_player = player_manager::get_player_by_id(player->m_team->m_data->m_mem[pos].id);
//		do_enter_raid_cost(t_player, r_config->CostItemID, r_config->CostNum);
//	}

//	return (0);
// }
// /*
// static int player_team_enter_raid(player_struct *player, raid_struct *raid)
// {
//	for (std::vector<MEM_INFO>::iterator ite = player->m_team->m_mem.begin(); ite != player->m_team->m_mem.end(); ++ite)
//	{
//		player_struct *t_player = player_manager::get_player_by_id((*ite).id);
//		assert(t_player);
//		if (raid->player_enter_raid(t_player, true) != 0)
//		{
//			LOG_ERR("%s: player[%lu] enter raid failed", __FUNCTION__, t_player->get_uuid());
//		}
//	}
//	return (0);
// }
// */


static int handle_planes_raid_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player->sight_space)
	{
		LOG_ERR("%s: player[%lu] do not have sightspace", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	sight_space_manager::del_player_from_sight_space(player->sight_space, player, true);
	return (0);
}

static int handle_team_raid_ready_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	if (!player->m_team)
	{
		LOG_ERR("%s %d: player[%lu] do not have team", __FUNCTION__, __LINE__, player->get_uuid());
		return (-10);
	}
	if (player->data->team_raid_id_wait_ready == 0)
	{
		LOG_ERR("%s %d: player[%lu] do not wait ready", __FUNCTION__, __LINE__, player->get_uuid());
		return (-20);		
	}
	if (player->data->is_team_raid_ready)
	{
		LOG_ERR("%s %d: player[%lu] already set ready", __FUNCTION__, __LINE__, player->get_uuid());
		return (-30);				
	}

	player->data->is_team_raid_ready = true;

	for (int pos = 0; pos < player->m_team->m_data->m_memSize; ++pos)
	{
		player_struct *t_player = player_manager::get_player_by_id(player->m_team->m_data->m_mem[pos].id);
		assert(t_player);
		assert(t_player->data);
		assert(t_player->data->team_raid_id_wait_ready == player->data->is_team_raid_ready);
		if (!t_player->data->is_team_raid_ready)
		{
			TeamRaidReadyNotify nty;
			team_raid_ready_notify__init(&nty);
			nty.player_id = player->get_uuid();
			player->m_team->BroadcastToTeam(MSG_ID_TEAM_RAID_READY_NOTIFY, &nty, (pack_func)team_raid_ready_notify__pack, 0);
			return (0);
		}
	}

		// 如果所有人都准备好了，就进入副本
	uint32_t raid_id = player->data->is_team_raid_ready;
	player->m_team->unset_raid_id_wait_ready();
	int ret = raid_manager::check_player_enter_raid(player, raid_id);
	if (ret != 1)
	{
		LOG_INFO("%s: player[%lu] enter raid[%u] failed", __FUNCTION__, extern_data->player_id, raid_id);
		TeamRaidCancelNotify nty;
		team_raid_cancel_notify__init(&nty);
		nty.player_id = player->get_uuid();
		nty.result = 1;
		player->m_team->BroadcastToTeam(MSG_ID_TEAM_RAID_CANCEL_NOTIFY, &nty, (pack_func)team_raid_cancel_notify__pack, 0);
		return -10;
	}
	raid_manager::do_team_enter_raid_cost(player, raid_id);
	raid_struct *raid = raid_manager::create_raid(raid_id, player);
	if (!raid)
	{
		LOG_ERR("%s: player[%lu] create raid[%u] failed", __FUNCTION__, extern_data->player_id, raid_id);
		return (-20);
	}
	assert(raid->res_config);
	raid->team_enter_raid(player->m_team);
	return (0);	
}
static int handle_team_raid_cancel_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	if (!player->m_team)
	{
		LOG_ERR("%s %d: player[%lu] do not have team", __FUNCTION__, __LINE__, player->get_uuid());
		return (-10);
	}
	if (player->data->team_raid_id_wait_ready == 0)
	{
		LOG_ERR("%s %d: player[%lu] do not wait ready", __FUNCTION__, __LINE__, player->get_uuid());
		return (-20);		
	}

	player->m_team->unset_raid_id_wait_ready();
	TeamRaidCancelNotify nty;
	team_raid_cancel_notify__init(&nty);
	nty.player_id = player->get_uuid();
	nty.result = 1;
	player->m_team->BroadcastToTeam(MSG_ID_TEAM_RAID_CANCEL_NOTIFY, &nty, (pack_func)team_raid_cancel_notify__pack, 0);
	return (0);
}

static int handle_complete_raid_ai_requrest(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	raid_struct *raid = player->get_raid();
	if (!raid)
	{
		LOG_ERR("[%s:%d] player[%lu] not in raid", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	
	direct_next_script(raid, &raid->SCRIPT_DATA.script_data);

	return 0;
}

static int handle_get_raid_monster_pos_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	raid_struct *raid = player->get_raid();
	if (!raid)
	{
		LOG_ERR("[%s:%d] player[%lu] not in raid", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	GetRaidMonsterPosRequest *req = get_raid_monster_pos_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t monster_id = req->monster_id;
	get_raid_monster_pos_request__free_unpacked(req, NULL);

	GetRaidMonsterPosAnswer resp;
	get_raid_monster_pos_answer__init(&resp);
	resp.monster_id = monster_id;
	resp.result = -1;

	PosData pos_data;
	pos_data__init(&pos_data);
	resp.data = &pos_data;
	
	for (std::set<monster_struct*>::iterator iter = raid->m_monster.begin(); iter != raid->m_monster.end(); ++iter)
	{
		monster_struct *monster = *iter;
		if (monster->data && monster->data->monster_id == monster_id)
		{
			struct position *pos = monster->get_pos();
			resp.result = 0;
			pos_data.pos_x = pos->pos_x;
			pos_data.pos_z = pos->pos_z;			
			break;
		}
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GET_RAID_MONSTER_POS_ANSWER, get_raid_monster_pos_answer__pack, resp);
	return (0);
}

static int handle_enter_raid_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	EnterRaidRequest *req = enter_raid_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t raid_id = req->raid_id;
	enter_raid_request__free_unpacked(req, NULL);

	LOG_INFO("[%s:%d] player[%lu] raid_id[%u]", __FUNCTION__, __LINE__, extern_data->player_id, raid_id);

	DungeonTable *config = get_config_by_id(raid_id, &all_raid_config);
	if (!config)
	{
		LOG_ERR("[%s:%d] player[%lu] get config failed, raid_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, raid_id);
		return -1;
	}

	if (config->DengeonRank == DUNGEON_TYPE_GUILD_LAND)
	{
		int ret = 0;
		do
		{
			if (player->data->guild_id == 0)
			{
				ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
				LOG_ERR("[%s:%d] player[%lu] not join guild", __FUNCTION__, __LINE__, extern_data->player_id);
				break;
			}

			if (raid_manager::check_player_enter_raid(player, raid_id) != 0)
			{
				break;
			}

			player->move_to_scene(raid_id, extern_data);
			// guild_land_raid_struct *raid = guild_land_raid_manager::get_guild_land_raid(player->data->guild_id);
			// if (!raid)
			// {
			// 	ret = ERROR_ID_SERVER;
			// 	LOG_ERR("[%s:%d] player[%lu] get raid failed", __FUNCTION__, __LINE__, extern_data->player_id);
			// 	break;
			// }

			// raid->player_enter_raid(player, raid->res_config->BirthPointX, raid->res_config->BirthPointZ);
		} while(0);

		if (ret != 0)
		{
			raid_manager::send_enter_raid_fail(player, ret, 0, NULL, 0);
		}
		return 0;
	}
	else if (config->DengeonRank == DUNGEON_TYPE_GUILD_WAIT)
	{
		int ret = 0;
		do
		{
			if (!is_guild_battle_opening())
			{
				ret = 190411002;
				LOG_ERR("[%s:%d] player[%lu] guild battle not open, raid_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, raid_id);
				break;
			}

			ret = player_can_participate_guild_battle(player);
			if (ret != 0)
			{
				LOG_ERR("[%s:%d] player[%lu] can't participate, raid_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, raid_id);
				break;
			}

			if (player->m_team)
			{
				ret = -1;
				LOG_ERR("[%s:%d] player[%lu] in team, raid_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, raid_id);
				break;
			}

//			guild_wait_raid_manager::add_player_to_guild_wait_raid(player);
			player->move_to_scene(raid_id, extern_data);			
		} while(0);

		if (ret != 0)
		{
			raid_manager::send_enter_raid_fail(player, ret, 0, NULL, 0);
		}
		return 0;
	}

	int ret = raid_manager::check_player_enter_raid(player, raid_id);
	switch (ret)
	{
		case 0: //进入副本
		case 1:
		{
			player->move_to_scene(raid_id, extern_data);
		}
		break;
		default:
		{
			LOG_INFO("%s: player[%lu] enter raid[%u] failed", __FUNCTION__, extern_data->player_id, raid_id);
			return (-15);
		}
	}
	// 		raid_struct *raid = raid_manager::create_raid(raid_id, player);
	// 		if (!raid)
	// 		{
	// 			LOG_ERR("%s: player[%lu] create raid[%u] failed", __FUNCTION__, extern_data->player_id, raid_id);
	// 			return (-20);
	// 		}

	// 		assert(raid->res_config);

	// 		if (player->m_team)
	// 		{
	// 			raid->team_enter_raid(player->m_team);
	// 		}
	// 		else
	// 		{
	// 			int x = raid->res_config->BirthPointX, z = raid->res_config->BirthPointZ;
	// 			double direct = 0;
	// 			if (raid->m_config->DengeonRank == DUNGEON_TYPE_BATTLE_NEW)
	// 			{
	// 				BattlefieldTable *table = zhenying_fight_config.begin()->second;
	// 				if (table != NULL)
	// 				{
	// 					ZhenyingBattle::GetInstance()->GetRelivePos(table, player->get_attr(PLAYER_ATTR_ZHENYING), &x, &z, &direct);
	// 				}
	// 			}
	// 			if (raid->player_enter_raid(player, x, z, direct) != 0)
	// 			{
	// 				LOG_ERR("%s: player[%lu] enter raid failed", __FUNCTION__, player->get_uuid());
	// 				return (-30);
	// 			}
	// 		}
	// 	}
	// 	break;
	// 	// case 1:
	// 	// {
	// 	// 	assert(player->m_team);
	// 	// 	TeamRaidWaitReadyNotify nty;
	// 	// 	team_raid_wait_ready_notify__init(&nty);
	// 	// 	nty.raid_id = raid_id;
	// 	// 	player->m_team->BroadcastToTeam(MSG_ID_TEAM_RAID_WAIT_READY_NOTIFY, &nty, (pack_func)team_raid_wait_ready_notify__pack, 0);
	// 	// 	player->m_team->set_raid_id_wait_ready(raid_id);
	// 	// }
	// 	// break;
	// 	default:
	// 		LOG_INFO("%s: player[%lu] enter raid[%u] failed", __FUNCTION__, extern_data->player_id, raid_id);
	// 		return (-15);
	// }

// 	raid_struct *raid = raid_manager::create_raid(raid_id, player);
// 	if (!raid)
// 	{
// 		LOG_ERR("%s: player[%lu] create raid[%u] failed", __FUNCTION__, extern_data->player_id, raid_id);
// 		return (-20);
// 	}

// 	assert(raid->res_config);

// 	if (player->m_team)
// 	{
// //		player_team_enter_raid(player, raid);
// 		raid->team_enter_raid(player->m_team);
// 	}
// 	else
// 	{
// 		if (raid->player_enter_raid(player, raid->res_config->BirthPointX, raid->res_config->BirthPointZ) != 0)
// 		{
// 			LOG_ERR("%s: player[%lu] enter raid failed", __FUNCTION__, player->get_uuid());
// 			return (-30);
// 		}
// 	}
 	return (0);
}
static int handle_leave_raid_request(player_struct *player, EXTERN_DATA *extern_data)
{
	raid_struct *raid = player->get_raid();
	if (!raid || !raid->data)
	{
		LOG_ERR("%s: player[%lu] not in raid", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (get_scene_looks_type(raid->m_id) == SCENE_TYPE_RAID
		&& raid->m_config->DengeonType != 2
		&& raid->data->state == RAID_STATE_START
		&& player->m_team)
	{
			//组队副本如果没结束，那么踢出队伍
		player->m_team->RemoveMember(*player, false);		
	}
	else
	{
		raid->player_leave_raid(player);
	}
	return (0);
}

static int handle_transfer_far_team_member_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player->m_team)
		return (-1);
		//是否是队长
	if (player->get_uuid() != player->m_team->GetLeadId())
	{
		LOG_ERR("%s %d: player[%lu]", __FUNCTION__, __LINE__, player->get_uuid());
		return (-10);
	}

	uint32_t team_mem_num = player->m_team->GetMemberSize();
		//检查距离
	struct position *leader_pos = player->get_pos();
	for (uint32_t i = 1; i < team_mem_num; ++i)
	{
		player_struct *t_player = player_manager::get_player_by_id(player->m_team->m_data->m_mem[i].id);
		if (!t_player || !t_player->data)
		{
			continue;
		}

		if (player->scene != t_player->scene)
		{
			send_transfer_to_leader_notify(t_player->data->player_id, player->get_uuid());
			continue;
		}

		struct position *pos = t_player->get_pos();
		uint64_t x = (pos->pos_x - leader_pos->pos_x);
		uint64_t z = (pos->pos_z - leader_pos->pos_z);
		if (x * x + z * z > max_team_mem_distance)
		{
			send_transfer_to_leader_notify(t_player->data->player_id, player->get_uuid());
		}
	}
	return (0);
}

static int check_can_transfer_to_leader(player_struct *player)
{
	player_struct *leader = player->m_team->GetLead();
	if (!leader || !leader->is_avaliable())
	{
		return 190500375;
	}
	int ret = player->check_can_transfer();
	if (ret != 0)
	{
		return ret;
	}
	if (leader->is_in_raid())
	{
		return 190500375;
	}
	if (player->get_attr(PLAYER_ATTR_LEVEL) < leader->scene->res_config->Level)
	{
		return 190500375;
	}
	return 0;
}
static int handle_transfer_to_leader_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (!player->m_team)
	{
		LOG_ERR("%s: player[%lu] not have team", __FUNCTION__, extern_data->player_id);
		return (-10);
	}

	if (player->get_uuid() == player->m_team->GetLeadId())
	{
		return (-20);
	}
	
	player_struct *leader = player->m_team->GetLead();
	if (leader == NULL)
	{
		return -11;
	}

	TransferToLeaderRequest *req = transfer_to_leader_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-40);
	}
	int answer = req->result;
	transfer_to_leader_request__free_unpacked(req, NULL);
	if (answer != 0)
	{
		send_team_member_refuse_transfer_notify(player, leader);
		return 0;
	}
	
	int ret = check_can_transfer_to_leader(player);
	if (ret == 0)
	{
		
		struct position *leader_pos = leader->get_pos();
		player->transfer_to_new_scene(leader->data->scene_id, leader_pos->pos_x, -1, leader_pos->pos_z, 0, extern_data);
	}

	send_comm_answer(MSG_ID_TRANSFER_TO_LEADER_ANSWER, ret, extern_data);

	return (0);
}

//装备信息请求
static int handle_equip_list_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	return notify_equip_list(player, extern_data);
}

static int notify_equip_list(player_struct *player, EXTERN_DATA *extern_data)
{
	EquipListAnswer resp;
	equip_list_answer__init(&resp);

	EquipData equip_data[MAX_EQUIP_NUM];
	EquipData* equip_data_point[MAX_EQUIP_NUM];
	EquipEnchantData enchant_data[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM];
	EquipEnchantData* enchant_data_point[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData cur_attr[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData* cur_attr_point[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData rand_attr[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM][MAX_EQUIP_ENCHANT_RAND_NUM];
	CommonRandAttrData* rand_attr_point[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM][MAX_EQUIP_ENCHANT_RAND_NUM];
	ProposeRingInfo ring_attr_info;

	resp.result = 0;
	size_t equip_num = 0;
	for (int k = 0; k < MAX_EQUIP_NUM; ++k)
	{
		EquipInfo &equip_info = player->data->equip_list[k];
		if (equip_info.stair == 0)
		{
			continue;
		}

		equip_data_point[equip_num] = &equip_data[equip_num];
		equip_data__init(&equip_data[equip_num]);

		equip_data[equip_num].type = k + 1;
		equip_data[equip_num].stair = equip_info.stair;
		equip_data[equip_num].starlv = equip_info.star_lv;
		equip_data[equip_num].starexp = equip_info.star_exp;
		//指环部位额外加求婚戒指属性
		if(equip_data[equip_num].type == 9)
		{
			WeddingRing *wedding_ring_config = get_config_by_id(player->data->player_marry_info.propose_type, &propose_ring_config);
			if(wedding_ring_config != NULL)
			{
				propose_ring_info__init(&ring_attr_info);
				ring_attr_info.ring_id = wedding_ring_config->ID;
				ring_attr_info.target_name = player->data->player_marry_info.target_name;
				equip_data[equip_num].ring_info = &ring_attr_info;
			}
		}
		size_t enchant_num = 0;
		for (int i = 0; i < MAX_EQUIP_ENCHANT_NUM; ++i)
		{
			EquipEnchantInfo &enchant_info = equip_info.enchant[i];

			enchant_data_point[equip_num][enchant_num] = &enchant_data[equip_num][enchant_num];
			equip_enchant_data__init(&enchant_data[equip_num][enchant_num]);
			enchant_data[equip_num][enchant_num].index = i;

			cur_attr_point[equip_num][enchant_num] = &cur_attr[equip_num][enchant_num];
			common_rand_attr_data__init(&cur_attr[equip_num][enchant_num]);
			cur_attr[equip_num][enchant_num].pool = enchant_info.cur_attr.pool;
			cur_attr[equip_num][enchant_num].id = enchant_info.cur_attr.id;
			cur_attr[equip_num][enchant_num].val = enchant_info.cur_attr.val;
			enchant_data[equip_num][enchant_num].curattr = cur_attr_point[equip_num][enchant_num];

			size_t rand_num = 0;
			if (enchant_info.rand_attr[0].id > 0)
			{
				for (int j = 0; j < MAX_EQUIP_ENCHANT_RAND_NUM; ++j)
				{
					rand_attr_point[equip_num][enchant_num][rand_num] = &rand_attr[equip_num][enchant_num][rand_num];
					common_rand_attr_data__init(&rand_attr[equip_num][enchant_num][rand_num]);
					rand_attr[equip_num][enchant_num][rand_num].pool = enchant_info.rand_attr[j].pool;
					rand_attr[equip_num][enchant_num][rand_num].id = enchant_info.rand_attr[j].id;
					rand_attr[equip_num][enchant_num][rand_num].val = enchant_info.rand_attr[j].val;
					rand_num++;
				}
			}
			enchant_data[equip_num][enchant_num].randattr = rand_attr_point[equip_num][enchant_num];
			enchant_data[equip_num][enchant_num].n_randattr = rand_num;

			enchant_num++;
		}
		equip_data[equip_num].n_enchant = enchant_num;
		equip_data[equip_num].enchant = enchant_data_point[equip_num];

		equip_data[equip_num].inlay = &equip_info.inlay[0];
		equip_data[equip_num].n_inlay = MAX_EQUIP_INLAY_NUM;
		equip_num++;
	}
	resp.equips = equip_data_point;
	resp.n_equips = equip_num;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_LIST_ANSWER, equip_list_answer__pack, resp);

	return 0;
}

//装备升星请求
static int handle_equip_star_up_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	EquipStarUpRequest *req = equip_star_up_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;
	uint32_t item_num = req->itemnum;
	equip_star_up_request__free_unpacked(req, NULL);

	int ret = 0;
	EquipInfo *equip_info = NULL;
	do
	{
		equip_info = player->get_equip(type);
		if (!equip_info || equip_info->stair == 0)
		{
			ret = ERROR_ID_EQUIP_NOT_ACTIVE;
			LOG_ERR("[%s:%d] player[%lu] get equip failed, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		uint32_t player_job = player->get_attr(PLAYER_ATTR_JOB);
		EquipStarLv *config = get_equip_star_config(player_job, type, equip_info->stair, equip_info->star_lv + 1);
		if (!config)
		{
			ret = ERROR_ID_EQUIP_STAR_MAX;
			LOG_ERR("[%s:%d] player[%lu] get star config failed, job:%u, type:%u, stair:%u, star:%u", __FUNCTION__, __LINE__, extern_data->player_id, player_job, type, equip_info->stair, equip_info->star_lv + 1);
			break;
		}

		uint32_t bag_num = player->get_item_can_use_num(config->ConsumeItem);
		if (bag_num < item_num || item_num == 0)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] item not enough, item_id:%u, item_num:%u, bag_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, (uint32_t)config->ConsumeItem, item_num, bag_num);
			break;
		}

		uint32_t player_coin = player->get_attr(PLAYER_ATTR_COIN);
		uint32_t need_coin = config->ConsumeCoin * item_num;
		if (player_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, player_coin:%u, need_coin:%u", __FUNCTION__, __LINE__, extern_data->player_id, player_coin, need_coin);
			break;
		}

		uint32_t player_lv = player->get_attr(PLAYER_ATTR_LEVEL);
		uint32_t need_lv = (uint32_t)config->Level;
		if (player_lv < need_lv)
		{
			ret = ERROR_ID_LEVEL_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] level not enough, player_lv:%u, need_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, player_lv, need_lv);
			break;
		}

		ItemsConfigTable *item_config = get_config_by_id(config->ConsumeItem, &::item_config);
		if (!item_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get item config failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, (uint32_t)config->ConsumeItem);
			break;
		}

		if (item_config->n_ParameterEffect == 0)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get item config failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, (uint32_t)config->ConsumeItem);
			break;
		}

		uint32_t max_exp = player->get_equip_max_star_need_exp(type);
		uint32_t item_exp = item_config->ParameterEffect[0];
		uint32_t use_num = item_num;
		if (item_exp * item_num > max_exp)
		{
			use_num = ceil((double)max_exp / (double)item_exp);
		}
		player->add_equip_exp(type, item_exp * use_num);

		player->sub_coin(use_num * config->ConsumeCoin, MAGIC_TYPE_EQUIP_STAR_UP);
		player->del_item(config->ConsumeItem, use_num, MAGIC_TYPE_EQUIP_STAR_UP);
	} while(0);

	EquipStarUpAnswer resp;
	equip_star_up_answer__init(&resp);

	resp.result = ret;
	if (equip_info)
	{
		resp.type = type;
		resp.starlv = equip_info->star_lv;
		resp.starexp = equip_info->star_exp;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_STAR_UP_ANSWER, equip_star_up_answer__pack, resp);

	return 0;
}

//装备进阶请求
static int handle_equip_stair_up_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	int ret = 0;
	do
	{
		bool all_star_max = true;
		bool all_has_next_stair = true;
		uint32_t player_job = player->get_attr(PLAYER_ATTR_JOB);
		uint32_t need_lv = 0;
		for (int i = 0; i < MAX_EQUIP_NUM; ++i)
		{
			EquipInfo &equip_info = player->data->equip_list[i];
			uint32_t type = i + 1;
			if (equip_info.stair == 0)
			{
				all_star_max = false;
				break;
			}

			if (get_equip_star_config(player_job, type, equip_info.stair, equip_info.star_lv + 1))
			{
				all_star_max = false;
				break;
			}

			EquipStarLv *config = get_equip_star_config(player_job, type, equip_info.stair + 1, 0);
			if (!config)
			{
				all_has_next_stair = false;
				break;
			}

			need_lv = std::max(need_lv, (uint32_t)config->Level);
		}

		if (!all_star_max)
		{
			ret = ERROR_ID_EQUIP_NOT_ALL_STAR_MAX;
			LOG_ERR("[%s:%d] player[%lu] not all equip star max", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (!all_has_next_stair)
		{
			ret = ERROR_ID_EQUIP_STAIR_MAX;
			LOG_ERR("[%s:%d] player[%lu] equip stair max", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		uint32_t player_lv = player->get_attr(PLAYER_ATTR_LEVEL);
		if (player_lv < need_lv)
		{
			ret = ERROR_ID_LEVEL_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] level not enough, player_lv:%u, need_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, player_lv, need_lv);
			break;
		}

		for (int i = 0; i < MAX_EQUIP_NUM; ++i)
		{
			EquipInfo &equip_info = player->data->equip_list[i];
			equip_info.stair++;
			equip_info.star_lv = 0;
			equip_info.star_exp = 0;
			player->add_task_progress(TCT_EQUIP_STAR, i, equip_info.stair * 10 + equip_info.star_lv);
		}

		//刷新武器外形
		player->update_weapon_skin(true);
		//刷新属性
		player->calculate_attribute(true);

		player->add_achievement_progress(ACType_EQUIP_STAIR, player->data->equip_list[0].stair, 0, 0, 1);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_STAIR_UP_ANSWER, comm_answer__pack, resp);

	return 0;
}

//装备附魔请求
static int handle_equip_enchant_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	EquipEnchantRequest *req = equip_enchant_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;
	uint32_t index = req->index - 1;
	uint32_t skip = req->skipretain;

	equip_enchant_request__free_unpacked(req, NULL);

	int ret = 0;
	EquipInfo *equip_info = NULL;
	do
	{
		equip_info = player->get_equip(type);
		if (!equip_info || equip_info->stair == 0)
		{
			ret = ERROR_ID_EQUIP_NOT_ACTIVE;
			LOG_ERR("[%s:%d] player[%lu] get equip failed, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		if (index >= MAX_EQUIP_ENCHANT_NUM)
		{
			ret = ERROR_ID_EQUIP_ENCHANT_INDEX;
			LOG_ERR("[%s:%d] player[%lu] enchant index error, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index);
			break;
		}

		EquipEnchantInfo *enchant_info = &equip_info->enchant[index];
		if (skip == 0 && enchant_info->rand_attr[0].id > 0)
		{
			ret = ERROR_ID_EQUIP_ENCHANT_NO_RETAIN;
			LOG_ERR("[%s:%d] player[%lu] enchant not retain yet, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index);
			break;
		}

		uint32_t player_job = player->get_job();
		EquipLock *lock_config = get_equip_lock_config(player_job, type);
		if (!lock_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get lock config failed, type:%u, job:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, player_job);
			break;
		}

		uint32_t need_stair = lock_config->EnchantQualityLock[index];
		uint32_t need_star = lock_config->EnchantStarLock[index];
		if (equip_info->stair < need_stair || (equip_info->stair == need_stair && equip_info->star_lv < need_star))
		{
			ret = ERROR_ID_EQUIP_STAR_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] star not enough, type:%u, index:%u, need_stair:%u, need_star:%u, cur_stair:%u, cur_star:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, need_stair, need_star, equip_info->stair, equip_info->star_lv);
			break;
		}

		uint32_t need_item_id = lock_config->EnchantItem[index];
		uint32_t need_item_num = lock_config->EnchantItemNum[index];
		uint32_t need_coin = lock_config->EnchantCoin[index];
		uint32_t bag_item_num = player->get_item_can_use_num(need_item_id);
		if (bag_item_num < need_item_num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] item not enough, item_id:%u, need_num:%u, bag_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, need_item_id, need_item_num, bag_item_num);
			break;
		}

		uint32_t player_coin = player->get_coin();
		if (player_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, need_coin, player_coin);
			break;
		}

//		EquipStarLv *star_config = get_equip_star_config(player_job, type, equip_info->stair, equip_info->star_lv);
		EquipmentTable *star_config = get_equip_config(player_job, type, equip_info->stair, equip_info->star_lv);
		if (!star_config || star_config->n_DatabaseSelection <= index)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get star config failed, type:%u, job:%u, stair:%u, star:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, player_job, equip_info->stair, equip_info->star_lv);
			break;
		}

		int rand_idx = 0;
		if (enchant_info->cur_attr.id > 0)
		{
			enchant_info->rand_attr[rand_idx].pool = enchant_info->cur_attr.pool;
			enchant_info->rand_attr[rand_idx].id = enchant_info->cur_attr.id;
			enchant_info->rand_attr[rand_idx].val = enchant_info->cur_attr.val;
			rand_idx++;
		}
		for (; rand_idx < MAX_EQUIP_ENCHANT_RAND_NUM; ++rand_idx)
		{
			ret = get_one_rand_attr(star_config->DatabaseSelection[index], enchant_info->rand_attr[rand_idx].id, enchant_info->rand_attr[rand_idx].val);
			if (ret != 0)
			{
				LOG_ERR("[%s:%d] player[%lu] get_one_rand_attr fail, type:%u, job:%u, stair:%u, star:%u, index:%u, pool:%lu, ret:%d", __FUNCTION__, __LINE__, extern_data->player_id, type, player_job, equip_info->stair, equip_info->star_lv, index, star_config->DatabaseSelection[index], ret);
				ret = ERROR_ID_CONFIG;
				break;
			}

			enchant_info->rand_attr[rand_idx].pool = star_config->DatabaseSelection[index];
		}

		if (ret != 0)
		{
			memset(enchant_info->rand_attr, 0, sizeof(AttrInfo) * MAX_EQUIP_ENCHANT_RAND_NUM);
			break;
		}

		player->del_item(need_item_id, need_item_num, MAGIC_TYPE_EQUIP_ENCHANT);
		player->sub_coin(need_coin, MAGIC_TYPE_EQUIP_ENCHANT);
		player->add_task_progress(TCT_EQUIP_ENCHANT, type, 1);
		player->add_achievement_progress(ACType_EQUIP_ENCHANT, 0, 0, 0, 1);
	} while(0);

	EquipEnchantAnswer resp;
	equip_enchant_answer__init(&resp);

	CommonRandAttrData rand_data[MAX_EQUIP_ENCHANT_RAND_NUM];
	CommonRandAttrData* rand_data_point[MAX_EQUIP_ENCHANT_RAND_NUM];

	resp.result = ret;
	resp.type = type;
	resp.index = index + 1;
	if (ret == 0 && equip_info != NULL)
	{
		for (int i = 0; i < MAX_EQUIP_ENCHANT_RAND_NUM; ++i)
		{
			rand_data_point[i] = &rand_data[i];
			common_rand_attr_data__init(&rand_data[i]);
			rand_data[i].pool = equip_info->enchant[index].rand_attr[i].pool;
			rand_data[i].id = equip_info->enchant[index].rand_attr[i].id;
			rand_data[i].val = equip_info->enchant[index].rand_attr[i].val;
		}
		resp.randattr = rand_data_point;
		resp.n_randattr = MAX_EQUIP_ENCHANT_RAND_NUM;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_ENCHANT_ANSWER, equip_enchant_answer__pack, resp);

	return 0;
}

//装备附魔保留请求
static int handle_equip_enchant_retain_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	EquipEnchantRetainRequest *req = equip_enchant_retain_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;
	uint32_t attr_index = req->attrindex - 1;
	uint32_t retain_index = req->retainindex - 1;

	equip_enchant_retain_request__free_unpacked(req, NULL);

	int ret = 0;
	EquipInfo *equip_info = NULL;
	CommonRandAttrData retain_attr;
	common_rand_attr_data__init(&retain_attr);
	do
	{
		equip_info = player->get_equip(type);
		if (!equip_info || equip_info->stair == 0)
		{
			ret = ERROR_ID_EQUIP_NOT_ACTIVE;
			LOG_ERR("[%s:%d] player[%lu] get equip failed, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		if (attr_index >= MAX_EQUIP_ENCHANT_NUM)
		{
			ret = ERROR_ID_EQUIP_ENCHANT_INDEX;
			LOG_ERR("[%s:%d] player[%lu] enchant index error, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, attr_index);
			break;
		}

		EquipEnchantInfo *enchant_info = &equip_info->enchant[attr_index];
		if (enchant_info->rand_attr[0].id == 0)
		{
			ret = ERROR_ID_EQUIP_ENCHANT_NO_IN_RETAIN;
			LOG_ERR("[%s:%d] player[%lu] enchant not in retain, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, attr_index);
			break;
		}

		if (retain_index >= MAX_EQUIP_ENCHANT_RAND_NUM)
		{
			ret = ERROR_ID_EQUIP_ENCHANT_RETAIN_INDEX;
			LOG_ERR("[%s:%d] player[%lu] enchant retain index error, type:%u, attr_index:%u, retain_index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, attr_index, retain_index);
			break;
		}

		if (!(retain_index == 0 && enchant_info->cur_attr.id > 0))
		{
			enchant_info->cur_attr.pool = enchant_info->rand_attr[retain_index].pool;
			enchant_info->cur_attr.id = enchant_info->rand_attr[retain_index].id;
			enchant_info->cur_attr.val = enchant_info->rand_attr[retain_index].val;
			player->calculate_attribute(true);
			player->add_achievement_progress(ACType_EQUIP_ENCHANT_COLOR_NUM, 0, 0, 0, 0);
		}

		memset(enchant_info->rand_attr, 0, sizeof(AttrInfo) * MAX_EQUIP_ENCHANT_RAND_NUM);
		retain_attr.pool = enchant_info->cur_attr.pool;
		retain_attr.id = enchant_info->cur_attr.id;
		retain_attr.val = enchant_info->cur_attr.val;
	} while(0);

	EquipEnchantRetainAnswer resp;
	equip_enchant_retain_answer__init(&resp);

	resp.result = ret;
	resp.type = type;
	resp.attrindex = attr_index + 1;
	resp.retainattr = &retain_attr;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_ENCHANT_RETAIN_ANSWER, equip_enchant_retain_answer__pack, resp);

	return 0;
}

//装备解锁孔位请求
static int handle_equip_drill_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	EquipDrillRequest *req = equip_drill_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;
	uint32_t index = req->index - 1;

	equip_drill_request__free_unpacked(req, NULL);

	int ret = 0;
	EquipInfo *equip_info = NULL;
	do
	{
		equip_info = player->get_equip(type);
		if (!equip_info || equip_info->stair == 0)
		{
			ret = ERROR_ID_EQUIP_NOT_ACTIVE;
			LOG_ERR("[%s:%d] player[%lu] get equip failed, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		if (index >= MAX_EQUIP_INLAY_NUM)
		{
			ret = ERROR_ID_EQUIP_HOLE_INDEX;
			LOG_ERR("[%s:%d] player[%lu] hole index error, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index);
			break;
		}

		if (equip_info->inlay[index] != -1)
		{
			ret = ERROR_ID_EQUIP_HOLE_OPENED;
			LOG_ERR("[%s:%d] player[%lu] hole opened, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index);
			break;
		}

		uint32_t player_job = player->get_job();
		EquipLock *config = get_equip_lock_config(player_job, type);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get lock config failed, type:%u, job:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, player_job);
			break;
		}

		uint32_t player_lv = player->get_attr(PLAYER_ATTR_LEVEL);
		uint32_t need_lv = config->LockLv[index];
		if (player_lv < need_lv)
		{
			ret = ERROR_ID_LEVEL_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] level not enough, type:%u, index:%u, need_lv:%u, player_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, need_lv, player_lv);
			break;
		}

		uint32_t need_stair = config->LockQuality[index];
		uint32_t need_star = config->LockStar[index];
		if (equip_info->stair < need_stair || (equip_info->stair == need_stair && equip_info->star_lv < need_star))
		{
			ret = ERROR_ID_EQUIP_STAR_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] star not enough, type:%u, index:%u, need_stair:%u, need_star:%u, cur_stair:%u, cur_star:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, need_stair, need_star, equip_info->stair, equip_info->star_lv);
			break;
		}

		uint32_t need_item_id = config->LockItem[index];
		uint32_t need_item_num = config->LockItemNum[index];
		uint32_t bag_item_num = player->get_item_can_use_num(need_item_id);
		if (bag_item_num < need_item_num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] item not enough, type:%u, index:%u, item_id:%u, need_num:%u, bag_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, need_item_id, need_item_num, bag_item_num);
			break;
		}

		equip_info->inlay[index] = 0;
		player->del_item(need_item_id, need_item_num, MAGIC_TYPE_EQUIP_DRILL);
	} while(0);

	EquipDrillAnswer resp;
	equip_drill_answer__init(&resp);

	resp.result = ret;
	resp.type = type;
	resp.index = index + 1;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_DRILL_ANSWER, equip_drill_answer__pack, resp);

	return 0;
}

//装备镶嵌请求
static int handle_equip_inlay_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	EquipInlayRequest *req = equip_inlay_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;
	uint32_t index = req->index - 1;
	uint32_t gem_id = req->gemid;

	equip_inlay_request__free_unpacked(req, NULL);

	int ret = 0;
	EquipInfo *equip_info = NULL;
	uint32_t inlay_gem = gem_id;
	do
	{
		equip_info = player->get_equip(type);
		if (!equip_info || equip_info->stair == 0)
		{
			ret = ERROR_ID_EQUIP_NOT_ACTIVE;
			LOG_ERR("[%s:%d] player[%lu] get equip failed, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		if (index >= MAX_EQUIP_INLAY_NUM)
		{
			ret = ERROR_ID_EQUIP_HOLE_INDEX;
			LOG_ERR("[%s:%d] player[%lu] hole index error, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index);
			break;
		}

		if (equip_info->inlay[index] == -1)
		{
			ret = ERROR_ID_EQUIP_HOLE_NOT_OPEN;
			LOG_ERR("[%s:%d] player[%lu] hole not open, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index);
			break;
		}

		uint32_t player_job = player->get_job();
		EquipLock *lock_config = get_equip_lock_config(player_job, type);
		if (!lock_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get lock config failed, type:%u, job:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, player_job);
			break;
		}

		GemAttribute *gem_config = get_gem_config(gem_id);
		if (!gem_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get gem config failed, gem_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, gem_id);
			break;
		}

//		bool bTypeFit = false;
//		for (uint32_t i = 0; i < lock_config->n_MosaicType; ++i)
//		{
//			if (lock_config->MosaicType[i] == gem_config->GemType)
//			{
//				bTypeFit = true;
//				break;
//			}
//		}

		if (gem_config->GemType != lock_config->MosaicType[index])
		{
			ret = ERROR_ID_EQUIP_GEM_TYPE_NOT_FIT;
			LOG_ERR("[%s:%d] player[%lu] gem type error, type:%u, index:%u, gem_id:%u, gem_type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, gem_id, (uint32_t)gem_config->GemType);
			break;
		}

		uint32_t need_gem_num = 1;
		uint32_t bag_gem_num = player->get_item_num_by_id(gem_id);
		if (bag_gem_num < need_gem_num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] gem not enough, type:%u, index:%u, gem_id:%u, need_num:%u, bag_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, gem_id, need_gem_num, bag_gem_num);
			break;
		}

		//剥离消耗
		uint32_t need_coin = 0;
		if (equip_info->inlay[index] > 0)
		{
			need_coin = sg_gem_strip_coin;
			uint32_t player_coin = player->get_coin();
			if (player_coin < need_coin)
			{
				ret = ERROR_ID_COIN_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] coin not enough, type:%u, index:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, need_coin, player_coin);
				break;
			}
		
			ret = player->check_can_add_item(equip_info->inlay[index], 1, NULL);
			if (ret != 0)
			{
				LOG_ERR("[%s:%d] player[%lu] bag not enough, type:%u, index:%u, item_id:%u, item_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, equip_info->inlay[index], 1);
				break;
			}
		}

		//非绑定宝石变绑定
		if (!item_is_bind(gem_id))
		{
			inlay_gem = get_item_relate_id(gem_id);
			if (inlay_gem == 0)
			{
				ret = ERROR_ID_EQUIP_GEM_NO_BIND_ID;
				LOG_ERR("[%s:%d] player[%lu] can't found bind id, type:%u, index:%u, gem_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, gem_id);
				break;
			}
		}

		//先剥离
		if (equip_info->inlay[index] > 0)
		{
			player->add_item(equip_info->inlay[index], 1, MAGIC_TYPE_EQUIP_STRIP);
			player->sub_coin(need_coin, MAGIC_TYPE_EQUIP_STRIP);
			equip_info->inlay[index] = 0;
			player->add_achievement_progress(ACType_EQUIP_STRIP, 0, 0, 0, 1);
		}

		//后镶嵌
		player->del_item_by_id(gem_id, need_gem_num, MAGIC_TYPE_EQUIP_INLAY);
		equip_info->inlay[index] = inlay_gem;
		player->calculate_attribute(true);
		player->add_task_progress(TCT_EQUIP_INLAY, type, 1);
		player->add_achievement_progress(ACType_EQUIP_INLAY_QULITY_NUM, 0, 0, 0, 1);
	} while(0);

	EquipInlayAnswer resp;
	equip_inlay_answer__init(&resp);

	resp.result = ret;
	resp.type = type;
	resp.index = index + 1;
	resp.gemid = inlay_gem;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_INLAY_ANSWER, equip_inlay_answer__pack, resp);

	return 0;
}

//装备剥离请求
static int handle_equip_strip_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	EquipStripRequest *req = equip_strip_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;
	uint32_t index = req->index - 1;

	equip_strip_request__free_unpacked(req, NULL);

	int ret = 0;
	EquipInfo *equip_info = NULL;
	do
	{
		equip_info = player->get_equip(type);
		if (!equip_info || equip_info->stair == 0)
		{
			ret = ERROR_ID_EQUIP_NOT_ACTIVE;
			LOG_ERR("[%s:%d] player[%lu] get equip failed, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		if (index >= MAX_EQUIP_INLAY_NUM)
		{
			ret = ERROR_ID_EQUIP_HOLE_INDEX;
			LOG_ERR("[%s:%d] player[%lu] hole index error, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index);
			break;
		}

		if (equip_info->inlay[index] <= 0)
		{
			ret = ERROR_ID_EQUIP_HOLE_NO_GEM;
			LOG_ERR("[%s:%d] player[%lu] hole has not gem, type:%u, index:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index);
			break;
		}

		//剥离消耗
		uint32_t need_coin = sg_gem_strip_coin;
		uint32_t player_coin = player->get_coin();
		if (player_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, type:%u, index:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, need_coin, player_coin);
			break;
		}

		ret = player->check_can_add_item(equip_info->inlay[index], 1, NULL);
		if(ret != 0)
		{
			LOG_ERR("[%s:%d] player[%lu] bag not enough, type:%u, index:%u, item_id:%u, item_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, index, equip_info->inlay[index], 1);
			break;
		}

		player->add_item(equip_info->inlay[index], 1, MAGIC_TYPE_EQUIP_STRIP);
		player->sub_coin(need_coin, MAGIC_TYPE_EQUIP_STRIP);
		equip_info->inlay[index] = 0;
		player->calculate_attribute(true);
		player->add_achievement_progress(ACType_EQUIP_STRIP, 0, 0, 0, 1);
	} while(0);

	EquipStripAnswer resp;
	equip_strip_answer__init(&resp);

	resp.result = ret;
	resp.type = type;
	resp.index = index + 1;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_STRIP_ANSWER, equip_strip_answer__pack, resp);

	return 0;
}

//宝石合成请求
static int handle_equip_gem_compose_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	EquipGemComposeRequest *req = equip_gem_compose_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t gem_id = req->gemid;
	uint32_t material_type = req->materialtype;

	equip_gem_compose_request__free_unpacked(req, NULL);

	int ret = 0;
	uint32_t product_id = 0;
	do
	{
		GemAttribute *gem_config = get_gem_config(gem_id);
		if (!gem_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get gem config failed, gem_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, gem_id);
			break;
		}

		uint32_t material_id = gem_config->GemSynthetic;
		if (material_id == 0)
		{
			ret = ERROR_ID_GEM_CAN_NOT_COMPOSE;
			LOG_ERR("[%s:%d] player[%lu] gem can't compose, gem_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, gem_id);
			break;
		}

		uint32_t gem_bind = 0, gem_unbind = 0;
		uint32_t material_bind = 0, material_unbind = 0;
		if (item_is_bind(gem_id))
		{
			gem_bind = gem_id;
			gem_unbind = get_item_relate_id(gem_id);
		}
		else
		{
			gem_bind = get_item_relate_id(gem_id);
			gem_unbind = gem_id;
		}

		if (item_is_bind(material_id))
		{
			material_bind = material_id;
			material_unbind = get_item_relate_id(material_id);
		}
		else
		{
			material_bind = get_item_relate_id(material_id);
			material_unbind = material_id;
		}

		if (gem_bind == 0 || gem_unbind == 0 || material_bind == 0 || material_unbind == 0)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu], gem_bind:%u, gem_unbind:%u, material_bind:%u, material_unbind:%u", __FUNCTION__, __LINE__, extern_data->player_id, gem_bind, gem_unbind, material_bind, material_unbind);
			break;
		}

		uint32_t need_material_num = gem_config->Number;
		std::map<uint32_t, uint32_t> material_map;
		uint32_t material_bind_num = player->get_item_num_by_id(material_bind);
		uint32_t material_unbind_num = player->get_item_num_by_id(material_unbind);
		switch(material_type)
		{
			case 1:
				{
					if (material_bind_num < need_material_num)
					{
						ret = ERROR_ID_GEM_COMPOSE_MATERIAL_NOT_ENOUGH;
						LOG_ERR("[%s:%d] player[%lu] need_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, material_bind, need_material_num, material_bind_num);
						break;
					}

					product_id = gem_bind;
					material_map[material_bind] = need_material_num;
				}
				break;
			case 2:
				{
					if (material_unbind_num < need_material_num)
					{
						ret = ERROR_ID_GEM_COMPOSE_MATERIAL_NOT_ENOUGH;
						LOG_ERR("[%s:%d] player[%lu] need_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, material_unbind, need_material_num, material_unbind_num);
						break;
					}

					product_id = gem_unbind;
					material_map[material_unbind] = need_material_num;
				}
				break;
			case 3:
				{
					if (material_bind_num + material_unbind_num < need_material_num)
					{
						ret = ERROR_ID_GEM_COMPOSE_MATERIAL_NOT_ENOUGH;
						LOG_ERR("[%s:%d] player[%lu] need_num:%u, bind_num:%u, unbind_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, need_material_num, material_bind_num, material_unbind_num);
						break;
					}

					if (material_bind_num > 0)
					{
						product_id = gem_bind;
						if (material_bind_num < need_material_num)
						{
							material_map[material_bind] = material_bind_num;
							material_map[material_unbind] = need_material_num - material_bind_num;
						}
						else
						{
							material_map[material_bind] = need_material_num;
						}
					}
					else
					{
						product_id = gem_unbind;
						material_map[material_unbind] = need_material_num;
					}
				}
				break;
		}

		if (product_id == 0)
		{
			if (ret == 0)
			{
				ret = ERROR_ID_GEM_COMPOSE_MATERIAL_TYPE;
			}
			break;
		}

		uint32_t product_num = 1;
		ret = player->check_can_add_item(product_id, product_num, NULL);
		if(ret != 0)
		{
			LOG_ERR("[%s:%d] player[%lu] bag not enough, item_id:%u, item_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, product_id, product_num);
			break;
		}

		uint32_t need_coin = gem_config->Consumption;
		uint32_t player_coin = player->get_attr(PLAYER_ATTR_COIN);
		if (player_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, need_coin:%u, has_coin:%u", __FUNCTION__, __LINE__, extern_data->player_id, need_coin, player_coin);
			break;
		}

		player->sub_coin(need_coin, MAGIC_TYPE_GEM_COMPOSE);
		for (std::map<uint32_t, uint32_t>::iterator iter = material_map.begin(); iter != material_map.end(); ++iter)
		{
			player->del_item_by_id(iter->first, iter->second, MAGIC_TYPE_GEM_COMPOSE);
		}

		player->add_item(product_id, product_num, MAGIC_TYPE_GEM_COMPOSE);
		player->add_task_progress(TCT_GEM_COMPOSE, 0, 1);
		player->add_achievement_progress(ACType_EQUIP_GEM_COMPOSE, gem_config->Level, 0, 0, 1);
	} while(0);

	EquipGemComposeAnswer resp;
	equip_gem_compose_answer__init(&resp);

	resp.result = ret;
	resp.gemid = product_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_GEM_COMPOSE_ANSWER, equip_gem_compose_answer__pack, resp);

	return 0;
}

//宝石一键合成请求
static int handle_equip_gem_onekey_compose_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	EquipGemOnekeyComposeRequest *req = equip_gem_onekey_compose_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t gem_id = req->gemid;
	uint32_t material_type = req->materialtype;

	equip_gem_onekey_compose_request__free_unpacked(req, NULL);

	int ret = 0;
	std::map<uint32_t, uint32_t> product_map;
	do
	{
		GemAttribute *gem_config = get_gem_config(gem_id);
		if (!gem_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get gem config failed, gem_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, gem_id);
			break;
		}

		uint32_t material_id = gem_config->GemSynthetic;
		if (material_id == 0)
		{
			ret = ERROR_ID_GEM_CAN_NOT_COMPOSE;
			LOG_ERR("[%s:%d] player[%lu] gem can't compose, gem_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, gem_id);
			break;
		}

		uint32_t gem_bind = 0, gem_unbind = 0;
		uint32_t material_bind = 0, material_unbind = 0;
		if (item_is_bind(gem_id))
		{
			gem_bind = gem_id;
			gem_unbind = get_item_relate_id(gem_id);
		}
		else
		{
			gem_bind = get_item_relate_id(gem_id);
			gem_unbind = gem_id;
		}

		if (item_is_bind(material_id))
		{
			material_bind = material_id;
			material_unbind = get_item_relate_id(material_id);
		}
		else
		{
			material_bind = get_item_relate_id(material_id);
			material_unbind = material_id;
		}

		if (gem_bind == 0 || gem_unbind == 0 || material_bind == 0 || material_unbind == 0)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu], gem_bind:%u, gem_unbind:%u, material_bind:%u, material_unbind:%u", __FUNCTION__, __LINE__, extern_data->player_id, gem_bind, gem_unbind, material_bind, material_unbind);
			break;
		}

		uint32_t coin_per_time = gem_config->Consumption;
		uint32_t player_coin = player->get_attr(PLAYER_ATTR_COIN);
		if (player_coin < coin_per_time)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, need_coin:%u, has_coin:%u", __FUNCTION__, __LINE__, extern_data->player_id, coin_per_time, player_coin);
			break;
		}

		uint32_t material_num_per_time = gem_config->Number;
		std::map<uint32_t, uint32_t> material_map;
		uint32_t material_bind_num = player->get_item_num_by_id(material_bind);
		uint32_t material_unbind_num = player->get_item_num_by_id(material_unbind);

		uint32_t total_product_num = 0;
		if (material_bind_num >= material_num_per_time || material_unbind_num >= material_num_per_time)
		{ //只合成同类材料的
			uint32_t coin_can_num = player_coin / coin_per_time;
			if (material_type == 1)
			{ //优先合成绑定
				if (material_bind_num >= material_num_per_time)
				{
					uint32_t product_bind_num = material_bind_num / material_num_per_time;
					if (coin_can_num > 0)
					{
						product_bind_num = std::min(product_bind_num, coin_can_num);
						material_map.insert(std::make_pair(material_bind, material_num_per_time * product_bind_num));
						product_map.insert(std::make_pair(gem_bind, product_bind_num));
						coin_can_num -= product_bind_num;
						total_product_num += product_bind_num;
					}
				}
				if (material_unbind_num >= material_num_per_time)
				{
					uint32_t product_unbind_num = material_unbind_num / material_num_per_time;
					if (coin_can_num > 0)
					{
						product_unbind_num = std::min(product_unbind_num, coin_can_num);
						material_map.insert(std::make_pair(material_unbind, material_num_per_time * product_unbind_num));
						product_map.insert(std::make_pair(gem_unbind, product_unbind_num));
						coin_can_num -= product_unbind_num;
						total_product_num += product_unbind_num;
					}
				}
			}
			else
			{ //优先合成非绑
				if (material_unbind_num >= material_num_per_time)
				{
					uint32_t product_unbind_num = material_unbind_num / material_num_per_time;
					if (coin_can_num > 0)
					{
						product_unbind_num = std::min(product_unbind_num, coin_can_num);
						material_map.insert(std::make_pair(material_unbind, material_num_per_time * product_unbind_num));
						product_map.insert(std::make_pair(gem_unbind, product_unbind_num));
						coin_can_num -= product_unbind_num;
						total_product_num += product_unbind_num;
					}
				}
				if (material_bind_num >= material_num_per_time)
				{
					uint32_t product_bind_num = material_bind_num / material_num_per_time;
					if (coin_can_num > 0)
					{
						product_bind_num = std::min(product_bind_num, coin_can_num);
						material_map.insert(std::make_pair(material_bind, material_num_per_time * product_bind_num));
						product_map.insert(std::make_pair(gem_bind, product_bind_num));
						coin_can_num -= product_bind_num;
						total_product_num += product_bind_num;
					}
				}
			}
		}
		else
		{ //只合成混合材料的
			if (material_bind_num + material_unbind_num < material_num_per_time)
			{
				ret = ERROR_ID_GEM_COMPOSE_MATERIAL_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] need_num:%u, bind_num:%u, unbind_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, material_num_per_time, material_bind_num, material_unbind_num);
				break;
			}

			material_map[material_bind] = material_bind_num;
			material_map[material_unbind] = material_num_per_time - material_bind_num;
			product_map[gem_bind] = 1;
			total_product_num = 1;
		}

		if (!player->check_can_add_item_list(product_map))
		{
			ret = ERROR_ID_BAG_GRID_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] bag not enough", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}


		player->sub_coin(coin_per_time * total_product_num, MAGIC_TYPE_GEM_COMPOSE);
		for (std::map<uint32_t, uint32_t>::iterator iter = material_map.begin(); iter != material_map.end(); ++iter)
		{
			player->del_item_by_id(iter->first, iter->second, MAGIC_TYPE_GEM_COMPOSE);
		}

		player->add_item_list(product_map, MAGIC_TYPE_GEM_COMPOSE);
		player->add_task_progress(TCT_GEM_COMPOSE, 0, total_product_num);
	} while(0);

	EquipGemOnekeyComposeAnswer resp;
	equip_gem_onekey_compose_answer__init(&resp);

	ItemData  product_data[2];
	ItemData* product_point[2];

	resp.result = ret;
	resp.gems = product_point;
	resp.n_gems = 0;
	for (std::map<uint32_t, uint32_t>::iterator iter = product_map.begin(); iter != product_map.end() && resp.n_gems < 2; iter++)
	{
		product_point[resp.n_gems] = &product_data[resp.n_gems];
		item_data__init(&product_data[resp.n_gems]);
		product_data[resp.n_gems].id = iter->first;
		product_data[resp.n_gems].num = iter->second;
		resp.n_gems++;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_EQUIP_GEM_ONEKEY_COMPOSE_ANSWER, equip_gem_onekey_compose_answer__pack, resp);

	return 0;
}

//邮件附件发放请求
static int handle_mail_give_attach_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	MailGiveAttachRequest *req = mail_give_attach_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	std::vector<uint64_t> success_ids;
	std::vector<uint64_t> fail_ids;
	for (size_t i = 0; i < req->n_mails; ++i)
	{
		MailAttachGiveInfo *give_info = req->mails[i];
		std::map<uint32_t, uint32_t> attach_map;
		for (size_t j = 0; j < give_info->n_attachs; ++j)
		{
			attach_map[give_info->attachs[j]->id] += give_info->attachs[j]->num;
		}

		if (player->add_item_list(attach_map, give_info->statisid, true))
		{
			success_ids.push_back(give_info->mailid);
		}
		else
		{
			fail_ids.push_back(give_info->mailid);
		}
	}

	mail_give_attach_request__free_unpacked(req, NULL);

	MailGiveAttachAnswer resp;
	mail_give_attach_answer__init(&resp);

	resp.successids = &success_ids[0];
	resp.n_successids = success_ids.size();
	resp.failids = &fail_ids[0];
	resp.n_failids = fail_ids.size();

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_MAIL_GIVE_ATTACH_ANSWER, mail_give_attach_answer__pack, resp);

	return 0;
}

static int handle_buy_fashion_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	BuyFashion *req = buy_fashion__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int shopid = req->id;
	int ind = req->during;
	buy_fashion__free_unpacked(req, NULL);

	std::map<uint64_t, struct ActorFashionTable *>::iterator it = fashion_config.find(shopid);
	if (it == fashion_config.end())
	{
		return 3;
	}
	if (ind < 0 || ind > 2)
	{
		return 4;
	}

	BuyFashionAnswer send;
	buy_fashion_answer__init(&send);
	FashionData data;
	send.data = &data;
	send.ret = 0;
	fashion_data__init(&data);
	if (it->second->UnlockType == 1)
	{
		if (player->get_comm_gold() < (int)it->second->WingBinding[ind])
		{
			send.ret = 190400005;
			fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BUY_FASHION_ANSWER, buy_fashion_answer__pack, send);
			return 5;
		}
	}
	else if (it->second->UnlockType == 2)
	{
		if (player->del_item(it->second->UnlockEffect1, it->second->UnlockEffect2, MAGIC_TYPE_FASHION) < 0)
		{
			return 7;
		}
	}
	else
	{
		return 6;
	}
	
	int ret = player->add_fashion(shopid, it->second->Colour, it->second->Time[ind]);
	if (ret >= 0)
	{
		pack_fashion_info(data, player, ret);
		player->sub_comm_gold(it->second->WingBinding[ind], MAGIC_TYPE_FASHION);
	}
	else
	{
		send.ret = 190300008; //时装包裹满了
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BUY_FASHION_ANSWER, buy_fashion_answer__pack, send);

	return 0;
}
static int handle_unlock_color_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	//UnlockColor *req = unlock_color__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	//if (!req)
	//{
	//	LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
	//	return (-10);
	//}

	//unlock_color__free_unpacked(req, NULL);
	std::vector<std::map<uint64_t, struct ColourTable*>::iterator> vcolor;
	int color = 0;
	std::map<uint64_t, struct ColourTable*>::iterator it = color_table_config.begin();
	for (;it != color_table_config.end(); ++it)
	{
		if (player->get_color(it->first) < 0)
		{
			vcolor.push_back(it);
		}
	}
	UnlockColorAns send;
	unlock_color_ans__init(&send);
	if (vcolor.empty())
	{
		send.ret = 190500087;
		goto done;
	}
	color = vcolor[rand()%vcolor.size()]->first;

	send.ret = 0;
	if (player->del_item(vcolor[rand() % vcolor.size()]->second->OpenColourItem, vcolor[rand() % vcolor.size()]->second->OpenColourNum, MAGIC_TYPE_FASHION) < 0)
	{
		send.ret = 190500063;
		goto done;
	}

	player->unlock_color(color);

done:
	send.color = color;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_UNLOCK_COLOR_ANSWER, unlock_color_ans__pack, send);


	return 0;
}
static int handle_unlock_weapon_color_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	UnlockColor *req = unlock_color__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t color = req->color;
	unlock_color__free_unpacked(req, NULL);

	WeaponsEffectTable *table = get_config_by_id(color, &weapon_color_config);
	if (table == NULL)
	{
		return 2;
	}
	EquipInfo *equ = player->get_equip(ET_WEAPON);
	if (equ == NULL)
	{
		return 3;
	}
	EquipStarLv *tableEqu = get_config_by_id(table->Requirement2, &equip_star_config);
	if (tableEqu == NULL)
	{
		return 4;
	}

	UnlockColorAns send;
	unlock_color_ans__init(&send);
	send.ret = 0;
	if (player->get_attr(PLAYER_ATTR_LEVEL) < table->Requirement1)
	{
		send.ret = 190500064;
		goto done;
	}

	if (equ->stair < tableEqu->Quality || equ->star_lv < tableEqu->StarLv)
	{
		send.ret = 190500063;
		goto done;
	}
	if (table->Item > 0 && player->del_item(table->Item, table->ItemNum , MAGIC_TYPE_FASHION) < 0)
	{
		send.ret = 190500063;
		goto done;
	}
	
	player->unlock_weapon_color(color);

done:
	send.color = color;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_UNLOCK_WEAPON_COLOR_ANSWER, unlock_color_ans__pack, send);

	

	return 0;
}
static int handle_set_fashion_color_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	SetFashionColor *req = set_fashion_color__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t fashoinid = req->id;
	uint32_t color = req->color;
	bool isdown = req->isdown;
	set_fashion_color__free_unpacked(req, NULL);

	if (player->get_color(color) < 0)
	{
		return 1;
	}

	SetFashionColorAnswer send;
	set_fashion_color_answer__init(&send);
	send.ret = 0;
	std::map<uint64_t, struct ColourTable*>::iterator it = color_table_config.find(color);
	AttrMap attrs;
	std::map<uint64_t, struct ActorFashionTable *>::iterator itFashion = fashion_config.find(fashoinid);
	ParameterTable *param_config = get_config_by_id(161000025, &parameter_config);
	int i = player->get_fashion(fashoinid);
	if (i < 0)
	{
		send.ret = 190500088; //过期
		goto done;
	}
	if (player->data->fashion[i].timeout != 0
		&& player->data->fashion[i].timeout <= (time_t)time_helper::get_cached_time() / 1000)
	{
		send.ret = 190500088; //过期
		goto done;
	}

	if (itFashion == fashion_config.end())
	{
		return 4;
	}
	if (itFashion->second->Type == FASHION_TYPE_WEAPON)
	{
		return 5;
	}

	if (it == color_table_config.end())
	{
		return 6;
	}


	if (param_config == NULL)
	{
		return 8;
	}
	if (player->del_item(param_config->parameter1[0], param_config->parameter1[1], MAGIC_TYPE_FASHION) < 0)
	{
		return 7;
	}

	//if (player->del_item(it->second->ColourItem, it->second->ColourNum, MAGIC_TYPE_FASHION) < 0)
	//{
	//	return 7;
	//}


	if (isdown)
	{
		player->data->fashion[i].colordown = color;
		player->set_attr(PLAYER_ATTR_CLOTHES_COLOR_DOWN, player->data->fashion[i].colordown);
		attrs[PLAYER_ATTR_CLOTHES_COLOR_DOWN] = player->data->fashion[i].colordown;
	}
	else
	{
		player->data->fashion[i].color = color;
		if (itFashion->second->Type == FASHION_TYPE_CLOTHES)
		{
			player->set_attr(PLAYER_ATTR_CLOTHES_COLOR_UP, player->data->fashion[i].color);
			attrs[PLAYER_ATTR_CLOTHES_COLOR_UP] = player->data->fashion[i].color;
		}
		else
		{
			player->set_attr(PLAYER_ATTR_HAT_COLOR, player->data->fashion[i].color);
			attrs[PLAYER_ATTR_HAT_COLOR] = player->data->fashion[i].color;;
		}
	}
	player->notify_attr(attrs, true, true);
	player->add_achievement_progress(ACType_FASHION_COLOR, 0, 0, 0, 1);
done:
	send.id = fashoinid;
	send.color = color;
	send.isdown = isdown;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_FASHION_COLOR_ANSWER, set_fashion_color_answer__pack, send);

	return 0;
}
static int handle_set_weapon_color_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	SetFashionColor *req = set_fashion_color__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t fashoinid = req->id;
	uint32_t color = req->color;
	set_fashion_color__free_unpacked(req, NULL);

	if (player->get_weapon_color(color) < 0)
	{
		return 1;
	}
	WeaponsEffectTable *table = get_config_by_id(color, &weapon_color_config);
	if (table == NULL)
	{
		return 2;
	}
	
	SetFashionColorAnswer send;
	set_fashion_color_answer__init(&send);
	send.ret = 0;
	std::map<uint64_t, struct ActorFashionTable *>::iterator itFashion = fashion_config.find(fashoinid);
//	std::map<uint64_t, struct ColourTable*>::iterator it = color_table_config.find(color);
	AttrMap attrs;
	int i = player->get_fashion(fashoinid);
	if (i < 0)
	{
		send.ret = 190500088; //过期
		goto done;
	}
	if (player->data->fashion[i].timeout != 0
		&& player->data->fashion[i].timeout <= (time_t)time_helper::get_cached_time() / 1000)
	{
		send.ret = 190500088; //过期
		goto done;
	}

	if (itFashion->second->Type != FASHION_TYPE_WEAPON)
	{
		return 5;
	}

	if (player->del_item(table->Item, table->ItemNum, MAGIC_TYPE_FASHION) < 0)
	{
		send.ret = 190400006;
		goto done;
	}

	player->data->fashion[i].color = color;
	player->set_attr(PLAYER_ATTR_WEAPON_COLOR, player->data->fashion[i].color);
	attrs[PLAYER_ATTR_WEAPON_COLOR] = player->data->fashion[i].color;;

	player->notify_attr(attrs, true, true);
	player->add_achievement_progress(ACType_FASHION_COLOR, 0, 0, 0, 1);
done:
	send.id = fashoinid;
	send.color = color;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_FASHION_COLOR_ANSWER, set_fashion_color_answer__pack, send);


	return 0;
}
static int  handle_puton_fashion_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	PutonFashion *req = puton_fashion__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t fashionId = req->id;
	puton_fashion__free_unpacked(req, NULL);

	int index = player->get_fashion(fashionId);
	if (index < 0)
	{
		return 2;
	}
	std::map<uint64_t, struct ActorFashionTable *>::iterator itFashion = fashion_config.find(fashionId);
	if (itFashion == fashion_config.end())
	{
		return 4;
	}
	AttrMap attrs;
	int resType = PLAYER_ATTR_CLOTHES;
	int colorType = PLAYER_ATTR_CLOTHES_COLOR_UP;
	PutonFashionAns send;
	puton_fashion_ans__init(&send);
	if (player->data->fashion[index].timeout != 0
		&& player->data->fashion[index].timeout <= (time_t)time_helper::get_cached_time() / 1000)
	{
		send.ret = 190500088;
		goto done; //过期
	}

	if (itFashion->second->Type == FASHION_TYPE_WEAPON)
	{
		resType = PLAYER_ATTR_WEAPON;
	}
	else if (itFashion->second->Type == FASHION_TYPE_HAT)
	{
		resType = PLAYER_ATTR_HAT;
		colorType = PLAYER_ATTR_HAT_COLOR;
	}

	send.newid = player->get_attr(resType);
	send.ret = 0;
	send.id = fashionId;


	player->set_attr(resType, fashionId);

	attrs[resType] = fashionId;
	if (resType != PLAYER_ATTR_WEAPON)
	{
		player->set_attr(colorType, player->data->fashion[index].color);
		attrs[colorType] = player->data->fashion[index].color;
	}
	if (resType == PLAYER_ATTR_CLOTHES)
	{
		player->set_attr(PLAYER_ATTR_CLOTHES_COLOR_DOWN, player->data->fashion[index].colordown);
		attrs[PLAYER_ATTR_CLOTHES_COLOR_DOWN] = player->data->fashion[index].colordown;
	}
	player->notify_attr(attrs, true, true);
	player->add_task_progress(TCT_FASHION_WEAR, fashionId, 1);
done:
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PUTON_FASHION_ANSWER, puton_fashion_ans__pack, send);

	return 0;
}
static int  handle_takeoff_fashion_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	PutonFashion *req = puton_fashion__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t fashionId = req->id;
	puton_fashion__free_unpacked(req, NULL);

	if (player->get_fashion(fashionId) < 0)
	{
		return 2;
	}
	std::map<uint64_t, struct ActorFashionTable *>::iterator itFashion = fashion_config.find(fashionId);
	if (itFashion == fashion_config.end())
	{
		return 3;
	}

	//换默认
	uint32_t carrerid = 101000000 + player->get_job();
	std::map<uint64_t, struct ActorTable *>::iterator it = actor_config.find(carrerid);
	if (it == actor_config.end())
	{
		return 5;
	}

	AttrMap attrs;
	PutonFashionAns send;
	puton_fashion_ans__init(&send);
	if (itFashion->second->Type == FASHION_TYPE_WEAPON)
	{
		player->set_attr(PLAYER_ATTR_WEAPON, it->second->WeaponId);
		attrs[PLAYER_ATTR_WEAPON] = it->second->WeaponId;
		send.newid = it->second->WeaponId;
	}
	else if (itFashion->second->Type == FASHION_TYPE_HAT)
	{
//		if (it->second->n_HairResId > 0)
		{
			player->set_attr(PLAYER_ATTR_HAT, player->data->default_hair);
			attrs[PLAYER_ATTR_HAT] = player->data->default_hair;
			send.newid = it->second->HairResId[0];
		}
//		std::map<uint64_t, struct ActorFashionTable *>::iterator itFNew = fashion_config.find(attrs[PLAYER_ATTR_HAT]);
//		if (itFNew != fashion_config.end())
		{
//			if (itFNew->second->n_ColourID1 > 0)
			{
				player->set_attr(PLAYER_ATTR_HAT_COLOR, player->data->default_hair_color);
				attrs[PLAYER_ATTR_HAT_COLOR] = player->data->default_hair_color;
			}
		}
	}
	else if (itFashion->second->Type == FASHION_TYPE_CLOTHES)
	{
		if (it->second->n_ResId > 0)
		{
			player->set_attr(PLAYER_ATTR_CLOTHES, it->second->ResId[0]);
			attrs[PLAYER_ATTR_CLOTHES] = it->second->ResId[0];
			send.newid = it->second->ResId[0];
		}
//		std::map<uint64_t, struct ActorFashionTable *>::iterator itFNew = fashion_config.find(it->second->ResId[0]);
//		if (itFNew != fashion_config.end())
		{
//			if (itFNew->second->n_ColourID1 > 0)
			{
				player->set_attr(PLAYER_ATTR_CLOTHES_COLOR_UP, player->data->default_up_color);
				attrs[PLAYER_ATTR_CLOTHES_COLOR_UP] = player->data->default_up_color;
			}
//			if (itFNew->second->n_ColourID2 > 0)
			{
				player->set_attr(PLAYER_ATTR_CLOTHES_COLOR_DOWN, player->data->default_down_color);
				attrs[PLAYER_ATTR_CLOTHES_COLOR_DOWN] = player->data->default_down_color;
			}
		}
	}
	player->notify_attr(attrs, true, true);
	send.ret = 0;
	send.id = fashionId;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TAKEOFF_FASHION_ANSWER, puton_fashion_ans__pack, send);

	return 0;
}
static int handle_set_color_old_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	UnlockColor *req = unlock_color__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	int index = player->get_color(req->color);
	if (index >= 0)
	{
		player->data->color_is_new[index] = false;
	}

	UnlockColorAns send;
	unlock_color_ans__init(&send);
	send.ret = 0;
	send.color = req->color;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_COLOR_OLD_ANSWER, unlock_color_ans__pack, send);

	unlock_color__free_unpacked(req, NULL);
	return 0;
}
static int handle_set_weapon_color_old_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	UnlockColor *req = unlock_color__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	int index = player->get_weapon_color(req->color);
	if (index >= 0)
	{
		player->data->weapon_color_is_new[index] = false;
	}

	UnlockColorAns send;
	unlock_color_ans__init(&send);
	send.ret = 0;
	send.color = req->color;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_WEAPON_COLOR_OLD_ANSWER, unlock_color_ans__pack, send);

	unlock_color__free_unpacked(req, NULL);
	return 0;
}
static int handle_set_fashion_old_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	PutonFashion *req = puton_fashion__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t fashionId = req->id;
	puton_fashion__free_unpacked(req, NULL);

	int index = player->get_fashion(fashionId);
	if (index < 0)
	{
		return 2;
	}

	player->set_fashion_old(fashionId);

	PutonFashionAns send;
	puton_fashion_ans__init(&send);
	send.ret = 0;
	send.id = fashionId;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_FASHION_OLD_ANSWER, puton_fashion_ans__pack, send);


	return 0;
}
static int set_cur_horse(player_struct *player, EXTERN_DATA *extern_data, uint32_t horseid)
{
	int i = player->get_horse(horseid);
	if (i < 0)
	{
		return -2;
	}

	int oldHorse = player->get_attr(PLAYER_ATTR_CUR_HORSE);
	SetCurHorseAns send;
	set_cur_horse_ans__init(&send);
	send.ret = 0;
	if ((uint64_t)player->data->horse[i].timeout != 0 && time_helper::get_cached_time() / 1000 >= (uint64_t)player->data->horse[i].timeout)
	{
		send.ret = 190500096;
		goto done;
	}

	player->set_attr(PLAYER_ATTR_CUR_HORSE, horseid);
	player->data->horse_attr.soul_star = player->data->horse[i].star;
	player->data->horse_attr.soul_step = player->data->horse[i].step;

	send.id = horseid;
	player->calc_horse_attr();
	send.power = player->data->horse_attr.power;
	send.playerid = player->get_uuid();
	player->calculate_attribute(true);
	send.old_id = oldHorse;
	send.step = player->data->horse_attr.soul_step;
	send.star = player->data->horse_attr.soul_star;
	player->add_achievement_progress(ACType_HORSE_SET_CUR, 0, 0, 0, 1);
done:
	player->broadcast_to_sight(MSG_ID_SET_CUR_HORSE_ANSWER, &send, (pack_func)set_cur_horse_ans__pack, true);

	return 0;
}
static int handle_set_cur_horse_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	HorseId *req = horse_id__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t horseid = req->id;
	horse_id__free_unpacked(req, NULL);

	return set_cur_horse(player, extern_data, horseid);
}

static int handle_buy_horse_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	BuyHorse *req = buy_horse__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t horseId = req->id;
	uint32_t shopid = req->shopid;
	int type = req->type;
	buy_horse__free_unpacked(req, NULL);

	std::map<uint64_t, struct MountsTable *>::iterator it = horse_config.find(horseId);
	if (it == horse_config.end())
	{
		return 3;
	}
	if (shopid < 0 || shopid > 2)
	{
		return 4;
	}
	
	if (shopid >= it->second->n_Time)
	{
		return 5;
	}

	int ret = 0;
	if (type == 2)
	{
		if (shopid >= it->second->n_Item || shopid >= it->second->n_ItemNum ||
			player->del_item(it->second->Item[shopid], it->second->ItemNum[shopid], MAGIC_TYPE_HORSE) < 0)
		{
			ret = 190400006;
		}
	} 
	else if (type == 1)
	{
		if (shopid >= it->second->n_Binding ||
			player->sub_unbind_gold(it->second->Binding[shopid], MAGIC_TYPE_HORSE) < 0)
		{
			ret = 190400005;
		}
	}
	else
	{
		return 6;
	}
	if (ret == 0)
	{
		player->add_horse(horseId, it->second->Time[shopid]);
	}
	send_comm_answer(MSG_ID_BUY_HORSE_ANSWER, ret, extern_data);
	if (ret == 0)
	{
		//player->data->horse_attr.old = horseId;
		set_cur_horse(player, extern_data, horseId);
	}

	return 0;
}
static int handle_down_horse_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	player->down_horse();

	return 0;
}
static int calc_add_horse_exp(player_struct *player, int num, uint32_t ind, int &ret)
{
	std::map<uint64_t, struct SpiritTable*>::iterator it = spirit_config.find(player->data->horse_attr.step);
	if (it == spirit_config.end())
	{
		return 1;
	}

	if (player->data->horse_attr.attr_exp[ind] >= it->second->GradeNum[ind])
	{
		return 190500367;
	}
	//if (player->data->horse_attr.step >= spirit_config.size())
	//{
	//	return 2;
	//}
	//if (player->get_attr(PLAYER_ATTR_LEVEL) < it->second->Level)
	//{
	//	return 190500094;
	//}
	ret = 0;
	for (int tmp = 1; tmp <= num || num == 0; ++tmp)
	{
		if (player->data->horse_attr.attr_exp[ind] + tmp > it->second->GradeNum[ind])
		{
			break;
		}
		if (player->del_item(it->second->SpiritExpend, it->second->ExpendNum, MAGIC_TYPE_HORSE) < 0)
		{
			 break;
		}
		++ret;
	}

	if (ret == 0)
	{
		return 190500063;  //
	}
	else
	{
		return 0;
	}

}
static int handle_add_horse_exp_request(player_struct *player, EXTERN_DATA *extern_data) //修灵
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	HorseAttr *req = horse_attr__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t num = req->type;
	uint32_t attid = req->arrtid;
	horse_attr__free_unpacked(req, NULL);

	uint32_t i = 0;
	for (; i < MAX_HORSE_ATTR_NUM; ++i)
	{
		if (player->data->horse_attr.attr[i] == attid)
		{
			break;
		}
	}
	if (i == MAX_HORSE_ATTR_NUM)
	{
		return -11;
	}

	HorseAttrAns send;
	horse_attr_ans__init(&send);
	int add = 0;
	send.ret = calc_add_horse_exp(player, num, i, add);
	if (send.ret != 0)
	{
		goto done;
	}

	player->data->horse_attr.attr_exp[i] += add;
	player->add_task_progress(TCT_HORSE_ADD_EXP, 0, add);
	player->add_achievement_progress(ACType_HORSE_ADD_EXP, 0, 0, 0, add);

done:
	send.arrtid = attid;
	send.num = player->data->horse_attr.attr_exp[i];
	player->calc_horse_attr();
	send.power = player->data->horse_attr.power;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ADD_HORSE_EXP_ANSWER, horse_attr_ans__pack, send);
	player->calculate_attribute(true);
	return 0;
}
static int check_horse_exp_full(player_struct *player)
{
	std::map<uint64_t, struct SpiritTable*>::iterator it = spirit_config.find(player->data->horse_attr.step);
	if (it == spirit_config.end())
	{
		return 1;
	}
	if (player->get_attr(PLAYER_ATTR_LEVEL) < it->second->Level)
	{
		return 2;
	}
	if (player->data->horse_attr.step == spirit_config.size())
	{
		return 3;
	}

	for (int i = 0; i < MAX_HORSE_ATTR_NUM; ++i)
	{
		if (player->data->horse_attr.attr_exp[i] < it->second->GradeNum[i])
		{
			return 4;
		}
	}
	return 0;
}
static int handle_add_horse_step_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}


	HorseStepAns send;
	horse_step_ans__init(&send);
	send.ret = check_horse_exp_full(player);

	if (send.ret != 0)
	{
		goto done;
	}

	++(player->data->horse_attr.step);
	for (int i = 0; i < MAX_HORSE_ATTR_NUM; ++i)
	{
		player->data->horse_attr.attr_exp[i] = 0;
	}
	player->add_achievement_progress(ACType_HORSE_STEP, player->data->horse_attr.step, 0, 0, 1);
done:
	send.step = player->data->horse_attr.step;
	player->calc_horse_attr();
	send.power = player->data->horse_attr.power;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ADD_HORSE_STEP_ANSWER, horse_step_ans__pack, send);
	player->calculate_attribute(true);

	return 0;
}
/*
static int handle_add_horse_soul_level_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	CastSpiritTable *table = get_horse_soul_table(); //坐骑铸灵配置
	if (it == horse_soul_config.end())
	{
		return -2;
	}
	if (player->data->horse_attr.soul == it->second->n_ToddlerLv)
	{
		return -3;//满级
	}

	for (uint32_t i = 1; i <= MAX_HORSE_SOUL; ++i)
	{
		if (player->data->horse_attr.soul_exp[i] < it->second->GradeNum[player->data->horse_attr.soul - 1])
		{
			return -4;
		}
	}
	if (it->second->AdvancedLv[player->data->horse_attr.soul - 1] > player->get_attr(PLAYER_ATTR_LEVEL))
	{
		return 190500092;
	}

	memset(player->data->horse_attr.soul_exp, 0, sizeof(player->data->horse_attr.soul_exp));

	MountsTable *table = get_config_by_id(it->second->AdvancedGet[player->data->horse_attr.soul - 1], &horse_config);
	if (table != NULL)
	{
		player->add_horse(it->second->AdvancedGet[player->data->horse_attr.soul - 1], 0);
	}
	player->data->horse_attr.soul += 1;

	HorseSoulLevelAns send;
	horse_soul_level_ans__init(&send);
	send.ret = 0;
	send.level = player->data->horse_attr.soul;
	player->calc_horse_attr();
	send.power = player->data->horse_attr.power;
	player->data->horse_attr.soul_full = false;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ADD_HORSE_SOUL_LEVEL_ANSWER, horse_soul_level_ans__pack, send);
	player->calculate_attribute(true);

	return 0;
}*/
static int handle_set_horse_old_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	HorseId *req = horse_id__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t horseid = req->id;
	horse_id__free_unpacked(req, NULL);

	int i = player->get_horse(horseid);
	if (i < 0)
	{
		return -2;
	}
	player->data->horse[i].isNew = false;
	HorseId send;
	horse_id__init(&send);
	send.id = horseid;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_HORSE_OLD_ANSWER, horse_id__pack, send);

	return 0;
}
static int handle_add_horse_soul_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	HorseId *req = horse_id__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t horseid = req->id;
	horse_id__free_unpacked(req, NULL);

	int i = player->get_horse(horseid);
	if (i < 0)
	{
		return -12;
	}
	MountsTable *tableHorse = get_config_by_id(player->data->horse[i].id, &horse_config);
	if (tableHorse == NULL)
	{
		return 14;
	}
	if (player->data->horse[i].star == 5 && player->data->horse[i].step == tableHorse->CastSpiritLimit)
	{
		return 13; //满阶
	}
	CastSpiritTable *table = get_horse_soul_table(horseid, player->data->horse[i].step, player->data->horse[i].star + 1); //坐骑铸灵配置
	if (table == NULL)
	{
		return -2;
	}

	HorseSoulAns send;
	horse_soul_ans__init(&send);
	send.ret = 0;
	bool up = false;
	for (uint32_t n = 0; n < table->n_CastExpend; ++n)
	{
		if (player->get_item_num_by_id(table->CastExpend[n]) < (int)table->Expend1Num[n])
		{
			send.ret = 190600003;
			goto done;
		}
	}
	for (uint32_t n = 0; n < table->n_CastExpend; ++n)
	{
		player->del_item(table->CastExpend[n], table->Expend1Num[n], MAGIC_TYPE_HORSE);
	}

	if (player->data->horse[i].star == 4 && player->data->horse[i].step < tableHorse->CastSpiritLimit)
	{
		player->data->horse[i].star = 0;
		++player->data->horse[i].step;
		up = true;
	}
	else
	{
		++player->data->horse[i].star;
	}

	player->add_achievement_progress(ACType_HORSE_ADD_SOUL, 0, 0, 0, 1);
	player->add_task_progress(TCT_HORSE_ADD_SOUL, 0, 1);
done:
	//send.soul = cur;
	player->calc_horse_attr();
	send.power = player->data->horse_attr.power;
	//send.num = player->data->horse_attr.soul_exp[cur];
	//send.cur_soul = player->data->horse_attr.cur_soul;
	//send.soul_full = player->data->horse_attr.soul_full;
	send.step = player->data->horse[i].step;
	send.star = player->data->horse[i].star;
	send.id = player->data->horse[i].id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ADD_HORSE_SOUL_ANSWER, horse_soul_ans__pack, send);
	player->calculate_attribute(true);

	if (player->get_attr(PLAYER_ATTR_CUR_HORSE) == horseid && send.ret == 0)
	{
		player->data->horse_attr.soul_star = player->data->horse[i].star;
		if (up)
		{
			player->data->horse_attr.soul_step = player->data->horse[i].step;
			SetCurHorseAns notify;
			set_cur_horse_ans__init(&notify);
			notify.ret = 0;
			notify.id = horseid;
			notify.power = player->data->horse_attr.power;
			notify.playerid = player->get_uuid();
			notify.old_id = horseid;
			notify.step = player->data->horse_attr.soul_step;
			notify.star = player->data->horse_attr.soul_star;
			player->broadcast_to_sight(MSG_ID_SET_CUR_HORSE_ANSWER, &notify, (pack_func)set_cur_horse_ans__pack, true);
		}
	}

	return 0;
}
static int handle_on_horse_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (!player->scene->can_use_horse())
	{
		LOG_ERR("%s: %lu can not use horse in scene %u", __FUNCTION__, extern_data->player_id, player->scene->m_id);
		return (-10);
	}

	//if (player->is_in_buff3())
	//{
	//	LOG_ERR("%s: %lu can not use horse in buff type3", __FUNCTION__, extern_data->player_id);
	//	return (-20);
	//}

	if (player->data->attrData[PLAYER_ATTR_CUR_HORSE] == 0)
	{
		return 1;
	}
	if (player->data->attrData[PLAYER_ATTR_ON_HORSE_STATE] == 1)
	{
		return 2;
	}
	if (player->data->truck.on_truck)
	{
		return 4;
	}

	OnHorseRequest *req = on_horse_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (3);
	}

	player->interrupt();

	player->data->pos_y = req->pos_y;
	on_horse_request__free_unpacked(req, NULL);

	AttrMap attrs;
	attrs[PLAYER_ATTR_MOVE_SPEED] = player->data->horse_attr.total[PLAYER_ATTR_MOVE_SPEED];
	player->set_attr(PLAYER_ATTR_MOVE_SPEED, player->data->horse_attr.total[PLAYER_ATTR_MOVE_SPEED]);
	player->notify_attr(attrs, true, true);
	player->data->horse_attr.fly = 1;

	player->data->attrData[PLAYER_ATTR_ON_HORSE_STATE] = 1;
	OnHorse send;
	on_horse__init(&send);
	send.playerid = player->get_uuid();
	send.horseid = player->data->attrData[PLAYER_ATTR_CUR_HORSE];
	player->broadcast_to_sight(MSG_ID_ON_HORSE_NOTIFY, &send, (pack_func)on_horse__pack, false);
	player->adjust_battle_partner();

	return 0;
}
static int handle_set_horse_fly_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->data->attrData[PLAYER_ATTR_CUR_HORSE] == 0)
	{
		return 1;
	}
	if (player->data->attrData[PLAYER_ATTR_ON_HORSE_STATE] == 0)
	{
		return 2;
	}

	FlyState *req = fly_state__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (3);
	}
	int fly = req->fly;
	fly_state__free_unpacked(req, NULL);

	player->data->horse_attr.fly = fly;
	AttrMap attrs;
	if (fly == 2)
	{
		attrs[PLAYER_ATTR_MOVE_SPEED] = player->data->horse_attr.total[PLAYER_ATTR_FLY_SPEED];
		player->set_attr(PLAYER_ATTR_MOVE_SPEED, player->data->horse_attr.total[PLAYER_ATTR_FLY_SPEED]);
	}
	else
	{
		attrs[PLAYER_ATTR_MOVE_SPEED] = player->data->horse_attr.total[PLAYER_ATTR_MOVE_SPEED];
		player->set_attr(PLAYER_ATTR_MOVE_SPEED, player->data->horse_attr.total[PLAYER_ATTR_MOVE_SPEED]);
	}

	player->notify_attr(attrs, true, true);

	return 0;
}
static int handle_horse_restory_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	int oldHorse = player->get_attr(PLAYER_ATTR_CUR_HORSE);

	player->set_attr(PLAYER_ATTR_CUR_HORSE, DEFAULT_HORSE);
	player->data->horse_attr.soul_step = player->data->horse[0].step;
	player->data->horse_attr.soul_star = player->data->horse[0].star;
	SetCurHorseAns send;
	set_cur_horse_ans__init(&send);
	send.ret = 0;
	send.id = DEFAULT_HORSE;
	player->calc_horse_attr();
	send.power = player->data->horse_attr.power;
	send.playerid = player->get_uuid();
	player->calculate_attribute(true);
	send.old_id = oldHorse;
	send.step = player->data->horse_attr.soul_step;
	send.star = player->data->horse_attr.soul_star;
	player->broadcast_to_sight(MSG_ID_SET_CUR_HORSE_ANSWER, &send, (pack_func)set_cur_horse_ans__pack, true);
	//player->data->horse_attr.old = 0;

	return 0;
}

//商城信息请求
static int handle_shop_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	uint64_t cur_tick = time_helper::get_cached_time() / 1000;
	std::set<uint64_t> shop_ids;

	ShopInfoAnswer resp;
	shop_info_answer__init(&resp);

	ShopOpenData shop_data[MAX_SHOP_LIST_NUM];
	ShopOpenData* shop_data_point[MAX_SHOP_LIST_NUM];
	GoodsData goods_data[MAX_SHOP_GOODS_NUM];
	GoodsData* goods_data_point[MAX_SHOP_GOODS_NUM];

	resp.n_shops = 0;
	resp.shops = shop_data_point;
	for (std::map<uint64_t, ShopListTable*>::iterator iter = shop_list_config.begin(); iter != shop_list_config.end() && resp.n_shops < MAX_SHOP_LIST_NUM; ++iter)
	{
		ShopListTable *config = iter->second;
		if (config->StartTime == 0 || config->EndTime == 0 ||
				(config->StartTime > 0 && config->EndTime > 0 &&
				 cur_tick >= config->StartTime && cur_tick < config->EndTime))
		{
			shop_data_point[resp.n_shops] = &shop_data[resp.n_shops];
			shop_open_data__init(&shop_data[resp.n_shops]);
			shop_data[resp.n_shops].shopid = config->ID;
			shop_data[resp.n_shops].opentime = config->EndTime;
			resp.n_shops++;
			shop_ids.insert(config->ID);
		}
	}

	resp.n_goods = 0;
	resp.goods = goods_data_point;
	for (int i = 0; i < MAX_SHOP_GOODS_NUM; ++i)
	{
		uint32_t goods_id = player->data->shop_goods[i].goods_id;
		if (goods_id == 0)
		{
			break;
		}

		ShopTable *goods_config = get_config_by_id(goods_id, &shop_config);
		if (!goods_config || goods_config->Acc == 2 || shop_ids.find(goods_config->ShopType) == shop_ids.end())
		{
			continue;
		}

		goods_data_point[resp.n_goods] = &goods_data[resp.n_goods];
		goods_data__init(&goods_data[resp.n_goods]);
		goods_data[resp.n_goods].goodsid = player->data->shop_goods[i].goods_id;
		goods_data[resp.n_goods].boughtnum = player->data->shop_goods[i].bought_num;
		resp.n_goods++;
	}

	resp.result = 0;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SHOP_INFO_ANSWER, shop_info_answer__pack, resp);

	return 0;
}

//商城购买请求
static int handle_shop_buy_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	ShopBuyRequest *req = shop_buy_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t goods_id = req->goodsid;
	uint32_t buy_num = req->goodsnum;

	shop_buy_request__free_unpacked(req, NULL);

	int ret = 0;
	GoodsInfo *goods_info = NULL;
	do
	{
		ShopTable *goods_config = get_config_by_id(goods_id, &shop_config);
		if (!goods_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get goods config failed, goods_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, goods_id);
			break;
		}

		ShopListTable *list_config = get_config_by_id(goods_config->ShopType, &shop_list_config);
		if (!list_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get list config failed, shop_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, goods_config->ShopType);
			break;
		}

		//限时的，检查时间
		uint64_t cur_tick = time_helper::get_cached_time() / 1000;
		if (list_config->StartTime > 0 && list_config->EndTime > 0 &&
				!(cur_tick >= list_config->StartTime && cur_tick <= list_config->EndTime))
		{
			ret = ERROR_ID_SHOP_CLOSED;
			LOG_ERR("[%s:%d] player[%lu] shop is closed, shop_id:%lu, start:%lu, end:%lu, now:%lu", __FUNCTION__, __LINE__, extern_data->player_id, goods_config->ShopType, list_config->StartTime, list_config->EndTime, cur_tick);
			break;
		}

		if (goods_config->Acc == 2)
		{
			ret = ERROR_ID_SHOP_GOODS_NOT_SELL;
			LOG_ERR("[%s:%d] player[%lu] goods not sell, goods_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, goods_id);
			break;
		}

		for (int i = 0; i < MAX_SHOP_GOODS_NUM; ++i)
		{
			uint32_t tmp_id = player->data->shop_goods[i].goods_id;
			if (tmp_id == 0 || tmp_id == goods_id)
			{
				goods_info = &player->data->shop_goods[i];
				break;
			}
		}

		if (!goods_info)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] error, goods_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, goods_id);
			break;
		}

		if (buy_num == 0)
		{
			ret = ERROR_ID_SHOP_GOODS_BUY_NUM;
			break;
		}

		//限购的，检查数量
		if ((int64_t)goods_config->BuyNum > 0 && goods_info->bought_num + buy_num > (uint32_t)goods_config->BuyNum)
		{
			ret = ERROR_ID_SHOP_GOODS_REMAIN;
			LOG_ERR("[%s:%d] player[%lu] goods remain not enough, goods_id:%u, limit_num:%lu, bought_num:%u, want_buy_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, goods_id, goods_config->BuyNum, goods_info->bought_num, buy_num);
			break;
		}

		uint32_t item_id = goods_config->ItemID;
		ret = player->check_can_add_item(item_id, buy_num, NULL);
		if(ret != 0)
		{
			if(ret == ERROR_ID_BAG_GRID_NOT_ENOUGH)
			{
				ret = 190300016;
			}
			LOG_ERR("[%s:%d] player[%lu] bag not enough, goods_id:%u, want_buy_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, goods_id, buy_num);
			break;
		}

		uint32_t money_type = goods_config->ConsumptionType;
		uint32_t need_money = goods_config->Discount * buy_num;
		uint32_t has_money = 0;
		uint32_t error_id = 0;
		switch(money_type)
		{
			case 1: //银票（原银两
				has_money = player->get_coin();
				error_id = ERROR_ID_COIN_NOT_ENOUGH;
				break;
			case 2: //元宝
				has_money = player->get_attr(PLAYER_ATTR_GOLD);
				error_id = ERROR_ID_GOLD_NOT_ENOUGH;
				break;
			case 3: //金票（原绑定元宝，不够可以消耗非绑定
				has_money = player->get_comm_gold();
				error_id = ERROR_ID_GOLD_NOT_ENOUGH;
				break;
			case 4: //积分
				break;
			case 5: //门宗贡献（原帮贡
				error_id = ERROR_ID_GUILD_PLAYER_DONATION;
				break;
			case 6: //功勋（原竞技场积分
			 	has_money = (uint32_t)player->get_attr(PLAYER_ATTR_GONGXUN);
			 	error_id = ERROR_ID_GONGXUN_NOT_ENOUGH;
				break;
			// case 7: //侠义值
			// 	has_money = player->get_chivalry();
			// 	error_id = ERROR_ID_CHIVALRY_NOT_ENOUGH;
			// 	break;
			 case 8: //灵石（原赏金货币
			 	has_money = (uint32_t)player->get_attr(PLAYER_ATTR_LINGSHI);
			 	error_id = ERROR_ID_LINGSHI_NOT_ENOUGH;
			 	break;
			 case 9: //声望（原惩戒货币
			 	has_money = (uint32_t)player->get_attr(PLAYER_ATTR_SHENGWANG);
			 	error_id = ERROR_ID_SHENGWANG_NOT_ENOUGH;
			 	break;
			 case 10: //血晶（原国御货币
			 	has_money = (uint32_t)player->get_attr(PLAYER_ATTR_XUEJING);
			 	error_id = ERROR_ID_XUEJING_NOT_ENOUGH;
			 	break;
		}

		if (has_money < need_money)
		{
			ret = error_id;
			LOG_ERR("[%s:%d] player[%lu] money not enough, goods_id:%u, money_type:%u, need_money:%u, has_money:%u", __FUNCTION__, __LINE__, extern_data->player_id, goods_id, money_type, need_money, has_money);
			break;
		}

		switch(money_type)
		{
			case 1: //银币
				player->sub_coin(need_money, MAGIC_TYPE_SHOP_BUY);
				break;
			case 2: //元宝
				player->sub_unbind_gold(need_money, MAGIC_TYPE_SHOP_BUY);
				break;
			case 3: //绑定元宝，不够可以消耗非绑定
				player->sub_comm_gold(need_money, MAGIC_TYPE_SHOP_BUY);
				break;
			case 4: //积分
				break;
			case 5: //帮贡
				break;
			case 6: //功勋
			 	player->sub_gongxun(need_money, MAGIC_TYPE_SHOP_BUY);
				break;
			// case 7: //侠义值
			// 	player->sub_chivalry(need_money, MAGIC_TYPE_SHOP_BUY);
			// 	break;
			 case 8: //灵石
			 	player->sub_lingshi(need_money, MAGIC_TYPE_SHOP_BUY);
			 	break;
			 case 9: //声望
			 	player->sub_shengwang(need_money, MAGIC_TYPE_SHOP_BUY);
			 	break;
			 case 10: //血晶
			 	player->sub_xuejing(need_money, MAGIC_TYPE_SHOP_BUY);
			 	break;
		}

		player->add_item(item_id, buy_num, MAGIC_TYPE_SHOP_BUY);

		//只有限购的商品，才记录
		if ((int64_t)goods_config->BuyNum > 0)
		{
			goods_info->goods_id = goods_id;
			goods_info->bought_num += buy_num;
		}

		player->add_task_progress(TCT_SHOP_BUY, goods_info->goods_id, 1);
		player->add_achievement_progress(ACType_SHOP_BUY, 0, 0, 0, 1);
	} while(0);

	ShopBuyAnswer resp;
	shop_buy_answer__init(&resp);

	resp.result = ret;
	resp.goodsid = goods_id;
	resp.boughtnum = (goods_info ? goods_info->bought_num : 0);
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SHOP_BUY_ANSWER, shop_buy_answer__pack, resp);

	return 0;
}

//获取御气道信息请求
static int handle_yuqidao_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	return notify_yuqidao_info(player, extern_data);
}

static int notify_pvp_raid_info(player_struct *player, int type, EXTERN_DATA *extern_data)
{
	player->send_pvp_raid_score_changed(PVP_TYPE_DEFINE_3);
	player->send_pvp_raid_score_changed(PVP_TYPE_DEFINE_5);
	return (0);
}

static int notify_yuqidao_info(player_struct *player, EXTERN_DATA *extern_data)
{
	YuqidaoInfoAnswer resp;
	yuqidao_info_answer__init(&resp);

	YuqidaoMaiData mai_data[MAX_YUQIDAO_MAI_NUM];
	YuqidaoMaiData* mai_data_point[MAX_YUQIDAO_MAI_NUM];

	YuqidaoBreakData break_data[MAX_YUQIDAO_BREAK_NUM];
	YuqidaoBreakData* break_data_point[MAX_YUQIDAO_BREAK_NUM];

	resp.n_mais = 0;
	resp.mais = mai_data_point;
	for (int i = 0; i < MAX_YUQIDAO_MAI_NUM; ++i)
	{
		YuqidaoMaiInfo &info = player->data->yuqidao_mais[i];
		if (info.mai_id == 0)
		{
			continue;
		}

		mai_data_point[resp.n_mais] = &mai_data[resp.n_mais];
		yuqidao_mai_data__init(&mai_data[resp.n_mais]);
		mai_data[resp.n_mais].maiid = info.mai_id;
		mai_data[resp.n_mais].acupointid = info.acupoint_id;
		mai_data[resp.n_mais].filllv = info.fill_lv;
		resp.n_mais++;
	}

	resp.n_breaks = 0;
	resp.breaks = break_data_point;
	for (int i = 0; i < MAX_YUQIDAO_BREAK_NUM; ++i)
	{
		YuqidaoBreakInfo &info = player->data->yuqidao_breaks[i];
		if (info.id == 0)
		{
			continue;
		}

		break_data_point[resp.n_breaks] = &break_data[resp.n_breaks];
		yuqidao_break_data__init(&break_data[resp.n_breaks]);
		break_data[resp.n_breaks].id = info.id;
		break_data[resp.n_breaks].curattr = info.cur_val;
		break_data[resp.n_breaks].n_curattr = MAX_YUQIDAO_BREAK_ATTR_NUM;
		break_data[resp.n_breaks].newattr = info.new_val;
		break_data[resp.n_breaks].n_newattr = MAX_YUQIDAO_BREAK_ATTR_NUM;
		break_data[resp.n_breaks].newaddn = info.new_addn;
		break_data[resp.n_breaks].n_newaddn = MAX_YUQIDAO_BREAK_ATTR_NUM;
		resp.n_breaks++;
	}

	resp.result = 0;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_YUQIDAO_INFO_ANSWER, yuqidao_info_answer__pack, resp);

	return 0;
}

//御气道灌入真气请求
static int handle_yuqidao_fill_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	YuqidaoFillRequest *req = yuqidao_fill_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t mai_id = req->maiid;

	yuqidao_fill_request__free_unpacked(req, NULL);

	int ret = 0;
	YuqidaoMaiInfo *mai_info = NULL;
	do
	{
		mai_info = player->get_yuqidao_mai(mai_id);
		if (!mai_info)
		{
			ret = ERROR_ID_YUQIDAO_MAI_NOT_OPEN;
			LOG_ERR("[%s:%d] player[%lu] jingmai is not open, mai_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, mai_id);
			break;
		}

		AcupunctureTable *config = get_config_by_id(mai_info->acupoint_id, &yuqidao_acupoint_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get acupoint config failed, mai_id:%u, acupoint_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, mai_id, mai_info->acupoint_id);
			break;
		}

		uint32_t need_coin = config->ExpendSilver;
		uint32_t has_coin = player->get_coin();
		if (has_coin < need_coin)
		{
			ret = 190500100;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, mai_id:%u, acupoint_id:%u, need_coin:%u, has_coin:%u", __FUNCTION__, __LINE__, extern_data->player_id, mai_id, mai_info->acupoint_id, need_coin, has_coin);
			break;
		}

		uint32_t need_zhenqi = config->ExpendQi;
		uint32_t has_zhenqi = player->get_zhenqi();
		if (has_zhenqi < need_zhenqi)
		{
			ret = 190500101;
			LOG_ERR("[%s:%d] player[%lu] zhenqi not enough, mai_id:%u, acupoint_id:%u, need_zhenqi:%u, has_zhenqi:%u", __FUNCTION__, __LINE__, extern_data->player_id, mai_id, mai_info->acupoint_id, need_zhenqi, has_zhenqi);
			break;
		}

		if (mai_info->fill_lv >= (uint32_t)config->GradeNum)
		{
			ret = ERROR_ID_YUQIDAO_MAI_FILL_LV_MAX;
			LOG_ERR("[%s:%d] player[%lu] fill level max, mai_id:%u, acupoint_id:%u, cur_lv:%u, max_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, mai_id, mai_info->acupoint_id, mai_info->fill_lv, (uint32_t)config->GradeNum);
			break;
		}
		
		if(player->get_level() < (uint32_t)config->Level)
		{
			ret = 190500569;
			LOG_ERR("[%s:%d] player[%lu]  level:%u < use level:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->get_level(), (uint32_t)config->Level);
			break;
		}

		player->sub_coin(need_coin, MAGIC_TYPE_YUQIDAO_FILL);
		player->sub_zhenqi(need_zhenqi, MAGIC_TYPE_YUQIDAO_FILL);

		mai_info->fill_lv++;
		player->add_achievement_progress(ACType_YUQIDAO_FILL, 0, 0, 0, 1);
		if (mai_info->fill_lv >= (uint32_t)config->GradeNum)
		{
			uint32_t next_acupoint = mai_info->acupoint_id + 1;
			if (get_config_by_id(next_acupoint, &yuqidao_acupoint_config) != NULL)
			{
				mai_info->acupoint_id = next_acupoint;
				mai_info->fill_lv = 0;
			}
			else
			{
				player->add_achievement_progress(ACType_YUQIDAO_MAI_FINISH, mai_id, 0, 0, 1);
				mai_info->acupoint_id = 0;
				mai_info->fill_lv = 0;

				PulseTable *mai_config = get_config_by_id(mai_id, &yuqidao_jingmai_config);
				if (mai_config)
				{
					bool open_break = true;
					for (std::map<uint64_t, struct PulseTable*>::iterator iter = yuqidao_jingmai_config.begin(); iter != yuqidao_jingmai_config.end(); ++iter)
					{
						PulseTable *tmp_config = iter->second;
						if (tmp_config->ID == mai_config->ID)
						{
							continue;
						}

						if (tmp_config->Break != mai_config->Break)
						{
							continue;
						}

						YuqidaoMaiInfo *tmp_mai = player->get_yuqidao_mai(tmp_config->ID);
						if (!tmp_mai || tmp_mai->acupoint_id != 0)
						{
							open_break = false;
							break;
						}
					}

					if (open_break)
					{
						player->init_yuqidao_break(mai_config->Break);
					}
				}
			}
		}

		player->calculate_attribute(true);
	} while(0);

	YuqidaoFillAnswer resp;
	yuqidao_fill_answer__init(&resp);

	resp.result = ret;
	resp.maiid = mai_id;
	if (mai_info)
	{
		resp.acupointid = mai_info->acupoint_id;
		resp.filllv = mai_info->fill_lv;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_YUQIDAO_FILL_ANSWER, yuqidao_fill_answer__pack, resp);

	return 0;
}

//御气道突破请求
static int handle_yuqidao_break_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	YuqidaoBreakRequest *req = yuqidao_break_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t break_id = req->breakid;
	uint32_t use_time = req->time;
	LOG_INFO("[%s:%d] player[%lu], break_id:%u, use_time:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id, use_time);

	yuqidao_break_request__free_unpacked(req, NULL);

	int ret = 0;
	YuqidaoBreakInfo *break_info = NULL;
	do
	{
		break_info = player->get_yuqidao_break(break_id);
		if (!break_info)
		{
			ret = ERROR_ID_YUQIDAO_BREAK_NOT_OPEN;
			LOG_ERR("[%s:%d] player[%lu] break is not open, break_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id);
			break;
		}

		BreakTable *break_config = get_config_by_id(break_id, &yuqidao_break_config);
		if (!break_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get break config failed, break_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id);
			break;
		}

		uint32_t need_lv = break_config->PulseLv;
		uint32_t player_lv = player->get_attr(PLAYER_ATTR_LEVEL);
		if (player_lv < need_lv)
		{
			ret = ERROR_ID_LEVEL_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] level not enough, break_id:%u, need_lv:%u, player_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id, need_lv, player_lv);
			break;
		}

		uint32_t need_item_id = sg_yuqidao_break_item_id;
		uint32_t need_item_num = sg_yuqidao_break_item_num;
		uint32_t has_item_num = player->get_item_can_use_num(need_item_id);
		if (has_item_num < need_item_num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] item not enough, break_id:%u, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id, need_item_id, need_item_num, has_item_num);
			break;
		}

		uint32_t tmp_new_attr[MAX_YUQIDAO_BREAK_ATTR_NUM] = {0};
		uint32_t tmp_new_addn[MAX_YUQIDAO_BREAK_ATTR_NUM] = {0};
		bool boom = (break_info->count >= (uint32_t)break_config->Minimum);
		for (int i = 0; i < MAX_YUQIDAO_BREAK_ATTR_NUM; ++i)
		{
			int quality = -1;
			uint64_t *probs = NULL;
			int n_probs = break_config->n_AttributeColor;
			switch (i)
			{
				case 0:
					probs = (boom ? break_config->MeridiansMinimum : break_config->MeridiansProbability);
					break;
				case 1:
					probs = (boom ? break_config->BloodMinimum : break_config->BloodProbability);
					break;
				case 2:
					probs = (boom ? break_config->VitalMinimum : break_config->VitalProbability);
					break;
				case 3:
					probs = (boom ? break_config->MarrowMinimum : break_config->MarrowProbability);
					break;
			}

			//先随机出落在那个品质
			uint32_t rand_num = rand() % probs[n_probs - 1];
			for (int j = 0; j < n_probs; ++j)
			{
				uint32_t lower = (j == 0 ? 0 : probs[j - 1]);
				uint32_t upper = probs[j];
				if (rand_num >= lower && rand_num < upper)
				{
					quality = j;
				}
			}

			if (quality < 0)
			{
				ret = ERROR_ID_NO_CONFIG;
				LOG_ERR("[%s:%d] player[%lu] can't get quality, break_id:%u, attr_no:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id, i);
				break;
			}

			//再在品质区间内随机出一个值
			int32_t percent_lower = (quality == 0 ? 0 : break_config->AttributeColor[quality - 1]);
			int32_t percent_upper = break_config->AttributeColor[quality];
			int32_t percent_block = percent_upper - percent_lower;
			if (percent_block <= 0)
			{
				ret = ERROR_ID_NO_CONFIG;
				LOG_ERR("[%s:%d] player[%lu] can't get quality block, break_id:%u, attr_no:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id, i);
				break;
			}

			rand_num = rand() % percent_block;
			double attr_percent = (double)(percent_lower + rand_num) / (double)percent_upper;
			uint32_t attr_lower = break_config->AttributeLower[i];
			uint32_t attr_upper = break_config->AttributeUpper[i];
			uint32_t attr_block = attr_upper - attr_lower;
			tmp_new_attr[i] = attr_lower + attr_percent * attr_block;

			//二次属性，对比配置的按下时间
			bool over = use_time > (uint32_t)break_config->Time;
			int second_part = (over ? break_config->Lost : break_config->Secondary);
			double second_percent = (double)second_part / 10000.0;
			uint32_t second_val = (over ? attr_block * second_percent : attr_block * second_percent * (double)use_time / (double)break_config->Time);
			tmp_new_addn[i] = std::min(second_val, attr_upper - tmp_new_attr[i]); //加起来不能超过上限
			LOG_INFO("[%s:%d] player[%lu], i:%u, second_percent:%f, second_val:%u, addn_val:%u", __FUNCTION__, __LINE__, extern_data->player_id, i, second_percent, second_val, tmp_new_addn[i]);
		}

		if (ret != 0)
		{
			break;
		}

		player->del_item(need_item_id, need_item_num, MAGIC_TYPE_YUQIDAO_BREAK);
		for (int i = 0; i < MAX_YUQIDAO_BREAK_ATTR_NUM; ++i)
		{
			break_info->new_val[i] = tmp_new_attr[i];
			break_info->new_addn[i] = tmp_new_addn[i];
		}

		if (boom)
		{
			break_info->count = 0;
		}
		else
		{
			break_info->count++;
		}
	} while(0);

	YuqidaoBreakAnswer resp;
	yuqidao_break_answer__init(&resp);

	YuqidaoBreakData break_data;
	yuqidao_break_data__init(&break_data);
	break_data.id = break_id;
	if (break_info)
	{
		break_data.curattr = break_info->cur_val;
		break_data.n_curattr = MAX_YUQIDAO_BREAK_ATTR_NUM;
		break_data.newattr = break_info->new_val;
		break_data.n_newattr = MAX_YUQIDAO_BREAK_ATTR_NUM;
		break_data.newaddn = break_info->new_addn;
		break_data.n_newaddn = MAX_YUQIDAO_BREAK_ATTR_NUM;
	}

	resp.result = ret;
	resp.breakinfo = &break_data;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_YUQIDAO_BREAK_ANSWER, yuqidao_break_answer__pack, resp);

	return 0;
}

//御气道突破保留请求
static int handle_yuqidao_break_retain_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	YuqidaoBreakRetainRequest *req = yuqidao_break_retain_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t break_id = req->breakid;

	yuqidao_break_retain_request__free_unpacked(req, NULL);

	int ret = 0;
	YuqidaoBreakInfo *break_info = NULL;
	do
	{
		break_info = player->get_yuqidao_break(break_id);
		if (!break_info)
		{
			ret = ERROR_ID_YUQIDAO_BREAK_NOT_OPEN;
			LOG_ERR("[%s:%d] player[%lu] break is not open, break_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id);
			break;
		}

		if (break_info->new_val[0] == 0)
		{
			ret = ERROR_ID_YUQIDAO_BREAK_RETAIN_ATTR;
			LOG_ERR("[%s:%d] player[%lu] break hasn't new attr, break_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, break_id);
			break;
		}

		bool first_break = (break_info->cur_val[0] == 0);
		for (int i = 0; i < MAX_YUQIDAO_BREAK_ATTR_NUM; ++i)
		{
			break_info->cur_val[i] = break_info->new_val[i] + break_info->new_addn[i];
			break_info->new_val[i] = 0;
			break_info->new_addn[i] = 0;
		}

		player->calculate_attribute(true);

		//第一次冲脉成功，开启下一级穴脉
		if (first_break)
		{
			player->init_yuqidao_mai(break_id, true);
		}
	} while(0);

	YuqidaoBreakRetainAnswer resp;
	yuqidao_break_retain_answer__init(&resp);

	YuqidaoBreakData break_data;
	yuqidao_break_data__init(&break_data);
	break_data.id = break_id;
	if (break_info)
	{
		break_data.curattr = break_info->cur_val;
		break_data.n_curattr = MAX_YUQIDAO_BREAK_ATTR_NUM;
		break_data.newattr = break_info->new_val;
		break_data.n_newattr = MAX_YUQIDAO_BREAK_ATTR_NUM;
		break_data.newaddn = break_info->new_addn;
		break_data.n_newaddn = MAX_YUQIDAO_BREAK_ATTR_NUM;
	}

	resp.result = ret;
	resp.breakinfo = &break_data;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_YUQIDAO_BREAK_RETAIN_ANSWER, yuqidao_break_retain_answer__pack, resp);

	return 0;
}

static void send_set_pk_type_answer(uint32_t result, uint32_t type, EXTERN_DATA *extern_data)
{
	SetPkTypeAnswer resp;
	set_pk_type_answer__init(&resp);
	resp.result = result;
	resp.pk_type = type;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_PK_TYPE_ANSWER, set_pk_type_answer__pack, resp);
}


static int handle_set_pk_type_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	if (player->get_attr(PLAYER_ATTR_MURDER) > sg_muder_cant_set_pktype)
	{
		LOG_ERR("%s: player[%lu] murder punish", __FUNCTION__, extern_data->player_id);
		return (-2);
	}

	if (player->is_in_qiecuo())
	{
		LOG_INFO("%s: player[%lu] in qiecuo state", __FUNCTION__, extern_data->player_id);
		send_set_pk_type_answer(1, 0, extern_data);
		return (-5);
	}

	SetPkTypeRequest *req = set_pk_type_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t type = req->pk_type;
	set_pk_type_request__free_unpacked(req, NULL);

		//检查等级
	if (player->get_attr(PLAYER_ATTR_LEVEL) < sg_pk_level)
	{
		LOG_ERR("%s: player[%lu] level not enough", __FUNCTION__, extern_data->player_id);
		return (-20);
	}
		// TODO: 检查区域

	uint32_t cur_type = (uint32_t)player->get_attr(PLAYER_ATTR_PK_TYPE);
	if (type == cur_type)
		return (0);
	// if (type != PK_TYPE_NORMAL && type != PK_TYPE_CAMP && type != PK_TYPE_MURDER)
	// {
	//	LOG_ERR("%s: player[%lu] pktype wrong[%d]", __FUNCTION__, player->get_uuid(), type);
	//	return (-1);
	// }
	cash_truck_struct *pTruck = cash_truck_manager::get_cash_truck_by_id(player->data->truck.truck_id);
	if (pTruck != NULL && pTruck->get_truck_type() == 2)
	{
		send_set_pk_type_answer(190500311, type, extern_data);
		return (-3);
	}

	switch (type)
	{
		case PK_TYPE_NORMAL:
				//检查CD
			if (player->data->next_set_pktype_time > time_helper::get_cached_time() / 1000 + 2)
			{
				LOG_INFO("[%s:%d] player[%lu] in cd", __FUNCTION__, __LINE__, extern_data->player_id);
				send_set_pk_type_answer(2, type, extern_data);
				return (0);
			}
			break;
		case PK_TYPE_CAMP:
				//增加CD
			if (cur_type == PK_TYPE_NORMAL)
				player->data->next_set_pktype_time = time_helper::get_cached_time() / 1000 + sg_set_pk_type_cd;
			break;
		case PK_TYPE_MURDER:
				//消耗道具
			if (player->del_item(201070037, 1, MAGIC_TYPE_SET_PK_TYPE) != 0)
			{
				LOG_ERR("[%s:%d] player[%lu] do not have item", __FUNCTION__, __LINE__, extern_data->player_id);
				return (-30);
			}
				//增加CD
			if (cur_type == PK_TYPE_NORMAL)
				player->data->next_set_pktype_time = time_helper::get_cached_time() / 1000 + sg_set_pk_type_cd;
			break;
		default:
			LOG_ERR("%s: player[%lu] pktype wrong[%d]", __FUNCTION__, player->get_uuid(), type);
			return (-40);
	}

	LOG_INFO("%s: player[%lu] set pktype [%d]", __FUNCTION__, player->get_uuid(), type);

	send_set_pk_type_answer(0, type, extern_data);

	player->set_attr(PLAYER_ATTR_PK_TYPE, type);
	player->broadcast_one_attr_changed(PLAYER_ATTR_PK_TYPE, type, false, true);

		//广播伙伴的PK模式变化
	for (int i = 0; i < MAX_PARTNER_BATTLE_NUM; ++i)
	{
		uint64_t uuid = player->data->partner_battle[i];
		if (uuid == 0)
		{
			continue;
		}

		partner_struct *partner = player->get_partner_by_uuid(uuid);
		if (partner && partner->is_alive())
		{
			partner->set_attr(PLAYER_ATTR_PK_TYPE, type);
			partner->broadcast_one_attr_changed(PLAYER_ATTR_PK_TYPE, type, false, false);			
		}
	}

	return (0);
}
static int handle_qiecuo_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	QiecuoRequest *req = qiecuo_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t target_id = req->player_id;
	qiecuo_request__free_unpacked(req, NULL);

	player_struct *target = player_manager::get_player_by_id(target_id);
	if (!target || !target->is_online())
	{
		send_comm_answer(MSG_ID_QIECUO_ANSWER, 1, extern_data);
		return (0);
	}

	if (player->is_in_qiecuo() || target->is_in_qiecuo())
	{
		send_comm_answer(MSG_ID_QIECUO_ANSWER, 190300028, extern_data);
		return (0);
	}

	//检查目标是否关闭了邀请
	if (target->data->qiecuo_invite_switch != 0)
	{
		send_comm_answer(MSG_ID_QIECUO_ANSWER, 190500158, extern_data);
		return (0);
	}

		// TODO: 检查地图是否允许切磋
		// TODO: 检查是否在安全区

	player->data->qiecuo_target = target_id;

	QiecuoNotify nty;
	qiecuo_notify__init(&nty);
	nty.name = player->get_name();
	nty.player_id = player->get_uuid();
	EXTERN_DATA ext;
	ext.player_id = target_id;
	fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_NOTIFY, qiecuo_notify__pack, nty);

	return (0);
}

static int handle_qiecuo_refuse_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	QiecuoRefuseRequest *req = qiecuo_refuse_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t target_id = req->player_id;
	qiecuo_refuse_request__free_unpacked(req, NULL);
	player_struct *target = player_manager::get_player_by_id(target_id);
	if (!target || !target->is_online())
	{
		return (0);
	}
	if (target->is_in_qiecuo())
	{
		return (0);
	}
	if (target->data->qiecuo_target != player->get_uuid())
	{
		return (0);
	}
	QiecuoRefuseNotify nty;
	qiecuo_refuse_notify__init(&nty);
	nty.player_id = player->get_uuid();
	nty.name = player->get_name();
	EXTERN_DATA ext;
	ext.player_id = target_id;
	fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_REFUSE_NOTIFY, qiecuo_refuse_notify__pack, nty);
	return (0);
}

static int handle_qiecuo_start_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	QiecuoStartRequest *req = qiecuo_start_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t target_id = req->player_id;
	qiecuo_start_request__free_unpacked(req, NULL);

	player_struct *target = player_manager::get_player_by_id(target_id);
	if (!target || !target->is_online())
	{
		send_comm_answer(MSG_ID_QIECUO_START_ANSWER, 1, extern_data);
		return (0);
	}

	if (player->is_in_qiecuo() || target->is_in_qiecuo())
	{
		send_comm_answer(MSG_ID_QIECUO_START_ANSWER, 2, extern_data);
		return (0);
	}
	if (target->data->qiecuo_target != player->get_uuid())
	{
		send_comm_answer(MSG_ID_QIECUO_START_ANSWER, 3, extern_data);
		return (0);
	}

	if (target->scene != player->scene)
	{
		send_comm_answer(MSG_ID_QIECUO_START_ANSWER, 4, extern_data);
		return (0);
	}

	if (player->get_attr(PLAYER_ATTR_PK_TYPE) != PK_TYPE_NORMAL
		|| target->get_attr(PLAYER_ATTR_PK_TYPE) != PK_TYPE_NORMAL)
	{
		send_comm_answer(MSG_ID_QIECUO_START_ANSWER, 190300029, extern_data);
		return (0);
	}

	// 检查安全区
	if (player->get_attr(PLAYER_ATTR_REGION_ID) == 11
		|| target->get_attr(PLAYER_ATTR_REGION_ID) == 11)
	{
		send_comm_answer(MSG_ID_QIECUO_START_ANSWER, 190300031, extern_data);
		return (0);
	}

		// TODO: 检查地图是否允许切磋
		// TODO: 检查距离

	struct position *player_pos = player->get_pos();
	struct position *target_pos = target->get_pos();
	uint32_t pos_x = (player_pos->pos_x - target_pos->pos_x) / 2 + target_pos->pos_x;
	uint32_t pos_z = (player_pos->pos_z - target_pos->pos_z) / 2 + target_pos->pos_z;
	player->set_qiecuo(pos_x, pos_z, target_id);
	target->set_qiecuo(pos_x, pos_z, player->get_uuid());

	QiecuoStartNotify nty;
	EXTERN_DATA ext;
	qiecuo_start_notify__init(&nty);
	nty.pos_x = pos_x;
	nty.pos_z = pos_z;

	nty.name = player->get_name();
	nty.player_id = player->get_uuid();
	ext.player_id = target_id;
	fast_send_msg(&conn_node_gamesrv::connecter, &ext, MSG_ID_QIECUO_START_NOTIFY, qiecuo_start_notify__pack, nty);

	nty.name = target->get_name();
	nty.player_id = target->get_uuid();
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_QIECUO_START_NOTIFY, qiecuo_start_notify__pack, nty);

	return (0);
}

//处理玩家离线的缓存
static int handle_player_cache_answer(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PlayerOfflineCache *req = player_offline_cache__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	//处理离线的缓存
	for (size_t i = 0; i < req->n_sub_exps; ++i)
	{
		player->sub_exp(req->sub_exps[i]->val, req->sub_exps[i]->statis_id);
	}

	for (size_t i = 0; i < req->n_pvp_lose; ++i)
	{
		player->change_pvp_raid_score(req->pvp_lose[i]->type, req->pvp_lose[i]->score);
	}
	uint32_t today_sec = time_helper::get_cached_time() / 1000;
	for (size_t i = 0; i < req->n_pvp_win; ++i)
	{
		player->change_pvp_raid_score(req->pvp_win[i]->type, req->pvp_win[i]->score);
		//使用time字段来判断是否是今日的胜利
		if (!time_helper::is_same_day(req->pvp_win[i]->time, today_sec, 3600 * 5))
		{
			if (req->pvp_win[i]->type == PVP_TYPE_DEFINE_3)
				--player->data->pvp_raid_data.oneday_win_num_3;
			else
				--player->data->pvp_raid_data.oneday_win_num_5;
		}
	}

	player_offline_cache__free_unpacked(req, NULL);

	//通知redis清除缓存的数据
	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_CLEAR_OFFLINE_CACHE, 0, get_seq());

	return 0;
}

static void send_pvp_match_start_answer(player_struct *player, EXTERN_DATA *extern_data, int result, uint64_t player_id, int cd)
{
	PvpMatchAnswer ans;
	pvp_match_answer__init(&ans);
	ans.playerid = player_id;
	ans.result = result;
	ans.cdtime = cd;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PVP_MATCH_START_ANSWER, pvp_match_answer__pack, ans);
}

static int handle_pvp_match_team_start_request(player_struct *player, EXTERN_DATA *extern_data, int type)
{	
		// 是不是队长
	if (player->m_team->GetLeadId() != player->get_uuid())
	{
		LOG_ERR("%s : player[%lu] not leader ", __FUNCTION__, player->get_uuid());
		return (-1);
	}

		// 是不是已经在队列中了
	if (pvp_match_is_team_in_waiting(player->m_team->GetId()))
	{
		LOG_ERR("%s : player[%lu] team already in waiting ", __FUNCTION__, player->get_uuid());
		send_pvp_match_start_answer(player, extern_data, 190500175, player->get_uuid(), 0);
		return (-10);
	}

	if (type == PVP_TYPE_DEFINE_3)
	{
		if (player->data->pvp_raid_data.level_3 == 1)
		{
			send_pvp_match_start_answer(player, extern_data, 190500108, player->get_uuid(), 0);
			return (-15);
		}
		
		// 队伍人数有没有超过
		if (player->m_team->GetMemberSize() != PVP_MATCH_PLAYER_NUM_3)
		{
			LOG_ERR("%s: player[%lu] team have too much member", __FUNCTION__, player->get_uuid());
			return (-20);
		}
		// 等级是否符合	以及是否在线
		uint8_t leader_lv = player->data->pvp_raid_data.level_3;
		int min_lv = leader_lv - MATCH_LEVEL_DIFF;
		int max_lv = leader_lv + MATCH_LEVEL_DIFF;

		for (int pos = 0; pos < player->m_team->m_data->m_memSize; ++pos)
		{
			if (player->m_team->m_data->m_mem[pos].timeremove != 0)
			{
				LOG_ERR("%s: player[%lu] team have player offline", __FUNCTION__, player->get_uuid());
				return (-30);
			}
			player_struct *t_player = player_manager::get_player_by_id(player->m_team->m_data->m_mem[pos].id);
			if (!t_player || !t_player->is_online())
			{
				LOG_ERR("%s: player[%lu] team have player offline", __FUNCTION__, player->get_uuid());
				return (-31);
			}
			int lv = t_player->data->pvp_raid_data.level_3;
			if (lv < min_lv || lv > max_lv)
			{
				LOG_ERR("%s: player[%lu] team have player lv[%d] not match", __FUNCTION__, player->get_uuid(), lv);
				send_pvp_match_start_answer(player, extern_data, 190500571, t_player->get_uuid(), 0);
				return (-40);
			}

			if (pvp_match_is_player_in_waiting(t_player->get_uuid()))
			{
				LOG_ERR("[%s:%d] player[%lu] already in waiting", __FUNCTION__, __LINE__, t_player->get_uuid());
				send_pvp_match_start_answer(player, extern_data, 190500175, t_player->get_uuid(), 0);
				return (-50);
			}
			uint32_t cd = pvp_match_is_player_in_cd(t_player);
			if (cd > 0)
			{
				LOG_INFO("[%s:%d] player[%lu] in cd[%u]", __FUNCTION__, __LINE__, t_player->get_uuid(), cd);
				send_pvp_match_start_answer(player, extern_data, 190500103, t_player->get_uuid(), cd);
				return (-60);
			}
		}
	}
	else
	{
		if (player->data->pvp_raid_data.level_5 == 1)
		{
			send_pvp_match_start_answer(player, extern_data, 190500108, player->get_uuid(), 0);
			return (-15);
		}
		
		// 队伍人数有没有超过
		if (player->m_team->GetMemberSize() != PVP_MATCH_PLAYER_NUM_5)
		{
			LOG_ERR("%s: player[%lu] team have too much member", __FUNCTION__, player->get_uuid());
			return (-21);
		}
		// 等级是否符合	以及是否在线
		uint8_t leader_lv = player->data->pvp_raid_data.level_5;
		int min_lv = leader_lv - MATCH_LEVEL_DIFF;
		int max_lv = leader_lv + MATCH_LEVEL_DIFF;

		for (int pos = 0; pos < player->m_team->m_data->m_memSize; ++pos)
		{
			if (player->m_team->m_data->m_mem[pos].timeremove != 0)
			{
				LOG_ERR("%s: player[%lu] team have player offline", __FUNCTION__, player->get_uuid());
				return (-30);
			}
			player_struct *t_player = player_manager::get_player_by_id(player->m_team->m_data->m_mem[pos].id);
			if (!t_player || !t_player->is_online())
			{
				LOG_ERR("%s: player[%lu] team have player offline", __FUNCTION__, player->get_uuid());
				return (-31);
			}
			int lv = t_player->data->pvp_raid_data.level_5;
			if (lv < min_lv || lv > max_lv)
			{
				LOG_ERR("%s: player[%lu] team have player lv[%d] not match", __FUNCTION__, player->get_uuid(), lv);
				send_pvp_match_start_answer(player, extern_data, 190500571, t_player->get_uuid(), 0);
				return (-40);
			}
			if (pvp_match_is_player_in_waiting(t_player->get_uuid()))
			{
				LOG_ERR("[%s:%d] player[%lu] already in waiting", __FUNCTION__, __LINE__, t_player->get_uuid());
				send_pvp_match_start_answer(player, extern_data, 190500175, t_player->get_uuid(), 0);
				return (-50);
			}
			uint32_t cd = pvp_match_is_player_in_cd(t_player);
			if (cd > 0)
			{
				LOG_INFO("[%s:%d] player[%lu] in cd[%u]", __FUNCTION__, __LINE__, t_player->get_uuid(), cd);
				send_pvp_match_start_answer(player, extern_data, 190500103, t_player->get_uuid(), cd);
				return (-60);
			}
		}
	}

	pvp_match_add_team_to_waiting(player, type);
	send_pvp_match_start_answer(player, extern_data, 0, 0, 0);
	return (0);
}

static int handle_pvp_match_start_request(player_struct *player, EXTERN_DATA *extern_data)
{
		// TODO: 增加一些检查，比如是否已经在副本中了等

	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PvpMatchRequest *req = pvp_match_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int type = req->type;

	pvp_match_request__free_unpacked(req, NULL);

	uint32_t raid_id;
	if (type == PVP_TYPE_DEFINE_3)
	{
		raid_id = sg_3v3_pvp_raid_param1[0];
	}
	else
	{
		raid_id = sg_5v5_pvp_raid_param1[0];
	}
	if (raid_manager::check_player_enter_raid(player, raid_id) != 0)
	{
		LOG_INFO("%s: player[%lu] can not enter raid %u", __FUNCTION__, player->get_uuid(), raid_id);
		return (-15);
	}

	if (player->m_team)
	{
		return handle_pvp_match_team_start_request(player, extern_data, type);
	}

	uint32_t cd = pvp_match_is_player_in_cd(player);
	if (cd > 0)
	{
		LOG_INFO("[%s:%d] player[%lu] in cd[%u]", __FUNCTION__, __LINE__, player->get_uuid(), cd);
		send_pvp_match_start_answer(player, extern_data, 190500103, player->get_uuid(), cd);
		return (-20);
	}

		//检查是否已经在队列中了
	if (pvp_match_is_player_in_waiting(player->get_uuid()))
	{
		LOG_ERR("[%s:%d] player[%lu] already in waiting", __FUNCTION__, __LINE__, extern_data->player_id);
		send_pvp_match_start_answer(player, extern_data, 190500175, player->get_uuid(), 0);
		return (-30);
	}

	if (type == PVP_TYPE_DEFINE_3)
	{
		if (player->data->pvp_raid_data.level_3 == 1)
		{
			send_pvp_match_start_answer(player, extern_data, 0, 0, 0);
			pvp_match_single_ai_player_3(player);
			return (0);
		}
	}
	else
	{
		if (player->data->pvp_raid_data.level_5 == 1)
		{
		}
	}

	pvp_match_add_player_to_waiting(player, type);
	send_pvp_match_start_answer(player, extern_data, 0, 0, 0);
	return (0);
}

static int handle_pvp_raid_praise_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	PvpRaidPraiseRequest *req = pvp_raid_praise_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t player_id = req->player_id;

	pvp_raid_praise_request__free_unpacked(req, NULL);

	pvp_match_player_praise(player, player_id);
	return (0);
}

static int handle_pvp_match_ready_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	pvp_match_player_set_ready(player);
	return (0);
}
static int handle_pvp_match_cancel_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	pvp_match_player_cancel(player);
	return (0);
}

// TODO: pvp排行榜 先不做
static int handle_pvp_rank_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	return (0);
}
static int handle_pvp_open_level_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	PvpOpenLevelRewardRequest *req = pvp_open_level_reward_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-30);
	}
	uint32_t type = req->type;
	pvp_open_level_reward_request__free_unpacked(req, NULL);

	PvpOpenLevelRewardAnswer answer;
	pvp_open_level_reward_answer__init(&answer);
	answer.type = type;

	if (type == PVP_TYPE_DEFINE_3)
	{
		if (player->data->pvp_raid_data.cur_level_id_3 < player->data->pvp_raid_data.avaliable_reward_level_3)
		{
			LOG_ERR("%s: player[%lu] level not enough", __FUNCTION__, extern_data->player_id);
			return (-40);
		}
		struct StageTable *config = get_config_by_id(player->data->pvp_raid_data.avaliable_reward_level_3, &pvp_raid_config);
		if (!config)
		{
			LOG_ERR("%s: player[%lu] can not get reward [%u]", __FUNCTION__, extern_data->player_id, player->data->pvp_raid_data.avaliable_reward_level_3);
			return (-50);
		}
		++player->data->pvp_raid_data.avaliable_reward_level_3;
		answer.avaliable_reward_level = player->data->pvp_raid_data.avaliable_reward_level_3;
		player->give_drop_item(config->StageReward, MAGIC_TYPE_PVP_RAID_LEVEL_REWARD, ADD_ITEM_SEND_MAIL_WHEN_BAG_FULL);
	}
	else
	{
		if (player->data->pvp_raid_data.cur_level_id_5 < player->data->pvp_raid_data.avaliable_reward_level_5)
		{
			LOG_ERR("%s: player[%lu] level not enough", __FUNCTION__, extern_data->player_id);
			return (-40);
		}
		struct StageTable *config = get_config_by_id(player->data->pvp_raid_data.avaliable_reward_level_5, &pvp_raid_config);
		if (!config)
		{
			LOG_ERR("%s: player[%lu] can not get reward [%u]", __FUNCTION__, extern_data->player_id, player->data->pvp_raid_data.avaliable_reward_level_5);
			return (-50);
		}
		++player->data->pvp_raid_data.avaliable_reward_level_5;
		answer.avaliable_reward_level = player->data->pvp_raid_data.avaliable_reward_level_5;
		player->give_drop_item(config->StageReward, MAGIC_TYPE_PVP_RAID_LEVEL_REWARD, ADD_ITEM_SEND_MAIL_WHEN_BAG_FULL);
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PVP_OPEN_LEVEL_REWARD_ANSWER, pvp_open_level_reward_answer__pack, answer);
	return (0);
}
static int handle_pvp_open_daily_box_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	PvpOpenDailyBoxRequest *req = pvp_open_daily_box_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req) {
		LOG_ERR("%s %d: can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-30);
	}
	uint32_t box_id = req->box_id;
	uint32_t type = req->type;
	pvp_open_daily_box_request__free_unpacked(req, NULL);
	bool found = false;

	PvpOpenDailyBoxAnswer answer;
	pvp_open_daily_box_answer__init(&answer);
	answer.result = 0;
	answer.type = type;
	uint32_t box[MAX_ONEDAY_PVP_BOX];
	answer.avaliable_box_id = box;

	if (type == PVP_TYPE_DEFINE_3)
	{
		if (player->data->pvp_raid_data.oneday_win_num_3 <= (box_id))
		{
			LOG_ERR("%s: player[%lu] [%u][%u]win num3 not enough", __FUNCTION__, extern_data->player_id,
				player->data->pvp_raid_data.oneday_win_num_3, box_id);
			return (-10);
		}
		for (int i = 0; i < MAX_ONEDAY_PVP_BOX; ++i)
		{
			if (player->data->pvp_raid_data.avaliable_box_3[i] == box_id)
			{
				player->data->pvp_raid_data.avaliable_box_3[i] = -1;
				found = true;
				break;
			}
		}
		if (!found)
		{
			LOG_ERR("%s: player[%lu] already get box %u", __FUNCTION__, extern_data->player_id, box_id);
			return (-20);
		}
		struct StageTable *config = get_config_by_id(player->data->pvp_raid_data.cur_level_id_3, &pvp_raid_config);
		if (!config)
		{
			LOG_ERR("%s: player[%lu] CAN NOT get config  %u", __FUNCTION__, extern_data->player_id, player->data->pvp_raid_data.cur_level_id_3);
			return (-30);
		}
		if (box_id >= config->n_VictoryReward3)
		{
			LOG_ERR("%s: player[%lu] box id no config %u", __FUNCTION__, extern_data->player_id, box_id);
			return (-40);
		}
		player->give_drop_item(config->VictoryReward3[box_id], MAGIC_TYPE_PVP_RAID_DAILY_REWARD, ADD_ITEM_SEND_MAIL_WHEN_BAG_FULL);
		for (int i = 0; i < MAX_ONEDAY_PVP_BOX; ++i)
		{
			if (player->data->pvp_raid_data.avaliable_box_3[i] == (uint8_t)-1)
				continue;
			box[answer.n_avaliable_box_id++] = player->data->pvp_raid_data.avaliable_box_3[i];
		}

	}
	else
	{
		if (player->data->pvp_raid_data.oneday_win_num_5 <= (box_id))
		{
			LOG_ERR("%s: player[%lu] win num5 not enough", __FUNCTION__, extern_data->player_id);
			return (-10);
		}
		for (int i = 0; i < MAX_ONEDAY_PVP_BOX; ++i)
		{
			if (player->data->pvp_raid_data.avaliable_box_5[i] == box_id)
			{
				player->data->pvp_raid_data.avaliable_box_5[i] = -1;
				found = true;
				break;
			}
		}
		if (!found)
		{
			LOG_ERR("%s: player[%lu] already get box %u", __FUNCTION__, extern_data->player_id, box_id);
			return (-20);
		}
		struct StageTable *config = get_config_by_id(player->data->pvp_raid_data.cur_level_id_5, &pvp_raid_config);
		if (!config)
		{
			LOG_ERR("%s: player[%lu] CAN NOT get config  %u", __FUNCTION__, extern_data->player_id, player->data->pvp_raid_data.cur_level_id_5);
			return (-30);
		}
		if (box_id >= config->n_VictoryReward5)
		{
			LOG_ERR("%s: player[%lu] box id no config %u", __FUNCTION__, extern_data->player_id, box_id);
			return (-40);
		}
		player->give_drop_item(config->VictoryReward5[box_id], MAGIC_TYPE_PVP_RAID_DAILY_REWARD, ADD_ITEM_SEND_MAIL_WHEN_BAG_FULL);
		for (int i = 0; i < MAX_ONEDAY_PVP_BOX; ++i)
		{
			if (player->data->pvp_raid_data.avaliable_box_5[i] == (uint8_t)-1)
				continue;
			box[answer.n_avaliable_box_id++] = player->data->pvp_raid_data.avaliable_box_5[i];
		}

	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PVP_OPEN_DAILY_BOX_ANSWER, pvp_open_daily_box_answer__pack, answer);
	return (0);
}

//获取八卦牌信息请求
static int handle_baguapai_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	return notify_baguapai_info(player, extern_data);
}

static int notify_baguapai_info(player_struct *player, EXTERN_DATA *extern_data)
{
	BaguapaiInfoAnswer resp;
	baguapai_info_answer__init(&resp);

	BaguapaiDressData dress_data[MAX_BAGUAPAI_STYLE_NUM];
	BaguapaiDressData* dress_data_point[MAX_BAGUAPAI_STYLE_NUM];
	BaguapaiCardData card_data[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM];
	BaguapaiCardData* card_data_point[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM];
	CommonRandAttrData attr_data[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* attr_data_point[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData attr_new_data[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* attr_new_data_point[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData additional_attr_data[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];
	CommonRandAttrData* additional_attr_data_point[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];

	uint32_t style_num = 0;
	for (int i = 0; i < MAX_BAGUAPAI_STYLE_NUM; ++i)
	{
		dress_data_point[style_num] = &dress_data[style_num];
		baguapai_dress_data__init(&dress_data[style_num]);
		dress_data[style_num].styleid = i + 1;

		uint32_t card_num = 0;
		for (int j = 0; j < MAX_BAGUAPAI_DRESS_NUM; ++j)
		{
			card_data_point[style_num][card_num] = &card_data[style_num][card_num];
			baguapai_card_data__init(&card_data[style_num][card_num]);
			card_data[style_num][card_num].id = player->data->baguapai_dress[i].card_list[j].id;
			card_data[style_num][card_num].star = player->data->baguapai_dress[i].card_list[j].star;

			uint32_t cur_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++k)
			{
				if (player->data->baguapai_dress[i].card_list[j].minor_attrs[k].id == 0)
				{
					break;
				}

				attr_data_point[style_num][card_num][cur_num] = &attr_data[style_num][card_num][cur_num];
				common_rand_attr_data__init(&attr_data[style_num][card_num][cur_num]);
				attr_data[style_num][card_num][cur_num].pool = player->data->baguapai_dress[i].card_list[j].minor_attrs[k].pool;
				attr_data[style_num][card_num][cur_num].id = player->data->baguapai_dress[i].card_list[j].minor_attrs[k].id;
				attr_data[style_num][card_num][cur_num].val = player->data->baguapai_dress[i].card_list[j].minor_attrs[k].val;
				cur_num++;
			}
			card_data[style_num][card_num].n_minorattrs = cur_num;
			card_data[style_num][card_num].minorattrs = attr_data_point[style_num][card_num];

			uint32_t new_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++k)
			{
				if (player->data->baguapai_dress[i].card_list[j].minor_attrs_new[k].id == 0)
				{
					break;
				}

				attr_new_data_point[style_num][card_num][new_num] = &attr_new_data[style_num][card_num][new_num];
				common_rand_attr_data__init(&attr_new_data[style_num][card_num][new_num]);
				attr_new_data[style_num][card_num][new_num].pool = player->data->baguapai_dress[i].card_list[j].minor_attrs_new[k].pool;
				attr_new_data[style_num][card_num][new_num].id = player->data->baguapai_dress[i].card_list[j].minor_attrs_new[k].id;
				attr_new_data[style_num][card_num][new_num].val = player->data->baguapai_dress[i].card_list[j].minor_attrs_new[k].val;
				new_num++;
			}
			card_data[style_num][card_num].n_minorattrsnew = new_num;
			card_data[style_num][card_num].minorattrsnew = attr_new_data_point[style_num][card_num];

			uint32_t addition_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM; ++k)
			{
				if (player->data->baguapai_dress[i].card_list[j].additional_attrs[k].id == 0)
				{
					break;
				}

				additional_attr_data_point[style_num][card_num][addition_num] = &additional_attr_data[style_num][card_num][addition_num];
				common_rand_attr_data__init(&additional_attr_data[style_num][card_num][addition_num]);
				additional_attr_data[style_num][card_num][addition_num].pool = player->data->baguapai_dress[i].card_list[j].additional_attrs[k].pool;
				additional_attr_data[style_num][card_num][addition_num].id = player->data->baguapai_dress[i].card_list[j].additional_attrs[k].id;
				additional_attr_data[style_num][card_num][addition_num].val = player->data->baguapai_dress[i].card_list[j].additional_attrs[k].val;
				addition_num++;
			}
			card_data[style_num][card_num].n_additionalattrs = addition_num;
			card_data[style_num][card_num].additionalattrs = additional_attr_data_point[style_num][card_num];

			card_num++;
		}
		dress_data[style_num].n_cards = card_num;
		dress_data[style_num].cards = card_data_point[style_num];

		style_num++;
	}
	resp.n_datas = style_num;
	resp.datas = dress_data_point;

	resp.result = 0;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_INFO_ANSWER, baguapai_info_answer__pack, resp);

	return (0);
}

//八卦牌切换请求
static int handle_baguapai_switch_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BaguapaiSwitchRequest *req = baguapai_switch_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t style_id = req->styleid;

	baguapai_switch_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		if (style_id > MAX_BAGUAPAI_STYLE_NUM)
		{
			ret = ERROR_ID_BAGUAPAI_STYLE_ID;
			LOG_ERR("[%s:%d] player[%lu] style_id error, style_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id);
			break;
		}

		uint32_t cur_style = player->get_attr(PLAYER_ATTR_BAGUA);
		if (style_id != cur_style)
		{
			player->data->attrData[PLAYER_ATTR_BAGUA] = style_id;
			AttrMap attrs;
			attrs[PLAYER_ATTR_BAGUA] = player->get_attr(PLAYER_ATTR_BAGUA);
			player->notify_attr(attrs);
			player->calculate_attribute(true);
		}
	} while(0);

	BaguapaiSwitchAnswer resp;
	baguapai_switch_answer__init(&resp);

	resp.result = ret;
	resp.styleid = style_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_SWITCH_ANSWER, baguapai_switch_answer__pack, resp);

	return 0;
}

//八卦牌更换请求
static int handle_baguapai_wear_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BaguapaiWearRequest *req = baguapai_wear_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t style_id = req->styleid;
	std::vector<uint32_t> grid_list;
	std::set<uint32_t> tmp_set;
	for (size_t i = 0; i < req->n_gridids; ++i)
	{
		grid_list.push_back(req->gridids[i]);
		tmp_set.insert(req->gridids[i]);
	}

	baguapai_wear_request__free_unpacked(req, NULL);

	int ret = 0;
	BaguapaiDressInfo *dress_info = NULL;
	do
	{
		//背包格子是否重复
		if (grid_list.size() != tmp_set.size())
		{
			ret = ERROR_ID_BAGUAPAI_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] bag pos repeated, style_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id);
			break;
		}

		//一次穿戴数量是否合法
		uint32_t wear_num = grid_list.size();
		if (wear_num == 0 || wear_num > MAX_BAGUAPAI_DRESS_NUM)
		{
			ret = ERROR_ID_BAGUAPAI_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] wear num error, style_id:%u, wear_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, wear_num);
			break;
		}

		std::set<uint32_t> part_set;
		std::map<uint32_t, bag_grid_data *> grid_datas;
		uint32_t player_lv = player->get_attr(PLAYER_ATTR_LEVEL);
		uint32_t suit_id = 0;
		for (std::vector<uint32_t>::iterator iter = grid_list.begin(); iter != grid_list.end(); ++iter)
		{
			uint32_t pos = *iter;
			if (pos > player->data->bag_grid_num)
			{
				ret = ERROR_ID_BAGUAPAI_BAG_POS;
				LOG_ERR("[%s:%d] player[%lu] bag pos invalid, style_id:%u, pos:%u, bag_size:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, pos, player->data->bag_grid_num);
				break;
			}

			bag_grid_data *grid = &player->data->bag[pos];
			uint32_t card_id = bagua_item_to_card(grid->id);
			BaguaTable *card_config = get_config_by_id(card_id, &bagua_config);
			if (!card_config)
			{
				ret = ERROR_ID_BAGUAPAI_BAG_POS;
				LOG_ERR("[%s:%d] player[%lu] get card config failed, style_id:%u, pos:%u, item_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, pos, grid->id, card_id);
				break;
			}

			//是否是同一套
			if ((uint32_t)card_config->BaguaType != style_id)
			{
				ret = ERROR_ID_BAGUAPAI_BAG_POS;
				LOG_ERR("[%s:%d] player[%lu] card style error, style_id:%u, pos:%u, item_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, pos, grid->id, card_id);
				break;
			}

			//是否是一个套装的
			if (suit_id == 0)
			{
				suit_id = card_config->Suit;
			}
			else if ((uint32_t)card_config->Suit != suit_id)
			{
				ret = ERROR_ID_BAGUAPAI_BAG_POS;
				LOG_ERR("[%s:%d] player[%lu] not suit, style_id:%u, pos:%u, item_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, pos, grid->id, card_id);
				break;
			}

			//等级是否达到
			uint32_t need_lv = (uint32_t)card_config->Level;
			if (player_lv < need_lv)
			{
				ret = ERROR_ID_LEVEL_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] level, style_id:%u, pos:%u, item_id:%u, card_id:%u, need_lv:%u, player_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, pos, grid->id, card_id, need_lv, player_lv);
				break;
			}

			//部位是否合法
			uint32_t part_id = (uint32_t)card_config->BaguaPosition;
			if (part_id > MAX_BAGUAPAI_DRESS_NUM)
			{
				ret = ERROR_ID_BAGUAPAI_BAG_POS;
				LOG_ERR("[%s:%d] player[%lu] part error, style_id:%u, pos:%u, item_id:%u, card_id:%u, part:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, pos, grid->id, card_id, part_id);
				break;
			}

			part_set.insert(card_config->BaguaPosition);
			grid_datas[part_id] = grid;
		}

		if (ret != 0)
		{
			break;
		}

		//部位是否有重复
		if (part_set.size() != (size_t)wear_num)
		{
			ret = ERROR_ID_BAGUAPAI_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] part repeated, style_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id);
			break;
		}

		dress_info = player->get_baguapai_dress(style_id);
		if (!dress_info)
		{
			ret = ERROR_ID_BAGUAPAI_STYLE_ID;
			LOG_ERR("[%s:%d] player[%lu] has not the style, style_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id);
			break;
		}

		std::vector<BaguapaiCardInfo> card_caches;
		for (std::map<uint32_t, bag_grid_data *>::iterator iter = grid_datas.begin(); iter != grid_datas.end(); ++iter)
		{
			uint32_t part_id = iter->first;
			bag_grid_data *grid = iter->second;
			uint32_t card_id = bagua_item_to_card(grid->id);
			BaguapaiCardInfo *card_info = &dress_info->card_list[part_id - 1];
			card_caches.push_back(*card_info);
			memset(card_info, 0, sizeof(BaguapaiCardInfo));
			card_info->id = card_id;
			card_info->star = grid->especial_item.baguapai.star;
			memcpy(card_info->minor_attrs, grid->especial_item.baguapai.minor_attrs, sizeof(card_info->minor_attrs));
			memcpy(card_info->additional_attrs, grid->especial_item.baguapai.additional_attrs, sizeof(card_info->additional_attrs));
			player->add_task_progress(TCT_BAGUA_WEAR, card_id, 1);
		}

		//把装备的卡牌从背包删除，由于不是消耗，此处不记录流水
		for (std::vector<uint32_t>::iterator iter = grid_list.begin(); iter != grid_list.end(); ++iter)
		{
			player->del_item_grid(*iter, true);
		}

		//把替换下来的卡牌放回背包
		for (std::vector<BaguapaiCardInfo>::iterator iter = card_caches.begin(); iter != card_caches.end(); ++iter)
		{
			if (iter->id == 0)
			{
				continue;
			}

			player->move_baguapai_to_bag(*iter);
		}

		player->calculate_attribute(true);

		suit_id = player->get_bagua_suit_id(dress_info);
		if (suit_id > 0)
		{
			player->add_achievement_progress(ACType_BAGUA_SUIT, suit_id, 0, 0, 1);
		}
		player->add_achievement_progress(ACType_BAGUA_MIN_STAR, 0, 0, 0, 1);
	} while(0);

	BaguapaiWearAnswer resp;
	baguapai_wear_answer__init(&resp);

	BaguapaiDressData dress_data;
	baguapai_dress_data__init(&dress_data);

	BaguapaiCardData card_data[MAX_BAGUAPAI_DRESS_NUM];
	BaguapaiCardData* card_data_point[MAX_BAGUAPAI_DRESS_NUM];
	CommonRandAttrData attr_data[MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* attr_data_point[MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData attr_new_data[MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* attr_new_data_point[MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData additional_attr_data[MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];
	CommonRandAttrData* additional_attr_data_point[MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];

	resp.result = ret;
	if (dress_info)
	{
		resp.datas = &dress_data;
		dress_data.styleid = style_id;
		uint32_t card_num = 0;
		for (int j = 0; j < MAX_BAGUAPAI_DRESS_NUM; ++j)
		{
			card_data_point[card_num] = &card_data[card_num];
			baguapai_card_data__init(&card_data[card_num]);
			card_data[card_num].id = dress_info->card_list[j].id;
			card_data[card_num].star = dress_info->card_list[j].star;

			uint32_t cur_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++k)
			{
				if (dress_info->card_list[j].minor_attrs[k].id == 0)
				{
					break;
				}

				attr_data_point[card_num][cur_num] = &attr_data[card_num][cur_num];
				common_rand_attr_data__init(&attr_data[card_num][cur_num]);
				attr_data[card_num][cur_num].pool = dress_info->card_list[j].minor_attrs[k].pool;
				attr_data[card_num][cur_num].id = dress_info->card_list[j].minor_attrs[k].id;
				attr_data[card_num][cur_num].val = dress_info->card_list[j].minor_attrs[k].val;
				cur_num++;
			}
			card_data[card_num].n_minorattrs = cur_num;
			card_data[card_num].minorattrs = attr_data_point[card_num];

			uint32_t new_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++k)
			{
				if (dress_info->card_list[j].minor_attrs_new[k].id == 0)
				{
					break;
				}

				attr_new_data_point[card_num][new_num] = &attr_new_data[card_num][new_num];
				common_rand_attr_data__init(&attr_new_data[card_num][new_num]);
				attr_new_data[card_num][new_num].pool = dress_info->card_list[j].minor_attrs_new[k].pool;
				attr_new_data[card_num][new_num].id = dress_info->card_list[j].minor_attrs_new[k].id;
				attr_new_data[card_num][new_num].val = dress_info->card_list[j].minor_attrs_new[k].val;
				new_num++;
			}
			card_data[card_num].n_minorattrsnew = new_num;
			card_data[card_num].minorattrsnew = attr_new_data_point[card_num];

			uint32_t addition_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM; ++k)
			{
				if (dress_info->card_list[j].additional_attrs[k].id == 0)
				{
					break;
				}

				additional_attr_data_point[card_num][addition_num] = &additional_attr_data[card_num][addition_num];
				common_rand_attr_data__init(&additional_attr_data[card_num][addition_num]);
				additional_attr_data[card_num][addition_num].pool = dress_info->card_list[j].additional_attrs[k].pool;
				additional_attr_data[card_num][addition_num].id = dress_info->card_list[j].additional_attrs[k].id;
				additional_attr_data[card_num][addition_num].val = dress_info->card_list[j].additional_attrs[k].val;
				addition_num++;
			}
			card_data[card_num].n_additionalattrs = addition_num;
			card_data[card_num].additionalattrs = additional_attr_data_point[card_num];

			card_num++;
		}
		dress_data.n_cards = card_num;
		dress_data.cards = card_data_point;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_WEAR_ANSWER, baguapai_wear_answer__pack, resp);

	return 0;
}

//八卦牌分解请求
static int handle_baguapai_decompose_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BaguapaiDecomposeRequest *req = baguapai_decompose_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	std::set<uint32_t> grid_ids;
	for (size_t i = 0; i < req->n_gridids; ++i)
	{
		grid_ids.insert(req->gridids[i]);
	}

	baguapai_decompose_request__free_unpacked(req, NULL);

	int ret = 0;
	std::map<uint32_t, uint32_t> gain_map;
	do
	{
		for (std::set<uint32_t>::iterator iter = grid_ids.begin(); iter != grid_ids.end(); ++iter)
		{
			uint32_t pos = *iter;
			if (pos > player->data->bag_grid_num)
			{
				ret = ERROR_ID_BAGUAPAI_BAG_POS;
				LOG_ERR("[%s:%d] player[%lu] bag pos invalid, pos:%u, bag_size:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, player->data->bag_grid_num);
				break;
			}

			bag_grid_data *grid = &player->data->bag[pos];
			uint32_t card_id = bagua_item_to_card(grid->id);
			BaguaTable *card_config = get_config_by_id(card_id, &bagua_config);
			if (!card_config)
			{
				ret = ERROR_ID_BAGUAPAI_BAG_POS;
				LOG_ERR("[%s:%d] player[%lu] get card config failed, pos:%u, item_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, grid->id, card_id);
				break;
			}

			for (size_t i = 0; i < card_config->n_DecomposeItem; ++i)
			{
				uint32_t item_id = card_config->DecomposeItem[i];
				uint32_t item_num = card_config->DecomposeNum[i];
				gain_map[item_id] += item_num * grid->num;
			}

			if (grid->especial_item.baguapai.star > 0)
			{
				BaguaStarTable *star_config = get_bagua_star_config(grid->especial_item.baguapai.star);
				if (!star_config)
				{
					ret = ERROR_ID_NO_CONFIG;
					LOG_ERR("[%s:%d] player[%lu] get star config failed, pos:%u, item_id:%u, star:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, grid->id, grid->especial_item.baguapai.star);
					break;
				}

				for (size_t i = 0; i < star_config->n_DecomposeCompensation; ++i)
				{
					uint32_t item_id = star_config->DecomposeCompensation[i];
					uint32_t item_num = star_config->DecomposeCompensationNum[i];
					gain_map[item_id] += item_num * grid->num;
				}
			}
		}

		if (ret != 0)
		{
			break;
		}

		if (!player->check_can_add_item_list(gain_map))
		{
			ret = 190500337;
			LOG_ERR("[%s:%d] player[%lu] bag not enough", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		//删除背包里的卡牌
		for (std::set<uint32_t>::iterator iter = grid_ids.begin(); iter != grid_ids.end(); ++iter)
		{
			uint32_t pos = *iter;
			bag_grid_data *grid = &player->data->bag[pos];
			player->del_item_by_pos(pos, grid->num, MAGIC_TYPE_BAGUAPAI_DECOMPOSE);
		}

		//返还材料到背包
		player->add_item_list(gain_map, MAGIC_TYPE_BAGUAPAI_DECOMPOSE);

		player->add_achievement_progress(ACType_BAGUA_DECOMPOSE, 0, 0, 0, grid_ids.size());
	} while(0);

	BaguapaiDecomposeAnswer resp;
	baguapai_decompose_answer__init(&resp);

	ItemData gain_item[3];
	ItemData* gain_item_point[3];

	resp.result = ret;
	resp.n_items = 0;
	resp.items = gain_item_point;
	for (std::map<uint32_t, uint32_t>::iterator iter = gain_map.begin(); iter != gain_map.end(); ++iter)
	{
		gain_item_point[resp.n_items] = &gain_item[resp.n_items];
		item_data__init(&gain_item[resp.n_items]);
		gain_item[resp.n_items].id = iter->first;
		gain_item[resp.n_items].num = iter->second;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_DECOMPOSE_ANSWER, baguapai_decompose_answer__pack, resp);

	return 0;
}

//八卦牌炼星请求
static int handle_baguapai_refine_star_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BaguapaiRefineCommonRequest *req = baguapai_refine_common_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t style_id = req->styleid;
	uint32_t part_id = req->partid;

	baguapai_refine_common_request__free_unpacked(req, NULL);

	int ret = 0;
	BaguapaiCardInfo *card_info = NULL;
	do
	{
		card_info = player->get_baguapai_card(style_id, part_id);
		if (!card_info)
		{
			ret = ERROR_ID_BAGUAPAI_PART_ID;
			LOG_ERR("[%s:%d] player[%lu] get card info failed, style_id:%u, part_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id);
			break;
		}

		uint32_t next_star = card_info->star + 1;
		BaguaStarTable *config = get_bagua_star_config(next_star);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get star config failed, style_id:%u, part_id:%u, star:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, next_star);
			break;
		}

		std::map<uint32_t, uint32_t> cost_map;
		for (size_t i = 0; i < config->n_StarItem; ++i)
		{
			uint32_t item_id = config->StarItem[i];
			uint32_t item_num = config->StarNum[i];
			cost_map[item_id] = item_num;
			uint32_t bag_num = player->get_item_can_use_num(item_id);
			if (bag_num < item_num)
			{
				ret = ERROR_ID_PROP_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] item not enough, style_id:%u, part_id:%u, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, item_id, item_num, bag_num);
				break;
			}
		}

		if (ret != 0)
		{
			break;
		}

		uint32_t has_coin = player->get_coin();
		uint32_t need_coin = config->StarCoin;
		if (has_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, style_id:%u, part_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, need_coin, has_coin);
			break;
		}

		//先扣除消耗
		for (std::map<uint32_t, uint32_t>::iterator iter = cost_map.begin(); iter != cost_map.end(); ++iter)
		{
			player->del_item(iter->first, iter->second, MAGIC_TYPE_BAGUAPAI_REFINE_STAR);
		}
		player->sub_coin(need_coin, MAGIC_TYPE_BAGUAPAI_REFINE_STAR);

		uint32_t rand_val = rand() % 10000;
		if (rand_val < (uint32_t)config->StatProbability)
		{ //成功升星
			card_info->star++;
		}
		else
		{ //失败降星
			if (card_info->star > 0)
			{
				card_info->star--;
			}
		}

		player->calculate_attribute(true);
		player->add_task_progress(TCT_BAGUA_REFINE_STAR, 0, 1);
		player->add_achievement_progress(ACType_BAGUA_REFINE_STAR, 0, 0, 0, 1);
		player->add_achievement_progress(ACType_BAGUA_MIN_STAR, 0, 0, 0, 0);
	} while(0);

	BaguapaiRefineStarAnswer resp;
	baguapai_refine_star_answer__init(&resp);

	resp.result = ret;
	resp.styleid = style_id;
	resp.partid = part_id;
	resp.star = (card_info ? card_info->star : 0);
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_REFINE_STAR_ANSWER, baguapai_refine_star_answer__pack, resp);

	return 0;
}

////八卦牌重铸请求
//static int handle_baguapai_refine_main_attr_request(player_struct *player, EXTERN_DATA *extern_data)
//{
//	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
//	if (!player || !player->is_online())
//	{
//		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
//		return (-1);
//	}
//
//	BaguapaiRefineCommonRequest *req = baguapai_refine_common_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
//	if (!req)
//	{
//		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
//		return (-10);
//	}
//
//	uint32_t style_id = req->styleid;
//	uint32_t part_id = req->partid;
//
//	baguapai_refine_common_request__free_unpacked(req, NULL);
//
//	int ret = 0;
//	BaguapaiCardInfo *card_info = NULL;
//	do
//	{
//		card_info = player->get_baguapai_card(style_id, part_id);
//		if (!card_info)
//		{
//			ret = ERROR_ID_BAGUAPAI_PART_ID;
//			LOG_ERR("[%s:%d] player[%lu] get card info failed, style_id:%u, part_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id);
//			break;
//		}
//
//		BaguaTable *config = get_config_by_id(card_info->id, &bagua_config);
//		if (!config)
//		{
//			ret = ERROR_ID_NO_CONFIG;
//			LOG_ERR("[%s:%d] player[%lu] get card config failed, style_id:%u, part_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, card_info->id);
//			break;
//		}
//
//		std::map<uint32_t, uint32_t> cost_map;
//		for (size_t i = 0; i < config->n_RecastItem; ++i)
//		{
//			uint32_t item_id = config->RecastItem[i];
//			uint32_t item_num = config->RecastNum[i];
//			cost_map[item_id] += item_num;
//			uint32_t bag_num = player->get_item_can_use_num(item_id);
//			if (bag_num < item_num)
//			{
//				ret = ERROR_ID_PROP_NOT_ENOUGH;
//				LOG_ERR("[%s:%d] player[%lu] item not enough, style_id:%u, part_id:%u, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, item_id, item_num, bag_num);
//				break;
//			}
//		}
//
//		if (ret != 0)
//		{
//			break;
//		}
//
//		uint32_t need_coin = config->RecastCoin;
//		uint32_t has_coin = player->get_coin();
//		if (has_coin < need_coin)
//		{
//			ret = ERROR_ID_COIN_NOT_ENOUGH;
//			LOG_ERR("[%s:%d] player[%lu] coin not enough, style_id:%u, part_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, need_coin, has_coin);
//			break;
//		}
//
//		double new_val = 0;
//		if (player->generate_baguapai_main_attr(card_info->id, new_val) != 0)
//		{
//			ret = ERROR_ID_NO_CONFIG;
//			LOG_ERR("[%s:%d] player[%lu] generate attr failed, style_id:%u, part_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, card_info->id);
//			break;
//		}
//
//		//先扣除消耗
//		for (std::map<uint32_t, uint32_t>::iterator iter = cost_map.begin(); iter != cost_map.end(); ++iter)
//		{
//			player->del_item(iter->first, iter->second, MAGIC_TYPE_BAGUAPAI_REFINE_MAIN_ATTR);
//		}
//		player->sub_coin(need_coin, MAGIC_TYPE_BAGUAPAI_REFINE_MAIN_ATTR);
//
//		card_info->main_attr_val_new = new_val;
//		player->add_task_progress(TCT_BAGUA_REFINE_MAIN_ATTR, 0, 1);
//		player->add_achievement_progress(ACType_BAGUA_REFINE_MAIN_ATTR, 0, 0, 1);
//	} while(0);
//
//	BaguapaiRefineMainAttrAnswer resp;
//	baguapai_refine_main_attr_answer__init(&resp);
//
//	resp.result = ret;
//	resp.styleid = style_id;
//	resp.partid = part_id;
//	if (card_info)
//	{
//		resp.mainattrvalnew = card_info->main_attr_val_new;
//	}
//	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_REFINE_MAIN_ATTR_ANSWER, baguapai_refine_main_attr_answer__pack, resp);
//
//	return 0;
//}
//
////八卦牌重铸保留请求
//static int handle_baguapai_retain_main_attr_request(player_struct *player, EXTERN_DATA *extern_data)
//{
//	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
//	if (!player || !player->is_online())
//	{
//		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
//		return (-1);
//	}
//
//	BaguapaiRefineCommonRequest *req = baguapai_refine_common_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
//	if (!req)
//	{
//		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
//		return (-10);
//	}
//
//	uint32_t style_id = req->styleid;
//	uint32_t part_id = req->partid;
//
//	baguapai_refine_common_request__free_unpacked(req, NULL);
//
//	int ret = 0;
//	BaguapaiCardInfo *card_info = NULL;
//	do
//	{
//		card_info = player->get_baguapai_card(style_id, part_id);
//		if (!card_info)
//		{
//			ret = ERROR_ID_BAGUAPAI_PART_ID;
//			LOG_ERR("[%s:%d] player[%lu] get card info failed, style_id:%u, part_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id);
//			break;
//		}
//
//		if (card_info->main_attr_val_new == 0)
//		{
//			ret = ERROR_ID_BAGUAPAI_NO_RETAIN_ATTR;
//			LOG_ERR("[%s:%d] player[%lu] no retain attr, style_id:%u, part_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id);
//			break;
//		}
//
//		card_info->main_attr_val = card_info->main_attr_val_new;
//		card_info->main_attr_val_new = 0;
//
//		player->calculate_attribute(true);
//	} while(0);
//
//	BaguapaiRetainMainAttrAnswer resp;
//	baguapai_retain_main_attr_answer__init(&resp);
//
//	resp.result = ret;
//	resp.styleid = style_id;
//	resp.partid = part_id;
//	if (card_info)
//	{
//		resp.mainattrval = card_info->main_attr_val;
//		resp.mainattrvalnew = card_info->main_attr_val_new;
//	}
//	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_RETAIN_MAIN_ATTR_ANSWER, baguapai_retain_main_attr_answer__pack, resp);
//
//	return 0;
//}

//八卦牌洗炼请求
static int handle_baguapai_refine_minor_attr_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BaguapaiRefineMinorAttrRequest *req = baguapai_refine_minor_attr_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t style_id = req->styleid;
	uint32_t part_id = req->partid;
	uint32_t type = req->type;

	baguapai_refine_minor_attr_request__free_unpacked(req, NULL);

	int ret = 0;
	BaguapaiCardInfo *card_info = NULL;
	do
	{
		if (type != 1 && type != 2)
		{
			ret = 1;
			break;
		}

		card_info = player->get_baguapai_card(style_id, part_id);
		if (!card_info)
		{
			ret = ERROR_ID_BAGUAPAI_PART_ID;
			LOG_ERR("[%s:%d] player[%lu] get card info failed, style_id:%u, part_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id);
			break;
		}

		BaguaTable *config = get_config_by_id(card_info->id, &bagua_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get card config failed, style_id:%u, part_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, card_info->id);
			break;
		}

		std::map<uint32_t, uint32_t> cost_map;
		uint32_t n_ClearItem = 0;
		uint64_t *ClearItem = NULL, *ClearNum = NULL;
		uint32_t need_coin = 0;
		if (type == 1)
		{
			n_ClearItem = config->n_ClearItem1;
			ClearItem = config->ClearItem1;
			ClearNum = config->ClearNum1;
			need_coin = config->ClearCoin1;
		}
		else
		{
			n_ClearItem = config->n_ClearItem2;
			ClearItem = config->ClearItem2;
			ClearNum = config->ClearNum2;
			need_coin = config->ClearCoin2;
		}

		for (size_t i = 0; i < n_ClearItem; ++i)
		{
			uint32_t item_id = ClearItem[i];
			uint32_t item_num = ClearNum[i];
			cost_map[item_id] += item_num;
			uint32_t bag_num = player->get_item_can_use_num(item_id);
			if (bag_num < item_num)
			{
				ret = ERROR_ID_PROP_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] item not enough, style_id:%u, part_id:%u, type:%u, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, type, item_id, item_num, bag_num);
				break;
			}
		}

		if (ret != 0)
		{
			break;
		}

		uint32_t has_coin = player->get_coin();
		if (has_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, style_id:%u, part_id:%u, type:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, type, need_coin, has_coin);
			break;
		}

		CommonRandAttrInfo attr_new[MAX_BAGUAPAI_MINOR_ATTR_NUM];
		memset(attr_new, 0, sizeof(attr_new));
		if (player->generate_baguapai_minor_attr(card_info->id, attr_new, type) != 0)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] generate attr failed, style_id:%u, part_id:%u, type:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, type, card_info->id);
			break;
		}

		//先扣除消耗
		for (std::map<uint32_t, uint32_t>::iterator iter = cost_map.begin(); iter != cost_map.end(); ++iter)
		{
			player->del_item(iter->first, iter->second, MAGIC_TYPE_BAGUAPAI_REFINE_MINOR_ATTR);
		}
		player->sub_coin(need_coin, MAGIC_TYPE_BAGUAPAI_REFINE_MINOR_ATTR);

		memset(card_info->minor_attrs_new, 0, sizeof(card_info->minor_attrs_new));
		memcpy(card_info->minor_attrs_new, attr_new, sizeof(card_info->minor_attrs_new));
		player->add_task_progress(TCT_BAGUA_REFINE_MINOR_ATTR, 0, 1);
		player->add_achievement_progress(ACType_BAGUA_REFINE_MINOR_ATTR, 0, 0, 0, 1);
	} while(0);

	BaguapaiRefineMinorAttrAnswer resp;
	baguapai_refine_minor_attr_answer__init(&resp);

	CommonRandAttrData minor_new_attr[MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* minor_new_attr_point[MAX_BAGUAPAI_MINOR_ATTR_NUM];

	resp.result = ret;
	resp.styleid = style_id;
	resp.partid = part_id;
	if (card_info)
	{
		uint32_t attr_num = 0;
		for (int i = 0; i < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++i)
		{
			if (card_info->minor_attrs_new[i].id == 0)
			{
				break;
			}

			minor_new_attr_point[i] = &minor_new_attr[i];
			common_rand_attr_data__init(&minor_new_attr[i]);
			minor_new_attr[i].pool = card_info->minor_attrs_new[i].pool;
			minor_new_attr[i].id = card_info->minor_attrs_new[i].id;
			minor_new_attr[i].val = card_info->minor_attrs_new[i].val;
			attr_num++;
		}
		resp.minorattrsnew = minor_new_attr_point;
		resp.n_minorattrsnew = attr_num;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_REFINE_MINOR_ATTR_ANSWER, baguapai_refine_minor_attr_answer__pack, resp);

	return 0;
}

//八卦牌洗炼保留请求
static int handle_baguapai_retain_minor_attr_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BaguapaiRefineCommonRequest *req = baguapai_refine_common_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t style_id = req->styleid;
	uint32_t part_id = req->partid;

	baguapai_refine_common_request__free_unpacked(req, NULL);

	int ret = 0;
	BaguapaiCardInfo *card_info = NULL;
	do
	{
		card_info = player->get_baguapai_card(style_id, part_id);
		if (!card_info)
		{
			ret = ERROR_ID_BAGUAPAI_PART_ID;
			LOG_ERR("[%s:%d] player[%lu] get card info failed, style_id:%u, part_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id);
			break;
		}

		if (card_info->minor_attrs_new[0].id == 0)
		{
			ret = ERROR_ID_BAGUAPAI_NO_RETAIN_ATTR;
			LOG_ERR("[%s:%d] player[%lu] no retain attr, style_id:%u, part_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id);
			break;
		}

		memset(card_info->minor_attrs, 0, sizeof(card_info->minor_attrs));
		memcpy(card_info->minor_attrs, card_info->minor_attrs_new, sizeof(card_info->minor_attrs));
		memset(card_info->minor_attrs_new, 0, sizeof(card_info->minor_attrs_new));

		player->calculate_attribute(true);
	} while(0);

	BaguapaiRetainMinorAttrAnswer resp;
	baguapai_retain_minor_attr_answer__init(&resp);

	CommonRandAttrData minor_attr[MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* minor_attr_point[MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData minor_new_attr[MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* minor_new_attr_point[MAX_BAGUAPAI_MINOR_ATTR_NUM];

	resp.result = ret;
	resp.styleid = style_id;
	resp.partid = part_id;
	if (card_info)
	{
		int cur_num = 0;
		for (int i = 0; i < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++i)
		{
			if (card_info->minor_attrs[i].id == 0)
			{
				break;
			}

			minor_attr_point[i] = &minor_attr[i];
			common_rand_attr_data__init(&minor_attr[i]);
			minor_attr[i].pool = card_info->minor_attrs[i].pool;
			minor_attr[i].id = card_info->minor_attrs[i].id;
			minor_attr[i].val = card_info->minor_attrs[i].val;
			cur_num++;
		}
		resp.minorattrs = minor_attr_point;
		resp.n_minorattrs = cur_num;

		int new_num = 0;
		for (int i = 0; i < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++i)
		{
			if (card_info->minor_attrs_new[i].id == 0)
			{
				break;
			}

			minor_new_attr_point[i] = &minor_new_attr[i];
			common_rand_attr_data__init(&minor_new_attr[i]);
			minor_new_attr[i].pool = card_info->minor_attrs_new[i].pool;
			minor_new_attr[i].id = card_info->minor_attrs_new[i].id;
			minor_new_attr[i].val = card_info->minor_attrs_new[i].val;
			new_num++;
		}
		resp.minorattrsnew = minor_new_attr_point;
		resp.n_minorattrsnew = new_num;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_RETAIN_MINOR_ATTR_ANSWER, baguapai_retain_minor_attr_answer__pack, resp);

	return 0;
}

//八卦牌新洗炼请求
static int handle_baguapai_move_additional_attr_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BaguapaiMoveAdditionalAttrRequest *req = baguapai_move_additional_attr_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t src_grid_pos = req->fromgridid;
	uint32_t src_attr_pos = req->fromattrindex - 1;
	uint32_t dest_attr_pos = req->toattrindex - 1;

	baguapai_move_additional_attr_request__free_unpacked(req, NULL);

	int ret = 0;
	BaguapaiCardInfo *card_info = NULL;
	uint32_t style_id = 0;
	uint32_t part_id = 0;
	do
	{
		if (src_grid_pos > player->data->bag_grid_num)
		{
			ret = ERROR_ID_BAGUAPAI_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] bag pos invalid, src_grid_pos:%u, bag_size:%u", __FUNCTION__, __LINE__, extern_data->player_id, src_grid_pos, player->data->bag_grid_num);
			break;
		}
		if (src_attr_pos >= MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM || dest_attr_pos >= MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM)
		{
			ret = 1905;
			LOG_ERR("[%s:%d] player[%lu] attr pos invalid, src_attr_pos:%u, dest_attr_pos:%u", __FUNCTION__, __LINE__, extern_data->player_id, src_attr_pos, dest_attr_pos);
			break;
		}

		bag_grid_data *src_grid = &player->data->bag[src_grid_pos];
		uint32_t src_card_id = bagua_item_to_card(src_grid->id);
		BaguaTable *src_card_config = get_config_by_id(src_card_id, &bagua_config);
		if (!src_card_config)
		{
			ret = ERROR_ID_BAGUAPAI_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] get card config failed, src_grid_pos:%u, item_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, src_grid_pos, src_grid->id, src_card_id);
			break;
		}

		style_id = src_card_config->BaguaType;
		part_id = src_card_config->BaguaPosition;
		card_info = player->get_baguapai_card(style_id, part_id);
		if (!card_info)
		{
			ret = ERROR_ID_BAGUAPAI_PART_ID;
			LOG_ERR("[%s:%d] player[%lu] get card info failed, style_id:%u, part_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id);
			break;
		}

		BaguaTable *dest_card_config = get_config_by_id(card_info->id, &bagua_config);
		if (!dest_card_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get card config failed, style_id:%u, part_id:%u, card_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, style_id, part_id, card_info->id);
			break;
		}

		if (src_card_config->Level > dest_card_config->Level)
		{
			ret = 1905;
			LOG_ERR("[%s:%d] player[%lu] src_card_lv:%lu, dest_card_lv:%lu", __FUNCTION__, __LINE__, extern_data->player_id, src_card_config->Level, dest_card_config->Level);
			break;
		}

		if (src_grid->especial_item.baguapai.additional_attrs[src_attr_pos].id == 0)
		{
			ret = 1905;
			LOG_ERR("[%s:%d] player[%lu] src attr is 0, src_grid_pos:%u, src_attr_pos:%u", __FUNCTION__, __LINE__, extern_data->player_id, src_grid_pos, src_attr_pos);
			break;
		}

		int dest_same_attr_pos = -1;
		int dest_empty_attr_pos = -1;
		for (int i = 0; i < MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM; ++i)
		{
			if (card_info->additional_attrs[i].id == src_grid->especial_item.baguapai.additional_attrs[src_attr_pos].id)
			{
				dest_same_attr_pos = i;
			}
			else if (card_info->additional_attrs[i].id == 0 && dest_empty_attr_pos < 0)
			{
				dest_empty_attr_pos = i;
			}
		}

		if (dest_same_attr_pos < 0)
		{ //没有相同属性条目
		}
		else
		{ //有相同属性条目
			if (dest_attr_pos != (uint32_t)dest_same_attr_pos)
			{
				ret = 1905; //不能让目标八卦牌拥有两条相同的属性
				LOG_ERR("[%s:%d] player[%lu] dest has same attr, can't replace another attr, src_grid_pos:%u, src_attr_pos:%u, dest_attr_pos:%u, dest_same_attr_pos:%u", __FUNCTION__, __LINE__, extern_data->player_id, src_grid_pos, src_attr_pos, dest_attr_pos, dest_same_attr_pos);
				break;
			}

			if (src_grid->especial_item.baguapai.additional_attrs[src_attr_pos].val < card_info->additional_attrs[dest_attr_pos].val)
			{
				ret = 1905; //材料属性值比目标属性值低，不能替换
				LOG_ERR("[%s:%d] player[%lu] src_attr_val < dest_attr_val, src_grid_pos:%u, src_attr_pos:%u, dest_attr_pos:%u, src_attr_val:%f, dest_attr_val:%f", __FUNCTION__, __LINE__, extern_data->player_id, src_grid_pos, src_attr_pos, dest_attr_pos, src_grid->especial_item.baguapai.additional_attrs[src_attr_pos].val, card_info->additional_attrs[dest_attr_pos].val);
				break;
			}
		}

		std::map<uint32_t, uint32_t> cost_map;
		for (size_t i = 0; i < dest_card_config->n_RecastItem; ++i)
		{
			uint32_t item_id = dest_card_config->RecastItem[i];
			uint32_t item_num = dest_card_config->RecastNum[i];
			cost_map[item_id] += item_num;
			uint32_t bag_num = player->get_item_can_use_num(item_id);
			if (bag_num < item_num)
			{
				ret = ERROR_ID_PROP_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] item not enough, dest_card_id:%u, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, card_info->id, item_id, item_num, bag_num);
				break;
			}
		}

		if (ret != 0)
		{
			break;
		}

		uint32_t need_coin = dest_card_config->RecastCoin;
		uint32_t has_coin = player->get_coin();
		if (has_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, dest_card_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, card_info->id, need_coin, has_coin);
			break;
		}


		//先扣除消耗
		for (std::map<uint32_t, uint32_t>::iterator iter = cost_map.begin(); iter != cost_map.end(); ++iter)
		{
			player->del_item(iter->first, iter->second, MAGIC_TYPE_BAGUAPAI_MOVE_ADDITIONAL_ATTR);
		}
		player->sub_coin(need_coin, MAGIC_TYPE_BAGUAPAI_MOVE_ADDITIONAL_ATTR);

		if (card_info->additional_attrs[dest_attr_pos].id == 0 && dest_empty_attr_pos >= 0 && (uint32_t)dest_empty_attr_pos != dest_attr_pos)
		{
			dest_attr_pos = dest_empty_attr_pos;
		}

		memset(&card_info->additional_attrs[dest_attr_pos], 0, sizeof(CommonRandAttrInfo));
		memcpy(&card_info->additional_attrs[dest_attr_pos], &src_grid->especial_item.baguapai.additional_attrs[src_attr_pos], sizeof(CommonRandAttrInfo));
		{
			uint32_t last_pos = MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM - 1;
			if (src_attr_pos != last_pos)
			{
				memmove(&src_grid->especial_item.baguapai.additional_attrs[src_attr_pos], &src_grid->especial_item.baguapai.additional_attrs[src_attr_pos + 1], sizeof(CommonRandAttrInfo) * (last_pos - src_attr_pos));
			}
			memset(&src_grid->especial_item.baguapai.additional_attrs[last_pos], 0, sizeof(CommonRandAttrInfo));
			player->update_bag_grid(src_grid_pos);
		}

		player->calculate_attribute(true);
//		player->add_task_progress(TCT_BAGUA_REFINE_MINOR_ATTR, 0, 1);
//		player->add_achievement_progress(ACType_BAGUA_REFINE_MINOR_ATTR, 0, 0, 1);
	} while(0);

	BaguapaiMoveAdditionalAttrAnswer resp;
	baguapai_move_additional_attr_answer__init(&resp);

	CommonRandAttrData  additional_attr[MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];
	CommonRandAttrData* additional_attr_point[MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];

	resp.result = ret;
	resp.styleid = style_id;
	resp.partid = part_id;
	if (card_info)
	{
		uint32_t attr_num = 0;
		for (int i = 0; i < MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM; ++i)
		{
			if (card_info->additional_attrs[i].id == 0)
			{
				break;
			}

			additional_attr_point[i] = &additional_attr[i];
			common_rand_attr_data__init(&additional_attr[i]);
			additional_attr[i].pool = card_info->additional_attrs[i].pool;
			additional_attr[i].id = card_info->additional_attrs[i].id;
			additional_attr[i].val = card_info->additional_attrs[i].val;
			attr_num++;
		}
		resp.additionalattrs = additional_attr_point;
		resp.n_additionalattrs = attr_num;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAGUAPAI_MOVE_ADDITIONAL_ATTR_ANSWER, baguapai_move_additional_attr_answer__pack, resp);

	return 0;
}

//领取活跃度奖励请求
static int handle_active_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	ActiveRewardRequest *req = active_reward_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t reward_id = req->rewardid;

	active_reward_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		uint32_t *pReward = NULL;
		for (int i = 0; i < MAX_ACTIVE_REWARD_NUM; ++i)
		{
			uint32_t tmp_id = player->data->active_reward[i];
			if (tmp_id == 0 || tmp_id == reward_id)
			{
				pReward = &player->data->active_reward[i];
				break;
			}
		}

		if (!pReward)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] memory not enough, reward_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, reward_id);
			break;
		}

		if (*pReward > 0)
		{
			ret = ERROR_ID_ACTIVE_REWARD_HAS_GOT;
			LOG_ERR("[%s:%d] player[%lu] reward has got, reward_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, reward_id);
			break;
		}

		ActiveTable *config = get_config_by_id(reward_id, &activity_activeness_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get config failed, reward_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, reward_id);
			break;
		}

		uint32_t need_activeness = config->Active;
		uint32_t has_activeness = (uint32_t)player->get_attr(PLAYER_ATTR_ACTIVENESS);
		if (has_activeness < need_activeness)
		{
			ret = ERROR_ID_ACTIVENESS_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] activeness not enough, reward_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, reward_id, need_activeness, has_activeness);
			break;
		}

		//发放奖励
		for (size_t i = 0; i < config->n_Reward; ++i)
		{
			player->add_item(config->Reward[i], config->RewardNum[i], MAGIC_TYPE_ACTIVE_REWARD);
		}

		//标记已领
		*pReward = reward_id;
	} while(0);

	ActiveRewardAnswer resp;
	active_reward_answer__init(&resp);

	resp.result = ret;
	resp.rewardid = reward_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ACTIVE_REWARD_ANSWER, active_reward_answer__pack, resp);

	return 0;
}




extern bool CheckGuoyuTaskOpen(int level, int &cd);

void send_cur_chengjie_task(player_struct *player, EXTERN_DATA *extern_data)
{
	if (player->data->chengjie.cur_task != 0)
	{
		STChengJie *pTask = ChengJieTaskManage::FindTask(player->data->chengjie.cur_task);
		if (pTask == NULL || (ChengJieTaskManage::GetTaskAccept(pTask->pid) != player->get_uuid() && !pTask->complete))
		{
			player->data->chengjie.cur_task = 0;
		}
		else
		{
			ChengjieTask send;
			chengjie_task__init(&send);
			if (ChengJieTaskManage::PackTask(*pTask, send))
			{
				conn_node_gamesrv::connecter.send_to_friend(extern_data, MSG_ID_CUR_CHENGJIE_TASK_NOTIFY, &send, (pack_func)chengjie_task__pack);
			}
			else
			{
				fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CUR_CHENGJIE_TASK_NOTIFY, chengjie_task__pack, send);
			}
		}
	}
}
static int on_login_send_auto_add_hp_data(player_struct *player, EXTERN_DATA *extern_data)
{
	AutoAddHpSetData data;
	auto_add_hp_set_data__init(&data);
	data.auto_add_hp_item_id = player->data->auto_add_hp_item_id;
	data.auto_add_hp_percent = player->data->auto_add_hp_percent;
	data.open_auto_add_hp = player->data->open_auto_add_hp;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_AUTO_ADD_HP_SET_ANSWER, auto_add_hp_set_data__pack, data);

	player->send_hp_pool_changed_notify();
	return (0);
}

static int on_login_send_yaoshi(player_struct *player, EXTERN_DATA *extern_data)
{
	if (player->data->guoyu.guoyu_level == 0)
	{
		player->data->next_update = time_helper::nextOffsetTime(5 * 3600, time_helper::get_cached_time() / 1000);

		player->data->guoyu.guoyu_level = 1;
		player->data->guoyu.type = 1;
		player->data->guoyu.award = false;
		TypeLevelTable *table = get_guoyu_level_table(GUOYU__TASK__TYPE__CRITICAL);
		if (table != NULL)
		{
			player->data->guoyu.guoyu_num = table->RewardTime;
			player->data->guoyu.critical_num = table->ShowTimes;
		}

		player->data->chengjie.level = 1;
		player->data->change_special_num = 1;
		ParameterTable *param_config = get_config_by_id(161000070, &parameter_config);
		if (param_config != NULL)
		{
			player->data->chengjie.chengjie_num = param_config->parameter1[0];
		}

		param_config = get_config_by_id(161000106, &parameter_config);
		if (param_config != NULL)
		{
			player->data->shangjin.shangjin_num = param_config->parameter1[0];
		}
		player->data->shangjin.level = 1;
		ShangjinManage::RefreshTask(player);
		SpecialtySkillTable *tableSkill = get_yaoshi_skill_config(SHANGJIN_THREE, player->data->shangjin.level);
		if (tableSkill != NULL)
		{
			player->data->shangjin.free = tableSkill->EffectValue[0];
		}
		player->data->shangjin.free = 2;
	}

	Yaoshi send;
	yaoshi__init(&send);
	send.cur_major = player->data->cur_yaoshi;
	GuoyuType guoYu;
	guoyu_type__init(&guoYu);
	send.guoyu = &guoYu;
	send.change_num = player->data->change_special_num;
	guoYu.guoyu_level = player->data->guoyu.guoyu_level;
	guoYu.cur_exp = player->data->guoyu.cur_exp;
	guoYu.cur_task = player->data->guoyu.cur_task;
	guoYu.type = player->data->guoyu.type;
	guoYu.award = player->data->guoyu.award;
	//CheckGuoyuTaskOpen(GUOYU__TASK__TYPE__CRITICAL, player->data->guoyu.critical_cd);
	uint64_t now = time_helper::get_cached_time() / 1000;
	if (player->data->guoyu.task_timeout > now)
	{
		guoYu.task_cd = player->data->guoyu.task_timeout - now;
	}
	else
	{
		guoYu.task_cd = 0;
	}
	if (player->data->guoyu.critical_cd > now)
	{
		guoYu.critical_cd = player->data->guoyu.critical_cd - now;
	}
	else
	{
		guoYu.critical_cd = 0;
	}
	guoYu.critical_num = player->data->guoyu.critical_num;
	guoYu.guoyu_num = player->data->guoyu.guoyu_num;// = 999;
	guoYu.map = player->data->guoyu.map;

	ChengjieType chengJie;
	chengjie_type__init(&chengJie);
	send.chengjie = &chengJie;
	chengJie.level = player->data->chengjie.level;
	chengJie.num = player->data->chengjie.chengjie_num;
	chengJie.cur_exp = player->data->chengjie.cur_exp;

	ShangjinType shangJin;
	shangjin_type__init(&shangJin);
	send.shangjin = &shangJin;
	shangJin.free_refresh = player->data->shangjin.free;
	shangJin.level = player->data->shangjin.level;
	shangJin.num = player->data->shangjin.shangjin_num;
	shangJin.cur_exp = player->data->shangjin.cur_exp;
	shangJin.cur_task = player->data->shangjin.cur_task;
	shangJin.accept = player->data->shangjin.accept;
	ShangjinTaskType arrTask[MAX_SHANGJIN_NUM];
	ShangjinTaskType *arrTaskPoint[MAX_SHANGJIN_NUM];// = {arrTask, arrTask + 1, arrTask + 2};
	for (int i = 0; i < MAX_SHANGJIN_NUM; ++i)
		arrTaskPoint[i] = &arrTask[i];
	shangJin.task_type = arrTaskPoint;
	shangJin.n_task_type = MAX_SHANGJIN_NUM;
//	ShangjinTaskAward arrAward[MAX_SHANGJIN_NUM][MAX_SHANGJIN_AWARD_NUM];
//	ShangjinTaskAward *arrAwardPoint[MAX_SHANGJIN_NUM][MAX_SHANGJIN_AWARD_NUM];
	int index = 0;
	for (int i = 0; i < MAX_SHANGJIN_NUM; ++i)
	{
		shangjin_task_type__init(arrTask + i);
		arrTask[i].id = player->data->shangjin.task[i].id;
		arrTask[i].quality = player->data->shangjin.task[i].quality;
		arrTask[i].reduce = player->data->shangjin.task[i].reduce;
		arrTask[i].coin = player->data->shangjin.task[i].coin;
		arrTask[i].exp = player->data->shangjin.task[i].exp;
		arrTask[i].n_award = pack_drop_config_item(player->data->shangjin.task[i].drop_id,
			MAX_SHANGJIN_AWARD_NUM, &index, &arrTask[i].award);
		
		// arrTask[i].n_award = player->data->shangjin.task[i].n_award;
		// for (uint32_t j = 0; j < player->data->shangjin.task[i].n_award; ++j)
		// {
		// 	shangjin_task_award__init(&arrAward[i][j]);// + i * MAX_SHANGJIN_AWARD_NUM + j);
		// 	arrAward[i][j].id = player->data->shangjin.task[i].award[j].id;
		// 	arrAward[i][j].num = player->data->shangjin.task[i].award[j].val;
		// 	arrAwardPoint[i][j] = &arrAward[i][j];// arrAward + i * j;
		// }
		// arrTask[i].award = arrAwardPoint[i];
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_YAOSHI_NOTIFY, yaoshi__pack, send);

	send_cur_chengjie_task(player, extern_data);
	if (player->data->chengjie.target != 0)
	{
		player_struct *target = player_manager::get_player_by_id(player->data->chengjie.target);
		if (target != NULL)
		{
			ChengjieKiller send;
			chengjie_killer__init(&send);
			send.playerid = player->get_uuid();

			EXTERN_DATA extern_data;
			extern_data.player_id = target->get_uuid();
			fast_send_msg(&conn_node_gamesrv::connecter, &extern_data, MSG_ID_CHENGJIE_KILLER_NOTIFY, chengjie_killer__pack, send);
		}
	}

	return 0;
}
static int refresh_guoyu_task(player_struct *player, int type, bool isItem)
{
	EXTERN_DATA extern_data;
	RandomMonsterTable *table = get_random_monster_config(type, player->data->guoyu.guoyu_level);
	while (table == NULL)
	{
		//LOG_ERR("[%s:%d] can not get guoyu task table player[%lu] ", __FUNCTION__, __LINE__, player->get_uuid());
		//return 1;
		table = get_random_monster_config(type, player->data->guoyu.guoyu_level);
	}
	RandomDungeonTable *tableFb = get_random_guoyu_fb_config(type);
	if (tableFb == NULL)
	{
		LOG_ERR("[%s:%d] can not get guoyu task table player[%lu] ", __FUNCTION__, __LINE__, player->get_uuid());
		return 2;
	}
	if (player->m_team == NULL)
	{
		return 3;
	}
	for (int i = 0; i < player->m_team->m_data->m_memSize; ++i)
	{
		player_struct *mem = player_manager::get_player_by_id(player->m_team->m_data->m_mem[i].id);
		if (mem == NULL)
		{
			continue;
		}
		//if (mem->data->guoyu.cur_task != 0 && i > 0)
		//{
		//	continue;
		//}
		mem->data->guoyu.cur_task = table->ID;
		mem->data->guoyu.map = tableFb->ResID;
		mem->data->guoyu.random_map = tableFb->ID;
		mem->data->guoyu.type = type;
		mem->data->guoyu.task_timeout = time_helper::get_cached_time() / 1000 + table->Times;
		UpdateGuoyuTask send;
		update_guoyu_task__init(&send);
		send.id = mem->data->guoyu.cur_task;
		send.cd = table->Times;
		send.ret = 0;
		send.map = tableFb->ResID;
		send.type = type;
		send.refresh = isItem;
		extern_data.player_id = player->m_team->m_data->m_mem[i].id;	

		if (mem->data->guoyu.guoyu_num > 0 && !isItem)
		{
			--mem->data->guoyu.guoyu_num;
			if (mem->data->guoyu.type == GUOYU__TASK__TYPE__CRITICAL)
			{
				--mem->data->guoyu.critical_num;
			}
			YaoshiNumber notify1;
			yaoshi_number__init(&notify1);
			notify1.critical_num = mem->data->guoyu.critical_num;
			notify1.guoyu_num = mem->data->guoyu.guoyu_num;
			fast_send_msg(&conn_node_gamesrv::connecter, &extern_data, MSG_ID_YAOSHI_NUM_NOTIFY, yaoshi_number__pack, notify1);
			mem->data->guoyu.award = true;
		}
		send.award = mem->data->guoyu.award;
		
		fast_send_msg(&conn_node_gamesrv::connecter, &extern_data, MSG_ID_GUOYU_TASK_NOTIFY, update_guoyu_task__pack, send);
	}
	
	return 0;
}
static int CheckAcceptGuoyuTask(player_struct *player, int level, bool isItem, uint64_t *pid, uint32_t &n_pid)
{
	
	int tmp = 0;
	if (!CheckGuoyuTaskOpen(level, tmp))
	{
		return 190500104;
	}
	TypeLevelTable *table = get_guoyu_level_table(level);
	if (table == NULL)
	{
		return 1;
	}
	if (player->get_attr(PLAYER_ATTR_LEVEL) < table->Level)
	{
		pid[0] = player->get_uuid();
		n_pid = 1;
		return 190500107;
	}
	if (player->m_team == NULL)
	{
		return 190500119;
	}
	if (player->m_team->GetLeadId() != player->get_uuid())
	{
		return 190500105;
	}
	
	
	if (level == GUOYU__TASK__TYPE__CRITICAL)
	{
		if (player->data->guoyu.critical_cd <= time_helper::get_cached_time() / 1000)
		{
			return 190500104;
		}
	}

	std::vector<uint64_t> cl;
	std::vector<uint64_t> coff;
	std::vector<uint64_t> ctask;
	std::vector<uint64_t> cdis;
	std::vector<uint64_t>::iterator it;
	for (int i = 0; i < player->m_team->m_data->m_memSize; ++i)
	{
		uint64_t pid = player->m_team->m_data->m_mem[i].id;
		player_struct *mem = player_manager::get_player_by_id(pid);
		if (mem == NULL)
		{
			coff.push_back(pid);
			continue;
		}
		if (mem->data->guoyu.cur_task != 0 && i > 0)
		{
			ctask.push_back(pid);
		}
		if (mem->scene != player->scene)
		{
			cdis.push_back(pid);
		}
		if (abs(mem->get_pos()->pos_x - player->get_pos()->pos_x) > 20 || abs(mem->get_pos()->pos_z - player->get_pos()->pos_z) > 20)
		{
			cdis.push_back(pid);
		}
		if (mem->get_attr(PLAYER_ATTR_LEVEL) < table->Level)
		{
			cl.push_back(pid);
		}
	}
	int i = 0;
	if (coff.size() > 0)
	{
		for (it = coff.begin(),i = 0; it != coff.end(); ++it, ++i)
		{
			pid[i] = *it;
		}
		n_pid = coff.size();
		return 190500109;
	}
	if (cl.size() > 0)
	{
		for (it = cl.begin(), i = 0; it != cl.end(); ++it, ++i)
		{
			pid[i] = *it;
		}
		n_pid = cl.size();
		return 190500107;
	}
	if (cdis.size() > 0)
	{
		for (it = cdis.begin(), i = 0; it != cdis.end(); ++it, ++i)
		{
			pid[i] = *it;
		}
		n_pid = cdis.size();
		return 190500050;
	}
	if (ctask.size() > 0 && !isItem)
	{
		for (it = ctask.begin(), i = 0; it != ctask.end(); ++it, ++i)
		{
			pid[i] = *it;
		}
		n_pid = ctask.size();
		return 190500120;
	}

	if (isItem)
	{
		for (it = ctask.begin(), i = 0; it != ctask.end(); ++it)
		{
			player_struct *mem = player_manager::get_player_by_id(*it);
			if (mem == NULL)
			{
				return 190500109;
			}
			if (mem->data->guoyu.cur_task != player->data->guoyu.cur_task)
			{
				RandomMonsterTable *taskMy = get_config_by_id(player->data->guoyu.cur_task, &random_monster);
				RandomMonsterTable *taskOther = get_config_by_id(mem->data->guoyu.cur_task, &random_monster);
				if (taskOther == NULL || taskMy == NULL)
				{
					return 5;
				}
				if (taskMy->TypeLevel != taskOther->TypeLevel)
				{
					pid[i++] = *it;
				}
			}
		}
		if (i > 0)
		{
			n_pid = i;
			return 190500147;
		}
		ParameterTable *param_config = get_config_by_id(161000084, &parameter_config);
		if (param_config == NULL)
		{
			return 2;
		}
		if (player->del_item(param_config->parameter1[0], param_config->parameter1[1], MAGIC_TYPE_YAOSHI) < 0)
		{
			return 190500123;
		}
	}
	return 0;
}
static int handle_accect_guoyu_task_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	ReqChoseGuoyuTask *req = req_chose_guoyu_task__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int type = req->type;
	bool refresh = req->refresh;
	req_chose_guoyu_task__free_unpacked(req, NULL);

	uint64_t pid[MAX_TEAM_MEM + 1];
	uint32_t n_pid = 0;
	int ret = CheckAcceptGuoyuTask(player, type, refresh, pid, n_pid);
	if (ret != 0)
	{
		UpdateGuoyuTask send;
		update_guoyu_task__init(&send);
		send.ret = ret;
		send.pid = pid;
		send.n_pid = n_pid;
		send.id = 0;
		send.cd = 0;
		send.map = 0;
		send.type = 0;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUOYU_TASK_NOTIFY, update_guoyu_task__pack, send);
		return 1;
	}
	if (player->m_team != NULL && player->m_team->GetMemberSize() > 1)
	{
		ReqChoseGuoyuTask tmp;
		req_chose_guoyu_task__init(&tmp);
		player->m_team->m_data->m_agreed = 0;
		player->m_team->m_data->m_guoyuType = type;
		player->m_team->m_data->m_guoyuItem = refresh;
		player->m_team->BroadcastToTeam(MSG_ID_ACCECT_GUOYU_TASK_NOTIFY, &tmp, (pack_func)req_chose_guoyu_task__pack, player->get_uuid());
	}
	else
	{
		refresh_guoyu_task(player, type, refresh);
	}
	
	return (0);
}
static int handle_agreed_guoyu_task_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	if (player->m_team == NULL)
	{
		return -2;
	}
	GuoyuSucc *req = guoyu_succ__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	bool accept = req->succ;
	guoyu_succ__free_unpacked(req, NULL);

	if (accept)
	{
		++player->m_team->m_data->m_agreed;
		if (player->m_team->m_data->m_agreed == player->m_team->GetMemberSize() - 1)
		{
			refresh_guoyu_task(player, player->m_team->m_data->m_guoyuType, player->m_team->m_data->m_guoyuItem);
		}
	}
	else
	{
		GuoyuName notify;
		guoyu_name__init(&notify);
		notify.name = player->get_name();
		EXTERN_DATA ext_data;
		ext_data.player_id = player->m_team->GetLeadId();
		fast_send_msg(&conn_node_gamesrv::connecter, &ext_data, MSG_ID_REFUCE_GUOYU_TASK_NOTIFY, guoyu_name__pack, notify);
	}
	return 0;
}
static int handle_giveup_guoyu_task_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	player->data->guoyu.cur_task = 0;
	player->data->guoyu.award = false;
	GiveupGuoyuTask send;
	giveup_guoyu_task__init(&send);
	send.ret = 0;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GIVEUP_GUOYU_TASK_ANSWER, giveup_guoyu_task__pack, send);
	//send_comm_answer(MSG_ID_GIVEUP_GUOYU_TASK_ANSWER, 0, extern_data);

	return (0);
}
static int handle_guoyu_boss_appear_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	if (player->m_team == NULL)
	{
		return -2;
	}

	BossId *req = boss_id__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int bossId = req->id;
	float x = req->x;
	float z = req->z;
	float y = req->y;
	boss_id__free_unpacked(req, NULL);

	if (player->data->scene_id < SCENCE_DEPART)
	{
		return -3;
	}

	raid_struct *raid = (raid_struct *)player->scene;
	if (!raid->data->ai_data.guoyu_data.note_boss)
	{
		BossId send;
		boss_id__init(&send);
		send.id = bossId;
		send.x = x;
		send.z = z;
		send.y = y;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUOYU_BOSS_APPEAR_NOTIFY, boss_id__pack, send);
		raid->data->ai_data.guoyu_data.note_boss = true;
	}

	return (0);
}
static int handle_set_special_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	ReqSetYaoshiSpecial *req = req_set_yaoshi_special__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int type = req->type;
	req_set_yaoshi_special__free_unpacked(req, NULL);

	uint32_t lv = player->data->change_special_num;
	if (lv > change_special_config.size())
	{
		lv = change_special_config.size();
	}
	AnsYaoshiSpecial send;
	ans_yaoshi_special__init(&send);
	send.ret = 0;
	ChangeSpecialty *table = get_change_special_table(lv);
	if (table == NULL)
	{
		send.ret = 1;
		goto done;
	}
	if (player->sub_comm_gold(table->CostValue, MAGIC_TYPE_YAOSHI) < 0)
	{
		send.ret = 190400005;
		goto done;
	}
	player->data->cur_yaoshi = type;
	++player->data->change_special_num;

done:
	send.type = player->data->cur_yaoshi;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SET_SPECIAL_ANSWER, ans_yaoshi_special__pack, send);

	return (0);
}
/*
static int handle_enter_guoyu_fb_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	uint32_t raid_id = 20023;
	raid_struct *raid = raid_manager::create_raid(raid_id, player);
	if (!raid)
	{
		LOG_ERR("%s: player[%lu] create raid[%u] failed", __FUNCTION__, extern_data->player_id, raid_id);
		return (-20);
	}

	if (player->m_team)
	{
		//		player_team_enter_raid(player, raid);
		raid->team_enter_raid(player->m_team);
	}
	else
	{
		if (raid->player_enter_raid(player) != 0)
		{
			LOG_ERR("%s: player[%lu] enter raid failed", __FUNCTION__, player->get_uuid());
			return (-30);
		}
	}

	return (0);
}
*/
static int handle_chengjie_find_target_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	ReqFindTarget *req = req_find_target__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	player_struct * target = NULL;
	if (req->name != NULL)
	{
		AnsFindTarget send;
		ans_find_target__init(&send);
		send.ret = 0;
		if (strcmp(req->name, player->get_name()) == 0)
		{
			send.ret = 190500128;
		}
		uint64_t pid = strtoull(req->name, NULL, 10);
		if (pid == player->get_uuid())
		{
			send.ret = 190500128;
		}
		if (send.ret != 0)
		{
			fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHENGJIE_FIND_TARGET_ANSWER, ans_find_target__pack, send);
		}
		else
		{
			target = player_manager::get_player_by_id(pid);
			if (target == NULL)
			{
				PROTO_FIND_PLAYER_REQ *todb = (PROTO_FIND_PLAYER_REQ *)&conn_node_base::global_send_buf[0];
				strncpy(todb->name, req->name, 33);
				todb->head.msg_id = ENDION_FUNC_2(SERVER_PROTO_FIND_PLAYER_REQUEST);
				todb->head.len = ENDION_FUNC_4(sizeof(PROTO_FIND_PLAYER_REQ));
				todb->org_msg = MSG_ID_CHENGJIE_FIND_TARGET_REQUEST;

				conn_node_base::add_extern_data(&todb->head, extern_data);
				if (conn_node_dbsrv::connecter->send_one_msg(&todb->head, 1) != (int)ENDION_FUNC_4(todb->head.len)) {
					LOG_ERR("%s %d: send to dbsrv err[%d]", __FUNCTION__, __LINE__, errno);
				}
			}
			else
			{
				send.pid = target->get_uuid();
				send.name = target->get_name();
				send.lv = target->get_attr(PLAYER_ATTR_LEVEL);
				fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHENGJIE_FIND_TARGET_ANSWER, ans_find_target__pack, send);
				ChengJieTaskManage::AddRoleLevel(send.pid, send.lv, target->data->chengjie.rest);
			}

		}

	}
	req_find_target__free_unpacked(req, NULL);

	return (0);
}
static int handle_refresh_chengjie_list_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	ChengjieRefreshType *req = chengjie_refresh_type__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	ChengJieTaskManage::ClientGetTaskList(player, req->type);
	chengjie_refresh_type__free_unpacked(req, NULL);

	return (0);
}
int check_can_add_chengjie_task(player_struct *player, uint32_t shangjin, uint64_t target, uint32_t lv)
{
	if (ChengJieTaskManage::IsTarget(target))
	{
		return 190500127;
	}
	if (ChengJieTaskManage::GetRoleCd(target) > time_helper::get_cached_time() / 1000)
	{
		return 190500130;
	}
	if (player->get_uuid() == target)
	{
		return 190500128;
	}
	ParameterTable *param_config = get_config_by_id(161000088, &parameter_config);
	if (param_config != NULL && param_config->parameter1[0] > lv)
	{
		//ChengJieTaskManage::DelRoleLevel(target);
		return 190500129;
	}
	param_config = get_config_by_id(161000090, &parameter_config);
	if (param_config != NULL)
	{
		uint64_t cost = (param_config->parameter1[0] + 10000) * shangjin / 10000;
		if (player->sub_coin(cost, MAGIC_TYPE_YAOSHI) < 0)
		{
			return 190500133;
		}
	}
	return 0;
}

void send_system_msg(char * str, player_struct *player);

static int handle_add_chengjie_task_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	ReqAddChengjieTask *req = req_add_chengjie_task__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	static uint32_t tid = 0;

	uint32_t lv = ChengJieTaskManage::GetRoleLevel(req->playerid);


	STChengJie task;
	ParameterTable *param_config = get_config_by_id(161000096, &parameter_config);
	//RewardTable * table = get_chengjie_reward_table(lv);
	AnsAddChengjieTask send;
	ans_add_chengjie_task__init(&send);

	if (lv == 0)
	{
		send.ret = 1;
		goto done;
	}
	if (req->step < 1 ||req->step > 3)
	{
		goto done;
	}
	send.ret = check_can_add_chengjie_task(player, req->shangjin, req->playerid, lv);
	if (send.ret != 0)
	{
		goto done;
	}
	task.id = ++tid;;
	task.pid = req->playerid;
	task.step = req->step;
	task.shuangjin = req->shangjin;
	task.timeOut = time_helper::get_cached_time() / 1000 + 3600 * 24 * 7;
	task.investor = player->get_uuid();
	task.anonymous = req->anonymous; 
	if (req->declaration != NULL && strnlen(req->declaration, 256) > 0)
	{
		strncpy(task.declaration, req->declaration, 256);
	}
	

	if (param_config != NULL)
	{
		task.timeOut = time_helper::get_cached_time() / 1000 + param_config->parameter1[0];
	}

	//if (table != NULL)
	//{
	//	task.exp = table->ExpCoefficient;
	//	task.courage = table->CoinReward;
	//}
	//ChengJieTaskManage::AddTask(task);
	ChengJieTaskManage::AddTaskDb(task, extern_data);

	if (player->data->guild_id != 0)
	{
		char str[1024];
		ParameterTable * configSpeek = get_config_by_id(161000271, &parameter_config);
		sprintf(str, configSpeek->parameter2, player->get_name(), req->name); 
		player->send_chat(CHANNEL__family, str);
	}

	player->add_achievement_progress(ACType_XUANSHANG, 0, 0, 0, 1);
done:
	req_add_chengjie_task__free_unpacked(req, NULL);
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ADD_CHENGJIE_TASK_ANSWER, ans_add_chengjie_task__pack, send);

	return (0);
}
int CheckCanAcceptChengjieTask(player_struct *player, STChengJie * task)
{
	if (player->data->chengjie.cur_task != 0)
	{
		return 190500136;
	}
	if (player->get_uuid() == task->investor)
	{
		return 190500135;
	}
	if (player->get_uuid() == task->pid)
	{
		return 190500340;
	}
	if (task->complete)
	{
		return 190500138;
	}
	if (player->data->chengjie.chengjie_num < 1)
	{
		return 190500140;
	}
	if (task->timeOut < time_helper::get_cached_time() / 1000)
	{
		return 190500138;
	}
	if (task->taskTime > time_helper::get_cached_time() / 1000)
	{
		return 190500137;
	}
	if (task->acceptCd > time_helper::get_cached_time() / 1000)
	{
		return 190500137;
	}
	int rate = task->fail;
	ParameterTable * config = get_config_by_id(161000087, &parameter_config);
	if (config != NULL)
	{
		rate = rate / config->parameter1[1];
		if (rate > config->parameter1[0])
		{
			rate = config->parameter1[0];
		}
	}
	ParameterTable * param_config = get_config_by_id(161000091, &parameter_config);
	if (param_config != NULL)
	{
		uint64_t cost = param_config->parameter1[0] *(rate / 10000 + 1) * task->shuangjin / 10000;
		if (player->sub_coin(cost, MAGIC_TYPE_YAOSHI) < 0)
		{
			return 190500139;
		}
	}
	return 0;
}
static int handle_accept_chengjie_task_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	ReqAcceptChengjieTask *req = req_accept_chengjie_task__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	AnsAcceptChengjieTask send;
	ans_accept_chengjie_task__init(&send);

	ChengjieKiller notify;
	EXTERN_DATA ext_data;
	player_struct *target = NULL;
	ParameterTable * param_config = get_config_by_id(161000097, &parameter_config);
	STChengJie * pTask = ChengJieTaskManage::FindTask(req->taskid);
	if (pTask == NULL)
	{
		send.ret = 190500138;
		goto done;
	}
	send.ret = CheckCanAcceptChengjieTask(player, pTask);
	if (send.ret != 0)
	{
		goto done;
	}

	player->data->chengjie.cur_task = req->taskid;
	player->data->chengjie.target = pTask->pid;
	if (param_config != NULL)
	{
		pTask->taskTime = time_helper::get_cached_time() / 1000 + param_config->parameter1[0];
		if (pTask->taskTime > pTask->timeOut)
		{
			pTask->taskTime = pTask->timeOut;
		}
	}
ChengJieTaskManage::ClientGetTaskList(player, 0);
	--player->data->chengjie.chengjie_num;
	ChengJieTaskManage::SetRoleTarget(pTask->pid, player->get_uuid());
	ChengJieTaskManage::UpdateTaskDb(*pTask);
	player->data->chengjie.first_hit = false;
	target = player_manager::get_player_by_id(pTask->pid);
	if (target != NULL)
	{
		chengjie_killer__init(&notify);
		notify.playerid = player->get_uuid();
		ext_data.player_id = target->get_uuid();
		fast_send_msg(&conn_node_gamesrv::connecter, &ext_data, MSG_ID_CHENGJIE_KILLER_NOTIFY, chengjie_killer__pack, notify);
	}

	send_cur_chengjie_task(player, extern_data);
	player->check_activity_progress(AM_YAOSHI, 2);
	

done:
	send.taskid = req->taskid;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ACCEPT_CHENGJIE_TASK_ANSWER, ans_accept_chengjie_task__pack, send);

	req_accept_chengjie_task__free_unpacked(req, NULL);

	return (0);
}
static int handle_submit_chengjie_task_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	uint64_t cost = 0;
	ParameterTable * param_config = get_config_by_id(161000091, &parameter_config);
	ParameterTable * config = get_config_by_id(161000087, &parameter_config);
	STChengJie * pTask = ChengJieTaskManage::FindTask(player->data->chengjie.cur_task);
	SpecialtySkillTable *tableSkill = get_yaoshi_skill_config(CHENGJIE_EIGHT, player->data->chengjie.level);
	uint32_t expadd = 0;
	uint32_t lv = 0;
	RewardTable * table = NULL;
	int rate = 0;
	
	AnsAcceptChengjieTask send;
	ans_accept_chengjie_task__init(&send);
	send.ret = 0;
	if (pTask == NULL)
	{
		send.ret = 190500138;
		goto done;
	}
	if (!pTask->complete)
	{
		send.ret = 190500138;
		goto done;
	}

	//if (param_config != NULL)
	//{
	//	cost = param_config->parameter1[0]; //押金
	//}
	rate = pTask->fail / config->parameter1[1]; 
	if (player->data->cur_yaoshi == MAJOR__TYPE__CHENGJIE)
	{
		SpecialTitleTable *title = get_yaoshi_title_table(player->data->cur_yaoshi, player->data->chengjie.level);
		if (title != NULL)
		{
			expadd = title->TitleEffect2;
		}
	}
	if (config != NULL)
	{
		if (rate > config->parameter1[0])
		{
			rate = config->parameter1[0];
		}
		if (rate > 0)
		{
			cost += (config->parameter1[2] * rate); //失败次数加成
		}
	}

	if (tableSkill != NULL)
	{
		cost += tableSkill->EffectValue[0];
		expadd += tableSkill->EffectValue[0];
	}
	player->add_coin(pTask->shuangjin * (10000 + cost) / 10000 + param_config->parameter1[0] * (rate / 10000 + 1) * pTask->shuangjin / 10000, MAGIC_TYPE_YAOSHI);
	lv = ChengJieTaskManage::GetRoleLevel(pTask->pid);
	table = get_chengjie_reward_table(lv);
	if (table != NULL)
	{
		for (int i = 0; i < 2; ++i)
		{
			player->add_item(table->RewardType[i], table->RewardValue[i] * ((config->parameter1[4 + i] + expadd) * rate / 10000.0 + 1), MAGIC_TYPE_YAOSHI);
		}
	}
	
	//player->add_chengjie_exp(pTask->exp * (10000 + expadd)/10000);
	//player->add_chengjie_courage(pTask->courage * (10000 + expadd) / 10000);
	send.taskid = player->data->chengjie.cur_task;

	ChengJieTaskManage::DelTask(player->data->chengjie.cur_task, false);
	player->data->chengjie.cur_task = 0;
	player->data->chengjie.target = 0;

done:

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SUBMIT_CHENGJIE_TASK_ANSWER, ans_accept_chengjie_task__pack, send);

	return (0);
}
static int handle_accept_shangjin_task_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	if (player->data->shangjin.accept)
	{
		return (-2);
	}

	int ret = 0;
	if (player->data->shangjin.shangjin_num < 1)
	{
		ret = 190500233;
		goto done;
	}
	ret = player->accept_task(player->data->shangjin.task[0].id, false);
	if (ret != 0)
	{
		goto done;
	}

	--player->data->shangjin.shangjin_num;
	player->data->shangjin.accept = true;
	player->check_activity_progress(AM_YAOSHI, 1);
	player->add_task_progress(TCT_YAOSHI_SHANGJIN, 0, 1);
done:
	send_comm_answer(MSG_ID_ACCEPT_SHANGJIN_TASK_ANSWER, ret, extern_data);

	return (0);
}
static int handle_refresh_shangjin_task_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	if (player->data->shangjin.accept)
	{
		return (-2);
	}

	ShangjinList shangJin;
	shangjin_list__init(&shangJin);
	shangJin.ret = 190500162;
	if (player->data->shangjin.free > 0)
	{
		--player->data->shangjin.free;
		player->send_all_yaoshi_num();
	}
	else
	{
		ParameterTable * config = get_config_by_id(161000084, &parameter_config);
		if (config == NULL || player->del_item(config->parameter1[0], config->parameter1[1], MAGIC_TYPE_YAOSHI) < 0)
		{
			shangJin.ret = 190500123;
			fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_REFRESH_SHANGJIN_TASK_ANSWER, shangjin_list__pack, shangJin);
			return 1;
		}
	}

	//fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_REFRESH_SHANGJIN_TASK_ANSWER, shangjin_list__pack, shangJin);
	ShangjinManage::RefreshTaskAndSend(player, true);

	return 0;
}

static int handle_get_on_cash_truck_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	if (player->data->truck.truck_id == 0)
	{
		return -2;
	}
	if (player->data->truck.on_truck)
	{
		return -3;
	}
	if (player->sight_space != NULL)
	{
		send_comm_answer(MSG_ID_GET_ON_CASH_TRUCK_ANSWER, 190500302, extern_data);
		return -4;
	}
	player->down_horse();
	player->data->truck.on_truck = true;

	send_comm_answer(MSG_ID_GET_ON_CASH_TRUCK_ANSWER, 0, extern_data);

	//UpDownCashTruck send;
	//up_down_cash_truck__init(&send);
	//send.cash_truck = player->data->truck_id;
	//send.playerid = player->get_uuid();
	//conn_node_gamesrv::send_to_all_player(MSG_ID_GET_ON_CASH_TRUCK_NOTIFY, (void *)&send, (pack_func)up_down_cash_truck__pack);


	SightChangedNotify notify;
	sight_changed_notify__init(&notify);
	uint64_t del_player_id[1];
	del_player_id[0] = player->data->player_id;
	notify.delete_player = del_player_id;
	notify.n_delete_player = 1;
	uint64_t *ppp = conn_node_gamesrv::prepare_broadcast_msg_to_players(MSG_ID_SIGHT_CHANGED_NOTIFY, &notify, (pack_func)sight_changed_notify__pack);
	for (int i = 0; i < player->data->cur_sight_player; ++i)
		conn_node_gamesrv::broadcast_msg_add_players(player->data->sight_player[i], ppp);
	conn_node_gamesrv::broadcast_msg_send();

	cash_truck_struct *truck = cash_truck_manager::get_cash_truck_by_id(player->data->truck.truck_id);
	if (truck == NULL)
	{
		return -4;
	}
	TruckEndurance send;
	truck_endurance__init(&send);
	send.endurance = truck->data->endurance;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CASH_TRUCK_ENDURANCE_NOTIFY, truck_endurance__pack, send);
	player->adjust_battle_partner();

	return 0;
}
static int handle_cash_truck_speed_up_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	if (player->data->truck.truck_id == 0)
	{
		return -2;
	}
	if (!player->data->truck.on_truck)
	{
		return -3;
	}

	ParameterTable *table = get_config_by_id(161000225, &parameter_config);
	if (table == NULL)
	{
		return -6;
	}

	cash_truck_struct *truck = cash_truck_manager::get_cash_truck_by_id(player->data->truck.truck_id);
	if (truck == NULL)
	{
		return -4;
	}
	if (truck->data->endurance < table->parameter1[2])
	{
		return -5;
	}
	//if (truck->data->n_speed_up == MAX_CASH_TRUCK_SPEED_UP)
	//{
	//	return -6;
	//}
	
	if (truck->data->speed_reduce / 1000 > time_helper::get_cached_time() / 1000)
	{
		AnsSpeedUp ans;
		ans_speed_up__init(&ans);
		ans.ret = 190500123;
		ans.cd = (truck->data->speed_reduce - time_helper::get_cached_time()) / 1000;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CASH_TRUCK_SPEED_UP_ANSWER, ans_speed_up__pack, ans);
		return -7;
	}

	truck->data->endurance -= table->parameter1[2];
	TruckEndurance send;
	truck_endurance__init(&send);
	send.endurance = truck->data->endurance;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CASH_TRUCK_ENDURANCE_NOTIFY, truck_endurance__pack, send);

	if (truck->data->endurance == 0)
	{
		truck->data->n_speed_up = 0;
		truck->data->speed_reduce = time_helper::get_cached_time() + table->parameter1[4] * 1000;
	}
	else
	{
		if (truck->data->n_speed_up < MAX_CASH_TRUCK_SPEED_UP)
		{
			truck->data->speed_up[truck->data->n_speed_up++] = time_helper::get_cached_time() + table->parameter1[4] * 1000;
		}
	}

	PlayerAttrNotify nty;
	player_attr_notify__init(&nty);
	AttrData attr_data[PLAYER_ATTR_MAX + 2];
	AttrData *attr_data_point[PLAYER_ATTR_MAX + 2];
	nty.player_id = truck->get_uuid();
	nty.n_attrs = 0;
	nty.attrs = attr_data_point;

	attr_data_point[nty.n_attrs] = &attr_data[nty.n_attrs];
	attr_data__init(&attr_data[nty.n_attrs]);
	attr_data[nty.n_attrs].id = PLAYER_ATTR_MOVE_SPEED;
	attr_data[nty.n_attrs].val = truck->get_speed();
	nty.n_attrs++;
	//conn_node_gamesrv::send_to_all_player(MSG_ID_PLAYER_ATTR_NOTIFY, (void *)&nty, (pack_func)player_attr_notify__pack);
	player->broadcast_to_sight(MSG_ID_PLAYER_ATTR_NOTIFY, (void *)&nty, (pack_func)player_attr_notify__pack, true);
	

	return 0;
}
/*
static int handle_submit_cash_truck_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	if (player->data->truck.truck_id == 0)
	{
		return -2;
	}


	BiaocheTable *table = get_config_by_id(player->data->truck.active_id, &cash_truck_config);
	if (table == NULL)
	{
		return -2;
	}
	if (table->Type == 1)  //粮草押镖
	{
		
	}
	else //财宝押镖
	{
		
	}

	player->add_task_progress(TCT_TRUCK, 0, 1);
	player->check_activity_progress(AM_TRUCK, table->ID);

	return 0;
}
*/
static int handle_go_downd_cash_truck_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	player->go_down_cash_truck();

	return 0;
}
int check_can_accept_cash_truck(player_struct *player, uint32_t type)
{
	BiaocheTable *table = get_config_by_id(type, &cash_truck_config);
	if (table == NULL)
	{
		return 2;
	}
	if (player->scene->m_id != table->Point[0])
	{
		return 8;
	}
	uint32_t cd = 0;
	if (!check_active_open(table->ActivityControl, cd))
	{
		return 190500104;
	}
	if (player->data->truck.truck_id != 0)
	{
		return 190500316;
	}
	if (table->Type == 1)
	{
		if (player->data->truck.num_coin < 1)
		{
			return 190500289;
		}
	}
	else
	{
		if (player->get_attr(PLAYER_ATTR_ZHENYING) + 1 != type % 10)
		{
			return 190500306;
		}
		if (player->data->truck.num_gold < 1)
		{
			return 190500291;
		}
	}
	TaskTable *config = get_config_by_id(table->TaskId, &task_config);
	if (!config)
	{
		return 5;
	}
	
	float dx = player->get_pos()->pos_x - table->Point[1];
	float dz = player->get_pos()->pos_z - table->Point[2];
	if (dx * dx + dz * dz > 18)
	{
		return 7;
	}

	if (config->Level > player->get_attr(PLAYER_ATTR_LEVEL))
	{
		return 190500107;
	}
	BiaocheRewardTable *reward_config = get_config_by_id(table->Reward, &cash_truck_reward_config);
	if (!reward_config)
	{
		return 6;
	}
	uint32_t subCoin = reward_config->RewardMoney1 * player->get_attr(PLAYER_ATTR_LEVEL) * reward_config->Deposit / 10000;
	if (subCoin > player->get_coin())
	{
		return 190500294;
	}
	int ret = player->accept_task(table->TaskId, false);
	if (ret != 0)
	{
		return 190500316;
	}
	if (table->Type == 1)
	{
		--player->data->truck.num_coin;
	}
	else
	{
		--player->data->truck.num_gold;
	}
	player->check_activity_progress(AM_TRUCK, table->ID);
	player->sub_coin(subCoin, MAGIC_TYPE_CASH_TRUCK);
	return 0;
}
static int handle_accept_cash_truck_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	AcceptCashTruck *req = accept_cash_truck__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t type = req->id;
	accept_cash_truck__free_unpacked(req, NULL);

	BiaocheTable *table = get_config_by_id(type, &cash_truck_config);
	if (table == NULL)
	{
		return -2;
	}
	
	ResAcceptCashTruck send;
	res_accept_cash_truck__init(&send);
	send.id = table->TaskId;
	
	player->data->truck.active_id = type;
	send.ret = check_can_accept_cash_truck(player, type);
	if (send.ret == 0)
	{
		cash_truck_struct *pTruck = cash_truck_manager::create_cash_truck_at_pos(player->scene, table->ID, *player);
		if (pTruck != NULL)
		{
			player->data->truck.truck_id = pTruck->get_uuid();
			
			player->data->truck.jiefei = 0;
			send.type = pTruck->get_truck_type();

			if (pTruck->get_truck_type() == 2 && player->get_attr(PLAYER_ATTR_PK_TYPE) != PK_TYPE_CAMP)
			{
				player->set_attr(PLAYER_ATTR_PK_TYPE, PK_TYPE_CAMP);
				player->broadcast_one_attr_changed(PLAYER_ATTR_PK_TYPE, PK_TYPE_CAMP, true, true);
			}
		}
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ACCEPT_CASH_TRUCK_ANSWER, res_accept_cash_truck__pack, send);
	
	return 0;
}

static int notify_setting_switch_info(player_struct *player, EXTERN_DATA *extern_data)
{
	SettingSwitchNotify resp;
	setting_switch_notify__init(&resp);

	SettingSwitchData switch_data[2];
	SettingSwitchData* switch_data_point[2];

	switch_data_point[0] = &switch_data[0];
	setting_switch_data__init(&switch_data[0]);
	switch_data[0].type = SETTING_SWITCH_TYPE__qiecuo;
	switch_data[0].state = player->data->qiecuo_invite_switch;

	switch_data_point[1] = &switch_data[1];
	setting_switch_data__init(&switch_data[1]);
	switch_data[1].type = SETTING_SWITCH_TYPE__team_invite;
	switch_data[1].state = player->data->team_invite_switch;

	resp.datas = switch_data_point;
	resp.n_datas = 2;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SETTING_SWITCH_NOTIFY, setting_switch_notify__pack, resp);

	return 0;
}

//设置开关信息请求
static int handle_setting_turn_switch_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	SettingTurnSwitchRequest *req = setting_turn_switch_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;

	setting_turn_switch_request__free_unpacked(req, NULL);

	int ret = 0;
	uint8_t *pSwitch = NULL;
	do
	{
		switch (type)
		{
			case SETTING_SWITCH_TYPE__qiecuo:
				pSwitch = &player->data->qiecuo_invite_switch;
				break;
			case SETTING_SWITCH_TYPE__team_invite:
				pSwitch = &player->data->team_invite_switch;
				break;
			case SETTING_SWITCH_TYPE__friend:
				{
					//换一个消息号，转发到好友服处理
					fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_FRIEND_TURN_SWITCH, 0, get_seq());
					return 0;
				}
		}

		if (!pSwitch)
		{
			ret = 1;
			LOG_ERR("[%s:%d] player[%lu] switch type error, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		if (*pSwitch == 0)
		{
			*pSwitch = 1;
		}
		else
		{
			*pSwitch = 0;
		}

		{
			SettingSwitchNotify resp;
			setting_switch_notify__init(&resp);

			SettingSwitchData switch_data[1];
			SettingSwitchData* switch_data_point[1];

			switch_data_point[0] = &switch_data[0];
			setting_switch_data__init(&switch_data[0]);
			switch_data[0].type = (SettingSwitchType)type;
			switch_data[0].state = *pSwitch;

			resp.datas = switch_data_point;
			resp.n_datas = 1;

			fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SETTING_SWITCH_NOTIFY, setting_switch_notify__pack, resp);
		}

	} while(0);

	SettingTurnSwitchAnswer resp;
	setting_turn_switch_answer__init(&resp);

	SettingSwitchData switch_data;
	setting_switch_data__init(&switch_data);
	switch_data.type = (SettingSwitchType)type;
	switch_data.state = (pSwitch ? *pSwitch : 0);

	resp.result = ret;
	resp.data = &switch_data;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SETTING_TURN_SWITCH_ANSWER, setting_turn_switch_answer__pack, resp);

	return 0;
}

static int notify_transfer_out_stuck_info(player_struct *player, EXTERN_DATA *extern_data)
{
	TransferOutStuckInfoNotify resp;
	transfer_out_stuck_info_notify__init(&resp);

	resp.cdtimestamp = player->data->out_stuck_time;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRANSFER_OUT_STUCK_INFO_NOTIFY, transfer_out_stuck_info_notify__pack, resp);

	return 0;
}

//脱离卡死请求
static int handle_transfer_out_stuck_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	int ret = 0;
	do
	{
		uint32_t now = time_helper::get_cached_time() / 1000;
		if (player->data->out_stuck_time > now)
		{
			ret = ERROR_ID_TRANSFER_OUT_STUCK_CDING;
			LOG_ERR("[%s:%d] player[%lu] out stuck cd, out_stuck_time:%u, now:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->data->out_stuck_time, now);
			break;
		}
		if (player->data->truck.on_truck)
		{
			player->go_down_cash_truck();
		}
		if (player->transfer_to_birth_position(extern_data) != 0)
		{
			ret = ERROR_ID_TRANSFER_OUT_STUCK_FAIL;
			LOG_ERR("[%s:%d] player[%lu] transfer fail", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}
		player->data->out_stuck_time = now + sg_transfer_out_stuck_cd_time;

	} while(0);

	TransferOutStuckAnswer resp;
	transfer_out_stuck_answer__init(&resp);

	resp.result = ret;
	resp.cdtimestamp = player->data->out_stuck_time;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRANSFER_OUT_STUCK_ANSWER, transfer_out_stuck_answer__pack, resp);

	return 0;
}

static int handle_srv_check_and_cost_request(player_struct *player, EXTERN_DATA *extern_data, uint32_t msg_id)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PROTO_SRV_CHECK_AND_COST_REQ *req = (PROTO_SRV_CHECK_AND_COST_REQ*)get_data();

	int ret = 0;
	SRV_COST_INFO real_cost;
	memset(&real_cost, 0, sizeof(SRV_COST_INFO));
	if (req->cost.statis_id == MAGIC_TYPE_GUILD_DONATE)
	{
		req->cost.coin *= player->get_coin_rate();
	}
	do
	{
		if (req->cost.gold > 0)
		{
			uint32_t has_gold = player->get_comm_gold();
			if (has_gold < req->cost.gold)
			{
				ret = ERROR_ID_GOLD_NOT_ENOUGH;
				break;
			}
		}
		if (req->cost.unbind_gold > 0)
		{
			uint32_t has_unbind_gold = player->get_attr(PLAYER_ATTR_GOLD);
			uint32_t has_bind_gold = player->get_attr(PLAYER_ATTR_BIND_GOLD);
			if (has_unbind_gold + has_bind_gold - req->cost.gold < req->cost.unbind_gold)
			{
				ret = ERROR_ID_GOLD_NOT_ENOUGH;
				break;
			}
		}
		if (req->cost.coin > 0)
		{
			uint32_t has_coin = player->get_coin();
			if (has_coin < req->cost.coin)
			{
				ret = ERROR_ID_COIN_NOT_ENOUGH;
				break;
			}
		}
		if (req->cost.silver > 0)
		{
			uint32_t has_silver = player->get_silver();
			if (has_silver < req->cost.silver)
			{
				ret = ERROR_ID_SILVER_NOT_ENOUGH;
				break;
			}
		}
		size_t item_len = (sizeof(req->cost.item_id) / sizeof(req->cost.item_id[0]));
		for (size_t i = 0; i < item_len; ++i)
		{
			uint32_t item_id = req->cost.item_id[i];
			uint32_t need_num = req->cost.item_num[i];
			if (item_id > 0 && need_num > 0)
			{
				uint32_t has_num = player->get_item_can_use_num(item_id);
				if (has_num < need_num)
				{
					ret = ERROR_ID_PROP_NOT_ENOUGH;
					break;
				}
			}
		}

		if (ret != 0)
		{
			break;
		}

		//扣除
		if (req->cost.gold > 0)
		{
			uint32_t has_bind_gold = player->get_attr(PLAYER_ATTR_BIND_GOLD);
			uint32_t has_unbind_gold = player->get_attr(PLAYER_ATTR_GOLD);
			player->sub_comm_gold(req->cost.gold, req->cost.statis_id);
			real_cost.gold = has_bind_gold - player->get_attr(PLAYER_ATTR_BIND_GOLD);
			real_cost.unbind_gold = has_unbind_gold - player->get_attr(PLAYER_ATTR_GOLD);
		}
		if (req->cost.unbind_gold > 0)
		{
			player->sub_unbind_gold(req->cost.unbind_gold, req->cost.statis_id);
			real_cost.unbind_gold += req->cost.unbind_gold;
		}
		if (req->cost.coin > 0)
		{
			player->sub_coin(req->cost.coin, req->cost.statis_id);
			real_cost.coin = req->cost.coin;
		}
		if (req->cost.silver > 0)
		{
			player->sub_silver(req->cost.silver, req->cost.statis_id);
			real_cost.silver = req->cost.silver;
		}
		uint32_t item_idx = 0;
		for (size_t i = 0; i < item_len; ++i)
		{
			uint32_t item_id = req->cost.item_id[i];
			uint32_t need_num = req->cost.item_num[i];
			if (item_id > 0 && need_num > 0)
			{
				uint32_t item_bind_id = 0, item_unbind_id = 0;
				get_item_bind_and_unbind_id(item_id, &item_bind_id, &item_unbind_id);
				uint32_t item_bind_num = player->get_item_num_by_id(item_bind_id);
				uint32_t item_unbind_num = player->get_item_num_by_id(item_unbind_id);
				player->del_item(item_id, need_num, req->cost.statis_id);
				uint32_t cost_bind_num = item_bind_num - player->get_item_num_by_id(item_bind_id);
				uint32_t cost_unbind_num = item_unbind_num - player->get_item_num_by_id(item_unbind_id);
				if (cost_bind_num > 0)
				{
					real_cost.item_id[item_idx] = item_bind_id;
					real_cost.item_num[item_idx] = cost_bind_num;
					item_idx++;
				}
				if (cost_unbind_num > 0)
				{
					real_cost.item_id[item_idx] = item_unbind_id;
					real_cost.item_num[item_idx] = cost_unbind_num;
					item_idx++;
				}
			}
		}

	} while(0);
	real_cost.statis_id = req->cost.statis_id;

	PROTO_SRV_CHECK_AND_COST_RES *res = (PROTO_SRV_CHECK_AND_COST_RES *)get_send_data();
	uint32_t data_len = sizeof(PROTO_SRV_CHECK_AND_COST_RES) + req->data_size;
	memset(res, 0, data_len);
	res->result = ret;
	memcpy(&res->cost, &real_cost, sizeof(SRV_COST_INFO));
	res->data_size = req->data_size;
	if (req->data_size > 0)
	{
		memcpy(res->data, req->data, req->data_size);
	}

	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, msg_id, data_len, 0);

	return 0;
}

//帮会服请求扣除消耗
static int handle_guildsrv_check_and_cost_request(player_struct *player, EXTERN_DATA *extern_data)
{
	return handle_srv_check_and_cost_request(player, extern_data, SERVER_PROTO_GUILDSRV_COST_ANSWER);
}

//帮会服请求发放奖励
static int handle_guildsrv_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	return handle_srv_reward_request(player, extern_data, SERVER_PROTO_GUILDSRV_REWARD_ANSWER);
}

//帮会服同步帮会技能
static int handle_sync_guild_skill_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	memset(player->data->guild_skills, 0, sizeof(player->data->guild_skills));
	PROTO_SYNC_GUILD_SKILL *req = (PROTO_SYNC_GUILD_SKILL*)buf_head();
	for (uint32_t i = 0; i < MAX_GUILD_SKILL_NUM; ++i)
	{
		player->data->guild_skills[i].skill_id = req->skills[i].skill_id;
		player->data->guild_skills[i].skill_lv = req->skills[i].skill_lv;
	}

	if (player->data->guild_id > 0)
	{
		player->calculate_attribute(true);
	}
	player->add_achievement_progress(ACType_GUILD_SKILL_LEVEL_NUM, 0, 0, 0, 0);

	return 0;
}

//帮会服同步帮会信息
static int handle_sync_guild_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_INFO("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	uint32_t old_guild_id = player->data->guild_id;
	uint32_t old_guild_office = player->data->guild_office;
	PROTO_SYNC_GUILD_INFO *req = (PROTO_SYNC_GUILD_INFO*)buf_head();
	player->data->guild_id = req->guild_id;
	player->data->guild_office = req->guild_office;

	//原来在帮会，现在退出了
	if (old_guild_id > 0 && player->data->guild_id == 0)
	{
		player->on_leave_guild();
		std::map<uint32_t, ProtoGuildInfo>::iterator iter = guild_summary_map.find(old_guild_id);
		if (iter != guild_summary_map.end())
		{
			ProtoGuildInfo &guild = iter->second;
			for (int i = 0; i < MAX_GUILD_MEMBER_NUM; ++i)
			{
				if (iter->second.player_data[i].player_id == extern_data->player_id)
				{
					if (i < MAX_GUILD_MEMBER_NUM - 1)
					{
						memmove(&guild.player_data[i], &guild.player_data[i + 1], sizeof(guild_player_data) * (MAX_GUILD_MEMBER_NUM - i - 1));
					}
					memset(&guild.player_data[MAX_GUILD_MEMBER_NUM - 1], 0, sizeof(guild_player_data));
					break;
				}
			}
		}
	}
	else if (old_guild_id == 0 && player->data->guild_id > 0)
	{
		player->add_task_progress(TCT_GUILD_JOIN, (player->data->guild_office == 1 ? 2 : 1), 1);
		player->add_achievement_progress(ACType_GUILD_JOIN, 0, 0, 0, 1);
		std::map<uint32_t, ProtoGuildInfo>::iterator iter = guild_summary_map.find(player->data->guild_id);
		if (iter != guild_summary_map.end())
		{
			ProtoGuildInfo &guild = iter->second;
			for (int i = 0; i < MAX_GUILD_MEMBER_NUM; ++i)
			{
				if(iter->second.player_data[i].player_id == player->get_uuid())
					break;

				if (iter->second.player_data[i].player_id == 0)
				{
					guild.player_data[i].player_id = player->get_uuid();
					break;
				}
			}
		}
	}

	bool isNty = false;
	if (old_guild_id != player->data->guild_id)
	{
		isNty = true;
		player->calculate_attribute(true);
	}
	if (old_guild_office != player->data->guild_office)
	{
		isNty = true;
	}

	if (isNty)
	{
		player->broadcast_sight_player_info_changed_notify();
		// SightPlayerInfoChangeNotify nty;
		// sight_player_info_change_notify__init(&nty);
		// nty.player_id = player->get_uuid();
		// nty.guild_id = player->data->guild_id;
		// ProtoGuildInfo *info = get_guild_summary(player->data->guild_id);
		// if (info)
		// 	nty.guild_name = info->name;
		// nty.guild_office = player->data->guild_office;		
		// player->broadcast_to_sight(MSG_ID_SIGHT_PLAYER_CHANGE_NOTIFY, &nty, (pack_func)sight_player_info_change_notify__pack, true);
	}

	return 0;
}

extern int send_mail(conn_node_base *connecter, uint64_t player_id, uint32_t type,
	char *title, char *sender_name, char *content, std::vector<char *> *args,
	std::map<uint32_t, uint32_t> *attachs, uint32_t statis_id);
static int handle_guild_answer_award(player_struct *player, EXTERN_DATA *extern_data)
{
	//if (player == NULL)
	//{
	//	return -1;
	//}
	ParameterTable *table = get_config_by_id(161000414, &parameter_config);
	if (table == NULL)
	{
		return -3;
	}
	ParameterTable *table1 = get_config_by_id(161000415, &parameter_config);
	if (table1 == NULL)
	{
		return -3;
	}
	ParameterTable *table2 = get_config_by_id(161000416, &parameter_config);
	if (table2 == NULL)
	{
		return -3;
	}
	ParameterTable *table3 = get_config_by_id(161000412, &parameter_config);
	if (table3 == NULL)
	{
		return -3;
	}
	PROTO_GUILD_DISBAND *req = (PROTO_GUILD_DISBAND*)buf_head();
	uint64_t *pData = (uint64_t *)req->data;
	std::map<uint32_t, uint32_t> item_list;
	uint64_t last = table3->parameter1[0] * req->player_num / 100;
	for (uint64_t i = 0; i < req->player_num; ++i)
	{
		item_list.clear();
		if (i >= table->parameter1[0] && i <= table->parameter1[1])
		{
			item_list.insert(std::make_pair(table->parameter1[2], 1));
		}
		else if (i >= table1->parameter1[0] && i <= table1->parameter1[1])
		{
			item_list.insert(std::make_pair(table1->parameter1[2], 1));
		}
		else if (i <= last)
		{
			item_list.insert(std::make_pair(table2->parameter1[2], 1));
		}
		else
		{
			item_list.insert(std::make_pair(table3->parameter1[2], table3->parameter1[3]));
		}
		player = player_manager::get_player_by_id(pData[i]);
		if (player == NULL)
		{
			::send_mail(&conn_node_gamesrv::connecter, pData[i], 270300044, NULL, NULL, NULL, NULL, &item_list, MAGIC_TYPE_QUESTION);
		}
		else
		{
			player->add_item_list_as_much_as_possible(item_list, MAGIC_TYPE_QUESTION);
			if (i <= last)
			{
				player->send_system_notice(190500492, NULL);
			}
			else
			{
				player->send_system_notice(190500381, NULL);
			}
		}
	}
	return 0;
}

//同步所有帮会信息
static int handle_guild_sync_all(player_struct * /*player*/, EXTERN_DATA * /*extern_data*/)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);

	if (guild_summary_map.size() > 0)
	{
		return 0;
	}
	
	PROTO_SYNC_ALL_GUILD *req = (PROTO_SYNC_ALL_GUILD*)buf_head();
	for (uint32_t i = 0; i < req->guild_num; ++i)
	{
		ProtoGuildInfo &info = req->guilds[i];
		guild_summary_map.insert(std::make_pair(info.guild_id, info));
		guild_land_raid_manager::create_guild_land_raid(info.guild_id);
		//LOG_INFO("[%s:%d] dddddddddddddddddddd %s", __FUNCTION__, __LINE__, info.name);
	}

	return 0;
}

//同步帮会创建
static int handle_sync_guild_create(player_struct * /*player*/, EXTERN_DATA * /*extern_data*/)
{
	ProtoGuildInfo *pData = (ProtoGuildInfo*)get_data();
	
	LOG_INFO("[%s:%d] guild_id:%u, guild_name:%s", __FUNCTION__, __LINE__, pData->guild_id, pData->name);

	//帮会创建的时候帮会成员仅帮主一人
	if(MAX_GUILD_MEMBER_NUM >= 1)
	{
		pData->player_data[0].player_id = pData->master_id;
	}
	guild_summary_map.insert(std::make_pair(pData->guild_id, *pData));
	guild_land_raid_manager::create_guild_land_raid(pData->guild_id);

	return 0;
}

//同步帮会解散
static int handle_sync_guild_disband(player_struct * /*player*/, EXTERN_DATA * /*extern_data*/)
{
	PROTO_GUILD_DISBAND *req = (PROTO_GUILD_DISBAND*)buf_head();
	uint64_t *pData = (uint64_t*)req->data;

	LOG_INFO("[%s:%d] guild_id:%u", __FUNCTION__, __LINE__, req->guild_id);

	//将所有玩家拉出帮会地图
	for (uint32_t i = 0; i < req->player_num; ++i)
	{
		uint64_t member_id = *pData++;
		player_struct *member = player_manager::get_player_by_id(member_id);
		if (member)
		{
			member->on_leave_guild();
		}
	}
	guild_summary_map.erase(req->guild_id);

	//销毁帮会场景
	guild_land_raid_manager::delete_guild_land_raid_by_guild_id(req->guild_id);

	return 0;
}

//同步帮会改名
static int handle_sync_guild_rename(player_struct * /*player*/, EXTERN_DATA * /*extern_data*/)
{
	PROTO_SYNC_GUILD_RENAME *pData = (PROTO_SYNC_GUILD_RENAME*)get_data();

	LOG_INFO("[%s:%d] guild_id:%u, new_name:%s", __FUNCTION__, __LINE__, pData->guild_id, pData->name);

	ProtoGuildInfo *info = get_guild_summary(pData->guild_id);
	if (info)
	{
		memcpy(info->name, pData->name, MAX_GUILD_NAME_LEN + 1);
	}

	for (uint32_t i = 0; i < pData->member_num; ++i)
	{
		uint64_t player_id = pData->member_ids[i];
		player_struct *player = player_manager::get_player_by_id(player_id);
		if (!player)
		{
			continue;
		}

		// SightPlayerInfoChangeNotify nty;
		// sight_player_info_change_notify__init(&nty);

		// nty.player_id = player->get_uuid();
		// nty.guild_name = pData->name;

		// player->broadcast_to_sight(MSG_ID_SIGHT_PLAYER_CHANGE_NOTIFY, &nty, (pack_func)sight_player_info_change_notify__pack, true);
		player->broadcast_sight_player_info_changed_notify();
	}

	return 0;
}

//帮会战征召请求
static int handle_guild_battle_call_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PROTO_HEAD *req = (PROTO_HEAD*)buf_head();
	uint32_t player_num = *((uint32_t*)(req->data));
	uint64_t *pData = (uint64_t*)(req->data + sizeof(uint32_t));

	std::vector<uint64_t> playerIds;
	for (uint32_t i = 0; i < player_num; ++i)
	{
		uint64_t member_id = *pData++;
		if (member_id == extern_data->player_id)
		{
			continue;
		}

		player_struct *member = player_manager::get_player_by_id(member_id);
		if (member && member->is_online() && !member->is_in_raid() && member->is_alive())
		{
			playerIds.push_back(member_id);
		}
	}

	int ret = 0;
	do
	{
		if (!is_guild_battle_opening())
		{
			ret = 190411002;
			LOG_ERR("[%s:%d] player[%lu] guild battle not open", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		uint32_t now = time_helper::get_cached_time() / 1000;
		uint32_t call_cd = get_guild_call_cd(player->data->guild_id);
		if (call_cd > now)
		{
			ret = ERROR_ID_GUILD_CALL_CD;
			LOG_ERR("[%s:%d] player[%lu] call cd, guild_id:%u, now:%u, call_cd:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->data->guild_id, now, call_cd);
			break;
		}

		set_guild_call_cd(player->data->guild_id);
		broadcast_guild_battle_call_notify(playerIds, player->get_uuid(), player->data->name);
	} while(0);
	
	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUILD_BATTLE_CALL_ANSWER, comm_answer__pack, resp);

	return 0;
}

//帮会战信息请求
static int handle_guild_battle_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	int ret = 0;
	do
	{
		if (!is_guild_battle_opening())
		{
			ret = 190411002;
			LOG_ERR("[%s:%d] player[%lu] guild battle not open", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (player->data->guild_id <= 0)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		conn_node_gamesrv::connecter.transfer_to_connsrv();
	} while(0);
	
	if (ret != 0)
	{
		GuildBattleInfoAnswer resp;
		guild_battle_info_answer__init(&resp);

		resp.result = ret;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUILD_BATTLE_INFO_ANSWER, guild_battle_info_answer__pack, resp);
	}

	return 0;
}

//帮会战同步排名
static int handle_guild_battle_final_list_answer(player_struct * /*player*/, EXTERN_DATA * /*extern_data*/)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);

	PROTO_GUILD_BATTLE_RANK *req = (PROTO_GUILD_BATTLE_RANK*)buf_head();
	set_final_guild_id(req->guild_id, sizeof(req->guild_id) / sizeof(req->guild_id[0]));

	guild_battle_manager_final_list_state = GBFLS_GOTTEN;
	guild_battle_manager_final_list_tick = time_helper::get_cached_time() / 1000;

	return 0;
}

//同步帮贡
static int handle_guild_sync_donation(player_struct * player, EXTERN_DATA * extern_data)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PROTO_SYNC_GUILD_DONATION *pData = (PROTO_SYNC_GUILD_DONATION*)get_data();

	player->data->guild_donation = pData->cur_donation;
	player->add_task_progress(TCT_GUILD_DONATION, 0, player->data->guild_donation);
	player->add_task_progress(TCT_GUILD_DONATION1, 0, 1);
	if (pData->is_change == 1)
	{
		player->add_achievement_progress(ACType_ADD_CURRENCY, ACurrency_GUILD_DONATION, 0, 0, pData->change_val);
	}

	return 0;
}

static int handle_guild_skill_level_up(player_struct * player, EXTERN_DATA * extern_data)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	player->add_achievement_progress(ACType_GUILD_SKILL_LEVEL_UP, 0, 0, 0, 1);

	return 0;
}

static int handle_guild_accept_task_request(player_struct * player, EXTERN_DATA * extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	uint32_t *req = (uint32_t *)get_data();
	uint32_t task_id = *req;
//	LOG_INFO("[%s:%d] player[%lu], task_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, task_id);

	int ret = 0;
	do
	{
		for (int i = 0; i < MAX_TASK_ACCEPTED_NUM; ++i)
		{
			TaskInfo *task = &player->data->task_list[i];
			if (task->id == 0)
			{
				continue;
			}

			if (get_task_type(task->id) == TT_GUILD_BUILD)
			{
				ret = ERROR_ID_GUILD_TASK_ON_GOING;
				break;
			}
		}

		if (ret != 0)
		{
			break;
		}

		ret = player->accept_task(task_id, false);
	} while(0);

	LOG_INFO("%s: player[%lu] task_id[%u] ret[%u]", __FUNCTION__, player->get_uuid(), task_id, ret);

	GUILD_ACCEPT_TASK_ANSWER *resp = (GUILD_ACCEPT_TASK_ANSWER*)get_send_data();
	uint32_t data_len = sizeof(GUILD_ACCEPT_TASK_ANSWER);
	memset(resp, 0, data_len);
	resp->result = ret;
	resp->task_id = task_id;
	resp->is_next = false;
	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_GUILD_ACCEPT_TASK_ANSWER, data_len, 0);

	return 0;
}

static int handle_guild_sync_task(player_struct * player, EXTERN_DATA * extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, player->get_uuid());

	GUILD_SYNC_TASK *req = (GUILD_SYNC_TASK *)get_data();
	player->data->guild_task_count = req->task_count;
	player->data->guild_task_config_id = req->config_id;

	for (int i = 0; i < MAX_TASK_ACCEPTED_NUM; ++i)
	{
		TaskInfo *task = &player->data->task_list[i];
		if (task->id == 0)
		{
			continue;
		}

		if (get_task_type(task->id) == TT_GUILD_BUILD)
		{
			player->task_update_notify(task);
			break;
		}
	}

	return 0;
}

static int handle_guild_ruqin_add_count(player_struct * player, EXTERN_DATA * extern_data)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);
	if (!player || !player->is_online())
	{
//		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	player->check_activity_progress(AM_GUILD_INTRUSION, 0);

	return 0;
}

static int handle_guild_ruqin_sync_count(player_struct * player, EXTERN_DATA * extern_data)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	uint32_t *req = (uint32_t *)get_data();
	uint32_t count = *req;
	if (count > 0)
	{
		for (std::map<uint64_t, EventCalendarTable*>::iterator iter = activity_config.begin(); iter != activity_config.end(); ++iter)
		{
			EventCalendarTable *config = iter->second;
			if (!activity_is_unlock_by_config(player, config))
			{
				continue;
			}

			if (config->ActivityType != AM_GUILD_INTRUSION)
			{
				continue;
			}

			if (config->Active > 0)
			{
				if (config->Sum != 0)
				{
					DailyActivityInfo *pDaily = NULL;
					for (int i = 0; i < MAX_DAILY_ACTIVITY_NUM; ++i)
					{
						if (player->data->daily_activity[i].act_id == (uint32_t)config->ID || player->data->daily_activity[i].act_id == 0)
						{
							pDaily = &player->data->daily_activity[i];
							break;
						}
					}

					if (pDaily && pDaily->count < count)
					{
						uint32_t prev_count = pDaily->count;
						for (uint32_t i = prev_count; i <= count; ++i)
						{
							player->check_activity_progress(AM_GUILD_INTRUSION, 0);
						}
					}
				}
			}
		}

		return 0;
	}

	return 0;
}

static int handle_guild_sync_donate(player_struct * player, EXTERN_DATA * extern_data)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	player->check_activity_progress(AM_GUILD_DONATE, 0);

	return 0;
}

static int handle_guild_sync_participate_answer_activity(player_struct * player, EXTERN_DATA * extern_data)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	player->add_task_progress(TCT_QUESTION_JOIN, 0, 1);
	player->add_achievement_progress(ACType_ANSWER, QUESTION_GUILD, 0, 0, 1);
	player->check_activity_progress(AM_ANSWER, QUESTION_GUILD);

	return 0;
}

static int handle_guild_bonfire_open_request(player_struct * player, EXTERN_DATA * extern_data)
{
	LOG_INFO("[%s:%d]", __FUNCTION__, __LINE__);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	GUILD_BONFIRE_OPEN_REQUEST *req = (GUILD_BONFIRE_OPEN_REQUEST*)get_data();

	int ret = guild_land_active_manager::guild_bonfire_open(player->data->guild_id, false);

	if (ret == 0)
	{
		player->check_activity_progress(AM_GUILD_BBQ, 0);		
	}

	GUILD_BONFIRE_OPEN_ANSWER *resp = (GUILD_BONFIRE_OPEN_ANSWER*)get_send_data();
	uint32_t data_len = sizeof(GUILD_BONFIRE_OPEN_ANSWER);
	memset(resp, 0, data_len);
	resp->result = ret;
	resp->last_begin_time = req->last_begin_time;
	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_GUILD_BONFIRE_OPEN_ANSWER, data_len, 0);

	return 0;
}

static int on_login_send_zhenying(player_struct *player, EXTERN_DATA *extern_data)
{
	if (player->data->zhenying.level == 0)
	{
		player->data->zhenying.level = 1;
		player->data->zhenying.step = 1;
		player->data->zhenying.step_lv = 1;
		std::map<uint64_t, struct CampTable*>::iterator it = zhenying_base_config.begin();
		player->data->zhenying.free = it->second->FreeNum;
		player->data->zhenying.daily_award = true;
		player->data->zhenying.daily_step = true;
		player->data->zhenying.task_award_state = ZHENYING__TASK__AWARD__STATE__UNACHIVE;
		WeekTable *table = get_rand_week_config();
		if (table != NULL)
		{
			player->data->zhenying.task = table->ID;
			player->data->zhenying.task_type = table->Type;
		}
		//FactionBattleTable *tableFa = get_zhenying_battle_table(player->get_attr(PLAYER_ATTR_LEVEL)); //todo delete
		//if (tableFa != NULL)
		//{
		//	player->data->zhenying.mine = tableFa->BoxOpenNum;
		//}
		player->data->zhenying.last_week = time_helper::nextWeek(5 * 3600);
		ParameterTable *tablePa = get_config_by_id(161000355, &parameter_config);
		if (tablePa != NULL)
		{
			player->data->zhenying.award_num = tablePa->parameter1[3];
		}
	}

	player->send_zhenying_info();

	return 0;
}
static int handle_into_zhenying_battle_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	FactionBattleTable *table = get_zhenying_battle_table(player->get_attr(PLAYER_ATTR_LEVEL));
	if (table == NULL)
	{
		return -2;
	}
	if (player->get_attr(PLAYER_ATTR_ZHENYING) == 0)
	{
		return -3;
	}
	if (player->scene->is_in_zhenying_raid())
	{
		send_comm_answer(MSG_ID_INTO_ZHENYING_BATTLE_ANSWER, 190500563, extern_data);
		return -4;
	}
	//int ret = raid_manager::check_player_enter_raid(player, table->Map);
	//if (ret != 0)
	//{
	//	send_comm_answer(MSG_ID_INTO_ZHENYING_BATTLE_ANSWER, ret, extern_data);
	//	return -3;
	//}
	zhenying_raid_struct *raid = zhenying_raid_manager::add_player_to_zhenying_raid(player);
	if (raid == NULL)
	{
		LOG_ERR("[%s] :player[%lu] fail", __FUNCTION__, extern_data->player_id);
		return (-10);
	}

	send_comm_answer(MSG_ID_INTO_ZHENYING_BATTLE_ANSWER, 0, extern_data);
	player->add_achievement_progress(ACType_ZHENYING_BATTLE, 0, 0, 0, 1);

	return 0;
}
static int handle_join_zhenying_battle_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	int ret = ZhenyingBattle::GetInstance()->Join(*player);
	send_comm_answer(MSG_ID_JOIN_ZHENYING_FIGHT_ANSWER, ret, extern_data);

	return 0;
}
static int handle_cancel_join_zhenying_battle_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	int ret = ZhenyingBattle::GetInstance()->CancelJoin(*player);
	send_comm_answer(MSG_ID_CANCEL_JOIN_ZHENYING_FIGHT_ANSWER, ret, extern_data);

	return 0;
}
static int handle_into_zhenying_fight_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	
	int ret = ZhenyingBattle::GetInstance()->IntoBattle(*player);
	if (ret != 0)
	{
		LOG_ERR("%s: %lu into fb zhengyinzhan failed ret = %d", __FUNCTION__, extern_data->player_id, ret);
	}
	send_comm_answer(MSG_ID_INTO_ZHENYING_FIGHT_ANSWER, ret, extern_data);

	return 0;
}
static int handle_zhenying_fight_set_ready_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	ZhenyingSetReady *req = zhenying_set_ready__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int ret = ZhenyingBattle::GetInstance()->SetReady(*player, req->ready);
	if (ret != 0)
	{
		LOG_ERR("[%s:%d] player[%lu] battle ready ret= %d", __FUNCTION__, __LINE__, extern_data->player_id, ret);
	}
	zhenying_set_ready__free_unpacked(req, NULL);
	return 0;
}
static int handle_zhenying_fight_ready_state_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	
	ZhenyingBattle::GetInstance()->GetReadyState(*player);

	return 0;
}
static int handle_zhenying_fight_myside_score_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	raid_struct *raid = player->get_raid();
	if (raid == NULL)
	{
		return -2;
	}
	//if (raid->m_config->DengeonRank == DUNGEON_TYPE_BATTLE)
	{
		ZhenyingBattle::GetInstance()->GetMySideScore(*player);
		ZhenyingBattle::GetInstance()->SendMyScore(*player);
	}
	//else
	//{
	//	ZhenyingBattle *battel = ZhenyingBattle::GetPrivateBattle(raid->data->uuid);
	//	if (battel == NULL)
	//	{
	//		return -3;
	//	}
	//	battel->GetMySideScore(*player);
	//}

	return 0;
}
static int handle_get_zhenying_dayli_award_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->get_attr(PLAYER_ATTR_ZHENYING) == 0)
	{
		return -2;
	}

	ZhenyingDailyAward *req = zhenying_daily_award__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	std::map<uint32_t, uint32_t> item_list;
	GradeTable *table = NULL;
	if (req->step && player->data->zhenying.daily_step)
	{
		player->data->zhenying.daily_step = false;
		GradeTable *table = get_zhenying_grade_table(player->get_attr(PLAYER_ATTR_ZHENYING), player->data->zhenying.level + 1);
		if (table == NULL)
		{
			ParameterTable *tableAward = get_config_by_id(161001014, &parameter_config);
			if (tableAward != NULL)
			{
				for (uint32_t i = 0; i < tableAward->n_parameter1; i += 2)
				{
					item_list.insert(std::make_pair(tableAward->parameter1[i], tableAward->parameter1[i + 1]));
				}
			}
		}
	} 
	else
	{
		if (player->data->zhenying.daily_award)
		{
			player->data->zhenying.daily_award = false;
			table = get_zhenying_grade_table(player->get_attr(PLAYER_ATTR_ZHENYING), player->data->zhenying.level);
			if (table != NULL)
			{
				for (uint32_t i = 0; i < table->n_DayReward; i += 2)
				{
					item_list.insert(std::make_pair(table->DayReward[i], table->DayRewardNum[i + 1]));
				}
			}
		}
	}
	if (item_list.size() > 0)
	{
		player->add_item_list_as_much_as_possible(item_list, MAGIC_TYPE_ZHENYING);
	}
	AnsZhenyingDailyAward send;
	ans_zhenying_daily_award__init(&send);
	send.step = req->step;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GET_ZHENYING_DAYLY_AWARD_ANSWER, ans_zhenying_daily_award__pack, send);
	zhenying_daily_award__free_unpacked(req, NULL);

	
	return 0;
}
static int handle_zhenying_change_line_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->scene == NULL)
	{
		return -2;
	}
	if (!player->scene->is_in_zhenying_raid())
	{
		return -3;
	}

	ZhenyingLineInfo *req = zhenying_line_info__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int line = req->id;
	zhenying_line_info__free_unpacked(req, NULL);

	zhenying_raid_struct *raid = (zhenying_raid_struct *)(player->scene);
	if (raid->get_line_num() == line)
	{
		return -4;
	}

	zhenying_raid_struct *newRaid = zhenying_raid_manager::get_zhenying_raid_by_line(player->data->scene_id, line);
	int ret = 0;
	if (newRaid == NULL)
	{
		ret = 1;
	}
	else
	{
		player->send_clear_sight();
		raid->on_player_leave_raid(player);
		//player->scene->delete_player_from_scene(player);
		newRaid->add_player_to_zhenying_raid(player);
		player_ready_enter_scene(player, extern_data, false);
	}
	send_comm_answer(MSG_ID_ZHENYING_CHANGE_LINE_ANSWER, ret, extern_data);

	return 0;
}
static int handle_zhenying_get_line_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->scene == NULL)
	{
		return -2;
	}
	if (!player->scene->is_in_zhenying_raid())
	{
		return -3;
	}

	ZhenyingLine send;
	zhenying_line__init(&send);
	int n_line = 0;

	ZhenyingLineInfo info[MAX_BATTLE_LINE_NUM];
	ZhenyingLineInfo *infoPoint[MAX_BATTLE_LINE_NUM];

	zhenying_raid_struct *raid = NULL;
	std::map<uint64_t, zhenying_raid_struct *>::iterator iter = zhenying_raid_manager_all_raid_id.begin();
	for (; iter != zhenying_raid_manager_all_raid_id.end(); ++iter)
	{
		raid = iter->second;
		if (raid->m_id == player->data->scene_id) //todo 对n_line上限做个保护
		{
			zhenying_line_info__init(&info[n_line]);
			info[n_line].id = raid->get_line_num();
			info[n_line].man = raid->get_cur_player_num();
			info[n_line].state = ZHENYING__LINE__STATE__NORMAL;
			infoPoint[n_line] = info + n_line;
			++n_line;
		}
	}

	raid = (zhenying_raid_struct *)(player->scene);
	send.my_line = raid->get_line_num();
	send.n_all_line = n_line;
	send.all_line = infoPoint;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ZHENYING_GET_LINE_INFO_ANSWER, zhenying_line__pack, send);

	return 0;
}
static int handle_get_zhenying_task_award_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	WeekTable *table = get_config_by_id(player->data->zhenying.task, &zhenying_week_config);
	if (table == NULL)
	{
		return -2;
	}

	if (player->data->zhenying.task_num == table->Num && player->data->zhenying.task_award_state != ZHENYING__TASK__AWARD__STATE__AWARD)
	{
		//任务完成 给奖励
		std::map<uint32_t, uint32_t> item_list;
		uint32_t reward = table->Reward;
		item_list.insert(std::make_pair(reward, 1));
		player->add_item_list_otherwise_send_mail(item_list, MAGIC_TYPE_ZHENYING, 0, NULL);
		//player->give_drop_item(table->Reward, MAGIC_TYPE_MONSTER_DEAD, ADD_ITEM_AS_MUCH_AS_POSSIBLE);
		player->add_achievement_progress(ACType_ZHENYING_TASK_AWARD, 0, 0, 0, 1);
		player->data->zhenying.task_award_state = ZHENYING__TASK__AWARD__STATE__AWARD;
	}

	//++player->data->zhenying.week;
	//if (player->data->zhenying.week > 6)
	//{
	//	player->data->zhenying.task = 0;
	//	player->data->zhenying.task_type = 0;
	//}
	//else
	//{
	//	table = get_rand_week_config();
	//	if (table != NULL)
	//	{
	//		player->data->zhenying.task = table->ID;
	//		player->data->zhenying.task_type = table->Type;
	//	}
	//}

	send_comm_answer(MSG_ID_GET_ZHENYING_TASK_AWARD_ANSWER, 0, extern_data);

	//player->data->zhenying.task_num = 0;
	//NewZhenyingTask send;
	//new_zhenying_task__init(&send);
	//send.task = player->data->zhenying.task;
	//send.task_type = player->data->zhenying.task_type;
	//send.task_num = player->data->zhenying.task_num;
	//fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_NEW_ZHENYING_TASK_NOTIFY, new_zhenying_task__pack, send);

	return 0;
}
// static int handle_exit_zhenying_battle_request(player_struct *player, EXTERN_DATA *extern_data)
// {
// 	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
// 	if (!player || !player->is_online())
// 	{
// 		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
// 		return (-1);
// 	}

// 	player->transfer_to_new_scene_by_config(155000004, extern_data);

// 	return 0;
// }
static int handle_zhenying_team_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	ZhenyingTeamInfo info[MAX_TEAM_MEM];
	ZhenyingTeamInfo *pInfo[MAX_TEAM_MEM] = {info, info + 1, info + 2, info + 3, info + 4};
	ZhenyingTeam send;
	zhenying_team__init(&send);
	int num = 0;
	std::vector<uint64_t> vt;
	if (player->m_team != NULL)
	{
		for (int i = 0; i < player->m_team->m_data->m_memSize; ++i)
		{
			vt.push_back(player->m_team->m_data->m_mem[i].id);
		}
	}
	else
	{
		vt.push_back(player->get_uuid());
	}
	for (std::vector<uint64_t>::iterator it = vt.begin(); it != vt.end(); ++it)
	{
		player_struct *mem = player_manager::get_player_by_id(*it);
		if (mem == NULL)
		{
			continue;
		}

		zhenying_team_info__init(info + num);
		info[num].playerid = mem->get_uuid();
		info[num].name = mem->get_name();
		info[num].job = mem->get_attr(PLAYER_ATTR_JOB);
		info[num].lv = mem->get_attr(PLAYER_ATTR_LEVEL);
		info[num].kill = mem->data->zhenying.kill;
		info[num].death = mem->data->zhenying.death;
		info[num].assist = mem->data->zhenying.help;
		info[num].score = mem->data->zhenying.score;
		++num;
	}
	send.n_mem = num;
	send.mem = pInfo;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ZHENYING_TEAM_INFO_ANSWER, zhenying_team__pack, send);

	return 0;
}
static int handle_zhenying_power_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	//ZhenyingPower send;
	//zhenying_power__init(&send);
	//send.power_fulongguo = 1;
	//send.power_wanyaogu = 2;
	//send.man_wanyaogu = 3;
	//send.man_fulongguo = 5;
	//send.power_man = 0;
	//fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ZHENYING_POWER_ANSWER, zhenying_power__pack, send);

	AddZhenyingPlayer tofr;
	add_zhenying_player__init(&tofr);
	tofr.name = player->get_name();
	tofr.zhenying = player->get_attr(PLAYER_ATTR_ZHENYING);
	tofr.zhenying_old = player->get_attr(PLAYER_ATTR_ZHENYING);
	tofr.fighting_capacity = player->get_attr(PLAYER_ATTR_FIGHTING_CAPACITY);
	tofr.kill = player->data->zhenying.kill;
	conn_node_gamesrv::connecter.send_to_friend(extern_data, MSG_ID_ZHENYING_POWER_REQUEST, &tofr, (pack_func)add_zhenying_player__pack);

	return 0;
}
static int check_change_zhenying(player_struct &player, int *cd)
{
	if (player.data->zhenying.change_cd > time_helper::get_cached_time() / 1000)
	{
		*cd = player.data->zhenying.change_cd - time_helper::get_cached_time() / 1000;
		return 190500226;
	}
	else if (player.data->truck.truck_id != 0)
	{
		return 190500310;
	}
	BATTLE_JOINER *join = ZhenyingBattle::GetInstance()->GetJoins(player.get_uuid());
	if (join != NULL)
	{
		return 190500424;
	}
	return 0;
}
static int handle_change_zhenying_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->scene->get_scene_type() == SCENE_TYPE_RAID)
	{
		raid_struct *raid = (raid_struct *)player->scene;
		if (raid->m_config->DengeonRank == DUNGEON_TYPE_BATTLE || raid->m_config->DengeonRank == DUNGEON_TYPE_BATTLE_NEW
			|| raid->m_config->DengeonRank == DUNGEON_TYPE_ZHENYING)
		{
			return -4;
		}
	}
	
	ChoseZhenying *req = chose_zhenying__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int zhenYing = req->zhenying;
	chose_zhenying__free_unpacked(req, NULL);

	if (zhenYing == player->get_attr(PLAYER_ATTR_ZHENYING) || player->get_attr(PLAYER_ATTR_ZHENYING) == 0)
	{
		return -2;
	}

	AnsChoseZhenying send;
	ans_chose_zhenying__init(&send);
	send.ret = check_change_zhenying(*player, &(send.zhenying));
	if (send.ret > 0)
	{
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHANGE_ZHENYING_ANSWER, ans_chose_zhenying__pack, send);
		return -3;
	}

	AddZhenyingPlayer tofr;
	add_zhenying_player__init(&tofr);
	tofr.name = player->get_name();
	tofr.zhenying = zhenYing;
	tofr.gold = player->get_comm_gold();
	tofr.zhenying_old = player->get_attr(PLAYER_ATTR_ZHENYING);
	tofr.fighting_capacity = player->get_attr(PLAYER_ATTR_FIGHTING_CAPACITY);
	tofr.kill = player->data->zhenying.kill;
	tofr.free = player->data->zhenying.free;
	conn_node_gamesrv::connecter.send_to_friend(extern_data, MSG_ID_CHANGE_ZHENYING_REQUEST, &tofr, (pack_func)add_zhenying_player__pack);

	return 0;
}
static int handle_server_chose_zhenying(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	AddZhenyingPlayer *req = add_zhenying_player__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	if (req->ret == 0)
	{
		player->set_attr(PLAYER_ATTR_ZHENYING, req->zhenying);
		AttrMap nty_list;
		nty_list[PLAYER_ATTR_ZHENYING] = player->get_attr(PLAYER_ATTR_ZHENYING);
		player->notify_attr(nty_list, true, false);
		player->add_task_progress(TCT_JOIN_CAMP, req->zhenying, 1);
		player->refresh_player_redis_info(false);
		player->add_achievement_progress(ACType_ZHENYING_CHOSE, 0, 0, 0, 1);

		ParameterTable *table = get_config_by_id(161001013, &parameter_config);
		if (table != NULL && req->rand)
		{
			std::map<uint32_t, uint32_t> item_list;
			for (uint32_t i = 0; i < table->n_parameter1; i += 2)
			{
				item_list.insert(std::make_pair(table->parameter1[i], table->parameter1[i + 1]));
			}
			player->add_item_list_as_much_as_possible(item_list, MAGIC_TYPE_ZHENYING);
		}
	}

	AnsChoseZhenying send;
	ans_chose_zhenying__init(&send);
	send.ret = req->ret;
	send.zhenying = req->zhenying;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHOSE_ZHENYING_ANSWER, ans_chose_zhenying__pack, send);
	add_zhenying_player__free_unpacked(req, NULL);

	return 0;
}
static int handle_server_change_zhenying(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	AddZhenyingPlayer *req = add_zhenying_player__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	if (req->ret == 0)
	{
		std::map<uint64_t, struct CampTable*>::iterator it = zhenying_base_config.begin();
		CampTable * config = it->second;
		if (player->data->zhenying.free > 0 && player->get_attr(PLAYER_ATTR_LEVEL) > config->FreeLv)
		{
			--player->data->zhenying.free;
		}
		else
		{
			int gold = req->gold;
			if (player->get_comm_gold() < gold)
			{
				gold = player->get_comm_gold();
			}
			player->sub_comm_gold(gold, MAGIC_TYPE_ZHENYING);
		}

		player->set_attr(PLAYER_ATTR_ZHENYING, req->zhenying);
		AttrMap nty_list;
		nty_list[PLAYER_ATTR_ZHENYING] = player->get_attr(PLAYER_ATTR_ZHENYING);
		player->notify_attr(nty_list, true, false);
		ParameterTable * table = get_config_by_id(161000162, &parameter_config);
		if (table != NULL)
		{
			player->data->zhenying.change_cd = time_helper::get_cached_time() / 1000 + table->parameter1[0];
		}
		player->add_task_progress(TCT_JOIN_CAMP, req->zhenying, 1);
		player->refresh_player_redis_info(false);
		player->add_achievement_progress(ACType_ZHENYING_CHANGE, 0, 0, 0, 1);

		ProtoGuildInfo *info = get_guild_summary(player->data->guild_id);
		if(info != NULL)
		{
			if(info->master_id == player->data->player_id)
			{
				info->zhenying = req->zhenying;
			}
		}
	}

	AnsChoseZhenying send;
	ans_chose_zhenying__init(&send);
	send.ret = req->ret;
	send.zhenying = req->zhenying;
	send.free = player->data->zhenying.free;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHANGE_ZHENYING_ANSWER, ans_chose_zhenying__pack, send);

	if (req->ret == 0)
	{
		player->clear_zhenying_task();
	}
	add_zhenying_player__free_unpacked(req, NULL);

	return 0;
}
static int chose_zhenying_impl(player_struct *player, EXTERN_DATA *extern_data, int zhenYing, bool randChosse)
{
	if (player->get_attr(PLAYER_ATTR_ZHENYING) != 0)
	{
		return -2;
	}
	std::map<uint64_t, struct CampTable*>::iterator it = zhenying_base_config.begin();
	CampTable * config = it->second;
	if (player->get_attr(PLAYER_ATTR_LEVEL) < config->Level)
	{
		return 1;
	}
	//player->set_attr(PLAYER_ATTR_ZHENYING, zhenYing);

	//AnsChoseZhenying send;
	//ans_chose_zhenying__init(&send);
	//send.ret = 0;
	//send.zhenying = zhenYing;
	//fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CHOSE_ZHENYING_ANSWER, ans_chose_zhenying__pack, send);

	AddZhenyingPlayer tofr;
	add_zhenying_player__init(&tofr);
	tofr.name = player->get_name();
	tofr.zhenying = zhenYing;
	tofr.zhenying_old = player->get_attr(PLAYER_ATTR_ZHENYING);
	tofr.fighting_capacity = player->get_attr(PLAYER_ATTR_FIGHTING_CAPACITY);
	tofr.kill = player->data->zhenying.kill;
	tofr.free = player->data->zhenying.free;
	tofr.rand = randChosse;
	conn_node_gamesrv::connecter.send_to_friend(extern_data, MSG_ID_CHOSE_ZHENYING_REQUEST, &tofr, (pack_func)add_zhenying_player__pack);

	return 0;
}
static int handle_rand_chose_zhenying_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}
	if (player->get_attr(PLAYER_ATTR_ZHENYING) != 0)
	{
		return -2;
	}
	chose_zhenying_impl(player, extern_data, rand() % 2 + 1, true);
	return 0;
}
static int handle_chose_zhenying_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	ChoseZhenying *req = chose_zhenying__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	int zhenYing = req->zhenying;
	chose_zhenying__free_unpacked(req, NULL);

	chose_zhenying_impl(player, extern_data, zhenYing, false);

	return 0;
}


static void on_login_send_question(player_struct *player, EXTERN_DATA *extern_data)
{
	if (player->data->common_answer.number == 0)
	{
		player->refresh_question_oneday(); 	
	}
	if (player->data->award_answer.trun == 0)
	{
		player->data->award_answer.question = get_rand_question(sg_award_question);
		player->data->award_answer.contin = 0;
		player->data->award_answer.exp = 0;
		player->data->award_answer.money = 0;
		player->data->award_answer.number = 1;
		player->data->award_answer.timer = 0;
		player->data->award_answer.score = 0;
		player->data->award_answer.trun = 1;
	}

	AwardAnswer send;
	award_answer__init(&send);
	send.trun = player->data->award_answer.trun;
	send.npc = player->data->award_answer.npc;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_AWARD_QUESTION_NOTIFY, award_answer__pack, send);
}
static int handle_get_common_question_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	EventCalendarTable *tableAct = get_config_by_id(COMMON_QUESTION_ACTIVE_ID, &activity_config);
	if (tableAct == NULL)
	{
		return -3;
	}
	uint32_t cd = 0;
	if (check_active_open(tableAct->RelationID, cd))
	{
		if (player->data->common_answer.question == 0 && player->data->common_answer.next_open < time_helper::get_cached_time() / 1000)
		{
			player->refresh_question_oneday();
		}
	}

	CommonQuestion send;
	common_question__init(&send);
	send.question = player->data->common_answer.question;
	send.contin = player->data->common_answer.contin;
	send.right = player->data->common_answer.right;
	send.money = player->data->common_answer.money;
	send.exp = player->data->common_answer.exp;
	send.tip = player->data->common_answer.tip;
	send.help = player->data->common_answer.help;
	send.btip = player->data->common_answer.btip;
	send.bhelp = player->data->common_answer.bhelp;
	send.number = player->data->common_answer.number;
	send.answer = player->data->common_answer.answer;
	send.anstip = player->data->common_answer.answer_tip;
	send.n_answer = MAX_QUESTION_ANSWER;
	send.n_anstip = 2;
	send.cd = cd;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GET_COMMON_QUESTION_ANSWER, common_question__pack, send);

	return 0;
}
static void get_question_award(player_struct &player, int rightAnswer, uint32_t contin, uint64_t *addCoin, uint64_t *addExp)
{
	QuestionTable *table = get_config_by_id(player.data->common_answer.question, &questions_config);
	ParameterTable *tableExp = get_config_by_id(161000314, &parameter_config);
	ActorLevelTable *tableLv = get_actor_level_config(player.get_attr(PLAYER_ATTR_JOB), player.get_attr(PLAYER_ATTR_LEVEL));
	if (tableExp == NULL)
	{
		return;
	}
	if (tableLv == NULL)
	{
		return;
	}
	if (table == NULL)
	{
		return;
	}
	int i = 0;
	if (rightAnswer == 0)
	{
		if (contin > 0)
		{
			i = 2;
		}
	}
	else
	{
		i = 1;
	}
	*addCoin = tableLv->QueLvCoin * tableExp->parameter1[i] * table->Coin / 10000;
	*addExp = tableLv->QueLvExp * tableExp->parameter1[i] * table->Exp / 10000;
}
static int handle_answer_common_question_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	ReqAnswer *req = req_answer__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t answer = req->id;
	req_answer__free_unpacked(req, NULL);

	EventCalendarTable *tableAct = get_config_by_id(COMMON_QUESTION_ACTIVE_ID, &activity_config);
	if (tableAct == NULL)
	{
		return -3;
	}
	uint32_t cd = 0;
	if (!check_active_open(tableAct->RelationID, cd))
	{
		return -4;
	}

	QuestionTable *table = get_config_by_id(player->data->common_answer.question, &questions_config);
	ParameterTable *tableNum = get_config_by_id(161000163, &parameter_config);
	if (tableNum == NULL)
	{
		return -6;
	}
	if (table == NULL)
	{
		return -2;
	}
	int rightAnswer = 0;
	uint64_t addCoin = 0;
	uint64_t addExp = 0;
	uint32_t contin = player->data->common_answer.contin;
	if (answer == table->RightAnseer)
	{
		++player->data->common_answer.right;
		++player->data->common_answer.contin;
	}
	else
	{
		rightAnswer = 1;
		player->data->common_answer.contin = 0;
	}
	get_question_award(*player, rightAnswer, contin, &addCoin, &addExp);
	
	player->add_exp(addExp, MAGIC_TYPE_QUESTION);
	player->data->common_answer.exp += addExp;

	player->add_coin(addCoin, MAGIC_TYPE_QUESTION);
	player->data->common_answer.money += addCoin;

	if (player->data->common_answer.number == tableNum->parameter1[0]) //完成一次每日答题
	{
		player->data->common_answer.question = 0;
		player->check_activity_progress(AM_ANSWER, QUESTION_DAILY);
		player->add_task_progress(TCT_QUESTION_JOIN, 0, 1);
		player->add_achievement_progress(ACType_ANSWER, QUESTION_DAILY, 0, 0, 1);
		if (player->data->common_answer.right == player->data->common_answer.number)
		{
			player->add_achievement_progress(ACType_DAILY_ANSWER_ALL_RIGHT, 0, 0, 0, 1);
		}
		player->data->common_answer.next_open = time_helper::get_cached_time() / 1000 + 3 + cd;
	}
	else
	{
		++player->data->common_answer.number;
		player->gen_common_question();
	}
	player->data->common_answer.bhelp = false;
	player->data->common_answer.btip = false;

	send_comm_answer(MSG_ID_ANSWER_COMMON_QUESTION_ANSWER, rightAnswer, extern_data);
	return 0;
}
static int handle_common_question_hint_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->data->common_answer.btip || player->data->common_answer.tip == 0)
	{
		return 0;
	}

	player->data->common_answer.btip = true;
	--player->data->common_answer.tip;
	send_comm_answer(MSG_ID_COMMON_QUESTION_HINT_ANSWER, 0, extern_data);

	return 0;
}
static int handle_common_question_help_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	if (player->data->common_answer.bhelp || player->data->common_answer.help == 0)
	{
		return 0;
	}

	player->data->common_answer.bhelp = true;
	--player->data->common_answer.help;

	send_comm_answer(MSG_ID_COMMON_QUESTION_HELP_ANSWER, 0, extern_data);

	return 0;
}
static int handle_get_award_question_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	EventCalendarTable *table = get_config_by_id(AWARD_QUESTION_ACTIVE_ID, &activity_config);
	if (table == NULL)
	{
		return -2;
	}
	uint32_t cd = 0;
	if (!check_active_open(table->RelationID, cd))
	{
		return -4;
	}
	
	AwardAnswer *req = award_answer__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	if (req->trun == player->data->award_answer.trun || req->trun == 0)
	{
		AwardQuestion send;
		award_question__init(&send);
		send.question = player->data->award_answer.question;
		send.contin = player->data->award_answer.contin;
		send.right = player->data->award_answer.right;
		send.money = player->data->award_answer.money;
		send.exp = player->data->award_answer.exp;
		send.number = player->data->award_answer.number;
		send.trun = player->data->award_answer.trun;
		send.npc = player->data->award_answer.npc;
		send.timer = player->data->award_answer.timer;
		send.cd = cd;
		ParameterTable *tablePoint = get_config_by_id(161000454, &parameter_config);
		if (tablePoint != NULL)
		{
			send.score = player->data->award_answer.score + tablePoint->parameter1[0] * (player->data->award_answer.right + player->data->award_answer.contin * 20 / 100);
		}
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GET_AWARD_QUESTION_ANSWER, award_question__pack, send);

		if (player->data->award_answer.question != 0 && req->trun != 0)
		{
			player->data->award_answer.begin_time = time_helper::get_cached_time() / 1000;
			player->data->award_answer.bOpenWin = true;
		}
	}
	
	award_answer__free_unpacked(req, NULL);
	return 0;
}
static int handle_answer_award_question_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	EventCalendarTable *tableE = get_config_by_id(AWARD_QUESTION_ACTIVE_ID, &activity_config);
	if (tableE == NULL)
	{
		return -2;
	}
	if (tableE->SubtabValue > player->get_attr(PLAYER_ATTR_LEVEL))
	{
		return -11;
	}
	uint32_t cdAct = 0;
	if (!check_active_open(tableE->RelationID, cdAct))
	{
		return -4;
	}

	ReqAnswer *req = req_answer__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t answer = req->id;
	bool timeout = req->timeout;
	req_answer__free_unpacked(req, NULL);

	QuestionTable *table = get_config_by_id(player->data->award_answer.question, &questions_config);
	if (table == NULL)
	{
		return -2;
	}
	ParameterTable *tableExp = get_config_by_id(161000199, &parameter_config);
	if (tableExp == NULL)
	{
		return -3;
	}
	ParameterTable *tablePoint = get_config_by_id(161000454, &parameter_config);
	int rightAnswer = 0;
	uint32_t conti = player->data->award_answer.contin;
	ParameterTable * config1 = get_config_by_id(161000170, &parameter_config);
	uint64_t cd = time_helper::get_cached_time() / 1000 - player->data->award_answer.begin_time;
	if (cd > config1->parameter1[0])
	{
		cd = config1->parameter1[0];
	}
	else
	{
		if (answer == table->RightAnseer)
		{
			++player->data->award_answer.right;
			++player->data->award_answer.contin;
			if (tablePoint != NULL)
			{
				if (cd < 6)
				{
					player->data->award_answer.score += tablePoint->parameter1[1];
				}
				else if (cd > 5 && cd < 11)
				{
					player->data->award_answer.score += tablePoint->parameter1[2];
				}
				else
				{
					player->data->award_answer.score += tablePoint->parameter1[3];
				}
				player->refresh_player_redis_info();
			}
		}
		else
		{
			cd = config1->parameter1[0];
			rightAnswer = 1;
			player->data->award_answer.contin = 0;
		}
	}
	if (timeout)
	{
		rightAnswer = 3;
	}
	send_comm_answer(MSG_ID_ANSWER_AWARD_QUESTION_ANSWER, rightAnswer, extern_data);

	uint64_t addCoin = 0;
	uint64_t addExp = 0;
	get_question_award(*player, rightAnswer, conti, &addCoin, &addExp);

	player->add_exp(addExp, MAGIC_TYPE_QUESTION);
	player->data->award_answer.exp += addExp;

	player->add_coin(addCoin, MAGIC_TYPE_QUESTION);
	player->data->award_answer.money += addCoin;

	player->data->award_answer.timer += cd;
	ParameterTable * config2 = get_config_by_id(161000169, &parameter_config);
	if (player->data->award_answer.number == config2->parameter1[0])
	{
		ParameterTable * config3 = get_config_by_id(161000168, &parameter_config);
		if (player->data->award_answer.trun == config3->parameter1[0])
		{
			player->data->award_answer.question = 0;
		}
		else
		{
			player->data->award_answer.number = 1;
			++player->data->award_answer.trun;
			player->data->award_answer.question = get_rand_question(sg_award_question);
			//send_comm_answer(MSG_ID_NEXT_AWARD_QUESTION_NOTIFY, 0, extern_data);
			//on_login_send_question(player, extern_data); //完成一次任务答题
			AwardAnswer send;
			award_answer__init(&send);
			send.trun = player->data->award_answer.trun;
			send.npc = player->data->award_answer.npc;
			fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_AWARD_QUESTION_NOTIFY, award_answer__pack, send);
		}
		player->check_activity_progress(AM_ANSWER, QUESTION_AWARD);
		player->add_task_progress(TCT_QUESTION, 0, 1);
		player->add_achievement_progress(ACType_ANSWER, QUESTION_AWARD, 0, 0, 1);
		player->data->award_answer.bOpenWin = false;

		//AwardQuestion send;
		//award_question__init(&send);
		//send.question = player->data->award_answer.question;
		//send.contin = player->data->award_answer.contin;
		//send.right = player->data->award_answer.right;
		//send.money = player->data->award_answer.money;
		//send.exp = player->data->award_answer.exp;
		//send.number = player->data->award_answer.number;
		//send.trun = player->data->award_answer.trun;
		//send.npc = player->data->award_answer.npc;
		//send.timer = player->data->award_answer.timer;
		//send.cd = cdAct;
		//
		//if (tablePoint != NULL)
		//{
		//	send.score = player->data->award_answer.score + tablePoint->parameter1[0] * (player->data->award_answer.right + player->data->award_answer.contin * 20 / 100);
		//}
		//fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GET_AWARD_QUESTION_ANSWER, award_question__pack, send);
	}
	else
	{
		++player->data->award_answer.number;
		player->data->award_answer.question = get_rand_question(sg_award_question);
	}

	//player->add_task_progress(TCT_QUESTION, 0, 1);

	return 0;
}
static int handle_interupt_award_question_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	EventCalendarTable *table = get_config_by_id(AWARD_QUESTION_ACTIVE_ID, &activity_config);
	if (table == NULL)
	{
		return -2;
	}
	uint32_t cd = 0;
	if (!check_active_open(table->RelationID, cd))
	{
		return -4;
	}

	player->logout_check_award_question();
	player->data->award_answer.bOpenWin = false;

	return 0;
}
static int handle_first_award_question_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->data)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	EventCalendarTable *table = get_config_by_id(AWARD_QUESTION_ACTIVE_ID, &activity_config);
	if (table == NULL)
	{
		return -2;
	}
	if (!player->activity_is_unlock(table->ID))
	{
		return -3;
	}
	uint32_t cd = 0;
	if (!check_active_open(table->RelationID, cd))
	{
		return -4;
	}

	if (player->data->award_answer.next_open < time_helper::get_cached_time() / 1000)
	{
		if (player->accept_task(table->AuxiliaryValue[0], false) != 0)
			return -5;
		player->data->award_answer.next_open = time_helper::get_cached_time() / 1000 + cd + 600;
		player->data->award_answer.question = get_rand_question(sg_award_question);
		player->data->award_answer.contin = 0;
		player->data->award_answer.exp = 0;
		player->data->award_answer.money = 0;
		player->data->award_answer.number = 1;
		player->data->award_answer.timer = 0;
		player->data->award_answer.trun = 1;
	}

	return 0;
}

static int on_login_send_live_skill(player_struct *player, EXTERN_DATA *extern_data)
{
	if (player->data->live_skill.level[LIVE__SKILL__TYPE__MEDICINE] == 0)
	{
		player->data->live_skill.level[LIVE__SKILL__TYPE__MEDICINE] = 1;
		player->data->live_skill.level[LIVE__SKILL__TYPE__COOK] = 1;
		ParameterTable *tablePar = get_config_by_id(161000175, &parameter_config);
		if (tablePar != NULL)
		{
			player->data->live_skill.book[LIVE__SKILL__TYPE__COOK] = tablePar->parameter1[0];
			player->data->live_skill.book[LIVE__SKILL__TYPE__MEDICINE] = tablePar->parameter1[0];
		}
		
		LifeSkillTable *table = get_medicine_table(LIVE__SKILL__TYPE__MEDICINE, 1);
		if (table != NULL)
		{
			player->data->live_skill.broken[LIVE__SKILL__TYPE__MEDICINE] = table->LvMax;
		}
		table = get_medicine_table(LIVE__SKILL__TYPE__COOK, 1);
		if (table != NULL)
		{
			player->data->live_skill.broken[LIVE__SKILL__TYPE__COOK] = table->LvMax;
		}

		for (std::map<uint64_t, struct BiaocheTable*>::iterator it = cash_truck_config.begin(); it != cash_truck_config.end(); ++it)
		{
			ControlTable *tableAct = get_config_by_id(it->second->ActivityControl, &all_control_config);
			if (it->second->Type == 1)
			{
				player->data->truck.num_coin = tableAct->RewardTime;
			}
			else if (it->second->Type == 2)
			{
				player->data->truck.num_gold = tableAct->RewardTime;
			}
		}
	}

	CashTruckInfo note;
	cash_truck_info__init(&note);
	note.num_cion = player->data->truck.num_coin;
	note.num_gold = player->data->truck.num_gold;
	note.cash_truck = player->data->truck.truck_id;
	note.mapid = player->data->truck.scene_id;
	note.x = player->data->truck.pos.pos_x;
	note.z = player->data->truck.pos.pos_z;
	BiaocheTable *tableTruck = get_config_by_id(player->data->truck.active_id, &cash_truck_config);
	if (tableTruck != NULL)
	{
		note.task = tableTruck->TaskId;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CASH_TRUCK_INFO_NOTIFY, cash_truck_info__pack, note);

	AnsLiveSkill send;
	ans_live_skill__init(&send);
	send.level = player->data->live_skill.level + 1;
	send.exp = player->data->live_skill.exp + 1;
	send.book = player->data->live_skill.book + 1;
	send.broken = player->data->live_skill.broken + 1;
	send.n_level = MAX_LIVE_SKILL_NUM;
	send.n_exp = MAX_LIVE_SKILL_NUM;
	send.n_book = MAX_LIVE_SKILL_NUM;
	send.n_broken = MAX_LIVE_SKILL_NUM;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LIVE_SKILL_NOTIFY, ans_live_skill__pack, send);

	return 0;
}

static int handle_team_info_request(player_struct *player, EXTERN_DATA *extern_data) 
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	//时装
	on_login_send_all_fashion(player, extern_data);

	//技能
	player->m_skill.SendAllSkill();

	on_login_send_all_horse(player, extern_data);
	//on_login_send_yaoshi(player, extern_data);
	on_login_send_zhenying(player, extern_data);
	on_login_send_question(player, extern_data);

	handle_first_award_question_request(player, extern_data);
	//队伍
	if (player->m_team == NULL)
	{
		return 2;
	}
	player->m_team->SendWholeTeamInfo(*player);
	
	return 0;
}

//个人信息请求
static int handle_personality_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (comm_check_player_valid(player, extern_data->player_id) != 0)
	{
		LOG_ERR("%s: %lu common check failed", __FUNCTION__, extern_data->player_id);
		return (-1);
	}

	PersonalityInfoAnswer resp;
	personality_info_answer__init(&resp);

	PersonalityData personality_data;
	personality_data__init(&personality_data);

	resp.result = 0;
	resp.data = &personality_data;
	personality_data.sex = player->data->personality_sex;
	personality_data.birthday = player->data->personality_birthday;
	personality_data.location = player->data->personality_location;
	personality_data.textintro = player->data->personality_text_intro;
	personality_data.voiceintro = player->data->personality_voice_intro;
	personality_data.tags = player->data->personality_tags;
	personality_data.n_tags = 0;
	for (int i = 0; i < MAX_PERSONALITY_TAG_NUM; ++i)
	{
		if (player->data->personality_tags[i] == 0)
		{
			break;
		}
		personality_data.n_tags++;
	}
	personality_data.province = player->data->personality_province;
	personality_data.city = player->data->personality_city;
	personality_data.bloodtype = player->data->personality_blood_type;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PERSONALITY_INFO_ANSWER, personality_info_answer__pack, resp);

	return 0;
}

//设置普通信息请求
static int handle_personality_set_general_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PersonalitySetGeneralRequest *req = personality_set_general_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	int ret = 0;
	do
	{
		if (strlen(req->location) >= MAX_PERSONALITY_LOCATION_LEN)
		{
			ret = -1;
			break;
		}
		player->data->personality_sex = req->sex;
		player->data->personality_birthday = req->birthday;
		strcpy(player->data->personality_location, req->location);
		player->data->personality_province = req->province;
		player->data->personality_city = req->city;
		player->data->personality_blood_type = req->bloodtype;

		player->refresh_player_redis_info();
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PERSONALITY_SET_GENERAL_ANSWER, comm_answer__pack, resp);

	personality_set_general_request__free_unpacked(req, NULL);

	return 0;
}

//设置标签信息请求
static int handle_personality_set_tags_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PersonalitySetTagsRequest *req = personality_set_tags_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	int ret = 0;
	do
	{
		memset(player->data->personality_tags, 0, sizeof(player->data->personality_tags));
		for (size_t i = 0; i < req->n_tags && i < MAX_PERSONALITY_TAG_NUM; ++i)
		{
			player->data->personality_tags[i] = req->tags[i];
		}

		player->refresh_player_redis_info();
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PERSONALITY_SET_TAGS_ANSWER, comm_answer__pack, resp);

	personality_set_tags_request__free_unpacked(req, NULL);
	return 0;
}

//设置签名信息请求
static int handle_personality_set_intro_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PersonalitySetIntroRequest *req = personality_set_intro_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	int ret = 0;
	do
	{
		if (req->textintro)
		{
			if (strlen(req->textintro) >= MAX_PERSONALITY_TEXT_INTRO_LEN)
			{
				ret = -1;
				break;
			}
			strcpy(player->data->personality_text_intro, req->textintro);
			player->refresh_player_redis_info();
		}
		if (req->voiceintro)
		{
			if (strlen(req->voiceintro) >= MAX_PERSONALITY_VOICE_INTRO_LEN)
			{
				ret = -1;
				break;
			}
			strcpy(player->data->personality_voice_intro, req->voiceintro);
		}
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PERSONALITY_SET_INTRO_ANSWER, comm_answer__pack, resp);

	personality_set_intro_request__free_unpacked(req, NULL);

	return 0;
}

void answer_get_other_info(EXTERN_DATA *extern_data, int result, player_struct *target, uint32_t status)
{
	GetOtherInfoAnswer resp;
	get_other_info_answer__init(&resp);

	resp.result = result;
	if (result != 0)
	{
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GET_OTHER_INFO_ANSWER, get_other_info_answer__pack, resp);
		return ;
	}

	OtherDetailData detail_data;
	other_detail_data__init(&detail_data);

	EquipData equip_data[MAX_EQUIP_NUM];
	EquipData* equip_data_point[MAX_EQUIP_NUM];
	EquipEnchantData enchant_data[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM];
	EquipEnchantData* enchant_data_point[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData cur_attr[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData* cur_attr_point[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM];
	CommonRandAttrData rand_attr[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM][MAX_EQUIP_ENCHANT_RAND_NUM];
	CommonRandAttrData* rand_attr_point[MAX_EQUIP_NUM][MAX_EQUIP_ENCHANT_NUM][MAX_EQUIP_ENCHANT_RAND_NUM];
	ProposeRingInfo ring_attr_info;

	BaguapaiDressData dress_data[MAX_BAGUAPAI_STYLE_NUM];
	BaguapaiDressData* dress_data_point[MAX_BAGUAPAI_STYLE_NUM];
	BaguapaiCardData card_data[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM];
	BaguapaiCardData* card_data_point[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM];
	CommonRandAttrData attr_data[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* attr_data_point[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData attr_new_data[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData* attr_new_data_point[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_MINOR_ATTR_NUM];
	CommonRandAttrData  additional_attr_data[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];
	CommonRandAttrData* additional_attr_data_point[MAX_BAGUAPAI_STYLE_NUM][MAX_BAGUAPAI_DRESS_NUM][MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM];

	resp.data = &detail_data;
	detail_data.playerid = target->data->player_id;
	detail_data.name = target->data->name;
	//AttrData Info
	AttrData player_attr[PLAYER_ATTR_MAX];
	AttrData* player_attr_point[PLAYER_ATTR_MAX];
	std::set<uint32_t> sAttrIds;
	sAttrIds.insert(PLAYER_ATTR_HP);//1
	sAttrIds.insert(PLAYER_ATTR_MAXHP);//2
	sAttrIds.insert(PLAYER_ATTR_FIGHTING_CAPACITY);//36
	sAttrIds.insert(PLAYER_ATTR_LEVEL);//45
	sAttrIds.insert(PLAYER_ATTR_ZHENYING);//47
	sAttrIds.insert(PLAYER_ATTR_JOB);//51
	sAttrIds.insert(PLAYER_ATTR_EXP);//53
	sAttrIds.insert(PLAYER_ATTR_HEAD);//57
	sAttrIds.insert(PLAYER_ATTR_CLOTHES);//58
	sAttrIds.insert(PLAYER_ATTR_CLOTHES_COLOR_UP);//59
	sAttrIds.insert(PLAYER_ATTR_HAT);//60
	sAttrIds.insert(PLAYER_ATTR_HAT_COLOR);//61
	sAttrIds.insert(PLAYER_ATTR_CLOTHES_COLOR_DOWN);//62
	sAttrIds.insert(PLAYER_ATTR_WEAPON);//65
	sAttrIds.insert(PLAYER_ATTR_BAGUA);//72
	sAttrIds.insert(PLAYER_ATTR_WEAPON_COLOR);//84
	sAttrIds.insert(PLAYER_ATTR_TITLE);//103
	sAttrIds.insert(PLAYER_ATTR_SEX);//110

// 读配置文件
	uint32_t carrerid = 101000000 + target->get_job() + target->get_sex()*100;
	std::map<uint64_t, struct ActorTable *>::iterator itAt = actor_config.find(carrerid);
	if (itAt != actor_config.end())
	{
		for (uint32_t idx = 0; idx < itAt->second->n_BasiceAttribute; ++idx)
			sAttrIds.insert(itAt->second->BasiceAttribute[idx]);
		for (uint32_t idx = 0; idx < itAt->second->n_LvAttributeNum; ++idx)
			sAttrIds.insert(itAt->second->LvAttributeNum[idx]);
		for (uint32_t idx = 0; idx < itAt->second->n_FiveAttribute; ++idx)
			sAttrIds.insert(itAt->second->FiveAttribute[idx]);
		for (uint32_t idx = 0; idx < itAt->second->n_SeniorAttribute; ++idx)
			sAttrIds.insert(itAt->second->SeniorAttribute[idx]);
		for (uint32_t idx = 0; idx < itAt->second->n_BuffAttribute; ++idx)
			sAttrIds.insert(itAt->second->BuffAttribute[idx]);
	}	

	detail_data.n_attrs = 0;
	detail_data.attrs = player_attr_point;
	for ( std::set<uint32_t>::iterator its = sAttrIds.begin(); its != sAttrIds.end(); ++its )
	{
		player_attr_point[detail_data.n_attrs] = &player_attr[detail_data.n_attrs];
		attr_data__init(&player_attr[detail_data.n_attrs]);
		player_attr[detail_data.n_attrs].id = *its;
		player_attr[detail_data.n_attrs].val = target->get_attr(*its);
		detail_data.n_attrs++;
	}
// end

	PersonalityData personality_data;
	personality_data__init(&personality_data);
	detail_data.personality = &personality_data;
	personality_data.sex = target->data->personality_sex;
	personality_data.sex = target->data->personality_sex;
	personality_data.birthday = target->data->personality_birthday;
	personality_data.location = target->data->personality_location;
	personality_data.textintro = target->data->personality_text_intro;
	personality_data.voiceintro = target->data->personality_voice_intro;
	personality_data.tags = target->data->personality_tags;
	personality_data.n_tags = 0;
	for (int i = 0; i < MAX_PERSONALITY_TAG_NUM; ++i)
	{
		if (target->data->personality_tags[i] == 0)
		{
			break;
		}
		personality_data.n_tags++;
	}
	personality_data.province = target->data->personality_province;
	personality_data.city = target->data->personality_city;
	personality_data.bloodtype = target->data->personality_blood_type;
	size_t equip_num = 0;
	for (int k = 0; k < MAX_EQUIP_NUM; ++k)
	{
		EquipInfo &equip_info = target->data->equip_list[k];
		if (equip_info.stair == 0)
		{
			continue;
		}

		equip_data_point[equip_num] = &equip_data[equip_num];
		equip_data__init(&equip_data[equip_num]);

		equip_data[equip_num].type = k + 1;
		equip_data[equip_num].stair = equip_info.stair;
		equip_data[equip_num].starlv = equip_info.star_lv;
		equip_data[equip_num].starexp = equip_info.star_exp;
		if(equip_data[equip_num].type == 9)
		{
			WeddingRing *wedding_ring_config = get_config_by_id(target->data->player_marry_info.propose_type, &propose_ring_config);
			if(wedding_ring_config != NULL)
			{
				propose_ring_info__init(&ring_attr_info);
				ring_attr_info.ring_id = wedding_ring_config->ID;
				ring_attr_info.target_name = target->data->player_marry_info.target_name;
				equip_data[equip_num].ring_info = &ring_attr_info;
			}
		}
		size_t enchant_num = 0;
		for (int i = 0; i < MAX_EQUIP_ENCHANT_NUM; ++i)
		{
			EquipEnchantInfo &enchant_info = equip_info.enchant[i];

			enchant_data_point[equip_num][enchant_num] = &enchant_data[equip_num][enchant_num];
			equip_enchant_data__init(&enchant_data[equip_num][enchant_num]);
			enchant_data[equip_num][enchant_num].index = i;

			cur_attr_point[equip_num][enchant_num] = &cur_attr[equip_num][enchant_num];
			common_rand_attr_data__init(&cur_attr[equip_num][enchant_num]);
			cur_attr[equip_num][enchant_num].pool = enchant_info.cur_attr.pool;
			cur_attr[equip_num][enchant_num].id = enchant_info.cur_attr.id;
			cur_attr[equip_num][enchant_num].val = enchant_info.cur_attr.val;
			enchant_data[equip_num][enchant_num].curattr = cur_attr_point[equip_num][enchant_num];

			size_t rand_num = 0;
			if (enchant_info.rand_attr[0].id > 0)
			{
				for (int j = 0; j < MAX_EQUIP_ENCHANT_RAND_NUM; ++j)
				{
					rand_attr_point[equip_num][enchant_num][rand_num] = &rand_attr[equip_num][enchant_num][rand_num];
					common_rand_attr_data__init(&rand_attr[equip_num][enchant_num][rand_num]);
					rand_attr[equip_num][enchant_num][rand_num].pool = enchant_info.rand_attr[j].pool;
					rand_attr[equip_num][enchant_num][rand_num].id = enchant_info.rand_attr[j].id;
					rand_attr[equip_num][enchant_num][rand_num].val = enchant_info.rand_attr[j].val;
					rand_num++;
				}
			}
			enchant_data[equip_num][enchant_num].randattr = rand_attr_point[equip_num][enchant_num];
			enchant_data[equip_num][enchant_num].n_randattr = rand_num;

			enchant_num++;
		}
		equip_data[equip_num].n_enchant = enchant_num;
		equip_data[equip_num].enchant = enchant_data_point[equip_num];

		equip_data[equip_num].inlay = &equip_info.inlay[0];
		equip_data[equip_num].n_inlay = MAX_EQUIP_INLAY_NUM;
		equip_num++;
	}
	detail_data.equips = equip_data_point;
	detail_data.n_equips = equip_num;

	uint32_t style_num = 0;
	for (int i = 0; i < MAX_BAGUAPAI_STYLE_NUM; ++i)
	{
		dress_data_point[style_num] = &dress_data[style_num];
		baguapai_dress_data__init(&dress_data[style_num]);
		dress_data[style_num].styleid = i + 1;

		uint32_t card_num = 0;
		for (int j = 0; j < MAX_BAGUAPAI_DRESS_NUM; ++j)
		{
			card_data_point[style_num][card_num] = &card_data[style_num][card_num];
			baguapai_card_data__init(&card_data[style_num][card_num]);
			card_data[style_num][card_num].id = target->data->baguapai_dress[i].card_list[j].id;
			card_data[style_num][card_num].star = target->data->baguapai_dress[i].card_list[j].star;

			uint32_t cur_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++k)
			{
				if (target->data->baguapai_dress[i].card_list[j].minor_attrs[k].id == 0)
				{
					break;
				}

				attr_data_point[style_num][card_num][cur_num] = &attr_data[style_num][card_num][cur_num];
				common_rand_attr_data__init(&attr_data[style_num][card_num][cur_num]);
				attr_data[style_num][card_num][cur_num].pool = target->data->baguapai_dress[i].card_list[j].minor_attrs[k].pool;
				attr_data[style_num][card_num][cur_num].id = target->data->baguapai_dress[i].card_list[j].minor_attrs[k].id;
				attr_data[style_num][card_num][cur_num].val = target->data->baguapai_dress[i].card_list[j].minor_attrs[k].val;
				cur_num++;
			}
			card_data[style_num][card_num].n_minorattrs = cur_num;
			card_data[style_num][card_num].minorattrs = attr_data_point[style_num][card_num];

			uint32_t new_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_MINOR_ATTR_NUM; ++k)
			{
				if (target->data->baguapai_dress[i].card_list[j].minor_attrs_new[k].id == 0)
				{
					break;
				}

				attr_new_data_point[style_num][card_num][new_num] = &attr_new_data[style_num][card_num][new_num];
				common_rand_attr_data__init(&attr_new_data[style_num][card_num][new_num]);
				attr_new_data[style_num][card_num][new_num].pool = target->data->baguapai_dress[i].card_list[j].minor_attrs_new[k].pool;
				attr_new_data[style_num][card_num][new_num].id = target->data->baguapai_dress[i].card_list[j].minor_attrs_new[k].id;
				attr_new_data[style_num][card_num][new_num].val = target->data->baguapai_dress[i].card_list[j].minor_attrs_new[k].val;
				new_num++;
			}
			card_data[style_num][card_num].n_minorattrsnew = new_num;
			card_data[style_num][card_num].minorattrsnew = attr_new_data_point[style_num][card_num];

			cur_num = 0;
			for (int k = 0; k < MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM; ++k)
			{
				if (target->data->baguapai_dress[i].card_list[j].additional_attrs[k].id == 0)
				{
					break;
				}

				additional_attr_data_point[style_num][card_num][cur_num] = &additional_attr_data[style_num][card_num][cur_num];
				common_rand_attr_data__init(&additional_attr_data[style_num][card_num][cur_num]);
				additional_attr_data[style_num][card_num][cur_num].pool = target->data->baguapai_dress[i].card_list[j].additional_attrs[k].pool;
				additional_attr_data[style_num][card_num][cur_num].id = target->data->baguapai_dress[i].card_list[j].additional_attrs[k].id;
				additional_attr_data[style_num][card_num][cur_num].val = target->data->baguapai_dress[i].card_list[j].additional_attrs[k].val;
				cur_num++;
			}
			card_data[style_num][card_num].n_additionalattrs = cur_num;
			card_data[style_num][card_num].additionalattrs = additional_attr_data_point[style_num][card_num];

			card_num++;
		}
		dress_data[style_num].n_cards = card_num;
		dress_data[style_num].cards = card_data_point[style_num];

		style_num++;
	}
	detail_data.n_baguas = style_num;
	detail_data.baguas = dress_data_point;

	detail_data.teamid = target->data->teamid;
	detail_data.status = status;
//坐骑信息
//horseinfo

	HorseList  horse_list;
		HorseData horse_data[MAX_HORSE_NUM];
		HorseData* horse_data_point[MAX_HORSE_NUM];

		HorseCommonAttr horse_common_attr;

	horse_list__init(&horse_list);

	uint32_t horse_nums = 0;
	uint64_t now = time_helper::get_cached_time();
	for (uint32_t idx = 0; idx < target->data->n_horse; ++idx )
	{
		horse_data__init(&horse_data[idx]);	
		horse_data[idx].id = target->data->horse[idx].id;
		horse_data[idx].isnew = target->data->horse[idx].isNew;
		horse_data[idx].step = target->data->horse[idx].step;
		horse_data[idx].star = target->data->horse[idx].star;

		if ( 0 != target->data->horse[idx].timeout )
		{
			if (target->data->horse[idx].timeout <= (time_t)now)
			{
				horse_data[idx].isexpire = true;
			}
			{
				horse_data[idx].cd =  target->data->horse[idx].timeout - now;
				horse_data[idx].isexpire = false;	
			}
		}
		else 
		{
			horse_data[idx].isexpire = false;
			horse_data[idx].cd = 0;
		}

		if (horse_data[idx].id == target->get_attr(PLAYER_ATTR_CUR_HORSE))
			horse_data[idx].is_current = true;
		else 
			horse_data[idx].is_current = false;
	
		horse_data_point[idx] = &horse_data[idx];
		
		horse_nums++;
	}
	//HorseCommonAttr 
	horse_common_attr__init(&horse_common_attr);
		

	horse_common_attr.step = target->data->horse_attr.step;
	horse_common_attr.attr = target->data->horse_attr.attr;
	horse_common_attr.attr_level = target->data->horse_attr.attr_exp;		
	horse_common_attr.n_attr = horse_common_attr.n_attr_level = MAX_HORSE_ATTR_NUM;		
		
	horse_common_attr.power = target->data->horse_attr.power;
	horse_common_attr.soul_step = target->data->horse_attr.soul_step;
	horse_common_attr.soul_star = target->data->horse_attr.soul_star;

	horse_list.attr = &horse_common_attr;
	horse_list.data = horse_data_point;
	horse_list.n_data = horse_nums;//坐骑数量
	detail_data.mounts = &horse_list;

	// partnerlist
	PartnerData  partner_data[MAX_PARTNER_NUM];
	PartnerData* partner_point[MAX_PARTNER_NUM];
	//当前属性
	PartnerAttr partner_cur_attr[MAX_PARTNER_NUM];
	//洗髓属性
	PartnerAttr partner_cur_flash[MAX_PARTNER_NUM];
	//属性列表
	AttrData  attr_data_partner[MAX_PARTNER_NUM][MAX_PARTNER_ATTR];
	AttrData* attr_point[MAX_PARTNER_NUM][MAX_PARTNER_ATTR]; PartnerSkillData  skill_data[MAX_PARTNER_NUM][MAX_PARTNER_SKILL_NUM];
	// PartnerAttr 的 技能列表
	PartnerSkillData* skill_point[MAX_PARTNER_NUM][MAX_PARTNER_SKILL_NUM];
	PartnerSkillData  partner_skill_data_flash[MAX_PARTNER_NUM][MAX_PARTNER_SKILL_NUM];
	PartnerSkillData* partner_skill_point_flash[MAX_PARTNER_NUM][MAX_PARTNER_SKILL_NUM];
	//
	PartnerCurFabaoInfo partner_cur_fabao[MAX_PARTNER_NUM];

	AttrData partner_fabao_minor_attr[MAX_PARTNER_NUM][MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData* partner_fabao_minor_attr_point[MAX_PARTNER_NUM][MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData partner_fabao_main_attr[MAX_PARTNER_NUM];

	uint32_t partner_num = 0;
	for (PartnerMap::iterator iter = target->m_partners.begin(); iter!= target->m_partners.end(); ++iter)
	{
		partner_struct *partner = iter->second;
		if(NULL==partner)
		{
			continue;
		}
		partner->calculate_attribute(false);
		partner_point[partner_num] = &partner_data[partner_num];
		partner_data__init(&partner_data[partner_num]);
		partner_data[partner_num].uuid = partner->data->uuid;
		partner_data[partner_num].partnerid = partner->data->partner_id;	
		partner_data[partner_num].relivetime = partner->data->relive_time;	
		partner_data[partner_num].rename_free = partner->data->partner_rename_free;	
	//	strncpy(partner_data[partner_num].name, partner->data->name, strlen(partner->data->name)+1);
		partner_data[partner_num].name = partner->data->name;
		uint32_t attr_num = 0;
		for (int i = 1; i < MAX_PARTNER_ATTR; ++i)
		{
			attr_point[partner_num][attr_num] = &attr_data_partner[partner_num][attr_num];
			attr_data__init(&attr_data_partner[partner_num][attr_num]);
			attr_data_partner[partner_num][attr_num].id = i;
			attr_data_partner[partner_num][attr_num].val = partner->data->attrData[i];
			attr_num++;
		}
		partner_data[partner_num].attrs = attr_point[partner_num];
		partner_data[partner_num].n_attrs = attr_num;
		LOG_INFO("%s:%d partner_num:%d ,djx:attr_num:%u\n", __FUNCTION__,__LINE__, partner_num, attr_num);
		uint32_t skill_num = 0;
		if (partner->data->attr_cur.base_attr_up[0] != 0)
		{
			partner_data[partner_num].cur_attr = partner_cur_attr + partner_num;
			partner_attr__init(partner_cur_attr + partner_num);
			for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
			{
				skill_point[partner_num][skill_num] = &skill_data[partner_num][skill_num];
				partner_skill_data__init(&skill_data[partner_num][skill_num]);
				skill_data[partner_num][skill_num].id = partner->data->attr_cur.skill_list[i].skill_id;
				skill_data[partner_num][skill_num].lv = partner->data->attr_cur.skill_list[i].lv;
				skill_data[partner_num][skill_num].lock = partner->data->attr_cur.skill_list[i].lock;
				skill_data[partner_num][skill_num].exp = partner->data->attr_cur.skill_list[i].exp;
				skill_num++;
			}
			partner_cur_attr[partner_num].skills = skill_point[partner_num];
			partner_cur_attr[partner_num].n_skills = skill_num;
			partner_cur_attr[partner_num].base_attr_id = (uint32_t *)&base_attr_id[0];
			partner_cur_attr[partner_num].n_base_attr_id = MAX_PARTNER_BASE_ATTR;
			partner_cur_attr[partner_num].base_attr_cur = partner->data->attr_cur.base_attr_vaual;
			partner_cur_attr[partner_num].n_base_attr_cur = MAX_PARTNER_BASE_ATTR;
			partner_cur_attr[partner_num].base_attr_up = partner->data->attr_cur.base_attr_up;
			partner_cur_attr[partner_num].n_base_attr_up = MAX_PARTNER_BASE_ATTR;
			partner_cur_attr[partner_num].base_attr_up = partner->data->attr_cur.base_attr_up;
			partner_cur_attr[partner_num].n_base_attr_up = MAX_PARTNER_BASE_ATTR;
			partner_cur_attr[partner_num].detail_attr_id = partner->data->attr_cur.detail_attr_id;
			partner_cur_attr[partner_num].n_detail_attr_id = partner->data->attr_cur.n_detail_attr;
			partner_cur_attr[partner_num].detail_attr_cur = partner->data->attr_cur.detail_attr_vaual;
			partner_cur_attr[partner_num].n_detail_attr_cur = partner->data->attr_cur.n_detail_attr;
			partner_cur_attr[partner_num].type = partner->data->attr_cur.type;
		}
		
		if (partner->data->attr_flash.base_attr_up[0] != 0)
		{
			partner_data[partner_num].flash_attr = partner_cur_flash + partner_num;
			partner_attr__init(partner_cur_flash + partner_num);
			skill_num = 0;
			for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
			{
				partner_skill_point_flash[partner_num][skill_num] = &partner_skill_data_flash[partner_num][skill_num];
				partner_skill_data__init(&partner_skill_data_flash[partner_num][skill_num]);
				partner_skill_data_flash[partner_num][skill_num].id = partner->data->attr_flash.skill_list[i].skill_id;
				partner_skill_data_flash[partner_num][skill_num].lv = partner->data->attr_flash.skill_list[i].lv;
				partner_skill_data_flash[partner_num][skill_num].exp = partner->data->attr_flash.skill_list[i].exp;
				skill_num++;
			}
			partner_cur_flash[partner_num].skills = partner_skill_point_flash[partner_num];
			partner_cur_flash[partner_num].n_skills = skill_num;
			partner_cur_flash[partner_num].base_attr_id = (uint32_t *)base_attr_id;
			partner_cur_flash[partner_num].n_base_attr_id = MAX_PARTNER_BASE_ATTR;
			partner_cur_flash[partner_num].base_attr_cur = partner->data->attr_flash.base_attr_vaual;
			partner_cur_flash[partner_num].n_base_attr_cur = MAX_PARTNER_BASE_ATTR;
			partner_cur_flash[partner_num].base_attr_up = partner->data->attr_flash.base_attr_up;
			partner_cur_flash[partner_num].n_base_attr_up = MAX_PARTNER_BASE_ATTR;
			partner_cur_flash[partner_num].base_attr_up = partner->data->attr_flash.base_attr_up;
			partner_cur_flash[partner_num].n_base_attr_up = MAX_PARTNER_BASE_ATTR;
			partner_cur_flash[partner_num].detail_attr_id = partner->data->attr_flash.detail_attr_id;
			partner_cur_flash[partner_num].n_detail_attr_id = partner->data->attr_flash.n_detail_attr;
			partner_cur_flash[partner_num].detail_attr_cur = partner->data->attr_flash.detail_attr_vaual;
			partner_cur_flash[partner_num].n_detail_attr_cur = partner->data->attr_flash.n_detail_attr;
			partner_cur_flash[partner_num].type = partner->data->attr_flash.type;
			partner_data[partner_num].power = partner->data->attr_flash.power_refresh;
		}

		partner_data[partner_num].god_id = partner->data->god_id;
		partner_data[partner_num].n_god_id = partner->data->n_god;
		partner_data[partner_num].god_level = partner->data->god_level;
		partner_data[partner_num].n_god_level = partner->data->n_god;

		if(partner->data->cur_fabao.fabao_id != 0)
		{
			partner_data[partner_num].cur_fabao = &partner_cur_fabao[partner_num];
			partner_cur_fabao_info__init(&partner_cur_fabao[partner_num]);
			partner_cur_fabao[partner_num].fabao_id = partner->data->cur_fabao.fabao_id;
			partner_cur_fabao[partner_num].main_attr = &partner_fabao_main_attr[partner_num];
			attr_data__init(&partner_fabao_main_attr[partner_num]);
			partner_fabao_main_attr[partner_num].id = partner->data->cur_fabao.main_attr.id;
			partner_fabao_main_attr[partner_num].val = partner->data->cur_fabao.main_attr.val;
			uint32_t attr_num = 0;
			for (int j = 0; j < MAX_HUOBAN_FABAO_MINOR_ATTR_NUM; ++j)
			{

				partner_fabao_minor_attr_point[partner_num][attr_num] = &partner_fabao_minor_attr[partner_num][attr_num];
				attr_data__init(&partner_fabao_minor_attr[partner_num][attr_num]);
				partner_fabao_minor_attr[partner_num][attr_num].id = partner->data->cur_fabao.minor_attr[j].id;
				partner_fabao_minor_attr[partner_num][attr_num].val = partner->data->cur_fabao.minor_attr[j].val;
				attr_num++;
			}

			partner_cur_fabao[partner_num].minor_attr = partner_fabao_minor_attr_point[partner_num];
			partner_cur_fabao[partner_num].n_minor_attr = attr_num;
		}
		partner_num++;
	}

	detail_data.partners = partner_point;
	detail_data.n_partners = partner_num;
	
	// 布阵信息
	//AC 魅力等级
	//AC  总魅力
	detail_data.charmlevel = target->data->charm_level;
	detail_data.charmtotal = target->data->charm_total;

	//时装列表
	FashionData fashion[MAX_FASHION_NUM];
	FashionData *fashionPoint[MAX_FASHION_NUM];
	FashionList sendFashion;
	fashion_list__init(&sendFashion);

	int fashion_num = 0;
	for (uint32_t idx = 0; idx < target->data->n_fashion; ++idx)
	{
		if (0!=target->data->fashion[idx].timeout)	
			if(target->data->fashion[idx].timeout <= (time_t)time_helper::get_cached_time()/1000)
				continue;
		pack_fashion_info(fashion[fashion_num], target, idx);
		fashionPoint[fashion_num] =  &fashion[fashion_num];
		++fashion_num;
	}
	sendFashion.n_data = fashion_num;
	sendFashion.data = fashionPoint;
	sendFashion.level = target->data->charm_level;
	sendFashion.charm = target->data->charm_total;

	detail_data.fashions = &sendFashion;

	//布阵信息
	detail_data.embattleinfo = target->data->partner_formation;
	detail_data.n_embattleinfo = MAX_PARTNER_FORMATION_NUM;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GET_OTHER_INFO_ANSWER, get_other_info_answer__pack, resp);
}

//查看他人个人信息请求
static int handle_get_other_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	GetOtherInfoRequest *req = get_other_info_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint64_t target_id = req->playerid;

	get_other_info_request__free_unpacked(req, NULL);

	player_struct *target = player_manager::get_player_by_id(target_id);
	if (target)
	{
		answer_get_other_info(extern_data, 0, target, 1);
	}
	else
	{
		conn_node_gamesrv::connecter.transfer_to_dbsrv();
	}

	return 0;
}

//推荐好友请求
static int handle_friend_recommend_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PROTO_FRIEND_RECOMMEND *proto = (PROTO_FRIEND_RECOMMEND*)get_send_buf(SERVER_PROTO_FRIEND_RECOMMEND, 0);
	memset(proto->head.data, 0, sizeof(PROTO_FRIEND_RECOMMEND) - sizeof(PROTO_HEAD));
	proto->head.len = ENDION_FUNC_4(sizeof(PROTO_FRIEND_RECOMMEND));

	std::set<uint64_t> playerIds;
	//同一场景地图玩家
	if (player->scene)
	{
		scene_struct *scene = player->scene;
		scene->get_all_player(playerIds);
	}
	//同帮会在线成员
	if (player->data->guild_id > 0)
	{
		for (std::map<uint64_t, player_struct *>::iterator iter = player_manager_all_players_id.begin(); iter != player_manager_all_players_id.end(); ++iter)
		{
			player_struct *tmp_player = iter->second;
			if (tmp_player->data->guild_id == player->data->guild_id)
			{
				playerIds.insert(iter->first);
			}
		}
	}

	for (std::set<uint64_t>::iterator iter = playerIds.begin(); iter != playerIds.end() && proto->player_num < MAX_FRIEND_RECOMMEND_PLAYER; ++iter)
	{
		//剔除自己
		if (*iter == extern_data->player_id)
		{
			continue;
		}
		proto->player_id[proto->player_num] = *iter;
		proto->player_num++;
	}

	add_extern_data(&proto->head, extern_data);

	if (conn_node_gamesrv::connecter.send_one_msg(&proto->head, 1) != (int)ENDION_FUNC_4(proto->head.len))
	{
		LOG_ERR("[%s:%d] send to client failed err[%d]", __FUNCTION__, __LINE__, errno);
	}

	return 0;
}

void answer_friend_search(EXTERN_DATA *extern_data, int result, player_struct *target, uint32_t logout_time)
{
	FriendSearchAnswer resp;
	friend_search_answer__init(&resp);

	FriendPlayerBriefData friend_data;
	friend_player_brief_data__init(&friend_data);

	resp.result = result;
	if (result == 0 && target != NULL)
	{
		resp.data = &friend_data;
		friend_data.playerid = target->data->player_id;
		friend_data.name = target->data->name;
		friend_data.job = target->get_job();
		friend_data.level = target->get_level();
		friend_data.head = target->get_attr(PLAYER_ATTR_HEAD);
		friend_data.offlinetime = logout_time;
		friend_data.closeness = 0;
		friend_data.tags = target->data->personality_tags;
		friend_data.n_tags = 0;
		for (int i = 0; i < MAX_PERSONALITY_TAG_NUM; ++i)
		{
			if (target->data->personality_tags[i] == 0)
			{
				break;
			}
			friend_data.n_tags++;
		}
		friend_data.textintro = target->data->personality_text_intro;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_FRIEND_SEARCH_ANSWER, friend_search_answer__pack, resp);
}

//搜索好友请求
static int handle_friend_search_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	FriendSearchRequest *req = friend_search_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	int ret = 0;
	do
	{
		if (req->playerid == player->get_uuid() || strcmp(req->playername, player->data->name) == 0)
		{
			ret = 190500220;
			break;
		}

		bool bFind = false;
		if (req->playerid > 0)
		{
			player_struct *target = player_manager::get_player_by_id(req->playerid);
			if (target)
			{
				answer_friend_search(extern_data, 0, target, 0);
				bFind = true;
			}
		}

		if (!bFind)
		{
			conn_node_gamesrv::connecter.transfer_to_dbsrv();
		}
	} while(0);

	if (ret != 0)
	{
		answer_friend_search(extern_data, ret, NULL, 0);
	}

	friend_search_request__free_unpacked(req, NULL);

	return 0;
}

//扩展好友上限应答
static int handle_friend_extend_contact_answer(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BagUseRequest *req = bag_use_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t pos = req->index;
	uint32_t use_all = req->use_all;

	bag_use_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		if (!(pos < player->data->bag_grid_num))
		{
			ret = ERROR_ID_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] bag pos error, pos:%u, grid_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, player->data->bag_grid_num);
			break;
		}

		bag_grid_data& grid = player->data->bag[pos];
		player->add_task_progress(TCT_USE_PROP, grid.id, 1);
		uint32_t del_num = (use_all == 0 ? 1 : grid.num);
		player->del_item_by_pos(pos, del_num, MAGIC_TYPE_BAG_USE);
	} while(0);

	BagUseAnswer resp;
	bag_use_answer__init(&resp);
	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_BAG_USE_ANSWER, bag_use_answer__pack, resp);

	return 0;
}

//好友服请求扣除消耗
static int handle_friendsrv_check_and_cost_request(player_struct *player, EXTERN_DATA *extern_data)
{
	return handle_srv_check_and_cost_request(player, extern_data, SERVER_PROTO_FRIENDSRV_COST_ANSWER);
}

static int handle_xunbao_pos_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("%s: player[%lu] set", __FUNCTION__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	BagUseRequest *req = bag_use_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint32_t pos = req->index;
	bag_use_request__free_unpacked(req, NULL);

	if (!(pos < player->data->bag_grid_num))
	{
		LOG_ERR("[%s:%d] player[%lu] bag pos error, pos:%u, grid_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, player->data->bag_grid_num);
		return -2;
	}
	if (player->data->bag[pos].num < 1)
	{
		return -3;
	}
	if (player->sight_space != NULL)
	{
		return -8;
	}

	ItemsConfigTable *prop_config = get_config_by_id(player->data->bag[pos].id, &item_config);
	if (prop_config == NULL)
	{
		return -5;
	}
	if (player->get_attr(PLAYER_ATTR_LEVEL) < prop_config->ItemLevel)
	{
		return -6;
	}

	SearchTable *sTable = get_config_by_id(player->data->bag[pos].id, &sg_xunbao);
	if (sTable == NULL)
	{
		return -4;
	}

	uint32_t empty_num = 0;
	for (uint32_t i = 0; i < player->data->bag_grid_num; ++i)
	{
		bag_grid_data& grid = player->data->bag[i];
		if (grid.id == 0)
		{
			empty_num++;
		}
	}
	if (empty_num < 3)
	{
		player->send_system_notice(190500284, NULL);
	}

	uint32_t mapId = 0;
	uint32_t x = 0;
	uint32_t z = 0;
	TreasureTable *tTable = NULL;
	std::map<uint32_t, uint32_t>::iterator itItem = player->xun_map_id.find(player->data->bag[pos].id);
	if (itItem == player->xun_map_id.end())
	{
		mapId = sTable->TreasureId[rand() % sTable->n_TreasureId];
		//uint32_t mapId = sTable->TreasureId[0];
		std::map<uint64_t, std::vector<uint64_t> >::iterator it = sg_xunbao_map.find(mapId);
		if (it == sg_xunbao_map.end())
		{
			return -5;
		}
		tTable = get_config_by_id(it->second.at(rand() % it->second.size()), &xunbao_map_config);
		if (tTable == NULL)
		{
			return -7;
		}
		player->xun_map_id[player->data->bag[pos].id] = tTable->ID;
	}
	else
	{
		tTable = get_config_by_id(itItem->second, &xunbao_map_config);
		if (tTable == NULL)
		{
			return -7;
		}
	}

	x = tTable->PointX / 100;
	z = tTable->PointZ / 100;
	mapId = tTable->MapId;

	XunbaoPos send;
	xunbao_pos__init(&send);
	send.mapid = mapId;
	send.x = x;
	send.z = z;
	send.transfer = tTable->TransferPoint;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_XUNBAO_POS_ANSWER, xunbao_pos__pack, send);

	return 0;
}

static int handle_auto_add_hp_set_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("%s: player[%lu] set", __FUNCTION__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	AutoAddHpSetData *data = auto_add_hp_set_data__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!data)
	{
		LOG_ERR("%s: player[%lu] can not unpack", __FUNCTION__, extern_data->player_id);
		return (-10);
	}

	player->data->auto_add_hp_item_id = data->auto_add_hp_item_id;
	player->data->auto_add_hp_percent = data->auto_add_hp_percent;
	player->data->open_auto_add_hp = data->open_auto_add_hp;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_AUTO_ADD_HP_SET_ANSWER, auto_add_hp_set_data__pack, *data);

	auto_add_hp_set_data__free_unpacked(data, NULL);
	return (0);
}

//好友服送礼消耗请求
static int handle_friend_gift_cost_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PROTO_COST_FRIEND_GIFT_REQ *req = (PROTO_COST_FRIEND_GIFT_REQ*)buf_head();
	uint32_t item_id = req->item_id;
	uint32_t item_num = req->gift_num;

	SRV_COST_INFO cost;
	memset(&cost, 0, sizeof(SRV_COST_INFO));
	int ret = 0;
	uint32_t add_closeness = 0;
	uint32_t send_item_id = 0;
	do
	{
		ItemsConfigTable *config = get_config_by_id(item_id, &item_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get itemconfig failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id);
			break;
		}

		bool can_use_bind = false;
		if (config->ItemEffect == 9) //好感度道具，可以使用绑定道具
		{
			can_use_bind = true;
		}
		else //其他道具，只能使用非绑定道具
		{
			can_use_bind = false;
		}

		uint32_t item_bind_id = 0, item_unbind_id = 0;
		if (config->BindType == 0) //道具本身是非绑定道具
		{
			item_bind_id = config->ItemRelation;
			item_unbind_id = item_id;
		}
		else //道具是绑定道具
		{
			item_bind_id = item_id;
			item_unbind_id = config->ItemRelation;
		}

		uint32_t item_bind_num = player->get_item_num_by_id(item_bind_id);
		uint32_t item_unbind_num = player->get_item_num_by_id(item_unbind_id);
		uint32_t has_num = (can_use_bind ? item_bind_num + item_unbind_num : item_unbind_num);
		if (has_num < item_num)
		{
			uint32_t replace_num = item_num - has_num;
			uint32_t need_money = replace_num * req->currency_val;
			uint32_t has_money = 0;
			uint32_t error_id = 0;
			switch(req->currency_type)
			{
				case FRIENF_SONGLI_SUB_COIN: //银票
					has_money = player->get_coin();
					error_id = ERROR_ID_COIN_NOT_ENOUGH;
					break;
				case FRIENF_SONGLI_SUB_GOLD: //金票(金票不足可以消耗元宝)
					has_money = player->get_attr(PLAYER_ATTR_BIND_GOLD) + player->get_attr(PLAYER_ATTR_GOLD);
					error_id = ERROR_ID_GOLD_NOT_ENOUGH;
					break;
				case FRIENF_SONGLI_SUB_ACER: //元宝
					has_money = player->get_attr(PLAYER_ATTR_GOLD);
					error_id = ERROR_ID_GOLD_NOT_ENOUGH;
					break;
			}

			if (has_money < need_money)
			{
				if(error_id == 0)
				{
					LOG_ERR("[%s:%d] 好友送礼配置错误", __FUNCTION__, __LINE__);
					return -2;
				}
				ret = error_id;
				break;
			}

			if (can_use_bind)
			{

				if (item_bind_num >= has_num)
				{
					player->del_item_by_id(item_bind_id, has_num, MAGIC_TYPE_FRIEND_GIFT);
				}
				else 
				{
					player->del_item_by_id(item_bind_id, item_bind_num, MAGIC_TYPE_FRIEND_GIFT); 
					uint32_t use_unbin_num = has_num - item_bind_num;
					player->del_item_by_id(item_unbind_id, use_unbin_num, MAGIC_TYPE_FRIEND_GIFT);
				}
			}
			else 
			{
				player->del_item_by_id(item_unbind_id, item_unbind_num, MAGIC_TYPE_FRIEND_GIFT);
			
			}

			switch(req->currency_type)
			{
				case FRIENF_SONGLI_SUB_COIN: //消耗银票
					player->sub_coin(need_money, MAGIC_TYPE_FRIEND_GIFT);
					cost.coin = need_money;
					break;
				case FRIENF_SONGLI_SUB_GOLD: //消耗金票(金票不足可以元宝代替)
					{
						uint32_t has_bind_gold = player->get_attr(PLAYER_ATTR_BIND_GOLD);
						uint32_t has_unbind_gold = player->get_attr(PLAYER_ATTR_GOLD);
						player->sub_comm_gold(need_money, MAGIC_TYPE_FRIEND_GIFT);
						cost.gold = has_bind_gold - player->get_attr(PLAYER_ATTR_BIND_GOLD);
						cost.unbind_gold = has_unbind_gold - player->get_attr(PLAYER_ATTR_GOLD);
					}
					break;
				case FRIENF_SONGLI_SUB_ACER: //消耗元宝
					player->sub_unbind_gold(need_money, MAGIC_TYPE_FRIEND_GIFT);
					cost.unbind_gold = need_money;
					break;
			}
		}
		else
		{
			if (can_use_bind)
			{

				if (item_bind_num >= item_num)
				{
					player->del_item_by_id(item_bind_id, item_num, MAGIC_TYPE_FRIEND_GIFT);
				}
				else 
				{
					player->del_item_by_id(item_bind_id, item_bind_num, MAGIC_TYPE_FRIEND_GIFT); 
					uint32_t use_unbin_num = item_num - item_bind_num;
					player->del_item_by_id(item_unbind_id, use_unbin_num, MAGIC_TYPE_FRIEND_GIFT);
				}
			}
			else 
			{
				player->del_item_by_id(item_unbind_id, item_num, MAGIC_TYPE_FRIEND_GIFT);
			
			}
			/*if (can_use_bind)
			{
				player->del_item(item_id, item_num, MAGIC_TYPE_FRIEND_GIFT);
			}
			else
			{
				player->del_item_by_id(item_unbind_id, item_num, MAGIC_TYPE_FRIEND_GIFT);
			}*/
		}

		uint32_t cost_bind_num = item_bind_num - player->get_item_num_by_id(item_bind_id);
		uint32_t cost_unbind_num = item_unbind_num - player->get_item_num_by_id(item_unbind_id);
		uint32_t item_idx = 0;
		if (cost_bind_num > 0)
		{
			cost.item_id[item_idx] = item_bind_id;
			cost.item_num[item_idx] = cost_bind_num;
			item_idx++;
		}
		if (cost_unbind_num > 0)
		{
			cost.item_id[item_idx] = item_unbind_id;
			cost.item_num[item_idx] = cost_unbind_num;
			item_idx++;
		}

		if (config->ItemEffect == 9)
		{
			if (config->n_ParameterEffect > 0)
			{
				add_closeness = config->ParameterEffect[0] * item_num;
			}
		}
		else
		{
			send_item_id = item_unbind_id;
		}

		cost.statis_id = MAGIC_TYPE_FRIEND_GIFT;
	} while(0);

	PROTO_COST_FRIEND_GIFT_RES *res = (PROTO_COST_FRIEND_GIFT_RES *)get_send_buf(SERVER_PROTO_FRIEND_GIFT_COST_ANSWER, get_seq());
	res->head.len = ENDION_FUNC_4(sizeof(PROTO_COST_FRIEND_GIFT_RES));
	memset(res->head.data, 0, sizeof(PROTO_COST_FRIEND_GIFT_RES) - sizeof(PROTO_HEAD));
	res->result = ret;
	res->target_id = req->target_id;
	res->gift_id = req->gift_id;
	res->gift_num = req->gift_num;
	res->item_id = send_item_id;
	res->add_closeness = add_closeness;
	memcpy(&res->cost, &cost, sizeof(SRV_COST_INFO));
	add_extern_data(&res->head, extern_data);
	if (conn_node_gamesrv::connecter.send_one_msg(&res->head, 1) != (int)ENDION_FUNC_4(res->head.len))
	{
		LOG_ERR("[%s:%d] send to friendsrv failed err[%d]", __FUNCTION__, __LINE__, errno);
	}

	return 0;
}

//好友服接收礼物
static int handle_friend_add_gift_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	FriendGiftData *req = friend_gift_data__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	std::map<uint32_t, uint32_t> item_map;
	item_map[req->item->id] = req->item->num;
	std::vector<char *> args;
	args.push_back(req->playername);
	player->send_mail_by_id(270100011, &args, &item_map, MAGIC_TYPE_FRIEND_RECEIVE_GIFT);
	//player->add_item_list_otherwise_send_mail(item_map, MAGIC_TYPE_FRIEND_RECEIVE_GIFT, 270300031, &args);

	friend_gift_data__free_unpacked(req, NULL);

	return 0;
}

static int handle_doufachang_challenge(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	DOUFACHANG_CHALLENGE_REQUEST *req = (DOUFACHANG_CHALLENGE_REQUEST *)get_data();
	uint64_t target_id = req->defence;

	LOG_INFO("[%s:%d] player[%lu] challenge[%lu]", __FUNCTION__, __LINE__, extern_data->player_id, target_id);

	player->check_activity_progress(AM_RAID, sg_doufachang_raid_id);
	player_struct *target = player_manager::get_player_by_id(target_id);
	if (!target)
	{
			// 让DB srv加载玩家数据
		DOUFACHANG_LOAD_PLAYER_REQUEST *req = (DOUFACHANG_LOAD_PLAYER_REQUEST *)conn_node_dbsrv::connecter->get_send_data();
		req->player_id = player->get_uuid();
		req->target_id = target_id;

		EXTERN_DATA extern_data;
		extern_data.player_id = req->player_id;
		fast_send_msg_base(conn_node_dbsrv::connecter, &extern_data, SERVER_PROTO_DOUFACHANG_LOAD_PLAYER_REQUEST, sizeof(*req), 0);
		return 0;
	}

	raid_struct *raid = raid_manager::create_raid(sg_doufachang_raid_id, NULL);
	if (!raid)
	{
		LOG_ERR("%s: create raid failed", __FUNCTION__);
		return -1;
	}

	raid->player_enter_raid_impl(player, 0, sg_3v3_pvp_raid_param1[1], sg_3v3_pvp_raid_param1[3]);
	player_struct *ai_player = player_manager::create_doufachang_ai_player(target);
	assert(ai_player);
	raid->player_enter_raid_impl(ai_player, MAX_TEAM_MEM, sg_3v3_pvp_raid_param2[1], sg_3v3_pvp_raid_param2[3]);

	player->send_player_enter_to_aisrv();
	ai_player->send_player_enter_to_aisrv();
	ai_player->send_player_sight_add_to_aisrv(player);
	return 0;
}

static int handle_doufachang_buy_challenge(player_struct *player, EXTERN_DATA *extern_data)
{
	DOUFACHANG_BUY_CHALLENGE_REQUEST *req = (DOUFACHANG_BUY_CHALLENGE_REQUEST *)buf_head();
	int ret = player->sub_comm_gold(req->gold_num, MAGIC_TYPE_DOUFACHANG_BUY_CHALLENGE_COUNT, true);

	DOUFACHANG_BUY_CHALLENGE_ANSWER *ans = (DOUFACHANG_BUY_CHALLENGE_ANSWER*)get_send_buf(SERVER_PROTO_DOUFACHANG_BUY_CHALLENGE_ANSWER, 0);
	ans->head.len = ENDION_FUNC_4(sizeof(DOUFACHANG_BUY_CHALLENGE_ANSWER));
	ans->result = ret;
	ans->count = req->count;
	ans->gold_num = req->gold_num;
	conn_node_base::add_extern_data(&ans->head, extern_data);
	if (conn_node_gamesrv::connecter.send_one_msg(&ans->head, 1) != (int)ENDION_FUNC_4(ans->head.len))
	{
		LOG_ERR("[%s:%d] send to doufachang failed err[%d]", __FUNCTION__, __LINE__, errno);
	}
	
	return (0);
}

static int handle_doufachang_sync_rank(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	DOUFACHANG_SYNC_RANK *req = (DOUFACHANG_SYNC_RANK *)get_data();
	player->data->doufachang_rank = req->my_rank;
	player->add_achievement_progress(ACType_DOUFACHANG_RANK, player->data->doufachang_rank, 0, 0, 1);
	if (req->defencer_id > 0)
	{
		player_struct *defencer = player_manager::get_player_by_id(req->defencer_id);
		if (defencer)
		{
			defencer->data->doufachang_rank = req->defencer_rank;
			defencer->add_achievement_progress(ACType_DOUFACHANG_RANK, defencer->data->doufachang_rank, 0, 0, 1);
		}
	}

	return 0;
}

static int handle_doufachang_sync_buy_challenge(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	uint32_t *req = (uint32_t *)get_data();
	uint32_t count = *req;
	player->add_achievement_progress(ACType_DOUFACHANG_BUY_CHALLENGE, 0, 0, 0, count);

	return 0;
}

static int handle_doufachang_reward(player_struct *player, EXTERN_DATA *extern_data)
{
	DOUFACHANG_GET_REWARD_REQUEST *req = (DOUFACHANG_GET_REWARD_REQUEST *)get_data();
	LOG_INFO("[%s:%d] player[%lu] rewardid[%u]", __FUNCTION__, __LINE__, extern_data->player_id, req->reward_id);

	struct ArenaRewardTable *config = get_config_by_id(req->reward_id, &doufachang_reward_config);
	if (!config)
	{
		LOG_ERR("%s: player[%lu] reward config fail", __FUNCTION__, extern_data->player_id);
		return (0);
	}
	std::map<uint32_t, uint32_t> item_list;
	for (size_t i = 0; i < config->n_ItemID; ++i)
	{
		item_list[config->ItemID[i]] = config->Num[i];
	}
	
	int result = player->add_item_list(item_list, MAGIC_TYPE_DOUFACHANG_REWARD);
	DOUFACHANG_GET_REWARD_ANSWER *ans = (DOUFACHANG_GET_REWARD_ANSWER *)get_send_data();
	ans->result = result;
	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_DOUFACHANG_ADD_REWARD_ANSWER, sizeof(*ans), 0);
	return (0);
}

//好友服同步好友数
static int handle_friend_sync_friend_num(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PROTO_FRIEND_SYNC_INFO *head = (PROTO_FRIEND_SYNC_INFO*)buf_head();
	memcpy(player->data->friend_contacts, head->contacts, sizeof(player->data->friend_contacts));

	uint32_t friend_num = player->get_friend_num();
	player->add_task_progress(TCT_FRIEND_NUM, 0, friend_num);
	player->add_achievement_progress(ACType_FRIEND_NUM, 0, 0, 0, friend_num);
	player->add_achievement_progress(ACType_FRIEND_CLOSE_NUM, 0, 0, 0, 0);

	return 0;
}

static int handle_friend_send_gift_success(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	player->add_achievement_progress(ACType_FRIEND_SEND_GIFT, 0, 0, 0, 1);
	return 0;
}

//好友服追踪仇人请求
static int handle_friend_track_enemy_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	FRIEND_TRACK_REQUEST *req = (FRIEND_TRACK_REQUEST*)get_data();
	uint64_t target_id = req->target_id;
	uint32_t item_id = req->cost_item_id;
	uint32_t item_num = req->cost_item_num;

	int ret = 0;
	do
	{
		uint32_t has_num = player->get_item_can_use_num(item_id);
		if (has_num < item_num)
		{
			ret = 190500518;
			break;
		}

		player_struct *target = player_manager::get_player_by_id(target_id);
		if (!target || !target->is_online())
		{
			ret = 190500522;
			break;
		}
		if(target->scene->get_scene_type() != SCENE_TYPE_WILD || target->sight_space)
		{
			ret = 190500520;
			break;
		}

		/*if (target->in_watched_list(extern_data->player_id))
		{
			ret = 190500156;
			break;
		}*/

		player->del_item(item_id, item_num, MAGIC_TYPE_FRIEND_TRACK_ENEMY);
		//target->add_watched_list(extern_data->player_id);

		SpecialPlayerPosNotify nty;
		special_player_pos_notify__init(&nty);
		struct position *pos = target->get_pos();
		nty.scene_id = target->data->scene_id;
		nty.uuid = target->data->player_id;
		nty.pos_x = pos->pos_x;
		nty.pos_z = pos->pos_z;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SPECIAL_PLAYER_POS_NOTIFY, special_player_pos_notify__pack, nty);

		for (int i = 0; i < MAX_FRIEND_ENEMY_NUM; ++i)
		{
			if (player->data->friend_enemies[i].player_id == target_id)
			{
				player->data->friend_enemies[i].track_time = req->track_time;
				break;
			}
		}
	} while(0);

	FRIEND_TRACK_ANSWER *res = (FRIEND_TRACK_ANSWER *)get_send_data();
	uint32_t data_len = sizeof(FRIEND_TRACK_ANSWER);
	memset(res, 0, data_len);
	res->result = ret;
	res->target_id = req->target_id;
	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_FRIEND_TRACK_ENEMY_ANSWER, data_len, 0);

	return 0;
}

//好友服同步仇人信息
static int handle_friend_sync_enemy(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PROTO_FRIEND_SYNC_ENEMY *head = (PROTO_FRIEND_SYNC_ENEMY*)get_data();
	memcpy(player->data->friend_enemies, head->enemies, sizeof(player->data->friend_enemies));

	uint32_t now = time_helper::get_cached_time() / 1000;
	for (int i = 0; i < MAX_FRIEND_ENEMY_NUM; ++i)
	{
		if (player->data->friend_enemies[i].player_id == 0)
		{
			break;
		}

		if (player->data->friend_enemies[i].track_time <= now)
		{

		}
		else
		{
			player_struct *target = player_manager::get_player_by_id(player->data->friend_enemies[i].player_id);
			if (target && !target->in_watched_list(extern_data->player_id))
			{
				target->add_watched_list(extern_data->player_id);

				SpecialPlayerPosNotify nty;
				special_player_pos_notify__init(&nty);
				struct position *pos = target->get_pos();
				nty.scene_id = target->data->scene_id;
				nty.uuid = target->data->player_id;
				nty.pos_x = pos->pos_x;
				nty.pos_z = pos->pos_z;
				fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SPECIAL_PLAYER_POS_NOTIFY, special_player_pos_notify__pack, nty);
			}
		}
	}

	return 0;
}

//伙伴信息请求
static int handle_partner_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	notify_partner_info(player, extern_data);
	return 0;
}

static int notify_partner_info(player_struct *player, EXTERN_DATA *extern_data)
{
	PartnerInfoAnswer resp;
	partner_info_answer__init(&resp);

	PartnerData  partner_data[MAX_PARTNER_NUM];
	PartnerAttr partner_cur_attr[MAX_PARTNER_NUM];
	PartnerAttr partner_cur_flash[MAX_PARTNER_NUM];
	PartnerData* partner_point[MAX_PARTNER_NUM];
	AttrData  attr_data[MAX_PARTNER_NUM][MAX_PARTNER_ATTR];
	AttrData* attr_point[MAX_PARTNER_NUM][MAX_PARTNER_ATTR];
	PartnerSkillData  skill_data[MAX_PARTNER_NUM][MAX_PARTNER_SKILL_NUM];
	PartnerSkillData* skill_point[MAX_PARTNER_NUM][MAX_PARTNER_SKILL_NUM];
	PartnerSkillData  partner_skill_data_flash[MAX_PARTNER_NUM][MAX_PARTNER_SKILL_NUM];
	PartnerSkillData* partner_skill_point_flash[MAX_PARTNER_NUM][MAX_PARTNER_SKILL_NUM];
	PartnerCurFabaoInfo partner_cur_fabao[MAX_PARTNER_NUM];
	AttrData partner_fabao_minor_attr[MAX_PARTNER_NUM][MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData* partner_fabao_minor_attr_point[MAX_PARTNER_NUM][MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData partner_fabao_main_attr[MAX_PARTNER_NUM];

	resp.result = 0;

	resp.cd = player->data->partner_add_angry_cd;
	uint32_t partner_num = 0;
	for (PartnerMap::iterator iter = player->m_partners.begin(); iter != player->m_partners.end() && partner_num < MAX_PARTNER_NUM; ++iter)
	{
		partner_struct *partner = iter->second;
		if (partner == NULL)
		{
			continue;
		}

		partner_point[partner_num] = &partner_data[partner_num];
		partner_data__init(&partner_data[partner_num]);
		partner_data[partner_num].uuid = partner->data->uuid;
		partner_data[partner_num].partnerid = partner->data->partner_id;
		partner_data[partner_num].relivetime = partner->data->relive_time;
		partner_data[partner_num].name = partner->data->name;
		partner_data[partner_num].rename_free = partner->data->partner_rename_free;
		
		uint32_t attr_num = 0;
		for (int i = 1; i < MAX_PARTNER_ATTR; ++i)
		{
			attr_point[partner_num][attr_num] = &attr_data[partner_num][attr_num];
			attr_data__init(&attr_data[partner_num][attr_num]);
			attr_data[partner_num][attr_num].id = i;
			attr_data[partner_num][attr_num].val = partner->data->attrData[i];
			attr_num++;
		}
		partner_data[partner_num].attrs = attr_point[partner_num];
		partner_data[partner_num].n_attrs = attr_num;

		uint32_t skill_num = 0;
		if (partner->data->attr_cur.base_attr_up[0] != 0)
		{
			partner_data[partner_num].cur_attr = partner_cur_attr + partner_num;
			partner_attr__init(partner_cur_attr + partner_num);
			for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
			{
				skill_point[partner_num][skill_num] = &skill_data[partner_num][skill_num];
				partner_skill_data__init(&skill_data[partner_num][skill_num]);
				skill_data[partner_num][skill_num].id = partner->data->attr_cur.skill_list[i].skill_id;
				skill_data[partner_num][skill_num].lv = partner->data->attr_cur.skill_list[i].lv;
				skill_data[partner_num][skill_num].lock = partner->data->attr_cur.skill_list[i].lock;
				skill_data[partner_num][skill_num].exp = partner->data->attr_cur.skill_list[i].exp;
				skill_num++;
			}
			partner_cur_attr[partner_num].skills = skill_point[partner_num];
			partner_cur_attr[partner_num].n_skills = skill_num;
			partner_cur_attr[partner_num].base_attr_id = (uint32_t *)&base_attr_id[0];
			partner_cur_attr[partner_num].n_base_attr_id = MAX_PARTNER_BASE_ATTR;
			partner_cur_attr[partner_num].base_attr_cur = partner->data->attr_cur.base_attr_vaual;
			partner_cur_attr[partner_num].n_base_attr_cur = MAX_PARTNER_BASE_ATTR;
			partner_cur_attr[partner_num].base_attr_up = partner->data->attr_cur.base_attr_up;
			partner_cur_attr[partner_num].n_base_attr_up = MAX_PARTNER_BASE_ATTR;
			partner_cur_attr[partner_num].base_attr_up = partner->data->attr_cur.base_attr_up;
			partner_cur_attr[partner_num].n_base_attr_up = MAX_PARTNER_BASE_ATTR;
			partner_cur_attr[partner_num].detail_attr_id = partner->data->attr_cur.detail_attr_id;
			partner_cur_attr[partner_num].n_detail_attr_id = partner->data->attr_cur.n_detail_attr;
			partner_cur_attr[partner_num].detail_attr_cur = partner->data->attr_cur.detail_attr_vaual;
			partner_cur_attr[partner_num].n_detail_attr_cur = partner->data->attr_cur.n_detail_attr;
			partner_cur_attr[partner_num].type = partner->data->attr_cur.type;
		}
		
		if (partner->data->attr_flash.base_attr_up[0] != 0)
		{
			partner_data[partner_num].flash_attr = partner_cur_flash + partner_num;
			partner_attr__init(partner_cur_flash + partner_num);
			skill_num = 0;
			for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
			{
				partner_skill_point_flash[partner_num][skill_num] = &partner_skill_data_flash[partner_num][skill_num];
				partner_skill_data__init(&partner_skill_data_flash[partner_num][skill_num]);
				partner_skill_data_flash[partner_num][skill_num].id = partner->data->attr_flash.skill_list[i].skill_id;
				partner_skill_data_flash[partner_num][skill_num].lv = partner->data->attr_flash.skill_list[i].lv;
				partner_skill_data_flash[partner_num][skill_num].exp = partner->data->attr_flash.skill_list[i].exp;
				skill_num++;
			}
			partner_cur_flash[partner_num].skills = partner_skill_point_flash[partner_num];
			partner_cur_flash[partner_num].n_skills = skill_num;
			partner_cur_flash[partner_num].base_attr_id = (uint32_t *)base_attr_id;
			partner_cur_flash[partner_num].n_base_attr_id = MAX_PARTNER_BASE_ATTR;
			partner_cur_flash[partner_num].base_attr_cur = partner->data->attr_flash.base_attr_vaual;
			partner_cur_flash[partner_num].n_base_attr_cur = MAX_PARTNER_BASE_ATTR;
			partner_cur_flash[partner_num].base_attr_up = partner->data->attr_flash.base_attr_up;
			partner_cur_flash[partner_num].n_base_attr_up = MAX_PARTNER_BASE_ATTR;
			partner_cur_flash[partner_num].base_attr_up = partner->data->attr_flash.base_attr_up;
			partner_cur_flash[partner_num].n_base_attr_up = MAX_PARTNER_BASE_ATTR;
			partner_cur_flash[partner_num].detail_attr_id = partner->data->attr_flash.detail_attr_id;
			partner_cur_flash[partner_num].n_detail_attr_id = partner->data->attr_flash.n_detail_attr;
			partner_cur_flash[partner_num].detail_attr_cur = partner->data->attr_flash.detail_attr_vaual;
			partner_cur_flash[partner_num].n_detail_attr_cur = partner->data->attr_flash.n_detail_attr;
			partner_cur_flash[partner_num].type = partner->data->attr_flash.type;
			partner_data[partner_num].power = partner->data->attr_flash.power_refresh;
		}
		
		partner_data[partner_num].god_id = partner->data->god_id;
		partner_data[partner_num].n_god_id = partner->data->n_god;
		partner_data[partner_num].god_level = partner->data->god_level;
		partner_data[partner_num].n_god_level = partner->data->n_god;

		if(partner->data->cur_fabao.fabao_id != 0)
		{
			partner_data[partner_num].cur_fabao = &partner_cur_fabao[partner_num];
			partner_cur_fabao_info__init(&partner_cur_fabao[partner_num]);
			partner_cur_fabao[partner_num].fabao_id = partner->data->cur_fabao.fabao_id;
			partner_cur_fabao[partner_num].main_attr = &partner_fabao_main_attr[partner_num];
			attr_data__init(&partner_fabao_main_attr[partner_num]);
			partner_fabao_main_attr[partner_num].id = partner->data->cur_fabao.main_attr.id;
			partner_fabao_main_attr[partner_num].val = partner->data->cur_fabao.main_attr.val;
			uint32_t attr_num = 0;
			for (int j = 0; j < MAX_HUOBAN_FABAO_MINOR_ATTR_NUM; ++j)
			{

				partner_fabao_minor_attr_point[partner_num][attr_num] = &partner_fabao_minor_attr[partner_num][attr_num];
				attr_data__init(&partner_fabao_minor_attr[partner_num][attr_num]);
				partner_fabao_minor_attr[partner_num][attr_num].id = partner->data->cur_fabao.minor_attr[j].id;
				partner_fabao_minor_attr[partner_num][attr_num].val = partner->data->cur_fabao.minor_attr[j].val;
				attr_num++;
			}
			partner_cur_fabao[partner_num].minor_attr = partner_fabao_minor_attr_point[partner_num];
			partner_cur_fabao[partner_num].n_minor_attr = attr_num;
		

		}

		partner_num++;
	}
	resp.partner_today_junior_recurit_cd = player->data->partner_today_junior_recurit_cd;
	resp.partner_today_junior_recurit_count = player->data->partner_today_junior_recurit_count;
	resp.partner_today_senior_recurit_cd = player->data->partner_today_senior_recurit_cd;
	resp.partner_today_senior_recurit_count = player->data->partner_today_senior_recurit_count;	
	
	resp.partners = partner_point;
	resp.n_partners = partner_num;

	resp.formation = player->data->partner_formation;
	resp.n_formation = MAX_PARTNER_FORMATION_NUM;

	resp.dictionary = player->data->partner_dictionary;
	resp.n_dictionary = 0;
	for (int i = 0; i < MAX_PARTNER_TYPE; ++i)
	{
		if (player->data->partner_dictionary[i] == 0)
		{
			break;
		}
		resp.n_dictionary++;
	}

	resp.recruitjuniorfreecd = player->data->partner_recruit_junior_free_cd;
	resp.recruitjuniorcount = player->data->partner_recruit_junior_count;
	resp.recruitseniorfreecount = player->data->partner_recruit_senior_free_count;
	resp.recruitseniorcount = player->data->partner_recruit_senior_count;

	resp.bonds = player->data->partner_bond;
	resp.n_bonds = 0;
	for (int i = 0; i < MAX_PARTNER_BOND_NUM; ++i)
	{
		if (player->data->partner_bond[i] == 0)
		{
			break;
		}
		resp.n_bonds++;
	}

	resp.bondreward = player->data->partner_bond_reward;
	resp.n_bondreward = 0;
	for (int i = 0; i < MAX_PARTNER_TYPE; ++i)
	{
		if (player->data->partner_bond_reward[i] == 0)
		{
			break;
		}
		resp.n_bondreward++;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_INFO_ANSWER, partner_info_answer__pack, resp);
	return 0;
}

//伙伴翻转开关请求 
static int handle_partner_turn_switch_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerTurnSwitchRequest *req = partner_turn_switch_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;

	partner_turn_switch_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		if (!(type == PLAYER_ATTR_PARTNER_FIGHT || type == PLAYER_ATTR_PARTNER_PRECEDENCE))
		{
			ret = -1;
			LOG_ERR("[%s:%d] player[%lu] set type %u failed.", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		uint32_t cur_val = player->get_attr(type);
		if (cur_val == 0)
		{
			player->set_attr(type, 1);
		}
		else
		{
			player->set_attr(type, 0);
		}

		player->notify_one_attr_changed(type, player->get_attr(type));
		player->adjust_battle_partner();
		if (type == PLAYER_ATTR_PARTNER_FIGHT)
		{
			player->calculate_attribute(true);
		}
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_TURN_SWITCH_ANSWER, comm_answer__pack, resp);

	return 0;
}

//伙伴布阵请求
static int handle_partner_formation_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerFormationRequest *req = partner_formation_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint64_t partner_uuid = req->uuid;
	uint32_t pos = req->position;

	partner_formation_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
		if (!partner)
		{
			ret = ERROR_ID_PARTNER_UUID;
			LOG_ERR("[%s:%d] player[%lu] can't find, partner:%lu, pos:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, pos);
			break;
		}

		if (!(pos <= MAX_PARTNER_FORMATION_NUM))
		{
			ret = ERROR_ID_PARTNER_POS;
			LOG_ERR("[%s:%d] player[%lu] pos error, partner:%lu, pos:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, pos);
			break;
		}

		int old_pos = -1;
		for (int i = 0; i < MAX_PARTNER_FORMATION_NUM; ++i)
		{
			if (player->data->partner_formation[i] == partner_uuid)
			{
				old_pos = i;
				break;
			}
		}

		if (pos == 0)
		{ //下阵
			if (old_pos < 0)
			{
				break;
			}

			player->data->partner_formation[old_pos] = 0;
		}
		else
		{
			uint64_t pos_partner_uuid = player->data->partner_formation[pos - 1];
			player->data->partner_formation[pos - 1] = partner_uuid;
			if (old_pos >= 0)
			{ //原来在阵上，位置互换
				player->data->partner_formation[old_pos] = pos_partner_uuid;
			}
			else
			{ //原来不在阵上，上阵
				player->add_task_progress(TCT_PARTNER_OUT_FIGHT, partner->data->partner_id, 1);
			}
		}

		partner->mark_bind();

		player->adjust_battle_partner();
		if (player->is_partner_battle())
		{
			player->calculate_attribute(true);
		}
	} while(0);

	PartnerFormationAnswer resp;
	partner_formation_answer__init(&resp);

	resp.result = ret;
	resp.uuids = player->data->partner_formation;
	resp.n_uuids = MAX_PARTNER_FORMATION_NUM;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_FORMATION_ANSWER, partner_formation_answer__pack, resp);

	return 0;
}

//伙伴学习技能请求
static int handle_partner_learn_skill_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerLearnSkillRequest *req = partner_learn_skill_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint64_t partner_uuid = req->uuid;
	uint32_t skill_idx = req->index;
	uint32_t book_id = req->bookid;

	partner_learn_skill_request__free_unpacked(req, NULL);

	int ret = 0;
	PartnerSkill *pSkill = NULL;
	do
	{
		partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
		if (!partner)
		{
			ret = ERROR_ID_PARTNER_UUID;
			LOG_ERR("[%s:%d] player[%lu] can't find, partner:%lu, skill_idx:%u, book_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id);
			break;
		}

		int real_idx = (int)skill_idx - 1;
		if (!(real_idx >= 0 && real_idx < MAX_PARTNER_SKILL_NUM))
		{
			ret = ERROR_ID_PARTNER_SKILL_INDEX;
			LOG_ERR("[%s:%d] player[%lu] skill index error, partner:%lu, skill_idx:%u, book_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id);
			break;
		}

		uint32_t need_book_num = 1;
		uint32_t has_book_num = player->get_item_num_by_id(book_id);
		if (has_book_num < need_book_num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] skill book not enough, partner:%lu, skill_idx:%u, book_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id, need_book_num, has_book_num);
			break;
		}

		ItemsConfigTable *book_config = get_config_by_id(book_id, &item_config);
		if (!book_config || book_config->ItemEffect != 16)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get itemconfig failed, partner:%lu, skill_idx:%u, book_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id);
			break;
		}

		if (book_config->n_ParameterEffect < 2)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] param num error, partner:%lu, skill_idx:%u, book_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id);
			break;
		}

		uint32_t skill_id = book_config->ParameterEffect[0];
		if (get_config_by_id(skill_id, &skill_config) == NULL)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] param value error, not skill, partner:%lu, skill_idx:%u, book_id:%u, skill_id:%u",
				__FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id, skill_id);
			break;
		}

		pSkill = &partner->data->attr_cur.skill_list[real_idx];
		if (pSkill->skill_id == skill_id)
		{ //技能升级
			uint32_t cur_exp = book_config->ParameterEffect[1] + pSkill->exp;
			int add_lv = 0;
			int cur_lv = pSkill->lv;
			for (;;)
			{
				int need_lv;
				uint32_t need_exp = get_partner_skill_levelup_exp(cur_lv, &need_lv);
				if (need_exp == 0)
					break;
				if (cur_exp < need_exp)
					break;
				if ((int)(partner->get_level()) < need_lv)
				{
					LOG_INFO("%s: player[%lu] partner %lu learn skill, but partner lv not enough", __FUNCTION__, extern_data->player_id, partner_uuid);
					cur_exp = need_exp - 1;
					break;
				}

				cur_exp -= need_exp;
				add_lv++;
			}
			assert(add_lv >= 0 && cur_exp >= 0);

			if (add_lv > 0 && get_skill_level_config(skill_id, pSkill->lv + add_lv) == NULL)
			{
				ret = ERROR_ID_PARTNER_SKILL_LEVEL_MAX;
				LOG_ERR("[%s:%d] player[%lu] skill level max, partner:%lu, skill_idx:%u, book_id:%u, skill_id:%u, cur_level:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id, skill_id, pSkill->lv);
				break;
			}
			LOG_DEBUG("%s: player[%lu] partner[%lu] upgrade skill %u, lv[%u] add[%u]", __FUNCTION__, extern_data->player_id, partner_uuid, skill_id, pSkill->lv, add_lv);
			pSkill->lv += add_lv;
			pSkill->exp = cur_exp;
		}
		else
		{
			if (pSkill->lock)
			{
				ret = ERROR_ID_PARTNER_CANT_LEARN_SAME_SKILL;
				LOG_ERR("[%s:%d] player[%lu] skill lock, partner:%lu, skill_idx:%u, book_id:%u, skill_id:%u, cur_level:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id, skill_id, pSkill->lv);
				break;
			}
			bool has_same_skill = false;
			for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
			{
				if (i == real_idx)
				{
					continue;
				}

				if (partner->data->attr_cur.skill_list[i].skill_id > 0 && partner->data->attr_cur.skill_list[i].skill_id == skill_id)
				{
					has_same_skill = true;
					break;
				}
			}

			if (has_same_skill)
			{
				ret = ERROR_ID_PARTNER_CANT_LEARN_SAME_SKILL;
				LOG_ERR("[%s:%d] player[%lu] skill same, partner:%lu, skill_idx:%u, book_id:%u, skill_id:%u, cur_level:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, skill_idx, book_id, skill_id, pSkill->lv);
				break;
			}

			pSkill->skill_id = skill_id;
			pSkill->lv = 1;
			pSkill->lock = false;
		}

		partner->mark_bind();
		player->del_item_by_id(book_id, need_book_num, MAGIC_TYPE_PARTNER_LEARN_SKILL);
	} while(0);

	PartnerLearnSkillAnswer resp;
	partner_learn_skill_answer__init(&resp);

	PartnerSkillData skill_data;
	partner_skill_data__init(&skill_data);

	resp.result = ret;
	resp.uuid = partner_uuid;
	resp.index = skill_idx;
	if (ret == 0 && pSkill != NULL)
	{
		resp.skill = &skill_data;
		skill_data.id = pSkill->skill_id;
		skill_data.lv = pSkill->lv;
		skill_data.exp = pSkill->exp;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_LEARN_SKILL_ANSWER, partner_learn_skill_answer__pack, resp);

	return 0;
}

//伙伴使用经验丹请求
static int handle_partner_use_exp_item_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerUseExpItemRequest *req = partner_use_exp_item_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint64_t partner_uuid = req->uuid;
	uint32_t item_id = req->itemid;

	partner_use_exp_item_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
		if (!partner)
		{
			ret = ERROR_ID_PARTNER_UUID;
			LOG_ERR("[%s:%d] player[%lu] can't find, partner:%lu, item_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, item_id);
			break;
		}

		uint32_t need_item_num = 1;
		uint32_t has_item_num = player->get_item_can_use_num(item_id);
		if (has_item_num < need_item_num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] exp item not enough, partner:%lu, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, item_id, need_item_num, has_item_num);
			break;
		}

		ItemsConfigTable *prop_config = get_config_by_id(item_id, &item_config);
		if (!prop_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get itemconfig failed, partner:%lu, item_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, item_id);
			break;
		}

		if (prop_config->ItemEffect != 15)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] item effect error, partner:%lu, item_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, item_id);
			break;
		}

		if (prop_config->n_ParameterEffect < 1)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] param num error, partner:%lu, item_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, item_id);
			break;
		}

		uint32_t exp_val = prop_config->ParameterEffect[0];
		if (exp_val == 0)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] param val error, partner:%lu, item_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, item_id);
			break;
		}

		uint32_t player_level = player->get_level();
		uint32_t partner_level = partner->get_level();
		if (partner_level >= player_level)
		{
			ret = 190500334;
			LOG_ERR("[%s:%d] player[%lu] partner level limit, partner:%lu, item_id:%u, partner_lv:%u, player_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, item_id, partner_level, player_level);
			break;
		}

		if (!get_partner_level_config(partner_level + 1))
		{
			ret = ERROR_ID_PARTNER_LEVEL_MAX;
			LOG_ERR("[%s:%d] player[%lu] partner level max, partner:%lu, item_id:%u, partner_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid, item_id, partner_level);
			break;
		}

		player->del_item(item_id, need_item_num, MAGIC_TYPE_PARTNER_USE_EXP_ITEM);

		partner->add_exp(exp_val, MAGIC_TYPE_PARTNER_USE_EXP_ITEM, player_level, true);
		partner->mark_bind();
		player->add_task_progress(TCT_PARTNER_USE_EXP_ITEM, 0, 1);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_USE_EXP_ITEM_ANSWER, comm_answer__pack, resp);

	return 0;
}

//伙伴遣散请求
static int handle_partner_dismiss_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerDismissRequest *req = partner_dismiss_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}



	int ret = 0;
	do
	{
		for (size_t i = 0; i < req->n_uuids; ++i)
		{
			uint64_t partner_uuid = req->uuids[i];
			partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
			if (!partner)
			{
				ret = ERROR_ID_PARTNER_UUID;
				LOG_ERR("[%s:%d] player[%lu] can't find, partner:%lu", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid);
				break;
			}

			if (player->partner_is_in_formation(partner_uuid))
			{
				ret = ERROR_ID_PARTNER_IN_FORMATION;
				LOG_ERR("[%s:%d] player[%lu] in formation, partner:%lu", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid);
				break;
			}
		}

		if (ret != 0)
		{
			break;
		}

		std::map<uint32_t, uint32_t> recycle_item_map;
		for (size_t i = 0; i < req->n_uuids; ++i)
		{
			uint64_t partner_uuid = req->uuids[i];
			partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
			PartnerTable *config = get_config_by_id(partner->data->partner_id, &partner_config);
			if (config)
			{
				for (uint32_t j = 0; j < config->n_SeveranceItem; ++j)
				{
					recycle_item_map[config->SeveranceItem[j]] += config->SeveranceNum[j];
				}
			}
			if(partner != NULL && partner->data != NULL && partner->data->cur_fabao.fabao_id != 0)
			{
				recycle_item_map[partner->data->cur_fabao.fabao_id] += 1;
			}
		}

		if (!player->check_can_add_item_list(recycle_item_map))
		{
			ret = ERROR_ID_BAG_GRID_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] bag grid not enough", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		//伙伴身上有法宝的先将法宝卸下放入背包
		for (size_t i = 0; i < req->n_uuids; ++i)
		{
			uint64_t partner_uuid = req->uuids[i];
			partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
			if(partner == NULL || partner->data == NULL)
			{
				continue;
			}
			std::map<uint32_t, uint32_t>::iterator itr = recycle_item_map.find(partner->data->cur_fabao.fabao_id);
			if(itr != recycle_item_map.end())
			{
				recycle_item_map.erase(itr);
			}
			if(partner->data->cur_fabao.fabao_id != 0)
			{
				for(uint32_t i =0; i < player->data->bag_grid_num; ++i)
				{
					bag_grid_data& fabao_grid = player->data->bag[i];
					if(fabao_grid.id == 0)
					{
						fabao_grid.id = partner->data->cur_fabao.fabao_id;
						fabao_grid.num = 1;
						fabao_grid.especial_item.fabao.main_attr.id = partner->data->cur_fabao.main_attr.id;
						fabao_grid.especial_item.fabao.main_attr.val = partner->data->cur_fabao.main_attr.val;
						memcpy(&fabao_grid.especial_item.fabao.minor_attr, &partner->data->cur_fabao.minor_attr, sizeof(AttrInfo) * MAX_HUOBAN_FABAO_MINOR_ATTR_NUM);
						player->add_item_pos_cache(fabao_grid.id, i);
						player->update_bag_grid(i);	
						player->add_task_progress(TCT_CARRY_ITEM, fabao_grid.id, fabao_grid.num);
						break;
					}
				}
			}

		
		}
		for (size_t i = 0; i < req->n_uuids; ++i)
		{
			uint64_t partner_uuid = req->uuids[i];
			player->remove_partner(partner_uuid);
		}
		player->add_item_list(recycle_item_map, MAGIC_TYPE_PARTNER_DISMISS);
	} while(0);

	if(ret == 0)
	{
		//同步信息到redis，放在最后
		player->refresh_player_redis_info();
	}


	PartnerDismissAnswer resp;
	partner_dismiss_answer__init(&resp);

	resp.result = ret;
	resp.uuids = req->uuids;
	resp.n_uuids = req->n_uuids;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_DISMISS_ANSWER, partner_dismiss_answer__pack, resp);

	partner_dismiss_request__free_unpacked(req, NULL);

	return 0;
}

//伙伴兑换请求
static int handle_partner_exchange_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerExchangeRequest *req = partner_exchange_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t partner_id = req->partnerid;

	partner_exchange_request__free_unpacked(req, NULL);

	int ret = 0;
	uint64_t uuid = 0;
	do
	{
		bool in_dictionary = false;
		for (int i = 0; i < MAX_PARTNER_TYPE; ++i)
		{
			if (player->data->partner_dictionary[i] == partner_id)
			{
				in_dictionary = true;
				break;
			}
		}

		if (!in_dictionary)
		{
			ret = ERROR_ID_PARTNER_NOT_IN_DICTIONARY;
			LOG_ERR("[%s:%d] player[%lu] not in dictionary, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		if (player->has_owned_partner(partner_id))
		{
			ret = 1;
			LOG_ERR("[%s:%d] player[%lu] has had same partner, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		PartnerTable *config = get_config_by_id(partner_id, &partner_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get config failed, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

//		if (config->n_Recruit < 2)
//		{
//			ret = ERROR_ID_NO_CONFIG;
//			LOG_ERR("[%s:%d] player[%lu] param num error, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
//			break;
//		}
//
//		uint32_t item_id = config->Recruit[0];
//		uint32_t need_item_num = config->Recruit[1];
		uint32_t item_id = config->PokedexItem;
		uint32_t need_item_num = config->PokedexItemNum;
		uint32_t has_item_num = player->get_item_can_use_num(item_id);
		if (has_item_num < need_item_num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] item not enough, partner_id:%u, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id, item_id, need_item_num, has_item_num);
			break;
		}

		if (player->m_partners.size() >= (size_t)MAX_PARTNER_NUM)
		{
			ret = ERROR_ID_PARTNER_NUM_MAX;
			LOG_ERR("[%s:%d] player[%lu] partner num max, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		ret = player->add_partner(partner_id, &uuid);
		if (ret != 0)
		{
			ret = ERROR_ID_PARTNER_NUM_MAX;
			LOG_ERR("[%s:%d] player[%lu] partner num max, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		player->del_item(item_id, need_item_num, MAGIC_TYPE_PARTNER_EXCHANGE);
	} while(0);

	PartnerExchangeAnswer resp;
	partner_exchange_answer__init(&resp);

	resp.result = ret;
	resp.uuid = uuid;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_EXCHANGE_ANSWER, partner_exchange_answer__pack, resp);

	return 0;
}

enum
{
	PARTNER_RECRUIT_JUNIOR_ONE = 1,
	PARTNER_RECRUIT_JUNIOR_REPEAT = 2,
	PARTNER_RECRUIT_SENIOR_ONE = 3,
	PARTNER_RECRUIT_SENIOR_REPEAT = 4,
};

struct PartnerRecruitResult
{
	uint32_t type;
	union
	{
		struct 
		{
			uint32_t partner_id;
			uint64_t uuid;
		} partner;
		struct
		{
			uint32_t id;
			uint32_t num;
		} item;
	} data;
};

#define MAX_PARTNER_RECRUIT_NUM 5

//伙伴招募请求
static int handle_partner_recruit_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerRecruitRequest *req = partner_recruit_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;

	partner_recruit_request__free_unpacked(req, NULL);

	int ret = 0;
	std::vector<PartnerRecruitResult> results;
	do
	{
		RecruitTable *config = get_partner_recruit_config(type);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] recruit type error, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		uint32_t *pRecruitCount = NULL;
		uint32_t *pRecruitFreeCD = NULL;
		uint32_t *pRecruitFreeCount = NULL;
		if (type == PARTNER_RECRUIT_JUNIOR_ONE || type == PARTNER_RECRUIT_JUNIOR_REPEAT)
		{
			pRecruitFreeCD = &player->data->partner_recruit_junior_free_cd;
			pRecruitFreeCount = &player->data->partner_recruit_junior_free_count;
			pRecruitCount = &player->data->partner_recruit_junior_count;
		}
		else if (type == PARTNER_RECRUIT_SENIOR_ONE || type == PARTNER_RECRUIT_SENIOR_REPEAT)
		{
			pRecruitFreeCD = &player->data->partner_recruit_senior_free_cd;
			pRecruitFreeCount = &player->data->partner_recruit_senior_free_count;
			pRecruitCount = &player->data->partner_recruit_senior_count;
		}

		if (!pRecruitCount || !pRecruitFreeCD || !pRecruitFreeCount)
		{
			ret = ERROR_ID_PARTNER_RECRUIT_TYPE;
			LOG_ERR("[%s:%d] player[%lu] recruit type error, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}

		uint32_t need_coin = 0, need_gold = 0, need_time = 0, need_item_id = 0, need_item_num = 0;
		uint32_t now = time_helper::get_cached_time() / 1000;
		if ((type == PARTNER_RECRUIT_JUNIOR_ONE || type == PARTNER_RECRUIT_SENIOR_ONE) && (*pRecruitFreeCount < config->FreeRecruit && *pRecruitFreeCD <= now))
		{
			need_time = config->Time;
		}
		else
		{
			for (uint32_t i = 0; i < config->n_ConsumeType; ++i)
			{
				switch (config->ConsumeType[i])
				{
					case 1: //银币
						{
							need_coin = config->ConsumeNum * (double)config->Discount / 10.0;
						}
						break;
					case 2: //元宝
						{
							need_gold = config->ConsumeNum * (double)config->Discount / 10.0;
						}
						break;
					case 3: //抽奖券
						{
							need_item_id = sg_choujiangquan_item_id;
							need_item_num = config->ConsumeNum1;
						}
						break;
				}
			}
		}

		if (need_item_num > 0)
		{
			uint32_t has_item_num = player->get_item_can_use_num(need_item_id);
			if (has_item_num >= need_item_num)
			{ //抽奖券足够，使用抽奖券
				need_coin = 0;
				need_gold = 0;
			}
			else
			{
				need_item_id = 0;
				need_item_num = 0;
			}
		}

		if (need_coin > 0)
		{
			uint32_t has_coin = player->get_coin();
			if (has_coin < need_coin)
			{
				ret = 190500555;
				LOG_ERR("[%s:%d] player[%lu] coin not enough, type:%u, need_coin:%u, has_coin:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, need_coin, has_coin);
				break;
			}
		}
		if (need_gold > 0)
		{
			uint32_t has_gold = player->get_comm_gold();
			if (has_gold < need_gold)
			{
				ret = 190500556;
				LOG_ERR("[%s:%d] player[%lu] gold not enough, type:%u, need_gold:%u, has_gold:%u", __FUNCTION__, __LINE__, extern_data->player_id, type, need_gold, has_gold);
				break;
			}
		}

		if (player->m_partners.size() + config->RecruitNum > MAX_PARTNER_NUM)
		{
			ret = ERROR_ID_PARTNER_NUM_MAX;
			LOG_ERR("[%s:%d] player[%lu] partner num, type:%u, cur_num:%lu", __FUNCTION__, __LINE__, extern_data->player_id, type, player->m_partners.size());
			break;
		}

		std::map<uint32_t, uint32_t> award_item_map;
		for (uint32_t i = 0; i + 1 < config->n_GetItem; i = i+2)
		{
			award_item_map[config->GetItem[i]] = config->GetItem[i + 1];
		}
//		if (award_item_map.size() > 0 && !player->check_can_add_item_list(award_item_map))
//		{
//			ret = ERROR_ID_BAG_GRID_NOT_ENOUGH;
//			LOG_ERR("[%s:%d] player[%lu] bag space not enough, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
//			break;
//		}

		bool is_first_time = (!player->data->partner_recruit_first);
		for (uint64_t i = 0; i < config->RecruitNum; ++i)
		{
			(*pRecruitCount)++;
			if (is_first_time)
			{
				PartnerRecruitResult info;
				info.type = 2;
				info.data.partner.partner_id = 0;
				info.data.partner.uuid = 0;
				uint32_t partner_id = config->First[rand() % config->n_First];
				info.data.partner.partner_id = partner_id;
				player->add_partner(partner_id, &info.data.partner.uuid);
				results.push_back(info);
				is_first_time = false;
				continue;
			}

			if ((*pRecruitCount) % config->BottomTime == 0)
			{ //触发保底，必出伙伴
				*pRecruitCount = 0;
				uint64_t total_rate = 0;
				for (uint32_t j = 0; j < config->n_RecruitBottom; ++j)
				{
					total_rate += config->RecruitBottom[j];
				}

				if (total_rate == 0)
				{
					continue;
				}

				uint64_t rand_val = rand() % total_rate;
				uint64_t rate_count = 0;
				for (uint32_t j = 0; j < config->n_RecruitBottom; ++j)
				{
					if (rand_val >= rate_count && rand_val < rate_count + config->RecruitBottom[j])
					{
						//按概率随机出伙伴的品级，然后找出对应品级的伙伴，再平均随机出一个
						std::vector<uint64_t> rand_vec;
						for (uint32_t k = 0; k < config->n_Recruit; ++k)
						{
							PartnerTable *tmp_config = get_config_by_id(config->Recruit[k], &partner_config);
							if (tmp_config && tmp_config->Grade == config->BottomGrade[j])
							{
								rand_vec.push_back(config->Recruit[k]);
							}
						}

						if (rand_vec.empty())
						{
							break;
						}

						rand_val = rand() % rand_vec.size();
						uint32_t partner_id = rand_vec[rand_val];

						PartnerRecruitResult info;
						info.type = 2;
						info.data.partner.partner_id = partner_id;
						info.data.partner.uuid = 0;
						if (player->has_owned_partner(partner_id))
						{ //已有伙伴，转换成道具
							uint32_t convert_item_id = 0, convert_item_num = 0;
							get_partner_recruit_convert_item(partner_id, convert_item_id, convert_item_num);
							award_item_map[convert_item_id] += convert_item_num;
						}
						else
						{
							player->add_partner(partner_id, &info.data.partner.uuid);
						}
						results.push_back(info);
						break;
					}

					rate_count += config->RecruitBottom[j];
				}
			}
			else
			{
				//先随机到底是道具还是伙伴
				uint64_t total_rate = 0;
				for (uint32_t j = 0; j < config->n_TypeProbability; ++j)
				{
					total_rate += config->TypeProbability[j];
				}

				if (total_rate == 0)
				{
					continue;
				}

				uint64_t rand_val = rand() % total_rate;
				uint64_t rate_count = 0;
				uint32_t result_type = 0;
				for (uint32_t j = 0; j < config->n_TypeProbability; ++j)
				{
					if (rand_val >= rate_count && rand_val < rate_count + config->TypeProbability[j])
					{
						result_type = j + 1;
						break;
					}

					rate_count += config->TypeProbability[j];
				}

				if (result_type == 1)
				{
					if (config->n_Item == 0)
					{
						continue;
					}

					rand_val = rand() % config->n_Item;
					PartnerRecruitResult info;
					info.type = result_type;
					info.data.item.id = config->Item[rand_val];
					info.data.item.num = config->ItemNum[rand_val];
					results.push_back(info);
					award_item_map[config->Item[rand_val]] += config->ItemNum[rand_val];
				}
				else if (result_type == 2)
				{
					total_rate = 0;
					for (uint32_t j = 0; j < config->n_RecruitProbability; ++j)
					{
						total_rate += config->RecruitProbability[j];
					}

					if (total_rate == 0)
					{
						continue;
					}

					uint64_t rand_val = rand() % total_rate;
					uint64_t rate_count = 0;
					for (uint32_t j = 0; j < config->n_RecruitProbability; ++j)
					{
						if (rand_val >= rate_count && rand_val < rate_count + config->RecruitProbability[j])
						{
							//按概率随机出伙伴的品级，然后找出对应品级的伙伴，再平均随机出一个
							std::vector<uint64_t> rand_vec;
							for (uint32_t k = 0; k < config->n_Recruit; ++k)
							{
								PartnerTable *tmp_config = get_config_by_id(config->Recruit[k], &partner_config);
								if (tmp_config && tmp_config->Grade == config->RecruitGrade[j])
								{
									rand_vec.push_back(config->Recruit[k]);
								}
							}

							if (rand_vec.empty())
							{
								break;
							}

							rand_val = rand() % rand_vec.size();
							uint32_t partner_id = rand_vec[rand_val];

							PartnerRecruitResult info;
							info.type = 2;
							info.data.partner.partner_id = partner_id;
							info.data.partner.uuid = 0;
							if (player->has_owned_partner(partner_id))
							{ //已有伙伴，转换成道具
								uint32_t convert_item_id = 0, convert_item_num = 0;
								get_partner_recruit_convert_item(partner_id, convert_item_id, convert_item_num);
								award_item_map[convert_item_id] += convert_item_num;
							}
							else
							{
								player->add_partner(partner_id, &info.data.partner.uuid);
							}
							results.push_back(info);
							break;
						}

						rate_count += config->RecruitProbability[j];
					}
				}
			}
		}

		if (need_coin > 0)
		{
			player->sub_coin(need_coin, MAGIC_TYPE_PARTNER_RECRUIT);
		}
		if (need_gold > 0)
		{
			player->sub_comm_gold(need_gold, MAGIC_TYPE_PARTNER_RECRUIT);
		}
		if (need_time > 0)
		{
			(*pRecruitFreeCount)++;
			*pRecruitFreeCD = now + need_time;
		}
		if (need_item_id > 0 && need_item_num > 0)
		{
			player->del_item(need_item_id, need_item_num, MAGIC_TYPE_PARTNER_RECRUIT);
		}
		if (!player->data->partner_recruit_first)
		{
			player->data->partner_recruit_first = true;
		}
		if (award_item_map.size() > 0)
		{
			player->add_item_list_otherwise_send_mail(award_item_map, MAGIC_TYPE_PARTNER_RECRUIT, 270300030, NULL);
		}
		player->add_task_progress(TCT_PARTNER_RECRUIT, 0, config->RecruitNum);
		player->add_achievement_progress(ACType_PARTNER_RECRUIT, type, 0, 0, 1);
	} while(0);

	PartnerRecruitAnswer resp;
	partner_recruit_answer__init(&resp);

	PartnerRecruitResultData result_data[MAX_PARTNER_RECRUIT_NUM];
	PartnerRecruitResultData* result_point[MAX_PARTNER_RECRUIT_NUM];
	ItemData  item_data[MAX_PARTNER_RECRUIT_NUM];
	PartnerRecruitPartnerData  partner_data[MAX_PARTNER_RECRUIT_NUM];

	resp.result = ret;
	resp.recruitjuniorfreecd = player->data->partner_recruit_junior_free_cd;
	resp.recruitjuniorfreecount = player->data->partner_recruit_junior_free_count;
	resp.recruitjuniorcount = player->data->partner_recruit_junior_count;
	resp.recruitseniorfreecd = player->data->partner_recruit_senior_free_cd;
	resp.recruitseniorfreecount = player->data->partner_recruit_senior_free_count;
	resp.recruitseniorcount = player->data->partner_recruit_senior_count;
	for (size_t i = 0; i < results.size() && i < MAX_PARTNER_RECRUIT_NUM; ++i)
	{
		result_point[i] = &result_data[i];
		partner_recruit_result_data__init(&result_data[i]);
		result_data[i].type = results[i].type;
		if (results[i].type == 1)
		{
			result_data[i].item = &item_data[i];
			item_data__init(&item_data[i]);
			item_data[i].id = results[i].data.item.id;
			item_data[i].num = results[i].data.item.num;
		}
		else
		{
			result_data[i].partner = &partner_data[i];
			partner_recruit_partner_data__init(&partner_data[i]);
			partner_data[i].partnerid = results[i].data.partner.partner_id;
			partner_data[i].uuid = results[i].data.partner.uuid;
		}
	}
	resp.recruits = result_point;
	resp.n_recruits = results.size();
	resp.type = type;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_RECRUIT_ANSWER, partner_recruit_answer__pack, resp);

	return 0;
}

static int handle_partner_dead_finish_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	player->adjust_battle_partner();

	return 0;
}

static int handle_partner_bond_active_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerBondActiveRequest *req = partner_bond_active_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t bond_id = req->bondid;

	partner_bond_active_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		FetterTable *bond_config = get_config_by_id(bond_id, &partner_bond_config);
		if (!bond_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get config failed, bond_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, bond_id);
			break;
		}

		int bond_idx = -1;
		for (int i = 0; i < MAX_PARTNER_BOND_NUM; ++i)
		{
			if (player->data->partner_bond[i] == 0)
			{
				bond_idx = i;
				break;
			}
			if (player->data->partner_bond[i] == bond_id)
			{
				bond_idx = i;
				break;
			}
		}

		if (bond_idx < 0)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] bond memory, bond_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, bond_id);
			break;
		}

		if (player->data->partner_bond[bond_idx] != 0)
		{
			ret = ERROR_ID_PARTNER_BOND_ACTIVED;
			LOG_ERR("[%s:%d] player[%lu] bond is actived, bond_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, bond_id);
			break;
		}

		if (bond_config->Relation == 0)
		{ //激活
			bool dictionary_active = true;
			for (uint32_t i = 0; i < bond_config->n_Partner; ++i)
			{
				if (!player->partner_dictionary_is_active(bond_config->Partner[i]))
				{
					dictionary_active = false;
					break;
				}
			}

			if (!dictionary_active)
			{
				ret = ERROR_ID_PARTNER_BOND_ACTIVE_CONDITION;
				LOG_ERR("[%s:%d] player[%lu] partner dictionary, bond_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, bond_id);
				break;
			}
		}
		else
		{ //升级
			if (!player->partner_bond_is_active(bond_config->Relation))
			{
				ret = ERROR_ID_PARTNER_BOND_ACTIVE_CONDITION;
				LOG_ERR("[%s:%d] player[%lu] partner bond, bond_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, bond_id);
				break;
			}

			uint32_t need_item_num = bond_config->ItemNum;
			uint32_t has_item_num = player->get_item_can_use_num(bond_config->LevelItem);
			if (has_item_num < need_item_num)
			{
				ret = ERROR_ID_PARTNER_BOND_ACTIVE_CONDITION;
				LOG_ERR("[%s:%d] player[%lu] item, bond_id:%u, item_id:%u, item_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, bond_id, (uint32_t)bond_config->LevelItem, need_item_num, has_item_num);
				break;
			}
		}

		if (bond_config->Relation != 0)
		{
			player->del_item(bond_config->LevelItem, bond_config->ItemNum, MAGIC_TYPE_PARTNER_BOND_ACTIVE);
		}
		else
		{
			player->add_achievement_progress(ACType_PARTNER_BOND_ACTIVE, 0, 0, 0, 1);
		}

		player->data->partner_bond[bond_idx] = bond_id;
		player->calculate_attribute(true);
	} while(0);

	PartnerBondActiveAnswer resp;
	partner_bond_active_answer__init(&resp);

	resp.result = ret;
	resp.bondid = bond_id;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_BOND_ACTIVE_ANSWER, partner_bond_active_answer__pack, resp);

	return 0;
}

static int handle_partner_bond_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerBondRewardRequest *req = partner_bond_reward_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t partner_id = req->partnerid;

	partner_bond_reward_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		PartnerTable *config = get_config_by_id(partner_id, &partner_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get config failed, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		int index = -1;
		for (int i = 0; i < MAX_PARTNER_TYPE; ++i)
		{
			if (player->data->partner_bond_reward[i] == 0)
			{
				index = i;
				break;
			}

			if (player->data->partner_bond_reward[i] == partner_id)
			{
				index = i;
				break;
			}
		}

		if (index < 0)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] bond reward memory, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		if (player->data->partner_bond_reward[index] > 0)
		{
			ret = ERROR_ID_PARTNER_BOND_REWARD_GET;
			LOG_ERR("[%s:%d] player[%lu] bond reward get, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		bool reward_can_get = true;
		for (uint32_t i = 0; i < config->n_Fetter; ++i)
		{
			if (!player->partner_bond_is_active(config->Fetter[i]))
			{
				reward_can_get = false;
				break;
			}
		}

		if (!reward_can_get)
		{
			ret = ERROR_ID_PARTNER_BOND_REWARD_CONDITION;
			LOG_ERR("[%s:%d] player[%lu] bond reward condition, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		std::map<uint32_t, uint32_t> award_map;
		for (uint32_t i = 0; i < config->n_FetterReward && i < config->n_FetterRewardNum; ++i)
		{
			award_map[config->FetterReward[i]] += config->FetterRewardNum[i];
		}
		if (!player->check_can_add_item_list(award_map))
		{
			ret = ERROR_ID_BAG_GRID_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] bag not enough, partner_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, partner_id);
			break;
		}

		player->add_item_list(award_map, MAGIC_TYPE_PARTNER_BOND_REWARD);
		player->data->partner_bond_reward[index] = partner_id;
	} while(0);

	PartnerBondRewardAnswer resp;
	partner_bond_reward_answer__init(&resp);

	resp.result = ret;
	resp.partnerid = partner_id;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_BOND_REWARD_ANSWER, partner_bond_reward_answer__pack, resp);

	return 0;
}

static int handle_partner_compose_stone_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerComposeStoneRequest *req = partner_compose_stone_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	std::map<uint32_t, uint32_t> stone_map;
	for (size_t i = 0; i < req->n_stones; ++i)
	{
		stone_map[req->stones[i]->id] += req->stones[i]->num;
	}

	partner_compose_stone_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		uint32_t stone_type = 0, stone_score = 0;
		for (std::map<uint32_t, uint32_t>::iterator iter = stone_map.begin(); iter != stone_map.end(); ++iter)
		{
			uint32_t item_id = iter->first;
			uint32_t item_num = iter->second;
			ItemsConfigTable *config = get_config_by_id(item_id, &item_config);
			if (!config)
			{
				ret = ERROR_ID_NO_CONFIG;
				LOG_ERR("[%s:%d] player[%lu] get config failed, item_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id);
				break;
			}

			if (stone_type == 0)
			{
				stone_type = config->ItemType;
			}
			else
			{
				if (stone_type != (uint32_t)config->ItemType)
				{
					ret = ERROR_ID_PARTNER_COMPOSE_STONE_TYPE;
					break;
				}
			}

			uint32_t has_num = player->get_item_can_use_num(item_id);
			if (has_num < item_num)
			{
				ret = ERROR_ID_PROP_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] prop not enough, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id, item_num, has_num);
				break;
			}

			if (config->n_ParameterEffect >= 1)
			{
				stone_score += config->ParameterEffect[0] * item_num;
			}
		}

		if (ret != 0)
		{
			break;
		}

		uint32_t product_id = 0, product_score = 0, product_coin = 0;
		if (stone_type == 12)
		{
			product_id = sg_partner_sanshenshi_id;
			product_score = sg_partner_sanshenshi_score;
			product_coin = sg_partner_sanshenshi_coin;
		}
		else if (stone_type == 13)
		{
			product_id = sg_partner_qiyaoshi_id;
			product_score = sg_partner_qiyaoshi_score;
			product_coin = sg_partner_qiyaoshi_coin;
		}
		else
		{
			ret = ERROR_ID_NO_CONFIG;
			break;
		}

		if (stone_score < product_score)
		{
			ret = ERROR_ID_PARTNER_COMPOSE_STONE_SCORE;
			LOG_ERR("[%s:%d] player[%lu] score not enough, stone_type:%u, stone_score:%u, product_score:%u", __FUNCTION__, __LINE__, extern_data->player_id, stone_type, stone_score, product_score);
			break;
		}

		uint32_t product_num = stone_score / product_score;
		ret = player->check_can_add_item(product_id, product_num, NULL);
		if(ret != 0)
		{
			LOG_ERR("[%s:%d] player[%lu] bag not enough, item_id:%u, item_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, product_id, product_num);
			break;
		}

		uint32_t need_coin = product_coin * product_num;
		uint32_t has_coin = player->get_coin();
		if (has_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin not enough, need:%u, has:%u", __FUNCTION__, __LINE__, extern_data->player_id, need_coin, has_coin);
			break;
		}

		for (std::map<uint32_t, uint32_t>::iterator iter = stone_map.begin(); iter != stone_map.end(); ++iter)
		{
			player->del_item(iter->first, iter->second, MAGIC_TYPE_PARTNER_COMPOSE_STONE);
		}
		
		player->add_item(product_id, product_num, MAGIC_TYPE_PARTNER_COMPOSE_STONE);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_COMPOSE_STONE_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int handle_partner_reset_attr_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerUuid *req = partner_uuid__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t partner_uuid = req->uuids;
	partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
	partner_uuid__free_unpacked(req, NULL);
	if (partner == NULL)
	{
		LOG_ERR("[%s:%d] player[%lu] can't find, partner:%lu", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid);
		return -2;
	}
	PartnerTable *config = partner->config;
	if (config == NULL)
	{
		return -3;
	}
	ResResetAttr send;
	res_reset_attr__init(&send);
	send.ret = 0;
	if (player->del_item(config->WashMarrowItem[0], config->WashMarrowItem[1], MAGIC_TYPE_PARTNER_ATTR) != 0)
	{
		send.ret = 190400006;
	}
	else
	{
		partner->relesh_attr();

		double attrData[MAX_PARTNER_ATTR]; //战斗属性 需要战斗力属性
		for (int i = 0; i < MAX_PARTNER_ATTR; ++i)
		{
			attrData[i] = 0;
		}
		partner->calculate_attribute(attrData, partner->data->attr_flash);
		partner->data->attr_flash.power_refresh = calculate_fighting_capacity(attrData);
		send.power = partner->data->attr_flash.power_refresh;
		PartnerAttr sendAttr;
		partner_attr__init(&sendAttr);
		send.flash_attr = &sendAttr;
		uint32_t skill_num = 0;
		PartnerSkillData  skill_data[MAX_PARTNER_SKILL_NUM];
		PartnerSkillData* skill_point[MAX_PARTNER_SKILL_NUM];
		for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
		{
			skill_point[skill_num] = &skill_data[skill_num];
			partner_skill_data__init(&skill_data[skill_num]);
			skill_data[skill_num].id = partner->data->attr_flash.skill_list[i].skill_id;
			skill_data[skill_num].lv = partner->data->attr_flash.skill_list[i].lv;
			skill_num++;
		}
		sendAttr.skills = skill_point;
		sendAttr.n_skills = skill_num;
		sendAttr.base_attr_id = (uint32_t *)base_attr_id;
		sendAttr.n_base_attr_id = MAX_PARTNER_BASE_ATTR;
		sendAttr.base_attr_cur = partner->data->attr_flash.base_attr_vaual;
		sendAttr.n_base_attr_cur = MAX_PARTNER_BASE_ATTR;
		sendAttr.base_attr_up = partner->data->attr_flash.base_attr_up;
		sendAttr.n_base_attr_up = MAX_PARTNER_BASE_ATTR;
		sendAttr.base_attr_up = partner->data->attr_flash.base_attr_up;
		sendAttr.n_base_attr_up = MAX_PARTNER_BASE_ATTR;
		sendAttr.detail_attr_id = partner->data->attr_flash.detail_attr_id;
		sendAttr.n_detail_attr_id = partner->data->attr_flash.n_detail_attr;
		sendAttr.detail_attr_cur = partner->data->attr_flash.detail_attr_vaual;
		sendAttr.n_detail_attr_cur = partner->data->attr_flash.n_detail_attr;
		sendAttr.type = partner->data->attr_flash.type;

		player->add_task_progress(TCT_PARTNER_RESET_ATTR, 0, 1);
		player->add_achievement_progress(ACType_PARTNER_RESET_ATTR, 0, 0, 0, 1);
		//同步信息到redis，放在最后
		player->refresh_player_redis_info();
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_RESET_ATTR_ANSWER, res_reset_attr__pack, send);

	return 0;
}
static int handle_partner_save_attr_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerUuid *req = partner_uuid__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t partner_uuid = req->uuids;
	partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
	partner_uuid__free_unpacked(req, NULL);
	if (partner == NULL)
	{
		LOG_ERR("[%s:%d] player[%lu] can't find, partner:%lu", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid);
		return -2;
	}
	if (partner->data->attr_flash.base_attr_up[0] ==0)
	{
		return -3;
	}

	std::map<uint32_t, uint32_t> item_list;
	for (int i = 0; i < MAX_PARTNER_SKILL_NUM; ++i)
	{
		if (partner->data->attr_cur.skill_list[i].lv > 0)
		{
			std::map<uint32_t, uint32_t>::iterator it = sg_partner_item_map.find(partner->data->attr_cur.skill_list[i].skill_id);
			if (it == sg_partner_item_map.end())
			{
				continue;
			}
			item_list.insert(std::make_pair(it->second, partner->data->attr_cur.skill_list[i].lv));
		}	
	}
	player->add_item_list_otherwise_send_mail(item_list, MAGIC_TYPE_PARTNER_ATTR, 0, NULL);
	memcpy(&(partner->data->attr_cur), &(partner->data->attr_flash), sizeof(partner_attr_data));
	memset(&(partner->data->attr_flash), 0, sizeof(partner_attr_data));
	partner->calculate_attribute(true);
	if (partner->data->strong_num > 0)
	{
		item_list.clear();
		item_list.insert(std::make_pair(partner->config->QualificationsItem[0], partner->config->QualificationsItem[1] * partner->data->strong_num));
		partner->data->strong_num = 0;
		player->add_item_list_otherwise_send_mail(item_list, MAGIC_TYPE_PARTNER_ATTR, 0, NULL);
	}
	send_comm_answer(MSG_ID_PARTNER_SAVE_ATTR_ANSWER, 0, extern_data);

	return 0;
}
static int handle_partner_add_attr_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	StrongPartner *req = strong_partner__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t partner_uuid = req->uuids;
	uint32_t attrId = req->attr_id;
	uint32_t count = req->count;
	partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
	strong_partner__free_unpacked(req, NULL);
	if (partner == NULL)
	{
		LOG_ERR("[%s:%d] player[%lu] can't find, partner:%lu", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid);
		return -2;
	}
	int i = 0;
	for (; i < MAX_PARTNER_BASE_ATTR; ++i)
	{
		if (attrId == base_attr_id[i])
		{
			break;
		}
	}
	if (i == MAX_PARTNER_BASE_ATTR)
	{
		return -2;
	}
	ItemsConfigTable *table = get_config_by_id(partner->config->QualificationsItem[0], &item_config);
	if (table == NULL)
	{
		return -3;
	}
	ResStrongPartner send;
	res_strong_partner__init(&send);
	send.ret = 0;
	for (uint32_t c = 0; partner->data->attr_cur.base_attr_vaual[i] < partner->data->attr_cur.base_attr_up[i] && c < count; ++c)
	{
		if (player->del_item(partner->config->QualificationsItem[0], partner->config->QualificationsItem[1], MAGIC_TYPE_PARTNER_ATTR) != 0)
		{
			send.ret = 190400006;
			break;
		}
		partner->data->attr_cur.base_attr_vaual[i] += rand() % table->ParameterEffect[1] + 1;
		++partner->data->strong_num;
		player->add_task_progress(TCT_PARTNER_ADD_ATTR, 0, 1);
		player->add_achievement_progress(ACType_PARTNER_ADD_ATTR, 0, 0, 0, 1);
		if (partner->data->attr_cur.base_attr_vaual[i] >= partner->data->attr_cur.base_attr_up[i])
		{
			partner->data->attr_cur.base_attr_vaual[i] = partner->data->attr_cur.base_attr_up[i];
			break;
		}
	}

	send.uuids = partner_uuid;
	send.attr_id = attrId;
	send.attr_cur = partner->data->attr_cur.base_attr_vaual[i];

	partner->calculate_attribute(true);
	//同步信息到redis，放在最后
	player->refresh_player_redis_info();
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_ADD_ATTR_ANSWER, res_strong_partner__pack, send);

	return 0;
}
static int handle_partner_add_god_request(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	StrongPartner *req = strong_partner__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	uint64_t partner_uuid = req->uuids;
	uint32_t attrId = req->attr_id;
	uint32_t count = req->count;
	partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
	strong_partner__free_unpacked(req, NULL);
	if (partner == NULL)
	{
		LOG_ERR("[%s:%d] player[%lu] can't find, partner:%lu", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid);
		return -2;
	}
	uint32_t i = 0;
	for (; i < partner->data->n_god; ++i)
	{
		if (attrId == partner->data->god_id[i])
		{
			break;
		}
	}

	ResStrongPartner send;
	res_strong_partner__init(&send);
	send.ret = 0;

	uint32_t itemId = 0;
	uint32_t itemNum = 0;
	if (count == 0) //通用
	{
		if (attrId <= 3) //三神
		{
			itemId = partner->config->ThreeCurrencyItem;
			itemNum = partner->config->ThreeCurrencyItemNum;
		}
		else //七曜
		{
			itemId = partner->config->SevenCurrencyItem;
			itemNum = partner->config->SevenCurrencyItemNum;
		}
	}
	else //专属
	{
		if (attrId <= 3) //三神
		{
			itemId = partner->config->ThreeExclusiveItem;
			itemNum = partner->config->ThreeExclusiveItemNum;
		}
		else //七曜
		{
			itemId = partner->config->SevenExclusiveItem;
			itemNum = partner->config->SevenExclusiveItemNum;
		}
	}
	if ((uint32_t)player->get_item_can_use_num(itemId) < itemNum)
	{
		send.ret = 190400006;
	}
	else if (i <= partner->data->n_god && partner->data->god_level[i] == partner->get_attr(PLAYER_ATTR_LEVEL))
	{
		send.ret = 1;
	}
	
	if (send.ret == 0)
	{
		player->del_item(itemId, itemNum, MAGIC_TYPE_PARTNER_ATTR);
		if (i == partner->data->n_god)
		{
			uint32_t n = 0;
			for (; n < partner->config->n_GodYao; ++n)
			{
				if (attrId == partner->config->GodYao[n])
				{
					partner->data->god_id[partner->data->n_god] = attrId;
					partner->data->god_level[partner->data->n_god++] = 1;
					break;
				}
			}
			if (n == partner->config->n_GodYao)
			{
				return -3;
			}
		}
		else
		{
			++partner->data->god_level[i];
		}
		partner->calculate_attribute(true);
		player->add_task_progress(TCT_PARTNER_ADD_GOD, 0, 1);
		player->add_achievement_progress(ACType_PARTNER_ADD_GOD, 0, 0, 0, 1);
		//同步信息到redis，放在最后
		player->refresh_player_redis_info();
	}

	send.uuids = partner_uuid;
	send.attr_id = attrId;
	send.attr_cur = partner->data->god_level[i];

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_ADD_GOD_ANSWER, res_strong_partner__pack, send);

	return 0;
}

static int handle_partner_fabao_stone_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerFabaoStoneRequest *req = partner_fabao_stone_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if(!req)
	{
		LOG_ERR("[%s:%d] can not upack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	
	uint64_t id = req->id;
	uint64_t mingti_id = req->mingti_id;
	partner_fabao_stone_request__free_unpacked(req, NULL);

	int ret = 0;
	do{
		LifeMagicTable * life_config = get_config_by_id(id, &lifemagic_config);
		if(!life_config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get config failed, id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, id);
			break;
		}
		

		//先判断精力值是否充足
		if(life_config->NeedJingli > player->get_attr(PLAYER_ATTR_ENERGY))
		{
			ret = 190410002;
			break;
		}
		
		//选择对应的命体
		uint64_t LifeNum = 0;
		uint32_t n_LifeProbability = 0;
		uint64_t *LifeProbability = 0;
		uint32_t n_Magic = 0;
		uint64_t *Magic =0;
		if(mingti_id == life_config->Life1)
		{
			LifeNum = life_config->LifeNum1;
			n_LifeProbability = life_config->n_LifeProbability1;
			LifeProbability = life_config->LifeProbability1;
			n_Magic = life_config->n_Magic1;
			Magic = life_config->Magic1;
		}
		else if(mingti_id == life_config->Life2)
		{
			LifeNum = life_config->LifeNum2;
			n_LifeProbability = life_config->n_LifeProbability2;
			LifeProbability = life_config->LifeProbability2;
			n_Magic = life_config->n_Magic2;
			Magic = life_config->Magic2;
		}
		else if(mingti_id == life_config->Life3)
		{
			LifeNum = life_config->LifeNum3;
			n_LifeProbability = life_config->n_LifeProbability3;
			LifeProbability = life_config->LifeProbability3;
			n_Magic = life_config->n_Magic3;
			Magic = life_config->Magic3;
		}
		else if(mingti_id == life_config->Life4)
		{
			LifeNum = life_config->LifeNum4;
			n_LifeProbability = life_config->n_LifeProbability4;
			LifeProbability = life_config->LifeProbability4;
			n_Magic = life_config->n_Magic4;
			Magic = life_config->Magic4;
		}
		else
		{			
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get config failed, item_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, life_config->NeedItem);
			break;
		}
		std::map<uint64_t, uint64_t> item_map;
		item_map[mingti_id] = LifeNum;
		item_map[life_config->NeedItem] = life_config->NeedItemNum;

		//判断要消耗的道具
		for(std::map<uint64_t, uint64_t>::iterator iter = item_map.begin(); iter != item_map.end(); ++iter)
		{
			uint64_t item_id = iter->first;
			uint64_t item_num = iter->second;
			ItemsConfigTable *config = get_config_by_id(item_id, &item_config);
			if (!config)
			{
				ret = ERROR_ID_NO_CONFIG;
				LOG_ERR("[%s:%d] player[%lu] get config failed, item_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, item_id);
				break;
			}
			uint32_t has_num = player->get_item_can_use_num(item_id);
			if (has_num < item_num)
			{
				ret = ERROR_ID_PROP_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] prop not enough, item_id:%lu, need_num:%lu, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id, item_num, has_num);
				break;
			}
		}
		
		if(ret != 0)
		{
			break;
		}

		//根据概率获取对应的法宝
		uint64_t total_gailv = 0;
		for(size_t	i = 0; i < n_LifeProbability; ++i)
		{
			total_gailv += LifeProbability[i];
		}
		if(total_gailv <= 0)
		{
			ret = ERROR_ID_CONFIG;
			break;
		}

		total_gailv = rand() % total_gailv + 1;
		int flag = -1;
		uint64_t gailv_begin = 0;
		uint64_t gailv_end = 0;
		for(size_t j = 0; j < n_LifeProbability; ++j)
		{
			gailv_end = gailv_begin + LifeProbability[j];
			if(total_gailv > gailv_begin && total_gailv <= gailv_end)
			{
				flag = j;
				break;	
			}
			gailv_begin = gailv_end;
		}


		if(flag < 0 || flag >= (int)n_Magic)
		{
			ret = ERROR_ID_CONFIG;
			break;
		}
	
		uint64_t fabao_id = Magic[flag];
		ret = player->check_can_add_item(fabao_id, 1, NULL);
		if(ret != 0)
		{
			LOG_ERR("[%s:%d] player[%lu] bag not enough, item_id:%lu, item_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, fabao_id, 1);
			break;
		}
		for (std::map<uint64_t, uint64_t>::iterator iter = item_map.begin(); iter != item_map.end(); ++iter)
		{
			player->del_item(iter->first, iter->second, MAGIC_TYPE_PARTNER_FABAO_STONE);
		}
		player->set_attr(PLAYER_ATTR_ENERGY, player->get_attr(PLAYER_ATTR_ENERGY) - life_config->NeedJingli);
		player->broadcast_one_attr_changed(PLAYER_ATTR_ENERGY, player->data->attrData[PLAYER_ATTR_ENERGY], false, true);
		
		player->add_item(fabao_id, 1 , MAGIC_TYPE_PARTNER_FABAO_STONE);
		player->add_achievement_progress(ACType_FABAO_COMPOSE, get_item_quality(fabao_id), 0, 0, 1);
	}while(0);


	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_FABAO_STONE_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int handle_partner_rename_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerRenameRequest *req = partner_rename_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(!req)
	{
		LOG_ERR("[%s:%d] partner request unpack failed, player_id:[%lu]", __FUNCTION__, __LINE__, player->data->player_id);
		return (-1);
	}
	partner_struct *partner = player->get_partner_by_uuid(req->uuid);
	if (!partner)
	{
		send_comm_answer(MSG_ID_PARTNER_RENAME_ANSWER, -1, extern_data);
		goto done;
	}
	
	if (strlen(req->name) >= sizeof(partner->data->name))
	{
		send_comm_answer(MSG_ID_PARTNER_RENAME_ANSWER, -1, extern_data);
		goto done;
	}

		// 检查次数，道具
	if (partner->data->partner_rename_free)
	{
		partner->data->partner_rename_free = false;
	}
	else
	{
//		int num = player->get_item_can_use_num(sg_partner_rename_item[0]);
//		if (num < sg_partner_rename_item[1])
		if (player->del_item(sg_partner_rename_item[0], sg_partner_rename_item[1], MAGIC_TYPE_PARTNER_RENAME) != 0)
		{
			send_comm_answer(MSG_ID_PARTNER_RENAME_ANSWER, 190300011, extern_data);
			goto done;
		}
		
	}
	strcpy(partner->data->name, req->name);
	send_comm_answer(MSG_ID_PARTNER_RENAME_ANSWER, 0, extern_data);
	//同步信息到redis，放在最后
	player->refresh_player_redis_info();

done:	
	partner_rename_request__free_unpacked(req, NULL);
	return 0;
}

static int handle_partner_lock_skill_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	ReqLockPartnerSkill *req = req_lock_partner_skill__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] partner fabao change request unpack failed, player_id:[%lu]", __FUNCTION__, __LINE__, player->data->player_id);
	}
	uint64_t partner_uuid = req->uuid;
	uint32_t skillId = req->skill;
	bool lock = req->lock;
	req_lock_partner_skill__free_unpacked(req, NULL);

	partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
	if (partner == NULL || partner->data == NULL)
	{
		LOG_ERR("[%s:%d] player[%lu] can't find partner:%lu", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid);
		return -2;
	}

	AnsLockPartnerSkill send;
	ans_lock_partner_skill__init(&send);
	send.skill = skillId;
	send.lock = lock;
	send.uuid = partner_uuid;
	send.ret = partner->lock_skill(skillId, lock);
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_LOCK_SKILLANSWER, ans_lock_partner_skill__pack, send);

	return 0;
}
//伙伴法宝佩戴或替换请求
static int handle_partner_fabao_change_request(player_struct *player, EXTERN_DATA *extern_data)
{	
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PartnerFabaoChangeRequest *req = partner_fabao_change_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(!req)
	{
		LOG_ERR("[%s:%d] partner fabao change request unpack failed, player_id:[%lu]", __FUNCTION__, __LINE__, player->data->player_id);
	}

	uint32_t bag_pos = req->bage_id;
	//uint32_t item_id = req->fabao_item_id;
	uint64_t partner_uuid = req->partner_uuid;
	partner_fabao_change_request__free_unpacked(req, NULL);
	partner_struct *partner = player->get_partner_by_uuid(partner_uuid);
	if( partner == NULL || partner->data == NULL )
	{
		LOG_ERR("[%s:%d] player[%lu] can't find partner:%lu", __FUNCTION__, __LINE__, extern_data->player_id, partner_uuid);
		return -2;
	}


	
	int ret = 0;
	do{
		bag_grid_data *grid = &player->data->bag[bag_pos];
		ItemsConfigTable *config = get_config_by_id(grid->id, &item_config);
		if(!config)
		{
			ret =  ERROR_ID_NO_CONFIG;
			break;
		}
		if(config->ItemType != 14 || config->n_ParameterEffect < 1)
		{
			ret = 190500376; //非伙伴法宝物品
			break;
		}
		MagicTable *magic_config = get_config_by_id(config->ParameterEffect[0], &MagicTable_config);
		if(!magic_config)
		{
			ret = ERROR_ID_NO_CONFIG;//配置错误
			break;
		}
		if(config->n_ParameterEffect >= 2)
		{
			if(config->ParameterEffect[1] != 0 && config->ParameterEffect[1] != partner->data->partner_id)
			{
				ret = 190500376; //新增错误码，伙伴专用法宝，此伙伴不能使用此法宝
				break;
			}
		}
		
		if(partner->get_level() < config->ItemLevel)
		{
			ret = 190500377; //新增错误码，伙伴等级不足
			break;
		}


		//当前伙伴身上有法宝,将法宝数据先保存下来
		partner_cur_fabao cur_fabao_info;
		memset(&cur_fabao_info, 0, sizeof(partner_cur_fabao));
		if(partner->data->cur_fabao.fabao_id != 0)
		{
			ItemsConfigTable *fabao_config = get_config_by_id(partner->data->cur_fabao.fabao_id, &item_config);
			if(fabao_config == NULL || fabao_config->ItemType !=14)
			{
				ret =  ERROR_ID_NO_CONFIG;
				break;
			}		
			cur_fabao_info.fabao_id = partner->data->cur_fabao.fabao_id;
			cur_fabao_info.main_attr.id = partner->data->cur_fabao.main_attr.id;
			cur_fabao_info.main_attr.val = partner->data->cur_fabao.main_attr.val;
			memcpy(&cur_fabao_info.minor_attr, partner->data->cur_fabao.minor_attr, sizeof(AttrInfo) * MAX_HUOBAN_FABAO_MINOR_ATTR_NUM);
			
		}
		//将当前要佩戴的伙伴法宝数据存到伙伴身上
		partner->data->cur_fabao.fabao_id = grid->id;
		partner->data->cur_fabao.main_attr.id = grid->especial_item.fabao.main_attr.id;
		partner->data->cur_fabao.main_attr.val = grid->especial_item.fabao.main_attr.val;
		for(uint32_t j = 0; j < MAX_HUOBAN_FABAO_MINOR_ATTR_NUM; ++j)
		{
			partner->data->cur_fabao.minor_attr[j].id = grid->especial_item.fabao.minor_attr[j].id;
			partner->data->cur_fabao.minor_attr[j].val = grid->especial_item.fabao.minor_attr[j].val;
		}

		//把本次佩戴的法宝从背包删除
		player->del_item_grid(bag_pos, true);
		

		//将替换下来的法宝放回背包
		if(cur_fabao_info.fabao_id != 0)
		{
			player->move_fabao_to_bag(cur_fabao_info);
		}


		//重新计算属性
		partner->calculate_attribute(true);

		player->add_achievement_progress(ACType_PARTNER_WEAR_FABAO, 0, 0, 0, 1);
		//同步信息到redis，放在最后
		player->refresh_player_redis_info();
	}while(0);

	PartnerFabaoChangeAnswer resp;
	PartnerCurFabaoInfo partner_cur_fabao;
	AttrData partner_fabao_minor_attr[MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData* partner_fabao_minor_attr_point[MAX_HUOBAN_FABAO_MINOR_ATTR_NUM];
	AttrData partner_fabao_main_attr;
	partner_fabao_change_answer__init(&resp);

	resp.result = ret;
	if( partner->data->cur_fabao.fabao_id != 0 )
	{
		resp.cur_fabao = &partner_cur_fabao;
		partner_cur_fabao_info__init(&partner_cur_fabao);
		partner_cur_fabao.fabao_id = partner->data->cur_fabao.fabao_id;
		partner_cur_fabao.main_attr = &partner_fabao_main_attr;
		attr_data__init(&partner_fabao_main_attr);
		partner_fabao_main_attr.id = partner->data->cur_fabao.main_attr.id;
		partner_fabao_main_attr.val = partner->data->cur_fabao.main_attr.val;
		uint32_t attr_num = 0;
		for(int i =0; i < MAX_HUOBAN_FABAO_MINOR_ATTR_NUM; ++i)
		{

			partner_fabao_minor_attr_point[attr_num] = &partner_fabao_minor_attr[attr_num];
			attr_data__init(&partner_fabao_minor_attr[attr_num]);
			partner_fabao_minor_attr[attr_num].id = partner->data->cur_fabao.minor_attr[i].id;
			partner_fabao_minor_attr[attr_num].val = partner->data->cur_fabao.minor_attr[i].val;
			attr_num++;
		}
		partner_cur_fabao.minor_attr = partner_fabao_minor_attr_point;
		partner_cur_fabao.n_minor_attr = attr_num;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PARTNER_FABAO_CHANGE_ANSWER, partner_fabao_change_answer__pack, resp);
	return 0;
}
//即将开启请求领奖
static int handle_gift_receive_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	GiftReceiveAnswer receive_answer;
	gift_receive_answer__init(&receive_answer);

	uint64_t receive_flag = player->data->Receive_type + 1;
	std::map<uint64_t, struct FunctionUnlockTable *>::iterator iter = sg_jijiangopen.find(receive_flag);
	

	if (iter != sg_jijiangopen.end())
	{
		if (iter->second->Level <= player->data->attrData[PLAYER_ATTR_LEVEL])
		{
			if (player->add_item(iter->second->ItemID, iter->second->ItemNum, MAGIC_TYPE_RECEIVE_JIJIANGOPEN) != 0)
			{
				receive_answer.gift_type = player->data->Receive_type;
				receive_answer.result = 190500315;
			}
			else
			{
				receive_answer.gift_type = receive_flag;
				receive_answer.result = 0;
				player->data->Receive_type = receive_flag;
			}
			
		}
		else
		{
			receive_answer.gift_type = player->data->Receive_type;
			receive_answer.result = 1;
		}
	}
	else
	{
		receive_answer.gift_type = player->data->Receive_type;
		receive_answer.result = 1;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_JIJIANGOP_GIFT_INFO_ANSWER, gift_receive_answer__pack, receive_answer);

	return(0);
}
static int notify_jijiangopen_gift_info(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}
	GiftCommNotify notify;
	gift_comm_notify__init(&notify);
	notify.gift_type = player->data->Receive_type;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_JIJIANGOP_GIFT_INFO_NOTIFY, gift_comm_notify__pack, notify);

	return(0);
}

//其他服操作失败返还扣除的资源
static int handle_undo_cost(player_struct *player, EXTERN_DATA *extern_data)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-1);
	}

	PROTO_UNDO_COST *req = (PROTO_UNDO_COST*)get_data();

	if (req->cost.gold > 0)
	{
		player->add_bind_gold(req->cost.gold, req->cost.statis_id);
	}
	if (req->cost.unbind_gold > 0)
	{
		player->add_unbind_gold(req->cost.unbind_gold, req->cost.statis_id);
	}
	if (req->cost.coin > 0)
	{
		player->add_coin(req->cost.coin, req->cost.statis_id);
	}
	if (req->cost.silver > 0)
	{
		player->add_silver(req->cost.silver, req->cost.statis_id);
	}
	std::map<uint32_t, uint32_t> item_map;
	for (size_t i = 0; i < sizeof(req->cost.item_id) / sizeof(req->cost.item_id[0]); ++i)
	{
		uint32_t item_id = req->cost.item_id[i];
		uint32_t item_num = req->cost.item_num[i];
		item_map[item_id] += item_num;
	}
	if (item_map.size() > 0)
	{
		player->add_item_list_otherwise_send_mail(item_map, req->cost.statis_id, 0, NULL);
	}

	return 0;
}
//客户端完成特定副本ai请求继续执行副本
static int handle_continue_raid_ai_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	RaidAiContinueRequest* req = raid_ai_continue_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());

	if(req == NULL)
	{
		LOG_ERR("[%s:%d] req == NULL", __FUNCTION__, __LINE__);
		return -2;
	}
	uint64_t type = req->ai_type;
	raid_ai_continue_request__free_unpacked(req, NULL);

	raid_struct *raid = player->get_raid();
	if(raid == NULL || raid->data == NULL)
	{
		LOG_ERR("[%s:%d] raid == NULL or raid->data == NULL", __FUNCTION__, __LINE__);
		return -3;
	}
	//组队副本,得是队长才行
	struct DungeonTable *r_config = get_config_by_id(raid->data->ID, &all_raid_config);
	if(r_config == NULL)
	{
		LOG_ERR("[%s:%d]获取副本配置失败raid_id[%u]", __FUNCTION__, __LINE__, raid->data->ID);
		return -4;
	}
	if(r_config->DengeonType == 1)
	{
		if (!player->m_team)
		{
			LOG_ERR("%s %d: player[%lu] raid[%u] do not have team", __FUNCTION__, __LINE__, player->get_uuid(), raid->data->ID);
			return -5;
		}
		
		//是否是队长
		if (player->get_uuid() != player->m_team->GetLeadId())
		{
			LOG_DEBUG("%s %d: player[%lu] raid[%u]", __FUNCTION__, __LINE__, player->get_uuid(), raid->data->ID);
			return -6;
		}
	
	}

	raid->data->raid_ai_event = type;

	return 0;
}

//客户端请求跳过新手副本
static int handle_skip_new_raid_request(player_struct *player, EXTERN_DATA *extern_data)
{

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	raid_struct *raid = player->get_raid();
	if (NULL == raid || raid->data->ID != 20035)
	{
		LOG_ERR("[%s:%d] skip new raid fail", __FUNCTION__, __LINE__);
		return -2;
	}

	//将pk模式和阵营设置回去
	player->data->noviceraid_flag = true;
	player->set_attr(PLAYER_ATTR_PK_TYPE, 0);
	player->broadcast_one_attr_changed(PLAYER_ATTR_PK_TYPE, 0, false, true);
	player->set_attr(PLAYER_ATTR_ZHENYING, 0);		
	player->broadcast_one_attr_changed(PLAYER_ATTR_ZHENYING, 0, false, true);
	player->send_zhenying_info();

	//重置玩家属性
	player->calculate_attribute(true);
	player->set_attr(PLAYER_ATTR_HP, player->data->attrData[PLAYER_ATTR_MAXHP]);
	player->data->buff_fight_attr[PLAYER_ATTR_HP] = player->data->attrData[PLAYER_ATTR_HP];
	player->broadcast_one_attr_changed(PLAYER_ATTR_HP, player->data->attrData[PLAYER_ATTR_HP], false, true);

	//将技能重置
	player->m_skill.clear(); //重新初始化技能
	player->m_skill.SendAllSkill();
	player->m_skill.Init();
	player->m_skill.SendAllSkill();
	

	raid->clear_monster();
	raid->m_player[0]->delete_one_buff(114400018, true);
	raid->player_leave_raid(raid->m_player[0]);		


	return 0;
}

void notify_server_level_info(player_struct *player, EXTERN_DATA *extern_data)
{
	ServerLevelInfoNotify nty;
	server_level_info_notify__init(&nty);

	nty.level_id = global_shared_data->server_level.level_id;
	nty.break_goal = global_shared_data->server_level.break_goal;
	nty.break_num = global_shared_data->server_level.break_num;
	nty.breaking = global_shared_data->server_level.breaking;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SERVER_LEVEL_INFO_NOTIFY, server_level_info_notify__pack, nty);
}

void notify_server_level_break(player_struct *player, EXTERN_DATA *extern_data)
{
	uint32_t level_id = global_shared_data->server_level.level_id;
	if (player->data->server_level_break_notify == level_id)
	{
		return;
	}

	player->data->server_level_break_notify = level_id;
	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SERVER_LEVEL_BREAK_NOTIFY, 0, 0);
}

static int notify_achievement_info(player_struct *player, EXTERN_DATA *extern_data)
{
	AchievementInfoNotify nty;
	achievement_info_notify__init(&nty);

	AchievementData  achievement_data[MAX_ACHIEVEMENT_NUM];
	AchievementData* achievement_point[MAX_ACHIEVEMENT_NUM];

	nty.achievements = achievement_point;
	nty.n_achievements = 0;
	for (int i = 0; i < MAX_ACHIEVEMENT_NUM; ++i)
	{
		AchievementInfo *info = &player->data->achievement_list[i];
		if (info->id == 0)
		{
			break;
		}

		achievement_point[nty.n_achievements] = &achievement_data[nty.n_achievements];
		achievement_data__init(&achievement_data[nty.n_achievements]);
		achievement_data[nty.n_achievements].id = info->id;
		achievement_data[nty.n_achievements].star = info->star;
		achievement_data[nty.n_achievements].progress = info->progress;
		achievement_data[nty.n_achievements].state = info->state;
		achievement_data[nty.n_achievements].achievetime = info->achieveTime;
		nty.n_achievements++;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ACHIEVEMENT_INFO_NOTIFY, achievement_info_notify__pack, nty);

	return 0;
}

//成就领取奖励请求
static int handle_achievement_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	AchievementRewardRequest *req = achievement_reward_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t achievement_id = req->achievementid;

	achievement_reward_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		AchievementInfo *info = player->get_achievement_info(achievement_id);
		if (!info)
		{
			ret = ERROR_ID_ACHIEVEMENT_ID;
			LOG_ERR("[%s:%d] player[%lu] get achievement failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, achievement_id);
			break;
		}

		if (info->state != Achievement_State_Achieved)
		{
			ret = 190500396;
			LOG_ERR("[%s:%d] player[%lu] achievement state, id:%u, state:%u", __FUNCTION__, __LINE__, extern_data->player_id, achievement_id, info->state);
			break;
		}

		AchievementHierarchyTable *config = get_achievement_config(info->id, info->star);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get achievement config, id:%u, star:%u", __FUNCTION__, __LINE__, extern_data->player_id, achievement_id, info->star);
			break;
		}

		info->star++;
		if (get_achievement_config(info->id, info->star) == NULL)
		{
			info->state = Achievement_State_Rewarded;
		}
		else
		{
			info->state = Achievement_State_Achieving;
			player->init_achievement_progress(info);
		}

		if (config->RewardValue > 0)
		{
			switch (config->Reward)
			{
				case 1: //经验
					player->add_exp(config->RewardValue, MAGIC_TYPE_ACHIEVEMENT_REWARD);
					break;
				case 2: //银两
					player->add_coin(config->RewardValue, MAGIC_TYPE_ACHIEVEMENT_REWARD);
					break;
				case 3: //绑定元宝
					player->add_bind_gold(config->RewardValue, MAGIC_TYPE_ACHIEVEMENT_REWARD);
					break;
				case 4: //非绑定元宝
					player->add_unbind_gold(config->RewardValue, MAGIC_TYPE_ACHIEVEMENT_REWARD);
					break;
			}
		}

		if (config->Title > 0)
		{
			player->add_title(config->Title);
		}
		if (config->NoticeID > 0)
		{
			player_struct::send_rock_notice(*player, config->NoticeID);
		}

		player->achievement_update_notify(info);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ACHIEVEMENT_REWARD_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int handle_rank_sync_rank(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	PROTO_SYNC_RANK *proto = (PROTO_SYNC_RANK*)get_data();

	for (int i = 0; i < MAX_RANK_TYPE; ++i)
	{
		uint32_t rank_type = proto->ranks[i].type;
		uint32_t rank_lv = proto->ranks[i].rank;
		uint32_t rank_score = proto->ranks[i].score;
		if (rank_type == 0)
		{
			break;
		}

		uint32_t pre_lv = 0;
		for (int j = 0; j < MAX_RANK_TYPE; ++j)
		{
			if (player->data->ranks[j].type == 0)
			{
				player->data->ranks[j].type = rank_type;
				player->data->ranks[j].rank = rank_lv;
				player->data->ranks[j].score = rank_score;
				pre_lv = 0;
				break;
			}
			else if (player->data->ranks[j].type == rank_type)
			{
				pre_lv = player->data->ranks[j].rank;
				player->data->ranks[j].score = rank_score;
				if (pre_lv != rank_lv)
				{
					player->data->ranks[j].rank = rank_lv;
				}
				break;
			}
		}

		if (pre_lv != rank_lv)
		{
			player->add_achievement_progress(ACType_RANKING_RANK, rank_type, rank_lv, rank_score, 1);
			player->check_title_condition(TCType_RANK_RANKING, rank_type, rank_lv);
		}
	}

	return (0);
}

static int handle_rank_sync_change(player_struct * /*player*/, EXTERN_DATA * /*extern_data*/)
{
	PROTO_SYNC_RANK_CHANGE *proto = (PROTO_SYNC_RANK_CHANGE*)buf_head();

	uint32_t rank_type = proto->type;
	ProtoRankPlayer *pPlayer = proto->changes;
	for (uint32_t i = 0; i < proto->num; ++i)
	{
		player_struct *player = player_manager::get_player_by_id(pPlayer[i].player);
		if (!player)
		{
			continue;
		}

		uint32_t pre_lv = 0;
		uint32_t post_lv = pPlayer[i].lv;
		uint32_t post_score = pPlayer[i].score;
		for (int j = 0; j < MAX_RANK_TYPE; ++j)
		{
			if (player->data->ranks[j].type == 0)
			{
				player->data->ranks[j].type = rank_type;
				player->data->ranks[j].rank = post_lv;
				player->data->ranks[j].score = post_score;
				pre_lv = 0;
				break;
			}
			else if (player->data->ranks[j].type == rank_type)
			{
				pre_lv = player->data->ranks[j].rank;
				player->data->ranks[j].score = post_score;
				if (pre_lv != post_lv)
				{
					player->data->ranks[j].rank = post_lv;
				}
				break;
			}
		}

		if (pre_lv != post_lv)
		{
			player->add_achievement_progress(ACType_RANKING_RANK, rank_type, post_lv, post_score, 1);
			player->check_title_condition(TCType_RANK_RANKING, rank_type, post_lv);
		}
	}

	return (0);
}

static int notify_title_info(player_struct *player, EXTERN_DATA *extern_data)
{
	TitleInfoNotify nty;
	title_info_notify__init(&nty);

	TitleData  title_data[MAX_TITLE_NUM];
	TitleData* title_point[MAX_TITLE_NUM];

	nty.titles = title_point;
	nty.n_titles = 0;
	for (int i = 0; i < MAX_TITLE_NUM; ++i)
	{
		TitleInfo *info = &player->data->title_list[i];
		if (info->id == 0)
		{
			break;
		}

		title_point[nty.n_titles] = &title_data[nty.n_titles];
		title_data__init(&title_data[nty.n_titles]);
		title_data[nty.n_titles].id = info->id;
		title_data[nty.n_titles].state = info->state;
		title_data[nty.n_titles].expiretime = info->expire_time;
		title_data[nty.n_titles].isnew = info->is_new;
		title_data[nty.n_titles].activetime = info->active_time;
		nty.n_titles++;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TITLE_INFO_NOTIFY, title_info_notify__pack, nty);

	return 0;
}

//佩戴称号请求
static int handle_title_wear_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	TitleWearRequest *req = title_wear_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t title_id = req->titleid;

	title_wear_request__free_unpacked(req, NULL);

	int ret = 0;
	uint32_t old_id = (uint32_t)player->get_attr(PLAYER_ATTR_TITLE);
	do
	{
		if (title_id != 0)
		{
			TitleInfo *info = player->get_title_info(title_id);
			if (!info)
			{
				ret = ERROR_ID_TITLE_ID;
				LOG_ERR("[%s:%d] player[%lu] get title failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, title_id);
				break;
			}

			if (info->state == 0)
			{
				ret = ERROR_ID_TITLE_NOT_ACTIVE;
				LOG_ERR("[%s:%d] player[%lu] title state, id:%u, state:%u", __FUNCTION__, __LINE__, extern_data->player_id, info->id, info->state);
				break;
			}
		}

		if (old_id == title_id)
		{
			break;
		}

		player->set_attr(PLAYER_ATTR_TITLE, title_id);

		player->calculate_attribute(true);
		player->broadcast_one_attr_changed(PLAYER_ATTR_TITLE, title_id, true, true);

	} while(0);

	TitleWearAnswer resp;
	title_wear_answer__init(&resp);

	resp.result = ret;
	resp.oldid = old_id;
	resp.newid = title_id;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TITLE_WEAR_ANSWER, title_wear_answer__pack, resp);

	return 0;
}

static int handle_title_mark_old_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	TitleMarkOldRequest *req = title_mark_old_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t title_id = req->titleid;

	title_mark_old_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		TitleInfo *info = player->get_title_info(title_id);
		if (!info)
		{
			ret = ERROR_ID_TITLE_ID;
			LOG_ERR("[%s:%d] player[%lu] get title failed, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, title_id);
			break;
		}

		if (info->state == 0)
		{
			ret = ERROR_ID_TITLE_NOT_ACTIVE;
			LOG_ERR("[%s:%d] player[%lu] title state, id:%u, state:%u", __FUNCTION__, __LINE__, extern_data->player_id, info->id, info->state);
			break;
		}

		info->is_new = false;
	} while(0);

	TitleMarkOldAnswer resp;
	title_mark_old_answer__init(&resp);

	resp.result = ret;
	resp.titleid = title_id;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TITLE_MARK_OLD_ANSWER, title_mark_old_answer__pack, resp);

	return 0;
}

static int handle_hero_challenge_main_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	HeroChallengedata hero_data[MAX_HERO_CHALLENGE_MONSTER_NUM];
	HeroChallengedata* hero_data_point[MAX_HERO_CHALLENGE_MONSTER_NUM];

	HeroChallengeMainInfoAnswer ans;
	hero_challenge_main_info_answer__init(&ans);
	ans.data = hero_data_point;
	ans.n_data = 0;

	for(size_t i =0; i <MAX_HERO_CHALLENGE_MONSTER_NUM; i++)
	{
		if(player->data->my_hero_info[i].id == 0)
		{
			break;
		}
		std::map<uint64_t, ChallengeTable*>::iterator itr = hero_challenge_config.find(player->data->my_hero_info[i].id);
		if(itr != hero_challenge_config.end())
		{
			hero_data_point[ans.n_data] = &hero_data[ans.n_data];
			hero_challengedata__init(&hero_data[ans.n_data]);
			hero_data[ans.n_data].hero_id = player->data->my_hero_info[i].id;
			hero_data[ans.n_data].num = player->get_raid_reward_count(itr->second->DungeonID);
			hero_data[ans.n_data].star = player->data->my_hero_info[i].star;
			hero_data[ans.n_data].reward_flag = player->data->my_hero_info[i].reward_flag;
			ans.all_num +=  hero_data[ans.n_data].num;
			ans.n_data++;
		}
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HERO_CHALLENGE_ZHUJIEMIAN_INFO_ANSWER, hero_challenge_main_info_answer__pack, ans);
	return 0;
}

static int handle_hero_challenge_sweep_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	HeroChallengeSweepRequest *req = hero_challenge_sweep_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] hero_challenge_sweep_request__unpack faild player_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t hero_id = req->hero_id;
	uint32_t item_flag = req->item_flag;
	hero_challenge_sweep_request__free_unpacked(req, NULL);
	HeroChallengeSweepAnswer ans;
	HeroChallengeSweepItemInfo itme_info[MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM];
	HeroChallengeSweepItemInfo *itme_info_point[MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM];
	hero_challenge_sweep_answer__init(&ans);
	ans.n_item_info = 0;
	ans.item_info = itme_info_point;
	uint32_t ret = 0;
	uint32_t use_item_id = 0;
	uint32_t use_item_num = 0;
	do{
		std::map<uint64_t, ChallengeTable*>::iterator itr = hero_challenge_config.find(hero_id);
		if(itr == hero_challenge_config.end())
		{
			LOG_ERR("[%s:%d] hero challenge receive reward faild ,hero_id erro hero_id[%u]", __FUNCTION__, __LINE__, hero_id);
			return -3;
		}

		if(item_flag != 1 && item_flag != 2 )
		{
			LOG_ERR("[%s:%d] hero challenge receive reward faild ,item_flag erro item_flag[%u]", __FUNCTION__, __LINE__, item_flag);
			return -4;
		}
		HeroChallengeInfo *hero_info = NULL;
		for(uint32_t j = 0; j < MAX_HERO_CHALLENGE_MONSTER_NUM; j++)
		{
			if(player->data->my_hero_info[j].id == hero_id)
			{
				hero_info = &(player->data->my_hero_info[j]);
				break;
			}
		}
		if(hero_info == NULL)
		{
			LOG_ERR("[%s:%d] hero challenge receive reward faild ,hero_id is erro hero_id[%u]", __FUNCTION__, __LINE__, hero_id);
			return -5;
		}
		if(hero_info->reward_flag ==1)
		{
			LOG_ERR("[%s:%d] hero challenge receive reward faild ,player have reward no lingqu hero_id[%u] reward_flag[%u]", __FUNCTION__, __LINE__, hero_id, hero_info->reward_flag);
			return -6;
		}



		//判断消耗物品够不够
		ParameterTable *parame_config = NULL;
		if(item_flag == 1)
		{
			parame_config = get_config_by_id(161000333, &parameter_config);
			if(parame_config == NULL || parame_config->n_parameter1 != 2)
			{				
				ret = ERROR_ID_CONFIG; //参数表有误
				LOG_ERR("[%s:%d] hero challenge receive reward faild ,parame_config erro", __FUNCTION__, __LINE__);
				break;
			}
			use_item_id = parame_config->parameter1[0];
			use_item_num = parame_config->parameter1[1];

			uint32_t has_num = player->get_item_can_use_num(use_item_id);
			if (has_num < use_item_num)
			{
				ret = ERROR_ID_PROP_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] prop not enough, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, use_item_id, use_item_num, has_num);
				break;
			}
			
		}
		else
		{
			parame_config = get_config_by_id(161000334, &parameter_config);
			if(parame_config == NULL || parame_config->n_parameter1 != 1)
			{				
				ret = ERROR_ID_CONFIG; //参数表有误
				LOG_ERR("[%s:%d] hero challenge receive reward faild ,parame_config erro", __FUNCTION__, __LINE__);
				break;
			}
			use_item_num = parame_config->parameter1[0];
			uint32_t has_num = player->get_attr(PLAYER_ATTR_GOLD) + player->get_attr(PLAYER_ATTR_BIND_GOLD);
			if(has_num < use_item_num)
			{
				ret = ERROR_ID_PROP_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] yuanbao not enough, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id,use_item_num, has_num);
				break;
			}
		}
		
		//判断星级够不够
		uint32_t has_star = hero_info->star;
		if(has_star < 3)
		{
			ret = 190500411; //星级不够
			LOG_ERR("[%s:%d] player[%lu] hero challenge star not enough, need_num:3, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, has_star);
			break;
		}

		//判断今日次数够不够
		uint32_t raid_id = itr->second->DungeonID;
		DungeonTable* dun_config = get_config_by_id(raid_id, &all_raid_config);
		if(dun_config == NULL && dun_config->n_Rewards < 3)
		{
			LOG_ERR("[%s:%d] player[%lu] hero challenge saodang reward faild config erro", __FUNCTION__, __LINE__, extern_data->player_id);
			ret = ERROR_ID_CONFIG; //配置不对
			break;
		}
		struct ControlTable *control_config = get_config_by_id(dun_config->ActivityControl, &all_control_config);
		if(control_config == NULL)
		{
			LOG_ERR("[%s:%d] player[%lu] hero challenge saodang reward faild config erro", __FUNCTION__, __LINE__, extern_data->player_id);
			ret = ERROR_ID_CONFIG; //配置不对
			break;
		}
		//单个已经被使用的次数判断
		uint32_t use_count = player->get_raid_reward_count(raid_id); 
		if(use_count >= control_config->RewardTime)
		{
			LOG_ERR("[%s:%d] player[%lu] hero challenge saodang faild reward num not enough used_num:%u max_num:%lu", __FUNCTION__, __LINE__, extern_data->player_id, use_count, control_config->RewardTime);
			ret = 190500410; //领奖次数不够
			break;
		}
		//判断总次数
		ParameterTable *hero_param_config = get_config_by_id(161000332, &parameter_config); 
		if(hero_param_config == NULL && hero_param_config->n_parameter1 < 1)
		{
			LOG_ERR("[%s:%d] player[%lu] hero challenge saodang reward faild config erro", __FUNCTION__, __LINE__, extern_data->player_id);
			ret = ERROR_ID_CONFIG; //配置不对
			break;
		}
		uint32_t max_all_count = hero_param_config->parameter1[0];
		uint32_t use_all_count = 0;
		for(std::map<uint64_t, ChallengeTable*>::iterator itr = raidid_to_hero_challenge_config.begin(); itr != raidid_to_hero_challenge_config.end(); itr++)
		{
			use_all_count += player->get_raid_reward_count(itr->second->DungeonID);
		}
		if(use_all_count >= max_all_count)
		{
			LOG_ERR("[%s:%d] player[%lu] hero challenge saodang faild reward num not enough used_num:%u max_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, use_all_count, max_all_count);
			ret = 190500409;
			break;
		}

		//扣消耗物品和次数
		if(item_flag == 1)
		{
			player->del_item(use_item_id, use_item_num, MAGIC_TYPE_HEROCHALLENGE_SWEEP_REWARD);
		}
		else
		{
			if(use_item_num > player->get_attr(PLAYER_ATTR_BIND_GOLD))
			{
				use_item_num = use_item_num - player->get_attr(PLAYER_ATTR_BIND_GOLD);
				player->data->attrData[PLAYER_ATTR_BIND_GOLD] = 0;
				player->data->attrData[PLAYER_ATTR_GOLD] = player->get_attr(PLAYER_ATTR_GOLD) - use_item_num;
			}
			else
			{
				player->data->attrData[PLAYER_ATTR_BIND_GOLD] = player->get_attr(PLAYER_ATTR_BIND_GOLD) - use_item_num;
			}
			player->broadcast_one_attr_changed(PLAYER_ATTR_BIND_GOLD, player->data->attrData[PLAYER_ATTR_BIND_GOLD], false, true);
			player->broadcast_one_attr_changed(PLAYER_ATTR_GOLD, player->data->attrData[PLAYER_ATTR_GOLD], false, true);
		}
		player->add_raid_reward_count(raid_id);
		player->check_activity_progress(AM_HERO_CHLLENGE, 0);

		//通过副本的奖励id获取一堆奖励物品
		std::map<uint32_t, uint32_t> item_list;
		uint32_t acer = 0, bind_acer = 0, silver = 0, exp = 0;
		uint32_t reward_id = dun_config->Rewards[2];
		uint32_t reward_num = 0;
		if (get_drop_item(reward_id, item_list) == 0)
		{
			for (std::map<uint32_t, uint32_t>::iterator ite = item_list.begin(); ite != item_list.end() && ans.n_item_info < MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM; ++ite)
			{
				int type = get_item_type(ite->first);
				switch (type)
				{
					case ITEM_TYPE_COIN:
						silver += ite->second;
						break;
					case ITEM_TYPE_BIND_GOLD:
						bind_acer += ite->second;
						break;
					case ITEM_TYPE_GOLD:
						acer += ite->second;
						break;
					case ITEM_TYPE_EXP:
						exp += ite->second;
						break;
					default:
						itme_info_point[ans.n_item_info] = &itme_info[ans.n_item_info];
						hero_challenge_sweep_item_info__init(&itme_info[ans.n_item_info]);
						itme_info[ans.n_item_info].item_id = ite->first;
						itme_info[ans.n_item_info].item_num = ite->second;
						ans.n_item_info++;
				}
				//奖励信息记录到玩家身上
				hero_info->item_info[reward_num].item_id = ite->first;
				hero_info->item_info[reward_num].item_num = ite->second;
				hero_info->reward_flag = 1;
				reward_num++;
			}
		}
		ans.silver = silver;
		ans.bind_acer = bind_acer;
		ans.acer = acer;
		ans.exp = exp;
		

	}while(0);
	ans.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HERO_CHALLENGE_SWEEP_ANSWER, hero_challenge_sweep_answer__pack, ans);
	return 0;
}

static int handle_hero_challenge_sweep_recive_request(player_struct *player, EXTERN_DATA *extern_data)
{

	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	HeroChallengeReciveSweepRewardRequest *req = hero_challenge_recive_sweep_reward_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] hero_challenge_recive_sweep_reward_request__unpack faild player_di:%lu", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t hero_id = req->hero_id;
	hero_challenge_recive_sweep_reward_request__free_unpacked(req, NULL);

	uint32_t ret = 0;
	do{
		HeroChallengeInfo* my_hero_info = NULL;
		for(size_t i =0; i < MAX_HERO_CHALLENGE_MONSTER_NUM; i++)
		{
			if(player->data->my_hero_info[i].id == hero_id)
			{
				my_hero_info = &(player->data->my_hero_info[i]);
				break;
			}
		}

		if(my_hero_info == NULL)
		{
			LOG_ERR("[%s:%d] hero challenge recive reward faild, hero_id:%u", __FUNCTION__, __LINE__, hero_id);
			return -3;
		}
		if(my_hero_info->star < 3)
		{	
			LOG_ERR("[%s:%d] hero challenge recive reward faild,star not enough hero_id:%u", __FUNCTION__, __LINE__, hero_id);
			return -4;
		}
		if(my_hero_info->reward_flag != 1)
		{
			LOG_ERR("[%s:%d] hero challenge recive reward faild,no reward can recive hero_id:%u", __FUNCTION__, __LINE__, hero_id);
			return -5;
		}


		std::map<uint32_t, uint32_t> item_list;
		uint32_t acer = 0, bind_acer = 0, silver = 0, exp = 0;
		for (size_t j =0; j < MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM && my_hero_info->item_info[j].item_id != 0; ++j)
		{
			int type = get_item_type(my_hero_info->item_info[j].item_id);
			switch (type)
			{
				case ITEM_TYPE_COIN:
					silver += my_hero_info->item_info[j].item_num;
					break;
				case ITEM_TYPE_BIND_GOLD:
					bind_acer += my_hero_info->item_info[j].item_num;
					break;
				case ITEM_TYPE_GOLD:
					acer += my_hero_info->item_info[j].item_num;
					break;
				case ITEM_TYPE_EXP:
					exp += my_hero_info->item_info[j].item_num;
					break;
				default:
					item_list[my_hero_info->item_info[j].item_id] += my_hero_info->item_info[j].item_num;
			}
		}

		//判断包过空位够不够
		if (!player->check_can_add_item_list(item_list))
		{
			ret = 190500407;
			LOG_INFO("[%s:%d] player[%lu] bag grid not enough", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}
		//物品进包裹
		player->add_item_list(item_list, MAGIC_TYPE_HEROCHALLENGE_SWEEP_REWARD_RECIVE);
		
		//货币类和经验直接加属性
		if(acer != 0)
		{
			player->add_unbind_gold(acer, MAGIC_TYPE_HEROCHALLENGE_SWEEP_REWARD_RECIVE);
		}
		if(bind_acer != 0 )
		{
			player->add_bind_gold(acer, MAGIC_TYPE_HEROCHALLENGE_SWEEP_REWARD_RECIVE);
		}
		if(silver != 0)
		{
			player->add_coin(silver, MAGIC_TYPE_HEROCHALLENGE_SWEEP_REWARD_RECIVE);
		}
		if(exp != 0)
		{
			player->add_exp(exp, MAGIC_TYPE_HEROCHALLENGE_SWEEP_REWARD_RECIVE);
		}

		//清空玩家身上奖励信息
		my_hero_info->reward_flag = 0;
		memset(&my_hero_info->item_info[0], 0, sizeof(HeroChallengeRewardInfo) * MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM);
		
	}while(0);
	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HERO_CHALLENGE_SWEEP_RECIVE_ANSWER, comm_answer__pack, resp);
	return 0;
}

static int handle_hero_challenge_sweep_reward_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	HeroChallengeReciveSweepRewardRequest *req = hero_challenge_recive_sweep_reward_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] hero_challenge_recive_sweep_reward_request__unpack faild player_di:%lu", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t hero_id = req->hero_id;
	hero_challenge_recive_sweep_reward_request__free_unpacked(req, NULL);

	HeroChallengeSweepAnswer ans;
	HeroChallengeSweepItemInfo itme_info[MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM];
	HeroChallengeSweepItemInfo *itme_info_point[MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM];
	hero_challenge_sweep_answer__init(&ans);
	ans.n_item_info = 0;
	ans.item_info = itme_info_point;

	uint32_t ret = 0;
	do{
		HeroChallengeInfo* my_hero_info = NULL;
		for(size_t i =0; i < MAX_HERO_CHALLENGE_MONSTER_NUM; i++)
		{
			if(player->data->my_hero_info[i].id == hero_id)
			{
				my_hero_info = &(player->data->my_hero_info[i]);
				break;
			}
		}

		if(my_hero_info == NULL)
		{
			LOG_ERR("[%s:%d] hero challenge recive reward faild, hero_id:%u", __FUNCTION__, __LINE__, hero_id);
			return -3;
		}
		if(my_hero_info->star < 3)
		{	
			LOG_ERR("[%s:%d] hero challenge recive reward faild,star not enough hero_id:%u", __FUNCTION__, __LINE__, hero_id);
			return -4;
		}
		if(my_hero_info->reward_flag != 1)
		{
			LOG_ERR("[%s:%d] hero challenge recive reward faild,no reward can recive hero_id:%u", __FUNCTION__, __LINE__, hero_id);
			return -5;
		}

		uint32_t acer = 0, bind_acer = 0, silver = 0, exp = 0;
		for (size_t j =0; j < MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM && my_hero_info->item_info[j].item_id != 0; ++j)
		{
			int type = get_item_type(my_hero_info->item_info[j].item_id);
			switch (type)
			{
				case ITEM_TYPE_COIN:
					silver += my_hero_info->item_info[j].item_num;
					break;
				case ITEM_TYPE_BIND_GOLD:
					bind_acer += my_hero_info->item_info[j].item_num;
					break;
				case ITEM_TYPE_GOLD:
					acer += my_hero_info->item_info[j].item_num;
					break;
				case ITEM_TYPE_EXP:
					exp += my_hero_info->item_info[j].item_num;
					break;
				default:
					itme_info_point[ans.n_item_info] = &itme_info[ans.n_item_info];
					hero_challenge_sweep_item_info__init(&itme_info[ans.n_item_info]);
					itme_info[ans.n_item_info].item_id = my_hero_info->item_info[j].item_id;
					itme_info[ans.n_item_info].item_num = my_hero_info->item_info[j].item_num;
					ans.n_item_info++;
			}
		}
		ans.silver = silver;
		ans.bind_acer = bind_acer;
	    ans.acer = acer;
		ans.exp = exp;

	}while(0);

	ans.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_HERO_CHALLENGE_SWEEP_REWARD_INFO_NOTIFY, hero_challenge_sweep_answer__pack, ans);

	return 0;
}
static int handle_mijing_xiulian_task_info_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	player->mijing_shilian_info_notify(1);
	return 0;

}
static int handle_mijing_xiulian_shuaxing_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	MiJingXiuLianTaskShuaXingAnswer ans;
	mi_jing_xiu_lian_task_shua_xing_answer__init(&ans);
	
	int ret = 0;
	UndergroundTask* mijing_config = get_config_by_id(player->data->mi_jing_xiu_lian.digong_id, &mijing_xiulian_config);
	if(mijing_config == NULL)
	{
		LOG_ERR("[%s:%d] mijing xiulian shuaxiang faild player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -5;
	}
	do{
		ParameterTable *mijing_parame = get_config_by_id(161000335, &parameter_config);
		if(mijing_parame->n_parameter1 < 2 || mijing_parame->n_parameter1%2 != 0)
		{
			LOG_ERR("[%s:%d] mijing xiulian shuaxiang faild player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
			ret = ERROR_ID_CONFIG; //配置不对
			break;
		}
		for(size_t j = 0; j+1 < mijing_parame->n_parameter1; j=j+2)
		{
			uint32_t item_id =  mijing_parame->parameter1[j];
			uint32_t use_item_num = mijing_parame->parameter1[j+1];
			uint32_t has_num = player->get_item_can_use_num(item_id);
			if (has_num < use_item_num)
			{
				ret = ERROR_ID_PROP_NOT_ENOUGH;
				LOG_ERR("[%s:%d] player[%lu] prop not enough, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id, use_item_num, has_num);
				break;
			}
		}
		if(ret !=0)
		{
			break;
		}
		for(size_t j = 0; j+1 < mijing_parame->n_parameter1; j=j+2)
		{
			player->del_item(mijing_parame->parameter1[j], mijing_parame->parameter1[j+1], MAGIC_TYPE_MIJING_XIULIAN_SHUAXING);
		}
		//根据概率获取对应的倍率
		uint64_t total_gailv = 0;
		for(size_t	i = 0; i < mijing_config->n_StarProbability; ++i)
		{
			total_gailv += mijing_config->StarProbability[i];
		}
		if(total_gailv <= 0)
		{
			LOG_ERR("[%s:%d]  mi jing xiu lian shua xing faild player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
			ret = ERROR_ID_CONFIG;
			break;
		}

		total_gailv = rand() % total_gailv + 1;
		int flag = -1;
		uint64_t gailv_begin = 0;
		uint64_t gailv_end = 0;
		for(size_t j = 0; j < mijing_config->n_StarProbability; ++j)
		{
			gailv_end = gailv_begin + mijing_config->StarProbability[j];
			if(total_gailv > gailv_begin && total_gailv <= gailv_end)
			{
				flag = j;
				break;	
			}
			gailv_begin = gailv_end;
		}


		if(flag < 0)
		{
			LOG_ERR("[%s:%d] get mi jing xiu lian task info faild player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
			ret = ERROR_ID_CONFIG;
			break;
		}
		player->data->mi_jing_xiu_lian.reward_beilv = uint32_t(flag);
		ans.reward_beilv = flag;

	}while(0);	
	ans.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MIJING_XIULIAN_TASK_SHUAXING_ANSWER, mi_jing_xiu_lian_task_shua_xing_answer__pack, ans);

	return 0;
}

#define ERROR_ID_HAS_NOT_FISHING_TASK		  2601 //当前没有钓鱼任务
static int handle_fishing_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	FishingRewardRequest *req = fishing_reward_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t type = req->type;

	fishing_reward_request__free_unpacked(req, NULL);

	LOG_INFO("%s: player[%lu], type[%u]", __FUNCTION__, extern_data->player_id, type);
	
	int ret = 0;
	std::map<uint32_t, uint32_t> reward_map;
	do
	{
		const uint32_t activity_id = 330000037;
		TaskInfo *task = NULL;
		TaskConditionTable *cond_config = NULL;
		if (type == 1)
		{ //普通钓鱼
			//检查区域
			uint32_t region_id = player->get_attr(PLAYER_ATTR_REGION_ID);
			if (region_id != 20)
			{
				ret = ERROR_ID_NOT_IN_FISHING_REGION;
				LOG_ERR("[%s:%d] player[%lu] not in fishing region, region_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, region_id);
				break;
			}

			//检查活动时间
			if (!activity_is_open(activity_id))
			{
				ret = 190500420;
				LOG_ERR("[%s:%d] player[%lu] activity not open, activity_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, activity_id);
				break;
			}
		}
		else if (type == 2)
		{ //任务钓鱼
			for (int i = 0; i < MAX_TASK_ACCEPTED_NUM; ++i)
			{
				TaskInfo *info = &player->data->task_list[i];
				if (info->id == 0)
				{
					continue;
				}

				for (int j = 0; j < MAX_TASK_TARGET_NUM; ++j)
				{
					TaskCountInfo &count_info = info->progress[j];
					TaskConditionTable *config = get_config_by_id(count_info.id, &task_condition_config);
					if (!config)
					{
						continue;
					}

					if (config->ConditionType != TCT_FISHING)
					{
						continue;
					}

					task = info;
					cond_config = config;
					break;
				}
				if (task != NULL)
				{
					break;
				}
			}

			if (!task)
			{
				ret = ERROR_ID_HAS_NOT_FISHING_TASK;
				LOG_ERR("[%s:%d] player[%lu] has not fishing task, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
				break;
			}

			ret = check_task_complete_pos(cond_config, player, task->id, cond_config->ID);
			if (ret != 0)
			{
				break;
			}
		}
		else
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] type, type:%u", __FUNCTION__, __LINE__, extern_data->player_id, type);
			break;
		}


		if (player->is_on_horse())
		{
			ret = ERROR_ID_NOT_IN_FISHING_REGION;
			LOG_ERR("[%s:%d] player[%lu] is on horse", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}


		uint32_t bait_id = player->data->fishing_bait_id;
		FishingTable *bait_config = get_config_by_id(bait_id, &fishing_config);
		if (!bait_config)
		{
			ret = 190500418;
			LOG_ERR("[%s:%d] player[%lu] bait error, bait_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, bait_id);
			break;
		}
		
		//检查鱼饵
		uint32_t bait_item_id = bait_config->Stosh;
		uint32_t bait_item_num = 1;
		uint32_t bait_bag_num = player->get_item_can_use_num(bait_item_id);
		if (bait_bag_num < bait_item_num)
		{
			ret = 190500418;
			LOG_ERR("[%s:%d] player[%lu] item not enough, item_id:%u, need_num:%u, has_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, bait_id, bait_item_num, bait_bag_num);
			break;
		}

		bool reward_limit = false;
		uint32_t drop_id = bait_config->Drop;
		if (type == 1)
		{
			uint32_t total_times = get_activity_reward_time(activity_id);
			if (player->data->fishing_reward_num >= total_times)
			{
				reward_limit = true;
				drop_id = bait_config->Drop1;
			}
		}
		if (get_drop_item(drop_id, reward_map) != 0)
		{
			ret = ERROR_ID_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get drop item failed, drop_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, drop_id);
			break;
		}

		if (!player->check_can_add_item_list(reward_map))
		{
			ret = ERROR_ID_BAG_GRID_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] bag full, drop_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, drop_id);
			break;
		}

		player->del_item(bait_item_id, bait_item_num, MAGIC_TYPE_FISHING);
		if (reward_map.size() > 0)
		{
			player->add_item_list(reward_map, MAGIC_TYPE_FISHING);
		}

		if (type == 1 && !reward_limit)
		{
			player->data->fishing_reward_num++;
			player->check_activity_progress(AM_FISHING, 0);
		}
		if (type == 2 && task && cond_config)
		{
			player->add_task_progress(TCT_FISHING, bait_id, 1, task->id, cond_config->ID);
		}
	} while(0);
	
	FishingRewardAnswer resp;
	fishing_reward_answer__init(&resp);

	ItemData  item_data[5];
	ItemData* item_point[5];
	
	resp.result = ret;
	resp.rewards = item_point;
	resp.n_rewards = 0;
	for (std::map<uint32_t, uint32_t>::iterator iter = reward_map.begin(); iter != reward_map.end() && resp.n_rewards < 5; ++iter)
	{
		item_point[resp.n_rewards] = &item_data[resp.n_rewards];
		item_data__init(&item_data[resp.n_rewards]);
		item_data[resp.n_rewards].id = iter->first;
		item_data[resp.n_rewards].num = iter->second;
		++resp.n_rewards;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_FISHING_REWARD_ANSWER, fishing_reward_answer__pack, resp);

	return 0;
}

static int handle_fishing_set_bait_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	FishingSetBaitRequest *req = fishing_set_bait_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t bait_id = req->baitid;

	fishing_set_bait_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		FishingTable *config = get_config_by_id(bait_id, &fishing_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get fishing config failed, bait_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, bait_id);
			break;
		}

		player->data->fishing_bait_id = bait_id;
	} while(0);
	
	FishingSetBaitAnswer resp;
	fishing_set_bait_answer__init(&resp);

	resp.result = ret;
	resp.baitid = player->data->fishing_bait_id;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_FISHING_SET_BAIT_ANSWER, fishing_set_bait_answer__pack, resp);

	return 0;
}

static int handle_fishing_begin_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	if (!player->data->fishing_during)
	{
		player->add_fishing_buff();
		player->data->fishing_during = true;
	}

	return 0;
}

static int handle_fishing_end_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	player->del_fishing_buff();
	player->data->fishing_during = false;

	return 0;
}

static int notify_strong_info(player_struct *player, EXTERN_DATA *extern_data)
{
	StrongInfoNotify nty;
	strong_info_notify__init(&nty);

	StrongGoalData  goal_data[MAX_STRONG_GOAL_NUM];
	StrongGoalData* goal_point[MAX_STRONG_GOAL_NUM];
	StrongChapterData  chapter_data[MAX_STRONG_CHAPTER_NUM];
	StrongChapterData* chapter_point[MAX_STRONG_CHAPTER_NUM];

	nty.goals = goal_point;
	nty.n_goals = 0;
	for (int i = 0; i < MAX_STRONG_GOAL_NUM; ++i)
	{
		if (player->data->strong_goals[i].id == 0)
		{
			break;
		}

		if (!strong_goal_is_open(player->data->strong_goals[i].id, player->get_level()))
		{
			continue;
		}

		goal_point[nty.n_goals] = &goal_data[nty.n_goals];
		strong_goal_data__init(&goal_data[nty.n_goals]);
		goal_data[nty.n_goals].id = player->data->strong_goals[i].id;
		goal_data[nty.n_goals].progress = player->data->strong_goals[i].progress;
		goal_data[nty.n_goals].state = player->data->strong_goals[i].state;
		nty.n_goals++;
	}

	nty.chapters = chapter_point;
	nty.n_chapters = 0;
	for (int i = 0; i < MAX_STRONG_CHAPTER_NUM; ++i)
	{
		if (player->data->strong_chapters[i].id == 0)
		{
			break;
		}

		chapter_point[nty.n_chapters] = &chapter_data[nty.n_chapters];
		strong_chapter_data__init(&chapter_data[nty.n_chapters]);
		chapter_data[nty.n_chapters].id = player->data->strong_chapters[i].id;
		chapter_data[nty.n_chapters].progress = player->data->strong_chapters[i].progress;
		chapter_data[nty.n_chapters].state = player->data->strong_chapters[i].state;
		nty.n_chapters++;
	}

	nty.endtime = player->data->create_time + sg_strong_function_time;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_STRONG_INFO_NOTIFY, strong_info_notify__pack, nty);

	return 0;
}

static int handle_strong_goal_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	StrongGoalRewardRequest *req = strong_goal_reward_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t goal_id = req->goalid;

	strong_goal_reward_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		if (!player->strong_function_open())
		{
			ret = ERROR_ID_FUNCTION_NOT_OPEN;
			LOG_ERR("[%s:%d] player[%lu] function, goal_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, goal_id);
			break;
		}

		GrowupTable *config = get_config_by_id(goal_id, &strong_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get goal config failed, goal_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, goal_id);
			break;
		}

		StrongGoalInfo *info = player->get_strong_goal_info(goal_id);
		if (!info)
		{
			ret = ERROR_ID_STRONG_GOAL_ID;
			LOG_ERR("[%s:%d] player[%lu] get goal info failed, goal_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, goal_id);
			break;
		}

		if (info->state != Strong_State_Achieved)
		{
			ret = ERROR_ID_STRONG_GOAL_STATE;
			LOG_ERR("[%s:%d] player[%lu] state, goal_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, goal_id);
			break;
		}

		for (uint32_t i = 0; i < config->n_RewardType; ++i)
		{
			switch (config->RewardType[i])
			{
				case 1: //经验
					player->add_exp(config->RewardValue[i], MAGIC_TYPE_STRONG_GOAL);
					break;
				case 2: //银两
					player->add_coin(config->RewardValue[i], MAGIC_TYPE_STRONG_GOAL);
					break;
				case 3: //元宝
					player->add_bind_gold(config->RewardValue[i], MAGIC_TYPE_STRONG_GOAL);
					break;
			}

		}

		info->state = Strong_State_Rewarded;
		player->strong_goal_update_notify(info);

	} while(0);
	
	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_STRONG_GOAL_REWARD_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int handle_strong_chapter_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	StrongChapterRewardRequest *req = strong_chapter_reward_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t chapter_id = req->chapterid;

	strong_chapter_reward_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		if (!player->strong_function_open())
		{
			ret = ERROR_ID_FUNCTION_NOT_OPEN;
			LOG_ERR("[%s:%d] player[%lu] function, chapter_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, chapter_id);
			break;
		}

		GrowupTable *config = get_config_by_id(chapter_id, &sg_strong_chapter_reward);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get chapter config failed, chapter_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, chapter_id);
			break;
		}

		StrongChapterInfo *info = player->get_strong_chapter_info(chapter_id);
		if (!info)
		{
			ret = ERROR_ID_STRONG_CHAPTER_ID;
			LOG_ERR("[%s:%d] player[%lu] get chapter info failed, chapter_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, chapter_id);
			break;
		}

		if (info->state != Strong_State_Achieved)
		{
			ret = ERROR_ID_STRONG_CHAPTER_STATE;
			LOG_ERR("[%s:%d] player[%lu] state, chapter_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, chapter_id);
			break;
		}

		uint32_t item_id = config->Reward;
		uint32_t item_num = 1;
		ret = player->check_can_add_item(item_id, item_num, NULL);
		if(ret != 0)
		{
			LOG_ERR("[%s:%d] player[%lu] bag space, chapter_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, chapter_id);
			break;
		}

		info->state = Strong_State_Rewarded;
		player->strong_chapter_update_notify(info);

		player->add_item(item_id, item_num, MAGIC_TYPE_STRONG_CHAPTER);
	} while(0);
	
	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_STRONG_CHAPTER_REWARD_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int get_trade_on_shelf_fee(uint32_t price)
{
	//上架花费是按照出售价格的1%来收取``最低500,最高100000
	uint32_t need_coin = price * sg_on_shelf_fee_percent;
	if (need_coin < 500)
	{
		need_coin = 500;
	}
	else if (need_coin > 100000)
	{
		need_coin = 100000;
	}

	return need_coin;
}

//交易道具上架请求
static int handle_trade_on_shelf_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	TradeOnShelfRequest *req = trade_on_shelf_request__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	uint32_t pos = req->bagindex;
	uint32_t num = req->num;
	uint32_t price = req->price;

	trade_on_shelf_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		if (!(pos < player->data->bag_grid_num))
		{
			ret = ERROR_ID_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] bag pos error, pos:%u, grid_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, pos, player->data->bag_grid_num);
			break;
		}

		bag_grid_data& grid = player->data->bag[pos];
		uint32_t trade_id = item_id_to_trade_id(grid.id);
		if (trade_id == 0)
		{
			ret = ERROR_ID_TRADE_ITEM_CANT_TRADE;
			LOG_ERR("[%s:%d] player[%lu] item can't trade, id:%u", __FUNCTION__, __LINE__, extern_data->player_id, grid.id);
			break;
		}

		if (num > grid.num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] prop num error, num:%u, bag_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, num, grid.num);
			break;
		}

		//上架费用
		uint32_t need_coin = get_trade_on_shelf_fee(price);
		uint32_t has_coin = player->get_coin();
		if (has_coin < need_coin)
		{
			ret = 190500454;
			LOG_ERR("[%s:%d] player[%lu] coin, has:%u, need:%u", __FUNCTION__, __LINE__, extern_data->player_id, has_coin, need_coin);
			break;
		}

		{
			TRADE_ON_SHELF_REQUEST *trade_req = (TRADE_ON_SHELF_REQUEST*)get_send_data();
			memset(trade_req, 0, sizeof(TRADE_ON_SHELF_REQUEST));
			trade_req->bag_index = pos;
			trade_req->num = num;
			trade_req->price = price;
			trade_req->fee = need_coin;
			trade_req->trade_id = trade_id;
			memcpy(&trade_req->especial, &grid.especial_item, sizeof(EspecialItemInfo));

			fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_TRADE_ON_SHELF_REQUEST, sizeof(TRADE_ON_SHELF_REQUEST), 0);
		}
	} while(0);
	
	if (ret != 0)
	{
		CommAnswer resp;
		comm_answer__init(&resp);

		resp.result = ret;

		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRADE_ON_SHELF_ANSWER, comm_answer__pack, resp);
	}

	return 0;
}

//交易上架要求扣除道具请求
static int handle_trade_on_shelf_delete_item_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	TRADE_ON_SHELF_DELETE_ITEM_REQUEST *req = (TRADE_ON_SHELF_DELETE_ITEM_REQUEST *)get_data();

	int ret = 0;
	do
	{
		if (!(req->bag_index < player->data->bag_grid_num))
		{
			ret = ERROR_ID_BAG_POS;
			LOG_ERR("[%s:%d] player[%lu] bag pos error, pos:%u, grid_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, req->bag_index, player->data->bag_grid_num);
			break;
		}

		bag_grid_data& grid = player->data->bag[req->bag_index];
		if (req->num > grid.num)
		{
			ret = ERROR_ID_PROP_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] prop num error, num:%u, bag_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, req->num, grid.num);
			break;
		}

		uint32_t has_coin = player->get_coin();
		if (has_coin < req->fee)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin, has:%u, need:%u", __FUNCTION__, __LINE__, extern_data->player_id, has_coin, req->fee);
			break;
		}

		player->del_item_by_pos(req->bag_index, req->num, MAGIC_TYPE_TRADE_ON_SHELF);
		player->sub_coin(req->fee, MAGIC_TYPE_TRADE_ON_SHELF);
		send_trade_statis(extern_data->player_id, MAGIC_TYPE_TRADE_ON_SHELF, req->shelf_index, req->bag_index, req->trade_id, grid.id, req->num, req->fee);
	} while(0);
	
	TRADE_ON_SHELF_DELETE_ITEM_ANSWER *resp = (TRADE_ON_SHELF_DELETE_ITEM_ANSWER*)get_send_data();
	memset(resp, 0, sizeof(TRADE_ON_SHELF_DELETE_ITEM_ANSWER));
	resp->result = ret;
	resp->shelf_index = req->shelf_index;

	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_TRADE_ON_SHELF_DELETE_ITEM_ANSWER, sizeof(TRADE_ON_SHELF_DELETE_ITEM_ANSWER), 0);

	return 0;
}

//交易下架要求把道具放回背包请求
static int handle_trade_off_shelf_add_item_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	TRADE_OFF_SHELF_ADD_ITEM_REQUEST *req = (TRADE_OFF_SHELF_ADD_ITEM_REQUEST *)get_data();

	int ret = 0;
	do
	{
		uint32_t item_id = trade_id_to_item_id(req->trade_id);
		if (item_id == 0)
		{
			ret = ERROR_ID_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] trade id to item id, trade_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, req->trade_id);
			break;
		}

		ret = player->move_trade_item_to_bag(item_id, req->num, req->especial);
		send_trade_statis(extern_data->player_id, MAGIC_TYPE_TRADE_OFF_SHELF, req->shelf_index, req->trade_id, item_id, req->num);
	} while(0);
	
	TRADE_OFF_SHELF_ADD_ITEM_ANSWER *resp = (TRADE_OFF_SHELF_ADD_ITEM_ANSWER*)get_send_data();
	memset(resp, 0, sizeof(TRADE_OFF_SHELF_ADD_ITEM_ANSWER));
	resp->result = ret;
	resp->shelf_index = req->shelf_index;

	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_TRADE_OFF_SHELF_ADD_ITEM_ANSWER, sizeof(TRADE_OFF_SHELF_ADD_ITEM_ANSWER), 0);

	return 0;
}

//交易重新上架要求改变请求
static int handle_trade_re_shelf_change_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	TRADE_RE_SHELF_CHANGE_REQUEST *req = (TRADE_RE_SHELF_CHANGE_REQUEST *)get_data();

	int ret = 0;
	do
	{
		//上架费用
		uint32_t need_coin = get_trade_on_shelf_fee(req->price);
		uint32_t has_coin = player->get_coin();
		if (has_coin < need_coin)
		{
			ret = ERROR_ID_COIN_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] coin, has:%u, need:%u", __FUNCTION__, __LINE__, extern_data->player_id, has_coin, need_coin);
			break;
		}

		uint32_t item_id = trade_id_to_item_id(req->trade_id);
		if (item_id == 0)
		{
			ret = ERROR_ID_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] trade id to item id, trade_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, req->trade_id);
			break;
		}

		if (req->off_num > 0)
		{
			ret = player->check_can_add_item(item_id, req->off_num, NULL);
			if(ret != 0)
			{
				LOG_ERR("[%s:%d] player[%lu] bag space, item_id:%u, num:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id, req->off_num);
				break;
			}
		}

		player->sub_coin(need_coin, MAGIC_TYPE_TRADE_RE_SHELF);
		if (req->off_num > 0)
		{
			player->move_trade_item_to_bag(item_id, req->off_num, req->especial);
		}
		send_trade_statis(extern_data->player_id, MAGIC_TYPE_TRADE_RE_SHELF, req->shelf_index, req->trade_id, item_id, req->off_num, need_coin);
	} while(0);
	
	TRADE_RE_SHELF_CHANGE_ANSWER *resp = (TRADE_RE_SHELF_CHANGE_ANSWER*)get_send_data();
	uint32_t data_size = sizeof(TRADE_RE_SHELF_CHANGE_ANSWER);
	memset(resp, 0, data_size);
	resp->result = ret;
	resp->shelf_index = req->shelf_index;
	resp->num = req->num;
	resp->price = req->price;

	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_TRADE_RE_SHELF_CHANGE_ANSWER, data_size, 0);

	return 0;
}

//交易服请求扣除消耗
static int handle_tradesrv_check_and_cost_request(player_struct *player, EXTERN_DATA *extern_data)
{
	return handle_srv_check_and_cost_request(player, extern_data, SERVER_PROTO_TRADESRV_COST_ANSWER);
}

//交易购买执行请求
static int handle_trade_buy_execute_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	TRADE_BUY_EXECUTE_REQUEST *req = (TRADE_BUY_EXECUTE_REQUEST *)get_data();

	int ret = 0;
	do
	{
		//购买费用
		uint32_t need_unbind_gold = req->buy_price * req->buy_num;
		uint32_t has_unbind_gold = player->get_attr(PLAYER_ATTR_GOLD);
		if (has_unbind_gold < need_unbind_gold)
		{
			ret = ERROR_ID_GOLD_NOT_ENOUGH;
			LOG_ERR("[%s:%d] player[%lu] unbind gold, has:%u, need:%u", __FUNCTION__, __LINE__, extern_data->player_id, has_unbind_gold, need_unbind_gold);
			break;
		}

		uint32_t item_id = trade_id_to_item_id(req->trade_id);
		if (item_id == 0)
		{
			ret = ERROR_ID_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] trade id to item id, trade_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, req->trade_id);
			break;
		}

		//购买后的道具变成绑定的
		uint32_t bind_id = 0;
		if (get_item_bind_and_unbind_id(item_id, &bind_id, NULL) != 0)
		{
			ret = ERROR_ID_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get item bind id, item_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id);
			break;
		}

		ret = player->check_can_add_item(bind_id, req->buy_num, NULL);
		if(ret != 0)
		{
			LOG_ERR("[%s:%d] player[%lu] bag space, item_id:%u, num:%u", __FUNCTION__, __LINE__, extern_data->player_id, item_id, req->buy_num);
			break;
		}

		player->sub_unbind_gold(need_unbind_gold, MAGIC_TYPE_TRADE_BUY);
		player->move_trade_item_to_bag(bind_id, req->buy_num, req->especial);
		send_trade_statis(extern_data->player_id, MAGIC_TYPE_TRADE_BUY, req->shelf_index, req->trade_id, bind_id, req->buy_num, need_unbind_gold, req->seller_id);
	} while(0);
	
	TRADE_BUY_EXECUTE_ANSWER *resp = (TRADE_BUY_EXECUTE_ANSWER*)get_send_data();
	uint32_t data_len = sizeof(TRADE_BUY_EXECUTE_ANSWER);
	memset(resp, 0, data_len);
	resp->result = ret;
	resp->seller_id = req->seller_id;
	resp->shelf_index = req->shelf_index;
	resp->buy_num = req->buy_num;

	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_TRADE_BUY_EXECUTE_ANSWER, data_len, 0);

	return 0;
}

//交易领取收益发放请求
static int handle_trade_get_earning_give_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint32_t *req = (uint32_t *)get_data();
	uint32_t give_unbind_gold = *req;

	player->add_unbind_gold(give_unbind_gold, MAGIC_TYPE_TRADE_SOLD_EARNING);

	return 0;
}

//拍卖失败返还
static int handle_trade_bid_fail_return(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint32_t *req = (uint32_t *)get_data();
	uint32_t return_unbind_gold = *req;

	player->add_unbind_gold(return_unbind_gold, MAGIC_TYPE_AUCTION_BID_FAIL);

	return 0;
}

static int handle_guild_ruqin_creat_monster_level_answer(player_struct * /*player*/, EXTERN_DATA * /*extern_data*/)
{
	PROTO_HEAD_RUQIN *req = (PROTO_HEAD_RUQIN*)buf_head();
	uint32_t guild_id = req->guild_id;
	uint32_t level   =  req->level;
	std::map<uint32_t, guild_land_raid_struct *>::iterator guild =  guild_land_raid_manager_raid_map.find(guild_id);
	if(guild == guild_land_raid_manager_raid_map.end())
	{
		LOG_ERR("帮会入侵活动设置刷怪等级失败,无此帮会guild_id[%u]", guild_id);
		return -1;
	}
	guild->second->ruqin_data.level = level;
	return 0;
}

static int handle_raid_player_enter_request(player_struct */*player*/, EXTERN_DATA *extern_data)
{
	uint64_t player_id = extern_data->player_id;
	player_struct* player = player_manager::get_player_by_id(player_id);
	if (player)
	{
		LOG_ERR("[%s:%d] player_id[%lu]", __FUNCTION__, __LINE__, player_id);
		return (0);
	}

	RAID_ENTER_REQUEST *req = (RAID_ENTER_REQUEST*)conn_node_gamesrv::connecter.get_data();

	LOG_INFO("[%s:%d] player[%lu] raid[%u]", __FUNCTION__, __LINE__, extern_data->player_id, req->raid_id);
	
	player = player_manager::create_player(req, extern_data->player_id);
	if (!player)
	{
		LOG_ERR("%s %d: can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}
	raid_struct *raid = raid_manager::create_raid(req->raid_id, player);
	if (!raid)
	{
		LOG_ERR("%s: player[%lu] create raid[%u] failed", __FUNCTION__, extern_data->player_id, req->raid_id);
		return (-20);
	}
	int x = raid->res_config->BirthPointX, z = raid->res_config->BirthPointZ;
	double direct = 0;
	if (raid->m_config->DengeonRank == DUNGEON_TYPE_BATTLE_NEW)
	{
		BattlefieldTable *table = zhenying_fight_config.begin()->second;
		if (table != NULL)
		{
			ZhenyingBattle::GetInstance()->GetRelivePos(table, player->get_attr(PLAYER_ATTR_ZHENYING), &x, &z, &direct);
		}
	}
	raid->player_enter_raid(player, x, z, direct);
	return (0);
}
static int handle_raid_team_enter_request(player_struct *player, EXTERN_DATA *extern_data)
{
	
	return (0);
}
static int handle_raid_leave_request(player_struct *player, EXTERN_DATA *extern_data)
{
	player->set_out_raid_pos_and_clear_scene();
	if (player->data->scene_id <= SCENCE_DEPART)
	{
		struct position *pos = player->get_pos();
		uint32_t scene_id = player->data->scene_id;

		scene_struct *new_scene = scene_manager::get_scene(scene_id);
		if (!new_scene)
		{
			LOG_ERR("%s %d: player[%lu] transfer to the wrong scene[%u]", __FUNCTION__, __LINE__, player->data->player_id, scene_id);
			return (-30);
		}
		new_scene->player_enter_scene(player, pos->pos_x, player->data->pos_y, pos->pos_z, player->data->leaveraid.direct);		
	}
	else
	{
		DungeonTable *config = get_config_by_id(player->data->scene_id, &all_raid_config);
		if (config)
			player->move_to_raid_impl(config, true);
	}
	return (0);
}
static int handle_attr_changed_request(player_struct *player, EXTERN_DATA *extern_data)
{
	ATTR_CHANGED_REQUEST *req = (ATTR_CHANGED_REQUEST*)conn_node_gamesrv::connecter.buf_head();		
	player->set_attr(req->id, req->value);
	return (0);
}

static int handle_play_drama_begin_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	player->data->playing_drama = true;
	raid_struct *raid = player->get_raid();
	if (raid)
	{
		raid->stop_monster_ai();
	}
	else if (player->sight_space)
	{
		player->sight_space->stop_monster_ai();
	}

	return 0;
}

static int handle_play_drama_end_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	player->data->playing_drama = false;
	raid_struct *raid = player->get_raid();
	if (raid)
	{
		raid->start_monster_ai();
		if (raid->ai && raid->ai->raid_on_play_drama_end)
		{
			raid->ai->raid_on_play_drama_end(raid);
		}
	}
	else if (player->sight_space)
	{
		player->sight_space->start_monster_ai();
	}

	return 0;
}

static int handle_travel_task_accept_request(player_struct *player, EXTERN_DATA *extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	int ret = 0;
	do
	{
		if (player->get_travel_task() != 0)
		{
			ret = 1;
			LOG_ERR("[%s:%d] player[%lu] doing task.", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (player->data->travel_round_num >= sg_travel_round_amount)
		{
			ret = 2;
			LOG_ERR("[%s:%d] player[%lu] travel task all done today.", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		uint32_t task_id = player->generate_next_travel_task();
		if (task_id == 0)
		{
			ret = 3;
			LOG_ERR("[%s:%d] player[%lu] generate travel task failed.", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		ret = player->accept_task(task_id, false);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_TRAVEL_TASK_ACCEPT_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int handle_level_reward_info_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	player->player_level_reward_info_notify();
	return 0;
}
static int handle_recieve_level_reward_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	PlayerReceiveLevelRewardRequest* req = player_receive_level_reward_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(!req)
	{
		LOG_ERR("[%s:%d] player receive level reward request unpack faild player_di[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t reward_id = req->id;
	player_receive_level_reward_request__free_unpacked(req, NULL);

	int ret = 0;
	do{

		PlayerLevelReward *reward_info = NULL;
		for(size_t i = 0; i < MAX_PLAYER_LEVEL_REWARD_NUM; i++)
		{
			if(player->data->my_level_reward[i].id == reward_id)
			{
				reward_info = &player->data->my_level_reward[i];
				break;
			}
		}

		if(reward_info == NULL)
		{
			LOG_ERR("[%s:%d] receive level reward faild, not find reward_id, player_id[%lu], reward_id[%u]", __FUNCTION__, __LINE__, extern_data->player_id, reward_id);
			return -3;
		}
		
		LevelReward* level_config = get_config_by_id(reward_id, &level_reward_config);
		if(level_config == NULL)
		{
			LOG_ERR("[%s:%d] receive level reward faild, config error, player_id[%lu], reward_id[%u]", __FUNCTION__, __LINE__, extern_data->player_id, reward_id);
			return -4;
		}

		if(reward_info->receive == true)
		{
			ret = 190500396;   //奖励已经领取
			break;
		}

		if(player->get_level() < level_config->Level)
		{
			ret = 190500480; //等级没到
			break;
		}
		std::map<uint32_t, uint32_t> item_map;
		for(size_t i = 0; i < level_config->n_ItemID && i < level_config->n_ItemValue; i++)
		{
			item_map[level_config->ItemID[i]] += level_config->ItemValue[i];
		}

		if (item_map.size() > 0 && !player->check_can_add_item_list(item_map))
		{
			ret = 190500315; //包裹空位不足
			LOG_ERR("[%s:%d] receive level reward faild, config error, player_id[%lu], reward_id[%u]", __FUNCTION__, __LINE__, extern_data->player_id, reward_id);
			break;
		}

		reward_info->receive = true;
		if (item_map.size() > 0)
		{
			player->add_item_list(item_map, MAGIC_TYPE_RECIEVE_LEVVL_REWARD);
		}

	}while(0);
	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_RECEIVE_LEVEL_REWARD_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int handle_online_reward_info_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	player->player_online_reward_info_notify();
	return 0;
}

static int handle_recieve_online_zhuanpan_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	uint32_t online_time = player->data->online_reward.befor_online_time + (time_helper::get_cached_time() / 1000 - player->data->online_reward.sign_time); //当日在线总时长
	uint32_t config_time = 0;
	uint32_t sun_num = 0;            //今日到目前为止可领奖次数(包过已经领取了的)
	uint32_t surplus_num = 0;        //今日剩余可领奖次数
	for(std::map<uint64_t, OnlineTimes*>::iterator itr = online_time_config.begin(); itr != online_time_config.end(); itr++)
	{
		config_time += itr->second->Times;
		if(online_time < config_time)
			break;
		sun_num++;
	}

	if(sun_num <= player->data->online_reward.use_reward_num || config_time == 0)
	{
		LOG_ERR("[%s:%d] online reward receive error, sun num > use num, sun num[%u], use_num[%u] configtime[%u]",
			__FUNCTION__, __LINE__, sun_num, player->data->online_reward.use_reward_num, config_time);
		return -2;
	}
	surplus_num = sun_num - player->data->online_reward.use_reward_num;
	if(surplus_num <= 0)
	{
		LOG_ERR("[%s:%d] 可领奖次数不够", __FUNCTION__, __LINE__);
		return -3;
	}

	int ret = 0;
	do{
		if(player->data->online_reward.reward_id == 0)
		{
			uint32_t reward[MAX_PLAYER_ONLINE_REWARD_NUM] = {0};
			uint32_t reward_gailv[MAX_PLAYER_ONLINE_REWARD_NUM] = {0};
			uint32_t n_reward = 0;
			uint32_t use_gailv = 0;
			//for(std::map<uint64_t, TimeReward*>::iterator ite = online_reward_config.begin(); ite != online_reward_config.end(); ite++)
			for(size_t i = 0; i < MAX_PLAYER_ONLINE_REWARD_NUM; i++)
			{
				if(player->data->online_reward.reward_id_today[i] == 0)
					break;
				TimeReward* time_reward_config = get_config_by_id(player->data->online_reward.reward_id_today[i], &online_reward_config);
				if(time_reward_config == NULL)
				{
					LOG_ERR("[%s:%d] 获取奖励找回配置失败? TimeReward表id[%u]", __FUNCTION__, __LINE__, player->data->online_reward.reward_id_today[i]);
					continue;
				}
				bool flag = false;
				for(size_t j = 0; j < MAX_PLAYER_ONLINE_REWARD_NUM; j++)
				{
					if(player->data->online_reward.reward_table_id[j] == 0)
						break;
					if(player->data->online_reward.reward_id_today[i] == player->data->online_reward.reward_table_id[j])
					{
						flag = true;
						use_gailv += time_reward_config->Probability;
						break;
					}
				}
				if(flag == false)
				{
					reward[n_reward] = time_reward_config->ID;
					reward_gailv[n_reward] = time_reward_config->Probability;
					n_reward++;
				}

			}
			if(n_reward <= 0)
			{
				LOG_ERR("[%s:%d] TimeReward 表的物品个数小于在线奖励次数", __FUNCTION__, __LINE__);
				return -4;
			}
			uint32_t j = 0;
			uint32_t k = 0;
			uint32_t tmp = reward_gailv[0];
			//计算出还没被领取的物品中的最高概率的物品,将已领取了的物品的概率总和加到上面，以保持概率的百分百
			for(; j< MAX_PLAYER_ONLINE_REWARD_NUM && j < n_reward; j++ )
			{
				if(reward[j] == 0)
					break;
				if(tmp < reward_gailv[j])
				{
					tmp = reward_gailv[j];
					k = j;
				}

			}
			reward_gailv[k] += use_gailv;
			uint32_t sum_gailv = 0;
			for(size_t m = 0; m< MAX_PLAYER_ONLINE_REWARD_NUM && m < n_reward; m++)
			{
				sum_gailv += reward_gailv[m];
			}

			//在剩余没有被领取的物品中选出要给的物品
			sum_gailv = rand() %  sum_gailv + 1;
			int flag_gailv = -1;
			uint32_t gailv_begin = 0;
			uint32_t gailv_end = 0;
			for(size_t p = 0; p < n_reward && p < MAX_PLAYER_ONLINE_REWARD_NUM; ++p)
			{
				gailv_end = gailv_begin + reward_gailv[p];
				if(sum_gailv > gailv_begin && sum_gailv <= gailv_end)
				{
					flag_gailv = p;
					break;	
				}
				gailv_begin = gailv_end;
			}

			if(flag_gailv < 0 || flag_gailv >= (int)n_reward)
			{
				LOG_ERR("[%s:%d] 在线领奖获取概率物品出错", __FUNCTION__, __LINE__);
				return -5;
			}
			if(reward[flag_gailv] == 0)
			{
				LOG_ERR("[%s:%d] 在线奖励获取奖励物品出错", __FUNCTION__, __LINE__);
				return -6;
			}
			player->data->online_reward.reward_id = reward[flag_gailv];
		}
		TimeReward* reward_config = get_config_by_id(player->data->online_reward.reward_id, &online_reward_config);
		if(reward_config == NULL)
		{
			LOG_ERR("[%s:%d] 在线奖励配置表出错, TimeReward表id[%u]", __FUNCTION__, __LINE__, player->data->online_reward.reward_id);
			return -7;
		}
		ret = player->check_can_add_item(reward_config->ItemID, reward_config->ItemValue, NULL);
		if(ret != 0)
		{
			if(ret == ERROR_ID_BAG_GRID_NOT_ENOUGH)
			{
				ret = 190500315; //包裹数量不足
				break;
			}
		}

	}while(0);
	PlayerOnlineRewardZhuanAnswer answer;
	player_online_reward_zhuan_answer__init(&answer);
	answer.result = ret;
	answer.reward_id = player->data->online_reward.reward_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ONLINE_REWARD_ZHUAN_ANSWER, player_online_reward_zhuan_answer__pack, answer);

	return 0;
}

int handle_recieve_online_reward_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	uint32_t online_time = player->data->online_reward.befor_online_time + (time_helper::get_cached_time() / 1000 - player->data->online_reward.sign_time); //当日在线总时长
	uint32_t config_time = 0;
	uint32_t sun_num = 0;            //今日到目前为止可领奖次数(包过已经领取了的)
	uint32_t surplus_num = 0;        //今日剩余可领奖次数
	for(std::map<uint64_t, OnlineTimes*>::iterator itr = online_time_config.begin(); itr != online_time_config.end(); itr++)
	{
		config_time += itr->second->Times;
		if(online_time < config_time)
			break;
		sun_num++;
	}

	if(sun_num < player->data->online_reward.use_reward_num || config_time == 0)
	{
		LOG_ERR("[%s:%d] online reward receive error, sun num > use num, sun num[%u], use_num[%u] config_time[%u]", __FUNCTION__, __LINE__, sun_num, player->data->online_reward.use_reward_num, config_time);
		return -2;
	}
	surplus_num = sun_num - player->data->online_reward.use_reward_num;
	if(surplus_num <= 0)
	{
		LOG_ERR("[%s:%d] 可领奖次数不够", __FUNCTION__, __LINE__);
		return -3;
	}
	

	TimeReward* reward_config = get_config_by_id(player->data->online_reward.reward_id, &online_reward_config);
	if(reward_config == NULL)
	{
		LOG_ERR("[%s:%d] 在线奖励配置表出错, TimeReward 表id[%u]", __FUNCTION__, __LINE__, player->data->online_reward.reward_id);
		return -4;
	}

	int ret = 0;
	do 
	{
		uint32_t item_id = reward_config->ItemID;
		uint32_t item_num = reward_config->ItemValue;
		if(item_id == 0 || item_num == 0)
		{
			LOG_ERR("[%s:%d] 在线奖励领奖失败,获取物品id或者物品数量失败 item_id[%u] item_num[%u]", __FUNCTION__, __LINE__, item_id, item_num);
			return -6;
		}
		ret = player->check_can_add_item(item_id, item_num, NULL);
		if(ret != 0)
		{
			if(ret == ERROR_ID_BAG_GRID_NOT_ENOUGH)
			{
				ret = 190500315; //包裹数量不足
				break;
			}
		}

		//扣次数，给物品
		player->data->online_reward.use_reward_num++;
		for(size_t i =0; i < MAX_PLAYER_ONLINE_REWARD_NUM; i++)
		{
			if(player->data->online_reward.reward_table_id[i] == 0)
			{
				player->data->online_reward.reward_table_id[i] = player->data->online_reward.reward_id;
				break;
			}
		}
		player->data->online_reward.reward_id = 0;
		player->add_item(item_id, item_num, MAGIC_TYPE_RECIEVE_ONLINE_REWARD);

	}while(0);

	PlayerOnlineReceiveRewardAnswer answer;
	player_online_receive_reward_answer__init(&answer);
	PlayerOnlineRewardInfoNotify notify;
	player_online_reward_info_notify__init(&notify);


	notify.can_use_num = sun_num - player->data->online_reward.use_reward_num;

	//在线时长超过领奖的总在线时长就置0
	if(online_time < config_time)
	{
		notify.shengyu_time = time_helper::get_cached_time() / 1000 + (config_time - online_time);
	}
	else 
	{
		notify.shengyu_time = 0;
	}

	uint32_t reward_table_id[MAX_PLAYER_ONLINE_REWARD_NUM];
	uint32_t reward_id_today[MAX_PLAYER_ONLINE_REWARD_NUM];
	memset(reward_table_id, 0, sizeof(uint32_t) * MAX_PLAYER_ONLINE_REWARD_NUM);
	memset(reward_id_today, 0, sizeof(uint32_t) * MAX_PLAYER_ONLINE_REWARD_NUM);
	notify.reward_table_id = reward_table_id;
	notify.reward_id_today = reward_id_today;
	notify.n_reward_table_id = 0;
	notify.n_reward_id_today = 0;
	notify.sigin_time = player->data->online_reward.sign_time;
	for(size_t i = 0; i < MAX_PLAYER_ONLINE_REWARD_NUM; i++)
	{
		if(player->data->online_reward.reward_table_id[i] == 0)
			break;
		reward_table_id[notify.n_reward_table_id] = player->data->online_reward.reward_table_id[notify.n_reward_table_id];
		notify.n_reward_table_id++;
	}
	for(size_t i = 0; i < MAX_PLAYER_ONLINE_REWARD_NUM; i++)
	{
		if(player->data->online_reward.reward_id_today[i] == 0)
			break;
		reward_id_today[notify.n_reward_id_today] = player->data->online_reward.reward_id_today[notify.n_reward_id_today];
		notify.n_reward_id_today++;
	}

	answer.result = ret;
	answer.info = &notify;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_ONLINE_REWARD_RECEIVE_ANSWER, player_online_receive_reward_answer__pack, answer);
	
	return 0;
}

int handle_sign_in_ervery_day_info_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	player->player_signin_reward_info_notify();
	return 0;
}

int handle_player_sign_in_ervery_day_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	int ret = 0;
	do{
		std::map<uint64_t, std::map<uint64_t, struct SignDay*> >::iterator itr_month = sign_day_zhuan_config.find(player->data->sigin_in_data.cur_month);
		if(itr_month == sign_day_zhuan_config.end())
		{
			LOG_ERR("[%s:%d] 每日签到根据月份获取配置出错 month[%u]", __FUNCTION__, __LINE__, player->data->sigin_in_data.cur_month);
			return -2;
		}

		std::map<uint64_t, struct SignDay*> sign_in_config = itr_month->second;
		if(sign_in_config.size() <= 0)
		{
			LOG_ERR("[%s:%d] 每日签到根据月份获取配置出错 month[%u]", __FUNCTION__, __LINE__, player->data->sigin_in_data.cur_month);
			return -3;
		}

		struct SignDay* day_config = get_config_by_id(player->data->sigin_in_data.month_sum + 1, &sign_in_config);
		if(day_config == NULL)
		{
			LOG_ERR("[%s:%d] 每日签到根据天数获取配置出错,本次签到是本月第[%u]此签到", __FUNCTION__, __LINE__, player->data->sigin_in_data.month_sum + 1);
			return -4;

		}
		uint32_t item_id = day_config->ItemID;
		uint32_t item_num = day_config->ItemValue;
		//这里物品数量的计算还要根据vip倍率来，当前没有做vip要预留
		//uint32_t vip_beilv = day_config->VipDouble;
		//先判断包裹是否满了
		ret = player->check_can_add_item(item_id, item_num, NULL);
		if(ret !=0)
		{
			if(ret == ERROR_ID_BAG_GRID_NOT_ENOUGH)
			{
				ret = 190500315; //包裹数量不足
			}
			break;
		}
		
		//当日没签，签当日的
		if(player->data->sigin_in_data.today_sign == false)
		{

			player->data->sigin_in_data.today_sign = true;
			player->data->sigin_in_data.month_sum += 1;
			player->add_item(item_id, item_num, MAGIC_TYPE_GIVE_QIANDAO_REWARD);
			break;
		}

		//补签的话要先判断遗漏，有遗漏才能补签
		if(player->data->sigin_in_data.yilou_sum <= 0)
		{
			LOG_ERR("[%s:%d] 补签失败,本月遗漏次数不够,遗漏[%u]", __FUNCTION__, __LINE__, player->data->sigin_in_data.yilou_sum);
			return -5;
		}

		//有补签次数使用补签次数，无补签次数那就扣元宝
	
		if(player->data->sigin_in_data.buqian_sum > 0)
		{
			

			player->data->sigin_in_data.month_sum += 1;
			player->data->sigin_in_data.yilou_sum -= 1;
			player->data->sigin_in_data.buqian_sum -= 1;
			player->add_item(item_id, item_num, MAGIC_TYPE_GIVE_QIANDAO_REWARD);
			break;
		}

		ParameterTable* param_config = get_config_by_id(161000401, &parameter_config);
		if(param_config == NULL && param_config->n_parameter1 > 0)
		{
			LOG_ERR("[%s:%d] 每日签到消耗元宝,获取配置出错,parame_config id[161000401]", __FUNCTION__, __LINE__);
			return -6;
		}
		uint32_t use_gold_num = param_config->parameter1[0];
		if(player->sub_comm_gold(use_gold_num, MAGIC_TYPE_BUQIAN_XIAOHAO_YUANBAO) < 0)
		{
			ret = 190400005; //元宝不足;
			break;
		}
		player->data->sigin_in_data.month_sum += 1;
		player->data->sigin_in_data.yilou_sum -= 1;
		player->add_item(item_id, item_num, MAGIC_TYPE_GIVE_QIANDAO_REWARD);
	
	}while(0);

	//更新累计奖励信息
	do 
	{
		std::map<uint64_t, std::map<uint64_t, struct SignMonth*> >::iterator itr_leiji = sign_month_zhuan_config.find(player->data->sigin_in_data.cur_month);
		if(itr_leiji == sign_month_zhuan_config.end())
		{
			LOG_ERR("[%s:%d] 每日签到根据月份获取累计奖励配置出错 month[%u]", __FUNCTION__, __LINE__, player->data->sigin_in_data.cur_month);
			break;
		}
		std::map<uint64_t, struct SignMonth*> leiji_reward_config = itr_leiji->second;
		if(leiji_reward_config.size() <= 0)
		{
			LOG_ERR("[%s:%d] 每日签到根据月份获取累计奖励配置出错 month[%u]", __FUNCTION__, __LINE__, player->data->sigin_in_data.cur_month);
			break;
		}
		for(std::map<uint64_t, struct SignMonth*>::iterator itr = leiji_reward_config.begin(); itr != leiji_reward_config.end(); itr++)
		{
			if(player->data->sigin_in_data.month_sum >= itr->second->Days)
			{
				for(size_t i = 0; i < MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM; i++)
				{
					if(player->data->sigin_in_data.grand_reward[i].id == 0)
						break;
					if(player->data->sigin_in_data.grand_reward[i].id != itr->second->ID)
						continue;
					if(player->data->sigin_in_data.grand_reward[i].state == 0)
						player->data->sigin_in_data.grand_reward[i].state = 1;
				}
			}
		}
		
	}while(0);

	PlayerReceiveSignInRewardAnswer answer;
	PlayerSignInEveryDayInfo notify;
	player_receive_sign_in_reward_answer__init(&answer);
	answer.result = ret;
	answer.info = &notify;
	player_sign_in_every_day_info__init(&notify);

	notify.today = player->data->sigin_in_data.today_sign;
	notify.month_sum = player->data->sigin_in_data.month_sum;
	notify.yilou_num = player->data->sigin_in_data.yilou_sum;
	notify.buqian_num = player->data->sigin_in_data.buqian_sum;

	PlayerLeiJiSignInRewardInfo leiji_info[MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM];
	PlayerLeiJiSignInRewardInfo* leiji_info_point[MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM];

	notify.leiji_reward = leiji_info_point;
	notify.n_leiji_reward = 0;
	for(size_t i = 0; i < MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM; i++)
	{
		if(player->data->sigin_in_data.grand_reward[i].id == 0)
				break;
		leiji_info_point[notify.n_leiji_reward] = &leiji_info[notify.n_leiji_reward];
		player_lei_ji_sign_in_reward_info__init(&leiji_info[notify.n_leiji_reward]);  
		leiji_info[notify.n_leiji_reward].id = player->data->sigin_in_data.grand_reward[i].id;
		leiji_info[notify.n_leiji_reward].state = player->data->sigin_in_data.grand_reward[i].state;
		notify.n_leiji_reward++;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SIGNIN_EVERYDAY_REWARD_RECEIVE_ANSWER, player_receive_sign_in_reward_answer__pack, answer);

	return 0;
}

int handle_sign_in_receive_leiji_reward_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	PlayerSignInReceiveLeijiRewardRequest* req = player_sign_in_receive_leiji_reward_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] unpack receive leiji reward request faild", __FUNCTION__, __LINE__);
		return -2;
	}
	uint32_t reward_id = req->id;
	player_sign_in_receive_leiji_reward_request__free_unpacked(req, NULL);
	std::map<uint64_t, std::map<uint64_t, struct SignMonth*> >::iterator itr_leiji = sign_month_zhuan_config.find(player->data->sigin_in_data.cur_month);
	if(itr_leiji == sign_month_zhuan_config.end())
	{
		LOG_ERR("[%s:%d] 领取签到累计奖励根据月份获取累计奖励配置出错 month[%u]", __FUNCTION__, __LINE__, player->data->sigin_in_data.cur_month);
		return -3;
	}
	std::map<uint64_t, struct SignMonth*> leiji_reward_config = itr_leiji->second;
	if(leiji_reward_config.size() <= 0)
	{
		LOG_ERR("[%s:%d] 领取签到累计奖励根据月份获取累计奖励配置出错 month[%u]", __FUNCTION__, __LINE__, player->data->sigin_in_data.cur_month);
		return -4;
	}
	struct SignMonth* leiji_config = get_config_by_id(reward_id, &leiji_reward_config);
	if(leiji_config == NULL)
	{
		LOG_ERR("[%s:%d] 领取累计奖励失败,配置出错", __FUNCTION__, __LINE__);
		return -5;
	}

	int ret = 0;
	do{
		SignInEveryDayCumulative* cumula_info = NULL;
		for(size_t i = 0; i < MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM; i++)
		{
			if(player->data->sigin_in_data.grand_reward[i].id == 0)
				break;
			if(player->data->sigin_in_data.grand_reward[i].id == reward_id)
				cumula_info = &player->data->sigin_in_data.grand_reward[i];
		}
		if(cumula_info == NULL)
		{
			LOG_ERR("[%s:%d] 领取累计奖励失败,玩家身上数据不对?player_di[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
			return -6;
		}

		if(cumula_info->state != 1)
		{
			LOG_ERR("[%s:%d] 奖励不可领或已领取,领取状态 statu : [%u]", __FUNCTION__, __LINE__, cumula_info->state);
			return -7;
		}
		ret = player->check_can_add_item(leiji_config->ItemID, leiji_config->ItemValue, NULL);
		if(ret !=0)
		{
			if(ret == ERROR_ID_BAG_GRID_NOT_ENOUGH)
			{
				ret = 190500315; //包裹数量不足
			}
			break;
		}

		cumula_info->state = 2;
		player->add_item(leiji_config->ItemID, leiji_config->ItemValue, MAGIC_TYPE_GIVE_QIANDAO_LEIJI_REWARD);


	}while(0);
	PlayerSignInReceiveLeijiRewardAnswer answer;
	PlayerLeiJiSignInRewardInfo leiji_info[MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM];
	PlayerLeiJiSignInRewardInfo* leiji_info_point[MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM];
	player_sign_in_receive_leiji_reward_answer__init(&answer);
	answer.result = ret;
	answer.leiji_reward = leiji_info_point;
	answer.n_leiji_reward = 0;
	for(size_t i = 0; i < MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM; i++)
	{
		if(player->data->sigin_in_data.grand_reward[i].id == 0)
				break;
		leiji_info_point[answer.n_leiji_reward] = &leiji_info[answer.n_leiji_reward];
		player_lei_ji_sign_in_reward_info__init(&leiji_info[answer.n_leiji_reward]);  
		leiji_info[answer.n_leiji_reward].id = player->data->sigin_in_data.grand_reward[i].id;
		leiji_info[answer.n_leiji_reward].state = player->data->sigin_in_data.grand_reward[i].state;
		answer.n_leiji_reward++;
	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_SIGNIN_LEIJI_REWARD_RECEIVE_ANSWER, player_sign_in_receive_leiji_reward_answer__pack, answer);

	return 0;
}

int handle_active_receive_zhaohui_info_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	player->player_active_zhaohui_reward_info_notify();
	return 0;
}

int handle_active_receive_zhaohui_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	PlayerRewardZhaoHuiRequest* req = player_reward_zhao_hui_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] player reward zhao hui request unpack faild, player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	uint32_t id = req->id;
	uint32_t statu = req->statu;

	player_reward_zhao_hui_request__free_unpacked(req, NULL);

	ActiveRewardZhaohuiInfo *zhaohui_info = NULL;
	for(size_t i = 0; i < MAX_ACTIVE_CAN_ZHAOHUI_REWARD; i++)
	{
		if(player->data->zhaohui_data[i].id == id)
		{
			zhaohui_info = &player->data->zhaohui_data[i];
		}

	}

	if(zhaohui_info == NULL)
	{
		LOG_ERR("[%s:%d] 请求找回奖励失败 id[%u]", __FUNCTION__, __LINE__, id);
		return -2;
	}

	RewardBack *reward_config = get_config_by_id(id, &reward_back_config);
	if(reward_config == NULL)
	{
		LOG_ERR("[%s:%d] 奖励找回获取配置出错, 表id[%u]", __FUNCTION__, __LINE__, id);
		return -3;
	}
	if(zhaohui_info->num <= 0)
	{
		LOG_ERR("[%s:%d] 奖励找回失败,今日已找回全部次数", __FUNCTION__, __LINE__);
		return -3;
	}

	uint32_t reward_id = 0;
	if(statu ==0)
	{
		reward_id = reward_config->Normal;
	}
	else 
	{
		reward_id = reward_config->Perfect;
	}



	PlayerRewardZhaoHuiAnswer answer;
	player_reward_zhao_hui_answer__init(&answer);
	PlayerRewardZhaoHuiItem zhaohui_item[MAX_ACTIVE_CAN_ZHAOHUI_ITEM_NUM];
	PlayerRewardZhaoHuiItem* zhaohui_item_point[MAX_ACTIVE_CAN_ZHAOHUI_ITEM_NUM];
	answer.n_item_info = 0;
	answer.id = id;
	answer.item_info = zhaohui_item_point;
	int ret = 0;
	do{
	
		std::map<uint32_t, uint32_t> reward_drop;
		get_drop_item(reward_id, reward_drop);
		if (!player->check_can_add_item_list(reward_drop))
		{
			ret = 190500315; //包裹空位不足
			break;
		}

		if(statu ==0)
		{
			//加上额外的银两和经验
			reward_drop[201010001] += zhaohui_info->normo_coin;
			reward_drop[201010004] += zhaohui_info->normo_exp;
			if( player->sub_coin(zhaohui_info->normo_use, MAGIC_TYPE_NORMO_BACK_USE_COIN) < 0)
			{
				ret = 190500100;   //银票数量不够
				break;
			}
		}
		else 
		{
			reward_drop[201010001] += zhaohui_info->perfect_coin;
			reward_drop[201010004] += zhaohui_info->perfect_exp;
			if(player->sub_comm_gold(zhaohui_info->perfect_use, MAGIC_TYPE_PERFECT_BACK_USE_GOLD) < 0)
			{
				ret = 190400005;     //金票数量不够
				break;
			}
		}
		zhaohui_info->num -= 1;
		player->add_item_list(reward_drop, MAGIC_TYPE_REWARD_BACK_FIVE_ITEM);
		
		for(std::map<uint32_t, uint32_t>::iterator itr = reward_drop.begin(); itr != reward_drop.end(); itr++)
		{
			zhaohui_item_point[answer.n_item_info] = &zhaohui_item[answer.n_item_info];
			player_reward_zhao_hui_item__init(&zhaohui_item[answer.n_item_info]);
			zhaohui_item[answer.n_item_info].item_id = itr->first;
			zhaohui_item[answer.n_item_info].item_num = itr->second;
			answer.n_item_info++;
		}

	}while(0);
	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_PLAYER_ACTIVE_REWARD_ZHAOHUI_ANSWER, player_reward_zhao_hui_answer__pack, answer);
	return 0;
}

int handle_reset_tower_cur_level_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	if (player->data->tower.reset_num < 1)
	{
		return 0;
	}
	ParameterTable *table = get_config_by_id(161001004, &parameter_config);
	if (table == NULL)
	{
		return 2;
	}
	--player->data->tower.reset_num;
	player->data->tower.cur_lv = 1;
	player->data->tower.cur_num = table->parameter1[0];

	AnsResetTower send;
	ans_reset_tower__init(&send);
	send.reset_num = player->data->tower.reset_num;
	send.cur_lv = player->data->tower.cur_lv;
	send.cur_num = player->data->tower.cur_num;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_RESET_TOWER_CUR_LEVEL_ANSWER, ans_reset_tower__pack, send);

	return 0;
}
int handle_clean_tower_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	if (player->data->tower.cur_num == 0 || player->data->tower.top_lv == 1)
	{
		return 1;
	}
	ParameterTable *table = get_config_by_id(161001004, &parameter_config);
	if (table == NULL)
	{
		return 2;
	}

	ReqCleanTower* req = req_clean_tower__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if (req == NULL)
	{
		LOG_ERR("[%s:%d] player reward zhao hui request unpack faild, player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return 3;
	}
	int type = req->type;
	req_clean_tower__free_unpacked(req, NULL);

	int ret = 0;
	if (type == 1)
	{
		if (player->del_item(table->parameter1[3], table->parameter1[4], MSGIC_TYPE_TOWER) != 0)
		{
			ret = 190500115;
		}
	} 
	else
	{
		if (player->sub_coin(table->parameter1[6], MSGIC_TYPE_TOWER) != 0)
		{
			ret = 190500100;
		}
	}
	AnsCleanTower send;
	ans_clean_tower__init(&send);
	if (ret != 0)
	{
		send.ret = ret;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CLEAN_TOWER_ANSWER, ans_clean_tower__pack, send);
	}

	uint32_t addExp = 0;
	uint32_t addMoney = 0;
	std::map<uint32_t, uint32_t> item_list;
	for (uint32_t l = player->data->tower.award_lv; l <= player->data->tower.top_lv; ++l)
	{
		P20076Table *monTable = get_config_by_id(351601000 + l, &tower_level_config);
		if (monTable == NULL)
		{
			continue;
		}
		for (uint32_t i = 0; i < monTable->n_RewardNum1; ++i)
		{
			item_list.insert(std::make_pair(monTable->Reward1[i], monTable->RewardNum1[i]));
		}
		for (uint32_t i = 0; i < monTable->n_RewardNum2; ++i)
		{
			item_list.insert(std::make_pair(monTable->Reward2[i], monTable->RewardNum2[i]));
		}
		addExp += monTable->ExpReward;
		addMoney += monTable->MoneyReward;
	}
	player->add_item_list_as_much_as_possible(item_list, MSGIC_TYPE_TOWER);
	player->add_exp(addExp, MSGIC_TYPE_TOWER);
	player->add_coin(addMoney, MSGIC_TYPE_TOWER);

	player->data->tower.cur_lv = player->data->tower.top_lv;
	player->data->tower.cur_num = 0;

	
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CLEAN_TOWER_ANSWER, ans_clean_tower__pack, send);

	return 0;
}
extern int refresh_next_tower(player_struct *player);
int handle_next_tower_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	int ret = refresh_next_tower(player);
	if (ret != 0)
	{
		LOG_ERR("[%s:%d] ret = %d", __FUNCTION__, __LINE__, ret);
	}

	return 0;
}
int handle_tower_max_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	ReqTowerMax* req = req_tower_max__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if (req == NULL)
	{
		LOG_ERR("[%s:%d] player reward zhao hui request unpack faild, player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	if (req->lv > 0 && req->lv <= MAX_TOWER_LEVEL)
	{
		conn_node_gamesrv::send_to_friend(extern_data, MSG_ID_TOWER_MAX_REQUEST, req, (pack_func)req_tower_max__pack);
	}

	req_tower_max__free_unpacked(req, NULL);

	return 0;
}

int handle_yao_qian_shu_info_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	player->player_yaoqian_shu_info_notify();
	return 0;
}

int handle_yao_qian_shu_start_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	PlayerYaoQianStartRequest* req = player_yao_qian_start_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] player yao qian shu start request unpack faild, player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t statu = req->num; //摇几次
	player_yao_qian_start_request__free_unpacked(req, NULL);
	uint32_t level = player->get_level();
	uint64_t id = 1041*100000 + level;
	ActorLevelTable *level_config = get_config_by_id(id, &actor_level_config);
	if(level_config == NULL)
	{
		LOG_INFO("[%s:%d]摇钱树给奖励失败，获取ActorLevelTable等级配置失败id[%lu]", __FUNCTION__, __LINE__, id);
		return -3;
	}
	uint32_t quelv_coin = level_config->QueLvCoin / 100; //银票系数

	PlayerYaoQianStartAnswer answer;
	player_yao_qian_start_answer__init(&answer);
	uint32_t all_beilv[YAO_QIAN_SHU_DAN_CI_MAX_NUM];
	memset(all_beilv, 0, sizeof(uint32_t) * YAO_QIAN_SHU_DAN_CI_MAX_NUM);
	answer.beilv = all_beilv;
	answer.n_beilv = 0;
	int ret = 0;
	do{
		//先判断能使用的次数够不够
		uint32_t yao_num = 1;
		if(player->data->yaoqian_data.sum_num <= player->data->yaoqian_data.use_num)
		{
			ret = 190500495;// 次数不够
			break;
		}
		if(statu != 0)
		{
			if(player->data->yaoqian_data.sum_num -  player->data->yaoqian_data.use_num < 10)
			{
				ret = 190500495; //次数不够
				break;
			}
			yao_num = 10;
		}
	
		//判断消耗的金票够不够
		uint32_t all_need_money;
		uint32_t next_need_money;
		bool free;

		free = player->count_yaoqian_shu_money(yao_num, &all_need_money, &next_need_money);
		
		if(all_need_money !=0)
		{
			if(player->sub_unbind_gold(all_need_money, MSGIC_TYPE_YAOQIAN_SHU_USE_GOLD) < 0)
			{
				ret = 190400005;     //元宝数量不够
				break;
			}
		}

		if (free)
		{
			player->data->yaoqian_data.free_num -= 1;			
		}
		
		player->data->yaoqian_data.use_num += yao_num;		
		player->data->yaoqian_data.next_need_money = next_need_money;
		
		uint32_t give_coin = 0;
		for(uint32_t i = 0; i < yao_num; i++)
		{
			//如果达到倍率机制要求，根据机制算获取的银票数量，在初始化机制条件
			ParameterTable *parame_config = NULL;
			player->data->yaoqian_data.cur_jizhi_num += 1;
			if(player->data->yaoqian_data.cur_jizhi_num == player->data->yaoqian_data.cur_suiji_zhong)
			{
				player->data->yaoqian_data.cur_jizhi_num = 0;
				give_coin += player->data->yaoqian_data.beilv_num * sg_yaoqian_shu_add_coin * quelv_coin;
				all_beilv[answer.n_beilv] = player->data->yaoqian_data.beilv_num;
				parame_config = get_config_by_id(161000423, &parameter_config);
				assert(parame_config != NULL && parame_config->n_parameter1 >=2 && parame_config->parameter1[0] < parame_config->parameter1[parame_config->n_parameter1-1]);
				player->data->yaoqian_data.cur_suiji_zhong = rand() % ((int)parame_config->parameter1[parame_config->n_parameter1-1] - (int)parame_config->parameter1[0]) + (int)parame_config->parameter1[0];
				uint32_t beilv = YAO_QIAN_SHU_CHU_SHI_BEILV;
				bool flag = false;
				for(uint32_t i = 0; i < parame_config->n_parameter1 && i + 1 < parame_config->n_parameter1; i++)
				{

					if(player->data->yaoqian_data.cur_suiji_zhong >= parame_config->parameter1[i] && player->data->yaoqian_data.cur_suiji_zhong < parame_config->parameter1[i+1])
					{
						player->data->yaoqian_data.beilv_num = beilv;
						flag = true;
						break;
					}
					beilv++;
				}
				if(flag == false)
				{
					LOG_ERR("[%s:%d] 摇钱树生成新的倍率失败，正常情况是不会失败的，失败就说明算法或配置有问题", __FUNCTION__, __LINE__);
				}
			}
			else 
			{
				bool is_double = false;
				parame_config = get_config_by_id(161000422, &parameter_config);
				if(parame_config != NULL && parame_config->n_parameter1 >= 2)
				{
					uint32_t double_shuiji = parame_config->parameter1[0];
					uint32_t double_max = parame_config->parameter1[1];
					uint32_t double_suiji = rand() % double_max + 1;
					if(double_suiji <= double_shuiji)
					{
						give_coin += 2 * sg_yaoqian_shu_add_coin * quelv_coin;
						all_beilv[answer.n_beilv] = 2;
						is_double = true;
					}
				}
				else 
				{
					LOG_ERR("[%s:%d] 摇钱树双倍暴击参数表有问题", __FUNCTION__, __LINE__);
				}

				if(is_double == false)
				{
					give_coin += sg_yaoqian_shu_add_coin * quelv_coin;
				}
			}
			answer.n_beilv++;
		}
		if(give_coin >0)
		{
			player->add_coin(give_coin, MSGIC_TYPE_YAOQIAN_SHU_ADD_COIN);
		}
	
	}while(0);

	
	PlayerYaoQianInfoNotify notify;
	player_yao_qian_info_notify__init(&notify);
	notify.sum_num = player->data->yaoqian_data.sum_num;
	notify.use_num = player->data->yaoqian_data.use_num;
	notify.free_num = player->data->yaoqian_data.free_num;
	player->count_yaoqian_shu_money(1, &notify.one_times, NULL);
	player->count_yaoqian_shu_money(10, &notify.ten_times, NULL);

	answer.result = ret;
	answer.into = &notify;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_YAOQIAN_SHU_START_ANSWER, player_yao_qian_start_answer__pack, answer);

	return 0;
}
int handle_receive_login_reward_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	PlayerLoginReceiveRewardRequest* req = player_login_receive_reward_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] player_login_receive_reward_request__unpack faild", __FUNCTION__, __LINE__);
		return -2;
	}
	uint32_t reward_id = req->id;
	player_login_receive_reward_request__free_unpacked(req, NULL);

	LoginReceiveInfo *login_reward_info = NULL;
	uint32_t now_time = time_helper::get_cached_time() / 1000;
	for(size_t i = 0; i < MAX_LOGIN_REWARD_RECEIVE_NUM; i++)
	{
		if(player->data->login_reward_info.info[i].id == reward_id)
			login_reward_info = &player->data->login_reward_info.info[i];
	}
	if(login_reward_info == NULL)
	{
		LOG_ERR("[%s:%d 领取登录奖励失败,未找到id对应的奖励信息,reward_id[%u]", __FUNCTION__, __LINE__, reward_id);
		return -3;
	}
	if(player->data->login_reward_info.open == false || player->data->login_reward_info.receive_time == false || (now_time - player->data->login_reward_info.open_time) > sg_login_reward_chixu_day * 24 * 3600)
	{
		LOG_ERR("[%s:%d] 领取登录奖励失败,功能未开启或已经超过领奖时间", __FUNCTION__, __LINE__);
		return -4;
	}
	if(login_reward_info->statu == 0 || login_reward_info->statu == 2)
	{
		LOG_ERR("[%s:%d] 领取登录奖励失败,奖励不可领或已领取", __FUNCTION__, __LINE__);
		return -5;
	}

	LoginGifts* login_reward_config = get_config_by_id(reward_id, &login_gifts_config);
	if(login_reward_config == NULL || login_reward_config->n_lRewards <= 0 || login_reward_config->n_Quantity <= 0 )
	{
		LOG_ERR("[%s:%d] 领取登录奖励失败,奖励配置表错误,reward_id[%u]", __FUNCTION__, __LINE__, reward_id);
		return -6;
	}
	int ret = 0;
	
	switch(login_reward_config->RewardsType)
	{
		//普通物品
		case 1:
			{
				if(login_reward_config->n_Quantity != login_reward_config->n_lRewards)
				{
					LOG_ERR("[%s:%d] 领取登录奖励失败,配置错误", __FUNCTION__, __LINE__);
					return -7;
				}
				std::map<uint32_t, uint32_t> item_map;
				item_map.clear();
				for(size_t i = 0; i < login_reward_config->n_lRewards; i++)
				{
					uint32_t item_id = login_reward_config->lRewards[i];
					uint32_t item_num = login_reward_config->Quantity[i];
					item_map[item_id] += item_num;
				}
				if (!player->check_can_add_item_list(item_map))
				{
					ret = 190500315; //包裹空位不足
					break;
				}
				login_reward_info->statu = 2;
				player->add_item_list(item_map, MSGIC_TYPE_RECEIVE_LOGIN_REWARD);

			}
			break;
			//增加坐骑
		case 2:
			{
				uint32_t horseId = login_reward_config->lRewards[0];
				login_reward_info->statu = 2;
				player->add_horse(horseId, login_reward_config->Time);
			}
			break;
			//增加时装
		case 3:
			{
				uint32_t player_job = player->get_job();
				uint32_t fashion_id = 0;
				uint32_t fashion_colour = 0;
				for(size_t i = 0; i < login_reward_config->n_lRewards; i++)
				{
					fashion_id = login_reward_config->lRewards[i];
					ActorFashionTable* fashion_table = get_config_by_id(fashion_id, &fashion_config);
					if(fashion_table == NULL)
					{
						LOG_ERR("[%s:%d] 领取登录奖励失败,获取时装配置表失败,时装id[%lu]", __FUNCTION__, __LINE__, login_reward_config->lRewards[i]);
						return -8;
					}
					if((uint32_t)fashion_table->Occupation == player_job)
					{
						fashion_colour = fashion_table->Colour;
					}
				}
				if(fashion_id == 0 || fashion_colour == 0)
				{
					LOG_ERR("[%s:%d] 领取登录奖励失败,未找到角色职业所对应的时装,时装id[%u]", __FUNCTION__, __LINE__, fashion_id);
					return -9;
				}
				if(player->add_fashion(fashion_id, fashion_colour, login_reward_config->Time) < 0)
				{
					ret = 190300008; //时装包裹满了
					break;
				}
				login_reward_info->statu = 2;
			}
			break;
			//增加伙伴
		case 4:
			{
				uint32_t partner_id = login_reward_config->lRewards[0];
				login_reward_info->statu = 2;
				player->add_partner(partner_id);
			}
			break;
		default:
			{
				LOG_ERR("[%s:%d]领取登录奖励失败,配置表错误", __FUNCTION__, __LINE__);
				return -10;
			}
	}
	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_LOGIN_RECEIVE_REWARD_ANSWER, comm_answer__pack, resp);

	return 0;
}

static int notify_function_info(player_struct *player, EXTERN_DATA *extern_data)
{
	FunctionOpenNotify nty;
	function_open_notify__init(&nty);

	nty.functionids = player->data->function_open;
	nty.n_functionids = 0;
	for (int i = 0; i < MAX_GAME_FUNCTION_NUM; ++i)
	{
		if (player->data->function_open[i] == 0)
		{
			break;
		}

		nty.n_functionids++;
	}

	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_FUNCTION_INFO_NOTIFY, function_open_notify__pack, nty);

	return 0;
}

int handle_receive_ci_fu_reward_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	PlayerReceiveCifuRequest* req = player_receive_cifu_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] player_receive_cifu_request__unpack faild", __FUNCTION__, __LINE__);
		return -2;
	}
	uint32_t reward_id = req->id;
	player_receive_cifu_request__free_unpacked(req, NULL);
	//CiFuTable 
	CiFuRewardData* ci_fu_data = NULL;
	for(size_t i = 0; i < MAX_CIFU_REWARD_NUM; i++)
	{
		if(player->data->ci_fu_reward[i].id == 0)
			break;
		if(player->data->ci_fu_reward[i].id == reward_id)
		{
			ci_fu_data = &player->data->ci_fu_reward[i];
		}
	}
	if(ci_fu_data == NULL)
	{
		LOG_ERR("[%s:%d 赐福领奖失败,未找到奖励id对应的数据, reward_id[%u]]", __FUNCTION__, __LINE__, reward_id);
		return -3;
	}
	CiFuTable* ci_fu_table = get_config_by_id(reward_id, &ci_fu_config);
	if(ci_fu_table == NULL)
	{
		LOG_ERR("[%s:%d 赐福领奖失败,未找到对应的配置, reward_id[%u]]", __FUNCTION__, __LINE__, reward_id);
		return -4;
	}
	uint32_t level = player->get_level();
	uint64_t id = 1041*100000 + level;
	ActorLevelTable *level_config = get_config_by_id(id, &actor_level_config);
	if(level_config == NULL)
	{
		LOG_INFO("[%s:%d]摇钱树给奖励失败，获取ActorLevelTable等级配置失败id[%lu]", __FUNCTION__, __LINE__, id);
		return -5;
	}
	uint32_t quelv_coin = level_config->QueLvCoin / 100; //银票系数

	//判断上次领取的时间是否跨过五点
	time_t now_time = time_helper::get_cached_time() / 1000;
	if(ci_fu_data->time != 0)
	{
		if(time_helper::is_same_day(ci_fu_data->time, now_time, 5*3600))
		{
			LOG_ERR("[%s:%d] 赐福领奖今日已经领取过了", __FUNCTION__, __LINE__);
			return -6;
		}
	}
	//是否是活动时间
	uint32_t cd =0;
	if(!check_active_open(ci_fu_table->ControlTable, cd))
	{
		LOG_ERR("[%s:%d] 赐福领奖不是活动期间", __FUNCTION__, __LINE__);
		return -7;
	}

	ci_fu_data->time = now_time;
	switch(ci_fu_table->Type)
	{
		case 1:
		{
			uint16_t give_coin = ci_fu_table->Vaule * quelv_coin;
			player->add_coin(give_coin, MSGIC_TYPE_CIFU_REWARD);
		}
			break;
		case 2:
		{
			player->add_bind_gold(ci_fu_table->Vaule, MSGIC_TYPE_CIFU_REWARD);
		}
			break;
		default:
			break;
	}
	PlayerReceiveCifuAnswer naswer;
	player_receive_cifu_answer__init(&naswer);
	PlayerCifuMainInfo main_info[MAX_CIFU_REWARD_NUM];
	PlayerCifuMainInfo* main_info_point[MAX_CIFU_REWARD_NUM];
	naswer.result = 0;
	naswer.n_info = 0;
	naswer.info = main_info_point;

	for(size_t i = 0; i < MAX_CIFU_REWARD_NUM; i++)
	{
		if(player->data->ci_fu_reward[i].id == 0)
			break;
		main_info_point[naswer.n_info] = &main_info[naswer.n_info];
		player_cifu_main_info__init(&main_info[naswer.n_info]);
		main_info[naswer.n_info].id = player->data->ci_fu_reward[i].id;
		main_info[naswer.n_info].time = player->data->ci_fu_reward[i].time;
		naswer.n_info++;
	}


	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_CIFU_RECEIVE_REWARD_ANSWER , player_receive_cifu_answer__pack, naswer);
	return 0;
}

static int handle_srv_reward_request(player_struct *player, EXTERN_DATA *extern_data, uint32_t msg_id)
{
	LOG_INFO("[%s:%d] player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);

	PROTO_SRV_REWARD_REQ *req = (PROTO_SRV_REWARD_REQ*)get_data();

	int ret = 0;
	do
	{
		if (!player || !player->is_online())
		{
			LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
			ret = 1111;
			break;
		}

		std::map<uint32_t, uint32_t> item_map;
		size_t item_len = (sizeof(req->item_id) / sizeof(req->item_id[0]));
		for (size_t i = 0; i < item_len; ++i)
		{
			uint32_t item_id = req->item_id[i];
			uint32_t item_num = req->item_num[i];
			if (item_id > 0 && item_num > 0)
			{
				item_map[item_id] += item_num;
			}
		}

		//检查背包格子
		if (item_map.size() > 0)
		{
			if (!player->check_can_add_item_list(item_map))
			{
				ret = ERROR_ID_BAG_GRID_NOT_ENOUGH;
				break;
			}
		}

		//发放奖励
		if (req->gold > 0)
		{
			player->add_bind_gold(req->gold, req->statis_id);
		}
		if (req->coin > 0)
		{
			player->add_coin(req->coin, req->statis_id);
		}
		if (item_map.size() > 0)
		{
			player->add_item_list(item_map, req->statis_id, true);
		}

		if (req->statis_id == MAGIC_TYPE_SHOP_BUY)
		{
			player->add_task_progress(TCT_SHOP_BUY, 0, 1);
		}
	} while(0);

	PROTO_SRV_REWARD_RES *res = (PROTO_SRV_REWARD_RES *)get_send_data();
	uint32_t data_len = sizeof(PROTO_SRV_REWARD_RES) + req->data_size;
	memset(res, 0, data_len);
	res->result = ret;
	res->statis_id = req->statis_id;
	res->data_size = req->data_size;
	if (req->data_size > 0)
	{
		memcpy(res->data, req->data, req->data_size);
	}
	fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, msg_id, data_len, 0);

	return 0;
}

//活动服请求发奖
static int handle_activitysrv_reward_request(player_struct *player, EXTERN_DATA *extern_data)
{
	return handle_srv_reward_request(player, extern_data, SERVER_PROTO_ACTIVITYSRV_REWARD_ANSWER);
}

//货币兑换
int handle_player_money_exchange_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if(!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	PlayerMoneyExchangeRequest* req = player_money_exchange_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] player money exchange request unpack faild", __FUNCTION__,__LINE__);
		return -2;
	}
	uint32_t change_type = req->type;
	uint32_t change_num  = req->num;
	player_money_exchange_request__free_unpacked(req, NULL);


	uint32_t level = player->get_level();
	uint64_t id = 1041*100000 + level;
	ActorLevelTable *level_config = get_config_by_id(id, &actor_level_config);
	if(level_config == NULL)
	{
		LOG_INFO("[%s:%d]货币兑换，获取ActorLevelTable等级配置失败id[%lu]", __FUNCTION__, __LINE__, id);
		return -3;
	}
	uint32_t quelv_coin = level_config->QueLvCoin / 100; //银票系数

	int ret = 0;
	switch(change_type)
	{
		case MONET_EXCHANG_TYPE__YUANBAO_CHANGE_YINBI:
		{
			uint32_t add_silver_num = change_num * sg_money_exchange_get_yinbi_num;
			if(player->data->attrData[PLAYER_ATTR_SILVER] + add_silver_num > (uint32_t)MAX_MONEY_VALUE)
			{
				ret = 190500523;
				break;
			}
			if(player->sub_unbind_gold(change_num, MAGIC_TYPE_MONEY_EXCHANGE_USE) !=0)
			{
				ret = 190500525; //元宝数量不足
				break;
			}
			player->add_silver(add_silver_num, MAGIC_TYPE_MONEY_EXCHANGE_ADD);
		}
			break;
		case MONET_EXCHANG_TYPE__YINBI_CHANGE_YINPIAO:
		{
			uint32_t add_coin_num = change_num * sg_money_exchange_get_yinpiao_num * quelv_coin;
			if(player->data->attrData[PLAYER_ATTR_COIN] + add_coin_num > (uint32_t)MAX_MONEY_VALUE)
			{
				ret = 190500523;
				break;
			}
			if(player->sub_silver(change_num, MAGIC_TYPE_MONEY_EXCHANGE_USE) != 0)
			{
				ret = 190500524; //银币数量不足
				break;
			}
			player->add_coin(add_coin_num, MAGIC_TYPE_MONEY_EXCHANGE_ADD);
		
		}
			break;
		default:
		{
			LOG_ERR("[%s:%d] 货币兑换类型错误,type[%u]", __FUNCTION__, __LINE__, change_type);
			return -3;
		}
		
	}

	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MONEY_EXCHANGE_ANSWER, comm_answer__pack, answer);
	return 0;
}

//门宗传功界面信息请求
int handle_player_guild_chuan_gong_info_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	if (player->data->guild_id == 0)
	{
		LOG_ERR("[%s:%d] get guild chuan gong info faild, player can't join guild, player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	ProtoGuildInfo *guild_info = get_guild_summary(player->data->guild_id);
	if (guild_info == NULL)
	{
		LOG_ERR("[%s:%d] get guild chuan gong info faild, get guild info faild, player_id[%lu] guild_id[%u]", __FUNCTION__, __LINE__, extern_data->player_id, player->data->guild_id);
		return -3;
	}

	GuildChuanGongInfoAnswer answer;
	guild_chuan_gong_info_answer__init(&answer);
	GuildChuanGongMemberInfo give_member_info[MAX_GUILD_MEMBER_NUM];
	GuildChuanGongMemberInfo* give_member_info_point[MAX_GUILD_MEMBER_NUM];
	GuildChuanGongMemberInfo get_member_info[MAX_GUILD_MEMBER_NUM];
	GuildChuanGongMemberInfo* get_member_info_point[MAX_GUILD_MEMBER_NUM];
	char give_player_name[MAX_GUILD_MEMBER_NUM][MAX_PLAYER_NAME_LEN + 1];
	char get_player_name[MAX_GUILD_MEMBER_NUM][MAX_PLAYER_NAME_LEN + 1];

	answer.bei_chuan_gong_num = player->data->guild_chuan_gong_info.bei_chuan_num;
	answer.give_chuan_gong_num = player->data->guild_chuan_gong_info.give_chuan_num;
	answer.can_jieshou_chuan_gong_member = give_member_info_point;
	answer.n_can_jieshou_chuan_gong_member = 0;
	answer.can_giveme_chuan_gong_member = get_member_info_point;
	answer.n_can_giveme_chuan_gong_member = 0;
	for (size_t i = 0; i < MAX_GUILD_MEMBER_NUM; i++)
	{
		if (guild_info->player_data[i].player_id == 0)
			break;
		if (guild_info->player_data[i].player_id == player->data->player_id)
			continue;
		player_struct* guild_member = player_manager::get_player_by_id(guild_info->player_data[i].player_id);
		if (!guild_member || !guild_member->is_online())
			continue;
		uint32_t my_level = player->get_level();
		uint32_t he_level = guild_member->get_level();
		if (guild_member->data->guild_chuan_gong_info.bei_chuan_num < sg_bei_dong_chuan_gong_num && my_level >= he_level && (my_level - he_level >= sg_chuan_gong_level_limit) && player->data->guild_id == guild_member->data->guild_id)
		{
			give_member_info_point[answer.n_can_jieshou_chuan_gong_member] = &give_member_info[answer.n_can_jieshou_chuan_gong_member];
			guild_chuan_gong_member_info__init(&give_member_info[answer.n_can_jieshou_chuan_gong_member]);
			give_member_info[answer.n_can_jieshou_chuan_gong_member].player_id = guild_member->data->player_id;
			memset(give_player_name[answer.n_can_jieshou_chuan_gong_member], 0, MAX_PLAYER_NAME_LEN + 1);
			strcpy(give_player_name[answer.n_can_jieshou_chuan_gong_member], guild_member->data->name);
			give_member_info[answer.n_can_jieshou_chuan_gong_member].player_name = give_player_name[answer.n_can_jieshou_chuan_gong_member];
			give_member_info[answer.n_can_jieshou_chuan_gong_member].player_level = he_level;
			answer.n_can_jieshou_chuan_gong_member++;
		}

		if (player->data->guild_chuan_gong_info.bei_chuan_num < sg_bei_dong_chuan_gong_num && guild_member->data->guild_chuan_gong_info.give_chuan_num < sg_zhu_dong_chuan_gong_num && he_level >= my_level && (he_level - my_level >= sg_chuan_gong_level_limit) && player->data->guild_id == guild_member->data->guild_id)
		{
			get_member_info_point[answer.n_can_giveme_chuan_gong_member] = &get_member_info[answer.n_can_giveme_chuan_gong_member];
			guild_chuan_gong_member_info__init(&get_member_info[answer.n_can_giveme_chuan_gong_member]);
			get_member_info[answer.n_can_giveme_chuan_gong_member].player_id = guild_member->data->player_id;
			memset(get_player_name[answer.n_can_giveme_chuan_gong_member], 0, MAX_PLAYER_NAME_LEN + 1);
			strcpy(get_player_name[answer.n_can_giveme_chuan_gong_member], guild_member->data->name);
			get_member_info[answer.n_can_giveme_chuan_gong_member].player_name = get_player_name[answer.n_can_giveme_chuan_gong_member];
			get_member_info[answer.n_can_giveme_chuan_gong_member].player_level = he_level;
			answer.n_can_giveme_chuan_gong_member++;
		}

	}
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUILD_CHUAN_GONG_INFO_ANSWER, guild_chuan_gong_info_answer__pack, answer);
	return 0;
}

//门宗传功请求
int handle_player_guild_chuan_gong_start_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	GuildChuanGongRequest* req = guild_chuan_gong_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if (req == NULL)
	{
		LOG_ERR("[%s:%d] guild chuan gong request unpack faild,player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t type = req->type;
	uint64_t player_id = req->player_id;
	guild_chuan_gong_request__free_unpacked(req, NULL);

	if (player_id == player->data->player_id)
	{
		LOG_ERR("[%s:%d] 帮会传功请求失败,对象不能是自己, player_id[%lu]", __FUNCTION__, __LINE__, player_id);
		return -3;
	}

	if (type != 1 && type != 2)
	{
		LOG_ERR("[%s:%d] 帮会传功请求失败,传功类型不对type[%u]", __FUNCTION__, __LINE__, type);
		return -4;
	}

	player_struct* guild_player = player_manager::get_player_by_id(player_id);
	int ret = 0;
	do{
		if (!guild_player || !guild_player->is_online())
		{
			ret = 190500534;
			break;
		}
		if (player->data->guild_id != guild_player->data->guild_id)
		{
			ret = 190500537;
			break;
		}

		//我能否传送到对方地图,以及我能否离开自己地图
		if (guild_player->scene == NULL || guild_player->check_can_transfer() != 0 || player->get_attr(PLAYER_ATTR_LEVEL) < guild_player->scene->res_config->Level)
		{
			ret = 190500533;    //我无法传送至对方的地图
			break;
		}
		if (player->check_can_transfer() != 0)
		{
			ret = 190500045;	//我不能离开自己地图
			break;
		}

		//我是否存活
		if (!player->is_alive())
		{
			ret = 190500549; //自己不是存活状态
			break;
		}

		//对方是否存活
		if (!guild_player->is_alive())
		{
			ret = 190500361;
			break;
		}
		uint32_t my_level = player->get_level();
		uint32_t he_level = guild_player->get_level();
		if (type == 1)
		{
			if (my_level < he_level || (my_level - he_level < sg_chuan_gong_level_limit))
			{
				ret = 190500535;
				break;
			}

		}
		else
		{
			if (my_level > he_level || (he_level - my_level < sg_chuan_gong_level_limit))
			{
				ret = 190500535;
				break;
			}
			if (guild_player->data->guild_chuan_gong_info.bei_chuan_num >= sg_bei_dong_chuan_gong_num)
			{
				ret = 190500536;
				break;
			}
		}

		//还需要判断对方是否正在传功
		if (player->is_in_guild_chuan_gong())
		{
			ret = 190500546;
			break;
		}
		if (guild_player->is_in_guild_chuan_gong())
		{
			ret = 190500538;
			break;
		}

	} while (0);

	//通知对方
	if (ret == 0 && guild_player != NULL && guild_player->is_online())
	{
		//如果对方在线，且符合要求，先计入自己的请求列表，再通知对方
		if (type == 1)
		{
			for (size_t i = 0; i < MAX_GUILD_MEMBER_NUM; i++)
			{
				if (player->data->guild_chuan_gong_info.me_to_other[i].player_id == guild_player->data->player_id)
				{
					player->data->guild_chuan_gong_info.me_to_other[i].time = time_helper::get_cached_time() / 1000;
					break;
				}
				if (player->data->guild_chuan_gong_info.me_to_other[i].player_id == 0)
				{
					player->data->guild_chuan_gong_info.me_to_other[i].player_id = guild_player->data->player_id;
					break;
				}
			}

		}
		else
		{
			for (size_t i = 0; i < MAX_GUILD_MEMBER_NUM; i++)
			{
				if (player->data->guild_chuan_gong_info.other_to_me[i].player_id == guild_player->data->player_id)
				{
					player->data->guild_chuan_gong_info.other_to_me[i].time = time_helper::get_cached_time() / 1000;
					break;
				}
				if (player->data->guild_chuan_gong_info.other_to_me[i].player_id == 0)
				{
					player->data->guild_chuan_gong_info.other_to_me[i].player_id = guild_player->data->player_id;
					break;
				}
			}

		}
		player->add_task_progress(TCT_GUILD_CHUAN_GONG, 0, 1);
		GuildChuanGongNotify notify;
		char player_name[MAX_PLAYER_NAME_LEN + 1];
		memset(player_name, 0, MAX_PLAYER_NAME_LEN + 1);
		strcpy(player_name, player->data->name);
		guild_chuan_gong_notify__init(&notify);
		notify.type = type;
		notify.player_id = player->data->player_id;
		notify.name = player_name;
		EXTERN_DATA he_ext_data;
		he_ext_data.player_id = guild_player->data->player_id;

		fast_send_msg(&conn_node_gamesrv::connecter, &he_ext_data, MSG_ID_GUILD_CHUAN_GONG_START_NOTIFY, guild_chuan_gong_notify__pack, notify);
	}

	GuildChuanGongAnswer answer;
	guild_chuan_gong_answer__init(&answer);

	answer.result = ret;
	answer.player_id = player_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUILD_CHUAN_GONG_START_ANSWER, guild_chuan_gong_answer__pack, answer);
	return 0;
}

//门宗传功确认或取消请求
int handle_player_guild_chuan_gong_queren_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	GuildChuanGongIsOrNoRequest* req = guild_chuan_gong_is_or_no_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if (req == NULL)
	{
		LOG_ERR("[%s:%d] guild chuan gong is or no request unpack faild, player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t type = req->type;
	uint32_t result = req->result;
	uint64_t player_id = req->player_id;
	guild_chuan_gong_is_or_no_request__free_unpacked(req, NULL);

	if ((type != 1 && type != 2) || (result != 1 && result != 2))
	{
		LOG_ERR("[%s:%d] 门宗传功请求类型或结果有误,type[%u] result[%u]", __FUNCTION__, __LINE__, type, result);
		return -3;
	}
	int ret = 0;
	double he_pos_x = 0;
	double he_pos_z = 0;
	player_struct* guild_player = player_manager::get_player_by_id(player_id);
	//先判断对方的列表里面有没有我,如没有即出错
	if (guild_player != NULL && guild_player->is_online())
	{
		bool flag = false;
		if (type == 1)
		{
			for (size_t i = 0; i < MAX_GUILD_MEMBER_NUM; i++)
			{
				if (guild_player->data->guild_chuan_gong_info.me_to_other[i].player_id == 0)
					break;
				if (guild_player->data->guild_chuan_gong_info.me_to_other[i].player_id == player->data->player_id)
				{
					flag = true;
					break;
				}
			}
		}
		else
		{
			for (size_t i = 0; i < MAX_GUILD_MEMBER_NUM; i++)
			{
				if (guild_player->data->guild_chuan_gong_info.other_to_me[i].player_id == 0)
					break;
				if (guild_player->data->guild_chuan_gong_info.other_to_me[i].player_id == player->data->player_id)
				{
					flag = true;
					break;
				}
			}

		}
		if (flag == false)
		{
			LOG_ERR("[%s:%d] 门宗传功确认出错,对方列表里面并没有我", __FUNCTION__, __LINE__);
			return -4;
		}
	}

	do
	{
		//取消的处理
		if (result == 1)
		{
			if (!guild_player || !guild_player->is_online())
			{
				ret = 190500534;
				break;
			}

			for (size_t i = 0; i < MAX_GUILD_MEMBER_NUM; i++)
			{
				if (guild_player->data->guild_chuan_gong_info.me_to_other[i].player_id == 0 && guild_player->data->guild_chuan_gong_info.other_to_me[i].player_id == 0)
					break;
				if (guild_player->data->guild_chuan_gong_info.me_to_other[i].player_id == player->data->player_id || guild_player->data->guild_chuan_gong_info.other_to_me[i].player_id == player->data->player_id)
				{
					EXTERN_DATA he_ext_data;
					he_ext_data.player_id = guild_player->data->player_id;
					GuildChuanGongQuXiaoNotify notify;
					char player_name[MAX_PLAYER_NAME_LEN + 1];
					memset(player_name, 0, MAX_PLAYER_NAME_LEN + 1);
					strcpy(player_name, player->data->name);
					guild_chuan_gong_qu_xiao_notify__init(&notify);
					notify.player_id = player->data->player_id;
					notify.name = player_name;
					fast_send_msg(&conn_node_gamesrv::connecter, &he_ext_data, MSG_ID_GUILD_CHUAN_GONG_QU_XIAO_NOTIFY, guild_chuan_gong_qu_xiao_notify__pack, notify);
					break;
				}
			}
			return 0;
		}

		//确认的处理
		//对方不在线
		if (!guild_player || !guild_player->is_online())
		{
			ret = 190500534;
			break;
		}


		//对方和自己不是在同一帮会
		if (player->data->guild_id == 0 || guild_player->data->guild_id == 0 || player->data->guild_id != guild_player->data->guild_id)
		{
			ret = 190500537;
			break;
		}


		//我是否存活
		if (!player->is_alive())
		{
			ret = 190500549; //自己不是存活状态
			break;
		}

		//对方是否存活
		if (!guild_player->is_alive())
		{
			ret = 190500361;
			break;
		}


		//判断对方能否传送到我的地图，以及对方能否离开自己的地图
		if (player->scene == NULL || player->check_can_transfer() != 0 || guild_player->get_attr(PLAYER_ATTR_LEVEL) < player->scene->res_config->Level)
		{
			ret = 190500544;    //对方无法传送至我的地图
			break;
		}
		if (guild_player->check_can_transfer() != 0)
		{
			ret = 190500542;	//对方目前无法离开自己的地图
			break;
		}

		//等级判断, 次数判断
		uint32_t my_level = player->get_level();
		uint32_t he_level = guild_player->get_level();

		if (type == 1)
		{
			if (my_level > he_level || (he_level - my_level < sg_chuan_gong_level_limit))
			{
				ret = 190500535;
				break;
			}
			if (player->data->guild_chuan_gong_info.bei_chuan_num >= sg_bei_dong_chuan_gong_num)
			{
				ret = 190500545; //自己的被传功次数已满
				break;
			}
			//判断对方包裹
			if (guild_player->check_can_add_item(sg_zhu_chuan_gong_add_item_id, sg_zhu_chuan_gong_add_item_num, NULL) != 0)
			{
				ret = 190500541; //对方包裹已满,不能获得奖励物品,提示失败
				break;
			}
		}
		else
		{
			if (my_level < he_level || (my_level - he_level < sg_chuan_gong_level_limit))
			{
				ret = 190500535;
				break;
			}
			if (guild_player->data->guild_chuan_gong_info.bei_chuan_num >= sg_bei_dong_chuan_gong_num)
			{
				ret = 190500536; //地方的被传功次数已满
				break;
			}
			//判断我自己的包裹
			ret = player->check_can_add_item(sg_zhu_chuan_gong_add_item_id, sg_zhu_chuan_gong_add_item_num, NULL);
			if(ret != 0)
			{
				if(ret == ERROR_ID_BAG_GRID_NOT_ENOUGH)
				{
					ret = 190500337; //我的包裹满了
				}
				break;
			}
		}

		//判断对方和自己是否正在传功
		if (player->is_in_guild_chuan_gong())
		{
			ret = 190500546;
			break;
		}
		if (guild_player->is_in_guild_chuan_gong())
		{
			ret = 190500538;
			break;
		}

		//选传送坐标点
		struct position *leader_pos = player->get_pos();
		int j = 0;
		bool get_pos_sussecc = true;
		while(1)
		{
			if(j > 10)
			{
				get_pos_sussecc = false;
				break;
			}
			uint32_t angle = rand()% 360;
			he_pos_x =  leader_pos->pos_x + 2 * cos(angle * 3.14 / 180);
			he_pos_z =  leader_pos->pos_z + 2 * sin(angle * 3.14 / 180);
			struct map_block *block_start = get_map_block(player->scene->map_config, he_pos_x, he_pos_z);
			if (block_start != NULL && block_start->can_walk == true)
				break;
			j++;
		}
		if(get_pos_sussecc == false)
		{
			ret = 190500544;
			break;
		}

	} while (0);

	if (ret == 0)
	{
		//下坐骑 
		player->down_horse();
		guild_player->down_horse();

		//将请求者拉到跟自己所在的同一地图,加buff,
		//先传送
		bool is_simple_scene = false;
		if (player->data->scene_id == guild_player->data->scene_id)
		{
			is_simple_scene = true;
		}
		EXTERN_DATA he_extern_datae;
		he_extern_datae.player_id = guild_player->data->player_id;

		int trans_result = guild_player->transfer_to_new_scene(player->data->scene_id, he_pos_x, -1, he_pos_z, 0, &he_extern_datae);
		if (trans_result != 0)
		{
			LOG_ERR("[%s:%d] 帮会传功拉人传送失败, result[%d]", __FUNCTION__, __LINE__, trans_result);
			return -4;
		}

		//加传功buff
		buff_struct *buff = buff_manager::create_default_buff(sg_guild_chuan_gong_buff_id, player, player, true);
		if (!buff)
		{
			LOG_ERR("[%s:%d] player[%lu] 帮会传功加buff失败, buff_id:%u", __FUNCTION__, __LINE__, player->data->player_id, sg_guild_chuan_gong_buff_id);
			return -5;;
		}

		buff = buff_manager::create_default_buff(sg_guild_chuan_gong_buff_id, guild_player, guild_player, true);
		if (!buff)
		{
			player->delete_one_buff(sg_guild_chuan_gong_buff_id, true);
			LOG_ERR("[%s:%d] player[%lu] 帮会传功加buff失败, buff_id:%u", __FUNCTION__, __LINE__, guild_player->data->player_id, sg_guild_chuan_gong_buff_id);
			return -5;;
		}


		//两边记录信息
		if (type == 1)
		{
			player->data->guild_chuan_gong_info.cur_info.type = 2;
			guild_player->data->guild_chuan_gong_info.cur_info.type = 1;
		}
		else
		{
			player->data->guild_chuan_gong_info.cur_info.type = 1;
			guild_player->data->guild_chuan_gong_info.cur_info.type = 2;

		}
		player->data->guild_chuan_gong_info.cur_info.player_id = guild_player->data->player_id;
		guild_player->data->guild_chuan_gong_info.cur_info.player_id = player->data->player_id;

		//停止移动传送通知
		CommAnswer stop_move;
		comm_answer__init(&stop_move);
		stop_move.result = 0;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUILD_CHUAN_GONG_STOP_MOVE_NOTIFY, comm_answer__pack, stop_move);

		//开始读条通知
		if (is_simple_scene == true)
		{
			EXTERN_DATA he_exter_data;
			he_exter_data.player_id = guild_player->data->player_id;
			fast_send_msg(&conn_node_gamesrv::connecter, &he_exter_data, MSG_ID_GUILD_CHUAN_GONG_STOP_MOVE_NOTIFY, comm_answer__pack, stop_move);


			CommAnswer start_notify;
			comm_answer__init(&start_notify);
			start_notify.result = 0;
			uint64_t *ppp = conn_node_gamesrv::prepare_broadcast_msg_to_players(MSG_ID_GUILD_CHUAN_GONG_DU_TIAO_NOTIFY, &start_notify, (pack_func)comm_answer__pack);
			ppp = conn_node_gamesrv::broadcast_msg_add_players(player->data->player_id, ppp);
			ppp = conn_node_gamesrv::broadcast_msg_add_players(guild_player->data->player_id, ppp);
			conn_node_gamesrv::broadcast_msg_send();

			//广播给视野内玩家
			GuildChuanGongStatuNotify chuan_gong_broad;
			guild_chuan_gong_statu_notify__init(&chuan_gong_broad);
			GuildPlayerPosInfo chuangong_player1;
			GuildPlayerPosInfo chuangong_player2;
			guild_player_pos_info__init(&chuangong_player1);
			guild_player_pos_info__init(&chuangong_player2);
			chuan_gong_broad.statu = 1;
			struct position *my_pos = player->get_pos();
			struct position *guild_pos = guild_player->get_pos();
			chuangong_player1.player_id = player->data->player_id;
			chuangong_player1.pos_x = my_pos->pos_x;
			chuangong_player1.pos_z = my_pos->pos_z;
			chuangong_player2.player_id = guild_player->data->player_id;
			chuangong_player2.pos_x = guild_pos->pos_x;
			chuangong_player2.pos_z = guild_pos->pos_z;
			chuan_gong_broad.player1_info = & chuangong_player1;
			chuan_gong_broad.player2_info = & chuangong_player2;
			player->broadcast_to_sight(MSG_ID_GUILD_CHUAN_GONG_ZHUANGTAI_BROADCAST, &chuan_gong_broad, (pack_func)guild_chuan_gong_statu_notify__pack, true);
			guild_player->broadcast_to_sight(MSG_ID_GUILD_CHUAN_GONG_ZHUANGTAI_BROADCAST, &chuan_gong_broad, (pack_func)guild_chuan_gong_statu_notify__pack, true);
		}

	}
	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUILD_CHUAN_GONG_IS_OR_NO_ANSWER, comm_answer__pack, answer);

	return 0;
}

//门宗传功完成请求
int handle_player_guild_chuan_gong_finish_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	if (!player->is_in_guild_chuan_gong())
	{
		LOG_DEBUG("[%s:%d]帮会传功完成请求失败,可能另一方已经确认完成,player_id[%lu]", __FUNCTION__, __LINE__, player->data->player_id);
		return -2;
	}
	player_struct* guild_player = player_manager::get_player_by_id(player->data->guild_chuan_gong_info.cur_info.player_id);

	int ret = 0;

	do{
		if (!guild_player || !guild_player->is_online())
		{
			ret = 11111;  //对方已经下线
			break;
		}

		if (!guild_player->is_in_guild_chuan_gong())
		{
			ret = 22222;
			LOG_ERR("[%s:%d]帮会传功完成请求失败,对方不是传功状态my_player_id[%lu], he_player_id[%lu]", __FUNCTION__, __LINE__, player->data->player_id, guild_player->data->player_id);
			break;
		}
		if (player->data->player_id != guild_player->data->guild_chuan_gong_info.cur_info.player_id || player->data->guild_chuan_gong_info.cur_info.player_id != guild_player->data->player_id)
		{
			ret = 33333;
			break;
		}

		//到这里说明所有条件都满足,可以给经验,扣次数
		uint32_t my_level = player->get_level();
		uint32_t he_level = guild_player->get_level();
		uint32_t mean_level = (my_level + he_level) / 2;
		uint32_t my_id = 1041 * 100000 + my_level;
		ActorLevelTable *my_level_config = get_config_by_id(my_id, &actor_level_config);
		uint32_t he_id = 1041 * 100000 + he_level;
		ActorLevelTable *he_level_config = get_config_by_id(he_id, &actor_level_config);
		uint32_t mean_id = 1041 * 100000 + mean_level;
		ActorLevelTable *mean_level_config = get_config_by_id(mean_id, &actor_level_config);
		if (my_level_config == NULL || he_level_config == NULL || mean_level_config == NULL)
		{
			LOG_INFO("[%s:%d]门宗传功，获取ActorLevelTable等级配置失败id[%u]", __FUNCTION__, __LINE__, my_id);
			return -3;
		}
		uint32_t my_quelv_exp = my_level_config->QueLvExp / 100; //经验系数
		uint32_t he_quelv_exp = he_level_config->QueLvExp / 100; //经验系数
		uint32_t mean_quelv_exp = mean_level_config->QueLvExp / 100; //经验系数
		int he_ret = 0;
		do{
			if (player->data->guild_chuan_gong_info.cur_info.type == 1)
			{

				ret = player->check_can_add_item(sg_zhu_chuan_gong_add_item_id, sg_zhu_chuan_gong_add_item_num, NULL);
				if(ret != 0)
				{
					if(ret == ERROR_ID_BAG_GRID_NOT_ENOUGH)
					{
						ret = 190500337;//自己的包裹满了
					}
					he_ret = 190500541; //告诉对方失败
					break;
				}
				if (player->data->guild_chuan_gong_info.give_chuan_num < sg_zhu_dong_chuan_gong_num)
				{
					player->data->guild_chuan_gong_info.give_chuan_num++;
					uint32_t exp = sg_zhu_chuan_gong_add_exp * my_quelv_exp;
					player->add_exp(exp, MAGIC_TYPE_GUILD_CHUAN_GONG);
					player->add_item(sg_zhu_chuan_gong_add_item_id, sg_zhu_chuan_gong_add_item_num, MAGIC_TYPE_GUILD_CHUAN_GONG);
				}
				if (guild_player->data->guild_chuan_gong_info.bei_chuan_num < sg_bei_dong_chuan_gong_num)
				{
					guild_player->data->guild_chuan_gong_info.bei_chuan_num++;
					uint32_t exp = sg_bei_chuan_gong_add_exp * mean_quelv_exp;
					guild_player->add_exp(exp, MAGIC_TYPE_GUILD_CHUAN_GONG);
				}
			}
			else
			{
				ret = guild_player->check_can_add_item(sg_zhu_chuan_gong_add_item_id, sg_zhu_chuan_gong_add_item_num, NULL);
				if(ret != 0)
				{
					ret = 190500541;
					he_ret = 190500337;
					break;
				}
				if (player->data->guild_chuan_gong_info.bei_chuan_num < sg_bei_dong_chuan_gong_num)
				{
					player->data->guild_chuan_gong_info.bei_chuan_num++;
					uint32_t exp = sg_bei_chuan_gong_add_exp * mean_quelv_exp;
					player->add_exp(exp, MAGIC_TYPE_GUILD_CHUAN_GONG);
				}
				if (guild_player->data->guild_chuan_gong_info.give_chuan_num < sg_zhu_dong_chuan_gong_num)
				{
					guild_player->data->guild_chuan_gong_info.give_chuan_num++;
					uint32_t exp = sg_zhu_chuan_gong_add_exp * he_quelv_exp;
					guild_player->add_exp(exp, MAGIC_TYPE_GUILD_CHUAN_GONG);
					guild_player->add_item(sg_zhu_chuan_gong_add_item_id, sg_zhu_chuan_gong_add_item_num, MAGIC_TYPE_GUILD_CHUAN_GONG);

				}

			}
		} while (0);

		guild_player->check_activity_progress(AM_GUILD_CHUANGONG, 0);		
		//清除信息
		uint32_t he_type = guild_player->data->guild_chuan_gong_info.cur_info.type;
		guild_player->clean_guild_chuan_gong_info();

		GuildChuanGongFinishNotify guild_player_notify;
		guild_chuan_gong_finish_notify__init(&guild_player_notify);
		char my_name[MAX_PLAYER_NAME_LEN + 1];
		memset(my_name, 0, MAX_PLAYER_NAME_LEN + 1);
		strcpy(my_name, player->data->name);
		guild_player_notify.result = he_ret;
		guild_player_notify.type = he_type;
		guild_player_notify.player_id = player->data->player_id;
		guild_player_notify.name = my_name;

		EXTERN_DATA he_exter_data;
		he_exter_data.player_id = guild_player->data->player_id;
		fast_send_msg(&conn_node_gamesrv::connecter, &he_exter_data, MSG_ID_GUILD_CHUAN_GONG_FINISH_NOTIFY, guild_chuan_gong_finish_notify__pack, guild_player_notify);

	} while (0);

	player->check_activity_progress(AM_GUILD_CHUANGONG, 0);			
	player->clean_guild_chuan_gong_info();
	uint32_t my_type = player->data->guild_chuan_gong_info.cur_info.type;

	GuildChuanGongFinishNotify notify;
	guild_chuan_gong_finish_notify__init(&notify);
	char he_name[MAX_PLAYER_NAME_LEN + 1];
	memset(he_name, 0, MAX_PLAYER_NAME_LEN + 1);
	notify.result = ret;
	if (ret == 0)
	{
		strcpy(he_name, guild_player->data->name);
		notify.type = my_type;
		notify.player_id = guild_player->data->player_id;
		notify.name = he_name;
		fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_GUILD_CHUAN_GONG_FINISH_NOTIFY, guild_chuan_gong_finish_notify__pack, notify);


		//广播给视野内玩家
		GuildChuanGongStatuNotify chuan_gong_broad;
		guild_chuan_gong_statu_notify__init(&chuan_gong_broad);
		GuildPlayerPosInfo chuangong_player1;
		GuildPlayerPosInfo chuangong_player2;
		guild_player_pos_info__init(&chuangong_player1);
		guild_player_pos_info__init(&chuangong_player2);
		chuan_gong_broad.statu = 2;
		struct position *my_pos = player->get_pos();
		struct position *guild_pos = guild_player->get_pos();
		chuangong_player1.player_id = player->data->player_id;
		chuangong_player1.pos_x = my_pos->pos_x;
		chuangong_player1.pos_z = my_pos->pos_z;
		chuangong_player2.player_id = guild_player->data->player_id;
		chuangong_player2.pos_x = guild_pos->pos_x;
		chuangong_player2.pos_z = guild_pos->pos_z;
		chuan_gong_broad.player1_info = & chuangong_player1;
		chuan_gong_broad.player2_info = & chuangong_player2;
		player->broadcast_to_sight(MSG_ID_GUILD_CHUAN_GONG_ZHUANGTAI_BROADCAST, &chuan_gong_broad, (pack_func)guild_chuan_gong_statu_notify__pack, true);
		guild_player->broadcast_to_sight(MSG_ID_GUILD_CHUAN_GONG_ZHUANGTAI_BROADCAST, &chuan_gong_broad, (pack_func)guild_chuan_gong_statu_notify__pack, true);
	}
	return 0;
}


//帮会入侵活动实时信息请求
int handle_guild_ruqin_shishi_info_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	if(player->data->guild_id == 0)
	{
		LOG_ERR("[%s:%d] guild ruqin shishi info request, player no guild player_id[%lu]", __FUNCTION__, __LINE__, player->data->player_id);
		return -2;
	}	
	
	std::map<uint32_t, guild_land_raid_struct *>::iterator itr = guild_land_raid_manager_raid_map.find(player->data->guild_id);
	if(itr == guild_land_raid_manager_raid_map.end())
	{
		LOG_ERR("[%s:%d] no guild lan raid ,guild_id[%u]", __FUNCTION__, __LINE__, player->data->guild_id);
		return -3;
	}

	guild_land_raid_struct *guild_land_raid = itr->second;
	if(guild_land_raid == NULL)
	{
		LOG_ERR("[%s:%d] get guild ruqin info faild", __FUNCTION__, __LINE__);
		return -4;
	}

	if(guild_land_raid->GUILD_LAND_DATA.activity_id != GUILD_RUQIN_ACTIVITY_ID || guild_land_raid->ruqin_data.guild_ruqin == false)
	{
		LOG_ERR("[%s:%d] get guild ruqin info fiald, activity no open, guild_id[%u]", __FUNCTION__, __LINE__, player->data->guild_id);
		return -5;
	}
	raid_struct *guild_raid = (raid_struct *)guild_land_raid;
	guild_ruqin_reward_info_to_guildsrv(guild_raid, player->data->player_id, false);

	return 0;
}

//获取仇人追踪实时位置
int handle_friend_get_enemy_pox_info_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	FriendGetChouRenPoxRequest* req = friend_get_chou_ren_pox_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] friend get chou ren pox request unpack faild", __FUNCTION__, __LINE__);
		return -2;
	}
	uint64_t target_id = req->player_id;
	friend_get_chou_ren_pox_request__free_unpacked(req, NULL);

	uint32_t now_time = time_helper::get_cached_time() / 1000;
	int ret  = 0;
	FriendGetChouRenPoxAnswer answer;
	friend_get_chou_ren_pox_answer__init(&answer);
	do{
		bool flag = false;
		for (int i = 0; i < MAX_FRIEND_ENEMY_NUM; ++i)
		{
			if (player->data->friend_enemies[i].player_id == target_id)
			{
				flag  = true;
				if(player->data->friend_enemies[i].track_time <  now_time)
				{
					ret = 190500568;
				}
				break;
			}
		}
		if(flag == false)
		{
			LOG_ERR("[%s:%d] get enemi pos failed, no find the enemi enemi_player_id[%lu]", __FUNCTION__, __LINE__, target_id);
			return -3;
		}
		if(ret != 0)
		{
			break;
		}
		player_struct *target = player_manager::get_player_by_id(target_id);
		if (!target || !target->is_online())
		{
			ret = 190500522;
			break;
		}
		if(target->scene->get_scene_type() != SCENE_TYPE_WILD || target->sight_space)
		{
			ret = 190500520;
			break;
		}
	
		struct position *pos = target->get_pos();
		answer.player_id = target_id;
		answer.scene_id = target->data->scene_id;
		answer.pos_x = pos->pos_x;
		answer.pos_z = pos->pos_z;
	}while(0);
	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_FRIEND_TRACK_ENEMY_POX_ANSWER, friend_get_chou_ren_pox_answer__pack, answer);
	return 0;
}

//九宫八卦领奖请求
int handle_jiu_gong_ba_gua_recive_reward_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	JiuGongBaGuaReciveRewardRequest *req = jiu_gong_ba_gua_recive_reward_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());

	if(req == NULL)
	{
		LOG_ERR("[%s:%d] jiu_gong_ba_gua_recive_reward_request__unpack faild player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t reward_id = req->id;
	jiu_gong_ba_gua_recive_reward_request__free_unpacked(req, NULL);

	JiuGongBaGuaRewardInfo *reward_info = NULL;
	for(size_t i = 0; i < MAX_JIU_GONG_BA_GUA_REWARD_NUM; i++)
	{
		if(player->data->jiu_gong_ba_gua_reward[i].id == 0)
			break;
		if(player->data->jiu_gong_ba_gua_reward[i].id == reward_id)
		{
			reward_info = &player->data->jiu_gong_ba_gua_reward[i];
			break;
		}
	}
	if(reward_info == NULL)
	{
		LOG_ERR("[%s:%d] jiu gong ba gua recive reward faild, reward_id[%u] is error player_id[%lu]", __FUNCTION__, __LINE__, reward_id, extern_data->player_id);
		return -3;
	}

	NineEightTable *reward_config = get_config_by_id(reward_id, &jiu_gong_ba_gua_reward_config);
	if(reward_config == NULL)
	{
		LOG_ERR("[%s:%d] jiu gong ba gua recive reward faild, get config faild reward_id[%u] player_id[%lu]", __FUNCTION__, __LINE__, reward_id, extern_data->player_id);
		return -4;
	}

	int ret = 0;
	std::map<uint32_t, uint32_t> item_map;
	item_map.clear();
	do{
		if(reward_info->statu == Strong_State_Achieving)
		{
			ret  = 190500574; //奖励不可领
			break;
		}
		if(reward_info->statu == Strong_State_Rewarded)
		{
			ret = 190500575;  //奖励已经领取
			break;
		}

		for(uint32_t i = 0; i < reward_config->n_Reward && i < reward_config->n_RewardNum; i++)
		{
			item_map[reward_config->Reward[i]] += reward_config->RewardNum[i];
		}
		if (!player->check_can_add_item_list(item_map))
		{
			ret = 190500576; //包裹空位不足
			break;
		}
		reward_info->statu = Strong_State_Rewarded;
		player->add_item_list(item_map, MAGIC_TYPE_JIU_GONG_BA_GUA_RECIVE_REWARD);
	}while(0);

	JiuGongBaGuaReciveRewardAnswer answer;
	jiu_gong_ba_gua_recive_reward_answer__init(&answer);
	answer.result = ret;
	answer.id = reward_id;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_JIU_GONG_BA_GUA_RECIVE_REWARD_ANSWER, jiu_gong_ba_gua_recive_reward_answer__pack, answer);
	return 0;
}

//发红包请求请求
int handle_red_packet_send_red_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	RedPacketSendRequest *req = red_packet_send_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] red packet send unpack faild player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	ParameterTable *red_text_config = get_config_by_id(161001064, &parameter_config);
	if(red_text_config == NULL)
	{
		LOG_ERR("[%s:%d] send_red_packet_failed, get parame config error player_id[%lu] config_id[161001064]", __FUNCTION__, __LINE__, extern_data->player_id);
		red_packet_send_request__free_unpacked(req, NULL);
		return -3;
	}
	uint32_t red_type = req->red_type;
	uint32_t red_coin_type = req->red_coin_type;
	uint32_t red_money_num = req->money_num;
	uint32_t send_red_packet_num = req->red_num;
	if((red_type != 1 && red_type != 5) || (red_coin_type != 0 && red_coin_type != 1) || red_money_num == 0)
	{
		LOG_ERR("[%s:%d] send red packet faild, data error red_type[%u] red_coin_type[%u] red_money_num[%u]", __FUNCTION__, __LINE__, red_type, red_coin_type, red_money_num);
		red_packet_send_request__free_unpacked(req, NULL);
		return -3;
	}
	if(red_money_num < send_red_packet_num)
	{
		LOG_ERR("[%s:%d] send red packet data error red_money_num[%u] send_red_packet_num[%u]", __FUNCTION__, __LINE__, red_money_num, send_red_packet_num);
		return -4;
	}

	int ret = 0;
	do {
		if(red_type == 5 && player->data->guild_id == 0)
		{
			ret = 190500269; //帮会红包没有帮会
			break;
		}
		//判断等级是否符合
		if(player->get_level() < send_red_packet_min_level)
		{
			ret = 190500582;
			break;
		}

		//元宝红包
		if(req->red_coin_type == 0)
		{
			//判断发红包的金钱数量和个数
			if(red_money_num < sg_yuanbao_red_packet_min_money || red_money_num > sg_yuanbao_red_packet_max_money)
			{
				ret = 190500157;
				break;
			}
			if(red_type == 1)
			{
				if(send_red_packet_num < sg_world_yuanbao_red_packet_min_num || send_red_packet_num > sg_world_yuanbao_red_packet_max_num)
				{
					ret = 190500157;
					break;
				}
			}
			else 
			{
				if(send_red_packet_num < sg_guild_yuanbao_red_packet_min_num || send_red_packet_num >sg_guild_yuanbao_red_packet_max_num)
				{
					ret = 190500157;
					break;
				}
			
			}

			if(player->sub_unbind_gold(red_money_num, MAGIC_TYPE_RED_PACKET_SEND_USE_MONEY) !=0)
			{
				ret = 190400005; //元宝不足
				break;
			}
		}
		else 
		{
			if(red_money_num < sg_yinpiao_red_packet_min_money || red_money_num > sg_yinpiao_red_packet_max_money)
			{
				ret = 190500157;
				break;
			}
			if(red_type == 1)
			{
				if(send_red_packet_num < sg_world_yinpiao_red_packet_min_num || send_red_packet_num > sg_world_yinpiao_red_packet_max_num)
				{
					ret = 190500157;
					break;
				}
			}
			else 
			{
				if(send_red_packet_num < sg_guild_yuanbao_red_packet_min_num || send_red_packet_num > sg_guild_yuanbao_red_packet_max_num)
				{
					ret = 190500157;
					break;
				}
			
			}
			if(player->sub_coin(red_money_num, MAGIC_TYPE_RED_PACKET_SEND_USE_MONEY) !=0)
			{
				ret = 190500498; //银票不足
				break;
			}
		}
		RED_PACKET_SEND_DATA_REQUEST *resq = (RED_PACKET_SEND_DATA_REQUEST*)get_send_data();
		uint32_t data_len = sizeof(RED_PACKET_SEND_DATA_REQUEST);
		memset(resq, 0, data_len);
		resq->red_type = red_type;
		resq->guild_type = 0;
		resq->red_coin_type = red_coin_type;
		resq->money_num = red_money_num;
		resq->red_num = req->red_num;
		if(strlen(req->player_text) == 0)
		{
			strcpy(resq->player_text, red_text_config->parameter2);
		}
		else 
		{
			strcpy(resq->player_text, req->player_text);
		}
		resq->player_text[MAX_RED_PACKET_LIU_YAN_LEN - 1] = '\0';
		fast_send_msg_base(&conn_node_gamesrv::connecter, extern_data, SERVER_PROTO_TRADE_SEND_RED_PACKET_REQUEST, data_len, 0);

	}while(0);

	red_packet_send_request__free_unpacked(req, NULL);
	if(ret == 0)
	{
		LOG_INFO("[%s:%d] sed red packet success player_id[%lu] red_type[%u] red_coin_type[%u] red_money_num[%u] red_num[%u]", __FUNCTION__, __LINE__, extern_data->player_id, red_type, red_coin_type, red_money_num, req->red_num);
	}
	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_RED_BACKET_SEND_TO_MANY_PLAYER_ANSWER, comm_answer__pack, answer);

	return 0;
}

//抢红包给钱
int handle_red_packet_grab_red_add_money_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	RED_PACKET_PLAYER_ADD_MONEY *req = (RED_PACKET_PLAYER_ADD_MONEY*)get_data();
	uint32_t money_type = req->money_type;
	uint32_t money_num = req->money_num;
	if((money_type != 0 && money_type != 1) || money_num == 0)
	{
		LOG_ERR("[%s:%d] grad red packet add money failed, money_type[%u] error money_num[%u] error", __FUNCTION__, __LINE__, money_type, money_num);
		return -2;
	}
	
	if(money_type == 0)
	{
		player->add_unbind_gold(money_num, MAGIC_TYPE_RED_PACKET_GRAB_ADD_MONEY_MONEY);
	}
	else 
	{
		player->add_coin(money_num, MAGIC_TYPE_RED_PACKET_GRAB_ADD_MONEY_MONEY);	
	}

	/*RedPacketGrabRedAnswer answer;
	red_packet_grab_red_answer__init(&answer);
	answer.result = 0;
	answer.money_num = money_num;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_RED_BACKET_QIANG_HONGBAO_ANSWER, red_packet_grab_red_answer__pack, answer);*/

	return 0;
}

//发红包失败还钱
int handle_red_packet_send_failed_still_money_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	RED_PACKET_PLAYER_ADD_MONEY *req = (RED_PACKET_PLAYER_ADD_MONEY*)get_data();
	uint32_t money_type = req->money_type;
	uint32_t money_num = req->money_num;
	if((money_type != 0 && money_type != 1) || money_num == 0)
	{
		LOG_ERR("[%s:%d] send red packet failed still money failed, money_type[%u] error money_num[%u] error", __FUNCTION__, __LINE__, money_type, money_num);
		return -2;
	}
	
	if(money_type == 0)
	{
		player->add_unbind_gold(money_num, MAGIC_TYPE_RED_PACKET_GRAB_ADD_MONEY_MONEY);
	}
	else 
	{
		player->add_coin(money_num, MAGIC_TYPE_RED_PACKET_GRAB_ADD_MONEY_MONEY);	
	}

	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = req->result;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_RED_BACKET_SEND_TO_MANY_PLAYER_ANSWER, comm_answer__pack, answer);
	return 0;
}

void handle_updata_player_some_marry_data(EXTERN_DATA* extern_data, uint64_t player_id, uint32_t marry_statu, uint32_t   marry_type, uint64_t   marry_period)
{
	PLAYER_SOME_MARRY_DATA *resq = (PLAYER_SOME_MARRY_DATA*)get_send_data();
	uint32_t data_len = sizeof(RED_PACKET_SEND_DATA_REQUEST);
	memset(resq, 0, data_len);
	resq->player_id = player_id;
	resq->marry_statu = marry_statu;
	resq->marry_type = marry_type;
	resq->marry_period = marry_period;
	fast_send_msg_base(conn_node_dbsrv::connecter, extern_data, SERVER_PROTO_UPDATE_PLAYER_SOME_MARRY_DATA, data_len, 0);

}

//玩家求婚请求
int handle_marry_player_propose_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	int ret = 0;
	uint64_t player_id = 0; 
	uint32_t ring_type = 0;
	ret = player->player_propose_check_up(player_id, ring_type);
	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MARRY_PLAYER_PROPOSE_ANSWER, comm_answer__pack, answer);
	return 0;
}

//玩家正式求婚请求
int handle_marry_player_propose_start_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	PlayerProposeStartRequest *req = player_propose_start_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] propose start faild player_id[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	double  my_pos_x =  marry_propose_active_player_x;		  //求婚者站立的x坐标点
	double  my_pos_z =  marry_propose_active_player_z;		  //求婚者站立的z坐标点
	double  target_pos_x =  marry_propose_passive_player_x;		  //被求婚者站立的x坐标点
	double  target_pos_z =  marry_propose_passive_player_z;		  //被求婚者站立的z坐标点
	uint64_t now_time = time_helper::get_cached_time() / 1000;

	uint64_t player_id = 0;
	uint32_t ring_type = req->propose_type;
	int ret = 0;
	do{
		
		ret = player->player_propose_check_up(player_id, ring_type);
		if(ret != 0)
			break;
		struct map_block *block_start = get_map_block(player->scene->map_config, my_pos_x, my_pos_z);
		if (block_start == NULL || block_start->can_walk != true)
		{
			LOG_ERR("[%s:%d] 求婚正式开始失败,求婚者坐标点配置错误pos_x[%lf] pos_z[%lf] player[%lu]", my_pos_x, my_pos_z, extern_data->player_id);
			ret = ERROR_ID_CONFIG; //配置错误
			break;
		}
		block_start = get_map_block(player->scene->map_config, target_pos_x, target_pos_z);
		if (block_start == NULL || block_start->can_walk != true)
		{
			LOG_ERR("[%s:%d] 求婚正式开始失败,被求婚者坐标点配置错误pos_x[%lf] pos_z[%lf] player[%lu]", target_pos_x, target_pos_z, extern_data->player_id);
			ret = ERROR_ID_CONFIG; //配置错误
			break;
		}
		player_struct *target_player = player_manager::get_player_by_id(player_id);
		if(target_player == NULL || target_player->data == NULL || target_player->scene == NULL || player->scene != target_player->scene)
		{
			LOG_ERR("[%s:%d] 求婚正式开始获取被求婚者玩家对象时出错了 player[%lu]", extern_data->player_id);
			ret = ERROR_ID_SERVER;
			break;
		}

		//条件都满足,将两个人拉到指定的坐标播放动画
		int trans_ret = player->cur_scene_jump(my_pos_x, my_pos_x, 0, extern_data);		
		if(trans_ret != 0)
		{
			LOG_ERR("[%s:%d] 求婚拉求婚者失败,求婚者[%lu] 被求婚者[%lu] result[%d]", __FUNCTION__, __LINE__, extern_data->player_id, target_player->data->player_id, trans_ret);
			ret = ERROR_ID_SERVER;
			break;
		}
		EXTERN_DATA he_extern_data;
		he_extern_data.player_id = target_player->data->player_id;
		trans_ret = target_player->cur_scene_jump(target_pos_x, target_pos_z, 0, &he_extern_data);		
		if(trans_ret != 0)
		{
			LOG_ERR("[%s:%d] 求婚拉被求婚者失败,求婚者[%lu] 被求婚者[%lu] result[%d]", __FUNCTION__, __LINE__, extern_data->player_id, target_player->data->player_id, trans_ret);
			ret = ERROR_ID_SERVER;
			break;
		}
		player->data->player_marry_info.cur_propose_info.statu = MARRY_PROPOSE_IS_ACTIVE;
		player->data->player_marry_info.cur_propose_info.player_id = target_player->data->player_id;
		player->data->player_marry_info.cur_propose_info.time = now_time;
		player->data->player_marry_info.cur_propose_info.ring_type = ring_type;

		target_player->data->player_marry_info.cur_propose_info.statu = MARRY_PROPOSE_IS_PASSIVE;
		target_player->data->player_marry_info.cur_propose_info.player_id = player->data->player_id;
		target_player->data->player_marry_info.cur_propose_info.time = now_time;
		target_player->data->player_marry_info.cur_propose_info.ring_type = ring_type;

		//通知两人
		PlayerProposeStatetNotify start_notify;
		PlayerProposeStartInfo   detailed_info;
		PlayerProposePlayerPosInfo active_player;
		PlayerProposePlayerPosInfo passive_player;
		player_propose_statet_notify__init(&start_notify);
		player_propose_start_info__init(&detailed_info);
		player_propose_player_pos_info__init(&active_player);
		player_propose_player_pos_info__init(&passive_player);
		active_player.player_id = player->data->player_id;
		active_player.player_name = player->data->name;
		active_player.pos_x = my_pos_x;
		active_player.pos_z = my_pos_z;

		passive_player.player_id = target_player->data->player_id;
		passive_player.player_name = target_player->data->name;
		passive_player.pos_x = target_pos_x;
		passive_player.pos_z = target_pos_z;
		
		detailed_info.time = now_time;
		detailed_info.propose_type = ring_type;
		detailed_info.propose_enounce = req->propose_enounce;

		start_notify.type = 0;
		start_notify.propose_info = &detailed_info;
		start_notify.active_player = &active_player;
		start_notify.passive_player = &passive_player;


		uint64_t *ppp = conn_node_gamesrv::prepare_broadcast_msg_to_players(MSG_ID_MARRY_PLAYER_PROPOSE_START_NOTIFY, &start_notify, (pack_func)player_propose_statet_notify__pack);
		ppp = conn_node_gamesrv::broadcast_msg_add_players(player->data->player_id, ppp);
		ppp = conn_node_gamesrv::broadcast_msg_add_players(target_player->data->player_id, ppp);
		conn_node_gamesrv::broadcast_msg_send();

	}while(0);
	player_propose_start_request__free_unpacked(req, NULL);

	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MARRY_PLAYER_PROPOSE_START_ANSWER, comm_answer__pack, answer);
	return 0;
	
}

//玩家正式求婚请求
int handle_marry_player_propose_queren_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	PlayerProposeQuerenRequest *req = player_propose_queren_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] player_propose_queren_request__unpack failed player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t result = req->statu;
	player_propose_queren_request__free_unpacked(req, NULL);

	if(!player->is_on_propose())
	{
		LOG_ERR("[%s:%d] player confirm propose fail, not propose state, player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -3;
	}
	if(player->data->player_marry_info.cur_propose_info.statu == MARRY_PROPOSE_IS_ACTIVE)
	{
		LOG_ERR("[%s:%d] player confirm propose fail, not proposed, not operation, player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -4;
	}

	int ret = 0;
	do{
		player_struct *target_player = player_manager::get_player_by_id(player->data->player_marry_info.cur_propose_info.player_id);
		if(!target_player || !target_player->is_online())
		{
			ret = 190500606; //玩家已经下线
			break;
		}
		if(target_player->data->player_marry_info.cur_propose_info.player_id != player->data->player_marry_info.cur_propose_info.player_id)
		{
			ret = ERROR_ID_SERVER; //信息不匹配?
			break;
		}

		//拒绝的处理
		//int result = 0;
		if(result == 1)
		{
			player->player_propose_end_deal_with(false);
			break;
		}

		//同意的处理 
		uint64_t player_id = 0;
		uint32_t ring_type = target_player->data->player_marry_info.cur_propose_info.ring_type;
		ret = target_player->player_propose_check_up(player_id, ring_type);
		if(ret != 0)
			break;
		WeddingRing *wedding_ring_config = get_config_by_id(ring_type, &propose_ring_config);
		if(wedding_ring_config)
		{
			target_player->del_item_by_id(wedding_ring_config->ItemsID, 1, MAGIC_TYPE_PROPOSE_USE_RING);
		}
		target_player->sub_unbind_gold(marry_propose_min_money, MAGIC_TYPE_PROPOSE_USE_RING);


		//记录信息
		uint64_t now_time = time_helper::get_cached_time() / 1000;
		player->data->player_marry_info.statu = MARRY_STATU_HAVE_PROPOSE_NOT_RESERVE_MARRY;
		player->data->player_marry_info.my_role = MARRY_PROPOSE_IS_PASSIVE;
		player->data->player_marry_info.propose_type = ring_type;
		player->data->player_marry_info.propose_success_time = now_time;
		player->data->player_marry_info.target_id = target_player->data->player_id;
		player->data->player_marry_info.target_id = target_player->get_attr(PLAYER_ATTR_SEX);
		strcpy(player->data->player_marry_info.target_name, target_player->data->name);

		target_player->data->player_marry_info.statu = MARRY_STATU_HAVE_PROPOSE_NOT_RESERVE_MARRY;
		target_player->data->player_marry_info.my_role = MARRY_PROPOSE_IS_ACTIVE;
		target_player->data->player_marry_info.propose_type = ring_type;
		target_player->data->player_marry_info.propose_success_time = now_time;
		target_player->data->player_marry_info.target_id = player->data->player_id;
		target_player->data->player_marry_info.target_id = player->get_attr(PLAYER_ATTR_SEX);
		strcpy(target_player->data->player_marry_info.target_name, player->data->name);

		//清除信息
		player->player_propose_end_deal_with(true);
		//广播求婚信息
		PlayerProposeBroadcast broadcast;
		PlayerProposeSomeInfo active_player;
		PlayerProposeSomeInfo passive_player;
		player_propose_broadcast__init(&broadcast);
		player_propose_some_info__init(&active_player);
		player_propose_some_info__init(&passive_player);

		broadcast.type = ring_type;
		broadcast.active_player = &active_player;
		broadcast.passive_player = &passive_player;
		active_player.player_id = target_player->data->player_id;
		active_player.player_name = target_player->data->name;
		passive_player.player_id = player->data->player_id;
		passive_player.player_name = player->data->name;
		conn_node_gamesrv::send_to_all_player(MSG_ID_MARRY_PLAYER_SUCCESS_BROADCAST, &broadcast, (pack_func)player_propose_broadcast__pack);

		//通知客户端婚姻信息改变
		player->player_cur_marry_info_notify();
		target_player->player_cur_marry_info_notify();
	
	}while(0);

	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MARRY_PLAYER_PROPOSE_QUEREN_ANSWER, comm_answer__pack, answer);

	return 0;
}

//玩家预定婚期请求
int handle_marry_player_reserve_wedding_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	PlayerReserveMarriageRequest *req = player_reserve_marriage_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(!req)
	{
		LOG_ERR("[%s;%d] reserve marriage fail, unpack fail, player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}

	uint32_t marry_type = req->type;
	time_t marry_time = req->marry_time;
	player_reserve_marriage_request__free_unpacked(req, NULL);
	time_t now_time = time_helper::get_cached_time() / 1000;
	if(marry_time < now_time)
	{
		LOG_ERR("[%s;%d] reserve marriage fail, time erroe, now_time[%lu] request_time[%lu] player[%lu]", __FUNCTION__, __LINE__, now_time, marry_time, extern_data->player_id);
		return -3;
	}

	WeddingTable *marry_table = get_config_by_id(marry_type, &wedding_config);
	if(marry_table == NULL)
	{
		LOG_ERR("[%s;%d] reserve marriage fail, time erroe, request_type[%u] player[%lu]", __FUNCTION__, __LINE__, marry_type, extern_data->player_id);
		return -4;
	}	

	int ret = 0;
	do{
		//先判断是不是求婚者
		if(player->data->player_marry_info.statu != MARRY_STATU_HAVE_PROPOSE_NOT_RESERVE_MARRY)
		{
			ret = 190500630;
			break;
		}
		if(player->data->player_marry_info.my_role != MARRY_PROPOSE_IS_ACTIVE)
		{
			ret = 190500619;
			break;
		}
		struct tm marry_tm;
		localtime_r(&marry_time, &marry_tm);

		time_t propose_time = player->data->player_marry_info.propose_success_time;
		struct tm propose_tm;
		localtime_r(&propose_time, &propose_tm);

		bool flag = false;
		for(uint32_t i = 0; i < marry_table->n_Time; i++)
		{
			int config_time = marry_table->Time[i];
			if(config_time == marry_tm.tm_hour && marry_tm.tm_min == 0)
			{
				flag = true;
			}
		}
		if(flag == false)
		{
			LOG_ERR("[%s;%d] reserve marriage fail, time is no config hour[%d] min[%d] player[%lu]", __FUNCTION__, __LINE__, marry_tm.tm_hour, marry_tm.tm_min, extern_data->player_id);
			ret = 1111; //时间不在配置表中
			break;
		}
		if(marry_tm.tm_year == propose_tm.tm_year && marry_tm.tm_mon == propose_tm.tm_mon && marry_tm.tm_mday == propose_tm.tm_mday)
		{
			ret = 190500625; //可预订的婚礼时间需在第二天开始的任意天
		}

		uint32_t use_gold = marry_table->Cost;
		if(player->sub_unbind_gold(use_gold, MAGIC_TYPE_RESERVE_WEDDING_USE_MONEY) !=0)
		{
			ret = 190500318; //元宝不足
			break;
		}
		
		//记录信息
		player->data->player_marry_info.statu = MARRY_STATU_HAVE_PROPOSE_HAVE_RESERVE_MARRY;
		player->data->player_marry_info.reserve_marry_type = marry_type;
		player->data->player_marry_info.reserve_marry_time = marry_time;

		player_struct *target_player = player_manager::get_player_by_id(player->data->player_marry_info.target_id);
		//对象在线直接记录
		if(target_player != NULL && target_player->data != NULL)
		{
			target_player->data->player_marry_info.statu = MARRY_STATU_HAVE_PROPOSE_HAVE_RESERVE_MARRY;
			target_player->data->player_marry_info.reserve_marry_type = marry_type;
			target_player->data->player_marry_info.reserve_marry_time = marry_time;
		}

		//发邮件给物品
		std::vector<char *> mail_text;
		std::map<uint32_t, uint32_t> mail_item_map_man;
		std::map<uint32_t, uint32_t> mail_item_map_woman;
		char reserve_time_text[256] = {0};
		snprintf(reserve_time_text, 256, "%d年%d月%d日%d:%d", marry_tm.tm_year + 1900, marry_tm.tm_mon + 1, marry_tm.tm_mday, marry_tm.tm_hour, marry_tm.tm_min);
		mail_text.push_back(reserve_time_text);

		for(uint32_t i = 0; i < marry_table->n_RewardItem1 && i < marry_table->n_RewardNum; i++)
		{
			uint32_t item_id = marry_table->RewardItem1[i];
			uint32_t item_num = marry_table->RewardNum[i];
			mail_item_map_man[item_id] += item_num;
		}
		for(uint32_t i = 0; i < marry_table->n_RewardItem2 && i < marry_table->n_RewardNum; i++)
		{
			uint32_t item_id = marry_table->RewardItem2[i];
			uint32_t item_num = marry_table->RewardNum[i];
			mail_item_map_woman[item_id] += item_num;
		}
		if(player->get_attr(PLAYER_ATTR_SEX) == 0)
		{
			send_mail(&conn_node_gamesrv::connecter, player->data->player_id, 270100020, NULL, NULL, NULL, &mail_text, &mail_item_map_man, MAGIC_TYPE_RESERVE_WEDDING_GIVE_ITEM);
			
		}
		else 
		{
			send_mail(&conn_node_gamesrv::connecter, player->data->player_id, 270100020, NULL, NULL, NULL, &mail_text, &mail_item_map_woman, MAGIC_TYPE_RESERVE_WEDDING_GIVE_ITEM);
		}
		if(player->data->player_marry_info.sex == 0)
		{
			send_mail(&conn_node_gamesrv::connecter, player->data->player_marry_info.target_id, 270100020, NULL, NULL, NULL, &mail_text, &mail_item_map_man, MAGIC_TYPE_RESERVE_WEDDING_GIVE_ITEM);
			
		}
		else 
		{
			send_mail(&conn_node_gamesrv::connecter, player->data->player_marry_info.target_id, 270100020, NULL, NULL, NULL, &mail_text, &mail_item_map_woman, MAGIC_TYPE_RESERVE_WEDDING_GIVE_ITEM);
		}
		//通知客户端婚姻信息改变
		player->player_cur_marry_info_notify();
		if(target_player != NULL)
		{
			target_player->player_cur_marry_info_notify();
		}
		else 
		{
			//不在线要更新mysql
			handle_updata_player_some_marry_data(extern_data, player->data->player_marry_info.target_id, MARRY_STATU_HAVE_PROPOSE_HAVE_RESERVE_MARRY, marry_type, marry_time);
		}


	}while(0);

	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MARRY_PLAYER_RESERVE_WEDDING_ANSWER, comm_answer__pack, answer);


	return 0;
}

//玩家取消订婚请求
int handle_marry_player_cancel_propose_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	PlayerCancelProposeRequest *req = player_cancel_propose_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());
	if(req == NULL)
	{
		LOG_ERR("[%s:%d] cancel propose request unpack fail[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t statu = req->statu;
	player_cancel_propose_request__free_unpacked(req, NULL);

	if(player->data->player_marry_info.statu != MARRY_STATU_HAVE_PROPOSE_NOT_RESERVE_MARRY || player->data->player_marry_info.statu != MARRY_STATU_HAVE_PROPOSE_HAVE_RESERVE_MARRY)
	{
		LOG_ERR("[%s:%d] player cancel propose fail, marry_statu[%u] player[%lu]", __FUNCTION__, __LINE__, player->data->player_marry_info.statu, extern_data->player_id);
		return -3;
	}
	if(player->data->player_marry_info.target_id == 0)
	{
		LOG_ERR("[%s:%d] player cancel propose fail, target player_id error player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -4;
	}
	WeddingTable *marry_table = NULL;
	if(player->data->player_marry_info.statu == MARRY_STATU_HAVE_PROPOSE_HAVE_RESERVE_MARRY)
	{
		uint32_t reserve_marry_type = player->data->player_marry_info.reserve_marry_type;
		marry_table = get_config_by_id(reserve_marry_type, &wedding_config);
		if(marry_table == NULL)
		{
			LOG_ERR("[%s;%d] cancel propose fail, get wedding table error, reserve_marry_type[%u] player[%lu]", __FUNCTION__, __LINE__, reserve_marry_type, extern_data->player_id);
			return -5;
		}	
	}

	int ret = 0;
	do{
		player_struct *target_player = player_manager::get_player_by_id(player->data->player_marry_info.target_id);
		//协议取消订婚
		if(statu == 0)
		{
			bool flag = false; //是否跟自己的对象组队
			if(player->m_team != NULL)
			{
				for(int pos = 0; pos < player->m_team->m_data->m_memSize && pos < 2; pos++)
				{
					if(player->m_team->m_data->m_mem[pos].id == player->data->player_marry_info.target_id)
					{
						flag = true;
						break;
					}
				}
			}
			if(flag == false)
			{
				ret = 190500629; //未组队
				break;
			}

			if(!target_player || !target_player->is_online())
			{
				ret = 190500626;
				break;
			}
			player->data->player_marry_info.cur_cancel_propose = true;
			target_player->data->player_marry_info.cur_cancel_propose = true;
			CommAnswer answer;
			comm_answer__init(&answer);

			fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MARRY_PLAYER_CANCEL_PROPOSE_NOTYFY, comm_answer__pack, answer);
			break;
		}

		//强制取消订婚
		if(player->sub_unbind_gold(marry_cancel_propose_marry_use_money, MAGIC_TYPE_CANCEL_PROPOSE_MARRY_USE_MONEY) !=0)
		{
			ret = 190500318; //元宝不足
			break;
		}
		uint64_t target_id = player->data->player_marry_info.target_id;
		char target_name[MAX_PLAYER_NAME_LEN + 1] = {0};
		strcpy(target_name, player->data->player_marry_info.target_name);
		player->clean_player_all_marry_info(false);
		player->clean_player_friend_closeness(target_id);
		player->player_cur_marry_info_notify();

		bool online = false;
		if(target_player != NULL && target_player->is_online())
		{
			online = true;
			target_player->clean_player_all_marry_info(false);
			target_player->player_cur_marry_info_notify();
		}
		else 
		{
			//不在线要更新mysql
			handle_updata_player_some_marry_data(extern_data, target_id, 0, 0, 0);
		}
		time_t now_time = time_helper::get_cached_time() / 1000;
		struct tm now_tm;
		localtime_r(&now_time, &now_tm);
		std::vector<char *> mail_text1;
		std::vector<char *> mail_text2;
		std::map<uint32_t, uint32_t> mail_item_map;
		mail_text1.clear();
		mail_text2.clear();
		mail_item_map.clear();
		char reserve_time_text[256] = {0};
		snprintf(reserve_time_text, 256, "%d年%d月%d日%d:%d", now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min);
		mail_text1.push_back(target_name);
		mail_text1.push_back(reserve_time_text);
		send_mail(&conn_node_gamesrv::connecter, player->data->player_id, 270300021, NULL, NULL, NULL, &mail_text1, &mail_item_map, MAGIC_TYPE_CANCEL_PROPOSE_MARRY_MAIL);

		mail_text2.push_back(player->data->name);
		mail_text2.push_back(reserve_time_text);
		send_mail(&conn_node_gamesrv::connecter, target_id, 270300021, NULL, NULL, NULL, &mail_text2, &mail_item_map, MAGIC_TYPE_CANCEL_PROPOSE_MARRY_MAIL);

		if(marry_table != NULL)
		{
			//另外把预定婚期的发费返回一部分给订婚的玩家
			std::vector<char *> mail_text3;
			std::map<uint32_t, uint32_t> mail_item_map1;
			mail_item_map.clear();
			uint64_t active_id = 0;
			char active_name[MAX_PLAYER_NAME_LEN + 1] = {0};
			if(player->data->player_marry_info.my_role == MARRY_PROPOSE_IS_ACTIVE)
			{
				active_id = player->data->player_id;
				strcpy(active_name, target_name);
			}
			else 
			{
				active_id = target_id;
				strcpy(active_name, player->data->name);
			}
			uint32_t return_money = marry_table->Cost * (double)marry_propose_cancle_return_money/(double)100;
			mail_text3.push_back(active_name);
			mail_text3.push_back(reserve_time_text);
			mail_item_map1[201010003] = return_money;
			send_mail(&conn_node_gamesrv::connecter, active_id, 270300022, NULL, NULL, NULL, &mail_text3, &mail_item_map1, MAGIC_TYPE_CANCEL_PROPOSE_MARRY_MAIL);
		}

		PlayerCancelProposeResultNotify notify;
		player_cancel_propose_result_notify__init(&notify);
		notify.result = 0;
		uint64_t *ppp = conn_node_gamesrv::prepare_broadcast_msg_to_players(MSG_ID_MARRY_PLAYER_CANCEL_RESULT_NOTYFY, &notify, (pack_func)player_cancel_propose_result_notify__pack);
		ppp = conn_node_gamesrv::broadcast_msg_add_players(player->data->player_id, ppp);
		if(online == true)
		{
			ppp = conn_node_gamesrv::broadcast_msg_add_players(target_id, ppp);
		}
		conn_node_gamesrv::broadcast_msg_send();

	}while(0);

	CommAnswer answer;
	comm_answer__init(&answer);

	answer.result = ret;
	fast_send_msg(&conn_node_gamesrv::connecter, extern_data, MSG_ID_MARRY_PLAYER_CANCEL_PROPOSE_ANSWER, comm_answer__pack, answer);

	return 0;
}

//玩家确认取消订婚请求
int handle_marry_player_cancel_confirm_request(player_struct* player, EXTERN_DATA* extern_data)
{
	if (!player || !player->is_online())
	{
		LOG_ERR("[%s:%d] can not find player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	PlayerCancelProposeConfirmRequest *req = player_cancel_propose_confirm_request__unpack(NULL, get_data_len(), (uint8_t*)get_data());

	if(req == NULL)
	{
		LOG_ERR("[%s:%d] confirm cancel propose unpack fail player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -2;
	}
	uint32_t result = req->result;
	player_cancel_propose_confirm_request__free_unpacked(req, NULL);
	if(player->data->player_marry_info.statu != MARRY_STATU_HAVE_PROPOSE_NOT_RESERVE_MARRY || player->data->player_marry_info.statu != MARRY_STATU_HAVE_PROPOSE_HAVE_RESERVE_MARRY)
	{
		LOG_ERR("[%s:%d] player confirm cancel propose fail, marry_statu[%u] player[%lu]", __FUNCTION__, __LINE__, player->data->player_marry_info.statu, extern_data->player_id);
		return -3;
	}
	uint64_t target_id = player->data->player_marry_info.target_id;
	if(target_id == 0)
	{
		LOG_ERR("[%s:%d] player confirm cancel propose fail, target player_id error player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -4;
	}
	if(player->data->player_marry_info.cur_cancel_propose == false)
	{
		LOG_ERR("[%s:%d] player confirm cancel propose fail, target not cancel player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
		return -5;
	}
	WeddingTable *marry_table = NULL;
	if(player->data->player_marry_info.statu == MARRY_STATU_HAVE_PROPOSE_HAVE_RESERVE_MARRY)
	{
		uint32_t reserve_marry_type = player->data->player_marry_info.reserve_marry_type;
		marry_table = get_config_by_id(reserve_marry_type, &wedding_config);
		if(marry_table == NULL)
		{
			LOG_ERR("[%s;%d] cancel propose fail, get wedding table error, reserve_marry_type[%u] player[%lu]", __FUNCTION__, __LINE__, reserve_marry_type, extern_data->player_id);
			return -6;
		}	
	}


	bool online = false;
	player_struct *target_player = player_manager::get_player_by_id(target_id);
	if(target_player != NULL && target_player->is_online())
	{
		online = true;
	}
	if(result == 0)
	{
		char target_name[MAX_PLAYER_NAME_LEN + 1] = {0};
		strcpy(target_name, player->data->player_marry_info.target_name);
		player->clean_player_all_marry_info(false);
		player->clean_player_friend_closeness(target_id);
		player->player_cur_marry_info_notify();
	
		if(online == true)
		{
			target_player->clean_player_all_marry_info(false);
			target_player->player_cur_marry_info_notify();
		}
		else 
		{
			//不在线要更新mysql
			handle_updata_player_some_marry_data(extern_data, target_id, 0, 0, 0);
		}
		//发邮件通知取消订婚
		time_t now_time = time_helper::get_cached_time() / 1000;
		struct tm now_tm;
		localtime_r(&now_time, &now_tm);
		std::vector<char *> mail_text1;
		std::vector<char *> mail_text2;
		std::map<uint32_t, uint32_t> mail_item_map;
		mail_text1.clear();
		mail_text2.clear();
		mail_item_map.clear();
		char reserve_time_text[256] = {0};
		snprintf(reserve_time_text, 256, "%d年%d月%d日%d:%d", now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min);
		mail_text1.push_back(target_name);
		mail_text1.push_back(reserve_time_text);
		send_mail(&conn_node_gamesrv::connecter, player->data->player_id, 270300021, NULL, NULL, NULL, &mail_text1, &mail_item_map, MAGIC_TYPE_CANCEL_PROPOSE_MARRY_MAIL);

		mail_text2.push_back(player->data->name);
		mail_text2.push_back(reserve_time_text);
		send_mail(&conn_node_gamesrv::connecter, target_id, 270300021, NULL, NULL, NULL, &mail_text2, &mail_item_map, MAGIC_TYPE_CANCEL_PROPOSE_MARRY_MAIL);

		if(marry_table != NULL)
		{
			//另外把预定婚期的发费返回一部分给订婚的玩家
			std::vector<char *> mail_text3;
			std::map<uint32_t, uint32_t> mail_item_map1;
			mail_item_map.clear();
			uint64_t active_id = 0;
			char active_name[MAX_PLAYER_NAME_LEN + 1] = {0};
			if(player->data->player_marry_info.my_role == MARRY_PROPOSE_IS_ACTIVE)
			{
				active_id = player->data->player_id;
				strcpy(active_name, target_name);
			}
			else 
			{
				active_id = target_id;
				strcpy(active_name, player->data->name);
			}
			uint32_t return_money = marry_table->Cost * (double)marry_propose_cancle_return_money/(double)100;
			mail_text3.push_back(active_name);
			mail_text3.push_back(reserve_time_text);
			mail_item_map1[201010003] = return_money;
			send_mail(&conn_node_gamesrv::connecter, active_id, 270300022, NULL, NULL, NULL, &mail_text3, &mail_item_map1, MAGIC_TYPE_CANCEL_PROPOSE_MARRY_MAIL);
		}
	
	}


	PlayerCancelProposeResultNotify notify;
	player_cancel_propose_result_notify__init(&notify);
	notify.result = 0;
	if(result != 0)
	{
		notify.result = 190500627;
		notify.player_id = player->data->player_id;
	}
	uint64_t *ppp = conn_node_gamesrv::prepare_broadcast_msg_to_players(MSG_ID_MARRY_PLAYER_CANCEL_RESULT_NOTYFY, &notify, (pack_func)player_cancel_propose_result_notify__pack);
	ppp = conn_node_gamesrv::broadcast_msg_add_players(player->data->player_id, ppp);
	if(online == true)
	{
		ppp = conn_node_gamesrv::broadcast_msg_add_players(target_id, ppp);
	}
	conn_node_gamesrv::broadcast_msg_send();
	return 0;
}

void install_msg_handle()
{
	add_msg_handle(MSG_ID_MOVE_REQUEST, handle_move_request);
	add_msg_handle(MSG_ID_SKILL_CAST_REQUEST, handle_skill_cast_request);
	add_msg_handle(MSG_ID_PARTNER_SKILL_CAST_REQUEST, handle_partner_skill_cast_request);	
	add_msg_handle(MSG_ID_SKILL_HIT_REQUEST, handle_skill_hit_request);
//	add_msg_handle(MSG_ID_CALL_ATTACK_REQUEST, handle_skill_call_attack_request);
//	add_msg_handle(MSG_ID_SKILL_MOVE_REQUEST, handle_skill_move_request);
	add_msg_handle(MSG_ID_LEARN_SKILL_REQUEST, handle_learn_skill_request);
	add_msg_handle(MSG_ID_LEARN_FUWEN_REQUEST, handle_learn_fuwen_request);
	add_msg_handle(MSG_ID_SET_SKILL_MENU_REQUEST, handle_set_skill_menu_request);
	add_msg_handle(MSG_ID_SET_FUWEN_OLD_REQUEST, handle_set_fuwen_old_request);
	add_msg_handle(MSG_ID_OPEN_SKILL_FOR_GUIDE, handle_open_skill_for_guide_request);
	add_msg_handle(MSG_ID_SET_FUWEN_REQUEST, handle_set_fuwen_request);
	add_msg_handle(MSG_ID_LEARN_LIVE_SKILL_REQUEST, handle_learn_live_skill_request);
	add_msg_handle(MSG_ID_LIVE_SKILL_BREAK_REQUEST, handle_live_skill_break_request);
	add_msg_handle(MSG_ID_PRODUCE_MEDICINE_REQUEST, handle_produce_medicine_request);
	add_msg_handle(MSG_ID_ADD_SPEED_BUFF_REQUEST, handle_add_speed_buff_request);
	add_msg_handle(MSG_ID_DEL_SPEED_BUFF_REQUEST, handle_del_speed_buff_request);
	add_msg_handle(MSG_ID_BOATING_START_REQUEST, handle_boating_start_request);
	add_msg_handle(MSG_ID_BOATING_STOP_REQUEST, handle_boating_stop_request);
	add_msg_handle(MSG_ID_ONE_WAY_TRANSFER_START_REQUEST, handle_one_way_trans_start_request);
	add_msg_handle(MSG_ID_ONE_WAY_TRANSFER_STOP_REQUEST, handle_one_way_trans_stop_request);

	//冲塔
	add_msg_handle(MSG_ID_RESET_TOWER_CUR_LEVEL_REQUEST, handle_reset_tower_cur_level_request);
	add_msg_handle(MSG_ID_CLEAN_TOWER_REQUEST, handle_clean_tower_request);
	add_msg_handle(MSG_ID_NEXT_TOWER_LEVEL_REQUEST, handle_next_tower_request);
	add_msg_handle(MSG_ID_TOWER_MAX_REQUEST, handle_tower_max_request);

	//镖车
	add_msg_handle(MSG_ID_ACCEPT_CASH_TRUCK_REQUEST, handle_accept_cash_truck_request);
	add_msg_handle(MSG_ID_GET_ON_CASH_TRUCK_REQUEST, handle_get_on_cash_truck_request);
	add_msg_handle(MSG_ID_GO_DOWND_CASH_TRUCK_REQUEST, handle_go_downd_cash_truck_request);
	//add_msg_handle(MSG_ID_SUBMIT_CASH_TRUCK_REQUEST, handle_submit_cash_truck_request);
	add_msg_handle(MSG_ID_CASH_TRUCK_SPEED_UP_REQUEST, handle_cash_truck_speed_up_request);

	//答题
	add_msg_handle(MSG_ID_GET_COMMON_QUESTION_REQUEST, handle_get_common_question_request);
	add_msg_handle(MSG_ID_ANSWER_COMMON_QUESTION_REQUEST, handle_answer_common_question_request);
	add_msg_handle(MSG_ID_COMMON_QUESTION_HINT_REQUEST, handle_common_question_hint_request);
	add_msg_handle(MSG_ID_COMMON_QUESTION_HELP_REQUEST, handle_common_question_help_request);
	add_msg_handle(MSG_ID_GET_AWARD_QUESTION_REQUEST, handle_get_award_question_request);
	add_msg_handle(MSG_ID_ANSWER_AWARD_QUESTION_REQUEST, handle_answer_award_question_request);
	add_msg_handle(MSG_ID_END_AWARD_QUESTION_REQUEST, handle_interupt_award_question_request);
	add_msg_handle(MSG_ID_FIRST_AWARD_QUESTION_REQUEST, handle_first_award_question_request);

	add_msg_handle(MSG_ID_XUNBAO_POS_REQUEST, handle_xunbao_pos_request);

	//阵营
	add_msg_handle(MSG_ID_CHOSE_ZHENYING_REQUEST, handle_chose_zhenying_request);
	add_msg_handle(MSG_ID_CHANGE_ZHENYING_REQUEST, handle_change_zhenying_request);
	add_msg_handle(MSG_ID_ZHENYING_POWER_REQUEST, handle_zhenying_power_request);
	add_msg_handle(MSG_ID_INTO_ZHENYING_BATTLE_REQUEST, handle_into_zhenying_battle_request);
	add_msg_handle(MSG_ID_ZHENYING_TEAM_INFO_REQUEST, handle_zhenying_team_info_request);
//	add_msg_handle(MSG_ID_EXIT_ZHENYING_BATTLE_REQUEST, handle_exit_zhenying_battle_request);
	add_msg_handle(MSG_ID_GET_ZHENYING_TASK_AWARD_REQUEST, handle_get_zhenying_task_award_request);
	add_msg_handle(MSG_ID_ZHENYING_GET_LINE_INFO_REQUEST, handle_zhenying_get_line_info_request);
	add_msg_handle(MSG_ID_ZHENYING_CHANGE_LINE_REQUEST, handle_zhenying_change_line_request);
	add_msg_handle(MSG_ID_GET_ZHENYING_DAYLY_AWARD_REQUEST, handle_get_zhenying_dayli_award_request);
	add_msg_handle(MSG_ID_RAND_CHOSE_ZHENYING_REQUEST, handle_rand_chose_zhenying_request);

	add_msg_handle(MSG_ID_JOIN_ZHENYING_FIGHT_REQUEST, handle_join_zhenying_battle_request); 
	add_msg_handle(MSG_ID_CANCEL_JOIN_ZHENYING_FIGHT_REQUEST, handle_cancel_join_zhenying_battle_request);
	add_msg_handle(MSG_ID_INTO_ZHENYING_FIGHT_REQUEST, handle_into_zhenying_fight_request);
	add_msg_handle(MSG_ID_ZHENYING_FIGHT_SET_READY_STATE_REQUEST, handle_zhenying_fight_set_ready_request);
	add_msg_handle(MSG_ID_ZHENYING_FIGHT_READY_STATE_REQUEST, handle_zhenying_fight_ready_state_request);
	add_msg_handle(MSG_ID_ZHENYING_FIGHT_MYSIDE_SCORE_REQUEST, handle_zhenying_fight_myside_score_request);

	//妖师国御
	add_msg_handle(MSG_ID_ACCECT_GUOYU_TASK_REQUEST, handle_accect_guoyu_task_request);
	add_msg_handle(MSG_ID_AGREED_GUOYU_TASK_REQUEST, handle_agreed_guoyu_task_request);
	add_msg_handle(MSG_ID_GIVEUP_GUOYU_TASK_REQUEST, handle_giveup_guoyu_task_request);
	add_msg_handle(MSG_ID_SET_SPECIAL_REQUEST, handle_set_special_request);
	add_msg_handle(MSG_ID_GUOYU_BOSS_APPEAR_REQUEST, handle_guoyu_boss_appear_request);
	//add_msg_handle(MSG_ID_ENTER_GUOYU_FB_REQUEST, handle_enter_guoyu_fb_request);
	//妖师惩戒
	add_msg_handle(MSG_ID_CHENGJIE_FIND_TARGET_REQUEST, handle_chengjie_find_target_request);
	add_msg_handle(MSG_ID_REFRESH_CHENGJIE_LIST_REQUEST, handle_refresh_chengjie_list_request);
	add_msg_handle(MSG_ID_ADD_CHENGJIE_TASK_REQUEST, handle_add_chengjie_task_request);
	add_msg_handle(MSG_ID_ACCEPT_CHENGJIE_TASK_REQUEST, handle_accept_chengjie_task_request);
	add_msg_handle(MSG_ID_SUBMIT_CHENGJIE_TASK_REQUEST, handle_submit_chengjie_task_request);
	//妖师赏金
	add_msg_handle(MSG_ID_ACCEPT_SHANGJIN_TASK_REQUEST, handle_accept_shangjin_task_request);
	add_msg_handle(MSG_ID_REFRESH_SHANGJIN_TASK_REQUEST, handle_refresh_shangjin_task_request);

	//坐骑
	add_msg_handle(MSG_ID_BUY_HORSE_REQUEST, handle_buy_horse_request);
	add_msg_handle(MSG_ID_SET_CUR_HORSE_REQUEST, handle_set_cur_horse_request);
	add_msg_handle(MSG_ID_ON_HORSE_REQUEST, handle_on_horse_request);
	add_msg_handle(MSG_ID_DOWN_HORSE_REQUEST, handle_down_horse_request);
	add_msg_handle(MSG_ID_ADD_HORSE_EXP_REQUEST, handle_add_horse_exp_request);
	add_msg_handle(MSG_ID_ADD_HORSE_STEP_REQUEST, handle_add_horse_step_request);
	add_msg_handle(MSG_ID_ADD_HORSE_SOUL_REQUEST, handle_add_horse_soul_request);
	//add_msg_handle(MSG_ID_ADD_HORSE_SOUL_LEVEL_REQUEST, handle_add_horse_soul_level_request);
	add_msg_handle(MSG_ID_SET_HORSE_OLD_REQUEST, handle_set_horse_old_request);
	add_msg_handle(MSG_ID_SET_HORSE_FLY_REQUEST, handle_set_horse_fly_request);
	add_msg_handle(MSG_ID_HORSE_RESTORE_REQUEST, handle_horse_restory_request);

	//时装
	add_msg_handle(MSG_ID_BUY_FASHION_REQUEST, handle_buy_fashion_request);
	add_msg_handle(MSG_ID_UNLOCK_COLOR_REQUEST, handle_unlock_color_request);
	add_msg_handle(MSG_ID_UNLOCK_WEAPON_COLOR_REQUEST, handle_unlock_weapon_color_request);
	add_msg_handle(MSG_ID_SET_FASHION_COLOR_REQUEST, handle_set_fashion_color_request);
	add_msg_handle(MSG_ID_SET_WEAPON_COLOR_REQUEST, handle_set_weapon_color_request);
	add_msg_handle(MSG_ID_PUTON_FASHION_REQUEST, handle_puton_fashion_request);
	add_msg_handle(MSG_ID_TAKEOFF_FASHION_REQUEST, handle_takeoff_fashion_request);
	add_msg_handle(MSG_ID_FASHION_OLD_REQUEST, handle_set_fashion_old_request);
	add_msg_handle(MSG_ID_SET_COLOR_OLD_REQUEST, handle_set_color_old_request);
	add_msg_handle(MSG_ID_SET_WEAPON_COLOR_OLD_REQUEST, handle_set_weapon_color_old_request);

	//组队
	add_msg_handle(MSG_ID_CREATE_TEAM_REQUEST, handle_create_team_request);
	add_msg_handle(MSG_ID_APPLY_TEAM_REQUEST, handle_apply_team_request);
	add_msg_handle(MSG_ID_HANDLE_APPLY_TEAM_REQUEST, handle_handle_apply_team_request);
	add_msg_handle(MSG_ID_HANDLE_REFUCE_APPLY_TEAM_REQUEST, handle_handle_refuce_apply_team_request);
	add_msg_handle(MSG_ID_KICK_TEAM_REQUEST, handle_kick_team_mem_request);
	add_msg_handle(MSG_ID_QUIT_TEAM_REQUEST, handle_quit_team_request);
	add_msg_handle(MSG_ID_TEAM_BE_LEAD_REQUEST, handle_team_be_lead_request);
	add_msg_handle(MSG_ID_TEAM_CHANGE_LEAD_REQUEST, handle_team_change_lead_request);
	add_msg_handle(MSG_ID_TEAM_LIMITED_REQUEST, handle_team_setlimit_request);
	add_msg_handle(MSG_ID_INVITE_TEAM_REQUEST, handle_team_invite_request);
	add_msg_handle(MSG_ID_HANDLEINVITE_TEAM_REQUEST, handle_team_invite_handle_request);
	add_msg_handle(MSG_ID_TEAM_LIST_REQUEST, handle_team_list_request);
	//add_msg_handle(MSG_ID_TEAM_INFO_REQUEST, handle_team_info_request);
	add_msg_handle(MSG_ID_TEAM_HANDLE_BE_LEAD_REQUEST, handle_be_team_lead_request);
	add_msg_handle(MSG_ID_TEAM_MATCH_REQUEST, handle_team_match_request);
	add_msg_handle(MSG_ID_TEAM_CANCEL_MATCH_REQUEST, handle_cancel_team_match_request);
	add_msg_handle(MSG_ID_TEAM_LEAD_POS_REQUEST, handle_team_lead_pos_request);
	add_msg_handle(MSG_ID_TEAM_SET_FOLLOW_REQUEST, handle_team_set_follow_request);
	add_msg_handle(MSG_ID_TEAM_SUMMON_MEM_REQUEST, handle_team_summon_mem_request);
	add_msg_handle(MSG_ID_TEAM_SPEEK_REQUEST, handle_team_speek_request);

	add_msg_handle(MSG_ID_TRANSFER_REQUEST, handle_transfer_request);
	add_msg_handle(MSG_ID_ENTER_SCENE_READY_REQUEST, handle_enter_scene_ready_request);
	add_msg_handle(MSG_ID_TRANSFER_TO_PLAYER_SCENE_REQUEST, handle_transfer_to_player_scene_request);

	add_msg_handle(SERVER_PROTO_ENTER_GAME_REQUEST, handle_enter_game);
	add_msg_handle(SERVER_PROTO_KICK_ROLE_NOTIFY, handle_kick_player);

	add_msg_handle(MSG_ID_MOVE_START_REQUEST, handle_move_start_request);
	add_msg_handle(MSG_ID_MOVE_STOP_REQUEST, handle_move_stop_request);

	add_msg_handle(MSG_ID_MOVE_Y_START_REQUEST, handle_move_y_start_request);
	add_msg_handle(MSG_ID_MOVE_Y_STOP_REQUEST, handle_move_y_stop_request);

	add_msg_handle(MSG_ID_OPEN_BIG_MAP_REQUEST, handle_open_big_map_request);

	add_msg_handle(MSG_ID_BAG_INFO_REQUEST, handle_bag_info_request);
	add_msg_handle(MSG_ID_BAG_UNLOCK_GRID_REQUEST, handle_bag_unlock_grid_request);
	add_msg_handle(MSG_ID_BAG_SELL_REQUEST, handle_bag_sell_request);
	add_msg_handle(MSG_ID_BAG_USE_REQUEST, handle_bag_use_request);
	add_msg_handle(MSG_ID_BAG_STACK_REQUEST, handle_bag_stack_request);
	add_msg_handle(MSG_ID_BAG_TIDY_REQUEST, handle_bag_tidy_request);
	add_msg_handle(MSG_ID_BAG_BATCH_SELL_REQUEST, handle_bag_batch_sell_request);
	add_msg_handle(MSG_ID_BAG_ITEM_COMBIN_REQUEST, handle_bag_item_combin_request);

	add_msg_handle(MSG_ID_RELIVE_REQUEST, handle_relive_request);
	add_msg_handle(MSG_ID_CHAT_REQUEST, handle_chat_request);
	add_msg_handle(MSG_ID_CHAT_BROADCAST_REQUEST, handle_chat_broadcast_request);
	add_msg_handle(MSG_ID_PRIVATE_CHAT_FIND_TARGET_REQUEST, handle_private_chat_find_target_request);
	add_msg_handle(MSG_ID_LATELY_CHAT_REQUEST, handle_lately_chat_request);
	add_msg_handle(MSG_ID_CHAT_SHOW_EQUIP_REQUEST, handle_chat_show_equip_request);
	add_msg_handle(MSG_ID_CHAT_SHOW_PARTNER_REQUEST, handle_chat_show_partner_request);	

	add_msg_handle(MSG_ID_PLAYER_RENAME_REQUEST, handle_rename_request);

	add_msg_handle(MSG_ID_HEAD_ICON_INFO_REQUEST, handle_head_icon_info_request);
	add_msg_handle(MSG_ID_HEAD_ICON_REPLACE_REQUEST, handle_head_icon_replace_request);
	add_msg_handle(MSG_ID_HEAD_ICON_OLD_REQUEST, handle_head_icon_old_request);
	add_msg_handle(MSG_ID_COLLECT_BEGIN_REQUEST, handle_gather_request);
	add_msg_handle(MSG_ID_COLLECT_COMMPLETE_REQUEST, handle_gather_complete);
	add_msg_handle(MSG_ID_COLLECT_INTERRUPT_REQUEST, handle_gather_interupt);
//	add_msg_handle(MSG_ID_BAG_SHOW_REQUEST, handle_bag_show_request);

	add_msg_handle(MSG_ID_TASK_LIST_REQUEST, handle_task_list_request);
	add_msg_handle(MSG_ID_TASK_ACCEPT_REQUEST, handle_task_accept_request);
	add_msg_handle(MSG_ID_TASK_SUBMIT_REQUEST, handle_task_submit_request);
	add_msg_handle(MSG_ID_TASK_ABANDON_REQUEST, handle_task_abandon_request);
	add_msg_handle(MSG_ID_TASK_MONSTER_REQUEST, handle_task_monster_request);
	add_msg_handle(MSG_ID_TASK_COMPLETE_REQUEST, handle_task_complete_request);
	add_msg_handle(MSG_ID_TASK_FAIL_REQUEST, handle_task_fail_request);
	add_msg_handle(MSG_ID_TASK_CHAPTER_REWARD_REQUEST, handle_task_chapter_reward_request);
	add_msg_handle(MSG_ID_TASK_ENTER_PLANES_REQUEST, handle_task_enter_planes_request);

	//add_msg_handle(MSG_ID_SET_FASHION, handle_set_fashion_request);

	add_msg_handle(MSG_ID_SING_INTERRUPT_REQUEST, handle_sing_interrupt_request);
	add_msg_handle(MSG_ID_SING_END_REQUEST, handle_sing_end_request);
	add_msg_handle(MSG_ID_SING_BEGIN_REQUEST, handle_sing_begin_request);

	add_msg_handle(MSG_ID_ENTER_RAID_REQUEST, handle_enter_raid_request);
	add_msg_handle(MSG_ID_TRANSFER_TO_LEADER_REQUEST, handle_transfer_to_leader_request);
	add_msg_handle(MSG_ID_LEAVE_RAID_REQUEST, handle_leave_raid_request);
	add_msg_handle(MSG_ID_TRANSFER_FAR_TEAM_MEMBER_REQUEST, handle_transfer_far_team_member_request);
	add_msg_handle(MSG_ID_TEAM_RAID_READY_REQUEST, handle_team_raid_ready_request);
	add_msg_handle(MSG_ID_TEAM_RAID_CANCEL_REQUEST, handle_team_raid_cancel_request);
	add_msg_handle(MSG_ID_GET_RAID_MONSTER_POS_REQUEST, handle_get_raid_monster_pos_request);
	add_msg_handle(MSG_ID_COMPLETE_RAID_AI_REQUEST, handle_complete_raid_ai_requrest);
	

	add_msg_handle(MSG_ID_EQUIP_LIST_REQUEST, handle_equip_list_request);
	add_msg_handle(MSG_ID_EQUIP_STAR_UP_REQUEST, handle_equip_star_up_request);
	add_msg_handle(MSG_ID_EQUIP_STAIR_UP_REQUEST, handle_equip_stair_up_request);
	add_msg_handle(MSG_ID_EQUIP_ENCHANT_REQUEST, handle_equip_enchant_request);
	add_msg_handle(MSG_ID_EQUIP_ENCHANT_RETAIN_REQUEST, handle_equip_enchant_retain_request);
	add_msg_handle(MSG_ID_EQUIP_DRILL_REQUEST, handle_equip_drill_request);
	add_msg_handle(MSG_ID_EQUIP_INLAY_REQUEST, handle_equip_inlay_request);
	add_msg_handle(MSG_ID_EQUIP_STRIP_REQUEST, handle_equip_strip_request);
	add_msg_handle(MSG_ID_EQUIP_GEM_COMPOSE_REQUEST, handle_equip_gem_compose_request);
	add_msg_handle(MSG_ID_EQUIP_GEM_ONEKEY_COMPOSE_REQUEST, handle_equip_gem_onekey_compose_request);

	add_msg_handle(MSG_ID_LEAVE_PLANES_RAID_REQUEST, handle_planes_raid_request);

	add_msg_handle(SERVER_PROTO_MAIL_GIVE_ATTACH_REQUEST, handle_mail_give_attach_request);

	add_msg_handle(MSG_ID_SHOP_INFO_REQUEST, handle_shop_info_request);
	add_msg_handle(MSG_ID_SHOP_BUY_REQUEST, handle_shop_buy_request);

	add_msg_handle(MSG_ID_YUQIDAO_INFO_REQUEST, handle_yuqidao_info_request);
	add_msg_handle(MSG_ID_YUQIDAO_FILL_REQUEST, handle_yuqidao_fill_request);
	add_msg_handle(MSG_ID_YUQIDAO_BREAK_REQUEST, handle_yuqidao_break_request);
	add_msg_handle(MSG_ID_YUQIDAO_BREAK_RETAIN_REQUEST, handle_yuqidao_break_retain_request);

	add_msg_handle(MSG_ID_SET_PK_TYPE_REQUEST, handle_set_pk_type_request);
	add_msg_handle(MSG_ID_QIECUO_REQUEST, handle_qiecuo_request);
	add_msg_handle(MSG_ID_QIECUO_START_REQUEST, handle_qiecuo_start_request);
	add_msg_handle(MSG_ID_QIECUO_REFUSE_REQUEST, handle_qiecuo_refuse_request);

	add_msg_handle(SERVER_PROTO_GET_OFFLINE_CACHE_ANSWER, handle_player_cache_answer);
	add_msg_handle(SERVER_PROTO_CHOSE_ZHENYING_REQUEST, handle_server_chose_zhenying);
	add_msg_handle(SERVER_PROTO_CHANGE_ZHENYING_REQUEST, handle_server_change_zhenying);

	add_msg_handle(MSG_ID_PVP_MATCH_START_REQUEST, handle_pvp_match_start_request);
	add_msg_handle(MSG_ID_PVP_MATCH_READY_REQUEST, handle_pvp_match_ready_request);
	add_msg_handle(MSG_ID_PVP_MATCH_CANCEL_REQUEST, handle_pvp_match_cancel_request);
	add_msg_handle(MSG_ID_PVP_RANK_REQUEST, handle_pvp_rank_request);
	add_msg_handle(MSG_ID_PVP_OPEN_LEVEL_REWARD_REQUEST, handle_pvp_open_level_reward_request);
	add_msg_handle(MSG_ID_PVP_OPEN_DAILY_BOX_REQUEST, handle_pvp_open_daily_box_request);
	add_msg_handle(MSG_ID_PVP_RAID_PRAISE_REQUEST, handle_pvp_raid_praise_request);

	add_msg_handle(MSG_ID_BAGUAPAI_INFO_REQUEST, handle_baguapai_info_request);
	add_msg_handle(MSG_ID_BAGUAPAI_SWITCH_REQUEST, handle_baguapai_switch_request);
	add_msg_handle(MSG_ID_BAGUAPAI_WEAR_REQUEST, handle_baguapai_wear_request);
	add_msg_handle(MSG_ID_BAGUAPAI_DECOMPOSE_REQUEST, handle_baguapai_decompose_request);
	add_msg_handle(MSG_ID_BAGUAPAI_REFINE_STAR_REQUEST, handle_baguapai_refine_star_request);
//	add_msg_handle(MSG_ID_BAGUAPAI_REFINE_MAIN_ATTR_REQUEST, handle_baguapai_refine_main_attr_request);
//	add_msg_handle(MSG_ID_BAGUAPAI_RETAIN_MAIN_ATTR_REQUEST, handle_baguapai_retain_main_attr_request);
	add_msg_handle(MSG_ID_BAGUAPAI_REFINE_MINOR_ATTR_REQUEST, handle_baguapai_refine_minor_attr_request);
	add_msg_handle(MSG_ID_BAGUAPAI_RETAIN_MINOR_ATTR_REQUEST, handle_baguapai_retain_minor_attr_request);
	add_msg_handle(MSG_ID_BAGUAPAI_MOVE_ADDITIONAL_ATTR_REQUEST, handle_baguapai_move_additional_attr_request);

	add_msg_handle(MSG_ID_ACTIVE_REWARD_REQUEST, handle_active_reward_request);

	add_msg_handle(MSG_ID_SETTING_TURN_SWITCH_REQUEST, handle_setting_turn_switch_request);
	add_msg_handle(MSG_ID_TRANSFER_OUT_STUCK_REQUEST, handle_transfer_out_stuck_request);

	add_msg_handle(SERVER_PROTO_GUILDSRV_COST_REQUEST, handle_guildsrv_check_and_cost_request);
	add_msg_handle(SERVER_PROTO_GUILDSRV_REWARD_REQUEST, handle_guildsrv_reward_request);
	add_msg_handle(SERVER_PROTO_SYNC_GUILD_SKILL, handle_sync_guild_skill_request);
	add_msg_handle(SERVER_PROTO_SYNC_GUILD_INFO, handle_sync_guild_info_request);
	add_msg_handle(SERVER_PROTO_GUILD_SYNC_ALL, handle_guild_sync_all);
	add_msg_handle(SERVER_PROTO_GUILD_CREATE, handle_sync_guild_create);
	add_msg_handle(SERVER_PROTO_GUILD_DISBAND, handle_sync_guild_disband);
	add_msg_handle(SERVER_PROTO_GUILD_RENAME, handle_sync_guild_rename);
	add_msg_handle(SERVER_PROTO_GUILD_ANSWER_AWARD, handle_guild_answer_award);
	add_msg_handle(SERVER_PROTO_GUILD_PRODUCE_MEDICINE, handle_guild_prodece_medicine);
	add_msg_handle(MSG_ID_GUILD_BATTLE_CALL_REQUEST, handle_guild_battle_call_request);
	add_msg_handle(MSG_ID_GUILD_BATTLE_INFO_REQUEST, handle_guild_battle_info_request);
	add_msg_handle(SERVER_PROTO_GUILD_BATTLE_FINAL_LIST_ANSWER, handle_guild_battle_final_list_answer);
	add_msg_handle(SERVER_PROTO_GUILD_SYNC_DONATION, handle_guild_sync_donation);
	add_msg_handle(SERVER_PROTO_GUILD_SKILL_LEVEL_UP, handle_guild_skill_level_up);
	add_msg_handle(SERVER_PROTO_GUILD_ACCEPT_TASK_REQUEST, handle_guild_accept_task_request);
	add_msg_handle(SERVER_PROTO_GUILD_SYNC_TASK, handle_guild_sync_task);
	add_msg_handle(SERVER_PROTO_GUILD_RUQIN_ADD_COUNT, handle_guild_ruqin_add_count);
	add_msg_handle(SERVER_PROTO_GUILD_RUQIN_SYNC_COUNT, handle_guild_ruqin_sync_count);
	add_msg_handle(SERVER_PROTO_GUILD_SYNC_DONATE, handle_guild_sync_donate);
	add_msg_handle(SERVER_PROTO_GUILD_SYNC_PARTICIPATE_ANSWER, handle_guild_sync_participate_answer_activity);
	add_msg_handle(SERVER_PROTO_GUILD_BONFIRE_OPEN_REQUEST, handle_guild_bonfire_open_request);

	add_msg_handle(MSG_ID_PERSONALITY_INFO_REQUEST, handle_personality_info_request);
	add_msg_handle(MSG_ID_PERSONALITY_SET_GENERAL_REQUEST, handle_personality_set_general_request);
	add_msg_handle(MSG_ID_PERSONALITY_SET_TAGS_REQUEST, handle_personality_set_tags_request);
	add_msg_handle(MSG_ID_PERSONALITY_SET_INTRO_REQUEST, handle_personality_set_intro_request);
	add_msg_handle(MSG_ID_GET_OTHER_INFO_REQUEST, handle_get_other_info_request);

	add_msg_handle(MSG_ID_FRIEND_RECOMMEND_REQUEST, handle_friend_recommend_request);
	add_msg_handle(MSG_ID_FRIEND_SEARCH_REQUEST, handle_friend_search_request);
	add_msg_handle(SERVER_PROTO_FRIEND_EXTEND_CONTACT_ANSWER, handle_friend_extend_contact_answer);
	add_msg_handle(SERVER_PROTO_FRIENDSRV_COST_REQUEST, handle_friendsrv_check_and_cost_request);
	add_msg_handle(SERVER_PROTO_FRIEND_GIFT_COST_REQUEST, handle_friend_gift_cost_request);
	add_msg_handle(SERVER_PROTO_FRIEND_ADD_GIFT, handle_friend_add_gift_request);
	add_msg_handle(SERVER_PROTO_FRIEND_SYNC_FRIEND_NUM, handle_friend_sync_friend_num);
	add_msg_handle(SERVER_PROTO_FRIEND_SEND_GIFT_SUCCESS, handle_friend_send_gift_success);
	add_msg_handle(SERVER_PROTO_FRIEND_TRACK_ENEMY_REQUEST, handle_friend_track_enemy_request);
	add_msg_handle(SERVER_PROTO_FRIEND_SYNC_ENEMY, handle_friend_sync_enemy);

	add_msg_handle(MSG_ID_AUTO_ADD_HP_SET_REQUEST, handle_auto_add_hp_set_request);

	add_msg_handle(MSG_ID_PARTNER_INFO_REQUEST, handle_partner_info_request);
	add_msg_handle(MSG_ID_PARTNER_TURN_SWITCH_REQUEST, handle_partner_turn_switch_request);
	add_msg_handle(MSG_ID_PARTNER_FORMATION_REQUEST, handle_partner_formation_request);
	add_msg_handle(MSG_ID_PARTNER_LEARN_SKILL_REQUEST, handle_partner_learn_skill_request);
	add_msg_handle(MSG_ID_PARTNER_USE_EXP_ITEM_REQUEST, handle_partner_use_exp_item_request);
	add_msg_handle(MSG_ID_PARTNER_DISMISS_REQUEST, handle_partner_dismiss_request);
	add_msg_handle(MSG_ID_PARTNER_EXCHANGE_REQUEST, handle_partner_exchange_request);
	add_msg_handle(MSG_ID_PARTNER_RECRUIT_REQUEST, handle_partner_recruit_request);
	add_msg_handle(MSG_ID_PARTNER_RESET_ATTR_REQUEST, handle_partner_reset_attr_request);
	add_msg_handle(MSG_ID_PARTNER_ADD_ATTR_REQUEST, handle_partner_add_attr_request);
	add_msg_handle(MSG_ID_PARTNER_ADD_GOD_REQUEST, handle_partner_add_god_request);
	add_msg_handle(MSG_ID_PARTNER_SAVE_ATTR_REQUEST, handle_partner_save_attr_request);
	add_msg_handle(MSG_ID_PARTNER_DEAD_FINISH_REQUEST, handle_partner_dead_finish_request);
	add_msg_handle(MSG_ID_PARTNER_BOND_ACTIVE_REQUEST, handle_partner_bond_active_request);
	add_msg_handle(MSG_ID_PARTNER_BOND_REWARD_REQUEST, handle_partner_bond_reward_request);
	add_msg_handle(MSG_ID_PARTNER_COMPOSE_STONE_REQUEST, handle_partner_compose_stone_request);
	add_msg_handle(MSG_ID_PARTNER_FABAO_STONE_REQUEST, handle_partner_fabao_stone_request);
	add_msg_handle(MSG_ID_PARTNER_FABAO_CHANGE_REQUEST, handle_partner_fabao_change_request);
	add_msg_handle(MSG_ID_PARTNER_RENAME_REQUEST, handle_partner_rename_request);	
	add_msg_handle(MSG_ID_PARTNER_LOCK_SKILL_REQUEST, handle_partner_lock_skill_request);

	add_msg_handle(MSG_ID_JIJIANGOP_GIFT_INFO_REQUEST, handle_gift_receive_request);

	add_msg_handle(SERVER_PROTO_UNDO_COST, handle_undo_cost);


	add_msg_handle(MSG_ID_RAID_AI_CONTINUE_REQUEST, handle_continue_raid_ai_request);
	add_msg_handle(MSG_ID_SKIP_NEW_RAID_REQUEST, handle_skip_new_raid_request);
	
	add_msg_handle(MSG_ID_ACHIEVEMENT_REWARD_REQUEST, handle_achievement_reward_request);

	add_msg_handle(SERVER_PROTO_DOUFACHANG_CHALLENGE_REQUEST, handle_doufachang_challenge);
	add_msg_handle(SERVER_PROTO_DOUFACHANG_ADD_REWARD_REQUEST, handle_doufachang_reward);
	add_msg_handle(SERVER_PROTO_DOUFACHANG_BUY_CHALLENGE_REQUEST, handle_doufachang_buy_challenge);			
	add_msg_handle(SERVER_PROTO_DOUFACHANG_SYNC_RANK, handle_doufachang_sync_rank);
	add_msg_handle(SERVER_PROTO_DOUFACHANG_SYNC_BUY_CHALLENGE, handle_doufachang_sync_buy_challenge);

	add_msg_handle(SERVER_PROTO_RANK_SYNC_RANK, handle_rank_sync_rank);		
	add_msg_handle(SERVER_PROTO_RANK_SYNC_CHANGE, handle_rank_sync_change);		

	add_msg_handle(MSG_ID_TITLE_WEAR_REQUEST, handle_title_wear_request);
	add_msg_handle(MSG_ID_TITLE_MARK_OLD_REQUEST, handle_title_mark_old_request);
	add_msg_handle(MSG_ID_HERO_CHALLENGE_ZHUJIEMIAN_INFO_REQUEST, handle_hero_challenge_main_info_request);
	add_msg_handle(MSG_ID_HERO_CHALLENGE_SWEEP_REQUEST, handle_hero_challenge_sweep_request);
	add_msg_handle(MSG_ID_HERO_CHALLENGE_SWEEP_RECIVE_REQUEST, handle_hero_challenge_sweep_recive_request);
	add_msg_handle(MSG_ID_HERO_CHALLENGE_SWEEP_REWARD_INFO_REQUEST, handle_hero_challenge_sweep_reward_info_request);
	add_msg_handle(MSG_ID_MIJING_XIULIAN_TASK_INFO_REQUEST, handle_mijing_xiulian_task_info_request);
	add_msg_handle(MSG_ID_MIJING_XIULIAN_TASK_SHUAXING_REQUEST, handle_mijing_xiulian_shuaxing_request);

	add_msg_handle(MSG_ID_FISHING_REWARD_REQUEST, handle_fishing_reward_request);
	add_msg_handle(MSG_ID_FISHING_SET_BAIT_REQUEST, handle_fishing_set_bait_request);
	add_msg_handle(MSG_ID_FISHING_BEGIN_REQUEST, handle_fishing_begin_request);
	add_msg_handle(MSG_ID_FISHING_END_REQUEST, handle_fishing_end_request);

	add_msg_handle(MSG_ID_STRONG_GOAL_REWARD_REQUEST, handle_strong_goal_reward_request);
	add_msg_handle(MSG_ID_STRONG_CHAPTER_REWARD_REQUEST, handle_strong_chapter_reward_request);

	add_msg_handle(MSG_ID_TRADE_ON_SHELF_REQUEST, handle_trade_on_shelf_request);
	add_msg_handle(SERVER_PROTO_TRADE_ON_SHELF_DELETE_ITEM_REQUEST, handle_trade_on_shelf_delete_item_request);
	add_msg_handle(SERVER_PROTO_TRADE_OFF_SHELF_ADD_ITEM_REQUEST, handle_trade_off_shelf_add_item_request);
	add_msg_handle(SERVER_PROTO_TRADE_RE_SHELF_CHANGE_REQUEST, handle_trade_re_shelf_change_request);
	add_msg_handle(SERVER_PROTO_TRADESRV_COST_REQUEST, handle_tradesrv_check_and_cost_request);
	add_msg_handle(SERVER_PROTO_TRADE_BUY_EXECUTE_REQUEST, handle_trade_buy_execute_request);
	add_msg_handle(SERVER_PROTO_TRADE_GET_EARNING_GIVE_REQUEST, handle_trade_get_earning_give_request);
	add_msg_handle(SERVER_PROTO_TRADE_BID_FAIL_RETURN, handle_trade_bid_fail_return);
	
	add_msg_handle(SERVER_PROTO_GUILD_RUQIN_CREAT_MONSTER_LEVEL_ANSWER, handle_guild_ruqin_creat_monster_level_answer);

	add_msg_handle(MSG_ID_PLAY_DRAMA_BEGIN_REQUEST, handle_play_drama_begin_request);
	add_msg_handle(MSG_ID_PLAY_DRAMA_END_REQUEST, handle_play_drama_end_request);

	add_msg_handle(SERVER_PROTO_RAID_PLAYER_ENTER_REQUEST, handle_raid_player_enter_request);
	add_msg_handle(SERVER_PROTO_RAID_TEAM_ENTER_REQUEST, handle_raid_team_enter_request);	
	add_msg_handle(SERVER_PROTO_RAID_LEAVE_REQUEST, handle_raid_leave_request);
	add_msg_handle(SERVER_PROTO_ATTR_CHANGED_REQUEST, handle_attr_changed_request);		

	add_msg_handle(MSG_ID_TRAVEL_TASK_ACCEPT_REQUEST, handle_travel_task_accept_request);
	add_msg_handle(MSG_ID_LEVEL_REWARD_INFO_REQUEST, handle_level_reward_info_request);
	add_msg_handle(MSG_ID_RECEIVE_LEVEL_REWARD_REQUEST, handle_recieve_level_reward_request);
	add_msg_handle(MSG_ID_ONLINE_REWARD_INFO_REQUEST, handle_online_reward_info_request);
	add_msg_handle(MSG_ID_ONLINE_REWARD_ZHUAN_REQUEST, handle_recieve_online_zhuanpan_request);
	add_msg_handle(MSG_ID_ONLINE_REWARD_RECEIVE_REQUEST, handle_recieve_online_reward_request);

	add_msg_handle(MSG_ID_SIGNIN_EVERYDAY_INFO_REQUEST, handle_sign_in_ervery_day_info_request);
	add_msg_handle(MSG_ID_SIGNIN_EVERYDAY_REWARD_RECEIVE_REQUEST, handle_player_sign_in_ervery_day_request);
	add_msg_handle(MSG_ID_SIGNIN_LEIJI_REWARD_RECEIVE_REQUEST, handle_sign_in_receive_leiji_reward_request);
	add_msg_handle(MSG_ID_PLAYER_ACTIVE_REWARD_ZHAOHUI_INFO_REQUEST, handle_active_receive_zhaohui_info_request);
	add_msg_handle(MSG_ID_PLAYER_ACTIVE_REWARD_ZHAOHUI_REQUEST, handle_active_receive_zhaohui_request);

	add_msg_handle(MSG_ID_YAOQIAN_SHU_INFO_REQUEST, handle_yao_qian_shu_info_request);
	add_msg_handle(MSG_ID_YAOQIAN_SHU_START_REQUEST, handle_yao_qian_shu_start_request);
	add_msg_handle(MSG_ID_LOGIN_RECEIVE_REWARD_REQUEST, handle_receive_login_reward_request);
	add_msg_handle(MSG_ID_CIFU_RECEIVE_REWARD_REQUEST, handle_receive_ci_fu_reward_request);

	add_msg_handle(SERVER_PROTO_ACTIVITYSRV_REWARD_REQUEST, handle_activitysrv_reward_request);

	add_msg_handle(MSG_ID_MONEY_EXCHANGE_REQUEST, handle_player_money_exchange_request);

	add_msg_handle(MSG_ID_GUILD_CHUAN_GONG_INFO_REQUEST, handle_player_guild_chuan_gong_info_request);
	add_msg_handle(MSG_ID_GUILD_CHUAN_GONG_START_REQUEST, handle_player_guild_chuan_gong_start_request);
	add_msg_handle(MSG_ID_GUILD_CHUAN_GONG_IS_OR_NO_REQUEST, handle_player_guild_chuan_gong_queren_request);
	add_msg_handle(MSG_ID_GUILD_CHUAN_GONG_FINISH_REQUEST, handle_player_guild_chuan_gong_finish_request);

	add_msg_handle(MSG_ID_GUILD_RUQIN_SHISHI_INFO_REQUEST, handle_guild_ruqin_shishi_info_request);
	add_msg_handle(MSG_ID_FRIEND_TRACK_ENEMY_POX_REQUEST, handle_friend_get_enemy_pox_info_request);
	add_msg_handle(MSG_ID_JIU_GONG_BA_GUA_RECIVE_REWARD_REQUEST, handle_jiu_gong_ba_gua_recive_reward_request);

	add_msg_handle(MSG_ID_RED_BACKET_SEND_TO_MANY_PLAYER_REQUEST, handle_red_packet_send_red_request);
	add_msg_handle(SERVER_PROTO_TRADE_GRAB_RED_PACKET_REQUEST, handle_red_packet_grab_red_add_money_request);
	add_msg_handle(SERVER_PROTO_TRADE_SEND_RED_PACKET_FAILED_ANSWER, handle_red_packet_send_failed_still_money_request);

	add_msg_handle(MSG_ID_MARRY_PLAYER_PROPOSE_REQUEST, handle_marry_player_propose_request);
	add_msg_handle(MSG_ID_MARRY_PLAYER_PROPOSE_START_REQUEST, handle_marry_player_propose_start_request);
	add_msg_handle(MSG_ID_MARRY_PLAYER_PROPOSE_START_REQUEST, handle_marry_player_propose_queren_request);
	add_msg_handle(MSG_ID_MARRY_PLAYER_RESERVE_WEDDING_REQUEST, handle_marry_player_reserve_wedding_request);
	add_msg_handle(MSG_ID_MARRY_PLAYER_CANCEL_PROPOSE_REQUEST, handle_marry_player_cancel_propose_request);
	add_msg_handle(MSG_ID_MARRY_PLAYER_CANCEL_CONFIRM_REQUEST, handle_marry_player_cancel_confirm_request);
}

void uninstall_msg_handle()
{
	m_game_handle_map.clear();
}
