#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "Graphe.h"
#include "Configuration.h"
#include "StringHandling.h"
#include "DataReader.h"
#include "ExceptionHandling.h"
#include "SolutionReader.h"


using namespace std;
using namespace chrono;
using namespace DataBase;
using namespace Network;
using namespace Builders;
using namespace Config;
using namespace Helpers;

vector<int> getDepots(Configuration &config) {
    vector<int> depots;
    string deps = config["DEPOTS"];
    auto vec = StringHandling::split_string_by(deps, ',');
    for (auto &dep: vec) {
        depots.push_back(stoi(dep));
    }
    return depots;
}


void print_its_no_stats(ofstream &file, unordered_map<int, Itinerary *> &allIts) {

    for (auto &it: allIts) {
        file << "cluster " << it.first << " :";
        it.second->Print_v3(file);
    }
}



int main(int argv, char** args)
{




    if (argv > 1) {
        Configuration::initParamLoc(args[1]);
    }
    Configuration &config = Configuration::getInstance();
    vector<int> depots = getDepots(config);
    //cout<<"depots[0] = "<<depots[0]<<"\n";
    //cout<<"depots[1] = "<<depots[1]<<"\n";
    string print_mode = config["PRINT_MODE"];
    float HLP_TOLERATION = stof(config["HLP_TOLERATION"]);
    float WS_TOLERATION = stof(config["WS_TOLERATION"]);
    float WD_TOLERATION = stof(config["WD_TOLERATION"]);
    bool print_line = !stoi(config["IGNORE_LINE"]);
    vector<string> Results_format = StringHandling::split_string_by(config["RESULTS_FORMAT"], ' ');
    for (int i = 0; i < depots.size(); i++) {
        cout << endl
             << "+------------------------------------------------------------+"
             << endl;
        cout << "|\t DEPOT [" << depots[i] << "]\t  |" << endl;
        string inputDataFile = config["DATA_LOCATION"] + (string) "InputDataDepot" + to_string(depots[i]) +
                              "_EstimatedDeadheadsWithBusLines.txt";
        cout<<"data Location = "<<inputDataFile<<endl;
        string Results = config["RESULTS_LOCATION"] + (string) "TripsInDepot" + to_string(depots[i]) + ".csv";
        string solutionFileToParse = "";
        if (Results_format.size() == 2) {
            solutionFileToParse =
                    config["RESULTS_LOCATION"] + (string) Results_format[0] + to_string(depots[i]) + Results_format[1];
        } else {
            solutionFileToParse = config["RESULTS_LOCATION"] + "Itinerary1Depot"+  to_string(depots[i]) + ".txt";
        }
        cout<<"solution file Location = "<<solutionFileToParse<<endl;

        //we give to the file reader the location of the data file
        DataReader file_reader(inputDataFile);
        AutoBusData &tripsData = file_reader.read_file(); //we read the inputDataFile

        SolutionReader Results_reader(solutionFileToParse, tripsData); //
        auto &itineraries = Results_reader.read_file();


        bool realisable = true;

        double coutTtl = 0.0;
        long NbOfHlpTtl = 0;
        long dureeHlpTtl = 0;
        long dureeAttDepotTtl = 0;
        long dureeAttStationTtl = 0;
        long NbVoyagesTtl = 0;
        long dureeTtl = 0;

        int PGItineraire = itineraries.begin()->first;
        int PPItineraire = itineraries.begin()->first;
        //printf(" %-4s| %-10s| %-5s|\n", "ID", "NAME", "AGE");

        printf("+-------------------------------------------------------------------------------------------------------------------------+\n");
        printf("| %-10s| %-10s  | %-24s| %-24s| %-20s| %-20s|\n", " CLUSTER ", "Durée ttl ", "VOYAGES (min) [%]",
               "HLPs (min) [%]", "WS (min) [%]", "WD (min) [%]");
        printf("+-------------------------------------------------------------------------------------------------------------------------+\n");

        vector<string> erreurs;

        for (auto it: itineraries) {


            if (itineraries.at(PGItineraire)->getNumberofTrips() < it.second->getNumberofTrips()) {
                PGItineraire = it.first; // numero du cluster
            } else if (itineraries.at(PPItineraire)->getNumberofTrips() > it.second->getNumberofTrips()) {
                PPItineraire = it.first; // numero du cluster
            }

            coutTtl += it.second->getCost();
            NbOfHlpTtl += it.second->getNumberOfHLP();
            dureeHlpTtl += it.second->getDurationOfHLP();
            dureeAttDepotTtl += it.second->getDurationOfWaitingInDepot();
            dureeAttStationTtl += it.second->getDurationOfWaiting();
            NbVoyagesTtl += it.second->getNumberofTrips();
            dureeTtl += it.second->getDuration();
            double duree_voyages = it.second->getDuration() -
                                   (it.second->getDurationOfHLP() + it.second->getDurationOfWaitingInDepot() +
                                    it.second->getDurationOfWaiting());
            double percent_voyages = 100 -
                                     (it.second->getPercentOfHLPDuration() + it.second->getPercentOfWaitingInDepot() +
                                      it.second->getPercentOfWaitingTime());
            printf("| %-10d| %6ld min  | %-3d (%-4.fmin) [%-6.2f%%] | %-3d (%-4.fmin) [%-6.2f%%] | (%-4.fmin) [%-6.2f%%] | (%-4.fmin) [%-6.2f%%] |\n",
                   it.first,
                   it.second->getDuration(),
                   it.second->getNumberofTrips(), duree_voyages, percent_voyages,
                   it.second->getNumberOfHLP(), (double) it.second->getDurationOfHLP(),
                   it.second->getPercentOfHLPDuration(),
                   (double) it.second->getDurationOfWaiting(), it.second->getPercentOfWaitingTime(),
                   (double) it.second->getDurationOfWaitingInDepot(), it.second->getPercentOfWaitingInDepot());

            auto itineraireErreurs = it.second->itineraryValid(it.first);
            if (itineraireErreurs.size() != 0)
                erreurs.insert(erreurs.end(), itineraireErreurs.begin(), itineraireErreurs.end());

            printf("+-------------------------------------------------------------------------------------------------------------------------+\n");
        }
        string messageRealisation = erreurs.size() == 0 ? "Solution Realisable" : "Solution Non Realisable";


        if (messageRealisation == "Solution Realisable") {
            printf("\t  SOLUTION REALISABLE POUR DEPOT %d \n",depots[i]);
        } else {
            printf("\t  SOLUTION NON REALISABLE :(\n");
            cout << "A cause de :" << endl;
            for (auto e: erreurs) {
                printf(" %s \n", e.c_str());
            }
            printf("\n +++++++++++++++++++++++++++++++++++++ \n");
            break;

        }

        float fat_result = ((HLP_TOLERATION*dureeHlpTtl)+(WS_TOLERATION*dureeAttStationTtl)+(WD_TOLERATION*dureeAttDepotTtl))/dureeTtl;
        cout <<"\t  Cout : "<<coutTtl <<" $"<<endl;
        //printf("\033[1;36m %.2lf\033[0m $\n", coutTtl);
//        cout << std::setprecision(2)<<"\t\033[1;4mFat :\033[0m " << fat_result*100 << " % " << endl;
        cout <<"\t  FAT = "<<fat_result*100<<" % "<<endl;
        cout <<"\t  Voyages : " << NbVoyagesTtl << " voyages " << endl;
        cout <<"\t  Duree Totale : " << dureeTtl << " min" << endl;
        cout <<"\t  Nombre (duree) des HLP : " << NbOfHlpTtl << " (" << dureeHlpTtl << " min)" << " ["
             << ((dureeHlpTtl * 1.0) / dureeTtl) * 100 << " %]" << endl;
        cout <<"\t  Duree des attentes depot : " << "( " << dureeAttDepotTtl << " min)" << " ["
             << ((dureeAttDepotTtl * 1.0) / dureeTtl) * 100 << " %]" << endl;
        cout <<"\t  Duree des attentes stations : " << "(" << dureeAttStationTtl << " min)" << " ["
             << ((dureeAttStationTtl * 1.0) / dureeTtl) * 100 << " %]" << endl;
        cout <<"\t  Moyenne des voyages par cluster : " << (NbVoyagesTtl * 1.0) / itineraries.size()<<" %"
             << endl;
        cout <<"\t  Le cluster avec le plus grand itineraire : Cluster [" << PGItineraire <<"]"<<endl;
        cout <<"\t  Le cluster avec le plus petit itineraire : Cluster [" << PPItineraire <<"]"<<endl;

        cout << "\t_________________________________________________________________________________________________"
             << endl;


    std::string cheminFichier = "D:\\Canada\\parseSolutionProject\\data\\SolutionStatistiques.txt";

    // Utilisez ofstream pour créer et écrire dans le fichier
    std::ofstream fichier(cheminFichier);


    // Rediriger la sortie standard vers un fichier
    //ofstream fichier("SolutionStatistiques.txt");

    // Vérifier si le fichier est ouvert avec succès
    if (fichier.is_open()) {
        // Sauvegarder la sortie standard actuelle
        streambuf* ancienCout = cout.rdbuf();

        // Rediriger std::cout vers le fichier
        cout.rdbuf(fichier.rdbuf());

        // Le programme commence ici
        cout << "Ce rapport contient les résultats statistiques générés par la Moulinette :" << std::endl;
        cout <<" " << std::endl;



        string messageRealisation = erreurs.size() == 0 ? "Solution Realisable" : "Solution Non Realisable";


        if (messageRealisation == "Solution Realisable") {
            cout << "\t  SOLUTION REALISABLE POUR DEPOT " << depots[i] << std::endl;
            cout<<"-----------------------------------------------------"<<endl;
        } else {
            printf("\t  SOLUTION NON REALISABLE :(\n");
            cout << "A cause de :" << endl;
            for (auto e: erreurs) {
                printf(" %s \n", e.c_str());
            }

        }

        float fat_result = ((HLP_TOLERATION*dureeHlpTtl)+(WS_TOLERATION*dureeAttStationTtl)+(WD_TOLERATION*dureeAttDepotTtl))/dureeTtl;
        cout <<"\t  Cout : "<<coutTtl<<" $ "<<endl;
        cout <<"\t  FAT = "<<fat_result*100<<" % "<<endl;
        cout <<"\t  Voyages : " << NbVoyagesTtl << " voyages " << endl;
        cout <<"\t  Duree Totale : " << dureeTtl << " min" << endl;
        cout <<"\t  Nombre (duree) des HLP : " << NbOfHlpTtl << " (" << dureeHlpTtl << " min)" << " ["
             << ((dureeHlpTtl * 1.0) / dureeTtl) * 100 << " %]" << endl;
        cout <<"\t  Duree des attentes depot : " << "( " << dureeAttDepotTtl << " min)" << " ["
             << ((dureeAttDepotTtl * 1.0) / dureeTtl) * 100 << " %]" << endl;
        cout <<"\t  Duree des attentes stations : " << "(" << dureeAttStationTtl << " min)" << " ["
             << ((dureeAttStationTtl * 1.0) / dureeTtl) * 100 << " %]" << endl;
        cout <<"\t  Moyenne des voyages par cluster : " << (NbVoyagesTtl * 1.0) / itineraries.size()
             << " %"<<endl;
        cout <<"\t  Plus grand itineraire :   Cluster " << PGItineraire << endl;
        cout <<"\t  Plus petit itineraire :   Cluster "<< PPItineraire << endl;

        cout << "\t_________________________________________________________________________________________________"
             << endl;



         // Affichage de l'en-tête
    cout << "+-------------------------------------------------------------------------------------------------------------------------+" << std::endl;
    cout << "| " << std::setw(10) << "CLUSTER" << "| " << std::setw(10) << "Durée ttl" << "  | " << std::setw(24) << "VOYAGES (min) [%]"
              << "| " << std::setw(24) << "HLPs (min) [%]" << "| " << std::setw(20) << "WS (min) [%]" << "| " << std::setw(20) << "WD (min) [%]" << " |" << std::endl;
    cout << "+-------------------------------------------------------------------------------------------------------------------------+" << std::endl;


    // Affichage des résultats
    for (const auto& it : itineraries) {
        // ... Votre logique existante ...

        // Affichage des détails de chaque itinéraire
        double duree_voyages = it.second->getDuration() -
                               (it.second->getDurationOfHLP() + it.second->getDurationOfWaitingInDepot() +
                                it.second->getDurationOfWaiting());
        double percent_voyages = 100 -
                                 (it.second->getPercentOfHLPDuration() + it.second->getPercentOfWaitingInDepot() +
                                  it.second->getPercentOfWaitingTime());

        cout << "| " << std::setw(10) << it.first << "| " << std::setw(6) << it.second->getDuration() << " min  | "
                  << std::setw(3) << it.second->getNumberofTrips() << " (" << std::setw(4) << duree_voyages
                  << "min) [" << std::setw(6) << std::fixed << std::setprecision(2) << percent_voyages << "%] | "
                  << std::setw(3) << it.second->getNumberOfHLP() << " (" << std::setw(4)
                  << (double)it.second->getDurationOfHLP() << "min) [" << std::setw(6)
                  << std::fixed << std::setprecision(2) << it.second->getPercentOfHLPDuration() << "%] | "
                  << "(" << std::setw(4) << (double)it.second->getDurationOfWaiting() << "min) [" << std::setw(6)
                  << std::fixed << std::setprecision(2) << it.second->getPercentOfWaitingTime() << "%] | "
                  << "(" << std::setw(4) << (double)it.second->getDurationOfWaitingInDepot() << "min) [" << std::setw(6)
                  << std::fixed << std::setprecision(2) << it.second->getPercentOfWaitingInDepot() << "%] |" << std::endl;

        // ... Votre logique existante ...

        cout << "+-------------------------------------------------------------------------------------------------------------------------+" << std::endl;
    }












    // Rétablir la sortie standard
        cout.rdbuf(ancienCout);

        // Fermer le fichier
        fichier.close();

        cout << "La sortie a ete redirigee vers 'SolutionStatistiques.txt' avec succes." << std::endl;
    } else {
        // Afficher un message d'erreur si le fichier n'a pas pu être ouvert
        cerr << "Erreur : Impossible d'ouvrir le fichier." << std::endl;
    }






    string cheminFichierExcel = "D:\\Canada\\parseSolutionProject\\data\\SolutionStatistiques.csv";

    // Utilisez ofstream pour créer et écrire dans le fichier
    ofstream fichierCSV((cheminFichierExcel));



    //ofstream fichierCSV("SolutionStatistiques.csv");
                // Vérifier si le fichier est ouvert avec succès
    if (fichierCSV.is_open()) {

        fichierCSV<<"\t Ce rapport contient les resultats statistiques générés par notre Moulinettes :"<<endl;
        fichierCSV<<"\t  SOLUTION REALISABLE POUR DEPOT " << depots[i] <<endl;




        // En-tête du fichier CSV
        fichierCSV << "CLUSTER; Duree ttl; Nbr voyage ; Duree VOYAGES (min) ;Voyage [%]; Nbr HLPs ; HLPs (min); HLPs [%];  WS (min); WS [%]; WD (min); WD [%]" << endl;

        // Affichage des résultats dans le fichier CSV
        for (const auto& it : itineraries) {
            // ... Votre logique existante ...


            if (itineraries.at(PGItineraire)->getNumberofTrips() < it.second->getNumberofTrips()) {
                PGItineraire = it.first; // numero du cluster
            } else if (itineraries.at(PPItineraire)->getNumberofTrips() > it.second->getNumberofTrips()) {
                PPItineraire = it.first; // numero du cluster
            }

            coutTtl += it.second->getCost();
            NbOfHlpTtl += it.second->getNumberOfHLP();
            dureeHlpTtl += it.second->getDurationOfHLP();
            dureeAttDepotTtl += it.second->getDurationOfWaitingInDepot();
            dureeAttStationTtl += it.second->getDurationOfWaiting();
            NbVoyagesTtl += it.second->getNumberofTrips();
            dureeTtl += it.second->getDuration();
            int duree_voyages = it.second->getDuration() -
                                   (it.second->getDurationOfHLP() + it.second->getDurationOfWaitingInDepot() +
                                    it.second->getDurationOfWaiting());
            int percent_voyages = 100 -
                                     (it.second->getPercentOfHLPDuration() + it.second->getPercentOfWaitingInDepot() +
                                      it.second->getPercentOfWaitingTime());




            fichierCSV << it.first << ";"
                  << it.second->getDuration()
                  << ";" << it.second->getNumberofTrips()
                  << ";" << duree_voyages
                  << ";" << percent_voyages
                  << ";" << it.second->getNumberOfHLP()
                  << ";"<< (double)it.second->getDurationOfHLP()
                  << ";" << (int)it.second->getPercentOfHLPDuration()
                  << ";"<< (double)it.second->getDurationOfWaiting()
                  << ";" << (int)it.second->getPercentOfWaitingTime()
                  << ";"<< (double)it.second->getDurationOfWaitingInDepot()
                  << ";" << it.second->getPercentOfWaitingInDepot() << endl;

            // ... Votre logique existante ...
        }


        // Rétablir la sortie standard
        fichierCSV.close();

        cout << "La sortie a ete redirigee vers 'SolutionStatistiques.csv' avec succes." << std::endl;
    } else {
        cerr << "Erreur : Impossible d'ouvrir le fichier CSV." << std::endl;
    }

}
}

