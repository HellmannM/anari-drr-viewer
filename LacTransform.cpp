
// std
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// json
#include <nlohmann/json.hpp>
// ours
#include "LacTransform.h"

LacReader::LacReader()
    : m_filename("LacLuts.json") {};

LacReader::LacReader(std::string &fn)
    : m_filename(fn) {};

void LacReader::setFilename(std::string &filename)
{
  m_filename = filename;
}

void LacReader::read()
{
  std::ifstream json_file(m_filename);
  if (json_file.fail()) {
    std::cerr << "ERROR: Could not open json file: " << m_filename << "\n"
              << std::strerror(errno) << std::endl;
    exit(1);
  }
  try {
    nlohmann::json data = nlohmann::json::parse(json_file);

    for (auto &p : data["lac_luts"]) {
      LacLut lacLut;
      lacLut.name = p["name"];
      lacLut.eV = p["eV"];
      for (auto &e : p["lut"])
        lacLut.lut.emplace_back(e["density"], e["lac"]);
      m_lacLuts.push_back(lacLut);
    }
  } catch (...) {
    std::cerr << "ERROR: Could not parse json file: " << m_filename
              << std::endl;
    exit(1);
  }

  std::cout << "Read LAC LUTs:\n";
  for (auto &lut : m_lacLuts)
    std::cout << lut.name << " (" << lut.eV << "eV)\n";
}

std::vector<std::pair<size_t, std::string>> LacReader::getNames() const
{
  std::vector<std::pair<size_t, std::string>> names;
  size_t id{0};
  for (auto &lut : m_lacLuts)
    names.emplace_back(id++, lut.name);
  return names;
}

float LacReader::lookup(ssize_t density) const
{
  return lookup(density, m_activeLut);
}

float LacReader::lookup(ssize_t density, size_t lacLutId) const
{
  // clamp density to range of lut
  density = std::min(m_lacLuts[lacLutId].lut.back().density,
      std::max(density, m_lacLuts[lacLutId].lut.front().density));

  const auto pos = find_if(m_lacLuts[lacLutId].lut.begin(),
      m_lacLuts[lacLutId].lut.end(),
      [density](LacLutEntry elem) { return elem.density > density; });
  const auto previous = pos - 1;
  return previous->lac
      + (density - previous->density) / (pos->density - previous->density)
      * (pos->lac - previous->lac);
}

size_t LacReader::getActiveLut() const
{
  return m_activeLut;
}

void LacReader::setActiveLut(size_t id)
{
  m_activeLut = id;
}