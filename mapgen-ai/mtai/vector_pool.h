#ifndef VECTOR_BOOL_HEADER
#define VECTOR_BOOL_HEADER

#include <vector>
#include <cstddef>

#include "eprintf.h"

template<typename T>
class vpool
{

public:
    typedef std::vector<T> obj_t;

private:
    static std::vector<std::vector<obj_t*>> pool;
    static std::vector<std::vector<bool>> usage;

    static bool free_vector ( obj_t* o_p )
    {
        //eprintf ( "free vector %p\n", o_p );
        const int n = o_p->size();
        std::vector<obj_t*>& pool_slice = pool[n];
        std::vector<bool>& usage_slice = usage[n];
        for ( int i = 0, s = pool_slice.size(); i < s; i++ )
        {
            if ( pool_slice[i] == o_p )
            {
                usage_slice[i] = false;
                return true;
            }
        }
        eprintf ( "Abnormal free vector! The vector is not in the pool.\n" );
        return false;
    }

public:
    class vptr_t
    {
        obj_t* ptr;
    public:
        vptr_t ( obj_t* ptr ) : ptr ( ptr )
        {
        }
        obj_t& operator*()
        {
            return *ptr;
        }
        obj_t* operator->()
        {
            return ptr;
        }
        ~vptr_t()
        {
            free_vector ( ptr );
        }
    };

    static vptr_t new_vector ( size_t n )
    {
        if ( pool.size() <= n )
        {
            pool.resize ( n + 1 );
            usage.resize ( n + 1 );
        }
        std::vector<obj_t*>& pool_slice = pool[n];
        std::vector<bool>& usage_slice = usage[n];
        size_t obj_index = pool_slice.size();
        for ( size_t i = 0, s = obj_index // the size
                                ; i < s; i++ )
        {
            if ( !usage_slice[i] )
            {
                obj_index = i;
                break;
            }
        }
        if ( obj_index == pool_slice.size() )
        {
            pool_slice.push_back ( new obj_t ( n ) );
            usage_slice.push_back ( false );
        }
        usage_slice[obj_index] = true;
        //eprintf ( "new vector %zu: %p\n", obj_index, pool_slice[obj_index] );
        return vptr_t ( pool_slice[obj_index] );
    }
};

template<typename T>
std::vector<std::vector<typename vpool<T>::obj_t*>> vpool<T>::pool;
template<typename T>
std::vector<std::vector<bool>> vpool<T>::usage;

#endif
