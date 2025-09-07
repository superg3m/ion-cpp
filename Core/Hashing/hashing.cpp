#include "hashing.hpp"
#include "../String/string.hpp"
#include "../Container/container.hpp"

// Original location:
// https://github.com/majek/csiphash/1
#if defined(_WIN32)
    #define _le64toh(x) ((u64)(x))
#elif defined(__APPLE__)
    #include <libkern/OSByteOrder.h>
    #define _le64toh(x) OSSwapLittleToHostInt64(x)
#else
    #if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
        #include <sys/endian.h>
    #else
        #include <endian.h>
    #endif

    #if defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN   
        #define _le64toh(x) ((u64)(x))
    #else
        #define _le64toh(x) le64toh(x)
    #endif
#endif

namespace ION::Hashing {
    #define ROTATE(x, b) (u64)( ((x) << (b)) | ( (x) >> (64 - (b))) )

    #define HALF_ROUND(a,b,c,d,s,t)	\
        a += b; c += d;			    \
        b = ROTATE(b, s) ^ a;		\
        d = ROTATE(d, t) ^ c;		\
        a = ROTATE(a, 32);          \

    #define DOUBLE_ROUND(v0,v1,v2,v3)  \
        HALF_ROUND(v0,v1,v2,v3,13,16); \
        HALF_ROUND(v2,v1,v0,v3,17,21); \
        HALF_ROUND(v0,v1,v2,v3,13,16); \
        HALF_ROUND(v2,v1,v0,v3,17,21)

    u64 siphash24(const void* source, u64 source_size) {
        const char key[16] = {
            0x00, 0x01, 0x02, 0x03,
            0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B,
            0x0C, 0x0D, 0x0E, 0x0F
        };

        const u64 *_key = (const u64*)key;
        u64 k0 = _le64toh(_key[0]);
        u64 k1 = _le64toh(_key[1]);
        u64 b = source_size << 56;
        const u64* in = (const u64*)source;

        u64 v0 = k0 ^ 0x736f6d6570736575ULL;
        u64 v1 = k1 ^ 0x646f72616e646f6dULL;
        u64 v2 = k0 ^ 0x6c7967656e657261ULL;
        u64 v3 = k1 ^ 0x7465646279746573ULL;

        while (source_size >= 8) {
            u64 mi = _le64toh(*in);
            in += 1; source_size -= 8;
            v3 ^= mi;
            DOUBLE_ROUND(v0,v1,v2,v3);
            v0 ^= mi;
        }

        u64 t = 0; 
        u8* pt = (u8*)&t; 
        const u8* m = (const u8*)in;
        switch (source_size) {
            case 7: pt[6] = m[6];
            case 6: pt[5] = m[5];
            case 5: pt[4] = m[4];
            case 4: *((u32*)&pt[0]) = *((u32*)&m[0]); break;
            case 3: pt[2] = m[2];
            case 2: pt[1] = m[1];
            case 1: pt[0] = m[0];
        }
        b |= _le64toh(t);

        v3 ^= b;
        DOUBLE_ROUND(v0,v1,v2,v3);
        v0 ^= b; v2 ^= 0xff;
        DOUBLE_ROUND(v0,v1,v2,v3);
        DOUBLE_ROUND(v0,v1,v2,v3);

        u64 ret = (v0 ^ v1) ^ (v2 ^ v3);
        return ret;
    }

    u64 cstring_hash(const void* str, u64 str_length) {
        (void)str_length;

        u64 hash = 5381;
        u8* str_ptr = (u8*)str;
        int c;

        while ((c = *str_ptr++)) {
            hash = ((hash << 5) + hash) + c;
        }

        return hash;
    }

    bool cstring_equality(const void* c1, byte_t c1_size, const void* c2, byte_t c2_size) {
        (void)c1_size;
        (void)c2_size;

        u64 c1_length = ION::String::length((const char*)c1);
        u64 c2_length = ION::String::length((const char*)c2);
        
        return ION::String::equal((const char*)c1, c1_length, (const char*)c2, c2_length);
    }

    u64 string_view_hash(const void* view, u64 str_length) {
        (void)str_length;
        ION::Container::View<char>* str_view = (ION::Container::View<char>*)view;
        u64 hash = 5381;
        int c;

        for (u64 i = 0; i < str_view->length; i++) {
            c = str_view->data[i];
            hash = ((hash << 5) + hash) + c;
        }

        return hash;
    }

    bool string_view_equality(const void* c1, size_t c1_size, const void* c2, size_t c2_size) {
        (void)c1_size;
        (void)c2_size;

        ION::Container::View<char>* s1 = (ION::Container::View<char>*)c1;
        ION::Container::View<char>* s2 = (ION::Container::View<char>*)c2;

        return ION::String::equal(s1->data, s1->length, s2->data, s2->length);
    }
}