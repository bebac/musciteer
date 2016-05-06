// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__channel_h__
#define __dripcore__channel_h__

// ----------------------------------------------------------------------------
#include <thread>
#include <condition_variable>
#include <queue>
#include <cassert>

// ----------------------------------------------------------------------------
#include "task.h"

// ----------------------------------------------------------------------------
namespace dripcore
{
  template<typename T>
  class thread_safe_channel_queue
  {
  public:
    thread_safe_channel_queue()
      //: waiting_receivers_(1, nullptr), waiting_senders_(1, nullptr)
      : waiting_receiver_(nullptr)
    {
    }
  public:
    //void send(const T& v)
    void send(T&& v)
    {
      std::lock_guard<std::mutex> _(m_);
      //std::cout << "send " << v << " this thread=" << std::this_thread::get_id() << std::endl;
      q_.push(std::move(v));

#if 0
      for ( auto& receiver : waiting_receivers_ )
      {
        if ( receiver )
        {
          //std::cout << "resume receiver this thread=" << std::this_thread::get_id() << std::endl;
          receiver->resume();
          receiver = nullptr;
        }
      }
#endif
      if ( waiting_receiver_ )
      {
        waiting_receiver_->resume();
        waiting_receiver_ = nullptr;
      }

      c_.notify_one();
    }
  public:
    //void send(const T& v, dripcore::task* task)
    void send(T&& v, dripcore::task* task)
    {
      std::lock_guard<std::mutex> _(m_);
      q_.push(std::move(v));

      if ( waiting_receiver_ )
      {
        waiting_receiver_->resume();
        waiting_receiver_ = nullptr;
      }

      c_.notify_one();
    }
  public:
    T recv()
    {
      std::unique_lock<std::mutex> _(m_);

      while ( q_.empty () ) {
        c_.wait(_);
      }

      auto v = std::move(q_.front());
      q_.pop();
      return v;
    }
  public:
    T recv(dripcore::task* task)
    {
      std::unique_lock<std::mutex> _(m_);

      if ( q_.empty() )
      {
  #if 0
        for ( auto& receiver : waiting_receivers_ )
        {
          if ( !receiver )
          {
            receiver = task;
            break;
          }
        }
  #endif
        waiting_receiver_ = task;
      }

      while ( q_.empty () )
      {
        _.unlock();
        task->yield();
        _.lock();
      }

      auto v = std::move(q_.front());
      q_.pop();
      return v;
    }
  private:
    // Track tasks waiting to send or receive.
    dripcore::task* waiting_receiver_;
    //std::vector<dripcore::task*> waiting_senders_;
    //std::vector<dripcore::task*> waiting_receivers_;
  private:
    std::mutex m_;
    std::condition_variable c_;
    std::queue<T> q_;
  };

  // ----------------------------------------------------------------------------
  template<typename T>
  class channel
  {
  public:
    channel() : q_(new thread_safe_channel_queue<T>())
    {
    }
  public:
    bool operator<(const channel<T>& rhs) const
    {
      return q_ < rhs.q_;
    }
  public:
    //void send(const T& v)
    void send(T&& v)
    {
      assert(q_);
      q_->send(std::move(v));
    }
    //void send(const T& v, dripcore::task* task)
    void send(T&& v, dripcore::task* task)
    {
      assert(q_);
      q_->send(std::move(v), task);
    }
  public:
    T recv()
    {
      assert(q_);
      return q_->recv();
    }
    T recv(dripcore::task* task)
    {
      assert(q_);
      return q_->recv(task);
    }
  private:
    std::shared_ptr<dripcore::thread_safe_channel_queue<T>> q_;
  };
}

// ----------------------------------------------------------------------------
#endif