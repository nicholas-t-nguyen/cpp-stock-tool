//A program that takes an input of stock tickers and outputs the price, open, high, and low for the stock.
//Includes the option to output information to a .txt

#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <sstream>
#include <cctype>
#include <string>

using namespace std;
using json = nlohmann::json;
//creating function to store data in memory
size_t writecallback(void *contents, size_t size, size_t nmemb, void *userdata) {
    ((string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    //Variables
    //initializing variables
    CURL *curl;
    CURLcode result;
    string base_url = "https://api.tiingo.com/iex/";
    string apikey = "";
    string input;
    string filename;
    //Inputs and Output (I/O)
    //Interaction
    //asking if data should be outputted into a file
    cout << "Output stock data to a .txt file? (y/n): ";
    char yn;
    cin >> yn;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //naming output file and checking if a .txt was included, and if not, then to add it
    if (yn == 'y' || yn == 'Y') {
        cout << "Enter the output file name: ";
        cin.ignore();
        getline(cin, filename);
        if (filename.find('.') == string::npos) {
            filename += ".txt";
        }
    }
    //inputting list of stock tickers to retrieve data
    cout << "Please enter the ticker(s) you would like to see, separated by spaces: ";
    getline(cin, input);
    //Arrays
    //creating array of tickers
    vector<string> elements;

    istringstream ss(input);
    string element;

    while (ss >> element) {
        transform(element.begin(), element.end(), element.begin(), ::toupper);
        elements.push_back(element);
    }
    //File I/O
    //Control
    //checking to see if output file is open, and if not then to return -1
    ofstream file;
    if (yn == 'y' || yn == 'Y') {
        file.open(filename);
        if (!file.is_open()) {
            cerr << "Failed to open the output file." << endl;
            return -1;
        }
    }
    //Iteration (loops)
    //the iteration of tickers to retrieve data and output
    for (const string& elem : elements) {
        //creating the correct url for each ticker to retrieve api data from
        string url = base_url + elem + "?token=" + apikey;
        //initializing curl, and if it fails then to return -1
        curl = curl_easy_init();
        if (curl == nullptr) {
            cerr << "HTTP request failed";
            return -1;
        }
        //setting options for curl, url and data writing functions/variable
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
        string tickerdata;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &tickerdata);
        //performing the curl request
        result = curl_easy_perform(curl);
        //checking to see if curl was ok, if not then to return -1
        if (result != CURLE_OK) {
            cerr << "Error: " << curl_easy_strerror(result);
            curl_easy_cleanup(curl);
            return -1;
        }
        //ending curl
        curl_easy_cleanup(curl);
        //parsing tickerdata that is currently in json format, and storing to jsondata
        json jsondata = json::parse(tickerdata);
        //Inputs and Outputs (I/O)
        //outputting ticker info
        cout << elem << endl;
        cout << "Data current as of: " << jsondata[0]["timestamp"] << endl;
        cout << "Price: " << jsondata[0]["tngoLast"] << endl;
        cout << "Open: " << jsondata[0]["open"] << endl;
        cout << "High: " << jsondata[0]["high"] << endl;
        cout << "Low: " << jsondata[0]["low"] << endl;
        //checking if user wanted to output data to file, and doing so if requested
        if (yn == 'y' || yn == 'Y') {
            file << elem << endl;
            file << "Data current as of: " << jsondata[0]["timestamp"] << endl;
            file << "Price: " << jsondata[0]["tngoLast"] << endl;
            file << "Open: " << jsondata[0]["open"] << endl;
            file << "High: " << jsondata[0]["high"] << endl;
            file << "Low: " << jsondata[0]["low"] << endl;
            file << endl;
        }
        //creating new line to format output
        cout << endl;
    }
    //closing file
    if (yn == 'y' || yn == 'Y') {
        file.close();
    }
    //successful program run
    return 0;
}