#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <random>
#include <iostream>
#include <string>
#include <functional>
#include "CountingVector.h"
#include "sorts.h"
#include "SortState.h"

// Global constants
const int WIDTH{1200};
const int HEIGHT{800};
const int LIGHT_DURATION{50}; // In milliseconds

bool isNumber(const std::string &s)
{
    for (auto& c : s)
    {
        if (!std::isdigit(c))
            return false;
    }

    return true;
}

bool validateInput(
    int argc,
    char *argv[],
    const std::map<std::string, std::function<void(SortState &, int)>> &sortingAlgorithms)
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <sortType> <n> <delay>" << std::endl
                  << std::endl
                  << "Arguments:" << std::endl
                  << "  sortType  The sorting algorithm to use." << std::endl
                  << std::string(12, ' ') << "(";

        for (auto &algorithm : sortingAlgorithms)
        {
            std::cerr << algorithm.first;

            if (algorithm.first != sortingAlgorithms.rbegin()->first)
                std::cerr << ", ";
        }

        std::cerr << ")" << std::endl
                  << "  n         The number of elements to sort." << std::endl
                  << "  delay     Sorting delay in milliseconds." << std::endl
                  << std::endl
                  << "Example: " << std::endl
                  << "  " << argv[0] << " bubble 25 50"
                  << std::endl;

        return false;
    }

    // Argument 1
    if (sortingAlgorithms.find(argv[1]) == sortingAlgorithms.end())
    {
        std::cerr << "Invalid input for sortType. Please provide one of the following options: ";

        for (auto &algorithm : sortingAlgorithms)
        {
            std::cerr << algorithm.first;

            if (algorithm.first != sortingAlgorithms.rbegin()->first)
                std::cerr << ", ";
        }

        std::cerr << std::endl;

        return false;
    }

    // Argument 2
    if (!isNumber(argv[2]) || std::stoi(argv[2]) < 0)
    {
        std::cerr << "Invalid input for n. Please provide a positive integer."
                  << std::endl;

        return false;
    }

    // Argument 3
    if (!isNumber(argv[3]) || std::stoi(argv[3]) < 0)
    {
        std::cerr << "Invalid input for delay. Please provide a positive integer."
                  << std::endl;

        return false;
    }

    return true;
}

void playTone(int frequency, int sortingDelay)
{
    const int sampleRate = 44100;
    const int numSamples = static_cast<int>(sampleRate * sortingDelay / 1000.0);

    if (numSamples == 0)
        return;

    std::vector<sf::Int16> samples(numSamples);

    for (int i = 0; i < numSamples; i++)
    {
        const double t = i / static_cast<double>(sampleRate);
        samples[i] = static_cast<sf::Int16>(std::sin(2 * M_PI * frequency * t) * 30000);
    }

    sf::SoundBuffer buffer;
    buffer.loadFromSamples(&samples[0], numSamples, 1, sampleRate);

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setVolume(5);
    sound.play();

    std::this_thread::sleep_for(std::chrono::milliseconds(sortingDelay * 10));
}

void drawBars(
    sf::RenderWindow &window,
    SortState &state,
    int timeElapsed,
    int sortingDelay,
    int checkingIndex,
    int &prevCheckingIndex,
    std::vector<int> &lastTimeAccessed)
{
    // Don't draw bars if window is too small
    if (window.getSize().y <= 40) return;

    for (int i = 0; i < state.numbers.size(); ++i)
    {
        std::unique_lock<std::mutex> lock(state.mtx);

        const double barWidth{static_cast<double>(window.getSize().x) / state.numbers.size()};
        const double barHeight{static_cast<double>(state.numbers[i] * (window.getSize().y - 40)) / state.numbers.size()};

        if (state.numbers.isAccessed(i))
        {
            lastTimeAccessed[i] = timeElapsed;
            state.numbers.clearAccessed(i);

            // Play tone when a number changes
            const int frequency = 1760 * (static_cast<double>(state.numbers[i]) / state.numbers.size());
            std::thread playToneThread(playTone, frequency, sortingDelay);
            playToneThread.detach();
        }

        if (checkingIndex != prevCheckingIndex)
        {
            prevCheckingIndex = checkingIndex;

            // Play tone when checking index changes
            const int frequency = 1760 * (static_cast<double>(state.numbers[checkingIndex]) / state.numbers.size());
            std::thread playToneThread(playTone, frequency, sortingDelay);
            playToneThread.detach();
        }

        lock.unlock();

        sf::RectangleShape rect(sf::Vector2f(barWidth, barHeight));

        // Light up the bar for LIGHT_DURATION milliseconds
        if (timeElapsed - lastTimeAccessed[i] < LIGHT_DURATION || i == checkingIndex)
            rect.setFillColor(sf::Color::Red);

        if (i < checkingIndex)
            rect.setFillColor(sf::Color::Green);

        rect.setPosition(sf::Vector2f(barWidth * i, window.getSize().y - barHeight));

        window.draw(rect);
    }
}

void verify(SortState &state, int &checkingIndex, int sortingDelay)
{
    for (int i = 0; i < state.numbers.size(); i++)
    {
        checkingIndex = i;

        std::unique_lock<std::mutex> lock(state.mtx);

        if (state.numbers[i] < state.numbers[i - 1])
        {
            std::cerr << "Sorting failed." << std::endl;
            exit(1);
        }

        lock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(sortingDelay));
    }
}

int main(int argc, char *argv[])
{
    const std::map<std::string, std::function<void(SortState &, int)>>
        sortingAlgorithms{
            {"bubble", bubbleSort},
            {"selection", selectionSort},
            {"insertion", insertionSort},
            {"merge", mergeSort},
        };

    if (!validateInput(argc, argv, sortingAlgorithms))
        return 1;

    const std::string sortType{argv[1]};
    const int n{std::stoi(argv[2])};
    const int sortingDelay{std::stoi(argv[3])};

    int timeElapsed{0};
    int sortTime{0};
    int checkingIndex{-1};
    int prevCheckingIndex{-1};

    // Shared state
    SortState state{
        CountingVector<int>(), // numbers
        std::mutex(),          // mtx
        0,                     // comparisons
        false,                 // sortingComplete
        true                   // running
    };

    for (int i = 0; i < n; i++)
        state.numbers.push_back(i + 1);

    std::shuffle(state.numbers.begin(), state.numbers.end(), std::random_device());

    // Last time each index was accessed
    std::vector<int> lastTimeAccessed(n, 0);

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Sorting Visualization");

    sf::Font font;
    font.loadFromFile("NotoSansMono.ttf");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setPosition(12, 8);

    sf::Clock clock;

    // Sort on a separate thread
    std::thread sortThread(std::ref(sortingAlgorithms.at(sortType)), std::ref(state), sortingDelay);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // Signal thread to stop
                state.running = false;
                sortThread.join();

                window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
            }
        }

        timeElapsed = clock.getElapsedTime().asMilliseconds();

        // Sorting complete
        if (state.sortingComplete && sortTime == 0)
        {
            sortTime = timeElapsed;

            // Begin verification
            std::thread verifyingThread(verify, std::ref(state), std::ref(checkingIndex), sortingDelay);
            verifyingThread.detach();
        }

        window.clear();

        // Draw bars
        drawBars(
            window,
            state,
            timeElapsed,
            sortingDelay,
            checkingIndex,
            prevCheckingIndex,
            lastTimeAccessed);

        // Draw text
        text.setString(std::string(1, toupper(sortType[0])) + sortType.substr(1) + " Sort - " +
                       std::to_string(state.comparisons) + " comparisons, " +
                       std::to_string(state.numbers.getAccessCount()) + " array accesses, " +
                       std::to_string(state.sortingComplete ? sortTime : timeElapsed) + "ms elapsed");
        window.draw(text);

        window.display();
    }

    return 0;
}
