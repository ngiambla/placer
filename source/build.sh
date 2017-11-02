#! /bin/bash

echo "-- Building UMFPACK";

cd SuiteSparse/SuiteSparse_config;
make library;
cd ../AMD;
make library;
cd ../UMFPACK;
make library;
cd ../../;


echo "-- Building Placer"; 
make clean;
make;