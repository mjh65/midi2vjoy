/*
 *   Midi2vJoy - Windows utility to feed vJoy from MIDI controller
 *   Copyright (C) 2023 Michael Hasling <michael.hasling@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

namespace midi2vjoy {

class rotary_callback
{
public:
    virtual void button_up(unsigned int id) = 0;
};

class rotary
{
public:
    rotary(rotary_callback *cb, unsigned int b);
    ~rotary();

    void mode(unsigned char val);
    unsigned int rotate(unsigned char velocity);
    void countdown();

private:
    void release();

private:
    rotary_callback* const m_cb;
    unsigned int const m_base_id;
    bool m_mode;

    unsigned int m_current_btn_down;
    unsigned int m_btn_timer;

};

}
