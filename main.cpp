#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <string>
#include "CountingVector.h"
#include "SortState.h"
#include "sorts.h"

// Window dimensions
const int WIDTH{1200};
const int HEIGHT{800};

void playTone(sf::Sound &sound, int frequency, int frameRate)
{
    const unsigned sampleRate = 44100;
    const double duration = 1 / static_cast<double>(frameRate);
    const unsigned numSamples = sampleRate * duration;
    sf::Int16 *samples = new sf::Int16[numSamples];

    for (unsigned i = 0; i < numSamples; ++i)
    {
        double time = i / static_cast<double>(sampleRate);
        samples[i] = 32767 * sin(2 * M_PI * frequency * time);
    }

    sf::SoundBuffer buffer;
    buffer.loadFromSamples(samples, numSamples, 1, sampleRate);

    sound.setBuffer(buffer);
    sound.play();

    sf::sleep(sf::seconds(duration));

    delete[] samples;
}

void initState(SortState &state, std::string sortType, int n)
{
    // Generate numbers from 1 to n
    for (int i = 1; i <= n; i++)
        state.numbers.push_back(i);

    // Shuffle numbers
    std::shuffle(state.numbers.begin(), state.numbers.end(), std::random_device());

    state.numbers.setAccessCount(0);

    state.cursorPos = 0;

    state.sorted = false;
    state.verified = false;

    state.comparisons = 0;
    state.checkingPos = 0;

    if (sortType == "bubble")
        state.sortedIndex = n - 1;
    else if (sortType == "selection")
        state.sortedIndex = 0;
    else if (sortType == "insertion")
        state.sortedIndex = -1;

    state.currentMinIndex = 0;
}

void drawBars(sf::RenderWindow &window, SortState &state, int n)
{
    for (int i = 0; i < n; i++)
    {
        const double barWidth{static_cast<double>(WIDTH) / static_cast<double>(n)};
        const double barHeight{static_cast<double>(state.numbers[i] * (HEIGHT - 40)) /
                                static_cast<double>(n)};

        sf::RectangleShape rect(sf::Vector2f(barWidth, barHeight));
        rect.setPosition(sf::Vector2f(barWidth * i, HEIGHT - barHeight));

        if (i < state.checkingPos)
            rect.setFillColor(sf::Color::Green);

        if (i == state.cursorPos)
            rect.setFillColor(sf::Color::Red);

        window.draw(rect);
    }
}

int main(int argc, char *argv[])
{
    std::vector<std::string> sortingAlgorithms{"bubble", "selection", "insertion"};

    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <sortType> <n> <frameRate>" << std::endl
                  << std::endl
                  << "Arguments:" << std::endl
                  << "  sortType  - The sorting algorithm to use. Possible values:" << std::endl
                  << "              bubble: Bubble Sort" << std::endl
                  << "              selection: Selection Sort" << std::endl
                  << "              insertion: Insertion Sort" << std::endl
                  << "  n         - The number of elements to sort." << std::endl
                  << "  frameRate - The number of frames per second." << std::endl
                  << std::endl
                  << "Example: " << std::endl
                  << "  " << argv[0] << " bubble 10 60"
                  << std::endl;
        return 1;
    }

    auto it = std::find(sortingAlgorithms.begin(), sortingAlgorithms.end(), argv[1]);

    if (it == sortingAlgorithms.end())
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
        std::cerr << "Invalid input for frameRate. Please provide an integer greater than 0."
                  << std::endl;
        return 1;
    }

    // Command line arguments
    const std::string sortType{argv[1]};
    const int n{std::stoi(argv[2])};
    const int frameRate{std::stoi(argv[3])};

    int time{0};

    SortState state{};
    initState(state, sortType, n);

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!");
    window.setFramerateLimit(frameRate);


    sf::Font font;
    font.loadFromFile("NotoSansMono.ttf");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setPosition(12, 8);

    sf::Sound sound;
    sound.setVolume(20);

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Count comparisons when sorting
        state.numbers.setAccessCounting(true);

        if (!state.sorted)
        {
            // Sort
            if (sortType == "bubble")
                state.sorted = bubbleSort(state);
            else if (sortType == "selection")
                state.sorted = selectionSort(state);
            else if (sortType == "insertion")
                state.sorted = insertionSort(state);
        }
        else if (!state.verified)
        {
            // Verify sort
            state.cursorPos = state.checkingPos;
            state.checkingPos++;

            if (state.cursorPos == n - 1)
                state.verified = true;
        }

        // Disable counting
        state.numbers.setAccessCounting(false);

        window.clear();

        if (!state.sorted) time = clock.getElapsedTime().asMilliseconds();

        text.setString(std::string(1, toupper(sortType[0])) + sortType.substr(1) + " Sort - " +
                       std::to_string(state.comparisons) + " comparisons, " +
                       std::to_string(state.numbers.getAccessCount()) + " array accesses, " +
                       std::to_string(time) + "ms elapsed");
        window.draw(text);

        drawBars(window, state, n);

        window.display();

        if (!state.verified)
            playTone(sound, 2 * state.numbers[state.cursorPos] * HEIGHT / n, frameRate);
    }

    return 0;
}
