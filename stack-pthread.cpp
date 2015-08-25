#include <exception>
#include <memory>
#include <mutex>
#include <stack>

struct empty_stack: std::exception
{
  const char* what() const throw();
};

template<typename T>
class threadsafe_stack
{
private:
  std::stack<T> data_;
  mutable std::mutex m_;

public:
  threadsafe_stack() {}
  threadsafe_stack(const threadsafe_stack& other)
  {
    std::lock_guard<std::mutex> lock(other.m_);
    data_ = other.data_;
  }
  threadsafe_stack& operator=(const threadsafe_stack&) = delete;

  void push(T new_value)
  {
    std::lock_guard<std::mutex> lock(m_);
    data_.push(new_value);
  }
  std::shared_ptr<T> pop()
  {
    std::lock_guard<std::mutex> lock(m_);
    if (data_.empty()) {
      throw empty_stack();
    }
    std::shared_ptr<T> const res(std::make_shared<T>(data_.top()));
    data_.pop();
    return res;
  }
  void pop(T& value)
  {
    std::lock_guard<std::mutex> lock(m_);
    if (data_.empty()) {
      throw empty_stack();
    }
    value = data_.top();
    data_.pop();
  }
  bool empty() const
  {
    std::lock_guard<std::mutex> lock(m_);
    return data_.empty();
  }
};
