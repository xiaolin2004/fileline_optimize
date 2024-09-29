#!/bin/bash

# 编译 filelines 程序
echo "Compiling filelines..."
make filelines

# 检查编译是否成功
if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi

echo "Compilation successful."

# 执行 filelines 程序，传入 data 作为参数
echo "Running ./filelines data"
./filelines data

# 检查程序执行是否成功
if [ $? -ne 0 ]; then
    echo "Execution failed."
    exit 1
fi

echo "Execution completed."
