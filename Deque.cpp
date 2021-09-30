#pragma once
#include <initializer_list>
#include <memory>

class Deque {
public:
    Deque() {
        data_ = std::make_unique<std::unique_ptr<int[]>[]>(1);
        data_[0] = std::make_unique<int[]>(block_length_);
        empty_ = true;
    }

    Deque(const Deque& rhs) {
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        size_ = rhs.size_;
        empty_ = rhs.empty_;
        data_ = std::make_unique<std::unique_ptr<int[]>[]>(rhs.size_);
        for (size_t i = 0; i < rhs.size_; ++i) {
            data_[i] = std::make_unique<int[]>(block_length_);
        }
        for (size_t i = 0; i < rhs.Size(); ++i) {
            (*this)[i] = rhs[i];
        }
    }

    Deque(Deque&& rhs) {
        std::swap(data_, rhs.data_);
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        size_ = rhs.size_;
        empty_ = rhs.empty_;
    }

    explicit Deque(size_t size) {
        if (size == 0) {
            empty_ = true;
        } else {
            empty_ = false;
        }
        size_ = size / block_length_;
        if (size % block_length_ != 0) {
            ++size_;
        }
        data_ = std::make_unique<std::unique_ptr<int[]>[]>(size_);
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = std::make_unique<int[]>(block_length_);
        }
        begin_ = 0;
        end_ = size;
        for (size_t i = 0; i < size; ++i) {
            data_[i / block_length_][i % block_length_] = 0;
        }
    }

    Deque(std::initializer_list<int> list) {
        Deque new_deque(list.size());
        auto it = list.begin();
        for (size_t i = 0; i < list.size(); ++i, ++it) {
            new_deque[i] = *it;
        }
        Swap(new_deque);
    }

    Deque& operator=(Deque rhs) {
        std::swap(data_, rhs.data_);
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        size_ = rhs.size_;
        empty_ = rhs.empty_;
        return *this;
    }

    void Swap(Deque& rhs) {
        if (data_ != rhs.data_) {
            std::swap(data_, rhs.data_);
            std::swap(begin_, rhs.begin_);
            std::swap(end_, rhs.end_);
            std::swap(size_, rhs.size_);
            std::swap(empty_, rhs.empty_);
        }
    }

    void PushBack(int value) {
        if (end_ % block_length_ != 0 || empty_) {
            empty_ = false;
            data_[end_ / block_length_][end_ % block_length_] = value;
            ++end_;
        } else {
            empty_ = false;
            if (end_ / block_length_ == size_) {
                if (begin_ / block_length_ == 0) {
                    auto new_data = std::make_unique<std::unique_ptr<int[]>[]>(size_ + 1);
                    for (size_t i = 0; i < size_; ++i) {
                        new_data[i].swap(data_[i]);
                    }
                    new_data[size_] = std::make_unique<int[]>(block_length_);
                    std::swap(data_, new_data);
                    data_[size_][0] = value;
                    ++end_;
                    ++size_;
                } else {
                    data_[0][0] = value;
                    end_ = 1;
                }
            } else {
                if (end_ / block_length_ == begin_ / block_length_) {
                    auto new_data = std::make_unique<std::unique_ptr<int[]>[]>(size_ + 1);
                    for (size_t i = 0; i < end_ / block_length_; ++i) {
                        new_data[i].swap(data_[i]);
                    }
                    new_data[end_ / block_length_] = std::make_unique<int[]>(block_length_);
                    for (size_t i = end_ / block_length_; i < size_; ++i) {
                        new_data[i + 1].swap(data_[i]);
                    }
                    std::swap(data_, new_data);
                    data_[end_ / block_length_][end_ % block_length_] = value;
                    ++end_;
                    ++size_;
                    begin_ += block_length_;
                } else {
                    data_[end_ / block_length_][end_ % block_length_] = value;
                    ++end_;
                }
            }
        }
    }

    void PopBack() {
        if (Size() == 0) {
            return;
        } else if (end_ == 1 && begin_ != 0) {
            end_ = size_ * block_length_;
        } else {
            --end_;
        }
        if (begin_ == end_) {
            empty_ = true;
        }
    }

    void PushFront(int value) {
        empty_ = false;
        if (begin_ % block_length_ != 0) {
            --begin_;
            data_[begin_ / block_length_][begin_ % block_length_] = value;
        } else {
            if (begin_ == 0) {
                if (end_ / block_length_ == size_ || end_ / block_length_ == size_ - 1) {
                    auto new_data = std::make_unique<std::unique_ptr<int[]>[]>(size_ + 1);
                    for (size_t i = 0; i < size_; ++i) {
                        new_data[i + 1].swap(data_[i]);
                    }
                    new_data[0] = std::make_unique<int[]>(block_length_);
                    std::swap(data_, new_data);
                    data_[0][block_length_ - 1] = value;
                    begin_ += block_length_;
                    end_ += block_length_;
                    --begin_;
                    ++size_;
                } else {
                    data_[size_ - 1][block_length_ - 1] = value;
                    begin_ = size_ * block_length_ - 1;
                }
            } else {
                if (end_ == begin_ || end_ / block_length_ == begin_ / block_length_ - 1) {
                    auto new_data = std::make_unique<std::unique_ptr<int[]>[]>(size_ + 1);
                    for (size_t i = 0; i < begin_ / block_length_; ++i) {
                        new_data[i].swap(data_[i]);
                    }
                    new_data[begin_ / block_length_] = std::make_unique<int[]>(block_length_);
                    for (size_t i = begin_ / block_length_; i < size_; ++i) {
                        new_data[i + 1].swap(data_[i]);
                    }
                    std::swap(data_, new_data);
                    begin_ += block_length_;
                    data_[begin_ / block_length_ - 1][block_length_ - 1] = value;
                    --begin_;
                    ++size_;
                } else {
                    --begin_;
                    data_[begin_ / block_length_][begin_ % block_length_] = value;
                }
            }
        }
    }

    void PopFront() {
        begin_ = (begin_ + 1) % (size_ * block_length_);
        if (begin_ == end_ || (begin_ == 0 && end_ == (size_ * block_length_))) {
            empty_ = true;
        }
    }

    int& operator[](size_t ind) {
        size_t true_index = (begin_ + ind) % (size_ * block_length_);
        return data_[true_index / block_length_][true_index % block_length_];
    }

    int operator[](size_t ind) const {
        size_t true_index = (begin_ + ind) % (size_ * block_length_);
        return data_[true_index / block_length_][true_index % block_length_];
    }

    size_t Size() const {
        if (begin_ < end_) {
            return end_ - begin_;
        } else if (empty_) {
            return 0;
        } else if (begin_ == end_) {
            return size_ * block_length_;
        } else {
            return size_ * block_length_ - begin_ + end_;
        }
    }

    void Clear() {
        *this = Deque();
    }

private:
    std::unique_ptr<std::unique_ptr<int[]>[]> data_;
    size_t begin_ = 0;
    size_t end_ = 0;
    size_t size_ = 1;
    bool empty_ = true;
    const size_t block_length_ = 128;
};
