local DropConfigTable = {
	[220100001] = {
		['ID'] = 220100001,	--掉落组ID
		['ProType'] = 0,	--概率计算方式
		['DropID'] = {201010001},	--掉落ID
		['Probability'] = {10000},	--掉落概率
		['NumMin'] = {1000},	--最小值
		['NumMax'] = {1000}	--最大值
		},
	[220100002] = {
		['ID'] = 220100002,
		['ProType'] = 0,
		['DropID'] = {201010002},
		['Probability'] = {10000},
		['NumMin'] = {1000},
		['NumMax'] = {1000}
		},
	[220100003] = {
		['ID'] = 220100003,
		['ProType'] = 0,
		['DropID'] = {201010003},
		['Probability'] = {10000},
		['NumMin'] = {1000},
		['NumMax'] = {1000}
		},
	[220100004] = {
		['ID'] = 220100004,
		['ProType'] = 0,
		['DropID'] = {201010004},
		['Probability'] = {10000},
		['NumMin'] = {1000},
		['NumMax'] = {1000}
		},
	[220100005] = {
		['ID'] = 220100005,
		['ProType'] = 0,
		['DropID'] = {201010001,201010003},
		['Probability'] = {5000,5000},
		['NumMin'] = {1000,1000},
		['NumMax'] = {1000,1000}
		},
	[220100006] = {
		['ID'] = 220100006,
		['ProType'] = 1,
		['DropID'] = {201010001,201010003},
		['Probability'] = {5000,5000},
		['NumMin'] = {1000,1000},
		['NumMax'] = {1000,1000}
		},
	[220100007] = {
		['ID'] = 220100007,
		['ProType'] = 0,
		['DropID'] = {201010002},
		['Probability'] = {10000},
		['NumMin'] = {1000},
		['NumMax'] = {2000}
		},
	[220100008] = {
		['ID'] = 220100008,
		['ProType'] = 1,
		['DropID'] = {201020001,201010001},
		['Probability'] = {5000,10000},
		['NumMin'] = {1,1000},
		['NumMax'] = {4,1000}
		},
	[220100009] = {
		['ID'] = 220100009,
		['ProType'] = 1,
		['DropID'] = {201010001},
		['Probability'] = {5000},
		['NumMin'] = {1000},
		['NumMax'] = {2000}
		},
	[220100010] = {
		['ID'] = 220100010,
		['ProType'] = 1,
		['DropID'] = {220100007,201020001},
		['Probability'] = {10000,10000},
		['NumMin'] = {0,1},
		['NumMax'] = {0,2}
		},
	[220100011] = {
		['ID'] = 220100011,
		['ProType'] = 1,
		['DropID'] = {201020004,201020005},
		['Probability'] = {10000,10000},
		['NumMin'] = {20,20},
		['NumMax'] = {20,20}
		},
	[220100012] = {
		['ID'] = 220100012,
		['ProType'] = 1,
		['DropID'] = {201010001,201010004},
		['Probability'] = {10000,10000},
		['NumMin'] = {5000,5000},
		['NumMax'] = {5000,5000}
		},
	[220100013] = {
		['ID'] = 220100013,
		['ProType'] = 1,
		['DropID'] = {201010001,201010004},
		['Probability'] = {10000,10000},
		['NumMin'] = {10000,10000},
		['NumMax'] = {10000,10000}
		},
	[220100014] = {
		['ID'] = 220100014,
		['ProType'] = 1,
		['DropID'] = {201010001,201010004},
		['Probability'] = {10000,10000},
		['NumMin'] = {20000,20000},
		['NumMax'] = {20000,20000}
		},
	[220100015] = {
		['ID'] = 220100015,
		['ProType'] = 1,
		['DropID'] = {201010001,201010004},
		['Probability'] = {10000,10000},
		['NumMin'] = {50000,50000},
		['NumMax'] = {50000,50000}
		},
	[220100016] = {
		['ID'] = 220100016,
		['ProType'] = 1,
		['DropID'] = {201010001,201010004},
		['Probability'] = {10000,10000},
		['NumMin'] = {100000,100000},
		['NumMax'] = {100000,100000}
		},
	[220100017] = {
		['ID'] = 220100017,
		['ProType'] = 1,
		['DropID'] = {201010005,201010008},
		['Probability'] = {10000,10000},
		['NumMin'] = {1000,2000},
		['NumMax'] = {1000,2000}
		},
	[220100018] = {
		['ID'] = 220100018,
		['ProType'] = 1,
		['DropID'] = {201010005,201010008},
		['Probability'] = {10000,10000},
		['NumMin'] = {2000,3000},
		['NumMax'] = {2000,3000}
		},
	[220100019] = {
		['ID'] = 220100019,
		['ProType'] = 1,
		['DropID'] = {201010005,201010008},
		['Probability'] = {10000,10000},
		['NumMin'] = {3000,4000},
		['NumMax'] = {3000,4000}
		},
	[220100020] = {
		['ID'] = 220100020,
		['ProType'] = 1,
		['DropID'] = {201010005,201010008},
		['Probability'] = {10000,10000},
		['NumMin'] = {4000,5000},
		['NumMax'] = {4000,5000}
		},
	[220100221] = {
		['ID'] = 220100221,
		['ProType'] = 1,
		['DropID'] = {201070019,201070020,201070021,201070022,201070023,201070017},
		['Probability'] = {10000,10000,10000,10000,10000,10000},
		['NumMin'] = {2,2,2,2,2,2},
		['NumMax'] = {2,2,2,2,2,2}
		},
	[220100222] = {
		['ID'] = 220100222,
		['ProType'] = 1,
		['DropID'] = {201060014},
		['Probability'] = {10000},
		['NumMin'] = {2},
		['NumMax'] = {2}
		},
	[220100223] = {
		['ID'] = 220100223,
		['ProType'] = 1,
		['DropID'] = {201060015},
		['Probability'] = {10000},
		['NumMin'] = {2},
		['NumMax'] = {2}
		},
	[220100224] = {
		['ID'] = 220100224,
		['ProType'] = 1,
		['DropID'] = {201070025,201070026},
		['Probability'] = {10000,10000},
		['NumMin'] = {5,5},
		['NumMax'] = {5,5}
		},
	[220100225] = {
		['ID'] = 220100225,
		['ProType'] = 1,
		['DropID'] = {201050017},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100226] = {
		['ID'] = 220100226,
		['ProType'] = 1,
		['DropID'] = {201050018},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100227] = {
		['ID'] = 220100227,
		['ProType'] = 1,
		['DropID'] = {201050004},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100228] = {
		['ID'] = 220100228,
		['ProType'] = 1,
		['DropID'] = {201050008},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100229] = {
		['ID'] = 220100229,
		['ProType'] = 1,
		['DropID'] = {201050009},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100230] = {
		['ID'] = 220100230,
		['ProType'] = 1,
		['DropID'] = {201050010},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100231] = {
		['ID'] = 220100231,
		['ProType'] = 1,
		['DropID'] = {201050022},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100232] = {
		['ID'] = 220100232,
		['ProType'] = 1,
		['DropID'] = {201050025},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100233] = {
		['ID'] = 220100233,
		['ProType'] = 1,
		['DropID'] = {201050026},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100234] = {
		['ID'] = 220100234,
		['ProType'] = 1,
		['DropID'] = {201050019},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100235] = {
		['ID'] = 220100235,
		['ProType'] = 1,
		['DropID'] = {201050027},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100236] = {
		['ID'] = 220100236,
		['ProType'] = 1,
		['DropID'] = {201050030},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100237] = {
		['ID'] = 220100237,
		['ProType'] = 1,
		['DropID'] = {201050031},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220100238] = {
		['ID'] = 220100238,
		['ProType'] = 1,
		['DropID'] = {201050032},
		['Probability'] = {10000},
		['NumMin'] = {1},
		['NumMax'] = {1}
		},
	[220101001] = {
		['ID'] = 220101001,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {5,500},
		['NumMax'] = {5,500}
		},
	[220101002] = {
		['ID'] = 220101002,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {10,600},
		['NumMax'] = {10,600}
		},
	[220101003] = {
		['ID'] = 220101003,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {15,700},
		['NumMax'] = {15,700}
		},
	[220101004] = {
		['ID'] = 220101004,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {20,800},
		['NumMax'] = {20,800}
		},
	[220101005] = {
		['ID'] = 220101005,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {25,900},
		['NumMax'] = {25,900}
		},
	[220101006] = {
		['ID'] = 220101006,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {30,1000},
		['NumMax'] = {30,1000}
		},
	[220101007] = {
		['ID'] = 220101007,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {35,1100},
		['NumMax'] = {35,1100}
		},
	[220101008] = {
		['ID'] = 220101008,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {40,1200},
		['NumMax'] = {40,1200}
		},
	[220101009] = {
		['ID'] = 220101009,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {45,1300},
		['NumMax'] = {45,1300}
		},
	[220101010] = {
		['ID'] = 220101010,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {50,1400},
		['NumMax'] = {50,1400}
		},
	[220101011] = {
		['ID'] = 220101011,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {55,1500},
		['NumMax'] = {55,1500}
		},
	[220101012] = {
		['ID'] = 220101012,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {60,1600},
		['NumMax'] = {60,1600}
		},
	[220101013] = {
		['ID'] = 220101013,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {65,1700},
		['NumMax'] = {65,1700}
		},
	[220101014] = {
		['ID'] = 220101014,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {70,1800},
		['NumMax'] = {70,1800}
		},
	[220101015] = {
		['ID'] = 220101015,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {75,1900},
		['NumMax'] = {75,1900}
		},
	[220101016] = {
		['ID'] = 220101016,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {80,2000},
		['NumMax'] = {80,2000}
		},
	[220101017] = {
		['ID'] = 220101017,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {85,2100},
		['NumMax'] = {85,2100}
		},
	[220101018] = {
		['ID'] = 220101018,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {90,2200},
		['NumMax'] = {90,2200}
		},
	[220101019] = {
		['ID'] = 220101019,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {95,2300},
		['NumMax'] = {95,2300}
		},
	[220101020] = {
		['ID'] = 220101020,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {100,2400},
		['NumMax'] = {100,2400}
		},
	[220101021] = {
		['ID'] = 220101021,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,5000},
		['NumMin'] = {110,2500},
		['NumMax'] = {110,2500}
		},
	[220102001] = {
		['ID'] = 220102001,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {50,5000},
		['NumMax'] = {50,5000}
		},
	[220102002] = {
		['ID'] = 220102002,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {100,6000},
		['NumMax'] = {100,6000}
		},
	[220102003] = {
		['ID'] = 220102003,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {150,7000},
		['NumMax'] = {150,7000}
		},
	[220102004] = {
		['ID'] = 220102004,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {200,8000},
		['NumMax'] = {200,8000}
		},
	[220102005] = {
		['ID'] = 220102005,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {250,9000},
		['NumMax'] = {250,9000}
		},
	[220102006] = {
		['ID'] = 220102006,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {300,10000},
		['NumMax'] = {300,10000}
		},
	[220102007] = {
		['ID'] = 220102007,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {350,11000},
		['NumMax'] = {350,11000}
		},
	[220102008] = {
		['ID'] = 220102008,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {400,12000},
		['NumMax'] = {400,12000}
		},
	[220102009] = {
		['ID'] = 220102009,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {450,13000},
		['NumMax'] = {450,13000}
		},
	[220102010] = {
		['ID'] = 220102010,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {500,14000},
		['NumMax'] = {500,14000}
		},
	[220102011] = {
		['ID'] = 220102011,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {550,15000},
		['NumMax'] = {550,15000}
		},
	[220102012] = {
		['ID'] = 220102012,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {600,16000},
		['NumMax'] = {600,16000}
		},
	[220102013] = {
		['ID'] = 220102013,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {650,17000},
		['NumMax'] = {650,17000}
		},
	[220102014] = {
		['ID'] = 220102014,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {700,18000},
		['NumMax'] = {700,18000}
		},
	[220102015] = {
		['ID'] = 220102015,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {750,19000},
		['NumMax'] = {750,19000}
		},
	[220102016] = {
		['ID'] = 220102016,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {800,20000},
		['NumMax'] = {800,20000}
		},
	[220102017] = {
		['ID'] = 220102017,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {850,21000},
		['NumMax'] = {850,21000}
		},
	[220102018] = {
		['ID'] = 220102018,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {900,22000},
		['NumMax'] = {900,22000}
		},
	[220102019] = {
		['ID'] = 220102019,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {950,23000},
		['NumMax'] = {950,23000}
		},
	[220102020] = {
		['ID'] = 220102020,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1000,24000},
		['NumMax'] = {1000,24000}
		},
	[220102021] = {
		['ID'] = 220102021,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1100,25000},
		['NumMax'] = {1100,25000}
		},
	[220103001] = {
		['ID'] = 220103001,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {150,15000},
		['NumMax'] = {150,15000}
		},
	[220103002] = {
		['ID'] = 220103002,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {300,18000},
		['NumMax'] = {300,18000}
		},
	[220103003] = {
		['ID'] = 220103003,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {450,21000},
		['NumMax'] = {450,21000}
		},
	[220103004] = {
		['ID'] = 220103004,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {600,24000},
		['NumMax'] = {600,24000}
		},
	[220103005] = {
		['ID'] = 220103005,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {750,27000},
		['NumMax'] = {750,27000}
		},
	[220103006] = {
		['ID'] = 220103006,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {900,30000},
		['NumMax'] = {900,30000}
		},
	[220103007] = {
		['ID'] = 220103007,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1050,33000},
		['NumMax'] = {1050,33000}
		},
	[220103008] = {
		['ID'] = 220103008,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1200,36000},
		['NumMax'] = {1200,36000}
		},
	[220103009] = {
		['ID'] = 220103009,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1350,39000},
		['NumMax'] = {1350,39000}
		},
	[220103010] = {
		['ID'] = 220103010,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1500,42000},
		['NumMax'] = {1500,42000}
		},
	[220103011] = {
		['ID'] = 220103011,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1650,45000},
		['NumMax'] = {1650,45000}
		},
	[220103012] = {
		['ID'] = 220103012,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1800,48000},
		['NumMax'] = {1800,48000}
		},
	[220103013] = {
		['ID'] = 220103013,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {1950,51000},
		['NumMax'] = {1950,51000}
		},
	[220103014] = {
		['ID'] = 220103014,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {2100,54000},
		['NumMax'] = {2100,54000}
		},
	[220103015] = {
		['ID'] = 220103015,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {2250,57000},
		['NumMax'] = {2250,57000}
		},
	[220103016] = {
		['ID'] = 220103016,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {2400,60000},
		['NumMax'] = {2400,60000}
		},
	[220103017] = {
		['ID'] = 220103017,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {2550,63000},
		['NumMax'] = {2550,63000}
		},
	[220103018] = {
		['ID'] = 220103018,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {2700,66000},
		['NumMax'] = {2700,66000}
		},
	[220103019] = {
		['ID'] = 220103019,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {2850,69000},
		['NumMax'] = {2850,69000}
		},
	[220103020] = {
		['ID'] = 220103020,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {3000,72000},
		['NumMax'] = {3000,72000}
		},
	[220103021] = {
		['ID'] = 220103021,
		['ProType'] = 1,
		['DropID'] = {201010004,201010001},
		['Probability'] = {10000,10000},
		['NumMin'] = {3300,75000},
		['NumMax'] = {3300,75000}
		}
	}
return DropConfigTable
