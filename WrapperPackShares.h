//
// Created by hassan on 2/4/26.
//

#ifndef RRSS_WRAPPERPACKSHARES_H
#define RRSS_WRAPPERPACKSHARES_H

#include <cstdint>
#include <string>
#include <stdexcept>
#include <vector>


void append_u32(std::string &out, uint32_t x);
uint32_t read_u32(const unsigned char *p);
std::string pack_share(const std::string &id, const std::vector<int> &v);
void unpack_share(const std::string &buf, std::string &id_out, std::vector<int> &v_out);

#endif //RRSS_WRAPPERPACKSHARES_H