from torch.utils.cpp_extension import load
lltm_cpp = load(name="linear_cpp", sources=["linear.cpp"], extra_cflags={'-march=haswell'}, verbose=True)
