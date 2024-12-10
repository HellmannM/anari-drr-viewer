// Copyright 2024 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include <cstring>
#include <iostream>
#include <vector>
// ITK
#include <itkImageFileReader.h>
#include "itkImageRegionIterator.h"
// ours
#include "FieldTypes.h"
#include "readNifti.h"

bool NiftiReader::open(const char *fileName)
{
  auto len = std::strlen(fileName);
  if ((len < 3) || (std::strncmp(fileName + len - 3, "nii", 3) != 0)) {
    return false;
  }

  std::cout << "Reading Nifi file... ";
  reader = reader_t::New();
  reader->SetFileName(fileName);
  reader->Update();
  img = reader->GetOutput();

  field.dimX = reader->GetImageIO()->GetDimensions(0);
  field.dimY = reader->GetImageIO()->GetDimensions(1);
  field.dimZ = reader->GetImageIO()->GetDimensions(2);
  field.bytesPerCell = sizeof(float);

  lacField.dimX = reader->GetImageIO()->GetDimensions(0);
  lacField.dimY = reader->GetImageIO()->GetDimensions(1);
  lacField.dimZ = reader->GetImageIO()->GetDimensions(2);
  lacField.bytesPerCell = sizeof(float);

  std::cout << "[" << field.dimX << ", " << field.dimY << ", " << field.dimZ << "]\n";

  return true;
}

const StructuredField& NiftiReader::getField(int index, LacReader& lacReader)
{
  std::cout << "Transform density values to linear attenuation coefficients\n";
  std::cout << "\t Using " << lacReader.m_lacLuts[lacReader.m_activeLut].name << "\n";
  using voxel_value_type = int16_t; //TODO
  std::vector<voxel_value_type> buffer;
  itk::ImageRegionConstIterator<img_t> inputIterator(img, img->GetLargestPossibleRegion());
  while (!inputIterator.IsAtEnd())
  {
      buffer.emplace_back(inputIterator.Get());
      ++inputIterator;
  }
  std::vector<float> attenuation_volume((size_t)field.dimX * field.dimY * field.dimZ);
  for (size_t i=0; i<buffer.size(); ++i) {
    attenuation_volume[i] = lacReader.lookup(buffer[i]);
  }
  size_t size = attenuation_volume.size() * sizeof(attenuation_volume[0]);
  lacField.dataF32.resize(size);
  memcpy((char *)lacField.dataF32.data(), attenuation_volume.data(), size);

  lacField.dataRange = {0.f, 3.f}; //TODO
  return lacField;
}
