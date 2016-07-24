// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#include "album_cover.h"

// ----------------------------------------------------------------------------
namespace musicbox
{
  namespace dm
  {
    album_cover::album_cover() : mime_type_(), data_()
    {
    }

    const std::string& album_cover::mime_type() const
    {
      return mime_type_;
    }

    const std::string& album_cover::data() const
    {
      return data_;
    }

    void album_cover::mime_type(const std::string& mime_type)
    {
      mime_type_ = mime_type;
    }

    void album_cover::data(const unsigned char* buf, size_t len)
    {
      data_.assign(reinterpret_cast<const char*>(buf), len);
    }

    void album_cover::read(msgpack::istream& is)
    {
      msgpack::map map;

      if ( is >> map )
      {
        for ( size_t i=0; i<map.size(); i++ )
        {
          unsigned key;

          if ( is >> key )
          {
            switch ( key )
            {
              case 1: is >> mime_type_; break;
              case 2: is >> data_; break;
              default:
                is >> msgpack::skip;
                break;
            }
          }
          else
          {
            // ERROR!
            std::cout << "failed to read album key" << std::endl;
          }
        }
      }
      else
      {
        // ERROR!
        std::cout << "failed to read album map" << std::endl;
      }
    }

    void album_cover::write(msgpack::ostream& os) const
    {
      msgpack::map map{2};

      os << map
        << 1 << mime_type_
        << 2 << data_;
    }
  }
}
