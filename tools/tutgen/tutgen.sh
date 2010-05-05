#!/bin/bash

./tutgen `find selftest -name "*.cpp" -or -name "*.h" -or -name "*.hpp"` --gcc-options="-I."

