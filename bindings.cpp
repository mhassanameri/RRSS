//
// Created by mameriek on 2/3/26.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "RandomRobustSS.h"
#include "WrapperPackShares.h"

namespace py = pybind11;

PYBIND11_MODULE(rrss, m) {
    py::class_<RandomRobustSS>(m, "RandomRobustSS")
            .def(py::init<int, int, int>(), py::arg("len"), py::arg("t"), py::arg("lambda_"))
            .def_readonly("_len", &RandomRobustSS::_len)
            .def_readonly("_t", &RandomRobustSS::_t)
            .def_readonly("lambda_", &RandomRobustSS::lambda_)
            .def_readonly("_m", &RandomRobustSS::_m)
            .def_readonly("_n", &RandomRobustSS::_n)

            .def("params",
                 [](const RandomRobustSS &self) {
                     py::dict d;
                     d["len"] = self._len;
                     d["t"] = self._t;
                     d["lambda"] = self.lambda_;
                     d["m"] = self._m;
                     d["n"] = self._n;
                     // add NTL_params fields if you want:
                     // d["m_V"] = self.NTL_params.m_V;
                     return d;
                 })


            // Return (secret, shares_bytes_list)
            .def("sharegen_bytes",
                 [](RandomRobustSS &self, py::bytes secret_in) {
                     std::string secret = secret_in;

                     // produces: vector<pair<string, vector<int>>>
                     auto shares = RandomRobustSS::ShareGen(self._len, self._t, self.NTL_params, secret);

                     py::list out;
                     for (auto &pr: shares) {
                         std::string blob = pack_share(pr.first, pr.second);
                         out.append(py::bytes(blob));
                     }

                     return py::make_tuple(py::bytes(secret), out);
                 },
                 py::arg("secret"))

            // Decode one packed share bytes -> (id: str, vec: List[int])

            .def_static("decode_share",
                        [](py::bytes b) {
                            std::string buf = b;
                            std::string id;
                            std::vector<int> v;
                            int len;
                            unpack_share(buf, id, v, len);

                            // return id as raw bytes (safe even if not UTF-8)
                            return py::make_tuple(py::bytes(id), v);
                        },
                        py::arg("share_bytes"))


            .def_static("encode_share",
                        [](py::bytes id_bytes, const std::vector<int> &v) {
                            std::string id = id_bytes; // raw bytes -> std::string
                            std::string blob = pack_share(id, v);
                            return py::bytes(blob);
                        },
                        py::arg("id_bytes"), py::arg("vec"))

            // Reconstruct secret from packed share bytes:
            // shares_bytes: list[bytes] of packed shares (as returned by sharegen_bytes)
            // threshold: optional override (defaults to self._t)
            .def("reconstruct_bytes",
                 [](RandomRobustSS &self,
                    const std::vector<py::bytes> &shares_bytes,
                    py::object threshold_obj) {
                     int threshold = self._t;
                     if (!threshold_obj.is_none()) {
                         threshold = threshold_obj.cast<int>();
                     }
                     if (threshold <= 0) {
                         throw std::runtime_error("threshold must be positive");
                     }

                     // Decode packed bytes -> vector<pair<string, vector<int>>>
                     std::vector<std::pair<std::string, std::vector<int> > > shares;
                     shares.reserve(shares_bytes.size());

                     for (const auto &b: shares_bytes) {
                         std::string buf = b;
                         std::string id;
                         std::vector<int> v;
                         unpack_share(buf, id, v, self._len);
                         shares.emplace_back(std::move(id), std::move(v));
                     }

                     // IMPORTANT: your SecretReconstruction expects `len` = number of shares provided
                     int provided_len = static_cast<int>(shares.size());
                     if (provided_len <= 0) {
                         throw std::runtime_error("no shares provided");
                     }

                     std::string secret = self.SecretReconstruction(
                         provided_len, // len (#shares provided)
                         threshold, // threshold
                         self.NTL_params, // params
                         shares // decoded shares
                     );

                     // Your C++ code prints "not enough shares" and returns "" (likely).
                     // Turn that into a Python exception for nicer UX.
                     if (secret.empty()) {
                         throw std::runtime_error("reconstruction failed (not enough valid shares or decoding error)");
                     }

                     return py::bytes(secret);
                 },
                 py::arg("shares_bytes"),
                 py::arg("threshold") = py::none()
            );
}
