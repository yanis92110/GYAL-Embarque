PYTHON ?= python3
.PHONY: test train-step1 train-step2 train-step3 train-step4 quantize benchmark
test:
	$(PYTHON) -m unittest discover -s tests -v
# Implement these files yourselves. This Makefile is a command contract.
train-step1:
	$(PYTHON) src/train_step1.py
train-step2:
	$(PYTHON) src/train_step2.py
train-step3:
	$(PYTHON) src/train_step3.py
train-step4:
	$(PYTHON) src/train_step4.py
quantize:
	$(PYTHON) src/quantize.py
benchmark:
	$(PYTHON) src/benchmark.py
