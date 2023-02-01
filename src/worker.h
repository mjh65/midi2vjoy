#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "rotary.h"

namespace midi2vjoy {

class worker : public rotary_callback
{
public:
    worker(unsigned int vjid);
    ~worker();

    void post_midi_event(unsigned char id, unsigned char val);

    void button_up(unsigned int id) override;

private:
    void thread_fn();
    void action(unsigned char id, unsigned char val);

private:
    static const unsigned int NUM_LAYERS = 2;
    static const unsigned int NUM_ROTARIES = 8;

    unsigned int vjd;
    std::vector<rotary *> rotaries;


    std::thread t;
    std::atomic_bool stop;
    std::queue<unsigned> events;
    std::mutex events_mutex;
    std::condition_variable cv;

};

}

