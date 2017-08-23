#include "conn_node_guildsrv.h"
#include "game_event.h"
#include "msgid.h"
#include "error_code.h"
#include "mysql_module.h"
#include "time_helper.h"
#include <vector>
#include <set>
#include "guild_util.h"
#include "redis_util.h"
#include "guild.pb-c.h"
#include "app_data_statis.h"
#include <algorithm>
#include "shop.pb-c.h"
#include "chat.pb-c.h"
#include "personality.pb-c.h"
#include "guild_answer.h"
#include "guild_battle.pb-c.h"
#include "send_mail.h"


conn_node_guildsrv conn_node_guildsrv::connecter;
//static char sql[1024];


static int handle_guild_create_cost(int data_len, uint8_t *data, int result, EXTERN_DATA *extern_data);
static int handle_guild_rename_cost(int data_len, uint8_t *data, int result, EXTERN_DATA *extern_data);
static int handle_guild_skill_practice_cost(int data_len, uint8_t *data, int result, EXTERN_DATA *extern_data);
static int handle_shop_buy_answer(int data_len, uint8_t *data, int result, EXTERN_DATA *extern_data);

conn_node_guildsrv::conn_node_guildsrv()
{
	max_buf_len = 1024 * 1024;
	buf = (uint8_t *)malloc(max_buf_len + sizeof(EXTERN_DATA));
	assert(buf);
	
	add_msg_handle(MSG_ID_GUILD_LIST_REQUEST, &conn_node_guildsrv::handle_guild_list_request);
	add_msg_handle(MSG_ID_GUILD_INFO_REQUEST, &conn_node_guildsrv::handle_guild_info_request);
	add_msg_handle(MSG_ID_GUILD_MEMBER_LIST_REQUEST, &conn_node_guildsrv::handle_guild_member_list_request);
	add_msg_handle(MSG_ID_GUILD_CREATE_REQUEST, &conn_node_guildsrv::handle_guild_create_request);
	add_msg_handle(MSG_ID_GUILD_JOIN_REQUEST, &conn_node_guildsrv::handle_guild_join_request);
	add_msg_handle(MSG_ID_GUILD_JOIN_LIST_REQUEST, &conn_node_guildsrv::handle_guild_join_list_request);
	add_msg_handle(MSG_ID_GUILD_DEAL_JOIN_REQUEST, &conn_node_guildsrv::handle_guild_deal_join_request);
	add_msg_handle(MSG_ID_GUILD_TURN_SWITCH_REQUEST, &conn_node_guildsrv::handle_guild_turn_switch_request);
	add_msg_handle(MSG_ID_GUILD_SET_WORDS_REQUEST, &conn_node_guildsrv::handle_guild_set_words_request);
	add_msg_handle(MSG_ID_GUILD_APPOINT_OFFICE_REQUEST, &conn_node_guildsrv::handle_guild_appoint_office_request);
	add_msg_handle(MSG_ID_GUILD_KICK_REQUEST, &conn_node_guildsrv::handle_guild_kick_request);
	add_msg_handle(MSG_ID_GUILD_RENAME_REQUEST, &conn_node_guildsrv::handle_guild_rename_request);
	add_msg_handle(MSG_ID_GUILD_EXIT_REQUEST, &conn_node_guildsrv::handle_guild_exit_request);

	add_msg_handle(SERVER_PROTO_PLAYER_ONLINE_NOTIFY, &conn_node_guildsrv::handle_player_online_notify);
	add_msg_handle(SERVER_PROTO_GUILDSRV_COST_ANSWER, &conn_node_guildsrv::handle_check_and_cost_answer);
	add_msg_handle(SERVER_PROTO_GUILDSRV_REWARD_ANSWER, &conn_node_guildsrv::handle_gamesrv_reward_answer);
	add_msg_handle(SERVER_PROTO_ADD_GUILD_RESOURCE, &conn_node_guildsrv::handle_add_guild_resrouce_request);
	add_msg_handle(SERVER_PROTO_GM_DISBAND_GUILD, &conn_node_guildsrv::handle_disband_request);

	add_msg_handle(MSG_ID_GUILD_BUILDING_INFO_REQUEST, &conn_node_guildsrv::handle_guild_building_info_request);
	add_msg_handle(MSG_ID_GUILD_BUILDING_UPGRADE_REQUEST, &conn_node_guildsrv::handle_guild_building_upgrade_request);
	add_msg_handle(SERVER_PROTO_SUB_GUILD_BUILDING_TIME, &conn_node_guildsrv::handle_sub_guild_building_time_request);

	add_msg_handle(MSG_ID_GUILD_SKILL_INFO_REQUEST, &conn_node_guildsrv::handle_guild_skill_info_request);
	add_msg_handle(MSG_ID_GUILD_SKILL_DEVELOP_REQUEST, &conn_node_guildsrv::handle_guild_skill_develop_request);
	add_msg_handle(MSG_ID_GUILD_SKILL_PRACTICE_REQUEST, &conn_node_guildsrv::handle_guild_skill_pratice_request);

	add_msg_handle(MSG_ID_GUILD_SHOP_INFO_REQUEST, &conn_node_guildsrv::handle_guild_shop_info_request);
	add_msg_handle(MSG_ID_GUILD_SHOP_BUY_REQUEST, &conn_node_guildsrv::handle_guild_shop_buy_request);

	add_msg_handle(SERVER_PROTO_GUILD_CHAT, &conn_node_guildsrv::handle_guild_chat_request);
	add_msg_handle(SERVER_PROTO_GUILD_PRODUCE_MEDICINE, &conn_node_guildsrv::handle_guild_prodece_medicine_request);
	add_msg_handle(MSG_ID_GET_OTHER_INFO_ANSWER, &conn_node_guildsrv::handle_get_other_info_request);

	add_msg_handle(MSG_ID_GUILD_BATTLE_CALL_REQUEST, &conn_node_guildsrv::handle_guild_battle_call_request);
	add_msg_handle(SERVER_PROTO_GUILD_BATTLE_ENTER_WAIT, &conn_node_guildsrv::handle_guild_battle_enter_wait_request);
	add_msg_handle(MSG_ID_GUILD_BATTLE_INFO_REQUEST, &conn_node_guildsrv::handle_guild_battle_info_request);
	add_msg_handle(SERVER_PROTO_GUILD_BATTLE_REWARD, &conn_node_guildsrv::handle_guild_battle_fight_reward_request);
	add_msg_handle(SERVER_PROTO_GUILD_BATTLE_BEGIN, &conn_node_guildsrv::handle_guild_battle_sync_begin);
	add_msg_handle(SERVER_PROTO_GUILD_BATTLE_END, &conn_node_guildsrv::handle_guild_battle_sync_end);
	add_msg_handle(SERVER_PROTO_GUILD_BATTLE_SETTLE, &conn_node_guildsrv::handle_guild_battle_sync_settle);
	add_msg_handle(SERVER_PROTO_GUILD_BATTLE_FINAL_LIST_REQUEST, &conn_node_guildsrv::handle_guild_battle_final_list_request);
	add_msg_handle(SERVER_PROTO_GUILD_ADD_FINAL_BATTLE_GUILD, &conn_node_guildsrv::handle_guild_battle_add_final_id);

	add_msg_handle(MSG_ID_OPEN_FACTION_QUESTION_REQUEST, &conn_node_guildsrv::handle_open_guild_answer_request);
}

conn_node_guildsrv::~conn_node_guildsrv()
{
}

void conn_node_guildsrv::add_msg_handle(uint32_t msg_id, handle_func func)
{
	connecter.m_handleMap[msg_id] = func;
}

int conn_node_guildsrv::recv_func(evutil_socket_t fd)
{
	EXTERN_DATA *extern_data;
	PROTO_HEAD *head;	
	for (;;) {
		int ret = get_one_buf();
		if (ret == 0) {
			head = (PROTO_HEAD *)buf_head();
			int cmd = get_cmd();
			uint64_t times = time_helper::get_micro_time();
			time_helper::set_cached_time(times / 1000);
			switch (cmd)
			{
				case SERVER_PROTO_GAMESRV_START:
					{
						LOG_INFO("[%s:%d] game_srv start notify.", __FUNCTION__, __LINE__);
					}
					break;
				case SERVER_PROTO_GUILD_BATTLE_REWARD:
				case SERVER_PROTO_GUILD_BATTLE_BEGIN:
				case SERVER_PROTO_GUILD_BATTLE_END:
				case SERVER_PROTO_GUILD_BATTLE_SETTLE:
				case SERVER_PROTO_GUILD_BATTLE_FINAL_LIST_REQUEST:
					{
						HandleMap::iterator iter = m_handleMap.find(cmd);
						if (iter != m_handleMap.end())
						{
							(this->*(iter->second))(NULL);
						}
					}
					break;
				default:
					{
						extern_data = get_extern_data(head);
						LOG_DEBUG("[%s:%d] cmd: %u, player_id: %lu", __FUNCTION__, __LINE__, cmd, extern_data->player_id);

						HandleMap::iterator iter = m_handleMap.find(cmd);
						if (iter != m_handleMap.end())
						{
							(this->*(iter->second))(extern_data);
						}
						else
						{
							LOG_ERR("[%s:%d] cmd %u has no handler", __FUNCTION__, __LINE__, cmd);
						}
					}
					break;
			}
		}
		
		if (ret < 0) {
			LOG_INFO("%s: connect closed from fd %u, err = %d", __FUNCTION__, fd, errno);
			exit(0);
			return (-1);		
		} else if (ret > 0) {
			break;
		}
		
		ret = remove_one_buf();
	}
	return (0);
}

int conn_node_guildsrv::broadcast_message(uint16_t msg_id, void *msg_data, pack_func packer, std::vector<uint64_t> &players)
{
	PROTO_HEAD_CONN_BROADCAST *head;
	PROTO_HEAD *real_head;

	head = (PROTO_HEAD_CONN_BROADCAST *)conn_node_base::global_send_buf;
	head->msg_id = ENDION_FUNC_2(SERVER_PROTO_BROADCAST);
	real_head = &head->proto_head;

	real_head->msg_id = ENDION_FUNC_2(msg_id);
	real_head->seq = 0;
//	memcpy(real_head->data, msg_data, len);
	size_t len = 0;
	if (msg_data && packer)
	{
		len = packer(msg_data, (uint8_t *)real_head->data);
	}
	real_head->len = ENDION_FUNC_4(sizeof(PROTO_HEAD) + len);

	uint64_t *ppp = (uint64_t*)((char *)&head->player_id + len);
	head->num_player_id = 0;
	for (std::vector<uint64_t>::iterator iter = players.begin(); iter != players.end(); ++iter)
	{
		ppp[head->num_player_id++] = *iter;
	}
	head->len = ENDION_FUNC_4(sizeof(PROTO_HEAD_CONN_BROADCAST) + len + sizeof(uint64_t) * head->num_player_id);
	if (conn_node_guildsrv::connecter.send_one_msg((PROTO_HEAD *)head, 1) != (int)(ENDION_FUNC_4(head->len)))
	{
		LOG_ERR("[%s:%d] send to conn_srv failed err[%d]", __FUNCTION__, __LINE__, errno);
	}
	return 0;
}

#define MAX_GUILD_NUM 5000
int conn_node_guildsrv::handle_guild_list_request(EXTERN_DATA *extern_data)
{
	GuildListAnswer resp;
	guild_list_answer__init(&resp);

	GuildBriefData guild_data[MAX_GUILD_NUM];
	GuildBriefData* guild_data_point[MAX_GUILD_NUM];

	resp.result = 0;
	resp.guilds = guild_data_point;
	resp.n_guilds = 0;
	std::map<uint32_t, GuildInfo*> &guild_map = get_all_guild();
	std::vector<uint32_t> applied_guild_ids;
	get_player_join_apply(extern_data->player_id, applied_guild_ids);
	for (std::map<uint32_t, GuildInfo*>::iterator iter = guild_map.begin(); iter != guild_map.end(); ++iter)
	{
		GuildInfo *guild = iter->second;
		guild_data_point[resp.n_guilds] = &guild_data[resp.n_guilds];
		guild_brief_data__init(&guild_data[resp.n_guilds]);
		guild_data[resp.n_guilds].guildid = guild->guild_id;
		guild_data[resp.n_guilds].name = guild->name;
		guild_data[resp.n_guilds].icon = guild->icon;
		guild_data[resp.n_guilds].level = get_guild_level(guild);
		guild_data[resp.n_guilds].membernum = guild->member_num;
		guild_data[resp.n_guilds].popularity = guild->popularity;
		guild_data[resp.n_guilds].approvestate = guild->approve_state;
		guild_data[resp.n_guilds].recruitstate = guild->recruit_state;
		guild_data[resp.n_guilds].recruitnotice = guild->recruit_notice;
		guild_data[resp.n_guilds].joinapplied = (std::find(applied_guild_ids.begin(), applied_guild_ids.end(), guild->guild_id) != applied_guild_ids.end());

		for (uint32_t i = 0; i < guild->member_num; ++i)
		{
			GuildPlayer *player = guild->members[i];
			if (!player)
			{
				continue;
			}

			AutoReleaseRedisPlayer t1;			
			PlayerRedisInfo *redis_player = get_redis_player(player->player_id, sg_player_key, sg_redis_client, t1);
			if (!redis_player)
			{
				continue;
			}

			guild_data[resp.n_guilds].totalfc += redis_player->fighting_capacity;
			if (player->player_id == guild->master_id)
			{
				guild_data[resp.n_guilds].masterid = player->player_id;
				guild_data[resp.n_guilds].mastername = redis_player->name;
				guild_data[resp.n_guilds].masterjob = redis_player->job;
				guild_data[resp.n_guilds].masterhead = redis_player->head_icon;
				guild_data[resp.n_guilds].mastercamp = redis_player->zhenying;
			}
		}

		resp.n_guilds++;
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_LIST_ANSWER, guild_list_answer__pack, resp);

	return 0;
}

void resp_guild_info(conn_node_guildsrv *node, EXTERN_DATA *extern_data, uint32_t msg_id, uint32_t result, GuildPlayer *player)
{
	GuildInfoAnswer resp;
	guild_info_answer__init(&resp);

	GuildData basic_data;
	guild_data__init(&basic_data);
	GuildPersonalData personal_data;
	guild_personal_data__init(&personal_data);

	GuildBuildingData building_data[MAX_GUILD_BUILDING_NUM];
	GuildBuildingData* building_data_point[MAX_GUILD_BUILDING_NUM];

	resp.result = result;
	if (player)
	{
		if (player->guild)
		{
			GuildInfo *guild = player->guild;
			resp.basicinfo = &basic_data;
			basic_data.guildid = guild->guild_id;
			basic_data.name = guild->name;
			basic_data.icon = guild->icon;
			basic_data.level = get_guild_level(guild);
			basic_data.membernum = guild->member_num;
			basic_data.popularity = guild->popularity;
			basic_data.approvestate = guild->approve_state;
			basic_data.recruitstate = guild->recruit_state;
			basic_data.recruitnotice = guild->recruit_notice;
			basic_data.announcement = guild->announcement;
			basic_data.treasure = guild->treasure;
			basic_data.buildboard = guild->build_board;
			basic_data.masterid = guild->master_id;
			basic_data.renametime = guild->rename_time;
			AutoReleaseRedisPlayer t1;			
			PlayerRedisInfo *redis_master = get_redis_player(guild->master_id, sg_player_key, sg_redis_client, t1);
			if (redis_master)
			{
				basic_data.mastername = redis_master->name;
			}

			resp.buildings = building_data_point;
			resp.n_buildings = 0;
			for (int i = 0; i < MAX_GUILD_BUILDING_NUM; ++i)
			{
				building_data_point[resp.n_buildings] = &building_data[resp.n_buildings];
				guild_building_data__init(&building_data[resp.n_buildings]);
				building_data[resp.n_buildings].buildingid = i + 1;
				building_data[resp.n_buildings].level = guild->buildings[i].level;
				resp.n_buildings++;
			}
		}

		resp.personalinfo = &personal_data;
		personal_data.office = player->office;
		personal_data.donation = player->donation;
		personal_data.allhistorydonation = player->all_history_donation;
		personal_data.curhistorydonation = player->cur_history_donation;
		personal_data.jointime = player->join_time;
		personal_data.exittime = player->exit_time;
	}

	fast_send_msg(node, extern_data, msg_id, guild_info_answer__pack, resp);
}

int conn_node_guildsrv::handle_guild_info_request(EXTERN_DATA *extern_data)
{
	GuildPlayer *player = get_guild_player(extern_data->player_id);
	resp_guild_info(&connecter, extern_data, MSG_ID_GUILD_INFO_ANSWER, 0, player);

	return 0;
}

int conn_node_guildsrv::handle_guild_member_list_request(EXTERN_DATA *extern_data)
{
	int ret = 0;
	GuildPlayer *player = NULL;
	GuildInfo *guild = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		guild = player->guild;
	} while(0);

	GuildMemberListAnswer resp;
	guild_member_list_answer__init(&resp);

	GuildMemberData member_data[MAX_GUILD_MEMBER_NUM];
	GuildMemberData* member_data_point[MAX_GUILD_MEMBER_NUM];

	resp.result = ret;
	if (ret == 0)
	{
		resp.members = member_data_point;
		resp.n_members = 0;
		for (uint32_t i = 0; i < guild->member_num; ++i)
		{
			GuildPlayer *member = guild->members[i];
			member_data_point[resp.n_members] = &member_data[resp.n_members];
			guild_member_data__init(&member_data[resp.n_members]);
			member_data[resp.n_members].playerid = member->player_id;
			member_data[resp.n_members].office = member->office;
			member_data[resp.n_members].donation = member->donation;
			member_data[resp.n_members].curhistorydonation = member->cur_history_donation;
			member_data[resp.n_members].curweekdonation = member->cur_week_donation;
			member_data[resp.n_members].jointime = member->join_time;

			AutoReleaseRedisPlayer t1;
			PlayerRedisInfo *redis_player = get_redis_player(member->player_id, sg_player_key, sg_redis_client, t1);
			if (redis_player)
			{
				member_data[resp.n_members].name = redis_player->name;
				member_data[resp.n_members].job = redis_player->job;
				member_data[resp.n_members].level = redis_player->lv;
				member_data[resp.n_members].head = redis_player->head_icon;
				member_data[resp.n_members].fc = redis_player->fighting_capacity;
				member_data[resp.n_members].offlinetime = redis_player->status;
			}

			resp.n_members++;
		}
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_MEMBER_LIST_ANSWER, guild_member_list_answer__pack, resp);

	return 0;
}

int conn_node_guildsrv::handle_guild_create_request(EXTERN_DATA *extern_data)
{
	GuildCreateRequest *req = guild_create_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

//	uint32_t icon = req->icon;
	std::string name(req->name);
	guild_create_request__free_unpacked(req, NULL);

	int ret = 0;
	GuildPlayer *player = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (player)
		{
			if (player->guild)
			{
				ret = ERROR_ID_GUILD_PLAYER_ALREADY_JOIN;
				LOG_ERR("[%s:%d] player[%lu] already in guild", __FUNCTION__, __LINE__, extern_data->player_id);
				break;
			}

			if (!check_guild_join_cd(player))
			{
				ret = ERROR_ID_GUILD_JOIN_CD;
				LOG_ERR("[%s:%d] player[%lu] join cd, exit_time:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->exit_time);
				break;
			}
		}

		if (name.size() == 0)
		{
			ret = ERROR_ID_GUILD_NAME_LEN_TOO_SHORT;
			break;
		}
		if (name.size() >= MAX_GUILD_NAME_LEN)
		{
			ret = ERROR_ID_GUILD_NAME_LEN_TOO_LONG;
			break;
		}

		//检查是否同名
		if (check_guild_name(name))
		{
			ret = ERROR_ID_GUILD_NAME_USED;
			LOG_ERR("[%s:%d] player[%lu] guild name exist, name:%s", __FUNCTION__, __LINE__, extern_data->player_id, name.c_str());
			break;
		}

		AutoReleaseRedisPlayer t1;
		PlayerRedisInfo *redis_player = get_redis_player(extern_data->player_id, sg_player_key, sg_redis_client, t1);
		if (!redis_player)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] get redis player failed", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		uint32_t need_level = sg_guild_create_level;
		uint32_t player_level = redis_player->lv;
		if (player_level < need_level)
		{
			ret = ERROR_ID_GUILD_CREATE_OR_JOIN_LEVEL;
			LOG_ERR("[%s:%d] player[%lu] level not enough, need_level:%u, player_level:%u", __FUNCTION__, __LINE__, extern_data->player_id, need_level, player_level);
			break;
		}

		if (sg_guild_create_coin == 0)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] config error, coin:%u", __FUNCTION__, __LINE__, extern_data->player_id, sg_guild_create_coin);
			break;
		}
	} while(0);

	if (ret != 0)
	{
		resp_guild_info(&connecter, extern_data, MSG_ID_GUILD_CREATE_ANSWER, ret, player);
	}
	else
	{
		//请求扣除消耗
		PROTO_GUILDSRV_CHECK_AND_COST_REQ *cost_req = (PROTO_GUILDSRV_CHECK_AND_COST_REQ *)get_send_buf(SERVER_PROTO_GUILDSRV_COST_REQUEST, get_seq());
		cost_req->head.len = ENDION_FUNC_4(sizeof(PROTO_GUILDSRV_CHECK_AND_COST_REQ) + get_data_len());
		memset(cost_req->head.data, 0, sizeof(PROTO_GUILDSRV_CHECK_AND_COST_REQ) - sizeof(PROTO_HEAD));
		cost_req->cost.statis_id = MAGIC_TYPE_GUILD_CREATE;
		cost_req->cost.coin = sg_guild_create_coin;
		cost_req->data_size = get_data_len();
		memcpy(cost_req->data, get_data(), cost_req->data_size);
		add_extern_data(&cost_req->head, extern_data);
		if (connecter.send_one_msg(&cost_req->head, 1) != (int)ENDION_FUNC_4(cost_req->head.len))
		{
			LOG_ERR("[%s:%d] send to gamesrv failed err[%d]", __FUNCTION__, __LINE__, errno);
		}
	}

	return 0;
}

static int handle_guild_create_cost(int data_len, uint8_t *data, int result, EXTERN_DATA *extern_data)
{
	GuildCreateRequest *req = guild_create_request__unpack(NULL, data_len, data);
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}
	
	uint32_t icon = req->icon;
	std::string name(req->name);
	guild_create_request__free_unpacked(req, NULL);

	int ret = result;
	GuildPlayer *player = NULL;
	bool internal = false;
	do
	{
		if (ret != 0)
		{
			if (ret == ERROR_ID_COIN_IS_NOT_ENOUGH)
			{
				ret = ERROR_ID_GUILD_CREATE_COIN;
			}
			break;
		}
		internal = true;

		ret = create_guild(extern_data->player_id, icon, name, player);
	} while(0);

	resp_guild_info(&conn_node_guildsrv::connecter, extern_data, MSG_ID_GUILD_CREATE_ANSWER, ret, player);

	return (internal ? ret : 0);
}

struct GuildCmpInfo
{
	GuildInfo *guild;
	uint32_t level;
	uint32_t fc;
};

bool guild_cmp_func(const GuildCmpInfo &l, const GuildCmpInfo &r)
{
	if (l.guild->approve_state < r.guild->approve_state)
	{
		return true;
	}
	else if (l.guild->approve_state > r.guild->approve_state)
	{
		return false;
	}
	else
	{
		if (l.level > r.level)
		{
			return true;
		}
		else if (l.level < r.level)
		{
			return false;
		}
		else
		{
			if (l.fc > r.fc)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

int conn_node_guildsrv::handle_guild_join_request(EXTERN_DATA *extern_data)
{
	GuildJoinRequest *req = guild_join_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint32_t guild_id = req->guildid;
	guild_join_request__free_unpacked(req, NULL);

	int ret = 0;
	std::vector<uint32_t> applyIds;
	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (player)
		{
			if (player->guild)
			{
				ret = ERROR_ID_GUILD_PLAYER_ALREADY_JOIN;
				LOG_ERR("[%s:%d] player[%lu] in guild, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild_id);
				break;
			}

			if (!check_guild_join_cd(player))
			{
				ret = ERROR_ID_GUILD_JOIN_CD;
				LOG_ERR("[%s:%d] player[%lu] join cd, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild_id);
				break;
			}
		}

		AutoReleaseRedisPlayer t1;		
		PlayerRedisInfo *redis_player = get_redis_player(extern_data->player_id, sg_player_key, sg_redis_client, t1);
		if (!redis_player)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] get redis info failed, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild_id);
			break;
		}

		uint32_t need_level = sg_guild_create_level;
		uint32_t player_level = redis_player->lv;
		if (player_level < need_level)
		{
			ret = ERROR_ID_GUILD_CREATE_OR_JOIN_LEVEL;
			LOG_ERR("[%s:%d] player[%lu] level not enough, need_level:%u, player_level:%u", __FUNCTION__, __LINE__, extern_data->player_id, need_level, player_level);
			break;
		}

		if (guild_id > 0)
		{
			GuildInfo *guild = get_guild(guild_id);
			if (!guild)
			{
				ret = ERROR_ID_GUILD_ID;
				LOG_ERR("[%s:%d] player[%lu] guild not exist, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild_id);
				break;
			}

			if (guild->recruit_state == 0)
			{
				ret = ERROR_ID_GUILD_RECRUIT_CLOSE;
				LOG_ERR("[%s:%d] player[%lu] guild close recruit, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild_id);
				break;
			}

			uint32_t max_member = get_guild_max_member(guild);
			if (guild->member_num >= max_member || guild->member_num >= MAX_GUILD_MEMBER_NUM)
			{
				ret = ERROR_ID_GUILD_MEMBER_MAX;
				LOG_ERR("[%s:%d] player[%lu] guild member max, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild_id);
				break;
			}

			AutoReleaseRedisPlayer t1;			
			PlayerRedisInfo *redis_master = get_redis_player(guild->master_id, sg_player_key, sg_redis_client, t1);
			if (!redis_master)
			{
				ret = ERROR_ID_SERVER;
				LOG_ERR("[%s:%d] player[%lu] get master redis info failed, guild_id:%u, master_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, guild_id, guild->master_id);
				break;
			}

			//检查阵营是否符合
			if (redis_player->zhenying != redis_master->zhenying)
			{
				ret = ERROR_ID_GUILD_CAMP;
				LOG_ERR("[%s:%d] player[%lu] camp not match, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild_id);
				break;
			}

			//无需审核，直接加入帮会
			if (guild->approve_state == 0)
			{
				ret = join_guild(extern_data->player_id, guild);
				break;
			}

			//检查是否已申请
			if (check_player_applied_join(extern_data->player_id, guild_id))
			{
				ret = ERROR_ID_GUILD_PLAYER_APPLIED_JOIN;
				LOG_ERR("[%s:%d] player[%lu] applied join, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild_id);
				break;
			}

			ret = insert_player_join_apply(extern_data->player_id, guild_id);
			if (ret == 0)
			{
				applyIds.push_back(guild_id);
			}
		}
		else
		{
			//全部申请处理
			std::vector<GuildCmpInfo> sort_vec;
			//筛选符合条件的帮会
			std::map<uint32_t, GuildInfo*> &guild_map = get_all_guild();
			for (std::map<uint32_t, GuildInfo*>::iterator iter = guild_map.begin(); iter != guild_map.end(); ++iter)
			{
				GuildInfo *guild = iter->second;
				if (guild->recruit_state == 0)
				{
					continue;
				}

				uint32_t max_member = get_guild_max_member(guild);
				if (guild->member_num >= max_member || guild->member_num >= MAX_GUILD_MEMBER_NUM)
				{
					continue;
				}
				AutoReleaseRedisPlayer t1;
				PlayerRedisInfo *redis_master = get_redis_player(guild->master_id, sg_player_key, sg_redis_client, t1);
				if (!redis_master)
				{
					continue;
				}

				//检查阵营是否符合
				if (redis_player->zhenying != redis_master->zhenying)
				{
					continue;
				}

				//检查是否已申请
				if (guild->approve_state != 0 && check_player_applied_join(extern_data->player_id, guild_id))
				{
					continue;
				}

				GuildCmpInfo cmp;
				cmp.guild = guild;
				cmp.level = get_guild_level(guild);
				cmp.fc = 0;
				for (uint32_t i = 0; i < guild->member_num; ++i)
				{
					AutoReleaseRedisPlayer t1;					
					PlayerRedisInfo *tmp_redis_player = get_redis_player(guild->members[i]->player_id, sg_player_key, sg_redis_client, t1);
					if (tmp_redis_player)
					{
						cmp.fc += tmp_redis_player->fighting_capacity;
					}
				}
				sort_vec.push_back(cmp);
			}

			//对符合条件的帮会进行排序
			std::sort(sort_vec.begin(), sort_vec.end(), guild_cmp_func);
			for (std::vector<GuildCmpInfo>::iterator iter = sort_vec.begin(); iter != sort_vec.end(); ++iter)
			{
				GuildCmpInfo *cmp = &(*iter);
				if (cmp->guild->approve_state == 0)
				{
					ret = join_guild(extern_data->player_id, cmp->guild);
					break;
				}
				else
				{
					ret = insert_player_join_apply(extern_data->player_id, cmp->guild->guild_id);
					if (ret == 0)
					{
						applyIds.push_back(cmp->guild->guild_id);
					}
				}
			}
		}
	} while(0);

	GuildJoinAnswer resp;
	guild_join_answer__init(&resp);

	GuildData basic_data;
	guild_data__init(&basic_data);
	GuildPersonalData personal_data;
	guild_personal_data__init(&personal_data);

	GuildBuildingData building_data[MAX_GUILD_BUILDING_NUM];
	GuildBuildingData* building_data_point[MAX_GUILD_BUILDING_NUM];

	resp.result = ret;
	GuildPlayer *player = get_guild_player(extern_data->player_id);
	if (player)
	{
		if (player->guild)
		{
			GuildInfo *guild = player->guild;
			resp.basicinfo = &basic_data;
			basic_data.guildid = guild->guild_id;
			basic_data.name = guild->name;
			basic_data.icon = guild->icon;
			basic_data.level = get_guild_level(guild);
			basic_data.membernum = guild->member_num;
			basic_data.popularity = guild->popularity;
			basic_data.approvestate = guild->approve_state;
			basic_data.recruitstate = guild->recruit_state;
			basic_data.recruitnotice = guild->recruit_notice;
			basic_data.announcement = guild->announcement;
			basic_data.treasure = guild->treasure;
			basic_data.buildboard = guild->build_board;
			basic_data.masterid = guild->master_id;
			basic_data.renametime = guild->rename_time;
			AutoReleaseRedisPlayer t1;			
			PlayerRedisInfo *redis_master = get_redis_player(guild->master_id, sg_player_key, sg_redis_client, t1);
			if (redis_master)
			{
				basic_data.mastername = redis_master->name;
			}

			resp.buildings = building_data_point;
			resp.n_buildings = 0;
			for (int i = 0; i < MAX_GUILD_BUILDING_NUM; ++i)
			{
				building_data_point[resp.n_buildings] = &building_data[resp.n_buildings];
				guild_building_data__init(&building_data[resp.n_buildings]);
				building_data[resp.n_buildings].buildingid = i + 1;
				building_data[resp.n_buildings].level = guild->buildings[i].level;
				resp.n_buildings++;
			}
		}

		resp.personalinfo = &personal_data;
		personal_data.office = player->office;
		personal_data.donation = player->donation;
		personal_data.allhistorydonation = player->all_history_donation;
		personal_data.curhistorydonation = player->cur_history_donation;
		personal_data.jointime = player->join_time;
		personal_data.exittime = player->exit_time;
	}

	if (ret == 0 && applyIds.size() > 0)
	{
		resp.n_applyids = applyIds.size();
		resp.applyids = &applyIds[0];
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_JOIN_ANSWER, guild_join_answer__pack, resp);

	return 0;
}

#define MAX_JOIN_LIST_SIZE 100
int conn_node_guildsrv::handle_guild_join_list_request(EXTERN_DATA *extern_data)
{
	int ret = 0;
	std::vector<uint64_t> applyIds;
	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (!office_has_permission(player->office, GOPT_DEAL_JOIN))
		{
			ret = ERROR_ID_GUILD_PLAYER_NO_PERMISSION;
			LOG_ERR("[%s:%d] player[%lu] no permission, office:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->office);
			break;
		}

		ret = get_guild_join_apply(player->guild->guild_id, applyIds);
	} while(0);

	GuildJoinListAnswer resp;
	guild_join_list_answer__init(&resp);

	GuildJoinPlayerData join_data[MAX_JOIN_LIST_SIZE];
	GuildJoinPlayerData* join_data_point[MAX_JOIN_LIST_SIZE];

	resp.result = ret;

	if (ret == 0 && applyIds.size() > 0)
	{
		resp.n_joins = 0;
		resp.joins = join_data_point;
		for (size_t i = 0; i < applyIds.size(); ++i)
		{
			uint64_t player_id = applyIds[i];
			AutoReleaseRedisPlayer t1;			
			PlayerRedisInfo *redis_player = get_redis_player(player_id, sg_player_key, sg_redis_client, t1);
			if (!redis_player)
			{
				continue;
			}

			join_data_point[resp.n_joins] = &join_data[resp.n_joins];
			guild_join_player_data__init(&join_data[resp.n_joins]);
			join_data[resp.n_joins].playerid = player_id;
			join_data[resp.n_joins].name = redis_player->name;
			join_data[resp.n_joins].job = redis_player->job;
			join_data[resp.n_joins].level = redis_player->lv;
			join_data[resp.n_joins].fc = redis_player->fighting_capacity;
			resp.n_joins++;
		}
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_JOIN_LIST_ANSWER, guild_join_list_answer__pack, resp);

	return 0;
}

int conn_node_guildsrv::handle_guild_deal_join_request(EXTERN_DATA *extern_data)
{
	GuildDealJoinRequest *req = guild_deal_join_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint64_t deal_id = req->playerid;
	GuildJoinDealType deal_type = req->deal;
	guild_deal_join_request__free_unpacked(req, NULL);

	int ret = 0;
	std::vector<uint64_t> dealIds;
	uint32_t member_num = 0;
	uint64_t del_id = 0;
	do
	{
		if (deal_type != GUILD_JOIN_DEAL_TYPE__APPROVE_JOIN && deal_type != GUILD_JOIN_DEAL_TYPE__REJECT_JOIN)
		{
			ret = (-1);
			break;
		}

		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (!office_has_permission(player->office, GOPT_DEAL_JOIN))
		{
			ret = ERROR_ID_GUILD_PLAYER_NO_PERMISSION;
			LOG_ERR("[%s:%d] player[%lu] no permission, office:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->office);
			break;
		}

		GuildInfo *guild = player->guild;
		uint32_t max_member = get_guild_max_member(guild);
		if (deal_id > 0)
		{
			if (deal_type == GUILD_JOIN_DEAL_TYPE__APPROVE_JOIN)
			{
				if (!check_player_applied_join(deal_id, guild->guild_id))
				{
					ret = ERROR_ID_GUILD_JOIN_APPLY_GONE;
					LOG_ERR("[%s:%d] player[%lu] no this player, deal_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, deal_id);
					break;
				}

				GuildPlayer *deal_player = get_guild_player(deal_id);
				if (deal_player && deal_player->guild)
				{
					ret = ERROR_ID_GUILD_JOIN_OTHER_GUILD;
					LOG_ERR("[%s:%d] player[%lu] already in guild, deal_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, deal_id);
					delete_player_join_apply(deal_id);
					break;
				}

				if (guild->member_num >= max_member)
				{
					ret = ERROR_ID_GUILD_MEMBER_MAX;
					LOG_ERR("[%s:%d] player[%lu] guild member max, deal_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, deal_id);
					break;
				}

				ret = join_guild(deal_id, guild);
			}
			else if (deal_type == GUILD_JOIN_DEAL_TYPE__REJECT_JOIN)
			{
				delete_guild_player_join_apply(guild->guild_id, deal_id);
			}
		}
		else
		{
			if (deal_type == GUILD_JOIN_DEAL_TYPE__APPROVE_JOIN)
			{
				std::vector<uint64_t> applyIds;
				get_guild_join_apply(guild->guild_id, applyIds);
				for (std::vector<uint64_t>::iterator iter = applyIds.begin(); iter != applyIds.end(); ++iter)
				{
					uint64_t tmp_id = *iter;
					GuildPlayer *deal_player = get_guild_player(tmp_id);
					if (deal_player && deal_player->guild)
					{
						del_id = tmp_id;
						continue;
					}

					if (guild->member_num >= max_member)
					{
						break;
					}

					join_guild(tmp_id, guild);
					del_id = tmp_id;
				}
			}
			else if (deal_type == GUILD_JOIN_DEAL_TYPE__REJECT_JOIN)
			{
				delete_guild_join_apply(guild->guild_id);
			}
		}

		member_num = guild->member_num;
	} while(0);

	GuildDealJoinAnswer resp;
	guild_deal_join_answer__init(&resp);

	resp.result = ret;
	resp.playerid = deal_id;
	resp.deal = deal_type;
	resp.membernum = member_num;
	resp.delid = del_id;

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_DEAL_JOIN_ANSWER, guild_deal_join_answer__pack, resp);

	return 0;
}

int conn_node_guildsrv::handle_guild_turn_switch_request(EXTERN_DATA *extern_data)
{
	GuildTurnSwitchRequest *req = guild_turn_switch_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	GuildSwitchType type = req->type;
	guild_turn_switch_request__free_unpacked(req, NULL);

	int ret = 0;
	uint32_t *pSwitch = NULL;
	do
	{
		if (type != GUILD_SWITCH_TYPE__APPROVE_SWITCH && type != GUILD_SWITCH_TYPE__RECRUIT_SWITCH)
		{
			ret = (-1);
			break;
		}

		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (!office_has_permission(player->office, GOPT_RECRUIT_SETTING))
		{
			ret = ERROR_ID_GUILD_PLAYER_NO_PERMISSION;
			LOG_ERR("[%s:%d] player[%lu] no permission, office:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->office);
			break;
		}

		GuildInfo *guild = player->guild;
		pSwitch = NULL;
		uint32_t attr_id = 0;
		if (type == GUILD_SWITCH_TYPE__APPROVE_SWITCH)
		{
			pSwitch = &guild->approve_state;
			attr_id = GUILD_ATTR_TYPE__ATTR_APPROVE_STATE;
		}
		else if (type == GUILD_SWITCH_TYPE__RECRUIT_SWITCH)
		{
			pSwitch = &guild->recruit_state;
			attr_id = GUILD_ATTR_TYPE__ATTR_RECRUIT_STATE;
		}

		if (*pSwitch == 0)
		{
			*pSwitch = 1;
		}
		else
		{
			*pSwitch = 0;
		}

		save_guild_switch(guild, type);
		broadcast_guild_attr_update(guild, attr_id, *pSwitch, player->player_id);
	} while(0);

	GuildTurnSwitchAnswer resp;
	guild_turn_switch_answer__init(&resp);

	resp.result = ret;
	resp.type = type;
	if (pSwitch)
	{
		resp.state = *pSwitch;
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_TURN_SWITCH_ANSWER, guild_turn_switch_answer__pack, resp);
	return 0;
}

int conn_node_guildsrv::handle_guild_set_words_request(EXTERN_DATA *extern_data)
{
	GuildSetWordsRequest *req = guild_set_words_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	GuildWordsType type = req->type;
	std::string words(req->words);
	guild_set_words_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		if (type != GUILD_WORDS_TYPE__RECRUIT_NOTICE && type != GUILD_WORDS_TYPE__ANNOUNCEMENT)
		{
			ret = (-1);
			break;
		}

		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (words.size() >= MAX_GUILD_ANNOUNCEMENT_LEN)
		{
			ret = -1;
			break;
		}

		uint32_t permission_type = 0;
		if (type == GUILD_WORDS_TYPE__RECRUIT_NOTICE)
		{
			permission_type = GOPT_RECRUIT_SETTING;
		}
		else if (type == GUILD_WORDS_TYPE__ANNOUNCEMENT)
		{
			permission_type = GOPT_ANNOUNCEMENT_SETTING;
		}
		if (!office_has_permission(player->office, permission_type))
		{
			ret = ERROR_ID_GUILD_PLAYER_NO_PERMISSION;
			LOG_ERR("[%s:%d] player[%lu] no permission, office:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->office);
			break;
		}

		GuildInfo *guild = player->guild;
		char *pWord = NULL;
		uint32_t attr_id = 0;
		if (type == GUILD_WORDS_TYPE__RECRUIT_NOTICE)
		{
			pWord = guild->recruit_notice;
			attr_id = GUILD_STR_ATTR_TYPE__ATTR_RECRUIT_NOTICE;
		}
		else if (type == GUILD_WORDS_TYPE__ANNOUNCEMENT)
		{
			pWord = guild->announcement;
			attr_id = GUILD_STR_ATTR_TYPE__ATTR_ANNOUNCEMENT;
		}

		strcpy(pWord, words.c_str());
		save_announcement(guild, type);
		broadcast_guild_str_attr_update(guild, attr_id, pWord, player->player_id);
	} while(0);

	GuildSetWordsAnswer resp;
	guild_set_words_answer__init(&resp);

	resp.result = ret;
	resp.type = type;

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_SET_WORDS_ANSWER, guild_set_words_answer__pack, resp);
	return 0;
}

int conn_node_guildsrv::handle_guild_appoint_office_request(EXTERN_DATA *extern_data)
{
	GuildAppointOfficeRequest *req = guild_appoint_office_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint64_t appoint_id = req->playerid;
	GuildOfficeType office = req->office;
	guild_appoint_office_request__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (appoint_id == extern_data->player_id)
		{
			ret = ERROR_ID_GUILD_CAN_NOT_DO_IT_TO_SELF;
			LOG_ERR("[%s:%d] player[%lu] can't appoint self, appoint_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, appoint_id);
			break;
		}

		GuildPlayer *appointee = get_guild_player(appoint_id);
		if (!appointee || !appointee->guild)
		{
			ret = ERROR_ID_GUILD_PLYAER_EXIT;
			LOG_ERR("[%s:%d] player[%lu] appoint not in guild, appoint_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, appoint_id);
			break;
		}

		if (appointee->guild != player->guild)
		{
			ret = ERROR_ID_GUILD_JOIN_OTHER_GUILD;
			LOG_ERR("[%s:%d] player[%lu] appoint not in same guild, appoint_id:%lu", __FUNCTION__, __LINE__, extern_data->player_id, appoint_id);
			break;
		}

		ret = appoint_office(player, appointee, office);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_APPOINT_OFFICE_ANSWER, comm_answer__pack, resp);
	return 0;
}

int conn_node_guildsrv::handle_guild_kick_request(EXTERN_DATA *extern_data)
{
	GuildKickReuqest *req = guild_kick_reuqest__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint64_t kick_id = req->playerid;
	guild_kick_reuqest__free_unpacked(req, NULL);

	int ret = 0;
	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		ret = kick_member(player, kick_id);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);

	resp.result = ret;

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_KICK_ANSWER, comm_answer__pack, resp);
	return 0;
}

int conn_node_guildsrv::handle_guild_rename_request(EXTERN_DATA *extern_data)
{
	GuildRenameRequest *req = guild_rename_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	std::string name(req->name);
	guild_rename_request__free_unpacked(req, NULL);

	int ret = 0;
	GuildInfo *guild = NULL;
	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		guild = player->guild;
		if (!office_has_permission(player->office, GOPT_RENAME))
		{
			ret = ERROR_ID_GUILD_PLAYER_NO_PERMISSION;
			LOG_ERR("[%s:%d] player[%lu] no permission, office:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->office);
			break;
		}

		uint32_t now = time_helper::get_cached_time() / 1000;
		if (now < player->guild->rename_time + sg_guild_rename_cd)
		{
			ret = ERROR_ID_GUILD_RENAME_CD;
			LOG_ERR("[%s:%d] player[%lu] rename cd, guild_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->guild->guild_id);
			break;
		}

		if (name.size() == 0)
		{
			ret = ERROR_ID_GUILD_NAME_LEN_TOO_SHORT;
			break;
		}
		if (name.size() >= MAX_GUILD_NAME_LEN)
		{
			ret = ERROR_ID_GUILD_NAME_LEN_TOO_LONG;
			break;
		}

		//检查是否同名
		if (check_guild_name(name))
		{
			ret = ERROR_ID_GUILD_NAME_USED;
			LOG_ERR("[%s:%d] player[%lu] guild name exist, name:%s", __FUNCTION__, __LINE__, extern_data->player_id, name.c_str());
			break;
		}

		if (sg_guild_rename_item_id == 0 || sg_guild_rename_item_num == 0)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] config error, item_id:%u, item_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, sg_guild_rename_item_id, sg_guild_rename_item_num);
			break;
		}
	} while(0);

	if (ret == 0)
	{
		//请求扣除消耗
		PROTO_GUILDSRV_CHECK_AND_COST_REQ *cost_req = (PROTO_GUILDSRV_CHECK_AND_COST_REQ *)get_send_buf(SERVER_PROTO_GUILDSRV_COST_REQUEST, get_seq());
		cost_req->head.len = ENDION_FUNC_4(sizeof(PROTO_GUILDSRV_CHECK_AND_COST_REQ) + get_data_len());
		memset(cost_req->head.data, 0, sizeof(PROTO_GUILDSRV_CHECK_AND_COST_REQ) - sizeof(PROTO_HEAD));
		cost_req->cost.statis_id = MAGIC_TYPE_GUILD_RENAME;
		cost_req->cost.item_id[0] = sg_guild_rename_item_id;
		cost_req->cost.item_num[0] = sg_guild_rename_item_num;
		cost_req->data_size = get_data_len();
		memcpy(cost_req->data, get_data(), cost_req->data_size);
		add_extern_data(&cost_req->head, extern_data);
		if (connecter.send_one_msg(&cost_req->head, 1) != (int)ENDION_FUNC_4(cost_req->head.len))
		{
			LOG_ERR("[%s:%d] send to gamesrv failed err[%d]", __FUNCTION__, __LINE__, errno);
		}
	}
	else
	{
		GuildRenameAnswer resp;
		guild_rename_answer__init(&resp);

		resp.result = ret;
		resp.name = const_cast<char*>(name.c_str());
		if (guild)
		{
			resp.renametime = guild->rename_time;
		}

		fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_RENAME_ANSWER, guild_rename_answer__pack, resp);
	}

	return 0;
}

static int handle_guild_rename_cost(int data_len, uint8_t *data, int result, EXTERN_DATA *extern_data)
{
	GuildRenameRequest *req = guild_rename_request__unpack(NULL, data_len, data);
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	std::string name(req->name);
	guild_rename_request__free_unpacked(req, NULL);

	int ret = result;
	GuildInfo *guild = NULL;
	bool internal = false;
	do
	{
		if (ret != 0)
		{
			if (ret == ERROR_ID_PROP_NOT_ENOUGH)
			{
				ret = ERROR_ID_GUILD_RENAME_ITEM;
			}
			break;
		}
		internal = true;

		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (name.size() >= MAX_GUILD_NAME_LEN)
		{
			ret = -1;
			break;
		}

		guild = player->guild;
		strcpy(player->guild->name, name.c_str());
		save_guild_name(player->guild);
		player->guild->rename_time = time_helper::get_cached_time() / 1000;
		save_guild_info(player->guild);

		std::vector<uint64_t> broadcast_ids;
		for (uint32_t i = 0; i < guild->member_num; ++i)
		{
			GuildPlayer *member = guild->members[i];
			//更新所有玩家的redis数据
			update_redis_player_guild(member);
			//同步所有在线玩家数据到game_srv
			if (member->player_id == player->player_id)
			{
				sync_guild_info_to_gamesrv(member);
			}
			else
			{
				AutoReleaseRedisPlayer t1;				
				PlayerRedisInfo *redis_member = get_redis_player(member->player_id, sg_player_key, sg_redis_client, t1);
				if (redis_member)
				{
					if (redis_member->status == 0)
					{
						sync_guild_info_to_gamesrv(member);
						broadcast_ids.push_back(member->player_id);
					}
				}
			}
		}

		//向所有在线玩家广播帮会名字
		if (broadcast_ids.size() > 0)
		{
			broadcast_guild_str_attr_update(GUILD_STR_ATTR_TYPE__ATTR_GUILD_NAME, guild->name, broadcast_ids);
		}
	} while(0);

	GuildRenameAnswer resp;
	guild_rename_answer__init(&resp);

	resp.result = ret;
	resp.name = const_cast<char*>(name.c_str());
	if (guild)
	{
		resp.renametime = guild->rename_time;
	}

	fast_send_msg(&conn_node_guildsrv::connecter, extern_data, MSG_ID_GUILD_RENAME_ANSWER, guild_rename_answer__pack, resp);

	return (internal ? ret : 0);
}

int conn_node_guildsrv::handle_guild_exit_request(EXTERN_DATA *extern_data)
{
	int ret = 0;
	GuildPlayer *player = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (player->office == (uint32_t)GUILD_OFFICE_TYPE__OFFICE_MASTER)
		{
			ret = ERROR_ID_GUILD_MASTER_CANNOT_EXIT;
			LOG_ERR("[%s:%d] player[%lu] is master office[%u]", __FUNCTION__, __LINE__, extern_data->player_id, player->office);
			break;
		}

		ret = exit_guild(player);
	} while(0);

	GuildExitAnswer resp;
	guild_exit_answer__init(&resp);

	resp.result = ret;
	if (player)
	{
		resp.exittime = player->exit_time;
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_EXIT_ANSWER, guild_exit_answer__pack, resp);

	return 0;
}

int conn_node_guildsrv::handle_open_guild_answer_request(EXTERN_DATA *extern_data)
{
	int ret = 0;
	GuildPlayer *player = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not in guild", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		if (player->office != (uint32_t)GUILD_OFFICE_TYPE__OFFICE_MASTER)
		{
			ret = ERROR_ID_GUILD_MASTER_CANNOT_EXIT;
			LOG_ERR("[%s:%d] player[%lu] is master office[%u]", __FUNCTION__, __LINE__, extern_data->player_id, player->office);
			break;
		}
	} while (0);

	CommAnswer resp;
	comm_answer__init(&resp);
	resp.result = ret;
	fast_send_msg(&connecter, extern_data, MSG_ID_OPEN_FACTION_QUESTION_ANSWER, comm_answer__pack, resp);

	if (ret == 0)
	{
		uint32_t question[GuildAnswer::MAX_SEND_GUILD_QUESTION];
		for (int i = 0; i < GuildAnswer::MAX_SEND_GUILD_QUESTION; ++i)
		{
			question[i] = sg_guild_question[rand() % sg_guild_question.size()];
		}
		//broadcast_guild_message(player->guild, MSG_ID_FACTION_QUESTION_OPEN_NOTIFY, &resp, (pack_func)comm_answer__pack);
		player->guild->answer.Start(player->guild, question, GuildAnswer::MAX_SEND_GUILD_QUESTION);
	}

	return 0;
}


int conn_node_guildsrv::handle_player_online_notify(EXTERN_DATA *extern_data)
{
	GuildPlayer *player = get_guild_player(extern_data->player_id);
	if (player)
	{
		sync_guild_info_to_gamesrv(player);
		sync_guild_skill_to_gamesrv(player);
		sync_player_donation_to_game_srv(player);
		//聊天发送
		do
		{
			if (!player->guild)
			{
				break;
			}

			GuildInfo *guild = player->guild;
			if (player->player_id != guild->master_id)
			{
				break;
			}

			ParameterTable *param_config = get_config_by_id(161000149, &parameter_config);
			if (!param_config)
			{
				break;
			}
			AutoReleaseRedisPlayer t1;
			PlayerRedisInfo *redis_player = get_redis_player(player->player_id, sg_player_key, sg_redis_client, t1);
			if (!redis_player)
			{
				break;
			}

			char content[1024];
			sprintf(content, param_config->parameter2, redis_player->name);

			Chat req;
			chat__init(&req);
			req.channel = CHANNEL__family;
			req.contain = content;

			broadcast_guild_chat(guild, &req);
		} while(0);

		if (player->guild)
		{
			player->guild->answer.OnPlayerLogin(player, extern_data);
		}
	}

	resp_guild_info(&connecter, extern_data, MSG_ID_GUILD_INFO_ANSWER, 0, player);

	return 0;
}

int conn_node_guildsrv::handle_check_and_cost_answer(EXTERN_DATA *extern_data)
{
	PROTO_GUILDSRV_CHECK_AND_COST_RES *res = (PROTO_GUILDSRV_CHECK_AND_COST_RES*)buf_head();
	int ret = 0;
	switch(res->cost.statis_id)
	{
		case MAGIC_TYPE_GUILD_CREATE:
			ret = handle_guild_create_cost(res->data_size, res->data, res->result, extern_data);
			break;
		case MAGIC_TYPE_GUILD_RENAME:
			ret = handle_guild_rename_cost(res->data_size, res->data, res->result, extern_data);
			break;
		case MAGIC_TYPE_GUILD_SKILL_PRACTICE:
			ret = handle_guild_skill_practice_cost(res->data_size, res->data, res->result, extern_data);
			break;
	}

	if (ret != 0)
	{
		PROTO_UNDO_COST *proto = (PROTO_UNDO_COST*)get_send_buf(SERVER_PROTO_UNDO_COST, 0);
		proto->head.len = ENDION_FUNC_4(sizeof(PROTO_UNDO_COST));
		memset(proto->head.data, 0, sizeof(PROTO_UNDO_COST) - sizeof(PROTO_HEAD));
		memcpy(&proto->cost, &res->cost, sizeof(SRV_COST_INFO));
		conn_node_base::add_extern_data(&proto->head, extern_data);
		if (connecter.send_one_msg(&proto->head, 1) != (int)ENDION_FUNC_4(proto->head.len))
		{
			LOG_ERR("[%s:%d] send to gamesrv failed err[%d]", __FUNCTION__, __LINE__, errno);
		}
	}

	return 0;
}

int conn_node_guildsrv::handle_gamesrv_reward_answer(EXTERN_DATA *extern_data)
{
	PROTO_GUILDSRV_REWARD_RES *res = (PROTO_GUILDSRV_REWARD_RES*)buf_head();
	switch(res->statis_id)
	{
		case MAGIC_TYPE_SHOP_BUY:
			handle_shop_buy_answer(res->data_size, res->data, res->result, extern_data);
			break;
	}

	return 0;
}

int conn_node_guildsrv::handle_add_guild_resrouce_request(EXTERN_DATA *extern_data)
{
	PROTO_HEAD *head = get_head();
	uint32_t *pData = (uint32_t *)head->data;
	uint32_t type = *(pData++);
	uint32_t num = *(pData++);

	GuildPlayer *player = get_guild_player(extern_data->player_id);
	if (!player || !player->guild)
	{
		return -1;
	}

	switch (type)
	{
		case 1:
			add_guild_popularity(player->guild, num);
			break;
		case 2:
			add_guild_treasure(player->guild, num);
			break;
		case 3:
			add_guild_build_board(player->guild, num);
			break;
		case 4:
			add_player_donation(player, num);
			break;
	}
	return 0;
}

int conn_node_guildsrv::handle_disband_request(EXTERN_DATA *extern_data)
{
	PROTO_HEAD *head = get_head();
	uint32_t *pData = (uint32_t *)head->data;
	uint32_t guild_id = *(pData++);
	GuildInfo *guild = get_guild(guild_id);
	if (guild)
	{
		disband_guild(guild);
	}
	return 0;
}


int conn_node_guildsrv::handle_guild_building_info_request(EXTERN_DATA *extern_data)
{
	int ret = 0;
	GuildPlayer *player = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}
	} while(0);

	GuildBuildingInfoAnswer resp;
	guild_building_info_answer__init(&resp);

	GuildBuildingData building_data[MAX_GUILD_BUILDING_NUM];
	GuildBuildingData* building_data_point[MAX_GUILD_BUILDING_NUM];

	resp.result = ret; 
	if (player && player->guild)
	{
		resp.buildings = building_data_point;
		resp.n_buildings = 0;
		for (int i = 0; i < MAX_GUILD_BUILDING_NUM; ++i)
		{
			building_data_point[resp.n_buildings] = &building_data[resp.n_buildings];
			guild_building_data__init(&building_data[resp.n_buildings]);
			building_data[resp.n_buildings].buildingid = i + 1;
			building_data[resp.n_buildings].level = player->guild->buildings[i].level;
			resp.n_buildings++;
		}

		resp.upgradeid = player->guild->building_upgrade_id;
		resp.upgradeend = player->guild->building_upgrade_end;
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_BUILDING_INFO_ANSWER, guild_building_info_answer__pack, resp);

	return 0;
}

int conn_node_guildsrv::handle_guild_building_upgrade_request(EXTERN_DATA *extern_data)
{
	GuildBuildingUpgradeRequest *req = guild_building_upgrade_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint32_t building_id = req->buildingid;
	guild_building_upgrade_request__free_unpacked(req, NULL);

	int ret = 0;
	GuildPlayer *player = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		GuildInfo *guild = player->guild;
		if (guild->building_upgrade_id > 0)
		{
			ret = ERROR_ID_GUILD_HAS_BUILDING_UPGRADE;
			LOG_ERR("[%s:%d] player[%lu] guild has building upgrade, upgrade_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->building_upgrade_id);
			break;
		}

		uint32_t building_level = get_building_level(guild, building_id);
		if (get_guild_building_config(building_id, building_level + 1) == NULL)
		{
			ret = ERROR_ID_GUILD_BUILDING_LEVEL_MAX;
			LOG_ERR("[%s:%d] player[%lu] guild building level max, building_id:%u, level:%u", __FUNCTION__, __LINE__, extern_data->player_id, building_id, building_level);
			break;
		}

		GangsTable *config = get_guild_building_config(building_id, building_level);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] guild building config failed, building_id:%u, level:%u", __FUNCTION__, __LINE__, extern_data->player_id, building_id, building_level);
			break;
		}

		if (player->office > (uint32_t)config->LeveCompetence)
		{
			ret = ERROR_ID_GUILD_PLAYER_NO_PERMISSION;
			LOG_ERR("[%s:%d] player[%lu] has not permission, building_id:%u, level:%u", __FUNCTION__, __LINE__, extern_data->player_id, building_id, building_level);
			break;
		}

		uint32_t hall_level = get_building_level(guild, Building_Hall);
		uint32_t need_hall_level = config->HallLeve;
		if (hall_level < need_hall_level)
		{
			ret = ERROR_ID_GUILD_HALL_LEVEL;
			LOG_ERR("[%s:%d] player[%lu] hall level not enough, building_id:%u, level:%u, hall_level:%u, need_hall_level:%u", __FUNCTION__, __LINE__, extern_data->player_id, building_id, building_level, hall_level, need_hall_level);
			break;
		}

		uint32_t need_treasure = config->Leve1Expend;
		if (guild->treasure < need_treasure)
		{
			ret = ERROR_ID_GUILD_BUILDING_UPGRADE_RESOURCE;
			LOG_ERR("[%s:%d] player[%lu] guild treasure not enough, building_id:%u, level:%u, has_treasure:%u, need_treasure:%u", __FUNCTION__, __LINE__, extern_data->player_id, building_id, building_level, guild->treasure, need_treasure);
			break;
		}

		uint32_t need_board = config->Leve2Expend;
		if (guild->build_board < need_board)
		{
			ret = ERROR_ID_GUILD_BUILDING_UPGRADE_RESOURCE;
			LOG_ERR("[%s:%d] player[%lu] guild board not enough, building_id:%u, level:%u, has_board:%u, need_board:%u", __FUNCTION__, __LINE__, extern_data->player_id, building_id, building_level, guild->build_board, need_board);
			break;
		}

		sub_guild_treasure(guild, need_treasure, false);
		sub_guild_build_board(guild, need_board, false);

		guild->building_upgrade_id = building_id;
		guild->building_upgrade_end = time_helper::get_cached_time() / 1000 + config->LeveTime;
		save_guild_info(guild);
	} while(0);

	GuildBuildingUpgradeAnswer resp;
	guild_building_upgrade_answer__init(&resp);

	resp.result = ret; 
	if (player && player->guild)
	{
		resp.upgradeid = player->guild->building_upgrade_id;
		resp.upgradeend = player->guild->building_upgrade_end;
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_BUILDING_UPGRADE_ANSWER, guild_building_upgrade_answer__pack, resp);
	return 0;
}

int conn_node_guildsrv::handle_sub_guild_building_time_request(EXTERN_DATA *extern_data)
{
	PROTO_HEAD *head = get_head();
	uint32_t *pData = (uint32_t *)head->data;
	uint32_t time = *(pData++);

	GuildPlayer *player = get_guild_player(extern_data->player_id);
	if (!player || !player->guild)
	{
		return -1;
	}

	sub_building_upgrade_time(player->guild, time);

	return 0;
}

int conn_node_guildsrv::handle_guild_skill_info_request(EXTERN_DATA *extern_data)
{
	int ret = 0;
	GuildPlayer *player = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}
	} while(0);

	GuildSkillInfoAnswer resp;
	guild_skill_info_answer__init(&resp);

	GuildSkillData develop_data[MAX_GUILD_SKILL_NUM];
	GuildSkillData* develop_data_point[MAX_GUILD_SKILL_NUM];
	GuildSkillData practice_data[MAX_GUILD_SKILL_NUM];
	GuildSkillData* practice_data_point[MAX_GUILD_SKILL_NUM];

	resp.result = ret; 
	if (player)
	{
		if (player->guild)
		{
			resp.develops = develop_data_point;
			resp.n_develops = 0;
			for (int i = 0; i < MAX_GUILD_SKILL_NUM; ++i)
			{
				if (player->guild->skills[i].skill_id == 0)
				{
					break;
				}

				develop_data_point[resp.n_develops] = &develop_data[resp.n_develops];
				guild_skill_data__init(&develop_data[resp.n_develops]);
				develop_data[resp.n_develops].skillid = player->guild->skills[i].skill_id;
				develop_data[resp.n_develops].level = player->guild->skills[i].skill_lv;
				resp.n_develops++;
			}
		}

		resp.practices = practice_data_point;
		resp.n_practices = 0;
		for (int i = 0; i < MAX_GUILD_SKILL_NUM; ++i)
		{
			if (player->skills[i].skill_id == 0)
			{
				break;
			}

			practice_data_point[resp.n_practices] = &practice_data[resp.n_practices];
			guild_skill_data__init(&practice_data[resp.n_practices]);
			practice_data[resp.n_practices].skillid = player->skills[i].skill_id;
			practice_data[resp.n_practices].level = player->skills[i].skill_lv;
			resp.n_practices++;
		}
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_SKILL_INFO_ANSWER, guild_skill_info_answer__pack, resp);

	return 0;
}

int conn_node_guildsrv::handle_guild_skill_develop_request(EXTERN_DATA *extern_data)
{
	GuildSkillUpgradeRequest *req = guild_skill_upgrade_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint32_t skill_id = req->skillid;
	guild_skill_upgrade_request__free_unpacked(req, NULL);

	int ret = 0;
	GuildPlayer *player = NULL;
	GuildSkill *pSkill = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		GuildInfo *guild = player->guild;
		if (!office_has_permission(player->office, GOPT_DEVELOP_SKILL))
		{
			ret = ERROR_ID_GUILD_PLAYER_NO_PERMISSION;
			LOG_ERR("[%s:%d] player[%lu] no permission, office:%u", __FUNCTION__, __LINE__, extern_data->player_id, player->office);
			break;
		}

		pSkill = get_guild_skill_info(guild, skill_id);
		if (!pSkill)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] guild skill memory exhaust, guild_id:%u, skill_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id);
			break;
		}

		uint32_t next_lv = pSkill->skill_lv;
		GangsSkillTable *config = get_guild_skill_config(skill_id, next_lv);
		if (!config)
		{
			ret = ERROR_ID_GUILD_SKILL_LEVEL_MAX;
			LOG_ERR("[%s:%d] player[%lu] get skill config failed, guild_id:%u, skill_id:%u, skill_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id, next_lv);
			break;
		}

		uint32_t need_lib_lv = config->BuildingLeve;
		uint32_t library_level = get_building_level(guild, Building_Library);
		if (library_level < need_lib_lv)
		{
			ret = ERROR_ID_GUILD_LIBRARY_LEVEL;
			LOG_ERR("[%s:%d] player[%lu] guild library level, guild_id:%u, skill_id:%u, skill_lv:%u, need_lv:%u, lib_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id, next_lv, need_lib_lv, library_level);
			break;
		}

		uint32_t need_treasure = config->CreateMnoney;
		if (guild->treasure < need_treasure)
		{
			ret = ERROR_ID_GUILD_SKILL_DEVELOP_TREASURE;
			LOG_ERR("[%s:%d] player[%lu] guild treasure not enough, guild_id:%u, skill_id:%u, skill_id:%u, has_treasure:%u, need_treasure:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id, next_lv, guild->treasure, need_treasure);
			break;
		}

		sub_guild_treasure(guild, need_treasure, true);

		pSkill->skill_id = skill_id;
		pSkill->skill_lv++;
		save_guild_info(guild);
		broadcast_skill_develop_update(guild, pSkill);
		broadcast_guild_object_attr_update(guild, GUILD_OBJECT_ATTR_TYPE__ATTR_SKILL_DEVELOP, skill_id, pSkill->skill_lv);
	} while(0);

	GuildSkillUpgradeAnswer resp;
	guild_skill_upgrade_answer__init(&resp);

	resp.result = ret; 
	resp.skillid = skill_id;
	if (pSkill)
	{
		resp.level = pSkill->skill_lv;
	}

	fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_SKILL_DEVELOP_ANSWER, guild_skill_upgrade_answer__pack, resp);
	return 0;
}

int conn_node_guildsrv::handle_guild_skill_pratice_request(EXTERN_DATA *extern_data)
{
	GuildSkillPracticeRequest *req = guild_skill_practice_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint32_t skill_id = req->skillid;
	uint32_t level_num = req->level;
	guild_skill_practice_request__free_unpacked(req, NULL);

	int ret = 0;
	GuildPlayer *player = NULL;
	GuildSkill *pSkill = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		GuildInfo *guild = player->guild;
		GuildSkill *pGuildSkill = get_guild_skill_info(guild, skill_id);
		if (!pGuildSkill)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] guild skill memory exhaust, guild_id:%u, skill_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id);
			break;
		}

		pSkill = get_player_skill_info(player, skill_id);
		if (!pSkill)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] skill memory exhaust, guild_id:%u, skill_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id);
			break;
		}

		uint32_t need_coin = 0;
		uint32_t need_donation = 0;
		uint32_t next_lv = pSkill->skill_lv;
		for (uint32_t i = 0; i < level_num; ++i)
		{
			if (next_lv > pGuildSkill->skill_lv)
			{
				ret = ERROR_ID_GUILD_SKILL_DEVELOP_FIRST;
				LOG_ERR("[%s:%d] player[%lu] skill develop level, guild_id:%u, skill_id:%u, skill_lv:%u, develop_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id, next_lv, pGuildSkill->skill_lv);
				break;
			}

			GangsSkillTable *config = get_guild_skill_config(skill_id, next_lv);
			if (!config)
			{
				ret = ERROR_ID_GUILD_SKILL_LEVEL_MAX;
				LOG_ERR("[%s:%d] player[%lu] get skill config failed, guild_id:%u, skill_id:%u, skill_lv:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id, next_lv);
				break;
			}

			need_donation += config->UseMoney1;
			need_coin += config->UseMoney2;
			next_lv++;
		}

		if (ret != 0)
		{
			break;
		}

		if (player->donation < need_donation)
		{
			ret = ERROR_ID_GUILD_SKILL_PRACTICE_RESOURCE;
			LOG_ERR("[%s:%d] player[%lu] donation not enough, guild_id:%u, skill_id:%u, skill_id:%u, has_donation:%u, need_donation:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id, next_lv, player->donation, need_donation);
			break;
		}

		{
			//请求扣除消耗
			PROTO_GUILDSRV_CHECK_AND_COST_REQ *cost_req = (PROTO_GUILDSRV_CHECK_AND_COST_REQ *)get_send_buf(SERVER_PROTO_GUILDSRV_COST_REQUEST, get_seq());
			cost_req->head.len = ENDION_FUNC_4(sizeof(PROTO_GUILDSRV_CHECK_AND_COST_REQ) + sizeof(GUILD_SKILL_PRACTICE_CARRY));
			memset(cost_req->head.data, 0, sizeof(PROTO_GUILDSRV_CHECK_AND_COST_REQ) - sizeof(PROTO_HEAD));
			cost_req->cost.statis_id = MAGIC_TYPE_GUILD_SKILL_PRACTICE;
			cost_req->cost.coin = need_coin;
			cost_req->data_size = sizeof(GUILD_SKILL_PRACTICE_CARRY);
			GUILD_SKILL_PRACTICE_CARRY *pCarry = (GUILD_SKILL_PRACTICE_CARRY*)cost_req->data;
			pCarry->skill_id = skill_id;
			pCarry->level_num = level_num;
			pCarry->need_donation = need_donation;
			add_extern_data(&cost_req->head, extern_data);
			if (connecter.send_one_msg(&cost_req->head, 1) != (int)ENDION_FUNC_4(cost_req->head.len))
			{
				LOG_ERR("[%s:%d] send to gamesrv failed err[%d]", __FUNCTION__, __LINE__, errno);
			}
		}
	} while(0);

	if (ret != 0)
	{
		GuildSkillUpgradeAnswer resp;
		guild_skill_upgrade_answer__init(&resp);

		resp.result = ret; 
		resp.skillid = skill_id;
		if (pSkill)
		{
			resp.level = pSkill->skill_lv;
		}

		fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_SKILL_PRACTICE_ANSWER, guild_skill_upgrade_answer__pack, resp);
	}
	return 0;
}

static int handle_guild_skill_practice_cost(int data_len, uint8_t *data, int result, EXTERN_DATA *extern_data)
{
	GUILD_SKILL_PRACTICE_CARRY *pCarry = (GUILD_SKILL_PRACTICE_CARRY *)data;
	uint32_t skill_id = pCarry->skill_id;
	uint32_t level_num = pCarry->level_num;
	uint32_t need_donation = pCarry->need_donation;

	int ret = result;
	GuildSkill *pSkill = NULL;
	bool internal = false;
	do
	{
		if (ret != 0)
		{
			if (ret == ERROR_ID_COIN_IS_NOT_ENOUGH)
			{
				ret = ERROR_ID_GUILD_SKILL_PRACTICE_RESOURCE;
			}
			break;
		}
		internal = true;

		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		GuildInfo *guild = player->guild;
		pSkill = get_player_skill_info(player, skill_id);
		if (!pSkill)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] skill memory exhaust, guild_id:%u, skill_id:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id);
			break;
		}

		if (player->donation < need_donation)
		{
			ret = ERROR_ID_GUILD_PLAYER_DONATION;
			LOG_ERR("[%s:%d] player[%lu] donation not enough, guild_id:%u, skill_id:%u, skill_lv:%u, level_num:%u, has_donation:%u, need_donation:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, skill_id, pSkill->skill_lv, level_num, player->donation, need_donation);
			break;
		}

		sub_player_donation(player, need_donation, false);

		pSkill->skill_id = skill_id;
		pSkill->skill_lv += level_num;
		save_guild_player(player);

		sync_guild_skill_to_gamesrv(player);
		fast_send_msg_base(&conn_node_guildsrv::connecter, extern_data, SERVER_PROTO_GUILD_SKILL_LEVEL_UP, 0, 0);
	} while(0);
	
	GuildSkillUpgradeAnswer resp;
	guild_skill_upgrade_answer__init(&resp);

	resp.result = ret; 
	resp.skillid = skill_id;
	if (pSkill)
	{
		resp.level = pSkill->skill_lv;
	}

	fast_send_msg(&conn_node_guildsrv::connecter, extern_data, MSG_ID_GUILD_SKILL_PRACTICE_ANSWER, guild_skill_upgrade_answer__pack, resp);

	return (internal ? ret : 0);
}

int conn_node_guildsrv::handle_guild_shop_info_request(EXTERN_DATA *extern_data)
{
	resp_guild_shop_info(extern_data->player_id);
	return 0;
}

int conn_node_guildsrv::handle_guild_shop_buy_request(EXTERN_DATA *extern_data)
{
	ShopBuyRequest *req = shop_buy_request__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	uint32_t goods_id = req->goodsid;
	uint32_t buy_num = req->goodsnum;
	shop_buy_request__free_unpacked(req, NULL);

	int ret = 0;
	GuildPlayer *player = NULL;
	do
	{
		if (buy_num == 0)
		{
			ret = ERROR_ID_SHOP_GOODS_BUY_NUM;
			break;
		}

		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_SHOP_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		GuildInfo *guild = player->guild;
		if (!goods_is_on_sell(guild, goods_id))
		{
			ret = ERROR_ID_GUILD_SHOP_LEVEL;
			LOG_ERR("[%s:%d] player[%lu] goods not on sell, guild_id:%u, goods_id:%u, buy_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, goods_id, buy_num);
			break;
		}

		ShopTable *config = get_config_by_id(goods_id, &shop_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get goods config failed, guild_id:%u, goods_id:%u, buy_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, goods_id, buy_num);
			break;
		}

		uint32_t need_history_donation = config->Condition;
		if (player->all_history_donation < need_history_donation)
		{
			ret = ERROR_ID_GUILD_HISTORY_DONATION;
			LOG_ERR("[%s:%d] player[%lu] history donation, guild_id:%u, goods_id:%u, buy_num:%u, need:%u, has:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, goods_id, buy_num, need_history_donation, player->all_history_donation);
			break;
		}

		GuildGoods *pGoods = get_player_goods_info(player, goods_id);
		if (!pGoods)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] goods memory exhaust, guild_id:%u, goods_id:%u, buy_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, goods_id, buy_num);
			break;
		}

		uint32_t limit_num = config->BuyNum;
		if ((int64_t)config->BuyNum > 0 && pGoods->bought_num + buy_num > limit_num)
		{
			ret = ERROR_ID_SHOP_GOODS_REMAIN;
			LOG_ERR("[%s:%d] player[%lu] goods remain, guild_id:%u, goods_id:%u, buy_num:%u, bought_num:%u, limit_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, goods_id, buy_num, pGoods->bought_num, limit_num);
			break;
		}

		uint32_t need_donation = config->Discount * buy_num;
		if (player->donation < need_donation)
		{
			ret = ERROR_ID_GUILD_PLAYER_DONATION;
			LOG_ERR("[%s:%d] player[%lu] donation not enough, guild_id:%u, goods_id:%u, has:%u, need:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, goods_id, player->donation, need_donation);
			break;
		}

		{
			//请求发放货物
			PROTO_GUILDSRV_REWARD_REQ *reward_req = (PROTO_GUILDSRV_REWARD_REQ *)get_send_buf(SERVER_PROTO_GUILDSRV_REWARD_REQUEST, get_seq());
			reward_req->head.len = ENDION_FUNC_4(sizeof(PROTO_GUILDSRV_REWARD_REQ) + sizeof(GUILD_SHOP_BUY_CARRY));
			memset(reward_req->head.data, 0, sizeof(PROTO_GUILDSRV_REWARD_REQ) - sizeof(PROTO_HEAD));
			reward_req->statis_id = MAGIC_TYPE_SHOP_BUY;
			reward_req->item_id[0] = config->ItemID;
			reward_req->item_num[0] = buy_num;
			reward_req->data_size = sizeof(GUILD_SHOP_BUY_CARRY);
			GUILD_SHOP_BUY_CARRY *pCarry = (GUILD_SHOP_BUY_CARRY*)reward_req->data;
			pCarry->goods_id = goods_id;
			pCarry->buy_num = buy_num;
			pCarry->need_donation = need_donation;
			add_extern_data(&reward_req->head, extern_data);
			if (connecter.send_one_msg(&reward_req->head, 1) != (int)ENDION_FUNC_4(reward_req->head.len))
			{
				LOG_ERR("[%s:%d] send to gamesrv failed err[%d]", __FUNCTION__, __LINE__, errno);
			}
		}
	} while(0);

	if (ret != 0)
	{
		ShopBuyAnswer resp;
		shop_buy_answer__init(&resp);

		resp.result = ret; 
		resp.goodsid = goods_id;
		resp.boughtnum = 0;

		fast_send_msg(&connecter, extern_data, MSG_ID_SHOP_BUY_ANSWER, shop_buy_answer__pack, resp);
	}

	return 0;
}

static int handle_shop_buy_answer(int data_len, uint8_t *data, int result, EXTERN_DATA *extern_data)
{
	GUILD_SHOP_BUY_CARRY *pCarry = (GUILD_SHOP_BUY_CARRY*)data;
	uint32_t goods_id = pCarry->goods_id;
	uint32_t buy_num = pCarry->buy_num;
	uint32_t need_donation = pCarry->need_donation;

	int ret = result;
	GuildPlayer *player = NULL;
	GuildGoods *pGoods = NULL;
	do
	{
		if (ret != 0)
		{
			break;
		}
		
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		GuildInfo *guild = player->guild;
		ShopTable *config = get_config_by_id(goods_id, &shop_config);
		if (!config)
		{
			ret = ERROR_ID_NO_CONFIG;
			LOG_ERR("[%s:%d] player[%lu] get goods config failed, guild_id:%u, goods_id:%u, buy_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, goods_id, buy_num);
			break;
		}

		pGoods = get_player_goods_info(player, goods_id);
		if (!pGoods)
		{
			ret = ERROR_ID_SERVER;
			LOG_ERR("[%s:%d] player[%lu] goods memory exhaust, guild_id:%u, goods_id:%u, buy_num:%u", __FUNCTION__, __LINE__, extern_data->player_id, guild->guild_id, goods_id, buy_num);
			break;
		}

		sub_player_donation(player, need_donation, false);

		if ((int64_t)config->BuyNum > 0)
		{
			pGoods->goods_id = goods_id;
			pGoods->bought_num += buy_num;
			save_guild_player(player);
		}
	} while(0);

	ShopBuyAnswer resp;
	shop_buy_answer__init(&resp);

	resp.result = ret; 
	resp.goodsid = goods_id;
	if (pGoods)
	{
		resp.boughtnum = pGoods->bought_num;
	}

	fast_send_msg(&conn_node_guildsrv::connecter, extern_data, MSG_ID_SHOP_BUY_ANSWER, shop_buy_answer__pack, resp);

	return 0;
}

int conn_node_guildsrv::handle_guild_chat_request(EXTERN_DATA *extern_data)
{
	Chat *req = chat__unpack(NULL, get_data_len(), get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] player[%lu] unpack failed", __FUNCTION__, __LINE__, extern_data->player_id);
		return -1;
	}

	int ret = 0;
	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = CHAR_RET_CODE__noGuild;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		broadcast_guild_chat(player->guild, req);
		player->guild->answer.Answer(extern_data, req->contain, req->sendname);
	} while(0);

	CommAnswer resp;
	comm_answer__init(&resp);
	
	resp.result = ret;
	fast_send_msg(&conn_node_guildsrv::connecter, extern_data, MSG_ID_CHAT_ANSWER, comm_answer__pack, resp);

	chat__free_unpacked(req, NULL);

	return 0;
}

int conn_node_guildsrv::handle_guild_prodece_medicine_request(EXTERN_DATA *extern_data)
{
	PROTO_UNDO_COST *req = (PROTO_UNDO_COST*)buf_head();

	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player)
		{
			LOG_ERR("[%s:%d] can't find guild player[%lu]", __FUNCTION__, __LINE__, extern_data->player_id);
			continue;
		}

		if (player->guild == NULL)
		{
			LOG_ERR("[%s:%d] player[%lu] is not guild", __FUNCTION__, __LINE__, extern_data->player_id);
			continue;
		}

		if (sub_player_donation(player, req->cost.gold) == 0)
		{
			req->cost.gold = req->cost.coin;
			req->cost.coin = 0;
		}
	} while (0);
	
	if (conn_node_guildsrv::connecter.send_one_msg((PROTO_HEAD *)req, 1) != (int)ENDION_FUNC_4(req->head.len))
	{
		LOG_ERR("[%s:%d] send to gamesrv failed err[%d]", __FUNCTION__, __LINE__, errno);
	}

	return 0;
}

int conn_node_guildsrv::handle_get_other_info_request(EXTERN_DATA *extern_data)
{
	GetOtherInfoAnswer *req = get_other_info_answer__unpack(NULL, get_data_len(), (uint8_t *)get_data());
	if (!req)
	{
		LOG_ERR("[%s:%d] can not unpack player[%lu] cmd", __FUNCTION__, __LINE__, extern_data->player_id);
		return (-10);
	}

	if (req->result != 0)
	{
		PROTO_HEAD *head = get_head();
		if (connecter.send_one_msg(head, 1) != (int)ENDION_FUNC_4(head->len))
		{
			LOG_ERR("[%s:%d] send to client failed err[%d]", __FUNCTION__, __LINE__, errno);
		}
	}
	else
	{
		GuildPlayer *target = get_guild_player(req->data->playerid);
		if (target && target->guild)
		{
			req->data->guildid = target->guild->guild_id;
			req->data->guildoffice = target->office;

			int name_len = strlen(target->guild->name);
			if (name_len == 0)
			{
				if (req->data->guildname)
				{
					req->data->guildname[0] = '\0';
				}
			}
			else
			{
				req->data->guildname = (char*)realloc(req->data->guildname, name_len + 1);
				strcpy(req->data->guildname, target->guild->name);
			}
		}

		fast_send_msg(&conn_node_guildsrv::connecter, extern_data, MSG_ID_GET_OTHER_INFO_ANSWER, get_other_info_answer__pack, *req);
	}

	get_other_info_answer__free_unpacked(req, NULL);
	return 0;
}

int conn_node_guildsrv::handle_guild_battle_call_request(EXTERN_DATA *extern_data)
{
	int ret = 0;
	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		GuildInfo *guild = player->guild;
		if (player->office != GUILD_OFFICE_TYPE__OFFICE_MASTER)
		{
			ret = ERROR_ID_GUILD_PLAYER_NO_PERMISSION;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		//发送消息到game_srv
		{
			PROTO_HEAD *head = (PROTO_HEAD*)get_send_buf(MSG_ID_GUILD_BATTLE_CALL_REQUEST, 0);
			uint32_t *player_num = (uint32_t*)head->data;
			*player_num = 0;
			uint64_t *player_id = (uint64_t*)((char*)head->data + sizeof(uint32_t));
			for (uint32_t i = 0; i < guild->member_num; ++i)
			{
				*player_id++ = guild->members[i]->player_id;
				(*player_num)++;
			}
			head->len = ENDION_FUNC_4(sizeof(PROTO_HEAD) + sizeof(uint32_t) + sizeof(uint64_t) * (*player_num));
			conn_node_base::add_extern_data(head, extern_data);
			if (connecter.send_one_msg(head, 1) != (int)ENDION_FUNC_4(head->len))
			{
				LOG_ERR("[%s:%d] send to game_srv failed err[%d]", __FUNCTION__, __LINE__, errno);
			}
		}
	} while(0);

	if (ret != 0)
	{
		CommAnswer resp;
		comm_answer__init(&resp);

		resp.result = ret;
		fast_send_msg(&conn_node_guildsrv::connecter, extern_data, MSG_ID_GUILD_BATTLE_CALL_ANSWER, comm_answer__pack, resp);
	}

	return 0;
}

int conn_node_guildsrv::handle_guild_battle_enter_wait_request(EXTERN_DATA *extern_data)
{
	uint8_t *pData = get_data();
	bool settling = (bool)(*pData);

	do
	{
		GuildPlayer *player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		GuildInfo *guild = player->guild;

		GuildBattleWaitInfoNotify nty;
		guild_battle_wait_info_notify__init(&nty);
		nty.guildscore = guild->battle_score;
		nty.has_guildscore = true;
		fast_send_msg(&connecter, extern_data, MSG_ID_GUILD_BATTLE_WAIT_INFO_NOTIFY, guild_battle_wait_info_notify__pack, nty);
	} while(0);

	if (settling)
	{
		notify_guild_battle_activity_settle(extern_data);
	}

	return 0;
}

static void fill_player_base_data(PlayerRedisInfo &info, PlayerBaseData &data)
{
	data.name = info.name;
	data.n_attrs = 0;
	data.attrs[data.n_attrs]->id = PLAYER_ATTR_LEVEL;
	data.attrs[data.n_attrs]->val = info.lv;
	data.n_attrs++;
	data.attrs[data.n_attrs]->id = PLAYER_ATTR_JOB;
	data.attrs[data.n_attrs]->val = info.job;
	data.n_attrs++;
	data.attrs[data.n_attrs]->id = PLAYER_ATTR_HEAD;
	data.attrs[data.n_attrs]->val = info.head_icon;
	data.n_attrs++;
	data.attrs[data.n_attrs]->id = PLAYER_ATTR_FIGHTING_CAPACITY;
	data.attrs[data.n_attrs]->val = info.fighting_capacity;
	data.n_attrs++;
}

#define MAX_RANK_GET_NUM  100
int conn_node_guildsrv::handle_guild_battle_info_request(EXTERN_DATA *extern_data)
{
	int ret = 0;
	std::vector<std::pair<uint64_t, uint32_t> > rank_info;
	AutoReleaseBatchRedisPlayer t1;		
	std::map<uint64_t, PlayerRedisInfo *> redis_players;
	uint32_t my_rank = 0;
	GuildPlayer *player = NULL;
	GuildInfo *guild = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			ret = ERROR_ID_GUILD_PLAYER_NOT_JOIN;
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		guild = player->guild;
		char *rank_key = sg_rank_guild_battle_key;
		int ret2 = sg_redis_client.zget(rank_key, 0, 99, rank_info);
		if (ret2 != 0)
		{
			ret = ERROR_ID_RANK_REDIS;
			LOG_ERR("[%s:%d] player[%lu] get rank failed, rank_key:%s", __FUNCTION__, __LINE__, extern_data->player_id, rank_key);
			break;
		}

		for (size_t i = 0; i < rank_info.size(); ++i)
		{
			if (rank_info[i].first == guild->guild_id)
			{
				my_rank = i + 1;
			}
		}

		std::set<uint64_t> playerIds;
		for (uint32_t i = 0; i < guild->member_num; ++i)
		{
			playerIds.insert(guild->members[i]->player_id);
		}
		if (get_more_redis_player(playerIds, redis_players, sg_player_key, sg_redis_client, t1) != 0)
		{
			ret = ERROR_ID_RANK_REDIS;
			LOG_ERR("[%s:%d] player[%lu] get player failed", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}
	} while(0);

	GuildBattleInfoAnswer resp;
	guild_battle_info_answer__init(&resp);

	GuildBattleRankData rank_data[MAX_RANK_GET_NUM];
	GuildBattleRankData* rank_point[MAX_RANK_GET_NUM];
	GuildBattleContributionData member_data[MAX_GUILD_MEMBER_NUM];
	GuildBattleContributionData* member_point[MAX_GUILD_MEMBER_NUM];
	PlayerBaseData base_data[MAX_GUILD_MEMBER_NUM];
	AttrData attr_data[MAX_GUILD_MEMBER_NUM][MAX_PLAYER_BASE_ATTR_NUM];
	AttrData* attr_point[MAX_GUILD_MEMBER_NUM][MAX_PLAYER_BASE_ATTR_NUM];

	resp.result = ret;
	if (ret == 0)
	{
		resp.ranks = rank_point;
		resp.n_ranks = 0;
		for (size_t i = 0; i < rank_info.size(); ++i)
		{
			GuildInfo *tmp_guild = get_guild(rank_info[i].first);
			if (!tmp_guild)
			{
				continue;
			}

			rank_point[resp.n_ranks] = &rank_data[resp.n_ranks];
			guild_battle_rank_data__init(&rank_data[resp.n_ranks]);
			rank_data[resp.n_ranks].rank = i + 1;
			rank_data[resp.n_ranks].guildid = tmp_guild->guild_id;
			rank_data[resp.n_ranks].guildname = tmp_guild->name;
			rank_data[resp.n_ranks].guildscore = rank_info[i].second;
			resp.n_ranks++;
		}

		resp.contributions = member_point;
		resp.n_contributions = 0;
		for (uint32_t i = 0; i < guild->member_num; ++i)
		{
			GuildPlayer *tmp_player = guild->members[i];
			if (tmp_player->battle_score == 0)
			{
				continue;
			}

			member_point[resp.n_contributions] = &member_data[resp.n_contributions];
			guild_battle_contribution_data__init(&member_data[resp.n_contributions]);

			member_data[resp.n_contributions].baseinfo = &base_data[resp.n_contributions];
			player_base_data__init(&base_data[resp.n_contributions]);
			base_data[resp.n_contributions].attrs = attr_point[resp.n_contributions];
			base_data[resp.n_contributions].n_attrs = 0;
			for (int j = 0; j < MAX_PLAYER_BASE_ATTR_NUM; ++j)
			{
				attr_point[resp.n_contributions][j] = &attr_data[resp.n_contributions][j];
				attr_data__init(&attr_data[resp.n_contributions][j]);
			}
			PlayerRedisInfo *redis_player = find_redis_from_map(redis_players, tmp_player->player_id);
			if (redis_player)
			{
				fill_player_base_data(*redis_player, base_data[resp.n_contributions]);
			}
			base_data[resp.n_contributions].playerid = tmp_player->player_id;

			member_data[resp.n_contributions].score = tmp_player->battle_score;
			resp.n_contributions++;
		}

		resp.myguildrank = my_rank;
		resp.myguildscore = guild->battle_score;
		resp.myscore = player->battle_score;
	}

	fast_send_msg(&conn_node_guildsrv::connecter, extern_data, MSG_ID_GUILD_BATTLE_INFO_ANSWER, guild_battle_info_answer__pack, resp);

	// for (std::map<uint64_t, PlayerRedisInfo*>::iterator iter = redis_players.begin(); iter != redis_players.end(); ++iter)
	// {
	// 	player_redis_info__free_unpacked(iter->second, NULL);
	// }
	return 0;
}

int conn_node_guildsrv::handle_guild_battle_fight_reward_request(EXTERN_DATA * /*extern_data*/)
{
	PROTO_GUILD_BATTLE_REWARD *req = (PROTO_GUILD_BATTLE_REWARD*)buf_head();
	LOG_INFO("[%s:%d] guild %u", __FUNCTION__, __LINE__, req->guild_id);
	uint32_t activity_id = req->activity_id;
	
	GuildInfo *guild = get_guild(req->guild_id);
	do
	{
		if (!guild)
		{
			LOG_ERR("[%s:%d] can't find guild[%u]", __FUNCTION__, __LINE__, req->guild_id);
			break;
		}

		uint32_t total_treasure = 0, total_score = 0;
		for (uint32_t i = 0; i < req->player_num; ++i)
		{
			GuildPlayer *player = get_guild_player(req->player_id[i]);
			if (!player)
			{
				LOG_ERR("[%s:%d] can't find guild player[%lu]", __FUNCTION__, __LINE__, req->player_id[i]);
				continue;
			}

			if (player->guild != guild)
			{
				LOG_ERR("[%s:%d] player[%lu] guild[%u] is not guild[%u]", __FUNCTION__, __LINE__, req->player_id[i], player->guild->guild_id, guild->guild_id);
				continue;
			}

			add_player_donation(player, req->donation[i]);
			add_player_battle_score(player, req->score[i]);
			total_treasure += req->treasure[i];
			total_score += req->score[i];
		}

		//为了减少消息广播和数据存库的次数，统计好一次过加
		add_guild_treasure(guild, total_treasure);
		add_guild_battle_score(guild, total_score);

		if (req->broadcast_num > 0)
		{
			std::vector<uint64_t> broadcast_ids;
			for (uint32_t i = 0; i < req->broadcast_num; ++i)
			{
				broadcast_ids.push_back(req->broadcast_id[i]);
			}

			broadcast_guild_battle_score(guild, broadcast_ids);
		}
	} while(0);

	do
	{
		if (!guild_battle_is_final(activity_id))
		{
			break;
		}

		std::vector<std::pair<uint64_t, uint32_t> > rank_info;
		char *rank_key = sg_rank_guild_battle_key;
		int ret2 = sg_redis_client.zget(rank_key, 0, 3, rank_info);
		if (ret2 != 0)
		{
			LOG_ERR("[%s:%d] get rank failed, rank_key:%s", __FUNCTION__, __LINE__, rank_key);
			break;
		}

		std::vector<uint32_t> guild_ids;
		load_guild_battle_final_list(guild_ids);
		if (guild_ids.size() > rank_info.size())
		{
			for (size_t i = 0; i < guild_ids.size(); ++i)
			{
				bool has = false;
				for (size_t j = 0; j < rank_info.size(); ++j)
				{
					if (rank_info[j].first == guild_ids[i])
					{
						has = true;
						break;
					}
				}

				if (!has)
				{
					rank_info.push_back(std::make_pair(guild_ids[i], 0));
				}
			}
		}

		GuildBattleRoundFinishNotify nty;
		GuildBattleRankData rank_data[MAX_GUILD_BATTLE_FINAL_GUILD_NUM];
		GuildBattleRankData* rank_point[MAX_GUILD_BATTLE_FINAL_GUILD_NUM];

		for (uint32_t i = 0; i < req->player_num; ++i)
		{
			if (req->result[i] == 0)
			{
				continue;
			}

			GuildPlayer *player = get_guild_player(req->player_id[i]);
			if (!player)
			{
				LOG_ERR("[%s:%d] can't find guild player[%lu]", __FUNCTION__, __LINE__, req->player_id[i]);
				continue;
			}

			if (player->guild != guild)
			{
				LOG_ERR("[%s:%d] player[%lu] guild[%u] is not guild[%u]", __FUNCTION__, __LINE__, req->player_id[i], player->guild->guild_id, guild->guild_id);
				continue;
			}

			guild_battle_round_finish_notify__init(&nty);

			nty.result = req->result[i];
			nty.score = req->score[i];
			nty.guildtreasure = req->treasure[i];
			nty.guilddonation = req->donation[i];
			nty.ranks = rank_point;
			nty.n_ranks = 0;
			for (size_t j = 0; j < rank_info.size() && j < MAX_GUILD_BATTLE_FINAL_GUILD_NUM; ++j)
			{
				GuildInfo *tmp_guild = get_guild(rank_info[j].first);
				if (!tmp_guild)
				{
					continue;
				}
				AutoReleaseRedisPlayer t1;
				PlayerRedisInfo *redis_player = get_redis_player(tmp_guild->master_id, sg_player_key, sg_redis_client, t1);
				if (!redis_player)
				{
					continue;
				}

				rank_point[nty.n_ranks] = &rank_data[nty.n_ranks];
				guild_battle_rank_data__init(&rank_data[nty.n_ranks]);
				rank_data[nty.n_ranks].rank = j + 1;
				rank_data[nty.n_ranks].guildid = tmp_guild->guild_id;
				rank_data[nty.n_ranks].guildname = tmp_guild->name;
				rank_data[nty.n_ranks].guildscore = rank_info[j].second;
				rank_data[nty.n_ranks].guildcamp = redis_player->zhenying;
				nty.n_ranks++;
			}

			EXTERN_DATA ext_data;
			ext_data.player_id = player->player_id;

			fast_send_msg(&conn_node_guildsrv::connecter, &ext_data, MSG_ID_GUILD_BATTLE_ROUND_FINISH_NOTIFY, guild_battle_round_finish_notify__pack, nty);
		}
	} while(0);

	return 0;
}

int conn_node_guildsrv::handle_guild_battle_sync_begin(EXTERN_DATA * /*extern_data*/)
{
	PROTO_HEAD *head = (PROTO_HEAD*)buf_head();
	uint32_t *pData = (uint32_t*)(head->data);
	uint32_t activity_id = *pData;
	LOG_INFO("[%s:%d] activity %u", __FUNCTION__, __LINE__, activity_id);

	guild_battle_opening = true;
	if (!guild_battle_is_final(activity_id))
	{ //预赛
	}
	else
	{ //决赛
		/* 决赛开始，保存参赛名单，清除积分排行榜 */
		std::vector<uint32_t> guild_ids;
		do
		{
			std::vector<std::pair<uint64_t, uint32_t> > rank_info;
			char *rank_key = sg_rank_guild_battle_key;
			int ret2 = sg_redis_client.zget(rank_key, 0, 3, rank_info);
			if (ret2 != 0)
			{
				break;
			}

			for (size_t i = 0; i < rank_info.size(); ++i)
			{
				guild_ids.push_back(rank_info[i].first);
			}
		} while(0);

		save_guild_battle_final_list(guild_ids);
		clear_player_total_battle_score();
	}

	return 0;
}

int conn_node_guildsrv::handle_guild_battle_sync_end(EXTERN_DATA * /*extern_data*/)
{
	PROTO_HEAD *head = (PROTO_HEAD*)buf_head();
	uint32_t *pData = (uint32_t*)(head->data);
	uint32_t activity_id = *pData;
	LOG_INFO("[%s:%d] activity %u", __FUNCTION__, __LINE__, activity_id);

	guild_battle_opening = false;
	uint32_t param_id = (guild_battle_is_final(activity_id) ? 161000319 : 161000318);
	ParameterTable *param_config = get_config_by_id(param_id, &parameter_config);
	if (param_config)
	{
		Chat nty;
		chat__init(&nty);
		nty.channel = CHANNEL__family;
		nty.contain = param_config->parameter2;

		std::map<uint32_t, GuildInfo*> &guild_map = get_all_guild();
		for (std::map<uint32_t, GuildInfo*>::iterator iter = guild_map.begin(); iter != guild_map.end(); ++iter)
		{
			GuildInfo *guild = iter->second;
			for (uint32_t i = 0; i < guild->member_num; ++i)
			{
				if (guild->members[i]->act_battle_score > 0)
				{
					broadcast_guild_chat(guild, &nty);
					break;
				}
			}
		}
	}

	if (!guild_battle_is_final(activity_id))
	{ //预赛
		//清除本次活动的玩家积分
		clear_player_act_battle_score();
	}
	else
	{ //决赛
		/* 决赛结束，清除参赛名单，清除积分排行榜 */
		clear_player_total_battle_score();
		sg_redis_client.del(sg_guild_battle_final_key);
	}

	return 0;
}

int conn_node_guildsrv::handle_guild_battle_sync_settle(EXTERN_DATA * /*extern_data*/)
{
	PROTO_GUILD_BATTLE_SETTLE *proto = (PROTO_GUILD_BATTLE_SETTLE*)buf_head();
	uint32_t activity_id = proto->activity_id;
	LOG_INFO("[%s:%d] activity %u", __FUNCTION__, __LINE__, activity_id);

	std::vector<std::pair<uint64_t, uint32_t> > rank_info;
	int ret = 0;
	do
	{
		CRedisClient &rc = sg_redis_client;
		char *rank_key = sg_rank_guild_battle_key;
		uint32_t rank_end;
		ret =  rc.zcard(rank_key, rank_end);
		if (ret != 0)
		{
			LOG_ERR("[%s:%d] rank_key:%s, ret:%d", __FUNCTION__, __LINE__, rank_key, ret);
			break;
		}

		ret = rc.zget(rank_key, 0, rank_end, rank_info);
		if (ret != 0)
		{
			LOG_ERR("[%s:%d] rank_key:%s, ret:%d", __FUNCTION__, __LINE__, rank_key, ret);
			break;
		}

		for (std::map<uint64_t, GangsDungeonTable*>::iterator iter = guild_battle_reward_config.begin(); iter != guild_battle_reward_config.end(); ++iter)
		{
			GangsDungeonTable *config = iter->second;
			if (guild_battle_is_final(activity_id))
			{
				if (config->Type != 1)
				{
					continue;
				}
			}
			else
			{
				if (config->Type != 0)
				{
					continue;
				}
			}

			if ((uint32_t)config->RankHigh > rank_end)
			{
				continue;
			}

			for (uint32_t i = (uint32_t)config->RankHigh - 1; i < (uint32_t)config->RankLow && i < rank_end; ++i)
			{
				std::map<uint32_t, uint32_t> attachs;
				for (uint32_t j = 0; j < config->n_RewardID; ++j)
				{
					attachs[config->RewardID[j]] += config->RewardNum[j];
				}

				uint32_t guild_id = rank_info[i].first;
				GuildInfo *guild = get_guild(guild_id);
				if (!guild)
				{
					continue;
				}

				for (uint32_t k = 0; k < guild->member_num; ++k)
				{
					GuildPlayer *player = guild->members[k];
					if (player->act_battle_score == 0)
					{
						continue;
					}

					send_mail(&connecter, player->player_id, config->mailID, NULL, NULL, NULL, NULL, &attachs, MAGIC_TYPE_GUILD_BATTLE_ACTIVITY);
				}
			}
		}
	} while(0);

	do
	{
		EXTERN_DATA ext_data;
		uint64_t *ppp = proto->broadcast_id;
		for (uint32_t i = 0; i < proto->broadcast_num; ++i)
		{
			ext_data.player_id = *ppp++;
			notify_guild_battle_activity_settle(&ext_data);
		}
	} while(0);

	if (!guild_battle_is_final(activity_id))
	{ //预赛
	}
	else
	{ //决赛
	}

	return 0;
}

int conn_node_guildsrv::notify_guild_battle_activity_settle(EXTERN_DATA *extern_data)
{
	std::vector<std::pair<uint64_t, uint32_t> > rank_info;
	uint32_t my_rank = 0;
	GuildPlayer *player = NULL;
	GuildInfo *guild = NULL;
	do
	{
		player = get_guild_player(extern_data->player_id);
		if (!player || !player->guild)
		{
			LOG_ERR("[%s:%d] player[%lu] not join guild yet", __FUNCTION__, __LINE__, extern_data->player_id);
			break;
		}

		guild = player->guild;
		char *rank_key = sg_rank_guild_battle_key;
		int ret2 = sg_redis_client.zget(rank_key, 0, 3, rank_info);
		if (ret2 != 0)
		{
			LOG_ERR("[%s:%d] player[%lu] get rank failed, rank_key:%s", __FUNCTION__, __LINE__, extern_data->player_id, rank_key);
			break;
		}

		for (size_t i = 0; i < rank_info.size(); ++i)
		{
			if (rank_info[i].first == guild->guild_id)
			{
				my_rank = i + 1;
			}
		}

		if (my_rank == 0)
		{
			ret2 = sg_redis_client.zget_rank(rank_key, guild->guild_id, my_rank);
			if (ret2 == 0)
			{
				my_rank++;
			}
		}
	} while(0);

	GuildBattleActivityFinishNotify resp;
	guild_battle_activity_finish_notify__init(&resp);

	GuildBattleRankData rank_data[MAX_GUILD_BATTLE_FINAL_GUILD_NUM];
	GuildBattleRankData* rank_point[MAX_GUILD_BATTLE_FINAL_GUILD_NUM];

	resp.ranks = rank_point;
	resp.n_ranks = 0;
	for (size_t i = 0; i < rank_info.size() && i < MAX_GUILD_BATTLE_FINAL_GUILD_NUM; ++i)
	{
		GuildInfo *tmp_guild = get_guild(rank_info[i].first);
		if (!tmp_guild)
		{
			continue;
		}

		AutoReleaseRedisPlayer t1;
		PlayerRedisInfo *redis_player = get_redis_player(tmp_guild->master_id, sg_player_key, sg_redis_client, t1);
		if (!redis_player)
		{
			continue;
		}

		rank_point[resp.n_ranks] = &rank_data[resp.n_ranks];
		guild_battle_rank_data__init(&rank_data[resp.n_ranks]);
		rank_data[resp.n_ranks].rank = i + 1;
		rank_data[resp.n_ranks].guildid = tmp_guild->guild_id;
		rank_data[resp.n_ranks].guildname = tmp_guild->name;
		rank_data[resp.n_ranks].guildscore = rank_info[i].second;
		rank_data[resp.n_ranks].guildcamp = redis_player->zhenying;
		resp.n_ranks++;
	}

	resp.guildrank = my_rank;
	resp.guildscore = guild->battle_score;

	fast_send_msg(&conn_node_guildsrv::connecter, extern_data, MSG_ID_GUILD_BATTLE_ACTIVITY_FINISH_NOTIFY, guild_battle_activity_finish_notify__pack, resp);
	return 0;
}

int conn_node_guildsrv::handle_guild_battle_final_list_request(EXTERN_DATA * /*extern_data*/)
{
	std::vector<uint32_t> guild_ids;
	if (load_guild_battle_final_list(guild_ids) != 0)
	{
		std::vector<std::pair<uint64_t, uint32_t> > rank_info;
		char *rank_key = sg_rank_guild_battle_key;
		int ret2 = sg_redis_client.zget(rank_key, 0, 3, rank_info);
		if (ret2 != 0)
		{
			LOG_ERR("[%s:%d] get final list from %s failed, ret:%d", __FUNCTION__, __LINE__, rank_key, ret2);
		}
		else
		{
			for (size_t i = 0; i < rank_info.size(); ++i)
			{
				guild_ids.push_back(rank_info[i].first);
			}
		}
	}

	PROTO_GUILD_BATTLE_RANK *rank_req = (PROTO_GUILD_BATTLE_RANK*)get_send_buf(SERVER_PROTO_GUILD_BATTLE_FINAL_LIST_ANSWER, 0);
	rank_req->head.len = ENDION_FUNC_4(sizeof(PROTO_GUILD_BATTLE_RANK));
	memset(rank_req->head.data, 0, sizeof(PROTO_GUILD_BATTLE_RANK) - sizeof(PROTO_HEAD));
	for (size_t i = 0; i < guild_ids.size() && i < 4; ++i)
	{
		rank_req->guild_id[i] = guild_ids[i];
	}
	if (connecter.send_one_msg(&rank_req->head, 1) != (int)ENDION_FUNC_4(rank_req->head.len))
	{
		LOG_ERR("[%s:%d] send to game_srv failed err[%d]", __FUNCTION__, __LINE__, errno);
	}

	return 0;
}

int conn_node_guildsrv::handle_guild_battle_add_final_id(EXTERN_DATA *extern_data)
{
	PROTO_HEAD *head = (PROTO_HEAD*)buf_head();
	uint32_t *pData = (uint32_t*)(head->data);
	uint32_t guild_id = *pData;

	do
	{
		GuildInfo *guild = get_guild(guild_id);
		if (!guild)
		{
			break;
		}

		std::vector<uint32_t> guild_ids;
		load_guild_battle_final_list(guild_ids);
		if (guild_ids.size() >= 4)
		{
			break;
		}

		guild_ids.push_back(guild_id);
		save_guild_battle_final_list(guild_ids);
	} while(0);

	return 0;
}





