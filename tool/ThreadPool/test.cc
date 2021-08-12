#include "ThreadPool.h"
#include <vector>
#include <iostream>
#include <chrono>

int main()
{
    ThreadPool pool(4);
    std::vector<std::future<int>> results;

    for (int i = 0; i < 8; ++i)
    {
        results.emplace_back(
            pool.enqueue([i] {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return i*i;
            })
        );
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
    for (auto && result : results)
    {
        std::cout << result.get() << ' ';
    }
    std::cout << std::endl;
    system("pause");
    return 0;
}
