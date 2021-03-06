#ifndef REGISTER_GAMESRV_H
#define REGISTER_GAMESRV_H

extern void install_msg_handle();
extern void uninstall_msg_handle();
extern void install_db_msg_handle();
extern void uninstall_db_msg_handle();
extern void install_ai_msg_handle();
extern void uninstall_ai_msg_handle();

extern int init_signals();
extern void init_http_server(uint32_t port);
extern uint64_t  sg_server_id;
extern uint32_t sg_gm_cmd_open;
extern uint32_t sg_server_open_time;
extern struct timeval gamesrv_timeout;
extern uint64_t timer_loop_count;
extern char * sg_show_url;
extern char * sg_show_path;

typedef int (*game_handle_func)(player_struct*, EXTERN_DATA*);
typedef std::map<uint32_t, game_handle_func> GameHandleMap;
extern GameHandleMap   m_game_handle_map;

typedef int(*db_handle_func)(EXTERN_DATA*);
typedef std::map<uint32_t, db_handle_func> DbHandleMap;
extern DbHandleMap m_db_handle_map;
extern DbHandleMap m_ai_handle_map;


#endif /* REGISTER_GAMESRV_H */

