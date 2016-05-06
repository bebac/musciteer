// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include <dripcore/loop.h>
#include <dripcore/task.h>

namespace dripcore
{
  loop::loop()
  {
    thread_id_ = std::this_thread::get_id();
    if ( (epoll_fd_ = epoll_create1(EPOLL_CLOEXEC)) < 0 ) {
      throw std::system_error(errno, std::system_category());
    }
  }

  void loop::add(int fd)
  {
    struct epoll_event ev;

    ev.events   = EPOLLET;
    ev.data.ptr = nullptr;

    if ( epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1 ) {
      throw std::system_error(errno, std::system_category());
    }
  }

  void loop::mod(event_descriptor* ev_desc)
  {
    if ( ev_desc->fd < 0 ) {
      std::cout << "epool_ctl invalid file descriptor fd=" << ev_desc->fd << std::endl;
      throw std::runtime_error("invalid file descriptor");
    }

    struct epoll_event ev;

    ev.events  = EPOLLET;

    if ( ev_desc->rd ) {
      ev.events |= EPOLLIN;
    }

    if ( ev_desc->wr ) {
      ev.events |= EPOLLOUT;
    }

    ev.data.ptr = ev_desc;

    if ( epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, ev_desc->fd, &ev) == -1 ) {
      throw std::system_error(errno, std::system_category());
    }
  }

  void loop::del(int fd)
  {
    if ( epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, 0) == -1 ) {
      throw std::system_error(errno, std::system_category());
    }
  }

  void loop::del(dripcore::task* task)
  {
    tasks_.erase(std::remove_if(tasks_.begin(), tasks_.end(),
      [&](const std::shared_ptr<dripcore::task>& x) {
        return x.get() == task;
      }),
      tasks_.end()
    );
  }

  void loop::run()
  {
    struct epoll_event ev;

    running_ = true;

    while ( tasks_.size() > 0 )
    {
      int nfds = epoll_wait(epoll_fd_, &ev, 1, 1000);

      if ( nfds > 0 )
      {
        auto evd = reinterpret_cast<event_descriptor *>(ev.data.ptr);

        if ( (ev.events & EPOLLIN) == EPOLLIN && evd->rd ) {
          evd->rd();
        }

        if ( (ev.events & EPOLLOUT) == EPOLLOUT && evd->wr ) {
          evd->wr();
        }
      }
      else if ( nfds < 0 && errno != EINTR ) {
        throw std::system_error(errno, std::system_category());
      }
    }
  }
}