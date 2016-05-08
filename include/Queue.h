/*
 * Queue.h
 *
 *  Created on: 18 марта 2016 г.
 *      Author: Bogdan
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <queue>

#include "ch.hpp"

template <typename T, int N>
class Queue
{
 public:

  T pop()
  {
    mutex_.lock();

    while (queue_.empty())
    {
      cond_.wait();
    }
    auto item = queue_.front();
    queue_.pop();

    mutex_.unlock();
    return item;
  }

  void pop(T& item)
  {
	  mutex_.lock();

    while (queue_.empty())
    {
      cond_.wait();
    }
    item = queue_.front();
    queue_.pop();

    mutex_.unlock();
  }

  void push(const T& item)
  {
	  mutex_.lock();
    queue_.push(item);
    mutex_.unlock();
    cond_.signal();
  }

  void push(T&& item)
  {
	  mutex_.lock();
    queue_.push(std::move(item));
    mutex_.unlock();
    cond_.signal();
  }

 private:
  std::queue<T> queue_;
  chibios_rt::Mutex mutex_;
  chibios_rt::CondVar cond_;
};
#endif /* QUEUE_H_ */
