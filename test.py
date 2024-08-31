import ctypes

lib = ctypes.CDLL("absolute/path/to/hello.dll")
py_add_one = lib.add_one
py_add_one.argtypes = [ctypes.c_int]
value = 5
results = py_add_one(value)
print(results)
