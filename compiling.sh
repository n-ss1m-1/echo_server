# 逐个编译，定位错误
for file in *.cc; do
    echo "Compiling $file..."
    g++ -std=c++11 -c "$file" -o "${file%.cc}.o"
    if [ $? -ne 0 ]; then
        echo "Failed to compile $file"
        exit 1
    fi
done
