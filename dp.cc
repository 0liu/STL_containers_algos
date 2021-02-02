/*
** Dynamic Programming exercises.
 */

#include "dp.hh"
#include <iostream> 
#include <vector>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <limits>
#include <numeric>
#include <ctime>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;


namespace DP {

    // Recursive DP & memoizing to compute Fibonacci numbers.
    unsigned fib(unsigned n) {
        static vector<int> memo {0, 1, 1};
        unsigned f;
        if (n >= memo.size() || memo[n] == 0) {
            f = fib(n - 1) + fib(n - 2);
            memo[n] = f;
        }
        return memo[n];
    }

    // Bottom-up DP to compute Fibonacci numbers.
    unsigned fib_bottomup(unsigned n) {
        vector<int> tbl {0, 1, 1};
        for (unsigned i = 3; i <= n; i++)
            tbl[i] = tbl[i-1] + tbl[i-2]; 
        return tbl[n];
    }

    TextJustify::TextJustify(const string& text_file, unsigned page_width) {
        read_text(text_file);
        m_page_width = std::max(1U, page_width);
    }

    void TextJustify::read_text(const string& text_file) {
        std::ifstream text {text_file};
        string word;
        while (text >> word)
            m_words.push_back(word);
        m_dp.clear();
        m_parents.clear();
    }

    double TextJustify::badness(unsigned i, unsigned j) {
        /*
        ** The cost function of how bad to use words[i, j] as a line.
        ** Assume 0 <= i <= j <= n.
        ** badness(i, j) = (page width - total width) ^ 3, if fit.
        **              or INF, if don't fit.
        */
        unsigned total_width;
        for (size_t idx = i; idx < j; idx++)
            total_width += m_words[idx].length();
        if (total_width > m_page_width)
            return std::numeric_limits<double>::infinity();
        else
            return std::pow(m_page_width - total_width, 3); 
    }

    void TextJustify::justify(unsigned page_width=0) {
        /*
        ** Justify words by bottom-up dynamic programming table.
        ** Second line starts at j-th word, j is from 1 to n.
        */
        if (m_words.empty())  // no text
            return;
        if (!m_dp.empty() and !m_parents.empty() and m_page_width == page_width)
            return;  // text has been justified
        if (page_width != 0)
            m_page_width = page_width;
        unsigned n = m_words.size();
        m_dp.clear();
        m_dp.assign(n+1, std::numeric_limits<double>::infinity());  // index 0 to n
        m_dp[n] = 0;  // base case
        m_parents.clear();
        m_parents.assign(n, n);  // parent pointers. possible values: 1 to n.
        // solve subproblem from n-1 to 0.
        for (int i = n-1; i >= 0; --i) {
            vector<double> guesses;
            for (size_t j = i+1; j <= n; ++j)
                guesses.push_back(m_dp[j] + badness(i, j));
            auto it_min = min_element(guesses.begin(), guesses.end());
            m_dp[i] = *it_min;
            m_parents[i] = i + 1 + (it_min - guesses.begin());
        }
    }

    void TextJustify::print_justified() {
        if (m_words.empty()) {
            cout << "Empty Text." << endl;
            return;
        }
        if (m_dp.empty() || m_parents.empty()) {
            cout << "Text not justified." << endl;
            return;
        }
        string justified;
        vector<double> cost;
        unsigned i = 0;
        while (i < m_words.size()) {
            unsigned j = m_parents[i];
            std::ostringstream oss;
            std::copy(m_words.begin() + i, m_words.begin() + j,
                      std::ostream_iterator<string>(oss, " "));
            justified += oss.str() + "\n";
            cost.push_back(m_dp[i]);
            i = j;
        }
        cout << "Justified text:" << endl;
        cout << justified << endl;
        cout << "Cost of each line:" << endl;
        for (size_t i = 0; i < cost.size() - 1; i++)
            cout << cost[i] - cost[i+1] << ", ";
        cout << cost.back() << endl;
    }

    int blackjack_dp(int i, vector<int>& deck) {
        /*
        ** After i cards played, find the max possible winnings by DP.
        */
        int n = deck.size();
        if (n - i < 4)
            return 0;  // base case: not enough cards.
        
        vector<int> options;
        for (int p = 2; p <= n-i-2; ++p) {  // Number of cards taken by player
            // run player strategy
            vector<int> player_cards;
            player_cards.push_back(deck[i]);
            player_cards.push_back(deck[i+2]);
            for (int j = i+4; j < i + 2 + p; ++j)
                player_cards.push_back(deck[j]);
            int ace = std::count(player_cards.begin(), player_cards.end(), 1);
            int player_score = std::accumulate(player_cards.begin(), player_cards.end(), 0);
            // decide if ace is 1 or 11 for maximum score
            while (ace > 0 && (21 - player_score) / 10 > 0) {
                player_score += 10;
                --ace;
            }
            
            // player bust case
            if (player_score > 21) {
                options.push_back(-1 + blackjack_dp(i+2+2, deck));
                break;
            }

            // run dealer strategy
            int dealer_score;
            int d;
            for (d = 2; d <= n-i-p; ++d) {
                vector<int> dealer_cards;
                dealer_cards.push_back(deck[i+1]);
                dealer_cards.push_back(deck[i+3]);
                for (int j = i+p+2; j < i+p+d; ++j)
                    dealer_cards.push_back(deck[j]);
                dealer_score = std::accumulate(dealer_cards.begin(), dealer_cards.end(), 0);
                dealer_score += 10 * std::count(dealer_cards.begin(), dealer_cards.end(), 1);
                if (dealer_score >= 17)
                    break;
            }
            if (dealer_score > 21)  // dealer bust
                dealer_score = 0;
                
            // compute recurrence
            int profit_loss = (player_score > dealer_score) - (dealer_score > player_score);  // could be 0 for tie
            options.push_back(profit_loss + blackjack_dp(i+p+d, deck));
        }

        return *std::max_element(options.begin(), options.end());
    }
    
    void blackjack() {
        /*
        ** Perfect-information blackjack game, solved by recursive DP.
        */
        vector<int> deck;
        for (int suit = 0; suit < 4; suit++)
            for (int i = 1; i < 14; i++)
                deck.push_back(i);
        std::srand(std::time(0));
        std::random_shuffle(deck.begin(), deck.end());
        cout << "Deck:" << endl;
        for (auto const& card : deck)
            cout << card << ", ";
        cout << endl;
        cout << "Max profit: " << blackjack_dp(0, deck) << endl;
    }

}

unsigned ask_fib_n() {
    unsigned n; 
    while (true) {
        cout << "Please input a positive integer n to compute Fibonacci(n):" << endl;
        cin >> n;
        if (cin.good() && n > 0)
            break;
        cin.clear();
        cin.ignore(INT_MAX, '\n');
    }
    return n;
}

int main(int argc, char** argv) {
    cout << endl;
    auto test_n = std::strtol(argv[1], nullptr, 10);
    if (argc <= 1 || test_n < 0 || test_n > 3) {
        cout << "Select a test between 0 and 3 as the argument." << endl;
        cout << "0 - Fibonacci recusive and memoizing." << endl;
        cout << "1 - Fibonacci bottom up." << endl;
        cout << "2 - Text justificaiton." << endl;
        cout << "3 - Blackjack." << endl << endl;
        return 0;
    }

    switch (test_n) {
        case 0: {
            unsigned n = ask_fib_n();
            cout << "Recursive fib(" << n << ") = " << DP::fib(n) << endl;
            break;
        }
        case 1: {
            unsigned n = ask_fib_n();
            cout << "Bottom-up fib(" << n << ") = " << DP::fib_bottomup(n) << endl;
            break;
        }
        case 2: {
            DP::TextJustify tj("news.txt", 80);
            tj.justify();
            tj.print_justified();
            break;
        }
        case 3:
            DP::blackjack();
    }
    return 0;
}
