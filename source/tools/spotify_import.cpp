// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "../dm/track.h"
#include "../dm/tracks.h"
#include "../dm/artist.h"
#include "../dm/artists.h"
#include "../dm/album.h"
#include "../dm/albums.h"
#include "../dm/album_cover.h"

// ----------------------------------------------------------------------------
#include <dripcore/loop.h>
#include <dripcore/task.h>
#include <dripcore/channel.h>
#include <dripcore/socket.h>
#include <dripcore/streambuf.h>

// ----------------------------------------------------------------------------
#include <http/request.h>
#include <http/response.h>

// ----------------------------------------------------------------------------
#include <json.h>

// ----------------------------------------------------------------------------
#include <openssl/conf.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// ----------------------------------------------------------------------------
#include <regex>

// ----------------------------------------------------------------------------
#include <netdb.h>

// spotify_import is sort of a play ground to get started on interacting with
// the spotify web api. It will quickly get quite complicated as the spotify
// web api only support https and oauth for authentication. Luckily we don't
// need to authenticate to get started.

// ----------------------------------------------------------------------------
namespace dripcore
{
  class ssl_socket : public dripcore::io
  {
  public:
    ssl_socket(dripcore::socket&& socket) : socket_(std::move(socket))
    {
      ctx_ = SSL_CTX_new(SSLv23_method());

      if ( !ctx_ ) {
        throw std::runtime_error("ssl context error");
      }

      ssl_ = SSL_new(ctx_);

      if ( !ssl_ ) {
        throw std::runtime_error("ssl error");
      }

      SSL_set_fd(ssl_, socket_.native_handle());
    }
  public:
    ~ssl_socket()
    {
      SSL_free(ssl_);
      SSL_CTX_free(ctx_);
    }
  public:
    void connect(dripcore::task* task)
    {
      int res;
      do
      {
        res = SSL_connect(ssl_);

        switch (SSL_get_error(ssl_, res))
        {
          case SSL_ERROR_NONE:
            break;
          case SSL_ERROR_WANT_READ:
            task->wait_readable(socket_);
            break;
          case SSL_ERROR_WANT_WRITE:
            task->wait_writable(socket_);
            break;
          default:
            std::cerr << ERR_error_string(SSL_get_error(ssl_, res), 0) << std::endl;
            throw std::runtime_error("ssl connect error");
        }
      }
      while ( res < 0 );
    }
  public:
    virtual ssize_t recv(void *buf, size_t len, int flags=0) override
    {
      return SSL_read(ssl_, buf, len);
    }
  public:
    virtual ssize_t send(const void *buf, size_t len, int flags=0) override
    {
      return SSL_write(ssl_, buf, len);
    }
  public:
    virtual int native_handle() const override
    {
      return socket_.native_handle();
    }
  private:
    dripcore::socket socket_;
    SSL_CTX* ctx_;
    SSL* ssl_;
  };
}

// ----------------------------------------------------------------------------
namespace http
{
  class client
  {
  public:
    client(dripcore::task* task)
      : task_(task), url_re_("^(http|https)://([^:/]*)(:?([^/]+)?(.*))")
    {
    }
  public:
    void get(const std::string url, std::function<void(http::response&)> cb)
    {
      auto scheme = std::string{};
      auto hostname = std::string{};
      auto port_s = std::string{};
      auto port = 0;
      auto path = std::string{};

      std::smatch match;

      if ( std::regex_match(url, match, url_re_) )
      {
        scheme = match[1];
        hostname = match[2];
        port_s = match[4];
        path = match[5];

        if ( scheme == "http" && port_s.empty() )
        {
          port = 80;
        }
        else if ( scheme == "https" && port_s.empty() )
        {
          port = 443;
        }
        else
        {
          port = atoi(port_s.c_str());
        }
      }
      else
      {
        throw std::runtime_error("invalid url");
      }

      auto socket = dripcore::ipv4::tcp::socket();

      init_sock_addr(hostname, port);
      connect(socket);

      if ( scheme == "https" )
      {
        dripcore::ssl_socket ssl_socket(std::move(socket));

        // Create streambuf before connecting as streambuf does attach_eventable.
        dripcore::streambuf sbuf(ssl_socket, *task_);

        // Perform ssl handshake.
        ssl_socket.connect(task_);

        http::request request(&sbuf);

        request
          << "GET " << path << " HTTP/1.1\r\n"
          << "Host: " << hostname << "\r\n"
          << "\r\n"
          << std::flush;

        http::response response(&sbuf);

        response >> response;

        cb(response);
      }
      else
      {
        dripcore::streambuf sbuf(socket, *task_);

        http::request request(&sbuf);

        request
          << "GET " << path << " HTTP/1.1\r\n"
          << "Host: " << hostname << "\r\n"
          << "\r\n"
          << std::flush;

        http::response response(&sbuf);

        response >> response;

        cb(response);
      }
    }
  private:
    void connect(dripcore::socket& socket)
    {
      socket.nonblocking(true);
      socket.connect((struct sockaddr *)&sock_addr_, sizeof(sock_addr_));

      task_->attach_eventable(socket);
      task_->wait_writable(socket);
      task_->detach_eventable(socket);
    }
  private:
    void init_sock_addr(const std::string& hostname, unsigned port)
    {
      gaicb aicb[1];
      gaicb* pai = &aicb[0];

      pai->ar_name = hostname.c_str();
      pai->ar_service = 0;
      pai->ar_request = 0;
      pai->ar_result = 0;

      sigevent ev;

      ev.sigev_notify = SIGEV_THREAD;
      ev.sigev_value.sival_ptr = this;
      ev.sigev_notify_attributes = nullptr;
      ev.sigev_notify_function = [](sigval value) {
        client* self = reinterpret_cast<client*>(value.sival_ptr);
        self->task_->resume();
      };

      int res;

      res = getaddrinfo_a(GAI_NOWAIT, &pai, 1, &ev);

      if ( res ) {
        throw std::runtime_error(gai_strerror(res));
      }

      task_->yield();

      res = gai_error(pai);

      if ( res ) {
        throw std::runtime_error(gai_strerror(res));
      }

      if ( sizeof(sock_addr_) != pai->ar_result->ai_addrlen ) {
        throw std::runtime_error("wrong sock_addr size");
      }

      std::memcpy(&sock_addr_, pai->ar_result->ai_addr, pai->ar_result->ai_addrlen);

      sock_addr_.sin_family = AF_INET;
      sock_addr_.sin_port = htons(port);

      freeaddrinfo(pai->ar_result);
    }
  private:
    dripcore::task* task_;
  private:
    struct sockaddr_in sock_addr_;
    std::regex url_re_;
  };
}

// ----------------------------------------------------------------------------
void print_track(const musciteer::dm::track& track)
{
  std::cout << "---" << std::endl;
  std::cout << "track id       : " << track.id() << std::endl;
  std::cout << "track title    : " << track.title() << std::endl;
  std::cout << "track duration : " << track.duration() << std::endl;
  std::cout << "track artists  : " << track.artists().size() << std::endl;
  for ( auto artist : track.artists() )
  {
    std::cout << "  artist name : " << artist.name() << std::endl;
  }
  std::cout << "track album    : " << track.album().title() << std::endl;
  std::cout << "track sources  : " << track.sources().size() << std::endl;
  for ( auto source : track.sources() )
  {
    std::cout << "  source name : " << source.name() << std::endl;
    std::cout << "  source uri  : " << source.uri() << std::endl;
  }
}

// ----------------------------------------------------------------------------
class spotify_importer : public dripcore::task
{
public:
  spotify_importer(std::vector<std::string> urls) : http_client_(this), urls_(urls)
  {
  }
private:
  void main() override
  {
    for ( const auto& url : urls_ )
    {
      auto j = http_get_json(url);

      if ( j["type"] == "track" )
      {
        import_track(j);
      }
      else if ( j["type"] == "album" )
      {
        auto items = j["tracks"]["items"];

        for ( auto item : items )
        {
          std::string track_url = item["href"];

          import_track(track_url);
        }
      }
      else
      {
        throw std::runtime_error("don't know how to import that");
      }
    }
  }
private:
  void import_track(const std::string& url)
  {
    auto track_json = http_get_json(url);

    if ( !track_json.is_object() ) {
      throw std::runtime_error("track json not an object");
    }

    import_track(track_json);
  }
private:
  void import_track(json& track_json)
  {
    if ( !track_json.is_object() ) {
      throw std::runtime_error("track json not an object");
    }

    //std::cout << "track title=" << track_json["name"] << std::endl;
    //std::cout << track_json << std::endl;

    auto album_json = track_json["album"];
    auto artists_json = track_json["artists"];
    auto album_artist = resolve_album_artist(album_json);

    if ( album_artist.id_is_null() ) {
      throw std::runtime_error("album artist id is empty");
    }

    //std::cout << album_json << std::endl;
    //std::cout << "album_artist id='" << album_artist.id() << "', name='" << album_artist.name() << "'" << std::endl;

    auto album = resolve_album(album_artist, album_json);

    //std::cout << "album id='" << album.id() << "', title='" << album.title() << "'" << std::endl;

    if ( album.id_is_null() ) {
      throw std::runtime_error("album id is empty");
    }

    auto track = musciteer::dm::track{};
    auto track_source = musciteer::dm::track_source{"spotify", track_json["uri"]};

    std::string track_title = track_json["name"];
    unsigned track_tn = track_json["track_number"];
    unsigned track_dn = track_json["disc_number"];
    unsigned track_duration = track_json["duration_ms"];

    album.tracks_each([&](const musciteer::dm::track& rec)
    {
      if ( rec.disc_number() == track_dn && rec.track_number() == track_tn )
      {
        track = rec;
        return false;
      }
      else
      {
        return true;
      }
    });

    if ( track.id_is_null() )
    {
      track = tracks_.create_track();

      track.title(track_title);
      track.track_number(track_tn);
      track.disc_number(track_dn);
      track.duration(track_duration);
      track.album(album);

      for ( auto artist_json : artists_json )
      {
        auto track_artist = musciteer::dm::artist{};

        std::string track_artist_name = artist_json["name"];

        artists_.each([&](const musciteer::dm::artist& rec)
        {
          if ( rec.name() == track_artist_name )
          {
            track_artist = rec;
            return false;
          }
          else
          {
            return true;
          }
        });

        if ( track_artist.id_is_null() )
        {
          //std::cout << "create artist (track) name=" << track_artist_name << std::endl;
          track_artist = artists_.create_artist(track_artist_name);
          artists_.save(track_artist);
        }

        track.artists_add(track_artist);
      }

      album.tracks_add(track);
    }
    else
    {
      std::cout << "track found" << std::endl;
    }

    track.sources_add(track_source);

    print_track(track);

    artists_.save(album_artist);
    albums_.save(album);
    tracks_.save(track);
  }
private:
  musciteer::dm::artist resolve_album_artist(const json& album_json)
  {
    auto album_artist = musciteer::dm::artist{};

    if ( !album_json.is_object() ) {
      throw std::runtime_error("album json not an object");
    }

    auto details_json = http_get_json(album_json["href"]);

    if ( !details_json["artists"].is_array() ) {
      throw std::runtime_error("album artists json not an array");
    }

    auto artist_json = details_json["artists"][0];

    if ( !artist_json.is_object() ) {
      throw std::runtime_error("artist json not an object");
    }

    std::string artist_name = artist_json["name"];

    artists_.each([&](const musciteer::dm::artist& rec)
    {
      if ( rec.name() == artist_name )
      {
        album_artist = rec;
        return false;
      }
      else
      {
        return true;
      }
    });

    if ( album_artist.id_is_null() )
    {
      //std::cout << "create artist (album) name=" << artist_name << std::endl;
      album_artist = artists_.create_artist(artist_name);
      artists_.save(album_artist);
    }

    return album_artist;
  }
private:
  musciteer::dm::album resolve_album(musciteer::dm::artist& artist, const json& album_json)
  {
    auto album = musciteer::dm::album{};

    std::string album_title = album_json["name"];
    std::string album_alt_id = album_json["uri"];

    artist.albums_each([&](const musciteer::dm::album& rec)
    {
      if ( rec.title() == album_title )
      {
        album = rec;
        return false;
      }
      else
      {
        return true;
      }
    });

    if ( album.id_is_null() )
    {
      album = albums_.create_album();

      album.title(album_title);
      album.artist(artist);
      album.alt_ids_add(album_alt_id);

      auto images_json = album_json["images"];

      if ( !images_json.is_array() ) {
        throw std::runtime_error("album images json not an array");
      }

      auto album_cover = http_get_image(images_json[0]["url"]);
      auto kvstore = musciteer::kvstore();

      kvstore.set(album.id()+"/cover", album_cover);

      artist.albums_add(album);
    }

    return album;
  }
private:
  json http_get_json(const std::string& url)
  {
    json j;

    http_client_.get(url, [&](http::response& response)
    {
      std::string content_type;
      std::string content_length_s;
      std::string content;
      std::string accept = "application/json";

      if ( !response.get_header("content-type", content_type) ) {
        throw std::runtime_error("no content-type header");
      }

      if ( content_type.compare(0, accept.size(), accept) != 0 ) {
        throw std::runtime_error("wrong content type");
      }

      if ( !response.get_header("content-length", content_length_s) ) {
        throw std::runtime_error("no content-length header");
      }

      auto pos = std::size_t{0};
      auto len = std::stoul(content_length_s, &pos);
      auto buf = response.rdbuf();

      for ( size_t i=0; i<len; ++i) {
        content += buf->sbumpc();
      }

      j = json::parse(content);
    });

    return j;
  }
private:
  musciteer::dm::album_cover http_get_image(const std::string& url)
  {
    auto cover = musciteer::dm::album_cover();

    http_client_.get(url, [&](http::response& response)
    {
      std::string content_type;
      std::string content_length_s;
      std::string content;

      if ( !response.get_header("content-type", content_type) ) {
        throw std::runtime_error("no content-type header");
      }

      if ( !response.get_header("content-length", content_length_s) ) {
        throw std::runtime_error("no content-length header");
      }

      auto pos = std::size_t{0};
      auto len = std::stoul(content_length_s, &pos);
      auto buf = response.rdbuf();

      for ( size_t i=0; i<len; ++i) {
        content += buf->sbumpc();
      }

      cover.mime_type(content_type);
      cover.data(reinterpret_cast<const unsigned char*>(content.data()), content.length());
    });

    return cover;
  }
private:
  http::client http_client_;
private:
  std::vector<std::string> urls_;
private:
  musciteer::dm::artists artists_;
  musciteer::dm::albums albums_;
  musciteer::dm::tracks tracks_;
};

// ----------------------------------------------------------------------------
void init_openssl_library(void)
{
  SSL_library_init();

  SSL_load_error_strings();

  /* ERR_load_crypto_strings(); */

  OPENSSL_config(NULL);

  /* Include <openssl/opensslconf.h> to get this define */
#if defined (OPENSSL_THREADS)
  fprintf(stdout, "Warning: thread locking is not implemented\n");
#endif
}

// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  dripcore::loop loop;

  init_openssl_library();

  musciteer::kvstore::start(".mboxd");

  // Import spotify track or album.
  //
  // Examples:
  // spotify_import /v1/albums/51ogMPThFURWcqZGxVA1uT
  // spotify_import /v1/tracks/6dben05JiKn09x31h1JgOW
  // spotify_import -f urls.txt

  std::vector<std::string> urls;

  if ( std::string(argv[1]) == "-f" )
  {
    std::ifstream file(argv[2]);

    while ( !file.eof() )
    {
      std::string path;
      std::getline(file, path);
      urls.push_back("https://api.spotify.com" + path);
    }
  }
  else
  {
    urls.push_back("https://api.spotify.com" + std::string(argv[1]));
  }

  loop.spawn<spotify_importer>(std::move(urls));
  loop.run();

  return 0;
}
