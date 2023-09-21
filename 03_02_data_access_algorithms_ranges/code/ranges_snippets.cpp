#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>


void AlgorithmVsView() {
    std::cout << "\n\nAlgorithms vs views:" << std::endl;
    const auto doubleFunc = [](auto v) { return 2 * v; };

    const std::vector<int> numbers = { 1, 2, 3 };
    std::vector<int> doubledContainer;
    std::ranges::transform(numbers, std::back_inserter(doubledContainer), doubleFunc);
    const auto doubledView = std::views::transform(numbers, doubleFunc);

    std::cout << "using ranges::transform: ";
    for (auto v : doubledContainer) {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    std::cout << "using transform view: ";
    for (auto v : doubledView) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

void LazyEvaluation() {
    std::cout << "\n\nLazy evaluation:" << std::endl;
    const auto transformFunc = [](auto v) {
        std::cout << "(processing " << v << ") ";
        return 2 * v;
    };
    const std::vector<int> numbers = { 1, 2, 3 };

    {
        std::cout << "using ranges::transform: " << std::endl;

        std::vector<int> doubled;
        std::ranges::transform(numbers, std::back_inserter(doubled), transformFunc);

        for (auto v : doubled) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }

    {
        std::cout << "using views::transform: " << std::endl;

        const auto doubled = std::views::transform(numbers, transformFunc);

        for (auto v : doubled) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
}


void Composition() {
    std::cout << "\n\nComposition:" << std::endl;

    const auto series =
        std::views::iota(1LL)
        | std::views::transform([](auto v) { return v * v; })
        | std::views::transform([](auto v) { return 1.0 / v; })
        | std::views::take(1'000'000);

    double sum = 0.0;
    for (auto v : series) {
        sum += v;
    }

    std::cout << std::sqrt(6 * sum) << std::endl;
}


int main() {
    AlgorithmVsView();
    LazyEvaluation();
    Composition();
}