#ifndef __COMM_DEFINE_H__
#define __COMM_DEFINE_H__

#define MAX_PLAYER_NAME_LEN     32 //人物名字最大长度
#define MAX_BAG_GRID_NUM        100 //背包格子数
#define MAX_MONEY_VALUE         999999999 //货币储存最大值
#define RAND_RATE_BASE          10000 //概率基数
#define MAX_HEAD_ICON_NUM       30 //最大头像数
#define MAX_TASK_TARGET_NUM     6 //最大任务目标数
#define MAX_TASK_NUM            300 //最大任务数
#define MAX_TASK_ACCEPTED_NUM   13 //最大可接任务数
#define MAX_EQUIP_ENCHANT_RAND_NUM   3 //最大装备附魔随机属性数
#define MAX_EQUIP_ENCHANT_NUM   3 //最大装备附魔属性条数
#define MAX_EQUIP_INLAY_NUM     6 //最大装备镶嵌宝石数
#define MAX_EQUIP_NUM           10 //最大装备数
#define MAX_COLOR_NUM           30 //最大颜色数
#define MAX_WEAPON_COLOR_NUM           100 //最大颜色数
#define MAX_FASHION_NUM           100 //最大时装数
#define MAX_HORSE_NUM           100 //最大坐骑数
//#define DEFAULT_HORSE           180000001 //
#define MAX_BATTLE_LINE_NUM     (8) //阵营战分线
//#define MAX_TOWER_LEVEL  (20) 
//最大副本数目
#define MAX_RAID_NUM            1000 
#define SCENCE_DEPART           (20000)

//邮件
#define MAX_MAIL_NUM            100 //最大邮件数
#define MAX_MAIL_ATTACH_NUM     8 //单封邮件最大附件数

//商城
#define MAX_SHOP_LIST_NUM       20 //商城页签最大数
#define MAX_SHOP_GOODS_NUM      500 //商城商品最大数

//御气道
#define MAX_YUQIDAO_MAI_NUM     8 //御气道经脉最大数
#define MAX_YUQIDAO_BREAK_NUM   4 //御气道冲穴最大数
#define MAX_YUQIDAO_BREAK_ATTR_NUM 4 //御气道冲穴属性最大数

//八卦牌
#define MAX_BAGUAPAI_STYLE_NUM  2 //八卦牌风格最大数
#define MAX_BAGUAPAI_DRESS_NUM  8 //八卦牌穿戴件数
#define MAX_BAGUAPAI_MINOR_ATTR_NUM  3 //八卦牌副属性最大数
#define MAX_BAGUAPAI_ADDITIONAL_ATTR_NUM  3 //八卦牌追加属性最大数

//活动
#define MAX_ACTIVE_REWARD_NUM   15 //活跃度奖励最大数
#define MAX_DAILY_ACTIVITY_NUM   100 //日常活动最大数
#define MAX_CHIVALRY_ACTIVITY_NUM   10 //侠义活动最大数

#define MAX_PERSONALITY_LOCATION_LEN 200
#define MAX_PERSONALITY_TAG_NUM      4
#define MAX_PERSONALITY_TEXT_INTRO_LEN 200
#define MAX_PERSONALITY_VOICE_INTRO_LEN 200
#define MAX_FRIEND_RECOMMEND_PLAYER    1000
#define MAX_FRIEND_CONTACT_NUM      200 //我的好友最大数
#define MAX_FRIEND_ENEMY_NUM        200 //仇人最大数

#define MAX_PLAYER_BASE_ATTR_NUM 10
#define MAX_RAID_TEAM_NUM  4
#define MAX_GUILD_RAID_TEAM_NUM  2
#define MAX_GUILD_BATTLE_FINAL_GUILD_NUM  4 //帮战决赛只有四个帮会参加
#define MAX_GUILD_NAME_LEN     32 //帮会名字最大长度
#define MAX_GUILD_SKILL_NUM     20 //帮会技能最大数
#define MAX_GUILD_MEMBER_NUM 500 //每个帮会最大成员数

#define MAX_PARTNER_TYPE   100
#define MAX_PARTNER_NUM   150
#define MAX_PARTNER_FORMATION_NUM   4
#define MAX_PARTNER_SKILL_NUM   6
#define MAX_PARTNER_BATTLE_NUM  4  //出战伙伴
#define MAX_PARTNER_BOND_NUM  100 //伙伴羁绊数

#define MAX_ESCORT_NUM   2
#define MAX_ESCORT_MONSTER_NUM   (40 + 1)
#define MAX_ESCORT_MONSTER_WAVE   (4)

#define MAX_ACHIEVEMENT_NUM   400 //成就数
#define MAX_TITLE_NUM   200 //称号数

//伙伴法宝
#define MAX_HUOBAN_FABAO_MINOR_ATTR_NUM 4 //伙伴法宝最大副属性

#define MAX_RANK_TYPE  100 //排行榜类型数

//最大英雄挑战怪物个数
#define MAX_HERO_CHALLENGE_MONSTER_NUM 100
//扫荡奖励物品最大数量
#define MAX_HERO_CHALLENGE_SAOTANGREWARD_NUM 20

#define MAX_STRONG_GOAL_NUM 200
#define MAX_STRONG_CHAPTER_NUM 10

//交易行
#define MAX_TRADE_SHELF_NUM  20 //单个玩家货架格子数
#define MAX_TRADE_SOLD_NUM  200 //单个玩家已出售记录数
#define MAX_AUCTION_LOT_NUM  200 //拍卖品数
#define MAX_AUCTION_MASTER_NUM  (MAX_GUILD_MEMBER_NUM) //拍卖品数
#define MAX_GUILD_LAND_ACTIVE_NUM            1000 

//最大等级奖励档次
#define MAX_PLAYER_LEVEL_REWARD_NUM 50
//最大在线奖励物品
#define MAX_PLAYER_ONLINE_REWARD_NUM 20
//最大累计签到奖励次数
#define MAX_PLAYER_SINGN_EVERYDAY_REWARD_NUM 20
//最大月份
#define MAX_ONE_YEARS_THE_MONTH 12
//最大可找回奖励的活动
#define MAX_ACTIVE_CAN_ZHAOHUI_REWARD 100
//最大奖励找回物品数量
#define MAX_ACTIVE_CAN_ZHAOHUI_ITEM_NUM 50
//摇钱树倍率机制的初始倍率
#define YAO_QIAN_SHU_CHU_SHI_BEILV 3
//摇钱树玩家单次请求要摇的最大次数
#define YAO_QIAN_SHU_DAN_CI_MAX_NUM 20
//最大可领登录奖励次数
#define MAX_LOGIN_REWARD_RECEIVE_NUM 10

#define MAX_GAME_FUNCTION_NUM 100

//最大赐福奖励个数
#define MAX_CIFU_REWARD_NUM 5

//九宫八卦最大奖励数量
#define MAX_JIU_GONG_BA_GUA_REWARD_NUM 10

//发红包留言最大长度
#define MAX_RED_PACKET_LIU_YAN_LEN 500

//玩家状态
enum PlayerStatus
{
	ONLINE,               //正常在线玩家
	OFFLINE_SAVING,       //下线玩家,保存数据未返回
};

//道具类型
enum ItemType
{
	ITEM_TYPE_COIN = 1, //银票
	ITEM_TYPE_BIND_GOLD = 2, //绑定元宝
	ITEM_TYPE_GOLD = 3, //元宝
	ITEM_TYPE_EXP = 4, //经验
	ITEM_TYPE_ITEM = 5, //普通道具
	ITEM_TYPE_EQUIP = 6, //装备
	ITEM_TYPE_GUOYU_EXP = 7, //国御经验
	ITEM_TYPE_CHENGJIE_EXP = 8, //惩戒经验
	ITEM_TYPE_SHANGJIN_EXP = 9, //赏金经验
//	ITEM_TYPE_GUOYU_COIN = 10, //国御货币
//	ITEM_TYPE_CHENGJIE_COIN = 11, //惩戒货币
//	ITEM_TYPE_SHANGJIN_COIN = 12, //赏金货币
	ITEM_TYPE_GUILD_TREASURE = 13, //帮会资金
	ITEM_TYPE_GUILD_DONATION = 14, //帮贡
	ITEM_TYPE_PARTNER_EXP = 15, //伙伴经验
	ITEM_TYPE_SILVER = 16, //银币
	ITEM_TYPE_GONGXUN = 17, //功勋
	ITEM_TYPE_LINGSHI = 18, //灵石
	ITEM_TYPE_XUEJING = 19, //血晶
	ITEM_TYPE_SHENGWANG = 20, //声望
	ITEM_TYPE_GUILD_BUILD_BOARD = 21, //建设令
};

enum JobDefine
{
	JOB_DEFINE_MONSTER = 0, //怪物
	JOB_DEFINE_DAO = 1, //大刀男
	JOB_DEFINE_GONG = 2, //弓箭女
	JOB_DEFINE_BI = 3, //毛笔男
	JOB_DEFINE_QIANG = 4, //长枪妖师
	JOB_DEFINE_FAZHANG = 5, //提杖妖师
//	JOB_DEFINE_COLLECT = 6, //采集点
};

enum SexDefine
{
	SEX_DEFINE_MALE = 1, //男性
	SEX_DEFINE_FEMALE = 2, //女性
};

enum MonsterTypeDefine
{
	MONSTER_TYPE_DEFINE_NORMAL = 1, //普通怪物
	MONSTER_TYPE_DEFINE_ELITE = 2, //精英怪物
	MONSTER_TYPE_DEFINE_BOSS = 3, //BOSS怪物
	MONSTER_TYPE_DEFINE_JIANTA = 4, //箭塔
	MONSTER_TYPE_DEFINE_TRAP = 5, //陷阱
};

enum MonsterHateTypeDefine
{
	MONSTER_HATETYPE_DEFINE_NORMAL = 1, //普通怪物
	MONSTER_HATETYPE_DEFINE_ELITE = 2, //精英怪物
	MONSTER_HATETYPE_DEFINE_BOSS = 3, //BOSS怪物
	MONSTER_HATETYPE_DEFINE_AIBOSS = 4, //BOSS怪物
};

enum AddItemDealWay
{
	ADD_ITEM_WILL_FAIL = 1, //添加道具会失败，要么全添加，要么全失败
	ADD_ITEM_AS_MUCH_AS_POSSIBLE = 2, //尽可能多的添加，背包不足时丢弃
	ADD_ITEM_SEND_MAIL_WHEN_BAG_FULL = 3, //背包空间不足时，发邮件
};

enum HeadIconStatus
{
	HIS_NEW_UNLOCK = 1,
};

enum HeadUnlockConditionType
{
	HUCT_LEVEL_UP = 1, //角色等级
	HUCT_ACTIVITY = 2, //参加活动获得
	HUCT_ITEM_USE = 3, //道具使用
	HUCT_RECHARGE = 4, //充值获得
	HUCT_VIP_LEVEL = 5, //VIP等级获得
};

enum ItemUseEffect
{
	IUE_UNLOCK_HEAD = 1, //解锁头像功能
	IUE_TRANSFER_SCENE = 2, //场景传送
	IUE_FIND_TARGET = 7, //追踪
	IUE_SUB_MURDER = 8, //降低杀戮值
	IUE_EXTEND_FRIEND_NUM = 10, //提升好友上限
	IUE_ADD_HP = 11, //加血药剂
	IUE_ADD_HP_POOL = 12, //血池
	IUE_ADD_ENERGY = 13, //精力值
	IUE_XUNBAO = 14, //寻宝
	IUE_ADD_HORSE = 18, //获得坐骑
	IUE_ADD_TITLE = 22, //获得称号
	IUE_OPEN_FUNCTION = 23, //开启功能
	IUE_RANDOM_BOX = 24, //随机宝箱
	IUE_DINNER = 26, //队伍共享的采集点
	IUE_PROPOSE_POST = 27, //传送到情人岛
};

enum TaskConditionType
{
	TCT_BASIC = 1, //基础条件
	TCT_ACCEPT = 2, //接受任务---接取条件
	TCT_SUBMIT = 3, //提交任务---没用到
	TCT_FINISH = 4, //完成任务---完成条件
	TCT_JOIN_CAMP = 5, //加入阵营---完成条件
	TCT_TEAM = 6, //组队
	TCT_GUILD_JOIN = 7, //加入帮派
	TCT_GUILD_DONATION = 8, //拥有帮贡
	TCT_KILL_MONSTER = 9, //击杀怪物
	TCT_SKILL_LEVEL_UP = 10, //技能升级
	TCT_CARRY_ITEM = 12, //携带物品
	TCT_USE_PROP = 13, //使用物品
	TCT_BAGUA_WEAR = 14, //佩戴八卦
	TCT_ACTIVITY = 17, //完成活动
	TCT_PARTNER_OUT_FIGHT = 18, //伙伴参战
	TCT_FINISH_RAID = 20, //通关副本
	TCT_KILL_PLAYER = 21, //杀人
	TCT_EXPLORE = 22, //探索
	TCT_ESCORT = 24, //护送
	TCT_TRACK = 25, //跟踪
	TCT_TALK = 28, //对话
	TCT_SNEAK = 29, //潜入---弃用
	TCT_QUESTION = 30, //答题
	TCT_ZHENYING_SCORE = 31, //阵营攻防活动积分
	TCT_TRUE = 32, //限制客户端自动接任务用
	TCT_TRUCK = 33, //押镖
	TCT_EQUIP_STAR = 34, //装备星级
	TCT_EQUIP_STAR_UP = 35, //装备升星
	TCT_EQUIP_ENCHANT = 36, //装备附魔
	TCT_EQUIP_INLAY = 37, //装备镶嵌
	TCT_GEM_COMPOSE = 38, //宝石合成
	TCT_FASHION_WEAR = 39, //佩戴时装
	TCT_FRIEND_NUM = 40, //好友数量
	TCT_JOIN_TEAM = 41, //加入队伍
	TCT_SHOP_BUY = 42, //商城购买
	TCT_XUNBAO = 43, //寻宝
	TCT_PARTNER_RECRUIT = 44, //伙伴招募
	TCT_PARTNER_ADD_ATTR = 45, //伙伴资质强化
	TCT_PARTNER_RESET_ATTR = 46, //伙伴洗髓
	TCT_PARTNER_ADD_GOD = 47, //伙伴神曜
	TCT_BAGUA_REFINE_STAR = 48, //八卦炼星
	TCT_BAGUA_REFINE_MAIN_ATTR = 49, //八卦重铸
	TCT_BAGUA_REFINE_MINOR_ATTR = 50, //八卦洗炼
	TCT_YAOSHI_GUOYU = 51, //妖师国御
	TCT_YAOSHI_SHANGJIN = 52, //妖师赏金
	TCT_WANYAOGU = 53, //万妖谷
	TCT_HORSE_ADD_EXP = 54, //坐骑修灵
	TCT_QUESTION_JOIN = 55, //参与答题
	TCT_TRUCK_NUM = 56, //押镖次数
	TCT_BOAT = 57, //坐船
	TCT_FISHING = 58, //钓鱼
	TCT_PUZZLE = 59, //拼图
	TCT_MIJING_XIULIANG = 60, //秘境修炼
	TCT_CHAT = 62, //聊天发言
	TCT_HORSE_ADD_SOUL = 63, //坐骑铸灵
	TCT_PARTNER_USE_EXP_ITEM = 64, //伙伴吃经验丹
	TCT_LIVE_SKILL_PRODUCE = 65, //生活技能生产
	TCT_CLIENT_QUESTION = 66, //任务答题
	TCT_GUILD_CHUAN_GONG = 67, //门宗传功
	TCT_GUILD_DONATION1 = 68, //门宗募捐
	TCT_PLAY_TOWER = 69, //幻宝地牢
};

enum TaskBasicCondition
{
	TBC_JOB = 1, //职业
	TBC_LEVEL = 2, //等级
	TBC_SEX = 3, //性别
	TBC_EXP = 4, //经验
	TBC_COIN = 5, //银两
	TBC_GOLD = 6, //元宝
	TBC_MAX_LEVEL = 7, //等级以下
	TBC_ZHENYING = 8, //阵营
};

enum TaskType
{
	TT_TRUNK = 1, //主线
	TT_BRANCH = 2, //支线
	TT_ACTIVITY = 3, //活动
	TT_SHANGJIN = 4, //赏金
	TT_QUESTION = 6, //答题
	TT_RAID = 9, //副本
	TT_CASH_TRUCK = 10, //押镖
	TT_DIGONG = 11, //地宫修炼
	TT_GUILD_BUILD = 12, //帮会建设
	TT_TRAVEL = 13, //游历
	TT_GUIDE = 14, //引导
};

enum TaskEventClass
{
	TEC_ACCEPTABLE = 1, //可接事件
	TEC_ACCEPT = 2, //接受事件
	TEC_ACHIEVE = 3, //完成事件
	TEC_SUBMIT = 4, //提交事件
	TEC_TALK = 5, //对话事件
};

enum TaskEventType
{
	TET_ADD_ITEM = 1, //添加物品
	TET_DEL_ITEM = 2, //删除物品
	TET_ADD_MONSTER = 3, //刷怪
	TET_DEL_MONSTER = 4, //删除怪物
	TET_ADD_BUFF = 5, //添加状态
	TET_DEL_BUFF = 6, //删除状态
	TET_ADD_NPC = 7, //添加NPC
	TET_DEL_NPC = 16, //删除NPC
	TET_NPC_FOLLOW = 19, //NPC跟随
	TET_ESCORT = 20, //护送
	TET_PLANES_ADD_COLLECT = 33, //刷新位面采集物
	TET_PLANES_DEL_COLLECT = 34, //删除位面采集物
	TET_PLANES_ADD_NPC = 35, //刷新位面NPC
	TET_PLANES_DEL_NPC = 36, //删除位面NPC
	TET_PLANES_EXIT = 37, //退出位面
	TET_SUB_CURRENCY = 38, //扣除货币
	TET_ADD_TRUCK = 43, //创建镖车
	TET_ADD_PARTNER = 46, //增加宠物
};

enum TaskRewardType
{
	TRT_ITEM = 1, //物品
	TRT_BUFF = 2, //BUFF
	TRT_GOLD = 4, //元宝
	TRT_BIND_GOLD = 5, //绑定元宝
};

enum EquipmentType
{
	ET_WEAPON = 1, //武器
	ET_HELM = 2, //头盔
	ET_COAT = 3, //衣服
	ET_BELT = 4, //腰带
	ET_BRACER = 5, //护腕
	ET_GLOVE = 6, //手套
	ET_PANT = 7, //裤子
	ET_SHOE = 8, //鞋子
	ET_RING = 9, //戒指
	ET_NECKLACE = 10, //项链
};

enum FashionType
{
	FASHION_TYPE_WEAPON = 1,
	FASHION_TYPE_HAT = 2,
	FASHION_TYPE_CLOTHES = 3
};

enum PlayerOfflineCacheType
{
	CACHE_SUB_EXP = 1,
	CACHE_PVP_RAID_LOSE,
	CACHE_PVP_RAID_WIN,
};

//答题类型
enum 
{
	QUESTION_DAILY = 1, //每日答题
	QUESTION_AWARD = 2, //任务答题
	QUESTION_GUILD = 3, //帮会答题
};

//妖师类型
enum
{
	YAOSHI_SHANGJIN = 1, //赏金妖师
	YAOSHI_CHENGJIE = 2, //惩戒妖师
	YAOSHI_GUOYU    = 3, //国御妖师
};

//切磋结果
enum
{
	QIECUO_VICTORY = 1, //胜利
	QIECUO_DEFEAT  = 2, //失败
	QIECUO_EVEN    = 3, //平手
};

//PVP类型
enum
{
	PVP_3_RAID = 1,
	PVP_5_RAID = 2,
};

enum ActivityMatter
{
	AM_RAID = 1, //通关副本
	AM_XUNBAO = 3, //寻宝
	AM_ANSWER = 4, //答题
	AM_TRUCK = 5, //押镖
	AM_YAOSHI = 6, //妖师客栈
	AM_MIJINGXIULIAN = 7, //秘境修炼
	AM_GUILD_INTRUSION = 8, //帮会入侵
	AM_HERO_CHLLENGE = 9, //英雄挑战
	AM_TRAVEL = 10, //游历
	AM_GUILD_DONATE = 11, //门宗募捐
	AM_FISHING = 12, //钓鱼
	AM_GUILD_BUILD = 13, //门宗建设
	AM_GUILD_BBQ = 14, //门宗篝火
	AM_GUILD_CHUANGONG = 15, //传功
	AM_WANYAOGU = 16, //万妖谷
};

enum AchievementConditionType
{
	ACType_PLAYER_LEVEL = 1, //人物等级达到N级
	ACType_PLAYER_FC = 2, //人物战力达到N点
	ACType_SKILL_LEVEL_UP = 3, //升级技能N次
	ACType_SKILL_ALL_LEVEL = 4, //人物4个技能等级都达到N级
	ACType_SKILL_FUWEN_UNLOCK = 5, //解锁N个技能符文
	ACType_SKILL_FUWEN_WEAR = 6, //装备N个技能符文
	ACType_SKILL_FUWEN_LEVEL_UP = 7, //升级技能符文N次
	ACType_SKILL_FUWEN_LEVEL_NUM = 8, //任意3个技能符文等级达到5级
	ACType_LIVE_SKILL_LEVEL = 9, //炼药等级达到5级
	ACType_LIVE_SKILL_ENERGY = 10, //炼药累计消耗精力200点
	ACType_FABAO_COMPOSE = 11, //打造法宝（法宝颜色）
	ACType_EQUIP_NUM = 12, //人物获得1件装备
	ACType_EQUIP_STAR_UP = 13, //装备升星5次
	ACType_EQUIP_STAIR = 14, //人物装备星级都达到2阶
	ACType_EQUIP_ENCHANT = 15, //累计附魔5次
	ACType_EQUIP_STRIP = 16, //剥离宝石5次
	ACType_EQUIP_INLAY_QULITY_NUM = 17, //人物10件装备镶嵌1级宝石
	ACType_ITEM_CONSUME = 18, //累计消耗八卦碎片50个
	ACType_BAGUA_DECOMPOSE = 19, //八卦分解
	ACType_BAGUA_REFINE_MAIN_ATTR = 20, //八卦重铸
	ACType_BAGUA_REFINE_STAR = 21, //八卦炼星
	ACType_BAGUA_REFINE_MINOR_ATTR = 22, //八卦洗炼
	ACType_BAGUA_SUIT = 23, //八卦套装
	ACType_BAGUA_MIN_STAR = 24, //八卦最小炼星
	ACType_YUQIDAO_FILL = 25, //御气道灌真气
	ACType_YUQIDAO_BREAK_OPEN = 26, //御气道开启冲穴
	ACType_HORSE_NUM = 27, //拥有坐骑
	ACType_HORSE_ADD_EXP = 28, //坐骑修灵
	ACType_HORSE_SET_CUR = 29, //坐骑幻化
	ACType_HORSE_ADD_SOUL = 30, //坐骑铸灵
	ACType_PARTNER_RECRUIT = 31, //伙伴招募
	ACType_PARTNER_NUM = 32, //伙伴拥有
	ACType_PARTNER_BOND_ACTIVE = 33, //伙伴羁绊激活
	ACType_PARTNER_ADD_ATTR = 34, //伙伴资质强化
	ACType_PARTNER_WEAR_FABAO = 35, //伙伴装备法宝
	ACType_PARTNER_RESET_ATTR = 36, //伙伴洗髓
	ACType_PARTNER_ADD_GOD = 37, //伙伴神曜
	ACType_PLAYER_RENAME = 38, //角色改名
	ACType_BAG_UNLOCK = 39, //扩充背包
	ACType_USE_PROP = 40, //使用道具
	ACType_CHAT_BOARDCAST = 41, //聊天广播
	ACType_FRIEND_NUM = 42, //拥有好友
	ACType_FRIEND_SEND_GIFT = 43, //好友送礼
	ACType_FRIEND_CLOSE_NUM = 44, //好友亲密度
	ACType_QIECUO = 45, //切磋
	ACType_XUANSHANG = 46, //悬赏
	ACType_GUILD_SKILL_LEVEL_UP = 47, //帮会技能升级
	ACType_GUILD_SKILL_LEVEL_NUM = 48, //帮会技能级数
	ACType_ANSWER = 49, //答题
	ACType_GUILD_BATTLE = 50, //帮会战
	ACType_GUILD_JOIN = 51, //加入帮会
	ACType_ZHENYING_CHOSE = 52, //加入阵营
	ACType_ZHENYING_CHANGE = 53, //转换阵营
	ACType_ZHENYING_BATTLE = 54, //阵营战
	ACType_ZHENYING_TASK_AWARD = 55, //阵营周目标奖励
	ACType_ZHENYING_GRADE = 56, //阵营军阶
	ACType_ZHENYING_KILL = 57, //击杀敌方阵营玩家
	ACType_MURDER_KILL = 58, //杀戮模式下击杀玩家
	ACType_MURDER_DEAD = 59, //杀戮模式下被击杀
	ACType_DEAD = 60, //死亡
	ACType_RELIVE = 61, //原地复活
	ACType_TASK_CHAPTER = 62, //完成任务章节
	ACType_TASK_NUM = 63, //完成任务数
	ACType_PVP_WIN = 64, //太极之巅胜利
	ACType_RAID_PASS_STAR = 65, //副本通关星数
	ACType_TRUCK = 66, //押镖
	ACType_FASHION_NUM = 67, //拥有时装
	ACType_FASHION_COLOR = 68, //时装染色
	ACType_FASHION_CHARM = 69, //时装魅力
	ACType_HEAD_NUM = 70, //拥有头像
	ACType_HEAD_REPLACE = 71, //更换头像
	ACType_ADD_CURRENCY = 72, //获得货币
	ACType_SHOP_BUY = 73, //商城购买
	ACType_RANKING_RANK = 74, //排行榜名次
	ACType_WORLD_BOSS = 75, //世界Boss
	ACType_YUQIDAO_MAI_FINISH = 76, //御气道穴脉修炼达到满级(穴脉)
	ACType_YUQIDAO_BREAK_COLOR_NUM = 77, //御气道冲脉
	ACType_BAG_GRID_NUM = 78, //背包格子数（格子数）
	ACType_EQUIP_ENCHANT_COLOR_NUM = 79, //装备附魔（附魔属性颜色）
	ACType_EQUIP_GEM_COMPOSE = 80, //合成宝石（宝石等级）
	ACType_HORSE_STEP = 81, //坐骑升阶（坐骑阶数）
	ACType_KILL_ENEMY = 82, //击杀仇人
	ACType_YAOSHI_TASK = 83, //妖师任务（任务类型）
	ACType_PVP_KILL = 84, //太极之巅击杀（PVP类型，击杀人数）
	ACType_DOUFACHANG_RANK = 85, //斗法场排名(名次)
	ACType_DOUFACHANG_BUY_CHALLENGE = 86, //斗法场购买挑战次数
	ACType_DAILY_ANSWER_ALL_RIGHT = 87, //每日答题全对
	ACType_ACTIVENESS = 88, //活跃度（活跃度）
	ACType_RAID_PASS_TYPE = 89, //副本通关（副本类型）
};

//成就用到的货币类型
enum
{
	ACurrency_COIN = 1, //银两
	ACurrency_GOLD = 2, //通用元宝
	ACurrency_BIND_GOLD = 3, //绑定元宝
	ACurrency_UNBIND_GOLD = 4, //非绑元宝
	ACurrency_GUILD_DONATION = 5, //帮贡
	ACurrency_CHENGJIE_COIN = 6, //惩戒货币
	ACurrency_GUOYU_COIN = 7, //国御货币
	ACurrency_SHANGJIN_COIN = 8, //赏金货币
};

//称号条件类型
enum
{
	TCType_RANK_RANKING = 1, //排行榜名次
	TCType_MURDER_NUM = 2, //杀戮值在N点以上
	TCType_FASHION_ID = 3, //拥有指定时装
	TCType_HORSE_ID = 4, //拥有指定坐骑
};

//奖励找回类型
enum 
{
	REWARD_BACK_NORMO_RAID_TYPE_ACTIVE = 1, //普通副本类型活动
	REWARD_BACK_GUANFU_XUANSHANG_ACTIVE	= 2, //官府悬赏
	REWARD_BACK_LIANGCAO_YAOBIAO_ACTIVE = 3,  // 粮草押镖
	REWARD_BACK_MIJING_XIULIAN_ACTIVE = 4,    //秘境修炼
	REWARD_BACK_JIANGHU_YOULI_ACTIVE = 5,    //游历任务
};

//送礼消耗货币类型
enum
{
	FRIENF_SONGLI_SUB_ACER = 54,   //消耗元宝
	FRIENF_SONGLI_SUB_GOLD = 55,   //消耗金票
	FRIENF_SONGLI_SUB_COIN = 56,   //消耗银两
};

//玩家婚姻状态
enum
{
	MARRY_STATU_SINGLE_NOT_MARRIAGE_HISTORY = 0,   //单身(且无婚史)
	MARRY_STATU_SINGLE_HAVE_MARRIAGE_HISTORY = 1,  //单身(且有婚史)
	MARRY_STATU_HAVE_PROPOSE_NOT_RESERVE_MARRY = 2,  //当前已经订婚(未预定婚礼)
	MARRY_STATU_HAVE_PROPOSE_HAVE_RESERVE_MARRY = 3,  //当前已经订婚(已预定婚礼)
	MARRY_STATU_HAVE_ALREADY_MARRIED   = 4,  //已婚
};

//求婚过程中玩家扮演的角色
enum 
{
	MARRY_PROPOSE_IS_ACTIVE = 0, //主动求婚者
	MARRY_PROPOSE_IS_PASSIVE = 1, //被动求婚者
};
#endif
