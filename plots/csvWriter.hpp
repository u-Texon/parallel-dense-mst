#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>


namespace writer {

    void writeResult(std::ofstream &file, Config &config) {
        hybridMST::mpi::MPIContext ctx;
        file << config.algo << "," << ctx.size() << "," << config.log_m << "," << config.log_n << ","
             << config.minWeight << "," << config.maxWeight << "," << config.graphType << "," << config.treeFactor
             << "," << config.edgesPerProc << "," << config.shuffle << "," << config.useKruskal
             << "," << config.boruvkaOverlapCount << "," << config.localMSTcount;
    }

    void writeMixedMergeResult(std::string timerOutput, std::ofstream &file, Config &config) {
        std::string delimiter = " ";

        std::vector<size_t> allreduce;
        std::vector<size_t> iteration;
        std::vector<size_t> removeParallelEdges;
        std::vector<size_t> calcLocalMST;
        std::vector<size_t> shrink;
        std::vector<size_t> parentArray;
        std::vector<size_t> calcIncident;
        std::vector<size_t> relabel;
        size_t fullTime = 0;
        size_t init = 0;
        size_t initialMST = 0;
        size_t pos = 0;
        std::string token;
        while ((pos = timerOutput.find(delimiter)) != std::string::npos) {
            token = timerOutput.substr(0, pos);

            if (token.substr(0, strlen("allreduce")) == "allreduce") {
                allreduce.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("mixedMerge")) == "mixedMerge") {
                fullTime = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("mm-initial-localMST")) == "mm-initial-localMST") {
                initialMST = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("shrink")) == "shrink") {
                shrink.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("calc-incident")) == "calc-incident") {
                calcIncident.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("relabel")) == "relabel") {
                relabel.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("parentArray")) == "parentArray") {
                parentArray.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("b-calcLocalMST")) == "b-calcLocalMST") {
                calcLocalMST.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("initVariables")) == "initVariables") {
                init = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("b-iteration")) == "b-iteration") {
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
            file << fullTime << "," << init << "," << initialMST << "," << calcLocalMST[i] << "," << iteration[i] << ","
                 << allreduce[i]
                 << ","
                 << removeParallelEdges[i] << "," << shrink[i] << "," << calcIncident[i] << "," << parentArray[i] << ","
                 << relabel[i];
            if (i == 0) {
                file << ",";
                writeResult(file, config);
            }
            file << std::endl;
        }


    }


    void writeBoruvkaMergeResult(std::string timerOutput, std::ofstream &file, Config &config) {
        std::string delimiter = " ";

        std::vector<size_t> allreduce;
        std::vector<size_t> iteration;
        std::vector<size_t> removeParallelEdges;
        std::vector<size_t> calcLocalMST;
        std::vector<size_t> shrink;
        std::vector<size_t> parentArray;
        std::vector<size_t> calcIncident;
        std::vector<size_t> relabel;
        size_t boruvka = 0;
        size_t init = 0;
        size_t initialMST = 0;
        size_t pos = 0;
        std::string token;
        while ((pos = timerOutput.find(delimiter)) != std::string::npos) {
            token = timerOutput.substr(0, pos);

            if (token.substr(0, strlen("allreduce")) == "allreduce") {
                allreduce.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("boruvka")) == "boruvka") {
                boruvka = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("b-initial-localMST")) == "b-initial-localMST") {
                initialMST = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("shrink")) == "shrink") {
                shrink.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("calc-incident")) == "calc-incident") {
                calcIncident.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("relabel")) == "relabel") {
                relabel.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("parentArray")) == "parentArray") {
                parentArray.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("b-calcLocalMST")) == "b-calcLocalMST") {
                calcLocalMST.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("initVariables")) == "initVariables") {
                init = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("b-iteration")) == "b-iteration") {
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
            file << boruvka << "," << init << "," << initialMST << "," << calcLocalMST[i] << "," << iteration[i] << ","
                 << allreduce[i]
                 << ","
                 << removeParallelEdges[i] << "," << shrink[i] << "," << calcIncident[i] << "," << parentArray[i] << ","
                 << relabel[i];
            if (i == 0) {
                file << ",";
                writeResult(file, config);
            }
            file << std::endl;
        }


    }

    void writeMergeResults(std::string timerOutput, std::ofstream &file, Config &config) {
        std::string delimiter = " ";
        std::vector<size_t> sendRecv;
        std::vector<size_t> iteration;
        std::vector<size_t> calcLocalMST;
        size_t runTime = 0;
        size_t initialMST = 0;
        size_t pos;
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
            file << runTime << "," << initialMST << "," << calcLocalMST[i] << "," << iteration[i] << "," << sendRecv[i];
            if (i == 0) {
                file << ",";
                writeResult(file, config);
            }
            file << std::endl;
        }

    }


    void writeBoruvkaResults(std::string timerOutput, std::ofstream &file, Config &config) {
        std::string delimiter = " ";

        std::vector<size_t> allreduce;
        std::vector<size_t> iteration;
        std::vector<size_t> removeParallelEdges;
        std::vector<size_t> calcLocalMST;
        std::vector<size_t> shrink;
        std::vector<size_t> parentArray;
        std::vector<size_t> calcIncident;
        std::vector<size_t> relabel;
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
            } else if (token.substr(0, strlen("shrink")) == "shrink") {
                shrink.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("overlap")) == "overlap") {
                allreduce.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            }
            else if (token.substr(0, strlen("calc-incident")) == "calc-incident") {
                calcIncident.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("relabel")) == "relabel") {
                relabel.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("parentArray")) == "parentArray") {
                parentArray.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("b-calcLocalMST")) == "b-calcLocalMST") {
                calcLocalMST.push_back(std::stoi(token.erase(0, timerOutput.find('=') + 1)));
            } else if (token.substr(0, strlen("init")) == "init") {
                init = std::stoi(token.erase(0, timerOutput.find('=') + 1));
            } else if (token.substr(0, strlen("b-iter")) == "b-iter") {
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
            file << boruvka << "," << init << "," << calcLocalMST[i] << "," << iteration[i] << "," << allreduce[i]
                 << "," << removeParallelEdges[i] << "," << shrink[i] << "," << calcIncident[i] << "," << parentArray[i]
                 << "," << relabel[i];
            if (i == 0) {
                file << ",";
                writeResult(file, config);
            }
            file << std::endl;
        }

    }


    void write_csv(const std::string &filePath, Config &config, std::string &timerOutput, size_t iteration) {
        std::ofstream file;
        hybridMST::mpi::MPIContext ctx;
        if (config.algo == "boruvka" && config.onlyThisAlgo) {

            if (config.boruvkaOverlapCount) {
                file.open(filePath + "only-" + config.algo + "-thread-proc" + std::to_string(ctx.size()) + +"-iter" +
                          std::to_string(iteration) + ".csv");
            } else {
                file.open(filePath + "only-" + config.algo + "-proc" + std::to_string(ctx.size()) + +"-iter" +
                          std::to_string(iteration) + ".csv");
            }


            if (!file.is_open()) {
                std::cout << "!!! error on opening file " << filePath << " !!!" << std::endl;
            }
            file
                    << "run time,init variables,calculate local MST,iteration,allreduce,remove parallel edges,shrink,calc-incident,parentArray,relabel,"
                    << "Algorithm,Processors,log(m),log(n),minimum weight,maximum weight,graph-type,tree-factor,edges per processor,edges are shuffled,kruskal as base case,boruvkaThread,localMSTcount"
                    << std::endl;
            writeBoruvkaResults(timerOutput, file, config);
        } else if (config.algo == "merge" && config.onlyThisAlgo) {
            file.open(filePath + "only-" + config.algo + "-proc" + std::to_string(ctx.size()) + +"-iter" +
                                                         std::to_string(iteration) + ".csv");
            if (!file.is_open()) {
                std::cout << "!!! error on opening mergeFile " << filePath << " !!!" << std::endl;
            }
            file << "run time,calculate initial MST,calculate local MST,iteration,send/receive MST,"
                 << "Algorithm,Processors,log(m),log(n),minimum weight,maximum weight,graph-type,tree-factor,edges per processor,edges are shuffled,kruskal as base case,boruvkaThread,localMSTcount"
                 << std::endl;
            writeMergeResults(timerOutput, file, config);
        } else if (config.onlyThisAlgo && (config.algo == "mixedMerge" || config.algo == "boruvkaMerge")) {
            std::ofstream boruvkaFile;
            boruvkaFile.open(filePath + "only-" + config.algo + "-boruvka-proc" + std::to_string(ctx.size()) + +"-iter" +
                                                                              std::to_string(iteration) + ".csv");

            if (!boruvkaFile.is_open()) {
                std::cout << "!!! error on opening mergeFile " << filePath << " !!!" << std::endl;
            }

            boruvkaFile
                    << "run time,init variables,initial local MST, calculate local MST,iteration,allreduce,remove parallel edges,shrink,calc-incident,parentArray,relabel,"
                    << "Algorithm,Processors,log(m),log(n),minimum weight,maximum weight,graph-type,tree-factor,edges per processor,edges are shuffled,kruskal as base case,boruvkaThread,localMSTcount"
                    << std::endl;

            if (config.algo == "mixedMerge") {
                writeMixedMergeResult(timerOutput, boruvkaFile, config);
            } else {
                writeBoruvkaMergeResult(timerOutput, boruvkaFile, config);
            }
            boruvkaFile.close();


            file.open(filePath + "only-" + config.algo + "-merge-proc" + std::to_string(ctx.size()) + +"-iter" +
                                                         std::to_string(iteration) + ".csv");
            if (!file.is_open()) {
                std::cout << "!!! error on opening mergeFile " << filePath << " !!!" << std::endl;
            }

            file
                    << "run time,calculate initial MST,calculate local MST,iteration,send/receive MST,"
                    << "Algorithm,Processors,log(m),log(n),minimum weight,maximum weight,graph-type,tree-factor,edges per processor,edges are shuffled,kruskal as base case,boruvkaThread,localMSTcount"
                    << std::endl;
            writeMergeResults(timerOutput, file, config);

        } else {
            std::string fileName = "";
            if (config.boruvkaOverlapCount) {
                fileName = filePath + config.algo + "-thread" + ".csv";
            } else {
                fileName = filePath + config.algo + ".csv";
            }


            std::ifstream f(fileName);
            bool alreadyExists = f.good();

            file.open(fileName, std::ios::app);
            if (!file.is_open()) {
                std::cout << "!!! error on opening mergeFile " << fileName << " !!!" << std::endl;
            }

            size_t result = 0;
            size_t filter = 0;
            std::string delimiter = " ";
            size_t pos = 0;
            std::string token;
            while ((pos = timerOutput.find(delimiter)) != std::string::npos) {
                token = timerOutput.substr(0, pos);
                if (token.substr(0, strlen("filterEdges")) == "filterEdges") {
                    filter = std::stoi(token.erase(0, timerOutput.find('=') + 1));
                } else if (token.substr(0, strlen(config.algo.c_str())) == config.algo) {
                    result = std::stoi(token.erase(0, timerOutput.find('=') + 1));
                }
                timerOutput.erase(0, pos + delimiter.length());
            }


            if (!alreadyExists) {
                file
                        << "Algorithm,Processors,log(m),log(n),minimum weight,maximum weight,graph-type,tree-factor,edges per processor,edges are shuffled,kruskal as base case,boruvkaThread,localMSTcount,run time,filter"
                        << std::endl;
            }
            writeResult(file, config);
            file << "," << result + filter << "," << filter << std::endl;
        }
        file.close();
    }
}