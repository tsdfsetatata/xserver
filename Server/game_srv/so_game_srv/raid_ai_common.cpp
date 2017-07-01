#include <math.h>
#include <stdlib.h>
#include "game_event.h"
#include "raid_ai.h"
#include "raid_ai_normal.h"
#include "raid_manager.h"
#include "buff_manager.h"
#include "collect.h"
#include "time_helper.h"
#include "app_data_statis.h"
#include "unit.h"
#include "msgid.h"
#include "raid.pb-c.h"
#include "zhenying.pb-c.h"
#include "monster_manager.h"
#include "excel_data.h"
#include "check_range.h"
#include "raid_ai_common.h"

extern void set_leixinye_type(monster_struct *monster, uint32_t type);
static player_struct *get_script_raid_event_player(raid_struct *raid)
{
	do
	{
		if (raid->m_config->DengeonType == 1)
		{
			if (!raid->m_raid_team)
			{
				break;
			}
			player_struct *player = raid->m_raid_team->GetLead();
			if (!player)
			{
				break;
			}

			int pos = -1;
			raid->get_raid_player_info(player->get_uuid(), &pos);
			if (pos < 0)
			{
				break;
			}

			return player;
		}
		else if (raid->m_config->DengeonType == 2)
		{
			return raid->m_player[0];
		}
	} while(0);

	return NULL;
}

static bool jump_to_another_script(raid_struct *raid, char *script_name, struct raid_script_data *script_data)
{
	std::vector<struct RaidScriptTable *> *configs = get_config_by_name(script_name, &all_raid_script_config);
	if (!configs)
	{
		return false;
	}

	for (int i = 0; i < MAX_SCRIPT_COND_NUM; ++i)
		script_data->cur_finished_num[i] = 0;
	script_data->cur_index = 0xffff;
	script_data->script_config = configs;

	return true;
}

static bool do_check_script_raid_monster_hp(double percent, monster_struct* monster);
static bool script_raid_check_finished(raid_struct *raid, struct raid_script_data *script_data)
{
	assert(script_data->cur_index < script_data->script_config->size());
	struct RaidScriptTable *config = (*script_data->script_config)[script_data->cur_index];
	switch (config->TypeID)
	{
		case SCRIPT_EVENT_STOP_MONSTER_AI:
		case SCRIPT_EVENT_TIME_OUT: //副本计时
			if (time_helper::get_cached_time() / 1000 >= script_data->cur_finished_num[0])
				return true;
			return false;
//		case SCRIPT_EVENT_MONSTER_DEAD_NUM: //指定怪物死亡
//			return false;
//		case SCRIPT_EVENT_MONSTER_DEAD_ALL: //所有指定怪物死亡 等同于检测副本内还存活指定怪物数量小于某值
//			return false;
//		case SCRIPT_EVENT_COLLECT_NUM: //采集指定采集物
//			return false;
		case SCRIPT_EVENT_ALIVE_MONSTER_EQUEL: //检测副本内还存活指定怪物数量等于某值
		{
			for (size_t i = 0; i + 1 < config->n_Parameter1; i = i+2)
			{
				uint32_t num = raid->get_id_monster_num(config->Parameter1[i]);
				if (num != config->Parameter1[i + 1])
					return false;
			}
			return true;
		}
		case SCRIPT_EVENT_ALIVE_MONSTER_LESSER: //检测副本内还存活指定怪物数量小于某值
		{
			for (size_t i = 0; i + 1 < config->n_Parameter1; i = i+2)
			{
				uint32_t num = raid->get_id_monster_num(config->Parameter1[i]);
				if (num >= config->Parameter1[i + 1])
					return false;
			}
			return true;
		}
//		case SCRIPT_EVENT_ALIVE_MONSTER_GREATER: //检测副本内还存活指定怪物数量大于某值  这个有啥用？
//			return false;
		case SCRIPT_EVENT_ALIVE_COLLECT_EQUEL: //检测副本内还存活指定采集物数量等于某值
			for (size_t i = 0; i + 1 < config->n_Parameter1; i = i+2)
			{
				uint32_t num = raid->get_id_collect_num(config->Parameter1[i]);
				if (num != config->Parameter1[i + 1])
					return false;
			}
			return true;
		case SCRIPT_EVENT_ACCEPT_TASK:
			{
				do
				{
					player_struct *player = get_script_raid_event_player(raid);
					if (!player)
					{
						break;
					}

					for (size_t i = 0; i < config->n_Parameter1; ++i)
					{
						if (player->get_task_info(config->Parameter1[i]) == NULL)
						{
							return false;
						}
					}
					return true;
				} while(0);
			}
			return false;
		case SCRIPT_EVENT_MONSTER_ARRIVE_POSITION:
			{
				do
				{
					if (config->n_Parameter1 < 4)
					{
						break;
					}

					uint32_t monster_id = config->Parameter1[0];
					position target_pos;
					target_pos.pos_x = config->Parameter1[1];
					target_pos.pos_z = config->Parameter1[2];

					for (std::set<monster_struct*>::iterator iter = raid->m_monster.begin(); iter != raid->m_monster.end(); ++iter)
					{
						monster_struct *monster = *iter;
						if (monster->data->monster_id != monster_id)
						{
							continue;
						}
						
						position *cur_pos = monster->get_pos();
						if (check_circle_in_range(cur_pos, &target_pos, config->Parameter1[3]))
						{
							return true;
						}
					}
				} while(0);
			}
			return false;
		case SCRIPT_EVENT_PLAYER_ARRIVE_POSITION:
			{
				do
				{
					player_struct *player = get_script_raid_event_player(raid);
					if (!player)
					{
						break;
					}

					if (config->n_Parameter1 < 3)
					{
						break;
					}

					position *cur_pos = player->get_pos();
					position target_pos;
					target_pos.pos_x = config->Parameter1[0];
					target_pos.pos_z = config->Parameter1[1];
					if (check_circle_in_range(cur_pos, &target_pos, config->Parameter1[2]))
					{
						return true;
					}
				} while(0);
			}
			return false;
		case SCRIPT_EVENT_PLAYER_TASK_FORK:
			{
				do
				{
					player_struct *player = get_script_raid_event_player(raid);
					if (!player)
					{
						break;
					}

					for (size_t i = 0; i + 1 < config->n_Parameter1; i = i+2)
					{
						if (player->get_task_info(config->Parameter1[i]) != NULL)
						{
							uint32_t val = config->Parameter1[i + 1];
							if (val > 0)
							{
								if (config->n_Parameter2 < val)
								{
									return false;
								}

								return jump_to_another_script(raid, config->Parameter2[val - 1], script_data);
							}

							return true;
						}
					}
				} while(0);
			}
			return false;
			case SCRIPT_EVENT_WAIT_MONST_HP:
			{
				do
				{
					if(config->n_Parameter1 < 2)
					{	
						break;
					}	
					bool flag = true;
					for(size_t i = 0; i+1 < config->n_Parameter1; i = i+2)
					{
						uint32_t monster_id = config->Parameter1[i];
						uint32_t percent_hp = config->Parameter1[i+1];		
						for(std::set<monster_struct*>::iterator target_monster = raid->m_monster.begin(); target_monster != raid->m_monster.end(); target_monster++)
						{
							monster_struct* cause_monster = *target_monster;
							if(monster_id != cause_monster->data->monster_id)
							{
								continue;
							}

							if(!do_check_script_raid_monster_hp(percent_hp,cause_monster))
							{
								flag = false;
							}
						}
					}
					return flag;
					
				}while(0);
			}
			return false;
			case SCRIPT_EVENT_AUTOMATIC_NPC_TALK:  //发送自动npc对话，并且等待对话完毕
			{
				if(raid != NULL && raid->data != NULL)
				{
					if(raid->data->raid_ai_event == SCRIPT_EVENT_AUTOMATIC_NPC_TALK)
					{
						raid->data->raid_ai_event = 0;
						return true;
					}
				}
			}
			return false;
		default:
			return false;
	}
	return false;
}

static void script_event_raid_stop(raid_struct *raid, struct raid_script_data *script_data)
{
	raid->clear_monster();
	if (!raid->m_player[0])
	{
		LOG_ERR("%s: raid[%p][%lu] can not find m_player[0] player id [%lu]",
			__FUNCTION__, raid, raid->data->uuid, raid->data->player_info[0].player_id);
		return;
	}

	raid->player_leave_raid(raid->m_player[0]);
}

static bool script_raid_init_cur_cond(raid_struct *raid, struct raid_script_data *script_data)
{
	assert(script_data->cur_index < script_data->script_config->size());
	struct RaidScriptTable *config = (*script_data->script_config)[script_data->cur_index];
	LOG_INFO("%s: raid[%u][%lu] config id[%lu] index[%u]", __FUNCTION__, raid->data->ID, raid->data->uuid, config->TypeID, script_data->cur_index);
	switch (config->TypeID)
	{
		case SCRIPT_EVENT_CREATE_MONSTER_NUM: //刷新配置表内指定怪物
			for (size_t i = 0; i + 1 < config->n_Parameter1; i = i+2)
				monster_manager::create_monster_by_id(raid, config->Parameter1[i], config->Parameter1[i + 1]);
			return true;
		case SCRIPT_EVENT_CREATE_MONSTER_ALL: //刷新配置表内所有指定怪物
			for (size_t i = 0; i < config->n_Parameter1; ++i)
				monster_manager::create_monster_by_id(raid, config->Parameter1[i], 9999);			
			return true;
		case SCRIPT_EVENT_CREATE_COLLECT_NUM: //刷新配置表内指定采集物
			for (size_t i = 0; i + 1 < config->n_Parameter1; i = i+2)
				Collect::CreateCollectByID(raid, config->Parameter1[i], config->Parameter1[i + 1]);
			return true;
		case SCRIPT_EVENT_CREATE_COLLECT_ALL: //刷新配置表内所有指定采集物
			for (size_t i = 0; i < config->n_Parameter1; ++i)
				Collect::CreateCollectByID(raid, config->Parameter1[i], 9999);
			return true;
		case SCRIPT_EVENT_TRANSFER_PLAYER_POSITION:
			{
				if (config->n_Parameter1 < 4)
				{
					return false;
				}

				double pos_x = config->Parameter1[0];
				double pos_y = config->Parameter1[1];
				double pos_z = config->Parameter1[2];
				double direct = config->Parameter1[3];
				for (int i = 0; i < MAX_TEAM_MEM; ++i)
				{
					player_struct *player = raid->m_player[i];
					if (!player)
					{
						continue;
					}

					player->send_clear_sight();
					raid->delete_player_from_scene(player);
					player->set_pos(pos_x, pos_z);
					raid->add_player_to_scene(player);
					player->data->m_angle = unity_angle_to_c_angle(direct);
					player->send_scene_transfer(direct, pos_x, pos_y, pos_z, raid->m_id, 0);
				}
			}
			return true;
		case SCRIPT_EVENT_FINISH_RAID:
			{
				raid->on_raid_finished();
			}
			return true;
		case SCRIPT_EVENT_MONSTER_13_TYPE:
			{
				if (config->n_Parameter1 < 1)
				{
					return false;
				}

				monster_struct *target_monster = NULL;
				for (std::set<monster_struct*>::iterator iter = raid->m_monster.begin(); iter != raid->m_monster.end(); ++iter)
				{
					monster_struct *monster = *iter;
					if (monster->ai_type != 13)
					{
						continue;
					}

					target_monster = monster;
					break;
				}

				if (!target_monster)
				{
					return false;
				}

				set_leixinye_type(target_monster, config->Parameter1[0]);
			}
			return true;
		case SCRIPT_EVENT_START_ESCORT:
			{
				if (config->n_Parameter1 < 1)
				{
					return false;
				}
				player_struct *player = get_script_raid_event_player(raid);
				if (!player)
				{
					return false;
				}

				player->start_escort(config->Parameter1[0]);
			}
			return true;
		case SCRIPT_EVENT_START_MONSTER_AI:
			{
				raid->start_monster_ai();
			}
			return true;
		case SCRIPT_EVENT_STOP_MONSTER_AI:
			{
				raid->stop_monster_ai();
				assert(config->n_Parameter1 >= 1);
				script_data->cur_finished_num[0] = time_helper::get_cached_time() / 1000 + config->Parameter1[0];
			}
			return false;
		case SCRIPT_EVENT_CREATE_NPC_NUM: //刷新配置表内指定NPC
		case SCRIPT_EVENT_CREATE_NPC_ALL: //刷新配置表内所有指定NPC
		case SCRIPT_EVENT_CREATE_TRANSFER_NUM: //刷新配置表内指定传送点
		case SCRIPT_EVENT_CREATE_TRANSFER_ALL: //刷新配置表内所有指定传送点
		case SCRIPT_EVENT_CREATE_AIR_WALL: //产生空气墙
		case SCRIPT_EVENT_REMOVE_AIR_WALL: //删除空气墙
		case SCRIPT_EVENT_REMOVE_NPC:
		case SCRIPT_EVENT_PLAY_NPC_ACTION:
		case SCRIPT_EVENT_PLAY_ANIMATION:
		case SCRIPT_EVENT_START_GONGCHENGCHUI:
		case SCRIPT_EVENT_PLAY_EFFECT:
		{
			RaidEventNotify nty;
			raid_event_notify__init(&nty);
			nty.type = config->TypeID;
			nty.param1 = config->Parameter1;
			nty.n_param1 = config->n_Parameter1;
			nty.param2 = config->Parameter2;
			nty.n_param2 = config->n_Parameter2;
			raid->broadcast_to_raid(MSG_ID_RAID_EVENT_NOTIFY, &nty, (pack_func)raid_event_notify__pack);
			return true;
		}
		case SCRIPT_EVENT_RAID_STOP:
		{
			script_event_raid_stop(raid, script_data);
			return false;
		}
		case SCRIPT_EVENT_SET_PK_MODE:
		{
			assert(config->n_Parameter1 >= 1);
			player_struct *player = get_script_raid_event_player(raid);
			if (player)
			{
				player->set_attr(PLAYER_ATTR_PK_TYPE, config->Parameter1[0]);
				player->broadcast_one_attr_changed(PLAYER_ATTR_PK_TYPE, config->Parameter1[0], false, true);
			}
			return true;
		}
		case SCRIPT_EVENT_SET_CAMP:
		{
			assert(config->n_Parameter1 >= 1);			
			player_struct *player = get_script_raid_event_player(raid);
			if (player)
			{
				player->set_attr(PLAYER_ATTR_ZHENYING, config->Parameter1[0]);
				player->broadcast_one_attr_changed(PLAYER_ATTR_ZHENYING, config->Parameter1[0], false, true);
				if (config->Parameter1[0] != 0)
				{
					player->m_skill.OpenAllSkill();
					player->m_skill.SendAllSkill();
				}
				player->send_zhenying_info();
			}
			return true;
		}
		case SCRIPT_EVENT_ADD_BUFF:
		{
			assert(config->n_Parameter1 >= 1);			
			player_struct *player = get_script_raid_event_player(raid);
			if (player)
			{
				buff_manager::create_buff(config->Parameter1[0], player, player, true);
			}
			return true; 
		}
		case SCRIPT_EVENT_DEL_BUFF:
		{
			assert(config->n_Parameter1 >= 1);			
			player_struct *player = get_script_raid_event_player(raid);
			if (player)
			{
				player->clear_one_buff(config->Parameter1[0]);								
			}
			return true; 
		}		
		case SCRIPT_EVENT_SET_NOVICERAID_FLAG:
		{
			assert(config->n_Parameter1 >= 1);			
			player_struct *player = get_script_raid_event_player(raid);
			if (player)
			{
				player->data->noviceraid_flag = config->Parameter1[0];

				player->m_skill.clear(); //重新初始化技能
				player->m_skill.SendAllSkill();
				player->m_skill.Init();
				player->m_skill.SendAllSkill();
			}
			return true;
		}
		case SCRIPT_EVENT_AUTOMATIC_NPC_TALK:  //发送自动npc对话，并且等待对话完毕
		{
			RaidEventNotify nty;
			raid_event_notify__init(&nty);
			nty.type = config->TypeID;
			nty.param1 = config->Parameter1;
			nty.n_param1 = config->n_Parameter1;
			nty.param2 = config->Parameter2;
			nty.n_param2 = config->n_Parameter2;
			raid->broadcast_to_raid(MSG_ID_RAID_EVENT_NOTIFY, &nty, (pack_func)raid_event_notify__pack);
		}
		case SCRIPT_EVENT_MONSTER_DEAD_ALL: //所有指定怪物死亡 等同于检测副本内还存活指定怪物数量小于某值
		case SCRIPT_EVENT_ALIVE_MONSTER_EQUEL: //检测副本内还存活指定怪物数量等于某值
		case SCRIPT_EVENT_ALIVE_MONSTER_LESSER: //检测副本内还存活指定怪物数量小于某值
		case SCRIPT_EVENT_ALIVE_MONSTER_GREATER: //检测副本内还存活指定怪物数量大于某值  这个有啥用？
		case SCRIPT_EVENT_ALIVE_COLLECT_EQUEL: //检测副本内还存活指定采集物数量等于某值
		case SCRIPT_EVENT_ACCEPT_TASK:
		case SCRIPT_EVENT_MONSTER_ARRIVE_POSITION:
		case SCRIPT_EVENT_PLAYER_ARRIVE_POSITION:
		case SCRIPT_EVENT_WAIT_MONST_HP:
			return script_raid_check_finished(raid, script_data);
		case SCRIPT_EVENT_TIME_OUT: //副本计时
			assert(config->n_Parameter1 >= 1);
			script_data->cur_finished_num[0] = time_helper::get_cached_time() / 1000 + config->Parameter1[0];
		case SCRIPT_EVENT_MONSTER_DEAD_NUM: //指定怪物死亡
		case SCRIPT_EVENT_COLLECT_NUM: //采集指定采集物
		case SCRIPT_EVENT_PLAYER_TASK_FORK:
		case SCRIPT_EVENT_ESCORT_RESULT:
		case SCRIPT_EVENT_WAIT_NPC_TALK:
			return false;
		case SCRIPT_EVENT_COLLECT_CALLBACK:
		{
			script_data->collect_callback_event = config->Parameter1[0];
			return true;
		}
		case SCRIPT_EVENT_ADD_BUFF_TO_MONSTER:
		{
			assert(config->n_Parameter1 >=1 && config->n_Parameter1%2 == 0);
			for(size_t i = 0; i < config->n_Parameter1; i = i+2)	
			{
				uint32_t monster_id = config->Parameter1[i];
				uint32_t buff_id = config->Parameter1[i+1];
				for(std::set<monster_struct*>::iterator itr = raid->m_monster.begin(); itr != raid->m_monster.end(); itr++)
				{
					monster_struct* monster = *itr;
					if(monster_id != monster->data->monster_id)
					{
						continue;
					}

					buff_manager::create_buff(buff_id, monster, monster, true);
				}
			}

		return true;
		}
		case SCRIPT_EVENT_DELETE_MONSTER:
		{
			assert(config->n_Parameter1 >=1);
			for(size_t j = 0; j < config->n_Parameter1; j++)
			{
				uint32_t monster_id = config->Parameter1[j];
				for(std::set<monster_struct*>::iterator itr = raid->m_monster.begin(); itr != raid->m_monster.end();)
				{
					std::set<monster_struct *>::iterator next_itr = itr;
					++next_itr;
					if ((*itr)->data->monster_id == monster_id)
					{
						raid->delete_monster_from_scene((*itr), true);
						monster_manager::delete_monster(*itr);
					}
					itr = next_itr;
				}

			}
		return true;
		}
		default:
			return false;
	}
	return false;
}

static void script_raid_next(raid_struct *raid, struct raid_script_data *script_data);
void do_script_raid_init_cond(raid_struct *raid, struct raid_script_data *script_data)
{
	LOG_INFO("%s: raid[%p][%lu][%lu] cur_index = %d", __FUNCTION__, raid, raid->m_config->DungeonID, raid->data->uuid, script_data->cur_index);
	if (script_data->cur_index >= script_data->script_config->size())
	{
			// TODO: 副本完成
		return;
	}

	for (int i = 0; i < MAX_SCRIPT_COND_NUM; ++i)
		script_data->cur_finished_num[i] = 0;

	if (script_raid_init_cur_cond(raid, script_data))
		script_raid_next(raid, script_data);
}

static void script_raid_next(raid_struct *raid, struct raid_script_data *script_data)
{
	script_data->cur_index++;
	do_script_raid_init_cond(raid, script_data);
}

void script_ai_common_npc_talk(raid_struct *raid, uint32_t npc_id, struct raid_script_data *script_data)
{
	LOG_DEBUG("%s: npc id[%u] raid[%p][%lu]", __FUNCTION__, npc_id, raid, raid->data->uuid);
	struct RaidScriptTable *config = (*script_data->script_config)[script_data->cur_index];
	if (config->TypeID != SCRIPT_EVENT_WAIT_NPC_TALK)
		return;
	if (config->n_Parameter1 != 1)
	{
		script_raid_next(raid, script_data);		
		return;
	}
	if (npc_id != config->Parameter1[0])
		return;
	script_raid_next(raid, script_data);
	return;
}

void script_ai_common_monster_dead(raid_struct *raid, monster_struct *monster, unit_struct *killer, struct raid_script_data *script_data)
{
	LOG_DEBUG("%s: monster id[%u] raid[%p][%lu]", __FUNCTION__, monster->data->monster_id, raid, raid->data->uuid);
	bool pass = true;
	struct RaidScriptTable *config = (*script_data->script_config)[script_data->cur_index];
	if (config->TypeID != SCRIPT_EVENT_MONSTER_DEAD_NUM)
		return;
	for (size_t i = 0; i + 1 < config->n_Parameter1; i = i+2)
	{
		if (config->Parameter1[i] == monster->data->monster_id)
			++script_data->cur_finished_num[i / 2];
		if (script_data->cur_finished_num[i / 2] < config->Parameter1[i + 1])
			pass = false;
	}

	if (pass)
		script_raid_next(raid, script_data);
}

extern void stop_leiminggu_skill(monster_struct *monster);
void script_ai_common_collect(raid_struct *raid, player_struct *player, Collect *collect, struct raid_script_data *script_data)
{
	switch (script_data->collect_callback_event)
	{
		case 1://打断雷鸣鼓
		{
			for (std::set<monster_struct *>::iterator ite = raid->m_monster.begin(); ite != raid->m_monster.end(); ++ite)
			{
				if ((*ite)->ai_type == 13)
				{
					stop_leiminggu_skill(*ite);					
					break;
				}
			}
			break;
		}
		default:
			break;
	}
	
	bool pass = true;
	struct RaidScriptTable *config = (*script_data->script_config)[script_data->cur_index];
	if (config->TypeID != SCRIPT_EVENT_COLLECT_NUM)
		return;
	for (size_t i = 0; i < config->n_Parameter1; ++i)
	{
		if (config->Parameter1[i] == collect->m_collectId)
			++script_data->cur_finished_num[i / 2];
		if (script_data->cur_finished_num[i / 2] < config->Parameter1[i + 1])
			pass = false;
	}

	if (pass)
		script_raid_next(raid, script_data);
}

void script_ai_common_tick(raid_struct *raid, struct raid_script_data *script_data)
{
	if (script_data->cur_index >= script_data->script_config->size())
	{
			// TODO: 副本完成
		return;
	}
	
	if (script_raid_check_finished(raid, script_data))
		script_raid_next(raid, script_data);

	if (raid->m_config->DengeonRank == DUNGEON_TYPE_SCRIPT)
		normal_raid_ai_tick(raid);
}

void script_ai_common_player_ready(raid_struct *raid, player_struct *player, struct raid_script_data *script_data)
{
	for (int i = 0; i < script_data->cur_index; ++i)
	{
		struct RaidScriptTable *config = (*script_data->script_config)[i];
		if (!config)
		{
			continue;
		}

		switch (config->TypeID)
		{
			case SCRIPT_EVENT_CREATE_NPC_NUM: //刷新配置表内指定NPC
			case SCRIPT_EVENT_CREATE_NPC_ALL: //刷新配置表内所有指定NPC
			case SCRIPT_EVENT_CREATE_TRANSFER_NUM: //刷新配置表内指定传送点
			case SCRIPT_EVENT_CREATE_TRANSFER_ALL: //刷新配置表内所有指定传送点
			case SCRIPT_EVENT_CREATE_AIR_WALL: //产生空气墙
			case SCRIPT_EVENT_REMOVE_AIR_WALL: //删除空气墙
			case SCRIPT_EVENT_REMOVE_NPC:
			case SCRIPT_EVENT_PLAY_NPC_ACTION:
			case SCRIPT_EVENT_PLAY_ANIMATION:
			case SCRIPT_EVENT_AUTOMATIC_NPC_TALK:
			case SCRIPT_EVENT_PLAY_EFFECT:
				//case SCRIPT_EVENT_START_GONGCHENGCHUI:
				{
					RaidEventNotify nty;
					raid_event_notify__init(&nty);
					nty.type = config->TypeID;
					nty.param1 = config->Parameter1;
					nty.n_param1 = config->n_Parameter1;
					nty.param2 = config->Parameter2;
					nty.n_param2 = config->n_Parameter2;

					EXTERN_DATA ext_data;
					ext_data.player_id = player->get_uuid();

					fast_send_msg(&conn_node_gamesrv::connecter, &ext_data, MSG_ID_RAID_EVENT_NOTIFY, raid_event_notify__pack, nty);
				}
				break;
		}
	}

	//单人副本在玩家进入副本后初始化
	if (raid->m_config->DengeonType == 2)
	{
		do_script_raid_init_cond(raid, script_data);
	}
}

void script_ai_common_escort_stop(raid_struct *raid, player_struct *player, uint32_t escort_id, bool success, struct raid_script_data *script_data)
{
	struct RaidScriptTable *config = (*script_data->script_config)[script_data->cur_index];
	if (config->TypeID != SCRIPT_EVENT_ESCORT_RESULT)
		return;
	if (get_script_raid_event_player(raid) != player)
		return;

	if (config->n_Parameter1 < 3)
	{
		return;
	}
	if ((uint32_t)config->Parameter1[0] != escort_id)
	{
		return;
	}

	int val = -1;
	if (success)
	{
		val = config->Parameter1[1];
	}
	else
	{
		val = config->Parameter1[2];
	}

	if (val == 0)
	{
		script_raid_next(raid, script_data);
	}
	else
	{
		if (config->n_Parameter2 < (uint32_t)val)
		{
			return ;
		}

		if (jump_to_another_script(raid, config->Parameter2[val -1], script_data))
		{
			script_raid_next(raid, script_data);
		}
	}
}

//计算某个怪物的血量是否低于某百分比
static bool do_check_script_raid_monster_hp(double percent, monster_struct* monster)
{
	if(NULL == monster)
	{
		return false;
	}

	double cur_percent = monster->data->attrData[PLAYER_ATTR_HP]/monster->data->attrData[PLAYER_ATTR_MAXHP];
	cur_percent *= 100;
	
	if(cur_percent < percent)
	{
		return true;
	}
	return false;
}
