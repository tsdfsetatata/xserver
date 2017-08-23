-- Generated By protoc-gen-lua Do not Edit
local protobuf = require "protobuf"
module('horse_pb')


local ONHORSEREQUEST = protobuf.Descriptor();
local ONHORSEREQUEST_POS_Y_FIELD = protobuf.FieldDescriptor();
local HORSEDATA = protobuf.Descriptor();
local HORSEDATA_ID_FIELD = protobuf.FieldDescriptor();
local HORSEDATA_CD_FIELD = protobuf.FieldDescriptor();
local HORSEDATA_ISNEW_FIELD = protobuf.FieldDescriptor();
local HORSEDATA_ISEXPIRE_FIELD = protobuf.FieldDescriptor();
local HORSEDATA_IS_CURRENT_FIELD = protobuf.FieldDescriptor();
local HORSECOMMONATTR = protobuf.Descriptor();
local HORSECOMMONATTR_STEP_FIELD = protobuf.FieldDescriptor();
local HORSECOMMONATTR_ATTR_FIELD = protobuf.FieldDescriptor();
local HORSECOMMONATTR_ATTR_LEVEL_FIELD = protobuf.FieldDescriptor();
local HORSECOMMONATTR_SOUL_LEVEL_FIELD = protobuf.FieldDescriptor();
local HORSECOMMONATTR_CUR_SOUL_FIELD = protobuf.FieldDescriptor();
local HORSECOMMONATTR_POWER_FIELD = protobuf.FieldDescriptor();
local HORSECOMMONATTR_SOUL_NUM_FIELD = protobuf.FieldDescriptor();
local HORSECOMMONATTR_SOUL_FULL_FIELD = protobuf.FieldDescriptor();
local HORSELIST = protobuf.Descriptor();
local HORSELIST_DATA_FIELD = protobuf.FieldDescriptor();
local HORSELIST_ATTR_FIELD = protobuf.FieldDescriptor();
local BUYHORSE = protobuf.Descriptor();
local BUYHORSE_ID_FIELD = protobuf.FieldDescriptor();
local BUYHORSE_SHOPID_FIELD = protobuf.FieldDescriptor();
local BUYHORSE_TYPE_FIELD = protobuf.FieldDescriptor();
local BUYHORSEANS = protobuf.Descriptor();
local BUYHORSEANS_RET_FIELD = protobuf.FieldDescriptor();
local BUYHORSEANS_DATA_FIELD = protobuf.FieldDescriptor();
local HORSEID = protobuf.Descriptor();
local HORSEID_ID_FIELD = protobuf.FieldDescriptor();
local SETCURHORSEANS = protobuf.Descriptor();
local SETCURHORSEANS_RET_FIELD = protobuf.FieldDescriptor();
local SETCURHORSEANS_ID_FIELD = protobuf.FieldDescriptor();
local SETCURHORSEANS_POWER_FIELD = protobuf.FieldDescriptor();
local SETCURHORSEANS_PLAYERID_FIELD = protobuf.FieldDescriptor();
local SETCURHORSEANS_OLD_ID_FIELD = protobuf.FieldDescriptor();
local HORSEATTR = protobuf.Descriptor();
local HORSEATTR_ARRTID_FIELD = protobuf.FieldDescriptor();
local HORSEATTR_TYPE_FIELD = protobuf.FieldDescriptor();
local HORSEATTRANS = protobuf.Descriptor();
local HORSEATTRANS_ARRTID_FIELD = protobuf.FieldDescriptor();
local HORSEATTRANS_NUM_FIELD = protobuf.FieldDescriptor();
local HORSEATTRANS_RET_FIELD = protobuf.FieldDescriptor();
local HORSEATTRANS_POWER_FIELD = protobuf.FieldDescriptor();
local HORSESTEPANS = protobuf.Descriptor();
local HORSESTEPANS_STEP_FIELD = protobuf.FieldDescriptor();
local HORSESTEPANS_RET_FIELD = protobuf.FieldDescriptor();
local HORSESTEPANS_POWER_FIELD = protobuf.FieldDescriptor();
local HORSEADDSOUL = protobuf.Descriptor();
local HORSEADDSOUL_SOUL_FIELD = protobuf.FieldDescriptor();
local HORSESOULANS = protobuf.Descriptor();
local HORSESOULANS_SOUL_FIELD = protobuf.FieldDescriptor();
local HORSESOULANS_POWER_FIELD = protobuf.FieldDescriptor();
local HORSESOULANS_NUM_FIELD = protobuf.FieldDescriptor();
local HORSESOULANS_RET_FIELD = protobuf.FieldDescriptor();
local HORSESOULANS_CUR_SOUL_FIELD = protobuf.FieldDescriptor();
local HORSESOULANS_SOUL_FULL_FIELD = protobuf.FieldDescriptor();
local HORSESOULLEVELANS = protobuf.Descriptor();
local HORSESOULLEVELANS_LEVEL_FIELD = protobuf.FieldDescriptor();
local HORSESOULLEVELANS_POWER_FIELD = protobuf.FieldDescriptor();
local HORSESOULLEVELANS_RET_FIELD = protobuf.FieldDescriptor();
local ONHORSE = protobuf.Descriptor();
local ONHORSE_PLAYERID_FIELD = protobuf.FieldDescriptor();
local ONHORSE_HORSEID_FIELD = protobuf.FieldDescriptor();
local FLYSTATE = protobuf.Descriptor();
local FLYSTATE_FLY_FIELD = protobuf.FieldDescriptor();

ONHORSEREQUEST_POS_Y_FIELD.name = "pos_y"
ONHORSEREQUEST_POS_Y_FIELD.full_name = ".OnHorseRequest.pos_y"
ONHORSEREQUEST_POS_Y_FIELD.number = 1
ONHORSEREQUEST_POS_Y_FIELD.index = 0
ONHORSEREQUEST_POS_Y_FIELD.label = 2
ONHORSEREQUEST_POS_Y_FIELD.has_default_value = false
ONHORSEREQUEST_POS_Y_FIELD.default_value = 0.0
ONHORSEREQUEST_POS_Y_FIELD.type = 2
ONHORSEREQUEST_POS_Y_FIELD.cpp_type = 6

ONHORSEREQUEST.name = "OnHorseRequest"
ONHORSEREQUEST.full_name = ".OnHorseRequest"
ONHORSEREQUEST.nested_types = {}
ONHORSEREQUEST.enum_types = {}
ONHORSEREQUEST.fields = {ONHORSEREQUEST_POS_Y_FIELD}
ONHORSEREQUEST.is_extendable = false
ONHORSEREQUEST.extensions = {}
HORSEDATA_ID_FIELD.name = "id"
HORSEDATA_ID_FIELD.full_name = ".HorseData.id"
HORSEDATA_ID_FIELD.number = 1
HORSEDATA_ID_FIELD.index = 0
HORSEDATA_ID_FIELD.label = 2
HORSEDATA_ID_FIELD.has_default_value = false
HORSEDATA_ID_FIELD.default_value = 0
HORSEDATA_ID_FIELD.type = 13
HORSEDATA_ID_FIELD.cpp_type = 3

HORSEDATA_CD_FIELD.name = "cd"
HORSEDATA_CD_FIELD.full_name = ".HorseData.cd"
HORSEDATA_CD_FIELD.number = 2
HORSEDATA_CD_FIELD.index = 1
HORSEDATA_CD_FIELD.label = 2
HORSEDATA_CD_FIELD.has_default_value = false
HORSEDATA_CD_FIELD.default_value = 0
HORSEDATA_CD_FIELD.type = 13
HORSEDATA_CD_FIELD.cpp_type = 3

HORSEDATA_ISNEW_FIELD.name = "isnew"
HORSEDATA_ISNEW_FIELD.full_name = ".HorseData.isnew"
HORSEDATA_ISNEW_FIELD.number = 3
HORSEDATA_ISNEW_FIELD.index = 2
HORSEDATA_ISNEW_FIELD.label = 2
HORSEDATA_ISNEW_FIELD.has_default_value = false
HORSEDATA_ISNEW_FIELD.default_value = false
HORSEDATA_ISNEW_FIELD.type = 8
HORSEDATA_ISNEW_FIELD.cpp_type = 7

HORSEDATA_ISEXPIRE_FIELD.name = "isexpire"
HORSEDATA_ISEXPIRE_FIELD.full_name = ".HorseData.isexpire"
HORSEDATA_ISEXPIRE_FIELD.number = 4
HORSEDATA_ISEXPIRE_FIELD.index = 3
HORSEDATA_ISEXPIRE_FIELD.label = 2
HORSEDATA_ISEXPIRE_FIELD.has_default_value = false
HORSEDATA_ISEXPIRE_FIELD.default_value = false
HORSEDATA_ISEXPIRE_FIELD.type = 8
HORSEDATA_ISEXPIRE_FIELD.cpp_type = 7

HORSEDATA_IS_CURRENT_FIELD.name = "is_current"
HORSEDATA_IS_CURRENT_FIELD.full_name = ".HorseData.is_current"
HORSEDATA_IS_CURRENT_FIELD.number = 5
HORSEDATA_IS_CURRENT_FIELD.index = 4
HORSEDATA_IS_CURRENT_FIELD.label = 2
HORSEDATA_IS_CURRENT_FIELD.has_default_value = false
HORSEDATA_IS_CURRENT_FIELD.default_value = false
HORSEDATA_IS_CURRENT_FIELD.type = 8
HORSEDATA_IS_CURRENT_FIELD.cpp_type = 7

HORSEDATA.name = "HorseData"
HORSEDATA.full_name = ".HorseData"
HORSEDATA.nested_types = {}
HORSEDATA.enum_types = {}
HORSEDATA.fields = {HORSEDATA_ID_FIELD, HORSEDATA_CD_FIELD, HORSEDATA_ISNEW_FIELD, HORSEDATA_ISEXPIRE_FIELD, HORSEDATA_IS_CURRENT_FIELD}
HORSEDATA.is_extendable = false
HORSEDATA.extensions = {}
HORSECOMMONATTR_STEP_FIELD.name = "step"
HORSECOMMONATTR_STEP_FIELD.full_name = ".HorseCommonAttr.step"
HORSECOMMONATTR_STEP_FIELD.number = 1
HORSECOMMONATTR_STEP_FIELD.index = 0
HORSECOMMONATTR_STEP_FIELD.label = 2
HORSECOMMONATTR_STEP_FIELD.has_default_value = false
HORSECOMMONATTR_STEP_FIELD.default_value = 0
HORSECOMMONATTR_STEP_FIELD.type = 13
HORSECOMMONATTR_STEP_FIELD.cpp_type = 3

HORSECOMMONATTR_ATTR_FIELD.name = "attr"
HORSECOMMONATTR_ATTR_FIELD.full_name = ".HorseCommonAttr.attr"
HORSECOMMONATTR_ATTR_FIELD.number = 2
HORSECOMMONATTR_ATTR_FIELD.index = 1
HORSECOMMONATTR_ATTR_FIELD.label = 3
HORSECOMMONATTR_ATTR_FIELD.has_default_value = false
HORSECOMMONATTR_ATTR_FIELD.default_value = {}
HORSECOMMONATTR_ATTR_FIELD.type = 13
HORSECOMMONATTR_ATTR_FIELD.cpp_type = 3

HORSECOMMONATTR_ATTR_LEVEL_FIELD.name = "attr_level"
HORSECOMMONATTR_ATTR_LEVEL_FIELD.full_name = ".HorseCommonAttr.attr_level"
HORSECOMMONATTR_ATTR_LEVEL_FIELD.number = 3
HORSECOMMONATTR_ATTR_LEVEL_FIELD.index = 2
HORSECOMMONATTR_ATTR_LEVEL_FIELD.label = 3
HORSECOMMONATTR_ATTR_LEVEL_FIELD.has_default_value = false
HORSECOMMONATTR_ATTR_LEVEL_FIELD.default_value = {}
HORSECOMMONATTR_ATTR_LEVEL_FIELD.type = 13
HORSECOMMONATTR_ATTR_LEVEL_FIELD.cpp_type = 3

HORSECOMMONATTR_SOUL_LEVEL_FIELD.name = "soul_level"
HORSECOMMONATTR_SOUL_LEVEL_FIELD.full_name = ".HorseCommonAttr.soul_level"
HORSECOMMONATTR_SOUL_LEVEL_FIELD.number = 4
HORSECOMMONATTR_SOUL_LEVEL_FIELD.index = 3
HORSECOMMONATTR_SOUL_LEVEL_FIELD.label = 2
HORSECOMMONATTR_SOUL_LEVEL_FIELD.has_default_value = false
HORSECOMMONATTR_SOUL_LEVEL_FIELD.default_value = 0
HORSECOMMONATTR_SOUL_LEVEL_FIELD.type = 13
HORSECOMMONATTR_SOUL_LEVEL_FIELD.cpp_type = 3

HORSECOMMONATTR_CUR_SOUL_FIELD.name = "cur_soul"
HORSECOMMONATTR_CUR_SOUL_FIELD.full_name = ".HorseCommonAttr.cur_soul"
HORSECOMMONATTR_CUR_SOUL_FIELD.number = 5
HORSECOMMONATTR_CUR_SOUL_FIELD.index = 4
HORSECOMMONATTR_CUR_SOUL_FIELD.label = 2
HORSECOMMONATTR_CUR_SOUL_FIELD.has_default_value = false
HORSECOMMONATTR_CUR_SOUL_FIELD.default_value = 0
HORSECOMMONATTR_CUR_SOUL_FIELD.type = 13
HORSECOMMONATTR_CUR_SOUL_FIELD.cpp_type = 3

HORSECOMMONATTR_POWER_FIELD.name = "power"
HORSECOMMONATTR_POWER_FIELD.full_name = ".HorseCommonAttr.power"
HORSECOMMONATTR_POWER_FIELD.number = 6
HORSECOMMONATTR_POWER_FIELD.index = 5
HORSECOMMONATTR_POWER_FIELD.label = 2
HORSECOMMONATTR_POWER_FIELD.has_default_value = false
HORSECOMMONATTR_POWER_FIELD.default_value = 0
HORSECOMMONATTR_POWER_FIELD.type = 13
HORSECOMMONATTR_POWER_FIELD.cpp_type = 3

HORSECOMMONATTR_SOUL_NUM_FIELD.name = "soul_num"
HORSECOMMONATTR_SOUL_NUM_FIELD.full_name = ".HorseCommonAttr.soul_num"
HORSECOMMONATTR_SOUL_NUM_FIELD.number = 7
HORSECOMMONATTR_SOUL_NUM_FIELD.index = 6
HORSECOMMONATTR_SOUL_NUM_FIELD.label = 3
HORSECOMMONATTR_SOUL_NUM_FIELD.has_default_value = false
HORSECOMMONATTR_SOUL_NUM_FIELD.default_value = {}
HORSECOMMONATTR_SOUL_NUM_FIELD.type = 13
HORSECOMMONATTR_SOUL_NUM_FIELD.cpp_type = 3

HORSECOMMONATTR_SOUL_FULL_FIELD.name = "soul_full"
HORSECOMMONATTR_SOUL_FULL_FIELD.full_name = ".HorseCommonAttr.soul_full"
HORSECOMMONATTR_SOUL_FULL_FIELD.number = 8
HORSECOMMONATTR_SOUL_FULL_FIELD.index = 7
HORSECOMMONATTR_SOUL_FULL_FIELD.label = 2
HORSECOMMONATTR_SOUL_FULL_FIELD.has_default_value = false
HORSECOMMONATTR_SOUL_FULL_FIELD.default_value = false
HORSECOMMONATTR_SOUL_FULL_FIELD.type = 8
HORSECOMMONATTR_SOUL_FULL_FIELD.cpp_type = 7

HORSECOMMONATTR.name = "HorseCommonAttr"
HORSECOMMONATTR.full_name = ".HorseCommonAttr"
HORSECOMMONATTR.nested_types = {}
HORSECOMMONATTR.enum_types = {}
HORSECOMMONATTR.fields = {HORSECOMMONATTR_STEP_FIELD, HORSECOMMONATTR_ATTR_FIELD, HORSECOMMONATTR_ATTR_LEVEL_FIELD, HORSECOMMONATTR_SOUL_LEVEL_FIELD, HORSECOMMONATTR_CUR_SOUL_FIELD, HORSECOMMONATTR_POWER_FIELD, HORSECOMMONATTR_SOUL_NUM_FIELD, HORSECOMMONATTR_SOUL_FULL_FIELD}
HORSECOMMONATTR.is_extendable = false
HORSECOMMONATTR.extensions = {}
HORSELIST_DATA_FIELD.name = "data"
HORSELIST_DATA_FIELD.full_name = ".HorseList.data"
HORSELIST_DATA_FIELD.number = 1
HORSELIST_DATA_FIELD.index = 0
HORSELIST_DATA_FIELD.label = 3
HORSELIST_DATA_FIELD.has_default_value = false
HORSELIST_DATA_FIELD.default_value = {}
HORSELIST_DATA_FIELD.message_type = HORSEDATA
HORSELIST_DATA_FIELD.type = 11
HORSELIST_DATA_FIELD.cpp_type = 10

HORSELIST_ATTR_FIELD.name = "attr"
HORSELIST_ATTR_FIELD.full_name = ".HorseList.attr"
HORSELIST_ATTR_FIELD.number = 2
HORSELIST_ATTR_FIELD.index = 1
HORSELIST_ATTR_FIELD.label = 2
HORSELIST_ATTR_FIELD.has_default_value = false
HORSELIST_ATTR_FIELD.default_value = nil
HORSELIST_ATTR_FIELD.message_type = HORSECOMMONATTR
HORSELIST_ATTR_FIELD.type = 11
HORSELIST_ATTR_FIELD.cpp_type = 10

HORSELIST.name = "HorseList"
HORSELIST.full_name = ".HorseList"
HORSELIST.nested_types = {}
HORSELIST.enum_types = {}
HORSELIST.fields = {HORSELIST_DATA_FIELD, HORSELIST_ATTR_FIELD}
HORSELIST.is_extendable = false
HORSELIST.extensions = {}
BUYHORSE_ID_FIELD.name = "id"
BUYHORSE_ID_FIELD.full_name = ".BuyHorse.id"
BUYHORSE_ID_FIELD.number = 1
BUYHORSE_ID_FIELD.index = 0
BUYHORSE_ID_FIELD.label = 2
BUYHORSE_ID_FIELD.has_default_value = false
BUYHORSE_ID_FIELD.default_value = 0
BUYHORSE_ID_FIELD.type = 13
BUYHORSE_ID_FIELD.cpp_type = 3

BUYHORSE_SHOPID_FIELD.name = "shopid"
BUYHORSE_SHOPID_FIELD.full_name = ".BuyHorse.shopid"
BUYHORSE_SHOPID_FIELD.number = 2
BUYHORSE_SHOPID_FIELD.index = 1
BUYHORSE_SHOPID_FIELD.label = 2
BUYHORSE_SHOPID_FIELD.has_default_value = false
BUYHORSE_SHOPID_FIELD.default_value = 0
BUYHORSE_SHOPID_FIELD.type = 13
BUYHORSE_SHOPID_FIELD.cpp_type = 3

BUYHORSE_TYPE_FIELD.name = "type"
BUYHORSE_TYPE_FIELD.full_name = ".BuyHorse.type"
BUYHORSE_TYPE_FIELD.number = 3
BUYHORSE_TYPE_FIELD.index = 2
BUYHORSE_TYPE_FIELD.label = 2
BUYHORSE_TYPE_FIELD.has_default_value = false
BUYHORSE_TYPE_FIELD.default_value = 0
BUYHORSE_TYPE_FIELD.type = 13
BUYHORSE_TYPE_FIELD.cpp_type = 3

BUYHORSE.name = "BuyHorse"
BUYHORSE.full_name = ".BuyHorse"
BUYHORSE.nested_types = {}
BUYHORSE.enum_types = {}
BUYHORSE.fields = {BUYHORSE_ID_FIELD, BUYHORSE_SHOPID_FIELD, BUYHORSE_TYPE_FIELD}
BUYHORSE.is_extendable = false
BUYHORSE.extensions = {}
BUYHORSEANS_RET_FIELD.name = "ret"
BUYHORSEANS_RET_FIELD.full_name = ".BuyHorseAns.ret"
BUYHORSEANS_RET_FIELD.number = 1
BUYHORSEANS_RET_FIELD.index = 0
BUYHORSEANS_RET_FIELD.label = 2
BUYHORSEANS_RET_FIELD.has_default_value = false
BUYHORSEANS_RET_FIELD.default_value = 0
BUYHORSEANS_RET_FIELD.type = 5
BUYHORSEANS_RET_FIELD.cpp_type = 1

BUYHORSEANS_DATA_FIELD.name = "data"
BUYHORSEANS_DATA_FIELD.full_name = ".BuyHorseAns.data"
BUYHORSEANS_DATA_FIELD.number = 2
BUYHORSEANS_DATA_FIELD.index = 1
BUYHORSEANS_DATA_FIELD.label = 2
BUYHORSEANS_DATA_FIELD.has_default_value = false
BUYHORSEANS_DATA_FIELD.default_value = nil
BUYHORSEANS_DATA_FIELD.message_type = HORSEDATA
BUYHORSEANS_DATA_FIELD.type = 11
BUYHORSEANS_DATA_FIELD.cpp_type = 10

BUYHORSEANS.name = "BuyHorseAns"
BUYHORSEANS.full_name = ".BuyHorseAns"
BUYHORSEANS.nested_types = {}
BUYHORSEANS.enum_types = {}
BUYHORSEANS.fields = {BUYHORSEANS_RET_FIELD, BUYHORSEANS_DATA_FIELD}
BUYHORSEANS.is_extendable = false
BUYHORSEANS.extensions = {}
HORSEID_ID_FIELD.name = "id"
HORSEID_ID_FIELD.full_name = ".HorseId.id"
HORSEID_ID_FIELD.number = 1
HORSEID_ID_FIELD.index = 0
HORSEID_ID_FIELD.label = 2
HORSEID_ID_FIELD.has_default_value = false
HORSEID_ID_FIELD.default_value = 0
HORSEID_ID_FIELD.type = 13
HORSEID_ID_FIELD.cpp_type = 3

HORSEID.name = "HorseId"
HORSEID.full_name = ".HorseId"
HORSEID.nested_types = {}
HORSEID.enum_types = {}
HORSEID.fields = {HORSEID_ID_FIELD}
HORSEID.is_extendable = false
HORSEID.extensions = {}
SETCURHORSEANS_RET_FIELD.name = "ret"
SETCURHORSEANS_RET_FIELD.full_name = ".SetCurHorseAns.ret"
SETCURHORSEANS_RET_FIELD.number = 1
SETCURHORSEANS_RET_FIELD.index = 0
SETCURHORSEANS_RET_FIELD.label = 2
SETCURHORSEANS_RET_FIELD.has_default_value = false
SETCURHORSEANS_RET_FIELD.default_value = 0
SETCURHORSEANS_RET_FIELD.type = 5
SETCURHORSEANS_RET_FIELD.cpp_type = 1

SETCURHORSEANS_ID_FIELD.name = "id"
SETCURHORSEANS_ID_FIELD.full_name = ".SetCurHorseAns.id"
SETCURHORSEANS_ID_FIELD.number = 2
SETCURHORSEANS_ID_FIELD.index = 1
SETCURHORSEANS_ID_FIELD.label = 2
SETCURHORSEANS_ID_FIELD.has_default_value = false
SETCURHORSEANS_ID_FIELD.default_value = 0
SETCURHORSEANS_ID_FIELD.type = 13
SETCURHORSEANS_ID_FIELD.cpp_type = 3

SETCURHORSEANS_POWER_FIELD.name = "power"
SETCURHORSEANS_POWER_FIELD.full_name = ".SetCurHorseAns.power"
SETCURHORSEANS_POWER_FIELD.number = 3
SETCURHORSEANS_POWER_FIELD.index = 2
SETCURHORSEANS_POWER_FIELD.label = 2
SETCURHORSEANS_POWER_FIELD.has_default_value = false
SETCURHORSEANS_POWER_FIELD.default_value = 0
SETCURHORSEANS_POWER_FIELD.type = 13
SETCURHORSEANS_POWER_FIELD.cpp_type = 3

SETCURHORSEANS_PLAYERID_FIELD.name = "playerid"
SETCURHORSEANS_PLAYERID_FIELD.full_name = ".SetCurHorseAns.playerid"
SETCURHORSEANS_PLAYERID_FIELD.number = 4
SETCURHORSEANS_PLAYERID_FIELD.index = 3
SETCURHORSEANS_PLAYERID_FIELD.label = 2
SETCURHORSEANS_PLAYERID_FIELD.has_default_value = false
SETCURHORSEANS_PLAYERID_FIELD.default_value = 0
SETCURHORSEANS_PLAYERID_FIELD.type = 4
SETCURHORSEANS_PLAYERID_FIELD.cpp_type = 4

SETCURHORSEANS_OLD_ID_FIELD.name = "old_id"
SETCURHORSEANS_OLD_ID_FIELD.full_name = ".SetCurHorseAns.old_id"
SETCURHORSEANS_OLD_ID_FIELD.number = 5
SETCURHORSEANS_OLD_ID_FIELD.index = 4
SETCURHORSEANS_OLD_ID_FIELD.label = 2
SETCURHORSEANS_OLD_ID_FIELD.has_default_value = false
SETCURHORSEANS_OLD_ID_FIELD.default_value = 0
SETCURHORSEANS_OLD_ID_FIELD.type = 13
SETCURHORSEANS_OLD_ID_FIELD.cpp_type = 3

SETCURHORSEANS.name = "SetCurHorseAns"
SETCURHORSEANS.full_name = ".SetCurHorseAns"
SETCURHORSEANS.nested_types = {}
SETCURHORSEANS.enum_types = {}
SETCURHORSEANS.fields = {SETCURHORSEANS_RET_FIELD, SETCURHORSEANS_ID_FIELD, SETCURHORSEANS_POWER_FIELD, SETCURHORSEANS_PLAYERID_FIELD, SETCURHORSEANS_OLD_ID_FIELD}
SETCURHORSEANS.is_extendable = false
SETCURHORSEANS.extensions = {}
HORSEATTR_ARRTID_FIELD.name = "arrtid"
HORSEATTR_ARRTID_FIELD.full_name = ".HorseAttr.arrtid"
HORSEATTR_ARRTID_FIELD.number = 1
HORSEATTR_ARRTID_FIELD.index = 0
HORSEATTR_ARRTID_FIELD.label = 2
HORSEATTR_ARRTID_FIELD.has_default_value = false
HORSEATTR_ARRTID_FIELD.default_value = 0
HORSEATTR_ARRTID_FIELD.type = 13
HORSEATTR_ARRTID_FIELD.cpp_type = 3

HORSEATTR_TYPE_FIELD.name = "type"
HORSEATTR_TYPE_FIELD.full_name = ".HorseAttr.type"
HORSEATTR_TYPE_FIELD.number = 2
HORSEATTR_TYPE_FIELD.index = 1
HORSEATTR_TYPE_FIELD.label = 2
HORSEATTR_TYPE_FIELD.has_default_value = false
HORSEATTR_TYPE_FIELD.default_value = 0
HORSEATTR_TYPE_FIELD.type = 13
HORSEATTR_TYPE_FIELD.cpp_type = 3

HORSEATTR.name = "HorseAttr"
HORSEATTR.full_name = ".HorseAttr"
HORSEATTR.nested_types = {}
HORSEATTR.enum_types = {}
HORSEATTR.fields = {HORSEATTR_ARRTID_FIELD, HORSEATTR_TYPE_FIELD}
HORSEATTR.is_extendable = false
HORSEATTR.extensions = {}
HORSEATTRANS_ARRTID_FIELD.name = "arrtid"
HORSEATTRANS_ARRTID_FIELD.full_name = ".HorseAttrAns.arrtid"
HORSEATTRANS_ARRTID_FIELD.number = 1
HORSEATTRANS_ARRTID_FIELD.index = 0
HORSEATTRANS_ARRTID_FIELD.label = 2
HORSEATTRANS_ARRTID_FIELD.has_default_value = false
HORSEATTRANS_ARRTID_FIELD.default_value = 0
HORSEATTRANS_ARRTID_FIELD.type = 13
HORSEATTRANS_ARRTID_FIELD.cpp_type = 3

HORSEATTRANS_NUM_FIELD.name = "num"
HORSEATTRANS_NUM_FIELD.full_name = ".HorseAttrAns.num"
HORSEATTRANS_NUM_FIELD.number = 2
HORSEATTRANS_NUM_FIELD.index = 1
HORSEATTRANS_NUM_FIELD.label = 2
HORSEATTRANS_NUM_FIELD.has_default_value = false
HORSEATTRANS_NUM_FIELD.default_value = 0
HORSEATTRANS_NUM_FIELD.type = 13
HORSEATTRANS_NUM_FIELD.cpp_type = 3

HORSEATTRANS_RET_FIELD.name = "ret"
HORSEATTRANS_RET_FIELD.full_name = ".HorseAttrAns.ret"
HORSEATTRANS_RET_FIELD.number = 3
HORSEATTRANS_RET_FIELD.index = 2
HORSEATTRANS_RET_FIELD.label = 2
HORSEATTRANS_RET_FIELD.has_default_value = false
HORSEATTRANS_RET_FIELD.default_value = 0
HORSEATTRANS_RET_FIELD.type = 5
HORSEATTRANS_RET_FIELD.cpp_type = 1

HORSEATTRANS_POWER_FIELD.name = "power"
HORSEATTRANS_POWER_FIELD.full_name = ".HorseAttrAns.power"
HORSEATTRANS_POWER_FIELD.number = 4
HORSEATTRANS_POWER_FIELD.index = 3
HORSEATTRANS_POWER_FIELD.label = 2
HORSEATTRANS_POWER_FIELD.has_default_value = false
HORSEATTRANS_POWER_FIELD.default_value = 0
HORSEATTRANS_POWER_FIELD.type = 13
HORSEATTRANS_POWER_FIELD.cpp_type = 3

HORSEATTRANS.name = "HorseAttrAns"
HORSEATTRANS.full_name = ".HorseAttrAns"
HORSEATTRANS.nested_types = {}
HORSEATTRANS.enum_types = {}
HORSEATTRANS.fields = {HORSEATTRANS_ARRTID_FIELD, HORSEATTRANS_NUM_FIELD, HORSEATTRANS_RET_FIELD, HORSEATTRANS_POWER_FIELD}
HORSEATTRANS.is_extendable = false
HORSEATTRANS.extensions = {}
HORSESTEPANS_STEP_FIELD.name = "step"
HORSESTEPANS_STEP_FIELD.full_name = ".HorseStepAns.step"
HORSESTEPANS_STEP_FIELD.number = 1
HORSESTEPANS_STEP_FIELD.index = 0
HORSESTEPANS_STEP_FIELD.label = 2
HORSESTEPANS_STEP_FIELD.has_default_value = false
HORSESTEPANS_STEP_FIELD.default_value = 0
HORSESTEPANS_STEP_FIELD.type = 13
HORSESTEPANS_STEP_FIELD.cpp_type = 3

HORSESTEPANS_RET_FIELD.name = "ret"
HORSESTEPANS_RET_FIELD.full_name = ".HorseStepAns.ret"
HORSESTEPANS_RET_FIELD.number = 2
HORSESTEPANS_RET_FIELD.index = 1
HORSESTEPANS_RET_FIELD.label = 2
HORSESTEPANS_RET_FIELD.has_default_value = false
HORSESTEPANS_RET_FIELD.default_value = 0
HORSESTEPANS_RET_FIELD.type = 5
HORSESTEPANS_RET_FIELD.cpp_type = 1

HORSESTEPANS_POWER_FIELD.name = "power"
HORSESTEPANS_POWER_FIELD.full_name = ".HorseStepAns.power"
HORSESTEPANS_POWER_FIELD.number = 3
HORSESTEPANS_POWER_FIELD.index = 2
HORSESTEPANS_POWER_FIELD.label = 2
HORSESTEPANS_POWER_FIELD.has_default_value = false
HORSESTEPANS_POWER_FIELD.default_value = 0
HORSESTEPANS_POWER_FIELD.type = 13
HORSESTEPANS_POWER_FIELD.cpp_type = 3

HORSESTEPANS.name = "HorseStepAns"
HORSESTEPANS.full_name = ".HorseStepAns"
HORSESTEPANS.nested_types = {}
HORSESTEPANS.enum_types = {}
HORSESTEPANS.fields = {HORSESTEPANS_STEP_FIELD, HORSESTEPANS_RET_FIELD, HORSESTEPANS_POWER_FIELD}
HORSESTEPANS.is_extendable = false
HORSESTEPANS.extensions = {}
HORSEADDSOUL_SOUL_FIELD.name = "soul"
HORSEADDSOUL_SOUL_FIELD.full_name = ".HorseAddSoul.soul"
HORSEADDSOUL_SOUL_FIELD.number = 1
HORSEADDSOUL_SOUL_FIELD.index = 0
HORSEADDSOUL_SOUL_FIELD.label = 2
HORSEADDSOUL_SOUL_FIELD.has_default_value = false
HORSEADDSOUL_SOUL_FIELD.default_value = 0
HORSEADDSOUL_SOUL_FIELD.type = 13
HORSEADDSOUL_SOUL_FIELD.cpp_type = 3

HORSEADDSOUL.name = "HorseAddSoul"
HORSEADDSOUL.full_name = ".HorseAddSoul"
HORSEADDSOUL.nested_types = {}
HORSEADDSOUL.enum_types = {}
HORSEADDSOUL.fields = {HORSEADDSOUL_SOUL_FIELD}
HORSEADDSOUL.is_extendable = false
HORSEADDSOUL.extensions = {}
HORSESOULANS_SOUL_FIELD.name = "soul"
HORSESOULANS_SOUL_FIELD.full_name = ".HorseSoulAns.soul"
HORSESOULANS_SOUL_FIELD.number = 1
HORSESOULANS_SOUL_FIELD.index = 0
HORSESOULANS_SOUL_FIELD.label = 2
HORSESOULANS_SOUL_FIELD.has_default_value = false
HORSESOULANS_SOUL_FIELD.default_value = 0
HORSESOULANS_SOUL_FIELD.type = 13
HORSESOULANS_SOUL_FIELD.cpp_type = 3

HORSESOULANS_POWER_FIELD.name = "power"
HORSESOULANS_POWER_FIELD.full_name = ".HorseSoulAns.power"
HORSESOULANS_POWER_FIELD.number = 2
HORSESOULANS_POWER_FIELD.index = 1
HORSESOULANS_POWER_FIELD.label = 2
HORSESOULANS_POWER_FIELD.has_default_value = false
HORSESOULANS_POWER_FIELD.default_value = 0
HORSESOULANS_POWER_FIELD.type = 13
HORSESOULANS_POWER_FIELD.cpp_type = 3

HORSESOULANS_NUM_FIELD.name = "num"
HORSESOULANS_NUM_FIELD.full_name = ".HorseSoulAns.num"
HORSESOULANS_NUM_FIELD.number = 3
HORSESOULANS_NUM_FIELD.index = 2
HORSESOULANS_NUM_FIELD.label = 2
HORSESOULANS_NUM_FIELD.has_default_value = false
HORSESOULANS_NUM_FIELD.default_value = 0
HORSESOULANS_NUM_FIELD.type = 13
HORSESOULANS_NUM_FIELD.cpp_type = 3

HORSESOULANS_RET_FIELD.name = "ret"
HORSESOULANS_RET_FIELD.full_name = ".HorseSoulAns.ret"
HORSESOULANS_RET_FIELD.number = 4
HORSESOULANS_RET_FIELD.index = 3
HORSESOULANS_RET_FIELD.label = 2
HORSESOULANS_RET_FIELD.has_default_value = false
HORSESOULANS_RET_FIELD.default_value = 0
HORSESOULANS_RET_FIELD.type = 5
HORSESOULANS_RET_FIELD.cpp_type = 1

HORSESOULANS_CUR_SOUL_FIELD.name = "cur_soul"
HORSESOULANS_CUR_SOUL_FIELD.full_name = ".HorseSoulAns.cur_soul"
HORSESOULANS_CUR_SOUL_FIELD.number = 5
HORSESOULANS_CUR_SOUL_FIELD.index = 4
HORSESOULANS_CUR_SOUL_FIELD.label = 2
HORSESOULANS_CUR_SOUL_FIELD.has_default_value = false
HORSESOULANS_CUR_SOUL_FIELD.default_value = 0
HORSESOULANS_CUR_SOUL_FIELD.type = 13
HORSESOULANS_CUR_SOUL_FIELD.cpp_type = 3

HORSESOULANS_SOUL_FULL_FIELD.name = "soul_full"
HORSESOULANS_SOUL_FULL_FIELD.full_name = ".HorseSoulAns.soul_full"
HORSESOULANS_SOUL_FULL_FIELD.number = 6
HORSESOULANS_SOUL_FULL_FIELD.index = 5
HORSESOULANS_SOUL_FULL_FIELD.label = 2
HORSESOULANS_SOUL_FULL_FIELD.has_default_value = false
HORSESOULANS_SOUL_FULL_FIELD.default_value = false
HORSESOULANS_SOUL_FULL_FIELD.type = 8
HORSESOULANS_SOUL_FULL_FIELD.cpp_type = 7

HORSESOULANS.name = "HorseSoulAns"
HORSESOULANS.full_name = ".HorseSoulAns"
HORSESOULANS.nested_types = {}
HORSESOULANS.enum_types = {}
HORSESOULANS.fields = {HORSESOULANS_SOUL_FIELD, HORSESOULANS_POWER_FIELD, HORSESOULANS_NUM_FIELD, HORSESOULANS_RET_FIELD, HORSESOULANS_CUR_SOUL_FIELD, HORSESOULANS_SOUL_FULL_FIELD}
HORSESOULANS.is_extendable = false
HORSESOULANS.extensions = {}
HORSESOULLEVELANS_LEVEL_FIELD.name = "level"
HORSESOULLEVELANS_LEVEL_FIELD.full_name = ".HorseSoulLevelAns.level"
HORSESOULLEVELANS_LEVEL_FIELD.number = 1
HORSESOULLEVELANS_LEVEL_FIELD.index = 0
HORSESOULLEVELANS_LEVEL_FIELD.label = 2
HORSESOULLEVELANS_LEVEL_FIELD.has_default_value = false
HORSESOULLEVELANS_LEVEL_FIELD.default_value = 0
HORSESOULLEVELANS_LEVEL_FIELD.type = 13
HORSESOULLEVELANS_LEVEL_FIELD.cpp_type = 3

HORSESOULLEVELANS_POWER_FIELD.name = "power"
HORSESOULLEVELANS_POWER_FIELD.full_name = ".HorseSoulLevelAns.power"
HORSESOULLEVELANS_POWER_FIELD.number = 2
HORSESOULLEVELANS_POWER_FIELD.index = 1
HORSESOULLEVELANS_POWER_FIELD.label = 2
HORSESOULLEVELANS_POWER_FIELD.has_default_value = false
HORSESOULLEVELANS_POWER_FIELD.default_value = 0
HORSESOULLEVELANS_POWER_FIELD.type = 13
HORSESOULLEVELANS_POWER_FIELD.cpp_type = 3

HORSESOULLEVELANS_RET_FIELD.name = "ret"
HORSESOULLEVELANS_RET_FIELD.full_name = ".HorseSoulLevelAns.ret"
HORSESOULLEVELANS_RET_FIELD.number = 3
HORSESOULLEVELANS_RET_FIELD.index = 2
HORSESOULLEVELANS_RET_FIELD.label = 2
HORSESOULLEVELANS_RET_FIELD.has_default_value = false
HORSESOULLEVELANS_RET_FIELD.default_value = 0
HORSESOULLEVELANS_RET_FIELD.type = 5
HORSESOULLEVELANS_RET_FIELD.cpp_type = 1

HORSESOULLEVELANS.name = "HorseSoulLevelAns"
HORSESOULLEVELANS.full_name = ".HorseSoulLevelAns"
HORSESOULLEVELANS.nested_types = {}
HORSESOULLEVELANS.enum_types = {}
HORSESOULLEVELANS.fields = {HORSESOULLEVELANS_LEVEL_FIELD, HORSESOULLEVELANS_POWER_FIELD, HORSESOULLEVELANS_RET_FIELD}
HORSESOULLEVELANS.is_extendable = false
HORSESOULLEVELANS.extensions = {}
ONHORSE_PLAYERID_FIELD.name = "playerid"
ONHORSE_PLAYERID_FIELD.full_name = ".OnHorse.playerid"
ONHORSE_PLAYERID_FIELD.number = 1
ONHORSE_PLAYERID_FIELD.index = 0
ONHORSE_PLAYERID_FIELD.label = 2
ONHORSE_PLAYERID_FIELD.has_default_value = false
ONHORSE_PLAYERID_FIELD.default_value = 0
ONHORSE_PLAYERID_FIELD.type = 4
ONHORSE_PLAYERID_FIELD.cpp_type = 4

ONHORSE_HORSEID_FIELD.name = "horseid"
ONHORSE_HORSEID_FIELD.full_name = ".OnHorse.horseid"
ONHORSE_HORSEID_FIELD.number = 2
ONHORSE_HORSEID_FIELD.index = 1
ONHORSE_HORSEID_FIELD.label = 2
ONHORSE_HORSEID_FIELD.has_default_value = false
ONHORSE_HORSEID_FIELD.default_value = 0
ONHORSE_HORSEID_FIELD.type = 13
ONHORSE_HORSEID_FIELD.cpp_type = 3

ONHORSE.name = "OnHorse"
ONHORSE.full_name = ".OnHorse"
ONHORSE.nested_types = {}
ONHORSE.enum_types = {}
ONHORSE.fields = {ONHORSE_PLAYERID_FIELD, ONHORSE_HORSEID_FIELD}
ONHORSE.is_extendable = false
ONHORSE.extensions = {}
FLYSTATE_FLY_FIELD.name = "fly"
FLYSTATE_FLY_FIELD.full_name = ".FlyState.fly"
FLYSTATE_FLY_FIELD.number = 1
FLYSTATE_FLY_FIELD.index = 0
FLYSTATE_FLY_FIELD.label = 2
FLYSTATE_FLY_FIELD.has_default_value = false
FLYSTATE_FLY_FIELD.default_value = 0
FLYSTATE_FLY_FIELD.type = 5
FLYSTATE_FLY_FIELD.cpp_type = 1

FLYSTATE.name = "FlyState"
FLYSTATE.full_name = ".FlyState"
FLYSTATE.nested_types = {}
FLYSTATE.enum_types = {}
FLYSTATE.fields = {FLYSTATE_FLY_FIELD}
FLYSTATE.is_extendable = false
FLYSTATE.extensions = {}

BuyHorse = protobuf.Message(BUYHORSE)
BuyHorseAns = protobuf.Message(BUYHORSEANS)
FlyState = protobuf.Message(FLYSTATE)
HorseAddSoul = protobuf.Message(HORSEADDSOUL)
HorseAttr = protobuf.Message(HORSEATTR)
HorseAttrAns = protobuf.Message(HORSEATTRANS)
HorseCommonAttr = protobuf.Message(HORSECOMMONATTR)
HorseData = protobuf.Message(HORSEDATA)
HorseId = protobuf.Message(HORSEID)
HorseList = protobuf.Message(HORSELIST)
HorseSoulAns = protobuf.Message(HORSESOULANS)
HorseSoulLevelAns = protobuf.Message(HORSESOULLEVELANS)
HorseStepAns = protobuf.Message(HORSESTEPANS)
OnHorse = protobuf.Message(ONHORSE)
OnHorseRequest = protobuf.Message(ONHORSEREQUEST)
SetCurHorseAns = protobuf.Message(SETCURHORSEANS)

