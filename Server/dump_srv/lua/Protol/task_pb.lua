-- Generated By protoc-gen-lua Do not Edit
local protobuf = require "protobuf"
local comm_message_pb = require("comm_message_pb")
module('task_pb')


local TASKCOMMREQUEST = protobuf.Descriptor();
local TASKCOMMREQUEST_TASK_ID_FIELD = protobuf.FieldDescriptor();
local TASKCOMMANSWER = protobuf.Descriptor();
local TASKCOMMANSWER_RESULT_FIELD = protobuf.FieldDescriptor();
local TASKCOMMANSWER_TASK_ID_FIELD = protobuf.FieldDescriptor();
local TASKUPDATENOTIFY = protobuf.Descriptor();
local TASKUPDATENOTIFY_DATA_FIELD = protobuf.FieldDescriptor();
local TASKLISTANSWER = protobuf.Descriptor();
local TASKLISTANSWER_RESULT_FIELD = protobuf.FieldDescriptor();
local TASKLISTANSWER_ONGOING_LIST_FIELD = protobuf.FieldDescriptor();
local TASKLISTANSWER_FINISH_LIST_FIELD = protobuf.FieldDescriptor();
local TASKLISTANSWER_CHAPTERID_FIELD = protobuf.FieldDescriptor();
local TASKLISTANSWER_CHAPTERSTATE_FIELD = protobuf.FieldDescriptor();
local TASKCOMPLETEREQUEST = protobuf.Descriptor();
local TASKCOMPLETEREQUEST_TASK_ID_FIELD = protobuf.FieldDescriptor();
local TASKCOMPLETEREQUEST_CONDITION_ID_FIELD = protobuf.FieldDescriptor();
local TASKEVENTREQUEST = protobuf.Descriptor();
local TASKEVENTREQUEST_TASK_ID_FIELD = protobuf.FieldDescriptor();
local TASKEVENTREQUEST_EVENT_ID_FIELD = protobuf.FieldDescriptor();
local TASKREMOVENOTIFY = protobuf.Descriptor();
local TASKREMOVENOTIFY_TASK_ID_FIELD = protobuf.FieldDescriptor();
local TASKUPDATEFINISHNOTIFY = protobuf.Descriptor();
local TASKUPDATEFINISHNOTIFY_ADDS_FIELD = protobuf.FieldDescriptor();
local TASKUPDATEFINISHNOTIFY_DELS_FIELD = protobuf.FieldDescriptor();
local TASKUPDATECHAPTERREWARDNOTIFY = protobuf.Descriptor();
local TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD = protobuf.FieldDescriptor();
local TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD = protobuf.FieldDescriptor();

TASKCOMMREQUEST_TASK_ID_FIELD.name = "task_id"
TASKCOMMREQUEST_TASK_ID_FIELD.full_name = ".TaskCommRequest.task_id"
TASKCOMMREQUEST_TASK_ID_FIELD.number = 1
TASKCOMMREQUEST_TASK_ID_FIELD.index = 0
TASKCOMMREQUEST_TASK_ID_FIELD.label = 2
TASKCOMMREQUEST_TASK_ID_FIELD.has_default_value = false
TASKCOMMREQUEST_TASK_ID_FIELD.default_value = 0
TASKCOMMREQUEST_TASK_ID_FIELD.type = 13
TASKCOMMREQUEST_TASK_ID_FIELD.cpp_type = 3

TASKCOMMREQUEST.name = "TaskCommRequest"
TASKCOMMREQUEST.full_name = ".TaskCommRequest"
TASKCOMMREQUEST.nested_types = {}
TASKCOMMREQUEST.enum_types = {}
TASKCOMMREQUEST.fields = {TASKCOMMREQUEST_TASK_ID_FIELD}
TASKCOMMREQUEST.is_extendable = false
TASKCOMMREQUEST.extensions = {}
TASKCOMMANSWER_RESULT_FIELD.name = "result"
TASKCOMMANSWER_RESULT_FIELD.full_name = ".TaskCommAnswer.result"
TASKCOMMANSWER_RESULT_FIELD.number = 1
TASKCOMMANSWER_RESULT_FIELD.index = 0
TASKCOMMANSWER_RESULT_FIELD.label = 2
TASKCOMMANSWER_RESULT_FIELD.has_default_value = false
TASKCOMMANSWER_RESULT_FIELD.default_value = 0
TASKCOMMANSWER_RESULT_FIELD.type = 13
TASKCOMMANSWER_RESULT_FIELD.cpp_type = 3

TASKCOMMANSWER_TASK_ID_FIELD.name = "task_id"
TASKCOMMANSWER_TASK_ID_FIELD.full_name = ".TaskCommAnswer.task_id"
TASKCOMMANSWER_TASK_ID_FIELD.number = 2
TASKCOMMANSWER_TASK_ID_FIELD.index = 1
TASKCOMMANSWER_TASK_ID_FIELD.label = 2
TASKCOMMANSWER_TASK_ID_FIELD.has_default_value = false
TASKCOMMANSWER_TASK_ID_FIELD.default_value = 0
TASKCOMMANSWER_TASK_ID_FIELD.type = 13
TASKCOMMANSWER_TASK_ID_FIELD.cpp_type = 3

TASKCOMMANSWER.name = "TaskCommAnswer"
TASKCOMMANSWER.full_name = ".TaskCommAnswer"
TASKCOMMANSWER.nested_types = {}
TASKCOMMANSWER.enum_types = {}
TASKCOMMANSWER.fields = {TASKCOMMANSWER_RESULT_FIELD, TASKCOMMANSWER_TASK_ID_FIELD}
TASKCOMMANSWER.is_extendable = false
TASKCOMMANSWER.extensions = {}
TASKUPDATENOTIFY_DATA_FIELD.name = "data"
TASKUPDATENOTIFY_DATA_FIELD.full_name = ".TaskUpdateNotify.data"
TASKUPDATENOTIFY_DATA_FIELD.number = 1
TASKUPDATENOTIFY_DATA_FIELD.index = 0
TASKUPDATENOTIFY_DATA_FIELD.label = 2
TASKUPDATENOTIFY_DATA_FIELD.has_default_value = false
TASKUPDATENOTIFY_DATA_FIELD.default_value = nil
TASKUPDATENOTIFY_DATA_FIELD.message_type = COMM_MESSAGE_PB_TASKDATA
TASKUPDATENOTIFY_DATA_FIELD.type = 11
TASKUPDATENOTIFY_DATA_FIELD.cpp_type = 10

TASKUPDATENOTIFY.name = "TaskUpdateNotify"
TASKUPDATENOTIFY.full_name = ".TaskUpdateNotify"
TASKUPDATENOTIFY.nested_types = {}
TASKUPDATENOTIFY.enum_types = {}
TASKUPDATENOTIFY.fields = {TASKUPDATENOTIFY_DATA_FIELD}
TASKUPDATENOTIFY.is_extendable = false
TASKUPDATENOTIFY.extensions = {}
TASKLISTANSWER_RESULT_FIELD.name = "result"
TASKLISTANSWER_RESULT_FIELD.full_name = ".TaskListAnswer.result"
TASKLISTANSWER_RESULT_FIELD.number = 1
TASKLISTANSWER_RESULT_FIELD.index = 0
TASKLISTANSWER_RESULT_FIELD.label = 2
TASKLISTANSWER_RESULT_FIELD.has_default_value = false
TASKLISTANSWER_RESULT_FIELD.default_value = 0
TASKLISTANSWER_RESULT_FIELD.type = 13
TASKLISTANSWER_RESULT_FIELD.cpp_type = 3

TASKLISTANSWER_ONGOING_LIST_FIELD.name = "ongoing_list"
TASKLISTANSWER_ONGOING_LIST_FIELD.full_name = ".TaskListAnswer.ongoing_list"
TASKLISTANSWER_ONGOING_LIST_FIELD.number = 2
TASKLISTANSWER_ONGOING_LIST_FIELD.index = 1
TASKLISTANSWER_ONGOING_LIST_FIELD.label = 3
TASKLISTANSWER_ONGOING_LIST_FIELD.has_default_value = false
TASKLISTANSWER_ONGOING_LIST_FIELD.default_value = {}
TASKLISTANSWER_ONGOING_LIST_FIELD.message_type = COMM_MESSAGE_PB_TASKDATA
TASKLISTANSWER_ONGOING_LIST_FIELD.type = 11
TASKLISTANSWER_ONGOING_LIST_FIELD.cpp_type = 10

TASKLISTANSWER_FINISH_LIST_FIELD.name = "finish_list"
TASKLISTANSWER_FINISH_LIST_FIELD.full_name = ".TaskListAnswer.finish_list"
TASKLISTANSWER_FINISH_LIST_FIELD.number = 3
TASKLISTANSWER_FINISH_LIST_FIELD.index = 2
TASKLISTANSWER_FINISH_LIST_FIELD.label = 3
TASKLISTANSWER_FINISH_LIST_FIELD.has_default_value = false
TASKLISTANSWER_FINISH_LIST_FIELD.default_value = {}
TASKLISTANSWER_FINISH_LIST_FIELD.type = 13
TASKLISTANSWER_FINISH_LIST_FIELD.cpp_type = 3

TASKLISTANSWER_CHAPTERID_FIELD.name = "chapterId"
TASKLISTANSWER_CHAPTERID_FIELD.full_name = ".TaskListAnswer.chapterId"
TASKLISTANSWER_CHAPTERID_FIELD.number = 4
TASKLISTANSWER_CHAPTERID_FIELD.index = 3
TASKLISTANSWER_CHAPTERID_FIELD.label = 2
TASKLISTANSWER_CHAPTERID_FIELD.has_default_value = false
TASKLISTANSWER_CHAPTERID_FIELD.default_value = 0
TASKLISTANSWER_CHAPTERID_FIELD.type = 13
TASKLISTANSWER_CHAPTERID_FIELD.cpp_type = 3

TASKLISTANSWER_CHAPTERSTATE_FIELD.name = "chapterState"
TASKLISTANSWER_CHAPTERSTATE_FIELD.full_name = ".TaskListAnswer.chapterState"
TASKLISTANSWER_CHAPTERSTATE_FIELD.number = 5
TASKLISTANSWER_CHAPTERSTATE_FIELD.index = 4
TASKLISTANSWER_CHAPTERSTATE_FIELD.label = 2
TASKLISTANSWER_CHAPTERSTATE_FIELD.has_default_value = false
TASKLISTANSWER_CHAPTERSTATE_FIELD.default_value = 0
TASKLISTANSWER_CHAPTERSTATE_FIELD.type = 13
TASKLISTANSWER_CHAPTERSTATE_FIELD.cpp_type = 3

TASKLISTANSWER.name = "TaskListAnswer"
TASKLISTANSWER.full_name = ".TaskListAnswer"
TASKLISTANSWER.nested_types = {}
TASKLISTANSWER.enum_types = {}
TASKLISTANSWER.fields = {TASKLISTANSWER_RESULT_FIELD, TASKLISTANSWER_ONGOING_LIST_FIELD, TASKLISTANSWER_FINISH_LIST_FIELD, TASKLISTANSWER_CHAPTERID_FIELD, TASKLISTANSWER_CHAPTERSTATE_FIELD}
TASKLISTANSWER.is_extendable = false
TASKLISTANSWER.extensions = {}
TASKCOMPLETEREQUEST_TASK_ID_FIELD.name = "task_id"
TASKCOMPLETEREQUEST_TASK_ID_FIELD.full_name = ".TaskCompleteRequest.task_id"
TASKCOMPLETEREQUEST_TASK_ID_FIELD.number = 1
TASKCOMPLETEREQUEST_TASK_ID_FIELD.index = 0
TASKCOMPLETEREQUEST_TASK_ID_FIELD.label = 2
TASKCOMPLETEREQUEST_TASK_ID_FIELD.has_default_value = false
TASKCOMPLETEREQUEST_TASK_ID_FIELD.default_value = 0
TASKCOMPLETEREQUEST_TASK_ID_FIELD.type = 13
TASKCOMPLETEREQUEST_TASK_ID_FIELD.cpp_type = 3

TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.name = "condition_id"
TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.full_name = ".TaskCompleteRequest.condition_id"
TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.number = 2
TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.index = 1
TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.label = 2
TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.has_default_value = false
TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.default_value = 0
TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.type = 13
TASKCOMPLETEREQUEST_CONDITION_ID_FIELD.cpp_type = 3

TASKCOMPLETEREQUEST.name = "TaskCompleteRequest"
TASKCOMPLETEREQUEST.full_name = ".TaskCompleteRequest"
TASKCOMPLETEREQUEST.nested_types = {}
TASKCOMPLETEREQUEST.enum_types = {}
TASKCOMPLETEREQUEST.fields = {TASKCOMPLETEREQUEST_TASK_ID_FIELD, TASKCOMPLETEREQUEST_CONDITION_ID_FIELD}
TASKCOMPLETEREQUEST.is_extendable = false
TASKCOMPLETEREQUEST.extensions = {}
TASKEVENTREQUEST_TASK_ID_FIELD.name = "task_id"
TASKEVENTREQUEST_TASK_ID_FIELD.full_name = ".TaskEventRequest.task_id"
TASKEVENTREQUEST_TASK_ID_FIELD.number = 1
TASKEVENTREQUEST_TASK_ID_FIELD.index = 0
TASKEVENTREQUEST_TASK_ID_FIELD.label = 2
TASKEVENTREQUEST_TASK_ID_FIELD.has_default_value = false
TASKEVENTREQUEST_TASK_ID_FIELD.default_value = 0
TASKEVENTREQUEST_TASK_ID_FIELD.type = 13
TASKEVENTREQUEST_TASK_ID_FIELD.cpp_type = 3

TASKEVENTREQUEST_EVENT_ID_FIELD.name = "event_id"
TASKEVENTREQUEST_EVENT_ID_FIELD.full_name = ".TaskEventRequest.event_id"
TASKEVENTREQUEST_EVENT_ID_FIELD.number = 2
TASKEVENTREQUEST_EVENT_ID_FIELD.index = 1
TASKEVENTREQUEST_EVENT_ID_FIELD.label = 2
TASKEVENTREQUEST_EVENT_ID_FIELD.has_default_value = false
TASKEVENTREQUEST_EVENT_ID_FIELD.default_value = 0
TASKEVENTREQUEST_EVENT_ID_FIELD.type = 13
TASKEVENTREQUEST_EVENT_ID_FIELD.cpp_type = 3

TASKEVENTREQUEST.name = "TaskEventRequest"
TASKEVENTREQUEST.full_name = ".TaskEventRequest"
TASKEVENTREQUEST.nested_types = {}
TASKEVENTREQUEST.enum_types = {}
TASKEVENTREQUEST.fields = {TASKEVENTREQUEST_TASK_ID_FIELD, TASKEVENTREQUEST_EVENT_ID_FIELD}
TASKEVENTREQUEST.is_extendable = false
TASKEVENTREQUEST.extensions = {}
TASKREMOVENOTIFY_TASK_ID_FIELD.name = "task_id"
TASKREMOVENOTIFY_TASK_ID_FIELD.full_name = ".TaskRemoveNotify.task_id"
TASKREMOVENOTIFY_TASK_ID_FIELD.number = 1
TASKREMOVENOTIFY_TASK_ID_FIELD.index = 0
TASKREMOVENOTIFY_TASK_ID_FIELD.label = 2
TASKREMOVENOTIFY_TASK_ID_FIELD.has_default_value = false
TASKREMOVENOTIFY_TASK_ID_FIELD.default_value = 0
TASKREMOVENOTIFY_TASK_ID_FIELD.type = 13
TASKREMOVENOTIFY_TASK_ID_FIELD.cpp_type = 3

TASKREMOVENOTIFY.name = "TaskRemoveNotify"
TASKREMOVENOTIFY.full_name = ".TaskRemoveNotify"
TASKREMOVENOTIFY.nested_types = {}
TASKREMOVENOTIFY.enum_types = {}
TASKREMOVENOTIFY.fields = {TASKREMOVENOTIFY_TASK_ID_FIELD}
TASKREMOVENOTIFY.is_extendable = false
TASKREMOVENOTIFY.extensions = {}
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.name = "adds"
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.full_name = ".TaskUpdateFinishNotify.adds"
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.number = 1
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.index = 0
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.label = 3
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.has_default_value = false
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.default_value = {}
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.type = 13
TASKUPDATEFINISHNOTIFY_ADDS_FIELD.cpp_type = 3

TASKUPDATEFINISHNOTIFY_DELS_FIELD.name = "dels"
TASKUPDATEFINISHNOTIFY_DELS_FIELD.full_name = ".TaskUpdateFinishNotify.dels"
TASKUPDATEFINISHNOTIFY_DELS_FIELD.number = 2
TASKUPDATEFINISHNOTIFY_DELS_FIELD.index = 1
TASKUPDATEFINISHNOTIFY_DELS_FIELD.label = 3
TASKUPDATEFINISHNOTIFY_DELS_FIELD.has_default_value = false
TASKUPDATEFINISHNOTIFY_DELS_FIELD.default_value = {}
TASKUPDATEFINISHNOTIFY_DELS_FIELD.type = 13
TASKUPDATEFINISHNOTIFY_DELS_FIELD.cpp_type = 3

TASKUPDATEFINISHNOTIFY.name = "TaskUpdateFinishNotify"
TASKUPDATEFINISHNOTIFY.full_name = ".TaskUpdateFinishNotify"
TASKUPDATEFINISHNOTIFY.nested_types = {}
TASKUPDATEFINISHNOTIFY.enum_types = {}
TASKUPDATEFINISHNOTIFY.fields = {TASKUPDATEFINISHNOTIFY_ADDS_FIELD, TASKUPDATEFINISHNOTIFY_DELS_FIELD}
TASKUPDATEFINISHNOTIFY.is_extendable = false
TASKUPDATEFINISHNOTIFY.extensions = {}
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.name = "chapterId"
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.full_name = ".TaskUpdateChapterRewardNotify.chapterId"
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.number = 2
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.index = 0
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.label = 2
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.has_default_value = false
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.default_value = 0
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.type = 13
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD.cpp_type = 3

TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.name = "chapterState"
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.full_name = ".TaskUpdateChapterRewardNotify.chapterState"
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.number = 3
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.index = 1
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.label = 2
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.has_default_value = false
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.default_value = 0
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.type = 13
TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD.cpp_type = 3

TASKUPDATECHAPTERREWARDNOTIFY.name = "TaskUpdateChapterRewardNotify"
TASKUPDATECHAPTERREWARDNOTIFY.full_name = ".TaskUpdateChapterRewardNotify"
TASKUPDATECHAPTERREWARDNOTIFY.nested_types = {}
TASKUPDATECHAPTERREWARDNOTIFY.enum_types = {}
TASKUPDATECHAPTERREWARDNOTIFY.fields = {TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERID_FIELD, TASKUPDATECHAPTERREWARDNOTIFY_CHAPTERSTATE_FIELD}
TASKUPDATECHAPTERREWARDNOTIFY.is_extendable = false
TASKUPDATECHAPTERREWARDNOTIFY.extensions = {}

TaskCommAnswer = protobuf.Message(TASKCOMMANSWER)
TaskCommRequest = protobuf.Message(TASKCOMMREQUEST)
TaskCompleteRequest = protobuf.Message(TASKCOMPLETEREQUEST)
TaskEventRequest = protobuf.Message(TASKEVENTREQUEST)
TaskListAnswer = protobuf.Message(TASKLISTANSWER)
TaskRemoveNotify = protobuf.Message(TASKREMOVENOTIFY)
TaskUpdateChapterRewardNotify = protobuf.Message(TASKUPDATECHAPTERREWARDNOTIFY)
TaskUpdateFinishNotify = protobuf.Message(TASKUPDATEFINISHNOTIFY)
TaskUpdateNotify = protobuf.Message(TASKUPDATENOTIFY)
