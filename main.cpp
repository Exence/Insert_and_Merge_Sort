#include <SFML/Graphics.hpp>
#include "windows.h"
#include <iostream>

using namespace sf;

const sf::Color colorArray[8] = { Color::Magenta, Color::Cyan, Color::Green, Color::Yellow,  Color(189, 51, 164), Color(168, 228, 160), Color(115, 204, 120), Color(197, 208, 230) };

struct row
{
    int x;
    int h;
};



int main()
{
    setlocale(LC_ALL, "rus");

    int N;
    int M;
    int sleepTime;
    int index = 0;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
    printf("Программа разбиения числовой последовательности из N элементов на группы по M записей, а затем сортирует.\n\n");
    SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
    
    std::cout << "Введите N: ";
    std::cin >> N;
    std::cout << "Введите M: ";
    std::cin >> M;
    std::cout << "Введите задержку для отображения перестановки элемента при сортировке (в мс): ";
    std::cin >> sleepTime;

    if (sleepTime < 0) { sleepTime = 0; }
    if (sleepTime > 1000) { sleepTime = 1000; }

    std::vector<struct row> rows;

    int parts = N / M;
    int fakeMassSize = M * parts;       // Недостатки целочисленного деления
    int tail = 0;
    int currentPart = 0;
    int offsetX = 1;
    int offsetSortX = 0;
    int offsetSortY = 0;
    int sortedCount = 0;
    float rowWidth;    

    int lenX;
    int lenY;

    int colorTone = 0;

    RenderWindow window(VideoMode(800, 600), "Insert + Merge Sort");
    RectangleShape rectangle;
    Event event;

    bool isCreated = false;
    bool startInsert = false;
    bool swapBegin = false;
    bool sleepOff = false;
    bool oneMorePartChecked = false;
    bool stopIncreaseCurrentPart = false;
    bool isEnded = false;

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if (isCreated && !startInsert && !isEnded)
        {                       
            if ((index + 1) % parts == 0) { index = offsetSortX; }          // Сдвиг текущего индекса, для сортировки в текущей части           

            if (rows[index].h > rows[index + 1].h)                          // Сортировка включением
            {
                if (!swapBegin)
                {
                    offsetSortY = index;                                    // Запоминаем текущий индекс для возврата
                    swapBegin = true;
                }

                std::swap(rows[index].h, rows[index + 1].h);                      
                index -= 2;                            
            }
            else 
            { 
                if (swapBegin)                                              // Возвращаем индекс к неотсортированной части
                {
                    index = offsetSortY - 1;
                    offsetSortY = 0;
                    swapBegin = false;
                }
                else { sortedCount++; }                                                     
            }      

            if (sortedCount == parts)                                       // Увеличение смещения, для сортировки в следующей части
            {
                offsetSortX += parts;
                index = offsetSortX - 1;
                sortedCount = 0;                
            }

            if (offsetSortX >= rows.size())                                 // Все части отсортированы, выходим из массива
            { 
                if (M <= 1) { sortedCount = N * 2; }
                else        { sortedCount = N; }                
            }            
        }
        
        if (startInsert)                                                   // Сортировка слиянием
        {            
            int y = currentPart * parts + parts / 2;                

            if (rows[currentPart*parts+offsetSortX].h >= rows[y + offsetSortY].h)
            {                  
                auto iter = rows.cbegin();
                rows.emplace(iter + currentPart * parts + offsetSortX, rows[y + offsetSortY]);
                iter = rows.cbegin();
                rows.erase(iter + y + offsetSortY + 1);

                lenY--;
                offsetSortX++;
                offsetSortY++;    
                //Sleep(sleepTime);
            }
            else
            {
                offsetSortX++;
                lenX--;
            }

            if (lenX == 0 || lenY == 0)
            {
                if (oneMorePartChecked || (currentPart >= M-1 && fakeMassSize >= N) )         // Если слиты все части
                {
                    sortedCount = N;                   
                }
                else
                {
                    offsetSortX = 0;     
                    currentPart++;
                    index = currentPart * parts - 1;

                    if (currentPart == M && fakeMassSize < N)
                    {
                        offsetSortY = parts - parts / 2;
                        lenY = N - fakeMassSize;
                        lenX = parts;                        
                        stopIncreaseCurrentPart = true;
                        currentPart--;
                    }
                    else
                    {
                        offsetSortY = 0;
                        lenY = parts / 2;
                        lenX = parts - lenY;                                               
                    }
                }

                if (M <= 1 && (fakeMassSize >= N || oneMorePartChecked))            
                {
                    sortedCount = 2 * N;
                    startInsert = false;
                }

                if (stopIncreaseCurrentPart) { oneMorePartChecked = true; }
            }            
        }

        if (!isCreated)                                                                     // Генерация последовательности
        {
            tail = 0;

            if (fakeMassSize != N) { tail = fakeMassSize + M - N; }
            
            for (int i = 0; i < N + tail; ++i)
            {
                struct row row;                
                row.x = i;
                if (i < N)  { row.h = 1 + rand() % 550; }
                else        { row.h = 551; }
                
                rows.push_back(row);
            }

            parts = (N + tail) / M;

            SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
            std::cout << "Заданная последовательность:" << std::endl;
            SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
            
            for (size_t j = 0; j < N; j++) { std::cout << rows[j].h << " "; }

            std::cout << std::endl;                
         
            isCreated = true;     

            if (M == N)                 // Если N == M, то массивы уже отсортированы   
            {
                startInsert == true;
                sortedCount = N;
            }
        }        

        window.clear(Color::Black);

        offsetX = 1;
        rowWidth = (800 - (rows.size() / parts - 1) * 5) / N;

        for (int i = 0; i < N; ++i)
        {
            rectangle.setSize(Vector2f(rowWidth, rows[i].h));
            if (M>1 && i % parts == 0)
            {
                offsetX += 5;
                colorTone++;
                if (colorTone > 7) { colorTone = 0; }                  
            }                
           
            rectangle.setPosition(Vector2f(i * rowWidth + offsetX, 600));            
            rectangle.setRotation(180);                        
            rectangle.setFillColor(colorArray[colorTone]);

            if (rowWidth > 2)
            { 
                rectangle.setOutlineColor(Color::Black); 
                rectangle.setOutlineThickness(0.5);
            }       

            if (i == index + 2 && !startInsert && !isEnded) rectangle.setFillColor(Color::Red);
            if (i == currentPart * parts + offsetSortX && startInsert) rectangle.setFillColor(Color::Red);

            window.draw(rectangle);    
        }

        colorTone = 0;
        window.display();
                
        if (sortedCount < N + N/2) { index++; }
        else
        {
            if (!isEnded)
            {
                index = -10;
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
                printf("\n\t\tОтсортированная последовательность:\n\n");
                SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
                for (size_t i = 0; i < N - 1; i++)
                {
                    std::cout << rows[i].h << " ";
                }
                std::cout << std::endl;
                isEnded = true;
            }            
        }
        
        if (index >= rows.size() - 1)  { index = 0; }
             
        if (sortedCount == N)
        {                   
            SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
            std::cout << "Количество частей: " << rows.size() / parts << std::endl;
            SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
            for (size_t i = 0; i < M; i++)
            {
                SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 5));
                std::cout << "Часть " << i+1 << ":\n";
                SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
                for (size_t j = i * parts; j < (i + 1) * parts; j++)
                {
                    std::cout << rows[j].h << " ";
                }
                std::cout << std::endl;
            }
                        
            M /= 2;
            parts *= 2;
            fakeMassSize = M * parts;            
            startInsert = true;

            Sleep(1000);
            offsetSortX = 0;
            offsetSortY = 0;
            lenY = parts / 2;
            lenX = parts - lenY;
            index = 0;
            sortedCount = 0;
            oneMorePartChecked = false;
            stopIncreaseCurrentPart = false;
        }

        if (!stopIncreaseCurrentPart) { currentPart = index / parts; }      
                
        if (!sleepOff)
        {
            Sleep(1500);
            sleepOff = true;
        }

        Sleep(sleepTime);
    }
    return 0;
}