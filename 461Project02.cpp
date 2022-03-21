using namespace std;
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>

class packItem {
public:
	float value;
	float weight;
	bool selection;

	//Constructor to initialize the values when read from the .txt file provided 
	packItem(bool selection1, float value1, float weight1) {
		selection = selection1, value = value1, weight = weight1;
	}
};

void itemSwap(int vectorPos, vector<packItem> vec)
{
	int randNum = rand() % 400;
	if (vec.at(vectorPos).selection == 1)
	{
		vec.at(vectorPos).selection = 0;
		vec.at(vectorPos) = vec.at(randNum);
		vec.at(randNum).selection = 1;
	}
}

float returnTotalWeight(vector<packItem> vec)
{
	float totalWeight = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec.at(i).selection == 1)
		{
			totalWeight += vec.at(i).weight;
		}
	}
	return totalWeight;
}

float returnTotalValue(vector<packItem> vec)
{
	float totalValue = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec.at(i).selection == 1)
		{
			totalValue += vec.at(i).value;
		}
	}
	return totalValue;
}

int returnTotalItems(vector<packItem> vec)
{
	int totalItems = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec.at(i).selection == 1)
		{
			totalItems++;
		}
	}
	return totalItems;
}

int main() {

	//Variable declaration
	string fileText;
	string weight;
	string value;
	const int maxWeight = 500;
	vector<packItem> itemVec;

	//random seed for first selection of 20 variables
	srand(time(NULL));

	//Access file you need to open
	cout << "Please enter the file with the data for simulated annealing." << endl;
	cin >> fileText;

	//Creating input and output streams
	ifstream inputFile;
	inputFile.open(fileText);
	ofstream outputText;
	outputText.open("Output.txt");

	if (inputFile.bad() == true)
		cout << "Error loading file. Please check and try again." << endl;

	else {
		cout << "File opening succesful, transferring and evaluating data." << endl;

		for (int i = 0; i < 400; i++) {

			//Floats get read in as a string and requires a conversion, I don't know much about this the 
			//behind the scenes work of this but used this resource: https://www.programiz.com/cpp-programming/string-float-conversion (this took way too long to figure out)
			//Reading data from file into a vector proved to be more difficult than it should...
			inputFile >> value >> weight;
			float tempValue = stof(value);
			float tempWeight = stof(weight);
			//Creating a single class instance to put into the vector was the key
			packItem itemTemp{ 0, tempValue, tempWeight };
			itemVec.push_back(itemTemp);
		}
	}

	//For selection of first random twenty (1/20 * 400)
	float totalValue = 0, totalWeight = 0;
	for (int i = 0; i < 20; i++)
	{
		int randNum = rand() % 399;
		totalValue += itemVec.at(randNum).value;
		totalWeight += itemVec.at(randNum).weight;
		itemVec[randNum].selection = 1;
	}

	outputText << "First 20 Totals - Value: " << totalValue << ", Weight: " << totalWeight << endl;

	//Annealing Variables
	float const initValue = 1000;
	int successfulChanges = 0;
	int totalAttempts = 0;
	double initTemp = 500;
	//Alpha is multiplied against temperature
	double alpha = .999;
	//epsilon is used to know when our temperature is completely cooled
	double epsilon = .001;


	while (initTemp > epsilon)
	{

		//The basis of the algorithm is to either accept a new item if it increaes our value without going over our weight
		//Must have current totals before any math happens so we know if the addition is profitable or not
		float weightBefore = returnTotalWeight(itemVec);
		float valueBefore = returnTotalValue(itemVec);
		float weightAfter = 0;
		float valueAfter = 0;
		int totalItems = returnTotalItems(itemVec);
		int randNum = rand() % 400;

		//Acceptance probability = e^-(delta(E) / T)), compare this by a random number from 0 to -1 according to my research to help the randomness of the algorithm https://www.machinelearningplus.com/machine-learning/simulated-annealing-algorithm-explained-from-scratch-python/
		//I think our "energy" in this case would be trying to minimize our given value of 1000 minus our current utility to find our "minimum"
		float valueWithNewItem = (returnTotalValue(itemVec));
		float weightWithNewItem = (returnTotalWeight(itemVec));

		//item is already selected
		//had an error where it would select every single item eventually so i tried changing this to deselect items ? im not sure if it helped
		if (itemVec.at(randNum).selection == 1)
		{
			itemVec.at(randNum).selection = 0;

			//Added penalty for being overweight
			if (returnTotalWeight(itemVec) > 500)
			{
				float penalty = ((returnTotalWeight(itemVec) - 500) * 20);
				valueAfter = (returnTotalValue(itemVec) - penalty);

				weightAfter = returnTotalWeight(itemVec);
				totalAttempts++;
			}

			else {
				weightAfter = returnTotalWeight(itemVec);
				valueAfter = returnTotalValue(itemVec);
				totalAttempts++;
			}

		}

		else {

			itemVec.at(randNum).selection = 1;
			if (returnTotalWeight(itemVec) > 500)
			{
				float penalty = ((returnTotalWeight(itemVec) - 500) * 20);
				valueAfter = (returnTotalValue(itemVec) - penalty);
				weightAfter = returnTotalWeight(itemVec);
				totalAttempts++;
			}

			else {
				weightAfter = returnTotalWeight(itemVec);
				valueAfter = returnTotalValue(itemVec);
				totalAttempts++;
			}
		}
			//this should follow up after the initial if statement was true
			//Acceptance probabilty (I've read both that you should divide by a random number and that you shouldn't so I'm not sure if I should be dividing this
			//acceptance number by another random number or not
			//this is our delta E over temperature function
			double acceptance = exp(-((1000 - returnTotalWeight(itemVec)) - (1000 - weightBefore)) / (initTemp));
			double randomAccept = (static_cast<float> (rand() / static_cast<float> (RAND_MAX)));
			//To measure if our "performance" has increased or not - if it hasn't then we branch to the acceptance probability 
			//We want our 1000 - valueAfter to be as small as possible 
			if ((1000 - valueAfter) < (1000 - valueBefore))
			{
				successfulChanges++;
			}

			else
			{
				if (randomAccept > acceptance)
				{
					itemVec.at(randNum).selection = 0;
					totalAttempts++;
				}

				else {
					itemVec.at(randNum).selection = 1;
					totalAttempts++;
				}
			}
		initTemp = (initTemp * alpha);
	}
	outputText << "Total value: " << returnTotalValue(itemVec) << "\nTotal weight: " << returnTotalWeight(itemVec) << "\nTotal items: " << returnTotalItems(itemVec) << endl;
	outputText << "\nTotal attempts: " << totalAttempts << "\nSuccessful changes: " << successfulChanges << endl;
	outputText.close();
	inputFile.close();
	return 0;
}




