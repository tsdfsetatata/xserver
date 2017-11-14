#ifndef __GUILD_CONFIG_H__
#define __GUILD_CONFIG_H__

#include <map>
#include <vector>
#include <stdint.h>
#include "excel_data.h"
#include "lua_template.h"

extern uint32_t sg_guild_create_level;
extern uint32_t sg_guild_create_gold;
extern uint32_t sg_guild_rename_item_id;
extern uint32_t sg_guild_rename_item_num;
extern uint32_t sg_guild_join_cd;
extern uint32_t sg_guild_apply_join_cd;
extern uint32_t sg_guild_rename_cd;
extern char* sg_guild_recruit_notice;
extern char* sg_guild_announcement;
extern std::vector<uint32_t> sg_guild_question;

extern std::map<uint64_t, struct QuestionTable*> questions_config; //考题表
extern std::map<uint64_t, struct ParameterTable *> parameter_config;
extern std::map<uint64_t, struct GangsTable*> guild_building_config; //帮会建筑表
extern std::map<uint64_t, struct GangsJurisdictionTable*> guild_office_config; //帮会职权表
extern std::map<uint64_t, struct GangsSkillTable*> guild_skill_config; //帮会技能表
extern std::map<uint64_t, struct ShopTable*> shop_config; //商品配置
extern std::map<uint64_t, struct GangsDungeonTable*> guild_battle_reward_config; //帮会战奖励表
extern std::map<uint64_t, struct EventCalendarTable*> activity_config; //活动配置
extern std::map<uint64_t, struct ControlTable*> all_control_config; //副本进入条件收益次数配置
extern std::map<uint64_t, struct ActorLevelTable *> actor_level_config; //角色等级配置
extern std::map<uint64_t, struct FactionActivity *> guild_land_active_config; //帮会领地活动表
extern std::map<uint64_t, struct GangsBuildTaskTable*> guild_build_task_config; //帮会建设任务表

int read_all_excel_data();

GangsTable *get_guild_building_config(uint32_t type, uint32_t level);
GangsSkillTable *get_guild_skill_config(uint32_t type, uint32_t level);
int get_guild_build_task_id(uint32_t player_lv);
int get_guild_build_task_amount(uint32_t id);

#endif /* __GUILD_CONFIG_H__ */
