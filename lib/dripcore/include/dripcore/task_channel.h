// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// ----------------------------------------------------------------------------
#ifndef __dripcore__task_channel_h__
#define __dripcore__task_channel_h__

// ----------------------------------------------------------------------------
#include <dripcore/task.h>

// ----------------------------------------------------------------------------
#include <queue>
#include <cassert>

// ----------------------------------------------------------------------------
namespace dripcore
{
  template<typename T>
  class task_channel_queue
  {
  public:
    task_channel_queue(size_t size)
      : waiting_receiver_(nullptr), waiting_sender_(nullptr), size_(size)
    {
    }
  public:
    void send(T&& v, dripcore::task* task)
    {
      while ( q_.size() >= size_ )
      {
        waiting_sender_ = task;

        task->yield();

        if ( task->stopping() ) {
          return;
        }
      }

      q_.push(std::forward<T>(v));

      if ( waiting_receiver_ )
      {
        waiting_receiver_->resume();
        waiting_receiver_ = nullptr;
      }
    }
  public:
    T recv(dripcore::task* task)
    {
      while ( q_.empty () )
      {
        waiting_receiver_ = task;

        task->yield();

        // Hmm - Maybe need to throw an exception here
        if ( task->stopping() ) {
          return T{};
        }
      }

      auto v = std::move(q_.front());
      q_.pop();

      if ( waiting_sender_ )
      {
        waiting_sender_->resume();
        waiting_sender_ = nullptr;
      }

      return v;
    }
  private:
    dripcore::task* waiting_receiver_;
    dripcore::task* waiting_sender_;
  private:
    std::queue<T> q_;
    size_t size_;
  };

  template<typename T>
  class basic_task_channel
  {
  protected:
    using queue_ptr = std::shared_ptr<dripcore::task_channel_queue<T>>;
  protected:
    basic_task_channel(dripcore::task_channel_queue<T>* q) : q_(q)
    {
    }
  public:
    basic_task_channel(const basic_task_channel<T>& other) : q_(other.q_)
    {
    }
  protected:
    void send(T&& v, dripcore::task* task)
    {
      assert(q_);
      q_->send(std::forward<T>(v), task);
    }
  protected:
    T recv(dripcore::task* task)
    {
      assert(q_);
      return q_->recv(task);
    }
  protected:
    std::shared_ptr<dripcore::task_channel_queue<T>> q_;
  };

  template<typename T, size_t U>
  class task_channel : public basic_task_channel<T>
  {
  public:
    task_channel() : basic_task_channel<T>(new dripcore::task_channel_queue<T>(U))
    {
    }
  public:
    using basic_task_channel<T>::send;
    using basic_task_channel<T>::recv;
  };

  template<typename T>
  class task_ochannel : public basic_task_channel<T>
  {
  public:
    task_ochannel(const basic_task_channel<T>& channel)
      : basic_task_channel<T>(channel), task_(nullptr)
    {
    }
    task_ochannel(const basic_task_channel<T>& channel, task* task)
      : basic_task_channel<T>(channel), task_(task)
    {
    }
  public:
    void send(T&& v)
    {
      assert(task_);
      basic_task_channel<T>::send(std::forward<T>(v), task_);
    }
  private:
    task* task_;
  };

  template<typename T>
  class task_ichannel : public basic_task_channel<T>
  {
  public:
    task_ichannel(const basic_task_channel<T>& channel)
      : basic_task_channel<T>(channel), task_(nullptr)
    {
    }
    task_ichannel(const basic_task_channel<T>& channel, task* task)
      : basic_task_channel<T>(channel), task_(task)
    {
    }
  public:
    T recv()
    {
      assert(task_);
      return basic_task_channel<T>::recv(task_);
    }
  private:
    task* task_;
  };
}

// ----------------------------------------------------------------------------
#endif