/**
 *  TODO:
 *  config.h should be auto-generated.
 *
 **/
// for a ruby gem
#include <ruby.h>
 // for our C code
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "pthread.h"

#include <libspotify/api.h>
#include "spotify_jukebox.h"


static char cwd[1024];
static char newurl[256];

static VALUE rubyClassSpotify;

typedef struct player_globals {
  /// The global session handle
  sp_session    **pg_sess;
  int             *mode;  //stopped, playing, paused
  pthread_t       playerThread;
  pthread_mutex_t *pnew_cmd_ready;
  char            *title;
  double          saved_volume;     // 0.0 to 1.0
  char            muted;
} sSpotGlobals;

//!
//! @brief     This is thethread that plays the media
//! @param[in] sSpotGlobals * pointer to existing struct
//! @param[out] none
//! @return 
//!     NULL          Function must return something
//! @details
//!     This function is essentially the background thread
//!     It manages the MPG123 player.
//!     It is the audio loop that decodes and plays media.
//!     The audio loop is in control_generic()
//!
void *PlayerThread(void *gPtr)
{
  int             result;
  sSpotGlobals   *pSpotGlobals = (sSpotGlobals *) gPtr;

  if (gPtr == NULL) 
  {
    return (void*)NULL;
  }

  pSpotGlobals->muted = 0;
  pSpotGlobals->mode = &player_mode;

  printf("PlayerThread Spotify started\n");
  result = spotify_infinite_loop(); 
  printf("PlayerThread finished\n");

  /* the function must return something - NULL will do */
  return (void*) result;
}

static void spotRuby_free( void *p)
{
  sSpotGlobals   *pSpotGlobals = (sSpotGlobals *) p;
  printf("spotRuby_free\n");
  // close and free the handle?  I think this is sSpotGlobals *ptr
  // kill thread
  if ( pSpotGlobals != NULL) {
    if (pSpotGlobals->playerThread != 0) {

    }
  }
}

VALUE spotRuby_alloc(VALUE klass) {
  sSpotGlobals  *pSpotGlobals = NULL;
  VALUE           obj;

  // TODO: if one exists, then ignore this.
  // Make this a singleton
	printf("new!\n");
  // wrap the C structure in Ruby
  obj = Data_Make_Struct(klass, sSpotGlobals, 0, spotRuby_free, pSpotGlobals);

  // start a thread
  /* create a second thread which executes inc_x(&x) */
  if(pthread_create(&pSpotGlobals->playerThread, NULL, PlayerThread, pSpotGlobals)) {
    fprintf(stderr, "Error creating thread\n");
  }

  return obj;
}


VALUE spotRuby_login(VALUE self, VALUE username, VALUE password)
{
  sSpotGlobals  *pSpotGlobals = NULL;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  printf("call spotRuby_login %s\n", (char*) RSTRING_PTR(username));
  Check_Type(username, T_STRING);
  Check_Type(password, T_STRING);
  //INT2FIX(mpg123_load(DATA_PTR(self), (char*) RSTRING_PTR(url)));
  if (pSpotGlobals == NULL)
    return Qnil;

  // if we are logged in, then logout.
  // login
  spotify_login((char*) RSTRING_PTR(username), (char*) RSTRING_PTR(password));

  return INT2NUM(*pSpotGlobals->mode);    // what should success look like?
}


VALUE spotRuby_logout(VALUE self)
{
  sSpotGlobals  *pSpotGlobals = NULL;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);
  if (pSpotGlobals == NULL)
    return Qnil;


  return Qnil;
}

VALUE spotRuby_load(VALUE self, VALUE url)
{
  sSpotGlobals  *pSpotGlobals = NULL;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  printf("call spotRuby_load %s\n", (char*) RSTRING_PTR(url));
  Check_Type(url, T_STRING);
  //INT2FIX(mpg123_load(DATA_PTR(self), (char*) RSTRING_PTR(url)));
  if (pSpotGlobals == NULL)
    return Qnil;

  strcpy(newurl, (char*) RSTRING_PTR(url));
  spotify_select_playlist(newurl);
  // get access, put in new load command and release the new_cmd mutex/lock
  //strcpy(newurl, (char*) RSTRING_PTR(url));
  //pthread_mutex_trylock(&new_cmd_ready);
  //new_cmd = 'L';
  //pthread_mutex_unlock(&new_cmd_ready);

  return INT2NUM(*pSpotGlobals->mode);    // what should success look like?
}

VALUE spotRuby_pause(VALUE self)
{
  sSpotGlobals  *pSpotGlobals = NULL;
  int newMode = MODE_STOPPED;
  int retValue;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  if (pSpotGlobals == NULL)
    return Qnil;

  printf("Spot_pause before %d\n", *pSpotGlobals->mode);
  if (*pSpotGlobals->mode == MODE_PAUSED)
    newMode = MODE_PLAYING;
  else if (*pSpotGlobals->mode == MODE_PLAYING)
    newMode = MODE_PAUSED;
  retValue = spotify_play_pause(newMode);
  printf("spot pause returned %d\n", retValue);

  return INT2NUM(*pSpotGlobals->mode);
}

double GetOutputVolume(sSpotGlobals *pSpotGlobals)
{
  double v = 0.0;
  // get current volume
  if (pSpotGlobals->muted)
    v = pSpotGlobals->saved_volume;
  else
    v = audio_get_volume();
    //mpg123_getvolume(pSpotGlobals->mh, &v, NULL, NULL); /* Necessary? */
  return v;  
}

// set/get volume.
// volume in/out of this is 0-100;  internally 0-1.0
VALUE spotRuby_volume(int argc, VALUE *argv, VALUE self)
{
  double  v = 0.0;
  VALUE   newvol;
  sSpotGlobals  *pSpotGlobals = NULL;

  rb_scan_args(argc, argv, "01", &newvol);    // throws exception if wrong # ags (0 req, 1 optional)

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  if (pSpotGlobals == NULL)
    return Qnil;
  // set new, return new
  if (newvol != Qnil) {
    v = NUM2DBL(newvol);
    if (pSpotGlobals->muted)
      pSpotGlobals->saved_volume = v/100.0;
    else
      audio_set_volume(v/100.0);
      //mpg123_volume(pSpotGlobals->mh, v/100);
  }
  return DBL2NUM(GetOutputVolume(pSpotGlobals)*100);
}

// jump to frame num,
// return frame - new current position
VALUE spotRuby_frame(int argc, VALUE *argv, VALUE self)
{
  VALUE   frame_num;
  sSpotGlobals  *pSpotGlobals = NULL;
  off_t   offset = 0;
  off_t oldpos;

  rb_scan_args(argc, argv, "01", &frame_num);    // throws exception if wrong # ags (0 req, 1 optional)

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  if (pSpotGlobals == NULL)
    return Qnil;
  // set new, return new
  if (frame_num != Qnil) {
    offset = NUM2INT(frame_num);
  }

  if(*pSpotGlobals->mode == MODE_STOPPED)
  {
    return Qnil;
  }
  oldpos = frame_num;


  return INT2NUM(frame_num);
}

// toggle mute
VALUE spotRuby_mute(VALUE self)
{
  sSpotGlobals  *pSpotGlobals = NULL;
  double          current_volume;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  if (pSpotGlobals == NULL) return Qnil;

  // if muted, unmute, restore volume
  if (pSpotGlobals->muted) {
    pSpotGlobals->muted = 0;
    audio_set_volume (pSpotGlobals->saved_volume);
  }
  // else mute, save volume
  else {
    pSpotGlobals->saved_volume = audio_get_volume();
    pSpotGlobals->muted = 1;
    audio_set_volume(0.0f);
  }

  return INT2NUM(pSpotGlobals->muted);
}

VALUE spotRuby_shuffle(VALUE self, VALUE shuffle)
{
  sSpotGlobals  *pSpotGlobals = NULL;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);
  Check_Type(shuffle, T_FIXNUM);

  if (pSpotGlobals == NULL)
    return Qnil;

  g_shuffle_playlist = NUM2INT(shuffle);

  return INT2NUM(*pSpotGlobals->mode);
}

 VALUE spotRuby_loopsong(VALUE self)
{
  sSpotGlobals  *pSpotGlobals = NULL;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  if (pSpotGlobals == NULL)
    return Qnil;

  g_loop_song ^= 1;   // toggle

  return INT2NUM(*pSpotGlobals->mode);
}

 VALUE spotRuby_looplist(VALUE self)
{
  sSpotGlobals  *pSpotGlobals = NULL;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  if (pSpotGlobals == NULL)
    return Qnil;

  return INT2NUM(*pSpotGlobals->mode);
}

 
/*
 *  This can be a ruby function
 */
VALUE spotRuby_state(VALUE self)
{
  sSpotGlobals  *pSpotGlobals = NULL;

  // get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  if (pSpotGlobals == NULL)
    return Qnil;

  return INT2NUM(pSpotGlobals->mode);
}

//
// info return a hash of
// mode:
// source-type: 
// source:
// volume:
// mute:
// shuffle:
// loopsong:
// looplist:
// position {...}
//  frame:
//  num-frames:
//  seconds:
//  num-seconds:
// position stuff:
//@F 5808 2361 151.72 61.68
//@F 5809 2360 151.75 61.65
//@F 5810 2359 151.77 61.62
// song {....}
/***
    track-name:   song:
    artist-name:  artist:
    album-name:   album:
    track-num:    track:
***/
VALUE spotRuby_info(VALUE self)
{
  sSpotGlobals  *pSpotGlobals = NULL;
  VALUE           rbHash;
  VALUE           songHash;
  off_t           pos;
  off_t           len;
  //mpg123_id3v1 *v1;
  //mpg123_id3v2 *v2;
  off_t           current_frame, frames_left;
  double          current_seconds, seconds_left;

// get the handle
  Data_Get_Struct(self, sSpotGlobals, pSpotGlobals);

  if (pSpotGlobals == NULL)
    return Qnil;

  // get state of the player and return a hash
  rbHash = rb_hash_new();
  // player state
  rb_hash_aset(rbHash, rb_str_new2("mode"),         INT2NUM(*pSpotGlobals->mode));
  rb_hash_aset(rbHash, rb_str_new2("source-type"),  INT2NUM(0));
  //rb_hash_aset(rbHash, rb_str_new2("source"),       rb_str_new2(newurl));
#if 0
  pos = mpg123_tell(pSpotGlobals->mh);
  len = mpg123_length(pSpotGlobals->mh);
  rb_hash_aset(rbHash, rb_str_new2("frame"),        INT2NUM(pos));
  rb_hash_aset(rbHash, rb_str_new2("num-frames"),   INT2NUM(len));
#endif
  rb_hash_aset(rbHash, rb_str_new2("volume"),       DBL2NUM(GetOutputVolume(pSpotGlobals)));
  rb_hash_aset(rbHash, rb_str_new2("mute"),         INT2NUM(pSpotGlobals->muted));
  
  rb_hash_aset(rbHash, rb_str_new2("shuffle"),  INT2NUM(g_shuffle_playlist));
  rb_hash_aset(rbHash, rb_str_new2("loopsong"), INT2NUM(g_loop_song));
  rb_hash_aset(rbHash, rb_str_new2("looplist"), INT2NUM(0));

  // position info

  /*
  if(!mpg123_position(pSpotGlobals->mh, 0, xfermem_get_usedspace(buffermem), &current_frame, &frames_left, &current_seconds, &seconds_left)) 
  {
    VALUE positionHash = rb_hash_new();
    //generic_sendmsg("F %"OFF_P" %"OFF_P" %3.2f %3.2f", (off_p)current_frame, (off_p)frames_left, current_seconds, seconds_left);
    rb_hash_aset(positionHash, rb_str_new2("seconds"),      DBL2NUM(current_seconds));
    rb_hash_aset(positionHash, rb_str_new2("num-seconds"),  DBL2NUM(current_seconds+seconds_left));
    rb_hash_aset(positionHash, rb_str_new2("frame"),        INT2NUM(current_frame));
    rb_hash_aset(positionHash, rb_str_new2("num-frames"),   INT2NUM(current_frame+frames_left));
    rb_hash_aset(rbHash, rb_str_new2("position"), positionHash);
  }
  */
 
  // song info
  songHash = rb_hash_new();
  /*
  if(MPG123_OK == mpg123_id3(pSpotGlobals->mh, &v1, &v2))
  {
    if(v1 != NULL && v2 == NULL)
    {
      rb_hash_aset(songHash, rb_str_new2("track-name"),   rb_str_new2(v1->title));
      rb_hash_aset(songHash, rb_str_new2("artist-name"),  rb_str_new2(v1->artist));
      rb_hash_aset(songHash, rb_str_new2("album-name"),   rb_str_new2(v1->album));
    }
    else if(v2 != NULL)
    {
      //generic_sendmsg("I ID3v2.year:%s",    v2->year);
      //generic_sendmsg("I ID3v2.comment:%s", v2->comment);
      //generic_sendmsg("I ID3v2.genre:%s",   v2->genre);
      if (v2->title != NULL)
        rb_hash_aset(songHash, rb_str_new2("track-name"),   rb_str_new2(v2->title->p));
      else if (v1 != NULL)
        rb_hash_aset(songHash, rb_str_new2("track-name"),   rb_str_new2(v1->title));
      if (v2->artist != NULL)
        rb_hash_aset(songHash, rb_str_new2("artist-name"),  rb_str_new2(v2->artist->p));
      else if (v1 != NULL)
        rb_hash_aset(songHash, rb_str_new2("artist-name"),  rb_str_new2(v1->artist));
      if (v2->album != NULL)
        rb_hash_aset(songHash, rb_str_new2("album-name"),   rb_str_new2(v2->album->p));
      else if (v1 != NULL)
        rb_hash_aset(songHash, rb_str_new2("album-name"),   rb_str_new2(v1->album));
    }
    if (v1 != NULL)
    {
      if(v1->comment[28] == 0 && v1->comment[29] != 0)
        rb_hash_aset(songHash, rb_str_new2("track-num"),    INT2NUM((unsigned char)v1->comment[29]));
    }
  }
  */

  rb_hash_aset(rbHash, rb_str_new2("song"), songHash);

  return rbHash;
}


void Init_spotify_ruby(void) {

  // init local things

  // connect to ruby class
  rubyClassSpotify = rb_define_class("Spotify_ruby", rb_cObject);

  rb_define_alloc_func(rubyClassSpotify, spotRuby_alloc);
  rb_define_method(rubyClassSpotify, "login",   spotRuby_login, 2);
  rb_define_method(rubyClassSpotify, "logout",  spotRuby_logout, 0);
  rb_define_method(rubyClassSpotify, "load",   spotRuby_load, 1);
  rb_define_method(rubyClassSpotify, "pause",  spotRuby_pause, 0);
  //rb_define_method(rubyClassSpotify, "stop",   spotRuby_stop, 0);
  //rb_define_method(rubyClassSpotify, "play",   spotRuby_play, 0);

  rb_define_method(rubyClassSpotify, "info",   spotRuby_info, 0);
  rb_define_method(rubyClassSpotify, "frame",  spotRuby_frame, -1);
  //rb_define_method(rubyClassSpotify, "equalizer",   spotRuby_equalizer, 0);

  rb_define_method(rubyClassSpotify, "volume", spotRuby_volume, -1);
  rb_define_method(rubyClassSpotify, "mute",   spotRuby_mute, 0);

  rb_define_method(rubyClassSpotify, "shuffle",  spotRuby_shuffle, 1);
  rb_define_method(rubyClassSpotify, "loopsong", spotRuby_loopsong, 0);
  rb_define_method(rubyClassSpotify, "looplist", spotRuby_looplist, 0);

}


/**
*!
*!  pause()           pause/resume
*!  stop()            stop.  not resumable
*!  tell()            return where we are (sample and num samples)
*!  seek(frame)       go that spot or relative frame or jump
*!  volume[new]       set volume if given; return volume
*!  mute()            silence the audio  - set volume 0 or does mute toggle?
*!  load(uri)         load file or url start playing
*!  eq(...)           set equalier, return current?
*!  tags()            return tags for this file
*!  info()
*!  format()          return sample rate in hz and channel
*!  state()           return 0, 1, 2.  stopped, playing, paused
*!
**/
