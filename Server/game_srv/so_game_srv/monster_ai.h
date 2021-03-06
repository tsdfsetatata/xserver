#ifndef MONSTER_AI_NORMAL_H
#define MONSTER_AI_NORMAL_H

#include "monster.h"
#include "player.h"
#include <stdint.h>

extern struct ai_interface monster_ai_normal_interface;
extern struct ai_interface monster_ai_circle_interface;
extern struct ai_interface monster_ai_0_interface;
extern struct ai_interface monster_ai_3_interface;
extern struct ai_interface monster_ai_4_interface;
extern struct ai_interface monster_ai_5_interface;
//extern struct ai_interface monster_ai_7_interface;
extern struct ai_interface monster_ai_8_interface;
extern struct ai_interface monster_ai_10_interface;
extern struct ai_interface monster_ai_11_interface;
extern struct ai_interface monster_ai_12_interface;
extern struct ai_interface monster_ai_13_interface;
extern struct ai_interface monster_ai_14_interface;
extern struct ai_interface monster_ai_15_interface;
extern struct ai_interface monster_ai_16_interface;
extern struct ai_interface monster_ai_17_interface;
extern struct ai_interface monster_ai_18_interface;
extern struct ai_interface monster_ai_19_interface;
extern struct ai_interface monster_ai_20_interface;
extern struct ai_interface monster_ai_21_interface;
extern struct ai_interface monster_ai_22_interface;
extern struct ai_interface monster_ai_23_interface;
extern struct ai_interface monster_ai_24_interface;
extern struct ai_interface monster_ai_25_interface;
extern struct ai_interface monster_ai_26_interface;
extern struct ai_interface monster_ai_27_interface;
extern struct ai_interface monster_ai_28_interface;
extern struct ai_interface monster_ai_29_interface;
extern struct ai_interface monster_ai_30_interface;
extern struct ai_interface monster_ai_31_interface;
extern struct ai_interface monster_ai_32_interface;
extern struct ai_interface monster_ai_33_interface;
extern struct ai_interface monster_ai_34_interface;

void monster_try_skill_talk(monster_struct *monster, uint64_t skill_id);
int get_monster_hp_percent(monster_struct *monster);
//计算技能硬直时间
uint32_t count_skill_delay_time(struct SkillTable *config);
//uint32_t choose_rand_skill(monster_struct *monster);
uint32_t choose_first_skill(monster_struct *monster);
uint32_t choose_skill_and_add_cd(monster_struct *monster);
//void send_patrol_move(monster_struct *monster);

void try_attack_friend(unit_struct *attack, struct SkillTable *config);
void try_attack_target(unit_struct *attack, unit_struct *target, struct SkillTable *config);
void try_attack(unit_struct *attack, struct SkillTable *config);
	
void hit_notify_to_many_friend(uint64_t skill_id, unit_struct *monster, std::vector<unit_struct *> *target);
void hit_notify_to_target(uint64_t skill_id, unit_struct *attack, unit_struct *target);
void hit_notify_to_many_target(uint64_t skill_id, unit_struct *attack, std::vector<unit_struct *> *target);
//void monster_hit_notify_to_many_player(uint64_t skill_id, monster_struct *monster, player_struct *owner, std::vector<unit_struct *> *target);
//void monster_hit_notify_to_player(uint64_t skill_id, monster_struct *monster, unit_struct *player);
void monster_cast_skill_to_target(uint64_t skill_id, monster_struct *monster, unit_struct *target, bool use_target_pos);
void monster_cast_call_monster_skill(monster_struct *monster, uint64_t skill_id);
void monster_cast_skill_to_direct(uint64_t skill_id, monster_struct *monster, float direct_x, float direct_z, struct position *target);

void cast_immediate_skill_to_target(uint64_t skill_id, uint32_t skill_lv, unit_struct *attack, unit_struct *target);
//void monster_cast_skill_to_pos(uint64_t skill_id, monster_struct *monster, float pos_x, float pos_z);
//void monster_cast_immediate_skill_to_player(uint64_t skill_id, monster_struct *monster, player_struct *owner, unit_struct *player);
//void monster_cast_skill_to_friend(monster_struct *monster, struct SkillTable *config);
//void monster_cast_delay_skill_to_player(uint64_t skill_id, monster_struct *monster, unit_struct *player);
int set_monster_skill_next_timeout(monster_struct *monster);
void do_monster_active_attack_to_target(monster_struct *monster, uint32_t skill_id, struct SkillTable *config);
bool check_monster_relive(monster_struct *monster);
void normal_ai_dead(monster_struct *monster, scene_struct *scene);
void do_normal_patrol(monster_struct *monster);
void do_normal_attack(monster_struct *monster);
void do_normal_pursue(monster_struct *monster);
void do_normal_dead(monster_struct *monster);

//定点巡逻以及单线不返回巡逻ai公用接口
void do_circlea_or_type22_ai_patrol(monster_struct *monster);
void do_type30_ai_patrol(monster_struct *monster);
void circle_ai_beattack(monster_struct *monster, unit_struct *player);
void circle_ai_befly(monster_struct *monster, unit_struct *player);
bool circle_ai_check_goback(monster_struct *monster);
void circle_ai_do_goback(monster_struct *monster);
void do_type22_ai_wait(monster_struct *monster);
void type22_ai_do_goback(monster_struct *monster);
#endif /* MONSTER_AI_NORMAL_H */
