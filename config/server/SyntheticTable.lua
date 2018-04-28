local SyntheticTable = {
	[651000001] = {
		['ID'] = 651000001,	--索引
		['SyntheticTarget'] = 201060002,	--合成目标
		['SyntheticMaterial'] = {201060001},	--合成材料
		['SyntheticMaterialNum'] = {4},	--合成材料所需数量
		['Consume'] = 2000	--单次合成所需银票数量
		},
	[651000002] = {
		['ID'] = 651000002,
		['SyntheticTarget'] = 201060003,
		['SyntheticMaterial'] = {201060002},
		['SyntheticMaterialNum'] = {4},
		['Consume'] = 10000
		},
	[651000003] = {
		['ID'] = 651000003,
		['SyntheticTarget'] = 201060028,
		['SyntheticMaterial'] = {201060027},
		['SyntheticMaterialNum'] = {5},
		['Consume'] = 10000
		},
	[651000004] = {
		['ID'] = 651000004,
		['SyntheticTarget'] = 201060029,
		['SyntheticMaterial'] = {201060028},
		['SyntheticMaterialNum'] = {5},
		['Consume'] = 50000
		},
	[651000005] = {
		['ID'] = 651000005,
		['SyntheticTarget'] = 201060231,
		['SyntheticMaterial'] = {201060230,201060004},
		['SyntheticMaterialNum'] = {3,2},
		['Consume'] = 800
		},
	[651000006] = {
		['ID'] = 651000006,
		['SyntheticTarget'] = 201060232,
		['SyntheticMaterial'] = {201060231,201060012,201060010},
		['SyntheticMaterialNum'] = {3,2,1},
		['Consume'] = 1000
		},
	[651000007] = {
		['ID'] = 651000007,
		['SyntheticTarget'] = 201060233,
		['SyntheticMaterial'] = {201060232,201060013},
		['SyntheticMaterialNum'] = {3,4},
		['Consume'] = 800
		},
	[651000008] = {
		['ID'] = 651000008,
		['SyntheticTarget'] = 201050124,
		['SyntheticMaterial'] = {201050121,201050122,201050123},
		['SyntheticMaterialNum'] = {1,1,1},
		['Consume'] = 0
		}
	}
return SyntheticTable
