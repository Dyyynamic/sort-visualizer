#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <random>
#include <iostream>
#include "CountingVector.h"

// Window dimensions
const int WIDTH{1200};
const int HEIGHT{800};
const int N{100};
const int FRAME_RATE{60};
const int SORTING_DELAY{500};

// Shared data
CountingVector<int> numbers{};
CountingVector<int> prevNumbers{};
std::mutex mtx;
bool sortingComplete{false};
int comparisons{0};

int timeElapsed{0};

void bubbleSort()
{
    for (int i = 0; i < numbers.size(); i++)
    {
        for (int j = 0; j < numbers.size() - i - 1; j++)
        {
            // Here we want to enable access counting on the counting vector
            // This requires a mutex lock, but I don't know how to do that

            if (numbers[j] > numbers[j + 1])
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::swap(numbers[j], numbers[j + 1]);

                comparisons++;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(SORTING_DELAY));
        }
    }

    sortingComplete = true;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Sorting Visualization");
    window.setFramerateLimit(FRAME_RATE);

    sf::Font font;
    font.loadFromFile("NotoSansMono.ttf");

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setPosition(12, 8);

    sf::Clock clock;

    for (int i = 0; i < N; i++)
        numbers.push_back(i + 1);

    std::shuffle(numbers.begin(), numbers.end(), std::random_device());

    prevNumbers = numbers;

    // Sort on a separate thread
    std::thread sortThread(bubbleSort);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        if (!sortingComplete) timeElapsed = clock.getElapsedTime().asMilliseconds();

        mtx.lock();

        for (int i = 0; i < numbers.size(); ++i)
        {
            const double barWidth{static_cast<double>(WIDTH) / static_cast<double>(numbers.size())};
            const double barHeight{static_cast<double>(numbers[i] * (HEIGHT - 40)) /
                                    static_cast<double>(numbers.size())};

            sf::RectangleShape rect(sf::Vector2f(barWidth, barHeight));

            if (numbers[i] != prevNumbers[i])
                rect.setFillColor(sf::Color::Red);
                prevNumbers[i] = numbers[i];

                // Here we want to play a tone based on the number being sorted

            rect.setPosition(sf::Vector2f(barWidth * i, HEIGHT - barHeight));

            window.draw(rect);
        }

        text.setString("Bubble Sort - " +
                       std::to_string(comparisons) + " comparisons, " +
                       std::to_string(numbers.getAccessCount()) + " array accesses, " +
                       std::to_string(timeElapsed) + "ms elapsed");
        window.draw(text);

        mtx.unlock();

        window.display();            
    }

    // Wait for the sorting thread to finish
    sortThread.join();

    return 0;
}
