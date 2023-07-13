#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>


namespace writer {



    void writeMergeResults(std::string &timerOutput, std::ofstream &file) {
        std::string delimiter = " ";

        std::vector<size_t> sendRecv;
        std::vector<size_t> iteration;
        std::vector<size_t> calcLocalMST;
        size_t runTime = 0;
        size_t initialMST = 0;

        size_t pos = 0;
        std::string token;
        while ((pos = timerOutput.find(delimiter)) != std::string::npos) {
            token = timerOutput.substr(0, pos);

            if (token.substr(0, strlen("sendMST")) == "sendMST") {
                sendRecv.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("merge")) == "merge") {
                runTime = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("localMST")) == "localMST") {
                calcLocalMST.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("initial-localMST")) == "initial-localMST") {
                initialMST = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("iteration")) == "iteration") {
                iteration.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            }

            timerOutput.erase(0, pos + delimiter.length());
        }


        for (int i = 0; i < iteration.size(); ++i) {
            file << runTime << "," << initialMST << "," << calcLocalMST[i] << "," << iteration[i] << "," << sendRecv[i] << std::endl;
        }

    }


    void writeBoruvkaResults(std::string &timerOutput, std::ofstream &file) {
        std::string delimiter = " ";

        std::vector<size_t> allreduce;
        std::vector<size_t> iteration;
        std::vector<size_t> removeParallelEdges;
        std::vector<size_t> calcLocalMST;
        size_t boruvka = 0;
        size_t init = 0;

        size_t pos = 0;
        std::string token;
        while ((pos = timerOutput.find(delimiter)) != std::string::npos) {
            token = timerOutput.substr(0, pos);

            if (token.substr(0, strlen("allreduce")) == "allreduce") {
                allreduce.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("boruvka")) == "boruvka") {
                boruvka = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("calcLocalMST")) == "calcLocalMST") {
                calcLocalMST.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("init")) == "init") {
                init = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("iter")) == "iter") {
                iteration.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("remove")) == "remove") {
                removeParallelEdges.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            }

            timerOutput.erase(0, pos + delimiter.length());
        }


        for (int i = 0; i < iteration.size(); ++i) {
            if (i >= calcLocalMST.size()) {
                calcLocalMST.push_back(0);
            }
            file << boruvka << "," << init << "," << calcLocalMST[i] << "," << iteration[i] << "," << allreduce[i] << ","
                 << removeParallelEdges[i] << std::endl;
        }

    }


    void writeResult(std::string &result, std::ofstream &file, Config &config) {
        hybridMST::mpi::MPIContext ctx;
        file << config.algo << "," << ctx.size() << "," << config.log_m << "," << config.log_n << ","
             << config.minWeight << "," << config.maxWeight << "," << config.graphType << "," << config.treeFactor
             << ","
             << config.edgesPerProc << "," << config.shuffle << "," << config.useKruskal << ","
             << result << std::endl;
    }

    void write_csv(const std::string &filePath, Config &config, std::string &timerOutput) {
        std::ofstream file;


        if (config.algo == "boruvka" && config.onlyThisAlgo) {
            file.open(filePath + "only-" + config.algo + ".csv");

            if (!file.is_open()) {
                std::cout << "!!! error on opening file " << filePath << " !!!" << std::endl;
            }

            file << "run time,init variables,calculate local MST,iteration,allreduce,remove parallel edges"
                 << std::endl;
            writeBoruvkaResults(timerOutput, file);
        } else if (config.algo == "merge" && config.onlyThisAlgo) {
            file.open(filePath + "only-" + config.algo + ".csv");

            if (!file.is_open()) {
                std::cout << "!!! error on opening file " << filePath << " !!!" << std::endl;
            }

            file << "run time,calculate initial MST,calculate local MST,iteration,send/receive MST"
                 << std::endl;
            writeMergeResults(timerOutput, file);
        } else {
            std::string fileName = "";
            if (config.onlyThisAlgo) {
                fileName = filePath + "only-" + config.algo + ".csv";
            } else {
                fileName = filePath + config.algo + ".csv";
            }

            std::ifstream f(fileName);
            bool alreadyExists = f.good();

            file.open(fileName, std::ios::app);
            if (!file.is_open()) {
                std::cout << "!!! error on opening file " << fileName << " !!!" << std::endl;
            }

            std::string result = timerOutput.erase(0, timerOutput.find('=') + 1);
            if (!alreadyExists) {
                file
                        << "Algorithm,Processors,log(m),log(n),minimum weight,maximum weight,graph-type,tree-factor,edges per processor,edges are shuffled,kruskal as base case,run time"
                        << std::endl;
            }
            writeResult(result, file, config);

        }


        file.close();

    }


}