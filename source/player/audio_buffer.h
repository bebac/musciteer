// ----------------------------------------------------------------------------
//
//     Author     : Benny Bach <benny.bach@gmail.com>
//                  Copyright (C) 2016
//
// ----------------------------------------------------------------------------
#ifndef __player__audio_buffer_h__
#define __player__audio_buffer_h__

// ----------------------------------------------------------------------------
#include <memory>

// ----------------------------------------------------------------------------
class audio_buffer
{
  // Interleaved signed 16 bit frame
  struct s16_le_i_frame
  {
    int16_t l;
    int16_t r;
  };

  // Interleaved signed 32 bit frame
  struct s32_le_frame
  {
    int32_t l;
    int32_t r;
  };

  // FLAC delivers samples non interleaved in 32 bit. Only handle 2 channels.
  struct non_interleved_32
  {
    int32_t* l;
    int32_t* r;
  };

public:
  audio_buffer()
    : capacity_(0), size_(0), data_()
  {
  }
  audio_buffer(size_t capacity)
    : capacity_(capacity), size_(0), data_(new s32_le_frame[capacity])
  {
  }
public:
  audio_buffer(audio_buffer&& other)
  {
    *this = std::move(other);
  }
public:
  audio_buffer& operator= (audio_buffer&& rhs)
  {
    data_ = std::move(rhs.data_);
    capacity_ = std::move(rhs.capacity_);
    size_ = std::move(rhs.size_);
    return *this;
  }
public:
  const s32_le_frame& operator[] (size_t i)
  {
    if ( i < size_ )
    {
      return data_[i];
    }
    else
    {
      throw std::runtime_error("out of range");
    }
  }
public:
  size_t size() const
  {
    return size_;
  }
public:
  const s32_le_frame* data() const
  {
    return data_.get();
  }
public:
  void clear()
  {
    size_ = 0;
  }
public:
  void write_s16_le_i(const void* frames, size_t num_frames)
  {
    auto ibuf = reinterpret_cast<const s16_le_i_frame*>(frames);

    if ( size_ + num_frames > capacity_ ) {
      //throw std::runtime_error("not enough space in audio_buffer");
      data_.reset(new s32_le_frame[num_frames]);
      capacity_ = num_frames;
    }

    for( size_t i = size_; i < num_frames; i++ )
    {
      data_[i].l = static_cast<int32_t>(ibuf[i].l) * (1<<16);
      data_[i].r = static_cast<int32_t>(ibuf[i].r) * (1<<16);
    }
    size_ += num_frames;
  }
public:
  void writen(const void* frames, size_t num_frames, size_t bits_per_sample)
  {
    auto ibuf = reinterpret_cast<const non_interleved_32*>(frames);
    auto scale = 32 - bits_per_sample;

    if ( size_ + num_frames > capacity_ ) {
      //throw std::runtime_error("not enough space in audio_buffer");
      data_.reset(new s32_le_frame[num_frames]);
      capacity_ = num_frames;
    }

    for( size_t i = size_; i < num_frames; ++i )
    {
      data_[i].l = static_cast<int32_t>(ibuf->l[i]) * (1<<scale);
      data_[i].r = static_cast<int32_t>(ibuf->r[i]) * (1<<scale);
    }
    size_ += num_frames;
  }
public:
  void scale(double factor)
  {
    for( size_t i = 0; i < size_; ++i )
    {
      data_[i].l = static_cast<int32_t>(data_[i].l*factor);
      data_[i].r = static_cast<int32_t>(data_[i].r*factor);
    }
  }
private:
  audio_buffer(const audio_buffer&) = delete;
  audio_buffer& operator= (const audio_buffer&) = delete;
private:
  size_t capacity_;
  size_t size_;
private:
  std::unique_ptr<s32_le_frame[]> data_;
};

// ----------------------------------------------------------------------------
#endif