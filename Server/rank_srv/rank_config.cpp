#include "rank_config.h"
#include "sproto.h"
#include "sprotoc_common.h"
#include "lua_load.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <fcntl.h>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "excel_data.h"

typedef std::map<uint64_t, void *> *config_type;
std::map<uint64_t, struct WorldBossTable*> rank_world_boss_config; //世界boss表
std::map<uint64_t, struct ActorAttributeTable *> actor_attribute_config;
std::map<uint64_t, struct MonsterTable *> monster_config;
std::map<uint64_t, struct WorldBossRewardTable *> world_boss_reward_config;
std::map<uint64_t, struct ParameterTable *> parameter_config;
#define READ_SPB_MAX_LEN (1024 * 1024)
int read_all_rank_excel_data()
{
	char *buf = (char *)malloc(READ_SPB_MAX_LEN);
	if (!buf)
		return -1;
	int fd = open("../excel_data/1.spb", O_RDONLY);
	if (fd <= 0) {
		return (-1);
	}
	size_t size =  read(fd, buf, READ_SPB_MAX_LEN);
	struct sproto *sp = sproto_create(&buf[0], size);
	close(fd);
    lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	int ret;
	struct sproto_type *type = sproto_type(sp, "WorldBossTable");
	assert(type);
	ret = traverse_main_table(L, type, "../lua_data/WorldBossTable.lua", (config_type)&rank_world_boss_config);
	assert(ret == 0);

	type = sproto_type(sp, "ActorAttributeTable");
	assert(type);	
	ret = traverse_main_table(L, type, "../lua_data/ActorAttributeTable.lua", (config_type)&actor_attribute_config);
	assert(ret == 0);

	type = sproto_type(sp, "MonsterTable");
	assert(type);		
	ret = traverse_main_table(L, type, "../lua_data/MonsterTable.lua", (config_type)&monster_config);
	assert(ret == 0);

	type = sproto_type(sp, "WorldBossRewardTable");
	assert(type);		
	ret = traverse_main_table(L, type, "../lua_data/WorldBossRewardTable.lua", (config_type)&world_boss_reward_config);
	assert(ret == 0);

	type = sproto_type(sp, "ParameterTable");
	assert(type);		
	ret = traverse_main_table(L, type, "../lua_data/ParameterTable.lua", (config_type)&parameter_config);
	assert(ret == 0);

	lua_close(L);	
	free(buf);
	return (0);
}



