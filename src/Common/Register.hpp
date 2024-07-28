//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_REGISTER_HPP
#define RISC_V_SIMULATOR_REGISTER_HPP

template <typename T>
class Register{
private:
    T nxt;
public:
    T val;
    Register(): val(), nxt() {}
    explicit Register(T val_): val(val_), nxt(val_) {}
    T read() { return val; }
    T current() { return nxt; }
    void write(const T &t) { nxt = t; }
    void tick() { val = nxt; }
    operator T() { return val; }
    void operator=(T next) { nxt = next; }
};

#endif //RISC_V_SIMULATOR_REGISTER_HPP
