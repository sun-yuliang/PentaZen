/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen pattern generator, by Sun Yuliang.
 */

#include "line.h"

#include <iostream>

// Global material table
int MatTable[MAT_TABLE_SIZE][MATERIAL_NUM];

namespace {

// Output the whole table in c header file format
void output_to_c_header_file() {
    Line line;

    std::string table_name = TARGET_RULE == FREESTYLE ? "Pattern_f" : "Pattern_s";

    std::cout << "#include <stdint.h>" << std::endl
              << std::endl;
    std::cout << "static uint32_t " << table_name << "[" << MAT_TABLE_SIZE << "][" << MAX_LINE_LEN + 1 << "] = { ";

    for (auto i = 0; i <= MAX_LINE_LEN; ++i)
        for (auto j = 0; j != (1 << i); ++j) {
            line.set(j, i);
            std::cout << (i != 0 ? ",\n" : "\n") << "    { " << merged_material_info(line);

            for (auto i = line.size() - 1; i >= 0; --i)
                std::cout << ", " << merged_position_info(line, i);

            std::cout << " }";
        }

    std::cout << "\n};" << std::endl;
}

} // namespace

int main() {
    generate(TARGET_RULE);

    output_to_c_header_file();

    // Line line;
    // for (auto i = 0; i != (1 << MAX_LINE_LEN); ++i) {
    //     line.set(i, MAX_LINE_LEN);
    //     std::cout << line << " ";
    //     for (auto m = 0; m != MATERIAL_NUM; ++m)
    //         std::cout << MatTable[line.index()][m] << " ";
    //     std::cout << std::endl;
    // }

    return 0;
}
