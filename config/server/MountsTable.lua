local MountsTable = {
	[180000001] = {
		['ID'] = 180000001,	--索引
		['Name'] = '飞剑',	--名称
		['ResId'] = 130500002,	--模型编号
		['BaseAttribute'] = 102200001,	--属性组id
		['MountsAttribute'] = {21,46,2,3,16,15},	--坐骑属性
		['AttributeCeiling'] = {15,15,550,30,50,20},	--属性上限
		['LockType'] = 0,	--解锁类型
		['TypeParameter'] = 0,	--类型参数
		['LockCondition'] = '0',	--解锁条件
		['Time'] = {0,20,2592000},	--有效时间
		['WingBinding'] = {2380,480,980},	--绑定元宝
		['Binding'] = {2380,480,980},	--元宝
		['Item'] = {201060196,201060196,201060196},	--道具兑换
		['ItemNum'] = {50,10,30},	--道具数量
		['ItemQuality'] = 3,	--道具品质
		['Lock'] = 2,	--锁定状态
		['ListAcc'] = 1,	--列表显示
		['PrintMounts'] = 1	--显示坐骑
		},
	[180000004] = {
		['ID'] = 180000004,
		['Name'] = '凤凰',
		['ResId'] = 130500001,
		['BaseAttribute'] = 102200001,
		['MountsAttribute'] = {21,46,2,3,16,15},
		['AttributeCeiling'] = {15,15,550,30,50,20},
		['LockType'] = 0,
		['TypeParameter'] = 0,
		['LockCondition'] = '0',
		['Time'] = {0,604800,2592000},
		['WingBinding'] = {10000,1000,5000},
		['Binding'] = {10000,1000,5000},
		['Item'] = {},
		['ItemNum'] = {},
		['ItemQuality'] = 3,
		['Lock'] = 1,
		['ListAcc'] = 1,
		['PrintMounts'] = 1
		}
	}
return MountsTable
