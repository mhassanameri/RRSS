//
// Created by hassan on 2/4/26.
//

#include "WrapperPackShares.h"


void append_u32(std::string &out, uint32_t x) {
    out.push_back(static_cast<char>(x & 0xFF));
    out.push_back(static_cast<char>((x >> 8) & 0xFF));
    out.push_back(static_cast<char>((x >> 16) & 0xFF));
    out.push_back(static_cast<char>((x >> 24) & 0xFF));
}

uint32_t read_u32(const unsigned char *p) {
    return (uint32_t) p[0]
           | ((uint32_t) p[1] << 8)
           | ((uint32_t) p[2] << 16)
           | ((uint32_t) p[3] << 24);
}

std::string pack_share(const std::string &id, const std::vector<int> &v) {
    std::string out;
    out.reserve(8 + id.size() + 4ull * v.size());

    append_u32(out, (uint32_t) id.size());
    out.append(id);

    append_u32(out, (uint32_t) v.size());
    for (int x: v) {
        append_u32(out, (uint32_t) x); // store as unsigned 32-bit
    }
    return out;
}

void unpack_share(const std::string &buf, std::string &id_out, std::vector<int> &v_out, int len) {
    const auto *p = (const unsigned char *) buf.data();
    size_t n = buf.size();
    size_t off = 0;

    if (n < 8) throw std::runtime_error("share too short");

    uint32_t id_len = read_u32(p + off);
    off += 4;
    if (off + id_len + 4 > n) {
        id_len = SSShareSizeMal;
        id_out.assign((const char *) p + off, id_len);
        off += id_len;
        // throw std::runtime_error("bad id_len"+std::to_string(n) + " " + std::to_string(off) + " " + std::to_string(id_len));
    } else {
        id_out.assign((const char *) p + off, id_len);
        off += id_len;
    }


    uint32_t v_len = read_u32(p + off);
    off += 4;

    if (off + 4ull * v_len > n) {
        v_len = 6*len;
        // v_len = 6 * RRSSLEN;
        // throw std::runtime_error("bad vec_len");
    }

    v_out.resize(v_len);
    for (uint32_t i = 0; i < v_len; i++) {
        v_out[i] = (int) read_u32(p + off);
        off += 4;
    }


    if (off != n) {
        // optional: allow trailing bytes, or enforce exact
        // throw std::runtime_error("extra trailing bytes");
    }
}
