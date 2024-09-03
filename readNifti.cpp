// Copyright 2024 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include <cstring>
#include <iostream>
#include <vector>
// ITK
#include <itkImageFileReader.h>
// ours
#include "attenuation.h"
#include "FieldTypes.h"
#include "readNifti.h"

bool NiftiReader::open(const char *fileName)
{
  auto len = std::strlen(fileName);
  if ((len < 3) || (std::strncmp(fileName + len - 3, "nii", 3) != 0)) {
    return false;
  }

  std::cout << "Reading Nifi file...\n";
  reader = reader_t::New();
  reader->SetFileName(fileName);
  reader->Update();
  img = reader->GetOutput();

  field.dimX = reader->GetImageIO()->GetDimensions(0);
  field.dimY = reader->GetImageIO()->GetDimensions(1);
  field.dimZ = reader->GetImageIO()->GetDimensions(2);
  field.bytesPerCell = sizeof(float);

  return true;
}

const StructuredField& NiftiReader::getField(int index)
{
  if (field.empty()) {

    // transform from ct density to linear attenuation coefficient
    std::cout << "Transforming density values to linear attenuation coefficients\n";
    std::vector<float> attenuation_volume(field.dimX * field.dimY * field.dimZ);
    for (unsigned x=0; x<field.dimX; ++x)
    {
      for (unsigned y=0; y<field.dimX; ++y)
      {
        for (unsigned z=0; z<field.dimZ; ++z)
        {
          const auto index = x + y * field.dimX + z * field.dimX * field.dimY;
          const auto value = img->GetPixel(img_t::IndexType({x, y, z}));
          attenuation_volume[index] = attenuation_lookup(value, tube_potential::TB13000EV);
        }
      }
    }

    size_t size = field.dimX * size_t(field.dimY) * field.dimZ * field.bytesPerCell;
    field.dataF32.resize(size);
    memcpy((char *)field.dataF32.data(), attenuation_volume.data(), size);

    field.dataRange = {0.f, 3.f}; //TODO
  }
  return field;
}
