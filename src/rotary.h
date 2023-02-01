
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
