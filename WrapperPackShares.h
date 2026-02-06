//
// Created by hassan on 2/4/26.
//

#ifndef RRSS_WRAPPERPACKSHARES_H
#define RRSS_WRAPPERPACKSHARES_H

#include <cstdint>
#include <string>
#include <stdexcept>
#include <vector>

const size_t SSShareSizeMal = 20; //Indicating the secret sharing size in bytes; TODO: I may need to do similar thing for the RRSSLEN case. Currently hardcoded.
// const size_t RRSSLEN = 10; //Indicating the secret sharing size in bytes; TODO: Fixed: I just considered the len as part of the inputs to the underlying bindings function.


void append_u32(std::string &out, uint32_t x);
uint32_t read_u32(const unsigned char *p);
std::string pack_share(const std::string &id, const std::vector<int> &v);
void unpack_share(const std::string &buf, std::string &id_out, std::vector<int> &v_out, int len);

#endif //RRSS_WRAPPERPACKSHARES_H