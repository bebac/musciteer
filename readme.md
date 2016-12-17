
Musciteer
=========

[![Join the chat at https://gitter.im/musciteer/Lobby](https://badges.gitter.im/musciteer/Lobby.svg)](https://gitter.im/musciteer/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Musciteer is a music server for Linux. It combines your local music files with
selected music from streaming services and presents it as one library through
a modern web interface

> NOTE: Currently it only supports flac and spotify



Features
========

A somewhat prioritized list of features / tasks.

- [x] Configurable through the desktop web interface.
- [x] FLAC local files
- [x] Spotify playback
- [x] Import Spotify albums/tracks from web interface. Not optimal, but will have to do for now.
- [x] Continuous playback. If nothing is explictly queued, play songs at random
      from the entire collection. Can be turned on/off from settings.
- [x] Mobile web application. For android open in browser and choose add to home
      screen. It is the same web app, but default page is /player. Don't know
      how it works on iphone.
- [ ] Ubuntu snap package. Not possible currently as snaps cannot access alsa
      :exclamation:
- [x] Debian package for Ubuntu 16.04
- [x] Browse and queue music from mobile web app. Works best on a tablet, on
      small screens it is not very usable.
- [x] Replay gain. Fairly straight forward for FLAC files. Not so for Spotify. See [spotify audio features](doc/spotify_audio_features.md)
- [ ] Tagging
- [ ] More advanced Continuous playback. Most liked, less played, by tag etc.
- [ ] Improve Spotify integration
- [ ] Remove content on rescan when local flac files are removed
- [ ] Play albums
- [ ] Playlists
- [ ] Support additional targets, like arm
- [ ] Ubuntu Touch app
- [ ] FLAC file tagging from web interface / Integrate with online music meta
      data service.
- [ ] Tidal
- [ ] MP3 local files
- [ ] Native Android app
- [ ] OGG local files
- [ ] WAV local files
- [ ] Google play
- [ ] Deezer
- [ ] Native IOS app



API
===

| HTTP Methods | URL
| ------------ | ---
| GET          | /api/tracks
| GET, DELETE  | /api/tracks/:id
| GET          | /api/albums
| GET, DELETE  | /api/albums/:id
| GET          | /api/albums/:id/tracks
| GET          | /api/albums/:id/cover
| GET, POST    | /api/sources/local/directories
| POST         | /api/sources/local/scan
| GET, POST    | /api/sources/spotify/settings
| GET          | /api/player
| GET, POST    | /api/player/ctpb
| GET, POST    | /api/player/output
| GET, POST    | /api/player/replaygain
| POST         | /api/spotify/import


Player control and notifications are done over a websocket.

Client -> Server

```json
{ "event": "play" }
{ "event": "play", "data" : "<track-id>" }
{ "event": "stop" }
{ "event": "skip" }
{ "event": "queue", "data" : "<track-id>" }
{ "event": "stream_data_sync", "data" : "<stream-id>" }
```

Server -> Client

```json
{ "event": "stream_begin", "data" : "<stream-id>" }
{ "event": "stream_end", "data" : "<stream-id>" }
{
    "event": "stream_progress", "data" : {
        "stream_id" : "<stream-id>",
        "duration"  : Number,
        "length"    : Number
    }
}
{
    "event": "queue_update", "data" : {
        "queue_size" : Number,
        "track"      : Object
    }
}
{
    "event": "player_state", "data" : {
        "state" : Number
    }
}
{
    "event": "source_notification", "data" : {
        "type" : Number,
        "source_name" : String,
        "message" : String
    }
}
{
    "event": "stream_data", "data" : {
        "stream_id" : String,
        "track" : Object
    }
}
```



Building
========


Building The Server
-------------------

To build you will need a spotify application key which is not included in the
repository. Register a new application on the spotify developer site and place
it in `source/player/source_spotify_app_key.cpp`

```c
#include <stdint.h>
#include <stdlib.h>

extern "C" const uint8_t g_appkey[] = {
  0x01, 0xB1, 0xA2, 0x29, ...
};

extern "C" const size_t g_appkey_size = sizeof(g_appkey);

```

For the spotify web api you will need the client id and client secret in `source/spotify_web/id.h`

```c
#ifndef __spotify_web__id_h__
#define __spotify_web__id_h__

// ----------------------------------------------------------------------------
namespace spotify_web
{
  const std::string client_id     = "................................";
  const std::string client_secret = "................................";
}

// ----------------------------------------------------------------------------
#endif

```


Install needed dependencies

```sh
sudo apt install ninja-build libasound2-dev libflac++-dev libkyotocabinet-dev libssl-dev libcrypto++-dev
```

Get additional dependencies like libspotify and json library.

```sh
ninja deps
```

Build

```sh
ninja
```


Building The Web interface
--------------------------

The web interface is a statically build ruby/opal application using the maquette.js
virtual dom library. To build you will need ruby. Personally I use rbenv and ruby
2.3, but I expect any recently new ruby version will do.

To isntall

```sh
cd opal; bundle install
```

To build

```
rake
```

This will build html, javascript and stylesheets and install them to the public
folder.

There is also a guard file to automatically build the files when they are changed.

```
bundle exec guard
```


Contributing
------------

Any kind of feed back is welcome. Feel free to ask me qustions on gitter or open
an issue, be it a bug or an idea.
