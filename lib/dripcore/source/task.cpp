// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2015
//
// --- Description: -----------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
#include <dripcore/task.h>

// ----------------------------------------------------------------------------
namespace dripcore
{
  void task::start(loop* loop)
  {
    loop_ = loop;

    attach_eventable(rdy_);

    init();
    // Setup event descriptor to wait for ready.
    wait_ready();
    // Signal ready.
    resume();
  }

  void task::resume_()
  {
    switch ( state_ )
    {
      case task_state::init:
      {
        getcontext(&callee_);

        callee_.uc_link = &caller_;
        callee_.uc_stack.ss_size = stack_.size();
        callee_.uc_stack.ss_sp = stack_.ptr();

        function_arguments args;

        args.ptr = this;
        makecontext(&callee_, reinterpret_cast<void (*)()>(&call), 2, args.u32[0], args.u32[1]);

        state_ = task_state::running;
      }
      case task_state::running:
      case task_state::stopping:
        swapcontext(&caller_, &callee_);
        break;
      case task_state::done:
        break;
    }

    if ( state_ == task_state::done ) {
      loop_->del(this);
    }
  }

  void task::yield(bool resume)
  {
    if ( resume )
    {
      rdy_.notify();
    }
    swapcontext(&callee_, &caller_);
  }

  void task::wait_readable(int fd)
  {
    // Setup event descriptor.
    ed_.fd = fd;
    ed_.rd = [&]() { resume_(); };
    ed_.wr = nullptr;

    // Activate event.
    loop_->mod(&ed_);

    // Switch back to the caller.
    swapcontext(&callee_, &caller_);
  }

  void task::wait_writable(int fd)
  {
    // Setup event descriptor.
    ed_.fd = fd;
    ed_.rd = nullptr;
    ed_.wr = [&]() { resume_(); };

    // Activate event.
    loop_->mod(&ed_);

    // Switch back to the caller.
    swapcontext(&callee_, &caller_);
  }

  void task::wait_ready()
  {
    // Setup event descriptor.
    ed_.fd = rdy_.native_handle();
    ed_.rd = [&]() {
      if ( rdy_.read() > 0 ) {
        resume_();
      }
    };

    // Activate event.
    loop_->mod(&ed_);
  }

  void task::call(int arg0, int arg1)
  {
    function_arguments args;

    args.u32[0] = arg0;
    args.u32[1] = arg1;

    task* self = reinterpret_cast<task *>(args.ptr);

    self->main();
    self->state_ = task_state::done;
  }
}
