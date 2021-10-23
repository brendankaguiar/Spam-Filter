#include <iostream>
#include <string>
#include <fstream>
#include <map>
const int COLS = 916;//max cols for words in each doc of spam.csv
const int ROWS = 3902;//rows of spam.csv
static int HAM_DCT = 0;//count of documents associated with HAM class
static int SPAM_DCT = 0;//count of documents associated with SPAM class
static int COUNT[2] = { 0, 0 };//SPAM TOTAL, HAM TOTAL
const int size = 390;
float M[size][2];//Conditional Probability on MDictionary of 

						  //Dictionary that uses words as indeces to keep track of count
std::map<std::string, unsigned int> SPAMDictionary, HAMDictionary;
std::map<std::pair<std::string, std::string>, unsigned int> MDictionary;//Shared Dictionary
struct DOCUMENT{
	std::string Class;
	std::string Text[175];
	void setClass(std::string parser)
	{
		Class = parser;
		if (Class == "ham")
			HAM_DCT++;
		else
			SPAM_DCT++;
	}
	void setText(std::string parser, int i)
	{
		if (Class == "spam")
		{
			if (!SPAMDictionary[parser])
				SPAMDictionary[parser] = 1;//add count to new indexed word
			else
				SPAMDictionary[parser] += 1;//increment indexed word count
		}
		else
		{
			if (!HAMDictionary[parser])
				HAMDictionary[parser] = 1;//add count to new indexed word
			else
				HAMDictionary[parser] += 1;//increment indexed word count
		}
	}
};
int load_docs(std::string filename, DOCUMENT DOCS[]);
void getMDictionary();
void getM(float z);
int printTrainingData(std::string fname1, std::string fname2, float p_of_x);

int main() 
{
	DOCUMENT* myDoc = new DOCUMENT[3902];
	int val1 = load_docs("spam.csv", myDoc);
	if (val1 == 0)
		std::cout << "Document Successfully loaded\n";
	getMDictionary();//build the Shared Dictionary
	float P_of_HAM = (float)HAM_DCT / (HAM_DCT + (float)SPAM_DCT);
	float P_of_SPAM = (float)1 - P_of_HAM;
	unsigned int L_of_V = MDictionary.size();//length of shared Dictionary/Vocabulary
	getM((float)L_of_V);//M[size][2] rows = word, cols = class, val = occurences
	int val2 = printTrainingData("spamtraining.txt", "hamtraining.txt", P_of_HAM);
	if (val2 == 0)
		std::cout << "Document Successfully Saved\n";
	return 0;
}

/* Prints data into "classtraining.txt" in the following format:
* P(class)
* word0,P(word0|class)
* ...
* wordn,P(wordn|class)
* */
int printTrainingData(std::string spamf, std::string hamf, float p_of_x)
{
	std::ofstream bfile(spamf.c_str());
	std::ofstream cfile(hamf.c_str());
	cfile << p_of_x << '\n';//store P(ham)
	bfile << 1 - p_of_x << '\n';//store P(spam)
	if (bfile.is_open() && cfile.is_open())
	{
		int k = 1;
		int i = 0;
		for (const auto& entry : MDictionary)
		{
			if (k)
			{
				bfile << entry.first.first << "," << M[i][0] << '\n';
				cfile << entry.first.first << "," << M[i][1] << '\n';
				i++;
			}
			k = !k;//flip k to skip repeat entries
		}
		bfile.close();
		cfile.close();
	}
	else
	{
		std::cout << "unable to open training files\n";
		return -1;
	}

	return 0;
}
/*
* Get Conditional Probabilities for all words in both classes
For Each word i in DOC
  M[i][1] = P(word|SPAM) = (Count(word, SPAM) + 1) / ( TOTAL_COUNT(SPAM) + |V|)
  M[i][2] = P(word|HAM) = (Count(word, HAM) + 1) / ( TOTAL_COUNT(HAM) + |V|)
  auto = std::map<std::pair<std::string, std::string>, unsigned int>::iterator
*/
void getM(float z)
{
	int j = 0;
	int k = 0;
	for (const auto& entry : MDictionary)
	{
		//order is SPAM0 -> HAM0 -> SPAM1 -> HAM1 -> ... -> SPAMn -> HAMn
		M[j][k] = ((float)MDictionary.at(entry.first) + (float)1) / ((float)COUNT[k] + z);//compute conditional probability of HAM class 
		if (k)
			j++;
		k = !k;//flip index for each class
	}
}
//Get Shared Dictionary of SPAM and HAM Class
void getMDictionary()
{
	std::map<std::string, unsigned int>::iterator i = HAMDictionary.begin();
	i++;//avoid garbage
	for (i; i != HAMDictionary.end(); i++)
	{
		if (i->second >= 5)//few or more occurences
		{
			if (SPAMDictionary.find(i->first) != SPAMDictionary.end())
			{
				int j = SPAMDictionary.find(i->first)->second;//assign occurences in spam the passed ham word
				if (j >= 5)//if spam also has few or more occurences
				{
					//Combine into Shared Dictionary M["word"]["class"] = val
					MDictionary[std::make_pair(i->first, "spam")] = j;//assign spam val
					MDictionary[std::make_pair(i->first, "ham")] = i->second;//assign ham val
					COUNT[0] += j;//increment total words in spam class 
					COUNT[1] += i->second;//increment total words in ham class
				}
			}
		}
	}
}
int load_docs(std::string filename, DOCUMENT DOCS[])
{
	std::ifstream bfile(filename.c_str());
	int l = 0;//index for COlS
	if (bfile.is_open())
	{
		std::string parser;
		for (int i = 0; i < 3863; i++)// For Each document
		{
			std::string scanner;
			getline(bfile, parser, ',');
			DOCS[i].setClass(parser);//Set the class of the current document
			do//For Each word in document
			{
				if (bfile.peek() != '"')
				{
					parser.clear();
					scanner.clear();
					while (bfile.peek() != '\n' && bfile.peek() != ' ')
						parser.push_back(bfile.get());
					int garbage = bfile.get();
					if (parser.size() >= 3)
					{
						scanner = parser.substr(parser.size() - 3, parser.size());
						if (scanner == ",,,")//If scanner discovers terminal characters
							parser.erase(parser.size() - 3, 3);//erase terminal characters
					}

					DOCS[i].setText(parser, l);//add word to document vector
					l++;
				}
				else
					int garbage = bfile.get();//collect error prone symbols
			} while(scanner != ",,,");//until terminal characters found
			l = 0;//reset COLS for next Text document
		}
		bfile.close();
	}
	else
	{
		printf("unable to open file: [%s]\n", filename.c_str());
		return -1;
	}
	return 0;
}
