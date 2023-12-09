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

double min(double first, double second) {
    if (first < second) {
        return first;
    }
    else {
        return second;
    }
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

    double getRangeToMax(double val) {
        return sourceVal + range - 1 - val;
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
    double maxSkipRange;
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
                maxSkipRange = currEntry->getRangeToMax(source);
                destination = currEntry->getMappedVal(source);
                return destination;
            }
        }
        return destination;
    }

    double getMaxSkipRange() {
        return maxSkipRange;
    }
};


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

class SeedRange {
private:
    double startSeed;
    double range;
    double currSeed;
public:
    SeedRange(double start, double r) {
        startSeed = start;
        currSeed = 0;
        range = r;
    }

    double getCurrentSeed() {
        return startSeed+currSeed;
    }

    double getNextSeed() {
        currSeed++;
        return startSeed+currSeed;
    }

    bool atLastSeed() {
        return currSeed >= range;
    }

    double getMaxSkipRange() {
        return range - 1 - currSeed;
    }

    bool isInRange(double seed) {
        return (startSeed <= seed && seed <= startSeed+range);
    }

    void updateCurrent(double newCurrent) {
        currSeed += newCurrent;
    }
};

class SeedRanges {
private:
    list<SeedRange> seedRanges;
    list<SeedRange>::iterator currRange;
    double seedsTested;
public:
    SeedRanges(string line) {
        int pos = 0;
        while (pos < line.length() && !isNum(line[pos])) {
            pos++;
        }
        double start;
        double range;
        while (pos < line.length()) {
            // get start value`
            start = 0;
            range = 0;
            while (pos < line.length() && isNum(line[pos])) {
                start = (start * 10) + toNum(line[pos]);
                pos++;
            }
            pos++; // move past space
            // get range value
            while (pos < line.length() && isNum(line[pos])) {
                range = (range * 10) + toNum(line[pos]);
                pos++;
            }
            seedRanges.push_back(SeedRange(start, range));
            pos++;
        }
        currRange = seedRanges.begin();
    }

    bool seedsRemain() {
        bool remain = true;
        list<SeedRange>::iterator nextRange;
        if (currRange != seedRanges.end()) {
            if (currRange->atLastSeed()) {
                nextRange = currRange;
                nextRange++;
                if (nextRange != seedRanges.end()) {
                    remain = true;
                }
                else {
                    remain = false;
                }
            }
            else {
                remain = true;
            }
        }
        return remain;
    }

    double getCurrentSeed() {
        return currRange->getCurrentSeed();
    }

    double getNextSeed() {
        if (currRange->atLastSeed()) {
            currRange++;
            return currRange->getCurrentSeed();
        }
        else {
            return currRange->getNextSeed();
        }
    }

    double getMaxSkipRange() {
        return currRange->getMaxSkipRange();
    }

    void updateCurrent(double newCurrent) {
        currRange->updateCurrent(newCurrent);
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
    double minLocationPart1;

    double soil = seedToSoilMap.getDestination(seedNum);
    double fertilizer = soilToFertilizerMap.getDestination(soil);
    double water = fertilizerToWaterMap.getDestination(fertilizer);
    double light = waterToLightMap.getDestination(water);
    double temp = lightToTempMap.getDestination(light);
    double humidity = tempToHumidityMap.getDestination(temp);
    double location = humidityToLocationMap.getDestination(humidity);
    minLocationPart1 = location;

    // Part 1
    seedNum = seeds.getNextSeed();
    while (seeds.seedsRemain()) {
        soil = seedToSoilMap.getDestination(seedNum);
        fertilizer = soilToFertilizerMap.getDestination(soil);
        water = fertilizerToWaterMap.getDestination(fertilizer);
        light = waterToLightMap.getDestination(water);
        temp = lightToTempMap.getDestination(light);
        humidity = tempToHumidityMap.getDestination(temp);
        location = humidityToLocationMap.getDestination(humidity);
        if (location < minLocationPart1) {
            minLocationPart1 = location;
        }
        seedNum = seeds.getNextSeed();
    }

    // Part 2
    double minLocationPart2 = minLocationPart1;
    SeedRanges seedRanges(seedLine);

    double maxSkipRange;

    seedNum = seedRanges.getCurrentSeed();
    maxSkipRange = seedRanges.getMaxSkipRange();
    soil = seedToSoilMap.getDestination(seedNum);
    maxSkipRange = min(maxSkipRange, seedToSoilMap.getMaxSkipRange());
    fertilizer = soilToFertilizerMap.getDestination(soil);
    maxSkipRange = min(maxSkipRange, soilToFertilizerMap.getMaxSkipRange());
    water = fertilizerToWaterMap.getDestination(fertilizer);
    maxSkipRange = min(maxSkipRange, fertilizerToWaterMap.getMaxSkipRange());
    light = waterToLightMap.getDestination(water);
    maxSkipRange = min(maxSkipRange, waterToLightMap.getMaxSkipRange());
    temp = lightToTempMap.getDestination(light);
    maxSkipRange = min(maxSkipRange, lightToTempMap.getMaxSkipRange());
    humidity = tempToHumidityMap.getDestination(temp);
    maxSkipRange = min(maxSkipRange, tempToHumidityMap.getMaxSkipRange());
    location = humidityToLocationMap.getDestination(humidity);
    maxSkipRange = min(maxSkipRange, humidityToLocationMap.getMaxSkipRange());
    if (location < minLocationPart2) {
        minLocationPart2 = location;
    }
    if (maxSkipRange > 0) {
        seedRanges.updateCurrent(maxSkipRange);
    }


    while (seedRanges.seedsRemain()) {
        seedNum = seedRanges.getNextSeed();
        maxSkipRange = seedRanges.getMaxSkipRange();
        soil = seedToSoilMap.getDestination(seedNum);
        maxSkipRange = min(maxSkipRange, seedToSoilMap.getMaxSkipRange());
        fertilizer = soilToFertilizerMap.getDestination(soil);
        maxSkipRange = min(maxSkipRange, soilToFertilizerMap.getMaxSkipRange());
        water = fertilizerToWaterMap.getDestination(fertilizer);
        maxSkipRange = min(maxSkipRange, fertilizerToWaterMap.getMaxSkipRange());
        light = waterToLightMap.getDestination(water);
        maxSkipRange = min(maxSkipRange, waterToLightMap.getMaxSkipRange());
        temp = lightToTempMap.getDestination(light);
        maxSkipRange = min(maxSkipRange, lightToTempMap.getMaxSkipRange());
        humidity = tempToHumidityMap.getDestination(temp);
        maxSkipRange = min(maxSkipRange, tempToHumidityMap.getMaxSkipRange());
        location = humidityToLocationMap.getDestination(humidity);
        maxSkipRange = min(maxSkipRange, humidityToLocationMap.getMaxSkipRange());
        if (location < minLocationPart2) {
            minLocationPart2 = location;
        }
        if (maxSkipRange > 0) {
            seedRanges.updateCurrent(maxSkipRange);
        }
    }


    cout << "Minimum location, part 1 = " << to_string(minLocationPart1) << '\n';

    cout << "Minimum location, part 2 = " << to_string(minLocationPart2) << '\n';

    return 0;
}
