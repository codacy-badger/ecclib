ACLOCAL_AMFLAGS=-I m4
if COND_BENCHMARKS
  MAYBE_BENCHMARKS = benchmark
endif
if COND_EXAMPLES
  MAYBE_EXAMPLES = examples
endif
SUBDIRS=src tests include $(MAYBE_BENCHMARKS) $(MAYBE_EXAMPLES)
