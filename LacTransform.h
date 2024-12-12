#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

struct LacLutEntry
{
    ssize_t density;
    float lac;
};

struct LacLut
{
    std::string name;
    size_t eV;
    std::vector<LacLutEntry> lut;
};

struct LacReader
{
    LacReader();
    LacReader(std::string& fn);

    void setFilename(std::string& filename);
    void read();
    std::vector<std::pair<size_t, std::string>> getNames() const;
    float lookup(ssize_t density) const;
    float lookup(ssize_t density, size_t lacLutId) const;
    size_t getActiveLut() const;
    void setActiveLut(size_t id);

    std::string m_filename;
    std::vector<LacLut> m_lacLuts;
    size_t m_activeLut{0};
};
