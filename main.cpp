#include <iostream>
#include <fstream>
#include <string>
#include <list>
using namespace std;


bool isNum(char c) {
    return ('0' <= c && c <= '9');
}

int toNum(char c) {
    return c - '0';
}

void create_s_t_r_from_string(string &line, double &source, double &target, double &range) {
    int pos=0;
    // parse target val from string
    double val = 0;
    for (pos; pos < line.length() && isNum(line[pos]); pos++) {
        val = (val * 10) + toNum(line[pos]);
    }
    target = val;

    // parse source from string
    val = 0;
    for (pos++; pos < line.length() && isNum(line[pos]); pos++) {
        val = (val * 10) + toNum(line[pos]);
    }
    source = val;

    // parse range from string
    val = 0;
    for (pos++; pos < line.length() && isNum(line[pos]); pos++) {
        val = (val * 10) + toNum(line[pos]);
    }
    range = val;
}

class MapEntry {
    private:
        double sourceVal;
        double targetVal;
        double range;

    public:
        bool isInSourceRange(double val) {
            return (sourceVal <= val && val < sourceVal+range);
        }

        double getMappedVal(double val) {
            if (isInSourceRange(val)) {
                return (val - sourceVal + targetVal);
            }
            else {
                return val;
            }
        }

        MapEntry (string line) {
            create_s_t_r_from_string(line, sourceVal, targetVal, range);
        }

        MapEntry();
};


class Map {
    private:
        list<MapEntry> entries;
        string name;

    public:
        void addEntry(string line) {
            entries.push_back(MapEntry(line));
        }
        Map(string file_name) {
            name = file_name;
            ifstream input_file;
            input_file.open(file_name);
            string line;
            if (input_file.is_open()) {
                getline(input_file, line);
                while (getline(input_file, line)) {
                    addEntry(line);
                }
            }
            else {
                cerr << "Error opening file " << file_name << '\n';
            }
            input_file.close();
        }

        double getDestination(double source) {
            double destination = source;
            for (list<MapEntry>::iterator currEntry = entries.begin(); currEntry != entries.end(); currEntry++) {
                if (currEntry->isInSourceRange(source)) {
                    destination = currEntry->getMappedVal(source);
                    // cout << name << " maps " << source << " to " << destination << '\n';
                    return destination;
                }
            }
            // cout << name << " doesn't contain a mapping for " << source << '\n';
            return destination;
        }
};

//TODO refactor for part 2
class SeedList {
private:
    list<double> seeds;
    list<double>::iterator currSeed;
public:
    SeedList(string line) {
        // clear initial text/non-nums;
        int pos = 0;
        while (pos < line.length() && !isNum(line[pos])) {
            pos++;
        }

        //add
        double val = 0;
        for (pos; pos < line.length(); pos++) {
            if (isNum(line[pos])) {
                val = (val * 10) + toNum(line[pos]);
            }
            else {
                if (val > 0) {
                    seeds.push_back(val);
                }
                val = 0;
            }
        }
        if (val > 0) {
            seeds.push_back(val);
        }
        currSeed = seeds.begin();
    }
    bool seedsRemain() {
        return (currSeed != seeds.end());
    }
    double getNextSeed() {
        currSeed++;
        return *currSeed;
    }
    double getCurrentSeed() {
        return *currSeed;
    }
};


int main(int argc, char **argv) {
    Map seedToSoilMap("seed-to-soil-map.txt");
    Map soilToFertilizerMap("soil-to-fertilizer-map.txt");
    Map fertilizerToWaterMap("fertilizer-to-water-map.txt");
    Map waterToLightMap("water-to-light-map.txt");
    Map lightToTempMap("light-to-temp-map.txt");
    Map tempToHumidityMap("temp-to-humidity-map.txt");
    Map humidityToLocationMap("humidity-to-location-map.txt");

    ifstream seed_file;
    seed_file.open("seeds.txt");
    string seedLine;
    getline(seed_file, seedLine);
    seed_file.close();
    SeedList seeds(seedLine);

    double seedNum = seeds.getCurrentSeed();
    double minLocation;

    double soil = seedToSoilMap.getDestination(seedNum);
    double fertilizer = soilToFertilizerMap.getDestination(soil);
    double water = fertilizerToWaterMap.getDestination(fertilizer);
    double light = waterToLightMap.getDestination(water);
    double temp = lightToTempMap.getDestination(light);
    double humidity = tempToHumidityMap.getDestination(temp);
    double location = humidityToLocationMap.getDestination(humidity);
    minLocation = location;


    seedNum = seeds.getNextSeed();
    while (seeds.seedsRemain()) {
        soil = seedToSoilMap.getDestination(seedNum);
        fertilizer = soilToFertilizerMap.getDestination(soil);
        water = fertilizerToWaterMap.getDestination(fertilizer);
        light = waterToLightMap.getDestination(water);
        temp = lightToTempMap.getDestination(light);
        humidity = tempToHumidityMap.getDestination(temp);
        location = humidityToLocationMap.getDestination(humidity);
        if (location < minLocation) {
            minLocation = location;
        }
        if (location < 0) {
            cout << "wtf for seed " << seedNum << "?\n";
        }
        seedNum = seeds.getNextSeed();
    }

    cout << "Minimum location = " << to_string(minLocation) << '\n';

    return 0;
}
