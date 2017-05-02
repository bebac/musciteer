// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "migration.h"

// ----------------------------------------------------------------------------
#include "player/sources.h"
#include "player/player.h"

// ----------------------------------------------------------------------------
#include "storage/kvstore.h"

// ----------------------------------------------------------------------------
#include "http/http_connection.h"

// ----------------------------------------------------------------------------
#include <dripcore/loop.h>
#include <dripcore/task.h>
#include <dripcore/tcp_server.h>

// ----------------------------------------------------------------------------
#include <program_options.h>

// ----------------------------------------------------------------------------
#include <openssl/conf.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// ----------------------------------------------------------------------------
namespace musciteer
{
  class options : public program_options::container
  {
  public:
    options()
      :
      help(false), interface_ip("0.0.0.0"), interface_port(8214),
      database_filename(".musciteerdb")
    {
      add('h', "help", "display this message", help);
      add('i', "ip", "interface ip address to listen on", interface_ip);
      add('p', "port", "interface port to listen on", interface_port);
      add('f', "database", "database filename", database_filename);
    }
  public:
    bool        help;
    std::string interface_ip;
    unsigned    interface_port;
    std::string database_filename;
  };

  void print_usage(const options& options)
  {
    std::cout
      << "Usage: musciteer [OPTION...]" << std::endl
      << std::endl
      << "Available option:" << std::endl
      << options << std::endl
      << std::endl;
  }
}

// ----------------------------------------------------------------------------
void init_openssl_library(void)
{
  SSL_library_init();

  SSL_load_error_strings();

  /* ERR_load_crypto_strings(); */

  OPENSSL_config(NULL);

  /* Include <openssl/opensslconf.h> to get this define */
#if defined (OPENSSL_THREADS)
  std::cerr << "Warning: openssl thread locking is not implemented" << std::endl;
#endif
}


// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  musciteer::options options;

  try
  {
    options.parse(argc, argv);

    if ( options.help )
    {
      print_usage(options);
      exit(1);
    }
  }
  catch(const program_options::error& err)
  {
    std::cerr
      << err.what() << std::endl
      << "try --help for usage" << std::endl;
    exit(2);
  }

  dripcore::loop loop;

  init_openssl_library();

  musciteer::kvstore::start(options.database_filename);
  musciteer::migration::run(options.database_filename);
  musciteer::sources::start(&loop);
  musciteer::player::start(&loop);

  auto on_connection = [&](dripcore::socket socket) {
    loop.spawn<http_connection>(std::move(socket));
  };

  loop.spawn<dripcore::tcp_server>(options.interface_ip, options.interface_port, on_connection);
  loop.run();

  return 0;
}
