#ifndef GROUTE_HEADER
#define GROUTE_HEADER

#include <vector>
#include <algorithm>

class groute
{
public:
    const static int B_MARK = 0x1000000;
    const static int NODE_MASK = B_MARK - 1;
    long val;
    long psi;
    long salt;
    std::vector<int> node_route;
    // a node_route that carries only one element can be used for processing a single node.
    //std::vector<int> p_mark;
    //std::vector<int> b_mark;
    std::vector<std::pair<int, int>> shop_thr;
    std::vector<int> shop_tb_tokens; // shop times before tokens.
    // this is accumulative, so if it is 50, 100, 150 then you must shop
    // no more than 50 times before the first token, 100 times in total
    // before the second, and 150 times before the third. This is kind of
    // more reasonable as the shopping times are now cumulative.
};

#endif
