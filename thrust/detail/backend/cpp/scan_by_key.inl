/*
 *  Copyright 2008-2011 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <thrust/detail/config.h>
#include <thrust/detail/backend/cpp/scan_by_key.h>
#include <thrust/iterator/iterator_traits.h>
#include <thrust/detail/backend/dereference.h>

namespace thrust
{
namespace detail
{
namespace backend
{
namespace cpp
{


template<typename InputIterator1,
         typename InputIterator2,
         typename OutputIterator,
         typename BinaryPredicate,
         typename BinaryFunction>
  OutputIterator inclusive_scan_by_key(tag,
                                       InputIterator1 first1,
                                       InputIterator1 last1,
                                       InputIterator2 first2,
                                       OutputIterator result,
                                       BinaryPredicate binary_pred,
                                       BinaryFunction binary_op)
{
    typedef typename thrust::iterator_traits<InputIterator1>::value_type KeyType;
    typedef typename thrust::iterator_traits<OutputIterator>::value_type ValueType;

    if(first1 != last1)
    {
        KeyType   prev_key   = backend::dereference(first1);
        ValueType prev_value = backend::dereference(first2);

        backend::dereference(result) = prev_value;

        for(++first1, ++first2, ++result;
                first1 != last1;
                ++first1, ++first2, ++result)
        {
            KeyType key = backend::dereference(first1);

            if (binary_pred(prev_key, key))
                backend::dereference(result) = prev_value = binary_op(prev_value, backend::dereference(first2));
            else
                backend::dereference(result) = prev_value = backend::dereference(first2);

            prev_key = key;
        }
    }

    return result;
}


template<typename InputIterator1,
         typename InputIterator2,
         typename OutputIterator,
         typename T,
         typename BinaryPredicate,
         typename BinaryFunction>
  OutputIterator exclusive_scan_by_key(tag,
                                       InputIterator1 first1,
                                       InputIterator1 last1,
                                       InputIterator2 first2,
                                       OutputIterator result,
                                       T init,
                                       BinaryPredicate binary_pred,
                                       BinaryFunction binary_op)
{
    typedef typename thrust::iterator_traits<InputIterator1>::value_type KeyType;
    typedef typename thrust::iterator_traits<OutputIterator>::value_type ValueType;

    if(first1 != last1)
    {
        KeyType   temp_key   = backend::dereference(first1);
        ValueType temp_value = backend::dereference(first2);
        
        ValueType next = init;

        // first one is init
        backend::dereference(result) = next;

        next = binary_op(next, temp_value);

        for(++first1, ++first2, ++result;
                first1 != last1;
                ++first1, ++first2, ++result)
        {
            KeyType key = backend::dereference(first1);

            // use temp to permit in-place scans
            temp_value = backend::dereference(first2);

            if (!binary_pred(temp_key, key))
                next = init;  // reset sum
                
            backend::dereference(result) = next;  
            next = binary_op(next, temp_value);

            temp_key = key;
        }
    }

    return result;
}

} // end namespace cpp
} // end namespace backend
} // end namespace detail
} // end namespace thrust
