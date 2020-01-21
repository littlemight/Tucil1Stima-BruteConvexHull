# Tucil 1 StiMa : Brute Force Convex Hull
Michel Fang / 13518137 / K2

## Prerequisites
1. C++11
2. Python 2.7 dan matplotlib
3. Linux (wrapper matplotlib untuk C++ hanya bekerja di linux)

## Struktur Folder
```
├── compile.sh
├── run.sh
├── run_input.sh
├── README.md
├── bin
│   ├── a.out
├── src
│   ├── dependencies
│   └── main.cpp
└── test_data
    ├── IN
    ├── IN2
    ├── TC_N10
    ├── TC_N20
    ├── TC_N20_Manual
    ├── TC_N5
    └── TC_N5_Manual
```
1. bin berisi executable dari program
2. src berisi kode program dan dependencies (matplotlib-cpp)
3. test_data berisi beberapa file input untuk program

## Compile dan Run
```
./compile.sh
```

Untuk menjalankan
```
./run.sh
```

Jika ingin menggunakan sebuah kasus uji dari sebuah file
```
./run_input.sh <nama file input>
```

## Credits
matplotlib-cpp : https://github.com/lava/matplotlib-cpp