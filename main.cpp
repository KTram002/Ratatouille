#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <map>
#include "json.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

using namespace std;
using json = nlohmann::json;
void complicated();

map<int,string> langConv = {
   {0, "fr"},
   {1, "en"},         // English
   {2, "es"},        // Spanish
   {3, "zh-CN"},     // Chinese(Traditional)
   {4, "ja"}         // Japanese
   
};

int langList();
char *convert(const string & s);

int main () {
   ofstream oFS;
   string sourceLang, targetLang, toTrans, text;
   int numLangFrom, numLangTo;
   
   text = "{\n\t'q': '";
   cout << "Welcome to Ratatouille!\n";
   numLangFrom = langList();
   cout << "\nPlease enter what you'd like to translate.\n";
   cin.ignore(256, '\n');
   getline(cin, toTrans); // What if the input is more than one line? Have a button
   // cin.ignore(256, '\n');
   cout << "\nWhat language would you like to translate to?\n";
   numLangTo = langList();
   
   text += toTrans + "',\n\t'source': '" + langConv.find(numLangFrom)->second + "',\n\t'target': '" +  langConv.find(numLangTo)->second + "',\n\t'format': 'text'\n}";
   
   
   oFS.open("translate-request.json");
   // oFS.open("test.txt");
   oFS << text;
   oFS.close();
   
   complicated();
   
   // Reading translated json
   ifstream iFS("translation.json");
   json j = json::parse(iFS);

   string translated = j.at("data").at("translations").at(0).at("translatedText");
   
   cout << translated << "\n";
   
   return 0;
}

int langList() {
   int num;
   cout << "Please choose a language from the list of supported languages\n"
        << "Appuyez sur 0 pour le français\n"
        << "For English press 1\nPara Español entra número dos\n"
        << "按3来输入中文\n日本語の場合は4を押してください\n";
   cin >> num;
   return num;
}

void complicated() {
   //char* cmd[] = 
   // string s = "curl -s -X POST -H \'Content-Type: application/json' -H \'Authorization: Bearer ya29.c.EloBBQFrtZJqUw0UpRnle3N7MIUO1KjapogX5foGv_3zLgTgICjxUGUy8RNUrY_gvGZn7yV72di824OcVyWpKgfl8igTE5BvY06BRYftkBnewL8IzD_Q3k6cgII\' \'https://translation.googleapis.com/language/translate/v2\' -d @translate-request.json > translation.json";
   // char argument[400] = "-s -X POST -H 'Content-Type: application/json' -H 'Authorization: Bearer ya29.c.EloBBQFrtZJqUw0UpRnle3N7MIUO1KjapogX5foGv_3zLgTgICjxUGUy8RNUrY_gvGZn7yV72di824OcVyWpKgfl8igTE5BvY06BRYftkBnewL8IzD_Q3k6cgII' 'https://translation.googleapis.com/language/translate/v2' -d @translate-request.json > translation.json";
   // char * const* argv = argument;
   // string c = "curl";
   // const char* argv[] = [
   //    c.c_str(), 
   //    "-s".c_str(), 
   //    "-X".c_str(), 
   //    "POST".c_str(),
   //    "-H".c_str(), 
   //    "\'Content-Type: application/json\'".c_str(), 
   //    "-H".c_str(),
   //    "\'Authorization: Bearer ya29.c.EloBBQFrtZJqUw0UpRnle3N7MIUO1KjapogX5foGv_3zLgTgICjxUGUy8RNUrY_gvGZn7yV72di824OcVyWpKgfl8igTE5BvY06BRYftkBnewL8IzD_Q3k6cgII\'".c_str(), 
   //    "\'https://translation.googleapis.com/language/translate/v2\'".c_str(),
   //    "-d".c_str(),
   //    "@translate-request.json".c_str(), 
   //    ">".c_str(),
   //    "translation.json".c_str()
   // ];
   //int execvp(const char *file, char *const argv[]);
   
   vector<char*> vc;
   vector<string> vs;
   
   vs.push_back("curl");
   vs.push_back("-s");
   vs.push_back("--silent");
   vs.push_back("-X");
   vs.push_back("POST");
   vs.push_back("-H");
   vs.push_back("Content-Type: application/json");
   vs.push_back("-H");
   vs.push_back("Authorization: Bearer ya29.c.EloCBYNZ6-ZQHUynZYraAPXutWny2voXBMhEzB5tzPMn-Yv7n4IKQ_3bEFawBW99wXfIg-rEWqthum763wXb4CqT_tV3Z97JqOjX1A6OK1x3F6lXimvIwKHslPQ");
   vs.push_back("https://translation.googleapis.com/language/translate/v2");
   vs.push_back("-d");
   vs.push_back("@translate-request.json");
   
   transform(vs.begin(), vs.end(), back_inserter(vc), convert);
   // for (unsigned i =0; i < vs.size(); i++) {
   //    cout << vs.at(i) << " ";
   // }
   // cout << endl;
   int pipefd[2];
   pipe(pipefd);
   pid_t child = fork();
   if (child < 0) {
      perror("fork failed");
      exit(1);
   }
   else if (child == 0) {
      close(pipefd[0]);
      
      dup2(pipefd[1],1);
      dup2(pipefd[1],2);
      close(pipefd[1]);
      
      execvp(vc[0], &vc[1]);
      exit(1);
   }
   else {
      if (waitpid(child, 0, 0) < 0) {
         perror("cannot wait for child");
         exit(1);
      }
      char buffer[1024] = {0};
      
      close(pipefd[1]);
      while (read(pipefd[0], buffer, sizeof(buffer)) != 0) {}
      ofstream trans;

      trans.open("translation.json");
      trans << buffer;
      trans.close();
   }
   return;
}


char *convert(const string & s)
{
   char *pc = new char[s.size()+1];
   std::strcpy(pc, s.c_str());
   return pc; 
}