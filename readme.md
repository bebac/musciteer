Musciteer
=========

Musciteer is music server for Linux. It combines your local music files with 
selected music from streaming services and presents it as one library through 
a modern web interface

> NOTE: Currently it only supports flac and spotify



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

Install needed dependencies

```sh
sudo apt install ninja-build libasound2-dev libflac++-dev libkyotocabinet-dev libcrypto++-dev
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

The web interface is a statically build ruby/opal application using the Inesita 
framework. To build you will need ruby. Personally I use rbenv and ruby 2.3, but
I expect any recently new ruby version will do.

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
