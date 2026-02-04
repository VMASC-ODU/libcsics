
#pragma once
#include <cstddef>
#include <memory>

bool binary_arr_eq(std::byte* arr1, std::byte* arr2, std::size_t size);

std::unique_ptr<std::byte[]> generate_random_bytes(std::size_t size);
    

