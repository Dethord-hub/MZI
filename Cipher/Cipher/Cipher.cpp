#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iomanip>
#include <map>

std::string gronsfeldCipher(const std::string& text, const std::vector<int>& key1, const std::vector<int>& key2) {
    std::string encryptedText = text;
    int textLength = text.length();
    int key1Length = key1.size();
    int key2Length = key2.size();

    for (int i = 0; i < textLength; ++i) {
        char currentChar = text[i];
        int shift1 = key1[i % key1Length];
        int shift2 = key2[i % key2Length];

        if (currentChar >= 'А' && currentChar <= 'Я') {
            currentChar = (currentChar - 'А' + shift1) % 33 + 'А';
        }
        else if (currentChar >= 'а' && currentChar <= 'я') {
            currentChar = (currentChar - 'а' + shift1) % 33 + 'а';
        }

        if (currentChar >= 'А' && currentChar <= 'Я') {
            currentChar = (currentChar - 'А' + shift2) % 33 + 'А';
        }
        else if (currentChar >= 'а' && currentChar <= 'я') {
            currentChar = (currentChar - 'а' + shift2) % 33 + 'а';
        }

        encryptedText[i] = currentChar;
    }

    return encryptedText;
}

std::string caesarCipher(const std::string& text, int shift) {
    std::string encryptedText = text;
    int textLength = text.length();

    for (int i = 0; i < textLength; ++i) {
        char currentChar = text[i];

        if (currentChar >= 'А' && currentChar <= 'Я') {
            currentChar = (currentChar - 'А' + shift) % 33 + 'А';
        }
        else if (currentChar >= 'а' && currentChar <= 'я') {
            currentChar = (currentChar - 'а' + shift) % 33 + 'а';
        }

        encryptedText[i] = currentChar;
    }

    return encryptedText;
}

std::string vigenereCipher(const std::string& text, const std::string& key) {
    std::string encryptedText = text;
    int textLength = text.length();
    int keyLength = key.length();

    for (int i = 0; i < textLength; ++i) {
        char currentChar = text[i];
        char keyChar = key[i % keyLength];
        int shift = (keyChar >= 'А' && keyChar <= 'Я') ? keyChar - 'А' : keyChar - 'а';

        if (currentChar >= 'А' && currentChar <= 'Я') {
            currentChar = (currentChar - 'А' + shift) % 33 + 'А';
        }
        else if (currentChar >= 'а' && currentChar <= 'я') {
            currentChar = (currentChar - 'а' + shift) % 33 + 'а';
        }

        encryptedText[i] = currentChar;
    }

    return encryptedText;
}

class LFSR {
private:
    std::vector<unsigned char> state;
    unsigned char tap;

public:
    LFSR(unsigned char seed) : state(8, seed), tap(0x87) {
        for (int i = 0; i < 8; ++i) {
            state[i] = (seed >> i) & 1;
        }
    }

    std::vector<unsigned char> generate(int length) {
        std::vector<unsigned char> sequence;
        for (int i = 0; i < length; ++i) {
            unsigned char newBit = (state[7] ^ state[5] ^ state[4] ^ state[3]) & 1;
            sequence.push_back((state[0] << 7) | (state[1] << 6) | (state[2] << 5) | (state[3] << 4) |
                (state[4] << 3) | (state[5] << 2) | (state[6] << 1) | newBit);

            for (int j = 7; j > 0; --j) {
                state[j] = state[j - 1];
            }
            state[0] = newBit;
        }
        return sequence;
    }
};

std::string lfsrCipher(const std::string& text, const std::vector<unsigned char>& psp) {
    std::string encryptedText = text;
    int textLength = text.length();

    for (int i = 0; i < textLength; ++i) {
        char currentChar = text[i];
        unsigned char pspByte = psp[i % psp.size()];

        if (currentChar >= 'А' && currentChar <= 'Я') {
            currentChar = (currentChar - 'А' + pspByte) % 33 + 'А';
        }
        else if (currentChar >= 'а' && currentChar <= 'я') {
            currentChar = (currentChar - 'а' + pspByte) % 33 + 'а';
        }

        encryptedText[i] = currentChar;
    }

    return encryptedText;
}

double calculateEntropy(const std::string& text) {
    std::vector<int> frequencies(256, 0);
    int textLength = text.length();

    for (char ch : text) {
        frequencies[static_cast<unsigned char>(ch)]++;
    }

    double entropy = 0.0;
    for (int freq : frequencies) {
        if (freq > 0) {
            double p = static_cast<double>(freq) / textLength;
            entropy -= p * std::log2(p);
        }
    }

    return entropy;
}

class FrequencyAnalyzer {
private:
    std::vector<int> frequencies;
    std::vector<int> redFrequencies;
    std::vector<int> greenFrequencies;
    std::vector<int> blueFrequencies;
    std::map<unsigned char, int> charFrequencies;
    sf::Font font;

    const float WINDOW_WIDTH = 512.f;
    const float WINDOW_HEIGHT = 384.f;
    const float MARGIN_LEFT = 50.f;
    const float MARGIN_BOTTOM = 40.f;
    const float MARGIN_TOP = 30.f;
    const float MARGIN_RIGHT = 30.f;
    const float GRAPH_WIDTH = WINDOW_WIDTH - MARGIN_LEFT - MARGIN_RIGHT;
    const float GRAPH_HEIGHT = WINDOW_HEIGHT - MARGIN_TOP - MARGIN_BOTTOM;
    const int MAX_DISPLAY_FREQUENCY = 10000;

    bool useLogScale;

public:
    FrequencyAnalyzer() :
        frequencies(256, 0),
        redFrequencies(256, 0),
        greenFrequencies(256, 0),
        blueFrequencies(256, 0),
        useLogScale(false) {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cout << "Error loading font! Make sure arial.ttf is in the program directory." << std::endl;
        }
    }

    bool analyzeTextFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cout << "Error opening file: " << filename << std::endl;
            return false;
        }

        frequencies = std::vector<int>(256, 0);
        charFrequencies.clear();

        char ch;
        while (file.get(ch)) {
            frequencies[static_cast<unsigned char>(ch)]++;
            charFrequencies[ch]++;
        }

        return true;
    }

    bool analyzeBMPFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cout << "Error opening file: " << filename << std::endl;
            return false;
        }

        frequencies = std::vector<int>(256, 0);
        redFrequencies = std::vector<int>(256, 0);
        greenFrequencies = std::vector<int>(256, 0);
        blueFrequencies = std::vector<int>(256, 0);
        charFrequencies.clear();

        unsigned char header[54];
        file.read(reinterpret_cast<char*>(header), 54);

        int width = *(int*)&header[18];
        int height = *(int*)&header[22];
        int padding = (4 - (width * 3) % 4) % 4;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                unsigned char pixel[3];
                file.read(reinterpret_cast<char*>(pixel), 3);

                blueFrequencies[pixel[0]]++;
                greenFrequencies[pixel[1]]++;
                redFrequencies[pixel[2]]++;
            }
            file.seekg(padding, std::ios::cur);
        }

        for (int i = 0; i < 256; i++) {
            frequencies[i] = redFrequencies[i] + greenFrequencies[i] + blueFrequencies[i];
        }

        return true;
    }

    void displayHistogram() {
        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Frequency Analyzer");
        window.setFramerateLimit(60);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color::White);
            drawHistogram(window, frequencies, sf::Vector2f(0, 0),
                sf::Color::Blue, "Combined Frequencies");
            window.display();
        }
    }

    void displayColorHistograms() {
        sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2),
            "Color Channel Frequency Analyzer");
        window.setFramerateLimit(60);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color::White);

            drawHistogram(window, frequencies, sf::Vector2f(0, 0),
                sf::Color::Blue, "Combined Channels");
            drawHistogram(window, redFrequencies, sf::Vector2f(WINDOW_WIDTH, 0),
                sf::Color::Red, "Red Channel");
            drawHistogram(window, greenFrequencies, sf::Vector2f(0, WINDOW_HEIGHT),
                sf::Color::Green, "Green Channel");
            drawHistogram(window, blueFrequencies, sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT),
                sf::Color(0, 0, 255), "Blue Channel");

            window.display();
        }
    }

    void printFrequencyTable() {
        std::cout << "Frequency Table:" << std::endl;
        std::cout << std::setw(10) << "Symbol" << std::setw(10) << "ASCII"
            << std::setw(15) << "Frequency" << std::endl;
        std::cout << std::string(35, '-') << std::endl;

        for (const auto& pair : charFrequencies) {
            std::cout << std::setw(10) << pair.first << std::setw(10)
                << static_cast<int>(pair.first) << std::setw(15)
                << pair.second << std::endl;
        }
    }

    void setLogScale(bool logScale) {
        useLogScale = logScale;
    }

private:
    std::vector<int> normalizeFrequencies(const std::vector<int>& input) {
        std::vector<int> normalized = input;
        int maxFreq = *std::max_element(input.begin(), input.end());

        if (maxFreq > MAX_DISPLAY_FREQUENCY) {
            float scale = static_cast<float>(MAX_DISPLAY_FREQUENCY) / maxFreq;
            for (int& freq : normalized) {
                freq = static_cast<int>(freq * scale);
            }
        }
        return normalized;
    }

    void drawGrid(sf::RenderWindow& window, int maxFreq, sf::Vector2f offset) {
        for (int i = 0; i <= 10; i++) {
            float y = offset.y + WINDOW_HEIGHT - MARGIN_BOTTOM - (GRAPH_HEIGHT * i / 10.f);

            sf::RectangleShape gridLine(sf::Vector2f(GRAPH_WIDTH, 1));
            gridLine.setPosition(offset.x + MARGIN_LEFT, y);
            gridLine.setFillColor(sf::Color(200, 200, 200));
            window.draw(gridLine);

            sf::Text valueLabel;
            valueLabel.setFont(font);
            valueLabel.setCharacterSize(8);
            valueLabel.setFillColor(sf::Color::Black);
            if (useLogScale) {
                valueLabel.setString(std::to_string(std::pow(10, (std::log10(maxFreq) * i / 10.f))));
            }
            else {
                valueLabel.setString(std::to_string(maxFreq * i / 10));
            }
            valueLabel.setPosition(offset.x + MARGIN_LEFT - 30, y - 5);
            window.draw(valueLabel);
        }

        for (int i = 0; i <= 10; i++) {
            float x = offset.x + MARGIN_LEFT + (GRAPH_WIDTH * i / 10.f);

            sf::RectangleShape gridLine(sf::Vector2f(1, GRAPH_HEIGHT));
            gridLine.setPosition(x, offset.y + WINDOW_HEIGHT - MARGIN_BOTTOM - GRAPH_HEIGHT);
            gridLine.setFillColor(sf::Color(200, 200, 200));
            window.draw(gridLine);

            sf::Text valueLabel;
            valueLabel.setFont(font);
            valueLabel.setCharacterSize(8);
            valueLabel.setFillColor(sf::Color::Black);
            valueLabel.setString(std::to_string(i * 25));
            valueLabel.setPosition(x - 8, offset.y + WINDOW_HEIGHT - MARGIN_BOTTOM + 5);
            window.draw(valueLabel);
        }
    }

    void drawAxes(sf::RenderWindow& window, sf::Vector2f offset) {
        sf::RectangleShape axisX(sf::Vector2f(GRAPH_WIDTH + 10, 2));
        axisX.setPosition(offset.x + MARGIN_LEFT, offset.y + WINDOW_HEIGHT - MARGIN_BOTTOM);
        axisX.setFillColor(sf::Color::Black);

        sf::RectangleShape axisY(sf::Vector2f(2, GRAPH_HEIGHT + 10));
        axisY.setPosition(offset.x + MARGIN_LEFT,
            offset.y + WINDOW_HEIGHT - MARGIN_BOTTOM - GRAPH_HEIGHT - 10);
        axisY.setFillColor(sf::Color::Black);

        window.draw(axisX);
        window.draw(axisY);
    }

    void drawLabels(sf::RenderWindow& window, int maxFreq, sf::Vector2f offset,
        const std::string& title) {
        sf::Text titleText;
        titleText.setFont(font);
        titleText.setString(title);
        titleText.setCharacterSize(12);
        titleText.setFillColor(sf::Color::Black);
        titleText.setPosition(
            offset.x + MARGIN_LEFT + GRAPH_WIDTH / 2 - 40,
            offset.y + MARGIN_TOP - 20
        );

        sf::Text labelX;
        labelX.setFont(font);
        labelX.setString("Value (0-255)");
        labelX.setCharacterSize(10);
        labelX.setFillColor(sf::Color::Black);
        labelX.setPosition(
            offset.x + MARGIN_LEFT + GRAPH_WIDTH / 2 - 30,
            offset.y + WINDOW_HEIGHT - MARGIN_BOTTOM + 20
        );

        sf::Text labelY;
        labelY.setFont(font);
        if (useLogScale) {
            labelY.setString("Frequency (log scale)");
        }
        else {
            labelY.setString("Frequency");
        }
        labelY.setCharacterSize(10);
        labelY.setFillColor(sf::Color::Black);
        labelY.setRotation(-90);
        labelY.setPosition(
            offset.x + MARGIN_LEFT - 40,
            offset.y + WINDOW_HEIGHT - MARGIN_BOTTOM - GRAPH_HEIGHT / 2 + 30
        );

        window.draw(titleText);
        window.draw(labelX);
        window.draw(labelY);
    }

    void drawHistogram(sf::RenderWindow& window, const std::vector<int>& data,
        sf::Vector2f offset, sf::Color color, const std::string& title) {
        std::vector<int> normalizedData = normalizeFrequencies(data);

        int maxFreq = *std::max_element(normalizedData.begin(), normalizedData.end());
        maxFreq = std::max(maxFreq, 1);

        const float barWidth = GRAPH_WIDTH / 256.f;

        drawGrid(window, maxFreq, offset);
        drawAxes(window, offset);
        drawLabels(window, maxFreq, offset, title);

        for (int i = 0; i < 256; i++) {
            if (normalizedData[i] > 0) {
                float height;
                if (useLogScale) {
                    height = (std::log10(normalizedData[i]) / std::log10(maxFreq)) * GRAPH_HEIGHT;
                }
                else {
                    height = (normalizedData[i] * GRAPH_HEIGHT) / maxFreq;
                }
                height = std::min(height, GRAPH_HEIGHT);

                sf::RectangleShape bar;
                bar.setSize(sf::Vector2f(barWidth - 1, height));
                bar.setPosition(
                    offset.x + MARGIN_LEFT + (i * barWidth),
                    offset.y + WINDOW_HEIGHT - MARGIN_BOTTOM - height
                );
                bar.setFillColor(color);

                window.draw(bar);
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    std::ifstream inputFile("input.txt");
    if (!inputFile) {
        std::cerr << "Не удалось открыть файл input.txt" << std::endl;
        return 1;
    }

    std::string text((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    std::vector<int> key1 = { 3, 1, 4, 1, 5, 9 };
    std::vector<int> key2 = { 2, 7, 1, 8, 2, 8 };

    std::string encryptedTextGronsfeld = gronsfeldCipher(text, key1, key2);

    std::ofstream outputFileGronsfeld("output.txt");
    if (!outputFileGronsfeld) {
        std::cerr << "Не удалось открыть файл output.txt" << std::endl;
        return 1;
    }

    outputFileGronsfeld << encryptedTextGronsfeld;
    outputFileGronsfeld.close();

    std::cout << "Зашифрованный текст (Гронфельд) успешно записан в файл output.txt" << std::endl;

    int shift = 13;

    std::string encryptedTextCaesar = caesarCipher(text, shift);

    std::ofstream outputFileCaesar("output_1.txt");
    if (!outputFileCaesar) {
        std::cerr << "Не удалось открыть файл output_1.txt" << std::endl;
        return 1;
    }

    outputFileCaesar << encryptedTextCaesar;
    outputFileCaesar.close();

    std::cout << "Зашифрованный текст (Цезарь) успешно записан в файл output_1.txt" << std::endl;

    std::string keyVigenere = "Промеж худых и хорошему плохо..";

    std::string encryptedTextVigenere = vigenereCipher(text, keyVigenere);

    std::ofstream outputFileVigenere("output_2.txt");
    if (!outputFileVigenere) {
        std::cerr << "Не удалось открыть файл output_2.txt" << std::endl;
        return 1;
    }

    outputFileVigenere << encryptedTextVigenere;
    outputFileVigenere.close();

    std::cout << "Зашифрованный текст (Виженер) успешно записан в файл output_2.txt" << std::endl;

    unsigned char seed = 0xAB; 
    LFSR lfsr(seed);
    std::vector<unsigned char> psp = lfsr.generate(text.length());

    std::string encryptedTextLFSR = lfsrCipher(text, psp);

    std::ofstream outputFileLFSR("output_3.txt");
    if (!outputFileLFSR) {
        std::cerr << "Не удалось открыть файл output_3.txt" << std::endl;
        return 1;
    }

    outputFileLFSR << encryptedTextLFSR;
    outputFileLFSR.close();

    std::cout << "Зашифрованный текст (LFSR) успешно записан в файл output_3.txt" << std::endl;

    FrequencyAnalyzer analyzer;

    std::vector<std::pair<std::string, std::string>> files = {
        {"output.txt", "Гронфельд"},
        {"output_1.txt", "Цезарь"},
        {"output_2.txt", "Виженер"},
        {"output_3.txt", "LFSR"}
    };

    for (const auto& file : files) {
        if (analyzer.analyzeTextFile(file.first)) {
            std::cout << "Гистограмма для " << file.second << " шифрования:" << std::endl;
            analyzer.displayHistogram();
            std::cout << "Энтропия для " << file.second << " шифрования: " << calculateEntropy(file.first) << std::endl;
        }
    }

    return 0;
}
