#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <utility>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>

#define UNUSED(X) ((void)X)

//#define BOUNDS_CHECKING

template<typename T>
inline void write_vec(std::vector<uint8_t> &vec,size_t idx,const T &v)
{
#ifdef BOUNDS_CHECKING
    if(vec.size() < idx + sizeof(T))
    {
        printf("[write_vec] bounds violation at %zd(size = %zd)\n",idx,vec.size());
        exit(1);
    }
#endif
    memcpy(&vec[idx],&v,sizeof(T));
}
template<typename T>
inline T read_vec(const std::vector<uint8_t> &vec,size_t idx)
{
#ifdef BOUNDS_CHECKING
    if(vec.size() < idx + sizeof(T))
    {
        printf("[read_vec] bounds violation at %zd(size = %zd)\n",idx,vec.size());
        exit(1);
    }
#endif
    T v;
    memcpy(&v,&vec[idx],sizeof(T));
    return v;
}


// read entire file into a string
inline std::string read_file(const std::string &filename)
{
    std::ifstream fp{filename};

    if(fp)
    {
        return std::string((std::istreambuf_iterator<char>(fp)),
                    (std::istreambuf_iterator<char>()));
    }

    return "";
}

inline std::string get_binary_file_name(const std::string &filename)
{
	std::string save_name = filename;

	size_t ext_idx = filename.find_last_of("."); 
	if(ext_idx != std::string::npos)
	{
		save_name = filename.substr(0, ext_idx); 	
	}

	save_name += ".mina";

	return save_name;
}



inline void write_file(const std::string &filename, std::vector<uint8_t> &buf)
{
    std::ofstream fp(filename,std::ios::binary);

    if(!fp)
    {
        printf("failed to open file: %s\n",filename.c_str());
        exit(1);
    }


    fp.write((char*)buf.data(),buf.size());
}


inline bool is_set(uint64_t reg, int bit) noexcept
{
	return ((reg >> bit) & 1);
}

inline uint64_t set_bit(uint64_t v,int bit) noexcept
{
    return v | (1 << bit);
}


inline uint64_t deset_bit(uint64_t v,int bit) noexcept
{
    return v & ~(1 << bit);
}

//https://stackoverflow.com/questions/42534749/signed-extension-from-24-bit-to-32-bit-in-c
template<typename T>
T sign_extend(T x, int b)
{
	T res = 1;
	res <<= b - 1;
	return (x ^ res) - res;
}