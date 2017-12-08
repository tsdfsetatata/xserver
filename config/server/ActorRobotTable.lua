local ActorRobotTable = {
	[107000001] = {
		['ID'] = 107000001,	--索引
		['Type'] = 1,	--类型
		['WeaponId'] = {103010101,103010102},	--随机武器
		['HairResId'] = {103010201,103010202},	--随机头饰
		['ResId'] = {103010301,103010302},	--随机时装
		['InitialHead'] = 105900001,	--机器人初始头像
		['Skill'] = {111100102,111100103,111100105,111100101,111100111,111100121},	--主动技能
		['PassiveSkill'] = {0},	--被动技能
		['FightPro'] = {100,120},	--战力系数
		['AttributeType'] = {2,3,9,16,15},	--分配的属性
		['AttributePro'] = {240,7,4,8,2},	--分配的比例
		['ActiveAttackRange'] = 5,	--主动攻击范围
		['ChaseRange'] = 10	--追击范围
		},
	[107000002] = {
		['ID'] = 107000002,
		['Type'] = 1,
		['WeaponId'] = {103020101,103020102},
		['HairResId'] = {103020201,103020202},
		['ResId'] = {103020301,103020302},
		['InitialHead'] = 105900002,
		['Skill'] = {111100202,111100203,111100205,111100201,111100207,111100208},
		['PassiveSkill'] = {0},
		['FightPro'] = {100,120},
		['AttributeType'] = {2,3,9,16,15},
		['AttributePro'] = {180,7,4,8,4},
		['ActiveAttackRange'] = 5,
		['ChaseRange'] = 10
		},
	[107000003] = {
		['ID'] = 107000003,
		['Type'] = 1,
		['WeaponId'] = {103030101},
		['HairResId'] = {103030201},
		['ResId'] = {103030301},
		['InitialHead'] = 105900003,
		['Skill'] = {111100302,111100303,111100305,111100301,111100307,111100308},
		['PassiveSkill'] = {0},
		['FightPro'] = {100,120},
		['AttributeType'] = {2,3,9,16,15},
		['AttributePro'] = {195,7,4,8,3},
		['ActiveAttackRange'] = 5,
		['ChaseRange'] = 10
		},
	[107000004] = {
		['ID'] = 107000004,
		['Type'] = 1,
		['WeaponId'] = {103040101,103040102,103040103},
		['HairResId'] = {103040201,103040202},
		['ResId'] = {103040301},
		['InitialHead'] = 105900004,
		['Skill'] = {111100402,111100403,111100405,111100401,111100407,111100408},
		['PassiveSkill'] = {0},
		['FightPro'] = {100,120},
		['AttributeType'] = {2,3,9,16,15},
		['AttributePro'] = {210,7,4,8,5},
		['ActiveAttackRange'] = 5,
		['ChaseRange'] = 10
		},
	[107000005] = {
		['ID'] = 107000005,
		['Type'] = 1,
		['WeaponId'] = {103040101,103040102,103040103},
		['HairResId'] = {103040201,103040202},
		['ResId'] = {103040301},
		['InitialHead'] = 105900005,
		['Skill'] = {111100402,111100403,111100405,111100401,111100407,111100408},
		['PassiveSkill'] = {0},
		['FightPro'] = {100,120},
		['AttributeType'] = {2,3,9,16,15},
		['AttributePro'] = {165,7,4,8,3},
		['ActiveAttackRange'] = 5,
		['ChaseRange'] = 10
		},
	[107020001] = {
		['ID'] = 107020001,
		['Type'] = 2,
		['WeaponId'] = {103010101,103010102},
		['HairResId'] = {103010201,103010202},
		['ResId'] = {103010301,103010302},
		['InitialHead'] = 105900001,
		['Skill'] = {111100102,111100103,111100105,111100101,111100111,111100121},
		['PassiveSkill'] = {0},
		['FightPro'] = {310,320},
		['AttributeType'] = {2,3,9,16},
		['AttributePro'] = {320,10,3,10},
		['ActiveAttackRange'] = 25,
		['ChaseRange'] = 20
		},
	[107020002] = {
		['ID'] = 107020002,
		['Type'] = 2,
		['WeaponId'] = {103020101,103020102},
		['HairResId'] = {103020201,103020202},
		['ResId'] = {103020301,103020302},
		['InitialHead'] = 105900003,
		['Skill'] = {111100202,111100203,111100205,111100201,111100207,111100208},
		['PassiveSkill'] = {0},
		['FightPro'] = {290,300},
		['AttributeType'] = {2,3,9,16},
		['AttributePro'] = {200,10,3,10},
		['ActiveAttackRange'] = 25,
		['ChaseRange'] = 20
		},
	[107020003] = {
		['ID'] = 107020003,
		['Type'] = 2,
		['WeaponId'] = {103030101},
		['HairResId'] = {103030201},
		['ResId'] = {103030301},
		['InitialHead'] = 105900001,
		['Skill'] = {111100302,111100303,111100305,111100301,111100307,111100308},
		['PassiveSkill'] = {0},
		['FightPro'] = {300,310},
		['AttributeType'] = {2,3,9,16},
		['AttributePro'] = {230,10,3,10},
		['ActiveAttackRange'] = 25,
		['ChaseRange'] = 20
		},
	[107020004] = {
		['ID'] = 107020004,
		['Type'] = 2,
		['WeaponId'] = {103040101,103040102,103040103},
		['HairResId'] = {103040201,103040202},
		['ResId'] = {103040301},
		['InitialHead'] = 105900003,
		['Skill'] = {111100402,111100403,111100405,111100401,111100407,111100408},
		['PassiveSkill'] = {0},
		['FightPro'] = {300,310},
		['AttributeType'] = {2,3,9,16},
		['AttributePro'] = {260,10,3,10},
		['ActiveAttackRange'] = 25,
		['ChaseRange'] = 20
		},
	[107020005] = {
		['ID'] = 107020005,
		['Type'] = 2,
		['WeaponId'] = {103040101,103040102,103040103},
		['HairResId'] = {103040201,103040202},
		['ResId'] = {103040301},
		['InitialHead'] = 105900003,
		['Skill'] = {111100402,111100403,111100405,111100401,111100407,111100408},
		['PassiveSkill'] = {0},
		['FightPro'] = {300,310},
		['AttributeType'] = {2,3,9,16},
		['AttributePro'] = {215,10,3,10},
		['ActiveAttackRange'] = 25,
		['ChaseRange'] = 20
		}
	}
return ActorRobotTable
