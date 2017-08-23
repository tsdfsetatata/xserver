-- Generated By protoc-gen-lua Do not Edit
local protobuf = require "protobuf"
local comm_message_pb = require("comm_message_pb")
module('role_pb')


local PLAYERATTRNOTIFY = protobuf.Descriptor();
local PLAYERATTRNOTIFY_PLAYER_ID_FIELD = protobuf.FieldDescriptor();
local PLAYERATTRNOTIFY_ATTRS_FIELD = protobuf.FieldDescriptor();
local PLAYERNAMENOTIFY = protobuf.Descriptor();
local PLAYERNAMENOTIFY_PLAYER_ID_FIELD = protobuf.FieldDescriptor();
local PLAYERNAMENOTIFY_PLAYER_NAME_FIELD = protobuf.FieldDescriptor();
local PLAYERRENAMEREQUEST = protobuf.Descriptor();
local PLAYERRENAMEREQUEST_NAME_FIELD = protobuf.FieldDescriptor();
local HEADICONREPLACEREQUEST = protobuf.Descriptor();
local HEADICONREPLACEREQUEST_ICON_ID_FIELD = protobuf.FieldDescriptor();
local HEADICONUNLOCKNOTIFY = protobuf.Descriptor();
local HEADICONUNLOCKNOTIFY_ICON_ID_FIELD = protobuf.FieldDescriptor();
local HEADICONINFOANSWER = protobuf.Descriptor();
local HEADICONINFOANSWER_RESULT_FIELD = protobuf.FieldDescriptor();
local HEADICONINFOANSWER_ICON_LIST_FIELD = protobuf.FieldDescriptor();
local HEADICONOLDANSWER = protobuf.Descriptor();
local HEADICONOLDANSWER_RESULT_FIELD = protobuf.FieldDescriptor();
local HEADICONOLDANSWER_ICON_ID_FIELD = protobuf.FieldDescriptor();
local SETFASHION = protobuf.Descriptor();
local SETFASHION_ID_FIELD = protobuf.FieldDescriptor();
local SETFASHION_VAUAL_FIELD = protobuf.FieldDescriptor();
local SYSTEMNOTICENOTIFY = protobuf.Descriptor();
local SYSTEMNOTICENOTIFY_ID_FIELD = protobuf.FieldDescriptor();
local SYSTEMNOTICENOTIFY_ARGS_FIELD = protobuf.FieldDescriptor();
local SYSTEMNOTICENOTIFY_TARGETID_FIELD = protobuf.FieldDescriptor();
local SERVERLEVELINFONOTIFY = protobuf.Descriptor();
local SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD = protobuf.FieldDescriptor();
local SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD = protobuf.FieldDescriptor();
local SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD = protobuf.FieldDescriptor();
local SERVERLEVELINFONOTIFY_BREAKING_FIELD = protobuf.FieldDescriptor();

PLAYERATTRNOTIFY_PLAYER_ID_FIELD.name = "player_id"
PLAYERATTRNOTIFY_PLAYER_ID_FIELD.full_name = ".PlayerAttrNotify.player_id"
PLAYERATTRNOTIFY_PLAYER_ID_FIELD.number = 1
PLAYERATTRNOTIFY_PLAYER_ID_FIELD.index = 0
PLAYERATTRNOTIFY_PLAYER_ID_FIELD.label = 2
PLAYERATTRNOTIFY_PLAYER_ID_FIELD.has_default_value = false
PLAYERATTRNOTIFY_PLAYER_ID_FIELD.default_value = 0
PLAYERATTRNOTIFY_PLAYER_ID_FIELD.type = 4
PLAYERATTRNOTIFY_PLAYER_ID_FIELD.cpp_type = 4

PLAYERATTRNOTIFY_ATTRS_FIELD.name = "attrs"
PLAYERATTRNOTIFY_ATTRS_FIELD.full_name = ".PlayerAttrNotify.attrs"
PLAYERATTRNOTIFY_ATTRS_FIELD.number = 2
PLAYERATTRNOTIFY_ATTRS_FIELD.index = 1
PLAYERATTRNOTIFY_ATTRS_FIELD.label = 3
PLAYERATTRNOTIFY_ATTRS_FIELD.has_default_value = false
PLAYERATTRNOTIFY_ATTRS_FIELD.default_value = {}
PLAYERATTRNOTIFY_ATTRS_FIELD.message_type = COMM_MESSAGE_PB_ATTRDATA
PLAYERATTRNOTIFY_ATTRS_FIELD.type = 11
PLAYERATTRNOTIFY_ATTRS_FIELD.cpp_type = 10

PLAYERATTRNOTIFY.name = "PlayerAttrNotify"
PLAYERATTRNOTIFY.full_name = ".PlayerAttrNotify"
PLAYERATTRNOTIFY.nested_types = {}
PLAYERATTRNOTIFY.enum_types = {}
PLAYERATTRNOTIFY.fields = {PLAYERATTRNOTIFY_PLAYER_ID_FIELD, PLAYERATTRNOTIFY_ATTRS_FIELD}
PLAYERATTRNOTIFY.is_extendable = false
PLAYERATTRNOTIFY.extensions = {}
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.name = "player_id"
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.full_name = ".PlayerNameNotify.player_id"
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.number = 1
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.index = 0
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.label = 2
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.has_default_value = false
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.default_value = 0
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.type = 4
PLAYERNAMENOTIFY_PLAYER_ID_FIELD.cpp_type = 4

PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.name = "player_name"
PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.full_name = ".PlayerNameNotify.player_name"
PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.number = 2
PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.index = 1
PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.label = 1
PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.has_default_value = false
PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.default_value = ""
PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.type = 9
PLAYERNAMENOTIFY_PLAYER_NAME_FIELD.cpp_type = 9

PLAYERNAMENOTIFY.name = "PlayerNameNotify"
PLAYERNAMENOTIFY.full_name = ".PlayerNameNotify"
PLAYERNAMENOTIFY.nested_types = {}
PLAYERNAMENOTIFY.enum_types = {}
PLAYERNAMENOTIFY.fields = {PLAYERNAMENOTIFY_PLAYER_ID_FIELD, PLAYERNAMENOTIFY_PLAYER_NAME_FIELD}
PLAYERNAMENOTIFY.is_extendable = false
PLAYERNAMENOTIFY.extensions = {}
PLAYERRENAMEREQUEST_NAME_FIELD.name = "name"
PLAYERRENAMEREQUEST_NAME_FIELD.full_name = ".PlayerRenameRequest.name"
PLAYERRENAMEREQUEST_NAME_FIELD.number = 1
PLAYERRENAMEREQUEST_NAME_FIELD.index = 0
PLAYERRENAMEREQUEST_NAME_FIELD.label = 2
PLAYERRENAMEREQUEST_NAME_FIELD.has_default_value = false
PLAYERRENAMEREQUEST_NAME_FIELD.default_value = ""
PLAYERRENAMEREQUEST_NAME_FIELD.type = 9
PLAYERRENAMEREQUEST_NAME_FIELD.cpp_type = 9

PLAYERRENAMEREQUEST.name = "PlayerRenameRequest"
PLAYERRENAMEREQUEST.full_name = ".PlayerRenameRequest"
PLAYERRENAMEREQUEST.nested_types = {}
PLAYERRENAMEREQUEST.enum_types = {}
PLAYERRENAMEREQUEST.fields = {PLAYERRENAMEREQUEST_NAME_FIELD}
PLAYERRENAMEREQUEST.is_extendable = false
PLAYERRENAMEREQUEST.extensions = {}
HEADICONREPLACEREQUEST_ICON_ID_FIELD.name = "icon_id"
HEADICONREPLACEREQUEST_ICON_ID_FIELD.full_name = ".HeadIconReplaceRequest.icon_id"
HEADICONREPLACEREQUEST_ICON_ID_FIELD.number = 1
HEADICONREPLACEREQUEST_ICON_ID_FIELD.index = 0
HEADICONREPLACEREQUEST_ICON_ID_FIELD.label = 2
HEADICONREPLACEREQUEST_ICON_ID_FIELD.has_default_value = false
HEADICONREPLACEREQUEST_ICON_ID_FIELD.default_value = 0
HEADICONREPLACEREQUEST_ICON_ID_FIELD.type = 13
HEADICONREPLACEREQUEST_ICON_ID_FIELD.cpp_type = 3

HEADICONREPLACEREQUEST.name = "HeadIconReplaceRequest"
HEADICONREPLACEREQUEST.full_name = ".HeadIconReplaceRequest"
HEADICONREPLACEREQUEST.nested_types = {}
HEADICONREPLACEREQUEST.enum_types = {}
HEADICONREPLACEREQUEST.fields = {HEADICONREPLACEREQUEST_ICON_ID_FIELD}
HEADICONREPLACEREQUEST.is_extendable = false
HEADICONREPLACEREQUEST.extensions = {}
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.name = "icon_id"
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.full_name = ".HeadIconUnlockNotify.icon_id"
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.number = 1
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.index = 0
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.label = 2
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.has_default_value = false
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.default_value = 0
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.type = 13
HEADICONUNLOCKNOTIFY_ICON_ID_FIELD.cpp_type = 3

HEADICONUNLOCKNOTIFY.name = "HeadIconUnlockNotify"
HEADICONUNLOCKNOTIFY.full_name = ".HeadIconUnlockNotify"
HEADICONUNLOCKNOTIFY.nested_types = {}
HEADICONUNLOCKNOTIFY.enum_types = {}
HEADICONUNLOCKNOTIFY.fields = {HEADICONUNLOCKNOTIFY_ICON_ID_FIELD}
HEADICONUNLOCKNOTIFY.is_extendable = false
HEADICONUNLOCKNOTIFY.extensions = {}
HEADICONINFOANSWER_RESULT_FIELD.name = "result"
HEADICONINFOANSWER_RESULT_FIELD.full_name = ".HeadIconInfoAnswer.result"
HEADICONINFOANSWER_RESULT_FIELD.number = 1
HEADICONINFOANSWER_RESULT_FIELD.index = 0
HEADICONINFOANSWER_RESULT_FIELD.label = 2
HEADICONINFOANSWER_RESULT_FIELD.has_default_value = false
HEADICONINFOANSWER_RESULT_FIELD.default_value = 0
HEADICONINFOANSWER_RESULT_FIELD.type = 13
HEADICONINFOANSWER_RESULT_FIELD.cpp_type = 3

HEADICONINFOANSWER_ICON_LIST_FIELD.name = "icon_list"
HEADICONINFOANSWER_ICON_LIST_FIELD.full_name = ".HeadIconInfoAnswer.icon_list"
HEADICONINFOANSWER_ICON_LIST_FIELD.number = 2
HEADICONINFOANSWER_ICON_LIST_FIELD.index = 1
HEADICONINFOANSWER_ICON_LIST_FIELD.label = 3
HEADICONINFOANSWER_ICON_LIST_FIELD.has_default_value = false
HEADICONINFOANSWER_ICON_LIST_FIELD.default_value = {}
HEADICONINFOANSWER_ICON_LIST_FIELD.message_type = COMM_MESSAGE_PB_HEADICONDATA
HEADICONINFOANSWER_ICON_LIST_FIELD.type = 11
HEADICONINFOANSWER_ICON_LIST_FIELD.cpp_type = 10

HEADICONINFOANSWER.name = "HeadIconInfoAnswer"
HEADICONINFOANSWER.full_name = ".HeadIconInfoAnswer"
HEADICONINFOANSWER.nested_types = {}
HEADICONINFOANSWER.enum_types = {}
HEADICONINFOANSWER.fields = {HEADICONINFOANSWER_RESULT_FIELD, HEADICONINFOANSWER_ICON_LIST_FIELD}
HEADICONINFOANSWER.is_extendable = false
HEADICONINFOANSWER.extensions = {}
HEADICONOLDANSWER_RESULT_FIELD.name = "result"
HEADICONOLDANSWER_RESULT_FIELD.full_name = ".HeadIconOldAnswer.result"
HEADICONOLDANSWER_RESULT_FIELD.number = 1
HEADICONOLDANSWER_RESULT_FIELD.index = 0
HEADICONOLDANSWER_RESULT_FIELD.label = 2
HEADICONOLDANSWER_RESULT_FIELD.has_default_value = false
HEADICONOLDANSWER_RESULT_FIELD.default_value = 0
HEADICONOLDANSWER_RESULT_FIELD.type = 13
HEADICONOLDANSWER_RESULT_FIELD.cpp_type = 3

HEADICONOLDANSWER_ICON_ID_FIELD.name = "icon_id"
HEADICONOLDANSWER_ICON_ID_FIELD.full_name = ".HeadIconOldAnswer.icon_id"
HEADICONOLDANSWER_ICON_ID_FIELD.number = 2
HEADICONOLDANSWER_ICON_ID_FIELD.index = 1
HEADICONOLDANSWER_ICON_ID_FIELD.label = 2
HEADICONOLDANSWER_ICON_ID_FIELD.has_default_value = false
HEADICONOLDANSWER_ICON_ID_FIELD.default_value = 0
HEADICONOLDANSWER_ICON_ID_FIELD.type = 13
HEADICONOLDANSWER_ICON_ID_FIELD.cpp_type = 3

HEADICONOLDANSWER.name = "HeadIconOldAnswer"
HEADICONOLDANSWER.full_name = ".HeadIconOldAnswer"
HEADICONOLDANSWER.nested_types = {}
HEADICONOLDANSWER.enum_types = {}
HEADICONOLDANSWER.fields = {HEADICONOLDANSWER_RESULT_FIELD, HEADICONOLDANSWER_ICON_ID_FIELD}
HEADICONOLDANSWER.is_extendable = false
HEADICONOLDANSWER.extensions = {}
SETFASHION_ID_FIELD.name = "id"
SETFASHION_ID_FIELD.full_name = ".SetFashion.id"
SETFASHION_ID_FIELD.number = 1
SETFASHION_ID_FIELD.index = 0
SETFASHION_ID_FIELD.label = 2
SETFASHION_ID_FIELD.has_default_value = false
SETFASHION_ID_FIELD.default_value = 0
SETFASHION_ID_FIELD.type = 13
SETFASHION_ID_FIELD.cpp_type = 3

SETFASHION_VAUAL_FIELD.name = "vaual"
SETFASHION_VAUAL_FIELD.full_name = ".SetFashion.vaual"
SETFASHION_VAUAL_FIELD.number = 2
SETFASHION_VAUAL_FIELD.index = 1
SETFASHION_VAUAL_FIELD.label = 2
SETFASHION_VAUAL_FIELD.has_default_value = false
SETFASHION_VAUAL_FIELD.default_value = 0
SETFASHION_VAUAL_FIELD.type = 13
SETFASHION_VAUAL_FIELD.cpp_type = 3

SETFASHION.name = "SetFashion"
SETFASHION.full_name = ".SetFashion"
SETFASHION.nested_types = {}
SETFASHION.enum_types = {}
SETFASHION.fields = {SETFASHION_ID_FIELD, SETFASHION_VAUAL_FIELD}
SETFASHION.is_extendable = false
SETFASHION.extensions = {}
SYSTEMNOTICENOTIFY_ID_FIELD.name = "id"
SYSTEMNOTICENOTIFY_ID_FIELD.full_name = ".SystemNoticeNotify.id"
SYSTEMNOTICENOTIFY_ID_FIELD.number = 1
SYSTEMNOTICENOTIFY_ID_FIELD.index = 0
SYSTEMNOTICENOTIFY_ID_FIELD.label = 2
SYSTEMNOTICENOTIFY_ID_FIELD.has_default_value = false
SYSTEMNOTICENOTIFY_ID_FIELD.default_value = 0
SYSTEMNOTICENOTIFY_ID_FIELD.type = 13
SYSTEMNOTICENOTIFY_ID_FIELD.cpp_type = 3

SYSTEMNOTICENOTIFY_ARGS_FIELD.name = "args"
SYSTEMNOTICENOTIFY_ARGS_FIELD.full_name = ".SystemNoticeNotify.args"
SYSTEMNOTICENOTIFY_ARGS_FIELD.number = 2
SYSTEMNOTICENOTIFY_ARGS_FIELD.index = 1
SYSTEMNOTICENOTIFY_ARGS_FIELD.label = 3
SYSTEMNOTICENOTIFY_ARGS_FIELD.has_default_value = false
SYSTEMNOTICENOTIFY_ARGS_FIELD.default_value = {}
SYSTEMNOTICENOTIFY_ARGS_FIELD.type = 9
SYSTEMNOTICENOTIFY_ARGS_FIELD.cpp_type = 9

SYSTEMNOTICENOTIFY_TARGETID_FIELD.name = "targetId"
SYSTEMNOTICENOTIFY_TARGETID_FIELD.full_name = ".SystemNoticeNotify.targetId"
SYSTEMNOTICENOTIFY_TARGETID_FIELD.number = 3
SYSTEMNOTICENOTIFY_TARGETID_FIELD.index = 2
SYSTEMNOTICENOTIFY_TARGETID_FIELD.label = 1
SYSTEMNOTICENOTIFY_TARGETID_FIELD.has_default_value = false
SYSTEMNOTICENOTIFY_TARGETID_FIELD.default_value = 0
SYSTEMNOTICENOTIFY_TARGETID_FIELD.type = 4
SYSTEMNOTICENOTIFY_TARGETID_FIELD.cpp_type = 4

SYSTEMNOTICENOTIFY.name = "SystemNoticeNotify"
SYSTEMNOTICENOTIFY.full_name = ".SystemNoticeNotify"
SYSTEMNOTICENOTIFY.nested_types = {}
SYSTEMNOTICENOTIFY.enum_types = {}
SYSTEMNOTICENOTIFY.fields = {SYSTEMNOTICENOTIFY_ID_FIELD, SYSTEMNOTICENOTIFY_ARGS_FIELD, SYSTEMNOTICENOTIFY_TARGETID_FIELD}
SYSTEMNOTICENOTIFY.is_extendable = false
SYSTEMNOTICENOTIFY.extensions = {}
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.name = "level_id"
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.full_name = ".ServerLevelInfoNotify.level_id"
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.number = 1
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.index = 0
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.label = 2
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.has_default_value = false
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.default_value = 0
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.type = 13
SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD.cpp_type = 3

SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.name = "break_goal"
SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.full_name = ".ServerLevelInfoNotify.break_goal"
SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.number = 2
SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.index = 1
SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.label = 2
SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.has_default_value = false
SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.default_value = 0
SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.type = 13
SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD.cpp_type = 3

SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.name = "break_num"
SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.full_name = ".ServerLevelInfoNotify.break_num"
SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.number = 3
SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.index = 2
SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.label = 2
SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.has_default_value = false
SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.default_value = 0
SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.type = 13
SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD.cpp_type = 3

SERVERLEVELINFONOTIFY_BREAKING_FIELD.name = "breaking"
SERVERLEVELINFONOTIFY_BREAKING_FIELD.full_name = ".ServerLevelInfoNotify.breaking"
SERVERLEVELINFONOTIFY_BREAKING_FIELD.number = 4
SERVERLEVELINFONOTIFY_BREAKING_FIELD.index = 3
SERVERLEVELINFONOTIFY_BREAKING_FIELD.label = 2
SERVERLEVELINFONOTIFY_BREAKING_FIELD.has_default_value = false
SERVERLEVELINFONOTIFY_BREAKING_FIELD.default_value = false
SERVERLEVELINFONOTIFY_BREAKING_FIELD.type = 8
SERVERLEVELINFONOTIFY_BREAKING_FIELD.cpp_type = 7

SERVERLEVELINFONOTIFY.name = "ServerLevelInfoNotify"
SERVERLEVELINFONOTIFY.full_name = ".ServerLevelInfoNotify"
SERVERLEVELINFONOTIFY.nested_types = {}
SERVERLEVELINFONOTIFY.enum_types = {}
SERVERLEVELINFONOTIFY.fields = {SERVERLEVELINFONOTIFY_LEVEL_ID_FIELD, SERVERLEVELINFONOTIFY_BREAK_GOAL_FIELD, SERVERLEVELINFONOTIFY_BREAK_NUM_FIELD, SERVERLEVELINFONOTIFY_BREAKING_FIELD}
SERVERLEVELINFONOTIFY.is_extendable = false
SERVERLEVELINFONOTIFY.extensions = {}

HeadIconInfoAnswer = protobuf.Message(HEADICONINFOANSWER)
HeadIconOldAnswer = protobuf.Message(HEADICONOLDANSWER)
HeadIconReplaceRequest = protobuf.Message(HEADICONREPLACEREQUEST)
HeadIconUnlockNotify = protobuf.Message(HEADICONUNLOCKNOTIFY)
PlayerAttrNotify = protobuf.Message(PLAYERATTRNOTIFY)
PlayerNameNotify = protobuf.Message(PLAYERNAMENOTIFY)
PlayerRenameRequest = protobuf.Message(PLAYERRENAMEREQUEST)
ServerLevelInfoNotify = protobuf.Message(SERVERLEVELINFONOTIFY)
SetFashion = protobuf.Message(SETFASHION)
SystemNoticeNotify = protobuf.Message(SYSTEMNOTICENOTIFY)

