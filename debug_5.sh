#!/bin/bash

# Debug script for Test 5
echo "=== Debugging Test 5 ==="

# Check if input_5.txt exists and examine its content
if [ -f "input_5.txt" ]; then
    echo "Input file content:"
    head -10 input_5.txt
    echo "..."
    echo "Total lines: $(wc -l < input_5.txt)"
    echo "Dimensions: $(head -1 input_5.txt | tr ',' '\n' | wc -l)"
else
    echo "input_5.txt not found!"
fi

echo ""
echo "=== Testing Python implementation ==="
if [ -f "input_5.txt" ]; then
    python3 kmeans.py 7 999 < input_5.txt > python_output.txt 2> python_error.txt
    echo "Python exit code: $?"
    echo "Python stdout:"
    cat python_output.txt
    echo "Python stderr:"
    cat python_error.txt
fi

echo ""
echo "=== Testing C implementation ==="
if [ -f "input_5.txt" ]; then
    ./kmeans 7 999 < input_5.txt > c_output.txt 2> c_error.txt
    echo "C exit code: $?"
    echo "C stdout:"
    cat c_output.txt
    echo "C stderr:"
    cat c_error.txt
fi

echo ""
echo "=== Expected output ==="
if [ -f "output_5.txt" ]; then
    cat output_5.txt
else
    echo "output_5.txt not found!"
fi

echo ""
echo "=== Comparing outputs ==="
if [ -f "python_output.txt" ] && [ -f "output_5.txt" ]; then
    echo "Python vs Expected:"
    diff python_output.txt output_5.txt
fi

if [ -f "c_output.txt" ] && [ -f "output_5.txt" ]; then
    echo "C vs Expected:"
    diff c_output.txt output_5.txt
fi