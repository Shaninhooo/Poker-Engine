#include <vector>  // Required for std::vector
#include <string>  // Required for std::string
#include <iostream>


class ValueTable {
private:

    std::vector<int> prime_values = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};
    std::vector<std::string> rank_names = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "JACK", "QUEEN", "KING", "ACE"};
    std::vector<std::string> suit_names = {"HEARTS", "SPADES", "DIAMONDS", "CLUBS"};
    std::vector<int> suit_masks = {0x1000, 0x2000, 0x4000, 0x8000};
    std::vector<int> deck;
public:
    ValueTable();
    ~ValueTable();

    void SetValue(int key, int value);
    int GetValue(int key) const;
}