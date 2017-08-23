#ifndef _SERVER_PROTO_H__
#define _SERVER_PROTO_H__

#include <stdint.h>
#include "comm_define.h"
#include "attr_id.h"
#pragma pack(1)
enum SERVER_PROTO
{
	SERVER_PROTO_BROADCAST = 1000,
	SERVER_PROTO_BROADCAST_ALL = 1001,
	
	SERVER_PROTO_LOGIN,							//帐号登录,login srv 通知conn srv放行
	SERVER_PROTO_ENTER_GAME_REQUEST,			//进入游戏请求，login_srv --> conn_srv --> game_srv --> db_srv
	SERVER_PROTO_ENTER_GAME_ANSWER,				//选择角色进入游戏, db_srv --> game_srv
	SERVER_PROTO_SAVE_PLAYER,					//保存玩家信息

	SERVER_PROTO_FIND_PLAYER_REQUEST,     //
	SERVER_PROTO_FIND_PLAYER_ANSWER,     //
	SERVER_PROTO_ADD_CHENGJIE_REQUEST,     //
	SERVER_PROTO_ADD_CHENGJIE_ANSWER,     //
	SERVER_PROTO_DEL_CHENGJIE_REQUEST,     //
	SERVER_PROTO_LOAD_CHENGJIE_REQUEST,     //
	SERVER_PROTO_LOAD_CHENGJIE_ANSWER,     //
	SERVER_PROTO_UPDATE_CHENGJIE_REQUEST,     //
	SERVER_PROTO_CHENGJIE_TASK_COMPLETE_REQUEST,     //
	SERVER_PROTO_LOAD_SERVER_LEVEL_REQUEST,     //加载服务器等级请求
	SERVER_PROTO_LOAD_SERVER_LEVEL_ANSWER,      //加载服务器等级应答
	SERVER_PROTO_SAVE_SERVER_LEVEL_REQUEST,     //保存服务器等级请求
	SERVER_PROTO_BREAK_SERVER_LEVEL_REQUEST,     //突破服务器等级请求
	SERVER_PROTO_BREAK_SERVER_LEVEL_ANSWER,      //突破服务器等级应答


	SERVER_PROTO_PLAYER_ONLINE_NOTIFY,			// 用户上线通知
	SERVER_PROTO_KICK_ROLE_NOTIFY,				//踢人
	SERVER_PROTO_KICK_ROLE_ANSWER,				//重新选角踢人，game_srv --> conn_srv
	SERVER_PROTO_RENAME_ANSWER,				    //改名应答，db_srv --> game_srv

	SERVER_PROTO_GAME_TO_FRIEND,     //游戏服通过网关转好友服
	SERVER_PROTO_FRIEND_TO_GAME,     //好友服通过网关转游戏服
	SERVER_PROTO_REFRESH_PLAYER_REDIS_INFO,     //更新redis缓存用户信息
	SERVER_PROTO_CHENGJIE_MONEY_BACK,     //退回惩戒赏金
	SERVER_PROTO_FRIEND_CHAT, //好友聊天
	SERVER_PROTO_FRIEND_ADD_ENEMY, //好友服增加仇人
	SERVER_PROTO_FRIEND_RECOMMEND, //好友推荐
	SERVER_PROTO_FRIEND_EXTEND_CONTACT_REQUEST, //好友扩展联系人上限 game_srv --> friend_srv
	SERVER_PROTO_FRIEND_EXTEND_CONTACT_ANSWER, //好友扩展联系人上限 friend_srv --> game_srv
	SERVER_PROTO_FRIENDSRV_COST_REQUEST, //好友服扣除消耗请求
	SERVER_PROTO_FRIENDSRV_COST_ANSWER, //好友服扣除消耗应答
	SERVER_PROTO_FRIEND_GIFT_COST_REQUEST, //送礼消耗请求
	SERVER_PROTO_FRIEND_GIFT_COST_ANSWER, //送礼消耗返回
	SERVER_PROTO_FRIEND_TURN_SWITCH, //翻转好友申请开关
	SERVER_PROTO_FRIEND_SYNC_RENAME, //玩家改名同步
	SERVER_PROTO_FRIEND_SYNC_FRIEND_NUM, //同步好友数
	SERVER_PROTO_FRIEND_ADD_GIFT, //好友送礼接收
	SERVER_PROTO_FRIEND_SEND_GIFT_SUCCESS, //好友送礼成功

	SERVER_PROTO_GAMESRV_START,                 //游戏服启动通知
	SERVER_PROTO_MAIL_INSERT,                   //插入新邮件
	SERVER_PROTO_MAIL_GIVE_ATTACH_REQUEST,      //邮件发放邮件附件请求 mail_srv --> game_srv
	SERVER_PROTO_MAIL_GIVE_ATTACH_ANSWER,       //邮件发放邮件附件应答 game_srv --> mail_srv

	SERVER_PROTO_ADD_WANYAOKA,       //添加万妖卡信息	
	SERVER_PROTO_SAVE_WANYAOKA,       //保存万妖卡信息
	SERVER_PROTO_LIST_WANYAOKA,       //读取万妖卡信息	

	SERVER_PROTO_GUILDSRV_COST_REQUEST, //帮会服扣除消耗请求
	SERVER_PROTO_GUILDSRV_COST_ANSWER, //帮会服扣除消耗应答
	SERVER_PROTO_GUILDSRV_REWARD_REQUEST, //帮会服发放奖励请求
	SERVER_PROTO_GUILDSRV_REWARD_ANSWER,  //帮会服发放奖励应答
	SERVER_PROTO_ADD_GUILD_RESOURCE, //增加帮会资源
	SERVER_PROTO_SUB_GUILD_BUILDING_TIME, //减少帮会建筑升级时间
	SERVER_PROTO_SYNC_GUILD_SKILL, //同步帮会技能
	SERVER_PROTO_SYNC_GUILD_INFO, //同步帮会信息
	SERVER_PROTO_GUILD_CHAT, //帮会聊天
	SERVER_PROTO_GUILD_DISBAND, //帮会解散
	SERVER_PROTO_GM_DISBAND_GUILD, //帮会解散
	SERVER_PROTO_GUILD_BATTLE_REWARD, //帮战奖励
	SERVER_PROTO_GUILD_BATTLE_ENTER_WAIT, //帮战进入准备区
	SERVER_PROTO_GUILD_ANSWER_AWARD, //帮会答题奖励
	SERVER_PROTO_GUILD_BATTLE_BEGIN, //帮战开始
	SERVER_PROTO_GUILD_BATTLE_END, //帮战结束
	SERVER_PROTO_GUILD_BATTLE_SETTLE, //帮战结算
	SERVER_PROTO_GUILD_BATTLE_FINAL_LIST_REQUEST, //帮战决赛参赛名单请求
	SERVER_PROTO_GUILD_BATTLE_FINAL_LIST_ANSWER, //帮战决赛参赛名单应答
	SERVER_PROTO_GUILD_ADD_FINAL_BATTLE_GUILD, //增加帮战决赛参赛名单
	SERVER_PROTO_GUILD_PRODUCE_MEDICINE, //炼药
	SERVER_PROTO_GUILD_SYNC_DONATION, //同步帮贡
	SERVER_PROTO_GUILD_SKILL_LEVEL_UP, //帮会技能升级成功

	SERVER_PROTO_CHOSE_ZHENYING_REQUEST, //加入阵营
	SERVER_PROTO_CHANGE_ZHENYING_REQUEST, //改变阵营
	SERVER_PROTO_ADD_ZHENYING_KILL_REQUEST, //
	SERVER_PROTO_ZHENYING_CHANGE_POWER_REQUEST, //

	//排行榜服消息
	SERVER_PROTO_RANK_SYNC_RANK,     //同步排名

	SERVER_PROTO_UNDO_COST, //通知game_srv操作失败，把扣取的资源还回去

	SERVER_PROTO_DOUFACHANG_CHALLENGE_REQUEST,  //挑战  doufachang -> game_srv
	SERVER_PROTO_DOUFACHANG_CHALLENGE_ANSWER,  //挑战	game_srv -> doufachang
	SERVER_PROTO_DOUFACHANG_ADD_REWARD_REQUEST,  //发奖励  doufachang -> game_srv 
	SERVER_PROTO_DOUFACHANG_ADD_REWARD_ANSWER,  //发奖励   game_srv -> doufachang 	
	SERVER_PROTO_DOUFACHANG_LOAD_PLAYER_REQUEST,  //加载玩家数据  game_srv -> db_srv 
	SERVER_PROTO_DOUFACHANG_LOAD_PLAYER_ANSWER,  //加载玩家数据   db_srv -> game_srv
	SERVER_PROTO_DOUFACHANG_BUY_CHALLENGE_REQUEST,   //扣元宝  doufachang -> game_srv
	SERVER_PROTO_DOUFACHANG_BUY_CHALLENGE_ANSWER,   //扣元宝   game_srv -> doufachang

	SERVER_PROTO_GET_OFFLINE_CACHE_REQUEST = 8000, //获取玩家离线缓存请求 game_srv --> friend_srv
	SERVER_PROTO_GET_OFFLINE_CACHE_ANSWER = 8001,  //获取玩家离线缓存应答 friend_srv --> game_srv
	SERVER_PROTO_CLEAR_OFFLINE_CACHE = 8002,       //清除玩家离线缓存 game_srv --> friend_srv
	SERVER_PROTO_INSERT_OFFLINE_CACHE = 8003,      //插入玩家离线缓存 game_srv --> friend_srv

	SERVER_PROTO_GET_USER_INFO = 9033,          // 后台管理端获取用户信息
	SERVER_PROTO_ADD_ITEM_REQUESRT = 9034,		// 后台管理端增加物品
	SERVER_PROTO_RELOAD_CONFIG_REQUEST,			// game srv重读配置
	SERVER_PROTO_RELOAD_ACTIVE_REQUEST,			// active srv重读配置
	SERVER_PROTO_RELOAD_ACTIVE_ANSWER,			// active srv重读配置的回复
	SERVER_PROTO_GET_OFFLINE_RECHARGE_REQUEST,	// 获取离线充值请求
	SERVER_PROTO_OFFLINE_RECHARGE_LIST_REQUEST,	// 获取离线充值列表请求
	SERVER_PROTO_OFFLINE_RECHARGE_LIST_ANSWER,	// 获取离线充值列表应答

	SERVER_PROTO_WORDBOSS_PLAYER_REDIS_INFO,  // 玩家世界boss数据存redis

	SERVER_PROTO_TIREN_LIST_NOTIFY,				// 外挂踢人
};

//除了broadcast的所有消息，第一个字段都是PROTO_HEAD, 最后一个字段都是EXTERN_DATA

typedef struct st_proto_head
{
	uint32_t len;		//长度
	uint16_t msg_id;	//消息ID
	uint16_t seq;		//序号
//	uint32_t crc;		//crc校验
	char data[0];		//PROTO 内容
} PROTO_HEAD;

typedef struct proto_head_conn_broadcast
{
	uint32_t len;
	uint16_t msg_id;
	uint16_t seq;
	uint16_t num_player_id;
	PROTO_HEAD proto_head;
	uint64_t player_id[0];
} PROTO_HEAD_CONN_BROADCAST;

//server之间外带的数据
typedef struct extern_data   
{
	uint64_t player_id;
	uint32_t open_id;
	uint16_t fd;
	uint16_t port;	
} EXTERN_DATA;

typedef struct proto_role_login
{
	PROTO_HEAD head;
	uint32_t result;
	uint16_t login_seq;
} PROTO_ROLE_LOGIN;

typedef struct proto_save_player_req
{
	PROTO_HEAD head;	
	uint16_t level;
	uint32_t plug;
	uint16_t data_size;
	char     name[50];
	uint32_t again; //是否重新选角
	uint64_t chengjie_cd;
	uint8_t data[0];
} PROTO_SAVE_PLAYER_REQ;

typedef struct proto_save_player_resp
{
	PROTO_HEAD head;
	uint32_t again; //是否重新选角
	uint32_t result;
} PROTO_SAVE_PLAYER_RESP;

typedef struct proto_role_kick
{
	PROTO_HEAD head;
	uint32_t reason;
	uint32_t again; //是否重新选角
	EXTERN_DATA extern_data; //要放到最后
} PROTO_ROLE_KICK;

typedef struct proto_enter_game_req
{
	PROTO_HEAD	head;
	uint64_t	player_id;
	uint32_t	reconnect;
} PROTO_ENTER_GAME_REQ;

typedef struct proto_enter_game_resp
{
	PROTO_HEAD head;
	uint8_t reconnect;
	uint8_t job;
	char name[MAX_PLAYER_NAME_LEN];
	uint16_t lv;
	uint64_t player_id;
	uint8_t avata_id;
	uint32_t platform;
	uint32_t ad_channel;
	uint32_t plug;
	uint32_t create_time;
	uint32_t logout_time;
	char    open_id[50];
	char    channel[50];
	uint64_t chengjie_cd;
	uint32_t guild_id;
	uint16_t data_size; //数据库blob最大64K
	uint8_t data[0];
} PROTO_ENTER_GAME_RESP;

typedef struct player_rename_db_answer
{
	PROTO_HEAD head;
	uint32_t result;
	char name[MAX_PLAYER_NAME_LEN];
} PLAYER_RENAME_DB_ANSWER;

typedef struct show_item_to_itemsrv
{
	PROTO_HEAD head;
	uint32_t type;
	uint16_t data_size;
	uint8_t data[0];
} SHOW_ITEM_TO_ITEMSRV;

/*
typedef struct refresh_player_redis_info
{
	PROTO_HEAD head;
	uint64_t player_id;
	uint16_t data_size;
	uint8_t data[0];
} REFRESH_PLAYER_REDIS_INFO;
*/

//插入一封新邮件
typedef struct proto_mail_insert
{
	PROTO_HEAD head;
	uint64_t player_id;
	uint32_t data_size;
	uint8_t data[0];
} PROTO_MAIL_INSERT;

#define MAX_WANYAOKA_EACH_TIME 10
typedef struct proto_add_wanyaoka
{
	uint64_t player_id;
	uint32_t wanyaoka[MAX_WANYAOKA_EACH_TIME];
} PROTO_ADD_WANYAOKA;

typedef struct proto_player_cache_insert
{
	PROTO_HEAD head;
	uint64_t player_id;
	uint32_t type; //cache类型
	uint8_t data[0];
} PROTO_PLAYER_CACHE_INSERT;

typedef struct proto_find_player_req
{
	PROTO_HEAD head;
	char     name[50];
} PROTO_FIND_PLAYER_REQ;
typedef struct proto_find_player_ans
{
	PROTO_HEAD head;
	uint64_t player_id;
	uint64_t lv;
	uint64_t cd;
	char     name[50];
} PROTO_FIND_PLAYER_ANS;
typedef struct proto_add_chengjie
{
	PROTO_HEAD head;
	uint32_t data_size;
	uint8_t data[0];
} PROTO_ADD_CHENGJIE;
typedef struct proto_add_chengjie_ans
{
	PROTO_HEAD head;
	uint32_t taskid;
	uint32_t data_size;
	uint8_t data[0];
} PROTO_ADD_CHENGJIE_ANS;
typedef struct proto_chengjie_id
{
	PROTO_HEAD head;
	uint32_t taskid;
} PROTO_CHENGJIE_ID;

typedef struct srv_cost_info
{
	uint32_t statis_id;
	uint32_t gold; //绑定元宝
	uint32_t unbind_gold; //非绑定元宝
	uint32_t coin;
	uint32_t item_id[10]; //最多携带5个道具
	uint32_t item_num[10];
} SRV_COST_INFO;

typedef struct proto_guildsrv_check_and_cost_req
{
	PROTO_HEAD head;
	SRV_COST_INFO cost;
	uint32_t data_size;
	uint8_t data[0];
} PROTO_GUILDSRV_CHECK_AND_COST_REQ;

typedef struct proto_guildsrv_check_and_cost_res
{
	PROTO_HEAD head;
	uint32_t result;
	SRV_COST_INFO cost;
	uint32_t data_size;
	uint8_t data[0];
} PROTO_GUILDSRV_CHECK_AND_COST_RES;

typedef struct proto_guildsrv_reward_req
{
	PROTO_HEAD head;
	uint32_t statis_id;
	uint32_t gold;
	uint32_t coin;
	uint32_t item_id[5];
	uint32_t item_num[5];
	uint32_t data_size;
	uint8_t data[0];
} PROTO_GUILDSRV_REWARD_REQ;

typedef struct proto_guildsrv_reward_res
{
	PROTO_HEAD head;
	uint32_t statis_id;
	uint32_t result;
	uint32_t data_size;
	uint8_t data[0];
} PROTO_GUILDSRV_REWARD_RES;

typedef struct guild_skill_practice_carry
{
	uint32_t skill_id;
	uint32_t level_num;
	uint32_t need_donation;
} GUILD_SKILL_PRACTICE_CARRY;

typedef struct guild_shop_buy_carry
{
	uint32_t goods_id;
	uint32_t buy_num;
	uint32_t need_donation;
} GUILD_SHOP_BUY_CARRY;

struct ProtoGuildSkill
{
	uint32_t skill_id;
	uint32_t skill_lv;
};

typedef struct proto_sync_guild_skill
{
	PROTO_HEAD head;
	ProtoGuildSkill skills[MAX_GUILD_SKILL_NUM];
} PROTO_SYNC_GUILD_SKILL;

typedef struct proto_sync_guild_info
{
	PROTO_HEAD head;
	uint32_t guild_id;
	uint32_t guild_office;
	char guild_name[300];
} PROTO_SYNC_GUILD_INFO;

typedef struct proto_sync_guild_donation
{
	uint32_t cur_donation;
	uint32_t is_change; //0没变化，1增加，2减少
	uint32_t change_val; //变化值
} PROTO_SYNC_GUILD_DONATION;

typedef struct proto_friend_recommend
{
	PROTO_HEAD head;
	uint32_t player_num;
	uint64_t player_id[MAX_FRIEND_RECOMMEND_PLAYER];
} PROTO_FRIEND_RECOMMEND;

typedef struct proto_cost_friend_gift_req
{
	PROTO_HEAD head;
	uint64_t target_id;
	uint32_t gift_id; //礼物ID
	uint32_t gift_num; //礼物数量
	uint32_t item_id; //对应道具ID
	uint32_t currency_type; //抵消货币类型
	uint32_t currency_val; //抵消货币值
} PROTO_COST_FRIEND_GIFT_REQ;

typedef struct proto_cost_friend_gift_res
{
	PROTO_HEAD head;
	int result;
	uint64_t target_id;
	uint32_t gift_id;
	uint32_t gift_num;
	uint32_t item_id; //对方实际收到道具
	uint32_t add_closeness; //获得的好感度
	SRV_COST_INFO cost; //实际消耗
} PROTO_COST_FRIEND_GIFT_RES;

struct ProtoFriend
{
	uint64_t player_id;
	uint32_t closeness;
};

typedef struct proto_friend_sync_info
{
	PROTO_HEAD head;
	ProtoFriend contacts[MAX_FRIEND_CONTACT_NUM];
} PROTO_FRIEND_SYNC_INFO;

typedef struct proto_friend_sync_rename
{
	PROTO_HEAD head;
	char old_name[MAX_PLAYER_NAME_LEN + 1];
	char new_name[MAX_PLAYER_NAME_LEN + 1];
} PROTO_FRIEND_SYNC_RENAME;

typedef struct proto_guild_disband
{
	PROTO_HEAD head;
	uint32_t guild_id;
	uint32_t player_num;
	char data[0];
} PROTO_GUILD_DISBAND;

//同个帮会的成员的帮战奖励
typedef struct proto_guild_battle_reward
{
	PROTO_HEAD head;
	uint32_t activity_id;
	uint32_t guild_id;
	uint32_t player_num; //奖励人数
	uint64_t player_id[MAX_GUILD_MEMBER_NUM]; //奖励玩家ID
	uint32_t result[MAX_GUILD_MEMBER_NUM]; //战斗结果
	uint32_t score[MAX_GUILD_MEMBER_NUM]; //奖励积分
	uint32_t treasure[MAX_GUILD_MEMBER_NUM]; //奖励资金
	uint32_t donation[MAX_GUILD_MEMBER_NUM]; //奖励帮贡
	uint32_t broadcast_num; //广播人数
	uint64_t broadcast_id[MAX_GUILD_MEMBER_NUM]; //广播玩家ID
} PROTO_GUILD_BATTLE_REWARD;

typedef struct proto_guild_battle_rank
{
	PROTO_HEAD head;
	uint32_t guild_id[4];
} PROTO_GUILD_BATTLE_RANK;

typedef struct proto_guild_battle_settle
{
	PROTO_HEAD head;
	uint32_t activity_id;
	uint32_t broadcast_num; //广播人数
	uint64_t broadcast_id[0]; //广播玩家ID
} PROTO_GUILD_BATTLE_SETTLE;

typedef struct proto_undo_cost
{
	PROTO_HEAD head;
	SRV_COST_INFO cost;
} PROTO_UNDO_COST;

typedef struct proto_zhenyin_change_power
{
//	PROTO_HEAD head;
	int power;
	int zhen_ying;
} PROTO_ZHENYIN_CHANGE_POWER;

struct ProtoRank
{
	uint32_t type; //排行榜类型
	uint32_t rank; //排行榜名次
};

typedef struct proto_sync_rank
{
	ProtoRank ranks[MAX_RANK_TYPE];
} PROTO_SYNC_RANK;

typedef struct doufachang_challenge_request
{
	uint64_t attack;
	uint64_t defence;
} DOUFACHANG_CHALLENGE_REQUEST;
typedef struct doufachang_challenge_answer
{
	uint32_t result;
	uint64_t attack;
	uint64_t defence;
	uint32_t add_gold;
	bool notify;
} DOUFACHANG_CHALLENGE_ANSWER;
typedef struct doufachang_load_player_request
{
	uint64_t player_id;
	uint64_t target_id;
} DOUFACHANG_LOAD_PLAYER_REQUEST;
typedef struct doufachang_load_player_answer
{
	uint64_t player_id;
	uint64_t target_id;
	char name[MAX_PLAYER_NAME_LEN];
	uint16_t lv;
	uint8_t job;	
	uint16_t data_size; //数据库blob最大64K	
	uint8_t data[0];
} DOUFACHANG_LOAD_PLAYER_ANSWER;

typedef struct doufachang_buy_challenge_request
{
	PROTO_HEAD head;	
	uint32_t count;
	uint32_t gold_num;
} DOUFACHANG_BUY_CHALLENGE_REQUEST;

typedef struct doufachang_buy_challenge_answer
{
	PROTO_HEAD head;
	int result;
	uint32_t count;
	uint32_t gold_num;
} DOUFACHANG_BUY_CHALLENGE_ANSWER;

typedef struct doufachang_get_reward_request
{
	uint32_t reward_id;
} DOUFACHANG_GET_REWARD_REQUEST;
typedef struct doufachang_get_reward_answer
{
	uint32_t result;
} DOUFACHANG_GET_REWARD_ANSWER;

#pragma pack() 
#endif

