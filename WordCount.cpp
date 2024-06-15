// File name: WordCount.cpp
// Author: Hanchu Shen
// userid: shenh2
// Email: hanchu.shen@vanderbilt.edu
// Class: CS2201 @ Vanderbilt U.
// Assignment Number: project8
// Description:  count words of texts for comparison
// Honor statement:I have not given or received unauthorized aid on this assignment.
// Last Changed: 2019/11/13


#include <cctype>
#include <cstdlib>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>


const double MINIMUM_APPEARANCE = 0.001;
const int STEM_LENGTH = 6;

//DO NOT CHANGE THESE
typedef std::set<std::string> WordSet;
typedef std::map<std::string, int> WordCountMap;


// function prototypes
// you are not allowed to change these function headers,
// but you can add helper functions as needed.
WordSet readCommonWords(const std::string &fname);

WordCountMap processFile(const std::string &fname, const WordSet &common);

double compareTexts(WordCountMap &first,
                    WordCountMap &second,
                    const std::string &fname);

//helper methods
double getSum(WordCountMap &map);

bool isValidScore(double b1, double b2);

std::string convertStr(const std::string &str);

/* main */
int main() {
    std::string commonFile = "texts/common.txt";
    std::string fin1, fin2, fout, str;

    do {
        std::cout << "Enter name of the first input file: ";
        std::cin >> fin1;
        fin1 = "texts/" + fin1;

        std::cout << "Enter name of the second input file: ";
        std::cin >> fin2;
        fin2 = "texts/" + fin2;

        std::cout << "Enter name of the output file: ";
        std::cin >> fout;
        fout = "texts/" + fout;

        WordSet common = readCommonWords(commonFile);

        WordCountMap first = processFile(fin1, common);
        WordCountMap second = processFile(fin2, common);

        double dist = compareTexts(first, second, fout);

        std::cout << "Vector Distance: " << dist << std::endl;

        std::cout << "Would you like to run the program again (Y|N): ";
        std::cin >> str;
    } while (tolower(str[0]) == 'y');


    return 0;
}

/* readCommonWords
 * This method reads words from a given file and places them
 * into a WordSet, which it returns.
 *
 * pre:  a valid file name with 1 word per line, words all in lower case
 * post: all words int the file are placed in the WordSet
 */
WordSet readCommonWords(const std::string &fname) {
    WordSet ret;
    std::string str;
    std::ifstream file(fname.c_str());
    if (!file) {//check validity of input file
        std::cout << "Unable to open file. Press enter to exit program.";
        getline(std::cin, str);  // consume existing line
        std::cin.get();  // get the key press
        exit(1);
    }
    while (getline(file, str)) {//construct WordSet
        ret.insert(str);
    }
    file.close();
    return ret;
}


/* processFile
 * This function reads in all words from the given file
 * after reading a word it converts it to lower case,
 * removes non alphabetic characters and stems it to STEM_LENGTH.
 * If the resulting word is considered common it is ignored.
 * Otherwise, the count in the map that matches the word
 * is incremented.
 *
 * pre:  the name of a text file and a set of words to be ignored.
 * post: The file has been read; a map of cleansed words is created and returned
 *
 */
WordCountMap processFile(const std::string &fname, const WordSet &common) {
    WordCountMap ret;
    std::string str;
    std::ifstream file(fname.c_str());
    if (!file) {//check validity of input file
        std::cout << "Unable to open file. Press enter to exit program.";
        getline(std::cin, str);  // consume existing line
        std::cin.get();  // get the key press
        exit(1);
    }
    while (file >> str) {//construct WordCountMap object
        str = convertStr(str);
        if (!str.empty() && common.find(str) == common.end()) {
            if (ret.find(str) == ret.end()) {//add new pair to map
                ret[str] = 1;
            } else {
                ret[str]++;//increase count of existing word
            }
        }
    }
    file.close();
    return ret;
}


/* compareTexts
 * Compares the count maps of 2 texts.
 * The result returned is the euclidean distance
 * where each word with a percentile appearance greater than
 * MINIMUM_APPEARANCE counts as a dimesion of the vectors.
 * (they still count toward the full sum used to calculate
 *  percentile appearance)
 *
 * This method also prints out the results to a file using the following
 * format (start and end tags added for clarity):
 * / -- start example output --/
 * word1:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 * word2:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 * ...
 * wordN:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 *
 * Counted words in document 1 = xxxx
 * Counted words in document 2 = xxxx
 *
 * Vector Distance: dist
 * /-- end example output --/
 *
 * word1-wordN are all words whose comparison is include in the final sum.
 * distance values are the square of the difference of the percentile
 *                         scores from the two maps.
 * score1 & score2 values are the scores from the two maps.
 * dist is the final distance that is also returned,
 *    it is the square root of the sum of all the individual distances.
 * To help line things up, there is a tab character after the ":" and another
 *    before the "(" on the line for each word.
 *
 * pre:  two maps of texts to be compared and the name of an output file
 * post: returns the euclidean distance of the two files
 *       if the output file cannot be opened, the method prints
 *       an error mesage and exits the program.
 */
double compareTexts(WordCountMap &first,
                    WordCountMap &second,
                    const std::string &fname) {
    std::ofstream outfile;
    outfile.open(fname.c_str());
    if (outfile.fail()) {//check validity of output file
        std::cout << "Unable to open file3. Press enter to exit program.";
        exit(1);
    }
    //declare variables
    double totalSum = 0;
    double sum1 = getSum(first);
    double sum2 = getSum(second);
    std::string word;
    WordCountMap::const_iterator iter;
    //calculate and print valid distances
    for (iter = first.begin(); iter != first.end(); ++iter) {
        word = iter->first;
        if (second.find(word) != second.end()) {//word exits in both objects
            auto score1 = (double) first[word] / sum1;
            auto score2 = (double) second[word] / sum2;
            if (isValidScore(score1, score2)) {
                auto distance = (score1 - score2) * (score1 - score2);
                outfile << word << ":\tdistance = " << distance
                        << "\t(score1 = " << score1 << ", score2 = " << score2 << ")" << std::endl;
                totalSum += distance;
            }
        }
    }
    outfile << "\nCounted words in document 1 = " << sum1
            << "\nCounted words in document 2 = " << sum2 << std::endl;
    outfile << "\nVector Distance:\t" << sqrt(totalSum) << std::endl;
    return sqrt(totalSum);
}

/*
 * convertStr
 * pre: str is a valid string object
 * post: return the string of valid chars with length less than 6
*/
std::string convertStr(const std::string &str) {
    std::string newStr;
    for (char c : str) {
        if (isalpha(c)) {
            newStr += tolower(c);
        }
    }
    if (newStr.length() > STEM_LENGTH) {
        newStr = newStr.substr(0, STEM_LENGTH);
    }
    return newStr;
}

/*
 * isValidScore
 * pre: b1 and b2 are double type
 * post: return if both doubles are less than minumum
 * return false otherwise
*/
bool isValidScore(double b1, double b2) {
    return b1 > MINIMUM_APPEARANCE && b2 > MINIMUM_APPEARANCE;
}

/*
 * getSum
 * pre: map is a valid WordCountMap object
 * post: return sum of all counts in map
*/
double getSum(WordCountMap &map) {
    double sum = 0;
    WordCountMap::const_iterator iter;
    for (iter = map.begin(); iter != map.end(); iter++) {
        sum += iter->second;
    }
    return sum;
}
