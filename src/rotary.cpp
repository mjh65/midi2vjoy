
#include "rotary.h"
#include <iostream>

midi2vjoy::rotary::rotary(rotary_callback* cb, unsigned int b)
:   m_cb(cb), m_base_id(b),
    m_mode(false), m_current_btn_down(0), m_btn_timer(0)

{
}

midi2vjoy::rotary::~rotary()
{
    release();
}

void midi2vjoy::rotary::mode(unsigned char val)
{
    m_mode = (val != 0);
}

unsigned int midi2vjoy::rotary::rotate(unsigned char velocity)
{
    int bidx = m_base_id + (m_mode ? 2 : 0) + ((velocity > 64) ? 1 : 0);
    if (m_current_btn_down != bidx)
    {
        release();
    }
    auto sv = (int)velocity - 64;
    auto mag = ((sv * sv - 1) / 4) + 1;
    m_current_btn_down = bidx;
    //std::cout << "T:" << m_btn_timer << "+" << mag << std::endl;
    m_btn_timer += mag;
    return bidx;
}

void midi2vjoy::rotary::countdown()
{
    if (m_btn_timer)
    {
        if (--m_btn_timer == 0)
        {
            release();
        }
    }
}

void midi2vjoy::rotary::release()
{
    if (m_current_btn_down > 0)
    {
        m_cb->button_up(m_current_btn_down);
    }
    m_current_btn_down = 0;
    m_btn_timer = 0;
}

