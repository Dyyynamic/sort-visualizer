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

// Shared state
SortState state = {
    CountingVector<int>(), // numbers
    std::mutex(),          // mtx
    0,                     // comparisons
    false                  // sortingComplete
};

std::vector<int> lastChecked{};

int timeElapsed{0};
int sortTime{0};
int sortingDelay{0};
int checkingIndex{-1};
int prevCheckingIndex{-1};

void verify(SortState &state, int sortingDelay)
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

        std::this_thread::sleep_for(std::chrono::microseconds(sortingDelay));
    }
}

void playTone(int frequency)
{
    const unsigned sampleRate = 44100;
    const double sortingDelaySeconds = sortingDelay / 1000000.0;
    const unsigned numSamples = static_cast<unsigned>(sampleRate * sortingDelaySeconds);

    if (numSamples == 0)
    {
        std::cerr << "Error: numSamples is zero. Check sortingDelay value." << std::endl;
        return;
    }

    std::vector<sf::Int16> samples(numSamples); // Pre-allocate size

    for (unsigned i = 0; i < numSamples; i++)
    {
        const double t = i / static_cast<double>(sampleRate);
        samples[i] = static_cast<sf::Int16>(std::sin(2 * M_PI * frequency * t) * 30000);
    }

    sf::SoundBuffer buffer;
    if (!buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate))
    {
        std::cerr << "Error: Failed to load sound buffer from samples." << std::endl;
        return;
    }

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setVolume(5);
    sound.play();

    std::this_thread::sleep_for(std::chrono::microseconds(sortingDelay * 10));
}

int main(int argc, char *argv[])
{
    std::map<std::string, std::function<void(SortState &, int)>>
        sortingAlgorithms{
            {"bubble", bubbleSort},
            {"selection", selectionSort},
            {"insertion", insertionSort},
        };

    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <sortType> <n> <delay>" << std::endl
                  << std::endl
                  << "Arguments:" << std::endl
                  << "  sortType  - The sorting algorithm to use. Possible values:" << std::endl
                  << "              bubble: Bubble Sort" << std::endl
                  << "              selection: Selection Sort" << std::endl
                  << "              insertion: Insertion Sort" << std::endl
                  << "  n         - The number of elements to sort." << std::endl
                  << "  delay     - Sorting delay in microseconds." << std::endl
                  << std::endl
                  << "Example: " << std::endl
                  << "  " << argv[0] << " bubble 10 60"
                  << std::endl;
        return 1;
    }

    if (sortingAlgorithms.find(argv[1]) == sortingAlgorithms.end())
    {
        std::cerr << "Invalid input for sortType. "
                  << "Please provide one of the following options: 'bubble', 'selection', 'insertion'."
                  << std::endl;
        return 1;
    }

    if (std::isdigit(argv[2][0]) == 0 || std::stoi(argv[2]) < 2)
    {
        std::cerr << "Invalid input for n. Please provide an integer greater than 1."
                  << std::endl;
        return 1;
    }

    if (std::isdigit(argv[3][0]) == 0 || std::stoi(argv[3]) < 1)
    {
        std::cerr << "Invalid input for delay. Please provide an integer greater than 0."
                  << std::endl;
        return 1;
    }

    std::string sortType{argv[1]};
    int n{std::stoi(argv[2])};
    sortingDelay = std::stoi(argv[3]);

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Sorting Visualization");

    sf::Font font;
    font.loadFromFile("NotoSansMono.ttf");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setPosition(12, 8);

    sf::Clock clock;

    for (int i = 0; i < n; i++)
        state.numbers.push_back(i + 1);

    std::shuffle(state.numbers.begin(), state.numbers.end(), std::random_device());

    lastChecked.resize(state.numbers.size(), 0);

    // Sort on a separate thread
    std::thread sortThread(sortingAlgorithms[sortType], std::ref(state), sortingDelay);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        timeElapsed = clock.getElapsedTime().asMilliseconds();

        for (int i = 0; i < state.numbers.size(); ++i)
        {
            state.mtx.lock();

            const double barWidth{static_cast<double>(WIDTH) / state.numbers.size()};
            const double barHeight{static_cast<double>(state.numbers[i] * (HEIGHT - 40)) / state.numbers.size()};

            if (state.numbers.isAccessed(i))
            {
                lastChecked[i] = timeElapsed;
                state.numbers.clearAccessed(i);

                // Play tone when a number changes
                std::thread playToneThread(playTone, 2 * state.numbers[i] * HEIGHT / n);
                playToneThread.detach();
            }

            if (checkingIndex != prevCheckingIndex)
            {
                prevCheckingIndex = checkingIndex;

                // Play tone when checking index changes
                std::thread playToneThread(playTone, 2 * state.numbers[checkingIndex] * HEIGHT / n);
                playToneThread.detach();
            }

            state.mtx.unlock();

            sf::RectangleShape rect(sf::Vector2f(barWidth, barHeight));

            // Light up the bar for LIGHT_DURATION milliseconds
            if (timeElapsed - lastChecked[i] < LIGHT_DURATION || i == checkingIndex)
                rect.setFillColor(sf::Color::Red);

            if (i < checkingIndex)
                rect.setFillColor(sf::Color::Green);

            rect.setPosition(sf::Vector2f(barWidth * i, HEIGHT - barHeight));

            window.draw(rect);
        }

        if (state.sortingComplete && sortTime == 0)
        {
            sortTime = timeElapsed;

            std::thread verifyingThread(verify, std::ref(state), sortingDelay);
            verifyingThread.detach();
        }

        text.setString(std::string(1, toupper(sortType[0])) + sortType.substr(1) + " Sort - " +
                       std::to_string(state.comparisons) + " comparisons, " +
                       std::to_string(state.numbers.getAccessCount()) + " array accesses, " +
                       std::to_string(state.sortingComplete ? sortTime : timeElapsed) + "ms elapsed");
        window.draw(text);

        window.display();
    }

    return 0;
}
