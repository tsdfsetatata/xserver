#ifndef RAID_H
#define RAID_H
#include <list>
#include "scene.h"
#include "player.h"
#include "monster.h"
#include "team.h"

struct raid_player_info
{
	uint64_t player_id;
	char name[MAX_PLAYER_NAME_LEN + 1];    //名字
	uint32_t headicon;    //头像ID
	uint32_t guild;
	uint16_t lv;
	uint8_t job;
	uint32_t damage;
	uint32_t injured;
	uint32_t cure;
	uint32_t dead_count;
};

enum RAID_STATE_DEFINE
{
	RAID_STATE_START,  //副本开始
	RAID_STATE_PASS,  //副本胜利
//	RAID_STATE_FAIL,  //副本失败
};

#define MAX_ITEM_REWARD_PER_RAID 20
#define MAX_WANYAOGU_RAID_NUM 3
#define MAX_SCRIPT_COND_NUM 5
static const int MAX_ZHENYING_FOLLOW = 8;
#define WANYAOGU_DATA data->ai_data.wanyaogu_data
#define PVP_DATA data->ai_data.pvp_data
#define DOUFACHANG_DATA data->ai_data.doufachang_data
#define SCRIPT_DATA data->ai_data.script_data
#define ZHENYING_DATA data->ai_data.zhenying_data
#define GUILD_DATA data->ai_data.guild_data
#define GUILD_FINAL_DATA data->ai_data.guild_final_data
#define GUILD_WAIT_DATA data->ai_data.guild_wait_data
#define GUILD_LAND_DATA data->ai_data.guild_land_data

enum WANYAOGU_STATE
{
	WANYAOGU_STATE_INIT,  //还没开始
	WANYAOGU_STATE_WAIT_START,  //开始10秒倒计时
	WANYAOGU_STATE_START,    //关卡开始
	WANYAOGU_STATE_BBQ,      //关卡结束，挂机得经验
	WANYAOGU_STATE_FINISH,   //所有关卡结束
};

enum PVP_RAID_STATE
{
	PVP_RAID_STATE_INIT,  //还没开始
	PVP_RAID_STATE_WAIT_START,  //开始10秒倒计时
	PVP_RAID_STATE_START,    //关卡开始
	PVP_RAID_STATE_FINISH,   //所有关卡结束
};
enum GUILD_RUQIN_ACTIVE
{
	GUILD_RUQIN_ACTIVE_INIT,  //还没开始
	GUILD_RUQIN_ACTIVE_START,	//活动开始
	GUILD_RUQIN_ACTIVE_FINISH,  //活动成功
	GUILD_RUQIN_ACTIVE_FAILD,  //活动失败
	GUILD_RUQIN_ACTIVE_BBQ,   //活动结束，挂机得经验
};

struct assist_data
{
	uint64_t player_id;   //造成伤害的玩家ID
	uint32_t damage_time;  //造成伤害的时间
};

struct pvp_player_praise_record
{
	bool praise[MAX_TEAM_MEM * 2];  //有没有点赞
};

#define MAX_RAID_REGION 20
struct raid_script_data
{
		//脚本配置部分
	std::vector<struct RaidScriptTable *> *script_config;
	uint16_t cur_index;
	uint32_t cur_finished_num[MAX_SCRIPT_COND_NUM];
	uint32_t collect_callback_event;   //采集回调的操作 1: 打断雷鸣鼓
	uint32_t dead_monster_id;  //死亡的怪物在判断怪物血量的时候也要算进去
	struct RaidScriptTable *region_config[MAX_RAID_REGION];
	uint8_t cur_region_config;
};

union guild_land_ai_data
{
	struct bonfire_ai_data
	{
		uint32_t begin_time;
		uint32_t end_time;
		uint32_t collect_refresh_time;
		monster_struct *bonfire;
	} bonfire_data;
};

union raid_ai_data
{
	struct 
	{
//		int star;
		WANYAOGU_STATE wanyaogu_state;
		uint64_t timer1;   //计时器1
		uint64_t timer2;   //计时器2		
		struct DungeonTable* m_config;		//当前关卡的配置
//		struct ControlTable *m_control_config;		//当前关卡的配置
		uint32_t wanyaogu_raid_id[MAX_WANYAOGU_RAID_NUM];  //随机的关卡ID
		uint32_t raid_pass[MAX_WANYAOGU_RAID_NUM];  //是否通关
		uint32_t wanyaoka_id[MAX_WANYAOKA_EACH_TIME];  //获得的万妖卡
		uint32_t wanyaoka_cond_param[MAX_WANYAOKA_COND_PARAM];  //当前关卡的条件参数完成情况
		struct raid_script_data script_data;
	} wanyaogu_data;

	struct
	{
		PVP_RAID_STATE pvp_raid_state;		
		uint32_t refresh_monster_time;
		uint32_t red_buff_relive_time;
		uint32_t blue_buff_relive_time;
		uint8_t kill_record[MAX_TEAM_MEM * 2];  //击杀记录
		uint8_t dead_record[MAX_TEAM_MEM * 2];  //死亡次数记录
		uint8_t assist_record[MAX_TEAM_MEM * 2];   //助攻次数记录
		struct assist_data assist_data[MAX_TEAM_MEM * 2][MAX_TEAM_MEM]; //伤害记录，用来计算助攻
		struct pvp_player_praise_record praise_index[MAX_TEAM_MEM * 2];  //对应的玩家下标
		bool pvp_raid_ready[MAX_TEAM_MEM * 2];  //是否已经进入游戏了
		uint32_t average_lv;   //平均等级，用来做怪物等级
	} pvp_data;

	struct
	{
		PVP_RAID_STATE pvp_raid_state;
	} doufachang_data;

	struct
	{
		uint8_t kill_record[MAX_TEAM_MEM * 2];  //击杀记录		
		struct assist_data assist_data[MAX_TEAM_MEM * 2][MAX_TEAM_MEM]; //伤害记录，用来计算助攻
		uint32_t guild_id[2]; //对应的工会ID
	} guild_data;

	struct
	{
		bool air_wall_close[4];  //4个空气墙是否关闭
		uint8_t kill_record[MAX_TEAM_MEM * 4];  //击杀玩家记录
		uint8_t monster_record[MAX_TEAM_MEM * 4];  //击杀小怪记录
		uint32_t boss_record[MAX_TEAM_MEM * 4];  //BOSS伤害纪录
		uint32_t guild_id[4]; //对应的工会ID
		uint32_t boss_maxhp; //boss的最大血量
		uint64_t boss_killer; //击杀boss的玩家
	} guild_final_data;
	
	struct
	{
		struct raid_script_data script_data;		
//		std::vector<struct RaidScriptTable *> *script_config;
//		uint16_t cur_index;
//		uint32_t cur_finished_num[MAX_SCRIPT_COND_NUM];
	} script_data;

	struct
	{
//		uint16_t cur_player_num;  //当前人数
		int m_line;   //第几条线	
		uint32_t cur;
		uint64_t truck; //矿车
		uint64_t camp; //攻守表ID
		uint64_t lv; //阵营表ID
		uint64_t time_rest;
		double speed; 
		uint64_t time_speed;
		int progress;
		uint64_t follow[MAX_ZHENYING_FOLLOW];
	} zhenying_data;
	struct
	{
		uint32_t step;   //
		uint32_t room;   //
	} battle_data;
	struct
	{
		uint32_t state;   //
		bool refresh;
		uint64_t end;   //
		uint64_t mon_num;
	} tower_data;

	struct
	{
		uint32_t guild_id; //对应的工会ID
	} guild_wait_data;

	struct
	{
		uint32_t guild_id; //对应的帮会ID
		struct raid_script_data script_data;		
		uint32_t activity_id; //当前正在进行的活动
		uint32_t activity_state; //当前正在进行的活动状态
		guild_land_ai_data activity_data;
	} guild_land_data;

	struct
	{
		bool note_boss;   //
		uint32_t target;
	}guoyu_data;
	struct
	{
		struct raid_script_data script_data;		
		uint32_t diaoxiang_id;   //雕像怪物id
		uint32_t diaoxiang_colour; //雕像颜色
		uint32_t gui_wang_id; //鬼王怪物id
		bool     raid_finished;  //鬼王死亡
		uint32_t po_buff_time; //破除猫鬼王buff时长
		uint64_t buff_time;  //猫鬼王buff时间
		uint32_t creat_time; //鬼王被攻击多少秒后召唤小怪
		uint64_t creat_monster_time; //鬼王召唤小怪时间点
		bool first_creat; //鬼王是否是第一次召唤小怪
		uint32_t zhaohuan_num; //鬼王召唤小怪次数
	}maogui_data;
};

//帮会入侵玩家数据
struct guild_ruqin_player_data
{
	uint64_t player_id;   //玩家id
	uint32_t damage_to_monster; //玩家对怪物造成的伤害
	uint32_t exp; //玩家获得的经验
};

//帮会入侵活动数据
struct guild_ruqin_data{
	bool guild_ruqin; //帮会入侵活动是否已经开启，在开启状态不再重复开启的操作
	uint64_t end_time; //本轮结束时间戳
	uint32_t zhengying; //帮会阵营
	uint32_t level; //活动刷怪等级
	uint32_t monster_boshu; //刷出第几波怪物
	uint32_t all_boshu;   //总共要刷出的波数
	uint32_t monster_id; //火堆怪物id
	uint64_t space_time; //上次获得经验时间
	uint32_t pos_x;     //位置
	uint32_t pos_z;     //位置
	uint32_t juli;     //范围
	uint64_t huodui_time; //火堆持续的时间点
	uint32_t exp;     //一次加多少经验
	GUILD_RUQIN_ACTIVE status; //帮会入侵活动状态
	std::map<uint64_t, guild_ruqin_player_data> player_data;
};

struct raid_data
{
	uint64_t uuid;
	uint32_t ID;
	uint64_t start_time;  //副本开始时间
	uint64_t delete_time;  //副本没人了，等待销毁的时间
	RAID_STATE_DEFINE state;
	uint32_t dead_count;   //死亡次数		

	struct raid_player_info player_info[MAX_TEAM_MEM];
	struct raid_player_info player_info2[MAX_TEAM_MEM];
	struct raid_player_info player_info3[MAX_TEAM_MEM];
	struct raid_player_info player_info4[MAX_TEAM_MEM];		

	uint64_t team_id;
	uint64_t team2_id;
	uint64_t team3_id;
	uint64_t team4_id;	
	bool delete_team1;   //副本结束后是否解散队伍1
	bool delete_team2;	 //副本结束后是否解散队伍2
	bool delete_team3;	 //副本结束后是否解散队伍2
	bool delete_team4;	 //副本结束后是否解散队伍2

	uint8_t star_bits;   //当前副本星级 按比特位计算
	uint8_t star_param[3];   //计算副本星级需要的参数
	
	uint32_t pass_index;  //通关类型下标
	uint32_t pass_value;  //通关参数对应完成的数值
	int ai_type;
	uint64_t raid_ai_event; //有些副本ai事件需要客户端执行完毕后通知后台继续副本ai，次参数用来记录客户端发回的副本事件id
	union raid_ai_data ai_data;
//	uint32_t monster_level; //刷怪等级
};

class raid_struct;

typedef void(*raid_ai_tick)(raid_struct *);
typedef void(*raid_ai_init)(raid_struct *, player_struct *);
typedef void(*raid_ai_finished)(raid_struct *);
typedef void(*raid_ai_failed)(raid_struct *);
typedef void(*raid_ai_player_enter)(raid_struct *, player_struct *);
typedef void(*raid_ai_player_leave)(raid_struct *, player_struct *);
typedef void(*raid_ai_player_dead)(raid_struct *, player_struct *, unit_struct *);
typedef void(*raid_ai_player_relive)(raid_struct *, player_struct *, uint32_t);
typedef void(*raid_ai_monster_dead)(raid_struct *, monster_struct *, unit_struct *);
typedef void(*raid_ai_collect)(raid_struct *, player_struct *, Collect *);
typedef void(*raid_ai_attack)(raid_struct *, player_struct *, unit_struct *, int);
typedef void(*raid_ai_player_region_changed)(raid_struct *, player_struct *, uint32_t, uint32_t);
typedef void(*raid_ai_monster_region_changed)(raid_struct *, monster_struct *, uint32_t, uint32_t);
typedef void(*raid_ai_escort_stop)(raid_struct *, player_struct *, uint32_t, bool);
typedef void(*raid_ai_npc_talk)(raid_struct *, player_struct *, uint32_t);
typedef void(*raid_ai_escort_end_piont)(raid_struct *, monster_struct *);
typedef struct DungeonTable* (*raid_ai_get_config)(raid_struct *);
typedef void(*raid_ai_monster_attack)(raid_struct *, monster_struct *, unit_struct *, int32_t, int32_t);
typedef void(*raid_ai_monster_relive)(raid_struct *, monster_struct *);
typedef void(*raid_ai_play_drama_end)(raid_struct *);

struct raid_ai_interface
{
	raid_ai_init raid_on_init; //初始化
	raid_ai_tick raid_on_tick; //定时驱动
	raid_ai_player_enter raid_on_player_enter;  //玩家进入
	raid_ai_player_leave raid_on_player_leave;  //玩家离开
	raid_ai_player_dead raid_on_player_dead;  //玩家死亡
	raid_ai_player_relive raid_on_player_relive;  //玩家复活
	raid_ai_monster_dead raid_on_monster_dead; //怪物死亡
	raid_ai_collect raid_on_raid_collect; //采集
	raid_ai_player_enter raid_on_player_ready;  //玩家进入场景(客户端loading完)
	raid_ai_finished raid_on_finished; //完成
	raid_ai_attack raid_on_player_attack; //玩家攻击
	raid_ai_player_region_changed raid_on_player_region_changed; //区域变化
	raid_ai_escort_stop raid_on_escort_stop; //护送结果
	raid_ai_npc_talk raid_on_npc_talk; //和npc对话
	raid_ai_get_config raid_get_config; //获取配置，主要是万妖谷的配置
	raid_ai_failed raid_on_failed; //失败
	raid_ai_monster_region_changed raid_on_monster_region_changed; //区域变化	
	raid_ai_escort_end_piont raid_on_escort_end_piont; //矿车到达终点
	raid_ai_monster_attack raid_on_monster_attacked; //怪物被击
	raid_ai_monster_relive raid_on_monster_relive;  //怪物复活或者创建
	raid_ai_play_drama_end raid_on_play_drama_end;  //剧情播放结束
};

class raid_struct : public scene_struct
{
public:
	virtual void clear();
	virtual int init_special_raid_data(player_struct *player);
	virtual bool check_raid_need_delete();
	virtual	int player_offline(player_struct *player);

		//进入另外一个野外场景，为空表示下线	
//	virtual int enter_other_scene(player_struct *player, scene_struct *new_scene, double pos_x, double pos_y, double pos_z, double direct);
//	virtual int enter_other_raid(player_struct *player, raid_struct *new_raid);	//进入另外一个副本，为空表示下线
	virtual int player_leave_scene(player_struct *player);
	virtual int player_enter_raid(player_struct *player, double pos_x, double pos_z, double direct);
	virtual int team_enter_raid(Team *team);
	virtual bool use_m_player();

	virtual int init_raid(player_struct *player);
	void raid_set_ai_interface(int ai_type);
	static void raid_add_ai_interface(int ai_type, struct raid_ai_interface *ai);
	void team_destoryed(Team *team);
	/* int team2_enter_raid(Team *team); */
	/* int team3_enter_raid(Team *team); */
	/* int team4_enter_raid(Team *team);		 */
	int player_return_raid(player_struct *player);
//	int player_enter_raid(player_struct *player, double pos_x, double pos_z, double direct = 0);
	int player_enter_raid_impl(player_struct *player, int index, double pos_x, double pos_z, double direct = 0);
	int player_leave_raid(player_struct *player);
	bool is_monster_alive(uint32_t id);	
	int get_id_monster_num(uint32_t id);	
	int get_id_collect_num(uint32_t id);
	int check_all_monster_region_buff(struct RaidScriptTable *config);
	int add_monster_to_scene(monster_struct *monster, uint32_t effectid);
//	int add_player_to_scene(player_struct *player);
	int add_collect_to_scene(Collect *pCollect);
	int delete_monster_from_scene(monster_struct *monster, bool send_msg);	
//	int delete_player_from_scene(player_struct *player);
//	int delete_collect_to_scene(Collect *pCollect);

	void on_player_enter_raid(player_struct *player);
	void on_player_leave_raid(player_struct *player);	
	int on_raid_finished();
	int on_raid_failed(uint32_t score_param);
	bool check_can_add_team_mem(player_struct *player);  //队伍能否在该副本中途加入队员
	SCENE_TYPE_DEFINE get_scene_type();
	void on_tick();
	void on_player_attack(player_struct *player, unit_struct *target, int damage);
	void on_monster_attack(monster_struct *monster, player_struct *player, int damage);
	void on_monster_dead(monster_struct *monster, unit_struct *killer);
	void on_player_dead(player_struct *player, unit_struct *killer);
	void on_collect(player_struct *player, Collect *collect);
	void send_raid_pass_param(player_struct *player);
	bool need_show_star();
	void send_star_changed_notify(uint32_t star_param[3], uint32_t score_param[3]);	
	bool add_raid_pass_value(uint32_t pass_type, struct DungeonTable* config);	  //true表示副本结束
	bool check_raid_failed();  //副本是否已经失败
	int check_cond_finished(int index, uint64_t cond_type, uint64_t cond_value, uint64_t cond_value1, uint32_t *ret_param);  //判断指定的完成条件是否达成
	int calc_raid_star(uint32_t star_param[3], uint32_t score_param[3]);
	virtual int broadcast_to_raid(uint32_t msg_id, void *msg_data, pack_func func, bool include_not_ready);
	void broadcast_msg_add_players(player_struct *player, uint64_t *ppp, bool include_not_ready);
	
	int init_common_script_data(const char *script_name, struct raid_script_data *script_data);	
	struct DungeonTable *get_raid_config();
	player_struct *get_team_leader_player();
	void stop_monster_ai();
	void start_monster_ai();
	void stop_player_ai();
	void start_player_ai();		
	virtual void clear_monster();
	int set_player_info(player_struct *player, struct raid_player_info *info);	
	int get_monster_num() { return m_monster.size(); }
	uint16_t get_cur_player_num() {return player_num;};
//	int get_raid_player_pos(uint64_t player_id);
	struct raid_player_info *get_raid_player_info(uint64_t player_id, int *pos);
	bool is_guild_battle_raid(); //是否帮战的副本
	bool is_guild_battle_fight_raid(); //是否帮战的战斗副本
	monster_struct *get_first_boss();
	
	struct raid_ai_interface *ai;
	struct raid_data *data;
	player_struct *m_player[MAX_TEAM_MEM];   //在线玩家，但是有可能还在loading
	player_struct *m_player2[MAX_TEAM_MEM];
	player_struct *m_player3[MAX_TEAM_MEM];
	player_struct *m_player4[MAX_TEAM_MEM];		
	Team *m_raid_team;
	Team *m_raid_team2;
	Team *m_raid_team3;
	Team *m_raid_team4;		
	struct DungeonTable* m_config;
	struct ControlTable *m_control_config;
	std::set<monster_struct *> m_monster;
	int mark_finished;   //副本是否结束了, 0表示没结束，1表示失败了，其他表示通过结束了
	guild_ruqin_data ruqin_data;	//帮会入侵活动数据
	uint32_t lv;  //副本等级
	
protected:
	uint16_t player_num;  //记录玩家数目，没有玩家了才可以删除
	uint64_t next_notify_monster_pos_time;
	void delete_raid_collect_safe(uint32_t uuid);
	void try_notify_monster_pos();
	void set_raid_lv(player_struct *player);	
	int init_script_data();
	int	init_wanyaogu_data();
	int	init_pvp_raid_data_3();
	int	init_pvp_raid_data_5();
	int init_guild_raid_data();
	int init_guild_final_raid_data();
	int	init_guoyu_raid_data(player_struct *player);
	virtual int set_m_player_and_player_info(player_struct *player, int index);
	virtual int clear_m_player_and_player_info(player_struct *player, bool clear_player_info);	
	int get_free_player_pos();
	void broadcast_player_hit_statis_changed(struct raid_player_info *info, player_struct *player);
};

#endif /* RAID_H */
