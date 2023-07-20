#ifndef PAGERANK_H
#define PAGERANK_H

#include <algorithm>
#include <cmath>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

#include "src/Sqlite.h"

#define DAMPING_FACTOR 0.85
#define CONVERGENCE_THRESHOLD 0.0001
#define MAX_ITERATIONS 1000

struct Page {
    std::vector<int> ins;
    int nbOuts;
};

void loadPages(std::vector<Page> &pages) {
    // Load nbOuts for each page
    Sqlite::exec(
        "SELECT fr, COUNT(t) FROM CSRPageRankMatrix GROUP BY fr",
        [](void *data, int argc, char **argv, char **azColName) -> int {
            auto &pages = *(std::vector<Page> *)data;
            int id = atoi(argv[0]) - 1;  // assuming IDs start from 1
            pages[id].nbOuts = atoi(argv[1]);
            return 0;
        },
        &pages);

    // Load the list of pages linking to each page
    Sqlite::exec(
        "SELECT t, fr FROM CSRPageRankMatrix",
        [](void *data, int argc, char **argv, char **azColName) -> int {
            auto &pages = *(std::vector<Page> *)data;
            int id = atoi(argv[0]) - 1;  // assuming IDs start from 1
            pages[id].ins.push_back(atoi(argv[1]) - 1);
            return 0;
        },
        &pages);
}

void bulkUpdate(const std::vector<std::pair<int, double>> &updates) {
    // Create a temporary table and insert the new values
    Sqlite::exec(
        "CREATE TEMPORARY TABLE temp_updates (id INTEGER, pageRank DOUBLE)");
    std::string insertQuery = "INSERT INTO temp_updates (id, pageRank) VALUES ";
    for (const auto &update : updates) {
        insertQuery += "(" + std::to_string(update.first) + ", " +
                       std::to_string(update.second) + "), ";
    }
    insertQuery.pop_back();  // Remove trailing comma and space
    insertQuery.pop_back();
    Sqlite::exec(insertQuery.c_str());

    // Perform the update join
    Sqlite::exec(
        "UPDATE files "
        "SET pageRank = (SELECT pageRank FROM temp_updates WHERE "
        "temp_updates.id = files.id) "
        "WHERE id IN (SELECT id FROM temp_updates)");

    // Drop the temporary table
    Sqlite::exec("DROP TABLE temp_updates");
}

void runPageRank() {
    Sqlite::beginTransaction();
    double N = 0;
    Sqlite::exec(
        "SELECT COUNT(id) FROM files",
        [](void *data, int argc, char **argv, char **azColName) -> int {
            auto &N = *(double *)data;
            N = std::stod(argv[0]);
            return 0;
        },
        &N);
    std::vector<double> scores(2 * N, 1.0 / N);
    std::vector<Page> pages(N);
    loadPages(pages);

    int src = 0;
    int dest = N;
    for (int it = 0; it < MAX_ITERATIONS; ++it) {
        double max_diff = 0;
        std::vector<std::future<void>> futures;
        for (int j = 0; j < N; ++j) {
            futures.push_back(std::async(std::launch::async, [&, j]() {
                double sum = 0;
                for (int k = 0; k < pages[j].ins.size(); ++k) {
                    sum += scores[src + pages[j].ins[k]] /
                           pages[pages[j].ins[k]].nbOuts;
                }
                double pr = (1 - DAMPING_FACTOR) / N + DAMPING_FACTOR * sum;
                scores[dest + j] = pr;
                max_diff = std::max(max_diff, std::abs(pr - scores[src + j]));
            }));
        }

        for (auto &f : futures) {
            f.get();
        }

        std::cout << "Iteration " << it << " diff: " << max_diff << std::endl;
        if (max_diff < CONVERGENCE_THRESHOLD) break;

        std::swap(src, dest);
    }

    std::vector<std::pair<int, double>> updates;
    for (int i = 0; i < N; ++i) {
        updates.emplace_back(i + 1, scores[src + i]);
    }

    // TODO: Bulk update the PageRank in the database with the values in updates
    // here.
    bulkUpdate(updates);

    Sqlite::commitTransaction();
}

#endif
