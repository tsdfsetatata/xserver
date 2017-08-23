-- Generated By protoc-gen-lua Do not Edit
local protobuf = require "protobuf"
local comm_message_pb = require("comm_message_pb")
module('guild_battle_pb')


local GUILDBATTLEWAITINFONOTIFY = protobuf.Descriptor();
local GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERANKDATA = protobuf.Descriptor();
local GUILDBATTLERANKDATA_RANK_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERANKDATA_GUILDID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERANKDATA_GUILDNAME_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERANKDATA_GUILDSCORE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERANKDATA_GUILDCAMP_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLECONTRIBUTIONDATA = protobuf.Descriptor();
local GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEINFOANSWER = protobuf.Descriptor();
local GUILDBATTLEINFOANSWER_RESULT_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEINFOANSWER_RANKS_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEINFOANSWER_MYSCORE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLECALLNOTIFY = protobuf.Descriptor();
local GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLECALLNOTIFY_CALLERID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEMATCHDATA = protobuf.Descriptor();
local GUILDBATTLEMATCHDATA_GUILDID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEMATCHDATA_GUILDNAME_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEMATCHDATA_MEMBERS_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEMATCHNOTIFY = protobuf.Descriptor();
local GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERECORDDATA = protobuf.Descriptor();
local GUILDBATTLERECORDDATA_PLAYERID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERECORDDATA_KILL_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERECORDDATA_DEAD_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERECORDDATA_BOSS_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERECORDDATA_MONSTER_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERECORDDATA_BOSSKILLER_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLERECORDNOTIFY = protobuf.Descriptor();
local GUILDBATTLERECORDNOTIFY_RECORDS_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEROUNDFINISHNOTIFY = protobuf.Descriptor();
local GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEACTIVITYFINISHNOTIFY = protobuf.Descriptor();
local GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEROUNDINFONOTIFY = protobuf.Descriptor();
local GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEKILLNOTIFY = protobuf.Descriptor();
local GUILDBATTLEKILLNOTIFY_KILLERID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEKILLNOTIFY_DEADID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEBOSSDAMAGENOTIFY = protobuf.Descriptor();
local GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD = protobuf.FieldDescriptor();
local GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD = protobuf.FieldDescriptor();

GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.name = "participateNum"
GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.full_name = ".GuildBattleWaitInfoNotify.participateNum"
GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.number = 1
GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.index = 0
GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.label = 1
GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.has_default_value = false
GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.default_value = 0
GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.type = 13
GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD.cpp_type = 3

GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.name = "guildScore"
GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.full_name = ".GuildBattleWaitInfoNotify.guildScore"
GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.number = 2
GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.index = 1
GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.label = 1
GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.has_default_value = false
GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.default_value = 0
GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.type = 13
GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD.cpp_type = 3

GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.name = "round"
GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.full_name = ".GuildBattleWaitInfoNotify.round"
GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.number = 3
GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.index = 2
GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.label = 1
GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.has_default_value = false
GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.default_value = 0
GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.type = 13
GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD.cpp_type = 3

GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.name = "waitTime"
GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.full_name = ".GuildBattleWaitInfoNotify.waitTime"
GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.number = 4
GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.index = 3
GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.label = 1
GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.has_default_value = false
GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.default_value = 0
GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.type = 13
GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD.cpp_type = 3

GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.name = "callCD"
GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.full_name = ".GuildBattleWaitInfoNotify.callCD"
GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.number = 5
GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.index = 4
GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.label = 1
GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.has_default_value = false
GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.default_value = 0
GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.type = 13
GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD.cpp_type = 3

GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.name = "activityId"
GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.full_name = ".GuildBattleWaitInfoNotify.activityId"
GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.number = 6
GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.index = 5
GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.label = 1
GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.has_default_value = false
GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.default_value = 0
GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.type = 13
GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD.cpp_type = 3

GUILDBATTLEWAITINFONOTIFY.name = "GuildBattleWaitInfoNotify"
GUILDBATTLEWAITINFONOTIFY.full_name = ".GuildBattleWaitInfoNotify"
GUILDBATTLEWAITINFONOTIFY.nested_types = {}
GUILDBATTLEWAITINFONOTIFY.enum_types = {}
GUILDBATTLEWAITINFONOTIFY.fields = {GUILDBATTLEWAITINFONOTIFY_PARTICIPATENUM_FIELD, GUILDBATTLEWAITINFONOTIFY_GUILDSCORE_FIELD, GUILDBATTLEWAITINFONOTIFY_ROUND_FIELD, GUILDBATTLEWAITINFONOTIFY_WAITTIME_FIELD, GUILDBATTLEWAITINFONOTIFY_CALLCD_FIELD, GUILDBATTLEWAITINFONOTIFY_ACTIVITYID_FIELD}
GUILDBATTLEWAITINFONOTIFY.is_extendable = false
GUILDBATTLEWAITINFONOTIFY.extensions = {}
GUILDBATTLERANKDATA_RANK_FIELD.name = "rank"
GUILDBATTLERANKDATA_RANK_FIELD.full_name = ".GuildBattleRankData.rank"
GUILDBATTLERANKDATA_RANK_FIELD.number = 1
GUILDBATTLERANKDATA_RANK_FIELD.index = 0
GUILDBATTLERANKDATA_RANK_FIELD.label = 2
GUILDBATTLERANKDATA_RANK_FIELD.has_default_value = false
GUILDBATTLERANKDATA_RANK_FIELD.default_value = 0
GUILDBATTLERANKDATA_RANK_FIELD.type = 13
GUILDBATTLERANKDATA_RANK_FIELD.cpp_type = 3

GUILDBATTLERANKDATA_GUILDID_FIELD.name = "guildId"
GUILDBATTLERANKDATA_GUILDID_FIELD.full_name = ".GuildBattleRankData.guildId"
GUILDBATTLERANKDATA_GUILDID_FIELD.number = 2
GUILDBATTLERANKDATA_GUILDID_FIELD.index = 1
GUILDBATTLERANKDATA_GUILDID_FIELD.label = 2
GUILDBATTLERANKDATA_GUILDID_FIELD.has_default_value = false
GUILDBATTLERANKDATA_GUILDID_FIELD.default_value = 0
GUILDBATTLERANKDATA_GUILDID_FIELD.type = 13
GUILDBATTLERANKDATA_GUILDID_FIELD.cpp_type = 3

GUILDBATTLERANKDATA_GUILDNAME_FIELD.name = "guildName"
GUILDBATTLERANKDATA_GUILDNAME_FIELD.full_name = ".GuildBattleRankData.guildName"
GUILDBATTLERANKDATA_GUILDNAME_FIELD.number = 3
GUILDBATTLERANKDATA_GUILDNAME_FIELD.index = 2
GUILDBATTLERANKDATA_GUILDNAME_FIELD.label = 2
GUILDBATTLERANKDATA_GUILDNAME_FIELD.has_default_value = false
GUILDBATTLERANKDATA_GUILDNAME_FIELD.default_value = ""
GUILDBATTLERANKDATA_GUILDNAME_FIELD.type = 9
GUILDBATTLERANKDATA_GUILDNAME_FIELD.cpp_type = 9

GUILDBATTLERANKDATA_GUILDSCORE_FIELD.name = "guildScore"
GUILDBATTLERANKDATA_GUILDSCORE_FIELD.full_name = ".GuildBattleRankData.guildScore"
GUILDBATTLERANKDATA_GUILDSCORE_FIELD.number = 4
GUILDBATTLERANKDATA_GUILDSCORE_FIELD.index = 3
GUILDBATTLERANKDATA_GUILDSCORE_FIELD.label = 2
GUILDBATTLERANKDATA_GUILDSCORE_FIELD.has_default_value = false
GUILDBATTLERANKDATA_GUILDSCORE_FIELD.default_value = 0
GUILDBATTLERANKDATA_GUILDSCORE_FIELD.type = 13
GUILDBATTLERANKDATA_GUILDSCORE_FIELD.cpp_type = 3

GUILDBATTLERANKDATA_GUILDCAMP_FIELD.name = "guildCamp"
GUILDBATTLERANKDATA_GUILDCAMP_FIELD.full_name = ".GuildBattleRankData.guildCamp"
GUILDBATTLERANKDATA_GUILDCAMP_FIELD.number = 5
GUILDBATTLERANKDATA_GUILDCAMP_FIELD.index = 4
GUILDBATTLERANKDATA_GUILDCAMP_FIELD.label = 2
GUILDBATTLERANKDATA_GUILDCAMP_FIELD.has_default_value = false
GUILDBATTLERANKDATA_GUILDCAMP_FIELD.default_value = 0
GUILDBATTLERANKDATA_GUILDCAMP_FIELD.type = 13
GUILDBATTLERANKDATA_GUILDCAMP_FIELD.cpp_type = 3

GUILDBATTLERANKDATA.name = "GuildBattleRankData"
GUILDBATTLERANKDATA.full_name = ".GuildBattleRankData"
GUILDBATTLERANKDATA.nested_types = {}
GUILDBATTLERANKDATA.enum_types = {}
GUILDBATTLERANKDATA.fields = {GUILDBATTLERANKDATA_RANK_FIELD, GUILDBATTLERANKDATA_GUILDID_FIELD, GUILDBATTLERANKDATA_GUILDNAME_FIELD, GUILDBATTLERANKDATA_GUILDSCORE_FIELD, GUILDBATTLERANKDATA_GUILDCAMP_FIELD}
GUILDBATTLERANKDATA.is_extendable = false
GUILDBATTLERANKDATA.extensions = {}
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.name = "baseInfo"
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.full_name = ".GuildBattleContributionData.baseInfo"
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.number = 1
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.index = 0
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.label = 2
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.has_default_value = false
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.default_value = nil
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.message_type = COMM_MESSAGE_PB_PLAYERBASEDATA
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.type = 11
GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD.cpp_type = 10

GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.name = "score"
GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.full_name = ".GuildBattleContributionData.score"
GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.number = 2
GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.index = 1
GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.label = 2
GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.has_default_value = false
GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.default_value = 0
GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.type = 13
GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD.cpp_type = 3

GUILDBATTLECONTRIBUTIONDATA.name = "GuildBattleContributionData"
GUILDBATTLECONTRIBUTIONDATA.full_name = ".GuildBattleContributionData"
GUILDBATTLECONTRIBUTIONDATA.nested_types = {}
GUILDBATTLECONTRIBUTIONDATA.enum_types = {}
GUILDBATTLECONTRIBUTIONDATA.fields = {GUILDBATTLECONTRIBUTIONDATA_BASEINFO_FIELD, GUILDBATTLECONTRIBUTIONDATA_SCORE_FIELD}
GUILDBATTLECONTRIBUTIONDATA.is_extendable = false
GUILDBATTLECONTRIBUTIONDATA.extensions = {}
GUILDBATTLEINFOANSWER_RESULT_FIELD.name = "result"
GUILDBATTLEINFOANSWER_RESULT_FIELD.full_name = ".GuildBattleInfoAnswer.result"
GUILDBATTLEINFOANSWER_RESULT_FIELD.number = 1
GUILDBATTLEINFOANSWER_RESULT_FIELD.index = 0
GUILDBATTLEINFOANSWER_RESULT_FIELD.label = 2
GUILDBATTLEINFOANSWER_RESULT_FIELD.has_default_value = false
GUILDBATTLEINFOANSWER_RESULT_FIELD.default_value = 0
GUILDBATTLEINFOANSWER_RESULT_FIELD.type = 13
GUILDBATTLEINFOANSWER_RESULT_FIELD.cpp_type = 3

GUILDBATTLEINFOANSWER_RANKS_FIELD.name = "ranks"
GUILDBATTLEINFOANSWER_RANKS_FIELD.full_name = ".GuildBattleInfoAnswer.ranks"
GUILDBATTLEINFOANSWER_RANKS_FIELD.number = 2
GUILDBATTLEINFOANSWER_RANKS_FIELD.index = 1
GUILDBATTLEINFOANSWER_RANKS_FIELD.label = 3
GUILDBATTLEINFOANSWER_RANKS_FIELD.has_default_value = false
GUILDBATTLEINFOANSWER_RANKS_FIELD.default_value = {}
GUILDBATTLEINFOANSWER_RANKS_FIELD.message_type = GUILDBATTLERANKDATA
GUILDBATTLEINFOANSWER_RANKS_FIELD.type = 11
GUILDBATTLEINFOANSWER_RANKS_FIELD.cpp_type = 10

GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.name = "contributions"
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.full_name = ".GuildBattleInfoAnswer.contributions"
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.number = 3
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.index = 2
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.label = 3
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.has_default_value = false
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.default_value = {}
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.message_type = GUILDBATTLECONTRIBUTIONDATA
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.type = 11
GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD.cpp_type = 10

GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.name = "myGuildRank"
GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.full_name = ".GuildBattleInfoAnswer.myGuildRank"
GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.number = 4
GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.index = 3
GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.label = 2
GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.has_default_value = false
GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.default_value = 0
GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.type = 13
GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD.cpp_type = 3

GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.name = "myGuildScore"
GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.full_name = ".GuildBattleInfoAnswer.myGuildScore"
GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.number = 5
GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.index = 4
GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.label = 2
GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.has_default_value = false
GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.default_value = 0
GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.type = 13
GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD.cpp_type = 3

GUILDBATTLEINFOANSWER_MYSCORE_FIELD.name = "myScore"
GUILDBATTLEINFOANSWER_MYSCORE_FIELD.full_name = ".GuildBattleInfoAnswer.myScore"
GUILDBATTLEINFOANSWER_MYSCORE_FIELD.number = 6
GUILDBATTLEINFOANSWER_MYSCORE_FIELD.index = 5
GUILDBATTLEINFOANSWER_MYSCORE_FIELD.label = 2
GUILDBATTLEINFOANSWER_MYSCORE_FIELD.has_default_value = false
GUILDBATTLEINFOANSWER_MYSCORE_FIELD.default_value = 0
GUILDBATTLEINFOANSWER_MYSCORE_FIELD.type = 13
GUILDBATTLEINFOANSWER_MYSCORE_FIELD.cpp_type = 3

GUILDBATTLEINFOANSWER.name = "GuildBattleInfoAnswer"
GUILDBATTLEINFOANSWER.full_name = ".GuildBattleInfoAnswer"
GUILDBATTLEINFOANSWER.nested_types = {}
GUILDBATTLEINFOANSWER.enum_types = {}
GUILDBATTLEINFOANSWER.fields = {GUILDBATTLEINFOANSWER_RESULT_FIELD, GUILDBATTLEINFOANSWER_RANKS_FIELD, GUILDBATTLEINFOANSWER_CONTRIBUTIONS_FIELD, GUILDBATTLEINFOANSWER_MYGUILDRANK_FIELD, GUILDBATTLEINFOANSWER_MYGUILDSCORE_FIELD, GUILDBATTLEINFOANSWER_MYSCORE_FIELD}
GUILDBATTLEINFOANSWER.is_extendable = false
GUILDBATTLEINFOANSWER.extensions = {}
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.name = "activityId"
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.full_name = ".GuildBattleCallNotify.activityId"
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.number = 1
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.index = 0
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.label = 2
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.has_default_value = false
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.default_value = 0
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.type = 13
GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD.cpp_type = 3

GUILDBATTLECALLNOTIFY_CALLERID_FIELD.name = "callerId"
GUILDBATTLECALLNOTIFY_CALLERID_FIELD.full_name = ".GuildBattleCallNotify.callerId"
GUILDBATTLECALLNOTIFY_CALLERID_FIELD.number = 2
GUILDBATTLECALLNOTIFY_CALLERID_FIELD.index = 1
GUILDBATTLECALLNOTIFY_CALLERID_FIELD.label = 1
GUILDBATTLECALLNOTIFY_CALLERID_FIELD.has_default_value = false
GUILDBATTLECALLNOTIFY_CALLERID_FIELD.default_value = 0
GUILDBATTLECALLNOTIFY_CALLERID_FIELD.type = 4
GUILDBATTLECALLNOTIFY_CALLERID_FIELD.cpp_type = 4

GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.name = "callerName"
GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.full_name = ".GuildBattleCallNotify.callerName"
GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.number = 3
GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.index = 2
GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.label = 1
GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.has_default_value = false
GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.default_value = ""
GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.type = 9
GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD.cpp_type = 9

GUILDBATTLECALLNOTIFY.name = "GuildBattleCallNotify"
GUILDBATTLECALLNOTIFY.full_name = ".GuildBattleCallNotify"
GUILDBATTLECALLNOTIFY.nested_types = {}
GUILDBATTLECALLNOTIFY.enum_types = {}
GUILDBATTLECALLNOTIFY.fields = {GUILDBATTLECALLNOTIFY_ACTIVITYID_FIELD, GUILDBATTLECALLNOTIFY_CALLERID_FIELD, GUILDBATTLECALLNOTIFY_CALLERNAME_FIELD}
GUILDBATTLECALLNOTIFY.is_extendable = false
GUILDBATTLECALLNOTIFY.extensions = {}
GUILDBATTLEMATCHDATA_GUILDID_FIELD.name = "guildId"
GUILDBATTLEMATCHDATA_GUILDID_FIELD.full_name = ".GuildBattleMatchData.guildId"
GUILDBATTLEMATCHDATA_GUILDID_FIELD.number = 1
GUILDBATTLEMATCHDATA_GUILDID_FIELD.index = 0
GUILDBATTLEMATCHDATA_GUILDID_FIELD.label = 2
GUILDBATTLEMATCHDATA_GUILDID_FIELD.has_default_value = false
GUILDBATTLEMATCHDATA_GUILDID_FIELD.default_value = 0
GUILDBATTLEMATCHDATA_GUILDID_FIELD.type = 13
GUILDBATTLEMATCHDATA_GUILDID_FIELD.cpp_type = 3

GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.name = "guildName"
GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.full_name = ".GuildBattleMatchData.guildName"
GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.number = 2
GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.index = 1
GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.label = 2
GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.has_default_value = false
GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.default_value = ""
GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.type = 9
GUILDBATTLEMATCHDATA_GUILDNAME_FIELD.cpp_type = 9

GUILDBATTLEMATCHDATA_MEMBERS_FIELD.name = "members"
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.full_name = ".GuildBattleMatchData.members"
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.number = 3
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.index = 2
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.label = 3
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.has_default_value = false
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.default_value = {}
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.message_type = COMM_MESSAGE_PB_PLAYERBASEDATA
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.type = 11
GUILDBATTLEMATCHDATA_MEMBERS_FIELD.cpp_type = 10

GUILDBATTLEMATCHDATA.name = "GuildBattleMatchData"
GUILDBATTLEMATCHDATA.full_name = ".GuildBattleMatchData"
GUILDBATTLEMATCHDATA.nested_types = {}
GUILDBATTLEMATCHDATA.enum_types = {}
GUILDBATTLEMATCHDATA.fields = {GUILDBATTLEMATCHDATA_GUILDID_FIELD, GUILDBATTLEMATCHDATA_GUILDNAME_FIELD, GUILDBATTLEMATCHDATA_MEMBERS_FIELD}
GUILDBATTLEMATCHDATA.is_extendable = false
GUILDBATTLEMATCHDATA.extensions = {}
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.name = "teams"
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.full_name = ".GuildBattleMatchNotify.teams"
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.number = 1
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.index = 0
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.label = 3
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.has_default_value = false
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.default_value = {}
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.message_type = GUILDBATTLEMATCHDATA
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.type = 11
GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD.cpp_type = 10

GUILDBATTLEMATCHNOTIFY.name = "GuildBattleMatchNotify"
GUILDBATTLEMATCHNOTIFY.full_name = ".GuildBattleMatchNotify"
GUILDBATTLEMATCHNOTIFY.nested_types = {}
GUILDBATTLEMATCHNOTIFY.enum_types = {}
GUILDBATTLEMATCHNOTIFY.fields = {GUILDBATTLEMATCHNOTIFY_TEAMS_FIELD}
GUILDBATTLEMATCHNOTIFY.is_extendable = false
GUILDBATTLEMATCHNOTIFY.extensions = {}
GUILDBATTLERECORDDATA_PLAYERID_FIELD.name = "playerId"
GUILDBATTLERECORDDATA_PLAYERID_FIELD.full_name = ".GuildBattleRecordData.playerId"
GUILDBATTLERECORDDATA_PLAYERID_FIELD.number = 1
GUILDBATTLERECORDDATA_PLAYERID_FIELD.index = 0
GUILDBATTLERECORDDATA_PLAYERID_FIELD.label = 2
GUILDBATTLERECORDDATA_PLAYERID_FIELD.has_default_value = false
GUILDBATTLERECORDDATA_PLAYERID_FIELD.default_value = 0
GUILDBATTLERECORDDATA_PLAYERID_FIELD.type = 4
GUILDBATTLERECORDDATA_PLAYERID_FIELD.cpp_type = 4

GUILDBATTLERECORDDATA_KILL_FIELD.name = "kill"
GUILDBATTLERECORDDATA_KILL_FIELD.full_name = ".GuildBattleRecordData.kill"
GUILDBATTLERECORDDATA_KILL_FIELD.number = 2
GUILDBATTLERECORDDATA_KILL_FIELD.index = 1
GUILDBATTLERECORDDATA_KILL_FIELD.label = 2
GUILDBATTLERECORDDATA_KILL_FIELD.has_default_value = false
GUILDBATTLERECORDDATA_KILL_FIELD.default_value = 0
GUILDBATTLERECORDDATA_KILL_FIELD.type = 13
GUILDBATTLERECORDDATA_KILL_FIELD.cpp_type = 3

GUILDBATTLERECORDDATA_DEAD_FIELD.name = "dead"
GUILDBATTLERECORDDATA_DEAD_FIELD.full_name = ".GuildBattleRecordData.dead"
GUILDBATTLERECORDDATA_DEAD_FIELD.number = 3
GUILDBATTLERECORDDATA_DEAD_FIELD.index = 2
GUILDBATTLERECORDDATA_DEAD_FIELD.label = 2
GUILDBATTLERECORDDATA_DEAD_FIELD.has_default_value = false
GUILDBATTLERECORDDATA_DEAD_FIELD.default_value = 0
GUILDBATTLERECORDDATA_DEAD_FIELD.type = 13
GUILDBATTLERECORDDATA_DEAD_FIELD.cpp_type = 3

GUILDBATTLERECORDDATA_BOSS_FIELD.name = "boss"
GUILDBATTLERECORDDATA_BOSS_FIELD.full_name = ".GuildBattleRecordData.boss"
GUILDBATTLERECORDDATA_BOSS_FIELD.number = 4
GUILDBATTLERECORDDATA_BOSS_FIELD.index = 3
GUILDBATTLERECORDDATA_BOSS_FIELD.label = 1
GUILDBATTLERECORDDATA_BOSS_FIELD.has_default_value = false
GUILDBATTLERECORDDATA_BOSS_FIELD.default_value = 0
GUILDBATTLERECORDDATA_BOSS_FIELD.type = 13
GUILDBATTLERECORDDATA_BOSS_FIELD.cpp_type = 3

GUILDBATTLERECORDDATA_MONSTER_FIELD.name = "monster"
GUILDBATTLERECORDDATA_MONSTER_FIELD.full_name = ".GuildBattleRecordData.monster"
GUILDBATTLERECORDDATA_MONSTER_FIELD.number = 5
GUILDBATTLERECORDDATA_MONSTER_FIELD.index = 4
GUILDBATTLERECORDDATA_MONSTER_FIELD.label = 1
GUILDBATTLERECORDDATA_MONSTER_FIELD.has_default_value = false
GUILDBATTLERECORDDATA_MONSTER_FIELD.default_value = 0
GUILDBATTLERECORDDATA_MONSTER_FIELD.type = 13
GUILDBATTLERECORDDATA_MONSTER_FIELD.cpp_type = 3

GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.name = "bossKiller"
GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.full_name = ".GuildBattleRecordData.bossKiller"
GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.number = 6
GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.index = 5
GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.label = 1
GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.has_default_value = false
GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.default_value = false
GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.type = 8
GUILDBATTLERECORDDATA_BOSSKILLER_FIELD.cpp_type = 7

GUILDBATTLERECORDDATA.name = "GuildBattleRecordData"
GUILDBATTLERECORDDATA.full_name = ".GuildBattleRecordData"
GUILDBATTLERECORDDATA.nested_types = {}
GUILDBATTLERECORDDATA.enum_types = {}
GUILDBATTLERECORDDATA.fields = {GUILDBATTLERECORDDATA_PLAYERID_FIELD, GUILDBATTLERECORDDATA_KILL_FIELD, GUILDBATTLERECORDDATA_DEAD_FIELD, GUILDBATTLERECORDDATA_BOSS_FIELD, GUILDBATTLERECORDDATA_MONSTER_FIELD, GUILDBATTLERECORDDATA_BOSSKILLER_FIELD}
GUILDBATTLERECORDDATA.is_extendable = false
GUILDBATTLERECORDDATA.extensions = {}
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.name = "records"
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.full_name = ".GuildBattleRecordNotify.records"
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.number = 1
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.index = 0
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.label = 3
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.has_default_value = false
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.default_value = {}
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.message_type = GUILDBATTLERECORDDATA
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.type = 11
GUILDBATTLERECORDNOTIFY_RECORDS_FIELD.cpp_type = 10

GUILDBATTLERECORDNOTIFY.name = "GuildBattleRecordNotify"
GUILDBATTLERECORDNOTIFY.full_name = ".GuildBattleRecordNotify"
GUILDBATTLERECORDNOTIFY.nested_types = {}
GUILDBATTLERECORDNOTIFY.enum_types = {}
GUILDBATTLERECORDNOTIFY.fields = {GUILDBATTLERECORDNOTIFY_RECORDS_FIELD}
GUILDBATTLERECORDNOTIFY.is_extendable = false
GUILDBATTLERECORDNOTIFY.extensions = {}
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.name = "result"
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.full_name = ".GuildBattleRoundFinishNotify.result"
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.number = 1
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.index = 0
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.label = 2
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.has_default_value = false
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.default_value = 0
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.type = 13
GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD.cpp_type = 3

GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.name = "score"
GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.full_name = ".GuildBattleRoundFinishNotify.score"
GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.number = 4
GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.index = 1
GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.label = 2
GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.has_default_value = false
GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.default_value = 0
GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.type = 13
GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD.cpp_type = 3

GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.name = "guildTreasure"
GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.full_name = ".GuildBattleRoundFinishNotify.guildTreasure"
GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.number = 5
GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.index = 2
GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.label = 2
GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.has_default_value = false
GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.default_value = 0
GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.type = 13
GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD.cpp_type = 3

GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.name = "guildDonation"
GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.full_name = ".GuildBattleRoundFinishNotify.guildDonation"
GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.number = 6
GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.index = 3
GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.label = 2
GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.has_default_value = false
GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.default_value = 0
GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.type = 13
GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD.cpp_type = 3

GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.name = "ranks"
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.full_name = ".GuildBattleRoundFinishNotify.ranks"
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.number = 7
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.index = 4
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.label = 3
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.has_default_value = false
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.default_value = {}
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.message_type = GUILDBATTLERANKDATA
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.type = 11
GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD.cpp_type = 10

GUILDBATTLEROUNDFINISHNOTIFY.name = "GuildBattleRoundFinishNotify"
GUILDBATTLEROUNDFINISHNOTIFY.full_name = ".GuildBattleRoundFinishNotify"
GUILDBATTLEROUNDFINISHNOTIFY.nested_types = {}
GUILDBATTLEROUNDFINISHNOTIFY.enum_types = {}
GUILDBATTLEROUNDFINISHNOTIFY.fields = {GUILDBATTLEROUNDFINISHNOTIFY_RESULT_FIELD, GUILDBATTLEROUNDFINISHNOTIFY_SCORE_FIELD, GUILDBATTLEROUNDFINISHNOTIFY_GUILDTREASURE_FIELD, GUILDBATTLEROUNDFINISHNOTIFY_GUILDDONATION_FIELD, GUILDBATTLEROUNDFINISHNOTIFY_RANKS_FIELD}
GUILDBATTLEROUNDFINISHNOTIFY.is_extendable = false
GUILDBATTLEROUNDFINISHNOTIFY.extensions = {}
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.name = "ranks"
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.full_name = ".GuildBattleActivityFinishNotify.ranks"
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.number = 1
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.index = 0
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.label = 3
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.has_default_value = false
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.default_value = {}
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.message_type = GUILDBATTLERANKDATA
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.type = 11
GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD.cpp_type = 10

GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.name = "guildRank"
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.full_name = ".GuildBattleActivityFinishNotify.guildRank"
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.number = 2
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.index = 1
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.label = 2
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.has_default_value = false
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.default_value = 0
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.type = 13
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD.cpp_type = 3

GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.name = "guildScore"
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.full_name = ".GuildBattleActivityFinishNotify.guildScore"
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.number = 3
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.index = 2
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.label = 2
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.has_default_value = false
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.default_value = 0
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.type = 13
GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD.cpp_type = 3

GUILDBATTLEACTIVITYFINISHNOTIFY.name = "GuildBattleActivityFinishNotify"
GUILDBATTLEACTIVITYFINISHNOTIFY.full_name = ".GuildBattleActivityFinishNotify"
GUILDBATTLEACTIVITYFINISHNOTIFY.nested_types = {}
GUILDBATTLEACTIVITYFINISHNOTIFY.enum_types = {}
GUILDBATTLEACTIVITYFINISHNOTIFY.fields = {GUILDBATTLEACTIVITYFINISHNOTIFY_RANKS_FIELD, GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDRANK_FIELD, GUILDBATTLEACTIVITYFINISHNOTIFY_GUILDSCORE_FIELD}
GUILDBATTLEACTIVITYFINISHNOTIFY.is_extendable = false
GUILDBATTLEACTIVITYFINISHNOTIFY.extensions = {}
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.name = "endTime"
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.full_name = ".GuildBattleRoundInfoNotify.endTime"
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.number = 1
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.index = 0
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.label = 2
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.has_default_value = false
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.default_value = 0
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.type = 13
GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD.cpp_type = 3

GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.name = "bossMaxHp"
GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.full_name = ".GuildBattleRoundInfoNotify.bossMaxHp"
GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.number = 2
GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.index = 1
GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.label = 2
GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.has_default_value = false
GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.default_value = 0
GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.type = 13
GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD.cpp_type = 3

GUILDBATTLEROUNDINFONOTIFY.name = "GuildBattleRoundInfoNotify"
GUILDBATTLEROUNDINFONOTIFY.full_name = ".GuildBattleRoundInfoNotify"
GUILDBATTLEROUNDINFONOTIFY.nested_types = {}
GUILDBATTLEROUNDINFONOTIFY.enum_types = {}
GUILDBATTLEROUNDINFONOTIFY.fields = {GUILDBATTLEROUNDINFONOTIFY_ENDTIME_FIELD, GUILDBATTLEROUNDINFONOTIFY_BOSSMAXHP_FIELD}
GUILDBATTLEROUNDINFONOTIFY.is_extendable = false
GUILDBATTLEROUNDINFONOTIFY.extensions = {}
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.name = "killerId"
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.full_name = ".GuildBattleKillNotify.killerId"
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.number = 1
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.index = 0
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.label = 2
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.has_default_value = false
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.default_value = 0
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.type = 4
GUILDBATTLEKILLNOTIFY_KILLERID_FIELD.cpp_type = 4

GUILDBATTLEKILLNOTIFY_DEADID_FIELD.name = "deadId"
GUILDBATTLEKILLNOTIFY_DEADID_FIELD.full_name = ".GuildBattleKillNotify.deadId"
GUILDBATTLEKILLNOTIFY_DEADID_FIELD.number = 2
GUILDBATTLEKILLNOTIFY_DEADID_FIELD.index = 1
GUILDBATTLEKILLNOTIFY_DEADID_FIELD.label = 2
GUILDBATTLEKILLNOTIFY_DEADID_FIELD.has_default_value = false
GUILDBATTLEKILLNOTIFY_DEADID_FIELD.default_value = 0
GUILDBATTLEKILLNOTIFY_DEADID_FIELD.type = 4
GUILDBATTLEKILLNOTIFY_DEADID_FIELD.cpp_type = 4

GUILDBATTLEKILLNOTIFY.name = "GuildBattleKillNotify"
GUILDBATTLEKILLNOTIFY.full_name = ".GuildBattleKillNotify"
GUILDBATTLEKILLNOTIFY.nested_types = {}
GUILDBATTLEKILLNOTIFY.enum_types = {}
GUILDBATTLEKILLNOTIFY.fields = {GUILDBATTLEKILLNOTIFY_KILLERID_FIELD, GUILDBATTLEKILLNOTIFY_DEADID_FIELD}
GUILDBATTLEKILLNOTIFY.is_extendable = false
GUILDBATTLEKILLNOTIFY.extensions = {}
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.name = "playerId"
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.full_name = ".GuildBattleBossDamageNotify.playerId"
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.number = 1
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.index = 0
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.label = 2
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.has_default_value = false
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.default_value = 0
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.type = 4
GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD.cpp_type = 4

GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.name = "damage"
GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.full_name = ".GuildBattleBossDamageNotify.damage"
GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.number = 2
GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.index = 1
GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.label = 2
GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.has_default_value = false
GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.default_value = 0
GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.type = 13
GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD.cpp_type = 3

GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.name = "kill"
GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.full_name = ".GuildBattleBossDamageNotify.kill"
GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.number = 3
GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.index = 2
GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.label = 2
GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.has_default_value = false
GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.default_value = false
GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.type = 8
GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD.cpp_type = 7

GUILDBATTLEBOSSDAMAGENOTIFY.name = "GuildBattleBossDamageNotify"
GUILDBATTLEBOSSDAMAGENOTIFY.full_name = ".GuildBattleBossDamageNotify"
GUILDBATTLEBOSSDAMAGENOTIFY.nested_types = {}
GUILDBATTLEBOSSDAMAGENOTIFY.enum_types = {}
GUILDBATTLEBOSSDAMAGENOTIFY.fields = {GUILDBATTLEBOSSDAMAGENOTIFY_PLAYERID_FIELD, GUILDBATTLEBOSSDAMAGENOTIFY_DAMAGE_FIELD, GUILDBATTLEBOSSDAMAGENOTIFY_KILL_FIELD}
GUILDBATTLEBOSSDAMAGENOTIFY.is_extendable = false
GUILDBATTLEBOSSDAMAGENOTIFY.extensions = {}

GuildBattleActivityFinishNotify = protobuf.Message(GUILDBATTLEACTIVITYFINISHNOTIFY)
GuildBattleBossDamageNotify = protobuf.Message(GUILDBATTLEBOSSDAMAGENOTIFY)
GuildBattleCallNotify = protobuf.Message(GUILDBATTLECALLNOTIFY)
GuildBattleContributionData = protobuf.Message(GUILDBATTLECONTRIBUTIONDATA)
GuildBattleInfoAnswer = protobuf.Message(GUILDBATTLEINFOANSWER)
GuildBattleKillNotify = protobuf.Message(GUILDBATTLEKILLNOTIFY)
GuildBattleMatchData = protobuf.Message(GUILDBATTLEMATCHDATA)
GuildBattleMatchNotify = protobuf.Message(GUILDBATTLEMATCHNOTIFY)
GuildBattleRankData = protobuf.Message(GUILDBATTLERANKDATA)
GuildBattleRecordData = protobuf.Message(GUILDBATTLERECORDDATA)
GuildBattleRecordNotify = protobuf.Message(GUILDBATTLERECORDNOTIFY)
GuildBattleRoundFinishNotify = protobuf.Message(GUILDBATTLEROUNDFINISHNOTIFY)
GuildBattleRoundInfoNotify = protobuf.Message(GUILDBATTLEROUNDINFONOTIFY)
GuildBattleWaitInfoNotify = protobuf.Message(GUILDBATTLEWAITINFONOTIFY)

