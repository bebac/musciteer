// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __musicbox__file_system_h__
#define __musicbox__file_system_h__

// ----------------------------------------------------------------------------
#include <chrono>
#include <exception>
#include <system_error>

// ----------------------------------------------------------------------------
#include <sys/stat.h>
#include <dirent.h>

// ----------------------------------------------------------------------------
namespace file_system
{
  class error : public std::system_error
  {
  public:
    error(int ec) : std::system_error(ec, std::system_category())
    {
    }
  };

  class status
  {
    using system_clock = std::chrono::system_clock;
  public:
    status(const std::string& filename)
    {
      if ( ::stat(filename.c_str(), &stat_) < 0 ) {
        err_ = std::make_exception_ptr<error>(errno);
      }
    }
  public:
    operator bool() const
    {
      return err_ ? false : true;
    }
  public:
    bool is_file()
    {
      check_error();
      return (stat_.st_mode & S_IFMT) == S_IFREG;
    }
  public:
    bool is_directory()
    {
      check_error();
      return (stat_.st_mode & S_IFMT) == S_IFDIR;
    }
  public:
    bool is_block_device()
    {
      check_error();
      return (stat_.st_mode & S_IFMT) == S_IFBLK;
    }
  public:
    system_clock::time_point mtime()
    {
      check_error();
      return system_clock::from_time_t(stat_.st_mtime);
    }
  private:
    void check_error()
    {
      if ( err_ ) {
        std::rethrow_exception(err_);
      }
    }
  private:
    std::exception_ptr err_;
    struct stat        stat_;
  };

  class directory
  {
  public:
    directory(const std::string& dirname)
      :
      dirname_(dirname),
      err_(nullptr),
      dir_(nullptr)
    {
      if ((dir_ = opendir(dirname.c_str())) == NULL) {
        err_ = std::make_exception_ptr<error>(errno);
      }
    }
  public:
    ~directory()
    {
      if ( dir_ ) {
        closedir(dir_);
      }
    }
  public:
    operator bool() const
    {
      return err_ ? false : true;
    }
  public:
    void rewind()
    {
      check_error();
      rewinddir(dir_);
    }
  public:
    void each_file(std::function<void(const std::string& filename)> callback)
    {
      check_error();

      struct dirent *dp;

      while ((dp = readdir(dir_)) != NULL)
      {
        auto entry = std::string(dp->d_name);

        if ( ! (entry == "." || entry == "..") )
        {
          auto path = dirname_+"/"+entry;
          auto stat = status{path};

          if ( stat.is_directory() )
          {
            auto dir = directory{path};
            dir.each_file(callback);
          }
          else if ( stat.is_file() ) {
            callback(path);
          }
          else {
            // Ignore.
          }
        }
      }
    }
  private:
    void check_error()
    {
      if ( err_ ) {
        std::rethrow_exception(err_);
      }
    }
  private:
    std::string        dirname_;
    std::exception_ptr err_;
    DIR*               dir_;
  };

  inline std::string extension(const std::string& filename)
  {
    size_t pos = filename.rfind('.');

    if ( pos != std::string::npos ) {
      return filename.substr(pos+1, std::string::npos);
    }
    else {
      return std::string();
    }
  }
} // namespace file_system

// ----------------------------------------------------------------------------
#endif
