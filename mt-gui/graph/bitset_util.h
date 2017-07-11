#ifndef FOR_BITSET_HEADER
#define FOR_BITSET_HEADER

// __bitset__ should be a left value because this is a macro.

#define forb(__iiii__, __bitset__) \
    for(int __iiii__ = __bitset__._Find_first();\
            __iiii__ != (int)__bitset__.size();\
            __iiii__ = __bitset__._Find_next(__iiii__))

#define forbb(__iiii__, __beginval__, __bitset__) \
    for(int __iiii__ = __bitset__._Find_next((__beginval__) - 1);\
            __iiii__ != (int)__bitset__.size();\
            __iiii__ = __bitset__._Find_next(__iiii__))

#define forbe(__iiii__, __endval__, __bitset__) \
    for(int __iiii__ = __bitset__._Find_first(), __end__val__ = (__endval__);\
            __iiii__ != (int)__bitset__.size() && __iiii__ < __end__val__ ;\
            __iiii__ = __bitset__._Find_next(__iiii__))

#define forbbe(__iiii__, __beginval__, __endval__, __bitset__) \
    for(int __iiii__ = __bitset__._Find_next((__beginval__) - 1), __end__val__ = (__endval__);\
            __iiii__ != (int)__bitset__.size() && __iiii__ < __end__val__ ;\
            __iiii__ = __bitset__._Find_next(__iiii__))
/* 
template<size_t _Nb>
bitset<_Nb> slice ( bitset<_Nb> base, int start = 0, int end = -1 )
{
    if(end>0)end-=_Nb;
}
 */
#endif
