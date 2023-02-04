#include "worker.h"
#include <Windows.h>
#include "public.h"
#include "vjoyinterface.h"
#include <iostream>

midi2vjoy::worker::worker(unsigned int vjid)
:   vjd(vjid)
{
    for (int i = 0; i < NUM_LAYERS * NUM_ROTARIES; ++i)
    {
        rotaries.push_back(new rotary(this, i * 6 + 3));
    }
    stop.store(false);
    t = std::thread(&worker::thread_fn, this);
}

midi2vjoy::worker::~worker()
{
    stop.store(true);
    cv.notify_one();
    t.join();
    for (auto j = rotaries.begin(); j != rotaries.end(); ++j)
    {
        delete (*j);
    }
}

void midi2vjoy::worker::post_midi_event(unsigned char id, unsigned char val)
{
    {
        std::lock_guard<std::mutex> guard(events_mutex);
        events.push((id * 256) + val);
    }
    cv.notify_one();
}

void midi2vjoy::worker::button_up(unsigned int id)
{
    SetBtn(0, vjd, id);
}

void midi2vjoy::worker::thread_fn()
{
    while (!stop.load())
    {
        unsigned e = 0;
        {
            std::unique_lock lock(events_mutex);
            cv.wait_for(lock, std::chrono::milliseconds(50));
            while (!events.empty())
            {
                e = events.front();
                events.pop();
            }
            for (auto j = rotaries.begin(); j != rotaries.end(); ++j)
            {
                (*j)->countdown();
            }
        }
        if (e)
        {
            action(e / 256, e % 256);
        }
    }
}

void midi2vjoy::worker::action(unsigned char id, unsigned char val)
{
    if ((id >= 25) && (id <= 32))
    {
        // Layer A, bottom row push buttons, maps to 1,7,13,...,43
        SetBtn(val != 0, vjd, ((id - 25) * 6) + 1);
    }
    else if ((id >= 17) && (id <= 24))
    {
        // Layer A, upper row push buttons, maps to 2,8,14,...,44
        SetBtn(val != 0, vjd, ((id - 17) * 6) + 2);
    }
    else if ((id >= 75) && (id <= 82))
    {
        // Layer B, bottom row push buttons, maps to 49,55,61,...,91
        SetBtn(val != 0, vjd, ((id - 75) * 6) + 49);
    }
    else if ((id >= 67) && (id <= 74))
    {
        // Layer B, upper row push buttons, maps to 50,56,62,...,92
        SetBtn(val != 0, vjd, ((id - 67) * 6) + 50);
    }
    else if ((id >= 9) && (id <= 16))
    {
        // Layer A, rotary button push - toggles mode
        rotaries[id - 9]->push(val);
    }
    else if ((id >= 59) && (id <= 66))
    {
        // Layer B, rotary button push - toggles mode
        rotaries[NUM_ROTARIES + id - 59]->push(val);
    }
    else if ((id >= 1) && (id <= 8))
    {
        // Layer A, rotary button turn
        auto bid = rotaries[id - 1]->rotate(val);
        if (bid) SetBtn(1, vjd, bid);
    }
    else if ((id >= 51) && (id <= 58))
    {
        // Layer B, rotary button turn
        auto bid = rotaries[NUM_ROTARIES + id - 51]->rotate(val);
        if (bid) SetBtn(1, vjd, bid);
    }
    else
    {
        std::cout << "UNKNOWN " << (unsigned)id << "," << (unsigned)val << std::endl;
    }
}


