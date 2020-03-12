// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <array>
#include <vector>
#include <cassert>

namespace MxEngine
{
    // in C++20 std::span will be added. Till that times, MxEngine provides ArrayView with simular functionality
    template<typename T>
    class array_view
    {
        T* _data;
        size_t _size;
    public:
        array_view();
        array_view(T* data, size_t size);
        array_view(const array_view&) = default;
        array_view(array_view&&) = default;
        array_view& operator=(const array_view&) = default;
        array_view& operator=(array_view&&) = default;
        ~array_view() = default;
        template<size_t N>
        array_view(T(&array)[N]);
        template<size_t N>
        array_view(std::array<T, N>& array);
        array_view(std::vector<T>& vec);
        size_t size() const;
        T* data();
        const T* data() const;
        T& operator[](size_t idx);
        const T& operator[](size_t idx) const;

        T* begin();
        T* end();
        const T* begin() const;
        const T* end() const;
    };

    template<typename T>
    using ArrayView = array_view<T>;

    template<typename T>
    inline array_view<T>::array_view()
    {
        this->_data = nullptr;
        this->_size = 0;
    }

    template<typename T>
    inline array_view<T>::array_view(T* data, size_t size)
    {
        this->_data = data;
        this->_size = size;
    }

    template<typename T>
    inline array_view<T>::array_view(std::vector<T>& vec)
    {
        this->_data = vec.data();
        this->_size = vec.size();
    }

    template<typename T>
    inline size_t array_view<T>::size() const
    {
        return this->_size;
    }

    template<typename T>
    inline T* array_view<T>::data()
    {
        return this->_data;
    }

    template<typename T>
    inline const T* array_view<T>::data() const
    {
        return this->_data;
    }

    template<typename T>
    inline T& array_view<T>::operator[](size_t idx)
    {
        assert(idx < this->_size);
        return this->_data[idx];
    }

    template<typename T>
    inline const T& array_view<T>::operator[](size_t idx) const
    {
        assert(idx < this->_size);
        return this->_data[idx];
    }

    template<typename T>
    inline T* array_view<T>::begin()
    {
        return this->_data;
    }

    template<typename T>
    inline T* array_view<T>::end()
    {
        return this->_data + this->_size;
    }

    template<typename T>
    inline const T* array_view<T>::begin() const
    {
        return this->_data;
    }

    template<typename T>
    inline const T* array_view<T>::end() const
    {
        return this->_data + this->_size;
    }

    template<typename T>
    template<size_t N>
    inline array_view<T>::array_view(T(&array)[N])
    {
        this->_data = array;
        this->_size = N;
    }

    template<typename T>
    template<size_t N>
    inline array_view<T>::array_view(std::array<T, N>& array)
    {
        this->_data = array.data();
        this->_size = N;
    }
}