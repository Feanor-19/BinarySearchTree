#pragma once

#include <algorithm>
#include <iostream>

namespace Driver
{

class DriverException : public std::runtime_error
{
public:
    DriverException(const char *err_msg) : std::runtime_error(err_msg) {}
};

class UnknownCmdE : public DriverException
{
public:
    UnknownCmdE() : DriverException{"Unknown command"} {}
};

class WrongInputE : public DriverException
{
public:
    WrongInputE() : DriverException{"Wrong input"} {}
};

template <typename C, typename T>
int range_query(const C& s, T fst, T snd)
{
    using itt = typename C::iterator;
    itt start = s.lower_bound(fst);
    itt final = s.upper_bound(snd);
    return std::distance(start, final);
}

template <typename C, typename T>
void driver(std::istream &inp, std::ostream &out)
{
    const char CMD_INSERT = 'k';
    const char CMD_QUERY  = 'q';

    C container;

    while (inp) 
    {
        char cmd = 0;
        inp >> cmd;
        if (inp && (!inp.good() || (cmd != CMD_INSERT && cmd != CMD_QUERY)))
            throw UnknownCmdE{};

        if (cmd == CMD_INSERT)
        {
            T inp_data = 0;
            inp >> inp_data;
            if (!inp.eof() && !inp.good()) throw WrongInputE{};
            container.insert(inp_data);
        }
        else if (cmd == CMD_QUERY)
        {
            T fst = 0, snd = 0;
            inp >> fst >> snd;
            if (!inp.eof() && !inp.good()) throw WrongInputE{};
            out << range_query(container, fst, snd) << std::endl;
        }
    }   
}

} // namespace Driver