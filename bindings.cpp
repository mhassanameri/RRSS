//
// Created by mameriek on 2/3/26.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/cast.h>

#include "RandomRobustSS.h"

namespace py = pybind11;

PYBIND11_MODULE(rrss, m) {
m.doc() = "Random Robust Secret Sharing (RRSS) bindings";

py::class_<RandomRobustSS>(m, "RandomRobustSS")
.def(py::init<int, int, int>(),
        py::arg("len"),
        py::arg("t"),
        py::arg("lambda_"))

.def_readonly("len", &RandomRobustSS::_len)
.def_readonly("t", &RandomRobustSS::_t)
.def_readonly("lambda_", &RandomRobustSS::lambda_)
.def_readonly("_m", &RandomRobustSS::_m)
.def_readonly("_n", &RandomRobustSS::_n)

.def_static("rrss_init",
&RandomRobustSS::RRSS_Init)

.def_static("sharegen",
&RandomRobustSS::ShareGen,
py::arg("len"),
py::arg("t"),
py::arg("lambda"),
py::arg("secret"))

.def_static("share_reconstruct",
&RandomRobustSS::SecretReconstruction,
py::arg("len"),
py::arg("t"),
py::arg("lambda"),
py::arg("shares"));
}
