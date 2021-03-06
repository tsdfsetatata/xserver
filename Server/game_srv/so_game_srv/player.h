#ifndef _PLAYER_H__
#define _PLAYER_H__
#include <stdint.h>
#include "cast_skill.pb-c.h"
#include "move_direct.pb-c.h"
#include <stdio.h>
#include <set>
#include <list>
#include <string.h>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include "conn_node_gamesrv.h"
#include "unit_path.h"
#include "attr_id.h"
#include "unit.h"
#include "../proto/move.pb-c.h"
#include "comm_define.h"
#include "my_skill.h"
#include "game_helper.h"

#define UNUSED(x) (void)(x)

class scene_struct;
struct area_struct;
class monster_struct;
class sight_space_struct;
class Team;
class raid_struct;
struct _PlayerDBInfo;
struct ItemsConfigTable;
class partner_struct;
class cash_truck_struct;
struct partner_cur_fabao;

typedef std::map<uint64_t, partner_struct *> PartnerMap;

enum
{
	SIGHT_PRIORITY_NORMAL = 100,
};

#define MAX_PLAYER_IN_PLAYER_SIGHT 60
#define MAX_TRUCK_IN_PLAYER_SIGHT 30
#define MAX_MONSTER_IN_PLAYER_SIGHT 200
#define MAX_COLLECT_IN_PLAYER_SIGHT 50
#define MAX_PARTNER_IN_PLAYER_SIGHT 60


struct ItemUseEffectInfo
{
	uint32_t pos;
	uint32_t use_all;
	bool is_easy;
	std::map<uint32_t, uint32_t> items;
	uint32_t random_box_item_id;
	uint32_t random_box_item_num;
	~ItemUseEffectInfo();
};


struct cast_skill_data
{
//	float direct_x;
//	float direct_z;	
	uint32_t skill_id;
	uint64_t start_time;
};


struct bag_grid_data
{
	uint32_t id;
	uint32_t num;
	uint32_t used_count;
	uint32_t expire_time;
	EspecialItemInfo especial_item;
};

struct HeadIconInfo
{
	uint32_t id;
	uint32_t status; //状态
};

struct TaskCountInfo
{
	uint32_t id;
	uint32_t num;
};

struct TaskInfo
{
	uint32_t id;
	uint32_t status;
	uint32_t accept_ts; //接任务时间
	uint32_t accu_time; //累计时间
	TaskCountInfo progress[MAX_TASK_TARGET_NUM];
};
typedef std::set<uint32_t> TaskSet;

struct SingInfo
{
	uint32_t type;
	uint32_t time;
	bool broad;
	bool include_myself;
	uint64_t start_ts;
	void *args;
};

//struct player_raid_data
//{
//	uint32_t scene_id;   //从副本返回的场景
//	uint32_t pos_x;   //从副本返回的坐标
//	uint32_t pos_z;
//};

struct EquipEnchantInfo
{
	CommonRandAttrInfo cur_attr;
	CommonRandAttrInfo rand_attr[MAX_EQUIP_ENCHANT_RAND_NUM];
};

struct EquipInfo
{
	uint32_t stair; //阶数
	uint32_t star_lv; //星数
	uint32_t star_exp; //升星经验
	EquipEnchantInfo enchant[MAX_EQUIP_ENCHANT_NUM]; //附魔
	int32_t inlay[MAX_EQUIP_INLAY_NUM]; //镶嵌
};

struct FashionInfo 
{
	uint32_t id;
	uint32_t color;
	uint32_t colordown;
	time_t timeout;
	bool isNew;
};

struct HorseInfo 
{
	uint32_t id;
	uint32_t step; //铸灵
	uint32_t star;//铸灵
	time_t timeout;
	bool isNew;
};
const int MAX_HORSE_ATTR_NUM = 4;
const int MAX_HORSE_SOUL = 8;
struct HorseCommonInfo
{
	uint32_t step;//修灵的阶	
	uint32_t attr[MAX_HORSE_ATTR_NUM + 1];
	uint32_t attr_exp[MAX_HORSE_ATTR_NUM + 1]; //修灵次数
	uint32_t n_attr;

	uint32_t soul_step;
	uint32_t soul_exp[MAX_HORSE_SOUL + 1];
	uint32_t soul_star;
	bool soul_full;
	uint64_t power;

	uint32_t total[PLAYER_ATTR_FIGHT_MAX];
	int fly;
	//uint32_t old;
};

struct GoodsInfo
{
	uint32_t goods_id;
	uint32_t bought_num;
};

struct ShopResetInfo
{
	uint32_t next_day_time;
	uint32_t next_week_time;
	uint32_t next_month_time;
};

struct YuqidaoMaiInfo
{
	uint32_t mai_id;
	uint32_t acupoint_id;
	uint32_t fill_lv;
};

struct YuqidaoBreakInfo
{
	uint32_t id;
	uint32_t cur_val[MAX_YUQIDAO_BREAK_ATTR_NUM];
	uint32_t new_val[MAX_YUQIDAO_BREAK_ATTR_NUM];
	uint32_t new_addn[MAX_YUQIDAO_BREAK_ATTR_NUM];
	uint32_t count; //保底计数
};

#define MAX_ONEDAY_PVP_BOX 3
#define PVP_TYPE_DEFINE_3 0
#define PVP_TYPE_DEFINE_5 1
static const int MAX_SHANGJIN_NUM = 3;
static const int MAX_SHANGJIN_AWARD_NUM = 3;

//pvp副本状态
enum pvp_match_state
{
	pvp_match_state_out = 0,  //没有参加
	pvp_match_state_waiting_3 = 10, //3v3等待中
	pvp_match_state_waiting_5, //5v5等待中
	pvp_match_state_not_ready_3, //3v3等待ready
	pvp_match_state_not_ready_5, //5v5等待ready
	pvp_match_state_ready_3, //3v3 ready
	pvp_match_state_ready_5, //5v5ready
	pvp_match_state_start_3, //3v3 开启
	pvp_match_state_start_5, //5v5 开启
};

struct pvp_raid_data
{
	uint32_t pvp_raid_cd;
	pvp_match_state state;
	uint64_t matched_index;

	uint16_t oneday_win_num_3;  //当天胜利次数	
	uint8_t level_3;  //段位
	uint8_t star_3;   //星级
	uint32_t cur_level_id_3;  //当前段位ID
	uint32_t max_level_id_3; //历史最高段位ID
	uint16_t max_score_3; //历史最高积分
	uint16_t score_3;  //积分
	uint32_t avaliable_reward_level_3;  //可以领取的段位奖励
	uint32_t avaliable_box_3[MAX_ONEDAY_PVP_BOX];  //今日剩余还没领取的宝箱下标，从0开始

	uint16_t oneday_win_num_5;  //当天胜利次数	
	uint8_t level_5;  //段位
	uint8_t star_5;   //星级
	uint32_t cur_level_id_5; //当前段位ID
	uint32_t max_level_id_5; //历史最高段位ID
	uint16_t max_score_5; //历史最高积分
	uint16_t score_5;  //积分
	uint32_t avaliable_reward_level_5;  //可以领取的段位奖励
	uint32_t avaliable_box_5[MAX_ONEDAY_PVP_BOX];  //今日剩余还没领取的宝箱下标，从0开始	
};

struct BaguapaiCardInfo
{
	uint32_t id; //八卦牌ID
	uint32_t star;
	CommonRandAttrInfo minor_attrs[MAX_BAGUAPAI_MINOR_ATTR_NUM]; //副属性
	CommonRandAttrInfo minor_attrs_new[MAX_BAGUAPAI_MINOR_ATTR_NUM]; //新的副属性
	CommonRandAttrInfo additional_attrs[MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM]; //追加属性
};

struct BaguapaiDressInfo
{
	BaguapaiCardInfo card_list[MAX_BAGUAPAI_DRESS_NUM];
};

struct GuoYu
{
	int32_t guoyu_level;
	int32_t cur_exp;
	int32_t cur_task; //0 没任务
	uint64_t task_timeout; //0 无cd  
	uint64_t critical_cd; // 紧急cd 0没开
	uint64_t critical_next_refresh; // 紧急cd下次刷新时间
	int32_t critical_num; // 紧急剩余次数
	int32_t guoyu_num; //国御剩余收益次数
	int32_t type; //当前难度
	int32_t map;
	uint32_t random_map;
	bool award; //高级奖励
};
struct ChengJie
{
	int32_t level;
	int32_t cur_exp;
	uint32_t cur_task; //0 没任务
	//uint64_t task_timeout; //0 无cd  
	int chengjie_num; //剩余次数
	uint64_t target;
	uint64_t rest; //下次能被悬赏cd
	bool first_hit;
};
struct ShangJinTask 
{
	uint32_t id;
	uint32_t quality;
//	AttrInfo award[MAX_SHANGJIN_AWARD_NUM];
//	uint32_t n_award;
	uint32_t drop_id;
	uint32_t reduce;
	uint32_t coin;
	uint32_t exp;
};
struct ShangJin
{
	int32_t level;
	int32_t cur_exp;
	ShangJinTask task[MAX_SHANGJIN_NUM];
	uint32_t cur_task; //
	int shangjin_num; //剩余次数
	bool accept;
	uint32_t free; //免费刷新次数
};

struct ZhenYing 
{
	uint32_t level; //军阶
	uint32_t exp;
	uint32_t step;
	uint32_t exp_day;//每天能获得的经验 
	uint32_t free;//免费换阵营次数
	uint32_t change_cd; //转换阵营CD 
	uint64_t exploit; //功勋
	uint64_t history_exploit;
	bool daily_award;  //军阶每日奖励
	bool daily_step;  //军阶每日奖励
	uint32_t step_lv; //军阶

	//周任务
	uint32_t last_week;//上次更新时间
	uint32_t week; 
	uint32_t task;
	uint32_t task_type;
	uint32_t task_num;
	uint32_t task_award_state;
	
	//战绩
	uint32_t kill;
	uint32_t death;
	uint32_t help;
	int score;
	uint32_t mine; //挖宝箱次数
	uint32_t kill_week; //战场一周杀人
	uint32_t score_week; //战场周积分
	uint32_t gather; //是否有矿车到达终点奖励
	uint64_t score_time; //护送矿车给积分的时间
	uint32_t protect_num; //护矿次数

	uint32_t award_num; //阵营对战收益次数
	uint64_t fb_cd;
	int one_award;
};

static const int MAX_QUESTION_ANSWER = 4;
struct CommonAnswer 
{
	uint32_t question; //题目
	uint32_t contin; //连续答对
	uint32_t right; //答对数
	uint32_t money; //
	uint32_t exp; //
	uint32_t tip; //剩余提示次数
	uint32_t help; //剩余求助次数
	bool btip; //true 已经提示过
	bool bhelp; //true 已经求助过 
	uint32_t number; //第几题
	uint32_t answer[MAX_QUESTION_ANSWER];
	uint32_t answer_tip[MAX_QUESTION_ANSWER];

	uint64_t next_open; //下次接任务时间
};

struct NpcAnswer
{
	uint32_t trun; //第几轮 从1开始
	uint32_t npc; //当前轮的答题NPC
	uint32_t right; //答对数
	uint32_t contin; //连续答对
	uint32_t money; //
	uint32_t exp; //
	uint32_t timer; //总耗时
	uint32_t score; //积分
	uint32_t question; //题目
	uint32_t number; //第几题
	bool bOpenWin;   //打开了答题界面
	uint64_t begin_time; //

	uint64_t next_open; //下次接任务时间
};

// public enum MapAreaType
// {
// 	Area_Normal=10,//普通区域
// 	Area_Peace=11,//和平区域
// 	Area_Camp=12,//阵营区域
// 	Area_Shade = 13,//阴区域
// 	Area_Positive = 14,//阳区域
// 	Area_Positive = 15,//悬崖
// }

struct DailyActivityInfo
{
	uint32_t act_id;
	uint32_t count;
};
struct ChivalryActivityInfo
{
	uint32_t act_id;
	uint32_t val;
};

struct FightingCapacity
{
	uint32_t level;
	uint32_t equip;
	uint32_t horse;
	uint32_t yuqidao;
	uint32_t bagua;
	uint32_t guild_skill;
	uint32_t partner;
	uint32_t fashion;
	uint32_t title;
	uint32_t zhenying;
	uint32_t get_total(void);
};

static const int MAX_LIVE_SKILL_NUM = 2;
struct LiveSkill 
{
	uint32_t exp[MAX_LIVE_SKILL_NUM + 1];
	uint32_t level[MAX_LIVE_SKILL_NUM + 1];
	uint32_t broken[MAX_LIVE_SKILL_NUM + 1];
	uint32_t book[MAX_LIVE_SKILL_NUM + 1];
};

struct XunBaoData 
{
	uint32_t door_map;   //场景ID
	float door_x;     //传送门坐标
	float door_z;
	float door_y;
	uint64_t cd;           //npc消失的时间
	uint32_t door_id;     //传送点ID
	bool send_next;
};

struct CashTruckData
{
	uint64_t truck_id; //镖车
	bool on_truck; //false 不在镖车上 true在
	uint32_t active_id;
	uint32_t num_coin; //粮草押镖次数
	uint32_t num_gold; //财宝押镖次数
	uint32_t jiefei;// 劫匪出现的次数 
	position pos;
	uint32_t hp;
	uint32_t scene_id;
};

//玩家离开副本后所到的位置
struct LeaveRaidPosition
{
	double direct; //退出副本朝向
	double ExitPointX;  //退出副本横坐标 
	double ExitPointY;  //退出副本高度坐标
	double ExitPointZ;  //退出副本的纵坐标
	uint32_t scene_id; //退出副本所进入的场景ID
};

struct EscortInfo
{
	uint32_t escort_id; //护送ID
	uint32_t start_time; //开始时间
	uint32_t too_far_time; //距离过远开始时间
	uint32_t summon_monster_waves[MAX_ESCORT_MONSTER_WAVE]; //召唤的波数
	uint64_t summon_monster_uuids[MAX_ESCORT_MONSTER_NUM]; //召唤的怪物ID
	uint32_t summon_num;
	bool     mark_delete;
};

struct AchievementInfo
{
	uint32_t id;
	uint32_t star;
	uint32_t progress;
	uint32_t state;
	uint32_t achieveTime;
};

enum
{
	Achievement_State_Achieving = 0, //奖励不可领
	Achievement_State_Achieved = 1, //奖励可领
	Achievement_State_Rewarded = 2, //奖励已领
};

struct TitleInfo
{
	uint32_t id;
	uint32_t state;
	uint32_t expire_time;
	bool     is_new;
	uint32_t active_time;
};

struct HeroChallengeRewardInfo
{
	uint32_t item_id;
	uint32_t item_num;
};

struct HeroChallengeInfo
{
	uint32_t id;     //id
	uint32_t star;   //最高星级
	uint32_t reward_flag; //领奖标志1:表示身上有扫荡奖励未领取
	HeroChallengeRewardInfo item_info[MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM];//未领取的奖励物品信息
};

struct MiJingXiuLianTaskInfo
{
	uint32_t digong_id; //地宫修炼表id
	uint32_t task_id; //当前任务id(0表示未接任务)
	uint32_t time_state; //任务时间标志(0:今日接,1:昨日接)
	uint32_t reward_beilv; //当前奖励倍率下标
	uint32_t huan_num; //今日已完成总环数
	uint32_t lun_num;  //当前环已完成轮数
};

struct StrongGoalInfo
{
	uint32_t id; //成长ID
	uint32_t progress;
	uint32_t state;
};

struct StrongChapterInfo
{
	uint32_t id; //章节ID
	uint32_t progress;
	uint32_t state;
};

//等级礼包
struct PlayerLevelReward
{
	uint32_t id;   //等级奖励表索引id
	bool receive;  //是否领取奖励
};

//在线奖励信息
struct PlayerOnlineReward
{
	uint64_t sign_time; //登录时间
	uint32_t befor_online_time; //当天本次登录前的总在线时间(不计算本次登录到目前的时间)
	uint32_t use_reward_num; //今日已经领奖次数
	uint32_t reward_id_today[MAX_PLAYER_ONLINE_REWARD_NUM]; //今日的奖励表id
	uint32_t reward_table_id[MAX_PLAYER_ONLINE_REWARD_NUM]; //已经领取的奖励表id
	uint32_t reward_id;    //奖励表id(奖励在转完盘后给，所以先记录)
};

//每日签到累计额奖励
struct SignInEveryDayCumulative
{
	uint32_t id;  //累计奖励表id
	uint32_t state; //领取状态 0:不可领 1:可领 2:已领
};
//每日签到数据
struct SignInEveryDayData
{
	bool today_sign; //今日是否签到
	uint32_t cur_month; //当前的月份
	uint32_t month_sum; //本月累计已经签到次数
	uint32_t yilou_sum; //本月遗漏次数
	uint32_t buqian_sum; //本月累计可补签次数;
	uint32_t activity_sum;  //本月通过活跃度所累积的补签次数

	//累计奖励
	SignInEveryDayCumulative grand_reward[MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM]; //累计奖励领取状态
};

//活动奖励找回单个活动信息
struct ActiveRewardZhaohuiInfo
{
	uint32_t id;  //奖励表id
	uint32_t num; //可找回奖励次数
	uint32_t normo_exp; //普通找回经验
	uint32_t perfect_exp; //完美找回经验
	uint32_t normo_coin; //普通找回银两
	uint32_t perfect_coin; //完美找回银两
	uint32_t normo_use;    //普通找回消耗银票数量
	uint32_t perfect_use; //完美找回消耗金票数量
};

// TowerInfo
struct TowerData
{
	uint32_t reset_num; //重置次数
	uint32_t cur_lv; //当前层
	uint32_t cur_num; //当前层次数
	uint32_t top_lv; //最高层
	uint32_t award_lv;
};

//摇钱树相关信息
struct YaoQianShuAllData
{
	uint32_t sum_num;   //今日可摇总次数
	uint32_t use_num;   //今日已经摇了多少次
	uint32_t free_num;  //今日剩余免费次数
	uint32_t next_need_money; //下一次摇需要消耗的元宝
	uint32_t cur_jizhi_num; //当前机制数(其实就是获得两倍以上暴击后又摇了多少次)
	uint32_t cur_suiji_zhong;  //当前的随机种子
	uint32_t beilv_num;      //当 cur_jizhi_num == cur_suiji_zhong时能获得的暴击倍率
};

//登录奖励领取情况
struct LoginReceiveInfo
{
	uint32_t id;    //奖励表id
	uint32_t statu; //领取标记 0:不可领, 1:可领 2:已领
};
//登录奖励
struct LoginRewardData
{
	bool open; //功能是否开启
	bool receive_time; //是否在领取时间内
	uint32_t open_time; //功能开启的时间点
	uint32_t login_day; //当前是第几天
	LoginReceiveInfo info[MAX_LOGIN_REWARD_RECEIVE_NUM]; //奖励领取情况
};

//赐福奖励信息
struct CiFuRewardData
{
	uint32_t id; //赐福表id
	uint64_t time; //上次领取时间
};

//门宗传功单请求列表
struct GuildChuanGongRequestList
{
	uint64_t player_id; //玩家id
	uint64_t time;    //请求额时间戳
};

//当前正在传功的信息
struct CurGuildChuanGongInfo 
{
	uint32_t type; //1:我传给别人 2:别人传给我
	uint64_t player_id; //对方玩家id
};

//门宗传功信息
struct GuildChuanGong
{
	uint32_t bei_chuan_num; //今日已经被传功次数
	uint32_t give_chuan_num; //今日已经传给别人的次数
	GuildChuanGongRequestList me_to_other[MAX_GUILD_MEMBER_NUM]; //我请求 自己传功给别的玩家的列表
	GuildChuanGongRequestList other_to_me[MAX_GUILD_MEMBER_NUM]; //我请求 别人传功给我的玩家的列表
	CurGuildChuanGongInfo  cur_info; //当前正在传功的信息
};

struct JiuGongBaGuaRewardInfo
{
	uint32_t id; //九宫八卦奖励表id
	uint32_t task_id; //九宫八卦奖励对应的任务id
	uint32_t statu; //领取标记 0:不可领, 1:可领 2:已领
};

//当前正在求婚的数据
struct CurProposeData
{
	uint32_t statu; //0:主动者 1:被动者
	uint64_t player_id; //对方玩家id
	uint64_t time; //开始的时间戳
	uint32_t ring_type; //婚戒档次(根据婚戒来)
};

//玩家情缘信息数据
struct PlayerMarryData
{
	uint32_t statu; //玩家当前的婚姻状态 0:单身(且无婚史) 1:单身(且有婚史) 2:当前已订婚(未预定婚礼) 3:当前已订婚(且已经预定婚礼) 4:已婚
	uint32_t my_role; //0:表示我是求婚者 1:是被求婚者
	uint32_t propose_type; //求婚档次(更婚戒档次一样)
	uint64_t propose_success_time; //求婚成功的时间点

	uint32_t reserve_marry_type; //所预定的婚礼档次
	uint64_t reserve_marry_time; //所预定的婚礼时间

	uint64_t target_id; //当前对象id
	char target_name[MAX_PLAYER_NAME_LEN + 1];    //当前对象名字
	uint32_t sex; //对象性别

	uint64_t time; //上次离婚时间戳

	CurProposeData cur_propose_info; //当前正在求婚的信息
	bool cur_cancel_propose; //当前是否在等待对方确认取消订婚
};

enum
{
	Strong_State_Achieving = 0, //奖励不可领
	Strong_State_Achieved = 1, //奖励可领
	Strong_State_Rewarded = 2, //奖励已领
};

struct player_data
{
	uint64_t player_id;
	PlayerStatus status;

	uint32_t default_hair;
	uint32_t default_icon;
	uint32_t default_hair_color;
	uint32_t default_up_color;
	uint32_t default_down_color;

	char name[MAX_PLAYER_NAME_LEN + 1];    //名字
	uint32_t create_time; //创角时间
//	uint32_t headicon;    //头像ID
	uint32_t scene_id;  //当前场景ID
	uint64_t player_raid_uuid; //副本唯一ID, 恢复模式的话需要用这个来恢复
	uint32_t last_scene_id; //上一个野外场景
//	bool player_is_in_loading; //是否在loading过程中，恢复模式需要用这个来恢复
//	uint16_t region_id;
//	struct position pos;
	float pos_y;  //注意y是高度
//	float speed;  //速度
	double attrData[PLAYER_ATTR_MAX]; //属性
	double buff_fight_attr[MAX_BUFF_FIGHT_ATTR]; //战斗算上buff百分比属性
	
//视野相关
	uint64_t sight_player[MAX_PLAYER_IN_PLAYER_SIGHT];
	uint64_t sight_monster[MAX_MONSTER_IN_PLAYER_SIGHT];
	uint64_t sight_truck[MAX_TRUCK_IN_PLAYER_SIGHT];
	uint64_t sight_partner[MAX_PARTNER_IN_PLAYER_SIGHT];	
	int cur_sight_player;
	int cur_sight_monster;	
	int cur_sight_truck;
	int cur_sight_partner;	

//移动路径
	struct unit_path move_path;

	SingInfo sing_info;
	bool login_notify;  //false表示第一次登陆进场景，true表示场景切换
	FightingCapacity fc_data;

	struct cast_skill_data cur_skill;  //正在释放的技能

	bag_grid_data bag[MAX_BAG_GRID_NUM]; //背包  size 9600
	uint32_t bag_grid_num; //背包开启格子数
	uint32_t bag_unlock_num; //背包解锁格子数

	//头像
	HeadIconInfo head_icon_list[MAX_HEAD_ICON_NUM]; //头像列表

	//任务
	TaskInfo task_list[MAX_TASK_ACCEPTED_NUM];
	uint32_t task_finish[MAX_TASK_NUM];
	uint32_t task_chapter_reward;

	uint64_t teamid;
	uint64_t next_time_refresh_oneday_job;

//	struct player_raid_data raid_data;
	uint32_t raid_reward_id[MAX_RAID_NUM];
	uint32_t raid_reward_num[MAX_RAID_NUM];
	uint32_t script_reward_num;

	//装备
	EquipInfo equip_list[MAX_EQUIP_NUM];

	//时装
	uint32_t color[MAX_COLOR_NUM];
	int color_is_new[MAX_COLOR_NUM];
	uint32_t n_color;
	uint32_t weapon_color[MAX_WEAPON_COLOR_NUM];
	int weapon_color_is_new[MAX_WEAPON_COLOR_NUM];
	uint32_t n_weapon_color;
	FashionInfo fashion[MAX_FASHION_NUM];
	uint32_t n_fashion;
	uint32_t charm_level;
	uint32_t charm_total;

	//坐骑
	HorseInfo horse[MAX_HORSE_NUM];
	uint32_t n_horse;
	HorseCommonInfo horse_attr;

	//商城
	GoodsInfo shop_goods[MAX_SHOP_GOODS_NUM];
	ShopResetInfo shop_reset;

	//御气道
	YuqidaoMaiInfo yuqidao_mais[MAX_YUQIDAO_MAI_NUM];
	YuqidaoBreakInfo yuqidao_breaks[MAX_YUQIDAO_BREAK_NUM];

	uint32_t next_set_pktype_time;
	uint64_t qiecuo_target; //请求的切磋对象
	struct position qiecuo_pos;  //切磋的中点
	uint64_t qiecuo_start_time; //开始切磋的时间，没有在切磋的话这个值是0
	uint32_t qiecuo_out_range_fail_time; //如果当前时间超过这个时间，表示切磋的时候离开圈太久而失败

	struct pvp_raid_data pvp_raid_data;

	uint32_t m_collect_uuid;  //玩家当前正在采集的采集物的唯一ID
	uint32_t m_rand_collect_num;  //随机采集次数
	uint32_t m_team_collect_num;  //队伍共享采集次数
	uint32_t guild_id;
	uint32_t guild_office;
	uint32_t guild_donation;
	uint32_t guild_battle_activity_time; //所参加的帮会战活动开始时间
	ProtoGuildSkill guild_skills[MAX_GUILD_SKILL_NUM];
	uint32_t guild_task_count;
	uint32_t guild_task_config_id;
	uint32_t guild_bonfire_last_ts; //用于记录开始计算的时间戳
	uint32_t guild_bonfire_reward_time; //帮会篝火累计收益时间
	uint32_t guild_bonfire_activity_time; //所参加的篝火活动开始时间

	//八卦牌
	BaguapaiDressInfo baguapai_dress[MAX_BAGUAPAI_STYLE_NUM]; //八卦牌装备列表
	GuoYu guoyu;
	int cur_yaoshi;
	uint32_t change_special_num;
	ChengJie chengjie;
	ShangJin shangjin;
	ZhenYing zhenying;
	CommonAnswer common_answer;
	NpcAnswer award_answer;

	uint8_t camp_id;

	double m_angle;  //朝向，移动的时候设置

	//活动
	uint32_t active_reward[MAX_ACTIVE_REWARD_NUM]; //已领取的活跃度奖励
	DailyActivityInfo daily_activity[MAX_DAILY_ACTIVITY_NUM]; //日常活动
	ChivalryActivityInfo chivalry_activity[MAX_CHIVALRY_ACTIVITY_NUM]; //侠义活动

	uint8_t qiecuo_invite_switch; //切磋邀请开关
	uint8_t team_invite_switch; //组队邀请开关
	uint32_t out_stuck_time; //脱离卡死CD时间

	uint64_t next_update;

	//个人信息
	uint32_t personality_sex; //性别
	uint32_t personality_birthday; //生日（格式：20170410）
	char personality_location[MAX_PERSONALITY_LOCATION_LEN + 1]; //位置
	uint32_t personality_tags[MAX_PERSONALITY_TAG_NUM]; //标签
	char personality_text_intro[MAX_PERSONALITY_TEXT_INTRO_LEN + 1]; //文字签名
	char personality_voice_intro[MAX_PERSONALITY_VOICE_INTRO_LEN + 1]; //语音签名
	uint32_t personality_province; //省份
	uint32_t personality_city; //城市
	uint32_t personality_blood_type; //血型

		//自动补血
	uint64_t next_auto_add_hp_time; //下次血池补血时间或者脱战时间
	bool   on_fight_state;    //是否在战斗状态
	bool   open_auto_add_hp;
	uint32_t auto_add_hp_item_id; //自动补血的道具ID
	uint32_t auto_add_hp_percent; //自动补血的百分比，0-100
	uint32_t hp_pool_num;   //剩余的血池容量

	LiveSkill live_skill;
	XunBaoData xunbao;

	EscortInfo escort_list[MAX_ESCORT_NUM]; //护送列表
	CashTruckData truck;

	//伙伴
	uint32_t partner_dictionary[MAX_PARTNER_TYPE]; //伙伴图鉴
	uint64_t partner_formation[MAX_PARTNER_FORMATION_NUM]; //伙伴上阵
	uint64_t partner_battle[MAX_PARTNER_BATTLE_NUM]; //伙伴出战
	uint32_t partner_recruit_junior_free_cd; //低级招募免费CD
	uint32_t partner_recruit_junior_free_count; //低级招募免费次数
	uint32_t partner_recruit_junior_count; //低级招募计数，用于保底概率
	uint32_t partner_recruit_senior_free_cd; //高级招募免费CD
	uint32_t partner_recruit_senior_free_count; //高级招募免费次数
	uint32_t partner_recruit_senior_count; //高级招募计数，用于保底概率
	bool     partner_recruit_first; //首次招募标志
	uint32_t partner_bond[MAX_PARTNER_BOND_NUM]; //伙伴羁绊
	uint32_t partner_bond_reward[MAX_PARTNER_TYPE]; //伙伴羁绊奖励领取记录
	uint32_t  partner_today_junior_recurit_count;  //今日JUNIOR伙伴招募次数
	uint32_t  partner_today_junior_recurit_cd;  //今日JUNIOR伙伴招募cd
	uint32_t  partner_today_senior_recurit_count;  //今日senior伙伴招募次数
	uint32_t  partner_today_senior_recurit_cd;  //今日senior伙伴招募cd
	uint64_t  partner_add_angry_cd;  //增加怒气值cd

	LeaveRaidPosition leaveraid; //离开副本
	bool noviceraid_flag;	//新手副本是否完成的标记
	uint64_t Receive_type;		//即将开启，已经领取最大奖励的类型

	uint64_t world_chat_cd;

	uint32_t team_raid_id_wait_ready;    //在等待准备的副本ID
	bool is_team_raid_ready;  //是否ready

	ProtoFriend friend_contacts[MAX_FRIEND_CONTACT_NUM];
	ProtoEnemy  friend_enemies[MAX_FRIEND_ENEMY_NUM];
	ProtoRank ranks[MAX_RANK_TYPE];

	uint8_t  server_level_break_count; //服务器等级突破计数
	uint32_t server_level_break_notify; //服务器等级突破通知，记录level_id

	AchievementInfo achievement_list[MAX_ACHIEVEMENT_NUM]; //成就列表  size 8000
	TitleInfo title_list[MAX_TITLE_NUM]; //称号列表

	//英雄挑战数据
	HeroChallengeInfo my_hero_info[MAX_HERO_CHALLENGE_MONSTER_NUM];  // size 17200
	//地宫修炼任务信息
	MiJingXiuLianTaskInfo mi_jing_xiu_lian;

	//钓鱼
	uint32_t fishing_bait_id; //钓鱼鱼饵ID
	uint32_t fishing_reward_num; //钓鱼收益次数
	bool     fishing_during; //钓鱼中，不存库

	//我要变强
	StrongGoalInfo    strong_goals[MAX_STRONG_GOAL_NUM];
	StrongChapterInfo strong_chapters[MAX_STRONG_CHAPTER_NUM];

	bool playing_drama;

	uint32_t doufachang_rank; //斗法场排名

	uint32_t travel_round_num; //游历当前轮数
	uint32_t travel_round_count_out; //游历当前轮数是否算在今天轮数里，0：算在内，非0：不算
	uint32_t travel_task_num; //游历当前环数

	//等级礼包
	PlayerLevelReward my_level_reward[MAX_PLAYER_LEVEL_REWARD_NUM];

	//在线奖励
	PlayerOnlineReward online_reward;
	
	//每日签到奖励
	SignInEveryDayData sigin_in_data;

	//活动奖励找回
	ActiveRewardZhaohuiInfo zhaohui_data[MAX_ACTIVE_CAN_ZHAOHUI_REWARD];

	TowerData tower;

	//摇钱树
	YaoQianShuAllData yaoqian_data;

	//登录奖励
	LoginRewardData login_reward_info;

	//功能开启
	uint32_t function_open[MAX_GAME_FUNCTION_NUM];

	//上次赐福奖励领取信息
	CiFuRewardData ci_fu_reward[MAX_CIFU_REWARD_NUM];

	//门宗传功信息
	GuildChuanGong guild_chuan_gong_info;

	//九宫八卦奖励信息
	JiuGongBaGuaRewardInfo jiu_gong_ba_gua_reward[MAX_JIU_GONG_BA_GUA_REWARD_NUM];

	//玩家情缘信息
	PlayerMarryData player_marry_info;
};

struct ai_player_data
{
	uint16_t ai_state;
	uint64_t ontick_time;
	uint32_t skill_id;  //即将释放的技能
	double angle;     //技能的角度
	struct position skill_target_pos;  //技能释放的位置
	uint64_t relive_time;
	uint64_t target_player_id;
	uint64_t skill_finished_time[MAX_SKILL_TIME_CONFIG_NUM];
	uint64_t skill_next_time[MAX_SKILL_TIME_CONFIG_NUM];
	uint8_t  skill_next_time_idx; 
	
		//ai巡逻配置
	struct RobotPatrolTable *ai_patrol_config;

	uint8_t player_ai_index;
	uint64_t origin_player_id;   //原玩家ID
	bool stop_ai; //停止AI
	uint8_t patrol_index; //巡逻路径
	uint8_t active_attack_range; //主动攻击范围
	uint8_t chase_range;  //追击范围

		//普攻三连击
	uint32_t normal_skill_id;  //下一个三连击技能ID，0表示没有
	uint64_t normal_skill_timeout;  //三连击的超时时间
};

class player_struct: public unit_struct
{
public:
		//场景跳转相关接口
	int move_to_scene_pos(uint32_t scene_id, double pos_x, double pos_z, double direct, EXTERN_DATA *extern_data);  //进入野外/副本		
	int move_to_scene(uint32_t scene_id, EXTERN_DATA *extern_data);  //进入野外/副本	
	int move_to_transfer(uint32_t transfer_id, EXTERN_DATA *extern_data);  //进入野外/副本
	int cur_scene_jump(double pos_x, double pos_z, double direct, EXTERN_DATA *extern_data);	//当前场景跳转
	int move_to_raid_impl(DungeonTable *config, bool ignore_check);

	int check_scene_enter_cond(uint32_t scene_id);	//进入地图的条件是否满足(镖车和进入等级)
	int check_raid_enter_cond(uint32_t raid_id);

	int check_enter_raid_cost(struct DungeonTable *r_config);
	int do_enter_raid_cost(uint32_t item_id, uint32_t item_num);
	int do_team_enter_raid_cost(uint32_t raid_id);	
	int check_enter_raid_reward_time(uint32_t id, struct ControlTable *config);
	int send_enter_raid_fail(uint32_t err_code, uint32_t n_reason_player, uint64_t *reason_player_id, uint32_t item_id);	
public:
	typedef std::multimap<uint32_t, uint32_t> ItemPosMap;

	ItemPosMap item_pos_cache;
	TaskSet task_finish_set;
	struct ai_player_data *ai_data;
public:
	player_struct();
	virtual ~player_struct();
	player_data *data;   //放共享内存的数据，可以恢复模式下恢复
	void init_player();
	void clear(void);

	void send_enter_region_notify(int region_id);

	bool is_in_same_guild(player_struct *player);
	bool is_on_horse(void);
	bool is_on_truck(void);

	void on_player_enter_scene(double direct);

	void refresh_player_redis_info(bool offline = false);
	void send_raid_earning_time_notify();
	void send_buff_info();
	void add_raid_reward_count(uint32_t raid_id);
	uint32_t get_raid_reward_count(uint32_t raid_id);
	
	void send_hp_pool_changed_notify();

	UNIT_TYPE get_unit_type();		
	bool is_avaliable();
	uint32_t get_skill_id();
	uint64_t get_uuid();
	double *get_all_attr();
	double get_attr(uint32_t id);
	double *get_all_buff_fight_attr();
	double get_buff_fight_attr(uint32_t id);
	void clear_cur_skill();
	bool is_ai_player();

	void add_attr(uint32_t id, double value);		
	void set_attr(uint32_t id, double value);	
	struct unit_path *get_unit_path();
	float get_speed();
	int *get_cur_sight_player();
	uint64_t *get_all_sight_player();
	int *get_cur_sight_monster();
	uint64_t *get_all_sight_monster();
	virtual int *get_cur_sight_truck();
	virtual uint64_t *get_all_sight_truck();
	virtual int *get_cur_sight_partner();
	virtual uint64_t *get_all_sight_partner();	
	void try_return_raid();
	void try_return_zhenying_raid();	
	void try_return_guild_land_raid();	
	void try_return_guild_wait_raid();	
	void try_return_guild_battle_raid();

	bool is_chengjie_target(uint64_t player_id);

	JobDefine get_job();
	uint32_t get_sex();	
	uint32_t get_level();

	virtual uint32_t count_life_steal_effect(int32_t damage);
	virtual uint32_t count_damage_return(int32_t damage, unit_struct *unit);

	int transfer_to_new_scene_by_config(uint32_t transfer_id, EXTERN_DATA *extern_data);	
//	int transfer_to_new_scene(uint32_t scene_id, EXTERN_DATA *extern_data);
	int transfer_to_new_scene(uint32_t scene_id, double pos_x, double pos_y, double pos_z, double direct, EXTERN_DATA *extern_data);
	int transfer_to_new_scene_impl(scene_struct *new_scene, double pos_x, double pos_y, double pos_z, double direct, EXTERN_DATA *extern_data);
	int transfer_to_birth_position(EXTERN_DATA *extern_data); //将玩家传送回地图出生点
	void send_chat(int channal, char *conten);

//	int get_pk_type();
	bool can_beattack();
	bool is_in_safe_region();
	int get_camp_id();
	void set_camp_id(int id);
	
	Team *get_team();

	void relive();
	
	void on_tick();
	void on_tick_10();
	void on_dead(unit_struct *killer);
	void on_hp_changed(int damage);
	void on_beattack(unit_struct *player, uint32_t skill_id, int32_t damage);
	void on_attack(unit_struct *target);
	void on_relive(uint32_t type);
	void on_repel(unit_struct *player);
	void interrupt();
	void refresh_oneday_job();  //每天刷新一次
	void refresh_shop_daily(void); //商城每日刷新

	bool is_online(void);
	bool is_in_raid();
	bool is_in_pvp_raid();	
//	bool is_player_in_sight(uint64_t other);

//	struct position *get_player_pos();

//sight相关
	int prepare_add_player_to_sight(player_struct *player);
	int prepare_add_monster_to_sight(monster_struct *monster);
	int prepare_add_truck_to_sight(cash_truck_struct *truck);
	int prepare_add_partner_to_sight(partner_struct *partner);	
	
	int get_sight_priority(player_struct *player);   //是不是要优先加入视野
//	bool is_player_in_sight(uint64_t player_id);

//	int add_player_to_sight(uint64_t player_id);
//	int del_player_from_sight(uint64_t player_id);

	bool on_truck_leave_sight(uint64_t player_id);		
	bool on_truck_enter_sight(uint64_t player_id);	
	bool on_player_leave_sight(uint64_t player_id);
	bool on_player_enter_sight(uint64_t player_id);
	bool on_monster_leave_sight(uint64_t uuid);
	bool on_monster_enter_sight(uint64_t uuid);
	bool on_partner_leave_sight(uint64_t player_id);
	bool on_partner_enter_sight(uint64_t player_id);

	void set_team_raid_id_wait_ready(uint32_t raid_id);
	void unset_team_raid_id_wait_ready();	

//both 系列函数
//player:
//	player, monster, truck, partner
//
//monster:
//	monster, truck, partner
//
//truck:
//	partner
//
//partner:
//	partner
	
	int add_player_to_sight_both(player_struct *player);
	int del_player_from_sight_both(player_struct *player);
	int add_monster_to_sight_both(monster_struct *monster);
	int del_monster_from_sight_both(monster_struct *monster);
	int add_cash_truck_to_sight_both(cash_truck_struct *truck);
	int del_cash_truck_from_sight_both(cash_truck_struct *truck);
	int add_partner_to_sight_both(partner_struct *partner);
	int del_partner_from_sight_both(partner_struct *partner);	
	
//	void broadcast_to_sight(uint16_t msg_id, void *msg_data, pack_func func, bool include_myself);

//	bool is_monster_in_sight(uint64_t uuid);
//	int add_monster_to_sight(uint64_t uuid);
//	int del_monster_from_sight(uint64_t uuid);
	void clear_player_sight();

	void process_offline(bool again = false, EXTERN_DATA *ext_data = NULL);
	void cache_to_dbserver(bool again = false, EXTERN_DATA *ext_data = NULL); //通过db_srv保存
	int pack_playerinfo_to_dbinfo(uint8_t *out_data);
	int unpack_dbinfo_to_playerinfo(uint8_t *packed_data, int len);
	void clear_temp_data();

	void broadcast_to_sight_and_team(uint16_t msg_id, void *msg_data, pack_func func, bool include_myself);
	
	void update_sight(area_struct *old_area, area_struct *new_area);
	int broadcast_player_create(scene_struct *scene);
	int broadcast_player_delete();
	void update_player_pos_and_sight();	
	void pack_sight_player_base_info(SightPlayerBaseInfo *info, ChuangongTarget *chuan_gong_info);
	void send_clear_sight();
	void send_clear_sight_monster();	
	void send_scene_transfer(float direct, float pos_x, float pos_y, float pos_z, uint32_t old_scene_id, uint32_t scene_id, int32_t result);
	void notify_watch_pos_change();
	void broadcast_sight_player_info_changed_notify();	

	//属性
	void calculate_attribute(bool isNty = false);
	void notify_attr(AttrMap& attr_list, bool broadcast = false, bool include_myself = true);
	void broadcast_one_attr_changed(uint32_t id, double value, bool send_team, bool include_myself);
	void notify_attr_changed(uint32_t num, uint32_t* id, double *value);
	void notify_one_attr_changed(uint32_t attr_id, double attr_val);
	
	void calcu_level_attr(double *attr); //等级属性
	void calcu_equip_attr(double *attr); //装备属性
	void calcu_yuqidao_attr(double *attr); //御气道属性
	void calcu_baguapai_attr(double *attr); //八卦牌属性
	void calcu_guild_skill_attr(double *attr); //帮会技能属性
	void calcu_partner_attr(double *attr); //伙伴属性
	void calcu_fashion_attr(double *attr); //时装属性
	void calcu_title_attr(double *attr); //称号属性

	uint32_t get_partner_fc(void); //获取伙伴模块的战力

	char *get_name();
	bool is_in_qiecuo();
	void set_qiecuo(uint32_t pos_x, uint32_t pos_z, uint64_t target);
	void finish_qiecuo();
	bool is_qiecuo_target(player_struct *target);

	void print_attribute(const char * stype, double *attr);

	//货币
	int get_comm_gold(void); //获取绑定+非绑定元宝
	int add_unbind_gold(uint32_t num, uint32_t statis_id, bool isNty = true); //增加非绑定元宝
	int add_bind_gold(uint32_t num, uint32_t statis_id, bool isNty = true); //增加绑定元宝
	int sub_unbind_gold(uint32_t num, uint32_t statis_id, bool isNty = true); //消耗非绑定元宝
	int sub_comm_gold(uint32_t num, uint32_t statis_id, bool isNty = true); //消耗通用元宝，先消耗绑定元宝
	int add_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //增加银票
	int sub_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //消耗银票
	int sub_comm_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //消耗银票，不够就消耗银币
	int add_silver(uint32_t num, uint32_t statis_id, bool isNty = true); //增加银两
	int sub_silver(uint32_t num, uint32_t statis_id, bool isNty = true); //消耗银两
//	int add_chengjie_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //
//	int sub_chengjie_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //
//	int add_guoyu_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //
//	int sub_guoyu_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //
//	int add_shangjin_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //
//	int sub_shangjin_coin(uint32_t num, uint32_t statis_id, bool isNty = true); //
	int add_gongxun(uint32_t num, uint32_t statis_id, bool isNty = true); //
	int sub_gongxun(uint32_t num, uint32_t statis_id, bool isNty = true); //
	int add_xuejing(uint32_t num, uint32_t statis_id, bool isNty = true); //
	int sub_xuejing(uint32_t num, uint32_t statis_id, bool isNty = true); //	
	int add_lingshi(uint32_t num, uint32_t statis_id, bool isNty = true); //
	int sub_lingshi(uint32_t num, uint32_t statis_id, bool isNty = true); //	
	int add_shengwang(uint32_t num, uint32_t statis_id, bool isNty = true); //
	int sub_shengwang(uint32_t num, uint32_t statis_id, bool isNty = true); //	
	
	uint32_t get_coin(void); //获取银票
	uint32_t get_silver(void); //获取银两	
	int add_zhenqi(uint32_t num, uint32_t statis_id, bool isNty = true); //增加真气
	int sub_zhenqi(uint32_t num, uint32_t statis_id, bool isNty = true); //消耗真气
	uint32_t get_zhenqi(void); //获取真气
	void add_guoyu_exp(uint32_t num);
	void add_chengjie_exp(uint32_t num);
//	void add_chengjie_courage(uint32_t num);
	void add_shangjin_exp(uint32_t num);
	void refresh_yaoshi_oneday();
	void refresh_zhenying_task_oneday();
	void refresh_question_oneday();
	void gen_common_question();
	void send_all_yaoshi_num();
	void add_zhenying_exp(uint32_t num);
	void send_zhenying_info();
	void clear_zhenying_task();
	uint32_t get_zhenying_grade(void);
	int add_currency(uint32_t id, uint32_t num, uint32_t statis_id, uint32_t limit = UINT32_MAX, bool isNty = true);
	int sub_currency(uint32_t id, uint32_t num, uint32_t statis_id, bool isNty = true);

	//背包
	void item_find_pos_by_cache(uint32_t id, std::vector<uint32_t>& pos_list); //通过id查找pos
	void add_item_pos_cache(uint32_t id, uint32_t pos);
	void del_item_pos_cache(uint32_t id, uint32_t pos);
	void create_item_cache(void);
	void fit_bag_grid_num(void);

	int check_can_transfer();

	uint32_t set_item_cd(ItemsConfigTable *config);	
	int check_item_cd(ItemsConfigTable *config);
	int check_can_add_item(uint32_t id, uint32_t num, std::map<uint32_t, uint32_t> *out_add_list); //检查背包空间
	bool check_can_add_item_list(std::map<uint32_t, uint32_t>& item_list);
	int add_item(uint32_t id, uint32_t num, uint32_t statis_id, bool isNty = true); //增加道具
	bool add_item_list(std::map<uint32_t, uint32_t>& item_list, uint32_t statis_id, bool isNty = true); //增加一堆道具，如果背包不足，会失败
	bool add_item_list_as_much_as_possible(std::map<uint32_t, uint32_t>& item_list, uint32_t statis_id, bool isNty = true); //增加一堆道具，尽可能放入背包，满了丢弃
	bool add_item_list_otherwise_send_mail(std::map<uint32_t, uint32_t>& item_list, uint32_t statis_id, uint32_t mail_id, std::vector<char *> *mail_args = NULL, bool isNty = true); //增加一堆道具，背包满后发邮件
	int get_item_num_by_id(uint32_t id); //获取指定id道具数量
	int get_item_can_use_num(uint32_t id); //获取绑定+非绑定道具数量
	int del_item_grid(uint32_t pos, bool isNty = true);
	int del_item_by_pos(uint32_t pos, uint32_t num, uint32_t statis_id, bool isNty = true); //消耗指定格子道具
	int del_item_by_id(uint32_t id, uint32_t num, uint32_t statis_id, bool isNty = true); //消耗指定id道具
	int del_item(uint32_t id, uint32_t num, uint32_t statis_id, bool isNty = true); //消耗道具，先消耗绑定的
	int merge_item(uint32_t id); //将非绑定道具变成绑定道具
	int stack_item(uint32_t id); //调整多个格子堆叠数量
	void tidy_bag(void);
	int try_use_prop(uint32_t pos, uint32_t use_all, ItemUseEffectInfo *info);
	int use_prop_effect(bag_grid_data& grid, ItemsConfigTable *config, uint32_t use_count, ItemUseEffectInfo *info);
	int use_prop(uint32_t pos, uint32_t use_all, ItemUseEffectInfo *info);
	void update_bag_grid(uint32_t pos);
	bool is_item_expire(uint32_t pos); //判断道具是否到期
	void check_bag_expire(bool isNty = false);

	//将已有道具从其他系统移动到背包里
	int move_baguapai_to_bag(BaguapaiCardInfo &card);
	int move_fabao_to_bag(partner_cur_fabao &fabao);
	int move_trade_item_to_bag(uint32_t item_id, uint32_t num, EspecialItemInfo &especial);
	//道具飞向背包效果
	void noitfy_item_flow_to_bag(std::map<uint32_t, uint32_t> &item_list);
	void notify_one_item_flow_to_bag(uint32_t id, uint32_t num);

		//ai服务器消息发送
	void send_player_enter_to_aisrv();
	void send_msgid_to_aisrv(uint16_t msgid);
	void send_player_attr_to_aisrv();
	void send_player_move_to_aisrv(MoveNotify *nty);
	void send_player_move_start_to_aisrv(MoveStartNotify *nty);
	void send_player_move_stop_to_aisrv(MoveStopNotify *nty);
	void send_player_sight_add_to_aisrv(player_struct *add_player);

		//
	void deal_skill_cast_request(SkillCastRequest *req, uint32_t ori_skill_id, SkillTable *config, struct ActiveSkillTable *active_config);
	void deal_skill_hit_request(SkillHitRequest *req);	

	//掉落
	bool give_drop_item(uint32_t drop_id, uint32_t statis_id, AddItemDealWay deal_way, bool isNty = true, uint32_t mail_id = 0, std::vector<char *> *mail_args = NULL); //发放掉落奖励

	//经验
	int add_exp(uint32_t val, uint32_t statis_id, bool isNty = true);
	int deal_level_up(uint32_t level_old, uint32_t level_new);
	int get_total_exp(void);
	uint32_t get_up_to_level_exp(uint32_t level); //角色从当前等级经验升级到指定等级所需经验值
	int sub_exp(uint32_t val, uint32_t statis_id, bool isNty = true); //减少经验

	double get_exp_rate(void);
	double get_coin_rate(void);		

	//头像
	int add_head_icon(uint32_t icon_id);
	int init_head_icon(void);
	HeadIconInfo *get_head_icon(uint32_t icon_id);
	void check_head_condition(uint32_t condition_id, uint32_t condition_val);
	uint32_t get_head_num(void);

	//任务
	bool check_task_accept_condition(uint32_t type, uint32_t target, uint32_t val);
	bool check_task_accept_condition_by_id(uint32_t id);
	bool task_is_finish(uint32_t task_id);
	int task_is_acceptable(uint32_t task_id);
	TaskInfo *get_task_info(uint32_t task_id);
	int add_task(uint32_t task_id, uint32_t status, bool isNty = false);
	void task_update_notify(TaskInfo *info);
	void check_task_collect(TaskInfo *info); //是否要增加 删除采集点
	void get_task_event_item(uint32_t task_id, uint32_t event_class, std::map<uint32_t, uint32_t> &item_list);
	void get_task_event_partner(uint32_t task_id, uint32_t event_class);
	int touch_task_event(uint32_t task_id, uint32_t event_class);
	int execute_task_event(uint32_t event_id, uint32_t event_class, bool internal, uint32_t task_id);
	int add_finish_task(uint32_t task_id);
	int del_finish_task(uint32_t task_id);
	int submit_task(uint32_t task_id);
	void get_task_reward_item(uint32_t task_id, std::map<uint32_t, uint32_t> &item_list);
	int give_task_reward(uint32_t task_id);
	int give_task_reward_by_reward_id(uint32_t reward_id, uint32_t statis_id);
	int touch_task_drop(uint32_t scene_id, uint32_t monster_id);
	void load_task_end(void);
	void remove_task(uint32_t task_id);

	void init_task_progress(TaskInfo *info);
	void add_task_progress(uint32_t type, uint32_t target, uint32_t num, uint32_t task_id = 0, uint32_t cond_id = 0, uint64_t teammate_id = 0);
	bool task_is_achieved(TaskInfo *info);
	int set_task_fail(TaskInfo *info);
	int send_task_submit(uint32_t task_id);
	static bool task_condition_can_fail(uint32_t task_id);

	void logout_check_task_time(void);
	void logout_check_award_question(void);
	void login_check_task_time(void);
	void check_task_time(void);
	uint32_t get_task_expire_time(TaskInfo *info);

	int accept_task(uint32_t task_id, bool check_condition = true);
	void clear_team_task(void); //清除整个队伍共享的任务
	void task_remove_notify(uint32_t task_id);
	void task_finish_add_notify(uint32_t task_id);
	void task_finish_del_notify(uint32_t task_id);
	void leave_team(player_struct *leader);
	void hand_out_team_leader(player_struct *leader); //把组队队长数据移交给新队长
	void clear_guild_task(void);
	void on_leave_guild(void);
	int get_guild_build_task(void);

	int get_task_chapter_info(uint32_t &id, uint32_t &state);
	void update_task_chapter_info(void);
	uint32_t get_task_chapter_id(void);

	bool is_task_event_execute(uint32_t event_id);
	void insert_task_event_execute(uint32_t event_id);
	void clear_task_event_execute(void);
	void add_task_planes_unit(uint32_t refresh_id, uint64_t unit_uuid);
	void del_task_planes_unit(uint32_t refresh_id, uint32_t type);
	void clear_task_planes_uints(void);
	void on_leave_sight_space(sight_space_struct *sight_space);

	uint32_t get_travel_task(void);
	void notify_travel_task_info(void);
	int generate_next_travel_task(uint32_t pre_task = 0);
	void on_travel_task_finish(uint32_t pre_task);

	void do_taunt_action();
	void update_region_id();

	void on_region_changed(uint16_t old_region_id, uint16_t new_region_id);

	//吟唱
	void sing_notify(uint32_t msg_id, uint32_t type, uint32_t time, bool broadcast, bool include_myself);
	int begin_sing(uint32_t type, uint32_t time, bool broadcast, bool include_myself, void *args);
	int interrupt_sing(void);
	int end_sing(void);

	//系统提示
	void send_system_notice(uint32_t id, std::vector<char*> *args);
	static void send_rock_notice(player_struct &player, uint32_t notify_id);

	int conserve_out_raid_pos_and_scene(struct DungeonTable* m_config); //进入副本前保存离开副本后所到的场景id以及位置
	int set_enter_raid_pos_and_scene(raid_struct *raid, double pos_x, double pos_z);	
	int set_out_raid_pos_and_clear_scene();
	int set_out_raid_pos();
	int on_leave_raid();

	//时装
	int add_fashion(uint32_t id , uint32_t color, time_t expire);
	void set_fashion_old(uint32_t id);
	void set_fashion_color(uint32_t id, uint32_t color, bool down);
	void unlock_color(uint32_t color);
	void unlock_weapon_color(uint32_t color);
	int get_fashion(uint32_t id);
	int get_color(uint32_t color);
	int get_weapon_color(uint32_t color);
	void check_fashion_expire();
	void open_new_fashion(uint32_t level_old, uint32_t level_new);


	//坐骑
	int add_horse(uint32_t id, time_t expire);
	void notify_add_horse(int i);
	int get_horse(uint32_t id);
	int get_on_horse_id();
	void unpack_horse(_PlayerDBInfo *db_info);
	void calc_horse_attr(double *attr);
	void calc_horse_attr();
	void check_horse_expire();
	void check_guoyu_expire();
	bool go_down_cash_truck();
	void down_horse();
	uint32_t get_horse_num();
	void init_horse();

	//装备
	EquipInfo *get_equip(uint32_t type);
	int add_equip(uint32_t type, uint32_t statis_id);
	int add_equip_exp(uint32_t type, uint32_t val);
	bool equip_is_max_star(uint32_t type);
	uint32_t get_equip_max_star_need_exp(uint32_t type); //升到当前最大星需要的经验
	void update_weapon_skin(bool isNty); //更新武器外形
	int get_equip_inlay_quality_num(uint32_t quality, uint32_t quality_num); //获取镶嵌N颗指定品质宝石的装备数量
	uint32_t get_equip_num(void);
	uint32_t get_equip_enchant_color_num(uint32_t color); //获取附魔属性全为color色的装备数量

		//pvp副本
	int change_pvp_raid_score(int type, int value);
	int add_today_pvp_win_num(int type);
	int send_pvp_raid_score_changed(int type);
	uint32_t pvp_raid_cancel_time;  //pvp副本匹配CD

		//清除类型3(变身)buff
//	bool is_in_buff3();
//	void clear_type3_buff();
//	void clear_god_buff();
//	void clear_one_buff(uint32_t id);	
	uint32_t add_murder_num(uint32_t num); //添加杀戮值, 返回修改后的杀戮值
	uint32_t sub_murder_num(uint32_t num);	//减少杀戮值, 返回修改后的杀戮值

	void add_pet(monster_struct *pet);
	void del_pet(monster_struct *pet);

	//邮件
	int send_mail(uint32_t type, char *title = NULL, char *sender_name = NULL, char *content = NULL, std::vector<char *> *args = NULL, std::map<uint32_t, uint32_t> *attachs = NULL, uint32_t statis_id = 0); //发邮件，注意一封邮件最多带6种附件
	int send_mail_by_id(uint32_t type, std::vector<char *> *args = NULL, std::map<uint32_t, uint32_t> *attachs = NULL, uint32_t statis_id = 0);

	//御气道
	int init_yuqidao_mai(uint32_t break_id, bool isNty); //通过冲脉激活新经脉
	YuqidaoMaiInfo *get_yuqidao_mai(uint32_t mai_id);
	int init_yuqidao_break(uint32_t break_id); //经脉升满激活冲脉
	YuqidaoBreakInfo *get_yuqidao_break(uint32_t break_id);
	bool yuqidao_mai_is_finish(uint32_t mai_id);

	//八卦牌
	BaguapaiDressInfo *get_baguapai_dress(uint32_t style_id);
	BaguapaiCardInfo *get_baguapai_card(uint32_t style_id, uint32_t part_id);
	int generate_baguapai_minor_attr(uint32_t card_id, CommonRandAttrInfo *attrs, uint32_t type);
	int generate_baguapai_additional_attr(uint32_t card_id, CommonRandAttrInfo *attrs);
	int get_bagua_suit_id(BaguapaiDressInfo *info);
	int get_bagua_min_star(BaguapaiDressInfo *info);

	//活动
	int add_activeness(uint32_t num, uint32_t statis_id, bool isNty = true);
	uint32_t get_activeness(void);
	bool activity_is_unlock(uint32_t act_id);
	int check_activity_progress(uint32_t matter, uint32_t value);
//	int activity_finish_check_chivalry(uint32_t chivalry_id);
//	int add_chivalry(uint32_t num, uint32_t statis_id, bool isNty = true);
//	int sub_chivalry(uint32_t num, uint32_t statis_id, bool isNty = true);
//	uint32_t get_chivalry(void);
	void update_daily_activity_item(DailyActivityInfo *info);
	void update_chivalry_activity_item(ChivalryActivityInfo *info);
	void notify_activity_info(EXTERN_DATA *extern_data);

	void add_wanyaoka(uint32_t *id, uint32_t n_id);
	void clear_award_question();
	

	//帮会
	void add_guild_resource(uint32_t type, uint32_t num);
	void sub_guild_building_time(uint32_t time);
	void disband_guild(uint32_t guild_id);
	uint32_t get_guild_skill_level_num(uint32_t level);

	//护送
	int start_escort(uint32_t escort_id);
	int stop_escort(uint32_t escort_id, bool success);
	void stop_all_escort(void);
	EscortInfo *get_escort_info(uint32_t escort_id);
	int clear_escort_by_id(uint32_t escort_id);
	int clear_escort_by_index(uint32_t idx);
	void clear_all_escort(void);
	void check_escort(void);

	//伙伴
	partner_struct *get_battle_partner();
	int add_partner(uint32_t partner_id, uint64_t *uuid = NULL);
	int add_partner_dictionary(uint32_t partner_id);
	int remove_partner(uint64_t partner_uuid);
	void clear_all_partners(void);
	partner_struct *get_partner_by_uuid(uint64_t partner_uuid);
	bool partner_is_in_formation(uint64_t partner_uuid);
	bool partner_is_in_battle(uint64_t partner_uuid);
	void load_partner_end(void);
	bool has_owned_partner(uint32_t partner_id);
	bool is_partner_battle(void); //伙伴是否参战
	bool is_partner_precedence(void); //主战伙伴是否优先出战
	int add_partner_to_scene(uint64_t partner_uuid); //把伙伴加入到玩家所在的场景
	int add_all_formation_partner_to_scene();	//把所有布阵中的伙伴加入场景
//	int del_all_formation_partner_from_scene(bool send_msg);	//把所有布阵中的伙伴移出场景
	int del_partner_from_scene(uint64_t partner_uuid, bool send_msg);
	int del_partner_from_scene(partner_struct *partner, bool send_msg);
	void del_battle_partner_from_scene();  //临时把所有出战伙伴移出场景(副本结束的时候用)
	void take_truck_into_scene(void);	//角色进入场景后，把镖车加入场景
	void take_partner_into_scene(void); //角色进入场景后，把伙伴加入场景
	void take_partner_into_sight_space(void);	 //角色进入位面后，把伙伴加入位面
	void take_partner_out_sight_space(sight_space_struct *sp);   //角色离开位面后，把伙伴拉出位面
	void take_truck_into_sight_space(void);	 //角色进入位面后，把镖车加入位面	
	void take_truck_out_sight_space(sight_space_struct *sp);  //角色离开位面后，把镖车拉出位面

	void stop_partner_ai();
	void start_partner_ai();		
	void adjust_battle_partner(void); //阵型变化，调整出战的伙伴
	void add_battle_partner(int index); //在index上出战一个伙伴
	void sub_battle_partner(int index); //把index的出战伙伴收回
	uint64_t get_next_can_battle_partner(void); //获取能出战的伙伴
	uint64_t get_fighting_partner(void);
	void on_enter_scene(scene_struct *new_scene);
	void on_leave_scene(scene_struct *old_scene);
	int add_partner_anger(uint32_t num, bool isNty = true);
	int reset_partner_anger(bool isNty = true);
	int add_partner_exp(uint32_t num, uint32_t statis_id, bool isNty = true);
	void notify_fighting_partner(void);
	void check_partner_relive(void);
	bool partner_dictionary_is_active(uint32_t partner_id); //伙伴图鉴是否激活
	bool partner_bond_is_active(uint32_t bond_id); //伙伴羁绊是否激活
	bool partner_bond_reward_is_get(uint32_t partner_id); //伙伴羁绊奖励是否领取
	int get_partner_fabao_main_attr(uint32_t card_id, AttrInfo &attr_val);
	int get_partner_fabao_minor_attr(uint32_t card_id, AttrInfo *attrs);
	uint32_t get_partner_quality_num(uint32_t quality);
	uint32_t get_partner_battle_num(void);

	bool is_too_high_to_beattack();   //是否飞的太高不能被攻击
	void calc_partner_pos(struct position *pos, float distance);

	//好友
	uint32_t get_friend_num(void);
	uint32_t get_friend_close_num(uint32_t close_lv);
	bool is_friend_enemy(uint64_t target_id);
	bool is_friend_contacts(uint64_t target_id);
	uint32_t get_friend_closeness(uint64_t target_id);
	void clean_player_friend_closeness(uint64_t target_id);

	bool get_rank_ranking(uint32_t rank_type, uint32_t rank_lv, uint32_t rank_score);

	//成就
	void load_achievement_end(void);
	void init_achievement_progress_internal(uint32_t &progress, uint32_t type, uint32_t config_target1, uint32_t config_target2, uint32_t config_target3);
	void init_achievement_progress(AchievementInfo *info);
	void add_achievement_progress_internal(uint32_t &progress, uint32_t type, uint32_t config_target1, uint32_t config_target2, uint32_t config_target3, uint32_t target1, uint32_t target2, uint32_t target3, uint32_t num);
	void add_achievement_progress(uint32_t type, uint32_t target1, uint32_t target2, uint32_t target3, uint32_t num);
	AchievementInfo *get_achievement_info(uint32_t id);
	void achievement_update_notify(AchievementInfo *info);

	//称号
	TitleInfo *get_title_info(uint32_t id);
	int add_title(uint32_t title_id, uint32_t keep_time = 0);
	int expire_title(uint32_t title_id);
	int sub_title(uint32_t title_idx);
	void title_update_notify(TitleInfo *info);
	void check_title_expire(void);
	void check_title_condition(uint32_t type, uint32_t target1, uint32_t target2);

	//英雄挑战信息初始化
	int init_hero_challenge_data();
	//地宫试炼任务信息推送
	int mijing_shilian_info_notify(uint32_t type);

		//功能是否已经开启
	bool is_function_unlock(uint32_t id);

	//我要变强
	void load_strong_end(void);
	void init_strong_goal_progress(StrongGoalInfo *info);
	void add_strong_goal_progress(uint32_t type, uint32_t target1, uint32_t target2, uint32_t target3, uint32_t num);
	void strong_goal_update_notify(StrongGoalInfo *info);
	StrongGoalInfo *get_strong_goal_info(uint32_t goal_id);
	void add_strong_chapter_progress(uint32_t chapter_id);
	void strong_chapter_update_notify(StrongChapterInfo *info);
	StrongChapterInfo *get_strong_chapter_info(uint32_t chapter_id);
	void check_strong_chapter_open(uint32_t old_lv, uint32_t new_lv);
	bool strong_function_open(void);

		//能否增加buff层数
	bool can_add_buff_lv(uint32_t buff_id);
	float get_buff_effect_rate(uint32_t effect_id);	
	
	//钓鱼换装
	void add_fishing_buff(void);
	void del_fishing_buff(void);

	//等级奖励
	int init_player_level_reward_data();
	int player_level_reward_info_notify();

	//在线奖励
	int init_online_reward_data();
	int player_online_reward_info_notify();
	int refresh_player_online_reward_info();

	//每日签到奖励
	int player_signin_reward_info_notify();
	int init_player_signin_leiji_reward_data();
	//每日更新签到奖励信息
	void refresh_player_signin_info_every_day(uint64_t befor_day_time);
	//每月更新签到奖励信息
	int refresh_player_signin_info_every_month(uint64_t befor_day_time);
	//活跃度达到要求加补签次数
	int player_huo_yue_du_add_sign_in_num(uint32_t befor_huoyue, uint32_t now_huoyue);

	//活动奖励找回信息通知
	int player_active_zhaohui_reward_info_notify();
	int refresh_player_reward_back_info(uint64_t befor_time);

	//摇钱树信息更新
	void refresh_yao_qian_shu_data(uint64_t befor_time);
	void player_yaoqian_shu_info_notify();
	bool count_yaoqian_shu_money(int num, uint32_t *all, uint32_t *next);

	//玩家登录奖励信息
	int init_player_login_reward_receive_data();
	//登录奖励信息通知前端
	int player_login_reward_info_notify();
	int refresh_player_login_reward_info(uint64_t befor_time, bool up_level);

	int open_function(std::vector<uint32_t> &func_ids);

	//赐福奖励
	int init_player_ci_fu_reward_receive_data();
	int player_ci_fu_info_notify();

	//门宗传功切场景处理
	void guild_chuan_gong_deal_with();
	//判断当前是否正在传功
	bool is_in_guild_chuan_gong();
	//传功中断
	void guild_chuan_gong_zhong_duan();
	//清除传功信息
	void clean_guild_chuan_gong_info();
	//更新传功信息
	void refresh_guild_chuan_gong_info();
	//通知玩家当前是否是帮会入侵活动期间
	void guild_ruqin_activity_notify();
	//九宫八卦奖励信息初始化
	void jiu_gong_ba_gua_reward_info_init();
	//九宫八卦奖励信息通知
	void jiu_gong_ba_gua_reward_info_notify();
	//九宫八卦任务完成处理
	void finish_jiu_gong_bagua_task(uint32_t task_id);
	//求婚相关检查
	int player_propose_check_up(uint64_t& player_id, uint32_t &ring_type);
	//当前是否是求婚状态
	bool is_on_propose();
	//清除订婚状态信息
	void clean_propose_state_and_info();
	//求婚成功或者失败处理
	void player_propose_end_deal_with(bool is_success);
	//玩家当前情缘数据通知
	void player_cur_marry_info_notify();
	//清除玩家所有婚姻信息(is_divorce:是否是离婚清除)
	void clean_player_all_marry_info(bool is_divorce);

	uint64_t last_change_area_time;
	sight_space_struct *sight_space;
//	raid_struct *m_raid;
	Team *m_team;
	std::set<uint64_t> m_inviter; 
	MySkill m_skill;
	std::set<monster_struct *> m_pet_list;
	std::vector<uint64_t> bagua_buffs;
	std::deque<uint64_t> m_hitMe;
	std::set<uint64_t> m_meHit;
	uint32_t guild_battle_wait_award_time;
	std::map<uint32_t, uint32_t> xun_map_id;  //second: 藏宝图对应的出生点, first:藏宝图ID(物品表)
	PartnerMap m_partners;
	uint32_t chengjie_kill; //悬赏目标被杀
	std::set<uint32_t> m_task_planes_events;
	std::multimap<uint32_t, uint64_t> m_task_planes_units;

private:
	int move_to_wild(uint32_t scene_id);  //进入野外
	int move_to_wild_pos(uint32_t scene_id, double pos_x, double pos_z, double direct); //进入野外
	int move_to_raid(uint32_t raid_id, EXTERN_DATA *extern_data);  //进入副本

	int enter_raidsrv(int raidsrv_id, uint32_t raid_id);	
	
	void calculate_lv2_attribute();
	void calculate_lv3_attribute();
	void calculate_lv4_attribute();	
	void calc_zhenying_attr(double *attr);
	void use_hp_pool_add_hp();
	void enter_fight_state();
	void leave_fight_state();
	void do_auto_add_hp();
	void pack_answer_db(_PlayerDBInfo &db_info);
	void unpack_answer_db(_PlayerDBInfo *db_info);
	int add_pvp_raid_score(int type, int value);
	int sub_pvp_raid_score(int type, int value);
	void on_relive_in_raid(raid_struct *raid, uint32_t type);
	void on_kill_player(player_struct *dead);	
	void send_raid_hit_statis(raid_struct *raid);	
	void try_out_raid();
	void check_qiecuo_range();	
	void del_sight_player_in_area(int n_del, area_struct **del_area, int *delete_player_id_index, uint64_t *delete_player_id);
	void del_sight_monster_in_area(int n_del, area_struct **del_area, int *delete_monster_uuid_index, uint64_t *delete_monster_uuid);
	void del_sight_partner_in_area(int n_del, area_struct **del_area, int *delete_partner_uuid_index, uint64_t *delete_partner_uuid);	
	void del_sight_truck_in_area(int n_del, area_struct **del_area, int *delete_truck_uuid_index, uint64_t *delete_truck_uuid);
	void add_area_player_to_sight(area_struct *area, int *add_player_id_index, SightPlayerBaseInfo *add_player);
	void add_area_monster_to_sight(area_struct *area, int *add_monster_id_index, SightMonsterInfo *add_monster);
	void add_area_collect_to_sight(area_struct *area, int *add_collect_id_index, SightCollectInfo *add_collect);
	void add_area_truck_to_sight(area_struct *area, int *add_truck_id_index, SightCashTruckInfo *add_truck);
	void add_area_partner_to_sight(area_struct *area, int *add_partner_id_index, SightPartnerInfo *add_partner);	
//	void add_area_player_to_sight(area_struct *area, int *add_player_id_index, uint64_t *add_player_id);	
	uint32_t get_first_skill_id();
	uint64_t reset_pvp_raid_level(uint16_t score, uint8_t *level, uint8_t *star);	
	void update_rtt(uint32_t stamp);
	int64_t srtt;       //网络延迟 * 8
};

void init_sight_unit_info_point();
int get_exp_notice_id(uint32_t statis_id);
int get_coin_notice_id(uint32_t statis_id);
int get_partner_exp_notice_id(uint32_t statis_id);
int check_qiecuo_finished(player_struct *p1, player_struct *p2);
#endif
