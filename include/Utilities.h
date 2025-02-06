#pragma once
#ifndef RSOS_UTILITY
#define RSOS_UTILITY
#include<vector>

template <typename T>
void no_dangle(T*& classPointer){
	if(classPointer){
		delete classPointer;
		classPointer = nullptr;
	}
}

template <typename T>
void no_dangle(std::vector<T>& vec){
    if (vec) {
        vec->clear();
        delete vec;
        vec = nullptr;
    }
}

template <typename T>
void no_dangle(std::vector<T*>& vecPointer){
    if (vecPointer) {
        for (T* ptr : *vecPointer) {
            delete ptr;
        }
        vecPointer->clear();
        delete vecPointer;
        vecPointer = nullptr;
    }
}

#endif