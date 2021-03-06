#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <algorithm> //only for uppercase

using namespace std::chrono;
using namespace std;

//id function takes a string and creates a key
long id(string const&str, long const&MAX) {
	long key = 0;
	int const SIZE = str.size();
	long i = 0;
	for (; i < SIZE; i++) {
		key += static_cast<long>(str[i]);
		key *= static_cast<long>(str[i]);
	}
	if (key < 0) {
		return (-key) % MAX;
	}
	return key % MAX;
}

//deletes special charachters
//if there are more than 1 special charachter return false
bool Rstring(string &str) {
	int SIZE = str.size();
	int stopwords = 0;
	int stop = false;
	for (int i = 0; i < SIZE; i++) {
		int low = static_cast<int>(str[i]);
		if ((low < 65 || low>90)) {
			if (low == 39 && stop == false) {
				stop = true;
			}
			else {
				stopwords++;
				str.erase(i, 1);
				i--;
				SIZE--;
			}
			if (stopwords > 1) {
				return false;
			}
		}
	}
	return true;
}
class hashmap;

class linkedhash {
	friend class hashmap;//make hashmap class friend because of easy use
private:
	int count;//presents how many words there are in node
	string word;
	linkedhash *next;
public:
	linkedhash(string const &str) {//when we are creating new node we are using only constructor
		this->count = 1;//when create a new node it should start with 1
		this->word = str;
		this->next = NULL;
	}

};

class hashmap {
private:
	long MAX;//table size
	linkedhash **table;
	long say;
	long collision;
public:
	//first all table points NULL
	hashmap(long maxs = 5000) :MAX(maxs), say(0), collision(0) {
		table = new linkedhash*[MAX];
		for (int i = 0; i < MAX; i++) {
			table[i] = NULL;
		}
	}

	//prints how many times keys collide
	/*void collid() {
		cout << "collision " << collision << endl;
	}*/

	//prints how much different words there are
	/*void sayac() {
		cout << "there are " << say << " different words" << endl;
	}*/

	//finds whether the word exist or not in hashmap
	bool find(string const &word) {
		long hash = id(word, MAX);
		if (table[hash] == NULL) {
			return false;
		}
		else {
			linkedhash *temp = table[hash];
			while (temp != NULL) {
				if (temp->word == word) {
					return true;
				}
				temp = temp->next;
			}
			return false;
		}
	}

	//adds the word
	void put(string const &word) {
		long hash = id(word, MAX);

		//if hash  location already points NULL
		//create a new node and add there
		if (table[hash] == NULL) {
			table[hash] = new linkedhash(word);
			say++;
		}
		//if hash location is not NULL
		//we have to look all nodes that presents same hash value
		else {
			linkedhash *temp = table[hash];
			linkedhash* prev = NULL;
			bool happen = false;//whether we found what we are looking for or not
			while (temp != NULL) {//while end of nodes that presents same hash value
				if (temp->word == word) {//if we found what we are looking for
					happen = true;										//make this true so we wont create new node
					temp->count++;//increase the number of that word
					break;
				}
				prev = temp;
				temp = temp->next;
			}
			if (happen == false) {//if we didnt find the word in nodes
				collision++;
				temp = new linkedhash(word);//create new node
				prev->next = temp;//connect with previous
				say++;
			}
		}
	}

	void most10() {
		linkedhash *temp;
		linkedhash *mostptr[10];//keeps most ten used words memory location
		int counter[10] = { 0 };//keeps counts
		int lowest = 0;//keeps lowest index
		for (int i = 0; i < MAX; i++) {
			if (table[i] == NULL) {}//if table is already NULL do noting
			else {
				temp = table[i];//look same hash location
				while (temp != NULL) {//look all nodes that has same hash value
					if (temp->count > counter[lowest]) {//if node's count is greater than lowest in array replace them
						mostptr[lowest] = temp;
						counter[lowest] = temp->count;
						for (int i = 0; i < 10; i++) {
							if (counter[i] < counter[lowest]) {
								lowest = i;
							}
						}
					}
					temp = temp->next;//go next node
				}

			}
		}
		cout << "_____TOP 10 WORDS USED IN TWEETS_____" << endl;
		cout << endl;
		for (int i = 0; i < 10; i++) {//print most used ten words
			cout << mostptr[i]->word << " " << counter[i] << " times" << endl;
		}
	}

	//deletes all table and nodes
	~hashmap() {
		for (long i = 0; i < MAX; i++) {
			if (table[i] != NULL) {
				linkedhash *prev = NULL;
				linkedhash *entry = table[i];
				while (entry != NULL) {
					prev = entry;
					entry = entry->next;
					delete prev;
				}
			}
		}
		delete[] table;
	}
};

int main()
{
	auto start = high_resolution_clock::now();
	hashmap ignore_map;
	ifstream ignorfile("stopwords.txt");
	if (ignorfile.is_open()) {//here we are adding stopwords to ignore_map
		while (!ignorfile.eof()) {
			string tempword;
			getline(ignorfile, tempword, '\n');
			transform(tempword.begin(), tempword.end(), tempword.begin(), ::toupper);
			ignore_map.put(tempword);
		}
		ignore_map.put("");//beacuse we erase some words there might be an empty string so we shoud add empty string to hashmap
		cout << "There was no problem in opening the file. Your file is currently being scanned. Please wait." << endl;
		cout << endl;
	}
	else {
		cerr << "Oops! Something went wrong while opening the ignore file" << endl;
	}
	ignorfile.close();

	long x = 0;
	hashmap hm(700000);
	string clm1;
	string clm2;
	string clm3;
	string clm4;
	string clm5;
	string temp;
	ifstream myfile;
	myfile.open("training.1600000.processed.noemoticon.csv");
	if (myfile.is_open()) {
		while (!myfile.eof()) {
			getline(myfile, clm1, ',');
			getline(myfile, clm2, ',');
			getline(myfile, clm3, ',');
			getline(myfile, clm4, ',');
			getline(myfile, clm5, ',');
			getline(myfile, temp, '\n');//go to 6th line

			istringstream buf(temp);//6th line whole sentence to object buf
									if (x % 1000 == 0) {
									auto stop = high_resolution_clock::now();
									auto duration = duration_cast<seconds>(stop - start);
									//cout << duration.count() << "seconds" << endl;
									//cout << "Reading " << x << " th line" << endl;
									//cout << "Up to now ";
									//hm.sayac();
									//hm.collid();
									}
									x++;
			string i;
			for (; buf >> i;) {//sentence to words
				transform(i.begin(), i.end(), i.begin(), ::toupper);
				if (Rstring(i)) {
					if (!ignore_map.find(i)) {//if it is in ignore map dont add it
						hm.put(i);

					}
				}
			}
		}
		hm.most10();
		//hm.collid();
		//hm.sayac();
		myfile.close();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<seconds>(stop - start);
		cout << endl; 
		cout << duration.count() << " seconds" << endl;
	}
	else {
		cerr << "Oops! Something went wrong" << endl;
	}
	system("pause");
	return 0;
}

