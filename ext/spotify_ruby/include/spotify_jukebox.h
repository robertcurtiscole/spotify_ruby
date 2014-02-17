/// spotify_jukebox.h

// TODO: this goes in a common file, somewhere
#define MODE_STOPPED 0
#define MODE_PLAYING 1
#define MODE_PAUSED 2

/// The global session handle
extern sp_session *g_sess;
/// Handle to the playlist currently being played
extern sp_playlist *g_jukeboxlist;
// tracking the player state
extern int player_mode;

/// looping and shuffling
extern int g_shuffle_playlist;
extern int g_loop_song;

int spotify_infinite_loop(void);
int spotify_login(const char *username, const char *password);
int spotify_select_playlist(const char *playlist);
int spotify_play_pause(int mode);

