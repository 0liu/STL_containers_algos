/*
** Dynamic programming exercises header.
 */

#ifndef __DP_H_
#define __DP_H_

#include <string>
#include <vector>

namespace DP {
    // Recursive DP & memoizing to compute Fibonacci numbers.
    unsigned fib(unsigned n);

    // Bottom-up DP to compute Fibonacci numbers.
    unsigned fib_bottomup(unsigned n);

    // Split text (list of words) into lines with DP.
    class TextJustify {
        public:
            TextJustify(const std::string& text_file, unsigned page_width=1);
            void read_text(const std::string& text_file);
            void justify(unsigned page_width);
            void print_justified();
        private:
            unsigned m_page_width;
            double badness(unsigned i, unsigned j);
            std::vector<std::string> m_words;
            std::vector<double> m_dp;
            std::vector<unsigned> m_parents;
    };

    // Perfect-information blackjack game, solved by DP.
    void blackjack();
            
}


#endif // __DP_H_
