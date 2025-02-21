#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Function declarations go at the top of the file so we can call them
// anywhere in our program, such as in main or in other functions.
// Most other function declarations are in the included header
// files.

// When you add a new helper function, make sure to declare it up here!

/**
 * Runs decryptSubstCipher() to get the cipher, then returns the score of that cipher
 * cipher is passed by reference, so that it can be used to decrypt the cipherText
 */
double decryptedSubstCipherScore(const QuadgramScorer& scorer, const string& ciphertext, vector<char>& cipher);

void decryptSubstCipherFile(const QuadgramScorer& scorer);

/**
 * Print instructions for using the program.
 */
void printMenu();

int main() {
  Random::seed(time(NULL));
  string command;

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  //Caesar Decrypt Command

  ifstream infile("dictionary.txt");
  vector<string> dict;
  string line;

  while (getline(infile, line)) {
    dict.push_back(line);
  }

  //Compute Englishness Command

  vector<string> quadgrams;
  vector<int> counts;
  ifstream quadgramsFile("english_quadgrams.txt");

  while (getline(quadgramsFile, line)) {
    quadgrams.push_back(line.substr(0, 4));
    counts.push_back(stoi(line.substr(5)));
  }

  QuadgramScorer scorer(quadgrams, counts);

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    // Adding option for "C - Encrypt with Caesar Cipher"
    if (command == "C" || command == "c") {
      caesarEncryptCommand();
    }

    if (command == "D" || command == "d") {
      caesarDecryptCommand(dict);
    }

    if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    }

    if (command == "E" || command == "e") {
      computeEnglishnessCommand(scorer);
    }

    if (command == "S" || command == "s") {
      decryptSubstCipherCommand(scorer);
    }

    if (command == "F" || command == "f") {
      decryptSubstCipherFile(scorer);
    }

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  // TODO: student
  int found = ALPHABET.find(c);
  found = (found + amount) % 26;
  return ALPHABET[found];
}

string rot(const string& line, int amount) {
  // TODO: student
  string result = "";
  for (int i = 0; i < line.length(); i++) {
    if (isalpha(line[i])) {
      int found = ALPHABET.find((char) toupper(line[i]));
      found = (found + amount) % 26;
      result += ALPHABET[found];
    }
    else if (isspace(line[i])) {
      result += " ";
    }
    else {
      continue;
    }
  }
  return result;
}

void caesarEncryptCommand() {
  // TODO: student
  string inputText;
  string amount;
  string encryptedText;

  cout << "Input the text that you want to be encrypted:" << endl;
  getline(cin, inputText);
  cout << "Input the amount you want to rotate:" << endl;
  getline(cin, amount);

  encryptedText = rot(inputText, stoi(amount));

  cout << "Your encrypted text is:" << endl;
  cout << encryptedText << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  // TODO: student
  for (int i = 0; i < strings.size(); i++) {
    strings.at(i) = rot(strings.at(i), amount);
  }
}

string clean(const string& s) {
  // TODO: student
  string result = "";
  for(int i = 0; i < s.size(); i++) {
    if (isalpha(s[i])) {
      result += (char) toupper(s[i]);
    }
  }
  return result;
}

vector<string> splitBySpaces(const string& s) {
  // TODO: student
  vector<string> words;
  string currentWord;
  bool isAWord = false;

  for (int i = 0; i < s.size(); i++) {
    if (isalpha(s[i]) && !isAWord) {        //if the current char isAlpha and !isAWord, then a new word is reached
      isAWord = true;                       //set isAWord to be true
      currentWord += s[i];                  //append char to currentWord
      if (i == s.size()-1) {                //edge case if index is on last char, add the currentWord to words
        words.push_back(currentWord);
      }
    }
    else if (isalpha(s[i]) && isAWord) {    //else if current char isAlpha and isAWord, then previous char is part of word
      currentWord += s[i];                  //append to currentWord
      if (i == s.size()-1) {                //edge case if index is on last char, add the currentWord to words
        words.push_back(currentWord);
      }
    }
    else if (!isalpha(s[i]) && isAWord) {   //else if current char !isAlpha (is a space) and isAWord, we reached end of word
      isAWord = false;                      //set isAWord to be false
      words.push_back(currentWord);         //add currentWord to vector
      currentWord = "";                     //reset currentWord to empty string
    }
  }

  return words;
}

string joinWithSpaces(const vector<string>& words) {
  // TODO: student
  string result;

  for (int i = 0; i < words.size(); i++) {
    result += words.at(i);
    if (i != words.size() - 1) {
      result += " ";
    }
  }

  return result;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  // TODO: student
  int count = 0;

  for (int i = 0; i < words.size(); i++) {
    for (int k = 0; k < dict.size(); k++) {
      if (words.at(i) == dict.at(k)) {
        count++;
        break;
      }
    }
  }

  return count;
}

void caesarDecryptCommand(const vector<string>& dict) {
  // TODO: student
  string text;
  string output;
  vector<string> words;
  vector<string> originalWords;
  int numWords;

  getline(cin, text);
  words = splitBySpaces(text);

  for (int i = 0; i < words.size(); i++) {
    words.at(i) = clean(words.at(i));
  }

  originalWords = words;              //copy of originalWords so that we can get to all cipher decryptions

  for (int i = 0; i < 26; i++) {
    words = originalWords;
    rot(words, i);
    numWords = numWordsIn(words, dict);
    if (numWords > words.size()/2) {
      output = joinWithSpaces(words);
      cout << output << endl;
    }
  }

  if (output.length() == 0) {
    cout << "No good decryptions found";
  }

}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  // TODO: student
  string newStr;

  for (int i = 0; i < s.length(); i++) {
    char currentChar = s[i];
    if (isalpha(currentChar)) {
      currentChar = toupper(currentChar);
      int found = ALPHABET.find(currentChar);
      newStr += cipher.at(found);
    }
    else {
      newStr += currentChar;
    }
  }

  return newStr;
}

void applyRandSubstCipherCommand() {
  // TODO: student
  vector<char> cipher = genRandomSubstCipher();
  string text;
  string newText;

  getline(cin, text);

  newText = applySubstCipher(cipher, text);

  cout << newText;
  
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  // TODO: student
  double score = 0.0;

  for (int i = 0; i < s.length()-3; i++) {
    score += scorer.getScore(s.substr(i, 4));
  }

  return score;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  // TODO: student
  string line;

  getline(cin, line);
  line = clean(line);

  cout << scoreString(scorer, line);
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  // TODO: student
  vector<char> cipher = genRandomSubstCipher();
  int count = 0;
  string decryptedText = applySubstCipher(cipher, ciphertext);
  double score = scoreString(scorer, decryptedText);

  while (count < 1000) {
    vector<char> newCipher = cipher;      //get a new key, copy of old key
    double oldScore = score;              //get the value of the previous score to compare

    int randOne = Random::randInt(25);    //generate two random integers from 0-25
    int randTwo = Random::randInt(25);
    while (randTwo == randOne) {
      randTwo = Random::randInt(25);
    }

    char randOneChar = newCipher.at(randOne);  //get the two chars at these random indexes
    char randTwoChar = newCipher.at(randTwo);

    newCipher.at(randOne) = randTwoChar;       //swap both of them to create a new cipher key
    newCipher.at(randTwo) = randOneChar;

    decryptedText = applySubstCipher(newCipher, ciphertext); //decrypt the text with the new key
    double newScore = scoreString(scorer, decryptedText);           //get score of decrypted text

    if (newScore > oldScore) {
      cipher = newCipher;
      score = newScore;
      count = 0;
    }
    else {
      count++;
    }
  }

  return cipher;
}

double decryptedSubstCipherScore(const QuadgramScorer& scorer, const string& ciphertext, vector<char>& cipher) {
  cipher = decryptSubstCipher(scorer, ciphertext);
  string decryptedText = applySubstCipher(cipher, ciphertext);
  double score = scoreString(scorer, decryptedText);

  return score;
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  // TODO: student
  string cipherText;
  string decryptedText;
  string origCipherText;
  double score;
  vector<char> cipher;
  vector<char> newCipher;

  getline(cin, cipherText);

  origCipherText = cipherText;

  cipherText = clean(cipherText);

  score = decryptedSubstCipherScore(scorer, cipherText, cipher);  //run once

  for (int i = 0; i < 50; i++) {                           //run 25 more times to get best score
    double newScore = decryptedSubstCipherScore(scorer, cipherText, newCipher);
    if (newScore > score) {
      score = newScore;
      cipher = newCipher;
    }
  }

  decryptedText = applySubstCipher(cipher, origCipherText);

  cout << decryptedText;
}

void decryptSubstCipherFile(const QuadgramScorer& scorer) {
  string inFileName;
  string outFileName;
  string line;
  string encryptedText;
  string origEncryptedText;
  string decryptedText;
  double score;
  vector<char> cipher;
  vector<char> newCipher;

  cout << "Input File: ";
  getline(cin, inFileName);
  ifstream inFile(inFileName);

  cout << "Output File: ";
  getline(cin, outFileName);
  ofstream outFile(outFileName);

  while(getline(inFile, line)) {
    encryptedText += line;
    encryptedText += "\n";
  }

  origEncryptedText = encryptedText;

  encryptedText = clean(encryptedText);

  score = decryptedSubstCipherScore(scorer, encryptedText, cipher);

  for(int i = 0; i < 25; i++) {
    double newScore = decryptedSubstCipherScore(scorer, encryptedText, newCipher);
    if (newScore > score) {
      score = newScore;
      cipher = newCipher;
    }
  }

  decryptedText = applySubstCipher(cipher, origEncryptedText);

  outFile << decryptedText;
}

#pragma endregion SubstDec
