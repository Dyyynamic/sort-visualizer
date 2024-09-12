#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <string>
#include "CountingVector.h"

// Window dimensions
const int WIDTH{1200};
const int HEIGHT{800};

struct SortState
{
    CountingVector<int> &numbers;
    int &cursorPos;
    bool &sorted;
    bool &verified;
    int &comparisons;
    int &sortedIndex;
    int &checkingPos;
    int &currentMinIndex;
};

bool bubbleSort(CountingVector<int> &numbers, int &cursorPos, int &comparisons, int &sortedIndex)
{
    if (numbers[cursorPos] > numbers[cursorPos + 1])
        std::swap(numbers[cursorPos], numbers[cursorPos + 1]);

    comparisons++;
    cursorPos++;

    if (cursorPos == sortedIndex)
    {
        // Finished sorting
        if (cursorPos == 1)
            return true;

        cursorPos = 0;
        sortedIndex -= 1;
    }

    return false;
}

bool selectionSort(
    CountingVector<int> &numbers,
    int &cursorPos,
    int &comparisons,
    int &sortedIndex,
    int &currentMinIndex)
{

    if (numbers[cursorPos] < numbers[currentMinIndex])
        currentMinIndex = cursorPos;

    comparisons++;
    cursorPos++;

    if (cursorPos == numbers.size())
    {
        std::swap(numbers[sortedIndex], numbers[currentMinIndex]);
        sortedIndex++;
        cursorPos = sortedIndex;
        currentMinIndex = sortedIndex;

        if (sortedIndex == numbers.size() - 1)
        {
            return true;
        }
    }

    return false;
}

bool insertionSort(CountingVector<int> &numbers, int &cursorPos, int &comparisons, int &sortedIndex)
{
    if (numbers[cursorPos] < numbers[cursorPos - 1])
    {
        std::swap(numbers[cursorPos], numbers[cursorPos - 1]);
        cursorPos--;
    }
    else
    {
        sortedIndex++;

        // Avoid moving cursor off the end
        if (sortedIndex < numbers.size() - 1)
            cursorPos = sortedIndex + 1;
    }

    comparisons++;

    return sortedIndex == numbers.size() - 1;
}

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

// This function is kinda unnecessary, but it may be used in the future
// to reset after a sort before starting a new one
void init(SortState state, int n, std::string sortType)
{
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

std::string capitalize(std::string str)
{
    str[0] = toupper(str[0]);

    return str;
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

    // Generate numbers from 1 to n
    CountingVector<int> numbers;
    for (int i = 1; i <= n; i++)
        numbers.push_back(i);

    int cursorPos;

    bool sorted;   // Has the sort finished
    bool verified; // Has the sort been verified

    int comparisons; // Number of comparisons done
    int sortedIndex; // The index of the sorted partition
    int checkingPos; // Current position when verifying the sort

    int currentMinIndex; // For selection sort

    int time{0};

    SortState state{
        numbers,
        cursorPos,
        sorted,
        verified,
        comparisons,
        sortedIndex,
        checkingPos,
        currentMinIndex};

    init(state, n, sortType);

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
        numbers.setAccessCounting(true);

        if (!sorted)
        {
            if (sortType == "bubble")
                sorted = bubbleSort(numbers, cursorPos, comparisons, sortedIndex);
            else if (sortType == "selection")
                sorted = selectionSort(numbers, cursorPos, comparisons, sortedIndex, currentMinIndex);
            else if (sortType == "insertion")
                sorted = insertionSort(numbers, cursorPos, comparisons, sortedIndex);
        }
        else if (!verified)
        {
            cursorPos = checkingPos;
            checkingPos++;

            if (cursorPos == n - 1)
                verified = true;
        }

        // Disable counting
        numbers.setAccessCounting(false);

        window.clear();

        if (!sorted) time = clock.getElapsedTime().asMilliseconds();

        text.setString(capitalize(sortType) + " Sort - " +
                       std::to_string(comparisons) + " comparisons, " +
                       std::to_string(numbers.getAccessCount()) + " array accesses, " +
                       std::to_string(time) + "ms elapsed");
        window.draw(text);

        for (int i = 0; i < n; i++)
        {
            const double barWidth{static_cast<double>(WIDTH) / static_cast<double>(n)};
            const double barHeight{static_cast<double>(numbers[i] * (HEIGHT - 40)) /
                                   static_cast<double>(n)};

            sf::RectangleShape rect(sf::Vector2f(barWidth, barHeight));
            rect.setPosition(sf::Vector2f(barWidth * i, HEIGHT - barHeight));

            if (i < checkingPos)
                rect.setFillColor(sf::Color::Green);

            if (i == cursorPos)
                rect.setFillColor(sf::Color::Red);

            window.draw(rect);
        }

        window.display();

        if (!verified)
            playTone(sound, 2 * numbers[cursorPos] * HEIGHT / n, frameRate);
    }

    return 0;
}
