local TypeLevelTable = {
	[324000001] = {
		['ID'] = 324000001,	--索引
		['TypeLevel'] = 1,	--怪物所属悬赏级别
		['LevelTxt'] = '初级悬赏',	--悬赏名称
		['Level'] = 18,	--参与等级
		['RewardTime'] = 2,	--收益次数
		['MinActor'] = 1,	--接任务最低人数
		['MaxActor'] = 5,	--接任务最大人数
		['OpenDay'] = {0,1,2,3,4,5,6},	--开放日
		['OpenTime'] = {0},	--具体开放时间点
		['CloseTime'] = {2359},	--具体结束时间点
		['JoinTxt'] = '组队参加',	--参与形式描述
		['TimeTxt'] = '每天开放',	--活动时间描述
		['OpenProbability'] = 10000,	--开启几率
		['SpecialtyPlus'] = 0,	--专精加成
		['ShowTimes'] = 0,	--最大出现次数
		['RewardDisplay'] = {201060230,201060193,201060025,201010005,201010008,201010001,201010004}	--掉落物品显示
		},
	[324000002] = {
		['ID'] = 324000002,
		['TypeLevel'] = 2,
		['LevelTxt'] = '中级悬赏',
		['Level'] = 40,
		['RewardTime'] = 2,
		['MinActor'] = 1,
		['MaxActor'] = 5,
		['OpenDay'] = {0,1,2,3,4,5,6},
		['OpenTime'] = {0},
		['CloseTime'] = {2359},
		['JoinTxt'] = '组队参加',
		['TimeTxt'] = '每天开放',
		['OpenProbability'] = 10000,
		['SpecialtyPlus'] = 0,
		['ShowTimes'] = 0,
		['RewardDisplay'] = {201060231,201060193,201060025,201010005,201010008,201010001,201010004}
		},
	[324000003] = {
		['ID'] = 324000003,
		['TypeLevel'] = 3,
		['LevelTxt'] = '高级悬赏',
		['Level'] = 60,
		['RewardTime'] = 2,
		['MinActor'] = 1,
		['MaxActor'] = 5,
		['OpenDay'] = {0,1,2,3,4,5,6},
		['OpenTime'] = {0},
		['CloseTime'] = {2359},
		['JoinTxt'] = '组队参加',
		['TimeTxt'] = '每天开放',
		['OpenProbability'] = 10000,
		['SpecialtyPlus'] = 0,
		['ShowTimes'] = 0,
		['RewardDisplay'] = {201060232,201060193,201060025,201010005,201010008,201010001,201010004}
		},
	[324000004] = {
		['ID'] = 324000004,
		['TypeLevel'] = 4,
		['LevelTxt'] = '紧急悬赏',
		['Level'] = 20,
		['RewardTime'] = 2,
		['MinActor'] = 1,
		['MaxActor'] = 5,
		['OpenDay'] = {0,1,2,3,4,5,6},
		['OpenTime'] = {1200,1800},
		['CloseTime'] = {1230,1830},
		['JoinTxt'] = '组队参加',
		['TimeTxt'] = '每天12点,18点有几率出现',
		['OpenProbability'] = 10000,
		['SpecialtyPlus'] = 0,
		['ShowTimes'] = 1,
		['RewardDisplay'] = {201060233,201060193,201060025,201010005,201010008,201010001,201010004}
		}
	}
return TypeLevelTable
