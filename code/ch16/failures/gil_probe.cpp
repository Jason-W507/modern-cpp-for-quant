#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <atomic>
#include <chrono>
#include <exception>
#include <thread>

namespace {

bool wait_for_progress(const std::atomic<bool>& progressed,
                       const std::chrono::steady_clock::duration timeout) {
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  while (!progressed.load(std::memory_order_acquire) &&
         std::chrono::steady_clock::now() < deadline) {
    std::this_thread::yield();
  }
  return progressed.load(std::memory_order_acquire);
}

bool run_probe(const bool release_during_wait) {
  std::atomic<bool> progressed{false};
  std::thread worker{[&progressed] {
    const PyGILState_STATE state = PyGILState_Ensure();
    progressed.store(true, std::memory_order_release);
    PyGILState_Release(state);
  }};

  bool observed = false;
  if (release_during_wait) {
    PyThreadState* state = PyEval_SaveThread();
    observed = wait_for_progress(progressed, std::chrono::seconds{2});
    worker.join();
    PyEval_RestoreThread(state);
  } else {
    observed = wait_for_progress(progressed, std::chrono::milliseconds{50});
    PyThreadState* state = PyEval_SaveThread();
    worker.join();
    PyEval_RestoreThread(state);
  }
  return observed;
}

PyObject* hold_gil_probe(PyObject*, PyObject*) {
  try {
    return PyBool_FromLong(run_probe(false));
  } catch (const std::exception& error) {
    PyErr_SetString(PyExc_RuntimeError, error.what());
    return nullptr;
  }
}

PyObject* release_gil_probe(PyObject*, PyObject*) {
  try {
    return PyBool_FromLong(run_probe(true));
  } catch (const std::exception& error) {
    PyErr_SetString(PyExc_RuntimeError, error.what());
    return nullptr;
  }
}

PyMethodDef methods[] = {
    {"hold_gil_probe", hold_gil_probe, METH_NOARGS,
     "Observe whether a worker can acquire the GIL while the caller holds it."},
    {"release_gil_probe", release_gil_probe, METH_NOARGS,
     "Observe whether a worker can acquire the GIL while the caller releases it."},
    {nullptr, nullptr, 0, nullptr},
};

PyModuleDef module = {PyModuleDef_HEAD_INIT,
                      "_quant_ch16_gil_probe",
                      nullptr,
                      -1,
                      methods,
                      nullptr,
                      nullptr,
                      nullptr,
                      nullptr};

}  // namespace

PyMODINIT_FUNC PyInit__quant_ch16_gil_probe() {
  return PyModule_Create(&module);
}
