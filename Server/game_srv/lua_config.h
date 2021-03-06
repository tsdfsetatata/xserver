#ifndef __LUA_CONFIG_H__
#define __LUA_CONFIG_H__

#include <map>
#include <vector>
#include <stdint.h>
#include <set>
#include "excel_data.h"
#include "lua_template.h"
#include "attr_id.h"
#include "comm_define.h"

//一个关卡的万妖卡最多条件参数 
#define MAX_WANYAOKA_COND_PARAM 8  

extern std::map<uint64_t, struct ActiveSkillTable *> active_skill_config;
extern std::map<uint64_t, struct SkillMoveTable *> move_skill_config;
extern std::map<uint64_t, struct PassiveSkillTable *> passive_skill_config;
extern std::map<uint64_t, struct MonsterTable *> monster_config;
extern std::map<uint64_t, struct ActorTable *> actor_config;
extern std::map<uint64_t, struct ActorAttributeTable *> actor_attribute_config;
extern std::map<uint64_t, struct region_config *> scene_region_config;  //区域数据
extern std::map<uint64_t, struct SceneResTable *> scene_res_config; //阻挡，寻路数据
extern std::map<uint64_t, struct map_config *> scene_map_config;
extern std::map<uint64_t, std::vector<struct SceneCreateMonsterTable *> *> all_scene_create_monster_config;
extern std::map<uint64_t, struct SkillTable *> skill_config;
extern std::map<uint64_t, struct SkillTable *> fuwen_config;
extern std::map<uint64_t, struct SkillLvTable *> skill_lv_config;
extern std::map<uint64_t, struct FlySkillTable *> fly_skill_config;
extern std::map<uint64_t, struct BuffTable *> buff_config;
extern std::map<uint64_t, struct SkillEffectTable *> skill_effect_config;
extern std::map<uint64_t, struct ActorLevelTable *> actor_level_config; //角色等级配置
extern std::map<uint64_t, struct ItemsConfigTable *> item_config; //道具配置
extern std::map<uint64_t, struct ParameterTable *> parameter_config; //参数配置
extern std::map<uint64_t, struct DropConfigTable *> drop_config; //掉落配置
extern std::map<uint64_t, struct BaseAITable *> base_ai_config;
extern std::map<uint64_t, struct ActorHeadTable *> actor_head_config; //头像配置
extern std::map<uint64_t, struct CollectTable *> collect_config;
extern std::map<uint64_t, struct TaskTable *> task_config; //任务配置
extern std::map<uint64_t, struct TaskConditionTable *> task_condition_config; //任务条件配置
extern std::map<uint64_t, struct TaskEventTable *> task_event_config; //任务事件配置
extern std::map<uint64_t, struct TaskDropTable *> task_drop_config; //任务掉落配置
extern std::map<uint64_t, struct TaskRewardTable *> task_reward_config; //任务奖励配置
extern std::map<uint64_t, struct TaskMonsterTable *> task_monster_config; //任务刷怪配置
extern std::map<uint64_t, struct TaskChapterTable *> task_chapter_config; //任务章节配置
extern std::map<uint64_t, struct ActorFashionTable *> fashion_config; //时装配置
extern std::map<uint64_t, struct CharmTable *> charm_config; //时装配置
extern std::map<uint64_t, struct DungeonTable*> all_raid_config; //副本配置
extern std::map<uint64_t, struct ControlTable*> all_control_config; //副本进入条件收益次数配置
extern std::map<uint64_t, struct EquipmentTable*> equipment_config; //装备配置
extern std::map<uint64_t, struct EquipStarLv*> equip_star_config; //装备升星配置
extern std::map<uint64_t, struct EquipLock*> equip_lock_config; //装备镶嵌配置
extern std::map<uint64_t, struct EquipAttribute*> equip_attr_config; //装备随机属性配置
extern std::map<uint64_t, struct GemAttribute*> equip_gem_config; //装备宝石配置
extern std::map<uint64_t, struct AttributeTypeTable*> attribute_type_config; //属性类型配置
extern std::map<uint64_t, struct ColourTable*> color_table_config; //
extern std::map<uint64_t, struct ShopListTable*> shop_list_config; //商城页签配置
extern std::map<uint64_t, struct ShopTable*> shop_config; //商品配置
extern std::map<uint64_t, struct TransferPointTable*> transfer_config; //传送配置
extern std::map<uint64_t, struct SpiritTable*> spirit_config; //坐骑皮肤配置
extern std::map<uint64_t, struct MountsTable*> horse_config; //坐骑皮肤配置
extern std::map<uint64_t, struct CastSpiritTable*> horse_soul_config; //坐骑铸灵配置
extern std::map<uint64_t, struct RandomCardTable*> wanyaoka_config; //万妖卡配置
extern std::map<uint64_t, struct RandomCardRewardTable*> wanyaoka_reward_config; //万妖卡奖励配置
extern std::map<uint64_t, struct PulseTable*> yuqidao_jingmai_config; //御气道经脉配置
extern std::map<uint64_t, struct AcupunctureTable*> yuqidao_acupoint_config; //御气道穴位配置
extern std::map<uint64_t, struct BreakTable*> yuqidao_break_config; //御气道冲穴配置
extern std::map<uint64_t, struct StageTable*> pvp_raid_config; //pvp副本配置
extern std::map<uint64_t, struct BaguaTable*> bagua_config; //八卦牌配置
extern std::map<uint64_t, struct BaguaStarTable*> bagua_star_config; //八卦牌炼星配置
extern std::map<uint64_t, struct BaguaViceAttributeTable*> bagua_vice_attr_config; //八卦牌洗炼配置
extern std::map<uint64_t, struct BaguaSuitTable*> bagua_suit_config; //八卦牌套装配置
extern std::vector<struct BootNameTable*> rand_name_config; //随机名字
#define ROBOT_CONFIG_TYPE_SIZE 5
extern std::vector<struct ActorRobotTable*> robot_config[ROBOT_CONFIG_TYPE_SIZE]; //机器人
extern std::map<uint64_t, struct RandomMonsterTable*> random_monster; //
extern std::map<uint64_t, struct SpecialtyLevelTable*> specialty_level_config; //妖师专精
extern std::map<uint64_t, struct TypeLevelTable*> guoyu_level_config; //国御难度
extern std::map<uint64_t, struct ChangeSpecialty*> change_special_config; //改专精费用 
extern std::map<uint64_t, struct RandomDungeonTable*> random_guoyu_dungenon_config; //国御随机副本
extern std::map<uint64_t, struct RewardTable*> chengjie_reward_config; //惩戒奖励
extern std::map<uint64_t, struct SpecialTitleTable*> yaoshi_title_config; //妖师专精称号
extern std::map<uint64_t, struct MoneyQuestTable*> shangjin_task_config; //妖师赏金任务
extern std::map<uint64_t, struct SpecialtySkillTable*> yaoshi_skill_config; //妖师技能
extern std::map<uint64_t, struct EventCalendarTable*> activity_config; //活动配置
extern std::map<uint64_t, struct ActiveTable*> activity_activeness_config; //活动活跃度配置
extern std::map<uint64_t, struct ChivalrousTable*> activity_chivalry_config; //侠义活动配置
extern std::map<uint64_t, struct GangsTable*> guild_building_config; //帮会建筑表
extern std::map<uint64_t, struct GangsJurisdictionTable*> guild_office_config; //帮会职权表
extern std::map<uint64_t, struct GangsSkillTable*> guild_skill_config; //帮会技能表
extern std::map<uint64_t, struct GangsBuildTaskTable*> guild_build_task_config; //帮会建设任务表
extern std::map<uint64_t, struct CampTable*> zhenying_base_config; //阵营基础信息表
extern std::map<uint64_t, struct GradeTable*> zhenying_level_config; //阵营等级表
extern std::map<uint64_t, struct WeekTable*> zhenying_week_config; //阵营周目标表
extern std::map<uint64_t, struct BattlefieldTable*> zhenying_fight_config; //阵营战表
extern std::map<uint64_t, struct BattleFieldRank*> zhenying_fight_rank_config; //阵营战表
extern std::map<char *, std::vector<struct RaidScriptTable*> *> all_raid_script_config; //副本AI配置表
extern std::map<uint64_t, struct QuestionTable*> questions_config; //考题表
extern std::vector<struct RobotPatrolTable*> robot_patrol_config; //机器人巡逻
extern std::vector<struct RobotPatrolTable*> robot_zhenyingzhan_config; //阵营战机器人移动路径
extern std::vector<struct FactionBattleTable*> zhenying_battle_config; //战场
extern std::map<uint64_t, struct LifeSkillTable*> medicine_config; //炼药表
extern std::map<uint64_t, struct NoticeTable*> notify_config; //公告表
extern std::map<uint64_t, struct SearchTable*> xunbao_config; //寻宝表
extern std::map<uint64_t, struct TreasureTable*> xunbao_map_config; //寻宝地图表
extern std::map<uint64_t, struct EscortTask*> escort_config; //护送表
extern std::map<uint64_t, struct PartnerTable*> partner_config; //伙伴表
extern std::map<uint64_t, struct PartnerSkillTable*> partner_rand_skill_config; //伙伴随机技能表
extern std::map<uint64_t, struct GodYaoAttributeTable*> partner_god_attr_config; //伙伴表神耀属性
extern std::map<uint64_t, struct RecruitTable*> partner_recruit_config; //伙伴招募表
extern std::map<uint64_t, struct PartnerLevelTable*> partner_level_config; //伙伴等级表
extern std::map<uint64_t, struct SkillLevelTable*> partner_skill_level_config; //伙伴技能等级表
extern std::map<uint64_t, struct FetterTable*> partner_bond_config; //伙伴羁绊表
extern std::map<uint64_t, struct BiaocheTable*> cash_truck_config; //镖车表
extern std::map<uint64_t, struct FunctionUnlockTable*> function_unlock_config; //功能开启表
extern std::map<uint64_t, struct NpcTalkTable*> monster_talk_config; //怪物冒泡说话
extern std::map<uint64_t, struct MonsterPkTypeTable*> pk_type_config; //pk type 设置
extern std::map<uint64_t, struct BiaocheRewardTable*> cash_truck_reward_config; //
extern std::map<uint64_t, struct WeaponsEffectTable*> weapon_color_config; //
extern std::map<uint64_t, struct LifeMagicTable*> lifemagic_config; //伙伴法宝配置表
extern std::map<uint64_t, struct MagicTable*> MagicTable_config;   //伙伴法宝属性主表
extern std::map<uint64_t, struct MagicAttributeTable*> MagicAttrbute_config;   //伙伴法宝副属性表
extern std::map<uint64_t, struct GenerateMonster*> GenerateMonster_config;   //定时刷怪配置
//extern std::map<uint64_t, std::vector<struct LifeProbabilitytable *> *> LifeProbabi_config; //法宝命体对应表
extern std::map<uint64_t, struct ServerResTable*> server_res_config; //服务器资源配置
extern std::map<uint64_t, struct ServerLevelTable*> server_level_config; //服务器等级配置
extern std::map<uint64_t, struct AchievementFunctionTable*> achievement_function_config; //成就功能点表
extern std::map<uint64_t, struct AchievementHierarchyTable*> achievement_hierarchy_config; //成就层级表
extern std::map<uint64_t, struct DegreeTable*> friend_close_config; //好友亲密度表
extern std::map<uint64_t, struct ArenaRewardTable*> doufachang_reward_config; //斗法场奖励
extern std::map<uint64_t, struct TitleFunctionTable*> title_function_config; //称号功能点表
extern std::map<uint64_t, struct WorldBossTable*> world_boss_config; //世界boss表
extern std::map<uint64_t, struct WorldBossTable*> monster_to_world_boss_config; //世界boss表(以怪物id为索引)
extern std::map<uint64_t, struct ChallengeTable*> hero_challenge_config; //英雄挑战
extern std::map<uint64_t, struct ChallengeTable*> raidid_to_hero_challenge_config; //英雄挑战(以副本id为索引)
extern std::map<uint64_t, struct UndergroundTask*> mijing_xiulian_config; //地宫修炼任务信息
extern std::map<uint64_t, struct CampDefenseTable*> zhenying_daily_config; //日常阵营战
extern std::map<uint64_t, struct FishingTable*> fishing_config; //钓鱼表
extern std::map<uint64_t, struct GrowupTable*> strong_config; //变强表
extern std::map<uint64_t, struct FactionActivity*> guild_activ_config; //帮会活动表
extern std::map<char *, std::vector<struct SceneCreateMonsterTable*> *> all_raid_ai_monster_config; //副本AI表里面配置的怪物刷新表
extern std::set<char* > some_monster_config_name; //有些场景会配置多个ai进程表，这里集合表名 
extern std::map<uint64_t, struct TradingTable*> trade_item_config; //交易物品表
extern std::map<uint64_t, struct AuctionTable*> auction_config; //拍卖品表
extern std::map<uint64_t, struct MGLYdiaoxiangTable*> maogui_diaoxiang_config; //猫鬼乐园雕像表
extern std::map<uint64_t, struct MGLYmaoguiTable*> maogui_monster_config; //猫鬼乐园相关猫鬼怪物表
extern std::map<uint64_t, struct MGLYyanseTable*> maogui_colour_config; //猫鬼乐园猫鬼颜色配置表
extern std::map<uint64_t, struct MGLYyanseTable*> maogui_diaoxiang_colour_config; //猫鬼乐园猫鬼雕像颜色配置表
extern std::map<uint64_t, struct MGLYyanseTable*> maogui_zhengning_colour_config; //猫鬼乐园狰狞猫鬼颜色配置表
extern std::map<uint64_t, struct MGLYyanseTable*> maogui_shouling_colour_config;  //猫鬼乐园鬼王召唤的首领怪颜色配置表
extern std::map<uint64_t, struct MGLYyanseTable*> maogui_xiaoguai_colour_config;  //猫鬼乐园鬼王召唤的小怪颜色配置表
extern std::map<uint64_t, struct MGLYmaoguiwangTable*> maogui_maogui_wang_config; //猫鬼乐园猫鬼王配置表
extern std::map<uint64_t, struct MGLYshoulingTable*> maogui_shouling_to_xiaoguai_config; //猫鬼乐园猫首领死亡删除对应小怪
extern std::map<uint64_t, struct MonsterIDTable*> raid_jincheng_suiji_kill_monster; //副本进程击杀id对应此表的随机一只怪物，进程加一
extern std::vector<struct raidsrv_config*> vec_raidsrv_config;
extern std::map<uint32_t, uint32_t> map_raidsrv_config;
extern std::map<uint64_t, struct TravelTable*> travel_config; //游历任务表
extern std::map<uint64_t, struct LevelReward*> level_reward_config; //等级奖励表
extern std::map<uint64_t, struct TimeReward*> online_reward_config; //在线奖励表
extern std::map<uint64_t, struct OnlineTimes*> online_time_config; //在线奖励时间表
extern std::map<uint64_t, struct SignDay*> sign_day_config; //每日签到奖励表
extern std::map<uint64_t, struct SignMonth*> sign_month_config; //每日签到累计奖励表
extern std::map<uint64_t, std::map<uint64_t, struct SignDay*> >sign_day_zhuan_config; //每日签到奖励表转表
extern std::map<uint64_t, struct SignDay*> sign_day_to_zhuan_config[MAX_ONE_YEARS_THE_MONTH]; //12个月
extern std::map<uint64_t, std::map<uint64_t, struct SignMonth*> >sign_month_zhuan_config; //每日签到累计奖励表转表
extern std::map<uint64_t, struct SignMonth*> sign_month_to_zhuan_config[MAX_ONE_YEARS_THE_MONTH]; //12个月
extern std::map<uint64_t, struct RewardBack*> reward_back_config; //活动奖励找回表
extern std::map<uint64_t, struct P20076Table*> tower_level_config; //冲塔表
extern std::map<uint64_t, struct LoginGifts*> login_gifts_config; //登录奖励表
extern std::map<uint64_t, struct CiFuTable*> ci_fu_config; //赐福奖励表
extern std::map<uint64_t, struct RandomBox*> random_box_config; //随机宝箱表
extern std::map<uint64_t, struct RandomCollectionTable*> random_collect_config; //随机采集点表
extern std::map<uint64_t, struct SkillTimeTable*> skill_time_config; //技能时间表
extern std::map<uint64_t, struct SyntheticTable*> item_combin_config; //合成表
extern std::map<uint64_t, struct NineEightTable*> jiu_gong_ba_gua_reward_config; //九宫八卦奖励表
extern std::map<uint64_t, struct TaskDungeonsTable*> script_raid_config; //剧情副本表
extern std::map<uint64_t, struct WeddingRing*> propose_ring_config; //求婚戒指表
extern std::map<uint64_t, struct WeddingTable*> wedding_config;	 //婚礼表

//////////////////
extern uint32_t sg_bag_unlock_base_price;
extern uint32_t sg_bag_unlock_incr_factor;
extern uint32_t sg_rename_item_id;
extern uint32_t sg_rename_item_num;
extern std::map<uint64_t, TaskChapterTable*> task_chapter_map;
extern uint32_t sg_first_trunk_task_id; //第一个主线任务id
extern int sg_fight_rand;
extern int sg_relive_free_times;
extern int sg_relive_first_cost;
extern int sg_relive_grow_cost;
extern int sg_relive_max_cost;
extern int sg_gem_strip_coin; //宝石剥离消耗银币
extern int sg_player_level_limit; //角色等级上限
extern int sg_wanyaogu_range;
extern int sg_wanyaogu_reward;
extern int sg_wanyaogu_time_delta;
extern int sg_wanyaogu_time_total;
extern int sg_wanyaogu_start_time;

extern int sg_set_pk_type_cd;
extern int sg_pk_level;
extern int sg_muder_num_max;
extern int sg_muder_add_num;
extern int sg_muder_sub_num;
extern int sg_muder_item_value;
extern int sg_qiecuo_out_range_timeout;
extern int sg_qiecuo_range;
extern uint32_t sg_qiecuo_god_time;  //切磋倒计时

extern float sg_hp_pool_percent;  

extern int sg_raid_keep_time;

extern std::vector<uint32_t> sg_vec_wanyaogu_raid_id;

extern int sg_yuqidao_break_item_id;
extern int sg_yuqidao_break_item_num;
extern double sg_fighting_capacity_coefficient[PLAYER_ATTR_MAX]; //战斗力系数
extern uint32_t sg_fighting_capacity_count_in[PLAYER_ATTR_MAX]; //战斗力计算
extern double sg_fighting_capacity_crt_dmg_init_val;

extern struct ControlTable *sg_pvp_control_config_3;
extern struct ControlTable *sg_pvp_control_config_5;

extern uint32_t  MAX_TOWER_LEVEL;
extern int sg_player_speed;

extern int sg_guild_raid_param1[6];  //队伍1出生点
extern int sg_guild_raid_param2[6];  //队伍2出生点
extern int sg_guild_raid_final_param1[6];  //队伍1出生点
extern int sg_guild_raid_final_param2[6];  //队伍2出生点
extern int sg_guild_raid_final_param3[6];  //队伍3出生点
extern int sg_guild_raid_final_param4[6];  //队伍4出生点

extern int sg_3v3_pvp_raid_param1[6];
extern int sg_3v3_pvp_raid_param2[6];
extern int sg_5v5_pvp_raid_param1[6];
extern int sg_5v5_pvp_raid_param2[6];
extern int sg_3v3_pvp_monster_id[3];
extern int sg_5v5_pvp_monster_id[3];
extern int sg_3v3_pvp_monster_place[13];
extern int sg_5v5_pvp_monster_place[13];
extern int sg_pvp_raid_relive_cd;
extern int sg_pvp_raid_monster_refresh_time;
extern int sg_pvp_raid_monster_first_refresh_time;
extern int sg_pvp_raid_red_region_buff_id[2];
extern int sg_pvp_raid_blue_region_buff_id[2];
extern int sg_pvp_raid_blue_region_buff_rate;

extern int MATCH_LEVEL_DIFF;         //段位范围
extern int TEAM_LEVEL1_DIFF_L;       //对手段位下限
extern int TEAM_LEVEL1_DIFF_R;
extern int TEAM_LEVEL2_DIFF_L;       //对手等级下限
extern int TEAM_LEVEL2_DIFF_R;

extern int sg_muder_punish_point;
extern int sg_muder_punish_base;
extern int sg_muder_punish_inc[2];
extern int sg_muder_punish_max;
extern int sg_muder_cant_set_pktype;
extern int sg_muder_debuff[2];

extern double sg_pvp_raid_win_score_param;
extern double sg_pvp_raid_tie_score_param;
extern double sg_pvp_raid_lose_score_param;
extern double sg_pvp_raid_win_gold_param;
extern double sg_pvp_raid_tie_gold_param;
extern double sg_pvp_raid_lose_gold_param;
extern double sg_pvp_raid_reward_score_3v3_param;
extern double sg_pvp_raid_reward_score_5v5_param;
extern double sg_pvp_raid_kill_param;
extern double sg_pvp_raid_assist_param;
extern double sg_pvp_raid_basic_param;

extern uint32_t sg_hp_pool_max;

extern int sg_3v3_pvp_buff_id[3];
extern int sg_5v5_pvp_buff_id[3];

extern int sg_3v3_pvp_raid_red_buff_param[5];
extern int sg_3v3_pvp_raid_blue_buff_param[5];
extern int sg_5v5_pvp_raid_red_buff_param[5];
extern int sg_5v5_pvp_raid_blue_buff_param[5];

extern uint32_t sg_pvp_raid_buff_relive_time;
extern int sg_pvp_center_buff_id[2];

extern uint32_t sg_guild_raid_final_monster_id[4];  //帮会战决赛4组小怪的ID

extern double sg_leiminggu_pos[4]; //雷鸣鼓坐标
extern double sg_leiminggu_boss_pos[4]; //雷鸣鼓boss坐标
extern uint32_t sg_leiminggu_collect_id; //雷鸣鼓采集物ID

extern uint8_t sg_n_shishen_id;
extern uint32_t *sg_shishen_xiaoguai_id;  //侍神小怪ID
extern uint32_t *sg_shishen_shouling_id;  //侍神首领ID

enum YAOSHI_SKILL
{
	SHANGJIN_ONE = 1,
	SHANGJIN_TWO = 2,
	SHANGJIN_THREE = 3,
	SHANGJIN_FOUR = 4,
	CHENGJIE_FIVE = 5,
	CHENGJIE_SIX = 6,
	CHENGJIE_SEVEN = 7,
	CHENGJIE_EIGHT = 8,
	GUOYU_NINE = 9,
	GUOYU_TEN = 10,
	GUOYU_ELEVEN = 11,
	GUOYU_TWELVE = 12,
	YAOSHI_SKILL_MAX_NUM
};

#define MAX_GUOYU_TASK_TYPE  (4)

extern uint64_t sg_yaoshi_level_limited[YAOSHI_SKILL_MAX_NUM];

extern int sg_pvp_raid_fighting_capacity_range[2];
extern std::map<uint32_t, uint32_t> sg_bagua_bind_item_map; //通过八卦ID找绑定道具ID
extern std::map<uint32_t, uint32_t> sg_partner_item_map; //skill_id, book_id
extern std::map<uint32_t, struct SpecialtySkillTable*> sg_yaoshi_skill_map; //
extern std::map<uint32_t, std::vector<RandomMonsterTable *> > sg_random_monster_map; //
extern std::vector<RandomDungeonTable *> random_guoyu_fb_arr[MAX_GUOYU_TASK_TYPE + 1];
extern std::vector<uint32_t> sg_common_question;
extern std::vector<uint32_t> sg_award_question;
extern std::map<uint64_t, struct SearchTable*> sg_xunbao; //寻宝表
extern std::map<uint64_t, std::vector<uint64_t> > sg_xunbao_map; //寻宝地图
extern std::map<uint64_t, std::vector<uint64_t> > sg_rand_collect; //随机宝箱
extern std::map<uint64_t, uint64_t> sg_show_collect; //任务ID 采集点ID
extern std::map<uint64_t, struct FunctionUnlockTable*> sg_jijiangopen; //即将开启表
extern uint64_t sg_partner_rename_item[2];  //伙伴改名消耗的道具id和数量
extern std::map<uint64_t, struct TaskDungeonsTable*> sg_script_raid_config; //剧情副本表
extern std::map<uint32_t, uint32_t> every_level_all_charm;  //每一级对应的总魅力

struct BattleFieldStepRank
{
	uint64_t LowerLimitRank;
	uint64_t UpperLimitRank;
	uint64_t *Reward;
	uint64_t *Num;
	uint32_t n_Reward;
	uint32_t n_Num;
};
extern std::map<uint32_t, std::vector<BattleFieldStepRank *> > sg_battle_award;

extern uint32_t sg_transfer_out_stuck_cd_time;
extern uint32_t sg_guild_scene_id;
extern uint32_t sg_guild_wait_raid_id; //帮战准备区场景ID
extern uint32_t sg_guild_battle_match_time; //帮战预赛匹配时间
extern uint32_t sg_guild_battle_fight_time; //帮战预赛战斗时间
extern uint32_t sg_guild_battle_settle_time; //帮战预赛结算时间
extern uint32_t sg_guild_battle_round_num; //帮战预赛回合数
extern uint32_t sg_guild_battle_final_match_time; //帮战决赛匹配时间
extern uint32_t sg_guild_battle_final_fight_time; //帮战决赛战斗时间
extern uint32_t sg_guild_battle_final_settle_time; //帮战决赛结算时间
extern uint32_t sg_guild_battle_final_round_num; //帮战决赛回合数
extern uint32_t sg_guild_battle_brave_init; //帮战初始勇武值
extern uint32_t sg_guild_battle_wait_award_interval; //帮战等待区奖励时间间隔
extern std::map<uint32_t, uint32_t> sg_guild_battle_wait_award;
extern int sg_guild_battle_fight_win_reward[4];
extern int sg_guild_battle_fight_lose_reward[4];
extern int sg_guild_battle_fight_draw_reward[4];
extern int sg_guild_battle_fight_auto_win_reward[4];
extern int sg_guild_battle_treasure_factor[3];
extern int sg_guild_battle_final_fight_reward_0[4];
extern int sg_guild_battle_final_fight_reward_1[3];
extern int sg_guild_battle_final_fight_reward_2[3];
extern int sg_guild_battle_final_fight_reward_3[3];
extern int sg_guild_battle_final_fight_reward_4[3];
extern int sg_guild_battle_final_treasure_factor[1];
extern int sg_guild_battle_final_score_factor[4];

extern double sg_partner_assist_percent;
extern uint32_t sg_partner_anger_max;
extern uint32_t sg_partner_relive_time;
extern uint32_t sg_partner_sanshenshi_id;
extern uint32_t sg_partner_sanshenshi_score;
extern uint32_t sg_partner_sanshenshi_coin;
extern uint32_t sg_partner_qiyaoshi_id;
extern uint32_t sg_partner_qiyaoshi_score;
extern uint32_t sg_partner_qiyaoshi_coin;

extern double sg_fight_param_161000274;                //体质转生命系数
extern double sg_fight_param_161000275;                //力量转攻击系数
extern double sg_fight_param_161000276;                //敏捷转闪避系数
extern double sg_fight_param_161000277;                //敏捷转全系抗性系数
extern double sg_fight_param_161000278;                //灵巧转命中系数
extern double sg_fight_param_161000279;                //灵巧转会心几率系数
extern double sg_fight_param_161000280;                //命中等级系数
extern double sg_fight_param_161000281;                //命中基础值
extern double sg_fight_param_161000282;                //闪避等级系数
extern double sg_fight_param_161000283;                //闪避基础值
extern double sg_fight_param_161000284;                //实际命中几率下限
extern double sg_fight_param_161000285;                //会心等级系数
extern double sg_fight_param_161000286;                //会心基础值
extern double sg_fight_param_161000287;                //五行抗性等级系数
extern double sg_fight_param_161000288;                //五行抗性基础值
extern double sg_fight_param_161000289;                //特殊属性基础值
extern double sg_fight_param_161000290;                //buff持续时间保底比例
extern double sg_fight_param_161000291;                //PVP等级系数
extern double sg_fight_param_161000292;                //PVP基础值
extern double sg_fight_param_161000293;                //PVP保底比例
extern double sg_fight_param_161000393;                //伤害公式加入战斗力计算
extern double sg_fight_param_161000394;                //伤害公式加入战斗力计算
extern double sg_fight_param_161000395;                //伤害公式加入战斗力计算
extern double sg_fight_param_161000396;                //伤害公式加入战斗力计算
extern double sg_fight_param_161000503;                //力量转五行攻击系数

extern uint32_t sg_server_level_reward_item_id;
extern uint32_t sg_server_level_reward_item_num;

extern std::map<uint32_t, GangsSkillTable*> skill_config_map;

extern uint32_t sg_doufachang_ai[2];
extern uint32_t sg_doufachang_raid_id;
extern uint32_t sg_doufachang_raid_win_reward[2];
extern uint32_t sg_doufachang_raid_lose_reward[2];

extern uint32_t DEFAULT_HORSE;

extern double sg_exp_turn_zhenqi_percent;

extern SceneCreateMonsterTable sg_zhenying_truck[2];

extern std::map<uint32_t, uint32_t> sg_strong_chapter_map;
extern std::map<uint64_t, GrowupTable*> sg_strong_chapter_reward;

extern const uint64_t max_team_mem_distance;

extern std::map<uint32_t, uint32_t> sg_item_trade_map;
extern double sg_on_shelf_fee_percent;

extern uint32_t sg_guild_ruqin_huodui_monster_id;
extern int sg_guild_ruqin_huodui_fanwei;
extern uint32_t sg_guild_ruqin_huodui_exp;
extern uint32_t sg_guild_ruqin_huodui_jiange;
extern uint32_t sg_guild_ruqin_huodui_chixutime;
extern uint64_t sg_guild_ruqin_renzu_bossid;
extern uint64_t sg_guild_ruqin_yaozu_bossid;

extern uint32_t sg_maogui_diaoxiang_stop_buff;
extern uint32_t sg_maogui_guiwang_wudi_buff;

extern char *sg_xunbao_boss_notice;

extern std::map<uint64_t, EquipAttribute*> sg_attr_seek_map;
extern std::map<uint64_t, std::vector<EquipAttribute*> > sg_attr_pool_map;

extern uint32_t sg_travel_round_amount;
extern uint32_t sg_travel_task_amount;

extern uint32_t sg_ai_srv_port;
extern uint32_t sg_yaoqian_shu_sum_num;   //摇钱树可使用基础次数
extern uint32_t sg_yaoqian_shu_free_num;  //摇钱树的免费次数
extern uint32_t sg_yaoqian_shu_use_gold;  //摇钱树消耗元宝基础数量
extern uint32_t sg_yaoqian_shu_add_gold;  //摇钱树每次增加消耗元宝的数量
extern uint32_t sg_yaoqian_shu_max_gold;  //摇钱树消耗元宝的上限
extern uint32_t sg_yaoqian_shu_add_coin;  //摇钱树获取的银票基础值

extern int AWARD_QUESTION_ACTIVE_ID;
extern int COMMON_QUESTION_ACTIVE_ID;
extern const uint32_t sg_login_reward_function_id;  //登录奖励FunctionUnlockTable表id
extern uint32_t sg_login_reward_chixu_day;  //登录奖励持续天数
extern uint32_t sg_new_battle_point;  //新手阵营战占旗加积分

extern uint32_t sg_strong_function_time;
extern uint32_t sg_money_exchange_get_yinbi_num;     //货币兑换获取银币基础系数
extern uint32_t sg_money_exchange_get_yinpiao_num;   //货币兑换获取银票基础系数

extern uint32_t sg_guild_bonfire_id;
extern double sg_guild_bonfire_pos_x;
extern double sg_guild_bonfire_pos_z;
extern double sg_guild_bonfire_radius;
extern uint32_t sg_guild_bonfire_reward_interval;
extern uint32_t sg_guild_bonfire_player_reward_time;
extern uint32_t sg_guild_bonfire_time;
extern double sg_guild_bonfire_reward[5];
extern uint32_t sg_guild_bonfire_refresh_collection_interval;
extern std::vector<RandomCollectionTable*> sg_guild_bonfire_collections;
extern uint32_t sg_digong_xiulian_sum_huan_num; //地宫修炼一天可以完成的总环数
extern uint32_t sg_digong_xiulian_sum_lun_num; //地宫修炼一环的总轮数
extern uint32_t sg_zhu_dong_chuan_gong_num;    //主动传功最大次数(自己传给别人)
extern uint32_t sg_bei_dong_chuan_gong_num;    //被动传功最大次数(别人传给自己)
extern uint32_t sg_chuan_gong_level_limit;    //传功等级限制
extern uint32_t sg_zhu_chuan_gong_add_exp;    //主动传功获得经验基础值 
extern uint32_t sg_bei_chuan_gong_add_exp;    //被动传功获得经验基础值 
extern uint32_t sg_zhu_chuan_gong_add_item_id; //主动传功者活动物品id
extern uint32_t sg_zhu_chuan_gong_add_item_num; //主动传功者活动物品num
extern uint32_t sg_guild_chuan_gong_buff_id;    //帮会传功专用buffid
extern uint64_t sg_rand_collect_num;    //
extern uint64_t sg_team_collect_num;    //

extern uint32_t sg_choujiangquan_item_id; //抽奖券id

extern uint32_t sg_world_yinpiao_red_packet_min_num;  //世界银票红包最小数量
extern uint32_t sg_world_yinpiao_red_packet_max_num;  //世界银票红包最大数量
extern uint32_t sg_world_yuanbao_red_packet_min_num;  //世界元宝红包最小数量
extern uint32_t sg_world_yuanbao_red_packet_max_num;  //世界元宝红包最大数量

extern uint32_t sg_guild_yinpiao_red_packet_min_num;  //帮会银票红包最小数量
extern uint32_t sg_guild_yinpiao_red_packet_max_num;  //帮会银票红包最大数量
extern uint32_t sg_guild_yuanbao_red_packet_min_num;  //帮会元宝红包最小数量
extern uint32_t sg_guild_yuanbao_red_packet_max_num;  //帮会元宝红包最大数量
extern uint32_t sg_yinpiao_red_packet_min_money;      //银票红包最小货币值
extern uint32_t sg_yinpiao_red_packet_max_money;      //银票红包最大货币值
extern uint32_t sg_yuanbao_red_packet_min_money;      //元宝红包最小货币值
extern uint32_t sg_yuanbao_red_packet_max_money;      //元宝红包最大货币值
extern uint32_t send_red_packet_min_level;            //发红包最小等级要求

extern uint32_t marry_propose_cancle_return_money;    //取消婚期退还费用百分比
extern uint32_t marry_propose_again_use_money;		   //再次预定婚期消耗百分比
extern uint32_t marry_propose_is_same_sex;            //是否允许是异性 0:不允许 否则允许
extern uint32_t marry_propose_divorce_time;           //离婚多少时长后才能再度求婚或者接受求婚
extern uint32_t marry_propose_min_money;			  //求婚消耗金钱数量
extern double   marry_propose_active_player_x;		  //求婚者站立的x坐标点
extern double   marry_propose_active_player_z;		  //求婚者站立的z坐标点
extern double   marry_propose_passive_player_x;		  //被求婚者站立的x坐标点
extern double   marry_propose_passive_player_z;		  //被求婚者站立的z坐标点
extern uint32_t marry_propose_gaojie_sing_item_id;          //求婚高级婚戒物品id
extern uint32_t marry_propose_haohua_sing_item_id;          //求婚豪华婚戒物品id
extern uint32_t marry_propose_shehua_sing_item_id;          //求婚奢华婚戒物品id
extern uint32_t marry_cancel_propose_marry_use_money;          //取消订婚消耗的元宝数量

#endif /* __LUA_CONFIG_H__ */
