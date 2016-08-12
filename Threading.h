#ifndef EBR_THREADING
#define EBR_THREADING

#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

//http://stackoverflow.com/questions/24465533/implementing-boostbarrier-in-c11
class Barrier
{
private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::size_t _count;

	std::size_t _original_count;

public:
	explicit Barrier(std::size_t count= THREAD_COUNT) : _count{count}, _original_count{count} { }
    void Wait()
    {
        std::unique_lock<std::mutex> lock{_mutex};
        if (--_count == 0) {
            _cv.notify_all();
        } else {
            _cv.wait(lock, [this] { return _count == 0; });
        }
    }

	void Reset()
	{
		_count= _original_count;
	}
};

#endif